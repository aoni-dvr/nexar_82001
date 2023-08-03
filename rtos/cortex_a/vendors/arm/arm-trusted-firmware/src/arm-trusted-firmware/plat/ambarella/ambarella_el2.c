/*
 * Copyright (c) 2013-2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <errno.h>
#include <common/runtime_svc.h>
#include <plat/common/platform.h>
#include <libfdt.h>
#include <lib/xlat_tables/xlat_tables_v2.h>
#include <lib/extensions/ras_arch.h>
#include <plat_private.h>
#include <ambarella_smc.h>

extern uint64_t el2_pgtable_base;
extern uint64_t el2_stack_base;
extern uint64_t el2_vector_base;

/*****************************************************************************/

#define EL2_PGTABLE_OFFSET		(0)

#if defined(AMBARELLA_CV5)
#define EL2_VECTOR_OFFSET		(FOUR_MB - (TWO_MB / 2))
#define EL2_STACK_OFFSET		(FOUR_MB - PAGE_SIZE)
#else
#define EL2_VECTOR_OFFSET		(TWO_MB / 2)
#define EL2_STACK_OFFSET		(TWO_MB - PAGE_SIZE)
#endif

#define FOUR_KB				(1ULL << FOUR_KB_SHIFT)
#define TWO_MB				(1ULL << TWO_MB_SHIFT)
#define FOUR_MB				(2ULL << TWO_MB_SHIFT)
#define ONE_GB				(1ULL << ONE_GB_SHIFT)
#define FOUR_GB				(4ULL << ONE_GB_SHIFT)
#define M64_GB				(64ULL << ONE_GB_SHIFT)
#define M128_GB				(128ULL << ONE_GB_SHIFT)
#define M256_GB				(256ULL << ONE_GB_SHIFT)

#define MEMORY_PROPERTIES		((0ULL << 54) | 0x7fc)
#define DEVICE_PROPERTIES		((1ULL << 54) | 0x6c4)

#define REG_ALLOW_READ			(1 << 0)
#define REG_ALLOW_WRITE			(1 << 1) /* currently not used */
#define REG_DBG_VERBOSE			(1 << 7)

static uint64_t el2_pgtable_level1;
static uint64_t el2_pgtable_level2;
static uint64_t el2_pgtable_level3;

struct register_permission_t {
	uint64_t start;
	uint32_t size;
	uint32_t flag;
};

/* The registers not defined in reg_permission[] are always accessible */
#define REG_PERM_MAX_NUM		(PAGE_SIZE / sizeof(struct register_permission_t))
static struct register_permission_t	reg_permission[REG_PERM_MAX_NUM];
static uint32_t reg_perm_num = 0;

/*****************************************************************************/
static uint64_t calc_dram_size(void)
{
	uint32_t config = mmio_read_32(DDRC_REG(DDRC_C0_OFFSET + 0x04));
	uint64_t size, max_size = DRAM_SPACE_MAX_SIZE;

#if defined(AMBARELLA_CV5)
	if (mmio_read_32(DDRC_REG(0x00)) & BIT(2))
		config = mmio_read_32(DDRC_REG(DDRC_C1_OFFSET + 0x04));

	switch ((config >> 2) & 0xF) {
	case 0:
		size = 0x10000000; /* 2Gb */
		break;
	case 1:
		size = 0x18000000; /* 3Gb */
		break;
	case 2:
		size = 0x20000000; /* 4Gb */
		break;
	case 3:
		size = 0x30000000; /* 6Gb */
		break;
	case 4:
		size = 0x40000000; /* 8Gb */
		break;
	case 5:
		size = 0x60000000; /* 12Gb */
		break;
	case 6:
		size = 0x80000000; /* 16Gb */
		break;
	case 7:
		size = 0xC0000000; /* 24Gb */
		break;
	case 8:
		size = 0x100000000; /* 32Gb */
		break;
	default:
		ERROR("Unexpected DRAM CONFIG.\n");
		panic();
		break;
	}

	if (config & BIT(26))
		size *= 2;
	if ((config & BIT(6)) == 0)
		size *= 2;

	if ((mmio_read_32(DDRC_REG(0x00)) & 0x6) == 0x6)
		size *= 2;

	size = MIN(max_size, size);
#else
	switch ((config >> 5) & 0x7) {
	case 3:
		size = 0x10000000;
		break;
	case 4:
		size = 0x20000000;
		break;
	case 5:
		size = 0x40000000;
		break;
	case 6:
		size = 0x80000000;
		break;
	default:
		ERROR("Unexpected DRAM CONFIG.\n");
		panic();
		break;
	}

	if (mmio_read_32(DDRC_REG(0x00)) & 0x1)
		size = MIN(max_size, size * 2);
#endif
	return size;
}

