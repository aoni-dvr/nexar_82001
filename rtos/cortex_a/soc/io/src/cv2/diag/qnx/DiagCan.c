/*
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

#include <stdio.h>
#include <string.h>
#include "diag.h"
#include "AmbaTypes.h"
#include "AmbaCAN.h"
#include "AmbaKAL.h"
#include "AmbaGPIO.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"


static const UINT8 IoDiag_CanDlcTable[AMBA_NUM_CAN_DATA_LENGTH] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64
};

typedef struct {
    UINT32              ChannelId;
    AMBA_CAN_CONFIG_s   Config;
} AMBA_SHELL_CAN_CTRL_s;


/* Common CANC and CAN 2.0 configurations */
static AMBA_SHELL_CAN_CTRL_s IoDiag_CanCtrl = {
    .ChannelId = AMBA_CAN_CHANNEL0,
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

        .FdTimeQuanta       = {
            .PhaseSeg2      = 1,                     /* E.g., To enable 1M bps transfer, it might set PropSeg = 0, Seg2 = 1, Seg1 = 2, SJW = 0, and  */
            .PhaseSeg1      = 2,                     /*       BRP = 2. Then, the bit-rate is 24M/((1 + 2 + 3) * 2) = 2M bps.      */
            .SJW            = 0,
            .BRP            = 1,
            .PropSeg        = 0,
        },

        .NumIdFilter        = 0,                      /* Number of ID filters */
        .pIdFilter          = NULL,                   /* Pointer to the ID filters */

    },
};


static void IoDiag_CanCmdUsage(char * const *pArgVector)
{
    printf("Usage: ");
    printf("       ");
    printf("%s\n", pArgVector[0]);
    printf(" can start [ch]: Enable CANC and create receiving tasks\n");
    printf("   [ch]        : controller channel ID\n");
    printf(" can write [ch] [id] [extention] [rtr] [length] [data] : Write a CAN message\n");
    printf("   [ch]        : controller channel ID\n");
    printf("   [id]        : message ID\n");
    printf("   [extention] : set to 1 to enable extented ID\n");
    printf("   [rtr]       : set to 1 to make a RTR message\n");
    printf("   [length]    : the data length\n");
    printf("   [data]      : the message data\n");
    printf(" can fdwrite [ch] [id] [extention] [length] [data] : Write a CAN FD message\n");
    printf("   [ch]        : controller channel ID\n");
    printf("   [id]        : message ID\n");
    printf("   [extention] : set to 1 to enable extented ID\n");
    printf("   [length]    : the data length\n");
    printf("   [data]      : the message data\n");
    printf(" can stop [ch]: Disable CANC and delete receiving tasks\n");
    printf("   [ch]        : controller channel ID\n");

}

static void IoDiag_CanEnable(UINT32 ArgCount, char * const * pArgVector)
{
    UINT32 Retstatus;

    // determine channel ID
    if (ArgCount == 3U) {
        IoDiag_CanCtrl.ChannelId = AMBA_CAN_CHANNEL0;
    } else {
        UINT32 ch;
        (void) AmbaUtility_StringToUInt32(pArgVector[3], &ch);
        IoDiag_CanCtrl.ChannelId = ch;
    }

    // setup tx/rx path
    if (IoDiag_CanCtrl.ChannelId == AMBA_CAN_CHANNEL0) {
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_158_CAN0_RX);
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_160_CAN0_TX);
    } else if (IoDiag_CanCtrl.ChannelId == AMBA_CAN_CHANNEL1) {
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_159_CAN1_RX);
        (void) AmbaGPIO_SetFuncAlt(GPIO_PIN_161_CAN1_TX);
    } else {
        printf(" Unknown Channel ID = 0x%x \n", IoDiag_CanCtrl.ChannelId);
    }

    Retstatus = AmbaCAN_Enable(IoDiag_CanCtrl.ChannelId, &IoDiag_CanCtrl.Config);
    if (Retstatus != CAN_ERR_NONE) {
        printf("Failed to start CAN = 0x%x \n", Retstatus);
    }
}

