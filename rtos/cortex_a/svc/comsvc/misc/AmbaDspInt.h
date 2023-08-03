/**
 *  @file AmbaDspInt.h
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @details Constants and Definitions for amba dsp interrupt
 *
 */

#ifndef AMBA_DSP_INT_H
#define AMBA_DSP_INT_H

#ifndef AMBA_KAL_H
#include "AmbaKAL.h"
#endif
#ifndef AMBA_VIN_DEF_H
#include "AmbaVIN_Def.h"
#endif
#ifndef AMBA_DSP_CAPABILITY_H
#include "AmbaDSP_Capability.h"
#endif

#define AMBA_DSP_INT_MEM_ADDR ULONG

typedef enum /*_AMBA_DSP_INT_TYPE_e_*/ {
    AMBA_DSP_INT_TYPE_VIN = 0,
    AMBA_DSP_INT_TYPE_VOUT,
    AMBA_DSP_INT_TYPE_TOTAL
} AMBA_DSP_INT_TYPE_e;

#define AMBA_NUM_VOUT_CHANNEL AMBA_DSP_MAX_VOUT_NUM

typedef struct /*_AMBA_DSP_INT_PTS_s_ */ {
    UINT64 Sof[AMBA_NUM_VIN_CHANNEL];
    UINT64 Eof[AMBA_NUM_VIN_CHANNEL];
    UINT64 Vout[AMBA_NUM_VOUT_CHANNEL];
} AMBA_DSP_INT_PTS_s;

typedef struct /*_AMBA_DSP_INT_VIN_INFO_s_*/ {
    UINT32 RawCapSeq[AMBA_NUM_VIN_CHANNEL];
} AMBA_DSP_INT_VIN_INFO_s;

typedef struct /*_AMBA_DSP_INT_INFO_s_*/ {
    AMBA_DSP_INT_PTS_s         Pts;
    AMBA_DSP_INT_VIN_INFO_s    Vin;
} AMBA_DSP_INT_INFO_s;

typedef struct /*_AMBA_DSP_INT_PORT_LINK_s_*/ {
    struct AMBA_DSP_INT_PORT_s_    *Up;
    struct AMBA_DSP_INT_PORT_s_    *Down;
} AMBA_DSP_INT_PORT_LINK_s;

typedef void (*AMBA_DSP_INT_CB_f)(UINT32 ActualFlag, const AMBA_DSP_INT_INFO_s *pInfo);

typedef struct AMBA_DSP_INT_PORT_s_ {
    UINT32                     Magic;
    UINT32                     Priority;
    AMBA_DSP_INT_CB_f          pFunc;
    AMBA_KAL_EVENT_FLAG_t      Event;
    AMBA_DSP_INT_PTS_s         Pts;
    AMBA_DSP_INT_PORT_LINK_s   Link;
} AMBA_DSP_INT_PORT_s;

typedef struct /*_AMBA_DSP_INT_s_*/ {
    UINT32                Magic;
    AMBA_KAL_MUTEX_t      Mutex;
    AMBA_DSP_INT_PORT_s    *pList;
} AMBA_DSP_INT_s;

