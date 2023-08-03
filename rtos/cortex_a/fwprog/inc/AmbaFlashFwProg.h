/**
 *  @file AmbaFlashFwProg.h
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
 *  @details Definitions & Constants for flash firmware program utilities
 */

#ifndef AMBA_FLASH_FW_PROG_H
#define AMBA_FLASH_FW_PROG_H

#define FLPROG_USB_RETURN_OFFSET    0xFFFFC

#define PART_MAGIC_NUM              (0x8732dfe6U)
#define PART_HEADER_MAGIC_NUM       (0xa324eb90U)

/* These are flags set on a firmware partition table entry */
#define PART_LOAD                   0x0     /* Load partition data */
#define PART_NO_LOAD                0x1     /* Don't load part data */
#define PART_COMPRESSED             0x2     /* Data is not compressed */
#define PART_READONLY               0x4     /* Data is RO */

/* Error codes */
#define FLPROG_OK                   0
#define FLPROG_ERR_MAGIC            -1
#define FLPROG_ERR_LENGTH           -2
#define FLPROG_ERR_CRC              -3
#define FLPROG_ERR_VER_NUM          -4
#define FLPROG_ERR_VER_DATE         -5
#define FLPROG_ERR_PROG_IMG         -6
#define FLPROG_ERR_FIRM_FILE        -9
#define FLPROG_ERR_FIRM_FLAG        -10
#define FLPROG_ERR_NO_MEM           -11
#define FLPROG_ERR_FIFO_OPEN        -12
#define FLPROG_ERR_FIFO_READ        -13
#define FLPROG_ERR_PAYLOAD          -14
#define FLPROG_ERR_ILLEGAL_HDR      -15
#define FLPROG_ERR_EXTRAS_MAGIC     -16
#define FLPROG_ERR_PREPROCESS       -17
#define FLPROG_ERR_POSTPROCESS      -18
#define FLPROG_ERR_IOPROCESS        -19
#define FLPROG_ERR_NOMOREBLOCK      -20
#define FLPROG_ERR_NOT_READY        0x00001000

#define BOOT_BUS_WIDTH  177
#define BOOT_CONFIG     179
#define EMMC_HW_RESET   162
#define EXT_CSD_SIZE    512

#define EMMC_ACCP_USER          0
#define EMMC_ACCP_BP_1          1
#define EMMC_ACCP_BP_2          2

#define EMMC_BOOTP_USER         0x38
#define EMMC_BOOTP_BP_1         0x8
#define EMMC_BOOTP_BP_2         0x10

#define EMMC_BOOT_1BIT          0
#define EMMC_BOOT_4BIT          1
#define EMMC_BOOT_8BIT          2
#define EMMC_BOOT_HIGHSPEED     0x8

/*-----------------------------------------------------------------------------------------------*\
 * This is the header for a flash image partition.
 *-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Crc32;              /* CRC32 Checksum       */
    UINT32  VerNum;             /* Version number       */
    UINT32  VerDate;            /* Version date         */
    UINT32  ImgLen;             /* Image length         */
    UINT32  MemAddr;            /* Location to be loaded into memory */
    UINT32  Flag;               /* Flag of partition    */
    UINT32  Magic;              /* The magic number     */
    UINT32  Reserved[57];
} AMBA_IMG_HEADER;

/*-----------------------------------------------------------------------------------------------*\
 *  * SPINOR_LENGTH_REG
 \*-----------------------------------------------------------------------------------------------*/
typedef union {
    UINT32  Data;

    struct {
        UINT32  ImageLen:   13;     /* [12:0] length of boot Image */
        UINT32  Reserved:   3;      /* [15:13] */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        UINT32  Reserved0 : 6;      /* [21:16] Dummy Cycle length */
#else
        UINT32  Clock_Div : 6;      /* [21:16] Dummy Cycle length */
#endif
        UINT32  DummyLen:   5;      /* [26:22] Dummy Cycle length */
        UINT32  AddrLen:    3;      /* [29:27] Address length */
        UINT32  CmdLen:     2;      /* [31:30] Command length */
    } Bits;
} AMBA_NOR_IMGLEN_REG_u;

