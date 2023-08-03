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
    static const char *_AmbaIntTypeStr[2] = {
        [0] = "IRQ",
        [1] = "FIQ",
    };

    static const char *_AmbaIntSenseStr[2] = {
        [INT_TRIG_HIGH_LEVEL]   = "High Lv",
        [INT_TRIG_RISING_EDGE]  = "Rise Eg",
    };

    static const char *_AmbaIntNameStr[AMBA_NUM_INTERRUPT] = {
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
        [AMBA_INT_PPI_ID30_NONSECURE_PHYSICAL_TIMER]= "NONSECURE_PHYSICAL_TIMER",
        [AMBA_INT_PPI_ID31_IRQ]                     = "IRQ",

        /* Shared Peripheral Interrupt (SPI): ID32 - ID191 */
        [AMBA_INT_SPI_ID32]                         = "",
        [AMBA_INT_SPI_ID33]                         = "",
        [AMBA_INT_SPI_ID34_CORTEX0_EXTERRIRQ]       = "Cortex0 nEXTERRIRQ",
        [AMBA_INT_SPI_ID35]                         = "",
        [AMBA_INT_SPI_ID36_CORTEX0_PMUIRQ0]         = "Cortex0 PMUIRQ[0]",
        [AMBA_INT_SPI_ID37_CORTEX0_PMUIRQ1]         = "Cortex0 PMUIRQ[1]",
        [AMBA_INT_SPI_ID38]                         = "",
        [AMBA_INT_SPI_ID39]                         = "",
        [AMBA_INT_SPI_ID40]                         = "",
        [AMBA_INT_SPI_ID41]                         = "",
        [AMBA_INT_SPI_ID42]                         = "",
        [AMBA_INT_SPI_ID43]                         = "",
        [AMBA_INT_SPI_ID44_CORTEX0_INTERRIRQ]       = "Cortex0 nINTERRIRQ",
        [AMBA_INT_SPI_ID45_ADC]                     = "ADC",
        [AMBA_INT_SPI_ID46]                         = "",
        [AMBA_INT_SPI_ID47_I2C_MASTER0]             = "I2C_MASTER0",
        [AMBA_INT_SPI_ID48_I2C_MASTER1]             = "I2C_MASTER1",
        [AMBA_INT_SPI_ID49_I2C_MASTER2]             = "I2C_MASTER2",
        [AMBA_INT_SPI_ID50_I2C_MASTER3]             = "I2C_MASTER3",
        [AMBA_INT_SPI_ID51_I2C_SLAVE]               = "I2C_SLAVE",
        [AMBA_INT_SPI_ID52_IR_INTERFACE]            = "IR_INTERFACE",
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
        [AMBA_INT_SPI_ID74_WATCHDOG_TIMER]          = "WATCHDOG_TIMER",
        [AMBA_INT_SPI_ID75_GPIO0]                   = "GPIO0",
        [AMBA_INT_SPI_ID76_GPIO1]                   = "GPIO1",
        [AMBA_INT_SPI_ID77_GPIO2]                   = "GPIO2",
        [AMBA_INT_SPI_ID78]                         = "",
        [AMBA_INT_SPI_ID79]                         = "",
        [AMBA_INT_SPI_ID80]                         = "",
        [AMBA_INT_SPI_ID81]                         = "",
        [AMBA_INT_SPI_ID82_SDIO1_CARD_DETECT]       = "SD2_CARD_DETECT",
        [AMBA_INT_SPI_ID83_USB_ID_CHANGE]           = "USB_DIGITAL_ID_CHANGE",
        [AMBA_INT_SPI_ID84_USB_CONNECT]             = "USB_CONNECT",
        [AMBA_INT_SPI_ID85_USB_CONNECT_CHANGE]      = "USB_CONNECT_CHANGE",
        [AMBA_INT_SPI_ID86_USB_CHARGE_DETECT]       = "USB_CHARGE_DETECT",
        [AMBA_INT_SPI_ID87_SDIO0_CARD_DETECT]       = "SD1_CARD_DETECT",
        [AMBA_INT_SPI_ID88_SD_CARD_DETECT]          = "SD0_CARD_DETECT",
        [AMBA_INT_SPI_ID89_ENET_PMT]                = "ETHERNET_PMT",
        [AMBA_INT_SPI_ID90_ENET_SBD]                = "ETHERNET_SUBSYSTEM",
        [AMBA_INT_SPI_ID91_UART_AHB0]               = "UART_AHB0",
        [AMBA_INT_SPI_ID92_UART_AHB1]               = "UART_AHB1",
        [AMBA_INT_SPI_ID93_UART_AHB2]               = "UART_AHB2",
        [AMBA_INT_SPI_ID94_UART_AHB3]               = "UART_AHB3",
        [AMBA_INT_SPI_ID95]                         = "",
        [AMBA_INT_SPI_ID96_SDIO1]                   = "SD2",
        [AMBA_INT_SPI_ID97_USB_HOST_EHCI]           = "USB_EHCI",
        [AMBA_INT_SPI_ID98_USB_HOST_OHCI]           = "USB_OHCI",
        [AMBA_INT_SPI_ID99_USB]                     = "USB",
        [AMBA_INT_SPI_ID100_FIO]                    = "FIO",
        [AMBA_INT_SPI_ID101_FIO_ECC_RPT]            = "FIO_ECC_RPT",
        [AMBA_INT_SPI_ID102_GDMA]                   = "GDMA",
        [AMBA_INT_SPI_ID103_SDIO0]                  = "SD1",
        [AMBA_INT_SPI_ID104_SD]                     = "SD0",
        [AMBA_INT_SPI_ID105_SPI_NOR]                = "SPI_NOR",
        [AMBA_INT_SPI_ID106_SSI_MASTER0]            = "SSI_MASTER0",
        [AMBA_INT_SPI_ID107_SSI_MASTER1]            = "SSI_MASTER1",
        [AMBA_INT_SPI_ID108_SSI_MASTER2]            = "SSI_MASTER2",
        [AMBA_INT_SPI_ID109_SSI_MASTER3]            = "SSI_MASTER3",
        [AMBA_INT_SPI_ID110]                        = "",
        [AMBA_INT_SPI_ID111]                        = "",
        [AMBA_INT_SPI_ID112_SSI_SLAVE]              = "SSI_SLAVE",
        [AMBA_INT_SPI_ID113_I2S_TX]                 = "I2S0_TX",
        [AMBA_INT_SPI_ID114_I2S_RX]                 = "I2S0_RX",
        [AMBA_INT_SPI_ID115_DMA0]                   = "DMA0",
        [AMBA_INT_SPI_ID116_DMA1]                   = "DMA1",
        [AMBA_INT_SPI_ID117]                        = "",
        [AMBA_INT_SPI_ID118_CANC]                   = "CANC",
        [AMBA_INT_SPI_ID119_I2S1_TX]                = "I2S1_TX",
        [AMBA_INT_SPI_ID120_I2S1_RX]                = "I2S1_RX",
        [AMBA_INT_SPI_ID121_VOUT_A_INT]             = "VOUT_A",
        [AMBA_INT_SPI_ID122_RANDOM_NUM_GENERATOR]   = "RANDOM_NUM_GENERATOR",
        [AMBA_INT_SPI_ID123_AXI_SOFT_IRQ0]          = "SOFT_IRQ0",
        [AMBA_INT_SPI_ID124_AXI_SOFT_IRQ1]          = "SOFT_IRQ1",
        [AMBA_INT_SPI_ID125_AXI_SOFT_IRQ2]          = "SOFT_IRQ2",
        [AMBA_INT_SPI_ID126_AXI_SOFT_IRQ3]          = "SOFT_IRQ3",
        [AMBA_INT_SPI_ID127_AXI_SOFT_IRQ4]          = "SOFT_IRQ4",
        [AMBA_INT_SPI_ID128_AXI_SOFT_IRQ5]          = "SOFT_IRQ5",
        [AMBA_INT_SPI_ID129_AXI_SOFT_IRQ6]          = "SOFT_IRQ6",
        [AMBA_INT_SPI_ID130_AXI_SOFT_IRQ7]          = "SOFT_IRQ7",
        [AMBA_INT_SPI_ID131_AXI_SOFT_IRQ8]          = "SOFT_IRQ8",
        [AMBA_INT_SPI_ID132_AXI_SOFT_IRQ9]          = "SOFT_IRQ9",
        [AMBA_INT_SPI_ID133_AXI_SOFT_IRQ10]         = "SOFT_IRQ10",
        [AMBA_INT_SPI_ID134_AXI_SOFT_IRQ11]         = "SOFT_IRQ11",
        [AMBA_INT_SPI_ID135_AXI_SOFT_IRQ12]         = "SOFT_IRQ12",
        [AMBA_INT_SPI_ID136_AXI_SOFT_IRQ13]         = "SOFT_IRQ13",
        [AMBA_INT_SPI_ID137_DRAM_ERROR]             = "DRAM_ERROR",
        [AMBA_INT_SPI_ID138_VP0_EXCEPTION]          = "VP0_EXCEPTION",
        [AMBA_INT_SPI_ID139]                        = "",
        [AMBA_INT_SPI_ID140_ROLLING_SHUTTER]        = "ROLLING_SHUTTER",
        [AMBA_INT_SPI_ID141_VIN_MASTER_VSYNC]       = "VIN0_MASTER_VSYNC",
        [AMBA_INT_SPI_ID142_VIN_SLAVE_VSYNC]        = "VIN0_SLAVE_VSYNC",
        [AMBA_INT_SPI_ID143_VIN_SOF]                = "VIN0_SOF",
        [AMBA_INT_SPI_ID144_VIN_LAST_PIXEL]         = "VIN0_LAST_PIXEL",
        [AMBA_INT_SPI_ID145_VIN_DELAYED_VSYNC]      = "VIN0_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID146_PIP2_SLAVE_VSYNC]       = "VIN2_SLAVE_VSYNC",
        [AMBA_INT_SPI_ID147_PIP2_SOF]               = "VIN2_SOF",
        [AMBA_INT_SPI_ID148_PIP2_LAST_PIXEL]        = "VIN2_LAST_PIXEL",
        [AMBA_INT_SPI_ID149_PIP2_DELAYED_VSYNC]     = "VIN2_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID150]                        = "",
        [AMBA_INT_SPI_ID151]                        = "",
        [AMBA_INT_SPI_ID152]                        = "",
        [AMBA_INT_SPI_ID153]                        = "",
        [AMBA_INT_SPI_ID154]                        = "",
        [AMBA_INT_SPI_ID155]                        = "",
        [AMBA_INT_SPI_ID156]                        = "",
        [AMBA_INT_SPI_ID157]                        = "",
        [AMBA_INT_SPI_ID158]                        = "",
        [AMBA_INT_SPI_ID159]                        = "",
        [AMBA_INT_SPI_ID160]                        = "",
        [AMBA_INT_SPI_ID161]                        = "",
        [AMBA_INT_SPI_ID162]                        = "",
        [AMBA_INT_SPI_ID163]                        = "",
        [AMBA_INT_SPI_ID164]                        = "",
        [AMBA_INT_SPI_ID165]                        = "",
        [AMBA_INT_SPI_ID166_PIP_MASTER_VSYNC]       = "VIN1_MASTER_VSYNC",
        [AMBA_INT_SPI_ID167_PIP_SLAVE_VSYNC]        = "VIN1_SLAVE_VSYNC",
        [AMBA_INT_SPI_ID168_PIP_SOF]                = "VIN1_SOF",
        [AMBA_INT_SPI_ID169_PIP_LAST_PIXEL]         = "VIN1_LAST_PIXEL",
        [AMBA_INT_SPI_ID170_PIP_DELAYED_VSYNC]      = "VIN1_DELAYED_VSYNC",
        [AMBA_INT_SPI_ID171_SMEM_ERROR]             = "SMEM_ERROR",
        [AMBA_INT_SPI_ID172_CODE_VIN_IRQ]           = "CODE_VIN0",
        [AMBA_INT_SPI_ID173_CODE_VDSP0_IRQ]         = "CODE_VDSP0",
        [AMBA_INT_SPI_ID174_CODE_VDSP1_IRQ]         = "CODE_VDSP1",
        [AMBA_INT_SPI_ID175_CODE_VDSP2_IRQ]         = "CODE_VDSP2",
        [AMBA_INT_SPI_ID176_CODE_VDSP3_IRQ]         = "CODE_VDSP3",
        [AMBA_INT_SPI_ID177_CODE_VOUT0_IRQ]         = "CODE_VOUT0",
        [AMBA_INT_SPI_ID178_CODE_VOUT1_IRQ]         = "CODE_VOUT1",
        [AMBA_INT_SPI_ID179_CODE_PIP_IRQ]           = "CODE_VIN1",
        [AMBA_INT_SPI_ID180_CODE_PIP2_IRQ]          = "CODE_VIN2",
        [AMBA_INT_SPI_ID181]                        = "",
        [AMBA_INT_SPI_ID182]                        = "",
        [AMBA_INT_SPI_ID183]                        = "",
        [AMBA_INT_SPI_ID184_VORC_L2C_EVENT_IRQ]     = "I2C_ERR",
        [AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ]       = "VORC_TH0",
        [AMBA_INT_SPI_ID186_VORC_THREAD1_IRQ]       = "VORC_TH1",
        [AMBA_INT_SPI_ID187_VORC_THREAD2_IRQ]       = "VORC_TH2",
        [AMBA_INT_SPI_ID188]                        = "",
        [AMBA_INT_SPI_ID189]                        = "",
        [AMBA_INT_SPI_ID190]                        = "",
        [AMBA_INT_SPI_ID191]                        = "",
        [AMBA_INT_SPI_ID192]                        = "",
    };

    /* ID */
    LogFunc("[");
    Diag_PrintFormattedInt(LogFunc, "%d", IntID, 3U);
    LogFunc("]:\t");

    /* Name, %-30s */
    LogFunc(_AmbaIntNameStr[IntID]);
    StringLength = IO_UtilityStringLength(_AmbaIntNameStr[IntID]);
    if (StringLength < 30U) {
        for (i = 0; i < (30U - StringLength); i++) {
            LogFunc(" ");
        }
    }

    /* Type */
    LogFunc("     ");
    LogFunc(_AmbaIntTypeStr[IntInfo->IrqType]);

    /* SenseType */
    LogFunc("   ");
    LogFunc(_AmbaIntSenseStr[IntInfo->TriggerType]);

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

