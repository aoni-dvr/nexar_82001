// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
 *  @file AmbaHDMI_Info.c
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
 *
 *  @details EDID and InfoFrame Data Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

#include "AmbaHDMI.h"
#include "AmbaHDMI_Ctrl.h"
//#include "AmbaVOUT_Ctrl.h"
//#include "AmbaRTSL_HDMI.h"
//#include "AmbaRTSL_VOUT.h"

/**
 *  AmbaHDMI_TxCopySinkInfo - Duplicate sink info data structure
 *  @param[out] pSinkInfo HDMI sink info
 *  @return error code
 */
UINT32 AmbaHDMI_TxCopySinkInfo(AMBA_HDMI_SINK_INFO_s *pSinkInfo)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    (void) pSinkInfo;
    return RetVal;
}



/**
 *  AmbaHDMI_TxParseEDID - Parse EDID to check if the display device supports features specific to HDMI
 *  @param[in] pEDID EDID buffer
 *  @param[out] pEdidDetect Indicate if the HDMI tag is found in EDID
 *  @return error code
 */
UINT32 AmbaHDMI_TxParseEDID(const UINT8 pEDID[EDID_NUM_BLOCKS][EDID_BLOCK_SIZE], UINT32 *pEdidDetect)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    (void) pEDID;
    (void) pEdidDetect;
    return RetVal;
}

/**
 *  AmbaHDMI_TxConfigAudio - Configure audio
 *  @param[in] pVideoConfig pointer to video format configuration
 *  @param[in] pAudioConfig pointer to audio format configuration
 *  @return error code
 */
UINT32 AmbaHDMI_TxConfigAudio(const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    (void) pVideoConfig;
    (void) pAudioConfig;
    return RetVal;
}

/**
 *  AmbaHDMI_TxSendAudioInfoFrame - Fill Audio InfoFrame Packet
 *  @param[in] pPacket pointer to a packet buffer
 *  @param[in] pAudioConfig pointer to audio format configuration
 *  @param[in] TxMode Packet transmission frequency
 *  @return error code
 */
UINT32 AmbaHDMI_TxSendAudioInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig, UINT32 TxMode)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    (void) pPacket;
    (void) pAudioConfig;
    (void) TxMode;
    return RetVal;
}

/**
 *  AmbaHDMI_TxSendAviInfoFrame - Fill AVI InfoFrame Packet
 *  @param[in] pPacket pointer to a packet buffer
 *  @param[in] pVideoConfig pointer to video format configuration
 *  @param[in] TxMode Packet transmission frequency
 *  @return error code
 */
UINT32 AmbaHDMI_TxSendAviInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, UINT32 TxMode)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    (void) pPacket;
    (void) pVideoConfig;
    (void) TxMode;
    return RetVal;
}

/**
 *  AmbaHDMI_TxSendSpdInfoFrame - Fill SPD InfoFrame Packet
 *  @param[in] pPacket pointer to a packet buffer
 *  @param[in] pSPD pointer to Source Product Information
 *  @param[in] TxMode Packet transmission frequency
 *  @return error code
 */
UINT32 AmbaHDMI_TxSendSpdInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_SOURCE_PRODUCT_INFO_s *pSPD, UINT32 TxMode)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    (void) pPacket;
    (void) pSPD;
    (void) TxMode;
    return RetVal;
}

/**
 *  AmbaHDMI_TxSendVendorInfoFrame - Fill Vendor-Specific InfoFrame Packet
 *  @param[in] pPacket pointer to a packet buffer
 *  @param[in] pVideoConfig pointer to video format configuration
 *  @param[in] TxMode Packet transmission frequency
 *  @return error code
 */
UINT32 AmbaHDMI_TxSendVendorInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, UINT32 TxMode)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    (void) pPacket;
    (void) pVideoConfig;
    (void) TxMode;
    return RetVal;
}


/**
 *  AmbaHDMI_ConfigDSP - Configure DSP
 *  @param[in] pVideoConfig  pointer to video format configuration
 *  @return error code
 */
UINT32 AmbaHDMI_ConfigDSP(const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    (void) pVideoConfig;
    return RetVal;
}

