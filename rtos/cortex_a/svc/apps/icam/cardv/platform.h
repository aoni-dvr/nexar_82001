#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "SvcErrCode.h"
#include <AmbaWrap.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdarg.h>
#include "AmbaPrint.h"
#include "AmbaGPIO.h"
#include <AmbaUtility.h>
#include <AmbaI2C.h>
#include <AmbaKAL.h>
#include <AmbaFS.h>
#include <bsp.h>

#define assert(condition) ((void)0)

static inline int debug_line(const char *fmt, ...);

typedef struct {
    unsigned int year;
    unsigned int month;
    unsigned int day;
    unsigned int weekday;
    unsigned int hour;
    unsigned int minute;
    unsigned int second;
#if defined(CONFIG_GNSS_PPS_TIME_SYNC_ENABLE)
    unsigned int msecond;
#endif
} time_s;

struct stat {
    unsigned int attr;
    unsigned long long size;
    time_s time_modify;
    time_s time_create;
};

#ifndef NULL
#define NULL ((void*)0)
#endif

#undef SEEK_CUR
#define SEEK_CUR PF_SEEK_CUR
typedef void FILE;

extern char *strchr(const char *s, int c);
extern void *memchr(const void *s, int c, size_t n);
extern void *memset(void *, int, size_t);

typedef long time_t;
typedef unsigned int size_t;

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

static inline unsigned int tick(void)
{
    unsigned int count;
    AmbaKAL_GetSysTickCount(&count);
    return count;
}

static inline int tolower(int c)
{
    if (c >= 'A' && c <= 'Z') {
        return c + 'a' - 'A';
    }

    return c;
}

static inline int toupper(int c)
{
    if (c >= 'a' && c <= 'z') {
        return c + 'A' - 'a';
    }

    return c;
}

static inline int isalpha(int c)
{
    if (c >= 'A' && c <= 'Z') {
        return 1;
    }
    if (c >= 'a' && c <= 'z') {
        return 1;
    }

    return 0;
}

static inline unsigned int strlen(const char *p)
{
    int len = 0;

    if (p == NULL) {
        len = 0;
    } else {
        while (p[len] != '\0') {
            len++;
        }
    }
    return len;
}

static inline int memcmp(const void *s1, const void *s2, size_t n)
{
    int v = 0;

    (void)AmbaWrap_memcmp(s1, s2, n, &v);

    return v;
}

static inline void *memcpy(void *dest, const void *src, size_t size)
{
    AmbaWrap_memcpy(dest, src, size);
    return dest;
}

static char *strncpy(char *dest, const char *src, size_t len)
{
    size_t i = 0;

    if (dest == NULL || src == NULL || strlen(src) <= 0 || len <= 0) {
        return NULL;
    }

    for (i = 0; i < len; i++) {
        *(dest + i) = *(src + i);
    }

    return dest;
}

static inline char *strcpy(char *dest, const char *src)
{
    return strncpy(dest, src, strlen(src));
}

static inline unsigned int _pow(unsigned int value, int bit)
{
    int i = 0;
    unsigned int target_value = 1;

    if (bit == 0) {
        return 1;
    }

    for (i = 0; i < bit; i++) {
        target_value *= value;
    }

    return target_value;
}

static inline int atoi(const char *src)
{
    int i = 0, bit = 0;
    int value = 0;
    int base = 10;

    if (src == NULL || strlen(src) <= 0) {
        return 0;
    }

    for (i = (int)(strlen(src) - 1); i >= 0; i--) {
        char tmp = *(src + i);
        if (tmp >= '0' && tmp <= '9') {
            value += ((tmp - '0') * (int)(_pow(base, bit)));
        }
        bit += 1;
    }

    if (src[0] == '-') {
        value = -value;
    }

    return value;

}

static inline double atof(const char *src)
{
    double num = 0.0;
    int n = 0;
    int count = 1;

    while (*src) {
        if ((*src >= '0') && (*src <= '9')) {
            num = num * 10 + (*src - '0');
            src++;
            if (n) {
                count *= 10;
            }
        } else if (*src == '.') {
            n = 1;
            src++;
        } else {
            num = 0;
            break;
        }
    }
    if (n) {
        num = num / count;
    }
    return num;
}

static inline int abs(int value)
{
    if (value < 0) {
        return -value;
    }
    return value;
}

