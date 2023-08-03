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
#include <sys/reboot.h>

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaMisraFix.h"

#include "AmbaDrvEntry.h"
#include "AmbaSYS.h"
#include "AmbaSYS_Ctrl.h"
#include "AmbaSpiNOR_Ctrl.h"
#include "AmbaSYS_Priv.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_PWC.h"
#include "AmbaRTSL_ORC.h"
#include "AmbaRTSL_GPIO.h"
#include "AmbaCSL_GPIO.h"
#include "AmbaCSL_WDT.h"
#include "AmbaCSL_DebugPort.h"
#include "AmbaDebugPort.h"
#include "AmbaDebugPort_IDSP.h"
#include "AmbaReg_PWM.h"
#include "AmbaReg_GDMA.h"
#include "AmbaReg_I2S.h"
#if !defined(CONFIG_SOC_CV2)
#include "AmbaReg_ScratchpadS.h"
#include "AmbaReg_ScratchpadNS.h"
#else
#include "AmbaReg_Scratchpad.h"
#endif
#include "AmbaReg_VOUT.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaReg_DCPHY.h"
#endif

#if defined(CONFIG_CPU_CORTEX_A76)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif

#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) \
    || defined(CONFIG_SOC_CV52) || defined(CONFIG_SOC_CV3DEV)
#define CHIP_WITH_CLK_SENSOR1
#endif

#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) \
    || defined(CONFIG_SOC_H32)
#define CLK_VID_IN1_FROM_ENET_PLL
#endif

static AMBA_KAL_MUTEX_t AmbaSysMutex;

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
/* cv5x */
#define PLL_FINEADJ_VOUT0CLK AmbaRTSL_PllFineAdjVoutAClk
#define PLL_GET_VOUT0CLK     AmbaRTSL_PllGetVoutAClk
#define PLL_FINEADJ_VOUT1CLK AmbaRTSL_PllFineAdjVoutBClk
#define PLL_GET_VOUT1CLK     AmbaRTSL_PllGetVoutBClk
#elif defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
/* cv2fs */
#define PLL_FINEADJ_VOUT0CLK AmbaRTSL_PllFineAdjVout0Clk
#define PLL_GET_VOUT0CLK     AmbaRTSL_PllGetVout0Clk
#define PLL_FINEADJ_VOUT1CLK AmbaRTSL_PllFineAdjVout1Clk
#define PLL_GET_VOUT1CLK     AmbaRTSL_PllGetVout1Clk
#else
/* cv2x */
#define PLL_FINEADJ_VOUT0CLK AmbaRTSL_PllFineAdjVoutLcdClk
#define PLL_GET_VOUT0CLK     AmbaRTSL_PllGetVoutLcdClk
#define PLL_FINEADJ_VOUT1CLK AmbaRTSL_PllFineAdjVoutTvClk
#define PLL_GET_VOUT1CLK     AmbaRTSL_PllGetVoutTvClk
#endif

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
#define GPIO_BASECOUNT 0x4U
#elif defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define GPIO_BASECOUNT 0x5U
#else
#if defined(CONFIG_SOC_CV22)
#define GPIO_BASECOUNT 0x5U
#elif defined(CONFIG_SOC_CV25)
#define GPIO_BASECOUNT 0x4U
#elif defined(CONFIG_SOC_CV2)
#define GPIO_BASECOUNT 0x6U
#else
#define GPIO_BASECOUNT 0x3U
#endif
#endif

#ifdef CONFIG_CPU_CORTEX_A53
const ULONG gIoMuxAddr = AMBA_CORTEX_A53_IO_MUX_BASE_ADDR;
#define ORC0_BASE_ADDR AMBA_DBG_PORT_CORC_BASE_ADDR
#else
const ULONG gIoMuxAddr = AMBA_CORTEX_A76_IO_MUX_BASE_ADDR;
#define ORC0_BASE_ADDR AMBA_DBG_PORT_CORC0_BASE_ADDR
#endif

#define FLAT_TREE_FILE "/tmp/flat_tree.txt"

