/**
 *  @file AmbaCVBS.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details composite video signal encoder related APIs
 *
 */

#if defined(CONFIG_QNX)
#include "Generic.h"
#include "hw/ambarella_clk.h"
#endif

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaCVBS.h"
#if defined(CONFIG_LINUX) && (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52))
#include "hw/ambarella_hdmi.h"
#else
#include "AmbaRTSL_HDMI.h"
#endif
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_VOUT.h"


static AMBA_KAL_MUTEX_t AmbaCvbsMutex;
static UINT32 AmbaCvbsSystem = CVBS_VIDEO_NTSC;

static AMBA_CVBS_IRE_CTRL_s AmbaCvbsConfig[NUM_CVBS_SYSTEM] = {
    [CVBS_VIDEO_NTSC] = {
        .BlankLevel     = 0x7AU,
        .SyncLevel      = 0x00U,
        .BlackLevel     = 0x7DU,
        .ClampLevel     = 0x00U,
        .OutGain        = 0x440U,   /* Original: 0x3B8. IQ team suggests to use 0x440. */
        .OutOffset      = 0x000U,
        .OutMin         = 0x000U,
        .OutMax         = 0x3FFU,
    },

    [CVBS_VIDEO_PAL] = {
        .BlankLevel     = 0x7EU,
        .SyncLevel      = 0x04U,
        .BlackLevel     = 0x69U,
        .ClampLevel     = 0x00U,
        .OutGain        = 0x3D8U,   /* Original: 0x3D8. */
        .OutOffset      = 0x000U,
        .OutMin         = 0x000U,
        .OutMax         = 0x3FFU,
    },
};

static AMBA_CVBS_INFO_s AmbaCvbsInfo[NUM_CVBS_SYSTEM] = {
    [CVBS_VIDEO_NTSC] = {
        .PixelClkFreq   = 27000000U,
        .VideoWidth     = 720U,
        .VideoHeight    = 480U,
        .TimeScale      = 60000U,
        .NumUnitsInTick = 1001U,
        .ScanType       = 1U,
    },

    [CVBS_VIDEO_PAL] = {
        .PixelClkFreq   = 27000000U,
        .VideoWidth     = 720U,
        .VideoHeight    = 576U,
        .TimeScale      = 50U,
        .NumUnitsInTick = 1U,
        .ScanType       = 1U,
    },
};

static AMBA_VOUT_DISPLAY_CVBS_CONFIG_REG_s AmbaCvbsDisplayCoeffTable[NUM_CVBS_SYSTEM] = {
    [CVBS_VIDEO_NTSC] = {
        .DispCtrl               = 0x10000021U,
        .DispFrmSize            = 0x06B30106U,
        .DispFldHeight          = 0x00000105U,
        .DispTopActiveStart     = 0x01110016U,
        .DispTopActiveEnd       = 0x06B00105U,
        .DispBtmActiveStart     = 0x01110015U,
        .DispBtmActiveEnd       = 0x06B00104U,
        .DispBackgroundColor    = 0x00108080U,
        .AnalogOutputMode       = 0x00000003U,
        .AnalogHSync            = 0x00000001U,
        .AnalogVSyncTopStart    = 0x00000000U,
        .AnalogVSyncTopEnd      = 0x00080000U,
        .AnalogVSyncBtmStart    = 0x00003FFFU,
        .AnalogVSyncBtmEnd      = 0x00013FFFU,
        .AnalogCSC0             = 0x04000400U,
        .AnalogCSC1             = 0x00000400U,
        .AnalogCSC2             = 0x00000000U,
        .AnalogCSC3             = 0x03FF0000U,
        .AnalogCSC4             = 0x03FF0000U,
        .AnalogCSC5             = 0x03FF0000U,
        .AnalogCSC6             = 0x03B80000U,
        .AnalogCSC7             = 0x00000000U,
        .AnalogCSC8             = 0x03FF0000U,
    },

    [CVBS_VIDEO_PAL] = {
        .DispCtrl               = 0x10000023U,
        .DispFrmSize            = 0x06BF0138U,
        .DispFldHeight          = 0x00000137U,
        .DispTopActiveStart     = 0x011E0017U,
        .DispTopActiveEnd       = 0x06BD0136U,
        .DispBtmActiveStart     = 0x011E0016U,
        .DispBtmActiveEnd       = 0x06BD0135U,
        .DispBackgroundColor    = 0x00108080U,
        .AnalogOutputMode       = 0x00000003U,
        .AnalogHSync            = 0x00000001U,
        .AnalogVSyncTopStart    = 0x00000000U,
        .AnalogVSyncTopEnd      = 0x00080000U,
        .AnalogVSyncBtmStart    = 0x00003FFFU,
        .AnalogVSyncBtmEnd      = 0x00013FFFU,
        .AnalogCSC0             = 0x04000400U,
        .AnalogCSC1             = 0x00000400U,
        .AnalogCSC2             = 0x00000000U,
        .AnalogCSC3             = 0x03FF0000U,
        .AnalogCSC4             = 0x03FF0000U,
        .AnalogCSC5             = 0x03FF0000U,
        .AnalogCSC6             = 0x03D80000U,
        .AnalogCSC7             = 0x00000000U,
        .AnalogCSC8             = 0x03FF0000U,
    }
};

