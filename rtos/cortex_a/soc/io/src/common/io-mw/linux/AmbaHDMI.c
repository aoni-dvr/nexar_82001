// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
 *  @file AmbaHDMI.c
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
 *
 *  @details HDMI APIs
 *
 */

#include <stdio.h>
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"

#include "hw/ambarella_hdmi.h"
#include "AmbaHDMI_EDID.h"
#include "AmbaI2C.h"
#include "AmbaI2S_Def.h"
#include "AmbaVOUT.h"
#include "AmbaReg_VOUT.h"
#include "AmbaRTSL_HDMI.h"
#include "AmbaRTSL_VOUT.h"
#include "AmbaRTSL_PLL.h"


static AMBA_KAL_MUTEX_t AmbaHdmiMutex;  /* Mutex */
UINT8 DispConfigData[AMBA_HDMI_DATA_BUF_SIZE];

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static UINT32 IsSuccessorVal = 0U;  /* Due to HDMI config flow, we need to store the result for double check */
#endif

/**
 *  IsSuccessor - Check recovery flow flag
 *  @param[in] None
 *  @return Recovery flow flag
 */
static UINT32 IsSuccessor(void)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 RetVal;
    ULONG BufAddr;
    const AMBA_VOUT_DISPLAY_C_CONFIG_REG_s *pVoutDisplayBufAddr;

    (void)AmbaRTSL_Vout2GetDispConfigAddr(&BufAddr);
    AmbaMisra_TypeCast(&pVoutDisplayBufAddr, &BufAddr);

    if (IsSuccessorVal == 0U) {
        if ((pAmbaVoutDisplay2_Reg->DispCtrl.HdmiOutput == 1U) &&
            (pVoutDisplayBufAddr->DispCtrl.HdmiOutput == 0U)) {
            IsSuccessorVal = 1U;
            RetVal = 1U;
        } else {
            RetVal = 0U;
        }
    } else {
        RetVal = 1U;
    }

    return RetVal;
#else
    /* Not support */
    return 0U;
#endif
}

/**
 *  HDMI_GetVideoQuantRange - Get video quantization range
 *  @param[in] pVideoConfig pointer to video format configuration
 *  @return Video quantization range
 */
static UINT32 HDMI_GetVideoQuantRange(const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig)
{
    UINT32 QuantRange = HDMI_QRANGE_DEFAULT;
    UINT8 VideoIdCode;

    VideoIdCode = HDMI_GET_RAW_VIC(pVideoConfig->VideoIdCode);

    if ((pVideoConfig->QuantRange == HDMI_QRANGE_LIMITED) || (pVideoConfig->QuantRange == HDMI_QRANGE_FULL)) {
        QuantRange = pVideoConfig->QuantRange;
    } else {
        /* By default, limited range shall be used for all video formats defined in CEA-861-D,
           with the exception of VGA (640x480) format, which requires full range */
        if ((VideoIdCode == HDMI_VIC_CUSTOM) ||
            (VideoIdCode == HDMI_VIC_DMT0659)) {
            QuantRange = HDMI_QRANGE_FULL;
        } else {
            QuantRange = HDMI_QRANGE_LIMITED;
        }
    }

    return QuantRange;
}

/**
 *  HDMI_ConfigVout - Configure Hdmi Vout
 *  @param[in] pVideoConfig  pointer to video format configuration
 *  @return error code
 */
