/**
 *  @file AmbaVOUT.c
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
 *  @details Video Output APIs
 *
 */

#if defined(CONFIG_QNX)
#include "Generic.h"
#include "hw/ambarella_clk.h"
#endif

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif

#include "AmbaDrvEntry.h"
#include "AmbaVOUT.h"
#if defined(CONFIG_LINUX) && (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52))
#include "hw/ambarella_hdmi.h"
#else
#include "AmbaRTSL_HDMI.h"
#endif
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_VOUT.h"

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include <AmbaIOUtility.h>
#endif


#define VOUT_DIGITAL_CLK_RATIO    7U
#define VOUT_LVDS_CLK_RATIO       7U


static AMBA_KAL_MUTEX_t AmbaVoutMutex;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static UINT32 VoutChan = 0U;
#endif

/**
 *  Float2Int32 - Convert a floating-point value to an integer
 *  @param[in] WorkFLOAT A floating-point value
 *  @return An integer
 */
static inline INT32 Float2Int32(FLOAT WorkFLOAT)
{
    INT32 WorkINT;

    if (WorkFLOAT >= 0.0F) {
        WorkINT = (INT32)(WorkFLOAT);
    } else {
        WorkINT = 0 - (INT32)(-WorkFLOAT);
    }
    return WorkINT;
}

/**
 *  VOUT_CalculateCsc - Calculate color space conversion parameters for digital vout interface
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] pVoutCscData Color space conversion data
 */
static void VOUT_CalculateCsc(const AMBA_VOUT_CSC_MATRIX_s *pCscMatrix, UINT16 *pVoutCscData)
{
    pVoutCscData[0] = (UINT16)Float2Int32(pCscMatrix->Coef[0][0] * 1024.0F);
    pVoutCscData[1] = (UINT16)Float2Int32(pCscMatrix->Coef[0][1] * 1024.0F);
    pVoutCscData[2] = (UINT16)Float2Int32(pCscMatrix->Coef[0][2] * 1024.0F);
    pVoutCscData[3] = (UINT16)Float2Int32(pCscMatrix->Coef[1][0] * 1024.0F);
    pVoutCscData[4] = (UINT16)Float2Int32(pCscMatrix->Coef[1][1] * 1024.0F);
    pVoutCscData[5] = (UINT16)Float2Int32(pCscMatrix->Coef[1][2] * 1024.0F);
    pVoutCscData[6] = (UINT16)Float2Int32(pCscMatrix->Coef[2][0] * 1024.0F);
    pVoutCscData[7] = (UINT16)Float2Int32(pCscMatrix->Coef[2][1] * 1024.0F);
    pVoutCscData[8] = (UINT16)Float2Int32(pCscMatrix->Coef[2][2] * 1024.0F);
    pVoutCscData[9] = (UINT16)Float2Int32(pCscMatrix->Offset[0]);
    pVoutCscData[10] = (UINT16)Float2Int32(pCscMatrix->Offset[1]);
    pVoutCscData[11] = (UINT16)Float2Int32(pCscMatrix->Offset[2]);
    pVoutCscData[12] = pCscMatrix->MinVal[0];
    pVoutCscData[13] = pCscMatrix->MaxVal[0];
    pVoutCscData[14] = pCscMatrix->MinVal[1];
    pVoutCscData[15] = pCscMatrix->MaxVal[1];
    pVoutCscData[16] = pCscMatrix->MinVal[2];
    pVoutCscData[17] = pCscMatrix->MaxVal[2];
}

#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX))
static UINT32 VOUT_MemAddrMap(void)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    ULONG BaseAddr, TempAddr;

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    // pAmbaVout_Reg
    BaseAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,vout", "reg", 0U);
    if (BaseAddr == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        BaseAddr = BaseAddr | AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR;
#endif
        AmbaMisra_TypeCast(&pAmbaVout_Reg, &BaseAddr);
    }
    // pAmbaVoutTop_Reg
    TempAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,vout_top", "reg", 0U);
    if (TempAddr == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        TempAddr = TempAddr | AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR;
#endif
        AmbaMisra_TypeCast(&pAmbaVoutTop_Reg, &TempAddr);
    }

    // pAmbaVoutTvEnc_Reg
    TempAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,vout_tvenc", "reg", 0U);
    if (TempAddr == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        TempAddr = TempAddr | AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR;
#endif
        AmbaMisra_TypeCast(&pAmbaVoutTvEnc_Reg, &TempAddr);
    }

    // pAmbaVoutDisplay0_Reg
    TempAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,vout_display0", "reg", 0U);
    if (TempAddr == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        TempAddr = TempAddr | AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR;
#endif
        AmbaMisra_TypeCast(&pAmbaVoutDisplay0_Reg, &TempAddr);
    }

#if !defined(CONFIG_SOC_CV28)
    // pAmbaVoutDisplay1_Reg
    TempAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,vout_display1", "reg", 0U);
    if (TempAddr == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        TempAddr = TempAddr | AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR;
#endif
        AmbaMisra_TypeCast(&pAmbaVoutDisplay1_Reg, &TempAddr);
    }
#endif  //#if !defined(CONFIG_SOC_CV28)

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    // pAmbaVoutDisplay2_Reg
    TempAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,vout_display2", "reg", 0U);
    if (TempAddr == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        TempAddr = TempAddr | AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaVoutDisplay2_Reg, &TempAddr);
    }

    // pAmbaVoutMipiDsiCmd0_Reg
    TempAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,vout_mipi_dsi_cmd0", "reg", 0U);
    if (TempAddr == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        TempAddr = TempAddr | AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaVoutMipiDsiCmd0_Reg, &TempAddr);
    }
    // pAmbaVoutMipiDsiCmd1_Reg
    TempAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,vout_mipi_dsi_cmd1", "reg", 0U);
    if (TempAddr == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        TempAddr = TempAddr | AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaVoutMipiDsiCmd1_Reg, &TempAddr);
    }
#else
#if !defined(CONFIG_SOC_H32)
    // pAmbaVoutMipiDsiCmd_Reg
    TempAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,vout_mipi_dsi_cmd0", "reg", 0U);
    if (TempAddr == 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        AmbaMisra_TypeCast(&pAmbaVoutMipiDsiCmd_Reg, &TempAddr);
    }
#endif
#endif  //#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)

#else
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    BaseAddr = AMBA_CORTEX_A76_VOUT_BASE_ADDR;
#else
    BaseAddr = AMBA_CORTEX_A53_VOUT_BASE_ADDR;
#endif
    // pAmbaVout_Reg
    AmbaMisra_TypeCast(&pAmbaVout_Reg, &BaseAddr);
    // pAmbaVoutTvEnc_Reg
    AmbaMisra_TypeCast(&pAmbaVoutTvEnc_Reg, &BaseAddr);
    // pAmbaVoutMixer0_Reg
    TempAddr = BaseAddr + VOUT_MIXER0_REG_OFFSET;
    AmbaMisra_TypeCast(&pAmbaVoutMixer0_Reg, &TempAddr);
    // pAmbaVoutDisplay0_Reg
    TempAddr = BaseAddr + VOUT_DISPLAY0_REG_OFFSET;
    AmbaMisra_TypeCast(&pAmbaVoutDisplay0_Reg, &TempAddr);
#if !defined(CONFIG_SOC_CV28)
    // pAmbaVoutMixer1_Reg
    TempAddr = BaseAddr + VOUT_MIXER1_REG_OFFSET;
    AmbaMisra_TypeCast(&pAmbaVoutMixer1_Reg, &TempAddr);
    // pAmbaVoutDisplay1_Reg
    TempAddr = BaseAddr + VOUT_DISPLAY1_REG_OFFSET;
    AmbaMisra_TypeCast(&pAmbaVoutDisplay1_Reg, &TempAddr);
#endif
    // pAmbaVoutOsdRescale_Reg
    TempAddr = BaseAddr + VOUT_OSD_REG_OFFSET;
    AmbaMisra_TypeCast(&pAmbaVoutOsdRescale_Reg, &TempAddr);
    // pAmbaVoutTop_Reg
    TempAddr = BaseAddr + VOUT_TOP_REG_OFFSET;
    AmbaMisra_TypeCast(&pAmbaVoutTop_Reg, &TempAddr);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    // pAmbaVoutMipiDsiCmd0_Reg
    TempAddr = BaseAddr + VOUT_DSI_CMD0_REG_OFFSET;
    AmbaMisra_TypeCast(&pAmbaVoutMipiDsiCmd0_Reg, &TempAddr);
    // pAmbaVoutDisplay2_Reg
    TempAddr = BaseAddr + VOUT_DISPLAY2_REG_OFFSET;
    AmbaMisra_TypeCast(&pAmbaVoutDisplay2_Reg, &TempAddr);
    // pAmbaVoutMipiDsiCmd1_Reg
    TempAddr = BaseAddr + VOUT_DSI_CMD1_REG_OFFSET;
    AmbaMisra_TypeCast(&pAmbaVoutMipiDsiCmd1_Reg, &TempAddr);
#else
#if !defined(CONFIG_SOC_H32)
    // pAmbaVoutMipiDsiCmd_Reg
    TempAddr = BaseAddr + VOUT_DSI_CMD0_REG_OFFSET;
    AmbaMisra_TypeCast(&pAmbaVoutMipiDsiCmd_Reg, &TempAddr);
