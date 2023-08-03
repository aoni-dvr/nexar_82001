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

/*
 * Cortex-A53 GIC Interrupt IDs
 */
enum {
    /*
     * Software Generated Interrupt (SGI): ID0 - ID15
     */
    AMBA_INT_SGI_ID0 = 0,                           /* Software Generated Interrupt (SGI) ID 0 */
    AMBA_INT_SGI_ID1,                               /* Software Generated Interrupt (SGI) ID 1 */
    AMBA_INT_SGI_ID2,                               /* Software Generated Interrupt (SGI) ID 2 */
    AMBA_INT_SGI_ID3,                               /* Software Generated Interrupt (SGI) ID 3 */
    AMBA_INT_SGI_ID4,                               /* Software Generated Interrupt (SGI) ID 4 */
    AMBA_INT_SGI_ID5,                               /* Software Generated Interrupt (SGI) ID 5 */
    AMBA_INT_SGI_ID6,                               /* Software Generated Interrupt (SGI) ID 6 */
    AMBA_INT_SGI_ID7,                               /* Software Generated Interrupt (SGI) ID 7 */
    AMBA_INT_SGI_ID8,                               /* Software Generated Interrupt (SGI) ID 8 */
    AMBA_INT_SGI_ID9,                               /* Software Generated Interrupt (SGI) ID 9 */
    AMBA_INT_SGI_ID10,                              /* Software Generated Interrupt (SGI) ID 10 */
    AMBA_INT_SGI_ID11,                              /* Software Generated Interrupt (SGI) ID 11 */
    AMBA_INT_SGI_ID12,                              /* Software Generated Interrupt (SGI) ID 12 */
    AMBA_INT_SGI_ID13,                              /* Software Generated Interrupt (SGI) ID 13 */
    AMBA_INT_SGI_ID14,                              /* Software Generated Interrupt (SGI) ID 14 */
    AMBA_INT_SGI_ID15,                              /* Software Generated Interrupt (SGI) ID 15 */

    /*
     * Private Peripheral Interrupt (PPI): ID16 - ID31
     */
    AMBA_INT_PPI_ID16,                              /* Private Peripheral Interrupt (PPI) ID 16 */
    AMBA_INT_PPI_ID17,                              /* Private Peripheral Interrupt (PPI) ID 17 */
    AMBA_INT_PPI_ID18,                              /* Private Peripheral Interrupt (PPI) ID 18 */
    AMBA_INT_PPI_ID19,                              /* Private Peripheral Interrupt (PPI) ID 19 */
    AMBA_INT_PPI_ID20,                              /* Private Peripheral Interrupt (PPI) ID 20 */
    AMBA_INT_PPI_ID21,                              /* Private Peripheral Interrupt (PPI) ID 21 */
    AMBA_INT_PPI_ID22,                              /* Private Peripheral Interrupt (PPI) ID 22 */
    AMBA_INT_PPI_ID23,                              /* Private Peripheral Interrupt (PPI) ID 23 */
    AMBA_INT_PPI_ID24,                              /* Private Peripheral Interrupt (PPI) ID 24 */

    AMBA_INT_PPI_ID25_VIRTUAL_MAINTENANCE,          /* Private Peripheral Interrupt (PPI) ID 25: Virtual maintenance */
    AMBA_INT_PPI_ID26_HYPERVISOR_TIMER,             /* Private Peripheral Interrupt (PPI) ID 26: Hypervisor timer event */
    AMBA_INT_PPI_ID27_VIRTUAL_TIMER,                /* Private Peripheral Interrupt (PPI) ID 27: Virtual timer event */
    AMBA_INT_PPI_ID28_FIQ,                          /* Private Peripheral Interrupt (PPI) ID 28: Legacy nFIQ */
    AMBA_INT_PPI_ID29_SECURE_PHYSICAL_TIMER,        /* Private Peripheral Interrupt (PPI) ID 29: Secure physical timer event */
    AMBA_INT_PPI_ID30_NONSECURE_PHYSICAL_TIMER,     /* Private Peripheral Interrupt (PPI) ID 30: Non-secure physical timer event */
    AMBA_INT_PPI_ID31_IRQ,                          /* Private Peripheral Interrupt (PPI) ID 31: Legacy nIRQ */

