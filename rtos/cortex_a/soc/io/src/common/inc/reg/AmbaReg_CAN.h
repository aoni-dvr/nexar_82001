/**
 *  @file AmbaReg_CAN.h
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
 *  @details Definitions & Constants for CV1 CAN Bus Controller Registers
 *
 */

#ifndef AMBA_REG_CAN_H
#define AMBA_REG_CAN_H

/*-----------------------------------------------------------------------------------------------*\
 * CAN: Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {

    UINT32 LoopbackMode:        2;  /* [1:0]: Loopback mode*/
    UINT32 ListenMode:          1;  /* [2]: Listen mode */
    UINT32 AutoRespMode:        1;  /* [3]: auto-response feature */
    UINT32 Reserved:            27; /* [30:4] */
    UINT32 EnableFD:            1;  /* [31]: 1= CAN 2.0 with flexible data rate */
} AMBA_CAN_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * CAN: Reset Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32 ResetAll:            1;  /* [0]: Software reset the CAN controller and DMA engine. */
    UINT32 ResetCanc:           1;  /* [1]: Software reset the CAN controller only */
    UINT32 ResetCancDma:        1;  /* [2]: Software reset the CAN DMA engine only */
    UINT32 Reserved:            29; /* [31:3] */
} AMBA_CAN_RESET_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * CAN FD SSP Registers
\*-----------------------------------------------------------------------------------------------*/

#define AMBA_CAN_FD_SSP_TDC_MASK            0x00000001U     /* [0]: Tx delay compensation enable*/
#define AMBA_CAN_FD_SSP_MODE_MASK           0x00000002U     /* [1]: SSP mode */
#define AMBA_CAN_FD_SSP_POS_MASK            0x0000FF00U     /* [15:8]: SSP position */
#define AMBA_CAN_FD_SSP_OFFSET_MASK         0x00FF0000U     /* [23:16]: SSP offset */

#define AMBA_CAN_FD_SSP_MODE_SHIFT          1U
#define AMBA_CAN_FD_SSP_POS_SHIFT           8U
#define AMBA_CAN_FD_SSP_OFFSET_SHIFT        16U

/*-----------------------------------------------------------------------------------------------*\
 * CAN Time Quanta Registers
\*-----------------------------------------------------------------------------------------------*/

#define AMBA_CAN_TQ_REG_TS2_MASK            0x0000000FU     /* [3:0]: */
#if defined(CONFIG_SOC_CV2)
#define AMBA_CAN_TQ_REG_TS1_MASK            0x000001F0U     /* [8:4]: */
#define AMBA_CAN_TQ_REG_SJW_MASK            0x00001E00U     /* [12:9]: Synchronization Jump Width */
#define AMBA_CAN_TQ_REG_BRP_MASK            0x001FE000U     /* [20:13]: Time quanta prescaler */
#else
#define AMBA_CAN_TQ_REG_TS1_MASK            0x000003F0U     /* [9:4]: */
#define AMBA_CAN_TQ_REG_SJW_MASK            0x00003C00U     /* [13:10]: Synchronization Jump Width */
#define AMBA_CAN_TQ_REG_BRP_MASK            0x003FC000U     /* [21:14]: Time quanta prescaler */
#endif
#define AMBA_CAN_TQ_REG_TS1_SHIFT            4U
#if defined(CONFIG_SOC_CV2)
#define AMBA_CAN_TQ_REG_SJW_SHIFT            9U
#define AMBA_CAN_TQ_REG_BRP_SHIFT            13U
#else
#define AMBA_CAN_TQ_REG_SJW_SHIFT            10U
#define AMBA_CAN_TQ_REG_BRP_SHIFT            14U
#endif

/*-----------------------------------------------------------------------------------------------*\
 * CAN: FD Time Quanta Config Register
\*-----------------------------------------------------------------------------------------------*/

#define AMBA_CAN_FDTQ_REG_TS2_MASK           0x0000000FU    /* [3:0]: */
#define AMBA_CAN_FDTQ_REG_TS1_MASK           0x000001F0U    /* [8:4]: */
#define AMBA_CAN_FDTQ_REG_SJW_MASK           0x00001E00U    /* [12:9]: Synchronization Jump Width */
#define AMBA_CAN_FDTQ_REG_BRP_MASK           0x001FE000U    /* [20:13]: Time quanta prescaler */

