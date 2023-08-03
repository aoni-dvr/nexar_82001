#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaKAL.h"
#include "AmbaDrvEntry.h"
#include "AmbaHDMI.h"
#include "AmbaHDMI_Ctrl.h"
#include "AmbaRTSL_HDMI.h"
#include "AmbaCSL_HDMI.h"
#include "AmbaI2S.h"
#include "AmbaReg_I2S.h"
#include "AmbaReg_Scratchpad.h"
#include "AmbaReg_RCT.h"
#include "AmbaReg_HDMI.h"

typedef struct {
    AMBA_HDMI_VIDEO_CONFIG_s    VideoCfg;
    AMBA_HDMI_AUDIO_CONFIG_s    AudioCfg;
    AMBA_HDMI_SOURCE_PRODUCT_INFO_s DeviceCfg;
} AMBA_HDMI_CTRL_s;

AMBA_HDMI_REG_s AmbaHDMI_Reg = {0};
AMBA_HDMI_REG_s *pAmbaHDMI_Reg = &AmbaHDMI_Reg;
AMBA_RCT_REG_s AmbaRCT_Reg = {0};
AMBA_RCT_REG_s *const pAmbaRCT_Reg = &AmbaRCT_Reg;
AMBA_SCRATCHPAD_REG_s AmbaScratchpadS_Reg = {0};
AMBA_SCRATCHPAD_REG_s *const pAmbaScratchpadS_Reg = &AmbaScratchpadS_Reg;
AMBA_I2S_REG_s *pAmbaI2S_Reg[AMBA_NUM_I2S_CHANNEL];

extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);
extern void Set_NoCopy_AmbaWrap_memcpy(UINT32 NoCopy);
extern void AmbaMisra_TypeCast(void * pNewType, const void * pOldType);

void TEST_AmbaHDMI_TxEnable(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    AmbaHDMI_TxEnable(HdmiPort);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_TxEnable(HdmiPort);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_TxEnable(HdmiPort);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_TxEnable(HdmiPort);

    printf("TEST_AmbaHDMI_TxEnable\n");
}

void TEST_AmbaHDMI_TxDisable(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    AmbaHDMI_TxDisable(HdmiPort);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_TxDisable(HdmiPort);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_TxDisable(HdmiPort);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_TxDisable(HdmiPort);

    printf("TEST_AmbaHDMI_TxDisable\n");
}

void TEST_AmbaHDMI_HookHpdHandler(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    AMBA_HDMI_HPD_ISR_f IntHandler;
    AmbaHDMI_HookHpdHandler(HdmiPort, IntHandler);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_HookHpdHandler(HdmiPort, IntHandler);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_HookHpdHandler(HdmiPort, IntHandler);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_HookHpdHandler(HdmiPort, IntHandler);

    printf("TEST_AmbaHDMI_HookHpdHandler\n");
}

void TEST_AmbaHDMI_HookCecHandler(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    AMBA_HDMI_CEC_ISR_f IntHandler;
    AmbaHDMI_HookCecHandler(HdmiPort, IntHandler);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_HookCecHandler(HdmiPort, IntHandler);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_HookCecHandler(HdmiPort, IntHandler);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_HookCecHandler(HdmiPort, IntHandler);

    printf("TEST_AmbaHDMI_HookCecHandler\n");
}

void TEST_AmbaHDMI_TxGetSinkInfo(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    UINT32 HotPlugDetect = 0U;
    UINT32 *pHotPlugDetect = &HotPlugDetect;
    UINT32 EdidDetect = 0U;
    UINT32 *pEdidDetect = &EdidDetect;
    AMBA_HDMI_SINK_INFO_s SinkInfo = {0};
    AMBA_HDMI_SINK_INFO_s *pSinkInfo = &SinkInfo;
    AmbaHDMI_TxGetSinkInfo(HdmiPort, NULL, NULL, NULL);

    AmbaHDMI_TxGetSinkInfo(HdmiPort, pHotPlugDetect, NULL, NULL);

    AmbaHDMI_TxGetSinkInfo(HdmiPort, pHotPlugDetect, pEdidDetect, NULL);

    AmbaHDMI_TxGetSinkInfo(HdmiPort, pHotPlugDetect, pEdidDetect, pSinkInfo);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_TxGetSinkInfo(HdmiPort, pHotPlugDetect, pEdidDetect, pSinkInfo);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_TxGetSinkInfo(HdmiPort, pHotPlugDetect, pEdidDetect, pSinkInfo);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_TxGetSinkInfo(HdmiPort, pHotPlugDetect, pEdidDetect, pSinkInfo);

    printf("TEST_AmbaHDMI_TxGetSinkInfo\n");
}

