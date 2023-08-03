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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaMisraFix.h"

#include "AmbaDrvEntry.h"
#include "AmbaSYS.h"
#include "AmbaSYS_Ctrl.h"
#include "AmbaSpiNOR_Ctrl.h"

#if 0
static const char *Amba_GclkName[] = {
    "gclk_adc", "gclk_ahb", "gclk_apb", "gclk_axi", "gclk_audio",
    "gclk_audio_aux", "gclk_can", "gclk_core", "gclk_cortex", "gclk_debug_port",
    "gclk_debounce", "gclk_ddr", "gclk_enet", "gclk_gpio_debounce", "gclk_idsp",
    "gclk_ir", "gclk_mipi_dsi",  "gclk_motor", "gclk_nand", "gclk_pwm",
    "gclk_sd0", "gclk_sd1", "gclk_so", "gclk_so2", "gclk_ssi",
    "gclk_ssis", "gclk_ssi3", "gclk_uart",
    "gclk_uart0", "gclk_uart1", "gclk_uart2", "gclk_uart3",
    "gclk_usb", "gclk_vin0", "gclk_vin1", "gclk_vision",
    "gclk_vo", "gclk_vo2"

    "pll_out_core", "pll_out_sd", "pll_out_hdmi", "pll_out_vo2",
    "smp_twd", "gclk_sdxc", "gclk_sdio", "gclk_sd",
    "gclk_stereo", "gclk_fex",
};
#endif

static UINT32 AmbaPllVoutLcdClkMode = 0U;
static UINT32 AmbaPllVoutLcdClkRatio = 1U;

#ifdef CONFIG_BOOT_CORE_SRTOS
__attribute__((section(".data"))) UINT32 AmbaRtosCoreId = (UINT32)CONFIG_BOOT_CORE_SRTOS;
#else
__attribute__((section(".data"))) UINT32 AmbaRtosCoreId = (UINT32)0;
#endif

/**
 *  AmbaSYS_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaSYS_DrvEntry(void)
{
    UINT32 RetVal = SYS_ERR_NONE;

    return RetVal;
}

/**
 *  AmbaSYS_GetRtosCoreId - This function used to get the info about which core executing RTOS
 *  @pCoreId[out] Pointer to store core ID
 *  @return error code
 */
UINT32 AmbaSYS_GetRtosCoreId(UINT32 *pCoreId)
{
    (void) pCoreId;

    return OK;
}

/**
 *  AmbaSYS_SetExtInputClkInfo - The function is used to assign the specified external clock frequency value.
 *  @param[in] ExtClkID External clock source ID
 *  @param[in] Freq Clock frequency in Hz.
 *  @return error code
 */