static AMBA_VOUT_TVENC_CONTROL_REG_s  AmbaCvbsTveCoeffTable[NUM_CVBS_SYSTEM] = {
    [CVBS_VIDEO_NTSC] = {
        .TvEncReg32             = 0x0000001FU,
        .TvEncReg33             = 0x0000007CU,
        .TvEncReg34             = 0x000000F0U,
        .TvEncReg35             = 0x00000021U,
        .TvEncReg36             = 0x00000000U,
        .TvEncReg37             = 0x00000000U,
        .TvEncReg38             = 0x00000000U,
        .TvEncReg39             = 0x00000000U,
        .TvEncReg40             = 0x00000000U,
        .TvEncReg42             = 0x0000007DU,
        .TvEncReg43             = 0x0000007AU,
        .TvEncReg44             = 0x00000000U,
        .TvEncReg45             = 0x00000000U,
        .TvEncReg46             = 0x00000030U,
        .TvEncReg47             = 0x00000000U,
        .TvEncReg50             = 0x000000C4U,
        .TvEncReg51             = 0x00000000U,
        .TvEncReg52             = 0x00000000U,
        .TvEncReg56             = 0x00000008U,
        .TvEncReg57             = 0x00000000U,
        .TvEncReg58             = 0x00000000U,
        .TvEncReg59             = 0x00000000U,
        .TvEncReg60             = 0x00000000U,
        .TvEncReg61             = 0x00000003U,
        .TvEncReg62             = 0x00000059U,
        .TvEncReg65             = 0x00000000U,
        .TvEncReg66             = 0x00000000U,
        .TvEncReg67             = 0x00000000U,
        .TvEncReg68             = 0x00000000U,
        .TvEncReg69             = 0x00000000U,
        .TvEncReg96             = 0x00000000U,
        .TvEncReg97             = 0x00000020U,
        .TvEncReg99             = 0x00000002U,
        .TvEncReg120            = 0x00000089U,
        .TvEncReg121            = 0x000003ACU,
        .Reserved0              = {0U},
        .Reserved1              = 0U,
        .Reserved2              = {0U},
        .Reserved3              = {0U},
        .Reserved4              = {0U},
        .Reserved5              = {0U},
        .Reserved6              = 0U,
        .Reserved7              = {0U},
        .Reserved8              = {0U},
    },

    [CVBS_VIDEO_PAL] = {
        .TvEncReg32             = 0x000000CBU,
        .TvEncReg33             = 0x0000008AU,
        .TvEncReg34             = 0x00000009U,
        .TvEncReg35             = 0x0000002AU,
        .TvEncReg36             = 0x00000000U,
        .TvEncReg37             = 0x00000000U,
        .TvEncReg38             = 0x00000000U,
        .TvEncReg39             = 0x00000000U,
        .TvEncReg40             = 0x00000000U,
        .TvEncReg42             = 0x00000069U,
        .TvEncReg43             = 0x0000007EU,
        .TvEncReg44             = 0x00000000U,
        .TvEncReg45             = 0x00000004U,
        .TvEncReg46             = 0x00000023U,
        .TvEncReg47             = 0x00000000U,
        .TvEncReg50             = 0x000000D3U,
        .TvEncReg51             = 0x0000002DU,
        .TvEncReg52             = 0x00000003U,
        .TvEncReg56             = 0x00000009U,
        .TvEncReg57             = 0x00000000U,
        .TvEncReg58             = 0x00000000U,
        .TvEncReg59             = 0x00000000U,
        .TvEncReg60             = 0x00000000U,
        .TvEncReg61             = 0x00000003U,
        .TvEncReg62             = 0x0000005FU,
        .TvEncReg65             = 0x00000000U,
        .TvEncReg66             = 0x00000000U,
        .TvEncReg67             = 0x00000000U,
        .TvEncReg68             = 0x00000000U,
        .TvEncReg69             = 0x00000000U,
        .TvEncReg96             = 0x00000000U,
        .TvEncReg97             = 0x00000000U,
        .TvEncReg99             = 0x00000000U,
        .TvEncReg120            = 0x00000089U,
        .TvEncReg121            = 0x000003ACU,
        .Reserved0              = {0U},
        .Reserved1              = 0U,
        .Reserved2              = {0U},
        .Reserved3              = {0U},
        .Reserved4              = {0U},
        .Reserved5              = {0U},
        .Reserved6              = 0U,
        .Reserved7              = {0U},
        .Reserved8              = {0U},
    },
};

