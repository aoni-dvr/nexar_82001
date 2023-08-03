/**
 *  @file AmbaRTSL_SD.h
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
 *  @details Definitions & Constants for SD Controller RTSL APIs
 *
 */

#ifndef AMBA_RTSL_SD_H
#define AMBA_RTSL_SD_H

#ifndef AMBA_SD_DEF_H
#include "AmbaSD_Def.h"
#endif
#ifndef AMBA_SD_STD_H
#include "AmbaSD_STD.h"
#endif
#ifndef AMBA_GPIO_DEF_H
#include "AmbaGPIO_Def.h"
#endif


/*
 * Definitions of SD Interrupt Status: must match AMBA_SD_IRQ_STATUS_REG_u of Amba_RegSD.h
 */
#define AMBA_SD_IRQ_CMD_DONE                   (UINT32) 1U     /* [0]: 1 - Command Complete */
#define AMBA_SD_IRQ_TRANSFER_DONE       ((UINT32) 1U << 1U)    /* [1]: 1 - a read / write transaction is completed */
#define AMBA_SD_IRQ_BLK_GAP_EVENT       ((UINT32) 1U << 2U)    /* [2]: 1 - Block Gap Event */
#define AMBA_SD_IRQ_DMA                 ((UINT32) 1U << 3U)    /* [3]: 1 - DMA Interrupt is Generated */
#define AMBA_SD_IRQ_BUF_WRITE_READY     ((UINT32) 1U << 4U)    /* [4]: 1 - Ready to Write Buffer */
#define AMBA_SD_IRQ_BUF_READ_READY      ((UINT32) 1U << 5U)    /* [5]: 1 - Ready to read Buffer */
#define AMBA_SD_IRQ_CARD_INSERTION      ((UINT32) 1U << 6U)    /* [6]: 1 - Card Inserted */
#define AMBA_SD_IRQ_CARD_REMOVAL        ((UINT32) 1U << 7U)    /* [7]: 1 - Card Removed */
#define AMBA_SD_IRQ_CARD                ((UINT32) 1U << 8U)    /* [8]: 1 - Generate Card Interrupt */
#define AMBA_SD_IRQ_ERROR               ((UINT32) 1U << 15U)   /* [15]: 1 - Error Interrupt */
#define AMBA_SD_IRQ_CMD_TIME_OUT_ERROR  ((UINT32) 1U << 16U)   /* [16]: 1 - Command Timeout Error */
#define AMBA_SD_IRQ_CMD_CRC_ERROR       ((UINT32) 1U << 17U)   /* [17]: 1 - Command CRC Error */
#define AMBA_SD_IRQ_CMD_END_BIT_ERROR   ((UINT32) 1U << 18U)   /* [18]: 1 - Command End Bit Error */
#define AMBA_SD_IRQ_CMD_INDEX_ERROR     ((UINT32) 1U << 19U)   /* [19]: 1 - Command Index Error */
#define AMBA_SD_IRQ_DATA_TIME_OUT_ERROR ((UINT32) 1U << 20U)   /* [20]: 1 - Data Timeout Error */
#define AMBA_SD_IRQ_DATA_CRC_ERROR      ((UINT32) 1U << 21U)   /* [21]: 1 - Data CRC Error */
#define AMBA_SD_IRQ_DATA_END_BIT_ERROR  ((UINT32) 1U << 22U)   /* [22]: 1 - Data End Bit Error */
#define AMBA_SD_IRQ_CURRENT_LIMIT_ERROR ((UINT32) 1U << 23U)   /* [23]: 1 - Current Limit Error (Power Fail) */
#define AMBA_SD_IRQ_AUTO_CMD12_ERROR    ((UINT32) 1U << 24U)   /* [24]: 1 - Auto CMD12 Error */
#define AMBA_SD_IRQ_ADMA_ERROR          ((UINT32) 1U << 25U)   /* [25]: 1 - ADMA Error */

/*
 * RTSL SD Error code
 */
