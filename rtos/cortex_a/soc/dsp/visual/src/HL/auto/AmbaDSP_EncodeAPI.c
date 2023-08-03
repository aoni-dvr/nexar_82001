/**
 *  @file AmbaDSP_EncodeAPI.c
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
 *  @details Implementation of SSP Encode internal API
 *
 */

#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_Log.h"
#include "AmbaDSP_ArmComm.h"
#include "AmbaDSP_EncodeAPI.h"
#include "AmbaDSP_EventCtrl.h"
#include "AmbaDSP_MsgDispatcher.h"

//const UINT16 HL_MaxVprocPinOutputWidthMap[DSP_VPROC_PIN_NUM] = {
//    [DSP_VPROC_PIN_PREVC] = PREVC_MAX_WIDTH,
//    [DSP_VPROC_PIN_PREVA] = PREVA_MAX_WIDTH,
//    [DSP_VPROC_PIN_PREVB] = PREVB_MAX_WIDTH,
//    [DSP_VPROC_PIN_MAIN]  = MAIN_MAX_WIDTH,
//};

//const UINT16 HL_MaxVprocPinOutput2XWidthMap[DSP_VPROC_PIN_NUM] = {
//    [DSP_VPROC_PIN_PREVC] = PREVC_2X_MAX_WIDTH,
//    [DSP_VPROC_PIN_PREVA] = PREVA_2X_MAX_WIDTH,
//    [DSP_VPROC_PIN_PREVB] = PREVB_2X_MAX_WIDTH,
//    [DSP_VPROC_PIN_MAIN]  = MAIN_MAX_WIDTH,
//};

const UINT16 HL_MaxVprocPinOutputRatioMap[DSP_VPROC_PIN_NUM] = {
    [DSP_VPROC_PIN_PREVC] = PREV_MAX_RATIO_16X,
    [DSP_VPROC_PIN_PREVA] = PREV_MAX_RATIO_16X,
    [DSP_VPROC_PIN_PREVB] = PREV_MAX_RATIO_8X,
    [DSP_VPROC_PIN_MAIN]  = PREV_MAX_RATIO_8X,
};

/* PrevC can't have upscale */
//const UINT8 HL_VprocPinUpscaleCapabilityMap[DSP_VPROC_PIN_NUM] = {
//    [DSP_VPROC_PIN_PREVC] = 0,
//    [DSP_VPROC_PIN_PREVA] = 1,
//    [DSP_VPROC_PIN_PREVB] = 1,
//    [DSP_VPROC_PIN_MAIN]  = 1,
//};

const UINT16 HL_CtxVprocPinDspPrevMap[DSP_VPROC_PIN_MAIN] = {
    [DSP_VPROC_PIN_PREVC] = DSP_VPROC_PREV_C,
    [DSP_VPROC_PIN_PREVA] = DSP_VPROC_PREV_A,
    [DSP_VPROC_PIN_PREVB] = DSP_VPROC_PREV_B,
};

const UINT16 HL_DspPrevCtxVprocPinMap[DSP_VPROC_PREV_D] = {
    [DSP_VPROC_PREV_A] = DSP_VPROC_PIN_PREVA,
    [DSP_VPROC_PREV_B] = DSP_VPROC_PIN_PREVB,
    [DSP_VPROC_PREV_C] = DSP_VPROC_PIN_PREVC,
};

const UINT16 HL_CtxVprocPinDspPinMap[DSP_VPROC_PIN_NUM] = {
    [DSP_VPROC_PIN_PREVC] = DSP_VPROC_OUT_STRM_PREV_C,
    [DSP_VPROC_PIN_PREVA] = DSP_VPROC_OUT_STRM_PREV_A,
    [DSP_VPROC_PIN_PREVB] = DSP_VPROC_OUT_STRM_PREV_B,
    [DSP_VPROC_PIN_MAIN]  = DSP_VPROC_OUT_STRM_MAIN,
};

const UINT16 HL_CtxDspPinVprocPinMap[NUM_DSP_VPROC_OUT_STRM] = {
    [DSP_VPROC_OUT_STRM_MAIN]   = DSP_VPROC_PIN_MAIN,
    [DSP_VPROC_OUT_STRM_PREV_A] = DSP_VPROC_PIN_PREVA,
    [DSP_VPROC_OUT_STRM_PREV_B] = DSP_VPROC_PIN_PREVB,
    [DSP_VPROC_OUT_STRM_PREV_C] = DSP_VPROC_PIN_PREVC,
};

//const UINT16 HL_VprocMemTypeVprocPinMap[NUM_VPROC_EXT_MEM_TYPE] = {
//    [VPROC_EXT_MEM_TYPE_MAIN]       = DSP_VPROC_PIN_MAIN,
//    [VPROC_EXT_MEM_TYPE_PREV_A]     = DSP_VPROC_PIN_PREVA,
//    [VPROC_EXT_MEM_TYPE_PREV_B]     = DSP_VPROC_PIN_PREVB,
//    [VPROC_EXT_MEM_TYPE_PREV_C]     = DSP_VPROC_PIN_PREVC,
//    [VPROC_EXT_MEM_TYPE_LN_DEC]     = DSP_VPROC_PIN_MAIN,   //TBD
//    [VPROC_EXT_MEM_TYPE_HIER_0]     = DSP_VPROC_PIN_MAIN,   //TBD
//    [VPROC_EXT_MEM_TYPE_COMP_RAW]   = DSP_VPROC_PIN_MAIN,   //TBD
//    [VPROC_EXT_MEM_TYPE_MAIN_ME]    = DSP_VPROC_PIN_MAIN,   //TBD
//    [VPROC_EXT_MEM_TYPE_PIP_ME]     = DSP_VPROC_PIN_MAIN,   //TBD
//};

const UINT16 HL_VprocPinVprocMemTypePoolMap[DSP_VPROC_PIN_NUM] = {
    [DSP_VPROC_PIN_PREVC]   = VPROC_EXT_MEM_TYPE_PREV_C,
    [DSP_VPROC_PIN_PREVA]   = VPROC_EXT_MEM_TYPE_PREV_A,
    [DSP_VPROC_PIN_PREVB]   = VPROC_EXT_MEM_TYPE_PREV_B,
    [DSP_VPROC_PIN_MAIN]    = VPROC_EXT_MEM_TYPE_MAIN,      //Mctf/Mctf/Main both share same memory pool
};

const UINT16 HL_VprocPinVprocMemMeTypeMap[DSP_VPROC_PIN_NUM] = {
    [DSP_VPROC_PIN_PREVC]   = VPROC_EXT_MEM_TYPE_PIP_ME,
    [DSP_VPROC_PIN_PREVA]   = VPROC_EXT_MEM_TYPE_PIP_ME,
    [DSP_VPROC_PIN_PREVB]   = VPROC_EXT_MEM_TYPE_PIP_ME,
    [DSP_VPROC_PIN_MAIN]    = VPROC_EXT_MEM_TYPE_MAIN_ME,
};

const UINT16 HL_VprocPinVinAttachBitMap[DSP_VPROC_PIN_NUM] = {
    [DSP_VPROC_PIN_PREVC]   = DSP_BIT_POS_EXT_PREV_C,
    [DSP_VPROC_PIN_PREVA]   = DSP_BIT_POS_EXT_PREV_A,
    [DSP_VPROC_PIN_PREVB]   = DSP_BIT_POS_EXT_PREV_B,
    [DSP_VPROC_PIN_MAIN]    = DSP_BIT_POS_EXT_MCTS,      //Mctf/Mctf/Main both share same memory pool
};

const UINT16 HL_VprocPinVinAttachMeBitMap[DSP_VPROC_PIN_NUM] = {
    [DSP_VPROC_PIN_PREVC]   = DSP_BIT_POS_EXT_PREV_C_ME,
    [DSP_VPROC_PIN_PREVA]   = DSP_BIT_POS_EXT_PREV_A_ME,
    [DSP_VPROC_PIN_PREVB]   = DSP_BIT_POS_EXT_PREV_B_ME,
    [DSP_VPROC_PIN_MAIN]    = DSP_BIT_POS_EXT_MAIN_ME,
};

//const RAW_BIT_RATE_t RawBitRateTable[RAW_COMPRESS_NUM] = {
//    [RAW_COMPRESS_OFF]   = {.Den =  0U, .Num =  0U, .Mantissa =  0U, .Block = 0U},
//    [RAW_COMPRESS_6P75]  = {.Den = 27U, .Num = 32U, .Mantissa =  6U, .Block = 4U},
//    [RAW_COMPRESS_7p5]   = {.Den = 30U, .Num = 32U, .Mantissa =  6U, .Block = 2U},
//    [RAW_COMPRESS_7p75]  = {.Den = 31U, .Num = 32U, .Mantissa =  7U, .Block = 4U},
//    [RAW_COMPRESS_8p5]   = {.Den = 34U, .Num = 32U, .Mantissa =  7U, .Block = 2U},
//    [RAW_COMPRESS_8p75]  = {.Den = 35U, .Num = 32U, .Mantissa =  8U, .Block = 4U},
//    [RAW_COMPRESS_9p5]   = {.Den = 38U, .Num = 32U, .Mantissa =  8U, .Block = 2U},
//    [RAW_COMPRESS_9p75]  = {.Den = 39U, .Num = 32U, .Mantissa =  9U, .Block = 4U},
//    [RAW_COMPRESS_10p5]  = {.Den = 42U, .Num = 32U, .Mantissa =  9U, .Block = 2U},
//    [RAW_COMPRESS_10p75] = {.Den = 43U, .Num = 32U, .Mantissa = 10U, .Block = 4U},
//    [RAW_COMPRESS_11P5]  = {.Den = 46U, .Num = 32U, .Mantissa = 10U, .Block = 2U},
//    [RAW_COMPRESS_11P75] = {.Den = 47U, .Num = 32U, .Mantissa = 11U, .Block = 4U},
//    [RAW_COMPRESS_12P5]  = {.Den = 50U, .Num = 32U, .Mantissa = 11U, .Block = 2U},
//    [RAW_COMPRESS_4P75]  = {.Den = 19U, .Num = 32U, .Mantissa =  4U, .Block = 4U},
//    [RAW_COMPRESS_5P5]   = {.Den = 22U, .Num = 32U, .Mantissa =  4U, .Block = 2U},
//    [RAW_COMPRESS_5P75]  = {.Den = 23U, .Num = 32U, .Mantissa =  5U, .Block = 4U},
//    [RAW_COMPRESS_6P5]   = {.Den = 26U, .Num = 32U, .Mantissa =  5U, .Block = 2U}
//};

const RAW_CMPT_RATE_t RawCmptRateTable[RAW_COMPACT_NUM] = {
    [RAW_COMPACT_OFF]   = {.Mantissa =  0U, .Block = 0U},
    [RAW_COMPACT_8BIT]  = {.Mantissa =  5U, .Block = 0xFFU}, // No 8bit in CV2, but will be 5 when CV2FS
    [RAW_COMPACT_10BIT] = {.Mantissa =  4U, .Block = 0xFFU},
    [RAW_COMPACT_12BIT] = {.Mantissa =  2U, .Block = 0xFFU},
    [RAW_COMPACT_14BIT] = {.Mantissa =  3U, .Block = 0xFFU},
};

//const UINT8 RawCmptBitLengthTable[RAW_COMPACT_NUM] = {
//    [RAW_COMPACT_OFF]   = 0U,
//    [RAW_COMPACT_8BIT]  = 8U, // No 8bit in CV2, but will be 5 when CV2FS
//    [RAW_COMPACT_10BIT] = 10U,
//    [RAW_COMPACT_12BIT] = 12U,
//    [RAW_COMPACT_14BIT] = 14U,
//};

const UINT16 HL_HierUserIdx2VprocIdx[AMBA_DSP_MAX_HIER_NUM] = {
    [0U] = 0U,
    [1U] = 1U,
    [2U] = 3U,
    [3U] = 5U,
    [4U] = 2U,
    [5U] = 4U,
};

const UINT8 HL_RotateFlipMap[AMBA_DSP_ROTATE_NUM] = {
    [AMBA_DSP_ROTATE_0]             = DSP_EFCT_ROT_NONE,
    [AMBA_DSP_ROTATE_0_HORZ_FLIP]   = DSP_EFCT_ROTATE_NONE_HFLIP,
    [AMBA_DSP_ROTATE_90]            = DSP_EFCT_ROT_90_DEGREE,
    [AMBA_DSP_ROTATE_90_VERT_FLIP]  = DSP_EFCT_ROTATE_90_VFLIP,
    [AMBA_DSP_ROTATE_180]           = DSP_EFCT_ROT_180_DEGREE,
    [AMBA_DSP_ROTATE_180_HORZ_FLIP] = DSP_EFCT_ROTATE_NONE_VFLIP,
    [AMBA_DSP_ROTATE_270]           = DSP_EFCT_ROT_270_DEGREE,
    [AMBA_DSP_ROTATE_270_VERT_FLIP] = DSP_EFCT_ROTATE_90_HFLIP,
};

const UINT8 YuvFmtTable[3U] = {
    [AMBA_DSP_YUV420 ] = DSP_YUV_420,
    [AMBA_DSP_YUV422 ] = DSP_YUV_422,
    [AMBA_DSP_YUV400 ] = DSP_YUV_MONO,
};

UINT32 HL_PollingEncodeTimerMode(UINT8 VinId, UINT8 WriteMode, UINT8 TimerScale, UINT32 WaitMs, UINT8 WaitType)
{
    UINT32 Rval = OK;
    UINT8 VinState;

(void)TimerScale;

    VinState = DSP_GetVinState(VinId);

    if (VinState != DSP_RAW_CAP_STATUS_TIMER) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_vin_idle_t *VinIdle = HL_DefCtxCmdBufPtrVinIdle;

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinIdle, &CmdBufferAddr);
        VinIdle->vin_id = VinId;
        (void)AmbaHL_CmdVinIdle(WriteMode, VinIdle);
        HL_RelCmdBuffer(CmdBufferId);

        if (WaitType == 0U) {
            if (OK != DSP_WaitVinState(VinId, DSP_RAW_CAP_STATUS_TIMER, WaitMs)) {
                AmbaLL_LogUInt5("Polling TimerMode Fail", 0U, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0000;
            }
        } else {
            //no support
        }
    } else {
        // do nothing
    }

    return Rval;
}

#if 0
UINT32 HL_FrameRateConvert(UINT32 Interlace, UINT32 Den, UINT32 Num)
{
    UINT32 FrameRate;

    FrameRate = ((Interlace << 31U) | (((Den == 1001U) ? (UINT32)1U: (UINT32)0U) << 30UL) | Num);

    return FrameRate;
}
#endif

UINT32 HL_GetViewZoneLocalIndexOnVin(UINT16 ViewZoneId)
{
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i = 0, VinIdx = 0, LocalIndex = 0, ViewZoneVinId;
    UINT8 IsDec2Vproc;
    UINT8 IsRecon2Vproc;
#ifdef DUPLEX_DEC_SHARE_VIRT_VIN
    CTX_DSP_INST_INFO_s DspInstInfo = {0};
#endif

    HL_GetResourcePtr(&Resource);
    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
    IsDec2Vproc = (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC)? (UINT8)1U: (UINT8)0U;
    IsRecon2Vproc = (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)? (UINT8)1U: (UINT8)0U;
#ifdef DUPLEX_DEC_SHARE_VIRT_VIN
    HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);
#endif

    HL_GetViewZoneVinId(ViewZoneId, &VinIdx);

    if (ViewZoneId == 0U) {
        LocalIndex = ViewZoneId;
    } else {
        for (i = 0U; i < ViewZoneId; i++) {
            if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
                continue;
            }
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
#ifdef DUPLEX_DEC_SHARE_VIRT_VIN
            if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) {
                continue;
            }

            if (IsDec2Vproc == (UINT8)1U) {
                if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) &&
                    (DspInstInfo.DecSharedVirtVinId != DSP_VIRT_VIN_IDX_INVALID)) {
                    LocalIndex++;
                }
            } else {
                HL_GetViewZoneVinId(i, &ViewZoneVinId);
                if ((ViewZoneVinId == VinIdx) &&
                    (VinIdx != VIN_IDX_INVALID)) {
                    LocalIndex++;
                }
            }
#else
            if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
                (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
                continue;
            }

            if ((IsDec2Vproc == (UINT8)0U) &&
                (IsRecon2Vproc == (UINT8)0U)) {
                HL_GetViewZoneVinId(i, &ViewZoneVinId);
                if ((ViewZoneVinId == VinIdx) &&
                    (VinIdx != VIN_IDX_INVALID)) {
                    LocalIndex++;
                }
            }
#endif
        }
    }

    return LocalIndex;
}

UINT32 HL_GetViewZoneNumOnVin(UINT16 VinId)
{
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i, ViewZoneVinId;
    UINT32 ViewZonNum = 0;
#ifdef DUPLEX_DEC_SHARE_VIRT_VIN
    CTX_DSP_INST_INFO_s DspInstInfo = {0};
#endif

    HL_GetResourcePtr(&Resource);
#ifdef DUPLEX_DEC_SHARE_VIRT_VIN
    HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);
#endif

    for (i = 0U; i < Resource->MaxViewZoneNum; i++) {
        if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
            continue;
        }
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);

        if (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_RAW_ONLY) {
            continue;
        }

#ifdef DUPLEX_DEC_SHARE_VIRT_VIN
        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) {
            continue;
        }

        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
            if ((DspInstInfo.DecSharedVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
                ((DspInstInfo.DecSharedVirtVinId + AMBA_DSP_MAX_VIN_NUM) == VinId)) {
                ViewZonNum++;
            }
        } else {
            HL_GetViewZoneVinId(i, &ViewZoneVinId);
            if (ViewZoneVinId == VinId) {
                ViewZonNum++;
            }
        }
#else
        if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
            continue;
        }
        HL_GetViewZoneVinId(i, &ViewZoneVinId);
        if (ViewZoneVinId == VinId) {
            ViewZonNum++;
        }
#endif
    }

    return ViewZonNum;
}

UINT32 HL_GetVoutTotalBit(void)
{
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    UINT16 i, VoutIdx = 0U;
    UINT32 VoutBitMask = 0U;

    HL_GetResourcePtr(&Resource);

    for (i=0; i<Resource->YuvStrmNum; i++) {
        HL_GetYuvStrmInfoPtr(i, &YuvStrmInfo);
        if (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) {
            if (YuvStrmInfo->DestVout > 0U) {
                DSP_Bit2U16Idx((UINT32)YuvStrmInfo->DestVout, &VoutIdx);
                DSP_SetBit(&VoutBitMask, VoutIdx);
            }
        }
    }

    /* Display */
    for (i=0; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
        if ((Resource->MaxVoutWidth[i] > 0U) || \
            (Resource->MaxOsdBufWidth[i] > 0U)) {
            DSP_SetBit(&VoutBitMask, i);
        }
    }
    return VoutBitMask;
}

UINT32 HL_GetEncFmtTotalBit(void)
{
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT16 i;
    UINT32 EncFmtBitMask = 0U;

    HL_GetResourcePtr(&Resource);

    for (i=0; i<Resource->MaxEncodeStream; i++) {
        if (1U == DSP_GetU8Bit(Resource->MaxStrmFmt[i], ENC_STRM_FMT_AVC_BIT_IDX, ENC_STRM_FMT_AVC_LEN)) {
            DSP_SetBit(&EncFmtBitMask, 0U);
        }
        if (1U == DSP_GetU8Bit(Resource->MaxStrmFmt[i], ENC_STRM_FMT_HEVC_BIT_IDX, ENC_STRM_FMT_HEVC_LEN)) {
            DSP_SetBit(&EncFmtBitMask, 1U);
        }
        if (1U == DSP_GetU8Bit(Resource->MaxStrmFmt[i], ENC_STRM_FMT_JPG_BIT_IDX, ENC_STRM_FMT_JPG_LEN)) {
            DSP_SetBit(&EncFmtBitMask, 2U);
        }
    }
    return EncFmtBitMask;
}

UINT32 HL_GetEncMaxWindow(UINT16 *Width, UINT16 *Height)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    UINT16 i = 0U;

    if ((Width == NULL) || (Height == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);

        for (i=0U; i<Resource->YuvStrmNum; i++) {
            HL_GetYuvStrmInfoPtr(i, &YuvStrmInfo);
            if (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U)) {
                *Width = (*Width > YuvStrmInfo->MaxWidth)? *Width: YuvStrmInfo->MaxWidth;
                *Height = (*Height > YuvStrmInfo->MaxHeight)? *Height: YuvStrmInfo->MaxHeight;
            }
        }
    }

    return Rval;
}

#ifdef SUPPORT_VPROC_OSD_INSERT
void HL_GetSysDlyLineCfg(UINT32 *pEncodeBit)
{
    CTX_RESOURCE_INFO_s *pResc = HL_CtxResInfoPtr;
    UINT16 i;

    HL_GetResourcePtr(&pResc);

    for (i = 0U; i < pResc->MaxEncodeStream; i++) {
        if (1U == DSP_GetU8Bit(pResc->MaxStrmFmt[i], ENC_STRM_FMT_OSDSYNC_BIT_IDX, ENC_STRM_FMT_OSDSYNC_LEN)) {
            DSP_SetBit(pEncodeBit, i);
        }
    }
}

void HL_GetSysDlyLineCfgOnViewZone(UINT16 ViewZoneId, UINT8 *pDlyLine)
{
    CTX_RESOURCE_INFO_s *pResc = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *pYuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    UINT16 i, EncStrmId = 0U;

    HL_GetResourcePtr(&pResc);

    for (i = 0U; i < pResc->YuvStrmNum; i++) {
        HL_GetYuvStrmInfoPtr(i, &pYuvStrmInfo);
        if (1U == DSP_GetU16Bit(pYuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U)) {
            if (1U == DSP_GetBit(pYuvStrmInfo->MaxChanBitMask, (UINT32)ViewZoneId, 1U)) {
                DSP_Bit2U16Idx(pYuvStrmInfo->DestEnc, &EncStrmId);
                if (1U == DSP_GetU8Bit(pResc->MaxStrmFmt[EncStrmId], ENC_STRM_FMT_OSDSYNC_BIT_IDX, ENC_STRM_FMT_OSDSYNC_LEN)) {
                    *pDlyLine = 1U;
                    break;
                }
            }
        }
    }
}

void HL_GetSysDlyLineOnViewZonePin(UINT16 ViewZoneId, UINT8 PinId, UINT16 *pDlyMs)
{
    CTX_RESOURCE_INFO_s *pResc = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *pYuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VPROC_INFO_s *pVprocInfo = HL_CtxVprocInfoPtr;
    UINT16 i, EncStrmId = 0U;

    HL_GetResourcePtr(&pResc);

    for (i = 0U; i < pResc->YuvStrmNum; i++) {
        HL_GetYuvStrmInfoPtr(i, &pYuvStrmInfo);
        if (1U == DSP_GetU16Bit(pYuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U)) {
            if (1U == DSP_GetBit(pYuvStrmInfo->MaxChanBitMask, (UINT32)ViewZoneId, 1U)) {
                HL_GetVprocInfoPtr(ViewZoneId, &pVprocInfo);
                if (1U == DSP_GetBit(pVprocInfo->PinUsage[PinId], i, 1U)) {
                    DSP_Bit2U16Idx(pYuvStrmInfo->DestEnc, &EncStrmId);
                    if (1U == DSP_GetU8Bit(pResc->MaxStrmFmt[EncStrmId], ENC_STRM_FMT_OSDSYNC_BIT_IDX, ENC_STRM_FMT_OSDSYNC_LEN)) {
                        *pDlyMs = pResc->MaxOsdSyncDly[EncStrmId];
                        break;
                    }
                }
            }
        }
    }
}
#endif

static inline void HL_GetLevelIdcAvc(const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *StrmCfg,
                                     UINT8 *pLevelIdc)
{
    UINT32 FrameSize, TotalMbCntPerSec;
    UINT32 MaxBR = StrmCfg->EncConfig.BitRateCfg.BitRate;
    UINT32 CbpBrVclFactor = 1250; //assume HighProfile
    UINT32 W = ALIGN_NUM((UINT32)StrmCfg->Window.Width, 16U);
    UINT32 H = ALIGN_NUM((UINT32)StrmCfg->Window.Height, 16U);

    FrameSize = (W >> 4U) * (H >> 4U);
    TotalMbCntPerSec = FrameSize * (StrmCfg->FrameRate.TimeScale / StrmCfg->FrameRate.NumUnitsInTick);

    /* Table A-1, Annex A.3 */
    if (TotalMbCntPerSec > 983040U) {
        *pLevelIdc = 52U;
    } else if (TotalMbCntPerSec > 589824U) {
        *pLevelIdc = 51U;
        if (MaxBR > (240000U*CbpBrVclFactor)) {
            *pLevelIdc = 52U;
        }
    } else if (TotalMbCntPerSec > 522240U) {
        *pLevelIdc = 50U;
        if (MaxBR > (135000U*CbpBrVclFactor)) {
            *pLevelIdc = 51U;
        }
    } else if (TotalMbCntPerSec > 245760U) {
        *pLevelIdc = 42U;
        if (MaxBR > (50000U*CbpBrVclFactor)) {
            *pLevelIdc = 50U;
        }
    } else {
        if (MaxBR > (50000U*CbpBrVclFactor)) {
            *pLevelIdc = 50U;
        }
    }

    if ((FrameSize > 22080U) &&
        (*pLevelIdc <= 50U)) {
        *pLevelIdc = 51U;
    } else if ((FrameSize > 8704U) &&
               (*pLevelIdc <= 42U)) {
        *pLevelIdc = 50U;
    } else {
        //TBD
    }
}

static inline void HL_GetLevelIdcHevcSmallPic(const UINT32 CbpRate,
                                              const UINT32 CbpFactor,
                                              UINT8 *pLevelIdc,
                                              UINT8 *pTierIdc)
{
    if (CbpRate < (20000U*CbpFactor)) {
        *pTierIdc = 0U;
        *pLevelIdc = 41U;
        if (CbpRate < (12000U*CbpFactor)) {
            *pLevelIdc = 40U;
        }
    } else {
        *pTierIdc = 1U;
        *pLevelIdc = 41U;
        if (CbpRate < (30000U*CbpFactor)) {
            *pLevelIdc = 40U;
        }
    }
}

static inline void HL_GetLevelIdcHevcMidPic(const UINT32 CbpRate,
                                            const UINT32 CbpFactor,
                                            UINT8 *pLevelIdc,
                                            UINT8 *pTierIdc)
{
    if (CbpRate < (60000U*CbpFactor)) {
        *pTierIdc = 0U;
        *pLevelIdc = 52U;
        if (CbpRate < (25000U*CbpFactor)) {
            *pLevelIdc = 50U;
        } else if (CbpRate < (40000U*CbpFactor)) {
            *pLevelIdc = 51U;
        } else {
            //TBD
        }
    } else {
        *pTierIdc = 1U;
        *pLevelIdc = 52U;
        if (CbpRate < (100000U*CbpFactor)) {
            *pLevelIdc = 50U;
        } else if (CbpRate < (160000U*CbpFactor)) {
            *pLevelIdc = 51U;
        } else {
            //TBD
        }
    }
}

static inline void HL_GetLevelIdcHevcLargePic(const UINT32 CbpRate,
                                              const UINT32 CbpFactor,
                                              UINT8 *pLevelIdc,
                                              UINT8 *pTierIdc)
{
    if (CbpRate <= (240000U*CbpFactor)) {
        *pTierIdc = 0U;
        *pLevelIdc = 62U;
        if (CbpRate < (60000U*CbpFactor)) {
            *pLevelIdc = 60U;
        } else if (CbpRate < (120000U*CbpFactor)) {
            *pLevelIdc = 61U;
        } else {
            //TBD
        }
    } else {
        *pTierIdc = 1U;
        *pLevelIdc = 62U;
        if (CbpRate < (240000U*CbpFactor)) {
            *pLevelIdc = 60U;
        } else if (CbpRate < (480000U*CbpFactor)) {
            *pLevelIdc = 61U;
        } else {
            //TBD
        }
    }
}

static inline void HL_GetLevelIdcHevcSamleRate(const UINT32 SamplePerSecond,
                                               UINT8 *pLevelIdc)
{
    if ((SamplePerSecond > 2139095040U) &&
       (*pLevelIdc <= 61U)) {
       *pLevelIdc = 62U;
    } else if ((SamplePerSecond > 1069547520U) &&
              (*pLevelIdc <= 60U)) {
       *pLevelIdc = 61U;
    } else if ((SamplePerSecond > 534773760U) &&
              (*pLevelIdc <= 51U)) {
       *pLevelIdc = 52U;
    } else if ((SamplePerSecond > 267386880U) &&
              (*pLevelIdc <= 50U)) {
       *pLevelIdc = 51U;
    } else if ((SamplePerSecond > 133693440U) &&
              (*pLevelIdc <= 41U)) {
       *pLevelIdc = 50U;
    } else if ((SamplePerSecond > 66846720U) &&
              (*pLevelIdc <= 40U)) {
       *pLevelIdc = 41U;
    } else {
       //TBD
    }
}

static inline void HL_GetLevelIdcHevc(const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *StrmCfg,
                                      UINT8 *pLevelIdc,
                                      UINT8 *pTierIdc)
{
    UINT32 W = ALIGN_NUM((UINT32)StrmCfg->Window.Width, 16U);
    UINT32 H = ALIGN_NUM((UINT32)StrmCfg->Window.Height, 16U);
    UINT32 PicSize = W*H;
    UINT32 SamplePerSecond;
    UINT32 CbpRate = StrmCfg->EncConfig.BitRateCfg.BitRate;
    UINT32 CbpFactor = 1000U;

    SamplePerSecond = PicSize*(StrmCfg->FrameRate.TimeScale / StrmCfg->FrameRate.NumUnitsInTick);

    /* level/tier (Table A.1) */
    if (PicSize <= 2228224U) {
        HL_GetLevelIdcHevcSmallPic(CbpRate, CbpFactor, pLevelIdc, pTierIdc);
    } else if (PicSize <= 8912896U) {
        HL_GetLevelIdcHevcMidPic(CbpRate, CbpFactor, pLevelIdc, pTierIdc);
    } else {
        HL_GetLevelIdcHevcLargePic(CbpRate, CbpFactor, pLevelIdc, pTierIdc);
    }

    /* Table A.2 */
    HL_GetLevelIdcHevcSamleRate(SamplePerSecond, pLevelIdc);
}

UINT32 HL_GetLevelIdc(const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *StrmCfg, UINT8 *pLevelIdc, UINT8 *pTierIdc)
{
    UINT32 Rval = OK;

    if (StrmCfg->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
        HL_GetLevelIdcAvc(StrmCfg, pLevelIdc);
    } else {
        HL_GetLevelIdcHevc(StrmCfg, pLevelIdc, pTierIdc);
    }

    return Rval;
}

