/**
 *  @file AmbaRTSL_GIC.c
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
 *  @details CoreLink GIC-400 RTSL APIs
 *
 */

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif
#include "AmbaMisraFix.h"

#include "AmbaTypes.h"
#include "AmbaIOUtility.h"

#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_UART.h"

#include "AmbaCSL_GIC.h"

#ifdef CONFIG_XEN_SUPPORT
#include <AmbaXen.h>
#endif

AMBA_GIC_DISTRIBUTOR_REG_s * pAmbaGIC_DistReg;
AMBA_GIC_CPU_IF_REG_s      * pAmbaGIC_CpuIfReg;

static AMBA_INT_ISR_f AmbaIsrTable[AMBA_NUM_INTERRUPT] GNU_SECTION_NOZEROINIT;     /* The table of ISRs */
static UINT32 AmbaIsrFuncArg[AMBA_NUM_INTERRUPT] GNU_SECTION_NOZEROINIT;           /* The table of ISRs */

static void (*AmbaIsrEntryPoint)(UINT32 IntID) = NULL;
static void (*AmbaIsrExitPoint)(UINT32 IntID) = NULL;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
static UINT32 Gic_GetDupIntID(UINT32 IntID)
{
    static const UINT32 DupIntLookupTable[AMBA_NUM_INTERRUPT] = {
        [AMBA_INT_SGI_ID00]                             = 0U,
        [AMBA_INT_SGI_ID01]                             = 0U,
        [AMBA_INT_SGI_ID02]                             = 0U,
        [AMBA_INT_SGI_ID03]                             = 0U,
        [AMBA_INT_SGI_ID04]                             = 0U,
        [AMBA_INT_SGI_ID05]                             = 0U,
        [AMBA_INT_SGI_ID06]                             = 0U,
        [AMBA_INT_SGI_ID07]                             = 0U,
        [AMBA_INT_SGI_ID08]                             = 0U,
        [AMBA_INT_SGI_ID09]                             = 0U,
        [AMBA_INT_SGI_ID10]                             = 0U,
        [AMBA_INT_SGI_ID11]                             = 0U,
        [AMBA_INT_SGI_ID12]                             = 0U,
        [AMBA_INT_SGI_ID13]                             = 0U,
        [AMBA_INT_SGI_ID14]                             = 0U,
        [AMBA_INT_SGI_ID15]                             = 0U,
        [AMBA_INT_PPI_ID016]                            = 0U,
        [AMBA_INT_PPI_ID017]                            = 0U,
        [AMBA_INT_PPI_ID018]                            = 0U,
        [AMBA_INT_PPI_ID019]                            = 0U,
        [AMBA_INT_PPI_ID020]                            = 0U,
        [AMBA_INT_PPI_ID021]                            = 0U,
        [AMBA_INT_PPI_ID022]                            = 0U,
        [AMBA_INT_PPI_ID023]                            = 0U,
        [AMBA_INT_PPI_ID024]                            = 0U,
        [AMBA_INT_PPI_ID025_VIRTUAL_MAINTENANCE]        = 0U,
        [AMBA_INT_PPI_ID026_HYPERVISOR_TIMER]           = 0U,
        [AMBA_INT_PPI_ID027_VIRTUAL_TIMER]              = 0U,
        [AMBA_INT_PPI_ID028_FIQ]                        = 0U,
        [AMBA_INT_PPI_ID029_SECURE_PHYSICAL_TIMER]      = 0U,
        [AMBA_INT_PPI_ID030_NONSECURE_PHYSICAL_TIMER]   = 0U,
        [AMBA_INT_PPI_ID031_IRQ]                        = 0U,
        [AMBA_INT_SPI_ID032_CEHU0]                      = 0U,
        [AMBA_INT_SPI_ID033_CEHU1]                      = 0U,
        [AMBA_INT_SPI_ID034_CA53_EXTERRIRQ]             = 0U,
        [AMBA_INT_SPI_ID035_CA53_PMUIRQ0]               = 0U,
        [AMBA_INT_SPI_ID036_CA53_PMUIRQ1]               = 0U,
        [AMBA_INT_SPI_ID037_CA53_PMUIRQ2]               = 0U,
        [AMBA_INT_SPI_ID038_CA53_PMUIRQ3]               = 0U,
        [AMBA_INT_SPI_ID039_CAN0]                       = AMBA_INT_SPI_ID227_CAN0,
        [AMBA_INT_SPI_ID040_CAN1]                       = AMBA_INT_SPI_ID228_CAN1,
        [AMBA_INT_SPI_ID041_CAN2]                       = AMBA_INT_SPI_ID229_CAN2,
        [AMBA_INT_SPI_ID042_CAN3]                       = AMBA_INT_SPI_ID230_CAN3,
        [AMBA_INT_SPI_ID043_CAN4]                       = AMBA_INT_SPI_ID231_CAN4,
        [AMBA_INT_SPI_ID044_CAN5]                       = AMBA_INT_SPI_ID232_CAN5,
        [AMBA_INT_SPI_ID045_CA53_INTERRIRQ]             = 0U,
        [AMBA_INT_SPI_ID046_I2C_MASTER0]                = AMBA_INT_SPI_ID247_I2C_MASTER0,
        [AMBA_INT_SPI_ID047_I2C_MASTER1]                = AMBA_INT_SPI_ID248_I2C_MASTER1,
        [AMBA_INT_SPI_ID048_I2C_MASTER2]                = AMBA_INT_SPI_ID249_I2C_MASTER2,
        [AMBA_INT_SPI_ID049_I2C_MASTER3]                = AMBA_INT_SPI_ID250_I2C_MASTER3,
        [AMBA_INT_SPI_ID050_I2C_MASTER4]                = AMBA_INT_SPI_ID251_I2C_MASTER4,
        [AMBA_INT_SPI_ID051_I2C_MASTER5]                = AMBA_INT_SPI_ID252_I2C_MASTER5,
        [AMBA_INT_SPI_ID052_I2C_SLAVE]                  = AMBA_INT_SPI_ID246_I2C_SLAVE,
        [AMBA_INT_SPI_ID053_UART_APB]                   = 0U,
        [AMBA_INT_SPI_ID054_TIMER0]                     = 0U,
        [AMBA_INT_SPI_ID055_TIMER1]                     = 0U,
        [AMBA_INT_SPI_ID056_TIMER2]                     = 0U,
        [AMBA_INT_SPI_ID057_TIMER3]                     = 0U,
        [AMBA_INT_SPI_ID058_TIMER4]                     = 0U,
        [AMBA_INT_SPI_ID059_TIMER5]                     = 0U,
        [AMBA_INT_SPI_ID060_TIMER6]                     = 0U,
        [AMBA_INT_SPI_ID061_TIMER7]                     = 0U,
        [AMBA_INT_SPI_ID062_TIMER8]                     = 0U,
        [AMBA_INT_SPI_ID063_TIMER9]                     = 0U,
        [AMBA_INT_SPI_ID064_TIMER10]                    = 0U,
        [AMBA_INT_SPI_ID065_TIMER11]                    = 0U,
        [AMBA_INT_SPI_ID066_TIMER12]                    = 0U,
        [AMBA_INT_SPI_ID067_TIMER13]                    = 0U,
        [AMBA_INT_SPI_ID068_TIMER14]                    = 0U,
        [AMBA_INT_SPI_ID069_TIMER15]                    = 0U,
        [AMBA_INT_SPI_ID070_TIMER16]                    = 0U,
        [AMBA_INT_SPI_ID071_TIMER17]                    = 0U,
        [AMBA_INT_SPI_ID072_TIMER18]                    = 0U,
        [AMBA_INT_SPI_ID073_TIMER19]                    = 0U,
        [AMBA_INT_SPI_ID074_TIMER20]                    = 0U,
        [AMBA_INT_SPI_ID075_TIMER21]                    = 0U,
        [AMBA_INT_SPI_ID076_TIMER22]                    = 0U,
        [AMBA_INT_SPI_ID077_TIMER23]                    = 0U,
        [AMBA_INT_SPI_ID078_TIMER24]                    = 0U,
        [AMBA_INT_SPI_ID079_TIMER25]                    = 0U,
        [AMBA_INT_SPI_ID080_TIMER26]                    = 0U,
        [AMBA_INT_SPI_ID081_TIMER27]                    = 0U,
        [AMBA_INT_SPI_ID082_TIMER28]                    = 0U,
        [AMBA_INT_SPI_ID083_TIMER29]                    = 0U,
        [AMBA_INT_SPI_ID084_WDT]                        = AMBA_INT_SPI_ID240_WDT,
        [AMBA_INT_SPI_ID085_GPIO_GROUP0]                = AMBA_INT_SPI_ID223_GPIO_GROUP0,
        [AMBA_INT_SPI_ID086_GPIO_GROUP1]                = AMBA_INT_SPI_ID224_GPIO_GROUP1,
        [AMBA_INT_SPI_ID087_GPIO_GROUP2]                = AMBA_INT_SPI_ID225_GPIO_GROUP2,
        [AMBA_INT_SPI_ID088_GPIO_GROUP3]                = AMBA_INT_SPI_ID226_GPIO_GROUP3,
        [AMBA_INT_SPI_ID089_USB_ID_CHANGE]              = 0U,
        [AMBA_INT_SPI_ID090_USB_CONNECT]                = 0U,
        [AMBA_INT_SPI_ID091_USB_CONNECT_CHANGE]         = 0U,
        [AMBA_INT_SPI_ID092_USB_CHARGE_DETECT]          = 0U,
        [AMBA_INT_SPI_ID093_SDIO0_CARD_DETECT]          = 0U,
        [AMBA_INT_SPI_ID094_SD_CARD_DETECT]             = 0U,
        [AMBA_INT_SPI_ID095_ENET0_PMT]                  = 0U,
        [AMBA_INT_SPI_ID096_ENET0_SBD]                  = 0U,
        [AMBA_INT_SPI_ID097_UART_AHB0]                  = 0U,
        [AMBA_INT_SPI_ID098_UART_AHB1]                  = 0U,
        [AMBA_INT_SPI_ID099_UART_AHB2]                  = 0U,
        [AMBA_INT_SPI_ID100_UART_AHB3]                  = 0U,
        [AMBA_INT_SPI_ID101_USB_HOST_EHCI]              = 0U,
        [AMBA_INT_SPI_ID102_USB_HOST_OHCI]              = 0U,
        [AMBA_INT_SPI_ID103_USB]                        = 0U,
        [AMBA_INT_SPI_ID104_FIO]                        = 0U,
        [AMBA_INT_SPI_ID105_FIO_ECC_RPT]                = 0U,
        [AMBA_INT_SPI_ID106_GDMA]                       = 0U,
        [AMBA_INT_SPI_ID107_SDIO0]                      = 0U,
        [AMBA_INT_SPI_ID108_SD]                         = 0U,
        [AMBA_INT_SPI_ID109_SPI_NOR]                    = 0U,
        [AMBA_INT_SPI_ID110_SSI_MASTER0]                = AMBA_INT_SPI_ID234_SSI_MASTER0,
        [AMBA_INT_SPI_ID111_SSI_MASTER1]                = AMBA_INT_SPI_ID235_SSI_MASTER1,
        [AMBA_INT_SPI_ID112_SSI_MASTER2]                = AMBA_INT_SPI_ID236_SSI_MASTER2,
        [AMBA_INT_SPI_ID113_SSI_MASTER3]                = AMBA_INT_SPI_ID237_SSI_MASTER3,
        [AMBA_INT_SPI_ID114_SSI_SLAVE]                  = AMBA_INT_SPI_ID233_SSI_SLAVE,
        [AMBA_INT_SPI_ID115_I2S_TX]                     = 0U,
        [AMBA_INT_SPI_ID116_I2S_RX]                     = 0U,
        [AMBA_INT_SPI_ID117_DMA_ENGINE0]                = AMBA_INT_SPI_ID253_DMA_ENGINE0,
        [AMBA_INT_SPI_ID118_DMA_ENGINE1]                = AMBA_INT_SPI_ID254_DMA_ENGINE1,
        [AMBA_INT_SPI_ID119_VOUT_B_INT]                 = 0U,
        [AMBA_INT_SPI_ID120_VOUT_A_INT]                 = 0U,
        [AMBA_INT_SPI_ID121_HRNG]                       = 0U,
        [AMBA_INT_SPI_ID122_AXI_SOFT_IRQ0]              = 0U,
        [AMBA_INT_SPI_ID123_AXI_SOFT_IRQ1]              = 0U,
        [AMBA_INT_SPI_ID124_AXI_SOFT_IRQ2]              = 0U,
        [AMBA_INT_SPI_ID125_AXI_SOFT_IRQ3]              = 0U,
        [AMBA_INT_SPI_ID126_AXI_SOFT_IRQ4]              = 0U,
        [AMBA_INT_SPI_ID127_AXI_SOFT_IRQ5]              = 0U,
        [AMBA_INT_SPI_ID128_AXI_SOFT_IRQ6]              = 0U,
        [AMBA_INT_SPI_ID129_AXI_SOFT_IRQ7]              = 0U,
        [AMBA_INT_SPI_ID130_AXI_SOFT_IRQ8]              = 0U,
        [AMBA_INT_SPI_ID131_AXI_SOFT_IRQ9]              = 0U,
        [AMBA_INT_SPI_ID132_AXI_SOFT_IRQ10]             = 0U,
        [AMBA_INT_SPI_ID133_AXI_SOFT_IRQ11]             = 0U,
        [AMBA_INT_SPI_ID134_AXI_SOFT_IRQ12]             = 0U,
        [AMBA_INT_SPI_ID135_AXI_SOFT_IRQ13]             = 0U,
        [AMBA_INT_SPI_ID136_DRAM_ERROR]                 = 0U,
        [AMBA_INT_SPI_ID137_VP0_EXCEPTION]              = 0U,
        [AMBA_INT_SPI_ID138_VIN0_SLAVE_VSYNC]           = 0U,
        [AMBA_INT_SPI_ID139_VIN0_SOF]                   = 0U,
        [AMBA_INT_SPI_ID140_VIN0_MASTER_VSYNC]          = 0U,
        [AMBA_INT_SPI_ID141_VIN0_LAST_PIXEL]            = 0U,
        [AMBA_INT_SPI_ID142_VIN0_DELAYED_VSYNC]         = 0U,
        [AMBA_INT_SPI_ID143_VIN1_SLAVE_VSYNC]           = 0U,
        [AMBA_INT_SPI_ID144_VIN1_SOF]                   = 0U,
        [AMBA_INT_SPI_ID145_VIN1_MASTER_VSYNC]          = 0U,
        [AMBA_INT_SPI_ID146_VIN1_LAST_PIXEL]            = 0U,
        [AMBA_INT_SPI_ID147_VIN1_DELAYED_VSYNC]         = 0U,
        [AMBA_INT_SPI_ID148_VIN2_SLAVE_VSYNC]           = 0U,
        [AMBA_INT_SPI_ID149_VIN2_SOF]                   = 0U,
        [AMBA_INT_SPI_ID150_VIN2_LAST_PIXEL]            = 0U,
        [AMBA_INT_SPI_ID151_VIN2_DELAYED_VSYNC]         = 0U,
        [AMBA_INT_SPI_ID152_VIN3_SLAVE_VSYNC]           = 0U,
        [AMBA_INT_SPI_ID153_VIN3_SOF]                   = 0U,
        [AMBA_INT_SPI_ID154_APB_DBG_LOCK_ACCESS]        = 0U,
        [AMBA_INT_SPI_ID155_VIN3_LAST_PIXEL]            = 0U,
        [AMBA_INT_SPI_ID156_VIN3_DELAYED_VSYNC]         = 0U,
        [AMBA_INT_SPI_ID157_VIN4_SLAVE_VSYNC]           = 0U,
        [AMBA_INT_SPI_ID158_VIN4_SOF]                   = 0U,
        [AMBA_INT_SPI_ID159_AXI_CFG_LOCK_ACCESS]        = 0U,
        [AMBA_INT_SPI_ID160_VIN4_LAST_PIXEL]            = 0U,
        [AMBA_INT_SPI_ID161_VIN4_DELAYED_VSYNC]         = 0U,
        [AMBA_INT_SPI_ID162_SAHB_CFG_LOCK_ACCESS]       = 0U,
        [AMBA_INT_SPI_ID163_NSAHB_CFG_LOCK_ACCESS]      = 0U,
        [AMBA_INT_SPI_ID164_ENET1_PMT]                  = 0U,
        [AMBA_INT_SPI_ID165_ENET1_SBD]                  = 0U,
        [AMBA_INT_SPI_ID166_SSI_MASTER4]                = AMBA_INT_SPI_ID238_SSI_MASTER4,
        [AMBA_INT_SPI_ID167_SSI_MASTER5]                = AMBA_INT_SPI_ID239_SSI_MASTER5,
        [AMBA_INT_SPI_ID168_WDT_5]                      = AMBA_INT_SPI_ID241_WDT_CR52,
        [AMBA_INT_SPI_ID169_WDT_1]                      = AMBA_INT_SPI_ID242_WDT_CA53_0,
        [AMBA_INT_SPI_ID170_WDT_2]                      = AMBA_INT_SPI_ID243_WDT_CA53_1,
        [AMBA_INT_SPI_ID171_WDT_3]                      = AMBA_INT_SPI_ID244_WDT_CA53_2,
        [AMBA_INT_SPI_ID172_WDT_4]                      = AMBA_INT_SPI_ID245_WDT_CA53_3,
        [AMBA_INT_SPI_ID173_VIN8_SLAVE_VSYNC]           = 0U,
        [AMBA_INT_SPI_ID174_VIN8_SOF]                   = 0U,
        [AMBA_INT_SPI_ID175_VIN8_LAST_PIXEL]            = 0U,
        [AMBA_INT_SPI_ID176_VIN8_DELAYED_VSYNC]         = 0U,
        [AMBA_INT_SPI_ID177_CODE_VDSP0_IRQ]             = AMBA_INT_SPI_ID202_CODE_VDSP0_IRQ,
        [AMBA_INT_SPI_ID178_CODE_VDSP1_IRQ]             = AMBA_INT_SPI_ID203_CODE_VDSP1_IRQ,
        [AMBA_INT_SPI_ID179_CODE_VDSP2_IRQ]             = AMBA_INT_SPI_ID204_CODE_VDSP2_IRQ,
        [AMBA_INT_SPI_ID180_CODE_VDSP3_IRQ]             = AMBA_INT_SPI_ID205_CODE_VDSP3_IRQ,
        [AMBA_INT_SPI_ID181_CODE_VIN0_IRQ]              = AMBA_INT_SPI_ID206_CODE_VIN0_IRQ,
        [AMBA_INT_SPI_ID182_CODE_VIN1_IRQ]              = AMBA_INT_SPI_ID207_CODE_VIN1_IRQ,
        [AMBA_INT_SPI_ID183_CODE_VIN2_IRQ]              = AMBA_INT_SPI_ID208_CODE_VIN2_IRQ,
        [AMBA_INT_SPI_ID184_CODE_VIN3_IRQ]              = AMBA_INT_SPI_ID209_CODE_VIN3_IRQ,
        [AMBA_INT_SPI_ID185_CODE_VIN4_IRQ]              = AMBA_INT_SPI_ID210_CODE_VIN4_IRQ,
        [AMBA_INT_SPI_ID186]                            = 0U,
        [AMBA_INT_SPI_ID187]                            = 0U,
        [AMBA_INT_SPI_ID188]                            = 0U,
        [AMBA_INT_SPI_ID189_CODE_VIN8_IRQ ]             = AMBA_INT_SPI_ID214_CODE_VIN8_IRQ,
        [AMBA_INT_SPI_ID190_CODE_VOUT0_IRQ]             = AMBA_INT_SPI_ID215_CODE_VOUT0_IRQ,
        [AMBA_INT_SPI_ID191_CODE_VOUT1_IRQ]             = AMBA_INT_SPI_ID216_CODE_VOUT1_IRQ,
        [AMBA_INT_SPI_ID192_VORC_L2C_EVENT_IRQ]         = 0U,
        [AMBA_INT_SPI_ID193_VORC_THREAD0_IRQ]           = 0U,
        [AMBA_INT_SPI_ID194_VORC_THREAD1_IRQ]           = 0U,
        [AMBA_INT_SPI_ID195_VORC_THREAD2_IRQ]           = 0U,
        [AMBA_INT_SPI_ID196_VORC_THREAD3_IRQ]           = 0U,
        [AMBA_INT_SPI_ID197_VORC_THREAD4_IRQ]           = 0U,
        [AMBA_INT_SPI_ID198_DDRC0]                      = 0U,
        [AMBA_INT_SPI_ID199_DDRC1]                      = 0U,
        [AMBA_INT_SPI_ID200_IDSP_SAFETY]                = 0U,
        [AMBA_INT_SPI_ID201]                            = 0U,
        [AMBA_INT_SPI_ID202_CODE_VDSP0_IRQ]             = 0U,
        [AMBA_INT_SPI_ID203_CODE_VDSP1_IRQ]             = 0U,
        [AMBA_INT_SPI_ID204_CODE_VDSP2_IRQ]             = 0U,
        [AMBA_INT_SPI_ID205_CODE_VDSP3_IRQ]             = 0U,
        [AMBA_INT_SPI_ID206_CODE_VIN0_IRQ]              = 0U,
        [AMBA_INT_SPI_ID207_CODE_VIN1_IRQ]              = 0U,
        [AMBA_INT_SPI_ID208_CODE_VIN2_IRQ]              = 0U,
        [AMBA_INT_SPI_ID209_CODE_VIN3_IRQ]              = 0U,
        [AMBA_INT_SPI_ID210_CODE_VIN4_IRQ]              = 0U,
        [AMBA_INT_SPI_ID211]                            = 0U,
        [AMBA_INT_SPI_ID212]                            = 0U,
        [AMBA_INT_SPI_ID213]                            = 0U,
        [AMBA_INT_SPI_ID214_CODE_VIN8_IRQ]              = 0U,
        [AMBA_INT_SPI_ID215_CODE_VOUT0_IRQ]             = 0U,
        [AMBA_INT_SPI_ID216_CODE_VOUT1_IRQ]             = 0U,
        [AMBA_INT_SPI_ID217_VORC_L2C_EVENT_IRQ]         = 0U,
        [AMBA_INT_SPI_ID218_VORC_THREAD0_IRQ]           = 0U,
        [AMBA_INT_SPI_ID219_VORC_THREAD1_IRQ]           = 0U,
        [AMBA_INT_SPI_ID220_VORC_THREAD2_IRQ]           = 0U,
        [AMBA_INT_SPI_ID221_VORC_THREAD3_IRQ]           = 0U,
        [AMBA_INT_SPI_ID222_VORC_THREAD4_IRQ]           = 0U,
        [AMBA_INT_SPI_ID223_GPIO_GROUP0]                = 0U,
        [AMBA_INT_SPI_ID224_GPIO_GROUP1]                = 0U,
        [AMBA_INT_SPI_ID225_GPIO_GROUP2]                = 0U,
        [AMBA_INT_SPI_ID226_GPIO_GROUP3]                = 0U,
        [AMBA_INT_SPI_ID227_CAN0]                       = 0U,
        [AMBA_INT_SPI_ID228_CAN1]                       = 0U,
        [AMBA_INT_SPI_ID229_CAN2]                       = 0U,
        [AMBA_INT_SPI_ID230_CAN3]                       = 0U,
        [AMBA_INT_SPI_ID231_CAN4]                       = 0U,
        [AMBA_INT_SPI_ID232_CAN5]                       = 0U,
        [AMBA_INT_SPI_ID233_SSI_SLAVE]                  = 0U,
        [AMBA_INT_SPI_ID234_SSI_MASTER0]                = 0U,
        [AMBA_INT_SPI_ID235_SSI_MASTER1]                = 0U,
        [AMBA_INT_SPI_ID236_SSI_MASTER2]                = 0U,
        [AMBA_INT_SPI_ID237_SSI_MASTER3]                = 0U,
        [AMBA_INT_SPI_ID238_SSI_MASTER4]                = 0U,
        [AMBA_INT_SPI_ID239_SSI_MASTER5]                = 0U,
        [AMBA_INT_SPI_ID240_WDT]                        = 0U,
        [AMBA_INT_SPI_ID241_WDT_CR52]                   = 0U,
        [AMBA_INT_SPI_ID242_WDT_CA53_0]                 = 0U,
        [AMBA_INT_SPI_ID243_WDT_CA53_1]                 = 0U,
        [AMBA_INT_SPI_ID244_WDT_CA53_2]                 = 0U,
        [AMBA_INT_SPI_ID245_WDT_CA53_3]                 = 0U,
        [AMBA_INT_SPI_ID246_I2C_SLAVE]                  = 0U,
        [AMBA_INT_SPI_ID247_I2C_MASTER0]                = 0U,
        [AMBA_INT_SPI_ID248_I2C_MASTER1]                = 0U,
        [AMBA_INT_SPI_ID249_I2C_MASTER2]                = 0U,
        [AMBA_INT_SPI_ID250_I2C_MASTER3]                = 0U,
        [AMBA_INT_SPI_ID251_I2C_MASTER4]                = 0U,
        [AMBA_INT_SPI_ID252_I2C_MASTER5]                = 0U,
        [AMBA_INT_SPI_ID253_DMA_ENGINE0]                = 0U,
        [AMBA_INT_SPI_ID254_DMA_ENGINE1]                = 0U,
        [AMBA_INT_SPI_ID255]                            = 0U,
    };

    return DupIntLookupTable[IntID];
}

