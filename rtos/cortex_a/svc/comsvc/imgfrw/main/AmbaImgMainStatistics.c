/**
 *  @file AmbaImgMainStatistics.c
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Amba Image Main Statistics
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"

#include "AmbaKAL.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_Image3aStatistics.h"

#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaImgFramework.h"
//#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"
#include "AmbaImgMainStatistics.h"

#define OK_UL    ((UINT32) 0U)
#define NG_UL    ((UINT32) 1U)

//#define AMBA_IMG_MAIN_STATISTICS_VIN_HISTO     (1U)

typedef union /*_STATISTICS_MEM_INFO_u_*/ {
    AMBA_IMG_MAIN_MEM_ADDR     Data;
    void                       *pVoid;
    const void                 *pCvoid;
    AMBA_IK_CFA_3A_DATA_s      *pCfa;
    AMBA_IK_PG_3A_DATA_s       *pRgb;
#ifdef AMBA_IMG_MAIN_STATISTICS_VIN_HISTO
    AMBA_IK_CFA_HIST_STAT_s    *pHisto;
#endif
} STATISTICS_MEM_INFO_u;

typedef struct /*_STATISTICS_MEM_INFO_s_*/ {
    STATISTICS_MEM_INFO_u Ctx;
} STATISTICS_MEM_INFO_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaImgMainStatisticsTaskId;
static void AmbaImgMainStatistics_Task(UINT32 Param);
static void *AmbaImgMainStatistics_TaskEx(void *pParam);

static AMBA_KAL_EVENT_FLAG_t GNU_SECTION_NOZEROINIT AmbaImgMainStatistics_Event;
static const void *CfaInfo[AMBA_IMG_NUM_FOV_CHANNEL];
static const void *RgbInfo[AMBA_IMG_NUM_FOV_CHANNEL];
static UINT32 AmbaImgMainStatistics_RawSeqNum[AMBA_IMG_NUM_FOV_CHANNEL];
static UINT32 AmbaImgMainStatistics_RepeatCnt[AMBA_IMG_NUM_FOV_CHANNEL];
static UINT32 AmbaImgMainStatistics_ExceedCnt;
#ifdef AMBA_IMG_MAIN_STATISTICS_VIN_HISTO
static const void *HistoInfo[AMBA_IMG_NUM_FOV_CHANNEL];
#endif
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainStatistics_Ready;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainStatistics_Go;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainStatistics_Ack;

static UINT32 AmbaImgMainStatistics_Cmd = (UINT32) STATISTICS_TASK_CMD_STOP;

#define AMBA_IMG_MAIN_STATISTICS_PUSH_EVENT_FLAG    0x80000000U

static UINT32 AmbaImgMainStatistics_Mask = 0xFFFFFFFFU;

static UINT32 AmbaImgMainStatistics_DummyFlag;

UINT32 AmbaImgMainStatistics_DebugCnt = 0U;

