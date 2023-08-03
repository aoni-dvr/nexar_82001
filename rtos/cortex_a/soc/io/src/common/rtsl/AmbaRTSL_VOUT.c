/*
 * Copyright (c) 2021 Ambarella International LP
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

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"

#include "AmbaRTSL_VOUT.h"
#include "AmbaCSL_VOUT.h"

#if defined(CONFIG_QNX)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "hw/ambarella_clk.h"
#include "AmbaSYS.h"
#else
#include "AmbaRTSL_PLL.h"
#endif

#define VOUT_REG_MAX_VALUE   (UINT16)0x3fff  /* 14 bits for vout timing paremeters */
#define VOUT_DISPLAY_BUF_SIZE   128U
#define VOUT_TVENC_BUF_SIZE     128U

/************** cv28 **************/
#if defined(CONFIG_SOC_CV28)
static AMBA_VOUT_DISPLAY0_CONFIG_REG_s AmbaVOUT_Display0Config __attribute__((aligned(128)));
static UINT32 AmbaVOUT_TveConfigData[VOUT_TVENC_BUF_SIZE] __attribute__((aligned(128)));

static AMBA_VOUT_DISPLAY0_CONFIG_REG_s* const pAmbaVOUT_DispCfg = &AmbaVOUT_Display0Config;  /* Ucode buffer pointer for FPD/DSI APIs */
static AMBA_VOUT_DISPLAY0_CONFIG_REG_s* const pAmbaVOUT_DispCfg_CSI  = &AmbaVOUT_Display0Config;
static AMBA_VOUT_DISPLAY0_CONFIG_REG_s* const pAmbaVOUT_DispCfg_CVBS = &AmbaVOUT_Display0Config;

/************** cv5x **************/
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static AMBA_VOUT_DISPLAY_CONFIG_REG_s AmbaVOUT_Display0Config __attribute__((aligned(128)));
static AMBA_VOUT_DISPLAY_CONFIG_REG_s AmbaVOUT_Display1Config __attribute__((aligned(128)));
static AMBA_VOUT_DISPLAY_C_CONFIG_REG_s AmbaVOUT_Display2Config __attribute__((aligned(128)));
static UINT32 AmbaVOUT_TveConfigData[VOUT_TVENC_BUF_SIZE] __attribute__((aligned(128)));

static AMBA_VOUT_DISPLAY_CONFIG_REG_s* pAmbaVOUT_DispCfg = &AmbaVOUT_Display0Config;  /* Ucode buffer pointer for DSI APIs, set default point to Vout0 */
static AMBA_VOUT_DISPLAY_CONFIG_REG_s* pAmbaVOUT_DispCfg_CSI = &AmbaVOUT_Display0Config;  /* Ucode buffer pointer for CSI APIs, set default point to Vout0 */
static AMBA_VOUT_DISPLAY_C_CONFIG_REG_s* pAmbaVOUT_DispCfg_CVBS = &AmbaVOUT_Display2Config;
static AMBA_VOUT_DISPLAY_C_CONFIG_REG_s* pAmbaVOUT_DispCfg_HDMI = &AmbaVOUT_Display2Config;

static UINT32 VoutChan = 0U;

/************** cv2x **************/
#else
#if defined(CONFIG_QNX)
AMBA_VOUT_DISPLAY0_CONFIG_REG_s AmbaVOUT_Display0Config;
AMBA_VOUT_DISPLAY1_CONFIG_REG_s AmbaVOUT_Display1Config;
UINT32 AmbaVOUT_TveConfigData[VOUT_TVENC_BUF_SIZE];
#else
static AMBA_VOUT_DISPLAY0_CONFIG_REG_s AmbaVOUT_Display0Config __attribute__((aligned(128)));
static AMBA_VOUT_DISPLAY1_CONFIG_REG_s AmbaVOUT_Display1Config __attribute__((aligned(128)));
static UINT32 AmbaVOUT_TveConfigData[VOUT_TVENC_BUF_SIZE] __attribute__((aligned(128)));
#endif
static AMBA_VOUT_DISPLAY0_CONFIG_REG_s* const pAmbaVOUT_DispCfg = &AmbaVOUT_Display0Config;  /* Ucode buffer pointer for Digital/FPD/DSI APIs */
#if !defined(CONFIG_SOC_H32)
static AMBA_VOUT_DISPLAY1_CONFIG_REG_s* const pAmbaVOUT_DispCfg_CSI = &AmbaVOUT_Display1Config;
#endif
static AMBA_VOUT_DISPLAY1_CONFIG_REG_s* const pAmbaVOUT_DispCfg_CVBS = &AmbaVOUT_Display1Config;
static AMBA_VOUT_DISPLAY1_CONFIG_REG_s* const pAmbaVOUT_DispCfg_HDMI = &AmbaVOUT_Display1Config;
#endif
/**********************************/


/**
 *  AmbaRTSL_VoutInitTvEncConfig - Zero initialize vout dve section
 *  @return error code
 */
