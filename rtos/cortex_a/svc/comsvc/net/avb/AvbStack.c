/**
 * @file AvbStack.c
 * AVB networks stack
 *
 * @ingroup enet
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
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "AmbaKAL.h"
#include "AmbaENET.h"
#include "NetStack.h"
#include "EnetUtility.h"
#include "AvbStack.h"
#include "AvbAvtp.h"

static AMBA_AVB_STACK_CONFIG_s *pAmbaAvbStackConfig[] = {
    NULL,
#if (AVB_STACK_INSTANCES > 1U)
    NULL,
#endif
};

extern AMBA_ENET_TS_s *pAmbaCSL_EnetPhc[AVB_STACK_INSTANCES];

static UINT32 AvbStackRxHandle(const UINT32 Idx, const void *frame,  const UINT16 Len, const UINT16 type)
{
    UINT32 Ret = 0U;

    if (Idx >= AVB_STACK_INSTANCES) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = NET_ERR_EINVAL;
    } else if (frame == NULL) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "frame NULL!");
        Ret = NET_ERR_EINVAL;
    } else if (pAmbaAvbStackConfig[Idx] == NULL) {
        AvbDebug("please run AmbaAvbStack_SetConfig!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        extern UINT32 AvbAvtp_Rx(UINT32 Idx, const void *frame, UINT16 framelen);
        extern UINT32 AvbPtp_Rx(UINT32 Idx, const void *frame, UINT16 framelen);

        switch (type) {
        case ETH_PTP2_TYPE:
            Ret = AvbPtp_Rx(Idx, frame, Len);
            break;
        case ETH_AVTP_TYPE:
            {
#define AVTPDU_SUBTYPE_ADP             ((UINT8)(0x7Au | 0x80u)) /**< AVDECC Discovery Protocol */
#define AVTPDU_SUBTYPE_AECP            ((UINT8)(0x7Bu | 0x80u)) /**< AVDECC Enumeration and Control Protocol */
#define AVTPDU_SUBTYPE_ACMP            ((UINT8)(0x7Cu | 0x80u)) /**< AVDECC Connection Management Protocol */
                const ETH_HDR_s *ethhdr;
                void *ptr = NULL;
                UINT16 L3Len = Len;
                UINT8 subtype;

                AmbaMisra_TypeCast(&ethhdr, &frame);
                AmbaMisra_TypeCast(&ptr, &frame);
                (void) AmbaNetStack_RemoveEthHdr(&ptr, &L3Len);

                if (AmbaWrap_memcpy(&subtype, ptr, sizeof(UINT8))!= 0U) { }

                if ((subtype == AVTPDU_SUBTYPE_ADP) ||
                    (subtype == AVTPDU_SUBTYPE_AECP) ||
                    (subtype == AVTPDU_SUBTYPE_ACMP)) {
                    if (pAmbaAvbStackConfig[Idx]->pAvdeccCb != NULL) {

                        Ret = pAmbaAvbStackConfig[Idx]->pAvdeccCb(Idx,
                                                                  ethhdr->dst,
                                                                  ethhdr->src,
                                                                  ptr,
                                                                  L3Len);
                    }
                } else {
                    Ret = AvbAvtp_Rx(Idx, frame, Len);
                }
            }
            break;
        default:
            /* TBD */
            break;
        }
    }

    return Ret;
}

