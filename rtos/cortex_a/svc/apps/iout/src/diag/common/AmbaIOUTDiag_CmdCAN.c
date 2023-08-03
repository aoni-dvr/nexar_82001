/**
 *  @file AmbaIOUTDiag_CmdCAN.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details CAN diagnostic Command
 */

#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif
#include "AmbaShell.h"
#include "AmbaCAN.h"
#include "AmbaGPIO.h"
#include "AmbaWrap.h"
#include <AmbaUtility.h>
#include <AmbaIOUTDiag.h>
#include "AmbaPrint.h"

#define IoDiagCanMaxBufSize      4096
#define IoDiagCanFDMaxBufSize    4096

static const UINT8 IoDiag_CanDlcTable[AMBA_NUM_CAN_DATA_LENGTH] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64
};

typedef struct {
    UINT32              ChannelId;
    AMBA_CAN_CONFIG_s   Config;
} AMBA_SHELL_CAN_CTRL_s;

static UINT32 IoDiag_CanMsgCounter;
static UINT32 IoDiag_CanFdMsgCounter;
static UINT32 IoDiag_CanTxComplCounter;

static UINT8 IoDiagCanRingBuf[IoDiagCanMaxBufSize] __attribute__ ((aligned(AMBA_CACHE_LINE_SIZE))) __attribute__((section(".bss.noinit")));
static UINT8 IoDiagCanFDRingBuf[IoDiagCanFDMaxBufSize] __attribute__ ((aligned(AMBA_CACHE_LINE_SIZE))) __attribute__((section(".bss.noinit")));

static AMBA_KAL_TASK_t IoDiag_CanRecvTask;
static AMBA_KAL_TASK_t IoDiag_CanFdRecvTask;
static AMBA_KAL_TASK_t IoDiag_CanTxCbTask;


/* Common CANC and CAN 2.0 configurations */
static AMBA_SHELL_CAN_CTRL_s IoDiag_CanCtrl = {
    //.ChannelId = AMBA_CAN_CHANNEL0,
    .Config = {                                     /* Common CANC and CAN 2.0 configurations */
        .OpMode             = 0,                    /* 0: Normal mode; 1: Inside-loopback mode; 2: Outside-loopback mode; 3: Listen Mode */
        .EnableDMA          = 1,
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        .EnableAa           = 0,                    /* Auto Answer Mode */
#endif

        .TimeQuanta = {                             /* Bit-rate = RefClock / (( 1 + (PropSeg + PhaseSeg1 + 1) + (PhaseSeg2 + 1)) * (Prescaler + 1)) */
            .PhaseSeg2      = 1,                    /* E.g., To enable 1M bps transfer, it might set PropSeg = 0, Seg2 = 1, Seg1 = 4, SJW = 0, and  */
            .PhaseSeg1      = 4,                    /*       BRP = 2. Then, the bit-rate is 24M/((1 + 2 + 5) * 3) = 1M bps.      */
            .SJW            = 0,
            .BRP            = 2,
            .PropSeg        = 0,
        },

        .MaxRxRingBufSize   = IoDiagCanMaxBufSize,   /* maximum receive ring-buffer size in bytes */
        .pRxRingBuf         = IoDiagCanRingBuf,      /* Pointer to the Rx ring buffer */

        .FdTimeQuanta       = {                      // Don't care if FdMaxRxRingBufSize is 0
            .PhaseSeg2      = 1,                     /* E.g., To enable 1M bps transfer, it might set PropSeg = 0, Seg2 = 1, Seg1 = 2, SJW = 0, and  */
            .PhaseSeg1      = 2,                     /*       BRP = 2. Then, the bit-rate is 24M/((1 + 2 + 3) * 2) = 2M bps.      */
            .SJW            = 0,
            .BRP            = 1,
            .PropSeg        = 0,
        },
        .FdSsp              = {                      // Don't care if FdMaxRxRingBufSize is 0
            .TdcEnable      = 1,                     /* Tx delay compensation enable */
            .Mode           = 0,                     /* 0: Delay measurement mode. 1: Fixed value mode */
            .Offset         = 1,                     /* SSP offset (Delay measurement mode) in reference clock period*/
            .Position       = 0,                     /* SSP position (Fixed value mode) in minimum time quanta */
        },

        .FdMaxRxRingBufSize = IoDiagCanFDMaxBufSize,  /* maximum receive ring-buffer size in bytes for CAN FD, set 0 to use classic can (CAN 2.0)*/
        .pFdRxRingBuf       = IoDiagCanFDRingBuf,     /* Pointer to the Rx ring buffer for CAN FD */

        .NumIdFilter        = 0,                      /* Number of ID filters */
        .pIdFilter          = NULL,                   /* Pointer to the ID filters */
        .TxComplFunc        = NULL,                   /* Deprecated */

    },
};


