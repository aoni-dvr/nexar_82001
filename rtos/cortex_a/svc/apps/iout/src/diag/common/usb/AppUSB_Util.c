/**
 *  @file AppUSB_Util.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *    @details Utility functions for usb unit tests.
 */

#ifndef APP_USB_H
#include "AppUSB.h"
#endif
#ifndef AMBAUSB_DEVICE_API_H
#include "AmbaUSBD_API.h"
#endif
#ifndef APPUSB_UTIL_H
#include "AppUSB_Util.h"
#endif
#include "AmbaUSB_ErrCode.h"
#include "AmbaUtility.h"
#include <AmbaMisraFix.h>

/** \addtogroup APPLICATION_APIS
 *  @{
 * */
/**
 * OS wrapper function for task create.
*/
UINT32 AppUsb_TaskCreate(
    AMBA_KAL_TASK_t *pTask,
    char *           pTaskName,
    UINT32           Priority,
    void *(*EntryFunction)(void *Arg),
    void *           EntryArg,
    void *           pStackBase,
    UINT32           StackByteSize,
    UINT32           AutoStart,
    UINT32           AffinityMask)
{
    // create taks with no start.
    UINT32 sRet;
    UINT32 Ret;
    static UINT32 CoreMask = AMBA_KAL_CPU_CORE_MASK;

    AmbaMisra_TouchUnused(EntryArg);

    sRet = AmbaKAL_TaskCreate(pTask,
                              pTaskName,
                              Priority,
                              EntryFunction,
                              EntryArg,
                              pStackBase,
                              StackByteSize,
                              0U);

    if (sRet == TX_SUCCESS) {
        UINT32 TmpAffMsk = AffinityMask;
        TmpAffMsk &= CoreMask;
        if (AmbaKAL_TaskSetSmpAffinity(pTask, TmpAffMsk) != TX_SUCCESS) {
            Ret = USB_ERR_FAIL;
        } else {
            if (AutoStart == 1U) {
                // kick task start.
                if (AmbaKAL_TaskResume(pTask) != TX_SUCCESS) {
                    Ret = USB_ERR_FAIL;
                } else {
                    Ret = USB_ERR_SUCCESS;
                }
            } else {
                Ret = USB_ERR_SUCCESS;
            }
        }
    } else {
        Ret = USB_ERR_FAIL;
    }

    return Ret;
}
/**
 * OS wrapper function for task sleep.
*/
VOID AppUsb_TaskSleep(UINT32 Ms)
{
    (VOID)AmbaKAL_TaskSleep(Ms);
}
/**
 * OS wrapper function for task resume.
*/
UINT32 AppUsb_TaskResume(AMBA_KAL_TASK_t *pTask)
{
    UINT32 uRet = 0;

    if (AmbaKAL_TaskResume(pTask) != TX_SUCCESS) {
        uRet = USB_ERR_FAIL;
    }
    return uRet;
}
/**
 * OS wrapper function for task suspend.
*/
UINT32 AppUsb_TaskSuspend(AMBA_KAL_TASK_t *pTask)
{
    UINT32 uRet = AmbaKAL_TaskSuspend(pTask);

    if (uRet != TX_SUCCESS) {
        AppUsb_PrintUInt5("AppUsb_TaskSuspend(): return 0x%X", uRet, 0, 0, 0, 0);
        uRet = USB_ERR_FAIL;
    }
    return uRet;
}

/**
 * OS wrapper function for task delete.
*/
UINT32 AppUsb_TaskDelete(AMBA_KAL_TASK_t *pTask, UINT32 Suspended)
{
    UINT32 Ret;
    UINT32 Leave = 0;

    if (Suspended != 0U) {
        AMBA_KAL_TASK_INFO_s TaskInfo;
        UINT32 CheckCnt = 10;
        do {
            if (AmbaKAL_TaskQuery(pTask, &TaskInfo) == TX_SUCCESS) {
                // Hanle sleep and all other suspend state.
                if (TaskInfo.TaskState >= 5U) { // 5 is sleep state in KAL API
                    if (tx_thread_wait_abort(pTask) != TX_SUCCESS) {
                        // shall not be here.
                    }
                }
                CheckCnt--;
                if (CheckCnt == 0U) {
                    Leave = 1;
                }
            } else {
                Leave = 1;
            }

            if (Leave != 0U) {
                break;
            }
            AppUsb_TaskSleep(1);
        } while (TaskInfo.TaskState != 3U);
    }

    if (AmbaKAL_TaskTerminate(pTask) == TX_SUCCESS) {
        if (AmbaKAL_TaskDelete(pTask) == TX_SUCCESS) {
            Ret = USB_ERR_SUCCESS;
        } else {
            Ret = USB_ERR_FAIL;
        }
    } else {
        Ret = USB_ERR_FAIL;
    }

    return Ret;
}
/**
 * Convert the UINT32 value to string
*/
UINT32 AppUsb_UInt32ToStr(char *Buffer, UINT32 BufSize, UINT32 Value, UINT32 Radix)
{
    return AmbaUtility_UInt32ToStr(Buffer, BufSize, Value, Radix);
}

