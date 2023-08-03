/**
 *  @file SvcRctaTask.c
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
 *  @details Svc Rcta Object Detection task
 *
 */
#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaCalib_AVMIF.h"
#include "AmbaCT_AvmTunerIF.h"
#include "AmbaCalib_EmirrorDef.h"
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#include "SvcResCfg.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcCvFlow.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvAppDef.h"
#include "SvcCalibAdas.h"
#include "RefFlow_RCTA.h"
#include "SvcSurDrawTask.h"
#include "SvcRctaTask.h"

#define SVC_LOG_RCTA_TASK     "RCTA_TASK"
#define SVC_RCTA_QUEUE_SIZE        128U

#define SVC_RCTA_TASK_STACK_SIZE (0xF000)

static UINT8 SvcRctaInit = 0U;
static UINT8 RctaCalibLoad = 0U; //Current always 1;
static UINT8 SvcRctaOn = 0U;
static SVC_RCTA_TASK_DRAW_DATA_s WarnResult;
static SVC_RCTA_TASK_WARN_ZONE_DATA_s FrontZone;
static SVC_RCTA_TASK_WARN_ZONE_DATA_s RearZone;
static AMBA_KAL_MSG_QUEUE_t    SvcRctaQueId GNU_SECTION_NOZEROINIT;
static AMBA_SR_SCV_CROP_INFO_s CropInfo;

static void SvcRctaTask_DetectionResultCallback(UINT32 Chan, UINT32 OutputTag, void *pDetResMsg);

static void* SvcRctaTaskEntry(void* EntryArg)
{
    UINT8 IsTrue = 1U;
    UINT32 Rval;
    static SVC_CV_DETRES_SEG_s SegResult;
    static REF_FLOW_RCTA_IN_DATA_s RCTAIn;
    static REF_FLOW_RCTA_OUT_DATA_s RCTAOut;

    AmbaSvcWrap_MisraMemset(&RCTAIn, 0, sizeof(REF_FLOW_RCTA_IN_DATA_s));
    AmbaSvcWrap_MisraMemset(&SegResult, 0, sizeof(SVC_CV_DETRES_SEG_s));
    AmbaSvcWrap_MisraMemset(&RCTAOut, 0, sizeof(REF_FLOW_RCTA_OUT_DATA_s));

    while (0U != IsTrue) {
        Rval = AmbaKAL_MsgQueueReceive(&SvcRctaQueId, &SegResult, AMBA_KAL_WAIT_FOREVER);
        if (SVC_NG == Rval) {
            SvcLog_NG(SVC_LOG_RCTA_TASK, "SvcLdwsQueId MsgQue Receive error !", 0U, 0U);
        } else {
            RCTAIn.ScvInData.TimeStamp = SegResult.CaptureTime;
            RCTAIn.ScvInData.SegBufInfo.pBuf = SegResult.pBuf;
            RCTAIn.ScvInData.SegBufInfo.Width = SegResult.Width;
            RCTAIn.ScvInData.SegBufInfo.Height = SegResult.Height;
            RCTAIn.ScvInData.SegBufInfo.Pitch = SegResult.Pitch;
            AmbaSvcWrap_MisraMemcpy(&(RCTAIn.ScvInData.SegBufCropInfo), &(CropInfo), sizeof(AMBA_SR_SCV_CROP_INFO_s));
            Rval = RefFlowRCTA_Process(SegResult.MsgCode, &RCTAIn, &RCTAOut);
            if (SVC_NG == Rval) {
                SvcLog_NG(SVC_LOG_RCTA_TASK, "RefFlowRCTA_Process error !", 0U, 0U);
            } else {
                const void *DrawOut;
                const SVC_RCTA_TASK_DRAW_DATA_s *OutData = &WarnResult;

                if (SegResult.MsgCode == RF_RCTA_CHANNEL_FRONT) {
                    AmbaSvcWrap_MisraMemcpy(&(WarnResult.WarningZone), &(FrontZone), sizeof(SVC_RCTA_TASK_WARN_ZONE_DATA_s));
                } else {
                    AmbaSvcWrap_MisraMemcpy(&(WarnResult.WarningZone), &(RearZone), sizeof(SVC_RCTA_TASK_WARN_ZONE_DATA_s));
                }
                AmbaSvcWrap_MisraMemcpy(&(WarnResult.WarnInfoL), &(RCTAOut.WarnInfoL), sizeof(AMBA_WS_RCTA_WARNING_INFO_s));
                AmbaSvcWrap_MisraMemcpy(&(WarnResult.WarnInfoR), &(RCTAOut.WarnInfoR), sizeof(AMBA_WS_RCTA_WARNING_INFO_s));
                AmbaMisra_TypeCast(&DrawOut, &OutData);
                Rval = SvcSurDrawTask_DrawMsg(SVC_RCTA_DRAW, DrawOut);
                if (SVC_NG == Rval) {
                    SvcLog_NG(SVC_LOG_RCTA_TASK, "SvcSurDrawTask_DrawMsg error !", 0U, 0U);
                }
                //AmbaPrint_PrintInt5("Warning Status L:%d R:%d", RCTAOut.WarnInfoL.WarningStat, RCTAOut.WarnInfoR.WarningStat, 0U, 0U, 0U);
            }
        }
    }
    AmbaMisra_TouchUnused(EntryArg);

    return NULL;
}

