#ifndef __SVC_USER_PREF_CUSTOM_H__
#define __SVC_USER_PREF_CUSTOM_H__

#include "imu.h"

#define SVC_USER_PREF_CUSTOM_MAGIC_CODE 0x5a5a5a5a

typedef struct {
    unsigned int MagicCode;
    IMU_CALIBRATION_DATA_s ImuCalibrationData;
}  SVC_USER_PREF_CUSTOM_s;

UINT32 SvcUserPrefCustom_Init(void);
UINT32 SvcUserPrefCustom_Get(SVC_USER_PREF_CUSTOM_s** ppSvcUserPrefCustom);
void SvcUserPrefCustom_Save(void);

#endif /* __SVC_USER_PREF_CUSTOM_H__ */

