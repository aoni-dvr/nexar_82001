/**
 *  @file MTPObjectProp.c
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
 *  @details USB MTP device class functions for APP/MW. It handles MTP object properties.
 */

#ifndef COMSVC_MTP_OPROP_H      /* Dir 4.10 */
#define COMSVC_MTP_OPROP_H

#define MTP_GROUP_CODE                        0x1U

#define APP_MTP_MAX_PROP_VALUE_BUFFER_SIZE    ((255U*2U) + 1U)
#define APP_MTP_MAX_PROP_LIST_BUFFER_SIZE     (16U*1024U)
#define APP_MTP_MAX_PROP_DESC_BUFFER_SIZE     (16U*1024U)

#define PROPERTY_DESC_SIZE_INT16_NO_FORM_FLAG 12
#define PROPERTY_DESC_SIZE_INT32_NO_FORM_FLAG 18
//#define PROPERTY_DESC_SIZE_INT128_NO_FORM_FLAG 26

static UINT16 DeviceSupportImgFormat[] = {
    MTP_OFC_UNDEFINED,
    MTP_OFC_ASSOCIATION,
    MTP_OFC_EXIF_JPEG,
    0
};

static UINT16 ObjectSupportProp[] = {
    /* Object format code : Undefined.  */
    MTP_OFC_UNDEFINED,

    /* NUmber of objects supported for this format.  */
    7,
    /* Mandatory objects for all formats.  */
    MTP_OBJECT_PROP_STORAGEID,
    MTP_OBJECT_PROP_OBJECT_FORMAT,
    MTP_OBJECT_PROP_PROTECTION_STATUS,
    MTP_OBJECT_PROP_OBJECT_SIZE,
    MTP_OBJECT_PROP_OBJECT_FILE_NAME,
    MTP_OBJECT_PROP_PARENT_OBJECT,
    MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER,
    //MTP_OBJECT_PROP_NAME,
    //MTP_OBJECT_PROP_NON_CONSUMABLE,

    /* Object format code : Association.  */
    MTP_OFC_ASSOCIATION,

    /* NUmber of objects supported for this format.  */
    7,
    /* Mandatory objects for all formats.  */
    MTP_OBJECT_PROP_STORAGEID,
    MTP_OBJECT_PROP_OBJECT_FORMAT,
    MTP_OBJECT_PROP_PROTECTION_STATUS,
    MTP_OBJECT_PROP_OBJECT_SIZE,
    MTP_OBJECT_PROP_OBJECT_FILE_NAME,
    MTP_OBJECT_PROP_PARENT_OBJECT,
    MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER,
    //MTP_OBJECT_PROP_NAME,
    //MTP_OBJECT_PROP_NON_CONSUMABLE,

    /* Object format code : Windows Media Audio Clip.  */
    MTP_OFC_EXIF_JPEG,

    /* NUmber of objects supported for this format.  */
    10,
    /* Mandatory objects for all formats.  */
    MTP_OBJECT_PROP_STORAGEID,
    MTP_OBJECT_PROP_OBJECT_FORMAT,
    MTP_OBJECT_PROP_PROTECTION_STATUS,
    MTP_OBJECT_PROP_OBJECT_SIZE,
    MTP_OBJECT_PROP_OBJECT_FILE_NAME,
    MTP_OBJECT_PROP_PARENT_OBJECT,
    MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER,
    //MTP_OBJECT_PROP_NAME,
    //MTP_OBJECT_PROP_NON_CONSUMABLE,

    /* Mandatory objects for all image objects.  */
    MTP_OBJECT_PROP_WIDTH,
    MTP_OBJECT_PROP_HEIGHT,
    MTP_OBJECT_PROP_DATE_AUTHORED,

    0
};

static UINT16 OperationSupportList[] = {
    MTP_OC_GET_DEVICE_INFO,
    MTP_OC_OPEN_SESSION,
    MTP_OC_CLOSE_SESSION,
    MTP_OC_GET_STORAGE_IDS,
    MTP_OC_GET_STORAGE_INFO,
    MTP_OC_GET_NUM_OBJECTS,
    MTP_OC_GET_OBJECT_HANDLES,
    MTP_OC_GET_OBJECT_INFO,
    MTP_OC_GET_OBJECT,
    MTP_OC_GET_THUMB,
    MTP_OC_GET_PARTIAL_OBJECT,
    MTP_OC_DELETE_OBJECT,
    MTP_OC_SEND_OBJECT_INFO,
    MTP_OC_SEND_OBJECT,
    MTP_OC_INITIATE_CAPTURE,
    MTP_OC_INITIATE_OPEN_CAPTURE,
    MTP_OC_TERMINATE_OPEN_CAPTURE,
    //MTP_OC_FORMAT_STORE,
    MTP_OC_RESET_DEVICE,
    MTP_OC_GET_DEVICE_PROP_DESC,
    MTP_OC_GET_DEVICE_PROP_VALUE,
    MTP_OC_SET_DEVICE_PROP_VALUE,
    MTP_OC_RESET_DEVICE_PROP_VALUE,
    MTP_OC_GET_OBJECT_PROPS_SUPPORTED,
    MTP_OC_GET_OBJECT_PROP_DESC,
    MTP_OC_GET_OBJECT_PROP_VALUE,
    MTP_OC_SET_OBJECT_PROP_VALUE,
    //MTP_OC_GET_OBJECT_REFERENCES,
    //MTP_OC_SET_OBJECT_REFERENCES,
    MTP_OC_GET_OBJECT_PROP_LIST,
    MTP_OC_CUSTOM_COMMAND,
    0
};

static UINT8 *GetPropValueBuffer(UINT32 Size)
{
    static UINT8 PropValueBuffer[APP_MTP_MAX_PROP_VALUE_BUFFER_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    UINT8 *pRet = PropValueBuffer;

    if (Size > APP_MTP_MAX_PROP_VALUE_BUFFER_SIZE) {
        pRet = NULL;
    }
    return pRet;
}

static UINT8 *GetPropListBuffer(UINT32 Size)
{
    static UINT8 PropListBuffer[APP_MTP_MAX_PROP_LIST_BUFFER_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    UINT8 *pRet = PropListBuffer;

    if (Size > APP_MTP_MAX_PROP_LIST_BUFFER_SIZE) {
        pRet = NULL;
    }
    return pRet;
}

static UINT8 *GetPropDescBuffer(UINT32 Size)
{
    static UINT8 PropDescBuffer[APP_MTP_MAX_PROP_DESC_BUFFER_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    UINT8 *pRet = PropDescBuffer;

    if (Size > APP_MTP_MAX_PROP_DESC_BUFFER_SIZE) {
        pRet = NULL;
    }
    return pRet;
}

static UINT32 FillPropDesc_ShortNoForm(UINT8 **Buf, ULONG *Length, ULONG PropertyCode, UINT32 Access, UINT32 DefaultValue, UINT32 GroupCode)
{
    UINT32 status = OK;
    UINT8 *ptr    = NULL;

    *Length = PROPERTY_DESC_SIZE_INT16_NO_FORM_FLAG;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_ShortNoForm(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        // Property code.
        if (0U < MTP_Utility_ShortPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_CODE], PropertyCode)) {
            /* do nothing */
        }

        // Data type is UINT16.
        if (0U < MTP_Utility_ShortPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_DATATYPE], MTP_TYPES_UINT16)) {
            /* do nothing */
        }

        // GetSet value
        ptr[MTP_OBJECT_PROPERTY_DATASET_GETSET] = (UINT8)Access;

        // Default value is UINT16.  */
        if (0U < MTP_Utility_ShortPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_VALUE], DefaultValue)) {
            /* do nothing */
        }

        // Group code
        if (0U < MTP_Utility_LongPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_VALUE + 2U], GroupCode)) {
            /* do nothing */
        }

        // Form Flag is not used.
        ptr[MTP_OBJECT_PROPERTY_DATASET_VALUE + 6U] = 0;
    }

    return status;
}

