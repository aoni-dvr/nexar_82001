#include "imu_task.h"
#include "imu_queue.h"

// msg task
static AMBA_KAL_TASK_t imu_feed_task;
#define IMU_FEED_TASK_STACK_SIZE (64 * 1024)
static unsigned char imu_feed_task_stack[IMU_FEED_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));

#if 0
static AMBA_KAL_TASK_t imu_eat_task;
#define IMU_EAT_TASK_STACK_SIZE (64 * 1024)
static unsigned char imu_eat_task_stack[IMU_EAT_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
#endif

static int debug_enable = 0;

static void *imu_feed_task_entry(void *arg);
#if 0
static void *imu_eat_task_entry(void *arg);
#endif

#ifdef CONFIG_PCBA_DVT
static AMBA_KAL_EVENT_FLAG_t SvcEventFlag;
#define IMU_EVENT_FLAG            0x00000001UL
static void imu_edge_handler(UINT32 GpioPinID, UINT32 UserArg)
{
    AmbaKAL_EventFlagSet(&SvcEventFlag, IMU_EVENT_FLAG);
}
#endif

static int running = 0;
int imu_task_start(void)
{
    unsigned int ret = 0;

    if (running) {
        debug_line("imu_task is already running");
        return 0;
    }
    running = 1;
    imu_queue_init();

    ret = AmbaKAL_TaskCreate(&imu_feed_task,
                            "imu_feed_task",
                            SVC_IMU_FEED_TASK_PRI,
                            (AMBA_KAL_TASK_ENTRY_f)imu_feed_task_entry,
                            0,
                            imu_feed_task_stack,
                            IMU_FEED_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("imu_feed_task create fail");
        running = 0;
        return -1;
    }
    AmbaKAL_TaskSetSmpAffinity(&imu_feed_task, SVC_IMU_FEED_TASK_CPU_BITS);
    AmbaKAL_TaskResume(&imu_feed_task);

#if 0
    ret = AmbaKAL_TaskCreate(&imu_eat_task,
                            "imu_eat_task",
                            SVC_IMU_EAT_TASK_PRI,
                            (AMBA_KAL_TASK_ENTRY_f)imu_eat_task_entry,
                            0,
                            imu_eat_task_stack,
                            IMU_EAT_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("imu_eat_task create fail");
        running = 0;
        return -1;
    }
    AmbaKAL_TaskSetSmpAffinity(&imu_eat_task, SVC_IMU_EAT_TASK_CPU_BITS);
    AmbaKAL_TaskResume(&imu_eat_task);
#endif

#ifdef CONFIG_PCBA_DVT
    AmbaKAL_EventFlagCreate(&SvcEventFlag, "ImuEventFlag");
    AmbaGPIO_SetFuncGPI(IMU_IRQ_PIN);
    AmbaGPIO_SetPullUpOrDown(IMU_IRQ_PIN, AMBA_GPIO_PULL_UP);
    AmbaGPIO_IntSetType(IMU_IRQ_PIN, GPIO_INT_FALLING_EDGE_TRIGGER);
    AmbaGPIO_IntHookHandler(IMU_IRQ_PIN, imu_edge_handler, 0);
    AmbaGPIO_IntEnable(IMU_IRQ_PIN);
#endif

    return 0;
}

int imu_task_stop(void)
{
    running = 0;
    msleep(200);
    AmbaKAL_TaskTerminate(&imu_feed_task);
    AmbaKAL_TaskDelete(&imu_feed_task);
#if 0
    AmbaKAL_TaskTerminate(&imu_eat_task);
    AmbaKAL_TaskDelete(&imu_eat_task);
#endif

    return 0;
}

int imu_task_set_debug(int enable)
{
    debug_enable = enable;

    return 0;
}

static void *imu_feed_task_entry(void *arg)
{
    IMU_DATA_s imu_data;
    TEMP_DATA_s temp_data;
    imu_frame_info_s *frame = NULL;
    int index = 0;
#ifdef CONFIG_PCBA_DVT
    unsigned int ActualFlags = 0;
    int first = 1;
#else
    unsigned int last_tick = 0, use_time = 0;
#endif
    linux_time_s linux_time;
    unsigned int imu_time;
    char line[512] = {0};
    int ir_state = -1;

    while (running) {
        if (app_helper.linux_booted == 0) {
            msleep(100);
            continue;
        }
#ifdef CONFIG_PCBA_DVT
        if (first) {
            first = 0;
            Imu_EnableInt();
        }
        (void)AmbaKAL_EventFlagGet(&SvcEventFlag, IMU_EVENT_FLAG,
                                 AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO,
                                 &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if ((ActualFlags & IMU_EVENT_FLAG) != IMU_EVENT_FLAG) {
			continue;
        }

        if (debug_enable == 2) {
            debug_line("imu data ready");
        }
#else
        last_tick = tick();
#endif
        Imu_GetData(&imu_data, 0);
        Imu_GetTemperature(&temp_data);
        Imu_GetSensorTime(&imu_time);
        app_helper.get_linux_time(&linux_time);
        imu_queue_get_mutex();
        imu_queue_get(&frame, &index);
        if (debug_enable) {
            debug_line("accel x:%f y:%f z:%f, gyro: x:%f y:%f z:%f", imu_data.accel.x_float, imu_data.accel.y_float, imu_data.accel.z_float,
                                                                    imu_data.gyro.x_float, imu_data.gyro.y_float, imu_data.gyro.z_float);
        }
        memcpy(&frame->imu_data, &imu_data, sizeof(IMU_DATA_s));
        memcpy(&frame->temp_data, &temp_data, sizeof(TEMP_DATA_s));
        frame->sec = linux_time.sec;
        frame->usec = linux_time.usec;
        frame->flag = 1;
        imu_queue_push(index);
        imu_queue_release_mutex();
        //for bt imu stream
        if (bt_imu_stream_check_enable()) {
            bt_imu_stream_send_message(frame->imu_data.accel.x_float16, frame->imu_data.accel.y_float16, frame->imu_data.accel.z_float16,
                                        frame->imu_data.gyro.x_float16, frame->imu_data.gyro.y_float16, frame->imu_data.gyro.z_float16);
        }
        //for imu record
        memset(line, 0, sizeof(line));
        snprintf(line, sizeof(line) - 1, "|G:%.6f,%.6f,%.6f,%d,%.6f,%.6f,%.6f,%d,%.2f\n",
                                        imu_data.accel.x_float, imu_data.accel.y_float, imu_data.accel.z_float,
                                        ir_state,
                                        imu_data.gyro.x_float, imu_data.gyro.y_float, imu_data.gyro.z_float,
                                        imu_data.accel.sensor_time,
                                        temp_data.temp_float
                                        );
        imu_record_control(IMU_RECORD_FILE_WRITE, imu_time, NULL, line);
#ifndef CONFIG_PCBA_DVT
        use_time = tick() - last_tick;
        if ((1000 / 50) > 2 * use_time) {
            msleep(1000 / 50 - 2 * use_time);
        }
#endif
    }

	return NULL;
}

#define MAX_SHARE_FRAME_NUM (200)
static imu_share_frame_data_s imu_share_data[MAX_SHARE_FRAME_NUM];
int imu_task_get_fifo_share_info(unsigned long long *share_addr, unsigned long long *phy_addr, unsigned int *share_size)
{
    UINT32 _phy_addr = 0, _share_addr = 0;

    _share_addr = (unsigned int)&imu_share_data[0];
    if (AmbaMMU_Virt32ToPhys32(_share_addr, &_phy_addr) == MMU_ERR_NONE) {
        *share_addr = CAST_TO_UINTPRT(_share_addr);
        *phy_addr = (unsigned long long)_phy_addr;
        *share_size = sizeof(imu_share_data);
        return 0;
    }

    return -1;
}

int imu_task_get_fifo_data(int max_frame_num, int *frame_num)
{
    imu_frame_info_s *frame = NULL;
    int num = 0, index = 0;
    char line[32] = {0};
    int write_index = 0;
    int k = 0;

    if (max_frame_num <= 0 || frame_num == NULL) {
        return -1;
    }
    if (max_frame_num >= MAX_SHARE_FRAME_NUM) {
        max_frame_num = MAX_SHARE_FRAME_NUM;
    }

    imu_queue_get_mutex();
    write_index = imu_queue_get_write_index();
    //debug_line("write index: %d", write_index);
    if (write_index == (IMU_QUEUE_MAX_SIZE - 1)) {
        write_index = -1;
    }
    for (k = write_index + 1; k < IMU_QUEUE_MAX_SIZE; k++) {
        if (imu_queue.queue[k].flag != 0) {
            imu_queue.queue[k].flag = 0;
            frame = &imu_queue.queue[k];
            index = 0;

            memcpy(line + index, &frame->sec, 4);
            index += 4;
            memcpy(line + index, &frame->usec, 4);
            index += 4;

            memcpy(line + index, &frame->imu_data.accel.sensor_time, sizeof(frame->imu_data.accel.sensor_time));
            index += sizeof(frame->imu_data.accel.sensor_time);
            line[index++] = (frame->imu_data.accel.x >> 8) & 0xFF;
            line[index++] = frame->imu_data.accel.x & 0xFF;
            line[index++] = (frame->imu_data.accel.y >> 8) & 0xFF;
            line[index++] = frame->imu_data.accel.y & 0xFF;
            line[index++] = (frame->imu_data.accel.z >> 8) & 0xFF;
            line[index++] = frame->imu_data.accel.z & 0xFF;
            line[index++] = (frame->imu_data.gyro.x >> 8) & 0xFF;
            line[index++] = frame->imu_data.gyro.x & 0xFF;
            line[index++] = (frame->imu_data.gyro.y >> 8) & 0xFF;
            line[index++] = frame->imu_data.gyro.y & 0xFF;
            line[index++] = (frame->imu_data.gyro.z >> 8) & 0xFF;
            line[index++] = frame->imu_data.gyro.z & 0xFF;
            line[index++] = (frame->temp_data.temp >> 8) & 0xFF;
            line[index++] = frame->temp_data.temp & 0xFF;

            memcpy(imu_share_data[num].data, line, index);
            num += 1;
        }
    }

    for (k = 0; k <= write_index; k++) {
        if (imu_queue.queue[k].flag != 0) {
            imu_queue.queue[k].flag = 0;
            frame = &imu_queue.queue[k];
            index = 0;

            memcpy(line + index, &frame->sec, 4);
            index += 4;
            memcpy(line + index, &frame->usec, 4);
            index += 4;

            memcpy(line + index, &frame->imu_data.accel.sensor_time, sizeof(frame->imu_data.accel.sensor_time));
            index += sizeof(frame->imu_data.accel.sensor_time);
            line[index++] = (frame->imu_data.accel.x >> 8) & 0xFF;
            line[index++] = frame->imu_data.accel.x & 0xFF;
            line[index++] = (frame->imu_data.accel.y >> 8) & 0xFF;
            line[index++] = frame->imu_data.accel.y & 0xFF;
            line[index++] = (frame->imu_data.accel.z >> 8) & 0xFF;
            line[index++] = frame->imu_data.accel.z & 0xFF;
            line[index++] = (frame->imu_data.gyro.x >> 8) & 0xFF;
            line[index++] = frame->imu_data.gyro.x & 0xFF;
            line[index++] = (frame->imu_data.gyro.y >> 8) & 0xFF;
            line[index++] = frame->imu_data.gyro.y & 0xFF;
            line[index++] = (frame->imu_data.gyro.z >> 8) & 0xFF;
            line[index++] = frame->imu_data.gyro.z & 0xFF;
            line[index++] = (frame->temp_data.temp >> 8) & 0xFF;
            line[index++] = frame->temp_data.temp & 0xFF;

            memcpy(imu_share_data[num].data, line, index);
            num += 1;
        }
    }
    imu_queue_release_mutex();
    (void)AmbaCache_DataClean((unsigned int)&imu_share_data[0], sizeof(imu_share_data));

    //debug_line("max_num: %d, num=%d", max_frame_num, num);
    if (frame_num != NULL) {
        *frame_num = num;
    }

    return 0;
}

#if 0
static void *imu_eat_task_entry(void *arg)
{
    imu_frame_info_s frame;
    unsigned int index = 0;
    char line[512] = {0};
    unsigned long long frame_num = 0;

    while (running) {
        if (app_helper.linux_booted == 0) {
            msleep(100);
            continue;
        }
        if (imu_queue_pop(&frame) < 0) {
            msleep(10);
            continue;
        }
        if (bt_imu_stream_check_enable()) {
            bt_imu_stream_send_message(frame.imu_data.accel.x_float16, frame.imu_data.accel.y_float16, frame.imu_data.accel.z_float16,
                                        frame.imu_data.gyro.x_float16, frame.imu_data.gyro.y_float16, frame.imu_data.gyro.z_float16);
        }
        // send stream share
        index = 0;

        frame_num++;
        memcpy(line + index, &frame_num, 8);
        index += 8;

        memcpy(line + index, &frame.sec, 4);
        index += 4;
        memcpy(line + index, &frame.usec, 4);
        index += 4;

        memcpy(line + index, &frame.imu_data.accel.sensor_time, sizeof(frame.imu_data.accel.sensor_time));
        index += sizeof(frame.imu_data.accel.sensor_time);
        line[index++] = (frame.imu_data.accel.x >> 8) & 0xFF;
        line[index++] = frame.imu_data.accel.x & 0xFF;
        line[index++] = (frame.imu_data.accel.y >> 8) & 0xFF;
        line[index++] = frame.imu_data.accel.y & 0xFF;
        line[index++] = (frame.imu_data.accel.z >> 8) & 0xFF;
        line[index++] = frame.imu_data.accel.z & 0xFF;
        line[index++] = (frame.imu_data.gyro.x >> 8) & 0xFF;
        line[index++] = frame.imu_data.gyro.x & 0xFF;
        line[index++] = (frame.imu_data.gyro.y >> 8) & 0xFF;
        line[index++] = frame.imu_data.gyro.y & 0xFF;
        line[index++] = (frame.imu_data.gyro.z >> 8) & 0xFF;
        line[index++] = frame.imu_data.gyro.z & 0xFF;
        line[index++] = (frame.temp_data.temp >> 8) & 0xFF;
        line[index++] = frame.temp_data.temp & 0xFF;
        stream_share_send_data(0, (unsigned char *)line, index, 0);

        //write file
        memset(line, 0, sizeof(line));
        snprintf(line, sizeof(line) - 1, "%d%.6d|G:%.6f,%.6f,%.6f,%d,%.6f,%.6f,%.6f,%d,%.2f\n",
                                        frame.sec, frame.usec,
                                        frame.imu_data.accel.x_float,frame.imu_data.accel.y_float, frame.imu_data.accel.z_float,
                                        -1,
                                        frame.imu_data.gyro.x_float, frame.imu_data.gyro.y_float, frame.imu_data.gyro.z_float,
                                        frame.imu_data.accel.sensor_time,
                                        frame.temp_data.temp_float);
        if (debug_enable) {
            debug_line(line);
        }
        msleep(5);
    }

    return NULL;
}
#endif

