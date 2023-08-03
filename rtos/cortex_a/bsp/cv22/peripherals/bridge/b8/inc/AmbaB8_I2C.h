/**
 *  @file AmbaB8_I2C.h
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
 *  @details Definitions & Constants for B6 I2C APIs
 *
 */

#ifndef AMBA_B8_I2C_H
#define AMBA_B8_I2C_H

#define B8_I2C_MAX_FIFO_ENTRY     63
#define B8_I2C_MAX_RX_FIFO_ENTRY  8U

#define B8_I2C_CHANNEL0           0U     /* I2C Channel0 */
#define B8_I2C_CHANNEL0_2         1U     /* I2C Channel0-2 */
#define B8_I2C_CHANNEL0_ALL       2U     /* I2C Channel0-all */
#define B8_NUM_I2C_CHANNEL        3U     /* Total Number of I2C Channels */

#define B8_I2C_SPEED_STANDARD     0U     /* I2C Standard speed: 100Kbps */
#define B8_I2C_SPEED_FAST         1U     /* I2C Fast speed: 384Kbps */
#define B8_NUM_I2C_SPEED          2U     /* Total Number of I2C Speed */

typedef struct {
    UINT32 Speed;
    UINT32 DutyCycle;
    UINT32 MinSTAHoldTime; /* in ns */
} B8_I2C_TIMING_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaB8_I2C.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_I2cInit(void);
UINT32 AmbaB8_I2cWrite(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed, UINT8 SlaveAddr,
                       UINT32 TxDataSize, const UINT8 *pTxDataBuf, UINT8 BurstEnable);
UINT32 AmbaB8_I2cRead(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed, UINT8 SlaveAddr,
                      UINT32 RxDataSize, UINT8 *pRxDataBuf, UINT8 BurstEnable);
UINT32 AmbaB8_I2cReadAfterWrite(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed,
                                UINT32 TxDataSize, const UINT8 *pTxDataBuf, UINT32 RxDataSize, UINT8 *pRxDataBuf, UINT8 BurstEnable);

#endif /* AMBA_B8_I2C_H */
