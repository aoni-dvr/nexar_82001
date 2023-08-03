/**
 * system/src/rpclnt/rpclnt.h
 *
 * History:
 *    2012/08/15 - [Tzu-Jung Lee] created file
 *
 * Copyright 2008-2015 Ambarella Inc.  All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/interrupt.h>
#include <linux/of_platform.h>
#include <linux/regmap.h>
#include <linux/of_irq.h>
#include <linux/mfd/syscon.h>

#include <plat/remoteproc_cfg.h>
#include <plat/rpdev.h>
#include <plat/chip.h>

#include "rpclnt.h"
#include "vq.h"

extern void rpdev_svq_cb(struct vq *vq);
extern void rpdev_rvq_cb(struct vq *vq);
extern struct ambalink_shared_memory_layout ambalink_shm_layout;

DECLARE_COMPLETION(rpclnt_comp);

static struct rpclnt G_rpclnt = {
	.name		= "rpclnt_linux",
	.inited		= 0,

	.svq_num_bufs	= MAX_RPMSG_NUM_BUFS >> 1,
	.svq_vring_algn	= RPMSG_VRING_ALIGN,

	.rvq_num_bufs	= MAX_RPMSG_NUM_BUFS >> 1,
	.rvq_vring_algn	= RPMSG_VRING_ALIGN,
};


struct rpclnt *rpclnt_sync(const char *bus_name)
{

	struct rpclnt *rpclnt = &G_rpclnt;

	if (strcmp(bus_name, "ca9_a_and_b"))
		return NULL;

	if (!rpclnt->inited)
		wait_for_completion(&rpclnt_comp);


	return rpclnt;
}

static void rpclnt_complete_registration(struct rpclnt *rpclnt)
{
	void *buf = (void *)rpclnt->rvq_buf_phys;

	vq_init_unused_bufs(rpclnt->rvq, buf, RPMSG_BUF_SIZE);

	rpclnt->inited = 1;
	complete_all(&rpclnt_comp);
}

static void rpmsg_send_irq(struct regmap *reg_ahb_scr, unsigned long irq)
{
#ifdef AXI_SOFT_IRQ2
	if (irq < AXI_SOFT_IRQ2(0))
		regmap_write_bits(reg_ahb_scr, AHB_SP_SWI_SET_OFFSET,
				0x1 << (irq - AXI_SOFT_IRQ(0)),
				0x1 << (irq - AXI_SOFT_IRQ(0)));
	else {
		regmap_write_bits(reg_ahb_scr, AHB_SP_SWI_SET_OFFSET1,
				0x1 << (irq - AXI_SOFT_IRQ2(0)),
				0x1 << (irq - AXI_SOFT_IRQ2(0)));
	}
#else
		regmap_write_bits(reg_ahb_scr, AHB_SP_SWI_SET_OFFSET,
			0x1 << (irq - AXI_SOFT_IRQ(0)),
			0x1 << (irq - AXI_SOFT_IRQ(0)));
#endif
}

static void rpmsg_ack_irq(struct regmap *reg_ahb_scr, unsigned long irq)
{
#ifdef AXI_SOFT_IRQ2
	if (irq < AXI_SOFT_IRQ2(0)+32)
		regmap_write(reg_ahb_scr, AHB_SP_SWI_CLEAR_OFFSET,
				0x1 << (irq - AXI_SOFT_IRQ(0)));
	else {
		regmap_write(reg_ahb_scr, AHB_SP_SWI_CLEAR_OFFSET1,
				0x1 << (irq - AXI_SOFT_IRQ2(0)));
	}
#else
	regmap_write(reg_ahb_scr, AHB_SP_SWI_CLEAR_OFFSET,
		0x1 << (irq - AXI_SOFT_IRQ(0)));
#endif
}

static irqreturn_t rpclnt_isr(int irq, void *dev_id)
{

	struct rpclnt *rpclnt = dev_id;
	struct irq_data *irq_data;

	irq_data = irq_get_irq_data(irq);

	if (!rpclnt->inited && irq == rpclnt->svq_rx_irq) {

		rpclnt_complete_registration(rpclnt);

		rpmsg_ack_irq(rpclnt->reg_ahb_scr, irq_data->hwirq);

		return IRQ_HANDLED;
	}

	/*
	 * Before scheduling the bottom half for processing the messages,
	 * we tell the remote host not to notify us (generate interrupts) when
	 * subsequent messages are enqueued.  The bottom half will pull out
	 * this message and the pending ones.  Once it processed all the messages
	 * in the queue, it will re-enable remote host for the notification.
	 */
	if (irq == rpclnt->rvq_rx_irq) {


		vq_disable_used_notify(rpclnt->rvq);
		schedule_work(&rpclnt->rvq_work);

	} else if (irq == rpclnt->svq_rx_irq) {


		vq_disable_used_notify(rpclnt->svq);
		schedule_work(&rpclnt->svq_work);
	}


	rpmsg_ack_irq(rpclnt->reg_ahb_scr, irq_data->hwirq);


	return IRQ_HANDLED;
}