typedef struct {
    ULONG paddr;            // Physical Address
    ULONG vaddr;            // Virtual Address
    const UINT32 msize;     // Mapping Area Size; Total Mapping Size is msize*count
    const UINT32 count;     // Mapping Counter
    const UINT32 reg64;     // 64bit reg
    const ULONG taddr;      // Target Variable Address
    const char *initKw;     // For Physical Address Setting, it's Keyword for finding address in device tree; if no need to find, it's NULL.
} AMBA_MAP_TABLE_s;

extern UINT32* const pAmbaDDRC_APB_Reg[2U];

static AMBA_MAP_TABLE_s gMapTable[] = {
    // pAmbaGPIO_Reg[AMBA_NUM_GPIO_GROUP]; continuous range
    {.paddr=0U, .vaddr=0U, .msize=0x1000U, .count=GPIO_BASECOUNT, .taddr=(ULONG)&pAmbaGPIO_Reg[0], .initKw="pinctrl"},
    // pAmbaIOMUX_Reg
    {.paddr=0U, .vaddr=0U, .msize=0x1000U, .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaIOMUX_Reg, .initKw=NULL},
#if !defined(CONFIG_SOC_CV2)
    // pAmbaScratchpadS_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_SCRATCHPAD_S_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaScratchpadS_Reg, .initKw="s_scratchpad"},
    // pAmbaScratchpadNS_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_SCRATCHPAD_NS_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaScratchpadNS_Reg, .initKw="scratchpad_syscon"},
#else
    // pAmbaScratchpadS_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_SCRATCHPAD_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaScratchpadS_Reg, .initKw="s_scratchpad"},
#endif
    // pAmbaRCT_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_RCT_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaRCT_Reg, .initKw="rct_syscon"},
    // pAmbaWDT_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_WDT_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaWDT_Reg, .initKw="wdt"},
    // pAmbaPWM_Reg[0~2]; non-continuous range: these three elements must adjacent to each other
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_PWM_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaPWM_Reg[0], .initKw="pwm"},
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_PWM_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaPWM_Reg[1], .initKw="pwm"},
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_PWM_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaPWM_Reg[2], .initKw="pwm"},
    // pAmbaGDMA_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_GDMA_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaGDMA_Reg, .initKw="gdma"},
    // pAmbaORC_Reg[0]
    {.paddr=ORC0_BASE_ADDR, .vaddr=0U, .msize=sizeof(AMBA_DBG_PORT_ORC_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaORC_Reg[0], .initKw=NULL},
#if defined(CONFIG_SOC_CV2)
    // pAmbaMISC_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_MISC_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaMISC_Reg, .initKw="rtc"},
#endif
//#if !defined(CONFIG_SVC_APPS_IOUT)
    // pAmbaIDSP_CtrlReg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_DBG_PORT_IDSP_CONTROLLER_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaIDSP_CtrlReg, .initKw="idsp_controller"},
    // pAmbaIDSP_VinGlobalReg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_DBG_PORT_IDSP_VIN_GLOBAL_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaIDSP_VinGlobalReg, .initKw="idsp_vin_global"},
    // pAmbaIDSP_VinMainReg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaIDSP_VinMainReg, .initKw="idsp_vin_main"},
    // pAmbaIDSP_VinMasterSyncReg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaIDSP_VinMasterSyncReg, .initKw="idsp_vin_msync"},
//#endif
    // pAmbaMIPI_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_MIPI_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaMIPI_Reg, .initKw="mipi_phy"},
    // pAmbaVout_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaVout_Reg, .initKw="vout"},
    // pAmbaVoutTop_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_TOP_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaVoutTop_Reg, .initKw="vout_top"},
    // pAmbaVoutTvEnc_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_TVENC_CONTROL_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaVoutTvEnc_Reg, .initKw="vout_tvenc"},
#if (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52))
    // pAmbaVoutDisplay0_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_DISPLAY_CONFIG_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaVoutDisplay0_Reg, .initKw="vout_display0"},
    // pAmbaVoutDisplay1_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_DISPLAY_CONFIG_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaVoutDisplay1_Reg, .initKw="vout_display1"},
    // pAmbaVoutDisplay2_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_DISPLAY_C_CONFIG_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaVoutDisplay2_Reg, .initKw="vout_display2"},
#else
    // pAmbaVoutDisplay0_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_DISPLAY0_CONFIG_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaVoutDisplay0_Reg, .initKw="vout_display0"},
#if !defined (CONFIG_SOC_CV28)
    // pAmbaVoutDisplay1_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_DISPLAY1_CONFIG_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaVoutDisplay1_Reg, .initKw="vout_display1"},
#endif
#endif
#if (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52))
    // pAmbaVoutMipiDsiCmd0_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_MIPI_DSI_COMMAND_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaVoutMipiDsiCmd0_Reg, .initKw="vout_mipi_dsi_cmd0"},
    // pAmbaVoutMipiDsiCmd1_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_MIPI_DSI_COMMAND_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaVoutMipiDsiCmd1_Reg, .initKw="vout_mipi_dsi_cmd1"},
#else
#if !defined(CONFIG_SOC_H32)
    // pAmbaVoutMipiDsiCmd_Reg
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_VOUT_MIPI_DSI_COMMAND_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaVoutMipiDsiCmd_Reg, .initKw="vout_mipi_dsi_cmd0"},
#endif
#endif
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    // pAmbaDCPHY_Regs[0]
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_DCPHY_REG_s), .count=1U, .reg64=1U, .taddr=(ULONG)&pAmbaDCPHY_Regs[0], .initKw="cphy0"},
    // pAmbaDCPHY_Regs[1]
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_DCPHY_REG_s), .count=1U, .reg64=1U, .taddr=(ULONG)&pAmbaDCPHY_Regs[1], .initKw="cphy1"},
#endif
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    // DDRC_PLL_Control
    {.paddr=AMBA_DBG_PORT_DDRH0_BASE_ADDR, .vaddr=0U, .msize=0x10000, .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaDDRC_APB_Reg[0], .initKw=NULL},
    {.paddr=AMBA_DBG_PORT_DDRH1_BASE_ADDR, .vaddr=0U, .msize=0x10000, .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaDDRC_APB_Reg[1], .initKw=NULL},
