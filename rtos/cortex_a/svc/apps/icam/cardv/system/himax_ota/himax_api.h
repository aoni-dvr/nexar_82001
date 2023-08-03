#ifndef __HIMAX_API_H__
#define __HIMAX_API_H__

typedef enum {
	OTA_NO_ERROR=0,
	OTA_ERROR_SEC_INIT,
	OTA_ERROR_SEC_TRNG,
	OTA_ERROR_CHECKSUM,
	OTA_ERROR_SEC_VERIFY_ERROR,
	OTA_ERROR_FLASH_CRC_CHECK_FAIL,
	OTA_ERROR_UNKNOWN,								
} OTA_ERROR_E;

typedef enum {
    OTA_STATUS_APPLICATION_IDLE=0,
    OTA_STATUS_OTA_LOADER_IDLE,
    OTA_STATUS_OTA_EIP130_INIT,
    OTA_STATUS_OTA_DATA_VERIFY,
    OTA_STATUS_OTA_FLASH_CHECK,
    OTA_STATUS_OTA_DONE,
} OTA_STATUS_E;

typedef enum {
	OTA_OPERATION     =0x50,
	OTA_UPG           =0x51,
} OTA_TYPE_E;

typedef enum {
	WEI_SYS_GET_VER=0x0,
	WEI_SYS_GET_PROJID=0x1,
	WEI_OTA_START=0x2,
	WEI_OTA_DATA=0x3,
	WEI_OTA_END=0x4,
	WEI_OTA_GET_RESULT=0x5,
	WEI_OTA_REBOOT=0x6,	
	WEI_OTA_JUMP2UPG=0x7,
	WEI_OTA_GET_CHIPID=0x8,
	WEI_OTA_TURN_ON_ECI=0x9,
	WEI_OTA_TURN_OFF_ECI=0xA,
	WEI_OTA_TURN_OFF_PDM=0xB,
	WEI_OTA_AUDIO_RECOGNITION_INIT=0xC,
	WEI_OTA_AUDIO_RECOGNITION_RECV_DATA=0xD,
	WEI_OTA_AUDIO_RECOGNITION=0xE,	
	WEI_OTA_GET_AUDIO_RESULT=0xF,
	WEI_OTA_XIP_ENABLE=0x10,
	WEI_OTA_XIP_DISENABLE=0x11,	
	WEI_OTA_FLASH_DUMP=0x12,
	WEI_OTA_FLASH_ERASE_ALL=0x13,
	WEI_OTA_GET_ALGO_ID=0x14,
	WEI_OTA_ENABLE_SW_UART=0x15,
	WEI_OTA_DISABLE_SW_UART=0x16,
} OTA_CMD_TYPE;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef uint32_t size_t;
typedef uint32_t uintptr_t;

typedef struct wei_memory_dsp_item_s {
	unsigned int pat_type;
	unsigned int pat_att;
	unsigned int pat_addr;
	unsigned int pat_size;
} wei_memory_dsp_item_t, *wei_memory_dsp_item_ptr;

#define SB_HEADER_SECT_MAX 32

typedef struct wei_img_header_s{
	uint8_t type;			// indicate the secure image type (refer to SB_HEADER_TYPE_S)
	uint8_t version;		// Version of the SB header
	uint16_t crc;			// CRC value (CRC is calculated over the bytes from Size to ADDR_NEXT_SBH)

	uint8_t pid;			// Product ID (should be 35 for PA8535)
	uint8_t num_sbs;		// section count of secure boot image
	uint16_t size;			// the size of secure boot header (except Type/Version/CRC)

	uint16_t reserved0;
	uint16_t reserved1;

	uint32_t next_img_header;
} wei_img_header_t;

typedef struct memory_dsp_s{
	wei_img_header_t info;
	wei_memory_dsp_item_t item[SB_HEADER_SECT_MAX];
} memory_dsp_t;

int himax_ota_send_command(unsigned char feature, unsigned char type,unsigned int data_len, unsigned char* data);
int himax_api_send_op_command(unsigned char feature, OTA_CMD_TYPE cmd);
void himax_api_send_upg_command(unsigned char feature, OTA_CMD_TYPE cmd, wei_memory_dsp_item_ptr dsp_data);
void himax_api_send_upg_file(unsigned char feature, OTA_CMD_TYPE cmd, wei_memory_dsp_item_ptr dsp_data, unsigned char *image_data);
int himax_api_read_data(unsigned char type);
int himax_api_print_result(unsigned int result);
void himax_api_waitting_time(wei_memory_dsp_item_ptr dsp_data);
unsigned int himax_api_parse_memdsp(unsigned char* num_sbs, wei_memory_dsp_item_ptr memory_dsp_item_ptr, unsigned int *index);

#endif//__HIMAX_API_H__

