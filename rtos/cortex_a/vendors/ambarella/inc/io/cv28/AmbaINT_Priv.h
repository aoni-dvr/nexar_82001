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
    AMBA_INT_SPI_ID34_CORTEX0_EXTERRIRQ,            /* Shared Peripheral Interrupt (SPI) ID 34 */
    AMBA_INT_SPI_ID35,                              /* Shared Peripheral Interrupt (SPI) ID 35 */
    AMBA_INT_SPI_ID36_CORTEX0_PMUIRQ0,              /* Shared Peripheral Interrupt (SPI) ID 36 */
    AMBA_INT_SPI_ID37_CORTEX0_PMUIRQ1,              /* Shared Peripheral Interrupt (SPI) ID 37 */
    AMBA_INT_SPI_ID38,                              /* Shared Peripheral Interrupt (SPI) ID 38 */
    AMBA_INT_SPI_ID39,                              /* Shared Peripheral Interrupt (SPI) ID 39 */
    AMBA_INT_SPI_ID40,                              /* Shared Peripheral Interrupt (SPI) ID 40 */
    AMBA_INT_SPI_ID41,                              /* Shared Peripheral Interrupt (SPI) ID 41 */
    AMBA_INT_SPI_ID42,                              /* Shared Peripheral Interrupt (SPI) ID 42 */
    AMBA_INT_SPI_ID43,                              /* Shared Peripheral Interrupt (SPI) ID 43 */
    AMBA_INT_SPI_ID44_CORTEX0_INTERRIRQ,            /* Shared Peripheral Interrupt (SPI) ID 44 */
    AMBA_INT_SPI_ID45_ADC,                          /* Shared Peripheral Interrupt (SPI) ID 45 */
    AMBA_INT_SPI_ID46,                              /* Shared Peripheral Interrupt (SPI) ID 46 */
    AMBA_INT_SPI_ID47_I2C_MASTER0,                  /* Shared Peripheral Interrupt (SPI) ID 47 */
    AMBA_INT_SPI_ID48_I2C_MASTER1,                  /* Shared Peripheral Interrupt (SPI) ID 48 */
    AMBA_INT_SPI_ID49_I2C_MASTER2,                  /* Shared Peripheral Interrupt (SPI) ID 49 */
    AMBA_INT_SPI_ID50_I2C_MASTER3,                  /* Shared Peripheral Interrupt (SPI) ID 50 */
    AMBA_INT_SPI_ID51_I2C_SLAVE,                    /* Shared Peripheral Interrupt (SPI) ID 51 */
    AMBA_INT_SPI_ID52_IR_INTERFACE,                 /* Shared Peripheral Interrupt (SPI) ID 52 */
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
    AMBA_INT_SPI_ID74_WATCHDOG_TIMER,               /* Shared Peripheral Interrupt (SPI) ID 74 */
    AMBA_INT_SPI_ID75_GPIO0,                        /* Shared Peripheral Interrupt (SPI) ID 75 */
    AMBA_INT_SPI_ID76_GPIO1,                        /* Shared Peripheral Interrupt (SPI) ID 76 */
    AMBA_INT_SPI_ID77_GPIO2,                        /* Shared Peripheral Interrupt (SPI) ID 77 */
    AMBA_INT_SPI_ID78,                              /* Shared Peripheral Interrupt (SPI) ID 78 */
    AMBA_INT_SPI_ID79,                              /* Shared Peripheral Interrupt (SPI) ID 79 */
    AMBA_INT_SPI_ID80,                              /* Shared Peripheral Interrupt (SPI) ID 80 */
    AMBA_INT_SPI_ID81,                              /* Shared Peripheral Interrupt (SPI) ID 81 */
    AMBA_INT_SPI_ID82_SDIO1_CARD_DETECT,            /* Shared Peripheral Interrupt (SPI) ID 82 */
    AMBA_INT_SPI_ID83_USB_ID_CHANGE,                /* Shared Peripheral Interrupt (SPI) ID 83 */
    AMBA_INT_SPI_ID84_USB_CONNECT,                  /* Shared Peripheral Interrupt (SPI) ID 84 */
    AMBA_INT_SPI_ID85_USB_CONNECT_CHANGE,           /* Shared Peripheral Interrupt (SPI) ID 85 */
    AMBA_INT_SPI_ID86_USB_CHARGE_DETECT,            /* Shared Peripheral Interrupt (SPI) ID 86 */
    AMBA_INT_SPI_ID87_SDIO0_CARD_DETECT,            /* Shared Peripheral Interrupt (SPI) ID 87 */
    AMBA_INT_SPI_ID88_SD_CARD_DETECT,               /* Shared Peripheral Interrupt (SPI) ID 88 */
    AMBA_INT_SPI_ID89_ENET_PMT,                     /* Shared Peripheral Interrupt (SPI) ID 89 */
    AMBA_INT_SPI_ID90_ENET_SBD,                     /* Shared Peripheral Interrupt (SPI) ID 90 */
    AMBA_INT_SPI_ID91_UART_AHB0,                    /* Shared Peripheral Interrupt (SPI) ID 91 */
    AMBA_INT_SPI_ID92_UART_AHB1,                    /* Shared Peripheral Interrupt (SPI) ID 92 */
    AMBA_INT_SPI_ID93_UART_AHB2,                    /* Shared Peripheral Interrupt (SPI) ID 93 */
    AMBA_INT_SPI_ID94_UART_AHB3,                    /* Shared Peripheral Interrupt (SPI) ID 94 */
    AMBA_INT_SPI_ID95,                              /* Shared Peripheral Interrupt (SPI) ID 95 */

    AMBA_INT_SPI_ID96_SDIO1,                        /* Shared Peripheral Interrupt (SPI) ID 96 */
    AMBA_INT_SPI_ID97_USB_HOST_EHCI,                /* Shared Peripheral Interrupt (SPI) ID 97 */
    AMBA_INT_SPI_ID98_USB_HOST_OHCI,                /* Shared Peripheral Interrupt (SPI) ID 98 */
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
    AMBA_INT_SPI_ID110,                             /* Shared Peripheral Interrupt (SPI) ID 110 */
    AMBA_INT_SPI_ID111,                             /* Shared Peripheral Interrupt (SPI) ID 111 */
    AMBA_INT_SPI_ID112_SSI_SLAVE,                   /* Shared Peripheral Interrupt (SPI) ID 112 */
    AMBA_INT_SPI_ID113_I2S_TX,                      /* Shared Peripheral Interrupt (SPI) ID 113 */
    AMBA_INT_SPI_ID114_I2S_RX,                      /* Shared Peripheral Interrupt (SPI) ID 114 */
    AMBA_INT_SPI_ID115_DMA0,                        /* Shared Peripheral Interrupt (SPI) ID 115 */
    AMBA_INT_SPI_ID116_DMA1,                        /* Shared Peripheral Interrupt (SPI) ID 116 */
    AMBA_INT_SPI_ID117,                             /* Shared Peripheral Interrupt (SPI) ID 117 */
    AMBA_INT_SPI_ID118_CANC,                        /* Shared Peripheral Interrupt (SPI) ID 118 */
    AMBA_INT_SPI_ID119_I2S1_TX,                     /* Shared Peripheral Interrupt (SPI) ID 119 */
    AMBA_INT_SPI_ID120_I2S1_RX,                     /* Shared Peripheral Interrupt (SPI) ID 120 */
    AMBA_INT_SPI_ID121_VOUT_A_INT,                  /* Shared Peripheral Interrupt (SPI) ID 121 */
    AMBA_INT_SPI_ID122_RANDOM_NUM_GENERATOR,        /* Shared Peripheral Interrupt (SPI) ID 122 */
    AMBA_INT_SPI_ID123_AXI_SOFT_IRQ0,               /* Shared Peripheral Interrupt (SPI) ID 123 */
    AMBA_INT_SPI_ID124_AXI_SOFT_IRQ1,               /* Shared Peripheral Interrupt (SPI) ID 124 */
    AMBA_INT_SPI_ID125_AXI_SOFT_IRQ2,               /* Shared Peripheral Interrupt (SPI) ID 125 */
    AMBA_INT_SPI_ID126_AXI_SOFT_IRQ3,               /* Shared Peripheral Interrupt (SPI) ID 126 */
    AMBA_INT_SPI_ID127_AXI_SOFT_IRQ4,               /* Shared Peripheral Interrupt (SPI) ID 127 */
    AMBA_INT_SPI_ID128_AXI_SOFT_IRQ5,               /* Shared Peripheral Interrupt (SPI) ID 128 */
    AMBA_INT_SPI_ID129_AXI_SOFT_IRQ6,               /* Shared Peripheral Interrupt (SPI) ID 129 */
    AMBA_INT_SPI_ID130_AXI_SOFT_IRQ7,               /* Shared Peripheral Interrupt (SPI) ID 130 */
    AMBA_INT_SPI_ID131_AXI_SOFT_IRQ8,               /* Shared Peripheral Interrupt (SPI) ID 131 */
    AMBA_INT_SPI_ID132_AXI_SOFT_IRQ9,               /* Shared Peripheral Interrupt (SPI) ID 132 */
    AMBA_INT_SPI_ID133_AXI_SOFT_IRQ10,              /* Shared Peripheral Interrupt (SPI) ID 133 */
    AMBA_INT_SPI_ID134_AXI_SOFT_IRQ11,              /* Shared Peripheral Interrupt (SPI) ID 134 */
    AMBA_INT_SPI_ID135_AXI_SOFT_IRQ12,              /* Shared Peripheral Interrupt (SPI) ID 135 */
    AMBA_INT_SPI_ID136_AXI_SOFT_IRQ13,              /* Shared Peripheral Interrupt (SPI) ID 136 */
    AMBA_INT_SPI_ID137_DRAM_ERROR,                  /* Shared Peripheral Interrupt (SPI) ID 137 */
    AMBA_INT_SPI_ID138_VP0_EXCEPTION,               /* Shared Peripheral Interrupt (SPI) ID 138 */
    AMBA_INT_SPI_ID139,                             /* Shared Peripheral Interrupt (SPI) ID 139 */
    AMBA_INT_SPI_ID140_ROLLING_SHUTTER,             /* Shared Peripheral Interrupt (SPI) ID 140 */
    AMBA_INT_SPI_ID141_VIN_MASTER_VSYNC,            /* Shared Peripheral Interrupt (SPI) ID 141 */
    AMBA_INT_SPI_ID142_VIN_SLAVE_VSYNC,             /* Shared Peripheral Interrupt (SPI) ID 142 */
    AMBA_INT_SPI_ID143_VIN_SOF,                     /* Shared Peripheral Interrupt (SPI) ID 143 */
    AMBA_INT_SPI_ID144_VIN_LAST_PIXEL,              /* Shared Peripheral Interrupt (SPI) ID 144 */
    AMBA_INT_SPI_ID145_VIN_DELAYED_VSYNC,           /* Shared Peripheral Interrupt (SPI) ID 145 */
    AMBA_INT_SPI_ID146_PIP2_SLAVE_VSYNC,            /* Shared Peripheral Interrupt (SPI) ID 146 */
    AMBA_INT_SPI_ID147_PIP2_SOF,                    /* Shared Peripheral Interrupt (SPI) ID 147 */
    AMBA_INT_SPI_ID148_PIP2_LAST_PIXEL,             /* Shared Peripheral Interrupt (SPI) ID 148 */
    AMBA_INT_SPI_ID149_PIP2_DELAYED_VSYNC,          /* Shared Peripheral Interrupt (SPI) ID 149 */
    AMBA_INT_SPI_ID150,                             /* Shared Peripheral Interrupt (SPI) ID 150 */
    AMBA_INT_SPI_ID151,                             /* Shared Peripheral Interrupt (SPI) ID 151 */
    AMBA_INT_SPI_ID152,                             /* Shared Peripheral Interrupt (SPI) ID 152 */
    AMBA_INT_SPI_ID153,                             /* Shared Peripheral Interrupt (SPI) ID 153 */
    AMBA_INT_SPI_ID154,                             /* Shared Peripheral Interrupt (SPI) ID 154 */
    AMBA_INT_SPI_ID155,                             /* Shared Peripheral Interrupt (SPI) ID 155 */
    AMBA_INT_SPI_ID156,                             /* Shared Peripheral Interrupt (SPI) ID 156 */
    AMBA_INT_SPI_ID157,                             /* Shared Peripheral Interrupt (SPI) ID 157 */
    AMBA_INT_SPI_ID158,                             /* Shared Peripheral Interrupt (SPI) ID 158 */
    AMBA_INT_SPI_ID159,                             /* Shared Peripheral Interrupt (SPI) ID 159 */
    AMBA_INT_SPI_ID160,                             /* Shared Peripheral Interrupt (SPI) ID 160 */
    AMBA_INT_SPI_ID161,                             /* Shared Peripheral Interrupt (SPI) ID 161 */
    AMBA_INT_SPI_ID162,                             /* Shared Peripheral Interrupt (SPI) ID 162 */
    AMBA_INT_SPI_ID163,                             /* Shared Peripheral Interrupt (SPI) ID 163 */
    AMBA_INT_SPI_ID164,                             /* Shared Peripheral Interrupt (SPI) ID 164 */
    AMBA_INT_SPI_ID165,                             /* Shared Peripheral Interrupt (SPI) ID 165 */
    AMBA_INT_SPI_ID166_PIP_MASTER_VSYNC,            /* Shared Peripheral Interrupt (SPI) ID 166 */
    AMBA_INT_SPI_ID167_PIP_SLAVE_VSYNC,             /* Shared Peripheral Interrupt (SPI) ID 167 */
    AMBA_INT_SPI_ID168_PIP_SOF,                     /* Shared Peripheral Interrupt (SPI) ID 168 */
    AMBA_INT_SPI_ID169_PIP_LAST_PIXEL,              /* Shared Peripheral Interrupt (SPI) ID 169 */
    AMBA_INT_SPI_ID170_PIP_DELAYED_VSYNC,           /* Shared Peripheral Interrupt (SPI) ID 170 */
    AMBA_INT_SPI_ID171_SMEM_ERROR,                  /* Shared Peripheral Interrupt (SPI) ID 171 */
    AMBA_INT_SPI_ID172_CODE_VIN_IRQ,                /* Shared Peripheral Interrupt (SPI) ID 172 */
    AMBA_INT_SPI_ID173_CODE_VDSP0_IRQ,              /* Shared Peripheral Interrupt (SPI) ID 173 */
    AMBA_INT_SPI_ID174_CODE_VDSP1_IRQ,              /* Shared Peripheral Interrupt (SPI) ID 174 */
    AMBA_INT_SPI_ID175_CODE_VDSP2_IRQ,              /* Shared Peripheral Interrupt (SPI) ID 175 */
    AMBA_INT_SPI_ID176_CODE_VDSP3_IRQ,              /* Shared Peripheral Interrupt (SPI) ID 176 */
    AMBA_INT_SPI_ID177_CODE_VOUT0_IRQ,              /* Shared Peripheral Interrupt (SPI) ID 177 */
    AMBA_INT_SPI_ID178_CODE_VOUT1_IRQ,              /* Shared Peripheral Interrupt (SPI) ID 178 */
    AMBA_INT_SPI_ID179_CODE_PIP_IRQ,                /* Shared Peripheral Interrupt (SPI) ID 179 */
    AMBA_INT_SPI_ID180_CODE_PIP2_IRQ,               /* Shared Peripheral Interrupt (SPI) ID 180 */
    AMBA_INT_SPI_ID181,                             /* Shared Peripheral Interrupt (SPI) ID 181 */
    AMBA_INT_SPI_ID182,                             /* Shared Peripheral Interrupt (SPI) ID 182 */
    AMBA_INT_SPI_ID183,                             /* Shared Peripheral Interrupt (SPI) ID 183 */
    AMBA_INT_SPI_ID184_VORC_L2C_EVENT_IRQ,          /* Shared Peripheral Interrupt (SPI) ID 184 */
    AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ,            /* Shared Peripheral Interrupt (SPI) ID 185 */
    AMBA_INT_SPI_ID186_VORC_THREAD1_IRQ,            /* Shared Peripheral Interrupt (SPI) ID 186 */
    AMBA_INT_SPI_ID187_VORC_THREAD2_IRQ,            /* Shared Peripheral Interrupt (SPI) ID 187 */
    AMBA_INT_SPI_ID188,                             /* Shared Peripheral Interrupt (SPI) ID 188 */
    AMBA_INT_SPI_ID189,                             /* Shared Peripheral Interrupt (SPI) ID 189 */
    AMBA_INT_SPI_ID190,                             /* Shared Peripheral Interrupt (SPI) ID 190 */
    AMBA_INT_SPI_ID191,                             /* Shared Peripheral Interrupt (SPI) ID 191 */
    AMBA_INT_SPI_ID192,                             /* Shared Peripheral Interrupt (SPI) ID 192 */

    AMBA_NUM_INTERRUPT                              /* Total number of Interrupts supported */
};

#define AMBA_INTERRUPT_NUM 193

#endif /* AMBA_INT_PRIV_H */