    /*
     * Shared Peripheral Interrupt (SPI): ID32 - ID192
     */
    AMBA_INT_SPI_ID32,                              /* Shared Peripheral Interrupt (SPI) ID 32 */
    AMBA_INT_SPI_ID33,                              /* Shared Peripheral Interrupt (SPI) ID 33 */
    AMBA_INT_SPI_ID34,                              /* Shared Peripheral Interrupt (SPI) ID 34 */
    AMBA_INT_SPI_ID35_PMUIRQ0,                      /* Shared Peripheral Interrupt (SPI) ID 35 */
    AMBA_INT_SPI_ID36_PMUIRQ1,                      /* Shared Peripheral Interrupt (SPI) ID 36 */
    AMBA_INT_SPI_ID37_CLUSTER_PMUIRQ,               /* Shared Peripheral Interrupt (SPI) ID 37 */
    AMBA_INT_SPI_ID38,                              /* Shared Peripheral Interrupt (SPI) ID 38 */
    AMBA_INT_SPI_ID39_CAN0,                         /* Shared Peripheral Interrupt (SPI) ID 39 */
    AMBA_INT_SPI_ID40_CAN1,                         /* Shared Peripheral Interrupt (SPI) ID 40 */
    AMBA_INT_SPI_ID41_LPI,                          /* Shared Peripheral Interrupt (SPI) ID 41 */
    AMBA_INT_SPI_ID42_LPI1,                         /* Shared Peripheral Interrupt (SPI) ID 42 */
    AMBA_INT_SPI_ID43,                              /* Shared Peripheral Interrupt (SPI) ID 43 */
    AMBA_INT_SPI_ID44,                              /* Shared Peripheral Interrupt (SPI) ID 44 */
    AMBA_INT_SPI_ID45,                              /* Shared Peripheral Interrupt (SPI) ID 45 */
    AMBA_INT_SPI_ID46_I2C_MASTER0,                  /* Shared Peripheral Interrupt (SPI) ID 46 */
    AMBA_INT_SPI_ID47_I2C_MASTER1,                  /* Shared Peripheral Interrupt (SPI) ID 47 */
    AMBA_INT_SPI_ID48_I2C_MASTER2,                  /* Shared Peripheral Interrupt (SPI) ID 48 */
    AMBA_INT_SPI_ID49_I2C_MASTER3,                  /* Shared Peripheral Interrupt (SPI) ID 49 */
    AMBA_INT_SPI_ID50_I2C_MASTER4,                  /* Shared Peripheral Interrupt (SPI) ID 50 */
    AMBA_INT_SPI_ID51_I2C_MASTER5,                  /* Shared Peripheral Interrupt (SPI) ID 51 */
    AMBA_INT_SPI_ID52_I2C_SLAVE,                    /* Shared Peripheral Interrupt (SPI) ID 52 */
    AMBA_INT_SPI_ID53_UART_APB,                     /* Shared Peripheral Interrupt (SPI) ID 53 */
    AMBA_INT_SPI_ID54_TIMER0,                       /* Shared Peripheral Interrupt (SPI) ID 54 */
    AMBA_INT_SPI_ID55_TIMER1,                       /* Shared Peripheral Interrupt (SPI) ID 55 */
    AMBA_INT_SPI_ID56_TIMER2,                       /* Shared Peripheral Interrupt (SPI) ID 56 */
    AMBA_INT_SPI_ID57_TIMER3,                       /* Shared Peripheral Interrupt (SPI) ID 57 */
    AMBA_INT_SPI_ID58_TIMER4,                       /* Shared Peripheral Interrupt (SPI) ID 58 */
    AMBA_INT_SPI_ID59_TIMER5,                       /* Shared Peripheral Interrupt (SPI) ID 59 */
    AMBA_INT_SPI_ID60_TIMER6,                       /* Shared Peripheral Interrupt (SPI) ID 60 */
    AMBA_INT_SPI_ID61_TIMER7,                       /* Shared Peripheral Interrupt (SPI) ID 61 */
    AMBA_INT_SPI_ID62_TIMER8,                       /* Shared Peripheral Interrupt (SPI) ID 62 */
    AMBA_INT_SPI_ID63_TIMER9,                       /* Shared Peripheral Interrupt (SPI) ID 63 */

