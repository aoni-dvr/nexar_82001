/**
 *  @file AmbaDMA_Def.h
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
 *  @details Definitions & Constants for DMA APIs.
 *
 */

#ifndef AMBA_DMA_DEF_H
#define AMBA_DMA_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBA_DMA_PRIV_H
#include "AmbaDMA_Priv.h"
#endif

#define DMA_ERR_0000            (DMA_ERR_BASE)              /* Invalid argument */
#define DMA_ERR_0001            (DMA_ERR_BASE + 0X1U)       /* Unable to do concurrency protection */
#define DMA_ERR_0002            (DMA_ERR_BASE + 0X2U)       /* Timeout occurred */
#define DMA_ERR_00FF            (DMA_ERR_BASE + 0XFFU)      /* Unexpected error */

#define DMA_ERR_NONE            (OK)
#define DMA_ERR_ARG             DMA_ERR_0000
#define DMA_ERR_MUTEX           DMA_ERR_0001
#define DMA_ERR_TMO             DMA_ERR_0002
#define DMA_ERR_UNEXPECTED      DMA_ERR_00FF

#define MAX_DMA_DATA_TRANSFER_SIZE      ((4U*1024U*1024U) - 1U)      /* maximum DMA data transfer size */

/*
 * Definitions of AMBA DMA Bus Data Transfer Size.
 */
#define DMA_BUS_DATA_1BYTE              (0U)  /* Bus Transfer Size = 1 Byte */
#define DMA_BUS_DATA_2BYTE              (1U)  /* Bus Transfer Size = Halfword (2 Byte) */
#define DMA_BUS_DATA_4BYTE              (2U)  /* Bus Transfer Size = 1 Word (4 Byte) */
#define DMA_BUS_DATA_8BYTE              (3U)  /* Bus Transfer Size = Double word (8 Byte) */

/*
 * Definitions of AMBA DMA Bus Request Size for Burst Operation.
 */
#define DMA_BUS_BLOCK_8BYTE             (0U)
#define DMA_BUS_BLOCK_16BYTE            (1U)
#define DMA_BUS_BLOCK_32BYTE            (2U)
#define DMA_BUS_BLOCK_64BYTE            (3U)
#define DMA_BUS_BLOCK_128BYTE           (4U)
#define DMA_BUS_BLOCK_256BYTE           (5U)
#define DMA_BUS_BLOCK_512BYTE           (6U)
#define DMA_BUS_BLOCK_1024KBYTE         (7U)

/* Legacy definitions */
#define AMBA_DMA_BUS_DATA_1BYTE         DMA_BUS_DATA_1BYTE
#define AMBA_DMA_BUS_DATA_2BYTE         DMA_BUS_DATA_2BYTE
#define AMBA_DMA_BUS_DATA_4BYTE         DMA_BUS_DATA_4BYTE
#define AMBA_DMA_BUS_DATA_8BYTE         DMA_BUS_DATA_8BYTE

#define AMBA_DMA_BUS_BLOCK_8BYTE        DMA_BUS_BLOCK_8BYTE
#define AMBA_DMA_BUS_BLOCK_16BYTE       DMA_BUS_BLOCK_16BYTE
#define AMBA_DMA_BUS_BLOCK_32BYTE       DMA_BUS_BLOCK_32BYTE
#define AMBA_DMA_BUS_BLOCK_64BYTE       DMA_BUS_BLOCK_64BYTE
#define AMBA_DMA_BUS_BLOCK_128BYTE      DMA_BUS_BLOCK_128BYTE
#define AMBA_DMA_BUS_BLOCK_256BYTE      DMA_BUS_BLOCK_256BYTE
#define AMBA_DMA_BUS_BLOCK_512BYTE      DMA_BUS_BLOCK_512BYTE
#define AMBA_DMA_BUS_BLOCK_1024KBYTE    DMA_BUS_BLOCK_1024KBYTE

/*
 * Definitions of DMA descriptor control
 */
typedef struct {
    UINT32  StopOnError:        1;  /* [0]: 1 = DMA channel will be disabled when an error occurs */
    UINT32  IrqOnError:         1;  /* [1]: 1 = DMA channel will signal an interrupt when an error occurs */
    UINT32  IrqOnDone:          1;  /* [2]: 1 = DMA channel will signal an interrupt if operation ends without an error */
    UINT32  Reserved0:          13; /* [15:3]: Reserved */
    UINT32  BusBlockSize:       3;  /* [18:16]: Bus Transaction Block size */
    UINT32  BusDataSize:        2;  /* [20:19]: Bus Data Transfer size */
    UINT32  NoBusAddrInc:       1;  /* [21]: 1 = No bus address increment during DMA operation */
    UINT32  ReadMem:            1;  /* [22]: 1 = DMA read from memory, 0=DMA read from AHB */
    UINT32  WriteMem:           1;  /* [23]: 1 = DMA write memory, 0=DMA write to AHB */
    UINT32  EndOfChain:         1;  /* [24]: 1 = End of chain flag. DMA will stop after this descriptor */
    UINT32  Reserved1:          7;  /* [31:25]: Reserved */
} AMBA_DMA_DESC_CTRL_s;

/*
 * Definitions of DMA descriptor format
 */
typedef struct {
    UINT32  PrivData[4];            /* Private data reserved for DMA controller */
    UINT32  DataSize;               /* Transfer byte count, max value = (4MB - 1) bytes */
    AMBA_DMA_DESC_CTRL_s  Ctrl;     /* Descriptor's attribute */
    void    *pSrcAddr;              /* pointer to the source */
    void    *pDstAddr;              /* pointer to the destination */
    void    *pNextDesc;             /* pointer to the next descriptor */
    UINT32  *pStatus;               /* pointer to status report */
} AMBA_DMA_DESC_s;

#endif /* AMBA_DMA_DEF_H */
