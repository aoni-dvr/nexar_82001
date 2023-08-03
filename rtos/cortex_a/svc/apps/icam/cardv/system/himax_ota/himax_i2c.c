#include "himax_i2c.h"
#include "platform.h"

#define I2C_Buffer_length 512
static unsigned char i2c_buf[I2C_Buffer_length]={0};
static unsigned char slave_addr = 0xc4;

static unsigned short crc16_ccitt(unsigned char *data_p, int length) 
{ 
	unsigned char i;
	unsigned int data;
	unsigned int crc = 0xffff;
#define POLY 0x8408 /* 1021H bit reversed */

	if (length == 0) {
		return (unsigned short)(crc);
    }
	do {
		for (i = 0, data = (unsigned int)0xff & *data_p++; i < 8; i++, data >>= 1) {
			if ((crc & 0x0001) ^ (data & 0x0001)) {
				crc = (crc >> 1) ^ POLY;
			} else {
				crc >>= 1;
            }
		}
	} while (--length);

	return (unsigned short)(crc);
}

void himax_i2c_set_slave_addr(unsigned char addr)
{
    slave_addr = addr;
}

int himax_i2c_send_data(unsigned char *data, unsigned int len)
{
    i2c_write(MAIN_MCU_I2C_CHANNEL, slave_addr, data, len);

    return 0;
}

static void debug_data(const unsigned char *data, unsigned int len)
{
#define ONE_LINE_NUM (64)
    int i = 0, j = 0, lines = 0;
    char buf[512] = {0};

    lines = len / ONE_LINE_NUM;
    if (len % ONE_LINE_NUM != 0) {
        lines += 1;
    }
    for (i = 0; i < lines; i++) {
        int num = (len - i * ONE_LINE_NUM) >= ONE_LINE_NUM ? ONE_LINE_NUM : len % ONE_LINE_NUM;
        memset(buf, 0, sizeof(buf));
        for (j = 0; j < num; j++) {
            snprintf(buf + strlen(buf), 4, "%.2X ", *(data + i * ONE_LINE_NUM + j));
        }
        debug_line("%s", buf);
    }
}

int himax_i2c_send_command(unsigned char feature, unsigned char type, unsigned char* data, unsigned int data_len)
{
	int total_len = 0;
	int checksum_offset = 0;
	unsigned short checksum = 0;

	memset(&i2c_buf, 0, sizeof(unsigned char) * I2C_Buffer_length);

    checksum_offset = I2CCOMM_HEADER_SIZE + data_len;
	total_len = checksum_offset + I2CFMT_CHECKSUM_SIZE;	

	i2c_buf[I2CFMT_FEATURE_OFFSET] = feature;
	i2c_buf[I2CFMT_COMMAND_OFFSET] = type;
	if (data_len != 0) {
		memcpy(&i2c_buf[I2CFMT_LENGTH_OFFSET], &data_len, 2);
		memcpy(&i2c_buf[I2CCOMM_PAYLOAD_OFFSET], data, data_len);
	}
	checksum = crc16_ccitt(i2c_buf, I2CCOMM_HEADER_SIZE + data_len);
	i2c_buf[checksum_offset] = checksum & 0xFF;
	i2c_buf[checksum_offset + 1] = (checksum >> 8) & 0xFF;

    //debug_line("himax_i2c_send_command:");
    //debug_data(i2c_buf, total_len);

	himax_i2c_send_data(i2c_buf, total_len);//start & stop

    return 0;
}

unsigned int himax_i2c_read_data(unsigned int data_len)
{
    unsigned char data[128] = {0};
	unsigned int ret = 0, ActualSize = 0;
	int chip_id_0 = 0xffffffff, chip_id_1 = 0xffffffff;
    AMBA_I2C_TRANSACTION_s rx_ctrl;

    rx_ctrl.SlaveAddr = slave_addr;
    rx_ctrl.DataSize = data_len;
    rx_ctrl.pDataBuf = data;
    AmbaI2C_MasterRead(MAIN_MCU_I2C_CHANNEL, I2C_SPEED_FAST, &rx_ctrl, &ActualSize, 5000);
    debug_line("himax_i2c_read_data:");
    debug_data(data, data_len);
	if ((data[data_len - 2] | (data[data_len - 1] << 8)) == 0xFFFF) {
		debug_line("Device still Updating , waitting for device response!");
		return -1;
	} else if ((data[data_len - 2] | (data[data_len - 1] << 8)) != crc16_ccitt(data, data_len - 2)) {
		debug_line("check sum error");
		return -1;
	} else {
		if (data_len > 10) {
			chip_id_0 = data[7]<<24 | data[6] <<16 | data[5] <<8 | data[4] ;
			chip_id_1 = data[11]<<24 | data[10] <<16 | data[9] <<8 | data[8] ;
			debug_line("CHIP ID : 0x%.8x %.8x", chip_id_0, chip_id_1);
			return 0;
		} else {
			ret = data[7] << 24 | data[6] << 16 | data[5] << 8 | data[4] ;
        }
		return ret;
	}

    return 0;
}

