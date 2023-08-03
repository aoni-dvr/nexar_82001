#include "AmbaTypes.h"
#include "AmbaDef.h"

static UINT32 RetVal_PllGetNumCpuCycleMs = 0;

UINT32 AmbaCSL_PllGetNumCpuCycleMs(void)
{
    return RetVal_PllGetNumCpuCycleMs;
}