/*-----------------------------------------------------------------------------------------------*\
 *  * SPINOR_CTRL_REG
 \*-----------------------------------------------------------------------------------------------*/
typedef union {
    UINT32  Data;

    struct {
        UINT32  DataReadn:      1;      /* [0] Data Part Read Mode */
        UINT32  DataWriteen:    1;      /* [1] Data Part Write Mode */
        UINT32  Reserved:       7;      /* [8:2] */
        UINT32  RxLane:         1;      /* [9] RxLANE count */
        UINT32  NumDataLane:    2;      /* [11:10] DataLANE count */
        UINT32  NumAddrLane:    2;      /* [13:12] AddrLANE count */
        UINT32  NumCmdLane:     2;      /* [15:14] CmdLANE count */
        UINT32  Reserved1:      8 ;     /* [23:16] */
        UINT32  Reserved2:      1;      /* [24] LSB & MSB First  (ignored)*/
        UINT32  Reserved3:      3;      /* [27:25] */
        UINT32  DataDTR:        1;      /* [28] Data Double Transfer Rate */
        UINT32  DummyDTR:       1;      /* [29] Dummy Double Transfer Rate */
        UINT32  AddressDTR:     1;      /* [30] Address DTR MODE */
        UINT32  CmdDTR:         1;      /* [31] Cmd DTR MODE */
    } Bits;
} AMBA_NOR_DTR_REG_u;

/*-----------------------------------------------------------------------------------------------*\
 *  * SPINOR_CFG_REG
 *  \*-----------------------------------------------------------------------------------------------*/
typedef union {
    UINT32  Data;

    struct {
        UINT32  RxSampleDelay:  5;      /* [4:0] Adjust RX sampling Data Phase */
        UINT32  Reserved:       5;      /* [9:5] */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        UINT32  Clock_Div:      8;      /* [17:10] Clock divider (N). The SPI clock frequency determines by N - it's 150/N MHz and N is 30 by default. Tune this factor for higher performance. N must be larger or equal to 2. */
#else
        UINT32  Reserved1:      8;      /* [17:10] ignored */
#endif
        UINT32  ChipSelect:     8;      /* [25:18] CEN for multiple device*/
        UINT32  HoldSwitchphase: 1;      /* [26] Clock will remain in standby mode*/
        UINT32  Reserved2:      1;      /* [27]  ignored*/
        UINT32  Hold:           3;      /* [30:28] For Flow control purpose */
        UINT32  FlowControl:    1;      /* [31] Flow control enable */
    } Bits;
} AMBA_NOR_FLOWCTR_REG_u;

/*-----------------------------------------------------------------------------------------------*\
 *  *  SPINOR_CMD_REG
 *  \*-----------------------------------------------------------------------------------------------*/
typedef union {
    UINT32  Data;

    struct {
        UINT32  Cmd0:       8;      /* [7:0] Command 0 for SPI Device*/
        UINT32  Cmd1:       8;      /* [15:8] Command 1 for SPI Device */
        UINT32  Cmd2:       8;      /* [23:16] Command 2 for SPI Device*/
        UINT32  Reserved:   8;      /* [31:24] */
    } Bits;
} AMBA_NOR_CMD_REG_u;

/*-----------------------------------------------------------------------------------------------*\
 *  *  SPINOR_ADDRESS_HI_Register
 *  \*-----------------------------------------------------------------------------------------------*/
typedef union {
    UINT32  Data;

    struct {
        UINT32  Addr4:      8;      /* [7:0] Address 4 Field for SPI Device*/
        UINT32  Addr5:      8;      /* [15:8] Address 5 Field for SPI Device */
        UINT32  Addr6:      8;      /* [23:16] Address 6 Field for SPI Device */
        UINT32  Reserved:   8;      /* [31:24] */
    } Bits;
} AMBA_NOR_ADDRHI_REG_u;

/*-----------------------------------------------------------------------------------------------*\
 *  *  SPINOR_ADDRESS_LOW_Register
 *  \*-----------------------------------------------------------------------------------------------*/