#endif
#endif  //#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#endif  //#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    return RetVal;
}
#endif  //#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX))

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
    UINT16 VoutCscData[VOUT_CSC_DATA_COUNT];

    /* Create Mutex */
    if (AmbaKAL_MutexCreate(&AmbaVoutMutex, AmbaVoutMutexName) != KAL_ERR_NONE) {
        RetVal = VOUT_ERR_MUTEX;    /* should never happen */
    } else {
#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX))
        RetVal = VOUT_MemAddrMap();
#endif
        if (RetVal == VOUT_ERR_NONE) {
            (void)AmbaRTSL_Vout0InitDispConfig();
#if !defined(CONFIG_SOC_CV28)
            (void)AmbaRTSL_Vout1InitDispConfig();
#endif
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            (void)AmbaRTSL_Vout2InitDispConfig();
#endif
            (void)AmbaRTSL_VoutInitTvEncConfig();

#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
            /* Set default Vout0 csc by digital csc: YCC(Limited) to RGB(Full) */
            (void)AmbaVout_GetCscMatrix(CSC_TYPE_BT601, 1U, &pCscMatrix);
            VOUT_CalculateCsc(pCscMatrix, VoutCscData);
            (void)AmbaRTSL_VoutDigiRgbSetCsc(VoutCscData, 0U);
#else
            /* Set default Vout0 csc by MIPI DSI csc: YCC(Limited) to RGB(Full) */
            (void)AmbaVout_GetCscMatrix(CSC_TYPE_BT601, 1U, &pCscMatrix);
            VOUT_CalculateCsc(pCscMatrix, VoutCscData);
            (void)AmbaRTSL_VoutMipiDsiSetCsc(VoutCscData, 0U);
#endif
#if !(defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32))
            /* Set default Vout1 csc by MIPI CSI csc: CSC_TYPE_IDENTITY */
            (void)AmbaVout_GetCscMatrix(CSC_TYPE_IDENTITY, 1U, &pCscMatrix);
            VOUT_CalculateCsc(pCscMatrix, VoutCscData);
            (void)AmbaRTSL_VoutMipiCsiSetCsc(VoutCscData, 0U);
#endif
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_ChannelSelect - Selects target vout channel to be programmed
 *  @param[in] VoutChannel Target vout channel
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_ChannelSelect(UINT32 VoutChannel)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 RetVal = VOUT_ERR_NONE;

    if (VoutChannel >= 2U) {
        /* Only Display A and B can be selected */
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            VoutChan = VoutChannel;
            RetVal = AmbaRTSL_VoutChannelSelect(VoutChannel);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)VoutChannel;
    return VOUT_ERR_INVALID_API;
#endif
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
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pVirtAddr == NULL) || (pSize == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout0GetDispConfigAddr(pVirtAddr);
            *pSize = AMBA_VOUT_DISP_BUF_SIZE;

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
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
#if !defined(CONFIG_SOC_CV28)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pVirtAddr == NULL) || (pSize == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout1GetDispConfigAddr(pVirtAddr);
            *pSize = AMBA_VOUT_DISP_BUF_SIZE;

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pVirtAddr);
    AmbaMisra_TouchUnused(pSize);
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_GetDisp2ConfigAddr - Get VOUT display 2 configuration
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pVirtAddr Memory address of the configuration
 *  @param[out] pSize Memory size of the configuration
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetDisp2ConfigAddr(ULONG *pVirtAddr, UINT32 *pSize)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pVirtAddr == NULL) || (pSize == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout2GetDispConfigAddr(pVirtAddr);
            *pSize = AMBA_VOUT_DISP_BUF_SIZE;

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pVirtAddr);
    AmbaMisra_TouchUnused(pSize);
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_GetTvEncAddr - Get VOUT display DVE configuration
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pVirtAddr Memory address of the configuration
 *  @param[out] pSize Memory size of the configuration
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetTvEncAddr(ULONG *pVirtAddr, UINT32 *pSize)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pVirtAddr == NULL) || (pSize == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
#if defined(CONFIG_SOC_CV28)
            RetVal = AmbaRTSL_Vout0GetTvEncAddr(pVirtAddr);
#elif defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
            RetVal = AmbaRTSL_Vout1GetTvEncAddr(pVirtAddr);
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            RetVal = AmbaRTSL_Vout2GetTvEncAddr(pVirtAddr);
#else
            RetVal = VOUT_ERR_UNEXPECTED;
#endif
            *pSize = AMBA_VOUT_TVENC_BUF_SIZE;

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
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
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pVirtAddr == NULL) || (pSize == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout0GetDigitalCscAddr(pVirtAddr);
            *pSize = AMBA_VOUT_CSC_BUF_SIZE;

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
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
#if !(defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32))
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pVirtAddr == NULL) || (pSize == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout1GetDigitalCscAddr(pVirtAddr);
            *pSize = AMBA_VOUT_CSC_BUF_SIZE;

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pVirtAddr);
    AmbaMisra_TouchUnused(pSize);
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_GetDisp0AnalogCscAddr - Get VOUT display 0 analog csc configuration
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pVirtAddr Memory address of the configuration
 *  @param[out] pSize Memory size of the configuration
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetDisp0AnalogCscAddr(ULONG *pVirtAddr, UINT32 *pSize)
{
#if defined(CONFIG_SOC_CV28)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pVirtAddr == NULL) || (pSize == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout0GetAnalogCscAddr(pVirtAddr);
            *pSize = AMBA_VOUT_CSC_BUF_SIZE;

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pVirtAddr);
    AmbaMisra_TouchUnused(pSize);
    return VOUT_ERR_INVALID_API;
#endif
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
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pVirtAddr == NULL) || (pSize == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout1GetAnalogCscAddr(pVirtAddr);
            *pSize = AMBA_VOUT_CSC_BUF_SIZE;

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pVirtAddr);
    AmbaMisra_TouchUnused(pSize);
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_GetDisp2AnalogCscAddr - Get VOUT display 2 analog csc configuration
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pVirtAddr Memory address of the configuration
 *  @param[out] pSize Memory size of the configuration
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetDisp2AnalogCscAddr(ULONG *pVirtAddr, UINT32 *pSize)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pVirtAddr == NULL) || (pSize == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout2GetAnalogCscAddr(pVirtAddr);
            *pSize = AMBA_VOUT_CSC_BUF_SIZE;

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pVirtAddr);
    AmbaMisra_TouchUnused(pSize);
    return VOUT_ERR_INVALID_API;
#endif
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
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pVirtAddr == NULL) || (pSize == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout1GetHdmiCscAddr(pVirtAddr);
            *pSize = AMBA_VOUT_CSC_BUF_SIZE;

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pVirtAddr);
    AmbaMisra_TouchUnused(pSize);
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_GetDisp2HdmiCscAddr - Get VOUT display 2 hdmi csc configuration
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pVirtAddr Memory address of the configuration
 *  @param[out] pSize Memory size of the configuration
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetDisp2HdmiCscAddr(ULONG *pVirtAddr, UINT32 *pSize)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pVirtAddr == NULL) || (pSize == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout2GetHdmiCscAddr(pVirtAddr);
            *pSize = AMBA_VOUT_CSC_BUF_SIZE;

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pVirtAddr);
    AmbaMisra_TouchUnused(pSize);
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_SetDisp0VinVoutSync - Enable/Disable vin-vout sync function in vout display 0 configuration
 *  @param[in] EnableFlag Enable(1)/Disable(0) vin-vout sync function
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_SetDisp0VinVoutSync(UINT32 EnableFlag)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (EnableFlag > 1U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout0SetVinVoutSync(EnableFlag);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
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
#if !defined(CONFIG_SOC_CV28)
    UINT32 RetVal = VOUT_ERR_NONE;

    if (EnableFlag > 1U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout1SetVinVoutSync(EnableFlag);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)EnableFlag;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_SetDisp2VinVoutSync - Enable/Disable vin-vout sync function in vout display 2 configuration
 *  @param[in] EnableFlag Enable(1)/Disable(0) vin-vout sync function
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_SetDisp2VinVoutSync(UINT32 EnableFlag)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 RetVal = VOUT_ERR_NONE;

    if (EnableFlag > 1U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout2SetVinVoutSync(EnableFlag);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)EnableFlag;
    return VOUT_ERR_INVALID_API;
#endif
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
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 EvenLineColor = ColorOrder & 0xffffU;
    UINT32 OddLineColor = ColorOrder >> 16U;
#if defined(CONFIG_QNX)
    int fd;
    vout_config_t VoutClkCfg;
    UINT32 ActualFreq;
#endif

    if ((YccMode >= VOUT_NUM_YCC_MODE) ||
        (EvenLineColor >= VOUT_NUM_YCC_MODE_ORDER) ||
        (OddLineColor >= VOUT_NUM_YCC_MODE_ORDER) ||
        (pDataLatch == NULL) ||
        (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            /* Config Vout PLL */
#if defined(CONFIG_QNX)
            /* Set clock hint */
            fd = open("/dev/clock", O_RDWR);
            if (fd == -1) {
                RetVal = VOUT_ERR_OPEN_FILE;
            } else {
                VoutClkCfg.device = AMBA_SYS_VOUT_DEVICE_LCD;
                VoutClkCfg.Ratio = VOUT_DIGITAL_CLK_RATIO;
                VoutClkCfg.Type = AMBA_PLL_VOUT_LCD_GENERIC;

                if (devctl(fd, DCMD_CLOCK_CONFIG_VOUT, &VoutClkCfg, sizeof(VoutClkCfg), NULL) != 0) {
                    RetVal = VOUT_ERR_UNEXPECTED;
                }
                close(fd);
            }

            /* Set target clock */
            (void)AmbaSYS_SetIoClkFreq(AMBA_CLK_VOUTLCD, pDisplayTiming->PixelClkFreq, &ActualFreq);
#else
            (void)AmbaRTSL_PllSetVoutLcdClkHint(AMBA_PLL_VOUT_LCD_GENERIC, VOUT_DIGITAL_CLK_RATIO);
            (void)AmbaRTSL_PllSetVoutLcdClk(pDisplayTiming->PixelClkFreq);
#endif

            /* Enable Digital YCC */
#if defined(CONFIG_QNX)
            if (RetVal == VOUT_ERR_NONE) {
                RetVal = AmbaRTSL_VoutDigiYccEnable(YccMode, ColorOrder, pDataLatch, pDisplayTiming);
            }
#else
            RetVal = AmbaRTSL_VoutDigiYccEnable(YccMode, ColorOrder, pDataLatch, pDisplayTiming);
#endif

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)YccMode;
    (void)ColorOrder;
    (void)pDataLatch;
    (void)pDisplayTiming;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_DigiYccSetPhyCtrl - Adjust electrical signal quality of digital YCC interface
 *  @param[in] ParamID PHY control parameter id
 *  @param[in] ParamVal PHY control parameter value
 *  @return error code
 */
UINT32 AmbaVout_DigiYccSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;

    AmbaMisra_TouchUnused(&ParamVal);

    if (ParamID >= VOUT_NUM_PHY_PARAM) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutDigiYccSetPhyCtrl(ParamID, ParamVal);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)ParamID;
    (void)ParamVal;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_DigiYccGetStatus - Get vout status of digital YCC interface
 *  @param[out] pDigiYccConfig Pixel format configuration
 *  @param[out] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_DigiYccGetStatus(AMBA_VOUT_DIGITAL_YCC_CONFIG_s *pDigiYccConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pDigiYccConfig == NULL) && (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutDigiYccGetStatus(pDigiYccConfig, pDisplayTiming);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pDigiYccConfig);
    AmbaMisra_TouchUnused(pDisplayTiming);
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_DigiYccSetCscMatrix - Set color space conversion matrix
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] YccMode Bit depth information
 *  @return error code
 */