/* flags[0:27] */
#if defined(AMBA_DSP_VIN0_SOF)
#define SVC_DSP_VIN0_SOF    AMBA_DSP_VIN0_SOF
#define SVC_DSP_VIN0_EOF    AMBA_DSP_VIN0_EOF
#endif
#if defined(AMBA_DSP_VIN1_SOF)
#define SVC_DSP_VIN1_SOF    AMBA_DSP_VIN1_SOF
#define SVC_DSP_VIN1_EOF    AMBA_DSP_VIN1_EOF
#endif
#if defined(AMBA_DSP_VIN2_SOF)
#define SVC_DSP_VIN2_SOF    AMBA_DSP_VIN2_SOF
#define SVC_DSP_VIN2_EOF    AMBA_DSP_VIN2_EOF
#endif
#if defined(AMBA_DSP_VIN3_SOF)
#define SVC_DSP_VIN3_SOF    AMBA_DSP_VIN3_SOF
#define SVC_DSP_VIN3_EOF    AMBA_DSP_VIN3_EOF
#endif
#if defined(AMBA_DSP_VIN4_SOF)
#define SVC_DSP_VIN4_SOF    AMBA_DSP_VIN4_SOF
#define SVC_DSP_VIN4_EOF    AMBA_DSP_VIN4_EOF
#endif
#if defined(AMBA_DSP_VIN5_SOF)
#define SVC_DSP_VIN5_SOF    AMBA_DSP_VIN5_SOF
#define SVC_DSP_VIN5_EOF    AMBA_DSP_VIN5_EOF
#endif
#if defined(AMBA_DSP_VIN6_SOF)
#define SVC_DSP_VIN6_SOF    AMBA_DSP_VIN6_SOF
#define SVC_DSP_VIN6_EOF    AMBA_DSP_VIN6_EOF
#endif
#if defined(AMBA_DSP_VIN7_SOF)
#define SVC_DSP_VIN7_SOF    AMBA_DSP_VIN7_SOF
#define SVC_DSP_VIN7_EOF    AMBA_DSP_VIN7_EOF
#endif
#if defined(AMBA_DSP_VIN8_SOF)
#define SVC_DSP_VIN8_SOF    AMBA_DSP_VIN8_SOF
#define SVC_DSP_VIN8_EOF    AMBA_DSP_VIN8_EOF
#endif
#if defined(AMBA_DSP_VIN9_SOF)
#define SVC_DSP_VIN9_SOF    AMBA_DSP_VIN9_SOF
#define SVC_DSP_VIN9_EOF    AMBA_DSP_VIN9_EOF
#endif
#if defined(AMBA_DSP_VIN10_SOF)
#define SVC_DSP_VIN10_SOF    AMBA_DSP_VIN10_SOF
#define SVC_DSP_VIN10_EOF    AMBA_DSP_VIN10_EOF
#endif
#if defined(AMBA_DSP_VIN11_SOF)
#define SVC_DSP_VIN11_SOF    AMBA_DSP_VIN11_SOF
#define SVC_DSP_VIN11_EOF    AMBA_DSP_VIN11_EOF
#endif
#if defined(AMBA_DSP_VIN12_SOF)
#define SVC_DSP_VIN12_SOF    AMBA_DSP_VIN12_SOF
#define SVC_DSP_VIN12_EOF    AMBA_DSP_VIN12_EOF
#endif
#if defined(AMBA_DSP_VIN13_SOF)
#define SVC_DSP_VIN13_SOF    AMBA_DSP_VIN13_SOF
#define SVC_DSP_VIN13_EOF    AMBA_DSP_VIN13_EOF
#endif

#define AMBA_DSP_INT_VOUT_FLAG_SHIFT    28U

/* flags[28:31], TBD 3 vout?  */
#if defined(AMBA_DSP_VOUT0_INT)
#define SVC_DSP_VOUT0_INT    (AMBA_DSP_VOUT0_INT << AMBA_DSP_INT_VOUT_FLAG_SHIFT)
#endif
#if defined(AMBA_DSP_VOUT1_INT)
#define SVC_DSP_VOUT1_INT    (AMBA_DSP_VOUT1_INT << AMBA_DSP_INT_VOUT_FLAG_SHIFT)
#endif
#if defined(AMBA_DSP_VOUT2_INT)
#define SVC_DSP_VOUT2_INT    (AMBA_DSP_VOUT2_INT << AMBA_DSP_INT_VOUT_FLAG_SHIFT)
#endif

UINT32 AmbaDspInt_Init(UINT32 Priority, UINT32 CoreInclusion);

UINT32 AmbaDspInt_Open(AMBA_DSP_INT_PORT_s *pPort);
UINT32 AmbaDspInt_OpenEx(AMBA_DSP_INT_PORT_s *pPort, UINT32 Priority, AMBA_DSP_INT_CB_f pFunc);
UINT32 AmbaDspInt_Take(AMBA_DSP_INT_PORT_s *pPort, UINT32 Flag, UINT32 *pActualFlag, UINT32 Timeout);
UINT32 AmbaDspInt_AuxGive(AMBA_DSP_INT_PORT_s *pPort, UINT32 Flags);
UINT32 AmbaDspInt_Close(AMBA_DSP_INT_PORT_s *pPort);

UINT32 AmbaDspInt_SysInit(void);
UINT32 AmbaDspInt_TaskCreate(UINT32 Priority, UINT32 CoreInclusion);

UINT32 AmbaDspInt_Raw(const void *pData);

#endif  /* SVC_DSP_INT_H */
