#ifdef CONFIG_CC_USESTD
#error "This is pseudo header when -nostdinc"
#else

#include "stddef.h"

#ifndef AMBA_STDWRAP_STRINT_H
#define AMBA_STDWRAP_STRINT_H

typedef signed char         int8_t;         /* 8 bits, [-128, 127] */
typedef short               int16_t;        /* 16 bits */
typedef int                 int32_t;        /* 32 bits */
typedef long long           int64_t;        /* 64 bits */

typedef unsigned char       uint8_t;        /* 8 bits, [0, 255] */
typedef unsigned short      uint16_t;       /* 16 bits */
typedef unsigned int        uint32_t;       /* 32 bits */
typedef unsigned long long  uint64_t;       /* 64 bits */

typedef unsigned short      wchar;          /* 16 bits */
typedef long unsigned int   size_t;

typedef int                intptr_t;
typedef unsigned int       uintptr_t;

#endif // AMBA_STDWRAP_STRINT_H

#endif // CONFIG_CC_USESTD

