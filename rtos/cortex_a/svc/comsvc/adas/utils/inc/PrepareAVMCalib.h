#ifndef PREPARE_AVM_CALIB_H
#define PREPARE_AVM_CALIB_H

#define PREP_AVM_CAL_CHANNEL_FRONT   (0U)
#define PREP_AVM_CAL_CHANNEL_BACK    (1U)
#define PREP_AVM_CAL_CHANNEL_LEFT    (2U)
#define PREP_AVM_CAL_CHANNEL_RIGHT   (3U)
#define PREP_AVM_CAL_CHANNEL_MAX     (4U)

#include "AmbaTypes.h"
#include "AmbaCalib_ErrNo.h"
#include "AmbaCalib_AVMIF.h"

void TEST_CASE(void);
UINT32 GetAvmCalibData(UINT32 Channel, char *pPath, AMBA_CAL_AVM_CALIB_DATA_s *pCalData, AMBA_CAL_AVM_PARK_AST_CFG_V1_s *pPaCfg);

#endif