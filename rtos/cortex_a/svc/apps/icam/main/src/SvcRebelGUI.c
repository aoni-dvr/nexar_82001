/**
*  @file SvcRebelGUI.c
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
*  @Svc Svc SvcRebel GUI header file
*
*/


#include "AmbaKAL.h"
#include "AmbaTypes.h"
#include "AmbaSYS.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaWS_FCWS.h"
#include "AmbaUtility.h"
#include "AmbaSurround.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaAP_LKA.h"
#include "SvcErrCode.h"
#include "SvcCan.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "AmbaCalib_AVMIF.h"
#include "SvcResCfg.h"
#include "SvcLog.h"
#include "SvcCvCamCtrl.h"
#include "SvcLdwsTask.h"
#include "SvcRebelGUI.h"
#include "SvcWrap.h"
#include "SvcOsd.h"
#include "SvcUserPref.h"
//#include "RefFlow_AutoCal.h"

#define HACK_FAKE_ENABLE 0

static AMBA_SR_CANBUS_TRANSFER_DATA_s SvcAdasDrawSpd_CanTraData GNU_SECTION_NOZEROINIT; // Canbus transfer data

static SVC_Rebel_GUI_DRAW_s g_SpeedLayOut    GNU_SECTION_NOZEROINIT;

#define ADAS_GUI_SPEED_UPDATE       (0x01U)

static UINT32 SvcAdasGuiUpdate = 0U;
static UINT8  TargetVout = VOUT_IDX_B;
static UINT8 ST_TuneLine = 0U;

/**
 *  Svc Draw Speedometer for ADAS
 *  @return error code
 */
UINT32 SvcRebelGUI_UpdatGUIeInfo(void)
{
    UINT32 RetVal = SVC_OK;
    static UINT32 OsdBufWidth, OsdBufHeight;

    RetVal = SvcOsd_GetOsdBufSize(TargetVout, &OsdBufWidth, &OsdBufHeight);
    if (SVC_OK != RetVal) {
        SvcLog_NG(__func__, "line %u SvcOsd_GetOsdBufSize return %u", __LINE__, RetVal);
    } else {
        //Do nothing
    }
    g_SpeedLayOut.StartW = OsdBufWidth - (SVC_Rebel_GUI_CHAR_W * 8U);
    g_SpeedLayOut.StartH = OsdBufHeight - SVC_Rebel_GUI_CHAR_H;

    (void)AmbaWrap_memset(&SvcAdasDrawSpd_CanTraData, 0, sizeof(AMBA_SR_CANBUS_TRANSFER_DATA_s));

    return RetVal;
}

