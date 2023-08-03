#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaRTSL_GIC.h"

static UINT32 RetVal_GicIntConfig = 0;
static UINT32 RetVal_GicIntEnable = 0;
static UINT32 RetVal_GicIntDisable = 0U;
static UINT32 RetVal_GicGetIntInfo = 0U;
static UINT32 RetVal_GicSendSGI = 0U;
static UINT32 RetVal_GicIntGetGlobalState = 0U;

UINT32 AmbaRTSL_GicIntConfig(UINT32 IntID, const AMBA_INT_CONFIG_s *pIntConfig, AMBA_INT_ISR_f IntFunc, UINT32 IntFuncArg)
{
    return RetVal_GicIntConfig;
}

UINT32 AmbaRTSL_GicIntEnable(UINT32 IntID)
{
    return RetVal_GicIntEnable;
}

UINT32 AmbaRTSL_GicIntDisable(UINT32 IntID)
{
    return RetVal_GicIntDisable;
}

UINT32 AmbaRTSL_GicGetIntInfo(UINT32 IntID, AMBA_INT_INFO_s *pIntInfo)
{
    return RetVal_GicGetIntInfo;
}

void AmbaRTSL_GicIntGlobalEnable(void)
{
}

void AmbaRTSL_GicIntGlobalDisable(void)
{
}

UINT32 AmbaRTSL_GicSendSGI(UINT32 IntID, UINT32 IrqType, UINT32 SgiType, UINT32 CpuTargetList)
{
    return RetVal_GicSendSGI;
}

UINT32 AmbaRTSL_GicIntGetGlobalState(void)
{
    return RetVal_GicIntGetGlobalState;
}

