/**
*  @file AmbaDSP_DecodeUtility.c
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
*  @details Implement of HL decode utility APIs
*
*/

#include "AmbaDSP_Log.h"
#include "AmbaDSP_DecodeUtility.h"
#include "AmbaDSPSimilar_DecodeUtility.h"
#include "AmbaDSP_EncodeUtility.h"
#include "AmbaDSPSimilar_EncodeUtility.h"
#include "dsp_priv_api.h"

UINT8 HL_GetDecFmtTotalBit(void)
{
    UINT16 i;
    UINT8 DecBitMask = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);

    for (i=0; i<Resource->DecMaxStreamNum; i++) {
        DecBitMask |= (UINT8)Resource->DecMaxStrmFmt[i];
    }
    return DecBitMask;
}

static UINT32 FillDecFlowMaxCfgSetup(cmd_dsp_dec_flow_max_cfg_t *DecFlowSetup)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    /* 20201211, CV2FS use duplex mode for decode.
     * we shall set smaller refsmem to prevent smem shortage under UHD size
     * and UHD only allow +-16 SR
     */
    DecFlowSetup->max_frm_width = Resource->DecMaxFrameWidth;
    DecFlowSetup->max_frm_height = ALIGN_NUM16(Resource->DecMaxFrameHeight, 16U);
    if ((Resource->DecMaxFrameWidth >= UHD_WIDTH) &&
        (Resource->DecMaxFrameHeight >= UHD_HEIGHT)) {
#define UHD_REF_SMEM_WITH_16SR  (0x65AC0U)
        DecFlowSetup->ref_smem_size = UHD_REF_SMEM_WITH_16SR;
    } else {
        DecFlowSetup->ref_smem_size = 0U; //let ucode calc, default is +-32SR
    }

    DecFlowSetup->max_bit_rate = 0U; //obsoleted
    DecFlowSetup->max_frm_num_of_dpb = 0U; //default value. 5 when MaxFrmWidth > 1920. 4 when MaxFrmWidth <= 1920
    DecFlowSetup->max_resource_alloc = (UINT8)0U; //Used for TestMode,Per Adam(20200512) leave it as zero now

    if (Resource->TestFrmNumOnStage[DSP_TEST_STAGE_VDSP_1] > 0U) {
        DecFlowSetup->test_decoder_interval = Resource->TestFrmInterval[DSP_TEST_STAGE_VDSP_1]/10U; //default 100ms
    }

    return Rval;
}

UINT32 HL_FillDecFlowMaxCfgSetup(cmd_dsp_dec_flow_max_cfg_t *DecFlowSetup)
{
    UINT32 Rval;
    UINT32 CrcHL, CrcSimilar;
    const UINT32 *pU32 = NULL;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef CONFIG_ENABLE_DSP_MONITOR
    AMBA_DSP_MONITOR_CRC_CMPR_s CrcCmpr;
#endif

    Rval = FillDecFlowMaxCfgSetup(DecFlowSetup);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            UINT32 SIM_Rval;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_dsp_dec_flow_max_cfg_t *SimDecFlowSetup = SIM_DefCtxCmdBufPtrDecMax;

            dsp_osal_typecast(&pU32, &DecFlowSetup);
            CrcHL = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_dsp_dec_flow_max_cfg_t), 0U);

            SIM_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SimDecFlowSetup, &CmdBufferAddr);
            SIM_Rval = SIM_FillDecFlowMaxCfgSetup(SimDecFlowSetup);
            if (SIM_Rval == OK) {
                dsp_osal_typecast(&pU32, &SimDecFlowSetup);
                CrcSimilar = CalcCheckSum32Add(pU32, (UINT32)sizeof(cmd_dsp_dec_flow_max_cfg_t), 0U);
#ifdef CONFIG_ENABLE_DSP_MONITOR
                CrcCmpr.ModuleId = AMBA_SSP_PRINT_MODULE_ID;
                CrcCmpr.Crc0 = CrcHL;
                CrcCmpr.Crc1 = CrcSimilar;
                CrcCmpr.UUID = __LINE__;
                SIM_Rval = dsp_mon_crc_cmpr(&CrcCmpr);
                if (SIM_Rval != OK) {
                    AmbaLL_LogUInt5("dsp_mon_crc_cmpr return 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
                }
#else
                if (CrcSimilar != CrcHL) {
                    AmbaLL_LogUInt5("[ERR] Similar CRC mismatch @ %d HL 0x%x Similar 0x%x", __LINE__, CrcHL, CrcSimilar, 0U, 0U);
                }
#endif
            } else {
                AmbaLL_LogUInt5("[ERR] Similar function returned NG 0x%x", SIM_Rval, 0U, 0U, 0U, 0U);
            }
            SIM_RelCmdBuffer(CmdBufferId);
        }
    }
    return Rval;
}

UINT32 HL_FillDecBitsStreamSetup(UINT16 StreamIdx, cmd_decoder_setup_t *DecBitsStrmSetup)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, StreamIdx, &VidDecInfo);
    HL_GetResourcePtr(&Resource);

    // When multiple decode, must send "decode setup" command for each decoder.
    DecBitsStrmSetup->hdr.decoder_id = (UINT8)VidDecInfo.StreamID;
    DecBitsStrmSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H264; // CV2FS only support AVC

    //CV2FS only use default type for general case
    DecBitsStrmSetup->dec_setup_type = DEC_SETUP_TYPE_DEFAULT;

    (void)dsp_osal_virt2cli(VidDecInfo.BitsBufAddr, &DecBitsStrmSetup->bits_fifo_base);
    DecBitsStrmSetup->bits_fifo_limit = DecBitsStrmSetup->bits_fifo_base + VidDecInfo.BitsBufSize - 1U;
    DecBitsStrmSetup->rbuf_smem_size = 0U;
