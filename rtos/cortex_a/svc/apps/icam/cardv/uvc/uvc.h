#ifndef __UVC_H__
#define __UVC_H__

#include "app_base.h"

#define YUV_QUEUE_MAX_SIZE (10)

#define UVC_STREAM_WIDTH (848)
#define UVC_STREAM_HEIGHT (480)

typedef struct _yuv_frame_info_s_ {
    int w;
    int h;
    int color_fmt;
    //unsigned char *yuv_buffer;
    unsigned char yuv_buffer[UVC_STREAM_WIDTH * UVC_STREAM_HEIGHT * 2];//max
} yuv_frame_info_s;

typedef struct _yuv_queue_s_ {
    int write_index;
    int read_index;
    yuv_frame_info_s queue[YUV_QUEUE_MAX_SIZE];
} yuv_queue_s;

int uvc_start(int vin);
int uvc_stop(void);
int uvc_yuv_queue_init(void);
int uvc_yuv_queue_get(yuv_frame_info_s **yuv_frame_info, int *index);
int uvc_yuv_queue_push(int index);
int uvc_yuv_queue_pop(yuv_frame_info_s *yuv_frame_info);
int uvc_yuv_queue_destroy(void);
int uvc_output_task_start(void);
int uvc_output_task_stop(void);
int uvc_output_task_set_debug(int debug);
int uvc_set_debug(int debug);

#endif//__UVC_H__