UINT32 AmbaSYS_SetExtInputClkInfo(UINT32 ExtClkID, UINT32 Freq)
{
    (void) ExtClkID;
    (void) Freq;
    return 1;
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
    int fd;
    char Cmd[64U] = {0U};

    (void) pActualFreq;

    fd = open("proc/ambarella/ambasys", O_RDWR);
    if (fd == -1) {
        fprintf(stderr,"%s %d, open device %s failed\n",__FUNCTION__, __LINE__,"proc/ambarella/ambasys");
        RetVal = SYS_ERR_UNEXPECTED;
    } else {
        if (ClkID == AMBA_SYS_CLK_CORE) {
            sprintf(Cmd, "%s %d", "gclk_core", DesiredFreq);
        } else if (ClkID == AMBA_SYS_CLK_IDSP) {
            sprintf(Cmd, "%s %d", "gclk_idsp", DesiredFreq);
        } else if (ClkID == AMBA_SYS_CLK_VISION) {
            sprintf(Cmd, "%s %d", "gclk_vision", DesiredFreq);
        } else if (ClkID == AMBA_SYS_CLK_AUD_0) {
            sprintf(Cmd, "%s %d", "gclk_audio_aux", DesiredFreq);
        } else if (ClkID == AMBA_SYS_CLK_VID_OUT0) {
            sprintf(Cmd, "%s %d", "pll_out_vo2", DesiredFreq);
        } else if (ClkID == AMBA_SYS_CLK_VID_OUT1) {
            sprintf(Cmd, "%s %d", "pll_out_hdmi", DesiredFreq);
        } else if (ClkID == AMBA_SYS_CLK_REF_OUT0) {
            sprintf(Cmd, "%s %d", "gclk_so", DesiredFreq);
        } else if (ClkID == AMBA_SYS_CLK_REF_OUT1) {
            sprintf(Cmd, "%s %d", "gclk_so2", DesiredFreq);
        } else {
            RetVal = SYS_ERR_ARG;
        }
    }

    write(fd, Cmd, sizeof(Cmd));
    close(fd);

    AmbaSYS_GetClkFreq(ClkID, pActualFreq);

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
    int fd;
    char Cmd[64U] = {0U};
    (void) ClkID;
    (void) pFreq;

    fd = open("proc/ambarella/ambasys", O_RDWR);
    if (fd == -1) {
        fprintf(stderr,"%s %d, open device %s failed\n",__FUNCTION__, __LINE__,"proc/ambarella/ambasys");
        RetVal = SYS_ERR_UNEXPECTED;
    } else {
        if (ClkID == AMBA_SYS_CLK_CORE) {
            sprintf(Cmd, "%s", "gclk_core");
        } else if (ClkID == AMBA_SYS_CLK_IDSP) {
            sprintf(Cmd, "%s", "gclk_idsp");
        } else if (ClkID == AMBA_SYS_CLK_CORTEX) {
            sprintf(Cmd, "%s", "gclk_cortex");
        } else if (ClkID == AMBA_SYS_CLK_DRAM) {
            sprintf(Cmd, "%s", "gclk_ddr");
        } else if (ClkID == AMBA_SYS_CLK_VISION) {
            sprintf(Cmd, "%s", "gclk_vision");
        } else if (ClkID == AMBA_SYS_CLK_DSP_SYS) {
            sprintf(Cmd, "%s", "gclk_audio_aux");
        } else if (ClkID == AMBA_SYS_CLK_AUD_0) {
            sprintf(Cmd, "%s", "gclk_audio_aux");
        } else if (ClkID == AMBA_SYS_CLK_REF_OUT0) {
            sprintf(Cmd, "%s", "gclk_so");
        } else if (ClkID == AMBA_SYS_CLK_REF_OUT1) {
            sprintf(Cmd, "%s", "gclk_so2");
        } else {
            RetVal = SYS_ERR_ARG;
        }

        read(fd, Cmd, sizeof(Cmd));
        sscanf(Cmd, "%d", pFreq);
        close(fd);
    }
    return RetVal;
}

UINT32 AmbaSYS_SetIoClkFreq(UINT32 ClkID, UINT32 DesiredFreq, UINT32 * pActualFreq)
{
    UINT32 RetVal = SYS_ERR_NONE;
    int fd;
    char Cmd[64U] = {0U};
    UINT32 TargetFreq = DesiredFreq;

    fd = open("proc/ambarella/ambasys", O_RDWR);
    if (fd == -1) {
        fprintf(stderr,"%s %d, open device %s failed\n",__FUNCTION__, __LINE__,"proc/ambarella/ambasys");
        RetVal = SYS_ERR_UNEXPECTED;
    } else {
        if ((TargetFreq < 24000000U) && (TargetFreq != 0x0U)) {
            TargetFreq = TargetFreq * (24000000U / TargetFreq) + 1;
        }
        if (ClkID == AMBA_CLK_SENSOR0) {
            sprintf(Cmd, "%s %d", "pll_out_so", TargetFreq);
            write(fd, Cmd, sizeof(Cmd));
            sprintf(Cmd, "%s %d", "gclk_so", DesiredFreq);
            write(fd, Cmd, sizeof(Cmd));
            sprintf(Cmd, "%s %d", "gclk_so_vin", DesiredFreq);
            write(fd, Cmd, sizeof(Cmd));
        } else if (ClkID == AMBA_CLK_ENET) {
            sprintf(Cmd, "%s %d", "pll_out_enet", TargetFreq);
            write(fd, Cmd, sizeof(Cmd));
            sprintf(Cmd, "%s %d", "gclk_so_pip", DesiredFreq);
            write(fd, Cmd, sizeof(Cmd));
        } else if (ClkID == AMBA_CLK_SENSOR1) {
            sprintf(Cmd, "%s %d", "gclk_so2", TargetFreq);
            write(fd, Cmd, sizeof(Cmd));
        } else if (ClkID == AMBA_CLK_VOUTLCD) {
            sprintf(Cmd, "%s %d %d %d", "pll_out_vo2", TargetFreq, AmbaPllVoutLcdClkMode, AmbaPllVoutLcdClkRatio);
            write(fd, Cmd, sizeof(Cmd));
        } else {
            RetVal = SYS_ERR_ARG;
        }
    }

    close(fd);
    AmbaSYS_GetIoClkFreq(ClkID, pActualFreq);

    return RetVal;
}

