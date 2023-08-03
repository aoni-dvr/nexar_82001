/*
 * Copyright (c) 2017-2017 Ambarella, Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cvtask_api.h"
#include "cvtask_errno.h"
#include "cvapi_idspfeeder_interface.h"
#include "cvapi_idsp_interface.h"
#include "private.h"

#define CACHELINE_ROUNDUP(x)    (((x) + 63) & ~63)
#define GYRO_SID                0


#ifdef CHIP_CV6

typedef struct { /* sideband_info_s */
    uint32_t                fov_id;
    uint32_t                fov_private_info_addr;
} sideband_info_old_t;

typedef struct { /* image_buffer_desc_s */
    uint32_t                buffer_addr;
    uint16_t                buffer_pitch;
    uint16_t                img_width;
    uint16_t                img_height;
    uint16_t                reserved_0;
} image_buffer_desc_old_t;

typedef struct { /* msg_vp_hier_out_lane_out_header_s */
    uint16_t                channel_id;
    uint16_t                reserved;
    uint32_t                cap_seq_no;
    uint32_t                batch_cmd_id;
    sideband_info_old_t     side_band_info[MAX_TOKEN_ARRAY];
    uint32_t                roi_cmd_tag[MAX_ROI_CMD_TAG_NUM];
    uint16_t                ln_det_hier_mask;
    uint16_t                is_hier_out_tile_mode;
    uint32_t                vin_cap_time;
    uint32_t                proc_roi_cmd_time;
    uint32_t                c2y_done_time;
    uint32_t                warp_mctf_done_time;
} idsp_vis_picinfo_hdr_old_t;

typedef struct { /* msg_vp_hier_out_lane_out_s */
    idsp_vis_picinfo_hdr_old_t  header;
    image_buffer_desc_old_t     luma_hier_outs[MAX_HALF_OCTAVES];
    image_buffer_desc_old_t     chroma_hier_outs[MAX_HALF_OCTAVES];
    image_offset_t              hier_out_offsets[MAX_HALF_OCTAVES];
    image_buffer_desc_old_t     luma_lane_det_out;
    image_buffer_desc_old_t     chroma_lane_det_out;
    image_offset_t              lane_det_out_offset;
    uint32_t                    padding[5]; // Align to 128b boundary
    uint32_t                    reserved_for_epi_daddr;   /* Reserved for extended pic info */
} idsp_vis_picinfo_old_t;

#endif /* ?CHIP_CV6 */

