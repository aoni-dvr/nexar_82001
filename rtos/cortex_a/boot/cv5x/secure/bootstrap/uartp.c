#if 0
#include <basedef.h>
#include <ambhw/rct.h>
#include <ambhw/gpio.h>
#include <ambhw/uart.h>
#endif
#include "AmbaBST.h"
#include "AmbaTypes.h"
typedef UINT16      u16;
typedef UINT32      u32;
typedef UINT64      u64;
#include "uartp.h"
#define writeb(addr,value)              (*(volatile UINT8*)addr = value)
#define writel(addr,value)              (*(volatile UINT32*)addr = value)
#define readb(addr)                     (*(volatile UINT8*)addr)
#define readl(addr)                     (*(volatile UINT32*)addr)
#define REF_CLK_FREQ   24000000
#define ARRAY_SIZE(x)          (sizeof(x) / sizeof((x)[0]))
#define PINID_TO_BANK(p)	((p) >> 5)
#define PINID_TO_OFFSET(p)	((p) & 0x1f)

#define UART0_REG(x)		(AMBA_CORTEX_A76_UART_BASE_ADDR + (x))
#define UART1_REG(x)		(AMBA_CORTEX_A76_UART0_BASE_ADDR + (x))
#define UART2_REG(x)		(AMBA_CORTEX_A76_UART1_BASE_ADDR + (x))
#define UART3_REG(x)		(AMBA_CORTEX_A76_UART2_BASE_ADDR + (x))
#define UART4_REG(x)		(AMBA_CORTEX_A76_UART3_BASE_ADDR + (x))

#define UART_REG(port, x)	((port) == 0 ? UART0_REG(x) : \
				(port) == 1 ? UART1_REG(x) :\
				(port) == 2 ? UART2_REG(x) :\
				(port) == 3 ? UART3_REG(x) :\
				UART4_REG(x))

#define UART_TH_OFFSET			0x00
#define UART_DLL_OFFSET			0x00
#define UART_DLH_OFFSET			0x04
#define UART_LC_OFFSET			0x0c
#define UART_LS_OFFSET			0x14
#define UART_SRR_OFFSET			0x88

#define UART_LC_DLAB			0x80
#define	UART_LC_8N1			0x03
#define UART_LS_TEMT			0x40

#define MINIPIN_UART_ALTFUNC            1
#define MINIPIN_UART_PIN                {10, 11}
#define IOMUX_CTRL_SET_OFFSET           0xf0
#define IOMUX_REG_OFFSET(bank, n)      (((bank) * 0xc) + ((n) * 4))
#define IOMUX_REG(x)                    (AMBA_CORTEX_A76_IO_MUX_BASE_ADDR + (x))

#define RCT_REG(x)                      (AMBA_DBG_PORT_RCT_BASE_ADDR + (x))
#define CG_UART0_OFFSET                 0x38
#define CG_UART1_OFFSET                 0x714
#define CG_UART2_OFFSET                 0x718
#define CG_UART3_OFFSET                 0x71c
#define CG_UART4_OFFSET                 0x720
#define CG_UART5_OFFSET                 0x724
#define CG_UART6_OFFSET                 0x728
#define CG_UART0_REG                    RCT_REG(CG_UART0_OFFSET)
#define CG_UART1_REG                    RCT_REG(CG_UART1_OFFSET)
#define CG_UART2_REG                    RCT_REG(CG_UART2_OFFSET)
#define CG_UART3_REG                    RCT_REG(CG_UART3_OFFSET)
#define CG_UART4_REG                    RCT_REG(CG_UART4_OFFSET)
#define CG_UART5_REG                    RCT_REG(CG_UART5_OFFSET)
#define CG_UART6_REG                    RCT_REG(CG_UART6_OFFSET)
#define CG_UART_REG(n)                  ((n) == 0 ? CG_UART0_REG : \
                                         (n) == 1 ? CG_UART1_REG : \
                                         (n) == 2 ? CG_UART2_REG : \
                                         (n) == 3 ? CG_UART3_REG : \
                                         (n) == 4 ? CG_UART4_REG : \
                                         (n) == 5 ? CG_UART5_REG : \
                                                    CG_UART6_REG)

