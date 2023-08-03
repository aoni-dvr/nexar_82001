/**
 *  @file MTPDeviceProp.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details USB MTP device class functions for Device Properties.
 */

#ifndef COMSVC_MTP_DPROP_H      /* Dir 4.10 */
#define COMSVC_MTP_DPROP_H

static UINT16 *GetSupportedDevicePropList(void)
{
    static UINT16 DeviceSupportProp[] = {
        MTP_DEV_PROP_BATTERY_LEVEL,
        MTP_DEV_PROP_DATE_TIME,
        MTP_DEV_PROP_USE_DEVICE_STAGE_FLAG,
        MTP_DEV_PROP_SYNCHRONIZATION_PARTNER,
        MTP_DEV_PROP_DEVICE_FRIENDLY_NAME,
        MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO,
        MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE,
        0
    };

    return DeviceSupportProp;
}

#define DEVICE_PROP_DATASET_LENGTH (1024)

static UINT8 *GetDevicePropBuffer(void)
{
    static UINT8 DevicePropBuffer[DEVICE_PROP_DATASET_LENGTH] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    return DevicePropBuffer;
}

static void Ascii2Unicode(const char *pAsciiStr, WCHAR *pUnicodeStr)
{
    SIZE_t      len, i;
    const char  *pSrc = pAsciiStr;
    WCHAR       *pDst = pUnicodeStr;

    if ((pSrc != NULL) && (pDst != NULL)) {
        len = AmbaUtility_StringLength(pSrc);
        for (i = 0; i < len; i++) {
            pDst[i] = (WCHAR)((UINT8)pSrc[i]);
        }
    }
}

static void MtpInitDeviceProperties(APP_MTPD_DEVICE_PROPS_s *Props)
{
    const char sync_partner[]  = "Ambarella";
    const char datetime[]      = "20180122T150105";
    const char friendly_name[] = "Ambarella MTP Device";
    const char session_init[]  = "Unknown";

    Ascii2Unicode(sync_partner, Props->SyncPartnerDefault);
    Ascii2Unicode(sync_partner, Props->SyncPartnerCurrent);

    Ascii2Unicode(datetime, Props->DateTimeDefault);
    Ascii2Unicode(datetime, Props->DateTimeCurrent);

    Ascii2Unicode(friendly_name, Props->DeviceFriendlyNameDefault);
    Ascii2Unicode(friendly_name, Props->DeviceFriendlyNameCurrent);

    Ascii2Unicode(session_init, Props->SessionInitVerInfoDefault);
    Ascii2Unicode(session_init, Props->SessionInitVerInfoCurrent);
    return;
}

static APP_MTPD_DEVICE_PROPS_s *GetDeviceProps(void)
{
    return &MtpClassInfo.DeviceProps;
}

static UINT32 Fill_String_Current_Value(UINT8 *desc, const WCHAR *current_value)
{
    UINT32 uRet = 0;

    if (desc != NULL) {
        uRet = MTP_Utility_Add_String(desc, current_value);
    }
    return uRet;
}


static ULONG Fill_String_Desc(UINT8 *desc, ULONG code, UINT8 access, const WCHAR *default_value, const WCHAR *current_value)
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
        pos += MTP_Utility_Add_String(&desc[pos], default_value);

        // current value
        pos += MTP_Utility_Add_String(&desc[pos], current_value);

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
static UINT8 DevicePropPrecivedDeviceTypeDataset[] = {
    0x07, 0xD4,                                             /* Prop code */
    0x06, 0x00,                                             /* Data Type  */
    0x00,                                                   /* GET/SET   */
    PERCEIVED_DEVICE_TYPE_STILL_IMAGE, 0x00, 0x00, 0x00,    /* Default value. */
    PERCEIVED_DEVICE_TYPE_STILL_IMAGE, 0x00, 0x00, 0x00,    /* Current value. */
    0x00                                                    /* Form Flag : None.  */
};