UINT32 HL_GetHevcMaxTileNum(UINT8 *TileNum, UINT16 EncodeWidth)
{
    const UINT16 MaxTileWidth = 256; //From HEVC Standard

    /*
     * According to Shihao, HEVC-encode's tile number must between 1 to 3.
     * 3 has better performance but may have artifact in tile boundary
     * 4K must run tile = 3 due to performance issue
     * only use tile = 1 in the following case:
     *  - encode size <= CIF (352*288)
     *  - aaa complains bad quality in tile boundary
     *  - when some resolutions can not afford multiple tile number
     */

    *TileNum = (UINT8)(EncodeWidth/MaxTileWidth);
    if (*TileNum > 3U) {
        *TileNum = 3U;
    } else if (*TileNum == 0U) {
        *TileNum = 1U;
    } else {
        // Keep the same value
    }
    return OK;
}

static inline void HL_CommPinBufMaxWindow(UINT16 PinId, UINT16 *pWidth, UINT16 *pHeight)
{
    UINT8 ExitILoop;
    UINT8 IsEfctYuvStrm, IsVoutPurpose, IsEncPurpose;
    UINT16 i;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VPROC_INFO_s VprocInfo = {0};

    HL_GetResourcePtr(&Resource);
    ExitILoop = 0U;
    for (i=0U; i<Resource->YuvStrmNum; i++) {
        HL_GetYuvStrmInfoPtr(i, &YuvStrmInfo);
        IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(i))? 1U: 0U;
        IsVoutPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U))? 1U: 0U;
        IsEncPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U))? 1U: 0U;

        // Only effect channels use postp buffers
        if (0U == IsEfctYuvStrm) {
            continue;
        }
        if ((PinId == DSP_VPROC_PIN_MAIN) && (IsVoutPurpose == 0U)) {
            HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &VprocInfo);
            if (VprocInfo.PinUsage[DSP_VPROC_PIN_MAIN] > 0U) {
                if (1U == DSP_GetBit(VprocInfo.PinUsage[DSP_VPROC_PIN_MAIN], i, 1U)) {
                    *pWidth = YuvStrmInfo->MaxWidth;
                    *pHeight = YuvStrmInfo->MaxHeight;

                    ExitILoop = 1U;
                }
            }
        } else if (PinId == DSP_VPROC_PIN_PREVA) {
            HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &VprocInfo);
            if (VprocInfo.PinUsage[DSP_VPROC_PIN_PREVA] > 0U) {
                if (1U == DSP_GetBit(VprocInfo.PinUsage[DSP_VPROC_PIN_PREVA], i, 1U)) {
                    *pWidth = YuvStrmInfo->MaxWidth;
                    *pHeight = YuvStrmInfo->MaxHeight;
                    ExitILoop = 1U;
                }
            }
        } else {
            // DO NOTHING
        }

        /* AVC need 16 alignment */
        if (IsEncPurpose == 1U) {
            *pHeight = ALIGN_NUM16(*pHeight, 16U);
        }

        if (ExitILoop == 1U) {
            break;
        }
    }
}

static inline void HL_CommDestBufMaxWindow(UINT16 Dest, UINT16 *pWidth, UINT16 *pHeight)
{
    UINT8 IsEfctYuvStrm;
    UINT16 i;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    UINT8 ExitILoop = 0U;
#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
    CTX_VPROC_INFO_s VprocInfo = {0};
#endif

    HL_GetResourcePtr(&Resource);
    for (i=0U; i<Resource->YuvStrmNum; i++) {
        HL_GetYuvStrmInfoPtr(i, &YuvStrmInfo);
        IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(i))? (UINT8)1U: (UINT8)0U;

        // Only Vout YuvStreams use common buffers
        if (0U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) {
            continue;
        }

#if defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
        /*
         * 20190619, In CVx current design
         * PostP_XX_ and Prev_comX_ have different meaning
         * NonEffect Pipeline
         *   - means the YuvBuffer to Destination
         * Effect Pipeline
         *   - means the YuvBuffer from which VprocPin
         *     postp_main_ : From Main
         *     postp_Pip_ : From PrevA
         *     prev_com0(DEST_VOUT0) : From PrevC
         *     prev_com1(DEST_VOUT1) : From PrevB
         */

        if (1U == IsEfctYuvStrm) {
            if ((1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) &&
                (Dest == AMBA_DSP_PREV_DEST_VOUT0)) {
                HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &VprocInfo);
                if (VprocInfo.PinUsage[DSP_VPROC_PIN_PREVC] > 0U) {
                    if (1U == DSP_GetBit(VprocInfo.PinUsage[DSP_VPROC_PIN_PREVC], i, 1U)) {
                        *pWidth = YuvStrmInfo->MaxWidth;
                        *pHeight = YuvStrmInfo->MaxHeight;
                        ExitILoop = (UINT8)1U;
                    }
                }
            } else if ((1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) &&
                       (Dest == AMBA_DSP_PREV_DEST_VOUT1)) {
                HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &VprocInfo);
                if (VprocInfo.PinUsage[DSP_VPROC_PIN_PREVB] > 0U) {
                    if (1U == DSP_GetBit(VprocInfo.PinUsage[DSP_VPROC_PIN_PREVB], i, 1U)) {
                        *pWidth = YuvStrmInfo->MaxWidth;
                        *pHeight = YuvStrmInfo->MaxHeight;
                        ExitILoop = (UINT8)1U;
                    }
                }
            } else {
                // DO NOTHING
            }
        } else {
            if ((1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, VOUT_IDX_A, 1U)) &&
                (Dest == AMBA_DSP_PREV_DEST_VOUT0)) {
                *pWidth = YuvStrmInfo->MaxWidth;
                *pHeight = YuvStrmInfo->MaxHeight;
                ExitILoop = (UINT8)1U;
            } else if ((1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, VOUT_IDX_B, 1U)) &&
                       (Dest == AMBA_DSP_PREV_DEST_VOUT1)) {
                *pWidth = YuvStrmInfo->MaxWidth;
                *pHeight = YuvStrmInfo->MaxHeight;
                ExitILoop = (UINT8)1U;
            } else {
                // DO NOTHING
            }
        }

#elif defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE)
        /*
         * 20190619, In CVx current design
         * PostP_XX_ and Prev_comX_ have different meaning
         * NonEffect Pipeline
         *   - means the YuvBuffer to Destination
         * Effect Pipeline
         *   - means the YuvBuffer from which VprocPin
         *     postp_main_ : From Main
         *     postp_Pip_ : From PrevA
         *     prev_com0(DEST_VOUT0) : From PrevC
         *     prev_com1(DEST_VOUT1) : From PrevB
         */

        if (1U == IsEfctYuvStrm) {
            if ((1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) &&
                (Dest == AMBA_DSP_PREV_DEST_VOUT0)) {
                HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &VprocInfo);
                if (VprocInfo.PinUsage[DSP_VPROC_PIN_PREVC] > 0U) {
                    if (1U == DSP_GetBit(VprocInfo.PinUsage[DSP_VPROC_PIN_PREVC], i, 1U)) {
                        *pWidth = YuvStrmInfo->MaxWidth;
                        *pHeight = YuvStrmInfo->MaxHeight;
                        ExitILoop = (UINT8)1U;
                    }
                }
            } else if ((1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) &&
                       (Dest == AMBA_DSP_PREV_DEST_VOUT1)) {
                HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &VprocInfo);
                if (VprocInfo.PinUsage[DSP_VPROC_PIN_PREVB] > 0U) {
                    if (1U == DSP_GetBit(VprocInfo.PinUsage[DSP_VPROC_PIN_PREVB], i, 1U)) {
                        *pWidth = YuvStrmInfo->MaxWidth;
                        *pHeight = YuvStrmInfo->MaxHeight;
                        ExitILoop = (UINT8)1U;
                    }
                }
            } else {
                // DO NOTHING
            }
        } else {
            if ((1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, VOUT_IDX_A, 1U)) &&
                (Dest == AMBA_DSP_PREV_DEST_VOUT0)) {
                *pWidth = YuvStrmInfo->MaxWidth;
                *pHeight = YuvStrmInfo->MaxHeight;
                ExitILoop = (UINT8)1U;
            } else if ((1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, VOUT_IDX_B, 1U)) &&
                       (Dest == AMBA_DSP_PREV_DEST_VOUT1)) {
                *pWidth = YuvStrmInfo->MaxWidth;
                *pHeight = YuvStrmInfo->MaxHeight;
                ExitILoop = (UINT8)1U;
            } else {
                // DO NOTHING
            }
        }
#else
        if ((1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, VOUT_IDX_A, 1U)) &&
            (Dest == AMBA_DSP_PREV_DEST_VOUT0)) {
            *pWidth = YuvStrmInfo->MaxWidth;
            *pHeight = YuvStrmInfo->MaxHeight;
            ExitILoop = (UINT8)1U;
        } else if ((1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, VOUT_IDX_B, 1U)) &&
                   (Dest == AMBA_DSP_PREV_DEST_VOUT1)) {
            *pWidth = YuvStrmInfo->MaxWidth;
            *pHeight = YuvStrmInfo->MaxHeight;
            ExitILoop = (UINT8)1U;
        } else {
            // DO NOTHING
        }
#endif
        if (ExitILoop == 1U) {
            break;
        }
    }
}

UINT32 HL_GetCommBufMaxWindow(UINT16 Dest, UINT16 PinId, UINT16 *pWidth, UINT16 *pHeight)
{
    UINT32 Rval = OK;

    if ((pWidth == NULL) || (pHeight == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (Dest == AMBA_DSP_PREV_DEST_PIN) {
            HL_CommPinBufMaxWindow(PinId, pWidth, pHeight);
        } else {
            HL_CommDestBufMaxWindow(Dest, pWidth, pHeight);
        }
    }

    return Rval;
}

UINT32 HL_GetEffectEnableOnYuvStrm(const UINT16 YuvStrmId)
{
    UINT32 Enable = 0U;
    CTX_YUV_STRM_INFO_s *pYuvStrm = HL_CtxYuvStrmInfoPtr;

    if (YuvStrmId < AMBA_DSP_MAX_YUVSTRM_NUM) {
        HL_GetYuvStrmInfoPtr(YuvStrmId, &pYuvStrm);
        Enable = HL_GetYuvStrmIsEffectOut(pYuvStrm->MaxChanNum,
                                          pYuvStrm->Layout.ChanCfg[0U].Window.OffsetX,
                                          pYuvStrm->Layout.ChanCfg[0U].Window.OffsetY,
                                          pYuvStrm->Layout.ChanCfg[0U].RotateFlip);
    }

    return Enable;
}

UINT32 HL_GetEffectChannelEnable(void)
{
    UINT16 i;
    UINT32 Enable = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    for (i=0; i<Resource->YuvStrmNum; i++) {
        if (1U == HL_GetEffectEnableOnYuvStrm(i)) {
            Enable = 1U;
            break;
        }
    }
    return Enable;
}

UINT8 HL_GetDefaultRawEnable(void)
{
    UINT8 Enable = 0U;
    UINT16 i;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    //if any vin support timeout, then use independent y2y buffer for every chan
    for (i=0; i<AMBA_DSP_MAX_VIN_NUM; i++) {
        if (Resource->DefaultRawBuf[i].BaseAddr != 0U) {
            Enable = 1U;
            break;
        }
    }
    return Enable;
}

UINT32 HL_GetProcRawDramOutEnable(UINT8 *pEnable)
{
    UINT32 Rval = OK;
    UINT16 i;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetResourcePtr(&Resource);
    for (i = 0U; i<Resource->MaxViewZoneNum; i++) {
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        if (ViewZoneInfo->IsProcRawDramOut == (UINT8)1U) {
            *pEnable = 1U;
            break;
        }
    }

    return Rval;
}

UINT32 HL_GetEffectChannelPostBlendNum(UINT16 YuvStrmIdx, const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout, UINT16 *TotalPostBldNum)
{
    UINT16 i;
    UINT32 Rval = OK;

(void)YuvStrmIdx;
    *TotalPostBldNum = 0U;

    /*
     * Except for user-defined blending jobs, SSP handles rotation by an extra blending,
     * thus when there is a rotation-only channel, it actually contains a blending job
     * */
    for (i=0; i<pYuvStrmLayout->NumChan; i++) {
        if (pYuvStrmLayout->ChanCfg[i].BlendNum > 0U) {
            *TotalPostBldNum += pYuvStrmLayout->ChanCfg[i].BlendNum;
        } else if ((pYuvStrmLayout->ChanCfg[i].RotateFlip == AMBA_DSP_ROTATE_90) ||
                   (pYuvStrmLayout->ChanCfg[i].RotateFlip == AMBA_DSP_ROTATE_270) ||
                   (pYuvStrmLayout->ChanCfg[i].RotateFlip == AMBA_DSP_ROTATE_180)) {
            *TotalPostBldNum += 1U;
        } else {
            // No increment
        }
    }

    return Rval;
}

#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
UINT32 HL_GetEffectChannelVprocPin(UINT16 Purpose,
                                   UINT16 DestVout,
                                   UINT32 DestEnc,
                                   UINT16 *pCandNum,
                                   UINT16 *pCandPinId,
                                   UINT8 IsDramMipiYuv,
                                   UINT8 ChromaFmt)
{
    UINT32 Rval = OK;
    UINT8 IsEncPurpose, IsVoutPurpose, IsVoutDestA, IsVoutDestB;
    UINT16 Num = 0U;

    (void)DestEnc;
    (void)ChromaFmt;
    IsEncPurpose =(1U == DSP_GetU16Bit(Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U))? 1U: 0U;
    IsVoutPurpose = (1U == DSP_GetU16Bit(Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U))? 1U: 0U;

    if (IsEncPurpose == 1U) {
        /*
         * Under Efect.
         * PIN_MAIN are not limited to 1st enc stream, so does PIN_PREVA
         * but PIN_MAIN can't have ROI and rescale
         */
        /* Filling in size increment order */
        pCandPinId[Num] = DSP_VPROC_PIN_PREVA;
        Num++;
        pCandPinId[Num] = DSP_VPROC_PIN_MAIN;
        Num++;
    } else if (IsVoutPurpose == 1U) {
#if defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
        IsVoutDestA = (1U == DSP_GetU16Bit(DestVout, VOUT_IDX_A, 1U))? 1U: 0U;
        IsVoutDestB = (1U == DSP_GetU16Bit(DestVout, VOUT_IDX_B, 1U))? 1U: 0U;

        if ((1U == IsVoutDestA) ||
            (1U == IsVoutDestB)) {
            /* Filling in size increment order */
            if (IsDramMipiYuv == 0U) {
                pCandPinId[Num] = DSP_VPROC_PIN_PREVC;
                Num++;
                if (ChromaFmt == AMBA_DSP_YUV420) {
                    pCandPinId[Num] = DSP_VPROC_PIN_PREVA;
                    Num++;
                }
                pCandPinId[Num] = DSP_VPROC_PIN_PREVB;
                Num++;
            } else {
                pCandPinId[Num] = DSP_VPROC_PIN_PREVA;
                Num++;
            }
        } else {
            pCandPinId[0U] = DSP_VPROC_PIN_NUM;
        }
#else
        IsVoutDestA = (1U == DSP_GetU16Bit(DestVout, VOUT_IDX_A, 1U))? 1U: 0U;
        IsVoutDestB = (1U == DSP_GetU16Bit(DestVout, VOUT_IDX_B, 1U))? 1U: 0U;

        if ((1U == IsVoutDestA) ||
            (1U == IsVoutDestB)) {
            /* Filling in size increment order */
            pCandPinId[Num] = DSP_VPROC_PIN_PREVC;
            Num++;
            pCandPinId[Num] = DSP_VPROC_PIN_PREVB;
            Num++;
        } else {
            pCandPinId[0U] = DSP_VPROC_PIN_NUM;
        }
#endif
    } else {
        pCandPinId[0U] = DSP_VPROC_PIN_NUM;
    }
    *pCandNum = Num;

    return Rval;
}
#else
UINT32 HL_GetEffectChannelVprocPin(UINT16 Purpose,
                                   UINT16 DestVout,
                                   UINT32 DestEnc,
                                   UINT16 *pPinId,
                                   UINT8 IsDramMipiYuv)
{
    UINT32 Rval = OK;
    UINT8 IsEncPurpose, IsVoutPurpose;

    IsEncPurpose =(1U == DSP_GetU16Bit(Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U))? 1U: 0U;
    IsVoutPurpose = (1U == DSP_GetU16Bit(Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U))? 1U: 0U;

    *pPinId = DSP_VPROC_PIN_NUM;
    if (IsEncPurpose == 1U) {
        if (1U == DSP_GetBit(DestEnc, 0U/*MainStrm*/, 1U)) {
            *pPinId = DSP_VPROC_PIN_MAIN;
        } else if (1U == DSP_GetBit(DestEnc, 1U/*2ndStrm*/, 1U)) {
            *pPinId = DSP_VPROC_PIN_PREVA;
        } else {
            *pPinId = DSP_VPROC_PIN_NUM;
        }
    }

    if (IsVoutPurpose == 1U) {
        if (1U == DSP_GetU16Bit(DestVout, VOUT_IDX_A, 1U)) {
            *pPinId = (IsDramMipiYuv == (UINT8)0U)? DSP_VPROC_PIN_PREVC: DSP_VPROC_PIN_NUM;
        } else if (1U == DSP_GetU16Bit(DestVout, VOUT_IDX_B, 1U)) {
            *pPinId = (IsDramMipiYuv == (UINT8)0U)? DSP_VPROC_PIN_PREVB: DSP_VPROC_PIN_NUM;
        } else {
            *pPinId = DSP_VPROC_PIN_NUM;
        }
    }

    return Rval;
}
#endif

static UINT8 HL_FindOverlapeforHori(const AMBA_DSP_WINDOW_s *WinA,
                                     const AMBA_DSP_WINDOW_s *WinB,
                                     AMBA_DSP_WINDOW_s *WinOverlap)
{
    UINT8 HoriOverlap = 0U;

    /* search for H overlap */
    if ((WinB->OffsetX <= WinA->OffsetX) &&
        ((WinB->OffsetX + WinB->Width) >= WinA->OffsetX) &&
        ((WinB->OffsetX + WinB->Width) <= (WinA->OffsetX + WinA->Width))) {
        WinOverlap->OffsetX = WinA->OffsetX;
        WinOverlap->Width = (WinB->OffsetX + WinB->Width) - WinA->OffsetX;
        HoriOverlap = 1U;
    } else if ((WinB->OffsetX <= WinA->OffsetX) &&
               ((WinB->OffsetX + WinB->Width) >= (WinA->OffsetX + WinA->Width))) {
        WinOverlap->OffsetX = WinA->OffsetX;
        WinOverlap->Width = WinA->Width;
        HoriOverlap = 1U;
    } else if ((WinB->OffsetX >= WinA->OffsetX) &&
               ((WinB->OffsetX + WinB->Width) <= (WinA->OffsetX + WinA->Width))) {
        WinOverlap->OffsetX = WinB->OffsetX;
        WinOverlap->Width = WinB->Width;
        HoriOverlap = 1U;
    } else if ((WinB->OffsetX >= WinA->OffsetX) &&
               (WinB->OffsetX <= (WinA->OffsetX + WinA->Width)) &&
               ((WinB->OffsetX + WinB->Width) >= (WinA->OffsetX + WinA->Width))) {
        WinOverlap->OffsetX = WinB->OffsetX;
        WinOverlap->Width = (WinA->OffsetX + WinA->Width) - WinB->OffsetX;
        HoriOverlap = 1U;
    } else {
        //TBD
    }

    return HoriOverlap;
}

static UINT8 HL_FindOverlapeforVert(const AMBA_DSP_WINDOW_s *WinA,
                                     const AMBA_DSP_WINDOW_s *WinB,
                                     AMBA_DSP_WINDOW_s *WinOverlap)
{
    UINT8 VertOverlape = 0U;

    /* Search for V overlap */
    if ((WinB->OffsetY <= WinA->OffsetY) &&
        ((WinB->OffsetY + WinB->Height) >= WinA->OffsetY) &&
        ((WinB->OffsetY + WinB->Height) <= (WinA->OffsetY + WinA->Height))) {
        WinOverlap->OffsetY = WinA->OffsetY;
        WinOverlap->Height = (WinB->OffsetY + WinB->Height) - WinA->OffsetY;
        VertOverlape = 1U;
    } else if ((WinB->OffsetY <= WinA->OffsetY) &&
               ((WinB->OffsetY + WinB->Height) >= (WinA->OffsetY + WinA->Height))) {
        WinOverlap->OffsetY = WinA->OffsetY;
        WinOverlap->Height = WinA->Height;
        VertOverlape = 1U;
    } else if ((WinB->OffsetY >= WinA->OffsetY) &&
               ((WinB->OffsetY + WinB->Height) <= (WinA->OffsetY + WinA->Height))) {
        WinOverlap->OffsetY = WinB->OffsetY;
        WinOverlap->Height = WinB->Height;
        VertOverlape = 1U;
    } else if ((WinB->OffsetY >= WinA->OffsetY) &&
               (WinB->OffsetY <= (WinA->OffsetY + WinA->Height)) &&
               ((WinB->OffsetY + WinB->Height) >= (WinA->OffsetY + WinA->Height))) {
        WinOverlap->OffsetY = WinB->OffsetY;
        WinOverlap->Height = (WinA->OffsetY + WinA->Height) - WinB->OffsetY;
        VertOverlape = 1U;
    } else {
        //TBD
    }

    return VertOverlape;
}

static UINT32 HL_FindOverlapeRegion(const AMBA_DSP_WINDOW_s *WinA,
                                    const AMBA_DSP_WINDOW_s *WinB,
                                    AMBA_DSP_WINDOW_s *WinOverlap,
                                    UINT16 *IsOverLap)
{
    UINT32 Rval = OK;
    UINT8 HoriOverlap = 0U, VertOverlape = 0U;

    (void)dsp_osal_memset(WinOverlap, 0, sizeof(AMBA_DSP_WINDOW_s));

    /* search for H overlap */
    HoriOverlap = HL_FindOverlapeforHori(WinA, WinB, WinOverlap);
    *IsOverLap += HoriOverlap;

    if (HoriOverlap > 0U) {
        /* Search for V overlap */
        VertOverlape = HL_FindOverlapeforVert(WinA, WinB, WinOverlap);
        *IsOverLap += VertOverlape;
    }

    return Rval;
}

//#define DEBUG_EFCT_JOB_CALC
static inline UINT32 HL_CalcEffectPostPJobInit(const CTX_RESOURCE_INFO_s *pResource,
                                               const DSP_EFFECT_BUF_DESC_s *pCurrentBufDesc,
                                               DSP_EFFECT_BUF_DESC_s *pBufDesc,
                                               CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                               UINT32 *pEffectBufMask)
{
    UINT16 i;
    UINT32 Rval = OK;

    /* Reset */
    for (i=0; i < pYuvStrmLayout->NumChan; i++) {
        (void)dsp_osal_memset(&pYuvStrmLayout->BlendJobDesc[i][0U],
                              0,
                              sizeof(DSP_EFFECT_BLEND_JOB_s)*DSP_MAX_PP_STRM_BLEND_NUMBER_CTX);

        pYuvStrmLayout->CopyJobNum[i] = 0U;
        (void)dsp_osal_memset(&pYuvStrmLayout->CopyJobDesc[i][0U],
                              0,
                              sizeof(DSP_EFFECT_COPY_JOB_s)*DSP_MAX_PP_STRM_COPY_NUMBER);

        /* skip reset temp buffer
           ucoder claims that once temp buffers are initialized, they have to be used in next multi-stream-pp
        */
        pBufDesc->OutputBufId       = pCurrentBufDesc->OutputBufId;
        pBufDesc->CpyBufId          = pCurrentBufDesc->CpyBufId;
        pBufDesc->Y2YBufId          = pCurrentBufDesc->Y2YBufId;
        pBufDesc->FirstPsThIdx      = DSP_EFCT_INVALID_IDX;
        pBufDesc->FirstCpyIdx       = DSP_EFCT_INVALID_IDX;
        pBufDesc->FirstBldIdx       = DSP_EFCT_INVALID_IDX;
        pBufDesc->FirstY2YBldIdx    = DSP_EFCT_INVALID_IDX;
        pBufDesc->LastPsThIdx       = DSP_EFCT_INVALID_IDX;
        pBufDesc->LastCpyIdx        = DSP_EFCT_INVALID_IDX;
        pBufDesc->LastBldIdx        = DSP_EFCT_INVALID_IDX;
#ifdef PPSTRM_INDPT_INT_BLD_BUFFER
        pBufDesc->ChannelOutputMask = 0U;
#endif
    }

    if (pCurrentBufDesc->OutputBufId == DSP_EFCT_INVALID_IDX) {
        /* Take the first available buffer as the final output buffer */
        DSP_FindEmptyBit(pEffectBufMask,
                         pResource->EffectLogicBufNum,
                         &pBufDesc->OutputBufId,
                         0U/*FindOnly*/,
                         EFFECT_BUF_MASK_DEPTH);
        if (pBufDesc->OutputBufId == DSP_EFCT_INVALID_IDX) {
            AmbaLL_LogUInt5("  No Buf for EffOut 0x%X%X%X%X",
                            pEffectBufMask[3], pEffectBufMask[2],
                            pEffectBufMask[1], pEffectBufMask[0], 0U);
            Rval = DSP_ERR_0006;
        } else {
            //TBD
#ifdef DEBUG_EFCT_JOB_CALC
        AmbaLL_LogUInt5("FinalOut %d", pBufDesc->OutputBufId, 0U, 0U, 0U, 0U);
#endif
        }
    }

    return Rval;
}

/* Handle Blend
 *  1. If blending window perfectly covers chanA window:
 *     (1) output chanA to Y2Y buffer
 *     (2) blend Y2Y buffer with the final output buffer, whom its blending target is on
 *  2. If it does not, and only overlap to one View
 *     (1) copy the overlap region of its blending target to Cpy buffer
 *     (2) blend Cpy buffer with the final output buffer
 *     Note: if its blending target has rotation, the copied content is not rotated
 * */
static inline UINT32 HL_CalcEffectPostPJobBldCpy(const UINT16 BldIdx,
                                                 const CTX_RESOURCE_INFO_s *pResource,
                                                 const AMBA_DSP_BUF_s *BldBuf,
                                                 CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                                 DSP_EFFECT_BUF_DESC_s *pBufDesc,
                                                 DSP_EFFECT_BLEND_JOB_s *pBldJobDesc,
                                                 UINT32 *pEffectBufMask)
{
    UINT16 CpyIdx, OverlapChanIdx;
    UINT16 IsOverLap = 0U;
    UINT32 Rval = OK;
    AMBA_DSP_WINDOW_s OverLapWin;
    DSP_EFFECT_COPY_JOB_s *pCpyJobDesc;

    DSP_Bit2U16Idx((UINT32)pBldJobDesc->OverlapChan, &OverlapChanIdx);
    (void)HL_FindOverlapeRegion(&pYuvStrmLayout->ChanCfg[OverlapChanIdx].Window,
                                &BldBuf[BldIdx].Window,
                                &OverLapWin,
                                &IsOverLap);

    /* Need Copy */
    CpyIdx = pYuvStrmLayout->CopyJobNum[OverlapChanIdx];
    pCpyJobDesc = &pYuvStrmLayout->CopyJobDesc[OverlapChanIdx][CpyIdx];
    pCpyJobDesc->JobId = 0U; //TBD
    pCpyJobDesc->SrcBufIdx = pBufDesc->OutputBufId;

    /* Search Copy Buffer */
    if (pBufDesc->Y2YBufId != DSP_EFCT_INVALID_IDX) {
        pBufDesc->CpyBufId = pBufDesc->Y2YBufId;;
    } else {
        if (pBufDesc->CpyBufId == DSP_EFCT_INVALID_IDX) {
            DSP_FindEmptyBit(pEffectBufMask,
                             pResource->EffectLogicBufNum,
                             &pBufDesc->CpyBufId,
                             0U/*FindOnly*/,
                             EFFECT_BUF_MASK_DEPTH);
            if (pBufDesc->CpyBufId == DSP_EFCT_INVALID_IDX) {
                AmbaLL_LogUInt5("  No Buf for CpyOut 0x%X%X%X%X",
                                pEffectBufMask[3], pEffectBufMask[2],
                                pEffectBufMask[1], pEffectBufMask[0], 0U);
                Rval = DSP_ERR_0006;
            }
        }
    }

    pCpyJobDesc->DestBufIdx = pBufDesc->CpyBufId;
#ifdef DEBUG_EFCT_JOB_CALC
        AmbaLL_LogUInt5("CpyBufId %d", pBufDesc->CpyBufId, 0U, 0U, 0U, 0U);
#endif

    (void)dsp_osal_memcpy(&pCpyJobDesc->SrcWin, &BldBuf[BldIdx].Window, sizeof(AMBA_DSP_WINDOW_s));
    (void)dsp_osal_memcpy(&pCpyJobDesc->DstWin, &BldBuf[BldIdx].Window, sizeof(AMBA_DSP_WINDOW_s));
    pYuvStrmLayout->CopyJobNum[OverlapChanIdx]++;

    if ((pBufDesc->LastCpyIdx == DSP_EFCT_INVALID_IDX) ||
        (OverlapChanIdx > pBufDesc->LastCpyIdx)) {
        pBufDesc->LastCpyIdx = OverlapChanIdx;
    }
    if ((pBufDesc->FirstCpyIdx == DSP_EFCT_INVALID_IDX) ||
        (OverlapChanIdx < pBufDesc->FirstCpyIdx)) {
        pBufDesc->FirstCpyIdx = OverlapChanIdx;
    }
#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    DSP_ClearBit(&PassThruChanMask, OverlapChanIdx);
#endif

    pBldJobDesc->Src0BufIdx = pBufDesc->OutputBufId;
    pBldJobDesc->Src1BufIdx = pBufDesc->CpyBufId; //Always be copied buffer
    pBldJobDesc->DestBufIdx = pBufDesc->OutputBufId;
#ifdef DEBUG_EFCT_JOB_CALC
    AmbaLL_LogUInt5("[II]BldCpy[%u] 0x%X 0x%X 0x%X", BldIdx,
            pBldJobDesc->Src0BufIdx, pBldJobDesc->Src1BufIdx,
            pBldJobDesc->DestBufIdx, 0U);
#endif
    return Rval;
}

/*
 * Rotate is possibly implemented at two stage
 * (1) Before Warp(when Dram2Warp pipeline), 20181005, ucode not support yet
 *     - Vwarp table need to be consider
 *     - Need to make sure following setting are synced when Runtime update
 *       - BayerPattern
 *       - VprocRotateCmd
 *       - WarpTable
 *       - EffectCmd
 * (2) Extra Pass before Blend
 *     - An intermediated buffer is need to store Yuv before Blend
 *     - Use Dreg(DMA) for Blend input to execute input rotate
 *
 *  Blend directly of full area, need ExtraPass as well
 */
static inline UINT32 HL_CalcEffectPostPJobBldY2y(const CTX_RESOURCE_INFO_s *pResource,
                                                 DSP_EFFECT_BUF_DESC_s *pBufDesc,
                                                 DSP_EFFECT_BLEND_JOB_s *pBldJobDesc,
                                                 UINT8 *pFirstY2YBuf,
                                                 UINT32 *pEffectBufMask)
{
    UINT8 FirstY2YBuf = *pFirstY2YBuf;
#ifdef PPSTRM_INDPT_INT_BLD_BUFFER
    UINT8 AlwaysAllocIntBuffer = HL_GetDefaultRawEnable();
#endif
    UINT32 Rval = OK;

#ifdef PPSTRM_INDPT_INT_BLD_BUFFER
    if ((pBufDesc->Y2YBufId == DSP_EFCT_INVALID_IDX) ||
        (AlwaysAllocIntBuffer == 1U)) {
#else
    if (pBufDesc->Y2YBufId == DSP_EFCT_INVALID_IDX) {
#endif
        DSP_FindEmptyBit(pEffectBufMask,
                         pResource->EffectLogicBufNum,
                         &pBufDesc->Y2YBufId,
                         0U/*FindOnly*/,
                         EFFECT_BUF_MASK_DEPTH);
        if (pBufDesc->Y2YBufId == DSP_EFCT_INVALID_IDX) {
            AmbaLL_LogUInt5("  No Buf for Y2YOut 0x%X%X%X%X",
                            pEffectBufMask[3], pEffectBufMask[2],
                            pEffectBufMask[1], pEffectBufMask[0], 0U);
            Rval = DSP_ERR_0006;
        } else {
            pBldJobDesc->NeedY2YBuf = BLD_JOB_Y2Y_BUF_NEW_INT;
            FirstY2YBuf = 1U;
        }
    } else {
        pBldJobDesc->NeedY2YBuf = BLD_JOB_Y2Y_BUF_REUSE_INT;
        if (FirstY2YBuf == 0U) {
            FirstY2YBuf = 1U;
            pBldJobDesc->NeedY2YBuf = BLD_JOB_Y2Y_BUF_NEW_INT;
        } else {
            pBldJobDesc->NeedY2YBuf = BLD_JOB_Y2Y_BUF_REUSE_INT;
        }
    }

    pBldJobDesc->Src0BufIdx = pBufDesc->Y2YBufId;
    pBldJobDesc->Src1BufIdx = pBufDesc->OutputBufId;
    pBldJobDesc->DestBufIdx= pBufDesc->OutputBufId;
#ifdef DEBUG_EFCT_JOB_CALC
    AmbaLL_LogUInt5("[III]BldY2y 0x%X 0x%X 0x%X Y2Y[%d]",
            pBldJobDesc->Src0BufIdx, pBldJobDesc->Src1BufIdx,
            pBldJobDesc->DestBufIdx, pBldJobDesc->NeedY2YBuf, 0U);
#endif

    *pFirstY2YBuf = FirstY2YBuf;
    return Rval;
}

static inline UINT32 HL_CalcEffectPostPJobBld(const UINT16 ChanIdx,
                                              const CTX_RESOURCE_INFO_s *pResource,
                                              const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pEffChan,
                                              const AMBA_DSP_WINDOW_s *pOrigEffChanWin,
                                              CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                              DSP_EFFECT_BUF_DESC_s *pBufDesc,
                                              UINT8 *pFirstY2YBuf,
                                              UINT32 *pEffectBufMask)
{
    UINT16 ChanBIdx, BldIdx;
    UINT16 IsOverLap, OverlapNum;
    UINT32 Rval = OK, U32Val;
    AMBA_DSP_WINDOW_s OverLapWin;
    DSP_EFFECT_BLEND_JOB_s *pBldJobDesc;
    const AMBA_DSP_BUF_s *BldBuf;
    const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pChanB;

    dsp_osal_typecast(&BldBuf, &pEffChan->LumaAlphaTable);

    for (BldIdx=0; BldIdx < pEffChan->BlendNum; BldIdx++) {
        pBldJobDesc = &pYuvStrmLayout->BlendJobDesc[ChanIdx][BldIdx];

        for (ChanBIdx=0U; ChanBIdx<ChanIdx; ChanBIdx++) {
            pChanB = &pYuvStrmLayout->ChanCfg[ChanBIdx];

            /* Find the overlap with Blend ROI */
            IsOverLap = 0U;
            (void)HL_FindOverlapeRegion(&pChanB->Window,
                                        &BldBuf[BldIdx].Window,
                                        &OverLapWin,
                                        &IsOverLap);
            if (IsOverLap > 0U) {
                U32Val = (UINT32)pBldJobDesc->OverlapChan;
                DSP_SetBit(&U32Val, (UINT32)ChanBIdx);
                pBldJobDesc->OverlapChan = (UINT16)U32Val;
            }
        }

        DSP_Bit2Cnt((UINT32)pBldJobDesc->OverlapChan, &U32Val);
        OverlapNum = (UINT16)U32Val;
        pBldJobDesc->JobId = 0U; //TBD

#ifdef DEBUG_EFCT_JOB_CALC
        AmbaLL_LogUInt5("ChanIdx:%u OverlapNum %d Rot %d OverlapChan:%x",
                ChanIdx, OverlapNum, pEffChan->RotateFlip, pBldJobDesc->OverlapChan, 0U);
#endif
        if (HL_GetDefaultRawEnable() == 0U) {
            if ((OverlapNum == 1U) &&
                ((BldBuf[BldIdx].Window.Width < pEffChan->Window.Width) ||
                 (BldBuf[BldIdx].Window.Height < pEffChan->Window.Height))) {
    #ifdef DEBUG_EFCT_JOB_CALC
            AmbaLL_LogUInt5("ChanIdx %d BldIdx %d", ChanIdx, BldIdx, 0U, 0U, 0U);
    #endif
                Rval = HL_CalcEffectPostPJobBldCpy(BldIdx,
                                                   pResource,
                                                   BldBuf,
                                                   pYuvStrmLayout,
                                                   pBufDesc,
                                                   pBldJobDesc,
                                                   pEffectBufMask);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("HL_CalcEffectPostPJobBldCpy:%x ChanIdx:%u BldIdx:%u",
                            Rval, ChanIdx, BldIdx, 0U, 0U);
                }
            } else {
                Rval = HL_CalcEffectPostPJobBldY2y(pResource,
                                                   pBufDesc,
                                                   pBldJobDesc,
                                                   pFirstY2YBuf,
                                                   pEffectBufMask);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("HL_CalcEffectPostPJobBldY2y:%x ChanIdx:%u BldIdx:%u",
                            Rval, ChanIdx, BldIdx, 0U, 0U);
                }
            }
        } else {
            Rval = HL_CalcEffectPostPJobBldY2y(pResource,
                                               pBufDesc,
                                               pBldJobDesc,
                                               pFirstY2YBuf,
                                               pEffectBufMask);
            if (Rval != OK) {
                AmbaLL_LogUInt5("HL_CalcEffectPostPJobBldY2y:%x ChanIdx:%u BldIdx:%u",
                        Rval, ChanIdx, BldIdx, 0U, 0U);
            }
        }

        /*
           Extra Y2Y using full area instead of overlap area,
           Put temp Yuv from starting of temp buffer to prevent memory over-write
         */
        if (pBldJobDesc->NeedY2YBuf > 0U) {
            (void)dsp_osal_memcpy(&pBldJobDesc->Src0Win, pOrigEffChanWin, sizeof(AMBA_DSP_WINDOW_s));
            if ((pBldJobDesc->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT) ||
                (pBldJobDesc->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT)) {
                pBldJobDesc->Src0Win.OffsetX = 0U;
                pBldJobDesc->Src0Win.OffsetY = 0U;
            }
            (void)dsp_osal_memcpy(&pBldJobDesc->Src1Win, pOrigEffChanWin, sizeof(AMBA_DSP_WINDOW_s));
            (void)dsp_osal_memcpy(&pBldJobDesc->DestWin, &pEffChan->Window, sizeof(AMBA_DSP_WINDOW_s));
        } else {
            (void)dsp_osal_memcpy(&pBldJobDesc->Src0Win, &BldBuf[BldIdx].Window, sizeof(AMBA_DSP_WINDOW_s));
            (void)dsp_osal_memcpy(&pBldJobDesc->Src1Win, &BldBuf[BldIdx].Window, sizeof(AMBA_DSP_WINDOW_s));
            (void)dsp_osal_memcpy(&pBldJobDesc->DestWin, &BldBuf[BldIdx].Window, sizeof(AMBA_DSP_WINDOW_s));
        }

        pBldJobDesc->AlphaAddr = BldBuf[BldIdx].BaseAddr;
        pBldJobDesc->AlphaPitch = BldBuf[BldIdx].Pitch;
    }

    if ((pBufDesc->LastBldIdx == DSP_EFCT_INVALID_IDX) ||
        (ChanIdx > pBufDesc->LastBldIdx)) {
        pBufDesc->LastBldIdx = ChanIdx;
    }
    if ((pBufDesc->FirstBldIdx == DSP_EFCT_INVALID_IDX) ||
        (ChanIdx < pBufDesc->FirstBldIdx)) {
        pBufDesc->FirstBldIdx = ChanIdx;
    }
    if ((pBufDesc->FirstY2YBldIdx == DSP_EFCT_INVALID_IDX) ||
        (ChanIdx < pBufDesc->FirstY2YBldIdx)) {
        pBufDesc->FirstY2YBldIdx = ChanIdx;
    }
#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    DSP_ClearBit(&PassThruChanMask, ChanIdx);
#endif

    return Rval;
}

