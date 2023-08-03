/**
 *  @file AmbaBST_NAND.asm
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
 *  @details Bootstrap stage-2 Code for NAND
 *
 */

#include "AmbaBST_NAND.h"
#include "AmbaBST_SpiNAND.h"

        .global __AmbaBootStrapStage2
        .global __AmbaWarmBoot
        .global __AmbaDeviceInit

#if defined(CONFIG_BST_LARGE_SIZE)
#define BST_ROM_LOAD_SIZE (16384)
#else
#define BST_ROM_LOAD_SIZE (8192)
#endif
#define AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE (BST_ROM_LOAD_SIZE)		/* Need to sync with AmbaNAND_Def.h */

#define SYS_PTB_BLOCK_ADDR      0               /* Block-0 */
#define SYS_PTB_PAGE_ADDR       (AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE / 2048)               /* Page-12 of the block */
#define SYS_PTB_PAGE_ADDR_4K    (AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE / 4096)               /* Page- 8 of the block */
#define SYS_PTB_RAM_START       AMBA_RAM_NVM_SYS_PARTITION_TABLE_OFFSET
#define USER_PTB_RAM_START      AMBA_RAM_NVM_USER_PARTITION_TABLE_OFFSET
#define BBT_RAM_START           AMBA_RAM_NVM_PRIMARY_BAD_BLOCK_TABLE_OFFSET
#define SPARE_RAM_START         AMBA_RAM_APPLICATION_SPECIFIC_OFFSET	/* BST wont load mirror BBT. */

#ifdef CONFIG_CHECK_TARGET_ALL
#define CONFIG_CHECK_TARGET_BLD
#endif

/*
 *  NOTE: The entire code contained in this file resides in the boot code
 *        but is relocated and executed from DRAM.
 */