UINT32 AmbaRTSL_VoutInitTvEncConfig(void)
{
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(AmbaVOUT_TveConfigData, 0, sizeof(AmbaVOUT_TveConfigData));
    if (RetVal != 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        RetVal = VOUT_ERR_NONE;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout0InitDispConfig - Zero initialize vout display section
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0InitDispConfig(void)
{
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(&AmbaVOUT_Display0Config, 0, sizeof(AmbaVOUT_Display0Config));
    if (RetVal != 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        RetVal = VOUT_ERR_NONE;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout0GetDispConfigAddr - Get display configuration parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0GetDispConfigAddr(ULONG *pVirtAddr)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    const AMBA_VOUT_DISPLAY_CONFIG_REG_s *pVout0DispCfg;
#else
    const AMBA_VOUT_DISPLAY0_CONFIG_REG_s *pVout0DispCfg;
#endif
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pVout0DispCfg = &AmbaVOUT_Display0Config;
        AmbaMisra_TypeCast(&VirtAddr, &pVout0DispCfg);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout0GetDigitalCscAddr - Get digital csc parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0GetDigitalCscAddr(ULONG *pVirtAddr)
{
    volatile const AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s *pCscMatrix;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pCscMatrix = &AmbaVOUT_Display0Config.DigitalCSC0;
        AmbaMisra_TypeCast(&VirtAddr, &pCscMatrix);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout0GetAnalogCscAddr - Get analog csc parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0GetAnalogCscAddr(ULONG *pVirtAddr)
{
#if defined(CONFIG_SOC_CV28)
    volatile const AMBA_VOUTD_ANALOG_CSC_PARAM_0_REG_s *pCscMatrix;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pCscMatrix = &AmbaVOUT_Display0Config.AnalogCSC0;
        AmbaMisra_TypeCast(&VirtAddr, &pCscMatrix);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
#else
    /* Not supported API */
    *pVirtAddr = 0U;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaRTSL_Vout0GetTvEncAddr - Get tv encoder configuration parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0GetTvEncAddr(ULONG *pVirtAddr)
{
#if defined(CONFIG_SOC_CV28)
    const UINT32 *pTvEncCfg;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pTvEncCfg = AmbaVOUT_TveConfigData;
        AmbaMisra_TypeCast(&VirtAddr, &pTvEncCfg);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
#else
    /* Not supported API */
    *pVirtAddr = 0U;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaRTSL_Vout0SetVinVoutSync - Enable/Disable vin-vout sync
 *  @param[in] EnableFlag Enable flag
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0SetVinVoutSync(UINT32 EnableFlag)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (EnableFlag > 1U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        AmbaVOUT_Display0Config.DispCtrl.VinVoutSync = (UINT8)EnableFlag;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout0GetStatus - Get vout0 status
 *  @param[out] pStatus Vout0 status
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0GetStatus(UINT32 *pStatus)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    *pStatus = AmbaCSL_Vout0GetStatus();

    return RetVal;
}

/**
 *  AmbaRTSL_Vout1InitDispConfig - Zero initialize vout display section
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1InitDispConfig(void)
{
#if !defined(CONFIG_SOC_CV28)
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(&AmbaVOUT_Display1Config, 0, sizeof(AmbaVOUT_Display1Config));
    if (RetVal != 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        RetVal = VOUT_ERR_NONE;
    }

    return RetVal;
#else
    /* Not supported API */
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaRTSL_Vout1GetDispConfigAddr - Get display configuration parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1GetDispConfigAddr(ULONG *pVirtAddr)
{
#if !defined(CONFIG_SOC_CV28)
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    const AMBA_VOUT_DISPLAY_CONFIG_REG_s *pVout1DispCfg;
#else
    const AMBA_VOUT_DISPLAY1_CONFIG_REG_s *pVout1DispCfg;
#endif
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pVout1DispCfg = &AmbaVOUT_Display1Config;
        AmbaMisra_TypeCast(&VirtAddr, &pVout1DispCfg);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
#else
    /* Not supported API */
    *pVirtAddr = 0U;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaRTSL_Vout1GetDigitalCscAddr - Get digital csc parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1GetDigitalCscAddr(ULONG *pVirtAddr)
{
#if !(defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32))
    volatile const AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s *pCscMatrix;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pCscMatrix = &AmbaVOUT_Display1Config.DigitalCSC0;
        AmbaMisra_TypeCast(&VirtAddr, &pCscMatrix);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
#else
    /* Not supported API */
    *pVirtAddr = 0U;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaRTSL_Vout1GetAnalogCscAddr - Get analog csc parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1GetAnalogCscAddr(ULONG *pVirtAddr)
{
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    volatile const AMBA_VOUTD_ANALOG_CSC_PARAM_0_REG_s *pCscMatrix;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pCscMatrix = &AmbaVOUT_Display1Config.AnalogCSC0;
        AmbaMisra_TypeCast(&VirtAddr, &pCscMatrix);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
#else
    /* Not supported API */
    *pVirtAddr = 0U;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaRTSL_Vout1GetHdmiCscAddr - Get hdmi csc parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1GetHdmiCscAddr(ULONG *pVirtAddr)
{
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    volatile const AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s *pCscMatrix;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pCscMatrix = &AmbaVOUT_Display1Config.HdmiCSC0;
        AmbaMisra_TypeCast(&VirtAddr, &pCscMatrix);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
#else
    /* Not supported API */
    *pVirtAddr = 0U;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaRTSL_Vout1GetTvEncAddr - Get tv encoder configuration parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1GetTvEncAddr(ULONG *pVirtAddr)
{
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    const UINT32 *pTvEncCfg;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pTvEncCfg = AmbaVOUT_TveConfigData;
        AmbaMisra_TypeCast(&VirtAddr, &pTvEncCfg);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
#else
    /* Not supported API */
    *pVirtAddr = 0U;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaRTSL_Vout1SetVinVoutSync - Enable/Disable vin-vout sync
 *  @param[in] EnableFlag Enable flag
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1SetVinVoutSync(UINT32 EnableFlag)
{
#if !defined(CONFIG_SOC_CV28)
    UINT32 RetVal = VOUT_ERR_NONE;

    if (EnableFlag > 1U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        AmbaVOUT_Display1Config.DispCtrl.VinVoutSync = (UINT8)EnableFlag;
    }

    return RetVal;
#else
    /* Not supported API */
    (void)EnableFlag;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaRTSL_Vout1GetStatus - Get vout1 status
 *  @param[out] pStatus Vout1 status
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1GetStatus(UINT32 *pStatus)
{
#if !defined(CONFIG_SOC_CV28)
    UINT32 RetVal = VOUT_ERR_NONE;

    *pStatus = AmbaCSL_Vout1GetStatus();

    return RetVal;
#else
    /* Not supported API */
    *pStatus = 0U;
    return VOUT_ERR_INVALID_API;
#endif
}

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
/**
 *  AmbaRTSL_VoutChannelSelect - Select vout display channel
 *  @param[in] VoutChannel Target vout channel
 *  @return error code
 */
UINT32 AmbaRTSL_VoutChannelSelect(UINT32 VoutChannel)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    VoutChan = VoutChannel;

    if (VoutChan == AMBA_VOUT_CHANNEL0) {
        pAmbaVOUT_DispCfg = &AmbaVOUT_Display0Config;
        pAmbaVOUT_DispCfg_CSI = &AmbaVOUT_Display0Config;
    } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
        pAmbaVOUT_DispCfg = &AmbaVOUT_Display1Config;
        pAmbaVOUT_DispCfg_CSI = &AmbaVOUT_Display1Config;
    } else {
        RetVal = VOUT_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout2InitDispConfig - Zero initialize vout display section
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2InitDispConfig(void)
{
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(&AmbaVOUT_Display2Config, 0, sizeof(AmbaVOUT_Display2Config));
    if (RetVal != 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        RetVal = VOUT_ERR_NONE;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout2GetDispConfigAddr - Get display configuration parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2GetDispConfigAddr(ULONG *pVirtAddr)
{
    const AMBA_VOUT_DISPLAY_C_CONFIG_REG_s *pVout2DispCfg;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pVout2DispCfg = &AmbaVOUT_Display2Config;
        AmbaMisra_TypeCast(&VirtAddr, &pVout2DispCfg);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout2GetAnalogCscAddr - Get analog csc parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2GetAnalogCscAddr(ULONG *pVirtAddr)
{
    volatile const AMBA_VOUTD_ANALOG_CSC_PARAM_0_REG_s *pCscMatrix;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pCscMatrix = &AmbaVOUT_Display2Config.AnalogCSC0;
        AmbaMisra_TypeCast(&VirtAddr, &pCscMatrix);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout2GetHdmiCscAddr - Get hdmi csc parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2GetHdmiCscAddr(ULONG *pVirtAddr)
{
    volatile const AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s *pCscMatrix;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pCscMatrix = &AmbaVOUT_Display2Config.HdmiCSC0;
        AmbaMisra_TypeCast(&VirtAddr, &pCscMatrix);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout2GetTvEncAddr - Get tv encoder configuration parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2GetTvEncAddr(ULONG *pVirtAddr)
{
    const UINT32 *pTvEncCfg;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pTvEncCfg = AmbaVOUT_TveConfigData;
        AmbaMisra_TypeCast(&VirtAddr, &pTvEncCfg);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout2SetVinVoutSync - Enable/Disable vin-vout sync
 *  @param[in] EnableFlag Enable flag
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2SetVinVoutSync(UINT32 EnableFlag)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (EnableFlag > 1U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        AmbaVOUT_Display2Config.DispCtrl.VinVoutSyncEnable = (UINT8)EnableFlag;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout2GetStatus - Get vout2 status
 *  @param[out] pStatus Vout2 status
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2GetStatus(UINT32 *pStatus)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    *pStatus = AmbaCSL_Vout2GetStatus();

    return RetVal;
}

/**
 *  AmbaRTSL_Vout0SetTimeout - Set Timeout for vout0
 *  @param[in] Timeout Timeout value
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0SetTimeout(UINT32 Timeout)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    AmbaCSL_Vout0SetTimeout(Timeout);

    return RetVal;
}

/**
 *  AmbaRTSL_Vout1SetTimeout - Set Timeout for vout1
 *  @param[in] Timeout Timeout value
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1SetTimeout(UINT32 Timeout)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    AmbaCSL_Vout1SetTimeout(Timeout);

    return RetVal;
}

/**
 *  AmbaRTSL_Vout2SetTimeout - Set Timeout for vout2
 *  @param[in] Timeout Timeout value
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2SetTimeout(UINT32 Timeout)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    AmbaCSL_Vout2SetTimeout(Timeout);

    return RetVal;
}
#endif

/**
 *  VOUT_SetDigitalCsc - Assign color space conversion parameters for digital/DSI vout interface
 *  @param[in] pVoutCscData Color space conversion data
 */
static void VOUT_SetDigitalCsc(const UINT16 *pVoutCscData)
{
    pAmbaVOUT_DispCfg->DigitalCSC0.CoefA0 = pVoutCscData[0];
    pAmbaVOUT_DispCfg->DigitalCSC0.CoefA1 = pVoutCscData[1];
    pAmbaVOUT_DispCfg->DigitalCSC1.CoefA2 = pVoutCscData[2];
    pAmbaVOUT_DispCfg->DigitalCSC1.CoefA3 = pVoutCscData[3];
    pAmbaVOUT_DispCfg->DigitalCSC2.CoefA4 = pVoutCscData[4];
    pAmbaVOUT_DispCfg->DigitalCSC2.CoefA5 = pVoutCscData[5];
    pAmbaVOUT_DispCfg->DigitalCSC3.CoefA6 = pVoutCscData[6];
    pAmbaVOUT_DispCfg->DigitalCSC3.CoefA7 = pVoutCscData[7];
    pAmbaVOUT_DispCfg->DigitalCSC4.CoefA8 = pVoutCscData[8];
    pAmbaVOUT_DispCfg->DigitalCSC4.ConstB0 = pVoutCscData[9];
    pAmbaVOUT_DispCfg->DigitalCSC5.ConstB1 = pVoutCscData[10];
    pAmbaVOUT_DispCfg->DigitalCSC5.ConstB2 = pVoutCscData[11];
    pAmbaVOUT_DispCfg->DigitalCSC6.Output0ClampLow  = pVoutCscData[12];
    pAmbaVOUT_DispCfg->DigitalCSC6.Output0ClampHigh = pVoutCscData[13];
    pAmbaVOUT_DispCfg->DigitalCSC7.Output1ClampLow  = pVoutCscData[14];
    pAmbaVOUT_DispCfg->DigitalCSC7.Output1ClampHigh = pVoutCscData[15];
    pAmbaVOUT_DispCfg->DigitalCSC8.Output2ClampLow  = pVoutCscData[16];
    pAmbaVOUT_DispCfg->DigitalCSC8.Output2ClampHigh = pVoutCscData[17];
}

/**
 *  VOUT_GetDisplayTiming - Get video timing configuration for Digital/DSI interface
 *  @param[in] pDisplayTiming Video timing parameters
 */
static void VOUT_GetDisplayTiming(AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    const AMBA_VOUT_DISPLAY_CONFIG_REG_s * pReg = pAmbaVoutDisplay0_Reg;
#else
    const AMBA_VOUT_DISPLAY0_CONFIG_REG_s *const pReg = pAmbaVoutDisplay0_Reg;
#endif
    UINT32 FrameActiveColStart, FrameActiveColWidth, FrameActiveRowStart, FrameActiveRowHeight;
    UINT32 FrameHeight; /* V-total */

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    if (VoutChan == AMBA_VOUT_CHANNEL0) {
        pReg = pAmbaVoutDisplay0_Reg;
        pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutAClk();
    } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
        pReg = pAmbaVoutDisplay1_Reg;
        pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutBClk();
    } else {
        /* Do nothing */
    }
#else
#if defined(CONFIG_QNX)
    (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_VOUTLCD, &pDisplayTiming->PixelClkFreq);
#else
    pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutLcdClk();
#endif
#endif
    /* Get Vout Display0 timing from Vout registers */
    if (pReg->DispCtrl.Interlaced == 0U) {
        /* Progressive scan */
        pDisplayTiming->DisplayMethod = 0U;
        FrameHeight = pReg->DispFrmSize.FrameHeightFld0 + (UINT32)1U;
    } else {
        /* Interlaced scan */
        pDisplayTiming->DisplayMethod = 1U;
        FrameHeight = (pReg->DispFrmSize.FrameHeightFld0 + (UINT32)1U) << 1U;
    }

    FrameActiveColStart = pReg->DispTopActiveStart.StartColumn;
    FrameActiveColWidth = (pReg->DispTopActiveEnd.EndColumn + (UINT32)1U) - FrameActiveColStart;

    FrameActiveRowStart = pReg->DispTopActiveStart.StartRow;
    FrameActiveRowHeight = (pReg->DispTopActiveEnd.EndRow + (UINT32)1U) - FrameActiveRowStart;

    pDisplayTiming->ActiveLines = (UINT16)FrameActiveRowHeight;
    pDisplayTiming->ActivePixels = (UINT16)FrameActiveColWidth;

    pDisplayTiming->HsyncPulseWidth  = pReg->DigitalHSync.EndColumn;
    pDisplayTiming->HsyncPulseWidth -= pReg->DigitalHSync.StartColumn;
    pDisplayTiming->VsyncPulseWidth  = pReg->DigitalVSyncTopEnd.EndRow;
    pDisplayTiming->VsyncPulseWidth -= pReg->DigitalVSyncTopStart.StartRow;

    pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - pReg->DigitalHSync.EndColumn);
    pDisplayTiming->HsyncFrontPorch = (UINT16)(pReg->DigitalHSync.StartColumn);

    pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - pDisplayTiming->VsyncPulseWidth);
    pDisplayTiming->VsyncFrontPorch = (UINT16)(FrameHeight - (FrameActiveRowStart + FrameActiveRowHeight));
}

#if (!defined(CONFIG_SOC_CV5) && !defined(CONFIG_SOC_CV52))
/**
 *  VOUT_SetDisplayTiming - Assign video timing parameters for display devices (Digital / FPD-link only)
 *  @param[in] pDisplayTiming Video timing parameters
 */
static void VOUT_SetDisplayTiming(const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT16 Htotal, Vtotal, RegVal;

    Htotal = pDisplayTiming->HsyncFrontPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels;
    if ( pDisplayTiming->DisplayMethod == 0U) {
        Vtotal = pDisplayTiming->VsyncFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines;
    } else {
        Vtotal = pDisplayTiming->VsyncFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch;
        Vtotal *= 2U;
        Vtotal = Vtotal + 1U + pDisplayTiming->ActiveLines;
    }

    /* Fill up the video timing */
    RegVal = Htotal - 1U;
    pAmbaVOUT_DispCfg->DispFrmSize.FrameWidth = RegVal;

    pAmbaVOUT_DispCfg->DigitalHSync.StartColumn = 0U;

    RegVal = pAmbaVOUT_DispCfg->DigitalHSync.StartColumn + pDisplayTiming->HsyncPulseWidth;
    pAmbaVOUT_DispCfg->DigitalHSync.EndColumn = RegVal;

    if (pDisplayTiming->DisplayMethod == 0U) {
        /* 0 = Progressive scan */
        RegVal = Vtotal - 1U;
        pAmbaVOUT_DispCfg->DispFrmSize.FrameHeightFld0 = RegVal;

        RegVal = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch;
        pAmbaVOUT_DispCfg->DispTopActiveStart.StartColumn = RegVal;

        RegVal = pDisplayTiming->VsyncPulseWidth + pDisplayTiming ->VsyncBackPorch;
        pAmbaVOUT_DispCfg->DispTopActiveStart.StartRow = RegVal;

        RegVal = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels - 1U;
        pAmbaVOUT_DispCfg->DispTopActiveEnd.EndColumn = RegVal;

        RegVal = pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines - 1U;
        pAmbaVOUT_DispCfg->DispTopActiveEnd.EndRow = RegVal;

        /* Sync */
        pAmbaVOUT_DispCfg->DigitalVSyncTopStart.StartColumn = pAmbaVOUT_DispCfg->DigitalHSync.StartColumn;
        pAmbaVOUT_DispCfg->DigitalVSyncTopStart.StartRow = 0U;
        pAmbaVOUT_DispCfg->DigitalVSyncTopEnd.EndColumn = pAmbaVOUT_DispCfg->DigitalHSync.StartColumn;

        RegVal = pAmbaVOUT_DispCfg->DigitalVSyncTopStart.StartRow + pDisplayTiming->VsyncPulseWidth;
        pAmbaVOUT_DispCfg->DigitalVSyncTopEnd.EndRow = RegVal;

        pAmbaVOUT_DispCfg->DispCtrl.Interlaced = 0U;
    } else {
        /* 1 = Interlaced scan */
        RegVal = (Vtotal >> 1U) - 1U;
        pAmbaVOUT_DispCfg->DispFrmSize.FrameHeightFld0 = RegVal;

        RegVal = pAmbaVOUT_DispCfg->DispFrmSize.FrameHeightFld0 + 1U;
        pAmbaVOUT_DispCfg->DispFldHeight.FrameHeightFld1 = RegVal;

        RegVal = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch;
        pAmbaVOUT_DispCfg->DispTopActiveStart.StartColumn = RegVal;

        RegVal = pDisplayTiming->VsyncPulseWidth + pDisplayTiming ->VsyncBackPorch;
        pAmbaVOUT_DispCfg->DispTopActiveStart.StartRow = RegVal;

        RegVal = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels - 1U;
        pAmbaVOUT_DispCfg->DispTopActiveEnd.EndColumn = RegVal;

        RegVal = pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + (pDisplayTiming->ActiveLines >> 1U) - 1U;
        pAmbaVOUT_DispCfg->DispTopActiveEnd.EndRow = RegVal;

        pAmbaVOUT_DispCfg->DispBtmActiveStart.StartColumn = pAmbaVOUT_DispCfg->DispTopActiveStart.StartColumn;

        RegVal = pAmbaVOUT_DispCfg->DispTopActiveStart.StartRow + 1U;
        pAmbaVOUT_DispCfg->DispBtmActiveStart.StartRow = RegVal;

        pAmbaVOUT_DispCfg->DispBtmActiveEnd.EndColumn = pAmbaVOUT_DispCfg->DispTopActiveEnd.EndColumn;

        RegVal = pAmbaVOUT_DispCfg->DispTopActiveEnd.EndRow + 1U;
        pAmbaVOUT_DispCfg->DispBtmActiveEnd.EndRow = RegVal;

        /* Sync */
        pAmbaVOUT_DispCfg->DigitalVSyncTopStart.StartColumn = pAmbaVOUT_DispCfg->DigitalHSync.StartColumn;
        pAmbaVOUT_DispCfg->DigitalVSyncTopStart.StartRow = 0U;
        pAmbaVOUT_DispCfg->DigitalVSyncTopEnd.EndColumn = pAmbaVOUT_DispCfg->DigitalVSyncTopStart.StartColumn;

        RegVal = pAmbaVOUT_DispCfg->DigitalVSyncTopStart.StartRow + pDisplayTiming->VsyncPulseWidth;
        pAmbaVOUT_DispCfg->DigitalVSyncTopEnd.EndRow = RegVal;

        RegVal = pAmbaVOUT_DispCfg->DigitalHSync.StartColumn + (Htotal >> 1U);
        pAmbaVOUT_DispCfg->DigitalVSyncBtmStart.StartColumn = RegVal;

        pAmbaVOUT_DispCfg->DigitalVSyncBtmStart.StartRow = 0U;
        pAmbaVOUT_DispCfg->DigitalVSyncBtmEnd.EndColumn = pAmbaVOUT_DispCfg->DigitalVSyncBtmStart.StartColumn;

        RegVal = pAmbaVOUT_DispCfg->DigitalVSyncBtmStart.StartRow + pDisplayTiming->VsyncPulseWidth;
        pAmbaVOUT_DispCfg->DigitalVSyncBtmEnd.EndRow = RegVal;

        pAmbaVOUT_DispCfg->DispCtrl.Interlaced = 1U;
    }
}
#endif
/************************************
    Digital(LVCMOS) APIs
*************************************/
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
/**
 *  AmbaRTSL_VoutDigiYccEnable - Enable YCbCr digital video interface display
 *  @param[in] YccMode YCbCr pixel format
 *  @param[in] ColorOrder Color component order of a pixel
 *  @param[in] pDataLatch Data latch signal polarity
 *  @param[in] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutDigiYccEnable(UINT32 YccMode, UINT32 ColorOrder, const AMBA_VOUT_DATA_LATCH_CONFIG_s *pDataLatch, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pDisplayTiming == NULL) {
        RetVal =  VOUT_ERR_ARG;
    } else {
        /* Fill up the control */
        pAmbaVOUT_DispCfg->DispCtrl.FlipMode = 0U;
        pAmbaVOUT_DispCfg->DispCtrl.DigitalOutput = 1U;
        pAmbaVOUT_DispCfg->DispCtrl.FixedFormatSelect = 0U;

        /* According to Vout output mode definition */
        if (YccMode == VOUT_YCC_MODE_BT601_24BIT) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.OutputMode = 6U;
        } else if (YccMode == VOUT_YCC_MODE_BT601_16BIT) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.OutputMode = 5U;
        } else if (YccMode == VOUT_YCC_MODE_BT601_8BIT) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.OutputMode = 7U;
        } else {
            /* 656 mode */
            pAmbaVOUT_DispCfg->DigitalOutputMode.OutputMode = 4U;

            /* (TBD) SAV/EAV Vbit */
        }

        if (pDataLatch->ExtLineSyncPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.HSyncPolarity = 0U;
        } else {
            pAmbaVOUT_DispCfg->DigitalOutputMode.HSyncPolarity = 1U;
        }
        if (pDataLatch->ExtFrameSyncPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.VSyncPolarity = 0U;
        } else {
            pAmbaVOUT_DispCfg->DigitalOutputMode.VSyncPolarity = 1U;
        }
        if (pDataLatch->ExtDataValidPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.HvldPolarity = 0U;
        } else {
            pAmbaVOUT_DispCfg->DigitalOutputMode.HvldPolarity = 1U;
        }
        if (pDataLatch->ExtClkSampleEdge == VOUT_SIGNAL_EDGE_LOW_TO_HIGH) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.ClkSampleEdge = 0U;
        } else {
            pAmbaVOUT_DispCfg->DigitalOutputMode.ClkSampleEdge = 1U;
        }

        /* Set color order */
        pAmbaVOUT_DispCfg->DigitalOutputMode.ColorSeqEvenLines = 0U;
        pAmbaVOUT_DispCfg->DigitalOutputMode.ColorSeqOddLines = 0U;
        AmbaMisra_TouchUnused(&ColorOrder);

        /* Fill up the video timing */
        VOUT_SetDisplayTiming(pDisplayTiming);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutDigiYccSetPhyCtrl - Set PHY control parameter
 *  @param[in] ParamID PHY parameter ID
 *  @param[in] ParamVal PHY parameter value
 *  @return error code
 */
UINT32 AmbaRTSL_VoutDigiYccSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    /* TODO: */
    (void)ParamID;
    (void)ParamVal;

    return RetVal;
}

/**
 *  AmbaRTSL_VoutDigiYccGetStatus - Get YCbCr digital video interface related configurations
 *  @param[out] pDigiYccConfig Digital YCbCr-specific vout configuration
 *  @param[out] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutDigiYccGetStatus(AMBA_VOUT_DIGITAL_YCC_CONFIG_s *pDigiYccConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    AMBA_VOUTD_DIGITAL_OUTPUT_MODE_REG_s DigitalOutputMode;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pDigiYccConfig != NULL) {
        DigitalOutputMode = pAmbaVoutDisplay0_Reg->DigitalOutputMode;
        if (DigitalOutputMode.OutputMode == 6U) {
            pDigiYccConfig->YccMode = VOUT_YCC_MODE_BT601_24BIT;
        } else if (DigitalOutputMode.OutputMode == 5U) {
            pDigiYccConfig->YccMode = VOUT_YCC_MODE_BT601_16BIT;
        } else if (DigitalOutputMode.OutputMode == 7U) {
            pDigiYccConfig->YccMode = VOUT_YCC_MODE_BT601_8BIT;
        } else if (DigitalOutputMode.OutputMode == 4U) {
            pDigiYccConfig->YccMode = VOUT_YCC_MODE_BT656_16BIT;
        } else {
            RetVal = VOUT_ERR_PROTOCOL;
        }

        if (RetVal == VOUT_ERR_NONE) {
            pDigiYccConfig->ColorOrder = 0U;

            if (DigitalOutputMode.HSyncPolarity == 0U) {
                pDigiYccConfig->ExtLineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
            } else {
                pDigiYccConfig->ExtLineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
            }
            if (DigitalOutputMode.VSyncPolarity == 0U) {
                pDigiYccConfig->ExtFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
            } else {
                pDigiYccConfig->ExtFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
            }
            if (DigitalOutputMode.HvldPolarity == 0U) {
                pDigiYccConfig->ExtDataValidPolarity = VOUT_SIGNAL_ACTIVE_LOW;
            } else {
                pDigiYccConfig->ExtDataValidPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
            }
            if (DigitalOutputMode.ClkSampleEdge == 0U) {
                pDigiYccConfig->ExtClkSampleEdge = VOUT_SIGNAL_EDGE_LOW_TO_HIGH;
            } else {
                pDigiYccConfig->ExtClkSampleEdge = VOUT_SIGNAL_EDGE_HIGH_TO_LOW;
            }
        }
    }

    if (pDisplayTiming != NULL) {
        VOUT_GetDisplayTiming(pDisplayTiming);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutDigiYccSetCsc - Set CSC matrix as register format
 *  @param[in] pVoutCscData Color space conversion data
 *  @param[in] YccMode Digital YCbCr mode for reference
 *  @return error code
 */
UINT32 AmbaRTSL_VoutDigiYccSetCsc(const UINT16 *pVoutCscData, UINT32 YccMode)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    VOUT_SetDigitalCsc(pVoutCscData);
    AmbaMisra_TouchUnused(&YccMode);

    return RetVal;
}

/************************************
    Digital RGB APIs
*************************************/
/**
 *  AmbaRTSL_VoutDigiRgbEnable - Enable RGB video interface display
 *  @param[in] RgbMode RGB pixel format
 *  @param[in] ColorOrder Color component order of a pixel
 *  @param[in] pDataLatch Data latch signal polarity
 *  @param[in] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutDigiRgbEnable(UINT32 RgbMode, UINT32 ColorOrder, const AMBA_VOUT_DATA_LATCH_CONFIG_s *pDataLatch, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pDisplayTiming == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        /* Fill up the control */
        pAmbaVOUT_DispCfg->DispCtrl.FlipMode = 0U;
        pAmbaVOUT_DispCfg->DispCtrl.DigitalOutput = 1U;
        pAmbaVOUT_DispCfg->DispCtrl.FixedFormatSelect = 0U;

        /* (TBD) */
        pAmbaVOUT_DispCfg->DigitalOutputMode.ClkDisable = 0U;
        pAmbaVOUT_DispCfg->DigitalOutputMode.ClkOutputDivider = 0U;
        pAmbaVOUT_DispCfg->DigitalOutputMode.ClkDividerEnable = 0U;

        /* According to Vout output mode definition */
        if (RgbMode == VOUT_RGB_MODE_888_SINGLE) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.OutputMode = 0U;
        } else if (RgbMode == VOUT_RGB_MODE_888) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.OutputMode = 1U;
        } else if (RgbMode == VOUT_RGB_MODE_888_DUMMY) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.OutputMode = 2U;
        } else {
            /* 565 mode */
            pAmbaVOUT_DispCfg->DigitalOutputMode.OutputMode = 3U;
        }

        if (pDataLatch->ExtLineSyncPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.HSyncPolarity = 0U;
        } else {
            pAmbaVOUT_DispCfg->DigitalOutputMode.HSyncPolarity = 1U;
        }
        if (pDataLatch->ExtFrameSyncPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.VSyncPolarity = 0U;
        } else {
            pAmbaVOUT_DispCfg->DigitalOutputMode.VSyncPolarity = 1U;
        }
        if (pDataLatch->ExtDataValidPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.HvldPolarity = 0U;
        } else {
            pAmbaVOUT_DispCfg->DigitalOutputMode.HvldPolarity = 1U;
        }
        if (pDataLatch->ExtClkSampleEdge == VOUT_SIGNAL_EDGE_LOW_TO_HIGH) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.ClkSampleEdge = 0U;
        } else {
            pAmbaVOUT_DispCfg->DigitalOutputMode.ClkSampleEdge = 1U;
        }

        /* Set color order */
        pAmbaVOUT_DispCfg->DigitalOutputMode.ColorSeqEvenLines = (UINT8)(ColorOrder & 0xffffU);
        pAmbaVOUT_DispCfg->DigitalOutputMode.ColorSeqOddLines = (UINT8)(ColorOrder >> 16);

        /* Fill up the video timing */
        VOUT_SetDisplayTiming(pDisplayTiming);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutDigiRgbSetPhyCtrl - Set PHY control parameter
 *  @param[in] ParamID PHY parameter ID
 *  @param[in] ParamVal PHY parameter value
 *  @return error code
 */
UINT32 AmbaRTSL_VoutDigiRgbSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    /* TODO: */
    (void)ParamID;
    (void)ParamVal;

    return RetVal;
}

/**
 *  AmbaRTSL_VoutDigiRgbGetStatus - Get RGB digital video interface related configurations
 *  @param[out] pDigiRgbConfig Digital RGB-specific vout configuration
 *  @param[out] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutDigiRgbGetStatus(AMBA_VOUT_DIGITAL_RGB_CONFIG_s *pDigiRgbConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    AMBA_VOUTD_DIGITAL_OUTPUT_MODE_REG_s DigitalOutputMode;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pDigiRgbConfig != NULL) {
        DigitalOutputMode = pAmbaVoutDisplay0_Reg->DigitalOutputMode;
        if (DigitalOutputMode.OutputMode == 0U) {
            pDigiRgbConfig->RgbMode = VOUT_RGB_MODE_888_SINGLE;
        } else if (DigitalOutputMode.OutputMode == 1U) {
            pDigiRgbConfig->RgbMode = VOUT_RGB_MODE_888;
        } else if (DigitalOutputMode.OutputMode == 2U) {
            pDigiRgbConfig->RgbMode = VOUT_RGB_MODE_888_DUMMY;
        } else if (DigitalOutputMode.OutputMode == 3U) {
            pDigiRgbConfig->RgbMode = VOUT_RGB_MODE_565;
        } else {
            RetVal = VOUT_ERR_PROTOCOL;
        }

        if (RetVal == VOUT_ERR_NONE) {
            pDigiRgbConfig->ColorOrder = DigitalOutputMode.ColorSeqOddLines;
            pDigiRgbConfig->ColorOrder <<= 16U;
            pDigiRgbConfig->ColorOrder |= DigitalOutputMode.ColorSeqEvenLines;

            if (DigitalOutputMode.HSyncPolarity == 0U) {
                pDigiRgbConfig->ExtLineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
            } else {
                pDigiRgbConfig->ExtLineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
            }
            if (DigitalOutputMode.VSyncPolarity == 0U) {
                pDigiRgbConfig->ExtFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
            } else {
                pDigiRgbConfig->ExtFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
            }
            if (DigitalOutputMode.HvldPolarity == 0U) {
                pDigiRgbConfig->ExtDataValidPolarity = VOUT_SIGNAL_ACTIVE_LOW;
            } else {
                pDigiRgbConfig->ExtDataValidPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
            }
            if (DigitalOutputMode.ClkSampleEdge == 0U) {
                pDigiRgbConfig->ExtClkSampleEdge = VOUT_SIGNAL_EDGE_LOW_TO_HIGH;
            } else {
                pDigiRgbConfig->ExtClkSampleEdge = VOUT_SIGNAL_EDGE_HIGH_TO_LOW;
            }
        }
    }

    if (pDisplayTiming != NULL) {
        VOUT_GetDisplayTiming(pDisplayTiming);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutDigiRgbSetCsc - Set CSC matrix as register format
 *  @param[in] pVoutCscData Color space conversion data
 *  @param[in] RgbMode Digital RGB mode for reference
 *  @return error code
 */
UINT32 AmbaRTSL_VoutDigiRgbSetCsc(const UINT16 *pVoutCscData, UINT32 RgbMode)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    VOUT_SetDigitalCsc(pVoutCscData);
    AmbaMisra_TouchUnused(&RgbMode);

    return RetVal;
}
#endif

/************************************
    FPD-Link APIs
*************************************/
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28)
/**
 *  AmbaRTSL_VoutFpdLinkEnable - Enable FPD-Link video interface display
 *  @param[in] FpdLinkMode FPD-Link pixel format
 *  @param[in] ColorOrder Color component order of a pixel
 *  @param[in] DataEnablePolarity Data enable signal polarity
 *  @param[in] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutFpdLinkEnable(UINT32 FpdLinkMode, UINT32 ColorOrder, UINT32 DataEnablePolarity, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pDisplayTiming == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {

        /* Fill up the control */
        pAmbaVOUT_DispCfg->DispCtrl.FlipMode = 0U;
        pAmbaVOUT_DispCfg->DispCtrl.DigitalOutput = 1U;
        pAmbaVOUT_DispCfg->DispCtrl.FixedFormatSelect = 0U;

        /* According to Vout output mode definition */
        if ( FpdLinkMode == VOUT_FPD_LINK_MODE_24BIT) {
            pAmbaVOUT_DispCfg->DispCtrl.Fpd4thLaneEnable = 1U;
        } else {
            pAmbaVOUT_DispCfg->DispCtrl.Fpd4thLaneEnable = 0U;
        }

        pAmbaVOUT_DispCfg->DigitalOutputMode.OutputMode = 15U;
        if (DataEnablePolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            pAmbaVOUT_DispCfg->DigitalOutputMode.HvldPolarity = 0U;
        } else {
            pAmbaVOUT_DispCfg->DigitalOutputMode.HvldPolarity = 1U;
        }

        /* Set color order */
        if (ColorOrder == VOUT_FPD_LINK_ORDER_MSB) {
            pAmbaVOUT_DispCfg->DispCtrl.FpdMsbSelect = 1U;
        } else {
            pAmbaVOUT_DispCfg->DispCtrl.FpdMsbSelect = 0U;
        }

        /* Fill up the video timing */
        VOUT_SetDisplayTiming(pDisplayTiming);

        /* Default PHY value */
        AmbaCSL_VoutMipiDsiSetCtrl0(0x51ac0b23);
#if defined(CONFIG_SOC_CV28)
        AmbaCSL_VoutMipiDsiSetCtrl2(0x1700001f);
#else
        AmbaCSL_VoutMipiDsiSetCtrl2(0x170000f1);
#endif
        AmbaCSL_VoutMipiDsiSetAuxCtrl(0x00000100);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutFpdLinkSetPhyCtrl - Set PHY control parameter
 *  @param[in] ParamID PHY parameter ID
 *  @param[in] ParamVal PHY parameter value
 *  @return error code
 */
UINT32 AmbaRTSL_VoutFpdLinkSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    /* TODO: */
    (void)ParamID;
    (void)ParamVal;

    return RetVal;
}

/**
 *  AmbaRTSL_VoutFpdLinkGetStatus - Get FPD-Link video interface related configurations
 *  @param[out] pFpdLinkConfig FPD-Link-specific vout configuration
 *  @param[out] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutFpdLinkGetStatus(AMBA_VOUT_FPD_LINK_CONFIG_s *pFpdLinkConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pFpdLinkConfig != NULL) {
        if (pAmbaVoutDisplay0_Reg->DigitalOutputMode.OutputMode != 15U) {
            RetVal = VOUT_ERR_PROTOCOL;
        } else {
            if (pAmbaVoutDisplay0_Reg->DispCtrl.Fpd4thLaneEnable == 1U) {
                pFpdLinkConfig->FpdLinkMode = VOUT_FPD_LINK_MODE_24BIT;
            } else {
                pFpdLinkConfig->FpdLinkMode = VOUT_FPD_LINK_MODE_18BIT;
            }

            if (pAmbaVoutDisplay0_Reg->DispCtrl.FpdMsbSelect == 1U) {
                pFpdLinkConfig->ColorOrder = VOUT_FPD_LINK_ORDER_MSB;
            } else {
                pFpdLinkConfig->ColorOrder = VOUT_FPD_LINK_ORDER_LSB;
            }
        }
    }

    if (pDisplayTiming != NULL) {
        VOUT_GetDisplayTiming(pDisplayTiming);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutFpdLinkSetCsc - Set CSC matrix as register format
 *  @param[in] pVoutCscData Color space conversion data
 *  @param[in] FpdLinkMode FPD-Link mode for reference
 *  @return error code
 */
UINT32 AmbaRTSL_VoutFpdLinkSetCsc(const UINT16 *pVoutCscData, UINT32 FpdLinkMode)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    VOUT_SetDigitalCsc(pVoutCscData);
    AmbaMisra_TouchUnused(&FpdLinkMode);

    return RetVal;
}
#endif

/************************************
    MIPI CSI/DSI APIs
*************************************/
#if !defined(CONFIG_SOC_H32)
/**
 *  VOUT_SetMipiCsiTiming - Assign video timing parameters for MIPI CSI output
 *  @param[in] pDisplayTiming Video timing parameters
 */
static UINT32 VOUT_SetMipiCsiTiming(const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT16 TempInt;

    /* MIPI CSI-2 spec */
    /* The active area is included in the sync area */
    /* The sync width must be greater than the active width */
    /* |----SyncBackPorch---|-----SyncPulseWidth-----|---SyncFrontPorch----| */
    /* |--------------------|-10-|--ActiveArea--|--------------------------| */


    /* Fill up and check video timing */
    TempInt = pDisplayTiming->HsyncBackPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncFrontPorch - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        pAmbaVOUT_DispCfg_CSI->DispFrmSize.FrameWidth = TempInt;
    }

    TempInt = pDisplayTiming->VsyncBackPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncFrontPorch - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        pAmbaVOUT_DispCfg_CSI->DispFrmSize.FrameHeightFld0 = TempInt;
    }

    TempInt = pDisplayTiming->HsyncBackPorch + 10U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        pAmbaVOUT_DispCfg_CSI->DispTopActiveStart.StartColumn = TempInt;
    }

    TempInt = pDisplayTiming ->VsyncBackPorch + 10U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        pAmbaVOUT_DispCfg_CSI->DispTopActiveStart.StartRow = TempInt;
    }

    TempInt = pDisplayTiming->HsyncBackPorch + 10U + pDisplayTiming->ActivePixels - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        pAmbaVOUT_DispCfg_CSI->DispTopActiveEnd.EndColumn = TempInt;
    }

    TempInt = pDisplayTiming->VsyncBackPorch + 10U + pDisplayTiming->ActiveLines - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        pAmbaVOUT_DispCfg_CSI->DispTopActiveEnd.EndRow = TempInt;
    }

    /* Sync */
    TempInt = pDisplayTiming->HsyncBackPorch;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        pAmbaVOUT_DispCfg_CSI->DigitalHSync.StartColumn = TempInt;
    }

    TempInt = pDisplayTiming->HsyncBackPorch + pDisplayTiming->HsyncPulseWidth - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        pAmbaVOUT_DispCfg_CSI->DigitalHSync.EndColumn = TempInt;
    }

    TempInt = pDisplayTiming->HsyncBackPorch;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        pAmbaVOUT_DispCfg_CSI->DigitalVSyncTopStart.StartColumn = TempInt;
    }

    TempInt = pDisplayTiming->VsyncBackPorch;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        pAmbaVOUT_DispCfg_CSI->DigitalVSyncTopStart.StartRow = TempInt;
    }

    TempInt = pDisplayTiming->HsyncBackPorch + pDisplayTiming->HsyncPulseWidth - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        pAmbaVOUT_DispCfg_CSI->DigitalVSyncTopEnd.EndColumn = TempInt;
    }

    TempInt = pDisplayTiming->VsyncBackPorch + pDisplayTiming->VsyncPulseWidth - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        pAmbaVOUT_DispCfg_CSI->DigitalVSyncTopEnd.EndRow = TempInt;
    }

    /* MIPI CSI has no interlaced format */
    pAmbaVOUT_DispCfg_CSI->DispCtrl.Interlaced = 0U;

    return RetVal;
}