    AMBA_INT_SPI_ID64_TIMER10,                      /* Shared Peripheral Interrupt (SPI) ID 64 */
    AMBA_INT_SPI_ID65_TIMER11,                      /* Shared Peripheral Interrupt (SPI) ID 65 */
    AMBA_INT_SPI_ID66_TIMER12,                      /* Shared Peripheral Interrupt (SPI) ID 66 */
    AMBA_INT_SPI_ID67_TIMER13,                      /* Shared Peripheral Interrupt (SPI) ID 67 */
    AMBA_INT_SPI_ID68_TIMER14,                      /* Shared Peripheral Interrupt (SPI) ID 68 */
    AMBA_INT_SPI_ID69_TIMER15,                      /* Shared Peripheral Interrupt (SPI) ID 69 */
    AMBA_INT_SPI_ID70_TIMER16,                      /* Shared Peripheral Interrupt (SPI) ID 70 */
    AMBA_INT_SPI_ID71_TIMER17,                      /* Shared Peripheral Interrupt (SPI) ID 71 */
    AMBA_INT_SPI_ID72_TIMER18,                      /* Shared Peripheral Interrupt (SPI) ID 72 */
    AMBA_INT_SPI_ID73_TIMER19,                      /* Shared Peripheral Interrupt (SPI) ID 73 */
    AMBA_INT_SPI_ID74_ADC,                          /* Shared Peripheral Interrupt (SPI) ID 74 */
    AMBA_INT_SPI_ID75_WATCHDOG_TIMER,               /* Shared Peripheral Interrupt (SPI) ID 75 */
    AMBA_INT_SPI_ID76_GPIO0,                        /* Shared Peripheral Interrupt (SPI) ID 76 */
    AMBA_INT_SPI_ID77_GPIO1,                        /* Shared Peripheral Interrupt (SPI) ID 77 */
    AMBA_INT_SPI_ID78_GPIO2,                        /* Shared Peripheral Interrupt (SPI) ID 78 */
    AMBA_INT_SPI_ID79_GPIO3,                        /* Shared Peripheral Interrupt (SPI) ID 79 */
    AMBA_INT_SPI_ID80_GPIO4,                        /* Shared Peripheral Interrupt (SPI) ID 80 */
    AMBA_INT_SPI_ID81_IR_INTERFACE,                 /* Shared Peripheral Interrupt (SPI) ID 81 */
    AMBA_INT_SPI_ID82_HDMITX,                       /* Shared Peripheral Interrupt (SPI) ID 82 */
    AMBA_INT_SPI_ID83_USB1_CONNECT,                 /* Shared Peripheral Interrupt (SPI) ID 83 */
    AMBA_INT_SPI_ID84_USB0_CONNECT,                 /* Shared Peripheral Interrupt (SPI) ID 84 */
    AMBA_INT_SPI_ID85_USB0_CONNECT_CHANGE,          /* Shared Peripheral Interrupt (SPI) ID 85 */
    AMBA_INT_SPI_ID86_USBP_CHARGE_DETECT,           /* Shared Peripheral Interrupt (SPI) ID 86 */
    AMBA_INT_SPI_ID87_SDIO0_CARD_DETECT,            /* Shared Peripheral Interrupt (SPI) ID 87 */
    AMBA_INT_SPI_ID88_SD_CARD_DETECT,               /* Shared Peripheral Interrupt (SPI) ID 88 */
    AMBA_INT_SPI_ID89_ENET_PMT,                     /* Shared Peripheral Interrupt (SPI) ID 89 */
    AMBA_INT_SPI_ID90_ENET_SBD,                     /* Shared Peripheral Interrupt (SPI) ID 90 */
    AMBA_INT_SPI_ID91_ENET_PMT1,                    /* Shared Peripheral Interrupt (SPI) ID 91 */
    AMBA_INT_SPI_ID92_ENET_SBD1,                    /* Shared Peripheral Interrupt (SPI) ID 92 */
    AMBA_INT_SPI_ID93_UART_AHB0,                    /* Shared Peripheral Interrupt (SPI) ID 93 */
    AMBA_INT_SPI_ID94_UART_AHB1,                    /* Shared Peripheral Interrupt (SPI) ID 94 */
    AMBA_INT_SPI_ID95_UART_AHB2,                    /* Shared Peripheral Interrupt (SPI) ID 95 */