#define DEVICE_PROP_USE_DEVICE_STAGE_FLAG_DATASET_LENGTH 8
static UINT8 DevicePropUseDeviceStageFlagDataset[] = {
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
    UINT32  i, len = (UINT32)WStrLen(str);

    AmbaPrint_PrintStr5("%s", title, 0U, 0U, 0U, 0U);
    for (i = 0U; i < len; i++) {
        AmbaPrint_PrintUInt5(" [%d]", i, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintStr5("      0x%X(%c)", str[i], str[i], 0U, 0U, 0U);
    }
}
#endif

/* Operation code : GetDevicePropDesc 0x1014 */
static UINT32 CbDevicePropDescGet(ULONG DeviceProperty, UCHAR **DevicePropDataset, ULONG *DevicePropDtasetLength)
{
    UINT32 status;
    const APP_MTPD_DEVICE_PROPS_s *props = GetDeviceProps();
    UINT8 *DataSet_String          = GetDevicePropBuffer();

    //dbg("%s(): Property Code: 0x%X", __func__, DeviceProperty);

    switch (DeviceProperty) {
    case MTP_DEV_PROP_DATE_TIME:
        *DevicePropDataset      = DataSet_String;
        *DevicePropDtasetLength = Fill_String_Desc(DataSet_String, DeviceProperty, 0x01, props->DateTimeDefault, props->DateTimeCurrent);
#ifdef DEBUG
        DumpUnicodeString("DateTime Default:", props->DateTimeDefault);
        DumpUnicodeString("DateTime Current:", props->DateTimeCurrent);
#endif
        status = OK;
        break;

    case MTP_DEV_PROP_USE_DEVICE_STAGE_FLAG:
        *DevicePropDataset      = DevicePropUseDeviceStageFlagDataset;
        *DevicePropDtasetLength = DEVICE_PROP_USE_DEVICE_STAGE_FLAG_DATASET_LENGTH;
        status                  = OK;
        break;

    case MTP_DEV_PROP_SYNCHRONIZATION_PARTNER:
        *DevicePropDataset      = DataSet_String;
        *DevicePropDtasetLength = Fill_String_Desc(DataSet_String, DeviceProperty, 0x01, props->SyncPartnerDefault, props->SyncPartnerCurrent);
        status                  = OK;
        break;

    case MTP_DEV_PROP_DEVICE_FRIENDLY_NAME:
        *DevicePropDataset      = DataSet_String;
        *DevicePropDtasetLength = Fill_String_Desc(DataSet_String, DeviceProperty, 0x01, props->DeviceFriendlyNameDefault, props->DeviceFriendlyNameCurrent);
        status                  = OK;
        break;

    case MTP_DEV_PROP_BATTERY_LEVEL:
        *DevicePropDataset      = DevicePropBattteryLevelDataset;
        *DevicePropDtasetLength = DEVICE_PROP_BATTERY_LEVEL_DATASET_LENGTH;
        status                  = OK;
        break;

    case MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE:
        *DevicePropDataset      = DevicePropPrecivedDeviceTypeDataset;
        *DevicePropDtasetLength = DEVICE_PROP_PERCEIVED_DEVICE_TYPE_DATASET_LENGTH;
        status                  = OK;
        break;

    case MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO:
        *DevicePropDataset      = DataSet_String;
        *DevicePropDtasetLength = Fill_String_Desc(DataSet_String, DeviceProperty, 0x01, props->SessionInitVerInfoDefault, props->SessionInitVerInfoCurrent);
        status                  = OK;
        break;

    default:
        status = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;
        break;
    }

    //dbg("%s(): Length = %d", __func__, *DevicePropDtasetLength);

    return status;
}

/* Operation code : GetDevicePropValue 0x1015 */
static UINT32 CbDevicePropValueGet(ULONG DeviceProperty, UCHAR **DevicePorpValue, ULONG *DevicePropValueLength)
{
    UINT32 status;
    const APP_MTPD_DEVICE_PROPS_s *props = GetDeviceProps();
    UINT8 *DataSet_String          = GetDevicePropBuffer();

    //dbg("%s(): Property Code: 0x%X", __func__, DeviceProperty);

    status = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;

    switch (DeviceProperty) {
    case MTP_DEV_PROP_DATE_TIME:
        *DevicePorpValue       = DataSet_String;
        *DevicePropValueLength = Fill_String_Current_Value(DataSet_String, props->DateTimeCurrent);
        status                 = OK;
        break;
    case MTP_DEV_PROP_USE_DEVICE_STAGE_FLAG:
        *DevicePorpValue       = &DevicePropUseDeviceStageFlagDataset[MTP_DATASET_VALUE_OFFSET];
        *DevicePropValueLength = 1;
        status                 = OK;
        break;

    case MTP_DEV_PROP_SYNCHRONIZATION_PARTNER:
        *DevicePorpValue       = DataSet_String;
        *DevicePropValueLength = Fill_String_Current_Value(DataSet_String, props->SyncPartnerCurrent);
        status                 = OK;
        break;

    case MTP_DEV_PROP_DEVICE_FRIENDLY_NAME:
        *DevicePorpValue       = DataSet_String;
        *DevicePropValueLength = Fill_String_Current_Value(DataSet_String, props->DeviceFriendlyNameCurrent);
        status                 = OK;
        break;

    case MTP_DEV_PROP_BATTERY_LEVEL:
        *DevicePorpValue       = &DevicePropBattteryLevelDataset[MTP_DATASET_VALUE_OFFSET];
        *DevicePropValueLength = 1;
        status                 = OK;
        break;

    case MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE:
        *DevicePorpValue       = &DevicePropPrecivedDeviceTypeDataset[9];
        *DevicePropValueLength = 4;
        status                 = OK;
        break;

    case MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO:
        *DevicePorpValue       = DataSet_String;
        *DevicePropValueLength = Fill_String_Current_Value(DataSet_String, props->SessionInitVerInfoCurrent);
        status                 = OK;
        break;

    default:
        status = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;
        break;
    }

    //dbg("%s(): Length = %d", __func__, *DevicePropValueLength);

    return (status);
}

/* Operation code : SetDeivcePropValue 0x1016 */
static UINT32 CbDevicePropValueSet(ULONG DeviceProperty, const UCHAR *DevicePropValue, ULONG DevicePropValueLength)
{
    UINT32 status                  = OK;
    APP_MTPD_DEVICE_PROPS_s *props = GetDeviceProps();

    //dbg("%s(): Property Code: 0x%X, Length %d", __func__, DeviceProperty, DevicePropValueLength);

    switch (DeviceProperty) {
    case MTP_DEV_PROP_DATE_TIME:
        if (DevicePropValueLength <= 255U) {
            AmbaSvcWrap_MisraMemcpy(props->DateTimeCurrent, &DevicePropValue[1], DevicePropValueLength - 1U);
        } else {
            AmbaPrint_PrintUInt5("CbDevicePropValueSet(): Value Length > 255 for DateTime.", 0U, 0U, 0U, 0U, 0U);
            status = MTP_RC_INVALID_DEVICE_PROP_VALUE;
        }
        break;
    case MTP_DEV_PROP_SYNCHRONIZATION_PARTNER:
        if (DevicePropValueLength <= 255U) {
            AmbaSvcWrap_MisraMemcpy(props->SyncPartnerCurrent, &DevicePropValue[1], DevicePropValueLength - 1U);
        } else {
            AmbaPrint_PrintUInt5("CbDevicePropValueSet(): Value Length > 255 for SyncPartner.", 0U, 0U, 0U, 0U, 0U);
            status = MTP_RC_INVALID_DEVICE_PROP_VALUE;
        }
        break;
    case MTP_DEV_PROP_DEVICE_FRIENDLY_NAME:
        if (DevicePropValueLength <= 255U) {
            AmbaSvcWrap_MisraMemcpy(props->DeviceFriendlyNameCurrent, &DevicePropValue[1], DevicePropValueLength - 1U);
        } else {
            AmbaPrint_PrintUInt5("CbDevicePropValueSet(): Value Length > 255 for DeviceFriendlyName.", 0U, 0U, 0U, 0U, 0U);
            status = MTP_RC_INVALID_DEVICE_PROP_VALUE;
        }
        break;
    case MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO:
        if (DevicePropValueLength <= 255U) {
            AmbaSvcWrap_MisraMemcpy(props->SessionInitVerInfoCurrent, &DevicePropValue[1], DevicePropValueLength - 1U);
        } else {
            AmbaPrint_PrintUInt5("CbDevicePropValueSet(): Value Length > 255 for Session Initiator Version Info.", 0U, 0U, 0U, 0U, 0U);
            status = MTP_RC_INVALID_DEVICE_PROP_VALUE;
        }
        break;
    case MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE:
        if (DevicePropValueLength != 4U) {
            AmbaPrint_PrintUInt5("CbDevicePropValueSet(): Value Length != 4 for Perceived Device Type.", 0U, 0U, 0U, 0U, 0U);
            status = MTP_RC_INVALID_DEVICE_PROP_VALUE;
        } else {
            AmbaSvcWrap_MisraMemcpy(&DevicePropPrecivedDeviceTypeDataset[9], DevicePropValue, (SIZE_t)4U);
        }
        break;
    default:
        status = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;
        break;
    }
    return (status);
}

/* Operation code : ResetDeivcePropValue 0x1017 */
static UINT32 CbDevicePropValueReset(ULONG DeviceProperty)
{
    UINT32 status                  = OK;
    APP_MTPD_DEVICE_PROPS_s *props = GetDeviceProps();

    //dbg("%s(): Property Code: 0x%X", __func__, DeviceProperty);

    switch (DeviceProperty) {
    case MTP_DEV_PROP_DATE_TIME:
        AmbaSvcWrap_MisraMemcpy(props->DateTimeCurrent, props->DateTimeDefault, DEVICE_PROP_DATASET_STRING_LENGTH);
        break;
    case MTP_DEV_PROP_SYNCHRONIZATION_PARTNER:
        AmbaSvcWrap_MisraMemcpy(props->SyncPartnerCurrent, props->SyncPartnerDefault, DEVICE_PROP_DATASET_STRING_LENGTH);
        break;
    case MTP_DEV_PROP_DEVICE_FRIENDLY_NAME:
        AmbaSvcWrap_MisraMemcpy(props->DeviceFriendlyNameCurrent, props->DeviceFriendlyNameDefault, DEVICE_PROP_DATASET_STRING_LENGTH);
        break;
    case MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO:
        AmbaSvcWrap_MisraMemcpy(props->SessionInitVerInfoCurrent, props->SessionInitVerInfoDefault, DEVICE_PROP_DATASET_STRING_LENGTH);
        break;
    case MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE:
        DevicePropPrecivedDeviceTypeDataset[9]  = DevicePropPrecivedDeviceTypeDataset[5];
        DevicePropPrecivedDeviceTypeDataset[10] = DevicePropPrecivedDeviceTypeDataset[6];
        DevicePropPrecivedDeviceTypeDataset[11] = DevicePropPrecivedDeviceTypeDataset[7];
        DevicePropPrecivedDeviceTypeDataset[12] = DevicePropPrecivedDeviceTypeDataset[8];
        break;
    case MTP_DEVICE_PROP_ALL_OBJECTS:
        // According to MTP spec, if the first parameter contains a value of 0xFFFFFFFF, all settable device properties,
        // except for the DateTime (0x5011), shall be reset to their default value.
        AmbaSvcWrap_MisraMemcpy(props->SyncPartnerCurrent, props->SyncPartnerDefault, DEVICE_PROP_DATASET_STRING_LENGTH);
        AmbaSvcWrap_MisraMemcpy(props->DeviceFriendlyNameCurrent, props->DeviceFriendlyNameDefault, DEVICE_PROP_DATASET_STRING_LENGTH);
        AmbaSvcWrap_MisraMemcpy(props->SessionInitVerInfoCurrent, props->SessionInitVerInfoDefault, DEVICE_PROP_DATASET_STRING_LENGTH);
        DevicePropPrecivedDeviceTypeDataset[9]  = DevicePropPrecivedDeviceTypeDataset[5];
        DevicePropPrecivedDeviceTypeDataset[10] = DevicePropPrecivedDeviceTypeDataset[6];
        DevicePropPrecivedDeviceTypeDataset[11] = DevicePropPrecivedDeviceTypeDataset[7];
        DevicePropPrecivedDeviceTypeDataset[12] = DevicePropPrecivedDeviceTypeDataset[8];
        break;
    default:
        status = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;
        break;
    }
    return (status);
}

#endif /* COMSVC_MTP_DPROP_H */