static void rpclnt_rvq_worker(struct work_struct *work)
{

	struct rpclnt *rpclnt = container_of(work, struct rpclnt, rvq_work);
	struct vq *vq = rpclnt->rvq;

	if (vq->cb)
		vq->cb(vq);
}

static void rpclnt_svq_worker(struct work_struct *work)
{

	struct rpclnt *rpclnt = container_of(work, struct rpclnt, svq_work);
	struct vq *vq = rpclnt->svq;

	if (vq->cb)
		vq->cb(vq);
}

static void rpclnt_kick_rvq(void *data)
{

	struct rpclnt *rpclnt = data;
	struct vq *vq = rpclnt->rvq;
	unsigned long irq = rpclnt->rvq_tx_irq;
	/*
	 * Honor the flag set by the remote host.
	 *
	 * Most of the time, the remote host want to supress their
	 * tx-complete interrupts.  In this case, we don't bother it.
	 */
	if (vq_kick_prepare(vq)) {

		rpmsg_send_irq(rpclnt->reg_ahb_scr, irq);
	}
}

static void rpclnt_kick_svq(void *data)
{

	struct rpclnt *rpclnt = data;
	struct vq *vq = rpclnt->svq;
	unsigned long irq = rpclnt->svq_tx_irq;
	/*
	 * Honor the flag set by the remote host.
	 *
	 * When the remote host is already busy enough processing the
	 * messages, it might suppress the interrupt and work in polling
	 * mode.
	 */
	if (vq_kick_prepare(vq)) {

		rpmsg_send_irq(rpclnt->reg_ahb_scr, irq);
	}
}

static const struct of_device_id ambarella_rpclnt_of_match[] = {
	{ .compatible = "ambarella,rpclnt", .data = (void *)&G_rpclnt },
	{},
};
MODULE_DEVICE_TABLE(of, ambarella_rpclnt_of_match);

