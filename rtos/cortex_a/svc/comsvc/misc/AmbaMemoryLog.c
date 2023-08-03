/**
 *  @file AmbaMemoryLog.c
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
 *  @details Ambarella memory log
 *
 */

#include "AmbaKAL.h"
//#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMemoryLog.h"

static AMBA_KAL_MUTEX_t MemoryLogMutex;
static UINT32           IsInited = 0;
static AMBA_MEMORY_LOG_INFO_s MemLogInfo __attribute__((section(".bss.noinit")));

static UINT32 GetMemoryLogSlotId(const UINT8 *MessageLogAddr, const UINT8 *AddressLogAddr)
{
    UINT32 i;
    UINT32 SlotId = 0xFFFFFFFFU;
    const AMBA_MEMORY_LOG_ENTRY_s *info;
    for (i = 0; i < AMBA_MEMORY_LOG_MAX_ENTRY; i++) {
        info = &MemLogInfo.LogEntry[i];
        if (info->Used == 1U) {
            if ((info->AddressPool == AddressLogAddr) && (info->MessagePool == MessageLogAddr)) {
                SlotId = i;
                break;
            }
        }
    }
    return SlotId;
}

/**
* Memory log module initialize, create mutex
* @return ErrorCode
*
*/
UINT32 AmbaMemoryLog_Init(void)
{
    UINT32 uRet = 0;
    UINT32 i;
    static char mutex_name[] = "MemoryLogMutex";

    if (IsInited == 0U) {

        MemLogInfo.MaxEntryNumber = AMBA_MEMORY_LOG_MAX_ENTRY;

        // no memset in SDK7
        for (i = 0; i < AMBA_MEMORY_LOG_MAX_ENTRY; i++) {
            MemLogInfo.LogEntry[i].AddressPool = NULL;
            MemLogInfo.LogEntry[i].AddressPoolSize = 0;
            MemLogInfo.LogEntry[i].AddressWriteIdx = 0;
            MemLogInfo.LogEntry[i].MessagePool = NULL;
            MemLogInfo.LogEntry[i].MessagePoolSize = 0;
            MemLogInfo.LogEntry[i].MessageSize = 0;
            MemLogInfo.LogEntry[i].MessageWriteIdx = 0;
            MemLogInfo.LogEntry[i].Used = 0;
        }

        if (AmbaKAL_MutexCreate(&MemoryLogMutex, mutex_name) != 0U) {
            uRet = MEMORY_LOG_ERR_INIT_FAIL;
        } else {
            IsInited = 1;
        }
    }
    return uRet;
}