static void load_frame(struct fov_info *fov, const char *file_name,
	idsp_vis_picinfo_t *info)
{
	FILE *ifp;
	uint32_t i, fsize, base;
#ifdef CHIP_CV6
    idsp_vis_picinfo_old_t old_picinfo;
#endif /* ?CHIP_CV6 */

	/* check if we can access the file */
	ifp = fopen(file_name, "rb");
	if (ifp == NULL) {
		printf("IDspFeeder: can't open input file %s\n", file_name);
		exit(-1);
	}

	/* check if the file is too big */
	fseek(ifp, 0, SEEK_END);
	fsize = ftell(ifp) - 1024;
	if (fsize > fov->frame_max_length) {
		printf("IDspFeeder: File %s is too big, size=%d max=%d!\n",
		       file_name, fsize, fov->frame_max_length);
		exit(-1);
	}

	/* read in the picinfo header */
	fseek(ifp, 0, SEEK_SET);

#ifdef CHIP_CV6
	fread(&old_picinfo, sizeof(idsp_vis_picinfo_old_t), 1, ifp);
	{ // Repatch old -> new picinfo (CV6)
		uint32_t loop;

		// repatch header
		info->header.channel_id     = old_picinfo.header.channel_id;
		info->header.reserved       = old_picinfo.header.reserved;
		info->header.cap_seq_no     = old_picinfo.header.cap_seq_no;
		info->header.batch_cmd_id   = old_picinfo.header.batch_cmd_id;

		for (loop = 0; loop < MAX_TOKEN_ARRAY; loop++)
		{
			info->header.side_band_info[loop].fov_id    = old_picinfo.header.side_band_info[loop].fov_id;
			info->header.side_band_info[loop].fov_private_info_addr = old_picinfo.header.side_band_info[loop].fov_private_info_addr;
			info->header.side_band_info[loop].fov_private_info_addr_upper.g_seg_id    = 0;  // [TODO][IPC-GA]
			info->header.side_band_info[loop].fov_private_info_addr_upper.rsvd0       = 0;  // [TODO][IPC-GA]
			info->header.side_band_info[loop].fov_private_info_addr_upper.rsvd1       = 0;  // [TODO][IPC-GA]
			info->header.side_band_info[loop].fov_private_info_addr_upper.segofs_msb8 = 0;  // [TODO][IPC-GA]
		}

		for (loop = 0; loop < MAX_ROI_CMD_TAG_NUM; loop++)
		{
			info->header.roi_cmd_tag[loop]    = old_picinfo.header.roi_cmd_tag[loop];
		}

		info->header.ln_det_hier_mask       = old_picinfo.header.ln_det_hier_mask;
		info->header.is_hier_out_tile_mode  = old_picinfo.header.is_hier_out_tile_mode;
		info->header.vin_cap_time           = old_picinfo.header.vin_cap_time;
		info->header.proc_roi_cmd_time      = old_picinfo.header.proc_roi_cmd_time;
		info->header.c2y_done_time          = old_picinfo.header.c2y_done_time;
		info->header.warp_mctf_done_time    = old_picinfo.header.warp_mctf_done_time;

		for (loop = 0; loop < MAX_HALF_OCTAVES; loop++)
		{
			info->luma_hier_outs[loop].buffer_addr    = old_picinfo.luma_hier_outs[loop].buffer_addr;
			info->luma_hier_outs[loop].buffer_pitch   = old_picinfo.luma_hier_outs[loop].buffer_pitch;
			info->luma_hier_outs[loop].buffer_addr_upper.g_seg_id     = 0x00; // [TODO][IPC-GA]
			info->luma_hier_outs[loop].buffer_addr_upper.rsvd0        = 0x00; // [TODO][IPC-GA]
			info->luma_hier_outs[loop].buffer_addr_upper.rsvd1        = 0x00; // [TODO][IPC-GA]
			info->luma_hier_outs[loop].buffer_addr_upper.segofs_msb8  = 0x00; // [TODO][IPC-GA]
			info->luma_hier_outs[loop].img_width      = old_picinfo.luma_hier_outs[loop].img_width;
			info->luma_hier_outs[loop].img_height     = old_picinfo.luma_hier_outs[loop].img_height;
			info->luma_hier_outs[loop].reserved_0     = old_picinfo.luma_hier_outs[loop].reserved_0;
			info->chroma_hier_outs[loop].buffer_addr  = old_picinfo.chroma_hier_outs[loop].buffer_addr;
			info->chroma_hier_outs[loop].buffer_pitch = old_picinfo.chroma_hier_outs[loop].buffer_pitch;
			info->chroma_hier_outs[loop].buffer_addr_upper.g_seg_id     = 0x00; // [TODO][IPC-GA]
			info->chroma_hier_outs[loop].buffer_addr_upper.rsvd0        = 0x00; // [TODO][IPC-GA]
			info->chroma_hier_outs[loop].buffer_addr_upper.rsvd1        = 0x00; // [TODO][IPC-GA]
			info->chroma_hier_outs[loop].buffer_addr_upper.segofs_msb8  = 0x00; // [TODO][IPC-GA]
			info->chroma_hier_outs[loop].img_width    = old_picinfo.chroma_hier_outs[loop].img_width;
			info->chroma_hier_outs[loop].img_height   = old_picinfo.chroma_hier_outs[loop].img_height;
			info->chroma_hier_outs[loop].reserved_0   = old_picinfo.chroma_hier_outs[loop].reserved_0;
			info->hier_out_offsets[loop]              = old_picinfo.hier_out_offsets[loop];
		}
		info->luma_lane_det_out.buffer_addr     = old_picinfo.luma_lane_det_out.buffer_addr;
		info->luma_lane_det_out.buffer_pitch    = old_picinfo.luma_lane_det_out.buffer_pitch;
		info->luma_lane_det_out.buffer_addr_upper.g_seg_id     = 0x00; // [TODO][IPC-GA]
		info->luma_lane_det_out.buffer_addr_upper.rsvd0        = 0x00; // [TODO][IPC-GA]
		info->luma_lane_det_out.buffer_addr_upper.rsvd1        = 0x00; // [TODO][IPC-GA]
		info->luma_lane_det_out.buffer_addr_upper.segofs_msb8  = 0x00; // [TODO][IPC-GA]
		info->luma_lane_det_out.img_width       = old_picinfo.luma_lane_det_out.img_width;
		info->luma_lane_det_out.img_height      = old_picinfo.luma_lane_det_out.img_height;
		info->luma_lane_det_out.reserved_0      = old_picinfo.luma_lane_det_out.reserved_0;
		info->chroma_lane_det_out.buffer_addr   = old_picinfo.chroma_lane_det_out.buffer_addr;
		info->chroma_lane_det_out.buffer_pitch  = old_picinfo.chroma_lane_det_out.buffer_pitch;
		info->chroma_lane_det_out.buffer_addr_upper.g_seg_id     = 0x00; // [TODO][IPC-GA]
		info->chroma_lane_det_out.buffer_addr_upper.rsvd0        = 0x00; // [TODO][IPC-GA]
		info->chroma_lane_det_out.buffer_addr_upper.rsvd1        = 0x00; // [TODO][IPC-GA]
		info->chroma_lane_det_out.buffer_addr_upper.segofs_msb8  = 0x00; // [TODO][IPC-GA]
		info->chroma_lane_det_out.img_width     = old_picinfo.chroma_lane_det_out.img_width;
		info->chroma_lane_det_out.img_height    = old_picinfo.chroma_lane_det_out.img_height;
		info->chroma_lane_det_out.reserved_0    = old_picinfo.chroma_lane_det_out.reserved_0;
		info->lane_det_out_offset               = old_picinfo.lane_det_out_offset;
	}
#else /* !CHIP_CV6 */
	fread(info, sizeof(*info), 1, ifp);
#endif /* ?CHIP_CV6 */

	/* read in the hierarchical frame data */
	fseek(ifp, 1024, SEEK_SET);
	fread(fov->frame_base, 1, fsize, ifp);
	fclose(ifp);

	fsize = CACHELINE_ROUNDUP(fsize);
	ambacv_cache_clean(fov->frame_base, fsize);
	fov->frame_wpos = fov->frame_base + fsize;

	/* adjust offsets */
	base = ambacv_v2p(fov->frame_base);
	for (i = 0; i < MAX_HALF_OCTAVES; i++) {
		info->luma_hier_outs[i].buffer_addr += base;
		info->chroma_hier_outs[i].buffer_addr += base;
	}
	info->luma_lane_det_out.buffer_addr += base;
	info->chroma_lane_det_out.buffer_addr += base;
	info->header.channel_id = fov->fov_id;
}

