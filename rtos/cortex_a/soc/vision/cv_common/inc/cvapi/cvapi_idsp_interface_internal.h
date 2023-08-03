/*
* Copyright (c) 2017-2017 Ambarella, Inc.
*
* This file and its contents ("Software") are protected by intellectual property rights including,
* without limitation, U.S. and/or foreign copyrights.  This Software is also the confidential and
* proprietary information of Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
* distribute, modify, or otherwise prepare derivative works of this Software or any portion thereof
* except pursuant to a signed license agreement or nondisclosure agreement with Ambarella, Inc. or
* its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
* return this Software to Ambarella, Inc.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CVAPI_IDSP_INTERFACE_INTERNAL_H_FILE
#define CVAPI_IDSP_INTERFACE_INTERNAL_H_FILE

#include <cvapi_idsp_interface.h>

#define IDSP_MESSAGE_PYRAMID_CONFIG       0x10240000
#define IDSP_MESSAGE_LANE_DETECT_CONFIG   0x10240001
#define IDSPRX_MESSAGE_NEW_PICINFO        0x1024f000

typedef struct { /* idsp_pyramid_config_message_s */
    uint32_t        message_type;
    idsp_pyramid_t  pyramid;
} idsp_pyramid_config_message_t;

typedef struct { /* idsp_lane_detect_config_message_s */
    uint32_t        message_type;
    idsp_ld_info_t  ld_info;
} idsp_lane_detect_config_message_t;

typedef struct { /* idsp_rx_new_picinfo_s */
    uint32_t        message_type;
    void           *vpRightIDspPicInfo;   /* idsp_vis_picinfo_t */
    void           *vpLeftIDspPicInfo;    /* idsp_vis_picinfo_t */
} idsp_rx_new_picinfo_t;

#endif  /* !CVAPI_IDSP_INTERFACE_INTERNAL_H_FILE */

