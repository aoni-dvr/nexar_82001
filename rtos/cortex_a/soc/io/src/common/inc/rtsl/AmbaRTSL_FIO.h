/**
 *  @file AmbaRTSL_FIO.h
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
 *  @details Definitions & Constants for FIO Controller RTSL APIs
 *
 */

#ifndef AMBA_RTSL_FIO_H
#define AMBA_RTSL_FIO_H

#ifndef AMBA_TYPES_H
#include "AmbaDMA_Def.h"
#endif

/*
 * RTSL FIO Management Structure
 */
typedef struct {
    UINT32  MainByteCount;      /* main area Byte Count */
    UINT8   *pMainBuf;          /* pointer to the Buffer for Main area */
    UINT32  SpareByteCount;     /* spare area Byte Count */
    UINT8   *pSpareBuf;         /* pointer to the Buffer for Spare area */
} AMBA_RTSL_FDMA_CTRL_s;

typedef struct {
    UINT32  FdmaMainStatus;
    UINT32  FdmaSpareStatus;
    UINT32  FioDmaStatus;
    UINT32  EccStatus;
    UINT32  IrqStatus;
    UINT8   *pWorkBufMain;      /* pointer to Internal Work Buffer for Main area (8-Byte alignment) */
    UINT8   *pWorkBufSpare;     /* pointer to Internal Work Buffer for Spare area (8-Byte alignment) */

    AMBA_RTSL_FDMA_CTRL_s FdmaCtrl;
} AMBA_RTSL_FIO_CTRL_s;

/*
 * Defined in AmbaRTSL_FIO.c
 */
extern AMBA_RTSL_FIO_CTRL_s AmbaRTSL_FioCtrl;

extern void (*AmbaRTSL_FioNandCmdIsrCallBack)(void);

void AmbaRTSL_FioInit(void);
void AmbaRTSL_FioInitInt(void);
void AmbaRTSL_FioReset(void);

UINT32 AmbaRTSL_FioIsRandomReadMode(void);

UINT32 AmbaRTSL_FdmaRead(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf);
UINT32 AmbaRTSL_FdmaWrite(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf);

INT32 AmbaRTSL_FioCheckIrqStatus(UINT32 ErrorBitMask);
INT32 AmbaRTSL_FioCheckEccStatus(void);
void AmbaRTSL_FdmaGetStatus(void);

#endif  /* AMBA_RTSL_FIO_H */