    AMBA_INT_SPI_ID96_UART_AHB3,                    /* Shared Peripheral Interrupt (SPI) ID 96 */
    AMBA_INT_SPI_ID97,                              /* Shared Peripheral Interrupt (SPI) ID 97 */
    AMBA_INT_SPI_ID98_USB1_CONNECT_CHANGE,          /* Shared Peripheral Interrupt (SPI) ID 98 */
    AMBA_INT_SPI_ID99_USB,                          /* Shared Peripheral Interrupt (SPI) ID 99 */
    AMBA_INT_SPI_ID100_FIO,                         /* Shared Peripheral Interrupt (SPI) ID 100 */
    AMBA_INT_SPI_ID101_FIO_ECC_RPT,                 /* Shared Peripheral Interrupt (SPI) ID 101 */
    AMBA_INT_SPI_ID102_GDMA,                        /* Shared Peripheral Interrupt (SPI) ID 102 */
    AMBA_INT_SPI_ID103_SDIO0,                       /* Shared Peripheral Interrupt (SPI) ID 103 */
    AMBA_INT_SPI_ID104_SD,                          /* Shared Peripheral Interrupt (SPI) ID 104 */
    AMBA_INT_SPI_ID105_SPI_NOR,                     /* Shared Peripheral Interrupt (SPI) ID 105 */
    AMBA_INT_SPI_ID106_SSI_MASTER0,                 /* Shared Peripheral Interrupt (SPI) ID 106 */
    AMBA_INT_SPI_ID107_SSI_MASTER1,                 /* Shared Peripheral Interrupt (SPI) ID 107 */
    AMBA_INT_SPI_ID108_SSI_MASTER2,                 /* Shared Peripheral Interrupt (SPI) ID 108 */
    AMBA_INT_SPI_ID109_SSI_MASTER3,                 /* Shared Peripheral Interrupt (SPI) ID 109 */
    AMBA_INT_SPI_ID110_SSI_MASTER4,                 /* Shared Peripheral Interrupt (SPI) ID 110 */
    AMBA_INT_SPI_ID111_SSI_MASTER5,                 /* Shared Peripheral Interrupt (SPI) ID 111 */
    AMBA_INT_SPI_ID112_SSI_SLAVE,                   /* Shared Peripheral Interrupt (SPI) ID 112 */
    AMBA_INT_SPI_ID113_I2S0_TX,                     /* Shared Peripheral Interrupt (SPI) ID 113 */
    AMBA_INT_SPI_ID114_I2S0_RX,                     /* Shared Peripheral Interrupt (SPI) ID 114 */
    AMBA_INT_SPI_ID115_I2S1_TX,                     /* Shared Peripheral Interrupt (SPI) ID 115 */
    AMBA_INT_SPI_ID116_I2S1_RX,                     /* Shared Peripheral Interrupt (SPI) ID 116 */
    AMBA_INT_SPI_ID117_DMA0,                        /* Shared Peripheral Interrupt (SPI) ID 117 */
    AMBA_INT_SPI_ID118_DMA1,                        /* Shared Peripheral Interrupt (SPI) ID 118 */
    AMBA_INT_SPI_ID119_VOUT_B_INT,                  /* Shared Peripheral Interrupt (SPI) ID 119 */
    AMBA_INT_SPI_ID120_VOUT_A_INT,                  /* Shared Peripheral Interrupt (SPI) ID 120 */
    AMBA_INT_SPI_ID121_RANDOM_NUM_GENERATOR,        /* Shared Peripheral Interrupt (SPI) ID 121 */
    AMBA_INT_SPI_ID122_AXI_SOFT_IRQ0,               /* Shared Peripheral Interrupt (SPI) ID 122 */
    AMBA_INT_SPI_ID123_AXI_SOFT_IRQ1,               /* Shared Peripheral Interrupt (SPI) ID 123 */
    AMBA_INT_SPI_ID124_AXI_SOFT_IRQ2,               /* Shared Peripheral Interrupt (SPI) ID 124 */
    AMBA_INT_SPI_ID125_AXI_SOFT_IRQ3,               /* Shared Peripheral Interrupt (SPI) ID 125 */
    AMBA_INT_SPI_ID126_AXI_SOFT_IRQ4,               /* Shared Peripheral Interrupt (SPI) ID 126 */
    AMBA_INT_SPI_ID127_AXI_SOFT_IRQ5,               /* Shared Peripheral Interrupt (SPI) ID 127 */

    AMBA_INT_SPI_ID128_AXI_SOFT_IRQ6,               /* Shared Peripheral Interrupt (SPI) ID 128 */
    AMBA_INT_SPI_ID129_AXI_SOFT_IRQ7,               /* Shared Peripheral Interrupt (SPI) ID 129 */
    AMBA_INT_SPI_ID130_AXI_SOFT_IRQ8,               /* Shared Peripheral Interrupt (SPI) ID 130 */
    AMBA_INT_SPI_ID131_AXI_SOFT_IRQ9,               /* Shared Peripheral Interrupt (SPI) ID 131 */
    AMBA_INT_SPI_ID132_AXI_SOFT_IRQ10,              /* Shared Peripheral Interrupt (SPI) ID 132 */
    AMBA_INT_SPI_ID133_AXI_SOFT_IRQ11,              /* Shared Peripheral Interrupt (SPI) ID 133 */
    AMBA_INT_SPI_ID134_AXI_SOFT_IRQ12,              /* Shared Peripheral Interrupt (SPI) ID 134 */
    AMBA_INT_SPI_ID135_AXI_SOFT_IRQ13,              /* Shared Peripheral Interrupt (SPI) ID 135 */
    AMBA_INT_SPI_ID136_DRAM_ERROR,                  /* Shared Peripheral Interrupt (SPI) ID 136 */
    AMBA_INT_SPI_ID137_VP0_EXCEPTION,               /* Shared Peripheral Interrupt (SPI) ID 137 */
    AMBA_INT_SPI_ID138_VIN4_VSYNC,                  /* Shared Peripheral Interrupt (SPI) ID 138 */
    AMBA_INT_SPI_ID139_VIN4_SOF,                    /* Shared Peripheral Interrupt (SPI) ID 139 */
    AMBA_INT_SPI_ID140_VIN4_MASTER_VSYNC,           /* Shared Peripheral Interrupt (SPI) ID 140 */
    AMBA_INT_SPI_ID141_VIN4_LAST_PIXEL,             /* Shared Peripheral Interrupt (SPI) ID 141 */
    AMBA_INT_SPI_ID142_VIN4_DELAYED_VSYNC,          /* Shared Peripheral Interrupt (SPI) ID 142 */
    AMBA_INT_SPI_ID143_VIN0_VSYNC,                  /* Shared Peripheral Interrupt (SPI) ID 143 */
    AMBA_INT_SPI_ID144_VIN0_SOF,                    /* Shared Peripheral Interrupt (SPI) ID 144 */
    AMBA_INT_SPI_ID145_VIN0_MASTER_VSYNC,           /* Shared Peripheral Interrupt (SPI) ID 145 */
    AMBA_INT_SPI_ID146_VIN0_LAST_PIXEL,             /* Shared Peripheral Interrupt (SPI) ID 146 */
    AMBA_INT_SPI_ID147_VIN0_DELAYED_VSYNC,          /* Shared Peripheral Interrupt (SPI) ID 147 */
    AMBA_INT_SPI_ID148_VIN1_VSYNC,                  /* Shared Peripheral Interrupt (SPI) ID 148 */
    AMBA_INT_SPI_ID149_VIN1_SOF,                    /* Shared Peripheral Interrupt (SPI) ID 149 */
    AMBA_INT_SPI_ID150_VIN1_LAST_PIXEL,             /* Shared Peripheral Interrupt (SPI) ID 150 */
    AMBA_INT_SPI_ID151_VIN1_DELAYED_VSYNC,          /* Shared Peripheral Interrupt (SPI) ID 151 */
    AMBA_INT_SPI_ID152_VIN2_VSYNC,                  /* Shared Peripheral Interrupt (SPI) ID 152 */
    AMBA_INT_SPI_ID153_VIN2_SOF,                    /* Shared Peripheral Interrupt (SPI) ID 153 */
    AMBA_INT_SPI_ID154_VIN2_LAST_PIXEL,             /* Shared Peripheral Interrupt (SPI) ID 154 */
    AMBA_INT_SPI_ID155_VIN2_DELAYED_VSYNC,          /* Shared Peripheral Interrupt (SPI) ID 155 */
    AMBA_INT_SPI_ID156_VIN3_VSYNC,                  /* Shared Peripheral Interrupt (SPI) ID 156 */
    AMBA_INT_SPI_ID157_VIN3_SOF,                    /* Shared Peripheral Interrupt (SPI) ID 157 */
    AMBA_INT_SPI_ID158_VIN3_LAST_PIXEL,             /* Shared Peripheral Interrupt (SPI) ID 158 */
    AMBA_INT_SPI_ID159_VIN3_DELAYED_VSYNC,          /* Shared Peripheral Interrupt (SPI) ID 159 */

