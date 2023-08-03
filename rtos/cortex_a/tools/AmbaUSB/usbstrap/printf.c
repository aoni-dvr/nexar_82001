/**
 * Copyright (c) 2021 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <common.h>

#define va_list			__builtin_va_list
#define va_start(v,l)		__builtin_va_start(v,l)
#define va_end(v)		__builtin_va_end(v)
#define va_arg(v,l)		__builtin_va_arg(v,l)

#define FLAG_TYPE_UNDEF0	(1 << 0)	/* '#' */
#define FLAG_FILL_ZERO		(1 << 1)	/* '0' */
#define FLAG_ALIGN_LEFT		(1 << 2)	/* '-' */
#define FLAG_FILL_SPACE		(1 << 3)	/* ' ' */
#define FLAG_TYPE_UNDEF1	(1 << 4)	/* '+' */
#define FLAG_TYPE_UNDEF2	(1 << 5)	/* '+' */
#define FLAG_TYPE_UNDEF4	(1 << 6)	/* '?' */
#define FLAG_TYPE_UNDEF5	(1 << 7)	/* '?' */
#define FLAG_TYPE_SIGN		(1 << 8)	/* 'd' */
#define FLAG_TYPE_UNDEF6	(1 << 9)	/* 'o' */
#define FLAG_TYPE_UNSIGN	(1 << 10)	/* 'u' */
#define FLAG_TYPE_HEX		(1 << 11)	/* 'x' */
#define FLAG_TYPE_CHAR		(1 << 12)	/* 'c' */
#define FLAG_TYPE_STR		(1 << 13)	/* 's' */
#define FLAG_TYPE_BIT		(1 << 14)	/* 'b' */
#define FLAG_TYPE_UNDEF7	(1 << 15)
#define FLAG_TYPE_UNDEF8	(1 << 16)
#define FLAG_TYPE_UNDEF9	(1 << 17)
#define FLAG_TYPE_UNDEF10	(1 << 18)
#define FLAG_TYPE_UNDEF11	(1 << 19)
#define FLAG_TYPE_UNDEF12	(1 << 20)
#define FLAG_TYPE_UNDEF13	(1 << 21)
#define FLAG_TYPE_UNDEF14	(1 << 22)
#define FLAG_TYPE_UNDEF15	(1 << 23)
#define FLAG_TYPE_LONG		(1 << 24)	/* 'l' */
#define FLAG_TYPE_LLONG		(1 << 25)	/* 'L' */
#define FLAG_TYPE_INT		(1 << 26)	/* "default" */
#define FLAG_PARAM_WIDTH	(1 << 28)
#define FLAG_PARAM_LENGTH	(1 << 29)
#define FLAG_TYPE_MASK		(0xffff << 8)

#define STRING_CONV_SIZE	64
#define STRING_LOG_SIZE		64

static char *log_buf;
static char *conv_buf;
static const char *ascii = "0123456789abcdef";

static void serial_puts(const char *s)
{
	while(*s) {
		if(*s == '\n')
			uart_putchar('\r');
		uart_putchar(*s++);
	}
}

static int isdigit(int c)
{
	return '0' <= c && c <= '9';
}

/*
 * printf %[#0- +][0-9][lL][udxscb], args
 */

static const char *format_parse(const char *fmt,
		unsigned int *flag, unsigned int *l, unsigned int *w)
{
	unsigned int ltype = 0;
	/* man 3 printf */
	switch(*fmt) {
	case '#':
		fmt++;
		break;
	case '0':
		fmt++;
		*flag |= FLAG_FILL_ZERO;
		break;
	case '-':
		fmt++;
		*flag |= FLAG_ALIGN_LEFT;
		break;
	case ' ':
		fmt++;
		*flag |= FLAG_FILL_SPACE;
		break;
	case '+':
		fmt++;
		break;
	default:
		break;
	}

	if (isdigit(*fmt)) {
		while (isdigit(*fmt)){
			*w *= 10;
			*w += *fmt - '0';
			fmt++;
		}
	} else if ( *fmt == '*') {
		fmt++;
		if (*fmt == '.')
			*flag |= FLAG_PARAM_WIDTH;
	}

	if (*fmt == '.') {
		fmt ++;
		if (isdigit(*fmt)) {
			while (isdigit(*fmt)){
				*l *= 10;
				*l += *fmt - '0';
				fmt++;
			}
		} else if (*fmt == '*') {
			fmt++;
			*flag |= FLAG_PARAM_LENGTH;
		}
	}

	switch (*fmt) {
	case 'l':				/* %l */
		fmt++;
		ltype = 1;
		if (*fmt == 'l') {		/* %ll */
			fmt++;
			*flag |= FLAG_TYPE_LLONG;
		} else {
			*flag |= FLAG_TYPE_LONG;
		}
		break;
	case 'L':				/* %L */
		fmt++;
		ltype = 1;
		*flag |= FLAG_TYPE_LLONG;
		break;
	default:
		break;
	}

	switch (*fmt) {
	case 'u':
		if (!ltype)
			*flag |= FLAG_TYPE_INT;
		fmt++;
		*flag |= FLAG_TYPE_UNSIGN;
		break;
	case 'd':
		if (!ltype)
			*flag |= FLAG_TYPE_INT;
		fmt++;
		*flag |= FLAG_TYPE_SIGN;
		break;
	case 'x':
		if (!ltype)
			*flag |= FLAG_TYPE_INT;
		fmt++;
		*flag |= FLAG_TYPE_HEX;
		break;
	case 's':
		/* printf *.* */
		fmt++;
		*flag |= FLAG_TYPE_STR;
		break;
	case 'c':
		fmt++;
		*flag |= FLAG_TYPE_CHAR;
		break;
	case 'b':
		fmt++;
		*flag |= FLAG_TYPE_BIT;
		break;
	default:
		break;
	}

	return fmt;
}

