/**
 *  @file SvcUcBIST.c
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
 *  @details svc ucode BIST functions
 *
 */

#include "AmbaWrap.h"
#include "AmbaDSP.h"
#include "AmbaUtility.h"
#include "AmbaNVM_Partition.h"
#include "AmbaCache.h"
#include "AmbaShell.h"
#include "AmbaLZ4_IF.h"
#include "AmbaDSP_Event.h"

#include ".svc_autogen"
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcNvm.h"
#include "SvcWrap.h"
#include "SvcVinSrc.h"
#include "SvcTiming.h"
#include "SvcResCfg.h"
#include "SvcUcBIST.h"
#include "SvcPlat.h"

#define SVC_LOG_UCBT    "UCBT"

#define LZ4_DECOMPRESS_USED     (1U)

#define TF_C2Y_IDX_S            (0U)
#define TF_Y2Y_IDX_S            (SVCAG_TF_C2Y_TC_NUM)
#define TF_ENC_IDX_S            (TF_Y2Y_IDX_S + SVCAG_TF_Y2Y_TC_NUM)
#define TF_DEC_IDX_S            (TF_ENC_IDX_S + SVCAG_TF_ENC_TC_NUM)
#define TF_C2YI_IDX_S           (TF_DEC_IDX_S + SVCAG_TF_DEC_TC_NUM)
#define TF_Y2YI_IDX_S           (TF_C2YI_IDX_S + SVCAG_TF_C2Y_IN_NUM)

#define MAX_TF_NUM              (TF_Y2YI_IDX_S + SVCAG_TF_Y2Y_IN_NUM)
#define MAX_TCA_NUM             (TF_C2YI_IDX_S)

#define TF_BUG_ALIGN            (128U)

/*#define UCBIST_FLG_C2Y_DONE     (0x01U << (UINT32)DSP_TEST_STAGE_IDSP_0)*/
/*#define UCBIST_FLG_Y2Y_DONE     (0x01U << (UINT32)DSP_TEST_STAGE_IDSP_1)*/
/*#define UCBIST_FLG_ENC_DONE     (0x01U << (UINT32)DSP_TEST_STAGE_VDSP_0)*/
#define UCBIST_FLG_LOAD_DONE    (0x10000000U)
#define UCBIST_FLG_CFG_DONE     (0x20000000U)

typedef struct {
    ULONG   Base;
    UINT32  Size;
    UINT32  StageId;
    UINT32  IdxS;
} UCBIST_BUF_s;

typedef struct {
    char    FileName[32];
    UINT32  FileSize;
} UCBIST_TF_s;

typedef struct {
    ULONG                       TempBufBase;
    UINT32                      TempBufSize;
    AMBA_KAL_EVENT_FLAG_t       Flag;

    UCBIST_BUF_s                TfBuf[MAX_TF_NUM];
    AMBA_DSP_TEST_FRAME_CFG_s   TfCfg[DSP_TEST_STAGE_NUM];
    AMBA_DSP_TEST_FRAME_CTRL_s  TfCtrl[DSP_TEST_STAGE_NUM];
    UINT32                      Enable[DSP_TEST_STAGE_NUM];
} UCBIST_CTRL_s;

static UCBIST_CTRL_s  g_UcBistCtrl GNU_SECTION_NOZEROINIT;
static UCBIST_TF_s    g_UcBistTf[MAX_TF_NUM] = SVCAG_TF_LIST;

static UINT32 IsDuplexMode(void)
{
    UINT32  DuplexMode = 0U;
    UINT32  i, Src, FovNum, FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM];

    /* duplex mode ? */
    if (SvcResCfg_GetFovIdxs(FovIdxs, &FovNum) == SVC_OK) {
        for (i = 0; i < FovNum; i++) {
            Src = 255U;
            if (SvcResCfg_GetFovSrc(i, &Src) == SVC_OK) {
                if (Src == SVC_VIN_SRC_MEM_DEC) {
                    DuplexMode = 1U;
                }
            }
        }
    }

    return DuplexMode;
}

static UINT32 TestFrameRdyCB(const void *pEventData)
{
    const AMBA_DSP_EVENT_TESTFRAME_INFO_s  *pEvt;

    AmbaMisra_TypeCast(&pEvt, &pEventData);
    SvcLog_DBG(SVC_LOG_UCBT, "## stage(0x%X)", pEvt->Stage, 0U);
    SvcLog_DBG(SVC_LOG_UCBT, "   state/count(0x%X/0x%X)", pEvt->State, pEvt->Count);

    return OK;
}


