/**
 *  @file AppUSB_MTPDeviceProp.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details USB MTP device class functions for Device Properties.
 */


#include "AppUSB.h"

#define DEVICE_PROP_DATASET_LENGTH (1024)

static UINT8 *device_prop_buffer_get(void)
{
    static UINT8 DevicePropBuffer[DEVICE_PROP_DATASET_LENGTH];

    return DevicePropBuffer;
}

static APP_MTPD_DEVICE_PROPS_s *device_props_get(void)
{
    return &g_mtp_class_info.DeviceProps;
}

static UINT32 string_append_uni(UINT8 *Desc, const WCHAR *CurrentValue)
{
    UINT32 uret = 0;

    if (Desc != NULL) {
        uret = AppMtpd_StringAppendUni(Desc, CurrentValue);
    }
    return uret;
}

static ULONG string_desc_fill(UINT8 *desc, ULONG code, UINT8 access, const WCHAR *default_value, const WCHAR *current_value)
{
    ULONG pos = 0;

    if (desc != NULL) {
        desc[pos] = (UINT8)(code & 0x0FFU); // Property Code
        pos++;
        desc[pos] = (UINT8)((code >> 8U) & 0x0FFU);
        pos++;
        desc[pos] = 0xFF;                   // Data Type, 0xFFFF --> String
        pos++;
        desc[pos] = 0xFF;
        pos++;
        desc[pos] = access;                 // Get/Set: Get
        pos++;

        // default value
        pos += AppMtpd_StringAppendUni(&desc[pos], default_value);

        // current value
        pos += AppMtpd_StringAppendUni(&desc[pos], current_value);

        desc[pos] = 0; // formflag, None
        pos++;
    }
    return pos;
}

#define DEVICE_PROP_BATTERY_LEVEL_DATASET_LENGTH 11
static UINT8 DevicePropBattteryLevelDataset[] = {
    0x01, 0x50,                                             /* Prop code */
    0x02, 0x00,                                             /* Data Type  */
    0x00,                                                   /* GET/SET   */
    0x64,                                                   /* Default value. */
    0x64,                                                   /* Current value. */
    0x01,                                                   /* Form Flag : Range.  */
    0x00,
    0x64,
    0x01
};


#define DEVICE_PROP_PERCEIVED_DEVICE_TYPE_DATASET_LENGTH 14
//#define PERCEIVED_DEVICE_TYPE_GENERIC         0x00
#define PERCEIVED_DEVICE_TYPE_STILL_IMAGE                0x01
//#define PERCEIVED_DEVICE_TYPE_MEDIA_PLAYER    0x02
//#define PERCEIVED_DEVICE_TYPE_MOBILE_HANDSET  0x03
//#define PERCEIVED_DEVICE_TYPE_VIDEO_PLAYER    0x04
//#define PERCEIVED_DEVICE_TYPE_PDA             0x05
//#define PERCEIVED_DEVICE_TYPE_AUDIO_RECORDER  0x06
static UINT8 dataset_precived_device_type[] = {
    0x07, 0xD4,                                             /* Prop code */
    0x06, 0x00,                                             /* Data Type  */
    0x00,                                                   /* GET/SET   */
    PERCEIVED_DEVICE_TYPE_STILL_IMAGE, 0x00, 0x00, 0x00,    /* Default value. */
    PERCEIVED_DEVICE_TYPE_STILL_IMAGE, 0x00, 0x00, 0x00,    /* Current value. */
    0x00                                                    /* Form Flag : None.  */
};

#define DEVICE_PROP_USE_DEVICE_STAGE_FLAG_DATASET_LENGTH 8
static UINT8 dataset_use_device_stage_flag[] = {
    0x03, 0xD3,                                             /* Prop code */
    0x02, 0x00,                                             /* Data Type  */
    0x00,                                                   /* GET/SET   */
    0x01,                                                   /* Default value. */
    0x01,                                                   /* Current value. */
    0x00,                                                   /* Form Flag : Range.  */
};

#ifdef DEBUG
void DumpUnicodeString(const char *title, const WCHAR *str)
{
    int len = AppUsb_WStrLen(str);
    int i   = 0;

    AppUsb_PrintStr5("%s", title, 0, 0, 0, 0);
    for (i = 0; i < len; i++) {
        AppUsb_PrintUInt5(" [%d]", i, 0, 0, 0, 0);
        AppUsb_PrintStr5("      0x%X(%c)", str[i], str[i], 0, 0, 0);
    }
}
#endif

/** \addtogroup APPLICATION_APIS
 *  @{
 * */
