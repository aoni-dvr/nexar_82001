#include <linux/module.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/of_irq.h>
#include <linux/uaccess.h>
#include "ambacv_kal.h"

#define VISORC_BASE     0xed000000
#define VISORC_SIZE     0x01000000

#define VISORC_SYNC     56

#define SOD_TRESET      0x000000
#define SOD_CACHE       0x000004
#define SOD_RESET_PC    0x000008
#define VP_TRESET       0x010000
#define VP_CACHE        0x010004
#define VP_RESET_PC     0x010008
#define VP_CTX_INFO_0   0x010030
#define VP_CTX_INFO_1   0x010034
#define VP_CTX_INFO_2   0x010038
#define VP_CTX_INFO_3   0x01003C
#define CLUSTER_SRESET  0x080228

#define SYNC_CTR_BASE   0x047C00

#define VP0_INT_STATUS  0x82000C
#define VP0_INT_CLEAR   0x820010
#define VP0_INT_ENABLE  0x820014
#define VP0_DPAR        0x820028
#define VP0_CTRL        0x820064
#define VP0_RESET_FLAG  0xa00020

#define VP1_INT_STATUS  0x92000C
#define VP1_INT_CLEAR   0x920010
#define VP1_INT_ENABLE  0x920014
#define VP1_DPAR        0x920028
#define VP1_CTRL        0x920064
#define VP1_RESET_FLAG  0xe00020

#define visorc_reg(x)   (visorc_io_base + (x))

static void __iomem     *visorc_io_base;
static unsigned int     vp0_irq;
static unsigned int     vp1_irq;
static unsigned int     vp_flag;

//#define DEBUG_ISR
#ifdef  DEBUG_ISR
#define ILOG            printk
#else
#define ILOG(...)
#endif

static irqreturn_t vp0_isr(int irq, void *devid)
{
	uint32_t value;

	value = readl(visorc_reg(VP0_INT_STATUS));
	ILOG("ENTER VP0 ISR, status 0x%X\n", value);

	value = readl(visorc_reg(VP_CTX_INFO_0));
	ILOG("CONTEXT SWTICH INFO_0 get: 0x%x\n", value);
	if (value & 0x10) {
		writel(0x00000000, visorc_reg(CLUSTER_SRESET));
		writel(0x00001400, visorc_reg(CLUSTER_SRESET));
		writel(0x00000000, visorc_reg(CLUSTER_SRESET));
		writel(0xA5AFE5E7, visorc_reg(VP0_RESET_FLAG));

		value &= ~0x10;
		writel(value, visorc_reg(VP_CTX_INFO_0));
		ILOG("CONTEXT SWTICH INFO_0 set: 0x%x\n", value);
		writel(0x00000002, visorc_reg(VP0_CTRL));
	}

	value = (readl(visorc_reg(VP0_INT_CLEAR))) & ~1;
	writel(value, visorc_reg(VP0_INT_CLEAR));
	ILOG("VP0 ISR, clear 0x%X\n", value);
	return IRQ_HANDLED;
}

static irqreturn_t vp1_isr(int irq, void *devid)
{
	uint32_t value;

	value = readl(visorc_reg(VP1_INT_STATUS));
	ILOG("ENTER VP1 ISR, status 0x%X\n", value);

	value = readl(visorc_reg(VP_CTX_INFO_2));
	ILOG("CONTEXT SWTICH INFO_2 get: 0x%x\n", value);
	if (value & 0x10) {
		writel(0x00000000, visorc_reg(CLUSTER_SRESET));
		writel(0x00002800, visorc_reg(CLUSTER_SRESET));
		writel(0x00000000, visorc_reg(CLUSTER_SRESET));
		writel(0xA5AFE5E7, visorc_reg(VP1_RESET_FLAG));

		value &= ~0x10;
		writel(value, visorc_reg(VP_CTX_INFO_2));
		ILOG("CONTEXT SWTICH INFO_2 set: 0x%x\n", value);
		writel(0x00000002, visorc_reg(VP1_CTRL));
	}

	value = (readl(visorc_reg(VP1_INT_CLEAR))) & ~1;
	writel(value, visorc_reg(VP1_INT_CLEAR));
	ILOG("VP1 ISR, clear 0x%X\n", value);
	return IRQ_HANDLED;
}

