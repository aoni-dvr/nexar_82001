/*
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
 */

#ifndef AMBA_RTSL_VOUT_H
#define AMBA_RTSL_VOUT_H

#ifndef AMBA_VOUT_DEF_H
#include "AmbaVOUT_Def.h"
#endif

#ifndef AMBA_REG_VOUT_H
#include "AmbaReg_VOUT.h"
#endif


/*
 * Defined in AmbaRTSL_VOUT_LCD.c
 */
UINT32 AmbaRTSL_Vout0InitDispConfig(void);
UINT32 AmbaRTSL_Vout0GetDispConfigAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout0GetDigitalCscAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout0SetVinVoutSync(UINT32 EnableFlag);

UINT32 AmbaRTSL_VoutMipiDsiEnable(UINT32 MipiDsiMode, UINT32 ColorOrder, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutMipiDsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
UINT32 AmbaRTSL_VoutMipiDsiGetStatus(AMBA_VOUT_MIPI_DSI_CONFIG_s *pMipiDsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutMipiDsiSetCsc(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 MipiDsiMode);

UINT32 AmbaRTSL_VoutMipiDsiDcsWrite(UINT32 DcsCmd, UINT32 NumParam, const UINT8 * pParam);
UINT32 AmbaRTSL_VoutMipiDsiNormalWrite(UINT32 NumParam, const UINT8 * pParam);
UINT32 AmbaRTSL_VoutMipiDsiSetBlankPkt(const AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s * pMipiDsiBlankPktCtrl);

UINT32 AmbaRTSL_Vout1InitDispConfig(void);
UINT32 AmbaRTSL_Vout1GetDispConfigAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout1GetDigitalCscAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout1SetVinVoutSync(UINT32 EnableFlag);

UINT32 AmbaRTSL_VoutMipiCsiEnable(UINT32 MipiCsiMode, UINT32 ColorOrder, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutMipiCsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
UINT32 AmbaRTSL_VoutMipiCsiGetStatus(AMBA_VOUT_MIPI_CSI_CONFIG_s *pMipiCsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutMipiCsiSetCsc(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 MipiCsiMode);

void AmbaRTSL_VoutChannelSelect(UINT32 VoutChannel);
UINT32 AmbaRTSL_VoutMipiCsiSetMaxFrame(UINT32 MaxFrameCount);
UINT32 AmbaRTSL_Vout0SetTimeout(UINT32 Timeout);
UINT32 AmbaRTSL_Vout1SetTimeout(UINT32 Timeout);
UINT32 AmbaRTSL_Vout0CheckFrameFrozen(UINT32 *pStatus);
UINT32 AmbaRTSL_Vout1CheckFrameFrozen(UINT32 *pStatus);
UINT32 AmbaRTSL_Vout0GetStatus(UINT32 *pStatus);
UINT32 AmbaRTSL_Vout1GetStatus(UINT32 *pStatus);
UINT32 AmbaRTSL_VoutGetTopLevelStatus(UINT32 *pStatus);
UINT32 AmbaRTSL_VoutSetTopLevelMask(UINT32 Mask);

UINT32 AmbaRTSL_VoutMipiDsiSetBtaSrc(UINT32 BtaEnSrc);
UINT32 AmbaRTSL_VoutMipiDsiGetAckData(UINT32 *pAckData);

#if defined(CONFIG_VOUT_ASIL)
/* Shadow functions */
UINT32 RTSL_Vout0InitDispConfig(void);
UINT32 RTSL_Vout0GetDispConfigAddr(ULONG *pVirtAddr);
UINT32 RTSL_Vout0GetDigitalCscAddr(ULONG *pVirtAddr);
UINT32 RTSL_Vout0SetVinVoutSync(UINT32 EnableFlag);

UINT32 RTSL_VoutMipiDsiEnable(UINT32 MipiDsiMode, UINT32 ColorOrder, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 RTSL_VoutMipiDsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
UINT32 RTSL_VoutMipiDsiGetStatus(AMBA_VOUT_MIPI_DSI_CONFIG_s *pMipiDsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 RTSL_VoutMipiDsiSetCsc(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 MipiDsiMode);

UINT32 RTSL_VoutMipiDsiDcsWrite(UINT32 DcsCmd, UINT32 NumParam, const UINT8 * pParam);
UINT32 RTSL_VoutMipiDsiNormalWrite(UINT32 NumParam, const UINT8 * pParam);
UINT32 RTSL_VoutMipiDsiSetBlankPkt(const AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s * pMipiDsiBlankPktCtrl);

UINT32 RTSL_Vout1InitDispConfig(void);
UINT32 RTSL_Vout1GetDispConfigAddr(ULONG *pVirtAddr);
UINT32 RTSL_Vout1GetDigitalCscAddr(ULONG *pVirtAddr);
UINT32 RTSL_Vout1SetVinVoutSync(UINT32 EnableFlag);

UINT32 RTSL_VoutMipiCsiEnable(UINT32 MipiCsiMode, UINT32 ColorOrder, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 RTSL_VoutMipiCsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
UINT32 RTSL_VoutMipiCsiGetStatus(AMBA_VOUT_MIPI_CSI_CONFIG_s *pMipiCsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 RTSL_VoutMipiCsiSetCsc(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 MipiCsiMode);

void RTSL_VoutChannelSelect(UINT32 VoutChannel);
UINT32 RTSL_VoutMipiCsiSetMaxFrame(UINT32 MaxFrameCount);
UINT32 RTSL_Vout0SetTimeout(UINT32 Timeout);
UINT32 RTSL_Vout1SetTimeout(UINT32 Timeout);
UINT32 RTSL_Vout0CheckFrameFrozen(UINT32 *pStatus);
UINT32 RTSL_Vout1CheckFrameFrozen(UINT32 *pStatus);
UINT32 RTSL_Vout0GetStatus(UINT32 *pStatus);
UINT32 RTSL_Vout1GetStatus(UINT32 *pStatus);
UINT32 RTSL_VoutGetTopLevelStatus(UINT32 *pStatus);
UINT32 RTSL_VoutSetTopLevelMask(UINT32 Mask);

UINT32 RTSL_VoutMipiDsiSetBtaSrc(UINT32 BtaEnSrc);
UINT32 RTSL_VoutMipiDsiGetAckData(UINT32 *pAckData);
#endif

#endif /* AMBA_RTSL_VOUT_H */