void TEST_AmbaHDMI_TxSetMode(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    AmbaHDMI_TxSetMode(HdmiPort, NULL, NULL);

    AMBA_HDMI_VIDEO_CONFIG_s VideoConfig = {0};
    AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig = &VideoConfig;
    AMBA_HDMI_AUDIO_CONFIG_s AudioConfig = {0};
    AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig = &AudioConfig;

    /* Avoid Floating point exception (Integer divide by zero) in AmbaHDMI_ConfigDSP */
    AMBA_HDMI_VIDEO_TIMING_s *pDtd = &pVideoConfig->CustomDTD;
    pDtd->HsyncFrontPorch = 1U;
    pDtd->VsyncFrontPorch = 2U;
    pDtd->HsyncBackPorch  = 3U;
    pDtd->ActivePixels    = 4U;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    pAmbaHDMI_Reg->Status.HotPlugDetected = 0x1U;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    /* AmbaRTSL_HDMI_Video.c */
    pVideoConfig->VideoIdCode = HDMI_VIC_CUSTOM;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    pVideoConfig->VideoIdCode = HDMI_VIC_480P;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    pVideoConfig->VideoIdCode = HDMI_VIC_720P;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    pVideoConfig->VideoIdCode = HDMI_VIC_2160P60_WIDE + 1U;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    /* AmbaHDMI_Info.c */
    pVideoConfig->VideoIdCode = HDMI_VIC_DMT0659;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    pVideoConfig->QuantRange = HDMI_QRANGE_FULL;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    pVideoConfig->QuantRange = HDMI_QRANGE_LIMITED;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    pVideoConfig->VideoIdCode = HDMI_FRAME_RATE_STRICT;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    pVideoConfig->PixelFormat = HDMI_PIXEL_FORMAT_YCC_444;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    pVideoConfig->PixelFormat = HDMI_PIXEL_FORMAT_YCC_422;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    pVideoConfig->PixelFormat = HDMI_PIXEL_FORMAT_YCC_420;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    pVideoConfig->CustomDTD.SyncPolarity = AMBA_VIDEO_SYNC_H_NEG_V_NEG;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    pVideoConfig->CustomDTD.SyncPolarity = AMBA_VIDEO_SYNC_H_NEG_V_POS;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    pVideoConfig->CustomDTD.SyncPolarity = AMBA_VIDEO_SYNC_H_POS_V_NEG;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    pDtd->ScanFormat = AMBA_VIDEO_SCAN_INTERLACED;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);

    printf("TEST_AmbaHDMI_TxSetMode\n");
}

void TEST_AmbaHDMI_TxGetCurrModeInfo(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    AmbaHDMI_TxGetCurrModeInfo(HdmiPort, NULL);

    AMBA_HDMI_MODE_INFO_s ModeInfo = {0};
    AMBA_HDMI_MODE_INFO_s *pModeInfo = &ModeInfo;
    AmbaHDMI_TxGetCurrModeInfo(HdmiPort, pModeInfo);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_TxGetCurrModeInfo(HdmiPort, pModeInfo);

    AMBA_HDMI_CTRL_s AmbaHdmiCtrl = {0};
    AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig = &AmbaHdmiCtrl.VideoCfg;
    pVideoConfig->VideoIdCode = HDMI_FRAME_RATE_STRICT;
    AmbaHDMI_TxGetCurrModeInfo(HdmiPort, pModeInfo);

    printf("TEST_AmbaHDMI_TxGetCurrModeInfo\n");
}

void TEST_AmbaHDMI_TxSetPhyCtrl(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    UINT32 ParamID = 0U;
    UINT32 ParamVal = 0U;
    AmbaHDMI_TxSetPhyCtrl(HdmiPort, ParamID, ParamVal);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_TxSetPhyCtrl(HdmiPort, ParamID, ParamVal);

    printf("TEST_AmbaHDMI_TxSetPhyCtrl\n");
}