/**
 *  VOUT_SetMipiDsiTiming - Assign video timing parameters for MIPI DSI output
 *  @param[in] pDisplayTiming Video timing parameters
 *  @param[in] NewHFrontPorch Modified new Hsync front porch
 *  @param[in] NewVFrontPorch Modified new Vsync front porch
 */
static void VOUT_SetMipiDsiTiming(const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming, const UINT16 NewHFrontPorch, const UINT16 NewVFrontPorch)
{
    UINT16 Htotal = pDisplayTiming->HsyncFrontPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels;
    UINT16 Vtotal = pDisplayTiming->VsyncFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines;

    /* Fill up the video timing */
    pAmbaVOUT_DispCfg->DispFrmSize.FrameWidth = (UINT16)(Htotal - 1U);
    pAmbaVOUT_DispCfg->DigitalHSync.StartColumn = NewHFrontPorch;
    pAmbaVOUT_DispCfg->DigitalHSync.EndColumn = (UINT16)(NewHFrontPorch + pDisplayTiming->HsyncPulseWidth);

    pAmbaVOUT_DispCfg->DispFrmSize.FrameHeightFld0 = (UINT16)(Vtotal - 1U);
    pAmbaVOUT_DispCfg->DispTopActiveStart.StartColumn = (UINT16)(NewHFrontPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch);
    pAmbaVOUT_DispCfg->DispTopActiveStart.StartRow = (UINT16)(NewVFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming ->VsyncBackPorch);
    pAmbaVOUT_DispCfg->DispTopActiveEnd.EndColumn = (UINT16)(NewHFrontPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels - 1U);
    pAmbaVOUT_DispCfg->DispTopActiveEnd.EndRow = (UINT16)(NewVFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines - 1U);

    /* Sync */
    pAmbaVOUT_DispCfg->DigitalVSyncTopStart.StartColumn = pAmbaVOUT_DispCfg->DigitalHSync.StartColumn;
    pAmbaVOUT_DispCfg->DigitalVSyncTopStart.StartRow = NewVFrontPorch;
    pAmbaVOUT_DispCfg->DigitalVSyncTopEnd.EndColumn = pAmbaVOUT_DispCfg->DigitalHSync.StartColumn;
    pAmbaVOUT_DispCfg->DigitalVSyncTopEnd.EndRow = (UINT16)(NewVFrontPorch + pDisplayTiming->VsyncPulseWidth);

    pAmbaVOUT_DispCfg->DispCtrl.Interlaced = 0U;
}