static void load_gyrod(struct fov_info *fov, const char *path,
	sideband_info_t *sideband)
{
	FILE *ifp;
	int used, gsize, fsize;

	gsize = CACHELINE_ROUNDUP(sizeof(idsp_gyro_data_t) + sizeof(uint32_t) + sizeof(uint32_t));
	used = fov->frame_wpos - fov->frame_base;

	if(fov->frame_max_length < (gsize + used))
	{
		printf("IDspFeeder: Not enough space for gyro data");
		exit(-1);
	}

	/* check if we can access the file */
	ifp = fopen(path, "rb");
	if (ifp == NULL) {
		printf("IDspFeeder: can't open gyro data file %s\n", path);
		exit(-1);
	}

	/* check if the file is right */
	fseek(ifp, 0, SEEK_END);
	fsize = ftell(ifp);
	if (fsize > sizeof(idsp_gyro_data_t)) {
		printf("IDspFeeder: gyro file %s is too big!\n", path);
		exit(-1);
	} else if (fsize < sizeof(idsp_gyro_data_t)) {
		printf("IDspFeeder: gyro data size %d is too small!\n", fsize);
		memset(fov->frame_wpos, 0, sizeof(idsp_gyro_data_t));
	}

    {
      uint32_t *recast32 = (uint32_t *)fov->frame_wpos;
      recast32[0] = METADATA_TYPE_GYRO_DATA;
      recast32[1] = sizeof(idsp_gyro_data_t) + (sizeof(uint32_t) * 2);
    }

	fseek(ifp, 0, SEEK_SET);
	fread(fov->frame_wpos + ((sizeof(uint32_t)*2)), fsize, 1, ifp);
	fclose(ifp);

	sideband->fov_id = 1;
	sideband->fov_private_info_addr = ambacv_v2p(fov->frame_wpos);
	ambacv_cache_clean(fov->frame_wpos, gsize);
	fov->frame_wpos += gsize;
}