uint64_t dram_vpn_page_size(void)
{
	uint64_t pagesz;

#if defined(AMBARELLA_CV5)
	uint64_t dramsz = calc_dram_size();
	pagesz = (uint64_t)(dramsz >> 14); /* based on Memory Size (DDR size / 16K) */
#else
	pagesz = (uint64_t)(1 << 18); /* fixed: 256KB */
#endif
	return pagesz;
}

/* Invalidate the translation table of the region */
static void ambarella_el2_pgtable_update(uint64_t start, uint64_t len)
{
	uint64_t _start, _end, i, j, *pgtable, index;

	assert(start >= DEVICE_BASE && IS_PAGE_ALIGNED(start));

	_start = round_down(start, TWO_MB);
	_end = round_up(start + len, TWO_MB);

	/* Replace corresponding 2MB entry with a level 3 table */
	for (i = _start; i < _end; i += TWO_MB) {
		pgtable = (uint64_t *)el2_pgtable_level2;
		index = TWO_MB_INDEX(i);

		/*
		 * Just invalid the corresponding entry in level 3 table if
		 * the level 3 table has been already created before.
		 */
		if ((pgtable[index] & DESC_MASK) == TABLE_DESC) {
			pgtable = (uint64_t *)(pgtable[index] & ~DESC_MASK);

			for (j = start; j < round_up(start + len, FOUR_KB); j += FOUR_KB) {
				index = FOUR_KB_INDEX(j) % 512;
				pgtable[index] = 0;
			}
			continue;
		}

		pgtable[index] = el2_pgtable_level3 | TABLE_DESC;

		/* Create the level 3 table */
		pgtable = (uint64_t *)el2_pgtable_level3;
		for (j = i; j < i + TWO_MB; j += FOUR_KB) {
			index = FOUR_KB_INDEX(j) % 512;
			if (j >= start && j < round_up(start + len, FOUR_KB))
				pgtable[index] = 0;
			else
				pgtable[index] = j | DEVICE_PROPERTIES | PAGE_DESC;
		}

		el2_pgtable_level3 += PAGE_SIZE;
		assert(el2_pgtable_level3 < el2_vector_base);
	}
}

/*
 * get base address for node in case #address-cells > 1
 * by parsing #address-cells and ranges;
*/
uint64_t ambarella_node_addr_base(void *fdt, int32_t offset)
{
	const fdt32_t *ranges, *p_addr_cells, *p_parent_addr_cells;
	int32_t ranges_len, i, parent_offset;
	uint32_t addr_cells, parent_addr_cells;
	uint64_t base_addr = 0, child_addr = 0, parent_addr = 0;

	parent_offset = fdt_parent_offset(fdt, offset);
	if (parent_offset < 0)
		return base_addr;

	ranges = fdt_getprop(fdt, parent_offset, "ranges", &ranges_len);
	if (!ranges || !ranges_len)
		return base_addr;

	p_addr_cells = fdt_getprop(fdt, parent_offset, "#address-cells", NULL);
	addr_cells = fdt32_to_cpu(p_addr_cells[0]);
	parent_offset = fdt_parent_offset(fdt, parent_offset);
	if (parent_offset < 0)
		return base_addr;

	p_parent_addr_cells = fdt_getprop(fdt, parent_offset, "#address-cells", NULL);
	parent_addr_cells = fdt32_to_cpu(p_parent_addr_cells[0]);
	/* sub addr in ranges */
	for (i = 0; i < addr_cells; i++, ranges++)
		child_addr = (child_addr << (i * 32)) | fdt32_to_cpu(*ranges);
	/* parent's addr in ranges */
	for (i = 0; i < parent_addr_cells; i++, ranges++)
		parent_addr = (parent_addr << (i * 32)) | fdt32_to_cpu(*ranges);

	base_addr = parent_addr - child_addr;
	INFO("base_addr = 0x%llx \n", base_addr);

	return base_addr;
}

