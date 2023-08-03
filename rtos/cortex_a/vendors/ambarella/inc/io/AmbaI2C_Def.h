/**
 *  @file AmbaI2C_Def.h
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
 *  @details Definitions & Constants for I2C Controller APIs
 *
 */

#ifndef AMBA_I2C_DEF_H
#define AMBA_I2C_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBA_I2C_PRIV_H
#include "AmbaI2C_Priv.h"
#endif

#define I2C_ERR_0000            (I2C_ERR_BASE)              /* Invalid argument */
#define I2C_ERR_0001            (I2C_ERR_BASE + 0X1U)       /* Unable to do concurrency protection */
#define I2C_ERR_0002            (I2C_ERR_BASE + 0X2U)       /* Timeout occurred */
#define I2C_ERR_0003            (I2C_ERR_BASE + 0X3U)       /* Slave responds NACK before all the data transmitted */
#define I2C_ERR_00FF            (I2C_ERR_BASE + 0XFFU)      /* Unexpected error */

/* I2C error values */
#define I2C_ERR_NONE            (OK)
#define I2C_ERR_ARG             I2C_ERR_0000
#define I2C_ERR_MUTEX           I2C_ERR_0001
#define I2C_ERR_TIMEOUT         I2C_ERR_0002
#define I2C_ERR_NACK            I2C_ERR_0003
#define I2C_ERR_UNEXPECTED      I2C_ERR_00FF

/* I2C bus speed */
/* The supported maximum I2C bus speed should refer to datasheet */
/* And please refer to System Hardware Application Note for the clock measurement condition */
#define I2C_SPEED_STANDARD          (0U)    /* Predefined speed. It is about 0~100Kbps. */
#define I2C_SPEED_FAST              (1U)    /* Predefined speed. It is about 100~400Kbps. */
#define I2C_SPEED_FAST_PLUS         (2U)    /* Predefined speed. It is about 400~1000Kbps. For testing purpose only. */
#define I2C_SPEED_HIGH              (3U)    /* Predefined speed. It is about 1000~3400Kbps. For testing purpose only. */
#define NUM_I2C_SPEED               (4U)

/* Legacy definitions */
#define AMBA_I2C_SPEED_STANDARD     I2C_SPEED_STANDARD
#define AMBA_I2C_SPEED_FAST         I2C_SPEED_FAST
#define AMBA_I2C_SPEED_FAST_PLUS    I2C_SPEED_FAST_PLUS
#define AMBA_I2C_SPEED_HIGH         I2C_SPEED_HIGH

typedef struct {
    UINT32 SlaveAddr;               /* Slave address */
    UINT32 DataSize;                /* Maximum number of TX/RX data */
    UINT8 *pDataBuf;                /* Pointer to TX/RX data buffer */
} AMBA_I2C_TRANSACTION_s;

typedef void (*AMBA_I2C_ISR_f)(UINT32 I2cChanNo);
typedef void (*AMBA_I2C_SLAVE_WRITE_ISR_f)(UINT32 SlaveAddr, UINT32 MaxTxData, UINT32 * pNumTxData, UINT8 ** pTxData);
typedef void (*AMBA_I2C_SLAVE_READ_ISR_f)(UINT32 SlaveAddr, UINT32 NumRxData, const UINT8 * pRxData, UINT32 * pStop);
typedef void (*AMBA_I2C_SLAVE_BREAK_ISR_f)(UINT32 CondType, UINT32 NumTxResidual);

#endif /* AMBA_I2C_DEF_H */
