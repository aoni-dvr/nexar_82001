/**
 *  @file AmbaUSBD_Uvc.c
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
 *  @details USB wrapper functions for Video Device Class
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaPrint.h>
#include <AmbaUSB_Generic.h>
#include "AmbaUSBD_API.h"
#include "AmbaUSBD_Uvc.h"
#include <AmbaUSB_ErrCode.h>

//#ifdef __USBX_DEBUG__

static void utility_print(const char *Str)
{
    AmbaPrint_PrintStr5(Str, NULL, NULL, NULL, NULL, NULL);
}

static void utility_print_uint5(const char *FmtStr, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    AmbaPrint_PrintUInt5(FmtStr, Arg1, Arg2, Arg3, Arg4, Arg5);
}

static UINT8 uvc_is_vc_desc(const UINT8 *desc)
{
    UINT8 uret = 0;

    // At least one video stream
    if ((desc[0] >= 0x0DU) && (desc[1] == UVC_DESC_TYPE_CS_INTF) && (desc[2] == UVC_DESC_SUBTYPE_VC_HEADER)) {
        uret = 1;
    }
    return uret;
}

static UINT8 uvc_is_vs_desc(const UINT8 *desc)
{
    UINT8 uret = 0;

    if ((desc[0] == 0x0EU) && (desc[1] == UVC_DESC_TYPE_CS_INTF) && (desc[2] == UVC_DESC_SUBTYPE_VS_INPUT_HEADER)) {
        uret = 1;
    } else if ((desc[0] == 0x0FU) && (desc[1] == UVC_DESC_TYPE_CS_INTF) && (desc[2] == UVC_DESC_SUBTYPE_VS_INPUT_HEADER)) {
        uret = 1;
    } else {
        // pass vcast
    }
    return uret;
}

static UINT8 uvc_is_desc_type(const UINT8 *desc, UINT8 type, UINT8 subtype)
{
    UINT8 uret = 0;

    if ((desc[1] == type) && (desc[2] == subtype)) {
        uret = 1;
    }
    return uret;
}

struct _uvc_desc_type {
    UINT8 type;
    UINT8 subtype;
};

static UINT8 uvc_is_vc_desc_type(const UINT8 *desc)
{
    static struct _uvc_desc_type uvc_vc_desc_types[] = {
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VC_OUTPUT_TERMINAL },   // output terminal
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VC_EXTENSION_UNIT },    // extension unit
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VC_INPUT_TERMINAL },    // input terminal
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VC_PROCESSING_UNIT },   // Processing Unit
    };

    static UINT32 uvc_vc_desc_types_size = sizeof(uvc_vc_desc_types) / sizeof(struct _uvc_desc_type);

    UINT8 i;
    UINT8 uret = 0;

    for (i = 0; i < uvc_vc_desc_types_size; i++) {
        const struct _uvc_desc_type *t = &uvc_vc_desc_types[i];
        if (uvc_is_desc_type(desc, t->type, t->subtype) == 1U) {
            uret = 1;
        }
    }
    return uret;
}

static UINT8 uvc_is_vc_input_terminal_desc(const UINT8 *desc)
{
    UINT8 uret = 0;

    if ((desc[1] == UVC_DESC_TYPE_CS_INTF) && (desc[2] == UVC_DESC_SUBTYPE_VC_INPUT_TERMINAL)) {
        uret = 1;
    }
    return uret;
}

static UINT8 uvc_is_vc_processing_unit_desc(const UINT8 *desc)
{
    UINT8 uret = 0;

    if ((desc[1] == UVC_DESC_TYPE_CS_INTF) && (desc[2] == UVC_DESC_SUBTYPE_VC_PROCESSING_UNIT)) {
        uret = 1;
    }
    return uret;
}

static UINT8 uvc_is_vc_extension_unit_desc(const UINT8 *desc)
{
    UINT8 uret = 0;

    if ((desc[1] == UVC_DESC_TYPE_CS_INTF) && (desc[2] == UVC_DESC_SUBTYPE_VC_EXTENSION_UNIT)) {
        uret = 1;
    }
    return uret;
}

static UINT8 uvc_is_vs_desc_type(const UINT8 *desc)
{
    static struct _uvc_desc_type uvc_vs_desc_types[] = {
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FORMAT_MJPEG },
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FRAME_MJPEG },
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FORMAT_UNCOMPRESSED },
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FRAME_UNCOMPRESSED },
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FORMAT_H264 },
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FRAME_H264 },
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED },
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED },
        {UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_STILL_IMAGE_FRAME},
    };

    static UINT32 uvc_vs_desc_types_size = sizeof(uvc_vs_desc_types) / sizeof(struct _uvc_desc_type);

    UINT8 i;
    UINT8 uret = 0;

    for (i = 0; i < uvc_vs_desc_types_size; i++) {
        const struct _uvc_desc_type *t = &uvc_vs_desc_types[i];
        if (uvc_is_desc_type(desc, t->type, t->subtype) == 1U) {
            uret = 1;
        }
    }
    return uret;
}

/**
 * This function is used for USB device system to patch UVC descriptors.
 * @param whole_desc full USB descriptor
 * @param desc_size  size of USB descriptor in byte
 * @return 0: Success, UX_ERROR: Fail
*/
UINT32 USBD_UVCDescPatch(UINT8 *whole_desc, UINT32 desc_size)
{
    UINT8 *desc                 = whole_desc;
    UINT16 total_length         = 0;
    UINT16 count                = 0;
    UINT8 length                = 0;
    UINT16 vc_desc_pos          = 0;
    UINT16 vs_desc_pos          = 0;
    UINT16 vc_desc_total_length = 0;
    UINT16 vs_desc_total_length = 0;
    UINT8 vc_desc_scan_start    = 0;
    UINT8 vs_desc_scan_start    = 0;
    UINT16 i                    = 0;
    UINT8 *config_desc;
    UINT32 uret = 0;

    // step to configuration descriptor
    while (desc[i] > 0U) {
        if ((desc[i] == USB_DESC_LENGTH_CFG) && (desc[i + 1U] == USB_DESC_TYPE_CFG)) {
            break;
        }
        length = desc[i];
        i     += length;
    }
    //utility_print_uint5("    config desc pos = %d.", i, 0, 0, 0, 0);

    desc_size  -= i;
    desc        = &whole_desc[i];
    config_desc = desc;

    do {
        // first byte should be desc length
        length = desc[count];
        if (length == 0U) {
            utility_print("[Error] descriptor size should not be zero.");
            utility_print_uint5("[Error]     @ position %d.", count, 0, 0, 0, 0);
            uret = UX_ERROR;
            break;
        } else {
            //utility_print_uint5("    desc size = %d.", length, 0, 0, 0, 0);

            if ((uvc_is_vc_desc(&desc[count]) == 1U) && (vc_desc_pos == 0U)) {
                vc_desc_pos           = count;
                vc_desc_total_length += length;
                vc_desc_scan_start    = 1;
                //utility_print_uint5("[uvc] vc_desc_pos = %d.", vc_desc_pos, 0, 0, 0, 0);
            } else if ((uvc_is_vs_desc(&desc[count]) == 1U) && (vs_desc_pos == 0U)) {
                vs_desc_pos           = count;
                vs_desc_total_length += length;
                vc_desc_scan_start    = 0;
                vs_desc_scan_start    = 1;
                //utility_print_uint5("[uvc] vs_desc_pos = %d.", vs_desc_pos, 0, 0, 0, 0);
            } else if ((uvc_is_vc_desc_type(&desc[count]) == 1U) && (vc_desc_scan_start == 1U)) {
                vc_desc_total_length += length;
                //utility_print("[uvc] vc desc type.");
                if (uvc_is_vc_input_terminal_desc(&desc[count]) != 0U) {
                    // patch VC Input Terminal's bmControls field
                } else if (uvc_is_vc_processing_unit_desc(&desc[count]) != 0U) {
                    // patch VC Processing Unit's bmControls field
                } else if (uvc_is_vc_extension_unit_desc(&desc[count]) != 0U) {
                    // XU
                } else {
                    // pass vcast
                }
            } else if ((uvc_is_vs_desc_type(&desc[count]) == 1U) && (vs_desc_scan_start == 1U)) {
                vs_desc_total_length += length;
                //utility_print("[uvc] vs desc type.");
            } else if ((vs_desc_scan_start == 1U) &&
                       (desc[count] == USB_DESC_LENGTH_ENDPOINT) &&
                       (desc[count + 1U] == USB_DESC_TYPE_ENDPOINT)) {
                // Assume that the endpont desc means the end of VS desc.
                vs_desc_scan_start = 0;
                //utility_print_uint5("vs_desc_total_length = 0x%X.", vs_desc_total_length, 0, 0, 0, 0);
                config_desc[vs_desc_pos + 4U] = (UINT8)(vs_desc_total_length & 0x0FFU);
                config_desc[vs_desc_pos + 5U] = (UINT8)((vs_desc_total_length >> 8U) & 0x0FFU);
                // to check if there is another vs
                vs_desc_pos          = 0;
                vs_desc_total_length = 0;
            } else {
                // pass vcast
            }
        }
        total_length += length;
        count        += length;
    } while (count < desc_size);

    if (count != desc_size) {
        utility_print("[Error] descriptor size mismatch.");
        utility_print_uint5("[Error]    real size is %d, but calculated size is %d.", desc_size, count, 0, 0, 0);
        uret = UX_ERROR;
    } else {
        //utility_print_uint5("uvc configuration desc length = 0x%X.", total_length, 0, 0, 0, 0);
        //utility_print_uint5("vc_desc_total_length = 0x%X.", vc_desc_total_length, 0, 0, 0, 0);

        config_desc[2] = (UINT8)(total_length & 0x0FFU);
        config_desc[3] = (UINT8)((total_length >> 8U) & 0x0FFU);

        config_desc[vc_desc_pos + 5U] = (UINT8)(vc_desc_total_length & 0x0FFU);
        config_desc[vc_desc_pos + 6U] = (UINT8)((vc_desc_total_length >> 8U) & 0x0FFU);
    }
    return uret;
}

