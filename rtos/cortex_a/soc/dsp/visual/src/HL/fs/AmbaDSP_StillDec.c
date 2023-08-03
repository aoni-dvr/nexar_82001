/**
*  @file AmbaDSP_StillDec.c
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
*  @details Implement of SSP Still Decoder related APIs
*
*/

#include "AmbaDSP_Log.h"
#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_ArmComm.h"
#include "AmbaDSP_ApiDump.h"
#include "AmbaDSP_ApiCheck.h"
#include "AmbaDSP_StillDec.h"
#include "AmbaDSP_ApiTiming.h"
#include "AmbaDSP_ApiResource.h"
#include "AmbaDSP_EncodeUtility.h"
#include "dsp_priv_api.h"

static UINT32 HL_StillDecStartChk(UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pStillDecStart(StreamIdx, pDecConfig);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pStillDecStart(StreamIdx, pDecConfig);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pStillDecStart(StreamIdx, pDecConfig);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pStillDecStart(StreamIdx, pDecConfig);
    }

    /* Logic sanity check */
    return Rval;
}

/**
* Still decode start function
* @param [in]  NumImg number of image
* @param [in]  pDecConfig decode configuration
* @return ErrorCode
*/
UINT32 dsp_still_dec_start(UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig)
{
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    CTX_VID_DEC_INFO_s DecInfo;

    Rval = HL_StillDecStartChk(StreamIdx, pDecConfig);

    /* Body */
    if (Rval == OK) {
        //cv2fs still decode need to check decode status
        //make sure decode status to idle before start
        HL_GetVidDecInfo(HL_MTX_OPT_ALL, StreamIdx, &DecInfo);

        if (DSP_GetDecState(DecInfo.StreamID) == DSP_DEC_OPM_RUN) {
            cmd_decoder_stop_t *StlStop = HL_DefCtxCmdBufPtrDecStop;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&StlStop, &CmdBufferAddr);
            StlStop->hdr.decoder_id = (UINT8)DecInfo.StreamID;
            StlStop->hdr.codec_type = DEC_CODEC_TYPE_AVC;
            StlStop->stop_mode = DEC_STOP_MODE_VOUT_STOP;
            Rval =  AmbaHL_CmdDecoderStop(WriteMode, StlStop);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval == OK) {
                Rval = DSP_WaitDecState((UINT32)DecInfo.StreamID, DEC_OPM_IDLE, MODE_SWITCH_TIMEOUT, 1U);
            } else {
                AmbaLL_LogUInt5("[Err][0x%X] decode stop failed id[%d] status[%d] ", Rval, DecInfo.StreamID, DecInfo.State, 0U, 0U);
            }
        }

        if (Rval == OK) {
            cmd_decoder_stilldec_t *StlDec = HL_DefCtxCmdBufPtrDecStlDec;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&StlDec, &CmdBufferAddr);
            StlDec->hdr.decoder_id = (UINT8)DecInfo.StreamID;
            StlDec->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H264; // CV2FS only support AVC
            (void)dsp_osal_virt2cli(pDecConfig->BitsAddr, &StlDec->bstrm_start);
            StlDec->bstrm_size = pDecConfig->BitsSize;
            (void)dsp_osal_virt2cli(pDecConfig->YuvBufAddr, &StlDec->yuv_buf_base);
            StlDec->yuv_buf_size = pDecConfig->YuvBufSize;
            Rval = AmbaHL_CmdDecoderStilldec(WriteMode, StlDec);
            HL_RelCmdBuffer(CmdBufferId);
        } else {
            AmbaLL_LogUInt5("[Err][0x%X] wait decode idle fail id[%d] status[%d] ", Rval, DecInfo.StreamID, DecInfo.State, 0U, 0U);
        }
    }

    if (Rval == OK) {
        HL_GetVidDecInfo(HL_MTX_OPT_GET, StreamIdx, &DecInfo);
        DecInfo.State = VIDDEC_STATE_RUN;
        DecInfo.BitsFormat = pDecConfig->BitsFormat;
        HL_SetVidDecInfo(HL_MTX_OPT_SET, StreamIdx, &DecInfo);
    } else {
        AmbaLL_LogUInt5("[Err][0x%X] StlDecStart", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* Still decode stop function
* @return ErrorCode
*/
UINT32 dsp_still_dec_stop(void)
{
    return ERR_NA;
}

/**
* Still decode Yuv blending function
* @param [in]  pSrc1YuvBufAddr blend source1
* @param [in]  pSrc2YuvBufAddr blend source2
* @param [in]  pDestYuvBufAddr blend destination
* @param [in]  pOperation blending option
* @return ErrorCode
*/
UINT32 dsp_still_dec_yuv2yuv(const AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                             const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                             const AMBA_DSP_STLDEC_YUV2YUV_s *pOperation)
{
(void)pSrcYuvBufAddr;
(void)pDestYuvBufAddr;
(void)pOperation;
    return ERR_NA;
}

/**
* Still decode Yuv blending function
* @param [in]  pSrc1YuvBufAddr blend source1
* @param [in]  pSrc2YuvBufAddr blend source2
* @param [in]  pDestYuvBufAddr blend destination
* @param [in]  pOperation blending option
* @return ErrorCode
*/
UINT32 dsp_still_dec_yuv_blend(const AMBA_DSP_YUV_IMG_BUF_s *pSrc1YuvBufAddr,
                               const AMBA_DSP_YUV_IMG_BUF_s *pSrc2YuvBufAddr,
                               const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                               const AMBA_DSP_STLDEC_BLEND_s *pOperation)
{
(void)pSrc1YuvBufAddr;
(void)pSrc2YuvBufAddr;
(void)pDestYuvBufAddr;
(void)pOperation;
    return ERR_NA;
}

/**
* Still decode display yuv image function
* @param [in]  VoutIdx vout index
* @param [in]  pYuvBufAddr yuv biffer address
* @param [in]  pVoutConfig vout video plane configuration
* @return ErrorCode
*/
UINT32 dsp_still_dec_disp_yuv_img(UINT8 VoutIdx,
                                  const AMBA_DSP_YUV_IMG_BUF_s *pYuvBufAddr,
                                  const AMBA_DSP_VOUT_VIDEO_CFG_s *pVoutConfig)
{
(void)VoutIdx;
(void)pYuvBufAddr;
(void)pVoutConfig;
    return ERR_NA;
}
