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
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define DSP_LOG_TEXT_BASE_ADDR_CORE                 (0x600000U)
#define DSP_LOG_TEXT_BASE_ADDR_MDXF                 (0x1600000U)
#define DSP_LOG_TEXT_BASE_ADDR_MEMD                 (0x300000U)
#elif defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define DSP_LOG_TEXT_BASE_ADDR_CORE                 (0x500000U)
#define DSP_LOG_TEXT_BASE_ADDR_VIN                  (0x1800000U)
#define DSP_LOG_TEXT_BASE_ADDR_IDSP0                (0x1200000U)
#define DSP_LOG_TEXT_BASE_ADDR_IDSP1                (0x1500000U)
#define DSP_LOG_TEXT_BASE_ADDR_MDXF                 (0xB00000U)
#define DSP_LOG_TEXT_BASE_ADDR_MEMD                 (0x800000U)
#define DSP_LOG_TEXT_BASE_ADDR_MDXF1                (0x1E00000U)
#define DSP_LOG_TEXT_BASE_ADDR_MEMD1                (0x1B00000U)
#else
#define DSP_LOG_TEXT_BASE_ADDR_CORE                 (0x900000U)
#define DSP_LOG_TEXT_BASE_ADDR_MDXF                 (0x600000U)
#define DSP_LOG_TEXT_BASE_ADDR_MEMD                 (0x300000U)
#endif
#define AMBA_DEBUG_DSP_LOG_DATA_AREA_SIZE           (0x20000U)     /* DSP Debug Log Data Memory Size in Bytes */

#if defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define DSP_LOG_PARTITION_NUM    (4U)
#else
#define DSP_LOG_PARTITION_NUM    (1U)
#endif

/*
 * Data structure of DSP log message format
 */
typedef struct {
#if defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32  Tick;       /* audio-tick */
    UINT32  ThreadId:16;
    UINT32  DspCore:16;
#else
    UINT32  SeqNum;     /* Sequence number */
    UINT8   DspCore;
    UINT8   ThreadId;
    UINT16  Reserved;
#endif
    UINT32  FormatAddr;  /* Address (offset) to find '%s' arg */
    UINT32  Arg[5];     /* variable arguments */
} AMBA_DSP_LOG_MSG_s;

/*
 * Data structure of DSP log buffer control
 */
typedef struct {
    AMBA_DSP_LOG_MSG_s *pDspLogBuf[DSP_LOG_PARTITION_NUM]; /* aligned memory address */
    UINT32 DspLogBufSize[DSP_LOG_PARTITION_NUM]; /* in number of bytes */
    UINT32 NumDspMsg[DSP_LOG_PARTITION_NUM];
    UINT32 HeadIndex[DSP_LOG_PARTITION_NUM];
    UINT32 TailIndex[DSP_LOG_PARTITION_NUM];
    ULONG  CodeAddr;
    ULONG  MemdAddr;
    ULONG  MdxfAddr;
    ULONG  CodeTextAddr;
    ULONG  MemdTextAddr;
    ULONG  MdxfTextAddr;
#if defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    ULONG  Code1Addr;
    ULONG  Code2Addr;
    ULONG  Code3Addr;
    ULONG  Memd1Addr;
    ULONG  Mdxf1Addr;
    ULONG  Code1TextAddr;
    ULONG  Code2TextAddr;
    ULONG  Code3TextAddr;
    ULONG  Memd1TextAddr;
    ULONG  Mdxf1TextAddr;
#endif
    UINT32 IsCachedBuf;
    UINT32 IsUcodeSharing;
} AMBA_DSP_LOG_PRINT_CTRL_s;

static AMBA_DSP_LOG_PRINT_CTRL_s AmbaDspLogCtrl __attribute__((section(".bss.noinit")));

static void SHELL_DmesgCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [dsp] [-[num lines]] - Print ths last or messages\n");
    PrintFunc(" [dsp] [-[num lines]] [filename] - Print ths last or messages to a file\n");
}

static void DspLog_Init(void)
{
    static UINT32 DspLogInited = 0U;

    if (DspLogInited == 0U) {
        extern UINT32 AmbaWrap_memset(void *ptr, INT32 v, SIZE_t n);
        if (AmbaWrap_memset(&AmbaDspLogCtrl, 0, sizeof(AMBA_DSP_LOG_PRINT_CTRL_s))!= 0U) {
            // action TBD
        }
        DspLogInited = 1U;
    }
}

