#include <app_base.h>
#include "aac_queue.h"

static UINT8 aac_in_task_stack[128 * 1024]__attribute__((section(".bss.noinit")));
static AMBA_KAL_TASK_t aac_in_task;

static int running = 0;
static int aac_record_flag = 0;

static char aac_path[128] = {0};
static void *aac_in_task_entry(void *arg)
{
    AMBA_FS_FILE *pFile = NULL;
    unsigned int num_write = 0;
    unsigned int start_tick = 0;
    aac_frame_info_s tmp_aac_frame;

    (void)arg;
    while (running) {
        if (aac_queue_pop(&tmp_aac_frame) < 0) {
            //debug_line("no aac data yet");
            msleep(20);
            continue;
        }
        if (aac_record_flag) {
            if (pFile == NULL) {
                if (AmbaFS_FileOpen(aac_path, "wb", &pFile) == AMBA_FS_ERR_NONE) {
                    debug_line("Audio AAC Created: %s", aac_path);
                    start_tick = tick();
                } else {
                    debug_line("Audio AAC Create Failed: %s", aac_path);
                }
            }
            if (pFile) {
                if (tmp_aac_frame.len > 0) {
                    AmbaFS_FileWrite(tmp_aac_frame.buffer, 1, tmp_aac_frame.len, pFile, &num_write);
                }
            }
        } else {
            if (pFile) {
                unsigned int cur_tick = tick();
                AmbaFS_FileSync(pFile);
                AmbaFS_FileClose(pFile);
                debug_line("Audio AAC Closed. record_time=%dms", cur_tick - start_tick);
            }
            pFile = NULL;
        }
        //debug_line("aac data len=%d", tmp_aac_frame.len);
        msleep(10);
    }
	
	return NULL;
}

static int aac_in_task_start(void)
{
    int ReturnValue = 0;

    if (running) {
        debug_line("%s is already running", __func__);
        return -1;
    }
    running = 1;
    ReturnValue = AmbaKAL_TaskCreate(&aac_in_task,
                                    "AAC_In_Task",
                                    SVC_AAC_RECORD_TASK_PRI,
                                    aac_in_task_entry,
                                    (unsigned int)0x0,
                                    aac_in_task_stack,
                                    sizeof(aac_in_task_stack),
                                    0);
    if (ReturnValue != OK) {
        debug_line("%s aac record task fail (error code: %d)", __func__, ReturnValue);
        running = 0;
        return -1;
    }
    AmbaKAL_TaskSetSmpAffinity(&aac_in_task, SVC_AAC_RECORD_TASK_CPU_BITS);
    AmbaKAL_TaskResume(&aac_in_task);

    return 0;
}

static int aac_in_task_stop(void)
{
    int Rval = 0;

    running = 0;
    msleep(200);
    Rval = AmbaKAL_TaskTerminate(&aac_in_task);
    if (Rval == OK) {
        Rval = AmbaKAL_TaskDelete(&aac_in_task);
    }

    return 0;
}

int aac_record_task_start(void)
{
    aac_queue_init();
    aac_in_task_start();
    memset(aac_path, 0, sizeof(aac_path));

    return 0;
}

int aac_record_task_stop(void)
{
    aac_in_task_stop();
    aac_queue_destroy();

    return 0;
}

static int check_create_dir(const char *path)
{
    char dir_path[128] = {0};
    int i = 0, index = -1;

    if (path == NULL || AmbaUtility_StringLength(path) <= 0) {
        AmbaPrint_PrintStr5("%s: %s", __func__, "invalid param", NULL, NULL, NULL);
        return -1;
    }
    AmbaUtility_MemorySetChar(dir_path, 0, sizeof(dir_path));
    for (i = AmbaUtility_StringLength(path) - 1; i >= 0; i--) {
        if (path[i] == '\\') {
            index = i;
            break;
        }
    }

    if (index <= 0) {
        AmbaPrint_PrintStr5("%s: invalid path[%s]", __func__, path, NULL, NULL, NULL);
        return -1;
    }
    AmbaWrap_memcpy(dir_path, path, index);
    if (AmbaFS_ChangeDir(dir_path) != AMBA_FS_ERR_NONE) {
        if (AmbaFS_MakeDir(dir_path) != AMBA_FS_ERR_NONE) {
            AmbaPrint_PrintStr5("%s: create %s failed", __func__, dir_path, NULL, NULL, NULL);
            return -1;
        }
    }

    return 0;
}

int aac_record_set_path(const char *path)
{
    if (path == NULL || strlen(path) <= 0) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    if (check_create_dir(path) < 0) {
        debug_line("%s create path fail", __func__);
        return -1;
    }
    memset(aac_path, 0, sizeof(aac_path));
    snprintf(aac_path, sizeof(aac_path) - 1, "%s", path);

    return 0;
}

int aac_record_is_busy(void)
{
    return aac_record_flag ? 1 : 0;
}

int aac_record_start(void)
{
    if (aac_queue_get_history_cnt() == 0) {
        debug_line("%s audio not start yet", __func__);
        return -1;
    }
    if (aac_record_flag) {        
        debug_line("%s aac record is busy", __func__);
        return -1;
    }
    if (strlen(aac_path) <= 0) {
        debug_line("%s please set aac path first", __func__);
        return -1;
    }    
    aac_queue_clear();
    aac_record_flag = 1;

    return 0;
}

int aac_record_stop(void)
{
    aac_record_flag = 0;

    return 0;
}

