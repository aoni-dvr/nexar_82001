/*
 * Remote processor messaging transport
 *
 *  (C) 2016 Ambarella, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/remoteproc.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/virtio_ids.h>
#include <linux/rpmsg.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>
#include <linux/irqdomain.h>
#include <linux/arm-smccc.h>
#include <soc/ambarella/ambalink/remoteproc.h>
#include <soc/ambarella/ambalink/ambalink_cfg.h>
#include <soc/ambarella/ambalink/rpmsg_compat.h>
#include <soc/ambarella/iav_helper.h>
#include <asm/cacheflush.h>

#include "remoteproc_internal.h"

#if defined(CONFIG_AMBALINK_SD) || defined(CONFIG_AMBALINK_SD_MODULE)
#include <linux/aipc/rpmsg_sd.h>
/* module memory not mapped, so put memory here to debug when CONFIG_AMBALINK_SD=m */
struct rpdev_sdinfo G_rpdev_sdinfo[3];
EXPORT_SYMBOL(G_rpdev_sdinfo);

struct rpdev_sdresp G_rpdev_sdresp;
EXPORT_SYMBOL(G_rpdev_sdresp);
#endif

#ifdef RPMSG_DEBUG
static AMBA_RPMSG_STATISTIC_s *rpmsg_stat;
extern struct ambalink_shared_memory_layout ambalink_shm_layout;
#endif

static int table_size;

/* arm-trusted-firmware/include/lib/psci/psci.h */
#define PSCI_CPU_ON_AARCH64             (0xC4000003u)

/* amba-specific sip */
#define AMBA_SIP_BOOT_RTOS              (0x8200FF04u)
#define S_SWITCH_AARCH32                (2)

/* to compatible with old experimental threadx firmware download */
/* TODO: get from dtb */
#define AHBSP_DATA0                     (0xe0022000 + 0x6c)

/* [start] rtos boot physical address */
static int bootaddr = 0xD00000;
static int ambarella_set_bootaddr(const char *str, const struct kernel_param *kp)
{
	int retval;
	unsigned int value;

	param_set_uint(str, kp);
	retval = kstrtou32(str, 10, &value);

	bootaddr = value;
	printk("0x%x\n", bootaddr);

	return retval;
}
static struct kernel_param_ops param_ops_bootaddr = {
	.set = ambarella_set_bootaddr,
	.get = param_get_int,
};
module_param_cb(bootaddr, &param_ops_bootaddr, &(bootaddr), 0644);
/* [end] rtos boot physical address */

/* [start] rtos firmware path */
static char *firmware = "amba_ssp.bin";
static int ambarella_set_firmware(const char *val, const struct kernel_param *kp)
{
	const struct firmware *fw;
	void *virt;
	char *data = strstrip((char *)val);
	int ret;

	virt = phys_to_virt((phys_addr_t)bootaddr);
	ret = request_firmware_into_buf(&fw, data, NULL, virt, 0xa00000);
	printk("request_firmware(%s) returned %d", data, ret);
#if defined(MODULE)
	ambcache_inv_range(virt, fw->size);
#else
	dcache_clean_inval_poc((unsigned long)virt, fw->size);
#endif
	release_firmware(fw);

	return param_set_charp(data, kp);
}
static struct kernel_param_ops param_ops_firmware = {
	.set = ambarella_set_firmware,
	.get = param_get_charp,
	.free = param_free_charp,
};
module_param_cb(firmware, &param_ops_firmware, &firmware, 0644);
/* [end] rtos firmware path */

/* [start] rtos boot core number */
static int bootcore = 0x1;
static int ambarella_set_bootcore(const char *str, const struct kernel_param *kp)
{
	int retval;
	unsigned int value;
	struct arm_smccc_res res;
	extern void rtos_dummyEntry(void);
	phys_addr_t entry = virt_to_phys(rtos_dummyEntry);
	void __iomem *regs = ioremap(AHBSP_DATA0, 4*4);

	param_set_uint(str, kp);

	retval = kstrtou32(str, 10, &value);
	bootcore = value;

	writel(AMBA_SIP_BOOT_RTOS, regs);
	writel(bootaddr, regs + 4);
	writel(0, regs + 8);
	writel(S_SWITCH_AARCH32, regs + 12);

	iounmap(regs);

	arm_smccc_smc(PSCI_CPU_ON_AARCH64, bootcore, (u64)entry,
			AHBSP_DATA0,
			0, 0, 0, 0, &res);

	printk("PSCI_CPU_ON_AARCH64 0x%llx core %d\n", entry, bootcore);

	return retval;
}
static struct kernel_param_ops param_ops_bootcore = {
	.set = ambarella_set_bootcore,
	.get = param_get_int,
};
module_param_cb(bootcore, &param_ops_bootcore,
	&(bootcore), 0644);