/**
 *  AmbaRTSL_VoutConfigMipiPhy - Configure MIPI PHY
 *  @param[in] pVoutMipiTiming Pointer to MIPI timing configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VoutConfigMipiPhy(const AMBA_VOUT_MIPI_TIMING_PARAM_s *pVoutMipiTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVoutMipiTiming == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (VoutChan == 0U) {
            AmbaCSL_Vout0MipiDsiSetTrail(pVoutMipiTiming->HsTrail);
            AmbaCSL_Vout0MipiDsiSetClkTrail(pVoutMipiTiming->ClkTrail);
            AmbaCSL_Vout0MipiDsiSetPrepare(pVoutMipiTiming->HsPrepare);
            AmbaCSL_Vout0MipiDsiSetClkPrepare(pVoutMipiTiming->ClkPrepare);
            AmbaCSL_Vout0MipiDsiSetZero(pVoutMipiTiming->HsZero);
            AmbaCSL_Vout0MipiDsiSetClkZero(pVoutMipiTiming->ClkZero);
            AmbaCSL_Vout0MipiDsiSetLpx(pVoutMipiTiming->HsLpx);
            AmbaCSL_Vout0MipiDsiSetInitTx(pVoutMipiTiming->InitTx);
        } else if (VoutChan == 1U) {
            AmbaCSL_Vout1MipiDsiSetTrail(pVoutMipiTiming->HsTrail);
            AmbaCSL_Vout1MipiDsiSetClkTrail(pVoutMipiTiming->ClkTrail);
            AmbaCSL_Vout1MipiDsiSetPrepare(pVoutMipiTiming->HsPrepare);
            AmbaCSL_Vout1MipiDsiSetClkPrepare(pVoutMipiTiming->ClkPrepare);
            AmbaCSL_Vout1MipiDsiSetZero(pVoutMipiTiming->HsZero);
            AmbaCSL_Vout1MipiDsiSetClkZero(pVoutMipiTiming->ClkZero);
            AmbaCSL_Vout1MipiDsiSetLpx(pVoutMipiTiming->HsLpx);
            AmbaCSL_Vout1MipiDsiSetInitTx(pVoutMipiTiming->InitTx);
        } else {
            RetVal = VOUT_ERR_ARG;
        }
#else
        AmbaCSL_VoutMipiDsiSetTrail(pVoutMipiTiming->HsTrail);
        AmbaCSL_VoutMipiDsiSetClkTrail(pVoutMipiTiming->ClkTrail);
        AmbaCSL_VoutMipiDsiSetPrepare(pVoutMipiTiming->HsPrepare);
        AmbaCSL_VoutMipiDsiSetClkPrepare(pVoutMipiTiming->ClkPrepare);
        AmbaCSL_VoutMipiDsiSetZero(pVoutMipiTiming->HsZero);
        AmbaCSL_VoutMipiDsiSetClkZero(pVoutMipiTiming->ClkZero);
        AmbaCSL_VoutMipiDsiSetLpx(pVoutMipiTiming->HsLpx);
        AmbaCSL_VoutMipiDsiSetInitTx(pVoutMipiTiming->InitTx);
#endif
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiCsiEnable - Enable MIPI-CSI display
 *  @param[in] MipiCsiMode HDMI pixel format
 *  @param[in] ColorOrder Pixel bits transmission order
 *  @param[in] MipiLaneNum MIPI lane count
 *  @param[in] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiCsiEnable(UINT32 MipiCsiMode, UINT32 ColorOrder, UINT32 MipiLaneNum, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 MipiCsiRawMode = AmbaVout_MipiGetRawMode(MipiCsiMode);
    UINT8 NumMipiLane;

    AmbaMisra_TouchUnused(&ColorOrder);

    if (pDisplayTiming == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        /* Fill up the control */
        NumMipiLane = (UINT8)MipiLaneNum - 1U;
        pAmbaVOUT_DispCfg_CSI->DispCtrl.NumMipiLane = NumMipiLane;
        pAmbaVOUT_DispCfg_CSI->DispCtrl.FlipMode = 0U;
        pAmbaVOUT_DispCfg_CSI->DispCtrl.DigitalOutput = 1U;
        pAmbaVOUT_DispCfg_CSI->DispCtrl.FixedFormatSelect = 0U;

        /* According to Vout output mode definition */
        if ((MipiCsiRawMode == VOUT_MIPI_CSI_MODE_422_1LANE) ||
            (MipiCsiRawMode == VOUT_MIPI_CSI_MODE_422_2LANE) ||
            (MipiCsiRawMode == VOUT_MIPI_CSI_MODE_422_4LANE)) {
            pAmbaVOUT_DispCfg_CSI->DigitalOutputMode.OutputMode = 11U;
        } else {
            /* RAW8 1/2/4 lane */
            pAmbaVOUT_DispCfg_CSI->DigitalOutputMode.OutputMode = 9U;
        }

        /* Enable frame number increments by 1 for every Frame Sync packet (from 0x1 to 0xffff) by default */
        pAmbaVOUT_DispCfg_CSI->DigitalOutputMode.MipiLineSync = 0U;
        pAmbaVOUT_DispCfg_CSI->DigitalOutputMode.MipiLineCount = 0U;
        pAmbaVOUT_DispCfg_CSI->DigitalOutputMode.MipiFrameCount = 1U;
        pAmbaVOUT_DispCfg_CSI->DigitalOutputMode.MipiLineDataFrameBlank = 0U;
        pAmbaVOUT_DispCfg_CSI->DigitalOutputMode.MipiLineSyncFrameBlank = 0U;
        pAmbaVOUT_DispCfg_CSI->DigitalOutputMode.MipiEccOrder = 0U;

        /* Fill up the video timing */
        RetVal = VOUT_SetMipiCsiTiming(pDisplayTiming);

        /* MIPI D-PHY related setting */
        if (AmbaVout_MipiGetDphyClkMode(MipiCsiMode) != 0U) {
            (void)AmbaRTSL_VoutMipiCsiSetPhyCtrl(VOUT_PHY_MIPI_DPHY_POWER_UP, VOUT_MIPI_DPHY_CONT_CLK);
        } else {
            (void)AmbaRTSL_VoutMipiCsiSetPhyCtrl(VOUT_PHY_MIPI_DPHY_POWER_UP, VOUT_MIPI_DPHY_NONCONT_CLK);
        }

        //Set to 1 when CSI is running
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
        pAmbaVoutTop_Reg->EnableSection1.MipiSelectCsi = 1U;
#else
        pAmbaVOUT_DispCfg_CSI->DispCtrl.SelectCsi = 1U;
#endif
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiCsiSetPhyCtrl - Set PHY control parameter
 *  @param[in] ParamID PHY parameter ID
 *  @param[in] ParamVal PHY parameter value
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiCsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
    UINT32 RetVal;

    switch (ParamID) {
    case VOUT_PHY_MIPI_DPHY_POWER_UP:
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        RetVal = AmbaCSL_VoutCsiPhyPowerUp(VoutChan, ParamVal);
#else
        RetVal = AmbaCSL_VoutCsiPhyPowerUp(ParamVal);
#endif
        break;
    case VOUT_PHY_MIPI_DPHY_POWER_DOWN:
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        RetVal = AmbaCSL_VoutCsiPhyPowerDown(VoutChan);
#else
        RetVal = AmbaCSL_VoutCsiPhyPowerDown();
#endif
        break;
    default:
        RetVal = VOUT_ERR_ARG;
        break;
    };

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiCsiGetStatus - Get MIPI CSI related configurations
 *  @param[out] pMipiCsiConfig MIPI-CSI-specific vout configuration
 *  @param[out] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiCsiGetStatus(AMBA_VOUT_MIPI_CSI_CONFIG_s *pMipiCsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 FrameActiveColStart, FrameActiveColWidth, FrameActiveRowStart, FrameActiveRowHeight;
    UINT32 FrameWidth, FrameHeight; /* H-total and V-total */
    UINT32 ModeOffset;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    const AMBA_VOUT_DISPLAY_CONFIG_REG_s * pReg = pAmbaVoutDisplay0_Reg;
#elif defined(CONFIG_SOC_CV28)
    const AMBA_VOUT_DISPLAY0_CONFIG_REG_s *const pReg = pAmbaVoutDisplay0_Reg;
#else
    const AMBA_VOUT_DISPLAY1_CONFIG_REG_s *const pReg = pAmbaVoutDisplay1_Reg;
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    if (VoutChan == AMBA_VOUT_CHANNEL0) {
        pReg = pAmbaVoutDisplay0_Reg;
    } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
        pReg = pAmbaVoutDisplay1_Reg;
    } else {
        /* Do nothing */
    }
#endif
    if (pMipiCsiConfig != NULL) {
        ModeOffset = pReg->DispCtrl.NumMipiLane;

        if (pReg->DigitalOutputMode.OutputMode == 11U) {
            pMipiCsiConfig->MipiCsiMode = ModeOffset;
        } else if (pReg->DigitalOutputMode.OutputMode == 9U) {
            pMipiCsiConfig->MipiCsiMode = VOUT_MIPI_CSI_MODE_RAW8_1LANE + ModeOffset;
        } else {
            RetVal = VOUT_ERR_PROTOCOL;
        }

        if (RetVal == VOUT_ERR_NONE) {
            pMipiCsiConfig->ColorOrder = 0U;
        }
    }

    if ((RetVal == VOUT_ERR_NONE) && (pDisplayTiming != NULL)) {
#if defined(CONFIG_SOC_CV28)
        pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutLcdClk();
#elif defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
#if defined(CONFIG_QNX)
        (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_VOUTTV, &pDisplayTiming->PixelClkFreq);
#else
        pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutTvClk();
#endif
#else
        if (VoutChan == AMBA_VOUT_CHANNEL0) {
            pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutAClk();
        } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
            pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutBClk();
        } else {
            /* Do nothing */
        }
