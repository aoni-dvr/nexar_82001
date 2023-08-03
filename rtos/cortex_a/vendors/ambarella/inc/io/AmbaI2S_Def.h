/**
 *  @file AmbaI2S_Def.h
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
 *  @details Common Definitions & Constants for I2S APIs
 *
 */

#ifndef AMBA_I2S_DEF_H
#define AMBA_I2S_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBA_I2S_PRIV_H
#include "AmbaI2S_Priv.h"
#endif

#define I2S_ERR_0000        (I2S_ERR_BASE)              /* Invalid argument */
#define I2S_ERR_0001        (I2S_ERR_BASE + 0X1U)       /* Mutex error */
#define I2S_ERR_0002        (I2S_ERR_BASE + 0X2U)       /* Timeout occurred */
#define I2S_ERR_0003        (I2S_ERR_BASE + 0X3U)       /* TBD */
#define I2S_ERR_0004        (I2S_ERR_BASE + 0X4U)       /* EventFlags error */

#define I2S_ERR_NONE        (OK)
#define I2S_ERR_ARG         I2S_ERR_0000
#define I2S_ERR_MUTEX       I2S_ERR_0001
#define I2S_ERR_TIMEOUT     I2S_ERR_0002
#define I2S_ERR_TBD         I2S_ERR_0003
#define I2S_ERR_EVTFLGS     I2S_ERR_0004

#define AMBA_I2S_MODE_LEFT_JUSTIFIED    0x0U    /* Left-Justified data format */
#define AMBA_I2S_MODE_RIGHT_JUSTIFIED   0x1U    /* Right-Justified data format */
#define AMBA_I2S_MODE_MSB_EXTENDED      0x2U    /* MSB extended data format */
#define AMBA_I2S_MODE_I2S               0x4U    /* Philips I2S data format */
#define AMBA_I2S_MODE_DSP               0x6U    /* DSP / TDM data format */

#define AMBA_I2S_SLAVE                  0x0U    /* Slave clock */
#define AMBA_I2S_MASTER                 0x1U    /* Master clock */

#define AMBA_I2S_MSB_FIRST              0x0U    /* Data word is MSB first */
#define AMBA_I2S_LSB_FIRST              0x1U    /* Data word is LSB first */

#define AMBA_I2S_CLK_EDGE_RISING        0x0U    /* Rising edge of serial clock */
#define AMBA_I2S_CLK_EDGE_FALLING       0x1U    /* Falling edge of serial clock */

#define AMBA_I2S_WS_16SCK               0x0U    /* 16 bit clock cycles */
#define AMBA_I2S_WS_32SCK               0x1U    /* 32 bit clock cycles */

#define AMBA_I2S_CLK_FREQ_128FS         0x0U    /* 128xFs oversampling rate */
#define AMBA_I2S_CLK_FREQ_256FS         0x1U    /* 256xFs oversampling rate */
#define AMBA_I2S_CLK_FREQ_384FS         0x2U    /* 384xFs oversampling rate */
#define AMBA_I2S_CLK_FREQ_512FS         0x3U    /* 512xFs oversampling rate */
#define AMBA_I2S_CLK_FREQ_768FS         0x4U    /* 768xFs oversampling rate */
#define AMBA_I2S_CLK_FREQ_1024FS        0x5U    /* 1024xFs oversampling rate */
#define AMBA_I2S_CLK_FREQ_1152FS        0x6U    /* 1152xFs oversampling rate */
#define AMBA_I2S_CLK_FREQ_1536FS        0x7U    /* 1536xFs oversampling rate */
#define AMBA_I2S_CLK_FREQ_2304FS        0x8U    /* 2304xFs oversampling rate */
#define AMBA_I2S_NUM_CLK_FREQ           0x9U

#define AMBA_I2S_2_CHANNELS             0x0U    /* 2 channel audio */
#define AMBA_I2S_4_CHANNELS             0x1U    /* 4 channel audio */

#define AMBA_I2S_AUDIO_CHANNELS_2       0x2U
#define AMBA_I2S_AUDIO_CHANNELS_4       0x4U

typedef struct {
    UINT32  Loopback;   /* Receiver loop back test */
    UINT32  Order;      /* Receiver bit order: 0U - MSB is first, 1U - LSB is first */
    UINT32  Rsp;        /* Receiver I2S_CLK polarity: 0 - Sample data at rising edge, 1 - Sample data at falling edge */
    UINT32  Shift;      /* Receiver shift: 0U: No shift
                                           8U: Shift 8 bit: dma_rd_data[31:0] = {rx_fifo_data[23:0],  8'b0}
                                           16U: Shift 16bit, dma_rd_data[31:0] = {rx_fifo_data[15:0], 16'b0} */
} AMBA_I2S_RX_CTRL_s;

typedef struct {
    UINT32  Loopback;   /* Transmitter loop back test */
    UINT32  Order;      /* Transmitter bit order: 0U - MSB is first, 1U - LSB is first */
    UINT32  Tsp;        /* Transmitter I2S_CLK polarity: 0 - data change at rising edge, 1 - data change at falling edge */
    UINT32  Shift;      /* Transmitter shift: 0U: No shift
                                              8U: Shift 8 bit: tx_fifo_data[31:0] = {8'b0,  dma_wt_data[31:8]}
                                              16U: Shift 16bit, tx_fifo_data[31:0] = {16'b0, dma_wt_data[31:16]} */
    UINT32  Unison;     /* Transmitter unison: 0U: tx_left_data[31:0] = left_data[31:0]
                                               1U: tx_left_data[31:0] = {right_data[15:0], left_data[15:0]} */
    UINT32  Mute;       /* Transmitter mute: 0 - Normal, 1 - Mute */
    UINT32  Mono;       /* Transmitter mono: 0U: Stereo, 1U: Reserved, 2U: Mono right channel, 3U: Mono left channel */
} AMBA_I2S_TX_CTRL_s;

typedef struct {
    UINT32 Mode;                /* I2S modes */
    UINT32 WordPrecision;       /* Word precision of I2S (unit: bits), 16U, 18U, 20U, 24U, 32U */
    UINT32 WordPos;             /* Ignored bits between two WS edges */
    UINT32 DspModeSlots;        /* Number of slots for DSP mode */
    UINT32 ChannelNum;          /* Number of audio channels */
    UINT32 ClkDirection;        /* Direction of I2S clock */
    UINT32 ClkDivider;          /* I2S_CLK (BCLK) = CLK_AU / [ 2*( ClkDivider +1) ] */
    UINT32 Echo;                /* 0U - No echo, 1U - Echo Rx data to Tx */
    AMBA_I2S_RX_CTRL_s RxCtrl;  /* I2S Receiver Control structure */
    AMBA_I2S_TX_CTRL_s TxCtrl;  /* I2S Transmitter Control structure */
} AMBA_I2S_CTRL_s;

#endif /* AMBA_I2S_DEF_H */