static UINT32 TransferWorld2Src(const AMBA_SR_SCV_CAL_AVM_CFG_s* pCalInfo, const AMBA_CAL_POINT_DB_3D_s *In, AMBA_CAL_POINT_DB_2D_s *Out)
{
    UINT32 Rval;
    UINT32 Width = pCalInfo->Cfg.RawArea.Width - 1U;
    UINT32 Height = pCalInfo->Cfg.RawArea.Height - 1U;


    Rval = AmbaCal_AvmGetParkAssistPosv1(&pCalInfo->CalibData, &pCalInfo->Cfg, In, Out);
    if (Rval != SVC_OK) {
        SvcLog_NG(SVC_LOG_RCTA_TASK, "AmbaCal_AvmGetParkAssistPosv1 err %d", Rval, 0U);
    } else if ((Out->X < 0.0) || (Out->X > (DOUBLE)Width) || (Out->Y < 0.0) || (Out->Y > (DOUBLE)Height)) {
        Rval = SVC_NG;
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}

static UINT32 CalLeftZoneCord(UINT32 Chan, const AMBA_WS_RCTA_CFG_DATA_s *pWSCfg, const AMBA_SR_SCV_CAL_AVM_CFG_s* pCalInfo, SVC_RCTA_TASK_WARN_ZONE_DATA_s *pWZOut)
{
    AMBA_CAL_POINT_DB_3D_s Pnt3D;
    AMBA_CAL_POINT_DB_2D_s Pnt2D;
    AMBA_CAL_POINT_INT_2D_s *pWarnZoneTtc = &pWZOut->WarnZoneTtcL[0];
    AMBA_CAL_POINT_INT_2D_s *pWarnZoneApc = &pWZOut->WarnZoneApcL[0];
    DOUBLE LengthApc = (DOUBLE)pWSCfg->WarningThreshold.ZoneLength;
    DOUBLE LengthTtc = (DOUBLE)pWSCfg->WarningThreshold.TTCZoneLength;
    DOUBLE Depth = (DOUBLE)pWSCfg->WarningThreshold.ZoneDepth;
    DOUBLE OfsY = (DOUBLE)pWSCfg->SelfCarSpec.Size.Length / 2.0;
    DOUBLE Sign = 1.0;
    DOUBLE Fac = 0.0;
    DOUBLE ATmp = 0.0, BTmp = 0.0;
    UINT32 Rval = SVC_OK;
    UINT32 i;

    if (Chan == RF_RCTA_CHANNEL_REAR) {
        Sign = -1.0;
    }

    /** ------------------------------------*/
    /** near horizontal part                */
    /** ------------------------------------*/
    for (i = 0U; i < SVC_RCTA_WZ_SLICE_L; i++) {
        ATmp = (DOUBLE)i;
        BTmp = (DOUBLE)SVC_RCTA_WZ_SLICE_L;
        Fac = ATmp / BTmp ;

        /** left of TTC Zone */
        Pnt3D.X = - (LengthTtc * (Fac * Fac * Fac));
        Pnt3D.Y = Sign * OfsY;
        Pnt3D.Z = 0.0;
        Rval = TransferWorld2Src(pCalInfo, &Pnt3D, &Pnt2D);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_RCTA_TASK, "TransferWorld2Src err %d", Rval, 0U);
        } else {
            pWarnZoneTtc[pWZOut->WZTtcPntNumL].X = (INT32)Pnt2D.X;
            pWarnZoneTtc[pWZOut->WZTtcPntNumL].Y = (INT32)Pnt2D.Y;
            pWZOut->WZTtcPntNumL++;
        }


        /** left of Approach Zone */
        if ((i % 4U) == 0U) { /**< reduce number */
            Pnt3D.X = - (((LengthApc - LengthTtc) * (Fac*Fac)) + LengthTtc);
            Pnt3D.Y = Sign * OfsY;
            Pnt3D.Z = 0.0;
            Rval = TransferWorld2Src(pCalInfo, &Pnt3D, &Pnt2D);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_RCTA_TASK, "TransferWorld2Src err %d", Rval, 0U);
            } else {
                pWarnZoneApc[pWZOut->WZApcPntNumL].X = (INT32)Pnt2D.X;
                pWarnZoneApc[pWZOut->WZApcPntNumL].Y = (INT32)Pnt2D.Y;
                pWZOut->WZApcPntNumL++;
            }
        }
    }

    /** ------------------------------------*/
    /** vertical part                       */
    /** ------------------------------------*/
    for (i = 0U; i < SVC_RCTA_WZ_SLICE_D; i++) {
        ATmp = (DOUBLE)i;
        BTmp = (DOUBLE)SVC_RCTA_WZ_SLICE_D;
        Fac = ATmp / BTmp;
        Fac = Fac * Fac;

        /** left of TTC Zone */
        Pnt3D.X = - LengthTtc;
        Pnt3D.Y = Sign * (OfsY + (Depth * Fac));
        Pnt3D.Z = 0.0;
        Rval = TransferWorld2Src(pCalInfo, &Pnt3D, &Pnt2D);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_RCTA_TASK, "TransferWorld2Src err %d", Rval, 0U);
        } else {
            pWarnZoneTtc[pWZOut->WZTtcPntNumL].X = (INT32)Pnt2D.X;
            pWarnZoneTtc[pWZOut->WZTtcPntNumL].Y = (INT32)Pnt2D.Y;
            pWZOut->WZTtcPntNumL++;
        }


        /** left of Approach Zone */
        if ((i % 2U) == 0U) { /**< reduce number */
            Pnt3D.X = - LengthApc;
            Pnt3D.Y = Sign * (OfsY + (Depth * Fac));
            Pnt3D.Z = 0.0;
            Rval = TransferWorld2Src(pCalInfo, &Pnt3D, &Pnt2D);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_RCTA_TASK, "TransferWorld2Src err %d", Rval, 0U);
            } else {
                pWarnZoneApc[pWZOut->WZApcPntNumL].X = (INT32)Pnt2D.X;
                pWarnZoneApc[pWZOut->WZApcPntNumL].Y = (INT32)Pnt2D.Y;
                pWZOut->WZApcPntNumL++;
            }
        }
    }

    /** ------------------------------------*/
    /** far horizontal part                 */
    /** ------------------------------------*/
    for (i = 0U; i <= SVC_RCTA_WZ_SLICE_L; i++) {  /** plus one point when i == SLICE_L */
        ATmp = (DOUBLE)i;
        BTmp = (DOUBLE)SVC_RCTA_WZ_SLICE_L;
        Fac = (BTmp - ATmp) / BTmp;
        Fac = Fac * Fac;

        /** left of TTC Zone */
        Pnt3D.X = - (LengthTtc * Fac);
        Pnt3D.Y = Sign * (OfsY + Depth);
        Pnt3D.Z = 0.0;
        Rval = TransferWorld2Src(pCalInfo, &Pnt3D, &Pnt2D);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_RCTA_TASK, "TransferWorld2Src err %d", Rval, 0U);
        } else {
            pWarnZoneTtc[pWZOut->WZTtcPntNumL].X = (INT32)Pnt2D.X;
            pWarnZoneTtc[pWZOut->WZTtcPntNumL].Y = (INT32)Pnt2D.Y;
            pWZOut->WZTtcPntNumL++;
        }

        /** left of Approach Zone */
        if ((i % 4U) == 0U) { /**< reduce number */
            Pnt3D.X = - (((LengthApc - LengthTtc) * Fac) + LengthTtc);
            Pnt3D.Y = Sign * (OfsY + Depth);
            Pnt3D.Z = 0.0;
            Rval = TransferWorld2Src(pCalInfo, &Pnt3D, &Pnt2D);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_RCTA_TASK, "TransferWorld2Src err %d", Rval, 0U);
            } else {
                pWarnZoneApc[pWZOut->WZApcPntNumL].X = (INT32)Pnt2D.X;
                pWarnZoneApc[pWZOut->WZApcPntNumL].Y = (INT32)Pnt2D.Y;
                pWZOut->WZApcPntNumL++;
            }
        }
    }

    return Rval;
}