#endif
        /* Get Vout Display0 timing from Vout registers */
        if (pReg->DispCtrl.Interlaced == 0U) {
            /* Progressive scan */
            pDisplayTiming->DisplayMethod = 0U;
            FrameWidth = pReg->DispFrmSize.FrameWidth + (UINT32)1U;
            FrameHeight = pReg->DispFrmSize.FrameHeightFld0 + (UINT32)1U;
        } else {
            /* Interlaced scan */
            pDisplayTiming->DisplayMethod = 1U;
            FrameWidth = pReg->DispFrmSize.FrameWidth + (UINT32)1U;
            FrameHeight = (pReg->DispFrmSize.FrameHeightFld0 + (UINT32)1U) << 1U;
        }

        FrameActiveColStart = pReg->DispTopActiveStart.StartColumn;
        FrameActiveColWidth = (pReg->DispTopActiveEnd.EndColumn + (UINT32)1U) - FrameActiveColStart;

        FrameActiveRowStart = pReg->DispTopActiveStart.StartRow;
        FrameActiveRowHeight = (pReg->DispTopActiveEnd.EndRow + (UINT32)1U) - FrameActiveRowStart;

        pDisplayTiming->ActiveLines = (UINT16)FrameActiveRowHeight;
        pDisplayTiming->ActivePixels = (UINT16)FrameActiveColWidth;

        if (pReg->DispCtrl.DigitalOutput != 0U) {
            pDisplayTiming->HsyncPulseWidth  = pReg->DigitalHSync.EndColumn;
            pDisplayTiming->HsyncPulseWidth -= pReg->DigitalHSync.StartColumn;
            pDisplayTiming->VsyncPulseWidth  = pReg->DigitalVSyncTopEnd.EndRow;
            pDisplayTiming->VsyncPulseWidth -= pReg->DigitalVSyncTopStart.StartRow;
        } else {
            /* No Output */
            pDisplayTiming->HsyncPulseWidth = 0U;
            pDisplayTiming->VsyncPulseWidth = 0U;
        }

        pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - pDisplayTiming->HsyncPulseWidth);
        pDisplayTiming->HsyncFrontPorch = (UINT16)(FrameWidth - (FrameActiveColStart + FrameActiveColWidth));

        pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - pDisplayTiming->VsyncPulseWidth);
        pDisplayTiming->VsyncFrontPorch = (UINT16)(FrameHeight - (FrameActiveRowStart + FrameActiveRowHeight));
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiCsiSetCsc - Set CSC matrix as register format
 *  @param[in] pVoutCscData Color space conversion data
 *  @param[in] MipiCsiMode MIPI CSI mode for reference
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiCsiSetCsc(const UINT16 *pVoutCscData, UINT32 MipiCsiMode)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    AmbaMisra_TouchUnused(&MipiCsiMode);

    pAmbaVOUT_DispCfg_CSI->DigitalCSC0.CoefA0 = pVoutCscData[0];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC0.CoefA1 = pVoutCscData[1];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC1.CoefA2 = pVoutCscData[2];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC1.CoefA3 = pVoutCscData[3];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC2.CoefA4 = pVoutCscData[4];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC2.CoefA5 = pVoutCscData[5];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC3.CoefA6 = pVoutCscData[6];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC3.CoefA7 = pVoutCscData[7];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC4.CoefA8 = pVoutCscData[8];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC4.ConstB0 = pVoutCscData[9];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC5.ConstB1 = pVoutCscData[10];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC5.ConstB2 = pVoutCscData[11];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC6.Output0ClampLow  = pVoutCscData[12];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC6.Output0ClampHigh = pVoutCscData[13];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC7.Output1ClampLow  = pVoutCscData[14];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC7.Output1ClampHigh = pVoutCscData[15];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC8.Output2ClampLow  = pVoutCscData[16];
    pAmbaVOUT_DispCfg_CSI->DigitalCSC8.Output2ClampHigh = pVoutCscData[17];

    return RetVal;
}

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
/**
 *  AmbaRTSL_VoutMipiCsiSetMaxFrame - Set mipi csi maxinum frame count
 *  @param[in] MaxFrameCount Maxinum frame count
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiCsiSetMaxFrame(UINT32 MaxFrameCount)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (VoutChan == AMBA_VOUT_CHANNEL0) {
        AmbaCSL_Vout0SetMipiMaxFrame(MaxFrameCount);
    } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
        AmbaCSL_Vout1SetMipiMaxFrame(MaxFrameCount);
    } else {
        RetVal = VOUT_ERR_ARG;
    }

    return RetVal;
}
#endif

/**
 *  AmbaRTSL_VoutMipiDsiEnable - Enable MIPI-DSI display
 *  @param[in] MipiDsiMode MIPI DSI mode
 *  @param[in] ColorOrder Pixel bits transmission order
 *  @param[in] MipiLaneNum MIPI lane count
 *  @param[in] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiEnable(UINT32 MipiDsiMode, UINT32 ColorOrder, UINT32 MipiLaneNum)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 MipiDsiRawMode = AmbaVout_MipiGetRawMode(MipiDsiMode);
    UINT8 NumMipiLane;

    AmbaMisra_TouchUnused(&ColorOrder);

    /* Fill up the control */
    NumMipiLane = (UINT8)MipiLaneNum - 1U;
    pAmbaVOUT_DispCfg->DispCtrl.NumMipiLane = NumMipiLane;
    pAmbaVOUT_DispCfg->DispCtrl.FlipMode = 0U;
    pAmbaVOUT_DispCfg->DispCtrl.DigitalOutput = 1U;
    pAmbaVOUT_DispCfg->DispCtrl.FixedFormatSelect = 0U;

    /* According to Vout output mode definition */
    if ((MipiDsiRawMode == VOUT_MIPI_DSI_MODE_422_1LANE) ||
        (MipiDsiRawMode == VOUT_MIPI_DSI_MODE_422_2LANE) ||
        (MipiDsiRawMode == VOUT_MIPI_DSI_MODE_422_4LANE)) {
        pAmbaVOUT_DispCfg->DigitalOutputMode.OutputMode = 11U;
    } else if ((MipiDsiRawMode == VOUT_MIPI_DSI_MODE_565_1LANE) ||
               (MipiDsiRawMode == VOUT_MIPI_DSI_MODE_565_2LANE) ||
               (MipiDsiRawMode == VOUT_MIPI_DSI_MODE_565_4LANE)) {
        pAmbaVOUT_DispCfg->DigitalOutputMode.OutputMode = 12U;
    } else if ((MipiDsiRawMode == VOUT_MIPI_DSI_MODE_666_1LANE) ||
               (MipiDsiRawMode == VOUT_MIPI_DSI_MODE_666_2LANE) ||
               (MipiDsiRawMode == VOUT_MIPI_DSI_MODE_666_4LANE)) {
        pAmbaVOUT_DispCfg->DigitalOutputMode.OutputMode = 13U;
    } else {
        /* 888 1/2/4 lane */
        pAmbaVOUT_DispCfg->DigitalOutputMode.OutputMode = 10U;
    }

    /* MIPI DSI related setting */
    pAmbaVOUT_DispCfg->DigitalOutputMode.MipiLineSync = 1U;
    pAmbaVOUT_DispCfg->DigitalOutputMode.MipiLineCount = 0U;
    pAmbaVOUT_DispCfg->DigitalOutputMode.MipiFrameCount = 0U;
    pAmbaVOUT_DispCfg->DigitalOutputMode.MipiLineDataFrameBlank = 0U;
    pAmbaVOUT_DispCfg->DigitalOutputMode.MipiLineSyncFrameBlank = 1U;
    pAmbaVOUT_DispCfg->DigitalOutputMode.MipiEccOrder = 0U;

    /* (TBD) */
    pAmbaVOUT_DispCfg->DispCtrl.MipiSyncEndEnable = 1U;

    /* MIPI Eotp Mode */
    if (AmbaVout_MipiGetEotpMode(MipiDsiMode) != 0U) {
        pAmbaVOUT_DispCfg->DispCtrl.MipiEotpEnable = 1U;
    } else {
        pAmbaVOUT_DispCfg->DispCtrl.MipiEotpEnable = 0U;
    }

    /* MIPI D-PHY related setting */
    (void)AmbaRTSL_VoutMipiDsiSetPhyCtrl(VOUT_PHY_MIPI_DPHY_POWER_UP, AmbaVout_MipiGetDphyClkMode(MipiDsiMode));

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    if (VoutChan == AMBA_VOUT_CHANNEL0) {
        AmbaCSL_Vout0SetMipiLineSync(1);
        AmbaCSL_Vout0SetMipiLineCount(0);
        AmbaCSL_Vout0SetMipiFrameCount(0);
        AmbaCSL_Vout0SetMipiLineDataBlank(0);
        AmbaCSL_Vout0SetMipiLineSyncBlank(1);
        AmbaCSL_Vout0SetMipiEccOrder(0);

        AmbaCSL_Vout0EnableDigitalOutput();

        /* Choose one mipi output mode as default */
        AmbaCSL_Vout0SetDigitalOutputMode(10);
    } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
        AmbaCSL_Vout1SetMipiLineSync(1);
        AmbaCSL_Vout1SetMipiLineCount(0);
        AmbaCSL_Vout1SetMipiFrameCount(0);
        AmbaCSL_Vout1SetMipiLineDataBlank(0);
        AmbaCSL_Vout1SetMipiLineSyncBlank(1);
        AmbaCSL_Vout1SetMipiEccOrder(0);

        AmbaCSL_Vout1EnableDigitalOutput();

        /* Choose one mipi output mode as default */
        AmbaCSL_Vout1SetDigitalOutputMode(10);
    } else {
        RetVal = VOUT_ERR_ARG;
    }
#else
    AmbaCSL_VoutSetMipiLineSync(1);
    AmbaCSL_VoutSetMipiLineCount(0);
    AmbaCSL_VoutSetMipiFrameCount(0);
    AmbaCSL_VoutSetMipiLineDataBlank(0);
    AmbaCSL_VoutSetMipiLineSyncBlank(1);
    AmbaCSL_VoutSetMipiEccOrder(0);

    AmbaCSL_VoutEnableDigitalOutput();

    /* Choose one mipi output mode as default */
    AmbaCSL_VoutSetDigitalOutputMode(10);
#endif
    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiDsiSetPhyCtrl - Set PHY control parameter
 *  @param[in] ParamID PHY parameter ID
 *  @param[in] ParamVal PHY parameter value
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
    UINT32 RetVal;

    switch (ParamID) {
    case VOUT_PHY_MIPI_DPHY_POWER_UP:
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        RetVal = AmbaCSL_VoutDsiPhyPowerUp(VoutChan, ParamVal);
#else
        RetVal = AmbaCSL_VoutDsiPhyPowerUp(ParamVal);
#endif
        break;
    case VOUT_PHY_MIPI_DPHY_POWER_DOWN:
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        RetVal = AmbaCSL_VoutDsiPhyPowerDown(VoutChan);
#else
        RetVal = AmbaCSL_VoutDsiPhyPowerDown();
#endif
        break;
    default:
        RetVal = VOUT_ERR_ARG;
        break;
    };

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiDsiGetStatus - Get MIPI DSI video interface related configurations
 *  @param[out] pMipiDsiConfig MIPI-DSI-specific vout configuration
 *  @param[out] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiGetStatus(AMBA_VOUT_MIPI_DSI_CONFIG_s *pMipiDsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 ModeOffset;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    const AMBA_VOUT_DISPLAY_CONFIG_REG_s * pReg = pAmbaVoutDisplay0_Reg;
#else
    const AMBA_VOUT_DISPLAY0_CONFIG_REG_s *const pReg = pAmbaVoutDisplay0_Reg;
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    if (VoutChan == AMBA_VOUT_CHANNEL0) {
        pReg = pAmbaVoutDisplay0_Reg;
    } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
        pReg = pAmbaVoutDisplay1_Reg;
    } else {
        /* Do nothing */
    }
#endif
    if (pMipiDsiConfig != NULL) {
        ModeOffset = pReg->DispCtrl.NumMipiLane;

        if (pReg->DigitalOutputMode.OutputMode == 11U) {
            pMipiDsiConfig->MipiDsiMode = ModeOffset;
        } else if (pReg->DigitalOutputMode.OutputMode == 12U) {
            pMipiDsiConfig->MipiDsiMode = VOUT_MIPI_DSI_MODE_565_1LANE + ModeOffset;
        } else if (pReg->DigitalOutputMode.OutputMode == 13U) {
            pMipiDsiConfig->MipiDsiMode = VOUT_MIPI_DSI_MODE_666_1LANE + ModeOffset;
        } else if (pReg->DigitalOutputMode.OutputMode == 10U) {
            pMipiDsiConfig->MipiDsiMode = VOUT_MIPI_DSI_MODE_888_1LANE + ModeOffset;
        } else {
            RetVal = VOUT_ERR_PROTOCOL;
        }

        if (RetVal == VOUT_ERR_NONE) {
            pMipiDsiConfig->ColorOrder = 0U;
        }
    }

    if ((RetVal == VOUT_ERR_NONE) && (pDisplayTiming != NULL)) {
        VOUT_GetDisplayTiming(pDisplayTiming);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiDsiSetCsc - Set CSC matrix as register format
 *  @param[in] pVoutCscData Color space conversion data
 *  @param[in] MipiDsiMode MIPI DSI mode for reference
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiSetCsc(const UINT16 *pVoutCscData, UINT32 MipiDsiMode)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    VOUT_SetDigitalCsc(pVoutCscData);
    AmbaMisra_TouchUnused(&MipiDsiMode);

    return RetVal;
}

/**
 *  AmbaRTSL_VoutSetMipiDsiTiming - Assign video timing parameters for MIPI DSI output
 *  @param[in] pDisplayTiming Video timing parameters
 *  @param[in] NewHFrontPorch Modified new Hsync front porch
 *  @param[in] NewVFrontPorch Modified new Vsync front porch
 */
UINT32 AmbaRTSL_VoutSetMipiDsiTiming(const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming, const UINT16 NewHFrontPorch, const UINT16 NewVFrontPorch)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    VOUT_SetMipiDsiTiming(pDisplayTiming, NewHFrontPorch, NewVFrontPorch);

    return RetVal;
}

/**
 *  VOUT_MipiDsiCommandSetup - Setup one MIPI DSI command
 *  @param[in] DataType Packet data type
 *  @param[in] NumParam Number of parameters
 *  @param[in] pParam Pointer to parameters
 */
static void VOUT_MipiDsiCommandSetup(UINT32 DataType, UINT32 NumParam, const UINT8 *pParam)
{
    UINT32 CtrlRegVal = 0x0U;
    UINT32 RegVal, i;
    UINT8 WorkByte[16];

    if (pParam != NULL) {
        /* Up to 16 payload bytes */
        for (i = 0U; i < 16U; i++) {
            if (i < NumParam) {
                WorkByte[i] = pParam[i];
            } else {
                WorkByte[i] = 0x00U;
            }
        }

        /* Write back payload bytes */
        RegVal = WorkByte[3];
        RegVal = (RegVal << 8U) | WorkByte[2];
        RegVal = (RegVal << 8U) | WorkByte[1];
        RegVal = (RegVal << 8U) | WorkByte[0];
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (VoutChan == AMBA_VOUT_CHANNEL0) {
            AmbaCSL_Mipi0SetCommandParam0(RegVal);
        } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
            AmbaCSL_Mipi1SetCommandParam0(RegVal);
        } else {
            /* Do nothing */
        }
#else
        AmbaCSL_MipiSetCommandParam0(RegVal);
#endif

        RegVal = WorkByte[7];
        RegVal = (RegVal << 8U) | WorkByte[6];
        RegVal = (RegVal << 8U) | WorkByte[5];
        RegVal = (RegVal << 8U) | WorkByte[4];
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (VoutChan == AMBA_VOUT_CHANNEL0) {
            AmbaCSL_Mipi0SetCommandParam1(RegVal);
        } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
            AmbaCSL_Mipi1SetCommandParam1(RegVal);
        } else {
            /* Do nothing */
        }
#else
        AmbaCSL_MipiSetCommandParam1(RegVal);
#endif

        RegVal = WorkByte[11];
        RegVal = (RegVal << 8U) | WorkByte[10];
        RegVal = (RegVal << 8U) | WorkByte[9];
        RegVal = (RegVal << 8U) | WorkByte[8];
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (VoutChan == AMBA_VOUT_CHANNEL0) {
            AmbaCSL_Mipi0SetCommandParam2(RegVal);
        } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
            AmbaCSL_Mipi1SetCommandParam2(RegVal);
        } else {
            /* Do nothing */
        }
#else
        AmbaCSL_MipiSetCommandParam2(RegVal);
#endif

        RegVal = WorkByte[15];
        RegVal = (RegVal << 8U) | WorkByte[14];
        RegVal = (RegVal << 8U) | WorkByte[13];
        RegVal = (RegVal << 8U) | WorkByte[12];
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (VoutChan == AMBA_VOUT_CHANNEL0) {
            AmbaCSL_Mipi0SetCommandParam3(RegVal);
        } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
            AmbaCSL_Mipi1SetCommandParam3(RegVal);
        } else {
            /* Do nothing */
        }
#else
        AmbaCSL_MipiSetCommandParam3(RegVal);
#endif
    }

    /* Set command control */
    if (DataType == MIPI_DSI_PKT_GENERIC_NONIMANGE_DATA) {
        CtrlRegVal = 0x00040000U;
    } else if (DataType == MIPI_DSI_PKT_DCS_LONGWRITE) {
        CtrlRegVal = 0x00080000U;
    } else {
        CtrlRegVal = 0x00000000U;
    }
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    if (VoutChan == AMBA_VOUT_CHANNEL0) {
        AmbaCSL_Mipi0SetCommandCtrl(CtrlRegVal);
    } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
        AmbaCSL_Mipi1SetCommandCtrl(CtrlRegVal);
    } else {
        /* Do nothing */
    }
#else
    AmbaCSL_MipiSetCommandCtrl(CtrlRegVal);
#endif
}

