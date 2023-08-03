/**
 *  @file AmbaVIN.c
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details VIN Middleware APIs
 *
 */

#include <stdio.h>
//#include "Generic.h"
//#include "hw/ambarella_vin.h"
//#include "hw/ambarella_clk.h"
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN.h"

static AMBA_KAL_MUTEX_t _AmbaVinMutex[AMBA_NUM_VIN_CHANNEL];  /* Mutex */

/**
 *  AmbaVIN_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVIN_DrvEntry(void)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 i;

    for (i = 0U; i < AMBA_NUM_VIN_CHANNEL; i ++) {
        /* Create Mutex */
        if (AmbaKAL_MutexCreate(&_AmbaVinMutex[i], NULL) != OK) {
            RetVal = VIN_ERR_MUTEX;    /* should never happen */
        }
    }
//    AmbaRTSL_VinInit();

    return RetVal;
}

/**
 *  AmbaVIN_SlvsReset - Reset VIN module and reset VIN pad to SLVS mode
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pPadConfig Pointer to VIN pad configuration
 *  @return error code
 */
UINT32 AmbaVIN_SlvsReset(UINT32 VinID, const AMBA_VIN_SLVS_PAD_CONFIG_s *pPadConfig)
{
    (void)VinID;
    (void)pPadConfig;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_MipiReset - Reset VIN module and reset VIN pad to MIPI mode
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pPadConfig Pointer to VIN pad configuration
 *  @return error code
 */
UINT32 AmbaVIN_MipiReset(UINT32 VinID, const AMBA_VIN_MIPI_PAD_CONFIG_s *pPadConfig)
{
    (void)VinID;
    (void)pPadConfig;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_DvpReset - Reset VIN module and reset VIN pad to Parallel mode
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pPadConfig Pointer to VIN pad configuration
 *  @return error code
 */
UINT32 AmbaVIN_DvpReset(UINT32 VinID, const AMBA_VIN_DVP_PAD_CONFIG_s *pPadConfig)
{
    (void)VinID;
    (void)pPadConfig;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_SlvsConfig - Configure VIN to receive data via SLVS or Sub-LVDS interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinSlvsConfig Pointer to SLVS configuration
 *  @return error code
 */
UINT32 AmbaVIN_SlvsConfig(UINT32 VinID, const AMBA_VIN_SLVS_CONFIG_s *pVinSlvsConfig)
{
    (void)VinID;
    (void)pVinSlvsConfig;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_MipiConfig - Configure VIN to receive data via MIPI interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinMipiConfig Pointer to MIPI configuration
 *  @return error code
 */
UINT32 AmbaVIN_MipiConfig(UINT32 VinID, const AMBA_VIN_MIPI_CONFIG_s *pVinMipiConfig)
{
    (void)VinID;
    (void)pVinMipiConfig;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_DvpConfig - Configure VIN to receive data via digital video port (DVP) parallel interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinDvpConfig Pointer to MIPI configuration
 *  @return error code
 */
UINT32 AmbaVIN_DvpConfig(UINT32 VinID, const AMBA_VIN_DVP_CONFIG_s *pVinDvpConfig)
{
    (void)VinID;
    (void)pVinDvpConfig;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_CaptureConfig - Configure Vin capture window
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pCaptureWindow Pointer to VIN capture window
 *  @return error code
 */
UINT32 AmbaVIN_CaptureConfig(UINT32 VinID, const AMBA_VIN_WINDOW_s *pCaptureWindow)
{
    (void)VinID;
    (void)pCaptureWindow;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_IsEnabled - Get Vin enable status
 *  @param[in] VinID Indicate VIN channel
 *  @return vin enable status
 */
UINT32 AmbaVIN_IsEnabled(UINT32 VinID)
{
    (void)VinID;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_GetMainCfgBufInfo - Get VIN main buffer address
 *  @param[in] VinID Indicate VIN channel
 *  @param[out] pAddr Pointer to receive the VIN main buffer address
 *  @param[out] pSize Pointer to receive the VIN main buffer size
 *  @return error code
 */
UINT32 AmbaVIN_GetMainCfgBufInfo(UINT32 VinID, ULONG *pAddr, UINT32 *pSize)
{
    (void)VinID;
    (void)pAddr;
    (void)pSize;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_SensorClkEnable - Enable clock as the sensor clock source
 *  @param[in] ClkID Indicate clock channel
 *  @param[in] Frequency Clock frequency
 *  @return error code
 */
UINT32 AmbaVIN_SensorClkEnable(UINT32 ClkID, UINT32 Frequency)
{
    (void)ClkID;
    (void)Frequency;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_SensorClkDisable - Disable the clock
 *  @param[in] ClkID Indicate clock channel
 *  @return error code
 */
UINT32 AmbaVIN_SensorClkDisable(UINT32 ClkID)
{
    (void)ClkID;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_GetInfo - Get Vin info
 *  @param[in] VinID Indicate VIN channel
 *  @param[out] pInfo Pointer to VIN info
 *  @return error code
 */
UINT32 AmbaVIN_GetInfo(UINT32 VinID, AMBA_VIN_INFO_s *pInfo)
{
    (void)VinID;
    (void)pInfo;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_DataLaneRemap - Remap data lane. It's used when sensor output data lanes are not connected to Ambarella chip in order
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pLaneRemapConfig Pointer to lane remapping configuration
 *  @return error code
 */
UINT32 AmbaVIN_DataLaneRemap(UINT32 VinID, const AMBA_VIN_LANE_REMAP_CONFIG_s *pLaneRemapConfig)
{
    (void)VinID;
    (void)pLaneRemapConfig;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_MasterSyncEnable - Enable master H/V Sync generator
 *  @param[in] MSyncID Indicate Master sync pin ID
 *  @param[in] pMSyncConfig Pointer to master H/V Sync configuration
 *  @return error code
 */
UINT32 AmbaVIN_MasterSyncEnable(UINT32 MSyncID, const AMBA_VIN_MASTER_SYNC_CONFIG_s *pMSyncConfig)
{
    (void)MSyncID;
    (void)pMSyncConfig;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_MasterSyncDisable - Disable master H/V Sync generator
 *  @param[in] MSyncID Indicate Master sync pin ID
 *  @return error code
 */
UINT32 AmbaVIN_MasterSyncDisable(UINT32 MSyncID)
{
    (void)MSyncID;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_SetLvdsTermination - Adjust the impendance of LVDS pad
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] Value Indicate the impendance
 *  @return error code
 */
UINT32 AmbaVIN_SetLvdsTermination(UINT32 VinID, UINT32 Value)
{
    (void)VinID;
    (void)Value;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_SensorClkSetDrvStrength - Adjust the driving strength of sensor clock
 *  @param[in] Value Indicate the strength
 *  @return error code
 */
UINT32 AmbaVIN_SensorClkSetDrvStrength(UINT32 Value)
{
    (void)Value;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_MipiVirtChanConfig - It's used to configure MIPI virtual channel receiving
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pMipiVirtChanConfig Pointer to MIPI virtual channel receiving configuration
 *  @return error code
 */
UINT32 AmbaVIN_MipiVirtChanConfig(UINT32 VinID, const AMBA_VIN_MIPI_VC_CONFIG_s *pMipiVirtChanConfig)
{
    (void)VinID;
    (void)pMipiVirtChanConfig;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_MipiEmbDataConfig - Configure MIPI embedded data capturing
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pMipiEmbDataConfig Pointer to MIPI embedded data configuration
 *  @return error code
 */
UINT32 AmbaVIN_MipiEmbDataConfig(UINT32 VinID, const AMBA_VIN_MIPI_EMB_DATA_CONFIG_s *pMipiEmbDataConfig)
{
    (void)VinID;
    (void)pMipiEmbDataConfig;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_DelayedVSyncEnable - Generate Delayed HSYNC/VSYNC output control (Only Delay Period can be changed after first time config)
 *  @param[in] pDelayedVSyncConfig Pointer to DelayedVSync configuration
 *  @return error code
 */
UINT32 AmbaVIN_DelayedVSyncEnable(const AMBA_VIN_DELAYED_VSYNC_CONFIG_s *pDelayedVSyncConfig)
{
    (void)pDelayedVSyncConfig;

    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}

/**
 *  AmbaVIN_DelayedVSyncDisable - Disable Delayed HSYNC/VSYNC output (Previous state and configurations will be kept)
 *  @return error code
 */
UINT32 AmbaVIN_DelayedVSyncDisable(void)
{
    printf("%s not implemented!\n", __func__);

    return VIN_ERR_NONE;
}
