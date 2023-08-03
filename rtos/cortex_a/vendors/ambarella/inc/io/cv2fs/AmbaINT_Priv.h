/**
 *  @file AmbaINT_Priv.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Common Definitions & Constants for Interrupt Controller - GIC-400 APIs
 *
 */

#ifndef AMBA_INT_PRIV_H
#define AMBA_INT_PRIV_H

/**
 *  GIC Interrupt IDs
 */
enum {
    /*
     * Software Generated Interrupt (SGI): ID0 - ID15
     */
    AMBA_INT_SGI_ID00 = 0,                           /* Software Generated Interrupt (SGI) ID 0 */
    AMBA_INT_SGI_ID01,                               /* Software Generated Interrupt (SGI) ID 1 */
    AMBA_INT_SGI_ID02,                               /* Software Generated Interrupt (SGI) ID 2 */
    AMBA_INT_SGI_ID03,                               /* Software Generated Interrupt (SGI) ID 3 */
    AMBA_INT_SGI_ID04,                               /* Software Generated Interrupt (SGI) ID 4 */
    AMBA_INT_SGI_ID05,                               /* Software Generated Interrupt (SGI) ID 5 */
    AMBA_INT_SGI_ID06,                               /* Software Generated Interrupt (SGI) ID 6 */
    AMBA_INT_SGI_ID07,                               /* Software Generated Interrupt (SGI) ID 7 */
    AMBA_INT_SGI_ID08,                               /* Software Generated Interrupt (SGI) ID 8 */
    AMBA_INT_SGI_ID09,                               /* Software Generated Interrupt (SGI) ID 9 */
    AMBA_INT_SGI_ID10,                               /* Software Generated Interrupt (SGI) ID 10 */
    AMBA_INT_SGI_ID11,                               /* Software Generated Interrupt (SGI) ID 11 */
    AMBA_INT_SGI_ID12,                               /* Software Generated Interrupt (SGI) ID 12 */
    AMBA_INT_SGI_ID13,                               /* Software Generated Interrupt (SGI) ID 13 */
    AMBA_INT_SGI_ID14,                               /* Software Generated Interrupt (SGI) ID 14 */
    AMBA_INT_SGI_ID15,                               /* Software Generated Interrupt (SGI) ID 15 */

    /*
     * Private Peripheral Interrupt (PPI): ID16 - ID31
     */
    AMBA_INT_PPI_ID016,                              /* Private Peripheral Interrupt (PPI) ID 16 */
    AMBA_INT_PPI_ID017,                              /* Private Peripheral Interrupt (PPI) ID 17 */
    AMBA_INT_PPI_ID018,                              /* Private Peripheral Interrupt (PPI) ID 18 */
    AMBA_INT_PPI_ID019,                              /* Private Peripheral Interrupt (PPI) ID 19 */
    AMBA_INT_PPI_ID020,                              /* Private Peripheral Interrupt (PPI) ID 20 */
    AMBA_INT_PPI_ID021,                              /* Private Peripheral Interrupt (PPI) ID 21 */
    AMBA_INT_PPI_ID022,                              /* Private Peripheral Interrupt (PPI) ID 22 */
    AMBA_INT_PPI_ID023,                              /* Private Peripheral Interrupt (PPI) ID 23 */
    AMBA_INT_PPI_ID024,                              /* Private Peripheral Interrupt (PPI) ID 24 */

    AMBA_INT_PPI_ID025_VIRTUAL_MAINTENANCE,          /* Private Peripheral Interrupt (PPI) ID 25: Virtual maintenance */
    AMBA_INT_PPI_ID026_HYPERVISOR_TIMER,             /* Private Peripheral Interrupt (PPI) ID 26: Hypervisor timer event */
    AMBA_INT_PPI_ID027_VIRTUAL_TIMER,                /* Private Peripheral Interrupt (PPI) ID 27: Virtual timer event */
    AMBA_INT_PPI_ID028_FIQ,                          /* Private Peripheral Interrupt (PPI) ID 28: Legacy nFIQ */
    AMBA_INT_PPI_ID029_SECURE_PHYSICAL_TIMER,        /* Private Peripheral Interrupt (PPI) ID 29: Secure physical timer event */
    AMBA_INT_PPI_ID030_NONSECURE_PHYSICAL_TIMER,     /* Private Peripheral Interrupt (PPI) ID 30: Non-secure physical timer event */
    AMBA_INT_PPI_ID031_IRQ,                          /* Private Peripheral Interrupt (PPI) ID 31: Legacy nIRQ */