static UINT32 CalRightZoneCord(UINT32 Chan, const AMBA_WS_RCTA_CFG_DATA_s *pWSCfg, const AMBA_SR_SCV_CAL_AVM_CFG_s* pCalInfo, SVC_RCTA_TASK_WARN_ZONE_DATA_s *pWZOut)
{
    AMBA_CAL_POINT_DB_3D_s Pnt3D;
    AMBA_CAL_POINT_DB_2D_s Pnt2D;
    AMBA_CAL_POINT_INT_2D_s *pWarnZoneTtc = &pWZOut->WarnZoneTtcR[0];
    AMBA_CAL_POINT_INT_2D_s *pWarnZoneApc = &pWZOut->WarnZoneApcR[0];
    DOUBLE LengthApc = (DOUBLE)pWSCfg->WarningThreshold.ZoneLength;
    DOUBLE LengthTtc = (DOUBLE)pWSCfg->WarningThreshold.TTCZoneLength;
    DOUBLE Depth = (DOUBLE)pWSCfg->WarningThreshold.ZoneDepth;
    DOUBLE OfsY = (DOUBLE)pWSCfg->SelfCarSpec.Size.Length / 2.0;
    DOUBLE Sign = 1.0;
    DOUBLE Fac = 0.0;
    DOUBLE ATmp = 0.0, BTmp = 0.0;
    UINT32 Rval = SVC_OK;
    UINT32 i;

    if (Chan == RF_RCTA_CHANNEL_REAR) {
        Sign = -1.0;
    }
    /** ------------------------------------*/
    /** near horizontal part                */
    /** ------------------------------------*/
    for (i = 0U; i < SVC_RCTA_WZ_SLICE_L; i++) {
        ATmp = (DOUBLE)i;
        BTmp = (DOUBLE)SVC_RCTA_WZ_SLICE_L;
        Fac = ATmp / BTmp;

        /** right of TTC Zone */
        Pnt3D.X = LengthTtc * (Fac*Fac*Fac);
        Pnt3D.Y = Sign*OfsY;
        Pnt3D.Z = 0.0;
        Rval = TransferWorld2Src(pCalInfo, &Pnt3D, &Pnt2D);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_RCTA_TASK, "TransferWorld2Src err %d", Rval, 0U);
        } else {
            pWarnZoneTtc[pWZOut->WZTtcPntNumR].X = (INT32)Pnt2D.X;
            pWarnZoneTtc[pWZOut->WZTtcPntNumR].Y = (INT32)Pnt2D.Y;
            pWZOut->WZTtcPntNumR++;
        }
        /** right of Approach Zone */
        if ((i % 4U) == 0U) { /**< reduce number */
            Pnt3D.X = ((LengthApc - LengthTtc) * (Fac*Fac)) + LengthTtc;
            Pnt3D.Y = Sign*OfsY;
            Pnt3D.Z = 0.0;
            Rval = TransferWorld2Src(pCalInfo, &Pnt3D, &Pnt2D);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_RCTA_TASK, "TransferWorld2Src err %d", Rval, 0U);
            } else {
                pWarnZoneApc[pWZOut->WZApcPntNumR].X = (INT32)Pnt2D.X;
                pWarnZoneApc[pWZOut->WZApcPntNumR].Y = (INT32)Pnt2D.Y;
                pWZOut->WZApcPntNumR++;
            }
        }
    }
    /** ------------------------------------*/
    /** vertical part                       */
    /** ------------------------------------*/
    for (i = 0U; i < SVC_RCTA_WZ_SLICE_D; i++) {
        ATmp = (DOUBLE)i;
        BTmp = (DOUBLE)SVC_RCTA_WZ_SLICE_D;
        Fac = ATmp / BTmp;
        Fac = Fac * Fac;

        /** right of TTC Zone */
        Pnt3D.X = LengthTtc;
        Pnt3D.Y = Sign * (OfsY + (Depth * Fac));
        Pnt3D.Z = 0.0;
        Rval = TransferWorld2Src(pCalInfo, &Pnt3D, &Pnt2D);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_RCTA_TASK, "TransferWorld2Src err %d", Rval, 0U);
        } else {
            pWarnZoneTtc[pWZOut->WZTtcPntNumR].X = (INT32)Pnt2D.X;
            pWarnZoneTtc[pWZOut->WZTtcPntNumR].Y = (INT32)Pnt2D.Y;
            pWZOut->WZTtcPntNumR++;
        }

        /** right of Approach Zone */
        if ((i % 2U) == 0U) { /**< reduce number */
            Pnt3D.X = LengthApc;
            Pnt3D.Y = Sign * (OfsY + (Depth * Fac));
            Pnt3D.Z = 0.0;
            Rval = TransferWorld2Src(pCalInfo, &Pnt3D, &Pnt2D);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_RCTA_TASK, "TransferWorld2Src err %d", Rval, 0U);
            } else {
                pWarnZoneApc[pWZOut->WZApcPntNumR].X = (INT32)Pnt2D.X;
                pWarnZoneApc[pWZOut->WZApcPntNumR].Y = (INT32)Pnt2D.Y;
                pWZOut->WZApcPntNumR++;
            }
        }
    }
    /** ------------------------------------*/
    /** far horizontal part                 */
    /** ------------------------------------*/
    for (i = 0U; i <= SVC_RCTA_WZ_SLICE_L; i++) {  /** plus one point when i == SLICE_L */
        ATmp = (DOUBLE)i;
        BTmp = (DOUBLE)SVC_RCTA_WZ_SLICE_L;
        Fac = (BTmp - ATmp) / BTmp;
        Fac = Fac * Fac;

        /** right of TTC Zone */
        Pnt3D.X = LengthTtc * Fac;
        Pnt3D.Y = Sign *(OfsY + Depth);
        Pnt3D.Z = 0.0;
        Rval = TransferWorld2Src(pCalInfo, &Pnt3D, &Pnt2D);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_RCTA_TASK, "TransferWorld2Src err %d", Rval, 0U);
        } else {
            pWarnZoneTtc[pWZOut->WZTtcPntNumR].X = (INT32)Pnt2D.X;
            pWarnZoneTtc[pWZOut->WZTtcPntNumR].Y = (INT32)Pnt2D.Y;
            pWZOut->WZTtcPntNumR++;
        }

        /** right of Approach Zone */
        if ((i % 4U) == 0U) { /**< reduce number */
            Pnt3D.X = ((LengthApc - LengthTtc) * Fac) + LengthTtc;
            Pnt3D.Y = Sign * (OfsY + Depth);
            Pnt3D.Z = 0.0;
            Rval = TransferWorld2Src(pCalInfo, &Pnt3D, &Pnt2D);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_RCTA_TASK, "TransferWorld2Src err %d", Rval, 0U);
            } else {
                pWarnZoneApc[pWZOut->WZApcPntNumR].X = (INT32)Pnt2D.X;
                pWarnZoneApc[pWZOut->WZApcPntNumR].Y = (INT32)Pnt2D.Y;
                pWZOut->WZApcPntNumR++;
            }
        }

    }
    return Rval;
}