/* [end] rtos boot core number */

static struct resource_table *gen_rsc_table_cortex(int *tablesz, int id)
{
	struct resource_table	*table;
	struct fw_rsc_hdr	        *hdr;
	struct fw_rsc_vdev	        *vdev;
	struct fw_rsc_vdev_vring        *vring;

	*tablesz 		= table_size;

	table			= kzalloc((*tablesz), GFP_KERNEL);
	table->ver              = 1;
	table->num              = 1;
	table->offset[0]        = sizeof(*table) + sizeof(u32) * table->num;

	hdr                     = (void*)table + table->offset[0];
	hdr->type               = RSC_VDEV;

        vdev                    = (void*)&hdr->data;
	vdev->id                = VIRTIO_ID_RPMSG;
	vdev->notifyid          = 124;
	vdev->dfeatures         = (1 << VIRTIO_RPMSG_F_NS);
	vdev->config_len	= 0;
	vdev->num_of_vrings     = 2;

	vring                   = (void*)&vdev->vring[0];
	vring->da               = aipc_shm_info[id].vring_host_to_clnt;
	vring->align		= PAGE_SIZE;
	vring->num              = MAX_RPMSG_NUM_BUFS >> 1;
	vring->notifyid		= 111;

	vring                   = (void*)&vdev->vring[1];
	vring->da               = aipc_shm_info[id].vring_clnt_to_host;
	vring->align            = PAGE_SIZE;
	vring->num              = MAX_RPMSG_NUM_BUFS >> 1;
	vring->notifyid		= 112;

	return table;
}

static struct resource_table *gen_rsc_table2_cortex(int *tablesz, int id)
{
	struct resource_table	*table;
	struct fw_rsc_hdr	        *hdr;
	struct fw_rsc_vdev	        *vdev;
	struct fw_rsc_vdev_vring        *vring;

	*tablesz 		= table_size;

	table			= kzalloc((*tablesz), GFP_KERNEL);
	table->ver              = 1;
	table->num              = 1;
	table->offset[0]        = sizeof(*table) + sizeof(u32) * table->num;

	hdr                     = (void*)table + table->offset[0];
	hdr->type               = RSC_VDEV;

        vdev                    = (void*)&hdr->data;
	vdev->id                = VIRTIO_ID_RPMSG;
	vdev->notifyid          = 123;
	vdev->dfeatures         = (1 << VIRTIO_RPMSG_F_NS);
	vdev->config_len	= 0;
	vdev->num_of_vrings     = 2;

	vring                   = (void*)&vdev->vring[0];
	vring->da               = aipc_shm_info[id].vring_host_to_clnt;
	vring->align		= PAGE_SIZE;
	vring->num              = MAX_RPMSG_NUM_BUFS >> 1;
	vring->notifyid		= 111;

	vring                   = (void*)&vdev->vring[1];
	vring->da               = aipc_shm_info[id].vring_clnt_to_host;
	vring->align            = PAGE_SIZE;
	vring->num              = MAX_RPMSG_NUM_BUFS >> 1;
	vring->notifyid		= 112;

	return table;
}

static void rpmsg_set_irq(struct ambarella_rproc_pdata *pdata, unsigned long irq)
{
	if (pdata) {
		struct irq_data *idata = irq_get_irq_data(pdata->rvq_rx_irq);
		regmap_write_bits(pdata->reg_ahb_scr,
				pdata->sp_swi_set_offset,
				0x1 << (irq - (idata->hwirq)),
				0x1 << (irq - (idata->hwirq)));
	}
}

static void rpmsg_clr_irq(struct ambarella_rproc_pdata *pdata, unsigned long irq)
{
	if (pdata) {
		struct irq_data *idata = irq_get_irq_data(pdata->rvq_rx_irq);
		regmap_write(pdata->reg_ahb_scr,
				pdata->sp_swi_clear_offset,
				0x1 << (irq - (idata->hwirq)));
	}
}