static UINT32 DspLog_GetUcodeBaseAddr(void)
{
    UINT32 ret = 0;
    AMBA_DSP_LOG_PRINT_CTRL_s *pDspLogCtrl = &AmbaDspLogCtrl;

    if ((pDspLogCtrl->CodeAddr == 0U) ||
        (pDspLogCtrl->CodeTextAddr == 0U)) {
        /* get DSP ucode base address */
        {
            extern UINT32 AmbaDSP_MainGetDspBinAddr(AMBA_DSP_BIN_ADDR_s *Info);
            AMBA_DSP_BIN_ADDR_s DspBin = {0};

            (void)AmbaDSP_MainGetDspBinAddr(&DspBin);
            pDspLogCtrl->CodeAddr = DspBin.CoreStartAddr;
            pDspLogCtrl->MemdAddr = DspBin.MeStartAddr;
            pDspLogCtrl->MdxfAddr = DspBin.MdxfStartAddr;
#if defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            pDspLogCtrl->Code1Addr = DspBin.Core1StartAddr;
            pDspLogCtrl->Code2Addr = DspBin.Core2StartAddr;
            pDspLogCtrl->Code3Addr = DspBin.Core3StartAddr;
            pDspLogCtrl->Memd1Addr = DspBin.Me1StartAddr;
            pDspLogCtrl->Mdxf1Addr = DspBin.Mdxf1StartAddr;
#endif
        }

        /* get DSP ucode text base address */
        pDspLogCtrl->CodeTextAddr = DSP_LOG_TEXT_BASE_ADDR_CORE;
        pDspLogCtrl->MemdTextAddr = DSP_LOG_TEXT_BASE_ADDR_MEMD;
        pDspLogCtrl->MdxfTextAddr = DSP_LOG_TEXT_BASE_ADDR_MDXF;
#if defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        pDspLogCtrl->Code1TextAddr = DSP_LOG_TEXT_BASE_ADDR_VIN;
        pDspLogCtrl->Code2TextAddr = DSP_LOG_TEXT_BASE_ADDR_IDSP0;
        pDspLogCtrl->Code3TextAddr = DSP_LOG_TEXT_BASE_ADDR_IDSP1;
        pDspLogCtrl->Memd1TextAddr = DSP_LOG_TEXT_BASE_ADDR_MEMD1;
        pDspLogCtrl->Mdxf1TextAddr = DSP_LOG_TEXT_BASE_ADDR_MDXF1;
#endif
    }

    return ret;
}

static UINT32 DspLog_WriteFormattedDataToString(char *pBuf, UINT32 BufferSize, const AMBA_DSP_LOG_MSG_s *pDspMsg)
{
    static const char *UnknownStr = "?";
    const AMBA_DSP_LOG_PRINT_CTRL_s *pDspLogCtrl = &AmbaDspLogCtrl;
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
#if !defined(CONFIG_SOC_CV5) && !defined(CONFIG_SOC_CV52)
        /* Resolved addresses of arguments */
        // pFmt = (char *) pDspMsg->FormatAddr;
        (void)pDspMsg->Reserved;
#endif

        AmbaMisra_TypeCast(&pFmt, &pDspMsg->FormatAddr);
        if (pFmt == NULL) {
            Len = 0;
        } else {

#if defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            Arg[0] = (UINT32)pDspMsg->ThreadId;
            Arg[1] = pDspMsg->Tick;
            if (pDspMsg->DspCore == 0U) {
                BaseAddr = pDspLogCtrl->MemdAddr;
                TextBaseAddr = pDspLogCtrl->MemdTextAddr;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "[MEMD0:th%d:%u] ", 2, Arg);
            } else if (pDspMsg->DspCore == 1U) {
                BaseAddr = pDspLogCtrl->Memd1Addr;
                TextBaseAddr = pDspLogCtrl->Memd1TextAddr;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "[MEMD1:th%d:%u] ", 2, Arg);
            } else if (pDspMsg->DspCore == 0x10U) {
                BaseAddr = pDspLogCtrl->MdxfAddr;
                TextBaseAddr = pDspLogCtrl->MdxfTextAddr;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "[MDXF0:th%d:%u] ", 2, Arg);
            } else if (pDspMsg->DspCore == 0x11U) {
                BaseAddr = pDspLogCtrl->Mdxf1Addr;
                TextBaseAddr = pDspLogCtrl->Mdxf1TextAddr;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "[MDXF1:th%d:%u] ", 2, Arg);
            } else if (pDspMsg->DspCore == 0x20U) {
                BaseAddr = pDspLogCtrl->CodeAddr;
                TextBaseAddr = pDspLogCtrl->CodeTextAddr;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "[CORE0:th%d:%u] ", 2, Arg);
            } else if (pDspMsg->DspCore == 0x21U) {
                BaseAddr = pDspLogCtrl->Code1Addr;
                TextBaseAddr = pDspLogCtrl->Code1TextAddr;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "[CORE1:th%d:%u] ", 2, Arg);
            } else if (pDspMsg->DspCore == 0x22U) {
                BaseAddr = pDspLogCtrl->Code2Addr;
                TextBaseAddr = pDspLogCtrl->Code2TextAddr;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "[CORE2:th%d:%u] ", 2, Arg);
            } else if (pDspMsg->DspCore == 0x23U) {
                if (0U < pDspLogCtrl->IsUcodeSharing) {
                    BaseAddr = pDspLogCtrl->Code2Addr;
                    TextBaseAddr = pDspLogCtrl->Code2TextAddr;
                } else {
                    BaseAddr = pDspLogCtrl->Code3Addr;
                    TextBaseAddr = pDspLogCtrl->Code3TextAddr;
                }
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "[CORE3:th%d:%u] ", 2, Arg);
            }