    AMBA_INT_SPI_ID160_SDIO1_CD_CHANGE,             /* Shared Peripheral Interrupt (SPI) ID 160 */
    AMBA_INT_SPI_ID161_SDIO1,                       /* Shared Peripheral Interrupt (SPI) ID 161 */
    AMBA_INT_SPI_ID162_VIN5_VSYNC,                  /* Shared Peripheral Interrupt (SPI) ID 162 */
    AMBA_INT_SPI_ID163_VIN5_SOF,                    /* Shared Peripheral Interrupt (SPI) ID 163 */
    AMBA_INT_SPI_ID164_VIN5_LAST_PIXEL,             /* Shared Peripheral Interrupt (SPI) ID 164 */
    AMBA_INT_SPI_ID165_VIN5_DELAYED_VSYNC,          /* Shared Peripheral Interrupt (SPI) ID 165 */
    AMBA_INT_SPI_ID166_VIN6_VSYNC,                  /* Shared Peripheral Interrupt (SPI) ID 166 */
    AMBA_INT_SPI_ID167_VIN6_SOF,                    /* Shared Peripheral Interrupt (SPI) ID 167 */
    AMBA_INT_SPI_ID168_VIN6_LAST_PIXEL,             /* Shared Peripheral Interrupt (SPI) ID 168 */
    AMBA_INT_SPI_ID169_VIN6_DELAYED_VSYNC,          /* Shared Peripheral Interrupt (SPI) ID 169 */
    AMBA_INT_SPI_ID170_VIN7_VSYNC,                  /* Shared Peripheral Interrupt (SPI) ID 170 */
    AMBA_INT_SPI_ID171_VIN7_SOF,                    /* Shared Peripheral Interrupt (SPI) ID 171 */
    AMBA_INT_SPI_ID172_VIN7_LAST_PIXEL,             /* Shared Peripheral Interrupt (SPI) ID 172 */
    AMBA_INT_SPI_ID173_VIN7_DELAYED_VSYNC,          /* Shared Peripheral Interrupt (SPI) ID 173 */
    AMBA_INT_SPI_ID174_CODE_VDSP0_IRQ0,             /* Shared Peripheral Interrupt (SPI) ID 174 */
    AMBA_INT_SPI_ID175_CODE_VDSP0_IRQ1,             /* Shared Peripheral Interrupt (SPI) ID 175 */
    AMBA_INT_SPI_ID176_CODE_VDSP0_IRQ2,             /* Shared Peripheral Interrupt (SPI) ID 176 */
    AMBA_INT_SPI_ID177_CODE_VDSP0_IRQ3,             /* Shared Peripheral Interrupt (SPI) ID 177 */
    AMBA_INT_SPI_ID178_CODE_VDSP1_IRQ0,             /* Shared Peripheral Interrupt (SPI) ID 178 */
    AMBA_INT_SPI_ID179_CODE_VDSP1_IRQ1,             /* Shared Peripheral Interrupt (SPI) ID 179 */
    AMBA_INT_SPI_ID180_CODE_VDSP1_IRQ2,             /* Shared Peripheral Interrupt (SPI) ID 180 */
    AMBA_INT_SPI_ID181_CODE_VDSP1_IRQ3,             /* Shared Peripheral Interrupt (SPI) ID 181 */
    AMBA_INT_SPI_ID182_CODE_VDSP2_IRQ0,             /* Shared Peripheral Interrupt (SPI) ID 182 */
    AMBA_INT_SPI_ID183_CODE_VDSP2_IRQ1,             /* Shared Peripheral Interrupt (SPI) ID 183 */
    AMBA_INT_SPI_ID184_CODE_VDSP2_IRQ2,             /* Shared Peripheral Interrupt (SPI) ID 184 */
    AMBA_INT_SPI_ID185_CODE_VDSP2_IRQ3,             /* Shared Peripheral Interrupt (SPI) ID 185 */
    AMBA_INT_SPI_ID186_CODE_VDSP3_IRQ0,             /* Shared Peripheral Interrupt (SPI) ID 186 */
    AMBA_INT_SPI_ID187_CODE_VDSP3_IRQ1,             /* Shared Peripheral Interrupt (SPI) ID 187 */
    AMBA_INT_SPI_ID188_CODE_VDSP3_IRQ2,             /* Shared Peripheral Interrupt (SPI) ID 188 */
    AMBA_INT_SPI_ID189_CODE_VDSP3_IRQ3,             /* Shared Peripheral Interrupt (SPI) ID 189 */
    AMBA_INT_SPI_ID190_CODE_VIN0_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 190 */
    AMBA_INT_SPI_ID191_CODE_VIN1_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 191 */

