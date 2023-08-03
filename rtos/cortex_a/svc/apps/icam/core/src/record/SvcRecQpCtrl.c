/**
 *  @file SvcRecQpCtrl.c
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
 *  @details svc record qp control
 */

#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"
#include "AmbaDef.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaVIN_Def.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcRecMain.h"
#include "SvcTask.h"
#include "SvcUtil.h"
#include "SvcResCfg.h"
#include "SvcRecQpCtrl.h"

#ifdef CONFIG_BUILD_IMGFRW_AAA
#include "AmbaImg_External_CtrlFunc.h"
#include "SvcImg.h"
#endif

#define SVC_LOG_RQC                     "RQC"
#define SVC_REC_QP_CTRL_STACK_SIZE      (0x2000U)

#define QP_OP_NONE                      (0x00U)
#define QP_OP_MIN_DECR                  (0x01U)
#define QP_OP_MIN_INCR                  (0x02U)
#define QP_OP_AQP_UPTE                  (0x04U)
#define QP_OP_INIT                      (0x08U)

#define QP_CTRL_FLG_START               (1U)

#define QP_CTRL_QP_INIT_MIN_I           (14U)
#define QP_CTRL_QP_INIT_MAX_I           (51U)
#define QP_CTRL_QP_INIT_RDC_I           (6U)
#define QP_CTRL_QP_INIT_MIN_P           (17U)
#define QP_CTRL_QP_INIT_MAX_P           (51U)
#define QP_CTRL_QP_INIT_RDC_P           (3U)
#define QP_CTRL_QP_INIT_MIN_B           (21U)
#define QP_CTRL_QP_INIT_MAX_B           (51U)

#define QP_CTRL_AQP_INIT                (2U)

#define QP_CTRL_BRATE_INTERVAL          (1000U)     /*ms*/
#define QP_CTRL_ADJUST_UNIT             (1U)

#define QPC_DBG_PRN                     0U
static inline void QPC_DBG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#if QPC_DBG_PRN
    SvcLog_DBG(SVC_LOG_RQC, pFormat, Arg1, Arg2);
#else
    ULONG Temp = 0U;
    AmbaMisra_TypeCast(&Temp, pFormat);
    AmbaMisra_TouchUnused(&Arg1);
    AmbaMisra_TouchUnused(&Arg2);
#endif
}

typedef struct {
    UINT8     QpMinIFrame;
    UINT8     QpMinPFrame;
    UINT8     QpMinBFrame;
    UINT32    Enable;
    UINT32    StableCnt;
    UINT64    DefaultBitRate;
    UINT32    AQPValue;
} SVC_REC_QP_CTRL_INFO_s;

static SVC_REC_QP_CTRL_INFO_s    CtrlInfo[CONFIG_ICAM_MAX_REC_STRM] = {0U};
static AMBA_KAL_EVENT_FLAG_t     QpCtrlEventFlag GNU_SECTION_NOZEROINIT;
static SVC_TASK_CTRL_s           TaskCtrl GNU_SECTION_NOZEROINIT;
static UINT32                    TaskExist = 0U;
static UINT32                    StartBits = 0U;

