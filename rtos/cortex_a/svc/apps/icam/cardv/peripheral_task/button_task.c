#include "button_task.h"
#include "platform.h"

// msg task
static AMBA_KAL_TASK_t button_task;
#define BUTTON_TASK_STACK_SIZE (32 * 1024)
static unsigned char button_task_stack[BUTTON_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static int debug_enable = 0;
static int running = 0;

static void *button_task_entry(void *arg);

static AMBA_KAL_EVENT_FLAG_t SvcEventFlag;
#define BUTTON_EVENT_FLAG        0x00000001UL
#define BUTTON_EVENT_STOP        0x00000002UL
#define LED_EVENT_FLAG            0x00000004UL

static void button_edge_handler(UINT32 GpioPinID, UINT32 UserArg)
{
    AmbaKAL_EventFlagSet(&SvcEventFlag, BUTTON_EVENT_FLAG);
}

#if !defined(CONFIG_BSP_H32_NEXAR_D081)
static void led_edge_handler(UINT32 GpioPinID, UINT32 UserArg)
{
    AmbaKAL_EventFlagSet(&SvcEventFlag, LED_EVENT_FLAG);
}
#endif

int button_task_start(void)
{
    unsigned int ret = 0;    
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    unsigned char rx_data = 0;
#endif
    running = 1;
    AmbaKAL_EventFlagCreate(&SvcEventFlag, "EventFlag");
    ret = AmbaKAL_TaskCreate(&button_task,
                            "button_task",
                            160,
                            (AMBA_KAL_TASK_ENTRY_f)button_task_entry,
                            0,
                            button_task_stack,
                            BUTTON_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("button_task create fail");
        running = 0;
        return -1;
    }
    AmbaKAL_TaskSetSmpAffinity(&button_task, 0x02);
    AmbaKAL_TaskResume(&button_task);

    AmbaGPIO_IntSetType(BUTTON_GPIO_PIN, GPIO_INT_BOTH_EDGE_TRIGGER);
    AmbaGPIO_IntHookHandler(BUTTON_GPIO_PIN, button_edge_handler, 0);
    AmbaGPIO_IntEnable(BUTTON_GPIO_PIN);
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    i2c_read_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x02, &rx_data);//clear ir status
    AmbaGPIO_SetPullUpOrDown(LED_IRQ_PIN, GPIO_PULL_UP);
    AmbaGPIO_IntSetType(LED_IRQ_PIN, GPIO_INT_FALLING_EDGE_TRIGGER);
    AmbaGPIO_IntHookHandler(LED_IRQ_PIN, led_edge_handler, 0);
    AmbaGPIO_IntEnable(LED_IRQ_PIN);
#endif

    return 0;
}

int button_task_stop(void)
{
    running = 0;
    AmbaKAL_EventFlagSet(&SvcEventFlag, BUTTON_EVENT_STOP);

    return 0;
}

int button_task_set_debug(int enable)
{
    debug_enable = enable;

    return 0;
}

static unsigned int long_press_milliseconds = 3000;
static unsigned int continue_press_milliseconds = 1000;
static unsigned int release_milliseconds = 300;

static unsigned int button_press_milliseconds = 0;
static unsigned int button_release_milliseconds = 0;
static unsigned int short_press_count = 0;

int button_task_set_time(unsigned int x, unsigned int y, unsigned int z)
{
    long_press_milliseconds = x;
    continue_press_milliseconds = y;
    release_milliseconds = z;

    return 0;
}

static void amba_kal_press_timer_handler(unsigned int tid)
{
    button_press_milliseconds += 1;
    if (button_press_milliseconds >= long_press_milliseconds) {
        unsigned int diff_time = button_press_milliseconds - long_press_milliseconds;
        if (diff_time == 0) {
            //long press event
            app_msg_queue_send(APP_MSG_ID_BUTTON_EVENT, 1, 0, button_press_milliseconds);
        } else if (diff_time % continue_press_milliseconds == 0) {
            //continue press event
            app_msg_queue_send(APP_MSG_ID_BUTTON_EVENT, 1, 1, button_press_milliseconds);
        }
    }
}

static AMBA_KAL_TIMER_t press_timer, release_timer;
static int button_press_timer_start(void)
{
    AmbaKAL_TimerCreate(&press_timer,
                        NULL,
                        amba_kal_press_timer_handler,
                        0,
                        1,
                        1,
                        AMBA_KAL_AUTO_START);

    return 0;
}

static int button_press_timer_stop(void)
{
    AmbaKAL_TimerStop(&press_timer);
    AmbaKAL_TimerDelete(&press_timer);

    return 0;
}

static int button_release_timer_stop(void);
static void amba_kal_release_timer_handler(unsigned int long_press)
{
    button_release_milliseconds += 1;
    if (button_release_milliseconds >= release_milliseconds) {
        // send count and stop timer
        if (long_press) {
            app_msg_queue_send(APP_MSG_ID_BUTTON_EVENT, 1, 2, button_press_milliseconds);
        } else {
            app_msg_queue_send(APP_MSG_ID_BUTTON_EVENT, 0, short_press_count, 0);
        }
        short_press_count = 0;
        button_release_timer_stop();
    }
}

static int button_release_timer_start(int long_press)
{
    AmbaKAL_TimerCreate(&release_timer,
                        NULL,
                        amba_kal_release_timer_handler,
                        long_press,
                        1,
                        1,
                        AMBA_KAL_AUTO_START);

    return 0;
}

static int button_release_timer_stop(void)
{
    AmbaKAL_TimerStop(&release_timer);
    AmbaKAL_TimerDelete(&release_timer);

    return 0;
}

static void *button_task_entry(void *arg)
{
    UINT32 ActualFlags = 0U;

    while (running) {
        (void)AmbaKAL_EventFlagGet(&SvcEventFlag, BUTTON_EVENT_FLAG | BUTTON_EVENT_STOP | LED_EVENT_FLAG,
                                 AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO,
                                 &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if (ActualFlags & BUTTON_EVENT_FLAG) {
            if (app_helper.gpio_get(BUTTON_GPIO_PIN)) {
                if (debug_enable) debug_line("button released");
                button_press_timer_stop();
                button_release_milliseconds = 0;
                if (button_press_milliseconds < long_press_milliseconds) {
                    short_press_count += 1;
                    //debug_line("button tap count: %d", short_press_count);
                    button_release_timer_start(0);
                } else {
                    button_release_timer_start(1);
                }
                //linux_api_service_notify_state_changed(NOTIFY_BUTTON_STATE_CHANGE_ID, 0);
            } else {
                if (debug_enable) debug_line("button pressed");
                //start timer
                button_release_timer_stop();
                button_press_milliseconds = 0;
                button_press_timer_start();
                //linux_api_service_notify_state_changed(NOTIFY_BUTTON_STATE_CHANGE_ID, 1);
            }
        }
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
        if (ActualFlags & LED_EVENT_FLAG) {
            ipc_event_s event;
            unsigned char rx_data = 0;

            i2c_read_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x02, &rx_data);//clear ir status
            memset(&event, 0, sizeof(event));
            event.event_id = NOTIFY_LED_IRQ_STATE_CHANGE_ID;
            event.param = app_helper.gpio_get(LED_IRQ_PIN);
            linux_api_service_notify_event(event);
        }
#endif
    }

	return NULL;
}

