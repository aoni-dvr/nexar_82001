/**
 *  @file AmbaBST_eMMC.asm
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
 *  @details Bootstrap stage-2 Code for eMMC
 *
 */

#include "AmbaBST_NAND.h"
#include "AmbaBST_eMMC.h"

        .global __AmbaBootStrapStage2
        .global __AmbaWarmBoot
        .global __AmbaDeviceInit

#define SYS_PTB_BLOCK_ADDR      8               /* Block-0 */
#define SYS_PTB_BLOCK_COUNT     2               /* Page-2 of the block */
#define SYS_PTB_RAM_START       AMBA_RAM_NVM_SYS_PARTITION_TABLE_OFFSET
#define USER_PTB_RAM_START      AMBA_RAM_NVM_USER_PARTITION_TABLE_OFFSET

/* sd_cmd register setting */
#define CMD_1     0x0102       /* CMD1  - SEND_OP_COND, 48 bit response        (R3) */
#define CMD_2     0x0209       /* CMD2  - ALL_SEND_CID, 136 bit response       (R2) */
#define CMD_3     0x031A       /* CMD3  - SET_RELATIVE_ADDR, 48 bit response   (R6) */
#define CMD_6     0x061B       /* CMD6  - SWITCH_BUS_WIDTH, 48 bit response    (R1) */
#define CMD_7     0x071B       /* CMD7  - SELECT_CARD, 48 bit response         (R1B) */
#define CMD_8     0x081A       /* CMD8  - SEND_EXT_CSD, 48 bit response        (R7) */
#define CMD_13    0x0d1A       /* CMD13 - SEND_STATUS, 48 bit response         (R1) */
#define CMD_16    0x101A       /* CMD16 - SET_BLOCKLEN, 48 bit response        (R1) */
#define CMD_17    0x113A       /* CMD17 - READ_SINGLE_BLOCK, 48 bit response   (R1) */
#define CMD_18    0x123A       /* CMD18 - READ_MULTIPLE_BLOCK, 48 bit response (R1) */
#define CMD_55    0x371A       /* CMD55 - APP_CMD, 48 bit response             (R1) */

/* CMD_6 argument */
#define BUS_WIDTH_4            0x3B70100
#define BUS_WIDTH_8            0x3B70200

#if defined(CONFIG_EMMC_ACCESS_8BIT)
#define HOST_BUS_WITH          0x8
#define ARG_CMD6_BUS_WIDTH     BUS_WIDTH_8

#elif defined(CONFIG_EMMC_ACCESS_4BIT)
#define HOST_BUS_WITH          0x2
#define ARG_CMD6_BUS_WIDTH     BUS_WIDTH_4

#else
#define HOST_BUS_WITH          0x0
#endif

#define ARG_CMD6_HIGH_SPEED    0x3B90100

#ifdef CONFIG_CHECK_TARGET_ALL
#define CONFIG_CHECK_TARGET_BLD
#endif

/*
 *  NOTE: The entire code contained in this file resides in the boot code
 *        but is relocated and executed from DRAM.
 */
