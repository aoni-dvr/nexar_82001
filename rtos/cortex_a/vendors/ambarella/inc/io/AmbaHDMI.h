/**
 * @file AmbaHDMI.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 * @details Definitions & Constants for HDMI Middleware APIs
 *
 */

#ifndef AMBA_HDMI_H
#define AMBA_HDMI_H

#ifndef AMBA_HDMI_DEF_H
#include "AmbaHDMI_Def.h"
#endif

#ifndef AMBA_VOUT_DEF_H
#include "AmbaVOUT_Def.h"
#endif

/*
 * Product Description Structure
 */
#define HDMI_SRC_TYPE_UNKNOWN          0x00U       /* Unknown */
#define HDMI_SRC_TYPE_DIGITAL_STB      0x01U       /* Digital STB */
#define HDMI_SRC_TYPE_DVD              0x02U       /* DVD player */
#define HDMI_SRC_TYPE_DVHS             0x03U       /* D-VHS */
#define HDMI_SRC_TYPE_HDD_RECORDER     0x04U       /* HDD Video Recorder */
#define HDMI_SRC_TYPE_DVC              0x05U       /* DVC */
#define HDMI_SRC_TYPE_DSC              0x06U       /* DSC */
#define HDMI_SRC_TYPE_VCD              0x07U       /* Video CD */
#define HDMI_SRC_TYPE_GAME             0x08U       /* Game */
#define HDMI_SRC_TYPE_PC               0x09U       /* PC general */
#define HDMI_SRC_TYPE_BD               0x0AU       /* Blu-Ray Disc */
#define HDMI_SRC_TYPE_SACD             0x0BU       /* Super Audio CD */
#define HDMI_SRC_TYPE_HD_DVD           0x0CU       /* HD DVD */
#define HDMI_SRC_TYPE_PMP              0x0DU       /* PMP */
#define NUM_HDMI_SRC_TYPE              0x0EU

typedef struct {
    char VendorName[8];
    char ProductDescription[16];
    UINT32 SrcType;
} AMBA_HDMI_SOURCE_PRODUCT_INFO_s;

/*
 * HDMI Sink Information
 */
typedef struct {
    char    ManufacturerName[3];    /*ISA (Industry Standard Architecture) Plug and Play Device Identifier (PNPID) */
    UINT8   WeekOfManufacture;      /*Week of Manufacture (range is 1-54 weeks) or Model Year Flag (0xFF) */
    UINT16  YearOfManufacture;      /*Year of Manufacture (range is 2006-2245) or Model Year (range is 2006-2245) */
    UINT8   ProductCode[2];
    UINT8   SerialNumber[4];
} AMBA_HDMI_SINK_PRODUCT_INFO_s;

typedef struct {
    UINT32  SrcPhysicalAddr;
    UINT32  DeepColorCapability;
    UINT32  QuantRangeCapability;
    UINT32  ColorFormatCapability;
    UINT32  NumDTD;
    AMBA_HDMI_VIDEO_TIMING_s DtdBuf[10];
    UINT32  NumVideoInfo;
    UINT32  VideoInfoBuf[NUM_HDMI_VIC];
    UINT32  NumAudioInfo;
    UINT32  AudioInfoBuf[NUM_HDMI_AIC];
    AMBA_HDMI_SINK_PRODUCT_INFO_s SinkProductInfo;
    UINT32  ScdcPresent;
} AMBA_HDMI_SINK_INFO_s;

/*
 * HDMI Mode Information
 */
typedef struct {
    UINT32 AudioSampleRate;
    FLOAT VideoFrameRate;           /* Frame or field rate. (Field rate when interlaced mode) */
    UINT32 AspectRtaio;
    UINT8 VideoIdCode;
    UINT8 PixelFormat;
    UINT8 ColorDepth;
    UINT8 QuantRange;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s   VideoConfig;
} AMBA_HDMI_MODE_INFO_s;

/*
 * Defined in AmbaHDMI.c
 */
UINT32 AmbaHDMI_TxEnable(UINT32 HdmiPort);
UINT32 AmbaHDMI_TxDisable(UINT32 HdmiPort);
UINT32 AmbaHDMI_HookHpdHandler(UINT32 HdmiPort, AMBA_HDMI_HPD_ISR_f IntHandler);
UINT32 AmbaHDMI_HookCecHandler(UINT32 HdmiPort, AMBA_HDMI_CEC_ISR_f IntHandler);
UINT32 AmbaHDMI_TxGetSinkInfo(UINT32 HdmiPort, UINT32 *pHotPlugDetect, UINT32 *pEdidDetect, AMBA_HDMI_SINK_INFO_s *pSinkInfo);
UINT32 AmbaHDMI_TxSetMode(UINT32 HdmiPort, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig);
UINT32 AmbaHDMI_TxGetCurrModeInfo(UINT32 HdmiPort, AMBA_HDMI_MODE_INFO_s *pModeInfo);
UINT32 AmbaHDMI_TxSetPhyCtrl(UINT32 HdmiPort, UINT32 ParamID, UINT32 ParamVal);
UINT32 AmbaHDMI_TxStart(UINT32 HdmiPort);
UINT32 AmbaHDMI_TxStop(UINT32 HdmiPort);
UINT32 AmbaHDMI_TxSetSourceInfo(UINT32 HdmiPort, const AMBA_HDMI_SOURCE_PRODUCT_INFO_s *pSrcProductInfo);
UINT32 AmbaHDMI_TxSetInfoFramePacket(UINT32 HdmiPort, const AMBA_HDMI_DATA_ISLAND_PACKET_s *pInfoFramePkt, UINT32 TxMode);
UINT32 AmbaHDMI_TxGetInfoFramePacket(UINT32 HdmiPort, UINT32 InfoFrameType, AMBA_HDMI_DATA_ISLAND_PACKET_s *pInfoFramePkt);
UINT32 AmbaHDMI_TxSetAvMute(UINT32 HdmiPort);
UINT32 AmbaHDMI_TxClearAvMute(UINT32 HdmiPort);
UINT32 AmbaHDMI_TxEnableTestPattern(UINT32 HdmiPort);

/*
 * Defined in AmbaHDMI_CEC.c
 */
UINT32 AmbaHDMI_CecEnable(UINT32 HdmiPort, UINT32 LogicalAddr);
UINT32 AmbaHDMI_CecDisable(UINT32 HdmiPort);
UINT32 AmbaHDMI_CecWrite(UINT32 HdmiPort, const UINT8 *pMsg, UINT32 MsgSize);
UINT32 AmbaHDMI_CecRead(UINT32 HdmiPort, UINT8 *pMsg, UINT32 *pMsgSize);

#endif /*AMBA_HDMI_H */
