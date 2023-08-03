#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <pthread.h>
#include <syslog.h>

#if 1
static inline void debug_line(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    openlog(NULL, LOG_PID | LOG_CONS, LOG_USER);
    vsyslog(LOG_DEBUG, format, ap);
    closelog();
    va_end(ap);
}
#else
static inline const char *format_time(void)
{
    struct timeval tv;    
    struct tm *tm_info = NULL;
    static char buf[64];
    
    memset(buf, 0, sizeof(buf));
    
    // clock time
    if (gettimeofday(&tv, NULL) == -1) {
        return buf;
    }
    
    // to calendar time
    if ((tm_info = localtime((const time_t*)&tv.tv_sec)) == NULL) {
        return buf;
    }

    sprintf(buf, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]",
        1900 + tm_info->tm_year, 1 + tm_info->tm_mon, tm_info->tm_mday,
        tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec,
        (int)(tv.tv_usec / 1000));

    return buf;
}

static inline void debug_line(const char *format, ...)
{
    va_list ap;
    fprintf(stderr, "%s: ", format_time());
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fputc('\n', stderr);
    fflush(stderr);
}
#endif

#endif//__DEBUG_H__