static void QpChanged(UINT16 StreamID, UINT32 Operation, UINT32 Param)
{
    UINT8                         CtrlUnit = QP_CTRL_ADJUST_UNIT, MinQp;
    UINT32                        Rval = SVC_NG;
    SVC_REC_QP_CTRL_INFO_s        *pCtrlInfo;
    AMBA_DSP_VIDEO_ENC_QUALITY_s  DspCtrl;

    pCtrlInfo = &(CtrlInfo[StreamID]);

    if (0U < (Operation & QP_OP_MIN_DECR)) {
        if ((pCtrlInfo->QpMinIFrame >= CtrlUnit) &&
            (pCtrlInfo->QpMinPFrame >= CtrlUnit) &&
            (pCtrlInfo->QpMinBFrame >= CtrlUnit)) {
                pCtrlInfo->QpMinIFrame -= CtrlUnit;
                pCtrlInfo->QpMinPFrame -= CtrlUnit;
                pCtrlInfo->QpMinBFrame -= CtrlUnit;
                Rval = SVC_OK;
        } else {
            QPC_DBG("QP Minus failed!", 0U, 0U);
        }
    }

    if (0U < (Operation & QP_OP_MIN_INCR)) {
        if ((pCtrlInfo->QpMinIFrame <= (QP_CTRL_QP_INIT_MAX_I - CtrlUnit)) &&
            (pCtrlInfo->QpMinPFrame <= (QP_CTRL_QP_INIT_MAX_P - CtrlUnit)) &&
            (pCtrlInfo->QpMinBFrame <= (QP_CTRL_QP_INIT_MAX_B - CtrlUnit))) {
                pCtrlInfo->QpMinIFrame += CtrlUnit;
                pCtrlInfo->QpMinPFrame += CtrlUnit;
                pCtrlInfo->QpMinBFrame += CtrlUnit;
                Rval = SVC_OK;
        } else {
            QPC_DBG("QP Plus failed!", 0U, 0U);
        }
    }

    if (0U < (Operation & QP_OP_AQP_UPTE)) {
        /* mapping between aqp value from IQ table */
        #define MAX_AQP_NUM         (9U)
        UINT32        NewAQPValue;
        const UINT32  AQPMapping[MAX_AQP_NUM] = {QP_CTRL_AQP_INIT, 0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U};

        if (Param < MAX_AQP_NUM) {
            NewAQPValue = AQPMapping[Param];
            if (pCtrlInfo->AQPValue != NewAQPValue) {
                pCtrlInfo->AQPValue = NewAQPValue;
                Rval = SVC_OK;
            }
        }
    }

    /* init deafult value */
    if (0U < (Operation & QP_OP_INIT)) {
        Rval = SVC_OK;
    }

    if (Rval == SVC_OK) {
        if (OK != AmbaWrap_memset(&DspCtrl, 0, sizeof(AMBA_DSP_VIDEO_ENC_QUALITY_s))) {
            SvcLog_NG(SVC_LOG_RQC, "AmbaWrap_memcpy failed", 0U, 0U);
        }

        DspCtrl.QPCtrl               = 1U;
        DspCtrl.QpCfg.QpMinIFrame    = pCtrlInfo->QpMinIFrame;
        DspCtrl.QpCfg.QpMaxIFrame    = QP_CTRL_QP_INIT_MAX_I;
        DspCtrl.QpCfg.QpMinPFrame    = pCtrlInfo->QpMinPFrame;
        DspCtrl.QpCfg.QpMaxPFrame    = QP_CTRL_QP_INIT_MAX_P;
        DspCtrl.QpCfg.QpMinBFrame    = pCtrlInfo->QpMinBFrame;
        DspCtrl.QpCfg.QpMaxBFrame    = QP_CTRL_QP_INIT_MAX_B;
        DspCtrl.QpCfg.QpReduceIFrame = QP_CTRL_QP_INIT_RDC_I;
        DspCtrl.QpCfg.QpReducePFrame = QP_CTRL_QP_INIT_RDC_P;
        DspCtrl.QpCfg.Aqp            = (UINT8)pCtrlInfo->AQPValue;

        MinQp = 10U + (3U * DspCtrl.QpCfg.Aqp / 2U);
        if (DspCtrl.QpCfg.QpMinIFrame < MinQp) {
            DspCtrl.QpCfg.QpMinIFrame = MinQp;
        }

        if (DspCtrl.QpCfg.QpMinPFrame < MinQp) {
            DspCtrl.QpCfg.QpMinPFrame = MinQp;
        }

        if (DspCtrl.QpCfg.QpMinBFrame < MinQp) {
            DspCtrl.QpCfg.QpMinBFrame = MinQp;
        }

        Rval = AmbaDSP_VideoEncControlQuality(1U, &StreamID, &DspCtrl);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_RQC, "AmbaDSP_VideoEncControlQuality failed(%u)", Rval, 0U);
        } else {
            QPC_DBG("QpUpdate[%u]: Min I(%u)", StreamID, (UINT32)pCtrlInfo->QpMinIFrame);
            QPC_DBG("              Min P(%u), Min B(%u)", (UINT32)pCtrlInfo->QpMinPFrame, (UINT32)pCtrlInfo->QpMinBFrame);
        }
    }
}

