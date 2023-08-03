/**
 * @file AmbaHDMI.c
 *
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

#if defined(CONFIG_QNX)
#include "Generic.h"
#include "hw/ambarella_clk.h"
#include "AmbaReg_VOUT.h"
#else
#include "AmbaRTSL_GIC.h"
#endif

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif

#include "AmbaDrvEntry.h"
#include "AmbaI2C.h"
#include "AmbaINT.h"
#include "AmbaHDMI.h"
#include "AmbaHDMI_Ctrl.h"
#include "AmbaRTSL_HDMI.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaCSL_HDMI.h"

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include "AmbaIOUtility.h"
#else
#include "AmbaINT_Priv.h"
#endif

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

static AMBA_HDMI_CTRL_s AmbaHdmiCtrl;
static AMBA_KAL_MUTEX_t AmbaHdmiMutex;
static UINT8 AmbaHdmiSinkEDID[EDID_NUM_BLOCKS][EDID_BLOCK_SIZE] GNU_SECTION_NOZEROINIT;
static AMBA_HDMI_DATA_ISLAND_PACKET_s AmbaHdmiInfoFrameBuf[NUM_HDMI_INFOFRAME_TYPE] GNU_SECTION_NOZEROINIT;
static UINT32 AmbaHdmiInfoFrameTxMode[NUM_HDMI_INFOFRAME_TYPE] = {
    [HDMI_INFOFRAME_RESERVED]           = HDMI_PACKET_SEND_NONE,
    [HDMI_INFOFRAME_VENDOR_SPECIFIC]    = HDMI_PACKET_SEND_EVERY_FRAME,
    [HDMI_INFOFRAME_AVI]                = HDMI_PACKET_SEND_EVERY_FRAME,
    [HDMI_INFOFRAME_SPD]                = HDMI_PACKET_SEND_EVERY_FRAME,
    [HDMI_INFOFRAME_AUDIO]              = HDMI_PACKET_SEND_EVERY_FRAME,
    [HDMI_INFOFRAME_MPEG_SOURCE]        = HDMI_PACKET_SEND_NONE,
};

static UINT32 HDMI_ValdidateEDID(const UINT8 *pEdidBlock);
static UINT32 HDMI_ReadEDID(UINT32 EdidBlockNo, UINT8 *pEdidBlock);
static UINT32 HDMI_DetectSink(UINT32 *pHotPlugDetect, UINT32 *pEdidDetect);
static UINT32 HDMI_WriteSCDC(UINT32 TxDataSize, UINT8 *pTxDataBuf);
static void HDMI_SetClockRatio(UINT32 PixelClock);
static void HDMI_RefreshInfoFrame(UINT32 InfoFrameType);
static UINT32 HDMI_ValidateConfig(UINT8 VideoIdCode, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig);

static AMBA_HDMI_HPD_ISR_f AmbaHdmHpdISR;
static AMBA_HDMI_CEC_ISR_f AmbaHdmCecISR;


#if !defined(CONFIG_LINUX)
/**
 *  HDMI_IntHandler - ISR for HDMI transmitter
 *  @param[in] IntID Interupt ID
 *  @param[in] IsrArg Optional argument of the interrupt handler
 */
#pragma GCC push_options
#pragma GCC target("general-regs-only")
static void HDMI_IntHandler(UINT32 IntID, UINT32 IsrArg)
{
    UINT32 HotPlugDetected;
    UINT32 RxSensed;

    /* CEC */
    if (AmbaCSL_HdmiIsCecTxOk() != 0U) {
        /* successful transmitting CEC frame */
        if (AmbaHdmCecISR != NULL) {
            AmbaHdmCecISR(HDMI_CEC_TX_OK);
        }
    } else if (AmbaCSL_HdmiIsCecTxFail() != 0U) {
        /* failed transmitting CEC frame (after retry fail) */
        if (AmbaHdmCecISR != NULL) {
            AmbaHdmCecISR(HDMI_CEC_TX_NG);
        }
    } else if (AmbaCSL_HdmiIsCecRx() != 0U) {
        /* successful receiving CEC frame (frame is available) */
        if (AmbaHdmCecISR != NULL) {
            AmbaHdmCecISR(HDMI_CEC_RX_OK);
        }
    } else {
        /* Hot-Plug Detection */
        if (AmbaHdmHpdISR != NULL) {
            HotPlugDetected = AmbaCSL_HdmiIsHotPlugDetected();
            RxSensed = AmbaCSL_HdmiIsRxSensed();
            AmbaHdmHpdISR(0U, HotPlugDetected, RxSensed);
        }
    }

    AmbaCSL_HdmiClearAllInt();
    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&IsrArg);
}
#pragma GCC pop_options
#endif

