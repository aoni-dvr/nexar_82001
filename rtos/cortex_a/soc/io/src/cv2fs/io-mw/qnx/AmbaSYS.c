/**
 *  @file AmbaSYS.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details System Control APIs
 *
 */
#include "Generic.h"
#include "hw/wdt.h"
#include "hw/ambarella_clk.h"
#include "hw/ambarella_misc.h"

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaSYS.h"
#include "AmbaDrvEntry.h"
#include "AmbaMisraFix.h"
#include "AmbaCortexA53.h"
#include "AmbaReg_RCT.h"
#include "AmbaReg_ScratchpadS.h"
#include "AmbaReg_ScratchpadNS.h"
#include "AmbaReg_VOUT.h"
#include "AmbaCSL_DebugPort.h"
#include "AmbaDebugPort.h"
#include "AmbaDebugPort_IDSP.h"

typedef struct {
    ULONG paddr;            // Physical Address
    ULONG vaddr;            // Virtual Address
    const UINT32 msize;     // Mapping Area Size; Total Mapping Size is msize*count
    const UINT32 count;     // Mapping Counter
    const ULONG taddr;      // Target Variable Address
    const char *name;
} AMBA_MAP_TABLE_s;

static AMBA_MAP_TABLE_s gMapTable[] = {
    // pAmbaRCT_Reg
    {.paddr=AMBA_DBG_PORT_RCT_BASE_ADDR, .vaddr=0U, .msize=sizeof(AMBA_RCT_REG_s), .count=1U, .taddr=(ULONG)&pAmbaRCT_Reg, "rct"},
    // pAmbaScratchpadS_Reg
    //{.paddr=AMBA_CA53_SCRATCHPAD_S_BASE_ADDR, .vaddr=0U, .msize=sizeof(AMBA_SCRATCHPAD_S_REG_s), .count=1U, .taddr=(ULONG)&pAmbaScratchpadS_Reg, .name="scratchpad_s"},
    // pAmbaScratchpadNS_Reg
    {.paddr=AMBA_CA53_SCRATCHPAD_NS_BASE_ADDR, .vaddr=0U, .msize=sizeof(AMBA_SCRATCHPAD_NS_REG_s), .count=1U, .taddr=(ULONG)&pAmbaScratchpadNS_Reg, .name="scratchpad_ns"},
    // pAmbaIDSP_CtrlReg
    {.paddr=AMBA_DBG_PORT_IDSP_CONTROLLER_BASE_ADDR, .vaddr=0U, .msize=sizeof(AMBA_DBG_PORT_IDSP_CONTROLLER_REG_s), .count=1U, .taddr=(ULONG)&pAmbaIDSP_CtrlReg, .name="idsp_controller"},
    // pAmbaIDSP_VinGlobalReg
    {.paddr=AMBA_DBG_PORT_IDSP_VIN_GLOBAL_CONFIG_BASE_ADDR, .vaddr=0U, .msize=sizeof(AMBA_DBG_PORT_IDSP_VIN_GLOBAL_REG_s), .count=1U, .taddr=(ULONG)&pAmbaIDSP_VinGlobalReg, .name="vin_global"},
    // pAmbaIDSP_VinMainReg
    {.paddr=AMBA_DBG_PORT_IDSP_VIN_MAIN_CONFIG_BASE_ADDR, .vaddr=0U, .msize=sizeof(AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s), .count=1U, .taddr=(ULONG)&pAmbaIDSP_VinMainReg, .name="vin_main"},
    // pAmbaIDSP_VinMasterSyncReg
    {.paddr=AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_CONFIG_BASE_ADDR, .vaddr=0U, .msize=sizeof(AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_REG_s), .count=1U, .taddr=(ULONG)&pAmbaIDSP_VinMasterSyncReg, .name="vin_msync"},
    // pAmbaMIPI_Reg
    {.paddr=AMBA_DBG_PORT_MIPI_BASE_ADDR, .vaddr=0U, .msize=sizeof(AMBA_MIPI_REG_s), .count=1U, .taddr=(ULONG)&pAmbaMIPI_Reg, .name="mipi"},
    // pAmbaVout_Reg
    {.paddr=AMBA_CA53_VOUT_BASE_ADDR, .vaddr=0U, .msize=sizeof(AMBA_VOUT_REG_s), .count=1U, .taddr=(ULONG)&pAmbaVout_Reg, .name="vout"},
    // pAmbaVoutMixer0_Reg
    {.paddr=AMBA_CA53_VOUT_BASE_ADDR+0x200U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_MIXER_REG_s), .count=1U, .taddr=(ULONG)&pAmbaVoutMixer0_Reg, .name="vout_mixer0"},
    // pAmbaVoutDisplay0_Reg
    {.paddr=AMBA_CA53_VOUT_BASE_ADDR+0x300U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_DISPLAY_CONFIG_REG_s), .count=1U, .taddr=(ULONG)&pAmbaVoutDisplay0_Reg, .name="vout_display0"},
    // pAmbaVoutMixer1_Reg
    {.paddr=AMBA_CA53_VOUT_BASE_ADDR+0x500U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_MIXER_REG_s), .count=1U, .taddr=(ULONG)&pAmbaVoutMixer1_Reg, .name="vout_mixer1"},
    // pAmbaVoutDisplay1_Reg
    {.paddr=AMBA_CA53_VOUT_BASE_ADDR+0x600U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_DISPLAY_CONFIG_REG_s), .count=1U, .taddr=(ULONG)&pAmbaVoutDisplay1_Reg, .name="vout_display1"},
    // pAmbaVoutOsdRescale_Reg
    {.paddr=AMBA_CA53_VOUT_BASE_ADDR+0x800U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_OSD_RESCALE_REG_s), .count=1U, .taddr=(ULONG)&pAmbaVoutOsdRescale_Reg, .name="vout_osd"},
    // pAmbaVoutTop_Reg
    {.paddr=AMBA_CA53_VOUT_BASE_ADDR+0xA00U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_TOP_REG_s), .count=1U, .taddr=(ULONG)&pAmbaVoutTop_Reg, .name="vout_top"},
    // pAmbaVoutMipiDsiCmd0_Reg
    {.paddr=AMBA_CA53_VOUT_BASE_ADDR+0xB00U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_MIPI_DSI_COMMAND_REG_s), .count=1U, .taddr=(ULONG)&pAmbaVoutMipiDsiCmd0_Reg, .name="vout_dsicmd0"},
    // pAmbaVoutMipiDsiCmd1_Reg
    {.paddr=AMBA_CA53_VOUT_BASE_ADDR+0xE00U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_MIPI_DSI_COMMAND_REG_s), .count=1U, .taddr=(ULONG)&pAmbaVoutMipiDsiCmd1_Reg, .name="vout_dsicmd1"},
};
#define MAP_TABLE_NUM       (sizeof(gMapTable) / sizeof(gMapTable[0U]))

