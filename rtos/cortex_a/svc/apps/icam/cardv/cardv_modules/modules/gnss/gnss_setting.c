#include"gnss_setting.h"

static void cal_checksum(unsigned char *data_buf, int len)
{
    int i = 0;

    data_buf[len -2] = 0;
    data_buf[len - 1] = 0;
    for (i = 2;i < len - 2;i++) {
        data_buf[len - 2] = data_buf[len - 2] + data_buf[i];
        data_buf[len - 1] = data_buf[len - 1] + data_buf[len - 2];
    }
    data_buf[len - 2] &= 0XFF;
    data_buf[len - 1] &= 0xFF;
}

void gnss_set_satellite_id(UINT8 id)
{
    unsigned char data_buf[] = {0xF1,0xD9,0x06,0x15,0x03,0x00,0x02,0x00,0x0A,0x2A,0xC7};
    int temp = 0;

    if (id & 0x02) {
        temp |= 0x0002;
    }
    if (id & 0x04) {
        temp |= 0x0004;
    }
    if (id & 0x08) {
        temp |= 0x0008;
    }
    if (id & 0x10) {
        temp |= 0x0010;
    }
    if (id & 0x20) {
        temp |= 0x0020;
    }
    if (id & 0x40) {
        temp |= 0x0040;
    }
    data_buf[6] = temp & 0xFF;
    data_buf[7] = (temp >> 8) & 0xFF;
    cal_checksum(data_buf, sizeof(data_buf) / sizeof(data_buf[0]));
    gnss_task_uart_write(data_buf, sizeof(data_buf) / sizeof(data_buf[0]));
}

void gnss_set_boot_mode(UINT8 mode)
{
    unsigned char data_buf[] = {0xF1,0xD9,0x06,0x40,0x01,0x00,0x01,0x48,0x22};

    if (mode == 0x01) {
        data_buf[6] = 0x01;
    } else if (mode == 0x02) {
        data_buf[6] = 0x02;
    } else if (mode == 0x03) {
        data_buf[6] = 0x03;
    } else {
        debug_line("boot mode error");  
    }
    cal_checksum(data_buf, sizeof(data_buf) / sizeof(data_buf[0]));
    gnss_task_uart_write(data_buf, sizeof(data_buf) / sizeof(data_buf[0]));
}

void gnss_set_data_rate(UINT8 rate)
{
    unsigned char data_rate_buf[] = {0xF1,0xD9,0x06,0x42,0x14,0x00,0x00,0x0A,0x38,0x00,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x24};

    if (rate == 1) {
        data_rate_buf[7]    = 0x01;
        data_rate_buf[8]    = 0x38;
        data_rate_buf[9]    = 0x35;
        data_rate_buf[10]   = 0xE8;
        data_rate_buf[11]   = 0x03;
    } else if(rate == 5) {
        data_rate_buf[7]    = 0x05;
        data_rate_buf[8]    = 0x38;
        data_rate_buf[9]    = 0x00;
        data_rate_buf[10]   = 0xC8;
        data_rate_buf[11]   = 0x00;
    } else if(rate == 10) {
        data_rate_buf[7]    = 0x0A;
        data_rate_buf[8]    = 0x38;
        data_rate_buf[9]    = 0x00;
        data_rate_buf[10]   = 0x64;
        data_rate_buf[11]   = 0x00;
    } else {
        debug_line("no data");
    }
    cal_checksum(data_rate_buf, sizeof(data_rate_buf) / sizeof(data_rate_buf[0]));
    gnss_task_uart_write(data_rate_buf, sizeof(data_rate_buf) / sizeof(data_rate_buf[0]));
}

void gnss_set_dsp_uart_baudrate(UINT8 rate)
{
    AMBA_UART_CONFIG_s UartConfig = {0};
    SVC_USER_PREF_s *pSvcUserPref = NULL;
    unsigned char baudrate_9600[]   = {0xF1,0xD9,0x06,0x00,0x08,0x00,0x01,0x00,0x00,0x00,0x80,0x25,0x00,0x00,0xB4,0x0F};
    unsigned char baudrate_115200[] = {0xF1,0xD9,0x06,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0xC2,0x01,0x00,0xD1,0xE0};

    SvcUserPref_Get(&pSvcUserPref);
    pSvcUserPref->GnssSetting.UartBitRate = (rate >= 5) ? 115200 : 9600;
    if (pSvcUserPref->GnssSetting.UartBitRate == 115200) {
        gnss_task_uart_write(baudrate_115200, sizeof(baudrate_115200) / sizeof(baudrate_115200[0]));
    } else {
        gnss_task_uart_write(baudrate_9600, sizeof(baudrate_9600) / sizeof(baudrate_9600[0]));
    }
    UartConfig.NumDataBits   = AMBA_UART_DATA_8_BIT;
    UartConfig.ParityBitMode = AMBA_UART_PARITY_NONE;
    UartConfig.NumStopBits   = AMBA_UART_STOP_1_BIT;
    AmbaUART_Config(pSvcUserPref->GnssSetting.UartChannel, pSvcUserPref->GnssSetting.UartBitRate, &UartConfig);
}

void gnss_set_gst_enable(UINT8 enable)
{
    unsigned char data_buf[] = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x08, 0x00, 0x02, 0x1F};

    if (enable) {
        data_buf[8] = 0x01;
    } else {
        data_buf[8] = 0x00;
    }
    cal_checksum(data_buf, sizeof(data_buf) / sizeof(data_buf[0]));
    gnss_task_uart_write(data_buf, sizeof(data_buf) / sizeof(data_buf[0]));
}