/**
 *  AmbaCVBS_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaCVBS_DrvEntry(void)
{
    static char AmbaCvbsMutexName[16] = "AmbaCvbsMutex";
    UINT32 RetVal = CVBS_ERR_NONE;

    AmbaMisra_TouchUnused(AmbaCvbsDisplayCoeffTable);
    AmbaMisra_TouchUnused(AmbaCvbsTveCoeffTable);
    AmbaMisra_TouchUnused(AmbaCvbsConfig);

    /* Create Mutex */
    if (AmbaKAL_MutexCreate(&AmbaCvbsMutex, AmbaCvbsMutexName) != KAL_ERR_NONE) {
        RetVal = CVBS_ERR_UNEXPECTED;
    }

    return RetVal;
}

/**
 *  AmbaCVBS_SetMode - Set mode for CVBS signal
 *  @param[in] CvbsMode Supported CVBS format
 *  @return error code
 */
UINT32 AmbaCVBS_SetMode(UINT32 CvbsMode)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaCvbsMutex;
    const AMBA_VOUT_DISPLAY_CVBS_CONFIG_REG_s *pDisplayCvbsConfig;
    const AMBA_VOUT_TVENC_CONTROL_REG_s *pTveConfig;
    UINT32 RawCvbsMode = CvbsMode & (UINT32)0x0fUL;
    UINT32 ColorBarEnable = CvbsMode & (UINT32)0x10UL;
    UINT32 RetVal = CVBS_ERR_NONE;
#if defined(CONFIG_QNX)
    int fd;
    vout_config_t VoutClkCfg;
    clk_freq_t ClkFreq;
#elif defined(CONFIG_LINUX) && (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52))
    int fd;
    hdmi_config_msg_t HdmiConfigMsg;