/**
 *  @private
 *  Amba image main statistics create
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainStatistics_Create(UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_IMG_MAIN_STATISTICS_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaImgMainStatistics_TaskStack[AMBA_IMG_MAIN_STATISTICS_TASK_STACK_SIZE];
    static char AmbaImgMainStatistics_TaskName[] = "ImageStatistics";

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* event */
    FuncRetCode = AmbaKAL_EventFlagCreate(&AmbaImgMainStatistics_Event, NULL);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    /* semaphore */
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainStatistics_Ready, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainStatistics_Go, NULL, 0);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainStatistics_Ack, NULL, 0);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    /* task */
    if (RetCode == OK) {
        AmbaImgPrintStr(PRINT_FLAG_MSG, "image create statistics task");
        {
            STATISTICS_MEM_INFO_s MemInfo;
            MemInfo.Ctx.Data = 0U;
            FuncRetCode = AmbaKAL_TaskCreate(&AmbaImgMainStatisticsTaskId,
                                             AmbaImgMainStatistics_TaskName,
                                             Priority,
                                             AmbaImgMainStatistics_TaskEx,
                                             MemInfo.Ctx.pVoid,
                                             AmbaImgMainStatistics_TaskStack,
                                             AMBA_IMG_MAIN_STATISTICS_TASK_STACK_SIZE,
                                             AMBA_KAL_DONT_START);
        }
        if (FuncRetCode == OK) {
            FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaImgMainStatisticsTaskId, CoreInclusion);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskResume(&AmbaImgMainStatisticsTaskId);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main statistics delete
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainStatistics_Delete(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* task */
    FuncRetCode = AmbaKAL_TaskTerminate(&AmbaImgMainStatisticsTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_TaskDelete(&AmbaImgMainStatisticsTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    /* event */
    FuncRetCode = AmbaKAL_EventFlagDelete(&AmbaImgMainStatistics_Event);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    /* semaphore */
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainStatistics_Ready);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainStatistics_Go);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainStatistics_Ack);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main statistics init
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainStatistics_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    /* statistics task dummy flag init */
    AmbaImgMainStatistics_DummyFlag = 1U;
    /* cfa/rgb pointer init */
    for(i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
        CfaInfo[i] = NULL;
        RgbInfo[i] = NULL;
    }
    /* raw seq num init */
    for(i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
        AmbaImgMainStatistics_RawSeqNum[i] = 0xFFFFFFFFU;
        AmbaImgMainStatistics_RepeatCnt[i] = 0U;
    }
    AmbaImgMainStatistics_ExceedCnt = 0U;
#ifdef AMBA_IMG_MAIN_STATISTICS_VIN_HISTO
    /* histo pointer init */
    for(i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
        HistoInfo[i] = NULL;
    }
#endif
    /* statistics debug count reset */
    if (AmbaImgMain_DebugCount == 0U) {
        AmbaImgMainStatistics_DebugCnt = 0xFFFFFFFFU;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main statistics active
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainStatistics_Active(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (AmbaImgMainStatistics_Cmd == (UINT32) STATISTICS_TASK_CMD_STOP) {
        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainStatistics_Ready, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* cmd start */
            AmbaImgMainStatistics_Cmd = (UINT32) STATISTICS_TASK_CMD_START;
            /* task go */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainStatistics_Go);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* ack take */
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainStatistics_Ack, AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main statistics inactive
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainStatistics_Inactive(void)
{
    if (AmbaImgMainStatistics_Cmd == (UINT32) STATISTICS_TASK_CMD_START) {
        AmbaImgMainStatistics_Cmd = (UINT32) STATISTICS_TASK_CMD_STOP;
    }

    return OK_UL;
}

/**
 *  @private
 *  Amba image main statistics push
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainStatistics_Push(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (AmbaImgMainStatistics_Cmd == (UINT32) STATISTICS_TASK_CMD_STOP) {
        FuncRetCode = AmbaKAL_EventFlagSet(&AmbaImgMainStatistics_Event, AMBA_IMG_MAIN_STATISTICS_PUSH_EVENT_FLAG);
        if (FuncRetCode != KAL_ERR_NONE) {
            /* */
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main statistics idle wait
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainStatistics_IdleWait(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainStatistics_Ready, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainStatistics_Ready);
        if (FuncRetCode != KAL_ERR_NONE) {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image main statistics cfa
 *  @param[in] pData pointer to the CFA statistics data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainStatistics_Cfa(const void *pData)
{
    UINT32 FovId;

    STATISTICS_MEM_INFO_s MemInfo;

    MemInfo.Ctx.pCvoid = pData;

    /* fov id get */
    FovId = MemInfo.Ctx.pCfa->Header.ChanIndex;
    /* fov check */
    if (FovId < AMBA_IMG_NUM_FOV_CHANNEL) {
        CfaInfo[FovId] = pData;
    }

    return OK;
}

/**
 *  Amba image main statistics rgb
 *  @param[in] pData pointer to the RGB statistics data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainStatistics_Rgb(const void *pData)
{
    UINT32 FuncRetCode;

    UINT32 FovId;
    UINT32 RawSeqNum;

    STATISTICS_MEM_INFO_s MemInfo;

    MemInfo.Ctx.pCvoid = pData;

    /* fov id get */
    FovId = MemInfo.Ctx.pRgb->Header.ChanIndex;
    /* fov check */
    if (FovId < AMBA_IMG_NUM_FOV_CHANNEL) {
        /* rgb put */
        RgbInfo[FovId] = pData;
        /* raw seq nem get */
        RawSeqNum = MemInfo.Ctx.pRgb->Header.RawPicSeqNum;
        /* raw seq num check */
        if (RawSeqNum != AmbaImgMainStatistics_RawSeqNum[FovId]) {
            /* statistics event put */
            FuncRetCode = AmbaKAL_EventFlagSet(&AmbaImgMainStatistics_Event, ((UINT32) 1U) << (UINT32) FovId);
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
            /* raw seq num update */
            AmbaImgMainStatistics_RawSeqNum[FovId] = RawSeqNum;
        } else {
            /* statistics repeat */
            AmbaImgMainStatistics_RepeatCnt[FovId]++;
        }
    } else {
        /* fov id exceed */
        AmbaImgMainStatistics_ExceedCnt++;
    }

    return OK;
}
#ifdef AMBA_IMG_MAIN_STATISTICS_VIN_HISTO
/**
 *  Amba image main statistics histogram
 *  @param[in] pData pointer to the histogram data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainStatistics_Histo(const void *pData)
{
    STATISTICS_MEM_INFO_s MemInfo;

    MemInfo.Ctx.pCvoid = pData;
    HistoInfo[MemInfo.Ctx.pHisto->Header.ChannelIndex] = pData;

    return OK;
}
#endif

/**
 *  @private
 *  Amba image main statistics mask set
 *  @param[in] Mask statistics fov mask (bits)
 *  @note this function is intended for internal use only
 */
void AmbaImgMainStatistics_MaskSet(UINT32 Mask)
{
    /* statistics fov mask */
    AmbaImgMainStatistics_Mask = Mask;
}

/**
 *  Amba image main statistics task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaImgMainStatistics_TaskEx(void *pParam)
{
    STATISTICS_MEM_INFO_s MemInfo;

    MemInfo.Ctx.pVoid = pParam;
    AmbaImgMainStatistics_Task((UINT32) MemInfo.Ctx.Data);

    return NULL;
}

/**
 *  Amba image main statistics task
 *  @param[in] Param unused data
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainStatistics_Task(UINT32 Param)
{
    UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;
    UINT32 i, k;

    UINT32 ActualFlags;
    UINT32 RawCapSeq;

    STATISTICS_MEM_INFO_s MemInfo;

    AmbaImgMainStatistics_Cmd = (UINT32) STATISTICS_TASK_CMD_STOP;
    (void) Param;

    while (AmbaImgMainStatistics_DummyFlag == 1U) {
        if (AmbaImgMainStatistics_Cmd == (UINT32) STATISTICS_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                /* task first time create */
                FirstActive = 0U;
            } else {
                /* unregister cfa event */
                FuncRetCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_CFA_AAA_RDY, AmbaImgMainStatistics_Cfa);
                if (FuncRetCode != OK) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics cfa event unregister");
                }
                /* unregister rgb event */
                FuncRetCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_PG_AAA_RDY, AmbaImgMainStatistics_Rgb);
                if (FuncRetCode != OK) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics rgb event unregister");
                }
