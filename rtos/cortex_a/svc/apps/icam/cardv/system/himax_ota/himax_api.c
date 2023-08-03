#include "himax_api.h"
#include "himax_i2c.h"
#include "platform.h"
#include "app_helper.h"

char* ota_error_string[] = {
	"OTA_NO_ERROR",								
	"OTA_ERROR_SEC_INIT",					
	"OTA_ERROR_SEC_TRNG",				
	"OTA_ERROR_CHECKSUM",
	"OTA_ERROR_I2C_DRV"
	"OTA_ERROR_SEC_VERIFY_ERROR",			
	"OTA_ERROR_FLASH_CRC_CHECK_FAIL",						
	"OTA_ERROR_UNKNOWN",	
};

char* ota_status_string[] = {
	"OTA_STATUS_APPLICATION_IDLE",	
	"OTA_STATUS_OTA_LOADER_IDLE",
	"OTA_STATUS_OTA_EIP130_INIT",	
	"OTA_STATUS_OTA_DATA_VERIFY",	
	"OTA_STATUS_OTA_FLASH_CHECK",
	"OTA_STATUS_OTA_DONE",
	"OTA_STATUS_OTA_LOADER_INIT"
	"OTA_STATUS_OTA_LOADER_RECEIVE"
	"OTA_STATUS_OTA_LOADER_BURN_FLASH"
};

int himax_ota_send_command(unsigned char feature, unsigned char type,unsigned int data_len, unsigned char* data)
{
	himax_i2c_send_command(feature, type, data, data_len);

    return 0;
}

int himax_api_send_op_command(unsigned char feature, OTA_CMD_TYPE cmd)
{	
	himax_ota_send_command(feature, cmd, 0, NULL);

    return 0;
}

void himax_api_send_upg_command(unsigned char feature, OTA_CMD_TYPE cmd, wei_memory_dsp_item_ptr dsp_data)
{
	himax_ota_send_command(feature, cmd, sizeof(wei_memory_dsp_item_t), (unsigned char *)dsp_data);	
}

void himax_api_send_upg_file(unsigned char feature, OTA_CMD_TYPE cmd, wei_memory_dsp_item_ptr dsp_data, unsigned char *image_data)
{	
    unsigned char *upgrade_data = (unsigned char *)(image_data + dsp_data->pat_addr);
    int sent_length = 0, count = 0, count_length = 0, data_length = 0;

    //memset(upgrade_data, 0, dsp_data->pat_size);
    //memcpy(upgrade_data, &image_data[dsp_data->pat_addr], dsp_data->pat_size);
    data_length = dsp_data->pat_size;
    while (count_length < data_length) {
        if (data_length - count_length > 256) {
            sent_length = 256;
        } else {
            sent_length = (data_length - count_length);
        }
        himax_ota_send_command(feature, cmd, sent_length, upgrade_data + count * 256);
        count_length += sent_length;
        debug_line("himax_api_send_upg_file %d/%d, %d%%", count_length, data_length, count_length * 100 / data_length);
        count++;
        msleep(10);
    }
    msleep(14);
}

int himax_api_read_data(unsigned char type)
{
    msleep(3);
    return himax_i2c_read_data(10);
}

int himax_api_print_result(unsigned int result)
{
	OTA_STATUS_E ota_status = result >> 16 & 0xFF;
	OTA_ERROR_E ota_error_status = result & 0xFF;
	debug_line("\n===>%s", ota_status_string[ota_status]);
	if (ota_error_status != OTA_NO_ERROR) {
		debug_line("- %s",ota_error_string[ota_error_status]);
		if (ota_status == OTA_STATUS_OTA_EIP130_INIT) {
			debug_line("Device EIP130 init fail, please check");
		} else if(ota_status == OTA_STATUS_OTA_DATA_VERIFY) {
			debug_line("Device verify fail, please check device's otp or image signature");
        }
		return 1;  //resent ota upgrade data
	} else if(ota_status == 1) {
		return 2;
    }
	return 0;
}

