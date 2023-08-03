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

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cvtask_api.h"
#include "cvtask_errno.h"
#include "cvtask_ossrv.h"
#include "cvapi_idsp_interface.h"
#include "cvapi_idsp_analyzer_interface.h"
#include "build_version.h"

#define TASKNAME        TASKNAME_IDSP_ANALYZER
#define INFO_ONLY_MODE  0

#define CMSG(...)       CVTASK_PRINTF(LVL_CRITICAL, __VA_ARGS__)
#define IMSG(...)       CVTASK_PRINTF(LVL_NORMAL,   __VA_ARGS__)
#define DMSG(...)       CVTASK_PRINTF(LVL_DEBUG,    __VA_ARGS__)
#define VMSG(...)       CVTASK_PRINTF(LVL_VERBOSE,  __VA_ARGS__)

#define show_addr(name, addr)                                                   \
	do {                                                                    \
		if (pd->mode == INFO_ONLY_MODE) {                               \
			IMSG("        " name "=0x%08X",  addr);                 \
		} else {                                                        \
			uint32_t p = pd->base + (int)addr;                      \
			IMSG("        " name "=0x%08X, phys=0x%08X",  addr, p); \
		}                                                               \
	} while (0)


struct priv_data {
	int mode;
	int stereo_dump;
	int format;
	int base;
	int frame_start;
	int frame_curr;
	int frame_end;
};

struct image_descriptor {
	int w;
	int h;
	int p;
	const char *y;
	const char *uv;
};

static void show_pic_info(cv_pic_info_t *pic,  struct priv_data *pd)
{
	half_octave_info_t *info;
	int i;

	pd->base = (int)ambacv_v2p(pic);
	DMSG("[" TASKNAME "@%d]", pic->frame_num);
	DMSG("    image_height: %d", pic->pyramid.image_height_m1+ 1);
	DMSG("    image_width : %d", pic->pyramid.image_width_m1 + 1);
	DMSG("    image_pitch : %d", pic->pyramid.image_pitch_m1 + 1);

	for (i = 0; i < MAX_HALF_OCTAVES; i++) {
		info = &(pic->pyramid.half_octave[i]);
		if (info->ctrl.disable == 1)
			continue;
		DMSG("    scale[%02d]: width=%d, height=%d, pitch=%d", i,
			info->roi_width_m1 + 1,
			info->roi_height_m1 + 1,
			info->ctrl.roi_pitch);
		if (info->ctrl.mode) {
			show_addr("  rpLumaL", pic->rpLumaLeft[i]);
			show_addr("rpChromaL", pic->rpChromaLeft[i]);
			show_addr("  rpLumaR", pic->rpLumaRight[i]);
			show_addr("rpChromaR", pic->rpChromaRight[i]);
		} else {
			show_addr("   rpLuma", pic->rpLumaRight[i]);
			show_addr(" rpChroma", pic->rpChromaRight[i]);
		}
	}

	DMSG("     land-det: width=%d, height=%d, pitch=%d",
		pic->lane_detect_info.ld_roi_width_m1 + 1,
		pic->lane_detect_info.ld_roi_height_m1 + 1,
		pic->lane_detect_info.ld_roi_pitch);
	show_addr("   rpLuma", pic->rpLaneDetectLuma);
	show_addr(" rpChroma", pic->rpLaneDetectChroma);
	DMSG(" ");
}

static void output_open(const char *name, FILE **ofp)
{
	*ofp = fopen(name, "wb");
	if (*ofp == NULL) {
		AmbaPrint("    Can't open output file %s", name);
		exit(-1);
	}
	AmbaPrint("    Saving image %s", name);
}

static void output_pgm_header(FILE *ofp, int w, int h)
{
	char header[32];

	sprintf(header, "P5\n%d %d\n255\n", w, h);
	fwrite(header, 1, strlen(header), ofp);
}

static void output_data(FILE *ofp, const char *buff,
		     int width, int height, int pitch)
{
	while (height--) {
		fwrite(buff, 1, width, ofp);
		buff += pitch;
	}
}

