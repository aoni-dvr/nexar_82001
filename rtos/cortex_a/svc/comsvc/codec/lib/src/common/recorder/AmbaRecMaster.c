/**
 *  @file AmbaRecMaster.c
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
 *  @details svc record master
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaDef.h"
#include "AmbaDSP.h"
#include "AmbaAudio_AENC.h"
#include "AmbaPrint.h"
#include "AmbaRecFrwk.h"
#include "AmbaRscInf.h"
#include "AmbaRscData.h"
#include "AmbaRbxInf.h"
#include "AmbaRdtInf.h"
#include "AmbaSvcWrap.h"
#include "AmbaRecMaster.h"
#include "AmbaCodecCom.h"
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "../../../../../../apps/icam/cardv/record/imu_record.h"
#endif

#undef DEBUG
//#define DEBUG
#ifdef DEBUG
#include "AmbaTypes.h"
#include "AmbaPrint.h"
#define PrintStr0(s1)        AmbaPrint_PrintStr5("%s",  (s1), NULL, NULL, NULL, NULL)
#define PrintStr1(fmt,s1)    AmbaPrint_PrintStr5((fmt), (s1), NULL, NULL, NULL, NULL)
#define PrintStr2(fmt,s1,s2) AmbaPrint_PrintStr5((fmt), (s1), (s2), NULL, NULL, NULL)
#define Puts(s1) PrintStr0(s1)
static char *ItoStrx(long p)
{
    static char *hexd = "0123456789ABCDEF";
    static char buf[9];
    buf[8]='\0';
    for (int i=7; i>=0; i--){
        buf[i] = hexd[p&0xf];
	p>>=4;
    }
    return buf;
}
#else
#define PrintStr0(s1)
#define PrintStr1(fmt,s1)
#define PrintStr2(fmt,s1,s2)
#define Puts(s1)
#endif

#define RMA_DBG_PRN     0
static inline void RMA_DBG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#if RMA_DBG_PRN
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
#else
    ULONG Temp = 0U;
    AmbaMisra_TypeCast(&Temp, pFormat);
    AmbaMisra_TouchUnused(&Arg1);
    AmbaMisra_TouchUnused(&Arg2);
#endif
}

static inline void RMA_NG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
}

static inline void RMA_OK(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
}

#define RMA_STACK_SIZE              (0xC000U)

/* record master event flags */
#define RMA_FLAG_RSC                (0x000000FFU)
#define RMA_FLAG_START              (0x00010000U)
#define RMA_FLAG_START_ACK          (0x00020000U)
#define RMA_FLAG_ALL_EOS            (0x00040000U)
#define RMA_FLAG_VIDEO_EOS          (0x00080000U)

#define RMA_FLAG_WAIT               (RMA_FLAG_RSC | RMA_FLAG_START)

/* schedule status */
#define RMA_STAT_SCHED_LOCK         (0x00000001U)
#define RMA_STAT_AUDIO_EXIST        (0x10000000U)
#define RMA_STAT_AVSYNC_S0          (0x20000000U)
#define RMA_STAT_AVSYNC_S1          (0x40000000U)

/* record source status */
#define RSC_STAT_LAST               (0x00000001U)
#define RSC_STAT_MULTI_FIRST        (0x00000002U)
#define RSC_STAT_MULTI_LAST         (0x00000004U)

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
/* default split time, unit (seconds) */
#define RMA_DEFAULT_SPLIT_TIME      (60U)
#else
/* default split time, unit (min) */
#define RMA_DEFAULT_SPLIT_TIME      (1U)
#endif


typedef struct {
    AMBA_REC_SRC_s           *pRscCtrl;
    AMBA_REC_FRWK_DESC_s     *pLiveData;
    AMBA_REC_FRWK_DESC_s     DataDesc;
    AMBA_DSP_ENC_PIC_RDY_s   VideoDesc;
    AMBA_AENC_AUDIO_DESC_s   AudioDesc;
    AMBA_RSC_TEXT_DESC_s     TextDesc;
    AMBA_DATG_s              DataG;

    UINT32                   RscStatus;
    UINT64                   CalcTimeStamp;
    UINT64                   FrameCount;
    UINT32                   SchFrameOffset;
    UINT32                   DropFrame;

    UINT32                   SrcType;

    DOUBLE                   FragThr;
    UINT32                   SchedCount;

    UINT32                   ProcBitsSize;
} AMBA_RSC_OP_s;

typedef struct {
    /* record source */
    AMBA_RSC_OP_s    RscOpArr[AMBA_RMA_SRC_NUM];
    UINT32           SrcNum;
    /* record box */
    AMBA_REC_BOX_s   *pRbxHead;
    AMBA_REC_BOX_s   *pRbxLast;
    UINT32           BoxNum;
    /* record destination */
    AMBA_REC_DST_s   *pRdtHead;
    AMBA_REC_DST_s   *pRdtLast;
    UINT32           DstNum;

    UINT32           State;
    UINT32           RmaStatus;
    UINT32           SrcEosBits;
    UINT32           AllSrcBits;
    UINT32           VEos;
    UINT32           DropSrcFrame;
    /* schedule */
    UINT32           SchedTimeScale;
    UINT32           SchedReadyBits;
    AMBA_RSC_OP_s    *pSchedLockSrc;
    UINT64           SchedTimeStamp;
    UINT32           FragReadyBits;
    /* split */
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    UINT32           SplitTime;    /* seconds */
#else
    UINT32           SplitTime;    /* min */
#endif

    UINT32           SplitTh;
    UINT32           SplitCnt;
    UINT32           SplitDisable;
    /* av sync */
    UINT64           SyncTime;

    UINT32           FragCnt;
} AMBA_RMA_PRIV_OP_s;

typedef struct {
    AMBA_KAL_TASK_t         TaskCtrl;
    UINT8                   TaskStack[RMA_STACK_SIZE];
    AMBA_KAL_EVENT_FLAG_t   TaskFlag;
    AMBA_KAL_MUTEX_t        TaskMutex;

    AMBA_RMA_PRIV_OP_s      OpParam;
} AMBA_RMA_PRIV_s;

static void RecDstDispatch(const AMBA_REC_DST_s *pRdtHead,
                           UINT32 Bit,
                           const AMBA_REC_FRWK_DESC_s *pLiveData,
                           UINT32 IsFromBox)
{
    UINT32               InBits;
    const AMBA_REC_DST_s  *pRdtCtrl = pRdtHead;

    while (pRdtCtrl != NULL) {
        if (0U < IsFromBox) {
            InBits = pRdtCtrl->UsrCfg.InBoxBits;
        } else {
            InBits = pRdtCtrl->UsrCfg.InSrcBits;
        }

        if (0U < (InBits & Bit)) {
            if (pRdtCtrl->pfnStore(pRdtCtrl, pLiveData) != RECODER_OK) {
                RMA_NG( "fail to rec dst store", 0U, 0U);
            }
        }
        pRdtCtrl = pRdtCtrl->pNext;
    }
}

