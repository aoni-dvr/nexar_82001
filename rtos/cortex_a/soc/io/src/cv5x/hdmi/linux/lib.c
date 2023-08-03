// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
 *  @file lib.c
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
 *
 *  @details lib for HDMI device module
 *
 */

#include <asm/io.h>
#include <linux/module.h>

#include "proto.h"
#include "osal.h"

#include "AmbaHDMI_EDID.h"
#include "AmbaReg_I2S.h"
#include "AmbaReg_VOUT.h"

#define AmbaWrap_memcpy OSAL_memcpy
#define AmbaWrap_memset OSAL_memset

/**
 * VESA Enahnced Display Data Channel (E-DDC) Standard
 */
#define DDC_ADDR_SET_SEGMENT_POINTER        (0x60U) /* I2C slave address to write segment pointer */
#define DDC_ADDR_SET_WORD_OFFSET            (0xA0U) /* I2C slave address to write word offset of the segment */
#define DDC_ADDR_GET_EDID_BLOCK             (0xA1U) /* I2C slave address to read one EDID block  */

/**
 * Status and Control Data Channel (SCDC)
 */
#define SCDC_ADDR_WRITE                     (0xA8U) /* I2C slave address to write SCDC */

/**
 * HDMI Control Structure
 */
typedef struct {
    AMBA_HDMI_VIDEO_CONFIG_s    VideoCfg;
    AMBA_HDMI_AUDIO_CONFIG_s    AudioCfg;
    AMBA_HDMI_SOURCE_PRODUCT_INFO_s DeviceCfg;
} AMBA_HDMI_CTRL_s;

extern AMBA_VOUT_DISPLAY_C_CONFIG_REG_s AmbaVOUT_Display2Config;
static AMBA_HDMI_CTRL_s AmbaHdmiCtrl;
static UINT8 AmbaHdmiSinkEDID[EDID_NUM_BLOCKS][EDID_BLOCK_SIZE];
static AMBA_HDMI_DATA_ISLAND_PACKET_s AmbaHdmiInfoFrameBuf[NUM_HDMI_INFOFRAME_TYPE];
static UINT32 AmbaHdmiInfoFrameTxMode[NUM_HDMI_INFOFRAME_TYPE] = {
    [HDMI_INFOFRAME_RESERVED]           = HDMI_PACKET_SEND_NONE,
    [HDMI_INFOFRAME_VENDOR_SPECIFIC]    = HDMI_PACKET_SEND_EVERY_FRAME,
    [HDMI_INFOFRAME_AVI]                = HDMI_PACKET_SEND_EVERY_FRAME,
    [HDMI_INFOFRAME_SPD]                = HDMI_PACKET_SEND_EVERY_FRAME,
    [HDMI_INFOFRAME_AUDIO]              = HDMI_PACKET_SEND_EVERY_FRAME,
    [HDMI_INFOFRAME_MPEG_SOURCE]        = HDMI_PACKET_SEND_NONE,
};

//static UINT32 HDMI_ValdidateEDID(const UINT8 *pEdidBlock);
//static UINT32 HDMI_ReadEDID(UINT32 EdidBlockNo, UINT8 *pEdidBlock);
static UINT32 HDMI_DetectSink(UINT32 *pHotPlugDetect, UINT32 *pEdidDetect);
//static UINT32 HDMI_WriteSCDC(UINT32 TxDataSize, UINT8 *pTxDataBuf);
static void HDMI_SetClockRatio(UINT32 PixelClock);
static void HDMI_RefreshInfoFrame(UINT32 InfoFrameType);
static UINT32 HDMI_ValidateConfig(UINT8 VideoIdCode, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig);