static void IoDiag_CanGetInfo(void)
{
    AMBA_CAN_BIT_INFO_s BitInfo;
    AMBA_CAN_FILTER_s FilterInfo[32];
    UINT32 FilterLen;
    UINT32 Ret;

    Ret = AmbaCAN_GetInfo(IoDiag_CanCtrl.ChannelId, &BitInfo, &FilterLen, FilterInfo);
    if (Ret == CAN_ERR_NONE) {
        UINT32 i;
        printf("Ch[%d], System Clock = %d, NominalTQ = %d (ns), DataTQ = %d (ns), NominalBitLen = %d (tq)\n",
               IoDiag_CanCtrl.ChannelId,
               BitInfo.SysClkFreq,
               BitInfo.NominalTQ,
               BitInfo.DataTQ,
               BitInfo.NominalBitLen);

        printf("DataBitLen = %d (tq), NominalBitRate = %d (bps), DataBitRate = %d (bps)\n",
               BitInfo.DataBitLen,
               BitInfo.NominalBitRate,
               BitInfo.DataBitRate);

        for (i=0; i<FilterLen; i++) {
            printf("[%d] IdFilter = 0x%x, IdMask = 0x%x\n",
                   i,
                   FilterInfo[i].IdFilter,
                   FilterInfo[i].IdMask);
        }
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


static void IoDiag_CanWrite(UINT32 ArgCount, char * const * pArgVector)
{
    const AMBA_CAN_CONFIG_s *CanConfig = &IoDiag_CanCtrl.Config;
    AMBA_CAN_MSG_s CanMessage;
    UINT32 i;
    UINT32 RetStatus;
    UINT32 TmpValue;
    UINT32 ch;

    (void) ArgCount;

    (void) AmbaUtility_StringToUInt32(pArgVector[3], &ch);
    IoDiag_CanCtrl.ChannelId = ch;

    if (CanConfig->OpMode == AMBA_CAN_OP_MODE_LISTEN) {
        printf("In listen mode, no message is sent.\n");
    } else {
        (void) AmbaUtility_StringToUInt32(pArgVector[4], &TmpValue);
        CanMessage.Id          = TmpValue;
        (void) AmbaUtility_StringToUInt32(pArgVector[5], &TmpValue);
        CanMessage.Extension   = (UINT8)(TmpValue & 0xFFU);
        (void) AmbaUtility_StringToUInt32(pArgVector[6], &TmpValue);
        CanMessage.RemoteTxReq = (UINT8)(TmpValue & 0xFFU);
        CanMessage.Priority    = 0;
        (void) AmbaUtility_StringToUInt32(pArgVector[7], &TmpValue);
        // The maximum data length of classical CAN is 8 bytes.
        if (TmpValue > 8U) {
            TmpValue = 8;
        }
        (void) IoDiag_CanCheckLEN(TmpValue, &CanMessage.DataLengthCode);
        for (i = 0; i < IoDiag_CanDlcTable[CanMessage.DataLengthCode]; i++) {
            (void) AmbaUtility_StringToUInt32(pArgVector[8U+i], &TmpValue);
            CanMessage.Data[i] = (UINT8)(TmpValue & 0xFFU);
        }
        CanMessage.TrackID    = CanMessage.Id; // user defined
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        CanMessage.AutoAnswer = 0; // not set auto answer flag
#endif

        RetStatus = AmbaCAN_Write(IoDiag_CanCtrl.ChannelId, &CanMessage, 100);
        if (RetStatus != CAN_ERR_NONE) {
            printf("Write failure, status = 0x%x\n\r", RetStatus);
        }
    }
}

static void IoDiag_CanWriteFd(UINT32 ArgCount, char * const * pArgVector)
{
    const AMBA_CAN_CONFIG_s *CanConfig = &IoDiag_CanCtrl.Config;
    AMBA_CAN_FD_MSG_s CanMessage;
    UINT32 i;
    UINT32 RetStatus;
    UINT32 TmpValue;
    UINT32 ch;

    (void) ArgCount;

    (void) AmbaUtility_StringToUInt32(pArgVector[3], &ch);
    IoDiag_CanCtrl.ChannelId = ch;

    if (CanConfig->OpMode == AMBA_CAN_OP_MODE_LISTEN) {
        printf("In listen mode, no message is sent.\n");
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    } else if (IoDiag_CanCtrl.ChannelId == AMBA_CAN_CHANNEL0) {
        printf("CAN0 does not support CAN FD Tx.\n");
#endif
    } else {
        (void) AmbaUtility_StringToUInt32(pArgVector[4], &TmpValue);
        CanMessage.Id          = TmpValue;
        (void) AmbaUtility_StringToUInt32(pArgVector[5], &TmpValue);
        CanMessage.Extension   = (UINT8)(TmpValue & 0xFFU);
        CanMessage.Priority    = 0;
        (void) AmbaUtility_StringToUInt32(pArgVector[6], &TmpValue);
        // The maximum data length of CAN FD is 64 bytes.
        if (TmpValue > 64U) {
            TmpValue = 64;
        }
        (void) IoDiag_CanCheckLEN(TmpValue, &CanMessage.DataLengthCode);
        CanMessage.FlexibleDataRate = 1;
        CanMessage.BitRateSwitch = 1;
        for (i = 0; i < IoDiag_CanDlcTable[CanMessage.DataLengthCode]; i++) {
            (void) AmbaUtility_StringToUInt32(pArgVector[7U+i], &TmpValue);
            CanMessage.Data[i] = (UINT8)(TmpValue & 0xFFU);
        }
        CanMessage.TrackID    = CanMessage.Id; // user defined
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        CanMessage.AutoAnswer = 0; // not set auto answer flag
#endif

        RetStatus = AmbaCAN_WriteFd(IoDiag_CanCtrl.ChannelId, &CanMessage, 1000);
        if (RetStatus != CAN_ERR_NONE) {
            printf("Write FD failure, status = 0x%x\n\r", RetStatus);
        }
    }

}

static void IoDiag_CanDisable(UINT32 ArgCount, char * const * pArgVector)
{
    UINT32 Retstatus;

    // determine channel ID
    if (ArgCount == 3U) {
        IoDiag_CanCtrl.ChannelId = AMBA_CAN_CHANNEL0;
    } else {
        UINT32 ch;
        (void) AmbaUtility_StringToUInt32(pArgVector[3], &ch);
        IoDiag_CanCtrl.ChannelId = ch;
    }

    Retstatus = AmbaCAN_Disable(IoDiag_CanCtrl.ChannelId);
    if (Retstatus != CAN_ERR_NONE) {
        printf("Disable CANC fail = %d\n", Retstatus);
    }
}

void *
funcThreadA( void )
{
    AMBA_CAN_MSG_s CanMessage;
    UINT32 RetStatus;
    UINT32 i;
    static UINT32 IoDiag_CanMsgCounter = 0;

    while(1) {
        /* Get one message from Rx Ring Buffer */
        RetStatus = AmbaCAN_Read(IoDiag_CanCtrl.ChannelId, &CanMessage,AMBA_KAL_WAIT_FOREVER);
        if (RetStatus == CAN_ERR_NONE) {
            /* Show the CAN messgae */
            printf("Count [%d]",IoDiag_CanMsgCounter);
            printf("CH: %d, ID: 0x%x, EXT: %d, RTR: %d, DLC: %d, Data: ",
                   IoDiag_CanCtrl.ChannelId,
                   CanMessage.Id,
                   CanMessage.Extension,
                   CanMessage.RemoteTxReq,
                   CanMessage.DataLengthCode);

            for (i = 0; i < IoDiag_CanDlcTable[CanMessage.DataLengthCode]; i++) {
                printf("%x ", CanMessage.Data[i]);
            }
            printf("\n");
            IoDiag_CanMsgCounter++;
        } else {
            printf("Read fail (%d)\n", RetStatus);
            break;
        }
    }
    return;
}

void *
funcThreadB( void )
{
    AMBA_CAN_FD_MSG_s CanMessage;
    UINT32 RetStatus;
    UINT32 i;
    static UINT32 IoDiag_CanFdMsgCounter = 0;

    while(1) {
        /* Get one message from Rx Ring Buffer */
        RetStatus = AmbaCAN_ReadFd(IoDiag_CanCtrl.ChannelId, &CanMessage, AMBA_KAL_WAIT_FOREVER);
        if (RetStatus == CAN_ERR_NONE) {
            /* Show the CAN FD messgae */
            printf("FD Count [%d] CH: %d, ID: 0x%x", IoDiag_CanFdMsgCounter, IoDiag_CanCtrl.ChannelId, CanMessage.Id);

            printf("EXT: %d, ESI: %d, BRS: %d, DLC: %d, Data: ",
                   CanMessage.Extension,
                   CanMessage.ErrorStateIndicator,
                   CanMessage.BitRateSwitch,
                   IoDiag_CanDlcTable[CanMessage.DataLengthCode]);


            for (i = 0; i < IoDiag_CanDlcTable[CanMessage.DataLengthCode]; i++) {
                printf("%x ", CanMessage.Data[i]);
            }
            printf("\n");

            IoDiag_CanFdMsgCounter++;
        } else {
            printf("Read fail (%d)\n", RetStatus);
            break;
        }
    }
    return;
}

static void IoDiag_CanRead(void)
{
    pthread_t threadA;
    pthread_t threadB;

    pthread_create( &threadA, NULL, &funcThreadA, NULL );
    pthread_create( &threadB, NULL, &funcThreadB, NULL );

    pthread_join( threadA, NULL );
    pthread_join( threadB, NULL );
}

int DoCanDiag(int argc, char *argv[])
{
    UINT32 Retstatus;
    AMBA_CAN_BIT_INFO_s BitInfo;
    UINT32 NumFilter;
    AMBA_CAN_FILTER_s Filter[32];
    UINT32 ch;

    if (argc >= 3) {
        if (strcmp("start", argv[2]) == 0) {
            IoDiag_CanEnable(argc, argv);
            IoDiag_CanGetInfo();
            IoDiag_CanRead();
        } else if (strcmp("write", argv[2]) == 0) {
            if (9U <= argc) {
                IoDiag_CanWrite(argc, argv);
            } else {
                IoDiag_CanCmdUsage(argv);
            }
        } else if (strcmp("fdwrite", argv[2]) == 0) {
            if (8U <= argc) {
                IoDiag_CanWriteFd(argc, argv);
            } else {
                IoDiag_CanCmdUsage(argv);
            }
        } else if (strcmp("stop", argv[2]) == 0) {
            IoDiag_CanDisable(argc, argv);
        } else {
            IoDiag_CanCmdUsage(argv);
        }
    } else {
        IoDiag_CanCmdUsage(argv);
    }
    return 0;
}
