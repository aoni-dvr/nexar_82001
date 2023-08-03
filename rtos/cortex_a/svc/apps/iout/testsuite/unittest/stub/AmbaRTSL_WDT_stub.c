#include "AmbaTypes.h"

static UINT32 RetVal_WdtGetResetStatus = ERR_NONE;

void Set_RetVal_WdtGetResetStatus(UINT32 RetVal)
{
    RetVal_WdtGetResetStatus = RetVal;
}

UINT32 AmbaRTSL_WdtGetResetStatus(void)
{
    return RetVal_WdtGetResetStatus;
}

void AmbaRTSL_WdtCoreFreqChg(UINT32 ChId)
{
}