/**
* Memory log module register
* @param [in] pMessageLogAddr msg log addr
* @param [in] MessageLogSize msg log size
* @param [in] pAddressLogAddr address log addr
* @param [in] AddressLogSize address log size
* @param [out] pSlotID registered slot serial number ID
* @return ErrorCode
*
*/
UINT32 AmbaMemoryLog_Register(UINT32 *pSlotID, UINT8 *pMessageLogAddr, UINT32 MessageLogSize, UINT8 *pAddressLogAddr, UINT32 AddressLogSize)
{
    UINT32 uRet = 0, found = 0;
    UINT32 i;

    if (IsInited == 0U) {
        uRet = MEMORY_LOG_ERR_NO_INIT;
    } else {
        // Check Arguments
        if ((pMessageLogAddr == NULL) || (MessageLogSize == 0U) || (pAddressLogAddr == NULL) || (AddressLogSize == 0U) || (pSlotID == NULL)) {
            uRet = MEMORY_LOG_ERR_PARAMETER_NULL;
        } else {
            // Lock mutex
            if (AmbaKAL_MutexTake(&MemoryLogMutex, AMBA_KAL_WAIT_FOREVER) != 0U) {
                uRet = MEMORY_LOG_ERR_MUTEX_LOCK;  /* Can't take mutex */
            } else {
                // Check if it is registered before
                uRet = GetMemoryLogSlotId(pMessageLogAddr, pAddressLogAddr);
                if (uRet != 0xFFFFFFFFU) {
                    // it is registered before
                    uRet = MEMORY_LOG_ERR_ALREADY_REGISTERED;
                } else {
                    // Find an empty slot
                    for (i = 0; i < AMBA_MEMORY_LOG_MAX_ENTRY; i++) {
                        if (MemLogInfo.LogEntry[i].Used == 0U) {
                            MemLogInfo.LogEntry[i].Used = 1;
                            MemLogInfo.LogEntry[i].MessagePool = pMessageLogAddr;
                            MemLogInfo.LogEntry[i].MessagePoolSize = MessageLogSize;
                            MemLogInfo.LogEntry[i].AddressPool = pAddressLogAddr;
                            MemLogInfo.LogEntry[i].AddressPoolSize = AddressLogSize;
                            MemLogInfo.LogEntry[i].MessageWriteIdx = 0;
                            MemLogInfo.LogEntry[i].AddressWriteIdx = 0;
                            *pSlotID = i;
                            found = 1;
                            break;
                        }
                    }
                    if (found == 1U) {
                        uRet = 0;
                    } else {
                        uRet = MEMORY_LOG_SLOT_FULL;
                    }
                }

                // release mutex
                if (AmbaKAL_MutexGive(&MemoryLogMutex) != 0U) {
                    uRet = MEMORY_LOG_ERR_MUTEX_UNLOCK;
                }
            }
        }
    }

    return uRet;
}

/**
* Memory log push address log
* @param [in] SlotId registered slot ID
* @param [in] pName pointer to name array
* @param [in] Address log address
* @param [in] Size log size
* @return ErrorCode
*
*/
UINT32 AmbaMemoryLog_PushAddressLog(UINT32 SlotId, const char *pName, UINT32 Address, UINT32 Size)
{
    UINT32 uRet = 0;
    if (IsInited == 0U) {
        uRet = MEMORY_LOG_ERR_NO_INIT;
    } else {
        if (SlotId < AMBA_MEMORY_LOG_MAX_ENTRY) {
            AMBA_MEMORY_LOG_ENTRY_s *ctrl = &MemLogInfo.LogEntry[SlotId];
            if (ctrl->Used != 0U) {
                // check NULL pointer
                if (ctrl->AddressPool != NULL) {
                    // check size
                    if ((ctrl->AddressWriteIdx + AMBA_MEMORY_LOG_ADDRESS_LOG_SIZE) <= ctrl->AddressPoolSize) {
                        if (AmbaKAL_MutexTake(&MemoryLogMutex, AMBA_KAL_WAIT_FOREVER) == 0U) {
                            UINT32 i;
                            UINT8  *ptr = &ctrl->AddressPool[ctrl->AddressWriteIdx];
                            // copy Name
                            for (i = 0; i < AMBA_MEMORY_LOG_ADDRESS_NAME_SIZE; i++) {
                                *ptr = (UINT8)pName[i];
                                ptr++;
                            }
                            // copy Address, little endian
                            *ptr = (UINT8)Address; ptr++;
                            *ptr = (UINT8)(Address >> 8U); ptr++;
                            *ptr = (UINT8)(Address >> 16U); ptr++;
                            *ptr = (UINT8)(Address >> 24U); ptr++;
                            // copy Size, little endian
                            *ptr = (UINT8)Size; ptr++;
                            *ptr = (UINT8)(Size >> 8U); ptr++;
                            *ptr = (UINT8)(Size >> 16U); ptr++;
                            *ptr = (UINT8)(Size >> 24U);

                            ctrl->AddressWriteIdx += AMBA_MEMORY_LOG_ADDRESS_LOG_SIZE;

                            // release mutex
                            if (AmbaKAL_MutexGive(&MemoryLogMutex) != 0U) {
                                uRet = MEMORY_LOG_ERR_MUTEX_UNLOCK;
                            }
                        } else {
                            uRet = MEMORY_LOG_ERR_MUTEX_LOCK;
                        }
                    } else {
                        uRet = MEMORY_LOG_BUFFER_FULL;
                    }
                } else {
                    uRet = MEMORY_LOG_BUFFER_NULL;
                }
            } else {
                uRet = MEMORY_LOG_ERR_INVALID_SLOTID;
            }
        } else {
            uRet = MEMORY_LOG_ERR_INVALID_SLOTID;
        }
    }
    return uRet;
}

