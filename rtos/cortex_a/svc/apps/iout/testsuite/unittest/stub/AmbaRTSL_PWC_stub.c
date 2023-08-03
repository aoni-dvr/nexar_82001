#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaSYS.h"

#include "AmbaRTSL_PWC.h"
#include "AmbaCSL_RCT.h"
#include "AmbaCSL_RTC.h"
#include "AmbaCSL_USB.h"
#include "AmbaCSL_WDT.h"

static UINT32 RetVal_PwcCheckSwReset  = ERR_NONE;
static UINT32 RetVal_PwcGetBootDevice = ERR_NONE;

void Set_RetVal_PwcGetBootDevice(UINT32 RetVal)
{
    RetVal_PwcGetBootDevice = RetVal;
}

UINT32 AmbaRTSL_PwcCheckSwReset(void)
{
    return RetVal_PwcCheckSwReset;
}

void AmbaRTSL_PwcReboot(void)
{
}

UINT32 AmbaRTSL_PwcGetBootDevice(void)
{
    return RetVal_PwcGetBootDevice;
}