#else
            Arg[0] = (UINT32)pDspMsg->ThreadId - 1U;
            Arg[1] = pDspMsg->SeqNum;
            if (pDspMsg->DspCore == 0U) {
                BaseAddr = pDspLogCtrl->CodeAddr;   /* shall use physical address */
                TextBaseAddr = pDspLogCtrl->CodeTextAddr;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "[CORE:th%d:%u] ", 2, Arg);
            } else if (pDspMsg->DspCore == 1U) {
                BaseAddr = pDspLogCtrl->MdxfAddr;   /* shall use physical address */
                TextBaseAddr = pDspLogCtrl->MdxfTextAddr;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "[MDXF:th%d:%u] ", 2, Arg);
            } else if (pDspMsg->DspCore == 2U) {
                BaseAddr = pDspLogCtrl->MemdAddr;   /* shall use physical address */
                TextBaseAddr = pDspLogCtrl->MemdTextAddr;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "[MEMD:th%d:%u] ", 2, Arg);
            }
#endif
            else {
                isSkip = 1;
#if defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                Arg[0] = pDspMsg->DspCore;
                Arg[1] = pDspMsg->ThreadId;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "dsp_core = 0x%X thread = %u", 2, Arg);
#else
                Arg[0] = pDspMsg->ThreadId;
                Len = AmbaUtility_StringPrintUInt32(pBuf, BufferSize, "dsp_core = %u", 1, Arg);
#endif
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

static void DspLog_PrintMessage(AMBA_SHELL_PRINT_f PrintFunc, UINT32 DspMsgStartIndex, UINT32 DspMsgEndIndex, UINT32 PartitionIndex)
{
    const AMBA_DSP_LOG_MSG_s *pDspMsg;
    UINT32 i;
    UINT32 Len;
    static char AmbaDspLogBuf[1024] __attribute__((section(".bss.noinit")));
    ULONG  Addr;

    for (i = DspMsgStartIndex; i <= DspMsgEndIndex; i++) {
        pDspMsg = &AmbaDspLogCtrl.pDspLogBuf[PartitionIndex][i];
        if (AmbaDspLogCtrl.IsCachedBuf == 1U) {
            AmbaMisra_TypeCast(&Addr, &pDspMsg);
            (void)AmbaCache_DataInvalidate(Addr, sizeof(AMBA_DSP_LOG_MSG_s));
        }

        Len = DspLog_WriteFormattedDataToString(AmbaDspLogBuf, 1024, pDspMsg);
        if (Len == 0U) {
            continue;
        }

        if (AmbaDspLogBuf[Len - 1U] == '\n') {
            AmbaDspLogBuf[Len - 1U] = '\0';
        }

        PrintFunc(AmbaDspLogBuf);
        PrintFunc("\n");
    }
}

static UINT32 DspLog_GetMaxNumDspMsg(UINT32 PartitionIndex)
{
    return AmbaDspLogCtrl.NumDspMsg[PartitionIndex];
}

