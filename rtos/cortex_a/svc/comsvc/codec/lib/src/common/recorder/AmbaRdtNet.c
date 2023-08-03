/**
 *  @file AmbaRdtNet.c
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
 *  @details svc record destination - Net
 */

/* ssp */
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDef.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaPrint.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaAudio_AENC.h"
#include "AmbaDSP_VideoEnc.h"
/* framework*/
#include "AmbaRecFrwk.h"
#include "AmbaRbxInf.h"
#include "AmbaRbxMP4.h"
#include "AmbaMux.h"
#include "AmbaRscInf.h"
#include "AmbaRdtInf.h"
#include "AmbaSvcWrap.h"
#include "AmbaRdtNet.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaCodecCom.h"


#if defined(CONFIG_AMBA_RTSP)
#if defined(CONFIG_RTSP_LINUX)
#include "SvcRtspLink.h"
#else
#include "SvcRtspServer.h"
#include "SvcRtsp.h"
#endif
#endif

#define SVC_RDTNET_SRC_VIDEO      (0U)
#define SVC_RDTNET_SRC_AUDIO      (1U)

#define SVC_RDTNET_MSG_CMD        (0U)
#define SVC_RDTNET_MSG_DATA       (1U)

#define SVC_RDTNET_FLG_READY      (1U)

#define SVC_RDTNET_QUEUENUM       (1024U)
#define SVC_RDTNET_STACKSIZE      (0x3000U)

typedef struct {
    UINT32              MsgType;
    UINT32              CmdType;
    UINT32              SrcType;
    UINT32              NetType;
    void                *pSrcData;
    const AMBA_REC_DST_s *pNetRdt;
} SVC_RDTNET_MSG_s;

typedef struct {
    UINT32                 TaskExist;
    AMBA_KAL_TASK_t        Task;
    AMBA_KAL_MSG_QUEUE_t   MsgQueue;
    SVC_RDTNET_MSG_s       QueueBuf[SVC_RDTNET_QUEUENUM];
    AMBA_KAL_EVENT_FLAG_t  Flag;
} SVC_RDTNET_TASK_CTRL_s;

typedef struct {
    AMBA_RDT_STATIS_s       Statis;
} AMBA_RDT_NET_PRIV_s;

static void*  RdtNetTaskEntry(void* EntryArg);
static UINT32 DataNotify(const AMBA_REC_DST_s *pRdtCtrl, const AMBA_REC_FRWK_DESC_s *pDesc);

static SVC_RDTNET_TASK_CTRL_s TaskCtrl GNU_SECTION_NOZEROINIT;

#define MAX_AUDIO_DESC 256U
#define MAX_VIDEO_DESC 256U

static inline void RDTN_NG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
}

static inline void RDTN_OK(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
}

