/**
*  @file SvcBRateCalc.c
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
*
*/

#include "AmbaDef.h"
// #include "AmbaDSP.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"
// #include "AmbaDSP_Capability.h"

// #include "SvcErrCode.h"
#include "SvcLog.h"

#include "SvcResCfg.h"
#include "SvcBRateCalc.h"
#include "SvcEnc.h"
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "../../../main/src/config/inc/SvcUserPref.h"
#endif

#define SVC_LOG_BRC     "BRC"

#define PIXEL_SIZE      0x000000000000000CU   /* 12 bit/pixel */
#define COMPRESS_RATIO  0x000000000000000CU   /* 0.012 */

typedef struct {
    UINT32    Mode;
    UINT32    BitRate;
    UINT32    MaxBitRate;
} SVC_BRATE_CALC_INFO_s;

static SVC_BRATE_CALC_INFO_s Info[CONFIG_ICAM_MAX_REC_STRM] = {0U};

static UINT32 BRateCalc(UINT32 StreamId)
{
    #define MAX_CALC_BITRATE    (100000000U)
    #define MIN_CALC_BITRATE    (1000000U)

    UINT32           Rval = 0U;
    UINT64           Width, Height, TimeScale, NumUnitsInTick, BitRate;
    SVC_ENC_INFO_s   EncInfo;
    const SVC_RES_CFG_s *pSvcResCfg = SvcResCfg_Get();

    SvcEnc_InfoGet(&EncInfo);

    /* AVC bitrate  = Width * Hieght * (PixelSize) * FrameRate * CompressRatio */
    /* HEVC bitrate = (AVC bitrate) * (HEVC ratio of AVC)                      */

    if (CheckBits(pSvcResCfg->RecBits, ((UINT32)1U << StreamId)) != 0U) {
        Width          = (UINT64)pSvcResCfg->RecStrm[StreamId].StrmCfg.Win.Width;
        Height         = (UINT64)pSvcResCfg->RecStrm[StreamId].StrmCfg.Win.Height;
        TimeScale      = (UINT64)pSvcResCfg->RecStrm[StreamId].RecSetting.FrameRate.TimeScale;
        NumUnitsInTick = (UINT64)pSvcResCfg->RecStrm[StreamId].RecSetting.FrameRate.NumUnitsInTick;

        BitRate   = (Width * Height * TimeScale * PIXEL_SIZE * COMPRESS_RATIO) / (NumUnitsInTick * (UINT64)1000U);
#if defined(AMBA_DSP_ENC_CODING_TYPE_H265)
        {
            #define HEVC_RATIO      0x00000000000002BCU   /* HEVC/AVC = 0.7 */

            if (EncInfo.pStrmCfg[StreamId].CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
                BitRate = (BitRate * HEVC_RATIO) / (UINT64)1000U;
            }
        }
#endif

        BitRate = GetRoundDownValU64(BitRate, (UINT64)1000000U) * (UINT64)1000000U;

        /* max. bit rate is 100M bps */
        if ((UINT64)MAX_CALC_BITRATE < BitRate) {
            BitRate = MAX_CALC_BITRATE;
        }

        /* min. bit rate is 1M bps */
        if (BitRate < (UINT64)MIN_CALC_BITRATE) {
            BitRate = MIN_CALC_BITRATE;
        }

        Rval = (UINT32)BitRate;
    }

    return Rval;
}


/**
* bit-rate calculation get based on record stream
* @param [in] StreamId id of record stream
* @return bit-rate
*/
UINT32 SvcBRateCalc_Get(UINT32 StreamId)
{
    UINT32 Rval = 0U;

    if (StreamId < (UINT32)CONFIG_ICAM_MAX_REC_STRM) {
        if (Info[StreamId].Mode == SVC_BR_CALC_MODE_ASSIGN) {
            Rval = Info[StreamId].BitRate;
        } else {
            Rval = BRateCalc(StreamId);
        }
    }

    return Rval;
}