    AMBA_INT_SPI_ID192_CODE_VIN2_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 192 */
    AMBA_INT_SPI_ID193_CODE_VIN3_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 193 */
    AMBA_INT_SPI_ID194_CODE_VIN4_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 194 */
    AMBA_INT_SPI_ID195_CODE_VIN5_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 195 */
    AMBA_INT_SPI_ID196_CODE_VIN6_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 196 */
    AMBA_INT_SPI_ID197_CODE_VIN7_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 197 */
    AMBA_INT_SPI_ID198_CODE_VOUTA_IRQ,              /* Shared Peripheral Interrupt (SPI) ID 198 */
    AMBA_INT_SPI_ID199_CODE_VOUTB_IRQ,              /* Shared Peripheral Interrupt (SPI) ID 199 */
    AMBA_INT_SPI_ID200_I2C_ARM_ERROR,               /* Shared Peripheral Interrupt (SPI) ID 200 */
    AMBA_INT_SPI_ID201_VORC_THREAD0_IRQ,            /* Shared Peripheral Interrupt (SPI) ID 201 */
    AMBA_INT_SPI_ID202_VORC_THREAD1_IRQ,            /* Shared Peripheral Interrupt (SPI) ID 202 */
    AMBA_INT_SPI_ID203_VORC_THREAD2_IRQ,            /* Shared Peripheral Interrupt (SPI) ID 203 */
    AMBA_INT_SPI_ID204_VORC_THREAD3_IRQ,            /* Shared Peripheral Interrupt (SPI) ID 204 */
    AMBA_INT_SPI_ID205_VORC_THREAD4_IRQ,            /* Shared Peripheral Interrupt (SPI) ID 205 */
    AMBA_INT_SPI_ID206_DDR_HOST0,                   /* Shared Peripheral Interrupt (SPI) ID 206 */
    AMBA_INT_SPI_ID207_DDR_HOST1,                   /* Shared Peripheral Interrupt (SPI) ID 207 */
    AMBA_INT_SPI_ID208_VIN8_VSYNC,                  /* Shared Peripheral Interrupt (SPI) ID 208 */
    AMBA_INT_SPI_ID209_VIN8_SOF,                    /* Shared Peripheral Interrupt (SPI) ID 209 */
    AMBA_INT_SPI_ID210_VIN8_LAST_PIXEL,             /* Shared Peripheral Interrupt (SPI) ID 210 */
    AMBA_INT_SPI_ID211_VIN8_DELAYED_VSYNC,          /* Shared Peripheral Interrupt (SPI) ID 211 */
    AMBA_INT_SPI_ID212_VIN9_VSYNC,                  /* Shared Peripheral Interrupt (SPI) ID 212 */
    AMBA_INT_SPI_ID213_VIN9_SOF,                    /* Shared Peripheral Interrupt (SPI) ID 213 */
    AMBA_INT_SPI_ID214_VIN9_LAST_PIXEL,             /* Shared Peripheral Interrupt (SPI) ID 214 */
    AMBA_INT_SPI_ID215_VIN9_DELAYED_VSYNC,          /* Shared Peripheral Interrupt (SPI) ID 215 */
    AMBA_INT_SPI_ID216_VIN10_VSYNC,                 /* Shared Peripheral Interrupt (SPI) ID 216 */
    AMBA_INT_SPI_ID217_VIN10_SOF,                   /* Shared Peripheral Interrupt (SPI) ID 217 */
    AMBA_INT_SPI_ID218_VIN10_LAST_PIXEL,            /* Shared Peripheral Interrupt (SPI) ID 218 */
    AMBA_INT_SPI_ID219_VIN10_DELAYED_VSYNC,         /* Shared Peripheral Interrupt (SPI) ID 219 */
    AMBA_INT_SPI_ID220_VIN11_VSYNC,                 /* Shared Peripheral Interrupt (SPI) ID 220 */
    AMBA_INT_SPI_ID221_VIN11_SOF,                   /* Shared Peripheral Interrupt (SPI) ID 221 */
    AMBA_INT_SPI_ID222_VIN11_LAST_PIXEL,            /* Shared Peripheral Interrupt (SPI) ID 222 */
    AMBA_INT_SPI_ID223_VIN11_DELAYED_VSYNC,         /* Shared Peripheral Interrupt (SPI) ID 223 */

