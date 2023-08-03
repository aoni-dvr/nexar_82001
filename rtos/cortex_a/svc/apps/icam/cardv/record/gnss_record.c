#include "gnss_record.h"
#include "platform.h"
#include "app_base.h"

#define GNSS_RECORD_MAX_NUM (2)

static gnss_record_s gnss_records[GNSS_RECORD_MAX_NUM] = {0};
static AMBA_KAL_MUTEX_t mutex;

static linux_time_s t0_time;

#define ICACHE_SIZE (20 * 1024)
typedef struct _cache_s_ {
    unsigned char cache[ICACHE_SIZE];
    int cache_num;
} cache_s;
static cache_s gnss_caches[GNSS_RECORD_MAX_NUM] = {0};

static int gnss_record_write_header(unsigned char *buf, int *len)
{
    char line[128] = {0};
    metadata_s *data = NULL;
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    metadata_get(&data);
    memset(line, 0, sizeof(line));
    app_helper.get_linux_time(&t0_time);
    SvcUserPref_Get(&pSvcUserPref);
    snprintf(line, sizeof(line) - 1, "t0=%d.%.3d\nts=0.001\n", t0_time.sec, t0_time.usec / 1000);
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

int gnss_record_control(unsigned int ctrl_type, const char *filename, void *param)
{
    int i = 0;
    static int inited = 0;
    SVC_USER_PREF_s *pSvcUserPref;

    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->GnssRecord == 0
        || dqa_test_script.disable_data_file) {
        return 0;
    }

    if (inited == 0) {
        if (AmbaKAL_MutexCreate(&mutex, "gnss_record") != OK) {
            debug_line("%s create mutex fail", __func__);
            return -1;
        }
        inited = 1;
    }

    AmbaKAL_MutexTake(&mutex, AMBA_KAL_WAIT_FOREVER);
    switch (ctrl_type) {
    case GNSS_RECORD_FILE_OPEN:
        for (i = 0; i < GNSS_RECORD_MAX_NUM; i++) {            
            memset(&gnss_caches[i], 0, sizeof(gnss_caches[i]));
            if ((gnss_records[i].status & GNSS_FILE_STAT_OPEN) == 0) {
                int index = 0;
                memset(gnss_records[i].filename, 0, sizeof(gnss_records[i].filename));
                index = find_filename_index(filename);
                memcpy(gnss_records[i].filename, filename, index);
                strcat(gnss_records[i].filename, DCF_GNSS_DIR_NAME"\\");
                memcpy(gnss_records[i].filename + strlen(gnss_records[i].filename), filename + index, strlen(filename) - index);
                gnss_records[i].filename[strlen(gnss_records[i].filename) - 3] = 't';
                gnss_records[i].filename[strlen(gnss_records[i].filename) - 2] = 'x';
                gnss_records[i].filename[strlen(gnss_records[i].filename) - 1] = 't';
                gnss_records[i].pFile = fopen(gnss_records[i].filename, "w");
                gnss_records[i].disable_record = 0;
                if (gnss_records[i].pFile != NULL) {
                    gnss_record_write_header(gnss_caches[i].cache, &(gnss_caches[i].cache_num));
                    gnss_records[i].status |= GNSS_FILE_STAT_OPEN;
                    debug_line("%s Create File: %s", __func__, gnss_records[i].filename);
                } else {
                    debug_line("%s Fail Create: %s", __func__, gnss_records[i].filename);
                }
                break;
            }
        }
        break;
    case GNSS_RECORD_FILE_CLOSE:
        for (i = 0; i < GNSS_RECORD_MAX_NUM; i++) {
            if (gnss_records[i].status & GNSS_FILE_STAT_OPEN) {
                int index1 = find_filename_index(filename);
                int index2 = find_filename_index(gnss_records[i].filename);
                if (strncmp(filename + index1, gnss_records[i].filename + index2, strlen(filename + index1) - 4) == 0) {
                    gnss_records[i].disable_record = 1;
                    if (gnss_records[i].pFile != NULL) {
                        if (gnss_caches[i].cache_num > 0) {
                            fwrite(gnss_caches[i].cache, gnss_caches[i].cache_num, 1, gnss_records[i].pFile);
                            gnss_caches[i].cache_num = 0;
                        }
                        fflush(gnss_records[i].pFile);
                        fclose(gnss_records[i].pFile);
                    }
                    gnss_records[i].pFile = NULL;
                    gnss_records[i].status &= ~GNSS_FILE_STAT_OPEN;
                    debug_line("%s Close File: %s", __func__, gnss_records[i].filename);
                    memset(gnss_records[i].filename, 0, sizeof(gnss_records[i].filename));
                    break;
                }
            }
        }
        break;
    case GNSS_RECORD_FILE_SPLIT:
        for (i = 0; i < GNSS_RECORD_MAX_NUM; i++) {
            if (gnss_records[i].status & GNSS_FILE_STAT_OPEN) {
                int index1 = find_filename_index(param);
                int index2 = find_filename_index(gnss_records[i].filename);
                if (strncmp((char *)param + index1, gnss_records[i].filename + index2, strlen((char *)param + index1) - 4) == 0) {
                    int index = 0;
                    memset(gnss_records[i].filename, 0, sizeof(gnss_records[i].filename));
                    index = find_filename_index(filename);
                    memcpy(gnss_records[i].filename, filename, index);
                    strcat(gnss_records[i].filename, DCF_GNSS_DIR_NAME"\\");
                    memcpy(gnss_records[i].filename + strlen(gnss_records[i].filename), filename + index, strlen(filename) - index);
                    gnss_records[i].filename[strlen(gnss_records[i].filename) - 3] = 't';
                    gnss_records[i].filename[strlen(gnss_records[i].filename) - 2] = 'x';
                    gnss_records[i].filename[strlen(gnss_records[i].filename) - 1] = 't';
                    gnss_records[i].status |= GNSS_FILE_STAT_SPLIT;
                    break;
                }
            }
        }
        break;
    case GNSS_RECORD_FILE_WRITE:
        for (i = 0; i < GNSS_RECORD_MAX_NUM; i++) {
            if ((gnss_records[i].status & GNSS_FILE_STAT_OPEN)
                && gnss_records[i].disable_record == 0) {
                int len = strlen((char *)param);
                if ((gnss_caches[i].cache_num + len) <= ICACHE_SIZE) {
                    memcpy(gnss_caches[i].cache + gnss_caches[i].cache_num, param, len);
                    gnss_caches[i].cache_num += len;
                } else {
                    fwrite(gnss_caches[i].cache, gnss_caches[i].cache_num, 1, gnss_records[i].pFile);
                    fflush(gnss_records[i].pFile);
                    memcpy(gnss_caches[i].cache, param, len);
                    gnss_caches[i].cache_num = len;
                }
                if (gnss_records[i].status & GNSS_FILE_STAT_SPLIT) {
                    gnss_records[i].status &= ~GNSS_FILE_STAT_SPLIT;
                    if (gnss_caches[i].cache_num > 0) {
                        fwrite(gnss_caches[i].cache, gnss_caches[i].cache_num, 1, gnss_records[i].pFile);
                        gnss_caches[i].cache_num = 0;
                    }
                    fflush(gnss_records[i].pFile);
                    fclose(gnss_records[i].pFile);
                    gnss_records[i].pFile = fopen(gnss_records[i].filename, "w");
                    if (gnss_records[i].pFile != NULL) {
                        gnss_record_write_header(gnss_caches[i].cache, &(gnss_caches[i].cache_num));
                        gnss_records[i].status |= GNSS_FILE_STAT_OPEN;
                        debug_line("%s Create File: %s", __func__, gnss_records[i].filename);
                    } else {
                        gnss_records[i].status &= ~GNSS_FILE_STAT_OPEN;
                        memset(gnss_records[i].filename, 0, sizeof(gnss_records[i].filename));
                        debug_line("%s Fail Create: %s", __func__, gnss_records[i].filename);
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

static AMBA_KAL_TASK_t gnss_record_task;
#define GNSS_RECORD_TASK_STACK_SIZE (64 * 1024)
static unsigned char gnss_record_task_stack[GNSS_RECORD_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static int running = 1;

static void *gnss_record_task_entry(void *arg)
{
    char line[512] = {0};
    IMU_DATA_s imu_data;
    char tmp[256] = {0};
    gnss_data_s gnss_data;
    int gnss_status = 0;
    char lat[32] = {0};
    char lon[32] = {0};
    unsigned int i = 0;
    int index = 0;
    linux_time_s linux_time;

    while (running) {
        Imu_GetData(&imu_data, 0);
        app_helper.get_linux_time(&linux_time);
        memset(line, 0, sizeof(line));
        //accel
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp) - 1, "%d,%d,%f,%f,%f\n",
                                1,
                                (linux_time.sec + linux_time.usec / 1000) - (t0_time.sec + t0_time.usec / 1000),
                                imu_data.accel.x_float, imu_data.accel.y_float, imu_data.accel.z_float
                                );
        strcat(line, tmp);
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp) - 1, "%d,%d,%d,%d,%d\n",
                                2,
                                (linux_time.sec + linux_time.usec / 1000) - (t0_time.sec + t0_time.usec / 1000),
                                imu_data.accel.x_float * 1000, imu_data.accel.y_float * 1000, imu_data.accel.z_float * 1000
                                );
        strcat(line, tmp);
        //gyro
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp) - 1, "%d,%d,%f,%f,%f\n",
                                5,
                                (linux_time.sec + linux_time.usec / 1000) - (t0_time.sec + t0_time.usec / 1000),
                                imu_data.gyro.x_float, imu_data.accel.y_float, imu_data.accel.z_float
                                );
        strcat(line, tmp);
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp) - 1, "%d,%d,%f,%f,%f\n",
                                6,
                                (linux_time.sec + linux_time.usec / 1000) - (t0_time.sec + t0_time.usec / 1000),
                                imu_data.gyro.x_float, imu_data.accel.y_float, imu_data.accel.z_float
                                );
        strcat(line, tmp);
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp) - 1, "%d,%d,%d,%d,%d\n",
                                7,
                                (linux_time.sec + linux_time.usec / 1000) - (t0_time.sec + t0_time.usec / 1000),
                                imu_data.gyro.x_float * 1000, imu_data.gyro.y_float * 1000, imu_data.gyro.z_float * 1000
                                );
        strcat(line, tmp);
        gnss_status = gnss_parser_get_connected();
        //gnss
        gnss_parser_get_data(&gnss_data);
        if (gnss_status) {
            if (gnss_data.m_bLocal) {
                gnss_status = 2;
            } else {
                gnss_status = 1;                
                memset(&gnss_data, 0, sizeof(gnss_data));
            }
        } else {
            memset(&gnss_data, 0, sizeof(gnss_data));
            gnss_status = 0;
        }
        memset(lat, 0, sizeof(lat));        
        memset(lon, 0, sizeof(lon));
        if (gnss_data.m_bLocal) {
            if (strlen(gnss_data.RmcFields[ED_LATITUDE]) > 0) {
                index = 0;
                if (gnss_data.RmcFields[ED_LATITUDE_TYPE][0] == 'S') {
                    lat[0] = '-';
                    index = 1;
                }
                for (i = 0;  i < strlen(gnss_data.RmcFields[ED_LATITUDE]); i++) {
                    if (gnss_data.RmcFields[ED_LATITUDE][i] != '.') {
                        lat[index++] = gnss_data.RmcFields[ED_LATITUDE][i];
                    }
                }
            }
            if (strlen(gnss_data.RmcFields[ED_LONGTITUDE]) > 0) {
                index = 0;
                if (gnss_data.RmcFields[ED_LONGTITUDE_TYPE][0] == 'W') {
                    lon[0] = '-';
                    index = 1;
                }
                for (i = 0;  i < strlen(gnss_data.RmcFields[ED_LONGTITUDE]); i++) {
                    if (gnss_data.RmcFields[ED_LONGTITUDE][i] != '.') {
                        lon[index++] = gnss_data.RmcFields[ED_LONGTITUDE][i];
                    }
                }
            }
        }
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp) - 1, "%d,%d,%d,%02d%02d%02d%02d,%s,%s,%s,%s,%c%c%c%c,%d,%.2f,%d,%.1f\n",
                                10,
                                (linux_time.sec + linux_time.usec / 1000) - (t0_time.sec + t0_time.usec / 1000),
                                gnss_status,//poistion state
                                gnss_data.m_nHour, gnss_data.m_nMinute, gnss_data.m_nSecond, 0,//"15360420",//time
                                lat, lon,//lat, lon
                                gnss_data.RmcFields[ED_SPEED],//speed
                                gnss_data.RmcFields[ED_COURSE],//cource
                                'A', 'N', 'N', 'N',//pos_mode
                                gnss_data.m_nSatNumInView,//num_sv
                                gnss_data.m_nHdop,//hdop
                                (int)(gnss_data.m_nAltitude * 10),//altitude
                                gnss_data.m_nGeoidHeight//geoid_sep
                                );
        strcat(line, tmp);

        gnss_record_control(GNSS_RECORD_FILE_WRITE, NULL, line);
        sleep(1);
    }

    return NULL;
}

int gnss_record_task_start(void)
{
    unsigned int ret = 0;

    running = 1;
    ret = AmbaKAL_TaskCreate(&gnss_record_task,
                            "gnss_record_task",
                            80,
                            (AMBA_KAL_TASK_ENTRY_f)gnss_record_task_entry,
                            0,
                            gnss_record_task_stack,
                            GNSS_RECORD_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("gnss_record_task create fail");
        running = 0;
        return -1;
    }

    return 0;
}

int gnss_record_task_stop(void)
{
    running = 0;

    return 0;
}

