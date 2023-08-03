#include <app_base.h>

static app_timer_info_s timers_info[TIMER_NUM] = {
    {TIMER_1HZ,         1000,       APP_MSG_ID_TIMER_1HZ},
    {TIMER_2HZ,         500,        APP_MSG_ID_TIMER_2HZ},
    {TIMER_30S,         30 * 1000,  APP_MSG_ID_TIMER_30S}
};

static app_timer_s Timers[TIMER_NUM] = {0};
static AMBA_KAL_SEMAPHORE_t app_timer_sem;
int app_timer_init(void)
{
    AmbaKAL_SemaphoreCreate(&app_timer_sem, NULL, 1);
    memset(&Timers[0], 0, sizeof(app_timer_s) * TIMER_NUM);

    return 0;
}

static void amba_kal_timer_handler(unsigned int tid)
{
    AmbaMisra_TouchUnused(&tid);
    app_msg_queue_send(timers_info[tid].msg_id, tid, 0, 0);
}

static int app_timer_set(int tid)
{
    unsigned int ReturnValue = 0;

    if ((tid < 0) || (tid >= TIMER_NUM)) {
        return -1;
    }

    AmbaKAL_SemaphoreTake(&app_timer_sem, AMBA_KAL_WAIT_FOREVER);
    if (Timers[tid].Valid) {
        ReturnValue = AmbaKAL_TimerStop(&Timers[tid].AppTimer);
        ReturnValue = AmbaKAL_TimerDelete(&Timers[tid].AppTimer);
    }
    ReturnValue = AmbaKAL_TimerCreate(&Timers[tid].AppTimer,
                                    NULL,
                                    amba_kal_timer_handler,
                                    tid,
                                    timers_info[tid].period,
                                    timers_info[tid].period,
                                    AMBA_KAL_AUTO_START);
    if (ReturnValue != KAL_ERR_NONE) {
        ReturnValue = -1;
        debug_line("<%s> timer create fail", __func__);
    } else {
        Timers[tid].Valid = 1;
        ReturnValue = 0;
    }
    AmbaKAL_SemaphoreGive(&app_timer_sem);

    return ReturnValue;
}

int app_timer_register(int tid, app_timer_handler handler)
{
    int i = 0, Found = 0;
    app_timer_s *CurrentTimer = NULL;

    if (tid >= TIMER_NUM) {
        debug_line("<%s> No timer id %d", __func__, tid);
        return -1;
    }

    if (handler == NULL) {
        debug_line("<%s>Timer handler is null", __func__);
        return -1;
    }
    CurrentTimer = &(Timers[tid]);
    Found = 0;
    for (i = 0; i < MAX_TIMER_HANDLER; i++) {
        if (handler == CurrentTimer->Handler[i]) {
            // Handler already exists
            debug_line("<%s>Timer has existed", __func__);
            Found = 1;
            break;
        }
    }

    if (!Found) {
        for (i = 0; i < MAX_TIMER_HANDLER; i++) {
            if (CurrentTimer->Handler[i] == NULL) {
                // Empty slot Found. Insert Handler
                CurrentTimer->Handler[i] = handler;
                CurrentTimer->HandlerNum++;
                if (!CurrentTimer->Valid) {
                    app_timer_set(tid);
                }
                break;
            }
        }
    }

    return 0;
}

static int app_timer_clear(int tid)
{
    int ret = 0;

    if ((tid < 0) || (tid >= TIMER_NUM)) {
        return -1;
    }
    AmbaKAL_SemaphoreTake(&app_timer_sem, AMBA_KAL_WAIT_FOREVER);
    if (Timers[tid].Valid) {
        ret = AmbaKAL_TimerStop(&Timers[tid].AppTimer);
        ret = AmbaKAL_TimerDelete(&Timers[tid].AppTimer);
        memset(&Timers[tid], 0, sizeof(app_timer_s));
    }
    AmbaKAL_SemaphoreGive(&app_timer_sem);

    return ret;
}

int app_timer_unregister(int tid, app_timer_handler handler)
{
    int i = 0, Found = 0;
    app_timer_s *CurrentTimer = NULL;

    if (tid >= TIMER_NUM) {
        debug_line("<%s>No timer id %d", __func__, tid);
        return -1;
    }

    if (handler == NULL) {
        debug_line("<%s>Timer handler is NULL", __func__);
        return -1;
    }

    CurrentTimer = &(Timers[tid]);
    if (CurrentTimer->HandlerNum == 0) {
        //debug_line("<%s>No timer handler is registered", __func__);
        return -1;
    }

    Found = 0;
    for (i = 0; i < MAX_TIMER_HANDLER; i++) {
        if (handler == CurrentTimer->Handler[i]) {
            Found = 1;
            CurrentTimer->Handler[i] = NULL;
            CurrentTimer->HandlerNum--;
            handler(TIMER_UNREGISTER);
            break;
        }
    }

    if (Found) {
        if (CurrentTimer->HandlerNum == 0) {
            app_timer_clear(tid);
        }
    }

    return 0;
}

int app_timer_unregister_all(void)
{
    int i = 0, j = 0;

    for (i = 1; i < TIMER_NUM; i++) {
        for (j = 0; j < MAX_TIMER_HANDLER; j++) {
            Timers[i].Handler[j] = NULL;
        }
        Timers[i].HandlerNum = 0;
        app_timer_clear(i);
    }

    return 0;
}

int app_timer_callback_handler(int tid)
{
    int i = 0;
    app_timer_s *CurrentTimer = NULL;

    if (tid >= TIMER_NUM) {
        debug_line("<%s>No timer id %d", __func__, tid);
        return -1;
    }

    CurrentTimer = &(Timers[tid]);
    if (!CurrentTimer->Valid) {
        debug_line("<%s>Timer id %d is invalid", __func__, tid);
        return -1;
    }

    if (CurrentTimer->HandlerNum == 0) {
        // no Handlers in timer
        debug_line("<%s>No timer handler is registered", __func__);
        return -1;
    }

    for (i = 0; i < MAX_TIMER_HANDLER; i++) {
        if (CurrentTimer->Handler[i] != NULL) {
            CurrentTimer->Handler[i](TIMER_TICK);
        }
    }

    return 0;
}