int hdmi_dev_init(void)
{
    void __iomem* VirtualAddr;

    printk("hdmi_dev_init()\n");

    if ((VirtualAddr = ioremap(AMBA_RCT_BASE_ADDR,
                               AMBA_RCT_MMAP_SIZE)) == NULL) {
        pr_err("%s: cannot access to rct region\n", __func__);
        return -1;
    } else {
        pAmbaRCT_Reg = (AMBA_RCT_REG_s *) VirtualAddr;
    }

    if ((VirtualAddr = ioremap(AMBA_HDMI_BASE_ADDR,
                               AMBA_HDMI_MMAP_SIZE)) == NULL) {
        pr_err("%s: cannot access to hdmi region\n", __func__);
        return -1;
    } else {
        pAmbaHDMI_Reg =  (AMBA_HDMI_REG_s *) VirtualAddr;
    }

    if ((VirtualAddr = ioremap(AMBA_SCRATCHPADNS_BASE_ADDR,
                               AMBA_SCRATCHPADNS_MMAP_SIZE)) == NULL) {
        pr_err("%s: cannot access to scratchpad region\n", __func__);
        return -1;
    } else {
        pAmbaScratchpadNS_Reg = (AMBA_SCRATCHPAD_NS_REG_s *) VirtualAddr;
    }

    if ((VirtualAddr = ioremap(AMBA_VOUT_TOP_BASE_ADDR,
                               AMBA_VOUT_TOP_MMAP_SIZE)) == NULL) {
        pr_err("%s: cannot access to vout top region\n", __func__);
        return -1;
    } else {
        pAmbaVoutTop_Reg = (AMBA_VOUT_TOP_REG_s *) VirtualAddr;
    }

    if ((VirtualAddr = ioremap(AMBA_VOUT_DISPLAY2_CONFIG_BASE_ADDR,
                               AMBA_VOUT_DISPLAY2_CONFIG_MMAP_SIZE)) == NULL) {
        pr_err("%s: cannot access to vout display2 region\n", __func__);
        return -1;
    } else {
        pAmbaVoutDisplay2_Reg = (AMBA_VOUT_DISPLAY_C_CONFIG_REG_s *) VirtualAddr;
    }

    if ((VirtualAddr = ioremap(AMBA_I2S0_BASE_ADDR,
                               AMBA_I2S_MMAP_SIZE)) == NULL) {
        pr_err("%s: cannot access to i2s0 region\n", __func__);
        return -1;
    } else {
        pAmbaI2S_Reg[AMBA_I2S_CHANNEL0] = (AMBA_I2S_REG_s *) VirtualAddr;
    }

    if ((VirtualAddr = ioremap(AMBA_I2S1_BASE_ADDR,
                               AMBA_I2S_MMAP_SIZE)) == NULL) {
        pr_err("%s: cannot access to i2s1 region\n", __func__);
        return -1;
    } else {
        pAmbaI2S_Reg[AMBA_I2S_CHANNEL1] = (AMBA_I2S_REG_s *) VirtualAddr;
    }

    AmbaRTSL_HdmiInit();

    return 0;
}

int hdmi_dev_dinit(void)
{
    iounmap((void __iomem*)pAmbaRCT_Reg);
    iounmap((void __iomem*)pAmbaHDMI_Reg);
    iounmap((void __iomem*)pAmbaScratchpadNS_Reg);
    iounmap((void __iomem*)pAmbaVoutTop_Reg);
    iounmap((void __iomem*)pAmbaVoutDisplay2_Reg);
    iounmap((void __iomem*)pAmbaI2S_Reg[AMBA_I2S_CHANNEL0]);
    iounmap((void __iomem*)pAmbaI2S_Reg[AMBA_I2S_CHANNEL1]);

    return 0;
}

/**
 *  hdmi_TxEnable - Turn-On HDMI transmitter
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
int hdmi_TxEnable(UINT32 HdmiPort)
{
    int RetVal = 0;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = -1;
    } else {
        /* Set default PHY configuration to make sure all the resolution work */
        AmbaRTSL_HdmiSetPhyCtrl(0U, 2U);
        AmbaRTSL_HdmiSetPhyCtrl(1U, 3U);
        AmbaRTSL_HdmiSetPhyCtrl(2U, 4U);
        AmbaRTSL_HdmiSetPhyCtrl(3U, 0U);
        AmbaRTSL_HdmiSetPhyCtrl(4U, 0U);
        AmbaRTSL_HdmiSetPowerCtrl(1U);
    }

    return RetVal;
}

/**
 *  hdmi_TxDisable - Turn-Off HDMI transmitter
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
int hdmi_TxDisable(UINT32 HdmiPort)
{
    int RetVal = 0;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = -1;
    } else {
        AmbaRTSL_HdmiSetPowerCtrl(0U);
    }

    return RetVal;
}

/**
 *  hdmi_HookHpdHandler - Hook HPD event handler
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] IntHandler Interrupt handler
 *  @return error code
 */
int hdmi_HookHpdHandler(UINT32 HdmiPort, AMBA_HDMI_HPD_ISR_f IntHandler)
{
    int RetVal = 0;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = -1;
    } else {
        RetVal = AmbaRTSL_HdmiHookHpdHandler(IntHandler);
    }

    return RetVal;
}

/**
 *  hdmi_TxGetSinkInfo - Get supported Video Identification Code (VIC) and Audio Format Code
 *  @param[in] HdmiPort HDMI port ID
 *  @param[out] pHotPlugDetect Hot-Plug Detection Status
 *  @param[out] pEdidDetect EDID Detection Status
 *  @param[out] pSinkInfo pointer to external buffer of sink information
 *  @return error code
 */
int hdmi_TxGetSinkInfo(UINT32 HdmiPort, UINT32 *pHotPlugDetect, UINT32 *pEdidDetect, AMBA_HDMI_SINK_INFO_s *pSinkInfo)
{
    int RetVal = 0;

    if ((HdmiPort >= NUM_HDMI_TX_PORT) || (pHotPlugDetect == NULL) || (pEdidDetect == NULL) || (pSinkInfo == NULL)) {
        RetVal = -1;
    } else {
        (void)HDMI_DetectSink(pHotPlugDetect, pEdidDetect);
        if (*pEdidDetect != HDMI_EDID_UNAVAIL) {
            RetVal = hdmi_TxCopySinkInfo(pSinkInfo);
        }
    }

    return RetVal;
}