/* config all avb MAC address here */
static UINT32 AvbStackRxFilter(const UINT32 Idx)
{
    /* 01:00:5e:00:00:6b PTPv2L4: Pdelay_Req, Pdelay_Resp, Pdelay_Resp_Follow_Up */
    const UINT8 PTP2L4_PDELAY[6] = { 0x01U, 0x00U, 0x5eU, 0x00U, 0x00U, 0x6bU };
    /* 01:00:5e:00:01:81 PTP1 */
    const UINT8 PTP1_DST[6] = { 0x01U, 0x00U, 0x5eU, 0x00U, 0x01U, 0x81U };
    /* 01:1b:19:00:00:00 PTP2 Announce, Sync, Follow_Up, Delay_Req, Delay_Resp */
    const UINT8 PTPv2_DST[6] = { 0x01U, 0x1bU, 0x19U, 0x00U, 0x00U, 0x00U };
    /* 01:80:c2:00:00:0e ALL gPTP + PTPv2L2: Pdelay_Req, Pdelay_Resp, Pdelay_Resp_Follow_Up */
    const UINT8 GPTP_DST[6] = { 0x01U, 0x80U, 0xc2U, 0x00U, 0x00U, 0x0eU };
    /* 91:e0:f0:00:0e:80: 91:e0:f0:00:00:00 ~ 91:e0:f0:00:fd:ff MAAP dynamic allocation pool */
    const UINT8 AVTP1[6] = { 0x91U, 0xe0U, 0xf0U, 0x00U, 0x0eU, 0x80U };
    /* 91:e0:f0:00:fe:00: 91:e0:f0:00:fe:00 ~ 91:e0:f0:00:fe:ff MAAP locally administered pool */
    const UINT8 AVTP2[6] = { 0x91U, 0xe0U, 0xf0U, 0x00U, 0xfeU, 0x00U };
    /* 91:e0:f0:01:00:00: 91:e0:f0:01:00:00 ~ 91:e0:f0:01:ff:ff ieee1722.1 */
    const UINT8 AVDECC[6] = { 0x91U, 0xe0U, 0xf0U, 0x01U, 0x00U, 0x00U };

    (void)AmbaEnet_McastRxHash(Idx, PTP2L4_PDELAY);
    (void)AmbaEnet_McastRxHash(Idx, PTP1_DST);
    (void)AmbaEnet_McastRxHash(Idx, PTPv2_DST);
    (void)AmbaEnet_McastRxHash(Idx, GPTP_DST);
    (void)AmbaEnet_McastRxHash(Idx, AVTP1);
    (void)AmbaEnet_McastRxHash(Idx, AVTP2);
    (void)AmbaEnet_McastRxHash(Idx, AVDECC);

    return NET_ERR_NONE;
}

static void AvbPtpUserTaskCreate(UINT32 Idx)
{
    extern void *AvbPtp_TaskEntry(void *arg);
    static char taskname[2][32] = { "AvbPtP0Task", "AvbPtP1Task" };
    const AMBA_AVB_STACK_CONFIG_s *pAvbConfig = pAmbaAvbStackConfig[Idx];
    UINT32 err;
    static UINT8 AvbPtpTaskStack[AVB_STACK_INSTANCES][AVB_PTP_TASKSTACKSIZE] __attribute__((section(".bss.noinit")));
    static AMBA_KAL_TASK_t AvbPtpTask[AVB_STACK_INSTANCES] = {
        [0] = {
            .tx_thread_id = 0U,
        },
#if (AVB_STACK_INSTANCES >= 2U)
        [1] = {
            .tx_thread_id = 0U,
        },
#endif
    };

    if (AvbPtpTask[Idx].tx_thread_id == 0U) {
        const UINT8 *pBuf;
        void *ptr;
        void *arg = NULL;

        /*arg = (void *)Idx;*/
        (void)arg;
        if (AmbaWrap_memcpy(&arg, &Idx, sizeof(Idx))!= 0U) { }
        if (AmbaWrap_memset(&AvbPtpTaskStack[Idx][0], 0, AVB_PTP_TASKSTACKSIZE)!= 0U) { }
        pBuf = &AvbPtpTaskStack[Idx][0];
        AmbaMisra_TypeCast(&ptr, &pBuf);
        err = AmbaKAL_TaskCreate(&AvbPtpTask[Idx],
                                 taskname[Idx],
                                 pAvbConfig->PtpConfig.TaskPri,
                                 AvbPtp_TaskEntry,
                                 arg,
                                 ptr,
                                 AVB_PTP_TASKSTACKSIZE,
                                 0U);
        if (err != 0U) {
            EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "AmbaKAL_TaskCreate failed");
        }

        // Using Core-0 (0x01)
        err = AmbaKAL_TaskSetSmpAffinity(&AvbPtpTask[Idx], pAvbConfig->PtpConfig.SmpCoreSet);
        if (err != 0U) {
            EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "AmbaKAL_TaskSetSmpAffinity failed");
        }
        err = (UINT32)AmbaKAL_TaskResume(&AvbPtpTask[Idx]);
        if (err != 0U) {
            EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "AmbaKAL_TaskResume failed");
        }
    }
}

