/**
 *  @file AmbaUSBD_Mtp.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details USB wrapper functions for MTP Device Class
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <usbx/ux_device_stack.h>
#include <AmbaUSBD_Mtp.h>
#include <AmbaUSB_Utility.h>
#include <AmbaUSB_ErrCode.h>

typedef struct {
    USBD_MTP_DEVICE_INFO_s DeviceInfo;
} MTP_CLASS_SYSTEM_INFO_s;

static MTP_CLASS_SYSTEM_INFO_s mtp_sys_info = { 0 };

static UINT object_delete_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima, ULONG ObjectHandle)
{
    UINT32 uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;

    if (mtp_sys_info.DeviceInfo.ObjectDelete != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectDelete(ObjectHandle);
    }

    return (uret);
}

static UINT device_reset_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima)
{
    UINT32 uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    uret = UX_SUCCESS;

    if (mtp_sys_info.DeviceInfo.DeviceReset != NULL) {
        uret = mtp_sys_info.DeviceInfo.DeviceReset();
    }

    return (uret);
}

static UINT microsoft_vendor_request_func(ULONG  Request,
        ULONG  RequestValue,
        ULONG  RequestIndex,
        ULONG  RequestLength,
        UCHAR *TransferRequestBuffer,
        ULONG *TransferRequestLength)
{
    UINT32 uret;
    UINT32 i;

    // pass misra-c
    (void)RequestValue;
    (void)RequestLength;

    /* Do some sanity check.  The request must be our vendor request. */
    if (Request != UX_DEMO_VENDOR_REQUEST) {
        uret = UX_ERROR;
    } else {
        /* Check the wIndex value. Values can be :
            0x0001 : Genre
            0x0004 : Extended compatible ID
            0x0005 : Extended properties */
        switch (RequestIndex) {
        case    0x0001:

            /* Not sure what this is for. Windows does not seem to request this. Drop it.  */
            uret = UX_ERROR;
            break;

        case    0x0004:
        case    0x0005:

            /* Build the descriptor to be returned.  This is not a composite descriptor. Single MTP.
               First dword is length of the descriptor.  */
            _ux_utility_long_put(TransferRequestBuffer, 0x0028);

            /* Then the version. fixed to 0x0100.  */
            _ux_utility_short_put(&TransferRequestBuffer[4], 0x0100);

            /* Then the descriptor ID. Fixed to 0x0004.  */
            _ux_utility_short_put(&TransferRequestBuffer[6], 0x0004);

            /* Then the bcount field. Fixed to 0x0001.  */
            TransferRequestBuffer[8] = 0x01;

            /* Reset the next 7 bytes.  */
            _ux_utility_memory_set(&TransferRequestBuffer[9], 0x00, 7);

            /* Last byte of header is the interface number, here 0.  */
            TransferRequestBuffer[16] = 0x00;

            /* First byte of descriptor is set to 1.  */
            TransferRequestBuffer[17] = 0x01;

            /* Reset the next 8 + 8 + 6 bytes.  */
            for (i = 0U; i < (22U); i++) {
                TransferRequestBuffer[18U + i] = 0U;
            }

            /* Set the compatible ID to MTP.  */
            TransferRequestBuffer[18] = 0x4D; // M
            TransferRequestBuffer[19] = 0x54; // T
            TransferRequestBuffer[20] = 0x50; // P

            /* Return the length.  */
            *TransferRequestLength = 0x28;

            /* We are done here.  */
            uret = UX_SUCCESS;
            break;

        default:
            uret = UX_ERROR;
            break;
        }
    }

    return (uret);
}

static UINT prop_desc_get_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                               ULONG                              DeviceProperty,
                               UCHAR **                           DevicePropDataset,
                               ULONG *                            DevicePropDtasetLength)
{
    UINT32 uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    /* We assume the worst.  */
    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;

    if (mtp_sys_info.DeviceInfo.PropDescGet != NULL) {
        uret = mtp_sys_info.DeviceInfo.PropDescGet(DeviceProperty, DevicePropDataset, DevicePropDtasetLength);
    }

    return (uret);
}

static UINT prop_value_get_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                ULONG                              DeviceProperty,
                                UCHAR **                           DevicePorpValue,
                                ULONG *                            DevicePropValueLength)
{
    UINT32 uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    /* We assume the worst.  */
    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;

    if (mtp_sys_info.DeviceInfo.PropValueGet != NULL) {
        uret = mtp_sys_info.DeviceInfo.PropValueGet(DeviceProperty, DevicePorpValue, DevicePropValueLength);
    }

    return (uret);
}

static UINT prop_value_set_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                ULONG                              DeviceProperty,
                                const UCHAR *                      DevicePropValue,
                                ULONG                              DevicePropValueLength)
{
    UINT32 uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    /* We assume the worst.  */
    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;

    if (mtp_sys_info.DeviceInfo.PropValueSet != NULL) {
        uret = mtp_sys_info.DeviceInfo.PropValueSet(DeviceProperty, DevicePropValue, DevicePropValueLength);
    }

    return (uret);
}

static UINT prop_value_reset_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima, ULONG DeviceProperty)
{
    UINT32 uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    /* We assume the worst.  */
    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;

    if (mtp_sys_info.DeviceInfo.PropValueReset != NULL) {
        uret = mtp_sys_info.DeviceInfo.PropValueReset(DeviceProperty);
    }

    return (uret);
}

static UINT storage_format_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima, ULONG StorageId)
{
    UINT uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;

    if (mtp_sys_info.DeviceInfo.StorageFormat != NULL) {
        uret = mtp_sys_info.DeviceInfo.StorageFormat(StorageId);
    }

    return uret;
}


