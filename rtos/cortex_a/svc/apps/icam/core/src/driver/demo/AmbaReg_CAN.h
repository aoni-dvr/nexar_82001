/**
 *  @file AmbaReg_CAN.h
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
 *  @details Definitions & Constants for CV1 CAN Bus Controller Registers
 *
 */

#ifndef AMBA_REG_CAN_H
#define AMBA_REG_CAN_H

#include "AmbaTypes.h"

/*-----------------------------------------------------------------------------------------------*\
 * CAN: Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {

    UINT32 LoopbackMode:        2;  /* [1:0]: Loopback mode*/
    UINT32 ListenMode:          1;  /* [2]: Listen mode */
    UINT32 AutoRespMode:        1;  /* [3]: auto-response feature */
    UINT32 NonIsoCanFdMode:     1;  /* [4]: ISO CAN FD mode(new) = 0; non-ISO CAN FD mode(old) = 1*/
    UINT32 Reserved:            26; /* [30:5] */
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
 * A9AQ CAN Time Quanta Registers
\*-----------------------------------------------------------------------------------------------*/

#if defined(CONFIG_SOC_CV25)
#define AMBA_CAN_TQ_REG_TS2_MASK            0x0000000FU     /* [3:0]: */
#define AMBA_CAN_TQ_REG_TS1_MASK            0x000003F0U     /* [9:4]: */
#define AMBA_CAN_TQ_REG_SJW_MASK            0x00003C00U     /* [13:10]: Synchronization Jump Width */
#define AMBA_CAN_TQ_REG_BRP_MASK            0x003FC000U     /* [21:14]: Time quanta prescaler */
#define AMBA_CAN_TQ_REG_SYNC_MODE_MASK      0x00400000U     /* [22]: Both edges or falling edge are used for synchronization */
#define AMBA_CAN_TQ_REG_SAMPLE_DLY_MASK     0x7F800000U     /* [30:23]: Delay the sample point in the unit of ref_clk cycles */
#define AMBA_CAN_TQ_REG_SAMPLE_TIMES_MASK   0x80000000U     /* [31]: sample times */

#define AMBA_CAN_TQ_REG_TS2_SHIFT            0U
#define AMBA_CAN_TQ_REG_TS1_SHIFT            4U
#define AMBA_CAN_TQ_REG_SJW_SHIFT            10U
#define AMBA_CAN_TQ_REG_BRP_SHIFT            14U
#define AMBA_CAN_TQ_REG_SYNC_MODE_SHIFT      22U
#define AMBA_CAN_TQ_REG_SAMPLE_DLY_SHIFT     23U
#define AMBA_CAN_TQ_REG_SAMPLE_TIMES_SHIFT   31U
#else
#define AMBA_CAN_TQ_REG_TS2_MASK            0x0000000FU     /* [3:0]: */
#define AMBA_CAN_TQ_REG_TS1_MASK            0x000001F0U     /* [8:4]: */
#define AMBA_CAN_TQ_REG_SJW_MASK            0x00001E00U     /* [12:9]: Synchronization Jump Width */
#define AMBA_CAN_TQ_REG_BRP_MASK            0x001FE000U     /* [20:13]: Time quanta prescaler */
#define AMBA_CAN_TQ_REG_SYNC_MODE_MASK      0x00200000U     /* [21]: Both edges or falling edge are used for synchronization */
#define AMBA_CAN_TQ_REG_SAMPLE_DLY_MASK     0x3FC00000U     /* [29:22]: Delay the sample point in the unit of ref_clk cycles */
#define AMBA_CAN_TQ_REG_SAMPLE_TIMES_MASK   0x40000000U     /* [30]: sample times */
                                                            /* [31] */
#define AMBA_CAN_TQ_REG_TS2_SHIFT            0U
#define AMBA_CAN_TQ_REG_TS1_SHIFT            4U
#define AMBA_CAN_TQ_REG_SJW_SHIFT            9U
#define AMBA_CAN_TQ_REG_BRP_SHIFT            13U
#define AMBA_CAN_TQ_REG_SYNC_MODE_SHIFT      21U
#define AMBA_CAN_TQ_REG_SAMPLE_DLY_SHIFT     22U
#define AMBA_CAN_TQ_REG_SAMPLE_TIMES_SHIFT   30U
#endif