static UINT32 Gic_DupIntConfig(UINT32 IntID, const AMBA_INT_CONFIG_s *pIntConfig)
{
    UINT32 RetVal = INT_ERR_NONE;
#if defined(CONFIG_BOOT_CORE_SRTOS) && (CONFIG_BOOT_CORE_SRTOS > 0)
    const UINT32 MinTarget = 0x1U << CONFIG_BOOT_CORE_SRTOS;
#else
    const UINT32 MinTarget = 0x1U;
#endif
    if ((IntID >= (UINT32)AMBA_NUM_INTERRUPT) || (pIntConfig == NULL)) {
        RetVal = INT_ERR_ARG;
    } else {
        AmbaCSL_GicSetIntConfig(IntID, pIntConfig->TriggerType);

        if (pIntConfig->CpuTargets >= MinTarget) {
            AmbaCSL_GicSetIntTargetCore(IntID, pIntConfig->CpuTargets);
        } else {
            /* if no core is assigned, default assign to first core */
            AmbaCSL_GicSetIntTargetCore(IntID, MinTarget);
        }
#if defined(CONFIG_ENABLE_AMBALINK)
#if defined(CONFIG_FWPROG_ATF_ENABLE)
        /* non-secure linux cannot see FIQ, use only IRQ */
        AmbaCSL_GicSetIntGroup(IntID, 1U);
#else
        /* secure linux cannot see IRQ, use only FIQ */
        AmbaCSL_GicSetIntGroup(IntID, 0U);
#endif
#else
        if (pIntConfig->IrqType == INT_TYPE_FIQ) {
            /* Group 0 interrupts are Secure interrupts. Group 0 interrupts could be either FIQs or IRQs. */
            AmbaCSL_GicSetIntGroup(IntID, 0U);  /* FIQEn bit of GICC_CTLR must be set */
        } else {
            /* Group 1 interrupts are Non-secure interrupts. Group 1 interrupts are always IRQs. */
            AmbaCSL_GicSetIntGroup(IntID, 1U);
        }
#endif
    }

    return RetVal;
}
#endif

