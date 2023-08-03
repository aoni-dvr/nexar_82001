/**
 *  @file AppUSB_MTPObjectProp.c
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
 *  @details USB MTP device class functions for APP/MW. It handles MTP object properties.
 */

#include "AppUSB.h"
#include "AmbaUtility.h"

#define MTP_GROUP_CODE                        0x1U

#define APP_MTP_MAX_PROP_VALUE_BUFFER_SIZE    ((255U*2U) + 1U)
#define APP_MTP_MAX_PROP_LIST_BUFFER_SIZE     (16U*1024U)
#define APP_MTP_MAX_PROP_DESC_BUFFER_SIZE     (16U*1024U)

#define PROPERTY_DESC_SIZE_INT16_NO_FORM_FLAG 12
#define PROPERTY_DESC_SIZE_INT32_NO_FORM_FLAG 18
//#define PROPERTY_DESC_SIZE_INT128_NO_FORM_FLAG 26

static UINT8 *prop_value_buffer_get(UINT32 Size)
{
    static UINT8 prop_value_buffer[APP_MTP_MAX_PROP_VALUE_BUFFER_SIZE];
    UINT8 *pret = prop_value_buffer;

    if (Size > APP_MTP_MAX_PROP_VALUE_BUFFER_SIZE) {
        pret = NULL;
    }
    return pret;
}

static UINT8 *prop_list_buffer_get(UINT32 Size)
{
    static UINT8 prop_list_buffer[APP_MTP_MAX_PROP_LIST_BUFFER_SIZE];
    UINT8 *pret = prop_list_buffer;

    if (Size > APP_MTP_MAX_PROP_LIST_BUFFER_SIZE) {
        pret = NULL;
    }
    return pret;
}

static UINT8 *prop_desc_buffer_get(UINT32 Size)
{
    static UINT8 prop_desc_buffer[APP_MTP_MAX_PROP_DESC_BUFFER_SIZE];
    UINT8 *pret = prop_desc_buffer;

    if (Size > APP_MTP_MAX_PROP_DESC_BUFFER_SIZE) {
        pret = NULL;
    }
    return pret;
}

static UINT32 prop_desc_short_no_form_fill(UINT8 **Buf, ULONG *Length, ULONG PropertyCode, UINT32 Access, UINT32 DefaultValue, UINT32 GroupCode)
{
    UINT32 status = OK;
    UINT8 *ptr    = NULL;

    *Length = PROPERTY_DESC_SIZE_INT16_NO_FORM_FLAG;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_short_no_form_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        // Property code.
        (void)AppMtpd_ShortPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_CODE], PropertyCode);

        // Data type is UINT16.
        (void)AppMtpd_ShortPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_DATATYPE], MTP_TYPES_UINT16);

        // GetSet value
        ptr[MTP_OBJECT_PROPERTY_DATASET_GETSET] = (UINT8)Access;

        // Default value is UINT16.  */
        (void)AppMtpd_ShortPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_VALUE], DefaultValue);

        // Group code
        (void)AppMtpd_LongPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_VALUE + 2U], GroupCode);

        // Form Flag is not used.
        ptr[MTP_OBJECT_PROPERTY_DATASET_VALUE + 6U] = 0;
    }

    return status;
}

static UINT32 prop_desc_long_no_form_fill(UINT8 **Buf, ULONG *Length, ULONG PropertyCode, UINT32 Access, UINT32 DefaultValue, UINT32 GroupCode)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;

    *Length = PROPERTY_DESC_SIZE_INT32_NO_FORM_FLAG;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_long_no_form_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        // Property code.
        (void)AppMtpd_ShortPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_CODE], PropertyCode);

        // Data type is UINT16.
        (void)AppMtpd_ShortPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_DATATYPE], MTP_TYPES_UINT32);

        // GetSet value
        ptr[MTP_OBJECT_PROPERTY_DATASET_GETSET] = (UINT8)Access;

        // Default value is UINT32.  */
        (void)AppMtpd_LongPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_VALUE], DefaultValue);

        // Group code
        (void)AppMtpd_LongPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_VALUE + 4U], GroupCode);

        // Form Flag is not used.
        ptr[MTP_OBJECT_PROPERTY_DATASET_VALUE + 8U] = 0;
    }
    return status;
}


static UINT32 prop_desc_protection_status_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 18;

    *Buf = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_protection_status_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(ptr, MTP_OBJECT_PROP_PROTECTION_STATUS);

        /* Data type is UINT16.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT16);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT16.  */
        pos += AppMtpd_ShortPut(&ptr[pos], 0);

        /* Group code is 4.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag ENUM.  */
        ptr[pos] = 2;
        pos++;

        /* Number of elements in Enum array.  */
        pos += AppMtpd_ShortPut(&ptr[pos], 2);

        /* Elements in Enum array.  Here we store only No protection and Read-Only protection values. This can be extended with
           Read-only data and Non transferrable data. Spec talks about MTP vendor extension range as well. Not used here.  */
        pos += AppMtpd_ShortPut(&ptr[pos], 0);
        pos += AppMtpd_ShortPut(&ptr[pos], 1);

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_protection_status_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }


    return status;
}

static UINT32 prop_desc_object_size_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    ULONG pos     = 0;

    *Length = 18;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_object_size_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(ptr, MTP_OBJECT_PROP_OBJECT_SIZE);

        /* Data type is UINT64.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT64);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT64.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0);
        pos += AppMtpd_LongPut(&ptr[pos], 0);

        /* Group code is NULL.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag is not used.  */
        ptr[pos] = 0;
        pos++;

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_object_size_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }
    return status;
}

static UINT32 prop_desc_parent_object_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 14;

    *Buf = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_parent_object_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(ptr, MTP_OBJECT_PROP_PARENT_OBJECT);

        /* Data type is UINT32.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT32.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0);

        /* Group code is NULL.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag is not used.  */
        ptr[pos] = 0;
        pos++;

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_parent_object_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }

    return status;
}
// fill Persistent Unique Object Identifier property descriptor
static UINT32 prop_desc_pst_uni_obj_id_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 26;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_pst_uni_obj_id_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(ptr, MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER);

        /* Data type is UINT128.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT128);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT128.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0);
        pos += AppMtpd_LongPut(&ptr[pos], 0);
        pos += AppMtpd_LongPut(&ptr[pos], 0);
        pos += AppMtpd_LongPut(&ptr[pos], 0);

        /* Group code is NULL.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag is not used.  */
        ptr[pos] = 0;
        pos++;

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_pst_uni_obj_id_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }
    return status;
}

static UINT32 prop_desc_non_consumable_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 15;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_non_consumable_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(ptr, MTP_OBJECT_PROP_NON_CONSUMABLE);

        /* Data type is UINT8.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT8);

        /* GetSet value is GET/SET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GETSET;
        pos++;

        /* Default value is UINT8.  */
        ptr[pos] = 0;
        pos++;

        /* Group code is 2.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag ENUM.  */
        ptr[pos] = 2;
        pos++;

        /* Number of elements in Enum array.  */
        pos += AppMtpd_ShortPut(&ptr[pos], 2);

        /* Elements in Enum array.  */
        ptr[pos] = 0;
        pos++;
        ptr[pos] = 1;
        pos++;

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_non_consumable_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }
    return status;
}