/*-----------------------------------------------------------------------------------------------*\
 * CAN: FD Time Quanta Config Register
\*-----------------------------------------------------------------------------------------------*/

#define AMBA_CAN_FDTQ_REG_TS2_MASK           0x0000000FU    /* [3:0]: */
#define AMBA_CAN_FDTQ_REG_TS1_MASK           0x000001F0U    /* [8:4]: */
#define AMBA_CAN_FDTQ_REG_SJW_MASK           0x00001E00U    /* [12:9]: Synchronization Jump Width */
#define AMBA_CAN_FDTQ_REG_BRP_MASK           0x001FE000U    /* [20:13]: Time quanta prescaler */

#define AMBA_CAN_FDTQ_REG_TS2_SHIFT          0U
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
 * CAN: Interrupt Status Register
\*-----------------------------------------------------------------------------------------------*/

#define AMBA_CAN_INT_REG_BUF_OFF_SHIFT        0U  /* [0]: Entering bus off state */
#define AMBA_CAN_INT_REG_ERR_PASSIVE_SHIFT    1U  /* [1]: Entering error passive mode */
#define AMBA_CAN_INT_REG_ACK_ERR_SHIFT        2U  /* [2]: */
#define AMBA_CAN_INT_REG_FORMAT_ERR_SHIFT     3U  /* [3]: */
#define AMBA_CAN_INT_REG_CRC_ERR_SHIFT        4U  /* [4]: */
#define AMBA_CAN_INT_REG_STUFF_ERR_SHIFT      5U  /* [5]: */
#define AMBA_CAN_INT_REG_BIT_ERR_SHIFT        6U  /* [6]: */
#define AMBA_CAN_INT_REG_TIMEOUT_SHIFT        7U  /* [7]: Any of the message buffers is in a timeout state in Rx or Tx mode */
#define AMBA_CAN_INT_REG_RX_BUF_OV            8U  /* [8]: Any of the Rx message buffers have overflowed */
#define AMBA_CAN_INT_REG_TX_DONE              9U  /* [9]: CAN bus packet has been transmitted */
#define AMBA_CAN_INT_REG_RX_DONE              10U /* [10]: CAN bus packet has been received */
#define AMBA_CAN_INT_REG_TIME_WRAP            11U /* [11]: TT timer wrap interrupt */
#define AMBA_CAN_INT_REG_WKUP                 12U /* [12]: Wake up interrupt (entering active state from bus off state) */
#define AMBA_CAN_INT_REG_RETRY_FAIL           13U /* [13]: Replay failed interrupt */
#define AMBA_CAN_INT_REG_RX_DONE_TIMEOUT      14U /* [14]: RxDone does not increase for a specified wating time */
#define AMBA_CAN_INT_REG_TX_DONE_TIMEOUT      15U /* [15]: TxDone does not increase for a specified wating time */
#define AMBA_CAN_INT_REG_RX_DMA_DESC_DONE     16U /* [16]: After one RX descriptor has been processed by DMA engine, the bit assert */
#define AMBA_CAN_INT_REG_RX_DMA_DONE          17U /* [17]: The RX DMA chain end */
#define AMBA_CAN_INT_REG_RX_DMA_TIMEOUT       18U /* [18]: When total number of descriptor have benn processed */
#define AMBA_CAN_INT_REG_RX_DMA_SPECIAL_ID    19U /* [19]: When Rx DMA receive special ID */
#define AMBA_CAN_INT_REG_RX_DMA_GET_RTR       20U /* [20]: When Rx DMA write a RTR message, write to the last position of the correspond descriptor */
#define AMBA_CAN_INT_REG_FD_STUFF_ERR         21U /* [21]: Stuff count error of CAN FD message */

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
#define CAN_IDF_REG_ENABLE_FILTER_MASK        0x80000000U      /* [31]: Enable ID filter */

#define CAN_IDF_REG_ID_FILTER_SHIFT           0U                /* [28:0]: ID filter for Rx data will be received if the IDs match */
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
    UINT32 Reserved:            16; /* [30:16]: */
} AMBA_CAN_DEBUG_PHYSICAL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * CAN: Message Buffer ID Register
\*-----------------------------------------------------------------------------------------------*/

#define CAN_MSG_BUF_ID_REG_MSGID_MASK         0x1FFFFFFFU      /* [28:0]: Message identification field */
#define CAN_MSG_BUF_ID_REG_MSGEXT_MASK        0x20000000U      /* [29]: Used to distinguish between 11-bit ID format and 29-bit ID format */