void himax_api_waitting_time(wei_memory_dsp_item_ptr dsp_data)
{
	unsigned int data_length = 0;

	debug_line("\n");
	if (dsp_data->pat_size <= 0x10000) {
    	msleep(1500);//1000000		 g_d_i2c_g_dt
	} else {
		for(data_length = 0; data_length < dsp_data->pat_size; data_length += 0x10000) {
			msleep(1500);//1000000		
		}
	}
}

char* ota_pat_type_string[] = {
	"1st_LOADER",
	"2nd_LOADER",
	"3rd_LOADER",
	"MEMDSP",
	"APPLICATION",
	"APPLICATION_CONFIG",
	"CUST_ALGO_CONFIG",
	"EMZA_CONFIG",
	"CNN_LUT",
	"LOGER",
	"AUDIO_DATA",
	"FR_DATA",
	"STANDALONE",
	"STANDALONE_RAW",
	"FLASHASSRAM",
	"ALANGO",
	"WATERMETER_CONF",
	"ALGORITHM_MODEL",
	"LOADER_CONFIG",
};

char* ota_sec_format_string[] = {
	"RAW",
	"BLp",
	"BLw",
	"Hmac",
};

#define ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))

unsigned int himax_api_parse_memdsp(unsigned char* num_sbs, wei_memory_dsp_item_ptr memory_dsp_item_ptr, unsigned int *index)
{
	int filelen = 0, i = 0, count = 0;
    struct stat fstat = {0};
    FILE *dsp_fp = NULL;
    unsigned char dsp_buf[64 * 1024] = {0};
	static memory_dsp_t memory_dsp_header;
	unsigned int temp_addr = 0, temp_size = 0,sector_count = 3;

    if (stat(SD_SLOT":\\layout.bin", &fstat) != 0) {
		debug_line("can't find layout.bin, please put layout.bin in img folder.exit");
        return -1;
    }
    filelen = (int)(fstat.size);
    debug_line("read layout.bin, size=%d", filelen);
    dsp_fp = fopen(SD_SLOT":\\layout.bin", "rb");
    if (dsp_fp == NULL) {
        debug_line("can't find layout.bin, please put layout.bin in img folder");
        return -1;
    }
    fread(dsp_buf, 1, filelen, dsp_fp);
    fclose(dsp_fp);

    memset(&memory_dsp_header, 0x00, sizeof(memory_dsp_t));
	memset(index, 0xFF, sizeof(int) * 128);
	memcpy((void *)&memory_dsp_header, (void *)dsp_buf, filelen);
	*num_sbs = memory_dsp_header.info.num_sbs;
	for (i = 0; i < memory_dsp_header.info.num_sbs; i++) {
		index[i] = count + 1;
		memcpy((void *)&memory_dsp_item_ptr[i], (void *)&memory_dsp_header.item[i], sizeof(wei_memory_dsp_item_t));
		if (memory_dsp_header.item[i].pat_type == 0 || memory_dsp_header.item[i].pat_type == 1) {
			count++;
			continue;
		}
		if ((memory_dsp_header.item[i].pat_att & 0x10) == 0x0) {
			debug_line("%d.[%s - %s]:\n\t start_addr:[0x%.8x] pat_size:[0x%.8x]",
                        sector_count,
                        ota_pat_type_string[memory_dsp_header.item[i].pat_type],
                        ota_sec_format_string[memory_dsp_header.item[i].pat_att&0xF],
                        memory_dsp_header.item[i].pat_addr,
                        memory_dsp_header.item[i].pat_size);
			count++;
			sector_count++;
		} else {
			if ((memory_dsp_header.item[i].pat_att & 0xE0) == 0x80){
				temp_addr = 0;
				temp_size = 0;
				temp_addr = memory_dsp_header.item[i].pat_addr;	
			}
			temp_size += memory_dsp_header.item[i].pat_size;
			if ((memory_dsp_header.item[i].pat_att & 0xE0) == 0x20){
				debug_line("%d.[%s - %s]:\n\t start_addr:[0x%08x] pat_size:[0x%08x]",
                            sector_count,
                            ota_pat_type_string[memory_dsp_header.item[i].pat_type],
                            ota_sec_format_string[memory_dsp_header.item[i].pat_att&0xF],
                            temp_addr,
                            temp_size);
				count++;
				sector_count++;
			}
		}
	}
	sector_count -=1;
	return sector_count;
}