    AMBA_INT_SPI_ID224_VIN12_VSYNC,                 /* Shared Peripheral Interrupt (SPI) ID 224 */
    AMBA_INT_SPI_ID225_VIN12_SOF,                   /* Shared Peripheral Interrupt (SPI) ID 225 */
    AMBA_INT_SPI_ID226_VIN12_LAST_PIXEL,            /* Shared Peripheral Interrupt (SPI) ID 226 */
    AMBA_INT_SPI_ID227_VIN12_DELAYED_VSYNC,         /* Shared Peripheral Interrupt (SPI) ID 227 */
    AMBA_INT_SPI_ID228_VIN13_VSYNC,                 /* Shared Peripheral Interrupt (SPI) ID 228 */
    AMBA_INT_SPI_ID229_VIN13_SOF,                   /* Shared Peripheral Interrupt (SPI) ID 229 */
    AMBA_INT_SPI_ID230_VIN13_LAST_PIXEL,            /* Shared Peripheral Interrupt (SPI) ID 230 */
    AMBA_INT_SPI_ID231_VIN13_DELAYED_VSYNC,         /* Shared Peripheral Interrupt (SPI) ID 231 */
    AMBA_INT_SPI_ID232_IDSP0_ARM_SAFETY,            /* Shared Peripheral Interrupt (SPI) ID 232 */
    AMBA_INT_SPI_ID233_IDSP1_ARM_SAFETY,            /* Shared Peripheral Interrupt (SPI) ID 233 */
    AMBA_INT_SPI_ID234_IDSPV_ARM_SAFETY,            /* Shared Peripheral Interrupt (SPI) ID 234 */
    AMBA_INT_SPI_ID235_CODE_VIN8_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 235 */
    AMBA_INT_SPI_ID236_CODE_VIN9_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 236 */
    AMBA_INT_SPI_ID237_CODE_VIN10_IRQ,              /* Shared Peripheral Interrupt (SPI) ID 237 */
    AMBA_INT_SPI_ID238_CODE_VIN11_IRQ,              /* Shared Peripheral Interrupt (SPI) ID 238 */
    AMBA_INT_SPI_ID239_CODE_VIN12_IRQ,              /* Shared Peripheral Interrupt (SPI) ID 239 */
    AMBA_INT_SPI_ID240_CODE_VIN13_IRQ,              /* Shared Peripheral Interrupt (SPI) ID 240 */
    AMBA_INT_SPI_ID241_VOUT_C_INT,                  /* Shared Peripheral Interrupt (SPI) ID 241 */
    AMBA_INT_SPI_ID242_SDIO0_WAKEUP,                /* Shared Peripheral Interrupt (SPI) ID 242 */
    AMBA_INT_SPI_ID243_SD_WAKEUP,                   /* Shared Peripheral Interrupt (SPI) ID 243 */
    AMBA_INT_SPI_ID244_PCIE_PHY_IN0,                /* Shared Peripheral Interrupt (SPI) ID 244 */
    AMBA_INT_SPI_ID245_PCIE_PHY_IN1,                /* Shared Peripheral Interrupt (SPI) ID 245 */
    AMBA_INT_SPI_ID246_PCIE_PHY_IN2,                /* Shared Peripheral Interrupt (SPI) ID 246 */
    AMBA_INT_SPI_ID247_PCIE_PHY_IN3,                /* Shared Peripheral Interrupt (SPI) ID 247 */
    AMBA_INT_SPI_ID248_USB32_PHY_IN0,               /* Shared Peripheral Interrupt (SPI) ID 248 */
    AMBA_INT_SPI_ID249_FAULT_IRQ0,                  /* Shared Peripheral Interrupt (SPI) ID 249 */
    AMBA_INT_SPI_ID250_FAULT_IRQ1,                  /* Shared Peripheral Interrupt (SPI) ID 250 */
    AMBA_INT_SPI_ID251_FAULT_IRQ2,                  /* Shared Peripheral Interrupt (SPI) ID 251 */
    AMBA_INT_SPI_ID252_ERR_IRQ0,                    /* Shared Peripheral Interrupt (SPI) ID 252 */
    AMBA_INT_SPI_ID253_ERR_IRQ1,                    /* Shared Peripheral Interrupt (SPI) ID 253 */
    AMBA_INT_SPI_ID254_ERR_IRQ2,                    /* Shared Peripheral Interrupt (SPI) ID 254 */
    AMBA_INT_SPI_ID255_USB32C,                      /* Shared Peripheral Interrupt (SPI) ID 255 */

