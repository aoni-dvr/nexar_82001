#include "mcu_update.h"
#include "app_base.h"

#define DEBUG_TAG "[mcu_update]"

struct _mcu_update_info_s_ {
    unsigned char i2c_channel;
    unsigned char slave_addr;
    unsigned char end_page;
    char path[64];
} mcu_update_infos[] = {
    {MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x7f, "x:\\mcu_main.bin"},
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    {LTE_MCU_I2C_CHANNEL, LTE_MCU_I2C_SLAVE_ADDR, 0x1f, "x:\\mcu_lte.bin"}
#endif
};
typedef struct _mcu_update_info_s_ mcu_update_info_s;

static int mcu_update_debug = 0;
static int check_lte_mcu_update(int index)
{
    if (index == 1
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        && (app_helper.check_lte_left_connected() == 0 && app_helper.check_lte_right_connected() == 0)
#endif
		) {
        return 0;
    }
    return 1;
}

static int mcu_update_notify(int index, int param)
{
    ipc_event_s event;
    memset(&event, 0, sizeof(event));
    event.event_id = (index == 0) ? NOTIFY_MAIN_MCU_UPDATE : NOTIFY_LTE_MCU_UPDATE;
    event.param = param;
    linux_api_service_notify_event(event);

    return 0;
}

static unsigned char buf[128 * 1024] = {0};
static int check_mcu_update_impl(void)
{
    unsigned int index = 0;
    //i2c_write_reg(mcu_update_infos[i].i2c_channel, mcu_update_infos[i].slave_addr, 0x07, 0);

    for (index = 0; index < sizeof(mcu_update_infos) / sizeof(mcu_update_info_s); index++) {
        unsigned long long filesize = 0;
        mcu_update_infos[index].path[0] = SD_SLOT[0];
        if (check_lte_mcu_update(index) == 0) {
            continue;
        }
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2) && defined(CONFIG_PCBA_DVT2)
        if (index == 1) {
            mcu_update_infos[index].i2c_channel = get_lte_i2c_channel();
        }
#endif
        if (app_helper.linux_booted && app_helper.check_file_exist(mcu_update_infos[index].path, &filesize)) {
            FILE *pFile = NULL;
            debug_line("%s_mcu update start", (index == 0) ? "main" : "lte");
            mcu_update_notify(index, 0);
            pFile = fopen(mcu_update_infos[index].path, "rb");
            if (pFile) {
                if (index == 0) {
                    int read_len = 0, i = 0, j = 0;
                    int page_total_num = 0;
                    unsigned char value = 0;

                    read_len = fread(buf, 1, sizeof(buf), pFile);
                    fclose(pFile);
                    sleep(1);
                    remove(mcu_update_infos[index].path);
                    if (app_helper.check_file_exist(mcu_update_infos[index].path, &filesize)) {
                        debug_line("delete %s failed", mcu_update_infos[index].path);
                    }
                    app_helper.mcu_update_flag = 1 << index;
                    i2c_write_reg(mcu_update_infos[index].i2c_channel, mcu_update_infos[index].slave_addr, 0xff, 0x99);
                    i2c_read_reg(mcu_update_infos[index].i2c_channel, mcu_update_infos[index].slave_addr, 0xfe, &value);
                    if (value == 0x55 || value == 0x66) {
                        unsigned char *start = (value == 0x55) ? buf : buf + (read_len / 2);
                        sleep(3);
                        read_len /= 2;
                        page_total_num = read_len / 128;
                        if ((read_len % 128) != 0) {
                            page_total_num += 1;
                        }
                        for (i = 0; i < page_total_num; i++) {
                            unsigned char base_reg = 0x52;
                            unsigned char check_sum = 0x00;
                            unsigned char cur_page_len = (read_len - i * 128) >= 128 ? 128 : (read_len % 128);
                            if (mcu_update_debug) {
                                debug_line("write mcu: %d, page: 0x%.2x, %d", index, i, i * 100 / page_total_num);
                            }
                            i2c_write_reg(mcu_update_infos[index].i2c_channel, mcu_update_infos[index].slave_addr, base_reg++, (i >> 8) & 0xff);                            
                            check_sum += ((i >> 8) & 0xff);
                            i2c_write_reg(mcu_update_infos[index].i2c_channel, mcu_update_infos[index].slave_addr, base_reg++, i & 0xff);
                            check_sum += (i & 0xff);
                            for (j = 0; j < cur_page_len; j++) {
                                i2c_write_reg(mcu_update_infos[index].i2c_channel, mcu_update_infos[index].slave_addr, base_reg++, *(start + 128 * i + j));
                                check_sum += *(start + 128 * i + j);
                            }
                            i2c_write_reg(mcu_update_infos[index].i2c_channel, mcu_update_infos[index].slave_addr, base_reg++, check_sum);
                            msleep(200);
                        }
                    }
                    i2c_write_reg(mcu_update_infos[index].i2c_channel, mcu_update_infos[index].slave_addr, 0xff, 0xbb);
                } else {
                    unsigned int read_len = 0, i = 0, j = 0;
                    unsigned int buf_size = 8 * 1024;

                    fseek(pFile, 0x1000, AMBA_FS_SEEK_START);
                    read_len = fread(buf, 1, buf_size, pFile);
                    fclose(pFile);
                    sleep(1);
                    remove(mcu_update_infos[index].path);
                    if (app_helper.check_file_exist(mcu_update_infos[index].path, &filesize)) {
                        debug_line("delete %s failed", mcu_update_infos[index].path);
                    }
                    if (read_len != buf_size) {
                        debug_line("mcu hex read failed");
                        mcu_update_notify(index, -1);
                        return -1;
                    }
                    app_helper.mcu_update_flag = 1 << index;
                    i2c_write_reg(mcu_update_infos[index].i2c_channel, mcu_update_infos[index].slave_addr, 0xff, 0x99);
                    sleep(3);
                    for (i = 0; i < (buf_size / 64); i++) {
                        unsigned char base_reg = 0x82;
                        unsigned char check_sum = 0x00;
                        check_sum += i;
                        if (mcu_update_debug) {
                            debug_line("write mcu: %d, page: 0x%.2x, %d", index, i, i * 100 / mcu_update_infos[index].end_page);
                        }
                        i2c_write_reg(mcu_update_infos[index].i2c_channel, mcu_update_infos[index].slave_addr, base_reg++, i);
                        for (j = 0; j < 32; j++) {
                            i2c_write_reg(mcu_update_infos[index].i2c_channel, mcu_update_infos[index].slave_addr, base_reg++, buf[64 * i + 2 * j + 1]);
                            check_sum += buf[64 * i + 2 * j + 1];
                            i2c_write_reg(mcu_update_infos[index].i2c_channel, mcu_update_infos[index].slave_addr, base_reg++, buf[64 * i + 2 * j]);
                            check_sum += buf[64 * i + 2 * j];
                        }
                        i2c_write_reg(mcu_update_infos[index].i2c_channel, mcu_update_infos[index].slave_addr, base_reg++, check_sum);
                        if (i >= mcu_update_infos[index].end_page) {
                            break;
                        }
                        if (check_lte_mcu_update(index) == 0) {
                            break;
                        }
                        msleep(200);
                    }
                    if (index == 1) {
                        sleep(2);
                        app_msg_queue_send(APP_MSG_ID_REBOOT, 0, 0, 0);
                    }
                }
                break;
            } else {
                debug_line("open %s failed", mcu_update_infos[index].path);
                mcu_update_notify(index, -1);
            }
        }
    }

    return 0;
}