    /*
     * Shared Peripheral Interrupt (SPI): ID32 - ID192
     */
    AMBA_INT_SPI_ID032_CEHU0,                        /* Shared Peripheral Interrupt (SPI) ID 32 */
    AMBA_INT_SPI_ID033_CEHU1,                        /* Shared Peripheral Interrupt (SPI) ID 33 */
    AMBA_INT_SPI_ID034_CA53_EXTERRIRQ,               /* Shared Peripheral Interrupt (SPI) ID 34 */
    AMBA_INT_SPI_ID035_CA53_PMUIRQ0,                 /* Shared Peripheral Interrupt (SPI) ID 35 */
    AMBA_INT_SPI_ID036_CA53_PMUIRQ1,                 /* Shared Peripheral Interrupt (SPI) ID 36 */
    AMBA_INT_SPI_ID037_CA53_PMUIRQ2,                 /* Shared Peripheral Interrupt (SPI) ID 37 */
    AMBA_INT_SPI_ID038_CA53_PMUIRQ3,                 /* Shared Peripheral Interrupt (SPI) ID 38 */
    AMBA_INT_SPI_ID039_CAN0,                         /* Shared Peripheral Interrupt (SPI) ID 39 */
    AMBA_INT_SPI_ID040_CAN1,                         /* Shared Peripheral Interrupt (SPI) ID 40 */
    AMBA_INT_SPI_ID041_CAN2,                         /* Shared Peripheral Interrupt (SPI) ID 41 */
    AMBA_INT_SPI_ID042_CAN3,                         /* Shared Peripheral Interrupt (SPI) ID 42 */
    AMBA_INT_SPI_ID043_CAN4,                         /* Shared Peripheral Interrupt (SPI) ID 43 */
    AMBA_INT_SPI_ID044_CAN5,                         /* Shared Peripheral Interrupt (SPI) ID 44 */
    AMBA_INT_SPI_ID045_CA53_INTERRIRQ,               /* Shared Peripheral Interrupt (SPI) ID 45 */
    AMBA_INT_SPI_ID046_I2C_MASTER0,                  /* Shared Peripheral Interrupt (SPI) ID 46 */
    AMBA_INT_SPI_ID047_I2C_MASTER1,                  /* Shared Peripheral Interrupt (SPI) ID 47 */
    AMBA_INT_SPI_ID048_I2C_MASTER2,                  /* Shared Peripheral Interrupt (SPI) ID 48 */
    AMBA_INT_SPI_ID049_I2C_MASTER3,                  /* Shared Peripheral Interrupt (SPI) ID 49 */
    AMBA_INT_SPI_ID050_I2C_MASTER4,                  /* Shared Peripheral Interrupt (SPI) ID 50 */
    AMBA_INT_SPI_ID051_I2C_MASTER5,                  /* Shared Peripheral Interrupt (SPI) ID 51 */
    AMBA_INT_SPI_ID052_I2C_SLAVE,                    /* Shared Peripheral Interrupt (SPI) ID 52 */
    AMBA_INT_SPI_ID053_UART_APB,                     /* Shared Peripheral Interrupt (SPI) ID 53 */
    AMBA_INT_SPI_ID054_TIMER0,                       /* Shared Peripheral Interrupt (SPI) ID 54 */
    AMBA_INT_SPI_ID055_TIMER1,                       /* Shared Peripheral Interrupt (SPI) ID 55 */
    AMBA_INT_SPI_ID056_TIMER2,                       /* Shared Peripheral Interrupt (SPI) ID 56 */
    AMBA_INT_SPI_ID057_TIMER3,                       /* Shared Peripheral Interrupt (SPI) ID 57 */
    AMBA_INT_SPI_ID058_TIMER4,                       /* Shared Peripheral Interrupt (SPI) ID 58 */
    AMBA_INT_SPI_ID059_TIMER5,                       /* Shared Peripheral Interrupt (SPI) ID 59 */
    AMBA_INT_SPI_ID060_TIMER6,                       /* Shared Peripheral Interrupt (SPI) ID 60 */
    AMBA_INT_SPI_ID061_TIMER7,                       /* Shared Peripheral Interrupt (SPI) ID 61 */
    AMBA_INT_SPI_ID062_TIMER8,                       /* Shared Peripheral Interrupt (SPI) ID 62 */
    AMBA_INT_SPI_ID063_TIMER9,                       /* Shared Peripheral Interrupt (SPI) ID 63 */