#define AMBA_CAN_FDTQ_REG_TS1_SHIFT          4U
#define AMBA_CAN_FDTQ_REG_SJW_SHIFT          9U
#define AMBA_CAN_FDTQ_REG_BRP_SHIFT          13U

/*-----------------------------------------------------------------------------------------------*\
 * CAN: Error Status Register
\*-----------------------------------------------------------------------------------------------*/

typedef struct {
    UINT32 TxErrorCount:        9;  /* [8:0]: Tx error counter (Max = 511) */
    UINT32 Reserved0:           7;  /* [15:9] */
    UINT32 RxErrorCount:        9;  /* [24:16]: Rx error counter */
    UINT32 ErrorState:          2;  /* [26:25]: 0: Idle State, 1: Error_active, 2: Error_passive, 3: Bus_off */
    UINT32 Reserved1:           5;  /* [31:27]: */
} AMBA_CAN_ERROR_STATUS_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * CAN: Accumulated Interrupt Control Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32 CountTh:             5;  /* [4:0]: The message buffer done number threshold */
    UINT32 Reserved0:           11; /* [15:5] */
    UINT32 Count:               6;  /* [21:16]: Accumulate message buffer done number */
    UINT32 Reserved1:           10; /* [31:22] */
} AMBA_CAN_ACC_INT_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * CAN: Message Buffer Done Control Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile AMBA_CAN_ACC_INT_CTRL_REG_s    AccIntCtrl;
    volatile UINT32                         IdleTime;
    volatile UINT32                         TimeOut;
} AMBA_CAN_INT_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * CAN: ID Filter Enable Register
\*-----------------------------------------------------------------------------------------------*/

#define CAN_IDF_REG_ID_FILTER_MASK            0x1FFFFFFFU      /* [28:0]: ID filter for Rx data will be received if the IDs match */

#define CAN_IDF_REG_ENABLE_FILTER_SHIFT       31U               /* [31]: Enable ID filter */

/*-----------------------------------------------------------------------------------------------*\
 * CAN: ID Filter Config Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile UINT32                        IdFilter;
    volatile UINT32                        IdFilterMask;
} AMBA_CAN_ID_FILTER_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * CAN: Debug Signal Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32 DebugRefErrorState:  2;  /* [1:0]: */
    UINT32 DebugRefRxState:     4;  /* [5:2]: */
    UINT32 DebugRefTxState:     4;  /* [9:6]: */
    UINT32 DebugRxS2pState:     3;  /* [12:10]: */
    UINT32 DebugTxP2sState:     3;  /* [15:13]: */
    UINT32 DebugRxAtResState:   2;  /* [17:16]: */
    UINT32 DebugRxState:        3;  /* [20:18]: */
    UINT32 DebugTxState:        4;  /* [24:21]: */
    UINT32 Reserved:            6;  /* [30:25]: */
    UINT32 EnableDebug:         1;  /* [31]: 1 = Enable Debug */
} AMBA_CAN_DEBUG_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * CAN: Debug Physical Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32 DebugBtCs:           2;  /* [1:0]: */
    UINT32 DebugFdCs:           2;  /* [3:2]: */
    UINT32 DebugPhyErrCs:       2;  /* [5:4]: */
    UINT32 DebugTxXmt:          1;  /* [6]: */
    UINT32 DebugCanTx:          1;  /* [7]: */
    UINT32 DebugHardSyncFlag:   1;  /* [8]: */
    UINT32 DebugHardSyncIntvl:  1;  /* [9]: */
    UINT32 DebugResyncEdge:     1;  /* [10]: */
    UINT32 DebugBusOff:         1;  /* [11]: */
    UINT32 DebugSamplePtLast:   1;  /* [12]: */
    UINT32 DebugSumRxd:         2;  /* [14:13]: */
    UINT32 DebugCanRxd:         1;  /* [15]: */
    UINT32 Reserved:            16; /* [31:16]: */
} AMBA_CAN_DEBUG_PHYSICAL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * CAN: Message Buffer ID Register
\*-----------------------------------------------------------------------------------------------*/

#define CAN_MSG_BUF_ID_REG_MSGID_MASK         0x1FFFFFFFU      /* [28:0]: Message identification field */
#define CAN_MSG_BUF_ID_REG_MSGEXT_MASK        0x20000000U      /* [29]: Used to distinguish between 11-bit ID format and 29-bit ID format */