static inline int isdigit(int c)
{
    if (c >= 0 && c <= 9) {
        return 1;
    }
    return 0;
}

static inline char *strcat(char *p1, const char *p2)
{
    if (p1 == NULL || p2 == NULL) {
        return p1;
    }
    if (strlen(p2) <= 0) {
        *(p1 + strlen(p1)) = '\0';
        return p1;
    }
    memcpy(p1 + strlen(p1), p2, strlen(p2));

    return p1;
}

static inline char *strncat(char *p1, const char *p2, size_t len)
{
    if (p1 == NULL || p2 == NULL) {
        return p1;
    }
    if (strlen(p2) <= 0) {
        *(p1 + strlen(p1)) = '\0';
        return p1;
    }
    memcpy(p1 + strlen(p1), p2, len);

    return p1;
}

static inline int strncmp(const char *p1, const char *p2, unsigned int len)
{
    int nRet = 0;

    if ((p1 == NULL) || (p2 == NULL) || (len <= 0)) {
        nRet = -1;
    } else {
        unsigned int i = 0;
        if (len > (strlen(p1) + 1) || len > (strlen(p2) + 1)) {
            return -1;
        }
        for (i = 0; i < len; i++) {
            if (p1[i] != p2[i]) {
                if (p1[i] < p2[i]) {
                    nRet = -1;
                } else {
                    nRet = 1;
                }
                break;
            }
        }
    }

    return nRet;
}

static inline int strncasecmp(const char *p1, const char *p2, unsigned int len)
{
    int nRet = 0;

    if ((p1 == NULL) || (p2 == NULL) || (len <= 0)) {
        nRet = -1;
    } else {
        unsigned int i = 0;
        if (len > (strlen(p1) + 1) || len > (strlen(p2) + 1)) {
            return -1;
        }
        for (i = 0; i < len; i++) {
            if (tolower(p1[i]) != tolower(p2[i])) {
                if (tolower(p1[i]) < tolower(p2[i])) {
                    nRet = -1;
                } else {
                    nRet = 1;
                }
                break;
            }
        }
    }

    return nRet;
}

static inline int strcmp(const char *p1, const char *p2)
{
    int len1 = 0, len2 = 0, len = 0;

    len1 = strlen(p1);
    len2 = strlen(p2);
    if (len1 > len2) {
        len = len1;
    } else {
        len = len2;
    }

    return strncmp(p1, p2, len);
}

static inline char *strstr(const char *src, const char *sub)
{
    int src_len = 0, sub_len = 0;
    int i = 0;

    if (src == NULL || sub == NULL) {
        return NULL;
    }

    if (strcmp(src + i, sub) == 0) {
        return (char *)src;
    }

    src_len = strlen(src);
    sub_len = strlen(sub);
    if (sub_len > src_len) {
        return NULL;
    }

    for (i = 0; i <= (src_len - sub_len); i++) {
        if (strncmp(src + i, sub, sub_len) == 0) {
            return (char *)(src + i);
        }
    }

    return NULL;
}

static inline unsigned int strspn(const char *src, const char *sub)
{
    unsigned int i = 0, j = 0;
    unsigned int count = 0;

    if (src == NULL || sub == NULL) {
        return 0;
    }

    if (strlen(src) <= 0 || strlen(sub) <= 0) {
        return 0;
    }

    for (i = 0; i < strlen(src); i++) {
        for (j = 0; j < strlen(sub); j++) {
            if (*(src + i) == *(sub + j)) {
                count += 1;
                break;
            }
        }
    }

    return count;
}

static inline unsigned long strtoul(const char *src, char **end, int base)
{
    int i = 0, bit = 0;
    unsigned long value = 0;

    (void)end;
    if (src == NULL || strlen(src) <= 0) {
        return 0;
    }

    for (i = strlen(src) - 1; i >= 0; i--) {
        char tmp = *(src + i);
        if (tmp >= '0' && tmp <= '9') {
            value += ((tmp - '0') * _pow(base, bit));
        } else if (tmp >= 'a' && tmp <= 'f') {
            value += ((tmp - 'a' + 10) * _pow(base, bit));
        } else if (tmp >= 'A' && tmp <= 'F') {
            value += ((tmp - 'A' + 10) * _pow(base, bit));
        }
        bit += 1;
    }

    return value;
}

