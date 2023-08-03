/**
 *  @file AmbaCANPeer.c
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
 *  @details CAN bus Peer APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaCAN.h"

UINT32 AmbaCANPeer_SetBaudRate(UINT32 CanCh,
                               UINT32 BRP,
                               UINT32 PropSeg,
                               UINT32 PhaseSeg1,
                               UINT32 PhaseSeg2,
                               UINT32 SJW,
                               UINT8  *pRingBuf,
                               UINT32 BufSize);
UINT32 AmbaCANPeer_SetFDBaudRate(UINT32 CanCh,
                                 UINT32 BRP,
                                 UINT32 PropSeg,
                                 UINT32 PhaseSeg1,
                                 UINT32 PhaseSeg2,
                                 UINT32 SJW,
                                 UINT8  *pRingBuf,
                                 UINT32 BufSize);
UINT32 AmbaCANPeer_AddFilter(UINT32 CanCh, UINT32 IdFilter, UINT32 IdMask, UINT32 CleanBefore);
UINT32 AmbaCANPeer_Enable(UINT32 CanCh);
UINT32 AmbaCANPeer_Read(UINT32 CanCh, UINT32 *pId, UINT8 *pBuf, UINT32 BufLEN, UINT32 *pActualLEN, UINT32 Timeout);
UINT32 AmbaCANPeer_FDRead(UINT32 CanCh, UINT32 *pId, UINT8 *pBuf, UINT32 BufLEN, UINT32 *pActualLEN, UINT32 Timeout);
UINT32 AmbaCANPeer_Disable(UINT32 CanCh);

#define MAX_CAN_PEER_FILEER_NUM     30U

typedef struct {
    UINT32  CanChannelId;
    AMBA_CAN_CONFIG_s   CanConfig;
} AMBA_CAN_PEER_CTRL_s;

typedef struct {
    UINT8  FilterNum;
    AMBA_CAN_FILTER_s   FilterConfig[MAX_CAN_PEER_FILEER_NUM];
} AMBA_CAN_PEER_FILTER_s;


static UINT8 CanPeer_DlcTable[AMBA_NUM_CAN_DATA_LENGTH] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64
};
static AMBA_CAN_PEER_FILTER_s CanPeer_Filter[AMBA_NUM_CAN_CHANNEL];
static AMBA_CAN_PEER_CTRL_s CanPeer_Config[AMBA_NUM_CAN_CHANNEL];

UINT32 AmbaCANPeer_SetBaudRate(UINT32 CanCh,
                               UINT32 BRP,
                               UINT32 PropSeg,
                               UINT32 PhaseSeg1,
                               UINT32 PhaseSeg2,
                               UINT32 SJW,
                               UINT8  *pRingBuf,
                               UINT32 BufSize)
{
    UINT32 Ret;

    if (CanCh >= AMBA_NUM_CAN_CHANNEL) {
        Ret = CAN_ERR_ARG;
    } else if (BufSize < sizeof(AMBA_CAN_MSG_s)) {
        Ret = CAN_ERR_ARG;
    } else {
        AMBA_CAN_CONFIG_s *Config = &CanPeer_Config[CanCh].CanConfig;
        CanPeer_Config[CanCh].CanChannelId = CanCh;
        Config->OpMode = AMBA_CAN_OP_MODE_LISTEN;
        Config->TimeQuanta.BRP = BRP;
        Config->TimeQuanta.PropSeg = PropSeg;
        Config->TimeQuanta.PhaseSeg1 = PhaseSeg1;
        Config->TimeQuanta.PhaseSeg2 = PhaseSeg2;
        Config->TimeQuanta.SJW = SJW;
        Config->pRxRingBuf = pRingBuf;
        Config->MaxRxRingBufSize = BufSize;
        Ret = CAN_ERR_NONE;
    }

    return Ret;
}

UINT32 AmbaCANPeer_SetFDBaudRate(UINT32 CanCh,
                                 UINT32 BRP,
                                 UINT32 PropSeg,
                                 UINT32 PhaseSeg1,
                                 UINT32 PhaseSeg2,
                                 UINT32 SJW,
                                 UINT8  *pRingBuf,
                                 UINT32 BufSize)
{
    UINT32 Ret;

    if (CanCh >= AMBA_NUM_CAN_CHANNEL) {
        Ret = CAN_ERR_ARG;
    } else if (BufSize < sizeof(AMBA_CAN_FD_MSG_s)) {
        Ret = CAN_ERR_ARG;
    } else {
        AMBA_CAN_CONFIG_s *Config = &CanPeer_Config[CanCh].CanConfig;
        CanPeer_Config[CanCh].CanChannelId = CanCh;
        Config->OpMode = AMBA_CAN_OP_MODE_LISTEN;
        Config->FdTimeQuanta.BRP = BRP;
        Config->FdTimeQuanta.PropSeg = PropSeg;
        Config->FdTimeQuanta.PhaseSeg1 = PhaseSeg1;
        Config->FdTimeQuanta.PhaseSeg2 = PhaseSeg2;
        Config->FdTimeQuanta.SJW = SJW;
        Config->pFdRxRingBuf = pRingBuf;
        Config->FdMaxRxRingBufSize = BufSize;
        Ret = CAN_ERR_NONE;
    }

    return Ret;
}

UINT32 AmbaCANPeer_AddFilter(UINT32 CanCh, UINT32 IdFilter, UINT32 IdMask, UINT32 CleanBefore)
{
    UINT32 Ret;

    if (CanCh >= AMBA_NUM_CAN_CHANNEL) {
        Ret = CAN_ERR_ARG;
    } else {
        AMBA_CAN_PEER_FILTER_s *Filter = &CanPeer_Filter[CanCh];

        if (CleanBefore != 0U) {
            if (AmbaWrap_memset(Filter, 0, sizeof(AMBA_CAN_PEER_FILTER_s)) != 0U) {
                /* Do nothing */
            }
        }
        if (Filter->FilterNum == AMBA_NUM_CAN_MSG_BUF) {
            Ret = CAN_ERR_ARG;
        } else {
            Filter->FilterConfig[Filter->FilterNum].IdFilter = IdFilter;
            Filter->FilterConfig[Filter->FilterNum].IdMask = IdMask;
            Filter->FilterNum++;
            Ret = CAN_ERR_NONE;
        }
    }

    return Ret;
}

