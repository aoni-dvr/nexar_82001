/**
 *  @file AmbaVOUT.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Video Output APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

#include "AmbaDrvEntry.h"
#include "AmbaVOUT.h"
#include "AmbaVOUT_Ctrl.h"
#include "AmbaVOUT_Priv.h"
#include "AmbaRTSL_VOUT.h"

#if defined(CONFIG_VOUT_ASIL)
#include "AmbaSafety_VOUT.h"
#endif

static UINT32 VOUT_DrvInited;
static AMBA_KAL_MUTEX_t AmbaVoutMutex;

/**
 *  AmbaVout_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_DrvEntry(void)
{
    static char AmbaVoutMutexName[16] = "AmbaVoutMutex";
    UINT32 RetVal = VOUT_ERR_NONE;
    AMBA_VOUT_CSC_MATRIX_s *pCscMatrix;
#if defined(CONFIG_VOUT_ASIL)
    UINT32 VoutID;
#endif

    /* Create Mutex */
    if (AmbaKAL_MutexCreate(&AmbaVoutMutex, AmbaVoutMutexName) != KAL_ERR_NONE) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {

#if defined(CONFIG_VOUT_ASIL)
        for (VoutID = 0U; VoutID < AMBA_NUM_VOUT_INDEX; VoutID++) {
            (void)AmbaSafety_VoutInit(VoutID);
        }
#endif
        (void)AmbaRTSL_Vout0InitDispConfig();
        (void)AmbaRTSL_Vout1InitDispConfig();

        VOUT_DrvInited = 1U;

        /* Set default digital csc: YCC(Limited) to RGB(Full) */
        (void)AmbaVout_GetCscMatrix(CSC_TYPE_BT601, 1U, &pCscMatrix);
        (void)AmbaRTSL_VoutMipiDsiSetCsc(pCscMatrix, 0U);

        /* Set default MIPI CSI csc: CSC_TYPE_IDENTITY */
        (void)AmbaVout_GetCscMatrix(CSC_TYPE_IDENTITY, 1U, &pCscMatrix);
        (void)AmbaRTSL_VoutMipiCsiSetCsc(pCscMatrix, 0U);
    }

    return RetVal;
}

