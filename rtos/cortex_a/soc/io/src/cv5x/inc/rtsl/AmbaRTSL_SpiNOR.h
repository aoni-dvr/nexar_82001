/**
 *  @file AmbaRTSL_SPINOR.h
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
 *  @details Definitions & Constants for NOR Controller RTSL APIs
 *
 */

#ifndef AMBA_RTSL_NOR_SPI_H
#define AMBA_RTSL_NOR_SPI_H

#ifndef AMBA_NOR_SPI_DEF_H
#include "AmbaSpiNOR_Def.h"
#endif

/*
 * NOR operations error code definitions.
 */
#define NOR_OP_DRIVER_ER               (-1)
#define NOR_OP_ERASE_ER                (-2)
#define NOR_OP_PROG_ER                 (-3)
#define NOR_OP_READ_ER                 (-4)
#define NOR_OP_WP_ER                   (-5)
#define NOR_OP_NOT_READY_ER            (-6)

#define AMBA_SPINOR_CLK_FROM_ENET   (0U) /* Source Clock From Enernet  */
#define AMBA_SPINOR_CLK_FROM_CORE   (1U) /* Source Clock From CoreClock */
#define AMBA_SPINOR_CLK_FROM_SPI0   (2U) /* Source Clock From Spi0 */
#define AMBA_SPINOR_CLK_FROM_CLKREF (3U) /* Source Clock From ClockRef */

#define AMBA_NORSPI_TXBUF_OFFSET (0x100U)
#define AMBA_NORSPI_RXBUF_OFFSET (0x200U)

#define AMBA_SPINOR_FIFO_SIZE (AMBA_NORSPI_RXBUF_OFFSET - AMBA_NORSPI_TXBUF_OFFSET)

/*
 * Defined in AmbaRTSL_NOR.c (RTSL for NOR flash)
 */
extern void (*AmbaRTSL_NORIsrDoneCallBack)(void);

void AmbaRTSL_AllocateNorSpiDMAChan(UINT32 RxChannelNum, UINT32 TxChannelNum);
void AmbaRTSL_NorSpiStop(void);
void AmbaRTSL_NorInit(void);
void AmbaRTSL_NorReset(void);
void AmbaRTSL_NorResetEnable(void);
UINT32 AmbaRTSL_SpiNORConfig(AMBA_NOR_SPI_CONFIG_s *pNorSpiConfig);
void AmbaRTSL_SpiNORDeviceReset(void);

void AmbaRTSL_NorReadRegSetup(UINT32 RegAddr);
void AmbaRTSL_NorWriteRegStetup(UINT32 RegAddr, UINT32 ByteCount, UINT8 *pDataBuf);

void AmbaRTSL_NorSpiWriteSetup(UINT32 Offset, UINT32 ByteCount, const UINT8 *pDataBuf, UINT8 EnableDMA);
void AmbaRTSL_NorSpiWriteDMA(UINT32 Offset, UINT32 ByteCount, const UINT8 *pDataBuf);
void AmbaRTSL_NorSpiReadSetup(UINT32 From, UINT32 ByteCount, UINT8 EnableDMA);
void AmbaRTSL_NorSpiReadDMA(UINT32 From, UINT32 ByteCount, UINT8 *pDataBuf);
void AmbaRTSL_ResetFIFO(void);
void AmbaRTSL_NorEraseBlock(UINT32 Offset);
void AmbaRTSL_NorSectorErase(UINT32 Offset);
void AmbaRTSL_NorReadFIFO(UINT32 RxDataSize, UINT8 *pRxDataBuf);

void AmbaRTSL_NorWriteEnableStart(void);
void AmbaRTSL_NorWriteDisable(void);

void AmbaRTSL_NorClearStatus(void);
void AmbaRTSL_NorReadStatusCmd(UINT8 StatusNum);
void AmbaRTSL_NorSetExtAddr(void);
void AmbaRTSL_NorWriteVcrSetup(UINT32 RegAddr, const UINT8 *pDataBuf);
void AmbaRTSL_NorReadIDCmd(void);
void AmbaRTSL_NorReadSFDP(void);
void AmbaRTSL_NorSetSPIClk(UINT32 Frequency);
UINT32 AmbaRTSL_NorSpiConfig(AMBA_NOR_SPI_CONFIG_s *pNorSpiConfig);
void AmbaRTSL_NorSetSPISetting(const AMBA_SERIAL_SPI_CONFIG_s *SpiConfig);
void AmbaRTSL_NorEraseChip(void);
UINT32 AmbaRTSL_NorErase(UINT32 Offset, UINT32 ByteCount);
UINT32 AmbaRTSL_NOR_Readbyte(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf);
UINT32 AmbaRTSL_NorProgram(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf);
void AmbaRTSL_NorWaitInt(void);
void AmbaRTSL_NorEnterQUADStart(UINT8 ByteCount, const UINT8 *pDataBuf);
void AmbaRTSL_NorExitQUADStart(UINT8 ByteCount, const UINT8 *pDataBuf);
void AmbaRTSL_NorReadFlagStatusCmd(void);
void AmbaRTSL_NorReadQuadModeCmd(void);
/*
 * Defined in AmbaRTSL_NOR_SPI_Partition.c
 */
UINT32 AmbaRTSL_SpiNorCreatePTB(void);
UINT32 AmbaRTSL_SpiNorCreateExtUserPTB(UINT32 StartBlk);
UINT32 AmbaRTSL_NorSpiInitPtbBbt(void);
UINT32 AmbaRTSL_NorSpiReadSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable);
UINT32 AmbaRTSL_NorSpiWriteSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable);
UINT32 AmbaRTSL_NorSpiReadUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo);
UINT32 AmbaRTSL_NorSpiWriteUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo);
UINT32 AmbaRTSL_NorSpiGetActivePtbNo(UINT32 * pPTBActive);
UINT32 AmbaRTSL_NorSpiSetActivePtbNo(UINT32 PtbNo, UINT8 Active);

AMBA_PARTITION_ENTRY_s *AmbaRTSL_NorSpiGetPartEntry(UINT32 PartFlag, UINT32 PartID);
UINT32 AmbaRTSL_NorSpiReadPartitionInfo(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo);
UINT32 AmbaRTSL_NorSpiReadPartition(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf);
UINT32 AmbaRTSL_NorSpiWritePartition(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf);
UINT32 AmbaRTSL_NorSpiErasePartition(UINT32 PartFlag, UINT32 PartID);

UINT32 AmbaRTSL_NorSpiEraseBldMagicCode(void);
UINT32 AmbaRTSL_NorSpiReadBldMagicCode(void);

UINT32 AmbaRTSL_NorSpiSetBldMagicCode(void);
UINT32 AmbaRTSL_NorSpiIsBldMagicCodeSet(void);

void AmbaRTSL_NorSpiSetWritePtbFlag(UINT32 Flag);

#endif /* _AMBA_RTSL_NAND_H_ */