static char *format_str(char *s, const char *argv,
		unsigned int length, unsigned int width)
{
	unsigned int padded_width;
	unsigned int slen = 0;
	const char fc = ' ';
	const char *str = argv;
	int i;


	while(*str && slen < 1024) {
		slen++;
		str++;
	}

	if (length)
		slen = min(length, slen);

	if (width)
		padded_width = width > slen ? width - slen : 0;
	else
		padded_width = 0;

	for (i = 0; i < padded_width; i++)
		*s++ = fc;

	for (i = 0; i < slen; i++)
		*s++ = *argv++;

	return s;
}

static char *format_hex(char *s, unsigned long long num,
		unsigned int flag, unsigned int width)
{
	int i;
	char fc = ' ';
	unsigned long long n = num;
	unsigned int actual_width = 0, padded_width;

	while (n) {
		conv_buf[actual_width++] = ascii[n % 16];
		n /= 16;
	}

	if (!actual_width)
		conv_buf[actual_width ++] = '0';

	padded_width =  width > actual_width ? width - actual_width : 0;

	if (FLAG_FILL_ZERO & flag)
		fc = '0';

	for(i = 0; i < padded_width; i++)
		*s++ = fc;

	for (i = actual_width - 1; i >= 0; i--)
		(*s ++ = conv_buf[i]);

	return s;

}

static char *format_dec(char *s, unsigned long long num,
		unsigned int flag, unsigned int width, unsigned int neg)
{
	int i;
	char fc = ' ';
	unsigned long long n = neg ? -num : num;
	unsigned int actual_width = 0, padded_width;

	while (n) {
		conv_buf[actual_width++] = ascii[n % 10];
		n /= 10;
	}

	if (!actual_width)
		conv_buf[actual_width ++] = '0';
	else if (neg)
		conv_buf[actual_width ++] = '-';


	padded_width =  width > actual_width ? width - actual_width : 0;

	if (!neg && (FLAG_FILL_ZERO & flag))
		fc = '0';

	for(i = 0; i < padded_width; i++)
		*s++ = fc;

	for (i = actual_width - 1; i >= 0; i--)
		(*s ++ = conv_buf[i]);

	return s;
}

static int vsprintf(char *buf, const char *fmt, va_list va)
{
	unsigned int flag, length, width;
	unsigned long long num = 0;
	const char *format = fmt;
	char *str = buf;
	unsigned int issign;
	unsigned int neg;

	while(*format) {
		if (*format != '%') {
			*str++ = *format++;
			continue;
		} else if (format[1] == '%') { /* Format '%%'*/
			*str++ = *format++;
			continue;
		}

		flag = 0;
		length = 0;
		width = 0;
		issign = 0;
		neg = 0;

		format++;
		format = format_parse(format, &flag, &length, &width);
		if (flag & FLAG_TYPE_SIGN)
			issign = 1;
		else
			issign = 0;

		if (!(flag & FLAG_TYPE_MASK))
			continue;

		if (flag & FLAG_PARAM_WIDTH)
			width = va_arg(va, unsigned int);

		if (flag & FLAG_PARAM_LENGTH)
			length = va_arg(va, unsigned int);

		width = min(width, STRING_CONV_SIZE);
		if (flag & FLAG_TYPE_LONG) {
			if (issign)
				num = va_arg(va, long);
			else
				num = va_arg(va, unsigned long);
			neg = ((long)num < 0) ? 1 : 0;
		} else if (flag & FLAG_TYPE_LLONG) {
			if (issign)
				num = va_arg(va, long long);
			else
				num = va_arg(va, unsigned long long);
			neg = ((long long)num < 0) ? 1 : 0;
		} else if (flag & FLAG_TYPE_INT){
			if (issign)
				num = va_arg(va, int);
			else
				num = va_arg(va, unsigned int);
			neg = ((int)num < 0) ? 1 : 0;
		}

		switch (flag & FLAG_TYPE_MASK) {
		case FLAG_TYPE_UNSIGN:
			neg = 0;
		case FLAG_TYPE_SIGN:
			str = format_dec(str, num, flag, width, neg);
			break;
		case FLAG_TYPE_HEX:
			str = format_hex(str, num, flag, width);
			break;
		case FLAG_TYPE_STR:
			str = format_str(str, va_arg(va, char *), length, width);
			break;
		case FLAG_TYPE_CHAR:
			break;
		case FLAG_TYPE_BIT:
			break;
		default:
			break;
		}
	}

	*str = '\0';
	return str - buf;
}

int printf(const char *format, ...)
{
	va_list args;

	if (!conv_buf)
		conv_buf = kmalloc(STRING_CONV_SIZE);
	if (!log_buf)
		log_buf = kmalloc(STRING_LOG_SIZE);

	va_start(args, format);
	vsprintf(log_buf, format, args);
	va_end(args);

	serial_puts(log_buf);

	return 0;
}