static void IoDiag_CanCmdUsage(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    (void) ArgCount;
    (void) pArgVector;

    PrintFunc(" can start [ch] [mode]: Enable CANC and create receiving tasks\n");
    PrintFunc("   [ch]        : controller channel ID\n");
    PrintFunc("   [mode]      : 'cc' means classic can. 'fd' means can fd. \n");
    PrintFunc(" can write [id] [extention] [rtr] [length] [data] : Write a CAN message\n");
    PrintFunc("   [id]        : message ID\n");
    PrintFunc("   [extention] : set to 1 to enable extented ID\n");
    PrintFunc("   [rtr]       : set to 1 to make a RTR message\n");
    PrintFunc("   [length]    : the data length\n");
    PrintFunc("   [data]      : the message data\n");
    PrintFunc(" can fdwrite [id] [extention] [length] [data] : Write a CAN FD message\n");
    PrintFunc("   [id]        : message ID\n");
    PrintFunc("   [extention] : set to 1 to enable extented ID\n");
    PrintFunc("   [length]    : the data length\n");
    PrintFunc("   [data]      : the message data\n");
    PrintFunc(" can stop : Disable CANC and delete receiving tasks\n");
}

static AMBA_SHELL_PRINT_f IoDiag_CanVoidPtr2PrintFunc(const void *value)
{
    AMBA_SHELL_PRINT_f Desc;

    if (AmbaWrap_memcpy(&Desc, &value, sizeof(void*)) != 0U) {
        /* Do nothing */
    }

    return Desc;
}

static void *IoDiag_CanPrintFunc2VoidPtr(AMBA_SHELL_PRINT_f fp)
{
    AMBA_SHELL_PRINT_f TmpAddr = fp;
    void *value;

    if (AmbaWrap_memcpy(&value, &TmpAddr, sizeof(AMBA_SHELL_PRINT_f)) != 0U) {
        /* Do nothing */
    }

    return value;
}

static void IoDiag_CanPrintUInt32(AMBA_SHELL_PRINT_f PrintFunc,
                                  const char *pFmtString,
                                  UINT32 Count,
                                  UINT32 Arg1,
                                  UINT32 Arg2,
                                  UINT32 Arg3,
                                  UINT32 Arg4,
                                  UINT32 Arg5)
{
    char StrBuf[256];
    UINT32 Arg[5];

    Arg[0] = Arg1;
    Arg[1] = Arg2;
    Arg[2] = Arg3;
    Arg[3] = Arg4;
    Arg[4] = Arg5;

    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, Count, Arg);
    PrintFunc(StrBuf);
}