static UINT32 prop_desc_use_count_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 14;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_use_count_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(ptr, MTP_OBJECT_PROP_USE_COUNT);

        /* Data type is UINT8.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET/SET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GETSET;
        pos++;

        /* Default value is UINT32.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0);

        /* Group code is 1.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag is not used.  */
        ptr[pos] = 0;
        pos++;

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_use_count_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }
    return status;
}

static UINT32 prop_desc_sample_rate_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 26;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_sample_rate_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(ptr, MTP_OBJECT_PROP_SAMPLE_RATE);

        /* Data type is UINT32.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT32.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag RANGE.  */
        ptr[pos] = 1;
        pos++;

        /* Minimum range in array is 0KHZ.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x00000000);

        /* Maximum range in array is KHZ.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x0002EE00);

        /* Range step size is 32HZ.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x00000020);

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_sample_rate_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }
    return status;
}

static UINT32 prop_desc_num_of_channels_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 20;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_num_of_channels_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(ptr, MTP_OBJECT_PROP_NUMBER_OF_CHANNELS);

        /* Data type is UINT16.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT16);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT16.  */
        pos += AppMtpd_ShortPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag ENUM.  */
        ptr[pos] = 2;
        pos++;

        /* Number of elements in Enum array.  */
        pos += AppMtpd_ShortPut(&ptr[pos], 3);

        /* Elements in Enum array.  */
        pos += AppMtpd_ShortPut(&ptr[pos], 0);
        pos += AppMtpd_ShortPut(&ptr[pos], 1);
        pos += AppMtpd_ShortPut(&ptr[pos], 2);

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_num_of_channels_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }

    return status;
}

static UINT32 prop_desc_audio_wave_codec_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 28;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_audio_wave_codec_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_OBJECT_PROP_AUDIO_WAVE_CODEC);

        /* Data type is UINT32.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT32.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag ENUM.  */
        ptr[pos] = 2;
        pos++;

        /* Number of elements in Enum array.  */
        pos += AppMtpd_ShortPut(&ptr[pos], 3);

        /* Elements in Enum array.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_WAVE_FORMAT_MPEGLAYER3);
        pos += AppMtpd_LongPut(&ptr[pos], MTP_WAVE_FORMAT_MPEG);
        pos += AppMtpd_LongPut(&ptr[pos], MTP_WAVE_FORMAT_RAW_AAC1);

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_audio_wave_codec_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }
    return status;
}

static UINT32 prop_desc_audio_bitrate_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 26;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_audio_bitrate_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_OBJECT_PROP_AUDIO_BITRATE);

        /* Data type is UINT32.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT32.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x0000FA00);

        /* Group code is 0.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag RANGE.  */
        ptr[pos] = 1;
        pos++;

        /* Minimum range in array is 1 bit per second.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x00000001);

        /* Maximum range in array is 1,500,000 bit per second.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x0016E360);

        /* Range step size is 1 bit per second.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x00000001);

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_audio_bitrate_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }

    return status;
}

static UINT32 prop_desc_audio_duration_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 14;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_audio_duration_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_OBJECT_PROP_DURATION);

        /* Data type is UINT32.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT32.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0);

        /* Group code is NULL.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag is 1.  */
        ptr[pos] = 1;
        pos++;

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_audio_duration_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }

    return status;
}

static UINT32 prop_desc_width_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 26;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_width_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_OBJECT_PROP_WIDTH);

        /* Data type is UINT32.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT32.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag RANGE.  */
        ptr[pos] = 1;
        pos++;

        /* Minimum range in array is customer defined.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0);

        /* Maximum range in array is customer defined.  */
        pos += AppMtpd_LongPut(&ptr[pos], 10000);

        /* Range step size is customer defined.  */
        pos += AppMtpd_LongPut(&ptr[pos], 1);

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_width_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }

    return status;
}

static UINT32 prop_desc_height_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 26;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_height_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_OBJECT_PROP_HEIGHT);

        /* Data type is UINT32.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT16.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x0000);

        /* Group code is 0.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag RANGE.  */
        ptr[pos] = 1;
        pos++;

        /* Minimum range in array is customer defined.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x0000);

        /* Maximum range in array is customer defined.  */
        pos += AppMtpd_LongPut(&ptr[pos], 10000);

        /* Range step size is customer defined.  */
        pos += AppMtpd_LongPut(&ptr[pos], 1);

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_height_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }
    return status;
}

static UINT32 prop_desc_scan_type_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 29;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_scan_type_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_OBJECT_PROP_SCAN_TYPE);

        /* Data type is UINT16.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT16);

        /* GetSet value is GET/SET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GETSET;
        pos++;

        /* Default value is UINT16.  */
        pos += AppMtpd_ShortPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag ENUM.  */
        ptr[pos] = 2;
        pos++;

        /* Number of elements in Enum array.  */
        pos += AppMtpd_ShortPut(&ptr[pos], 8);

        /* Elements in Enum array.  */
        pos += AppMtpd_ShortPut(&ptr[pos], 0x0000);
        pos += AppMtpd_ShortPut(&ptr[pos], 0x0001);
        pos += AppMtpd_ShortPut(&ptr[pos], 0x0002);
        pos += AppMtpd_ShortPut(&ptr[pos], 0x0003);
        pos += AppMtpd_ShortPut(&ptr[pos], 0x0004);
        pos += AppMtpd_ShortPut(&ptr[pos], 0x0005);
        pos += AppMtpd_ShortPut(&ptr[pos], 0x0006);
        pos += AppMtpd_ShortPut(&ptr[pos], 0x0007);

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_scan_type_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }

    return status;
}

static UINT32 prop_desc_fourcc_codec_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 22;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_fourcc_codec_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_OBJECT_PROP_VIDEO_FOURCC_CODEC);

        /* Data type is UINT32.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET/SET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GETSET;
        pos++;

        /* Default value is UINT32.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag ENUM.  */
        ptr[pos] = 2;
        pos++;

        /* Number of elements in Enum array.  */
        pos += AppMtpd_LongPut(&ptr[pos], 1);

        /* Elements in Enum array.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x00000000);

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_fourcc_codec_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }
    return status;
}

static UINT32 prop_desc_video_bitrate_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 26;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_video_bitrate_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(ptr, MTP_OBJECT_PROP_VIDEO_BITRATE);

        /* Data type is UINT32.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT32.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag RANGE.  */
        ptr[pos] = 1;
        pos++;

        /* Minimum range in array is 0.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x00000000);

        /* Maximum range in array is 0xFFFFFFFF.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0xFFFFFFFFU);

        /* Range step size is 1.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x00000001);

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_video_bitrate_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }
    return status;
}

static UINT32 prop_desc_frame_per_1000s_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 22;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_frame_per_1000s_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(ptr, MTP_OBJECT_PROP_FRAMES_PER_THOUSAND_SECONDS);

        /* Data type is UINT32.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET/SET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GETSET;
        pos++;

        /* Default value is UINT32.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag ENUM.  */
        ptr[pos] = 2;
        pos++;

        /* Number of elements in Enum array.  */
        pos += AppMtpd_LongPut(&ptr[pos], 1);

        /* Elements in Enum array.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x00000000);

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_frame_per_1000s_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }

    return status;
}

static UINT32 prop_desc_key_frame_dist_fill(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 26;
    *Buf    = prop_desc_buffer_get(32);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_key_frame_dist_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(ptr, MTP_OBJECT_PROP_KEYFRAME_DISTANCE);

        /* Data type is UINT32.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET/SET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GETSET;
        pos++;

        /* Default value is UINT32.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += AppMtpd_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag RANGE.  */
        ptr[pos] = 1;
        pos++;

        /* Minimum range in array is 0.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x00000000);

        /* Maximum range in array is FFFFFFFF.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x0000FFFF);

        /* Range step size is 1.  */
        pos += AppMtpd_LongPut(&ptr[pos], 0x00000001);

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_key_frame_dist_fill(): pos(%d) != length (%d).", pos, *Length, 0, 0, 0);
        }
    }

    return status;
}