UINT32 AmbaSYS_GetIoClkFreq(UINT32 ClkID, UINT32 * pFreq)
{
    UINT32 RetVal = SYS_ERR_NONE;
    int fd;
    char Cmd[64U] = {0U};
    (void) ClkID;
    (void) pFreq;

    fd = open("proc/ambarella/ambasys", O_RDWR);
    if (fd == -1) {
        fprintf(stderr,"%s %d, open device %s failed\n",__FUNCTION__, __LINE__,"proc/ambarella/ambasys");
        RetVal = SYS_ERR_UNEXPECTED;
    } else {
        if (ClkID == AMBA_CLK_SENSOR0) {
            sprintf(Cmd, "%s", "gclk_so");
        } else if (ClkID == AMBA_CLK_ENET) {
            sprintf(Cmd, "%s", "pll_out_enet");
        } else if (ClkID == AMBA_CLK_SENSOR1) {
            sprintf(Cmd, "%s", "gclk_so2");
        }  else {
            RetVal = SYS_ERR_ARG;
        }

        read(fd, Cmd, sizeof(Cmd));
        sscanf(Cmd, "%d", pFreq);
        close(fd);
    }
    return RetVal;
}

static UINT32 Sys_SetVin0Config(UINT32 Mode)
{
    UINT32 RetVal = SYS_ERR_NONE;
    int fd;
    UINT32* pVirtualAddr;

    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        perror("Unable to open /dev/mem");
        RetVal = SYS_ERR_UNEXPECTED;
    } else {
        if ((pVirtualAddr = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xed080000)) == MAP_FAILED) {
            perror("mmap failed");
            RetVal = SYS_ERR_UNEXPECTED;
        } else {
            switch (Mode) {
            case AMBA_SYS_SENSOR_REF_CLK_OUTPUT:
                pVirtualAddr[0xBC >> 2U] = 0U;
                break;

            case AMBA_SYS_SENSOR_REF_CLK_INPUT:
                pVirtualAddr[0xBC >> 2U] = 1U;
                break;

            default:
                RetVal = SYS_ERR_ARG;
                break;
            }

            if (munmap(pVirtualAddr, 0x1000) < 0) {
                perror("munmap failed");
            }

        }
        close(fd);
    }

    return RetVal;
}

static UINT32 Sys_SetVin1Config(UINT32 Mode)
{
    UINT32 RetVal = SYS_ERR_NONE;
    int fd;
    UINT32* pVirtualAddr;

    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        perror("Unable to open /dev/mem");
        RetVal = SYS_ERR_UNEXPECTED;
    } else {
        if ((pVirtualAddr = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xed080000)) == MAP_FAILED) {
            perror("mmap failed");
            RetVal = SYS_ERR_UNEXPECTED;
        } else {
            switch (Mode) {
            case AMBA_SYS_SENSOR_REF_CLK_OUTPUT:
                pVirtualAddr[0x5FC >> 2U] = 1U;
                break;

            case AMBA_SYS_SENSOR_REF_CLK_INPUT:
                pVirtualAddr[0x5FC >> 2U] = 0U;
                break;

            default:
                RetVal = SYS_ERR_ARG;
                break;
            }

            if (munmap(pVirtualAddr, 0x1000) < 0) {
                perror("munmap failed");
            }

        }
        close(fd);
    }

    return RetVal;
}