/**
 *  AmbaHDMI_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaHDMI_DrvEntry(void)
{
    static char AmbaHdmiMutexName[16] = "AmbaHdmiMutex";
    UINT32 RetVal = HDMI_ERR_NONE;
    UINT32 IrqId;
    AMBA_INT_CONFIG_s IntConfig = {
        .TriggerType = INT_TRIG_HIGH_LEVEL,
        .IrqType = INT_TYPE_IRQ,
        .CpuTargets = 1U,
    };
#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX))
    ULONG BaseAddr;
#endif

    /* Create Mutex */
    if (AmbaKAL_MutexCreate(&AmbaHdmiMutex, AmbaHdmiMutexName) != KAL_ERR_NONE) {
        RetVal = HDMI_ERR_UNEXPECTED;
    } else {
        /* Get HDMI base address */
#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX))
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
        BaseAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,hdmi", "reg", 0U);
        if (BaseAddr == 0U) {
            RetVal |= HDMI_ERR_UNEXPECTED;
        }
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        else {
            BaseAddr |= AMBA_CORTEX_A76_AHB_PHYS_BASE_ADDR;
        }
#endif
#else
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        BaseAddr = AMBA_CORTEX_A76_HDMI_BASE_ADDR;
#else
        BaseAddr = AMBA_CORTEX_A53_HDMI_BASE_ADDR;
#endif
#endif  //#if defined(CONFIG_DEVICE_TREE_SUPPORT)
        AmbaMisra_TypeCast(&pAmbaHDMI_Reg, &BaseAddr);
#endif  //#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX))

        /* Get interrupt ID */
#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX)) && defined(CONFIG_DEVICE_TREE_SUPPORT)
        IrqId = IO_UtilityFDTPropertyU32Quick(0, "ambarella,hdmi", "interrupts", 1U) + 32U;
        if (IrqId == 0U) {
            RetVal |= HDMI_ERR_UNEXPECTED;
        }
#else
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        IrqId = AMBA_INT_SPI_ID82_HDMITX;
#else
        IrqId = AMBA_INT_SPI_ID117_HDMI_TX;
#endif
#endif  //#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX)) && defined(CONFIG_DEVICE_TREE_SUPPORT)

        /* Register interrupt ID */
#if !defined(CONFIG_LINUX)
        (void)AmbaINT_Disable(IrqId);
        (void)AmbaINT_Config(IrqId, &IntConfig, HDMI_IntHandler, 0U);
        (void)AmbaINT_Enable(IrqId);
#endif
        /* HDMI and CEC Init */
        AmbaRTSL_HdmiInit();
        RetVal |= AmbaHDMI_CecDrvEntry();
    }

    AmbaMisra_TouchUnused(AmbaHdmiSinkEDID[0]);

    return RetVal;
}

/**
 *  AmbaHDMI_TxEnable - Turn-On HDMI transmitter
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
UINT32 AmbaHDMI_TxEnable(UINT32 HdmiPort)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    UINT32 RetVal = HDMI_ERR_NONE;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            /* Set default PHY configuration to make sure all the resolution work */
            AmbaRTSL_HdmiSetPhyCtrl(0U, 2U);
            AmbaRTSL_HdmiSetPhyCtrl(1U, 3U);
            AmbaRTSL_HdmiSetPhyCtrl(2U, 4U);
            AmbaRTSL_HdmiSetPhyCtrl(3U, 0U);
            AmbaRTSL_HdmiSetPhyCtrl(4U, 0U);
            AmbaRTSL_HdmiSetPowerCtrl(1U);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
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
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    UINT32 RetVal = HDMI_ERR_NONE;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            AmbaRTSL_HdmiSetPowerCtrl(0U);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
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
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    UINT32 RetVal = HDMI_ERR_NONE;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            AmbaHdmHpdISR = IntHandler;

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaHDMI_HookCecHandler - Hook CEC event handler
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] IntHandler Interrupt handler
 *  @return error code
 */