#else
#endif

    if (CvbsMode == CVBS_POWER_DOWN) {
        (void)AmbaRTSL_VoutCvbsPowerCtrl(0U);
    } else if (RawCvbsMode >= NUM_CVBS_SYSTEM) {
        RetVal = CVBS_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = CVBS_ERR_MUTEX;
        } else {
            (void)AmbaRTSL_VoutCvbsPowerCtrl(1U);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#if defined(CONFIG_LINUX)
            fd = open("/dev/hdmi", O_RDWR);
            if (fd == -1) {
                RetVal = HDMI_ERR_OPEN_FILE;
            } else {
                HdmiConfigMsg.HdmiPort = 0U;

                if (ioctl(fd, HDMI_IOC_TX_ENABLE, &HdmiConfigMsg, sizeof(HdmiConfigMsg), NULL) != OK) {
                    RetVal = HDMI_ERR_DEV_CTL;
                }
                close(fd);
            }
#else
            (void)AmbaRTSL_HdmiSetPowerCtrl(1U);
#endif
#endif
            AmbaCvbsSystem = RawCvbsMode;

            pDisplayCvbsConfig = &AmbaCvbsDisplayCoeffTable[AmbaCvbsSystem];
            pTveConfig = &AmbaCvbsTveCoeffTable[AmbaCvbsSystem];

            /* Set clock hint and target clock */
#if defined(CONFIG_QNX)
            fd = open("/dev/clock", O_RDWR);
            if (fd == -1) {
                RetVal = HDMI_ERR_OPEN_FILE;
            } else {
#if defined(CONFIG_SOC_CV28)
                VoutClkCfg.device = AMBA_SYS_VOUT_DEVICE_LCD;
                VoutClkCfg.Type = AMBA_PLL_VOUT_LCD_CVBS;
                ClkFreq.id = AMBA_CLK_VOUTLCD;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                VoutClkCfg.VoutID = AMBA_CLK_VOUTC;
                VoutClkCfg.Type = AMBA_PLL_VOUT_TV_CVBS;
                ClkFreq.id = AMBA_CLK_VOUTC;
#else
                VoutClkCfg.device = AMBA_SYS_VOUT_DEVICE_TV;
                VoutClkCfg.Type = AMBA_PLL_VOUT_TV_CVBS;
                ClkFreq.id = AMBA_CLK_VOUTTV;
#endif
                VoutClkCfg.Ratio = 1U;
                ClkFreq.freq = AmbaCvbsInfo[AmbaCvbsSystem].PixelClkFreq;

                if (devctl(fd, DCMD_CLOCK_CONFIG_VOUT, &VoutClkCfg, sizeof(VoutClkCfg), NULL) != 0) {
                    RetVal = CVBS_ERR_DEV_CTL;
                } else {
                    if (devctl(fd, DCMD_CLOCK_SET_FREQ, &ClkFreq, sizeof(ClkFreq), NULL) != 0) {
                        RetVal = CVBS_ERR_DEV_CTL;
                    }
                }

                close(fd);
            }
#else
/* Non-QNX */
#if defined(CONFIG_SOC_CV28)
            (void)AmbaRTSL_PllSetVoutLcdClkHint(AMBA_PLL_VOUT_LCD_CVBS, 1U);
            (void)AmbaRTSL_PllSetVoutLcdClk(AmbaCvbsInfo[AmbaCvbsSystem].PixelClkFreq);
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            (void)AmbaRTSL_PllSetVoutCClkHint(AMBA_PLL_VOUT_TV_CVBS, 1U);
            (void)AmbaRTSL_PllSetVoutCClk(AmbaCvbsInfo[AmbaCvbsSystem].PixelClkFreq);
#else
            (void)AmbaRTSL_PllSetVoutTvClkHint(AMBA_PLL_VOUT_TV_CVBS, 1U);
            (void)AmbaRTSL_PllSetVoutTvClk(AmbaCvbsInfo[AmbaCvbsSystem].PixelClkFreq);
#endif
#endif
            /* Default IRE control */
            RetVal |= AmbaRTSL_VoutCvbsEnable(pDisplayCvbsConfig, pTveConfig);

            /* Update IRE control */
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_BLACK, (UINT32)AmbaCvbsConfig[AmbaCvbsSystem].BlackLevel);
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_BLANK, (UINT32)AmbaCvbsConfig[AmbaCvbsSystem].BlankLevel);
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_CLAMP, (UINT32)AmbaCvbsConfig[AmbaCvbsSystem].ClampLevel);
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_OUTGAIN, (UINT32)AmbaCvbsConfig[AmbaCvbsSystem].OutGain);
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_OUTMAX, (UINT32)AmbaCvbsConfig[AmbaCvbsSystem].OutMax);
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_OUTMIN, (UINT32)AmbaCvbsConfig[AmbaCvbsSystem].OutMin);
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_OUTOFFSET, (UINT32)AmbaCvbsConfig[AmbaCvbsSystem].OutOffset);
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_SYNC, (UINT32)AmbaCvbsConfig[AmbaCvbsSystem].SyncLevel);

            /* Show color bar or not */
            (void)AmbaRTSL_VoutCvbsEnableColorBar(ColorBarEnable, pTveConfig);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = CVBS_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaCVBS_SetIreCtrl - Set CVBS IRE control
 *  @param[in] pIreCtrl Pointer to IRE control block
 *  @return error code
 */