/**
* evaluate needed buffer size
* @param [out] pNeedSize needed buffer size
* @return none
*/
void SvcUcBIST_EvalMemSize(UINT32 *pNeedSize)
{
    UINT32  i, BufSize, MaxDecomSize;

    *pNeedSize = 0U;
    MaxDecomSize = 0U;

    for (i = 0U; i < MAX_TF_NUM; i++) {
        BufSize = g_UcBistTf[i].FileSize;
        BufSize = GetAlignedValU32(BufSize, TF_BUG_ALIGN);
        *pNeedSize += BufSize;

        /* calculate decompress buffer */
        MaxDecomSize = GetMaxValU32(MaxDecomSize, BufSize);
    }
    *pNeedSize += MaxDecomSize;
}

/**
* initialization of ucode BIST
* @param [in] BufAddr address of buffer
* @param [in] BufSize size of buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcUcBIST_Init(ULONG BufAddr, UINT32 BufSize)
{
    static char UcBISTFlagName[32] = "UcBISTFlag";

    UINT32        i, Rval = SVC_OK, NeedSize, MaxDecomSize, TfBufSize;
    ULONG         Base = BufAddr;
    UCBIST_BUF_s  *pBufInfo;

    AmbaSvcWrap_MisraMemset(&g_UcBistCtrl, 0, sizeof(g_UcBistCtrl));

    SvcUcBIST_EvalMemSize(&NeedSize);
    /* assign buffer */
    if (NeedSize <= BufSize) {
        /* calculate decompress buffer */
        MaxDecomSize = 0U;
        for (i = 0U; i < MAX_TF_NUM; i++) {
            TfBufSize = g_UcBistTf[i].FileSize;
            TfBufSize = GetAlignedValU32(TfBufSize, TF_BUG_ALIGN);
            /* calculate decompress buffer */
            MaxDecomSize = GetMaxValU32(MaxDecomSize, TfBufSize);
        }
        g_UcBistCtrl.TempBufBase = Base;
        g_UcBistCtrl.TempBufSize = MaxDecomSize;

        Base += MaxDecomSize;
        for (i = 0U; i < MAX_TF_NUM; i++) {
            pBufInfo = &(g_UcBistCtrl.TfBuf[i]);

            TfBufSize = g_UcBistTf[i].FileSize;
            TfBufSize = GetAlignedValU32(TfBufSize, TF_BUG_ALIGN);

            pBufInfo->Base = Base;
            pBufInfo->Size = TfBufSize;

            if (i < TF_Y2Y_IDX_S) {
                /* C2Y */
                pBufInfo->StageId = DSP_TEST_STAGE_IDSP_0;
                pBufInfo->IdxS    = TF_C2Y_IDX_S;
            } else if (i < TF_ENC_IDX_S) {
                /* Y2Y */
                pBufInfo->StageId = DSP_TEST_STAGE_IDSP_1;
                pBufInfo->IdxS    = TF_Y2Y_IDX_S;
            } else if (i < TF_DEC_IDX_S) {
                /* ENC */
                pBufInfo->StageId = DSP_TEST_STAGE_VDSP_0;
                pBufInfo->IdxS    = TF_ENC_IDX_S;
            } else if (i < TF_C2YI_IDX_S) {
                /* DEC */
                pBufInfo->StageId = DSP_TEST_STAGE_VDSP_1;
                pBufInfo->IdxS    = TF_DEC_IDX_S;
            } else if (i < TF_Y2YI_IDX_S) {
                /* C2Y INP */
                pBufInfo->StageId = DSP_TEST_STAGE_IDSP_0;
                pBufInfo->IdxS = TF_C2YI_IDX_S;
            } else {
                /* Y2Y INP */
                pBufInfo->StageId = DSP_TEST_STAGE_IDSP_1;
                pBufInfo->IdxS = TF_Y2YI_IDX_S;
            }

            Base += pBufInfo->Size;
        }

        if (Base <= (BufAddr + BufSize)) {
            Rval = AmbaKAL_EventFlagCreate(&(g_UcBistCtrl.Flag), UcBISTFlagName);
        } else {
            Rval = SVC_NG;
        }
    } else {
        Rval = SVC_NG;
    }

    if (Rval != SVC_OK) {
        SvcLog_NG(SVC_LOG_UCBT, "buffer is too small in/needed(0x%X/0x%X)", BufSize, NeedSize);
    }

    return Rval;
}

