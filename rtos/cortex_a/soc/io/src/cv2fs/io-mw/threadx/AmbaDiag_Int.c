/**
 *  @file AmbaDiag_Int.c
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
 *  @details Diagnostic functions for interrupts
 *
 */

#include "AmbaTypes.h"
#include "AmbaIOUtility.h"

#include "AmbaSYS_Ctrl.h"
#include "AmbaRTSL_GIC.h"

static UINT32 DiagIrqCount[AMBA_NUM_INTERRUPT] GNU_SECTION_NOZEROINIT;

static void Diag_IntCounter(UINT32 IntID)
{
    if (IntID < (UINT32)AMBA_NUM_INTERRUPT) {
        DiagIrqCount[IntID]++;
    }
}

/**
 *  AmbaDiag_IntEnableProfiler - Enable IRQ profiling
 */
void AmbaDiag_IntEnableProfiler(void)
{
    UINT32 i;

    for (i = 0; i < (UINT32)AMBA_NUM_INTERRUPT; i++) {
        DiagIrqCount[i] = 0;
    }

    AmbaINT_SetProfileFunc(Diag_IntCounter, NULL);
}

/**
 *  AmbaDiag_IntRestartProfiler - Reset statistics
 */
void AmbaDiag_IntRestartProfiler(void)
{
    UINT32 i;

    for (i = 0; i < (UINT32)AMBA_NUM_INTERRUPT; i++) {
        DiagIrqCount[i] = 0;
    }
}

static void Diag_PrintFormattedInt(AMBA_SYS_LOG_f LogFunc, const char *pFmtString, UINT32 Value, UINT32 Digits)
{
    char StrBuf[64];
    UINT32 ArgsUInt32[2];
    UINT32 StringLength, i;

    ArgsUInt32[0] = Value;

    StringLength = IO_UtilityStringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgsUInt32);
    if (StringLength < Digits) {
        for (i = 0; i < (Digits - StringLength); i++) {
            LogFunc(" ");
        }
    }
    LogFunc(StrBuf);
}