static inline UINT32 HL_CalcEffectPostPJobRotFlip(const UINT16 ChanIdx,
                                                 const CTX_RESOURCE_INFO_s *pResource,
                                                 const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pEffChan,
                                                 const AMBA_DSP_WINDOW_s *pOrigEffChanWin,
                                                 CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                                 DSP_EFFECT_BUF_DESC_s *pBufDesc,
                                                 UINT8 *pFirstY2YBuf,
                                                 UINT32 *pEffectBufMask)
{
    UINT8 FirstY2YBuf = *pFirstY2YBuf;
#ifdef PPSTRM_INDPT_INT_BLD_BUFFER
    UINT8 AlwaysAllocIntBuffer = HL_GetDefaultRawEnable();
#endif
    UINT16 ChanBIdx, IsOverLap;
    UINT32 Rval = OK, U32Val=0U;
    DSP_EFFECT_BLEND_JOB_s *pBldJobDesc;
#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
    UINT32 PassThruChanMask = 0U;
#endif

    // any rotation or flip needs an extra pass
    if (pYuvStrmLayout->ChanCfg[ChanIdx].RotateFlip != AMBA_DSP_ROTATE_0) {
        pBldJobDesc = &pYuvStrmLayout->BlendJobDesc[ChanIdx][0U];

        //rotate/flip also need to consider overlap
        for (ChanBIdx=0U; ChanBIdx<ChanIdx; ChanBIdx++) {
            const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pChanB;
            AMBA_DSP_WINDOW_s OverLapWin;

            pChanB = &pYuvStrmLayout->ChanCfg[ChanBIdx];

            /* Find the overlap with Blend ROI */
            IsOverLap = 0U;
            (void)HL_FindOverlapeRegion(&pChanB->Window,
                                        &pEffChan->Window,
                                        &OverLapWin,
                                        &IsOverLap);
            if (IsOverLap > 0U) {
                U32Val = (UINT32)pBldJobDesc->OverlapChan;
                DSP_SetBit(&U32Val, (UINT32)ChanBIdx);
                pBldJobDesc->OverlapChan = (UINT16)U32Val;
            }
#ifdef DEBUG_EFCT_JOB_CALC
            AmbaLL_LogUInt5("ChanIdx:%u IsOverLap %d Rot %d OverlapChan:%x",
                    ChanIdx, IsOverLap, pEffChan->RotateFlip, pBldJobDesc->OverlapChan, 0U);
#endif
        }
#ifdef PPSTRM_INDPT_INT_BLD_BUFFER
        if ((pBufDesc->Y2YBufId == DSP_EFCT_INVALID_IDX) ||
            (AlwaysAllocIntBuffer == 1U)) {
#else
        if (pBufDesc->Y2YBufId == DSP_EFCT_INVALID_IDX) {
#endif
            DSP_FindEmptyBit(pEffectBufMask,
                             pResource->EffectLogicBufNum,
                             &pBufDesc->Y2YBufId,
                             0U/*FindOnly*/,
                             EFFECT_BUF_MASK_DEPTH);
            if (pBufDesc->Y2YBufId == DSP_EFCT_INVALID_IDX) {
                AmbaLL_LogUInt5("  No Buf for Y2YOut 0x%X%X%X%X",
                                    pEffectBufMask[3], pEffectBufMask[2],
                                    pEffectBufMask[1], pEffectBufMask[0], 0U);
                Rval = DSP_ERR_0006;
            } else {
                pBldJobDesc->NeedY2YBuf = BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY;
                FirstY2YBuf = 1U;
            }
        } else {
            if (FirstY2YBuf == 0U) {
                FirstY2YBuf = 1U;
                pBldJobDesc->NeedY2YBuf = BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY;
            } else {
                pBldJobDesc->NeedY2YBuf = BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY;
            }
        }

        pBldJobDesc->Src0BufIdx = pBufDesc->Y2YBufId;
        pBldJobDesc->Src1BufIdx = pBufDesc->Y2YBufId;
        pBldJobDesc->DestBufIdx = pBufDesc->OutputBufId;
#ifdef DEBUG_EFCT_JOB_CALC
        AmbaLL_LogUInt5("[IV]BldJob[%d] 0x%X 0x%X 0x%X Y2Y[%d]",
                ChanIdx, pBldJobDesc->Src0BufIdx, pBldJobDesc->Src1BufIdx,
                pBldJobDesc->DestBufIdx, pBldJobDesc->NeedY2YBuf);
#endif
        /*
         * Extra Y2Y using full area instead of overlap area,
         * Put temp Yuv from starting of temp buffer to prevent memory over-write
         */
        (void)dsp_osal_memcpy(&pBldJobDesc->Src0Win, pOrigEffChanWin, sizeof(AMBA_DSP_WINDOW_s));
        (void)dsp_osal_memcpy(&pBldJobDesc->Src1Win, pOrigEffChanWin, sizeof(AMBA_DSP_WINDOW_s));
        if ((pBldJobDesc->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY) ||
            (pBldJobDesc->NeedY2YBuf == BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY)) {
            pBldJobDesc->Src0Win.OffsetX = 0U;
            pBldJobDesc->Src0Win.OffsetY = 0U;
            pBldJobDesc->Src1Win.OffsetX = 0U;
            pBldJobDesc->Src1Win.OffsetY = 0U;
        }
        (void)dsp_osal_memcpy(&pBldJobDesc->DestWin,
                              &pEffChan->Window,
                              sizeof(AMBA_DSP_WINDOW_s));

        pBldJobDesc->AlphaAddr = 0x0U; //any dummy address
        pBldJobDesc->AlphaPitch = pEffChan->Window.Width;

        if ((pBufDesc->LastBldIdx == DSP_EFCT_INVALID_IDX) ||
            (ChanIdx > pBufDesc->LastBldIdx)) {
            pBufDesc->LastBldIdx = ChanIdx;
        }
        if ((pBufDesc->FirstBldIdx == DSP_EFCT_INVALID_IDX) ||
            (ChanIdx < pBufDesc->FirstBldIdx)) {
            pBufDesc->FirstBldIdx = ChanIdx;
        }
        if ((pBufDesc->FirstY2YBldIdx == DSP_EFCT_INVALID_IDX) ||
            (ChanIdx < pBufDesc->FirstY2YBldIdx)) {
            pBufDesc->FirstY2YBldIdx = ChanIdx;
        }
#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
        DSP_ClearBit(&PassThruChanMask, ChanIdx);
#endif
    }

    *pFirstY2YBuf = FirstY2YBuf;
    return Rval;
}

/*
 * Currently only consider following case :
 *  - VR case with 2 Copy 2 Blend
 *  - AVM case with 4 copy 4 Blend
 *  - AVM case with 2 copy 2 Blend
 */
UINT32 HL_CalcEffectPostPJob(UINT16 YuvStrmIdx, CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout)
{
    UINT8 FirstY2YBuf = 0U;
    UINT16 ChanIdx;
    UINT32 Rval;
    UINT32 EffectBufMask[EFFECT_BUF_MASK_DEPTH];
    AMBA_DSP_WINDOW_s OrigEffChanWin = {0}; //BeforeRotate
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    DSP_EFFECT_BUF_DESC_s *pBufDesc;
    const DSP_EFFECT_BUF_DESC_s *pCurrentBufDesc;
    const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pEffChan;

    HL_GetResourcePtr(&Resource);
    (void)dsp_osal_memcpy(EffectBufMask,
                          &Resource->EffectLogicBufMask[0U],
                          sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);

    HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);
    pCurrentBufDesc = &YuvStrmInfo->Layout.EffectBufDesc;
    pBufDesc = &pYuvStrmLayout->EffectBufDesc;

    Rval = HL_CalcEffectPostPJobInit(Resource,
                                     pCurrentBufDesc,
                                     pBufDesc,
                                     pYuvStrmLayout,
                                     EffectBufMask);
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_CalcEffectPostPJobInit:%x", Rval, 0U, 0U, 0U, 0U);
    }

    if (Rval == OK) {
        for (ChanIdx=0U; ChanIdx<pYuvStrmLayout->NumChan; ChanIdx++) {
            pEffChan = &pYuvStrmLayout->ChanCfg[ChanIdx];

            if (HL_GET_ROTATE(pEffChan->RotateFlip) == DSP_ROTATE_90_DEGREE) {
                OrigEffChanWin.Width = pEffChan->Window.Height;
                OrigEffChanWin.Height = pEffChan->Window.Width;
                OrigEffChanWin.OffsetX = pEffChan->Window.OffsetX;
                OrigEffChanWin.OffsetY = pEffChan->Window.OffsetY;
            } else {
                (void)dsp_osal_memcpy(&OrigEffChanWin,
                                      &pEffChan->Window,
                                      sizeof(AMBA_DSP_WINDOW_s));
            }

            if (pEffChan->BlendNum > 0U) {
                Rval = HL_CalcEffectPostPJobBld(ChanIdx,
                                                Resource,
                                                pEffChan,
                                                &OrigEffChanWin,
                                                pYuvStrmLayout,
                                                pBufDesc,
                                                &FirstY2YBuf,
                                                EffectBufMask);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("HL_CalcEffectPostPJobBld:%x", Rval, 0U, 0U, 0U, 0U);
                }
            } else {
                Rval = HL_CalcEffectPostPJobRotFlip(ChanIdx,
                                                    Resource,
                                                    pEffChan,
                                                    &OrigEffChanWin,
                                                    pYuvStrmLayout,
                                                    pBufDesc,
                                                    &FirstY2YBuf,
                                                    EffectBufMask);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("HL_CalcEffectPostPJobRotFlip:%x", Rval, 0U, 0U, 0U, 0U);
                }
            }
        }
    }

    HL_GetResourceLock(&Resource);
    (void)dsp_osal_memcpy(&Resource->EffectLogicBufMask[0U],
                          &EffectBufMask[0U],
                          sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);
    HL_GetResourceUnLock();

    return Rval;
}

static inline UINT32 HL_EffPostPReBaseExtraOut(const UINT16 *pYuvIdxOrder,
                                               const UINT16 ReBaseChanId,
                                               const UINT8 EffectLogicBufNum,
                                               UINT32 *pEffectBufMask,
                                               DSP_EFFECT_BUF_DESC_s *pBufDesc,
                                               CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout)
{
    UINT8 TotalEff = 0U;
    UINT8 FirstY2YBuf = 1U;
    UINT16 CfgIdx, ChanId, ChanIdx;
    UINT16 OutBufId = DSP_EFCT_INVALID_IDX;
    UINT32 Rval = OK;
    DSP_EFFECT_BLEND_JOB_s *pBldJobDesc;
    const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pEffChan;

    pBldJobDesc = &pYuvStrmLayout->BlendJobDesc[ReBaseChanId][0U];
    for (ChanIdx=0U; ChanIdx<pYuvStrmLayout->NumChan; ChanIdx++) {
        if (ChanIdx == ReBaseChanId) {
            continue;
        }
        pEffChan = &pYuvStrmLayout->ChanCfg[ChanIdx];
        TotalEff += pEffChan->BlendNum;
        if (pEffChan->RotateFlip != AMBA_DSP_ROTATE_0) {
            TotalEff++;
        }
    }
    if (TotalEff > 1U) {
        DSP_FindEmptyBit(pEffectBufMask,
                         EffectLogicBufNum,
                         &OutBufId,
                         0U/*FindOnly*/,
                         EFFECT_BUF_MASK_DEPTH);
    } else {
        OutBufId = pBufDesc->Y2YBufId;
    }
    if (OutBufId == DSP_EFCT_INVALID_IDX) {
        AmbaLL_LogUInt5("  No Buf for Y2YOut 0x%X%X%X%X",
                pEffectBufMask[3], pEffectBufMask[2], pEffectBufMask[1], pEffectBufMask[0], 0U);
        Rval = DSP_ERR_0006;
    } else {
#ifdef DEBUG_EFCT_JOB_CALC
        AmbaLL_LogUInt5("HL_EffPostPReBaseExtraOut ChanId %d Y2YBufId %u->%u NeedY2YBuf:%u",
                ReBaseChanId, pBufDesc->Y2YBufId, OutBufId, pBldJobDesc->NeedY2YBuf, 0);
#endif
        //use pBufDesc->Y2YBufId
        if (pBldJobDesc->NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY) {
            pBldJobDesc->NeedY2YBuf = BLD_JOB_Y2Y_BUF_NEW_OUT_INT_DUMMY;
        } else {
            pBldJobDesc->NeedY2YBuf = BLD_JOB_Y2Y_BUF_NEW_OUT_INT;
        }
        pBldJobDesc->ReBaseOutBufIdx = pBufDesc->Y2YBufId;
        pBldJobDesc->Src0Win.OffsetX = 0U;
        pBldJobDesc->Src0Win.OffsetY = 0U;

        //change other chan using new Y2YBufId
        pBufDesc->Y2YBufId = OutBufId;
        for (CfgIdx=1U; CfgIdx<pYuvStrmLayout->NumChan; CfgIdx++) {
            UINT16 BldIdx, BldNum;

            ChanId = pYuvIdxOrder[CfgIdx];
            BldNum = pYuvStrmLayout->ChanCfg[ChanId].BlendNum;
            for (BldIdx=0U; BldIdx<BldNum; BldIdx++) {
                DSP_EFFECT_BLEND_JOB_s *pOtherBldJob = &pYuvStrmLayout->BlendJobDesc[ChanId][BldIdx];

                if (FirstY2YBuf == 1U) {
                    FirstY2YBuf = 0U;
#ifdef DEBUG_EFCT_JOB_CALC
                    AmbaLL_LogUInt5("HL_EffPostPReBaseExtraOut change0 CfgIdx:%u ChanId:%d BldIdx:%u Src0BufIdx:%u NeedY2YBuf:%u",
                            CfgIdx, ChanId, BldIdx, pOtherBldJob->Src0BufIdx, pOtherBldJob->NeedY2YBuf);
#endif
                    pOtherBldJob->NeedY2YBuf = BLD_JOB_Y2Y_BUF_NEW_INT;
                } else {
#ifdef DEBUG_EFCT_JOB_CALC
                    AmbaLL_LogUInt5("HL_EffPostPReBaseExtraOut change1 CfgIdx:%u ChanId:%d BldIdx:%u Src0BufIdx:%u NeedY2YBuf:%u",
                            CfgIdx, ChanId, BldIdx, pOtherBldJob->Src0BufIdx, pOtherBldJob->NeedY2YBuf);
#endif
                    pOtherBldJob->NeedY2YBuf = BLD_JOB_Y2Y_BUF_REUSE_INT;
                }
                pOtherBldJob->Src0BufIdx = OutBufId;
            }
        }
    }
    return Rval;
}

#ifdef PPSTRM_REBASE_CHK_OVERLAP
static inline UINT32 HL_EffPostPReBaseBld(const UINT16 *pYuvIdxOrder,
                                          const UINT16 ReBaseChanId,
                                          CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                          AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pEffChan)
{
    UINT16 CfgIdx, ChanId, BldIdx;
    UINT16 BldNum = pEffChan->BlendNum;
    UINT32 Rval = OK;
    DSP_EFFECT_BLEND_JOB_s *pBldJobDesc;

    //change ReBaseChanId NeedY2YBuf
    for (BldIdx=0U; BldIdx<BldNum; BldIdx++) {
        pBldJobDesc = &pYuvStrmLayout->BlendJobDesc[ReBaseChanId][BldIdx];
#ifdef DEBUG_EFCT_JOB_CALC
        AmbaLL_LogUInt5("HL_EffPostPReBaseBld ReBaseChanId:%u BldIdx:%d NeedY2YBuf %u->%u",
                ReBaseChanId, BldIdx, pBldJobDesc->NeedY2YBuf, BLD_JOB_Y2Y_BUF_NEW_INT, 0U);
#endif
        pBldJobDesc->NeedY2YBuf = BLD_JOB_Y2Y_BUF_NEW_INT;
    }

    //check other chan NeedY2YBuf
    for (CfgIdx=1U; CfgIdx<pYuvStrmLayout->NumChan; CfgIdx++) {
        UINT8 FirstY2YBuf = 1U;

        ChanId = pYuvIdxOrder[CfgIdx];
        BldNum = pYuvStrmLayout->ChanCfg[ChanId].BlendNum;
        for (BldIdx=0U; BldIdx<BldNum; BldIdx++) {
            DSP_EFFECT_BLEND_JOB_s *pOtherBldJob = &pYuvStrmLayout->BlendJobDesc[ChanId][BldIdx];

            if (FirstY2YBuf == 1U) {
                FirstY2YBuf = 0U;
#ifdef DEBUG_EFCT_JOB_CALC
                AmbaLL_LogUInt5("HL_EffPostPReBaseBld change0 CfgIdx:%u ChanId:%d BldIdx:%u Src0BufIdx:%u NeedY2YBuf:%u",
                        CfgIdx, ChanId, BldIdx, pOtherBldJob->Src0BufIdx, pOtherBldJob->NeedY2YBuf);
#endif
                pOtherBldJob->NeedY2YBuf = BLD_JOB_Y2Y_BUF_NEW_INT;
            } else {
#ifdef DEBUG_EFCT_JOB_CALC
                AmbaLL_LogUInt5("HL_EffPostPReBaseBld change1 CfgIdx:%u ChanId:%d BldIdx:%u Src0BufIdx:%u NeedY2YBuf:%u",
                        CfgIdx, ChanId, BldIdx, pOtherBldJob->Src0BufIdx, pOtherBldJob->NeedY2YBuf);
#endif
                pOtherBldJob->NeedY2YBuf = BLD_JOB_Y2Y_BUF_REUSE_INT;
            }
        }
    }

    return Rval;
}
#endif

UINT32 HL_EffPostPReBase(UINT16 YuvStrmIdx,
                         const UINT16 *pYuvIdxOrder,
                         CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout)
{
    UINT16 CfgIdx, ReBaseChanId;
    UINT32 Rval = OK;
    UINT32 EffectBufMask[EFFECT_BUF_MASK_DEPTH];
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    DSP_EFFECT_BUF_DESC_s *pBufDesc;
    const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pEffChan;

    HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);
    pBufDesc = &pYuvStrmLayout->EffectBufDesc;

    CfgIdx = 0U;
    ReBaseChanId = pYuvIdxOrder[CfgIdx];
    pEffChan = &pYuvStrmLayout->ChanCfg[ReBaseChanId];
#ifdef DEBUG_EFCT_JOB_CALC
    AmbaLL_LogUInt5("HL_EffPostPReBase CfgIdx:%u ChanId %d Rot %d BlendNum:%u",
            CfgIdx, ReBaseChanId, pEffChan->RotateFlip, pEffChan->BlendNum, 0U);
#endif

    HL_GetResourcePtr(&Resource);
    (void)dsp_osal_memcpy(EffectBufMask,
                          &Resource->EffectLogicBufMask[0U],
                          sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);
#ifdef PPSTRM_REBASE_CHK_OVERLAP
    //check chan0 overlap
    for (CfgIdx=1U; CfgIdx<pYuvStrmLayout->NumChan; CfgIdx++) {
        ChanId = pYuvIdxOrder[CfgIdx];
        (void)HL_FindOverlapeRegion(&pYuvStrmLayout->ChanCfg[ReBaseChanId].Window,
                                    &pYuvStrmLayout->ChanCfg[ChanId].Window,
                                    &OverLapWin,
                                    &IsOverLap);
        if (IsOverLap > 0U) {
            AmbaLL_LogUInt5("HL_EffPostPReBase OverLap ChanId %d OverLapWin: %u %u %u %u",
                    ChanId,
                    OverLapWin.OffsetX, OverLapWin.OffsetY,
                    OverLapWin.Width, OverLapWin.Height);
        }
    }

    if (IsOverLap > 0U) {   //output to new y2y buffer
        Rval = HL_EffPostPReBaseExtraOut(pYuvIdxOrder,
                                         ReBaseChanId,
                                         Resource->EffectLogicBufNum,
                                         EffectBufMask,
                                         pBufDesc,
                                         pYuvStrmLayout);
    } else if (pEffChan->BlendNum > 0U) { //if no overlap, but has rot/flip
        Rval = HL_EffPostPReBaseBld(pYuvIdxOrder,
                                    ReBaseChanId,
                                    pYuvStrmLayout,
                                    pEffChan);

    } else if (pEffChan->RotateFlip != AMBA_DSP_ROTATE_0) {   //if no overlap, but has bld job, erase it
        //TBD
    } else {
        //direct out, do nothing
    }
#endif

    if ((pEffChan->BlendNum > 0U) ||
        (pEffChan->RotateFlip != AMBA_DSP_ROTATE_0)) {   //output to new y2y buffer
        Rval = HL_EffPostPReBaseExtraOut(pYuvIdxOrder,
                                         ReBaseChanId,
                                         Resource->EffectLogicBufNum,
                                         EffectBufMask,
                                         pBufDesc,
                                         pYuvStrmLayout);
    } else {
        //direct out, do nothing
    }

    if (Rval == OK) {
        HL_GetResourceLock(&Resource);
        (void)dsp_osal_memcpy(&Resource->EffectLogicBufMask[0U],
                              &EffectBufMask[0U],
                              sizeof(UINT32)*EFFECT_BUF_MASK_DEPTH);
        HL_GetResourceUnLock();
    }

    return Rval;
}

