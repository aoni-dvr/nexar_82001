/**
 *  @file AmbaDiag_Int.c
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

    StringLength = IO_UtilityStringPrintUInt32(StrBuf, (UINT32)sizeof(StrBuf), pFmtString, 1U, ArgsUInt32);
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
        [AMBA_INT_SGI_ID0]                          = "SOFT_Gen0",
        [AMBA_INT_SGI_ID1]                          = "SOFT_Gen1",
        [AMBA_INT_SGI_ID2]                          = "SOFT_Gen2",
        [AMBA_INT_SGI_ID3]                          = "SOFT_Gen3",
        [AMBA_INT_SGI_ID4]                          = "SOFT_Gen4",
        [AMBA_INT_SGI_ID5]                          = "SOFT_Gen5",
        [AMBA_INT_SGI_ID6]                          = "SOFT_Gen6",
        [AMBA_INT_SGI_ID7]                          = "SOFT_Gen7",
        [AMBA_INT_SGI_ID8]                          = "SOFT_Gen8",
        [AMBA_INT_SGI_ID9]                          = "SOFT_Gen9",
        [AMBA_INT_SGI_ID10]                         = "SOFT_Gen10",
        [AMBA_INT_SGI_ID11]                         = "SOFT_Gen11",
        [AMBA_INT_SGI_ID12]                         = "SOFT_Gen12",
        [AMBA_INT_SGI_ID13]                         = "SOFT_Gen13",
        [AMBA_INT_SGI_ID14]                         = "SOFT_Gen14",
        [AMBA_INT_SGI_ID15]                         = "SOFT_Gen15",

        /* Private Peripheral Interrupt (PPI): ID16 - ID31 */
        [AMBA_INT_PPI_ID16]                         = "PPI_ID16",
        [AMBA_INT_PPI_ID17]                         = "PPI_ID17",
        [AMBA_INT_PPI_ID18]                         = "PPI_ID18",
        [AMBA_INT_PPI_ID19]                         = "PPI_ID19",
        [AMBA_INT_PPI_ID20]                         = "PPI_ID20",
        [AMBA_INT_PPI_ID21]                         = "PPI_ID21",
        [AMBA_INT_PPI_ID22]                         = "PPI_ID22",
        [AMBA_INT_PPI_ID23]                         = "PPI_ID23",
        [AMBA_INT_PPI_ID24]                         = "PPI_ID24",
        [AMBA_INT_PPI_ID25_VIRTUAL_MAINTENANCE]     = "VIRTUAL_MAINTENANCE",
        [AMBA_INT_PPI_ID26_HYPERVISOR_TIMER]        = "HYPERVISOR_TIMER",
        [AMBA_INT_PPI_ID27_VIRTUAL_TIMER]           = "VIRTUAL_TIMER",
        [AMBA_INT_PPI_ID28_FIQ]                     = "FIQ",
        [AMBA_INT_PPI_ID29_SECURE_PHYSICAL_TIMER]   = "SECURE_PHYSICAL_TIMER",
        [AMBA_INT_PPI_ID30_NONSECURE_PHYSICAL_TIMER] = "NONSECURE_PHYSICAL_TIMER",
        [AMBA_INT_PPI_ID31_IRQ]                     = "IRQ",

        /* Shared Peripheral Interrupt (SPI): ID32 - ID191 */
        [AMBA_INT_SPI_ID32]                         = "",
        [AMBA_INT_SPI_ID33]                         = "",
        [AMBA_INT_SPI_ID34]                         = "",
        [AMBA_INT_SPI_ID35_PMUIRQ0]                 = "PMUIRQ0",
        [AMBA_INT_SPI_ID36_PMUIRQ1]                 = "PMUIRQ1",
        [AMBA_INT_SPI_ID37_CLUSTER_PMUIRQ]          = "CLUSTER_PMUIRQ",
        [AMBA_INT_SPI_ID38]                         = "",
        [AMBA_INT_SPI_ID39_CAN0]                    = "CAN0",
        [AMBA_INT_SPI_ID40_CAN1]                    = "CAN1",
        [AMBA_INT_SPI_ID41_LPI]                     = "LPI",
        [AMBA_INT_SPI_ID42_LPI1]                    = "LPI1",
        [AMBA_INT_SPI_ID43]                         = "",
        [AMBA_INT_SPI_ID44]                         = "",
        [AMBA_INT_SPI_ID45]                         = "",
        [AMBA_INT_SPI_ID46_I2C_MASTER0]             = "I2C_MASTER0",
        [AMBA_INT_SPI_ID47_I2C_MASTER1]             = "I2C_MASTER1",
        [AMBA_INT_SPI_ID48_I2C_MASTER2]             = "I2C_MASTER2",
        [AMBA_INT_SPI_ID49_I2C_MASTER3]             = "I2C_MASTER3",
        [AMBA_INT_SPI_ID50_I2C_MASTER4]             = "I2C_MASTER4",
        [AMBA_INT_SPI_ID51_I2C_MASTER5]             = "I2C_MASTER5",
        [AMBA_INT_SPI_ID52_I2C_SLAVE]               = "I2C_SLAVE",
        [AMBA_INT_SPI_ID53_UART_APB]                = "UART_APB",
        [AMBA_INT_SPI_ID54_TIMER0]                  = "TIMER0",
        [AMBA_INT_SPI_ID55_TIMER1]                  = "TIMER1",
        [AMBA_INT_SPI_ID56_TIMER2]                  = "TIMER2",
        [AMBA_INT_SPI_ID57_TIMER3]                  = "TIMER3",
        [AMBA_INT_SPI_ID58_TIMER4]                  = "TIMER4",
        [AMBA_INT_SPI_ID59_TIMER5]                  = "TIMER5",
        [AMBA_INT_SPI_ID60_TIMER6]                  = "TIMER6",
        [AMBA_INT_SPI_ID61_TIMER7]                  = "TIMER7",
        [AMBA_INT_SPI_ID62_TIMER8]                  = "TIMER8",
        [AMBA_INT_SPI_ID63_TIMER9]                  = "TIMER9",

        [AMBA_INT_SPI_ID64_TIMER10]                 = "TIMER10",
        [AMBA_INT_SPI_ID65_TIMER11]                 = "TIMER11",
        [AMBA_INT_SPI_ID66_TIMER12]                 = "TIMER12",
        [AMBA_INT_SPI_ID67_TIMER13]                 = "TIMER13",
        [AMBA_INT_SPI_ID68_TIMER14]                 = "TIMER14",
        [AMBA_INT_SPI_ID69_TIMER15]                 = "TIMER15",
        [AMBA_INT_SPI_ID70_TIMER16]                 = "TIMER16",
        [AMBA_INT_SPI_ID71_TIMER17]                 = "TIMER17",
        [AMBA_INT_SPI_ID72_TIMER18]                 = "TIMER18",
        [AMBA_INT_SPI_ID73_TIMER19]                 = "TIMER19",
        [AMBA_INT_SPI_ID74_ADC]                     = "ADC",
        [AMBA_INT_SPI_ID75_WATCHDOG_TIMER]          = "WATCHDOG_TIMER",
        [AMBA_INT_SPI_ID76_GPIO0]                   = "GPIO0",
        [AMBA_INT_SPI_ID77_GPIO1]                   = "GPIO1",
        [AMBA_INT_SPI_ID78_GPIO2]                   = "GPIO2",
        [AMBA_INT_SPI_ID79_GPIO3]                   = "GPIO3",
        [AMBA_INT_SPI_ID80_GPIO4]                   = "GPIO4",
        [AMBA_INT_SPI_ID81_IR_INTERFACE]            = "IR_INTERFACE",
        [AMBA_INT_SPI_ID82_HDMITX]                  = "HDMITX",
        [AMBA_INT_SPI_ID83_USB1_CONNECT]            = "USB1_CONNECT",
        [AMBA_INT_SPI_ID84_USB0_CONNECT]            = "USB0_CONNECT",
        [AMBA_INT_SPI_ID85_USB0_CONNECT_CHANGE]     = "USB0_CONNECT_CHANGE",
        [AMBA_INT_SPI_ID86_USBP_CHARGE_DETECT]      = "USBP_CHARGE_DETECT",
        [AMBA_INT_SPI_ID87_SDIO0_CARD_DETECT]       = "SDIO0_CARD_DETECT",
        [AMBA_INT_SPI_ID88_SD_CARD_DETECT]          = "SD_CARD_DETECT",
        [AMBA_INT_SPI_ID89_ENET_PMT]                = "ENET_PMT",
        [AMBA_INT_SPI_ID90_ENET_SBD]                = "ENET_SBD",
        [AMBA_INT_SPI_ID91_ENET_PMT1]               = "ENET_PMT1",
        [AMBA_INT_SPI_ID92_ENET_SBD1]               = "ENET_SBD1",
        [AMBA_INT_SPI_ID93_UART_AHB0]               = "UART_AHB0",
        [AMBA_INT_SPI_ID94_UART_AHB1]               = "UART_AHB1",
        [AMBA_INT_SPI_ID95_UART_AHB2]               = "UART_AHB2",

        [AMBA_INT_SPI_ID96_UART_AHB3]               = "UART_AHB3",
        [AMBA_INT_SPI_ID97]                         = "",
        [AMBA_INT_SPI_ID98_USB1_CONNECT_CHANGE]     = "USB1_CONNECT_CHANGE",
        [AMBA_INT_SPI_ID99_USB]                     = "USB",
        [AMBA_INT_SPI_ID100_FIO]                    = "FIO",
        [AMBA_INT_SPI_ID101_FIO_ECC_RPT]            = "FIO_ECC_RPT",
        [AMBA_INT_SPI_ID102_GDMA]                   = "GDMA",
        [AMBA_INT_SPI_ID103_SDIO0]                  = "SDIO0",
        [AMBA_INT_SPI_ID104_SD]                     = "SD",
        [AMBA_INT_SPI_ID105_SPI_NOR]                = "SPI_NOR",
        [AMBA_INT_SPI_ID106_SSI_MASTER0]            = "SSI_MASTER0",
        [AMBA_INT_SPI_ID107_SSI_MASTER1]            = "SSI_MASTER1",
        [AMBA_INT_SPI_ID108_SSI_MASTER2]            = "SSI_MASTER2",
        [AMBA_INT_SPI_ID109_SSI_MASTER3]            = "SSI_MASTER3",
        [AMBA_INT_SPI_ID110_SSI_MASTER4]            = "SSI_MASTER4",
        [AMBA_INT_SPI_ID111_SSI_MASTER5]            = "SSI_MASTER5",
        [AMBA_INT_SPI_ID112_SSI_SLAVE]              = "SSI_SLAVE",
        [AMBA_INT_SPI_ID113_I2S0_TX]                = "I2S0_TX",
        [AMBA_INT_SPI_ID114_I2S0_RX]                = "I2S0_RX",
        [AMBA_INT_SPI_ID115_I2S1_TX]                = "I2S1_TX",
        [AMBA_INT_SPI_ID116_I2S1_RX]                = "I2S1_RX",
        [AMBA_INT_SPI_ID117_DMA0]                   = "DMA0",
        [AMBA_INT_SPI_ID118_DMA1]                   = "DMA1",
        [AMBA_INT_SPI_ID119_VOUT_B_INT]             = "VOUT_B_INT",
        [AMBA_INT_SPI_ID120_VOUT_A_INT]             = "VOUT_A_INT",
        [AMBA_INT_SPI_ID121_RANDOM_NUM_GENERATOR]   = "RANDOM_NUM_GENERATOR",
        [AMBA_INT_SPI_ID122_AXI_SOFT_IRQ0]          = "AXI_SOFT_IRQ0",
        [AMBA_INT_SPI_ID123_AXI_SOFT_IRQ1]          = "AXI_SOFT_IRQ1",
        [AMBA_INT_SPI_ID124_AXI_SOFT_IRQ2]          = "AXI_SOFT_IRQ2",
        [AMBA_INT_SPI_ID125_AXI_SOFT_IRQ3]          = "AXI_SOFT_IRQ3",
        [AMBA_INT_SPI_ID126_AXI_SOFT_IRQ4]          = "AXI_SOFT_IRQ4",
        [AMBA_INT_SPI_ID127_AXI_SOFT_IRQ5]          = "AXI_SOFT_IRQ5",

        [AMBA_INT_SPI_ID128_AXI_SOFT_IRQ6]          = "AXI_SOFT_IRQ6",
        [AMBA_INT_SPI_ID129_AXI_SOFT_IRQ7]          = "AXI_SOFT_IRQ7",
        [AMBA_INT_SPI_ID130_AXI_SOFT_IRQ8]          = "AXI_SOFT_IRQ8",
        [AMBA_INT_SPI_ID131_AXI_SOFT_IRQ9]          = "AXI_SOFT_IRQ9",
        [AMBA_INT_SPI_ID132_AXI_SOFT_IRQ10]         = "AXI_SOFT_IRQ10",
        [AMBA_INT_SPI_ID133_AXI_SOFT_IRQ11]         = "AXI_SOFT_IRQ11",
        [AMBA_INT_SPI_ID134_AXI_SOFT_IRQ12]         = "AXI_SOFT_IRQ12",
        [AMBA_INT_SPI_ID135_AXI_SOFT_IRQ13]         = "AXI_SOFT_IRQ13",
        [AMBA_INT_SPI_ID136_DRAM_ERROR]             = "DRAM_ERROR",
        [AMBA_INT_SPI_ID137_VP0_EXCEPTION]          = "VP0_EXCEPTION",
        [AMBA_INT_SPI_ID138_VIN4_VSYNC]             = "VIN4_VSYNC",
        [AMBA_INT_SPI_ID139_VIN4_SOF]               = "VIN4_SOF",
        [AMBA_INT_SPI_ID140_VIN4_MASTER_VSYNC]      = "VIN4_MASTER_VSYNC",
        [AMBA_INT_SPI_ID141_VIN4_LAST_PIXEL]        = "VIN4_LAST_PIXEL",
        [AMBA_INT_SPI_ID142_VIN4_DELAYED_VSYNC]     = "VIN4_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID143_VIN0_VSYNC]             = "VIN0_VSYNC",
        [AMBA_INT_SPI_ID144_VIN0_SOF]               = "VIN0_SOF",
        [AMBA_INT_SPI_ID145_VIN0_MASTER_VSYNC]      = "VIN0_MASTER_VSYNC",
        [AMBA_INT_SPI_ID146_VIN0_LAST_PIXEL]        = "VIN0_LAST_PIXEL",
        [AMBA_INT_SPI_ID147_VIN0_DELAYED_VSYNC]     = "VIN0_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID148_VIN1_VSYNC]             = "VIN1_VSYNC",
        [AMBA_INT_SPI_ID149_VIN1_SOF]               = "VIN1_SOF",
        [AMBA_INT_SPI_ID150_VIN1_LAST_PIXEL]        = "VIN1_LAST_PIXEL",
        [AMBA_INT_SPI_ID151_VIN1_DELAYED_VSYNC]     = "VIN1_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID152_VIN2_VSYNC]             = "VIN2_VSYNC",
        [AMBA_INT_SPI_ID153_VIN2_SOF]               = "VIN2_SOF",
        [AMBA_INT_SPI_ID154_VIN2_LAST_PIXEL]        = "VIN2_LAST_PIXEL",
        [AMBA_INT_SPI_ID155_VIN2_DELAYED_VSYNC]     = "VIN2_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID156_VIN3_VSYNC]             = "VIN3_VSYNC",
        [AMBA_INT_SPI_ID157_VIN3_SOF]               = "VIN3_SOF",
        [AMBA_INT_SPI_ID158_VIN3_LAST_PIXEL]        = "VIN3_LAST_PIXEL",
        [AMBA_INT_SPI_ID159_VIN3_DELAYED_VSYNC]     = "VIN3_DELAYED_VSYNC",

        [AMBA_INT_SPI_ID160_SDIO1_CD_CHANGE]        = "SDIO1_CD_CHANGE",
        [AMBA_INT_SPI_ID161_SDIO1]                  = "SDIO1",
        [AMBA_INT_SPI_ID162_VIN5_VSYNC]             = "VIN5_VSYNC",
        [AMBA_INT_SPI_ID163_VIN5_SOF]               = "VIN5_SOF",
        [AMBA_INT_SPI_ID164_VIN5_LAST_PIXEL]        = "VIN5_LAST_PIXEL",
        [AMBA_INT_SPI_ID165_VIN5_DELAYED_VSYNC]     = "VIN5_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID166_VIN6_VSYNC]             = "VIN6_VSYNC",
        [AMBA_INT_SPI_ID167_VIN6_SOF]               = "VIN6_SOF",
        [AMBA_INT_SPI_ID168_VIN6_LAST_PIXEL]        = "VIN6_LAST_PIXEL",
        [AMBA_INT_SPI_ID169_VIN6_DELAYED_VSYNC]     = "VIN6_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID170_VIN7_VSYNC]             = "VIN7_VSYNC",
        [AMBA_INT_SPI_ID171_VIN7_SOF]               = "VIN7_SOF",
        [AMBA_INT_SPI_ID172_VIN7_LAST_PIXEL]        = "VIN7_LAST_PIXEL",
        [AMBA_INT_SPI_ID173_VIN7_DELAYED_VSYNC]     = "VIN7_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID174_CODE_VDSP0_IRQ0]        = "CODE_VDSP0_IRQ0",
        [AMBA_INT_SPI_ID175_CODE_VDSP0_IRQ1]        = "CODE_VDSP0_IRQ1",
        [AMBA_INT_SPI_ID176_CODE_VDSP0_IRQ2]        = "CODE_VDSP0_IRQ2",
        [AMBA_INT_SPI_ID177_CODE_VDSP0_IRQ3]        = "CODE_VDSP0_IRQ3",
        [AMBA_INT_SPI_ID178_CODE_VDSP1_IRQ0]        = "CODE_VDSP1_IRQ0",
        [AMBA_INT_SPI_ID179_CODE_VDSP1_IRQ1]        = "CODE_VDSP1_IRQ1",
        [AMBA_INT_SPI_ID180_CODE_VDSP1_IRQ2]        = "CODE_VDSP1_IRQ2",
        [AMBA_INT_SPI_ID181_CODE_VDSP1_IRQ3]        = "CODE_VDSP1_IRQ3",
        [AMBA_INT_SPI_ID182_CODE_VDSP2_IRQ0]        = "CODE_VDSP2_IRQ0",
        [AMBA_INT_SPI_ID183_CODE_VDSP2_IRQ1]        = "CODE_VDSP2_IRQ1",
        [AMBA_INT_SPI_ID184_CODE_VDSP2_IRQ2]        = "CODE_VDSP2_IRQ2",
        [AMBA_INT_SPI_ID185_CODE_VDSP2_IRQ3]        = "CODE_VDSP2_IRQ3",
        [AMBA_INT_SPI_ID186_CODE_VDSP3_IRQ0]        = "CODE_VDSP3_IRQ0",
        [AMBA_INT_SPI_ID187_CODE_VDSP3_IRQ1]        = "CODE_VDSP3_IRQ1",
        [AMBA_INT_SPI_ID188_CODE_VDSP3_IRQ2]        = "CODE_VDSP3_IRQ2",
        [AMBA_INT_SPI_ID189_CODE_VDSP3_IRQ3]        = "CODE_VDSP3_IRQ3",
        [AMBA_INT_SPI_ID190_CODE_VIN0_IRQ]          = "CODE_VIN0_IRQ",
        [AMBA_INT_SPI_ID191_CODE_VIN1_IRQ]          = "CODE_VIN1_IRQ",

        [AMBA_INT_SPI_ID192_CODE_VIN2_IRQ]          = "CODE_VIN2_IRQ",
        [AMBA_INT_SPI_ID193_CODE_VIN3_IRQ]          = "CODE_VIN3_IRQ",
        [AMBA_INT_SPI_ID194_CODE_VIN4_IRQ]          = "CODE_VIN4_IRQ",
        [AMBA_INT_SPI_ID195_CODE_VIN5_IRQ]          = "CODE_VIN5_IRQ",
        [AMBA_INT_SPI_ID196_CODE_VIN6_IRQ]          = "CODE_VIN6_IRQ",
        [AMBA_INT_SPI_ID197_CODE_VIN7_IRQ]          = "CODE_VIN7_IRQ",
        [AMBA_INT_SPI_ID198_CODE_VOUTA_IRQ]         = "CODE_VOUTA_IRQ",
        [AMBA_INT_SPI_ID199_CODE_VOUTB_IRQ]         = "CODE_VOUTB_IRQs",
        [AMBA_INT_SPI_ID200_I2C_ARM_ERROR]          = "I2C_ARM_ERROR",
        [AMBA_INT_SPI_ID201_VORC_THREAD0_IRQ]       = "VORC_THREAD0_IRQ",
        [AMBA_INT_SPI_ID202_VORC_THREAD1_IRQ]       = "VORC_THREAD1_IRQ",
        [AMBA_INT_SPI_ID203_VORC_THREAD2_IRQ]       = "VORC_THREAD2_IRQ",
        [AMBA_INT_SPI_ID204_VORC_THREAD3_IRQ]       = "VORC_THREAD3_IRQ",
        [AMBA_INT_SPI_ID205_VORC_THREAD4_IRQ]       = "VORC_THREAD4_IRQ",
        [AMBA_INT_SPI_ID206_DDR_HOST0]              = "DDR_HOST0",
        [AMBA_INT_SPI_ID207_DDR_HOST1]              = "DDR_HOST1",
        [AMBA_INT_SPI_ID208_VIN8_VSYNC]             = "VIN8_VSYNC",
        [AMBA_INT_SPI_ID209_VIN8_SOF]               = "VIN8_SOF",
        [AMBA_INT_SPI_ID210_VIN8_LAST_PIXEL]        = "VIN8_LAST_PIXEL",
        [AMBA_INT_SPI_ID211_VIN8_DELAYED_VSYNC]     = "VIN8_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID212_VIN9_VSYNC]             = "VIN9_VSYNC",
        [AMBA_INT_SPI_ID213_VIN9_SOF]               = "VIN9_SOF",
        [AMBA_INT_SPI_ID214_VIN9_LAST_PIXEL]        = "VIN9_LAST_PIXEL",
        [AMBA_INT_SPI_ID215_VIN9_DELAYED_VSYNC]     = "VIN9_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID216_VIN10_VSYNC]            = "VIN10_VSYNC",
        [AMBA_INT_SPI_ID217_VIN10_SOF]              = "VIN10_SOF",
        [AMBA_INT_SPI_ID218_VIN10_LAST_PIXEL]       = "VIN10_LAST_PIXEL",
        [AMBA_INT_SPI_ID219_VIN10_DELAYED_VSYNC]    = "VIN10_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID220_VIN11_VSYNC]            = "VIN11_VSYNC",
        [AMBA_INT_SPI_ID221_VIN11_SOF]              = "VIN11_SOF",
        [AMBA_INT_SPI_ID222_VIN11_LAST_PIXEL]       = "VIN11_LAST_PIXEL",
        [AMBA_INT_SPI_ID223_VIN11_DELAYED_VSYNC]    = "VIN11_DELAYED_VSYNC",

        [AMBA_INT_SPI_ID224_VIN12_VSYNC]            = "VIN12_VSYNC",
        [AMBA_INT_SPI_ID225_VIN12_SOF]              = "VIN12_SOF",
        [AMBA_INT_SPI_ID226_VIN12_LAST_PIXEL]       = "VIN12_LAST_PIXEL",
        [AMBA_INT_SPI_ID227_VIN12_DELAYED_VSYNC]    = "VIN12_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID228_VIN13_VSYNC]            = "VIN13_VSYNC",
        [AMBA_INT_SPI_ID229_VIN13_SOF]              = "VIN13_SOF",
        [AMBA_INT_SPI_ID230_VIN13_LAST_PIXEL]       = "VIN13_LAST_PIXEL",
        [AMBA_INT_SPI_ID231_VIN13_DELAYED_VSYNC]    = "VIN13_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID232_IDSP0_ARM_SAFETY]       = "IDSP0_ARM_SAFETY",
        [AMBA_INT_SPI_ID233_IDSP1_ARM_SAFETY]       = "IDSP1_ARM_SAFETY",
        [AMBA_INT_SPI_ID234_IDSPV_ARM_SAFETY]       = "IDSPV_ARM_SAFETY",
        [AMBA_INT_SPI_ID235_CODE_VIN8_IRQ]          = "CODE_VIN8_IRQ",
        [AMBA_INT_SPI_ID236_CODE_VIN9_IRQ]          = "CODE_VIN9_IRQ",
        [AMBA_INT_SPI_ID237_CODE_VIN10_IRQ]         = "CODE_VIN10_IRQ",
        [AMBA_INT_SPI_ID238_CODE_VIN11_IRQ]         = "CODE_VIN11_IRQ",
        [AMBA_INT_SPI_ID239_CODE_VIN12_IRQ]         = "CODE_VIN12_IRQ",
        [AMBA_INT_SPI_ID240_CODE_VIN13_IRQ]         = "CODE_VIN13_IRQ",
        [AMBA_INT_SPI_ID241_VOUT_C_INT]             = "VOUT_C_INT",
        [AMBA_INT_SPI_ID242_SDIO0_WAKEUP]           = "SDIO0_WAKEUP",
        [AMBA_INT_SPI_ID243_SD_WAKEUP]              = "SD_WAKEUP",
        [AMBA_INT_SPI_ID244_PCIE_PHY_IN0]           = "PCIE_PHY_IN0",
        [AMBA_INT_SPI_ID245_PCIE_PHY_IN1]           = "PCIE_PHY_IN1",
        [AMBA_INT_SPI_ID246_PCIE_PHY_IN2]           = "PCIE_PHY_IN2",
        [AMBA_INT_SPI_ID247_PCIE_PHY_IN3]           = "PCIE_PHY_IN3",
        [AMBA_INT_SPI_ID248_USB32_PHY_IN0]          = "USB32_PHY_IN0",
        [AMBA_INT_SPI_ID249_FAULT_IRQ0]             = "FAULT_IRQ0",
        [AMBA_INT_SPI_ID250_FAULT_IRQ1]             = "FAULT_IRQ1",
        [AMBA_INT_SPI_ID251_FAULT_IRQ2]             = "FAULT_IRQ2",
        [AMBA_INT_SPI_ID252_ERR_IRQ0]               = "ERR_IRQ0",
        [AMBA_INT_SPI_ID253_ERR_IRQ1]               = "ERR_IRQ1",
        [AMBA_INT_SPI_ID254_ERR_IRQ2]               = "ERR_IRQ2",
        [AMBA_INT_SPI_ID255_USB32C]                 = "USB32C",

        [AMBA_INT_SPI_ID256_USB32C_OTG]                 = "USB32C_OTG",
        [AMBA_INT_SPI_ID257_USB32C_HOST_SYSYEM_ERROR]   = "USB32C_HOST_SYSYEM_ERROR",
        [AMBA_INT_SPI_ID258_USB32C_ITP_EXTENDED]        = "USB32C_ITP_EXTENDED",
        [AMBA_INT_SPI_ID259_PCIEC_DMA]                  = "PCIEC_DMA",
        [AMBA_INT_SPI_ID260_PCIEC_LOCAL]                = "PCIEC_LOCAL",
        [AMBA_INT_SPI_ID261_PCIEC_INTA_OUT]             = "PCIEC_INTA_OUT",
        [AMBA_INT_SPI_ID262_PCIEC_INTB_OUT]             = "PCIEC_INTB_OUT",
        [AMBA_INT_SPI_ID263_PCIEC_INTC_OUT]             = "PCIEC_INTC_OUT",
        [AMBA_INT_SPI_ID264_PCIEC_INTD_OUT]             = "PCIEC_INTD_OUT",
        [AMBA_INT_SPI_ID265_PCIEC_PHY_OUT]              = "PCIEC_PHY_OUT",
        [AMBA_INT_SPI_ID266_PCIEC_INT_ACK_EXTENDED]     = "PCIEC_INT_ACK_EXTENDED",
        [AMBA_INT_SPI_ID267_PCIEC_POWER_STATE_CHANGE]   = "PCIEC_POWER_STATE_CHANGE",
        [AMBA_INT_SPI_ID268_PCIEC_HOT_RESET]            = "PCIEC_HOT_RESET",
        [AMBA_INT_SPI_ID269_PCIEC_LINK_DOWN_RESET]      = "PCIEC_LINK_DOWN_RESET",
        [AMBA_INT_SPI_ID270_PCIEC_CORRECTABLE_ERROR]    = "PCIEC_CORRECTABLE_ERROR",
        [AMBA_INT_SPI_ID271_PCIEC_NON_FATAL_ERROR]      = "PCIEC_NON_FATAL_ERROR",
        [AMBA_INT_SPI_ID272_PCIEC_FATAL_ERROR]          = "PCIEC_FATAL_ERROR",
        [AMBA_INT_SPI_ID273_CODE_VOUTC_IRQ]             = "CODE_VOUTC_IRQ",
        [AMBA_INT_SPI_ID274]                            = "",
        [AMBA_INT_SPI_ID275]                            = "",
        [AMBA_INT_SPI_ID276]                            = "",
        [AMBA_INT_SPI_ID277]                            = "",
        [AMBA_INT_SPI_ID278]                            = "",
        [AMBA_INT_SPI_ID279]                            = "",
        [AMBA_INT_SPI_ID280]                            = "",
        [AMBA_INT_SPI_ID281]                            = "",
        [AMBA_INT_SPI_ID282]                            = "",
        [AMBA_INT_SPI_ID283]                            = "",
        [AMBA_INT_SPI_ID284]                            = "",
        [AMBA_INT_SPI_ID285]                            = "",
        [AMBA_INT_SPI_ID286]                            = "",
        [AMBA_INT_SPI_ID287]                            = "",

    };

    /* ID */
    LogFunc("[");
    Diag_PrintFormattedInt(LogFunc, "%d", IntID, 3U);
    LogFunc("]:\t");

    /* Name, %-30s */
    LogFunc(AmbaIntNameStr[IntID]);
    StringLength = (UINT32)IO_UtilityStringLength(AmbaIntNameStr[IntID]);
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

