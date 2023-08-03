/*
 * Copyright 2020, Ambarella International LP
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

#ifndef AMBA_HDMI_CTRL_H
#define AMBA_HDMI_CTRL_H

#ifndef AMBA_HDMI_DEF_H
#include "AmbaHDMI_Def.h"
#endif
#include "AmbaHDMI_EDID.h"

/*
 * HDMI 2D/3D Video Format Support
 */
#define HDMI_VID_2D                        (1U << 0U)  /* 2D */
#define HDMI_VID_3D_FRAME_PACKING          (1U << 1U)  /* 3D Frame packing */
#define HDMI_VID_3D_FIELD_ALTERNATIVE      (1U << 2U)  /* 3D Field alternative */
#define HDMI_VID_3D_LINE_ALTERNATIVE       (1U << 3U)  /* 3D Line alternative */
#define HDMI_VID_3D_SBS_FULL               (1U << 4U)  /* 3D Left-and-Right with full resolution */
#define HDMI_VID_3D_L_DEPTH                (1U << 5U)  /* 3D L + depth */
#define HDMI_VID_3D_L_DEPTH_GFX_DEPTH      (1U << 6U)  /* 3D L + depth + Graphics + Graphics-depth */
#define HDMI_VID_3D_TOP_AND_BOTTOM         (1U << 7U)  /* 3D Top-and-Bottom with half vertical resolution */
#define HDMI_VID_3D_HALF_HORIZONTAL        (1U << 8U)  /* 3D Horizontal sub-sampling */
#define HDMI_VID_3D_HALF_QUIN_LO_RODD      (1U << 12U) /* 3D Quincunx: Odd/Left pictureU Odd/Right picture */
#define HDMI_VID_3D_HALF_QUIN_LO_REVEN     (1U << 13U) /* 3D Quincunx: Odd/Left pictureU Even/Right picture */
#define HDMI_VID_3D_HALF_QUIN_LE_RODD      (1U << 14U) /* 3D Quincunx: Even/Left pictureU Odd/Right picture */
#define HDMI_VID_3D_HALF_QUIN_LE_REVEN     (1U << 15U) /* 3D Quincunx: Even/Left pictureU Even/Right picture */

#define HDMI_VID_3D_SBS_HALF_QUINCUNX          \
    (HDMI_VID_3D_HALF_QUIN_LO_RODD         |   \
     HDMI_VID_3D_HALF_QUIN_LO_REVEN        |   \
     HDMI_VID_3D_HALF_QUIN_LE_RODD         |   \
     HDMI_VID_3D_HALF_QUIN_LE_REVEN)

#define HDMI_VID_3D_SBS_HALF                   \
    (HDMI_VID_3D_HALF_HORIZONTAL           |   \
     HDMI_VID_3D_SBS_HALF_QUINCUNX)

#define HDMI_VID_3D                            \
    (HDMI_VID_3D_FRAME_PACKING             |   \
     HDMI_VID_3D_FIELD_ALTERNATIVE         |   \
     HDMI_VID_3D_LINE_ALTERNATIVE          |   \
     HDMI_VID_3D_SBS_FULL                  |   \
     HDMI_VID_3D_L_DEPTH                   |   \
     HDMI_VID_3D_L_DEPTH_GFX_DEPTH         |   \
     HDMI_VID_3D_TOP_AND_BOTTOM            |   \
     HDMI_VID_3D_SBS_HALF)