void TEST_AmbaHDMI_TxStart(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    AMBA_HDMI_VIDEO_CONFIG_s VideoConfig = {0};
    AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig = &VideoConfig;
    AMBA_HDMI_AUDIO_CONFIG_s AudioConfig = {0};
    AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig = &AudioConfig;

    /* Avoid Floating point exception (Integer divide by zero) in AmbaHDMI_ConfigDSP */
    AMBA_HDMI_VIDEO_TIMING_s *pDtd = &pVideoConfig->CustomDTD;
    pDtd->HsyncFrontPorch = 1U;
    pDtd->VsyncFrontPorch = 2U;
    pDtd->HsyncBackPorch  = 3U;
    pDtd->ActivePixels    = 4U;
    
    pVideoConfig->CustomDTD.PixelClock = 1U;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);
    AmbaHDMI_TxStart(HdmiPort);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_TxStart(HdmiPort);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_TxStart(HdmiPort);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    pVideoConfig->CustomDTD.PixelClock = 0U;
    AmbaHDMI_TxSetMode(HdmiPort, pVideoConfig, pAudioConfig);
    AmbaHDMI_TxStart(HdmiPort);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_TxStart(HdmiPort);

    printf("TEST_AmbaHDMI_TxStart\n");
}

void TEST_AmbaHDMI_TxStop(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    AmbaHDMI_TxStop(HdmiPort);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_TxStop(HdmiPort);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_TxStop(HdmiPort);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_TxStop(HdmiPort);

    printf("TEST_AmbaHDMI_TxStop\n");
}

void TEST_AmbaHDMI_TxSetSourceInfo(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    AmbaHDMI_TxSetSourceInfo(HdmiPort, NULL);

    AMBA_HDMI_SOURCE_PRODUCT_INFO_s SrcProductInfo = {0};
    AMBA_HDMI_SOURCE_PRODUCT_INFO_s *pSrcProductInfo = &SrcProductInfo;
    AmbaHDMI_TxSetSourceInfo(HdmiPort, pSrcProductInfo);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_TxSetSourceInfo(HdmiPort, pSrcProductInfo);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    pSrcProductInfo->SrcType = NUM_HDMI_SRC_TYPE - 1U;
    AmbaHDMI_TxSetSourceInfo(HdmiPort, pSrcProductInfo);

    pSrcProductInfo->SrcType = NUM_HDMI_SRC_TYPE;
    AmbaHDMI_TxSetSourceInfo(HdmiPort, pSrcProductInfo);

    Set_NoCopy_AmbaWrap_memcpy(1U);
    AmbaHDMI_TxSetSourceInfo(HdmiPort, pSrcProductInfo);
    Set_NoCopy_AmbaWrap_memcpy(0U);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_TxSetSourceInfo(HdmiPort, pSrcProductInfo);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_TxSetSourceInfo(HdmiPort, pSrcProductInfo);

    printf("TEST_AmbaHDMI_TxSetSourceInfo\n");
}

void TEST_AmbaHDMI_TxSetInfoFramePacket(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    UINT32 TxMode = NUM_HDMI_PACKET_SEND_MODE - 1U;
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, NULL, TxMode);

    AMBA_HDMI_DATA_ISLAND_PACKET_s InfoFramePkt = {0};
    AMBA_HDMI_DATA_ISLAND_PACKET_s *pInfoFramePkt = &InfoFramePkt;
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, pInfoFramePkt, TxMode);

    pInfoFramePkt->Header = NUM_HDMI_INFOFRAME_TYPE;
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, pInfoFramePkt, TxMode);

    pInfoFramePkt->Header = HDMI_INFOFRAME_VENDOR_SPECIFIC;
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, pInfoFramePkt, TxMode);

    pInfoFramePkt->Header = HDMI_INFOFRAME_SPD;
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, pInfoFramePkt, TxMode);

    pInfoFramePkt->Header = HDMI_INFOFRAME_AVI;
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, pInfoFramePkt, TxMode);

    pInfoFramePkt->Header = HDMI_INFOFRAME_AUDIO;
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, pInfoFramePkt, TxMode);

    pInfoFramePkt->Header = HDMI_INFOFRAME_RESERVED;
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, pInfoFramePkt, TxMode);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, pInfoFramePkt, TxMode);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, pInfoFramePkt, TxMode);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    TxMode = HDMI_PACKET_SEND_NONE;
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, pInfoFramePkt, TxMode);

    TxMode = HDMI_PACKET_SEND_ONCE;
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, pInfoFramePkt, TxMode);

    TxMode = HDMI_PACKET_SEND_EVERY_FRAME;
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, pInfoFramePkt, TxMode);

    TxMode = NUM_HDMI_PACKET_SEND_MODE;
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, pInfoFramePkt, TxMode);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_TxSetInfoFramePacket(HdmiPort, pInfoFramePkt, TxMode);

    printf("TEST_AmbaHDMI_TxSetInfoFramePacket\n");
}

