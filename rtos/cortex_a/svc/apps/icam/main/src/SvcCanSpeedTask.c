/**
 *  @file SvcCanSpeedTask.c
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
 *  @details svc display task
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaUtility.h"

/* ssp */
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaPrint.h"
#include "AmbaImg_Proc.h"

#if defined(CONFIG_BUILD_AMBA_ADAS) && defined(CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII)
#include "AmbaFPD.h"
#include "AmbaFPD_MAXIM_ZS095BH3001A8H3_BII.h"
#if defined(CONFIG_FPD_MAXIM_ZS095_MAX96789)
#include "AmbaFPD_MAX96789_96752_ZS095BH.h"
#endif
#endif

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"

/* svc-shared */
#include "SvcGui.h"
#include "SvcResCfg.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcOsd.h"
#include "SvcSysStat.h"

/* svc-icam */
#include "SvcAppStat.h"
#include "SvcCan.h"
#include "SvcWarningIconTask.h"
#include "SvcUserPref.h"
#include "SvcPref.h"

// Audio
#include "AmbaFS.h"
#include "SvcTask.h"
#include "AmbaVfs.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaADecFlow.h"
#include "SvcCanSpeedTask.h"

#if defined(CONFIG_BUILD_AMBA_ADAS) && defined(CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII)

#define CAN_SPEED_GUI_LEVEL (26U)

static UINT32 SvcCanSpeedGuiUpdate = 0U;

static void SvcCanSpeedTask_GuiDraw(UINT32 VoutIdx, UINT32 Level)
{
    AMBA_SR_CANBUS_RAW_DATA_s CanbusRawData;
    AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusTransData; // canbus transfer data
    UINT32 Rval;
    char   DoubleBuf[32U];

    AmbaMisra_TouchUnused(&VoutIdx);//pass vcast
    AmbaMisra_TouchUnused(&Level);//pass vcast

    /* init CanbusTransData */
    CanbusTransData.CANBusGearStatus = 0U;
    CanbusTransData.CANBusTurnLightStatus = 0U;
    CanbusTransData.FlagValidTransferData = 0U;
    CanbusTransData.TransferSpeed = 0.0;
    CanbusTransData.TransferWheelAngle = 0.0;
    CanbusTransData.TransferWheelAngleSpeed = 0.0;
    CanbusTransData.WheelDir = 0U;
    CanbusTransData.CapTS = 0U;

    SvcCan_GetRawData(&CanbusRawData);

    /* canbus transfer */
    Rval = SvcCan_Transfer(&CanbusRawData, &CanbusTransData);
    if (Rval != SVC_OK) {
        SvcLog_NG(__func__, "%d", __LINE__, 0U);
    }

    #if 0
    {
        UINT32 RandomVal;
        (void) AmbaWrap_rand(&RandomVal);
        CanbusTransData.TransferSpeed = (DOUBLE)(RandomVal % 140);
        CanbusTransData.FlagValidTransferData = 1U;
    }
    #endif

    if(CanbusTransData.FlagValidTransferData == 1U) {
        Rval = AmbaUtility_DoubleToStr(DoubleBuf, 32U, CanbusTransData.TransferSpeed, 0U);
        if (Rval != SVC_OK) {
            // SvcLog_NG(__func__, "%d", __LINE__, 0U);
        }

        Rval =  SvcOsd_DrawSolidRect(VOUT_IDX_A, 880, 420, 1025, 480, 0x80202040U);
        if (Rval != SVC_OK) {
            SvcLog_NG(__func__, "%d", __LINE__, 0U);
        }
        if(CanbusTransData.TransferSpeed >= 100.0) {
            Rval = SvcOsd_DrawString(VOUT_IDX_A, 890U, 430U, 4U, 0xffffffffU, DoubleBuf);
            if (Rval != SVC_OK) {
                SvcLog_NG(__func__, "%d", __LINE__, 0U);
            }
        } else if(CanbusTransData.TransferSpeed >= 10.0) {
            Rval = SvcOsd_DrawString(VOUT_IDX_A, 915U, 430U, 4U, 0xffffffffU, DoubleBuf);
            if (Rval != SVC_OK) {
                SvcLog_NG(__func__, "%d", __LINE__, 0U);
            }
        } else {
            Rval = SvcOsd_DrawString(VOUT_IDX_A, 940U, 430U, 4U, 0xffffffffU, DoubleBuf);
            if (Rval != SVC_OK) {
                SvcLog_NG(__func__, "%d", __LINE__, 0U);
            }
        }
        Rval = SvcOsd_DrawString(VOUT_IDX_A, 970U, 430U, 4U, 0xffffffffU, "KM");
        if (Rval != SVC_OK) {
            SvcLog_NG(__func__, "%d", __LINE__, 0U);
        }
    }
}

static void SvcCanSpeedTask_GuiDrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if (SvcCanSpeedGuiUpdate == 1U) {
        SvcCanSpeedGuiUpdate = 0U;
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

static void SvcCanSpeedTask_GuiUpdate(UINT32 EntryArg)
{
    AmbaMisra_TouchUnused(&EntryArg);
    SvcCanSpeedGuiUpdate = 1U;
}
#endif

void SvcCanSpeed_TaskInit(void)
{
    UINT32 RetVal;

#if defined(CONFIG_BUILD_AMBA_ADAS) && defined(CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII)
    static char SvcCanGuiTimerName[16] = "SvcCanGuiTimer";
    static AMBA_KAL_TIMER_t SvcCanGuiTimer GNU_SECTION_NOZEROINIT;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

#if defined(CONFIG_FPD_MAXIM_ZS095_MAX96789)
    if (pResCfg->DispStrm[0U].pDriver == &AmbaFPD_MAX789_752_ZS095Obj) {
        /* Create timer and GUI */

        SvcGui_Register(VOUT_IDX_A, CAN_SPEED_GUI_LEVEL, "CanSpd", SvcCanSpeedTask_GuiDraw, SvcCanSpeedTask_GuiDrawUpdate);
        /* Timer is used to showing and update GUI */
        RetVal = AmbaKAL_TimerCreate(&SvcCanGuiTimer, SvcCanGuiTimerName, SvcCanSpeedTask_GuiUpdate, 0U, 1000U, 1000U, AMBA_KAL_AUTO_START);
        if (SVC_OK != RetVal) {
            SvcLog_NG(__func__, "line %u AmbaKAL_TimerCreate return %u", __LINE__, RetVal);
        }
    }
#else
    if (pResCfg->DispStrm[0U].pDriver == &AmbaFPD_MAXIM_ZS095Obj) {
        /* Create timer and GUI */

        SvcGui_Register(VOUT_IDX_A, CAN_SPEED_GUI_LEVEL, "CanSpd", SvcCanSpeedTask_GuiDraw, SvcCanSpeedTask_GuiDrawUpdate);
        /* Timer is used to showing and update GUI */
        RetVal = AmbaKAL_TimerCreate(&SvcCanGuiTimer, SvcCanGuiTimerName, SvcCanSpeedTask_GuiUpdate, 0U, 1000U, 1000U, AMBA_KAL_AUTO_START);
        if (SVC_OK != RetVal) {
            SvcLog_NG(__func__, "line %u AmbaKAL_TimerCreate return %u", __LINE__, RetVal);
        }
    }
#endif

#endif
    AmbaMisra_TouchUnused(&RetVal);
}
