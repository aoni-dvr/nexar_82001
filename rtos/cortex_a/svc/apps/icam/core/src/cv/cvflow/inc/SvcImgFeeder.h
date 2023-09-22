/**
 *  @file SvcImgFeeder.h
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
 *  @details Header of SvcImgFeeder
 *
 */

#ifndef SVC_IMG_FEEDER_H
#define SVC_IMG_FEEDER_H

#define SVC_IMG_FEEDER_MAX_ID               (12U)    /* Maximum number of feeders */
#define SVC_IMG_FEEDER_PORT_MAX             (12U)    /* Maximum number of ports per feeder */

/* Feeder flag */
#define SVC_IMG_FEEDER_FLAG_SYNC_CAP_SEQ    (1U)
#define SVC_IMG_FEEDER_FLAG_DIVISOR         (2U)
#define SVC_IMG_FEEDER_FLAG_EXT_SYNC        (4U)

#define SVC_IMG_FEEDER_FLAG_DIVISOR_SFT     (8U)
#define SVC_IMG_FEEDER_FLAG_DIVISOR_BITS    (0xFFU)

typedef struct {
    UINT32 FeederID;
    UINT32 PortID;
} SVC_IMG_FEEDER_PORT_s;

typedef struct {
    UINT32 Reserved;
} SVC_IMG_FEEDER_INIT_CFG_s;

typedef struct {
    UINT32 TaskPriority;
    UINT32 CpuBits;
    UINT32 FeederFlag;
/* Feeder flag */
#define SVC_IMG_FEEDER_FLAG_SYNC_CAP_SEQ    (1U)

} SVC_IMG_FEEDER_CREATE_CFG_s;

typedef UINT32 (*SVC_IMG_FEEDER_EVENT_CALLBACK_f)(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

typedef struct {
    UINT8   NumPath;
    SVC_CV_INPUT_IMG_CONTENT_s Content[SVC_CV_INPUT_INFO_MAX_DATA_NUM];
    SVC_IMG_FEEDER_EVENT_CALLBACK_f SendFunc;
} SVC_IMG_FEEDER_PORT_CFG_s;

/* Feeder to Receiver event */
#define SVC_IMG_FEEDER_IMAGE_DATA_READY     (1U)    /* SVC_CV_INPUT_IMG_INFO_s */
#define SVC_IMG_FEEDER_PICINFO_READY        (2U)    /* memio_source_recv_picinfo_t */


UINT32 SvcImgFeeder_Init(SVC_IMG_FEEDER_INIT_CFG_s *pCfg);

UINT32 SvcImgFeeder_Create(UINT32 FeederID, SVC_IMG_FEEDER_CREATE_CFG_s *pCfg);
UINT32 SvcImgFeeder_Delete(UINT32 FeederID);

#define SVC_IMG_FEEDER_CTRL_ENABLE          (0U)
#define SVC_IMG_FEEDER_EXT_SYNC             (1U)
UINT32 SvcImgFeeder_Ctrl(UINT32 FeederID, UINT32 Msg, void *pInfo);

/* Open a port and specify the configuration of the port */
UINT32 SvcImgFeeder_OpenPort(UINT32 FeederID, SVC_IMG_FEEDER_PORT_CFG_s *pCfg, SVC_IMG_FEEDER_PORT_s *pPort);
/* Close the port */
UINT32 SvcImgFeeder_ClosePort(SVC_IMG_FEEDER_PORT_s *pPort);

/* Connect to the port to listen to feeder */
UINT32 SvcImgFeeder_Connect(SVC_IMG_FEEDER_PORT_s *pPort);
/* Disconnect the port*/
UINT32 SvcImgFeeder_Disconnect(SVC_IMG_FEEDER_PORT_s *pPort);

/* Receiver to Feeder message */
#define SVC_IMG_FEEDER_RECEIVER_READY       (1U)
UINT32 SvcImgFeeder_MsgToFeeder(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Msg, void *pInfo);

void SvcImgFeeder_DebugEnable(UINT32 DebugEnable);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
UINT32 SvcImgFeeder_ReEnable(UINT32 FeederID);
#endif

#endif /* SVC_IMG_FEEDER_H */