static UINT storage_info_get_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima, ULONG StorageId)
{
    UINT32 uret;
    ULONG max_capacity_low;
    ULONG max_capacity_high;
    ULONG free_space_low;
    ULONG free_space_high;

    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;

    if (mtp_sys_info.DeviceInfo.StorageInfoGet != NULL) {
        uret = mtp_sys_info.DeviceInfo.StorageInfoGet(StorageId, &max_capacity_low, &max_capacity_high,
                &free_space_low, &free_space_high);

        Pima->storage_max_capacity_low  = max_capacity_low;
        Pima->storage_max_capacity_high = max_capacity_high;
        Pima->storage_free_space_low    = free_space_low;
        Pima->storage_free_space_high   = free_space_high;
    }

    return uret;
}

static UINT object_number_get_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                   ULONG                              ObjectFormatCode,
                                   ULONG                              ObjectAssociation,
                                   ULONG *                            ObjectNumber)
{
    UINT uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;

    if (mtp_sys_info.DeviceInfo.ObjectNumberGet != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectNumberGet(ObjectFormatCode, ObjectAssociation, ObjectNumber);
    }

    return uret;
}

static UINT object_handles_get_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                    ULONG                              FormatCode,
                                    ULONG                              Association,
                                    ULONG **                           HandlesArray,
                                    ULONG *                            HandlesNumber)
{
    UINT32 uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;

    if (mtp_sys_info.DeviceInfo.ObjectHandlesGet != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectHandlesGet(FormatCode, Association,
                HandlesArray, HandlesNumber);
    }

    return uret;
}

static void usbd_object_to_ux_object(const USBD_MTP_OBJECT_s *SrcObject, UX_SLAVE_CLASS_PIMA_OBJECT *DestObject)
{
    DestObject->storage_id            = SrcObject->StorageId;
    DestObject->format                = SrcObject->Format;
    DestObject->protection_status     = SrcObject->ProtectionStatus;
    DestObject->compressed_size       = SrcObject->CompressedSize;
    DestObject->thumb_format          = SrcObject->ThumbFormat;
    DestObject->thumb_compressed_size = SrcObject->ThumbCompressedSize;
    DestObject->thumb_pix_width       = SrcObject->ThumbPixWidth;
    DestObject->thumb_pix_height      = SrcObject->ThumbPixHeight;
    DestObject->image_pix_width       = SrcObject->ImagePixWidth;
    DestObject->image_pix_height      = SrcObject->ImagePixHeight;
    DestObject->image_bit_depth       = SrcObject->ImageBitDepth;
    DestObject->parent_object         = SrcObject->ParentObject;
    DestObject->association_type      = SrcObject->AssociationType;
    DestObject->association_desc      = SrcObject->AssociationDesc;
    DestObject->sequence_number       = SrcObject->SequenceNumber;
    DestObject->state                 = SrcObject->State;
    DestObject->offset                = SrcObject->Offset;
    DestObject->transfer_status       = SrcObject->TransferStatus;
    DestObject->handle_id             = SrcObject->HandleId;
    // fix bug: length should be from ObjectCompressedSize.
    // Otherwise pima class might get wrong object size since ObjectLength may not be set by application.
    DestObject->length                = SrcObject->CompressedSize;
    DestObject->buffer                = SrcObject->pBuffer;
    USB_UtilityMemcpyUInt8(DestObject->filename,
                           SrcObject->Filename,
                           USBD_MTP_MAX_UNICODE_STR_LEN);
    USB_UtilityMemcpyUInt8(DestObject->capture_date,
                           SrcObject->CaptureDate,
                           UX_DEVICE_CLASS_PIMA_DATE_TIME_STRING_MAX_LENGTH);
    USB_UtilityMemcpyUInt8(DestObject->modification_date,
                           SrcObject->ModificationDate,
                           UX_DEVICE_CLASS_PIMA_DATE_TIME_STRING_MAX_LENGTH);
    USB_UtilityMemcpyUInt8(DestObject->keywords,
                           SrcObject->Keywords,
                           UX_DEVICE_CLASS_PIMA_UNICODE_MAX_LENGTH);
}

static UINT object_info_get_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                 ULONG                              ObjectHandle,
                                 UX_SLAVE_CLASS_PIMA_OBJECT **      Object)
{
    UINT uret;
    USBD_MTP_OBJECT_s *usbd_mtp_object = NULL;
    static UX_SLAVE_CLASS_PIMA_OBJECT ux_mtp_object __attribute__((section(".bss.noinit")));

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    USB_UtilityMemorySet(&ux_mtp_object, 0, sizeof(ux_mtp_object));

    *Object = &ux_mtp_object;

    uret = UX_DEVICE_CLASS_PIMA_RC_OPERATION_NOT_SUPPORTED;
    if (mtp_sys_info.DeviceInfo.ObjectInfoGet != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectInfoGet(ObjectHandle, &usbd_mtp_object);
        if ((uret == (UINT)UX_DEVICE_CLASS_PIMA_RC_OK) || (uret == (UINT)UX_SUCCESS)) {
            // to pass misra-c, no pointer type conversion. Do object copy here.
            if (usbd_mtp_object != NULL) {
                usbd_object_to_ux_object(usbd_mtp_object, &ux_mtp_object);
            }
        }
    }

    return uret;
}

static void transfer_canceled_func(void)
{
    mtp_sys_info.DeviceInfo.TransferCanceled();
}