static int32_t ambarella_fdt_parse_gpio_security(void *fdt, int32_t offset)
{
	const char *pinctrl = "ambarella,pinctrl";
	const fdt32_t *gpio, *reg, *ctrl_bit;
	int32_t i, lenp, pinctrl_reg_idx, cell;
	uint32_t gpio_security_mask[GPIO_BANK] = {0};

	gpio = fdt_getprop(fdt, offset, "gpio", &lenp);
	if (gpio == NULL || lenp == 0)
		return 0;

	for (i = 0; i < lenp / 4U; i++, gpio++) {
		int32_t sec_gpio = fdt32_to_cpu(*gpio);
		int32_t bank = sec_gpio / NRGPIO_PER_BANK;
		int32_t bitoff = sec_gpio % NRGPIO_PER_BANK;

		gpio_security_mask[bank] |= (1 << bitoff);
	}

	offset = fdt_node_offset_by_compatible(fdt, -1, pinctrl);
	if (offset < 0)
		return offset;

	pinctrl_reg_idx = fdt_stringlist_search(fdt, offset, "reg-names", "iomux");
	cell = pinctrl_reg_idx;

	ctrl_bit = fdt_getprop(fdt, offset, "amb,secure-ctrl-bit", &lenp);
	if (ctrl_bit == NULL || lenp == 0) {
		ERROR("'%s': Secure control bit is not specified\n", pinctrl);
		return 0;
	}

	reg = fdt_getprop(fdt, offset, "reg", &lenp);
	if (reg == NULL || lenp == 0)
		return 0;
	else {
		uintptr_t pinctrl_reg[2];
		uintptr_t base_addr;

		base_addr = ambarella_node_addr_base(fdt, offset);
		pinctrl_reg[0] = fdt32_to_cpu(reg[2 * pinctrl_reg_idx]);
		pinctrl_reg[0] |= base_addr;
		pinctrl_reg[1] = fdt32_to_cpu(reg[2 * pinctrl_reg_idx + 1]);
		/* Enable IOMUX Security */
		ambarella_device_security_setup(fdt32_to_cpu(ctrl_bit[pinctrl_reg_idx]));
		ambarella_el2_pgtable_update(pinctrl_reg[0], pinctrl_reg[1]);
		ambarella_pinctrl_security_request(pinctrl_reg, gpio_security_mask);

			/* Enable GPIO BANK Security */
			for (i = 0; i < cell; i++, ctrl_bit++) {

				uintptr_t gpio_reg[2];

				if (!gpio_security_mask[i])
					continue;

				gpio_reg[0] = fdt32_to_cpu(reg[2 * i]);
				gpio_reg[0] |= base_addr;
				gpio_reg[1] = fdt32_to_cpu(reg[2 * i + 1]);

				ambarella_device_security_setup(fdt32_to_cpu(*ctrl_bit));
				ambarella_el2_pgtable_update(gpio_reg[0], gpio_reg[1]);
				ambarella_gpio_security_request(gpio_reg, i);
			}
	}

	return 0;
}

