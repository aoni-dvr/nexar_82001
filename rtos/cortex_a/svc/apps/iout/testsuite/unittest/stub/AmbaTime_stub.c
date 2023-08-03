#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaTime.h"

static UINT32 RetVal_TaiDateTime2TimeStamp = ERR_NONE;
static UINT32 RetVal_TaiTimeStamp2DateTime = ERR_NONE;

void Set_RetVal_TaiDateTime2TimeStamp(UINT32 RetVal)
{
    RetVal_TaiDateTime2TimeStamp = RetVal;
}

void Set_RetVal_TaiTimeStamp2DateTime(UINT32 RetVal)
{
    RetVal_TaiTimeStamp2DateTime = RetVal;
}

void AmbaTime_Init(void){}

UINT32 AmbaTime_TaiDateTime2TimeStamp(const AMBA_RTC_DATE_TIME_s * pDateTime, UINT32 * pTimeStamp)
{
    return RetVal_TaiDateTime2TimeStamp;
}

UINT32 AmbaTime_TaiTimeStamp2DateTime(UINT32 TimeStamp, AMBA_RTC_DATE_TIME_s * pDateTime)
{
    return RetVal_TaiTimeStamp2DateTime;
}