static void __init vp_of_init(struct device_node *np)
{
	vp0_irq = irq_of_parse_and_map(np, 0);
	if (vp0_irq <= 0) {
		pr_err("[AMBACV] Error!! can't setup VP0 irq\n");
	}

	vp1_irq = irq_of_parse_and_map(np, 1);
	if (vp1_irq <= 0) {
		pr_err("[AMBACV] Error!! can't setup VP1 irq\n");
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
	ambacv_global_mem.sod.base  = ambacv_global_mem.all.base + 0x00200000;
	ambacv_global_mem.sod.size  = 0x00200000;
	ambacv_global_mem.vp.base   = ambacv_global_mem.all.base + 0x00400000;
	ambacv_global_mem.vp.size   = 0x00200000;
	ambacv_debug_port.base      = VISORC_BASE;
	ambacv_debug_port.size      = VISORC_SIZE;
	pr_info("[AMBACV] TARGET CHIP:   CV1 \n");
}

void visorc_shutdown(void)
{
	irq_dispose_mapping(vp0_irq);
	irq_dispose_mapping(vp1_irq);

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
	ambacv_cache_clean(ambacv_c2v(mem.sod.base),  mem.sod.size);

	pr_info("\tVP:   [0x%08x ++ 0x%08x]\n",
	       (uint32_t)ambacv_c2p(mem.vp.base),   (uint32_t)mem.vp.size);
	pr_info("\tSOD:  [0x%08x ++ 0x%08x]\n",
	       (uint32_t)ambacv_c2p(mem.sod.base),  (uint32_t)mem.sod.size);

	return 0;
}

int visorc_start(unsigned int flag, visorc_init_params_t *sysconfig)
{
	int i, rval;
	phys_addr_t offset;

	/* soft-reset all clusters*/
	/* have to skip bit[9] since it resets audio tick counter as well */
	writel_relaxed(0x00000000, visorc_reg(CLUSTER_SRESET));
	writel_relaxed(0x00003D80, visorc_reg(CLUSTER_SRESET));
	writel_relaxed(0x00000000, visorc_reg(CLUSTER_SRESET));

	/* setup remap registers */
	writel_relaxed(0x00000000, visorc_reg(0xa0040));
	writel_relaxed(0x00600000, visorc_reg(0xa0044));
	writel_relaxed(ambacv_global_mem.all.base | 0x1, visorc_reg(0xa0048));
	writel_relaxed(0x00000000, visorc_reg(0xa0058));
	writel_relaxed(0x00000000, visorc_reg(0xa0068));
	writel_relaxed(0x00000000, visorc_reg(0xa0078));

	/* setup visorc reset vector */
	offset = ambacv_global_mem.sod.base - ambacv_global_mem.all.base;
	writel_relaxed((uint32_t)offset, visorc_reg(SOD_RESET_PC));
	offset = ambacv_global_mem.vp.base - ambacv_global_mem.all.base;
	writel_relaxed((uint32_t)offset, visorc_reg(VP_RESET_PC));

	/* invalidate I-cache */
	writel_relaxed(0x00000001, visorc_reg(SOD_CACHE));
	writel_relaxed(0x00000001, visorc_reg(VP_CACHE));
	writel_relaxed(0x00000000, visorc_reg(SOD_CACHE));
	writel_relaxed(0x00000000, visorc_reg(VP_CACHE));

	/* set VP dram_fromat to 2 */
	writel_relaxed(0x00000010, visorc_reg(VP0_DPAR));
	writel_relaxed(0x00000010, visorc_reg(VP1_DPAR));

	/* enable VP-reset flow if requested by user-space */
	vp_flag = flag;
	if (vp_flag) {
		rval = request_irq(vp0_irq, vp0_isr, IRQF_TRIGGER_RISING,
				   "vp0", NULL);
		if (rval) {
			pr_err("[AMBACV] request vp0 irq failed(%d)\n", rval);
		}

		rval = request_irq(vp1_irq, vp1_isr, IRQF_TRIGGER_RISING,
				   "vp1", NULL);
		if (rval) {
			pr_err("[AMBACV] request vp1 irq failed(%d)\n", rval);
		}

		/* enable VMEM write from VIS-ORC */
		writel_relaxed(0x00000002, visorc_reg(VP0_CTRL));
		writel_relaxed(0x00000002, visorc_reg(VP1_CTRL));
	}

	/* reset orc-sod/vp */
	writel_relaxed(0x00000F00, visorc_reg(SOD_TRESET));
	writel_relaxed(0x00000F00, visorc_reg(VP_TRESET));

	/* reset all sync counters */
	for (i = 0; i < 0x40; i++) {
		if (1 <= i && i <= 49) continue;
		orc_send_sync_count(i, 0x1020);
	}
	for (i = 0; i < 0x40; i++) {
		if (1 <= i && i <= 49) continue;
		orc_send_sync_count(i, 0x0000);
	}
	for (i = 0; i < 0x40; i++) {
		if (1 <= i && i <= 49) continue;
		orc_send_sync_count(i, 0x1020);
	}
	for (i = 0; i < 0x40; i++) {
		if (1 <= i && i <= 49) continue;
		orc_send_sync_count(i, 0x0000);
	}
	sysconfig->arm_base_timestamp = readl(visorc_reg(0x30044));

	writel_relaxed(0x00000E0F, visorc_reg(VP_TRESET));
	writel_relaxed(0x0000000E, visorc_reg(VP_TRESET));
	writel_relaxed(0x00000E0F, visorc_reg(SOD_TRESET));
	writel_relaxed(0x0000000E, visorc_reg(SOD_TRESET));

	/* release orc-sod/vp */
	writel_relaxed(0x0000000F, visorc_reg(SOD_TRESET));
	writel_relaxed(0x0000000F, visorc_reg(VP_TRESET));

	return 0;
}

int visorc_stop(void)
{
	if (vp_flag) {
		free_irq(vp0_irq, NULL);
		free_irq(vp1_irq, NULL);

		/* disable VMEM write from VIS-ORC */
		writel_relaxed(0x00000000, visorc_reg(VP0_CTRL));
		writel_relaxed(0x00000000, visorc_reg(VP1_CTRL));
	}
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
}

uint32_t  visorc_get_curtime(void)
{
  return readl(visorc_reg(0x30044));
} /* visorc_get_curtime() */