static void RecBoxDispatch(const AMBA_RMA_PRIV_OP_s *pRmaOp, UINT32 SrcBit, AMBA_REC_FRWK_DESC_s *pLiveData)
{
    const AMBA_REC_BOX_s  *pRbxCtrl = pRmaOp->pRbxHead;

    while (pRbxCtrl != NULL) {
        if (0U < (pRbxCtrl->UsrCfg.InSrcBits & SrcBit)) {
            if (pRbxCtrl->pfnProc(pRbxCtrl, pLiveData) == RECODER_OK) {
                pLiveData->BoxBit     = pRbxCtrl->BoxBit;
                pLiveData->BoxType    = pRbxCtrl->UsrCfg.BoxType;
                pLiveData->BoxSubType = pRbxCtrl->UsrCfg.SubType;

                RecDstDispatch(pRmaOp->pRdtHead, pRbxCtrl->BoxBit, pLiveData, 1U);
            } else {
                RMA_NG( "fail to rec box proc", 0U, 0U);
            }
        }
        pRbxCtrl = pRbxCtrl->pNext;
    }
}

static void UpdateProcBitsSize(const AMBA_REC_MASTER_s *pRmaCtrl, const AMBA_REC_FRWK_DESC_s *pLiveData)
{
    UINT32                         BoxExist = 0U, i, RSize;
    const UINT8                    *pPrivData;
    AMBA_RMA_PRIV_s                *pPriv;
    const AMBA_REC_DST_s           *pRdtCtrl;
    AMBA_RMA_PRIV_OP_s             *pRmaOp;
    AMBA_RSC_OP_s                  *pRscOp;
    const AMBA_DSP_ENC_PIC_RDY_s   *pVDesc;
    const AMBA_AENC_AUDIO_DESC_s   *pADesc;
    const AMBA_RSC_TEXT_DESC_s     *pTDesc;
    const AMBA_DATG_s              *pDataG;

    pPrivData = pRmaCtrl->PrivData;
    AmbaMisra_TypeCast(&pPriv, &(pPrivData));
    pRmaOp    = &(pPriv->OpParam);
    pRdtCtrl  = pRmaOp->pRdtHead;

    while (pRdtCtrl != NULL) {
        if (0U < pRdtCtrl->UsrCfg.InBoxBits) {
            BoxExist = 1U;
            break;
        }
        pRdtCtrl = pRdtCtrl->pNext;
    }

    /* if record box is existed, we can only update the bitstream data size when the fragment is finished */
    /* because fmp4 muxer will only copy all the frame data from bitstream to fragment buffer when fragment flush */
    if ((BoxExist == 1U) && (0U < pLiveData->BoxBufNum)) {
        if (pRmaCtrl->UsrCfg.pfnProcBitsSizeUpdate != NULL) {
            for (i = 0U; i < pRmaOp->SrcNum; i++) {
                pRscOp  = &(pRmaOp->RscOpArr[i]);

                RSize = pRmaCtrl->UsrCfg.pfnProcBitsSizeUpdate(pRmaCtrl->UsrCfg.StreamId, pRscOp->SrcType, pRscOp->ProcBitsSize);
                if (RSize == 0U) {
                    /* nothing */
                }
                pRscOp->ProcBitsSize = 0U;
            }
        }
    }

    for (i = 0U; i < pRmaOp->SrcNum; i++) {
        pRscOp = &(pRmaOp->RscOpArr[i]);

        if (pRscOp->SrcType == pLiveData->SrcType) {
            if (pLiveData->SrcType == AMBA_RSC_TYPE_VIDEO) {
                AmbaMisra_TypeCast(&pVDesc, &(pLiveData->pSrcData));
                if (pVDesc->PicSize != AMBA_DSP_ENC_END_MARK) {
                    pRscOp->ProcBitsSize += pVDesc->PicSize;
                }
            } else if (pLiveData->SrcType == AMBA_RSC_TYPE_AUDIO) {
                AmbaMisra_TypeCast(&pADesc, &(pLiveData->pSrcData));
                if (pADesc->Eos == 0U) {
                    pRscOp->ProcBitsSize += pADesc->DataSize;
                }
            } else if (pLiveData->SrcType == AMBA_RSC_TYPE_DATA) {
                AmbaMisra_TypeCast(&pTDesc, &(pLiveData->pSrcData));
                AmbaMisra_TypeCast(&pDataG, &(pTDesc->pRawData));
                if (pDataG->Eos == 0U) {
                    pRscOp->ProcBitsSize++;
                }
            } else {
                /* should not get here */
            }

            if (BoxExist == 0U) {
                RSize = pRmaCtrl->UsrCfg.pfnProcBitsSizeUpdate(pRmaCtrl->UsrCfg.StreamId, pRscOp->SrcType, pRscOp->ProcBitsSize);
                if (RSize == 0U) {
                    /* nothing */
                }
                pRscOp->ProcBitsSize = 0U;
            }
        }
    }
}