UINT32 AmbaVout_DigiYccSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 YccMode)
{
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT16 VoutCscData[VOUT_CSC_DATA_COUNT];

    if ((pCscMatrix == NULL) || (YccMode >= VOUT_NUM_YCC_MODE)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            VOUT_CalculateCsc(pCscMatrix, VoutCscData);
            RetVal = AmbaRTSL_VoutDigiYccSetCsc(VoutCscData, YccMode);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)pCscMatrix;
    (void)YccMode;
    return VOUT_ERR_INVALID_API;
#endif
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
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 EvenLineColor = ColorOrder & 0xffffU;
    UINT32 OddLineColor = ColorOrder >> 16U;
#if defined(CONFIG_QNX)
    int fd;
    vout_config_t VoutClkCfg;
    UINT32 ActualFreq;
#endif

    if ((RgbMode >= VOUT_NUM_RGB_MODE) ||
        (EvenLineColor >= VOUT_NUM_RGB_MODE_ORDER) ||
        (OddLineColor >= VOUT_NUM_RGB_MODE_ORDER) ||
        (pDataLatch == NULL) ||
        (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            /* Config Vout PLL */
#if defined(CONFIG_QNX)
            /* Set clock hint */
            fd = open("/dev/clock", O_RDWR);
            if (fd == -1) {
                RetVal = VOUT_ERR_OPEN_FILE;
            } else {
                VoutClkCfg.device = AMBA_SYS_VOUT_DEVICE_LCD;
                VoutClkCfg.Ratio = VOUT_DIGITAL_CLK_RATIO;
                VoutClkCfg.Type = AMBA_PLL_VOUT_LCD_GENERIC;
                if (devctl(fd, DCMD_CLOCK_CONFIG_VOUT, &VoutClkCfg, sizeof(VoutClkCfg), NULL) != 0) {
                    RetVal = VOUT_ERR_UNEXPECTED;
                }
                close(fd);
            }

            /* Set target clock */
            (void)AmbaSYS_SetIoClkFreq(AMBA_CLK_VOUTLCD, pDisplayTiming->PixelClkFreq, &ActualFreq);
#else
            (void)AmbaRTSL_PllSetVoutLcdClkHint(AMBA_PLL_VOUT_LCD_GENERIC, VOUT_DIGITAL_CLK_RATIO);
            (void)AmbaRTSL_PllSetVoutLcdClk(pDisplayTiming->PixelClkFreq);
#endif

            /* Enable Digital RGB */
#if defined(CONFIG_QNX)
            if (RetVal == VOUT_ERR_NONE) {
                RetVal = AmbaRTSL_VoutDigiRgbEnable(RgbMode, ColorOrder, pDataLatch, pDisplayTiming);
            }
#else
            RetVal = AmbaRTSL_VoutDigiRgbEnable(RgbMode, ColorOrder, pDataLatch, pDisplayTiming);
#endif

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)RgbMode;
    (void)ColorOrder;
    (void)pDataLatch;
    (void)pDisplayTiming;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_DigiRgbSetPhyCtrl - Adjust electrical signal quality of digital RGB interface
 *  @param[in] ParamID PHY control parameter id
 *  @param[in] ParamVal PHY control parameter value
 *  @return error code
 */
UINT32 AmbaVout_DigiRgbSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;

    AmbaMisra_TouchUnused(&ParamVal);

    if (ParamID >= VOUT_NUM_PHY_PARAM) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutDigiRgbSetPhyCtrl(ParamID, ParamVal);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)ParamID;
    (void)ParamVal;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_DigiRgbGetStatus - Get vout status of digital RGB interface
 *  @param[out] pDigiRgbConfig Pixel format configuration
 *  @param[out] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_DigiRgbGetStatus(AMBA_VOUT_DIGITAL_RGB_CONFIG_s *pDigiRgbConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pDigiRgbConfig == NULL) && (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutDigiRgbGetStatus(pDigiRgbConfig, pDisplayTiming);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pDigiRgbConfig);
    AmbaMisra_TouchUnused(pDisplayTiming);
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_DigiRgbSetCscMatrix - Set color space conversion matrix
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] RgbMode Bit depth information
 *  @return error code
 */
UINT32 AmbaVout_DigiRgbSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 RgbMode)
{
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT16 VoutCscData[VOUT_CSC_DATA_COUNT];

    if ((pCscMatrix == NULL) || (RgbMode >= VOUT_NUM_RGB_MODE)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            VOUT_CalculateCsc(pCscMatrix, VoutCscData);
            RetVal = AmbaRTSL_VoutDigiRgbSetCsc(VoutCscData, RgbMode);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)pCscMatrix;
    (void)RgbMode;
    return VOUT_ERR_INVALID_API;
#endif
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
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28)
    UINT32 RetVal = VOUT_ERR_NONE;
#if defined(CONFIG_QNX)
    int fd;
    vout_config_t VoutClkCfg;
    UINT32 ActualFreq;
#endif

    if ((FpdLinkMode >= VOUT_NUM_FPD_LINK_MODE) ||
        (ColorOrder >= VOUT_NUM_FPD_LINK_ORDER) ||
        (DataEnablePolarity >= VOUT_NUM_SIGNAL_MODE) ||
        (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            /* Config Vout PLL */
#if defined(CONFIG_QNX)
            /* Set clock hint */
            fd = open("/dev/clock", O_RDWR);
            if (fd == -1) {
                RetVal = VOUT_ERR_OPEN_FILE;
            } else {
                VoutClkCfg.device = AMBA_SYS_VOUT_DEVICE_LCD;
                VoutClkCfg.Ratio = VOUT_LVDS_CLK_RATIO;
                VoutClkCfg.Type = AMBA_PLL_VOUT_LCD_FPD_LINK;
                if (devctl(fd, DCMD_CLOCK_CONFIG_VOUT, &VoutClkCfg, sizeof(VoutClkCfg), NULL) != 0) {
                    RetVal = VOUT_ERR_UNEXPECTED;
                }
                close(fd);
            }

            /* Set target clock */
            (void)AmbaSYS_SetIoClkFreq(AMBA_CLK_VOUTLCD, pDisplayTiming->PixelClkFreq, &ActualFreq);
#else
            (void)AmbaRTSL_PllSetVoutLcdClkHint(AMBA_PLL_VOUT_LCD_FPD_LINK, VOUT_LVDS_CLK_RATIO);
            (void)AmbaRTSL_PllSetVoutLcdClk(pDisplayTiming->PixelClkFreq);
#endif

            /* Enable FPD-Link */
#if defined(CONFIG_QNX)
            if (RetVal == VOUT_ERR_NONE) {
                RetVal = AmbaRTSL_VoutFpdLinkEnable(FpdLinkMode, ColorOrder, DataEnablePolarity, pDisplayTiming);
            }
#else
            RetVal = AmbaRTSL_VoutFpdLinkEnable(FpdLinkMode, ColorOrder, DataEnablePolarity, pDisplayTiming);
#endif

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)FpdLinkMode;
    (void)ColorOrder;
    (void)DataEnablePolarity;
    (void)pDisplayTiming;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_FpdLinkSetPhyCtrl - Adjust electrical signal quality of FPD-Link interface
 *  @param[in] ParamID PHY control parameter id
 *  @param[in] ParamVal PHY control parameter value
 *  @return error code
 */
UINT32 AmbaVout_FpdLinkSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28)
    UINT32 RetVal = VOUT_ERR_NONE;

    AmbaMisra_TouchUnused(&ParamVal);

    if (ParamID >= VOUT_NUM_PHY_PARAM) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutFpdLinkSetPhyCtrl(ParamID, ParamVal);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)ParamID;
    (void)ParamVal;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_FpdLinkGetStatus - Get vout status of FPD-Link interface
 *  @param[out] pFpdLinkConfig Pixel format configuration
 *  @param[out] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_FpdLinkGetStatus(AMBA_VOUT_FPD_LINK_CONFIG_s *pFpdLinkConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pFpdLinkConfig == NULL) && (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutFpdLinkGetStatus(pFpdLinkConfig, pDisplayTiming);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pFpdLinkConfig);
    AmbaMisra_TouchUnused(pDisplayTiming);
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_FpdLinkSetCscMatrix - Set color space conversion matrix
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] FpdMode Bit depth information
 *  @return error code
 */
UINT32 AmbaVout_FpdLinkSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s * pCscMatrix, UINT32 FpdMode)
{
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28)
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT16 VoutCscData[VOUT_CSC_DATA_COUNT];

    if ((pCscMatrix == NULL) || (FpdMode >= VOUT_NUM_FPD_LINK_MODE)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            VOUT_CalculateCsc(pCscMatrix, VoutCscData);
            RetVal = AmbaRTSL_VoutFpdLinkSetCsc(VoutCscData, FpdMode);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)pCscMatrix;
    (void)FpdMode;
    return VOUT_ERR_INVALID_API;
#endif
}

#if !defined(CONFIG_SOC_H32)
/**
 *  BoundaryCheck - Check the boundary for mipi-phy parameters
 *  @param[in] PowerOfTwo The power of 2
 *  @param[in,out] Num The number to be checked
 */
static void BoundaryCheck(UINT32 PowerOfTwo, DOUBLE *pNum)
{
    DOUBLE UpperBound = 0.0;

    if (AmbaWrap_pow(2.0, (DOUBLE) PowerOfTwo, &UpperBound) == ERR_NONE) {
        if (*pNum > (UpperBound - 1.0)) {
            *pNum = (UpperBound - 1.0);
        } else if (*pNum == 0.0) {
            *pNum = 0.0;
        } else {
            *pNum = *pNum - 1.0;
        }
    }
}

/**
 *  VOUT_CalculateMphyConfig - Calculate mipi-phy parameters
 *  @param[in] BitRate MIPI data rate
 *  @param[out] pVoutMipiTiming MIPI phy data
 */