static void DspLog_GetLastMessage(AMBA_DSP_LOG_PRINT_CTRL_s *pDspLogCtrl, UINT32 NumLines, UINT32 PartitionIndex)
{
#define MSG_GRP_NUM (32U)
    const AMBA_DSP_LOG_MSG_s *pDspMsg;
    UINT32 DspMsgGroupIndex = 0;   /* We define 32 messages as 1 group. This is used to speedup the search. */
    UINT32 DspMsgIndex = 0;
    UINT32 MaxDspMsgIndex;
    UINT32 i;
    UINT32 NumLinesX = NumLines;
    ULONG  Addr;

    pDspMsg = &pDspLogCtrl->pDspLogBuf[PartitionIndex][0];
    MaxDspMsgIndex = DspLog_GetMaxNumDspMsg(PartitionIndex) - 1U;

    /* Search the latest sequence number */
    for (i = MaxDspMsgIndex - (MSG_GRP_NUM - 1U); i > (MSG_GRP_NUM - 1U); i -= MSG_GRP_NUM) {
        if (AmbaDspLogCtrl.IsCachedBuf == 1U) {
            AmbaMisra_TypeCast(&Addr, &pDspMsg);
            (void)AmbaCache_DataInvalidate((Addr + (i*sizeof(AMBA_DSP_LOG_MSG_s))), sizeof(AMBA_DSP_LOG_MSG_s));
            (void)AmbaCache_DataInvalidate((Addr + (DspMsgGroupIndex*sizeof(AMBA_DSP_LOG_MSG_s))), sizeof(AMBA_DSP_LOG_MSG_s));
        }
#if defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (pDspMsg[i].Tick > pDspMsg[DspMsgGroupIndex].Tick)
#else
        if (pDspMsg[i].SeqNum > pDspMsg[DspMsgGroupIndex].SeqNum)
#endif
        {
            DspMsgGroupIndex = i;
            break;
        }
    }

    for (i = (DspMsgGroupIndex + (MSG_GRP_NUM - 1U)); i >= DspMsgGroupIndex; i--) {
        if (AmbaDspLogCtrl.IsCachedBuf == 1U) {
            AmbaMisra_TypeCast(&Addr, &pDspMsg);
            (void)AmbaCache_DataInvalidate((Addr + (i*sizeof(AMBA_DSP_LOG_MSG_s))), sizeof(AMBA_DSP_LOG_MSG_s));
            (void)AmbaCache_DataInvalidate((Addr + (DspMsgGroupIndex*sizeof(AMBA_DSP_LOG_MSG_s))), sizeof(AMBA_DSP_LOG_MSG_s));
        }

#if defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (pDspMsg[i].Tick >= pDspMsg[DspMsgGroupIndex].Tick)
#else
        if (pDspMsg[i].SeqNum >= pDspMsg[DspMsgGroupIndex].SeqNum)
#endif
        {
            DspMsgIndex = i;
            break;
        }
    }

    /* get the tail log location */
    pDspLogCtrl->TailIndex[PartitionIndex] = DspMsgIndex;

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

    pDspLogCtrl->HeadIndex[PartitionIndex] = DspMsgIndex;
}

static void AmbaDspLog_ShowMessage(AMBA_SHELL_PRINT_f PrintFunc, UINT32 NumLines, UINT32 PartitionIndex)
{
    AMBA_DSP_LOG_PRINT_CTRL_s *pDspLogCtrl = &AmbaDspLogCtrl;
    UINT32 DspMsgStartIndex;
    UINT32 DspMsgEndIndex;
    UINT32 MaxDspMsgIndex;

    if (PartitionIndex >= DSP_LOG_PARTITION_NUM) {
        // DO NOTHING
    } else if ((NumLines == 0U) || (pDspLogCtrl->pDspLogBuf[PartitionIndex] == NULL)) {
        // no action
    } else {
        /* Get the DSP log range */
        DspLog_GetLastMessage(pDspLogCtrl, NumLines, PartitionIndex);

        MaxDspMsgIndex = DspLog_GetMaxNumDspMsg(PartitionIndex) - 1U;
        /* Show log messages */
        if (pDspLogCtrl->HeadIndex[PartitionIndex] > pDspLogCtrl->TailIndex[PartitionIndex]) {
            DspMsgStartIndex = pDspLogCtrl->HeadIndex[PartitionIndex];
            DspMsgEndIndex = MaxDspMsgIndex;
            DspLog_PrintMessage(PrintFunc, DspMsgStartIndex, DspMsgEndIndex, PartitionIndex);

            DspMsgStartIndex = 0;
            DspMsgEndIndex = pDspLogCtrl->TailIndex[PartitionIndex];
            DspLog_PrintMessage(PrintFunc, DspMsgStartIndex, DspMsgEndIndex, PartitionIndex);
        } else {
            DspMsgStartIndex = pDspLogCtrl->HeadIndex[PartitionIndex];
            DspMsgEndIndex = pDspLogCtrl->TailIndex[PartitionIndex];
            DspLog_PrintMessage(PrintFunc, DspMsgStartIndex, DspMsgEndIndex, PartitionIndex);
        }
    }
}