#define SD_ERR_0000                (SD_ERR_BASE)   /* Invalid argument */
#define SD_ERR_0001                (SD_ERR_BASE | 0x1U)
#define SD_ERR_0002                (SD_ERR_BASE | 0x2U)
#define SD_ERR_0003                (SD_ERR_BASE | 0x3U)
#define SD_ERR_0004                (SD_ERR_BASE | 0x4U)
#define SD_ERR_0005                (SD_ERR_BASE | 0x5U)
#define SD_ERR_0006                (SD_ERR_BASE | 0x6U)
#define SD_ERR_0007                (SD_ERR_BASE | 0x7U)
#define SD_ERR_0008                (SD_ERR_BASE | 0x8U)
#define SD_ERR_0009                (SD_ERR_BASE | 0x9U)
#define SD_ERR_0010                (SD_ERR_BASE | 0x10U)
#define SD_ERR_0011                (SD_ERR_BASE | 0x11U)
#define SD_ERR_0012                (SD_ERR_BASE | 0x12U)
#define SD_ERR_0013                (SD_ERR_BASE | 0x13U)
#define SD_ERR_0014                (SD_ERR_BASE | 0x14U)
#define SD_ERR_0015                (SD_ERR_BASE | 0x15U)


#define AMBA_SD_ERR_NONE              OK              /* Success */
#define AMBA_SD_ERR_INVALID_ARG       SD_ERR_0000     /* Invalid argument */
#define AMBA_SD_ERR_TIMEOUT           SD_ERR_0001     /* Timeout in SDHC I/O */
#define AMBA_SD_ERR_BADCRC            SD_ERR_0002     /* CRC error in SDHC I/O */
#define AMBA_SD_ERR_ENDBIT            SD_ERR_0003     /* Command failure in SDHC I/O */
#define AMBA_SD_ERR_INDEX             SD_ERR_0004     /* Command failure in SDHC I/O */
#define AMBA_SD_ERR_ACMD12            SD_ERR_0005     /* Auto CMD12 failure in SDHC I/O */
#define AMBA_SD_ERR_ADMA              SD_ERR_0006     /* ADMA error in SDHC I/O */
#define AMBA_SD_ERR_INVALID           SD_ERR_0007     /* Invalid cmd/arg in SDHC I/O */
#define AMBA_SD_ERR_UNUSABLE          SD_ERR_0008     /* Unusable card */
#define AMBA_SD_ERR_ISR_TIMEOUT       SD_ERR_0009     /* Can't wait SDHC ISR and timeout in sd driver. */
#define AMBA_SD_ERR_DLINE_TIMEOUT     SD_ERR_0010     /* Can't wait SDHC dat line ready and timeout in sd driver. */
#define AMBA_SD_ERR_CLINE_TIMEOUT     SD_ERR_0011     /* Can't wait SDHC cmd line ready and timeout in sd driver. */
#define AMBA_SD_ERR_NO_CARD           SD_ERR_0012
#define AMBA_SD_ERR_CHECK_PATTERN     SD_ERR_0013     /* CMD19 check pattern mismatch */
#define AMBA_SD_ERR_UNEXPECT          SD_ERR_0014     /* The response or the data is not in spec expect */
#define AMBA_SD_ERR_OTHER             SD_ERR_0015     /* For other system error */

#define AMBA_SD_PRINT_MODULE_ID       ((UINT16)(SD_ERR_BASE >> 16U))

/*
 * RTSL SD Management Structure
 */

#define    AMBA_SD_WRITE_DATA    0U
#define    AMBA_SD_READ_DATA     1U

typedef struct {
    UINT8   CmdIdx;                         /* Command Index */
    UINT8   ResponseType;                   /* Response Type */
    UINT32  Argument;                       /* Argument */

    UINT32  Response[4];                    /* Response */
    UINT32  IrqStatus;

    UINT32  BlkSize;                        /* Data block size */
    UINT32  NumBlk;                         /* number of blocks */
    void    *pDataBuf;                      /* pointer to Data Buffer */
    UINT8 DataDirection; /* Data direction, 0:Write, 1:Read */
} AMBA_SD_CMD_CONFIG_s;

typedef struct {
    AMBA_SD_CMD_CONFIG_s CmdConfig;         /* SD CMD Configurations */

    INT16   Status;                         /* SD status */
    UINT32  IntID;                          /* Interrupt ID */
    void    (*pISR)(UINT32 IntID, UINT32 UserArg);             /* pointer to the ISR */
    UINT16   Rca;                            /* Relative Card Address */
    UINT32    BusWidth;
    UINT8   SupportSdPhy;                   /* Support SD Delay Ctrl Phy*/

    AMBA_PARTITION_CONFIG_s *pSysPartConfig;    /* pointer to System partition configurations */
    AMBA_PARTITION_CONFIG_s *pUserPartConfig;   /* pointer to User Partition Configurations */

    UINT16  BstPageCount;                   /* Number of Pages for BST */
    UINT16  SysPtblPageCount;               /* Number of Pages for System Partition Table */
    UINT16  UserPtblPageCount;              /* Number of Pages for User Partition Table */
    UINT16  VendorDataPageCount;            /* Number of Pages for Vendor Specific Data */
} AMBA_RTSL_SD_CTRL_s;

