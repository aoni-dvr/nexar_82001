#include "scan_file_info_task.h"
#include "app_base.h"

#define DEBUG_TAG "[scan_file_info_task]"

static AMBA_KAL_TASK_t scan_file_info_task;
#define SCAN_FILE_INFO_TASK_STACK_SIZE (32 * 1024)
static unsigned char scan_file_info_task_stack[SCAN_FILE_INFO_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static void *scan_file_info_task_entry(void *argv);
static int running = 0;

static AMBA_KAL_EVENT_FLAG_t ScanFileInfoFlag;
#define SCAN_FILE_INFO_START_FLAG (0x00000001U)

int scan_file_info_task_start(void)
{
    unsigned int ret = 0;

    running = 1;
    AmbaKAL_EventFlagCreate(&ScanFileInfoFlag, "ScanFileInfoFlag");
    ret = AmbaKAL_TaskCreate(&scan_file_info_task,
                            "scan_file_info_task",
                            250,
                            (AMBA_KAL_TASK_ENTRY_f)scan_file_info_task_entry,
                            0,
                            scan_file_info_task_stack,
                            SCAN_FILE_INFO_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line(DEBUG_TAG"task create fail");
        running = 0;
        return -1;
    }
    AmbaKAL_TaskSetSmpAffinity(&scan_file_info_task, 0x02);
    AmbaKAL_TaskResume(&scan_file_info_task);

    return 0;
}

int scan_file_info_task_stop(void)
{
    int Rval = 0;

    running = 0;
    AmbaKAL_TaskSleep(200);
    Rval = AmbaKAL_TaskTerminate(&scan_file_info_task);
    if (Rval == OK) {
        Rval = AmbaKAL_TaskDelete(&scan_file_info_task);
    }

    return 0;
}

int scan_file_info_task_run(void)
{
    AmbaKAL_EventFlagSet(&ScanFileInfoFlag, SCAN_FILE_INFO_START_FLAG);

    return 0;
}

static void *scan_file_info_task_entry(void *argv)
{
    UINT32 ActualFlags = 0U, StartTime = 0;

    while (running) {
        (void)AmbaKAL_EventFlagGet(&ScanFileInfoFlag, SCAN_FILE_INFO_START_FLAG,
                         AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO,
                         &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if (ActualFlags & SCAN_FILE_INFO_START_FLAG) {
            debug_line(DEBUG_TAG"scan file info start");
            StartTime = tick();
            //AmbaDCF_DashcamScanFileAttr();
            debug_line(DEBUG_TAG"scan file info finish. use time: %ds", (tick() - StartTime) / 1000);
        }
    }

    return NULL;
}

