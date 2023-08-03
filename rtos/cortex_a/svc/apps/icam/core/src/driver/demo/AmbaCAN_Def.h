/**
 *  @file AmbaCAN_Def.h
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
 *  @details Definitions & Constants for CAN Control
 *
 */

#ifndef AMBA_CAN_DEF_H
#define AMBA_CAN_DEF_H

#include "AmbaTypes.h"
#include "AmbaErrorCode.h"

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of CAN Error Code
\*-----------------------------------------------------------------------------------------------*/

#define CAN_ERR_0000        (CAN_ERR_BASE)              /* Invalid argument */
#define CAN_ERR_0001        (CAN_ERR_BASE + 0X1U)       /* Unable to do concurrency protection */
#define CAN_ERR_0002        (CAN_ERR_BASE + 0X2U)       /* Controller is busy */
#define CAN_ERR_0003        (CAN_ERR_BASE + 0X3U)       /* A timeout occurred */
#define CAN_ERR_0004        (CAN_ERR_BASE + 0X4U)       /* Attempt to add filter conflict that already exists */
#define CAN_ERR_0005        (CAN_ERR_BASE + 0X5U)       /* Maximum limit reached */
#define CAN_ERR_0006        (CAN_ERR_BASE + 0X6U)       /* Cannot find filter or filter is already removed */
#define CAN_ERR_0007        (CAN_ERR_BASE + 0X7U)       /* Cannot find a proper ID filter setting */
#define CAN_ERR_0008        (CAN_ERR_BASE + 0X8U)       /* Unable to find an available buffer for transmit */
#define CAN_ERR_0009        (CAN_ERR_BASE + 0X9U)       /* TQ parameters out of range */

#define CAN_ERR_NONE            (OK)
#define CAN_ERR_ARG             CAN_ERR_0000
#define CAN_ERR_TIMEOUT         CAN_ERR_0003
#define CAN_ERR_NO_TX_BUFFER    CAN_ERR_0008
#define CAN_ERR_MUTEX           CAN_ERR_0001
#define CAN_ERR_INVALID_TQ      CAN_ERR_0009
/*-----------------------------------------------------------------------------------------------*\
 * Definitions of CAN Length Code
\*-----------------------------------------------------------------------------------------------*/

#define AMBA_CAN_DATA_LENGTH_0          0U
#define AMBA_CAN_DATA_LENGTH_1          1U
#define AMBA_CAN_DATA_LENGTH_2          2U
#define AMBA_CAN_DATA_LENGTH_3          3U
#define AMBA_CAN_DATA_LENGTH_4          4U
#define AMBA_CAN_DATA_LENGTH_5          5U
#define AMBA_CAN_DATA_LENGTH_6          6U
#define AMBA_CAN_DATA_LENGTH_7          7U
#define AMBA_CAN_DATA_LENGTH_8          8U
#define AMBA_CAN_DATA_LENGTH_12         9U
#define AMBA_CAN_DATA_LENGTH_16         10U
#define AMBA_CAN_DATA_LENGTH_20         11U
#define AMBA_CAN_DATA_LENGTH_24         12U
#define AMBA_CAN_DATA_LENGTH_32         13U
#define AMBA_CAN_DATA_LENGTH_48         14U
#define AMBA_CAN_DATA_LENGTH_64         15U
#define AMBA_NUM_CAN_DATA_LENGTH        16U

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of CAN Message Buffer
\*-----------------------------------------------------------------------------------------------*/

#define AMBA_CAN_MSG_BUF_0              0U         /* CAN message buffer 0 */
#define AMBA_CAN_MSG_BUF_1              1U         /* CAN message buffer 1 */
#define AMBA_CAN_MSG_BUF_2              2U         /* CAN message buffer 2 */
#define AMBA_CAN_MSG_BUF_3              3U         /* CAN message buffer 3 */
#define AMBA_CAN_MSG_BUF_4              4U         /* CAN message buffer 4 */
#define AMBA_CAN_MSG_BUF_5              5U         /* CAN message buffer 5 */
#define AMBA_CAN_MSG_BUF_6              6U         /* CAN message buffer 6 */
#define AMBA_CAN_MSG_BUF_7              7U         /* CAN message buffer 7 */
#define AMBA_CAN_MSG_BUF_8              8U         /* CAN message buffer 8 */
#define AMBA_CAN_MSG_BUF_9              9U         /* CAN message buffer 9 */
#define AMBA_CAN_MSG_BUF_10             10U        /* CAN message buffer 10 */
#define AMBA_CAN_MSG_BUF_11             11U        /* CAN message buffer 11 */
#define AMBA_CAN_MSG_BUF_12             12U        /* CAN message buffer 12 */
#define AMBA_CAN_MSG_BUF_13             13U        /* CAN message buffer 13 */
#define AMBA_CAN_MSG_BUF_14             14U        /* CAN message buffer 14 */
#define AMBA_CAN_MSG_BUF_15             15U        /* CAN message buffer 15 */
#define AMBA_CAN_MSG_BUF_16             16U        /* CAN message buffer 16 */
#define AMBA_CAN_MSG_BUF_17             17U        /* CAN message buffer 17 */
#define AMBA_CAN_MSG_BUF_18             18U        /* CAN message buffer 18 */
#define AMBA_CAN_MSG_BUF_19             19U        /* CAN message buffer 19 */
#define AMBA_CAN_MSG_BUF_20             20U        /* CAN message buffer 20 */
#define AMBA_CAN_MSG_BUF_21             21U        /* CAN message buffer 21 */
#define AMBA_CAN_MSG_BUF_22             22U        /* CAN message buffer 22 */
#define AMBA_CAN_MSG_BUF_23             23U        /* CAN message buffer 23 */
#define AMBA_CAN_MSG_BUF_24             24U        /* CAN message buffer 24 */
#define AMBA_CAN_MSG_BUF_25             25U        /* CAN message buffer 25 */
#define AMBA_CAN_MSG_BUF_26             26U        /* CAN message buffer 26 */
#define AMBA_CAN_MSG_BUF_27             27U        /* CAN message buffer 27 */
#define AMBA_CAN_MSG_BUF_28             28U        /* CAN message buffer 28 */
#define AMBA_CAN_MSG_BUF_29             29U        /* CAN message buffer 29 */
#define AMBA_CAN_MSG_BUF_30             30U        /* CAN message buffer 30 */
#define AMBA_CAN_MSG_BUF_31             31U        /* CAN message buffer 31 */
#define AMBA_NUM_CAN_MSG_BUF            32U

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of CAN Channel
\*-----------------------------------------------------------------------------------------------*/