static UINT32 HDMI_ConfigVout(const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig)
{
    const AMBA_HDMI_VIDEO_TIMING_s *pDtd;
    AMBA_VOUT_CSC_MATRIX_s *pCscMatrix;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DispTiming;
    UINT32 HdmiMode, FrameSyncPolarity, LineSyncPolarity, System50Hz;
    UINT16 FrameWidth, FrameHeight;
    UINT16 MaxPixelValue, CscSelect;
    UINT32 RetVal = HDMI_ERR_NONE;
    FLOAT PixelClkFreq;
    UINT8 StrictFrameRate;

    if ((pVideoConfig->VideoIdCode & HDMI_FRAME_RATE_STRICT) == 0U) {
        StrictFrameRate = 0U;
    } else {
        StrictFrameRate = 1U;
    }

    if (pVideoConfig->PixelFormat == HDMI_PIXEL_FORMAT_RGB_444) {
        /* YCC444 to RGB */
        HdmiMode = VOUT_HDMI_MODE_RGB888_24BIT;
        if (HDMI_GetVideoQuantRange(pVideoConfig) == HDMI_QRANGE_LIMITED) {
            CscSelect = CSC_YCC_FULL_2_RGB_LIMIT;
        } else {
            CscSelect = CSC_YCC_FULL_2_RGB_FULL;
        }

        /* Default colorimetry. SD: SMPTE 170M/ITR-R BT.601, HD: ITU-R BT.709 */
        if (pVideoConfig->CustomDTD.ActiveLines >= 720U) {
            (void)AmbaVout_GetCscMatrix(CSC_TYPE_BT709, (UINT32)CscSelect, &pCscMatrix);
        } else {
            (void)AmbaVout_GetCscMatrix(CSC_TYPE_BT601, (UINT32)CscSelect, &pCscMatrix);
        }
    } else {
        if (pVideoConfig->PixelFormat == HDMI_PIXEL_FORMAT_YCC_444) {
            /* YCC444 to YCC444 (8-bit) */
            HdmiMode = VOUT_HDMI_MODE_YCC444_24BIT;
            MaxPixelValue = (UINT16)(((UINT16)1U << 8U) - 1U);
        } else {
            /* YCC444 to YCC444/YCC422 (12-bit) */
            HdmiMode = VOUT_HDMI_MODE_YCC422_24BIT;
            MaxPixelValue = (UINT16)(((UINT16)1U << 12U) - 1U);
        }
        (void)AmbaVout_GetCscMatrix(CSC_TYPE_IDENTITY, 0U, &pCscMatrix);
        pCscMatrix->MaxVal[0] = MaxPixelValue;
        pCscMatrix->MaxVal[1] = MaxPixelValue;
        pCscMatrix->MaxVal[2] = MaxPixelValue;
    }

    if (pVideoConfig->CustomDTD.SyncPolarity == AMBA_VIDEO_SYNC_H_NEG_V_NEG) {
        LineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        FrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
    } else if (pVideoConfig->CustomDTD.SyncPolarity == AMBA_VIDEO_SYNC_H_NEG_V_POS) {
        LineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        FrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
    } else if (pVideoConfig->CustomDTD.SyncPolarity == AMBA_VIDEO_SYNC_H_POS_V_NEG) {
        LineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        FrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
    } else {
        LineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        FrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
    }

    pDtd = &pVideoConfig->CustomDTD;
    DispTiming.PixelClkFreq     = pDtd->PixelClock * 1000U;
    DispTiming.DisplayMethod    = pDtd->ScanFormat;
    DispTiming.HsyncFrontPorch  = pDtd->HsyncFrontPorch;
    DispTiming.HsyncPulseWidth  = pDtd->HsyncPulseWidth;
    DispTiming.HsyncBackPorch   = pDtd->HsyncBackPorch;
    DispTiming.VsyncFrontPorch  = pDtd->VsyncFrontPorch;
    DispTiming.VsyncPulseWidth  = pDtd->VsyncPulseWidth;
    DispTiming.VsyncBackPorch   = pDtd->VsyncBackPorch;
    DispTiming.ActivePixels     = pDtd->ActivePixels;
    DispTiming.ActiveLines      = pDtd->ActiveLines;

    FrameWidth = pDtd->HsyncFrontPorch + pDtd->HsyncPulseWidth + pDtd->HsyncBackPorch + pDtd->ActivePixels;
    FrameHeight = pDtd->VsyncFrontPorch + pDtd->VsyncPulseWidth + pDtd->VsyncBackPorch + pDtd->ActiveLines;

    if (pDtd->ScanFormat == AMBA_VIDEO_SCAN_INTERLACED) {
        FrameHeight += pDtd->VsyncFrontPorch + pDtd->VsyncPulseWidth + pDtd->VsyncBackPorch + 1U;
    }

    System50Hz = DispTiming.PixelClkFreq % (6U * (UINT32)FrameWidth * (UINT32)FrameHeight);
    if ((System50Hz == 0U) && (DispTiming.ActivePixels != 4096U) && (StrictFrameRate == 0U)) {
        PixelClkFreq = (FLOAT)DispTiming.PixelClkFreq / 1.001f;
        DispTiming.PixelClkFreq = (UINT32)PixelClkFreq;
    }

    /* Config Vout PLL */
    if (IsSuccessor() == 0U) {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        (void)AmbaRTSL_PllSetVoutCClkHint(AMBA_PLL_VOUT_TV_HDMI, 10U);
        (void)AmbaRTSL_PllSetVoutCClk(DispTiming.PixelClkFreq);
#else
        (void)AmbaRTSL_PllSetVoutTvClkHint(AMBA_PLL_VOUT_TV_HDMI, 10U);
        (void)AmbaRTSL_PllSetVoutTvClk(DispTiming.PixelClkFreq);
#endif
    }

    (void)AmbaVout_HdmiSetCsc(pCscMatrix, HdmiMode);
    (void)AmbaVout_HdmiEnable(HdmiMode, FrameSyncPolarity, LineSyncPolarity, &DispTiming);

    return RetVal;
}

