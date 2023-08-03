/**
 *  @file bst_regs.h
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
 *  @details Constants & Definitions for Bootstrap
 *
 */
#define DRAMC_BASE            0xDFFE0000
#define AHB_N_BASE            0xE0000000
#define APB_N_BASE            0xE4000000
#define AHB_S_BASE            0xE8000000
#define APB_S_BASE            0xEC000000
#define RCT_BASE            0xED080000
#define PWC_DBG_BASE            0xED0D0000

/* Dram controller and config address */
#define DRAMC_DRAM_BASE            (DRAMC_BASE + 0x000)
#define DRAMC_DDRC_BASE            (DRAMC_BASE + 0x800)

/* FIO/NAND controller base address */
#define FIO_BASE            (AHB_N_BASE + 0x2000)

/* SD0 controller base address */
#define SD0_BASE            (AHB_N_BASE + 0x4000)

/* SPINOR controller base address */
#define SPINOR_BASE            (AHB_N_BASE + 0x1000)

/* Non-Secure Scratchpad base address */
#define N_SCRATCHPAD_BASE        (AHB_N_BASE + 0x22000)

/* Secure Scratchpad base address */
#define S_SCRATCHPAD_BASE        (AHB_S_BASE + 0x1000)

/* Secure Scratchpad RAM base address */
#define AHB_SP0_RAM_BASE        (AHB_S_BASE + 0x20000)
#define AHB_SP1_RAM_BASE        (AHB_S_BASE + 0x21000)


/* Register Offset in DRAM Uarch */
#define REG_DRAM_SECMEM_CTRL        0x218
#define REG_DRAM_SEC_KEY0        0x230    /* 0x230~0x240, 160 bits in total */
#define REG_DRAM_SEC_KEY1        0x234
#define REG_DRAM_SEC_KEY2        0x238
#define REG_DRAM_SEC_KEY3        0x23c
#define REG_DRAM_SEC_KEY4        0x240

/* Register Offset in Non-Secure Srcatchpad */
#define AHBSP_DATA0_OFFSET        0x6C    /* non-secure */
#define AHBSP_DATA1_OFFSET        0x70

/* Register Offset in Secure Srcatchpad */
#define RNG_CNT_OFFSET            0x00    /* secure */
#define RNG_DATA0_OFFSET        0x04
#define RNG_DATA1_OFFSET        0x08
#define RNG_DATA2_OFFSET        0x0C
#define RNG_DATA3_OFFSET        0x10
#define RNG_DATA4_OFFSET        0xB0
#define PWC_RESET_OFFSET        0x40
#define PWC_REG_STA_OFFSET        0x54
#define PWC_SET_STATUS_OFFSET        0x58
#define PWC_STROBE_OFFSET        0x90

/* Register Offset in RCT */
#define PLL_CORE_CTRL_OFFSET        0x000
#define PLL_IDSP_CTRL_OFFSET        0x0E4
#define PLL_CORTEX_CTRL_OFFSET        0x264
#define PLL_DDR_CTRL_OFFSET        0x0DC
#define PLL_DDR_CTRL2_OFFSET        0x110
#define PLL_DDR_CTRL3_OFFSET        0x114
#define PLL_SD_CTRL_OFFSET        0x4AC
#define SCALER_SD48_OFFSET        0x00C
#define SCALER_SDXC_OFFSET        0x434
#define RCT_TIMER_OFFSET        0x254
#define SYS_CONFIG_OFFSET        0x034
#define RNG_CTRL_OFFSET            0x1A4

/* Register Offset in PWC Debug port */
#define PWC_KEY_IN0_REG_OFFSET        0x000
#define PWC_KEY_IN1_REG_OFFSET        0x004
#define PWC_KEY_IN2_REG_OFFSET        0x008
#define PWC_KEY_IN3_REG_OFFSET        0x00c
#define PWC_KEY_IN4_REG_OFFSET        0x010
#define PWC_KEY_IN5_REG_OFFSET        0x014
#define PWC_KEY_IN6_REG_OFFSET        0x018
#define PWC_KEY_IN7_REG_OFFSET        0x01c
#define PWC_KEY_OUT0_REG_OFFSET        0x020
#define PWC_KEY_OUT1_REG_OFFSET        0x024
#define PWC_KEY_OUT2_REG_OFFSET        0x028
#define PWC_KEY_OUT3_REG_OFFSET        0x02c
#define PWC_KEY_OUT4_REG_OFFSET        0x030
#define PWC_KEY_OUT5_REG_OFFSET        0x034
#define PWC_KEY_OUT6_REG_OFFSET        0x038
#define PWC_KEY_OUT7_REG_OFFSET        0x03c