static UINT initiate_capture_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                  ULONG                              StorageID,
                                  ULONG                              ObjectFormatCode)
{
    // pass misra-c
    AmbaMisra_TouchUnused(Pima);
    (void)StorageID;
    (void)ObjectFormatCode;

    return UX_DEVICE_CLASS_PIMA_RC_OPERATION_NOT_SUPPORTED;
}

static UINT open_capture_initiate_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                       ULONG                              StorageID,
                                       ULONG                              ObjectFormatCode)
{
    // pass misra-c
    AmbaMisra_TouchUnused(Pima);
    (void)StorageID;
    (void)ObjectFormatCode;

    return UX_DEVICE_CLASS_PIMA_RC_OPERATION_NOT_SUPPORTED;
}

static UINT open_capture_terminate_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                        ULONG                              TransactionID)
{
    // pass misra-c
    AmbaMisra_TouchUnused(Pima);
    (void)TransactionID;

    return UX_DEVICE_CLASS_PIMA_RC_OPERATION_NOT_SUPPORTED;
}

static UINT object_thumb_get_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                  ULONG                              ObjectHandle,
                                  UCHAR *                            ObjectBuffer,
                                  ULONG                              ObjectOffset,
                                  ULONG                              ObjectLengthRequested,
                                  ULONG *                            ObjectActualLength)
{
    UINT32 uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;
    if (mtp_sys_info.DeviceInfo.ObjectThumbGet != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectThumbGet(ObjectHandle, ObjectBuffer, ObjectOffset,
                ObjectLengthRequested, ObjectActualLength);
    }

    return uret;
}

static UINT object_data_get_ex_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                    ULONG                              ObjectHandle,
                                    UCHAR *                            ObjectBuffer,
                                    UINT64                             ObjectOffset,
                                    UINT64                             ObjectLengthRequested,
                                    UINT64 *                           ObjectActualLength,
                                    USHORT                             OpCode)
{
    UINT32 uret = UX_ERROR;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    if (mtp_sys_info.DeviceInfo.ObjectDataGet != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectDataGet(
                   ObjectHandle,
                   ObjectBuffer,
                   ObjectOffset,
                   ObjectLengthRequested,
                   ObjectActualLength,
                   OpCode);
    }

    return uret;
}

static void ux_object_to_usbd_object(const UX_SLAVE_CLASS_PIMA_OBJECT *SrcObject, USBD_MTP_OBJECT_s *DestObject)
{
    DestObject->StorageId           = SrcObject->storage_id;
    DestObject->Format              = SrcObject->format;
    DestObject->ProtectionStatus    = SrcObject->protection_status;
    DestObject->CompressedSize      = SrcObject->compressed_size;
    DestObject->ThumbFormat         = SrcObject->thumb_format;
    DestObject->ThumbCompressedSize = SrcObject->thumb_compressed_size;
    DestObject->ThumbPixWidth       = SrcObject->thumb_pix_width;
    DestObject->ThumbPixHeight      = SrcObject->thumb_pix_height;
    DestObject->ImagePixWidth       = SrcObject->image_pix_width;
    DestObject->ImagePixHeight      = SrcObject->image_pix_height;
    DestObject->ImageBitDepth       = SrcObject->image_bit_depth;
    DestObject->ParentObject        = SrcObject->parent_object;
    DestObject->AssociationType     = SrcObject->association_type;
    DestObject->AssociationDesc     = SrcObject->association_desc;
    DestObject->SequenceNumber      = SrcObject->sequence_number;
    USB_UtilityMemcpyUInt8(DestObject->Filename,
                           SrcObject->filename,
                           USBD_MTP_MAX_UNICODE_STR_LEN);
    USB_UtilityMemcpyUInt8(DestObject->CaptureDate,
                           SrcObject->capture_date,
                           UX_DEVICE_CLASS_PIMA_DATE_TIME_STRING_MAX_LENGTH);
    USB_UtilityMemcpyUInt8(DestObject->ModificationDate,
                           SrcObject->modification_date,
                           UX_DEVICE_CLASS_PIMA_DATE_TIME_STRING_MAX_LENGTH);
    USB_UtilityMemcpyUInt8(DestObject->Keywords,
                           SrcObject->keywords,
                           UX_DEVICE_CLASS_PIMA_UNICODE_MAX_LENGTH);
    DestObject->State          = SrcObject->state;
    DestObject->Offset         = SrcObject->offset;
    DestObject->TransferStatus = SrcObject->transfer_status;
    DestObject->HandleId       = SrcObject->handle_id;
    DestObject->Length         = SrcObject->length;
    DestObject->pBuffer        = SrcObject->buffer;
}

static UINT object_info_send_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                  const  UX_SLAVE_CLASS_PIMA_OBJECT *Object,
                                  ULONG                              StorageId,
                                  ULONG                              ParentObjectHandle,
                                  ULONG *                            ObjectHandle)
{
    UINT uret;
    USBD_MTP_OBJECT_s usbd_mtp_object;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    uret = UX_DEVICE_CLASS_PIMA_RC_OPERATION_NOT_SUPPORTED;
    if (mtp_sys_info.DeviceInfo.ObjectInfoSend != NULL) {
        // to pass misra-c, no pointer type conversion. Do object copy here.
        // convert UX MTP object to USBD object
        ux_object_to_usbd_object(Object, &usbd_mtp_object);
        uret = mtp_sys_info.DeviceInfo.ObjectInfoSend(&usbd_mtp_object, StorageId,
                ParentObjectHandle, ObjectHandle);
    }

    return uret;
}

