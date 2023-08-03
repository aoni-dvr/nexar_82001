#include "agnss_task.h"

#define DEBUG_TAG "[agnss]"

static int agnss_task_inject_data(const char *path)
{
    struct stat fstat;
    FILE *pFile = NULL;
    unsigned char buf[512] = {0};
    int playload_len = 0, nwrite = 0, nread = 0, rval = -1;
    unsigned int write_len = 0, timeout = 0;

    if (stat(path, &fstat) != 0 || fstat.size == 0) {
        //debug_line(DEBUG_TAG"%s not exist or empty", path);
        return -1;
    }
    buf[0] = 0xF1;
    buf[1] = 0xD9;
    buf[2] = 0x06;
    buf[3] = 0x40;
    buf[4] = 0x01;
    buf[5] = 0x00;
    buf[6] = 0x01;
    buf[7] = 0x48;
    buf[8] = 0x22;
    gnss_parser_set_cold_boot_response(0);
    gnss_task_uart_write(buf, 9);
    while (gnss_parser_get_cold_boot_response() == 0 && timeout < 100) {
        timeout++;
        msleep(100);
    }
    if (timeout >= 100) {
        debug_line(DEBUG_TAG"set cold boot response timeout");
        return -1;
    }
    debug_line(DEBUG_TAG"start to write data");
    pFile = fopen(path, "rb");
    if (pFile == NULL) {
        debug_line(DEBUG_TAG"%s open fail", path);
        return -1;
    }
    if ((nread = fread(buf, 1, 6, pFile)) != 6) {
        debug_line(DEBUG_TAG"%s read fail(%d) on line: %d", path, nread, __LINE__);
        fclose(pFile);
        return -1;
    }
    while (1) {
        if (buf[0] == 0xF1 && buf[1] == 0xD9) {
            playload_len = (unsigned short)(buf[4] | (buf[5] << 8));
            if ((nread = fread(buf + 6, 1, playload_len + 2, pFile)) != (playload_len + 2)) {
                debug_line(DEBUG_TAG"%s read fail(%d) on line: %d", path, nread, __LINE__);
                break;
            }
            nwrite = gnss_task_uart_write(buf, playload_len + 8);
            if (nwrite < 0) {
                debug_line(DEBUG_TAG"uart send fail");
                break;
            }
            write_len += (playload_len + 8);
        } else {
            debug_line(DEBUG_TAG"header is not 0xF1:%x,%x", buf[0], buf[1]);
            break;
        }
        if (write_len >= fstat.size) {
            debug_line(DEBUG_TAG"write finished");
            rval = 0;
            break;
        }
        if ((nread = fread(buf, 1, 6, pFile)) != 6) {
            debug_line(DEBUG_TAG"%s read fail(%d) on line: %d", path, nread, __LINE__);
            break;
        }
        msleep(3); // just example, use wait time 3ms instead of ACK
    }
    fclose(pFile);

    return rval;
}

static AMBA_KAL_TASK_t agnss_task;
#define AGNSS_TASK_STACK_SIZE (32 * 1024)
static unsigned char agnss_task_stack[AGNSS_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static void *agnss_task_entry(void *argv);
static int running = 0;

int agnss_task_start(void)
{
    unsigned int ret = 0;

    running = 1;
    ret = AmbaKAL_TaskCreate(&agnss_task,
                            "agnss_task",
                            180,
                            (AMBA_KAL_TASK_ENTRY_f)agnss_task_entry,
                            0,
                            agnss_task_stack,
                            AGNSS_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line(DEBUG_TAG"agnss_task create fail");
        running = 0;
        return -1;
    }

    return 0;
}

int agnss_task_stop(void)
{
    int Rval = 0;

    running = 0;
    AmbaKAL_TaskSleep(200);
    Rval = AmbaKAL_TaskTerminate(&agnss_task);
    if (Rval == OK) {
        Rval = AmbaKAL_TaskDelete(&agnss_task);
    }

    return 0;
}

static void *agnss_task_entry(void *argv)
{
#define AGNSS_FILE_PATH "a:\\pref\\agnss.hdb"

    while (running) {
        agnss_task_inject_data(AGNSS_FILE_PATH);
        sleep(10);
    }

    return NULL;
}

