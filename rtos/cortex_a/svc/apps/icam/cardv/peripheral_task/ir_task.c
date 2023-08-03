#include "ir_task.h"
#include <app_base.h>

// msg task
static AMBA_KAL_TASK_t ir_task;
#define IR_TASK_STACK_SIZE (64 * 1024)
static unsigned char ir_task_stack[IR_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static int debug_enable = 0;
static int running = 0;
static int ir_check = 0;
static int average_value = 0;
static void *ir_task_entry(void *arg);

int ir_task_start(void)
{
    unsigned int ret = 0;

    running = 1;
    ret = AmbaKAL_TaskCreate(&ir_task,
                            "ir_task",
                            160,
                            (AMBA_KAL_TASK_ENTRY_f)ir_task_entry,
                            0,
                            ir_task_stack,
                            IR_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("ir_task create fail");
        running = 0;
        return -1;
    }

    return 0;
}

int ir_task_stop(void)
{
    running = 0;

    return 0;
}

int ir_task_set_debug(int enable)
{
    debug_enable = enable;

    return 0;
}

int ir_task_set_check_enable(int enable)
{
    ir_check = enable;

    return 0;
}

int ir_task_get_brightness(void)
{
    return average_value;
}

static int ir_task_get_cur_brightness(void)
{
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    unsigned char rx_data[2] = {0};
    AMBA_I2C_TRANSACTION_s RxCtrl;
    UINT32 ActualSize = 0;
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2) && defined(CONFIG_PCBA_DVT2)
    unsigned int i2c_channel = get_brightness_i2c_channel();
#else
    unsigned int i2c_channel = AMBA_I2C_CHANNEL1;
#endif

    memset(rx_data, 0, sizeof(rx_data));

    RxCtrl.SlaveAddr = 0x9A;
    RxCtrl.DataSize = 2;
    RxCtrl.pDataBuf = rx_data;
    AmbaI2C_MasterRead(i2c_channel, I2C_SPEED_FAST, &RxCtrl, &ActualSize, 1000);

    return ((rx_data[0] & 0xF) << 6) | ((rx_data[1] & 0xFC) >> 2);
#else
    unsigned int data = 0;
    AmbaADC_Config(10000);
    AmbaADC_SingleRead(AMBA_ADC_CHANNEL1, &data);

    return data;
#endif
}

static void *ir_task_entry(void *arg)
{
    int value[5] = {0};
    int cnt = 0, sum_value = 0;
    int ir_on_cnt = 0, ir_off_cnt = 0, i = 0;
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->IrThresHold > 5) {
        pSvcUserPref->IrThresHold = 5;
    }
    for (i = 0; i < 5; i++) {
        value[i] = 0;
    }
    while (running) {
        if (ir_check) {
            if (pSvcUserPref->IrThresHold == 0) {
                cnt = 0;sum_value = 0;ir_on_cnt = 0;ir_off_cnt = 0;
                for (i = 0; i < 5; i++) {
                    value[i] = 0;
                }
                if (ir_check) {
                    if (app_helper.set_ir_brightness(0) >= 0) {
                        rec_dvr_switch_black_white_mode(CAMERA_CHANNEL_EXTERNAL, 0);
                    }
                }
            } else if (pSvcUserPref->IrThresHold == 5) {
                cnt = 0;sum_value = 0;ir_on_cnt = 0;ir_off_cnt = 0;
                for (i = 0; i < 5; i++) {
                    value[i] = 0;
                }
                if (ir_check) {
                    if (app_helper.set_ir_brightness(pSvcUserPref->IrBrightness) >= 0) {
                        rec_dvr_switch_black_white_mode(CAMERA_CHANNEL_EXTERNAL, 1);
                    }
                }
            } else  {
                int taget_value = 2, cur_value = 0;
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
                taget_value = 80;
#endif
#if 0
                if (pSvcUserPref->IrThresHold == 1) {
                    taget_value = 5;
                } else if (pSvcUserPref->IrThresHold == 2) {
                    taget_value = 8;
                } else if (pSvcUserPref->IrThresHold == 3) {
                    taget_value = 10;
                } else if (pSvcUserPref->IrThresHold == 4) {
                    taget_value = 30;
                } else {
                    taget_value = 30;
                }
#endif
                cur_value = ir_task_get_cur_brightness();
                if (cnt < 5) {
                    value[cnt] = cur_value;
                    cnt++;
                } else {
                    for (i = 0; i < 4; i++) {
                        value[i] = value[i + 1];
                    }
                    value[4] = cur_value;
                    cnt = 5;
                }
                sum_value = 0;
                for (i = 0; i < cnt; i++) {
                    sum_value += value[i];
                }
                average_value = sum_value / cnt;
                if (debug_enable) {
                    debug_line("brightness adc: %d, average: %d", cur_value, average_value);
                }

                if (average_value < taget_value) {
                    ir_on_cnt++;
                } else {
                    ir_on_cnt = 0;
                }

                if (average_value > (taget_value + 15)) {
                    ir_off_cnt++;
                } else {
                    ir_off_cnt = 0;
                }

                if (ir_on_cnt >= 5) {
                    ir_on_cnt = 0;
                    if (ir_check) {
                        if (app_helper.set_ir_brightness(pSvcUserPref->IrBrightness) >= 0) {
                            rec_dvr_switch_black_white_mode(CAMERA_CHANNEL_EXTERNAL, 1);
                        }
                    }
                }
                if (ir_off_cnt >= 5) {
                    ir_off_cnt = 0;
                    if (ir_check) {
                        if (app_helper.set_ir_brightness(0) >= 0) {
                            rec_dvr_switch_black_white_mode(CAMERA_CHANNEL_EXTERNAL, 0);
                        }
                    }
                }
            }
        }
        sleep(1);
    }

	return NULL;
}

