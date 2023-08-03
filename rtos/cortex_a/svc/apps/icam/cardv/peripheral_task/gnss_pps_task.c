#include <app_base.h>

static AMBA_KAL_TASK_t gnss_pps_task;
#define GNSS_PPS_TASK_STACK_SIZE (32 * 1024)
static unsigned char gnss_pps_task_stack[GNSS_PPS_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static void *gnss_pps_task_entry(void *argv);

static AMBA_KAL_EVENT_FLAG_t SvcEventFlag;
#define GNSS_PPS_EVENT_FLAG            0x00000001UL
#define GNSS_MS_TIMER_EVENT_FLAG       0x00000002UL

static AMBA_KAL_TIMER_t timer;
static time_s last_time;
static unsigned int pps_diff = 0;
static unsigned int base_tick = 0;
static unsigned int pps_valid_tick = 0;
static int debug_enable = 0;

static void gnss_pps_handler(UINT32 GpioPinID, UINT32 UserArg)
{
    pps_valid_tick = tick();
    pps_diff = pps_valid_tick - base_tick;
    if (debug_enable) {
        AmbaKAL_EventFlagSet(&SvcEventFlag, GNSS_PPS_EVENT_FLAG);
    }
}

static void amba_kal_timer_handler(unsigned int tid)
{
    AmbaMisra_TouchUnused(&tid);
    AmbaKAL_EventFlagSet(&SvcEventFlag, GNSS_MS_TIMER_EVENT_FLAG);
}

int gnss_pps_task_start(void)
{
    unsigned int ret = 0;

    AmbaKAL_EventFlagCreate(&SvcEventFlag, "GnssPpsEventFlag");
    ret = AmbaKAL_TaskCreate(&gnss_pps_task,
                            "gnss_pps_task",
                            150,
                            (AMBA_KAL_TASK_ENTRY_f)gnss_pps_task_entry,
                            0,
                            gnss_pps_task_stack,
                            GNSS_PPS_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("gnss_pps_task create fail");
        return -1;
    }

    ret = AmbaKAL_TimerCreate(&timer,
                                NULL,
                                amba_kal_timer_handler,
                                0,
                                1,
                                1,
                                AMBA_KAL_AUTO_START);
    if (ret != KAL_ERR_NONE) {
        ret = -1;
        debug_line("<%s> timer create fail", __func__);
        return -1;
    }

    return 0;
}

int gnss_pps_task_set_debug(int enable)
{
    debug_enable = enable;

    return 0;
}

static int get_rtc_time(unsigned int *sec, unsigned int *usec)
{
    time_s time;
    AMBA_RTC_DATE_TIME_s rtc_time;

    time_now(&time);
    if (base_tick == 0) {
        time.msecond = 0;
    } else {
        time.msecond = tick() - base_tick;
    }
    rtc_time.Year = time.year;
    rtc_time.Month = time.month;
    rtc_time.Day = time.day;
    rtc_time.Hour = time.hour;
    rtc_time.Minute = time.minute;
    rtc_time.Second = time.second;
    if (sec != NULL) {
        AmbaTime_UtcDateTime2TimeStamp(&rtc_time, sec);
    }
    if (usec != NULL) {
        *usec = time.msecond * 1000;
    }

    return 0;
}

static unsigned char time_set = 0;
static unsigned char need_add = 0;
static unsigned int time_diff_sec = 0;
static unsigned int time_diff_usec = 0;
int gnss_set_time(unsigned int sec, unsigned int usec)
{
    unsigned int rtc_sec = 0, rtc_usec = 0;
    unsigned int target_sec = sec, target_usec = usec;

    get_rtc_time(&rtc_sec, &rtc_usec);
    if (target_sec > rtc_sec) {
        need_add = 1;
        if (target_usec >= rtc_usec) {
            time_diff_sec = target_sec - rtc_sec;
            time_diff_usec = target_usec - rtc_usec;
        } else {
            time_diff_sec = target_sec - rtc_sec - 1;
            time_diff_usec = 1000000 - (rtc_usec - target_usec);
        }
    } else if (target_sec < rtc_sec) {
        need_add = 0;
        if (rtc_usec >= target_usec) {
            time_diff_sec = rtc_sec - target_sec;
            time_diff_usec = rtc_usec - target_usec;
        } else {
            time_diff_sec = rtc_sec - target_sec - 1;
            time_diff_usec = 1000000 - (target_usec - rtc_usec);
        }
    } else {
        time_diff_sec = 0;
        if (target_usec > rtc_usec) {
            need_add = 1;                    
            time_diff_usec = target_usec - rtc_usec;
        } else {
            need_add = 0;
            time_diff_usec = rtc_usec - target_usec;
        }
    }
    debug_line("linux set time %d.%.6d, rtc time: %d.%.6d. need_add: %d, diff: %d.%.6d", target_sec, target_usec,
                                                            rtc_sec, rtc_usec,
                                                            need_add, time_diff_sec, time_diff_usec);
    time_set = 1;

    return 0;
}

int gnss_time_now(time_s *time)
{
    //raw time get from rtc
    time_now(time);
    if (base_tick == 0) {
        time->msecond = 0;
    } else {
        time->msecond = tick() - base_tick;
    }
    if (time_set) {
        if (need_add) {            
            unsigned int seconds = 0;
            AMBA_RTC_DATE_TIME_s rtc_time;
            rtc_time.Year = time->year;
            rtc_time.Month = time->month;
            rtc_time.Day = time->day;
            rtc_time.Hour = time->hour;
            rtc_time.Minute = time->minute;
            rtc_time.Second = time->second;
            AmbaTime_UtcDateTime2TimeStamp(&rtc_time, &seconds);
            if (time->msecond + (time_diff_usec / 1000) > 1000) {
                seconds += (time_diff_sec + 1);
                time->msecond = time->msecond + (time_diff_usec / 1000) - 1000;
            } else {
                seconds += time_diff_sec;
                time->msecond = time->msecond + (time_diff_usec / 1000);
            }
            AmbaTime_UtcTimeStamp2DateTime(seconds, &rtc_time);            
            time->year    = rtc_time.Year;
            time->month   = rtc_time.Month;
            time->day     = rtc_time.Day;
            time->hour    = rtc_time.Hour;
            time->minute  = rtc_time.Minute;
            time->second  = rtc_time.Second;
        } else {
            unsigned int seconds = 0;
            AMBA_RTC_DATE_TIME_s rtc_time;
            rtc_time.Year = time->year;
            rtc_time.Month = time->month;
            rtc_time.Day = time->day;
            rtc_time.Hour = time->hour;
            rtc_time.Minute = time->minute;
            rtc_time.Second = time->second;
            AmbaTime_UtcDateTime2TimeStamp(&rtc_time, &seconds);
            if (time->msecond < (time_diff_usec / 1000)) {
                seconds -= time_diff_sec;
                seconds -= 1;
                time->msecond = 1000 - ((time_diff_usec / 1000) - time->msecond);
            } else {
                seconds -= time_diff_sec;
                time->msecond = time->msecond - (time_diff_usec / 1000);
            }
            AmbaTime_UtcTimeStamp2DateTime(seconds, &rtc_time);            
            time->year    = rtc_time.Year;
            time->month   = rtc_time.Month;
            time->day     = rtc_time.Day;
            time->hour    = rtc_time.Hour;
            time->minute  = rtc_time.Minute;
            time->second  = rtc_time.Second;
        }
    }
    if (pps_valid_tick > 0) {
        if (pps_diff > 500) {//treat rtc run slow, add (1000 - pps_diff)
            if (time->msecond + (1000 - pps_diff) > 1000) {
                unsigned int seconds = 0;
                AMBA_RTC_DATE_TIME_s rtc_time;
                rtc_time.Year = time->year;
                rtc_time.Month = time->month;
                rtc_time.Day = time->day;
                rtc_time.Hour = time->hour;
                rtc_time.Minute = time->minute;
                rtc_time.Second = time->second;
                AmbaTime_UtcDateTime2TimeStamp(&rtc_time, &seconds);
                seconds += 1;
                AmbaTime_UtcTimeStamp2DateTime(seconds, &rtc_time);
                time->year    = rtc_time.Year;
                time->month   = rtc_time.Month;
                time->day     = rtc_time.Day;
                time->hour    = rtc_time.Hour;
                time->minute  = rtc_time.Minute;
                time->second  = rtc_time.Second;
                time->msecond = time->msecond + (1000 - pps_diff) - 1000;
            } else {
                time->msecond = time->msecond + (1000 - pps_diff);
            }
        } else {//treat rtc run fast, sub pps_diff
            if (time->msecond < pps_diff) {
                unsigned int seconds = 0;
                AMBA_RTC_DATE_TIME_s rtc_time;
                rtc_time.Year = time->year;
                rtc_time.Month = time->month;
                rtc_time.Day = time->day;
                rtc_time.Hour = time->hour;
                rtc_time.Minute = time->minute;
                rtc_time.Second = time->second;
                AmbaTime_UtcDateTime2TimeStamp(&rtc_time, &seconds);
                seconds -= 1;
                AmbaTime_UtcTimeStamp2DateTime(seconds, &rtc_time);
                time->year    = rtc_time.Year;
                time->month   = rtc_time.Month;
                time->day     = rtc_time.Day;
                time->hour    = rtc_time.Hour;
                time->minute  = rtc_time.Minute;
                time->second  = rtc_time.Second;
                time->msecond = 1000 - (pps_diff - time->msecond);
            } else {
                time->msecond = time->msecond - pps_diff;
            }
        }
    }

    return 0;
}

static void *gnss_pps_task_entry(void *argv)
{
    unsigned int ActualFlags = 0;
    time_s cur_time;
    int first = 0;
    int ms_getted = 0;

    while (1) {
        (void)AmbaKAL_EventFlagGet(&SvcEventFlag, GNSS_PPS_EVENT_FLAG | GNSS_MS_TIMER_EVENT_FLAG,
                                 AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO,
                                 &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if ((ActualFlags & GNSS_PPS_EVENT_FLAG) == GNSS_PPS_EVENT_FLAG) {
            unsigned char diff = tick() - pps_valid_tick;
            gnss_time_now(&cur_time);
            debug_line("pps occused, gnss time: %.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3d,diff=%d", cur_time.year, cur_time.month, cur_time.day,
                                                                    cur_time.hour, cur_time.minute, cur_time.second, cur_time.msecond, diff);
        } else if ((ActualFlags & GNSS_MS_TIMER_EVENT_FLAG) == GNSS_MS_TIMER_EVENT_FLAG) {
            if (first == 0) {
                first = 1;
                time_now(&last_time);
                AmbaGPIO_SetFuncGPI(GNSS_PPS_PIN);
                AmbaGPIO_SetPullUpOrDown(GNSS_PPS_PIN, AMBA_GPIO_PULL_DOWN);
                AmbaGPIO_IntSetType(GNSS_PPS_PIN, GPIO_INT_RISING_EDGE_TRIGGER);
                AmbaGPIO_IntHookHandler(GNSS_PPS_PIN, gnss_pps_handler, 0);
                AmbaGPIO_IntEnable(GNSS_PPS_PIN);
    			continue;
            }
            time_now(&cur_time);
            if (cur_time.second != last_time.second) {
                ms_getted = 1;
                base_tick = tick();
                memcpy(&last_time, &cur_time, sizeof(cur_time));
                if (debug_enable) {
                    gnss_time_now(&cur_time);
                    debug_line("cur time: %.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3d", cur_time.year, cur_time.month, cur_time.day,
                                                                            cur_time.hour, cur_time.minute, cur_time.second, cur_time.msecond);
                }
            } else {
                if (ms_getted == 0) {
                    continue;
                }
                if (debug_enable) {                    
                    gnss_time_now(&cur_time);
                    debug_line("cur time: %.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3d", cur_time.year, cur_time.month, cur_time.day,
                                                                            cur_time.hour, cur_time.minute, cur_time.second, cur_time.msecond);
                }
            }
        }
    }

    return NULL;
}

