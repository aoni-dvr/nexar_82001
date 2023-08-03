#include "AmbaTypes.h"
#include "AmbaRTSL_Cache.h"

static UINT32 RetVal_CacheCleanData = 0U;

UINT32 AmbaRTSL_CacheCleanData(ULONG VirtAddr, ULONG Size)
{
    return RetVal_CacheCleanData;
}