static UINT object_data_send_ex_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                     ULONG                              ObjectHandle,
                                     ULONG                              Phase,
                                     const UCHAR *                      ObjectBuffer,
                                     UINT64                             ObjectOffset,
                                     UINT64                             ObjectLength)
{
    UINT uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    uret = UX_DEVICE_CLASS_PIMA_RC_OPERATION_NOT_SUPPORTED;
    if (mtp_sys_info.DeviceInfo.ObjectDataSend != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectDataSend(ObjectHandle, Phase, ObjectBuffer,
                ObjectOffset, ObjectLength);
    }

    return uret;
}

static UINT object_prop_desc_get_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                      ULONG                              ObjectProperty,
                                      ULONG                              ObjectFormatCode,
                                      UCHAR **                           ObjectPropDataset,
                                      ULONG *                            ObjectPropDatasetLength)
{
    UINT uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);


    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;
    if (mtp_sys_info.DeviceInfo.ObjectPropDescGet != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectPropDescGet(ObjectProperty, ObjectFormatCode,
                ObjectPropDataset, ObjectPropDatasetLength);
    }

    return uret;
}

static UINT object_prop_list_get_func(void *Pima, ULONG *params, UCHAR **PropList, ULONG *PropListLength)
{
    UINT uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;

    if (mtp_sys_info.DeviceInfo.ObjectPropListGet != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectPropListGet(
                   params,
                   PropList,
                   PropListLength);
    }

    return uret;
}

static UINT object_prop_value_get_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                       ULONG                              ObjectHandle,
                                       ULONG                              ObjectProperty,
                                       UCHAR **                           ObjectPropValue,
                                       ULONG *                            ObjectPropValueLength)
{
    UINT uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);
    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;
    if (mtp_sys_info.DeviceInfo.ObjectPropValueGet != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectPropValueGet(ObjectHandle, ObjectProperty,
                ObjectPropValue, ObjectPropValueLength);
    }

    return uret;
}

static UINT object_prop_value_set_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                       ULONG                              ObjectHandle,
                                       ULONG                              ObjectProperty,
                                       UCHAR *                            ObjectPropValue,
                                       ULONG                              ObjectPropValueLength)
{
    UINT uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;
    if (mtp_sys_info.DeviceInfo.ObjectPropValueSet != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectPropValueSet(ObjectHandle, ObjectProperty,
                ObjectPropValue, ObjectPropValueLength);
    }

    return uret;
}

static UINT object_reference_get_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                      ULONG                              ObjectHandle,
                                      UCHAR **                           ObjectReferenceArray,
                                      ULONG *                            ObjectReferenceArrayLength)
{
    UINT uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;
    if (mtp_sys_info.DeviceInfo.ObjectReferenceGet != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectReferenceGet(ObjectHandle, ObjectReferenceArray, ObjectReferenceArrayLength);
    }

    return uret;
}

static UINT object_reference_set_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                      ULONG                              ObjectHandle,
                                      UCHAR *                            ObjectReferenceArray,
                                      ULONG                              ObjectReferenceArrayLength)
{
    UINT uret;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    uret = UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED;
    if (mtp_sys_info.DeviceInfo.ObjectReferenceSet != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectReferenceSet(ObjectHandle, ObjectReferenceArray, ObjectReferenceArrayLength);
    }

    return uret;
}

static UINT custom_command_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima,
                                ULONG Parameter1, ULONG Parameter2, ULONG Parameter3, ULONG Parameter4, ULONG Parameter5, ULONG* Length, ULONG* Direction)
{
    UINT uret = UX_SUCCESS;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    if (mtp_sys_info.DeviceInfo.ObjectCustomCommand != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectCustomCommand(Parameter1, Parameter2, Parameter3,
                Parameter4, Parameter5, Length, Direction);
    }

    return uret;
}

static UINT custom_data_get_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima, UCHAR *ObjectBuffer, ULONG ObjectOffset, ULONG ObjectLengthRequested, ULONG *ObjectActualLength)
{
    UINT uret = UX_SUCCESS;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);

    if (mtp_sys_info.DeviceInfo.ObjectCustomDataGet != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectCustomDataGet(ObjectBuffer, ObjectOffset, ObjectLengthRequested, ObjectActualLength);
    }

    return uret;
}

static UINT custom_data_send_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima, UCHAR *ObjectBuffer, ULONG ObjectOffset, ULONG ObjectLength)
{
    UINT uret = UX_SUCCESS;

    // pass misra-c
    AmbaMisra_TouchUnused(Pima);
    (void)ObjectBuffer;

    if (mtp_sys_info.DeviceInfo.ObjectCustomDataSend != NULL) {
        uret = mtp_sys_info.DeviceInfo.ObjectCustomDataSend(ObjectBuffer, ObjectOffset, ObjectLength);
    }

    return uret;
}

static UINT vendor_process_func(struct UX_SLAVE_CLASS_PIMA_STRUCT *Pima, UX_SLAVE_CLASS_PIMA_VENDOR_PROCESS_INFO *vendor_cmd_info)
{
    // pass misra-c
    AmbaMisra_TouchUnused(Pima);
    AmbaMisra_TouchUnused(vendor_cmd_info);
    // we don't support vendor commands
    return UX_ERROR;
}