/**
* load test data to buffer
* @return none
*/
UINT32 SvcUcBIST_LoadData(void)
{
    UINT8               *pSrc;
    UINT32              i = 0U, Rval = SVC_OK, FileSize, AlignSize;
    const UCBIST_BUF_s  *pBufInfo;
    const UCBIST_TF_s   *pTfInfo;

    SvcLog_DBG(SVC_LOG_UCBT, "## load test frame ...", 0U, 0U);
    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_BIST_LOAD_START, "Ucode load BIST START");
    #endif

    /* load test frame */
    for (i = 0U; i < MAX_TF_NUM; i++) {
        pBufInfo = &(g_UcBistCtrl.TfBuf[i]);
        pTfInfo = &(g_UcBistTf[i]);

        Rval = SvcNvm_GetRomFileSize( AMBA_USER_PARTITION_DSP_uCODE,
                                      pTfInfo->FileName,
                                      &FileSize);
        #if defined(LZ4_DECOMPRESS_USED)
        if ((Rval == SVC_OK) && (FileSize <= g_UcBistCtrl.TempBufSize)) {
            UINT8  *pDst;

            AmbaMisra_TypeCast(&pSrc, &(g_UcBistCtrl.TempBufBase));
            AmbaMisra_TypeCast(&pDst, &(pBufInfo->Base));

            AlignSize = GetAlignedValU32(FileSize, AMBA_CACHE_LINE_SIZE);
            if (SvcPlat_CacheInvalidate(g_UcBistCtrl.TempBufBase, AlignSize) != OK) {
                /* do nothong */
            }

            Rval = SvcNvm_ReadRomFileCrc(AMBA_USER_PARTITION_DSP_uCODE,
                                         pTfInfo->FileName,
                                         0U,
                                         FileSize,
                                         pSrc,
                                         10000U);

            AlignSize = AmbaLZ4_DeCompressFile(pSrc, pDst, FileSize, pBufInfo->Size);
            if (0U < AlignSize) {
                SvcLog_DBG(SVC_LOG_UCBT, "decompress base/size(0x%X/%u)", pBufInfo->Base, AlignSize);

                AlignSize = GetAlignedValU32(AlignSize, AMBA_CACHE_LINE_SIZE);
                if (SvcPlat_CacheClean(pBufInfo->Base, AlignSize) != OK) {
                    /* do nothong */
                }
            } else {
                SvcLog_NG(SVC_LOG_UCBT, "## fail to decompress file", 0U, 0U);
            }

        #else
        if ((Rval == SVC_OK) && (FileSize <= pBufInfo->Size)) {
            AmbaMisra_TypeCast(&pSrc, &(pBufInfo->Base));

            AlignSize = GetAlignedValU32(FileSize, AMBA_CACHE_LINE_SIZE);
            if (SvcPlat_CacheInvalidate(g_UcBistCtrl.TempBufBase, AlignSize) != OK) {
                /* do nothong */
            }

            Rval = SvcNvm_ReadRomFileCrc(AMBA_USER_PARTITION_DSP_uCODE,
                                         pTfInfo->FileName,
                                         0U,
                                         FileSize,
                                         pSrc,
                                         10000U);

            AlignSize = GetAlignedValU32(FileSize, AMBA_CACHE_LINE_SIZE);
            if (SvcPlat_CacheClean(pBufInfo->Base, AlignSize) != OK) {
                /* do nothong */
            }
        #endif
        } else {
            Rval = SVC_NG;
        }

        if (Rval == SVC_NG) {
            break;
        } else {
            #if defined(CONFIG_ICAM_TIMING_LOG)
            SvcTime_CalBistSize(FileSize);
            #endif
        }
    }

    if (Rval == SVC_OK) {
        Rval = AmbaKAL_EventFlagSet(&(g_UcBistCtrl.Flag), UCBIST_FLG_LOAD_DONE);
        if (Rval == SVC_OK) {
            SvcLog_OK(SVC_LOG_UCBT, "## load test frame is done", 0U, 0U);
        }
        #if defined(CONFIG_ICAM_TIMING_LOG)
        SvcTime(SVC_TIME_BIST_LOAD_DONE, "Ucode load BIST DONE");
        #endif
    }

    if (Rval != SVC_OK) {
        SvcLog_NG(SVC_LOG_UCBT, "## fail to load test frame_%u", i, 0U);
    }

    return Rval;
}

