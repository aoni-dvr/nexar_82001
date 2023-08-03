#include "AmbaTypes.h"
#include "AmbaDef.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

static UINT32 RetVal_pow              = ERR_NONE;
static UINT32 RetVal_floor            = ERR_NONE;
static UINT32 RetVal_ceil             = ERR_NONE;
static UINT32 RetVal_memset           = ERR_NONE;
static UINT32 RetVal_memcpy           = ERR_NONE;
static UINT32 NoCopy_AmbaWrap_memset  = 0U;  /* 1U: directly return */
static UINT32 NoCopy_AmbaWrap_memcpy  = 0U;  /* 1U: directly return */

static UINT64 *safe_buffer;

void Set_RetVal_pow(UINT32 RetVal)
{
    RetVal_pow = RetVal;
}
void Set_RetVal_floor(UINT32 RetVal)
{
    RetVal_floor = RetVal;
}
void Set_RetVal_ceil(UINT32 RetVal)
{
    RetVal_ceil = RetVal;
}
void Set_RetVal_memset(UINT32 RetVal)
{
    RetVal_memset = RetVal;
}
void Set_RetVal_memcpy(UINT32 RetVal)
{
    RetVal_memcpy = RetVal;
}

void Set_NoCopy_AmbaWrap_memset(UINT32 NoCopy)
{
    NoCopy_AmbaWrap_memset = NoCopy;
}
void Set_NoCopy_AmbaWrap_memcpy(UINT32 NoCopy)
{
    NoCopy_AmbaWrap_memcpy = NoCopy;
}

static void safe_buffer_cpy(void * pDst)
{
    UINT64 *pDes = pDst;
    UINT64 *pSrc;

    if(safe_buffer == NULL) {
        safe_buffer = malloc(sizeof(UINT64));
    }
    pSrc = (UINT64)&safe_buffer;
    *pDes = *pSrc;
    return;
}

UINT32 AmbaWrap_pow(DOUBLE base, DOUBLE exponent, void *pV)
{
    *(DOUBLE *)pV = pow(base, exponent);
    return RetVal_pow;
}

UINT32 AmbaWrap_floor(DOUBLE x, void *pV)
{
    *(DOUBLE *)pV = floor(x);
    return RetVal_floor;
}

UINT32 AmbaWrap_ceil(DOUBLE x, void *pV)
{
    *(DOUBLE *)pV = ceil(x);
    return RetVal_ceil;
}

UINT32 AmbaWrap_memset(void *ptr, INT32 v, SIZE_t n)
{
    if (NoCopy_AmbaWrap_memset == 0U) {
        memset(ptr, v, n);
    }
    return RetVal_memset;
}

UINT32 AmbaWrap_memcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    if (NoCopy_AmbaWrap_memcpy == 2U) {
        /* Because accessing source addr causes segment fault,
         * allocating other safe memory for gcovr test.
         */
        safe_buffer_cpy(pDst);
    } else if (NoCopy_AmbaWrap_memcpy == 0U) {
        memcpy(pDst, pSrc, num);
    }
    return RetVal_memcpy;
}