static UINT32 AvbStackListenerLookUp(const AMBA_AVB_AVTP_LISTENER_s *pListener, AMBA_AVB_AVTP_LISTENER_INFO_s **ppListenerInfo)
{
    UINT32 Ret = NET_ERR_NONE,i;

    for(i = 0; i < AVB_AVTP_MAX_LISTENER_NUM; i++) {
        if( (AvtpListenerInfo[i].Register == 1U) && ( AvtpListenerInfo[i].pListener == pListener) ) {
            *ppListenerInfo =  &(AvtpListenerInfo[i]);
            break;
        }
    }

    if(i == AVB_AVTP_MAX_LISTENER_NUM) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "Listener over range!");
        Ret = NET_ERR_ENODEV;
    }

    return Ret;
}

static UINT32 AvbStackTalkerLookUp(const AMBA_AVB_AVTP_TALKER_s *pTalker, AMBA_AVB_AVTP_TALKER_INFO_s **ppTalkerInfo)
{
    UINT32 Ret = NET_ERR_NONE,i;

    for(i = 0; i < AVB_AVTP_MAX_TALKER_NUM; i++) {
        if( (AvtpTalkerInfo[i].Register == 1U) && ( AvtpTalkerInfo[i].pTalker == pTalker) ) {
            *ppTalkerInfo =  &(AvtpTalkerInfo[i]);
            break;
        }
    }

    if(i == AVB_AVTP_MAX_TALKER_NUM) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "Talker over range!");
        Ret = NET_ERR_ENODEV;
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to set Avb stack config
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] pAvbConfig Avb Stack Config Struct for ENET
 * @return enet error number
 */
UINT32 AmbaAvbStack_SetConfig(UINT32 Idx, AMBA_AVB_STACK_CONFIG_s *pAvbConfig)
{
    UINT32 Ret = NET_ERR_NONE;

    if (pAvbConfig == NULL) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pAvbConfig NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        pAmbaAvbStackConfig[Idx] = pAvbConfig;
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function is used to get Avb stack config
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [out] pAvbConfig Avb Stack Config Struct for ENET
 * @return enet error number
 */
UINT32 AmbaAvbStack_GetConfig(UINT32 Idx, AMBA_AVB_STACK_CONFIG_s **pAvbConfig)
{
    UINT32 Ret = NET_ERR_NONE;

    if (Idx >= AVB_STACK_INSTANCES) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = NET_ERR_EINVAL;
    } else if (pAvbConfig == NULL) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pAvbConfig NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        *pAvbConfig = pAmbaAvbStackConfig[Idx];
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function is used to configure and init AvbStack by Ethernet driver
 *
 * @param [in] pAvbConfig AvbStack Config
 * @return enet error number
 */
UINT32 AmbaAvbStack_EnetInit(AMBA_AVB_STACK_CONFIG_s *pAvbConfig)
{
    UINT32 Ret = NET_ERR_NONE;

    if (pAvbConfig == NULL) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pAvbConfig NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        UINT32 Idx = pAvbConfig->Idx;
        pAmbaAvbStackConfig[Idx] = pAvbConfig;
        pAvbConfig->pGetTxBufCb = AmbaEnet_GetTxBuf;
        pAvbConfig->pGetRxBufCb = AmbaEnet_GetRxBuf;
        pAvbConfig->pDoTxCb = AmbaEnet_Tx;
        pAvbConfig->pDoTxWaitCb = AmbaEnet_TxWait;
        pAvbConfig->pRxCb = AvbStackRxHandle;
        if (pAvbConfig->PtpConfig.MstSlv == AVB_PTP_MASTER) {
            pAvbConfig->PtpConfig.PtpPri = PTP2_GM_LIKELY;
        }
        else if (pAvbConfig->PtpConfig.MstSlv == AVB_PTP_SLAVE) {
            pAvbConfig->PtpConfig.PtpPri = PTP2_GM_UNLIKELY;
        }
        else {
            pAvbConfig->PtpConfig.PtpPri = PTP2_GM_AUTO;
        }
        (void)AvbStackRxFilter(Idx);
        AvbPtpUserTaskCreate(Idx);
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to get Ptp Hardware Clock value
 *

 * @param [in] Idx     ENET controller index on AMBA SoC, value start from 0
 * @param [out] pSec    Seconds value of Ptp Hardware Clock
 * @param [out] pNs     Nano-Seconds value of Ptp Hardware Clock
 * @return enet error number
 */
UINT32 AmbaAvbStack_GetPhc(UINT32 Idx, UINT32 *pSec, UINT32 *pNs)
{
    const AMBA_ENET_TS_s *const pPhcTs = pAmbaCSL_EnetPhc[Idx];
    UINT32 Ret = NET_ERR_NONE;

    /* arg check */
    if (Idx >= AVB_STACK_INSTANCES) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = NET_ERR_EINVAL;
    } else if (pSec == NULL) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pSec NULL!");
        Ret = NET_ERR_EINVAL;
    } else if (pNs == NULL) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pNs NULL!");
        Ret = NET_ERR_EINVAL;
    } else if (pPhcTs == NULL) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pPhcTs NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        *pSec = pPhcTs->Sec;
        *pNs = pPhcTs->Ns;
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to get Ptp Hardware Clock value
 *

 * @param [in] Idx     ENET controller index on AMBA SoC, value start from 0
 * @param [out] pPhcNs  64 bit value of Ptp Hardware Clock
 * @return enet error number
 */