static void VOUT_CalculateMphyConfig(UINT32 BitRate, AMBA_VOUT_MIPI_TIMING_PARAM_s *pVoutMipiTiming)
{
    UINT32 RetVal;
    DOUBLE MipiBitRate;
    DOUBLE TxByteClkHS, UI;
    DOUBLE ClkPrepare, ClkPrepareMax, ClkPrepareSec;
    DOUBLE HsLpxMin;
    DOUBLE HsTrailMin, Temp;
    DOUBLE HsPrepareMax, HsPrepareSec;
    DOUBLE HsZeroMin, HsZeroMinSec;
    DOUBLE ClkZeroMinSec, ClkZeroMin;
    DOUBLE ClkTrailMin;
    DOUBLE InitTxMin;

    MipiBitRate = (DOUBLE)BitRate / 1e6;                // in MHz

    /********************************************************
     * TX (TxByteClkHS)
     ********************************************************/
    TxByteClkHS = (DOUBLE)MipiBitRate * 1e6 / 8.0;      // in Hz
    UI = (1.0 / (MipiBitRate * 1e6));                   // in seccond

    // ClkPrepare (use HsPrepare)
    ClkPrepareMax = 95e-9 * TxByteClkHS;
    RetVal = AmbaWrap_floor(ClkPrepareMax + 0.5, &ClkPrepareMax);

    ClkPrepareSec = (38e-9 + 95e-9) / 2.0;
    ClkPrepare = ClkPrepareSec * TxByteClkHS;
    RetVal |= AmbaWrap_floor(ClkPrepare + 0.5, &ClkPrepare);

    // ClkZero  (use 2 x HsZero)
    ClkZeroMinSec = (0.0 != TxByteClkHS) ? (300e-9 - (ClkPrepare / TxByteClkHS)) : 0.0;
    ClkZeroMin = ClkZeroMinSec * TxByteClkHS;
    RetVal |= AmbaWrap_ceil(ClkZeroMin, &ClkZeroMin);

    // ClkTrail
    ClkTrailMin = 60e-9 * TxByteClkHS;
    RetVal |= AmbaWrap_ceil(ClkTrailMin, &ClkTrailMin);
    BoundaryCheck(5, &ClkTrailMin);

    // HsPrepare
    HsPrepareMax = (85e-9 + (6.0 * UI)) * TxByteClkHS;
    RetVal |= AmbaWrap_floor(HsPrepareMax + 0.5, &HsPrepareMax);

    HsPrepareSec = (40e-9 + (4.0 * UI) + 85e-9 + (6.0 * UI)) / 2.0;

    if (HsPrepareMax > ClkPrepareMax) {
        HsPrepareMax = ClkPrepareMax;
    }
    BoundaryCheck(6, &HsPrepareMax);

    // HsZero
    HsZeroMinSec = 145e-9 + (10.0 * UI) - HsPrepareSec;
    HsZeroMin = HsZeroMinSec * TxByteClkHS;
    RetVal |= AmbaWrap_ceil(HsZeroMin, &HsZeroMin);

    if (HsZeroMin < (ClkZeroMin / 2.0)) {
        HsZeroMin = ClkZeroMin / 2.0;
    }
    BoundaryCheck(6, &HsZeroMin);

    // HsTrail
    Temp = 8.0 * UI;
    HsTrailMin = 60e-9 + (4.0 * UI);
    if (HsTrailMin < Temp) {
        HsTrailMin = Temp;
    }
    HsTrailMin = HsTrailMin * TxByteClkHS;
    RetVal |= AmbaWrap_ceil(HsTrailMin, &HsTrailMin);
    BoundaryCheck(5, &HsTrailMin);

    // HsLpx
    HsLpxMin = 50e-9 * TxByteClkHS;
    RetVal |= AmbaWrap_ceil(HsLpxMin, &HsLpxMin);
    BoundaryCheck(5, &HsLpxMin);

    // InitTx
    InitTxMin = 100e-6 * TxByteClkHS / 1024.0;
    RetVal |= AmbaWrap_ceil(InitTxMin, &InitTxMin);
    BoundaryCheck(6, &InitTxMin);

    if (RetVal == ERR_NONE) {
        /* Set the related values */
        pVoutMipiTiming->HsTrail = (UINT8)HsTrailMin;
        pVoutMipiTiming->ClkTrail = (UINT8)ClkTrailMin;
        pVoutMipiTiming->HsPrepare = (UINT8)HsPrepareMax;
        pVoutMipiTiming->ClkPrepare = (UINT8)HsPrepareMax;
        pVoutMipiTiming->HsZero = (UINT8)HsZeroMin;
        pVoutMipiTiming->ClkZero = (UINT8)HsZeroMin;
        pVoutMipiTiming->HsLpx = (UINT8)HsLpxMin;
        pVoutMipiTiming->InitTx = (UINT8)InitTxMin;
    } else {
        pVoutMipiTiming->HsTrail = 0U;
        pVoutMipiTiming->ClkTrail = 0U;
        pVoutMipiTiming->HsPrepare = 0U;
        pVoutMipiTiming->ClkPrepare = 0U;
        pVoutMipiTiming->HsZero = 0U;
        pVoutMipiTiming->ClkZero = 0U;
        pVoutMipiTiming->HsLpx = 0U;
        pVoutMipiTiming->InitTx = 0U;
    }
}

/**
 *  VOUT_GetMipiBytePerClk - Get MIPI byte per clock infomation
 *  @param[in] MipiRawMode MIPI raw mode
 *  @param[in] Interface MIPI Interface
 *  @return Byte Per Clock
 */
static UINT32 VOUT_GetMipiBytePerClk(UINT32 MipiRawMode, UINT32 Interface)
{
    UINT32 BytePerClock = 0U;

    if (Interface == VOUT_MIPI_INTERFACE_CSI) {
        switch (MipiRawMode) {
        case VOUT_MIPI_CSI_MODE_422_1LANE:
        case VOUT_MIPI_CSI_MODE_422_2LANE:
        case VOUT_MIPI_CSI_MODE_422_4LANE:
        case VOUT_MIPI_CSI_MODE_RAW8_1LANE:
        case VOUT_MIPI_CSI_MODE_RAW8_2LANE:
        case VOUT_MIPI_CSI_MODE_RAW8_4LANE:
            BytePerClock = 2U;
            break;
        default:
            BytePerClock = 3U;
            break;
        };
    } else if (Interface == VOUT_MIPI_INTERFACE_DSI) {
        switch (MipiRawMode) {
        case VOUT_MIPI_DSI_MODE_422_1LANE:
        case VOUT_MIPI_DSI_MODE_565_1LANE:
        case VOUT_MIPI_DSI_MODE_422_2LANE:
        case VOUT_MIPI_DSI_MODE_565_2LANE:
        case VOUT_MIPI_DSI_MODE_422_4LANE:
        case VOUT_MIPI_DSI_MODE_565_4LANE:
            BytePerClock = 2U;
            break;
        case VOUT_MIPI_DSI_MODE_666_1LANE:
        case VOUT_MIPI_DSI_MODE_888_1LANE:
        case VOUT_MIPI_DSI_MODE_666_2LANE:
        case VOUT_MIPI_DSI_MODE_888_2LANE:
        case VOUT_MIPI_DSI_MODE_666_4LANE:
        case VOUT_MIPI_DSI_MODE_888_4LANE:
        default:
            BytePerClock = 3U;
            break;
        };
    } else {
        /* Defualt Value */
        BytePerClock = 3U;
    }

    return BytePerClock;
}

/**
 *  VOUT_GetMipiLaneNum - Get MIPI lane num information
 *  @param[in] MipiRawMode MIPI raw mode
 *  @param[in] Interface MIPI Interface
 *  @return Lane number
 */
static UINT32 VOUT_GetMipiLaneNum(UINT32 MipiRawMode, UINT32 Interface)
{
    UINT32 NumMipiLane = 0U;

    if (Interface == VOUT_MIPI_INTERFACE_CSI) {
        switch (MipiRawMode) {
        case VOUT_MIPI_CSI_MODE_422_1LANE:
        case VOUT_MIPI_CSI_MODE_RAW8_1LANE:
            NumMipiLane = 1U;
            break;
        case VOUT_MIPI_CSI_MODE_422_2LANE:
        case VOUT_MIPI_CSI_MODE_RAW8_2LANE:
            NumMipiLane = 2U;
            break;
        case VOUT_MIPI_CSI_MODE_422_4LANE:
        case VOUT_MIPI_CSI_MODE_RAW8_4LANE:
        default:
            NumMipiLane = 4U;
            break;
        };
    } else if (Interface == VOUT_MIPI_INTERFACE_DSI) {
        switch (MipiRawMode) {
        case VOUT_MIPI_DSI_MODE_422_1LANE:
        case VOUT_MIPI_DSI_MODE_565_1LANE:
        case VOUT_MIPI_DSI_MODE_666_1LANE:
        case VOUT_MIPI_DSI_MODE_888_1LANE:
            NumMipiLane = 1U;
            break;
        case VOUT_MIPI_DSI_MODE_422_2LANE:
        case VOUT_MIPI_DSI_MODE_565_2LANE:
        case VOUT_MIPI_DSI_MODE_666_2LANE:
        case VOUT_MIPI_DSI_MODE_888_2LANE:
            NumMipiLane = 2U;
            break;
        case VOUT_MIPI_DSI_MODE_422_4LANE:
        case VOUT_MIPI_DSI_MODE_565_4LANE:
        case VOUT_MIPI_DSI_MODE_666_4LANE:
        case VOUT_MIPI_DSI_MODE_888_4LANE:
        default:
            NumMipiLane = 4U;
            break;
        };
    } else {
        /* Defualt Value */
        NumMipiLane = 4U;
    }

    return NumMipiLane;
}

/**
 *  VOUT_CalculateMipiDsiTiming - Assign video timing parameters for MIPI DSI output
 *  @param[in] pDisplayTiming Video timing parameters
 *  @param[in] BytePerClock Number of bytes per clock cycle
 *  @param[out] pNewHFP Modified new Hsync front porch
 *  @param[out] pNewVFP Modified new Vsync front porch
 */
static void VOUT_CalculateMipiDsiTiming(const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming, const UINT32 BytePerClock, UINT16 *pNewHFP, UINT16 *pNewVFP)
{
    UINT32 RetVal;
    UINT16 NewActiveStartX, NewActiveStartY;
    UINT16 ActiveStartX = pDisplayTiming->HsyncFrontPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch;
    UINT16 ActiveStartY = pDisplayTiming->VsyncFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch;
    UINT8 Byte4Clock, Byte2Clock;
    UINT16 Overhead, TempInt;
    DOUBLE TempDouble;
    UINT16 ActiveWidthBound, ActiveHeightBound;
    UINT16 Htotal = pDisplayTiming->HsyncFrontPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels;
    UINT16 Vtotal = pDisplayTiming->VsyncFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines;

    if (BytePerClock == 3U) {
        Byte4Clock = 1U;
        Byte2Clock = 1U;
    } else {
        Byte4Clock = 2U;
        Byte2Clock = 1U;
    }

    /* Calculate new Hsync front porch */
    TempInt = (UINT16) 6U * Byte4Clock;
    TempInt += (UINT16) 4U * Byte2Clock;
    TempInt += 6U;
    ActiveWidthBound = Htotal - TempInt;

    Overhead = (UINT16) 5U * Byte4Clock;
    Overhead += (UINT16) 2U * Byte2Clock;
    Overhead += 5U;

    TempDouble =  (DOUBLE)ActiveWidthBound - (DOUBLE)pDisplayTiming->ActivePixels;
    TempDouble = TempDouble / 3.0;
    RetVal = AmbaWrap_ceil(TempDouble, &TempDouble);

    if (RetVal == ERR_NONE) {
        NewActiveStartX = Overhead + (UINT16)((UINT16)TempDouble * 2U) - 20U;
        *pNewHFP = pDisplayTiming->HsyncFrontPorch - (ActiveStartX - NewActiveStartX);
    } else {
        /* Should never happen! */
        *pNewHFP = 0U;
    }

    /* Calculate new Vsync front porch */
    ActiveHeightBound = Vtotal - 6U;
    NewActiveStartY = ActiveHeightBound - pDisplayTiming->ActiveLines;
    NewActiveStartY = NewActiveStartY >> 1U;
    NewActiveStartY += 5U;
    *pNewVFP = pDisplayTiming->VsyncFrontPorch - (ActiveStartY - NewActiveStartY);
}