void TEST_AmbaHDMI_TxGetInfoFramePacket(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    UINT32 InfoFrameType = NUM_HDMI_INFOFRAME_TYPE - 1U;
    AmbaHDMI_TxGetInfoFramePacket(HdmiPort, InfoFrameType, NULL);

    AMBA_HDMI_DATA_ISLAND_PACKET_s InfoFramePkt = {0};
    AMBA_HDMI_DATA_ISLAND_PACKET_s *pInfoFramePkt = &InfoFramePkt;
    AmbaHDMI_TxGetInfoFramePacket(HdmiPort, InfoFrameType, pInfoFramePkt);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_TxGetInfoFramePacket(HdmiPort, InfoFrameType, pInfoFramePkt);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_TxGetInfoFramePacket(HdmiPort, InfoFrameType, pInfoFramePkt);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    InfoFrameType = NUM_HDMI_INFOFRAME_TYPE;
    AmbaHDMI_TxGetInfoFramePacket(HdmiPort, InfoFrameType, pInfoFramePkt);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_TxGetInfoFramePacket(HdmiPort, InfoFrameType, pInfoFramePkt);

    printf("TEST_AmbaHDMI_TxGetInfoFramePacket\n");
}

void TEST_AmbaHDMI_TxSetAvMute(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    AmbaHDMI_TxSetAvMute(HdmiPort);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_TxSetAvMute(HdmiPort);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_TxSetAvMute(HdmiPort);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_TxSetAvMute(HdmiPort);

    printf("TEST_AmbaHDMI_TxSetAvMute\n");
}

void TEST_AmbaHDMI_TxClearAvMute(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    AmbaHDMI_TxClearAvMute(HdmiPort);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_TxClearAvMute(HdmiPort);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_TxClearAvMute(HdmiPort);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_TxClearAvMute(HdmiPort);

    printf("TEST_AmbaHDMI_TxClearAvMute\n");
}

void TEST_AmbaHDMI_TxEnableTestPattern(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    AmbaHDMI_TxEnableTestPattern(HdmiPort);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_TxEnableTestPattern(HdmiPort);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_TxEnableTestPattern(HdmiPort);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_TxEnableTestPattern(HdmiPort);

    printf("TEST_AmbaHDMI_TxEnableTestPattern\n");
}

void TEST_AmbaHDMI_TxCopySinkInfo(void)
{
    AMBA_HDMI_SINK_INFO_s SinkInfo = {0};
    AMBA_HDMI_SINK_INFO_s *pSinkInfo = &SinkInfo;
    AmbaHDMI_TxCopySinkInfo(NULL);

    AmbaHDMI_TxCopySinkInfo(pSinkInfo);

    printf("TEST_AmbaHDMI_TxCopySinkInfo\n");
}

void TEST_AmbaHDMI_TxParseEDID(void)
{
    UINT8 pEDID[EDID_NUM_BLOCKS][EDID_BLOCK_SIZE];
    UINT32 EdidDetect = 0U;
    UINT32 *pEdidDetect = &EdidDetect;
    EDID_FORMAT_s *pEdidStruct = {0};
    UINT8 *pEdidBlock = pEDID[0];
    AmbaHDMI_TxParseEDID(pEDID, pEdidDetect);

    AmbaMisra_TypeCast(&pEdidStruct, &pEdidBlock);

    pEdidStruct->VersionNumber = 1U;
    AmbaHDMI_TxParseEDID(pEDID, pEdidDetect);

    pEdidStruct->RevisionNumber = 3U;
    AmbaHDMI_TxParseEDID(pEDID, pEdidDetect);

    pEdidStruct->ExtensionBlockCount = 1U;
    AmbaHDMI_TxParseEDID(pEDID, pEdidDetect);

    pEdidStruct->VideoInputDefinition = 0x80U;
    AmbaHDMI_TxParseEDID(pEDID, pEdidDetect);

    pEdidStruct->FeatureSupport = 0x08U;
    AmbaHDMI_TxParseEDID(pEDID, pEdidDetect);

    pEdidStruct->FeatureSupport = 0x10U;
    AmbaHDMI_TxParseEDID(pEDID, pEdidDetect);

    pEdidStruct->RevisionNumber = 4U;
    AmbaHDMI_TxParseEDID(pEDID, pEdidDetect);

    pEdidStruct->FeatureSupport = 0x2U;
    AmbaHDMI_TxParseEDID(pEDID, pEdidDetect);

    /* AmbaRTSL_HDMI_Video.c */
    pEdidStruct->Descriptor[0][0] = 1U;
    AmbaHDMI_TxParseEDID(pEDID, pEdidDetect);

    printf("TEST_AmbaHDMI_TxParseEDID\n");
}