/**
 * Get the length of the string
*/
UINT32 AppUsb_StringLength(const char *pString)
{
    UINT32 idx = 0;

    while (pString[idx] != '\0') {
        idx++;
    }
    return idx;
}
/**
 * Append source string to the destination string buffer.
*/
VOID AppUsb_StringCat(char *pDestination, UINT32 BufferSize, const char *Source)
{
    UINT32 DestLen   = AppUsb_StringLength(pDestination);
    UINT32 SourceLen = AppUsb_StringLength(Source);

    if ((DestLen + SourceLen + 1U) > BufferSize) {
        // over buffer size, do nothing
    } else {
        UINT32 idx;
        for (idx = 0; idx < SourceLen; idx++) {
            pDestination[DestLen + idx] = Source[idx];
        }
        pDestination[DestLen + SourceLen] = '\0';
    }
}
/**
 * Convert the UINT32 value into string and append it to the string buffer.
*/
VOID AppUsb_StringAppendUInt32(char *Buffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix)
{
    char IntString[UTIL_MAX_INT_STR_LEN];

    if (Buffer != NULL) {
        (void)AppUsb_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Value, Radix);
        AppUsb_StringCat(Buffer, BufferSize, IntString);
    }
}
/**
 * Set the range of memory to all zero
*/
VOID  AppUsb_MemoryZeroSet(void *pDest, UINT32 Size)
{
    if (AmbaWrap_memset(pDest, 0, Size) != 0U) {
        // ignore error
    }
}
/**
 * Set the range of memory to the specific value.
*/
VOID  AppUsb_MemorySet(void *pDest, INT32 Value, UINT32 Size)
{
    if (AmbaWrap_memset(pDest, Value, Size) != 0U) {
        // ignore error
    }
}

VOID  AppUsb_MemoryCopy(void *pDest, const void *pSrc, SIZE_t Size)
{
    if (AmbaWrap_memcpy(pDest, pSrc, Size) != 0U) {
        // ignore error
    }
}

/**
 * Translate String to UINT32 value
*/
VOID AppUsb_StringToUINT32(const char *pString, UINT32 *pValue)
{
    if (pValue != NULL) {
        UINT32 uret = AmbaUtility_StringToUInt32(pString, pValue);

        if (uret != 0U) {
            *pValue = 0;
        }
    }
}

/**
 * Convert UINT8 pointer to void pointer
*/
VOID* AppUsb_U8p2Vp(const UINT8 *Ptr)
{
    const UINT8 *SrcPtr = Ptr;
    VOID *DesPtr;

    AppUsb_MemoryCopy(&DesPtr, &SrcPtr, sizeof(UINT8*));

    return DesPtr;
}
/**
 * Convert void pointer to UINT8 pointer
*/
UINT8* AppUsb_Vp2U8p(const VOID *Ptr)
{
    const VOID *SrcPtr = Ptr;
    UINT8 *DesPtr;

    AppUsb_MemoryCopy(&DesPtr, &SrcPtr, sizeof(VOID*));

    return DesPtr;
}

/**
 * Convert ascii string to unicode string
*/
VOID AppUsb_Ascii2Unicode(const char *pAsciiStr, WCHAR *pUnicodeStr)
{

    SIZE_t len, i;
    char char_code;
    WCHAR wchar_code;
    if((pAsciiStr != NULL) && (pUnicodeStr != NULL)) {
        len = AmbaUtility_StringLength(pAsciiStr);
        for (i=0; i<len; i++) {
            char_code = *pAsciiStr;
            wchar_code = (WCHAR)char_code;
            *pUnicodeStr = wchar_code;
            pUnicodeStr++;
            pAsciiStr++;
        }
    }
}

/**
 * Get string length for the wide-string
*/
SIZE_t AppUsb_WStrLen(const WCHAR *WString)
{
    SIZE_t len = 0;

    while (WString != NULL) {
        if (*WString == 0x0000U) {
            break;
        }
        len ++;
        WString ++;
    }

    return len;
}

void AppUsb_PrintStr5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    AmbaPrint_PrintStr5(pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
    //AmbaPrint_Flush();
}
void AppUsb_Print(const char *pFmt)
{
    AmbaPrint_PrintUInt5(pFmt, 0, 0, 0, 0, 0);
    //AmbaPrint_Flush();
}
void AppUsb_PrintUInt5(const char *pFmt, UINT32 pArg1, UINT32 pArg2, UINT32 pArg3, UINT32 pArg4, UINT32 pArg5)
{
    AmbaPrint_PrintUInt5(pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
    //AmbaPrint_Flush();
}
void AppUsb_PrintInt5(const char *pFmt, INT32 pArg1, INT32 pArg2, INT32 pArg3, INT32 pArg4, INT32 pArg5)
{
    AmbaPrint_PrintInt5(pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
    //AmbaPrint_Flush();
}
/** @} */
