#include "AmbaTypes.h"
#include "AmbaDef.h"

static UINT32 RetVal_SetVsdelaySrc = ERR_NONE;

void Set_RetVal_SetVsdelaySrc(UINT32 RetVal)
{
    RetVal_SetVsdelaySrc = RetVal;
}

UINT32 AmbaCSL_SetVsdelaySrc(UINT32 VsdelaySrc)
{
    return RetVal_SetVsdelaySrc;
}

void AmbaCSL_SetVsdelayWidth(const UINT32 HsPulseWidth, const UINT32 *VsPulseWidth)
{
}