static void save_image(int format, char *name, struct image_descriptor *dsptr)
{
	FILE *ofp;
	int nlen = strlen(name);

	switch(format) {
	case IDSPA_FORMAT_Y_PGM:
		output_open(strcat(name, ".pgm"), &ofp);
		output_pgm_header(ofp, dsptr->w, dsptr->h);
		output_data(ofp, dsptr->y, dsptr->w, dsptr->h, dsptr->p);
		fclose(ofp);
		break;
	case IDSPA_FORMAT_Y_UV_BIN:
		output_open(strcat(name, ".y"), &ofp);
		output_data(ofp, dsptr->y, dsptr->w, dsptr->h, dsptr->p);
		fclose(ofp);
		name[nlen] = 0;
		output_open(strcat(name, ".uv"), &ofp);
		output_data(ofp, dsptr->uv, dsptr->w, (dsptr->h+1)>>1, dsptr->p);
		fclose(ofp);
		break;
	case IDSPA_FORMAT_YUV_BIN:
		output_open(strcat(name, "_nv12.yuv"), &ofp);
		output_data(ofp, dsptr->y, dsptr->w, dsptr->h, dsptr->p);
		output_data(ofp, dsptr->uv, dsptr->w, (dsptr->h+1)>>1, dsptr->p);
		fclose(ofp);
		break;
	}
}

static void save_pic_data(cv_pic_info_t *picinfo, struct priv_data *pd)
{
	struct image_descriptor dsptr;
	half_octave_info_t *info;
	char name[128], *base = (char*)picinfo;
	int frame_no = pd->frame_curr;
	int cid = picinfo->channel_id;
	int format = pd->format;
	int i;

	if (frame_no <  pd->frame_start || frame_no >= pd->frame_end)
		return;

	show_pic_info(picinfo, pd);
	for (i = 0; i < MAX_HALF_OCTAVES; i++) {
		info = &(picinfo->pyramid.half_octave[i]);

		// check if we're configed to dump this scale
		if ((pd->mode & (1<<i)) == 0) {
			continue;
		}

		// check if this scale is enabled
		if (info->ctrl.disable == 1) {
			CMSG("    Skip disabled scale %d!!", i);
			continue;
		}

		dsptr.w = info->roi_width_m1 + 1;
		dsptr.h = info->roi_height_m1 + 1;
		dsptr.p = info->ctrl.roi_pitch;

		if (info->ctrl.mode) {
			sprintf(name, "frame%06d_fov%d_left_scale%d_%dx%d",
				frame_no, cid, i, dsptr.w, dsptr.h);
			dsptr.y  = base + picinfo->rpLumaLeft[i];
			dsptr.uv = base + picinfo->rpChromaLeft[i];
			if (pd->stereo_dump & IDSP_DUMP_LEFT) {
				save_image(format, name, &dsptr);
			}

			sprintf(name, "frame%06d_fov%d_right_scale%d_%dx%d",
				frame_no, cid, i, dsptr.w, dsptr.h);
			dsptr.y  = base + picinfo->rpLumaRight[i];
			dsptr.uv = base + picinfo->rpChromaRight[i];
			if (pd->stereo_dump & IDSP_DUMP_RIGHT) {
				save_image(format, name, &dsptr);
			}
		} else {
			sprintf(name, "frame%06d_fov%d_mono_scale%d_%dx%d",
				frame_no, cid, i, dsptr.w, dsptr.h);
			dsptr.y  = base + picinfo->rpLumaRight[i];
			dsptr.uv = base + picinfo->rpChromaRight[i];
			save_image(format, name, &dsptr);
		}
	}

	/* output lane-detection image */
	if ((pd->mode & (1<<MAX_HALF_OCTAVES)) != 0) {
		dsptr.w = picinfo->lane_detect_info.ld_roi_width_m1 + 1;
		dsptr.h = picinfo->lane_detect_info.ld_roi_height_m1 + 1;
		dsptr.p = picinfo->lane_detect_info.ld_roi_pitch;
		dsptr.y = base + picinfo->rpLaneDetectLuma;
		dsptr.uv= base + picinfo->rpLaneDetectChroma;

		sprintf(name, "frame%06d_fov%d_lanedet_%dx%d",
			frame_no, cid, dsptr.w, dsptr.h);
		save_image(format, name, &dsptr);
	}
}