    AMBA_INT_SPI_ID064_TIMER10,                      /* Shared Peripheral Interrupt (SPI) ID 64 */
    AMBA_INT_SPI_ID065_TIMER11,                      /* Shared Peripheral Interrupt (SPI) ID 65 */
    AMBA_INT_SPI_ID066_TIMER12,                      /* Shared Peripheral Interrupt (SPI) ID 66 */
    AMBA_INT_SPI_ID067_TIMER13,                      /* Shared Peripheral Interrupt (SPI) ID 67 */
    AMBA_INT_SPI_ID068_TIMER14,                      /* Shared Peripheral Interrupt (SPI) ID 68 */
    AMBA_INT_SPI_ID069_TIMER15,                      /* Shared Peripheral Interrupt (SPI) ID 69 */
    AMBA_INT_SPI_ID070_TIMER16,                      /* Shared Peripheral Interrupt (SPI) ID 70 */
    AMBA_INT_SPI_ID071_TIMER17,                      /* Shared Peripheral Interrupt (SPI) ID 71 */
    AMBA_INT_SPI_ID072_TIMER18,                      /* Shared Peripheral Interrupt (SPI) ID 72 */
    AMBA_INT_SPI_ID073_TIMER19,                      /* Shared Peripheral Interrupt (SPI) ID 73 */
    AMBA_INT_SPI_ID074_TIMER20,                      /* Shared Peripheral Interrupt (SPI) ID 74 */
    AMBA_INT_SPI_ID075_TIMER21,                      /* Shared Peripheral Interrupt (SPI) ID 75 */
    AMBA_INT_SPI_ID076_TIMER22,                      /* Shared Peripheral Interrupt (SPI) ID 76 */
    AMBA_INT_SPI_ID077_TIMER23,                      /* Shared Peripheral Interrupt (SPI) ID 77 */
    AMBA_INT_SPI_ID078_TIMER24,                      /* Shared Peripheral Interrupt (SPI) ID 78 */
    AMBA_INT_SPI_ID079_TIMER25,                      /* Shared Peripheral Interrupt (SPI) ID 79 */
    AMBA_INT_SPI_ID080_TIMER26,                      /* Shared Peripheral Interrupt (SPI) ID 80 */
    AMBA_INT_SPI_ID081_TIMER27,                      /* Shared Peripheral Interrupt (SPI) ID 81 */
    AMBA_INT_SPI_ID082_TIMER28,                      /* Shared Peripheral Interrupt (SPI) ID 82 */
    AMBA_INT_SPI_ID083_TIMER29,                      /* Shared Peripheral Interrupt (SPI) ID 83 */
    AMBA_INT_SPI_ID084_WDT,                          /* Shared Peripheral Interrupt (SPI) ID 84 */
    AMBA_INT_SPI_ID085_GPIO_GROUP0,                  /* Shared Peripheral Interrupt (SPI) ID 85 */
    AMBA_INT_SPI_ID086_GPIO_GROUP1,                  /* Shared Peripheral Interrupt (SPI) ID 86 */
    AMBA_INT_SPI_ID087_GPIO_GROUP2,                  /* Shared Peripheral Interrupt (SPI) ID 87 */
    AMBA_INT_SPI_ID088_GPIO_GROUP3,                  /* Shared Peripheral Interrupt (SPI) ID 88 */
    AMBA_INT_SPI_ID089_USB_ID_CHANGE,                /* Shared Peripheral Interrupt (SPI) ID 89 */
    AMBA_INT_SPI_ID090_USB_CONNECT,                  /* Shared Peripheral Interrupt (SPI) ID 90 */
    AMBA_INT_SPI_ID091_USB_CONNECT_CHANGE,           /* Shared Peripheral Interrupt (SPI) ID 91 */
    AMBA_INT_SPI_ID092_USB_CHARGE_DETECT,            /* Shared Peripheral Interrupt (SPI) ID 92 */
    AMBA_INT_SPI_ID093_SDIO0_CARD_DETECT,            /* Shared Peripheral Interrupt (SPI) ID 93 */
    AMBA_INT_SPI_ID094_SD_CARD_DETECT,               /* Shared Peripheral Interrupt (SPI) ID 94 */
    AMBA_INT_SPI_ID095_ENET0_PMT,                    /* Shared Peripheral Interrupt (SPI) ID 95 */

