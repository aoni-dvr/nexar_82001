/**
 *  @file AmbaRTSL_VIN.h
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for Video Input Control APIs
 *
 */

#ifndef AMBA_RTSL_VIN_H
#define AMBA_RTSL_VIN_H

#ifndef AMBA_VIN_DEF_H
#include "AmbaVIN_Def.h"
#endif

/*
 * Defined in AmbaRTSL_VIN.c
 */

void AmbaRTSL_VinInit(void);
UINT32 AmbaRTSL_VinReset(UINT32 VinID, UINT8 ResetFlag);
UINT32 AmbaRTSL_VinSetLvdsPadMode(UINT32 VinID, UINT32 PadMode, UINT32 EnabledPin);
UINT32 AmbaRTSL_VinConfigMipiPhy(UINT32 VinID, const AMBA_VIN_MIPI_TIMING_PARAM_s* pVinMipiTiming, UINT32 ClkMode, UINT32 EnabledPin);
UINT32 AmbaRTSL_VinResetHvSyncOutput(UINT32 MSyncID);
UINT32 AmbaRTSL_VinSlvsConfig(UINT32 VinID, const AMBA_VIN_SLVS_CONFIG_s *pVinSlvsConfig);
UINT32 AmbaRTSL_VinMipiConfig(UINT32 VinID, const AMBA_VIN_MIPI_CONFIG_s *pVinMipiConfig);
UINT32 AmbaRTSL_VinDvpConfig(UINT32 VinID, const AMBA_VIN_DVP_CONFIG_s *pVinDvpConfig);
UINT32 AmbaRTSL_VinMasterSyncConfig(UINT32 MSyncID, const AMBA_VIN_MASTER_SYNC_CONFIG_s *pMSyncConfig);
UINT32 AmbaRTSL_VinSetLvdsTermination(UINT32 VinID, UINT32 Value);
UINT32 AmbaRTSL_VinDataLaneRemap(UINT32 VinID, const AMBA_VIN_LANE_REMAP_CONFIG_s *pLaneRemapConfig);
UINT32 AmbaRTSL_VinMipiVirtChanConfig(UINT32 VinID, const AMBA_VIN_MIPI_VC_CONFIG_s *pMipiVirtChanConfig);
UINT32 AmbaRTSL_VinSetSensorClkDrvStr(UINT32 Value);
UINT32 AmbaRTSL_VinResetMipiLogic(UINT32 VinID);
UINT32 AmbaRTSL_VinResetSlvsPhy(UINT32 VinID);

#endif /* _AMBA_RTSL_VIN_H_ */