/**
 *  AmbaRTSL_VoutMipiDsiDcsWrite - Send one DCS command packet
 *  @param[in] DcsCmd DCS command ID
 *  @param[in] NumParam Number of parameters
 *  @param[in] pParam Pointer to parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiDcsWrite(UINT32 DcsCmd, UINT32 NumParam, const UINT8 *pParam)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    AMBA_MIPI_DCS_SHORT_COMMAND_HEADER_s ShortCmdHeader;
    AMBA_MIPI_DCS_LONG_COMMAND_HEADER_s LongCmdHeader;
    UINT8 WordCount0;
    UINT32 RegVal = 0U;

    if (NumParam < 2U) {
        /* Fill up header */
        if (AmbaWrap_memcpy(&ShortCmdHeader, &RegVal, sizeof(AMBA_MIPI_DCS_SHORT_COMMAND_HEADER_s)) == ERR_NONE) {
            ShortCmdHeader.CommandType = (UINT8)DcsCmd;
            if ((NumParam != 0U) && (pParam != NULL)) {
                ShortCmdHeader.DataType = MIPI_DSI_PKT_DCS_SHORTWRITE1;
                ShortCmdHeader.Parameter = *pParam;
            } else {
                ShortCmdHeader.DataType = MIPI_DSI_PKT_DCS_SHORTWRITE0;
            }
        }
        if (AmbaWrap_memcpy(&RegVal, &ShortCmdHeader, sizeof(AMBA_MIPI_DCS_SHORT_COMMAND_HEADER_s)) == ERR_NONE) {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            if (VoutChan == AMBA_VOUT_CHANNEL0) {
                AmbaCSL_Mipi0SetCommandHeader(RegVal);
            } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
                AmbaCSL_Mipi1SetCommandHeader(RegVal);
            } else {
                /* Do nothing */
            }
#else
            AmbaCSL_MipiSetCommandHeader(RegVal);
#endif
        }

        /* Setup Mipi command */
        VOUT_MipiDsiCommandSetup(ShortCmdHeader.DataType, NumParam, NULL);
    } else {
        /* Fill up header */
        if (AmbaWrap_memcpy(&LongCmdHeader, &RegVal, sizeof(AMBA_MIPI_DCS_LONG_COMMAND_HEADER_s)) == ERR_NONE) {
            LongCmdHeader.DataType = MIPI_DSI_PKT_DCS_LONGWRITE;
            LongCmdHeader.CommandType = (UINT8)DcsCmd;
            WordCount0 = (UINT8)NumParam + (UINT8)1U;
            LongCmdHeader.WordCount0 = WordCount0;    /* WordCount0 = send byte + 1 */
        }
        if (AmbaWrap_memcpy(&RegVal, &LongCmdHeader, sizeof(AMBA_MIPI_DCS_LONG_COMMAND_HEADER_s)) == ERR_NONE) {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            if (VoutChan == AMBA_VOUT_CHANNEL0) {
                AmbaCSL_Mipi0SetCommandHeader(RegVal);
            } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
                AmbaCSL_Mipi1SetCommandHeader(RegVal);
            } else {
                /* Do nothing */
            }
#else
            AmbaCSL_MipiSetCommandHeader(RegVal);
#endif
        }

        /* Setup Mipi command */
        VOUT_MipiDsiCommandSetup(MIPI_DSI_PKT_DCS_LONGWRITE, NumParam, pParam);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiDsiNormalWrite - Send one normal packet
 *  @param[in] NumParam Number of parameters
 *  @param[in] pParam Pointer to parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiNormalWrite(UINT32 NumParam, const UINT8 *pParam)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    AMBA_MIPI_DCS_LONG_COMMAND_HEADER_s LongCmdHeader;
    UINT32 RegVal = 0U;

    /* Fill up header */
    if (AmbaWrap_memcpy(&LongCmdHeader, &RegVal, sizeof(AMBA_MIPI_DCS_LONG_COMMAND_HEADER_s)) == ERR_NONE) {
        LongCmdHeader.DataType = MIPI_DSI_PKT_GENERIC_NONIMANGE_DATA;
        LongCmdHeader.WordCount0 = (UINT8)NumParam; /* WordCount0 = send byte */
    }
    if (AmbaWrap_memcpy(&RegVal, &LongCmdHeader, sizeof(AMBA_MIPI_DCS_LONG_COMMAND_HEADER_s)) == ERR_NONE) {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (VoutChan == AMBA_VOUT_CHANNEL0) {
            AmbaCSL_Mipi0SetCommandHeader(RegVal);
        } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
            AmbaCSL_Mipi1SetCommandHeader(RegVal);
        } else {
            /* Do nothing */
        }
#else
        AmbaCSL_MipiSetCommandHeader(RegVal);
#endif
    }

    /* Setup Mipi command */
    VOUT_MipiDsiCommandSetup(MIPI_DSI_PKT_GENERIC_NONIMANGE_DATA, NumParam, pParam);

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiDsiSetBlankPkt - Setup blank packets
 *  @param[in] pMipiDsiBlankPktCtrl Blank packets control
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiSetBlankPkt(const AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s *pMipiDsiBlankPktCtrl)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    /* Payload bytes */
    if (pMipiDsiBlankPktCtrl->LenHBP > 0U) {
        pAmbaVOUT_DispCfg->MipiBlankCtrl0.PayloadBytes0 = (UINT16)pMipiDsiBlankPktCtrl->LenHBP;
        pAmbaVOUT_DispCfg->MipiBlankCtrl0.UseBlank = 1;
    } else {
        pAmbaVOUT_DispCfg->MipiBlankCtrl0.UseBlank = 0;
    }

    if (pMipiDsiBlankPktCtrl->LenHSA > 0U) {
        pAmbaVOUT_DispCfg->MipiBlankCtrl0.PayloadBytes1 = (UINT16)pMipiDsiBlankPktCtrl->LenHSA;
        pAmbaVOUT_DispCfg->MipiBlankCtrl0.SyncUseBlank = 1;
    } else {
        pAmbaVOUT_DispCfg->MipiBlankCtrl0.SyncUseBlank = 0;
    }

    if (pMipiDsiBlankPktCtrl->LenHFP > 0U) {
        pAmbaVOUT_DispCfg->MipiBlankCtrl1.PayloadBytes0 = (UINT16)pMipiDsiBlankPktCtrl->LenHFP;
        pAmbaVOUT_DispCfg->MipiBlankCtrl1.UseBlank = 1;
    } else {
        pAmbaVOUT_DispCfg->MipiBlankCtrl1.UseBlank = 0;
    }

    if (pMipiDsiBlankPktCtrl->LenBLLP > 0U) {
        pAmbaVOUT_DispCfg->MipiBlankCtrl1.PayloadBytes1 = (UINT16)pMipiDsiBlankPktCtrl->LenBLLP;
        pAmbaVOUT_DispCfg->MipiBlankCtrl1.SyncUseBlank = 1;
    } else {
        pAmbaVOUT_DispCfg->MipiBlankCtrl1.SyncUseBlank = 0;
    }

    return RetVal;
}

#if defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
/**
 *  AmbaRTSL_VoutMipiDsiSetBtaSrc - Set BTA enable source
 *  @param[in] BtaEnSrc BTA enable source
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiSetBtaSrc(UINT32 BtaEnSrc)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (BtaEnSrc >= NUM_MIPI_DSI_BTA_ENABLE) {
        RetVal = VOUT_ERR_ARG;
    } else {
        AmbaCSL_VoutMipiDsiSetBtaSrc(BtaEnSrc);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiDsiGetAck - Get ack data with BTA
 *  @param[out] pAckData pointer to received ack data
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiGetAckData(UINT32 *pAckData)
{
    UINT32 RetVal = VOUT_ERR_NONE;
#if defined(CONFIG_SOC_CV28)
    RetVal = AmbaCSL_VoutMipiDsiGetAckData(pAckData);
#elif !defined(CONFIG_QNX)
    RetVal = AmbaCSL_VoutMipiDsiGetAckData(VoutChan, pAckData);
#else
    AmbaMisra_TouchUnused(pAckData);
#endif
    return RetVal;
}
#endif
#endif

/************************************
    CVBS APIs
*************************************/
/**
 *  SetRegValue - Assign cvbs settings to memory address
 *  @param[in] pRegAddr register address
 *  @param[in] RegVal data value
 */
static inline void SetRegValue(const volatile void *pRegAddr, UINT32 RegVal)
{
    UINT32 *pReg;
    AmbaMisra_TypeCast(&pReg, &pRegAddr);
    *pReg = RegVal;
}

/**
 *  AmbaRTSL_VoutCvbsEnable - Enable CVBS output
 *  @param[in] pDisplayCvbsConfig CVBS configuration
 *  @param[in] pTveConfig TV encoder configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VoutCvbsEnable(const AMBA_VOUT_DISPLAY_CVBS_CONFIG_REG_s *pDisplayCvbsConfig, const AMBA_VOUT_TVENC_CONTROL_REG_s *pTveConfig)
{
    UINT32 RetVal = VOUT_ERR_NONE;
#if defined(CONFIG_SOC_CV28)
    const AMBA_VOUT_DISPLAY0_CONFIG_REG_s *const pReg = pAmbaVoutDisplay0_Reg;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    const AMBA_VOUT_DISPLAY_C_CONFIG_REG_s *const pReg = pAmbaVoutDisplay2_Reg;
    /* Vout Top settings */
    pAmbaVoutTop_Reg->EnableSection1.Enable = 0U;    /* Disable clocks for Display_b and Mixer_b */
    pAmbaVoutTop_Reg->EnableSection2.Enable = 2U;    /* Enable mixer_b drive display_c */
#else
    const AMBA_VOUT_DISPLAY1_CONFIG_REG_s *const pReg = pAmbaVoutDisplay1_Reg;
#endif

    /* Config through ucode as default setting */
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->DispCtrl, pDisplayCvbsConfig->DispCtrl);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->DispFrmSize, pDisplayCvbsConfig->DispFrmSize);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->DispFldHeight, pDisplayCvbsConfig->DispFldHeight);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->DispTopActiveStart, pDisplayCvbsConfig->DispTopActiveStart);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->DispTopActiveEnd, pDisplayCvbsConfig->DispTopActiveEnd);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->DispBtmActiveStart, pDisplayCvbsConfig->DispBtmActiveStart);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->DispBtmActiveEnd, pDisplayCvbsConfig->DispBtmActiveEnd);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->DispBackgroundColor, pDisplayCvbsConfig->DispBackgroundColor);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogOutputMode, pDisplayCvbsConfig->AnalogOutputMode);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogHSync, pDisplayCvbsConfig->AnalogHSync);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogVSyncTopStart, pDisplayCvbsConfig->AnalogVSyncTopStart);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogVSyncTopEnd, pDisplayCvbsConfig->AnalogVSyncTopEnd);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogVSyncBtmStart, pDisplayCvbsConfig->AnalogVSyncBtmStart);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogVSyncBtmEnd, pDisplayCvbsConfig->AnalogVSyncBtmEnd);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogCSC0, pDisplayCvbsConfig->AnalogCSC0);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogCSC1, pDisplayCvbsConfig->AnalogCSC1);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogCSC2, pDisplayCvbsConfig->AnalogCSC2);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogCSC3, pDisplayCvbsConfig->AnalogCSC3);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogCSC4, pDisplayCvbsConfig->AnalogCSC4);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogCSC5, pDisplayCvbsConfig->AnalogCSC5);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogCSC6, pDisplayCvbsConfig->AnalogCSC6);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogCSC7, pDisplayCvbsConfig->AnalogCSC7);
    SetRegValue(&pAmbaVOUT_DispCfg_CVBS->AnalogCSC8, pDisplayCvbsConfig->AnalogCSC8);

    /* clear TV encoder configuration data */
    if (AmbaWrap_memset(AmbaVOUT_TveConfigData, 0, sizeof(AmbaVOUT_TveConfigData)) == ERR_NONE) {

        AmbaVOUT_TveConfigData[32] = pTveConfig->TvEncReg32;
        AmbaVOUT_TveConfigData[33] = pTveConfig->TvEncReg33;
        AmbaVOUT_TveConfigData[34] = pTveConfig->TvEncReg34;
        AmbaVOUT_TveConfigData[35] = pTveConfig->TvEncReg35;
        AmbaVOUT_TveConfigData[36] = pTveConfig->TvEncReg36;
        AmbaVOUT_TveConfigData[37] = pTveConfig->TvEncReg37;
        AmbaVOUT_TveConfigData[38] = pTveConfig->TvEncReg38;
        AmbaVOUT_TveConfigData[39] = pTveConfig->TvEncReg39;
        AmbaVOUT_TveConfigData[40] = pTveConfig->TvEncReg40;
        AmbaVOUT_TveConfigData[42] = pTveConfig->TvEncReg42;
        AmbaVOUT_TveConfigData[43] = pTveConfig->TvEncReg43;
        AmbaVOUT_TveConfigData[44] = pTveConfig->TvEncReg44;
        AmbaVOUT_TveConfigData[45] = pTveConfig->TvEncReg45;
        AmbaVOUT_TveConfigData[46] = pTveConfig->TvEncReg46;
        AmbaVOUT_TveConfigData[47] = pTveConfig->TvEncReg47;
        AmbaVOUT_TveConfigData[50] = pTveConfig->TvEncReg50;
        AmbaVOUT_TveConfigData[51] = pTveConfig->TvEncReg51;
        AmbaVOUT_TveConfigData[52] = pTveConfig->TvEncReg52;
        AmbaVOUT_TveConfigData[56] = pTveConfig->TvEncReg56;
        AmbaVOUT_TveConfigData[57] = pTveConfig->TvEncReg57;
        AmbaVOUT_TveConfigData[58] = pTveConfig->TvEncReg58;
        AmbaVOUT_TveConfigData[59] = pTveConfig->TvEncReg59;
        AmbaVOUT_TveConfigData[60] = pTveConfig->TvEncReg60;
        AmbaVOUT_TveConfigData[61] = pTveConfig->TvEncReg61;
        AmbaVOUT_TveConfigData[62] = pTveConfig->TvEncReg62;
        AmbaVOUT_TveConfigData[65] = pTveConfig->TvEncReg65;
        AmbaVOUT_TveConfigData[66] = pTveConfig->TvEncReg66;
        AmbaVOUT_TveConfigData[67] = pTveConfig->TvEncReg67;
        AmbaVOUT_TveConfigData[68] = pTveConfig->TvEncReg68;
        AmbaVOUT_TveConfigData[69] = pTveConfig->TvEncReg69;
        AmbaVOUT_TveConfigData[96] = pTveConfig->TvEncReg96;
        AmbaVOUT_TveConfigData[97] = pTveConfig->TvEncReg97;
        AmbaVOUT_TveConfigData[99] = pTveConfig->TvEncReg99;
        AmbaVOUT_TveConfigData[120] = pTveConfig->TvEncReg120;
        AmbaVOUT_TveConfigData[121] = pTveConfig->TvEncReg121;
    }

    /* Config through debugport as default setting */
    SetRegValue(&pReg->DispCtrl, pDisplayCvbsConfig->DispCtrl);
    SetRegValue(&pReg->DispFrmSize, pDisplayCvbsConfig->DispFrmSize);
    SetRegValue(&pReg->DispFldHeight, pDisplayCvbsConfig->DispFldHeight);
    SetRegValue(&pReg->DispTopActiveStart, pDisplayCvbsConfig->DispTopActiveStart);
    SetRegValue(&pReg->DispTopActiveEnd, pDisplayCvbsConfig->DispTopActiveEnd);
    SetRegValue(&pReg->DispBtmActiveStart, pDisplayCvbsConfig->DispBtmActiveStart);
    SetRegValue(&pReg->DispBtmActiveEnd, pDisplayCvbsConfig->DispBtmActiveEnd);
    SetRegValue(&pReg->DispBackgroundColor, pDisplayCvbsConfig->DispBackgroundColor);
    SetRegValue(&pReg->AnalogOutputMode, pDisplayCvbsConfig->AnalogOutputMode);
    SetRegValue(&pReg->AnalogHSync, pDisplayCvbsConfig->AnalogHSync);
    SetRegValue(&pReg->AnalogVSyncTopStart, pDisplayCvbsConfig->AnalogVSyncTopStart);
    SetRegValue(&pReg->AnalogVSyncTopEnd, pDisplayCvbsConfig->AnalogVSyncTopEnd);
    SetRegValue(&pReg->AnalogVSyncBtmStart, pDisplayCvbsConfig->AnalogVSyncBtmStart);
    SetRegValue(&pReg->AnalogVSyncBtmEnd, pDisplayCvbsConfig->AnalogVSyncBtmEnd);
    SetRegValue(&pReg->AnalogCSC0, pDisplayCvbsConfig->AnalogCSC0);
    SetRegValue(&pReg->AnalogCSC1, pDisplayCvbsConfig->AnalogCSC1);
    SetRegValue(&pReg->AnalogCSC2, pDisplayCvbsConfig->AnalogCSC2);
    SetRegValue(&pReg->AnalogCSC3, pDisplayCvbsConfig->AnalogCSC3);
    SetRegValue(&pReg->AnalogCSC4, pDisplayCvbsConfig->AnalogCSC4);
    SetRegValue(&pReg->AnalogCSC5, pDisplayCvbsConfig->AnalogCSC5);
    SetRegValue(&pReg->AnalogCSC6, pDisplayCvbsConfig->AnalogCSC6);
    SetRegValue(&pReg->AnalogCSC7, pDisplayCvbsConfig->AnalogCSC7);
    SetRegValue(&pReg->AnalogCSC8, pDisplayCvbsConfig->AnalogCSC8);

    /* Config TvEnc through debugport as default setting */
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg32, pTveConfig->TvEncReg32);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg33, pTveConfig->TvEncReg33);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg34, pTveConfig->TvEncReg34);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg35, pTveConfig->TvEncReg35);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg36, pTveConfig->TvEncReg36);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg37, pTveConfig->TvEncReg37);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg38, pTveConfig->TvEncReg38);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg39, pTveConfig->TvEncReg39);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg40, pTveConfig->TvEncReg40);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg42, pTveConfig->TvEncReg42);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg43, pTveConfig->TvEncReg43);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg44, pTveConfig->TvEncReg44);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg45, pTveConfig->TvEncReg45);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg46, pTveConfig->TvEncReg46);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg47, pTveConfig->TvEncReg47);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg50, pTveConfig->TvEncReg50);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg51, pTveConfig->TvEncReg51);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg52, pTveConfig->TvEncReg52);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg56, pTveConfig->TvEncReg56);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg57, pTveConfig->TvEncReg57);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg58, pTveConfig->TvEncReg58);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg59, pTveConfig->TvEncReg59);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg60, pTveConfig->TvEncReg60);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg61, pTveConfig->TvEncReg61);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg62, pTveConfig->TvEncReg62);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg65, pTveConfig->TvEncReg65);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg66, pTveConfig->TvEncReg66);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg67, pTveConfig->TvEncReg67);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg68, pTveConfig->TvEncReg68);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg69, pTveConfig->TvEncReg69);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg120, pTveConfig->TvEncReg120);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg121, pTveConfig->TvEncReg121);

    return RetVal;
}