static int __init rpclnt_drv_init(struct device_node *np, struct rpclnt *rpclnt)
{
	int ret = 0;
	u32 val;

	/*--------*/
	rpclnt->inited = 0;

	if (of_property_read_u32(np, "amb,svq_kick", &val) < 0) {
		dev_err(&rpclnt->pdev->dev, "Can not get amb,svq_kick\n");
		return -EFAULT;
	} else {
		rpclnt->svq_tx_irq = val + AXI_SPI_BASE;
		dev_info(&rpclnt->pdev->dev, "amb,svq_kick: %d\n", rpclnt->svq_tx_irq);
	}

	if (of_property_read_u32(np, "amb,rvq_ack", &val) < 0) {
		dev_err(&rpclnt->pdev->dev, "Can not get amb,rvq_ack\n");
		return -EFAULT;
	} else {
		rpclnt->rvq_tx_irq = val + AXI_SPI_BASE;
		dev_info(&rpclnt->pdev->dev, "amb,rvq_ack: %d\n", rpclnt->rvq_tx_irq);
	}

	/*--------*/

	/*
	   Usage of RPMSG memory layout
	   ============================
	   Client   base    Host
	            Buffers --->
	       <--- Buffers
	            Ring_Descriptor --->
	            Ring_Avail
	            Ring_Used
	   ======== Padding ========
	       <--- Ring_Descriptor
	            Ring_Avail
	            Ring_Used
	 */
	rpclnt->svq_buf_phys	= ambalink_shm_layout.vring_host_and_clnt_buf;
	rpclnt->rvq_buf_phys	= ambalink_shm_layout.vring_host_and_clnt_buf + (RPMSG_TOTAL_BUF_SPACE >> 1);
	rpclnt->svq_vring_phys	= ambalink_shm_layout.vring_clnt_to_host;
	rpclnt->rvq_vring_phys	= ambalink_shm_layout.vring_host_to_clnt;

	dev_info(&rpclnt->pdev->dev, "(phy) rvq_buf: 0x%016lX, rvq_vring: 0x%016lX, svq_buf, 0x%016lX, svq_ring: 0x%016lX\n",
		rpclnt->rvq_buf_phys,
		rpclnt->rvq_vring_phys,
		rpclnt->svq_buf_phys,
		rpclnt->svq_vring_phys);

	rpclnt->svq = vq_create(rpdev_svq_cb,
				rpclnt_kick_svq,
				rpclnt->svq_num_bufs,
				phys_to_virt(rpclnt->svq_vring_phys),
				rpclnt->svq_vring_algn);
	rpclnt->rvq = vq_create(rpdev_rvq_cb,
				rpclnt_kick_rvq,
				rpclnt->rvq_num_bufs,
				phys_to_virt(rpclnt->rvq_vring_phys),
				rpclnt->rvq_vring_algn);
	dev_info(&rpclnt->pdev->dev, "svq: 0x%016X, rvq: 0x%016X\n",
			(unsigned int)(uintptr_t)rpclnt->svq,
			(unsigned int)(uintptr_t)rpclnt->rvq);

	INIT_WORK(&rpclnt->svq_work, rpclnt_svq_worker);
	INIT_WORK(&rpclnt->rvq_work, rpclnt_rvq_worker);

	rpclnt->reg_ahb_scr = syscon_regmap_lookup_by_phandle(np, "amb,scr-regmap");
	if (IS_ERR(rpclnt->reg_ahb_scr)) {
		printk(KERN_ERR "no scr regmap!\n");
		ret = PTR_ERR(rpclnt->reg_ahb_scr);
		return ret;
	}

	rpclnt->svq_rx_irq = irq_of_parse_and_map(np, 1);
	ret = request_irq(rpclnt->svq_rx_irq, rpclnt_isr,
			  IRQF_SHARED | IRQF_TRIGGER_HIGH, "rpclnt_svq_rx", rpclnt);
	if (ret) {
		printk(KERN_ERR "Error: failed to request svq_rx_irq: %d, err: %d\n",
		       rpclnt->svq_rx_irq, ret);
		return -EFAULT;
	}

	rpclnt->rvq_rx_irq = irq_of_parse_and_map(np, 0);
	ret = request_irq(rpclnt->rvq_rx_irq, rpclnt_isr,
			  IRQF_SHARED | IRQF_TRIGGER_HIGH, "rpclnt_rvq_rx", rpclnt);
	if (ret) {
		printk(KERN_ERR "Error: failed to request rvq_rx_irq: %d, err: %d\n",
		       rpclnt->rvq_rx_irq, ret);
		return -EFAULT;
	}

	return ret;
}

static int rpclnt_probe(struct platform_device *pdev)
{
	int ret;
	const struct of_device_id *of_id;

	of_id = of_match_node(ambarella_rpclnt_of_match, pdev->dev.of_node);
	if (of_id != NULL)  {
		struct rpclnt *dev_priv;
		struct device_node *np = pdev->dev.of_node;

		dev_priv = (struct rpclnt *)of_id->data;
		dev_priv->pdev = pdev;
		ret = rpclnt_drv_init(np, dev_priv);
		if (!ret) {
			platform_set_drvdata(pdev, dev_priv);
		} else {
			dev_priv->pdev = NULL;
		}
	} else {
		dev_err(&pdev->dev, "Missing rpclnt data.\n");
		printk("Missing rpclnt data.\n");
		ret = -EFAULT;
	}

        return ret;
}

static int __exit rpclnt_remove(struct platform_device *pdev)
{
#if 0
	struct rpclnt_device_priv *dev_priv = platform_get_drvdata(pdev);

	if (dev_priv) {
		vq_delete();
		free_irq();
	}
#endif

	return 0;
}




static struct platform_driver rpclnt_driver = {
	.probe		= rpclnt_probe,
	.remove		= rpclnt_remove,
#if 0
#ifdef CONFIG_PM
	.suspend	= NULL,
	.resume		= NULL,
#endif
#endif
	.driver = {
		.name = "rpclnt",
		.of_match_table = ambarella_rpclnt_of_match,
	},
};

static int __init rpclnt_init(void)
{
	int ret;

	ret = platform_driver_register(&rpclnt_driver);

	return ret;
}
device_initcall(rpclnt_init);

static void __exit rpclnt_exit(void)
{
	platform_driver_unregister(&rpclnt_driver);
}
module_exit(rpclnt_exit);

MODULE_DESCRIPTION("Ambarella client/remote of RPMSG/VQ");
MODULE_LICENSE("GPL");

