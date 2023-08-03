/**
 *  @file AmbaBST_SpiNOR.asm
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
 *  @details Bootstrap stage-2 Code for SPI-NOR
 *
 */

#include "AmbaBST_SpiNOR.h"

        .global __AmbaBootStrapStage2
        .global __AmbaWarmBoot
        .global __AmbaDeviceInit

#define SYS_PTB_BLOCK_ADDR      0               /* Block-0 */

#define SYS_PTB_RAM_START       AMBA_RAM_NVM_SYS_PARTITION_TABLE_OFFSET
#define USER_PTB_RAM_START      AMBA_RAM_NVM_USER_PARTITION_TABLE_OFFSET

#ifdef CONFIG_CHECK_TARGET_ALL
#define CONFIG_CHECK_TARGET_BLD
#endif

/*
 *  NOTE: The entire code contained in this file resides in the boot code
 *        but is relocated and executed from DRAM.
 */
/*
 * Load PTB from Nor flash to memory
 * (Garbles r0 - r2).
 *
 * r3 - DRAM address
 * r4 - Offset
 * r5 - BytebyCount
 * r8 - Bytes per block@
 */
__AmbaWarmBoot:
__AmbaBootStrapStage2:

        /* STEP 1: Load system partition table */
        /* Load partition table first */
        MOVZ    W0, #(SYS_PTB_BLOCK_ADDR)
        LDR     W22, =(AMBA_ROM_MAGIC_CODE_PHYS_ADDR + AMBA_MAGIC_CODE_SIZE - AMBA_CORTEX_A53_DRAM_VIRT_BASE_ADDR + AMBA_CORTEX_A53_DRAM_PHYS_BASE_ADDR)

        LDR     W2, [X22, #8]                           /* Get PageCount of BST from peripheral */
        LDR     W8, [X22, #0]                           /* Get Byte per Block from SpiNOR device */
        LDR     W1, [X22, #4]                           /* Get Byte per Page from SpiNOR device */
        MUL     W4, W0, W8
        MUL     W3, W1, W2
        ADD     W4, W4, W3
        MOVZ    W3, #(SYS_PTB_RAM_START)
        MOV     W5, #SPI_NOR_DMA_BUF_SIZE

        /* Set DMA Channel_0 to SpiNOR RX */
        LDR     W27, =(AMBA_CORTEX_A53_SCRATCHPAD_S_BASE_ADDR - AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_NONSECURE_AHB_PHYS_BASE_ADDR)
        MOV     W2, #AMBA_DMA_CHANNEL_NOR_SPI_RX
        STR     W2, [X27, #AMBA_DMA_CHANNEL_SLELECT_OFFSET]
        BL      SpiNorRead

        /* STEP 2: Load user partition table */
        MOVZ    W3, #(SYS_PTB_RAM_START)
        LDR     W0, [X3, #12]                           /* Get the USER_PTB block address from SYS_PTB */
        MUL     W4, W0, W8
        MOVZ    W3, #(USER_PTB_RAM_START)
        MOVZ     W5, #SPI_NOR_DMA_BUF_SIZE
        BL      SpiNorRead

        /* Get the BLD length from ptb just loaded */
        MOVZ    W3, #(SYS_PTB_RAM_START)                /* R3 = SYS_PTB dram address */

        LDR     W14,[X3, #132]                          /* X14 = the actural size of BLD, used for RSA. */

#ifdef CONFIG_ATF_HAVE_BL2
        LDR     W6, [X3, #252]                          /* Block address (ATF fip image) */
        MUL     W6, W6, W8                              /* Pass byte-address */
        LDR     X7, =0xF2000000
        STR     W6, [X7, #0x80]                         /* Store for later usage in BL2 */
#endif /* CONFIG_ATF_HAVE_BL2 */
        LDR     W10, [X3, #120]                         /* Save the BLD block count from SYS_PTB */
        LDR     W2, [X3, #124]                          /* Save the BLD block address from SYS_PTB */
        LDR     W3, [X3, #128]                          /* R3 = the BLD dram address */
        MOV     X16, X3                                 /* X16 = the BLD dram address */

        /* Get Len of BLD by block */
        LDR     W8, [X22, #0]                           /* Get Byte per Block from SpiNOR device */
        MUL     W4, W2, W8
        MUL     W11, W10, W8

 LoadSpinorBld:
        BL      SpiNorRead
        ADD     W10, W10, #SPI_NOR_DMA_BUF_SIZE
        ADD     W4, W4, #SPI_NOR_DMA_BUF_SIZE
        ADD     W3, W3, #SPI_NOR_DMA_BUF_SIZE
        CMP     W11, W10
        BHS     LoadSpinorBld

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
 * Load Data from Nor flash to memory
 * (Garbles r0 - r2).
 *
 * r3 - DRAM address
 * r4 - Offset
 * r5 - BytebyCount
 * r8 - Bytes per block@
 */

SpiNorRead:
        LDR     W6, =(AMBA_CORTEX_A53_NOR_SPI_BASE_ADDR - AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_NONSECURE_AHB_PHYS_BASE_ADDR)
        LDR     W9, = (AMBA_CORTEX_A53_DMA0_BASE_ADDR + DMA_RX_OFFSET - AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_NONSECURE_AHB_PHYS_BASE_ADDR)
        LDR     W22, =(AMBA_ROM_MAGIC_CODE_PHYS_ADDR + AMBA_MAGIC_CODE_SIZE)

        /* mask all interrupts */
        MOVZ    W0, #0x20
        STR     W0, [X6, #SPI_NOR_INTRMASK_OFFSET]

        /* reset tx/rx fifo */
        MOVZ    W0, #1
        STR     W0, [X6, #SPI_NOR_TXFIFORST_OFFSET]
        STR     W0, [X6, #SPI_NOR_RXFIFORST_OFFSET]
        /* set tx/rx fifo threshhold level to blk - 1 */
        MOVZ    W0, #31
        STR     W0, [X6, #SPI_NOR_RXFIFOTHLV_OFFSET]
        STR     W0, [X6, #SPI_NOR_TXFIFOTHLV_OFFSET]

        LDR     W0, [X22, #16]
        ORR     W0, W0, W5
        STR     W0, [X6, #SPI_NOR_LENGTH_OFFSET]
        LDR     W0, [X22, #12]
        STR     W0, [X6, #SPI_NOR_CFG_OFFSET]
        LDR     W0, =0x00001400
        ORR     W0, W0, #0x1
        STR     W0, [X6, #SPI_NOR_CTRL_OFFSET]

        MOVZ    W0, #0xBB
        STR     W0, [X6, #SPI_NOR_CMD_OFFSET]
        MOV     W0, #0x0
        STR     W0, [X6, #SPI_NOR_ADDRHI_OFFSET]
        STR     W4, [X6, #SPI_NOR_ADDRLO_OFFSET]

        /* Setup DMA with Channel_0 */
        LDR     W0, = (AMBA_CORTEX_A53_NOR_SPI_BASE_ADDR - AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_NONSECURE_AHB_PHYS_BASE_ADDR + SPI_NOR_RXDATA_OFFSET)
        STR     W0, [X9, #DMA_SRCADDR_OFFSET]
        STR     W3, [X9, #DMA_DESADDR_OFFSET]
        LDR     W0, =0xaa800000
        ORR     W0, W0, W5
        STR     W0, [X9, #DMA_CTRL_OFFSET]

        MOVZ    W0, #0x1
        STR     W0, [X6, #SPI_NOR_DMACTRL_OFFSET]

        /* Start transfer */
        MOVZ    W0, #0x1
        STR     W0, [X6, #SPI_NOR_START_OFFSET]

        MOVZ    W7, #0

        /* Wait for interrupt from DMA */
WaitDmaDone:
        LDR     W0, [X9, #DMA_INT_OFFSET]
        AND     W0, W0, #(0x1 << SPI_NOR_RX_DMA_CHAN)
        CMP     W0, #0
        B.EQ    WaitDmaDone
        MOVZ    W0, #0
        STR     W0, [X9, #DMA_INT_OFFSET]
ReadNorLoop:
        LDR     W0, [X6, #SPI_NOR_RAWINTR_OFFSET]
        AND     W0,  W0, #SPI_NOR_DATA_TRASDONE
        CMP     W0, #SPI_NOR_DATA_TRASDONE
        B.NE     ReadNorLoop

        /* Busy Wait */
        MOV     W0, #0x0
BusyWait:
        ADD     W0, W0, #1
        CMP     W0, #0xff
        B.NE    BusyWait

        /* Clear the DataDone interrupt Status */
        MOVZ    W0, #0x0
        STR     W0, [X9, #DMA_STATUS_OFFSET]

        /* Clear the DataDone interrupt Status */
        MOVZ    W0, #0x7f
        STR     W0, [X6, #SPI_NOR_CLRINTR_OFFSET]

        /* Disable the RX DMA */
        MOVZ    W0, #0x0
        STR     W0, [X6, #SPI_NOR_DMACTRL_OFFSET]

NorReadDone:
        RET

/*
 *  NOTE: The __AmbaDeviceInit is used to init boot device
 */
.section AmbaDeviceInit
__AmbaDeviceInit:
        RET