static void Gic_PrintStr(const char *pFmt)
{
    static char ShellPrintBuf[128U] GNU_SECTION_NOZEROINIT;
    UINT32 ActualTxSize, UartTxSize, Index = 0U;
    const UINT8 *pPrintBuf;
    const char *pString, *pArg = NULL;

    pString = &ShellPrintBuf[0];
    AmbaMisra_TypeCast64(&pPrintBuf, &pString);
    UartTxSize = IO_UtilityStringPrintStr(&ShellPrintBuf[0], 128U, pFmt, 1U, &pArg);
    while (UartTxSize > 0U) {
        if (AmbaRTSL_UartWrite(AMBA_UART_APB_CHANNEL0, UartTxSize, &pPrintBuf[Index], &ActualTxSize) != UART_ERR_NONE) {
            break;
        } else {
            UartTxSize -= ActualTxSize;
            Index += ActualTxSize;
        }
    }
}

static void Gic_PrintStrInt(UINT32 value)
{
    char IntStr[4];

    (void)IO_UtilityUInt32ToStr(IntStr, 4U, value, 10U);
    Gic_PrintStr(IntStr);
    Gic_PrintStr("\r\n");
}

/**
 *  AmbaRTSL_GicSetCpuTarget - Set CPU targets
 *  @param[in] IntID Interrupt ID
 *  @param[in] CpuTargetList CPU target list (mask) to receive the SGI
 */