static void CarSpeedUpdate(void)
{
#if defined(CONFIG_ICAM_32BITS_OSD_USED)
static UINT32 ADAS_StringColor = 0xffffffffU;
#else
static UINT32 ADAS_StringColor = 255U;
#endif

    static UINT32 IsDecodeMode = 0U; //0U: Not decode mode (Liveview mode); 1U: Decode mode
    static DOUBLE DecodeModeSpeed = 0.0;
    AMBA_SR_CANBUS_RAW_DATA_s CanbusRawData;
    UINT32 Rval;
    char   DoubleBuf[32U];

    if (ST_TuneLine == 1U) {
        (void) SvcOsd_DrawLine(TargetVout,
                              (0U   + 10U), (540U/2U),
                              (960U - 10U) , (540U/2U),
                              3U, 0xffffffffU);
    }

    /* init SvcAdasDrawSpd_CanTraData */
    SvcAdasDrawSpd_CanTraData.CANBusGearStatus = 0U;
    SvcAdasDrawSpd_CanTraData.CANBusTurnLightStatus = 0U;
    SvcAdasDrawSpd_CanTraData.FlagValidTransferData = 0U;
    SvcAdasDrawSpd_CanTraData.TransferSpeed = 0.0;
    SvcAdasDrawSpd_CanTraData.TransferWheelAngle = 0.0;
    SvcAdasDrawSpd_CanTraData.TransferWheelAngleSpeed = 0.0;
    SvcAdasDrawSpd_CanTraData.WheelDir = 0U;
    SvcAdasDrawSpd_CanTraData.CapTS = 0U;

    /* init CanbusParaData (only for Tiguan) */
    SvcCan_GetRawData(&CanbusRawData);

    /* canbus transfer */
    Rval = SvcCan_Transfer(&CanbusRawData, &SvcAdasDrawSpd_CanTraData);
    if (Rval != SVC_OK) {
        SvcLog_NG(__func__, "%d", __LINE__, 0U);
    }

    SvcAdasGuiUpdate |= ADAS_GUI_SPEED_UPDATE;


#if (1 == HACK_FAKE_ENABLE)
    {
//        UINT32 RandomVal;
//        (void)AmbaWrap_rand(&RandomVal);
//        SvcAdasDrawSpd_CanTraData.TransferSpeed = (DOUBLE)(RandomVal % 140U);
        static DOUBLE SpeedHack = 0.0;
        SvcAdasDrawSpd_CanTraData.TransferSpeed = SpeedHack;
        SpeedHack++;
        if (SpeedHack >= 1000.0)
            SpeedHack = 0.0;

        SvcLog_NG(__func__, "SpeedHack = %d", SpeedHack, 0U);
    }
#endif

#if (0 == HACK_FAKE_ENABLE)
    if ((SvcAdasDrawSpd_CanTraData.FlagValidTransferData == 1U)||(1U == IsDecodeMode)) {
#endif
        if (1U == IsDecodeMode) {
            SvcAdasDrawSpd_CanTraData.TransferSpeed = DecodeModeSpeed;
        }
        (void) AmbaUtility_DoubleToStr(DoubleBuf, 32U, SvcAdasDrawSpd_CanTraData.TransferSpeed, 0U);

        if(SvcAdasDrawSpd_CanTraData.TransferSpeed >= 100.0) {
            (void) SvcOsd_DrawString(TargetVout, g_SpeedLayOut.StartW + (GUI_CHAR_OFFSET * 3U),
                                                 g_SpeedLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
        } else if(SvcAdasDrawSpd_CanTraData.TransferSpeed >= 10.0) {
            (void) SvcOsd_DrawString(TargetVout, g_SpeedLayOut.StartW + SVC_Rebel_GUI_CHAR_W + (GUI_CHAR_OFFSET * 2U),
                                                 g_SpeedLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
        } else {
            (void) SvcOsd_DrawString(TargetVout, g_SpeedLayOut.StartW + (SVC_Rebel_GUI_CHAR_W * 2U) + GUI_CHAR_OFFSET,
                                                 g_SpeedLayOut.StartH, 3U, ADAS_StringColor, DoubleBuf);
        }
#if (0 == HACK_FAKE_ENABLE)
    } else {
        (void) SvcOsd_DrawString(TargetVout, g_SpeedLayOut.StartW + SVC_Rebel_GUI_CHAR_W + GUI_CHAR_OFFSET,
                                             g_SpeedLayOut.StartH, 3U, ADAS_StringColor, "NA");
    }
#endif
    (void) SvcOsd_DrawString(TargetVout, g_SpeedLayOut.StartW + (SVC_Rebel_GUI_CHAR_W * 3U),
                                         g_SpeedLayOut.StartH, 3U, ADAS_StringColor, "KM");
}

static void SvcRebelGUI_MsgDrawCB(UINT32 VoutIdx, UINT32 Level)
{
    AmbaMisra_TouchUnused(&Level);//pass vcast
    AmbaMisra_TouchUnused(&VoutIdx);//pass vcast

    CarSpeedUpdate();
}

static void SvcRebelGUI_MsgDrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((SvcAdasGuiUpdate & ADAS_GUI_SPEED_UPDATE) > 0U) {
        SvcAdasGuiUpdate &= ~(ADAS_GUI_SPEED_UPDATE);
        *pUpdate = 1U;
    }
}

/**
 *  Svc notify draw init function
 */
void SvcRebelGUI_DrawInit(void)
{
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    TargetVout = (UINT8)pResCfg->DispStrm[0U].VoutID;
    SvcLog_OK(__func__, "SvcRebelGUI_DrawInit, TargetVout = %d", TargetVout, 0U);

    SvcGui_Register(TargetVout, SVC_Rebel_GUI_DRAW_MSG, "Adas", SvcRebelGUI_MsgDrawCB, SvcRebelGUI_MsgDrawUpdate);
}

void SvcRebelGUI_TuneLineEnable(UINT8 Enable)
{
    ST_TuneLine = Enable;

    SvcLog_OK(__func__, "TuneLineEnable() Enable = %d", Enable, 0U);
}