/**
 *  hdmi_TxSetMode - Configure HDMI/DVI outputs
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] pVideoConfig pointer to video format configuration
 *  @param[in] pAudioConfig pointer to audio format configuration
 *  @param[in] IsSuccessor recovery flow flag
 *  @param[out] pWorkingVideoConfig pointer to video format configuration, return information to userspace
 *  @return error code
 */
int hdmi_TxSetMode(UINT32 HdmiPort, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig, UINT32 IsSuccessor, AMBA_HDMI_VIDEO_CONFIG_s *pWorkingVideoConfig)
{
    UINT32 HotPlugDetect, EdidDetect;
    int RetVal = 0;
    UINT8 VideoIdCode;

    VideoIdCode = HDMI_GET_RAW_VIC(pVideoConfig->VideoIdCode);

    if ((HdmiPort >= NUM_HDMI_TX_PORT) || (pVideoConfig == NULL)) {
        RetVal = -1;
    } else {

        RetVal = HDMI_ValidateConfig(VideoIdCode, pVideoConfig, pAudioConfig);
        if (RetVal == 0) {
            if (IsSuccessor == 0U){
                (void)AmbaRTSL_HdmiReset(1U);
                (void)AmbaRTSL_HdmiReset(0U);
                (void)AmbaRTSL_HdmiConfigVideo(&AmbaHdmiCtrl.VideoCfg.CustomDTD);
                HDMI_SetClockRatio(AmbaHdmiCtrl.VideoCfg.CustomDTD.PixelClock);

                /* DVI does not support transport of CEA InfoFrames, audio or YCbCr pixel data. */
                (void)HDMI_DetectSink(&HotPlugDetect, &EdidDetect);
                EdidDetect = HDMI_EDID_HDMI;  //TODO EDID not complete
                if (EdidDetect == HDMI_EDID_HDMI) {
                    (void)hdmi_TxConfigAudio(&AmbaHdmiCtrl.VideoCfg, &AmbaHdmiCtrl.AudioCfg);
                    (void)AmbaWrap_memset(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_AUDIO], 0, sizeof(AMBA_HDMI_DATA_ISLAND_PACKET_s));
                    (void)AmbaWrap_memset(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_SPD], 0, sizeof(AMBA_HDMI_DATA_ISLAND_PACKET_s));
                    (void)AmbaWrap_memset(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_AVI], 0, sizeof(AMBA_HDMI_DATA_ISLAND_PACKET_s));
                    (void)AmbaWrap_memset(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_VENDOR_SPECIFIC], 0, sizeof(AMBA_HDMI_DATA_ISLAND_PACKET_s));
                    (void)hdmi_TxSendAudioInfoFrame(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_AUDIO], &AmbaHdmiCtrl.AudioCfg, AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_AUDIO]);
                    (void)hdmi_TxSendSpdInfoFrame(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_SPD], &AmbaHdmiCtrl.DeviceCfg, AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_SPD]);
                    (void)hdmi_TxSendAviInfoFrame(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_AVI], &AmbaHdmiCtrl.VideoCfg, AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_AVI]);
                    (void)hdmi_TxSendVendorInfoFrame(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_VENDOR_SPECIFIC], &AmbaHdmiCtrl.VideoCfg, AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_VENDOR_SPECIFIC]);

                    /* Update all infoframe packets */
                    AmbaRTSL_HdmiPacketUpdate();
                }
            }

            RetVal = AmbaWrap_memcpy(pWorkingVideoConfig, &AmbaHdmiCtrl.VideoCfg, sizeof(AMBA_HDMI_VIDEO_CONFIG_s));
        }

    }

    return RetVal;
}

/**
 *  hdmi_TxGetCurrModeInfo - Configure HDMI/DVI outputs
 *  @param[in] HdmiPort HDMI port ID
 *  @param[out] pModeInfo pointer to current mode information
 *  @param[out] pStrictFrameRate pointer to StrictFrameRate
 *  @param[out] pFrameWidth pointer to FrameWidth
 *  @param[out] pFrameHeight pointer to FrameHeight
 *  @return error code
 */
