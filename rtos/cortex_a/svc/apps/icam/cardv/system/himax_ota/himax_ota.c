#include "himax_api.h"
#include "app_base.h"
#include "himax_i2c.h"

#define DEBUG_TAG "[himax_ota]"

int himax_do_ota(void)
{
    int result = 0;
    FILE *img_fp = NULL;
    struct stat fstat = {0};
    int ret = 0;
    int sector_count = 0;
    unsigned char count = 0;
    unsigned char image_buf[1024 * 1024] = {0};
    static wei_memory_dsp_item_t g_memory_dsp_item[128];
    unsigned int list_index[128] = {0};
    unsigned char value = 0;
    unsigned int fail_times = 0;

    debug_line(DEBUG_TAG"enter ota");
    fail_times = 0;
    while (fail_times <= 200) {
        i2c_read_reg(MAIN_MCU_I2C_CHANNEL, 0xc4, 0x90, &value);
        if (value == 0xAA) {
            break;
        }
        msleep(50);
        fail_times += 1;
    }

    if (fail_times >= 200) {
        debug_line(DEBUG_TAG"%s, at line %d,failed", __func__, __LINE__);
        return -1;
    }

    fail_times = 0;
    while (fail_times <= 200) {
        debug_line(DEBUG_TAG"jump2upg");
    	himax_api_send_op_command(OTA_OPERATION, WEI_OTA_JUMP2UPG);
        debug_line(DEBUG_TAG"get result");
    	himax_api_send_op_command(OTA_OPERATION, WEI_OTA_GET_RESULT);
        msleep(5);
        result = himax_api_read_data(WEI_OTA_GET_RESULT);
        if (result == -1) {
            debug_line(DEBUG_TAG"no data receive.continue");
            msleep(50);
            fail_times += 1;
            continue;
        } else {
            ret = himax_api_print_result(result);
            debug_line(DEBUG_TAG"himax_api_print_result=%d", ret);
            if (ret != 2) {
                msleep(50);
                fail_times += 1;
                continue;
            }
            break;
        }
    }
    if (fail_times >= 200) {
        debug_line(DEBUG_TAG"%s, at line %d,failed", __func__, __LINE__);
        return -1;
    }

    if (stat(SD_SLOT":\\output.img", &fstat) != 0) {
		debug_line(DEBUG_TAG"can't find output.img, please put output.img in img folder.exit");
        return -1;
    }
    debug_line(DEBUG_TAG"read output.img, size=%d", (int)(fstat.size));
	img_fp = fopen(SD_SLOT":\\output.img", "rb");
	if (img_fp == NULL) {
		debug_line(DEBUG_TAG"can't find output.img, please put output.img in img folder.exit");
		return -1;
	}
	fread(image_buf, 1, (int)(fstat.size), img_fp);
    fclose(img_fp);

    debug_line(DEBUG_TAG"parse_memdsp");
    himax_api_parse_memdsp(&count, g_memory_dsp_item, list_index);

#if 0
	himax_api_send_op_command(OTA_OPERATION, WEI_OTA_GET_RESULT);
    msleep(5);
    result = himax_api_read_data(WEI_OTA_GET_RESULT);
    if (result == -1) {
        debug_line(DEBUG_TAG"no data receive.exit");
        return -1;
    } else {
        ret = himax_api_print_result(result);
    }
    debug_line(DEBUG_TAG"ret:%x result:%x\n", ret, result);
    if (ret != 2 || result == -1) {
        debug_line(DEBUG_TAG"ret != 2 || result == -1.exit");
        return -1;
    }
#endif

    for (sector_count = 2; sector_count < count; sector_count++) {
        debug_line(DEBUG_TAG"pat_type:[0x%.8x] pat_att:[0x%0.8x] start_addr:[0x%.8x] pat_size:[0x%.8x]",
                    g_memory_dsp_item[sector_count].pat_type, g_memory_dsp_item[sector_count].pat_att,
                    g_memory_dsp_item[sector_count].pat_addr, g_memory_dsp_item[sector_count].pat_size);
        himax_api_send_upg_command(OTA_UPG, WEI_OTA_START, &g_memory_dsp_item[sector_count]);
        msleep(5);
        himax_api_send_upg_file(OTA_UPG, WEI_OTA_DATA, &g_memory_dsp_item[sector_count], image_buf);		
		himax_ota_send_command(OTA_UPG, WEI_OTA_END, 0, NULL);
        himax_api_waitting_time(&g_memory_dsp_item[sector_count]);
        debug_line(DEBUG_TAG"get result");
        himax_api_send_op_command(OTA_OPERATION, WEI_OTA_GET_RESULT);
        msleep(5);
        result = himax_api_read_data(WEI_OTA_GET_RESULT);
        if (result == -1) {
            debug_line(DEBUG_TAG"no data receive.exit");
            return -1;
        } else {
            himax_api_print_result(result);
        }
    }
    msleep(1200);
    debug_line(DEBUG_TAG"send file finished, get result");
	himax_api_send_op_command(OTA_OPERATION, WEI_OTA_GET_RESULT);
    msleep(5);
	result = himax_api_read_data(WEI_OTA_GET_RESULT);
	if (result == -1) {
        debug_line(DEBUG_TAG"no data receive");
	} else {
		ret = himax_api_print_result(result);
    }
	debug_line(DEBUG_TAG"ret=%d, result=%d", ret, result);
	if (ret == 2 && result != -1){
		debug_line(DEBUG_TAG"reboot WEI");
		himax_api_send_op_command(OTA_OPERATION, WEI_OTA_REBOOT);
	}

	return 0;
}