static UINT32 QpCtrlCheck(void)
{
    UINT32 Rval = SVC_OK, ActualFlags, Err;

    Err = AmbaKAL_EventFlagGet(&QpCtrlEventFlag, QP_CTRL_FLG_START, AMBA_KAL_FLAGS_ALL,
                                AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_NO_WAIT);
    if (Err != KAL_ERR_NONE) {
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        if (CheckBits(ActualFlags, QP_CTRL_FLG_START) == 0U) {
            Rval = SVC_NG;
        }
    }

    return Rval;
}

static void* QpCtrlTask(void* EntryArg)
{
    UINT16                      StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM] = {0U};
    UINT32                      LoopU = 1U, StreamBits = 0U, i, StreamNum = 0U, ActualFlags = 0U, Break = 0U;
    UINT32                      QpCtrl, Param, Err;
    UINT64                      BitRate;
    SVC_REC_QP_CTRL_INFO_s      *pCtrlInfo;
    AMBA_RSC_STATIS_s           StartRscStatis[CONFIG_ICAM_MAX_REC_STRM];
    AMBA_RSC_STATIS_s           EndRscStatis[CONFIG_ICAM_MAX_REC_STRM];

    AmbaMisra_TouchUnused(EntryArg);
    SvcLog_OK(SVC_LOG_RQC, "QpCtrlTask created", 0U, 0U);

    if (OK != AmbaWrap_memset(StartRscStatis, 0, sizeof(StartRscStatis))) {
        SvcLog_NG(SVC_LOG_RQC, "AmbaWrap_memset failed", 0U, 0U);
    }

    if (OK != AmbaWrap_memset(EndRscStatis, 0, sizeof(EndRscStatis))) {
        SvcLog_NG(SVC_LOG_RQC, "AmbaWrap_memset failed", 0U, 0U);
    }

    while (1U == LoopU) {
        Err = AmbaKAL_EventFlagGet(&QpCtrlEventFlag, QP_CTRL_FLG_START, AMBA_KAL_FLAGS_ALL,
                                   AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_RQC, "AmbaKAL_EventFlagGet QP_CTRL_FLG_START failed %u", Err, 0U);
            LoopU = 0U;
        } else {
            SvcLog_OK(SVC_LOG_RQC, "QpCtrlTask start", 0U, 0U);
        }

        while (1U == LoopU) {
            Break = QpCtrlCheck();
            if (SVC_OK == Break) {
                SvcRecMain_Control(SVC_RCM_GET_ENC_STATUS, 0U, StrmIdxArr, &StreamBits);
            }

            SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, StreamBits, &StreamNum, StrmIdxArr);

            Break = QpCtrlCheck();
            if (SVC_OK == Break) {
                SvcRecMain_Control(SVC_RCM_GET_VID_STATIS, (UINT16)StreamNum, StrmIdxArr, StartRscStatis);
            }

            Err = AmbaKAL_TaskSleep(QP_CTRL_BRATE_INTERVAL);
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(SVC_LOG_RQC, "AmbaKAL_TaskSleep failed %u", Err, 0U);
            }

            Break = QpCtrlCheck();
            if (SVC_OK == Break) {
                SvcRecMain_Control(SVC_RCM_GET_VID_STATIS, (UINT16)StreamNum, StrmIdxArr, EndRscStatis);
            }

            if (Break == SVC_NG) {
                QPC_DBG("QpCtrlTask stop", 0U, 0U);
                break;
            }

            for (i = 0U; i < StreamNum; i++) {
                pCtrlInfo = &(CtrlInfo[StrmIdxArr[i]]);
                if (pCtrlInfo->Enable == SVC_REC_QP_CTRL_ENABLE) {
                    QpCtrl = QP_OP_NONE;
                    Param = 0U;

                    /* bitrate monitor */
                    BitRate = EndRscStatis[i].RxSize - StartRscStatis[i].RxSize;
                    BitRate = (BitRate * (UINT64)8000U) / (UINT64)(QP_CTRL_BRATE_INTERVAL);
                    QPC_DBG("BitRate(%u) = %u", (UINT32)StrmIdxArr[i], (UINT32)BitRate);
                    if (BitRate != 0ULL) {
                        if (BitRate > ((pCtrlInfo->DefaultBitRate * 11ULL) / 10ULL)) {
                            QPC_DBG("QP Add (%u/%u)", (UINT32)BitRate, (UINT32)pCtrlInfo->DefaultBitRate);
                            QpCtrl |= QP_OP_MIN_INCR;
                        } else if (BitRate < ((pCtrlInfo->DefaultBitRate * 9ULL) / 10ULL)) {
                            QPC_DBG("QP Minus (%u/%u)", (UINT32)BitRate, (UINT32)pCtrlInfo->DefaultBitRate);
                            QpCtrl |= QP_OP_MIN_DECR;
                        } else {
                            pCtrlInfo->StableCnt++;
                        }
                    }

#ifdef CONFIG_BUILD_IMGFRW_AAA
                    /* AQP monitor */
                    {
                        UINT32           VinID = 0U, SensorId = 0U, AlgoId = 0U, RetVal, v, FovBits;
                        ADJ_AQP_INFO_s   AQPInfo = {0U};

                        SvcResCfg_GetFovBitsOfRecIdx(StrmIdxArr[i], &FovBits);
                        for (v = 0U; v < AMBA_DSP_MAX_VIEWZONE_NUM; v++) {
                            if (0U < (FovBits & ((UINT32)0x01U << v))) {
                                RetVal = SvcResCfg_GetSensorIdxOfFovIdx(v, &VinID, &SensorId);
                                if (RetVal == SVC_OK) {
                                    RetVal = SvcImg_AlgoIdGet(VinID, SensorId, &AlgoId);
                                }
                                if (RetVal == SVC_OK) {
                                    RetVal = AmbaImgProc_Get_AQPInfo(AlgoId, &AQPInfo);
                                }
                                if (RetVal == OK) {
                                    Param = (UINT32)AQPInfo.AQPParams.Value[0];
                                    if (Param < MAX_AQP_NUM) {
                                        QpCtrl |= QP_OP_AQP_UPTE;
                                        break;
                                    }
                                }
                            }
                        }
                    }
#endif

                    if (QpCtrl != QP_OP_NONE) {
                        QpChanged(StrmIdxArr[i], QpCtrl, Param);
                    }
                }
            }
        }
    }

    return NULL;
}