UINT32 HL_CategorizeEncodeResolution(UINT16 Width, UINT16 Height, UINT8 *StrmResol)
{
    UINT32 Rval = OK;
    UINT32 TotalPixel = (UINT32)Width*Height;

    if (StrmResol == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        /**
         * Per ShiHao Resol will use to decide Dram and Smem usage
         *  - Dram : use total pixel
         *  - Smem : use Width
         * hence, make sure both conditions met
         */
        if ((TotalPixel > (4096U*2160U)) || (Width > 4096U)) {
            *StrmResol = DSP_ENC_MAX_RESOL_USER_DEFINED;
        } else if ((TotalPixel > (3840U*2160U)) || (Width > 3840U)) {
            *StrmResol = DSP_ENC_MAX_RESOL_4096;
        } else if ((TotalPixel > (1920U*1080U)) || (Width > 1920U)) {
            *StrmResol = DSP_ENC_MAX_RESOL_3840;
        } else if ((TotalPixel > (1280U*720U)) || (Width > 1280U)) {
            *StrmResol = DSP_ENC_MAX_RESOL_1920;
        } else {
            *StrmResol = DSP_ENC_MAX_RESOL_1280;
        }
    }

    return Rval;
}

UINT32 HL_ComposeEfctSyncJobId(DSP_EFCT_SYNC_JOB_ID_s *JobId, UINT16 YuvStrmIdx, UINT16 SeqIndx, UINT16 TypeBit, UINT16 SubJobIdx)
{
    UINT32 Rval = OK;

    if (JobId == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (YuvStrmIdx != EFCT_SYNC_JOB_ID_DONTCARE) {
            JobId->YuvStrmId = (UINT8)YuvStrmIdx;
        }
        if (SeqIndx != EFCT_SYNC_JOB_ID_DONTCARE) {
            JobId->SeqIdx = (UINT8)SeqIndx;
        }
        if (TypeBit != EFCT_SYNC_JOB_ID_DONTCARE) {
            JobId->JobTypeBit = (UINT8)TypeBit;
        }
        if (SubJobIdx != EFCT_SYNC_JOB_ID_DONTCARE) {
            JobId->SubJobIdx = (UINT8)SubJobIdx;
        }
#if 0
        {
            UINT32 U32Val;
            (void)dsp_osal_memcpy(&U32Val , JobId, sizeof(UINT32));
            AmbaLL_LogUInt5("  Compose SyncJobId 0x%X", U32Val, 0U, 0U, 0U, 0U);
        }
#endif
    }

    return Rval;
}

UINT32 HL_IsVirtualVinIdx(UINT16 VinId, UINT8 *IsVirtVin)
{
    UINT32 Rval = OK;

    if (VinId == DSP_VIRT_VIN_IDX_INVALID) {
        *IsVirtVin= (UINT8)0U;
    } else {
        if (VinId >= DSP_VIN_MAX_NUM) {
            *IsVirtVin= (UINT8)0U;
            Rval = DSP_ERR_0001;
        } else {
            if (VinId >= AMBA_DSP_MAX_VIN_NUM) {
                *IsVirtVin = (UINT8)1U;
            } else {
                *IsVirtVin = (UINT8)0U;
            }
        }
    }

    return Rval;
}

UINT32 HL_GetVirtualVinNum(UINT16 *Num)
{
    UINT32 Rval = OK, Cnt = 0U;
    CTX_DSP_INST_INFO_s DspInstInfo = {0};

    /*
     * Following features may need VirtualVin
     * 1) PIV Raw2Yuv
     * 2) PIV Yuv2Yuv
     * 3) Yuv2Jpg, Yuv2Hevc, Yuv2Avc
     * 4) RawEncode regression
     * 5) AVM to have transparent chassis
     * 6) VOUT Pip with arbitrary YuvInput
     * 7) Vin TimeDivision
     * 8) dec to vproc
     */

    HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);
    DSP_Bit2Cnt(DspInstInfo.VirtVinBitMask, &Cnt);
    *Num = (UINT16)Cnt;
    return Rval;
}

UINT32 HL_GetTimeDivisionVirtVinInfo(UINT16 VinId, UINT16 *PhysicalVin)
{
    UINT8 IsVirtVin = 0U, ExitILoop;
    UINT16 i, j, VinExist;
    UINT32 Rval = OK;
    CTX_VIN_INFO_s VinInfo = {0};

    (void)HL_IsVirtualVinIdx(VinId, &IsVirtVin);
    if (IsVirtVin > 0U) {
        ExitILoop = 0U;
        for (i = 0; i < AMBA_DSP_MAX_VIN_NUM; i++) {
            VinExist = 0U;
            HL_GetVinExistence(i, &VinExist);
            if (VinExist > 0U) {
                HL_GetVinInfo(HL_MTX_OPT_ALL, i, &VinInfo);
                if (VinInfo.TimeDivisionNum[0U] > 1U) {
                    for (j = 1U; j < VinInfo.TimeDivisionNum[0U]; j++) {
                        if (VinId == (VinInfo.TimeDivisionVinId[0U][j] + AMBA_DSP_MAX_VIN_NUM)) {
                            ExitILoop = 1U;
                            *PhysicalVin = i;
                            break;
                        }
                    }
                }
            }
            if (ExitILoop == 1U) {
                break;
            }
        }
    } else {
        *PhysicalVin = DSP_VIRT_VIN_IDX_INVALID;
    }

    return Rval;
}

UINT32 HL_GetVprocNum(UINT16 *Num)
{
    UINT32 Rval = OK, Cnt = 0U;
    CTX_DSP_INST_INFO_s DspInstInfo = {0};

    HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);
    DSP_Bit2Cnt(DspInstInfo.VprocBitMask, &Cnt);
    *Num = (UINT16)Cnt;
    return Rval;
}

UINT32 HL_GetDramMipiYuvEnable(void)
{
    UINT32 Enable = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i;

    HL_GetResourcePtr(&Resource);
    for (i=0U; i<Resource->MaxViewZoneNum; i++) {
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);

        if ((ViewZoneInfo->Pipe == (UINT8)DSP_DRAM_PIPE_MIPI_YUV) ||
            (ViewZoneInfo->Pipe == (UINT8)DSP_DRAM_PIPE_MIPI_RAW)) {
            Enable = 1U;
            break;
        }
    }

    return Enable;
}

UINT32 HL_IsSliceMode(UINT8 *IsSliceMode)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i;
    UINT8 BitMask;

    HL_GetResourcePtr(&Resource);

    BitMask = *IsSliceMode;
    for (i=0U; i<Resource->MaxViewZoneNum; i++) {
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);

        if (ViewZoneInfo->SliceNumCol > (UINT8)1U) {
            DSP_SetU8Bit(&BitMask, SLICE_MODE_HOR_IDX);
        }
        if (ViewZoneInfo->SliceNumRow > (UINT8)1U) {
            DSP_SetU8Bit(&BitMask, SLICE_MODE_VER_IDX);
        }
    }
    *IsSliceMode = BitMask;

    return Rval;
}

UINT32 HL_IsTileMode(UINT8 *IsTileMode)
{
    UINT8 IsSliceMode = (UINT8)0U;
    UINT16 MaxVinWidth = 0U, MaxVinHeight = 0U;
    UINT16 MaxMainWidth = 0U, MaxMainHeight = 0U;
    UINT32 Rval = OK;

    (void)HL_IsSliceMode(&IsSliceMode);
    (void)HL_GetSystemVinMaxWindow(&MaxVinWidth, &MaxVinHeight);
    (void)HL_GetSystemVprocPinMaxWindow(DSP_VPROC_PIN_MAIN, &MaxMainWidth, &MaxMainHeight);

    // Simple Rule of tile : Vin > 1920 or MainW > 1920
    if ((MaxVinWidth > SEC2_MAX_IN_WIDTH) ||
        (MaxMainWidth > SEC2_MAX_OUT_WIDTH) ||
        (IsSliceMode > (UINT8)0U)) {
        *IsTileMode = 1U;
    } else {
        *IsTileMode = 0U;
    }

    return Rval;
}

static UINT32 HL_CalcVideoTileHeight(UINT16 Height, UINT16 TileNum, UINT16 *TileHeight)
{
    UINT32 Rval = OK;
    if (Height == 0U) {
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("CalcVdoTileH H %d", Height, 0U, 0U, 0U, 0U);
#endif
        *TileHeight = 0U;
        Rval = DSP_ERR_0001;
    } else if (TileNum == 0U) {
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("CalcVdoTileH T %d", TileNum, 0U, 0U, 0U, 0U);
#endif
        *TileHeight = 0U;
        Rval = DSP_ERR_0001;
    } else {
        *TileHeight = (UINT16)(((Height + TileNum) - 1U)/TileNum);
    }

    return Rval;
}

UINT32 HL_FillSliceCapLine(UINT16 ViewZoneId, UINT16 *pCapLineCfg, UINT16 IntcNumMinusOne)
{
    UINT32 Rval = OK;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i, SliceHeight = 0U;
    UINT16 ExtraCapLine;
    UINT16 Factor = IntcNumMinusOne + 1U;

    if (pCapLineCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

        ExtraCapLine = (ViewZoneInfo->VinDragLine > 0U)? ViewZoneInfo->VinDragLine: CV2X_EXTRA_SLICE_CAP_LINE;

//FIXME, YUYV case
        (void)HL_CalcVideoTileHeight(ViewZoneInfo->CapWindow.Height, ViewZoneInfo->SliceNumRow, &SliceHeight);
        if (ViewZoneInfo->HdrBlendNumMinusOne > 0U) {
            UINT16 HdrOffset, HdrFrmNum;

            HdrOffset = (UINT16)ViewZoneInfo->HdrRawYOffset[ViewZoneInfo->HdrBlendNumMinusOne];
            HdrFrmNum = (UINT16)ViewZoneInfo->HdrBlendNumMinusOne + 1U;
            for (i = 0U; i < ViewZoneInfo->SliceNumRow; i++) {
                if (i == ((UINT16)ViewZoneInfo->SliceNumRow - 1U)) {
                    pCapLineCfg[i] = HdrOffset;
                    pCapLineCfg[i] += ((ViewZoneInfo->CapWindow.OffsetY + ViewZoneInfo->CapWindow.Height)*HdrFrmNum);
                    pCapLineCfg[i] -= 1U;
                } else {
                    pCapLineCfg[i] = HdrOffset;
                    pCapLineCfg[i] += (((ViewZoneInfo->CapWindow.OffsetY + (SliceHeight*(i+1U))) + ExtraCapLine)*HdrFrmNum);
                    pCapLineCfg[i] -= 1U;
                }
                pCapLineCfg[i] *= Factor;
            }
        } else {
            for (i = 0U; i < ViewZoneInfo->SliceNumRow; i++) {
                if (i == ((UINT16)ViewZoneInfo->SliceNumRow - 1U)) {
                    pCapLineCfg[i] = ViewZoneInfo->CapWindow.OffsetY + ViewZoneInfo->CapWindow.Height;
                } else {
                    pCapLineCfg[i] = (ViewZoneInfo->CapWindow.OffsetY + (SliceHeight*(i+1U))) + ExtraCapLine;
                }
                pCapLineCfg[i] *= Factor;
            }
        }
    }

    return Rval;
}

UINT32 HL_FillSliceLayoutCfg(UINT16 ColNum, UINT16 RowNum, UINT16 Width, UINT16 Height, DSP_SLICE_LAYOUT_s *pSliceLayout)
{
    UINT32 Rval = OK;
    UINT16 i, j, LayoutIdx;

    if ((ColNum == 0U) ||
        (RowNum == 0U) ||
        (Width == 0U) ||
        (Height == 0U)) {
        Rval = DSP_ERR_0001;
    } else if (pSliceLayout == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        for (j=0U; j<RowNum; j++) {
            for (i=0U; i<ColNum; i++) {
                LayoutIdx = (j*ColNum) + i;

                pSliceLayout[LayoutIdx].TileColIdx = (UINT8)i;
                pSliceLayout[LayoutIdx].TileRowIdx = (UINT8)j;
                //FIXME consider overlap
                (void)HL_CalcVideoTileWidth(Width,
                                            ColNum,
                                            &pSliceLayout[LayoutIdx].TileColWidth);
                (void)HL_CalcVideoTileHeight(Height, RowNum, &pSliceLayout[LayoutIdx].TileRowHeight);
                pSliceLayout[LayoutIdx].TileRowHeight = ALIGN_NUM16(pSliceLayout[LayoutIdx].TileRowHeight, 16U);

                pSliceLayout[LayoutIdx].TileColStart = i*pSliceLayout[LayoutIdx].TileColWidth;
                pSliceLayout[LayoutIdx].TileRowStart = j*pSliceLayout[LayoutIdx].TileRowHeight;

                if (i == (ColNum - 1U)) {
                    pSliceLayout[LayoutIdx].TileColWidth = Width - pSliceLayout[LayoutIdx].TileColStart;
                }

                if (j == (RowNum - 1U)) {
                    pSliceLayout[LayoutIdx].TileRowHeight = Height - pSliceLayout[LayoutIdx].TileRowStart;
                }

                //AmbaLL_LogUInt5("SliceLayout[%d][%d] LayoutIdx[%d]", i, j, LayoutIdx, 0U, 0U);
                //AmbaLL_LogUInt5("     Idx[%d][%d]", pSliceLayout[LayoutIdx].TileColIdx, pSliceLayout[LayoutIdx].TileRowIdx,
                //                                    0U, 0U, 0U);
                //AmbaLL_LogUInt5("     Win[%d %d] Offset[%d %d]", pSliceLayout[LayoutIdx].TileColWidth, pSliceLayout[LayoutIdx].TileRowHeight,
                //                                    pSliceLayout[LayoutIdx].TileColStart, pSliceLayout[LayoutIdx].TileRowStart, 0U);

            }
        }
    }

    return Rval;
}


UINT32 HL_GetReconPostStatus(UINT16 StrmIdx, UINT16 *Status)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i, Idx;

    HL_GetResourcePtr(&Resource);

    for (i=0U; i<Resource->MaxViewZoneNum; i++) {
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) {
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &Idx);
            if (StrmIdx == Idx) {
                DSP_SetU16Bit(Status, RECON_POST_2_VPROC_BIT);
                break;
            }
        }
    }

    return Rval;
}

UINT32 HL_ComposeStlProcJobId(DSP_STL_PROC_JOB_ID_s *JobId, UINT16 DatFmt, UINT16 OutputPinMask, UINT16 IsExtMem, UINT16 VprocId)
{
    UINT32 Rval = OK;

    if (JobId == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (DatFmt != STL_PROC_JOB_ID_DONTCARE) {
            JobId->DataFmt = (UINT8)DatFmt;
        }
        if (OutputPinMask != STL_PROC_JOB_ID_DONTCARE) {
            JobId->OutputPin = (UINT8)OutputPinMask;
        }
        if (IsExtMem != STL_PROC_JOB_ID_DONTCARE) {
            JobId->ExtMem = (UINT8)IsExtMem;
        }
        if (VprocId != STL_PROC_JOB_ID_DONTCARE) {
            JobId->VprocId = (UINT8)VprocId;
        }
#if 0
        {
            UINT32 U32Val;
            (void)dsp_osal_memcpy(&U32Val , JobId, sizeof(UINT32));
            AmbaLL_LogUInt5("  Compose StlProcJobId 0x%X", U32Val, 0U, 0U, 0U, 0U);
        }
#endif
    }

    return Rval;
}


UINT32 HL_GetEncodePjpegDramSize(UINT16 MaxEncWidth, UINT16 MaxEncHeight)
{
#define PJPEG_MB_THRESHOLD  (28800U)
#define MB_PIXEL_NUM        (256U)

    UINT32 Size;

    Size = ((UINT32)MaxEncWidth*MaxEncHeight)/MB_PIXEL_NUM;
    Size = (Size > PJPEG_MB_THRESHOLD)? ((UINT32)12U << 20U): ((UINT32)6U << 20U);
    return Size;
}

UINT32 HL_GetSystemVinMaxViewZoneNum(UINT16 VinId)
{
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    UINT16 i, ViewZoneVinId;
    UINT32 ViewZonNum = 0;
    UINT16 DecId;

    HL_GetResourcePtr(&Resource);

    for (i = 0U; i < Resource->MaxViewZoneNum; i++) {
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);

        if (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_RAW_ONLY) {
            continue;
        }

        if ((ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_DEC) &&
            (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_RECON)) {
            HL_GetViewZoneVinId(i, &ViewZoneVinId);
            if (ViewZoneVinId == VinId) {
                ViewZonNum++;
            }
        } else if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &DecId);
            HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecId, &VidDecInfo);
            if ((VidDecInfo.YuvInVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
                (VidDecInfo.YuvInVirtVinId == (VinId - AMBA_DSP_MAX_VIN_NUM))) {
            #ifdef DUPLEX_DEC_SHARE_VIRT_VIN
                ViewZonNum++;
            #else
                ViewZonNum = 1U;
                break;
            #endif
            }
        } else {
            // DO NOTHING
        }
    }

    return ViewZonNum;
}

void HL_GetVinExistence(UINT16 VinId, UINT16 *Exist)
{
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_DSP_INST_INFO_s DspInstInfo = {0};

    HL_GetResourcePtr(&Resource);
    HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);

    *Exist = 0U;
    if (VinId < AMBA_DSP_MAX_VIN_NUM) {
        if (1U == DSP_GetU16Bit(Resource->MaxVinBit, VinId, 1U)) {
            *Exist = 1U;
        }
    } else {
        if (1U == DSP_GetBit(DspInstInfo.VirtVinBitMask, ((UINT32)VinId - AMBA_DSP_MAX_VIN_NUM), 1U)) {
            *Exist = 1U;
        }
    }
}

UINT32 HL_GetSystemVinMaxWindow(UINT16 *Width, UINT16 *Height)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_DSP_INST_INFO_s DspInstInfo = {0};
    UINT16 i = 0U, VirtVinId;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT32 RawCapOnlyVinBit = 0U;
    UINT16 ViewZoneVinId;

    if ((Width == NULL) || (Height == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);
        HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);

        for (i = 0U; i < Resource->ViewZoneNum; i++) {
            HL_GetViewZoneInfoPtr(i, &pViewZoneInfo);
            if (pViewZoneInfo->Pipe == DSP_DRAM_PIPE_RAW_ONLY) {
                ViewZoneVinId = 0xFFFFU;
                (void)HL_GetViewZoneVinId(i, &ViewZoneVinId);
                if (ViewZoneVinId != 0xFFFFU) {
                    DSP_SetBit(&RawCapOnlyVinBit, ViewZoneVinId);
                }
            }
        }

        for (i = 0U; i < DSP_VIN_MAX_NUM; i++) {
            if (i < AMBA_DSP_MAX_VIN_NUM) {
                if (1U == DSP_GetU16Bit(Resource->MaxVinBit, i, 1U)) {
                    if (1U == DSP_GetBit(RawCapOnlyVinBit, i, 1U)) {
                        *Width = CV2X_VPROC_DUMMY_SIZE;
                        *Height = CV2X_VPROC_DUMMY_SIZE;
                    } else {
                        *Width = MAX2_16(*Width, Resource->MaxVinVirtChanWidth[i][0U]);
                        *Height = MAX2_16(*Height, Resource->MaxVinVirtChanHeight[i][0U]);
                        if (Resource->MaxProcessFormat > 0U) {
                            *Width = MAX2_16(*Width, Resource->MaxStlVinWidth[i][0U]);
                            *Height = MAX2_16(*Height, Resource->MaxStlVinHeight[i][0U]);
                        }
                    }
                }
            } else {
                if (Resource->MaxProcessFormat > 0U) {
                    *Width = MAX2_16(*Width, Resource->MaxStlRawInputWidth);
                    *Width = MAX2_16(*Width, Resource->MaxStlYuvInputWidth);

                    *Height = MAX2_16(*Height, Resource->MaxStlRawInputHeight);
                    *Height = MAX2_16(*Height, Resource->MaxStlYuvInputHeight);
                }

                VirtVinId = i - AMBA_DSP_MAX_VIN_NUM;
                if (1U == DSP_GetBit(DspInstInfo.VirtVinBitMask, (UINT32)VirtVinId, 1U)) {
                    *Width = MAX2_16(*Width, Resource->MaxVirtVinWidth[i - AMBA_DSP_MAX_VIN_NUM]);
                    *Height = MAX2_16(*Height, Resource->MaxVirtVinHeight[i - AMBA_DSP_MAX_VIN_NUM]);
                }
            }
        }
    }

    return Rval;
}

UINT32 HL_GetSystemVprocPinMaxWindow(UINT8 PinType, UINT16 *Width, UINT16 *Height)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT16 i = 0U;
    UINT16 PinOutWidth = 0U, PinOutHeight = 0U;

    if ((Width == NULL) || (Height == NULL)) {
        Rval = DSP_ERR_0000;
    } else if (PinType > DSP_VPROC_PIN_MAIN) {
        Rval = DSP_ERR_0001;
    } else {
        HL_GetResourcePtr(&Resource);

        for (i=0U; i<Resource->MaxViewZoneNum; i++) {
            Rval = HL_GetViewZoneVprocPinMaxWin(i, PinType, &PinOutWidth, &PinOutHeight);
            *Width = MAX2_16(*Width, PinOutWidth);
            *Height = MAX2_16(*Height, PinOutHeight);
        }

        /* Consider StlProc */
        if ((Resource->MaxProcessFormat > 0U) &&
            ((PinType == (UINT8)DSP_VPROC_PIN_PREVB) ||
             (PinType == (UINT8)DSP_VPROC_PIN_PREVA) ||
             (PinType == (UINT8)DSP_VPROC_PIN_MAIN))) {
            *Width = MAX2_16(*Width, Resource->MaxStlMainWidth);
            *Height = MAX2_16(*Height, Resource->MaxStlMainHeight);
        }
    }

    return Rval;
}

UINT32 HL_GetViewZoneVprocPinMaxWin(UINT16 ViewZoneId, UINT16 VprocPin, UINT16 *Width, UINT16 *Height)
{
    UINT32 Rval = OK;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    UINT16 YuvStrmIdx = 0U;
    UINT8 IsEfctYuvStrm = 0U;
    UINT8 IsEncPurpose = 0U;

    HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);
    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

    if (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_RAW_ONLY) {
        *Width = CV2X_VPROC_DUMMY_SIZE;
        *Height = CV2X_VPROC_DUMMY_SIZE;
    } else {
        if (VprocInfo.PinUsage[VprocPin] > 0U) {
            //Use First Linked YuvStrm
            DSP_Bit2U16Idx(VprocInfo.PinUsage[VprocPin], &YuvStrmIdx);

            HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);
            IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(YuvStrmIdx))? (UINT8)1U: (UINT8)0U;
            IsEncPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U))? 1U: 0U;
        }

        /* Check this Pin is linked to EffectStrm or not */
        if (IsEfctYuvStrm > 0U) {
    //FIXME, calc union window
            if (VprocPin == DSP_VPROC_PIN_MAIN) {
                *Width = ViewZoneInfo->Main.Width;
                *Height = ViewZoneInfo->Main.Height;
            } else {
                HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);
                *Width = MAX2_16(YuvStrmInfo->MaxWidth, ViewZoneInfo->PinMaxWindow[VprocPin].Width);
                *Height = MAX2_16(YuvStrmInfo->MaxHeight, ViewZoneInfo->PinMaxWindow[VprocPin].Height);
            }
        } else {
            if (VprocPin == DSP_VPROC_PIN_MAIN) {
                if (VprocInfo.PinUsage[VprocPin] > 0U) {
                    HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);
                    *Width = MAX2_16(ViewZoneInfo->Main.Width, YuvStrmInfo->MaxWidth);
                    *Height = MAX2_16(ViewZoneInfo->Main.Height, YuvStrmInfo->MaxHeight);
                } else {
                    *Width = ViewZoneInfo->Main.Width;
                    *Height = ViewZoneInfo->Main.Height;
                }
            } else {
                *Width = ViewZoneInfo->PinMaxWindow[VprocPin].Width;
                *Height = ViewZoneInfo->PinMaxWindow[VprocPin].Height;
            }
        }

        /* AVC need 16 alignment */
        if (IsEncPurpose == 1U) {
            *Height = ALIGN_NUM16(*Height, 16U);
        }
    }

    return Rval;
}

UINT32 HL_GetVprocPinMaxWidth(UINT16 PrevIdx, const AMBA_DSP_WINDOW_s *Input, const AMBA_DSP_WINDOW_s *Output, UINT16 *MaxWidth)
{
    static const UINT16 HL_MaxVprocPinOutputWidthMap[DSP_VPROC_PIN_NUM] = {
            [DSP_VPROC_PIN_PREVC] = PREVC_MAX_WIDTH,
            [DSP_VPROC_PIN_PREVA] = PREVA_MAX_WIDTH,
            [DSP_VPROC_PIN_PREVB] = PREVB_MAX_WIDTH,
            [DSP_VPROC_PIN_MAIN]  = MAIN_MAX_WIDTH,
    };
    static const UINT16 HL_MaxVprocPinOutput2XWidthMap[DSP_VPROC_PIN_NUM] = {
            [DSP_VPROC_PIN_PREVC] = PREVC_2X_MAX_WIDTH,
            [DSP_VPROC_PIN_PREVA] = PREVA_2X_MAX_WIDTH,
            [DSP_VPROC_PIN_PREVB] = PREVB_2X_MAX_WIDTH,
            [DSP_VPROC_PIN_MAIN]  = MAIN_MAX_WIDTH,
    };
    UINT32 Rval = OK;
    UINT8 IsExtra2x = 0U;
    UINT16 HorRatio, VerRatio;

    if (PrevIdx != DSP_VPROC_PIN_MAIN) {
        if (Input->Width > Output->Width) {
            HorRatio = Input->Width/Output->Width;
        } else {
            HorRatio = Output->Width/Input->Width;
        }

        if (Input->Height > Output->Height) {
            VerRatio = Input->Height/Output->Height;
        } else {
            VerRatio = Output->Height/Input->Height;
        }

#ifdef SUPPORT_VOUT_16XDS_TRUNCATE_TRICK
        /* Only support 2 pixel truncate */
        if ((HL_MaxVprocPinOutputRatioMap[PrevIdx] == PREV_MAX_RATIO_16X) &&
            (Input->Width >= Output->Width)) {
            if (Input->Width == (Output->Width*PREV_MAX_RATIO_16X)) {
                HorRatio = (Input->Width - VOUT_16XDS_TRUNCATE_LINE_NUM)/Output->Width;
            }
        }
#endif

        if ((HorRatio >= HL_MaxVprocPinOutputRatioMap[PrevIdx]) ||
            (VerRatio >= HL_MaxVprocPinOutputRatioMap[PrevIdx])) {
            AmbaLL_LogUInt5("  Exceed Resampling ratio[%d][%d %d]", PrevIdx, HorRatio, VerRatio, 0U, 0U);
            Rval = DSP_ERR_0001;
        } else {
            if ((HorRatio >= PREV_MAX_RATIO_8X) ||
                (VerRatio >= PREV_MAX_RATIO_8X)) {
                IsExtra2x = 1U;
            }

            if (IsExtra2x > 0U) {
                *MaxWidth = (UINT16)(HL_MaxVprocPinOutput2XWidthMap[PrevIdx]);
            } else {
                *MaxWidth = (UINT16)(HL_MaxVprocPinOutputWidthMap[PrevIdx]);
            }
        }
    } else {
        *MaxWidth = (UINT16)(HL_MaxVprocPinOutputWidthMap[PrevIdx]);
    }

    return Rval;
}

UINT32 HL_VprocUpscaleCheck(UINT16 PrevIdx,
                            const AMBA_DSP_WINDOW_s *In,
                            const AMBA_DSP_WINDOW_s *Out,
                            UINT8 *pCheckPass)
{
    /* PrevC can't have upscale */
    static const UINT8 HL_VprocPinUpscaleCapabilityMap[DSP_VPROC_PIN_NUM] = {
        [DSP_VPROC_PIN_PREVC] = 0,
        [DSP_VPROC_PIN_PREVA] = 1,
        [DSP_VPROC_PIN_PREVB] = 1,
        [DSP_VPROC_PIN_MAIN]  = 1,
    };
    UINT32 Rval = OK;

    /* Reset */
    *pCheckPass = 0;

    if (0U == HL_VprocPinUpscaleCapabilityMap[PrevIdx]) {
        if ((In->Width < Out->Width) || (In->Height < Out->Height)) {
            *pCheckPass = 0;
        } else {
            *pCheckPass = 1;
        }
    } else {
        *pCheckPass = 1;
    }

    return Rval;
}

//#define DEBUG_BINDING_WIDTH
inline static UINT32 CheckEveryTileOutput(UINT16 MainWidth,
                                          UINT16 ROIWidth,
                                          UINT16 ROIOffsetX,
                                          UINT16 PrevOutWidth,
                                          UINT16 MaxPinOutWidth,
                                          UINT16 TileNum,
                                          UINT16 Overlap)
{
    UINT32 Rval = OK;
    UINT16 i;
    UINT16 TileStart, TileEnd, ROIWidthInTile, MaxSrcWidthInTile, TileOut = 0U;
    UINT16 SingleTileW = MainWidth/TileNum;

    MaxSrcWidthInTile = (UINT16)((MaxPinOutWidth*ROIWidth)/PrevOutWidth);
    for (i=0U; i<TileNum; i++) {
        TileStart = (i == 0U)? 0U: TRUNCATE_16((UINT16)(SingleTileW*i) - Overlap, TILE_WIDTH_ALIGN);
        TileEnd = (i == (TileNum - 1U))? MainWidth: ALIGN_NUM16((UINT16)(SingleTileW*(i+1U)) + Overlap, TILE_WIDTH_ALIGN);

        if (ROIOffsetX <= TileStart) {
            if ((ROIOffsetX+ROIWidth) > TileEnd) {
                ROIWidthInTile = TileEnd - TileStart;
            } else if ((ROIOffsetX+ROIWidth) > TileStart) {
                ROIWidthInTile = ROIOffsetX + ROIWidth - TileStart;
            } else {
                ROIWidthInTile = 0U;
            }
        } else if (ROIOffsetX <= TileEnd){
            if ((ROIOffsetX+ROIWidth) > TileEnd) {
                ROIWidthInTile = TileEnd - ROIOffsetX;
            } else {
                ROIWidthInTile = ROIWidth;
            }
        } else {
            ROIWidthInTile = 0U;
        }
        TileOut = (ROIWidthInTile*PrevOutWidth)/ROIWidth;
#ifdef DEBUG_BINDING_WIDTH
        AmbaLL_LogUInt5("Tile[%d]   (S, E) = (%d, %d) ", i, TileStart, TileEnd, 0U, 0U);
        AmbaLL_LogUInt5("   ROI    (X, W) = (%d, %d) ", ROIOffsetX, ROIWidth, 0U, 0U, 0U);
        AmbaLL_LogUInt5("   ROIInTileW %d MaxSrc %d TiledOutW %d MaxPinOut %d",
                        ROIWidthInTile, MaxSrcWidthInTile, TileOut, MaxPinOutWidth, 0U);
#endif
        if (ROIWidthInTile > MaxSrcWidthInTile) {
            Rval = DSP_ERR_0001;
        }
        if (TileOut > MaxPinOutWidth) {
            Rval = DSP_ERR_0001;
        }
        if (Rval != OK) {
            break;
        }
    }

    return Rval;
}

