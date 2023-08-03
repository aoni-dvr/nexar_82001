/**
 *  @file AmbaDSP_Liveview_Priv.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Implementation of SSP Liveview private control API
 *
 */

#include "AmbaDSP_Log.h"
#include "AmbaDSP_EncodeAPI.h"
#include "dsp_priv_api.h"

/**
* Liveview Pyramid config update
* @param [in]  NumViewZone number of viewzone
* @param [in]  pViewZoneId viewzone index
* @param [in]  pPyramid pyramid config information
* @param [in]  pPyramidBuf pyramid buffer config information
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_liveview_update_pymdcfg(UINT16 NumViewZone,
                                   const UINT16 *pViewZoneId,
                                   const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid,
                                   const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf,
                                   UINT64 *pAttachedRawSeq)
{
(void)NumViewZone;
(void)pViewZoneId;
(void)pPyramid;
(void)pPyramidBuf;
AmbaMisra_TouchUnused(pAttachedRawSeq);

AmbaLL_LogUInt5("AmbaDSP_LiveviewUpdatePymdCfg Error[0x%X]", DSP_ERR_0008, 0U, 0U, 0U, 0U);

    return DSP_ERR_0008;
}

UINT32 dsp_liveview_parse_vp_msg(ULONG VpMsgAddr)
{
(void)VpMsgAddr;
AmbaLL_LogUInt5("AmbaDSP_LiveviewParseVpMsg Error[0x%X]", DSP_ERR_0008, 0U, 0U, 0U, 0U);
    return DSP_ERR_0008;
}

/**
* Liveview ViewZone sideband update function, USED WHEN IDSPRX(VpMsg) ENABLE
* @param [in]  ViewZoneId viewzone index
* @param [in]  NumBand sideband number
* @param [out] pSidebandBufAddr SideBand information
* @return ErrorCode
*/
UINT32 dsp_liveview_sb_update(UINT16 ViewZoneId, UINT8 NumBand, const ULONG *pSidebandBufAddr)
{
(void)ViewZoneId;
(void)NumBand;
(void)pSidebandBufAddr;
AmbaLL_LogUInt5("AmbaDSP_LiveviewSideBandUpdate Error[0x%X]", DSP_ERR_0008, 0U, 0U, 0U, 0U);
    return DSP_ERR_0008;
}