/**
 *  AmbaHDMI_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaHDMI_DrvEntry(void)
{
    UINT32 RetVal = HDMI_ERR_NONE;

    if (system("modprobe ambahdmi_sdk") < 0) { }

    /* Create Mutex */
    if (AmbaKAL_MutexCreate(&AmbaHdmiMutex, NULL) != OK) {
        RetVal = HDMI_ERR_MUTEX;    /* should never happen */
    }

    //AmbaRTSL_HdmiInit();

    return RetVal;
}

/**
 *  AmbaHDMI_TxEnable - Turn-On HDMI transmitter
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
UINT32 AmbaHDMI_TxEnable(UINT32 HdmiPort)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_config_msg_t HdmiConfigMsg;

    if (IsSuccessor() == 0U){
        if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
            fd = open("/dev/hdmi", O_RDWR);

            if (fd == -1) {
                RetVal = HDMI_ERR_OPEN_FILE;
            } else {
                HdmiConfigMsg.HdmiPort = HdmiPort;

                if (ioctl(fd, HDMI_IOC_TX_ENABLE, &HdmiConfigMsg, sizeof(HdmiConfigMsg), NULL) != 0) {
                    RetVal = HDMI_ERR_DEV_CTL;
                }
                close(fd);
            }
            (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
        } else {
            RetVal = HDMI_ERR_MUTEX;
        }
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxDisable - Turn-Off HDMI transmitter
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
UINT32 AmbaHDMI_TxDisable(UINT32 HdmiPort)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_config_msg_t HdmiConfigMsg;

    if (IsSuccessor() == 0U) {
        if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
            fd = open("/dev/hdmi", O_RDWR);

            if (fd == -1) {
                RetVal = HDMI_ERR_OPEN_FILE;
            } else {
                HdmiConfigMsg.HdmiPort = HdmiPort;

                if (ioctl(fd, HDMI_IOC_TX_DISABLE, &HdmiConfigMsg, sizeof(HdmiConfigMsg), NULL) != 0) {
                    RetVal = HDMI_ERR_DEV_CTL;
                }
                close(fd);
            }
            (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
        } else {
            RetVal = HDMI_ERR_MUTEX;
        }
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_HookHpdHandler - Hook HPD event handler
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] IntHandler Interrupt handler
 *  @return error code
 */