static int32_t ambarella_fdt_parse_device_security(void *fdt, uint32_t offset)
{
	const fdt32_t *phandle, *ctrl_bit, *reg, *reg_ro, *reg_na;
	int32_t lenp, sub_lenp, i, j;
	const char *device_name;
	uintptr_t base_addr;

	phandle = fdt_getprop(fdt, offset, "device", &lenp);
	if (phandle == NULL)
		return 0;

	for (i = 0; i < lenp / 4U; i++, phandle++) {
		offset = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*phandle));
		if (offset < 0)
			return offset;

		device_name = fdt_getprop(fdt, offset, "compatible", NULL);
		if (device_name == NULL)
			continue;

		/* base-addr for this node(phandle point to) */
		base_addr = ambarella_node_addr_base(fdt, offset);
		/* If there is no control bit property, treat the device as NON-Secure. */
		ctrl_bit = fdt_getprop(fdt, offset, "amb,secure-ctrl-bit", NULL);
		if (ctrl_bit == NULL) {
			ERROR("'%s': Secure control bit is not specified\n", device_name);
			continue;
		}

		INFO("'%s': Secure device[%d].\n", device_name, fdt32_to_cpu(*ctrl_bit));

		/* Enable device Security */
		ambarella_device_security_setup(fdt32_to_cpu(*ctrl_bit));

		reg = fdt_getprop(fdt, offset, "reg", &sub_lenp);
		if (reg == NULL)
			continue;
		assert((sub_lenp / 4U) % 2 == 0);

		reg_ro = fdt_getprop(fdt, offset, "amb,secure-reg-ro", &sub_lenp);
		if (reg_ro != NULL) {
			assert((sub_lenp / 4U) % 2 == 0);

			for (j = 0; j < sub_lenp / 4U; j += 2) {
				reg_permission[reg_perm_num].start = fdt32_to_cpu(reg_ro[j]);
				reg_permission[reg_perm_num].start |= base_addr;
				reg_permission[reg_perm_num].size = fdt32_to_cpu(reg_ro[j+1]);
				reg_permission[reg_perm_num].flag = REG_ALLOW_READ;

				INFO("'%s': readonly reg: [0x%llx, 0x%x]\n", device_name,
					reg_permission[reg_perm_num].start,
					reg_permission[reg_perm_num].size);

				reg_perm_num++;
				assert(reg_perm_num < REG_PERM_MAX_NUM);
			}
		}

		reg_na = fdt_getprop(fdt, offset, "amb,secure-reg-na", &sub_lenp);
		if (reg_na != NULL) {
			assert((sub_lenp / 4U) % 2 == 0);

			for (j = 0; j < sub_lenp / 4U; j += 2) {
				reg_permission[reg_perm_num].start = fdt32_to_cpu(reg_na[j]);
				reg_permission[reg_perm_num].start |= base_addr;
				reg_permission[reg_perm_num].size = fdt32_to_cpu(reg_na[j+1]);
				reg_permission[reg_perm_num].flag = 0;

				INFO("'%s': noaccess reg: [0x%llx, 0x%x]\n", device_name,
					reg_permission[reg_perm_num].start,
					reg_permission[reg_perm_num].size);

				reg_perm_num++;
				assert(reg_perm_num < REG_PERM_MAX_NUM);
			}
		}

		if (reg_ro == NULL && reg_na == NULL)
			continue;

		/* Update the stage2 translation table reg[0] */
		ambarella_el2_pgtable_update((fdt32_to_cpu(reg[0]) | base_addr), fdt32_to_cpu(reg[1]));
	}

	/*
	 * Monitor DDRC registers, and set it as read-only.
	 * Non-secure world BLD needs READ permission to access these registers.
	 */
	reg_permission[reg_perm_num].start = DDRC_BASE;
	reg_permission[reg_perm_num].size = DDRC_SIZE;
	reg_permission[reg_perm_num].flag = REG_ALLOW_READ;
	reg_perm_num++;

	return 0;
}

static int32_t ambarella_fdt_parse_security(void)
{
	void *fdt = (void *)(uintptr_t)(boot_cookie_ptr()->dtb_ram_start);
	int32_t offset;

	/*
	 * If secure boot is disabled, reg_permission[] will be empty,
	 * it means all registers are accessible.
	 */
	if (!ambarella_is_secure_boot())
		return 0;

	offset = fdt_path_offset(fdt, "/secure-monitor");
	if (offset < 0)
		return offset;

	ambarella_fdt_parse_gpio_security(fdt, offset);

	ambarella_fdt_parse_device_security(fdt, offset);

	return 0;
}

/*
 * Setup stage2 translation, and relocate EL2 exception vector
 */
