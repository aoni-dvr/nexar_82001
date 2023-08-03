/**
*  @file SvcSafeStateMgr.h
*
*  @copyright Copyright (c) 2015 Ambarella, Inc.
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
*
*/

#ifndef SVC_IPC_H
#define SVC_IPC_H

#define SVC_SAFE_STATE_MGR_STACK_SIZE (0x4000U)

typedef struct {
    UINT32 ModuleID;   //!< The ID of the Module
    UINT32 InstanceID; //!< The instance ID of the Module. For modules having only one instance, it should be set as 0.
    UINT32 ApiID;      //!< The API ID causing this error. It could be set as 0 if unknown. Applications could also use it for extended purpose.
    UINT32 ErrorID;    //!< The ID of this error.
} AMBA_SEMGR_MODULE_ERROR_s;

typedef struct {
    UINT32 ErrorBits;     //!< **bit[19:0]**: bit vector for client ID causing an error.<br>**0**: no error for the client ID<br>**1**: error for the client ID
    UINT32 FaultAddress;  //!< First client address to cause an error
    UINT32 FaultClientID; //!< First client ID to cause an error<br>**bit[4:0]**: Client ID<br>**bit[5]**: **0**:physical address, **1**:virtual address
} AMBA_SEMGR_CHIP_ATT_ERROR_s;

typedef struct {
    UINT32 CehuErrorArray[5]; //!< CEHU error bits.
    UINT32 EcruErrorArray[4]; //!< ECRU error bits.
    UINT32 CemuInvalidAddr;   //!< **Only valid when bit 19 of CehuErrors[2] is 1.** <br>The invalid address which causes an range check error.
    UINT32 CemuInvalidCore;   //!< **Only valid when bit 19 of CehuErrors[2] is 1.** <br>The CPU core which attempts to access an invalid address.<br><b>0</b>: Core 0<br><b>1</b>: Core 1<br><b>2</b>: Core 2<br><b>3</b>: Core 3<br><b>Others</b>: invalid value
    AMBA_SEMGR_CHIP_ATT_ERROR_s AttError; //!< Information for DRAM ATT errors.
} AMBA_SEMGR_CHIP_ERROR_s;

typedef struct {
    UINT32 Flags;  //!< **bit[0]**: Set when ChipError data is meaningful.<br>**bit[1]**: Set when ModuleError data is meaningful.<br>**bit[2~31]**: not used now.
    AMBA_SEMGR_CHIP_ERROR_s ChipError;  //!< Information for errors detected by hardware.
    AMBA_SEMGR_MODULE_ERROR_s ModuleError; //!< Information for errors detected or reported by software.
} AMBA_SEMGR_ERROR_INFO_s;

typedef struct {
    UINT32 Flags;  //!< **bit[0]**: Set when ChipError data is meaningful.<br>**bit[1]**: Set when ModuleError data is meaningful.<br>**bit[2~31]**: not used now.
    AMBA_SEMGR_CHIP_ERROR_s ChipError;  //!< Information for errors detected by hardware.
} SVC_SAFE_STATE_ERR_0_s;

typedef struct {
    AMBA_SEMGR_MODULE_ERROR_s ModuleError; //!< Information for errors detected or reported by software.
} SVC_SAFE_STATE_ERR_1_s;

typedef struct {
    UINT32   Priority;      /* task priority */
    UINT32   CpuBits;       /* core selection which task running at */
} SVC_SAFE_STATE_MGR_TSK_CFG_s;

#ifdef CONFIG_ICAM_SENSOR_ASIL_ENABLED
typedef struct {
    UINT32   VinID;  
    UINT32   SensorID;  
    UINT32   FrameCnt;
    UINT32   EmbCrc; // from MX01_IMX390_EmbI2cCrcGet
    UINT32   DataCrc; // form MX01_IMX390_I2cCrcGet
} SVC_SAFE_STATE_MGR_VIN_EMBD_s;
#endif