/**
 * This function is used for USB device system to register UVC class.
 * @param ConfigurationIdx USB configuration index of UVC class
 * @param InterfaceIdx USB interface index  of UVC class
 * @return 0: Success, others: Fail
*/
UINT32 USBD_UVCClassRegister(UINT32 ConfigurationIdx, UINT32 InterfaceIdx)
{
    static UX_SLAVE_CLASS_UVC_PARAMETER gUVCParameter = { 0 };
    static UCHAR uxd_uvc_name[] =  "ux_slave_class_uvc";
    UINT32 uret;

    uret = _ux_device_stack_class_register(uxd_uvc_name,
                                           uxd_uvc_entry,
                                           ConfigurationIdx,
                                           InterfaceIdx,
                                           &gUVCParameter);

    if (uret != 0U) {
        utility_print_uint5("[USBD] Failed to register video device class with error 0x%X.", uret, 0, 0, 0, 0);
    }

    return uret;
}

/** \addtogroup Public_APIs
 *  @{
 * */

/**
 * This function is used for USB device system to start UVC class.
 * Applications should pass it as ClassStartCb parameter when calling AmbaUSBD_SystemClassStart().
 * @param DescInfo Information of USB descriptors
 * @return 0: Success, others: Fail
*/
UINT32 AmbaUSBD_UVCClassStart(const USBD_DESC_CUSTOM_INFO_s *DescInfo)
{
    UINT32 uret;

    uret = USBD_UVCDescPatch(DescInfo->DescFrameworkFs, DescInfo->DescSizeFs);
    if (uret != USB_ERR_SUCCESS) {
        utility_print("AmbaUSBD_UVCClassStart(): can't patch UVC FS desc.");
    }
    uret = USBD_UVCDescPatch(DescInfo->DescFrameworkHs, DescInfo->DescSizeHs);
    if (uret != USB_ERR_SUCCESS) {
        utility_print("AmbaUSBD_UVCClassStart(): can't patch UVC HS desc.");
    }

    if (DescInfo->DescFrameworkSs != NULL) {
        uret = USBD_UVCDescPatch(DescInfo->DescFrameworkSs, DescInfo->DescSizeSs);
        if (uret != USB_ERR_SUCCESS) {
            utility_print("AmbaUSBD_UVCClassStart(): can't patch UVC SS desc.");
        }
    }

    if (uret == USB_ERR_SUCCESS) {

        if (DescInfo->DescFrameworkSs != NULL) {
            // call _ux_device_stack_initialize for SS
            uret =  _ux_device_stack_ss_initialize(DescInfo->DescFrameworkSs, DescInfo->DescSizeSs);
        } else {
            uret = 0;
        }
        if (uret != (UINT32)UX_SUCCESS) {
#ifdef  USB_PRINT_SUPPORT
            USB_UtilityPrintUInt5("AmbaUSBD_UVCClassStart(): Failed to init SS device framework with error 0x%x", uret, 0, 0, 0, 0);
#endif
        } else {

            // initialize protocol stack for HS/FS
            uret = _ux_device_stack_initialize(DescInfo->DescFrameworkHs,
                                               DescInfo->DescSizeHs,
                                               DescInfo->DescFrameworkFs,
                                               DescInfo->DescSizeFs,
                                               DescInfo->StrFramework,
                                               DescInfo->StrSize,
                                               DescInfo->LangIDFramework,
                                               DescInfo->LangIDSize,
                                               UX_NULL);

            if (uret != (UINT32)UX_SUCCESS) {
                utility_print_uint5("AmbaUSBD_UVCClassStart(): Failed to init USBX UVC device framework with error %x\n", uret, 0, 0, 0, 0);
            } else {
                utility_print("AmbaUSBD_UVCClassStart(): Finish init USBX UVC device framework");
                // start class
                uret = USBD_UVCClassRegister(1, 0);
            }
        }
    }

    // For multiple video stream interface case, set interface call is registered in app layer.
    // Since we don't know how many video stream that application used.

    return uret;
}

/**
 * This function is used for USB device system to stop UVC class.
 * Applications should pass it as ClassStopCb parameter when calling AmbaUSBD_SystemClassStart().
 * @return 0: Success, others: Fail
 * */
UINT32 AmbaUSBD_UVCClassStop(void)
{
    UINT32 status;

    status = _ux_device_stack_class_clear();

    if (status != 0U) {
        utility_print("AmbaUSBD_UVCClassStop(): Failed to clean UVC class");
    } else {
        utility_print("AmbaUSBD_UVCClassStop(): UVC class clean.");
    }

    return status;
}
/** @} */