static void RecSrcInspect(AMBA_RMA_PRIV_OP_s *pRmaOp, AMBA_RSC_OP_s *pRscOp)
{
    AMBA_REC_FRWK_DESC_s           *pDesc = pRscOp->pLiveData;
    const AMBA_DSP_ENC_PIC_RDY_s   *pVDesc;
    const AMBA_AENC_AUDIO_DESC_s   *pADesc;
    const AMBA_RSC_TEXT_DESC_s     *pTDesc;
    const AMBA_DATG_s              *pDataG;
    const AMBA_REC_SRC_s           *pRsc = pRscOp->pRscCtrl;

    if (pRsc->UsrCfg.SrcType == AMBA_RSC_TYPE_VIDEO) {
        AmbaMisra_TypeCast(&pVDesc, &(pDesc->pSrcData));

        if (pVDesc->PicSize != AMBA_DSP_ENC_END_MARK) {
            if (0U < CheckBits(pRmaOp->RmaStatus, RMA_STAT_AVSYNC_S0)) {
                pRmaOp->RmaStatus = ClearBits(pRmaOp->RmaStatus, RMA_STAT_AVSYNC_S0);

                /* get sync time */
                pRmaOp->SyncTime = pVDesc->CaptureTimeStamp;
                pRmaOp->RmaStatus = SetBits(pRmaOp->RmaStatus, RMA_STAT_AVSYNC_S1);
                RMA_DBG( "[AV_s1]: SyncTime = %u", (UINT32)pRmaOp->SyncTime, 0U);
            }

            if ((1U < pVDesc->NumTile) || (1U < pVDesc->NumSlice)) {
                if ((pVDesc->TileIdx == 0U) && (pVDesc->SliceIdx == 0U)) {
                    pRscOp->RscStatus = SetBits(pRscOp->RscStatus, RSC_STAT_MULTI_FIRST);
                    pRscOp->RscStatus = ClearBits(pRscOp->RscStatus, RSC_STAT_MULTI_LAST);
                } else if ((pVDesc->TileIdx == (UINT8)(pVDesc->NumTile - 1U)) &&
                           (pVDesc->SliceIdx == (UINT8)(pVDesc->NumSlice - 1U))) {
                    pRscOp->RscStatus = ClearBits(pRscOp->RscStatus, RSC_STAT_MULTI_FIRST);
                    pRscOp->RscStatus = SetBits(pRscOp->RscStatus, RSC_STAT_MULTI_LAST);
                } else {
                    pRscOp->RscStatus = ClearBits(pRscOp->RscStatus, RSC_STAT_MULTI_FIRST);
                    pRscOp->RscStatus = ClearBits(pRscOp->RscStatus, RSC_STAT_MULTI_LAST);
                }
            }

            /* count for split */
            if (0U < pRmaOp->SplitTh) {
                if ((pVDesc->TileIdx == 0U) && (pVDesc->SliceIdx == 0U)) {
                    if ((pVDesc->FrameType == PIC_FRAME_IDR) &&
                        (pRmaOp->SplitTh <= pRmaOp->SplitCnt) &&
                        (0U == pRmaOp->SplitDisable)) {
                        extern void UpdateVideoStartTime(int streamId);
                        pDesc->DescFlags = SetBits(pDesc->DescFlags, AMBA_REC_DDESC_FLAG_SPLIT);
                        pRmaOp->SplitCnt = 0U;
                        
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                        if (pVDesc->StreamId % 2 == 0) {
                            AmbaPrint_PrintUInt5("stream%u split---------------------------------------------------------", pVDesc->StreamId, 0U, 0U, 0U, 0U);
                            UpdateVideoStartTime(pVDesc->StreamId);
                            imu_record_control(IMU_RECORD_CACHE_SPLIT, pVDesc->StreamId, NULL, NULL);
                        }                       
#endif
                    }
                    pRmaOp->SplitCnt += 1U;
                }
            }

            /* calc time */
            pRscOp->CalcTimeStamp = pRsc->UsrCfg.NumUnitsInTick;
            pRscOp->CalcTimeStamp *= (pRscOp->FrameCount + pRscOp->SchFrameOffset);
            pRscOp->CalcTimeStamp *= pRmaOp->SchedTimeScale;
            pRscOp->CalcTimeStamp /= pRsc->UsrCfg.TimeScale;

            if ((pVDesc->TileIdx == 0U) && (pVDesc->SliceIdx == 0U)) {
                pRscOp->FrameCount++;
            }
        } else {
            pDesc->DescFlags = SetBits(pDesc->DescFlags, AMBA_REC_DDESC_FLAG_EOS);
            pRscOp->RscStatus = SetBits(pRscOp->RscStatus, RSC_STAT_LAST);
            RMA_OK( "[V%uEOS]: eos", pVDesc->StreamId, 0U);
        }
    } else if (pRsc->UsrCfg.SrcType == AMBA_RSC_TYPE_AUDIO) {
        AmbaMisra_TypeCast(&pADesc, &(pDesc->pSrcData));

        if (pADesc->Eos == 0U) {
            if (0U < CheckBits(pRmaOp->RmaStatus, RMA_STAT_AVSYNC_S1)) {
                RMA_DBG( "[AV_s1]: audio = %u", (UINT32)pADesc->AudioTicks, 0U);
            }

            /* calc time */
            pRscOp->CalcTimeStamp = pRsc->UsrCfg.NumUnitsInTick;
            pRscOp->CalcTimeStamp *= (pRscOp->FrameCount + pRscOp->SchFrameOffset);
            pRscOp->CalcTimeStamp *= pRmaOp->SchedTimeScale;
            pRscOp->CalcTimeStamp /= pRsc->UsrCfg.TimeScale;

            pRscOp->FrameCount++;
        } else {
            pDesc->DescFlags = SetBits(pDesc->DescFlags, AMBA_REC_DDESC_FLAG_EOS);
            pRscOp->RscStatus = SetBits(pRscOp->RscStatus, RSC_STAT_LAST);
            RMA_OK( "[AEOS]: eos", 0U, 0U);
        }
    } else if (pRsc->UsrCfg.SrcType == AMBA_RSC_TYPE_DATA) {
        AmbaMisra_TypeCast(&pTDesc, &(pDesc->pSrcData));
        AmbaMisra_TypeCast(&pDataG, &(pTDesc->pRawData));

        if (pDataG->Eos == 0U) {
            /* calc time */
            pRscOp->CalcTimeStamp = pRsc->UsrCfg.NumUnitsInTick;
            pRscOp->CalcTimeStamp *= (pRscOp->FrameCount + pRscOp->SchFrameOffset);
            pRscOp->CalcTimeStamp *= pRmaOp->SchedTimeScale;
            pRscOp->CalcTimeStamp /= pRsc->UsrCfg.TimeScale;

            pRscOp->FrameCount++;
        } else {
            pDesc->DescFlags = SetBits(pDesc->DescFlags, AMBA_REC_DDESC_FLAG_EOS);
            pRscOp->RscStatus = SetBits(pRscOp->RscStatus, RSC_STAT_LAST);
            RMA_OK( "[DEOS]: eos", 0U, 0U);
        }
    } else {
        RMA_NG( "invalid source", 0U, 0U);
    }

    /* Disable split if reach eos */
    if (0U != CheckBits(pRscOp->RscStatus, RSC_STAT_LAST)) {
        pRmaOp->SplitDisable = 1U;
    }
}

static void RecSrcEncrypt(const AMBA_REC_MASTER_s *pRmaCtrl, AMBA_REC_FRWK_DESC_s *pDataDesc)
{
    UINT32              Err;
    AMBA_RMAC_ENCRYPT_s Info;

    if (pRmaCtrl->UsrCfg.pfnEncrypt != NULL) {
        Err = AmbaWrap_memset(&Info, 0, sizeof(Info));
        if (Err != RECODER_OK) {
            RMA_NG("AmbaWrap_memset failed %u ", Err, 0U);
        }

        Info.StreamId    = pRmaCtrl->UsrCfg.StreamId;
        Info.RscType     = pDataDesc->SrcType;
        Info.SrcBufBase  = pDataDesc->SrcBufBase;
        Info.SrcBufSize  = pDataDesc->SrcBufSize;

        if (pDataDesc->SrcType == AMBA_RSC_TYPE_VIDEO) {
            AMBA_DSP_ENC_PIC_RDY_s *pVDesc;

            AmbaMisra_TypeCast(&pVDesc, &(pDataDesc->pSrcData));

            if (pVDesc->PicSize != AMBA_DSP_ENC_END_MARK) {
                Info.SrcDataAddr  = pVDesc->StartAddr;
                Info.SrcDataSize  = pVDesc->PicSize;
                Info.FrameType    = pVDesc->FrameType;
                Info.TileIdx      = pVDesc->TileIdx;
                Info.NumTile      = pVDesc->NumTile;
                Info.SliceIdx     = pVDesc->SliceIdx;
                Info.NumSlice     = pVDesc->NumSlice;

                pRmaCtrl->UsrCfg.pfnEncrypt(&Info);

                /* update the bitstream data info */
                pVDesc->StartAddr     = Info.DstDataAddr;
                pVDesc->PicSize       = Info.DstDataSize;
                pDataDesc->SrcBufBase = Info.DstBufBase;
                pDataDesc->SrcBufSize = Info.DstBufSize;
            }
        } else if (pDataDesc->SrcType == AMBA_RSC_TYPE_AUDIO) {
            AMBA_AENC_AUDIO_DESC_s *pADesc;

            AmbaMisra_TypeCast(&pADesc, &(pDataDesc->pSrcData));

            if (pADesc->Eos == 0U) {
                AmbaMisra_TypeCast(&(Info.SrcDataAddr), &(pADesc->pBufAddr));
                Info.SrcDataSize  = pADesc->DataSize;

                pRmaCtrl->UsrCfg.pfnEncrypt(&Info);

                /* update the bitstream data info */
                AmbaMisra_TypeCast(&(pADesc->pBufAddr), &(Info.DstDataAddr));
                pADesc->DataSize      = Info.DstDataSize;
                pDataDesc->SrcBufBase = Info.DstBufBase;
                pDataDesc->SrcBufSize = Info.DstBufSize;
            }
        } else if (pDataDesc->SrcType == AMBA_RSC_TYPE_DATA) {
            /* don't support data encryption now */
        } else {
            /* should not be here */
        }
    }
}

