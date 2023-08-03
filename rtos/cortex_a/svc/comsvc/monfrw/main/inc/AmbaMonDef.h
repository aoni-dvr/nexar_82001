/**
 *  @file AmbaMonDef.h
 *
 *  Copyright (c) [2020] Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Constants and Definitions for Amba Monitor Def
 *
 */

#ifndef AMBA_MONITOR_DEF_H
#define AMBA_MONITOR_DEF_H

#define AMBA_MON_MAIN_MEM_ADDR    ULONG

#define AMBA_MON_NUM_VIN_SENSOR     4U

#define AMBA_MON_NUM_VIN_CHANNEL    AMBA_NUM_VIN_CHANNEL
#define AMBA_MON_NUM_FOV_CHANNEL    AMBA_DSP_MAX_VIEWZONE_NUM
#define AMBA_MON_NUM_DISP_CHANNEL   AMBA_DSP_MAX_VOUT_NUM
#define AMBA_MON_NUM_VOUT_CHANNEL   AMBA_DSP_MAX_VOUT_NUM

//#define AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
/* evaluated only */
void AmbaMonListenAaa_CdEnable(UINT32 Enable, UINT32 Interval, UINT32 Shift);
typedef union /*_AMBA_MON_AAA_CFA_STATE_ID_u_*/{
    UINT32 Data;
    struct {
        UINT32 CdZero:         1;
        UINT32 Reserved:      30;
    } Bits;
} AMBA_MON_AAA_CFA_STATE_ID_u;
typedef struct /*_AMBA_MON_AAA_CFA_STATE_s_*/ {
    UINT32                         FovId;
    AMBA_MON_AAA_CFA_STATE_ID_u    StatusId;
} AMBA_MON_AAA_CFA_STATE_s;
#endif

typedef struct /*_AMBA_MON_VIN_ISR_RDY_s_*/ {
    UINT32 VinId;
    UINT64 Dts;
    UINT64 Pts;
} AMBA_MON_VIN_ISR_RDY_s;

typedef struct /*_AMBA_MON_VIN_ISR_INFO_s_*/ {
    UINT32 VinId;
    UINT32 SensorId;
    UINT32 SerdesId;
    UINT64 Dts;
    UINT64 Pts;
} AMBA_MON_VIN_ISR_INFO_s;

typedef AMBA_MON_VIN_ISR_INFO_s  AMBA_MON_VIN_ISR_STATE_s;

typedef struct /*_AMBA_MON_VIN_SERDES_STATE_s_*/ {
    UINT32 VinId;
    UINT32 SensorId;
    UINT32 SerdesId;
    UINT32 LinkId;
    UINT32 DataId;
} AMBA_MON_VIN_SERDES_STATE_s;

typedef struct /*_AMBA_MON_DSP_RAW_STATE_s_*/ {
    UINT32 VinId;
    UINT64 SeqNum;
    UINT64 Pts;
} AMBA_MON_DSP_RAW_STATE_s;

typedef struct /*_AMBA_MON_VOUT_SERDES_STATE_s_*/ {
    UINT32 VoutId;
    UINT32 SerdesId;
    UINT32 LinkId;
} AMBA_MON_VOUT_SERDES_STATE_s;

typedef struct /*_AMBA_MON_DSP_FOV_STATE_*/ {
    UINT32 FovId;
    UINT64 YuvSeqNum;
    UINT32 Latency;
} AMBA_MON_DSP_FOV_STATE_s;

typedef struct /*_AMBA_MON_DSP_VOUT_STATE_*/ {
    UINT32 VoutId;
    UINT64 Pts;
    UINT64 DispSeqNum;
    UINT32 DispLatency;
    UINT32 FovSelectBits;
    UINT32 Latency[AMBA_MON_NUM_FOV_CHANNEL];
} AMBA_MON_DSP_VOUT_STATE_s;

typedef struct /*_AMBA_MON_DSP_VIN_TIMEOUT_STATE_*/ {
    UINT32 VinId;
} AMBA_MON_DSP_VIN_TIMEOUT_STATE_s;

typedef struct /*_AMBA_MON_VOUT_ISR_RDY_s_*/ {
    UINT32 VoutId;
    UINT64 Dts;
    UINT64 Pts;
} AMBA_MON_VOUT_ISR_RDY_s;

#define AMBA_MON_DSP_FOV_LATENCY_s     AMBA_MON_DSP_FOV_STATE_s
#define AMBA_MON_DSP_VOUT_LATENCY_s    AMBA_MON_DSP_VOUT_STATE_s

#endif  /* AMBA_MONITOR_DEF_H */
