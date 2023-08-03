/**
 *  @file set_roi.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Set ROI APIs
 *
 */

#include <stdlib.h>
#include "os_api.h"
#include "ambacv_kal.h"
#include "idsp_vis_msg.h"
#include "schdr_util.h"

static int32_t              amba_fd;
static pthread_mutex_t  lock = PTHREAD_MUTEX_INITIALIZER;
static dsp_cmd_t        cmd[32];
static dsp_header_cmd_t *header = (dsp_header_cmd_t*)&cmd[0];

static void setup_to_async(roi_setup_t *setup)
{
    cmd_vproc_img_pyramid_setup_t *pyramid;
    cmd_vproc_lane_det_setup_t    *ld;
    scale_info_t *info;
    int32_t i, mask = 0;

    pyramid = (cmd_vproc_img_pyramid_setup_t *)&cmd[++header->num_cmds];
    thread_memset(pyramid, 0, sizeof(*pyramid));
    pyramid->cmd_code = CMD_VPROC_IMG_PRMD_SETUP;
    pyramid->is_hier_poly_sqrt2 = 1;
    pyramid->roi_tag = setup->roi_tag;

    for (i = 0; i < MAX_HALF_OCTAVES; i++) {
        info = &pyramid->scale_info[i];
        info->roi_width     = setup->half_octave[i].w;
        info->roi_height    = setup->half_octave[i].h;
        info->roi_start_col = setup->half_octave[i].x;
        info->roi_start_row = setup->half_octave[i].y;
        if (*(uint64_t*)info != 0) {
            mask |= (1<<i);
        }
    }
    pyramid->enable_bit_mask = mask;

    ld = (cmd_vproc_lane_det_setup_t*)&cmd[++header->num_cmds];
    thread_memset(ld, 0, sizeof(*ld));
    ld->cmd_code = CMD_VPROC_LN_DET_SETUP;
    ld->pyr_filter_idx   = setup->ld_input_scale;
    ld->ld_roi_start_col = setup->lane_det.x;
    ld->ld_roi_start_row = setup->lane_det.y;
    ld->ld_roi_width     = setup->lane_det.w;
    ld->ld_roi_height    = setup->lane_det.h;
}

int32_t schdr_setup_rois(int32_t count, roi_setup_t *setup)
{
    int32_t i;
    ambacv_asynvis_msg_t arg;

    pthread_mutex_lock(&lock);
    if (amba_fd == 0) {
        amba_fd = open("/dev/ambacv", O_SYNC | O_RDONLY);
        if (amba_fd < 0) {
            printf("Can't open device file /dev/ambacv !!!\n");
            exit(-1);
        } /* if (amba_fd < 0) : open("/dev/ambacv", ...) */
    } /* if (amba_fd == 0) */

    if (count > 15) {
        printf("ROI setup count is too big!\n");
        exit(0);
    }

    header->cmd_code = CMD_DSP_HEADER;
    header->num_cmds = 0;

    for (i = 0; i < count; i++) {
        setup_to_async(&setup[i]);
    }

    thread_memcpy(&arg.cmd, cmd, sizeof(cmd));
    arg.cmd_size = sizeof(cmd);
    ioctl(amba_fd, AMBACV_SEND_ASYNCMSG, &arg);
    pthread_mutex_unlock(&lock);
    return 0;
}