void AmbaRTSL_GicSetCpuTarget(UINT32 IntID, UINT32 CpuTargetList)
{
    AmbaCSL_GicSetIntTargetCore(IntID, CpuTargetList);
}

/**
 *  GIC_DistInit - GIC Distributor initialization of non-banked registers for SPIs.
 */
static void GIC_DistInit(void)
{
    UINT32 NumInts = AmbaCSL_GicGetNumINT();
    UINT32 i, RegIndex = 0U;

    pAmbaGIC_DistReg->IntCtrl = 0U;    /* GIC Global Disable */

    /*
     * GIC Distributor Interrupt Group Registers (GICD_IGROUPRn):
     *      to configure interrupts as Group 0 or Group 1
     *
     * Default value is 0.
     *
     * Initialize to Group 0 for all SPIs.
     */
    for (i = 32U; i < NumInts; i += 32U) {
#if defined(CONFIG_ENABLE_AMBALINK) && defined(CONFIG_FWPROG_ATF_ENABLE)
        RegIndex = i / 32U;
        if(RegIndex < NUM_INT_GROUP_REG) {
            /* non-secure linux cannot see FIQ, use only IRQ */
            pAmbaGIC_DistReg->IntGroup[RegIndex] |= ~0U;
        }
#else
        RegIndex = i / 32U;
        if(RegIndex < NUM_INT_GROUP_REG) {
            pAmbaGIC_DistReg->IntGroup[RegIndex] = 0U;
        }
#endif
    }

    /*
     * GIC Distributor Interrupt Configuration Registers (GICD_ICFGRn):
     *      to specify whether each interrupt is level-sensitive or edge-triggered
     *
     * SGI: The bits are read-only and a bit-pair always reads as b10. (Default: 0xAAAAAAAA)
     * PPI: The bits are read-only. PPI0 is ID 27. (Default: 0x7DC00000)
     * SPI: The LSB bit of a bit-pair is read-only and is always b1. Start from ID32.(Default: 0x55555555)
     *
     * Initialize to Level-Triggered for all SPIs.
     */
    for (i = 32U; i < NumInts; i += 16U) {
        RegIndex = i / 16U;
        if (RegIndex < NUM_INT_CONFIG_REG) {
            pAmbaGIC_DistReg->IntConfig[RegIndex] = 0x55555555U;
        }
    }

    /*
     * GIC Distributor Interrupt Priority Registers (GICD_IPRIORITYRn):
     *      to specify the priority value for each interrupt
     *
     * Initialize to highest priority for all SPIs.
     */
    for (i = 32U; i < NumInts; i += 4U) {
        RegIndex = i / 4U;
        if(RegIndex < NUM_INT_PRIORITY_REG) {
            pAmbaGIC_DistReg->IntPriority[RegIndex] = 0U;
        }
    }

    /*
     * GIC Distributor Interrupt Processor Targets Registers (GICD_ITARGETSRn):
     *      to specify the target processor list for each interrupt
     *
     * by default, all SPIs to Core-0.
     */
    for (i = 32U; i < NumInts; i += 4U) {
        RegIndex = i / 4U;
        if(RegIndex < NUM_INT_TARGET_REG) {
#if !defined(CONFIG_ENABLE_AMBALINK) || defined(CONFIG_LINUX)
            pAmbaGIC_DistReg->IntTarget[RegIndex] = 0x01010101U;
#elif (CONFIG_BOOT_CORE_SRTOS == 0)
            pAmbaGIC_DistReg->IntTarget[RegIndex] |= 0x01010101U;
#elif  (CONFIG_BOOT_CORE_SRTOS == 1)
            pAmbaGIC_DistReg->IntTarget[RegIndex] |= 0x02020202U;
#elif  (CONFIG_BOOT_CORE_SRTOS == 2)
            pAmbaGIC_DistReg->IntTarget[RegIndex] |= 0x04040404U;
#elif  (CONFIG_BOOT_CORE_SRTOS == 3)
            pAmbaGIC_DistReg->IntTarget[RegIndex] |= 0x08080808U;
#endif
        }
    }

#if defined(CONFIG_BLD_SEQ_LINUX) || defined(CONFIG_BLD_SEQ_ATF_LINUX)
    /* linux already init GIC */
#else
    /*
     * GIC Distributor Interrupt Clear-Pending Registers (GICD_ICPENDRn):
     *      to clear pending of the interrupts
     *
     * Initialize to disable all for all SPIs.
     */
#ifdef CONFIG_XEN_SUPPORT
    if (AmbaXen_is_guest() == 0u) {
#endif
        for (i = 32U; i < NumInts; i += 32U) {
            RegIndex  = i / 32U;
            if(RegIndex < NUM_INT_CLEAN_PENDING_REG) {
                pAmbaGIC_DistReg->IntClearPending[RegIndex] = 0xffffffffU;
            }
        }
#ifdef CONFIG_XEN_SUPPORT
    }
#endif

    /*
     * GIC Distributor Interrupt Clear-Enable Registers (GICD_ICENABLERn):
     *      to clear enable of the interrupts
     *
     * Initialize to disable all for all SPIs.
     */
    for (i = 32U; i < NumInts; i += 32U) {
        RegIndex = i / 32U;
        if(RegIndex < NUM_INT_CLEAN_ENABLE_REG) {
            pAmbaGIC_DistReg->IntClearEnable[RegIndex] = 0xffffffffU;
        }
    }
#endif

#if defined(CONFIG_ENABLE_AMBALINK) && defined(CONFIG_FWPROG_ATF_ENABLE)
    /* workaround to get rid of BL32 FIQ */
    pAmbaGIC_DistReg->IntCtrl = 2U;   /*  bit[1] Enable Non-secure Group 1 interrupts */
#else
    pAmbaGIC_DistReg->IntCtrl = 3U;   /* GIC Global Enable */
#endif
}