    AMBA_INT_SPI_ID256_USB32C_OTG,                  /* Shared Peripheral Interrupt (SPI) ID 256 */
    AMBA_INT_SPI_ID257_USB32C_HOST_SYSYEM_ERROR,    /* Shared Peripheral Interrupt (SPI) ID 257 */
    AMBA_INT_SPI_ID258_USB32C_ITP_EXTENDED,         /* Shared Peripheral Interrupt (SPI) ID 258 */
    AMBA_INT_SPI_ID259_PCIEC_DMA,                   /* Shared Peripheral Interrupt (SPI) ID 259 */
    AMBA_INT_SPI_ID260_PCIEC_LOCAL,                 /* Shared Peripheral Interrupt (SPI) ID 260 */
    AMBA_INT_SPI_ID261_PCIEC_INTA_OUT,              /* Shared Peripheral Interrupt (SPI) ID 261 */
    AMBA_INT_SPI_ID262_PCIEC_INTB_OUT,              /* Shared Peripheral Interrupt (SPI) ID 262 */
    AMBA_INT_SPI_ID263_PCIEC_INTC_OUT,              /* Shared Peripheral Interrupt (SPI) ID 263 */
    AMBA_INT_SPI_ID264_PCIEC_INTD_OUT,              /* Shared Peripheral Interrupt (SPI) ID 264 */
    AMBA_INT_SPI_ID265_PCIEC_PHY_OUT,               /* Shared Peripheral Interrupt (SPI) ID 265 */
    AMBA_INT_SPI_ID266_PCIEC_INT_ACK_EXTENDED,      /* Shared Peripheral Interrupt (SPI) ID 266 */
    AMBA_INT_SPI_ID267_PCIEC_POWER_STATE_CHANGE,    /* Shared Peripheral Interrupt (SPI) ID 267 */
    AMBA_INT_SPI_ID268_PCIEC_HOT_RESET,             /* Shared Peripheral Interrupt (SPI) ID 268 */
    AMBA_INT_SPI_ID269_PCIEC_LINK_DOWN_RESET,       /* Shared Peripheral Interrupt (SPI) ID 269 */
    AMBA_INT_SPI_ID270_PCIEC_CORRECTABLE_ERROR,     /* Shared Peripheral Interrupt (SPI) ID 270 */
    AMBA_INT_SPI_ID271_PCIEC_NON_FATAL_ERROR,       /* Shared Peripheral Interrupt (SPI) ID 271 */
    AMBA_INT_SPI_ID272_PCIEC_FATAL_ERROR,           /* Shared Peripheral Interrupt (SPI) ID 272 */
    AMBA_INT_SPI_ID273_CODE_VOUTC_IRQ,              /* Shared Peripheral Interrupt (SPI) ID 273 */
    AMBA_INT_SPI_ID274,                             /* Shared Peripheral Interrupt (SPI) ID 274 */
    AMBA_INT_SPI_ID275,                             /* Shared Peripheral Interrupt (SPI) ID 275 */
    AMBA_INT_SPI_ID276,                             /* Shared Peripheral Interrupt (SPI) ID 276 */
    AMBA_INT_SPI_ID277,                             /* Shared Peripheral Interrupt (SPI) ID 277 */
    AMBA_INT_SPI_ID278,                             /* Shared Peripheral Interrupt (SPI) ID 278 */
    AMBA_INT_SPI_ID279,                             /* Shared Peripheral Interrupt (SPI) ID 279 */
    AMBA_INT_SPI_ID280,                             /* Shared Peripheral Interrupt (SPI) ID 280 */
    AMBA_INT_SPI_ID281,                             /* Shared Peripheral Interrupt (SPI) ID 281 */
    AMBA_INT_SPI_ID282,                             /* Shared Peripheral Interrupt (SPI) ID 282 */
    AMBA_INT_SPI_ID283,                             /* Shared Peripheral Interrupt (SPI) ID 283 */
    AMBA_INT_SPI_ID284,                             /* Shared Peripheral Interrupt (SPI) ID 284 */
    AMBA_INT_SPI_ID285,                             /* Shared Peripheral Interrupt (SPI) ID 285 */
    AMBA_INT_SPI_ID286,                             /* Shared Peripheral Interrupt (SPI) ID 286 */
    AMBA_INT_SPI_ID287,                             /* Shared Peripheral Interrupt (SPI) ID 287 */

    AMBA_NUM_INTERRUPT                              /* Total number of Interrupts supported */
};

#define AMBA_INTERRUPT_NUM 288

#endif /* AMBA_INT_PRIV_H */