static UINT32 VOUT_ConfigMipiClk(UINT32 Interface, UINT32 ClkRatio, UINT32 PixelClkFreq)
{
    UINT32 RetVal = VOUT_ERR_NONE;
#if defined(CONFIG_QNX)
    int fd;
    vout_config_t VoutClkCfg;
    clk_freq_t ClkFreq;
#endif

    if (Interface == VOUT_MIPI_INTERFACE_CSI) {
        /* Set clock hint and target clock */
#if defined(CONFIG_QNX)
        fd = open("/dev/clock", O_RDWR);
        if (fd == -1) {
            RetVal = VOUT_ERR_OPEN_FILE;
        } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            switch (VoutChan) {
            case AMBA_VOUT_CHANNEL0:
                VoutClkCfg.VoutID = AMBA_CLK_VOUTA;
                ClkFreq.id = AMBA_CLK_VOUTA;
                break;
            case AMBA_VOUT_CHANNEL1:
                VoutClkCfg.VoutID = AMBA_CLK_VOUTB;
                ClkFreq.id = AMBA_CLK_VOUTB;
                break;
            default:
                VoutClkCfg.VoutID = AMBA_CLK_VOUTA;
                ClkFreq.id = AMBA_CLK_VOUTA;
                break;
            }
#else
            VoutClkCfg.device = AMBA_SYS_VOUT_DEVICE_TV;
            ClkFreq.id = AMBA_CLK_VOUTTV;
#endif
            VoutClkCfg.Ratio = ClkRatio;
#if defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            VoutClkCfg.Type = AMBA_PLL_VOUT_LCD_MIPI_CSI;
#else
            VoutClkCfg.Type = AMBA_PLL_VOUT_TV_MIPI_CSI;
#endif
            ClkFreq.freq = PixelClkFreq;

            if (devctl(fd, DCMD_CLOCK_CONFIG_VOUT, &VoutClkCfg, sizeof(VoutClkCfg), NULL) != 0) {
                RetVal = VOUT_ERR_DEV_CTL;
            } else {
                if (devctl(fd, DCMD_CLOCK_SET_FREQ, &ClkFreq, sizeof(ClkFreq), NULL) != 0) {
                    RetVal = VOUT_ERR_DEV_CTL;
                }
            }

            close(fd);
        }
#else
/* Non-QNX */
#if defined(CONFIG_SOC_CV28)
        (void)AmbaRTSL_PllSetVoutLcdClkHint(AMBA_PLL_VOUT_LCD_MIPI_CSI, ClkRatio);
        (void)AmbaRTSL_PllSetVoutLcdClk(PixelClkFreq);
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        switch (VoutChan) {
        case AMBA_VOUT_CHANNEL0:
            (void)AmbaRTSL_PllSetVoutAClkHint(AMBA_PLL_VOUT_LCD_MIPI_CSI, ClkRatio);
            (void)AmbaRTSL_PllSetVoutAClk(PixelClkFreq);
            break;
        case AMBA_VOUT_CHANNEL1:
            (void)AmbaRTSL_PllSetVoutBClkHint(AMBA_PLL_VOUT_LCD_MIPI_CSI, ClkRatio);
            (void)AmbaRTSL_PllSetVoutBClk(PixelClkFreq);
            break;
        default:
            (void)AmbaRTSL_PllSetVoutAClkHint(AMBA_PLL_VOUT_LCD_MIPI_CSI, ClkRatio);
            (void)AmbaRTSL_PllSetVoutAClk(PixelClkFreq);
            break;
        }
#else
        (void)AmbaRTSL_PllSetVoutTvClkHint(AMBA_PLL_VOUT_TV_MIPI_CSI, ClkRatio);
        (void)AmbaRTSL_PllSetVoutTvClk(PixelClkFreq);
#endif
#endif
    } else if (Interface == VOUT_MIPI_INTERFACE_DSI) {
        /* Set clock hint and target clock */
#if defined(CONFIG_QNX)
        fd = open("/dev/clock", O_RDWR);
        if (fd == -1) {
            RetVal = VOUT_ERR_OPEN_FILE;
        } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            switch (VoutChan) {
            case AMBA_VOUT_CHANNEL0:
                VoutClkCfg.VoutID = AMBA_CLK_VOUTA;
                ClkFreq.id = AMBA_CLK_VOUTA;
                break;
            case AMBA_VOUT_CHANNEL1:
                VoutClkCfg.VoutID = AMBA_CLK_VOUTB;
                ClkFreq.id = AMBA_CLK_VOUTB;
                break;
            default:
                VoutClkCfg.VoutID = AMBA_CLK_VOUTA;
                ClkFreq.id = AMBA_CLK_VOUTA;
                break;
            }
#else
            VoutClkCfg.device = AMBA_SYS_VOUT_DEVICE_LCD;
            ClkFreq.id = AMBA_CLK_VOUTLCD;
#endif
            VoutClkCfg.Ratio = ClkRatio;
            VoutClkCfg.Type = AMBA_PLL_VOUT_LCD_MIPI_DSI;
            ClkFreq.freq = PixelClkFreq;

            if (devctl(fd, DCMD_CLOCK_CONFIG_VOUT, &VoutClkCfg, sizeof(VoutClkCfg), NULL) != 0) {
                RetVal = VOUT_ERR_DEV_CTL;
            } else {
                if (devctl(fd, DCMD_CLOCK_SET_FREQ, &ClkFreq, sizeof(ClkFreq), NULL) != 0) {
                    RetVal = VOUT_ERR_DEV_CTL;
                }
            }

            close(fd);
        }
#else
/* Non-QNX */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        switch (VoutChan) {
        case AMBA_VOUT_CHANNEL0:
            (void)AmbaRTSL_PllSetVoutAClkHint(AMBA_PLL_VOUT_LCD_MIPI_DSI, ClkRatio);
            (void)AmbaRTSL_PllSetVoutAClk(PixelClkFreq);
            break;
        case AMBA_VOUT_CHANNEL1:
            (void)AmbaRTSL_PllSetVoutBClkHint(AMBA_PLL_VOUT_LCD_MIPI_DSI, ClkRatio);
            (void)AmbaRTSL_PllSetVoutBClk(PixelClkFreq);
            break;
        default:
            (void)AmbaRTSL_PllSetVoutAClkHint(AMBA_PLL_VOUT_LCD_MIPI_DSI, ClkRatio);
            (void)AmbaRTSL_PllSetVoutAClk(PixelClkFreq);
            break;
        }
#else
        (void)AmbaRTSL_PllSetVoutLcdClkHint(AMBA_PLL_VOUT_LCD_MIPI_DSI, ClkRatio);
        (void)AmbaRTSL_PllSetVoutLcdClk(PixelClkFreq);
#endif
#endif
    } else {
        RetVal = VOUT_ERR_ARG;
    }

    return RetVal;
}

/**
 *  VOUT_CheckHorizontalSync - CHECK HORIZONTAL SYNC CASES
 *  @param[in] SyncDis
 *  @param[in] DisMin
 *  @param[in] DisMax
 *  @param[in] HBPDis
 *  @param[in] HFPDis
 *  @return AllHS if either HS case is matched
 */
static UINT8 VOUT_CheckHorizontalSync(UINT16 SyncDis, UINT16 DisMin, UINT16 DisMax, UINT16 HBPDis, UINT16 HFPDis)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 RandomBetween0_1 = 0U;
    UINT32 MustUseSyncBlank, MustUseHBPBlank, MustUseHFPBlank, AllHSCase1, AllHSCase2;
    UINT32 SyncUseBlank, SendSyncEnd, MipiHBPUseBlank, MipiHFPUseBlank;
    UINT8 AllHS = 0U;

    RetVal |= AmbaWrap_srand(0xA5);
    RetVal |= AmbaWrap_rand(&RandomBetween0_1);

    if (RetVal == ERR_NONE) {
        MustUseSyncBlank = (SyncDis < DisMin) ? 1U : ((SyncDis > DisMax) ? 0U : (RandomBetween0_1 % 2U));
        SyncUseBlank = (MustUseSyncBlank == 1U) ? 1U : 0U;
        SendSyncEnd = (SyncUseBlank == 1U) ? 1U : (RandomBetween0_1 % 2U);

        MustUseHBPBlank = (HBPDis < DisMin) ? 1U : ((HBPDis > DisMax) ? 0U : (RandomBetween0_1 % 2U));
        MipiHBPUseBlank = (MustUseHBPBlank == 1U) ? 1U : (RandomBetween0_1 % 2U);

        MustUseHFPBlank = (HFPDis < DisMin) ? 1U : ((HFPDis > DisMax) ? 0U : (RandomBetween0_1 % 2U));
        MipiHFPUseBlank = (MustUseHFPBlank == 1U) ? 1U : (RandomBetween0_1 % 2U);

        AllHSCase1 = ((SendSyncEnd == 1U) && (MipiHBPUseBlank == 1U) && (MipiHFPUseBlank == 1U)) ? 1U : 0U;
        AllHSCase2 = ((SendSyncEnd == 0U) && (MipiHBPUseBlank == 1U) && (MipiHFPUseBlank == 1U)) ? 1U : 0U;

        if ((AllHSCase1 == 1U) || (AllHSCase2 == 1U)) {
            AllHS = 1U;
        } else {
            AllHS = 0U;
        }
    }
    return AllHS;
}

/**
 *  VOUT_CheckCalculatedWidth - CHECK IF THE CALCULATED WIDTH EQUALS TO THE FRAME WIDTH
 *  @param[in] BytePerClock Number of bytes per pixel clock
 *  @param[in] pFrameTiming VOUT frame timing parameters
 *  @param[out] pDisplayTiming Video timing parameters
 *  @param[out] pBlankPacket DSI Blank Packet length parameters
 *  @return error code
 */