void TEST_AmbaHDMI_AudioChannelSelect(void)
{
    UINT32 HDMIAudioChannel = 0U;
    AmbaHDMI_AudioChannelSelect(HDMIAudioChannel);

    printf("TEST_AmbaHDMI_AudioChannelSelect\n");
}

void TEST_AmbaHDMI_TxConfigAudio(void)
{
    AMBA_HDMI_VIDEO_CONFIG_s VideoConfig = {0};
    AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig = &VideoConfig;
    AMBA_HDMI_AUDIO_CONFIG_s AudioConfig = {0};
    AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig = &AudioConfig;
    AmbaHDMI_TxConfigAudio(pVideoConfig, pAudioConfig);

    pVideoConfig->ColorDepth = 1U;
    AmbaHDMI_TxConfigAudio(pVideoConfig, pAudioConfig);

    pVideoConfig->ColorDepth = HDMI_COLOR_DEPTH_24BPP;
    AmbaHDMI_TxConfigAudio(pVideoConfig, pAudioConfig);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_TxConfigAudio(pVideoConfig, pAudioConfig);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_TxConfigAudio(pVideoConfig, pAudioConfig);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaHDMI_TxConfigAudio\n");
}

void TEST_AmbaHDMI_TxSendAudioInfoFrame(void)
{
    AMBA_HDMI_DATA_ISLAND_PACKET_s Packet = {0};
    AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket = &Packet;
    AMBA_HDMI_AUDIO_CONFIG_s AudioConfig = {0};
    AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig = &AudioConfig;
    UINT32 TxMode = 0U;
    AmbaHDMI_TxSendAudioInfoFrame(pPacket, pAudioConfig, TxMode);

    AmbaHDMI_TxSendAudioInfoFrame(pPacket, NULL, TxMode);

    printf("TEST_AmbaHDMI_TxSendAudioInfoFrame\n");
}

void TEST_AmbaHDMI_TxSendAviInfoFrame(void)
{
    AMBA_HDMI_DATA_ISLAND_PACKET_s Packet = {0};
    AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket = &Packet;
    AMBA_HDMI_VIDEO_CONFIG_s VideoConfig = {0};
    AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig = &VideoConfig; 
    UINT32 TxMode = 0U;
    AmbaHDMI_TxSendAviInfoFrame(pPacket, pVideoConfig, TxMode);

    AmbaHDMI_TxSendAviInfoFrame(pPacket, NULL, TxMode);

    pVideoConfig->PixelFormat = HDMI_PIXEL_FORMAT_YCC_422;
    AmbaHDMI_TxSendAviInfoFrame(pPacket, pVideoConfig, TxMode);

    pVideoConfig->PixelFormat = HDMI_PIXEL_FORMAT_YCC_444;
    AmbaHDMI_TxSendAviInfoFrame(pPacket, pVideoConfig, TxMode);

    pVideoConfig->PixelFormat = HDMI_PIXEL_FORMAT_YCC_420;
    AmbaHDMI_TxSendAviInfoFrame(pPacket, pVideoConfig, TxMode);

    pVideoConfig->VideoIdCode = HDMI_VIC_2160P30;
    AmbaHDMI_TxSendAviInfoFrame(pPacket, pVideoConfig, TxMode);

    pVideoConfig->VideoIdCode = HDMI_VIC_2160P25;
    AmbaHDMI_TxSendAviInfoFrame(pPacket, pVideoConfig, TxMode);

    pVideoConfig->VideoIdCode = HDMI_VIC_2160P24;
    AmbaHDMI_TxSendAviInfoFrame(pPacket, pVideoConfig, TxMode);

    pVideoConfig->VideoIdCode = HDMI_VIC_2160P24_ULTRAWIDE;
    AmbaHDMI_TxSendAviInfoFrame(pPacket, pVideoConfig, TxMode);

    pVideoConfig->QuantRange = HDMI_QRANGE_FULL;
    AmbaHDMI_TxSendAviInfoFrame(pPacket, pVideoConfig, TxMode);

    pVideoConfig->VideoIdCode = HDMI_VIC_DMT0659;
    AmbaHDMI_TxSendAviInfoFrame(pPacket, pVideoConfig, TxMode);

    pVideoConfig->QuantRange = HDMI_QRANGE_DEFAULT;
    AmbaHDMI_TxSendAviInfoFrame(pPacket, pVideoConfig, TxMode);

    printf("TEST_AmbaHDMI_TxSendAviInfoFrame\n");
}

