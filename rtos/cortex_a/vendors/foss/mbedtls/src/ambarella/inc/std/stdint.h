/*
 *  Dummy header when not using standard libraries from toolchain.
 */

//#ifndef CONFIG_CC_USESTD
#ifndef STDIN_H
#define STDIN_H

#define AMBA_ERROR_CODE_H  // Prevent to define conflicts, ERR_ARG
#include <AmbaWrap.h>

typedef UINT8               boolean;

typedef UINT8               uint8_t;
typedef UINT16              uint16_t;
typedef UINT32              uint32_t;
typedef UINT64              uint64_t;

typedef INT8                int8_t;
typedef INT16               int16_t;
typedef INT32               int32_t;
typedef INT64               int64_t;

#define SIZE_MAX 0xFFFFFFFF

#endif /* !STDIN_H */
//#endif

