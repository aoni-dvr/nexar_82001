/**
 *  @file AmbaShell_DisplayMessage.c
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details dmesg (display message or driver message) shell command.
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaMisraFix.h>
#include <AmbaShell.h>
#include <AmbaFS.h>
#include <AmbaUtility.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"
#include <AmbaMMU.h>
#include <AmbaCache.h>
#include "AmbaDSP.h"

/* Following defines shall be update base on different chip */
#ifdef CONFIG_SOC_CV2FS
#define DSP_LOG_TEXT_BASE_ADDR_CORE                 (0x600000U)
#define DSP_LOG_TEXT_BASE_ADDR_MDXF                 (0x1600000U)
#define DSP_LOG_TEXT_BASE_ADDR_MEMD                 (0x300000U)
#else
#define DSP_LOG_TEXT_BASE_ADDR_CORE                 (0x900000U)
#define DSP_LOG_TEXT_BASE_ADDR_MDXF                 (0x600000U)
#define DSP_LOG_TEXT_BASE_ADDR_MEMD                 (0x300000U)
#endif
#define AMBA_DEBUG_DSP_LOG_DATA_AREA_SIZE           (0x20000U)     /* DSP Debug Log Data Memory Size in Bytes */

/*
 * Data structure of DSP log message format
 */
typedef struct {
    UINT32  SeqNum;     /* Sequence number */
    UINT8   DspCore;
    UINT8   ThreadId;
    UINT16  Reserved;
    UINT32  FormatAddr; /* Address (offset) to find '%s' arg */
    UINT32  Arg[5];     /* variable arguments */
} AMBA_DSP_LOG_MSG_s;

/*
 * Data structure of DSP log buffer control
 */
typedef struct {
    AMBA_DSP_LOG_MSG_s *pDspLogBuf;     /* aligned memory address */
    UINT32 DspLogBufSize;               /* in number of bytes */
    UINT32 NumDspMsg;
    UINT32 HeadIndex;
    UINT32 TailIndex;
    ULONG  CodeAddr;
    ULONG  MemdAddr;
    ULONG  MdxfAddr;
    ULONG  CodeTextAddr;
    ULONG  MemdTextAddr;
    ULONG  MdxfTextAddr;
    UINT32 IsCachedBuf;
} AMBA_DSP_LOG_PRINT_CTRL_s;

static AMBA_DSP_LOG_PRINT_CTRL_s _AmbaDspLogCtrl = {
    NULL,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static UINT32 IsUcodeFunctionValid = 1;

static void SHELL_DmesgCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [dsp] [-[num lines]] - Print ths last or messages\n");
    PrintFunc(" [dsp] [-[num lines]] [filename] - Print ths last or messages to a file\n");
}

static UINT32 DspLog_GetUcodeBaseAddr(void)
{
    UINT32 ret = 0;
    AMBA_DSP_LOG_PRINT_CTRL_s *pDspLogCtrl = &_AmbaDspLogCtrl;

    /* get DSP ucode base address */
    {
        extern UINT32 AmbaDSP_MainGetDspBinAddr(AMBA_DSP_BIN_ADDR_s *Info);
        AMBA_DSP_BIN_ADDR_s DspBin = {0};

        (void)AmbaDSP_MainGetDspBinAddr(&DspBin);
        pDspLogCtrl->CodeAddr = DspBin.CoreStartAddr;
        pDspLogCtrl->MemdAddr = DspBin.MeStartAddr;
        pDspLogCtrl->MdxfAddr = DspBin.MdxfStartAddr;
    }

    /* get DSP ucode text base address */
    pDspLogCtrl->CodeTextAddr = DSP_LOG_TEXT_BASE_ADDR_CORE;
    pDspLogCtrl->MemdTextAddr = DSP_LOG_TEXT_BASE_ADDR_MEMD;
    pDspLogCtrl->MdxfTextAddr = DSP_LOG_TEXT_BASE_ADDR_MDXF;

    return ret;
}