void TEST_AmbaHDMI_TxSendSpdInfoFrame(void)
{
    AMBA_HDMI_DATA_ISLAND_PACKET_s Packet = {0};
    AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket = &Packet;
    AMBA_HDMI_SOURCE_PRODUCT_INFO_s SPD = {0};
    AMBA_HDMI_SOURCE_PRODUCT_INFO_s *pSPD = &SPD; 
    UINT32 TxMode = 0U;
    AmbaHDMI_TxSendSpdInfoFrame(pPacket, pSPD, TxMode);

    AmbaHDMI_TxSendSpdInfoFrame(pPacket, NULL, TxMode);

    printf("TEST_AmbaHDMI_TxSendSpdInfoFrame\n");
}

void TEST_AmbaHDMI_TxSendVendorInfoFrame(void)
{
    AMBA_HDMI_DATA_ISLAND_PACKET_s Packet = {0};
    AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket = &Packet;
    AMBA_HDMI_VIDEO_CONFIG_s VideoConfig = {0};
    AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig = &VideoConfig; 
    UINT32 TxMode = 0U;
    AmbaHDMI_TxSendVendorInfoFrame(pPacket, pVideoConfig, TxMode);

    AmbaHDMI_TxSendVendorInfoFrame(pPacket, NULL, TxMode);

    pVideoConfig->VideoIdCode = HDMI_VIC_2160P30;
    AmbaHDMI_TxSendVendorInfoFrame(pPacket, pVideoConfig, TxMode);

    pVideoConfig->VideoIdCode = HDMI_VIC_2160P25;
    AmbaHDMI_TxSendVendorInfoFrame(pPacket, pVideoConfig, TxMode);

    pVideoConfig->VideoIdCode = HDMI_VIC_2160P24;
    AmbaHDMI_TxSendVendorInfoFrame(pPacket, pVideoConfig, TxMode);

    pVideoConfig->VideoIdCode = HDMI_VIC_2160P24_ULTRAWIDE;
    AmbaHDMI_TxSendVendorInfoFrame(pPacket, pVideoConfig, TxMode);

    printf("TEST_AmbaHDMI_TxSendVendorInfoFrame\n");
}

void TEST_AmbaHDMI_CecEnable(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    UINT32 LogicalAddr = 0U;
    AmbaHDMI_CecEnable(HdmiPort, LogicalAddr);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_CecEnable(HdmiPort, LogicalAddr);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_CecEnable(HdmiPort, LogicalAddr);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_CecEnable(HdmiPort, LogicalAddr);

    printf("TEST_AmbaHDMI_CecEnable\n");
}

void TEST_AmbaHDMI_CecRead(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    AmbaHDMI_CecRead(HdmiPort, NULL, NULL);

    UINT8 Msg = 0U;
    UINT8 *pMsg = &Msg;
    AmbaHDMI_CecRead(HdmiPort, pMsg, NULL);

    UINT32 MsgSize = 0U;
    UINT32 *pMsgSize = &MsgSize;
    AmbaHDMI_CecRead(HdmiPort, pMsg, pMsgSize);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_CecRead(HdmiPort, pMsg, pMsgSize);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_CecRead(HdmiPort, pMsg, pMsgSize);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_CecRead(HdmiPort, pMsg, pMsgSize);

    printf("TEST_AmbaHDMI_CecRead\n");
}

