/**
 *  @file AmbaSYS_DispInfo.c
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
 *  @details Display the manual information of a specified module on the screen.
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaSYS.h"
#include "AmbaSYS_Ctrl.h"

static inline UINT32 ReadWord(ULONG MemAddr)
{
    const volatile UINT32 *pMem;
    AmbaMisra_TypeCast(&pMem, &MemAddr);
    return *pMem;
}

static void SYS_PrintHex(UINT32 NumDigit, UINT32 Value, AMBA_SYS_LOG_f LogFunc)
{
    static const char HexCharSet[20] = "0123456789ABCDEF";
    char StrBuf[12];
    UINT32 i, ActualDigits, NumPadding;

    /* Check how many characters are must-have */
    ActualDigits = 1U;
    while ((ActualDigits < 8UL) && (Value >= (1UL << (ActualDigits << 2UL)))) {
        ActualDigits ++;
    }

    /* Determine the number of padding characters */
    if (NumDigit < ActualDigits) {
        NumPadding = 0UL; /* No padding */
    } else if (NumDigit >= sizeof(StrBuf)) {
        NumPadding = (sizeof(StrBuf) - 1UL) - ActualDigits;
    } else {
        NumPadding = NumDigit - ActualDigits;
    }

    /* Fill character from end of string */
    StrBuf[NumPadding + ActualDigits] = '\0';
    for (i = 0U; i < ActualDigits; i ++) {
        StrBuf[(NumPadding + ActualDigits - 1U) - i] = HexCharSet[(Value >> (i << 2U)) & 0xFU];
    }

    while (NumPadding > 0UL) {
        StrBuf[NumPadding - 1U] = '0'; /* Zero padding */
        NumPadding --;
    }

    /* Output left aligned text */
    LogFunc(StrBuf);
}

static UINT32 SYS_DispMemRaw(ULONG MemAddr, UINT32 MemSize, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 MemData, RetVal = SYS_ERR_NONE;
    ULONG i;

    if (((MemAddr & 0x3U) != 0x0U) || ((MemSize & 0x3U) != 0x0U)) {
        /* This function is designed for register dump. The input arguments shall be word-aligned. */
        RetVal = SYS_ERR_ARG;
    } else {
        LogFunc("Offset     00       04       08       0C   \n");

        /* Only show offset info. For memory block information, it shall be shown outside this function. */
        for (i = 0U; i < MemSize; i++) {
            MemData = ReadWord(MemAddr + (i * 4U));
            if ((i & 0x3U) == 0x0U) {
                SYS_PrintHex(3UL, i * 4U, LogFunc);
                LogFunc(":    ");
            }
            SYS_PrintHex(8UL, MemData, LogFunc);

            if ((i & 0x3U) == 0x3U) {
                LogFunc("\n");
            } else {
                LogFunc(" ");
            }
        }

        if ((i & 0x3U) != 0x0U) {
            LogFunc("\n");
        }
    }

    return RetVal;
}

static UINT32 SYS_DispKalInfo(UINT32 ModuleID, UINT32 FuncID, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (ModuleID == SYS_LOG_KAL_CPU_LOAD) {
        if (FuncID == SYS_LOG_FUNC_RESET) {
            AmbaDiag_KalResetCpuLoadInfo();
        } else {
#ifdef CONFIG_ARM64
            AmbaDiag_KalShowCpuLoadInfo(LogFunc);
#else
            AmbaDiag_KalShowCpuLoadInfoA32(LogFunc);
#endif
        }
    } else if (ModuleID == SYS_LOG_KAL_TASK) {
#ifdef CONFIG_ARM64
        AmbaDiag_KalShowTaskInfo(LogFunc);
#else
        AmbaDiag_KalShowTaskInfoA32(LogFunc);
#endif
    } else if (ModuleID == SYS_LOG_KAL_MUTEX) {
#ifdef CONFIG_ARM64
        AmbaDiag_KalShowMutexInfo(LogFunc);
#else
        AmbaDiag_KalShowMutexInfoA32(LogFunc);
#endif
    } else if (ModuleID == SYS_LOG_KAL_SEMAPHORE) {
#ifdef CONFIG_ARM64
        AmbaDiag_KalShowSemaphoreInfo(LogFunc);
#else
        AmbaDiag_KalShowSemaphoreInfoA32(LogFunc);
#endif
    } else if (ModuleID == SYS_LOG_KAL_EVENTFLAGS) {
#ifdef CONFIG_ARM64
        AmbaDiag_KalShowEventFlagsInfo(LogFunc);
#else
        AmbaDiag_KalShowEventFlagsInfoA32(LogFunc);
#endif
    } else if (ModuleID == SYS_LOG_KAL_MSG_QUEUE) {
#ifdef CONFIG_ARM64
        AmbaDiag_KalShowMsgQueueInfo(LogFunc);
#else
        AmbaDiag_KalShowMsgQueueInfoA32(LogFunc);
#endif
    } else {
        RetVal = SYS_ERR_ARG;
    }

    return RetVal;
}