static UINT32 DspLog_WriteFormattedDataToString(char *pBuf, UINT32 BufferSize, const AMBA_DSP_LOG_MSG_s *pDspMsg)
{
    static const char *UnknownStr = "?";
    const AMBA_DSP_LOG_PRINT_CTRL_s *pDspLogCtrl = &_AmbaDspLogCtrl;
    const char *pFmt;
    UINT32 Len = 0;
    ULONG  BaseAddr = 0;
    ULONG  TextBaseAddr = 0;
    UINT32 NumArgs = 0;
    UINT32 Arg[5] = {0x0, 0x0, 0x0, 0x0, 0x0};
    const char *pStr;
    UINT32 ArgData = 0;
    ULONG  FmtAddress = 0U;

    if ((DspLog_GetUcodeBaseAddr() != 0U) || (pDspMsg == NULL)) {
        Len = 0;
    } else {
        UINT32 isSkip = 0;
        /* Resolved addresses of arguments */
        // pFmt = (char *) pDspMsg->FormatAddr;
        (void)pDspMsg->Reserved;

        AmbaMisra_TypeCast(&pFmt, &pDspMsg->FormatAddr);
        if (pDspMsg->FormatAddr == NULL) {
            Len = 0;
        } else {
            Arg[0] = (UINT32)pDspMsg->ThreadId - 1U;
            Arg[1] = pDspMsg->SeqNum;
            if (pDspMsg->DspCore == 0U) {
                BaseAddr = pDspLogCtrl->CodeAddr;
                TextBaseAddr = pDspLogCtrl->CodeTextAddr;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "[CORE:th%d:%u] ", 2, Arg);
            } else if (pDspMsg->DspCore == 1U) {
                BaseAddr = pDspLogCtrl->MdxfAddr;
                TextBaseAddr = pDspLogCtrl->MdxfTextAddr;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "[MDXF:th%d:%u] ", 2, Arg);
            } else if (pDspMsg->DspCore == 2U) {
                BaseAddr = pDspLogCtrl->MemdAddr;
                TextBaseAddr = pDspLogCtrl->MemdTextAddr;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "[MEMD:th%d:%u] ", 2, Arg);
            } else {
                isSkip = 1;
                Arg[0] = pDspMsg->ThreadId;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "dsp_core = %u", 1, Arg);
            }

            if (isSkip == 0U) {
                if (BaseAddr == 0U) {
                    //pFmt = (char *) UnknownStr;
                    AmbaMisra_TypeCast(&pFmt, &UnknownStr);
                    NumArgs = 0;
                } else {
                    //pFmt = (char *) (BaseAddr + (((UINT32) pFmt) - TextBaseAddr));
                    FmtAddress = (BaseAddr + pDspMsg->FormatAddr) - TextBaseAddr;
                    AmbaMisra_TypeCast(&pFmt, &FmtAddress);
                    pStr = pFmt;
                    for (NumArgs = 0; NumArgs < 5U; NumArgs++) {
                        for ( ; ((*pStr != '\0') && (*pStr != '%')); pStr++) {
                            /* make misra happy */
                        }
                        if (*pStr == '\0') {
                            break;
                        }
                        pStr++;

                        ArgData = pDspMsg->Arg[NumArgs];
                        if ((*pStr == 's') && (ArgData != 0x0U)) {
                            Arg[NumArgs] = (UINT32)(BaseAddr + (((UINT32) ArgData) - TextBaseAddr));
                        } else {
                            Arg[NumArgs] = ArgData;
                        }
                    }
                }

                /* Print message */
                switch (NumArgs) {
                case 5:
                    Len += AmbaUtility_StringPrintUInt32(&pBuf[Len], BufferSize - Len, pFmt, 5, Arg);
                    break;
                case 4:
                    Len += AmbaUtility_StringPrintUInt32(&pBuf[Len], BufferSize - Len, pFmt, 4, Arg);
                    break;
                case 3:
                    Len += AmbaUtility_StringPrintUInt32(&pBuf[Len], BufferSize - Len, pFmt, 3, Arg);
                    break;
                case 2:
                    Len += AmbaUtility_StringPrintUInt32(&pBuf[Len], BufferSize - Len, pFmt, 2, Arg);
                    break;
                case 1:
                    Len += AmbaUtility_StringPrintUInt32(&pBuf[Len], BufferSize - Len, pFmt, 1, Arg);
                    break;
                case 0:
                default:
                    Len += AmbaUtility_StringPrintUInt32(&pBuf[Len], BufferSize - Len, pFmt, 0, Arg);
                    break;
                }
            }
        }
    }

    return Len;
}

static void DspLog_PrintMessage(AMBA_SHELL_PRINT_f PrintFunc, UINT32 DspMsgStartIndex, UINT32 DspMsgEndIndex)
{
    const AMBA_DSP_LOG_MSG_s *pDspMsg;
    UINT32 i;
    UINT32 Len;
    static char _AmbaDspLogBuf[1024] __attribute__((section(".bss.noinit")));
    ULONG  Addr;

    for (i = DspMsgStartIndex; i <= DspMsgEndIndex; i++) {
        pDspMsg = &_AmbaDspLogCtrl.pDspLogBuf[i];
        if (_AmbaDspLogCtrl.IsCachedBuf == 1U) {
            AmbaMisra_TypeCast(&Addr, &pDspMsg);
            (void)AmbaCache_DataInvalidate(Addr, sizeof(AMBA_DSP_LOG_MSG_s));
        }

        Len = DspLog_WriteFormattedDataToString(_AmbaDspLogBuf, 1024, pDspMsg);
        if (Len == 0U) {
            continue;
        }

        if (_AmbaDspLogBuf[Len - 1U] == '\n') {
            _AmbaDspLogBuf[Len - 1U] = '\0';
        }

        PrintFunc(_AmbaDspLogBuf);
        PrintFunc("\n");
    }
}

