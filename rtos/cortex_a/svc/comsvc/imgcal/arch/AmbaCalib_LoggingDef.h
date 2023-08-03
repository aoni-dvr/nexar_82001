/**
 *  @file AmbaCalib_LoggingDef.h
 *
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
#ifndef AMBA_CALIB_LOGGING_DEF_H
#define AMBA_CALIB_LOGGING_DEF_H
#include "AmbaTypes.h"
#define AMBA_CAL_MAX_MSG_NUM (20U)
#define AMBA_CAL_MSG_POOL_NUM (4000U)

#define AMBA_CAL_LOG_LEVEL_DEBUG (0x1U)
#define AMBA_CAL_LOG_LEVEL_WARNING (0x2U)
#define AMBA_CAL_LOG_LEVEL_ERROR (0x4U)
#define AMBA_CAL_LOG_LEVEL_INFO (0x8U)

typedef enum {
    AMBA_CAL_LOG_TYPE_VAR_U8 = 0,
    AMBA_CAL_LOG_TYPE_VAR_S8,
    AMBA_CAL_LOG_TYPE_VAR_U16,
    AMBA_CAL_LOG_TYPE_VAR_S16,
    AMBA_CAL_LOG_TYPE_VAR_U32,
    AMBA_CAL_LOG_TYPE_VAR_S32,
    AMBA_CAL_LOG_TYPE_VAR_U64,
    AMBA_CAL_LOG_TYPE_VAR_S64,
    AMBA_CAL_LOG_TYPE_VAR_CHAR,
    AMBA_CAL_LOG_TYPE_VAR_DB,
    AMBA_CAL_LOG_TYPE_VAR_STR,
    AMBA_CAL_LOG_TYPE_PT_ADDR,
    AMBA_CAL_LOG_TYPE_END_OF_MSG,
    AMBA_CAL_LOG_TYPE_MAX,
} AMBA_CAL_LOG_TYPE_E;

typedef struct {
    AMBA_CAL_LOG_TYPE_E Id;
    UINT64 Data;
} AMBA_CAL_LOG_MSG_s;

typedef struct {
    UINT32 EndIdx;
    AMBA_CAL_LOG_MSG_s MsgPool[AMBA_CAL_MSG_POOL_NUM];
    UINT32 IsRing;
    UINT32 LogLevel;
} AMBA_CAL_LOG_CONTAINER_s;
#endif