static AMBA_KAL_TASK_t mcu_update_task;
#define MCU_UPDATE_TASK_STACK_SIZE (32 * 1024)
static unsigned char mcu_update_task_stack[MCU_UPDATE_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static void *mcu_update_task_entry(void *argv);
static int running = 0;
static int mcu_update_run = 0;

int mcu_update_task_start(void)
{
    unsigned int ret = 0;

    running = 1;
    ret = AmbaKAL_TaskCreate(&mcu_update_task,
                            "mcu_update_task",
                            180,
                            (AMBA_KAL_TASK_ENTRY_f)mcu_update_task_entry,
                            0,
                            mcu_update_task_stack,
                            MCU_UPDATE_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line(DEBUG_TAG"mcu_update_task create fail");
        running = 0;
        return -1;
    }
    AmbaKAL_TaskSetSmpAffinity(&mcu_update_task, 0x02);
    AmbaKAL_TaskResume(&mcu_update_task);

    return 0;
}

int mcu_update_task_stop(void)
{
    int Rval = 0;

    running = 0;
    AmbaKAL_TaskSleep(200);
    Rval = AmbaKAL_TaskTerminate(&mcu_update_task);
    if (Rval == OK) {
        Rval = AmbaKAL_TaskDelete(&mcu_update_task);
    }

    return 0;
}

void mcu_update_set_run(void)
{
    mcu_update_run = 1;
}

static void *mcu_update_task_entry(void *argv)
{
    while (running) {
        if (mcu_update_run) {
            mcu_update_run = 0;
            check_mcu_update_impl();
        }
        sleep(3);
    }

    return NULL;
}

int mcu_update_task_set_debug(int debug)
{
    mcu_update_debug = debug;

    return 0;
}