/**
* configure test interval
* @param [in] IntervalMs test interval
* @param [in] EncBufAddr address of encoder bitstream buffer
* @param [in] EncBufSize size of encoder bitstream buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcUcBIST_Config(UINT32 IntervalMs, ULONG EncBufAddr, UINT32 EncBufSize)
{
    void                        *pAddr;
    UINT8                       TFNum, InpNum;
    UINT32                      i, t, Rval = SVC_OK, UcInt, ActualFlag = 0U, NumStage;
    const UCBIST_BUF_s          *pBufInfo;
    AMBA_DSP_TEST_FRAME_CFG_s   *pCfg;
    AMBA_KAL_EVENT_FLAG_INFO_s  FlgInfo;

    AmbaSvcWrap_MisraMemset(&FlgInfo, 0, sizeof(FlgInfo));
    if (AmbaKAL_EventFlagQuery(&(g_UcBistCtrl.Flag), &FlgInfo) != OK) {
        SvcLog_NG(SVC_LOG_UCBT, "fail to get flag info", 0U, 0U);
    }

    if ((FlgInfo.CurrentFlags & UCBIST_FLG_CFG_DONE) == 0U) {
        Rval = SVC_NG;

        if (AmbaKAL_EventFlagGet(&(g_UcBistCtrl.Flag),
                                UCBIST_FLG_LOAD_DONE,
                                AMBA_KAL_FLAGS_ALL,
                                AMBA_KAL_FLAGS_CLEAR_NONE,
                                &ActualFlag,
                                10000U) != OK) {
            SvcLog_NG(SVC_LOG_UCBT, "fail to wait UCBIST_FLG_LOAD_DONE", 0U, 0U);
        }

        /* don't run dec test case if not duplex */
        NumStage = DSP_TEST_STAGE_NUM;
        if (IsDuplexMode() == 0U) {
            NumStage--;
        }

        UcInt = IntervalMs * 10U;
        for (i = 0U; i < NumStage; i++) {
            pCfg = &(g_UcBistCtrl.TfCfg[i]);

            TFNum = 0U;
            InpNum = 0U;
            for (t = 0U; t < MAX_TF_NUM; t++) {
                pBufInfo = &(g_UcBistCtrl.TfBuf[t]);

                if (pBufInfo->StageId == i) {
                    if (t < MAX_TCA_NUM) {
                        /* test case */
                        pCfg->TestFrameCfg[TFNum] = pBufInfo->Base;
                        TFNum++;
                    } else {
                        /* inp */
                        pCfg->TestFrameInp[InpNum] = pBufInfo->Base;
                        InpNum++;
                    }
                }
            }
            pCfg->NumTestFrame = TFNum;
            pCfg->Interval     = UcInt;
            if (i == DSP_TEST_STAGE_VDSP_0) {
                pCfg->BitsBufSize = EncBufSize;
                if (EncBufSize != 0U) {
                    AmbaMisra_TypeCast(&pAddr, &EncBufAddr);
                    pCfg->pBitsBufAddr = pAddr;
                } else {
                    /* skip encoder BIST if there is no encoder buffer */
                    continue;
                }
            }

            Rval = AmbaDSP_TestFrameConfig(i, pCfg);
            if (Rval != OK) {
                break;
            }
            SvcLog_DBG(SVC_LOG_UCBT, "ucode BIST config(%u/0x%X) is on", i, pCfg->NumTestFrame);
        }

        if (Rval == SVC_OK) {
            Rval = AmbaKAL_EventFlagSet(&(g_UcBistCtrl.Flag), UCBIST_FLG_CFG_DONE);
            if (Rval == SVC_OK) {
                Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_TESTFRAME_RDY, TestFrameRdyCB);
                if (Rval == OK) {
                    SvcLog_OK(SVC_LOG_UCBT, "ucode BIST config is done", 0U, 0U);
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_UCBT, "## fail to ucode BIST config", 0U, 0U);
        }
    }

    return Rval;
}

