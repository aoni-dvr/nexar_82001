/*
*  @file AmbaTUNE_HdlrManager_cv2x.h
*
* Copyright (c) 2020 Ambarella International LP
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
*/

#ifndef AMBA_TUNE_HDLR_MANAGER_CV2X_H
#define AMBA_TUNE_HDLR_MANAGER_CV2X_H
#include "AmbaKAL.h"
#include "AmbaDSP.h"
#include "AmbaTypes.h"
#include "AmbaDSP_ImageFilter.h"

#define TUNE_OK             (0UL)
#define TUNE_ERROR_GENERAL  (TUNE_ERR_BASE)

typedef enum {
    TEXT_TUNE = 0,
    USB_TUNE,
    MAX_TUNE,
} AMBA_ITN_Hdlr_Mode_e;

typedef struct {
    UINT32 ItunerRev;
    char SensorId[32];
    char TuningMode[32];
    char RawPath[128];
    UINT32 RawWidth;
    UINT32 RawHeight;
    UINT32 RawPitch;
    UINT32 RawResolution;
    UINT32 RawBayer;
    UINT32 MainWidth;
    UINT32 MainHeight;
    UINT32 InputPicCnt;
    UINT32 CompressedRaw;
    UINT32 SensorReadoutMode;
    UINT32 RawStartX;
    UINT32 RawStartY;
    UINT32 HSubSampleFactorNum;
    UINT32 HSubSampleFactorDen;
    UINT32 VSubSampleFactorNum;
    UINT32 VSubSampleFactorDen;
    UINT32 NumberOfExposures;
    UINT32 SensorMode; // 0:normal, 1:rgb_ir, 2:rccc
    INT32 CompressionOffset;
    UINT32 Ability;
    UINT32 YuvFormat;
    UINT32 FlipH;
    UINT32 FlipV;
    UINT32 FrameNumber;
    UINT32 NumberOfFrames;
    UINT32 CfaWindowEnable;
    UINT32 CfaWidth;
    UINT32 CfaHeight;
    UINT8  UseExternalHdsMode;
} AMBA_ITN_SYSTEM_s;

/*!@brief Specified the tuning mode ext*/
typedef enum {
    SINGLE_SHOT = 0,
    SINGLE_SHOT_MULTI_EXPOSURE_HDR,
    TUNING_MODE_EXT_NUMBER
} AMBA_ITN_TUNING_MODE_EXT_e;

/*!@brief Specified the tuning mode info, that capture flow need*/
typedef struct {
    AMBA_ITN_TUNING_MODE_EXT_e TuningModeExt;
} AMBA_ITN_ITUNER_INFO_s;

typedef struct {
    UINT32 RawAreaWidth[3];
    UINT32 RawAreaHeight[3];
    AMBA_IK_HDR_RAW_INFO_s Offset;
} AMBA_ITN_VIDEO_HDR_RAW_INFO_s;

typedef struct {
    char *FilePath; //!< Source Path
} AMBA_ITN_TEXT_Load_Param_s;

typedef struct {
    char *Filepath; //!< Dest. Path
} AMBA_ITN_TEXT_Save_Param_s;

typedef struct {
    AMBA_ITN_TEXT_Load_Param_s Text; //!< The input param of AmbaTUNE_Load_IDSP(TEXT), detail please reference AMBA_ITN_TEXT_Load_Param_s
} AMBA_ITN_Load_Param_s;

typedef struct {
    AMBA_ITN_TEXT_Save_Param_s Text;//!< The input param of AmbaTUNE_Save_IDSP(TEXT), detail please reference AMBA_ITN_TEXT_Save_Param_s
} AMBA_ITN_Save_Param_s;

UINT32 AmbaItn_QueryItuner(UINT32 *pSize);
UINT32 AmbaItn_Init(void *pBuffer, UINT32 ItunerSize);
UINT32 AmbaItn_Change_Parser_Mode(AMBA_ITN_Hdlr_Mode_e ParserMode);
UINT32 AmbaItn_Save_IDSP(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_ITN_Save_Param_s *pSaveParam);
UINT32 AmbaItn_Load_IDSP(const AMBA_ITN_Load_Param_s *pLoadParam);
UINT32 AmbaItn_Execute_IDSP(const AMBA_IK_MODE_CFG_s *pMode);
UINT32 AmbaItn_Get_SystemInfo(AMBA_ITN_SYSTEM_s *pSystem);
UINT32 AmbaItn_Set_SystemInfo(const AMBA_ITN_SYSTEM_s *pSystem);
UINT32 AmbaItn_Get_VideoHdrRawInfo(AMBA_ITN_VIDEO_HDR_RAW_INFO_s *pRawInfo);

void Never_Ever_Call_This(void);

#endif /* AMBA_TUNE_HDLR_MANAGER_H */
