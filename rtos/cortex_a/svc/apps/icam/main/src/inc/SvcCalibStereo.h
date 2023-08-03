/**
 *  @file SvcCalibStereo.h
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details svc calib stereo
 *
 */


#ifndef SVC_CALIB_STEREO_H
#define SVC_CALIB_STEREO_H
/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcCalibSensorSync.c
\*-----------------------------------------------------------------------------------------------*/
#define SVC_CALIB_STEREO_ID            (13U)

#define SVC_CALIB_STEREO_TBL_NUM       (4U)

#define SVC_CALIB_STEREO_GET_ALL            (0U)
#define SVC_CALIB_STEREO_GET_SERIAL_NUMBER  (1U)

#include "AmbaSTU_StereoBarCalibInfo.h"

typedef struct {
    UINT8                            Cmd;
    char                             SerialNumber[16U];
    AMBA_STU_STEREO_CAM_WARP_INFO_s  CamWarpInfo;
} SVC_CALIB_STEREO_INFO_s;

// typedef AMBA_STU_STEREO_CAM_WARP_INFO_s SVC_CALIB_STEREO_TBL_s;
UINT32 SvcCalib_StereoGetTblIDInVinIDSensorID(UINT32 VinID, UINT32 SensorID, UINT32 *pTblID);
UINT32 SvcCalib_StereoCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4);
UINT32 SvcCalib_StereoShellFunc(UINT32 ArgCount, char * const *pArgVector);

#endif /* SVC_CALIB_STEREO_H */