UINT32 AmbaHDMI_HookCecHandler(UINT32 HdmiPort, AMBA_HDMI_CEC_ISR_f IntHandler)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    UINT32 RetVal = HDMI_ERR_NONE;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            AmbaHdmCecISR = IntHandler;

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
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
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    UINT32 RetVal = HDMI_ERR_NONE;

    if ((HdmiPort >= NUM_HDMI_TX_PORT) || (pHotPlugDetect == NULL) || (pEdidDetect == NULL) || (pSinkInfo == NULL)) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            (void)HDMI_DetectSink(pHotPlugDetect, pEdidDetect);
            if (*pEdidDetect != HDMI_EDID_UNAVAIL) {
                RetVal = AmbaHDMI_TxCopySinkInfo(pSinkInfo);
            }

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
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
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    UINT32 HotPlugDetect, EdidDetect;
    UINT32 RetVal = HDMI_ERR_NONE;
    UINT8 VideoIdCode;
#if defined(CONFIG_QNX)
    int fd;
    vout_config_t VoutClkCfg;
    clk_freq_t ClkFreq;
#endif

    if ((HdmiPort >= NUM_HDMI_TX_PORT) || (pVideoConfig == NULL)) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            VideoIdCode = HDMI_GET_RAW_VIC(pVideoConfig->VideoIdCode);

            RetVal = HDMI_ValidateConfig(VideoIdCode, pVideoConfig, pAudioConfig);
            if (RetVal == HDMI_ERR_NONE) {
                RetVal = AmbaRTSL_HdmiReset(1U);

                /* Since gclk_vo is from pll_out_hdmi, set a default value to hdmi pll for accessing video control register  */
#if defined(CONFIG_QNX)
                fd = open("/dev/clock", O_RDWR);
                if (fd == -1) {
                    RetVal = HDMI_ERR_OPEN_FILE;
                } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                    VoutClkCfg.VoutID = AMBA_CLK_VOUTC;
                    ClkFreq.id = AMBA_CLK_VOUTC;
#else
                    VoutClkCfg.device = AMBA_SYS_VOUT_DEVICE_TV;
                    ClkFreq.id = AMBA_CLK_VOUTTV;
#endif
                    VoutClkCfg.Ratio = 10U;
                    VoutClkCfg.Type = AMBA_PLL_VOUT_TV_HDMI;
                    ClkFreq.freq = 27000000U;

                    if (devctl(fd, DCMD_CLOCK_CONFIG_VOUT, &VoutClkCfg, sizeof(VoutClkCfg), NULL) != 0) {
                        RetVal = HDMI_ERR_DEV_CTL;
                    } else {
                        if (devctl(fd, DCMD_CLOCK_SET_FREQ, &ClkFreq, sizeof(ClkFreq), NULL) != 0) {
                            RetVal = HDMI_ERR_DEV_CTL;
                        }
                    }

                    close(fd);
                }
#else
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                (void)AmbaRTSL_PllSetVoutCClkHint(AMBA_PLL_VOUT_TV_HDMI, 10U);
                (void)AmbaRTSL_PllSetVoutCClk(27000000U);
#else
                (void)AmbaRTSL_PllSetVoutTvClkHint(AMBA_PLL_VOUT_TV_HDMI, 10U);
                (void)AmbaRTSL_PllSetVoutTvClk(27000000U);
#endif
#endif
                RetVal |= AmbaRTSL_HdmiReset(0U);
                (void)AmbaRTSL_HdmiConfigVideo(&AmbaHdmiCtrl.VideoCfg.CustomDTD);
                HDMI_SetClockRatio(AmbaHdmiCtrl.VideoCfg.CustomDTD.PixelClock);

                /* DVI does not support transport of CEA InfoFrames, audio or YCbCr pixel data. */
                (void)HDMI_DetectSink(&HotPlugDetect, &EdidDetect);
#if defined(CONFIG_QNX)
                EdidDetect = HDMI_EDID_HDMI;  /* Force to use HDMI interface */