/**
* create qp control task
* @param [in] Priority Task priority
* @param [in] CpuBits The task is running on which core
* @return ErrorCode
*/
UINT32 SvcRecQpCtrlCreate(UINT32 Priority, UINT32 CpuBits)
{
    UINT32           Err, Rval = SVC_OK;
    static UINT8     RecQpCtrlStack[SVC_REC_QP_CTRL_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char      FlagName[] = "SvcRecQpCtrFlag";

    if (TaskExist == 0U) {
        Err = AmbaKAL_EventFlagCreate(&QpCtrlEventFlag, FlagName);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_RQC, "create flag failed!(%u)", Err, 0U);
            Rval = SVC_NG;
        }

        /* task create */
        if (SVC_OK == Rval) {
            TaskCtrl.Priority    = Priority;
            TaskCtrl.EntryFunc   = QpCtrlTask;
            TaskCtrl.EntryArg    = 0U;
            TaskCtrl.pStackBase  = RecQpCtrlStack;
            TaskCtrl.StackSize   = SVC_REC_QP_CTRL_STACK_SIZE;
            TaskCtrl.CpuBits     = CpuBits;

            Err = SvcTask_Create("RecQpCtrl_Task", &TaskCtrl);
            if (SVC_OK != Err) {
                SvcLog_NG(SVC_LOG_RQC, "RecQpCtrl_Task created failed!(%u)", Err, 0U);
                Rval = SVC_NG;
            } else {
                TaskExist = 1U;
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_RQC, "RecQpCtrl_Task is already existed!", 0U, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

/**
* destroy qp control task
* @return ErrorCode
*/
UINT32 SvcRecQpCtrlDestroy(void)
{
    UINT32 Err, Rval = SVC_OK;

    /* task delete */
    Err = SvcTask_Destroy(&TaskCtrl);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_RQC, "RecQpCtrl_Task created failed!(%u)", Err, 0U);
        Rval = SVC_NG;
    }

    /* flag delete */
    Err = AmbaKAL_EventFlagDelete(&QpCtrlEventFlag);
    if (Err != KAL_ERR_NONE) {
        SvcLog_NG(SVC_LOG_RQC, "delete flag failed(%u)", Err, 0U);
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        SvcLog_OK(SVC_LOG_RQC, "RecQpCtrl_Task is destroyed", 0U, 0U);
        TaskExist = 0U;
    }

    return Rval;
}

/**
* Start qp control task
* @param [in] StreamBits StreamBits
*/
void SvcRecQpCtrlStart(UINT32 StreamBits)
{
    UINT32  i, StreamNum = 0U;
    UINT16  StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM] = {0U};

    StartBits = SetBits(StartBits, StreamBits);
    if ((TaskExist == 1U) && (StartBits != 0U)) {
        SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, StreamBits, &StreamNum, StrmIdxArr);
        for (i = 0U; i < StreamNum; i++) {
            QpChanged(StrmIdxArr[i], QP_OP_INIT, 0U);
        }

        if (KAL_ERR_NONE != AmbaKAL_EventFlagSet(&QpCtrlEventFlag, QP_CTRL_FLG_START)) {
            SvcLog_NG(SVC_LOG_RQC, "set flag failed!", 0U, 0U);
        }
    }
}

