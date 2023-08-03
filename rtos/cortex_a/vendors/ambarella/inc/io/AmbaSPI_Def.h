/**
 *  @file AmbaSPI_Def.h
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
 *  @details Common Definitions & Constants for SPI APIs
 *
 */

#ifndef AMBA_SPI_DEF_H
#define AMBA_SPI_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBA_SPI_PRIV_H
#include "AmbaSPI_Priv.h"
#endif

#define SPI_ERR_0000            (SPI_ERR_BASE)          /* Invalid argument */
#define SPI_ERR_0001            (SPI_ERR_BASE + 0x1U)   /* Unable to do concurrency protection */
#define SPI_ERR_0002            (SPI_ERR_BASE + 0X2U)   /* Invalid configuration */
#define SPI_ERR_0003            (SPI_ERR_BASE + 0x3U)   /* Service unavailable */
#define SPI_ERR_0004            (SPI_ERR_BASE + 0X4U)   /* Time-out */
#define SPI_ERR_0005            (SPI_ERR_BASE + 0x5U)
#define SPI_ERR_0006            (SPI_ERR_BASE + 0X6U)
#define SPI_ERR_00FF            (SPI_ERR_BASE + 0XFFU)  /* Unexpected error */

/* SPI error values */
#define SPI_ERR_NONE            0U
#define SPI_ERR_ARG             SPI_ERR_0000
#define SPI_ERR_MUTEX           SPI_ERR_0001
#define SPI_ERR_CFG             SPI_ERR_0002
#define SPI_ERR_UNAVAIL         SPI_ERR_0003
#define SPI_ERR_TMO             SPI_ERR_0004

#define SPI_ERR_DEV_CTL         SPI_ERR_0005
#define SPI_ERR_OPEN_FILE       SPI_ERR_0006
#define SPI_ERR_UNEXPECTED      SPI_ERR_00FF

#define SPI_STATUS_IDLE         (0U)
#define SPI_STATUS_BUSY         (1U)
#define SPI_STATUS_ERROR        (2U)

/* SPI mode number: The combinations of clock polarity and phases */
#define SPI_CPOL_LO_CPHA_LO     (0U)    /* the leading (first) clock edge is rising edge, and data is sampled on the leading (first) clock edge */
#define SPI_CPOL_LO_CPHA_HI     (1U)    /* the leading (first) clock edge is rising edge, and data is sampled on the trailing (second) clock edge */
#define SPI_CPOL_HI_CPHA_LO     (2U)    /* the leading (first) clock edge is fallng edge, and data is sampled on the leading (first) clock edge */
#define SPI_CPOL_HI_CPHA_HI     (3U)    /* the leading (first) clock edge is fallng edge, and data is sampled on the trailing (second) clock edge */

/* SPI slave select polarity */
#define SPI_CS_ACTIVE_LO        (0U)    /* Slave select is active low */
#define SPI_CS_ACTIVE_HI        (1U)    /* Slave select is active high */

/* SPI transfer order */
#define SPI_XFER_MSB_FIRST      (0U)    /* Bit transfer order is from MSB to LSB (default) */
#define SPI_XFER_LSB_FIRST      (1U)    /* Bit transfer order is from LSB to MSB */

/* SPI data frame size */
#define SPI_DFS_LSB_4BIT        (4U)
#define SPI_DFS_LSB_5BIT        (5U)
#define SPI_DFS_LSB_6BIT        (6U)
#define SPI_DFS_LSB_7BIT        (7U)
#define SPI_DFS_LSB_8BIT        (8U)
#define SPI_DFS_LSB_9BIT        (9U)
#define SPI_DFS_LSB_10BIT       (10U)
#define SPI_DFS_LSB_11BIT       (11U)
#define SPI_DFS_LSB_12BIT       (12U)
#define SPI_DFS_LSB_13BIT       (13U)
#define SPI_DFS_LSB_14BIT       (14U)
#define SPI_DFS_LSB_15BIT       (15U)
#define SPI_DFS_LSB_16BIT       (16U)

/* Legacy definitions */
#define AMBA_SPI_CPOL_LOW_CPHA_LOW          SPI_CPOL_LO_CPHA_LO
#define AMBA_SPI_CPOL_LOW_CPHA_HIGH         SPI_CPOL_LO_CPHA_HI
#define AMBA_SPI_CPOL_HIGH_CPHA_LOW         SPI_CPOL_HI_CPHA_LO
#define AMBA_SPI_CPOL_HIGH_CPHA_HIGH        SPI_CPOL_HI_CPHA_HI

#define AMBA_SPI_CHIP_SELECT_ACTIVE_LOW     SPI_CS_ACTIVE_LO
#define AMBA_SPI_CHIP_SELECT_ACTIVE_HIGH    SPI_CS_ACTIVE_HI

#define AMBA_SPI_TRANSFER_MSB_FIRST         SPI_XFER_MSB_FIRST
#define AMBA_SPI_TRANSFER_LSB_FIRST         SPI_XFER_LSB_FIRST

typedef struct {
    UINT32  BaudRate;       /* Desired baudrate */
    UINT32  CsPolarity;     /* Slave select polarity */
    UINT32  ClkMode;        /* SPI Protocol mode */
    UINT32  ClkStretch;     /* Stretch the duration (us) from the active edge of chip select to the first edge of clock */
    UINT32  DataFrameSize;  /* Data Frame Size in Bits */
    UINT32  FrameBitOrder;  /* Bit transfer order */
} AMBA_SPI_CONFIG_s;

#endif /* AMBA_SPI_DEF_H */