/**
 *  AmbaVout_ChannelSelect - Selects target vout channel to be programmed
 *  @param[in] VoutChan Target vout channel
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_ChannelSelect(UINT32 VoutChannel)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (VoutChannel >= 2U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            AmbaRTSL_VoutChannelSelect(VoutChannel);
#if defined(CONFIG_VOUT_ASIL)
            RTSL_VoutChannelSelect(VoutChannel);
#endif
            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_GetDisp0ConfigAddr - Get VOUT display 0 configuration
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pVirtAddr Memory address of the configuration
 *  @param[out] pSize Memory size of the configuration
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetDisp0ConfigAddr(ULONG *pVirtAddr, UINT32 *pSize)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout0GetDispConfigAddr(pVirtAddr);
            *pSize = AMBA_VOUT_DISP_BUF_SIZE;

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_GetDisp1ConfigAddr - Get VOUT display 1 configuration
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pVirtAddr Memory address of the configuration
 *  @param[out] pSize Memory size of the configuration
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetDisp1ConfigAddr(ULONG *pVirtAddr, UINT32 *pSize)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout1GetDispConfigAddr(pVirtAddr);
            *pSize = AMBA_VOUT_DISP_BUF_SIZE;

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_GetDisp0DigiCscAddr - Get VOUT display 0 digital csc configuration
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pVirtAddr Memory address of the configuration
 *  @param[out] pSize Memory size of the configuration
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetDisp0DigiCscAddr(ULONG *pVirtAddr, UINT32 *pSize)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
            *pSize = AMBA_VOUT_CSC_BUF_SIZE;
        } else {
            RetVal = AmbaRTSL_Vout0GetDigitalCscAddr(pVirtAddr);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_GetDisp1AnalogCscAddr - Get VOUT display 1 analog csc configuration
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pVirtAddr Memory address of the configuration
 *  @param[out] pSize Memory size of the configuration
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetDisp1AnalogCscAddr(ULONG *pVirtAddr, UINT32 *pSize)
{
    AmbaMisra_TouchUnused(pVirtAddr);
    AmbaMisra_TouchUnused(pSize);

    return VOUT_ERR_PROTOCOL;
}

/**
 *  AmbaVout_GetDisp1DigiCscAddr - Get VOUT display 1 digital csc configuration
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pVirtAddr Memory address of the configuration
 *  @param[out] pSize Memory size of the configuration
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetDisp1DigiCscAddr(ULONG *pVirtAddr, UINT32 *pSize)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout1GetDigitalCscAddr(pVirtAddr);
            *pSize = AMBA_VOUT_CSC_BUF_SIZE;

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_GetDisp1HdmiCscAddr - Get VOUT display 1 hdmi csc configuration
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pVirtAddr Memory address of the configuration
 *  @param[out] pSize Memory size of the configuration
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetDisp1HdmiCscAddr(ULONG *pVirtAddr, UINT32 *pSize)
{
    AmbaMisra_TouchUnused(pVirtAddr);
    AmbaMisra_TouchUnused(pSize);

    return VOUT_ERR_PROTOCOL;
}

/**
 *  AmbaVout_SetDisp0VinVoutSync - Enable/Disable vin-vout sync function in vout display 0 configuration
 *  @param[in] EnableFlag Enable(1)/Disable(0) vin-vout sync function
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_SetDisp0VinVoutSync(UINT32 EnableFlag)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (EnableFlag > 1U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout0SetVinVoutSync(EnableFlag);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_SetDisp1VinVoutSync - Enable/Disable vin-vout sync function in vout display 1 configuration
 *  @param[in] EnableFlag Enable(1)/Disable(0) vin-vout sync function
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_SetDisp1VinVoutSync(UINT32 EnableFlag)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (EnableFlag > 1U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout1SetVinVoutSync(EnableFlag);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_DigiYccEnable - Configuration VOUT display interface as digital YCbCr output
 *  @param[in] YccMode YCbCr pixel format
 *  @param[in] ColorOrder Color order selection of a pixel
 *  @param[in] pDataLatch Data latching configuraiton of external signals
 *  @param[in] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_DigiYccEnable(UINT32 YccMode, UINT32 ColorOrder, const AMBA_VOUT_DATA_LATCH_CONFIG_s *pDataLatch, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    (void)YccMode;
    (void)ColorOrder;
    (void)pDataLatch;
    (void)pDisplayTiming;

    return VOUT_ERR_PROTOCOL;
}

/**
 *  AmbaVout_DigiYccSetPhyCtrl - Adjust electrical signal quality of digital YCC interface
 *  @param[in] ParamID PHY control parameter id
 *  @param[in] ParamVal PHY control parameter value
 *  @return error code
 */
UINT32 AmbaVout_DigiYccSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
    (void)ParamID;
    (void)ParamVal;

    return VOUT_ERR_PROTOCOL;
}

/**
 *  AmbaVout_DigiYccGetStatus - Get vout status of digital YCC interface
 *  @param[out] pDigiYccConfig Pixel format configuration
 *  @param[out] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_DigiYccGetStatus(AMBA_VOUT_DIGITAL_YCC_CONFIG_s *pDigiYccConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    AmbaMisra_TouchUnused(pDigiYccConfig);
    AmbaMisra_TouchUnused(pDisplayTiming);

    return VOUT_ERR_PROTOCOL;
}

/**
 *  AmbaVout_DigiYccSetCscMatrix - Set color space conversion matrix
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] YccMode Bit depth information
 *  @return error code
 */
UINT32 AmbaVout_DigiYccSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 YccMode)
{
    (void)pCscMatrix;
    (void)YccMode;

    return VOUT_ERR_PROTOCOL;
}

/**
 *  AmbaVout_DigiRgbEnable - Configuration VOUT display interface as digital RGB output
 *  @param[in] RgbMode RGB pixel format
 *  @param[in] ColorOrder Color order selection of a pixel
 *  @param[in] pDataLatch Data latching configuraiton of external signals
 *  @param[in] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_DigiRgbEnable(UINT32 RgbMode, UINT32 ColorOrder, const AMBA_VOUT_DATA_LATCH_CONFIG_s *pDataLatch, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    (void)RgbMode;
    (void)ColorOrder;
    (void)pDataLatch;
    (void)pDisplayTiming;

    return VOUT_ERR_PROTOCOL;
}

/**
 *  AmbaVout_DigiRgbSetPhyCtrl - Adjust electrical signal quality of digital RGB interface
 *  @param[in] ParamID PHY control parameter id
 *  @param[in] ParamVal PHY control parameter value
 *  @return error code
 */
