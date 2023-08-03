/**
*  @file SvcAdasNotify.h
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
*  @details Svc ADAS Notify header file
*
*/

#ifndef SVC_ADAS_NOTIFY_H
#define SVC_ADAS_NOTIFY_H

#define SVC_NOTIFY_GUI_DRAW_LANE      (1U)
#define SVC_NOTIFY_GUI_DRAW_MSG       (5U)

#ifndef CONFIG_SVC_LVGL_USED 
#define SVC_NOTIFY_GUI_CHAR_W         (30U)
#define GUI_CHAR_OFFSET               (0U)
#define SVC_NOTIFY_GUI_CHAR_H         (40U)
#else
#define SVC_NOTIFY_GUI_CHAR_W         (40U)
#define GUI_CHAR_OFFSET               (20U)
#define SVC_NOTIFY_GUI_CHAR_H         (40U)
#endif

#define ADAS_WARNING_SHOW_CNT         (40U)

typedef enum /* SvcADAS_NOTIFY_ADAS_PRIORITY_e */{
    ADAS_PRIORITY_FCWS_RED = 0L,
    ADAS_PRIORITY_FCWS_YELLOW,
    ADAS_PRIORITY_LDWS,
    ADAS_PRIORITY_FCMD,
    ADAS_PRIORITY_NONE
} SvcADAS_NOTIFY_ADAS_PRIORITY_e;

typedef struct {
    UINT32 StartW;
    UINT32 StartH;
} SvcADAS_NOTIFY_DRAW_s;

typedef struct {
    INT16 x;
    INT16 y;
} SvcADAS_NOTIFY_POINT_s;

SvcADAS_NOTIFY_ADAS_PRIORITY_e SvcAdasNotify_ADAS_IconShow(SvcADAS_NOTIFY_ADAS_PRIORITY_e EventCheck);
UINT32 SvcAdasNotify_DrawSpeed(void);
UINT32 SvcAdasNotify_DrawFcInfo(void);
void SvcAdasNotify_FcwsSafeDistUpdate(DOUBLE Dist);
void SvcAdasNotify_FcwsTtcUpdate(DOUBLE Time);

UINT32 SvcAdasNotify_LdwsDistR(void);
UINT32 SvcAdasNotify_LdwsDistL(void);
void SvcAdasNotify_LdwsDistRLUpdate(DOUBLE DistL, DOUBLE DistR);
void SvcAdasNotify_DrawInit(void);
void SvcAdasNotify_DrawDeInit(void);

void SvcAdasNotify_DecModeDrawSpeed(DOUBLE Speed);
void SvcAdasNotify_DrawFcMode(UINT32 Enable);

UINT32 SvcAdasNotify_CalibCheck(void);
void SvcAdasNotify_WarningZoneUpdate(const INT32 Zone1X[4], const INT32 Zone1Y[4], UINT32 SizeZone1X, UINT32 SizeZone1Y);
UINT32 SvcAdasNotify_HorizontalLineUpdate(const SVC_CV_DISP_OSD_CMD_s *pInfo);
UINT32 SvcAdasNotify_SetCalibHorizontal(UINT32 Enable, UINT32 Y);
UINT32 SvcAdasNotify_GetCalibHorizontal(void);
UINT32 SvcAdasNotify_GetWarningZoneX_InPixel(INT32 WarningZoneX, UINT32 SenserWidth);
void SvcAdasNotify_WarningZoneEnable(UINT32 Enable);
void SvcAdasNotify_WarningZoneGet(INT32 *ZoneX, INT32 *ZoneY);
void SvcAdasNotify_WarningZoneSet(INT32 *ZoneX, INT32 *ZoneY, UINT32 SetEnable);
void SvcAdasNotify_WarningZoneAutoTuneInit(void);

void SvcAdasNotify_SetACCRequiredSpeed(DOUBLE Speed);
void SvcAdasNotify_SetACCAcceleration(DOUBLE Acceleration);
void SvcAdasNotify_UpdateFCWarningZone(const AMBA_SR_LANE_MEMBER_s *pSubjectLaneInfo);
void SvcAdasNotify_UpdateFCInitInfo(const AMBA_WS_FCWS_DATA_s *pWsFcwsCfg);
void SvcAdasNotify_UpdateFCCurrentTransferSpeed(DOUBLE  FcTransferSpeed);
void SvcAdasNotify_UpdateLDSegCropInfo(const AMBA_SEG_CROP_INFO_s  *pSegCropInfo);

void SvcAdasNotify_UpdateAutoCalDataCollectRatio(DOUBLE DataCollectRatio, UINT32 AutoCalResult);

#endif  /* SVC_ADAS_NOTIFY_H */