/**
 * Get supported device property list
*/
UINT16 *AppMtpd_SupportedDevPropListGet(void)
{
    static UINT16 supported_device_prop[] = {
        MTP_DEV_PROP_BATTERY_LEVEL,
        MTP_DEV_PROP_DATE_TIME,
        MTP_DEV_PROP_USE_DEVICE_STAGE_FLAG,
        MTP_DEV_PROP_SYNCHRONIZATION_PARTNER,
        MTP_DEV_PROP_DEVICE_FRIENDLY_NAME,
        MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO,
        MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE,
        0
    };

    return supported_device_prop;
}
/**
 * Initialize Device Properties.
*/
void AppMtpd_DevicePropertiesInit(APP_MTPD_DEVICE_PROPS_s *Props)
{
    const char sync_partner[]  = "Ambarella";
    const char datetime[]      = "20180122T150105";
    const char friendly_name[] = "Ambarella MTP Device";
    const char session_init[]  = "Unknown";

    AppUsb_Ascii2Unicode(sync_partner, Props->SyncPartnerDefault);
    AppUsb_Ascii2Unicode(sync_partner, Props->SyncPartnerCurrent);

    AppUsb_Ascii2Unicode(datetime, Props->DateTimeDefault);
    AppUsb_Ascii2Unicode(datetime, Props->DateTimeCurrent);

    AppUsb_Ascii2Unicode(friendly_name, Props->DeviceFriendlyNameDefault);
    AppUsb_Ascii2Unicode(friendly_name, Props->DeviceFriendlyNameCurrent);

    AppUsb_Ascii2Unicode(session_init, Props->SessionInitVerInfoDefault);
    AppUsb_Ascii2Unicode(session_init, Props->SessionInitVerInfoCurrent);

    return;
}
/**
 * An utility function for filling unicode string to MTP memory.
 * @param pBuffer The output PTP memory
 * @param UniString The Unicode String
 * @return The size of data in PTP memory
 * */
UINT32 AppMtpd_StringAppendUni(UINT8 *pBuffer, const WCHAR *UniString)
{
    UINT32 uni_str_len    ;
    UINT32 pos         = 0;

    uni_str_len = AppUsb_WStrLen(UniString);

    if (uni_str_len == 0U) {
        pBuffer[pos] = 0x0;
        pos++;
    } else {
        pBuffer[pos] = (UINT8)((uni_str_len + 1U) & 0x0FFU);
        pos++;
        AppUsb_MemoryCopy(&pBuffer[pos], UniString, uni_str_len * 2U);
        pos += uni_str_len * 2U;
        // put NULL terminated char.
        pBuffer[pos] = 0;
        pos++;
        pBuffer[pos] = 0;
        pos++;
    }

    return pos;
}

/**
 *  Callback function for handling MTP request: Get Device Property Desc.
 * */
UINT32 AppMtpd_CbDevicePropDescGet(ULONG DeviceProperty, UCHAR **DevicePropDataset, ULONG *DevicePropDtasetLength)
{
    UINT32 status;
    const APP_MTPD_DEVICE_PROPS_s *props = device_props_get();
    UINT8 *DataSet_String          = device_prop_buffer_get();

    //dbg("%s(): Property Code: 0x%X", __func__, DeviceProperty);

    switch (DeviceProperty) {
    case MTP_DEV_PROP_DATE_TIME:
        *DevicePropDataset      = DataSet_String;
        *DevicePropDtasetLength = string_desc_fill(DataSet_String, DeviceProperty, 0x01, props->DateTimeDefault, props->DateTimeCurrent);
#ifdef DEBUG
        DumpUnicodeString("DateTime Default:", props->DateTimeDefault);
        DumpUnicodeString("DateTime Current:", props->DateTimeCurrent);
#endif
        status = OK;
        break;

    case MTP_DEV_PROP_USE_DEVICE_STAGE_FLAG:
        *DevicePropDataset      = dataset_use_device_stage_flag;
        *DevicePropDtasetLength = DEVICE_PROP_USE_DEVICE_STAGE_FLAG_DATASET_LENGTH;
        status                  = OK;
        break;

    case MTP_DEV_PROP_SYNCHRONIZATION_PARTNER:
        *DevicePropDataset      = DataSet_String;
        *DevicePropDtasetLength = string_desc_fill(DataSet_String, DeviceProperty, 0x01, props->SyncPartnerDefault, props->SyncPartnerCurrent);
        status                  = OK;
        break;

    case MTP_DEV_PROP_DEVICE_FRIENDLY_NAME:
        *DevicePropDataset      = DataSet_String;
        *DevicePropDtasetLength = string_desc_fill(DataSet_String, DeviceProperty, 0x01, props->DeviceFriendlyNameDefault, props->DeviceFriendlyNameCurrent);
        status                  = OK;
        break;

    case MTP_DEV_PROP_BATTERY_LEVEL:
        *DevicePropDataset      = DevicePropBattteryLevelDataset;
        *DevicePropDtasetLength = DEVICE_PROP_BATTERY_LEVEL_DATASET_LENGTH;
        status                  = OK;
        break;

    case MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE:
        *DevicePropDataset      = dataset_precived_device_type;
        *DevicePropDtasetLength = DEVICE_PROP_PERCEIVED_DEVICE_TYPE_DATASET_LENGTH;
        status                  = OK;
        break;

    case MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO:
        *DevicePropDataset      = DataSet_String;
        *DevicePropDtasetLength = string_desc_fill(DataSet_String, DeviceProperty, 0x01, props->SessionInitVerInfoDefault, props->SessionInitVerInfoCurrent);
        status                  = OK;
        break;

    default:
        status = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;
        break;
    }

    //dbg("%s(): Length = %d", __func__, *DevicePropDtasetLength);

    return status;
}
/**
 *  Callback function for handling MTP request: Get Device Property Value.
 * */