#ifdef AMBA_IMG_MAIN_STATISTICS_VIN_HISTO
                /* unregister histo event */
                FuncRetCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_HIST_AAA_RDY, AmbaImgMainStatistics_Histo);
                if (FuncRetCode != OK) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics histo event unregister");
                }
#endif
            }
            /* ready give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainStatistics_Ready);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics task idle");
            }
            /* go take */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainStatistics_Go, AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics task go");
            }

            /* even clear */
            FuncRetCode = AmbaKAL_EventFlagClear(&AmbaImgMainStatistics_Event, 0xFFFFFFFFU);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics task event clear");
            }
            /* raw seq num reset */
            for(i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                AmbaImgMainStatistics_RawSeqNum[i] = 0xFFFFFFFFU;
                AmbaImgMainStatistics_RepeatCnt[i] = 0U;
            }
            AmbaImgMainStatistics_ExceedCnt = 0U;
            /* register cfa event */
            FuncRetCode = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_CFA_AAA_RDY, AmbaImgMainStatistics_Cfa);
            if (FuncRetCode != OK) {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics cfa event register");
            }
            /* register rgb event */
            FuncRetCode = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_PG_AAA_RDY, AmbaImgMainStatistics_Rgb);
            if (FuncRetCode != OK) {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics rgb event register");
            }