#endif
    // pAmbaI2S_Reg[0~1]
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_I2S_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaI2S_Reg[0], .initKw="i2s"},
    {.paddr=0U, .vaddr=0U, .msize=sizeof(AMBA_I2S_REG_s), .count=1U, .reg64=0U, .taddr=(ULONG)&pAmbaI2S_Reg[1], .initKw="i2s"},
};
#define MAP_TABLE_NUM       (sizeof(gMapTable) / sizeof(gMapTable[0U]))

/*  GetOfBase - of_path: device-tree reg exported by kernel open firmware
 *  @return physical address in device tree
 *  @note this function is an internal using only API
 */
static ULONG GetOfBase(char *of_path, INT32 reg64)
{
    char buf[24], str[24];
    INT32 fd;
    ULONG base = 0U;
    INT32 n;

    fd = open(of_path, O_RDONLY);
    n = read(fd, buf, 8);
    if (n > 0) {
        if (reg64 == 0) {
            sprintf(str, "%02x%02x%02x%02x", buf[0], buf[1], buf[2], buf[3]);
            base = strtol(str, NULL, 16);
        } else {
            sprintf(str, "%02x%02x%02x%02x", buf[4], buf[5], buf[6], buf[7]);
            base = strtol(str, NULL, 16);
        }
    }
    return base;
}

/*  SetBaseToTable - Set gMapTable[idx].paddr from popen() result
 *  @return error code
 *  @note if popen() result is more than 1 line, it will continuously set paddr in gMapTable[].
          So, in this case, "i" will point to next unfinish element.
 */
