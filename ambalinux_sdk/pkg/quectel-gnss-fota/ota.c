#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "serial_port.h"
#include "debug.h"
#include "rtos_api_lib.h"

#define ROM_PARTITION_TABLE    0x08000000
#define ROM_BL                 0x08003000
#define ROM_RTOS               0x08013000
#define ROM_GNSS_CONFIG        0x083FF000
#define FORMAT_LENGTH          0x00400000

static int do_handshake(int fd)
{
    //reset module
    //send 0xA0 to module every 20ms, wait 0x5F
    //send 0x0A, wait 0xF5
    //send 0x50, wait 0xAF
    //send 0x05, wait 0xFA
    unsigned char send_bytes[4] = {0xA0, 0x0A, 0x50, 0x05};
    unsigned char recv_bytes[4] = {0x5F, 0xF5, 0xAF, 0xFA};
    unsigned char recv_value = 0, i = 0;
    int ret = 0;

    rtos_api_lib_gnss_reset();
    for (i = 0; i < sizeof(send_bytes); i++) {
        debug_line("Step%d, send hand shake command: %02X", i, send_bytes[i]);
        while (1) {
            serial_write(fd, &send_bytes[i], 1);
            ret = serial_read(fd, &recv_value, 1, i == 0 ? 20 : 500);
            if (ret > 0 && recv_value == recv_bytes[i]) {
                debug_line("[Receive hand shake command feedback:] success %02X", recv_value);
                break;
            } else {
                //debug_line("[Receive hand shake command feedback:] fail %02X", recv_value);
            }
        }
    }

    return 0;
}

static int disable_watchdog(int fd)
{
    //send 0xD2, wait 0xD2
    //send wdt register address, 4 bytes, wait wdt register address
    //send 0x00000001, wait 0x00000001, wait brom 2bytes
    //send wdt value 2bytes, wait wdt value, wait brom 2bytes
    unsigned char send_data = 0xD2;
    unsigned char receive_data[4] = {0};
    unsigned char send_wdt_data[4] = {0xA2, 0x08, 0x00, 0x00};
    unsigned char send_command[4] = {0x00, 0x00, 0x00, 0x01};
    unsigned char send_command_end[2] = {0x00, 0x10};
    unsigned char i = 0;
    int ret = 0;

    serial_write(fd, &send_data, 1);
    debug_line("Step1: Send disabled WDT command:%02X", send_data);
    ret = serial_read(fd, receive_data, 1, 500);
    if (ret > 0 && receive_data[0] == send_data) {
        debug_line("[Receive disabled WDT command feedback:] success %02X", receive_data[0]);
    } else {
        debug_line("[Receive disabled WDT command feedback:] fail %02X", receive_data[0]);
        return -1;
    }

    serial_write(fd, send_wdt_data, 4);
    debug_line("Step2: Send disabled WDT command:%02X%02X%02X%02X", send_wdt_data[0], send_wdt_data[1], send_wdt_data[2], send_wdt_data[3]);
    ret = serial_read(fd, receive_data, 4, 500);
    if (ret > 0 && (receive_data[0] == send_wdt_data[0]) && (receive_data[1] == send_wdt_data[1])
                &&(receive_data[2] == send_wdt_data[2]) && (receive_data[3] == send_wdt_data[3])) {
        debug_line("[Receive disabled WDT command feedback:] success %02X%02X%02X%02X", receive_data[0], receive_data[1], receive_data[2], receive_data[3]);
    } else {
        debug_line("[Receive disabled WDT command feedback:] fail %02X%02X%02X%02X", receive_data[0], receive_data[1], receive_data[2], receive_data[3]);
        return -1;
    }

    serial_write(fd, send_command, 4);
    debug_line("Step3: Send disabled WDT command:%02X%02X%02X%02X", send_command[0], send_command[1], send_command[2], send_command[3]);
    ret = serial_read(fd, receive_data, 4, 500);
    if (ret > 0 && (receive_data[0] == send_command[0]) && (receive_data[1] == send_command[1])
                &&(receive_data[2] == send_command[2]) && (receive_data[3] == send_command[3])) {
        debug_line("[Receive disabled WDT command feedback:] success %02X%02X%02X%02X", receive_data[0], receive_data[1], receive_data[2], receive_data[3]);
    } else {
        debug_line("[Receive disabled WDT command feedback:] fail %02X%02X%02X%02X", receive_data[0], receive_data[1], receive_data[2], receive_data[3]);
        return -1;
    }

    ret = serial_read(fd, receive_data, 2, 500);
    if (ret > 0 && (0x00 == receive_data[0]) && (0x01 == receive_data[1])) {
        //Receive 0x0001 success
        debug_line("[Receive disabled WDT command feedback:] success %02X%02X", receive_data[0], receive_data[1]);
    } else {
        //Error
        debug_line("[Receive disabled WDT command feedback:] fail %02X%02X\r\n", receive_data[0], receive_data[1]);
        return -1;
    }

    serial_write(fd, send_command_end, 2);
    debug_line("Step5: Send disabled WDT command:%02X%02X", send_command_end[0], send_command_end[1]);
    ret = serial_read(fd, receive_data, 2, 500);
    if (ret > 0 && (receive_data[0] == send_command_end[0]) && (receive_data[1] == send_command_end[1])) {
        debug_line("[Receive disabled WDT command feedback:] success %02X%02X%", receive_data[0], receive_data[1]);
    } else {
        debug_line("[Receive disabled WDT command feedback:] fail %02X%02X", receive_data[0], receive_data[1]);
        return -1;
    }

    ret = serial_read(fd, receive_data, 2, 500);
    if (ret > 0 && (0x00 == receive_data[0]) && (0x01 == receive_data[1])) {
        //Receive 0x0001 success
        debug_line("[Receive disabled WDT command feedback:] success %02X%02X", receive_data[0], receive_data[1]);
    } else {
        //Error
        debug_line("[Receive disabled WDT command feedback:] fail %02X%02X\r\n", receive_data[0], receive_data[1]);
        return -1;
    }

    return 0;
}