/**
* create the task
* @param [in]  pNetRdt destination info
* @return ErrorCode
*/
UINT32 AmbaRdtNet_Create(AMBA_REC_DST_s *pNetRdt)
{
    static UINT32         IsInit = 0U;
    static UINT8          RdtNetTaskStack[SVC_RDTNET_STACKSIZE] GNU_SECTION_NOZEROINIT;
    static char           QueueName[32];
    static char           FlagName[] = "AmbaRdtNetFlag";

    UINT32                Err, Rval = RECODER_OK;
    ULONG                 Num;
    const UINT8           *pPrivData;
    AMBA_RDT_NET_PRIV_s   *pPriv;

    pPrivData = &(pNetRdt->PrivData[0U]);
    AmbaMisra_TypeCast(&pPriv, &pPrivData);

    if (pNetRdt->UsrCfg.DstType != AMBA_RDT_TYPE_NET) {
        RDTN_NG("Wrong destination type (%u)", pNetRdt->UsrCfg.DstType, 0U);
        Rval = RECODER_ERROR_GENERAL_ERROR;
    }

    if (IsInit == 0U) {
        Err = AmbaWrap_memset(&TaskCtrl, 0, sizeof(SVC_RDTNET_TASK_CTRL_s));
        if (RECODER_OK != Err) {
            RDTN_NG("AmbaWrap_memset failed(%u)", Err, 0U);
        }
        IsInit = 1U;
    }

    if ((TaskCtrl.TaskExist == 0U) && (Rval == RECODER_OK)) {
        /* clean private data */
        Err = AmbaWrap_memset(pPriv, 0, sizeof(AMBA_RDT_NET_PRIV_s));
        if (RECODER_OK != Err) {
            RDTN_NG("AmbaWrap_memset failed(%u)", Err, 0U);
        }

        Num = pNetRdt->UsrCfg.StreamID;
        if (0U < AmbaUtility_UInt32ToStr(QueueName, 32U, (UINT32)Num, 16U)) {
            AmbaUtility_StringAppend(QueueName, 32, "RdtNetQueue");
        }

        /* create msg queue */
        Err = AmbaKAL_MsgQueueCreate(&(TaskCtrl.MsgQueue), QueueName, (UINT32)sizeof(SVC_RDTNET_MSG_s),
                                     TaskCtrl.QueueBuf, SVC_RDTNET_QUEUENUM * (UINT32)sizeof(SVC_RDTNET_MSG_s));
        if (KAL_ERR_NONE != Err) {
            RDTN_NG("Fail to create msg queue(%u)", Err, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }

        Err = AmbaKAL_MsgQueueFlush(&(TaskCtrl.MsgQueue));
        if (KAL_ERR_NONE != Err) {
            RDTN_NG("Fail to flush msg queue(%u)", Err, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }

        /* create flag */
        Err = AmbaKAL_EventFlagCreate(&(TaskCtrl.Flag), FlagName);
        if (KAL_ERR_NONE != Err) {
            RDTN_NG("Fail to create flag(%u)", Err, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }

        Err = AmbaKAL_EventFlagClear(&(TaskCtrl.Flag), 0xFFFFFFFFU);
        if (KAL_ERR_NONE != Err) {
            RDTN_NG("Fail to clear flag(%u)", Err, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }

        /* create task */
        if (Rval == RECODER_OK) {
            static char Name[] = "AmbaRdtNetTask";
            Err = AmbaKAL_TaskCreate(&(TaskCtrl.Task),
                                    Name,
                                    pNetRdt->UsrCfg.TaskPriority,
                                    RdtNetTaskEntry,
                                    NULL,
                                    RdtNetTaskStack,
                                    SVC_RDTNET_STACKSIZE,
                                    0);
            if (Err != KAL_ERR_NONE) {
                RDTN_NG("AmbaKAL_TaskCreate failed %u ", Err, 0U);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            } else {
                TaskCtrl.TaskExist = 1U;
            }
        }

        if (Rval == RECODER_OK) {
            Err = AmbaKAL_TaskSetSmpAffinity(&(TaskCtrl.Task), pNetRdt->UsrCfg.TaskCpuBits);
            if (Err != KAL_ERR_NONE) {
                RDTN_NG("AmbaKAL_TaskSetSmpAffinity failed %u ", Err, 0U);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            }
        }

        if (Rval == RECODER_OK) {
            Err = AmbaKAL_TaskResume(&(TaskCtrl.Task));
            if (Err != KAL_ERR_NONE) {
                RDTN_NG("AmbaKAL_TaskResume failed %u ", Err, 0U);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            }
        }
    }

    if (Rval == RECODER_OK) {
        pNetRdt->pfnStore = DataNotify;
    }

    return Rval;
}

/**
* destriy the task
* @param [in]  pNetRdt destination info
* @return ErrorCode
*/
UINT32 AmbaRdtNet_Destroy(const AMBA_REC_DST_s *pNetRdt)
{
    UINT32        Err, Rval = RECODER_OK;

    if (pNetRdt->UsrCfg.DstType != AMBA_RDT_TYPE_NET) {
        RDTN_NG("Wrong destination type (%u)", pNetRdt->UsrCfg.DstType, 0U);
        Rval = RECODER_ERROR_GENERAL_ERROR;
    }

    if ((TaskCtrl.TaskExist == 1U) && (Rval == RECODER_OK)) {
        /* delete task */
        Err = AmbaKAL_TaskTerminate(&(TaskCtrl.Task));
        if (Err != KAL_ERR_NONE) {
            RDTN_NG("AmbaKAL_TaskTerminate failed %u ", Err, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }

        if (Rval == RECODER_OK) {
            Err = AmbaKAL_TaskDelete(&(TaskCtrl.Task));
            if (Err != KAL_ERR_NONE) {
                RDTN_NG("AmbaKAL_TaskDelete failed %u ", Err, 0U);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            } else {
                TaskCtrl.TaskExist = 0U;
            }
        }

        /* delete msg queue */
        Err = AmbaKAL_MsgQueueDelete(&(TaskCtrl.MsgQueue));
        if (Err != KAL_ERR_NONE) {
            RDTN_NG("MsgQue delete failed(%u)", Err, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }

        /* delete flag */
        Err = AmbaKAL_EventFlagDelete(&(TaskCtrl.Flag));
        if (KAL_ERR_NONE != Err) {
            RDTN_NG("Fail to delete flag(%u)", Err, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}

/**
* destination net control
* @param [in]  pNetRdt destination info
* @param [in]  CtrlType control type
* @param [in]  pParam parameter
*/
void AmbaRdtNet_Control(const AMBA_REC_DST_s *pNetRdt, UINT32 CtrlType, void *pParam)
{
    UINT32                Err;

    AmbaMisra_TouchUnused(&pNetRdt);
    AmbaMisra_TouchUnused(pParam);
    AmbaMisra_TouchUnused(&Err);

    switch (CtrlType) {
    case AMBA_RDT_NET_STATIS_GET:
         break;
#if defined(CONFIG_AMBA_RTSP)
#if defined(CONFIG_RTSP_LINUX)
    case AMBA_RDT_NET_RTSP_START:
        Err = SvcRtspLink_Create();

        if (Err == RECODER_ERROR_GENERAL_ERROR) {
            RDTN_NG("SvcRtspLink_Create failed", 0U, 0U);
        } else {
            Err = SvcRtspLink_Command(pNetRdt->UsrCfg.StreamID, RTSP_LINK_CMD_START);
            if (Err != CODEC_OK) {
                RDTN_NG("SvcRtspLink_Command failed %u", Err, 0U);
            }
        }

        Err = AmbaKAL_EventFlagSet(&(TaskCtrl.Flag), SVC_RDTNET_FLG_READY);
        if (KAL_ERR_NONE != Err) {
            RDTN_NG("Fail to set flag(%u)", Err, 0U);
        }
        break;
    case AMBA_RDT_NET_RTSP_STOP:
        Err = SvcRtspLink_Command(pNetRdt->UsrCfg.StreamID, RTSP_LINK_CMD_STOP);
        if (Err != CODEC_OK) {
            RDTN_NG("SvcRtspLink_Command failed %u", Err, 0U);
        }
        break;
#else
    case AMBA_RDT_NET_RTSP_START:
        Err = SvcRtsp_Init();
        if (RECODER_OK != Err) {
            RDTN_NG("SvcRtsp_Init failed (%u)", Err, 0U);
        }
        Err = SvcRtspServer_Notify(SVC_RTSP_NOTIFY_STARTENC, 0, 0);
        if (RECODER_OK != Err) {
            RDTN_NG("SvcRtspServer_Notify failed (%u)", Err, 0U);
        }

        Err = AmbaKAL_EventFlagSet(&(TaskCtrl.Flag), SVC_RDTNET_FLG_READY);
        if (KAL_ERR_NONE != Err) {
            RDTN_NG("Fail to set flag(%u)", Err, 0U);
        }
        break;
    case AMBA_RDT_NET_RTSP_STOP:
        Err = AmbaKAL_EventFlagClear(&(TaskCtrl.Flag), SVC_RDTNET_FLG_READY);
        if (KAL_ERR_NONE != Err) {
            RDTN_NG("Fail to set flag(%u)", Err, 0U);
        }
        Err = SvcRtspServer_Notify(SVC_RTSP_NOTIFY_STOPENC, 0, 0);
        if (RECODER_OK != Err) {
            RDTN_NG("SvcRtspServer_Notify failed (%u)", Err, 0U);
        }
        Err = SvcRtsp_DeInit();
        if (RECODER_OK != Err) {
            RDTN_NG("SvcRtsp_DeInit failed (%u)", Err, 0U);
        }
        break;
#endif
#endif
    default:
        RDTN_NG("Unknown CtrlType(%u)", CtrlType, 0U);
        break;
    }
}

static void* RdtNetTaskEntry(void* EntryArg)
{
    UINT32              Err, ActualFlags, LoopU = 1U;
    SVC_RDTNET_MSG_s    Msg;

    AmbaMisra_TouchUnused(EntryArg);
    RDTN_OK("Create RdtNet task successfully", 0U, 0U);

    while (LoopU == 1U) {
        Err = AmbaKAL_EventFlagGet(&(TaskCtrl.Flag), SVC_RDTNET_FLG_READY, AMBA_KAL_FLAGS_ALL,
                                   AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if (KAL_ERR_NONE != Err) {
            RDTN_NG("Fail to get flag(%u)", Err, 0U);
            LoopU = 0U;
        }

        Err = AmbaWrap_memset(&Msg, 0, sizeof(SVC_RDTNET_MSG_s));
        if (RECODER_OK != Err) {
            RDTN_NG("AmbaWrap_memset failed (%u)", Err, 0U);
        }
        Err = AmbaKAL_MsgQueueReceive(&(TaskCtrl.MsgQueue), &Msg, AMBA_KAL_WAIT_FOREVER);
        if (KAL_ERR_NONE != Err) {
            RDTN_NG("Fail to receive msg queue(%u)", Err, 0U);
        } else {
            switch (Msg.NetType) {
            case AMBA_RDT_STYPE_RTSP:
                if (Msg.MsgType == SVC_RDTNET_MSG_CMD) {
                    /* nothing */
                } else if (Msg.MsgType == SVC_RDTNET_MSG_DATA) {
#if defined(CONFIG_AMBA_RTSP)
#if defined(CONFIG_RTSP_LINUX)
                    if (Msg.SrcType == SVC_RDTNET_SRC_VIDEO) {
                        Err = SvcRtspLink_Notify(Msg.pNetRdt->UsrCfg.StreamID, RTSP_LINK_NOTIFY_VID, Msg.pSrcData);
                        if (RECODER_OK != Err) {
                            RDTN_NG("SvcRtspLink_Notify video failed (%u)", Err, 0U);
                        }
                    } else if (Msg.SrcType == SVC_RDTNET_SRC_AUDIO) {
                        Err = SvcRtspLink_Notify(Msg.pNetRdt->UsrCfg.StreamID, RTSP_LINK_NOTIFY_AUD, Msg.pSrcData);
                        if (RECODER_OK != Err) {
                            RDTN_NG("SvcRtspLink_Notify audio failed (%u)", Err, 0U);
                        }
                    } else {
                        RDTN_NG("Unknown SrcType(%u)", Msg.SrcType, 0U);
                    }
#else
                    if (Msg.SrcType == SVC_RDTNET_SRC_VIDEO) {
                        Err = SvcRtsp_Notify(Msg.pNetRdt->UsrCfg.StreamID, RTSP_SERVICE_NOTIFY_VID, Msg.pSrcData);
                        if (RECODER_OK != Err) {
                            RDTN_NG("SvcRtsp_Notify video failed (%u)", Err, 0U);
                        }
                    } else if (Msg.SrcType == SVC_RDTNET_SRC_AUDIO) {
                        Err = SvcRtsp_Notify(Msg.pNetRdt->UsrCfg.StreamID, RTSP_SERVICE_NOTIFY_AUD, Msg.pSrcData);
                        if (RECODER_OK != Err) {
                            RDTN_NG("SvcRtsp_Notify audio failed (%u)", Err, 0U);
                        }
                    } else {
                        RDTN_NG("Unknown SrcType(%u)", Msg.SrcType, 0U);
                    }
#endif
#endif
                } else {
                    RDTN_NG("Unknown MsgType(%u)", Msg.MsgType, 0U);
                }
                break;
            default:
                RDTN_NG("Unknown NetType(%u)", Msg.NetType, 0U);
                break;
            }
        }
    }

    return NULL;
}

static UINT32 DataNotify(const AMBA_REC_DST_s *pRdtCtrl, const AMBA_REC_FRWK_DESC_s *pDesc)
{
    static AMBA_AENC_AUDIO_DESC_s AudioDesc[MAX_AUDIO_DESC] GNU_SECTION_NOZEROINIT;
    static AMBA_DSP_ENC_PIC_RDY_s VideoDesc[MAX_VIDEO_DESC] GNU_SECTION_NOZEROINIT;
    static UINT32 AudDesIndex = 0U;
    static UINT32 VidDesIndex = 0U;

    UINT32                  Err, Rval = RECODER_OK, ActualFlags, QueueSend = 0U;
    SVC_RDTNET_MSG_s        Msg;

    Err = AmbaWrap_memset(&Msg, 0, sizeof(SVC_RDTNET_MSG_s));
    if (RECODER_OK != Err) {
        RDTN_NG("AmbaWrap_memset failed (%u)", Err, 0U);
    }
    if ((0U < CheckBits(pRdtCtrl->UsrCfg.InSrcBits, pDesc->SrcBit))) {
        if (pDesc->SrcType == AMBA_RSC_TYPE_VIDEO) {
            Msg.MsgType  = SVC_RDTNET_MSG_DATA;
            Msg.SrcType  = SVC_RDTNET_SRC_VIDEO;
            Msg.NetType  = pRdtCtrl->UsrCfg.SubType;

            Err = AmbaWrap_memcpy(&VideoDesc[VidDesIndex], pDesc->pSrcData, sizeof(AMBA_DSP_ENC_PIC_RDY_s));
            if (RECODER_OK != Err) {
                RDTN_NG("AmbaWrap_memcpy failed (%u)", Err, 0U);
            }
            Msg.pSrcData = &VideoDesc[VidDesIndex];
            VidDesIndex = (VidDesIndex+1U)%MAX_VIDEO_DESC;

            Msg.pNetRdt  = pRdtCtrl;
            QueueSend    = 1U;

        } else if (pDesc->SrcType == AMBA_RSC_TYPE_AUDIO) {
            Msg.MsgType  = SVC_RDTNET_MSG_DATA;
            Msg.SrcType  = SVC_RDTNET_SRC_AUDIO;
            Msg.NetType  = pRdtCtrl->UsrCfg.SubType;

            Err = AmbaWrap_memcpy(&AudioDesc[AudDesIndex], pDesc->pSrcData, sizeof(AMBA_AENC_AUDIO_DESC_s));
            if (RECODER_OK != Err) {
                RDTN_NG("AmbaWrap_memcpy failed (%u)", Err, 0U);
            }
            Msg.pSrcData = &AudioDesc[AudDesIndex];
            AudDesIndex = (AudDesIndex+1U)%MAX_AUDIO_DESC;

            Msg.pNetRdt  = pRdtCtrl;
            QueueSend    = 1U;


        } else if (pDesc->SrcType == AMBA_RSC_TYPE_DATA) {
            /* Bypass */
        } else {
            RDTN_NG("Unknown source type(%u)", pDesc->SrcType, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }

        if (QueueSend == 1U) {
            do {
                Err = AmbaKAL_EventFlagGet(&(TaskCtrl.Flag), SVC_RDTNET_FLG_READY, AMBA_KAL_FLAGS_ALL,
                                   AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, 2000U);
                if(Err != RECODER_OK) {
                    AmbaPrint_PrintUInt5("[SvcRdtBet]<DataNotify>get SVC_RDTNET_FLG_READY failed!",
                        0U, 0U, 0U, 0U, 0U);
                }
            } while(RECODER_OK != Err);
            AmbaMisra_TouchUnused(&Err);
            if ((ActualFlags & SVC_RDTNET_FLG_READY) > 0U) {
                Err = AmbaKAL_MsgQueueSend(&(TaskCtrl.MsgQueue), &Msg, AMBA_KAL_NO_WAIT);

                if (KAL_ERR_NONE != Err) {
                    AmbaPrint_PrintUInt5("[SvcRdtBet]<DataNotify>Fail to send msg queue(%u)", Err,
                        0U, 0U, 0U, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }
        }
    }

    return Rval;
}