static void IoDiag_CanEnable(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Retstatus;

    // determine channel ID
    if (ArgCount == 2U) { // channel 0 and can fd by default.
        IoDiag_CanCtrl.ChannelId                 = AMBA_CAN_CHANNEL0;
        // Enable can fd
        IoDiag_CanCtrl.Config.FdMaxRxRingBufSize = IoDiagCanFDMaxBufSize;
        IoDiag_CanCtrl.Config.pFdRxRingBuf       = IoDiagCanFDRingBuf;
    } else if (ArgCount == 3U) { // can fd by default
        UINT32 ch;
        (void) AmbaUtility_StringToUInt32(pArgVector[2], &ch);
        // Enable can fd
        IoDiag_CanCtrl.Config.FdMaxRxRingBufSize = IoDiagCanFDMaxBufSize;
        IoDiag_CanCtrl.Config.pFdRxRingBuf       = IoDiagCanFDRingBuf;
        IoDiag_CanCtrl.ChannelId = ch;
    } else if (ArgCount == 4U) {
        UINT32 ch;
        (void) AmbaUtility_StringToUInt32(pArgVector[2], &ch);
        IoDiag_CanCtrl.ChannelId = ch;
        if (AmbaUtility_StringCompare(pArgVector[3], "cc", 2U) == 0) { // classic can
            // Disable can fd to use classice can (CAN 2.0)
            IoDiag_CanCtrl.Config.FdMaxRxRingBufSize = 0;
            IoDiag_CanCtrl.Config.pFdRxRingBuf = NULL;
        } else if (AmbaUtility_StringCompare(pArgVector[3], "fd", 2U) == 0) {
            // Enable can fd
            IoDiag_CanCtrl.Config.FdMaxRxRingBufSize = IoDiagCanFDMaxBufSize;
            IoDiag_CanCtrl.Config.pFdRxRingBuf       = IoDiagCanFDRingBuf;
        } else {  // can fd by default
            // Enable can fd
            IoDiag_CanCtrl.Config.FdMaxRxRingBufSize = IoDiagCanFDMaxBufSize;
            IoDiag_CanCtrl.Config.pFdRxRingBuf       = IoDiagCanFDRingBuf;
        }
    } else {
        IoDiag_CanPrintUInt32(PrintFunc, "Unknown input arguments, arg count = %d\n", 1, ArgCount, 0, 0, 0, 0);
    }

    // setup tx/rx path
    if (IoDiag_CanCtrl.ChannelId == AMBA_CAN_CHANNEL0) {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_098_CAN0_RX);
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_099_CAN0_TX);
    } else if (IoDiag_CanCtrl.ChannelId == AMBA_CAN_CHANNEL1) {
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_100_CAN1_RX);
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_101_CAN1_TX);
    } else if (IoDiag_CanCtrl.ChannelId == AMBA_CAN_CHANNEL2) {
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_044_CAN2_RX);
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_048_CAN2_TX);
    } else if (IoDiag_CanCtrl.ChannelId == AMBA_CAN_CHANNEL3) {
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_053_CAN3_RX);
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_054_CAN3_TX);
    } else if (IoDiag_CanCtrl.ChannelId == AMBA_CAN_CHANNEL4) {
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_055_CAN4_RX);
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_056_CAN4_TX);
    } else if (IoDiag_CanCtrl.ChannelId == AMBA_CAN_CHANNEL5) {
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_057_CAN5_RX);
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_058_CAN5_TX);
#elif defined(CONFIG_SOC_CV28)
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_81_CAN_RX);
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_82_CAN_TX);
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_127_CAN0_RX);
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_128_CAN0_TX);
    } else if (IoDiag_CanCtrl.ChannelId == AMBA_CAN_CHANNEL1) {
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_129_CAN1_RX);
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_130_CAN1_TX);
#else
#error "Unsupported chip version"
#endif
    } else {
        /* Should not be here */
        IoDiag_CanPrintUInt32(PrintFunc, "Unknown channel ID %d\n", 1, IoDiag_CanCtrl.ChannelId, 0, 0, 0, 0);
    }

    Retstatus = AmbaCAN_Enable(IoDiag_CanCtrl.ChannelId, &IoDiag_CanCtrl.Config);
    if (Retstatus != CAN_ERR_NONE) {
        IoDiag_CanPrintUInt32(PrintFunc, "Enable CANC fail = %d\n", 1, Retstatus, 0, 0, 0, 0);
    }
}