static UINT32 VOUT_CheckCalculatedWidth(UINT32 BytePerClock, const AMBA_VOUT_FRAME_TIMING_CONFIG_s *pFrameTiming,
        const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming, AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s *pBlankPacket)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT16 HSyncStart, HSyncActive, HSyncEnd, HBackPorch, PacketHeader, Payload, PacketFooter, HFrontPorch, TotalWidth;
    DOUBLE CeilVal;

    /* Check 1: Number of bytes */
    HSyncStart = 4U;
    HSyncEnd = 4U;
    PacketHeader = 4U;
    PacketFooter = 2U;
    Payload = pDisplayTiming->ActivePixels * (UINT16)BytePerClock;
    HSyncActive = PacketHeader + (UINT16)pBlankPacket->LenHSA + PacketFooter;
    HBackPorch  = PacketHeader + (UINT16)pBlankPacket->LenHBP + PacketFooter;
    HFrontPorch = PacketHeader + (UINT16)pBlankPacket->LenHFP + PacketFooter;

    TotalWidth = HSyncStart + HSyncActive + HSyncEnd + HBackPorch + PacketHeader + Payload + PacketFooter + HFrontPorch;

    if (TotalWidth > ((UINT16)BytePerClock * pFrameTiming->Htotal)) {
        pBlankPacket->LenHBP -= (((UINT32)TotalWidth - (BytePerClock * (UINT32)pFrameTiming->Htotal)) / 2U);
        pBlankPacket->LenHFP -= (((UINT32)TotalWidth - (BytePerClock * (UINT32)pFrameTiming->Htotal)) / 2U);
    } else if (TotalWidth < ((UINT16)BytePerClock * pFrameTiming->Htotal)) {
        pBlankPacket->LenHBP += (((BytePerClock * (UINT32)pFrameTiming->Htotal) - (UINT32)TotalWidth) / 2U);
        pBlankPacket->LenHFP += (((BytePerClock * (UINT32)pFrameTiming->Htotal) - (UINT32)TotalWidth) / 2U);
    } else {
        /* Fix MISRAC violation */
    }

    /* Check 2: Number of cycles (2 bytes/pixel) */
    if (BytePerClock == 2U) {
        HSyncStart = 2U;
        HSyncEnd = 2U;
        PacketHeader = 2U;
        PacketFooter = 1U;
        Payload = pDisplayTiming->ActivePixels;

        if (AmbaWrap_ceil((DOUBLE)pBlankPacket->LenHSA / (DOUBLE)BytePerClock, &CeilVal) != ERR_NONE) {
            RetVal |= VOUT_ERR_UNEXPECTED;
        }
        HSyncActive = PacketHeader + (UINT16)CeilVal + PacketFooter;

        if (AmbaWrap_ceil((DOUBLE)pBlankPacket->LenHBP / (DOUBLE)BytePerClock, &CeilVal) != ERR_NONE) {
            RetVal |= VOUT_ERR_UNEXPECTED;
        }
        HBackPorch  = PacketHeader + (UINT16)CeilVal + PacketFooter;

        if (AmbaWrap_ceil((DOUBLE)pBlankPacket->LenHFP / (DOUBLE)BytePerClock, &CeilVal) != ERR_NONE) {
            RetVal |= VOUT_ERR_UNEXPECTED;
        }
        HFrontPorch = PacketHeader + (UINT16)CeilVal + PacketFooter;

        TotalWidth = HSyncStart + HSyncActive + HSyncEnd + HBackPorch + PacketHeader + Payload + PacketFooter + HFrontPorch;

        if (TotalWidth > pFrameTiming->Htotal) {
            pBlankPacket->LenHBP -= (((UINT32)TotalWidth - (UINT32)pFrameTiming->Htotal) / 2U);
            pBlankPacket->LenHFP -= (((UINT32)TotalWidth - (UINT32)pFrameTiming->Htotal) / 2U);
        } else if (TotalWidth < pFrameTiming->Htotal) {
            pBlankPacket->LenHBP += (((UINT32)pFrameTiming->Htotal - (UINT32)TotalWidth) / 2U);
            pBlankPacket->LenHFP += (((UINT32)pFrameTiming->Htotal - (UINT32)TotalWidth) / 2U);
        } else {
            /* Fix MISRAC violation */
        }
    }

    /* Check 3: Number of cycles (3 bytes/pixel) */
    if (BytePerClock == 3U) {
        HSyncStart = 1U;
        HSyncEnd = 1U;
        PacketHeader = 1U;
        PacketFooter = 1U;
        Payload = pDisplayTiming->ActivePixels;

        if (AmbaWrap_ceil((DOUBLE)pBlankPacket->LenHSA / (DOUBLE)BytePerClock, &CeilVal) != ERR_NONE) {
            RetVal |= VOUT_ERR_UNEXPECTED;
        }
        HSyncActive = PacketHeader + (UINT16)CeilVal + PacketFooter;

        if (AmbaWrap_ceil((DOUBLE)pBlankPacket->LenHBP / (DOUBLE)BytePerClock, &CeilVal) != ERR_NONE) {
            RetVal |= VOUT_ERR_UNEXPECTED;
        }
        HBackPorch  = PacketHeader + (UINT16)CeilVal + PacketFooter;

        if (AmbaWrap_ceil((DOUBLE)pBlankPacket->LenHFP / (DOUBLE)BytePerClock, &CeilVal) != ERR_NONE) {
            RetVal |= VOUT_ERR_UNEXPECTED;
        }
        HFrontPorch = PacketHeader + (UINT16)CeilVal + PacketFooter;

        TotalWidth = HSyncStart + HSyncActive + HSyncEnd + HBackPorch + PacketHeader + Payload + PacketFooter + HFrontPorch;

        if (TotalWidth > pFrameTiming->Htotal) {
            pBlankPacket->LenHBP -= (((UINT32)TotalWidth - (UINT32)pFrameTiming->Htotal) / 2U);
            pBlankPacket->LenHFP -= (((UINT32)TotalWidth - (UINT32)pFrameTiming->Htotal) / 2U);
        } else if (TotalWidth < pFrameTiming->Htotal) {
            pBlankPacket->LenHBP += (((UINT32)pFrameTiming->Htotal - (UINT32)TotalWidth) / 2U);
            pBlankPacket->LenHFP += (((UINT32)pFrameTiming->Htotal - (UINT32)TotalWidth) / 2U);
        } else {
            /* Fix MISRAC violation */
        }
    }
    return RetVal;
}
#endif

/**
 *  AmbaVout_MipiCsiEnable - Configuration VOUT display interface as MIPI CSI-2 output
 *  @param[in] MipiCsiMode MIPI CSI-2 pixel format
 *  @param[in] ColorOrder Color order selection of a pixel
 *  @param[in] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_MipiCsiEnable(UINT32 MipiCsiMode, UINT32 ColorOrder, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
#if !defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 ClkRatio, BytePerClock, MipiLaneNum;
    UINT32 MipiCsiRawMode = AmbaVout_MipiGetRawMode(MipiCsiMode);
    AMBA_VOUT_MIPI_TIMING_PARAM_s VoutMipiTiming;
#if defined(CONFIG_LINUX) && (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52))
    int fd;
    hdmi_config_msg_t HdmiConfigMsg;
#endif

    if ((MipiCsiRawMode >= VOUT_NUM_MIPI_CSI_MODE) ||
        (ColorOrder >= VOUT_NUM_MIPI_CSI_MODE_ORDER) ||
        (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            BytePerClock = VOUT_GetMipiBytePerClk(MipiCsiRawMode, VOUT_MIPI_INTERFACE_CSI);
            MipiLaneNum = VOUT_GetMipiLaneNum(MipiCsiRawMode, VOUT_MIPI_INTERFACE_CSI);
            ClkRatio = BytePerClock * 8U / MipiLaneNum;

            /* Config Vout PLL */
            RetVal = VOUT_ConfigMipiClk(VOUT_MIPI_INTERFACE_CSI, ClkRatio, pDisplayTiming->PixelClkFreq);

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            if (RetVal == VOUT_ERR_NONE) {
                /* VoutB use pll_hdmi, need to turn on hdmi power */
                if (VoutChan == AMBA_VOUT_CHANNEL1) {
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
                }
            }
#endif
            /* Set MIPI PHY according to bit rate */
            if (RetVal == VOUT_ERR_NONE) {
                VOUT_CalculateMphyConfig(pDisplayTiming->PixelClkFreq * ClkRatio, &VoutMipiTiming);
                RetVal = AmbaRTSL_VoutConfigMipiPhy(&VoutMipiTiming);
            }

            /* Enable MIPI CSI */
            if (RetVal == VOUT_ERR_NONE) {
                RetVal = AmbaRTSL_VoutMipiCsiEnable(MipiCsiMode, ColorOrder, MipiLaneNum, pDisplayTiming);
            }

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)MipiCsiMode;
    (void)ColorOrder;
    (void)pDisplayTiming;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_MipiCsiSetPhyCtrl - Adjust electrical signal quality of MIPI CSI-2 interface
 *  @param[in] ParamID PHY control parameter id
 *  @param[in] ParamVal PHY control parameter value
 *  @return error code
 */
UINT32 AmbaVout_MipiCsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
#if !defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((ParamID >= VOUT_NUM_PHY_PARAM) || (ParamVal >= VOUT_NUM_MIPI_DPHY_CLK_MODE)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiCsiSetPhyCtrl(ParamID, ParamVal);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)ParamID;
    (void)ParamVal;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_MipiCsiGetStatus - Get vout status of MIPI CSI-2 interface
 *  @param[out] pMipiCsiConfig Pixel format configuration
 *  @param[out] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_MipiCsiGetStatus(AMBA_VOUT_MIPI_CSI_CONFIG_s *pMipiCsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
#if !defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pMipiCsiConfig == NULL) && (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiCsiGetStatus(pMipiCsiConfig, pDisplayTiming);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pMipiCsiConfig);
    AmbaMisra_TouchUnused(pDisplayTiming);
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_MipiCsiSetCscMatrix - Set color space conversion matrix
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] MipiCsiMode Bit depth information
 *  @return error code
 */
UINT32 AmbaVout_MipiCsiSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s *pCscMatrix, UINT32 MipiCsiMode)
{
#if !defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT16 VoutCscData[VOUT_CSC_DATA_COUNT];

    if ((pCscMatrix == NULL) || (MipiCsiMode >= VOUT_NUM_MIPI_CSI_MODE)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            VOUT_CalculateCsc(pCscMatrix, VoutCscData);
            RetVal = AmbaRTSL_VoutMipiCsiSetCsc(VoutCscData, MipiCsiMode);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)pCscMatrix;
    (void)MipiCsiMode;
    return VOUT_ERR_INVALID_API;
#endif
}

#if !defined(CONFIG_SOC_H32)
static void ConvertByteClk(const UINT32 BytePerClock, UINT16* pByte4Clock, UINT16* pByte2Clock)
{
    if (BytePerClock == 3U) {
        *pByte4Clock = 1U;
        *pByte2Clock = 1U;
    } else if (BytePerClock == 2U) {
        *pByte4Clock = 2U;
        *pByte2Clock = 1U;
    } else if (BytePerClock == 1U) { /* RAW8 */
        *pByte2Clock = 2U;
        *pByte4Clock = 4U;
    } else {
        *pByte4Clock = 0U;
        *pByte2Clock = 1U;
    }
}
#endif