UINT32 HL_FindMinimalVprocPin(const AMBA_DSP_WINDOW_s *Output,
                              const AMBA_DSP_WINDOW_s *ROI,
                              UINT16 MaxTileNum,
                              UINT16 VprocId,
                              UINT16 *VprocPin,
                              UINT8 *pFound,
                              UINT16 LowDlyPurpose,
                              UINT8 IsEncSync,
                              UINT8 IsDramMipiYuv)
{
    UINT8 Found = 0U;
    UINT8 UpscaleChkPass;
    UINT8 IsLowDlyEncPurpose;
    UINT16 TileOverlapX;
    UINT16 PrevIdx, MaxPinOutWidth = 0U;
    UINT32 Rval = OK;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VPROC_INFO_s VprocInfo = {0};

    HL_GetViewZoneInfoPtr(VprocId, &ViewZoneInfo);
    HL_GetVprocInfo(HL_MTX_OPT_ALL, VprocId, &VprocInfo);

    IsLowDlyEncPurpose = (1U == DSP_GetU16Bit(LowDlyPurpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U))? 1U: 0U;

    (void)HL_GetViewZoneWarpOverlapX(VprocId, &TileOverlapX);
    for (PrevIdx = DSP_VPROC_PIN_PREVC; PrevIdx<=DSP_VPROC_PIN_PREVB; PrevIdx++) {
        if ((IsDramMipiYuv == 1U) &&
            ((PrevIdx == DSP_VPROC_PIN_PREVC) ||
            (PrevIdx == DSP_VPROC_PIN_PREVB))) {
            continue;
        }

        if ((0U == IsLowDlyEncPurpose) &&
            (1U == IsEncSync) &&
            (PrevIdx == DSP_VPROC_PIN_PREVB)) {
            /* PrevB is reserved for LowDelay encode strm */
            continue;
        } else {
#ifdef SUPPORT_RESAMPLING_CHECK
            if (0U != VprocInfo.PinUsage[PrevIdx]) {
                continue;
            }
#ifdef DEBUG_BINDING_WIDTH
            AmbaLL_LogUInt5("VprocId %d PrevIdx %d ", VprocId, PrevIdx, 0U, 0U, 0U);
#endif
            (void)HL_GetVprocPinMaxWidth(PrevIdx, ROI, Output, &MaxPinOutWidth);
            Rval = CheckEveryTileOutput(ViewZoneInfo->Main.Width,
                                        ROI->Width, ROI->OffsetX,
                                        Output->Width,
                                        MaxPinOutWidth,
                                        MaxTileNum, TileOverlapX);
            if (Rval == OK) {
                if ((1U == IsLowDlyEncPurpose) &&
                    (1U == IsEncSync) &&
                    (PrevIdx == DSP_VPROC_PIN_PREVB)) {
                    *VprocPin = PrevIdx;
                    Found = 1U;
                } else {
                    (void)HL_VprocUpscaleCheck(PrevIdx,
                                               ROI,
                                               Output,
                                               &UpscaleChkPass);
                    if (UpscaleChkPass == 1U) {
                        *VprocPin = PrevIdx;
                        Found = 1U;
                    } else {
                        Found = 0U;
                    }
                }
            } else {
                Found = 0U;
            }
#else
                if (PrevIdx != DSP_VPROC_PIN_MAIN) {
                    MaxPinOutWidth = (UINT16)(HL_MaxVprocPinOutputWidthMap[PrevIdx]*MaxTileNum);
                } else {
                    MaxPinOutWidth = (UINT16)(HL_MaxVprocPinOutputWidthMap[PrevIdx]);
                }
                MaxPinOutWidth = (UINT16)(HL_MaxVprocPinOutputWidthMap[PrevIdx]*MaxTileNum);
#endif
        }

        if (Found == 1U) {
            break;
        }
    }

    /* Every tile is within the maximal pin width */
    *pFound = Found;
    if (Found == 0U) {
        Rval = DSP_ERR_0006;
    }

    return Rval;
}

#ifdef SUPPORT_VPROC_GROUPING
#ifdef PPSTRM_SWITCH_CHANGE_INPUTS_NUM
static inline UINT32 HL_GetVprocGroupNumImplEffChg(const CTX_RESOURCE_INFO_s *pResource,
                                                   UINT16 *pNum,
                                                   UINT32 *pViewZoneActiveBit)
{
    UINT8 IsEfctYuvStrm;
    UINT16 Num = *pNum;
    UINT16 i, j;
    UINT32 Rval = OK;
    UINT32 ViewZoneActiveBit = *pViewZoneActiveBit;
    UINT16 ViewZoneIdx = 0U;
    UINT16 NumVprocInGrp = 0U;
    UINT32 YuvStrmMaxChanBit = 0U;
    CTX_YUV_STRM_INFO_s *pYuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;

    /*
     * Groups for Effect-channel
     * Classify effect-channel components to the same group
     * */
    for (i = 0; i < pResource->YuvStrmNum; i++) {
        HL_GetYuvStrmInfoPtr(i, &pYuvStrmInfo);
        IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(i))? (UINT8)1U: (UINT8)0U;
        YuvStrmMaxChanBit = pYuvStrmInfo->MaxChanBitMask;
        NumVprocInGrp = 0U;

        if (0U == IsEfctYuvStrm) {
            continue;
        }

        if (Num >= NUM_VPROC_MAX_GROUP) {
            AmbaLL_LogUInt5("HL_GetVprocGroupNumImplEffChg GroupId[%d] is out of range[%d]",
                    Num, NUM_VPROC_MAX_GROUP, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
            break;
        }
        pYuvStrmLayout = &pYuvStrmInfo->Layout;

        /* Active */
        for (j = 0; j < pYuvStrmLayout->NumChan; j++) {
            ViewZoneIdx = pYuvStrmLayout->ChanCfg[j].ViewZoneId;
            if (0U == DSP_GetBit(ViewZoneActiveBit, ViewZoneIdx, 1U)) {
                continue;
            }
            NumVprocInGrp++;

            DSP_ClearBit(&ViewZoneActiveBit, ViewZoneIdx);
            DSP_ClearBit(&YuvStrmMaxChanBit, ViewZoneIdx);

            HL_GetViewZoneInfoLock(ViewZoneIdx, &pViewZoneInfo);
            pViewZoneInfo->VprocGrpId = (UINT8)Num;
            HL_GetViewZoneInfoUnLock(ViewZoneIdx);
        }

        /* Potential */
        if (YuvStrmMaxChanBit > 0U) {
            for (j = 0; j < pYuvStrmInfo->MaxChanNum; j++) {
                DSP_Bit2U16Idx(YuvStrmMaxChanBit, &ViewZoneIdx);
                if (0U == DSP_GetBit(ViewZoneActiveBit, ViewZoneIdx, 1U)) {
                    continue;
                }
                NumVprocInGrp++;

                DSP_ClearBit(&ViewZoneActiveBit, ViewZoneIdx);
                DSP_ClearBit(&YuvStrmMaxChanBit, ViewZoneIdx);

                HL_GetViewZoneInfoLock(ViewZoneIdx, &pViewZoneInfo);
                pViewZoneInfo->VprocGrpId = (UINT8)Num;
                HL_GetViewZoneInfoUnLock(ViewZoneIdx);
            }
        }

        if (NumVprocInGrp > 0U) {
            Num++;
        }
    }

    *pNum = Num;
    *pViewZoneActiveBit = ViewZoneActiveBit;

    return Rval;
}
#else
static inline UINT32 HL_GetVprocGroupNumImplEffNoChg(const CTX_RESOURCE_INFO_s *pResource,
                                                     UINT16 *pNum,
                                                     UINT32 *pViewZoneActiveBit)
{
    UINT8 IsEfctYuvStrm;
    UINT16 Num = *pNum;
    UINT16 i, j;
    UINT32 Rval = OK;
    UINT32 ViewZoneActiveBit = *pViewZoneActiveBit;
#ifndef SUPPORT_EFCT_UNION_GRP
    UINT16 ViewZoneIdx = 0U;
    UINT16 NumVprocInGrp = 0U;
    UINT32 YuvStrmMaxChanBit = 0U;
#endif
    CTX_YUV_STRM_INFO_s *pYuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;
#ifdef SUPPORT_EFCT_UNION_GRP
    CTX_YUV_STRM_INFO_s *pTmpYuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    UINT32 YuvStrmOI = 0U, YuvStrmBitsInGrp = 0U;
    UINT32 ViewZoneBitInGrp = 0U;
    UINT32 PendingViewZoneBits, NewPendingBits = 0U;
    UINT16 TargetYuvStrmId, TargetViewZoneId;
#endif

#ifdef SUPPORT_EFCT_UNION_GRP
    /* Search all EffectYuvStrm */
    for (i = 0; i < pResource->YuvStrmNum; i++) {
        IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(i))? (UINT8)1U: (UINT8)0U;
        if (0U == IsEfctYuvStrm) {
            continue;
        }
        DSP_SetBit(&YuvStrmOI, i);
    }

    for (i = 0U; i < pResource->YuvStrmNum; i++) {
        if (YuvStrmOI > 0U) {
            if (Num >= NUM_VPROC_MAX_GROUP) {
                AmbaLL_LogUInt5("HL_GetVprocGroupNumImplEffNoChg GroupId[%d] is out of range[%d]",
                        Num, NUM_VPROC_MAX_GROUP, 0U, 0U, 0U);
                Rval = DSP_ERR_0000;
                break;
            }

            DSP_Bit2U16Idx(YuvStrmOI, &TargetYuvStrmId);
            HL_GetYuvStrmInfoPtr(TargetYuvStrmId, &pYuvStrmInfo);
            ViewZoneBitInGrp = pYuvStrmInfo->MaxChanBitMask;
            PendingViewZoneBits = pYuvStrmInfo->MaxChanBitMask;
            DSP_SetBit(&YuvStrmBitsInGrp, TargetYuvStrmId);
            while (PendingViewZoneBits > 0U) {
                DSP_Bit2U16Idx(PendingViewZoneBits, &TargetViewZoneId);
                for (j = (TargetYuvStrmId + 1U); j < pResource->YuvStrmNum; j++) {
                    if (1U == DSP_GetBit(YuvStrmOI, j, 1U)) {
                        HL_GetYuvStrmInfoPtr(j, &pTmpYuvStrmInfo);
                        if (1U == DSP_GetBit(pTmpYuvStrmInfo->MaxChanBitMask, TargetViewZoneId, 1U)) {
                            NewPendingBits = pTmpYuvStrmInfo->MaxChanBitMask;
                            NewPendingBits &= ~ViewZoneBitInGrp;
                            DSP_SetBit(&YuvStrmBitsInGrp, j);
                        }
                    }
                }
                DSP_SetBit(&ViewZoneBitInGrp, TargetViewZoneId);
                PendingViewZoneBits |= NewPendingBits;
                NewPendingBits = 0U;
                DSP_ClearBit(&PendingViewZoneBits, TargetViewZoneId);

                HL_GetViewZoneInfoLock(TargetViewZoneId, &pViewZoneInfo);
                pViewZoneInfo->VprocGrpId = (UINT8)Num;
                HL_GetViewZoneInfoUnLock(TargetViewZoneId);
            }

            YuvStrmOI &= ~YuvStrmBitsInGrp;
            ViewZoneActiveBit &= ~ViewZoneBitInGrp;
            Num += 1U;
        }
    }

#else
    /*
     * Groups for Effect-channel
     * Classify effect-channel components to the same group
     * */
    for (i = 0; i < pResource->YuvStrmNum; i++) {
        HL_GetYuvStrmInfoPtr(i, &pYuvStrmInfo);
        IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(i))? (UINT8)1U: (UINT8)0U;
        YuvStrmMaxChanBit = pYuvStrmInfo->MaxChanBitMask;
        NumVprocInGrp = 0U;

        if (0U == IsEfctYuvStrm) {
            continue;
        }
        if (Num >= NUM_VPROC_MAX_GROUP) {
            AmbaLL_LogUInt5("HL_GetVprocGroupNumImplEffNoChg GroupId[%d] is out of range[%d]",
                    Num, NUM_VPROC_MAX_GROUP, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
            break;
        }
        if (YuvStrmMaxChanBit > 0U) {
            for (j = 0; j < AMBA_DSP_MAX_YUVSTRM_VIEW_NUM; j++) {
                DSP_Bit2U16Idx(YuvStrmMaxChanBit, &ViewZoneIdx);
                if (0U == DSP_GetBit(ViewZoneActiveBit, ViewZoneIdx, 1U)) {
                    continue;
                }

                NumVprocInGrp++;

                DSP_ClearBit(&ViewZoneActiveBit, ViewZoneIdx);
                DSP_ClearBit(&YuvStrmMaxChanBit, ViewZoneIdx);

                HL_GetViewZoneInfoLock(ViewZoneIdx, &pViewZoneInfo);
                pViewZoneInfo->VprocGrpId = (UINT8)Num;
                HL_GetViewZoneInfoUnLock(ViewZoneIdx);
            }
        }
        if (NumVprocInGrp > 0U) {
            Num += 1U;
        }
    }
#endif
    *pNum = Num;
    *pViewZoneActiveBit = ViewZoneActiveBit;

    return Rval;
}
#endif

static inline UINT32 HL_GetVprocGroupNumImplEff(const CTX_RESOURCE_INFO_s *pResource,
                                              UINT16 *pNum,
                                              UINT32 *pViewZoneActiveBit)
{
    UINT32 Rval = OK;

#ifdef PPSTRM_SWITCH_CHANGE_INPUTS_NUM
    Rval = HL_GetVprocGroupNumImplEffChg(pResource,
                                         pNum,
                                         pViewZoneActiveBit);
#else
    Rval = HL_GetVprocGroupNumImplEffNoChg(pResource,
                                           pNum,
                                           pViewZoneActiveBit);
#endif
    return Rval;
}

#ifdef SUPPORT_VPROC_INDEPENDENT_WITHIN_GROUPING
static inline UINT32 HL_GetVidGrpNumImplVzNonEfct(const CTX_RESOURCE_INFO_s *pResource,
                                                  UINT16 *pNum,
                                                  UINT32 *pViewZoneActiveBit,
                                                  UINT16 *pFreeRunBit)
{
    UINT32 Rval = OK;
    UINT8 ExitLoop = 0U;
    UINT16 Num = *pNum;
    UINT16 NumVprocInGrp = 0U, FreeRunBit = *pFreeRunBit;
    UINT16 i, j, find = 0U, PostViewId;
    UINT32 ViewZoneActiveBit = *pViewZoneActiveBit;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VIEWZONE_INFO_s *pPreViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VIEWZONE_INFO_s *pPostViewZoneInfo = HL_CtxViewZoneInfoPtr;

    NumVprocInGrp = 0U;
    for (i = 0U; i < pResource->MaxViewZoneNum; i++) {
        HL_GetViewZoneInfoPtr(i, &pViewZoneInfo);
        if ((0U == DSP_GetBit(ViewZoneActiveBit, i, 1U)) ||
            (pViewZoneInfo->InputFromMemory != VIN_SRC_FROM_HW)) {
            continue;
        }
        if (Num >= NUM_VPROC_MAX_GROUP) {
            AmbaLL_LogUInt5("HL_GetVidGrpNumImplVzNonEfct GroupId[%d] is out of range[%d]",
                    Num, NUM_VPROC_MAX_GROUP, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
            ExitLoop = 1U;
        } else {
            NumVprocInGrp++;
            DSP_ClearBit(&ViewZoneActiveBit, i);
            HL_GetViewZoneInfoLock(i, &pViewZoneInfo);
            pViewZoneInfo->VprocGrpId = (UINT8)Num;
            HL_GetViewZoneInfoUnLock(i);

            if (pViewZoneInfo->IsPostStrm > 0U) {
                find = 0U;
                for (j = 0U; j < pResource->MaxViewZoneNum; j++) {
                    if (find == 0U) {
                        HL_GetViewZoneInfoPtr(j, &pPreViewZoneInfo);
                        if ((pPreViewZoneInfo->IsUpStrmSrc > 0U) && (pPreViewZoneInfo->UpStrmSrcId == i)) {
                            PostViewId = pViewZoneInfo->UpStrmSrcId;
                            HL_GetViewZoneInfoPtr(PostViewId, &pPostViewZoneInfo);
                            if (pPostViewZoneInfo->Pipe == DSP_DRAM_PIPE_RAW_PROC) {
                                if (NumVprocInGrp >= NUM_VPROC_MAX_CHAN) {
                                    AmbaLL_LogUInt5("HL_GetVidGrpNumImplVzNonEfct VprocOrderId[%d] is out of range[%d]",
                                            NumVprocInGrp, NUM_VPROC_MAX_CHAN, 0U, 0U, 0U);
                                    Rval = DSP_ERR_0000;
                                    ExitLoop = 1U;
                                    break;
                                }
                                NumVprocInGrp++;
                                DSP_ClearBit(&ViewZoneActiveBit, PostViewId);
                                HL_GetViewZoneInfoLock(PostViewId, &pPostViewZoneInfo);
                                pPostViewZoneInfo->VprocGrpId = (UINT8)Num;
                                HL_GetViewZoneInfoUnLock(PostViewId);

                                NumVprocInGrp++;
                                DSP_ClearBit(&ViewZoneActiveBit, j);
                                HL_GetViewZoneInfoLock(j, &pPreViewZoneInfo);
                                pPreViewZoneInfo->VprocGrpId = (UINT8)Num;
                                HL_GetViewZoneInfoUnLock(j);
                                find = 1U;
                            }
                        }
                    }
                }
            }
        }
        if ((find == 1U) || (ExitLoop == 1U)) {
            break;
        }
    }
    if (NumVprocInGrp > 0U) {
        if (pViewZoneInfo->IsPostStrm == 0U) {
            DSP_SetU16Bit(&FreeRunBit, Num);
        }
        Num += 1U;
    }

    *pNum = Num;
    *pViewZoneActiveBit = ViewZoneActiveBit;
    *pFreeRunBit = FreeRunBit;

    return Rval;

}
#else

static inline UINT32 HL_GetVidGrpNumImplVzEfct(const CTX_RESOURCE_INFO_s *pResource,
                                               UINT16 *pNum,
                                               UINT32 *pViewZoneActiveBit)
{
    UINT32 Rval = OK;
    UINT8 MasterVinDecimation;
    UINT16 Num = *pNum;
    UINT16 NumVprocInGrp = 0U;
    UINT16 i, j, k, VinIdx = 0U;
    UINT32 ViewZoneActiveBit = *pViewZoneActiveBit;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;

    for (i = 0; i<AMBA_DSP_MAX_VIN_NUM; i++) {
        if (0U == DSP_GetU16Bit(pResource->VinBit, i, 1U)) {
            continue;
        }

        for (j = 0; j < pResource->MaxViewZoneNum; j++) {
            NumVprocInGrp = 0U;
            HL_GetViewZoneInfoPtr(j, &pViewZoneInfo);
            DSP_Bit2U16Idx(pViewZoneInfo->SourceVin, &VinIdx);
            if ((0U == DSP_GetBit(ViewZoneActiveBit, j, 1U)) ||
                (pViewZoneInfo->InputFromMemory != VIN_SRC_FROM_HW) ||
                (VinIdx != i)) {
                continue;
            }

            MasterVinDecimation = pViewZoneInfo->VinDecimationRate;
            for (k = j; k < pResource->MaxViewZoneNum; k++) {
                HL_GetViewZoneInfoPtr(k, &pViewZoneInfo);
                DSP_Bit2U16Idx(pViewZoneInfo->SourceVin, &VinIdx);
                if ((0U == DSP_GetBit(ViewZoneActiveBit, k, 1U)) ||
                    (pViewZoneInfo->InputFromMemory != VIN_SRC_FROM_HW) ||
                    (VinIdx != i) ||
                    (MasterVinDecimation != pViewZoneInfo->VinDecimationRate)) {
                    continue;
                }
                if ((Num >= NUM_VPROC_MAX_GROUP) || (NumVprocInGrp >= NUM_VPROC_MAX_CHAN))  {
                    AmbaLL_LogUInt5("HL_GetVidGrpNumImplVzEfct is out of range: GroupId[%d %d] VprocOrderId[%d %d]",
                                Num, NUM_VPROC_MAX_GROUP, NumVprocInGrp, NUM_VPROC_MAX_CHAN, 0U);
                    Rval = DSP_ERR_0000;
                    break;
                }

                NumVprocInGrp++;
                DSP_ClearBit(&ViewZoneActiveBit, k);
                HL_GetViewZoneInfoLock(k, &pViewZoneInfo);
                pViewZoneInfo->VprocGrpId = (UINT8)Num;
                HL_GetViewZoneInfoUnLock(k);
            }
            if (NumVprocInGrp > 0U) {
                Num += 1U;
            }
        }
    }
    *pNum = Num;
    *pViewZoneActiveBit = ViewZoneActiveBit;

    return Rval;
}
#endif

static inline UINT32 HL_GetVprocGroupNumImplVz(const CTX_RESOURCE_INFO_s *pResource,
                                               UINT16 *pNum,
                                               UINT32 *pViewZoneActiveBit,
                                               UINT16 *pFreeRunBit)
{
    UINT32 Rval = OK;

    /*
     * Groups for ViewZone
     * Rule_1 : Different Vin use different group
     * Rule_2 : Different DecimationRate in same Vin use different Group
     * 20191213, since MaxGrpNum = 4 inside uCode.
     *   for the case multi-vin but no effect,
     *   we can put differnt vin into same grp and ENABLE free_run bit
     *   after that, ucode will treat those vproc as independent channel
     */

#ifdef SUPPORT_VPROC_INDEPENDENT_WITHIN_GROUPING
    Rval = HL_GetVidGrpNumImplVzNonEfct(pResource, pNum, pViewZoneActiveBit, pFreeRunBit);
#else
    (void)pFreeRunBit;
    Rval = HL_GetVidGrpNumImplVzEfct(pResource, pNum, pViewZoneActiveBit);
#endif

    return Rval;
}

static inline UINT32 HL_GetVprocGroupNumImplDp(const CTX_RESOURCE_INFO_s *pResource,
                                               UINT16 *pNum,
                                               UINT32 *pViewZoneActiveBit,
                                               UINT16 *pFreeRunBit)
{
    UINT16 Num = *pNum, FreeRunBit = *pFreeRunBit;
    UINT16 i;
    UINT16 NumVprocInGrp;
    UINT32 Rval = OK;
    UINT32 ViewZoneActiveBit = *pViewZoneActiveBit;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;

    /* Groups for Duplex */
    if (HL_HasDecResource() == 1U) {
        NumVprocInGrp = 0U;
        for (i = 0U; i < pResource->MaxViewZoneNum; i++) {
            HL_GetViewZoneInfoPtr(i, &pViewZoneInfo);
            if ((0U == DSP_GetBit(ViewZoneActiveBit, i, 1U)) ||
                (pViewZoneInfo->InputFromMemory != VIN_SRC_FROM_DEC)) {
                continue;
            }
            if ((Num >= NUM_VPROC_MAX_GROUP) || (NumVprocInGrp >= NUM_VPROC_MAX_CHAN))  {
                AmbaLL_LogUInt5("HL_GetVprocGroupNumImplDp is out of range: GroupId[%d %d] VprocOrderId[%d %d]",
                            Num, NUM_VPROC_MAX_GROUP, NumVprocInGrp, NUM_VPROC_MAX_CHAN, 0U);
                Rval = DSP_ERR_0000;
                break;
            }

            NumVprocInGrp++;
            DSP_ClearBit(&ViewZoneActiveBit, i);
            HL_GetViewZoneInfoLock(i, &pViewZoneInfo);
            pViewZoneInfo->VprocGrpId = (UINT8)Num;
            HL_GetViewZoneInfoUnLock(i);
        }
        if (NumVprocInGrp > 0U) {
            /* duplex group member should go free */
            DSP_SetU16Bit(&FreeRunBit, Num);
            Num += 1U;
        }

        *pNum = Num;
        *pViewZoneActiveBit = ViewZoneActiveBit;
        *pFreeRunBit = FreeRunBit;
    }

    return Rval;
}

static inline UINT32 HL_GetVprocGroupNumImplMem(const CTX_RESOURCE_INFO_s *pResource,
                                                UINT16 *pNum,
                                                UINT32 *pViewZoneActiveBit,
                                                UINT16 *pFreeRunBit)
{
    UINT16 Num = *pNum, FreeRunBit = *pFreeRunBit;
    UINT16 i, NumVprocInGrp = 0U;
    UINT32 Rval = OK;
    UINT32 ViewZoneActiveBit = *pViewZoneActiveBit;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;

    /* Groups for MemInput */
    NumVprocInGrp = 0U;
    for (i = 0U; i < pResource->ViewZoneNum; i++) {
        if (0U == DSP_GetBit(ViewZoneActiveBit, i, 1U)) {
            continue;
        }
        HL_GetViewZoneInfoPtr(i, &pViewZoneInfo);
        if ((pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) ||
            (pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422) ||
            (pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420)) {
            if ((Num >= NUM_VPROC_MAX_GROUP) || (NumVprocInGrp >= NUM_VPROC_MAX_CHAN)) {
                AmbaLL_LogUInt5("HL_GetVprocGroupNumImplMem is out of range: GroupId[%d %d] VprocOrderId[%d %d]",
                            Num, NUM_VPROC_MAX_GROUP, NumVprocInGrp, NUM_VPROC_MAX_CHAN, 0U);
                Rval = DSP_ERR_0000;
                break;
            }
            NumVprocInGrp++;
            DSP_ClearBit(&ViewZoneActiveBit, i);
            HL_GetViewZoneInfoLock(i, &pViewZoneInfo);
            pViewZoneInfo->VprocGrpId = (UINT8)Num;
            HL_GetViewZoneInfoUnLock(i);
        }
    }
    if (NumVprocInGrp > 0U) {
        /* MemInput group member should go free */
        DSP_SetU16Bit(&FreeRunBit, Num);
        Num += 1U;
    }

    *pViewZoneActiveBit = ViewZoneActiveBit;
    *pNum = Num;
    *pFreeRunBit = FreeRunBit;

    return Rval;
}

static inline UINT32 HL_GetVprocGroupNumImplRecon(const CTX_RESOURCE_INFO_s *pResource,
                                                UINT16 *pNum,
                                                UINT32 *pViewZoneActiveBit)
{
    UINT16 Num = *pNum;
    UINT16 i;
    UINT32 Rval = OK;
    UINT32 ViewZoneActiveBit = *pViewZoneActiveBit;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;

    /* Groups for Recon */
    for (i = 0U; i < pResource->ViewZoneNum; i++) {
        if (0U == DSP_GetBit(ViewZoneActiveBit, i, 1U)) {
            continue;
        }
        HL_GetViewZoneInfoPtr(i, &pViewZoneInfo);
        if (pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) {
            if (Num >= NUM_VPROC_MAX_GROUP) {
                AmbaLL_LogUInt5("HL_GetVprocGroupNumImplRecon GroupId[%d] is out of range[%d]",
                            Num, NUM_VPROC_MAX_GROUP, 0U, 0U, 0U);
                Rval = DSP_ERR_0000;
                break;
            }

            DSP_ClearBit(&ViewZoneActiveBit, i);
            HL_GetViewZoneInfoLock(i, &pViewZoneInfo);
            pViewZoneInfo->VprocGrpId = (UINT8)Num;
            HL_GetViewZoneInfoUnLock(i);

            /* Recon use different group for each decoder */
            Num += 1U;
        }
    }

    *pViewZoneActiveBit = ViewZoneActiveBit;
    *pNum = Num;

    return Rval;
}

static inline UINT32 HL_GetVprocGroupNumImplPiv(UINT16 *pNum)
{
    UINT16 Num = *pNum;
    UINT16 i, NumVprocInGrp = 0U;
    UINT32 Rval = OK;
    CTX_DSP_INST_INFO_s DspInstInfo = {0};
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;

    /* Groups for PIV  */
    HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);
    for (i = 0U; i < DspInstInfo.MaxVpocNum; i++) {
        /* StlProc is the last Vproc Group */
        if (DspInstInfo.VprocPurpose[i] == VPROC_PURPOSE_STILL) {
            if ((Num >= NUM_VPROC_MAX_GROUP) || (NumVprocInGrp >= NUM_VPROC_MAX_CHAN)) {
                AmbaLL_LogUInt5("HL_GetVprocGroupNumImplPiv is out of range: GroupId[%d %d] VprocOrderId[%d %d]",
                            Num, NUM_VPROC_MAX_GROUP, NumVprocInGrp, NUM_VPROC_MAX_CHAN, 0U);
                Rval = DSP_ERR_0000;
                break;
            }

            NumVprocInGrp++;
            HL_GetViewZoneInfoLock(i, &pViewZoneInfo);
            pViewZoneInfo->VprocGrpId = (UINT8)Num;
            HL_GetViewZoneInfoUnLock(i);
        }
    }
    if (NumVprocInGrp > 0U) {
        Num += 1U;
    }

    *pNum = Num;

    return Rval;
}

static inline UINT32 HL_GetVprocGroupNumImpl(UINT16 *pNum, UINT16 *pFreeRunBit)
{
    UINT16 i;
    UINT16 Num = 0U, CountRawProc = 0U, FreeRunBit = 0x0U;
    UINT32 Rval;
    UINT32 ViewZoneActiveBit;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetResourcePtr(&pResource);
    ViewZoneActiveBit = pResource->ViewZoneActiveBit;

    /* [1] Groups for Effect-channel */
    Rval = HL_GetVprocGroupNumImplEff(pResource, &Num, &ViewZoneActiveBit);
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_GetVprocGroupNumImplEff Num[%d] Rval[0x%X]", Num, Rval, 0U, 0U, 0U);
    }

    /* [2] Groups for ViewZone */
    for (i = 0U; i < pResource->MaxViewZoneNum; i++) {
        HL_GetViewZoneInfoPtr(i, &pViewZoneInfo);
        if (pViewZoneInfo->Pipe == DSP_DRAM_PIPE_RAW_PROC) {
            CountRawProc++;
        }
    }
    if (CountRawProc == 0U) {
        CountRawProc = 1U;
    }
    for (i = 0U; i < CountRawProc; i++) {
        Rval = HL_GetVprocGroupNumImplVz(pResource, &Num, &ViewZoneActiveBit, &FreeRunBit);
        if (Rval != OK) {
            AmbaLL_LogUInt5("HL_GetVprocGroupNumImplVz Num[%d] Rval[0x%X]", Num, Rval, 0U, 0U, 0U);
        }
    }

    /* [3] Groups for Duplex */
    Rval = HL_GetVprocGroupNumImplDp(pResource, &Num, &ViewZoneActiveBit, &FreeRunBit);
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_GetVprocGroupNumImplDp Num[%d] Rval[0x%X]", Num, Rval, 0U, 0U, 0U);
    }

    /* [4] Groups for MemInput */
    Rval = HL_GetVprocGroupNumImplMem(pResource, &Num, &ViewZoneActiveBit, &FreeRunBit);
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_GetVprocGroupNumImplMem Num[%d] Rval[0x%X]", Num, Rval, 0U, 0U, 0U);
    }

    /* [5] Groups for Recon */
    Rval = HL_GetVprocGroupNumImplRecon(pResource, &Num, &ViewZoneActiveBit);
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_GetVprocGroupNumImplRecon Num[%d] Rval[0x%X]", Num, Rval, 0U, 0U, 0U);
    }

    /* [6] Groups for PIV */
    /* StlProc is the last Vproc Group */
    Rval = HL_GetVprocGroupNumImplPiv(&Num);
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_GetVprocGroupNumImplPiv Num[%d] Rval[0x%X]", Num, Rval, 0U, 0U, 0U);
    }

    /* Check if number of group excesses the limit */
    if (Num > NUM_VPROC_MAX_GROUP) {
        AmbaLL_LogUInt5("Number of groups (%d) is out of range ", Num, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    }

    *pNum = Num;
    *pFreeRunBit = FreeRunBit;
    return Rval;
}
#endif