typedef struct {
    UINT32  FormatID:                   15;     /* [14:0] AMBA_CEA861_VIDEO_ID_CODE_e */
    UINT32  Native:                     1;      /* [15] 1 = native video format */
    UINT32  Valid2D:                    1;      /* [16] 1 = the video format is valid in 2D frame structure */
    UINT32  Valid3dFramePacking:        1;      /* [17] 1 = the video format is valid in 3D frame packing structure */
    UINT32  Valid3dFieldALT:            1;      /* [18] 1 = the video format is valid in 3D field alternative frame structure */
    UINT32  Valid3dLineALT:             1;      /* [19] 1 = the video format is valid in 3D line alternative frame structure */
    UINT32  Valid3dSideBySideFull:      1;      /* [20] 1 = the video format is valid in 3D left-and-right with full resolution frame structure */
    UINT32  Valid3dLdepth:              1;      /* [21] 1 = the video format is valid in 3D L + depth frame structure */
    UINT32  Valid3dLdepthGfxDepth:      1;      /* [22] 1 = the video format is valid in 3D L + depth + graphics + graphics-depth frame structure */
    UINT32  Valid3dTopAndBottom:        1;      /* [23] 1 = the video format is valid in 3D top-and-bottom with half vertical resolution frame structure */
    UINT32  Valid3dSideBySideHalfHori:  1;      /* [24] 1 = the video format is valid in 3D horizontal sub-sampling frame structure */
    UINT32  Reserved:                   3;      /* [27:25] Reserved */
    UINT32  Valid3dSideBySideHalfLoRo:  1;      /* [28] 1 = the video format is valid in 3D quincunx odd/left picture, odd/right frame structure */
    UINT32  Valid3dSideBySideHalfLoRe:  1;      /* [29] 1 = the video format is valid in 3D quincunx odd/left picture, even/right frame structure */
    UINT32  Valid3dSideBySideHalfLeRo:  1;      /* [30] 1 = the video format is valid in 3D quincunx even/left picture, odd/right frame structure */
    UINT32  Valid3dSideBySideHalfLeRe:  1;      /* [31] 1 = the video format is valid in 3D quincunx even/left picture, even/right frame structure */
} AMBA_HDMI_VIDEO_DESCRIPTOR_s;

/*
 * HDMI Audio Sampling Rate Support
 */
#define HDMI_AUD_FS_32K             0x01U       /* Sample rate =  32000 */
#define HDMI_AUD_FS_44P1K           0x02U       /* Sample rate =  44100 */
#define HDMI_AUD_FS_48K             0x04U       /* Sample rate =  48000 */
#define HDMI_AUD_FS_88P2K           0x08U       /* Sample rate =  88200 */
#define HDMI_AUD_FS_96K             0x10U       /* Sample rate =  96000 */
#define HDMI_AUD_FS_176P4K          0x20U       /* Sample rate = 176400 */
#define HDMI_AUD_FS_192K            0x40U       /* Sample rate = 192000 */

#define HDMI_AUD_LPCM_16BIT         0x01U       /* 16-bit L-PCM */
#define HDMI_AUD_LPCM_20BIT         0x02U       /* 20-bit L-PCM */
#define HDMI_AUD_LPCM_24BIT         0x04U       /* 24-bit L-PCM */

typedef struct {
    UINT8  MaxNumChan;                          /* Max Number of channels */
    UINT8  SampleRate;                          /* Sample rate support */
    UINT8  Detail;                              /* Detailed information associated to each Audio Format Code */
    UINT8  Reserved;                            /* Reserved */
} AMBA_HDMI_AUDIO_DESCRIPTOR_s;

/*
 * Defined in AmbaHDMI_Info.c
 */
UINT32 AmbaHDMI_TxCopySinkInfo(AMBA_HDMI_SINK_INFO_s *pSinkInfo);
UINT32 AmbaHDMI_TxParseEDID(const UINT8 pEDID[EDID_NUM_BLOCKS][EDID_BLOCK_SIZE], UINT32 *pEdidDetect);
UINT32 AmbaHDMI_TxConfigAudio(const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig);
UINT32 AmbaHDMI_TxSendAudioInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig, UINT32 TxMode);
UINT32 AmbaHDMI_TxSendAviInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, UINT32 TxMode);
UINT32 AmbaHDMI_TxSendSpdInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_SOURCE_PRODUCT_INFO_s *pSPD, UINT32 TxMode);
UINT32 AmbaHDMI_TxSendVendorInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, UINT32 TxMode);
UINT32 AmbaHDMI_ConfigDSP(const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig);
#if defined(CONFIG_LINUX)
UINT32 AmbaHDMI_AudioChannelSelect(UINT32 HDMIAudioChannel);
#else
void AmbaHDMI_AudioChannelSelect(UINT32 HDMIAudioChannel);
#endif

/*
 * Defined in AmbaHDMI_CEC.c
 */
UINT32 AmbaHDMI_CecDrvEntry(void);

#endif /* AMBA_HDMI_CTRL_H */