    AMBA_INT_SPI_ID096_ENET0_SBD,                    /* Shared Peripheral Interrupt (SPI) ID 96 */
    AMBA_INT_SPI_ID097_UART_AHB0,                    /* Shared Peripheral Interrupt (SPI) ID 97 */
    AMBA_INT_SPI_ID098_UART_AHB1,                    /* Shared Peripheral Interrupt (SPI) ID 98 */
    AMBA_INT_SPI_ID099_UART_AHB2,                    /* Shared Peripheral Interrupt (SPI) ID 99 */
    AMBA_INT_SPI_ID100_UART_AHB3,                    /* Shared Peripheral Interrupt (SPI) ID 100 */
    AMBA_INT_SPI_ID101_USB_HOST_EHCI,                /* Shared Peripheral Interrupt (SPI) ID 101 */
    AMBA_INT_SPI_ID102_USB_HOST_OHCI,                /* Shared Peripheral Interrupt (SPI) ID 102 */
    AMBA_INT_SPI_ID103_USB,                          /* Shared Peripheral Interrupt (SPI) ID 103 */
    AMBA_INT_SPI_ID104_FIO,                          /* Shared Peripheral Interrupt (SPI) ID 104 */
    AMBA_INT_SPI_ID105_FIO_ECC_RPT,                  /* Shared Peripheral Interrupt (SPI) ID 105 */
    AMBA_INT_SPI_ID106_GDMA,                         /* Shared Peripheral Interrupt (SPI) ID 106 */
    AMBA_INT_SPI_ID107_SDIO0,                        /* Shared Peripheral Interrupt (SPI) ID 107 */
    AMBA_INT_SPI_ID108_SD,                           /* Shared Peripheral Interrupt (SPI) ID 108 */
    AMBA_INT_SPI_ID109_SPI_NOR,                      /* Shared Peripheral Interrupt (SPI) ID 109 */
    AMBA_INT_SPI_ID110_SSI_MASTER0,                  /* Shared Peripheral Interrupt (SPI) ID 110 */
    AMBA_INT_SPI_ID111_SSI_MASTER1,                  /* Shared Peripheral Interrupt (SPI) ID 111 */
    AMBA_INT_SPI_ID112_SSI_MASTER2,                  /* Shared Peripheral Interrupt (SPI) ID 112 */
    AMBA_INT_SPI_ID113_SSI_MASTER3,                  /* Shared Peripheral Interrupt (SPI) ID 113 */
    AMBA_INT_SPI_ID114_SSI_SLAVE,                    /* Shared Peripheral Interrupt (SPI) ID 114 */
    AMBA_INT_SPI_ID115_I2S_TX,                       /* Shared Peripheral Interrupt (SPI) ID 115 */
    AMBA_INT_SPI_ID116_I2S_RX,                       /* Shared Peripheral Interrupt (SPI) ID 116 */
    AMBA_INT_SPI_ID117_DMA_ENGINE0,                  /* Shared Peripheral Interrupt (SPI) ID 117 */
    AMBA_INT_SPI_ID118_DMA_ENGINE1,                  /* Shared Peripheral Interrupt (SPI) ID 118 */
    AMBA_INT_SPI_ID119_VOUT_B_INT,                   /* Shared Peripheral Interrupt (SPI) ID 119 */
    AMBA_INT_SPI_ID120_VOUT_A_INT,                   /* Shared Peripheral Interrupt (SPI) ID 120 */
    AMBA_INT_SPI_ID121_HRNG,                         /* Shared Peripheral Interrupt (SPI) ID 121 */
    AMBA_INT_SPI_ID122_AXI_SOFT_IRQ0,                /* Shared Peripheral Interrupt (SPI) ID 122 */
    AMBA_INT_SPI_ID123_AXI_SOFT_IRQ1,                /* Shared Peripheral Interrupt (SPI) ID 123 */
    AMBA_INT_SPI_ID124_AXI_SOFT_IRQ2,                /* Shared Peripheral Interrupt (SPI) ID 124 */
    AMBA_INT_SPI_ID125_AXI_SOFT_IRQ3,                /* Shared Peripheral Interrupt (SPI) ID 125 */
    AMBA_INT_SPI_ID126_AXI_SOFT_IRQ4,                /* Shared Peripheral Interrupt (SPI) ID 126 */
    AMBA_INT_SPI_ID127_AXI_SOFT_IRQ5,                /* Shared Peripheral Interrupt (SPI) ID 127 */

