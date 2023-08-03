#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include <time.h>

static pthread_mutex_t lock;

int log_init(void)
{
        pthread_mutex_init(&lock, NULL);
        return 0;
}

int log_shutdown(void)
{
        return 0;
}

int log_msg(int level, const char *fmt, ...)
{
        va_list args;
        int ret;
        time_t rawtime;
        struct tm * timeinfo;
        char header [80];

        time (&rawtime);
        timeinfo = localtime (&rawtime);
        strftime(header, sizeof(header), "%H:%M:%S ", timeinfo);

        pthread_mutex_lock(&lock);
        printf("%s", header);
        va_start(args, fmt);
        ret = vprintf(fmt, args);
        va_end(args);
        pthread_mutex_unlock(&lock);

        return ret;
}