static inline char *itoa(int i, char *str)
{
    int mod = 0, div = 0, index = 0;
    char *start = NULL, *end = NULL;
    char tmp = 0;

    div = i;
    if (div < 0) {
        div = -div;
    }
    do {
        mod = div % 10;
        if (mod < 10) {
            str[index++] = '0' + mod;
        }
        div = div / 10;
    } while (div != 0);
    if (i < 0) {
        str[index++] = '-';
    }
    str[index] = '\0';

    for (start = str, end = str + strlen(str) - 1; start < end; start++, end--) {
        tmp = *start;
        *start = *end;
        *end = tmp;
    }

    return str;
}

static inline char *dtoa(double d, char *str, int len)
{
    int mod = 0, div = 0, index = 0, i = 0;
    char *start = NULL, *end = NULL;
    char tmp = 0;
    double small_div = 0.0;
    double abs_d = (d < 0) ? -d : d;

    //handler int part
    small_div = (abs_d - (int)abs_d);
    for (i = 0; i < len; i++) {
        small_div *= 10;
    }
    div = (int)small_div;
    for (i = 0; i < len; i++) {
        mod = div % 10;
        if (mod < 10) {
            str[index++] = '0' + mod;
        }
        div = div / 10;
    }
    str[index++] = '.';

    //handler int part
    div = (int)abs_d;
    if (div < 0) {
        div = -div;
    }
    do {
        mod = div % 10;
        if (mod < 10) {
            str[index++] = '0' + mod;
        }
        div = div / 10;
    } while (div != 0);
    if (d < 0) {
        str[index++] = '-';
    }
    str[index] = '\0';

    for (start = str, end = str + strlen(str) - 1; start < end; start++, end--) {
        tmp = *start;
        *start = *end;
        *end = tmp;
    }

    return str;
}

static inline char *itohex(unsigned int i, char *str, int upper)
{
    int mod = 0, index = 0;
    char *start = NULL, *end = NULL;
    char tmp = 0;

    do {
        mod = i % 16;
        if (mod < 10) {
            str[index++] = '0' + mod;
        } else {
            if (upper) {
                str[index++] = 'A' + mod - 10;
            } else {
                str[index++] = 'a' + mod - 10;
            }
        }
        i = i / 16;
    } while (i != 0);
    str[index] = '\0';

    for (start = str, end = str + strlen(str) - 1; start < end; start++, end--) {
        tmp = *start;
        *start = *end;
        *end = tmp;
    }

    return str;
}

static inline int sprintf(char *str, const char *fmt, ...)
{
    va_list args;
    int i = 0, len = 0;
    int index = 0;
    char *str_tmp = NULL;
    int m = 1;
    int start_index = 0;
    int found = 0;

    va_start(args, fmt);
    len = strlen(fmt);
    for (i = 0; i < len; i++) {
        if (fmt[i] != '%') {
            str[index++] = fmt[i];
        } else {
            m = 1;
            switch (fmt[i + 1]) {
            case 's':
            case 'S':
                str_tmp = va_arg(args, char *);
                strcpy(str + index, str_tmp);
                index += strlen(str_tmp);
                i++;
                break;
            case 'c':
            case 'C':
                {
                    int value = va_arg(args, int);
                    *(str + index) = value & 0xFF;
                    index += 1;
                    i++;
                }
                break;
            default:
                start_index = i + 1;
                while (fmt[i + m] != '\0') {
                    if (fmt[i + m] == 'd' || fmt[i + m] == 'D'
                        || fmt[i + m] == 'x' || fmt[i + m] == 'X'
                        || fmt[i + m] == 'f' || fmt[i + m] == 'F') {
                        char i_str[1024] = {0};

                        found = 1;
                        memset(i_str, 0, sizeof(i_str));
                        if (fmt[i + m] == 'd' || fmt[i + m] == 'D') {
                            int value = va_arg(args, int);
                            str_tmp = itoa(value, i_str);
                        } else if (fmt[i + m] == 'x') {
                            unsigned int value = va_arg(args, unsigned int);
                            str_tmp = itohex(value, i_str, 0);
                        } else if (fmt[i + m] == 'X') {
                            unsigned int value = va_arg(args, unsigned int);
                            str_tmp = itohex(value, i_str, 1);
                        }
                        if (m > 1) {
                            char tmp[128] = {0};
                            unsigned int tmp_len = 0;

                            memset(tmp, 0, sizeof(tmp));
                            memcpy(tmp, fmt + start_index, m - 1);
                            if (tmp[0] >= '0' && tmp[0] <= '9') {
                                tmp_len = atoi(tmp);
                            } else {
                                tmp_len = atoi(tmp + 1);
                            }
                            if (fmt[i + m] == 'f' || fmt[i + m] == 'F') {
                                double value = va_arg(args, double);
                                str_tmp = dtoa(value, i_str, tmp_len);
                                strcat(str, str_tmp);
                                index += strlen(str_tmp);
                            } else {
                                if (strlen(str_tmp) < tmp_len) {
                                    unsigned int k = 0;
                                    for (k = 0; k < (tmp_len - strlen(str_tmp)); k++) {
                                        if (fmt[start_index] == '.') {
                                            strcat(str, "0");
                                        } else {
                                            strcat(str, " ");
                                        }
                                    }
                                    strcat(str, str_tmp);
                                    index += tmp_len;
                                } else {
                                    strcat(str, str_tmp);
                                    index += strlen(str_tmp);
                                }
                            }
                        } else {
                            if (fmt[i + m] == 'f' || fmt[i + m] == 'F') {
                                double value = va_arg(args, double);
                                str_tmp = dtoa(value, i_str, 6);
                            }
                            strcat(str, str_tmp);
                            index += strlen(str_tmp);
                        }
                        i += m;
                        break;
                    }
                    m++;
                }
                if (found == 0) {
                    str[index++] = fmt[i];
                }
                break;
            }
        }
    }

    str[index] = '\0';
    va_end(args);

    return 0;
}