    AMBA_INT_SPI_ID128_AXI_SOFT_IRQ6,                /* Shared Peripheral Interrupt (SPI) ID 128 */
    AMBA_INT_SPI_ID129_AXI_SOFT_IRQ7,                /* Shared Peripheral Interrupt (SPI) ID 129 */
    AMBA_INT_SPI_ID130_AXI_SOFT_IRQ8,                /* Shared Peripheral Interrupt (SPI) ID 130 */
    AMBA_INT_SPI_ID131_AXI_SOFT_IRQ9,                /* Shared Peripheral Interrupt (SPI) ID 131 */
    AMBA_INT_SPI_ID132_AXI_SOFT_IRQ10,               /* Shared Peripheral Interrupt (SPI) ID 132 */
    AMBA_INT_SPI_ID133_AXI_SOFT_IRQ11,               /* Shared Peripheral Interrupt (SPI) ID 133 */
    AMBA_INT_SPI_ID134_AXI_SOFT_IRQ12,               /* Shared Peripheral Interrupt (SPI) ID 134 */
    AMBA_INT_SPI_ID135_AXI_SOFT_IRQ13,               /* Shared Peripheral Interrupt (SPI) ID 135 */
    AMBA_INT_SPI_ID136_DRAM_ERROR,                   /* Shared Peripheral Interrupt (SPI) ID 136 */
    AMBA_INT_SPI_ID137_VP0_EXCEPTION,                /* Shared Peripheral Interrupt (SPI) ID 137 */
    AMBA_INT_SPI_ID138_VIN0_SLAVE_VSYNC,             /* Shared Peripheral Interrupt (SPI) ID 138 */
    AMBA_INT_SPI_ID139_VIN0_SOF,                     /* Shared Peripheral Interrupt (SPI) ID 139 */
    AMBA_INT_SPI_ID140_VIN0_MASTER_VSYNC,            /* Shared Peripheral Interrupt (SPI) ID 140 */
    AMBA_INT_SPI_ID141_VIN0_LAST_PIXEL,              /* Shared Peripheral Interrupt (SPI) ID 141 */
    AMBA_INT_SPI_ID142_VIN0_DELAYED_VSYNC,           /* Shared Peripheral Interrupt (SPI) ID 142 */
    AMBA_INT_SPI_ID143_VIN1_SLAVE_VSYNC,             /* Shared Peripheral Interrupt (SPI) ID 143 */
    AMBA_INT_SPI_ID144_VIN1_SOF,                     /* Shared Peripheral Interrupt (SPI) ID 144 */
    AMBA_INT_SPI_ID145_VIN1_MASTER_VSYNC,            /* Shared Peripheral Interrupt (SPI) ID 145 */
    AMBA_INT_SPI_ID146_VIN1_LAST_PIXEL,              /* Shared Peripheral Interrupt (SPI) ID 146 */
    AMBA_INT_SPI_ID147_VIN1_DELAYED_VSYNC,           /* Shared Peripheral Interrupt (SPI) ID 147 */
    AMBA_INT_SPI_ID148_VIN2_SLAVE_VSYNC,             /* Shared Peripheral Interrupt (SPI) ID 148 */
    AMBA_INT_SPI_ID149_VIN2_SOF,                     /* Shared Peripheral Interrupt (SPI) ID 149 */
    AMBA_INT_SPI_ID150_VIN2_LAST_PIXEL,              /* Shared Peripheral Interrupt (SPI) ID 150 */
    AMBA_INT_SPI_ID151_VIN2_DELAYED_VSYNC,           /* Shared Peripheral Interrupt (SPI) ID 151 */
    AMBA_INT_SPI_ID152_VIN3_SLAVE_VSYNC,             /* Shared Peripheral Interrupt (SPI) ID 152 */
    AMBA_INT_SPI_ID153_VIN3_SOF,                     /* Shared Peripheral Interrupt (SPI) ID 153 */
    AMBA_INT_SPI_ID154_APB_DBG_LOCK_ACCESS,          /* Shared Peripheral Interrupt (SPI) ID 154 */
    AMBA_INT_SPI_ID155_VIN3_LAST_PIXEL,              /* Shared Peripheral Interrupt (SPI) ID 155 */
    AMBA_INT_SPI_ID156_VIN3_DELAYED_VSYNC,           /* Shared Peripheral Interrupt (SPI) ID 156 */
    AMBA_INT_SPI_ID157_VIN4_SLAVE_VSYNC,             /* Shared Peripheral Interrupt (SPI) ID 157 */
    AMBA_INT_SPI_ID158_VIN4_SOF,                     /* Shared Peripheral Interrupt (SPI) ID 158 */
    AMBA_INT_SPI_ID159_AXI_CFG_LOCK_ACCESS,          /* Shared Peripheral Interrupt (SPI) ID 159 */