UINT32 AppMtpd_CbDevicePropValueGet(ULONG DeviceProperty, UCHAR **DevicePorpValue, ULONG *DevicePropValueLength)
{
    UINT32 status;
    const APP_MTPD_DEVICE_PROPS_s *props = device_props_get();
    UINT8 *DataSet_String          = device_prop_buffer_get();

    //dbg("%s(): Property Code: 0x%X", __func__, DeviceProperty);

    status = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;

    switch (DeviceProperty) {
    case MTP_DEV_PROP_DATE_TIME:
        *DevicePorpValue       = DataSet_String;
        *DevicePropValueLength = string_append_uni(DataSet_String, props->DateTimeCurrent);
        status                 = OK;
        break;
    case MTP_DEV_PROP_USE_DEVICE_STAGE_FLAG:
        *DevicePorpValue       = &dataset_use_device_stage_flag[MTP_DATASET_VALUE_OFFSET];
        *DevicePropValueLength = 1;
        status                 = OK;
        break;

    case MTP_DEV_PROP_SYNCHRONIZATION_PARTNER:
        *DevicePorpValue       = DataSet_String;
        *DevicePropValueLength = string_append_uni(DataSet_String, props->SyncPartnerCurrent);
        status                 = OK;
        break;

    case MTP_DEV_PROP_DEVICE_FRIENDLY_NAME:
        *DevicePorpValue       = DataSet_String;
        *DevicePropValueLength = string_append_uni(DataSet_String, props->DeviceFriendlyNameCurrent);
        status                 = OK;
        break;

    case MTP_DEV_PROP_BATTERY_LEVEL:
        *DevicePorpValue       = &DevicePropBattteryLevelDataset[MTP_DATASET_VALUE_OFFSET];
        *DevicePropValueLength = 1;
        status                 = OK;
        break;

    case MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE:
        *DevicePorpValue       = &dataset_precived_device_type[9];
        *DevicePropValueLength = 4;
        status                 = OK;
        break;

    case MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO:
        *DevicePorpValue       = DataSet_String;
        *DevicePropValueLength = string_append_uni(DataSet_String, props->SessionInitVerInfoCurrent);
        status                 = OK;
        break;

    default:
        status = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;
        break;
    }

    //dbg("%s(): Length = %d", __func__, *DevicePropValueLength);

    return (status);
}

/**
 *  Callback function for handling MTP request: Set Device Property Value.
 * */
