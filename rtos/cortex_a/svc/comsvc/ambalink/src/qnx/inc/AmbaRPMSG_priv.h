/**
 * @file AmbaRPMSG_priv.h
 *
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
 *
 * @details Locally private definition
 */

#ifndef AMBARPMSG_PRIV_H
#define AMBARPMSG_PRIV_H

#include <stdint.h>

#include <devctl.h>

#include <AmbaTypes.h>
#include <AmbaIPC.h>

#define AMBA_RPMSG_MAGIC     (0x61626d61u) /*amba*/
#define DCMD_RPMSG_ALLOC     (0x00636c61u) /* alc*/
#define DCMD_RPMSG_REG       (0x00676572u) /* reg*/
#define DCMD_RPMSG_SEND      (0x00646e73u) /* snd*/
#define DCMD_RPMSG_RECV      (0x00766372u) /* rcv*/

/* For AmbaIPC_Alloc() */
#define RPMSG_NAME_SIZE     (32)
typedef struct {
    char name[RPMSG_NAME_SIZE];
} amba_rpmsg_call_alloc_t;

/* For AmbaIPC_RegisterChannel() */
typedef struct {
    AMBA_IPC_HANDLE handle;
} amba_rpmsg_call_reg_t;

/* For AmbaIPC_Send() */
#if defined(CONFIG_AMBALINK_RPMSG_SIZE)
#define AMBA_RPMSG_DATA_SIZE (CONFIG_AMBALINK_RPMSG_SIZE)
#elif defined(CONFIG_AMBALINK_G2_RPMSG_SIZE)
#define AMBA_RPMSG_DATA_SIZE (CONFIG_AMBALINK_G2_RPMSG_SIZE)
#else
#define AMBA_RPMSG_DATA_SIZE (2048u)
#endif
typedef struct {
    AMBA_IPC_HANDLE handle;
    int32_t Length;
    uint8_t Data[AMBA_RPMSG_DATA_SIZE];
} amba_rpmsg_call_send_t;

typedef struct {
    uint32_t    magic;
    uint32_t    cmd;            /**< AMBA_RPMSG_XXX */
    uint32_t    len;            /**< Length of param */
    uint32_t    pad;
} amba_rpmsg_pkt_hdr_t;

typedef struct {
    amba_rpmsg_pkt_hdr_t        hdr;
    uint8_t                     param[CONFIG_AMBALINK_G2_RPMSG_SIZE]; /**< amba_rpmsg_call_xxx_t */
} amba_rpmsg_pkt_t;

typedef struct {
    amba_rpmsg_pkt_hdr_t        hdr;
    int32_t                     err;
} amba_rpmsg_call_rsp_t;

typedef struct {
    amba_rpmsg_pkt_hdr_t        hdr;
    int32_t                     err;
    AMBA_IPC_HANDLE             handle;
} amba_rpmsg_call_alloc_rsp_t;

typedef struct {
    AMBA_IPC_HANDLE handle;
    int32_t cbfd;
    AMBA_IPC_MSG_HANDLER_f call_back;
} amba_rpmsg_cq_t;

#endif // AMBARPMSG_PRIV_H
