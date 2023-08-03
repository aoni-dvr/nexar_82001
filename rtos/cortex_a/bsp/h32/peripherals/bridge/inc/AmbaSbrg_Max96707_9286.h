/**
 *  @file AmbaSbrg_Max96707_9286.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Definitions & Constants for MAX96707 & MAX9286 APIs
 *
 */

#ifndef MAX96707_9286_H
#define MAX96707_9286_H

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

/* Error message */
#define MAX96707_9286_ERR_NONE                  (0U)
#define MAX96707_9286_ERR_ARG                   (BRIDGE_ERR_BASE + 1U)
#define MAX96707_9286_ERR_MUTEX                 (BRIDGE_ERR_BASE + 2U)
#define MAX96707_9286_ERR_UNKNOWN_CHIP_ID       (BRIDGE_ERR_BASE + 3U)

/* Print module */
#define MAX96707_9286_MODULE_ID                 ((UINT16)(BRIDGE_ERR_BASE >> 16U))

/* Slave addr (HW dependent) */
#define IDC_ADDR_MAX9286_A           0x90U      /* Original Max9286 board */
#define IDC_ADDR_MAX9286_B           0x98U      /* New Max9286 board */
#define NUM_IDC_ADDR_MAX9286            2U      /* Possible Max9286 slave addr count */
#define IDC_ADDR_MAX96707            0x80U

/* User-defined VIN<->SlaveID mapping */
#define IDC_ADDR_MAX9286_VIN0        IDC_ADDR_MAX9286_A      /* Original Max9286 board on Vin0 */
#define IDC_ADDR_MAX9286_VIN1        IDC_ADDR_MAX9286_B      /* New Max9286 board on Vin1 */
#define IDC_ADDR_MAX9286_VIN2        IDC_ADDR_MAX9286_A

/* ChipID definition */
#define MAX96707_LINK0_ID               0U
#define MAX96707_LINK1_ID               1U
#define MAX96707_LINK2_ID               2U
#define MAX96707_LINK3_ID               3U
#define MAX96707_ALL_ID                 4U
#define MAX9286_ID                      5U

/* Polarity definition */
#define MAX96707_9286_ACTIVE_LOW        0U
#define MAX96707_9286_ACTIVE_HIGH       1U

#define MIPI_VS_DELAY_PCLK            200U      /* VSync delay clock cycle required by MAX9286 */

typedef struct {
    AMBA_KAL_MUTEX_t Mutex;
    UINT8 InitDone;
    UINT8 DesAddr;                 /* Max9286 I2C Slave Addr */
    UINT8 SerAddr0;                /* Distinguishable Max96707 Link0 I2C Slave Addr */
    UINT8 SerAddr1;                /* Distinguishable Max96707 Link1 I2C Slave Addr */
    UINT8 SerAddr2;                /* Distinguishable Max96707 Link2 I2C Slave Addr */
    UINT8 SerAddr3;                /* Distinguishable Max96707 Link3 I2C Slave Addr */
    UINT8 SerAddrBroadCast;        /* Broadcast Max96707 I2C Slave Addr */
} MAX96707_9286_SERDES_INFO_s;

typedef struct {
    UINT32 EnabledLinkID;          /* Enabled Link ID */
    UINT8  SensorSlaveID;          /* Sensor slave ID */
    UINT8  NumDataBits;            /* Input pixel data bit depth */
    UINT32 PixelRate;              /* Input pixel data rate (unit: pixel/sec) */
    UINT32 FrameSyncPeriod;        /* Internal generated frame sync period in pixel data rate, set 0 if use external frame sync */
    UINT32 VSyncPolarity;          /* VSync polarity for GMSL to CSI-2 conversion (Active-high: 1, Active-low: 0) */
    UINT32 HSyncPolarity;          /* HSync polarity for GMSL to CSI-2 conversion (Active-high: 1, Active-low: 0) */
} MAX96707_9286_SERDES_CONFIG_s;

typedef struct {
    UINT32 VsHighWidth;            /* VSync high pulse width in pixel clock cycle (Unit:PCLK) */
    UINT32 VsLowWidth;             /* VSync low pulse width in pixel clock cycle (Unit:PCLK) */
    UINT32 VsDelay;                /* VSync delay compared with original VSync (Unit:PCLK) */
} MAX96707_9286_RETIMING_s;


UINT32 Max96707_9286_Init(UINT32 VinID);
UINT32 Max96707_9286_Config(UINT32 VinID, const MAX96707_9286_SERDES_CONFIG_s *pSerdesConfig);
UINT32 Max96707_9286_EnableVsRetiming(UINT32 VinID, UINT32 ChipID, const MAX96707_9286_RETIMING_s *pRetimingConfig);
UINT32 Max96707_9286_ReverseDataPin(UINT32 VinID, UINT32 ChipID, UINT8 NumDataBits);
UINT32 Max96707_9286_RegisterWrite(UINT32 VinID, UINT32 ChipID, UINT8 Addr, UINT8 Data);
UINT32 Max96707_9286_RegisterRead(UINT32 VinID, UINT32 ChipID, UINT8 Addr, UINT8* pData);
UINT32 Max96707_9286_EnableCSIOutput(UINT32 VinID, UINT32 FramePeriod);

#endif /* MAX96707_9286_H */