/**
 *  AmbaRTSL_VoutCvbsSetIreParam - Set CVBS IRE parameter
 *  @param[in] IreIndex IRE parameter ID
 *  @param[in] Param IRE parameter value
 *  @return error code
 */
UINT32 AmbaRTSL_VoutCvbsSetIreParam(UINT32 IreIndex, UINT32 Param)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (IreIndex == VOUT_CVBS_IRE_PARAM_CLAMP) {
        AmbaVOUT_TveConfigData[44] = Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_BLACK) {
        AmbaVOUT_TveConfigData[42] = Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_BLANK) {
        AmbaVOUT_TveConfigData[43] = Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_SYNC) {
        AmbaVOUT_TveConfigData[45] = Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTGAIN) {
        pAmbaVOUT_DispCfg_CVBS->AnalogCSC6.CoefA4 = (UINT16)Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTOFFSET) {
        pAmbaVOUT_DispCfg_CVBS->AnalogCSC7.ConstB1 = (UINT16)Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTMIN) {
        pAmbaVOUT_DispCfg_CVBS->AnalogCSC8.Output1ClampLow = (UINT16)Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTMAX) {
        pAmbaVOUT_DispCfg_CVBS->AnalogCSC8.Output1ClampHigh = (UINT16)Param;
    } else {
        RetVal = VOUT_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutCvbsGetIreParam - Get CVBS IRE parameter
 *  @param[in] IreIndex IRE parameter ID
 *  @return IRE parameter value
 */
UINT32 AmbaRTSL_VoutCvbsGetIreParam(UINT32 IreIndex)
{
    UINT32 RetValue = 0;

    if (IreIndex == VOUT_CVBS_IRE_PARAM_CLAMP) {
        RetValue = AmbaVOUT_TveConfigData[44];
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_BLACK) {
        RetValue = AmbaVOUT_TveConfigData[42];
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_BLANK) {
        RetValue = AmbaVOUT_TveConfigData[43];
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_SYNC) {
        RetValue = AmbaVOUT_TveConfigData[45];
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTGAIN) {
        RetValue = (UINT32)pAmbaVOUT_DispCfg_CVBS->AnalogCSC6.CoefA4;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTOFFSET) {
        RetValue = (UINT32)pAmbaVOUT_DispCfg_CVBS->AnalogCSC7.ConstB1;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTMIN) {
        RetValue = (UINT32)pAmbaVOUT_DispCfg_CVBS->AnalogCSC8.Output1ClampLow;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTMAX) {
        RetValue = (UINT32)pAmbaVOUT_DispCfg_CVBS->AnalogCSC8.Output1ClampHigh;
    } else {
        /* else */
    }

    return RetValue;
}

/**
 *  AmbaRTSL_VoutCvbsEnableColorBar - Enable color bar pattern generation
 *  @param[in] EnableFlag 1 = Enable; 0 = Disable color bar pattern generation
 *  @param[in] pTveConfig TV encoder configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VoutCvbsEnableColorBar(UINT32 EnableFlag, const AMBA_VOUT_TVENC_CONTROL_REG_s *pTveConfig)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 TvEncReg46Data, TvEncReg56Data;

    if (EnableFlag == 0U) {
        /* Update ucode buffer settings */
        AmbaVOUT_TveConfigData[46] &= ~((UINT32)1U << 5U);
        AmbaVOUT_TveConfigData[56] &= ~((UINT32)1U << 4U);
        AmbaVOUT_TveConfigData[56] |= (UINT32)1U << 3U;
        /* Update settings with debugport */
        TvEncReg46Data = pTveConfig->TvEncReg46 & ~((UINT32)1U << 5U);
        TvEncReg56Data = pTveConfig->TvEncReg56 & ~((UINT32)1U << 4U);
        TvEncReg56Data |= (UINT32)1U << 3U;
    } else {
        /* Update ucode buffer settings */
        AmbaVOUT_TveConfigData[46] |= (UINT32)1U << 5U;
        AmbaVOUT_TveConfigData[56] |= (UINT32)1U << 4U;
        AmbaVOUT_TveConfigData[56] &= ~((UINT32)1U << 3U);
        /* Update settings with debugport */
        TvEncReg46Data = pTveConfig->TvEncReg46 | ((UINT32)1U << 5U);
        TvEncReg56Data = pTveConfig->TvEncReg56 | ((UINT32)1U << 4U);
        TvEncReg56Data &= ~((UINT32)1U << 3U);
    }

    /* Config through debugport */
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg46, TvEncReg46Data);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg56, TvEncReg56Data);

    return RetVal;
}

/**
 *  AmbaRTSL_VoutCvbsPowerCtrl - Enable/Disable DAC power
 *  @param[in] EnableFlag Enable flag
 *  @return error code
 */
UINT32 AmbaRTSL_VoutCvbsPowerCtrl(UINT32 EnableFlag)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (EnableFlag == 0U) {
        AmbaCSL_VoutDacPowerDown();
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        AmbaCSL_VoutDacBistEnable();
#endif
    } else {
        AmbaCSL_VoutDacPowerOn();
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        AmbaCSL_VoutDacBistDisable();
#endif
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutAnalogSetCsc - Set CSC matrix as register format
 *  @param[in] pVoutCscData Color space conversion data
 *  @param[in] AnalogMode Analog mode for reference
 *  @return error code
 */
UINT32 AmbaRTSL_VoutAnalogSetCsc(const UINT16 *pVoutCscData, UINT32 AnalogMode)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    AmbaMisra_TouchUnused(&AnalogMode);

    pAmbaVOUT_DispCfg_CVBS->AnalogCSC0.CoefA0 = pVoutCscData[0];  /* Coef[0][0] */
    pAmbaVOUT_DispCfg_CVBS->AnalogCSC0.CoefA4 = pVoutCscData[3];  /* Coef[1][0] */
    pAmbaVOUT_DispCfg_CVBS->AnalogCSC1.CoefA8 = pVoutCscData[6];  /* Coef[2][0] */
    pAmbaVOUT_DispCfg_CVBS->AnalogCSC1.ConstB0 = pVoutCscData[9];
    pAmbaVOUT_DispCfg_CVBS->AnalogCSC2.ConstB1 = pVoutCscData[10];
    pAmbaVOUT_DispCfg_CVBS->AnalogCSC2.ConstB2 = pVoutCscData[11];
    pAmbaVOUT_DispCfg_CVBS->AnalogCSC3.Output0ClampLow  = pVoutCscData[12];
    pAmbaVOUT_DispCfg_CVBS->AnalogCSC3.Output0ClampHigh = pVoutCscData[13];
    pAmbaVOUT_DispCfg_CVBS->AnalogCSC4.Output1ClampLow  = pVoutCscData[14];
    pAmbaVOUT_DispCfg_CVBS->AnalogCSC4.Output1ClampHigh = pVoutCscData[15];
    pAmbaVOUT_DispCfg_CVBS->AnalogCSC5.Output2ClampLow  = pVoutCscData[16];
    pAmbaVOUT_DispCfg_CVBS->AnalogCSC5.Output2ClampHigh = pVoutCscData[17];

    return RetVal;
}

/************************************
    HDMI APIs
*************************************/
#if !defined(CONFIG_SOC_CV28)
/**
 *  VOUT_SetFixedFormat - Set fixed format according to display timing
 *  @param[in] pDisplayTiming Video timing parameters
 */
static void VOUT_SetFixedFormat(const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    /* Use fixed format for 480I and 576I to support pixel repetition features */
    if ((pDisplayTiming->DisplayMethod == 1U) &&
        (pDisplayTiming->HsyncFrontPorch == 38U) &&
        (pDisplayTiming->HsyncPulseWidth == 124U) &&
        (pDisplayTiming->HsyncBackPorch == 114U) &&
        (pDisplayTiming->ActivePixels == 1440U) &&
        (pDisplayTiming->VsyncFrontPorch == 4U) &&
        (pDisplayTiming->VsyncPulseWidth == 3U) &&
        (pDisplayTiming->VsyncBackPorch == 15U) &&
        (pDisplayTiming->ActiveLines == 480U)) {
        /* 480I60 */
        pAmbaVOUT_DispCfg_HDMI->DispCtrl.FixedFormatSelect = 1U;
    } else if ((pDisplayTiming->DisplayMethod == 1U) &&
               (pDisplayTiming->HsyncFrontPorch == 24U) &&
               (pDisplayTiming->HsyncPulseWidth == 126U) &&
               (pDisplayTiming->HsyncBackPorch == 138U) &&
               (pDisplayTiming->ActivePixels == 1440U) &&
               (pDisplayTiming->VsyncFrontPorch == 2U) &&
               (pDisplayTiming->VsyncPulseWidth == 3U) &&
               (pDisplayTiming->VsyncBackPorch == 19U) &&
               (pDisplayTiming->ActiveLines == 576U)) {
        /* 576I50 */
        pAmbaVOUT_DispCfg_HDMI->DispCtrl.FixedFormatSelect = 3U;
    } else {
        pAmbaVOUT_DispCfg_HDMI->DispCtrl.FixedFormatSelect = 0U;
    }
}

/**
 *  VOUT_SetDisplayHdmiTiming - Assign video timing parameters for display devices
 *  @param[in] HdmiMode Video pixel format
 *  @param[in] pDisplayTiming Video timing parameters
 */
static void VOUT_SetDisplayHdmiTiming(UINT32 HdmiMode, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT16 Htotal, Vtotal, RegVal;

    Htotal = pDisplayTiming->HsyncFrontPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels;
    if (pDisplayTiming->DisplayMethod == 0U) {
        Vtotal = pDisplayTiming->VsyncFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines;
    } else {
        Vtotal = pDisplayTiming->VsyncFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch;
        Vtotal *= 2U;
        Vtotal = Vtotal + 1U + pDisplayTiming->ActiveLines;
    }

    /* Fill up the video timing */
    RegVal = Htotal - 1U;
    pAmbaVOUT_DispCfg_HDMI->DispFrmSize.FrameWidth = RegVal;
    if ((HdmiMode == HDMI_PIXEL_FORMAT_YCC_420) && ((RegVal % 2U) != 0U)) {
        /* FrameWidth should be even in YUV420 mode */
        RegVal = RegVal - 1U;
        pAmbaVOUT_DispCfg_HDMI->DispFrmSize.FrameWidth = RegVal;
    }

    if (pDisplayTiming->DisplayMethod == 0U) {
        /* 0 = Progressive scan */
        RegVal = Vtotal - 1U;
        pAmbaVOUT_DispCfg_HDMI->DispFrmSize.FrameHeightFld0 = RegVal;

        RegVal = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch;
        pAmbaVOUT_DispCfg_HDMI->DispTopActiveStart.StartColumn = RegVal;

        RegVal = pDisplayTiming->VsyncPulseWidth + pDisplayTiming ->VsyncBackPorch;
        pAmbaVOUT_DispCfg_HDMI->DispTopActiveStart.StartRow = RegVal;

        RegVal = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels - 1U;
        pAmbaVOUT_DispCfg_HDMI->DispTopActiveEnd.EndColumn = RegVal;

        RegVal = pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines - 1U;
        pAmbaVOUT_DispCfg_HDMI->DispTopActiveEnd.EndRow = RegVal;

        /* Sync */
        if (pAmbaVOUT_DispCfg_HDMI->DispCtrl.HdmiOutput != 0U) {
            pAmbaVOUT_DispCfg_HDMI->HdmiHSync.StartColumn = 0U;

            RegVal = pAmbaVOUT_DispCfg_HDMI->HdmiHSync.StartColumn + pDisplayTiming->HsyncPulseWidth;
            pAmbaVOUT_DispCfg_HDMI->HdmiHSync.EndColumn = RegVal;

            pAmbaVOUT_DispCfg_HDMI->HdmiVSyncTopStart.StartColumn = pAmbaVOUT_DispCfg_HDMI->HdmiHSync.StartColumn;
            pAmbaVOUT_DispCfg_HDMI->HdmiVSyncTopStart.StartRow = 0U;
            pAmbaVOUT_DispCfg_HDMI->HdmiVSyncTopEnd.EndColumn = pAmbaVOUT_DispCfg_HDMI->HdmiVSyncTopStart.StartColumn;

            RegVal = pAmbaVOUT_DispCfg_HDMI->HdmiVSyncTopStart.StartRow + pDisplayTiming->VsyncPulseWidth;
            pAmbaVOUT_DispCfg_HDMI->HdmiVSyncTopEnd.EndRow = RegVal;
        }

        pAmbaVOUT_DispCfg_HDMI->DispCtrl.Interlaced = 0U;
    } else {
        /* 1 = Interlaced scan */
        RegVal = (Vtotal >> 1U) - 1U;
        pAmbaVOUT_DispCfg_HDMI->DispFrmSize.FrameHeightFld0 = RegVal;

        RegVal = pAmbaVOUT_DispCfg_HDMI->DispFrmSize.FrameHeightFld0 + 1U;
        pAmbaVOUT_DispCfg_HDMI->DispFldHeight.FrameHeightFld1 = RegVal;

        RegVal = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch;
        pAmbaVOUT_DispCfg_HDMI->DispTopActiveStart.StartColumn = RegVal;

        RegVal = pDisplayTiming->VsyncPulseWidth + pDisplayTiming ->VsyncBackPorch;
        pAmbaVOUT_DispCfg_HDMI->DispTopActiveStart.StartRow = RegVal;

        RegVal = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels - 1U;
        pAmbaVOUT_DispCfg_HDMI->DispTopActiveEnd.EndColumn = RegVal;

        RegVal = pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + (pDisplayTiming->ActiveLines >> 1U) - 1U;
        pAmbaVOUT_DispCfg_HDMI->DispTopActiveEnd.EndRow = RegVal;

        pAmbaVOUT_DispCfg_HDMI->DispBtmActiveStart.StartColumn = pAmbaVOUT_DispCfg_HDMI->DispTopActiveStart.StartColumn;

        RegVal = pAmbaVOUT_DispCfg_HDMI->DispTopActiveStart.StartRow + 1U;
        pAmbaVOUT_DispCfg_HDMI->DispBtmActiveStart.StartRow = RegVal;

        pAmbaVOUT_DispCfg_HDMI->DispBtmActiveEnd.EndColumn = pAmbaVOUT_DispCfg_HDMI->DispTopActiveEnd.EndColumn;

        RegVal = pAmbaVOUT_DispCfg_HDMI->DispTopActiveEnd.EndRow + 1U;
        pAmbaVOUT_DispCfg_HDMI->DispBtmActiveEnd.EndRow = RegVal;

        /* Sync */
        if (pAmbaVOUT_DispCfg_HDMI->DispCtrl.HdmiOutput != 0U) {
            pAmbaVOUT_DispCfg_HDMI->HdmiHSync.StartColumn = 0U;

            RegVal = pAmbaVOUT_DispCfg_HDMI->HdmiHSync.StartColumn + pDisplayTiming->HsyncPulseWidth;
            pAmbaVOUT_DispCfg_HDMI->HdmiHSync.EndColumn = RegVal;

            pAmbaVOUT_DispCfg_HDMI->HdmiVSyncTopStart.StartColumn = pAmbaVOUT_DispCfg_HDMI->HdmiHSync.StartColumn;
            pAmbaVOUT_DispCfg_HDMI->HdmiVSyncTopStart.StartRow = 0U;
            pAmbaVOUT_DispCfg_HDMI->HdmiVSyncTopEnd.EndColumn = pAmbaVOUT_DispCfg_HDMI->HdmiVSyncTopStart.StartColumn;

            RegVal = pAmbaVOUT_DispCfg_HDMI->HdmiVSyncTopStart.StartRow + pDisplayTiming->VsyncPulseWidth;
            pAmbaVOUT_DispCfg_HDMI->HdmiVSyncTopEnd.EndRow = RegVal;

            RegVal = pAmbaVOUT_DispCfg_HDMI->HdmiHSync.StartColumn + (Htotal >> 1U);
            pAmbaVOUT_DispCfg_HDMI->HdmiVSyncBtmStart.StartColumn = RegVal;

            pAmbaVOUT_DispCfg_HDMI->HdmiVSyncBtmStart.StartRow = 0U;
            pAmbaVOUT_DispCfg_HDMI->HdmiVSyncBtmEnd.EndColumn = pAmbaVOUT_DispCfg_HDMI->HdmiVSyncBtmStart.StartColumn;

            RegVal = pAmbaVOUT_DispCfg_HDMI->HdmiVSyncBtmStart.StartRow + pDisplayTiming->VsyncPulseWidth;
            pAmbaVOUT_DispCfg_HDMI->HdmiVSyncBtmEnd.EndRow = RegVal;
        }

        pAmbaVOUT_DispCfg_HDMI->DispCtrl.Interlaced = 1U;
    }

    VOUT_SetFixedFormat(pDisplayTiming);
}