int hdmi_TxGetCurrModeInfo(UINT32 HdmiPort, AMBA_HDMI_MODE_INFO_s *pModeInfo, UINT8 *pStrictFrameRate, UINT16 *pFrameWidth, UINT16 *pFrameHeight)
{
    const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig = &AmbaHdmiCtrl.VideoCfg;
    const AMBA_HDMI_VIDEO_TIMING_s *pDtd = &pVideoConfig->CustomDTD;
    UINT16 FrameWidth, FrameHeight;
    int RetVal = 0;
    UINT8 StrictFrameRate, VideoIdCode;

    if ((pVideoConfig->VideoIdCode & HDMI_FRAME_RATE_STRICT) == 0U) {
        StrictFrameRate = 0U;
    } else {
        StrictFrameRate = 1U;
    }

    VideoIdCode = HDMI_GET_RAW_VIC(pVideoConfig->VideoIdCode);

    if ((HdmiPort >= NUM_HDMI_TX_PORT) || (pModeInfo == NULL)) {
        RetVal = -1;
    } else {

        pModeInfo->VideoConfig.PixelClkFreq     = 0;    //Get this in HDMI MW
        pModeInfo->VideoConfig.HsyncPulseWidth  = pVideoConfig->CustomDTD.HsyncPulseWidth;
        pModeInfo->VideoConfig.HsyncBackPorch   = pVideoConfig->CustomDTD.HsyncBackPorch;
        pModeInfo->VideoConfig.VsyncFrontPorch  = pVideoConfig->CustomDTD.VsyncFrontPorch;
        pModeInfo->VideoConfig.VsyncPulseWidth  = pVideoConfig->CustomDTD.VsyncPulseWidth;
        pModeInfo->VideoConfig.VsyncBackPorch   = pVideoConfig->CustomDTD.VsyncBackPorch;
        pModeInfo->VideoConfig.ActiveLines      = pVideoConfig->CustomDTD.ActiveLines;
        pModeInfo->VideoConfig.DisplayMethod    = pVideoConfig->CustomDTD.ScanFormat;

        /* The pixel repetition (only doubling is supported) is done by vout display section. */
        /* The input of display section (i.e. mixer section) must be half horizontal resolution. */
        /* Ex. Return 1440/2=720 picture width for mixer settings of 480I and 576I. */
        if (pVideoConfig->CustomDTD.PixelRepeat == 1U) {
            pModeInfo->VideoConfig.HsyncFrontPorch  = pVideoConfig->CustomDTD.HsyncFrontPorch + (pVideoConfig->CustomDTD.ActivePixels >> 1U);
            pModeInfo->VideoConfig.ActivePixels     = pVideoConfig->CustomDTD.ActivePixels >> 1U;
        } else {
            pModeInfo->VideoConfig.HsyncFrontPorch  = pVideoConfig->CustomDTD.HsyncFrontPorch;
            pModeInfo->VideoConfig.ActivePixels     = pVideoConfig->CustomDTD.ActivePixels;
        }

        FrameWidth = pDtd->HsyncFrontPorch + pDtd->HsyncPulseWidth + pDtd->HsyncBackPorch + pDtd->ActivePixels;
        if (pVideoConfig->CustomDTD.ScanFormat == AMBA_VIDEO_SCAN_PROGRESSIVE) {
            FrameHeight = pDtd->VsyncFrontPorch + pDtd->VsyncPulseWidth + pDtd->VsyncBackPorch + pDtd->ActiveLines;
        } else {
            FrameHeight = pDtd->VsyncFrontPorch + pDtd->VsyncPulseWidth + pDtd->VsyncBackPorch;
            FrameHeight *= 2U;
            FrameHeight = FrameHeight + 1U + pDtd->ActiveLines;
        }

        pModeInfo->AudioSampleRate = AmbaHdmiCtrl.AudioCfg.SampleRate;

        *pStrictFrameRate = StrictFrameRate;
        *pFrameWidth = FrameWidth;
        *pFrameHeight = FrameHeight;

        if (VideoIdCode == pVideoConfig->CustomDTD.Vic4_3) {
            pModeInfo->AspectRtaio  = HDMI_ASPECT_RATIO_4x3;
        } else if (VideoIdCode == pVideoConfig->CustomDTD.Vic16_9) {
            pModeInfo->AspectRtaio  = HDMI_ASPECT_RATIO_16x9;
        } else if (VideoIdCode == pVideoConfig->CustomDTD.Vic64_27) {
            pModeInfo->AspectRtaio  = HDMI_ASPECT_RATIO_64x27;
        } else if (VideoIdCode == pVideoConfig->CustomDTD.Vic256_135) {
            pModeInfo->AspectRtaio  = HDMI_ASPECT_RATIO_256x135;
        } else {
            pModeInfo->AspectRtaio  = ((UINT32)pDtd->ActivePixels << 16U) | (UINT32)pDtd->ActiveLines;
        }
        pModeInfo->VideoIdCode      = VideoIdCode;
        pModeInfo->PixelFormat      = pVideoConfig->PixelFormat;
        pModeInfo->ColorDepth       = pVideoConfig->ColorDepth;
        pModeInfo->QuantRange       = pVideoConfig->QuantRange;
    }

    return RetVal;
}