static void ambarella_rproc_kick(struct rproc *rproc, int vqid)
{
	struct ambarella_rproc_pdata *pdata = rproc->priv;

	if (vqid == 0) {
		pr_debug("\e[1;32m ack rvq %d\n \e[0m", pdata->rvq_tx_irq);
		rpmsg_set_irq(pdata, pdata->rvq_tx_irq);
	} else {
		pr_debug("\e[1;32m kick svq %d\n \e[0m", pdata->svq_tx_irq);
		rpmsg_set_irq(pdata, pdata->svq_tx_irq);
	}
}

static void rproc_svq_worker(struct work_struct *work)
{
	struct ambarella_rproc_pdata *pdata;
	struct rproc *rproc;

	pdata = container_of(work, struct ambarella_rproc_pdata, svq_work);
	rproc = pdata->rproc;

	rproc_vq_interrupt(rproc, 1);
}

static void rproc_rvq_worker(struct work_struct *work)
{
	struct ambarella_rproc_pdata *pdata;
	struct rproc *rproc;
	struct rproc_vring *rvring;

	pdata = container_of(work, struct ambarella_rproc_pdata, rvq_work);
	rproc = pdata->rproc;
	rvring = idr_find(&pdata->rproc->notifyids, 0);

	while (1) {

		if (rproc_vq_interrupt(rproc, 0) == IRQ_HANDLED)
			continue;

		if (rvring && rvring->vq) {
			virtqueue_enable_cb(rvring->vq);
		}

		if (rproc_vq_interrupt(rproc, 0) == IRQ_HANDLED) {

		if (rvring && rvring->vq) {
			virtqueue_disable_cb(rvring->vq);
		}
			continue;
		}

		break;
	}
}

static irqreturn_t rproc_ambarella_isr(int irq, void *dev_id)
{
	struct ambarella_rproc_pdata *pdata = dev_id;
	struct rproc_vring *rvring;
        struct irq_data *idata = NULL;

	if (irq == pdata->rvq_rx_irq) {
		pr_debug("\e[1;32m enter rvq_rx_irq %d\n \e[0m", irq);
#ifdef RPMSG_DEBUG
		rpmsg_stat->LxRvqIsrCount++;
#endif
                idata = irq_get_irq_data(pdata->rvq_rx_irq);
		rvring = idr_find(&pdata->rproc->notifyids, 0);
		virtqueue_disable_cb(rvring->vq);
		schedule_work(&pdata->rvq_work);
	} else if (irq == pdata->svq_rx_irq) {
		pr_debug("\e[1;32m enter svq_rx_irq %d\n \e[0m", irq);
	        idata = irq_get_irq_data(pdata->svq_rx_irq);
		schedule_work(&pdata->svq_work);
	} else {
                BUG();
	}

	rpmsg_clr_irq(pdata, idata->hwirq);

	return IRQ_HANDLED;
}

static int ambarella_rproc_start(struct rproc *rproc)
{
	return 0;
}

static int ambarella_rproc_stop(struct rproc *rproc)
{
	return 0;
}

static int ambarella_rproc_dummy_load_segments(struct rproc *rproc,
				     const struct firmware *fw)
{
	return 0;
}

static int ambarella_rproc_dummy_find_rsc_table(struct rproc *rproc, const struct firmware *fw)
{
	int tablesz;
	struct ambarella_rproc_pdata *pdata = rproc->priv;
	struct resource_table *table;

	table = pdata->gen_rsc_table(&tablesz, pdata->shm_id);

	rproc->table_sz = tablesz;
	rproc->cached_table = kmemdup(table, tablesz, GFP_KERNEL);
	rproc->table_ptr = rproc->cached_table;

	return 0;
}

static struct rproc_ops ambarella_rproc_ops = {
	.start			= ambarella_rproc_start,
	.stop			= ambarella_rproc_stop,
	.kick			= ambarella_rproc_kick,
	.load			= ambarella_rproc_dummy_load_segments,
	.find_loaded_rsc_table	= NULL,
	.parse_fw		= ambarella_rproc_dummy_find_rsc_table,
	.sanity_check		= NULL,
	.get_boot_addr		= NULL
};