static UINT32 Sys_SetVoutLcdConfig(UINT32 Config)
{
    UINT32 RetVal = SYS_ERR_NONE;
    UINT32 Mode = Config & 0xf;
    UINT32 Ratio = Config >> 0x4;

    if ((Mode >= AMBA_NUM_SYS_VOUT_LCD_MODE) || (Ratio == 0U)) {
        RetVal = SYS_ERR_ARG;
    } else {
        AmbaPllVoutLcdClkMode = Mode;
        AmbaPllVoutLcdClkRatio = Ratio;
    }

    return RetVal;
}

UINT32 AmbaSYS_ClkSetConfig(UINT32 ClkID, UINT32 Config)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (ClkID == AMBA_CLK_SENSOR0) {
        RetVal = Sys_SetVin0Config(Config);
    } else if (ClkID == AMBA_CLK_SENSOR1) {
        RetVal = Sys_SetVin1Config(Config);
    } else if (ClkID == AMBA_CLK_VOUTLCD) {
        Sys_SetVoutLcdConfig(Config);
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
    (void) MaxIdleCycleTime;
    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_GetClkFreq - The function is used to get the maximum idle cycle time.
 *  @param[out] pMaxIdleCycleTime The maximum idle cycle time value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSYS_GetMaxIdleCycleTime(UINT32 *pMaxIdleCycleTime)
{
    UINT32 RetVal = SYS_ERR_NONE;

    (void) pMaxIdleCycleTime;
    return RetVal;
}

/**
 *  AmbaSYS_Reboot - The function is used to reboot the system.
 *  @return error code
 */
UINT32 AmbaSYS_Reboot(void)
{

    return 1;
}

/**
 *  AmbaSYS_ChangePowerMode - The function kicks off a power mode change
 *  @param[in] PowerMode Power mode
 *  @return error code
 */
UINT32 AmbaSYS_ChangePowerMode(UINT32 PowerMode)
{
    UINT32 RetVal = SYS_ERR_NONE;

    (void) PowerMode;
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
    int fd;
    char Cmd[64U] = {0U};
    (void) pBootMode;

    fd = open("proc/ambarella/bootmode", O_RDWR);
    if (fd == -1) {
        fprintf(stderr,"%s %d, open device %s failed\n",__FUNCTION__, __LINE__,"proc/ambarella/bootmode");
        RetVal = SYS_ERR_UNEXPECTED;
    } else {
        read(fd, Cmd, sizeof(Cmd));
        sscanf(Cmd, "%d", pBootMode);
        close(fd);
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
    int fd;
    char Cmd[64U] = {0U};
    (void) SysFeature;

    fd = open("proc/ambarella/ambasys", O_RDWR);
    if (fd == -1) {
        fprintf(stderr,"%s %d, open device %s failed\n",__FUNCTION__, __LINE__,"proc/ambarella/ambafeature");
        RetVal = SYS_ERR_UNEXPECTED;
    } else {
        sprintf(Cmd, "%d %d", SysFeature, 1U);
        write(fd, Cmd, sizeof(Cmd));
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
    int fd;
    char Cmd[64U] = {0U};
    (void) SysFeature;

    fd = open("proc/ambarella/ambasys", O_RDWR);
    if (fd == -1) {
        fprintf(stderr,"%s %d, open device %s failed\n",__FUNCTION__, __LINE__,"proc/ambarella/ambafeature");
        RetVal = SYS_ERR_UNEXPECTED;
    } else {
        sprintf(Cmd, "%d %d", SysFeature, 0U);
        write(fd, Cmd, sizeof(Cmd));
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
    (void) SysFeature;
    (void) pEnable;

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
    (void) pOrcTimer;
    //if (pOrcTimer != NULL) {
    //*pOrcTimer = AmbaRTSL_GetOrcTimer();
    //} else {
    //    RetVal = SYS_ERR_ARG;
    //}
    return RetVal;
}

/**
 *  AmbaSYS_JtagOn - enable JTAG during secure boot.
 *  @return error code
 */
UINT32 AmbaSYS_JtagOn(void)
{
    //AmbaCSL_ScratchpadJtagOn();

    return 0U;
}