static void RecSrcDeque(const AMBA_REC_MASTER_s *pRmaCtrl, AMBA_RMA_PRIV_OP_s *pRmaOp)
{
    #define AV_DIFF_TIME_CMP        (0x7FFFFFFFU)

    UINT32                        i, Rval, SrcType, IsValid, IsSynced;
    UINT32                        AvDiff, SyncTime, AudioTime;
    AMBA_RSC_OP_s                  *pRscOp;
    const AMBA_REC_SRC_s           *pRsc;
    const AMBA_AENC_AUDIO_DESC_s  *pADesc;

    for (i = 0U; i < pRmaOp->SrcNum; i++) {
        pRscOp  = &(pRmaOp->RscOpArr[i]);
        pRsc    = pRscOp->pRscCtrl;
        SrcType = pRsc->UsrCfg.SrcType;

        if (pRscOp->pLiveData == NULL) {
            Rval = AmbaWrap_memset(&(pRscOp->DataDesc), 0, sizeof(AMBA_REC_FRWK_DESC_s));
            if (Rval != RECODER_OK) {
                RMA_NG("AmbaWrap_memset failed %u ", Rval, 0U);
            }

            IsValid = 1U;
            if (SrcType == AMBA_RSC_TYPE_VIDEO) {
                pRscOp->DataDesc.pSrcData = &(pRscOp->VideoDesc);
            } else if (SrcType == AMBA_RSC_TYPE_AUDIO) {
                pRscOp->DataDesc.pSrcData = &(pRscOp->AudioDesc);
            } else if (SrcType == AMBA_RSC_TYPE_DATA) {
                pRscOp->TextDesc.pRawData = &(pRscOp->DataG);
                pRscOp->DataDesc.pSrcData = &(pRscOp->TextDesc);
            } else {
                IsValid = 0U;
            }

            if (0U < IsValid) {
                if (pRsc->pfnDeque(pRsc, &(pRscOp->DataDesc)) == RECODER_OK) {
                    RecSrcEncrypt(pRmaCtrl, &(pRscOp->DataDesc));
                    pRscOp->pLiveData = &(pRscOp->DataDesc);
                    RecSrcInspect(pRmaOp, pRscOp);
                }
            }
        } else {
            /* av sync stage */
            if (SrcType == AMBA_RSC_TYPE_AUDIO) {
                if (pRmaOp->State == AMBA_RMA_STATE_AVSYNC) {
                    if (0U < CheckBits(pRmaOp->RmaStatus, RMA_STAT_AVSYNC_S1)) {
                        /* do av sync */
                        AmbaMisra_TypeCast(&pADesc, &(pRscOp->pLiveData->pSrcData));

                        /* check for av sync, need to take care overflow of time */
                        /* we keep dropping audio frames until AudioTime >= SyncTime */
                        IsSynced = 0U;
                        SyncTime = (UINT32)pRmaOp->SyncTime;
                        AudioTime = (UINT32)pADesc->AudioTicks;
                        if (SyncTime <= AudioTime) {
                            AvDiff = AudioTime - SyncTime;
                            if (AvDiff < AV_DIFF_TIME_CMP) {
                                IsSynced = 1U;
                            }
                        } else {
                            AvDiff = SyncTime - AudioTime;
                            /* if AudioTime overflow */
                            if (AvDiff > AV_DIFF_TIME_CMP) {
                                IsSynced = 1U;
                            }
                        }

                        if (0U < IsSynced) {
                            pRmaOp->RmaStatus = ClearBits(pRmaOp->RmaStatus, RMA_STAT_AVSYNC_S1);
                            /* [state]: avsync -> run */
                            pRmaOp->State = AMBA_RMA_STATE_RUN;
                            RMA_OK( "## AV synced (a_%u/v_%u)", (UINT32)AudioTime
                                                                          , (UINT32)SyncTime);
                        } else {
                            RMA_DBG( "## drop audio(a_%u/v_%u)", (UINT32)AudioTime
                                                                          , (UINT32)SyncTime);
                            pRscOp->pLiveData = NULL;
                        }
                    }
                }
            }
        }
    }
}