UINT32 AmbaAvbStack_GetPhcNs(UINT32 Idx, UINT64 *pPhcNs)
{
    const AMBA_ENET_TS_s *const pPhcTs = pAmbaCSL_EnetPhc[Idx];
    UINT32 Ret = NET_ERR_NONE;

    /* arg check */
    if (Idx >= AVB_STACK_INSTANCES) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = NET_ERR_EINVAL;
    } else if (pPhcNs == NULL) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pPhcNs NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        *pPhcNs = pPhcTs->Ns;
        *pPhcNs  += pPhcTs->Sec * NS_PER_SECOND;
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to Register avtp listener
 *
 * @param [in] pListener Listener data struct include, pAvbConfig, StreamID, pBuf, BufSize, pFrameQueue, FrameQueueNum
 * @return enet error number
 */
UINT32 AmbaAvbStack_AvtpListenerReg(AMBA_AVB_AVTP_LISTENER_s *pListener)
{
    UINT32 Ret = NET_ERR_NONE, Ret1 = NET_ERR_NONE,i;
    char name[64] = {"AvtpBufQueue"};

    if ( pListener == NULL ) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pListener == NULL!");
        Ret = NET_ERR_EINVAL;
    } else if (pListener->pAvbConfig == NULL) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pAvbConfig == NULL");
        Ret = NET_ERR_EINVAL;
    } else if ( pListener->BufSize == 0U ) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "BufSize == 0 !");
        Ret = NET_ERR_EINVAL;
    } else if ( pListener->FrameQueueNum == 0U ) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "FrameInfoNum == 0 !");
        Ret = NET_ERR_EINVAL;
    } else if ( pListener->pBuf == NULL ) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pBuf == 0 !");
        Ret = NET_ERR_EINVAL;
    } else if ( pListener->pFrameQueue == NULL ) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pFrameInfo == 0 !");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        for(i = 0; i < AVB_AVTP_MAX_LISTENER_NUM; i++) {
            if(AvtpListenerInfo[i].Register == 0U) {
                AvtpListenerInfo[i].pListener = pListener;
                AvtpListenerInfo[i].FirstIdr = 0U;
                Ret1 += AvbAvtp_BufQueueInit( &(AvtpListenerInfo[i].BufQueue), pListener->pBuf,pListener->BufSize);
                Ret1 += AmbaKAL_MsgQueueCreate(&(AvtpListenerInfo[i].FrameQueue), name, sizeof(AMBA_AVB_AVTP_FRAME_INFO_s), pListener->pFrameQueue, pListener->FrameQueueNum*sizeof(AMBA_AVB_AVTP_FRAME_INFO_s));
                Ret1 += AvbAvtp_UnPacketReset(&(AvtpListenerInfo[i].BufQueue), &(AvtpListenerInfo[i].Unpacket));
                Ret1 += AmbaEnet_McastRxHash(pListener->pAvbConfig->Idx, pListener->MultiCastMac);
                if(Ret1 == NET_ERR_NONE) {
                    AvtpListenerInfo[i].Register = 1U;
                } else {
                    Ret = NET_ERR_EPERM;
                }
                break;
            }
        }

        if(i == AVB_AVTP_MAX_LISTENER_NUM) {
            EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "Listener over range!");
            Ret = NET_ERR_EBUSY;
        }
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to Un-Register avtp listener
 *
 * @param [in] pListener Listener data struct
 * @return enet error number
 */