#define CAN_MSG_BUF_ID_REG_MSGEXT_SHIFT       29U              /* [29]: Used to distinguish between 11-bit ID format and 29-bit ID format */

/*-----------------------------------------------------------------------------------------------*\
 * CAN: Message Buffer Control Register
\*-----------------------------------------------------------------------------------------------*/

#define CAN_MBC_REG_DLC_MASK              0x0000000FU      /* [3:0]: Data length code */
#define CAN_MBC_REG_ESI_MASK              0x00000010U      /* [4]: Error state (Should be filled by hardware)*/
#define CAN_MBC_REG_BRS_MASK              0x00000020U      /* [5]: Bit-rate-switch */
#define CAN_MBC_REG_EDL_MASK              0x00000040U      /* [6]: Extended data length */
#define CAN_MBC_REG_RTR_MASK              0x00000080U      /* [7]: Remote transmission request (Only used in CAN 2.0) */

#define CAN_MBC_REG_ESI_SHIFT              4U      /* [4]: Error state (Should be filled by hardware)*/
#define CAN_MBC_REG_BRS_SHIFT              5U      /* [5]: Bit-rate-switch */
#define CAN_MBC_REG_EDL_SHIFT              6U      /* [6]: Extended data length */
#define CAN_MBC_REG_RTR_SHIFT              7U      /* [7]: Remote transmission request (Only used in CAN 2.0) */

/*-----------------------------------------------------------------------------------------------*\
 * CAN: Message Buffer Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile UINT32                         MsgBufId;
    volatile UINT32                         MsgBufCtrl;
    volatile UINT32                         MsgBufTimeTrig;
    volatile UINT32                         MsgBufTimeoutTh;
} AMBA_CAN_MSG_BUF_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * CAN: DMA Descriptor Control Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32 CountTh:             8;  /* [7:0]: Receive descriptor length threshold */
    UINT32 Reserved0:           8;  /* [15:8] */
    UINT32 Count:               9;  /* [24:16]: Number of received messages */
    UINT32 Reserved1:           6;  /* [30:25] */
    UINT32 EnableTimeOut:       1;  /* [31]: Enable the timeout funhction in CAN DMA description mode */
} AMBA_CAN_DMA_DESC_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * CAN: DMA SPECIAL ID Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32 SpecialId:           5;  /* [4:0]: The ID filter index for special ID rule */
    UINT32 Reserved0:           10; /* [14:5] */
    UINT32 EnableSpecialId:     1;  /* [15]: Enable the special ID function */
    UINT32 EnableRtrStop:       1;  /* [16]: Stop DMA transfer when getting a RTR message */
    UINT32 Reserved1:           15; /* [31:17] */
} AMBA_CAN_DMA_SPECIAL_ID_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * CAN: All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile AMBA_CAN_CTRL_REG_s                Ctrl;                   /* 0x000(RW): CAN control register */
    volatile UINT32                             EnableTtCan;            /* 0x004(RW): Time trigger control register (1-bit) */
    volatile AMBA_CAN_RESET_REG_s               Reset;                  /* 0x008(RW): Software reset */
    volatile UINT32                             FdSsp;                  /* 0x00C(RW): SSP register for CAN FD */
    volatile UINT32                             Tq;                     /* 0x010(RW): Time Quanta for CAN 2.0 */
    volatile UINT32                             TqFD;                   /* 0x014(RW): Time Quanta for CAN FD */
    volatile UINT32                             TtTimer;                /* 0x018(RW): Timer for TT-CAN */
    volatile UINT32                             EnableTtTimer;          /* 0x01C(RW): Run/Pause the timer value for TT-CAN (1-bit) */
    volatile AMBA_CAN_ERROR_STATUS_REG_s        ErrorStatus;            /* 0x020(RO): Error status register */
    volatile UINT32                             RxOverflowId;           /* 0x024(RO): The 1st dropped message ID when Rx overflow (29-bits) */
    volatile UINT32                             RxCount;                /* 0x028(RW): RxCount will increase when controller receive valid message (9-bits) */
    volatile UINT32                             IntStatus;              /* 0x02C(RO): Interrupt status register (IntStatus = IntRaw & IntMask) */
    volatile UINT32                             IntRaw;                 /* 0x030(WC): Raw interrupt register (set to 1 to clear interrupt) */
    volatile UINT32                             IntMask;                /* 0x034(RW): Interrupt mask register */
    volatile UINT32                             EnableCanc;             /* 0x038(RW): CAN controller (1-bit) */
    volatile UINT32                             TxBusyStatus;           /* 0x03C(RO): The busy status of Tx message buffers */
    volatile UINT32                             RxBusyStatus;           /* 0x040(RO): The busy status of Rx message buffers */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)// Support for auto answering mode
    volatile UINT32                             AutoAnswerFlag;         /* 0x044(RW): Auto answer flag */
    volatile UINT32                             AutoTxDone;             /* 0x048(RO): Auto transmit is done */
    volatile UINT32                             AutoRtrMatch;           /* 0x04C(RO): RTR's ID and DLC match Tx message buffers */
    volatile UINT32                             Reserved0[12];          /* 0x050-0x07C: */
