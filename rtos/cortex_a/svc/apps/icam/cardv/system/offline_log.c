#include "app_base.h"
#include "offline_log.h"

static AMBA_KAL_MUTEX_t mutex_log, mutex_log2;
#define POWER_ON_LOG_BUF_SIZE (256 * 1024)
static unsigned char power_on_log_buf[POWER_ON_LOG_BUF_SIZE] = {0};
static unsigned int power_on_log_buf_len = 0;

#define LASTEST_LOG_BUF_SIZE (256 * 1024)
static unsigned char lastest_log_buf[LASTEST_LOG_BUF_SIZE] = {0};
static unsigned int lastest_log_buf_is_looped = 0;
static unsigned int lastest_log_wr_index = 0;

#define LOG_SEPARATER "\r\n\r\n\r\n--------------------------------the latest log--------------------------------\r\n\r\n\r\n"

static int offline_log_init(void)
{
    static int init_flag = 0;

    if (init_flag) {
        return 0;
    }
    if (AmbaKAL_MutexCreate(&mutex_log, "mutex_log") != KAL_ERR_NONE) {
        debug_line("%s create mutex failed", __func__);
        return -1;
    }
    init_flag = 1;

    return 0;
}

unsigned char *offline_log_get_power_on_log_buffer(unsigned int *len)
{
    if (len == NULL) {
        return NULL;
    }
    *len = power_on_log_buf_len;
    return power_on_log_buf;
}

int offline_log_get_mutex(void)
{
    AmbaKAL_MutexTake(&mutex_log, AMBA_KAL_WAIT_FOREVER);

    return 0;
}

int offline_log_release_mutex(void)
{
    AmbaKAL_MutexGive(&mutex_log);

    return 0;
}

int offline_log_append(const unsigned char *line, unsigned int len)
{
    offline_log_init();
    AmbaKAL_MutexTake(&mutex_log, AMBA_KAL_WAIT_FOREVER);
    if (power_on_log_buf_len + len <= POWER_ON_LOG_BUF_SIZE) {
        memcpy(power_on_log_buf + power_on_log_buf_len, line, len);
        power_on_log_buf_len += len;
    } else {
        if (lastest_log_wr_index + len > LASTEST_LOG_BUF_SIZE) {
            lastest_log_buf_is_looped = 1;
            memcpy(lastest_log_buf + lastest_log_wr_index, line, (LASTEST_LOG_BUF_SIZE - lastest_log_wr_index));
            memcpy(lastest_log_buf, line + LASTEST_LOG_BUF_SIZE - lastest_log_wr_index, len - (LASTEST_LOG_BUF_SIZE - lastest_log_wr_index));
            lastest_log_wr_index = len - (LASTEST_LOG_BUF_SIZE - lastest_log_wr_index);
        } else {
            memcpy(lastest_log_buf + lastest_log_wr_index, line, len);
            lastest_log_wr_index += len;
        }
    }
    AmbaKAL_MutexGive(&mutex_log);

    return 0;
}

int offline_log_dump(const char *path)
{
    AMBA_FS_FILE *pFile = NULL;
    int rval = -1;

    if (path == NULL || strlen(path) <= 0) {
        return -1;
    }
    offline_log_init();
    AmbaPrint_Flush();//flush log buffer
    AmbaKAL_MutexTake(&mutex_log, AMBA_KAL_WAIT_FOREVER);
    pFile = fopen(path, "w");
    if (pFile) {
        if (power_on_log_buf_len > 0) {
            fwrite(power_on_log_buf, power_on_log_buf_len, 1, pFile);
        }
        if (lastest_log_buf_is_looped) {
            fwrite(LOG_SEPARATER, strlen(LOG_SEPARATER), 1, pFile);
            if (lastest_log_wr_index < LASTEST_LOG_BUF_SIZE) {
                fwrite(lastest_log_buf + lastest_log_wr_index, (LASTEST_LOG_BUF_SIZE - lastest_log_wr_index), 1, pFile);
            }
            fwrite(lastest_log_buf, lastest_log_wr_index, 1, pFile);
        } else {
            if (lastest_log_wr_index > 0) {
                fwrite(LOG_SEPARATER, strlen(LOG_SEPARATER), 1, pFile);
                fwrite(lastest_log_buf, lastest_log_wr_index, 1, pFile);
            }
        }
        fflush(pFile);
        fclose(pFile);
        rval = 0;
    }
    AmbaKAL_MutexGive(&mutex_log);

    return rval;
}

static FILE *pFile = NULL;
static int file_log_init(void)
{
#define FILE_LOG_PATH     FILE_LOG_DISK":\\logs"
    static int flag = 0;
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    if (flag == 0) {
        if (AmbaKAL_MutexCreate(&mutex_log2, "mutex_log2") != KAL_ERR_NONE) {
            debug_line("%s create mutex failed", __func__);
            return -1;
        }
        flag = 1;
    }
    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->DebugMode && pFile == NULL) {
        char path[64] = {0};
        time_s cur_time = {0};

        AmbaFS_MakeDir(FILE_LOG_PATH);
        time_now(&cur_time);
        memset(path, 0, sizeof(path));
        snprintf(path, sizeof(path) - 1, "%s\\rtos_%.4d%.2d%.2d_%.2d%.2d%.2d.log", FILE_LOG_PATH, cur_time.year, cur_time.month, cur_time.day,
                                                                             cur_time.hour, cur_time.minute, cur_time.second);
        pFile = fopen(path, "w");
        if (pFile == NULL) {
            return -1;
        }
        debug_line("%s open %s failed", __func__, path);
    }

    return 0;
}

int file_log_append(const unsigned char *line, unsigned int len)
{
    time_s cur_time;

    if (app_helper.sd_init_done && AmbaSD_IsCardPresent(SD_CHANNEL)) {
        file_log_init();
        AmbaKAL_MutexTake(&mutex_log2, AMBA_KAL_WAIT_FOREVER);
        if (pFile == NULL) {
            AmbaKAL_MutexGive(&mutex_log2);
            return -1;
        }
        time_now(&cur_time);
        fwrite(line, len, 1, pFile);
        fflush(pFile);
        AmbaKAL_MutexGive(&mutex_log2);
    }

    return 0;
}

int file_log_close(void)
{
    AmbaKAL_MutexTake(&mutex_log2, AMBA_KAL_WAIT_FOREVER);
    if (pFile == NULL) {
        AmbaKAL_MutexGive(&mutex_log2);
        return -1;
    }
    fflush(pFile);
    fclose(pFile);
    pFile = NULL;
    AmbaKAL_MutexGive(&mutex_log2);

    return 0;
}