static void IoDiag_CanGetInfo(AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_CAN_BIT_INFO_s BitInfo;
    AMBA_CAN_FILTER_s FilterInfo[32];
    UINT32 FilterLen;
    UINT32 Ret;

    Ret = AmbaCAN_GetInfo(IoDiag_CanCtrl.ChannelId, &BitInfo, &FilterLen, FilterInfo);
    if (Ret == CAN_ERR_NONE) {
        UINT32 i;
        IoDiag_CanPrintUInt32(PrintFunc, "Ch[%d], System Clock = %d, NominalTQ = %d (ns), DataTQ = %d (ns), NominalBitLen = %d (tq)\n", 5,
                              IoDiag_CanCtrl.ChannelId,
                              BitInfo.SysClkFreq,
                              BitInfo.NominalTQ,
                              BitInfo.DataTQ,
                              BitInfo.NominalBitLen);
        // show data bit rate info under can fd mode.
        if (IoDiag_CanCtrl.Config.FdMaxRxRingBufSize != 0U) {
            IoDiag_CanPrintUInt32(PrintFunc, "DataBitLen = %d (tq), NominalBitRate = %d (bps), DataBitRate = %d (bps)\n", 3,
                                  BitInfo.DataBitLen,
                                  BitInfo.NominalBitRate,
                                  BitInfo.DataBitRate,
                                  0,
                                  0);
        }
        for (i=0; i<FilterLen; i++) {
            IoDiag_CanPrintUInt32(PrintFunc, "[%d] IdFilter = 0x%x, IdMask = 0x%x\n", 3,
                                  i,
                                  FilterInfo[i].IdFilter,
                                  FilterInfo[i].IdMask,
                                  0,
                                  0);
        }
    }
}

static void *IoDiag_CanReadTaskEntry(void *EntryArg)
{
    AMBA_CAN_MSG_s CanMessage;
    UINT32 RetStatus;
    UINT32 i;
    AMBA_SHELL_PRINT_f PrintFunc = IoDiag_CanVoidPtr2PrintFunc(EntryArg);
    (void) EntryArg; // keep misra happy, or const void* is needed. But compile would fail.

    for (;;) {
        /* Get one message from Rx Ring Buffer */
        RetStatus = AmbaCAN_Read(IoDiag_CanCtrl.ChannelId, &CanMessage, AMBA_KAL_WAIT_FOREVER);
        if ((RetStatus == CAN_ERR_NONE) || (RetStatus == CAN_ERR_MIX_FORMAT)) {
            if (RetStatus == CAN_ERR_MIX_FORMAT) {
                PrintFunc("\n [ERROR] Mixed-format network is not allowed ! \n");
                PrintFunc("\t enable CAN-FD but receive CAN-2.0 packet. \n\n");
            }
            /* Show the CAN messgae */
            IoDiag_CanPrintUInt32(PrintFunc, "Count [%d]", 1, IoDiag_CanMsgCounter, 0, 0, 0, 0);
            IoDiag_CanPrintUInt32(PrintFunc, "CH: %d, ID: 0x%x, EXT: %d, RTR: %d, DLC: %d, Data: ", 5,
                                  IoDiag_CanCtrl.ChannelId,
                                  CanMessage.Id,
                                  CanMessage.Extension,
                                  CanMessage.RemoteTxReq,
                                  CanMessage.DataLengthCode);

            for (i = 0; i < IoDiag_CanDlcTable[CanMessage.DataLengthCode]; i++) {
                IoDiag_CanPrintUInt32(PrintFunc, "%x ", 1, CanMessage.Data[i], 0, 0, 0, 0);
            }
            PrintFunc("\n");
            IoDiag_CanMsgCounter++;
        } else {
            IoDiag_CanPrintUInt32(PrintFunc, "Read fail (%d)\n", 1, RetStatus, 0, 0, 0, 0);
            break;
        }
    }
    return NULL;
}