UINT32 AmbaAvbStack_AvtpListenerUnReg(const AMBA_AVB_AVTP_LISTENER_s *pListener)
{
    UINT32 Ret = NET_ERR_NONE, i;

    if ( pListener == NULL ) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pListener == NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        for(i = 0; i < AVB_AVTP_MAX_LISTENER_NUM; i++) {
            if(AvtpListenerInfo[i].pListener == pListener) {
                AvtpListenerInfo[i].pListener = NULL;
                (void) AvbAvtp_BufQueueDeInit(&(AvtpListenerInfo[i].BufQueue));
                (void) AmbaKAL_MsgQueueDelete(&(AvtpListenerInfo[i].FrameQueue));
                AvtpListenerInfo[i].Register = 0;
                break;
            }
        }

        if(i == AVB_AVTP_MAX_LISTENER_NUM) {
            EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "Listener is not exit!");
            Ret = NET_ERR_ENODEV;
        }
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to receive frame to avtp listener
 *
 * @param [in] pListener Listener data struct
 * @param [in] pFrame [in] Frame information
 * @param [in] TimeOut Timeout to wait Rx frame
 * @return enet error number
 */
UINT32 AmbaAvbStack_AvtpListenerRx(const AMBA_AVB_AVTP_LISTENER_s *pListener, const AMBA_AVB_AVTP_FRAME_INFO_s *pFrame, UINT32 TimeOut)
{
    UINT32 Ret = NET_ERR_NONE;
    AMBA_AVB_AVTP_LISTENER_INFO_s *pListenerInfo;
    void *pMsg;

    if ( pListener == NULL ) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pListener == NULL!");
        Ret = NET_ERR_EINVAL;
    } else if ( pFrame == NULL ) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "ppFrame == NULL!");
        Ret = NET_ERR_EINVAL;
    } else if (pListener->pAvbConfig == NULL) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pAvbConfig == NULL");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        Ret = AvbStackListenerLookUp(pListener, &pListenerInfo);
        if(Ret == 0U) {
            AmbaMisra_TypeCast(&pMsg, &pFrame);
            if( 0U != AmbaKAL_MsgQueueReceive(&(pListenerInfo->FrameQueue), pMsg, TimeOut)){
                Ret = NET_ERR_ETIMEDOUT;
            }
        }
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to tell driver receive frame done from avtp listener
 *
 * @param [in] pListener Listener data struct
 * @param [in] pFrame Frame information
 * @return enet error number
 */
UINT32 AmbaAvbStack_AvtpListenerRxDone(const AMBA_AVB_AVTP_LISTENER_s *pListener, const AMBA_AVB_AVTP_FRAME_INFO_s *pFrame)
{
    UINT32 Ret = NET_ERR_NONE,i;
    UINT8 *pCurBuf;
    AMBA_AVB_AVTP_LISTENER_INFO_s *pListenerInfo;

    if ( pListener == NULL ) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pListener == NULL!");
        Ret = NET_ERR_EINVAL;
    } else if ( pFrame == NULL ) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pFrame == NULL!");
        Ret = NET_ERR_EINVAL;
    } else if (pListener->pAvbConfig == NULL) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pAvbConfig == NULL");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        Ret = AvbStackListenerLookUp(pListener, &pListenerInfo);
        if(Ret == NET_ERR_NONE) {
            Ret = AvbAvtp_BufQueueCurrentOut(&(pListenerInfo->BufQueue), &pCurBuf);
            if(Ret == NET_ERR_NONE) {
                if(pCurBuf == pFrame->Vec[0].pBase) {
                    for(i = 0; i < pFrame->VecNum; i++) {
                        Ret = AvbAvtp_BufQueueSeOut( &(pListenerInfo->BufQueue), pFrame->Vec[i].Len);
                        if(Ret != NET_ERR_NONE) {
                            break;
                        }
                    }
                } else {
                    EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "Buffer Queue error");
                    (void) AmbaAvbStack_AvtpListenerUnReg(pListener);
                    Ret = NET_ERR_EFAULT;
                }
            }
        }
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to Register avtp talker
 *
 * @param [in] pTalker Talker data struct include, pAvbConfig, StreamID, DstMac, tci
 * @return enet error number
 */