#endif
                if (EdidDetect == HDMI_EDID_HDMI) {
                    RetVal = AmbaWrap_memset(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_AUDIO], 0, sizeof(AMBA_HDMI_DATA_ISLAND_PACKET_s));
                    RetVal |= AmbaWrap_memset(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_SPD], 0, sizeof(AMBA_HDMI_DATA_ISLAND_PACKET_s));
                    RetVal |= AmbaWrap_memset(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_AVI], 0, sizeof(AMBA_HDMI_DATA_ISLAND_PACKET_s));
                    RetVal |= AmbaWrap_memset(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_VENDOR_SPECIFIC], 0, sizeof(AMBA_HDMI_DATA_ISLAND_PACKET_s));
                    if (RetVal == ERR_NONE) {
                        (void)AmbaHDMI_TxConfigAudio(&AmbaHdmiCtrl.VideoCfg, &AmbaHdmiCtrl.AudioCfg);
                        (void)AmbaHDMI_TxSendAudioInfoFrame(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_AUDIO], &AmbaHdmiCtrl.AudioCfg, AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_AUDIO]);
                        (void)AmbaHDMI_TxSendSpdInfoFrame(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_SPD], &AmbaHdmiCtrl.DeviceCfg, AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_SPD]);
                        (void)AmbaHDMI_TxSendAviInfoFrame(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_AVI], &AmbaHdmiCtrl.VideoCfg, AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_AVI]);
                        (void)AmbaHDMI_TxSendVendorInfoFrame(&AmbaHdmiInfoFrameBuf[HDMI_INFOFRAME_VENDOR_SPECIFIC], &AmbaHdmiCtrl.VideoCfg, AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_VENDOR_SPECIFIC]);

                        /* Update all infoframe packets */
                        AmbaRTSL_HdmiPacketUpdate();
                    } else {
                        RetVal = HDMI_ERR_UNEXPECTED;
                    }
                }
                if (RetVal == HDMI_ERR_NONE) {
                    RetVal = AmbaHDMI_ConfigDSP(&AmbaHdmiCtrl.VideoCfg);
                }
            }

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
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
    const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig = &AmbaHdmiCtrl.VideoCfg;
    const AMBA_HDMI_VIDEO_TIMING_s *pDtd = &pVideoConfig->CustomDTD;
    UINT16 FrameWidth, FrameHeight;
    UINT32 RetVal = HDMI_ERR_NONE;
    UINT8 StrictFrameRate, VideoIdCode;

    if ((pVideoConfig->VideoIdCode & HDMI_FRAME_RATE_STRICT) == 0U) {
        StrictFrameRate = 0U;
    } else {
        StrictFrameRate = 1U;
    }

    VideoIdCode = HDMI_GET_RAW_VIC(pVideoConfig->VideoIdCode);

    if ((HdmiPort >= NUM_HDMI_TX_PORT) || (pModeInfo == NULL)) {
        RetVal = HDMI_ERR_ARG;
    } else {
#if defined(CONFIG_QNX)
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_VOUTC, &pModeInfo->VideoConfig.PixelClkFreq);
#else
        (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_VOUTTV, &pModeInfo->VideoConfig.PixelClkFreq);
#endif
#else
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        pModeInfo->VideoConfig.PixelClkFreq     = AmbaRTSL_PllGetVoutCClk();
#else
        pModeInfo->VideoConfig.PixelClkFreq     = AmbaRTSL_PllGetVoutTvClk();
#endif
#endif
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

        if (pVideoConfig->PixelFormat == HDMI_PIXEL_FORMAT_YCC_420) {
            /* Restore to orignal size when GetModeInfo */
            pModeInfo->VideoConfig.HsyncFrontPorch  *= 2U;
            pModeInfo->VideoConfig.HsyncPulseWidth  *= 2U;
            pModeInfo->VideoConfig.HsyncBackPorch   *= 2U;
            pModeInfo->VideoConfig.ActivePixels     *= 2U;
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
        if (StrictFrameRate == 0U) {
            pModeInfo->VideoFrameRate   = ((FLOAT)pModeInfo->VideoConfig.PixelClkFreq * 1.001f) / ((FLOAT)FrameWidth * (FLOAT)FrameHeight);
        } else {
            pModeInfo->VideoFrameRate   = ((FLOAT)pModeInfo->VideoConfig.PixelClkFreq) / ((FLOAT)FrameWidth * (FLOAT)FrameHeight);
        }

        if (pModeInfo->VideoConfig.DisplayMethod == AMBA_VIDEO_SCAN_INTERLACED) {
            /* Provide field rate when interlaced mode */
            pModeInfo->VideoFrameRate *= 2.0f;
        }

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
 *  AmbaHDMI_TxSetPhyCtrl - Set HDMI PHY control
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] ParamID PHY parameter type
 *  @param[in] ParamVal PHY parameter value
 *  @return error code
 */