static void *IoDiag_CanFdReadTaskEntry(void *EntryArg)
{
    AMBA_CAN_FD_MSG_s CanMessage;
    UINT32 RetStatus;
    UINT32 i;
    AMBA_SHELL_PRINT_f PrintFunc = IoDiag_CanVoidPtr2PrintFunc(EntryArg);
    (void) EntryArg; // keep misra happy, or const void* is needed. But compile would fail.

    for (;;) {
        /* Get one message from Rx Ring Buffer */
        RetStatus = AmbaCAN_ReadFd(IoDiag_CanCtrl.ChannelId, &CanMessage, AMBA_KAL_WAIT_FOREVER);
        if (RetStatus == CAN_ERR_NONE) {
            /* Show the CAN FD messgae */
            IoDiag_CanPrintUInt32(PrintFunc, "FD Count [%d] CH: %d, ID: 0x%x", 3, IoDiag_CanFdMsgCounter, IoDiag_CanCtrl.ChannelId, CanMessage.Id, 0, 0);

            IoDiag_CanPrintUInt32(PrintFunc, "EXT: %d, ESI: %d, BRS: %d, DLC: %d, Data: ", 4,
                                  CanMessage.Extension,
                                  CanMessage.ErrorStateIndicator,
                                  CanMessage.BitRateSwitch,
                                  IoDiag_CanDlcTable[CanMessage.DataLengthCode],
                                  0);


            for (i = 0; i < IoDiag_CanDlcTable[CanMessage.DataLengthCode]; i++) {
                IoDiag_CanPrintUInt32(PrintFunc, "%x ", 1, CanMessage.Data[i], 0, 0, 0, 0);
            }
            PrintFunc("\n");

            IoDiag_CanFdMsgCounter++;
        } else {
            IoDiag_CanPrintUInt32(PrintFunc, "Read fail (%d)\n", 1, RetStatus, 0, 0, 0, 0);
            break;
        }
    }
    return NULL;
}

static void IoDiag_CanRead(AMBA_SHELL_PRINT_f PrintFunc)
{
    static UINT8 IoDiag_CanRecvTaskStack[2048] GNU_SECTION_NOZEROINIT;
    static UINT8 IoDiag_CanFdRecvTaskStack[2048] GNU_SECTION_NOZEROINIT;
    static char can_recv_task_name[] = "IoDiagCanRecvTask";
    static char can_fd_recv_task_name[] = "IoDiagCanFdRecvTask";
    UINT32 Retstatus;

    IoDiag_CanMsgCounter = 0;
    IoDiag_CanFdMsgCounter = 0;

    Retstatus = AmbaKAL_TaskCreate(&IoDiag_CanRecvTask,                     /* pTask */
                                   can_recv_task_name,                      /* pTaskName */
                                   249,                                     /* Priority */
                                   IoDiag_CanReadTaskEntry,                 /* EntryFunction */
                                   IoDiag_CanPrintFunc2VoidPtr(PrintFunc),  /* EntryArg */
                                   IoDiag_CanRecvTaskStack,                 /* pStackBase */
                                   sizeof(IoDiag_CanRecvTaskStack),         /* StackByteSize */
                                   1);                                      /* AutoStart */
    if (Retstatus != KAL_ERR_NONE) {
        IoDiag_CanPrintUInt32(PrintFunc, "Create CAN message receiving tasks fail = %d\n", 1, Retstatus, 0, 0, 0, 0);
    }

    // Only create can fd read task under can fd mode.
    if (IoDiag_CanCtrl.Config.FdMaxRxRingBufSize != 0U) {
        Retstatus = AmbaKAL_TaskCreate(&IoDiag_CanFdRecvTask,                   /* pTask */
                                       can_fd_recv_task_name,                   /* pTaskName */
                                       249,                                     /* Priority */
                                       IoDiag_CanFdReadTaskEntry,               /* EntryFunction */
                                       IoDiag_CanPrintFunc2VoidPtr(PrintFunc),  /* EntryArg */
                                       IoDiag_CanFdRecvTaskStack,               /* pStackBase */
                                       sizeof(IoDiag_CanFdRecvTaskStack),       /* StackByteSize */
                                       1);                                      /* AutoStart */
        if (Retstatus != KAL_ERR_NONE) {
            IoDiag_CanPrintUInt32(PrintFunc, "Create CAN FD message receiving tasks fail = %d\n", 1, Retstatus, 0, 0, 0, 0);
        }
    }
}

