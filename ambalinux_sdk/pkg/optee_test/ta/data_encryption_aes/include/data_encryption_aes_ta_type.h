

#ifndef MOUDLE_DATA_ENCRYPTION_AES_TA_TYPE_H_
#define MOUDLE_DATA_ENCRYPTION_AES_TA_TYPE_H_


//abb6f4b5-7e33-4ad2-9802-e64f2a7cc20c

#define DATA_ENCRYPTION_AES_UUID_ID {0xabb6f4b5, 0x7e33, 0x4ad2, \
    { \
        0x98, 0x02, 0xe6, 0x4f, 0x2a, 0x7c, 0xc2, 0x0c \
    } \
}

/* Define the comman ID */
#define CMD_AES                          4U

/* Define the debug flag */
#define DEBUG
#define DLOG    MSG_RAW
//#define DLOG    ta_debug

#define UNUSED(x) (void)(x)

/* AES operation type */
typedef enum
{
    E_AES_OP_INVALID = 0,
    E_AES_OP_ENCRYPT = 1,
    E_AES_OP_DECRYPT = 2,
} E_AES_OPERATION;

/* AES mode type */
typedef enum
{
    E_AES_MODE_INVALID = 0,
    E_AES_MODE_CBC = 1,
    E_AES_MODE_ECB = 2,
    E_AES_MODE_CTR = 3,
    E_AES_MODE_CTS = 4,
} E_AES_MODE;

typedef struct
{
    E_AES_OPERATION action;
    E_AES_MODE mode;
} aes_operation_info_t;


#endif