static unsigned short da_compute_checksum(unsigned char *buf, unsigned int len)
{
    unsigned short checksum = 0;
    int i = 0;

    if (buf == NULL || len == 0) {
        return 0;
    }
    for (i = 0; i < len / 2; i++) {
        checksum ^= *(unsigned short *)(buf + i * 2);
    }
    if (len % 2) {
        checksum ^= buf[i * 2];
    }
    return checksum;
}

static int send_da_file(int fd, const char *path)
{
    //send 0xd7, wait 0xd7
    //send da run address 4bytes, wait da run address
    //send da length 4bytes, wait da lenght 4bytes
    //send 0x00000000, wait 0x00000000, wait brom 2bytes
    //loop send da data, every 1024 bytes
    //wait drom checksum 2 bytes
    //wait brom 2bytes
    unsigned char send_cmd = 0xD7;
    unsigned char da_address[4] = {0x04, 0x20, 0x40, 0x00};
    unsigned char da_length[4] = {0x00, 0x00, 0x6C, 0xF7};
    unsigned char da_flag[4] = {0x00, 0x00, 0x00, 0x00};
    unsigned char da_ack[4] = {0};
    int ret = 0;
    FILE *fp = NULL;
    unsigned int len = 0;
    int i = 0, send_times = 0;
    unsigned char buffer[1024] = {0};
    unsigned short checksum = 0;

    serial_write(fd, &send_cmd, 1);
    debug_line("Step1: Send DA file command:%02X", send_cmd);
    ret = serial_read(fd, da_ack, 1, 500);
    if (ret > 0 && da_ack[0] == send_cmd) {
        debug_line("[Receive DA File feedback:] success:%02X", da_ack[0]);
    } else {
        debug_line("[Receive DA File feedback:] fail %02X", da_ack[0]);
        return -1;
    }

    serial_write(fd, da_address, 4);
    debug_line("Step2: Send DA file command:%02X%02X%02X%02X", da_address[0], da_address[1], da_address[2], da_address[3]);
    ret = serial_read(fd, da_ack, 4, 500);
    if (ret > 0 && (da_ack[0] == da_address[0]) && (da_ack[1] == da_address[1])
                &&(da_ack[2] == da_address[2]) && (da_ack[3] == da_address[3])) {
        debug_line("[Receive DA File feedback:] success: %02X%02X%02X%02X", da_ack[0], da_ack[1], da_ack[2], da_ack[3]);
    } else {
        debug_line("[Receive DA File feedback:] fail %02X%02X%02X%02X", da_ack[0], da_ack[1], da_ack[2], da_ack[3]);
        return -1;
    }

    serial_write(fd, da_length, 4);
    debug_line("Step3: Send DA file command:%02X%02X%02X%02X", da_length[0], da_length[1], da_length[2], da_length[3]);
    ret = serial_read(fd, da_ack, 4, 500);
    if (ret > 0 && (da_ack[0] == da_length[0]) && (da_ack[1] == da_length[1])
                &&(da_ack[2] == da_length[2]) && (da_ack[3] == da_length[3])) {
        debug_line("[Receive DA File feedback:] success: %02X%02X%02X%02X", da_ack[0], da_ack[1], da_ack[2], da_ack[3]);
    } else {
        debug_line("[Receive DA File feedback:] fail %02X%02X%02X%02X", da_ack[0], da_ack[1], da_ack[2], da_ack[3]);
        return -1;
    }

    serial_write(fd, da_flag, 4);
    debug_line("Step4: Send DA file command:%02X%02X%02X%02X", da_flag[0], da_flag[1], da_flag[2], da_flag[3]);
    ret = serial_read(fd, da_ack, 4, 500);
    if (ret > 0 && (da_ack[0] == da_flag[0]) && (da_ack[1] == da_flag[1])
                &&(da_ack[2] == da_flag[2]) && (da_ack[3] == da_flag[3])) {
        debug_line("[Receive DA File feedback:] success: %02X%02X%02X%02X", da_ack[0], da_ack[1], da_ack[2], da_ack[3]);
    } else {
        debug_line("[Receive DA File feedback:] fail %02X%02X%02X%02X", da_ack[0], da_ack[1], da_ack[2], da_ack[3]);
        return -1;
    }

    ret = serial_read(fd, da_ack, 2, 500);
    if (ret > 0 && (0x00 == da_ack[0]) && (0x00 == da_ack[1])) {
        //Receive 0x0001 success
        debug_line("[Receive DA File feedback:] success:%02X%02X", da_ack[0], da_ack[1]);
    } else {
        //Error
        debug_line("[Receive DA File feedback:] fail %02X%02X\r\n", da_ack[0], da_ack[1]);
        return -1;
    }

    fp = fopen(path, "rb+");
    if (fp == NULL) {
        debug_line("Open %s fail!!!", path);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    send_times = len / 1024;
    if (len % 1024 != 0) {
        send_times += 1;
    }
    for (i = 0; i < send_times; i++) {
        int bytes = (len - i * 1024) >= 1024 ?  1024 : len % 1024;
        fseek(fp, i * 1024, SEEK_SET);
        fread(buffer, 1, bytes, fp);
        //usleep(20 * 1000);
        debug_line("Send the %d/%d DA file", i + 1, send_times);
        serial_write(fd, buffer, bytes);
        checksum ^= da_compute_checksum(buffer, bytes);
    }
    fclose(fp);
    debug_line("The checksum of the DA file is:%04X",checksum);

    ret = serial_read(fd, da_ack, 4, 500);
    if (ret > 0 && (da_ack[0] == (unsigned char)(checksum >> 8)) && (da_ack[1] == (unsigned char)(checksum))
                &&(da_ack[2] == 0x00) && (da_ack[3] == 0x00)) {
        debug_line("[Receive DA File feedback:] success: %02X%02X%02X%02X", da_ack[0], da_ack[1], da_ack[2], da_ack[3]);
    } else {
        debug_line("[Receive DA File feedback:] fail %02X%02X%02X%02X", da_ack[0], da_ack[1], da_ack[2], da_ack[3]);
        return -1;
    }

    return 0;
}

static int jump_to_da(int fd)
{
    //send 0xd5, wait 0xd5
    //send da run address 4bytes, wait da run address
    //wait brom 2bytes
    unsigned char jump_to_da = 0xD5;
    unsigned char da_run_address[4] = {0x04, 0x20, 0x40, 0x00};
    unsigned char jump_ack[4] = {0};
    int ret = 0;

    serial_write(fd, &jump_to_da, 1);
    debug_line("Step1: Send DA jump command :%02X", jump_to_da);
    ret = serial_read(fd, jump_ack, 1, 500);
    if (ret > 0 && jump_ack[0] == jump_to_da) {
        debug_line("[Receive DA jump command feedback:] success:%02X", jump_ack[0]);
    } else {
        debug_line("[Receive DA jump command feedback:] fail %02X", jump_ack[0]);
        return -1;
    }

    serial_write(fd, da_run_address, 4);
    debug_line("Step2: Send DA jump command :%02X%02X%02X%02X", da_run_address[0], da_run_address[1], da_run_address[2], da_run_address[3]);
    ret = serial_read(fd, jump_ack, 4, 500);
    if (ret > 0 && (jump_ack[0] == da_run_address[0]) && (jump_ack[1] == da_run_address[1])
                &&(jump_ack[2] == da_run_address[2]) && (jump_ack[3] == da_run_address[3])) {
        debug_line("[Receive DA jump command feedback:] success:%02X%02X%02X%02X", jump_ack[0], jump_ack[1], jump_ack[2], jump_ack[3]);
    } else {
        debug_line("[Receive DA jump command feedback:] fail %02X%02X%02X%02X", jump_ack[0], jump_ack[1], jump_ack[2], jump_ack[3]);
        return -1;
    }

    ret = serial_read(fd, jump_ack, 2, 500);
    if (ret > 0 && (0x00 == jump_ack[0]) && (0x00 == jump_ack[1])) {
        //Receive 0x0001 success
        debug_line("[Receive DA jump command feedback:] success:%02X%02X", jump_ack[0], jump_ack[1]);
    } else {
        //Error
        debug_line("[Receive DA jump command feedback:] fail %02X%02X\r\n", jump_ack[0], jump_ack[1]);
        return -1;
    }

    return 0;
}

static int synchronize_with_da(int fd)
{
    //wait 0xc0
    //send 0x3f, wait 0x0c
    //send 0xf3, wait 0x3f
    //send 0xc0, wait 0xf3
    //send 0x0c00, wait 0x5a6969
    //send 0x5a00, wait 0x69
    //send 0x5a, wait 0x69
    //send 0x5a
    //send 0xc0, wait 0xc0
    //send 0x5a, wait 0x5a69
    //send 0x5a
    //wait flash manufacturer id, 2bytes
    //wait flash device id, 2bytes
    //wait flash device id2, 2bytes
    //wait flash mount status, 4bytes
    //wait flash start address, 4bytes
    //wait flash size, 4bytes
    //wait 0x5a
    //send 0x5a
    unsigned char sync_ack[4] = {0};
    unsigned char send_com[7] = {0x3F, 0xF3, 0xC0, 0x0C, 0x00, 0x5A, 0x00};
    int ret = 0;

    ret = serial_read(fd, sync_ack, 1, 500);
    if (ret > 0 && sync_ack[0] == send_com[2]) {
        debug_line("[Receive DA jump command feedback:] success:%02X", sync_ack[0]);
    } else {
        debug_line("[Receive DA jump command feedback:] fail %02X", sync_ack[0]);
        return -1;
    }

    serial_write(fd, &send_com[0], 1);
    debug_line("Step1: Send sync DA command:%02X", send_com[0]);
    ret = serial_read(fd, sync_ack, 1, 500);
    if (ret > 0 && sync_ack[0] == send_com[3]) {
        debug_line("[Receive sync DA command feedback:] success:%02X", sync_ack[0]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X", sync_ack[0]);
        return -1;
    }

    serial_write(fd, &send_com[1], 1);
    debug_line("Step2: Send sync DA command:%02X", send_com[1]);
    ret = serial_read(fd, sync_ack, 1, 500);
    if (ret > 0 && sync_ack[0] == send_com[0]) {
        debug_line("[Receive sync DA command feedback:] success:%02X", sync_ack[0]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X", sync_ack[0]);
        return -1;
    }

    serial_write(fd, &send_com[2], 1);
    debug_line("Step3: Send sync DA command:%02X", send_com[2]);
    ret = serial_read(fd, sync_ack, 1, 500);
    if (ret > 0 && sync_ack[0] == send_com[1]) {
        debug_line("[Receive sync DA command feedback:] success:%02X", sync_ack[0]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X", sync_ack[0]);
        return -1;
    }

    serial_write(fd, &send_com[3], 2);
    debug_line("Step4: Send sync DA command:%02X%02X", send_com[3], send_com[4]);
    ret = serial_read(fd, sync_ack, 3, 500);
    if (ret > 0 && (sync_ack[0] == send_com[5] && sync_ack[1] == 0x69 && sync_ack[2] == 0x69)) {
        debug_line("[Receive sync DA command feedback:] success:%02X%02X%02X", sync_ack[0], sync_ack[1], sync_ack[2]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X%02X%02X", sync_ack[0], sync_ack[1], sync_ack[2]);
        return -1;
    }

    serial_write(fd, &send_com[5], 2);
    debug_line("Step5: Send sync DA command:%02X%02X", send_com[5], send_com[6]);
    ret = serial_read(fd, sync_ack, 3, 500);
    if (ret > 0 && (sync_ack[0] == 0x69)) {
        debug_line("[Receive sync DA command feedback:] success:%02X", sync_ack[0]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X", sync_ack[0]);
        return -1;
    }

    serial_write(fd, &send_com[5], 1);
    debug_line("Step6: Send sync DA command:%02X", send_com[5]);
    ret = serial_read(fd, sync_ack, 1, 500);
    if (ret > 0 && (sync_ack[0] == 0x69)) {
        debug_line("[Receive sync DA command feedback:] success:%02X", sync_ack[0]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X", sync_ack[0]);
        return -1;
    }

    serial_write(fd, &send_com[5], 1);
    debug_line("Step7: Send sync DA command:%02X", send_com[5]);
    usleep(5 * 1000);

    serial_write(fd, &send_com[2], 1);
    debug_line("Step8: Send sync DA command:%02X", send_com[2]);
    ret = serial_read(fd, sync_ack, 1, 500);
    if (ret > 0 && (sync_ack[0] == send_com[2])) {
        debug_line("[Receive sync DA command feedback:] success:%02X", sync_ack[0]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X", sync_ack[0]);
        return -1;
    }

    serial_write(fd, &send_com[5], 1);
    debug_line("Step9: Send sync DA command:%02X", send_com[5]);
    ret = serial_read(fd, sync_ack, 2, 500);
    if (ret > 0 && (sync_ack[0] == send_com[5] && sync_ack[1] == 0x69)) {
        debug_line("[Receive sync DA command feedback:] success:%02X%02X", sync_ack[0], sync_ack[1]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X%02X", sync_ack[0], sync_ack[1]);
        return -1;
    }

    serial_write(fd, &send_com[5], 1);
    debug_line("Step10: Send sync DA command:%02X", send_com[5]);
    ret = serial_read(fd, sync_ack, 2, 500);
    if (ret > 0 && (sync_ack[0] == send_com[4] && sync_ack[1] == 0xef)) {
        debug_line("[Receive sync DA command feedback:] success:%02X%02X", sync_ack[0], sync_ack[1]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X%02X", sync_ack[0], sync_ack[1]);
        return -1;
    }

    ret = serial_read(fd, sync_ack, 2, 500);
    if (ret > 0 && (sync_ack[0] == send_com[4] && sync_ack[1] == 0x60)) {
        debug_line("[Receive sync DA command feedback:] success:%02X%02X", sync_ack[0], sync_ack[1]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X%02X", sync_ack[0], sync_ack[1]);
        return -1;
    }

    ret = serial_read(fd, sync_ack, 2, 500);
    if (ret > 0 && (sync_ack[0] == send_com[4] && sync_ack[1] == 0x16)) {
        debug_line("[Receive sync DA command feedback:] success:%02X%02X", sync_ack[0], sync_ack[1]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X%02X", sync_ack[0], sync_ack[1]);
        return -1;
    }

    ret = serial_read(fd, sync_ack, 4, 500);
    if (ret > 0 && (sync_ack[0] == send_com[4] && sync_ack[1] == send_com[4] && sync_ack[2] == send_com[4] && sync_ack[3] == send_com[4])) {
        debug_line("[Receive sync DA command feedback:] success:%02X%02X%02X%02X", sync_ack[0], sync_ack[1], sync_ack[2], sync_ack[3]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X%02X%02X%02X", sync_ack[0], sync_ack[1], sync_ack[2], sync_ack[3]);
        return -1;
    }

    ret = serial_read(fd, sync_ack, 4, 500);
    if (ret > 0 && (sync_ack[0] == 0x08 && sync_ack[1] == send_com[4] && sync_ack[2] == send_com[4] && sync_ack[3] == send_com[4])) {
        debug_line("[Receive sync DA command feedback:] success:%02X%02X%02X%02X", sync_ack[0], sync_ack[1], sync_ack[2], sync_ack[3]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X%02X%02X%02X", sync_ack[0], sync_ack[1], sync_ack[2], sync_ack[3]);
        return -1;
    }

    ret = serial_read(fd, sync_ack, 4, 500);
    if (ret > 0 && (sync_ack[0] == send_com[4] && sync_ack[1] == 0x40 && sync_ack[2] == send_com[4] && sync_ack[3] == send_com[4])) {
        debug_line("[Receive sync DA command feedback:] success:%02X%02X%02X%02X", sync_ack[0], sync_ack[1], sync_ack[2], sync_ack[3]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X%02X%02X%02X", sync_ack[0], sync_ack[1], sync_ack[2], sync_ack[3]);
        return -1;
    }

    ret = serial_read(fd, sync_ack, 1, 500);
    if (ret > 0 && (sync_ack[0] == send_com[5])) {
        debug_line("[Receive sync DA command feedback:] success:%02X%02X%02X%02X", sync_ack[0], sync_ack[1], sync_ack[2], sync_ack[3]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X%02X%02X%02X", sync_ack[0], sync_ack[1], sync_ack[2], sync_ack[3]);
        return -1;
    }

    serial_write(fd, &send_com[5], 1);
    debug_line("Step11: Send sync DA command:%02X", send_com[5]);

    return 0;
}

static int format_flash(int fd)
{
    //send 0xd400
    //send format physical address, 4bytes
    //send format length, 4bytes
    //wait 0x5A5A?
    //wait 0x00000BCD
    //wait format progress 1bytes

    //loop send 0x5A, wait 0x00000BCD, wait format progress
    //send 0x5A, wait 0x5A
    unsigned char format_flash[10] = {0xD4, 0x00};
    unsigned char format_flag = 0x5A;
    unsigned char ack = 0x00;
    unsigned char format_ack[5] = {0};
    int ret = 0, i = 0;

    format_flash[2] = (unsigned char)(ROM_PARTITION_TABLE >> 24);
    format_flash[3] = (unsigned char)(ROM_PARTITION_TABLE >> 16);
    format_flash[4] = (unsigned char)(ROM_PARTITION_TABLE >> 8);
    format_flash[5] = (unsigned char)ROM_PARTITION_TABLE;

    format_flash[6] = (unsigned char)(FORMAT_LENGTH >> 24);
    format_flash[7] = (unsigned char)(FORMAT_LENGTH >> 16);
    format_flash[8] = (unsigned char)(FORMAT_LENGTH >> 8);
    format_flash[9] = (unsigned char)FORMAT_LENGTH;

    serial_write(fd, format_flash, 10);
    debug_line("Step1: Send format flash command 10 bytes");
    ret = serial_read(fd, format_ack, 2, 500);
    if (ret > 0 && (format_ack[0] == format_flag && format_ack[1] == format_flag)) {
        debug_line("[Receive sync DA command feedback:] success:%02X%02X", format_ack[0], format_ack[1]);
    } else {
        debug_line("[Receive sync DA command feedback:] fail %02X%02X", format_ack[0], format_ack[1]);
        return -1;
    }

    for (i = 0; i < 99; i++) {
        ack++;
        ret = serial_read(fd, format_ack, 5, 500);
        if (ret > 0 && (format_ack[0] == 0x00 && format_ack[1] == 0x00 && format_ack[2] == 0x0B
                        && format_ack[3] == 0xCD && format_ack[4] == ack)) {
            debug_line("[Receive format flash command feedback:] success:%02X%02X%02X%02X%02X", format_ack[0], format_ack[1], format_ack[2], format_ack[3], format_ack[4]);
        } else {
            debug_line("[Receive format flash command feedback:] fail %02X%02X%02X%02X%02X", format_ack[0], format_ack[1], format_ack[2], format_ack[3], format_ack[4]);
            return -1;
        }
        serial_write(fd, &format_flag, 1);
        debug_line("Step2: Send format flash command:%02X", format_flag);
    }

    ret = serial_read(fd, format_ack, 5, 500);
    if (ret > 0 && (format_ack[0] == 0x00 && format_ack[1] == 0x00 && format_ack[2] == 0x00
                    && format_ack[3] == 0x00 && format_ack[4] == 0x64)) {
        debug_line("[Receive format flash command feedback:] success:%02X%02X%02X%02X%02X", format_ack[0], format_ack[1], format_ack[2], format_ack[3], format_ack[4]);
    } else {
        debug_line("[Receive format flash command feedback:] fail %02X%02X%02X%02X%02X", format_ack[0], format_ack[1], format_ack[2], format_ack[3], format_ack[4]);
        return -1;
    }

    serial_write(fd, &format_flag, 1);
    debug_line("Step3: Send format flash command:%02X", format_flag);
    ret = serial_read(fd, format_ack, 1, 500);
    if (ret > 0 && (format_ack[0] == format_flag)) {
        debug_line("[Receive format flash command feedback:] success:%02X", format_ack[0]);
    } else {
        debug_line("[Receive format flash command feedback:] fail %02X", format_ack[0]);
        return -1;
    }

    return 0;
}

static unsigned int simple_checksum(unsigned char *buf, unsigned int buf_len)
{
    unsigned int checksum = 0;
    unsigned int i = 0;

    if (buf == NULL || buf_len == 0) {
        return 0;
    }
    for (i = 0; i < buf_len; i++) {
        checksum += *(buf + i);
    }
    return checksum;
}

static int send_fw_to_module(int fd, unsigned int rom, const char *path)
{
    //send 0xb2
    //send fw flash address, 4bytes
    //send fw total length, 4bytes
    //send fw packet length, 4bytes
    //wait 0x5A5A?

    //loop send fw packet 4096bytes
    //send checksum, 4bytes, wait 0x69

    //wait 0x5A
    //send fw checksum , 4bytes
    //wait 0x5A

    //send 0x5A(bootloader), or 0xA5(other), wait 0x5A
    unsigned char partition_table[5] = {0xB2};
    unsigned char fw_all_length[4] = {0};
    unsigned char fw_length[4] = {0x00, 0x00, 0x10, 0x00};
    unsigned char download_flag[3] = {0x5A, 0xA5, 0x69};
    unsigned char download_ack[2] = {0};
    FILE *fp = NULL;
    unsigned int len = 0;
    int send_times = 0, i = 0, j = 0;
    int ret = 0;
    unsigned char buffer[4096] = {0};
    unsigned char checksum[4] = {0};
    unsigned int fw_checksum = 0;
    unsigned int packet_checksum = 0;

    partition_table[1] = (unsigned char)(rom >> 24);
    partition_table[2] = (unsigned char)(rom >> 16);
    partition_table[3] = (unsigned char)(rom >> 8);
    partition_table[4] = (unsigned char)rom;

    serial_write(fd, partition_table, 5);
    debug_line("Step1: Send download fw to flash command 5 bytes");
    usleep(5 * 1000);
    fp = fopen(path, "rb+");
    if (fp == NULL) {
        debug_line("Open %s fail!!!", path);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    send_times = len / 4096;
    if (len % 4096 != 0) {
        for (i = 0; i < 4; i++) {
            fw_all_length[3 - i] = (((send_times + 1) * 4096) >> 8 * i);
        }
        send_times += 1;
    } else {
        for (i = 0; i < 4; i++) {
            fw_all_length[3 - i] = ((send_times * 4096) >> 8 * i);
        }
    }
    serial_write(fd, fw_all_length, 4);
    debug_line("Step2: Send download fw to flash command.fw_all_length=0x%02x%02x%02x%02x", fw_all_length[0], fw_all_length[1], fw_all_length[2], fw_all_length[3]);
    usleep(5 * 1000);
    serial_write(fd, fw_length, 4);
    usleep(5 * 1000);
    debug_line("Step3: Send download fw to flash command.fw_length=0x%02x%02x%02x%02x", fw_length[0], fw_length[1], fw_length[2], fw_length[3]);
    ret = serial_read(fd, download_ack, 2, 500);
    if (ret > 0 && (download_ack[0] == download_flag[0] && download_ack[1] == download_flag[0])) {
        debug_line("[Receive download fw to flash command feedback:] success:%02X%02X", download_ack[0], download_ack[1]);
    } else {
        debug_line("[Receive download fw to flash command feedback:] fail %02X%02X", download_ack[0], download_ack[1]);
        fclose(fp);
        return -1;
    }

    for (i = 0; i < send_times; i++) {
        int bytes = (len - i * 4096) >= 4096 ?  4096 : len % 4096;
        fseek(fp, i * 4096, SEEK_SET);
        fread(buffer, 1, bytes, fp);
        if (bytes < 4096) {
            memset(buffer + bytes, 0xff, 4096 - bytes);
        }
        //usleep(20 * 1000);
        debug_line("Step4: Send download %d/%d fw to flash command:", i + 1, send_times);
        serial_write(fd, buffer, 4096);
        //send checksum
        packet_checksum = simple_checksum(buffer, 4096);
        fw_checksum += packet_checksum;
        for (j = 0; j < 4; j++) {
            checksum[3 - j] = (packet_checksum >> 8 * j);
        }
        serial_write(fd, checksum, 4);
        debug_line("Step5: Send download fw to flash command");
        //wait 0x69
        if (i == (send_times - 1)) {
            ret = serial_read(fd, download_ack, 2, 5000);
            if (ret > 0 && (download_ack[0] == 0x69 && download_ack[1] == 0x5A)) {
                debug_line("[Receive download fw to flash command feedback:] success:%02X%02X", download_ack[0], download_ack[1]);
            } else {
                debug_line("[Receive download fw to flash command feedback:] fail %02X%02X", download_ack[0], download_ack[1]);
                fclose(fp);
                return -1;
            }
        } else {
            ret = serial_read(fd, download_ack, 1, 5000);
            if (ret > 0 && (download_ack[0] == 0x69)) {
                debug_line("[Receive download fw to flash command feedback:] success:%02X", download_ack[0]);
            } else {
                debug_line("[Receive download fw to flash command feedback:] fail %02X", download_ack[0]);
                fclose(fp);
                return -1;
            }
        }
    }
    fclose(fp);
    //send total checksum
    for (i = 0; i < 4; i++) {
        checksum[3 - i] = (fw_checksum >> 8 * i);
    }
    serial_write(fd, checksum, 4);
    debug_line("Step6: Send download fw to flash command");
    ret = serial_read(fd, download_ack, 1, 5000);
    if (ret > 0 && (download_ack[0] == download_flag[0])) {
        debug_line("[Receive download fw to flash command feedback:] success:%02X", download_ack[0]);
    } else {
        debug_line("[Receive download fw to flash command feedback:] fail %02X", download_ack[0]);
        return -1;
    }

    if (rom == ROM_BL) {
        serial_write(fd, &download_flag[0], 1);
    } else {
        serial_write(fd, &download_flag[1], 1);
    }
    debug_line("Step7: Send download fw to flash command");
    ret = serial_read(fd, download_ack, 1, 5000);
    if (ret > 0 && (download_ack[0] == download_flag[0])) {
        debug_line("[Receive download fw to flash command feedback:] success:%02X", download_ack[0]);
    } else {
        debug_line("[Receive download fw to flash command feedback:] fail %02X", download_ack[0]);
        return -1;
    }

    return 0;
}


int do_ota(const char *dev_name, const char *dir)
{
    int fd = 0;
    char path[128] = {0};

    if (serial_init(dev_name, &fd, 0, 115200, 0) < 0) {
        return -1;
    }
    if (do_handshake(fd) < 0) {
        goto fail;
    }
    if (disable_watchdog(fd) < 0) {
        goto fail;
    }
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "%s/%s", dir, "slave_da_UART115200.bin");
    if (send_da_file(fd, path) < 0) {
        goto fail;
    }
    if (jump_to_da(fd) < 0) {
        goto fail;
    }
    if (synchronize_with_da(fd) < 0) {
        goto fail;
    }
    if (format_flash(fd) < 0) {
        goto fail;
    }
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "%s/%s", dir, "partition_table.bin");
    if (send_fw_to_module(fd, ROM_PARTITION_TABLE, path) < 0) {
        debug_line("send partition_table.bin failed");
        goto fail;
    }
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "%s/%s", dir, "ag3335_bootloader.bin");
    if (send_fw_to_module(fd, ROM_BL, path) < 0) {
        debug_line("send ag3335_bootloader.bin failed");
        goto fail;
    }
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "%s/%s", dir, "dr.bin");
    if (send_fw_to_module(fd, ROM_RTOS, path) < 0) {
        debug_line("send dr.bin failed");
        goto fail;
    }
    usleep(1000 * 1000);
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "%s/%s", dir, "gnss_config.bin");
    if (send_fw_to_module(fd, ROM_GNSS_CONFIG, path) < 0) {
        debug_line("send gnss_config.bin failed");
        goto fail;
    }
    serial_close(fd);
    debug_line("upgrade successly~~~");

    return 0;
fail:
    serial_close(fd);
    return -1;
}

