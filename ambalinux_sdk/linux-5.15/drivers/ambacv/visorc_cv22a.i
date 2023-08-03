#include <linux/module.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/of_irq.h>
#include <linux/uaccess.h>
#include "ambacv_kal.h"

#define VISORC_BASE     0xed000000
#define VISORC_SIZE     0x01000000

#define VISORC_SYNC     127

/* for VP-ORC */
#define VP_TRESET       0x010000
#define VP_CACHE        0x010004
#define VP_RESET_PC     0x010008

/* for SMEM */
#define SYNC_CTR_BASE   0x05F000

/* for RCT */
#define CLUSTER_SRESET  0x080228

/* For chicken bits */
#define VP_CROP_DISABLE 0x082002C

/* For CORC current time */
#define CORC_TIME       0x160044

/* for L2-cache */
#define REMAP_R0_START  0x0a0040
#define REMAP_R0_LIMIT  0x0a0044
#define REMAP_R0_RBASE  0x0a0048
#define REMAP_R1_START  0x0a0050
#define REMAP_R1_LIMIT  0x0a0054
#define REMAP_R1_RBASE  0x0a0058
#define REMAP_R2_START  0x0a0060
#define REMAP_R2_LIMIT  0x0a0064
#define REMAP_R2_RBASE  0x0a0068
#define REMAP_R3_START  0x0a0070
#define REMAP_R3_LIMIT  0x0a0074
#define REMAP_R3_RBASE  0x0a0078

/*For dummy registers in VP */
#define ARITH1_PU_DUMMY_OFFSET      0x800000U
#define INTERP_PU_DUMMY_OFFSET      0x801000U
#define INTEG_PU_DUMMY_OFFSET       0x802000U
#define MINMAX_PU_DUMMY_OFFSET      0x803000U
#define GEN_PU_DUMMY_OFFSET         0x804000U
#define CMP_PU_DUMMY_OFFSET         0x805000U
#define LOGIC1_PU_DUMMY_OFFSET      0x806000U
#define COUNT_PU_DUMMY_OFFSET       0x807000U
#define WARP_PU_DUMMY_OFFSET        0x808000U
#define SEGMENTS_PU_DUMMY_OFFSET    0x809000U
#define STATISTICS_PU_DUMMY_OFFSET  0x80a000U
#define COPY_PU_DUMMY_OFFSET        0x80b000U
#define TRAN1_PU_DUMMY_OFFSET       0x80c000U
#define TRAN2_PU_DUMMY_OFFSET       0x80d000U
#define RESAMP_PU_DUMMY_OFFSET      0x80e000U
#define ICE1_PU_DUMMY_OFFSET        0x80f000U
#define ICE2_PU_DUMMY_OFFSET        0x810000U
#define VP_MAIN_PU_DUMMY_OFFSET     0x820008U
#define VP_XMEM_PU_DUMMY_OFFSET     0x860004U

/*For dummy registers in VMEM */
#define VMEM_DUMMY_OFFSET           0x13fff8U


/*For dummy registers in STORC */
#define STORC_DUMMY_OFFSET           0x010008U

/*cehu registers */
#define CEHU_ARITH1_DP_OFFSET       0x800FFCU
#define CEHU_INTERP_DP_OFFSET       0x801FFCU
#define CEHU_INTEG_DP_OFFSET        0x802FFCU
#define CEHU_MINMAX_DP_OFFSET       0x803FFCU
#define CEHU_GEN_DP_OFFSET          0x804FFCU
#define CEHU_COMPARE_DP_OFFSET      0x805FFCU
#define CEHU_LOGIC1_DP_OFFSET       0x806FFCU
#define CEHU_COUNT_DP_OFFSET        0x807FFCU
#define CEHU_WARP_DP_OFFSET         0x808FFCU
#define CEHU_SEGMENTS_DP_OFFSET     0x8097F0U
#define CEHU_STATISTICS_DP_OFFSET   0x80AFFCU
#define CEHU_COPY_DP_OFFSET         0x80BFFCU
#define CEHU_TRANS1_DP_OFFSET       0x80CFFCU
#define CEHU_TRANS2_DP_OFFSET       0x80DFFCU
#define CEHU_RESAMP_DP_OFFSET       0x80EFFCU
#define CEHU_ICE1_DP_OFFSET         0x80FFFCU
#define CEHU_ICE2_DP_OFFSET         0x810FFCU
#define CEHU_VPMAIN_DP_OFFSET       0x820FFCU
#define CEHU_XMEM_DP_OFFSET         0x860FFCU
#define CEHU_VMEM_DP_OFFSET         0x13FFF4U
#define CEHU_OL2C_DP_OFFSET         0x0A00C8U
#define CEHU_FEX_DP_OFFSET          0x255FFCU

