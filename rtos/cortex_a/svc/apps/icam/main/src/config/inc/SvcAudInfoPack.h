/**
*  @file SvcAudInfoPack.h
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
*  @details svc information pack
*
*/

#ifndef SVC_AUD_INFOPACK_H
#define SVC_AUD_INFOPACK_H

void SvcAudInfoPack_AINInfo(UINT32 *pAinNum, AMBA_AIN_IO_CREATE_INFO_s *pAinInfo);
void SvcAudInfoPack_AINConfig(UINT32 *pAinNum, AMBA_AUDIO_INPUT_s *pAinCfg);
void SvcAudInfoPack_AENCInfo(AMBA_AUDIO_ENC_CREATE_INFO_s *pAEncInfo);
void SvcAudInfoPack_AENCConfig(AMBA_AUDIO_ENC_s *pAEncCfg, AMBA_AUDIO_SETUP_INFO_s *pAEncSetup);
void SvcAudInfoPack_AOUTInfo(AMBA_AOUT_IO_CREATE_INFO_s *pAOutInfo);
void SvcAudInfoPack_AOUTConfig(AMBA_AUDIO_OUTPUT_s *pAoutCfg);
void SvcAudInfoPack_ADECInfo(AMBA_AUDIO_DEC_CREATE_INFO_s *pADecInfo);
void SvcAudInfoPack_ADECConfig(AMBA_AUDIO_DEC_s *pADecInfo, AMBA_AUDIO_SETUP_INFO_s *pADecSetup);

#endif  /* SVC_AUD_INFOPACK_H */