void ambarella_el2_pgtable_setup(void)
{
	uint64_t el2_base, el2_limit, start, *pgtable, dramsz;
	int32_t index, rval;

	if (!ambarella_el2_is_used())
		return;

	dramsz = calc_dram_size();

	ambarella_el2_rsvd_mem(&el2_base, &el2_limit);

	rval = mmap_add_dynamic_region(el2_base, el2_base, el2_limit - el2_base,
					MT_MEMORY | MT_RW | MT_NS);
	assert(rval == 0);

	el2_pgtable_base = el2_base + EL2_PGTABLE_OFFSET;
	el2_vector_base = el2_base + EL2_VECTOR_OFFSET;
	el2_stack_base = el2_base + EL2_STACK_OFFSET;

#if defined(AMBARELLA_CV5)
	/*
	 *  + ------------ + ---> Pgtable
	 *  |              |
	 *  |     3MB      |
	 *  |              |
	 *  + ------------ + ---> Vector
	 *  |              |
	 *  |  1MB - 4KB   |
	 *  |              |
	 *  + ------------ + ---> Stack
	 *  |     4KB      |
	 *  + ------------ + ---> END
	 */

	el2_pgtable_level1 = el2_pgtable_base;
	el2_pgtable_level2 = el2_pgtable_level1 + PAGE_SIZE;
	el2_pgtable_level3 = el2_pgtable_level2 + PAGE_SIZE * (128 + 4);

	/* clear all table entry */
	pgtable = (uint64_t *)el2_pgtable_level1;
	memset(pgtable, 0, PAGE_SIZE);

	/* normal ram */
	for (start = 0; start < M64_GB; start += ONE_GB) {
		index = ONE_GB_INDEX(start);
		pgtable[index] = (el2_pgtable_level2 + PAGE_SIZE * index) | MEMORY_PROPERTIES | TABLE_DESC;
	}
	/* DDRC */
	for (start = M64_GB; start < M128_GB; start += ONE_GB) {
		index = ONE_GB_INDEX(start);
		pgtable[index] = (el2_pgtable_level2 + PAGE_SIZE * index) | DEVICE_PROPERTIES | TABLE_DESC;
	}
	/* normal device */
	for (start = M128_GB; start < (M128_GB + FOUR_GB - ONE_GB); start += ONE_GB) {
		index = ONE_GB_INDEX(start);
		pgtable[index] = start | DEVICE_PROPERTIES | BLOCK_DESC;
	}

	/* NOTE: CV5 security device locate in last 1GB of device memroy[may change on the other platform]
	 * let ambarella_el2_pagetable_update to insert L3 table, don't insert level3 table here or you will
	 * step on the memory el2_pgtable_level3 and corrupt el2_pgtable_level3.
	 * Take Care: ambarella_el2_pagetable_update not use the 1:1 offset of the index where
	 * mmu table level1 or level2 does.
	 * VA=39, 9(PGD):9(PMD):9(PTE):12(offset). VA =64 - MMU.TxSZ
	*/
	for (start = (M128_GB + FOUR_GB - ONE_GB); start < (M128_GB + FOUR_GB); start += ONE_GB) {
		index = ONE_GB_INDEX(start);
		pgtable[index] = (el2_pgtable_level2 + PAGE_SIZE * index) | DEVICE_PROPERTIES | TABLE_DESC;
		pgtable = (uint64_t *)(el2_pgtable_level2 + PAGE_SIZE * index);
		for (index = 0; index < 512; index++) {
			pgtable[index] = ((start + index * TWO_MB) & ~(TWO_MB - 1) )| DEVICE_PROPERTIES | BLOCK_DESC;
		}
	}

	/* Create the level 2 table */
	pgtable = (uint64_t *)el2_pgtable_level2;
	memset(pgtable, 0, PAGE_SIZE * 128);
	for (start = 0; start < M64_GB; start += TWO_MB) {
		index = TWO_MB_INDEX(start);
		/*
		 * Stage2 MMU
		 *
		 *  + ------------- +
		 *  | Normal memory |
		 *  + ------------- +
		 *  | EL2 Reserved  | Invalid
		 *  + ------------- +
		 *  | Normal memory |
		 *  + ------------- +
		 *  |      DDRC     | Invalid
		 *  + ------------- +
		 *  | Device memory |
		 *  + ------------- +
		 */
		if (start >= DRAM_SPACE_START && start < DRAM_SPACE_START + dramsz)
			pgtable[index] = start | MEMORY_PROPERTIES | BLOCK_DESC;

		if (start >= el2_base && start < el2_limit)
			pgtable[index] = 0;
	}
#else
	/*
	 *  + ------------ + ---> Pgtable
	 *  |              |
	 *  |     1MB      |
	 *  |              |
	 *  + ------------ + ---> Vector
	 *  |              |
	 *  |  1MB - 4KB   |
	 *  |              |
	 *  + ------------ + ---> Stack
	 *  |     4KB      |
	 *  + ------------ + ---> END
	 */

	/* 4 entry * 8B */
	el2_pgtable_level1 = el2_pgtable_base;
	/* 2KB entry * 8B */
	el2_pgtable_level2 = el2_pgtable_level1 + PAGE_SIZE;
	/* 251 * 512 * 8B */
	el2_pgtable_level3 = el2_pgtable_level2 + PAGE_SIZE * 4;

	/* Create the level 1 table */
	pgtable = (uint64_t *)el2_pgtable_level1;
	memset(pgtable, 0, PAGE_SIZE);
	for (start = 0; start < FOUR_GB; start += ONE_GB) {
		index = ONE_GB_INDEX(start);
		pgtable[index] = (el2_pgtable_level2 + PAGE_SIZE * index) | TABLE_DESC;
	}

	/* Create the level 2 table */
	pgtable = (uint64_t *)el2_pgtable_level2;
	memset(pgtable, 0, PAGE_SIZE * 4);
	for (start = 0; start < FOUR_GB; start += TWO_MB) {
		index = TWO_MB_INDEX(start);
		/*
		 * Stage2 MMU
		 *
		 *  + ------------- +
		 *  | Normal memory |
		 *  + ------------- +
		 *  | EL2 Reserved  | Invalid
		 *  + ------------- +
		 *  | Normal memory |
		 *  + ------------- +
		 *  |      DDRC     | Invalid
		 *  + ------------- +
		 *  | Device memory |
		 *  + ------------- +
		 */
		if (start >= el2_base && start < el2_limit)
			pgtable[index] = 0;
		else if (start >= DRAM_SPACE_START && start < DRAM_SPACE_START + dramsz)
			pgtable[index] = start | MEMORY_PROPERTIES | BLOCK_DESC;
		else if (start >= DEVICE_BASE && start < DEVICE_BASE + DEVICE_SIZE)
			pgtable[index] = start | DEVICE_PROPERTIES | BLOCK_DESC;
		else
			pgtable[index] = 0;
	}
#endif

	clean_dcache_range((uintptr_t)el2_base, EL2_RSVD_SIZE);

	/* Parse secure monitor */
	ambarella_fdt_parse_security();

	/* Setup EL2 system register */
	ambarella_el2_runtime_setup();
}