static UINT32 check_null_for_device_info(const USBD_MTP_DEVICE_INFO_s *DeviceInfo)
{
    UINT32 uret = USB_ERR_SUCCESS;

    if (DeviceInfo == NULL) {
        uret = USB_ERR_PARAMETER_NULL;
    } else if ((DeviceInfo->DeviceInfoVendorName == NULL) ||
               (DeviceInfo->DeviceInfoProductName == NULL) ||
               (DeviceInfo->DeviceInfoSerialNo == NULL) ||
               (DeviceInfo->DeviceInfoVersion == NULL) ||
               (DeviceInfo->VolumeDescription == NULL) ||
               (DeviceInfo->VolumeLabel == NULL) ||
               (DeviceInfo->DeviceSupportProp == NULL) ||
               (DeviceInfo->DeviceSupportCaptureFormat == NULL) ||
               (DeviceInfo->DeviceSupportImgFormat == NULL) ||
               (DeviceInfo->ObjectSupportProp == NULL) ||
               (DeviceInfo->OperationSupportList == NULL) ||
               (DeviceInfo->EventSupportList == NULL) ||
               (DeviceInfo->PropDescGet == NULL) ||
               (DeviceInfo->PropValueGet == NULL) ||
               (DeviceInfo->PropValueSet == NULL) ||
               (DeviceInfo->PropValueReset == NULL) ||
               (DeviceInfo->StorageFormat == NULL) ||
               (DeviceInfo->ObjectDelete == NULL) ||
               (DeviceInfo->DeviceReset == NULL) ||
               (DeviceInfo->StorageInfoGet == NULL) ||
               (DeviceInfo->ObjectNumberGet == NULL) ||
               (DeviceInfo->ObjectHandlesGet == NULL) ||
               (DeviceInfo->ObjectInfoGet == NULL) ||
               (DeviceInfo->ObjectDataGet == NULL) ||
               (DeviceInfo->ObjectInfoSend == NULL) ||
               (DeviceInfo->ObjectDataSend == NULL) ||
               (DeviceInfo->ObjectPropDescGet == NULL) ||
               (DeviceInfo->ObjectPropValueGet == NULL) ||
               (DeviceInfo->ObjectPropValueSet == NULL) ||
               (DeviceInfo->ObjectReferenceGet == NULL) ||
               (DeviceInfo->ObjectReferenceSet == NULL) ||
               (DeviceInfo->ObjectPropListGet == NULL) ||
               (DeviceInfo->ObjectCustomCommand == NULL) ||
               (DeviceInfo->ObjectCustomDataGet == NULL) ||
               (DeviceInfo->ObjectCustomDataSend == NULL) ||
               (DeviceInfo->ObjectClearAll == NULL) ||
               (DeviceInfo->TransferCanceled == NULL) ||
               (DeviceInfo->ObjectThumbGet == NULL) ||
               (DeviceInfo->VendorExtensionDesc == NULL)) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else {
        // do nothing
    }
    return uret;
}
/** \addtogroup Public_APIs
 *  @{
 * */