static UINT32 prop_desc_string_fill(UINT8 **Buf, ULONG *Length, ULONG Property, UINT32 Access, UINT32 Group, const WCHAR *str_uni)
{
    UINT32 status      = OK;
    UINT8 *ptr         = NULL;
    UINT32 pos         = 0;
    UINT32 uni_str_len = AppUsb_WStrLen(str_uni);

    if (uni_str_len == 0U) {
        *Length = (10U + 1U);                      // 1 byte for length
    } else {
        *Length = (10U + (uni_str_len * 2U) + 3U); // 1 byte for length and 2 bytes for NULL-terminated character.
    }

    *Buf = prop_desc_buffer_get(*Length);
    if (*Buf == NULL) {
        AppUsb_PrintUInt5("prop_desc_string_fill(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += AppMtpd_ShortPut(ptr, Property);

        /* Data type is STRING.  */
        pos += AppMtpd_ShortPut(&ptr[pos], MTP_TYPES_STR);

        /* GetSet value.  */
        ptr[pos] = (UINT8)Access;
        pos++;

        /* Store a Unicode string.   */
        pos += AppMtpd_StringAppendUni(&ptr[pos], str_uni);

        // Group code
        pos += AppMtpd_LongPut(&ptr[pos], Group);

        /* Form Flag is not used.  */
        ptr[pos] = 0;
        pos++;

        if (pos != *Length) {
            AppUsb_PrintUInt5("prop_desc_string_fill(): pos(%d) != length (%d) for Property 0x%X", pos, *Length, Property, 0, 0);
        }
    }
    return status;
}


static UINT32 property_desc_get_001(ULONG Property, UCHAR **Dataset, ULONG *DatasetLength)
{
    UINT32 uret;
    WCHAR  empty_wchar[] = { 0, 0 };

    /* Isolate the property. That will determine the dataset header.  */
    switch (Property) {
    case MTP_OBJECT_PROP_STORAGEID:

        uret = prop_desc_long_no_form_fill(
                   Dataset,
                   DatasetLength,
                   Property,
                   MTP_OBJ_PROP_VALUE_GET,
                   0,
                   MTP_GROUP_CODE);
        break;

    case MTP_OBJECT_PROP_OBJECT_FORMAT:

        uret = prop_desc_short_no_form_fill(
                   Dataset,
                   DatasetLength,
                   Property,
                   MTP_OBJ_PROP_VALUE_GET,
                   0,
                   MTP_GROUP_CODE);
        break;

    case MTP_OBJECT_PROP_PROTECTION_STATUS:
        uret = prop_desc_protection_status_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_OBJECT_SIZE:
        uret = prop_desc_object_size_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_OBJECT_FILE_NAME:
        uret = prop_desc_string_fill(
                   Dataset,
                   DatasetLength,
                   Property,
                   MTP_OBJ_PROP_VALUE_GETSET,
                   MTP_GROUP_CODE,
                   empty_wchar);
        break;

    case MTP_OBJECT_PROP_PARENT_OBJECT:
        uret = prop_desc_parent_object_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER:
        uret = prop_desc_pst_uni_obj_id_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_NAME:
        uret = prop_desc_string_fill(
                   Dataset,
                   DatasetLength,
                   Property,
                   MTP_OBJ_PROP_VALUE_GETSET,
                   MTP_GROUP_CODE,
                   empty_wchar);
        break;

    case MTP_OBJECT_PROP_NON_CONSUMABLE:
        uret = prop_desc_non_consumable_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_ARTIST:
        uret = prop_desc_string_fill(
                   Dataset,
                   DatasetLength,
                   Property,
                   MTP_OBJ_PROP_VALUE_GETSET,
                   MTP_GROUP_CODE,
                   empty_wchar);
        break;

    default:
        uret = MTP_RC_INVALID_OBJECT_PROP_CODE;
        break;
    }

    return uret;
}

static UINT32 property_desc_get_002(ULONG Property, UCHAR **Dataset, ULONG *DatasetLength)
{
    UINT32 uret;
    WCHAR  empty_wchar[] = { 0, 0 };

    /* Isolate the property. That will determine the dataset header.  */
    switch (Property) {
    case MTP_OBJECT_PROP_TRACK:

        uret = prop_desc_short_no_form_fill(
                   Dataset,
                   DatasetLength,
                   Property,
                   MTP_OBJ_PROP_VALUE_GET,
                   0,
                   MTP_GROUP_CODE);
        break;

    case MTP_OBJECT_PROP_USE_COUNT:
        uret = prop_desc_use_count_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_DATE_AUTHORED:
        uret = prop_desc_string_fill(
                   Dataset,
                   DatasetLength,
                   Property,
                   MTP_OBJ_PROP_VALUE_GETSET,
                   MTP_GROUP_CODE,
                   empty_wchar);
        break;

    case MTP_OBJECT_PROP_GENRE:
        uret = prop_desc_string_fill(
                   Dataset,
                   DatasetLength,
                   Property,
                   MTP_OBJ_PROP_VALUE_GETSET,
                   MTP_GROUP_CODE,
                   empty_wchar);
        break;

    case MTP_OBJECT_PROP_ALBUM_NAME:
        uret = prop_desc_string_fill(
                   Dataset,
                   DatasetLength,
                   Property,
                   MTP_OBJ_PROP_VALUE_GETSET,
                   MTP_GROUP_CODE,
                   empty_wchar);
        break;

    case MTP_OBJECT_PROP_ALBUM_ARTIST:
        uret = prop_desc_string_fill(
                   Dataset,
                   DatasetLength,
                   Property,
                   MTP_OBJ_PROP_VALUE_GETSET,
                   MTP_GROUP_CODE,
                   empty_wchar);
        break;

    case MTP_OBJECT_PROP_SAMPLE_RATE:
        uret = prop_desc_sample_rate_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_NUMBER_OF_CHANNELS:
        uret = prop_desc_num_of_channels_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_AUDIO_WAVE_CODEC:
        uret = prop_desc_audio_wave_codec_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_AUDIO_BITRATE:
        uret = prop_desc_audio_bitrate_fill(Dataset, DatasetLength);
        break;
    default:
        uret = MTP_RC_INVALID_OBJECT_PROP_CODE;
        break;
    }
    return uret;
}

static UINT32 property_desc_get_003(ULONG Property, UCHAR **Dataset, ULONG *DatasetLength)
{
    UINT32 uret;
    WCHAR  empty_wchar[] = { 0, 0 };

    /* Isolate the property. That will determine the dataset header.  */
    switch (Property) {
    case MTP_OBJECT_PROP_DURATION:
        uret = prop_desc_audio_duration_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_WIDTH:
        uret = prop_desc_width_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_HEIGHT:
        uret = prop_desc_height_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_SCAN_TYPE:
        uret = prop_desc_scan_type_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_VIDEO_FOURCC_CODEC:
        uret = prop_desc_fourcc_codec_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_VIDEO_BITRATE:
        uret = prop_desc_video_bitrate_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_FRAMES_PER_THOUSAND_SECONDS:
        uret = prop_desc_frame_per_1000s_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_KEYFRAME_DISTANCE:
        uret = prop_desc_key_frame_dist_fill(Dataset, DatasetLength);
        break;

    case MTP_OBJECT_PROP_ENCODING_PROFILE:
        uret = prop_desc_string_fill(Dataset, DatasetLength, Property, MTP_OBJ_PROP_VALUE_GETSET, MTP_GROUP_CODE, empty_wchar);
        break;

    default:
        uret = MTP_RC_INVALID_OBJECT_PROP_CODE;
        break;
    }
    return uret;
}




static UINT32 pascal_string_size_in_bytes_get(const UINT8 *str)
{
    UINT32 size = str[0];

    size = ((size & 0x0FFU) * 2U) + 1U;
    return size;
}

static UINT32 property_value_get_001(UINT8 *Buffer, ULONG Handle, ULONG Property, ULONG *PropValueLength)
{
    UINT32 uret = 0;
    char FileName[64] = {'\0'};

    switch (Property) {
    case MTP_OBJECT_PROP_STORAGEID:
        *PropValueLength = AppMtpd_LongPut(Buffer, g_mtp_class_info.DeviceInfo.StorageId);
        break;

    case MTP_OBJECT_PROP_OBJECT_FORMAT: {
        *PropValueLength = AppMtpd_ShortPut(Buffer, AppMtpd_DbFileFormatGet(Handle));
        break;
    }
    case MTP_OBJECT_PROP_PROTECTION_STATUS:
        *PropValueLength = AppMtpd_ShortPut(Buffer, 0);
        break;

    case MTP_OBJECT_PROP_OBJECT_SIZE: {
        UINT64 FileSize;
        FileSize          = AppMtpd_DbFileSizeGet(Handle);
        *PropValueLength  = AppMtpd_LongPut(Buffer, (UINT32)(FileSize & 0xFFFFFFFFU));
        *PropValueLength += AppMtpd_LongPut(&Buffer[4], (UINT32)(FileSize >> 32U));
        break;
    }

    case MTP_OBJECT_PROP_OBJECT_FILE_NAME: {
        if (AppMtpd_DbFileNameFromHandleGet(Handle, FileName) != 0U) {
            /* Store the file name in unicode format.  */
            AppMtpd_Asc2PascalString(FileName, Buffer);
            /* Set the length.  First Unicode string data.  */
            *PropValueLength = pascal_string_size_in_bytes_get(Buffer);
        } else {
            uret = MTP_RC_ACCESS_ERROR;
        }
        break;
    }

    case MTP_OBJECT_PROP_PARENT_OBJECT:
        *PropValueLength = AppMtpd_LongPut(Buffer, AppMtpd_DbParentHandleGet(Handle));
        break;

    case MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER:
        *PropValueLength  = AppMtpd_LongPut(Buffer, Handle);
        *PropValueLength += AppMtpd_LongPut(&Buffer[4], 0);
        *PropValueLength += AppMtpd_LongPut(&Buffer[8], 0);
        *PropValueLength += AppMtpd_LongPut(&Buffer[12], 0);
        break;

    case MTP_OBJECT_PROP_NAME:
        if (AppMtpd_DbFileNameFromHandleGet(Handle, FileName) != 0U) {
            /* Store the file name in unicode format.  */
            AppMtpd_Asc2PascalString(FileName, Buffer);

            /* Set the length.  First Unicode string data.  */
            *PropValueLength = pascal_string_size_in_bytes_get(Buffer);
        } else {
            uret = MTP_RC_ACCESS_ERROR;
        }

        break;

    case  MTP_OBJECT_PROP_NON_CONSUMABLE:

        Buffer[0]        = 0;
        *PropValueLength = 1;
        break;

    case MTP_OBJECT_PROP_ARTIST:
        //AppMtpd_Asc2PascalString(NULL, buf);
        //*PropValueLength = pascal_string_size_in_bytes_get(buf);
        uret = MTP_RC_INVALID_OBJECT_PROP_CODE;
        break;

    default:
        uret = MTP_RC_INVALID_OBJECT_PROP_CODE;
        break;
    }
    return uret;
}

static UINT32 property_value_get_002(UINT8 *Buffer, ULONG Handle, ULONG Property, ULONG *PropValueLength)
{
    UINT32 uret = 0;

    (void)Handle;

    switch (Property) {
    case MTP_OBJECT_PROP_TRACK:
        *PropValueLength = AppMtpd_ShortPut(Buffer, 0);
        break;

    case MTP_OBJECT_PROP_USE_COUNT:
        *PropValueLength = AppMtpd_ShortPut(Buffer, 0);
        break;

    case MTP_OBJECT_PROP_DATE_AUTHORED:
        //AppMtpd_Asc2PascalString(NULL, buf);
        //*PropValueLength = pascal_string_size_in_bytes_get(buf);
        uret = MTP_RC_INVALID_OBJECT_PROP_CODE;
        break;

    case MTP_OBJECT_PROP_GENRE:
        //AppMtpd_Asc2PascalString(0, buf);
        //*PropValueLength = pascal_string_size_in_bytes_get(buf);
        uret = MTP_RC_INVALID_OBJECT_PROP_CODE;
        break;

    case MTP_OBJECT_PROP_ALBUM_NAME:
        //AppMtpd_Asc2PascalString(0, buf);
        //*PropValueLength = pascal_string_size_in_bytes_get(buf);
        uret = MTP_RC_INVALID_OBJECT_PROP_CODE;
        break;

    case MTP_OBJECT_PROP_ALBUM_ARTIST:
        //AppMtpd_Asc2PascalString(0, buf);
        //*PropValueLength = pascal_string_size_in_bytes_get(buf);
        uret = MTP_RC_INVALID_OBJECT_PROP_CODE;
        break;

    case MTP_OBJECT_PROP_SAMPLE_RATE:
        *PropValueLength = AppMtpd_ShortPut(Buffer, 0);
        uret             = MTP_RC_INVALID_OBJECT_PROP_CODE;
        break;

    case MTP_OBJECT_PROP_NUMBER_OF_CHANNELS:
        *PropValueLength = AppMtpd_ShortPut(Buffer, 0);
        break;

    case MTP_OBJECT_PROP_AUDIO_WAVE_CODEC:
        *PropValueLength = AppMtpd_LongPut(Buffer, 0);
        break;

    case MTP_OBJECT_PROP_AUDIO_BITRATE:
        *PropValueLength = AppMtpd_LongPut(Buffer, 0);
        break;

    default:
        uret = MTP_RC_INVALID_OBJECT_PROP_CODE;
        break;
    }

    return uret;
}

static UINT32 property_value_get_003(UINT8 *Buffer, ULONG Handle, ULONG Property, ULONG *PropValueLength)
{
    UINT32 uret = 0;

    switch (Property) {

    case MTP_OBJECT_PROP_AUDIO_BITRATE:
        *PropValueLength = AppMtpd_LongPut(Buffer, 0);
        break;

    case MTP_OBJECT_PROP_DURATION:
        *PropValueLength = AppMtpd_LongPut(Buffer, 0);
        break;

    case MTP_OBJECT_PROP_WIDTH:
        *PropValueLength = AppMtpd_LongPut(Buffer, AppMtpd_DbObjectWidthGet(Handle));
        break;

    case MTP_OBJECT_PROP_HEIGHT:
        *PropValueLength = AppMtpd_LongPut(Buffer, AppMtpd_DbObjectHeightGet(Handle));
        break;

    case MTP_OBJECT_PROP_SCAN_TYPE:
        *PropValueLength = AppMtpd_ShortPut(Buffer, 0);
        break;

    case MTP_OBJECT_PROP_VIDEO_FOURCC_CODEC:
        *PropValueLength = AppMtpd_LongPut(Buffer, 0);
        break;

    case MTP_OBJECT_PROP_VIDEO_BITRATE:
        *PropValueLength = AppMtpd_LongPut(Buffer, 0);
        break;

    case MTP_OBJECT_PROP_FRAMES_PER_THOUSAND_SECONDS:
        *PropValueLength = AppMtpd_LongPut(Buffer, 0);
        break;

    case MTP_OBJECT_PROP_KEYFRAME_DISTANCE:
        *PropValueLength = AppMtpd_LongPut(Buffer, 0);
        break;

    case MTP_OBJECT_PROP_ENCODING_PROFILE:

        //AppMtpd_Asc2PascalString(0, buf);
        //*PropValueLength = pascal_string_size_in_bytes_get(buf);
        uret = MTP_RC_INVALID_OBJECT_PROP_CODE;
        break;

    default:
        uret = MTP_RC_INVALID_OBJECT_PROP_CODE;
        break;
    }
    return uret;
}

static UINT32 check_group_code(UINT32 group_code)
{
    UINT32 uret;

    if (MTP_GROUP_CODE == group_code) {
        uret = 1;
    } else {
        AppUsb_PrintUInt5("check_group_code(): don't know how to handle group code 0x%X", group_code, 0, 0, 0, 0);
        uret = 0;
    }
    return uret;
}

static UINT32 prop_list_elemt_short_fill(
    UINT8 *Buf, UINT32 Pos, ULONG Handle, ULONG Property, ULONG Value)
{
    Pos += AppMtpd_LongPut(&Buf[Pos], Handle);
    Pos += AppMtpd_ShortPut(&Buf[Pos], Property);
    Pos += AppMtpd_ShortPut(&Buf[Pos], MTP_TYPES_UINT16);
    Pos += AppMtpd_ShortPut(&Buf[Pos], Value);
    return Pos;
}

static UINT32 prop_list_element_long_fill(
    UINT8 *Buf, UINT32 Pos, ULONG Handle, ULONG Property, ULONG Value)
{
    Pos += AppMtpd_LongPut(&Buf[Pos], Handle);
    Pos += AppMtpd_ShortPut(&Buf[Pos], Property);
    Pos += AppMtpd_ShortPut(&Buf[Pos], MTP_TYPES_UINT32);
    Pos += AppMtpd_LongPut(&Buf[Pos], Value);
    return Pos;
}

static UINT32 prop_list_element_2long_fill(
    UINT8 *Buf, UINT32 Pos, ULONG Handle, ULONG Property, UINT64 Value)
{
    Pos += AppMtpd_LongPut(&Buf[Pos], Handle);
    Pos += AppMtpd_ShortPut(&Buf[Pos], Property);
    Pos += AppMtpd_ShortPut(&Buf[Pos], MTP_TYPES_UINT64);
    Pos += AppMtpd_LongPut(&Buf[Pos], (UINT32)Value);
    Pos += AppMtpd_LongPut(&Buf[Pos], (UINT32)(Value >> 32U));
    return Pos;
}

static UINT32 prop_list_element_4long_fill(
    UINT8 *Buf, UINT32 Pos, ULONG Handle, ULONG Property,
    ULONG Value1, ULONG Value2, ULONG Value3, ULONG Value4)
{
    Pos += AppMtpd_LongPut(&Buf[Pos], Handle);
    Pos += AppMtpd_ShortPut(&Buf[Pos], Property);
    Pos += AppMtpd_ShortPut(&Buf[Pos], MTP_TYPES_UINT128);
    Pos += AppMtpd_LongPut(&Buf[Pos], Value1);
    Pos += AppMtpd_LongPut(&Buf[Pos], Value2);
    Pos += AppMtpd_LongPut(&Buf[Pos], Value3);
    Pos += AppMtpd_LongPut(&Buf[Pos], Value4);
    return Pos;
}

static UINT32 prop_list_emelemt_string_fill(
    UINT8 *Buf, UINT32 Pos, UINT32 Handle, UINT16 Property, const char *AsciiString)
{
    UINT32 len;
    UINT8  pascall_str[USBD_MTP_MAX_FILENAME_LEN * 2U];
    UINT32 size;

    len = AmbaUtility_StringLength(AsciiString);
    size = (len + 3U) * 2U;

    if (size > sizeof(pascall_str)) {
        AppUsb_PrintUInt5("prop_list_emelemt_string_fill(): string size too big.", 0, 0, 0, 0, 0);
    } else {
        Pos += AppMtpd_LongPut(&Buf[Pos], Handle);
        Pos += AppMtpd_ShortPut(&Buf[Pos], Property);
        Pos += AppMtpd_ShortPut(&Buf[Pos], MTP_TYPES_STR);
        AppMtpd_Asc2PascalString(AsciiString, pascall_str);
        size = pascal_string_size_in_bytes_get(pascall_str);
        AppUsb_MemoryCopy(&Buf[Pos], pascall_str, size);
        Pos += size;
    }

    return Pos;
}

static void property_fill(UINT8 *Buffer,
                          UINT32 *Position,
                          UINT32 *ValidNumber,
                          UINT16 PropertyCode,
                          UINT32 Handle,
                          UINT32 FormatCode,
                          UINT32 *IsBreak,
                          UINT32 *URet)
{
    char name[64];

    switch (PropertyCode) {
    case MTP_OBJECT_PROP_STORAGEID: {
        *ValidNumber = *ValidNumber + 1U;
        *Position = prop_list_element_long_fill(
                        Buffer, *Position, Handle, PropertyCode, g_mtp_class_info.DeviceInfo.StorageId);
        break;
    }
    case MTP_OBJECT_PROP_OBJECT_FORMAT: {
        *ValidNumber = *ValidNumber + 1U;
        *Position = prop_list_elemt_short_fill(
                        Buffer, *Position, Handle, PropertyCode, FormatCode);
        break;
    }

    case MTP_OBJECT_PROP_PROTECTION_STATUS: {
        *ValidNumber = *ValidNumber + 1U;
        *Position = prop_list_elemt_short_fill(
                        Buffer, *Position, Handle, PropertyCode, 0);
        break;
    }

    case MTP_OBJECT_PROP_OBJECT_SIZE: {
        UINT64 size;
        *ValidNumber = *ValidNumber + 1U;
        size = AppMtpd_DbFileSizeGet(Handle);
        *Position  = prop_list_element_2long_fill(
                         Buffer, *Position, Handle, PropertyCode, size);
        break;
    }
    case MTP_OBJECT_PROP_WIDTH: {
        UINT32 width;
        width = AppMtpd_DbObjectWidthGet(Handle);
        *ValidNumber = *ValidNumber + 1U;
        *Position = prop_list_element_long_fill(
                        Buffer, *Position, Handle, PropertyCode, width);
        break;
    }
    case MTP_OBJECT_PROP_HEIGHT: {
        UINT32 height;
        height = AppMtpd_DbObjectHeightGet(Handle);
        *ValidNumber = *ValidNumber + 1U;
        *Position = prop_list_element_long_fill(
                        Buffer, *Position, Handle, PropertyCode, height);
        break;
    }

    case MTP_OBJECT_PROP_PARENT_OBJECT: {
        UINT32 parent_handle;
        parent_handle = AppMtpd_DbParentHandleGet(Handle);
        *ValidNumber = *ValidNumber + 1U;
        *Position = prop_list_element_long_fill(
                        Buffer, *Position, Handle, PropertyCode, parent_handle);
        break;
    }
    case MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER: {
        *ValidNumber = *ValidNumber + 1U;
        *Position = prop_list_element_4long_fill(
                        Buffer, *Position, Handle, PropertyCode, Handle, 0, 0, 0);
        break;
    }
    case MTP_OBJECT_PROP_OBJECT_FILE_NAME: {
        UINT32 rval = AppMtpd_DbFileNameFromHandleGet(Handle, name);
        if (rval != 0U) {
            AppUsb_PrintUInt5("property_fill() : can't get filename for handl 0x%x", Handle, 0, 0, 0, 0);
        } else {
            *ValidNumber = *ValidNumber + 1U;
            *Position = prop_list_emelemt_string_fill(
                            Buffer, *Position, Handle, PropertyCode, name);
        }
        break;
    }
    case MTP_OBJECT_PROP_DATE_AUTHORED: {
        *ValidNumber = *ValidNumber + 1U;
        *Position = prop_list_emelemt_string_fill(
                        Buffer, *Position, Handle, PropertyCode, "20140813T101430");
        break;
    }
    default:
        AppUsb_PrintUInt5("property_fill() : unsupported object proerpty code = 0x%x", PropertyCode, 0, 0, 0, 0);
        *URet     = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;
        *IsBreak = 1;
        break;
    }
}

static UINT32 prop_list_fill(const ULONG *Params, UINT8 *Buf, ULONG *Length, UINT32 Header, UINT32 *ElementNumber)
{
    UINT32 handle      = Params[0];
    UINT32 format_code = Params[1];
    UINT32 prop_code   = Params[2];
    UINT32 group_code  = Params[3];
    const UINT16 *prop_list;
    UINT16 prop_count = 0;
    UINT32 i;
    UINT32 pos              = 0;
    UINT16 valid_prop_code  = 0;
    UINT16 flag_check_group_code = 0;
    UINT16 cur_prop_code;
    UINT32 valid_number = *ElementNumber;
    UINT32 uret         = MTP_RC_OK;
    UINT32 is_break     = 0;

    if (format_code == 0x0U) {
        // all ObjectFormats are desired
        format_code = AppMtpd_DbFileFormatGet(handle);
    }

    prop_list = AppMtpd_ObjectPropListGet((UINT16)format_code);
    if (prop_list == NULL) {
        AppUsb_PrintUInt5("prop_list_fill(): property list is NULL for format code 0x%X", format_code, 0, 0, 0, 0);
        uret = MTP_RC_INVALID_DEVICE_PROP_FORMAT;
    } else {
        prop_list++;
        prop_count = *prop_list;
        prop_list++;

        if (Header != 0U) {
            pos += 4U;
        }

        for (i = 0; i < prop_count; i++) {
            if (is_break == 1U) {
                break;
            }

            cur_prop_code = prop_list[i];
            if (prop_code == 0x0U) {
                if (group_code == 0x0U) {
                    uret     = MTP_RC_PARAMETER_NOT_SUPPORTED;
                    is_break = 1;
                    continue;
                } else {
                    valid_prop_code  = cur_prop_code;
                    flag_check_group_code = 1;
                }
            } else if (prop_code == 0xFFFFFFFFU) {
                valid_prop_code = cur_prop_code;
            } else if (prop_code == cur_prop_code) {
                valid_prop_code = cur_prop_code;
            } else {
                // pass vcast checking
            }

            if (valid_prop_code != cur_prop_code) { //jump over this property code
                continue;
            } else {
                // pass vcast checking.
            }

            //dbg("%s(): DataCode = 0x%X", __FUNCTION__, cur_prop_code);
            if ((check_group_code(group_code) == 0U) && (flag_check_group_code != 0U)) {
                continue;
            }

            property_fill(Buf, &pos, &valid_number, cur_prop_code, handle, format_code, &is_break, &uret);
        }

        if (uret == MTP_RC_OK) {
            *ElementNumber = valid_number;
            *Length        = pos;
            //dbg("%s(): length = %d, number = %d", __func__, *Length, *ElementNumber);
        }
    }

    return uret;
}

/* If the value of the depth is 0x0 and objectHandle is also 0x0, the responder shall return an empty set */
static void prop_list_empty_fill(UINT8 *Buf, ULONG *Length, UINT32 *ElementNumber)
{
    *Length        = 4;
    *ElementNumber = 0;
    Buf[0] = 0;
    Buf[1] = 0;
    Buf[2] = 0;
    Buf[3] = 0;
    return;
}

static UINT32 prop_list_all_fill(ULONG *Params, UINT8 *Buf, ULONG *Length, UINT32 *ElementNumber)
{
    UINT32 total_count = AppMtpd_DbTotalHandleNumberGet();
    UINT32 count       = 0;
    UINT32 *handles;
    UINT32 local_element_number = 0;
    UINT32 uret                 = MTP_RC_OK;
    ULONG local_length          = 0;
    UINT32 header               = 1;
    UINT32 is_break             = 0;
    UINT32 target_size          = (total_count + 1U) * 4U;
    static UINT32 buffer_all_handles[MTP_MAX_HANDLES + 1U];
    UINT8 *tmp_buf = Buf;

    handles = buffer_all_handles;
    if (target_size > sizeof(buffer_all_handles)) {
        uret = MTP_RC_ACCESS_ERROR;
    } else {
        if (AppMtpd_DbObjectHandlesFill(0x0, 0x0, handles) == 0U) {
            *Length = 0;

            for (count = 0; count < total_count; count++) {
                if (is_break == 1U) {
                    break;
                }

                local_element_number = 0;
                local_length         = 0;
                Params[0]            = handles[count + 1U];
                uret                 = prop_list_fill(Params, tmp_buf, &local_length, header, &local_element_number);
                if (uret != MTP_RC_OK) {
                    AppUsb_PrintUInt5("prop_list_all_fill(): can't Fill PropList for Handle %d", count, 0, 0, 0, 0);
                    is_break = 1;
                    continue;
                }

                header = 0;

                *Length        += local_length;
                *ElementNumber += local_element_number;
                tmp_buf         = &Buf[local_length];

                if (APP_MTP_MAX_PROP_LIST_BUFFER_SIZE < *Length) {
                    AppUsb_PrintUInt5("prop_list_all_fill(): Length %d > %d.", *Length, APP_MTP_MAX_PROP_LIST_BUFFER_SIZE, 0, 0, 0);
                    uret     = MTP_RC_GENERAL_ERROR;
                    is_break = 1;
                    continue;
                }
            }
        } else {
            uret = MTP_RC_ACCESS_ERROR;
        }
    }

    return uret;
}

/** \addtogroup APPLICATION_APIS
 *  @{
 * */
/**
 *  Callback function for handling MTP request: Get Object Property Desc.
 *  */
UINT32 AppMtpd_CbObjectPropDescGet(ULONG Property, ULONG Format, UCHAR **Dataset, ULONG *DatasetLength)
{
    UINT32 uret         = 0;

    if (AppMtpd_IsObjectFormatSupported((UINT16)Format) == 0U) {
        AppUsb_PrintUInt5("AppMtpd_CbObjectPropDescGet(): Object Format 0x%X is not supported.", Format, 0, 0, 0, 0);
        uret = MTP_RC_INVALID_OBJECT_PROP_FORMAT;
    } else if (AppMtpd_IsObjectPropSupported((UINT16)Format, (UINT16)Property) == 0U) {
        AppUsb_PrintUInt5("AppMtpd_CbObjectPropDescGet(): Property 0x%X with Format 0x%X is not supported.", Property, Format, 0, 0, 0);
        uret = MTP_RC_OBJECT_PROP_NOT_SUPPORTED;
    } else {
        /* Check the object format belongs to the list. 3 ategories : generic, audio, video */
        switch (Format) {
        case    MTP_OFC_UNDEFINED:
        case    MTP_OFC_ASSOCIATION:
        case    MTP_OFC_EXIF_JPEG:
        case    MTP_OFC_MP3:
        case    MTP_OFC_ASF:
        case    MTP_OFC_WMA:
        case    MTP_OFC_WMV:
        case    MTP_OFC_ABSTRACT_AUDIO_ALBUM:
        case    MTP_OFC_ABSTRACT_AUDIO_AND_VIDEO_PLAYLIST:
            uret = property_desc_get_001(Property, Dataset, DatasetLength);
            if (uret == MTP_RC_INVALID_OBJECT_PROP_CODE) {
                uret = property_desc_get_002(Property, Dataset, DatasetLength);
            }
            if (uret == MTP_RC_INVALID_OBJECT_PROP_CODE) {
                uret = property_desc_get_003(Property, Dataset, DatasetLength);
            }
            break;
        default:
            /* We get here when we have the wrong format code.  */
            uret = MTP_RC_INVALID_OBJECT_FORMAT_CODE;
            break;
        }
    }
    return uret;
}

/**
 *  Callback function for handling MTP request: Get Object Property Value.
 *  */
UINT32 AppMtpd_CbObjectPropValueGet(ULONG Handle, ULONG Property, UCHAR **PropValue, ULONG *PropValueLength)
{
    UINT32 status;
    UINT8 *buf    = NULL;

    /* Check the object handle. It must be in the local array.  */
    status = AppMtpd_ObjectHandleCheck(Handle);

    /* Does the object handle exist ?  */
    if (status == 0U) {
        buf = prop_value_buffer_get(APP_MTP_MAX_PROP_VALUE_BUFFER_SIZE);
        if (buf == NULL) {
            AppUsb_PrintUInt5("AppMtpd_CbObjectPropValueGet(): can't get data buffer.", 0, 0, 0, 0, 0);
            status = MTP_RC_GENERAL_ERROR;
        } else {
            *PropValue = buf;

            status = property_value_get_001(buf, Handle, Property, PropValueLength);
            if (status == MTP_RC_INVALID_OBJECT_PROP_CODE) {
                status = property_value_get_002(buf, Handle, Property, PropValueLength);
            }
            if (status == MTP_RC_INVALID_OBJECT_PROP_CODE) {
                status = property_value_get_003(buf, Handle, Property, PropValueLength);
            }
        }
    } else {
        status = MTP_RC_INVALID_OBJECT_HANDLE;
    }
    return status;
}

typedef struct {
    const char *OldPath;
    const char *NewFileName;
    UINT32      Result;
    UINT32      FlagDone;
} FILE_RENAME_ARG_s;

static FILE_RENAME_ARG_s file_rename_arg;

static void *file_rename_func(void *Arg)
{
    UINT32 uret;

    (void)Arg;

    uret = AmbaFS_Rename(file_rename_arg.OldPath, file_rename_arg.NewFileName);
    if (uret != 0U) {
        AppUsb_PrintStr5("file_rename_func(): fail to rename %s -> %s", file_rename_arg.OldPath, file_rename_arg.NewFileName, NULL, NULL, NULL);
    } else {
        AppUsb_PrintStr5("file_rename_func(): %s -> %s", file_rename_arg.OldPath, file_rename_arg.NewFileName, NULL, NULL, NULL);
    }
    file_rename_arg.Result = uret;
    file_rename_arg.FlagDone = 1;
    return NULL;
}

static UINT32 prop_file_rename_impl(const char *OldPath, const char *NewFileName)
{
    static char task_name[] = "appmtpd_rename_task";
    static UINT8 task_stack[16 * 1024];
    AMBA_KAL_TASK_t task;
    UINT32 func_uret;
    UINT32 retry_count = 0;

    // prepare reanme parameters
    file_rename_arg.FlagDone = 0;
    file_rename_arg.OldPath = OldPath;
    file_rename_arg.NewFileName = NewFileName;

    // create task to do rename
    func_uret = AppUsb_TaskCreate(&task, task_name, 70, file_rename_func, NULL, task_stack, sizeof(task_stack), 1, 1);
    if (func_uret == 0U) {
        while (file_rename_arg.FlagDone == 0U) {
            AppUsb_TaskSleep(10);
            retry_count++;
            if (retry_count > 200U) {
                AppUsb_PrintUInt5("prop_file_rename_impl(): fail to wait task done.", 0, 0, 0, 0, 0);
                file_rename_arg.Result = 0xFFFFFFFFUL;
                break;
            }
        }
        AppUsb_PrintUInt5("prop_file_rename_impl(): rename task done.", 0, 0, 0, 0, 0);
        func_uret = AppUsb_TaskDelete(&task, 1);
        if (func_uret != 0U) {
            AppUsb_PrintUInt5("prop_file_rename_impl(): fail to delete task.", 0, 0, 0, 0, 0);
        }
    } else {
        AppUsb_PrintUInt5("prop_file_rename_impl(): fail to create task.", 0, 0, 0, 0, 0);
    }
    return file_rename_arg.Result;
}


static void prop_file_rename(ULONG ObjectHandle, const char *NewFileName)
{
    static char old_file_path[MTP_MAX_FILENAME_LENGTH];
    static char new_file_path[MTP_MAX_FILENAME_LENGTH];
    UINT32 uret;

    uret = AppMtpd_DbFilePathFromHandleGet(ObjectHandle, old_file_path);
    if (uret == 0U) {
        // find last '\\' in old_file_path
        UINT32 string_length = AmbaUtility_StringLength(old_file_path);
        UINT32 idx;
        for (idx = (string_length - 1U); idx != 0U; idx--) {
            if (old_file_path[idx] == '\\') {
                break;
            }
        }

        if (idx != 0U) {
            // make new file path with NewFileName
            string_length = AmbaUtility_StringLength(NewFileName);
            AppUsb_MemoryZeroSet(new_file_path, MTP_MAX_FILENAME_LENGTH);
            AppUsb_MemoryCopy(new_file_path, old_file_path, idx + 1U);
            AppUsb_MemoryCopy(&new_file_path[idx + 1U], NewFileName, string_length);

            AppUsb_PrintStr5("prop_file_rename(): %s -> %s", old_file_path, new_file_path, NULL, NULL, NULL);

            // the calling thread is PIMA thread, and the stack size may be small.
            // try to call AmbaFS APIs in another thread which having larger stack size.
            uret = prop_file_rename_impl(old_file_path, NewFileName);

            if (uret == 0U) {
                uret = AppMtpd_DbHandlePathUpdate(ObjectHandle, new_file_path);
                if (uret != 0U) {
                    // ignore error
                }
            }
        }
    } else {
        AppUsb_PrintUInt5("prop_file_rename(): can't get file path for object %d.", ObjectHandle, 0, 0, 0, 0);
    }
}

/**
 *  Callback function for handling MTP request: Set Object Property Value.
 *
 */
UINT32 AppMtpd_CbObjectPropValueSet(ULONG ObjectHandle, ULONG ObjectProperty, UCHAR *ObjectPropValue, ULONG ObjectPropValueLength)
{
    static char file_name_asc[64];
    UINT32 status;
    (VOID)ObjectPropValue;
    (VOID)ObjectPropValueLength;

    if (AppMtpd_IsOperationSupported(MTP_OC_SET_OBJECT_PROP_VALUE) == 0U) {
        status = MTP_RC_OPERATION_NOT_SUPPORTED;
    } else {
        /* Check the object handle. It must be in the local array.  */
        status = AppMtpd_ObjectHandleCheck(ObjectHandle);

        if (status == 0U) {
            /* Isolate the property.  This is SET. So the properties that are GET only will not be changed.  */
            switch (ObjectProperty) {
            case    MTP_OBJECT_PROP_STORAGEID:
            case    MTP_OBJECT_PROP_OBJECT_FORMAT:
            case    MTP_OBJECT_PROP_OBJECT_SIZE:
            case    MTP_OBJECT_PROP_PARENT_OBJECT:
            case    MTP_OBJECT_PROP_DURATION:
                status = MTP_RC_OBJECT_WRITE_PROTECTED;
                break;

            case    MTP_OBJECT_PROP_OBJECT_FILE_NAME:
                if (ObjectPropValueLength < 128U) {
                    AppMtpd_Pascal2AscString(ObjectPropValue, file_name_asc);
                    AppUsb_PrintStr5("AppMtpd_CbObjectPropValueSet(): FileName %s.", file_name_asc, NULL, NULL, NULL, NULL);
                    prop_file_rename(ObjectHandle, file_name_asc);
                } else {
                    AppUsb_PrintUInt5("AppMtpd_CbObjectPropValueSet(): FileName Length = %d too large.", ObjectPropValueLength, 0, 0, 0, 0);
                }
                status = OK;
                break;

            case    MTP_OBJECT_PROP_NAME:
                status = OK;
                break;

            case    MTP_OBJECT_PROP_ARTIST:
                status = OK;
                break;

            case    MTP_OBJECT_PROP_DATE_AUTHORED:
                status = OK;
                break;

            case    MTP_OBJECT_PROP_GENRE:
                status = OK;
                break;

            default:
                status = MTP_RC_INVALID_OBJECT_PROP_CODE;
                break;
            }
        } else {
            status = MTP_RC_INVALID_OBJECT_HANDLE;
        }
    }

    return status;
}

/**
 *  Callback function for handling MTP request: Get Object Property List.
 *  */
UINT32 AppMtpd_CbObjectPropListGet(ULONG *Params, UCHAR **PropList, ULONG *PropListLength)
{
    UINT8 *buf;
    UINT32 handle         = Params[0];
    UINT32 depth          = Params[4];
    ULONG  length         = 0;
    UINT32 element_number = 0;
    UINT32 uret           = MTP_RC_OK;

    if (AppMtpd_IsOperationSupported(MTP_OC_GET_OBJECT_PROP_LIST) == 0U) {
        uret = MTP_RC_OPERATION_NOT_SUPPORTED;
    } else {

        if ((handle != 0x0U) && (handle != 0xFFFFFFFFU)) {
            if (AppMtpd_ObjectHandleCheck(handle) != 0U) {
                AppUsb_PrintUInt5("AppMtpd_CbObjectPropListGet(): Invalid Handle 0x%X", handle, 0, 0, 0, 0);
                uret = MTP_RC_INVALID_OBJECT_HANDLE;
            }
        }

        if (uret == MTP_RC_OK) {
            buf = prop_list_buffer_get(APP_MTP_MAX_PROP_LIST_BUFFER_SIZE);
            if (buf == NULL) {
                AppUsb_PrintUInt5("AppMtpd_CbObjectPropListGet(): can't allocate data buffer.", 0, 0, 0, 0, 0);
                uret = MTP_RC_GENERAL_ERROR;
            } else {
                switch (depth) {
                case 0:
                    switch (handle) {
                    case 0U:
                        prop_list_empty_fill(buf, &length, &element_number);
                        *PropList       = buf;
                        *PropListLength = length;
                        (void)AppMtpd_LongPut(buf, element_number);
                        break;
                    case 0xFFFFFFFFU:
                        uret            = prop_list_all_fill(Params, buf, &length, &element_number);
                        *PropList       = buf;
                        *PropListLength = length;
                        (void)AppMtpd_LongPut(buf, element_number);
                        break;
                    default:
                        uret            = prop_list_fill(Params, buf, &length, 1, &element_number);
                        *PropList       = buf;
                        *PropListLength = length;
                        (void)AppMtpd_LongPut(buf, element_number);
                        break;
                    }
                    break;
                case 0xFFFFFFFFU:
                default:
                    AppUsb_PrintUInt5("AppMtpd_CbObjectPropListGet(): Not supported depth 0x%X.", depth, 0, 0, 0, 0);
                    uret = MTP_RC_SPECIFICATION_BY_DEPTH_UNSUPPORTED;
                    break;
                }

                if (uret == MTP_RC_OK) {
                    if (length > APP_MTP_MAX_PROP_LIST_BUFFER_SIZE) {
                        AppUsb_PrintUInt5("AppMtpd_CbObjectPropListGet(): Length %d > %d.", length, APP_MTP_MAX_PROP_LIST_BUFFER_SIZE, 0, 0, 0);
                        uret = MTP_RC_GENERAL_ERROR;
                    }
                }
            }
        }
    }

    return uret;
}
/** @} */