typedef struct {
    UINT32 RdLatency:     2;
    UINT32 RXClkPol:      1;
    UINT32 ClkOutBypass:  1;
    UINT32 DataCmdBypass: 1;
    UINT32 SelValue:      8;
    UINT32 SbcCoreDelay:  4;
    UINT32 DinClkPol:     1;
    UINT32 Rev:          14;
} AMBA_SD_DETAIL_DELAY_s;

/*
 * Defined in AmbaRTSL_SD.c
 */
extern void (*AmbaRTSL_SdIsrCallBack)(UINT32 SdChanNo, UINT32 SdIrqStatus);
#define SD_HOST_NUM 2
extern AMBA_RTSL_SD_CTRL_s AmbaRTSL_SdCtrl[SD_HOST_NUM];

extern const UINT8 AmbaRTSL_SdFixedTuningBlkPattern[AMBA_SD_TUNING_BLOCK_BYTE_SIZE];
extern UINT8 AmbaRTSL_SdCmd19TuningBlkPattern[AMBA_SD_TUNING_BLOCK_BYTE_SIZE];
extern const UINT8 AmbaRTSL_SdEmm8bitTuningBlkPattern[AMBA_SD_TUNING_BLOCK_BYTE_SIZE * 2U];
extern UINT8 AmbaRTSL_SdEmmcCmd21TuningBlkPattern[2U * AMBA_SD_TUNING_BLOCK_BYTE_SIZE];
#define AMBA_SD_WORK_BUFFER_SIZE    (32U * 1024U)
extern UINT8 AmbaRTSL_SdWorkBuf[AMBA_NUM_SD_CHANNEL * AMBA_SD_WORK_BUFFER_SIZE];

extern AMBA_SYS_PARTITION_TABLE_s  AmbaRTSL_EmmcSysPartTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
extern AMBA_USER_PARTITION_TABLE_s AmbaRTSL_EmmcUserPartTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

UINT32 AmbaRTSL_SdGetWorkBufferSize(void);
UINT32 AmbaRTSL_SdGetDescTransferSize(void);
void AmbaRTSL_SdResetAll(UINT32 SdChanNo);
void AmbaRTSL_SdioCardIrqEnable(UINT32 SdChanNo, UINT32 Enable);
void AmbaRTSL_SdSetHostBusWidth(UINT32 SdChanNo, UINT32 BusWidth);
void AmbaRTSL_SdClockEnable(UINT32 SdChanNo, UINT32 Enable);
void AmbaRTSL_SdGetResponse48Bits(UINT32 SdChanNo, UINT32 *pResp);
void AmbaRTSL_SdGetResponse136Bits(UINT32 SdChanNo, AMBA_SD_RESPONSE_R2_s * pResp);
UINT32 AmbaRTSL_SdParseResponse(const AMBA_SD_RESPONSE_R2_s *pResponse, UINT32 StartBits, UINT32 BitsSize);
void AmbaRTSL_SdStoreCardRca(UINT32 SdChanNo, UINT16 Rca);
INT32 AmbaRTSL_SdCheckVoltageSwitch(UINT32 SdChanNo, UINT32 Level);
UINT32 AmbaRTSL_SdCardPresent(UINT32 SdChanNo);
UINT32 AmbaRTSL_SdGetWriteProtectPin(UINT32 SdChanNo);
INT32 AmbaRTSL_SdSendCmd(UINT32 SdChanNo);

UINT32 AmbaRTSL_SdInit(void);