static inline int snprintf(char *str, size_t size, const char *fmt, ...)
{
    va_list args;
    int i = 0, len = 0;
    int index = 0;
    char *str_tmp = NULL;
    int m = 1;
    int start_index = 0;
    int found = 0;

    (void)size;
    va_start(args, fmt);
    len = strlen(fmt);
    for (i = 0; i < len; i++) {
        if (fmt[i] != '%') {
            str[index++] = fmt[i];
        } else {
            m = 1;
            switch (fmt[i + 1]) {
            case 's':
            case 'S':
                str_tmp = va_arg(args, char *);
                strcpy(str + index, str_tmp);
                index += strlen(str_tmp);
                i++;
                break;
            case 'c':
            case 'C':
                {
                    int value = va_arg(args, int);
                    *(str + index) = value & 0xFF;
                    index += 1;
                    i++;
                }
                break;
            default:
                start_index = i + 1;
                while (fmt[i + m] != '\0') {
                    if (fmt[i + m] == 'd' || fmt[i + m] == 'D'
                        || fmt[i + m] == 'x' || fmt[i + m] == 'X'
                        || fmt[i + m] == 'f' || fmt[i + m] == 'F') {
                        char i_str[1024] = {0};

                        found = 1;
                        memset(i_str, 0, sizeof(i_str));
                        if (fmt[i + m] == 'd' || fmt[i + m] == 'D') {
                            int value = va_arg(args, int);
                            str_tmp = itoa(value, i_str);
                        } else if (fmt[i + m] == 'x') {
                            unsigned int value = va_arg(args, int);
                            str_tmp = itohex(value, i_str, 0);
                        } else if (fmt[i + m] == 'X') {
                            unsigned int value = va_arg(args, int);
                            str_tmp = itohex(value, i_str, 1);
                        }
                        if (m > 1) {
                            char tmp[128] = {0};
                            unsigned int tmp_len = 0;

                            memset(tmp, 0, sizeof(tmp));
                            memcpy(tmp, fmt + start_index, m - 1);
                            if (tmp[0] >= '0' && tmp[0] <= '9') {
                                tmp_len = atoi(tmp);
                            } else {
                                tmp_len = atoi(tmp + 1);
                            }
                            if (fmt[i + m] == 'f' || fmt[i + m] == 'F') {
                                double value = va_arg(args, double);
                                str_tmp = dtoa(value, i_str, tmp_len);
                                strcat(str, str_tmp);
                                index += strlen(str_tmp);
                            } else {
                                if (strlen(str_tmp) < tmp_len) {
                                    unsigned int k = 0;
                                    for (k = 0; k < (tmp_len - strlen(str_tmp)); k++) {
                                        if (fmt[start_index] == '.' || fmt[start_index] == '0') {
                                            strcat(str, "0");
                                        } else {
                                            strcat(str, " ");
                                        }
                                    }
                                    strcat(str, str_tmp);
                                    index += tmp_len;
                                } else {
                                    strcat(str, str_tmp);
                                    index += strlen(str_tmp);
                                }
                            }
                        } else {
                            if (fmt[i + m] == 'f' || fmt[i + m] == 'F') {
                                double value = va_arg(args, double);
                                str_tmp = dtoa(value, i_str, 6);
                            }
                            strcat(str, str_tmp);
                            index += strlen(str_tmp);
                        }
                        i += m;
                        break;
                    }
                    m++;
                }
                if (found == 0) {
                    str[index++] = fmt[i];
                }
                break;
            }
        }
    }

    str[index] = '\0';
    va_end(args);

    return 0;
}

