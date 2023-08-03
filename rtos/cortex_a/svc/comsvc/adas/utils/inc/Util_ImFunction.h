#ifndef UTIL_IM_FUNCTION_H
#define UTIL_IM_FUNCTION_H

#include "AmbaTypes.h"
#include "AmbaErrorCode.h"

#define UTIL_IMF_OK   (OK)
#define UTIL_IMF_ERR_ARG  (ERR_ARG)
#define UTIL_IMF_ERR_IMPL (ERR_IMPL)
#define UTIL_IMF_ERR_NA   (ERR_NA)

#define UTIL_IMF_MAX_DEPTH_SIZE (3U)

#define UTIL_IMG_INTER_METHOD_LINEAR (0U)
#define UTIL_IMG_INTER_METHOD_NEAREST (1U)
#define UTIL_IMG_INTER_METHOD_UNDEFINE (2U)

typedef struct {
    UINT8* pBuf[UTIL_IMF_MAX_DEPTH_SIZE];
    UINT32 Stride;
    UINT32 Width;
    UINT32 Height;
    UINT32 Depth;
} UTIL_IMF_BUF_INFO_s;


UINT32 UtilIMF_Resize(const UINT8* src, UINT32 srcW, UINT32 srcH, UINT32 srcStride, UINT8* dst, UINT32 dstW, UINT32 dstH, UINT32 dstStride, UINT32 InterMethod);
UINT32 UtilIMF_ValueScaleU8(const UINT8 *src, UINT8 *dst, UINT32 width, UINT32 height, UINT32 stride, UINT32 srcL, UINT32 srcH, UINT32 dstL, UINT32 dstH);

#endif