static void SetBaseToTable(UINT8 *i, FILE* fptr, UINT32 file_sz)
{
    char *tokp;
    char buf[128]  = "\0";
    char target_path[96];
    char kw[32];
    ULONG base;
    UINT8 idx = *i;
    UINT32 curr_sz = 0;

    sprintf(kw, "%s@", gMapTable[idx].initKw);

    while (fgets(buf, sizeof(buf), fptr) != NULL) {
        if ((idx < MAP_TABLE_NUM) &&
            (gMapTable[idx].initKw != NULL) &&
            ((tokp = strtok(buf, "\t \n")) != NULL) &&
            (strstr(buf, kw) != NULL)) {
            memset(target_path, '\0', sizeof(target_path));
            snprintf(target_path, sizeof(target_path), "%s", tokp);
            strcat(target_path, "/reg");
            base = GetOfBase(target_path, gMapTable[idx].reg64);

#if !defined(CONFIG_CPU_CORTEX_A53)
            base |= 0x2000000000;
#endif

            gMapTable[idx].paddr = base;
            printf("%s 0x%lx\n", gMapTable[idx].initKw, gMapTable[idx].paddr);
            idx++;
            break;
        }
    }
    *i = idx - 1;
    curr_sz = ftell(fptr);
    if (curr_sz >= file_sz) {
        fseek(fptr, 0, SEEK_SET);
    }
}

static UINT32 GetFileSize(FILE *fptr)
{
    UINT32 sz;
    fseek(fptr, 0, SEEK_END);
    sz = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    return sz;
}

/*  InitPhysicalAddr - Initialize gMapTable[].paddr by gMapTable[].initKw
 *  @return error code
 *  @note this function is an internal using only API
 */
static void InitPhysicalAddr(void)
{
    UINT8 i;
    char cmd[96];
    FILE* fptr;
    UINT32 file_sz;

    sprintf(cmd, "find /sys/firmware/devicetree/base/ -type d | grep @ |sort > %s", FLAT_TREE_FILE);
    if (system(cmd) < 0) { }

    fptr = fopen(FLAT_TREE_FILE, "r");
    if (fptr == NULL) {
        printf("Open %s failed.\n", FLAT_TREE_FILE);
    } else {
        file_sz = GetFileSize(fptr);
        for (i = 0U; i < MAP_TABLE_NUM; i++) {
            if (gMapTable[i].initKw != NULL) {
                SetBaseToTable(&i, fptr, file_sz);
            } else if (gMapTable[i].taddr == (ULONG)&pAmbaIOMUX_Reg) {
                gMapTable[i].paddr = gIoMuxAddr;
                printf("pAmbaIOMUX_Reg 0x%lx\n", gMapTable[i].paddr);
            } else if (gMapTable[i].taddr == (ULONG)&pAmbaORC_Reg) {
                printf("pAmbaORC_Reg 0x%lx\n", gMapTable[i].paddr);
#if (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52))
            } else if (gMapTable[i].taddr == (ULONG)&pAmbaDDRC_APB_Reg[0]) {
                printf("pAmbaDDRC_APB_Reg[0] 0x%lx\n", gMapTable[i].paddr);
            } else if (gMapTable[i].taddr == (ULONG)&pAmbaDDRC_APB_Reg[1]) {
                printf("pAmbaDDRC_APB_Reg[1] 0x%lx\n", gMapTable[i].paddr);
#endif
            }
        }
        fclose(fptr);
    }
    remove(FLAT_TREE_FILE);
}

/**
 *  MapAAddr - Map virtual addr to GPIO
 *  @return error code
 *  @note this function is an internal using only API
 */
static void *MapAddr(INT32 fd, ULONG Addr, ULONG PageOffset, UINT32 SSize, UINT32 Count)
{
    UINT32 MMapSize = (Count * SSize) + (Addr - PageOffset);
    void *pVirtualAddr;

    if ((pVirtualAddr = mmap(NULL, MMapSize, PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, PageOffset)) == MAP_FAILED) {
        perror("mmap failed");
    }
    // printf("mapped 0x%lx size 0x%x to %p\n", PageOffset, MMapSize, pVirtualAddr);

    return pVirtualAddr;
}

/**
 *  MapAllAddr - Map virtual addr to GPIO in gMapTable[MAP_TABLE_NUM]
 *  @return error code
 *  @note this function is an internal using only API
 */