UINT32 AmbaVout_DigiRgbSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
    (void)ParamID;
    (void)ParamVal;

    return VOUT_ERR_PROTOCOL;
}

/**
 *  AmbaVout_DigiRgbGetStatus - Get vout status of digital RGB interface
 *  @param[out] pDigiRgbConfig Pixel format configuration
 *  @param[out] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_DigiRgbGetStatus(AMBA_VOUT_DIGITAL_RGB_CONFIG_s *pDigiRgbConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    AmbaMisra_TouchUnused(pDigiRgbConfig);
    AmbaMisra_TouchUnused(pDisplayTiming);

    return VOUT_ERR_PROTOCOL;
}

/**
 *  AmbaVout_DigiRgbSetCscMatrix - Set color space conversion matrix
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] RgbMode Bit depth information
 *  @return error code
 */
UINT32 AmbaVout_DigiRgbSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 RgbMode)
{
    (void)pCscMatrix;
    (void)RgbMode;

    return VOUT_ERR_PROTOCOL;
}

/**
 *  AmbaVout_FpdLinkEnable - Configuration VOUT display interface as FPD-Link output
 *  @param[in] FpdLinkMode FPD-Link pixel format
 *  @param[in] ColorOrder Color order selection of a pixel
 *  @param[in] DataEnablePolarity Polarity of data enable signal
 *  @param[in] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_FpdLinkEnable(UINT32 FpdLinkMode, UINT32 ColorOrder, UINT32 DataEnablePolarity, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    AmbaMisra_TouchUnused(&FpdLinkMode);
    AmbaMisra_TouchUnused(&ColorOrder);
    AmbaMisra_TouchUnused(&DataEnablePolarity);
    AmbaMisra_TouchUnused(&pDisplayTiming);

    return VOUT_ERR_INVALID_API;
}

/**
 *  AmbaVout_FpdLinkSetPhyCtrl - Adjust electrical signal quality of FPD-Link interface
 *  @param[in] ParamID PHY control parameter id
 *  @param[in] ParamVal PHY control parameter value
 *  @return error code
 */
UINT32 AmbaVout_FpdLinkSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
    AmbaMisra_TouchUnused(&ParamID);
    AmbaMisra_TouchUnused(&ParamVal);

    return VOUT_ERR_INVALID_API;
}

/**
 *  AmbaVout_FpdLinkGetStatus - Get vout status of FPD-Link interface
 *  @param[out] pFpdLinkConfig Pixel format configuration
 *  @param[out] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_FpdLinkGetStatus(AMBA_VOUT_FPD_LINK_CONFIG_s *pFpdLinkConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    AmbaMisra_TouchUnused(pFpdLinkConfig);
    AmbaMisra_TouchUnused(pDisplayTiming);

    return VOUT_ERR_INVALID_API;
}

/**
 *  AmbaVout_FpdLinkSetCscMatrix - Set color space conversion matrix
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] FpdMode Bit depth information
 *  @return error code
 */
UINT32 AmbaVout_FpdLinkSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 FpdMode)
{
    AmbaMisra_TouchUnused(&pCscMatrix);
    AmbaMisra_TouchUnused(&FpdMode);

    return VOUT_ERR_INVALID_API;
}

