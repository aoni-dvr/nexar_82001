#include "AmbaTypes.h"
#include "AmbaINT.h"

static UINT32 RetVal_RTSL_GicIntConfig = 0U;
static UINT32 RetVal_RTSL_GicIntEnable = 0U;
static UINT32 RetVal_RTSL_GicIntDisable = 0U;

void Set_RetVal_RTSL_GicIntConfig(UINT32 RetVal)
{
    RetVal_RTSL_GicIntConfig = RetVal;
}

void Set_RetVal_RTSL_GicIntEnable(UINT32 RetVal)
{
    RetVal_RTSL_GicIntEnable = RetVal;
}

void Set_RetVal_RTSL_GicIntDisable(UINT32 RetVal)
{
    RetVal_RTSL_GicIntDisable = RetVal;
}

UINT32 AmbaRTSL_GicIntConfig(UINT32 IntID, const AMBA_INT_CONFIG_s *pIntConfig, AMBA_INT_ISR_f IntFunc, UINT32 IntFuncArg)
{
    AMBA_INT_ISR_f AmbaIsr;
    AmbaIsr = IntFunc;

    if (AmbaIsr != NULL) {
        AmbaIsr(IntID, IntFuncArg);
    
        return RetVal_RTSL_GicIntConfig;
    
    } else {
        return 0U;
    }
}

UINT32 AmbaRTSL_GicIntEnable(UINT32 IntID)
{
    return RetVal_RTSL_GicIntEnable;
}

UINT32 AmbaRTSL_GicIntDisable(UINT32 IntID)
{
    return RetVal_RTSL_GicIntDisable;
}