UINT32 AmbaHDMI_TxSetPhyCtrl(UINT32 HdmiPort, UINT32 ParamID, UINT32 ParamVal)
{
    UINT32 RetVal = HDMI_ERR_NONE;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = HDMI_ERR_ARG;
    } else {
        AmbaRTSL_HdmiSetPhyCtrl(ParamID, ParamVal);
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
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig = &AmbaHdmiCtrl.VideoCfg;
    UINT32 HotPlugDetect, EdidDetect;
    UINT32 RetVal = HDMI_ERR_NONE;

    if ((HdmiPort >= NUM_HDMI_TX_PORT) || (pVideoConfig->CustomDTD.PixelClock == 0U)) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            RetVal = HDMI_DetectSink(&HotPlugDetect, &EdidDetect);
#if defined(CONFIG_QNX)
            EdidDetect = HDMI_EDID_HDMI;  /* Force to use HDMI interface */
#endif
            if (EdidDetect == HDMI_EDID_HDMI) {
                AmbaRTSL_HdmiStart(1U); /* Start HDMI signal encoder */
            } else {
                AmbaRTSL_HdmiStart(0U); /* Start DVI signal encoder */
            }

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
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
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    UINT32 RetVal = HDMI_ERR_NONE;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            AmbaRTSL_HdmiStop();    /* Stop DVI/HDMI signal encoder */

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
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
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    UINT32 RetVal = HDMI_ERR_NONE;

    if ((HdmiPort >= NUM_HDMI_TX_PORT) || (pSrcProductInfo == NULL)) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            if (AmbaWrap_memcpy(&AmbaHdmiCtrl.DeviceCfg, pSrcProductInfo, sizeof(AMBA_HDMI_SOURCE_PRODUCT_INFO_s)) != ERR_NONE) {
                RetVal = HDMI_ERR_UNEXPECTED;
            }

            if (pSrcProductInfo->SrcType < NUM_HDMI_SRC_TYPE) {
                AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_SPD] = HDMI_PACKET_SEND_EVERY_FRAME;
            } else {
                AmbaHdmiInfoFrameTxMode[HDMI_INFOFRAME_SPD] = HDMI_PACKET_SEND_NONE;
            }

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
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
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket;
    UINT32 InfoFrameType = 0U;
    UINT32 i, RetVal = HDMI_ERR_NONE;

    if ((HdmiPort >= NUM_HDMI_TX_PORT) || (pInfoFramePkt == NULL) || (TxMode >= NUM_HDMI_PACKET_SEND_MODE)) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            /* Update data */
            InfoFrameType = pInfoFramePkt->Header & 0x7fU;
            if (InfoFrameType >= NUM_HDMI_INFOFRAME_TYPE) {
                RetVal = HDMI_ERR_NA;
            } else if ((InfoFrameType == HDMI_INFOFRAME_AUDIO) ||
                       (InfoFrameType == HDMI_INFOFRAME_AVI) ||
                       (InfoFrameType == HDMI_INFOFRAME_SPD) ||
                       (InfoFrameType == HDMI_INFOFRAME_VENDOR_SPECIFIC)) {
                RetVal = HDMI_ERR_PKT_UNAVIL;
            } else {
                pPacket = &AmbaHdmiInfoFrameBuf[InfoFrameType];
                pPacket->Header = pInfoFramePkt->Header;
                for (i = 0U; i < HDMI_INFOFRAME_SIZE_MAX; i ++) {
                    pPacket->Payload[i] = pInfoFramePkt->Payload[i];
                }
                AmbaHdmiInfoFrameTxMode[InfoFrameType] = TxMode;

                AmbaRTSL_HdmiDataIslandPackCtrl(pPacket, AmbaHdmiInfoFrameTxMode[InfoFrameType]);
            }

            if (RetVal == HDMI_ERR_NONE) {
                HDMI_RefreshInfoFrame(InfoFrameType);
            }

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
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
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    UINT32 RetVal = HDMI_ERR_NONE;

    if ((HdmiPort >= NUM_HDMI_TX_PORT) || (InfoFrameType >= NUM_HDMI_INFOFRAME_TYPE) || (pInfoFramePkt == NULL)) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            if (AmbaWrap_memcpy(pInfoFramePkt, &AmbaHdmiInfoFrameBuf[InfoFrameType], sizeof(AMBA_HDMI_DATA_ISLAND_PACKET_s)) != ERR_NONE) {
                RetVal = HDMI_ERR_UNEXPECTED;
            }

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
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
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    UINT32 RetVal = HDMI_ERR_NONE;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            AmbaRTSL_HdmiSendAVMUTE(1U);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
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
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    UINT32 RetVal = HDMI_ERR_NONE;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            AmbaRTSL_HdmiSendAVMUTE(0U);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