static inline int debug_line(const char *fmt, ...)
{
    va_list args;
    int i = 0, len = 0;
    int index = 0;
    char *str_tmp = NULL;
    int m = 1;
    int start_index = 0;
    int found = 0;
    char str[4096] = {0};

    memset(str, 0, sizeof(str));
    va_start(args, fmt);
    len = strlen(fmt);
    for (i = 0; i < len; i++) {
        if (fmt[i] != '%') {
            str[index++] = fmt[i];
        } else {
            m = 1;
            switch (fmt[i + 1]) {
            case 's':
            case 'S':
                str_tmp = va_arg(args, char *);
                strcpy(str + index, str_tmp);
                index += strlen(str_tmp);
                i++;
                break;
            case 'c':
            case 'C':
                {
                    int value = va_arg(args, int);
                    *(str + index) = value & 0xFF;
                    index += 1;
                    i++;
                }
                break;
            default:
                start_index = i + 1;
                while (fmt[i + m] != '\0') {
                    if (fmt[i + m] == 'd' || fmt[i + m] == 'D'
                        || fmt[i + m] == 'x' || fmt[i + m] == 'X'
                        || fmt[i + m] == 'f' || fmt[i + m] == 'F') {
                        char i_str[1024] = {0};

                        found = 1;
                        memset(i_str, 0, sizeof(i_str));
                        if (fmt[i + m] == 'd' || fmt[i + m] == 'D') {
                            int value = va_arg(args, int);
                            str_tmp = itoa(value, i_str);
                        } else if (fmt[i + m] == 'x') {
                            unsigned int value = va_arg(args, unsigned int);
                            str_tmp = itohex(value, i_str, 0);
                        } else if (fmt[i + m] == 'X') {
                            unsigned int value = va_arg(args, unsigned int);
                            str_tmp = itohex(value, i_str, 1);
                        }
                        if (m > 1) {
                            char tmp[128] = {0};
                            unsigned int tmp_len = 0;

                            memset(tmp, 0, sizeof(tmp));
                            memcpy(tmp, fmt + start_index, m - 1);
                            if (tmp[0] >= '0' && tmp[0] <= '9') {
                                tmp_len = atoi(tmp);
                            } else {
                                tmp_len = atoi(tmp + 1);
                            }
                            if (fmt[i + m] == 'f' || fmt[i + m] == 'F') {
                                double value = va_arg(args, double);
                                str_tmp = dtoa(value, i_str, tmp_len);
                                strcat(str, str_tmp);
                                index += strlen(str_tmp);
                            } else {
                                if (strlen(str_tmp) < tmp_len) {
                                    unsigned int k = 0;
                                    for (k = 0; k < (tmp_len - strlen(str_tmp)); k++) {
                                        if (fmt[start_index] == '.') {
                                            strcat(str, "0");
                                        } else {
                                            strcat(str, " ");
                                        }
                                    }
                                    strcat(str, str_tmp);
                                    index += tmp_len;
                                } else {
                                    strcat(str, str_tmp);
                                    index += strlen(str_tmp);
                                }
                            }
                        } else {
                            if (fmt[i + m] == 'f' || fmt[i + m] == 'F') {
                                double value = va_arg(args, double);
                                str_tmp = dtoa(value, i_str, 6);
                            }
                            strcat(str, str_tmp);
                            index += strlen(str_tmp);
                        }
                        i += m;
                        break;
                    }
                    m++;
                }
                if (found == 0) {
                    str[index++] = fmt[i];
                }
                break;
            }
        }
    }

    str[index] = '\0';
    va_end(args);
    AmbaPrint_PrintStr5(str, NULL, NULL, NULL, NULL, NULL);

    return 0;
}

