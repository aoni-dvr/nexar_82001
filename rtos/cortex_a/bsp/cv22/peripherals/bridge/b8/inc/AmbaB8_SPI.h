/**
 *  @file AmbaB8_SPI.h
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
 *  @details Definitions & Constants for B6 SPI APIs
 *
 */

#ifndef AMBA_B8_SPI_H
#define AMBA_B8_SPI_H

/* SPI protocol modes */
#define B8_SPI_CPOL_LOW_CPHA_LOW          0U           /* Motorola SPI, clock polarity = low, clock phase = low */
#define B8_SPI_CPOL_LOW_CPHA_HIGH         1U           /* Motorola SPI, clock polarity = low, clock phase = high */
#define B8_SPI_CPOL_HIGH_CPHA_LOW         2U           /* Motorola SPI, clock polarity = high, clock phase = low */
#define B8_SPI_CPOL_HIGH_CPHA_HIGH        3U           /* Motorola SPI, clock polarity = high, clock phase = high */
#define B8_SPI_PROTOCOL_MODE_e            4U

/* SPI slave select polarity */
#define B8_SPI_CHIP_SEL_ACTIVE_LOW     0U           /* Slave select is active low */
#define B8_SPI_CHIP_SEL_ACTIVE_HIGH    1U           /* Slave select is active high */
#define B8_SPI_CHIP_SEL_POL_e          2U

typedef struct {
    UINT32                          ProtocolMode;   /* SPI Protocol mode */
    UINT32                          CsPolarity;     /* Slave select polarity */
    UINT8                           DataFrameSize;  /* Data frame size in number of bits */
    UINT32                          BaudRate;       /* Transfer BaudRate in Hz */
    UINT32                          NumDataFrames;  /* Number of data frames for read-only operation mode */
    UINT8                           SlaveID;        /* Slave ID (For MASTER only) */
} B8_SPI_CONFIG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaB8_SPI.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_SpiInit(void);
UINT32 AmbaB8_SpiTransfer(UINT32 ChipID, const B8_SPI_CONFIG_s *pSpiConfig, UINT32 DataSize,
                          UINT32 *pTxDataBuf, UINT32 *pRxDataBuf);

#endif /* AMBA_B8_SPI_H */