static void RecMasterSched(AMBA_RMA_PRIV_OP_s *pRmaOp, UINT32 *pProcBits)
{
    UINT32                     i, ProcBits, SrcBit, SchedBits, CurEosBits, VBits, IsIdr = 0U;
    UINT64                     Min = 0U, Cmp;
    AMBA_RSC_OP_s               *pRscOp;
    AMBA_RSC_OP_s               *pSrcPicked;
    const AMBA_REC_FRWK_DESC_s  *pDesc;

    /* check eos in current list */
    CurEosBits = 0U;
    for (i = 0U; i < pRmaOp->SrcNum; i++) {
        pRscOp = &(pRmaOp->RscOpArr[i]);
        pRscOp->DropFrame = 0U;

        SrcBit = pRscOp->pRscCtrl->SrcBit;
        if (0U < CheckBits(pRscOp->RscStatus, RSC_STAT_LAST)) {
            if (CheckBits(pRmaOp->SrcEosBits, SrcBit) == 0U) {
                CurEosBits = SetBits(CurEosBits, SrcBit);
            }
        }
    }

    ProcBits = 0U;
    if (0U < CheckBits(pRmaOp->RmaStatus, RMA_STAT_SCHED_LOCK)) {
        if (pRmaOp->pSchedLockSrc != NULL) {
            /* only process lock sourec in lock status */
            pRscOp = pRmaOp->pSchedLockSrc;
            if (pRscOp->pLiveData != NULL) {
                SrcBit = pRscOp->pRscCtrl->SrcBit;

                /* if it's last multi tile/slice, clear lock status */
                if (0U < CheckBits(pRscOp->RscStatus, RSC_STAT_MULTI_LAST)) {
                    pRmaOp->RmaStatus = ClearBits(pRmaOp->RmaStatus, RMA_STAT_SCHED_LOCK);
                    pRmaOp->pSchedLockSrc = NULL;
                }
                ProcBits = SrcBit;

                RMA_DBG("[l_%u] CalcTimeStamp = %u", SrcBit, (UINT32)pRscOp->CalcTimeStamp);
                RMA_DBG("[l_%u] quelen = %u", ProcBits, pRscOp->pRscCtrl->pfnQueSize(pRscOp->pRscCtrl));
            }
        } else {
            RMA_NG( "sch_lock but lock source is null", 0U, 0U);
        }
    } else {
        SchedBits = ClearBits(pRmaOp->AllSrcBits, pRmaOp->SrcEosBits);
        SchedBits = ClearBits(SchedBits, CurEosBits);

        for (i = 0U; i < pRmaOp->SrcNum; i++) {
            pRscOp = &(pRmaOp->RscOpArr[i]);

            SrcBit = pRscOp->pRscCtrl->SrcBit;
            if (0U < CheckBits(SchedBits, SrcBit)) {
                if (CheckBits(pRmaOp->SchedReadyBits, SrcBit) == 0U) {
                    pDesc = pRscOp->pLiveData;
                    if (pDesc != NULL) {
                        pRmaOp->SchedReadyBits = SetBits(pRmaOp->SchedReadyBits, SrcBit);
                    }
                }
            }
        }

        pSrcPicked = NULL;
        if (SchedBits == pRmaOp->SchedReadyBits) {
            RMA_DBG("@@0 SchedBits/ReadyBits(0x%X/0x%X)", SchedBits, pRmaOp->SchedReadyBits);

            for (i = 0U; i < pRmaOp->SrcNum; i++) {
                pRscOp = &(pRmaOp->RscOpArr[i]);

                SrcBit = pRscOp->pRscCtrl->SrcBit;
                if (0U < CheckBits(SchedBits, SrcBit)) {
#ifndef CONFIG_ENABLE_CAPTURE_TIME_MODE
                    DOUBLE                         FrameThr, SchedCount;
                    const AMBA_DSP_ENC_PIC_RDY_s   *pVDesc;

                    FrameThr   = ((DOUBLE)pRmaOp->FragCnt + 1.0) * pRscOp->FragThr;
                    SchedCount = (DOUBLE)pRscOp->SchedCount;

                    /* video length must <= other source length when flush gop */
                    if (pRscOp->SrcType == AMBA_RSC_TYPE_VIDEO) {
                        AmbaMisra_TypeCast(&pVDesc, &(pRscOp->pLiveData->pSrcData));
                        if ((SchedCount >= 1.0) && (pVDesc->TileIdx == 0U) &&
                            (pVDesc->SliceIdx == 0U) && (pVDesc->FrameType == PIC_FRAME_IDR)) {
                            UINT32 OtherSrcBits = ClearBits(pRmaOp->AllSrcBits, SrcBit);

                            if (pRmaOp->FragReadyBits == OtherSrcBits) {
                                pRmaOp->FragReadyBits = 0U;
                                pRmaOp->FragCnt++;
                                IsIdr                 = 1U;
                            } else {
                                continue;
                            }
                        }
                    } else {
                        if (SchedCount >= FrameThr) {
                            pRmaOp->FragReadyBits = SetBits(pRmaOp->FragReadyBits, SrcBit);
                        }
                    }
#endif

                    if (0U < ProcBits) {
                        RMA_DBG("@@2 SrcBit/Cmp(0x%X/%u)", SrcBit, (UINT32)pRscOp->CalcTimeStamp);

                        Cmp = GetMinValU64(Min, pRscOp->CalcTimeStamp);
                        /* if coming timestamp is equal min value, the rule is fin-fout */
                        if ((Cmp != Min) && (Cmp == pRscOp->CalcTimeStamp)) {
                            ProcBits = SrcBit;
                            pSrcPicked = pRscOp;
                            RMA_DBG("@@3 Min/CalcTime(%u/%u)", (UINT32)Min, (UINT32)Cmp);
                        }
                    } else {
                        Min = pRscOp->CalcTimeStamp;
                        ProcBits = SrcBit;
                        pSrcPicked = pRscOp;
                        RMA_DBG("@@1 SrcBit/Min(0x%X/%u)", SrcBit, (UINT32)Min);
                    }
                }
            }
        }

        if (pSrcPicked != NULL) {
            if ((Min < pRmaOp->SchedTimeStamp) && (IsIdr == 0U)) {
                RMA_NG( "SchedTimeStamp = %u", (UINT32)pRmaOp->SchedTimeStamp, 0U);
                RMA_NG( "[%u] CalcTimeStamp = %u", ProcBits, (UINT32)Min);
            }
            pRmaOp->SchedTimeStamp = Min;

            RMA_DBG("[f_%u] CalcTimeStamp = %u", ProcBits, (UINT32)Min);
            RMA_DBG("[f_%u] quelen = %u", ProcBits
                                        , pSrcPicked->pRscCtrl->pfnQueSize(pSrcPicked->pRscCtrl));

            if (0U < CheckBits(pSrcPicked->RscStatus, RSC_STAT_MULTI_FIRST)) {
                pRmaOp->RmaStatus = SetBits(pRmaOp->RmaStatus, RMA_STAT_SCHED_LOCK);
                pRmaOp->pSchedLockSrc = pSrcPicked;
            }
        }
    }

    /* after we receive video eos, we will make sure audio and text duration are long enough to meet muxer rule. Then
       we drop residual audio and text frames to avoid muxer fragment buffer overflow */
    if ((pRmaOp->DropSrcFrame == 1U) && (0U < ProcBits)) {
        for (i = 0U; i < pRmaOp->SrcNum; i++) {
            pRscOp = &(pRmaOp->RscOpArr[i]);
            SrcBit = pRscOp->pRscCtrl->SrcBit;
            if ((0U < CheckBits(ProcBits, SrcBit)) && (0U == CheckBits(pRscOp->RscStatus, RSC_STAT_LAST))) {
                pRscOp->DropFrame = 1U;
            }
        }
    }

    /* for muxer: video eos need to be pushed after other source eos */
    {
        VBits = 0U;
        for (i = 0U; i < pRmaOp->SrcNum; i++) {
            pRscOp = &(pRmaOp->RscOpArr[i]);
            if (pRscOp->SrcType == AMBA_RSC_TYPE_VIDEO) {
                VBits = (UINT32)1U << i;
                break;
            }
        }

        if ((0U < CheckBits(CurEosBits, VBits))) {
            UINT32 OtherSrcBits = ClearBits(pRmaOp->AllSrcBits, VBits);

            pRmaOp->VEos = 1U;
            if ((OtherSrcBits != CheckBits(pRmaOp->SrcEosBits, OtherSrcBits))) {
                CurEosBits = ClearBits(CurEosBits, VBits);
            }
        }
    }

    (*pProcBits) = SetBits(ProcBits, CurEosBits);
}