UINT32 HL_CalcVprocGroupNum(void)
{
    UINT32 Rval;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;
    UINT16 GrpNum = 0U, FreeRunBit = 0U;

#ifdef SUPPORT_VPROC_GROUPING
    Rval = HL_GetVprocGroupNumImpl(&GrpNum, &FreeRunBit);
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_GetVprocGroupNumImpl Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }
#endif
    HL_GetResourceLock(&pResource);
    pResource->VprocGrpNum = GrpNum;
    pResource->VprocGrpFreeRunBit = FreeRunBit;
    HL_GetResourceUnLock();

    return Rval;
}

UINT32 HL_GetVprocGroupNum(UINT16 *Num)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    *Num = Resource->VprocGrpNum;

    return Rval;
}

UINT32 HL_GetVprocGroupFreeRun(UINT16 *pFreeRunBit)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    *pFreeRunBit = Resource->VprocGrpFreeRunBit;

    return Rval;
}

static inline UINT8 HL_IsViewzoneFromVinId(const UINT32 ViewZoneActiveBit,
                                           const UINT16 ViewZoneId,
                                           const UINT16 VinIdx)
{
    UINT8 IsSkippedVz = 0U;
    UINT16 VzVinIdx = 0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    if (0U == DSP_GetBit(ViewZoneActiveBit, ViewZoneId, 1U)) {
        IsSkippedVz = 1U;
    }

    if (IsSkippedVz == 0U) {
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VzVinIdx);
        if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) ||
            (VzVinIdx != VinIdx)) {
            IsSkippedVz = 1U;
        }
    }

    return IsSkippedVz;
}

static inline UINT8 HL_IsViewzoneFromVinIdDeci(const UINT32 ViewZoneActiveBit,
                                               const UINT16 ViewZoneId,
                                               const UINT16 VinIdx,
                                               const UINT8 DecimationRate)
{
    UINT8 IsSkippedVz = 0U;
    UINT16 VzVinIdx = 0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    if (0U == DSP_GetBit(ViewZoneActiveBit, ViewZoneId, 1U)) {
        IsSkippedVz = 1U;
    }

    if (IsSkippedVz == 0U) {
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VzVinIdx);
        if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) ||
            (ViewZoneInfo->VinDecimationRate != DecimationRate) ||
            (VzVinIdx != VinIdx)) {
            IsSkippedVz = 1U;
        }
    }

    return IsSkippedVz;
}

UINT32 HL_GetVprocGroupIdx(UINT16 VprocId, UINT16 *pIdx, UINT8 IsStlProc)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;

    *pIdx = 0U;
    HL_GetResourcePtr(&pResource);
    if (IsStlProc > 0U) {
        /* StlProc is the last Vproc Group */
        if (pResource->VprocGrpNum > 0U) {
            *pIdx = pResource->VprocGrpNum - 1U;
        }
    } else {
        HL_GetViewZoneInfoPtr(VprocId, &pViewZoneInfo);
        *pIdx = (UINT16)pViewZoneInfo->VprocGrpId;
    }

    return Rval;
}

UINT32 HL_GetViewZonePrevDest(UINT16 ViewZoneId, UINT16 VprocPin, UINT16 *Dest)
{
    UINT32 Rval = OK;
#ifdef UCODE_SUPPORT_PREVFILTER_REARRANGE
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 IsEncPurpose = 0U, IsVoutPurpose = 0U, IsMemPurpose = 0U;
    UINT16 YuvStrmIdx = 0U;

    HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);
    HL_GetResourcePtr(&Resource);
    if (VprocInfo.PinUsage[VprocPin] > 0U) {
        DSP_Bit2U16Idx(VprocInfo.PinUsage[VprocPin], &YuvStrmIdx);
        HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);

        IsEncPurpose =(1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U))? 1U: 0U;
        IsVoutPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U))? 1U: 0U;
        IsMemPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_MEMORY_IDX, 1U))? 1U: 0U;

        if (IsEncPurpose > 0U) {
            *Dest = AMBA_DSP_PREV_DEST_PIN;
        } else if (IsVoutPurpose > 0U) {
            if (1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, VOUT_IDX_A, 1U)) {
                *Dest = AMBA_DSP_PREV_DEST_VOUT0;
            } else if (1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, VOUT_IDX_B, 1U)) {
                *Dest = AMBA_DSP_PREV_DEST_VOUT1;
            } else {
                *Dest = AMBA_DSP_PREV_DEST_DUMMY;
            }
        } else if (IsMemPurpose > 0U) {
            *Dest = AMBA_DSP_PREV_DEST_PIN;
        } else {
            *Dest = AMBA_DSP_PREV_DEST_DUMMY;
        }
    } else {
        *Dest = AMBA_DSP_PREV_DEST_DUMMY;
    }
#else
    if (VprocPin == DSP_VPROC_PIN_PREVA) {
        *Dest = AMBA_DSP_PREV_DEST_PIN;
    } else if (VprocPin == DSP_VPROC_PIN_PREVB) {
        *Dest = AMBA_DSP_PREV_DEST_VOUT1;
    } else { //PrevC
        *Dest = AMBA_DSP_PREV_DEST_VOUT0;
    }
#endif
    return Rval;
}

static inline void HL_GetPinOutDestBufNumPin(UINT16 VprocPin,
                                             UINT16 *pNum,
                                             UINT16 *pAuxNum)
{
    UINT8 IsEfctYuvStrm, IsVoutPurpose;
    UINT16 i, MaxNum = 0U, MaxAuxNum = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VPROC_INFO_s VprocInfo = {0};

    HL_GetResourcePtr(&Resource);
    for (i=0U; i<Resource->YuvStrmNum; i++) {
        HL_GetYuvStrmInfoPtr(i, &YuvStrmInfo);
        IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(i))? (UINT8)1U: (UINT8)0U;
        IsVoutPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U))? (UINT8)1U: (UINT8)0U;

        // Only effect channels use postp buffers
        if (0U == IsEfctYuvStrm) {
            continue;
        }
        if ((VprocPin == DSP_VPROC_PIN_MAIN) && (IsVoutPurpose == 0U)) {
            HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &VprocInfo);
            if (VprocInfo.PinUsage[DSP_VPROC_PIN_MAIN] > 0U) {
                if (1U == DSP_GetBit(VprocInfo.PinUsage[DSP_VPROC_PIN_MAIN], i, 1U)) {
                    if (YuvStrmInfo->BufNum > 0U) {
                        MaxNum = MAX2_16(MaxNum, YuvStrmInfo->BufNum);
                    } else {
                        MaxNum = MAX2_16(MaxNum, CV2X_MAX_PREV_FB_NUM);
                    }
                    MaxAuxNum = MaxNum;
                }
            }
        } else if (VprocPin == DSP_VPROC_PIN_PREVA) {
            HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &VprocInfo);
            if (VprocInfo.PinUsage[DSP_VPROC_PIN_PREVA] > 0U) {
                if (1U == DSP_GetBit(VprocInfo.PinUsage[DSP_VPROC_PIN_PREVA], i, 1U)) {
                    if (YuvStrmInfo->BufNum > 0U) {
                        MaxNum = MAX2_16(MaxNum, YuvStrmInfo->BufNum);
                    } else {
                        MaxNum = MAX2_16(MaxNum, CV2X_MAX_PREV_FB_NUM);
                    }
//FIXME, uCode DONT support disable AuxBuf yet when vout purpose
                    //MaxAuxNum = 0U;
                    MaxAuxNum = MaxNum;
                }
            }
        } else {
            // keep MaxNum the same
        }
    }
    *pNum = MaxNum;
    *pAuxNum = MaxAuxNum;

}


#if defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
static inline void HL_GetPinOutDestBufNumVoutImp(UINT8 IsEfctYuvStrm, UINT16 Dest, UINT16 i, UINT16 *pMaxNum, UINT16 *pMaxAuxNum,
                                                 CTX_VPROC_INFO_s *pVprocInfo, const CTX_YUV_STRM_INFO_s *pYuvStrmInfo)
{
    if (1U == IsEfctYuvStrm) {
        if ((1U == DSP_GetU16Bit(pYuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) &&
            (Dest == AMBA_DSP_PREV_DEST_VOUT0)) {
            HL_GetVprocInfo(HL_MTX_OPT_ALL, pYuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, pVprocInfo);
            if (pVprocInfo->PinUsage[DSP_VPROC_PIN_PREVC] > 0U) {
                if (1U == DSP_GetBit(pVprocInfo->PinUsage[DSP_VPROC_PIN_PREVC], i, 1U)) {
                    if (pYuvStrmInfo->BufNum > 0U) {
                        *pMaxNum = MAX2_16(*pMaxNum, pYuvStrmInfo->BufNum);
                    } else {
                        *pMaxNum = MAX2_16(*pMaxNum, CV2X_MAX_PREV_FB_NUM);
                    }
                    *pMaxAuxNum = *pMaxNum;
                }
            }
        } else if ((1U == DSP_GetU16Bit(pYuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) &&
                   (Dest == AMBA_DSP_PREV_DEST_VOUT1)) {
            HL_GetVprocInfo(HL_MTX_OPT_ALL, pYuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, pVprocInfo);
            if (pVprocInfo->PinUsage[DSP_VPROC_PIN_PREVB] > 0U) {
                if (1U == DSP_GetBit(pVprocInfo->PinUsage[DSP_VPROC_PIN_PREVB], i, 1U)) {
                    if (pYuvStrmInfo->BufNum > 0U) {
                        *pMaxNum = MAX2_16(*pMaxNum, pYuvStrmInfo->BufNum);
                    } else {
                        *pMaxNum = MAX2_16(*pMaxNum, CV2X_MAX_PREV_FB_NUM);
                    }
                    *pMaxAuxNum = *pMaxNum;
                }
            }
        } else {
            // keep MaxNum the same
        }
    } else {
        if ((1U == DSP_GetU16Bit(pYuvStrmInfo->DestVout, VOUT_IDX_A, 1U)) &&
            (Dest == AMBA_DSP_PREV_DEST_VOUT0)) {
            if (pYuvStrmInfo->BufNum > 0U) {
                *pMaxNum = MAX2_16(*pMaxNum, pYuvStrmInfo->BufNum);
            } else {
                *pMaxNum = MAX2_16(*pMaxNum, CV2X_MAX_PREV_FB_NUM);
            }
            *pMaxAuxNum = *pMaxNum;
        } else if ((1U == DSP_GetU16Bit(pYuvStrmInfo->DestVout, VOUT_IDX_B, 1U)) &&
                   (Dest == AMBA_DSP_PREV_DEST_VOUT1)) {
            if (pYuvStrmInfo->BufNum > 0U) {
                *pMaxNum = MAX2_16(*pMaxNum, pYuvStrmInfo->BufNum);
            } else {
                *pMaxNum = MAX2_16(*pMaxNum, CV2X_MAX_PREV_FB_NUM);
            }
            *pMaxAuxNum = *pMaxNum;
        } else {
            // keep MaxNum the same
        }
    }
}
#endif

static inline void HL_GetPinOutDestBufNumVout(UINT16 Dest,
                                              UINT16 *pNum,
                                              UINT16 *pAuxNum)
{
    UINT8 IsEfctYuvStrm;
    UINT16 i, MaxNum = 0U, MaxAuxNum = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
    CTX_VPROC_INFO_s VprocInfo = {0};
#endif

    HL_GetResourcePtr(&Resource);
    for (i=0U; i<Resource->YuvStrmNum; i++) {
        HL_GetYuvStrmInfoPtr(i, &YuvStrmInfo);
        IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(i))? (UINT8)1U: (UINT8)0U;

        // Only Vout YuvStreams use common buffers
        if (0U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) {
            continue;
        }

#if defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
        /*
         * 20190619, In CVx current design
         * PostP_XX_ and Prev_comX_ have different meaning
         * NonEffect Pipeline
         *   - means the YuvBuffer to Destination
         * Effect Pipeline
         *   - means the YuvBuffer from which VprocPin
         *     postp_main_ : From Main
         *     postp_Pip_ : From PrevA
         *     prev_com0(DEST_VOUT0) : From PrevC
         *     prev_com1(DEST_VOUT1) : From PrevB
         */

        HL_GetPinOutDestBufNumVoutImp(IsEfctYuvStrm, Dest, i, &MaxNum, &MaxAuxNum, &VprocInfo, YuvStrmInfo);

#elif defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE)
        /*
         * 20190619, In CVx current design
         * PostP_XX_ and Prev_comX_ have different meaning
         * NonEffect Pipeline
         *   - means the YuvBuffer to Destination
         * Effect Pipeline
         *   - means the YuvBuffer from which VprocPin
         *     postp_main_ : From Main
         *     postp_Pip_ : From PrevA
         *     prev_com0(DEST_VOUT0) : From PrevC
         *     prev_com1(DEST_VOUT1) : From PrevB
         */

        if (1U == IsEfctYuvStrm) {
            if ((1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) &&
                (Dest == AMBA_DSP_PREV_DEST_VOUT0)) {
                HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &VprocInfo);
                if (VprocInfo.PinUsage[DSP_VPROC_PIN_PREVC] > 0U) {
                    if (1U == DSP_GetBit(VprocInfo.PinUsage[DSP_VPROC_PIN_PREVC], i, 1U)) {
                        if (YuvStrmInfo->BufNum > 0U) {
                            MaxNum = MAX2_16(MaxNum, YuvStrmInfo->BufNum);
                        } else {
                            MaxNum = MAX2_16(MaxNum, CV2X_MAX_PREV_FB_NUM);
                        }
                        MaxAuxNum = MaxNum;
                    }
                }
            } else if ((1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) &&
                       (Dest == AMBA_DSP_PREV_DEST_VOUT1)) {
                HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &VprocInfo);
                if (VprocInfo.PinUsage[DSP_VPROC_PIN_PREVB] > 0U) {
                    if (1U == DSP_GetBit(VprocInfo.PinUsage[DSP_VPROC_PIN_PREVB], i, 1U)) {
                        if (YuvStrmInfo->BufNum > 0U) {
                            MaxNum = MAX2_16(MaxNum, YuvStrmInfo->BufNum);
                        } else {
                            MaxNum = MAX2_16(MaxNum, CV2X_MAX_PREV_FB_NUM);
                        }
                        MaxAuxNum = MaxNum;
                    }
                }
            } else {
                // keep MaxNum the same
            }
        } else {
            if ((1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, VOUT_IDX_A, 1U)) &&
                (Dest == AMBA_DSP_PREV_DEST_VOUT0)) {
                if (YuvStrmInfo->BufNum > 0U) {
                    MaxNum = MAX2_16(MaxNum, YuvStrmInfo->BufNum);
                } else {
                    MaxNum = MAX2_16(MaxNum, CV2X_MAX_PREV_FB_NUM);
                }
                MaxAuxNum = MaxNum;
            } else if ((1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, VOUT_IDX_B, 1U)) &&
                       (Dest == AMBA_DSP_PREV_DEST_VOUT1)) {
                if (YuvStrmInfo->BufNum > 0U) {
                    MaxNum = MAX2_16(MaxNum, YuvStrmInfo->BufNum);
                } else {
                    MaxNum = MAX2_16(MaxNum, CV2X_MAX_PREV_FB_NUM);
                }
                MaxAuxNum = MaxNum;
            } else {
                // keep MaxNum the same
            }
        }
#else
        if ((1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, VOUT_IDX_A, 1U)) &&
            (Dest == AMBA_DSP_PREV_DEST_VOUT0)) {
            if (YuvStrmInfo->BufNum > 0U) {
                MaxNum = MAX2_16(MaxNum, YuvStrmInfo->BufNum);
            } else {
                MaxNum = MAX2_16(MaxNum, CV2X_MAX_PREV_FB_NUM);
            }
            MaxAuxNum = MaxNum;
        } else if ((1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, VOUT_IDX_B, 1U)) &&
                   (Dest == AMBA_DSP_PREV_DEST_VOUT1)) {
            if (YuvStrmInfo->BufNum > 0U) {
                MaxNum = MAX2_16(MaxNum, YuvStrmInfo->BufNum);
            } else {
                MaxNum = MAX2_16(MaxNum, CV2X_MAX_PREV_FB_NUM);
            }
            MaxAuxNum = MaxNum;
        } else {
            // keep MaxNum the same
        }
#endif
    }
    *pNum = MaxNum;
    *pAuxNum = MaxAuxNum;
}

UINT32 HL_GetPinOutDestBufNum(UINT16 Dest, UINT16 VprocPin, UINT16 *pNum, UINT16 *pAuxNum)
{
    //AMBA_DSP_PREV_DEST_NULL / AMBA_DSP_PREV_DEST_DUMMY
    if ((Dest != AMBA_DSP_PREV_DEST_VOUT0) &&
        (Dest != AMBA_DSP_PREV_DEST_VOUT1) &&
        (Dest != AMBA_DSP_PREV_DEST_PIN)) {
        *pNum = DSP_VPROC_FB_NUM_DISABLE;
        *pAuxNum = DSP_VPROC_FB_NUM_DISABLE;
    //AMBA_DSP_PREV_DEST_PIN
    } else if (Dest == AMBA_DSP_PREV_DEST_PIN) {
        HL_GetPinOutDestBufNumPin(VprocPin, pNum, pAuxNum);
    //AMBA_DSP_PREV_DEST_VOUT0 / AMBA_DSP_PREV_DEST_VOUT1
    } else {
        HL_GetPinOutDestBufNumVout(Dest, pNum, pAuxNum);
    }

    return OK;
}

UINT32 HL_GetPinOutDestChFmt(UINT16 Dest, UINT16 VprocPin, UINT8 *pFmt)
{
    (void)VprocPin;
    *pFmt = DSP_YUV_420;

    //AMBA_DSP_PREV_DEST_NULL / AMBA_DSP_PREV_DEST_DUMMY
    if ((Dest != AMBA_DSP_PREV_DEST_VOUT0) &&
        (Dest != AMBA_DSP_PREV_DEST_VOUT1) &&
        (Dest != AMBA_DSP_PREV_DEST_PIN)) {
        *pFmt = DSP_YUV_420;
    //AMBA_DSP_PREV_DEST_PIN
    } else if (Dest == AMBA_DSP_PREV_DEST_PIN) {
        *pFmt = DSP_YUV_420;
    //AMBA_DSP_PREV_DEST_VOUT0 / AMBA_DSP_PREV_DEST_VOUT1
    } else {
        UINT16 YuvStrmIdx;
        CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
        CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
        CTX_VPROC_INFO_s VprocInfo = {0};

        HL_GetResourcePtr(&Resource);
        for (YuvStrmIdx=0U; YuvStrmIdx<Resource->YuvStrmNum; YuvStrmIdx++) {
            HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);

            // Only Vout YuvStreams use common buffers
            if (0U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) {
                continue;
            }

            /*
            * NonEffect Pipeline
            *   - means the YuvBuffer to Destination
            * Effect Pipeline
            *   - means the YuvBuffer from which VprocPin
            *     postp_main_ : keep as DSP_YUV_420
            *     postp_Pip_ : keep as DSP_YUV_420
            *     prev_com0(DEST_VOUT0) : From PrevC, could use 420/422, check by use input
            *     prev_com1(DEST_VOUT1) : From PrevB, could use 420/422, check by use input
            */

            if (1U == HL_GetEffectEnableOnYuvStrm(YuvStrmIdx)) {
                if ((1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) &&
                    (Dest == AMBA_DSP_PREV_DEST_VOUT0)) {
                    HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &VprocInfo);
                    if (VprocInfo.PinUsage[DSP_VPROC_PIN_PREVC] > 0U) {
                        if (1U == DSP_GetBit(VprocInfo.PinUsage[DSP_VPROC_PIN_PREVC], YuvStrmIdx, 1U)) {
                            *pFmt = YuvFmtTable[YuvStrmInfo->YuvBuf.DataFmt];
                        }
                    }
                } else if ((1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) &&
                           (Dest == AMBA_DSP_PREV_DEST_VOUT1)) {
                    HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &VprocInfo);
                    if (VprocInfo.PinUsage[DSP_VPROC_PIN_PREVB] > 0U) {
                        if (1U == DSP_GetBit(VprocInfo.PinUsage[DSP_VPROC_PIN_PREVB], YuvStrmIdx, 1U)) {
                            *pFmt = YuvFmtTable[YuvStrmInfo->YuvBuf.DataFmt];
                        }
                    }
                } else {
                    // keep as DSP_YUV_420
                }
            } else {
                if ((1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, VOUT_IDX_A, 1U)) &&
                    (Dest == AMBA_DSP_PREV_DEST_VOUT0)) {
                    *pFmt = YuvFmtTable[YuvStrmInfo->YuvBuf.DataFmt];
                } else if ((1U == DSP_GetU16Bit(YuvStrmInfo->DestVout, VOUT_IDX_B, 1U)) &&
                           (Dest == AMBA_DSP_PREV_DEST_VOUT1)) {
                    *pFmt = YuvFmtTable[YuvStrmInfo->YuvBuf.DataFmt];
                } else {
                    // keep as DSP_YUV_420
                }
            }
        }
    }

    return OK;
}

UINT32 HL_GetViewZonePinFbNum(UINT16 ViewZoneId, UINT16 VprocPin, UINT16 *pNum, UINT16 *pAuxNum, UINT8 IsExtFb)
{
    UINT32 Rval = OK;
#ifdef UCODE_SUPPORT_PREVFILTER_REARRANGE
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    UINT16 YuvStrmIdx = 0xFFFFU;

    HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);
    DSP_Bit2U16Idx(VprocInfo.PinUsage[VprocPin], &YuvStrmIdx);

    if (VprocPin == DSP_VPROC_PIN_MAIN) {
        /*
         * 2022/12/8, In CV2x, HL use same mctf relative buffer for mctf ref/cmpr/uncmpr.
         *              Hence, we at least need 3 buffer.
         *              If Mctf disabled, we can further reduce buf number.
         */
        if (YuvStrmIdx != 0xFFFFU) {
            HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);
            *pNum = YuvStrmInfo->BufNum;
            if (*pNum == 0U) {
                *pNum = CV2X_MAX_MAIN_FB_NUM;
            }

            /* 2021/05/03, DSP support disable main-me if we don't need it at all */
            if (0U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U)) {
                *pAuxNum = DSP_VPROC_FB_NUM_DISABLE;
            } else {
                *pAuxNum = *pNum;
            }
        } else {
            *pNum = CV2X_MAX_MAIN_FB_NUM;
            *pAuxNum = *pNum;
        }
    } else if (YuvStrmIdx == 0xFFFFU) {
        // No one want it, set FB as Disable
        *pNum = DSP_VPROC_FB_NUM_DISABLE;
        *pAuxNum = *pNum;
    } else {
        HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);
        if (IsExtFb == 1U) {
            if (YuvStrmInfo->BufNum > 0U) {
                *pNum = YuvStrmInfo->BufNum;
            } else {
                *pNum = CV2X_MAX_PREV_FB_NUM;
            }
        } else {
            if (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) {
                /* If Prev filter outputs to Vout, it actually uses Vout common buffers
                 * disable Prev fb to reduce dram usage */
                *pNum = DSP_VPROC_FB_NUM_DISABLE;
            } else if (YuvStrmInfo->Layout.NumChan > 1U) {
                /* When PrevA is EffectChan, it uses postp_pip */
                *pNum = DSP_VPROC_FB_NUM_DISABLE;
            } else {
                if (YuvStrmInfo->BufNum > 0U) {
                    *pNum = YuvStrmInfo->BufNum;
                } else {
                    *pNum = CV2X_MAX_PREV_FB_NUM;
                }
            }
        }
        *pAuxNum = *pNum;
    }
#else
    if (VprocPin == DSP_VPROC_PIN_PREVA) {
        *pNum = CV2X_MAX_MAIN_FB_NUM;
    } else if (VprocPin == DSP_VPROC_PIN_PREVB) {
        *pNum = CV2X_MAX_PREV_FB_NUM;
    } else { //PrevC
        *pNum = CV2X_MAX_PREV_FB_NUM;
    }
    *pAuxNum = *pNum;
#endif
    return Rval;
}

static inline void HL_GetEncStrmVinId(UINT16 EncStrmIdx, UINT16* VinId)
{
    CTX_VIEWZONE_INFO_s *EncViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_STREAM_INFO_s StrmInfo;

    HL_GetStrmInfo(HL_MTX_OPT_ALL, EncStrmIdx, &StrmInfo);
    HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &YuvStrmInfo);
    HL_GetViewZoneInfoPtr(YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &EncViewZoneInfo);
    DSP_Bit2U16Idx(EncViewZoneInfo->SourceVin, VinId);
}

void HL_GetViewZoneVinId(UINT16 ViewZoneId, UINT16* VinId)
{
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VIN_INFO_s VinInfo = {0};
    UINT16 ViewZoneVinId = 0U;

    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
    if (ViewZoneInfo->SourceVin > 0U) {
        DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &ViewZoneVinId);
        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) {
            UINT16 EncStrmId = ViewZoneVinId;
            HL_GetEncStrmVinId(EncStrmId, &ViewZoneVinId);
        }
    } else if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
               (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
        ViewZoneVinId = VIN_IDX_INVALID;
    } else {
        ViewZoneVinId = VIN_IDX_INVALID;
    }

    if (ViewZoneInfo->SourceTdIdx != VIEWZONE_SRC_TD_IDX_NULL) {
        HL_GetVinInfo(HL_MTX_OPT_ALL, ViewZoneVinId, &VinInfo);
        if (ViewZoneInfo->SourceTdIdx == 0U) {
            // physical
            *VinId = VinInfo.TimeDivisionVinId[0U][ViewZoneInfo->SourceTdIdx];
        } else {
            // Virtual
            *VinId = VinInfo.TimeDivisionVinId[0U][ViewZoneInfo->SourceTdIdx] + AMBA_DSP_MAX_VIN_NUM;
        }
    } else {
        *VinId = ViewZoneVinId;
    }
}

void HL_GetViewZonePhysVinId(const CTX_VIEWZONE_INFO_s *pViewZoneInfo, UINT16 *pPhysVinId)
{
    UINT16 SrcVinId;
    UINT8 IsVirtVin;

    if ((pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
        (pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
        *pPhysVinId = VIN_IDX_INVALID;
    } else if ((pViewZoneInfo->IsUpStrmSrc > (UINT8)0U) &&
        (pViewZoneInfo->UpStrmSrcType == (UINT8)VZ_UPSTRMSRC_TYPE_FROM_VZ)) {
        HL_GetViewZoneVinId(pViewZoneInfo->UpStrmSrcId, &SrcVinId);

        if (SrcVinId != VIN_IDX_INVALID) {
            (void)HL_IsVirtualVinIdx(SrcVinId, &IsVirtVin);
            if (IsVirtVin == 0U) {
                *pPhysVinId = SrcVinId;
            } else {
                (void)HL_GetTimeDivisionVirtVinInfo(SrcVinId, pPhysVinId);
                if (*pPhysVinId == DSP_VIRT_VIN_IDX_INVALID) {
                    *pPhysVinId = 0U;
                }
            }
        } else {
            *pPhysVinId = VIN_IDX_INVALID;
        }
    } else {
        DSP_Bit2U16Idx(pViewZoneInfo->SourceVin, pPhysVinId);
    }
}

UINT32 HL_GetVinViewZoneId(UINT16 VinId, UINT16 *pViewZoneId)
{
    UINT32 Rval = OK;
    UINT16 i, ViewZoneVinId = VIN_IDX_INVALID;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetResourcePtr(&Resource);
    for (i = 0U; i < Resource->MaxViewZoneNum; i++) {
        if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
            continue;
        }
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        //skip, MemInput viewzone
        if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC)) {
            continue;
        }
        if (ViewZoneInfo->SourceVin > 0U) {
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &ViewZoneVinId);
        }
        if (ViewZoneVinId == VinId) {
            *pViewZoneId = i;
            break;
        }
    }

    return Rval;
}

void HL_GetGroupAliveViewZoneId(UINT8 GrpID,
                                UINT16 MasterVinId,
                                UINT32 DisableViewZoneBitMask,
                                UINT16 *pMasterViewZoneId,
                                UINT16 *pNumVproc)
{
    UINT16 i, ViewGrpIdx, NumVproc = 0U;
    UINT16 VzVinIdx = 0U;
    UINT16 MasterViewZoneId = AMBA_DSP_MAX_VIEWZONE_NUM;
    UINT32 GrpViewZoneBitMask = 0U;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    /* Find all the ViewZones in GrpID */
    for (i=0; i<Resource->MaxViewZoneNum; i++) {
        if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
            continue;
        }

        (void)HL_GetVprocGroupIdx(i, &ViewGrpIdx, 0/*IsStlProc*/);
        if (GrpID == ViewGrpIdx) {
            DSP_SetBit(&GrpViewZoneBitMask, i);
            NumVproc++;
        }
    }
    /* find new master, aka smallest alive vin */
    for (i=0; i<Resource->MaxViewZoneNum; i++) {
        if (0U == DSP_GetBit(GrpViewZoneBitMask, i, 1U)) {
            continue;
        }
        if (1U == DSP_GetBit(DisableViewZoneBitMask, i, 1U)) {
            continue;
        }
        HL_GetViewZoneVinId(i, &VzVinIdx);
        if (VzVinIdx == MasterVinId) {
            continue;
        }
        HL_GetVinInfo(HL_MTX_OPT_ALL, VzVinIdx, &VinInfo);
        if (VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_ACTIVE) {
            MasterViewZoneId = i;
            break;
        }
    }

    *pMasterViewZoneId = MasterViewZoneId;
    *pNumVproc = NumVproc;
}

void HL_GetGroupFirstVprocId(const UINT16 GrpId,
                             UINT16 *pFirstVprocId)
{
    UINT16 i;
    UINT16 FirstVprocId = AMBA_DSP_MAX_VIEWZONE_NUM;
    UINT32 Rval;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);

    /* find master vproc in group, ie. 1st order(smallest ViewIdx) in Grp */
    for (i=0U; i<Resource->MaxViewZoneNum; i++) {
        UINT16 VzGrpId = 0U;
        UINT32 ViewZoneActive = DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U);

        Rval = HL_GetVprocGroupIdx(i, &VzGrpId, 0/*IsStlProc*/);
        if ((Rval != OK) || (VzGrpId != GrpId) || (ViewZoneActive == 0U)) {
            continue;
        }
        FirstVprocId = (FirstVprocId > i)? i: FirstVprocId;
    }

    *pFirstVprocId = FirstVprocId;
}