void ambarella_el2_rsvd_mem(uint64_t *base, uint64_t *limit)
{
	boot_cookie_t *cookie = boot_cookie_ptr();

	if (cookie->bld_ram_start & (TWO_MB - 1)) {
		ERROR("EL2 reserved memory must be 2MB aligned!\n");
		panic();
	}

	*base = cookie->bld_ram_start - EL2_RSVD_SIZE;
	*limit = cookie->bld_ram_start;
}

/*****************************************************************************/

/* ISS encoding for EXCEPTION from a Data Abort */
typedef struct __data_abort {
	/* Data Fault status Code. */
	uint32_t dfsc:6;
	/*  Write not Read */
	uint32_t wnr: 1;
	uint32_t s1ptw:1;
	uint32_t cm:1;
	uint32_t ea:1;
	uint32_t fnv:1;
	uint32_t res0:3;
	uint32_t ar:1;
	uint32_t sf:1;
	/* register number of Rt */
	uint32_t srt:5;
	uint32_t sse:1;
	/*
	 * Syndrome Access size.
	 * 00 - Byte
	 * 01 - Halfword
	 * 10 - word
	 * 11 - Doubleword
	 * */
	uint32_t sas:2;
	uint32_t isv:1;
	uint32_t il:1;
	uint32_t ec:6;
} da_esr_t;

/* Context when calling SMC */
typedef struct __pt_reg {
	uint64_t reg[32];

	uint64_t elr_el2;
	uint64_t spsr_el2;
	uint64_t far_el2;
	uint64_t esr_el2;
	uint64_t hpfar_el2;
	uint64_t res0;

	uint64_t elr_el1;
	uint64_t spsr_el1;
	uint64_t far_el1;
	uint64_t esr_el1;
} pt_reg_t;

static int32_t pinctrl_handle_mmio(pt_reg_t *pt_reg, u_register_t r, da_esr_t *esr)
{
	return ambarella_pinctrl_security_handle(&pt_reg->reg[esr->srt], r, esr->sas, esr->wnr);
}

static int32_t gpio_handle_mmio(pt_reg_t *pt_reg, u_register_t r, da_esr_t *esr)
{
	return ambarella_gpio_security_handle(&pt_reg->reg[esr->srt], r, esr->sas, esr->wnr);
}

static bool iomem_permitted(uint32_t r, uint32_t wnr)
{
	uint32_t idx, start, size, flag;

	for (idx = 0; idx < ARRAY_SIZE(reg_permission); idx++) {
		start = reg_permission[idx].start;
		size = reg_permission[idx].size;
		flag = reg_permission[idx].flag;

		if (start == 0 && size == 0)
			break;

		if (r >= start && r < start + size) {

			if (flag & REG_DBG_VERBOSE)
				NOTICE("[MON] CPU%d: %s [0x%08x]\n", plat_my_core_pos(), wnr ? "Write" : "Read", r);

			if (wnr && (flag & REG_ALLOW_WRITE))
				break;
			else if (!wnr && (flag & REG_ALLOW_READ))
				break;

			ERROR("%s register[0x%08x] denied\n", wnr ? "Write" : "Read", r);
			return false;
		}
	}


	return true;
}