typedef void (*SVC_SAFE_STATE_CALLBACK_f)(UINT32 AutoRunInterval, UINT8 DisableFex);
typedef void (*SVC_SAFE_STATE_DDRSHM_CALLBACK_f)(UINT32 ID, UINT32 Addr, UINT32 *pFlag);

typedef struct {
    SVC_SAFE_STATE_CALLBACK_f pEnableVpuBist;
    SVC_SAFE_STATE_DDRSHM_CALLBACK_f pChkDdrShm;
} SVC_SAFE_STATE_CALLBACK_s;

typedef struct {
    UINT32 ModuleID;
    UINT8  Enable;
    UINT8  UpperBound;
    UINT8  LowerBound;
} SVC_PWR_MONITOR_CFG_s;

#define AMBA_PSM_MIPI_SLAVE                   0U
#define AMBA_PSM_PLL_1                        1U      /* PLL */
#define AMBA_PSM_PLL_2                        2U      /* PLL DDR */
#define AMBA_PSM_MIPI_DSI_0                   3U
#define AMBA_PSM_MIPI_DSI_1                   4U
#define AMBA_PSM_RNG                          5U
#define AMBA_PSM_BIASGEN                      6U
#define AMBA_PSM_RTC                          7U
#define AMBA_PSM_GPIO_3                       8U      /* GPIO3 UART_AHB */
#define AMBA_PSM_GPIO_2                       9U
#define AMBA_PSM_GPIO_1                       10U     /* GPIO1 (ENET, JTAG) */
#define AMBA_PSM_NAND                         11U
#define AMBA_PSM_SD                           12U
#define AMBA_PSM_SDIO                         13U
#define AMBA_PSM_XOSC                         14U
#define AMBA_PSM_DDRHOST0                     15U
#define AMBA_PSM_DDRHOSTE                     16U
#define AMBA_PSM_USB                          17U
#define AMBA_PSM_TEM1051X                     18U
#define AMBA_PSM_OTP                          19U

#define AMBA_PSM_PERCENT_5                    1U
#define AMBA_PSM_PERCENT_7                    2U
#define AMBA_PSM_PERCENT_10                   3U

#define AMBA_PSM_ENABLE                       0U
#define AMBA_PSM_DISABLE                      1U

UINT32 SvcSafeStateMgr_Init(const SVC_SAFE_STATE_MGR_TSK_CFG_s *pCfg);
UINT32 SvcSafeStateMgr_WaitSysReady(void);
UINT32 SvcSafeStateMgr_SetClkFreq(UINT32 ClkID, UINT32 DesiredFreq, UINT32 * pActualFreq);
UINT32 SvcSafeStateMgr_GetClkFreq(UINT32 ClkID, UINT32 * pFreq);
UINT32 SvcSafeStateMgr_EnableFeature(UINT32 SysFeature);
UINT32 SvcSafeStateMgr_DisableFeature(UINT32 SysFeature);
void   SvcSafeStateMgr_Debug(UINT8 Enable);
#ifdef CONFIG_ICAM_SENSOR_ASIL_ENABLED
UINT32 SvcSafeStateMgr_CheckVinEmbd(const SVC_SAFE_STATE_MGR_VIN_EMBD_s *pInfo);
#endif
void SvcSafeStateMgr_RegisterCallback(const SVC_SAFE_STATE_CALLBACK_s *pCallback);
UINT32 SvcSafeStateMgr_EnableVpBist(void);
UINT32 SvcSafeStateMgr_SetPwrMonitor(const SVC_PWR_MONITOR_CFG_s *pCfg);

UINT32 SvcSafeStateMgr_SndDrmShmInfo(void *pCtrl, void *pMisc, UINT32 CtrlSize, UINT32 MiscSize);
UINT32 SvcSafeStateMgr_SetIDspWDT(UINT32 TimeoutMs);
UINT32 SvcSafeStateMgr_LiveviewStart(void);
UINT32 SvcSafeStateMgr_LiveviewStop(void);

#endif  /* SVC_IPC_H */
