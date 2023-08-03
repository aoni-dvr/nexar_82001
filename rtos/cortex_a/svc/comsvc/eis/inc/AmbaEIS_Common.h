#ifndef AMBA_EIS_COMMON_H
#define AMBA_EIS_COMMON_H

#include <AmbaWrap.h>
#include "AmbaTypes.h"
#include "AmbaErrorCode.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaImg_ImuManager.h"
#include "AmbaSensor.h"
#include "AmbaShell.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaEIS_SystemApi.h"
#include "AmbaEIS_Control.h"



#define LOG_EIS_COM            "EIS_Common"




UINT32 AmbaEISCommon_UnitTest(const AMBA_EISCTRL_WINPARM_s *pWindow, const AMBA_EISCTRL_SENSOR_s *pSensorInfo,const AMBA_EISCTRL_IMU_INFO_s ImuInfo, AMBA_EISCTRL_SYS_INFO_s SystemInfo, AMBA_EISCTRL_USER_DATA_s *pEisResInfo);
void AmbaEISCommon_TestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);

#endif