static void* RecMasterEntry(void* EntryArg)
{
    const UINT8             *pPrivData;
    UINT32                  i, Rval, ActualFlags, LoopU = 1U;
    UINT32                  ProcBits, SrcBit, Bit = 0x01U;
    const AMBA_REC_MASTER_s  *pRmaCtrl;
    AMBA_RMA_PRIV_s          *pPriv;
    AMBA_RMA_PRIV_OP_s       *pRmaOp;
    AMBA_RSC_OP_s            *pRscOp;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pRmaCtrl, &(EntryArg));
    pPrivData = pRmaCtrl->PrivData;
    AmbaMisra_TypeCast(&pPriv, &(pPrivData));
    pRmaOp = &(pPriv->OpParam);

    /* [state]: invalid -> idle */
    pRmaOp->State = AMBA_RMA_STATE_IDLE;

    while (0U < LoopU) {
        ActualFlags = 0U;
        Rval = AmbaKAL_EventFlagGet(&(pPriv->TaskFlag),
                                    RMA_FLAG_WAIT,
                                    AMBA_KAL_FLAGS_ANY,
                                    AMBA_KAL_FLAGS_CLEAR_NONE,
                                    &ActualFlags,
                                    AMBA_KAL_WAIT_FOREVER);
        if (Rval != RECODER_OK) {
            RMA_NG( "fail to get flag", 0U, 0U);
            LoopU = 0U;
        }

        Rval = AmbaKAL_EventFlagClear(&(pPriv->TaskFlag), RMA_FLAG_WAIT);
        if (Rval != RECODER_OK) {
            RMA_NG( "fail to clear flag", 0U, 0U);
        }

        if (0U == (ActualFlags & RMA_FLAG_WAIT)) {
            continue;
        }

        /* record start */
        if (0U < (ActualFlags & RMA_FLAG_START)) {
            if (pRmaOp->State == AMBA_RMA_STATE_IDLE) {
                /* [state]: idle -> run or avsync */
                if (0U < CheckBits(pRmaOp->RmaStatus, RMA_STAT_AUDIO_EXIST)) {
                    pRmaOp->State = AMBA_RMA_STATE_AVSYNC;
                    pRmaOp->RmaStatus = SetBits(pRmaOp->RmaStatus, RMA_STAT_AVSYNC_S0);
                    RMA_DBG( "[AV_s0]: sync ...", 0U, 0U);
                } else {
                    pRmaOp->State = AMBA_RMA_STATE_RUN;
                }

                /* reset something */
                pRmaOp->SrcEosBits     = 0U;
                pRmaOp->SplitCnt       = 0U;
                pRmaOp->SplitDisable   = 0U;
                pRmaOp->SchedTimeStamp = 0U;
                pRmaOp->FragReadyBits  = 0U;
                pRmaOp->VEos           = 0U;
                pRmaOp->FragCnt        = 0U;
                pRmaOp->DropSrcFrame   = 0U;
                for (i = 0U; i < pRmaOp->SrcNum; i++) {
                    pRmaOp->RscOpArr[i].RscStatus    = 0U;
                    pRmaOp->RscOpArr[i].FrameCount   = 0U;
                    pRmaOp->RscOpArr[i].SchedCount   = 0U;
                    pRmaOp->RscOpArr[i].ProcBitsSize = 0U;
                }

                /* ack to app */
                if (AmbaKAL_EventFlagSet(&(pPriv->TaskFlag), RMA_FLAG_START_ACK) == RECODER_OK) {
                    RMA_OK( "record master go ...", 0U, 0U);
                } else {
                    RMA_NG( "fail to set flag", 0U, 0U);
                }
            } else {
                RMA_NG( "rma isn't in idle state(%u)", pRmaOp->State, 0U);
            }
        }

        if (0U < (ActualFlags & RMA_FLAG_RSC)) {
            do {
                ProcBits = 0U;

                /* get data from record source */
                RecSrcDeque(pRmaCtrl, pRmaOp);

                /* run stage */
                if (pRmaOp->State == AMBA_RMA_STATE_RUN) {
                    /* schedule record sources */
                    RecMasterSched(pRmaOp, &ProcBits);

                    if (0U < ProcBits) {
                        /* dispatch data */
                        for (i = 0U; i < pRmaOp->SrcNum; i++) {
                            SrcBit = (Bit << i);
                            if (0U < CheckBits(ProcBits, SrcBit)) {
                                pRscOp = &(pRmaOp->RscOpArr[i]);

                                if (pRscOp->DropFrame == 0U) {
                                    /* src->box and box->dst */
                                    RecBoxDispatch(pRmaOp, SrcBit, pRscOp->pLiveData);
                                    /* src->dst */
                                    RecDstDispatch(pRmaOp->pRdtHead, SrcBit, pRscOp->pLiveData, 0U);

                                    /* update ProcBitsSize */
                                    UpdateProcBitsSize(pRmaCtrl, pRscOp->pLiveData);

                                    if (pRscOp->SrcType == AMBA_RSC_TYPE_VIDEO) {
                                        const AMBA_DSP_ENC_PIC_RDY_s   *pVDesc;
                                        AmbaMisra_TypeCast(&(pVDesc), &(pRscOp->pLiveData->pSrcData));
                                        if ((pVDesc->TileIdx == 0U) && (pVDesc->SliceIdx == 0U)) {
                                            pRscOp->SchedCount++;
                                        }
                                    } else {
                                        pRscOp->SchedCount++;
                                    }
                                }

                                /* clear ready bit */
                                pRscOp->pLiveData = NULL;
                                pRmaOp->SchedReadyBits = ClearBits(pRmaOp->SchedReadyBits, SrcBit);

                                if (0U < CheckBits(pRscOp->RscStatus, RSC_STAT_LAST)) {
                                    pRmaOp->SrcEosBits = SetBits(pRmaOp->SrcEosBits, SrcBit);
                                }
                            }
                        }

                        /* make sure audio and text track lenght >= video track length */
                        if ((0U == (ActualFlags & RMA_FLAG_VIDEO_EOS)) && (pRmaOp->VEos != 0U) && (pRmaOp->DropSrcFrame == 0U)) {
                            const AMBA_RSC_OP_s   *pVideo = NULL;
                            UINT32                SetFlag = 1U;
                            UINT64                FrameThr;

                            for (i = 0U; i < pRmaOp->SrcNum; i++) {
                                pRscOp = &(pRmaOp->RscOpArr[i]);
                                if (pRscOp->SrcType == AMBA_RSC_TYPE_VIDEO) {
                                    pVideo = pRscOp;
                                    break;
                                }
                            }

                            if (pVideo != NULL) {
                                for (i = 0U; i < pRmaOp->SrcNum; i++) {
                                    pRscOp = &(pRmaOp->RscOpArr[i]);
                                    if (pRscOp != pVideo) {
                                        FrameThr  = (UINT64)pVideo->SchedCount * (UINT64)pVideo->pRscCtrl->UsrCfg.NumUnitsInTick * (UINT64)pRscOp->pRscCtrl->UsrCfg.TimeScale;
                                        FrameThr  = GetRoundUpValU64(FrameThr, (UINT64)pVideo->pRscCtrl->UsrCfg.TimeScale * (UINT64)pRscOp->pRscCtrl->UsrCfg.NumUnitsInTick);
                                        if ((UINT64)pRscOp->SchedCount <= FrameThr) {
                                            SetFlag = 0U;
                                            break;
                                        }
                                    }
                                }

                                if (SetFlag == 1U) {
                                    if (AmbaKAL_EventFlagSet(&(pPriv->TaskFlag), RMA_FLAG_VIDEO_EOS) != RECODER_OK) {
                                        RMA_NG( "fail to set flag RMA_FLAG_VIDEO_EOS", 0U, 0U);
                                    }
                                    pRmaOp->DropSrcFrame = 1U;
                                }
                            }
                        }

                        if (pRmaOp->SrcEosBits == pRmaOp->AllSrcBits) {
                            if (AmbaKAL_EventFlagSet(&(pPriv->TaskFlag), RMA_FLAG_ALL_EOS) == RECODER_OK) {
                                RMA_OK( "@@ all sources are eos", 0U, 0U);

                                /* [state]: avsync -> run */
                                pRmaOp->State        = AMBA_RMA_STATE_IDLE;
                                pRmaOp->DropSrcFrame = 0U;
                            } else {
                                RMA_NG( "fail to set flag", 0U, 0U);
                            }
                        }
                    }
                }
            } while (0U < ProcBits);
        }
    }

    return NULL;
}

static void DataNotify(const AMBA_REC_MASTER_s *pRmaCtrl, UINT32 SrcBit)
{
    const UINT8     *pPrivData = pRmaCtrl->PrivData;
    UINT32          Rval;
    AMBA_RMA_PRIV_s  *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    Rval = AmbaKAL_EventFlagSet(&(pPriv->TaskFlag), SrcBit);
    if (Rval != RECODER_OK) {
        RMA_NG( "fail to set flag", 0U, 0U);
    }
}