static void *MapAddr(ULONG Addr, ULONG PhysicalAddr, UINT32 SSize, UINT32 Count, const char *Name)
{
    UINT32 MMapSize = (Count * SSize) + (Addr - PhysicalAddr);
    void *pVirtualAddr;

    if ((pVirtualAddr = mmap_device_memory(NULL, MMapSize, PROT_READ | PROT_WRITE | PROT_NOCACHE,
                                           0, Addr)) == MAP_FAILED) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot access to %s region",
              __FUNCTION__, Name);
    } else {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "%s: addr=0x%lx, size=0x%x, name=%s",
              __FUNCTION__, Addr, MMapSize, Name);
    }

    return pVirtualAddr;
}

static UINT32 MapAllAddr(void)
{
    UINT32 RetVal = SYS_ERR_NONE;
    ULONG MAddr;        /* start addr of mapping zone  */
    void *pVirtualAddr = NULL;
    ULONG VirtualAddr;  /* virtual addr for desired Addr */
    ULONG PAddr;
    UINT32 MSize;
    UINT32 Count;
    UINT32 i, j;
    void *pTarget;

    for (i = 0U; i < MAP_TABLE_NUM; i++) {
        PAddr = gMapTable[i].paddr;
        MSize = gMapTable[i].msize;
        Count = gMapTable[i].count;

        /* start addr for mmap_device_memory should be multiple of 128 */
        MAddr = PAddr & ~(128U - 1U);
        pVirtualAddr = MapAddr(PAddr, MAddr, MSize, Count, gMapTable[i].name);
        gMapTable[i].vaddr = (ULONG) pVirtualAddr;
        if (pVirtualAddr != (void *)(-1)) {
            VirtualAddr = (ULONG) pVirtualAddr + (PAddr - MAddr);
            for (j = 0; j < Count; j++) {
                pTarget = (void *)(gMapTable[i].taddr + j * sizeof(void *));
                AmbaMisra_TypeCast(pTarget, &VirtualAddr);
                VirtualAddr += MSize;
            }
        } else {
            RetVal = SYS_ERR_UNEXPECTED;
            break;
        }
    }

    return RetVal;
}

