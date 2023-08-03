#include "imu_record.h"
#include "platform.h"
#include "app_base.h"

#define IMU_RECORD_MAX_NUM (2)

static imu_record_s imu_records[IMU_RECORD_MAX_NUM] = {0};
static AMBA_KAL_MUTEX_t mutex;
static int started = 0;

typedef struct _imu_time_s_ {
    linux_time_s rtc_time;
    unsigned int imu_time;
} imu_time_s;

#define ICACHE_SIZE (1536 * 1024)
typedef struct _cache_s_ {
    unsigned char cache[ICACHE_SIZE];
    int cache_num;
    imu_time_s start_time;
} cache_s;
static cache_s imu_caches[IMU_RECORD_MAX_NUM] = {0};
static cache_s imu_caches2[IMU_RECORD_MAX_NUM] = {0};

static int imu_record_write_header(unsigned char *buf, int *len, imu_time_s *base_time)
{
    char line[1024] = {0};
    metadata_s *data = NULL;
    device_info_s *device_info = NULL;
    SVC_USER_PREF_s *pSvcUserPref = NULL;
    char accel_line[128] = {0};
    char gyro_line[128] = {0};
    IMU_SETTING_s setting_data;
    linux_time_s linux_time;

    metadata_get(&data);
    memset(line, 0, sizeof(line));
    device_info = app_helper.get_device_info();
    app_helper.get_linux_time(&linux_time);
    SvcUserPref_Get(&pSvcUserPref);
    Imu_GetSetting(&setting_data);
    memset(accel_line, 0, sizeof(accel_line));
    snprintf(accel_line, sizeof(accel_line) - 1, "%d|%.6f,%.6f,%.6f,%.6f,%.6f,%.6f|%d,%d,%d,%d|%s",
                                                    setting_data.accel_setting.calibrationed,
                                                    setting_data.accel_setting.calibration_data[0], setting_data.accel_setting.calibration_data[1], setting_data.accel_setting.calibration_data[2],
                                                    setting_data.accel_setting.calibration_data[3], setting_data.accel_setting.calibration_data[4], setting_data.accel_setting.calibration_data[5],
                                                    setting_data.accel_setting.range, setting_data.accel_setting.odr, setting_data.accel_setting.swap_polarity, setting_data.accel_setting.power_bw,
                                                    Imu_GetName());
    memset(gyro_line, 0, sizeof(gyro_line));
    snprintf(gyro_line, sizeof(gyro_line) - 1, "%d|%.6f,%.6f,%.6f,%.6f,%.6f|%d,%d",
                                                    setting_data.gyro_setting.calibrationed,
                                                    setting_data.gyro_setting.calibration_data[0], setting_data.gyro_setting.calibration_data[1], setting_data.gyro_setting.calibration_data[2],
                                                    setting_data.gyro_setting.calibration_data[3], setting_data.gyro_setting.calibration_data[4],
                                                    setting_data.gyro_setting.range, setting_data.gyro_setting.odr);
    snprintf(line, sizeof(line) - 1, "device=%s-%s; sn=%s; vsn=%s; cpuid=%s; wifimac=%.2x:%.2x:%.2x:%.2x:%.2x:%.2x; trip=%s; timezone=%d; time=%d,%d; gsensor=%s; gyro=%s\n",
                                     PROJECT_MODEL, device_info->sw_version,
                                     (char *)data->SerialNumber,
                                     device_info->vsn,
                                     (char *)data->CpuId,
                                     data->WifiMac[0], data->WifiMac[1], data->WifiMac[2], data->WifiMac[3], data->WifiMac[4], data->WifiMac[5],
                                     device_info->trip_key,
                                     pSvcUserPref->TimeZone,
                                     linux_time.sec,
                                     linux_time.usec,
                                     accel_line,
                                     gyro_line);
    base_time->rtc_time = linux_time;
    Imu_GetSensorTime(&(base_time->imu_time));
    memcpy(buf, line, strlen(line));
    *len = strlen(line);

    return 0;
}

static int find_filename_index(const char *name)
{
    int i = 0;

    for (i = strlen(name) - 1; i >= 0; i--) {
        if (*(name + i) == '\\') {
            return i + 1;
        }
    }

    return 0;
}