static struct ambarella_rproc_pdata pdata_cortex = {
	.name           = "ambarella",
	.firmware       = "ambadummy",
	.ops            = NULL,
	.gen_rsc_table  = gen_rsc_table_cortex,
};

static struct ambarella_rproc_pdata pdata2_cortex = {
	.name           = "c1_and_c2",
	.firmware       = "ambadummy",
	/* .svq_tx_irq	= VRING_IRQ_C1_TO_C2_KICK,*/
	/* .rvq_tx_irq	= VRING_IRQ_C1_TO_C2_ACK, */
	.ops            = NULL,
	.gen_rsc_table  = gen_rsc_table2_cortex,
};

static const struct of_device_id ambarella_rproc_of_match[] = {
	{ .compatible = "ambarella,rproc", .data = &pdata_cortex, },
	{ .compatible = "c1_and_c2,rproc", .data = &pdata2_cortex, },
	{},
};

static int get_shm_id(const char *name)
{
	const char *rproc_names[AMBALINK_NUM] = {"ambarella", "c1_and_c2"};
	int i;
	for (i=0; i<AMBALINK_NUM; i++) {
		if (!strcmp(rproc_names[i], name)) {
			return i;
		}
	}

	return -1;
}

static int ambarella_rproc_probe(struct platform_device *pdev)
{
	const struct of_device_id *of_id;
	struct ambarella_rproc_pdata *pdata;
	struct rproc *rproc;
	int ret;
	struct device *dev = &pdev->dev;
	char irq_name[100];
	u32 val;

	/* generate a shared rsc table. */
	table_size = sizeof(struct resource_table) + sizeof(u32) * 1
		     + sizeof(struct fw_rsc_hdr) + sizeof(struct fw_rsc_vdev)
		     + sizeof(struct fw_rsc_vdev_vring) * 2;

	//table = kzalloc(table_size, GFP_KERNEL);

	ret = dma_set_coherent_mask(&pdev->dev, DMA_BIT_MASK(64));
	if (ret) {
		dev_err(pdev->dev.parent, "dma_set_coherent_mask: %d\n", ret);
		return ret;
	}

	/* axi software irq2 */
	ret = of_property_read_u32(dev->of_node, "amb,svq_kick", &val);
	if (ret < 0) {
		dev_err(pdev->dev.parent, "Can not get amb,svq_kick\n");
		return ret;
	}
	val += AXI_SPI_BASE;
	pdata_cortex.svq_tx_irq	= val;
	pr_debug("svq_tx_irq %d\n", pdata_cortex.svq_tx_irq);

	/* axi software irq3 */
	ret = of_property_read_u32(dev->of_node, "amb,rvq_ack", &val);
	if (ret < 0) {
		dev_err(pdev->dev.parent, "Can not get amb,rvq_ack\n");
		return ret;
	}
	val += AXI_SPI_BASE;    /* linux ack  rvq -> rtos svq */
	pdata_cortex.rvq_tx_irq = val;
	pr_debug("rvq_tx_irq %d\n", pdata_cortex.rvq_tx_irq);


	/* set scratchpad sw interrupt   */
	ret = of_property_read_u32(dev->of_node, "amb,scr-swiset", &val);
	if (ret < 0) {
		dev_err(pdev->dev.parent, "Can not get amb,scr-swiset\n");
		return ret;
	}
	pdata_cortex.sp_swi_set_offset = val;
	pr_debug("sp_swi_set_offset 0x%x\n", pdata_cortex.sp_swi_set_offset);

	/* clear scratchpad sw interrupt */
	ret = of_property_read_u32(dev->of_node, "amb,scr-swiclear", &val);
	if (ret < 0) {
		dev_err(pdev->dev.parent, "Can not get amb,scr-swiset\n");
		return ret;
	}
	pdata_cortex.sp_swi_clear_offset = val;
	pr_debug("sp_swi_clear_offset 0x%x\n", pdata_cortex.sp_swi_clear_offset);

	of_id = of_match_node(ambarella_rproc_of_match, dev->of_node);
	pdata = (struct ambarella_rproc_pdata *) of_id->data;
	pdata->shm_id 		= get_shm_id(pdata->name);
	pdata->buf_addr_pa      = aipc_shm_info[pdata->shm_id].vring_host_and_clnt_buf;

	if (!pdata->buf_addr_pa) {
		printk(KERN_ERR "%s: guest%d shared memory is NULL, skip %s\n", __func__, pdata->shm_id + 1, pdata->name);
		return -ENOMEM;
	}

	rproc = rproc_alloc(&pdev->dev, pdata->name, &ambarella_rproc_ops,
			    pdata->firmware, sizeof(*rproc));
	if (!rproc) {
		ret = -ENOMEM;
		goto free_rproc;
	}

	dev_info(&pdev->dev, "%s device registered\n", __func__);

#if 0
        pdata->svq_rx_irq = platform_get_irq(pdev, 1);
        if (pdata->svq_rx_irq < 0) {
                dev_err(&pdev->dev, "no irq for svq_rx_irq!\n");
                ret = -ENODEV;
                goto free_rproc;
        }
#ifdef RPMSG_DEBUG
	rpmsg_stat = (AMBA_RPMSG_STATISTIC_s *) \
				ambalink_phys_to_virt(ambalink_shm_layout.rpmsg_profile_addr);
#endif
	strcpy(irq_name, pdata->name);
	strcat(irq_name, "-svq_rx");
	ret = request_irq(pdata->svq_rx_irq, rproc_ambarella_isr,
			IRQF_SHARED | IRQF_TRIGGER_HIGH,
			irq_name, pdata);
	if (ret) {
		dev_err(&pdev->dev, "request_irq error: %d\n", ret);
		goto free_rproc;
	}
#endif

        pdata->rvq_rx_irq = platform_get_irq(pdev, 0);
        if (pdata->rvq_rx_irq < 0) {
                dev_err(&pdev->dev, "no irq for rvq_rx_irq!\n");
                ret = -ENODEV;
                goto free_rproc;
        }

        strcpy(irq_name, pdata->name);
        strcat(irq_name, "-rvq_rx");
        ret = request_irq(pdata->rvq_rx_irq, rproc_ambarella_isr,
			IRQF_SHARED | IRQF_TRIGGER_HIGH,
			irq_name, pdata);
	if (ret) {
		dev_err(&pdev->dev, "request_irq error: %d\n", ret);
		goto free_rproc;
	}

        pdata->reg_ahb_scr = syscon_regmap_lookup_by_phandle(pdev->dev.of_node,
                                                        "amb,scr-regmap");
        if (IS_ERR(pdata->reg_ahb_scr)) {
                printk(KERN_ERR "no scr regmap!\n");
                ret = PTR_ERR(pdata->reg_ahb_scr);
        }

	INIT_WORK(&pdata->svq_work, rproc_svq_worker);
	INIT_WORK(&pdata->rvq_work, rproc_rvq_worker);

	mutex_init(&rproc->lock);

	rproc->priv = pdata;
	pdata->rproc = rproc;
	rproc->has_iommu = false;

	platform_set_drvdata(pdev, rproc);

	//rproc->fw_ops = &rproc_dummy_fw_ops;
	ret = rproc_add(rproc);
	if (ret)
		goto free_rproc;

	return 0;

free_rproc:
	rproc_put(rproc);
	return ret;
}