static UINT32 GetClkId(UINT32 ClkID)
{
    UINT32 RetVal = AMBA_CLK_NUM;

    if (ClkID == AMBA_SYS_CLK_CORE) {
        RetVal = AMBA_PLL_CORE;
    } else if (ClkID == AMBA_SYS_CLK_IDSP) {
        RetVal = AMBA_PLL_IDSP;
    } else if (ClkID == AMBA_SYS_CLK_CORTEX) {
        RetVal = AMBA_PLL_CORTEX1;
    } else if (ClkID == AMBA_SYS_CLK_DRAM) {
        RetVal = AMBA_PLL_DDR;
    } else if (ClkID == AMBA_SYS_CLK_DSP_SYS) {
        RetVal = AMBA_PLL_AUDIO;
    } else if (ClkID == AMBA_SYS_CLK_VISION) {
        RetVal = AMBA_PLL_VISION;
    } else if (ClkID == AMBA_SYS_CLK_FEX) {
        RetVal = AMBA_PLL_FEX;
    } else if (ClkID == AMBA_SYS_CLK_AUD_0) {
        RetVal = AMBA_PLL_AUDIO;
    } else if (ClkID == AMBA_SYS_CLK_VID_IN0) {
        RetVal = AMBA_PLL_SENSOR0;
    } else if (ClkID == AMBA_SYS_CLK_VID_IN1) {
        RetVal = AMBA_PLL_SENSOR2;
    } else if (ClkID == AMBA_SYS_CLK_VID_OUT0) {
        RetVal = AMBA_PLL_VIDEO_A;
    } else if (ClkID == AMBA_SYS_CLK_VID_OUT1) {
        RetVal = AMBA_PLL_VIDEO_B;
    } else if (ClkID == AMBA_SYS_CLK_REF_OUT0) {
        RetVal = AMBA_PLL_SENSOR0;
    } else if (ClkID == AMBA_SYS_CLK_REF_OUT1) {
        RetVal = AMBA_PLL_SENSOR2;
    } else if (ClkID == AMBA_SYS_CLK_SD) {
        RetVal = AMBA_PLL_SD;
    } else {
        RetVal = SYS_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaSYS_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaSYS_DrvEntry(void)
{
    UINT32 RetVal = SYS_ERR_NONE;

    /* Mapping Physical Address to Virtual Address */
    if (MapAllAddr() != SYS_ERR_NONE) {
        RetVal = SYS_ERR_UNEXPECTED;
    }

    return RetVal;
}

/**
 *  AmbaSYS_GetRtosCoreId - This function used to get the info about which core executing RTOS
 *  @pCoreId[out] Pointer to store core ID
 *  @return error code
 */
UINT32 AmbaSYS_GetRtosCoreId(UINT32 *pCoreId)
{
    (void)pCoreId;

    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_SetExtInputClkInfo - The function is used to assign the specified external clock frequency value.
 *  @param[in] ExtClkID External clock source ID
 *  @param[in] Freq Clock frequency in Hz.
 *  @return error code
 */
UINT32 AmbaSYS_SetExtInputClkInfo(UINT32 ExtClkID, UINT32 Freq)
{
    (void)ExtClkID;
    (void)Freq;

    UINT32 RetVal = SYS_ERR_NONE;

#if 0
    AMBA_PLL_EXT_CLK_CONFIG_s ExtClkConfig;

    (void)AmbaRTSL_PllGetExtClkConfig(&ExtClkConfig);

    if (ExtClkID == AMBA_SYS_EXT_CLK_AUDIO0) {
        ExtClkConfig.ExtAudioClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_AUDIO1) {
        ExtClkConfig.ExtAudio1ClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_ETHERNET) {
        ExtClkConfig.ExtEthernetClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_SENSOR_IN) {
        ExtClkConfig.ExtSensorInputClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_SENSOR_SPCLK0) {
        ExtClkConfig.SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_C0;
        ExtClkConfig.ExtMuxLvdsSpclkClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_SENSOR_SPCLK1) {
        ExtClkConfig.SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_C1;
        ExtClkConfig.ExtMuxLvdsSpclkClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_SENSOR_SPCLK2) {
        ExtClkConfig.SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_C2;
        ExtClkConfig.ExtMuxLvdsSpclkClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_SENSOR_SPCLK3) {
        ExtClkConfig.SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_C3;
        ExtClkConfig.ExtMuxLvdsSpclkClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_SENSOR_SPCLK4) {
        ExtClkConfig.SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_C4;
        ExtClkConfig.ExtMuxLvdsSpclkClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_SENSOR_SPCLK5) {
        ExtClkConfig.SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_C5;
        ExtClkConfig.ExtMuxLvdsSpclkClkFreq = Freq;
    } else {
        RetVal = SYS_ERR_ARG;
    }

    (void)AmbaRTSL_PllSetExtClkConfig(&ExtClkConfig);
#endif

    return RetVal;
}

/**
 *  AmbaSYS_SetClkFreq - The function is used to set the specified clock frequency.
 *  @param[in] ClkID Clock ID
 *  @param[in] DesiredFreq Desired clock frequency in Hz.
 *  @param[out] pActualFreq The actual clock frequency in Hz.
 *  @return error code
 */
UINT32 AmbaSYS_SetClkFreq(UINT32 ClkID, UINT32 DesiredFreq, UINT32 * pActualFreq)
{
    UINT32 RetVal = SYS_ERR_NONE;
    UINT32 ActualFreq = 0U;
    pll_freq_t clk_freq = {0U};
    int fd, err;

    fd = open("/dev/clock", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager. Use default clocks", __FUNCTION__);
    } else {
        clk_freq.freq = DesiredFreq;
        clk_freq.id = GetClkId(ClkID);

        err = devctl(fd, DCMD_CLOCK_SET_PLL_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
        if (err) {
            fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
        }

        err = devctl(fd, DCMD_CLOCK_GET_PLL_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
        if (err) {
            fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
        }

        ActualFreq = clk_freq.freq;

        close(fd);
    }

    if ((pActualFreq != NULL) && (RetVal == SYS_ERR_NONE)) {
        *pActualFreq = ActualFreq;
    }

    return RetVal;
}

/**
 *  AmbaSYS_GetClkFreq - The function is used to get the specified clock frequency.
 *  @param[in] ClkID Clock ID
 *  @param[out] pFreq The actual clock frequency in Hz.
 *  @return error code
 */
UINT32 AmbaSYS_GetClkFreq(UINT32 ClkID, UINT32 * pFreq)
{
    UINT32 RetVal = SYS_ERR_NONE;
    pll_freq_t clk_freq = {0U};
    int fd, err;

    if (pFreq == NULL) {
        RetVal = SYS_ERR_ARG;
    } else {
        fd = open("/dev/clock", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager. Use default clocks", __FUNCTION__);
        } else {
            clk_freq.freq = -1;
            clk_freq.id = GetClkId(ClkID);

            err = devctl(fd, DCMD_CLOCK_GET_PLL_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            }

            *pFreq = clk_freq.freq;

            close(fd);
        }
    }

    return RetVal;
}

/**
 *  AmbaSYS_SetIoClkFreq - The function is used to set the specified periphral I/O clock frequency.
 *  @param[in] ClkID Clock ID
 *  @param[in] DesiredFreq Desired clock frequency in Hz.
 *  @param[out] pActualFreq The actual clock frequency in Hz.
 *  @return error code
 */
UINT32 AmbaSYS_SetIoClkFreq(UINT32 ClkID, UINT32 DesiredFreq, UINT32 * pActualFreq)
{
    UINT32 RetVal = SYS_ERR_NONE;
    UINT32 ActualFreq = 0U;
    clk_freq_t clk_freq = {0U};
    int fd, err;

    fd = open("/dev/clock", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager. Use default clocks", __FUNCTION__);
    } else {
        clk_freq.freq = DesiredFreq;
        clk_freq.id = ClkID;

        err = devctl(fd, DCMD_CLOCK_SET_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
        if (err) {
            fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
        }

        err = devctl(fd, DCMD_CLOCK_GET_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
        if (err) {
            fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
        }

        ActualFreq = clk_freq.freq;

        close(fd);
    }

    if ((pActualFreq != NULL) && (RetVal == SYS_ERR_NONE)) {
        *pActualFreq = ActualFreq;
    }

    return RetVal;
}

/**
 *  AmbaSYS_GetIoClkFreq - The function is used to get the specified periphral I/O clock frequency.
 *  @param[in] ClkID Clock ID
 *  @param[out] pFreq The actual clock frequency in Hz.
 *  @return error code
 */
UINT32 AmbaSYS_GetIoClkFreq(UINT32 ClkID, UINT32 * pFreq)
{
    UINT32 RetVal = SYS_ERR_NONE;
    clk_freq_t clk_freq = {0U};
    int fd, err;

    if (pFreq == NULL) {
        RetVal = SYS_ERR_ARG;
    } else {
        fd = open("/dev/clock", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager. Use default clocks", __FUNCTION__);
        } else {
            clk_freq.freq = -1;
            clk_freq.id = ClkID;

            err = devctl(fd, DCMD_CLOCK_GET_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            }

            *pFreq = clk_freq.freq;

            close(fd);
        }
    }

    return RetVal;
}

/**
 *  AmbaSYS_SetMaxIdleCycleTime - The function is used to set the maximum idle cycle time.
 *  @param[in] MaxIdleCycleTime The maximum idle cycle time value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSYS_SetMaxIdleCycleTime(UINT32 MaxIdleCycleTime)
{
    (void)MaxIdleCycleTime;

    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_GetClkFreq - The function is used to get the maximum idle cycle time.
 *  @param[out] pMaxIdleCycleTime The maximum idle cycle time value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSYS_GetMaxIdleCycleTime(UINT32 *pMaxIdleCycleTime)
{
    (void)pMaxIdleCycleTime;

    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_Reboot - The function is used to reboot the system.
 *  @return error code
 */
UINT32 AmbaSYS_Reboot(void)
{
    UINT32 RetVal = SYS_ERR_NONE;
    int fd, err;

    system("echo 'disable' > /dev/cehu");

    fd = open("/dev/clock", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager.", __FUNCTION__);
    } else {
        err = devctl(fd, DCMD_SYS_RESET, NULL, 0, NULL);
        if (err) {
            fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
        }

        close(fd);
    }

    return RetVal;
}

/**
 *  AmbaSYS_ChangePowerMode - The function kicks off a power mode change
 *  @param[in] PowerMode Power mode
 *  @return error code
 */
UINT32 AmbaSYS_ChangePowerMode(UINT32 PowerMode)
{
    (void)PowerMode;

    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_ConfigPowerSequence - The function is used to configure one of the power sequence intervals.
 *  @param[in] PwrSeq0 Power sequence interval 0
 *  @param[in] PwrSeq1 Power sequence interval 1
 *  @param[in] PwrSeq2 Power sequence interval 2
 *  @param[in] PwrSeq3 Power sequence interval 3
 *  @return error code
 */
UINT32 AmbaSYS_ConfigPowerSequence(UINT32 PwrSeq0, UINT32 PwrSeq1, UINT32 PwrSeq2, UINT32 PwrSeq3)
{
    (void)PwrSeq0;
    (void)PwrSeq1;
    (void)PwrSeq2;
    (void)PwrSeq3;

    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_TrigPowerDownSequence - The function is used to trigger power-down sequence.
 *  @param[in] Option Force PSEQ3 pin state or not.
 *  @return error code
 */
UINT32 AmbaSYS_TrigPowerDownSequence(UINT32 Option)
{
    (void)Option;
    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_SetWakeUpAlarm - The function is used to wake up system with RTC service.
 *  @param[in] Countdown Number of seconds before triggering the power-up sequence.
 *  @return error code
 */
UINT32 AmbaSYS_SetWakeUpAlarm(UINT32 Countdown)
{
    (void)Countdown;
    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_DismissWakeUpAlarm - The function is used to dismiss the wakeup alarm.
 *  @return error code
 */
UINT32 AmbaSYS_DismissWakeUpAlarm(void)
{
    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_ClearWakeUpInfo - The function is used to clear all the information about system wakeup.
 *  @return error code
 */
UINT32 AmbaSYS_ClearWakeUpInfo(void)
{
    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_GetWakeUpInfo - The function returns the system wakeup information.
 *  @param[out] pWakeUpInfo System wakeup information
 *  @return error code
 */
UINT32 AmbaSYS_GetWakeUpInfo(UINT32 * pWakeUpInfo)
{
    UINT32 RetVal = SYS_ERR_NONE;
    int fd, err;
    amba_wdt_info_t WdtInfo;
    clk_reset_info_t SwResetInfo;

    *pWakeUpInfo = 0x0U;

    fd = open("/dev/clock", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager.", __FUNCTION__);
    } else {
        SwResetInfo.ResetStatus = 0;
        err = devctl(fd, DCMD_CLOCK_GET_RESET_ST, &SwResetInfo, sizeof(clk_reset_info_t), NULL);
        if (err) {
            fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
        } else {
            *pWakeUpInfo |= (SwResetInfo.ResetStatus << 0x6U);
        }

        close(fd);
    }

    fd = open("/dev/wdt", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/wdt manager. Use default clocks", __FUNCTION__);
    } else {
        err = devctl(fd, DCMD_WDT_RET_STATUS, &WdtInfo, sizeof(amba_wdt_info_t), NULL);
        if (err) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
            RetVal = WDT_ERR_UNEXPECTED;
        } else {
            *pWakeUpInfo |= ((WdtInfo.ResetStatus) << 0x7U);
        }
        close(fd);
    }

    return RetVal;
}

/**
 *  AmbaSYS_GetBootMode - The function returns the system wakeup information.
 *  @param[out] pBootMode System wakeup information
 *  @return error code
 */
UINT32 AmbaSYS_GetBootMode(UINT32 * pBootMode)
{
    UINT32 RetVal = SYS_ERR_NONE;
    clk_boot_info_t Mode = {0U};
    int fd, err;

    if (pBootMode != NULL) {
        fd = open("/dev/clock", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager.", __FUNCTION__);
        } else {
            err = devctl(fd, DCMD_CLOCK_GETBOOTMODE, &Mode, sizeof(clk_boot_info_t), NULL);
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            }

            *pBootMode = Mode.BootMode;

            close(fd);
        }
    }

    return RetVal;
}

/**
 *  AmbaSYS_EnableFeature - The function enables the specified feature support by enabling its clock source.
 *  @param[in] SysFeature System feature
 *  @return error code
 */
UINT32 AmbaSYS_EnableFeature(UINT32 SysFeature)
{
    UINT32 RetVal = SYS_ERR_NONE;
    clk_config_t ClockConfig = {0U};
    int fd, err;

    fd = open("/dev/clock", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager.", __FUNCTION__);
    } else {
        ClockConfig.feature = SysFeature;
        err = devctl(fd, DCMD_CLOCK_SETFEATURE, &ClockConfig, sizeof(clk_config_t), NULL);
        if (err) {
            fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
        }

        close(fd);
    }


    return RetVal;
}

/**
 *  AmbaSYS_DisableFeature - The function disables the specified feature support by disabling its clock source.
 *  @param[in] SysFeature System feature
 *  @return error code
 */
UINT32 AmbaSYS_DisableFeature(UINT32 SysFeature)
{
    UINT32 RetVal = SYS_ERR_NONE;
    clk_config_t ClockConfig = {0U};
    int fd, err;

    fd = open("/dev/clock", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager.", __FUNCTION__);
    } else {
        ClockConfig.feature = SysFeature;
        err = devctl(fd, DCMD_CLOCK_DISFEATURE, &ClockConfig, sizeof(clk_config_t), NULL);
        if (err) {
            fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
        }

        close(fd);
    }


    return RetVal;
}

/**
 *  AmbaSYS_CheckFeature - The function checks the specified feature support by the clock source enabled or not.
 *  @param[in] SysFeature System feature
 *  @param[out] pEnable - Enable Flag, 0 = not enabled, 1 = enabled
 *  @return error code
 */
UINT32 AmbaSYS_CheckFeature(UINT32 SysFeature, UINT32 *pEnable)
{
    UINT32 RetVal = SYS_ERR_NONE;
    clk_config_t ClockConfig = {0U};
    int fd, err;

    fd = open("/dev/clock", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager.", __FUNCTION__);
    } else {
        ClockConfig.feature = SysFeature;
        err = devctl(fd, DCMD_CLOCK_CHECKFEATURE, &ClockConfig, sizeof(clk_config_t), NULL);
        if (err) {
            fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
        } else {
            *pEnable = ClockConfig.enable;
        }

        close(fd);
    }

    return RetVal;
}

/**
 *  AmbaSYS_GetOrcTimer - The function returns the system wakeup information.
 *  @param[out] pBootMode System wakeup information
 *  @return error code
 */
UINT32 AmbaSYS_GetOrcTimer(UINT32 *pOrcTimer)
{
    UINT32 RetVal = SYS_ERR_NONE;
    clk_orc_timer_t OrcTimer = {0U};
    int fd, err;

    if (pOrcTimer != NULL) {
        fd = open("/dev/clock", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager.", __FUNCTION__);
        } else {
            err = devctl(fd, DCMD_CLOCK_GETORCTIMER, &OrcTimer, sizeof(clk_orc_timer_t), NULL);
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            }

            *pOrcTimer = OrcTimer.TimeTick;

            close(fd);
        }
    }

    return RetVal;
}

/**
 *  AmbaSYS_JtagOn - enable JTAG during secure boot.
 *  @return error code
 */
UINT32 AmbaSYS_JtagOn(void)
{
    //AmbaCSL_ScratchpadJtagOn();
    int fd, err;
    UINT32 dummy = 0;
    UINT32 uret = 0;

    fd = open("/dev/sec", O_RDWR);
    if (fd == -1) {
        uret = SYS_ERR_UNEXPECTED;
    } else {
        err = devctl(fd, DCMD_SYS_JTAG_ON, &dummy, sizeof(dummy), NULL);
        if (err != 0) {
            printf("DCMD_SYS_JTAG_ON error %d\n", err);
            uret = (UINT32) err;
        }

        close(fd);
    }

    return uret;
}