#define AMBA_CAN_CHANNEL0               0U         /* CAN Channel-0 */
#define AMBA_NUM_CAN_CHANNEL            1U

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of CAN Operation Mode
\*-----------------------------------------------------------------------------------------------*/

#define AMBA_CAN_OP_MODE_NORMAL         0U          /* Normal Mode */
#define AMBA_CAN_OP_MODE_INSIDE_LPBK    1U          /* Inside Loopback Mode */
#define AMBA_CAN_OP_MODE_OUTSIDE_LPBK   2U          /* Outside Loopback Mode */
#define AMBA_CAN_OP_MODE_LISTEN         3U          /* Listen Only Mode */
#define AMBA_NUM_CAN_OP_MODE            4U

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of CAN Time Quanta
\*-----------------------------------------------------------------------------------------------*/

typedef struct {
    UINT32                  BRP;                    /* Baud Rate Prescaler */
    UINT32                  PropSeg;                /* Propagation Time Segment */
    UINT32                  PhaseSeg1;              /* Phase Buffer Segment 1 */
    UINT32                  PhaseSeg2;              /* Phase Buffer Segment 2 */
    UINT32                  SJW;                    /* (Re-)Synchronization Jump Width */
} AMBA_CAN_TQ_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of CAN Message Buffer Format
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32                  Id;                     /* Message ID */
    UINT8                   Extension;              /* Extented ID format (11 bits or 29 bits) */
    UINT8                   RemoteTxReq;            /* Remote transmission request */
    UINT8                   Priority;               /* Message priority, the message with lower priority value will be transmitted first */
    UINT8                   DataLengthCode;         /* Data length code */
    UINT8                   Data[8];                /* Message data */
} AMBA_CAN_MSG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of CAN FD Message Buffer Format
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32                  Id;                     /* Message ID */
    UINT8                   Extension;              /* Extented ID format (11 bits or 29 bits) */
    UINT8                   Priority;               /* Message priority, the message with lower priority value will be transmitted first */
    UINT8                   FlexibleDataRate;       /* FD frame in CiA specifications (EDL == 1) */
    UINT8                   BitRateSwitch;          /* FD bit rate switch, CAN data at a higher bitrate */
    UINT8                   ErrorStateIndicator;    /* FD error state indicator (Filled by hardware only) */
    UINT8                   DataLengthCode;         /* Data length code */
    UINT8                   Data[64];               /* Message data, the maximum size in CAN 2.0 and FD are 8 and 64 */
} AMBA_CAN_FD_MSG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of CAN ID Filter
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32                  IdFilter;               /* Filter value for identifier comparison */
    UINT32                  IdMask;                 /* Mask value for identifier comparison */
} AMBA_CAN_FILTER_s;

typedef struct {
    UINT32          SysClkFreq;             /* System clock frequency (hz) */
    UINT32          NominalTQ;              /* Time quantum for nominal bit time (ns) */
    UINT32          DataTQ;                 /* Time quantum for data bit time (ns) */
    UINT32          NominalBitLen;          /* Bit length for nominal bit time (tq) */
    UINT32          DataBitLen;             /* Bit length for data bit time (tq) */
    UINT32          NominalBitRate;         /* Bit rate for nominal bit time (bps) */
    UINT32          DataBitRate;            /* Bit rate for data bit time (bps) */
} AMBA_CAN_BIT_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of CAN 2.0 Config
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT8                   OpMode;                 /* Operation Mode*/
    UINT8                   EnableDMA;              /* CANC DMA Mode */
    AMBA_CAN_TQ_s           TimeQuanta;             /* Time quanta for CAN 2.0 A and B */
    UINT32                  MaxRxRingBufSize;        /* maximum receive ring-buffer size in bytes */
    UINT8                   *pRxRingBuf;            /* pointer to the receive ring-buffer */
    AMBA_CAN_TQ_s           FdTimeQuanta;           /* Time quanta for CAN FD */
    UINT32                  FdMaxRxRingBufSize;      /* maximum receive ring-buffer size for CAN FD in bytes */
    UINT8                   *pFdRxRingBuf;          /* pointer to the receive ring-buffer for CAN FD */
    UINT8                   NumIdFilter;            /* Number of Rx ID filters */
    AMBA_CAN_FILTER_s       *pIdFilter;             /* pointer to the ID filters */

} AMBA_CAN_CONFIG_s;

#endif /* AMBA_CAN_DEF_H */
