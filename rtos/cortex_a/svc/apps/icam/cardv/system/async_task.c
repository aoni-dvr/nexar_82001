#include "async_task.h"
#include "SvcStgMgr.h"
#include "AmbaDCF.h"
#include "SvcStgMonitor.h"

// msg queue
static AMBA_KAL_MSG_QUEUE_t async_queue;
#define ASYNC_TASK_QUEUE_SIZE (16)
static async_message_s async_pool[ASYNC_TASK_QUEUE_SIZE] __attribute__((section(".bss.noinit")));

static AMBA_KAL_TASK_t async_task;
#define ASYNC_TASK_STACK_SIZE (32 * 1024)
static unsigned char async_task_stack[ASYNC_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static void *async_task_entry(void *argv);

int async_task_start(void)
{
    unsigned int ret = 0;

    // create async queue
    ret = AmbaKAL_MsgQueueCreate(&async_queue,
                                "async_queue",
                                sizeof(async_message_s),
                                async_pool,
                                ASYNC_TASK_QUEUE_SIZE);
    if (ret != KAL_ERR_NONE) {
        debug_line("async_queue create fail");
        return -1;
    }

    //create async task
    ret = AmbaKAL_TaskCreate(&async_task,
                            "async_task",
                            SVC_ASYNC_TASK_PRI,
                            (AMBA_KAL_TASK_ENTRY_f)async_task_entry,
                            0,
                            async_task_stack,
                            ASYNC_TASK_STACK_SIZE,
                            0);
    if (ret != KAL_ERR_NONE) {
        debug_line("async_task create fail");
        return -1;
    }
    AmbaKAL_TaskSetSmpAffinity(&async_task, SVC_ASYNC_TASK_CPU_BITS);
    AmbaKAL_TaskResume(&async_task);

    return 0;
}

int async_msg_queue_send(unsigned int msg_id, unsigned int param1, unsigned int param2, unsigned int param3)
{
    async_message_s msg;

    msg.msg_id = msg_id;
    msg.param1 = param1;
    msg.param2 = param2;
    msg.param3 = param3;
    if (AmbaKAL_MsgQueueSend(&async_queue, &msg, AMBA_KAL_NO_WAIT) != KAL_ERR_NONE) {
        debug_line("<%s> send fail", __func__);
        return -1;
    }

    return 0;
}

int async_msg_queue_recv(async_message_s *msg)
{
    if (AmbaKAL_MsgQueueReceive(&async_queue, msg, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {        
        debug_line("<%s> recv fail", __func__);
        return -1;
    }

    return 0;
}

static void *async_task_entry(void *argv)
{
    async_message_s msg;

    AmbaMisra_TouchUnused(&argv);
    while (1) {
        if (async_msg_queue_recv(&msg) < 0) {
            continue;
        }
        switch (msg.msg_id) {
        case ASYNC_MSG_ID_FORAMT_SD:
            if (msg.param1) {//format sd
                if (AmbaSD_IsCardPresent(SD_CHANNEL)) {
                    char Drive = SD_SLOT[0];
                    sleep(3);
                    AmbaDCF_SetReady(0);
                    app_helper.sd_init_done = 0;
                    if (tolower(FILE_LOG_DISK[0]) == tolower(SD_SLOT[0])) {
                        file_log_close();
                    }
                    if (AmbaFS_Format(Drive) == AMBA_FS_ERR_NONE) {
                        SVC_STG_MONI_DRIVE_INFO_s DriveInfo;
                        UINT32 LastValidDirNum = 0;
                        SvcStgMgr_TrigUpdFreeSize(Drive, 5000U);
                        sleep(1);
                        SvcStgMonitor_GetDriveInfo(Drive, &DriveInfo);
                        AmbaDCF_Destroy(DriveInfo.DriveID);
                        AmbaDCF_ConfigDrive(DriveInfo.DriveID, &Drive);
                        AmbaDCF_Init(DriveInfo.DriveID, AMBA_DCF_MOV_FMT_MP4);
                        AmbaDCF_ScanRootStep1(DriveInfo.DriveID, &LastValidDirNum);
                        //AmbaDCF_ScanRootStep2(DriveInfo.DriveID, LastValidDirNum);
                        AmbaDCF_SetReady(1);
                        app_msg_queue_send(APP_MSG_ID_FORMAT_SD_DONE, 0, 0, 0);
                    } else {
                        AmbaDCF_SetReady(1);
                        app_msg_queue_send(APP_MSG_ID_FORMAT_SD_DONE, 0xff, 0, 0);
                    }
                    app_helper.sd_init_done = 1;
                } else {
                    app_msg_queue_send(APP_MSG_ID_FORMAT_SD_DONE, 0xff, 0, 0);
                }
            } else {
#if defined(CONFIG_ENABLE_EMMC_BOOT)
                if (tolower(FILE_LOG_DISK[0]) == tolower(EMMC_SLOT[0])) {
                    file_log_close();
                }
                if (AmbaFS_Format(EMMC_SLOT[0]) == AMBA_FS_ERR_NONE) {
                    app_msg_queue_send(APP_MSG_ID_FORMAT_EMMC_DONE, 0, 0, 0);
                } else {
                    app_msg_queue_send(APP_MSG_ID_FORMAT_EMMC_DONE, 0xff, 0, 0);
                }
#else
                debug_line("not support format emmc");
                app_msg_queue_send(APP_MSG_ID_FORMAT_EMMC_DONE, 0xff, 0, 0);
#endif
            }
            break;
        default:
            break;
        }
    }

    return NULL;
}