void HL_GetGroupMasterVprocId(const UINT16 GrpId,
                              UINT16 *pMasterVprocId)
{
    UINT16 i;
    UINT16 MasterVprocId = AMBA_DSP_MAX_VIEWZONE_NUM;
    UINT32 Rval;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetResourcePtr(&Resource);

    for (i=0U; i<Resource->MaxViewZoneNum; i++) {
        UINT16 VzGrpId = 0U;
        UINT32 ViewZoneActive = DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U);

        Rval = HL_GetVprocGroupIdx(i, &VzGrpId, 0/*IsStlProc*/);
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
//        AmbaLL_LogUInt5("HL_GetGroupMasterVprocId vz:%u Grp:%u ViewZoneActive:%u IsMaster:%u",
//                i, VzGrpId, ViewZoneActive, ViewZoneInfo->IsMaster, 0U);
        if ((Rval == OK) &&
            (VzGrpId == GrpId) &&
            (ViewZoneActive == 1U) &&
            (ViewZoneInfo->IsMaster == 1U)) {
            MasterVprocId = i;
            break;
        }
    }

    *pMasterVprocId = MasterVprocId;
}

#if 0
void HL_GetGroupVZMask(const UINT16 GrpId, UINT32 *pGroupVZMask)
{
    UINT16 i;
    UINT32 Rval;
    UINT32 GroupVZMask = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);

    for (i=0U; i<Resource->MaxViewZoneNum; i++) {
        UINT16 VzGrpId = 0U;
        UINT32 ViewZoneActive = DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U);

        Rval = HL_GetVprocGroupIdx(i, &VzGrpId, 0/*IsStlProc*/);
        if ((Rval == OK) &&
            (VzGrpId == GrpId) &&
            (ViewZoneActive == 1U)) {
            DSP_SetBit(&GroupVZMask, i);
        }
    }

    *pGroupVZMask = GroupVZMask;
}
#endif

static void HL_GetVinAttEvtDftMsk2ndHalf(UINT16 VinId,
                                         UINT32 *pMask,
                                         const CTX_RESOURCE_INFO_s *pResource,
                                         UINT8 IsExtPymdYuv)
{
    UINT16 i;
    UINT32 ViewZoneVinId = 0U;
    UINT8 IsExtLndtYuv = 0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    for (i = 0U; i < pResource->MaxViewZoneNum; i++) {
        if (0U == DSP_GetBit(pResource->ViewZoneActiveBit, i, 1U)) {
            continue;
        }
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
            continue;
        }
        DSP_Bit2Idx(ViewZoneInfo->SourceVin, &ViewZoneVinId);
        if (ViewZoneVinId == (UINT32)VinId) {
            if (ViewZoneInfo->LndtIsExtMem > 0U) {
                IsExtLndtYuv++;
                break;
            }
        }
    }

    if (IsExtPymdYuv > 0U) {
        *pMask |= ((UINT32)1U<<DSP_BIT_POS_EXT_HIER_0);
    }

    if (IsExtLndtYuv > 0U) {
        *pMask |= ((UINT32)1U<<DSP_BIT_POS_EXT_LN_DEC);
    }
}

/**
 * Get default Mask to keep sustain able events taking effect
 * @param [In] VinId
 * @param [In] ViewZoneId
 * @param [Out] pMask pointer to Mask value
 * @return ErrorCode
 */
//UINT32 HL_GetVinAttachEventDefaultMask(UINT16 VinId, UINT16 ViewZoneId, UINT32 *pMask)
UINT32 HL_GetVinAttachEventDefaultMask(UINT16 VinId, UINT32 *pMask)
{
    UINT8 IsExtPymdYuv = 0U;
    UINT16 i;
    UINT32 Rval = OK;
    UINT32 ViewZoneVinId = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

//FIXME, single Vin contains multiple ViewZone
//(void)ViewZoneId;

    if (pMask == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        *pMask = 0;
        HL_GetResourcePtr(&Resource);

        /* Check if this Vin contains ViewZone with external Hierarchy */
        for (i = 0U; i < Resource->MaxViewZoneNum; i++) {
            if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
                continue;
            }
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
            if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
                (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
                continue;
            }
            DSP_Bit2Idx(ViewZoneInfo->SourceVin, &ViewZoneVinId);
            if (ViewZoneVinId == (UINT32)VinId) {
                if (ViewZoneInfo->PymdIsExtMem > 0U) {
                    IsExtPymdYuv++;
                    break;
                }
            }
        }

        HL_GetVinAttEvtDftMsk2ndHalf(VinId, pMask, Resource, IsExtPymdYuv);
    }

    return Rval;
}

UINT32 HL_GetSystemPymdMaxWindow(UINT16 *Width, UINT16 *Height)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT16 i = 0U;

    if ((Width == NULL) || (Height == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);

        for (i=0U; i<Resource->MaxViewZoneNum; i++) {
            *Width = (*Width > Resource->MaxHierWidth[i])? *Width: Resource->MaxHierWidth[i];
            *Height = (*Height > Resource->MaxHierHeight[i])? *Height: Resource->MaxHierHeight[i];
        }
    }

    return Rval;
}

UINT32 HL_GetViewZonePymdMaxWindow(UINT16 *Width, UINT16 *Height, UINT16 ViewZoneId)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    if ((Width == NULL) || (Height == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);
//        AmbaLL_LogUInt5("HL_GetViewZonePymdMaxWindow ViewZoneId[%d] MaxHierWidth:%u MaxHierHeight:%u",
//                ViewZoneId, Resource->MaxHierWidth[ViewZoneId], Resource->MaxHierHeight[ViewZoneId], 0U, 0U);
        *Width = (*Width > Resource->MaxHierWidth[ViewZoneId])? *Width: Resource->MaxHierWidth[ViewZoneId];
        *Height = (*Height > Resource->MaxHierHeight[ViewZoneId])? *Height: Resource->MaxHierHeight[ViewZoneId];
    }

    return Rval;
}

UINT32 HL_GetSystemLndtMaxWindow(UINT16 *Width, UINT16 *Height)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT16 i = 0U;

    if ((Width == NULL) || (Height == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);

        for (i=0U; i<Resource->MaxViewZoneNum; i++) {
            *Width = (*Width > Resource->MaxLndtWidth[i])? *Width: Resource->MaxLndtWidth[i];
            *Height = (*Height > Resource->MaxLndtHeight[i])? *Height: Resource->MaxLndtHeight[i];
        }
    }

    return Rval;
}

UINT32 HL_GetSystemHdrSetting(UINT8 *MaxHdrExpNumMiunsOne, UINT8 *LinearCeEnable, UINT16 *MaxHdrBlendHeight)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i = 0U;

    if ((MaxHdrExpNumMiunsOne == NULL) || (LinearCeEnable == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);

        for (i=0U; i<Resource->MaxViewZoneNum; i++) {
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
            *MaxHdrExpNumMiunsOne = (*MaxHdrExpNumMiunsOne > ViewZoneInfo->HdrBlendNumMinusOne)? *MaxHdrExpNumMiunsOne: ViewZoneInfo->HdrBlendNumMinusOne;
            *LinearCeEnable |= ViewZoneInfo->LinearCe;
            *MaxHdrBlendHeight = (*MaxHdrBlendHeight > ViewZoneInfo->HdrBlendHeight)? *MaxHdrBlendHeight: ViewZoneInfo->HdrBlendHeight;
        }
    }

    return Rval;
}

UINT32 HL_GetSystemHdrSettingOnVin(UINT16 VinId, UINT8 *MaxHdrExpNumMiunsOne, UINT8 *LinearCeEnable, UINT16 *MaxHdrBlendHeight)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i = 0U, VinIdx = 0U;

    if ((MaxHdrExpNumMiunsOne == NULL) || (LinearCeEnable == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);

        for (i=0U; i<Resource->MaxViewZoneNum; i++) {
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
            if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
                (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
                continue;
            }
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinIdx);
            if (VinIdx == VinId) {
                *MaxHdrExpNumMiunsOne = (*MaxHdrExpNumMiunsOne > ViewZoneInfo->HdrBlendNumMinusOne)? *MaxHdrExpNumMiunsOne: ViewZoneInfo->HdrBlendNumMinusOne;
                *LinearCeEnable |= ViewZoneInfo->LinearCe;
                *LinearCeEnable |= (ViewZoneInfo->InputMuxSelSrc >= 1U)? 1U: 0U;
                *MaxHdrBlendHeight = (*MaxHdrBlendHeight > ViewZoneInfo->HdrBlendHeight)? *MaxHdrBlendHeight: ViewZoneInfo->HdrBlendHeight;
            }
        }
    }

    return Rval;
}

UINT32 HL_GetSystemMctfSetting(UINT8 *pMctfEnable, UINT8 *pMctsEnable, UINT8 *pMctfCmprEnable)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i = 0U;

    if ((pMctfEnable == NULL) || (pMctsEnable == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);

        for (i=0U; i<Resource->MaxViewZoneNum; i++) {
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
            if (0U == DSP_GetBit(ViewZoneInfo->FlowBypassOption, FLOW_BYPASS_MCTF_IDX, 1U)) {
                *pMctfEnable |= 1U;
                if (ViewZoneInfo->MctfCmpr > 0U) {
                    *pMctfCmprEnable |= 1U;
                }
            }

            if (0U == DSP_GetBit(ViewZoneInfo->FlowBypassOption, FLOW_BYPASS_MCTS_IDX, 1U)) {
                *pMctsEnable |= 1U;
            }
        }
    }

    return Rval;
}

UINT32 HL_GetSystemWarpDmaSetting(UINT16 *pLumaDmaSize, UINT16 *pChromaDmaSize)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT16 i = 0U;

    if ((pLumaDmaSize == NULL) || (pChromaDmaSize == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);

        for (i=0U; i<Resource->MaxViewZoneNum; i++) {
            *pLumaDmaSize = (*pLumaDmaSize > Resource->MaxWarpDma[i])? *pLumaDmaSize: Resource->MaxWarpDma[i];
        }
        *pChromaDmaSize = *pLumaDmaSize;
    }

    return Rval;
}

static inline UINT32 CalcVideoTile(UINT16 Width,
                                   UINT16 TileWidth,
                                   UINT16 Overlap,
                                   UINT16 TileAlignNum,
                                   UINT16 *TileNum)
{
    UINT32 Rval = OK;
    UINT8 IsWhileOne = (UINT8)1U, ExitILoop;
    UINT16 i = 0U, CalcTileWidth = 0U;
    UINT16 AlignWidth;

    /* Initial TileNum */
    *TileNum = (UINT16)(((Width + TileWidth) - 1U)/TileWidth);

    /* Consider TileOverlap */
    if ((Overlap > 0U) &&
        (*TileNum > 1U)) {
        while (IsWhileOne > 0U) {
            ExitILoop = (UINT8)0U;
            for (i = 0U; i < *TileNum; i++) {
                CalcTileWidth = Width/(*TileNum);

                if ((i == 0U) ||
                    (i == (*TileNum - 1U))) {
                    AlignWidth = CalcTileWidth + Overlap;
                    AlignWidth = ALIGN_NUM16(AlignWidth, TileAlignNum);
                    if (AlignWidth > TileWidth) {
                        ExitILoop = (UINT8)1U;
                    }
                } else {
                    AlignWidth = (UINT16)(CalcTileWidth + (UINT16)(2U*Overlap));
                    AlignWidth = ALIGN_NUM16(AlignWidth, TileAlignNum);
                    if (AlignWidth > TileWidth) {
                        ExitILoop = (UINT8)1U;
                    }
                }
                if (ExitILoop == (UINT8)1U) {
                    break;
                }
            }

            if (ExitILoop == (UINT8)1U) {
                *TileNum += 1U;
                if (*TileNum > MAX_TILE_NUM) {
                    AmbaLL_LogUInt5("CalcVdoTile Force TileNum[%d]", MAX_TILE_NUM, 0U, 0U, 0U, 0U);
                    *TileNum = MAX_TILE_NUM;
                    IsWhileOne = (UINT8)0U;
                }
            } else {
                IsWhileOne = (UINT8)0U;
            }
        }
    }

    return Rval;
}

#define TILE_ALIGN_NUM  (128U)  // based on ucode's internal logic

UINT32 HL_CalcVideoTile(UINT16 Width, UINT16 TileWidth, UINT16 *TileNum, UINT16 Overlap)
{
    UINT32 Rval = OK;

    if (Width == 0U) {
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("HL_CalcVideoTile W %d", 0U, 0U, 0U, 0U, 0U);
#endif
        *TileNum = 0U;
        Rval = DSP_ERR_0001;
    } else if (TileWidth == 0U) {
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("HL_CalcVideoTile T_W %d", 0U, 0U, 0U, 0U, 0U);
#endif
        *TileNum = 0U;
        Rval = DSP_ERR_0001;
    } else {
        UINT16 TileAlignNum = TILE_ALIGN_NUM;

        Rval = CalcVideoTile(Width, TileWidth, Overlap, TileAlignNum, TileNum);
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("HL_CalcVideoTile W:%u T_W:%u N:%d O:%u",
                Width, TileWidth, *TileNum, Overlap, 0U);
#endif
    }

    return Rval;
}

UINT32 HL_CalcVideoTileViewZone(UINT16 ViewZoneId,
                                UINT16 Width,
                                UINT16 TileWidth,
                                UINT16 Overlap,
                                UINT16 *TileNum)
{
    UINT32 Rval = OK;

    if (Width == 0U) {
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("HL_CalcVideoTile W %d", 0U, 0U, 0U, 0U, 0U);
#endif
        *TileNum = 0U;
        Rval = DSP_ERR_0001;
    } else if (TileWidth == 0U) {
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("HL_CalcVideoTile T_W %d", 0U, 0U, 0U, 0U, 0U);
#endif
        *TileNum = 0U;
        Rval = DSP_ERR_0001;
    } else {
        UINT16 TileAlignNum;
        CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

        HL_GetResourcePtr(&Resource);
        /* ucode use 32 align when raw_tile_alignment_mode = 1 */
        if ((Resource->MaxHorWarpComp[ViewZoneId] > 0U) &&
            (Resource->MaxHorWarpComp[ViewZoneId] < 64U)) {
            TileAlignNum = 32U;
        } else {
            TileAlignNum = TILE_ALIGN_NUM;
        }
        Rval = CalcVideoTile(Width, TileWidth, Overlap, TileAlignNum, TileNum);
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("HL_CalcVideoTile W:%u T_W:%u N:%d O:%u",
                Width, TileWidth, *TileNum, Overlap, 0U);
#endif
    }

    return Rval;
}

static inline UINT32 ForceC2yTileMinimum(UINT16 Width, UINT16 *TileNum)
{
    UINT32 Rval = OK;
    UINT16 TileNumMin = (Width / SEC2_MAX_OUT_WIDTH_INTERNAL) + 1U;

    if (*TileNum < TileNumMin) {
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("ForceC2yTileMinimum w:%u t:%u/%u ",
                Width, *TileNum, TileNumMin, 0U, 0U);
#endif
        *TileNum = TileNumMin;
        Rval = DSP_ERR_0000;
    }
    return Rval;
}

static inline UINT32 ForceC2yTileWidth(UINT16 Width, UINT8 FixedOverlap, UINT16 *TileNum, UINT16 *Overlap)
{
    UINT32 Rval = OK;
    UINT16 TileWidth;
    UINT16 OverlapPadding = *Overlap;

    (void)HL_CalcVideoTileWidth(Width, *TileNum, &TileWidth);
    if (*TileNum > 2U) {
        OverlapPadding += *Overlap;
    }
    if ((UINT16)(TileWidth + (UINT16)OverlapPadding) > SEC2_MAX_OUT_WIDTH_SMEM) {
        if (FixedOverlap == 0U) {
            OverlapPadding = SEC2_MAX_OUT_WIDTH_SMEM - (Width / *TileNum);
            if (*TileNum > 2U) {
                OverlapPadding = OverlapPadding >> 1U;
            }
#ifdef DEBUG_TILE_CALC
            AmbaLL_LogUInt5("ForceC2yTileWidth w:%u n:%u t_w:%u o:%u/%u ",
                    Width, *TileNum, TileWidth, *Overlap, OverlapPadding);
#endif
            *Overlap = OverlapPadding;
        } else {
#ifdef DEBUG_TILE_CALC
            AmbaLL_LogUInt5("ForceC2yTileWidth w:%u n+:%u t_w:%u o:%u/%u ",
                    Width, *TileNum, TileWidth, *Overlap, OverlapPadding);
#endif
            *TileNum += 1U;
        }

        Rval = DSP_ERR_0001;
    }

    return Rval;
}

static inline UINT32 CheckPrevFromYuvStreamInfo(UINT16 ViewZoneId,
                                                UINT16 MainWidth,
                                                UINT16 NumYuvStream,
                                                UINT16 TileWidth,
                                                UINT8 FixedOverlap,
                                                UINT16 *TileNum,
                                                UINT16 *Overlap)
{
    UINT32 Rval = OK;
    UINT16 i, j, Found = 0U, IsUpsampling = 0U, NumOfOverlaps = 0U;
    UINT16 MaxPrevOutWidth = 0U, PrevCropSrcWidth = 0U;
    UINT32 TileOutWidth, TileWidthMinusOverlap, UpdatedTileWidth;
    UINT32 NewOverlap = (UINT32)*Overlap;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

    for (i = 0; i < NumYuvStream; i++) {
        HL_GetYuvStrmInfoPtr(i, &YuvStrmInfo);
        for (j = 0; j < YuvStrmInfo->Layout.NumChan; j++) {
            IsUpsampling = (YuvStrmInfo->Layout.ChanCfg[j].Window.Width > YuvStrmInfo->Layout.ChanCfg[j].ROI.Width)? (UINT16)1U: (UINT16)0U;
            if ((ViewZoneId == YuvStrmInfo->Layout.ChanCfg[j].ViewZoneId) &&
                (MainWidth != YuvStrmInfo->Layout.ChanCfg[j].ROI.Width) &&
                (IsUpsampling == 1U)) {
                Found = 1U;
                if (MaxPrevOutWidth < YuvStrmInfo->Layout.ChanCfg[j].Window.Width) {
                    MaxPrevOutWidth = YuvStrmInfo->Layout.ChanCfg[j].Window.Width;
                    PrevCropSrcWidth = YuvStrmInfo->Layout.ChanCfg[j].ROI.Width;
                }
            }
        }
    }

    if ((Found == 1U) &&
        (PrevCropSrcWidth > 0U) && (MaxPrevOutWidth > 0U)) {
        /* The input tile width contains the overlaps */
        if (TileWidth < PrevCropSrcWidth) {
            TileOutWidth = (UINT32)TileWidth*MaxPrevOutWidth;
            TileOutWidth = TileOutWidth/(UINT32)PrevCropSrcWidth;
        } else {
            TileOutWidth = (UINT32)MaxPrevOutWidth;
        }

        if ((UINT16)TileOutWidth > PREVB_MAX_WIDTH) {
            if (FixedOverlap == 0U) {
                UpdatedTileWidth = (UINT32)PREVB_MAX_WIDTH*(UINT32)PrevCropSrcWidth;
                UpdatedTileWidth = UpdatedTileWidth/(UINT32)MaxPrevOutWidth;
                NumOfOverlaps = (*TileNum>2U)? (UINT16)2U: (UINT16)1U;
                TileWidthMinusOverlap = (UINT32)TileWidth - ((UINT32)(*Overlap)*(UINT32)NumOfOverlaps);
                if (UpdatedTileWidth > TileWidthMinusOverlap) {
                    NewOverlap = (UpdatedTileWidth-TileWidthMinusOverlap)/NumOfOverlaps;
                } else {
                    /* It suggested that YuvStream setting exceeds maximal preview pin out width
                     * and it cannot be avoided by reducing overlap */
                }
#ifdef DEBUG_TILE_CALC
                AmbaLL_LogUInt5("CheckPrevFromYuvStreamInfo w:%u n:%u t_w:%u o:%u/%u ",
                        MainWidth, *TileNum, TileWidth, *Overlap, NewOverlap);
                AmbaLL_LogUInt5("    MaxPrevOutWidth %u PrevCropSrcWidth %u TileOutWidth:%u ",
                        MaxPrevOutWidth, PrevCropSrcWidth, TileOutWidth, 0U, 0U);
#endif
                *Overlap = (UINT16)NewOverlap;
            } else {
#ifdef DEBUG_TILE_CALC
                AmbaLL_LogUInt5("CheckPrevFromYuvStreamInfo w:%u n+:%u t_w:%u o:%u/%u ",
                        MainWidth, *TileNum, TileWidth, *Overlap, NewOverlap);
#endif
                *TileNum += 1U;
            }

            Rval = DSP_ERR_0001;
        }
    }

    return Rval;
}

#if 0
static inline UINT32 ForceC2yTileAlign(UINT16 Width,
                                       UINT16 TileWidthExp,
                                       UINT16 TileNum,
                                       UINT16 *Overlap)
{
    //e.g. w=2016, t=2, exp=4(16), o=128
    UINT32 Rval = OK;
    UINT16 Truncate;
    UINT16 InpOverlap = *Overlap;
    //1006
    UINT16 TileContentWidth = Width / TileNum;
    UINT16 LastTileStart, TileWidth;

//    AmbaLL_LogUInt5("ForceC2yTileWidthAlign w:%u e:%u n:%u o:%u",
//            Width, TileWidthExp, TileNum, InpOverlap, 0U);
    if ((TileWidthExp > 2U) && (TileWidthExp < 9U)) {
        Truncate = (UINT16)1U<<TileWidthExp;
    } else {
        Truncate = (UINT16)1U<<WARP_GRID_EXP;
    }
    //1006 - 128 --> 880
    LastTileStart = TRUNCATE_16(((TileContentWidth * (TileNum - 1U)) - InpOverlap), Truncate);
    //1136 = 2016 - 880
    TileWidth = Width - LastTileStart;

    if (IS_ALIGN_NUM((UINT32)TileWidth, TILE_WIDTH_ALIGN) == 0U) {
        //1152 = 1136 align32
        UINT32 AlignTileWidth = ALIGN_NUM((UINT32)TileWidth, TILE_WIDTH_ALIGN);
        //864 = 2016-1152
        UINT16 TileStart = Width - (UINT16)AlignTileWidth;
        //144 = 1008-864
        UINT16 AlignOverlap = (TileContentWidth * (TileNum - 1U)) - TileStart;
        //456 = (1921 - 1008) / 2
        UINT16 OverlapMax = (SEC2_MAX_OUT_WIDTH_INTERNAL - TileContentWidth) >> 1U;

        //increase overlap
        if (AlignOverlap < OverlapMax) {
            *Overlap = AlignOverlap;
        //decrease
        } else {
            UINT16 TruncateOverlap;
            //1120 = 1136 truncate32
            UINT16 TruncateTileWidth = TRUNCATE_16(TileWidth, TILE_WIDTH_ALIGN);
            //864 = 2016-1152
            TileStart = Width - TruncateTileWidth;
            //112 = 1008-864
            TruncateOverlap = (TileContentWidth * (TileNum - 1U)) - TileStart;
            *Overlap = TruncateOverlap;
        }
        Rval = DSP_ERR_0001;
    } else {
        //keep as input overlap
    }

    return Rval;
}
#endif

static inline UINT32 ForceC2yTileTruncate(UINT16 Width,
                                          UINT16 TileWidthExp,
                                          UINT16 TileNum,
                                          UINT16 *Overlap,
                                          UINT16 *MaxTileWidth)
{
    //e.g. w=2016, t=2, exp=4(16), o=128
    UINT32 Rval = OK;
    UINT16 TileTruncate;
    UINT16 InpOverlap = *Overlap;
    //1006
    UINT16 TileContentWidth = Width / TileNum;
    UINT16 LastTileStart, TileWidth;
    UINT16 SecondTileStart, SecondTileStop;

    if ((TileWidthExp > 2U) && (TileWidthExp < 9U)) {
        TileTruncate = (UINT16)1U<<TileWidthExp;
    } else {
        TileTruncate = (UINT16)1U<<WARP_GRID_EXP;
    }
    //1006 - 128 --> 880
    LastTileStart = TRUNCATE_16((UINT16)((TileContentWidth * (TileNum - 1U)) - InpOverlap), TileTruncate);
    //1136 = 2016 - 880
    TileWidth = Width - LastTileStart;
    if (IS_ALIGN_NUM((UINT32)TileWidth, TILE_WIDTH_ALIGN) == 0U) {
        //1120 = 1136 truncate32
        UINT16 TruncateTileWidth = TRUNCATE_16(TileWidth, TILE_WIDTH_ALIGN);
        //864 = 2016-1152
        UINT16 TileStart = Width - TruncateTileWidth;
        //112 = 1008-864
        UINT16 TruncateOverlap = (UINT16)(TileContentWidth * (TileNum - 1U)) - TileStart;
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("ForceC2yTileTruncate w:%u n:%u t_w:%u o:%u/%u ",
                Width, TileNum, TileWidth, *Overlap, TruncateOverlap);
#endif
        *Overlap = TruncateOverlap;
        *MaxTileWidth = TruncateTileWidth;
        Rval = DSP_ERR_0001;
    } else {
        //overlap keeps the same
        *MaxTileWidth = TileWidth;
    }
    /*
     * When the tile number is greater than 2, those tiles in the middle must be the widest because:
     *  (1) they contain one more overlap than the first or the last tile
     *  (2) they also need to follow the alignment restriction
     */
    if (TileNum > 2U) {
        SecondTileStart = TRUNCATE_16(TileContentWidth - InpOverlap, TileTruncate);
        SecondTileStop = ALIGN_NUM16(((TileContentWidth << 1UL) + InpOverlap), TILE_WIDTH_ALIGN);
        *MaxTileWidth = SecondTileStop - SecondTileStart;
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("Multiple-tile: SecondTile Start %d Stop %d TileWidth %d",
                SecondTileStart, SecondTileStop, *MaxTileWidth, 0U, 0U);
#endif
    }
    return Rval;
}

static inline UINT16 CalcTileWidthSmem(UINT16 Width, UINT16 TileNum, UINT16 Overlap)
{
    UINT16 TileWidth;
    UINT16 TileWidthNoneOverlap;

    (void)HL_CalcVideoTileWidth(Width, TileNum, &TileWidthNoneOverlap);
    TileWidth = TileWidthNoneOverlap + (Overlap << 1);

    return TileWidth;
}

UINT32 HL_CalcVideoTileC2Y(UINT16 ViewZoneId,
                           UINT16 Width,
                           UINT16 MaxTileWidth,
                           UINT32 TileWidthExp,
                           UINT8 FixedOverlap,
                           UINT8 ChkSmem,
                           UINT16 *TileNum,
                           UINT16 *Overlap)
{
    UINT32 Rval = OK;

    if (Width == 0U) {
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("HL_CalcVideoTileC2Y W %d", 0U, 0U, 0U, 0U, 0U);
#endif
        *TileNum = 0U;
        Rval = DSP_ERR_0001;
    } else if (MaxTileWidth == 0U) {
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("HL_CalcVideoTileC2Y T_W %d", 0U, 0U, 0U, 0U, 0U);
#endif
        *TileNum = 0U;
        Rval = DSP_ERR_0001;
    } else {
        CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
        UINT16 NewOverlap = *Overlap;
        UINT16 NewTileNum, ActualTileWidth = MaxTileWidth;
        UINT16 TileAlignNum;

        if (ViewZoneId != 0xFFU) {
            HL_GetResourcePtr(&Resource);
            /* ucode use 32 align when raw_tile_alignment_mode = 1 */
            if ((Resource->MaxHorWarpComp[ViewZoneId] > 0U) &&
                (Resource->MaxHorWarpComp[ViewZoneId] < 64U)) {
                TileAlignNum = 32U;
            } else {
                TileAlignNum = TILE_ALIGN_NUM;
            }
        } else {
            TileAlignNum = TILE_ALIGN_NUM;
        }

        Rval = CalcVideoTile(Width, MaxTileWidth, NewOverlap, TileAlignNum, TileNum);
        if (ForceC2yTileMinimum(Width, TileNum) != OK) {
            (void)CalcVideoTile(Width, MaxTileWidth, NewOverlap, TileAlignNum, TileNum);
#ifdef DEBUG_TILE_CALC
            AmbaLL_LogUInt5("CalcVdoTile ForceM w:%u t_w:%u o:%u t:%u/%u",
                    Width, MaxTileWidth, *Overlap, *TileNum, NewTileNum);
#endif
        }
        NewTileNum = *TileNum;
        if (ForceC2yTileWidth(Width, FixedOverlap, &NewTileNum, &NewOverlap) != OK) {
            /*
             * Re-calculate tile number with new TileNum and Overlap to confirm that
             * current setting doesn't exceed maximal tile width.
             * After ForceC2yTileWidth, TileNum could get larger or Overlap could get smaller
             */
//            (void)CalcVideoTile(Width, MaxTileWidth, &NewTileNum, NewOverlap);
#ifdef DEBUG_TILE_CALC
            AmbaLL_LogUInt5("CalcVdoTile r w:%u o:%u->%u n:%u->%u",
                    Width, *Overlap, NewOverlap, *TileNum, NewTileNum);
#endif
        }

        if (FixedOverlap == 0U) {
           if (ForceC2yTileTruncate(Width, (UINT16)TileWidthExp, NewTileNum, &NewOverlap, &ActualTileWidth) != OK) {
               /*
                * Re-calculate tile number with new TileNum and Overlap to confirm that
                * current setting doesn't exceed maximal tile width.
                * After ForceC2yTileTruncate, TileNum could get larger or Overlap could get smaller
                */
                (void)CalcVideoTile(Width, MaxTileWidth, NewOverlap, TileAlignNum, TileNum);
#ifdef DEBUG_TILE_CALC
                AmbaLL_LogUInt5("CalcVdoTile ForceT w:%u o:%u->%u n:%u->%u",
                        Width, *Overlap, NewOverlap, *TileNum, NewTileNum);
#endif
           }
        }

        HL_GetResourcePtr(&Resource);
        if (ViewZoneId != 0xFFU) {
            if (CheckPrevFromYuvStreamInfo(ViewZoneId,
                                           Width,
                                           Resource->YuvStrmNum,
                                           ActualTileWidth,
                                           FixedOverlap,
                                           &NewTileNum, &NewOverlap) != OK) {
#ifdef DEBUG_TILE_CALC
                AmbaLL_LogUInt5("CheckPrev ForceW w:%u o:%u->%u n:%u->%u",
                        Width, *Overlap, NewOverlap, *TileNum, NewTileNum);
#endif
                if (FixedOverlap == 0U) {
                    if (ForceC2yTileTruncate(Width, (UINT16)TileWidthExp, NewTileNum, &NewOverlap, &ActualTileWidth) != OK) {
                        (void)CalcVideoTile(Width, MaxTileWidth, NewOverlap, TileAlignNum, TileNum);
#ifdef DEBUG_TILE_CALC
                        AmbaLL_LogUInt5("CalcVdoTile ForceT w:%u o:%u->%u n:%u->%u",
                               Width, *Overlap, NewOverlap, *TileNum, NewTileNum);
#endif
                    }
                }
            }
        }
        if ((ChkSmem == 1U) && (Resource->MaxVprocTileWidth > 0U)) {
            if (CalcTileWidthSmem(Width, NewTileNum, NewOverlap) >
                (Resource->MaxVprocTileWidth + (Resource->MaxVprocTileOverlap << 1U))) {
#ifdef DEBUG_TILE_CALC
                AmbaLL_LogUInt5("CalcVdoTile ForceS w:%u n:%u o:%u smem:%u max:%u",
                        Width, NewTileNum, NewOverlap, CalcTileWidthSmem(Width, NewTileNum, NewOverlap),
                        (UINT32)(Resource->MaxVprocTileWidth + ((UINT32)Resource->MaxVprocTileOverlap << 1U)));
#endif
                NewTileNum += 1U;
#ifdef DEBUG_TILE_CALC
                AmbaLL_LogUInt5("CalcVdoTile ForceS w:%u o:%u->%u n:%u->%u",
                        Width, *Overlap, NewOverlap, *TileNum, NewTileNum);
#endif
            }
        }
        *Overlap = NewOverlap;
        *TileNum = NewTileNum;
    }

    return Rval;
}