#define visorc_reg(x)   (visorc_io_base + (x))

static void __iomem     *visorc_io_base;
static unsigned int     vp_irq;

#define DEBUG_ISR
#ifdef  DEBUG_ISR
#define ILOG            printk
#else
#define ILOG(...)
#endif

#define DUMMP_WRITE_DATA_PATTERN    0x12345678
static void vp_dummy_reg_write(void)
{
    // Write fix pattern to VP dummy registers
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(ARITH1_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(INTERP_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(INTEG_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(MINMAX_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(GEN_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(CMP_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(LOGIC1_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(COUNT_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(WARP_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(SEGMENTS_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(STATISTICS_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(COPY_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(TRAN1_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(TRAN2_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(RESAMP_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(ICE1_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(ICE2_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(VP_MAIN_PU_DUMMY_OFFSET));
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(VP_XMEM_PU_DUMMY_OFFSET));

    // Write fix pattern to VMEM dummy registers
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(VMEM_DUMMY_OFFSET));

    // Write fix pattern to STORC dummy registers
    writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(STORC_DUMMY_OFFSET));
}

static void pre_reset_proc(void)
{
    vp_dummy_reg_write();
}

static uint32_t post_reset_proc_part1(void)
{
    uint32_t val;
    uint32_t retcode = 0U;

    // Read back VP dummy registers and check with default value
    // There is no WARP_PU_DUMMY and SEGMENTS_PU_DUMMY registers

    val = readl(visorc_reg(ARITH1_PU_DUMMY_OFFSET));
    if (val != 0xcafef00dU) {
        retcode |= 0x1U;
    }

    val = readl(visorc_reg(INTERP_PU_DUMMY_OFFSET));
    if (val != 0xc0cac01aU) {
        retcode |= 0x2U;
    }

    val = readl(visorc_reg(INTEG_PU_DUMMY_OFFSET));
    if (val != 0x0U) {
        retcode |= 0x4U;
    }

    val = readl(visorc_reg(MINMAX_PU_DUMMY_OFFSET));
    if (val != 0xfe52c67bU) {
        retcode |= 0x8U;
    }

    val = readl(visorc_reg(GEN_PU_DUMMY_OFFSET));
    if (val != 0x00000000U) {
        retcode |= 0x10U;
    }

    val = readl(visorc_reg(CMP_PU_DUMMY_OFFSET));
    if (val != 0x63a87ad4U) {
        retcode |= 0x20U;
    }

    val = readl(visorc_reg(LOGIC1_PU_DUMMY_OFFSET));
    if (val != 0xc0cac01aU) {
        retcode |= 0x40U;
    }

    val = readl(visorc_reg(COUNT_PU_DUMMY_OFFSET));
    if (val != 0xc0cac01aU) {
        retcode |= 0x80U;
    }

    val = readl(visorc_reg(STATISTICS_PU_DUMMY_OFFSET));
    if (val != 0xc0cac01aU) {
        retcode |= 0x400U;
    }

    val = readl(visorc_reg(COPY_PU_DUMMY_OFFSET));
    if (val != 0xc09100d9U) {
        retcode |= 0x800U;
    }

    val = readl(visorc_reg(TRAN1_PU_DUMMY_OFFSET));
    if (val != 0xc0ffee44U) {
        retcode |= 0x1000U;
    }

    val = readl(visorc_reg(TRAN2_PU_DUMMY_OFFSET));
    if (val != 0xc0ffee44U) {
        retcode |= 0x2000U;
    }

    val = readl(visorc_reg(RESAMP_PU_DUMMY_OFFSET));
    if (val != 0x5aca3b12U) {
        retcode |= 0x4000U;
    }

    val = readl(visorc_reg(ICE1_PU_DUMMY_OFFSET));
    if (val != 0xb32ba980U) {
        retcode |= 0x8000U;
    }

    return retcode;
}

static uint32_t post_reset_proc_part2(void)
{
    uint32_t val;
    uint32_t retcode = 0U;

    val = readl(visorc_reg(ICE2_PU_DUMMY_OFFSET));
    if (val != 0xb32ba980U) {
        retcode |= 0x10000U;
    }

    val = readl(visorc_reg(VP_MAIN_PU_DUMMY_OFFSET));
    if (val != 0x00000000U) {
        retcode |= 0x20000U;
    }

    val = readl(visorc_reg(VP_XMEM_PU_DUMMY_OFFSET));
    if (val != 0x00000000U) {
        retcode |= 0x40000U;
    }

    // Read back VMEM dummy registers and check with default value
    val = readl(visorc_reg(VMEM_DUMMY_OFFSET));
    if (val != 0x007fU) {
        retcode |= 0x80000U;
    }

    // Read back STORC dummy registers and check with default value
    val = readl(visorc_reg(STORC_DUMMY_OFFSET));
    if (val != 0x0U) {
        retcode |= 0x100000U;
    }

    if (retcode == 0U) {
        // reset to defaul value ok, write dummy reg
        vp_dummy_reg_write();
    }

    return retcode;
}

static uint32_t post_reset_proc_part3(void)
{

    /* Clear vp, vmem and ol2c safety register. */
    writel_relaxed(0x00000011U, visorc_reg(CEHU_ARITH1_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_INTERP_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_INTEG_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_MINMAX_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_GEN_DP_OFFSET));
    /* #sm_mask_failure_point=1, sm_failure_point=107, reset_cycles_m1=3 , mask out vp error for COMPARE_DP */
    writel_relaxed(0x23001ae1U, visorc_reg(CEHU_COMPARE_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_LOGIC1_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_COUNT_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_WARP_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_SEGMENTS_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_STATISTICS_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_COPY_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_TRANS1_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_TRANS2_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_RESAMP_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_ICE1_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_ICE2_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_VPMAIN_DP_OFFSET));
    writel_relaxed(0x00000011U, visorc_reg(CEHU_XMEM_DP_OFFSET));
    writel_relaxed(0x00000001U, visorc_reg(CEHU_VMEM_DP_OFFSET));
    writel_relaxed(0x00000000U, visorc_reg(CEHU_OL2C_DP_OFFSET));
    writel_relaxed(0x00040000U, visorc_reg(CEHU_FEX_DP_OFFSET));

    return 0U;
}

static uint32_t post_reset_proc(void)
{
    uint32_t retcode = 0U;

    retcode = post_reset_proc_part1();
    retcode |= post_reset_proc_part2();
    retcode |= post_reset_proc_part3();

    return retcode;
}

static void __init vp_of_init(struct device_node *np)
{
	vp_irq = irq_of_parse_and_map(np, 0);
	if (vp_irq <= 0) {
		pr_err("[AMBACV] Error!! can't setup VP0 irq\n");
	}
}

static const struct of_device_id __vp_of_table = {
	.compatible = "ambarella,vp-module",
	.data = vp_of_init,
};

void visorc_init(void)
{
	struct device_node *np;
	const struct of_device_id *match;
	of_init_fn_1 init_func;

	if (visorc_io_base == NULL) {
		visorc_io_base = ioremap(VISORC_BASE, VISORC_SIZE);
		if (visorc_io_base == NULL) {
			pr_err("ioremap VISORC registers failed\n");
		}
	}

	for_each_matching_node_and_match(np, &__vp_of_table, &match) {
		if (!of_device_is_available(np)) {
			continue;
		}

		init_func = match->data;
		init_func(np);
	}

	/* set up sod/vp binary memory regions */
	ambacv_global_mem.sod.base  = 0;
	ambacv_global_mem.sod.size  = 0;
	ambacv_global_mem.vp.base   = ambacv_global_mem.all.base + 0x00400000;
	ambacv_global_mem.vp.size   = 0x00200000;
	ambacv_debug_port.base      = VISORC_BASE;
	ambacv_debug_port.size      = VISORC_SIZE;
	pr_info("[AMBACV] TARGET CHIP:   CV22A\n");
}

void visorc_shutdown(void)
{
	irq_dispose_mapping(vp_irq);
	if (visorc_io_base != NULL) {
		iounmap(visorc_io_base);
		visorc_io_base = NULL;
	}
}

int visorc_finalize_loading(unsigned long arg)
{
	ambacv_mem_t mem;
	int ret;

	ret = copy_from_user(&mem, (void*)arg, 4 * sizeof(mem.sod));
	if (ret != 0)
		return ret;

	ambacv_cache_clean(ambacv_c2v(mem.vp.base),   mem.vp.size);
	pr_info("\tVP:   [0x%llx ++ 0x%llx]\n",
	       ambacv_c2p(mem.vp.base),   mem.vp.size);

	return 0;
}

int visorc_start(unsigned int flag, visorc_init_params_t *sysconfig)
{
	int i;
	int ret;
	phys_addr_t offset;

	pre_reset_proc();

	/* soft-reset all clusters*/
	/* soft-reset vp0, vmem0 and storc. reset storc/fex also reset visorc audio clock coutner. */
	writel_relaxed(0x00000000, visorc_reg(CLUSTER_SRESET));
	writel_relaxed(0x00000380, visorc_reg(CLUSTER_SRESET));
	writel_relaxed(0x00000000, visorc_reg(CLUSTER_SRESET));

	ret = (int)post_reset_proc();
	if(ret != 0) {
		pr_err("[ERROR] visorc_start(): post_reset_proc failed with ret = 0x%x", ret);
	}

	/*-= Reset chicken bits =-*/
	writel_relaxed(0x00100010, visorc_reg(VP_CROP_DISABLE));

	/* setup remap registers */
	writel_relaxed(0x00000000, visorc_reg(REMAP_R0_START));
	writel_relaxed(0x00600000, visorc_reg(REMAP_R0_LIMIT));
	writel_relaxed(ambacv_global_mem.all.base | 0x1, visorc_reg(REMAP_R0_RBASE));

	writel_relaxed(HOTLINK_MEM_REGION_VADDR_BASE, visorc_reg(REMAP_R1_START));
	writel_relaxed(HOTLINK_MEM_REGION_VADDR_BASE + ambacv_global_mem.hotlink_region.size, visorc_reg(REMAP_R1_LIMIT));
	writel_relaxed(ambacv_global_mem.hotlink_region.base | 0x1, visorc_reg(REMAP_R1_RBASE));

	writel_relaxed(0x00000000, visorc_reg(REMAP_R2_START));
	writel_relaxed(0x00000000, visorc_reg(REMAP_R2_LIMIT));
	writel_relaxed(0x00000000, visorc_reg(REMAP_R2_RBASE));

	writel_relaxed(0x00000000, visorc_reg(REMAP_R3_START));
	writel_relaxed(0x00000000, visorc_reg(REMAP_R3_LIMIT));
	writel_relaxed(0x00000000, visorc_reg(REMAP_R3_RBASE));

	/* setup visorc reset vector */
	offset = ambacv_global_mem.vp.base - ambacv_global_mem.all.base;
	writel_relaxed((uint32_t)offset, visorc_reg(VP_RESET_PC));

	/* invalidate both I/D cache */
	writel_relaxed(0x00000003, visorc_reg(VP_CACHE));
	//writel_relaxed(0x00000000, visorc_reg(0x0a008C));

	/* reset orcvp */
	writel_relaxed(0x00001F00, visorc_reg(VP_TRESET));

	/* reset all sync counters */
	for (i = 0; i < 0x80; i++) {
		if (0 <= i && i <= 111) continue;
		orc_send_sync_count(i, 0x1020);
	}
	for (i = 0; i < 0x80; i++) {
		if (0 <= i && i <= 111) continue;
		orc_send_sync_count(i, 0x0000);
	}
	for (i = 0; i < 0x80; i++) {
		if (0 <= i && i <= 111) continue;
		orc_send_sync_count(i, 0x1020);
	}
	for (i = 0; i < 0x80; i++) {
		if (0 <= i && i <= 111) continue;
		orc_send_sync_count(i, 0x0000);
	}
	sysconfig->arm_base_timestamp = readl(visorc_reg(CORC_TIME));
	(void) ambacv_cache_clean(&sysconfig->arm_base_timestamp, sizeof(sysconfig->arm_base_timestamp));

	/* release orc-sod/vp */
	writel_relaxed(0x0000001F, visorc_reg(VP_TRESET));

	return ret;
}

int visorc_stop(void)
{
	return 0;
}

void orc_send_sync_count(int id, int val)
{
	writel_relaxed(val, visorc_reg(SYNC_CTR_BASE) + 4 * id);
}

void visorc_kick(void)
{
	orc_send_sync_count(VISORC_SYNC, 0x1400);
}

void visorc_armidsp_queue_kick(void)
{
	orc_send_sync_count(VISORC_SYNC, 0x1400);
}

void visorc_armvin_queue_kick(void)
{
	orc_send_sync_count(VISORC_SYNC, 0x1400);
}

void visorc_softreset(int unit_id)
{
	pr_info("[AMBACV]: softreset %d\n", unit_id);
	/* reset vp0 and vmem0 */
	writel(0x0000000, visorc_reg(CLUSTER_SRESET));
	writel(0x0000300, visorc_reg(CLUSTER_SRESET));
	writel(0x0000000, visorc_reg(CLUSTER_SRESET));

	/*-= Reset chicken bits =-*/
	writel(0x00100010, visorc_reg(VP_CROP_DISABLE));
}

uint32_t  visorc_get_curtime(void)
{
  return readl(visorc_reg(CORC_TIME));
} /* visorc_get_curtime() */