static UINT32 MapAllAddr(void)
{
    INT32 RetVal = SYS_ERR_NONE;
    INT32 fd;
    ULONG PageOffset;   /* start addr of mapping zone  */
    void* pVirtualAddr = 0;
    ULONG VirtualAddr;  /* virtual addr for desired Addr */
    UINT32 i, j;
    ULONG PAddr;
    UINT32 MSize;
    UINT32 Count;
    void *pTarget;

    InitPhysicalAddr();

    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        perror("Unable to open /dev/mem");
        RetVal = SYS_ERR_UNEXPECTED;
    } else {
        for (i = 0U; i < MAP_TABLE_NUM; i++) {
            PAddr = gMapTable[i].paddr;
            MSize = gMapTable[i].msize;
            Count = gMapTable[i].count;

            PageOffset = PAddr & ~(sysconf(_SC_PAGE_SIZE) - 1);
            pVirtualAddr = MapAddr(fd, PAddr, PageOffset, MSize, Count);
            gMapTable[i].vaddr = (ULONG) pVirtualAddr;
            if (pVirtualAddr != (void *)(-1)) {
                VirtualAddr = (ULONG) pVirtualAddr + (PAddr - PageOffset);
                for (j = 0; j < Count; j++) {
                    pTarget = (void *)(gMapTable[i].taddr + j * sizeof(void *));
                    AmbaMisra_TypeCast(pTarget, &VirtualAddr);
                    VirtualAddr += MSize;
                }
            } else {
                RetVal = SYS_ERR_UNEXPECTED;
                break;
            }
            // printf("[Debug] %s:\t\t\t0x%lx -> 0x%lx\n", gMapTable[i].initKw, gMapTable[i].paddr, gMapTable[i].vaddr);
        }
        close(fd);
    }

    return RetVal;
}

#if 0
/**
 *  UnMapAllAddr - Unmap all virtual addr in gMapTable[]
 *  @return error code
 *  @note this function is an internal using only API
 */
static UINT32 UnMapAllAddr(void)
{
    UINT32 RetVal = GPIO_ERR_NONE;
    ULONG PageOffset;
    UINT32 MMapSize;
    UINT32 i;
    void *pVaddr;

    for(i = 0; i < MAP_TABLE_NUM; i++) {
        pVaddr = (void *)gMapTable[i].vaddr;
        if (pVaddr != NULL) {
            PageOffset = gMapTable[i].paddr & ~(sysconf(_SC_PAGE_SIZE) - 1);
            MMapSize = (gMapTable[i].count * gMapTable[i].msize) + (gMapTable[i].paddr - PageOffset);
            if (munmap(pVaddr, MMapSize) < 0) {
                perror("munmap failed");
                RetVal = GPIO_ERR_UNEXPECTED;
            }
            gMapTable[i].vaddr = 0U;
        }
    }
    return RetVal;
}
#endif

/**
 *  AmbaSYS_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaSYS_DrvEntry(void)
{
    static char AmbaSysMutexName[16] = "AmbaSysMutex";
    UINT32 RetVal = SYS_ERR_NONE;

    if (AmbaKAL_MutexCreate(&AmbaSysMutex, AmbaSysMutexName) != KAL_ERR_NONE) {
        RetVal = SYS_ERR_UNEXPECTED;
    }

    /* Mapping Physical Address to Virtual Address */
    if (MapAllAddr() != SYS_ERR_NONE) {
        RetVal = SYS_ERR_UNEXPECTED;
    }

    AmbaRTSL_PllInit(24000000U);

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

static int sys_popen(char *cmd, char *str, int strsize)
{
    FILE *ifp;
    char *tokp;
    char buf[128]  = "\0";

    if ((ifp = popen(cmd, "r")) == NULL) {
        return 1;
    }
    if (fgets(buf, sizeof(buf), ifp) == NULL) {
        pclose(ifp);
        return 1;
    }
    if ((tokp = strtok(buf, "\t \n")) != NULL) {
        memset(str, '\0', strsize);
        snprintf(str, strsize, "%s", tokp);
    } else {
        pclose(ifp);
        return 1;
    }

    pclose(ifp);

    return 0;
}