UINT32 HL_CalcVideoTileWidth(UINT16 Width,
                             UINT16 TileNum,
                             UINT16 *TileWidth)
{
    UINT32 Rval = OK;
    if (Width == 0U) {
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("CalcVdoTileW W %d", Width, 0U, 0U, 0U, 0U);
#endif
        *TileWidth = 0U;
        Rval = DSP_ERR_0001;
    } else if (TileNum == 0U) {
#ifdef DEBUG_TILE_CALC
        AmbaLL_LogUInt5("CalcVdoTileW T %d", TileNum, 0U, 0U, 0U, 0U);
#endif
        *TileWidth = 0U;
        Rval = DSP_ERR_0001;
    } else if (TileNum == 1U) {
        *TileWidth = Width;
    } else {
        *TileWidth = (UINT16)(((Width + TileNum) - 1U)/TileNum);
    }

    return Rval;
}

UINT32 HL_GetBatchCmdNumber(ULONG BatchAddr)
{
    UINT32 CmdNum = 0;
    UINT32 i, Offset;

    if (BatchAddr > 0U) {
        const UINT32 *pU32;

        dsp_osal_typecast(&pU32, &BatchAddr);
        for (i=0U; i<MAX_BATCH_CMD_NUM; i++) {
            Offset = i*CMD_SIZE_IN_WORD;
            if (pU32[Offset] == MAX_BATCH_CMD_END) {
                break;
            }
            CmdNum++;
        }
    }

    return CmdNum;
}

#define VIN_REG_CFG_WAIT_TIME   (3000U) //3sec
UINT32 HL_VinPostCfgHandler(const UINT16 VinOI)
{
    UINT8 ExitWhileLoop = 0U, Update;
    UINT16 VinOILocal = VinOI;
    UINT16 VinEnbBitMask = 0U;
    UINT16 Idx, CountDown = VIN_REG_CFG_WAIT_TIME;
    UINT32 Rval = OK;
    UINT32 VinEnb = 1U;
    AMBA_DSP_VIN_POST_CONFIG_STATE_s *pVinPostCfgInfo = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    while ((VinOILocal > 0U) && (ExitWhileLoop == 0U)) {
        Update = 0U;
        for (Idx=0U; Idx<AMBA_DSP_MAX_VIN_NUM; Idx++) {
            if (1U == DSP_GetU16Bit(VinOILocal, Idx, 1U)) {
                /* Invoke CSL_VIN to get VinStatus */
                dsp_osal_dbgport_get_vin_enable_status(Idx, &VinEnb);

                if (VinEnb != 0U) {
                    DSP_ClearU16Bit(&VinOILocal, Idx);
                    DSP_SetU16Bit(&VinEnbBitMask, (UINT32)Idx);
                    Update = 1U;
                }
            }
        }

        if (Update > 0U) {
            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, AMBA_DSP_EVENT_VIN_POST_CONFIG);
            if (Rval == OK) {
                dsp_osal_typecast(&pVinPostCfgInfo, &EventBufAddr);
                (void)dsp_osal_memset(pVinPostCfgInfo, 0, sizeof(AMBA_DSP_VIN_POST_CONFIG_STATE_s));

                pVinPostCfgInfo->VinIdx = VinEnbBitMask;
                (void)DSP_GiveEvent(AMBA_DSP_EVENT_VIN_POST_CONFIG, pVinPostCfgInfo, EventBufIdx);
            } else {
                AmbaLL_LogUInt5("[VinPostEvt] Request event info pool[%d] fail[0x%x]", EVENT_INFO_POOL_PROT_BUF, Rval, 0U, 0U, 0U);
            }
        }

        if (VinOILocal > 0U) {
            (void)dsp_osal_sleep(1U);
            CountDown--;
            if (CountDown == 0U) {
                ExitWhileLoop = 1U;
            }
        } else {
            ExitWhileLoop = 1U;
        }
    }

    if (CountDown == 0U) {
        for (Idx=0U; Idx<AMBA_DSP_MAX_VIN_NUM; Idx++) {
            if (1U == DSP_GetU16Bit(VinOILocal, Idx, 1U)) {
                AmbaLL_LogUInt5("Vin[%d] RegCfg fail", Idx, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0006;
            }
        }
    }

    return Rval;
}

void HL_DisassembleVinCompression(UINT32 Compression, UINT8 *pCompressedMode, UINT8 *pCompactMode)
{
    if (DSP_GetBit(Compression, 8U, 1U)/*IsCompactMode*/ == 1U) {
        *pCompressedMode = 0U;
        *pCompactMode = (UINT8)(Compression - 255U);
    } else {
        *pCompressedMode = (UINT8)DSP_GetBit(Compression, 0U, 8U);
        *pCompactMode = 0U;
    }
}

UINT32 HL_GetCmprRawBufInfo(UINT16 Width, UINT8 CmprRate, UINT8 CmptMode,
                            UINT16 *pRawWidth, UINT16 *pRawPitch,
                            UINT16 *pMantissa, UINT16 *pBlkSz)
{
    static const UINT8 RawCmptBitLengthTable[RAW_COMPACT_NUM] = {
            [RAW_COMPACT_OFF]   = 0U,
            [RAW_COMPACT_8BIT]  = 8U, // No 8bit in CV2, but will be 5 when CV2FS
            [RAW_COMPACT_10BIT] = 10U,
            [RAW_COMPACT_12BIT] = 12U,
            [RAW_COMPACT_14BIT] = 14U,
    };
    static const RAW_BIT_RATE_t RawBitRateTable[RAW_COMPRESS_NUM] = {
            [RAW_COMPRESS_OFF]   = {.Den =  0U, .Num =  0U, .Mantissa =  0U, .Block = 0U},
            [RAW_COMPRESS_6P75]  = {.Den = 27U, .Num = 32U, .Mantissa =  6U, .Block = 4U},
            [RAW_COMPRESS_7p5]   = {.Den = 30U, .Num = 32U, .Mantissa =  6U, .Block = 2U},
            [RAW_COMPRESS_7p75]  = {.Den = 31U, .Num = 32U, .Mantissa =  7U, .Block = 4U},
            [RAW_COMPRESS_8p5]   = {.Den = 34U, .Num = 32U, .Mantissa =  7U, .Block = 2U},
            [RAW_COMPRESS_8p75]  = {.Den = 35U, .Num = 32U, .Mantissa =  8U, .Block = 4U},
            [RAW_COMPRESS_9p5]   = {.Den = 38U, .Num = 32U, .Mantissa =  8U, .Block = 2U},
            [RAW_COMPRESS_9p75]  = {.Den = 39U, .Num = 32U, .Mantissa =  9U, .Block = 4U},
            [RAW_COMPRESS_10p5]  = {.Den = 42U, .Num = 32U, .Mantissa =  9U, .Block = 2U},
            [RAW_COMPRESS_10p75] = {.Den = 43U, .Num = 32U, .Mantissa = 10U, .Block = 4U},
            [RAW_COMPRESS_11P5]  = {.Den = 46U, .Num = 32U, .Mantissa = 10U, .Block = 2U},
            [RAW_COMPRESS_11P75] = {.Den = 47U, .Num = 32U, .Mantissa = 11U, .Block = 4U},
            [RAW_COMPRESS_12P5]  = {.Den = 50U, .Num = 32U, .Mantissa = 11U, .Block = 2U},
            [RAW_COMPRESS_4P75]  = {.Den = 19U, .Num = 32U, .Mantissa =  4U, .Block = 4U},
            [RAW_COMPRESS_5P5]   = {.Den = 22U, .Num = 32U, .Mantissa =  4U, .Block = 2U},
            [RAW_COMPRESS_5P75]  = {.Den = 23U, .Num = 32U, .Mantissa =  5U, .Block = 4U},
            [RAW_COMPRESS_6P5]   = {.Den = 26U, .Num = 32U, .Mantissa =  5U, .Block = 2U}
    };
    UINT32 Rval = OK;
    RAW_BIT_RATE_t RawBitRate = {0};
    UINT16 SymbolSize = 0U, BlockAlign = 0U;
    UINT32 TempWidth;

    if (CmprRate == RAW_COMPRESS_OFF) {
        if (CmptMode == 0U) {
            RawBitRate = RawBitRateTable[CmprRate];
            *pRawWidth = Width*2U;
            *pMantissa = RawBitRate.Mantissa;
            *pBlkSz = RawBitRate.Block;
        } else {
            TempWidth = (UINT32)Width;
            TempWidth *= RawCmptBitLengthTable[CmptMode];
            TempWidth = ALIGN_NUM(TempWidth, 8U)/8U;
            *pRawWidth = (UINT16)TempWidth;
            *pMantissa = RawCmptRateTable[CmptMode].Mantissa;
            *pBlkSz = RawCmptRateTable[CmptMode].Block;
        }
    } else {
        RawBitRate = RawBitRateTable[CmprRate];

        // Aligned to block size*2 (two colors for each line)
        BlockAlign = RawBitRate.Block << 1U;
        TempWidth = (UINT32)ALIGN_NUM16(Width, BlockAlign);
        SymbolSize = 3U + (RawBitRate.Block*RawBitRate.Mantissa);

        TempWidth = ((TempWidth)/((UINT32)RawBitRate.Block << 1U))*((UINT32)SymbolSize << 1U);
        TempWidth = ALIGN_NUM(TempWidth, 32U); // in bits
        *pRawWidth = (UINT16)(TempWidth >> 3U); // in bytes

        *pMantissa = RawBitRate.Mantissa;
        *pBlkSz = RawBitRate.Block;
    }

    *pRawPitch = ALIGN_NUM16(*pRawWidth, (UINT16)DSP_BUF_ALIGNMENT);

    return Rval;
}

UINT32 HL_GetViewZoneMctfSettings(UINT16 ViewZoneId, UINT8 *pMctfEnable, UINT8 *pMctsOutEnable, UINT8 *pMctfCmpr)
{
    UINT32 Rval = OK;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    if ((pMctfEnable == NULL) || (pMctsOutEnable == NULL)) {
        Rval = DSP_ERR_0000;
    } else if (ViewZoneId >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001;
    } else {
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

        if ((ViewZoneInfo->Pipe == DSP_DRAM_PIPE_MIPI_RAW) ||
             (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_RAW_PROC)) {
            *pMctfEnable = 0U;
            *pMctsOutEnable = 0U;
            *pMctfCmpr = 0U;
        } else if (ViewZoneInfo->IsEffectChanMember == 1U) {
            /* veffect group all vproc channels MUST BE configured with full pipeline steps c2y + warp/mctf/prev
             * regardless input is yuv or sensor raw data to ensure all jobs execution order to get correct final
             * output data. */
            *pMctfEnable = 1U;
            *pMctsOutEnable = 1U;
            *pMctfCmpr = ViewZoneInfo->MctfCmpr;
        } else if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
                   (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
            //No need Mctf/Mcts, using warp out
            *pMctfEnable = 0U;
            *pMctsOutEnable = 0U;
            *pMctfCmpr = 0U;
        } else {
            if (0U == DSP_GetBit(ViewZoneInfo->FlowBypassOption, FLOW_BYPASS_MCTF_IDX, 1U)) {
                *pMctfEnable = 1U;
                *pMctfCmpr = ViewZoneInfo->MctfCmpr;
                if (0U == DSP_GetBit(ViewZoneInfo->FlowBypassOption, FLOW_BYPASS_MCTS_DRAM_OUT_IDX, 1U)) {
                    *pMctsOutEnable = 1U;
                } else {
                    *pMctsOutEnable = 0U;
                }
            } else {
                *pMctfEnable = 0U;
                *pMctsOutEnable = 0U;
                *pMctfCmpr = 0U;
            }
        }
    }
    return Rval;
}

UINT32 HL_GetEncMaxTimeLapseWindow(UINT16 *Width, UINT16 *Height)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    UINT16 i = 0U, YuvStrmIdx = 0U;
    UINT32 EncIdx = 0U;
    UINT8 IsLiveviewEncStrm = (UINT8)0U;

    if ((Width == NULL) || (Height == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);

        for (i=0U; i<Resource->MaxEncodeStream; i++) {
            if (1U == DSP_GetU16Bit(Resource->MaxStrmFmt[i], ENC_STRM_FMT_TIMELAPSE_BIT_IDX, ENC_STRM_FMT_TIMELAPSE_LEN)) {
                for (YuvStrmIdx=0U; YuvStrmIdx<Resource->YuvStrmNum; YuvStrmIdx++) {
                    HL_GetYuvStrmInfoPtr(YuvStrmIdx, &YuvStrmInfo);
                    if (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U)) {
                        DSP_Bit2Idx((UINT32)YuvStrmInfo->DestEnc, &EncIdx);
                        if (EncIdx == i) {
                            IsLiveviewEncStrm = (UINT8)1U;
                            if (1U == DSP_GetU16Bit(Resource->MaxStrmFmt[i], ENC_STRM_FMT_TIMELAPSE_BIT_IDX, ENC_STRM_FMT_TIMELAPSE_LEN)) {
                                *Width = (*Width > YuvStrmInfo->MaxWidth)? *Width: YuvStrmInfo->MaxWidth;
                                *Height = (*Height > YuvStrmInfo->MaxHeight)? *Height: YuvStrmInfo->MaxHeight;
                            }
                        }
                    }
                }

                if (IsLiveviewEncStrm == (UINT8)0U) {
                    if (1U == DSP_GetU16Bit(Resource->MaxStrmFmt[i], ENC_STRM_FMT_TIMELAPSE_BIT_IDX, ENC_STRM_FMT_TIMELAPSE_LEN)) {
                        *Width = (*Width > Resource->MaxExtMemWidth[i])? *Width: Resource->MaxExtMemWidth[i];
                        *Height = (*Height > Resource->MaxExtMemHeight[i])? *Height: Resource->MaxExtMemHeight[i];
                    }
                }
            }
        }
    }

    return Rval;
}

UINT32 HL_GetEncGrpCmprStroSetting(UINT16 StrmIdx, UINT16 *pCsLsb, UINT16 *pCsResIdx, ULONG *pCsCfgULAddr)
{
    UINT16 i, j;
    UINT32 Rval = OK;
    CTX_ENC_GRP_INFO_s EncGrpInfo = {0};

    if (StrmIdx >= AMBA_DSP_MAX_STREAM_NUM) {
        Rval = DSP_ERR_0001;
    } else if ((pCsLsb == NULL) || (pCsResIdx == NULL) || (pCsCfgULAddr == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        /* Sweep GrpCfg */
        for (i = 0U; i < AMBA_DSP_MAX_ENC_GRP_NUM; i++) {
            HL_GetEncGrpInfo(HL_MTX_OPT_ALL, i, &EncGrpInfo);
            if (1U == DSP_GetU16Bit(EncGrpInfo.Purpose, DSP_ENC_GRP_PURPOSE_RC_IDX, 1U)) {
                for (j = 0U; j < EncGrpInfo.StrmNum; j++) {
                    if (StrmIdx == EncGrpInfo.StrmIdx[j]) {
                        if (EncGrpInfo.MonoIncrement == 0U) {
                            if ((j % EncGrpInfo.Stride) == 0U) {
                                *pCsLsb = 0U;
                            } else {
                                *pCsLsb = 1U;
                            }
                            *pCsResIdx = j/EncGrpInfo.Stride;
                        } else {
                            if ((j % EncGrpInfo.Stride) == 0U) {
                                *pCsLsb = 1U;
                            } else {
                                *pCsLsb = 0U;
                            }
                            *pCsResIdx = ((EncGrpInfo.StrmNum/EncGrpInfo.Stride) - 1U) - (j/EncGrpInfo.Stride);
                        }

                        //TBD, not sure CsCfg shall be put when MonoIncrement
                        if ((*pCsLsb == 0U) &&
                            (*pCsResIdx == 0U)) {
                            *pCsCfgULAddr = EncGrpInfo.RcCfgAddr;
                        }
                        break;
                    }
                }
            }
        }
    }

    return Rval;
}

UINT32 HL_GetVinNum(UINT16 *Num) {
    UINT32 Rval = OK, Cnt = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    DSP_Bit2Cnt(Resource->MaxVinBit, &Cnt);
    *Num = (UINT16)Cnt;

    return Rval;
}

UINT32 HL_CheckPrevROI(AMBA_DSP_WINDOW_s ROI, AMBA_DSP_WINDOW_s Main) {
    UINT32 Rval = OK;

    if (((ROI.Width + ROI.OffsetX) > Main.Width) ||
        ((ROI.Height + ROI.OffsetY) > Main.Height)){
        Rval = DSP_ERR_0001;
    }

    return Rval;
}

UINT32 HL_GetSmemToWarpEnable(void)
{
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i;
    UINT32 Enable = 0U;

    HL_GetResourcePtr(&Resource);
    for (i=0U; i<Resource->MaxViewZoneNum; i++) {
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        if (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_EFCY) {
            Enable = 1U;
            break;
        }
    }
    return Enable;
}

UINT32 HL_GetViewZoneSmemToWarpEnable(UINT16 ViewZoneId)
{
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT32 Enable = 0U;

    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
    if (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_EFCY) {
        Enable = 1U;
    }
    return Enable;
}

void HL_GetViewZoneDownStrmViewZoneId(UINT16 ViewZoneId, UINT16 *pDownStrmId, UINT8 *pDataBindType)
{
    UINT16 i;
    UINT8 ExitLoop;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VIEWZONE_INFO_s *pTmpViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VIEWZONE_INFO_s *pUpstrmViewZoneInfo = HL_CtxViewZoneInfoPtr;

    *pDownStrmId = DSP_VIEWZONE_IDX_INVALID;

    HL_GetResourcePtr(&pResource);
    HL_GetViewZoneInfoPtr(ViewZoneId, &pViewZoneInfo);

    if (pViewZoneInfo->IsPostStrm > 0U) {
        *pDownStrmId = pViewZoneInfo->UpStrmSrcId;
        *pDataBindType = pViewZoneInfo->UpStrmDataBind;
    } else {
        ExitLoop = (UINT8)0U;
        for (i = 0U; i < pResource->MaxViewZoneNum; i++) {
            if (i == ViewZoneId) {
                continue;
            } else {
                HL_GetViewZoneInfoPtr(i, &pTmpViewZoneInfo);

                if ((pTmpViewZoneInfo->IsPostStrm == 0U) &&
                    (pTmpViewZoneInfo->IsUpStrmSrc > 0U)) {
                    if (ViewZoneId == pTmpViewZoneInfo->UpStrmSrcId) {
                        *pDownStrmId = i;
                        *pDataBindType = pViewZoneInfo->UpStrmDataBind;
                        ExitLoop = (UINT8)1U;
                    } else {
                        HL_GetViewZoneInfoPtr(pTmpViewZoneInfo->UpStrmSrcId, &pUpstrmViewZoneInfo);
                        if ((pUpstrmViewZoneInfo->IsPostStrm > 0U) &&
                            (ViewZoneId == pUpstrmViewZoneInfo->UpStrmSrcId)) {
                            *pDownStrmId = i;
                            *pDataBindType = pTmpViewZoneInfo->UpStrmDataBind;
                        }
                    }
                }
            }

            if (ExitLoop == (UINT8)1U) {
                break;
            }
        }
    }
}

void HL_GetViewZoneUpStrmViewZoneId(UINT16 ViewZoneId, UINT16 *pUpStrmId, UINT8 *pDataBindType)
{
    UINT16 i;
    UINT8 ExitLoop;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VIEWZONE_INFO_s *pTmpViewZoneInfo = HL_CtxViewZoneInfoPtr;

    *pUpStrmId = DSP_VIEWZONE_IDX_INVALID;

    HL_GetResourcePtr(&pResource);
    HL_GetViewZoneInfoPtr(ViewZoneId, &pViewZoneInfo);

    if (pViewZoneInfo->IsUpStrmSrc > 0U) {
        *pUpStrmId = pViewZoneInfo->UpStrmSrcId;
        *pDataBindType = pViewZoneInfo->UpStrmDataBind;
    } else {
        ExitLoop = (UINT8)0U;
        for (i = 0U; i < pResource->MaxViewZoneNum; i++) {
            if (i == ViewZoneId) {
                continue;
            } else {
                HL_GetViewZoneInfoPtr(i, &pTmpViewZoneInfo);

                if ((pTmpViewZoneInfo->IsPostStrm > 0U) &&
                    (ViewZoneId == pTmpViewZoneInfo->UpStrmSrcId)) {
                        *pUpStrmId = i;
                        *pDataBindType = pTmpViewZoneInfo->UpStrmDataBind;
                        ExitLoop = (UINT8)1U;
                }
            }

            if (ExitLoop == (UINT8)1U) {
                break;
            }
        }
    }
}


UINT8 HL_GetYuvStrmIsEffectOut(UINT16 MaxChanNum, UINT16 WindowOffsetX, UINT16 WindowOffsetY, UINT8 RotateFlip)
{
    UINT8 Enable = 0U;

    if (MaxChanNum > 1U) {
        Enable = 1U;
    } else if (MaxChanNum == 1U) {
        if ((WindowOffsetX > 0U) ||
            (WindowOffsetY > 0U) ||
            (RotateFlip > 0U)) {
            Enable = 1U;
        }
    } else {
        //TBD
    }

    return Enable;
}

UINT32 HL_GetDecIdxFromVinId(UINT16 VinId, UINT16 *pIdx)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    UINT16 i, DecIdx = 0U;

    HL_GetResourcePtr(&Resource);
    for (i = 0U; i < Resource->MaxViewZoneNum; i++) {
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);

        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &DecIdx);
            HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);
            if ((VidDecInfo.YuvInVirtVinId != DSP_VIRT_VIN_IDX_INVALID) &&
                (VidDecInfo.YuvInVirtVinId == (VinId - AMBA_DSP_MAX_VIN_NUM))) {
                Rval = OK;
                break;
            } else {
                Rval = DSP_ERR_0001;
            }
        }
    }

    *pIdx = DecIdx;

    return Rval;
}

#define ZOOM_FACTOR_1_SQRT2 (46341U) // 1/sqrt(2), 16.16
UINT32 HL_GetOctaveSize(UINT8 OctaveMode, UINT16 Input, UINT16 *pOutput)
{
    UINT32 Rval = OK;

    if (pOutput == NULL) {
        Rval = DSP_ERR_0000;
    } else if (OctaveMode >= DSP_HIER_OCTAVE_MODE_NUM) {
        Rval = DSP_ERR_0001;
    } else {
        if (OctaveMode == DSP_HIER_HALF_OCTAVE_MODE) {
            *pOutput = (UINT16)((((UINT32)Input * ZOOM_FACTOR_1_SQRT2) + ((UINT32)1U << 15U)) >> 16U);
        } else {
            *pOutput = Input >> 1U;
        }
    }

    return Rval;
}

UINT8 HL_IsVoutRotate(UINT8 VoutId)
{
    UINT8 IsRotate = 0U;
    CTX_VOUT_INFO_s VoutInfo = {0};

    if (VoutId < AMBA_DSP_MAX_VOUT_NUM) {
        HL_GetVoutInfo(HL_MTX_OPT_ALL, VoutId, &VoutInfo);
        if (HL_IS_ROTATE(VoutInfo.VideoCfg.RotateFlip) == 1U){
            IsRotate = 1U;
        }
    }

    return IsRotate;
}

#if 0
inline static UINT32 ROUND_UP_DIV(UINT32 a, UINT32 b)
{
    return ((a + b) - 1U) / b;
}

#endif
#ifndef SUPPORT_MAX_UCODE
UINT32 HL_CalHierWindow(UINT8 OctaveMode, UINT8 HierIdx,
                        const UINT16 *pMaxMainWidth, const UINT16 *pMaxMainHeight,
                        UINT16 *pHierWidth, UINT16 *pHierHeight)
{
    UINT32 Rval = OK;
    UINT16 InitialWidth = 0U, InitialHeight = 0U;

    if ((pMaxMainWidth == NULL) || (pMaxMainHeight == NULL) ||
        (pHierWidth == NULL) || (pHierHeight == NULL)) {
        Rval = DSP_ERR_0000;
    } else if ((OctaveMode >= DSP_HIER_OCTAVE_MODE_NUM) ||
               (HierIdx >= AMBA_DSP_MAX_HIER_NUM)) {
        Rval = DSP_ERR_0001;
    } else {
        /* Hier supports 2 mode:
           1. Octave mode:
              Hier order is 0/1/3/5/2/4, with downsampling of 1x/2x/4x/8x/16x/32x (2x each tier)
           2. Half octave mode:
              Hier order is 0/1/2/3/4/5, with downsampling of 1x/sqrt(2)x/2x/2*sqrt(2)x/4x/4*sqrt(2)x (sqrt(2)x each tier)
         */

        if ((HierIdx%2U) == 0U) {
            InitialWidth = *pMaxMainWidth;
            InitialHeight = *pMaxMainHeight;
        } else {
            (void)HL_GetOctaveSize(OctaveMode, *pMaxMainWidth, &InitialWidth);
            InitialWidth = ALIGN_NUM16(InitialWidth, 2U);

            (void)HL_GetOctaveSize(OctaveMode, *pMaxMainHeight, &InitialHeight);
            InitialHeight = ALIGN_NUM16(InitialHeight, 2U);
        }

        switch (HierIdx) {
            case DSP_HIER_0:
            case DSP_HIER_1:
            {
                *pHierWidth = InitialWidth;
                *pHierHeight = InitialHeight;
            }
                break;
            case DSP_HIER_2:
            case DSP_HIER_4:
            {
                if (OctaveMode == DSP_HIER_HALF_OCTAVE_MODE) {
                    *pHierWidth = ALIGN_NUM16(ROUND_UP_DIV(InitialWidth, HierIdx), 2U);
                    *pHierHeight = ALIGN_NUM16(ROUND_UP_DIV(InitialHeight, HierIdx), 2U);

                } else {
                    *pHierWidth = ALIGN_NUM16(ROUND_UP_DIV(InitialWidth, (UINT32)HierIdx << 3U), 2U);
                    *pHierHeight = ALIGN_NUM16(ROUND_UP_DIV(InitialHeight, (UINT32)HierIdx << 3U), 2U);
                }
            }
                break;
            case DSP_HIER_3:
            case DSP_HIER_5:
            {
                *pHierWidth = ALIGN_NUM16(ROUND_UP_DIV(InitialWidth, ((UINT32)HierIdx - DSP_HIER_1)), 2U);
                *pHierHeight = ALIGN_NUM16(ROUND_UP_DIV(InitialHeight, ((UINT32)HierIdx - DSP_HIER_1)), 2U);
            }
                break;
            default:
            {
                //TBD
            }
                break;
        }
    }

//    AmbaLL_LogUInt5("Octave[%d] idx[%d] W %d --> %d Init %d", OctaveMode, HierIdx, *pMaxMainWidth, *pHierWidth, InitialWidth);
//    AmbaLL_LogUInt5("                   H %d --> %d Init %d", *pMaxMainHeight, *pHierHeight, InitialHeight, 0U, 0U);
    return Rval;
}
#endif

UINT16 HL_GetMinimalActiveVinId(void)
{
    UINT16 MinVinId;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    if (Resource->VinBit > 0U) {
        DSP_Bit2U16Idx(Resource->VinBit, &MinVinId);
    } else {
        MinVinId = 0U;
    }

    return MinVinId;
}

UINT32 Gcd(const UINT32 M, const UINT32 N)
{
    UINT32 Tmp = 1U;
    UINT32 Val0 = M;
    UINT32 Val1 = N;

    while (Tmp != 0U) {
        Tmp = Val0 % Val1;
        Val0 = Val1;
        Val1 = Tmp;
    }

    return Val0;
}

UINT32 HL_ConvertDspFrameRate(AMBA_DSP_FRAME_RATE_s FrmRate, UINT32 *pDspFrmRate)
{
    UINT32 Rval = OK, Value;
    UINT32 TimeScaleTruncate2U30, NewTimeScale;
    UINT32 FrateGcd;

    /* EncFrameRate
        B[31] Scan format. 0 is progressive, 1 is interlace
        B[30] NumUnitsInTick. 0 is 1000,  1 is 1001
        B[29:0] TimeScale

        if NumUnitsInTick=0, ucode will use TimeScale / 1000
        when user input 30/1, we will use 30/1*1000 = 30000
        if input 30000/1000, we will use 30000/1000*1000 = 30000
     */

    if (pDspFrmRate == NULL) {
        AmbaLL_LogUInt5("HL_ConvertDspFrameRate: Null input", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
//FIXME, Frame divisor

        *pDspFrmRate = 0U; //reset

        Value = (FrmRate.Interlace == 1U)? ((UINT32)1U<<31U): (UINT32)0U;
        Value |= (FrmRate.NumUnitsInTick == 1001U)? ((UINT32)1U<<30U): (UINT32)0U;

        if ((FrmRate.NumUnitsInTick == 1001U) || (FrmRate.NumUnitsInTick == 1000U)) {
            NewTimeScale = FrmRate.TimeScale;
        } else {
            FrateGcd = Gcd(FrmRate.TimeScale, FrmRate.NumUnitsInTick);
            NewTimeScale = ((FrmRate.TimeScale / FrateGcd) * 1000U);
        }

        TimeScaleTruncate2U30 = (NewTimeScale & 0x3FFFFFFFU);
        Value |= TimeScaleTruncate2U30;
        *pDspFrmRate = Value;
    }

    return Rval;
}

