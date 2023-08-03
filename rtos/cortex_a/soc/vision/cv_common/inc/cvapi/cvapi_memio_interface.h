/*
 * Copyright (c) 2017-2018 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CVAPI_MEMIO_INTERFACE_H
#define CVAPI_MEMIO_INTERFACE_H

#include "ambint.h"
#include "cvapi_idsp_interface.h"

#define TRANSFER_CHANNEL_MEMIO "TRANS_MEMIO"

/**
 * Memio CVTasks config:
 *
 * data_source:
 *     config[0]: mode
 *     config[1]: sub-channel
 *     config[2]: label name index of cvtask instance
 *     config[3]: num of output node if mode = 2
 *
 * data_sink:
 *     config[0]: num_inputs
 *     config[1]: sub-channel
 *     config[2]: label name index of cvtask instance
 *
 * data_trigger:
 *     config[0]: input node label name index
 *     config[1]: uuid of cvtask to be triggered
 *     config[2]: label name index of cvtask instance
 *
 *
 */

/**
 * @SubChannel:     A sub-channel in memio AmbaTransfer impelmentation/channel support bi-direction data transfer.
 *                  SubChannle is an index used to identify which communication instance used in AmbaTransfer implementation.
 * @Owner:          Onwer is to identify who call the AmbaTransfer API in CVTask or APP.
 *
 */
#define MEMIO_OWNER_CVTASK                  0U
#define MEMIO_OWNER_APP                     1U

#define MEMIO_SUBCHAIN_DATASOURCE_0         0U
#define MEMIO_SUBCHAIN_DATASINK_0           1U
#define MEMIO_SUBCHAIN_DATASOURCE_1         2U
#define MEMIO_SUBCHAIN_DATASINK_1           3U
#define MEMIO_SUBCHAIN_DATASOURCE_2         4U
#define MEMIO_SUBCHAIN_DATASINK_2           5U
#define MEMIO_SUBCHAIN_DATASOURCE_3         6U
#define MEMIO_SUBCHAIN_DATASINK_3           7U

#define TRANS_MSGQ_MAX                      (MEMIO_SUBCHAIN_DATASINK_3 + 1U)


#define MEMIO_ALIGN_SIZE            64U
#define MEMIO_ALIGN_SIZE_MASK       (~(MEMIO_ALIGN_SIZE - 1U))
//#define MEMIO_ALIGN_PAD_SIZE(x) ((((x) + (MEMIO_ALIGN_SIZE - 1U))&(MEMIO_ALIGN_SIZE_MASK)) - (x))


typedef struct {
    uint32_t              SubChannel;
    uint32_t              Owner;
} memio_setting_t;

/**
 * Memio source cvtask send data input request to App task.
 * The msg can be a seq_no here.
 *
 * AmbaTransfer data direction: data_source cvtask -> App Task
 */
typedef struct {
    uint32_t seq_no;
    uint32_t cvtask_frameset_id;
} memio_source_send_req_t;

/**
 * Memio source cvtask receive ack from App task. App provides a msg data chunk to cvtask.
 * The msg can be either,
 *      1.) 4 bytes address of a raw buffer
 *      2.) cv_pic_info_t (same as idsprx cvtask output)
 *
 * AmbaTransfer data direction: App Task -> data_source cvtask
 */
#define MMIO_SOURCE_REVC_RAW 0xFACEBABEU
typedef struct {
    uint32_t magic;
    uint32_t addr;
    uint32_t size;
    uint32_t pitch;
    uint32_t rsv;
    uint32_t batch_cnt;
    uint8_t  pad[(((sizeof(uint32_t) * 6U) + (MEMIO_ALIGN_SIZE - 1U))&(MEMIO_ALIGN_SIZE_MASK)) - (sizeof(uint32_t) * 6U)];
} memio_source_recv_raw_t;

typedef struct {
    cv_pic_info_t   pic_info;
    uint8_t  pad[(((sizeof(cv_pic_info_t)) + (MEMIO_ALIGN_SIZE - 1U))&(MEMIO_ALIGN_SIZE_MASK)) - sizeof(cv_pic_info_t)];
} memio_source_recv_picinfo_t;

/**
 * Memio sink cvtask send data output to App task. CVTask provides a msg data chunk to App task.
 * The msg can be either,
 *
 * AmbaTransfer data direction: data_sink cvtask -> App Task
 */
typedef struct {
    uint32_t  addr;
    uint32_t  size;
} memio_desc_t;


#define MEMIO_OUTPUT_TYPE_RAW   0
#define MEMIO_OUTPUT_TYPE_OSD   1
typedef struct {
    uint32_t        type;
    uint32_t        cvtask_frameset_id;
    uint32_t        num_of_io;
    memio_desc_t    io[8];
} memio_sink_send_out_t;


/**
 * Memio sink cvtask receive ack from App task. App provides a ack to cvtask.
 * The msg can be a seq_no here.
 *
 * AmbaTransfer data direction: App Task -> data_source cvtask
 */
typedef struct {
    uint32_t seq_no;
} memio_sink_recv_act_t;

#endif /* !CVAPI_MEMIO_INTERFACE_H */