static UINT32 linux_PllGetDramClk(void)
{
    char cmd[64];
    char buf[64];

    snprintf(cmd, sizeof(cmd), "cat /proc/ambarella/clock |grep gclk_ddr|awk '{print $2}'");
    (void)sys_popen(cmd, buf, sizeof(buf));

    return atol(buf);
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
    if (AmbaKAL_MutexTake(&AmbaSysMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = SYS_ERR_MUTEX;
    } else {
        if (ClkID == AMBA_SYS_CLK_CORE) {
            (void)AmbaRTSL_PllSetCoreClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetCoreClk();
#if defined(AMBA_SYS_CLK_DRAM)
        } else if (ClkID == AMBA_SYS_CLK_DRAM) {
            ActualFreq = linux_PllGetDramClk();
#endif
#if defined(AMBA_SYS_CLK_DSU)
        } else if (ClkID == AMBA_SYS_CLK_DSU) {
            (void)AmbaRTSL_PllSetDsuClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetDsuClk();
#endif
#if defined(AMBA_SYS_CLK_IDSPV)
        } else if (ClkID == AMBA_SYS_CLK_IDSPV) {
            (void)AmbaRTSL_PllSetIdspvClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetIdspvClk();
#endif
        } else if (ClkID == AMBA_SYS_CLK_IDSP) {
            (void)AmbaRTSL_PllSetIdspClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetIdspClk();
#if defined(AMBA_SYS_CLK_VISION)
        } else if (ClkID == AMBA_SYS_CLK_VISION) {
            (void)AmbaRTSL_PllSetVisionClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetVisionClk();
#endif

#if defined(AMBA_SYS_CLK_FEX)
        } else if (ClkID == AMBA_SYS_CLK_FEX) {
            (void)AmbaRTSL_PllSetFexClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetFexClk();
#endif
        } else if (ClkID == AMBA_SYS_CLK_AUD_0) {
            (void)AmbaRTSL_PllFineAdjAudioClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetAudioClk();
        } else if (ClkID == AMBA_SYS_CLK_VID_IN0) {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            (void)AmbaRTSL_PllFineAdjSensor1Clk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetSensor1Clk();
#else
            (void)AmbaRTSL_PllFineAdjSensor0Clk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetSensor0Clk();
#endif
        } else if (ClkID == AMBA_SYS_CLK_VID_IN1) {
#if defined(CLK_VID_IN1_FROM_ENET_PLL)
            /* gclk_so_pip is from enet pll */
            (void)AmbaRTSL_PllFineAdjEthernetClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetEthernetClk();
#else
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            /* Return sensor clock in place of vin clock here.
             * The clock frequency would be (Vin0 clock) / 4 if
             * AMBA_PLL_FOURFOLD_GCLK_SO_VIN is defined. */
            (void)AmbaRTSL_PllFineAdjSensor0Clk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetSensor0Clk();
#else
            (void)AmbaRTSL_PllFineAdjSensor1Clk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetSensor1Clk();
#endif
#endif
        } else if (ClkID == AMBA_SYS_CLK_VID_OUT0) {
            (void)PLL_FINEADJ_VOUT0CLK(DesiredFreq);
            ActualFreq = PLL_GET_VOUT0CLK();
#if defined(AMBA_SYS_CLK_VID_OUT1) && !defined(CONFIG_SOC_CV28)
        } else if (ClkID == AMBA_SYS_CLK_VID_OUT1) {
            (void)PLL_FINEADJ_VOUT1CLK(DesiredFreq);
            ActualFreq = PLL_GET_VOUT1CLK();
#endif
#if defined(AMBA_SYS_CLK_VID_OUT2)
        } else if (ClkID == AMBA_SYS_CLK_VID_OUT2) {
            (void)AmbaRTSL_PllFineAdjVoutCClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetVoutCClk();
#endif
        } else if (ClkID == AMBA_SYS_CLK_REF_OUT0) {
            (void)AmbaRTSL_PllFineAdjSensor0Clk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetSensor0Clk();
        } else if (ClkID == AMBA_SYS_CLK_REF_OUT1) {
#if defined(CHIP_WITH_CLK_SENSOR1)
            (void)AmbaRTSL_PllFineAdjSensor1Clk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetSensor1Clk();
#else
            (void)AmbaRTSL_PllFineAdjEthernetClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetEthernetClk();
#endif
        } else {
            RetVal = SYS_ERR_ARG;
        }

        if ((pActualFreq != NULL) && (RetVal == SYS_ERR_NONE)) {
            *pActualFreq = ActualFreq;
        }

        if (AmbaKAL_MutexGive(&AmbaSysMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = SYS_ERR_UNEXPECTED;
        }
    }
    return RetVal;
}

/**
 *  AmbaSYS_GetSubClkFreq - The function is used to get the selected clock frequency.
 *  @param[in] ClkID Clock ID
 *  @param[out] pFreq The actual clock frequency in Hz.
 *  @return error code
 */
static UINT32 AmbaSYS_GetSubClkFreq(UINT32 ClkID, UINT32 * pFreq)
{
    UINT32 RetVal = SYS_ERR_NONE;
    switch (ClkID) {
    case AMBA_SYS_CLK_CORE:
        *pFreq = AmbaRTSL_PllGetCoreClk();
        break;
    case AMBA_SYS_CLK_CORTEX:
        *pFreq = AmbaRTSL_PllGetCortexClk();
        break;
#if defined(AMBA_SYS_CLK_DRAM)
    case AMBA_SYS_CLK_DRAM:
        *pFreq = linux_PllGetDramClk();
        break;
#endif
    case AMBA_SYS_CLK_IDSP:
        *pFreq = AmbaRTSL_PllGetIdspClk();
        break;
#if defined(AMBA_SYS_CLK_VISION)
    case AMBA_SYS_CLK_VISION:
        *pFreq = AmbaRTSL_PllGetVisionClk();
        break;
#endif

#if defined(AMBA_SYS_CLK_FEX)
    case AMBA_SYS_CLK_FEX:
        *pFreq = AmbaRTSL_PllGetFexClk();
        break;
#endif
    case AMBA_SYS_CLK_DSP_SYS:
        *pFreq = AmbaRTSL_PllGetAudioClk();
        break;
    case AMBA_SYS_CLK_AUD_0:
        *pFreq = AmbaRTSL_PllGetAudioClk();
        break;
    case AMBA_SYS_CLK_VID_IN0:
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        /* Return sensor clock in place of vin clock here.
         * The clock frequency would be (Vin0 clock) / 4 if
         * AMBA_PLL_FOURFOLD_GCLK_SO_VIN is defined. */
        *pFreq = AmbaRTSL_PllGetSensor1Clk();
#else
        *pFreq = AmbaRTSL_PllGetSensor0Clk();
#endif
        break;
    case AMBA_SYS_CLK_VID_IN1:
#if defined(CLK_VID_IN1_FROM_ENET_PLL)
        /* gclk_so_pip is from enet pll */
        *pFreq = AmbaRTSL_PllGetEthernetClk();
#else
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        /* Return sensor clock in place of vin clock here.
         * The clock frequency would be (Vin4 clock) / 4 if
         * AMBA_PLL_FOURFOLD_GCLK_SO_VIN is defined. */
        *pFreq = AmbaRTSL_PllGetSensor0Clk();
#else
        *pFreq = AmbaRTSL_PllGetSensor1Clk();
#endif
#endif
        break;
    case AMBA_SYS_CLK_VID_OUT0:
        *pFreq = PLL_GET_VOUT0CLK();
        break;
#if defined(AMBA_SYS_CLK_VID_OUT1) && !defined(CONFIG_SOC_CV28)
    case AMBA_SYS_CLK_VID_OUT1:
        *pFreq = PLL_GET_VOUT1CLK();
        break;
#endif
#if defined(AMBA_SYS_CLK_VID_OUT2)
    case AMBA_SYS_CLK_VID_OUT2:
        *pFreq = AmbaRTSL_PllGetVoutCClk();
        break;
#endif
    case AMBA_SYS_CLK_REF_OUT0:
        *pFreq = AmbaRTSL_PllGetSensor0Clk();
        break;
    case AMBA_SYS_CLK_REF_OUT1:
#if defined(CHIP_WITH_CLK_SENSOR1)
        *pFreq = AmbaRTSL_PllGetSensor1Clk();
#else
        *pFreq = AmbaRTSL_PllGetEthernetClk();
#endif
        break;
#if defined(AMBA_SYS_CLK_DSU)
    case AMBA_SYS_CLK_DSU:
        *pFreq = AmbaRTSL_PllGetDsuClk();
        break;
#endif
#if defined(AMBA_SYS_CLK_IDSPV)
    case AMBA_SYS_CLK_IDSPV:
        *pFreq = AmbaRTSL_PllGetIdspvClk();
        break;
#endif
    default:
        RetVal = SYS_ERR_ARG;
        break;
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

    if (pFreq == NULL) {
        RetVal = SYS_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSysMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SYS_ERR_MUTEX;
        } else {
            RetVal = AmbaSYS_GetSubClkFreq(ClkID, pFreq);

            if (AmbaKAL_MutexGive(&AmbaSysMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SYS_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

UINT32 AmbaSYS_SetIoClkFreq(UINT32 ClkID, UINT32 DesiredFreq, UINT32 * pActualFreq)
{
#if defined(CONFIG_CPU_CORTEX_A76) || defined(CONFIG_SOC_CV28)
    /*TODO: AmbaRTSL_PllSetIOClk not ready*/
    AmbaMisra_TouchUnused(&ClkID);
    AmbaMisra_TouchUnused(&DesiredFreq);
    if (pActualFreq != NULL) {
        AmbaMisra_TouchUnused(&pActualFreq);
    }

    return SYS_ERR_NONE;
#else
    UINT32 RetVal = SYS_ERR_NONE;

    if ((ClkID < (UINT32)AMBA_CLK_NUM) && (pActualFreq != NULL)) {
        RetVal = AmbaRTSL_PllSetIOClk(ClkID, DesiredFreq, pActualFreq);
    } else {
        RetVal = SYS_ERR_ARG;
    }
    return RetVal;
#endif
}

UINT32 AmbaSYS_GetIoClkFreq(UINT32 ClkID, UINT32 * pFreq)
{
    UINT32 RetVal = SYS_ERR_NONE;
#if defined(CONFIG_SOC_CV28)
    AmbaMisra_TouchUnused(&ClkID);
    AmbaMisra_TouchUnused(pFreq);
#else
    if ((pFreq != NULL) && (ClkID < (UINT32)AMBA_CLK_NUM)) {
        RetVal = AmbaRTSL_PllGetIOClk(ClkID, pFreq);
    } else {
        RetVal = SYS_ERR_ARG;
    }
#endif
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
    reboot(RB_AUTOBOOT);
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

    if (pBootMode == NULL) {
        RetVal = SYS_ERR_ARG;
    } else {
        *pBootMode = AmbaRTSL_PwcGetBootDevice();
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

    if (AmbaKAL_MutexTake(&AmbaSysMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = SYS_ERR_MUTEX;
    } else {
        AmbaRTSL_PllClkEnable(SysFeature);

        if (AmbaKAL_MutexGive(&AmbaSysMutex) != KAL_ERR_NONE) {
            RetVal = SYS_ERR_UNEXPECTED;
        }
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

    if (AmbaKAL_MutexTake(&AmbaSysMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = SYS_ERR_MUTEX;
    } else {
        AmbaRTSL_PllClkDisable(SysFeature);

        if (AmbaKAL_MutexGive(&AmbaSysMutex) != KAL_ERR_NONE) {
            RetVal = SYS_ERR_UNEXPECTED;
        }
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

    if (pEnable != NULL) {
        AmbaRTSL_PllClkCheck(SysFeature, pEnable);
    } else {
        RetVal = SYS_ERR_ARG;
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
    (void) pOrcTimer;
    if (pOrcTimer != NULL) {
        *pOrcTimer = AmbaRTSL_GetOrcTimer();
    } else {
        RetVal = SYS_ERR_ARG;
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

    return 0U;
}