/**
 *  GIC_DistInit_NonSPI - GIC Distributor initialization of banked registers for PPIs and SGIs.
 */
static void GIC_DistInit_NonSPI(void)
{
    UINT32 i;

#if defined(CONFIG_ENABLE_AMBALINK) && defined(CONFIG_FWPROG_ATF_ENABLE)
    pAmbaGIC_DistReg->IntGroup[0] |= ~0U;    /* All in Group 1 Interrupts */
#else
    pAmbaGIC_DistReg->IntGroup[0] = 0U;    /* All in Group 0 Interrupts */
#endif

    for (i = 0U; i < 32U; i += 4U) {
        pAmbaGIC_DistReg->IntPriority[i / 4U] = 0U;
    }

#ifdef CONFIG_XEN_SUPPORT
    if (AmbaXen_is_guest() == 0u) {
#endif
        pAmbaGIC_DistReg->IntClearPending[0] = 0xffffffffU;
#ifdef CONFIG_XEN_SUPPORT
    }
#endif

    /*
     * SGI: ID0~ID15:  enable all
     * PPI: ID16~ID31: disable all
     */
    pAmbaGIC_DistReg->IntClearEnable[0] = 0xffff0000U;
    pAmbaGIC_DistReg->IntSetEnable[0] = 0x0000ffffU;
}

