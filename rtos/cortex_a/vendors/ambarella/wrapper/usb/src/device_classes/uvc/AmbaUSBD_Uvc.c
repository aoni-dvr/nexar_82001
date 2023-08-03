/**
 *  @file AmbaUSBD_Uvc.c
 *
 *  @copyright Copyright (c) 2017 Ambarella, Inc.
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
#include <AmbaUSB_Generic.h>
#include "AmbaUSBD_API.h"
#include "AmbaUSBD_Uvc.h"
#include <AmbaUSB_ErrCode.h>

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define AmbaUSB_Utility_PrintUInt5(...)
#define AmbaUSB_Utility_Print(...)
#endif
//#ifdef __USBX_DEBUG__

UINT32 AmbaUSBD_UVCBulkSend(UINT32 Id, UINT8 *Buffer, UINT32 Size, UINT32 Timeout, UINT32 NoCopy)
{
	return _ux_device_class_uvc_multi_payload_send(Id, Buffer, Size, Timeout, NoCopy);
}

// can be handled by application
#if 0
static UINT32 uvc_cb_set_interface(UINT32 intface_number, UINT32 alternate_setting)
{
    if (alternate_setting == 0) {
        uvcd_notify_encode_stop(intface_number - 1); //Assume 1st interface is video control
    }

    return UX_SUCCESS;
}
#endif

static UINT32 usbd_uvc_register_class(void)
{
    static UX_SLAVE_CLASS_UVC_PARAMETER gUVCParameter = { 0 };
    static UCHAR uxd_uvc_name[] =  "ux_slave_class_uvc";
    UINT32 uRet;

    uRet = _ux_device_stack_class_register(uxd_uvc_name,
                                           _ux_device_class_uvc_entry,
                                           1,
                                           0,
                                           &gUVCParameter);

    if (uRet != 0U) {
        AmbaUSB_Utility_PrintUInt5("[USBD] Failed to register video device class with error 0x%X.", uRet, 0, 0, 0, 0);
    }

    return uRet;
}

UINT32 AmbaUSBD_UVCClassStart(const USBD_DESC_CUSTOM_INFO_s *DescInfo)
{
    UINT32 uRet;
    UINT32 it = 0;
    UINT32 pu = 0;

    uRet = AmbaUSBD_UVCGetITAttribute(&it);
    if (uRet != USB_ERR_SUCCESS) {
        AmbaUSB_Utility_Print("usbd_init_class(): can't get IT attribure.");
    }

    uRet = AmbaUSBD_UVCGetPUAttribute(&pu);
    if (uRet != USB_ERR_SUCCESS) {
        AmbaUSB_Utility_Print("usbd_init_class(): can't get PU attribure.");
    }

    uRet = usbd_uvc_desc_patch(DescInfo->DescFrameworkFs, DescInfo->DescSizeFs, it, pu);
    if (uRet != USB_ERR_SUCCESS) {
        AmbaUSB_Utility_Print("usbd_init_class(): can't patch UVC FS desc.");
    }
    uRet = usbd_uvc_desc_patch(DescInfo->DescFrameworkHs, DescInfo->DescSizeHs, it, pu);
    if (uRet != USB_ERR_SUCCESS) {
        AmbaUSB_Utility_Print("usbd_init_class(): can't patch UVC HS desc.");
    }

    if (uRet == USB_ERR_SUCCESS) {
        // initialize protocol stack
        uRet = _ux_device_stack_initialize(DescInfo->DescFrameworkHs,
                                           DescInfo->DescSizeHs,
                                           DescInfo->DescFrameworkFs,
                                           DescInfo->DescSizeFs,
                                           DescInfo->StrFramework,
                                           DescInfo->StrSize,
                                           DescInfo->LangIDFramework,
                                           DescInfo->LangIDSize,
                                           UX_NULL);

        if (uRet != (UINT32)UX_SUCCESS) {
            AmbaUSB_Utility_PrintUInt5("usbd_init_class(): Failed to init USBX UVC device framework with error %x\n", uRet, 0, 0, 0, 0);
        } else {
            AmbaUSB_Utility_Print("usbd_init_class(): Finish init USBX UVC device framework");
            // start class
            uRet = usbd_uvc_register_class();
        }
    }

    // For multiple video stream interface case, set interface call is registered in app layer.
    // Since we don't know how many video stream that application used.

    return uRet;
}

UINT32 AmbaUSBD_UVCClassStop(void)
{
    UINT32 status;

    status = _ux_device_stack_class_clear();

    if (status != 0U) {
        AmbaUSB_Utility_Print("usbd_uvc_stop(): Failed to clean UVC class");
    } else {
        AmbaUSB_Utility_Print("usbd_uvc_stop(): UVC class clean.");
    }

    return status;
}

UINT32 AmbaUSBD_UVCRegisterCallback(const USBD_UVC_EVENT_CALLBACK_s *cbs)
{
    uvcd_event_callback_t cbs_udc = { NULL };

    cbs_udc.encode_start = cbs->EncodeStart;
    cbs_udc.encode_stop  = cbs->EncodeStop;
    cbs_udc.pu_set       = cbs->PuSet;
    cbs_udc.it_set       = cbs->ItSet;
    cbs_udc.xu_handler   = cbs->XuHandler;
    return uvcd_register_callback(&cbs_udc);
}

UINT32 AmbaUSBD_UVCSetITAttribute(UINT32 Attribute)
{
    return uvcd_set_it_attribute(Attribute);
}

UINT32 AmbaUSBD_UVCSetPUAttribute(UINT32 Attribute)
{
    return uvcd_set_pu_attribute(Attribute);
}

UINT32 AmbaUSBD_UVCGetITAttribute(UINT32 *pAttribute)
{
    *pAttribute = uvcd_get_it_attribute();
    return 0;
}

UINT32 AmbaUSBD_UVCGetPUAttribute(UINT32 *pAttribute)
{
    *pAttribute = uvcd_get_pu_attribute();
    return 0;
}

UINT32 AmbaUSBD_UVCGetHostConfig(USBD_UVC_HOST_CONFIG_s *pConfig)
{
    uvcd_host_config_t config_udc = { 0 };
    UINT32 nRet                   = uvcd_get_host_config(&config_udc);

    if (nRet == 0U) {
        pConfig->Framerate    = config_udc.framerate;
        pConfig->Height       = config_udc.height;
        pConfig->Width        = config_udc.width;
        pConfig->MjpegQuality = config_udc.mjpeg_quality;
        pConfig->Format       = config_udc.format;
    }

    return nRet;
}


UINT32 AmbaUSBD_UVCGetBrightness(USBD_UVC_BRIGHTNESS_s *pBrightness)
{
    uvcd_pu_brightness_t get;
    UINT32 uRet = 0;

    if (pBrightness == NULL) {
        uRet = USB_ERR_PARAMETER_INVALID;
    } else {
        uRet = uvcd_pu_get_brightness(&get);
        if (uRet == 0U) {
            pBrightness->Current = get.current_value;
            pBrightness->Default = get.default_value;
            pBrightness->Maximum = get.maximum_value;
            pBrightness->Minimum = get.minimum_value;
        } else {
            uRet = USB_ERR_FAIL;
        }
    }
    return uRet;
}

UINT32 AmbaUSBD_UVCSetBrightness(const USBD_UVC_BRIGHTNESS_s *pBrightness)
{
    uvcd_pu_brightness_t set;
    UINT32 uRet = 0;

    if (pBrightness == NULL) {
        uRet = USB_ERR_PARAMETER_INVALID;
    } else {
        set.current_value = pBrightness->Current;
        set.default_value = pBrightness->Default;
        set.maximum_value = pBrightness->Maximum;
        set.minimum_value = pBrightness->Minimum;
        uRet              = uvcd_pu_set_brightness(&set);
    }
    return uRet;
}

UINT32 AmbaUSBD_UVCGetSaturation(USBD_UVC_SATURATION_s *pSaturation)
{
    uvcd_pu_saturation_t get;
    UINT32 uRet = 0;

    if (pSaturation == NULL) {
        uRet = USB_ERR_PARAMETER_INVALID;
    } else {
        uRet = uvcd_pu_get_saturation(&get);
        if (uRet == 0U) {
            pSaturation->Current = get.current_value;
            pSaturation->Default = get.default_value;
            pSaturation->Maximum = get.maximum_value;
            pSaturation->Minimum = get.minimum_value;
        } else {
            uRet = USB_ERR_FAIL;
        }
    }
    return uRet;
}

UINT32 AmbaUSBD_UVCSetSaturation(const USBD_UVC_SATURATION_s *pSaturation)
{
    uvcd_pu_saturation_t set;
    UINT32 uRet = 0;

    if (pSaturation == NULL) {
        uRet = USB_ERR_PARAMETER_INVALID;
    } else {
        set.current_value = pSaturation->Current;
        set.default_value = pSaturation->Default;
        set.maximum_value = pSaturation->Maximum;
        set.minimum_value = pSaturation->Minimum;
        uRet              = uvcd_pu_set_saturation(&set);
    }
    return uRet;
}

UINT32 AmbaUSBD_UVCGetContrast(USBD_UVC_CONTRAST_s *pContrast)
{
    uvcd_pu_contrast_t get;
    UINT32 uRet = 0;

    if (pContrast == NULL) {
        uRet = USB_ERR_PARAMETER_INVALID;
    } else {
        uRet = uvcd_pu_get_contrast(&get);
        if (uRet == 0U) {
            pContrast->Current = get.current_value;
            pContrast->Default = get.default_value;
            pContrast->Maximum = get.maximum_value;
            pContrast->Minimum = get.minimum_value;
        } else {
            uRet = USB_ERR_FAIL;
        }
    }
    return uRet;
}

UINT32 AmbaUSBD_UVCSetContrast(const USBD_UVC_CONTRAST_s *pContrast)
{
    uvcd_pu_contrast_t set;
    UINT32 uRet = 0;

    if (pContrast == NULL) {
        uRet = USB_ERR_PARAMETER_INVALID;
    } else {
        set.current_value = pContrast->Current;
        set.default_value = pContrast->Default;
        set.maximum_value = pContrast->Maximum;
        set.minimum_value = pContrast->Minimum;
        uRet              = uvcd_pu_set_contrast(&set);
    }
    return uRet;
}
UINT32 AmbaUSBD_UVCGetHue(USBD_UVC_HUE_s *pHue)
{
    uvcd_pu_hue_t get;
    UINT32 uRet = 0;

    if (pHue == NULL) {
        uRet = USB_ERR_PARAMETER_INVALID;
    } else {
        uRet = uvcd_pu_get_hue(&get);
        if (uRet == 0U) {
            pHue->Current = get.current_value;
            pHue->Default = get.default_value;
            pHue->Maximum = get.maximum_value;
            pHue->Minimum = get.minimum_value;
        } else {
            uRet = USB_ERR_FAIL;
        }
    }
    return uRet;
}

UINT32 AmbaUSBD_UVCSetHue(const USBD_UVC_HUE_s *pHue)
{
    uvcd_pu_hue_t set;
    UINT32 uRet = 0;

    if (pHue == NULL) {
        uRet = USB_ERR_PARAMETER_INVALID;
    } else {
        set.current_value = pHue->Current;
        set.default_value = pHue->Default;
        set.maximum_value = pHue->Maximum;
        set.minimum_value = pHue->Minimum;
        uRet              = uvcd_pu_set_hue(&set);
    }
    return uRet;
}

UINT32 AmbaUSBD_UVCGetSharpness(USBD_UVC_SHARPNESS_s *pSharpness)
{
    uvcd_pu_sharpness_t get;
    UINT32 uRet = 0;

    if (pSharpness == NULL) {
        uRet = USB_ERR_PARAMETER_INVALID;
    } else {
        uRet = uvcd_pu_get_sharpness(&get);
        if (uRet == 0U) {
            pSharpness->Current = get.current_value;
            pSharpness->Default = get.default_value;
            pSharpness->Maximum = get.maximum_value;
            pSharpness->Minimum = get.minimum_value;
        } else {
            uRet = USB_ERR_FAIL;
        }
    }
    return uRet;
}

UINT32 AmbaUSBD_UVCSetSharpness(const USBD_UVC_SHARPNESS_s *pSharpness)
{
    uvcd_pu_sharpness_t set;
    UINT32 uRet = 0;

    if (pSharpness == NULL) {
        uRet = USB_ERR_PARAMETER_INVALID;
    } else {
        set.current_value = pSharpness->Current;
        set.default_value = pSharpness->Default;
        set.maximum_value = pSharpness->Maximum;
        set.minimum_value = pSharpness->Minimum;
        uRet              = uvcd_pu_set_sharpness(&set);
    }
    return uRet;
}

UINT32 AmbaUSBD_UVCGetPowerLine(USBD_UVC_POWER_LINE_s *pPowerLine)
{
    uvcd_pu_power_line_t get;
    UINT32 uRet = 0;

    if (pPowerLine == NULL) {
        uRet = USB_ERR_PARAMETER_INVALID;
    } else {
        uRet = uvcd_pu_get_powerline(&get);
        if (uRet == 0U) {
            pPowerLine->Current = get.current_value;
            pPowerLine->Default = get.default_value;
            pPowerLine->Maximum = get.maximum_value;
            pPowerLine->Minimum = get.minimum_value;
        } else {
            uRet = USB_ERR_FAIL;
        }
    }
    return uRet;
}

UINT32 AmbaUSBD_UVCSetPowerLine(const USBD_UVC_POWER_LINE_s *pPowerLine)
{
    uvcd_pu_power_line_t set;
    UINT32 uRet = 0;

    if (pPowerLine == NULL) {
        uRet = USB_ERR_PARAMETER_INVALID;
    } else {
        set.current_value = pPowerLine->Current;
        set.default_value = pPowerLine->Default;
        set.maximum_value = pPowerLine->Maximum;
        set.minimum_value = pPowerLine->Minimum;
        uRet              = uvcd_pu_set_powerline(&set);
    }
    return uRet;
}

UINT32 AmbaUSBD_UVCGetProbeMaxPayloadXferSize(void)
{
    return uvcd_get_probe_max_payload_xfer_size();
}

VOID AmbaUSBD_UVC_SetProbeMaxPayloadXferSize(UINT32 Size)
{
    uvcd_set_probe_max_payload_xfer_size(Size);
}

static UINT8 _uvc_is_vc_desc(const UINT8 *desc)
{
    UINT8 uRet = 0;

    // At least one video stream
    if ((desc[0] >= 0x0DU) && (desc[1] == UVC_DESC_TYPE_CS_INTF) && (desc[2] == UVC_DESC_SUBTYPE_VC_HEADER)) {
        uRet = 1;
    }
    return uRet;
}

static UINT8 _uvc_is_vs_desc(const UINT8 *desc)
{
    UINT8 uRet = 0;

    if ((desc[0] == 0x0EU) && (desc[1] == UVC_DESC_TYPE_CS_INTF) && (desc[2] == UVC_DESC_SUBTYPE_VS_INPUT_HEADER)) {
        uRet = 1;
    } else if ((desc[0] == 0x0FU) && (desc[1] == UVC_DESC_TYPE_CS_INTF) && (desc[2] == UVC_DESC_SUBTYPE_VS_INPUT_HEADER)) {
        uRet = 1;
    } else {
        // pass vcast
    }
    return uRet;
}

static UINT8 _uvc_is_desc_type(const UINT8 *desc, UINT8 type, UINT8 subtype)
{
    UINT8 uRet = 0;

    if ((desc[1] == type) && (desc[2] == subtype)) {
        uRet = 1;
    }
    return uRet;
}

struct _uvc_desc_type {
    UINT8 type;
    UINT8 subtype;
};

static UINT8 _uvc_is_vc_desc_type(const UINT8 *desc)
{
    static struct _uvc_desc_type uvc_vc_desc_types[] =
    {
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VC_OUTPUT_TERMINAL },   // output terminal
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VC_EXTENSION_UNIT },    // extension unit
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VC_INPUT_TERMINAL },    // input terminal
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VC_PROCESSING_UNIT },   // Processing Unit
    };

    static UINT32 uvc_vc_desc_types_size = sizeof(uvc_vc_desc_types) / sizeof(struct _uvc_desc_type);

    UINT8 i;
    UINT8 uRet = 0;

    for (i = 0; i < uvc_vc_desc_types_size; i++) {
        const struct _uvc_desc_type *t = &uvc_vc_desc_types[i];
        if (_uvc_is_desc_type(desc, t->type, t->subtype) == 1U) {
            uRet = 1;
        }
    }
    return uRet;
}

static UINT8 _uvc_is_vc_input_terminal_desc(const UINT8 *desc)
{
    UINT8 uRet = 0;

    if ((desc[1] == UVC_DESC_TYPE_CS_INTF) && (desc[2] == UVC_DESC_SUBTYPE_VC_INPUT_TERMINAL)) {
        uRet = 1;
    }
    return uRet;
}

static UINT8 _uvc_is_vc_processing_unit_desc(const UINT8 *desc)
{
    UINT8 uRet = 0;

    if ((desc[1] == UVC_DESC_TYPE_CS_INTF) && (desc[2] == UVC_DESC_SUBTYPE_VC_PROCESSING_UNIT)) {
        uRet = 1;
    }
    return uRet;
}

static UINT8 _uvc_is_vc_extension_unit_desc(const UINT8 *desc)
{
    UINT8 uRet = 0;

    if ((desc[1] == UVC_DESC_TYPE_CS_INTF) && (desc[2] == UVC_DESC_SUBTYPE_VC_EXTENSION_UNIT)) {
        uRet = 1;
    }
    return uRet;
}

static UINT8 _uvc_is_vs_desc_type(const UINT8 *desc)
{
    static struct _uvc_desc_type uvc_vs_desc_types[] =
    {
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FORMAT_MJPEG },
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FRAME_MJPEG },
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FORMAT_UNCOMPRESSED },
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FRAME_UNCOMPRESSED },
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FORMAT_H264 },
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FRAME_H264 },
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED },
        { UVC_DESC_TYPE_CS_INTF, UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED },
    };

    static UINT32 uvc_vs_desc_types_size = sizeof(uvc_vs_desc_types) / sizeof(struct _uvc_desc_type);

    UINT8 i;
    UINT8 uRet = 0;

    for (i = 0; i < uvc_vs_desc_types_size; i++) {
        const struct _uvc_desc_type *t = &uvc_vs_desc_types[i];
        if (_uvc_is_desc_type(desc, t->type, t->subtype) == 1U) {
            uRet = 1;
        }
    }
    return uRet;
}

UINT32 usbd_uvc_desc_patch(UINT8 *whole_desc, UINT32 desc_size, UINT32 it_attribute, UINT32 pu_attribute)
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
    UINT32 uRet = 0;

    // step to configuration descriptor
    while (desc[i] > 0U) {
        if ((desc[i] == USB_DESC_LENGTH_CFG) && (desc[i + 1U] == USB_DESC_TYPE_CFG)) {
            break;
        }
        length = desc[i];
        i     += length;
    }
    AmbaUSB_Utility_PrintUInt5("    config desc pos = %d.", i, 0, 0, 0, 0);

    desc_size  -= i;
    desc        = &whole_desc[i];
    config_desc = desc;

    do {
        // first byte should be desc length
        length = desc[count];
        if (length == 0U) {
            AmbaUSB_Utility_Print("[Error] descriptor size should not be zero.");
            AmbaUSB_Utility_PrintUInt5("[Error]     @ position %d.", count, 0, 0, 0, 0);
            uRet = UX_ERROR;
            break;
        } else {
            AmbaUSB_Utility_PrintUInt5("    desc size = %d.", length, 0, 0, 0, 0);

            if ((_uvc_is_vc_desc(&desc[count]) == 1U) && (vc_desc_pos == 0U)) {
                vc_desc_pos           = count;
                vc_desc_total_length += length;
                vc_desc_scan_start    = 1;
                AmbaUSB_Utility_PrintUInt5("[uvc] vc_desc_pos = %d.", vc_desc_pos, 0, 0, 0, 0);
            } else if ((_uvc_is_vs_desc(&desc[count]) == 1U) && (vs_desc_pos == 0U)) {
                vs_desc_pos           = count;
                vs_desc_total_length += length;
                vc_desc_scan_start    = 0;
                vs_desc_scan_start    = 1;
                AmbaUSB_Utility_PrintUInt5("[uvc] vs_desc_pos = %d.", vs_desc_pos, 0, 0, 0, 0);
            } else if ((_uvc_is_vc_desc_type(&desc[count]) == 1U) && (vc_desc_scan_start == 1U)) {
                vc_desc_total_length += length;
                AmbaUSB_Utility_Print("[uvc] vc desc type.");
                if (_uvc_is_vc_input_terminal_desc(&desc[count]) != 0U) {
                    // patch VC Input Terminal's bmControls field
                    desc[count + 15U] = (UINT8)(it_attribute & 0x0FFU);
                    desc[count + 16U] = (UINT8)((it_attribute >> 8U) & 0x0FFU);
                    desc[count + 17U] = (UINT8)((it_attribute >> 16U) & 0x0FFU);
                    uRet              = uvcd_set_input_terminal_id(desc[count + 3U]);
                } else if (_uvc_is_vc_processing_unit_desc(&desc[count]) != 0U) {
                    // patch VC Processing Unit's bmControls field
                    desc[count + 8U] = (UINT8)(pu_attribute & 0x0FFU);
                    desc[count + 9U] = (UINT8)((pu_attribute >> 8U) & 0x0FFU);
                    uRet             = uvcd_set_processing_unit_id(desc[count + 3U]);
                } else if (_uvc_is_vc_extension_unit_desc(&desc[count]) != 0U) {
                    uRet = uvcd_set_extension_unit_id(desc[count + 3U]);
                } else {
                    // pass vcast
                }
            } else if ((_uvc_is_vs_desc_type(&desc[count]) == 1U) && (vs_desc_scan_start == 1U)) {
                vs_desc_total_length += length;
                AmbaUSB_Utility_Print("[uvc] vs desc type.");
            } else if ((vs_desc_scan_start == 1U) &&
                       (desc[count] == USB_DESC_LENGTH_ENDPOINT) &&
                       (desc[count + 1U] == USB_DESC_TYPE_ENDPOINT)) {
                // Assume that the endpont desc means the end of VS desc.
                vs_desc_scan_start = 0;
                AmbaUSB_Utility_PrintUInt5("vs_desc_total_length = 0x%X.", vs_desc_total_length, 0, 0, 0, 0);
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
        AmbaUSB_Utility_Print("[Error] descriptor size mismatch.");
        AmbaUSB_Utility_PrintUInt5("[Error]    real size is %d, but calculated size is %d.", desc_size, count, 0, 0, 0);
        uRet = UX_ERROR;
    } else {
        AmbaUSB_Utility_PrintUInt5("uvc configuration desc length = 0x%X.", total_length, 0, 0, 0, 0);
        AmbaUSB_Utility_PrintUInt5("vc_desc_total_length = 0x%X.", vc_desc_total_length, 0, 0, 0, 0);

        config_desc[2] = (UINT8)(total_length & 0x0FFU);
        config_desc[3] = (UINT8)((total_length >> 8U) & 0x0FFU);

        config_desc[vc_desc_pos + 5U] = (UINT8)(vc_desc_total_length & 0x0FFU);
        config_desc[vc_desc_pos + 6U] = (UINT8)((vc_desc_total_length >> 8U) & 0x0FFU);
    }
    return uRet;
}