static void SHELL_DmesgDspDumpMessage(AMBA_SHELL_PRINT_f PrintFunc, UINT32 NumMessage, UINT32 PartitionIndex)
{
    AmbaDspLog_ShowMessage(PrintFunc, NumMessage, PartitionIndex);
    return;
}

UINT32 AmbaShell_CommandDspLogInit(const void *pDspDebugLogBuf, UINT32 DspDebugLogSize)
{
    AMBA_DSP_LOG_PRINT_CTRL_s *pDspLogCtrl = &AmbaDspLogCtrl;
    UINT32 uRet = 0, DspDebugLogSizeX = DspDebugLogSize;
    UINT32 i;

    DspLog_Init();

    if (pDspDebugLogBuf == NULL) {
        uRet = 0xFFFFFFFFU;
    } else {
        AMBA_DSP_LOG_MSG_s *ptr = NULL;
        ULONG Addr;
        UINT8 *pU8 = NULL;

        if (DspDebugLogSizeX == 0U) {
            DspDebugLogSizeX = AMBA_DEBUG_DSP_LOG_DATA_AREA_SIZE/DSP_LOG_PARTITION_NUM;
        } else {
            DspDebugLogSizeX /= DSP_LOG_PARTITION_NUM;
        }

        AmbaMisra_TypeCast(&pU8, &pDspDebugLogBuf);
        for (i = 0U; i < DSP_LOG_PARTITION_NUM; i++) {
            AmbaMisra_TypeCast(&ptr, &pU8);
            pDspLogCtrl->pDspLogBuf[i] = ptr;
            pDspLogCtrl->DspLogBufSize[i] = DspDebugLogSizeX;
            pDspLogCtrl->NumDspMsg[i] = DspDebugLogSizeX / sizeof(AMBA_DSP_LOG_MSG_s);

            pU8 = &(pU8[DspDebugLogSizeX]);
        }

        AmbaMisra_TypeCast(&Addr, &pDspDebugLogBuf);

        pDspLogCtrl->IsCachedBuf = 1U;

    #if defined(CONFIG_ICAM_UCODE_ORCIDSP_SHARING)
        pDspLogCtrl->IsUcodeSharing = 1U;
    #else
        pDspLogCtrl->IsUcodeSharing = 0U;
    #endif
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
        UINT32 PartitionMask = 0xFFU, PartitionIndex;
        char ArgvStrBuf[16] = {'\0'};
        char *pChar = &(ArgvStrBuf[0]);

        AmbaUtility_StringCopy(ArgvStrBuf, sizeof(ArgvStrBuf), pArgVector[2]);

        PrintFunc("Insert Argv[2]: ");
        PrintFunc(ArgvStrBuf);
        PrintFunc("\n\r");

        while ((*pChar != '\0') && (*pChar != 'm')) {
            pChar ++;
        }

        if (*pChar == 'm') {
            *pChar = '\0';
            pChar ++;
            (void)AmbaUtility_StringToUInt32(pChar, &PartitionMask);
        }

        if (ArgvStrBuf[0] != '-') {
            (void)AmbaUtility_StringToUInt32(ArgvStrBuf, &NumMessage);
        } else {
            (void)AmbaUtility_StringToUInt32(&ArgvStrBuf[1], &NumMessage);
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
                for (PartitionIndex = 0U; PartitionIndex < DSP_LOG_PARTITION_NUM; PartitionIndex ++) {
                    if ((PartitionMask & (0x1UL << PartitionIndex)) > 0U) {
                        SHELL_DmesgDspDumpMessage(Print2File, NumMessage, PartitionIndex);
                    }
                }
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
                for (PartitionIndex = 0U; PartitionIndex < DSP_LOG_PARTITION_NUM; PartitionIndex ++) {
                    if ((PartitionMask & (0x1UL << PartitionIndex)) > 0U) {
                        SHELL_DmesgDspDumpMessage(PrintFunc, NumMessage, PartitionIndex);
                    }
                }
            }
        } else {
            SHELL_DmesgCmdUsage(pArgVector, PrintFunc);
        }
    }

    return;
}