/**
 *  hdmi_TxSetPhyCtrl - Set HDMI PHY control
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] ParamID PHY parameter type
 *  @param[in] ParamVal PHY parameter value
 *  @return error code
 */
int hdmi_TxSetPhyCtrl(UINT32 HdmiPort, UINT32 ParamID, UINT32 ParamVal)
{
    int RetVal = 0;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = -1;
    } else {
        AmbaRTSL_HdmiSetPhyCtrl(ParamID, ParamVal);
    }

    return RetVal;
}

/**
 *  hdmi_TxStart - Start HDMI/DVI video outputs
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
int hdmi_TxStart(UINT32 HdmiPort)
{
    int RetVal = 0;

//TODO EDID part is removed, different from threadx
    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = -1;
    } else {
        AmbaRTSL_HdmiStart(1U); /* Start HDMI signal encoder */
    }

    return RetVal;
}

/**
 *  hdmi_TxStop - Stop HDMI/DVI video outputs
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
int hdmi_TxStop(UINT32 HdmiPort)
{
    int RetVal = 0;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = -1;
    } else {
        AmbaRTSL_HdmiStop();    /* Stop DVI/HDMI signal encoder */
    }

    return RetVal;
}

/**
 *  hdmi_TxSetSourceInfo - Set source product information
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] pSrcProductInfo Source Product Information
 *  @return error code
 */
int hdmi_TxSetSourceInfo(UINT32 HdmiPort, const AMBA_HDMI_SOURCE_PRODUCT_INFO_s *pSrcProductInfo)
{
    int RetVal = 0;

    if ((HdmiPort >= NUM_HDMI_TX_PORT) || (pSrcProductInfo == NULL)) {
        RetVal = -1;
    } else {
        (void)AmbaWrap_memcpy(&AmbaHdmiCtrl.DeviceCfg, pSrcProductInfo, sizeof(AMBA_HDMI_SOURCE_PRODUCT_INFO_s));

        if (pSrcProductInfo->SrcType < NUM_HDMI_SRC_TYPE) {
            AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_SPD] = HDMI_PACKET_SEND_EVERY_FRAME;
        } else {
            AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_SPD] = HDMI_PACKET_SEND_NONE;
        }
    }

    return RetVal;
}

/**
 *  hdmi_TxSetInfoFramePacket - Confiugre the transmission of the specified HDMI InfoFrame packet
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] pInfoFramePkt pointer to InfoFrame packet data
 *  @param[in] TxMode InfoFrame packet transmission mode
 *  @return error code
 */
int hdmi_TxSetInfoFramePacket(UINT32 HdmiPort, const AMBA_HDMI_DATA_ISLAND_PACKET_s *pInfoFramePkt, UINT32 TxMode)
{
    AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket;
    UINT32 InfoFrameType = 0U;
    UINT32 i;
    int RetVal = 0;

    if ((HdmiPort >= NUM_HDMI_TX_PORT) || (pInfoFramePkt == NULL) || (TxMode >= NUM_HDMI_PACKET_SEND_MODE)) {
        RetVal = -1;
    } else {

        /* Update data */
        InfoFrameType = pInfoFramePkt->Header & 0x7fU;
        if (InfoFrameType >= NUM_HDMI_INFOFRAME_TYPE) {
            RetVal = -1;
        } else if ((InfoFrameType == HDMI_INFOFRAME_AUDIO) ||
                   (InfoFrameType == HDMI_INFOFRAME_AVI) ||
                   (InfoFrameType == HDMI_INFOFRAME_SPD) ||
                   (InfoFrameType == HDMI_INFOFRAME_VENDOR_SPECIFIC)) {
            RetVal = -1;
        } else {
            pPacket = &AmbaHdmiInfoFrameBuf[InfoFrameType];
            pPacket->Header = pInfoFramePkt->Header;
            for (i = 0U; i < HDMI_INFOFRAME_SIZE_MAX; i ++) {
                pPacket->Payload[i] = pInfoFramePkt->Payload[i];
            }
            AmbaHdmiInfoFrameTxMode[InfoFrameType] = TxMode;

            AmbaRTSL_HdmiDataIslandPackCtrl(pPacket, AmbaHdmiInfoFrameTxMode[InfoFrameType]);
        }

        if (RetVal == 0) {
            HDMI_RefreshInfoFrame(InfoFrameType);
        }
    }

    return RetVal;
}

/**
 *  hdmi_TxGetInfoFramePacket - Get HDMI current infoframe packet according to infoframe type, the packet data is valid when the infoframe is enabled
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] InfoFrameType InfoFrame packet type
 *  @param[in] pInfoFramePkt pointer to an InfoFrame packet data
 *  @return error code
 */