static void MemoryCopyCharToU8(UINT8 *Dst, const char *Src, UINT32 Size)
{
    if ((Dst != NULL) && (Src != NULL) && (Size > 0U)) {
        UINT32 i;
        for (i = 0; i < Size; i++) {
            Dst[i] = (UINT8)Src[i];
        }
    }
}

/**
* Memory log push message log
* @param [in] SlotId registered slot ID
* @param [in] pString pointer to string
* @return ErrorCode
*
*/
UINT32 AmbaMemoryLog_PushMessageLog(UINT32 SlotId, const char *pString)
{
    UINT32 uRet = 0;

    if (IsInited == 0U) {
        uRet = MEMORY_LOG_ERR_NO_INIT;
    } else {
        if (SlotId < AMBA_MEMORY_LOG_MAX_ENTRY) {
            UINT32 Tmp;
            UINT32 StringLength = (UINT32)AmbaUtility_StringLength(pString);
            AMBA_MEMORY_LOG_ENTRY_s *ctrl = &MemLogInfo.LogEntry[SlotId];
            if (ctrl->Used != 0U) {
                if (ctrl->MessagePool != NULL) {
                    if (StringLength > ctrl->MessagePoolSize) {
                        // string length is over pool size
                        uRet = MEMORY_LOG_MESSAGE_TOO_LARGE;
                    } else {
                        if (AmbaKAL_MutexTake(&MemoryLogMutex, AMBA_KAL_WAIT_FOREVER) == 0U) {
                            UINT32 Head = ctrl->MessageWriteIdx;
                            if ((Head + StringLength) > ctrl->MessagePoolSize) {
                                /* Wrap around circular buffer */
                                Tmp = ctrl->MessagePoolSize - Head;
                                ctrl->MessageWriteIdx = StringLength - Tmp;
                                MemoryCopyCharToU8(&ctrl->MessagePool[Head], &pString[0], Tmp);
                                MemoryCopyCharToU8(&ctrl->MessagePool[0], &pString[Tmp], StringLength - Tmp);
                            } else {
                                /* No wrap */
                                ctrl->MessageWriteIdx = Head + StringLength;
                                MemoryCopyCharToU8(&ctrl->MessagePool[Head], pString, StringLength);
                            }
                            ctrl->MessageSize += StringLength;
                            // make sure no wrap around
                            if (ctrl->MessageSize > ctrl->MessagePoolSize) {
                                ctrl->MessageSize = ctrl->MessagePoolSize;
                            }
                            // release mutex
                            if (AmbaKAL_MutexGive(&MemoryLogMutex) != 0U) {
                                uRet = MEMORY_LOG_ERR_MUTEX_UNLOCK;
                            }
                        } else {
                            uRet = MEMORY_LOG_ERR_MUTEX_LOCK;
                        }
                    }
                } else {
                    uRet = MEMORY_LOG_BUFFER_NULL;
                }
            } else {
                uRet = MEMORY_LOG_ERR_INVALID_SLOTID;
            }
        } else {
            uRet = MEMORY_LOG_ERR_INVALID_SLOTID;
        }
    }
    return uRet;
}

/**
* Memory log get info
* @param [out] pInfo pointer of pointer to info struct
* @return 0
*
*/
UINT32 AmbaMemoryLog_GetInfo(const AMBA_MEMORY_LOG_INFO_s **pInfo)
{
    *pInfo = &MemLogInfo;
    return 0U;
}