/**
 * This function helps to setup the MTP class system information. It must be called before AmbaUSBD_SystemClassStart().
 * @param pDeviceInfo [Input] MTP device information.
*/
UINT32 AmbaUSBD_MTPSetInfo(const USBD_MTP_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 uret = check_null_for_device_info(pDeviceInfo);

    // check parameters
    if (uret == USB_ERR_SUCCESS) {
        // copy parameters. To pass misra-c, do not use memcpy
        mtp_sys_info.DeviceInfo.StorageId                  = pDeviceInfo->StorageId;
        mtp_sys_info.DeviceInfo.StorageType                = pDeviceInfo->StorageType;
        mtp_sys_info.DeviceInfo.FileSystemType             = pDeviceInfo->FileSystemType;
        mtp_sys_info.DeviceInfo.AccessCapability           = pDeviceInfo->AccessCapability;
        mtp_sys_info.DeviceInfo.MaxCapacityLow             = pDeviceInfo->MaxCapacityLow;
        mtp_sys_info.DeviceInfo.MaxCapacityHigh            = pDeviceInfo->MaxCapacityHigh;
        mtp_sys_info.DeviceInfo.FreeSpaceLow               = pDeviceInfo->FreeSpaceLow;
        mtp_sys_info.DeviceInfo.FreeSpaceHigh              = pDeviceInfo->FreeSpaceHigh;
        mtp_sys_info.DeviceInfo.FreeSpaceImage             = pDeviceInfo->FreeSpaceImage;
        mtp_sys_info.DeviceInfo.DeviceInfoVendorName       = pDeviceInfo->DeviceInfoVendorName;
        mtp_sys_info.DeviceInfo.DeviceInfoProductName      = pDeviceInfo->DeviceInfoProductName;
        mtp_sys_info.DeviceInfo.DeviceInfoSerialNo         = pDeviceInfo->DeviceInfoSerialNo;
        mtp_sys_info.DeviceInfo.DeviceInfoVersion          = pDeviceInfo->DeviceInfoVersion;
        mtp_sys_info.DeviceInfo.VolumeDescription          = pDeviceInfo->VolumeDescription;
        mtp_sys_info.DeviceInfo.VolumeLabel                = pDeviceInfo->VolumeLabel;
        mtp_sys_info.DeviceInfo.DeviceSupportProp          = pDeviceInfo->DeviceSupportProp;
        mtp_sys_info.DeviceInfo.DeviceSupportCaptureFormat = pDeviceInfo->DeviceSupportCaptureFormat;
        mtp_sys_info.DeviceInfo.DeviceSupportImgFormat     = pDeviceInfo->DeviceSupportImgFormat;
        mtp_sys_info.DeviceInfo.ObjectSupportProp          = pDeviceInfo->ObjectSupportProp;
        mtp_sys_info.DeviceInfo.OperationSupportList       = pDeviceInfo->OperationSupportList;
        mtp_sys_info.DeviceInfo.EventSupportList           = pDeviceInfo->EventSupportList;
        mtp_sys_info.DeviceInfo.PropDescGet                = pDeviceInfo->PropDescGet;
        mtp_sys_info.DeviceInfo.PropValueGet               = pDeviceInfo->PropValueGet;
        mtp_sys_info.DeviceInfo.PropValueSet               = pDeviceInfo->PropValueSet;
        mtp_sys_info.DeviceInfo.PropValueReset             = pDeviceInfo->PropValueReset;
        mtp_sys_info.DeviceInfo.StorageFormat              = pDeviceInfo->StorageFormat;
        mtp_sys_info.DeviceInfo.ObjectDelete               = pDeviceInfo->ObjectDelete;
        mtp_sys_info.DeviceInfo.DeviceReset                = pDeviceInfo->DeviceReset;
        mtp_sys_info.DeviceInfo.StorageInfoGet             = pDeviceInfo->StorageInfoGet;
        mtp_sys_info.DeviceInfo.ObjectNumberGet            = pDeviceInfo->ObjectNumberGet;
        mtp_sys_info.DeviceInfo.ObjectHandlesGet           = pDeviceInfo->ObjectHandlesGet;
        mtp_sys_info.DeviceInfo.ObjectInfoGet              = pDeviceInfo->ObjectInfoGet;
        mtp_sys_info.DeviceInfo.ObjectDataGet              = pDeviceInfo->ObjectDataGet;
        mtp_sys_info.DeviceInfo.ObjectInfoSend             = pDeviceInfo->ObjectInfoSend;
        mtp_sys_info.DeviceInfo.ObjectDataSend             = pDeviceInfo->ObjectDataSend;
        mtp_sys_info.DeviceInfo.ObjectPropDescGet          = pDeviceInfo->ObjectPropDescGet;
        mtp_sys_info.DeviceInfo.ObjectPropValueGet         = pDeviceInfo->ObjectPropValueGet;
        mtp_sys_info.DeviceInfo.ObjectPropValueSet         = pDeviceInfo->ObjectPropValueSet;
        mtp_sys_info.DeviceInfo.ObjectReferenceGet         = pDeviceInfo->ObjectReferenceGet;
        mtp_sys_info.DeviceInfo.ObjectReferenceSet         = pDeviceInfo->ObjectReferenceSet;
        mtp_sys_info.DeviceInfo.ObjectPropListGet          = pDeviceInfo->ObjectPropListGet;
        mtp_sys_info.DeviceInfo.ObjectCustomCommand        = pDeviceInfo->ObjectCustomCommand;
        mtp_sys_info.DeviceInfo.ObjectCustomDataGet        = pDeviceInfo->ObjectCustomDataGet;
        mtp_sys_info.DeviceInfo.ObjectCustomDataSend       = pDeviceInfo->ObjectCustomDataSend;
        mtp_sys_info.DeviceInfo.ObjectClearAll             = pDeviceInfo->ObjectClearAll;
        USB_UtilityMemcpyUInt8(mtp_sys_info.DeviceInfo.RootPath, pDeviceInfo->RootPath, USBD_MTP_MAX_FILENAME_LEN);
        mtp_sys_info.DeviceInfo.RootPath[USBD_MTP_MAX_FILENAME_LEN - 1U] = 0;
        mtp_sys_info.DeviceInfo.TransferCanceled                         = pDeviceInfo->TransferCanceled;
        mtp_sys_info.DeviceInfo.ObjectThumbGet                           = pDeviceInfo->ObjectThumbGet;
        mtp_sys_info.DeviceInfo.VendorExtensionID                        = pDeviceInfo->VendorExtensionID;
        mtp_sys_info.DeviceInfo.VendorExtensionVersion                   = pDeviceInfo->VendorExtensionVersion;
        mtp_sys_info.DeviceInfo.VendorExtensionDesc                      = pDeviceInfo->VendorExtensionDesc;
    }

    return uret;
}
/**
 * This function informs the Host of a MTP event through an interrupt endpoint.
 * @param pEvent [Input] The event information
*/
UINT32 AmbaUSBD_MTPAddEvent(const USBD_MTP_EVENT_s *pEvent)
{
    UINT32 uret                            = 0;
    UX_SLAVE_CLASS_PIMA_EVENT ux_mtp_event = { 0 };
    UX_SLAVE_CLASS_PIMA *mtp_ctx           = udc_mtp_get_context();

    if ((pEvent != NULL) && (pEvent->ParameterNumber <= 3U) && (pEvent->SessionId == 0U)) {
        if (mtp_ctx != NULL) {
            // to pass misra-c, do not use memcpy
            ux_mtp_event.code             = pEvent->Code;
            ux_mtp_event.length_change    = 1;
            ux_mtp_event.transaction_id   = pEvent->TransactionId;
            ux_mtp_event.parameter_number = pEvent->ParameterNumber;
            ux_mtp_event.parameter_1      = pEvent->Parameter1;
            ux_mtp_event.parameter_2      = pEvent->Parameter2;
            ux_mtp_event.parameter_3      = pEvent->Parameter3;
            uret                          = _ux_device_class_pima_event_set(mtp_ctx, &ux_mtp_event);
            if (uret != (UINT32)UX_SUCCESS) {
                uret = USBX_ERR_MTP_EVENT_ARRAY_FULL;
            }
        } else {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "AmbaUSBD_MTPAddEvent(): No MTP context.");
            uret = USB_ERR_NO_INSTANCE;
        }
    } else {
        if ((pEvent != NULL) && (pEvent->ParameterNumber > 3U)) {
            // print debug message
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "AmbaUSBD_MTPAddEvent(): Parameter number > 3.");
        }
        uret = USB_ERR_PARAMETER_INVALID;
    }

    return uret;
}
/** @} */

