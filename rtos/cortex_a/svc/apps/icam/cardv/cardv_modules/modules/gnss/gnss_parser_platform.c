#include <app_base.h>

static AMBA_KAL_MUTEX_t mutex;

int gnss_parser_init_muxer(void)
{
    if (AmbaKAL_MutexCreate(&mutex, "gps_parser") != OK) {
        debug_line("%s create mutex fail", __func__);
        return -1;
    }

    return 0;
}

int gnss_parser_get_muxer(void)
{
    AmbaKAL_MutexTake(&mutex, AMBA_KAL_WAIT_FOREVER);

    return 0;
}

int gnss_parser_release_muxer(void)
{
    AmbaKAL_MutexGive(&mutex);

    return 0;
}

unsigned int gnss_parser_get_tickcount(void)
{
    unsigned int count = 0;

    AmbaKAL_GetSysTickCount(&count);

    return count;
}