static UINT32 DspLog_GetMaxNumDspMsg(void)
{
    return _AmbaDspLogCtrl.NumDspMsg;
}

static void DspLog_GetLastMessage(AMBA_DSP_LOG_PRINT_CTRL_s *pDspLogCtrl, UINT32 NumLines)
{
    const AMBA_DSP_LOG_MSG_s *pDspMsg;
    UINT32 DspMsgGroupIndex = 0;   /* We define 32 messages as 1 group. This is used to speedup the search. */
    UINT32 DspMsgIndex = 0;
    UINT32 MaxDspMsgIndex;
    UINT32 i;
    UINT32 NumLinesX = NumLines;
    ULONG  Addr;

    pDspMsg = &pDspLogCtrl->pDspLogBuf[0];
    MaxDspMsgIndex = DspLog_GetMaxNumDspMsg() - 1U;

    /* Search the latest sequence number */
    for (i = MaxDspMsgIndex - 31U; i > (32U - 1U); i -= 32U) {
        if (_AmbaDspLogCtrl.IsCachedBuf == 1U) {
            AmbaMisra_TypeCast(&Addr, &pDspMsg);
            (void)AmbaCache_DataInvalidate((Addr + (i*sizeof(AMBA_DSP_LOG_MSG_s))), sizeof(AMBA_DSP_LOG_MSG_s));
            (void)AmbaCache_DataInvalidate((Addr + (DspMsgGroupIndex*sizeof(AMBA_DSP_LOG_MSG_s))), sizeof(AMBA_DSP_LOG_MSG_s));
        }

        if (pDspMsg[i].SeqNum > pDspMsg[DspMsgGroupIndex].SeqNum) {
            DspMsgGroupIndex = i;
            break;
        }
    }

    for (i = (DspMsgGroupIndex + (32U - 1U)); i >= DspMsgGroupIndex; i--) {
        if (_AmbaDspLogCtrl.IsCachedBuf == 1U) {
            AmbaMisra_TypeCast(&Addr, &pDspMsg);
            (void)AmbaCache_DataInvalidate((Addr + (i*sizeof(AMBA_DSP_LOG_MSG_s))), sizeof(AMBA_DSP_LOG_MSG_s));
            (void)AmbaCache_DataInvalidate((Addr + (DspMsgGroupIndex*sizeof(AMBA_DSP_LOG_MSG_s))), sizeof(AMBA_DSP_LOG_MSG_s));
        }

        if (pDspMsg[i].SeqNum >= pDspMsg[DspMsgGroupIndex].SeqNum) {
            DspMsgIndex = i;
            break;
        }
    }

    /* get the tail log location */
    pDspLogCtrl->TailIndex = DspMsgIndex;

    /* get the head log location */
    NumLinesX = NumLinesX - 1U;
    if (NumLinesX >= MaxDspMsgIndex) { /* if the NumLines >= MaxDspMsgIndex, then print all DSP messages */
        DspMsgIndex = DspMsgIndex + 1U;
        if (DspMsgIndex > MaxDspMsgIndex) {
            DspMsgIndex = 0;
        }
    } else {
        if (DspMsgIndex >= NumLinesX) {
            DspMsgIndex = DspMsgIndex - NumLinesX;
        } else {
            DspMsgIndex = MaxDspMsgIndex + DspMsgIndex - NumLinesX;
        }
    }

    pDspLogCtrl->HeadIndex = DspMsgIndex;
}

