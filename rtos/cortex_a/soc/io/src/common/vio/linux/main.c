// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
 *  @file main.c
 *
 *  @copyright Copyright (c) 2022 Ambarella International LP
 *
 *  @details VIN/VOUT helper module
 *
 */

#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

static irqreturn_t amba_vio_intr_handler(int irq, void *data)
{
    return IRQ_HANDLED;
}

static int amba_vio_request_irq(struct platform_device *pdev,
                                const char *compatible, irq_handler_t handler)
{
    int i, irq, ret;
    struct device *dev = &pdev->dev;
    struct device_node *np = pdev->dev.of_node;
    const char *propname = "interrupt-names", *irq_name;

    for_each_compatible_node(np, NULL, compatible) {
        int count = of_property_count_strings(np, propname);

        if (count <= 0) {
            dev_err(dev, "[%s] Count %s failed\n", np->name, propname);
            break;
        }

        for (i = 0; i < count; i++) {
            irq = irq_of_parse_and_map(np, i);

            ret = of_property_read_string_index(np, propname, i, &irq_name);
            if (ret != 0) {
                dev_err(dev, "Get %s %s[%d] failed\n", np->name, propname, i);
                break;
            }

            ret = devm_request_irq(dev, irq, handler, 0, irq_name, dev);
            if (ret) {
                dev_err(dev, "[%s] Request irq[%d] failed\n", np->name, i);
                return ret;
            }
        }
    }

    return ret;
}

static int amba_vio_probe(struct platform_device *pdev)
{
    int ret;

    /* request vin interrupts */
    ret = amba_vio_request_irq(pdev, "ambarella,dspvin", amba_vio_intr_handler);
    if (ret) {
        return ret;
    }

    /* request vout interrupts */
    ret = amba_vio_request_irq(pdev, "ambarella,dspvout", amba_vio_intr_handler);
    if (ret) {
        return ret;
    }

    return 0;
}

static int amba_vio_remove(struct platform_device *pdev)
{
    return 0;
}

static const struct of_device_id amba_vio_of_ids[] = {
    { .compatible = "ambarella,dspdrv"},
    {},
};
MODULE_DEVICE_TABLE(of, amba_vio_of_ids);

static struct platform_driver amba_vio_platform_driver = {
    .probe = amba_vio_probe,
    .remove = amba_vio_remove,
    .driver = {
        .name = "amba-vio",
        .of_match_table = amba_vio_of_ids,
        .owner = THIS_MODULE,
    }
};

module_platform_driver(amba_vio_platform_driver);

MODULE_AUTHOR("Rice Cheng");
MODULE_LICENSE("GPL");