UINT32 AmbaHDMI_HookHpdHandler(UINT32 HdmiPort, AMBA_HDMI_HPD_ISR_f IntHandler)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_hpd_isr_msg_t HdmiHpdIsrMsg;

    if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
        fd = open("/dev/hdmi", O_RDWR);

        if (fd == -1) {
            RetVal = HDMI_ERR_OPEN_FILE;
        } else {
            HdmiHpdIsrMsg.HdmiPort = HdmiPort;
            HdmiHpdIsrMsg.IntHandler = IntHandler;

            if (ioctl(fd, HDMI_IOC_TX_SET_MODE, &HdmiHpdIsrMsg, sizeof(HdmiHpdIsrMsg), NULL) != 0) {
                RetVal = HDMI_ERR_DEV_CTL;
            }
            close(fd);
        }
        (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
    } else {
        RetVal = HDMI_ERR_MUTEX;
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxGetSinkInfo - Get supported Video Identification Code (VIC) and Audio Format Code
 *  @param[in] HdmiPort HDMI port ID
 *  @param[out] pHotPlugDetect Hot-Plug Detection Status
 *  @param[out] pEdidDetect EDID Detection Status
 *  @param[out] pSinkInfo pointer to external buffer of sink information
 *  @return error code
 */
UINT32 AmbaHDMI_TxGetSinkInfo(UINT32 HdmiPort, UINT32 *pHotPlugDetect, UINT32 *pEdidDetect, AMBA_HDMI_SINK_INFO_s *pSinkInfo)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    //hdmi_sinkinfo_msg_t HdmiSinkinfoMsg;

    if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
        fd = open("/dev/hdmi", O_RDWR);

        if (fd == -1) {
            RetVal = HDMI_ERR_OPEN_FILE;
        } else {
            (void) HdmiPort;
            /* Update sink info */
            *pHotPlugDetect = 1;
            *pEdidDetect = 1;
            //memcpy(pSinkInfo, &HdmiSinkinfoMsg.SinkInfo, sizeof(AMBA_HDMI_SINK_INFO_s));
            (void) pSinkInfo;

            close(fd);
        }
        (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
    } else {
        RetVal = HDMI_ERR_MUTEX;
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxSetMode - Configure HDMI/DVI outputs
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] pVideoConfig pointer to video format configuration
 *  @param[in] pAudioConfig pointer to audio format configuration
 *  @return error code
 */
UINT32 AmbaHDMI_TxSetMode(UINT32 HdmiPort, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_config_msg_t HdmiConfigMsg;

    if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
        fd = open("/dev/hdmi", O_RDWR);

        if (fd == -1) {
            RetVal = HDMI_ERR_OPEN_FILE;
        } else {
            if (IsSuccessor() == 0U) {
                /* Set default clock */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                (void)AmbaRTSL_PllSetVoutCClkHint(AMBA_PLL_VOUT_TV_HDMI, 10U);
                (void)AmbaRTSL_PllSetVoutCClk(27000000U);
#else
                (void)AmbaRTSL_PllSetVoutTvClkHint(AMBA_PLL_VOUT_TV_HDMI, 10U);
                (void)AmbaRTSL_PllSetVoutTvClk(27000000U);
#endif
            }

            HdmiConfigMsg.HdmiPort = HdmiPort;
            if (pVideoConfig != NULL) {
                memcpy(&HdmiConfigMsg.VideoConfig, pVideoConfig, sizeof(AMBA_HDMI_VIDEO_CONFIG_s));
            }

            if (pAudioConfig != NULL) {
                memcpy(&HdmiConfigMsg.AudioConfig, pAudioConfig, sizeof(AMBA_HDMI_AUDIO_CONFIG_s));
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                HdmiConfigMsg.AudioCfgIsNull = 0U;
            } else {
                HdmiConfigMsg.AudioCfgIsNull = 1U;
#endif
            }

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            if (IsSuccessor() == 0U) {
                HdmiConfigMsg.IsSuccessor = 0U;
            } else {
                HdmiConfigMsg.IsSuccessor = 1U;
            }
#endif

            if (ioctl(fd, HDMI_IOC_TX_SET_MODE, &HdmiConfigMsg, sizeof(HdmiConfigMsg), NULL) != 0) {
                RetVal = HDMI_ERR_DEV_CTL;
            } else {
                /* Get working video config and set vout */
                HDMI_ConfigVout(&HdmiConfigMsg.WorkingVideoConfig);
            }
            close(fd);
        }
        (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
    } else {
        RetVal = HDMI_ERR_MUTEX;
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxGetCurrModeInfo - Configure HDMI/DVI outputs
 *  @param[in] HdmiPort HDMI port ID
 *  @param[out] pModeInfo pointer to current mode information
 *  @return error code
 */
UINT32 AmbaHDMI_TxGetCurrModeInfo(UINT32 HdmiPort, AMBA_HDMI_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_modeinfo_msg_t HdmiModeinfoMsg;
    UINT16 FrameWidth, FrameHeight;
    UINT8 StrictFrameRate;

    if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
        fd = open("/dev/hdmi", O_RDWR);

        if (fd == -1) {
            RetVal = HDMI_ERR_OPEN_FILE;
        } else {
            HdmiModeinfoMsg.HdmiPort = HdmiPort;

            if (ioctl(fd, HDMI_IOC_TX_GET_CURR_MODE, &HdmiModeinfoMsg, sizeof(HdmiModeinfoMsg), NULL) != 0) {
                RetVal = HDMI_ERR_DEV_CTL;
            } else {
                /* Get current mode info */
                memcpy(pModeInfo, &HdmiModeinfoMsg.ModeInfo, sizeof(AMBA_HDMI_MODE_INFO_s));
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                pModeInfo->VideoConfig.PixelClkFreq = AmbaRTSL_PllGetVoutCClk();
#else
                pModeInfo->VideoConfig.PixelClkFreq = AmbaRTSL_PllGetVoutTvClk();
#endif

                StrictFrameRate = HdmiModeinfoMsg.StrictFrameRate;
                FrameWidth = HdmiModeinfoMsg.FrameWidth;
                FrameHeight = HdmiModeinfoMsg.FrameHeight;

                if (StrictFrameRate == 0U) {
                    pModeInfo->VideoFrameRate = ((FLOAT)pModeInfo->VideoConfig.PixelClkFreq * 1.001f) / ((FLOAT)FrameWidth * (FLOAT)FrameHeight);
                } else {
                    pModeInfo->VideoFrameRate = ((FLOAT)pModeInfo->VideoConfig.PixelClkFreq) / ((FLOAT)FrameWidth * (FLOAT)FrameHeight);
                }

                if (pModeInfo->VideoConfig.DisplayMethod == AMBA_VIDEO_SCAN_INTERLACED) {
                    /* Provide field rate when interlaced mode */
                    pModeInfo->VideoFrameRate *= 2.0f;
                }

            }
            close(fd);
        }
        (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
    } else {
        RetVal = HDMI_ERR_MUTEX;
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxSetPhyCtrl - Set HDMI PHY control
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] ParamID PHY parameter type
 *  @param[in] ParamVal PHY parameter value
 *  @return error code
 */
UINT32 AmbaHDMI_TxSetPhyCtrl(UINT32 HdmiPort, UINT32 ParamID, UINT32 ParamVal)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_phy_msg_t HdmiPhyMsg;

    if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
        fd = open("/dev/hdmi", O_RDWR);

        if (fd == -1) {
            RetVal = HDMI_ERR_OPEN_FILE;
        } else {
            HdmiPhyMsg.HdmiPort = HdmiPort;
            HdmiPhyMsg.ParamID = ParamID;
            HdmiPhyMsg.ParamVal = ParamVal;

            if (ioctl(fd, HDMI_IOC_TX_SET_PHY, &HdmiPhyMsg, sizeof(HdmiPhyMsg), NULL) != 0) {
                RetVal = HDMI_ERR_DEV_CTL;
            }
            close(fd);
        }
        (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
    } else {
        RetVal = HDMI_ERR_MUTEX;
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxStart - Start HDMI/DVI video outputs
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
UINT32 AmbaHDMI_TxStart(UINT32 HdmiPort)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_config_msg_t HdmiConfigMsg;

    if (IsSuccessor() == 0U) {
        if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
            fd = open("/dev/hdmi", O_RDWR);

            if (fd == -1) {
                RetVal = HDMI_ERR_OPEN_FILE;
            } else {
                HdmiConfigMsg.HdmiPort = HdmiPort;

                if (ioctl(fd, HDMI_IOC_TX_START, &HdmiConfigMsg, sizeof(HdmiConfigMsg), NULL) != 0) {
                    RetVal = HDMI_ERR_DEV_CTL;
                }
                close(fd);
            }
            (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
        } else {
            RetVal = HDMI_ERR_MUTEX;
        }
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxStop - Stop HDMI/DVI video outputs
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
UINT32 AmbaHDMI_TxStop(UINT32 HdmiPort)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_config_msg_t HdmiConfigMsg;

    if (IsSuccessor() == 0U) {
        if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
            fd = open("/dev/hdmi", O_RDWR);

            if (fd == -1) {
                RetVal = HDMI_ERR_OPEN_FILE;
            } else {
                HdmiConfigMsg.HdmiPort = HdmiPort;

                if (ioctl(fd, HDMI_IOC_TX_STOP, &HdmiConfigMsg, sizeof(HdmiConfigMsg), NULL) != 0) {
                    RetVal = HDMI_ERR_DEV_CTL;
                }
                close(fd);
            }
            (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
        } else {
            RetVal = HDMI_ERR_MUTEX;
        }
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxSetSourceInfo - Set source product information
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] pSrcProductInfo Source Product Information
 *  @return error code
 */
UINT32 AmbaHDMI_TxSetSourceInfo(UINT32 HdmiPort, const AMBA_HDMI_SOURCE_PRODUCT_INFO_s *pSrcProductInfo)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_sourceinfo_msg_t HdmiSourceInfoMsg;

    if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
        fd = open("/dev/hdmi", O_RDWR);

        if (fd == -1) {
            RetVal = HDMI_ERR_OPEN_FILE;
        } else {
            HdmiSourceInfoMsg.HdmiPort = HdmiPort;
            memcpy(&HdmiSourceInfoMsg.SrcProductInfo, pSrcProductInfo, sizeof(AMBA_HDMI_SOURCE_PRODUCT_INFO_s));

            if (ioctl(fd, HDMI_IOC_TX_SET_SOURCEINFO, &HdmiSourceInfoMsg, sizeof(HdmiSourceInfoMsg), NULL) != 0) {
                RetVal = HDMI_ERR_DEV_CTL;
            }
            close(fd);
        }
        (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
    } else {
        RetVal = HDMI_ERR_MUTEX;
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxSetInfoFramePacket - Confiugre the transmission of the specified HDMI InfoFrame packet
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] pInfoFramePkt pointer to InfoFrame packet data
 *  @param[in] TxMode InfoFrame packet transmission mode
 *  @return error code
 */
UINT32 AmbaHDMI_TxSetInfoFramePacket(UINT32 HdmiPort, const AMBA_HDMI_DATA_ISLAND_PACKET_s *pInfoFramePkt, UINT32 TxMode)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_infoframe_msg_t HdmiInfoframeMsg;

    if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
        fd = open("/dev/hdmi", O_RDWR);

        if (fd == -1) {
            RetVal = HDMI_ERR_OPEN_FILE;
        } else {
            HdmiInfoframeMsg.HdmiPort = HdmiPort;
            HdmiInfoframeMsg.TxMode = TxMode;
            memcpy(&HdmiInfoframeMsg.InfoFramePkt, pInfoFramePkt, sizeof(AMBA_HDMI_DATA_ISLAND_PACKET_s));

            if (ioctl(fd, HDMI_IOC_TX_SET_INFOFRAME, &HdmiInfoframeMsg, sizeof(HdmiInfoframeMsg), NULL) != 0) {
                RetVal = HDMI_ERR_DEV_CTL;
            }
            close(fd);
        }
        (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
    } else {
        RetVal = HDMI_ERR_MUTEX;
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxGetInfoFramePacket - Get HDMI current infoframe packet according to infoframe type, the packet data is valid when the infoframe is enabled
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] InfoFrameType InfoFrame packet type
 *  @param[in] pInfoFramePkt pointer to an InfoFrame packet data
 *  @return error code
 */
UINT32 AmbaHDMI_TxGetInfoFramePacket(UINT32 HdmiPort, UINT32 InfoFrameType, AMBA_HDMI_DATA_ISLAND_PACKET_s *pInfoFramePkt)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_infoframe_msg_t HdmiInfoframeMsg;

    if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
        fd = open("/dev/hdmi", O_RDWR);

        if (fd == -1) {
            RetVal = HDMI_ERR_OPEN_FILE;
        } else {
            HdmiInfoframeMsg.HdmiPort = HdmiPort;
            HdmiInfoframeMsg.InfoFrameType = InfoFrameType;

            if (ioctl(fd, HDMI_IOC_TX_GET_INFOFRAME, &HdmiInfoframeMsg, sizeof(HdmiInfoframeMsg), NULL) != 0) {
                RetVal = HDMI_ERR_DEV_CTL;
            } else {
                /* Get infoframe */
                memcpy(pInfoFramePkt, &HdmiInfoframeMsg.InfoFramePkt, sizeof(AMBA_HDMI_DATA_ISLAND_PACKET_s));
            }
            close(fd);
        }
        (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
    } else {
        RetVal = HDMI_ERR_MUTEX;
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxSetAvMute - Enable the HDMI Audio/Video Mute (AVMUTE) feature
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
UINT32 AmbaHDMI_TxSetAvMute(UINT32 HdmiPort)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_config_msg_t HdmiConfigMsg;

    if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
        fd = open("/dev/hdmi", O_RDWR);

        if (fd == -1) {
            RetVal = HDMI_ERR_OPEN_FILE;
        } else {
            HdmiConfigMsg.HdmiPort = HdmiPort;

            if (ioctl(fd, HDMI_IOC_TX_SET_AVMUTE, &HdmiConfigMsg, sizeof(HdmiConfigMsg), NULL) != 0) {
                RetVal = HDMI_ERR_DEV_CTL;
            }
            close(fd);
        }
        (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
    } else {
        RetVal = HDMI_ERR_MUTEX;
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxClearAvMute - Disable the HDMI Audio/Video Mute (AVMUTE) feature
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
UINT32 AmbaHDMI_TxClearAvMute(UINT32 HdmiPort)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_config_msg_t HdmiConfigMsg;

    if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
        fd = open("/dev/hdmi", O_RDWR);

        if (fd == -1) {
            RetVal = HDMI_ERR_OPEN_FILE;
        } else {
            HdmiConfigMsg.HdmiPort = HdmiPort;

            if (ioctl(fd, HDMI_IOC_TX_CLEAR_AVMUTE, &HdmiConfigMsg, sizeof(HdmiConfigMsg), NULL) != 0) {
                RetVal = HDMI_ERR_DEV_CTL;
            }
            close(fd);
        }
        (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
    } else {
        RetVal = HDMI_ERR_MUTEX;
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

UINT32 AmbaHDMI_TxGetDispCfgAddr(ULONG *pAddr, UINT32 *pSize)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_disp_config_msg_t HdmiDispConfigMsg;

    if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
        fd = open("/dev/hdmi", O_RDWR);

        if (fd == -1) {
            RetVal = HDMI_ERR_OPEN_FILE;
        } else {
            HdmiDispConfigMsg.HdmiPort = 0;

            if (ioctl(fd, HDMI_IOC_TX_GET_DISP_CONFIG, &HdmiDispConfigMsg, sizeof(HdmiDispConfigMsg), NULL) != 0) {
                RetVal = HDMI_ERR_DEV_CTL;
            }

            memcpy(DispConfigData, HdmiDispConfigMsg.Out, AMBA_HDMI_DATA_BUF_SIZE);

            *pAddr = (uintptr_t)DispConfigData;
            *pSize = AMBA_HDMI_DATA_BUF_SIZE;

            close(fd);
        }
        (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
    } else {
        RetVal = HDMI_ERR_MUTEX;
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

UINT32 AmbaHDMI_TxEnableTestPattern(UINT32 HdmiPort)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    int fd;
    hdmi_disp_config_msg_t HdmiDispConfigMsg;

    if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
        fd = open("/dev/hdmi", O_RDWR);

        if (fd == -1) {
            RetVal = HDMI_ERR_OPEN_FILE;
        } else {
            HdmiDispConfigMsg.HdmiPort = HdmiPort;

            if (ioctl(fd, HDMI_IOC_TX_ENABLE_TESTPATTERN, &HdmiDispConfigMsg, sizeof(HdmiDispConfigMsg), NULL) != 0) {
                RetVal = HDMI_ERR_DEV_CTL;
            }
            close(fd);
        }
        (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
    } else {
        RetVal = HDMI_ERR_MUTEX;
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_AudioChannelSelect - Selects target HDMI audio channel to be programmed
 *  @param[in] HDMIAudioChannel Target vout channel
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaHDMI_AudioChannelSelect(UINT32 HDMIAudioChannel)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 RetVal = HDMI_ERR_NONE;
    UINT32 I2sChanNo = HDMIAudioChannel;
    UINT32 AudioClkFreq;
    int fd;
    hdmi_audio_msg_t HdmiAudioMsg;

    if (AmbaKAL_MutexTake(&AmbaHdmiMutex, 100) == OK) {
        fd = open("/dev/hdmi", O_RDWR);

        if (fd == -1) {
            RetVal = HDMI_ERR_OPEN_FILE;
        } else {
            if (I2sChanNo == AMBA_I2S_CHANNEL0) {
                AudioClkFreq = AmbaRTSL_PllGetAudio3Clk();
            } else if (I2sChanNo == AMBA_I2S_CHANNEL1) {
                AudioClkFreq = AmbaRTSL_PllGetAudio2Clk();
            } else {
                AudioClkFreq = 0U;
            }

            HdmiAudioMsg.AudioSrc = I2sChanNo;
            HdmiAudioMsg.AudioClk = AudioClkFreq;

            if (ioctl(fd, HDMI_IOC_TX_SET_AUDIO_SRC, &HdmiAudioMsg, sizeof(HdmiAudioMsg), NULL) != 0) {
                RetVal = HDMI_ERR_DEV_CTL;
            }
            close(fd);
        }
        (void)AmbaKAL_MutexGive(&AmbaHdmiMutex);
    } else {
        RetVal = HDMI_ERR_MUTEX;
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
#else
    /* Not supported API */
    (void)HDMIAudioChannel;
    return HDMI_ERR_NA;
#endif
}