#define CAN_MSG_BUF_ID_REG_MSGID_SHIFT        0U               /* [28:0]: Message identification field */
#define CAN_MSG_BUF_ID_REG_MSGEXT_SHIFT       29U              /* [29]: Used to distinguish between 11-bit ID format and 29-bit ID format */

/*-----------------------------------------------------------------------------------------------*\
 * CAN: Message Buffer Control Register
\*-----------------------------------------------------------------------------------------------*/

#define CAN_MBC_REG_DLC_MASK              0x0000000FU      /* [3:0]: Data length code */
#define CAN_MBC_REG_ESI_MASK              0x00000010U      /* [4]: Error state (Should be filled by hardware)*/
#define CAN_MBC_REG_BRS_MASK              0x00000020U      /* [5]: Bit-rate-switch */
#define CAN_MBC_REG_EDL_MASK              0x00000040U      /* [6]: Extended data length */
#define CAN_MBC_REG_RTR_MASK              0x00000080U      /* [7]: Remote transmission request (Only used in CAN 2.0) */
#define CAN_MBC_REG_TX_EN_MASK            0x001E0000U      /* [20:17]: For TT-CAN only, data in Tx buffer will be transmitted in tx_enable+1 bit-time window */
#define CAN_MBC_REG_Tx_RETRY_NUM_EN_MASK  0x00800000U      /* [23]: Limit Tx re-transmit times up to tx_retry_num */
#define CAN_MBC_REG_TX_RETRY_NUM_MASK     0xFF000000U      /* [31:24]: x-times Tx retransmission */

#define CAN_MBC_REG_DLC_SHIFT              0U      /* [3:0]: Data length code */
#define CAN_MBC_REG_ESI_SHIFT              4U      /* [4]: Error state (Should be filled by hardware)*/
#define CAN_MBC_REG_BRS_SHIFT              5U      /* [5]: Bit-rate-switch */
#define CAN_MBC_REG_EDL_SHIFT              6U      /* [6]: Extended data length */
#define CAN_MBC_REG_RTR_SHIFT              7U      /* [7]: Remote transmission request (Only used in CAN 2.0) */
#define CAN_MBC_REG_TX_EN_SHIFT            17U      /* [20:17]: For TT-CAN only, data in Tx buffer will be transmitted in tx_enable+1 bit-time window */
#define CAN_MBC_REG_Tx_RETRY_NUM_EN_SHIFT  23U      /* [23]: Limit Tx re-transmit times up to tx_retry_num */
#define CAN_MBC_REG_TX_RETRY_NUM_SHIFT     24U      /* [31:24]: x-times Tx retransmission */

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
 * CAN: Message Priority Register
\*-----------------------------------------------------------------------------------------------*/

#define CAN_MSG_PRI_REG_GP3_PRIORITY_MASK          0x0000001FU     /* [4:0]: Msg 3/7/11/15 Priority */
#define CAN_MSG_PRI_REG_GP2_PRIORITY_MASK          0x00001F00U     /* [12:8]: Msg 2/6/10/14 Priority */
#define CAN_MSG_PRI_REG_GP1_PRIORITY_MASK          0x001F0000U     /* [20:16]: Msg 1/5/9/13 Priority */
#define CAN_MSG_PRI_REG_GP0_PRIORITY_MASK          0x1F000000U     /* [28:24]: Msg 0/4/8/12 Priority */

#define CAN_MSG_PRI_REG_GP3_PRIORITY_SHIFT         0x0000001FU
#define CAN_MSG_PRI_REG_GP2_PRIORITY_SHIFT         0x00001F00U
#define CAN_MSG_PRI_REG_GP1_PRIORITY_SHIFT         0x001F0000U
#define CAN_MSG_PRI_REG_GP0_PRIORITY_SHIFT         0x1F000000U

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
    volatile UINT32                             WakeUp;                 /* 0x00C(RW): Wake up register (1-bit) */
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
    volatile UINT32                             Reserved0[15];          /* 0x044-0x07C: */
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
    volatile UINT32                             Reserved3[186];         /* 0x518-0x7FC(RW): */
    volatile UINT32                             MsgData[32][16];        /* 0x800-0xBFC(RW): Message buffer data field */
} AMBA_CAN_REG_s ;

#endif /* AMBA_REG_CAN_H */