/**
* Stop qp control task
* @param [in] StreamBits StreamBits
*/
void SvcRecQpCtrlStop(UINT32 StreamBits)
{
    UINT32 i, StreamNum = 0U;
    UINT16 StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM] = {0U};

    StartBits = ClearBits(StartBits, StreamBits);

    SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, StreamBits, &StreamNum, StrmIdxArr);

    /* reset qp value */
    for (i = 0U; i < StreamNum; i++) {
        SvcRecQpCtrlCfg(StrmIdxArr[i], CtrlInfo[StrmIdxArr[i]].Enable, (UINT32)CtrlInfo[StrmIdxArr[i]].DefaultBitRate);
    }

    if ((TaskExist == 1U) && (StartBits == 0U)) {
        if (KAL_ERR_NONE != AmbaKAL_EventFlagClear(&QpCtrlEventFlag, QP_CTRL_FLG_START)) {
            SvcLog_NG(SVC_LOG_RQC, "clear flag failed!", 0U, 0U);
        }
    }
}

/**
* Config for the stream
* @param [in] StreamID record stream id
* @param [in] Enable enable(1U), disable(0U)
* @param [in] DefaultBitRate default bitrate for this stream
*/
void SvcRecQpCtrlCfg(UINT16 StreamID, UINT32 Enable, UINT32 DefaultBitRate)
{
    SVC_REC_QP_CTRL_INFO_s *pCtrlInfo;

    if (StreamID < (UINT16)CONFIG_ICAM_MAX_REC_STRM) {
        pCtrlInfo = &(CtrlInfo[StreamID]);

        if (OK != AmbaWrap_memset(pCtrlInfo, 0, sizeof(SVC_REC_QP_CTRL_INFO_s))) {
            SvcLog_NG(SVC_LOG_RQC, "AmbaWrap_memcpy failed", 0U, 0U);
        }

        pCtrlInfo->Enable          = Enable;
        pCtrlInfo->DefaultBitRate  = (UINT64)DefaultBitRate;
        pCtrlInfo->QpMinIFrame     = QP_CTRL_QP_INIT_MIN_I;
        pCtrlInfo->QpMinPFrame     = QP_CTRL_QP_INIT_MIN_P;
        pCtrlInfo->QpMinBFrame     = QP_CTRL_QP_INIT_MIN_B;
        pCtrlInfo->AQPValue        = QP_CTRL_AQP_INIT;
    } else {
        SvcLog_NG(SVC_LOG_RQC, "not support stream%u", (UINT32)StreamID, 0U);
    }

}