__AmbaWarmBoot:
__AmbaBootStrapStage2:

        /* Program base address of sd */
        LDR     W0, =(AMBA_CORTEX_A53_SD0_BASE_ADDR - AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_NONSECURE_AHB_PHYS_BASE_ADDR)

        /* Reset all */
        MOV     W1, #0x1
        STRB    W1, [X0, #SD_RESET_OFFSET]

        MOV     W1, 15000
DelayReset:
        SUB     W1, W1, #0x1
        CMP     W1, #0x0
        BNE     DelayReset

_wait_reset_done:
        LDRB    W1, [X0, #SD_RESET_OFFSET]
        CMP     W1, #0
        BNE     _wait_reset_done

_wait_ready_to_boot:
        LDRB    W1, [X0, #SD_BOOT_STA_OFFSET]
        AND     W1, W1, #0x1
        CMP     W1, #1
        BNE     _wait_ready_to_boot

        /* Set data timeout */
        MOV     W1, #0xE
        STRB    W1, [X0, #SD_TMO_OFFSET]

        /* Reset DATA line and make sure data line ready */
        LDR     W1, =(SD_RESET_DAT | SD_RESET_CMD)
        LDR     W2, =SD_RESET_REG
        STRB    W1, [X2, #0x0]

        /* Wait dataline ready */
        MOV     W2, #(SD_STA_CMD_INHIBIT_DAT | SD_STA_CMD_INHIBIT_CMD)
wait_data_line_ready:
        LDR     W3, [X0, #SD_STA_OFFSET]
        AND     W3, W3, W2
        CMP     W3, #0
        BNE     wait_data_line_ready

       /* Setup clock */
_wait_clock_stable:
        MOV     W1, #0x80       /* XXMhz input clock */
        LSL     W1, W1, #0x8
        MOV     W2, #(SD_CLK_ICLK_EN | SD_CLK_EN)
        ORR     W1, W1, W2
        STRH    W1, [X0, #SD_CLOCK_CONTROL]

        /* Enable interrupts on events we want to know */
        MOV     W1, #(SD_NISEN_XFR_DONE | SD_NISEN_CMD_DONE)
        STRH    W1, [X0, #SD_NISEN_OFFSET]

        /* Enable error interrupt status */
        LDR     W1, =0x1FF
        STRH    W1, [X0, #SD_EISEN_OFFSET]

        /*
         CMD0
        */
        MOV     W3, #(SD_NIS_CMD_DONE)

        MOV     W6, #0x0
        MOV     W7, #0x0000                             /* send CMD0 - GO_IDLE_STATE */
        BL      sdmmc_command

_setup_mmc_card:
        /*
         CMD1 - Send CMD1 with working voltage until the memory becomes ready
        */

        /* mov    W6, W5        @card->ocr */
        LDR     W6, =0x40ff0000
        LDR     W7, =CMD_1
        BL      sdmmc_command

        MOV     W2, #0x1
        LSL     W2, W2, #0x1f                            /* W2 = 0x80000000 */

        LDR     W5, [X0, #SD_RSP0_OFFSET]                /* ocr = UNSTUFF_BITS(cmd.resp, 8, 32) */
        MOV     W1, W5
        AND     W1, W1, W2                               /* SDMMC_CARD_BUSY */
        CMP     W1, W2
        BNE     _setup_mmc_card

        /*
          CMD2 - All Send CID
        */

        MOV     W6, #0x0
        LDR     W7, =CMD_2
        BL      sdmmc_command

        /*
          CMD3 - Ask RCA
        */
        MOV     W4, #0x1                                  /* card->rca = 1 */
        LSL     W4, W4, #0x10                             /* W6 = (card->rca << 16) */
        MOV     W6, W4
        LDR     W7, =CMD_3
        BL      sdmmc_command

        /*
          CMD7 - Select card
        */
        MOV     W3, #(SD_NIS_CMD_DONE | SD_NIS_XFR_DONE)

        MOV     W6, W4       /* (card->rca << 16) */
        LDR     W7, =CMD_7
        BL      sdmmc_command

        /*
          CMD16: Set block length
        */
        MOV     W3, #(SD_NIS_CMD_DONE)

        MOV     W6, #0x1       /* W6 = 512
        LSL     W6, W6, #0x9
        LDR     W7, =CMD_16
        BL      sdmmc_command

        /*
          CMD6: Set To High Speed
        */
        MOV     W3, #(SD_NIS_CMD_DONE | SD_NIS_XFR_DONE)

        LDR     W6, =ARG_CMD6_HIGH_SPEED
        LDR     W7, =CMD_6
        BL      sdmmc_command

        /* Set bus width */
#if (HOST_BUS_WITH > 0)
        LDR     W6, =ARG_CMD6_BUS_WIDTH
        LDR     W7, =CMD_6
        BL      sdmmc_command
#endif

_setup_card_done:                                         /* Done card initialize. Do setup controler register */

        MOV     W3, #HOST_BUS_WITH
        STRB    W3, [X0, #SD_HOST_OFFSET]                 /* Host Bus Width */

        MOV     W3, #0x72                                 /* Set block size */
        LSL     W3, W3, #0x8                              /* block size : 512 (W3 = 7200) */
        STRH    W3, [X0, #SD_BLK_SZ_OFFSET]

        MOV     W3, #0x37                                 /* Set Transfer control */
        STRH    W3, [X0, #SD_XFR_OFFSET]

#if 0
        /* Disable clock */
        MOV     W1, #0x0
        STRH    W1, [X0, #SD_CLOCK_CONTROL]

        /* Setup clock by RCT */
        LDR     W1, =(AMBA_DBG_PORT_RCT_BASE_ADDR - AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_APB_PHY_BASE_ADDR)
        /* Set Clock source (0x13 + 1) * 24HMZ = 480Mhz */
        LDR     W3, =0x13000000
        STR     W3, [X1, #PLL_SD_CTRL_REG]

        MOV    W3, #0xA                                     /* Set SD divider 480Mhz / 0xA  = 48Mhz */
        STR    W3, [X1, #PLL_SD48_POST_SCALER_REG]
#endif
        /* Enable clock */
        MOV     W2, #(SD_CLK_ICLK_EN | SD_CLK_EN)
        STRH    W2, [X0, #SD_CLOCK_CONTROL]

WaitClkStable:
        LDRH    W1, [X0, #SD_CLOCK_CONTROL]
        BIC     W1, W1, W2
        CMP     W1, #SD_CLK_ICLK_STABLE
        BNE     WaitClkStable

        MOV     W1, #0x1000
Delay1:
        SUB     W1, W1, #0x1
        CMP     W1, #0x0
        BNE     Delay1

/*
 * Perform BLD loading on the EMMC
 *
 * W0~W2 - Garbled by the page loader function, so they are used as tmp var.
 * W3 - DRAM address
 * W1 - Start block #
 * W5 - page variable
 *
 */
t_sdmmc_boot:

        /* STEP 1: Load system partition table */
        LDR     W6, =SYS_PTB_RAM_START                  /* W6 = SYS_PTB dram address */
        LDR     W1, =SYS_PTB_BLOCK_ADDR
        LDR     W5, =SYS_PTB_BLOCK_COUNT                /* Read one page (SYS_PTB is smaller than one page size) */
        BL      t_sdmmc_read_sector                     /*  Load SYS_PTB page */

        /* STEP 2: Load user partition table */
        LDR     W1, [X6, #12]                            /* Get the USER_PTB block address from SYS_PTB */
        LDR     W5, =0x4
        LDR     W6, =USER_PTB_RAM_START                 /* W6 = USER_PTB dram address */
        BL      t_sdmmc_read_sector                     /*  Load USER_PTB page */

        /* STEP 3: Load BLD */
        LDR     W3, =SYS_PTB_RAM_START                  /* W3 = SYS_PTB dram address */

        LDR     W14,[X3, #132]                          /* X14 = the actural size of BLD, used for RSA. */

#ifdef CONFIG_ATF_HAVE_BL2
        LDR     W6, [X3, #252]                          /* Block/Sector address (ATF fip image) */
        MOVZ    W7, #512                                /* Byte-size per sector */
        MUL     W6, W6, W7                              /* Pass byte-address */
        LDR     X8, =0xF2000000
        STR     W6, [X8, #0x80]                         /* Store for later usage in BL2 */
#endif /* CONFIG_ATF_HAVE_BL2 */
        LDR     W5, [X3, #120]                          /* Save the BLD block count from SYS_PTB */
        LDR     W1, [X3, #124]                          /* Save the BLD block address from SYS_PTB */
        LDR     W6, [X3, #128]                          /* W3 = the BLD dram address */
        MOV     W16, W6
        BL      t_sdmmc_read_sector

EnterBLD:
        MOV     X22, X16                                /* X16 = the BLD dram address */
        BL      verify_signature

#ifdef CONFIG_CHECK_TARGET_BLD
        /*  Verify BLD integrity  */
        MOV     X28, X16                                /* X28 = the BLD dram address, X16 used for crc32 */
        MOVZ    W3, #(SYS_PTB_RAM_START)                /* X3 = SYS_PTB dram address */
        LDR     W15,[X3, #132]                          /* X15 = the actural size of BLD, used for CRC32. */
        LDR     W16,[X3, #140]                          /* Save the BLD CRC32 from SYS_PTB */
        BL      verify_crc32
        BR      X28                                     /* Jump to BLD */
#endif

        BR      X16                                     /* Jump to BLD */

/*
 * Issue a SD eMMC command
 *
 * W6 - argument
 * W7 - command
 * W3 - wait condition
 *
 * The result is returned in W1.
 */
sdmmc_command:

        STR    W6, [X0, #SD_ARGUMENT0]     /* argument */
        STRH   W7, [X0, #SD_CMD_OFFSET]    /* command */

        /* Wait for command to complete */

_wait_command_complete:
        LDRH   W1, [X0, #SD_NIS_OFFSET]
        AND    W1, W1, W3
        CMP    W1, W3
        BNE    _wait_command_complete

        /* save error status to W1 and clear interrupts */
        STRH    W1, [X0, #SD_NIS_OFFSET]

        LDRH    W1, [X0, #SD_EIS_OFFSET]          /* Check Error */
        CMP     W1, #0
        BNE     CmdError

        RET

/*
 * Reading a sector from SD/MMC in DMA mode.
 *
 * W1 - sector to read
 * W5 - count
 * W3 - temp
 * W6, W7
 *
 * The result is returned in W1.
 */
t_sdmmc_read_sector:

        /* Wait data/cmd line ready */
        MOV     W2, #(SD_STA_CMD_INHIBIT_CMD | SD_STA_CMD_INHIBIT_DAT)
_wait_cmdline_ready:
        LDR     W3, [X0, #SD_STA_OFFSET]
        AND     W3, W3, W2
        CMP     W3, #0
        BNE     _wait_cmdline_ready

        STR     W6, [X0, #SD_DMA_ADDR_OFFSET]
        STRH    W5, [X0, #SD_BLK_CNT_OFFSET]
        STR     W1, [X0, #SD_ARGUMENT0]        /* argument */

        LDR     W3, =CMD_18                    /* CMD18 Read multiple sectors */
        STRH    W3, [X0, #SD_CMD_OFFSET]       /* command */

        /* Wait for command to complete */
        MOV     W5, #(SD_NIS_CMD_DONE | SD_NIS_XFR_DONE)
_wait_cmd_complete_s:
        LDRH    W3, [X0, #SD_EIS_OFFSET]          /* Check Error */
        CMP     W3, #0
        BNE     CmdError

        LDRH    W3, [X0, #SD_NIS_OFFSET]
        AND     W3, W3, W5
        CMP     W3, W5
        BNE     _wait_cmd_complete_s

        /* Clear interrupts */
        STRH    W3, [X0, #SD_NIS_OFFSET]

    /* Make sure the DMA Transport Done */
WaitDmaDone:
        LDRH   W1, [X0, #SD_BLK_CNT_OFFSET]
        CMP    W1, #0x0
        BNE    WaitDmaDone

        RET

CmdError:
        B       .

.section AmbaDeviceInit
__AmbaDeviceInit:
/*
 *  NOTE: Send eMMC command 0 with argument 0xF0F0F0F0 for warm reset.
 */
        /*  Program base address of sd */
#if 0
        LDR     W0, =SD_BASE            /*  load sd base address register */

        /*  Reset all */
        strb    W2, [X0, #SD_RESET_OFFSET]
_wait_reset_done0:
        LDRB    W2, [X0, #SD_RESET_OFFSET]
        CMP     W2, #0
        BNE     _wait_reset_done0

        /*  Setup clock */
        MOV     W2, #(SD_CLK_ICLK_EN | SD_CLK_EN)
        STRH    W2, [X0, #SD_CLOCK_CONTROL]

WaitClkStable0:
        LDRH    W1, [X0, #SD_CLOCK_CONTROL]
        BIC     W1, W1, W2
        CMP     W1, #SD_CLK_ICLK_STABLE
        BNE     WaitClkStable0

        /*  Enable interrupts on events we want to know */
        LDR     W1, =(SD_NISEN_XFR_DONE | SD_NISEN_CMD_DONE)
        STRH    W1, [X0, #SD_NISEN_OFFSET]

        /*  Enable error interrupt status */
        LDR     W1, =0x1FF
        STRH    W1, [X0, #SD_EISEN_OFFSET]

        /*  Set rst_fio_boot_en */
        LDR     W1, =0x10800
        STR     W1, [X0, #SD_BOOT_CTR_OFFSET]

        /*
         *  CMD0 with argumet 0xF0F0F0F0
        */
        LDR     W6, =0xF0F0F0F0
        MOV     W7, #0x0                   /* send CMD0 - GO_IDLE_STATE */

        STR     W6, [X0, #SD_ARGUMENT0]    /*  W6 - argument */
        STRH    W7, [X0, #SD_CMD_OFFSET]   /*  W7 - command */

        /*  Wait for command to complete */
        MOV     W2, #SD_NIS_CMD_DONE
_wait_command_complete0:
        LDRH    W1, [X0, #SD_NIS_OFFSET]
        AND     W1, W1, W2
        CMP     W1, #0
        BEQ     _wait_command_complete0
#endif
        RET