/**
* record master creation
* @param [in]  pRmaCtrl control info
* @param [in]  Priority task priority
* @param [in]  CpuBits cpu bits
* @return ErrorCode
*/
UINT32 AmbaRecMaster_Create(AMBA_REC_MASTER_s *pRmaCtrl, UINT32 Priority, UINT32 CpuBits)
{
    static char    RmaFlag[] = "SvcRmaFlag";
    static char    RmaMutex[] = "SvcRmaFlag";
    static UINT32  RmaPrivSize = (UINT32)sizeof(AMBA_RMA_PRIV_s);

    const UINT8     *pPrivData = pRmaCtrl->PrivData;
    UINT32          Rval = RECODER_ERROR_GENERAL_ERROR, Reserved = CONFIG_AMBA_REC_RMA_PRIV_SIZE, Err;
    AMBA_RMA_PRIV_s  *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    if (RmaPrivSize <= Reserved) {
        /* configure internal fields */
        pRmaCtrl->pfnNotify = DataNotify;

        /* reset operation parameters */
        Err = AmbaWrap_memset(&(pPriv->OpParam), 0, sizeof(AMBA_RMA_PRIV_OP_s));
        if (Err != RECODER_OK) {
            RMA_NG("AmbaWrap_memset failed %u ", Err, 0U);
        }

        pPriv->OpParam.SplitTime = RMA_DEFAULT_SPLIT_TIME;
        PrintStr1("***** pPriv->OpParam.SplitTime = %s", ItoStrx(pPriv->OpParam.SplitTime ));

        /* create record master flag */
        Rval = AmbaKAL_EventFlagCreate(&(pPriv->TaskFlag), RmaFlag);
        if (Rval == RECODER_OK) {
            Rval = AmbaKAL_EventFlagClear(&(pPriv->TaskFlag), 0xFFFFFFFFU);

            /* create record master mutex */
            if (Rval == RECODER_OK) {
                Rval = AmbaKAL_MutexCreate(&(pPriv->TaskMutex), RmaMutex);
            }

            /* create record master task */
            if (Rval == RECODER_OK) {
                static char Name[] = "AmbaRecMaster";

                Err = AmbaKAL_TaskCreate(&(pPriv->TaskCtrl),
                                        Name,
                                        Priority,
                                        RecMasterEntry,
                                        pRmaCtrl,
                                        pPriv->TaskStack,
                                        RMA_STACK_SIZE,
                                        0);
                if (Err != KAL_ERR_NONE) {
                    RMA_NG("AmbaKAL_TaskCreate failed %u ", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == RECODER_OK) {
                Err = AmbaKAL_TaskSetSmpAffinity(&(pPriv->TaskCtrl), CpuBits);
                if (Err != KAL_ERR_NONE) {
                    RMA_NG("AmbaKAL_TaskSetSmpAffinity failed %u ", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == RECODER_OK) {
                Err = AmbaKAL_TaskResume(&(pPriv->TaskCtrl));
                if (Err != KAL_ERR_NONE) {
                    RMA_NG("AmbaKAL_TaskResume failed %u ", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }
        }
    }

    if (Rval != RECODER_OK) {
        RMA_NG( "fail to create record master, (%u/%u)", RmaPrivSize, Reserved);
    }

    return Rval;
}

/**
* record master destroying
* @param [in]  pRmaCtrl control info
* @return  ErrorCode
*/
UINT32 AmbaRecMaster_Destroy(const AMBA_REC_MASTER_s *pRmaCtrl)
{
    const UINT8     *pPrivData = pRmaCtrl->PrivData;
    UINT32          Rval = RECODER_OK, Err;
    AMBA_RMA_PRIV_s  *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));
    AmbaMisra_TouchUnused(&Rval);

    /* destroy task */
    if (Rval == RECODER_OK) {
        Err = AmbaKAL_TaskTerminate(&(pPriv->TaskCtrl));
        if (Err != KAL_ERR_NONE) {
            RMA_NG("AmbaKAL_TaskTerminate failed %u ", Err, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == RECODER_OK) {
        Err = AmbaKAL_TaskDelete(&(pPriv->TaskCtrl));
        if (Err != KAL_ERR_NONE) {
            RMA_NG("AmbaKAL_TaskDelete failed %u ", Err, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == RECODER_OK) {
        /* delete flag */
        Rval = AmbaKAL_EventFlagDelete(&(pPriv->TaskFlag));
        if (Rval == RECODER_OK) {
            /* destroy mutex */
            Rval = AmbaKAL_MutexDelete(&(pPriv->TaskMutex));
            if (Rval == RECODER_OK) {
                /* [state]: idle -> invalid */
                pPriv->OpParam.State = AMBA_RMA_STATE_INVALID;
            } else {
                RMA_NG( "fail to delete mutex", 0U, 0U);
            }
        } else {
            RMA_NG( "fail to delete flag", 0U, 0U);
        }
    } else {
        RMA_NG( "fail to create record master", 0U, 0U);
    }

    return Rval;
}

/**
* record master control
* @param [in]  pRmaCtrl control info
* @param [in]  CtrlType control type
* @param [in]  pParam pointer of control parameter
*/
void AmbaRecMaster_Control(const AMBA_REC_MASTER_s *pRmaCtrl, UINT32 CtrlType, void *pParam)
{
    const UINT8                  *pPrivData = pRmaCtrl->PrivData;
    UINT32                       i, Val, ActualFlags;
    const UINT32                 *pVal;
    AMBA_RMA_PRIV_s               *pPriv;
    AMBA_RMA_PRIV_OP_s            *pRmaOp;
    AMBA_RSC_OP_s                 *pRscOp;
    const AMBA_RMAC_SCHOFF_PARAM  *pSchParam;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));
    pRmaOp = &(pPriv->OpParam);

    switch (CtrlType) {
    case AMBA_RMAC_STATE_GET:
        Val = AmbaWrap_memcpy(pParam, &(pRmaOp->State), sizeof(UINT32));
        if (Val != RECODER_OK) {
            RMA_NG( "AmbaWrap_memcpy failed %u", Val, 0U);
        }
        break;
    case AMBA_RMAC_GO:
        Val = AmbaKAL_EventFlagClear(&(pPriv->TaskFlag), RMA_FLAG_VIDEO_EOS);
        if (Val != KAL_ERR_NONE) {
            RMA_NG( "AmbaKAL_EventFlagClear failed %u", Val, 0U);
        }
        if (AmbaKAL_EventFlagSet(&(pPriv->TaskFlag), RMA_FLAG_START) == RECODER_OK) {
            if (AmbaKAL_EventFlagGet(&(pPriv->TaskFlag),
                                     RMA_FLAG_START_ACK,
                                     AMBA_KAL_FLAGS_ANY,
                                     AMBA_KAL_FLAGS_CLEAR_AUTO,
                                     &ActualFlags,
                                     1000U) != RECODER_OK) {
                RMA_NG( "fail to wait ack", 0U, 0U);
            }
        } else {
            RMA_NG( "fail to set flag", 0U, 0U);
        }
        break;
    case AMBA_RMAC_HALT:
        AmbaMisra_TypeCast(&pVal, &pParam);

        Val = AmbaKAL_EventFlagGet(&(pPriv->TaskFlag),
                                   RMA_FLAG_ALL_EOS,
                                   AMBA_KAL_FLAGS_ANY,
                                   AMBA_KAL_FLAGS_CLEAR_AUTO,
                                   &ActualFlags,
                                   *pVal);
        if (Val != RECODER_OK) {
            RMA_NG( "fail to get flag %u", RMA_FLAG_ALL_EOS, 0U);
        }
        break;
    case AMBA_RMAC_WAIT_VEOS:
        AmbaMisra_TypeCast(&pVal, &pParam);

        Val = AmbaKAL_EventFlagGet(&(pPriv->TaskFlag),
                                   RMA_FLAG_VIDEO_EOS,
                                   AMBA_KAL_FLAGS_ANY,
                                   AMBA_KAL_FLAGS_CLEAR_NONE,
                                   &ActualFlags,
                                   *pVal);
        if (Val != RECODER_OK) {
            RMA_NG( "fail to get flag %u", RMA_FLAG_VIDEO_EOS, 0U);
        }
        break;
    case AMBA_RMAC_SPLIT_TIME:
        AmbaMisra_TypeCast(&pVal, &pParam);

        pRmaOp->SplitTime = (*pVal);
        break;
    case AMBA_RMAC_SCH_OFFSET:
        AmbaMisra_TypeCast(&pSchParam, &(pParam));

        for (i = 0U; i < pRmaOp->SrcNum; i++) {
            pRscOp = &(pRmaOp->RscOpArr[i]);
            if (0U < CheckBits(pSchParam->SrcBits, pRscOp->pRscCtrl->SrcBit)) {
                pRscOp->SchFrameOffset = pSchParam->SchFrameOffset;
            }
        }
        break;
    default:
        RMA_NG( "invalid control type", 0U, 0U);
        break;
    }
}

/**
* record source hook
* @param [in]  pRmaCtrl control info
* @param [in]  pRecSrc source info
*/
void AmbaRecMaster_SrcHook(const AMBA_REC_MASTER_s *pRmaCtrl, AMBA_REC_SRC_s *pRecSrc)
{
    const UINT8        *pPrivData = pRmaCtrl->PrivData;
    UINT32             Rval = RECODER_OK, Bit = 0x01U;
    AMBA_RMA_PRIV_s     *pPriv;
    AMBA_RMA_PRIV_OP_s  *pOp;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));
    pOp = &(pPriv->OpParam);

    if (AmbaKAL_MutexTake(&(pPriv->TaskMutex), 1000U) == RECODER_OK) {
        if (pOp->SrcNum < AMBA_RMA_SRC_NUM) {
            pRecSrc->SrcBit = (Bit << pOp->SrcNum);
            pOp->RscOpArr[pOp->SrcNum].pRscCtrl = pRecSrc;
            pOp->RscOpArr[pOp->SrcNum].FragThr  = pRecSrc->UsrCfg.FramePerFrag;
            pOp->RscOpArr[pOp->SrcNum].SrcType  = pRecSrc->UsrCfg.SrcType;

            pOp->AllSrcBits = SetBits(pOp->AllSrcBits, pRecSrc->SrcBit);
            if (pRecSrc->UsrCfg.SrcType == AMBA_RSC_TYPE_VIDEO) {
                /* calculate split time */
                if (pOp->SplitTh == 0U) {
                    pOp->SplitTh = pOp->SplitTime;
                    PrintStr2("****(1) SplitTime = %s Split Th = %s", ItoStrx(pOp->SplitTime), ItoStrx(pOp->SplitTh));
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
                    pOp->SplitTh *= 60U;
#endif
                    PrintStr2("****(2) SplitTime = %s Split Th = %s", ItoStrx(pOp->SplitTime), ItoStrx(pOp->SplitTh));

                    pOp->SplitTh *= GetRoundUpValU32(pRecSrc->UsrCfg.TimeScale,
                                                     pRecSrc->UsrCfg.NumUnitsInTick);
                    PrintStr2("****(3) SplitTime = %s Split Th = %s", ItoStrx(pOp->SplitTime), ItoStrx(pOp->SplitTh));                    
                }
            } else if (pRecSrc->UsrCfg.SrcType == AMBA_RSC_TYPE_AUDIO) {
                pOp->RmaStatus = SetBits(pOp->RmaStatus, RMA_STAT_AUDIO_EXIST);
            } else {
                /* do nothing */
            }

            /* set base schedule timescale */
            /* we use the largest source time scale as base schedule timescale*/
            if (pOp->SchedTimeScale < pRecSrc->UsrCfg.TimeScale) {
                pOp->SchedTimeScale = pRecSrc->UsrCfg.TimeScale;
            }

            pOp->SrcNum += 1U;
        } else {
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }

        if (AmbaKAL_MutexGive(&(pPriv->TaskMutex)) != RECODER_OK) {
            RMA_NG( "fail to give mutex", 0U, 0U);
        }
    } else {
        RMA_NG( "fail to take mutex", 0U, 0U);
    }

    if (Rval == RECODER_ERROR_GENERAL_ERROR) {
        RMA_NG( "exceed max. source", 0U, 0U);
    }
}

/**
* record box hook
* @param [in]  pRmaCtrl control info
* @param [in]  pRecBox record box info
*/
void AmbaRecMaster_BoxHook(const AMBA_REC_MASTER_s *pRmaCtrl, AMBA_REC_BOX_s *pRecBox)
{
    const UINT8     *pPrivData = pRmaCtrl->PrivData;
    UINT32          Bit = 0x01U;
    AMBA_RMA_PRIV_s  *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    if (AmbaKAL_MutexTake(&(pPriv->TaskMutex), 1000U) == RECODER_OK) {
        pRecBox->BoxBit = (Bit << pPriv->OpParam.BoxNum);
        pPriv->OpParam.BoxNum += 1U;


        if (pPriv->OpParam.pRbxHead == NULL) {
            pPriv->OpParam.pRbxHead = pRecBox;
            pPriv->OpParam.pRbxLast = pRecBox;
        } else {
            pPriv->OpParam.pRbxLast->pNext = pRecBox;
            pPriv->OpParam.pRbxLast = pRecBox;
        }
        pRecBox->pNext = NULL;

        if (AmbaKAL_MutexGive(&(pPriv->TaskMutex)) != RECODER_OK) {
            RMA_NG( "fail to give mutex", 0U, 0U);
        }
    } else {
        RMA_NG( "fail to take mutex", 0U, 0U);
    }
}

/**
* record destination hook
* @param [in]  pRmaCtrl control info
* @param [in]  pRecDst record destination info
*/
void AmbaRecMaster_DstHook(const AMBA_REC_MASTER_s *pRmaCtrl, AMBA_REC_DST_s *pRecDst)
{
    const UINT8     *pPrivData = pRmaCtrl->PrivData;
    AMBA_RMA_PRIV_s  *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    if (0U < (pRecDst->UsrCfg.InSrcBits & pRecDst->UsrCfg.InBoxBits)) {
        RMA_DBG( "## duplicate input bits, InSrcBits/InBoxBits"
                              , pRecDst->UsrCfg.InSrcBits
                              , pRecDst->UsrCfg.InBoxBits);
    }

    if (AmbaKAL_MutexTake(&(pPriv->TaskMutex), 1000U) == RECODER_OK) {
        if (pPriv->OpParam.pRdtHead == NULL) {
            pPriv->OpParam.pRdtHead = pRecDst;
            pPriv->OpParam.pRdtLast = pRecDst;
        } else {
            pPriv->OpParam.pRdtLast->pNext = pRecDst;
            pPriv->OpParam.pRdtLast = pRecDst;
        }
        pRecDst->pNext = NULL;

        pPriv->OpParam.DstNum += 1U;

        if (AmbaKAL_MutexGive(&(pPriv->TaskMutex)) != RECODER_OK) {
            RMA_NG( "fail to give mutex", 0U, 0U);
        }
    } else {
        RMA_NG( "fail to take mutex", 0U, 0U);
    }
}

