#include "AmbaTypes.h"
#include "AmbaINT_Def.h"

UINT32 AmbaINT_Disable(UINT32 Irq)
{
    return 0;
}

UINT32 AmbaINT_Enable(UINT32 Irq)
{
    return 0;
}

UINT32 AmbaINT_Config(UINT32 Irq, const AMBA_INT_CONFIG_s *pIntConfig, AMBA_INT_ISR_f IntFunc, UINT32 IntFuncArg)
{
    UINT32 RetVal = 0U;
    UINT32 IntID = 0U;
    UINT32 IsrArg = 0U;

    if (IntFunc != NULL) {
        IntFunc(IntID, IsrArg);
    }

    return RetVal;
}

