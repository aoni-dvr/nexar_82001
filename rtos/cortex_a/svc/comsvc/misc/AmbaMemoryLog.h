/**
 *  @file AmbaMemoryLog.h
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
 *
 *  @details Header file for Ambarella memoey log functions.
 *
 */

#ifndef AMBA_MEMORY_LOG_H
#define AMBA_MEMORY_LOG_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

#define AMBA_MEMORY_LOG_MAX_ENTRY          (100U)
#define AMBA_MEMORY_LOG_ADDRESS_NAME_SIZE  (12U)
#define AMBA_MEMORY_LOG_ADDRESS_LOG_SIZE   (AMBA_MEMORY_LOG_ADDRESS_NAME_SIZE+4U+4U)
#define AMBA_MEMORY_LOG_TASK_NAME_SIZE     (20U)

#define MEMORY_LOG_ERR_PARAMETER_NULL      5000U
#define MEMORY_LOG_ERR_MUTEX_LOCK          5001U
#define MEMORY_LOG_ERR_MUTEX_UNLOCK        5002U
#define MEMORY_LOG_ERR_INVALID_SLOTID      5003U
#define MEMORY_LOG_ERR_INIT_FAIL           5004U
#define MEMORY_LOG_ERR_ALREADY_REGISTERED  5005U
#define MEMORY_LOG_ERR_NO_INIT             5006U
#define MEMORY_LOG_BUFFER_FULL             5007U
#define MEMORY_LOG_BUFFER_NULL             5008U
#define MEMORY_LOG_SLOT_FULL               5009U
#define MEMORY_LOG_MESSAGE_TOO_LARGE       5010U

typedef struct {
    UINT32 MessagePoolSize;         /* Byte size of ready to print string buffer */
    UINT8 *MessagePool;             /* Pointer to ready to print string buffer */
    UINT32 MessageWriteIdx;         /* The next write buffer position */
    UINT32 MessageSize;             /* Message size */
    UINT32 AddressPoolSize;         /* Byte size of ready to print string buffer */
    UINT8 *AddressPool;             /* Pointer to ready to print string buffer */
    UINT32 AddressWriteIdx;         /* The next write buffer position */
    UINT8  Used;                    /* 1: Occupied, 0: Unused */
} AMBA_MEMORY_LOG_ENTRY_s;

typedef struct {
    UINT32 MaxEntryNumber;
    AMBA_MEMORY_LOG_ENTRY_s LogEntry[AMBA_MEMORY_LOG_MAX_ENTRY];
} AMBA_MEMORY_LOG_INFO_s;

#ifdef __cplusplus
extern "C" {
#endif
UINT32 AmbaMemoryLog_Init(void);
UINT32 AmbaMemoryLog_Register(UINT32 *pSlotID, UINT8 *pMessageLogAddr, UINT32 MessageLogSize, UINT8 *pAddressLogAddr, UINT32 AddressLogSize);
UINT32 AmbaMemoryLog_PushMessageLog(UINT32 SlotId, const char *pString);
UINT32 AmbaMemoryLog_PushAddressLog(UINT32 SlotId, const char *pName, UINT32 Address, UINT32 Size);
UINT32 AmbaMemoryLog_GetInfo(const AMBA_MEMORY_LOG_INFO_s ** pInfo);

#ifdef __cplusplus
}
#endif

#endif