/**
 *  AmbaVout_CalculateMipiDsiTiming - Calculate video timing parameters for MIPI DSI output
 *  @param[in] MipiDsiMode MIPI DSI pixel format
 *  @param[in] pFrameTiming VOUT frame timing parameters
 *  @param[out] pDisplayTiming Video timing parameters
 *  @param[out] pBlankPacket DSI Blank Packet length parameters
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiCalculateTiming(const UINT8 MipiDsiMode, const AMBA_VOUT_FRAME_TIMING_CONFIG_s *pFrameTiming,
                                        AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming, AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s *pBlankPacket)
{
#if !defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 MipiDsiRawMode, BytePerClock, MipiLaneNum;
    UINT16 Byte4Clock, Byte2Clock;
    DOUBLE CeilVal;
    UINT16 ActiveStartMin, ActiveRowStartMin, RowOffset, RtlOffset;
    UINT16 ActiveWidthBoundary, ActiveHeightBoundary;
    UINT16 X0 = 0U, Y0 = 0U, ActiveStartX0, ActiveStartY0, ActiveEndX0;
    UINT16 HsyncStartMax, HsyncStartMin;
    UINT16 IntRandHsyncStartRange, IntRangeHsyncStart = 0U, IntRandHsyncEndRange, IntRangeHsyncEnd = 0U;
    UINT16 IntRandVsyncStartRange, IntRangeVsyncStart = 0U, IntRandVsyncEndRange, IntRangeVsyncEnd = 0U;
    UINT16 HsyncStartX, HsyncEndX, VsyncStartY0, VsyncEndY0;
    UINT32 RandomBetween0_1 = 0U;
    UINT32 SyncUseBlank, SendSyncEnd;
    UINT16 SyncDis, DisMin, DisMax, HFPDis, HBPDis;
    UINT32  MustUseSyncBlank, AllHS;
    UINT16 SyncOverhead, HBPOverhead, HFPOverhead, MipiSyncWordCnt, MipiHBPWordCnt, MipiHFPWordCnt;

    MipiDsiRawMode = AmbaVout_MipiGetRawMode((UINT32)MipiDsiMode);

    if ((MipiDsiRawMode >= VOUT_NUM_MIPI_DSI_MODE) || (pDisplayTiming == NULL) || (pBlankPacket == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pDisplayTiming->PixelClkFreq = pFrameTiming->PixelClkFreq;
        pDisplayTiming->DisplayMethod = (UINT32)pFrameTiming->Interlace;
        pDisplayTiming->ActivePixels = pFrameTiming->ActivePixels;
        pDisplayTiming->ActiveLines = pFrameTiming->ActiveLines;

        BytePerClock = VOUT_GetMipiBytePerClk(MipiDsiRawMode, VOUT_MIPI_INTERFACE_DSI);
        MipiLaneNum = VOUT_GetMipiLaneNum(MipiDsiRawMode, VOUT_MIPI_INTERFACE_DSI);

        ConvertByteClk(BytePerClock, &Byte4Clock, &Byte2Clock);

        /* HsyncStartRight == 0U */
        ActiveStartMin = 5U + (5U * Byte4Clock) + (2U * Byte2Clock);
        RowOffset = 1U;
        RtlOffset = 4U;

        /* if HsyncStartRight == 1U
         * ActiveStartMin = Byte4Clock;
         * RowOffset = 0U;
         * RtlOffset = 0U;
         */

        ActiveRowStartMin = 5U;

        /* Calculate Hsync Pulse Width */
        ActiveWidthBoundary = pFrameTiming->Htotal - (6U * Byte4Clock) - (4U * Byte2Clock) - RtlOffset - 2U;
        if (AmbaWrap_ceil(((DOUBLE)ActiveWidthBoundary - (DOUBLE)pDisplayTiming->ActivePixels) / 3.0, &CeilVal) != ERR_NONE) {
            RetVal |= VOUT_ERR_UNEXPECTED;
        }
        X0 = ((UINT16)CeilVal * 2U) - 20U;

        ActiveStartX0 = X0 + ActiveStartMin;
        ActiveEndX0 = ActiveStartX0 + pDisplayTiming->ActivePixels - 1U;

        /* HsyncStartRight == 0U */
        HsyncStartMin = 4U;

        /* if HsyncStartRight == 1U
         * HsyncStartMin = ActiveEndX0 + (2U * Byte2Clock) + Byte4Clock + 1U;
         */

        HsyncStartMax = ActiveStartX0 - (5U * Byte4Clock) - (2U * Byte2Clock) - 2U;

        IntRandHsyncStartRange = HsyncStartMax - HsyncStartMin - 1U;
        if (AmbaWrap_ceil((DOUBLE)IntRandHsyncStartRange / 3.0, &CeilVal) != ERR_NONE) {
            RetVal |= VOUT_ERR_UNEXPECTED;
        }
        IntRangeHsyncStart = (UINT16)CeilVal - 4U;
        HsyncStartX = IntRangeHsyncStart + HsyncStartMin;

        IntRandHsyncEndRange = HsyncStartMax - HsyncStartX;
        if (AmbaWrap_ceil((DOUBLE)IntRandHsyncEndRange / 2.0, &CeilVal) != ERR_NONE) {
            RetVal |= VOUT_ERR_UNEXPECTED;
        }
        IntRangeHsyncEnd = (UINT16)CeilVal - 5U;
        HsyncEndX = HsyncStartX + IntRangeHsyncEnd + (2U * Byte4Clock) + Byte2Clock + 1U;

        pDisplayTiming->HsyncPulseWidth = HsyncEndX - HsyncStartX;

        /* Calculate Vsync Pulse Width */
        ActiveHeightBoundary = pFrameTiming->Vtotal - ActiveRowStartMin - RowOffset;
        if (AmbaWrap_ceil(((DOUBLE)ActiveHeightBoundary - (DOUBLE)pDisplayTiming->ActiveLines) / 2.0, &CeilVal) != ERR_NONE) {
            RetVal |= VOUT_ERR_UNEXPECTED;
        }
        Y0 = (UINT16)CeilVal;
        ActiveStartY0 = Y0 + ActiveRowStartMin;

        IntRandVsyncStartRange = ActiveStartY0 - 2U - 1U;
        if (AmbaWrap_ceil((DOUBLE)IntRandVsyncStartRange / 2.0, &CeilVal) != ERR_NONE) {
            RetVal |= VOUT_ERR_UNEXPECTED;
        }
        IntRangeVsyncStart = (UINT16)CeilVal - 2U;
        VsyncStartY0 = IntRangeVsyncStart + 1U;

        IntRandVsyncEndRange = ActiveStartY0 - VsyncStartY0 - 2U;
        if (AmbaWrap_ceil((DOUBLE)IntRandVsyncEndRange / 2.0, &CeilVal) != ERR_NONE) {
            RetVal |= VOUT_ERR_UNEXPECTED;
        }
        IntRangeVsyncEnd = (UINT16)CeilVal - 1U;
        VsyncEndY0 = VsyncStartY0 + IntRangeVsyncEnd + 1U;

        pDisplayTiming->VsyncPulseWidth = VsyncEndY0 - VsyncStartY0;

        /* Calculate Hsync Back Porch */
        pDisplayTiming->HsyncBackPorch  = ActiveStartX0 - HsyncEndX;

        /* Calculate Vsync Back Porch */
        pDisplayTiming->VsyncBackPorch  = ActiveStartY0 - VsyncEndY0;

        /* Calculate Hsync Front Porch */
        pDisplayTiming->HsyncFrontPorch = pFrameTiming->Htotal - pDisplayTiming->HsyncPulseWidth - pDisplayTiming->HsyncBackPorch - pDisplayTiming->ActivePixels;

        /* Calculate Vsync Front Porch */
        pDisplayTiming->VsyncFrontPorch = pFrameTiming->Vtotal - pDisplayTiming->VsyncPulseWidth - pDisplayTiming->VsyncBackPorch - pDisplayTiming->ActiveLines;

        /* Calculate blanking payload for HS case */
        SyncDis = (HsyncEndX - 1U) - HsyncStartX + 1U;
        DisMin = (150U * ((UINT16)MipiLaneNum + 1U) / (UINT16)BytePerClock) + 1U;
        DisMax = 520U;

        if (AmbaWrap_srand(0xA5) != ERR_NONE) {
            RetVal |= VOUT_ERR_UNEXPECTED;
        }
        if (AmbaWrap_rand(&RandomBetween0_1) != ERR_NONE) {
            RetVal |= VOUT_ERR_UNEXPECTED;
        }

        MustUseSyncBlank = (SyncDis < DisMin) ? 1U : ((SyncDis > DisMax) ? 0U : (RandomBetween0_1 % 2U));
        SyncUseBlank = MustUseSyncBlank;
        SendSyncEnd = (SyncUseBlank == 1U) ? 1U : (RandomBetween0_1 % 2U);

        HBPDis = (SendSyncEnd == 1U) ? (ActiveStartX0 - 1U - HsyncEndX + 1U) : (ActiveStartX0 - 1U - HsyncStartX + 1U);
        HFPDis = (pFrameTiming->Htotal - 1U) - (ActiveEndX0 + 1U) + 1U + (HsyncStartX - 1U) + 1U;

        /* Check all HS cases */
        AllHS = VOUT_CheckHorizontalSync(SyncDis, DisMin, DisMax, HBPDis, HFPDis);

        /* Calculate length of Horizontal Sync Active */
        /* Calculate length of Horizontal Back Porch */
        SyncOverhead = (2U * Byte4Clock) + Byte2Clock;
        HBPOverhead = (3U * Byte4Clock) + Byte2Clock;
        if ((AllHS == 1U) && (BytePerClock == 3U)) {
            MipiSyncWordCnt = ((SyncDis - SyncOverhead) * (UINT16)BytePerClock) - 1U;
            MipiHBPWordCnt = ((HBPDis - HBPOverhead) * (UINT16)BytePerClock) - 1U;
        } else {
            MipiSyncWordCnt = (SyncDis - SyncOverhead) * (UINT16)BytePerClock;
            MipiHBPWordCnt = (HBPDis - HBPOverhead) * (UINT16)BytePerClock;
        }
        pBlankPacket->LenHSA = MipiSyncWordCnt;
        pBlankPacket->LenHBP = MipiHBPWordCnt;

        /* Calculate length of Horizontal Front Porch */
        HFPOverhead = (2U * Byte2Clock) + Byte4Clock;
        MipiHFPWordCnt = (HFPDis - HFPOverhead) * (UINT16)BytePerClock;
        pBlankPacket->LenHFP = MipiHFPWordCnt;

        pBlankPacket->LenBLLP = 0U;

        /* Adjust blanking payload size to satisfy horizontal number of bytes/cycles in width */
        if (RetVal == VOUT_ERR_NONE) {
            RetVal = VOUT_CheckCalculatedWidth(BytePerClock, pFrameTiming, pDisplayTiming, pBlankPacket);
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)MipiDsiMode;
    (void)pFrameTiming;
    if (AmbaWrap_memset(pDisplayTiming, 0, sizeof(AMBA_VOUT_DISPLAY_TIMING_CONFIG_s)) == ERR_NONE) {
        /* Do nothing */
    }
    if (AmbaWrap_memset(pBlankPacket, 0, sizeof(AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s)) == ERR_NONE) {
        /* Do nothing */
    }
    return VOUT_ERR_INVALID_API;