INT32 AmbaRTSL_SdSendCMD0(UINT32 SdChanNo, UINT32 Cmd0Arg);
INT32 AmbaRTSL_SdSendCMD1(UINT32 SdChanNo, const AMBA_SD_CMD1_ARGUMENT_s * pCmd1Arg, UINT32 * pOcr);
INT32 AmbaRTSL_SdSendCMD2(UINT32 SdChanNo, AMBA_SD_INFO_CID_REG_s *pCid);
INT32 AmbaRTSL_SdSendCMD3(UINT32 SdChanNo);
INT32 AmbaRTSL_SdSendCMD6(UINT32 SdChanNo, const AMBA_SD_CMD6_ARGUMENT_s *pArgCmd6, AMBA_SD_CMD6_STATUS_s *pCmd6Status);
INT32 AmbaRTSL_SdSendCMD7(UINT32 SdChanNo, UINT16 Rca);
INT32 AmbaRTSL_SdSendCMD8(UINT32 SdChanNo, const AMBA_SD_CMD8_ARGUMENT_s *pCmd8Arg);
INT32 AmbaRTSL_SdSendCMD9(UINT32 SdChanNo);
INT32 AmbaRTSL_SdSendCMD10(UINT32 SdChanNo);
INT32 AmbaRTSL_SdSendCMD11(UINT32 SdChanNo);
INT32 AmbaRTSL_SdSendCMD12(UINT32 SdChanNo);
INT32 AmbaRTSL_SdSendCMD13(UINT32 SdChanNo, UINT32 *pStatus);
INT32 AmbaRTSL_SdSendCMD16(UINT32 SdChanNo, UINT32 BlockLength);
INT32 AmbaRTSL_SdSendCMD17(UINT32 SdChanNo, UINT32 Sector, UINT8 *pBuf);
INT32 AmbaRTSL_SdSendCMD18(UINT32 SdChanNo, UINT32 Sector, UINT32 Sectors, UINT8 *pBuf);
INT32 AmbaRTSL_SdSendCMD19(UINT32 SdChanNo, UINT8 *pBuf);
INT32 AmbaRTSL_SdSendCMD20(UINT32 SdChanNo, UINT8 SpeedClassCtrl);
INT32 AmbaRTSL_SdSendEmmcCMD21(UINT32 SdChanNo, UINT8 *pBuf, UINT32 BlockLength);
INT32 AmbaRTSL_SdSendCMD24(UINT32 SdChanNo, UINT32 Sector, UINT8 *pBuf);
INT32 AmbaRTSL_SdSendCMD25(UINT32 SdChanNo, UINT32 Sector, UINT32 Sectors, UINT8 *pBuf);
INT32 AmbaRTSL_SdSendCMD32(UINT32 SdChanNo, UINT32 StartSector);
INT32 AmbaRTSL_SdSendCMD33(UINT32 SdChanNo, UINT32 EndSector);
INT32 AmbaRTSL_SdSendCMD38(UINT32 SdChanNo);
INT32 AmbaRTSL_SdSendCMD55(UINT32 SdChanNo);

INT32 AmbaRTSL_SdSendACMD6(UINT32 SdChanNo, UINT8 BusWidth);
INT32 AmbaRTSL_SdSendACMD13(UINT32 SdChanNo, AMBA_SD_INFO_SSR_REG_s *pSSR);
INT32 AmbaRTSL_SdSendACMD22(UINT32 SdChanNo, UINT32 Blocks);
INT32 AmbaRTSL_SdSendACMD23(UINT32 SdChanNo, UINT32 Blocks);
INT32 AmbaRTSL_SdSendACMD41(UINT32 SdChanNo, const AMBA_SD_ACMD41_ARGUMENT_s *pAcmd41Arg, UINT32 *pOcr);
INT32 AmbaRTSL_SdSendACMD42(UINT32 SdChanNo, UINT8 SetCardDetect);
INT32 AmbaRTSL_SdSendACMD51(UINT32 SdChanNo, AMBA_SD_INFO_SCR_REG_s *pScr);


INT32 AmbaRTSL_SdSendCMD5(UINT32 SdChanNo, const AMBA_SDIO_CMD5_ARGUMENT_s *pSDIOcmd5Arg);
INT32 AmbaRTSL_SdSendCMD52(UINT32 SdChanNo, const AMBA_SDIO_CMD52_ARGUMENT_s* pArgCmd52);
INT32 AmbaRTSL_SdSendCMD53(UINT32 SdChanNo, const AMBA_SDIO_CMD53_ARGUMENT_s* pArgCmd53, UINT16 BlockSize, UINT8* pDataBuf);

