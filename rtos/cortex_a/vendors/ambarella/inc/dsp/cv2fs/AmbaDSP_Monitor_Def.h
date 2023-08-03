/**
 *  @file AmbaDSP_Monitor_Def.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for DSP Driver Monitor Definition
 *
 */
#ifndef AMBA_DSP_MONITOR_DEF_H
#define AMBA_DSP_MONITOR_DEF_H

#include "AmbaTypes.h"

/* Error definition */
#define DSPMON_ERR_NONE                     (0U)
#define DSPMON_ERR_0000                     (DSPMON_ERR_BASE       ) // Invalid argument (null pointer)
#define DSPMON_ERR_0001                     (DSPMON_ERR_BASE + 0x1U) // Input argument out of range
#define DSPMON_ERR_0002                     (DSPMON_ERR_BASE + 0x2U) // OS resource error
#define DSPMON_ERR_0003                     (DSPMON_ERR_BASE + 0x3U) // Invalid timing
#define DSPMON_ERR_0004                     (DSPMON_ERR_BASE + 0x4U) // not support or not implement
#define DSPMON_ERR_0005                     (DSPMON_ERR_BASE + 0x5U) // CRC compare error
#define DSPMON_ERR_0006                     (DSPMON_ERR_BASE + 0x6U) // HeartBeat lost
#define DSPMON_ERR_0007                     (DSPMON_ERR_BASE + 0x7U) // HeartBeat Freeze
#define DSPMON_ERR_0008                     (DSPMON_ERR_BASE + 0x8U) // CRC compare timeout
#define DSPMON_ERR_0009                     (DSPMON_ERR_BASE + 0x9U) // DSP assert

#define DSP_MON_BITMASK_LOGIC               (0x01U)
#define DSP_MON_BITMASK_CRC                 (0x02U)
#define DSP_MON_BITMASK_UCODE               (0x04U)
#define DSP_MON_BITMASK_HEARTBEAT           (0x08U)

#define DSP_MON_MODULE_MAX_NUM      (0x3U)  // only support DSP/IK/DSPMON
typedef struct {
    UINT32 BitMask;
    UINT32 Rsvd[15U];
} AMBA_DSP_MONITOR_CONFIG_s;

typedef struct {
    UINT32 ModuleId;
    UINT32 Crc0;
    UINT32 Crc1;
    UINT64 UUID;
} AMBA_DSP_MONITOR_CRC_CMPR_s;

typedef struct {
    UINT32 ModuleId;
    UINT32 InstId;
    UINT32 ApiId;
    UINT32 ErrorId;
} AMBA_DSP_MONITOR_ERR_NOTIFY_s;

#define DSPMON_HEARTBEAT_0      (0U)
#define DSPMON_HEARTBEAT_1      (1U)
#define DSPMON_HEARTBEAT_2      (2U)
#define DSPMON_HEARTBEAT_3      (3U)
#define DSPMON_HEARTBEAT_NUM    (4U)
#define DSPMON_HEARTBEAT_INT_ID_INVALID (0xFFFFU)
typedef struct {
    UINT16 IntId[DSPMON_HEARTBEAT_NUM];
    UINT64 Addr[DSPMON_HEARTBEAT_NUM];
} AMBA_DSP_MONITOR_HEARTBEAT_s;

/* IPC */
#define DSPMON_IPC_CMD_STATE_INITED    (0x00000001U) /* Inform remote side that local is ready for message */
#define DSPMON_IPC_CMD_CFG_BIT_MASK    (0x00000002U)
#define DSPMON_IPC_CMD_CRC_CMPR        (0x00000003U)
#define DSPMON_IPC_CMD_ERR_NOTIFY      (0x00000004U)
#define DSPMON_IPC_CMD_CFG_HEARTBEAT   (0x00000005U)

#define DSPMON_IPC_CMD_DATA_LEN    (15U)
typedef struct {
    UINT32 Code;
    UINT32 Data[DSPMON_IPC_CMD_DATA_LEN];
} DSPMON_IPC_CMD_s; //64Byte

typedef struct {
    UINT32 Code;
    UINT32 ModuleId;
    UINT32 BitMask;
} DSPMON_CMD_CFG_BIT_MASK_s;

typedef struct {
    UINT32 Code;
    UINT32 ModuleId;
    UINT32 Crc0;
    UINT32 Crc1;
    UINT64 UUID;
} DSPMON_CMD_CRC_CMPR_s;

typedef struct {
    UINT32 Code;
    UINT16 IntId[DSPMON_HEARTBEAT_NUM];
    UINT64 Addr[DSPMON_HEARTBEAT_NUM];
} DSPMON_CMD_HEARTBEAT_s;

typedef struct {
    UINT32 Code;
    UINT32 ModuleId;
    UINT32 InstId;
    UINT32 ApiId;
    UINT32 ErrorId;
} DSPMON_CMD_ERR_NOTIFY_s;

typedef struct {
    UINT16 YuvStreamId;     //AMBA_DSP_MAX_YUVSTRM_NUM
//#define DSP_LV_STRM_PURPOSE_MEMORY_IDX  (0U)
//#define DSP_LV_STRM_PURPOSE_ENCODE_IDX  (1U)
//#define DSP_LV_STRM_PURPOSE_VOUT_IDX    (2U)
//#define DSP_LV_STRM_PURPOSE_RAWCAP_IDX  (3U)
    UINT16 Purpose;
} DSPMON_YUV_SRC_s;

#define DSPMON_MAX_YUVSTRM_NUM      (32U)

typedef struct {
    DSPMON_YUV_SRC_s YuvSrc[DSPMON_MAX_YUVSTRM_NUM];
    UINT16 YuvSrcNum;
    UINT16 Reserved;
} AMBA_DSP_MONITOR_YUV_SRC_INFO_s;

#endif  /* AMBA_DSP_MONITOR_DEF_H */