extern void rct_timer_delay(u32 delay_cnt);

#define UART_CONSOLE		0
#define BAUD_RATE		115200

//#define SHOW_TRAINING_TIME

void bst_uart_putchar(char c)
{
	while (!(readb(UART_REG(UART_CONSOLE, UART_LS_OFFSET)) & UART_LS_TEMT));
	writeb(UART_REG(UART_CONSOLE, UART_TH_OFFSET), c);
}

void bst_uart_puthex(u32 h, int b)
{
	char c;

	for (b = b - 1; b >= 0; b--) {
		c = (char) ((h >> (b * 4)) & 0xf);

		if (c > 9)
			c += ('A' - 10);
		else
			c += '0';
		bst_uart_putchar(c);
	}
}

void bst_uart_putstr(char *str)
{
	while (*str != '\0') {
		if (*str == '\n')
			bst_uart_putchar('\r');
		bst_uart_putchar(*str);
		str++;
	}
}

void bst_uart_init(void)
{
	u32 uart_pins[] = MINIPIN_UART_PIN, i, j, bank, offset, data;
	u16 dl;

	for (i = 0; i < ARRAY_SIZE(uart_pins); i++) {
		bank = PINID_TO_BANK(uart_pins[i]);
		offset = PINID_TO_OFFSET(uart_pins[i]);

		for (j = 0; j < 3; j++) {
			data = readl(IOMUX_REG(IOMUX_REG_OFFSET(bank, j)));
			data &= (~(0x1 << offset));
			data |= (((MINIPIN_UART_ALTFUNC >> j) & 0x1) << offset);
			writel(IOMUX_REG(IOMUX_REG_OFFSET(bank, j)), data);
		}
	}

	writel(IOMUX_REG(IOMUX_CTRL_SET_OFFSET), 0x1);
	writel(IOMUX_REG(IOMUX_CTRL_SET_OFFSET), 0x0);

	/* set uart pll divider to 1 */
	writel(CG_UART_REG(UART_CONSOLE), 1);

	writeb(UART_REG(UART_CONSOLE, UART_SRR_OFFSET), 0x00);

	dl = REF_CLK_FREQ * 10 / BAUD_RATE / 16;
	if (dl % 10 >= 5)
		dl = (dl / 10) + 1;
	else
		dl = (dl / 10);

	writeb(UART_REG(UART_CONSOLE, UART_LC_OFFSET), UART_LC_DLAB);
	writeb(UART_REG(UART_CONSOLE, UART_DLL_OFFSET), dl & 0xff);
	writeb(UART_REG(UART_CONSOLE, UART_DLH_OFFSET), dl >> 8);
	writeb(UART_REG(UART_CONSOLE, UART_LC_OFFSET), UART_LC_8N1);

	/* 40ns*100000=4ms should add some delay here */
	rct_timer_delay(0x20000);
}

#if defined(SHOW_TRAINING_TIME)
void bst_uart_putint(u32 in, int base)
{
	char rm = (in % base);
	u32 q = in / base;

	if (q != 0) {
		bst_uart_putint(q, base);       // Recursive call to do MSB first
	}
	if ((q != 0) || (rm != 0)) {
		if (rm < 10) {
			rm += '0';
		} else {
			rm += 'A' - 10;
		}
		bst_uart_putchar(rm);
	}
}

void training_time_start(void)
{
	writel(RCT_REG(RCT_TIMER_2_CTRL_REG), 1);   // Reset
	writel(RCT_REG(RCT_TIMER_2_CTRL_REG), 0);   // Enable
}

void training_time_show(void)
{
	UINT32 Count, TimeUs;
	writel(RCT_REG(RCT_TIMER_2_CTRL_REG), 2);       // Freeze
	Count = readl(RCT_REG(RCT_TIMER_2_COUNT_REG));  // read
	TimeUs = Count / 24000;

	bst_uart_putstr("Time=");
	bst_uart_putint(TimeUs, 10);
	bst_uart_putstr("ms.\n");
}
#else
void training_time_start(void) {}
void training_time_show(void) {}
#endif