static int32_t handle_read(pt_reg_t *pt_reg, u_register_t r, da_esr_t *esr)
{
	uint64_t value;

	switch(esr->sas) {
	case 0:
		value = mmio_read_8(r);
		break;
	case 1:
		value = mmio_read_16(r);
		break;
	case 2:
		value = mmio_read_32(r);
		break;
	case 3:
		value = mmio_read_64(r);
		break;
	default:
		panic();
	}

	pt_reg->reg[esr->srt] = value;

	return 0;
}

static int32_t handle_write(pt_reg_t *pt_reg, u_register_t r, da_esr_t *esr)
{
	uint64_t value = pt_reg->reg[esr->srt];

	switch(esr->sas) {
	case 0:
		mmio_write_8(r, (uint8_t)value);
		break;
	case 1:
		mmio_write_16(r, (uint16_t)value);
		break;
	case 2:
		mmio_write_32(r, (uint32_t)value);
		break;
	case 3:
		mmio_write_64(r, (uint64_t)value);
		break;
	default:
		panic();
	}

	return 0;
}

static int32_t handle_mmio(pt_reg_t *pt_reg, u_register_t r, da_esr_t *esr)
{
	if (!iomem_permitted(r, esr->wnr))
		return -EPERM;

	if (!pinctrl_handle_mmio(pt_reg, r, esr))
		return 0;

	if (!gpio_handle_mmio(pt_reg, r, esr))
		return 0;

	return esr->wnr ? handle_write(pt_reg, r, esr) : handle_read(pt_reg, r, esr);
}

/*
 * Stage2 Translation SIP handler
 */
uintptr_t ambarella_el2_fault_handler(uint32_t smc_fid,
		u_register_t x1, u_register_t x2, u_register_t x3, u_register_t x4,
		void *cookie, void *handle, u_register_t flags)
{
	pt_reg_t *pt_reg = (pt_reg_t *)x1;
	bool user_mode;
	da_esr_t *esr;
	u_register_t r;
	int32_t rval = SMC_UNK;

	assert(FNID_OF_SMC(smc_fid) == AMBA_SIP_EL2_DATA_ABORT);

	inv_dcache_range(x1, sizeof(pt_reg_t));

	esr = (da_esr_t *)&pt_reg->esr_el2;

	r = ((pt_reg->hpfar_el2 >> 4) << 12) | (pt_reg->far_el2 & PAGE_SIZE_MASK);

	if (!esr->isv)
		ERROR("No valid instruction syndrome: %lx \n", r);

	/* Handle device memory from EL1t/h */
	if (esr->ec == EC_DABORT_LOWER_EL
			&& esr->isv
#ifndef AMBARELLA_ENABLE_MEM_MONITOR
			&& r >= DDRC_BASE
#endif
			) {
		rval = handle_mmio(pt_reg, r, esr);
		if (!rval) {
			pt_reg->elr_el2 += 4;	/* Skip the PC to fix the EXCEPTION */
			goto exit;
		}
	}

	/* Route DATA Abort to EL1t EXCEPTION */
	pt_reg->far_el1 = pt_reg->far_el2;
	pt_reg->elr_el1 = pt_reg->elr_el2;
	pt_reg->esr_el1 = pt_reg->esr_el2;

	pt_reg->esr_el1 &= ~(ESR_EC_MASK << ESR_EC_SHIFT);			// clear EC
	pt_reg->esr_el1 &= ~(EABORT_DFSC_MASK << EABORT_DFSC_SHIFT);		// clear DFSC
	pt_reg->esr_el1 |= SYNC_EA_FSC;						// Synchronous External Abort

	user_mode = (GET_EL(pt_reg->spsr_el2) == MODE_EL0) ? true : false;

	if (user_mode)
		pt_reg->esr_el1 |= (EC_DABORT_LOWER_EL << ESR_EC_SHIFT);
	else
		pt_reg->esr_el1 |= (EC_DABORT_CUR_EL << ESR_EC_SHIFT);

	pt_reg->spsr_el1 = pt_reg->spsr_el2;
	pt_reg->spsr_el2 = 0x3c5;
	pt_reg->elr_el2 = read_vbar_el1() + (user_mode ? 0x400 : 0x200);

exit:
	clean_dcache_range(x1, sizeof(pt_reg_t));
	SMC_RET1(handle, rval);
}