static void AmbaDspLog_ShowMessage(AMBA_SHELL_PRINT_f PrintFunc, UINT32 NumLines)
{
    AMBA_DSP_LOG_PRINT_CTRL_s *pDspLogCtrl = &_AmbaDspLogCtrl;
    UINT32 DspMsgStartIndex;
    UINT32 DspMsgEndIndex;
    UINT32 MaxDspMsgIndex;

    if ((NumLines == 0U) || (pDspLogCtrl->pDspLogBuf == NULL)) {
        // no action
    } else {
        /* Get the DSP log range */
        DspLog_GetLastMessage(pDspLogCtrl, NumLines);

        MaxDspMsgIndex = DspLog_GetMaxNumDspMsg() - 1U;
        /* Show log messages */
        if (pDspLogCtrl->HeadIndex > pDspLogCtrl->TailIndex) {
            DspMsgStartIndex = pDspLogCtrl->HeadIndex;
            DspMsgEndIndex = MaxDspMsgIndex;
            DspLog_PrintMessage(PrintFunc, DspMsgStartIndex, DspMsgEndIndex);

            DspMsgStartIndex = 0;
            DspMsgEndIndex = pDspLogCtrl->TailIndex;
            DspLog_PrintMessage(PrintFunc, DspMsgStartIndex, DspMsgEndIndex);
        } else {
            DspMsgStartIndex = pDspLogCtrl->HeadIndex;
            DspMsgEndIndex = pDspLogCtrl->TailIndex;
            DspLog_PrintMessage(PrintFunc, DspMsgStartIndex, DspMsgEndIndex);
        }
    }
}

static void SHELL_DmesgDspDumpMessage(AMBA_SHELL_PRINT_f PrintFunc, UINT32 NumMessage)
{
    AmbaDspLog_ShowMessage(PrintFunc, NumMessage);
    return;
}

UINT32 AmbaShell_CommandDspLogInit(const void *pDspDebugLogBuf, UINT32 DspDebugLogSize)
{
    AMBA_DSP_LOG_PRINT_CTRL_s *pDspLogCtrl = &_AmbaDspLogCtrl;
    UINT32 uRet = 0, DspDebugLogSizeX = DspDebugLogSize;

    if (pDspDebugLogBuf == NULL) {
        uRet = 0xFFFFFFFFU;
    } else {
        AMBA_DSP_LOG_MSG_s *ptr = NULL;

        if (DspDebugLogSizeX == 0U) {
            DspDebugLogSizeX = AMBA_DEBUG_DSP_LOG_DATA_AREA_SIZE;
        }

        AmbaMisra_TypeCast(&ptr, &pDspDebugLogBuf);
        pDspLogCtrl->pDspLogBuf = ptr;
        pDspLogCtrl->DspLogBufSize = DspDebugLogSizeX;
        pDspLogCtrl->NumDspMsg = _AmbaDspLogCtrl.DspLogBufSize / sizeof(AMBA_DSP_LOG_MSG_s);

        pDspLogCtrl->IsCachedBuf = 0U;
    }
    return uRet;
}

static AMBA_FS_FILE *pLogFile = NULL;
static void Print2File(const char *pMessage)
{
    if (pLogFile != NULL) {
        UINT32 Length = AmbaUtility_StringLength(pMessage);
        UINT32 ByteWritten;
        UINT32 uRet;
        void *DataPtr = NULL;

        AmbaMisra_TypeCast(&DataPtr, &pMessage);
        if (Length > 0U) {
            uRet = AmbaFS_FileWrite(DataPtr, 1, Length, pLogFile, &ByteWritten);
            if ((uRet != 0U) || (ByteWritten != Length)) {
                // ignore error
            }
        }
    }
}

void AmbaShell_CommandDisplayMsg(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NumMessage = 0;
    UINT32 uRet;

    if (ArgCount < 3U) {
        SHELL_DmesgCmdUsage(pArgVector, PrintFunc);
    } else {
        if (pArgVector[2][0] != '-') {
            (void)AmbaUtility_StringToUInt32(pArgVector[2], &NumMessage);
        } else {
            (void)AmbaUtility_StringToUInt32(&pArgVector[2][1], &NumMessage);
        }

        if (ArgCount == 4U) {
            uRet = AmbaFS_FileOpen(pArgVector[3], "w", &pLogFile);
            if (uRet != 0U) {
                PrintFunc("Can't open ");
                PrintFunc(pArgVector[3]);
                PrintFunc("\n");
            }
        }

        if (AmbaUtility_StringCompare(pArgVector[1], "dsp", 3) == 0) {
            if (pLogFile != NULL) {
                SHELL_DmesgDspDumpMessage(Print2File, NumMessage);
                if (AmbaFS_FileClose(pLogFile) != 0U) {
                    PrintFunc("Can't close ");
                    PrintFunc(pArgVector[3]);
                    PrintFunc("\n");
                } else {
                    PrintFunc(pArgVector[3]);
                    PrintFunc(" saved.\n");
                }
                pLogFile = NULL;
            } else {
                SHELL_DmesgDspDumpMessage(PrintFunc, NumMessage);
            }
        } else {
            SHELL_DmesgCmdUsage(pArgVector, PrintFunc);
        }
    }

    return;
}