void TEST_AmbaHDMI_CecWrite(void)
{
    UINT32 HdmiPort = NUM_HDMI_TX_PORT - 1U;
    UINT8 Msg = 0U;
    UINT8 *pMsg = &Msg;
    UINT32 MsgSize = 0U;
    AmbaHDMI_CecWrite(HdmiPort, pMsg, MsgSize);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaHDMI_CecWrite(HdmiPort, pMsg, MsgSize);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaHDMI_CecWrite(HdmiPort, pMsg, MsgSize);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    HdmiPort = NUM_HDMI_TX_PORT;
    AmbaHDMI_CecWrite(HdmiPort, pMsg, MsgSize);

    printf("TEST_AmbaHDMI_CecWrite\n");
}

void TEST_AmbaRTSL_HdmiEnableAudioChan(void)
{
    UINT32 AudChanNo = 0U;
    AmbaRTSL_HdmiEnableAudioChan(AudChanNo);

    AudChanNo = 1U;
    AmbaRTSL_HdmiEnableAudioChan(AudChanNo);

    AudChanNo = 2U;
    AmbaRTSL_HdmiEnableAudioChan(AudChanNo);

    printf("TEST_AmbaRTSL_HdmiEnableAudioChan\n");
}

void TEST_AmbaRTSL_HdmiDisableAudioChan(void)
{
    UINT32 AudChanNo = 0U;
    AmbaRTSL_HdmiDisableAudioChan(AudChanNo);

    AudChanNo = 1U;
    AmbaRTSL_HdmiDisableAudioChan(AudChanNo);

    AudChanNo = 2U;
    AmbaRTSL_HdmiDisableAudioChan(AudChanNo);

    printf("TEST_AmbaRTSL_HdmiDisableAudioChan\n");
}

int main(void)
{
    /* AmbaHDMI.c */
    for (UINT8 i = 0U; i < AMBA_NUM_I2S_CHANNEL; i++) {
        pAmbaI2S_Reg[i] = malloc(sizeof(AMBA_I2S_REG_s));
    }
    
    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaHDMI_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    /* avoid AmbaHDMI_DrvEntry to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    AmbaHDMI_DrvEntry();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    TEST_AmbaHDMI_TxEnable();
    TEST_AmbaHDMI_TxDisable();
    TEST_AmbaHDMI_HookHpdHandler();
    TEST_AmbaHDMI_HookCecHandler();
    TEST_AmbaHDMI_TxGetSinkInfo();
    TEST_AmbaHDMI_TxSetMode();
    TEST_AmbaHDMI_TxGetCurrModeInfo();
    TEST_AmbaHDMI_TxSetPhyCtrl();
    TEST_AmbaHDMI_TxStart();
    TEST_AmbaHDMI_TxStop();
    TEST_AmbaHDMI_TxSetSourceInfo();
    TEST_AmbaHDMI_TxSetInfoFramePacket();
    TEST_AmbaHDMI_TxGetInfoFramePacket();
    TEST_AmbaHDMI_TxSetAvMute();
    TEST_AmbaHDMI_TxClearAvMute();
    TEST_AmbaHDMI_TxEnableTestPattern();

    /* AmbaHDMI_Info.c */
    TEST_AmbaHDMI_TxCopySinkInfo();
    TEST_AmbaHDMI_TxParseEDID();
    TEST_AmbaHDMI_AudioChannelSelect();
    TEST_AmbaHDMI_TxConfigAudio();
    TEST_AmbaHDMI_TxSendAudioInfoFrame();
    TEST_AmbaHDMI_TxSendAviInfoFrame();
    TEST_AmbaHDMI_TxSendSpdInfoFrame();
    TEST_AmbaHDMI_TxSendVendorInfoFrame();

    /* AmbaHDMI_CEC.c */
    TEST_AmbaHDMI_CecEnable();
    TEST_AmbaHDMI_CecRead();
    TEST_AmbaHDMI_CecWrite();

    /* AmbaRTSL_HDMI_Audio.c */
    TEST_AmbaRTSL_HdmiEnableAudioChan();
    TEST_AmbaRTSL_HdmiDisableAudioChan();

    return 0;
}