static void SHELL_PrintFormatedIrqInfo(AMBA_SYS_LOG_f LogFunc, UINT32 IntID, const AMBA_INT_INFO_s *IntInfo, UINT32 Count)
{
    UINT32 StringLength, i;
    static const char * const AmbaIntTypeStr[2] = {
        [0] = "IRQ",
        [1] = "FIQ",
    };

    static const char * const AmbaIntSenseStr[2] = {
        [INT_TRIG_HIGH_LEVEL]   = "High Lv",
        [INT_TRIG_RISING_EDGE]  = "Rise Eg",
    };

    static const char * const AmbaIntNameStr[AMBA_NUM_INTERRUPT] = {
        /* Software Generated Interrupt (SGI): ID0 - ID15 */
        [AMBA_INT_SGI_ID00] = "SOFT_Gen0",
        [AMBA_INT_SGI_ID01] = "SOFT_Gen1",
        [AMBA_INT_SGI_ID02] = "SOFT_Gen2",
        [AMBA_INT_SGI_ID03] = "SOFT_Gen3",
        [AMBA_INT_SGI_ID04] = "SOFT_Gen4",
        [AMBA_INT_SGI_ID05] = "SOFT_Gen5",
        [AMBA_INT_SGI_ID06] = "SOFT_Gen6",
        [AMBA_INT_SGI_ID07] = "SOFT_Gen7",
        [AMBA_INT_SGI_ID08] = "SOFT_Gen8",
        [AMBA_INT_SGI_ID09] = "SOFT_Gen9",
        [AMBA_INT_SGI_ID10] = "SOFT_Gen10",
        [AMBA_INT_SGI_ID11] = "SOFT_Gen11",
        [AMBA_INT_SGI_ID12] = "SOFT_Gen12",
        [AMBA_INT_SGI_ID13] = "SOFT_Gen13",
        [AMBA_INT_SGI_ID14] = "SOFT_Gen14",
        [AMBA_INT_SGI_ID15] = "SOFT_Gen15",

        /* Private Peripheral Interrupt (PPI): ID16 - ID31 */
        [AMBA_INT_PPI_ID016]                                = "PPI_ID16",
        [AMBA_INT_PPI_ID017]                                = "PPI_ID17",
        [AMBA_INT_PPI_ID018]                                = "PPI_ID18",
        [AMBA_INT_PPI_ID019]                                = "PPI_ID19",
        [AMBA_INT_PPI_ID020]                                = "PPI_ID20",
        [AMBA_INT_PPI_ID021]                                = "PPI_ID21",
        [AMBA_INT_PPI_ID022]                                = "PPI_ID22",
        [AMBA_INT_PPI_ID023]                                = "PPI_ID23",
        [AMBA_INT_PPI_ID024]                                = "PPI_ID24",
        [AMBA_INT_PPI_ID025_VIRTUAL_MAINTENANCE]            = "VIRTUAL_MAINTENANCE",
        [AMBA_INT_PPI_ID026_HYPERVISOR_TIMER]               = "HYPERVISOR_TIMER",
        [AMBA_INT_PPI_ID027_VIRTUAL_TIMER]                  = "VIRTUAL_TIMER",
        [AMBA_INT_PPI_ID028_FIQ]                            = "FIQ",
        [AMBA_INT_PPI_ID029_SECURE_PHYSICAL_TIMER]          = "SECURE_PHYSICAL_TIMER",
        [AMBA_INT_PPI_ID030_NONSECURE_PHYSICAL_TIMER]       = "NONSECURE_PHYSICAL_TIMER",
        [AMBA_INT_PPI_ID031_IRQ]                            = "IRQ",

        /* Shared Peripheral Interrupt (SPI): ID32 - ID191 */
        [AMBA_INT_SPI_ID032_CEHU0]                          = "cehu0",
        [AMBA_INT_SPI_ID033_CEHU1]                          = "cuhu1",
        [AMBA_INT_SPI_ID034_CA53_EXTERRIRQ]                 = "Cortex1 nEXTERRIRQ",
        [AMBA_INT_SPI_ID035_CA53_PMUIRQ0]                   = "Cortex1 PMUIRQ[0]",
        [AMBA_INT_SPI_ID036_CA53_PMUIRQ1]                   = "Cortex1 PMUIRQ[1]",
        [AMBA_INT_SPI_ID037_CA53_PMUIRQ2]                   = "Cortex1 PMUIRQ[2]",
        [AMBA_INT_SPI_ID038_CA53_PMUIRQ3]                   = "Cortex1 PMUIRQ[3]",
        [AMBA_INT_SPI_ID039_CAN0]                           = "can0",
        [AMBA_INT_SPI_ID040_CAN1]                           = "can1",
        [AMBA_INT_SPI_ID041_CAN2]                           = "can2",
        [AMBA_INT_SPI_ID042_CAN3]                           = "can3",
        [AMBA_INT_SPI_ID043_CAN4]                           = "can4",
        [AMBA_INT_SPI_ID044_CAN5]                           = "can5",
        [AMBA_INT_SPI_ID045_CA53_INTERRIRQ]                 = "Cortex0 nINTERRIRQ",
        [AMBA_INT_SPI_ID046_I2C_MASTER0]                    = "I2C_MASTER0",
        [AMBA_INT_SPI_ID047_I2C_MASTER1]                    = "I2C_MASTER1",
        [AMBA_INT_SPI_ID048_I2C_MASTER2]                    = "I2C_MASTER2",
        [AMBA_INT_SPI_ID049_I2C_MASTER3]                    = "I2C_MASTER3",
        [AMBA_INT_SPI_ID050_I2C_MASTER4]                    = "I2C_MASTER4",
        [AMBA_INT_SPI_ID051_I2C_MASTER5]                    = "I2C_MASTER5",
        [AMBA_INT_SPI_ID052_I2C_SLAVE]                      = "I2C_SLAVE",
        [AMBA_INT_SPI_ID053_UART_APB]                       = "UART_APB",
        [AMBA_INT_SPI_ID054_TIMER0]                         = "TIMER0",
        [AMBA_INT_SPI_ID055_TIMER1]                         = "TIMER1",
        [AMBA_INT_SPI_ID056_TIMER2]                         = "TIMER2",
        [AMBA_INT_SPI_ID057_TIMER3]                         = "TIMER3",
        [AMBA_INT_SPI_ID058_TIMER4]                         = "TIMER4",
        [AMBA_INT_SPI_ID059_TIMER5]                         = "TIMER5",
        [AMBA_INT_SPI_ID060_TIMER6]                         = "TIMER6",
        [AMBA_INT_SPI_ID061_TIMER7]                         = "TIMER7",
        [AMBA_INT_SPI_ID062_TIMER8]                         = "TIMER8",
        [AMBA_INT_SPI_ID063_TIMER9]                         = "TIMER9",
        [AMBA_INT_SPI_ID064_TIMER10]                        = "TIMER10",
        [AMBA_INT_SPI_ID065_TIMER11]                        = "TIMER11",
        [AMBA_INT_SPI_ID066_TIMER12]                        = "TIMER12",
        [AMBA_INT_SPI_ID067_TIMER13]                        = "TIMER13",
        [AMBA_INT_SPI_ID068_TIMER14]                        = "TIMER14",
        [AMBA_INT_SPI_ID069_TIMER15]                        = "TIMER15",
        [AMBA_INT_SPI_ID070_TIMER16]                        = "TIMER16",
        [AMBA_INT_SPI_ID071_TIMER17]                        = "TIMER17",
        [AMBA_INT_SPI_ID072_TIMER18]                        = "TIMER18",
        [AMBA_INT_SPI_ID073_TIMER19]                        = "TIMER19",
        [AMBA_INT_SPI_ID074_TIMER20]                        = "TIMER20",
        [AMBA_INT_SPI_ID075_TIMER21]                        = "TIMER21",
        [AMBA_INT_SPI_ID076_TIMER22]                        = "TIMER22",
        [AMBA_INT_SPI_ID077_TIMER23]                        = "TIMER23",
        [AMBA_INT_SPI_ID078_TIMER24]                        = "TIMER24",
        [AMBA_INT_SPI_ID079_TIMER25]                        = "TIMER25",
        [AMBA_INT_SPI_ID080_TIMER26]                        = "TIMER26",
        [AMBA_INT_SPI_ID081_TIMER27]                        = "TIMER27",
        [AMBA_INT_SPI_ID082_TIMER28]                        = "TIMER28",
        [AMBA_INT_SPI_ID083_TIMER29]                        = "TIMER29",
        [AMBA_INT_SPI_ID084_WDT]                            = "WATCHDOG_TIMER",
        [AMBA_INT_SPI_ID085_GPIO_GROUP0]                    = "GPIO0",
        [AMBA_INT_SPI_ID086_GPIO_GROUP1]                    = "GPIO1",
        [AMBA_INT_SPI_ID087_GPIO_GROUP2]                    = "GPIO2",
        [AMBA_INT_SPI_ID088_GPIO_GROUP3]                    = "GPIO3",
        [AMBA_INT_SPI_ID089_USB_ID_CHANGE]                  = "USB_DIGITAL_ID_CHANGE",
        [AMBA_INT_SPI_ID090_USB_CONNECT]                    = "USB_CONNECT",
        [AMBA_INT_SPI_ID091_USB_CONNECT_CHANGE]             = "USB_CONNECT_CHANGE",
        [AMBA_INT_SPI_ID092_USB_CHARGE_DETECT]              = "USB_CHARGE_DETECT",
        [AMBA_INT_SPI_ID093_SDIO0_CARD_DETECT]              = "SDXC_CARD_DETECT",
        [AMBA_INT_SPI_ID094_SD_CARD_DETECT]                 = "SD_CARD_DETECT",
        [AMBA_INT_SPI_ID095_ENET0_PMT]                      = "ETHERNET_PMT",
        [AMBA_INT_SPI_ID096_ENET0_SBD]                      = "ETHERNET_SB",
        [AMBA_INT_SPI_ID097_UART_AHB0]                      = "UART_AHB0",
        [AMBA_INT_SPI_ID098_UART_AHB1]                      = "UART_AHB1",
        [AMBA_INT_SPI_ID099_UART_AHB2]                      = "UART_AHB2",
        [AMBA_INT_SPI_ID100_UART_AHB3]                      = "UART_AHB3",
        [AMBA_INT_SPI_ID101_USB_HOST_EHCI]                  = "USB_EHCI",
        [AMBA_INT_SPI_ID102_USB_HOST_OHCI]                  = "USB_OHCI",
        [AMBA_INT_SPI_ID103_USB]                            = "USB",
        [AMBA_INT_SPI_ID104_FIO]                            = "FIO",
        [AMBA_INT_SPI_ID105_FIO_ECC_RPT]                    = "FIO_ECC_RPT",
        [AMBA_INT_SPI_ID106_GDMA]                           = "GDMA",
        [AMBA_INT_SPI_ID107_SDIO0]                          = "SDXC",
        [AMBA_INT_SPI_ID108_SD]                             = "SD",
        [AMBA_INT_SPI_ID109_SPI_NOR]                        = "SPI_NOR",
        [AMBA_INT_SPI_ID110_SSI_MASTER0]                    = "SSI_MASTER0",
        [AMBA_INT_SPI_ID111_SSI_MASTER1]                    = "SSI_MASTER1",
        [AMBA_INT_SPI_ID112_SSI_MASTER2]                    = "SSI_MASTER2",
        [AMBA_INT_SPI_ID113_SSI_MASTER3]                    = "SSI_MASTER3",
        [AMBA_INT_SPI_ID114_SSI_SLAVE]                      = "SSI_SLAVE",
        [AMBA_INT_SPI_ID115_I2S_TX]                         = "I2S0_TX",
        [AMBA_INT_SPI_ID116_I2S_RX]                         = "I2S0_RX",
        [AMBA_INT_SPI_ID117_DMA_ENGINE0]                    = "DMA0",
        [AMBA_INT_SPI_ID118_DMA_ENGINE1]                    = "DMA1",
        [AMBA_INT_SPI_ID119_VOUT_B_INT]                     = "VOUT_B",
        [AMBA_INT_SPI_ID120_VOUT_A_INT]                     = "VOUT_A",
        [AMBA_INT_SPI_ID121_HRNG]                           = "RANDOM_NUM_GENERATOR",
        [AMBA_INT_SPI_ID122_AXI_SOFT_IRQ0]                  = "SOFT_IRQ0",
        [AMBA_INT_SPI_ID123_AXI_SOFT_IRQ1]                  = "SOFT_IRQ1",
        [AMBA_INT_SPI_ID124_AXI_SOFT_IRQ2]                  = "SOFT_IRQ2",
        [AMBA_INT_SPI_ID125_AXI_SOFT_IRQ3]                  = "SOFT_IRQ3",
        [AMBA_INT_SPI_ID126_AXI_SOFT_IRQ4]                  = "SOFT_IRQ4",
        [AMBA_INT_SPI_ID127_AXI_SOFT_IRQ5]                  = "SOFT_IRQ5",
        [AMBA_INT_SPI_ID128_AXI_SOFT_IRQ6]                  = "SOFT_IRQ6",
        [AMBA_INT_SPI_ID129_AXI_SOFT_IRQ7]                  = "SOFT_IRQ7",
        [AMBA_INT_SPI_ID130_AXI_SOFT_IRQ8]                  = "SOFT_IRQ8",
        [AMBA_INT_SPI_ID131_AXI_SOFT_IRQ9]                  = "SOFT_IRQ9",
        [AMBA_INT_SPI_ID132_AXI_SOFT_IRQ10]                 = "SOFT_IRQ10",
        [AMBA_INT_SPI_ID133_AXI_SOFT_IRQ11]                 = "SOFT_IRQ11",
        [AMBA_INT_SPI_ID134_AXI_SOFT_IRQ12]                 = "SOFT_IRQ12",
        [AMBA_INT_SPI_ID135_AXI_SOFT_IRQ13]                 = "SOFT_IRQ13",
        [AMBA_INT_SPI_ID136_DRAM_ERROR]                     = "DRAM_ERROR",
        [AMBA_INT_SPI_ID137_VP0_EXCEPTION]                  = "VP0_EXCEPTION",
        [AMBA_INT_SPI_ID138_VIN0_SLAVE_VSYNC]               = "VIN0_SLAVE_VSYNC",
        [AMBA_INT_SPI_ID139_VIN0_SOF]                       = "VIN0_SOF",
        [AMBA_INT_SPI_ID140_VIN0_MASTER_VSYNC]              = "VIN0_MASTER_VSYNC",
        [AMBA_INT_SPI_ID141_VIN0_LAST_PIXEL]                = "VIN0_LAST_PIXEL",
        [AMBA_INT_SPI_ID142_VIN0_DELAYED_VSYNC]             = "VIN0_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID143_VIN1_SLAVE_VSYNC]               = "VIN1_SLAVE_VSYNC",
        [AMBA_INT_SPI_ID144_VIN1_SOF]                       = "VIN1_SOF",
        [AMBA_INT_SPI_ID145_VIN1_MASTER_VSYNC]              = "VIN1_MASTER_VSYNC",
        [AMBA_INT_SPI_ID146_VIN1_LAST_PIXEL]                = "VIN1_LAST_PIXEL",
        [AMBA_INT_SPI_ID147_VIN1_DELAYED_VSYNC]             = "VIN1_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID148_VIN2_SLAVE_VSYNC]               = "VIN2_SLAVE_VSYNC",
        [AMBA_INT_SPI_ID149_VIN2_SOF]                       = "VIN2_SOF",
        [AMBA_INT_SPI_ID150_VIN2_LAST_PIXEL]                = "VIN2_LAST_PIXEL",
        [AMBA_INT_SPI_ID151_VIN2_DELAYED_VSYNC]             = "VIN2_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID152_VIN3_SLAVE_VSYNC]               = "VIN3_SLAVE_VSYNC",
        [AMBA_INT_SPI_ID153_VIN3_SOF]                       = "VIN3_SOF",
        [AMBA_INT_SPI_ID154_APB_DBG_LOCK_ACCESS]            = "APB_DBG_LOCK_ACCESS",
        [AMBA_INT_SPI_ID155_VIN3_LAST_PIXEL]                = "VIN3_LAST_PIXEL",
        [AMBA_INT_SPI_ID156_VIN3_DELAYED_VSYNC]             = "VIN3_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID157_VIN4_SLAVE_VSYNC]               = "VIN4_SLAVE_VSYNC",
        [AMBA_INT_SPI_ID158_VIN4_SOF]                       = "VIN4_SOF",
        [AMBA_INT_SPI_ID159_AXI_CFG_LOCK_ACCESS]            = "AXI_CFG_LOCK_ACCESS",
        [AMBA_INT_SPI_ID160_VIN4_LAST_PIXEL]                = "VIN4_LAST_PIXEL",
        [AMBA_INT_SPI_ID161_VIN4_DELAYED_VSYNC]             = "VIN4_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID162_SAHB_CFG_LOCK_ACCESS]           = "SAHB_CFG_LOCK_ACCESS",
        [AMBA_INT_SPI_ID163_NSAHB_CFG_LOCK_ACCESS]          = "NSAHB_CFG_LOCK_ACCESS",
        [AMBA_INT_SPI_ID164_ENET1_PMT]                      = "ENET1_PMT",
        [AMBA_INT_SPI_ID165_ENET1_SBD]                      = "ENET1_SBD",
        [AMBA_INT_SPI_ID166_SSI_MASTER4]                    = "SSI_MASTER4",
        [AMBA_INT_SPI_ID167_SSI_MASTER5]                    = "SSI_MASTER5",
        [AMBA_INT_SPI_ID168_WDT_5]                          = "WDT_CR52",
        [AMBA_INT_SPI_ID169_WDT_1]                          = "WDT_CA53_0",
        [AMBA_INT_SPI_ID170_WDT_2]                          = "WDT_CA53_1",
        [AMBA_INT_SPI_ID171_WDT_3]                          = "WDT_CA53_2",
        [AMBA_INT_SPI_ID172_WDT_4]                          = "WDT_CA53_3",
        [AMBA_INT_SPI_ID173_VIN8_SLAVE_VSYNC]               = "VIN8_SLAVE_VSYNC",
        [AMBA_INT_SPI_ID174_VIN8_SOF]                       = "VIN8_SOF",
        [AMBA_INT_SPI_ID175_VIN8_LAST_PIXEL]                = "VIN8_LAST_PIXEL",
        [AMBA_INT_SPI_ID176_VIN8_DELAYED_VSYNC]             = "VIN8_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID177_CODE_VDSP0_IRQ]                 = "CODE_VDSP0",
        [AMBA_INT_SPI_ID178_CODE_VDSP1_IRQ]                 = "CODE_VDSP1",
        [AMBA_INT_SPI_ID179_CODE_VDSP2_IRQ]                 = "CODE_VDSP2",
        [AMBA_INT_SPI_ID180_CODE_VDSP3_IRQ]                 = "CODE_VDSP3",
        [AMBA_INT_SPI_ID181_CODE_VIN0_IRQ]                  = "CODE_VIN0",
        [AMBA_INT_SPI_ID182_CODE_VIN1_IRQ]                  = "CODE_VIN1",
        [AMBA_INT_SPI_ID183_CODE_VIN2_IRQ]                  = "CODE_VIN2",
        [AMBA_INT_SPI_ID184_CODE_VIN3_IRQ]                  = "CODE_VIN3",
        [AMBA_INT_SPI_ID185_CODE_VIN4_IRQ]                  = "CODE_VIN4",
        [AMBA_INT_SPI_ID186]                                = "",
        [AMBA_INT_SPI_ID187]                                = "",
        [AMBA_INT_SPI_ID188]                                = "",
        [AMBA_INT_SPI_ID189_CODE_VIN8_IRQ]                  = "CODE_VIN8",
        [AMBA_INT_SPI_ID190_CODE_VOUT0_IRQ]                 = "CODE_VOUT_0",
        [AMBA_INT_SPI_ID191_CODE_VOUT1_IRQ]                 = "CODE_VOUT_1",
        [AMBA_INT_SPI_ID192_VORC_L2C_EVENT_IRQ]             = "VORC_L2C_EVENT",
        [AMBA_INT_SPI_ID193_VORC_THREAD0_IRQ]               = "VORC_THREAD0_IRQ",
        [AMBA_INT_SPI_ID194_VORC_THREAD1_IRQ]               = "VORC_THREAD1_IRQ",
        [AMBA_INT_SPI_ID195_VORC_THREAD2_IRQ]               = "VORC_THREAD2_IRQ",
        [AMBA_INT_SPI_ID196_VORC_THREAD3_IRQ]               = "VORC_THREAD3_IRQ",
        [AMBA_INT_SPI_ID197_VORC_THREAD4_IRQ]               = "VORC_THREAD4_IRQ",
        [AMBA_INT_SPI_ID198_DDRC0]                          = "DDRC0",
        [AMBA_INT_SPI_ID199_DDRC1]                          = "DDRC1",
        [AMBA_INT_SPI_ID200_IDSP_SAFETY]                    = "IDSP_SAFETY",
        [AMBA_INT_SPI_ID201]                                = "",
        [AMBA_INT_SPI_ID202_CODE_VDSP0_IRQ]                 = "CODE_VDSP0_IRQ",
        [AMBA_INT_SPI_ID203_CODE_VDSP1_IRQ]                 = "CODE_VDSP1_IRQ",
        [AMBA_INT_SPI_ID204_CODE_VDSP2_IRQ]                 = "CODE_VDSP2_IRQ",
        [AMBA_INT_SPI_ID205_CODE_VDSP3_IRQ]                 = "CODE_VDSP3_IRQ",
        [AMBA_INT_SPI_ID206_CODE_VIN0_IRQ]                  = "CODE_VIN0_IRQ",
        [AMBA_INT_SPI_ID207_CODE_VIN1_IRQ]                  = "CODE_VIN1_IRQ",
        [AMBA_INT_SPI_ID208_CODE_VIN2_IRQ]                  = "CODE_VIN2_IRQ",
        [AMBA_INT_SPI_ID209_CODE_VIN3_IRQ]                  = "CODE_VIN3_IRQ",
        [AMBA_INT_SPI_ID210_CODE_VIN4_IRQ]                  = "CODE_VIN4_IRQ",
        [AMBA_INT_SPI_ID211]                                = "",
        [AMBA_INT_SPI_ID212]                                = "",
        [AMBA_INT_SPI_ID213]                                = "",
        [AMBA_INT_SPI_ID214_CODE_VIN8_IRQ]                  = "CODE_VIN8_IRQ",
        [AMBA_INT_SPI_ID215_CODE_VOUT0_IRQ]                 = "CODE_VOUT0_IRQ",
        [AMBA_INT_SPI_ID216_CODE_VOUT1_IRQ]                 = "CODE_VOUT1_IRQ",
        [AMBA_INT_SPI_ID217_VORC_L2C_EVENT_IRQ]             = "VORC_L2C_EVENT_IRQ",
        [AMBA_INT_SPI_ID218_VORC_THREAD0_IRQ]               = "VORC_THREAD0",
        [AMBA_INT_SPI_ID219_VORC_THREAD1_IRQ]               = "VORC_THREAD1",
        [AMBA_INT_SPI_ID220_VORC_THREAD2_IRQ]               = "VORC_THREAD2",
        [AMBA_INT_SPI_ID221_VORC_THREAD3_IRQ]               = "VORC_THREAD3",
        [AMBA_INT_SPI_ID222_VORC_THREAD4_IRQ]               = "VORC_THREAD4",
        [AMBA_INT_SPI_ID223_GPIO_GROUP0]                    = "",
        [AMBA_INT_SPI_ID224_GPIO_GROUP1]                    = "",
        [AMBA_INT_SPI_ID225_GPIO_GROUP2]                    = "",
        [AMBA_INT_SPI_ID226_GPIO_GROUP3]                    = "",
        [AMBA_INT_SPI_ID227_CAN0]                           = "",
        [AMBA_INT_SPI_ID228_CAN1]                           = "",
        [AMBA_INT_SPI_ID229_CAN2]                           = "",
        [AMBA_INT_SPI_ID230_CAN3]                           = "",
        [AMBA_INT_SPI_ID231_CAN4]                           = "",
        [AMBA_INT_SPI_ID232_CAN5]                           = "",
        [AMBA_INT_SPI_ID233_SSI_SLAVE]                      = "",
        [AMBA_INT_SPI_ID234_SSI_MASTER0]                    = "",
        [AMBA_INT_SPI_ID235_SSI_MASTER1]                    = "",
        [AMBA_INT_SPI_ID236_SSI_MASTER2]                    = "",
        [AMBA_INT_SPI_ID237_SSI_MASTER3]                    = "",
        [AMBA_INT_SPI_ID238_SSI_MASTER4]                    = "",
        [AMBA_INT_SPI_ID239_SSI_MASTER5]                    = "",
        [AMBA_INT_SPI_ID240_WDT]                            = "",
        [AMBA_INT_SPI_ID241_WDT_CR52]                       = "",
        [AMBA_INT_SPI_ID242_WDT_CA53_0]                     = "",
        [AMBA_INT_SPI_ID243_WDT_CA53_1]                     = "",
        [AMBA_INT_SPI_ID244_WDT_CA53_2]                     = "",
        [AMBA_INT_SPI_ID245_WDT_CA53_3]                     = "",
        [AMBA_INT_SPI_ID246_I2C_SLAVE]                      = "",
        [AMBA_INT_SPI_ID247_I2C_MASTER0]                    = "",
        [AMBA_INT_SPI_ID248_I2C_MASTER1]                    = "",
        [AMBA_INT_SPI_ID249_I2C_MASTER2]                    = "",
        [AMBA_INT_SPI_ID250_I2C_MASTER3]                    = "",
        [AMBA_INT_SPI_ID251_I2C_MASTER4]                    = "",
        [AMBA_INT_SPI_ID252_I2C_MASTER5]                    = "",
        [AMBA_INT_SPI_ID253_DMA_ENGINE0]                    = "",
        [AMBA_INT_SPI_ID254_DMA_ENGINE1]                    = "",
        [AMBA_INT_SPI_ID255]                                = "",
    };
    /* ID */
    LogFunc("[");
    Diag_PrintFormattedInt(LogFunc, "%d", IntID, 3U);
    LogFunc("]:\t");

    /* Name, %-30s */
    LogFunc(AmbaIntNameStr[IntID]);
    StringLength = IO_UtilityStringLength(AmbaIntNameStr[IntID]);
    if (StringLength < 30U) {
        for (i = 0; i < (30U - StringLength); i++) {
            LogFunc(" ");
        }
    }

    /* Type */
    LogFunc("     ");
    LogFunc(AmbaIntTypeStr[IntInfo->IrqType]);

    /* SenseType */
    LogFunc("   ");
    LogFunc(AmbaIntSenseStr[IntInfo->TriggerType]);

    /* Target Core, %13d */
    Diag_PrintFormattedInt(LogFunc, "%X", IntInfo->CpuTargets, 13U);

    /* Enable, %8d */
    Diag_PrintFormattedInt(LogFunc, "%d", IntInfo->IrqEnable, 8U);

    /* Count, %12d */
    Diag_PrintFormattedInt(LogFunc, "%d", Count, 12U);

    LogFunc("\n");
}

