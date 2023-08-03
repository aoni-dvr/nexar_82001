/**
 *  @file AmbaIP_Monitor_Def.h
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
 *  @details Definitions & Constants for IP Driver Monitor Definition
 *
 */
#ifndef AMBA_IPMONITOR_DEF_H
#define AMBA_IPMONITOR_DEF_H

#include "AmbaTypes.h"
#include "AmbaImg_Proc.h"
/* Error definition */

//#define AMBA_IPC_MUTEX_IPMON               (18)

#define IPMON_ERR_NONE                     (0U)
#define IPMON_ERR_0000                     (IPMONADJ_ERR_BASE       ) // Invalid argument (null pointer)
#define IPMON_ERR_0001                     (IPMONADJ_ERR_BASE + 0x1U) // Input argument out of range
#define IPMON_ERR_0002                     (IPMONADJ_ERR_BASE + 0x2U) // OS resource error
#define IPMON_ERR_0003                     (IPMONADJ_ERR_BASE + 0x3U) // Invalid timing
#define IPMON_ERR_0004                     (IPMONADJ_ERR_BASE + 0x4U) // not support or not implement
#define IPMON_ERR_0005                     (IPMONADJ_ERR_BASE + 0x5U) // CRC compare error
#define IPMON_ERR_0006                     (IPMONADJ_ERR_BASE + 0x6U) // HeartBeat lost
#define IPMON_ERR_0007                     (IPMONADJ_ERR_BASE + 0x7U) // HeartBeat Freeze
#define IPMON_ERR_0008                     (IPMONADJ_ERR_BASE + 0x8U) // CRC compare timeout

#define IP_MON_BITMASK_LOGIC               (0x01U)
#define IP_MON_BITMASK_CRC                 (0x02U)
#define IP_MON_BITMASK_UCODE               (0x04U)
#define IP_MON_BITMASK_HEARTBEAT           (0x08U)

#define IP_MON_MODULE_MAX_NUM      (0x3U)  // only support AE/AWB/ADJ

typedef struct {
    UINT32 ModuleId;
    INT32  Result;
    UINT64 UUID;
} AMBA_IP_MONITOR_ADJ_CMPR_s;

typedef struct {
    UINT32 ModuleId;
    INT32  Result;
    UINT64 UUID;
} AMBA_IP_MONITOR_AWB_CMPR_s;

// typedef struct {
//     UINT32 DGain;
//     FLOAT  AgcGain;
//     FLOAT  ShutterTime;
// } AMBA_AE_INFORMATION_s;

typedef struct {
    UINT32 ModuleId;
    INT32  Result;
    UINT64 UUID;
} AMBA_IP_MONITOR_AE_CMPR_s;

typedef struct {
    UINT32 ModuleId;
    UINT32 InstId;
    UINT32 ApiId;
    UINT32 ErrorId;
} AMBA_IP_MONITOR_ERR_NOTIFY_s;

#define IPMON_HEARTBEAT_0      (0U)
#define IPMON_HEARTBEAT_1      (1U)
#define IPMON_HEARTBEAT_2      (2U)
#define IPMON_HEARTBEAT_3      (3U)
#define IPMON_HEARTBEAT_NUM    (4U)
#define IPMON_HEARTBEAT_INT_ID_INVALID (0xFFFFU)
typedef struct {
    UINT16 IntId[IPMON_HEARTBEAT_NUM];
    UINT64 Addr[IPMON_HEARTBEAT_NUM];
} AMBA_IP_MONITOR_HEARTBEAT_s;

/* IPC */
#define IPMON_IPC_CMD_STATE_INITED    (0x00000001U) /* Inform remote side that local is ready for message */
#define IPMON_IPC_CMD_CFG_BIT_MASK    (0x00000002U)
#define IPMON_IPC_CMD_CRC_CMPR        (0x00000003U)
#define IPMON_IPC_CMD_ERR_NOTIFY      (0x00000004U)
#define IPMON_IPC_CMD_CFG_HEARTBEAT   (0x00000005U)

#define IPMON_IPC_CMD_DATA_LEN    (15U)
typedef struct {
    UINT32 Code;
    UINT32 Data[IPMON_IPC_CMD_DATA_LEN];
} IPMON_IPC_CMD_s; //64Byte

typedef struct {
    UINT32 Code;
    UINT32 ModuleId;
    UINT32 BitMask;
} IPMON_CMD_CFG_BIT_MASK_s;

typedef struct {
    UINT32 Code;
    UINT32 ModuleId;
    UINT32 Crc0;
    UINT32 Crc1;
    UINT64 UUID;
} IPMON_CMD_CRC_CMPR_s;

typedef struct {
    UINT32 Code;
    UINT16 IntId[IPMON_HEARTBEAT_NUM];
    UINT64 Addr[IPMON_HEARTBEAT_NUM];
} IPMON_CMD_HEARTBEAT_s;

typedef struct {
    UINT32 Code;
    UINT32 ModuleId;
    UINT32 InstId;
    UINT32 ApiId;
    UINT32 ErrorId;
} IPMON_CMD_ERR_NOTIFY_s;

#endif  /* AMBA_IP_MONITOR_DEF_H */