UINT32 AmbaAvbStack_AvtpTalkerReg(AMBA_AVB_AVTP_TALKER_s *pTalker)
{
    UINT32 Ret = NET_ERR_NONE, i;

    if ( pTalker == NULL ) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pTalker == NULL!");
        Ret = NET_ERR_EINVAL;
    } else if (pTalker->pAvbConfig == NULL) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pAvbConfig == NULL");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        for(i = 0; i < AVB_AVTP_MAX_TALKER_NUM; i++) {
            if(AvtpTalkerInfo[i].Register == 0U) {
                AvtpTalkerInfo[i].pTalker = pTalker;
                AvtpTalkerInfo[i].Register = 1U;
                AvtpTalkerInfo[i].SeqNum = 0U;
                break;
            }
        }

        if(i == AVB_AVTP_MAX_TALKER_NUM) {
            EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "Talker over range!");
            Ret = NET_ERR_EBUSY;
        }
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to Un-Register avtp talker
 *
 * @param [in] pTalker Talker data struct
 * @return enet error number
 */
UINT32 AmbaAvbStack_AvtpTalkerUnReg(const AMBA_AVB_AVTP_TALKER_s *pTalker)
{
    UINT32 Ret = NET_ERR_NONE, i;

    if ( pTalker == NULL ) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pTalker == NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        for(i = 0; i < AVB_AVTP_MAX_TALKER_NUM; i++) {
            if(AvtpTalkerInfo[i].pTalker == pTalker) {
                AvtpTalkerInfo[i].pTalker = NULL;
                AvtpTalkerInfo[i].Register = 0U;
                break;
            }
        }

        if(i == AVB_AVTP_MAX_TALKER_NUM) {
            EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "Talker is not exit!");
            Ret = NET_ERR_ENODEV;
        }
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to transmit frame from avtp talker
 *
 * @param [in] pTalker Talker data struct
 * @param [in] Frame Frame data struct include, Vec, VecNum, FrameType, FrameTimeStamp, Completed
 * @return enet error number
 */
UINT32 AmbaAvbStack_AvtpTalkerTx(const AMBA_AVB_AVTP_TALKER_s *pTalker, const AMBA_AVB_AVTP_FRAME_INFO_s *Frame)
{
    UINT32 Ret = NET_ERR_NONE;
    AMBA_AVB_AVTP_TALKER_INFO_s *pTalkerInfo;

    /* arg check */
    if (pTalker == NULL) {
        Ret = NET_ERR_EINVAL;
    } else if (Frame == NULL) {
        Ret = NET_ERR_EINVAL;
    } else if (pTalker->pAvbConfig == NULL) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "pAvbConfig == NULL");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        Ret = AvbStackTalkerLookUp(pTalker, &pTalkerInfo);
        if(Ret == NET_ERR_NONE) {
            if(Frame->FrameType == AVTP_FORMAT_SUBTYPE_H264) {
                Ret = H264Subsession_Packetize(pTalkerInfo, Frame);
            } else if((Frame->FrameType >= AVTP_FORMAT_SUBTYPE_RESERVED_CARINFO_ACF) && (Frame->FrameType <= AVTP_FORMAT_SUBTYPE_RESERVED_GRID_ACF)) {
                Ret = ControlSubsession_Packetize(pTalkerInfo, Frame);
            } else {
                EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "AmbaAvbStack_AvtpTxFrame: Unsupport FrameType");
            }
        }
    }

    return Ret;
}

