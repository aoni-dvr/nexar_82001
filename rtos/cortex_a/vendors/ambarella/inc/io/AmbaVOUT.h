/**
 *  @file AmbaVOUT.h
 *
 *  Copyright (c) 2021 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for Video Output Middleware APIs
 *
 */

#ifndef AMBA_VOUT_H
#define AMBA_VOUT_H

#ifndef AMBA_VOUT_DEF_H
#include "AmbaVOUT_Def.h"
#endif

#ifndef AMBA_SYS_DEF_H
#include "AmbaSYS_Def.h"
#endif


/* Defined in AmbaVOUT.c */
UINT32 AmbaVout_ChannelSelect(UINT32 VoutChannel);

UINT32 AmbaVout_GetDisp0ConfigAddr(ULONG *pVirtAddr, UINT32 *pSize);
UINT32 AmbaVout_GetDisp1ConfigAddr(ULONG *pVirtAddr, UINT32 *pSize);
UINT32 AmbaVout_GetDisp2ConfigAddr(ULONG *pVirtAddr, UINT32 *pSize);
UINT32 AmbaVout_GetTvEncAddr(ULONG *pVirtAddr, UINT32 *pSize);

UINT32 AmbaVout_GetDisp0DigiCscAddr(ULONG *pVirtAddr, UINT32 *pSize);
UINT32 AmbaVout_GetDisp1DigiCscAddr(ULONG *pVirtAddr, UINT32 *pSize);

UINT32 AmbaVout_GetDisp0AnalogCscAddr(ULONG *pVirtAddr, UINT32 *pSize);
UINT32 AmbaVout_GetDisp1AnalogCscAddr(ULONG *pVirtAddr, UINT32 *pSize);
UINT32 AmbaVout_GetDisp2AnalogCscAddr(ULONG *pVirtAddr, UINT32 *pSize);

UINT32 AmbaVout_GetDisp1HdmiCscAddr(ULONG *pVirtAddr, UINT32 *pSize);
UINT32 AmbaVout_GetDisp2HdmiCscAddr(ULONG *pVirtAddr, UINT32 *pSize);

UINT32 AmbaVout_SetDisp0VinVoutSync(UINT32 EnableFlag);
UINT32 AmbaVout_SetDisp1VinVoutSync(UINT32 EnableFlag);
UINT32 AmbaVout_SetDisp2VinVoutSync(UINT32 EnableFlag);

UINT32 AmbaVout_DigiYccEnable(UINT32 YccMode, UINT32 ColorOrder, const AMBA_VOUT_DATA_LATCH_CONFIG_s *pDataLatch, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaVout_DigiYccSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
UINT32 AmbaVout_DigiYccGetStatus(AMBA_VOUT_DIGITAL_YCC_CONFIG_s *pDigiYccConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaVout_DigiYccSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 YccMode);

UINT32 AmbaVout_DigiRgbEnable(UINT32 RgbMode, UINT32 ColorOrder, const AMBA_VOUT_DATA_LATCH_CONFIG_s *pDataLatch, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaVout_DigiRgbSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
UINT32 AmbaVout_DigiRgbGetStatus(AMBA_VOUT_DIGITAL_RGB_CONFIG_s *pDigiRgbConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaVout_DigiRgbSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 RgbMode);

UINT32 AmbaVout_FpdLinkEnable(UINT32 FpdLinkMode, UINT32 ColorOrder, UINT32 DataEnablePolarity, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaVout_FpdLinkSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
UINT32 AmbaVout_FpdLinkGetStatus(AMBA_VOUT_FPD_LINK_CONFIG_s *pFpdLinkConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaVout_FpdLinkSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 FpdMode);

UINT32 AmbaVout_MipiCsiEnable(UINT32 MipiCsiMode, UINT32 ColorOrder, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaVout_MipiCsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
UINT32 AmbaVout_MipiCsiGetStatus(AMBA_VOUT_MIPI_CSI_CONFIG_s *pMipiCsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaVout_MipiCsiSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 MipiCsiMode);

UINT32 AmbaVout_MipiDsiEnable(UINT32 MipiDsiMode, UINT32 ColorOrder, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaVout_MipiDsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
UINT32 AmbaVout_MipiDsiGetStatus(AMBA_VOUT_MIPI_DSI_CONFIG_s *pMipiDsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaVout_MipiDsiSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 MipiDsiMode);
UINT32 AmbaVout_MipiDsiCalculateTiming(const UINT8 MipiDsiMode, const AMBA_VOUT_FRAME_TIMING_CONFIG_s *pFrameTiming, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming, AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s *pBlankPacket);

UINT32 AmbaVout_MipiDsiSendDcsWrite(UINT32 DcsCmd, UINT32 NumParam, const UINT8 * pParam);
UINT32 AmbaVout_MipiDsiSendNormalWrite(UINT32 NumParam, const UINT8 * pParam);
UINT32 AmbaVout_MipiDsiSetBlankPkt(const AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s * pMipiDsiBlankPktCtrl);

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
UINT32 AmbaVout_SafetyStart(UINT32 VoutID);
UINT32 AmbaVout_SafetyStop(UINT32 VoutID);
UINT32 AmbaVout_GetTopLevelStatus(UINT32 *pStatus);
UINT32 AmbaVout_GetDisp0Status(UINT32 *pStatus, UINT32 *pFreezeDetect);
UINT32 AmbaVout_GetDisp1Status(UINT32 *pStatus, UINT32 *pFreezeDetect);
UINT32 AmbaVout_MipiDsiGetAckData(UINT32 *pAckData);
#else
UINT32 AmbaVout_GetDisp0Status(UINT32 *pStatus);
UINT32 AmbaVout_GetDisp1Status(UINT32 *pStatus);
#endif
UINT32 AmbaVout_GetDisp2Status(UINT32 *pStatus);

UINT32 AmbaVout_SetDisp0Timeout(UINT32 Timeout);
UINT32 AmbaVout_SetDisp1Timeout(UINT32 Timeout);
UINT32 AmbaVout_SetDisp2Timeout(UINT32 Timeout);

#if defined(CONFIG_LINUX) //TODO remove if linux hdmi ko removed
UINT32 AmbaVout_HdmiEnable(UINT32 HdmiMode, UINT32 FrameSyncPolarity, UINT32 LineSyncPolarity, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaVout_HdmiGetStatus(AMBA_VOUT_HDMI_CONFIG_s *pHdmiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming);
UINT32 AmbaVout_HdmiSetCsc(const AMBA_VOUT_CSC_MATRIX_s *pCscMatrix, UINT32 HdmiMode);
#endif

/*
 * Defined in AmbaDiag_VOUT.c
 */
UINT32 AmbaDiag_VoutShowInfo(UINT32 VoutChan, AMBA_SYS_LOG_f LogFunc);
UINT32 AmbaDiag_VoutGetMmioAddr(UINT32 MmioID, ULONG *pMmioAddr, UINT32 *pMmioSize);
UINT32 AmbaDiag_VoutMipiTestPattern(void);
UINT32 AmbaDiag_VoutConfigThrDbgPort(UINT32 VoutChan);

/*
 * Defined in AmbaVOUT_CSC.c
 */
UINT32 AmbaVout_GetCscMatrix(UINT32 CscType, UINT32 CscIndex, AMBA_VOUT_CSC_MATRIX_s **pCSC);

#endif /* AMBA_VOUT_H */