    AMBA_INT_SPI_ID160_VIN4_LAST_PIXEL,              /* Shared Peripheral Interrupt (SPI) ID 160 */
    AMBA_INT_SPI_ID161_VIN4_DELAYED_VSYNC,           /* Shared Peripheral Interrupt (SPI) ID 161 */
    AMBA_INT_SPI_ID162_SAHB_CFG_LOCK_ACCESS,         /* Shared Peripheral Interrupt (SPI) ID 162 */
    AMBA_INT_SPI_ID163_NSAHB_CFG_LOCK_ACCESS,        /* Shared Peripheral Interrupt (SPI) ID 163 */
    AMBA_INT_SPI_ID164_ENET1_PMT,                    /* Shared Peripheral Interrupt (SPI) ID 164 */
    AMBA_INT_SPI_ID165_ENET1_SBD,                    /* Shared Peripheral Interrupt (SPI) ID 165 */
    AMBA_INT_SPI_ID166_SSI_MASTER4,                  /* Shared Peripheral Interrupt (SPI) ID 166 */
    AMBA_INT_SPI_ID167_SSI_MASTER5,                  /* Shared Peripheral Interrupt (SPI) ID 167 */
    AMBA_INT_SPI_ID168_WDT_5,                        /* Shared Peripheral Interrupt (SPI) ID 168 */
    AMBA_INT_SPI_ID169_WDT_1,                        /* Shared Peripheral Interrupt (SPI) ID 169 */
    AMBA_INT_SPI_ID170_WDT_2,                        /* Shared Peripheral Interrupt (SPI) ID 170 */
    AMBA_INT_SPI_ID171_WDT_3,                        /* Shared Peripheral Interrupt (SPI) ID 171 */
    AMBA_INT_SPI_ID172_WDT_4,                        /* Shared Peripheral Interrupt (SPI) ID 172 */
    AMBA_INT_SPI_ID173_VIN8_SLAVE_VSYNC,             /* Shared Peripheral Interrupt (SPI) ID 173 */
    AMBA_INT_SPI_ID174_VIN8_SOF,                     /* Shared Peripheral Interrupt (SPI) ID 174 */
    AMBA_INT_SPI_ID175_VIN8_LAST_PIXEL,              /* Shared Peripheral Interrupt (SPI) ID 175 */
    AMBA_INT_SPI_ID176_VIN8_DELAYED_VSYNC,           /* Shared Peripheral Interrupt (SPI) ID 176 */
    AMBA_INT_SPI_ID177_CODE_VDSP0_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 177 */
    AMBA_INT_SPI_ID178_CODE_VDSP1_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 178 */
    AMBA_INT_SPI_ID179_CODE_VDSP2_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 179 */
    AMBA_INT_SPI_ID180_CODE_VDSP3_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 180 */
    AMBA_INT_SPI_ID181_CODE_VIN0_IRQ,                /* Shared Peripheral Interrupt (SPI) ID 181 */
    AMBA_INT_SPI_ID182_CODE_VIN1_IRQ,                /* Shared Peripheral Interrupt (SPI) ID 182 */
    AMBA_INT_SPI_ID183_CODE_VIN2_IRQ,                /* Shared Peripheral Interrupt (SPI) ID 183 */
    AMBA_INT_SPI_ID184_CODE_VIN3_IRQ,                /* Shared Peripheral Interrupt (SPI) ID 184 */
    AMBA_INT_SPI_ID185_CODE_VIN4_IRQ,                /* Shared Peripheral Interrupt (SPI) ID 185 */
    AMBA_INT_SPI_ID186,                              /* Shared Peripheral Interrupt (SPI) ID 186 */
    AMBA_INT_SPI_ID187,                              /* Shared Peripheral Interrupt (SPI) ID 187 */
    AMBA_INT_SPI_ID188,                              /* Shared Peripheral Interrupt (SPI) ID 188 */
    AMBA_INT_SPI_ID189_CODE_VIN8_IRQ,                /* Shared Peripheral Interrupt (SPI) ID 189 */
    AMBA_INT_SPI_ID190_CODE_VOUT0_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 190 */
    AMBA_INT_SPI_ID191_CODE_VOUT1_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 191 */

