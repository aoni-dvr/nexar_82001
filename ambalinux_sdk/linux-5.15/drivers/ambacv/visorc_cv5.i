#include <linux/module.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/of_irq.h>
#include <linux/uaccess.h>
#include "ambacv_kal.h"

#define VISORC_BASE     0x20ed000000
#define VISORC_SIZE     0x01000000

#define VISORC_SYNC     255

/* for VP-ORC */
#define VP_TRESET       0x010000
#define VP_CACHE        0x010004
#define VP_RESET_PC     0x010008

/* for SMEM */
#define SYNC_CTR_BASE   0x05F000

/* for RCT */
#define CLUSTER_SRESET  0x0d0228

/* For chicken bits */
#define VP_CROP_DISABLE 0x082002c

/* For CORC current time */
#define CORC_TIME       0x140044

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

#define visorc_reg(x)   (visorc_io_base + (x))

static void __iomem     *visorc_io_base;
static unsigned int     vp_irq;
extern uint32_t debug_level;

#define DEBUG_ISR
#ifdef  DEBUG_ISR
#define ILOG            printk
#else
#define ILOG(...)
#endif

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
	pr_info("[AMBACV] TARGET CHIP:   CV5 \n");
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
	pr_info("\tVP:   [0x%08x ++ 0x%08x]\n",
	       (uint32_t)ambacv_c2p(mem.vp.base),   (uint32_t)mem.vp.size);

	return 0;
}

int visorc_start(unsigned int flag, visorc_init_params_t *sysconfig)
{
	int i;
	phys_addr_t offset;

	/* soft-reset all clusters*/
	/* have to skip bit[8] since it resets audio tick counter as well */
	writel_relaxed(0x00000000, visorc_reg(CLUSTER_SRESET));
	writel_relaxed(0x00000300, visorc_reg(CLUSTER_SRESET));
	writel_relaxed(0x00000000, visorc_reg(CLUSTER_SRESET));

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
	for (i = 240; i < 256; i++) {
		orc_send_sync_count(i, 0x1020);
	}
	for (i = 240; i < 256; i++) {
		orc_send_sync_count(i, 0x0000);
	}
	for (i = 240; i < 256; i++) {
		orc_send_sync_count(i, 0x1020);
	}
	for (i = 240; i < 256; i++) {
		orc_send_sync_count(i, 0x0000);
	}
	sysconfig->arm_base_timestamp = readl(visorc_reg(CORC_TIME));
	(void) ambacv_cache_clean(&sysconfig->arm_base_timestamp, sizeof(sysconfig->arm_base_timestamp));

	/* release orc-sod/vp */
	writel_relaxed(0x0000001F, visorc_reg(VP_TRESET));

	return 0;
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
	if (debug_level > 0)
	{
		pr_info("[AMBACV]: softreset %d\n", unit_id);
	} /* if (debug_level > 0) */
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