static int ambarella_rproc_remove(struct platform_device *pdev)
{
	struct rproc *rproc = platform_get_drvdata(pdev);
	struct ambarella_rproc_pdata *pdata = rproc->priv;

	//disable_irq(pdata->svq_rx_irq); //not used
	disable_irq(pdata->rvq_rx_irq);
	//free_irq(pdata->svq_rx_irq, pdata); //not used
	free_irq(pdata->rvq_rx_irq, pdata);

	/* TODO: release memory for table */

	rproc_del(rproc);
	rproc_free(rproc);

	return 0;
}

static struct platform_driver ambarella_rproc_driver = {
	.probe = ambarella_rproc_probe,
	.remove = ambarella_rproc_remove,
	.driver = {
	        .name = "ambarella-rproc",
                .of_match_table = ambarella_rproc_of_match,
	},
};

int __init ambarella_rproc_init(void)
{
	return platform_driver_register(&ambarella_rproc_driver);
}

void __exit ambarella_rproc_exit(void)
{
	platform_driver_unregister(&ambarella_rproc_driver);
}

device_initcall(ambarella_rproc_init);
module_exit(ambarella_rproc_exit);
//module_platform_driver(ambarella_rproc_driver);

MODULE_DESCRIPTION("Ambarella Remote Processor control driver");
MODULE_LICENSE("GPL");