static UINT32 FillPropDesc_LongNoForm(UINT8 **Buf, ULONG *Length, ULONG PropertyCode, UINT32 Access, UINT32 DefaultValue, UINT32 GroupCode)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;

    *Length = PROPERTY_DESC_SIZE_INT32_NO_FORM_FLAG;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_LongNoForm(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        // Property code.
        if (0U < MTP_Utility_ShortPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_CODE], PropertyCode)) {
            /* do nothing */
        }

        // Data type is UINT16.
        if (0U < MTP_Utility_ShortPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_DATATYPE], MTP_TYPES_UINT32)) {
            /* do nothing */
        }

        // GetSet value
        ptr[MTP_OBJECT_PROPERTY_DATASET_GETSET] = (UINT8)Access;

        // Default value is UINT32.  */
        if (0U < MTP_Utility_LongPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_VALUE], DefaultValue)) {
            /* do nothing */
        }

        // Group code
        if (0U < MTP_Utility_LongPut(&ptr[MTP_OBJECT_PROPERTY_DATASET_VALUE + 4U], GroupCode)) {
            /* do nothing */
        }

        // Form Flag is not used.
        ptr[MTP_OBJECT_PROPERTY_DATASET_VALUE + 8U] = 0;
    }
    return status;
}


static UINT32 FillPropDesc_ProtectionStatus(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 18;

    *Buf = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_ProtectionStatus(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(ptr, MTP_OBJECT_PROP_PROTECTION_STATUS);

        /* Data type is UINT16.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT16);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT16.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], 0);

        /* Group code is 4.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag ENUM.  */
        ptr[pos] = 2;
        pos++;

        /* Number of elements in Enum array.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], 2);

        /* Elements in Enum array.  Here we store only No protection and Read-Only protection values. This can be extended with
           Read-only data and Non transferrable data. Spec talks about MTP vendor extension range as well. Not used here.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], 0);
        pos += MTP_Utility_ShortPut(&ptr[pos], 1);

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_ProtectionStatus(): pos != length", 0U, 0U, 0U, 0U, 0U);
        }
    }


    return status;
}

static UINT32 FillPropDesc_ObjectSize(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    ULONG pos     = 0;

    *Length = 18;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_ObjectSize(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(ptr, MTP_OBJECT_PROP_OBJECT_SIZE);

        /* Data type is UINT64.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT64);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT64.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0);
        pos += MTP_Utility_LongPut(&ptr[pos], 0);

        /* Group code is NULL.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag is not used.  */
        ptr[pos] = 0;
        pos++;

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_ObjectSize(): pos != length", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return status;
}

static UINT32 FillPropDesc_ParentObject(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 14;

    *Buf = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_ParentObject(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(ptr, MTP_OBJECT_PROP_PARENT_OBJECT);

        /* Data type is UINT32.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT32.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0);

        /* Group code is NULL.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag is not used.  */
        ptr[pos] = 0;
        pos++;

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_ParentObject(): pos != length", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return status;
}

static UINT32 FillPropDesc_PersistentUniqueObjectIdentifier(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 26;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_PersistentUniqueObjectIdentifier(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(ptr, MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER);

        /* Data type is UINT128.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT128);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT128.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0);
        pos += MTP_Utility_LongPut(&ptr[pos], 0);
        pos += MTP_Utility_LongPut(&ptr[pos], 0);
        pos += MTP_Utility_LongPut(&ptr[pos], 0);

        /* Group code is NULL.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag is not used.  */
        ptr[pos] = 0;
        pos++;

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_PersistentUniqueObjectIdentifier(): pos != length", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return status;
}

static UINT32 FillPropDesc_NonConsumable(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 15;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_NonConsumable(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(ptr, MTP_OBJECT_PROP_NON_CONSUMABLE);

        /* Data type is UINT8.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT8);

        /* GetSet value is GET/SET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GETSET;
        pos++;

        /* Default value is UINT8.  */
        ptr[pos] = 0;
        pos++;

        /* Group code is 2.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag ENUM.  */
        ptr[pos] = 2;
        pos++;

        /* Number of elements in Enum array.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], 2);

        /* Elements in Enum array.  */
        ptr[pos] = 0;
        pos++;
        ptr[pos] = 1;
        pos++;

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_NonConsumable(): pos != length", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return status;
}

static UINT32 FillPropDesc_UseCount(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 14;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_UseCount(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(ptr, MTP_OBJECT_PROP_USE_COUNT);

        /* Data type is UINT8.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET/SET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GETSET;
        pos++;

        /* Default value is UINT32.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0);

        /* Group code is 1.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag is not used.  */
        ptr[pos] = 0;
        pos++;

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_UseCount(): pos != length", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return status;
}

static UINT32 FillPropDesc_SampleRate(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 26;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_SampleRate(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(ptr, MTP_OBJECT_PROP_SAMPLE_RATE);

        /* Data type is UINT32.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT32.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag RANGE.  */
        ptr[pos] = 1;
        pos++;

        /* Minimum range in array is 0KHZ.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x00000000);

        /* Maximum range in array is KHZ.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x0002EE00);

        /* Range step size is 32HZ.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x00000020);

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_SampleRate(): pos != length", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return status;
}

static UINT32 FillPropDesc_NumberOfChannels(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 20;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_NumberOfChannels(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(ptr, MTP_OBJECT_PROP_NUMBER_OF_CHANNELS);

        /* Data type is UINT16.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT16);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT16.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag ENUM.  */
        ptr[pos] = 2;
        pos++;

        /* Number of elements in Enum array.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], 3);

        /* Elements in Enum array.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], 0);
        pos += MTP_Utility_ShortPut(&ptr[pos], 1);
        pos += MTP_Utility_ShortPut(&ptr[pos], 2);

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_NumberOfChannels(): pos != length", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return status;
}

static UINT32 FillPropDesc_AudioWaveCodec(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 28;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_AudioWaveCodec(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_OBJECT_PROP_AUDIO_WAVE_CODEC);

        /* Data type is UINT32.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT32.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag ENUM.  */
        ptr[pos] = 2;
        pos++;

        /* Number of elements in Enum array.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], 3);

        /* Elements in Enum array.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_WAVE_FORMAT_MPEGLAYER3);
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_WAVE_FORMAT_MPEG);
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_WAVE_FORMAT_RAW_AAC1);

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_AudioWaveCodec(): pos != length.", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return status;
}

static UINT32 FillPropDesc_AudioBitrate(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 26;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_AudioBitrate(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_OBJECT_PROP_AUDIO_BITRATE);

        /* Data type is UINT32.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT32.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x0000FA00);

        /* Group code is 0.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag RANGE.  */
        ptr[pos] = 1;
        pos++;

        /* Minimum range in array is 1 bit per second.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x00000001);

        /* Maximum range in array is 1,500,000 bit per second.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x0016E360);

        /* Range step size is 1 bit per second.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x00000001);

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_AudioBitrate(): pos != length", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return status;
}

static UINT32 FillPropDesc_AudioDuration(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 14;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_AudioDuration(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_OBJECT_PROP_DURATION);

        /* Data type is UINT32.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT32.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0);

        /* Group code is NULL.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag is 1.  */
        ptr[pos] = 1;
        pos++;

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_AudioDuration(): pos != length.", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return status;
}

static UINT32 FillPropDesc_Width(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 26;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_Width(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_OBJECT_PROP_WIDTH);

        /* Data type is UINT32.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT32.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag RANGE.  */
        ptr[pos] = 1;
        pos++;

        /* Minimum range in array is customer defined.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0);

        /* Maximum range in array is customer defined.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 10000);

        /* Range step size is customer defined.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 1);

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_Width(): pos != length", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return status;
}

static UINT32 FillPropDesc_Height(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 26;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_Height(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_OBJECT_PROP_HEIGHT);

        /* Data type is UINT32.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT16.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x0000);

        /* Group code is 0.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag RANGE.  */
        ptr[pos] = 1;
        pos++;

        /* Minimum range in array is customer defined.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x0000);

        /* Maximum range in array is customer defined.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 10000);

        /* Range step size is customer defined.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 1);

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_Height(): pos != length", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return status;
}

static UINT32 FillPropDesc_ScanType(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 29;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_ScanType(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_OBJECT_PROP_SCAN_TYPE);

        /* Data type is UINT16.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT16);

        /* GetSet value is GET/SET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GETSET;
        pos++;

        /* Default value is UINT16.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag ENUM.  */
        ptr[pos] = 2;
        pos++;

        /* Number of elements in Enum array.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], 8);

        /* Elements in Enum array.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], 0x0000);
        pos += MTP_Utility_ShortPut(&ptr[pos], 0x0001);
        pos += MTP_Utility_ShortPut(&ptr[pos], 0x0002);
        pos += MTP_Utility_ShortPut(&ptr[pos], 0x0003);
        pos += MTP_Utility_ShortPut(&ptr[pos], 0x0004);
        pos += MTP_Utility_ShortPut(&ptr[pos], 0x0005);
        pos += MTP_Utility_ShortPut(&ptr[pos], 0x0006);
        pos += MTP_Utility_ShortPut(&ptr[pos], 0x0007);

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_ScanType(): pos != length", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return status;
}

static UINT32 FillPropDesc_FourCCCodec(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 22;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_FourCCCodec(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_OBJECT_PROP_VIDEO_FOURCC_CODEC);

        /* Data type is UINT32.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET/SET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GETSET;
        pos++;

        /* Default value is UINT32.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag ENUM.  */
        ptr[pos] = 2;
        pos++;

        /* Number of elements in Enum array.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 1);

        /* Elements in Enum array.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x00000000);

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_FourCCCodec(): pos != length", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return status;
}

static UINT32 FillPropDesc_VideoBitrate(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 26;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_VideoBitrate(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(ptr, MTP_OBJECT_PROP_VIDEO_BITRATE);

        /* Data type is UINT32.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GET;
        pos++;

        /* Default value is UINT32.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag RANGE.  */
        ptr[pos] = 1;
        pos++;

        /* Minimum range in array is 0.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x00000000);

        /* Maximum range in array is 0xFFFFFFFF.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0xFFFFFFFFU);

        /* Range step size is 1.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x00000001);

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_VideoBitrate(): pos != length.", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return status;
}

static UINT32 FillPropDesc_FramePerThousandSeconds(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 22;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_FramePerThousandSeconds(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(ptr, MTP_OBJECT_PROP_FRAMES_PER_THOUSAND_SECONDS);

        /* Data type is UINT32.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET/SET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GETSET;
        pos++;

        /* Default value is UINT32.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag ENUM.  */
        ptr[pos] = 2;
        pos++;

        /* Number of elements in Enum array.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 1);

        /* Elements in Enum array.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x00000000);

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_FramePerThousandSeconds(): pos != length", 0U, 0U, 0, 0, 0);
        }
    }

    return status;
}

static UINT32 FillPropDesc_KeyFrameDistance(UINT8 **Buf, ULONG *Length)
{
    UINT32 status = 0U;
    UINT8 *ptr    = NULL;
    UINT32 pos    = 0;

    *Length = 26;
    *Buf    = GetPropDescBuffer(32);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_KeyFrameDistance(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(ptr, MTP_OBJECT_PROP_KEYFRAME_DISTANCE);

        /* Data type is UINT32.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_UINT32);

        /* GetSet value is GET/SET.  */
        ptr[pos] = MTP_OBJ_PROP_VALUE_GETSET;
        pos++;

        /* Default value is UINT32.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0);

        /* Group code is 0.  */
        pos += MTP_Utility_LongPut(&ptr[pos], MTP_GROUP_CODE);

        /* Form Flag RANGE.  */
        ptr[pos] = 1;
        pos++;

        /* Minimum range in array is 0.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x00000000);

        /* Maximum range in array is FFFFFFFF.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x0000FFFF);

        /* Range step size is 1.  */
        pos += MTP_Utility_LongPut(&ptr[pos], 0x00000001);

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_KeyFrameDistance(): pos != length", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return status;
}

static UINT32 FillPropDesc_String(UINT8 **Buf, ULONG *Length, ULONG Property, UINT32 Access, UINT32 Group, const WCHAR *str_uni)
{
    UINT32 status      = OK;
    UINT8  *ptr        = NULL;
    UINT32 pos         = 0;
    ULONG  uni_str_len = WStrLen(str_uni);

    if (uni_str_len == 0U) {
        *Length = (10U + 1U);                      // 1 byte for length
    } else {
        *Length = (10U + (uni_str_len * 2U) + 3U); // 1 byte for length and 2 bytes for NULL-terminated character.
    }

    *Buf = GetPropDescBuffer((UINT32)*Length);
    if (*Buf == NULL) {
        AmbaPrint_PrintUInt5("FillPropDesc_String(): can't allocate memory.", 0, 0, 0, 0, 0);
        status = MTP_RC_GENERAL_ERROR;
    } else {
        ptr = *Buf;

        /* Add the property code.  */
        pos += MTP_Utility_ShortPut(ptr, Property);

        /* Data type is STRING.  */
        pos += MTP_Utility_ShortPut(&ptr[pos], MTP_TYPES_STR);

        /* GetSet value.  */
        ptr[pos] = (UINT8)Access;
        pos++;

        /* Store a Unicode string.   */
        pos += MTP_Utility_Add_String(&ptr[pos], str_uni);

        // Group code
        pos += MTP_Utility_LongPut(&ptr[pos], Group);

        /* Form Flag is not used.  */
        ptr[pos] = 0;
        pos++;

        if (pos != *Length) {
            AmbaPrint_PrintUInt5("FillPropDesc_String(): pos != length or Property", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return status;
}

static UINT32 IsObjectPropertySupported(UINT16 Format, UINT16 Property)
{
    const UINT16 *ptr = ObjectSupportProp;
    UINT16 of         = 0;
    UINT16 count      = 0;
    UINT16 i;
    UINT32 uRet     = 0;
    UINT32 idx      = 0;
    UINT32 is_break = 0;

    do {
        if (ptr[idx] == 0U) {
            uRet     = 0;
            is_break = 1;
        } else {
            of = ptr[idx];
            idx++;
            count = ptr[idx];
            idx++;
            if (of == Format) {
                for (i = 0; i < count; i++) {
                    if (ptr[idx] == Property) {
                        uRet = 1;
                        break;
                    }
                    idx++;
                }
                is_break = 1;
            } else {
                idx += count;
            }
        }

        if (is_break == 1U) {
            break;
        }
    } while (ptr[idx] != 0U);

    return uRet;
}


static UINT32 IsObjectFormatSupported(UINT16 Format)
{
    UINT32 count = (UINT32)(sizeof(DeviceSupportImgFormat) / sizeof(UINT16));
    UINT32 i;
    UINT32 uRet = 0;

    for (i = 0; i < count; i++) {
        if (DeviceSupportImgFormat[i] == Format) {
            uRet = 1;
            break;
        }
    }
    return uRet;
}

/* Operation code : GetObjectPropDesc 0x9802 */
static UINT32 CbObjectPropDescGet(ULONG Property, ULONG Format, UCHAR **Dataset, ULONG *DatasetLength)
{
    WCHAR empty_wchar[] = { 0, 0 };
    UINT32 uRet         = 0;

    AmbaMisra_TouchUnused(empty_wchar);

    if (IsObjectFormatSupported((UINT16)Format) == 0U) {
        AmbaPrint_PrintUInt5("CbObjectPropDescGet(): Object Format is not supported.", 0U, 0U, 0U, 0U, 0U);
        uRet = MTP_RC_INVALID_OBJECT_PROP_FORMAT;
    } else if (IsObjectPropertySupported((UINT16)Format, (UINT16)Property) == 0U) {
        AmbaPrint_PrintUInt5("CbObjectPropDescGet(): Property/Format are not supported.", 0U, 0U, 0U, 0U, 0U);
        uRet = MTP_RC_OBJECT_PROP_NOT_SUPPORTED;
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

            /* Isolate the property. That will determine the dataset header.  */
            switch (Property) {
            case MTP_OBJECT_PROP_STORAGEID:

                uRet = FillPropDesc_LongNoForm(
                           Dataset,
                           DatasetLength,
                           Property,
                           MTP_OBJ_PROP_VALUE_GET,
                           0,
                           MTP_GROUP_CODE);
                break;

            case MTP_OBJECT_PROP_OBJECT_FORMAT:

                uRet = FillPropDesc_ShortNoForm(
                           Dataset,
                           DatasetLength,
                           Property,
                           MTP_OBJ_PROP_VALUE_GET,
                           0,
                           MTP_GROUP_CODE);
                break;

            case MTP_OBJECT_PROP_PROTECTION_STATUS:
                uRet = FillPropDesc_ProtectionStatus(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_OBJECT_SIZE:
                uRet = FillPropDesc_ObjectSize(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_OBJECT_FILE_NAME:
                uRet = FillPropDesc_String(
                           Dataset,
                           DatasetLength,
                           Property,
                           MTP_OBJ_PROP_VALUE_GETSET,
                           MTP_GROUP_CODE,
                           empty_wchar);
                break;

            case MTP_OBJECT_PROP_PARENT_OBJECT:
                uRet = FillPropDesc_ParentObject(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER:
                uRet = FillPropDesc_PersistentUniqueObjectIdentifier(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_NAME:
                uRet = FillPropDesc_String(
                           Dataset,
                           DatasetLength,
                           Property,
                           MTP_OBJ_PROP_VALUE_GETSET,
                           MTP_GROUP_CODE,
                           empty_wchar);
                break;

            case MTP_OBJECT_PROP_NON_CONSUMABLE:
                uRet = FillPropDesc_NonConsumable(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_ARTIST:
                uRet = FillPropDesc_String(
                           Dataset,
                           DatasetLength,
                           Property,
                           MTP_OBJ_PROP_VALUE_GETSET,
                           MTP_GROUP_CODE,
                           empty_wchar);
                break;

            case MTP_OBJECT_PROP_TRACK:

                uRet = FillPropDesc_ShortNoForm(
                           Dataset,
                           DatasetLength,
                           Property,
                           MTP_OBJ_PROP_VALUE_GET,
                           0,
                           MTP_GROUP_CODE);
                break;

            case MTP_OBJECT_PROP_USE_COUNT:
                uRet = FillPropDesc_UseCount(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_DATE_AUTHORED:
                uRet = FillPropDesc_String(
                           Dataset,
                           DatasetLength,
                           Property,
                           MTP_OBJ_PROP_VALUE_GETSET,
                           MTP_GROUP_CODE,
                           empty_wchar);
                break;

            case MTP_OBJECT_PROP_GENRE:
                uRet = FillPropDesc_String(
                           Dataset,
                           DatasetLength,
                           Property,
                           MTP_OBJ_PROP_VALUE_GETSET,
                           MTP_GROUP_CODE,
                           empty_wchar);
                break;

            case MTP_OBJECT_PROP_ALBUM_NAME:
                uRet = FillPropDesc_String(
                           Dataset,
                           DatasetLength,
                           Property,
                           MTP_OBJ_PROP_VALUE_GETSET,
                           MTP_GROUP_CODE,
                           empty_wchar);
                break;

            case MTP_OBJECT_PROP_ALBUM_ARTIST:
                uRet = FillPropDesc_String(
                           Dataset,
                           DatasetLength,
                           Property,
                           MTP_OBJ_PROP_VALUE_GETSET,
                           MTP_GROUP_CODE,
                           empty_wchar);
                break;

            case MTP_OBJECT_PROP_SAMPLE_RATE:
                uRet = FillPropDesc_SampleRate(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_NUMBER_OF_CHANNELS:
                uRet = FillPropDesc_NumberOfChannels(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_AUDIO_WAVE_CODEC:
                uRet = FillPropDesc_AudioWaveCodec(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_AUDIO_BITRATE:
                uRet = FillPropDesc_AudioBitrate(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_DURATION:
                uRet = FillPropDesc_AudioDuration(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_WIDTH:
                uRet = FillPropDesc_Width(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_HEIGHT:
                uRet = FillPropDesc_Height(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_SCAN_TYPE:
                uRet = FillPropDesc_ScanType(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_VIDEO_FOURCC_CODEC:
                uRet = FillPropDesc_FourCCCodec(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_VIDEO_BITRATE:
                uRet = FillPropDesc_VideoBitrate(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_FRAMES_PER_THOUSAND_SECONDS:
                uRet = FillPropDesc_FramePerThousandSeconds(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_KEYFRAME_DISTANCE:
                uRet = FillPropDesc_KeyFrameDistance(Dataset, DatasetLength);
                break;

            case MTP_OBJECT_PROP_ENCODING_PROFILE:
                uRet = FillPropDesc_String(Dataset, DatasetLength, Property, MTP_OBJ_PROP_VALUE_GETSET, MTP_GROUP_CODE, empty_wchar);
                break;

            default:
                uRet = MTP_RC_INVALID_OBJECT_PROP_CODE;
                break;
            }
            break;
        default:
            /* We get here when we have the wrong format code.  */
            uRet = MTP_RC_INVALID_OBJECT_FORMAT_CODE;
            break;
        }
    }
    return uRet;
}

static UINT32 MTP_Utility_GetPascalStringSizeInBytes(const UINT8 *str)
{
    UINT32 size = str[0];

    size = ((size & 0x0FFU) * 2U) + 1U;
    return size;
}

static UINT32 ObjectHandleCheck(ULONG Handle)
{
    UINT32 status = MTP_RC_INVALID_OBJECT_HANDLE;

    if (DbIsObjectHandleValid(Handle) == 1U) {
        status = USB_ERR_SUCCESS;
    }

    return status;
}

/* Operation code : GetObjectPropValue 0x9803 */
static UINT32 CbObjectPropValueGet(ULONG Handle, ULONG Property, UCHAR **PropValue, ULONG *PropValueLength)
{
    UINT32 status = 0U;
    UINT8 *buf    = NULL;

    /* Check the object handle. It must be in the local array.  */
    status = ObjectHandleCheck(Handle);

    /* Does the object handle exist ?  */
    if (status == 0U) {
        buf = GetPropValueBuffer(APP_MTP_MAX_PROP_VALUE_BUFFER_SIZE);
        if (buf == NULL) {
            AmbaPrint_PrintUInt5("CbObjectPropValueGet(): can't get data buffer.", 0, 0, 0, 0, 0);
            status = MTP_RC_GENERAL_ERROR;
        } else {
            *PropValue = buf;

            /* Isolate the property. That will determine were we fetch the value.  We use the dataset storage area to build the value.  */
            switch (Property) {
            case MTP_OBJECT_PROP_STORAGEID:
                *PropValueLength = MTP_Utility_LongPut(buf, MtpClassInfo.DeviceInfo.StorageId);
                break;

            case MTP_OBJECT_PROP_OBJECT_FORMAT: {
                *PropValueLength = MTP_Utility_ShortPut(buf, DbGetFileFormat(Handle));
                break;
            }
            case MTP_OBJECT_PROP_PROTECTION_STATUS:
                *PropValueLength = MTP_Utility_ShortPut(buf, 0);
                break;

            case MTP_OBJECT_PROP_OBJECT_SIZE: {
                UINT64 FileSize;
                FileSize          = DbGetFileSize(Handle);
                *PropValueLength  = MTP_Utility_LongPut(buf, (UINT32)(FileSize & 0xFFFFFFFFU));
                *PropValueLength += MTP_Utility_LongPut(&buf[4], (UINT32)(FileSize >> 32U));
                break;
            }

            case MTP_OBJECT_PROP_OBJECT_FILE_NAME: {
                char FileName[64];

                if (DbGetFileNameFromHandle(Handle, FileName) != 0U) {
                    /* Store the file name in unicode format.  */
                    Asc2PascalString(FileName, buf);
                    /* Set the length.  First Unicode string data.  */
                    *PropValueLength = MTP_Utility_GetPascalStringSizeInBytes(buf);
                } else {
                    status = MTP_RC_ACCESS_ERROR;
                }
                break;
            }

            case MTP_OBJECT_PROP_PARENT_OBJECT:
                *PropValueLength = MTP_Utility_LongPut(buf, DbGetParentHandle(Handle));
                break;

            case MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER:
                *PropValueLength  = MTP_Utility_LongPut(buf, Handle);
                *PropValueLength += MTP_Utility_LongPut(&buf[4], 0);
                *PropValueLength += MTP_Utility_LongPut(&buf[8], 0);
                *PropValueLength += MTP_Utility_LongPut(&buf[12], 0);
                break;

            case MTP_OBJECT_PROP_NAME: {
                char FileName[64];

                if (DbGetFileNameFromHandle(Handle, FileName) != 0U) {
                    /* Store the file name in unicode format.  */
                    Asc2PascalString(FileName, buf);

                    /* Set the length.  First Unicode string data.  */
                    *PropValueLength = MTP_Utility_GetPascalStringSizeInBytes(buf);
                } else {
                    status = MTP_RC_ACCESS_ERROR;
                }

                break;
            }

            case  MTP_OBJECT_PROP_NON_CONSUMABLE:

                buf[0]           = 0;
                *PropValueLength = 1;
                break;

            case MTP_OBJECT_PROP_ARTIST:
                //Asc2PascalString(NULL, buf);
                //*PropValueLength = MTP_Utility_GetPascalStringSizeInBytes(buf);
                status = MTP_RC_INVALID_OBJECT_PROP_CODE;
                break;

            case MTP_OBJECT_PROP_TRACK:
                *PropValueLength = MTP_Utility_ShortPut(buf, 0);
                break;

            case MTP_OBJECT_PROP_USE_COUNT:
                *PropValueLength = MTP_Utility_ShortPut(buf, 0);
                break;

            case MTP_OBJECT_PROP_DATE_AUTHORED:
                //Asc2PascalString(NULL, buf);
                //*PropValueLength = MTP_Utility_GetPascalStringSizeInBytes(buf);
                status = MTP_RC_INVALID_OBJECT_PROP_CODE;
                break;

            case MTP_OBJECT_PROP_GENRE:
                //Asc2PascalString(0, buf);
                //*PropValueLength = MTP_Utility_GetPascalStringSizeInBytes(buf);
                status = MTP_RC_INVALID_OBJECT_PROP_CODE;
                break;

            case MTP_OBJECT_PROP_ALBUM_NAME:
                //Asc2PascalString(0, buf);
                //*PropValueLength = MTP_Utility_GetPascalStringSizeInBytes(buf);
                status = MTP_RC_INVALID_OBJECT_PROP_CODE;
                break;

            case MTP_OBJECT_PROP_ALBUM_ARTIST:
                //Asc2PascalString(0, buf);
                //*PropValueLength = MTP_Utility_GetPascalStringSizeInBytes(buf);
                status = MTP_RC_INVALID_OBJECT_PROP_CODE;
                break;

            case MTP_OBJECT_PROP_SAMPLE_RATE:
                *PropValueLength = MTP_Utility_ShortPut(buf, 0);
                status           = MTP_RC_INVALID_OBJECT_PROP_CODE;
                break;

            case MTP_OBJECT_PROP_NUMBER_OF_CHANNELS:
                *PropValueLength = MTP_Utility_ShortPut(buf, 0);
                break;

            case MTP_OBJECT_PROP_AUDIO_WAVE_CODEC:
                *PropValueLength = MTP_Utility_LongPut(buf, 0);
                break;

            case MTP_OBJECT_PROP_AUDIO_BITRATE:
                *PropValueLength = MTP_Utility_LongPut(buf, 0);
                break;

            case MTP_OBJECT_PROP_DURATION:
                *PropValueLength = MTP_Utility_LongPut(buf, 0);
                break;

            case MTP_OBJECT_PROP_WIDTH:
                *PropValueLength = MTP_Utility_LongPut(buf, DbGetObjectWidth(Handle));
                break;

            case MTP_OBJECT_PROP_HEIGHT:
                *PropValueLength = MTP_Utility_LongPut(buf, DbGetObjectHeight(Handle));
                break;

            case MTP_OBJECT_PROP_SCAN_TYPE:
                *PropValueLength = MTP_Utility_ShortPut(buf, 0);
                break;

            case MTP_OBJECT_PROP_VIDEO_FOURCC_CODEC:
                *PropValueLength = MTP_Utility_LongPut(buf, 0);
                break;

            case MTP_OBJECT_PROP_VIDEO_BITRATE:
                *PropValueLength = MTP_Utility_LongPut(buf, 0);
                break;

            case MTP_OBJECT_PROP_FRAMES_PER_THOUSAND_SECONDS:
                *PropValueLength = MTP_Utility_LongPut(buf, 0);
                break;

            case MTP_OBJECT_PROP_KEYFRAME_DISTANCE:
                *PropValueLength = MTP_Utility_LongPut(buf, 0);
                break;

            case MTP_OBJECT_PROP_ENCODING_PROFILE:

                //Asc2PascalString(0, buf);
                //*PropValueLength = MTP_Utility_GetPascalStringSizeInBytes(buf);
                status = MTP_RC_INVALID_OBJECT_PROP_CODE;
                break;

            default:
                status = MTP_RC_INVALID_OBJECT_PROP_CODE;
                break;
            }
        }
    } else {
        status = MTP_RC_INVALID_OBJECT_HANDLE;
    }
    return status;
}

static UINT32 IsOperationSupported(UINT16 Opcode)
{
    UINT32 count = (UINT32)(sizeof(OperationSupportList) / sizeof(UINT16));
    UINT32 i;
    UINT32 uRet = 0;

    for (i = 0; i < count; i++) {
        if (OperationSupportList[i] == Opcode) {
            uRet = 1;
            break;
        }
    }
    return uRet;
}

/* Operation code : SetObjectPropValue 0x9804 */
static UINT32 CbObjectPropValueSet(ULONG ObjectHandle, ULONG ObjectProperty, UCHAR *ObjectPropValue, ULONG ObjectPropValueLength)
{
    UINT32 status;

    AmbaMisra_TouchUnused(ObjectPropValue);
    AmbaMisra_TouchUnused(&ObjectPropValueLength);

    if (IsOperationSupported(MTP_OC_SET_OBJECT_PROP_VALUE) == 0U) {
        status = MTP_RC_OPERATION_NOT_SUPPORTED;
    } else {
        /* Check the object handle. It must be in the local array.  */
        status = ObjectHandleCheck(ObjectHandle);

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

static UINT32 CheckGroupCode(UINT32 group_code)
{
    UINT32 uRet;

    if (MTP_GROUP_CODE == group_code) {
        uRet = 1;
    } else {
        AmbaPrint_PrintUInt5("CheckGroupCode(): don't know how to handle group code 0x%X", group_code, 0, 0, 0, 0);
        uRet = 0;
    }
    return uRet;
}

static UINT32 FillPropListElemtShort(UINT8 *Buf, UINT32 Pos, ULONG Handle, ULONG Property, ULONG Value)
{
    UINT32  Idx = Pos;

    Idx += MTP_Utility_LongPut(&Buf[Idx], Handle);
    Idx += MTP_Utility_ShortPut(&Buf[Idx], Property);
    Idx += MTP_Utility_ShortPut(&Buf[Idx], MTP_TYPES_UINT16);
    Idx += MTP_Utility_ShortPut(&Buf[Idx], Value);

    return Idx;
}

static UINT32 FillPropListElemtLong(UINT8 *Buf, UINT32 Pos, ULONG Handle, ULONG Property, ULONG Value)
{
    UINT32  Idx = Pos;

    Idx += MTP_Utility_LongPut(&Buf[Idx], Handle);
    Idx += MTP_Utility_ShortPut(&Buf[Idx], Property);
    Idx += MTP_Utility_ShortPut(&Buf[Idx], MTP_TYPES_UINT32);
    Idx += MTP_Utility_LongPut(&Buf[Idx], Value);

    return Idx;
}

static UINT32 FillPropListElemtLLong(UINT8 *Buf, UINT32 Pos, ULONG Handle, ULONG Property, UINT64 Value)
{
    UINT32  Idx = Pos;

    Idx += MTP_Utility_LongPut(&Buf[Idx], Handle);
    Idx += MTP_Utility_ShortPut(&Buf[Idx], Property);
    Idx += MTP_Utility_ShortPut(&Buf[Idx], MTP_TYPES_UINT64);
    Idx += MTP_Utility_LongPut(&Buf[Idx], (UINT32)Value);
    Idx += MTP_Utility_LongPut(&Buf[Idx], (UINT32)(Value >> 32U));

    return Idx;
}

static UINT32 FillPropListElemtLLLL(UINT8 *Buf, UINT32 Pos, ULONG Handle, ULONG Property, ULONG Value1, ULONG Value2, ULONG Value3, ULONG Value4)
{
    UINT32  Idx = Pos;

    Idx += MTP_Utility_LongPut(&Buf[Idx], Handle);
    Idx += MTP_Utility_ShortPut(&Buf[Idx], Property);
    Idx += MTP_Utility_ShortPut(&Buf[Idx], MTP_TYPES_UINT128);
    Idx += MTP_Utility_LongPut(&Buf[Idx], Value1);
    Idx += MTP_Utility_LongPut(&Buf[Idx], Value2);
    Idx += MTP_Utility_LongPut(&Buf[Idx], Value3);
    Idx += MTP_Utility_LongPut(&Buf[Idx], Value4);
    return Idx;
}

static UINT32 FillPropListEmelemtString(UINT8 *Buf, UINT32 Pos, ULONG Handle, UINT16 Property, const char *AsciiString)
{
    UINT8   pascall_str[USBD_MTP_MAX_FILENAME_LEN * 2U];
    UINT32  len = 0, size = 0;
    UINT32  Idx = Pos;

    len = (UINT32)AmbaUtility_StringLength(AsciiString);
    size = (len + 3U) * 2U;

    if (size > (UINT32)sizeof(pascall_str)) {
        AmbaPrint_PrintUInt5("FillPropListEmelemtString(): string size too big.", 0, 0, 0, 0, 0);
    } else {
        Idx += MTP_Utility_LongPut(&Buf[Idx], Handle);
        Idx += MTP_Utility_ShortPut(&Buf[Idx], Property);
        Idx += MTP_Utility_ShortPut(&Buf[Idx], MTP_TYPES_STR);

        Asc2PascalString(AsciiString, pascall_str);
        size = MTP_Utility_GetPascalStringSizeInBytes(pascall_str);

        if ((size <= (UINT32)sizeof(pascall_str)) && (size < APP_MTP_MAX_PROP_LIST_BUFFER_SIZE)) {
            AmbaSvcWrap_MisraMemcpy(&Buf[Idx], pascall_str, (SIZE_t)size);
            Idx += size;
        } else {
            AmbaPrint_PrintUInt5("FillPropListEmelemtString(): pascall_str too big.", 0, 0, 0, 0, 0);
        }
    }

    return Idx;
}

static UINT16 *GetObjectPropList(UINT16 Format)
{
    UINT16 *ptr = ObjectSupportProp;
    UINT16 count;
    UINT32 idx   = 0;
    UINT16 *pRet = NULL;

    while (ptr[idx] != 0U) {
        if (ptr[idx] == Format) {
            pRet = &ptr[idx];
            break;
        } else {
            idx++;
            count = ptr[idx];
            idx   = idx + ((UINT32)count + 1U);
        }
    }
    return pRet;
}

static UINT32 FillPropList(const ULONG *params, UINT8 *Buf, ULONG *Length, UINT32 Header, UINT32 *ElementNumber)
{
    ULONG Handle      = params[0];
    ULONG format_code = params[1];
    ULONG prop_code   = params[2];
    ULONG group_code  = params[3];
    const UINT16 *prop_list;
    UINT16 prop_count = 0;
    UINT32 i;
    UINT32 pos              = 0;
    UINT16 valid_prop_code  = 0;
    UINT16 check_group_code = 0;
    UINT16 cur_prop_code;
    UINT32 valid_number = *ElementNumber;
    UINT32 uRet         = MTP_RC_OK;
    UINT32 is_break     = 0;

    if (format_code == 0x0U) {
        // all ObjectFormats are desired
        format_code = (ULONG)DbGetFileFormat(Handle);
    }

    prop_list = GetObjectPropList((UINT16)format_code);
    if (prop_list == NULL) {
        AmbaPrint_PrintUInt5("FillPropList(): property list is NULL", 0U, 0U, 0U, 0U, 0U);
        uRet = MTP_RC_INVALID_DEVICE_PROP_FORMAT;
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
                    uRet     = MTP_RC_PARAMETER_NOT_SUPPORTED;
                    is_break = 1;
                    continue;
                } else {
                    valid_prop_code  = cur_prop_code;
                    check_group_code = 1;
                }
            } else if (prop_code == 0xFFFFFFFFU) {
                valid_prop_code = cur_prop_code;
            } else if (prop_code == (ULONG)cur_prop_code) {
                valid_prop_code = cur_prop_code;
            } else {
                // pass vcast checking
            }

            if (valid_prop_code != cur_prop_code) { //jump over this property code
                continue;
            } else {
                // pass vcast checking.
            }

            //dbg("%s(): DataCode = 0x%X", __func__, cur_prop_code);
            if ((CheckGroupCode((UINT32)group_code) == 0U) && (check_group_code != 0U)) {
                continue;
            }

            switch (cur_prop_code) {
            case MTP_OBJECT_PROP_STORAGEID: {
                valid_number++;
                pos = FillPropListElemtLong(
                          Buf, pos, Handle, cur_prop_code, MtpClassInfo.DeviceInfo.StorageId);
                break;
            }
            case MTP_OBJECT_PROP_OBJECT_FORMAT: {
                valid_number++;
                pos = FillPropListElemtShort(
                          Buf, pos, Handle, cur_prop_code, format_code);
                break;
            }

            case MTP_OBJECT_PROP_PROTECTION_STATUS: {
                valid_number++;
                pos = FillPropListElemtShort(
                          Buf, pos, Handle, cur_prop_code, 0);
                break;
            }

            case MTP_OBJECT_PROP_OBJECT_SIZE: {
                UINT64 size;
                valid_number++;
                size = DbGetFileSize(Handle);
                pos  = FillPropListElemtLLong(
                           Buf, pos, Handle, cur_prop_code, size);
                break;
            }
            case MTP_OBJECT_PROP_WIDTH: {
                UINT32 width;
                width = DbGetObjectWidth(Handle);
                valid_number++;
                pos = FillPropListElemtLong(
                          Buf, pos, Handle, cur_prop_code, width);
                break;
            }
            case MTP_OBJECT_PROP_HEIGHT: {
                UINT32 height;
                height = DbGetObjectHeight(Handle);
                valid_number++;
                pos = FillPropListElemtLong(
                          Buf, pos, Handle, cur_prop_code, height);
                break;
            }

            case MTP_OBJECT_PROP_PARENT_OBJECT: {
                UINT32 parent_handle;
                parent_handle = DbGetParentHandle(Handle);
                valid_number++;
                pos = FillPropListElemtLong(
                          Buf, pos, Handle, cur_prop_code, parent_handle);
                break;
            }
            case MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER: {
                valid_number++;
                pos = FillPropListElemtLLLL(
                          Buf, pos, Handle, cur_prop_code, Handle, 0, 0, 0);
                break;
            }
            case MTP_OBJECT_PROP_OBJECT_FILE_NAME: {
                char name[64];
                UINT32 rval = DbGetFileNameFromHandle(Handle, name);
                if (rval != 0U) {
                    AmbaPrint_PrintUInt5("FillPropList() : can't get filename", 0U, 0U, 0U, 0U, 0U);
                } else {
                    valid_number++;
                    pos = FillPropListEmelemtString(
                              Buf, pos, Handle, cur_prop_code, name);
                }
                break;
            }
            case MTP_OBJECT_PROP_DATE_AUTHORED: {
                valid_number++;
                pos = FillPropListEmelemtString(
                          Buf, pos, Handle, cur_prop_code, "20140813T101430");
                break;
            }
            default:
                AmbaPrint_PrintUInt5("FillPropList() : unsupported object proerpty code = 0x%x", cur_prop_code, 0, 0, 0, 0);
                uRet     = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;
                is_break = 1;
                break;
            }
        }

        if (uRet == MTP_RC_OK) {
            *ElementNumber = valid_number;
            *Length        = pos;
            //dbg("%s(): length = %d, number = %d", __func__, *Length, *ElementNumber);
        }
    }

    return uRet;
}

/* If the value of the depth is 0x0 and objectHandle is also 0x0, the responder shall return an empty set */
static void FillPropListEmpty(UINT8 *Buf, ULONG *Length, UINT32 *ElementNumber)
{
    *Length        = 4;
    *ElementNumber = 0;

    Buf[0] = 0;
    Buf[1] = 0;
    Buf[2] = 0;
    Buf[3] = 0;
    return;
}

static UINT32 FillPropListAll(ULONG *Params, UINT8 *Buf, ULONG *Length, UINT32 *ElementNumber)
{
    UINT32 total_count = DbGetTotalHandleNumber();
    UINT32 count       = 0;
    ULONG  *Handles;
    UINT32 local_element_number = 0;
    UINT32 uRet                 = MTP_RC_OK;
    ULONG local_length          = 0;
    UINT32 header               = 1;
    UINT32 is_break             = 0;
    UINT32 target_size          = (total_count + 1U) * 4U;
    static ULONG BufferAllHandles[MTP_MAX_HANDLES + 1U] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    UINT8 *tmp_buf = Buf;

    Handles = BufferAllHandles;
    if (target_size > (UINT32)sizeof(BufferAllHandles)) {
        uRet = MTP_RC_ACCESS_ERROR;
    } else {
        if (DbFillObjectHandles(0x0, 0x0, Handles) == 0U) {
            *Length = 0;

            for (count = 0; count < total_count; count++) {
                if (is_break == 1U) {
                    break;
                }

                local_element_number = 0;
                local_length         = 0;
                Params[0]            = Handles[count + 1U];
                uRet                 = FillPropList(Params, tmp_buf, &local_length, header, &local_element_number);
                if (uRet != MTP_RC_OK) {
                    AmbaPrint_PrintUInt5("FillPropListAll(): can't Fill PropList for Handle %d", count, 0, 0, 0, 0);
                    is_break = 1;
                    continue;
                }

                header = 0;

                *Length        += local_length;
                *ElementNumber += local_element_number;
                tmp_buf         = &Buf[local_length];

                if (APP_MTP_MAX_PROP_LIST_BUFFER_SIZE < *Length) {
                    AmbaPrint_PrintUInt5("FillPropListAll(): Length > %d.", APP_MTP_MAX_PROP_LIST_BUFFER_SIZE, 0U, 0U, 0U, 0U);
                    uRet     = MTP_RC_GENERAL_ERROR;
                    is_break = 1;
                    continue;
                }
            }
        } else {
            uRet = MTP_RC_ACCESS_ERROR;
        }
    }

    return uRet;
}

/* Operation code : GetObjectPropList 0x9805 */
static UINT32 CbObjectPropListGet(ULONG *Params, UCHAR **PropList, ULONG *PropListLength)
{
    UINT8 *buf;
    UINT32 handle         = (UINT32)Params[0];
    UINT32 depth          = (UINT32)Params[4];
    ULONG length          = 0;
    UINT32 element_number = 0;
    UINT32 uRet           = MTP_RC_OK;

    if (IsOperationSupported(MTP_OC_GET_OBJECT_PROP_LIST) == 0U) {
        uRet = MTP_RC_OPERATION_NOT_SUPPORTED;
    } else {
        //dbg("get_obj_prop_list(): Handle = 0x%X, format 0x%X, property 0x%X, group 0x%X, depth = 0x%X", handle, format_code, property_code, group_code, depth);

        if ((handle != 0x0U) && (handle != 0xFFFFFFFFU)) {
            if (ObjectHandleCheck(handle) != 0U) {
                AmbaPrint_PrintUInt5("CbObjectPropListGet(): Invalid Handle 0x%X", handle, 0U, 0U, 0U, 0U);
                uRet = MTP_RC_INVALID_OBJECT_HANDLE;
            }
        }

        if (uRet == MTP_RC_OK) {
            buf = GetPropListBuffer(APP_MTP_MAX_PROP_LIST_BUFFER_SIZE);
            if (buf == NULL) {
                AmbaPrint_PrintUInt5("CbObjectPropListGet(): can't allocate data buffer.", 0U, 0U, 0U, 0U, 0U);
                uRet = MTP_RC_GENERAL_ERROR;
            } else {
                switch (depth) {
                case 0:
                    switch (handle) {
                    case 0U:
                        FillPropListEmpty(buf, &length, &element_number);
                        *PropList       = buf;
                        *PropListLength = length;
                        if (0U < MTP_Utility_LongPut(buf, element_number)) {
                            /* do nothing */
                        }
                        break;
                    case 0xFFFFFFFFU:
                        uRet            = FillPropListAll(Params, buf, &length, &element_number);
                        *PropList       = buf;
                        *PropListLength = length;
                        if (0U < MTP_Utility_LongPut(buf, element_number)) {
                            /* do nothing */
                        }
                        break;
                    default:
                        uRet            = FillPropList(Params, buf, &length, 1, &element_number);
                        *PropList       = buf;
                        *PropListLength = length;
                        if (0U < MTP_Utility_LongPut(buf, element_number)) {
                            /* do nothing */
                        }
                        break;
                    }
                    break;
                case 0xFFFFFFFFU:
                default:
                    AmbaPrint_PrintUInt5("CbObjectPropListGet(): Not supported depth 0x%X.", depth, 0U, 0U, 0U, 0U);
                    uRet = MTP_RC_SPECIFICATION_BY_DEPTH_UNSUPPORTED;
                    break;
                }

                if (uRet == MTP_RC_OK) {
                    if (length > APP_MTP_MAX_PROP_LIST_BUFFER_SIZE) {
                        AmbaPrint_PrintUInt5("CbObjectPropListGet(): Length > %d.", APP_MTP_MAX_PROP_LIST_BUFFER_SIZE, 0U, 0U, 0U, 0U);
                        uRet = MTP_RC_GENERAL_ERROR;
                    }
                }
            }
        }
    }

    return uRet;
}

#endif /* COMSVC_MTP_OPROP_H */