    AMBA_INT_SPI_ID192_VORC_L2C_EVENT_IRQ,           /* Shared Peripheral Interrupt (SPI) ID 192 */
    AMBA_INT_SPI_ID193_VORC_THREAD0_IRQ,             /* Shared Peripheral Interrupt (SPI) ID 193 */
    AMBA_INT_SPI_ID194_VORC_THREAD1_IRQ,             /* Shared Peripheral Interrupt (SPI) ID 194 */
    AMBA_INT_SPI_ID195_VORC_THREAD2_IRQ,             /* Shared Peripheral Interrupt (SPI) ID 195 */
    AMBA_INT_SPI_ID196_VORC_THREAD3_IRQ,             /* Shared Peripheral Interrupt (SPI) ID 196 */
    AMBA_INT_SPI_ID197_VORC_THREAD4_IRQ,             /* Shared Peripheral Interrupt (SPI) ID 197 */
    AMBA_INT_SPI_ID198_DDRC0,                        /* Shared Peripheral Interrupt (SPI) ID 198 */
    AMBA_INT_SPI_ID199_DDRC1,                        /* Shared Peripheral Interrupt (SPI) ID 199 */
    AMBA_INT_SPI_ID200_IDSP_SAFETY,                  /* Shared Peripheral Interrupt (SPI) ID 200 */
    AMBA_INT_SPI_ID201,                              /* Shared Peripheral Interrupt (SPI) ID 201 */
    AMBA_INT_SPI_ID202_CODE_VDSP0_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 202 */
    AMBA_INT_SPI_ID203_CODE_VDSP1_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 203 */
    AMBA_INT_SPI_ID204_CODE_VDSP2_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 204 */
    AMBA_INT_SPI_ID205_CODE_VDSP3_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 205 */
    AMBA_INT_SPI_ID206_CODE_VIN0_IRQ,                /* Shared Peripheral Interrupt (SPI) ID 206 */
    AMBA_INT_SPI_ID207_CODE_VIN1_IRQ,                /* Shared Peripheral Interrupt (SPI) ID 207 */
    AMBA_INT_SPI_ID208_CODE_VIN2_IRQ,                /* Shared Peripheral Interrupt (SPI) ID 208 */
    AMBA_INT_SPI_ID209_CODE_VIN3_IRQ,                /* Shared Peripheral Interrupt (SPI) ID 209 */
    AMBA_INT_SPI_ID210_CODE_VIN4_IRQ,                /* Shared Peripheral Interrupt (SPI) ID 210 */
    AMBA_INT_SPI_ID211,                              /* Shared Peripheral Interrupt (SPI) ID 211 */
    AMBA_INT_SPI_ID212,                              /* Shared Peripheral Interrupt (SPI) ID 212 */
    AMBA_INT_SPI_ID213,                              /* Shared Peripheral Interrupt (SPI) ID 213 */
    AMBA_INT_SPI_ID214_CODE_VIN8_IRQ,                /* Shared Peripheral Interrupt (SPI) ID 214 */
    AMBA_INT_SPI_ID215_CODE_VOUT0_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 215 */
    AMBA_INT_SPI_ID216_CODE_VOUT1_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 216 */
    AMBA_INT_SPI_ID217_VORC_L2C_EVENT_IRQ,           /* Shared Peripheral Interrupt (SPI) ID 217 */
    AMBA_INT_SPI_ID218_VORC_THREAD0_IRQ,             /* Shared Peripheral Interrupt (SPI) ID 218 */
    AMBA_INT_SPI_ID219_VORC_THREAD1_IRQ,             /* Shared Peripheral Interrupt (SPI) ID 219 */
    AMBA_INT_SPI_ID220_VORC_THREAD2_IRQ,             /* Shared Peripheral Interrupt (SPI) ID 220 */
    AMBA_INT_SPI_ID221_VORC_THREAD3_IRQ,             /* Shared Peripheral Interrupt (SPI) ID 221 */
    AMBA_INT_SPI_ID222_VORC_THREAD4_IRQ,             /* Shared Peripheral Interrupt (SPI) ID 222 */
    AMBA_INT_SPI_ID223_GPIO_GROUP0,                  /* Shared Peripheral Interrupt (SPI) ID 223 */