__AmbaWarmBoot:
__AmbaBootStrapStage2:
        LDR     X20, =(AMBA_DBG_PORT_RCT_BASE_ADDR - AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A76_APB_PHYS_BASE_ADDR)
        LDR     X22, =(AMBA_CORTEX_A76_FLASH_CPU_BASE_ADDR - AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + AMBA_CORTEX_A76_AHB_PHYS_BASE_ADDR)

        MOV     W1, #(FIO_STATUS_CMD_DONE | FIO_STATUS_BCH_ERROR | FIO_STATUS_BCH_UNCORRECTABLE)
        STR     W1, [X22, #FIO_RAW_STATUS_REG]          /* Write 1 to Clear NAND interrupt register */
        STR     W1, [X22, #FIO_INT_ENABLE_REG]          /* Enable NAND INT */

        STR     W1, [X22, #FIO_INT_ENABLE_REG]          /* Enable NAND INT */
        /* Setup SpiNAND timing parameters */
        LDR     W0, =(AMBA_ROM_MAGIC_CODE_PHYS_ADDR + AMBA_MAGIC_CODE_SIZE)
        LDP     W1, W2, [X0], #8                        /* Load SPINAND flash timing parameter 0, 1 */
        LDR     W7, [X0], #4                            /* Load SpiNAND flash timing parameter 2  */
        LDR     W3, [X0], #4                            /* Load Plane select bit */
        STR     W7, [X22, #SPINAND_TIMING2_REG]         /* Write to NAND flash timing register 2*/

        STR     W1, [X22, #SPINAND_TIMING0_REG]         /* Write to NAND flash timing register 0*/
        STR     W2, [X22, #SPINAND_TIMING1_REG]         /* Write to NAND flash timing register 1*/

        /* Config DSM BCH ECC bits */
        LDR     W10, [X20, #SYS_CONFIG_RAW_REG]         /* Load SYS_CONFIG_REG */

        TST     W10, #SYS_CONFIG_NAND_FLASH_PAGE_4K
        LDR     W1, =(NAND_EXT_CTRL_PAGE_4K_ENABLE)     /* Set Page size to 4k */
        MOVZ    W2, #0
        CSEL    W0, W1, W2, EQ                          /* W0 = NAND_EXT_CTRL. */
        STR     W0, [X22, #NAND_EXT_CTRL_REG]

        MOVZ    W1, #4096                               /* MAIN_SIZE. 2K page */
        MOVZ    W2, #2048                               /* MAIN_SIZE. 4K page */
        CSEL    W11, W1, W2, EQ                         /* W11 = MAIN_SIZE. */

        MOVZ    W1, #128                               /* SPARE_SIZE = 64 bytes per page */
        MOVZ    W2, #64                                /* SPARE_SIZE = 128 bytes per page */
        CSEL    W12, W1, W2, EQ

        TST     W10, #SYS_CONFIG_NAND_SPARE_2X
        MOVZ    W1, #1
        MOVZ    W2, #2
        CSEL    W0, W1, W2, EQ
        MUL     W12, W12, W0                            /* 2X spare size */

        /* Disable BCH of nand controller and apply DSM Setting by Main and
         * Spare Size of nand device */
        CMP     W11, #4096
        B.EQ    Enter_Page4K
        TST     W10, #SYS_CONFIG_NAND_SPARE_2X
        LDR     W1, =(FDMA_DSM_MAIN_STRIDE_SIZE_2KB | FDMA_DSM_SPARE_STRIDE_SIZE_64B)
        LDR     W2, =(FDMA_DSM_MAIN_STRIDE_SIZE_2KB | FDMA_DSM_SPARE_STRIDE_SIZE_128B)
        CSEL    W0, W1, W2, EQ
        STR     W0, [X22, #FDMA_DSM_CTRL_REG]            /* FIO Dual-Space Mode control */
        BL      Set_NAND_EXTReg

Enter_Page4K:
        TST     W10, #SYS_CONFIG_NAND_SPARE_2X
        LDR     W1, =(FDMA_DSM_MAIN_STRIDE_SIZE_4KB | FDMA_DSM_SPARE_STRIDE_SIZE_128B)
        LDR     W2, =(FDMA_DSM_MAIN_STRIDE_SIZE_4KB | FDMA_DSM_SPARE_STRIDE_SIZE_256B)
        CSEL    W0, W1, W2, EQ                           /* W0 = DSM_CTRL. */
        STR     W0, [X22, #FDMA_DSM_CTRL_REG]            /* FIO Dual-Space Mode control */

Set_NAND_EXTReg:
        MOVZ    W1, #0                                  /* 1X spare */
        MOVZ    W2, #1                                  /* 2X spare */
        CSEL    W0, W1, W2, EQ                          /* W0 = NAND_EXT_CTRL. */
        LDR     W1, [X22, #NAND_EXT_CTRL_REG]
        ORR     W0, W0, W1
        STR     W0, [X22, #NAND_EXT_CTRL_REG]

        /* MOVZ    W1, #0 */
        /* MOVZ    W2, #(FIO_CTRL_ECC_BCH8)
        /* CSEL    W0, W1, W2, EQ */                         /* W0 = FIO_CTRL. */

        /* Set FIO CTRL */
        MOVZ    W0, #0
        ORR     W0, W0, #(FIO_CTRL_SKIP_BLANK_ECC)
        STR     W0, [X22, #FIO_CTRL_REG]                /* Setup FLASH_IO Control Register */

        /* Set FIO CTRL2 */
        MOVZ    W1, #(FIO_CTRL2_SLECET_SPINAD)
        STR     W1, [X22, #FIO_CTRL2_REG]                /* Setup FLASH_IO Control Register */

        /* Setup SpiNAND_CRTL_Reg */
        /* SCK = 0, Max_CMD_Loop = 0x3fff, Flow_ctrl = 0, PlaneSelect  */
        ORR     W3, W3, #SPINAND_CTRL_MAX_CMD_LOOP
        STR     W3, [X22, #SPINAND_CTRL_REG]                /* Setup FLASH_IO Control Register */

/*
 * Perform BLD loading on the NAND flash
 * W11 - current main_size
 * W12 - current spare_size
 *
 * W3 - DRAM address
 * W4 - Start block #
 * W5 - page variable
 * W7 - Number of pages
 */
        /* STEP 1: Load system partition table */
        MOVZ    W4, #SYS_PTB_PAGE_ADDR_4K               /* 4K page PTB at page 1 */
        MOVZ    W5, #SYS_PTB_PAGE_ADDR                  /* 2K page PTB at page 2 */
        CMP     W11, #4096
        CSEL    W5, W4, W5, EQ                          /* X5 = SYS_PTB start page */

        MOVZ    W3, #(SYS_PTB_RAM_START)                /* X3 = SYS_PTB dram address */
        MOVZ    W4, #(SYS_PTB_BLOCK_ADDR)
        MOVZ    W7, #1                                  /* Read one page (SYS_PTB is smaller than one page size) */
        BL      SpiNandReadPage                            /* Load SYS_PTB page */

        /* STEP 2: Load user partition table */
        LDR     W4, [X3, #12]                            /* Get the USER_PTB block address from SYS_PTB */
        MOVZ    W5, #0
        MOVZ    W3, #(USER_PTB_RAM_START)               /* X3 = USER_PTB dram address */
        BL      SpiNandReadPage                            /* Load USER_PTB page */

        /* STEP 3: Load bad block table */
        LDR     W4, [X3, #1416]                         /* Get the primary BBT block address from USER_PTB */
        MOVZ    W5, #0
        MOVZ    W3, #(BBT_RAM_START)                    /* X3 = primary bad block table dram address */
        BL      SpiNandReadPage                            /* Load USER_PTB page */

        /* STEP 4: Load BLD */
        MOVZ    W3, #(SYS_PTB_RAM_START)                /* X3 = SYS_PTB dram address */

        LDR     W14,[X3, #132]                          /* X14 = the actural size of BLD, used for RSA. */

#ifdef CONFIG_ATF_HAVE_BL2
        LDR     W6, [X3, #252]                          /* Block address (ATF fip image) */
        MOVZ    W7, #64                                 /* Number of pages per block */
        MUL     W7, W7, W11                             /* Bloack * main-size */
        MUL     W6, W6, W7                              /* Pass byte-address */
        LDR     X8, =0x20F2000000UL                     /* AXI configuration, bypass_boot_change */
        STR     W6, [X8, #0x80]                         /* Store for later usage in BL2 */
#endif /* CONFIG_ATF_HAVE_BL2 */

        MOV     W7, #0                                  /* Start to Count actural number of pages of BLD */
        LDR     W6, [X3, #132]                          /* W6 = the actural size of BLD */
PageCountLoop:
        SUB     W6, W6, W11
        ADD     W7, W7, #1
        CMP     W6, W11
        B.GT    PageCountLoop

        CMP     W6, #0
        B.EQ    DonePageCnt
        ADD     W7, W7, #1
DonePageCnt:
        MOV     W6, W7                                  /* W6 = BLD page count. */

        LDR     W4, [X3, #124]                          /* Save the BLD block address from SYS_PTB */
        LDR     W3, [X3, #128]                          /* X3 = the BLD dram address */
        MOVZ    W7, #64                                 /* Number of pages per block */
        MUL     W1, W12, W7
        SUB     W1, W3, W1
        ADR     X10, SpareAnchor
        STR     W1, [X10]
        MOV     X16, X3                                 /* X16 = the BLD dram address */

SpiNandReadBlock:
        CMP     W6, #0                                  /* check if all the BLD loaded done */
        B.EQ    EnterBLD

        BL      NandCheckBlock
        CMP     W0, #0
        B.NE    TryNextBlock                            /* if current block is bad, check the next block */

        MOVZ    W5, #0                                  /* Set page number to 0 */
        CMP     W6, W7
        CSEL    W7, W6, W7, le
        BL      SpiNandReadPage                         /* Load one block if page count large than pages of block, else load W6 pages */
        MUL     W0, W7, W11                             /* W0: load size. ex:(num_page * main_size) = (64 * 2048) */
        ADD     W3, W3, W0                              /* move DRAM address */
        SUB     W6, W6, W7                              /* W6: the remaining pages count minus load pages(W7) */

TryNextBlock:
        ADD     W4, W4, #1                              /* move block index to the next */
        B       SpiNandReadBlock

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
 * Check if a block is bad. The result is returned in W0
 * W3 - DRAM address
 * W4 - Block index
 * W5 - Page index
 * W7 - Number of pages
 */
NandCheckBlock:
        MOVZ    W2, #(BBT_RAM_START)                    /* X2 = primary bad block table dram address */
        ADD     W4, W4, #20
        LSR     W1, W4, #4                              /* 2-bit per block mark. 16 block marks per word. */
        ADD     W2, W2, W1, LSL #2                      /* X2 = word address contains the mark of block 'W4' */
        LDR     W0, [X2]                                /* Load one word (16 block marks) */
        MVN     W0, W0                                  /* Good block mark: 0x3 -> 0x0 */
        MOVZ    W2, #0x3                                /* W2 = mask for block mark */
        AND     W1, W4, #0xf
        LSL     W1, W1, #1                              /* W1 = mask offset */
        LSL     W2, W2, W1
        AND     W0, W0, W2                              /* W0 = 0 if good block */
        SUB     W4, W4, #20
        RET                                             /* return to caller */

/*
 * Load a page from flash to memory
 * W3 - DRAM address
 * W4 - Block index
 * W5 - Page index
 * W7 - Number of pages
 */
SpiNandReadPage:

        /* Set NAND CTRL:FIO_REG(0x120) */
        LDR     W2, =(NAND_CTRL_PAGE_ADDR_CYCLE_3 | NAND_CTRL_READ_ID_CYCLE_4 | NAND_CTRL_CHIP_SIZE_8G)
        STR     W2, [X22, #NAND_CTRL_REG]               /* Setup Flash Control Register */

        STR     W3, [X22, #FDMA_MAIN_MEM_ADDR_REG]      /* main dst address */

        LDR     W1, SpareAnchor
        STR     W1, [X22, #FDMA_SPARE_MEM_ADDR_REG]     /* spare dst address */

        /* Setup FDMA MAIN CTRL: 0x300 */
        ADD     W0, W12, W11                             /* main size = num_page * main_size */
        MUL     W0, W0, W7
        LDR     W1, DmaChanCtrl
        ORR     W1, W1, W0
        STR     W1, [X22, #FDMA_MAIN_CTRL_REG]

        /* Caluculate target flash memory address */
        LSL     W1, W4, #6
        ADD     W5, W1, W5
        MUL     W0, W5, W11                             /* page addr = page idx * main_size */
        LSR     W0, W0, #4
        LSL     W0, W0, #4                              /* We do not need BIT 0~3 of address */
        STR     W0, [X22, #NAND_CMD_REG]                /* Setup Set NAND CMD Register */

        /* Setup Read Error Pattern */
        LDR     W2, =(AMBA_SPINAND_ERROR_PATTERN)
        STR     W2, [X22, #SPINAND_ERROR_PATTERN_REG]

        /* Setup OP code to Custom CMD1_val0 & CMD2_val0 */
        LDR     W2, =(AMBA_SPINAND_CMD_READ_ARRAY | AMBA_SPINAND_CMD_READ)
        STR     W2, [X22, #NAND_CUSTOM_CMDWORD_REG]               /* Setup Flash Control Register */

        /* Setup Spinand Customer CMD2 */
        LDR     W2, =(SPINAND_CUSTOM_CMD2_ENABLE | SPINAND_CUSTOM_CMD2_RE_TYPE | SPINAND_CUSTOM_CMD2_DATA_SRC_DMA | SPINAND_CUSTOM_CMD2_ADDR_SRC_BY_ROW_COLUMN | SPINAND_CUSTOM_CMD2_DATA_DUMMY_ONE_CYCLE | SPINAND_CUSTOM_CMD2_ADDR_SRC_CYCLE)
        STR     W2, [X22, #SPINAND_CUSTOM_CMD2_REG]               /* Setup Flash Control Register */

        /* Setup Spinand Customer CMD1 */
        LDR     W2, =(SPINAND_CUSTOM_CMD1_DATA_SRC_DMA | SPINAND_CUSTOM_CMD1_ADDR_SRC_BY_ROW | SPINAND_CUSTOM_CMD1_ADDR_SRC_CYCLE | SPINAND_CUSTOM_CMD1_ENABLE_AUTO_STATUS)
        STR     W2, [X22, #SPINAND_CUSTOM_CMD1_REG]               /* Setup Flash Control Register */

        /* Wait for interrupt from FLASH_IO DMA */
WaitNandFioDmaDone:
        LDR     W1, [X22, #FIO_RAW_STATUS_REG]
        TST     W1, #FIO_STATUS_CMD_DONE
        B.EQ    WaitNandFioDmaDone

        /* Clear DMA status register */
        MOV     W1, #(FIO_STATUS_CMD_DONE | FIO_STATUS_BCH_ERROR | FIO_STATUS_BCH_UNCORRECTABLE)
        STR     W1, [X22, #FIO_RAW_STATUS_REG]          /* Write 1 to Clear NAND interrupt register */

EotNand:
        RET                                             /* return to caller */

DmaChanCtrl:        .word   (FDMA_CTRL_WRITE_MEM | FDMA_CTRL_NO_ADDR_INC | FDMA_CTRL_BUS_BLK_SIZE_512 | FDMA_CTRL_ENABLE)
SpareAnchor:        .word   (SPARE_RAM_START)

/*
 *  NOTE: The __AmbaDeviceInit is used to init boot device
 */
.section AmbaDeviceInit
__AmbaDeviceInit:
        RET