UINT32 AmbaCVBS_SetIreCtrl(const AMBA_CVBS_IRE_CTRL_s *pIreCtrl)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaCvbsMutex;
    UINT32 RetVal = CVBS_ERR_NONE;

    if (pIreCtrl == NULL) {
        RetVal = CVBS_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = CVBS_ERR_MUTEX;
        } else {
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_BLACK, (UINT32)pIreCtrl->BlackLevel);
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_BLANK, (UINT32)pIreCtrl->BlankLevel);
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_CLAMP, (UINT32)pIreCtrl->ClampLevel);
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_OUTGAIN, (UINT32)pIreCtrl->OutGain);
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_OUTMAX, (UINT32)pIreCtrl->OutMax);
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_OUTMIN, (UINT32)pIreCtrl->OutMin);
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_OUTOFFSET, (UINT32)pIreCtrl->OutOffset);
            (void)AmbaRTSL_VoutCvbsSetIreParam(VOUT_CVBS_IRE_PARAM_SYNC, (UINT32)pIreCtrl->SyncLevel);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = CVBS_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaCVBS_GetStatus - Get current CVBS status
 *  @param[in] pCvbsInfo CVBS configuration
 *  @param[in] pIreCtrl Pointer to IRE control block
 *  @return error code
 */
UINT32 AmbaCVBS_GetStatus(AMBA_CVBS_INFO_s *pCvbsInfo, AMBA_CVBS_IRE_CTRL_s * pIreCtrl)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaCvbsMutex;
    UINT32 RetVal = CVBS_ERR_NONE;

    if ((pCvbsInfo == NULL) && (pIreCtrl == NULL)) {
        RetVal = CVBS_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = CVBS_ERR_MUTEX;
        } else {
            if (pCvbsInfo != NULL) {
                pCvbsInfo->PixelClkFreq   = AmbaCvbsInfo[AmbaCvbsSystem].PixelClkFreq;
                pCvbsInfo->VideoWidth     = AmbaCvbsInfo[AmbaCvbsSystem].VideoWidth;
                pCvbsInfo->VideoHeight    = AmbaCvbsInfo[AmbaCvbsSystem].VideoHeight;
                pCvbsInfo->TimeScale      = AmbaCvbsInfo[AmbaCvbsSystem].TimeScale;
                pCvbsInfo->NumUnitsInTick = AmbaCvbsInfo[AmbaCvbsSystem].NumUnitsInTick;
                pCvbsInfo->ScanType       = AmbaCvbsInfo[AmbaCvbsSystem].ScanType;
            }

            if (pIreCtrl != NULL) {
                pIreCtrl->BlackLevel = (UINT8)AmbaRTSL_VoutCvbsGetIreParam(VOUT_CVBS_IRE_PARAM_BLACK);
                pIreCtrl->BlankLevel = (UINT8)AmbaRTSL_VoutCvbsGetIreParam(VOUT_CVBS_IRE_PARAM_BLANK);
                pIreCtrl->ClampLevel = (UINT8)AmbaRTSL_VoutCvbsGetIreParam(VOUT_CVBS_IRE_PARAM_CLAMP);
                pIreCtrl->OutGain    = (UINT16)AmbaRTSL_VoutCvbsGetIreParam(VOUT_CVBS_IRE_PARAM_OUTGAIN);
                pIreCtrl->OutOffset  = (UINT16)AmbaRTSL_VoutCvbsGetIreParam(VOUT_CVBS_IRE_PARAM_OUTOFFSET);
                pIreCtrl->OutMax     = (UINT16)AmbaRTSL_VoutCvbsGetIreParam(VOUT_CVBS_IRE_PARAM_OUTMAX);
                pIreCtrl->OutMin     = (UINT16)AmbaRTSL_VoutCvbsGetIreParam(VOUT_CVBS_IRE_PARAM_OUTMIN);
                pIreCtrl->SyncLevel  = (UINT8)AmbaRTSL_VoutCvbsGetIreParam(VOUT_CVBS_IRE_PARAM_SYNC);
            }

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = CVBS_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}
