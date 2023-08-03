#ifndef USER_TA_HEADER_DEFINES_H
#define USER_TA_HEADER_DEFINES_H

#include <secure_credential_provisioning_ta_type.h> /* To get the TA_MY_TEST_UUID define */

#define TA_UUID SECURE_CREDENTIAL_PROVISIONING_UUID

#define TA_FLAGS                    (TA_FLAG_MULTI_SESSION | TA_FLAG_EXEC_DDR)
#define TA_STACK_SIZE               (2 * 1024)
#define TA_DATA_SIZE                (32 * 1024)

#define TA_CURRENT_TA_EXT_PROPERTIES \
    { "gp.ta.description", USER_TA_PROP_TYPE_STRING, \
        "secure credential provisioning TA" }, \
    { "gp.ta.version", USER_TA_PROP_TYPE_U32, &(const uint32_t){ 0x0010 } }

#endif /*USER_TA_HEADER_DEFINES_H*/
