#include "thermal_task.h"

// msg task
static AMBA_KAL_TASK_t thermal_task;
#define THERMAL_TASK_STACK_SIZE (32 * 1024)
static unsigned char thermal_task_stack[THERMAL_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static int debug_enable = 0;

static void *thermal_task_entry(void *arg);

int thermal_task_start(void)
{
    unsigned int ret = 0;

    ret = AmbaKAL_TaskCreate(&thermal_task,
                            "thermal_task",
                            160,
                            (AMBA_KAL_TASK_ENTRY_f)thermal_task_entry,
                            0,
                            thermal_task_stack,
                            THERMAL_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("thermal_task create fail");
        return -1;
    }

    return 0;
}

int thermal_task_set_debug(int enable)
{
    debug_enable = enable;

    return 0;
}

static void *thermal_task_entry(void *arg)
{
    THERMAL_DATA_s thermal_data;
    int temp = 0;

    while (1) {
        if (Thermal_GetData(&thermal_data) < 0) {
            continue;
        }
        if (thermal_data.value > 128) {
            temp = thermal_data.value - 255 - 1;
        } else {
            temp = thermal_data.value;
        }
        if (debug_enable) {
            debug_line("thermal: %d, temp=%d", thermal_data.value, temp);
        }
        sleep(1);
    }

	return NULL;
}