INT32 AmbaEMMC_HS200SupportChk(UINT32 SdChanNo);
void AmbaRTSL_GeteMMCSpeedMode(AMBA_SD_EMMC_CMD6_ARGUMENT_s *pArgCmd6, const AMBA_SD_INFO_EXT_CSD_REG_s *pExtCsd);
void AmbaRTSL_GeteMMCDriveStrength(AMBA_SD_EMMC_CMD6_ARGUMENT_s *pArgCmd6, const AMBA_SD_INFO_EXT_CSD_REG_s *pExtCsd);
INT32 AmbaRTSL_SdSendEmmcCMD6(UINT32 SdChanNo, const AMBA_SD_EMMC_CMD6_ARGUMENT_s * pArgCmd6);
INT32 AmbaRTSL_SdSendEmmcCMD8(UINT32 SdChanNo, AMBA_SD_INFO_EXT_CSD_REG_s * pExtCsd);
INT32 AmbaRTSL_SdSendCMD2Done(UINT32 SdChanNo, AMBA_SD_INFO_CID_REG_s * pCid);
INT32 AmbaRTSL_SdSendCMD9Done(UINT32 SdChanNo, AMBA_SD_INFO_CSD_REG_s * pCsd);

INT32 AmbaRTSL_SdErase(UINT32 SdChanNo, UINT32 StartBlkAddr, UINT32 EndBlkAddr);
INT32 AmbaRTSL_SdSetWriteProtect(UINT32 SdChanNo, UINT32 StartBlkAddr, UINT32 EndBlkAddr);
INT32 AmbaRTSL_SdClearWriteProtect(UINT32 SdChanNo, UINT32 StartBlkAddr, UINT32 EndBlkAddr);
UINT16 AmbaRTSL_SdGetPostscaler(UINT32 SdChanNo);
void AmbaRTSL_SdSetPostscaler(UINT32 SdChanNo, UINT16 Postscaler);

INT32 AmbaRTSL_SdReadSector(UINT32 SdChanNo, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig);
INT32 AmbaRTSL_SdWriteSector(UINT32 SdChanNo, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig);

INT32 AmbaRTSL_SdGetResponse(UINT32 SdChanNo, UINT32 OpCode, UINT32 * pResp);

/*
 * Defined in AmbaRTSL_SD_Ctrl.c
 */

#define     AMBA_SD_PHY_DIN_CLK_POL      0U
#define     AMBA_SD_PHY_CLK_OUT_BYPASS   1U
#define     AMBA_SD_PHY_RESET            2U
#define     AMBA_SD_PHY_CLK270_ALONE     3U
#define     AMBA_SD_PHY_RX_CLK_POL       4U
#define     AMBA_SD_PHY_DATA_CMD_BYPASS  5U
#define     AMBA_SD_PHY_DLL_BYPASS       6U


typedef enum {
    AMBA_SD_PHY_SBC_AUTO_COARSE_ADJ = 0,
    AMBA_SD_PHY_SBC_POWER_DOWN_SHIFT,
    AMBA_SD_PHY_SBC_FORCE_LOCK_INTERCYC,
    AMBA_SD_PHY_SBC_FORCE_LOCK_VSHIFT,
    AMBA_SD_PHY_SBC_SEL_FSM,
    AMBA_SD_PHY_SBC_BYPASS_HIGH,
    AMBA_SD_PHY_SBC_SEL_PDBB,
    AMBA_SD_PHY_SBC_SEL_INTERCLOCK,
    AMBA_SD_PHY_SBC_LOCK_RANGE,
    AMBA_SD_PHY_SBC_COARSE_DELAY,
    AMBA_SD_PHY_SBC_ENABLEDLL
} AMBA_SD_PHY_SBC_e;

#define     AMBA_SD_PHY_SEL0     0U
#define     AMBA_SD_PHY_SEL1     1U
#define     AMBA_SD_PHY_SEL2     2U
#define     AMBA_SD_PHY_SEL3     3U