int hdmi_TxGetInfoFramePacket(UINT32 HdmiPort, UINT32 InfoFrameType, AMBA_HDMI_DATA_ISLAND_PACKET_s *pInfoFramePkt)
{
    int RetVal = 0;

    if ((HdmiPort >= NUM_HDMI_TX_PORT) || (InfoFrameType >= NUM_HDMI_INFOFRAME_TYPE) || (pInfoFramePkt == NULL)) {
        RetVal = -1;
    } else {
        (void)AmbaWrap_memcpy(pInfoFramePkt, &AmbaHdmiInfoFrameBuf[InfoFrameType], sizeof(AMBA_HDMI_DATA_ISLAND_PACKET_s));
    }

    return RetVal;
}

/**
 *  hdmi_TxSetAvMute - Enable the HDMI Audio/Video Mute (AVMUTE) feature
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
int hdmi_TxSetAvMute(UINT32 HdmiPort)
{
    int RetVal = 0;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = -1;
    } else {
        AmbaRTSL_HdmiSendAVMUTE(1U);
    }

    return RetVal;
}

/**
 *  hdmi_TxClearAvMute - Disable the HDMI Audio/Video Mute (AVMUTE) feature
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
int hdmi_TxClearAvMute(UINT32 HdmiPort)
{
    int RetVal = 0;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = -1;
    } else {
        AmbaRTSL_HdmiSendAVMUTE(0U);
    }

    return RetVal;
}
#if 0
/**
 *  HDMI_ValdidateEDID - Valdidate one EDID block
 *  @param[in] pEdidBlock pointer to a storage of EDID block
 *  @return error code
 */
static UINT32 HDMI_ValdidateEDID(const UINT8 *pEdidBlock)
{
    UINT8 Checksum = 0U;
    UINT32 i, RetVal = 0;

    for (i = 0U; i < EDID_BLOCK_SIZE; i++) {
        Checksum += pEdidBlock[i];
    }

    /* The checksum of the entire 128-byte BASE EDID shall equal 00h */
    if (Checksum != 0x00U) {
        RetVal = HDMI_ERR_EDID;
    }

    return RetVal;
}

/**
 *  HDMI_ReadEDID - Read one EDID block
 *  @param[in] EdidBlockNo EDID block number
 *  @param[out] pEdidBlock pointer to a storage of EDID block
 *  @return error code
 */
static UINT32 HDMI_ReadEDID(UINT32 EdidBlockNo, UINT8 *pEdidBlock)
{
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0U};
    AMBA_I2C_TRANSACTION_s I2cTxConfig[2U] = {0U};
    UINT8 DdcCmdBuf[2U];
    UINT32 RetVal = 0;

    if (pEdidBlock == NULL) {
        RetVal = -1;
    } else {
        /* Retrieve EDID data block by using Enhanced Display Data Channel (E-DDC) protocol */
        I2cTxConfig[0U].SlaveAddr = DDC_ADDR_SET_SEGMENT_POINTER;
        I2cTxConfig[0U].DataSize = 1U;
        I2cTxConfig[0U].pDataBuf = &DdcCmdBuf[0U];
        DdcCmdBuf[0U] = (UINT8)((EdidBlockNo >> 1U) & 0xffU);   /* Segment Pointer */

        I2cTxConfig[1U].SlaveAddr = DDC_ADDR_SET_WORD_OFFSET;
        I2cTxConfig[1U].DataSize = 1U;
        I2cTxConfig[1U].pDataBuf = &DdcCmdBuf[1U];
        if ((EdidBlockNo & 1U) == 0U) {
            DdcCmdBuf[1U] = 0x00U;                              /* Word Offset 0x00 (block 0) */
        } else {
            DdcCmdBuf[1U] = EDID_BLOCK_SIZE;                    /* Word Offset 0x80 (block 1) */
        }

        I2cRxConfig.SlaveAddr = DDC_ADDR_GET_EDID_BLOCK;
        I2cRxConfig.DataSize = EDID_BLOCK_SIZE;
        I2cRxConfig.pDataBuf = pEdidBlock;

        if (AmbaI2C_MasterReadAfterWrite(AMBA_I2C_HDMI_CHANNEL, I2C_SPEED_STANDARD, 2U, &I2cTxConfig[0], &I2cRxConfig, NULL, 1000U) != I2C_ERR_NONE) {
            /* Disconnect occurs or the DDC connection is unstable */
            RetVal = HDMI_ERR_DDC;
        } else {
            /* Validate EDID block by checksum */
            RetVal = HDMI_ValdidateEDID(pEdidBlock);
        }
    }

    return RetVal;
}
#endif
/**
 *  HDMI_DetectSink - Detect cable connection (DVI/HDMI) and read EDID if cable plugged
 *  @param[out] pHotPlugDetect Hot-Plug Detection Status
 *  @param[out] pEdidDetect EDID Detection Status
 *  @return error code
 */
