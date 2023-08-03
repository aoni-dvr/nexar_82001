/**
 *  @file AmbaB8_Communicate.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Definitions & Constants for B6 Communication APIs
 *
 */

#ifndef AMBA_B8_COMMUNICATE_H
#define AMBA_B8_COMMUNICATE_H

#define B8_REG_READ                 0
#define B8_REG_WRITE                1

#define B8_REG_ADDR_NO_INCREMENT    0
#define B8_REG_ADDR_INCREMENT       1

#define B8_DATA_WIDTH_8BIT          0U  /* Byte */
#define B8_DATA_WIDTH_16BIT         1U  /* Halfword */
#define B8_DATA_WIDTH_32BIT         2U  /* Word */

#define B8_PWMENC_POLLING_START     6U
#define B8_PWMENC_POLLING_TIMEOUT   3000U

#define B8_PWMENC_CHANNEL0          0U
#define B8_PWMENC_CHANNEL1          1U
#define B8_PWMENC_CHANNEL2          2U
#define B8_PWMENC_CHANNEL3          3U
#define B8_PWMENC_CHANNEL12         12U
#define B8_PWMENC_CHANNEL13         13U

typedef struct {
    UINT32  IsAPB:          1;      /* [0]: 0 = AHB, 1 = APB */
    UINT32  DataWidth:      2;      /* [2:1]: Transfer Size: 0:Byte, 1:Halfword, 2:Word, 3: Special command */
    UINT32  DataSize:       7;      /* [9:3]: Transfer byte count by size */
    UINT32  AddrInc:        1;      /* [10]: 0 = Fixed address 1 = increment */
    UINT32  ReadOrWrite:    1;      /* [11]: 0 = Read, 1 = Write */
    UINT32  ChipID:         4;      /* [15:12]: Chip ID */
    UINT32  Reserved:       16;     /* for misrac workaround. B6 command only occupies 16 bits */
} B8_COMMAND_s;

typedef struct {
    UINT8 ReplayTimes;
    UINT8 ChunkSize;    /* 0x0: 4 bytes, 0x1: 8 bytes, 0x2: 16 bytes, 0x3: 32 bytes, 0x4: 64 bytes */
    UINT8 CreditPwr;    /* 0: Credit Number = 1, 1: Credit Number = 2, 2: Credit Number = 4, 3: Credit Number = 8, 4: Credit Number =16 */
} B8_COMM_CONFIG_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB8_Communicate.c
\*---------------------------------------------------------------------------*/
UINT32 AmbaB8_CommInit(void);
UINT32 AmbaB8_CommReset(UINT32 ChipID);
UINT32 AmbaB8_CommConfig(UINT32 ChipID, const B8_COMM_CONFIG_s *pConfig);
UINT32 AmbaB8_CommDisableAck(UINT32 ChipID, UINT32 IsDisable);

UINT32 AmbaB8_RegWrite(UINT32 ChipID, const volatile void* pReg, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, const void *pTxDataBuf);
UINT32 AmbaB8_RegRead(UINT32 ChipID, const volatile void* pReg, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, void *pRxDataBuf);
UINT32 AmbaB8_RegMultiRead(UINT32 ChipID, const volatile void* pReg, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, void *pRxDataBuf[4]);

/* remove after */
UINT32 AmbaB8_RegWriteU32(UINT32 ChipID, UINT32 RegAddr, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, const void *pTxDataBuf);
UINT32 AmbaB8_RegReadU32(UINT32 ChipID, UINT32 RegAddr, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, void *pRxDataBuf);
UINT32 AmbaB8_RegMultiReadU32(UINT32 ChipID, UINT32 RegAddr, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, void **pRxDataBuf);

#endif /* AMBA_B8_COMMUNICATE_H */