static UINT32 usbd_mtp_start_impl(UINT32 configuration, UINT32 interface_index)
{
    UINT32 uret;
    static UX_SLAVE_CLASS_PIMA_PARAMETER pima_parameter;
    static UCHAR uxd_pima_name[] =  "ux_slave_class_pima";

    /* Initialialize the pima device parameter.  */
    pima_parameter.manufacturer                   = mtp_sys_info.DeviceInfo.DeviceInfoVendorName;
    pima_parameter.model                          = mtp_sys_info.DeviceInfo.DeviceInfoProductName;
    pima_parameter.device_version                 = mtp_sys_info.DeviceInfo.DeviceInfoVersion;
    pima_parameter.serial_number                  = mtp_sys_info.DeviceInfo.DeviceInfoSerialNo;
    pima_parameter.storage_id                     = mtp_sys_info.DeviceInfo.StorageId;
    pima_parameter.storage_type                   = mtp_sys_info.DeviceInfo.StorageType;
    pima_parameter.storage_file_system_type       = mtp_sys_info.DeviceInfo.FileSystemType;
    pima_parameter.storage_access_capability      = mtp_sys_info.DeviceInfo.AccessCapability;
    pima_parameter.storage_max_capacity_low       = mtp_sys_info.DeviceInfo.MaxCapacityLow;
    pima_parameter.storage_max_capacity_high      = mtp_sys_info.DeviceInfo.MaxCapacityHigh;
    pima_parameter.storage_free_space_low         = mtp_sys_info.DeviceInfo.FreeSpaceLow;
    pima_parameter.storage_free_space_high        = mtp_sys_info.DeviceInfo.FreeSpaceHigh;
    pima_parameter.storage_free_space_image       = mtp_sys_info.DeviceInfo.FreeSpaceImage;
    pima_parameter.storage_description            = mtp_sys_info.DeviceInfo.VolumeDescription;
    pima_parameter.storage_volume_label           = mtp_sys_info.DeviceInfo.VolumeLabel;
    pima_parameter.device_properties_list         = mtp_sys_info.DeviceInfo.DeviceSupportProp;
    pima_parameter.supported_capture_formats_list = mtp_sys_info.DeviceInfo.DeviceSupportCaptureFormat;
    pima_parameter.supported_image_formats_list   = mtp_sys_info.DeviceInfo.DeviceSupportImgFormat;
    pima_parameter.object_properties_list         = mtp_sys_info.DeviceInfo.ObjectSupportProp;
    pima_parameter.supported_operation_list       = mtp_sys_info.DeviceInfo.OperationSupportList;
    pima_parameter.vendor_extension_id            = mtp_sys_info.DeviceInfo.VendorExtensionID;
    pima_parameter.vendor_extension_version       = mtp_sys_info.DeviceInfo.VendorExtensionVersion;
    pima_parameter.vendor_extension_desc          = mtp_sys_info.DeviceInfo.VendorExtensionDesc;

    /* Define the callbacks.  */
    pima_parameter.device_reset            = device_reset_func;
    pima_parameter.device_prop_desc_get    = prop_desc_get_func;
    pima_parameter.device_prop_value_get   = prop_value_get_func;
    pima_parameter.device_prop_value_set   = prop_value_set_func;
    pima_parameter.device_prop_value_reset = prop_value_reset_func;
    pima_parameter.storage_format          = storage_format_func;
    pima_parameter.storage_info_get        = storage_info_get_func;
    pima_parameter.object_number_get       = object_number_get_func;
    pima_parameter.object_handles_get      = object_handles_get_func;
    pima_parameter.object_info_get         = object_info_get_func;
    pima_parameter.object_data_get         = NULL;
    pima_parameter.object_data_get_ex      = object_data_get_ex_func;
    pima_parameter.object_thumb_get        = object_thumb_get_func;
    pima_parameter.object_info_send        = object_info_send_func;
    pima_parameter.object_data_send        = NULL;
    pima_parameter.object_data_send_ex     = object_data_send_ex_func;
    pima_parameter.object_delete           = object_delete_func;
    pima_parameter.object_prop_desc_get    = object_prop_desc_get_func;
    pima_parameter.object_prop_value_get   = object_prop_value_get_func;
    pima_parameter.object_prop_value_set   = object_prop_value_set_func;
    pima_parameter.object_references_get   = object_reference_get_func;
    pima_parameter.object_references_set   = object_reference_set_func;
    pima_parameter.prop_list_get           = object_prop_list_get_func;
    pima_parameter.transfer_canceled       = transfer_canceled_func;
    pima_parameter.initiate_capture        = initiate_capture_func;
    pima_parameter.open_capture_initiate   = open_capture_initiate_func;
    pima_parameter.open_capture_terminate  = open_capture_terminate_func;

    pima_parameter.ObjectXferExtensionVer = 1; // force to use UINT64 version for DataGet and DataSend

    pima_parameter.custom_command   = custom_command_func;
    pima_parameter.custom_data_get  = custom_data_get_func;
    pima_parameter.custom_data_send = custom_data_send_func;
    pima_parameter.vendor_handler   = vendor_process_func;

    /* Store the instance owner.  */
    pima_parameter.application = NULL;

    /* Set Event Support List individually */
    if (_udc_mtp_set_supported_events(mtp_sys_info.DeviceInfo.EventSupportList) != 0U) {
        // always success
    }

    /* Initilize the device pima class. The class is connected with interface 0 */

    uret = _ux_device_stack_class_register(uxd_pima_name,
                                           _ux_device_class_pima_entry,
                                           configuration,
                                           interface_index,
                                           &pima_parameter);

    /* Check the status of the init of the PIMA class.  */
    if (uret != (UINT32)UX_SUCCESS) {
        uret = USBX_ERR_CLASS_REGISTER_FAIL;
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "usbd_mtp_start_impl(): Failed to register MTP entry");
    } else {
        // print debug message
        USB_UtilityPrint(USB_PRINT_FLAG_INFO, "usbd_mtp_start_impl(): MTP entry registers successfully");
    }

    return uret;
}
/** \addtogroup Public_APIs
 *  @{
 * */