static inline int stat(const char *path, struct stat *p_stat)
{
    AMBA_FS_FILE_INFO_s file_info;

    if (AmbaFS_GetFileInfo(path, &file_info) == AMBA_FS_ERR_NONE) {
        p_stat->attr = file_info.Attr;
        p_stat->size = file_info.Size;
        p_stat->time_modify.year    = file_info.TimeModify.Year;
        p_stat->time_modify.month   = file_info.TimeModify.Month;
        p_stat->time_modify.day     = file_info.TimeModify.Day;
        p_stat->time_modify.hour    = file_info.TimeModify.Hour;
        p_stat->time_modify.minute  = file_info.TimeModify.Minute;
        p_stat->time_modify.second  = file_info.TimeModify.Second;
        p_stat->time_modify.weekday = file_info.TimeModify.WeekDay;
        p_stat->time_create.year    = file_info.TimeCreate.Year;
        p_stat->time_create.month   = file_info.TimeCreate.Month;
        p_stat->time_create.day     = file_info.TimeCreate.Day;
        p_stat->time_create.hour    = file_info.TimeCreate.Hour;
        p_stat->time_create.minute  = file_info.TimeCreate.Minute;
        p_stat->time_create.second  = file_info.TimeCreate.Second;
        p_stat->time_create.weekday = file_info.TimeCreate.WeekDay;
        return 0;
    }

    return -1;
}

static inline int time_now(time_s *cur_time)
{
    AMBA_RTC_DATE_TIME_s Calendar;

    AmbaRTC_GetSysTime(&Calendar);
    cur_time->year    = Calendar.Year;
    cur_time->month   = Calendar.Month;
    cur_time->day     = Calendar.Day;
    cur_time->hour    = Calendar.Hour;
    cur_time->minute  = Calendar.Minute;
    cur_time->second  = Calendar.Second;

    return 0;
}

static inline int time_convert_12h(time_s *time, int *pm)
{
    if (time->hour >= 12) {
        *pm = 1;
    } else {
        *pm = 0;
    }
    if (time->hour == 0) {
        time->hour = 12;
    } else if (time->hour > 12) {
        time->hour = time->hour - 12;
    } else {
        time->hour = time->hour;
    }

    return 0;
}

static inline FILE *fopen(const char *path, const char *mode)
{
    AMBA_FS_FILE *pFile = NULL;

    if (AmbaFS_FileOpen(path, mode, &pFile) == AMBA_FS_ERR_NONE) {
        return pFile;
    }
    return NULL;
}

static inline int fread(void *buf, int size, int count, FILE *pFile)
{
    size_t num_read = 0;
    size_t Err = AmbaFS_FileRead(buf, size, count, pFile, &num_read);
    if (Err != AMBA_FS_ERR_NONE) {
        return -1;
    }
    return num_read;
}

static inline size_t fwrite(const void *buf, size_t size, size_t count, FILE *pFile)
{
    size_t num_write = 0;
    UINT32 Err = AmbaFS_FileWrite((void *)buf, size, count, pFile, &num_write);
    if (Err != AMBA_FS_ERR_NONE) {
        return -1;
    }
    return num_write;
}

static inline int fseek(FILE *pFile, int offset, int origin)
{
    UINT32 Err = AmbaFS_FileSeek(pFile, offset, origin);
    if (Err != AMBA_FS_ERR_NONE) {
        return -1;
    }
    return 0;
}

static inline int ferror(FILE *pFile)
{
    if (AmbaFS_GetFileError(pFile) == AMBA_FS_ERR_NONE) {
        return 0;
    }
    return 1;
}

static inline int fflush(FILE *pFile)
{
    AmbaFS_FileSync(pFile);

    return 0;
}

static inline int fclose(FILE *pFile)
{
    UINT32 Err = AmbaFS_FileClose(pFile);
    if (Err != AMBA_FS_ERR_NONE) {
        return -1;
    }
    return 0;
}

static inline int is_file_readonly(const char *path)
{
    AMBA_FS_FILE_INFO_s fileInfo;

	AmbaFS_GetFileInfo(path, &fileInfo);
    if (fileInfo.Attr & AMBA_FS_ATTR_RDONLY) {
        return 1;
    }
    return 0;
}

static inline int remove(const char *path)
{
    if (AmbaFS_Remove(path) == AMBA_FS_ERR_NONE) {
        return 0;
    }
    return -1;
}