/**
* control test cases of each stage
* @param [in] StageId stage id
* @return 0-OK, 1-NG
*/
UINT32 SvcUcBIST_Ctrl(UINT32 StageId)
{
    UINT32                           i, Idx, Rval = SVC_OK, Flag = 0x01U;
    const UCBIST_BUF_s               *pBufInfo;
    const AMBA_DSP_TEST_FRAME_CFG_s  *pCfg;
    AMBA_DSP_TEST_FRAME_CTRL_s       *pCtrl;
    AMBA_KAL_EVENT_FLAG_INFO_s       FlgInfo;

    AmbaSvcWrap_MisraMemset(&FlgInfo, 0, sizeof(FlgInfo));
    if (AmbaKAL_EventFlagQuery(&(g_UcBistCtrl.Flag), &FlgInfo) != OK) {
        SvcLog_NG(SVC_LOG_UCBT, "fail to get flag info", 0U, 0U);
    }

    Flag <<= StageId;
    if ((FlgInfo.CurrentFlags & Flag) == 0U) {
        Rval = SVC_NG;
        if (StageId < DSP_TEST_STAGE_NUM) {
            pCfg  = &(g_UcBistCtrl.TfCfg[StageId]);
            pCtrl = &(g_UcBistCtrl.TfCtrl[StageId]);

            if ((StageId == DSP_TEST_STAGE_VDSP_0) && (pCfg->BitsBufSize == 0U)) {
                SvcLog_NG(SVC_LOG_UCBT, "need to configure BitsBuf for enc test frame", 0U, 0U);
            } else {
                if (g_UcBistCtrl.Enable[StageId] == 0U) {
                    pCtrl->StageId = (UINT8)StageId;
                    pCtrl->IsRepeat = 1U;

                    for (i = 0U; i < MAX_TCA_NUM; i++) {
                        pBufInfo = &(g_UcBistCtrl.TfBuf[i]);
                        if (pBufInfo->StageId == StageId) {
                            Idx = i - pBufInfo->IdxS;

                            pCtrl->EnableMask |= (UINT32)0x01U << Idx;
                            pCtrl->Order[Idx] = Idx;
                        }
                    }

                    Rval = AmbaDSP_TestFrameCtrl(1U, pCtrl);
                    if (Rval == SVC_OK) {
                        g_UcBistCtrl.Enable[StageId] = 1U;
                    }
                } else {
                    Rval = SVC_OK;
                }
            }
        }

        if (Rval == SVC_OK) {
            Rval = AmbaKAL_EventFlagSet(&(g_UcBistCtrl.Flag), Flag);
            if (Rval == SVC_OK) {
                SvcLog_OK(SVC_LOG_UCBT, "ucode BIST control is done", 0U, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_UCBT, "## fail to control ucode BIST", 0U, 0U);
        }
    }

    return Rval;
}

/**
* dump setting of ucode bist
* @param [in] PrintFunc print function of shell
* @return none
*/
void SvcUcBIST_CtrlReset(void)
{
    UINT32  i;

    for (i = 0U; i < DSP_TEST_STAGE_NUM; i++) {
        g_UcBistCtrl.Enable[i] = 0U;
    }
}

/**
* dump setting of ucode bist
* @param [in] PrintFunc print function of shell
* @return none
*/
void SvcUcBIST_Dump(AMBA_SHELL_PRINT_f PrintFunc)
{
    char                              StrBuf[256] = {'\0'};
    UINT32                            i, t, Rval, BufLen = 256U, Interval;
    ULONG                             Addr;
    const AMBA_DSP_TEST_FRAME_CFG_s   *pCfg;
    const AMBA_DSP_TEST_FRAME_CTRL_s  *pCtrl;

    PrintFunc("#### ucode BIST information ####\n");

    for (i = 0U; i < DSP_TEST_STAGE_NUM; i++) {
        pCfg = &(g_UcBistCtrl.TfCfg[i]);
        pCtrl = &(g_UcBistCtrl.TfCtrl[i]);

        AmbaMisra_TypeCast(&Addr, &(pCfg->pBitsBufAddr));

        Interval = (pCfg->Interval / 10U);

        SVC_WRAP_SNPRINT            "[stage_%u]\n Interval((%ums)\n BitsBuf Addr/Size(0x%X/0x%X)\n"
            SVC_SNPRN_ARG_S         StrBuf
            SVC_SNPRN_ARG_UINT32    i                   SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    Interval            SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    Addr                SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    pCfg->BitsBufSize   SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);

        SVC_WRAP_SNPRINT            " EnableMask(0x%X)\n NumTestFrame(%d)\n"
            SVC_SNPRN_ARG_S         StrBuf
            SVC_SNPRN_ARG_UINT32    pCtrl->EnableMask   SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT8     pCfg->NumTestFrame  SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);

        for (t = 0U; t < DSP_MAX_TEST_FRAME_NUM; t++) {
            if (pCfg->TestFrameCfg[t] == 0U) {
                break;
            }

            SVC_WRAP_SNPRINT            "   TestFrameCfg(0x%X)\n"
                SVC_SNPRN_ARG_S         StrBuf
                SVC_SNPRN_ARG_UINT32    pCfg->TestFrameCfg[t]   SVC_SNPRN_ARG_POST
                SVC_SNPRN_ARG_BSIZE     BufLen
                SVC_SNPRN_ARG_RLEN      &Rval
                SVC_SNPRN_ARG_E
            PrintFunc(StrBuf);
        }
    }
}
