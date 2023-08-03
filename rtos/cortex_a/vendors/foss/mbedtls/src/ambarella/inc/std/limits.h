/*
 *  Dummy header when not using standard libraries from toolchain.
 */

//#ifndef CONFIG_CC_USESTD
#ifndef LIMITS_H
#define LIMITS_H

#define AMBA_ERROR_CODE_H  // Prevent to define conflicts, ERR_ARG
#include <AmbaWrap.h>

#define CHAR_BIT 8
#define UINT_MAX 0xFFFFFFFF
#define INT_MAX __INT_MAX__

#endif /* !LIMITS_H */
////#endif