static inline int delete_dir(const char *path)
{
    if (AmbaFS_DeleteDir(path) != 0) {
        debug_line("delete %s failed", path);
        return -1;
    }
    return 0;
}

static inline int clean_dir(const char *path)
{
    if (AmbaFS_CleanDir(path) != 0) {
        debug_line("clean %s failed", path);
        return -1;
    }
    return 0;
}

static inline int readline(FILE *pFile, char buf[])
{
    unsigned char ch;
    // Normal state
    do {
        if (fread(&ch, 1, 1, pFile) == 0) { // read 1 byte
            return -1;
        }
        if ( (ch == '\n') || (ch == '\r') ) {
            break;  // go to end-of-line status
        }
        *buf = ch;
        buf++;
    } while (1);
    // End of line state
    do {
        if (fread(&ch, 1, 1, pFile) == 0) { // read 1 byte
            break;
        }
        if ( (ch == '\n') || (ch == '\r') ) {
            /* do nothing */
        } else {
            // Reach to next line, roll back 1 byte
            fseek(pFile, -1, SEEK_CUR);
            break;
        }
    } while (1);
    *buf = '\0';
    while (strlen(buf) > 0) {
        if (buf[strlen(buf) - 1] == ' ') {
            buf[strlen(buf) - 1] = '\0';
        } else {
            break;
        }
    }

    return 0;
}

static inline void msleep(unsigned int m_seconds)
{
    if (m_seconds == 0) {
        return;
    }
    AmbaKAL_TaskSleep(m_seconds);
}

static inline void sleep(unsigned int seconds)
{
    unsigned int i = 0;

    if (seconds == 0) {
        return;
    }
    for (i = 0; i < seconds; i++) {
        AmbaKAL_TaskSleep(1000);
    }
}

#define exit(code) return

static inline int i2c_write(unsigned int channel, unsigned char slave_addr, const unsigned char *data_buf, unsigned int data_len)
{
    AMBA_I2C_TRANSACTION_s i2c_config;
    unsigned int actual_len = 0;
    int retry = 0;

    slave_addr &= 0xfe;
    i2c_config.SlaveAddr = slave_addr;
    i2c_config.pDataBuf = (unsigned char *)data_buf;
    i2c_config.DataSize  = data_len;
    while (retry < 3) {
        if (AmbaI2C_MasterWrite(channel, AMBA_I2C_SPEED_STANDARD, &i2c_config, &actual_len, 5000) != I2C_ERR_NONE) {            
            retry += 1;
            debug_line("i2c write fail: channel=%d, salve_addr=0x%.2X reg=0x%.2X, retry: %d", channel, slave_addr, data_buf[0], retry);
            continue;
        }
        break;
    }

    if (retry >= 3) {
        return -1;
    }

    return actual_len;
}

static inline int i2c_read_impl(unsigned int channel, unsigned char slave_addr, unsigned char reg, unsigned char *rx_data_buf, int rx_size)
{
    AMBA_I2C_TRANSACTION_s i2c_tx_config;
    AMBA_I2C_TRANSACTION_s i2c_rx_config;
    unsigned int actual_len = 0;
    int retry = 0;

    i2c_tx_config.SlaveAddr = slave_addr;
    i2c_tx_config.pDataBuf = &reg;
    i2c_tx_config.DataSize  = 1;

    i2c_rx_config.SlaveAddr = slave_addr | 0x01;
    i2c_rx_config.pDataBuf = rx_data_buf;
    i2c_rx_config.DataSize  = rx_size;
    while (retry < 3) {
        if (AmbaI2C_MasterReadAfterWrite(channel, AMBA_I2C_SPEED_STANDARD, 1, &i2c_tx_config, &i2c_rx_config, &actual_len, 5000) != I2C_ERR_NONE) {
            retry += 1;
            debug_line("i2c read fail: channel=%d, salve_addr=0x%.2X reg=0x%.2X, retry: %d", channel, slave_addr, reg, retry);
            continue;
        }
        break;
    }

    if (retry >= 3) {
        return -1;
    }

    return actual_len;
}