static void *IoDiag_CanTxCbTaskEntry(void *EntryArg)
{
    UINT32 RetStatus;
    AMBA_SHELL_PRINT_f PrintFunc = IoDiag_CanVoidPtr2PrintFunc(EntryArg);
    UINT32 TrackId;
    (void) EntryArg; // keep misra happy, or const void* is needed. But compile would fail.

    for (;;) {
        /* Get one message from Rx Ring Buffer */
        RetStatus = AmbaCAN_WaitForTxComplete(IoDiag_CanCtrl.ChannelId, &TrackId);
        if (RetStatus == CAN_ERR_NONE) {
            /* Show the CAN messgae */
            IoDiag_CanPrintUInt32(PrintFunc, "Count [%d]: TrackId = 0x%x\n", 2, IoDiag_CanTxComplCounter, TrackId, 0, 0, 0);

            IoDiag_CanTxComplCounter++;
        } else {
            IoDiag_CanPrintUInt32(PrintFunc, "Wait for tx complete fail (%d)\n", 1, RetStatus, 0, 0, 0, 0);
            break;
        }
    }
    return NULL;
}

static void IoDiag_CanTxCb(AMBA_SHELL_PRINT_f PrintFunc)
{
    static UINT8 IoDiag_CanTxCbTaskStack[2048] GNU_SECTION_NOZEROINIT;
    static char can_tx_cb_task_name[] = "IoDiagCanTxCbTask";

    UINT32 Retstatus;

    IoDiag_CanTxComplCounter = 0;

    Retstatus = AmbaKAL_TaskCreate(&IoDiag_CanTxCbTask,                     /* pTask */
                                   can_tx_cb_task_name,                      /* pTaskName */
                                   249,                                     /* Priority */
                                   IoDiag_CanTxCbTaskEntry,                 /* EntryFunction */
                                   IoDiag_CanPrintFunc2VoidPtr(PrintFunc),  /* EntryArg */
                                   IoDiag_CanTxCbTaskStack,                 /* pStackBase */
                                   sizeof(IoDiag_CanTxCbTaskStack),         /* StackByteSize */
                                   1);                                      /* AutoStart */
    if (Retstatus != KAL_ERR_NONE) {
        IoDiag_CanPrintUInt32(PrintFunc, "Create CAN transmit callback tasks fail = %d\n", 1, Retstatus, 0, 0, 0, 0);
    }

}

static UINT32 IoDiag_CanCheckLEN(UINT32 Length, UINT8 *pCode)
{
    UINT32 Ret;
    UINT8 i;

    for (i = 0U; i < AMBA_NUM_CAN_DATA_LENGTH; i++) {
        if (Length <= IoDiag_CanDlcTable[i]) {
            *pCode = i;
            break;
        }
    }

    if (i == AMBA_NUM_CAN_DATA_LENGTH) {
        *pCode = IoDiag_CanDlcTable[AMBA_NUM_CAN_DATA_LENGTH - 1U];
        Ret = CAN_ERR_ARG;
    } else {
        Ret = CAN_ERR_NONE;
    }

    return Ret;
}