/**
 *  GIC_CpuIfInit - GIC CPU interface initialization.
 */
static void GIC_CpuIfInit(void)
{
    /*
     * GIC CPU Interface Interrupt Priority Mask Register (GICC_PMR):
     *      to set the priority mask for the interface
     *
     * Initalize to allow all interrrupt priority sending to CPU.
     */
    pAmbaGIC_CpuIfReg->PriorityMask.Priority = 0xffU;

    /*
     *  GIC CPU Interface Binary Point Register (GICC_BPR) for Group 0 Interrupts:
     *           Aliased Binary Point Register (GICC_ABPR) for Group 1 Interrupts:
     *      to set the binary point position
     *
     *  Binary point value = 7 (No preemption)
     */
    pAmbaGIC_CpuIfReg->BinaryPoint.SplitValue = 7U;
    pAmbaGIC_CpuIfReg->AliasedBinaryPoint.SplitValue = 7U;

    /*
     * GIC CPU Interface Control Register (ICCICR):
     *
     * SBPR[4]:         0 - To determine any preemption:
     *                      Secure Binary Point Register for Secure interrupts
     *                      Non-secure Binary Point Register for Non-secure interrupts.
     * FIQEn[3]:        1 - Signal Secure interrupts using the FIQ signal.
     * AckCtl[2]:       0 - ARM deprecates use of GICC_CTLR.AckCtl, and strongly recommends
     *                      using a software model where GICC_CTLR.AckCtl is set to 0.
     * EnableGroup1[1]: 1 - Secure software can enable signalling of Non-secure interrupts.
     * EnableGroup0[0]: 1 - Enable signalling of Secure interrupts.
     */
    pAmbaGIC_CpuIfReg->Ctrl = 0x0b;
}

/**
 *  AmbaRTSL_GicInit - GIC Initializations for first core.
 */
void AmbaRTSL_GicInit(void)
{
    static AMBA_GIC_CPU_IF_REG_s *pAmbaGIC_VmCpuIfReg;
    UINT32 i;

#if !defined(CONFIG_XEN_SUPPORT) || defined(AMBA_FWPROG)
    ULONG base_addr;
#endif

    for (i = 0U; i < (UINT32)AMBA_NUM_INTERRUPT; i ++) {
        AmbaIsrTable[i] = NULL;
        AmbaIsrFuncArg[i] = 0U;
    }

#if !defined(CONFIG_XEN_SUPPORT) || defined(AMBA_FWPROG)
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    base_addr = AMBA_CA53_GIC_DISTRIBUTOR_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGIC_DistReg, &base_addr);
    base_addr = AMBA_CA53_GIC_CPU_IF_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGIC_CpuIfReg, &base_addr);
    base_addr = AMBA_CA53_GIC_VIRTUAL_CPU_IF_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGIC_VmCpuIfReg, &base_addr);
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    base_addr = AMBA_CORTEX_A76_GIC_DISTRIBUTOR_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGIC_DistReg, &base_addr);
    base_addr = AMBA_CORTEX_A76_GIC_CPU_IF_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGIC_CpuIfReg, &base_addr);
    base_addr = AMBA_CORTEX_A76_GIC_VIRTUAL_CPU_IF_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGIC_VmCpuIfReg, &base_addr);
#else
    base_addr = AMBA_CORTEX_A53_GIC_DISTRIBUTOR_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGIC_DistReg, &base_addr);
    base_addr = AMBA_CORTEX_A53_GIC_CPU_IF_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGIC_CpuIfReg, &base_addr);
    base_addr = AMBA_CORTEX_A53_GIC_VIRTUAL_CPU_IF_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGIC_VmCpuIfReg, &base_addr);
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    for (i = 0U ; i < 3U ; i++) {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        base_addr = IO_UtilityFDTPropertyU64Quick(0, "arm,gic-400", "reg", i * 2U);
        if ( (i == 0U) && (0x0U != base_addr) ) {
            AmbaMisra_TypeCast(&pAmbaGIC_DistReg, &base_addr);
        } else if ( (i == 1U) && (0x0U != base_addr) ) {
            AmbaMisra_TypeCast(&pAmbaGIC_CpuIfReg, &base_addr);
        } else {
            AmbaMisra_TypeCast(&pAmbaGIC_VmCpuIfReg, &base_addr);
        }
#else
        base_addr = IO_UtilityFDTPropertyU32Quick(0, "arm,gic-400", "reg", i * 2U);
        if ( (i == 0U) && (0x0U != base_addr) ) {
            AmbaMisra_TypeCast(&pAmbaGIC_DistReg, &base_addr);
        } else if ( (i == 1U) && (0x0U != base_addr) ) {
            AmbaMisra_TypeCast(&pAmbaGIC_CpuIfReg, &base_addr);
        } else {
            AmbaMisra_TypeCast(&pAmbaGIC_VmCpuIfReg, &base_addr);
        }
#endif
    }
#endif
#endif // !defined(CONFIG_XEN_SUPPORT) || defined(AMBA_FWPROG)

    GIC_DistInit();                 /* initialization of non-banked registers for SPIs */
    AmbaRTSL_GicInitCorePrivate();  /* GIC Initializations for Core Banking (Private) registers */
}

/**
 *  AmbaRTSL_GicInitCorePrivate - GIC Initializations for Core Banking (Private) registers
 */