int imu_record_control(unsigned int ctrl_type, unsigned int arg, const char *filename, void *param)
{
    static int inited = 0;
    int stream_index = 0;
    SVC_USER_PREF_s *pSvcUserPref;

    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->ImuRecord == 0
        || dqa_test_script.disable_data_file) {
        return 0;
    }

    if (inited == 0) {
        if (AmbaKAL_MutexCreate(&mutex, "imu_record") != OK) {
            debug_line("%s create mutex fail", __func__);
            return -1;
        }
        inited = 1;
    }
    AmbaKAL_MutexTake(&mutex, AMBA_KAL_WAIT_FOREVER);
    switch (ctrl_type) {
    case IMU_RECORD_FILE_OPEN:        
        stream_index = arg / 2;
        if ((imu_records[stream_index].status & IMU_FILE_STAT_OPEN) == 0) {
            int index = 0;
            //write cache
            imu_records[stream_index].cache_index = 0;
            memset(&imu_caches[stream_index], 0, sizeof(imu_caches[stream_index]));
            imu_record_write_header(imu_caches[stream_index].cache, &(imu_caches[stream_index].cache_num), &(imu_caches[stream_index].start_time));
            imu_records[stream_index].status |= IMU_FILE_STAT_OPEN;
            //open file
            memset(imu_records[stream_index].filename, 0, sizeof(imu_records[stream_index].filename));
            index = find_filename_index(filename);
            memcpy(imu_records[stream_index].filename, filename, index);
            strcat(imu_records[stream_index].filename, DCF_IMU_DIR_NAME"\\");
            memcpy(imu_records[stream_index].filename + strlen(imu_records[stream_index].filename), filename + index, strlen(filename) - index);
            imu_records[stream_index].filename[strlen(imu_records[stream_index].filename) - 3] = 'd';
            imu_records[stream_index].filename[strlen(imu_records[stream_index].filename) - 2] = 'a';
            imu_records[stream_index].filename[strlen(imu_records[stream_index].filename) - 1] = 't';
            imu_records[stream_index].pFile = fopen(imu_records[stream_index].filename, "w");
            if (imu_records[stream_index].pFile != NULL) {
                debug_line("%s Create File: %s", __func__, imu_records[stream_index].filename);
            } else {
                debug_line("%s Fail Create: %s", __func__, imu_records[stream_index].filename);
            }
        }
        started = 1;
        break;
    case IMU_RECORD_FILE_CLOSE:
        stream_index = arg / 2;
        if (imu_records[stream_index].status & IMU_FILE_STAT_OPEN) {
            imu_records[stream_index].status &= ~IMU_FILE_STAT_OPEN;
            if (imu_records[stream_index].pFile != NULL) {
                if (imu_records[stream_index].cache_index == 0) {
                    if (imu_caches[stream_index].cache_num > 0) {
                        fwrite(imu_caches[stream_index].cache, imu_caches[stream_index].cache_num, 1, imu_records[stream_index].pFile);
                        imu_caches[stream_index].cache_num = 0;
                    }
                } else {
                    if (imu_caches2[stream_index].cache_num > 0) {
                        fwrite(imu_caches2[stream_index].cache, imu_caches2[stream_index].cache_num, 1, imu_records[stream_index].pFile);
                        imu_caches2[stream_index].cache_num = 0;
                    }
                }
                fflush(imu_records[stream_index].pFile);
                fclose(imu_records[stream_index].pFile);
            }
            imu_records[stream_index].pFile = NULL;
            debug_line("%s Close File: %s", __func__, imu_records[stream_index].filename);
            break;
        }
        started = 0;
        break;
    case IMU_RECORD_CACHE_SPLIT:
        stream_index = arg / 2;
        imu_records[stream_index].save_index = imu_records[stream_index].cache_index;
        if (imu_records[stream_index].cache_index == 0) {
            debug_line("%s vin%d split, cache_index=%d, len=%d", __func__, stream_index, 0, imu_caches[stream_index].cache_num);
            memset(&imu_caches2[stream_index], 0, sizeof(imu_caches2[stream_index]));
            imu_record_write_header(imu_caches2[stream_index].cache, &(imu_caches2[stream_index].cache_num), &(imu_caches[stream_index].start_time));
            imu_records[stream_index].cache_index = 1;
        } else {
            debug_line("%s vin%d split, cache_index=%d, len=%d", __func__, stream_index, 1, imu_caches2[stream_index].cache_num);
            memset(&imu_caches[stream_index], 0, sizeof(imu_caches[stream_index]));
            imu_record_write_header(imu_caches[stream_index].cache, &(imu_caches[stream_index].cache_num), &(imu_caches[stream_index].start_time));
            imu_records[stream_index].cache_index = 0;
        }
        break;
    case IMU_RECORD_FILE_SPLIT:
        stream_index = arg / 2;
        if (imu_records[stream_index].status & IMU_FILE_STAT_OPEN) {
            int index = 0;
            memset(imu_records[stream_index].filename, 0, sizeof(imu_records[stream_index].filename));
            index = find_filename_index(filename);
            memcpy(imu_records[stream_index].filename, filename, index);
            strcat(imu_records[stream_index].filename, DCF_IMU_DIR_NAME"\\");
            memcpy(imu_records[stream_index].filename + strlen(imu_records[stream_index].filename), filename + index, strlen(filename) - index);
            imu_records[stream_index].filename[strlen(imu_records[stream_index].filename) - 3] = 'd';
            imu_records[stream_index].filename[strlen(imu_records[stream_index].filename) - 2] = 'a';
            imu_records[stream_index].filename[strlen(imu_records[stream_index].filename) - 1] = 't';
            imu_records[stream_index].status |= IMU_FILE_STAT_SPLIT;
        }
        break;
    case IMU_RECORD_FILE_WRITE:
        {
            int i = 0;
            for (i = 0; i < IMU_RECORD_MAX_NUM; i++) {
                if (imu_records[i].status & IMU_FILE_STAT_OPEN) {
                    int len = strlen((char *)param);
                    int time_len = 0;
                    char tmp[32] = {0};
                    unsigned int microseconds_passed = (arg - imu_caches[i].start_time.imu_time);//microseconds passed
                    unsigned int cur_seconds = imu_caches[i].start_time.rtc_time.sec + (imu_caches[i].start_time.rtc_time.usec + microseconds_passed) / 1000000;
                    unsigned int cur_microseconds = (imu_caches[i].start_time.rtc_time.usec + microseconds_passed) % 1000000;
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp) - 1, "%d.%.6d", cur_seconds, cur_microseconds);
                    time_len += strlen(tmp);
                    len += time_len;
                    if (imu_records[i].cache_index == 0) {
                        if ((imu_caches[i].cache_num + len) <= ICACHE_SIZE) {
                            memcpy(imu_caches[i].cache + imu_caches[i].cache_num, tmp, time_len);
                            memcpy(imu_caches[i].cache + imu_caches[i].cache_num + time_len, param, len - time_len);
                            imu_caches[i].cache_num += len;
                        } else {
                            debug_line("%s imu cache0 is full(%d>%d)", __func__, imu_caches[i].cache_num + len, ICACHE_SIZE);
                        }
                    } else {
                        if ((imu_caches2[i].cache_num + len) <= ICACHE_SIZE) {
                            memcpy(imu_caches2[i].cache + imu_caches2[i].cache_num, tmp, time_len);
                            memcpy(imu_caches2[i].cache + imu_caches2[i].cache_num + time_len, param, len - time_len);
                            imu_caches2[i].cache_num += len;
                        } else {
                            debug_line("%s imu cache1 is full(%d>%d)", __func__, imu_caches2[i].cache_num + len, ICACHE_SIZE);
                        }
                    }
                }
            }
        }
        break;
    default:
        break;
    }
    AmbaKAL_MutexGive(&mutex);

    return 0;
}

