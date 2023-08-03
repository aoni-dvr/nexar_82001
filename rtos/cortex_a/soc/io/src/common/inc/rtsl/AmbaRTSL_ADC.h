/*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AMBA_RTSL_ADC_H
#define AMBA_RTSL_ADC_H

#ifndef AMBA_ADC_DEF_H
#include "AmbaADC_Def.h"
#endif

#define AMBA_NUM_ADC_TIME_SLOT      8U      /* Total Number of ADC Time Slots */
#define AMBA_ADC_FIFO_SIZE          1024U
#define AMBA_ADC_ONE_SLOT_PERIOD    20U     /* Samping one channel requires 20 clock ticks */

#define AMBA_ADC_FIFO0              0x0U    /* 1st ADC FIFO */
#define AMBA_ADC_FIFO1              0x1U    /* 2nd ADC FIFO */
#define AMBA_ADC_FIFO2              0x2U    /* 3rd ADC FIFO */
#define AMBA_ADC_FIFO3              0x3U    /* 4th ADC FIFO */
#define AMBA_NUM_ADC_FIFO           0x4U    /* Number of ADC Channels */

#define AMBA_ADC_INT_EVENT_COUNTER          0x01U    /* Event counter interrupt */
#define AMBA_ADC_INT_SLOT_PERIOD_ERR        0x02U    /* Sample period over limit interrupt */
#define AMBA_ADC_INT_INVAILD_FIFO_PARAM     0x04U    /* Invaild fifo param interrupt */
#define AMBA_ADC_INT_FIFO_ERROR             0x08U    /* FIFO full, underflow or overflow */
#define AMBA_ADC_INT_DATA_EXCEPTION         0x10U    /* ADC Channel Interrupt */

/*
 * ADC Status Structures
 */
typedef struct {
    UINT8   TimeSlotNumberUsed;                         /* Number of valid time slots */
    UINT16  TimeSlotPeriod;                             /* Ticks of one time slot period */
    UINT16  TimeSlotChannels[AMBA_NUM_ADC_TIME_SLOT];   /* Enabled ADC channels in round-robin scheduling */
} AMBA_RTSL_ADC_PARAM_s;

/*
 * ADC Channel Structures
 */
typedef struct {
    UINT16  DataUpperBound;                             /* Maximum allowed data value of the ADC channel */
    UINT16  DataLowerBound;                             /* Minimum allowed data value of the ADC channel */
} AMBA_ADC_CHANNEL_s;

/*
 * ADC FIFO Structures
 */
typedef struct {
    UINT32  AdcChanNo;                                  /* ADC channel number */
    UINT16  FifoSize;                                   /* Allocated FIFO size for ADC channel */
} AMBA_ADC_FIFO_s;

/*
 * Defined in AmbaRTSL_ADC.c
 */
void AmbaRTSL_AdcInit(void);
UINT32 AmbaRTSL_AdcConfig(UINT32 SampleRate);
UINT32 AmbaRTSL_AdcSeamlessRead(UINT32 AdcCh, UINT32 BufSize, UINT32 * const pBuf);
UINT32 AmbaRTSL_AdcSingleRead(UINT32 AdcCh, UINT32 *pData);
void AmbaRTSL_AdcStop(UINT32 * pActualSize);

void AmbaRTSL_AdcHookIntHandler(AMBA_ADC_ISR_HANDLER_f CallBackFunc);
void AmbaRTSL_AdcGetInfo(UINT32 *pSampleRate, UINT32 *pActiveChanList);

#endif /* AMBA_RTSL_ADC_H */