static UINT32 SvcRctaTask_CalWarningZone(UINT32 Chan, const AMBA_WS_RCTA_CFG_DATA_s *pWSCfg, const AMBA_SR_SCV_CAL_AVM_CFG_s* pCalInfo, SVC_RCTA_TASK_WARN_ZONE_DATA_s *pOut)
{
    UINT32 Rval = SVC_OK;

    Rval = CalRightZoneCord(Chan, pWSCfg, pCalInfo, pOut);
    if (Rval == SVC_OK) {
        Rval = CalLeftZoneCord(Chan, pWSCfg, pCalInfo, pOut);
    }
    return Rval;
}

UINT32 SvcRctaTask_Init(void)
{
    UINT32 Rval;
    static SVC_TASK_CTRL_s     SvcRctaTaskCtrl GNU_SECTION_NOZEROINIT;
    static UINT8               SvcRctaTaskStack[SVC_RCTA_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char                QueueName[] = "SvcRctaQueue";
    static SVC_CV_DETRES_SEG_s SvcRctaQue[SVC_RCTA_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static AMBA_SR_SCV_CAL_AVM_CFG_s CalCfg GNU_SECTION_NOZEROINIT;
    static REF_FLOW_RCTA_CFG_s RctaCfg[2];
    static AMBA_CAL_AVM_MV_LDC_CFG_s LdcCfg[2];
    static AMBA_CAL_LENS_DST_REAL_EXPECT_s RealExpect[2];
    static DOUBLE Real[2][MAX_LENS_DISTO_TBL_LEN];
    static DOUBLE Expect[2][MAX_LENS_DISTO_TBL_LEN];
    AMBA_CAL_SIZE_s CarSize;
    UINT32 BufSize = 0U, i, WorkBufSize;
    ULONG BufBase = 0U;

    AmbaSvcWrap_MisraMemset(&WarnResult, 0, sizeof(SVC_RCTA_TASK_DRAW_DATA_s));
    AmbaSvcWrap_MisraMemset(&FrontZone, 0, sizeof(SVC_RCTA_TASK_WARN_ZONE_DATA_s));
    AmbaSvcWrap_MisraMemset(&RearZone, 0, sizeof(SVC_RCTA_TASK_WARN_ZONE_DATA_s));
    AmbaSvcWrap_MisraMemset(&RctaCfg, 0, (sizeof(REF_FLOW_RCTA_CFG_s) * 2U));
    AmbaSvcWrap_MisraMemset(&CarSize, 0, sizeof(AMBA_CAL_SIZE_s));

    for (i = 0U; i < 2U; i++) {
        Rval = RefFlowRCTA_GetDefaultCfg(RCTA_SEG_WIDTH, RCTA_SEG_HEIGH, &WorkBufSize, &RctaCfg[i]);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_RCTA_TASK, "RefFlowRCTA_GetDefaultCfg err %d", Rval, 0U);
        } else {
            static SVC_CALIB_ADAS_NAND_TABLE_s Cfg;
            SVC_CALIB_ADAS_NAND_TABLE_s *pCfg = &Cfg;
            UINT32 Chan,RetVal;
            Chan = i + SVC_CALIB_ADAS_TYPE_AVM_F;
            AmbaSvcWrap_MisraMemset(&Cfg, 0, sizeof(SVC_CALIB_ADAS_NAND_TABLE_s));
            Cfg.AdasAvmPaCfg.Cfg.pLDC = &LdcCfg[i];
            Cfg.AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect = &RealExpect[i];
            RealExpect[i].pRealTbl= &Real[i][0];
            RealExpect[i].pExpectTbl = &Expect[i][0];
            RetVal = SvcCalib_AdasCfgGet(Chan, pCfg);
            if (RetVal == SVC_OK) {
                AmbaSvcWrap_MisraMemcpy(&(RctaCfg[i].CalibData), &(Cfg.AdasAvmCalibCfg), sizeof(AMBA_CAL_AVM_CALIB_DATA_s));
                AmbaSvcWrap_MisraMemcpy(&(RctaCfg[i].PACfgV1), &(Cfg.AdasAvmPaCfg), sizeof(AMBA_CAL_AVM_PARK_AST_CFG_V1_s));
                AmbaSvcWrap_MisraMemcpy(&CarSize, &(Cfg.AdasAvm2DCfg.Car), sizeof(AMBA_CAL_SIZE_s));
                RctaCalibLoad = 1U;
            } else {
                RctaCalibLoad = 0U;
            }
        }

        if (RctaCalibLoad == 1U) {
            UINT8 *WorkBufAddr;
            Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_SUR_CV_RCTA, &BufBase, &BufSize);
            if ((BufSize >= WorkBufSize) && (Rval == SVC_OK)) {
                AmbaMisra_TypeCast(&WorkBufAddr, &BufBase);
                RctaCfg[i].ScvSRCfg.Clock = 12288;
                RctaCfg[i].ScvSRCfg.VehicleIdxInfo.IdxNum = 1;
                RctaCfg[i].ScvSRCfg.VehicleIdxInfo.IdxList[0] = 0U;
                RctaCfg[i].ScvSRCfg.SelfCarSpec.Size.Width = CarSize.Width;
                RctaCfg[i].ScvSRCfg.SelfCarSpec.Size.Length = CarSize.Height;
                RctaCfg[i].RctaWSCfg.WarningThreshold.TTCZoneLength = 10000U;
                RctaCfg[i].RctaWSCfg.Clock = 12288;
                Rval = RefFlowRCTA_Init(i, &RctaCfg[i], WorkBufAddr);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_RCTA_TASK, "RefFlowRCTA_Init failed!!", 0U, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_RCTA_TASK, "SvcBuffer_Request failed or buffer size not enough!!", 0U, 0U);
            }
            AmbaSvcWrap_MisraMemcpy(&(CalCfg.CalibData), &(RctaCfg[i].CalibData), sizeof(AMBA_CAL_AVM_CALIB_DATA_s));
            AmbaSvcWrap_MisraMemcpy(&(CalCfg.Cfg), &(RctaCfg[i].PACfgV1), sizeof(AMBA_CAL_AVM_PARK_AST_CFG_V1_s));

            if (i == RF_RCTA_CHANNEL_FRONT) {
                Rval = SvcRctaTask_CalWarningZone(i, &RctaCfg[i].RctaWSCfg, &CalCfg, &FrontZone);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_RCTA_TASK, "SvcRctaTask_CalWarningZone failed!!", 0U, 0U);
                }
            } else {
                Rval = SvcRctaTask_CalWarningZone(i, &RctaCfg[i].RctaWSCfg, &CalCfg, &RearZone);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_RCTA_TASK, "SvcRctaTask_CalWarningZone failed!!", 0U, 0U);
                }
            }
        } else {
            break;
        }
    }



    if (Rval == SVC_OK) {
        /* create msg queue */
        if (SVC_OK != AmbaKAL_MsgQueueCreate(&SvcRctaQueId,
                                        QueueName,
                                        sizeof(SVC_CV_DETRES_SEG_s),
                                        SvcRctaQue,
                                        SVC_RCTA_QUEUE_SIZE * sizeof(SVC_CV_DETRES_SEG_s))) {
            SvcLog_NG(SVC_LOG_RCTA_TASK, "SvcRctaTask_Init: Fail to create msg queue", 0U, 0U);
        } else {
            //Do nothing
        }

        /* Create LDWS task */
        SvcRctaTaskCtrl.Priority   = 55U;
        SvcRctaTaskCtrl.EntryFunc  = SvcRctaTaskEntry;
        SvcRctaTaskCtrl.pStackBase = SvcRctaTaskStack;
        SvcRctaTaskCtrl.StackSize  = SVC_RCTA_TASK_STACK_SIZE;
        SvcRctaTaskCtrl.CpuBits    = 0x1U;
        SvcRctaTaskCtrl.EntryArg   = 0x0U;

        Rval = SvcTask_Create("SvcRctaTask", &SvcRctaTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_RCTA_TASK, "SvcRcta_TaskCreate create fail", 0U, 0U);
        }

        if (SVC_OK == Rval) {
            const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
            UINT32 CvFlowNum = pResCfg->CvFlowNum;
            UINT32 CvFlowBits = pResCfg->CvFlowBits;
            const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

            for (i = 0U; i < CvFlowNum; i++) {
                if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                    if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_SEG_FDAG) ||
                        (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_SEG)) {
                        UINT32 CvFlowRegisterID = 0U;
                        Rval |= SvcCvFlow_Register(i, SvcRctaTask_DetectionResultCallback, &CvFlowRegisterID);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_RCTA_TASK, "SvcRctaTask_Init err. SvcCvFlow_Register failed %d", Rval, 0U);
                        } else {
                            CropInfo.SrcW = pCvFlow[i].InputCfg.Input[0].FrameWidth;
                            CropInfo.SrcH = pCvFlow[i].InputCfg.Input[0].FrameHeight;
                            CropInfo.RoiW = 1280U;
                            CropInfo.RoiH = 512U;
                            CropInfo.RoiX = pCvFlow[i].InputCfg.Input[0].Roi[0].StartX;
                            CropInfo.RoiY = pCvFlow[i].InputCfg.Input[0].Roi[0].StartY;
                            SvcRctaInit = 1U;
                        }
                    }
                }
            }
        }

    }
    return Rval;
}