#endif
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
#if !defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 ClkRatio, BytePerClock, MipiLaneNum;
    UINT32 MipiDsiRawMode = AmbaVout_MipiGetRawMode(MipiDsiMode);
    AMBA_VOUT_MIPI_TIMING_PARAM_s VoutMipiTiming;
    UINT16 NewHFrontPorch, NewVFrontPorch;
#if defined(CONFIG_LINUX) && (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52))
    int fd;
    hdmi_config_msg_t HdmiConfigMsg;
#endif

    if ((MipiDsiRawMode >= VOUT_NUM_MIPI_DSI_MODE) ||
        (ColorOrder >= VOUT_NUM_MIPI_DSI_MODE_ORDER) ||
        (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            BytePerClock = VOUT_GetMipiBytePerClk(MipiDsiRawMode, VOUT_MIPI_INTERFACE_DSI);
            MipiLaneNum = VOUT_GetMipiLaneNum(MipiDsiRawMode, VOUT_MIPI_INTERFACE_DSI);
            ClkRatio = BytePerClock * 8U / MipiLaneNum;

            /* Config Vout PLL */
            RetVal = VOUT_ConfigMipiClk(VOUT_MIPI_INTERFACE_DSI, ClkRatio, pDisplayTiming->PixelClkFreq);

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            if (RetVal == VOUT_ERR_NONE) {
                /* VoutB use pll_hdmi, need to turn on hdmi power */
                if (VoutChan == AMBA_VOUT_CHANNEL1) {
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
                }
            }
#endif
            /* Set MIPI PHY according to bit rate */
            if (RetVal == VOUT_ERR_NONE) {
                VOUT_CalculateMphyConfig(pDisplayTiming->PixelClkFreq * ClkRatio, &VoutMipiTiming);
                RetVal = AmbaRTSL_VoutConfigMipiPhy(&VoutMipiTiming);
            }

            if (RetVal == VOUT_ERR_NONE) {
                /* Fill up the video timing */
                VOUT_CalculateMipiDsiTiming(pDisplayTiming, BytePerClock, &NewHFrontPorch, &NewVFrontPorch);
                (void)AmbaRTSL_VoutSetMipiDsiTiming(pDisplayTiming, NewHFrontPorch, NewVFrontPorch);

                /* Enable MIPI DSI */
                RetVal = AmbaRTSL_VoutMipiDsiEnable(MipiDsiMode, ColorOrder, MipiLaneNum);
            }

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)MipiDsiMode;
    (void)ColorOrder;
    (void)pDisplayTiming;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_MipiDsiSetPhyCtrl - Adjust electrical signal quality of MIPI DSI interface
 *  @param[in] ParamID PHY control parameter id
 *  @param[in] ParamVal PHY control parameter value
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
#if !defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((ParamID >= VOUT_NUM_PHY_PARAM) || (ParamVal >= VOUT_NUM_MIPI_DPHY_CLK_MODE)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiDsiSetPhyCtrl(ParamID, ParamVal);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)ParamID;
    (void)ParamVal;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_MipiDsiGetStatus - Get vout status of MIPI DSI interface
 *  @param[out] pMipiDsiConfig Pixel format configuration
 *  @param[out] pDisplayTiming Video frame timing parameters
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiGetStatus(AMBA_VOUT_MIPI_DSI_CONFIG_s *pMipiDsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
#if !defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pMipiDsiConfig == NULL) && (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiDsiGetStatus(pMipiDsiConfig, pDisplayTiming);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pMipiDsiConfig);
    AmbaMisra_TouchUnused(pDisplayTiming);
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_MipiDsiSetCscMatrix - Set color space conversion matrix
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] MipiDsiMode Bit depth information
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiSetCscMatrix(const AMBA_VOUT_CSC_MATRIX_s *pCscMatrix, UINT32 MipiDsiMode)
{
#if !defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT16 VoutCscData[VOUT_CSC_DATA_COUNT];

    if ((pCscMatrix == NULL) || (MipiDsiMode >= VOUT_NUM_MIPI_DSI_MODE)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            VOUT_CalculateCsc(pCscMatrix, VoutCscData);
            RetVal = AmbaRTSL_VoutMipiDsiSetCsc(VoutCscData, MipiDsiMode);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)pCscMatrix;
    (void)MipiDsiMode;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_MipiDsiSendDcsWrite - Send DCS command
 *  @param[in] DcsCmd DCS command ID
 *  @param[in] NumParam DCS command payload size
 *  @param[in] pParam DCS command payload
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiSendDcsWrite(UINT32 DcsCmd, UINT32 NumParam, const UINT8 *pParam)
{
#if !defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((NumParam > VOUT_DSI_CMD_MAX_PARAM_NUM) || ((NumParam != 0U) && (pParam == NULL))) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiDsiDcsWrite(DcsCmd, NumParam, pParam);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)DcsCmd;
    (void)NumParam;
    (void)pParam;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_MipiDsiSendNormalWrite - Send normal packet
 *  @param[in] NumParam Normal packet payload size
 *  @param[in] pParam Normal packet payload
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiSendNormalWrite(UINT32 NumParam, const UINT8 *pParam)
{
#if !defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((NumParam > VOUT_DSI_CMD_MAX_PARAM_NUM) || (pParam == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiDsiNormalWrite(NumParam, pParam);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)NumParam;
    (void)pParam;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_MipiDsiSetBlankPkt - Send blank packet
 *  @param[in] pMipiDsiBlankPktCtrl Blank packets control block
 *  @return error code
 */
UINT32 AmbaVout_MipiDsiSetBlankPkt(const AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s *pMipiDsiBlankPktCtrl)
{
#if !defined(CONFIG_SOC_H32)
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pMipiDsiBlankPktCtrl == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_VoutMipiDsiSetBlankPkt(pMipiDsiBlankPktCtrl);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)pMipiDsiBlankPktCtrl;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_GetDisp0Status - Get VOUT display 0 status
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pStatus Vout status
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetDisp0Status(UINT32 *pStatus)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pStatus == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal |= AmbaRTSL_Vout0GetStatus(pStatus);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVout_GetDisp1Status - Get VOUT display 1 status
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pStatus Vout status
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetDisp1Status(UINT32 *pStatus)
{
#if !defined(CONFIG_SOC_CV28)
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pStatus == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal |= AmbaRTSL_Vout1GetStatus(pStatus);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pStatus);
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_GetDisp2Status - Get VOUT display 2 status
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[out] pStatus Vout status
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_GetDisp2Status(UINT32 *pStatus)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pStatus == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal |= AmbaRTSL_Vout2GetStatus(pStatus);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    AmbaMisra_TouchUnused(pStatus);
    return VOUT_ERR_INVALID_API;
#endif
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
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 RetVal = VOUT_ERR_NONE;

    if (Timeout == 0U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout0SetTimeout(Timeout);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)Timeout;
    return VOUT_ERR_INVALID_API;
#endif
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
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 RetVal = VOUT_ERR_NONE;

    if (Timeout == 0U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout1SetTimeout(Timeout);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)Timeout;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_SetDisp2Timeout - Set VOUT display 1 timeout value
 *  @note Chip-dependent. This function is required by dsp support library.
 *  @param[in] Timeout Timeout value
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVout_SetDisp2Timeout(UINT32 Timeout)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 RetVal = VOUT_ERR_NONE;

    if (Timeout == 0U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = VOUT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_Vout2SetTimeout(Timeout);

            if (AmbaKAL_MutexGive(&AmbaVoutMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VOUT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)Timeout;
    return VOUT_ERR_INVALID_API;
#endif
}

#if defined(CONFIG_LINUX)
/**
 *  AmbaVout_HdmiEnable - Enable HDMI display
 *  @param[in] HdmiMode HDMI pixel format
 *  @param[in] FrameSyncPolarity Frame sync signal polarity
 *  @param[in] LineSyncPolarity Line sync signal polarity
 *  @param[in] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaVout_HdmiEnable(UINT32 HdmiMode, UINT32 FrameSyncPolarity, UINT32 LineSyncPolarity, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
#if !defined(CONFIG_SOC_CV28)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((HdmiMode >= VOUT_NUM_HDMI_MODE) ||
        (FrameSyncPolarity >= VOUT_NUM_SIGNAL_MODE) ||
        (LineSyncPolarity >= VOUT_NUM_SIGNAL_MODE) ||
        (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, 100) == OK) {
            /* VOUT PLL is configured in AmbaHDMI.c */

            RetVal = AmbaRTSL_VoutHdmiEnable(HdmiMode, FrameSyncPolarity, LineSyncPolarity, pDisplayTiming);

            (void)AmbaKAL_MutexGive(&AmbaVoutMutex);
        } else {
            RetVal = VOUT_ERR_MUTEX;
        }
    }

    return RetVal;
#else
    /* Not supported API */
    (void)HdmiMode;
    (void)FrameSyncPolarity;
    (void)LineSyncPolarity;
    (void)pDisplayTiming;
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_HdmiGetStatus - Get HDMI related configurations
 *  @param[out] pHdmiConfig HDMI-specific vout configuration
 *  @param[out] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaVout_HdmiGetStatus(AMBA_VOUT_HDMI_CONFIG_s *pHdmiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
#if !defined(CONFIG_SOC_CV28)
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((pHdmiConfig == NULL) ||
        (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, 100) == OK) {
            RetVal = AmbaRTSL_VoutHdmiGetStatus(pHdmiConfig, pDisplayTiming);

            (void)AmbaKAL_MutexGive(&AmbaVoutMutex);
        } else {
            RetVal = VOUT_ERR_MUTEX;
        }
    }

    return RetVal;
#else
    AmbaMisra_TouchUnused(pHdmiConfig);
    AmbaMisra_TouchUnused(pDisplayTiming);
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaVout_HdmiSetCsc - Set CSC matrix as register format
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] HdmiMode HDMI mode for reference
 *  @return error code
 */
UINT32 AmbaVout_HdmiSetCsc(const AMBA_VOUT_CSC_MATRIX_s *pCscMatrix, UINT32 HdmiMode)
{
#if !defined(CONFIG_SOC_CV28)
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT16 HdmiCscData[VOUT_CSC_DATA_COUNT];

    if ((pCscMatrix == NULL) ||
        (HdmiMode >= VOUT_NUM_HDMI_MODE)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaVoutMutex, 100) == OK) {
            VOUT_CalculateCsc(pCscMatrix, HdmiCscData);
            RetVal = AmbaRTSL_VoutHdmiSetCsc(HdmiCscData, HdmiMode);

            (void)AmbaKAL_MutexGive(&AmbaVoutMutex);
        } else {
            RetVal = VOUT_ERR_MUTEX;
        }
    }

    return RetVal;
#else
    (void)pCscMatrix;
    (void)HdmiMode;
    return VOUT_ERR_INVALID_API;
#endif
}
#endif