    AMBA_INT_SPI_ID224_GPIO_GROUP1,                  /* Shared Peripheral Interrupt (SPI) ID 224 */
    AMBA_INT_SPI_ID225_GPIO_GROUP2,                  /* Shared Peripheral Interrupt (SPI) ID 225 */
    AMBA_INT_SPI_ID226_GPIO_GROUP3,                  /* Shared Peripheral Interrupt (SPI) ID 226 */
    AMBA_INT_SPI_ID227_CAN0,                         /* Shared Peripheral Interrupt (SPI) ID 227 */
    AMBA_INT_SPI_ID228_CAN1,                         /* Shared Peripheral Interrupt (SPI) ID 228 */
    AMBA_INT_SPI_ID229_CAN2,                         /* Shared Peripheral Interrupt (SPI) ID 229 */
    AMBA_INT_SPI_ID230_CAN3,                         /* Shared Peripheral Interrupt (SPI) ID 230 */
    AMBA_INT_SPI_ID231_CAN4,                         /* Shared Peripheral Interrupt (SPI) ID 231 */
    AMBA_INT_SPI_ID232_CAN5,                         /* Shared Peripheral Interrupt (SPI) ID 232 */
    AMBA_INT_SPI_ID233_SSI_SLAVE,                    /* Shared Peripheral Interrupt (SPI) ID 233 */
    AMBA_INT_SPI_ID234_SSI_MASTER0,                  /* Shared Peripheral Interrupt (SPI) ID 234 */
    AMBA_INT_SPI_ID235_SSI_MASTER1,                  /* Shared Peripheral Interrupt (SPI) ID 235 */
    AMBA_INT_SPI_ID236_SSI_MASTER2,                  /* Shared Peripheral Interrupt (SPI) ID 236 */
    AMBA_INT_SPI_ID237_SSI_MASTER3,                  /* Shared Peripheral Interrupt (SPI) ID 237 */
    AMBA_INT_SPI_ID238_SSI_MASTER4,                  /* Shared Peripheral Interrupt (SPI) ID 238 */
    AMBA_INT_SPI_ID239_SSI_MASTER5,                  /* Shared Peripheral Interrupt (SPI) ID 239 */
    AMBA_INT_SPI_ID240_WDT,                          /* Shared Peripheral Interrupt (SPI) ID 240 */
    AMBA_INT_SPI_ID241_WDT_CR52,                     /* Shared Peripheral Interrupt (SPI) ID 241 */
    AMBA_INT_SPI_ID242_WDT_CA53_0,                   /* Shared Peripheral Interrupt (SPI) ID 242 */
    AMBA_INT_SPI_ID243_WDT_CA53_1,                   /* Shared Peripheral Interrupt (SPI) ID 243 */
    AMBA_INT_SPI_ID244_WDT_CA53_2,                   /* Shared Peripheral Interrupt (SPI) ID 244 */
    AMBA_INT_SPI_ID245_WDT_CA53_3,                   /* Shared Peripheral Interrupt (SPI) ID 245 */
    AMBA_INT_SPI_ID246_I2C_SLAVE,                    /* Shared Peripheral Interrupt (SPI) ID 246 */
    AMBA_INT_SPI_ID247_I2C_MASTER0,                  /* Shared Peripheral Interrupt (SPI) ID 247 */
    AMBA_INT_SPI_ID248_I2C_MASTER1,                  /* Shared Peripheral Interrupt (SPI) ID 248 */
    AMBA_INT_SPI_ID249_I2C_MASTER2,                  /* Shared Peripheral Interrupt (SPI) ID 249 */
    AMBA_INT_SPI_ID250_I2C_MASTER3,                  /* Shared Peripheral Interrupt (SPI) ID 250 */
    AMBA_INT_SPI_ID251_I2C_MASTER4,                  /* Shared Peripheral Interrupt (SPI) ID 251 */
    AMBA_INT_SPI_ID252_I2C_MASTER5,                  /* Shared Peripheral Interrupt (SPI) ID 252 */
    AMBA_INT_SPI_ID253_DMA_ENGINE0,                  /* Shared Peripheral Interrupt (SPI) ID 253 */
    AMBA_INT_SPI_ID254_DMA_ENGINE1,                  /* Shared Peripheral Interrupt (SPI) ID 254 */
    AMBA_INT_SPI_ID255,                              /* Shared Peripheral Interrupt (SPI) ID 255 */

    AMBA_NUM_INTERRUPT                          /* Total number of Interrupts supported */
};

#define AMBA_INTERRUPT_NUM 256

#endif /* AMBA_INT_PRIV_H */
