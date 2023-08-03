
#ifndef __BST_UART_H__
#define __BST_UART_H__

#define CONFIG_AMBST_DEBUG
#ifdef CONFIG_AMBST_DEBUG
extern void bst_uart_init(void);
extern void bst_uart_putchar(char c);
extern void bst_uart_puthex(u32 h, int b);
extern void bst_uart_putstr(char *str);
#else
static inline void bst_uart_init(void) {}
static inline void bst_uart_putchar(char c) {}
static inline void bst_uart_puthex(u32 h, int b) {}
static inline void bst_uart_putstr(char *str) {}
#endif

#endif

