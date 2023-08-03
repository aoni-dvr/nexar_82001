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

#ifndef AMBA_HDMI_DEF_H
#include "AmbaHDMI_Def.h"
#endif

#ifndef AMBA_VOUT_DEF_H
#include "AmbaVOUT_Def.h"
#endif

#ifndef AMBA_REG_VOUT_H
#include "AmbaReg_VOUT.h"
#endif


/*
 * Defined in AmbaRTSL_VOUT.c
 */
UINT32 AmbaRTSL_Vout0InitDispConfig(void);
UINT32 AmbaRTSL_Vout0GetDispConfigAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout0GetDigitalCscAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout0SetVinVoutSync(UINT32 EnableFlag);
UINT32 AmbaRTSL_Vout0GetStatus(UINT32 *pStatus);
UINT32 AmbaRTSL_Vout0GetTvEncAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout0GetAnalogCscAddr(ULONG *pVirtAddr);

UINT32 AmbaRTSL_Vout1InitDispConfig(void);
UINT32 AmbaRTSL_Vout1GetDispConfigAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout1GetDigitalCscAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout1GetAnalogCscAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout1GetHdmiCscAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout1SetVinVoutSync(UINT32 EnableFlag);
UINT32 AmbaRTSL_Vout1GetStatus(UINT32 *pStatus);
UINT32 AmbaRTSL_Vout1GetTvEncAddr(ULONG *pVirtAddr);

UINT32 AmbaRTSL_VoutInitTvEncConfig(void);

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
UINT32 AmbaRTSL_Vout2InitDispConfig(void);
UINT32 AmbaRTSL_Vout2GetDispConfigAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout2GetTvEncAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout2GetAnalogCscAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout2GetHdmiCscAddr(ULONG *pVirtAddr);
UINT32 AmbaRTSL_Vout2SetVinVoutSync(UINT32 EnableFlag);
UINT32 AmbaRTSL_Vout2GetStatus(UINT32 *pStatus);
UINT32 AmbaRTSL_Vout2SetTimeout(UINT32 Timeout);

UINT32 AmbaRTSL_VoutChannelSelect(UINT32 VoutChannel);
UINT32 AmbaRTSL_Vout0SetTimeout(UINT32 Timeout);
UINT32 AmbaRTSL_Vout1SetTimeout(UINT32 Timeout);
#endif

#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
UINT32 AmbaRTSL_VoutDigiYccEnable(UINT32 YccMode, UINT32 ColorOrder, const AMBA_VOUT_DATA_LATCH_CONFIG_s *pDataLatch, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutDigiYccSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
UINT32 AmbaRTSL_VoutDigiYccGetStatus(AMBA_VOUT_DIGITAL_YCC_CONFIG_s *pDigiYccConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutDigiYccSetCsc(const UINT16 *pVoutCscData, UINT32 YccMode);

UINT32 AmbaRTSL_VoutDigiRgbEnable(UINT32 RgbMode, UINT32 ColorOrder, const AMBA_VOUT_DATA_LATCH_CONFIG_s *pDataLatch, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutDigiRgbSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
UINT32 AmbaRTSL_VoutDigiRgbGetStatus(AMBA_VOUT_DIGITAL_RGB_CONFIG_s *pDigiRgbConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutDigiRgbSetCsc(const UINT16 *pVoutCscData, UINT32 RgbMode);
#endif
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28)
UINT32 AmbaRTSL_VoutFpdLinkEnable(UINT32 FpdLinkMode, UINT32 ColorOrder, UINT32 DataEnablePolarity, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutFpdLinkSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
UINT32 AmbaRTSL_VoutFpdLinkGetStatus(AMBA_VOUT_FPD_LINK_CONFIG_s *pFpdLinkConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutFpdLinkSetCsc(const UINT16 *pVoutCscData, UINT32 FpdLinkMode);
#endif

#if !defined(CONFIG_SOC_H32)
UINT32 AmbaRTSL_VoutConfigMipiPhy(const AMBA_VOUT_MIPI_TIMING_PARAM_s *pVoutMipiTiming);
UINT32 AmbaRTSL_VoutMipiCsiEnable(UINT32 MipiCsiMode, UINT32 ColorOrder, UINT32 MipiLaneNum, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutMipiCsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
UINT32 AmbaRTSL_VoutMipiCsiGetStatus(AMBA_VOUT_MIPI_CSI_CONFIG_s *pMipiCsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutMipiCsiSetCsc(const UINT16 *pVoutCscData, UINT32 MipiCsiMode);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
UINT32 AmbaRTSL_VoutMipiCsiSetMaxFrame(UINT32 MaxFrameCount);
#endif

UINT32 AmbaRTSL_VoutMipiDsiEnable(UINT32 MipiDsiMode, UINT32 ColorOrder, UINT32 MipiLaneNum);
UINT32 AmbaRTSL_VoutMipiDsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
UINT32 AmbaRTSL_VoutMipiDsiGetStatus(AMBA_VOUT_MIPI_DSI_CONFIG_s *pMipiDsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutMipiDsiSetCsc(const UINT16 *pVoutCscData, UINT32 MipiDsiMode);
UINT32 AmbaRTSL_VoutSetMipiDsiTiming(const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming, const UINT16 NewHFrontPorch, const UINT16 NewVFrontPorch);
UINT32 AmbaRTSL_VoutMipiDsiDcsWrite(UINT32 DcsCmd, UINT32 NumParam, const UINT8 * pParam);
UINT32 AmbaRTSL_VoutMipiDsiNormalWrite(UINT32 NumParam, const UINT8 * pParam);
UINT32 AmbaRTSL_VoutMipiDsiSetBlankPkt(const AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s * pMipiDsiBlankPktCtrl);
#if defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
UINT32 AmbaRTSL_VoutMipiDsiSetBtaSrc(UINT32 BtaEnSrc);
UINT32 AmbaRTSL_VoutMipiDsiGetAckData(UINT32 *pAckData);
#endif
#endif

UINT32 AmbaRTSL_VoutCvbsEnable(const AMBA_VOUT_DISPLAY_CVBS_CONFIG_REG_s *pDisplayCvbsConfig, const AMBA_VOUT_TVENC_CONTROL_REG_s *pTveConfig);
UINT32 AmbaRTSL_VoutCvbsSetIreParam(UINT32 IreIndex, UINT32 Param);
UINT32 AmbaRTSL_VoutCvbsGetIreParam(UINT32 IreIndex);
UINT32 AmbaRTSL_VoutCvbsEnableColorBar(UINT32 EnableFlag, const AMBA_VOUT_TVENC_CONTROL_REG_s *pTveConfig);
UINT32 AmbaRTSL_VoutCvbsPowerCtrl(UINT32 EnableFlag);
UINT32 AmbaRTSL_VoutAnalogSetCsc(const UINT16 *pVoutCscData, UINT32 AnalogMode);

#if !defined(CONFIG_SOC_CV28)
UINT32 AmbaRTSL_VoutHdmiEnable(UINT32 HdmiMode, UINT32 FrameSyncPolarity, UINT32 LineSyncPolarity, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutHdmiGetStatus(AMBA_VOUT_HDMI_CONFIG_s *pHdmiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaRTSL_VoutHdmiSetCsc(const UINT16 *pHdmiCscData, UINT32 HdmiMode);
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
UINT32 AmbaRTSL_VoutMipiTestPattern(void);
UINT32 AmbaRTSL_VoutConfigThrDbgPort(UINT32 VoutChannel, const volatile void *pVoutDispReg);
#endif

#endif /* AMBA_RTSL_VOUT_H */