/**
 * This function is used for USB device system to stop MTP class.
 * Applications should pass it as ClassStopCb parameter when calling AmbaUSBD_SystemClassStart().
*/
UINT32 AmbaUSBD_MTPClassStop(void)
{
    UINT32 uret;

    if (USBD_SystemIsClassConfiged() != USB_ERR_SUCCESS) {
        uret = USB_ERR_NO_INIT;
    } else {
        if (mtp_sys_info.DeviceInfo.ObjectClearAll != NULL) {
            mtp_sys_info.DeviceInfo.ObjectClearAll();
        }

        /* clear device PIMA class */
        uret = _ux_device_stack_class_clear();

        /* Check the status of the clear of the PIMA class.  */
        if (uret != (UINT32)UX_SUCCESS) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "Failed to clear MTP class");
            uret = USBX_ERR_CLASS_DEINIT_FAIL;
        } else {
            // print debug message
            USB_UtilityPrint(USB_PRINT_FLAG_INFO, "MTP class clear successfully");
        }

        if (uret == (UINT32)UX_SUCCESS) {
            // free device stack
            uret = _ux_device_stack_free();
            if (uret != ((UINT32)UX_SUCCESS)) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_MTPClassStop(): Failed to free USBX device class with error %x\n", uret, 0, 0, 0, 0);
                uret = USBX_ERR_CLASS_DEINIT_FAIL;
            }
        }
    }
    return uret;
}
/** @} */

UINT32 USBD_MtpStackRegister(UINT32 Configuration, UINT32 InterfaceIndex)
{
    UINT32 uret;

    /* MTP requires MTP extensions.  */
    uret = _ux_device_stack_microsoft_extension_register(UX_DEMO_VENDOR_REQUEST, microsoft_vendor_request_func);

    if (uret == (UINT32)UX_SUCCESS) {
        uret = usbd_mtp_start_impl(Configuration, InterfaceIndex);
        USB_UtilityPrint(USB_PRINT_FLAG_INFO, "MTP device class start");
    }

    return uret;
}
/** \addtogroup Public_APIs
 *  @{
 * */
/**
 * This function is used for USB device system to start MTP class.
 * Applications should pass it as ClassStartCb parameter when calling AmbaUSBD_SystemClassStart().
 * @param pDescInfo [Input] Descriptor information.
*/
UINT32 AmbaUSBD_MTPClassStart(const USBD_DESC_CUSTOM_INFO_s *pDescInfo)
{
    UINT32 uret;

    if (pDescInfo == NULL) {
        uret = USB_ERR_PARAMETER_NULL;
    } else if ((pDescInfo->DescFrameworkFs == NULL) ||
               (pDescInfo->DescFrameworkHs == NULL) ||
               (pDescInfo->StrFramework == NULL) ||
               (pDescInfo->LangIDFramework == NULL)) {
        uret = USB_ERR_PARAMETER_NULL;
    } else if ((pDescInfo->DescSizeFs == 0U) ||
               (pDescInfo->DescSizeHs == 0U) ||
               (pDescInfo->StrSize == 0U) ||
               (pDescInfo->LangIDSize == 0U)) {
        uret = USB_ERR_PARAMETER_INVALID;

    } else if (USBD_SystemIsClassConfiged() != USB_ERR_SUCCESS) {
        uret = USB_ERR_NO_INIT;

    } else {

        if (pDescInfo->DescFrameworkSs != NULL) {
            // call _ux_device_stack_initialize for SS
            uret =  _ux_device_stack_ss_initialize(pDescInfo->DescFrameworkSs, pDescInfo->DescSizeSs);
        } else {
            uret = 0;
        }
        if (uret != (UINT32)UX_SUCCESS) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_MTPClassStart(): Failed to init SS device framework with error 0x%x", uret, 0, 0, 0, 0);
        } else {

            // call _ux_device_stack_initialize for HS/FS
            uret = _ux_device_stack_initialize(pDescInfo->DescFrameworkHs,
                                               pDescInfo->DescSizeHs,
                                               pDescInfo->DescFrameworkFs,
                                               pDescInfo->DescSizeFs,
                                               pDescInfo->StrFramework,
                                               pDescInfo->StrSize,
                                               pDescInfo->LangIDFramework,
                                               pDescInfo->LangIDSize,
                                               UX_NULL);

            if (uret != (UINT32)UX_SUCCESS) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_MTPClassStart(): Failed to init USBX MTP device framework with error %x\n", uret, 0, 0, 0, 0);
                uret = USB_UtilityUXCode2AmbaCode(uret);
            } else {
                USB_UtilityPrint(USB_PRINT_FLAG_INFO, "AmbaUSBD_MTPClassStart(): Finish init USBX MTP device framework");
                // start class
                uret = USBD_MtpStackRegister(1, 0);
            }
        }
    }

    return uret;
}
/** @} */