UINT32 AppMtpd_CbDevicePropValueSet(ULONG DeviceProperty, const UCHAR *DevicePropValue, ULONG DevicePropValueLength)
{
    UINT32 status                  = OK;
    APP_MTPD_DEVICE_PROPS_s *props = device_props_get();

    //dbg("%s(): Property Code: 0x%X, Length %d", __func__, DeviceProperty, DevicePropValueLength);

    switch (DeviceProperty) {
    case MTP_DEV_PROP_DATE_TIME:
        if (DevicePropValueLength <= 255U) {
            AppUsb_MemoryCopy(props->DateTimeCurrent, &DevicePropValue[1], DevicePropValueLength - 1U);
        } else {
            AppUsb_PrintUInt5("AppMtpd_CbDevicePropValueSet(): Value Length %d > 255 for DateTime.", DevicePropValueLength, 0, 0, 0, 0);
            status = MTP_RC_INVALID_DEVICE_PROP_VALUE;
        }
        break;
    case MTP_DEV_PROP_SYNCHRONIZATION_PARTNER:
        if (DevicePropValueLength <= 255U) {
            AppUsb_MemoryCopy(props->SyncPartnerCurrent, &DevicePropValue[1], DevicePropValueLength - 1U);
        } else {
            AppUsb_PrintUInt5("AppMtpd_CbDevicePropValueSet(): Value Length %d > 255 for SyncPartner.", DevicePropValueLength, 0, 0, 0, 0);
            status = MTP_RC_INVALID_DEVICE_PROP_VALUE;
        }
        break;
    case MTP_DEV_PROP_DEVICE_FRIENDLY_NAME:
        if (DevicePropValueLength <= 255U) {
            AppUsb_MemoryCopy(props->DeviceFriendlyNameCurrent, &DevicePropValue[1], DevicePropValueLength - 1U);
        } else {
            AppUsb_PrintUInt5("AppMtpd_CbDevicePropValueSet(): Value Length %d > 255 for DeviceFriendlyName.", DevicePropValueLength, 0, 0, 0, 0);
            status = MTP_RC_INVALID_DEVICE_PROP_VALUE;
        }
        break;
    case MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO:
        if (DevicePropValueLength <= 255U) {
            AppUsb_MemoryCopy(props->SessionInitVerInfoCurrent, &DevicePropValue[1], DevicePropValueLength - 1U);
        } else {
            AppUsb_PrintUInt5("AppMtpd_CbDevicePropValueSet(): Value Length %d > %d for Session Initiator Version Info.", DevicePropValueLength, 255, 0, 0, 0);
            status = MTP_RC_INVALID_DEVICE_PROP_VALUE;
        }
        break;
    case MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE:
        if (DevicePropValueLength != 4U) {
            AppUsb_PrintUInt5("AppMtpd_CbDevicePropValueSet(): Value Length %d != 4 for Perceived Device Type.", DevicePropValueLength, 0, 0, 0, 0);
            status = MTP_RC_INVALID_DEVICE_PROP_VALUE;
        } else {
            AppUsb_MemoryCopy(&dataset_precived_device_type[9], DevicePropValue, 4);
        }
        break;
    default:
        status = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;
        break;
    }
    return (status);
}

/**
 *  Callback function for handling MTP request: Reset Device Property Value.
 * */
UINT32 AppMtpd_CbDevicePropValueReset(ULONG DeviceProperty)
{
    UINT32 status                  = OK;
    APP_MTPD_DEVICE_PROPS_s *props = device_props_get();

    //dbg("%s(): Property Code: 0x%X", __func__, DeviceProperty);

    switch (DeviceProperty) {
    case MTP_DEV_PROP_DATE_TIME:
        AppUsb_MemoryCopy(props->DateTimeCurrent, props->DateTimeDefault, DEVICE_PROP_DATASET_STRING_LENGTH);
        break;
    case MTP_DEV_PROP_SYNCHRONIZATION_PARTNER:
        AppUsb_MemoryCopy(props->SyncPartnerCurrent, props->SyncPartnerDefault, DEVICE_PROP_DATASET_STRING_LENGTH);
        break;
    case MTP_DEV_PROP_DEVICE_FRIENDLY_NAME:
        AppUsb_MemoryCopy(props->DeviceFriendlyNameCurrent, props->DeviceFriendlyNameDefault, DEVICE_PROP_DATASET_STRING_LENGTH);
        break;
    case MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO:
        AppUsb_MemoryCopy(props->SessionInitVerInfoCurrent, props->SessionInitVerInfoDefault, DEVICE_PROP_DATASET_STRING_LENGTH);
        break;
    case MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE:
        //AppUsb_Memcpy(dataset_precived_device_type + 9, dataset_precived_device_type + 5, 4);
        dataset_precived_device_type[9]  = dataset_precived_device_type[5];
        dataset_precived_device_type[10] = dataset_precived_device_type[6];
        dataset_precived_device_type[11] = dataset_precived_device_type[7];
        dataset_precived_device_type[12] = dataset_precived_device_type[8];
        break;
    case MTP_DEVICE_PROP_ALL_OBJECTS:
        // According to MTP spec, if the first parameter contains a value of 0xFFFFFFFF, all settable device properties,
        // except for the DateTime (0x5011), shall be reset to their default value.
        AppUsb_MemoryCopy(props->SyncPartnerCurrent, props->SyncPartnerDefault, DEVICE_PROP_DATASET_STRING_LENGTH);
        AppUsb_MemoryCopy(props->DeviceFriendlyNameCurrent, props->DeviceFriendlyNameDefault, DEVICE_PROP_DATASET_STRING_LENGTH);
        AppUsb_MemoryCopy(props->SessionInitVerInfoCurrent, props->SessionInitVerInfoDefault, DEVICE_PROP_DATASET_STRING_LENGTH);
        //AppUsb_Memcpy(dataset_precived_device_type + 9, dataset_precived_device_type + 5, 4);
        dataset_precived_device_type[9]  = dataset_precived_device_type[5];
        dataset_precived_device_type[10] = dataset_precived_device_type[6];
        dataset_precived_device_type[11] = dataset_precived_device_type[7];
        dataset_precived_device_type[12] = dataset_precived_device_type[8];
        break;
    default:
        status = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;
        break;
    }
    return (status);
}
/** @} */