UINT32 SvcRctaTask_Start(void)
{
    UINT32 RetVal;

    if (RctaCalibLoad == 1U) {
        if (SvcRctaInit == 1U) {
            //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS, SVC_WARN_ICON_FLG_ENA);
            //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            SvcRctaOn = 1U;
            RetVal = SVC_OK;
        } else {
            SvcLog_NG(SVC_LOG_RCTA_TASK, "RCTA module not init yet. Cannot start", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_RCTA_TASK, "Abort ADAS-RCTA enable due to calib data not in NAND", 0U, 0U);
        RetVal = SVC_NG;
    }
    return RetVal;
}

UINT32 SvcRctaTask_Stop(void)
{
    UINT32 RetVal;

    if (SvcRctaInit == 1U) {
        SvcRctaOn = 0U;
        //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS,   SVC_WARN_ICON_FLG_ENA);
        //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L, SVC_WARN_ICON_FLG_ENA);
        //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R, SVC_WARN_ICON_FLG_ENA);
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_RCTA_TASK, "RCTA module not init yet. Cannot start", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

static void SvcRctaTask_DetectionResultCallback(UINT32 Chan, UINT32 OutputTag, void *pDetResMsg)
{
    SVC_CV_DETRES_SEG_s SegResult;
    const SVC_CV_DETRES_SEG_s* pSegResult;
    UINT32 FovIdx = 0U;
    UINT8 RctaEnable = 0U;

    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&OutputTag);

    if (SvcRctaOn == 1U) {
        const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
        const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

        FovIdx = pCvFlow[Chan].InputCfg.Input[0].StrmId;
        if ((pResCfg->FovNum > 1U) && ((FovIdx == 0U) || (FovIdx == 1U))) {
            RctaEnable = 1U;
            WarnResult.WarnChan = FovIdx;
        } else {
            RctaEnable = 0U;
        }
        (void) pDetResMsg;
        if (RctaEnable == 1U) {
            AmbaMisra_TypeCast(&pSegResult, &pDetResMsg);
            SegResult.Width       = pSegResult->Width;
            SegResult.Height      = pSegResult->Height;
            SegResult.Pitch       = pSegResult->Pitch;
            SegResult.FrameNum    = pSegResult->FrameNum;
            SegResult.CaptureTime = pSegResult->CaptureTime;
            SegResult.MsgCode = FovIdx;
            AmbaMisra_TypeCast(&SegResult.pBuf, &pSegResult->pBuf);

            if (SVC_OK != AmbaKAL_MsgQueueSend(&SvcRctaQueId, &SegResult, AMBA_KAL_WAIT_FOREVER)) {
                SvcLog_NG(SVC_LOG_RCTA_TASK, "AmbaKAL_MsgQueueSend failed", 0U, 0U);
            } else {
                //Do nothing
            }
        }
    } else {
        //Do nothing
    }
}