#else
    volatile UINT32                             Reserved0[15];          /* 0x044-0x07C: */
#endif
    volatile UINT32                             MsgBufCfgDone;          /* 0x080(RWSC): Config message buffer done */
    volatile UINT32                             MsgBufRequest;          /* 0x084(RWSC): Request to configure message buffer */
    volatile UINT32                             MsgBufGrant;            /* 0x088(RO): Grant to configure message buffer */
    volatile UINT32                             TxTransferDone;         /* 0x08C(RO): The transfer of Tx message buffer is done */
    volatile UINT32                             RxTransferDone;         /* 0x090(RO): The transfer of Rx message buffer is done */
    volatile UINT32                             MsgBufRetryFail;        /* 0x094(RO): Message retry failure status */
    volatile UINT32                             MsgBufTimeout;          /* 0x098(RO): Message buffer time out */
    volatile UINT32                             MsgBufType;             /* 0x09C(RW): Message buffer type */
    volatile UINT32                             Reserved1[24];          /* 0x0A0-0x0FC */
    volatile AMBA_CAN_ID_FILTER_REG_s           IdFilter[32];           /* 0x100-0x1FC(RW): ID filter configurations */
    volatile AMBA_CAN_MSG_BUF_REG_s             MsgCtrl[32];            /* 0x200-0x3FC(RW): Message buffer control */
    volatile UINT32                             MsgPriority[8];         /* 0x400-0x41C(RW): Config message buffer priority */
    volatile AMBA_CAN_INT_CTRL_REG_s            TxIntCtrl;              /* 0x420-0x428(RW): Tx message buffer done configurations */
    volatile AMBA_CAN_INT_CTRL_REG_s            RxIntCtrl;              /* 0x42C-0x434(RW): Rx message buffer done configurations */
    volatile AMBA_CAN_DEBUG_REG_s               DebugStatus;            /* 0x438(RO): Debug registers */
    volatile AMBA_CAN_DEBUG_PHYSICAL_REG_s      DebugPhysicalStatus;    /* 0x43C(RO): Debug physical registers */
    volatile UINT32                             Reserved2[48];          /* 0x440-0x4FC */
    volatile UINT32                             EnableRxDma;            /* 0x500(RW): CANC Rx DMA mode is enabled */
    volatile UINT32                             RxDmaDescPtr;           /* 0x504(RW): CANC RX DMA descriptor pointer */
    volatile AMBA_CAN_DMA_DESC_CTRL_REG_s       RxDmaDescCtrl;          /* 0x508(RW): CANC RX DMA descriptor mode configurations */
    volatile UINT32                             RxDmaIdleTime;          /* 0x50C(RW): The idle time of geting CAN messages in DMA mode */
    volatile UINT32                             RxDmaTimeOut;           /* 0x510(RW): The time-out value of geting CAN messages in DMA mode */
    volatile AMBA_CAN_DMA_SPECIAL_ID_CTRL_REG_s RxDmaSpecialIdCtrl;     /* 0x514(RW): THe special ID configurations in DMA mode */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    volatile UINT32                             CancFaultInj;           /* 0x518(RW): SM error detection logic */
    volatile UINT32                             Reserved3[185];         /* 0x51C-0x7FC(RW): */
#else
    volatile UINT32                             Reserved3[186];         /* 0x518-0x7FC(RW): */
#endif
    volatile UINT32                             MsgData[32][16];        /* 0x800-0xBFC(RW): Message buffer data field */
} AMBA_CAN_REG_s ;

#endif /* AMBA_REG_CAN_H */