#if defined(CONFIG_QNX)
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
extern AMBA_VOUT_DISPLAY1_CONFIG_REG_s AmbaVOUT_Display1Config;    /* cv2x vout display buffer */
//#else
//TODO: cv5x vout display buffer
//extern AMBA_VOUT_DISPLAY_C_CONFIG_REG_s AmbaVOUT_Display2Config;
#endif
UINT32 AmbaHDMI_TxGetDispCfgAddr(ULONG *pAddr, UINT32 *pSize)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    UINT32 RetVal = HDMI_ERR_NONE;
    const AMBA_VOUT_DISPLAY1_CONFIG_REG_s *pVoutDisp1ConfigAddr;
    ULONG Addr;

    if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = HDMI_ERR_MUTEX;
    } else {
        pVoutDisp1ConfigAddr = &AmbaVOUT_Display1Config;
        AmbaMisra_TypeCast(&Addr, &pVoutDisp1ConfigAddr);
        *pAddr = Addr;

        *pSize = AMBA_VOUT_DISP_BUF_SIZE;

        if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = HDMI_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}
#endif
/**
 *  AmbaHDMI_TxEnableTestPattern - Set HDMI video data source to be test pattern
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
UINT32 AmbaHDMI_TxEnableTestPattern(UINT32 HdmiPort)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiMutex;
    UINT32 RetVal = HDMI_ERR_NONE;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            AmbaRTSL_HdmiSetVideoDataSource(1);
            AmbaRTSL_HdmiSetAudioDataSource(1);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  HDMI_ValdidateEDID - Valdidate one EDID block
 *  @param[in] pEdidBlock pointer to a storage of EDID block
 *  @return error code
 */
