/*
 * Copyright (c) 2013-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <assert.h>
#include <errno.h>
#include <plat_private.h>

#if (AMBARELLA_SUPPORT_SEURITY_CTRL == 1)

static uint32_t devic_security_ctrl[3] = {0};

static void ambarella_device_security_restore(void)
{
	mmio_write_32((AXI_SEC_CTRL_BASE + AXI_SEC0_CTRL_OFFSET), devic_security_ctrl[0]);
	mmio_write_32((AXI_SEC_CTRL_BASE + AXI_SEC1_CTRL_OFFSET), devic_security_ctrl[1]);
	mmio_write_32((AXI_SEC_CTRL_BASE + AXI_SEC2_CTRL_OFFSET), devic_security_ctrl[2]);
}

#endif

/* Setup specified device into secure world */
void ambarella_device_security_setup(uint32_t ctrl_bit)
{
#if (AMBARELLA_SUPPORT_SEURITY_CTRL == 1)
	assert(ctrl_bit < 96);
	mmio_setbits_32(AXI_SEC_CTRL_BASE + AXI_SEC0_CTRL_OFFSET + (ctrl_bit >> 5) * 4, 1 << (ctrl_bit % 32));
	devic_security_ctrl[ctrl_bit >> 5] |= 1 << (ctrl_bit % 32);
#endif
}

/*
 * Security setup:
 * - Setup all devices non-secure, and let DTS determine the secure devices.
 * - Setup secure memory if secure boot.
 * - Setup ATT to prevent EL2 reserved memory from corrupted by peripherals if
 *   EL2 is used, i.e., the DTB is signed.
 */
void ambarella_security_setup(void)
{
#if !defined(CONFIG_ATF_AMBALINK)
	uint64_t secure_limit, pagesz;
#endif

	assert(boot_cookie_ptr()->bld_ram_start == BL33_BASE);

#if (AMBARELLA_SUPPORT_SEURITY_CTRL == 1)
	/* Disable all devices's security by default in cold boot */
	ambarella_device_security_restore();
#endif

#if defined(CONFIG_ATF_AMBALINK)
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
	/* cv2fs: GPV NIC400 B 0xf1000000 is R52 only */
#else
	mmio_write_32(NIC_GPV_REG(0x08), NON_SECURE); /* Non-Secure AHB */
	mmio_write_32(NIC_GPV_REG(0x0c), NON_SECURE); /* Secure AHB */
	mmio_write_32(NIC_GPV_REG(0x10), NON_SECURE); /* AXI Config */
	mmio_write_32(NIC_GPV_REG(0x14), NON_SECURE); /* GIC */
#if (NIC_GPV_MASTER_PORT > 4)
	mmio_write_32(NIC_GPV_REG(0x10), NON_SECURE); /* AXI Config */
	mmio_write_32(NIC_GPV_REG(0x18), NON_SECURE);
	mmio_write_32(NIC_GPV_REG(0x1c), NON_SECURE);
	mmio_write_32(NIC_GPV_REG(0x20), NON_SECURE);
	mmio_write_32(NIC_GPV_REG(0x24), NON_SECURE);
	mmio_write_32(NIC_GPV_REG(0x28), NON_SECURE);
#endif
#endif

#if defined(CONFIG_ATF_SPD_OPTEE) && (AMBARELLA_SUPPORT_SEURITY_CTRL == 1)
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
	mmio_write_32(NIC_GPV_REG(0x0C), SECURE);     /* AXI Config */
#else
	mmio_write_32(NIC_GPV_REG(0x10), SECURE);     /* AXI Config */
#endif // AXI for SOC
	/* LINUX should not access Secure scratchpad */
	mmio_write_32((AXI_SEC_CTRL_BASE + AXI_SEC0_CTRL_OFFSET), 0x80000000);
#endif

	isb();
	dsb();

#else //CONFIG_ATF_AMBALINK
	mmio_write_32(NIC_GPV_REG(0x08), NON_SECURE); /* Non-Secure AHB */
	mmio_write_32(NIC_GPV_REG(0x0c), NON_SECURE); /* Secure AHB */
	mmio_write_32(NIC_GPV_REG(0x10), SECURE);     /* AXI Config */
	mmio_write_32(NIC_GPV_REG(0x14), NON_SECURE); /* GIC */
#if (NIC_GPV_MASTER_PORT > 4)
	mmio_write_32(NIC_GPV_REG(0x10), NON_SECURE); /* AXI Config */
	mmio_write_32(NIC_GPV_REG(0x18), NON_SECURE);
	mmio_write_32(NIC_GPV_REG(0x1c), NON_SECURE);
	mmio_write_32(NIC_GPV_REG(0x20), NON_SECURE);
	mmio_write_32(NIC_GPV_REG(0x24), NON_SECURE);
	mmio_write_32(NIC_GPV_REG(0x28), NON_SECURE);
#endif
	isb();
	dsb();

	/* setup platform ATT (Address Translation Table) */
	ambarella_att_setup();

	/* If secure boot is disabled, no need to setup Secure Memory */
	if (!ambarella_is_secure_boot())
		return;

	/* Setup and enable Secure Memory */
	pagesz = dram_vpn_page_size();
	mmio_write_32(DDRC_REG(DRAM_SECMEM_BASE_OFFSET), (uint32_t)(BL2_BASE / pagesz));
	secure_limit = boot_cookie_ptr()->bld_ram_start;
	secure_limit -= ambarella_el2_is_used() ? EL2_RSVD_SIZE : 0;
	secure_limit -= BL32_SHMEM_SIZE;
	mmio_write_32(DDRC_REG(DRAM_SECMEM_LIMIT_OFFSET), (uint32_t)((secure_limit / pagesz) - 1));
	mmio_setbits_32(DDRC_REG(DRAM_SECMEM_CTRL_OFFSET), 0x1);
#endif //CONFIG_ATF_AMBALINK
}

