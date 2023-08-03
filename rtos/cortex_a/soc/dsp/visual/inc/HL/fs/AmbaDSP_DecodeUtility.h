/**
*  @file AmbaDSP_DecodeUtility.h
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
*  @details Definitions for the APIs of SSP Decoder
*
*/

#ifndef AMBADSP_DECODEUTILITY_H
#define AMBADSP_DECODEUTILITY_H

#include "cmd_msg_dsp.h"
#include "cmd_msg_prod_dv.h"
#include "AmbaDSP_VideoDec.h"


/* Config for VideoDec */
extern UINT32 HL_FillDecFlowMaxCfgSetup(cmd_dsp_dec_flow_max_cfg_t *DecFlowSetup);
extern UINT32 HL_FillDecBitsStreamSetup(UINT16 StreamIdx, cmd_decoder_setup_t *DecBitsStrmSetup);
extern UINT32 HL_FillBitsUpdateSetup(const UINT16 StreamIdx,
                                     const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo,
                                     cmd_decoder_bitsfifo_update_t *DecUpdateSetup);
 extern UINT32 HL_FillDecSpeedSetup(const UINT16 DecIdx,
                                   const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig,
                                   cmd_decoder_speed_t *SpdDirSetup);
extern UINT32 HL_FillDecTrickSetup(const UINT16 DecIdx,
                                   const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickConfig,
                                   cmd_decoder_trickplay_t *TrickSetup);
extern UINT32 HL_FillDecStartSetup(const UINT16 DecIdx,
                                   const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig,
                                   cmd_decoder_start_t *StartSetup);
extern UINT8 HL_GetDecFmtTotalBit(void);

#endif //AMBADSP_DECODEUTILITY_H