void AmbaRTSL_GicInitCorePrivate(void)
{
    GIC_DistInit_NonSPI();   /* initialize the banked registers for PPIs and SGIs */
    GIC_CpuIfInit();         /* initialize the CPU interface */
}

/**
 *  AmbaRTSL_GicIntGlobalEnable - GIC Global Enable interrupts
 */
void AmbaRTSL_GicIntGlobalEnable(void)
{
    pAmbaGIC_CpuIfReg->Ctrl |= (UINT32)3U;
}

/**
 *  AmbaRTSL_GicIntGlobalDisable - GIC Global Disable interrupts
 */
void AmbaRTSL_GicIntGlobalDisable(void)
{
    pAmbaGIC_CpuIfReg->Ctrl &= 0xfffffffcU;
}

UINT32 AmbaRTSL_GicIntGetGlobalState(void)
{
    UINT32 RetVal;

    if ((pAmbaGIC_CpuIfReg->Ctrl & (UINT32)0x3U) == 0U) {
        RetVal = 0U;
    } else {
        RetVal = 1U;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GicIntConfig - Hook GIC ISR
 *  @param[in] IntID Interrupt ID
 *  @param[in] pIntConfig pointer to the configurations of this interrupt
 *  @param[in] IntFunc function to be called when interrupt occurs. NULL is only valid for edge-triggered interrupt.
 *  @param[in] IntFuncArg Optional argument attached to the interrupt handler
 *  @return error code
 */
UINT32 AmbaRTSL_GicIntConfig(UINT32 IntID, const AMBA_INT_CONFIG_s *pIntConfig, AMBA_INT_ISR_f IntFunc, UINT32 IntFuncArg)
{
    UINT32 RetVal = INT_ERR_NONE;
#if defined(CONFIG_BOOT_CORE_SRTOS) && (CONFIG_BOOT_CORE_SRTOS > 0)
    const UINT32 MinTarget = 0x1U << CONFIG_BOOT_CORE_SRTOS;
#else
    const UINT32 MinTarget = 0x1U;
#endif
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    UINT32 DupIntID = 0U;
#endif

    if ((IntID >= (UINT32)AMBA_NUM_INTERRUPT) || (pIntConfig == NULL)) {
        RetVal = INT_ERR_ARG;
    } else {
        if (AmbaIsrTable[IntID] != NULL) {
            RetVal = INT_ERR_ID_CONFLIC;
        } else {
            AmbaCSL_GicSetIntConfig(IntID, pIntConfig->TriggerType);

            if (pIntConfig->CpuTargets >= MinTarget) {
                AmbaCSL_GicSetIntTargetCore(IntID, pIntConfig->CpuTargets);
            } else {
                /* if no core is assigned, default assign to first core */
                AmbaCSL_GicSetIntTargetCore(IntID, MinTarget);
            }
#if defined(CONFIG_ENABLE_AMBALINK)
#if defined(CONFIG_FWPROG_ATF_ENABLE)
            /* non-secure linux cannot see FIQ, use only IRQ */
            AmbaCSL_GicSetIntGroup(IntID, 1U);
#else
            /* secure linux cannot see IRQ, use only FIQ */
            AmbaCSL_GicSetIntGroup(IntID, 0U);
#endif
#else
            if (pIntConfig->IrqType == INT_TYPE_FIQ) {
                /* Group 0 interrupts are Secure interrupts. Group 0 interrupts could be either FIQs or IRQs. */
                AmbaCSL_GicSetIntGroup(IntID, 0U);  /* FIQEn bit of GICC_CTLR must be set */
            } else {
                /* Group 1 interrupts are Non-secure interrupts. Group 1 interrupts are always IRQs. */
                AmbaCSL_GicSetIntGroup(IntID, 1U);
            }
#endif
            AmbaIsrTable[IntID] = IntFunc;
            AmbaIsrFuncArg[IntID] = IntFuncArg;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
            DupIntID = Gic_GetDupIntID(IntID);
            if (DupIntID != 0U) {
                RetVal = Gic_DupIntConfig(DupIntID, pIntConfig);
            }
#endif
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GicIntEnable - Enable Interrupt
 *  @param[in] IntID Interrupt ID
 *  @return error code
 */
UINT32 AmbaRTSL_GicIntEnable(UINT32 IntID)
{
    UINT32 RetVal = INT_ERR_NONE;

    if (IntID >= (UINT32)AMBA_NUM_INTERRUPT) {
        RetVal = INT_ERR_ARG;
    } else {
        AmbaCSL_GicSetIntEnable(IntID);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GicIntDisable - Disable Interrupt
 *  @param[in] IntID Interrupt ID
 *  @return error code
 */
UINT32 AmbaRTSL_GicIntDisable(UINT32 IntID)
{
    UINT32 RetVal = INT_ERR_NONE;

    if (IntID >= (UINT32)AMBA_NUM_INTERRUPT) {
        RetVal = INT_ERR_ARG;
    } else {
        AmbaCSL_GicClearIntEnable(IntID);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GicSendSGI - GIC Set SGI interrrupt.
 *  @param[in] IntID SGI interrupt ID
 *  @param[in] IrqType IRQ or FIQ
 *  @param[in] SgiType CPU targets of SGI
 *  @param[in] CpuTargetList  CPU target list
 *  @return error code
 */
UINT32 AmbaRTSL_GicSendSGI(UINT32 IntID, UINT32 IrqType, UINT32 SgiType, UINT32 CpuTargetList)
{
    UINT32 RetVal = INT_ERR_NONE;

    if ((IntID >= (UINT32)AMBA_NUM_INTERRUPT) || (SgiType >= (UINT32)INT_NUM_SGI2CPU_TYPE)) {
        RetVal = INT_ERR_ARG;
    } else {
        AmbaCSL_GicSGI(IntID, IrqType, SgiType, CpuTargetList);
    }

    return RetVal;
}

#pragma GCC push_options
#pragma GCC target("general-regs-only")
/**
 *  AmbaRTSL_GicIsrIRQ - ISR (C function) for IRQ
 */
void AmbaRTSL_GicIsrIRQ(void)
{
    UINT32 CpuIntActRegVal;
    UINT32 ActIntID, CpuID;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    UINT32 DupIntID;
#endif

    CpuIntActRegVal = pAmbaGIC_CpuIfReg->AliasedIntAck;
    ActIntID = CpuIntActRegVal & (UINT32)0x3ff;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    DupIntID = Gic_GetDupIntID(ActIntID);
#endif
    CpuID = (CpuIntActRegVal >> 10U) & (UINT32)0x7U;

    if (ActIntID < (UINT32)AMBA_NUM_INTERRUPT) {
        if (AmbaIsrEntryPoint != NULL) {
            AmbaIsrEntryPoint(ActIntID);
        }

        /* a valid interrupt ID */
        if (AmbaIsrTable[ActIntID] != NULL) {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
            if (DupIntID == 0U) {
                /* No duplicated IRQ */
                AmbaIsrTable[ActIntID](ActIntID, AmbaIsrFuncArg[ActIntID]);   /* invoke the ISR */
            } else if (AmbaCSL_GicGetIntPending(DupIntID) != 0U) {
                /* Duplicated IRQ is pending as expected */
                AmbaIsrTable[ActIntID](ActIntID, AmbaIsrFuncArg[ActIntID]);   /* invoke the ISR */
                AmbaCSL_GicClearIntPending(DupIntID);
            } else {
                Gic_PrintStr("Can't get duplicated IRQ, ActIntID: ");
                Gic_PrintStrInt(ActIntID);
                Gic_PrintStr("\n\r");
            }
#else
            AmbaIsrTable[ActIntID](ActIntID, AmbaIsrFuncArg[ActIntID]);   /* invoke the ISR */
#endif
        } else {
            Gic_PrintStr("Get Invalid IRQ : ");
            Gic_PrintStrInt(ActIntID);
            Gic_PrintStr("\n\r");
        }

        if (AmbaIsrExitPoint != NULL) {
            AmbaIsrExitPoint(ActIntID);
        }
    }

    /* Copy the value of IntAckReg to the End of Interrupt Register (GICC_AEOIR) */
    pAmbaGIC_CpuIfReg->AliasedEndOfInt = (((UINT32)CpuID << 10U) | (UINT32)ActIntID);
}

/**
 *  AmbaRTSL_GicIsrFIQ - ISR for FIQ
 */
void AmbaRTSL_GicIsrFIQ(void)
{
    UINT32 CpuIntActRegVal;
    UINT32 ActIntID, CpuID;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    UINT32 DupIntID;
#endif

    CpuIntActRegVal = pAmbaGIC_CpuIfReg->IntAck;
    ActIntID = CpuIntActRegVal & (UINT32)0x3ff;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    DupIntID = Gic_GetDupIntID(ActIntID);
#endif
    CpuID = (CpuIntActRegVal >> 10U) & (UINT32)0x7U;

    if (ActIntID < (UINT32)AMBA_NUM_INTERRUPT) {
        if (AmbaIsrEntryPoint != NULL) {
            AmbaIsrEntryPoint((UINT32) ActIntID);
        }

        /* a valid interrupt ID */
        if (AmbaIsrTable[ActIntID] != NULL) {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
            if (DupIntID == 0U) {
                /* No duplicated FIQ */
                AmbaIsrTable[ActIntID](ActIntID, AmbaIsrFuncArg[ActIntID]);   /* invoke the ISR */
            } else if (AmbaCSL_GicGetIntPending(DupIntID) != 0U) {
                /* Duplicated FIQ is pending as expected */
                AmbaIsrTable[ActIntID](ActIntID, AmbaIsrFuncArg[ActIntID]);   /* invoke the ISR */
                AmbaCSL_GicClearIntPending(DupIntID);
            } else {
                Gic_PrintStr("Can't get duplicated FIQ, ActIntID: ");
                Gic_PrintStrInt(ActIntID);
                Gic_PrintStr("\n\r");
            }
#else
            AmbaIsrTable[ActIntID](ActIntID, AmbaIsrFuncArg[ActIntID]);   /* invoke the ISR */
#endif
        } else {
            Gic_PrintStr("Get Invalid FIQ : ");
            Gic_PrintStrInt(ActIntID);
            Gic_PrintStr("\n\r");
        }

        if (AmbaIsrExitPoint != NULL) {
            AmbaIsrExitPoint((UINT32) ActIntID);
        }
    }

    /* Copy the value of IntAckReg to the End of Interrupt Register (GICC_EOIR) */
    pAmbaGIC_CpuIfReg->EndOfInt = (((UINT32)CpuID << 10U) | (UINT32)ActIntID);
}
#pragma GCC pop_options

/**
 *  AmbaRTSL_GicGetIntInfo - Get GIC interrupt Info
 *  @param[in] IntID Interrupt ID
 *  @param[out] pIntInfo Interrupt state
 *  @return error code
 */
UINT32 AmbaRTSL_GicGetIntInfo(UINT32 IntID, AMBA_INT_INFO_s *pIntInfo)
{
    UINT32 RetVal = INT_ERR_NONE;

    if ((IntID >= (UINT32)AMBA_NUM_INTERRUPT) || (pIntInfo == NULL)) {
        RetVal = INT_ERR_ARG;
    } else {
        /* Interrupt sensitivity type */
        if (AmbaCSL_GicGetIntConfig(IntID) != 0x0U) {
            pIntInfo->TriggerType = INT_TRIG_RISING_EDGE;
        } else {
            pIntInfo->TriggerType = INT_TRIG_HIGH_LEVEL;
        }

        /* Interrupt type */
        if (AmbaCSL_GicGetIntGroup(IntID) != 0x0U) {
            pIntInfo->IrqType = INT_TYPE_IRQ;
        } else {
            pIntInfo->IrqType = INT_TYPE_FIQ;
        }

        /* CPU target list */
        pIntInfo->CpuTargets = (UINT8)AmbaCSL_GicGetIntTarget(IntID);

        /* Interrupt is pending or not */
        pIntInfo->IrqPending = AmbaCSL_GicGetIntPending(IntID);

        /* Interrupt is enabled or not */
        pIntInfo->IrqEnable = AmbaCSL_GicGetIntEnable(IntID);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GicGetIntConfig - Get GIC interrupt config
 *  @param[in] IntID Interrupt ID
 *  @return GIC interrupt config
 */
UINT32 AmbaRTSL_GicGetIntConfig(UINT32 IntID)
{
    return AmbaCSL_GicGetIntConfig(IntID);
}

/**
 *  AmbaRTSL_GicGetISR - Get ISR
 *  @param[in] IntID Interrupt ID
 *  @return NULL or a function pointer to ISR
 */
AMBA_INT_ISR_f AmbaRTSL_GicGetISR(UINT32 IntID)
{
    AMBA_INT_ISR_f IsrEntry = NULL;

    if (IntID < (UINT32)AMBA_NUM_INTERRUPT) {
        IsrEntry = AmbaIsrTable[IntID];
    }

    return IsrEntry;
}

/**
 *  AmbaRTSL_GicSetISR - Set ISR
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrEntry Interrupt handler
 *  @return error code
 */
UINT32 AmbaRTSL_GicSetISR(UINT32 IntID, AMBA_INT_ISR_f IsrEntry)
{
    UINT32 RetVal = INT_ERR_NONE;

    if (IntID >= (UINT32)AMBA_NUM_INTERRUPT) {
        RetVal = INT_ERR_ARG;
    } else {
        AmbaIsrTable[IntID] = IsrEntry;         /* Register the ISR */
    }

    return RetVal;
}

/**
 *  AmbaINT_SetProfileFunc - Set ISR
 *  @param[in] EntryCb A callback before ISR
 *  @param[in] ExitCb A callback after ISR
 */
void AmbaINT_SetProfileFunc(AMBA_INT_ISR_PROFILE_f EntryCb, AMBA_INT_ISR_PROFILE_f ExitCb)
{
    AmbaIsrEntryPoint = EntryCb;
    AmbaIsrExitPoint = ExitCb;
}