static UINT32 SYS_DispIntInfo(UINT32 ModuleID, UINT32 FuncID, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (ModuleID == SYS_LOG_INT) {
        if (FuncID == SYS_LOG_FUNC_RESET) {
            AmbaDiag_IntRestartProfiler();
        } else if (FuncID == SYS_LOG_FUNC_INIT) {
            AmbaDiag_IntEnableProfiler();
        } else if (FuncID == SYS_LOG_FUNC_NORMAL) {
            RetVal = AmbaDiag_IntShowInfo(0xFFFFFFFFU, LogFunc);
        } else {
            RetVal = AmbaDiag_IntShowInfo(FuncID - SYS_LOG_FUNC_CUSTOM, LogFunc);
        }
    } else {
        RetVal = SYS_ERR_ARG;
    }

    return RetVal;
}

static UINT32 SYS_DispDramInfo(UINT32 ModuleID, UINT32 FuncID, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (ModuleID == SYS_LOG_DRAM) {
        if (FuncID == SYS_LOG_FUNC_RESET) {
            AmbaDiag_DramResetStatis();
        } else {
            RetVal = AmbaDiag_DramShowStatisInfo(LogFunc);
        }
    } else {
        RetVal = SYS_ERR_ARG;
    }

    return RetVal;
}

static UINT32 SYS_DispVoutInfo(UINT32 ModuleID, UINT32 FuncID, AMBA_SYS_LOG_f LogFunc)
{
    ULONG MemAddr = 0U;
    UINT32 MemSize = 0U;
    UINT32 RetVal = SYS_ERR_NONE;

    if (ModuleID == SYS_LOG_VOUT) {
        if (FuncID == SYS_LOG_FUNC_RAW) {
            LogFunc("Vout Disp 0:\n");
            (void)AmbaDiag_VoutGetMmioAddr(VOUT_MMIO_DISP0, &MemAddr, &MemSize);
            (void)SYS_DispMemRaw(MemAddr, MemSize, LogFunc);
            LogFunc("Vout Disp 1:\n");
            (void)AmbaDiag_VoutGetMmioAddr(VOUT_MMIO_DISP1, &MemAddr, &MemSize);
            (void)SYS_DispMemRaw(MemAddr, MemSize, LogFunc);
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
            LogFunc("Vout Disp 2:\n");
            (void)AmbaDiag_VoutGetMmioAddr(VOUT_MMIO_DISP2, &MemAddr, &MemSize);
            (void)SYS_DispMemRaw(MemAddr, MemSize, LogFunc);
#endif

        } else {
            RetVal = AmbaDiag_VoutShowInfo(FuncID - SYS_LOG_FUNC_CUSTOM, LogFunc);
        }
    } else {
        RetVal = SYS_ERR_ARG;
    }

    return RetVal;
}

static UINT32 SYS_DispVinInfo(UINT32 ModuleID, UINT32 FuncID, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (FuncID == SYS_LOG_FUNC_NORMAL) {
        RetVal = AmbaDiag_VinShowInfo((ModuleID & 0xffffU), LogFunc);
    } else if (FuncID == SYS_LOG_FUNC_RESET) {
        RetVal = AmbaDiag_VinResetStatus((ModuleID & 0xffffU), LogFunc);
    } else {
        RetVal = SYS_ERR_ARG;
    }

    return RetVal;
}

UINT32 AmbaSYS_DispInfo(UINT32 ModuleID, UINT32 FuncID, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 ModuleType = ModuleID & 0xffff0000UL;
    UINT32 RetVal;

    if (ModuleType == KAL_ERR_BASE) {
        RetVal = SYS_DispKalInfo(ModuleID, FuncID, LogFunc);
    } else if (ModuleType == INT_ERR_BASE) {
        RetVal = SYS_DispIntInfo(ModuleID, FuncID, LogFunc);
    } else if (ModuleType == DRAM_ERR_BASE) {
        RetVal = SYS_DispDramInfo(ModuleID, FuncID, LogFunc);
    } else if (ModuleType == VOUT_ERR_BASE) {
        RetVal = SYS_DispVoutInfo(ModuleID, FuncID, LogFunc);
    } else if (ModuleType == VIN_ERR_BASE) {
        RetVal = SYS_DispVinInfo(ModuleID, FuncID, LogFunc);
    } else {
        RetVal = SYS_ERR_ARG;
    }

    return RetVal;
}

