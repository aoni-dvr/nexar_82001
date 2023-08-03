#ifdef CONFIG_CC_USESTD
#error "This is pseudo header when -nostdinc"
#else

#ifndef AMBA_STDWRAP_LIMITS_H
#define AMBA_STDWRAP_LIMITS_H

/* Number of bits for 'char' */
#undef CHAR_BIT
#define CHAR_BIT 8

#endif // AMBA_STDWRAP_LIMITS_H

#endif // CONFIG_CC_USESTD