//visual    SetupCmd.cabac_2_recon_delay = 6U;
    DecBitsStrmSetup->err_handling_mode = DEC_ERR_HANDLING_HALT;
    /*
     * This flag decide DPB buffer size,
     * Set to 2 if we need B frame encode
     * Set to 0, DPB size is inferred from pic size and profile/level
     */
    DecBitsStrmSetup->max_frm_num_of_dpb = 1U;

//Do we need this??
//    (void)AmbaDSP_GetDecModeOnFmt(0, &DecState);
//    if ((DecState != DEC_STATE_IDLE) && (DecState != DEC_STATE_IDLE_WITH_LAST_PIC)) {
//        AmbaLL_LogUInt5("DecState(%d) != IDLE before Setup", DecState, 0U, 0U, 0U, 0U);
//    }

    DecBitsStrmSetup->enable_pic_info = 1U;

    if (StreamIdx == Resource->TestDecStrmId) {
        ULONG TestCfgAddr = VidDecInfo.BitsBufAddr - DEC_TEST_BS_FIFO_OFFSET;

        (void)dsp_osal_virt2cli(TestCfgAddr, &DecBitsStrmSetup->test_binary_daddr);
    }

    return Rval;
}

UINT32 HL_FillBitsUpdateSetup(const UINT16 StreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo, cmd_decoder_bitsfifo_update_t *DecUpdateSetup)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, StreamIdx, &VidDecInfo);

    DecUpdateSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H264; // CV2FS only support AVC
    DecUpdateSetup->hdr.decoder_id = (UINT8)VidDecInfo.StreamID;
    (void)dsp_osal_virt2cli(pBitsFifo->StartAddr, &DecUpdateSetup->bits_fifo_start);
    (void)dsp_osal_virt2cli(pBitsFifo->EndAddr, &DecUpdateSetup->bits_fifo_end);

    return Rval;
}

UINT32 HL_FillDecSpeedSetup(const UINT16 DecIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig, cmd_decoder_speed_t *SpdDirSetup)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;
    CTX_VOUT_INFO_s VoutAInfo = {0};
    CTX_VOUT_INFO_s VoutBInfo = {0};
    static const UINT16 SpeedMap[11] = {
            0x0100,  /**< playback speed 1x */
            0x0200,  /**< playback speed 2x */
            0x0400,  /**< playback speed 4x */
            0x0800,  /**< playback speed 8x */
            0x1000,  /**< playback speed 16x */
            0x2000,  /**< playback speed 32x */
            0x4000,  /**< playback speed 64x */
            0x0080,  /**< playback speed 1/2x */
            0x0040,  /**< playback speed 1/4x */
            0x0020,  /**< playback speed 1/8x */
            0x0010   /**< playback speed 1/16x */
    };

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);

    /** Setup speed */
    SpdDirSetup->hdr.decoder_id = (UINT8)VidDecInfo.StreamID;

    SpdDirSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H264; // CV2FS only support AVC
    SpdDirSetup->speed = SpeedMap[pStartConfig->SpeedIndex];    /**< AMBA_DSP_VIDDEC_SPEED_xxx */
    (void)dsp_osal_memcpy(&SpdDirSetup->direction, &pStartConfig->Direction, sizeof(UINT8)); /**< 1: backward */

    HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_A, &VoutAInfo);
    HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_B, &VoutBInfo);

    // only set time_scale & num_units_in_tick when dec only
    if (VidDecInfo.XcodeMode == AMBA_DSP_XCODE_NONE) {
        if (VoutBInfo.VideoEnable != 0U) {
            SpdDirSetup->out_strm_num_units_in_tick = VoutBInfo.MixerCfg.FrameRate.NumUnitsInTick;
            SpdDirSetup->out_strm_time_scale = VoutBInfo.MixerCfg.FrameRate.TimeScale;
        } else if (VoutAInfo.VideoEnable != 0U) {
            SpdDirSetup->out_strm_num_units_in_tick = VoutAInfo.MixerCfg.FrameRate.NumUnitsInTick;
            SpdDirSetup->out_strm_time_scale = VoutAInfo.MixerCfg.FrameRate.TimeScale;
        } else {
            SpdDirSetup->out_strm_time_scale = 0U;
            SpdDirSetup->out_strm_num_units_in_tick = 0U;
        }
    } else {
        SpdDirSetup->out_strm_time_scale = 0U;
        SpdDirSetup->out_strm_num_units_in_tick = 0U;
    }

    return Rval;
}

UINT32 HL_FillDecTrickSetup(const UINT16 DecIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickConfig, cmd_decoder_trickplay_t *TrickSetup)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);

    /* Setup trick play */
    TrickSetup->hdr.decoder_id = (UINT8)VidDecInfo.StreamID;
    TrickSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H264; // CV2FS only support AVC
    TrickSetup->mode = (UINT32)pTrickConfig->Operation;

    return Rval;
}

UINT32 HL_FillDecStartSetup(const UINT16 DecIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig, cmd_decoder_start_t *StartSetup)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);

    /** When multiple decode, must send "h264 decode" command for each decoder. */
    StartSetup->hdr.decoder_id = (UINT8)VidDecInfo.StreamID;
    StartSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H264; // CV2FS only support AVC
    (void)dsp_osal_virt2cli(VidDecInfo.BitsBufAddr, &StartSetup->bits_fifo_start);
    StartSetup->bits_fifo_end = StartSetup->bits_fifo_start + pStartConfig->PreloadDataSize - 1U;
    StartSetup->first_frame_display = pStartConfig->FirstDisplayPTS;

    return Rval;
}