static void config_pyramid(struct fov_info *fov, idsp_pyramid_t *pyramid,
	idsp_vis_picinfo_t *info)
{
	int i;

	for (i = 0; i < MAX_HALF_OCTAVES; i++) {
		if (pyramid->half_octave[i].ctrl.disable) {
			info->header.ln_det_hier_mask &= ~(1 << i);
			continue;
		}

		info->luma_hier_outs[i].buffer_addr +=
			(pyramid->half_octave[i].roi_start_row) *
			(info->luma_hier_outs[i].buffer_pitch);
		info->luma_hier_outs[i].buffer_addr +=
			(pyramid->half_octave[i].roi_start_col);
		info->chroma_hier_outs[i].buffer_addr +=
			(pyramid->half_octave[i].roi_start_row >> 1) *
			(info->luma_hier_outs[i].buffer_pitch);
		info->chroma_hier_outs[i].buffer_addr += 0xFFFFFFFE &
			(pyramid->half_octave[i].roi_start_col + 1);

		info->luma_hier_outs[i].img_width =
			pyramid->half_octave[i].roi_width_m1 + 1;
		info->luma_hier_outs[i].img_height =
			pyramid->half_octave[i].roi_height_m1 + 1;
		info->chroma_hier_outs[i].img_width =
			info->luma_hier_outs[i].img_width;
		info->chroma_hier_outs[i].img_height =
			(info->luma_hier_outs[i].img_height + 1) >> 1;
		info->hier_out_offsets[i].x_offset =
			pyramid->half_octave[i].roi_start_col;
		info->hier_out_offsets[i].y_offset =
			pyramid->half_octave[i].roi_start_row;
	}
}

static void load_fov(struct priv_data *pd, struct fov_info *fov,
		     const char *channel_name)
{
	idsp_vis_picinfo_t *picinfo;
	uint32_t findex, file_num;
	char name[128];

	findex = pd->frame_load % fov->frame_buffer_entries;
	picinfo = &(fov->frame_info[findex].picinfo);
	memset(picinfo, 0, sizeof(*picinfo));
	fov->frame_base = fov->frame_info[findex].frame_base;

	file_num = (pd->frame_load % pd->num_frames) + pd->frame_start;
	fov->frame_info[findex].file_num = file_num;

	/* load frame data */
	sprintf(name, pd->frame_format, file_num, channel_name);
	load_frame(fov, name, picinfo);
	//printf("                                                       ");
	//printf("IdspFeeder%d load frame=%d(%d), fov=%d\n", pd->instance_id,
	//       pd->frame_load, file_num, fov->fov_id);

	/* load gyro data */
	if(pd->is_gyro_disabled == 0)
	{
		if (pd->gyrod_format) {
			sprintf(name, pd->gyrod_format, file_num);
			load_gyrod(fov, name,
				   &picinfo->header.side_band_info[GYRO_SID]);
		}
	}
	else
	{
		printf("Skipping Gyro Data Load\n");
	}
}

static void send_fov(struct priv_data *pd, struct fov_info *fov)
{
	uint32_t findex = pd->frame_send % fov->frame_buffer_entries;
	struct frame_info *frame_info = &fov->frame_info[findex];
	idsp_vis_picinfo_t picinfo;

	memcpy(&picinfo, &(frame_info->picinfo), sizeof(picinfo));

	/* apply ROIs as needed */
	if (pd->pyramid_valid != 0) {
		config_pyramid(fov, &pd->pyramid, &picinfo);
	}

	/* apply Lane-Dection as needed */
	if (pd->lanedetect_valid != 0) {
		/* TODO */
	}

	/* send pic to VPMSG queue */
	picinfo.header.cap_seq_no   = pd->cap_seq_no;
	picinfo.header.vin_cap_time = cvtask_get_timestamp();
	//printf("IdspFeeder%d send frame=%d(%d), fov=%d\n", pd->instance_id,
	//       pd->cap_seq_no, frame_info->file_num, picinfo.header.channel_id);
	schdrmsg_send_vpmsg(&picinfo);
}

void load_frame_set(struct priv_data *pd)
{
	if (pd->num_fov == 2)
	{
		load_fov(pd, &pd->fov[0], "left");
		load_fov(pd, &pd->fov[1], "right");
	}
	else
	{
		load_fov(pd, &pd->fov[0], "mono");
	}
	pd->frame_load++;
}

void send_frame_set(struct priv_data *pd)
{
	uint32_t i;

	if (pd->frame_send + 1 < pd->frame_load) {
		pd->frame_send++;
	}

	for (i = 0; i < pd->num_fov; i++) {
		send_fov(pd, &pd->fov[i]);
	}
	pd->cap_seq_no++;
}