/**
 *  AmbaRTSL_VoutHdmiEnable - Enable HDMI display
 *  @param[in] HdmiMode HDMI pixel format
 *  @param[in] FrameSyncPolarity Frame sync signal polarity
 *  @param[in] LineSyncPolarity Line sync signal polarity
 *  @param[in] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutHdmiEnable(UINT32 HdmiMode, UINT32 FrameSyncPolarity, UINT32 LineSyncPolarity, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (HdmiMode >= VOUT_NUM_HDMI_MODE) {
        RetVal = VOUT_ERR_ARG;
    } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        /* Vout Top settings */
        pAmbaVoutTop_Reg->EnableSection1.Enable = 0U;    /* Disable clocks for Display_b and Mixer_b */
        pAmbaVoutTop_Reg->EnableSection2.Enable = 2U;    /* Enable mixer_b drive display_c */
#endif
        pAmbaVOUT_DispCfg_HDMI->DispCtrl.HdmiOutput = 1U;
        if (HdmiMode == VOUT_HDMI_MODE_RGB888_24BIT) {
            pAmbaVOUT_DispCfg_HDMI->HdmiOutputMode.OutputMode = 1U;
        } else if (HdmiMode == VOUT_HDMI_MODE_YCC444_24BIT) {
            pAmbaVOUT_DispCfg_HDMI->HdmiOutputMode.OutputMode = 0U;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        } else if (HdmiMode == VOUT_HDMI_MODE_YCC420_24BIT) {
            pAmbaVOUT_DispCfg_HDMI->HdmiOutputMode.OutputMode = 3U;
            pAmbaVOUT_DispCfg_HDMI->InputSyncCounterCtrl.SyncEnable = 0U;
            pAmbaVOUT_DispCfg_HDMI->InputConfig.ColorRes = 1U;
#endif
        } else {  /* HdmiMode == VOUT_HDMI_MODE_YCC422_24BIT */
            pAmbaVOUT_DispCfg_HDMI->HdmiOutputMode.OutputMode = 2U;
        }

        if (FrameSyncPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            pAmbaVOUT_DispCfg_HDMI->HdmiOutputMode.VSyncPolarity = 0U;
        } else {
            pAmbaVOUT_DispCfg_HDMI->HdmiOutputMode.VSyncPolarity = 1U;
        }
        if (LineSyncPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            pAmbaVOUT_DispCfg_HDMI->HdmiOutputMode.HSyncPolarity = 0U;
        } else {
            pAmbaVOUT_DispCfg_HDMI->HdmiOutputMode.HSyncPolarity = 1U;
        }

        if (pDisplayTiming != NULL) {
            /* Fill up the video timing */
            VOUT_SetDisplayHdmiTiming(HdmiMode, pDisplayTiming);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutHdmiGetStatus - Get HDMI related configurations
 *  @param[out] pHdmiConfig HDMI-specific vout configuration
 *  @param[out] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutHdmiGetStatus(AMBA_VOUT_HDMI_CONFIG_s *pHdmiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    AMBA_VOUTD_HDMI_OUTPUT_MODE_REG_s HdmiOutputMode;
    UINT32 FrameActiveColStart, FrameActiveColWidth, FrameActiveRowStart, FrameActiveRowHeight;
    UINT32 FrameWidth, FrameHeight; /* H-total and V-total */
#if defined(CONFIG_SOC_CV28)
    const AMBA_VOUT_DISPLAY0_CONFIG_REG_s *const pReg = pAmbaVoutDisplay0_Reg;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    const AMBA_VOUT_DISPLAY_C_CONFIG_REG_s *const pReg = pAmbaVoutDisplay2_Reg;
#else
    const AMBA_VOUT_DISPLAY1_CONFIG_REG_s *const pReg = pAmbaVoutDisplay1_Reg;
#endif

    if (pHdmiConfig != NULL) {
        HdmiOutputMode = pReg->HdmiOutputMode;
        if (HdmiOutputMode.OutputMode == 1U) {
            pHdmiConfig->HdmiMode = VOUT_HDMI_MODE_RGB888_24BIT;
        } else if (HdmiOutputMode.OutputMode == 0U) {
            pHdmiConfig->HdmiMode = VOUT_HDMI_MODE_YCC444_24BIT;
        } else if (HdmiOutputMode.OutputMode == 2U) {
            pHdmiConfig->HdmiMode = VOUT_HDMI_MODE_YCC422_24BIT;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        } else if (HdmiOutputMode.OutputMode == 3U) {
            pHdmiConfig->HdmiMode = VOUT_HDMI_MODE_YCC420_24BIT;
#endif
        } else {
            RetVal = VOUT_ERR_PROTOCOL;
        }

        if (RetVal == VOUT_ERR_NONE) {
            if (HdmiOutputMode.HSyncPolarity == 0U) {
                pHdmiConfig->LineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
            } else {
                pHdmiConfig->LineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
            }
            if (HdmiOutputMode.VSyncPolarity == 0U) {
                pHdmiConfig->FrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
            } else {
                pHdmiConfig->FrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
            }
        }
    }

    if (pDisplayTiming != NULL) {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutCClk();
#else
#if defined(CONFIG_QNX)
        (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_VOUTTV, &pDisplayTiming->PixelClkFreq);
#else
        pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutTvClk();
#endif
#endif
        /* Get Vout Display timing from Vout registers */
        if (pReg->DispCtrl.Interlaced == 0U) {
            /* Progressive scan */
            pDisplayTiming->DisplayMethod = 0U;
            FrameWidth = pReg->DispFrmSize.FrameWidth + (UINT32)1U;
            FrameHeight = pReg->DispFrmSize.FrameHeightFld0 + (UINT32)1U;
        } else {
            /* Interlaced scan */
            pDisplayTiming->DisplayMethod = 1U;
            FrameWidth = pReg->DispFrmSize.FrameWidth + (UINT32)1U;
            FrameHeight = (pReg->DispFrmSize.FrameHeightFld0 + (UINT32)1U) << 1U;
        }

        FrameActiveColStart = pReg->DispTopActiveStart.StartColumn;
        FrameActiveColWidth = (pReg->DispTopActiveEnd.EndColumn + (UINT32)1U) - FrameActiveColStart;

        FrameActiveRowStart = pReg->DispTopActiveStart.StartRow;
        FrameActiveRowHeight = (pReg->DispTopActiveEnd.EndRow + (UINT32)1U) - FrameActiveRowStart;

        pDisplayTiming->ActiveLines = (UINT16)FrameActiveRowHeight;
        pDisplayTiming->ActivePixels = (UINT16)FrameActiveColWidth;

        if (pReg->DispCtrl.HdmiOutput != 0U) {
            pDisplayTiming->HsyncPulseWidth  = pReg->HdmiHSync.EndColumn;
            pDisplayTiming->HsyncPulseWidth -= pReg->HdmiHSync.StartColumn;
            pDisplayTiming->VsyncPulseWidth  = pReg->HdmiVSyncTopEnd.EndRow;
            pDisplayTiming->VsyncPulseWidth -= pReg->HdmiVSyncTopStart.StartRow;
        } else {
            /* No Output */
            pDisplayTiming->HsyncPulseWidth = 0U;
            pDisplayTiming->VsyncPulseWidth = 0U;
        }

        pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - pDisplayTiming->HsyncPulseWidth);
        pDisplayTiming->HsyncFrontPorch = (UINT16)(FrameWidth - (FrameActiveColStart + FrameActiveColWidth));

        pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - pDisplayTiming->VsyncPulseWidth);
        pDisplayTiming->VsyncFrontPorch = (UINT16)(FrameHeight - (FrameActiveRowStart + FrameActiveRowHeight));
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutHdmiSetCsc - Set CSC matrix as register format
 *  @param[in] pHdmiCscData Color space conversion data
 *  @param[in] HdmiMode HDMI mode for reference
 *  @return error code
 */
UINT32 AmbaRTSL_VoutHdmiSetCsc(const UINT16 *pHdmiCscData, UINT32 HdmiMode)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    AmbaMisra_TouchUnused(&HdmiMode);

    pAmbaVOUT_DispCfg_HDMI->HdmiCSC0.CoefA0 = pHdmiCscData[0];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC0.CoefA1 = pHdmiCscData[1];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC1.CoefA2 = pHdmiCscData[2];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC1.CoefA3 = pHdmiCscData[3];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC2.CoefA4 = pHdmiCscData[4];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC2.CoefA5 = pHdmiCscData[5];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC3.CoefA6 = pHdmiCscData[6];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC3.CoefA7 = pHdmiCscData[7];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC4.CoefA8 = pHdmiCscData[8];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC4.ConstB0 = pHdmiCscData[9];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC5.ConstB1 = pHdmiCscData[10];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC5.ConstB2 = pHdmiCscData[11];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC6.Output0ClampLow  = pHdmiCscData[12];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC6.Output0ClampHigh = pHdmiCscData[13];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC7.Output1ClampLow  = pHdmiCscData[14];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC7.Output1ClampHigh = pHdmiCscData[15];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC8.Output2ClampLow  = pHdmiCscData[16];
    pAmbaVOUT_DispCfg_HDMI->HdmiCSC8.Output2ClampHigh = pHdmiCscData[17];

    return RetVal;
}
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
/**
 *  AmbaRTSL_VoutMipiTestPattern - Enable MIPI test pattern output
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiTestPattern(void)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    AMBA_VOUT_DISPLAY_CONFIG_REG_s *pDispCfg;

    if (VoutChan == AMBA_VOUT_CHANNEL0) {
        pDispCfg = &AmbaVOUT_Display0Config;
    } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
        pDispCfg = &AmbaVOUT_Display1Config;
    } else {
        RetVal = VOUT_ERR_ARG;
    }

    if (RetVal == VOUT_ERR_NONE) {
        pDispCfg->DispBackgroundColor.CR = 0xF9U;
        pDispCfg->DispBackgroundColor.CB = 0x5DU;
        pDispCfg->DispBackgroundColor.Y  = 0x18U;

        pDispCfg->ForceBackground = 0x2ffU;
        pDispCfg->OutputEnable = 0x1U;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutConfigThrDbgPort - Configure Vout display through debug port
 *  @param[in] VoutChannel Target vout channel
 *  @param[in] pVoutDispReg pointer to vout display register
 *  @return error code
 */
UINT32 AmbaRTSL_VoutConfigThrDbgPort(UINT32 VoutChannel, const volatile void *pVoutDispReg)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 DisplayConfigData[VOUT_DISPLAY_BUF_SIZE];
    ULONG BaseAddr, Addr, i;
    UINT32 *pAddr;

    if (VoutChannel == AMBA_VOUT_CHANNEL0) {
        if (AmbaWrap_memcpy(DisplayConfigData, &AmbaVOUT_Display0Config, sizeof(DisplayConfigData)) != ERR_NONE) {
            RetVal = VOUT_ERR_UNEXPECTED;
        }
    } else if (VoutChannel == AMBA_VOUT_CHANNEL1) {
        if (AmbaWrap_memcpy(DisplayConfigData, &AmbaVOUT_Display1Config, sizeof(DisplayConfigData)) != ERR_NONE) {
            RetVal = VOUT_ERR_UNEXPECTED;
        }
    } else if (VoutChannel == AMBA_VOUT_CHANNEL2) {
        if (AmbaWrap_memcpy(DisplayConfigData, &AmbaVOUT_Display2Config, sizeof(DisplayConfigData)) != ERR_NONE) {
            RetVal = VOUT_ERR_UNEXPECTED;
        }
    } else {
        RetVal = VOUT_ERR_UNEXPECTED;
    }

    AmbaMisra_TypeCast(&BaseAddr, &pVoutDispReg);

    if (RetVal == VOUT_ERR_NONE) {
        for (i = 0U; i < VOUT_DISPLAY_BUF_SIZE; i++) {
            Addr = BaseAddr + (i * 4U);
            AmbaMisra_TypeCast(&pAddr, &Addr);
            SetRegValue(pAddr, DisplayConfigData[i]);
        }
    }

    return RetVal;
}
#endif

