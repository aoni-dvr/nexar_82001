#include "app_base.h"

extern void AppUvcd_SendFrame(unsigned int id, unsigned char *start, unsigned int size);

static int running = 0;
static int debug_mode = 0;
static yuv_frame_info_s tmp_yuv_frame;

int uvc_output_task_set_debug(int debug)
{
    debug_mode = debug;

    return 0;
}

static int img2yuyv(yuv_frame_info_s *yuv_frame_info, unsigned char *out, unsigned int out_size)
{
    int out_w = 0, out_h = 0;
    int j = 0, k = 0;

    if (yuv_frame_info == NULL || out == NULL || out_size == 0) {
        return -1;
    }
    out_w = yuv_frame_info->w;
    out_h = yuv_frame_info->h;
    memset(out, 128, out_size);
    for (j = 0; j < out_h; j++) {
        for (k = 0; k < out_w; k++) {
            *(out + j * (out_w * 2) + 2 * k) = *((UINT8 *)(yuv_frame_info->yuv_buffer + (j * out_w) + k));
        }
    }

    return 0;
}

static unsigned char tmp_buffer[UVC_STREAM_WIDTH * UVC_STREAM_HEIGHT * 2] = {0};
static void *uvc_output_task_entry(void *argv)
{
    unsigned int start_tick = 0, cur_tick = 0;

	(void)argv;
    while (running) {
        start_tick = tick();
        if (uvc_yuv_queue_pop(&tmp_yuv_frame) < 0) {
            if (debug_mode) {
                debug_line("uvc_yuv_queue_pop fail");
            }
            msleep(5);
            continue;
        }
        if (debug_mode) {
            debug_line("send %dx%d", tmp_yuv_frame.w, tmp_yuv_frame.h);
        }
        if (Pmic_CheckUsbConnected() == 0) {
            debug_line("usb removed, stop uvc output");
            break;
        }
        img2yuyv(&tmp_yuv_frame, tmp_buffer, sizeof(tmp_buffer));
        AppUvcd_SendFrame(0, tmp_buffer, sizeof(tmp_buffer));
        cur_tick = tick();
        if (cur_tick - start_tick < 33) {
            msleep(33 - (cur_tick - start_tick));
        } else {
            msleep(1);
        }
    }

    return NULL;
}

static UINT8 uvc_output_task_stack[1024 * 1024] __attribute__((section(".bss.noinit")));
static AMBA_KAL_TASK_t uvc_output_task;
int uvc_output_task_start(void)
{
    int ReturnValue = 0;

    if (running) {
        debug_line("%s is already running", __func__);
        return -1;
    }
    running = 1;
    ReturnValue = AmbaKAL_TaskCreate(&uvc_output_task,
                                    "UVC YUV OUTTask",
                                    SVC_UVC_TASK_PRI,
                                    uvc_output_task_entry,
                                    (unsigned int)0x0,
                                    uvc_output_task_stack,
                                    sizeof(uvc_output_task_stack),
                                    AMBA_KAL_AUTO_START);
    if (ReturnValue != OK) {
        debug_line("%s yuv output task fail (error code: %d)", __func__, ReturnValue);
        running = 0;
        return -1;
    }
    AmbaKAL_TaskSetSmpAffinity(&uvc_output_task, SVC_UVC_TASK_CPU_BITS);
    AmbaKAL_TaskResume(&uvc_output_task);

    debug_line("%s create task ok", __func__);

    return 0;
}

int uvc_output_task_stop(void)
{
    int Rval = 0;

    running = 0;
    msleep(200);
    Rval = AmbaKAL_TaskTerminate(&uvc_output_task);
    if (Rval == OK) {
        Rval = AmbaKAL_TaskDelete(&uvc_output_task);
    }

    return 0;
}