#ifdef AMBA_IMG_MAIN_STATISTICS_VIN_HISTO
            /* register histo event */
            FuncRetCode = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_HIST_AAA_RDY, AmbaImgMainStatistics_Histo);
            if (FuncRetCode != OK) {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics histo event register");
            }
#endif
            /* ack */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainStatistics_Ack);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics task ack");
            }
        }

        FuncRetCode = AmbaKAL_EventFlagGet(&AmbaImgMainStatistics_Event, 0xFFFFFFFFU, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            if (AmbaImgMainStatistics_Cmd == (UINT32) STATISTICS_TASK_CMD_STOP) {
                /* stop */
                continue;
            }
            /* statistics fov mask */
            if (ActualFlags > 0U) {
                ActualFlags &= AmbaImgMainStatistics_Mask;
            }
            /* flags check */
            if (ActualFlags > 0U) {
                for (k = 0U; k < AMBA_IMG_NUM_FOV_CHANNEL; k++) {
                    if ((ActualFlags >> k) == 0U) {
                        break;
                    }
                    if ((ActualFlags & (((UINT32) 1U) << k)) > 0U) {
                        /* raw cap seq get */
                        MemInfo.Ctx.pCvoid = CfaInfo[k];
                        if (MemInfo.Ctx.pCvoid != NULL) {
                            RawCapSeq = MemInfo.Ctx.pCfa->Header.RawPicSeqNum;
                        } else {
                            MemInfo.Ctx.pCvoid = RgbInfo[k];
                            if (MemInfo.Ctx.pCvoid != NULL) {
                                RawCapSeq = MemInfo.Ctx.pRgb->Header.RawPicSeqNum;
                            } else {
                                RawCapSeq = 0U;
                            }
                        }
                        /* statistics put */
                        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
                            if ((pAmbaImgMainChannel != NULL) && (pAmbaImgMainChannel[i] != NULL)) {
                                if (AmbaImgMainStatistics_DebugCnt < AmbaImgMain_DebugCount) {
                                    AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                                    AmbaImgPrint2(PRINT_FLAG_DBG, "statistics entry", i, k);
                                }
                                /* statistics entry */
                                FuncRetCode = AmbaImgStatistics_Entry(i, ((UINT32) 1U) << k, CfaInfo, RgbInfo, RawCapSeq);
                                if (FuncRetCode != OK_UL) {
                                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: statistics entry", i, k);
                                }
                            }
                        }
                    }
                }
            }
            /* debug only */
            if (AmbaImgMainStatistics_DebugCnt < AmbaImgMain_DebugCount) {
                for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                    if ((ActualFlags >> i) == 0U) {
                        break;
                    }
                    if ((ActualFlags & (((UINT32) 1U) << i)) > 0U) {
                        AmbaImgPrintEx2(PRINT_FLAG_DBG, "statistics index", i, 10U, AmbaImgMainStatistics_RepeatCnt[i], 10U);
                        MemInfo.Ctx.pCvoid = CfaInfo[i];
                        if (MemInfo.Ctx.pCvoid != NULL) {
                            AmbaImgPrint64Ex2(PRINT_FLAG_DBG, "statistics cfa", (UINT64) MemInfo.Ctx.pCfa->Header.ChanIndex, 10U, (UINT64) MemInfo.Ctx.Data, 16U);
                            AmbaImgPrintEx2(PRINT_FLAG_DBG, "frame id", MemInfo.Ctx.pCfa->Header.ChanIndex, 10U, MemInfo.Ctx.pCfa->FrameId, 16U);
                            AmbaImgPrintEx2(PRINT_FLAG_DBG, "raw cap seq", MemInfo.Ctx.pCfa->Header.ChanIndex, 16U, MemInfo.Ctx.pCfa->Header.RawPicSeqNum, 16U);
                            AmbaImgPrintEx2(PRINT_FLAG_DBG, "cfa iso config tag", MemInfo.Ctx.pCfa->Header.ChanIndex, 10U, AAA_HEADER_CFA_ISO_TAG, 16U);
                        }
                        MemInfo.Ctx.pCvoid = RgbInfo[i];
                        if (MemInfo.Ctx.pCvoid != NULL) {
                            AmbaImgPrint64Ex2(PRINT_FLAG_DBG, "statistics rgb", (UINT64) MemInfo.Ctx.pRgb->Header.ChanIndex, 10U, (UINT64) MemInfo.Ctx.Data, 16U);
                            AmbaImgPrintEx2(PRINT_FLAG_DBG, "frame id", MemInfo.Ctx.pRgb->Header.ChanIndex, 10U, MemInfo.Ctx.pRgb->FrameId, 16U);
                            AmbaImgPrintEx2(PRINT_FLAG_DBG, "raw cap seq", MemInfo.Ctx.pRgb->Header.ChanIndex, 16U, MemInfo.Ctx.pRgb->Header.RawPicSeqNum, 16U);
                            AmbaImgPrintEx2(PRINT_FLAG_DBG, "rgb iso config tag", MemInfo.Ctx.pRgb->Header.ChanIndex, 10U, AAA_HEADER_RGB_ISO_TAG, 16U);
                        }
#ifdef AMBA_IMG_MAIN_STATISTICS_VIN_HISTO
                        MemInfo.Ctx.pCvoid = HistoInfo[i];
                        if (MemInfo.Ctx.pCvoid != NULL) {
                            AmbaImgPrintEx2(PRINT_FLAG_DBG, "statistics histo", MemInfo.Ctx.pHisto->Header.ChannelIndex, 10U, (UINT32) MemInfo.Ctx.Data, 16U);
                        }
#endif
                        MemInfo.Ctx.pCvoid = CfaInfo[i];
                        if (MemInfo.Ctx.pCvoid != NULL) {
                            for (UINT32 m = 0U; m < MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AWB.AwbTileNumRow; m++) {
                                for (UINT32 n = 0U; n < MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AWB.AwbTileNumCol; n++) {
                                    AmbaImgPrintEx2(PRINT_FLAG_STAT, "awb ", (m*MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AWB.AwbTileNumCol)+n, 10U, MemInfo.Ctx.pCfa->Awb[(m*MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AWB.AwbTileNumCol)+n].SumG, 16U);
                                }
                            }
                            for (UINT32 m = 0U; m < MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AE.AeTileNumRow; m++) {
                                for (UINT32 n = 0U; n < MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AE.AeTileNumCol; n++) {
                                    AmbaImgPrintEx2(PRINT_FLAG_STAT, "ae ", (m*MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AE.AeTileNumCol)+n, 10U, MemInfo.Ctx.pCfa->Ae[(m*MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AE.AeTileNumCol)+n].LinY, 16U);
                                }
                            }
                        }
                    }
                }
                AmbaImgPrintEx(PRINT_FLAG_DBG, "statistics exceed", AmbaImgMainStatistics_ExceedCnt, 16U);
                /* count accumulate */
                AmbaImgMainStatistics_DebugCnt++;
                if (AmbaImgMainStatistics_DebugCnt == AmbaImgMain_DebugCount) {
                    /* count reset */
                    AmbaImgMainStatistics_DebugCnt = 0xFFFFFFFFU;
                }
            }
        }
    }
}
