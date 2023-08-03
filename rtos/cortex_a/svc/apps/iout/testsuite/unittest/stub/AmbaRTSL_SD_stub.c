#include "AmbaTypes.h"
#include "AmbaDef.h"

static UINT32 RetVal_SdSendCMD0     = ERR_NONE;

void Set_RetVal_SdSendCMD0(UINT32 RetVal)
{
    RetVal_SdSendCMD0 = RetVal;
}

void AmbaRTSL_SdClockEnable(UINT32 SdChanNo, UINT32 Enable)
{
}

INT32 AmbaRTSL_SdSendCMD0(UINT32 SdChanNo, UINT32 Cmd0Arg)
{
    return RetVal_SdSendCMD0;
}