static uint32_t get_mode(const uint32_t *config)
{
	uint32_t mode = config[0];
	const char *ptr;

	if (!cvtable_find("IDSP_ANALYZER_MODE", (const void **)&ptr, NULL)) {
		mode = strtoul(ptr, NULL, 0);
	}

	return mode;
}

static uint32_t idsp_analyzer_query(uint32_t iid, const uint32_t *config,
	cvtask_memory_interface_t *dst)
{
	CMSG("query cvtask [" TASKNAME "]");

	memset(dst, 0, sizeof(*dst));
	dst->Instance_private_storage_needed  = sizeof(struct priv_data);

	dst->num_outputs = 1;
	strcpy(dst->output[0].io_name, "IDSP_ANALYZER_FINISH");
	dst->output[0].buffer_size = 4;

	dst->num_inputs = 1;
	if (get_mode(config) == INFO_ONLY_MODE) {
		strcpy(dst->input[0].io_name, "IDSP_CVPICINFO_ONLY");
	} else {
		strcpy(dst->input[0].io_name, "IDSP_PICINFO");
	}

	return CVTASK_ERR_OK;
}

static uint32_t idsp_analyzer_init(const cvtask_parameter_interface_t *e,
	const uint32_t *config)
{
	struct priv_data *pd = (struct priv_data*)e->vpInstance_private_storage;

	CMSG("init cvtask [" TASKNAME "]");

	pd->mode = get_mode(config);
	if (pd->mode != INFO_ONLY_MODE) {
		pd->frame_start = config[1];
		pd->frame_end = config[2];
	}
	pd->format = config[3];

	pd->stereo_dump = config[4];
	if (pd->stereo_dump == 0) {
		pd->stereo_dump = IDSP_DUMP_LEFT | IDSP_DUMP_RIGHT;
	}

	return CVTASK_ERR_OK;
}

static uint32_t idsp_analyzer_run(const cvtask_parameter_interface_t *e)
{
	struct priv_data *pd = (struct priv_data*)e->vpInstance_private_storage;
	cv_pic_info_t *picinfo;

	IMSG("[IDSP_ANA %d]", e->cvtask_frameset_id);

	pd->frame_curr = e->cvtask_frameset_id;
	picinfo = (cv_pic_info_t*)e->vpInputBuffer[0];

	if (pd->mode == INFO_ONLY_MODE) {
		show_pic_info(picinfo, pd);
	} else {
		save_pic_data(picinfo, pd);
	}

	*(uint32_t*)e->vpOutputBuffer[0] = e->cvtask_frameset_id;
	return CVTASK_ERR_OK;
}

static uint32_t idsp_analyzer_get_info(
	const cvtask_parameter_interface_t *env,
	uint32_t info_id, void *info_data)
{
	uint32_t err;
	const char **str;

	switch (info_id) {
	case CVTASK_INFO_ID_GET_SELF_INFO:
		str = (const char **)info_data;
		*str = LIBRARY_VERSION;
		err = CVTASK_ERR_OK;
		break;
	default:
		err = CVTASK_ERR_UNIMPLEMENTED;
	}

	return err;
}

static const cvtask_entry_t entry = {
	.cvtask_name     = TASKNAME,
	.cvtask_type     = CVTASK_TYPE_ARM,
	.cvtask_query    = idsp_analyzer_query,
	.cvtask_init     = idsp_analyzer_init,
	.cvtask_run      = idsp_analyzer_run,
	.cvtask_get_info = idsp_analyzer_get_info,
};

uint32_t idsp_analyzer_create(void)
{
	// register ourself to the framework so it knows our existence
	cvtask_register(&entry, CVTASK_API_VERSION);
	return CVTASK_ERR_OK;
}

cvtask_declare_create (idsp_analyzer_create);
