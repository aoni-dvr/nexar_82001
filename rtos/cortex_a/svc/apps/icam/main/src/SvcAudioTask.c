/**
 *  @file SvcAudioTask.c
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
 *  @details svc audio task
 *
 */

#include "AmbaTypes.h"

#include "AmbaKAL.h"
#include "AmbaDMA_Def.h"
#include "AmbaVIN_Def.h"
#include "AmbaVfs.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaAEncFlow.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaADecFlow.h"

#include "SvcLog.h"
#include "SvcTask.h"
#include "SvcPlat.h"
#include "SvcAudInfoPack.h"
#include "SvcCmdAudio.h"
#include "SvcAudioTask.h"


#define SVC_AUDIO_TASK     "SVC_AUDIO_TASK"

/**
 *  Initial audio codec and audio memory packing
 */
void SvcAudioTask_Init(UINT32 EnableDMIC)
{
    if ((g_pPlatCbEntry != NULL) && (g_pPlatCbEntry->pCbADrvInit != NULL)) {
        g_pPlatCbEntry->pCbADrvInit(CONFIG_ICAM_I2S_CHANNEL, EnableDMIC);
    }

    SvcCmdAudio_Install();
}

/**
 *  Initial audio codec and audio memory configuration
 */
void SvcAudioTask_Config(void)
{
    /* Audio memory packing */
    AMBA_AENC_FLOW_INFO_s    AudioEncInfo;
    AMBA_ADEC_FLOW_INFO_s    AudioDecInfo;
    AmbaAEncFlow_InfoGet(&AudioEncInfo);
    SvcAudInfoPack_AINConfig(AudioEncInfo.pAinNum, AudioEncInfo.pAinInfo);
    SvcAudInfoPack_AENCConfig(AudioEncInfo.pAEncInfo, AudioEncInfo.pAEncSetup);

    AmbaADecFlow_InfoGet(&AudioDecInfo);
    SvcAudInfoPack_AOUTConfig(AudioDecInfo.pAoutInfo);
    SvcAudInfoPack_ADECConfig(AudioDecInfo.pADecInfo, AudioDecInfo.pADecSetup);
    SvcLog_OK(SVC_AUDIO_TASK, "## Audio memory packing", 0U, 0U);

}