/**
 *  AmbaVout_MipiCsiEnable - Configuration VOUT display interface as MIPI CSI-2 output
 *  @param[in] MipiCsiMode MIPI CSI-2 pixel format
 *  @param[in] ColorOrder Color order selection of a pixel
 *  @param[in] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_MipiCsiEnable(UINT32 MipiCsiMode, UINT32 ColorOrder, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((AmbaVout_MipiGetRawMode(MipiCsiMode) >= VOUT_NUM_MIPI_CSI_MODE) ||
        (ColorOrder >= VOUT_NUM_MIPI_CSI_MODE_ORDER) || (pDisplayTiming == NULL) ||
        (pDisplayTiming->PixelClkFreq == 0U) || (pDisplayTiming->PixelClkFreq >= VOUT_MIPI_CSI_MAX_PIXEL_CLOCK)) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiCsiEnable(MipiCsiMode, ColorOrder, pDisplayTiming);
#if defined(CONFIG_VOUT_ASIL)
            (void)RTSL_VoutMipiCsiEnable(MipiCsiMode, ColorOrder, pDisplayTiming);
#endif
            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_MipiCsiSetPhyCtrl - Adjust electrical signal quality of MIPI CSI-2 interface
 *  @param[in] ParamID PHY control parameter id
 *  @param[in] ParamVal PHY control parameter value
 *  @return error code
 */
UINT32 AmbaVout_MipiCsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (ParamID >= VOUT_NUM_PHY_PARAM) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiCsiSetPhyCtrl(ParamID, ParamVal);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_MipiCsiGetStatus - Get vout status of MIPI CSI-2 interface
 *  @param[out] pMipiCsiConfig Pixel format configuration
 *  @param[out] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_MipiCsiGetStatus(AMBA_VOUT_MIPI_CSI_CONFIG_s *pMipiCsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pMipiCsiConfig == NULL) && (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiCsiGetStatus(pMipiCsiConfig, pDisplayTiming);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_MipiCsiSetCscMatrix - Set color space conversion matrix
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] MipiCsiMode Bit depth information
 *  @return error code
 */
UINT32 AmbaVout_MipiCsiSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 MipiCsiMode)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pCscMatrix == NULL) || (MipiCsiMode >= VOUT_NUM_MIPI_CSI_MODE)) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiCsiSetCsc(pCscMatrix, MipiCsiMode);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_MipiDsiEnable - Configuration VOUT display interface as MIPI DSI output
 *  @param[in] MipiDsiMode MIPI DSI pixel format
 *  @param[in] ColorOrder Color order selection of a pixel
 *  @param[in] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiEnable(UINT32 MipiDsiMode, UINT32 ColorOrder, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((AmbaVout_MipiGetRawMode(MipiDsiMode) >= VOUT_NUM_MIPI_DSI_MODE) ||
        (ColorOrder >= VOUT_NUM_MIPI_DSI_MODE_ORDER) || (pDisplayTiming == NULL) ||
        (pDisplayTiming->PixelClkFreq == 0U) || (pDisplayTiming->PixelClkFreq >= VOUT_MIPI_DSI_MAX_PIXEL_CLOCK)) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
#if defined(CONFIG_VOUT_ASIL)
    } else if (AmbaSafety_VoutGetSafeState(0U, NULL) != 0U) {
        RetVal = VOUT_ERR_OSERR;
#endif
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiDsiEnable(MipiDsiMode, ColorOrder, pDisplayTiming);
#if defined(CONFIG_VOUT_ASIL)
            (void)RTSL_VoutMipiDsiEnable(MipiDsiMode, ColorOrder, pDisplayTiming);
#endif

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_MipiDsiSetPhyCtrl - Adjust electrical signal quality of MIPI DSI interface
 *  @param[in] ParamID PHY control parameter id
 *  @param[in] ParamVal PHY control parameter value
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (ParamID >= VOUT_NUM_PHY_PARAM) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiDsiSetPhyCtrl(ParamID, ParamVal);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_MipiDsiGetStatus - Get vout status of MIPI DSI interface
 *  @param[out] pMipiDsiConfig Pixel format configuration
 *  @param[out] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiGetStatus(AMBA_VOUT_MIPI_DSI_CONFIG_s *pMipiDsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pMipiDsiConfig == NULL) && (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiDsiGetStatus(pMipiDsiConfig, pDisplayTiming);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_MipiDsiSetCscMatrix - Set color space conversion matrix
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] MipiDsiMode Bit depth information
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 MipiDsiMode)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pCscMatrix == NULL) || (MipiDsiMode >= VOUT_NUM_MIPI_DSI_MODE)) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiDsiSetCsc(pCscMatrix, MipiDsiMode);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_MipiDsiSendDcsWrite - Send DCS command
 *  @param[in] DcsCmd DCS command ID
 *  @param[in] NumParam DCS command payload size
 *  @param[in] pParam DCS command payload
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiSendDcsWrite(UINT32 DcsCmd, UINT32 NumParam, const UINT8 * pParam)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((NumParam > VOUT_DSI_CMD_MAX_PARAM_NUM) || ((NumParam != 0U) && (pParam == NULL))) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiDsiDcsWrite(DcsCmd, NumParam, pParam);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_MipiDsiSendNormalWrite - Send normal packet
 *  @param[in] NumParam Normal packet payload size
 *  @param[in] pParam Normal packet payload
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiSendNormalWrite(UINT32 NumParam, const UINT8 * pParam)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (NumParam > VOUT_DSI_CMD_MAX_PARAM_NUM) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiDsiNormalWrite(NumParam, pParam);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_MipiDsiSetBlankPkt - Send blank packet
 *  @param[in] pMipiDsiBlankPktCtrl Blank packets control block
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiSetBlankPkt(const AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s * pMipiDsiBlankPktCtrl)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pMipiDsiBlankPktCtrl == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiDsiSetBlankPkt(pMipiDsiBlankPktCtrl);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_MipiDsiGetAckData - Get ack data with BTA
 *  @param[out] pAckData pointer to received ack data
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiGetAckData(UINT32 *pAckData)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pAckData == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            /* Default to enable BTA by RCT */
            RetVal |= AmbaRTSL_VoutMipiDsiSetBtaSrc(MIPI_DSI_BTA_ENABLE_FROM_RCT);
            RetVal |= AmbaRTSL_VoutMipiDsiGetAckData(pAckData);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_GetDisp0Status - Get VOUT display 0 status
 *  @param[out] pStatus Vout status
 *  @param[out] pFreezeDetect Freeze is detected or not
 *  @return error code
 */
