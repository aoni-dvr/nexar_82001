#include "AmbaDramTrain.h"
#include "AmbaDramTrainingLog.h"
#include "hwio.h"

#define CONFIG_UART_BAUDRATE 115200
#define CONFIG_UART_PORT 0
#define UART_PLL_DIV 1
#define UART_CONSOLE		CONFIG_UART_PORT
#define UART_PLL_DIV		1
#define RCT_BASE 0x20ED080000UL
#define IOMUX_BASE 0x20E4010000UL
#define UART0_BASE 0x20E4000000UL
#define UART1_BASE              0x20E0017000UL
#define UART2_BASE              0x20E0018000UL
#define UART3_BASE              0x20E0019000UL
#define UART4_BASE              0x20E001A000UL
#define REF_CLK_FREQ   24000000
#define ARRAY_SIZE(x)          (sizeof(x) / sizeof((x)[0]))
extern u32_t debug_level;

#define CG_UART_OFFSET(port)	\
	((port) == 0 ? 0x038 :	\
	 (port) == 1 ? 0x714 :	\
	 (port) == 2 ? 0x718 :	\
	 (port) == 3 ? 0x71c :	\
	 0x720)

static const unsigned int init_pins[][2] = {
	{10, 1},{11, 1},
};

void plat_set_uart_pll_div(int port, unsigned int div)
{
	writel(RCT_BASE + CG_UART_OFFSET(port), div);
}


void pinmux_init(const unsigned int (*pins)[2], int num)
{
	int i, j, pin, mux;

	for (i = 0; i < num; i++) {
		pin = pins[i][0];
		mux = pins[i][1];

		for (j = 0; j < 3; j++) {
			if (mux & 1 << j)
				setbitsl(IOMUX_BASE + (pin >> 5) * 0xc + j * 0x4,
						1 << (pin % 32));
			else
				clrbitsl(IOMUX_BASE + (pin >> 5) * 0xc + j * 0x4,
						1 << (pin % 32));
		}
	}

	writel(IOMUX_BASE + 0xF0, 0x1);
	writel(IOMUX_BASE + 0xF0, 0x0);
}

void plat_pinmux_init(void)
{
	pinmux_init(init_pins, ARRAY_SIZE(init_pins));
}

static void __uart_init(int baud)
{
	unsigned int clk;
	unsigned short dl;

	/* set uart pll divider */
	plat_set_uart_pll_div(UART_CONSOLE, UART_PLL_DIV);

	writeb(UART_REG(UART_CONSOLE, UART_SRR_OFFSET), 0x00);

	clk = REF_CLK_FREQ / UART_PLL_DIV;

	dl = clk * 10 / baud / 16;
	if (dl % 10 >= 5)
		dl = (dl / 10) + 1;
	else
		dl = (dl / 10);

	writeb(UART_REG(UART_CONSOLE, UART_LC_OFFSET), UART_LC_DLAB);
	writeb(UART_REG(UART_CONSOLE, UART_DLL_OFFSET), dl & 0xff);
	writeb(UART_REG(UART_CONSOLE, UART_DLH_OFFSET), dl >> 8);
	writeb(UART_REG(UART_CONSOLE, UART_LC_OFFSET), UART_LC_8N1);

}
static int __uart_poll(void)
{
	return !!(readb(UART_REG(UART_CONSOLE, UART_LS_OFFSET)) & UART_LS_DR);
}

static void __uart_putchar(char c)
{
	while (!(readb(UART_REG(UART_CONSOLE, UART_LS_OFFSET)) & UART_LS_TEMT));
	writeb(UART_REG(UART_CONSOLE, UART_TH_OFFSET), c);
}

static int __uart_getchar(void)
{
	while (!(__uart_poll()));

	return readb(UART_REG(UART_CONSOLE, UART_RB_OFFSET));
}

static void __uart_flush_input(void)
{
	while (__uart_poll()) {
		readb(UART_REG(UART_CONSOLE, UART_RB_OFFSET));
	}
}

static void __uart_putstr(void *str)
{
	char *c = str;

	while (*c) {
		if (*c == '\n')
			__uart_putchar('\r');
		__uart_putchar(*c);
		c++;
	}
}

void uart_init(void)
{
	__uart_init(CONFIG_UART_BAUDRATE);
}

void uart_putchar(char c)
{
	__uart_putchar(c);
}

int uart_getchar(void)
{
	return __uart_getchar();
}

void uart_putstr(void *str)
{
	__uart_putstr(str);
}
void uart_flush_input(void)
{
	__uart_flush_input();
}

void print(void *str, u32_t no_newline, u32_t debug)
{
#ifndef DDRCT_SIM_ENV
  if (debug <= (debug_level & 0xf)) {
    uart_putstr(str);
    if (!no_newline) {
      uart_putstr("\n");
    }
  }
#endif
}

void printInt(u32_t in, int base, u32_t no_newline, u32_t debug) {
#ifndef DDRCT_SIM_ENV
  if (debug <= (debug_level & 0xf)) {
    int i;
    char pb[9];
    for(i=7; i>=0; i--) {
      char rm = (in%base);
      if(rm < 10){rm += '0';}
      else {rm += 'A' - 10;}
      pb[i]=rm;
      in = in/base;
    }
    pb[8] = 0;
    uart_putstr(pb);
    if (!no_newline) {
      uart_putstr("\n");
    } else {
      uart_putstr(", ");
    }
  }
#endif
}