static UINT32 HDMI_DetectSink(UINT32 *pHotPlugDetect, UINT32 *pEdidDetect)
{
#if 0
    const UINT8 *pEdidBlock = AmbaHdmiSinkEDID[0];
    const EDID_FORMAT_s *pEdidStruct;
    UINT32 HotPlugged, RxSensed;
    UINT32 i, RetVal = 0;

    /* Set default value */
    *pEdidDetect = HDMI_EDID_DVI;

    /* Check HPD and RxSense */
    AmbaRTSL_HdmiCheckConnection(&HotPlugged, &RxSensed);
    if (HotPlugged != 0U) {
        if (RxSensed != 0U) {
            *pHotPlugDetect = HDMI_HPD_ACTIVE;
        } else {
            *pHotPlugDetect = HDMI_HPD_STANDBY;
        }
    } else {
        /* No device is detected */
        *pHotPlugDetect = HDMI_HPD_NONE;
        *pEdidDetect = HDMI_EDID_UNAVAIL;
    }

    /* Read all available EDID blocks if HPD is active */
    if (HotPlugged != 0U) {
        /* If the cable is unplugged right after the connection check, it shall fail on E-DDC communication. */
        RetVal = HDMI_ReadEDID(0U, AmbaHdmiSinkEDID[0]);
        if (RetVal != 0) {
            *pEdidDetect = HDMI_EDID_UNAVAIL;
        } else {
            memcpy(&pEdidStruct, &pEdidBlock, 4);
            for (i = 1U; i <= pEdidStruct->ExtensionBlockCount; i ++) {
                RetVal = HDMI_ReadEDID(i, AmbaHdmiSinkEDID[i]);
                if (RetVal != 0) {
                    *pEdidDetect = HDMI_EDID_UNAVAIL;
                    break;
                }
            }
        }
    }

    if (*pEdidDetect != HDMI_EDID_UNAVAIL) {
        /* Parse EDID and determine whether the sink device is HDMI capable */
        RetVal = hdmi_TxParseEDID(AmbaHdmiSinkEDID, pEdidDetect);
    }

    return RetVal;
#endif
    *pEdidDetect = HDMI_EDID_DVI;
    return 0;
}
#if 0
/**
 *  HDMI_WriteSCDC - Write data to SCDC.
 *  @param[in] TxDataSize Data size in Byte
 *  @param[in] pTxDataBuf Pointer to the Tx data buffer (the 1st Byte can be the I2C Subaddress)
 *  @return error code
 */
static UINT32 HDMI_WriteSCDC(UINT32 TxDataSize, UINT8 *pTxDataBuf)
{
    AMBA_I2C_TRANSACTION_s TxTransaction;
    UINT32 RetVal = 0;

    if ((TxDataSize == 0U) || (pTxDataBuf == NULL)) {
        RetVal = -1;
    } else {
        TxTransaction.SlaveAddr = SCDC_ADDR_WRITE;
        TxTransaction.DataSize = TxDataSize;
        TxTransaction.pDataBuf = pTxDataBuf;
//TODO should i call io-mw?
        if (AmbaI2C_MasterWrite(AMBA_I2C_HDMI_CHANNEL, I2C_SPEED_STANDARD, &TxTransaction, NULL, 1000U) != I2C_ERR_NONE) {
            RetVal = HDMI_ERR_SCDC;
        }
    }

    return RetVal;
}
#endif
/**
 *  HDMI_SetClockRatio - Set clock ratio and config scrambler
 *  @param[in] PixelClock Pixel clock frequency
 */
static void HDMI_SetClockRatio(UINT32 PixelClock)
{
    UINT8 ScdcData[2U];

    if (PixelClock > 340000U) {
        /* Enable Scrambler with ratio 40 */
        AmbaRTSL_HdmiConfigScrambler(1U);
        ScdcData[1U] = 0x3U;
    } else {
        /* Disable Scrambler */
        AmbaRTSL_HdmiConfigScrambler(0U);
        ScdcData[1U] = 0x0U;
    }

    /* No matter SCDC is present or not */
    //ScdcData[0U] = 0x20U;           /* Offset of TMDS_Config */
    //(void)HDMI_WriteSCDC(2U, ScdcData);
}

/**
 *  HDMI_RefreshInfoFrame - Refresh infoframe packet configuration
 *  @param[in] InfoFrameType Type of InfoFrame packet
 */