UINT32 AmbaVout_GetDisp0Status(UINT32 *pStatus, UINT32 *pFreezeDetect)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pStatus == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal |= AmbaRTSL_Vout0GetStatus(pStatus);
            RetVal |= AmbaRTSL_Vout0CheckFrameFrozen(pFreezeDetect);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_GetDisp1Status - Get VOUT display 1 status
 *  @param[out] pStatus Vout status
 *  @param[out] pFreezeDetect Freeze is detected or not
 *  @return error code
 */
UINT32 AmbaVout_GetDisp1Status(UINT32 *pStatus, UINT32 *pFreezeDetect)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pStatus == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal |= AmbaRTSL_Vout1GetStatus(pStatus);
            RetVal |= AmbaRTSL_Vout1CheckFrameFrozen(pFreezeDetect);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_SetDisp0Timeout - Set VOUT display 0 timeout value
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[in] Timeout Timeout value
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_SetDisp0Timeout(UINT32 Timeout)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (Timeout == 0U) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout0SetTimeout(Timeout);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_SetDisp1Timeout - Set VOUT display 1 timeout value
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[in] Timeout Timeout value
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_SetDisp1Timeout(UINT32 Timeout)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (Timeout == 0U) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout1SetTimeout(Timeout);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_GetTopLevelStatus - Get VOUT top level status
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pStatus Vout status
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetTopLevelStatus(UINT32 *pStatus)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pStatus == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else if (VOUT_DrvInited == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        pMutex = &AmbaVoutMutex;
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutGetTopLevelStatus(pStatus);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_SafetyStart - start vout safety
 *  @param[in] VoutID VOUT id
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_SafetyStart(UINT32 VoutID)
{
#if defined(CONFIG_VOUT_ASIL)
    return AmbaSafety_VoutInit(VoutID);
#else
    (void) VoutID;
    return VOUT_ERR_UNEXPECTED;
#endif
}

/**
 *  AmbaVout_SafetyStop - stop vout safety
 *  @param[in] VoutID VOUT id
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_SafetyStop(UINT32 VoutID)
{
#if defined(CONFIG_VOUT_ASIL)
    return AmbaSafety_VoutDeInit(VoutID);
#else
    (void) VoutID;
    return VOUT_ERR_UNEXPECTED;
#endif
}