typedef union {
    UINT32  Data;

    struct {
        UINT32  Addr0:  8;  /* [7:0] Address 0 Field for SPI Device */
        UINT32  Addr1:  8;  /* [15:8] Address 1 Field for SPI Device */
        UINT32  Addr2:  8;  /* [23:16] Address 2 Field for SPI Device */
        UINT32  Addr3:  8;  /* [31:24] Address 3 Field for SPI Device */
    } Bits;
} AMBA_NOR_ADDRLOW_REG_u;

/*-----------------------------------------------------------------------------------------------*\
 *  * The boot image header is a128-byte block and only first 24 bytes are used.
 *  * Each 4 bytes is corresponding to the layout (register definition) of
 *  * registers 0x0~0x14, respectively except for the data length field, clock
 *  * divder field, and those controlled by boot options. Please refer to document
 *  * for detailed information.
 \*-----------------------------------------------------------------------------------------------*/
typedef struct {
    AMBA_NOR_IMGLEN_REG_u   Img;     /* 0x0: NOR flash the format of command  */
    AMBA_NOR_DTR_REG_u      DTR;     /* 0x4: NOR flash DTR mode control register  */
    AMBA_NOR_FLOWCTR_REG_u  Flow;    /* 0x8: NOR flash flowcontrol */
    AMBA_NOR_CMD_REG_u      Cmd;     /* 0xc: NOR flash command for SPI device*/
    AMBA_NOR_ADDRHI_REG_u   AddrHi;  /* 0x10: NOR flash Address 6~4 Field for SPI Command */
    AMBA_NOR_ADDRLOW_REG_u  AddrLow; /* 0x14: NOR flash Address 3~0 Field for SPI Command */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 DqsEnable;    /* 0x18: DQS settings. RTL Reg 0x58, See PRM for more details */
    UINT32 Reserved[25]; /* 0x19 - 0x80 */
#else
    UINT32 Reserved[26]; /* 0x18 - 0x80 */
#endif
} AMBA_NOR_FW_HEADER_s;

/*-----------------------------------------------------------------------------------------------*\
 * The following data structure is used by the memfwprog program to output
 * the flash programming results to a memory area.
 *-----------------------------------------------------------------------------------------------*/

#define FWPROG_RESULT_FLAG_LEN_MASK     (0x00ffffffU)
#define FWPROG_RESULT_FLAG_CODE_MASK    (0xff000000U)

typedef struct {
    UINT32  Magic;
#define FWPROG_RESULT_MAGIC (0xb0329ac3U)

    UINT32  BadBlockInfo;
#define BST_BAD_BLK_OVER    0x00000001
#define BLD_BAD_BLK_OVER    0x00000002
#define PBA_BAD_BLK_OVER    0x00000008
#define MTA_BAD_BLK_OVER    0x00000080
#define SYS_BAD_BLK_OVER    0x00000010
#define DSP_BAD_BLK_OVER    0x00000200
#define ROM_BAD_BLK_OVER    0x00000100
#define LNX_BAD_BLK_OVER    0x00000020
#define RFS_BAD_BLK_OVER    0x00000040
    UINT32  Flag[AMBA_NUM_SYS_PARTITION + AMBA_NUM_USER_PARTITION];
} GNU_MIN_PADDING AMBA_FWPROG_RESULT_s;

#define AMBA_FWPROG_RESULT_ADDR  0x000ffe00

INT32 AmbaFlashFwProg_DeviceInit(void);
INT32 AmbaFwProg_ProgramSysPartition(UINT32 PartID, UINT8 * pImage);
INT32 AmbaFwProg_ProgramUserPartition(UINT32 PartID, UINT8 * pImage);
char *AmbaFlashFwProg_GetSysPartitionName(UINT32 PartID);
char *AmbaFlashFwProg_GetUserPartitionName(UINT32 PartID);

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
void AmbaFWProg_SetUserActivePartititon(void);
#endif

void AmbaFlashFwProg_ReturnToUsb(void);
void AmbaFlashFwProg_CheckErasePTB(void);

void AmbaPrint_PrintStr5Fwprog(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);
void AmbaPrint_PrintUInt5Fwprog(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);

#ifndef AMBA_FLASH_FW_HOST_TOOL
INT32 main(void);
#endif

#endif /* AMBA_FLASH_FW_PROG_H */