/**
* evaluation of max. bit-rate
* @param [in] StreamId index of record stream
* @param [in] pSvcResCfg setting of record stream
* @param [in] ResCfgNum number of confg
* @return max. bit-rate
*/
UINT32 SvcBRateCalc_MaxBRateEval(UINT32 StreamId, const SVC_RES_CFG_s *pSvcResCfg, UINT32 ResCfgNum)
{
    UINT32  i, Rval = 0U;
    UINT64  Width, Height, TimeScale, NumUnitsInTick, BitRate;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    SvcUserPref_Get(&pSvcUserPref);
#endif

    for (i = 0U; i < ResCfgNum; i++) {
        if (CheckBits(pSvcResCfg[i].RecBits, ((UINT32)1U << StreamId)) != 0U) {
            Width          = (UINT64)pSvcResCfg[i].RecStrm[StreamId].StrmCfg.Win.Width;
            Height         = (UINT64)pSvcResCfg[i].RecStrm[StreamId].StrmCfg.Win.Height;
            TimeScale      = (UINT64)pSvcResCfg[i].RecStrm[StreamId].RecSetting.FrameRate.TimeScale;
            NumUnitsInTick = (UINT64)pSvcResCfg[i].RecStrm[StreamId].RecSetting.FrameRate.NumUnitsInTick;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
            if (StreamId == 0) {
                BitRate = pSvcUserPref->InternalCamera.MainStream.bitrate * (UINT64)1000000U;
            } else if (StreamId == 1) {
                BitRate = pSvcUserPref->InternalCamera.SecStream.bitrate * (UINT64)1000000U;
            } else if (StreamId == 2) {
                BitRate = pSvcUserPref->ExternalCamera.MainStream.bitrate * (UINT64)1000000U;
            } else if (StreamId == 3) {
                BitRate = pSvcUserPref->ExternalCamera.SecStream.bitrate * (UINT64)1000000U;
            } else {
                BitRate = 0;
            }
            if (BitRate == 0) {
                BitRate = (Width * Height * TimeScale * PIXEL_SIZE * COMPRESS_RATIO) / (NumUnitsInTick * (UINT64)1000U);
            }
            SvcLog_OK(SVC_LOG_BRC, "Setup [%d] to %u bps", StreamId, BitRate);
#else
            BitRate   = (Width * Height * TimeScale * PIXEL_SIZE * COMPRESS_RATIO) / (NumUnitsInTick * (UINT64)1000U);
#endif
            BitRate = GetRoundUpValU64(BitRate, (UINT64)1000000U) * (UINT64)1000000U;

            if ((UINT32)BitRate > Rval) {
                Rval = (UINT32)BitRate;
            }
        }
    }

    Info[StreamId].MaxBitRate = Rval;

    return Rval;
}

/**
* max. bit-rate get
* @param [in] StreamId index of record stream
* @return max. bit-rate
*/
UINT32 SvcBRateCalc_GetMaxBRate(UINT32 StreamId)
{
    return Info[StreamId].MaxBitRate;
}

/**
* configuration of user-defined max. bit-rate
* @param [in] StreamId index of record stream
* @param [in] MaxBRate user-defined max. bit-rate (unit is MB)
* @return max. bit-rate
*/
UINT32 SvcBRateCalc_SetMaxBRate(UINT32 StreamId, UINT32 MaxBRate)
{
    UINT32 Bitrate = MaxBRate;

    Bitrate *= 1000000U;
    Info[StreamId].MaxBitRate = Bitrate;

    return Info[StreamId].MaxBitRate;
}

/**
* on-the-fly configuration of bit-rate
* @param [in] StreamId index of record stream
* @param [in] Mode calc. mode
* @param [in] BitRate bit-rate
* @return none
*/
void SvcBRateCalc_Set(UINT32 StreamId, UINT32 Mode, UINT32 BitRate)
{
    if (StreamId < (UINT32)CONFIG_ICAM_MAX_REC_STRM) {
        Info[StreamId].Mode       = Mode;
        if (Mode == SVC_BR_CALC_MODE_ASSIGN) {
            if (BitRate <= Info[StreamId].MaxBitRate) {
                Info[StreamId].BitRate = GetRoundUpValU32(BitRate, (UINT32)1000000U) * (UINT32)1000000U;
                SvcLog_OK(SVC_LOG_BRC, "Setup to %u Mbps", Info[StreamId].BitRate, 0U);
            } else {
                SvcLog_NG(SVC_LOG_BRC, "bit rate(%u) should not exceed %u: ", StreamId, Info[StreamId].MaxBitRate);
                Info[StreamId].BitRate = Info[StreamId].MaxBitRate;
            }
        }
    }
}
