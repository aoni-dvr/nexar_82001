#ifndef __HIMAX_I2C_H__
#define __HIMAX_I2C_H__

#define I2CFMT_FEATURE_OFFSET	0
#define I2CFMT_FEATURE_SIZE	1

#define I2CFMT_COMMAND_OFFSET	1
#define I2CFMT_COMMAND_SIZE	1

#define I2CFMT_LENGTH_OFFSET	2
#define I2CFMT_LENGTH_SIZE	2

#define I2CCOMM_HEADER_SIZE		4
#define I2CCOMM_ADDRESS_SIZE		1

#define I2CCOMM_PAYLOAD_COMMAND_SIZE		4
#define I2CCOMM_PAYLOAD_DATA_SIZE		256

#define I2CFMT_CHECKSUM_SIZE	2
#define I2CFMT_CHECHSUM_OFFSET	8
#define I2CFMT_CHECHSUM_PAYLOADLEN_OFFSET	(I2CFMT_CHECHSUM_OFFSET+I2CCOMM_PAYLOAD_DATA_SIZE)

#define I2CCOMM_PAYLOAD_OFFSET	4

#define I2C_BUFFER_LENGTH			I2CCOMM_HEADER_SIZE+I2CCOMM_PAYLOAD_DATA_SIZE+I2CFMT_CHECKSUM_SIZE

#define slv_addr 0x62	

struct _i2c_data{
    unsigned char address;
    unsigned char buf[I2C_BUFFER_LENGTH];
};

#define MAIN_MCU_OTA_I2C_SLAVE_ADDR 0xc4

int himax_i2c_send_data(unsigned char *data, unsigned int len);
int himax_i2c_send_command(unsigned char feature, unsigned char type, unsigned char* data, unsigned int data_len);
unsigned int himax_i2c_read_data(unsigned int data_len);
void himax_i2c_set_slave_addr(unsigned char addr);

#endif//__HIMAX_I2C_H__