static AMBA_KAL_TASK_t imu_record_task;
#define IMU_RECORD_TASK_STACK_SIZE (64 * 1024)
static unsigned char imu_record_task_stack[IMU_RECORD_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static int running = 1;

static void *imu_record_task_entry(void *arg)
{
    int i = 0;

    while (running) {
        if (started) {
            for (i = 0; i < IMU_RECORD_MAX_NUM; i++) {
                if (imu_records[i].status & IMU_FILE_STAT_SPLIT) {
                    imu_records[i].status &= ~IMU_FILE_STAT_SPLIT;
                    if (imu_records[i].save_index == 0) {
                        if (imu_caches[i].cache_num > 0) {
                            fwrite(imu_caches[i].cache, imu_caches[i].cache_num, 1, imu_records[i].pFile);
                            imu_caches[i].cache_num = 0;
                        }
                    } else {
                        if (imu_caches2[i].cache_num > 0) {
                            fwrite(imu_caches2[i].cache, imu_caches2[i].cache_num, 1, imu_records[i].pFile);
                            imu_caches2[i].cache_num = 0;
                        }
                    }
                    fflush(imu_records[i].pFile);
                    fclose(imu_records[i].pFile);
                    imu_records[i].pFile = fopen(imu_records[i].filename, "w");
                    if (imu_records[i].pFile != NULL) {
                        debug_line("%s Create File: %s", __func__, imu_records[i].filename);
                    } else {
                        debug_line("%s Fail Create: %s", __func__, imu_records[i].filename);
                    }
                }
            }
        }
        sleep(1);
    }

    return NULL;
}

int imu_record_task_start(void)
{
    unsigned int ret = 0;

    running = 1;
    ret = AmbaKAL_TaskCreate(&imu_record_task,
                            "imu_record_task",
                            80,
                            (AMBA_KAL_TASK_ENTRY_f)imu_record_task_entry,
                            0,
                            imu_record_task_stack,
                            IMU_RECORD_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("imu_record_task create fail");
        running = 0;
        return -1;
    }

    return 0;
}

int imu_record_task_stop(void)
{
    running = 0;

    return 0;
}