static UINT32 HDMI_ValdidateEDID(const UINT8 *pEdidBlock)
{
    UINT8 Checksum = 0U;
    UINT32 i, RetVal = HDMI_ERR_NONE;

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
    UINT32 RetVal = HDMI_ERR_NONE;

    if (pEdidBlock == NULL) {
        RetVal = HDMI_ERR_ARG;
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

#if defined(CONFIG_QNX)
        /* QNX middleware does not support multiple write in AmbaI2C_MasterReadAfterWrite, so we split them */
        if (AmbaI2C_MasterWrite(AMBA_I2C_HDMI_CHANNEL, I2C_SPEED_STANDARD, &I2cTxConfig[0], NULL, 1000U) != I2C_ERR_NONE) {
            RetVal = HDMI_ERR_DDC;
        } else if (AmbaI2C_MasterReadAfterWrite(AMBA_I2C_HDMI_CHANNEL, I2C_SPEED_STANDARD, 1U, &I2cTxConfig[1], &I2cRxConfig, NULL, 1000U) != I2C_ERR_NONE) {
#else
        if (AmbaI2C_MasterReadAfterWrite(AMBA_I2C_HDMI_CHANNEL, I2C_SPEED_STANDARD, 2U, &I2cTxConfig[0], &I2cRxConfig, NULL, 1000U) != I2C_ERR_NONE) {
#endif
            /* Disconnect occurs or the DDC connection is unstable */
            RetVal = HDMI_ERR_DDC;
        } else {
            /* Validate EDID block by checksum */
            RetVal = HDMI_ValdidateEDID(pEdidBlock);
        }
    }

    return RetVal;
}

/**
 *  HDMI_DetectSink - Detect cable connection (DVI/HDMI) and read EDID if cable plugged
 *  @param[out] pHotPlugDetect Hot-Plug Detection Status
 *  @param[out] pEdidDetect EDID Detection Status
 *  @return error code
 */
static UINT32 HDMI_DetectSink(UINT32 *pHotPlugDetect, UINT32 *pEdidDetect)
{
    const UINT8 *pEdidBlock = AmbaHdmiSinkEDID[0];
    const EDID_FORMAT_s *pEdidStruct;
    UINT32 HotPlugged, RxSensed;
    UINT32 i, RetVal = HDMI_ERR_NONE;

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
        if (RetVal != HDMI_ERR_NONE) {
            *pEdidDetect = HDMI_EDID_UNAVAIL;
        } else {
            AmbaMisra_TypeCast(&pEdidStruct, &pEdidBlock);
            for (i = 1U; i <= pEdidStruct->ExtensionBlockCount; i ++) {
                RetVal = HDMI_ReadEDID(i, AmbaHdmiSinkEDID[i]);
                if (RetVal != HDMI_ERR_NONE) {
                    *pEdidDetect = HDMI_EDID_UNAVAIL;
                    break;
                }
            }
        }
    }

    if (*pEdidDetect != HDMI_EDID_UNAVAIL) {
        /* Parse EDID and determine whether the sink device is HDMI capable */
        RetVal = AmbaHDMI_TxParseEDID((const UINT8 (*)[128])AmbaHdmiSinkEDID, pEdidDetect);
    }

    return RetVal;
}

/**
 *  HDMI_WriteSCDC - Write data to SCDC.
 *  @param[in] TxDataSize Data size in Byte
 *  @param[in] pTxDataBuf Pointer to the Tx data buffer (the 1st Byte can be the I2C Subaddress)
 *  @return error code
 */
static UINT32 HDMI_WriteSCDC(UINT32 TxDataSize, UINT8 *pTxDataBuf)
{
    AMBA_I2C_TRANSACTION_s TxTransaction;
    UINT32 RetVal = HDMI_ERR_NONE;

    if ((TxDataSize == 0U) || (pTxDataBuf == NULL)) {
        RetVal = HDMI_ERR_ARG;
    } else {
        TxTransaction.SlaveAddr = SCDC_ADDR_WRITE;
        TxTransaction.DataSize = TxDataSize;
        TxTransaction.pDataBuf = pTxDataBuf;

        if (AmbaI2C_MasterWrite(AMBA_I2C_HDMI_CHANNEL, I2C_SPEED_STANDARD, &TxTransaction, NULL, 1000U) != I2C_ERR_NONE) {
            RetVal = HDMI_ERR_SCDC;
        }
    }

    return RetVal;
}

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
    ScdcData[0U] = 0x20U;           /* Offset of TMDS_Config */
    (void)HDMI_WriteSCDC(2U, ScdcData);
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
    UINT32 RetVal = HDMI_ERR_NONE;

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

    if (pVideoConfig->PixelFormat == HDMI_PIXEL_FORMAT_YCC_420) {
        /* YUV420 mode needs to half its horizontal size for hdmi controller */
        AmbaHdmiCtrl.VideoCfg.CustomDTD.PixelClock      = AmbaHdmiCtrl.VideoCfg.CustomDTD.PixelClock >> 1U;
        AmbaHdmiCtrl.VideoCfg.CustomDTD.HsyncFrontPorch = AmbaHdmiCtrl.VideoCfg.CustomDTD.HsyncFrontPorch >> 1U;
        AmbaHdmiCtrl.VideoCfg.CustomDTD.HsyncPulseWidth = AmbaHdmiCtrl.VideoCfg.CustomDTD.HsyncPulseWidth >> 1U;
        AmbaHdmiCtrl.VideoCfg.CustomDTD.HsyncBackPorch  = AmbaHdmiCtrl.VideoCfg.CustomDTD.HsyncBackPorch >> 1U;
        AmbaHdmiCtrl.VideoCfg.CustomDTD.ActivePixels    = AmbaHdmiCtrl.VideoCfg.CustomDTD.ActivePixels >> 1U;
    }

    return RetVal;
}