static void HDMI_RefreshInfoFrame(UINT32 InfoFrameType)
{
    AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket;

    /* Refill Audio InfoFrame Packet */
    if ((InfoFrameType != HDMI_INFOFRAME_AUDIO) && (AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_AUDIO] != HDMI_PACKET_SEND_NONE)) {
        pPacket = &AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_AUDIO];
        AmbaRTSL_HdmiDataIslandPackCtrl(pPacket, AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_AUDIO]);
    }

    /* Refill Source Product Description InfoFrame Packet */
    if (AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_SPD] != HDMI_PACKET_SEND_NONE) {
        pPacket = &AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_SPD];
        AmbaRTSL_HdmiDataIslandPackCtrl(pPacket, AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_SPD]);
    }

    /* Refill AVI InforFrame Packet */
    if ((InfoFrameType != HDMI_INFOFRAME_AVI) && (AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_AVI] != HDMI_PACKET_SEND_NONE)) {
        pPacket = &AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_AVI];
        AmbaRTSL_HdmiDataIslandPackCtrl(pPacket, AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_AVI]);
    }

    /* Refill Vendor Specific InfoFrame Packet */
    if ((InfoFrameType != HDMI_INFOFRAME_VENDOR_SPECIFIC) && (AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_VENDOR_SPECIFIC] != HDMI_PACKET_SEND_NONE)) {
        pPacket = &AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_VENDOR_SPECIFIC];
        AmbaRTSL_HdmiDataIslandPackCtrl(pPacket, AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_VENDOR_SPECIFIC]);
    }

    /* Update all infoframe packets */
    AmbaRTSL_HdmiPacketUpdate();
}

/**
 *  HDMI_ValidateConfig - Check if the specified configuration could be supported or not.
 *  @param[in] VideoIdCode video id code
 *  @param[in] pVideoConfig pointer to video format configuration
 *  @param[in] pAudioConfig pointer to audio format configuration
 *  @return error code
 */
static UINT32 HDMI_ValidateConfig(UINT8 VideoIdCode, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig)
{
    const AMBA_HDMI_VIDEO_TIMING_s *pDtd = &AmbaHdmiCtrl.VideoCfg.CustomDTD;
    UINT32 RetVal = 0;

    if (pAudioConfig != NULL) {
        AmbaHdmiCtrl.AudioCfg.SampleRate = pAudioConfig->SampleRate;
        AmbaHdmiCtrl.AudioCfg.SpeakerMap = pAudioConfig->SpeakerMap;
    } else {
        AmbaHdmiCtrl.AudioCfg.SampleRate = HDMI_AUDIO_FS_48K;
        AmbaHdmiCtrl.AudioCfg.SpeakerMap = HDMI_CA_2CH;
    }

    if (AmbaHdmiCtrl.AudioCfg.SpeakerMap < NUM_HDMI_AUDIO_CHANNEL_MAP) {
        AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_AUDIO] = HDMI_PACKET_SEND_EVERY_FRAME;
    } else {
        AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_AUDIO] = HDMI_PACKET_SEND_NONE;
    }

    AmbaHdmiCtrl.VideoCfg.VideoIdCode = pVideoConfig->VideoIdCode;
    AmbaHdmiCtrl.VideoCfg.PixelFormat = pVideoConfig->PixelFormat;
    AmbaHdmiCtrl.VideoCfg.ColorDepth = pVideoConfig->ColorDepth;
    AmbaHdmiCtrl.VideoCfg.QuantRange = pVideoConfig->QuantRange;

    /* Update video timing details. */
    if (VideoIdCode != HDMI_VIC_CUSTOM) {
        RetVal = AmbaRTSL_HdmiVicGet(VideoIdCode, &AmbaHdmiCtrl.VideoCfg.CustomDTD);
    } else {
        RetVal = AmbaRTSL_HdmiDtdCopy(&AmbaHdmiCtrl.VideoCfg.CustomDTD, &pVideoConfig->CustomDTD);
    }

    /* Only 480I and 576I support pixel repetition */
    if ((RetVal == 0) && (pDtd->PixelRepeat != 0U)) {
        if (pDtd->ScanFormat != AMBA_VIDEO_SCAN_INTERLACED) {
            RetVal = HDMI_ERR_NA;       /* Not interlaced */
        } else if ((pDtd->ActivePixels == 1440U) && (pDtd->ActiveLines == 480U)) {
            if ((pDtd->HsyncFrontPorch != 38U) ||
                (pDtd->HsyncPulseWidth != 124U) ||
                (pDtd->HsyncBackPorch != 114U) ||
                (pDtd->VsyncFrontPorch != 4U) ||
                (pDtd->VsyncPulseWidth != 3U) ||
                (pDtd->VsyncBackPorch != 15U)) {
                RetVal = HDMI_ERR_NA;   /* Not 480I */
            }
        } else if ((pDtd->ActivePixels == 1440U) && (pDtd->ActiveLines == 576U)) {
            if ((pDtd->HsyncFrontPorch != 24U) ||
                (pDtd->HsyncPulseWidth != 126U) ||
                (pDtd->HsyncBackPorch != 138U) ||
                (pDtd->VsyncFrontPorch != 2U) ||
                (pDtd->VsyncPulseWidth != 3U) ||
                (pDtd->VsyncBackPorch != 19U)) {
                RetVal = HDMI_ERR_NA;   /* Not 576I */
            }
        } else {
            RetVal = HDMI_ERR_NA;       /* Neither 480I nor 576I */
        }
    }

    return RetVal;
}