INT32 AmbaRTSL_SdWaitCmdDone(UINT32 SdChanNo, UINT32 Flag);
void AmbaRTSL_SDPhySetConfig(UINT32 SdChanNo, UINT32 Type, UINT32 Value);
UINT8 AmbaRTSL_SDPhyGetConfig(UINT32 SdChanNo, UINT32 Type);
void AmbaRTSL_SDPhySetDelayChainSel(UINT32 SdChanNo, UINT8 Value);
UINT8 AmbaRTSL_SDPhyGetDelayChainSel(UINT32 SdChanNo);
void AmbaRTSL_SDPhySetDutySelect(UINT32 SdChanNo, UINT8 Value);
UINT8 AmbaRTSL_SDPhyGetDutySelect(UINT32 SdChanNo);
void AmbaRTSL_SDPhySetSharedBusCtrl(UINT32 SdChanNo, AMBA_SD_PHY_SBC_e Type, UINT8 Value);
UINT32 AmbaRTSL_SDPhyGetSharedBusCtrl(UINT32 SdChanNo, AMBA_SD_PHY_SBC_e Type);
void AmbaRTSL_SDPhySetSelect(UINT32 SdChanNo, UINT32 Type, UINT8 Value);
UINT8 AmbaRTSL_SDPhyGetSelect(UINT32 SdChanNo, UINT32 Type);
UINT8 AmbaRTSL_SDPhyGetVshift(UINT32 SdChanNo);
void AmbaRTSL_SDPhyReset(UINT32 SdChanNo);
void AmbaRTSL_SDPhySelReset(UINT32 SdChanNo);
void AmbaRTSL_SDSetRdLatencyCtrl(UINT32 SdChanNo, UINT8 DelayCycle);
UINT32 AmbaRTSL_SDGetRdLatencyCtrl(UINT32 SdChanNo);
void AmbaRTSL_SDPhyDisable(UINT32 SdChanNo);
UINT32 AmbaRTSL_SdConfig(UINT32 SdChanNo, const AMBA_SD_CONFIG_s * pSdConfig);
UINT32 AmbaRTSL_SdCardInit(UINT32 SdChanNo);
INT32 AmbaRTSL_SetDelayConfig(UINT32 SdChanNo, UINT32 DetailDelay);
UINT32 AmbaRTSL_SdGetLowDataPinStatus(UINT32 SdChanNo);
INT32 AmbaRTSL_SetPhyDelay(UINT32 SdChanNo, UINT32 ClkBypass, UINT32 RxClkPol, UINT32 SbcCore, UINT32 SelValue, UINT32 DinClkPol, UINT32 CmdBypass);
void AmbaRTSL_SdDelayCtrlEnable(INT32 Enable);
INT32 AmbaRTSL_SdSetDriveStrengthAll(UINT32 SdChanNo, const AMBA_SD_SETTING_s * pSdSetting);

/*
 * Defined in AmbaRTSL_EMMC_Partition.c
 */
INT32 AmbaRTSL_EmmcInitPtbBbt(void);
INT32 AmbaRTSL_EmmcReadSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable);
INT32 AmbaRTSL_EmmcWriteSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable);
INT32 AmbaRTSL_EmmcReadUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo);
INT32 AmbaRTSL_EmmcWriteUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo);

#if defined(CONFIG_MUTI_BOOT_DEVICE)
UINT32 AmbaRTSL_EmmcCreateSlaveUserPTB(const AMBA_PARTITION_CONFIG_s *pUserPartConfig, AMBA_USER_PARTITION_TABLE_s * pUserPartTable, UINT32 *pEndBlock);
#endif
UINT32 AmbaRTSL_EmmcCreateExtUserPTB(const AMBA_PARTITION_CONFIG_s *pUserPartConfig, AMBA_USER_PARTITION_TABLE_s * pUserPartTable, UINT32 StartBlk);

INT32 AmbaRTSL_EmmcGetActivePtbNo(UINT32 * pPTBActive);
INT32 AmbaRTSL_EmmcSetActivePtbNo(UINT32 PtbNo, UINT8 Active);

AMBA_PARTITION_ENTRY_s *AmbaRTSL_EmmcGetPartEntry(UINT32 PartFlag, UINT32 PartID);
INT32 AmbaRTSL_EmmcReadPartitionInfo(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo);
INT32 AmbaRTSL_EmmcReadPartition(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf);
INT32 AmbaRTSL_EmmcWritePartition(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf);
INT32 AmbaRTSL_EmmcErasePartition(UINT32 PartFlag, UINT32 PartID);

UINT32 AmbaRTSL_EmmcIsBldMagicCodeSet(void);
UINT32 AmbaRTSL_EmmcSetBldMagicCode(void);
UINT32 AmbaRTSL_EmmcEraseBldMagicCode(void);
UINT32 AmbaRTSL_EmmcReadBldMagicCode(void);
void AmbaRTSL_EmmcSetWritePtbFlag(UINT32 Flag);
UINT8 AmbaRTSL_EmmcGetBootChannel(void);
/*
 * Defined in AmbaRTSL_EMMC_Partition.c
 */
extern AMBA_SYS_PARTITION_TABLE_s  *pAmbaRTSL_EmmcSysPartTable ;
extern AMBA_USER_PARTITION_TABLE_s *pAmbaRTSL_EmmcUserPartTable ;

#endif  /* AMBA_RTSL_SD_H */
