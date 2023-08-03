#include <app_base.h>
#include "pcm_queue.h"

static UINT8 pcm_in_task_stack[128 * 1024]__attribute__((section(".bss.noinit")));
static AMBA_KAL_TASK_t pcm_in_task;

static int running = 0;
static int pcm_record_flag = 0;
static int record_stop_busy = 0;

static char pcm_path[128] = {0};
static void *pcm_in_task_entry(void *arg)
{
    FILE *pFile = NULL;
    unsigned int start_tick = 0;
    pcm_frame_info_s tmp_pcm_frame;

    (void)arg;
    while (running) {
        if (pcm_queue_pop(&tmp_pcm_frame) < 0) {
            //debug_line("no pcm data yet");
            msleep(20);
            continue;
        }
        if (pcm_record_flag) {
            if (pFile == NULL) {
                pFile = fopen(pcm_path, "wb");
                if (pFile) {
                    debug_line("Audio PCM Created: %s", pcm_path);
                    start_tick = tick();
                } else {
                    debug_line("Audio PCM Create Failed: %s", pcm_path);
                }
            }
            if (pFile) {
                if (tmp_pcm_frame.len > 0) {
                    fwrite(tmp_pcm_frame.buffer, tmp_pcm_frame.len, 1, pFile);                    
                    fflush(pFile);
                }
            }
        } else {
            if (pFile) {
                unsigned int cur_tick = tick();
                fflush(pFile);
                fclose(pFile);
                record_stop_busy = 0;
                debug_line("Audio PCM Closed. record_time=%dms", cur_tick - start_tick);
            }
            pFile = NULL;
        }
        //debug_line("pcm data len=%d", tmp_pcm_frame.len);
        msleep(10);
    }

	return NULL;
}

static int pcm_in_task_start(void)
{
    int ReturnValue = 0;

    if (running) {
        debug_line("%s is already running", __func__);
        return -1;
    }
    running = 1;
    ReturnValue = AmbaKAL_TaskCreate(&pcm_in_task,
                                    "Pcm_In_Task",
                                    SVC_PCM_RECORD_TASK_PRI,
                                    pcm_in_task_entry,
                                    (unsigned int)0x0,
                                    pcm_in_task_stack,
                                    sizeof(pcm_in_task_stack),
                                    0);
    if (ReturnValue != OK) {
        debug_line("%s pcm record task fail (error code: %d)", __func__, ReturnValue);
        running = 0;
        return -1;
    }
    AmbaKAL_TaskSetSmpAffinity(&pcm_in_task, SVC_PCM_RECORD_TASK_CPU_BITS);
    AmbaKAL_TaskResume(&pcm_in_task);

    return 0;
}

static int pcm_in_task_stop(void)
{
    int Rval = 0;

    running = 0;
    msleep(200);
    Rval = AmbaKAL_TaskTerminate(&pcm_in_task);
    if (Rval == OK) {
        Rval = AmbaKAL_TaskDelete(&pcm_in_task);
    }

    return 0;
}

int pcm_record_task_start(void)
{
    pcm_queue_init();
    pcm_in_task_start();
    memset(pcm_path, 0, sizeof(pcm_path));

    return 0;
}

int pcm_record_task_stop(void)
{
    pcm_in_task_stop();
    pcm_queue_destroy();

    return 0;
}

static int check_create_dir(const char *path)
{
    char dir_path[128] = {0};
    int i = 0, index = -1;

    if (path == NULL || strlen(path) <= 0) {
        debug_line("%s: %s", __func__, "invalid param");
        return -1;
    }
    memset(dir_path, 0, sizeof(dir_path));
    for (i = strlen(path) - 1; i >= 0; i--) {
        if (path[i] == '\\') {
            index = i;
            break;
        }
    }

    if (index <= 0) {
        debug_line("%s: invalid path[%s]", __func__, path);
        return -1;
    }
    memcpy(dir_path, path, index);
    if (AmbaFS_ChangeDir(dir_path) != AMBA_FS_ERR_NONE) {
        if (AmbaFS_MakeDir(dir_path) != AMBA_FS_ERR_NONE) {
            debug_line("%s: create %s failed", __func__, dir_path);
            return -1;
        }
    }

    return 0;
}

int pcm_record_set_path(const char *path)
{
    if (path == NULL || strlen(path) <= 0) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    if (check_create_dir(path) < 0) {
        debug_line("%s create path fail", __func__);
        return -1;
    }
    memset(pcm_path, 0, sizeof(pcm_path));
    snprintf(pcm_path, sizeof(pcm_path) - 1, "%s", path);

    return 0;
}

int pcm_record_is_busy(void)
{
    return (pcm_record_flag || record_stop_busy) ? 1 : 0;
}

int pcm_record_start(void)
{
    if (pcm_queue_get_history_cnt() == 0) {
        debug_line("%s audio not start yet", __func__);
        return -1;
    }
    if (pcm_record_flag) {
        debug_line("%s pcm record is busy", __func__);
        return -1;
    }
    if (record_stop_busy) {
        debug_line("%s record stop is busy", __func__);
        return -1;
    }
    if (strlen(pcm_path) <= 0) {
        debug_line("%s please set pcm path first", __func__);
        return -1;
    }
    pcm_queue_clear();
    pcm_record_flag = 1;

    return 0;
}

int pcm_record_wait_finish(void)
{
    while (record_stop_busy) {
        msleep(20);
    }

    return 0;
}

int pcm_record_stop(void)
{
    if (pcm_record_flag) {
        record_stop_busy = 1;
        pcm_record_flag = 0;
    }

    return 0;
}

