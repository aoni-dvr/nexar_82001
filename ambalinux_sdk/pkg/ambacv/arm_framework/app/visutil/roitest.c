#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "schdr_util.h"


#define ALIGN(x, a)             (((x) + (a-1)) & ~(a-1))

static void setup_default_config(roi_setup_t *config, int w0, int h0)
{
	roi_rect_t *info;
	uint32_t w1, h1, i = 0;
	float    factor = 1.41421356237;

	memset(config, 0, sizeof(*config));

	// half-octave 0
	info = &config->half_octave[i++];
	info->w = w0;
	info->h = h0;

	w1 = w0 / factor; w1 = ALIGN(w1, 3);
	h1 = h0 / factor; h1 = ALIGN(h1, 3);
	w0 = ALIGN(w0>>1, 1);
	h0 = ALIGN(h0>>1, 1);

	// half-octave 1 ~ 12,
	while (i < 13) {
		info = &config->half_octave[i++];
		info->w = w1;
		info->h = h1;
		info = &config->half_octave[i++];
		info->w = w0;
		info->h = h0;

		// scale down by 2
		w1 = ALIGN(w1>>1, 1);
		h1 = ALIGN(h1>>1, 1);
		w0 = ALIGN(w0>>1, 1);
		h0 = ALIGN(h0>>1, 1);
	}

	// lane detection
	config->ld_input_scale = 0;
	config->lane_det.x = 200;
	config->lane_det.y = 200;
	config->lane_det.w = 240;
	config->lane_det.h = 320;
}

int main(int argc, char **argv)
{
	roi_setup_t roi;
	int scale, x, y, w, h;

	setup_default_config(&roi, 1920, 1080);
	scale = atoi(argv[1]);
	x = atoi(argv[2]);
	y = atoi(argv[3]);
	w = atoi(argv[4]);
	h = atoi(argv[5]);
	roi.half_octave[scale].x = x;
	roi.half_octave[scale].y = y;
	roi.half_octave[scale].w = w;
	roi.half_octave[scale].h = h;
	schdr_setup_rois(1, &roi);
	return 0;
}