static int32_t el2_create_l3_entry(uint64_t addr)
{
	uint32_t index;
	uint64_t i, align_addr, attr;
	uint64_t *desc = (uint64_t *)el2_pgtable_level2;

	assert(IS_PAGE_ALIGNED(addr));

	align_addr = round_down(addr, TWO_MB);
	index = TWO_MB_INDEX(align_addr);

	if ((desc[index] & DESC_MASK) == TABLE_DESC)
		return 0;

	assert(el2_pgtable_level3 < el2_vector_base);
	/* if corresponding L2 entry is a BLOCK_DESC, create it */
	desc[index] = el2_pgtable_level3 | TABLE_DESC;
	desc = (uint64_t *)el2_pgtable_level3;

	if (addr == CLAMP(addr, (uint64_t)DRAM_SPACE_START,
				DRAM_SPACE_START + calc_dram_size()))
		attr =  MEMORY_PROPERTIES | PAGE_DESC;
	else
		attr =  DEVICE_PROPERTIES | PAGE_DESC;

	for (i = align_addr; i < align_addr + TWO_MB; i += FOUR_KB) {
		index = FOUR_KB_INDEX(i) % 512;
		desc[index] = i | attr;
	}

	el2_pgtable_level3 += PAGE_SIZE;

	return 0;
}

static uint64_t *el2_find_l3_entry(uint64_t addr)
{
	uint64_t align_addr, index;
	uint64_t *desc = (uint64_t *)el2_pgtable_level2;

	assert(IS_PAGE_ALIGNED(addr));
	align_addr = round_down(addr, TWO_MB);
	index = TWO_MB_INDEX(align_addr);

	if ((desc[index] & DESC_MASK) != TABLE_DESC)
		return NULL;

	desc = (uint64_t *)(desc[index] & ~ DESC_MASK);

	return &desc[FOUR_KB_INDEX(addr) % 512];
}

/*
 * Ambarella Monitor [DANGEROUS]
 */
int32_t ambarella_el2_config_monitor_region(uint64_t start, uint32_t len,
		uint32_t access)
{
	uint64_t align_start, align_end, i;
	uint64_t el2_base, el2_limit;

	ambarella_el2_rsvd_mem(&el2_base, &el2_limit);
	if (start != CLAMP(start, el2_limit, DRAM_SPACE_START + calc_dram_size()))
		return -1;

	align_start = round_down(start, FOUR_KB);
	align_end = round_up(start + len, FOUR_KB);

	if (mmap_add_dynamic_region(align_start, align_start,
				align_end - align_start,
				MT_NON_CACHEABLE | MT_RW | MT_NS)) {
		ERROR("Mmap region error.\n");
		return -1;
	}

	for (i = align_start; i < align_end; i += FOUR_KB)
		el2_create_l3_entry(i);

	reg_permission[reg_perm_num].start = start;
	reg_permission[reg_perm_num].size = len;
	reg_permission[reg_perm_num].flag = access;
	reg_perm_num++;

	NOTICE("Ambarella Monitor: %llx - %llx attr: 0x%x\n",
			start, start + len, access);
	return 0;
}

void ambarella_el2_enable_monitor_region(uint64_t start, uint32_t len,
		uint32_t access)
{
	uint64_t *desc;
	uint64_t align_start, align_end, i;

	align_start = round_down(start, FOUR_KB);
	align_end = round_up(start + len, FOUR_KB);

	for (i = align_start; i < align_end; i += FOUR_KB) {
		desc = el2_find_l3_entry(i);
		assert(desc);

		if ((*desc & DESC_MASK) != INVALID_DESC) {
			*desc &= ~DESC_MASK;
			*desc |= INVALID_DESC;
		}
	}
}

void ambarella_el2_disable_monitor_region(uint64_t start, uint32_t len,
		uint32_t access)
{
	uint64_t *desc;
	uint64_t align_start, align_end, i;

	align_start = round_down(start, FOUR_KB);
	align_end = round_up(start + len, FOUR_KB);

	for (i = align_start; i < align_end; i += FOUR_KB) {
		desc = el2_find_l3_entry(i);
		assert(desc);

		if ((*desc & DESC_MASK) != PAGE_DESC) {
			*desc &= ~DESC_MASK;
			*desc |= PAGE_DESC;
		}
	}
}