static inline int i2c_read2_impl(unsigned int channel, unsigned char slave_addr, unsigned char reg, unsigned char *rx_data_buf, int rx_size)
{
    AMBA_I2C_TRANSACTION_s i2c_tx_config;
    AMBA_I2C_TRANSACTION_s i2c_rx_config;
    unsigned int actual_len = 0;
    int retry = 0;

    i2c_tx_config.SlaveAddr = slave_addr;
    i2c_tx_config.pDataBuf = &reg;
    i2c_tx_config.DataSize  = 1;

    i2c_rx_config.SlaveAddr = slave_addr | 0x01;
    i2c_rx_config.pDataBuf = rx_data_buf;
    i2c_rx_config.DataSize  = rx_size;
    while (retry < 3) {
        if (AmbaI2C_MasterReadAfterWrite2(channel, AMBA_I2C_SPEED_STANDARD, 1, &i2c_tx_config, &i2c_rx_config, &actual_len, 5000) != I2C_ERR_NONE) {
            retry += 1;
            debug_line("i2c read2 fail: channel=%d, salve_addr=0x%.2X reg=0x%.2X, retry: %d", channel, slave_addr, reg, retry);
            continue;
        }
        break;
    }

    if (retry >= 3) {
        return -1;
    }

    return actual_len;
}

static inline int i2c_read(unsigned int channel, unsigned char slave_addr, unsigned char reg, unsigned char *rx_data_buf, int rx_size)
{
    slave_addr &= 0xfe;
    if (slave_addr == 0xc4) {
        unsigned int start_tick = tick();
        int rval = i2c_read2_impl(channel, slave_addr, reg, rx_data_buf, rx_size);        
        debug_line("mcu read reg, 0x%.2x=0x%.2x, used=%dms", reg, *rx_data_buf, tick() - start_tick);
        return rval;
    }
    return i2c_read_impl(channel, slave_addr, reg, rx_data_buf, rx_size);
}

static inline int i2c_write_reg(unsigned int channel, unsigned char slave_addr, unsigned char reg, unsigned char value)
{
    unsigned char tx_buf[2] = {0};

    tx_buf[0] = reg;
    tx_buf[1] = value;
    return i2c_write(channel, slave_addr, tx_buf, sizeof(tx_buf));
}

static inline int i2c_read_reg(unsigned int channel, unsigned char slave_addr, unsigned char reg, unsigned char *rx_data_buf)
{
    return i2c_read(channel, slave_addr, reg, rx_data_buf, 1);
}

static inline int i2c_modify_reg(unsigned int channel, unsigned char slave_addr, unsigned char reg, unsigned char mask, unsigned char value)
{
    unsigned char new_value = 0;

    if (i2c_read_reg(channel, slave_addr, reg, &new_value) < 0) {
        return -1;
    }
    new_value &= ~mask;
    new_value |= value;
    return i2c_write_reg(channel, slave_addr, reg, new_value);
}

static inline int rand(void)
{
    int value = 0;
    AmbaWrap_rand(&value);
    return value;
}

static inline int sscanf(const char *str, const char *format, ...)
{
    (void)str;
    (void)format;
    debug_line("%s not impl", __func__);AmbaPrint_Flush();
    return -1;
}

static inline void *memmove(void *dest, const void *src, size_t n)
{
    (void)dest;
    (void)src;
    (void)n;
    debug_line("%s not impl", __func__);AmbaPrint_Flush();
    return NULL;
}

static inline char *strtok(char *str, const char *delim)
{
    (void)str;
    (void)delim;
    debug_line("%s not impl", __func__);AmbaPrint_Flush();
    return NULL;
}

static inline char *strtok_r(char *str, const char *delim, char **saveptr)
{
    (void)str;
    (void)delim;
    (void)saveptr;
    debug_line("%s not impl", __func__);AmbaPrint_Flush();
    return NULL;
}

static inline struct tm *localtime(const time_t *timep)
{
    (void)timep;
    debug_line("%s not impl", __func__);AmbaPrint_Flush();
    return NULL;
}

static inline char *strchrnul(const char *s, int c)
{
    (void)s;
    (void)c;
    debug_line("%s not impl", __func__);AmbaPrint_Flush();
    return NULL;
}

static inline size_t strcspn(const char *s, const char *reject)
{
    (void)s;
    (void)reject;
    debug_line("%s not impl", __func__);AmbaPrint_Flush();
    return 0;
}

static inline char *strrchr(const char *s, int c)
{
    (void)s;
    (void)c;
    debug_line("%s not impl", __func__);AmbaPrint_Flush();
    return NULL;
}
#endif//__PLATFORM_H__