UINT32 AmbaCANPeer_Enable(UINT32 CanCh)
{
    UINT32 Ret;

    if (CanCh >= AMBA_NUM_CAN_CHANNEL) {
        Ret = CAN_ERR_ARG;
    } else {
        AMBA_CAN_CONFIG_s *pConfig = &CanPeer_Config[CanCh].CanConfig;
        AMBA_CAN_PEER_FILTER_s *pFilter = &CanPeer_Filter[CanCh];
        if (pFilter->FilterNum != 0U) {
            pConfig->NumIdFilter = pFilter->FilterNum;
            pConfig->pIdFilter = pFilter->FilterConfig;
        } else {
            pConfig->NumIdFilter = 0;
            pConfig->pIdFilter = NULL;
        }
        Ret = AmbaCAN_Enable(CanCh, pConfig);
    }

    return Ret;
}

UINT32 AmbaCANPeer_Read(UINT32 CanCh, UINT32 *pId, UINT8 *pBuf, UINT32 BufLEN, UINT32 *pActualLEN, UINT32 Timeout)
{
    UINT32 Ret;

    if (CanCh >= AMBA_NUM_CAN_CHANNEL) {
        Ret = CAN_ERR_ARG;
    } else if ((pId == NULL) || (pBuf == NULL) || (pActualLEN == NULL)) {
        Ret = CAN_ERR_ARG;
    } else {
        AMBA_CAN_MSG_s Message;
        UINT32 i;

        Ret = AmbaCAN_Read(CanCh, &Message, Timeout);
        if (Ret == CAN_ERR_NONE) {
            *pId = Message.Id;
            *pActualLEN = (CanPeer_DlcTable[Message.DataLengthCode]<BufLEN) ? CanPeer_DlcTable[Message.DataLengthCode] : BufLEN;
            for (i=0; i<*pActualLEN; i++) {
                pBuf[i] = Message.Data[i];
            }
        }
    }

    return Ret;
}

UINT32 AmbaCANPeer_FDRead(UINT32 CanCh, UINT32 *pId, UINT8 *pBuf, UINT32 BufLEN, UINT32 *pActualLEN, UINT32 Timeout)
{
    UINT32 Ret;

    if (CanCh >= AMBA_NUM_CAN_CHANNEL) {
        Ret = CAN_ERR_ARG;
    } else if ((pId == NULL) || (pBuf == NULL) || (pActualLEN == NULL)) {
        Ret = CAN_ERR_ARG;
    } else {
        AMBA_CAN_FD_MSG_s Message;
        UINT32 i;

        Ret = AmbaCAN_ReadFd(CanCh, &Message, Timeout);
        if (Ret == CAN_ERR_NONE) {
            *pId = Message.Id;
            *pActualLEN = (CanPeer_DlcTable[Message.DataLengthCode]<BufLEN) ? CanPeer_DlcTable[Message.DataLengthCode] : BufLEN;
            for (i=0; i<*pActualLEN; i++) {
                pBuf[i] = Message.Data[i];
            }
        }
    }

    return Ret;
}

UINT32 AmbaCANPeer_Disable(UINT32 CanCh)
{
    return AmbaCAN_Disable(CanCh);
}