/**
 *  AmbaDiag_IntShowInfo - Show IRQ info
 *  @param[in] IntID interrupt id
 *  @return error code
 */
UINT32 AmbaDiag_IntShowInfo(UINT32 IntID, AMBA_SYS_LOG_f LogFunc)
{
    static UINT32 DiagIrqCountTmp[AMBA_NUM_INTERRUPT];
    AMBA_INT_INFO_s IntInfo;
    UINT32 i, RetVal = INT_ERR_NONE;

    if ((IntID >= (UINT32)AMBA_NUM_INTERRUPT) && (IntID != 0xFFFFFFFFU)) {
        LogFunc("Unknown interrupt ID: ");
        Diag_PrintFormattedInt(LogFunc, "%d", IntID, 3U);
        LogFunc("\n\r");
        RetVal = INT_ERR_ARG;
    } else {
        LogFunc("------------------------------------------------------------------------------------------\n");
        LogFunc(" ID     NAME                               TYPE  SENSE      CORE_MASK  ENABLE     COUNTER \n");
        LogFunc("------------------------------------------------------------------------------------------\n");

        if (IntID == 0xFFFFFFFFU) {
            /* show all */
            for (i = 0; i < (UINT32)AMBA_NUM_INTERRUPT; i++) {
                DiagIrqCountTmp[i] = DiagIrqCount[i];
            }
            for (i = 0; i < (UINT32)AMBA_NUM_INTERRUPT; i++) {
                if (AmbaRTSL_GicGetIntInfo(i, &IntInfo) == 0U) {
                    SHELL_PrintFormatedIrqInfo(LogFunc, i, &IntInfo, DiagIrqCountTmp[i]);
                }
            }
        } else {
            if (AmbaRTSL_GicGetIntInfo(IntID, &IntInfo) == 0U) {
                SHELL_PrintFormatedIrqInfo(LogFunc, IntID, &IntInfo, DiagIrqCount[IntID]);
            }
        }
        LogFunc("------------------------------------------------------------------------------------------\n");
    }

    return RetVal;
}