static void IoDiag_CanWrite(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    const AMBA_CAN_CONFIG_s *CanConfig = &IoDiag_CanCtrl.Config;
    AMBA_CAN_MSG_s CanMessage;
    UINT32 i;
    UINT32 RetStatus;
    UINT32 TmpValue;

    (void) ArgCount;

    if (CanConfig->OpMode == AMBA_CAN_OP_MODE_LISTEN) {
        PrintFunc("In listen mode, no message is sent.\n");
    } else if (CanConfig->FdMaxRxRingBufSize != 0U) {
        PrintFunc("In can fd mode, no classic can message is sent.\n");
    } else {
        (void) AmbaUtility_StringToUInt32(pArgVector[2], &TmpValue);
        CanMessage.Id          = TmpValue;
        (void) AmbaUtility_StringToUInt32(pArgVector[3], &TmpValue);
        CanMessage.Extension   = (UINT8)(TmpValue & 0xFFU);
        (void) AmbaUtility_StringToUInt32(pArgVector[4], &TmpValue);
        CanMessage.RemoteTxReq = (UINT8)(TmpValue & 0xFFU);
        CanMessage.Priority    = 0;
        (void) AmbaUtility_StringToUInt32(pArgVector[5], &TmpValue);
        // The maximum data length of classical CAN is 8 bytes.
        if (TmpValue > 8U) {
            TmpValue = 8;
        }
        (void) IoDiag_CanCheckLEN(TmpValue, &CanMessage.DataLengthCode);
        for (i = 0; i < IoDiag_CanDlcTable[CanMessage.DataLengthCode]; i++) {
            (void) AmbaUtility_StringToUInt32(pArgVector[6U+i], &TmpValue);
            CanMessage.Data[i] = (UINT8)(TmpValue & 0xFFU);
        }
        CanMessage.TrackID    = CanMessage.Id; // user defined
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        CanMessage.AutoAnswer = 0; // not set auto answer flag
#endif

        RetStatus = AmbaCAN_Write(IoDiag_CanCtrl.ChannelId, &CanMessage, 100);

        if (RetStatus == CAN_ERR_NONE) {

        } else if (RetStatus == CAN_ERR_MIX_FORMAT) {
            PrintFunc("\n [ERROR] Mixed-format network is not allowed ! \n");
            PrintFunc("\t enable CAN-FD but send CAN-2.0 packet. \n\n");
        } else {
            IoDiag_CanPrintUInt32(PrintFunc, "Write failure, status = 0x%x\n\r", 1, RetStatus, 0, 0, 0, 0);
        }
    }
}

static void IoDiag_CanWriteFd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    const AMBA_CAN_CONFIG_s *CanConfig = &IoDiag_CanCtrl.Config;
    AMBA_CAN_FD_MSG_s CanMessage;
    UINT32 i;
    UINT32 RetStatus;
    UINT32 TmpValue;

    (void) ArgCount;

    if (CanConfig->OpMode == AMBA_CAN_OP_MODE_LISTEN) {
        PrintFunc("In listen mode, no message is sent.\n");
    } else if (CanConfig->FdMaxRxRingBufSize == 0U) {
        PrintFunc("In classic can mode, no fd message is sent.\n");
    } else {
        (void) AmbaUtility_StringToUInt32(pArgVector[2], &TmpValue);
        CanMessage.Id          = TmpValue;
        (void) AmbaUtility_StringToUInt32(pArgVector[3], &TmpValue);
        CanMessage.Extension   = (UINT8)(TmpValue & 0xFFU);
        CanMessage.Priority    = 0;
        (void) AmbaUtility_StringToUInt32(pArgVector[4], &TmpValue);
        // The maximum data length of CAN FD is 64 bytes.
        if (TmpValue > 64U) {
            TmpValue = 64;
        }
        (void) IoDiag_CanCheckLEN(TmpValue, &CanMessage.DataLengthCode);
        CanMessage.BitRateSwitch = 1;
        for (i = 0; i < IoDiag_CanDlcTable[CanMessage.DataLengthCode]; i++) {
            (void) AmbaUtility_StringToUInt32(pArgVector[5U+i], &TmpValue);
            CanMessage.Data[i] = (UINT8)(TmpValue & 0xFFU);
        }
        CanMessage.TrackID    = CanMessage.Id; // user defined
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        CanMessage.AutoAnswer = 0; // not set auto answer flag
#endif

        RetStatus = AmbaCAN_WriteFd(IoDiag_CanCtrl.ChannelId, &CanMessage, 1000);
        if (RetStatus != CAN_ERR_NONE) {
            IoDiag_CanPrintUInt32(PrintFunc, "Write FD failure, status = 0x%x\n\r", 1, RetStatus, 0, 0, 0, 0);
        }
    }

}

