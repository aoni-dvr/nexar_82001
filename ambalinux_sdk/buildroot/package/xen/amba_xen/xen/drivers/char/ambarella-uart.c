/*
 * xen/drivers/char/ambarella-uart.c
 *
 * Driver for Ambarella UART.
 *
 * Copyright (C) 2020, Ambarella Inc.
 */

#include <xen/console.h>
#include <xen/errno.h>
#include <xen/serial.h>
#include <xen/init.h>
#include <xen/irq.h>
#include <xen/mm.h>
#include <asm/device.h>
#include <asm/io.h>
#include <asm/ambarella-uart.h>

static struct ambarella_uart {
    unsigned int irq;
    void __iomem *regs;
    struct irqaction irqaction;
    struct vuart_info vuart;
} ambarella_com = {0};

static void ambarella_uart_interrupt(int irq, void *data,
                                     struct cpu_user_regs *regs)
{
    struct serial_port *port = data;
    struct ambarella_uart *uart = port->uart;
    uint32_t iir;

    iir = readl(uart->regs + UART_IIR);
    switch (iir & 0xf) {
    case UART_IIR_RCV_DATA_AVAIL:
        serial_rx_interrupt(port, regs);
        break;
    default:
        break;
    }
}

static void __init ambarella_uart_init_preirq(struct serial_port *port)
{
    /* DO NOTHING (TODO) */
    /* UART controller assumed to be initialized by BLD or ATF */
}

static void __init ambarella_uart_init_postirq(struct serial_port *port)
{
    struct ambarella_uart *uart = port->uart;
    int rc;

    if (uart->irq > 0) {
        uart->irqaction.handler = ambarella_uart_interrupt;
        uart->irqaction.name = "ambarella_uart";
        uart->irqaction.dev_id = port;
        rc = setup_irq(uart->irq, 0, &uart->irqaction);
        if (rc != 0)
            printk("ERROR: failed to allocate UART IRQ %d\n", uart->irq);
    }

    /* Unmask interrupts */
    writel(UART_IER_ERBFI, uart->regs + UART_IER);
}

static void ambarella_uart_suspend(struct serial_port *port)
{
	BUG(); // XXX
}

static void ambarella_uart_resume(struct serial_port *port)
{
	BUG(); // XXX
}

static int ambarella_uart_tx_ready(struct serial_port *port)
{
    struct ambarella_uart *uart = port->uart;
    uint32_t usr;

    usr = readl(uart->regs + UART_USR);

    return (usr & UART_USR_TFNF) ? 1 : 0;
}

static void ambarella_uart_putc(struct serial_port *port, char c)
{
    struct ambarella_uart *uart = port->uart;

    writeb(c, uart->regs + UART_THR);
}

static int ambarella_uart_getc(struct serial_port *port, char *pc)
{
    struct ambarella_uart *uart = port->uart;

    *pc = (char) readl(uart->regs + UART_RBR) & 0xff;

    return 1;
}

static int __init ambarella_uart_irq(struct serial_port *port)
{
    struct ambarella_uart *uart = port->uart;

    return ((uart->irq > 0) ? uart->irq : -1);
}

static const struct vuart_info *ambarella_vuart_info(struct serial_port *port)
{
    struct ambarella_uart *uart = port->uart;

    return &uart->vuart;
}

static struct uart_driver __read_mostly ambarella_uart_driver = {
    .init_preirq  = ambarella_uart_init_preirq,
    .init_postirq = ambarella_uart_init_postirq,
    .endboot      = NULL,
    .suspend      = ambarella_uart_suspend,
    .resume       = ambarella_uart_resume,
    .tx_ready     = ambarella_uart_tx_ready,
    .putc         = ambarella_uart_putc,
    .getc         = ambarella_uart_getc,
    .irq          = ambarella_uart_irq,
    .vuart_info   = ambarella_vuart_info,
};

static int __init ambarella_uart_init(struct dt_device_node *dev,
                                      const void *data)
{
    const char *config = data;
    struct ambarella_uart *uart;
    int res;
    u64 addr, size;

    if (strcmp(config, ""))
	printk("WARNING: UART configuration is not supported\n");

    uart = &ambarella_com;

    res = dt_device_get_address(dev, 0, &addr, &size);
    if (res) {
	printk("ambarella: Unable to retrieve the base address of the UART\n");
	return res;
    }

    res = platform_get_irq(dev, 0);
    if (res < 0) {
	printk("ambarella: Unable to retrieve the IRQ\n");
	return -EINVAL;
    }

    uart->irq = res;
    uart->regs = ioremap_nocache(addr, size);
    if (!uart->regs) {
	printk("ambarella: Unable to map the UART memory\n");
	return -ENOMEM;
    }

    uart->vuart.base_addr = addr;
    uart->vuart.size = size;
    uart->vuart.data_off = 0x00;
    uart->vuart.status_off = 0x7c;
    uart->vuart.status = (1 << 2);

    /* Register with generic serial driver. */
    serial_register_uart(SERHND_DTUART, &ambarella_uart_driver, uart);

    dt_device_set_used_by(dev, DOMID_XEN);

    return 0;
}

static const struct dt_device_match ambarella_dt_match[] __initconst =
{
    DT_MATCH_COMPATIBLE("ambarella,uart"),
    { /* sentinel */ },
};

DT_DEVICE_START(ambarella, "ambarella uart", DEVICE_SERIAL)
	.dt_match = ambarella_dt_match,
	.init = ambarella_uart_init,
DT_DEVICE_END

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
