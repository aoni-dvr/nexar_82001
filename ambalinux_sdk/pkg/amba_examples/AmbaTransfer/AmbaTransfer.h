/**
 *  @file AmbaTransfer.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Header of AmbaTransfer interface
 *
 */

#ifndef AMBATRANSFER_H
#define AMBATRANSFER_H

#include "AmbaTransfer_impl.h"


#define TRANSFER_OK (0U)
#define TRANSFER_ERR_0001 (1U)
#define TRANSFER_ERR_0002 (2U)
#define TRANSFER_ERR_0003 (3U)
#define TRANSFER_ERR_0004 (4U)
#define TRANSFER_ERR_0005 (5U)
#define TRANSFER_ERR_0006 (6U)

#define TRANSFER_ERR_INVALID_CONF       TRANSFER_ERR_0001
#define TRANSFER_ERR_INVALID_INFO       TRANSFER_ERR_0002
#define TRANSFER_ERR_INVALID_HANDLER    TRANSFER_ERR_0003
#define TRANSFER_ERR_OUT_OF_MEMORY      TRANSFER_ERR_0004
#define TRANSFER_ERR_CHANNEL_ERROR      TRANSFER_ERR_0005
#define TRANSFER_ERR_IMPL_ERROR         TRANSFER_ERR_0006


#define AMBATRANFER_NAME_MAXLENGTH (32)


typedef struct AMBA_TRANSFER_DEFCONFIG_s_ {
    UINT32 ChAmount;
} AMBA_TRANSFER_DEFCONFIG_s;

typedef struct AMBA_TRANSFER_CHANNEL_CONFIG_s_ {
    char Name[AMBATRANFER_NAME_MAXLENGTH];
    AMBA_TRANSFER_IMPL_s *Impl;
    void *ImplSetting;
} AMBA_TRANSFER_CHANNEL_CONFIG_s;

typedef struct AMBA_TRANSFER_CONNECT_s_ {
    char Name[AMBATRANFER_NAME_MAXLENGTH];
    void *Settings;
} AMBA_TRANSFER_CONNECT_s;

typedef struct AMBA_TRANSFER_DISCONNECT_s_ {
    void *Settings;
} AMBA_TRANSFER_DISCONNECT_s;

typedef struct AMBA_TRANSFER_SEND_s_ {
    UINT32 Size;
    void *PayloadAddr;
    void *Settings;
    void *Result;
} AMBA_TRANSFER_SEND_s;

typedef struct AMBA_TRANSFER_REGISTER_s_ {
    char Name[AMBATRANFER_NAME_MAXLENGTH];
    AmbaTransfer_RecvCBFunc CBFunc;
    void *Settings;
} AMBA_TRANSFER_REGISTER_s;

typedef struct AMBA_TRANSFER_UNREGISTER_s_ {
    void *Settings;
} AMBA_TRANSFER_UNREGISTER_s;

typedef struct AMBA_TRANSFER_RECEIVE_s_ {
    UINT32 Size;
    UINT8 *BufferAddr;
    void *Settings;
    void *Result;
} AMBA_TRANSFER_RECEIVE_s;

typedef struct AMBA_TRANSFER_GETSTATUS_s_ {
    void *Settings;
    void *Result;
} AMBA_TRANSFER_GETSTATUS_s;

enum AMBATRANSFER_ER_CODE_e {
    AMBATRANSFER_OK = 0,
    AMBATRANSFER_ER_GENERAL_ERROR = -1,
    AMBATRANSFER_ER_OUT_OF_MEMORY = -2,
    AMBATRANSFER_ER_NOT_INITED = -3,
};

/**
 * get AmbaTransfer module default config for initializing
 *
 * @param [out] DefConf AmbaTransfer module default config
 *
 * @return 0 - OK, others - AMBATRANSFER_ER_CODE_e
 * @see AMBATRANSFER_ER_CODE_e
 */
UINT32 AmbaTransfer_GetDefConfig(AMBA_TRANSFER_DEFCONFIG_s *DefConf);

/**
 * Initialize AmbaTransfer module
 *
 * @param [in] Conf AmbaTransfer module config
 *
 * @return 0 - OK, others - AMBATRANSFER_ER_CODE_e
 * @see AMBATRANSFER_ER_CODE_e
 */
UINT32 AmbaTransfer_Init(AMBA_TRANSFER_DEFCONFIG_s *Conf);

/**
 * get AmbaTransfer implementation module default config for channel creating
 *
 * @param [out] Conf AmbaTransfer implementation module default config
 *
 * @return 0 - OK, others - AMBATRANSFER_ER_CODE_e
 * @see AMBATRANSFER_ER_CODE_e
 */
UINT32 AmbaTransfer_GetConfig(AMBA_TRANSFER_CHANNEL_CONFIG_s *Conf);

/**
 * Create AmbaTransfer Channel which is linked with specified implementation
 *
 * @param [out] Handler AmbaTransfer Channel Handler
 * @param [in] Conf AmbaTransfer Channel config
 *
 * @return 0 - OK, others - AMBATRANSFER_ER_CODE_e
 * @see AMBATRANSFER_ER_CODE_e
 */
UINT32 AmbaTransfer_Create(INT32 *Handler, AMBA_TRANSFER_CHANNEL_CONFIG_s *Conf);

UINT32 AmbaTransfer_Delete(INT32 Handler, void *Info);
UINT32 AmbaTransfer_Connect(INT32 *Handler, AMBA_TRANSFER_CONNECT_s *Info);
UINT32 AmbaTransfer_Disconnect(INT32 Handler, AMBA_TRANSFER_DISCONNECT_s *Info);
UINT32 AmbaTransfer_Send(INT32 Handler, AMBA_TRANSFER_SEND_s *Info);
UINT32 AmbaTransfer_Register(INT32 *Handler, AMBA_TRANSFER_REGISTER_s *Info);
UINT32 AmbaTransfer_Unregister(INT32 Handler, AMBA_TRANSFER_UNREGISTER_s *Info);
UINT32 AmbaTransfer_Receive(INT32 Handler, AMBA_TRANSFER_RECEIVE_s *Info);
UINT32 AmbaTransfer_GetStatus(INT32 Handler, AMBA_TRANSFER_GETSTATUS_s *Info);

#endif /* AMBATRANSFER_H */