static void IoDiag_CanDisable(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Retstatus;

    IoDiag_CanMsgCounter = 0;
    IoDiag_CanFdMsgCounter = 0;
    IoDiag_CanTxComplCounter = 0;

    Retstatus = AmbaKAL_TaskTerminate(&IoDiag_CanRecvTask);
    if (Retstatus != KAL_ERR_NONE) {
        IoDiag_CanPrintUInt32(PrintFunc, "Terminate CAN message receiving tasks fail = %d\n", 1, Retstatus, 0, 0, 0, 0);
    }
    Retstatus = AmbaKAL_TaskDelete(&IoDiag_CanRecvTask);
    if (Retstatus != KAL_ERR_NONE) {
        IoDiag_CanPrintUInt32(PrintFunc, "Delete CAN message receiving tasks fail = %d\n", 1, Retstatus, 0, 0, 0, 0);
    }

    if (IoDiag_CanCtrl.Config.FdMaxRxRingBufSize != 0U) {
        Retstatus = AmbaKAL_TaskTerminate(&IoDiag_CanFdRecvTask);
        if (Retstatus != KAL_ERR_NONE) {
            IoDiag_CanPrintUInt32(PrintFunc, "Terminate CAN FD message receiving tasks fail = %d\n", 1, Retstatus, 0, 0, 0, 0);
        }
        Retstatus = AmbaKAL_TaskDelete(&IoDiag_CanFdRecvTask);
        if (Retstatus != KAL_ERR_NONE) {
            IoDiag_CanPrintUInt32(PrintFunc, "Delete CAN FD message receiving tasks fail = %d\n", 1, Retstatus, 0, 0, 0, 0);
        }
    }

    Retstatus = AmbaKAL_TaskTerminate(&IoDiag_CanTxCbTask);
    if (Retstatus != KAL_ERR_NONE) {
        IoDiag_CanPrintUInt32(PrintFunc, "Terminate CAN transmit callback tasks fail = %d\n", 1, Retstatus, 0, 0, 0, 0);
    }
    Retstatus = AmbaKAL_TaskDelete(&IoDiag_CanTxCbTask);
    if (Retstatus != KAL_ERR_NONE) {
        IoDiag_CanPrintUInt32(PrintFunc, "Delete CAN transmit callback tasks fail = %d\n", 1, Retstatus, 0, 0, 0, 0);
    }

    Retstatus = AmbaCAN_Disable(IoDiag_CanCtrl.ChannelId);
    if (Retstatus != CAN_ERR_NONE) {
        IoDiag_CanPrintUInt32(PrintFunc, "Disable CANC fail = %d\n", 1, Retstatus, 0, 0, 0, 0);
    }
}


void AmbaIOUTDiag_CmdCAN(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount < 2U) {
        IoDiag_CanCmdUsage(ArgCount, pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "start", 5U) == 0) {
            IoDiag_CanEnable(ArgCount, pArgVector, PrintFunc);
            IoDiag_CanGetInfo(PrintFunc);
            IoDiag_CanRead(PrintFunc);
            IoDiag_CanTxCb(PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "write", 5U) == 0) {
            if (6U <= ArgCount) {
                IoDiag_CanWrite(ArgCount, pArgVector, PrintFunc);
            } else {
                IoDiag_CanCmdUsage(ArgCount, pArgVector, PrintFunc);
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "fdwrite", 7U) == 0) {
            if (5U <= ArgCount) {
                IoDiag_CanWriteFd(ArgCount, pArgVector, PrintFunc);
            } else {
                IoDiag_CanCmdUsage(ArgCount, pArgVector, PrintFunc);
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "stop", 4U) == 0) {
            IoDiag_CanDisable(PrintFunc);
        } else {
            IoDiag_CanCmdUsage(ArgCount, pArgVector, PrintFunc);
        }
    }
}
