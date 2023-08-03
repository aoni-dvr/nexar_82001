/*
 * Copyright (c) 2022 Ambarella International LP
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
 */

#include "AmbaReg_RCT.h"
#include "AmbaReg_ScratchpadNS.h"
#include "AmbaReg_VOUT.h"

AMBA_RCT_REG_s                       *pAmbaRCT_Reg;
AMBA_SCRATCHPAD_NS_REG_s             *pAmbaScratchpadNS_Reg;
AMBA_VOUT_REG_s                      *pAmbaVout_Reg;
AMBA_VOUT_MIXER_REG_s                *pAmbaVoutMixer0_Reg;
AMBA_VOUT_DISPLAY_CONFIG_REG_s       *pAmbaVoutDisplay0_Reg;
AMBA_VOUT_MIXER_REG_s                *pAmbaVoutMixer1_Reg;
AMBA_VOUT_DISPLAY_CONFIG_REG_s       *pAmbaVoutDisplay1_Reg;
AMBA_VOUT_OSD_RESCALE_REG_s          *pAmbaVoutOsdRescale_Reg;
AMBA_VOUT_TOP_REG_s                  *pAmbaVoutTop_Reg;
AMBA_VOUT_MIPI_DSI_COMMAND_REG_s     *pAmbaVoutMipiDsiCmd0_Reg;
AMBA_VOUT_MIPI_DSI_COMMAND_REG_s     *pAmbaVoutMipiDsiCmd1_Reg;
