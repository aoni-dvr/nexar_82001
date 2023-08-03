/**
 *  @file AmbaUSBD_Descriptor.c
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
 *  @details USB Device Descriptor configuration.
 */
#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSBD_Descriptor.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSB_Utility.h>

//#ifdef AMBA_USB_DEBUG

static USBD_DESC_CUSTOM_INFO_s desc_custom_info = { NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0 };

#define DESC_CONFIG_NUM_OFFSET 17
#define DESC_CONFIG_LEN_OFFSET 2

#define DESC_SIZE_OFFSET   0
#define DESC_TYPE_OFFSET   1

#define DESC_TYPE_DEVICE   0x01
#define DESC_TYPE_DEVICE_Q 0x06
#define DESC_TYPE_CONFIG   0x02
#define DESC_TYPE_ENDPOINT 0x05
#define DESC_TYPE_BOS      0x0F

#define MAX_CONFIGURATION  ((UINT32)2UL)

typedef struct {
    UINT8 *LenPtr;
    UINT32 Length;
} DESC_NUM_s;

#define  DESC_DEVICE_LENGTH  (18U)
#define  DESC_DEVICEQ_LENGTH (10U)
#define  DESC_CONFIG_LENGTH  (9U)

static void recalculate_config_length_fs(const USBD_DESC_CUSTOM_INFO_s *info)
{
    UINT8 *cur_ptr = NULL;
    UINT32 config_num;
    UINT32 i;
    UINT32 desc_type = 0;
    UINT32 offset = 0;
    UINT8  max_pkt_size = 0;

    DESC_NUM_s config_array[MAX_CONFIGURATION];

    // Vcast check for misra c 2012 rule 9.1
    for (i = 0; i < MAX_CONFIGURATION; i++) {
        config_array[i].LenPtr = NULL;
        config_array[i].Length = 0;
    }

    config_num = info->DescFrameworkFs[DESC_CONFIG_NUM_OFFSET];

    // Reset the index i.
    i = 0;
    while (info->DescSizeFs > offset) {
        if (i > config_num) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "recalculate_config_length_fs(): FS Configuration Num error!!");
            break;
        }

        cur_ptr   = &(info->DescFrameworkFs[offset]);
        desc_type = cur_ptr[DESC_TYPE_OFFSET];

        switch (desc_type) {
        case DESC_TYPE_DEVICE:
            // check maximum packet size, it must be 64
            max_pkt_size = cur_ptr[7];
            if (max_pkt_size != 64U) {
                // print warning messages
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "-----------------------------------------------");
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[Error] FS Maximum Pakcet Size (%d) for Control Endpoint must be 64.", max_pkt_size, 0, 0, 0, 0);
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "Please check your USB Device descriptor.");
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "-----------------------------------------------");
            }
            offset += DESC_DEVICE_LENGTH;
            break;
        case DESC_TYPE_DEVICE_Q:
            offset += DESC_DEVICEQ_LENGTH;
            break;
        case DESC_TYPE_CONFIG:
            if (i < MAX_CONFIGURATION) {
                config_array[i].LenPtr = &cur_ptr[DESC_CONFIG_LEN_OFFSET];
                config_array[i].Length = DESC_CONFIG_LENGTH;
                offset               += DESC_CONFIG_LENGTH;
                i++;
            } else {
                // exception case
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Configuration Num %d is larger than %d", i, MAX_CONFIGURATION, 0, 0, 0);
            }
            break;
        default:
            if (i > 0U) {
                UINT32 ConfigIdx = i - 1U;
                if (ConfigIdx < MAX_CONFIGURATION) {
                    config_array[ConfigIdx].Length += (UINT32)cur_ptr[DESC_SIZE_OFFSET];
                } else {
                    // exception case
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Configuration Num %d is larger than %d", ConfigIdx, MAX_CONFIGURATION, 0, 0, 0);
                }
            }
            offset += cur_ptr[DESC_SIZE_OFFSET];
            break;
        }
    }

    for (i = 0; i < config_num; i++) {
#ifdef AMBA_USB_DEBUG
#ifdef  USB_PRINT_SUPPORT
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "Config[%d] addr = 0x%x, length = %d", i, config_array[i].LenPtr, config_array[i].length, 0, 0);
#endif
#endif

        _ux_utility_short_put((UCHAR*)config_array[i].LenPtr, (USHORT)config_array[i].Length);
    }

    return;
}

static void recalculate_config_length_hs(const USBD_DESC_CUSTOM_INFO_s *info)
{
    UINT8 *cur_ptr = NULL;
    UINT32 config_num, i;
    UINT32 desc_type = 0, offset;
    UINT8  ep_attribute, ep_interval;

    DESC_NUM_s config_array[MAX_CONFIGURATION];

    // Vcast check for misra c 2012 rule 9.1
    for (i = 0; i < MAX_CONFIGURATION; i++) {
        config_array[i].LenPtr = NULL;
        config_array[i].Length = 0;
    }

    config_num = info->DescFrameworkHs[DESC_CONFIG_NUM_OFFSET];
    i         = 0;
    offset    = 0;

    while (info->DescSizeHs > offset) {
        if (i > config_num) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "recalculate_config_length_hs(): HS Configuration Num error!!");
            break;
        }

        cur_ptr  = &(info->DescFrameworkHs[offset]);
        desc_type = cur_ptr[DESC_TYPE_OFFSET];

        switch (desc_type) {
        case DESC_TYPE_DEVICE:
            offset += DESC_DEVICE_LENGTH;
            break;
        case DESC_TYPE_DEVICE_Q:
            offset += DESC_DEVICEQ_LENGTH;
            break;
        case DESC_TYPE_CONFIG:
            config_array[i].LenPtr = &cur_ptr[DESC_CONFIG_LEN_OFFSET];
            config_array[i].Length = DESC_CONFIG_LENGTH;
            offset               += DESC_CONFIG_LENGTH;
            i++;
            break;
        default:
            if (desc_type == (UINT32)DESC_TYPE_ENDPOINT) {
                ep_attribute = cur_ptr[3] & 0x03U;
                ep_interval  = cur_ptr[6];
                if ((ep_attribute == 0x3U) && (ep_interval < 4U)) {
                    // pollo - 2014/08/25
                    // If the interrupt interval is 1 for High Speed, there are chances that
                    // "NO-ACK" for this endpoint. This should be a Hardware Limitation.
                    UINT8 EpAddress   = cur_ptr[2];
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "\n\n[Warning] The Interval (%d) of Interrupt Endpoint 0x%X  should be > 4 due to HW limitation.\n\n", ep_interval, EpAddress, 0, 0, 0);
                }
            }
            if (i > 0U) {
                config_array[i - 1U].Length += cur_ptr[DESC_SIZE_OFFSET];
            }
            offset += cur_ptr[DESC_SIZE_OFFSET];
            break;
        }
    }

    for (i = 0; i < config_num; i++) {
#ifdef AMBA_USB_DEBUG
#ifdef  USB_PRINT_SUPPORT
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "Config[%d] addr = 0x%x, length = %d", i, config_array[i].LenPtr, config_array[i].length, 0, 0, 0);
#endif
#endif
        _ux_utility_short_put((UCHAR*)config_array[i].LenPtr, (USHORT)config_array[i].Length);
    }

    return;
}

static void recalculate_config_length_ss(const USBD_DESC_CUSTOM_INFO_s *info)
{
    UINT8 *cur_ptr = NULL;
    UINT32 config_num = 0;
    UINT32 i;
    UINT32 desc_type = 0;
    UINT32 offset;
    UINT32 desc_len;

    DESC_NUM_s config_array[MAX_CONFIGURATION];

    // Vcast check for misra c 2012 rule 9.1
    for (i = 0; i < MAX_CONFIGURATION; i++) {
        config_array[i].LenPtr = NULL;
        config_array[i].Length = 0;
    }

    // Calculate the configuration length of SS descriptor.
    config_num = info->DescFrameworkSs[DESC_CONFIG_NUM_OFFSET];
    i          = 0;
    offset     = 0;

    while (info->DescSizeSs > offset) {

        if (i > config_num) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "recalculate_config_length_ss(): SS Configuration Num error!!");
            break;
        }

        cur_ptr   = &info->DescFrameworkSs[offset];
        desc_type = cur_ptr[DESC_TYPE_OFFSET];
        //AmbaPrint("desc_type = 0x%x, offset = %d", desc_type, offset);
        switch (desc_type) {
        case DESC_TYPE_DEVICE:
            offset += DESC_DEVICE_LENGTH;
            break;
        case DESC_TYPE_BOS:
            //offset += BOS_DESCRIPTOR_LENGTH_SS;
            desc_len = cur_ptr[2];
            desc_len += ((UINT32)cur_ptr[3] << 8U);
            offset += desc_len;
            break;
        case DESC_TYPE_CONFIG:
            config_array[i].LenPtr = &cur_ptr[DESC_CONFIG_LEN_OFFSET];
            config_array[i].Length = DESC_CONFIG_LENGTH;
            offset                += DESC_CONFIG_LENGTH;
            i++;
            break;
        default:
            if (i > 0U) {
                config_array[i-1U].Length += cur_ptr[DESC_SIZE_OFFSET];
            }
            offset += cur_ptr[DESC_SIZE_OFFSET];
            break;
        }
    }

    for (i = 0; i < config_num; i++) {
#ifdef AMBA_USB_DEBUG
#ifdef  USB_PRINT_SUPPORT
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "Config[%d] addr = 0x%08x%08x, length = %d", i, config_array[i].LenPtr, config_array[i].Length, 0, 0);
#endif
#endif

        _ux_utility_short_put((UCHAR *)config_array[i].LenPtr, (USHORT)config_array[i].Length);
    }

    return;
}


static void recalculate_config_length(const USBD_DESC_CUSTOM_INFO_s *info)
{
    if (info->DescFrameworkFs != NULL) {
        recalculate_config_length_fs(info);
    }
    if (info->DescFrameworkHs != NULL) {
        recalculate_config_length_hs(info);
    }
    if (info->DescFrameworkSs != NULL) {
        recalculate_config_length_ss(info);
    }
    return;
}

static UINT32 check_string(const UINT8 *framework, UINT32 size)
{
    UINT32 pos = 0;
    UINT8 len;
    UINT8 i;
    UINT32 status = USB_ERR_SUCCESS;

    while (pos < size) {
        pos += 2U; // skip language ID, 2 bytes
        pos++;
        len  = framework[pos];
        pos++;
        if ((len + pos) > size) {
            break;
        }
        for (i = 0; i < len; i++) {
            if (framework[pos] == 0U) {
                status = USB_ERR_FAIL;
            }
            pos++;
        }
    }

    return status;
}
/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Retrieve current USB descriptor information. The wTotalLength field in Configuration descriptor will be recalculated and filled.
 * */
void USBD_DescriptorInfoGet(USBD_DESC_CUSTOM_INFO_s *Info)
{
    /* Apply Customer StrFramework */
    Info->StrFramework = desc_custom_info.StrFramework;
    Info->StrSize      = desc_custom_info.StrSize;

    // pollo - 2014/08/01 - Compatibility issue with Windows 8.
    // If a string contains 0x0, then Windows 8 will refuse to recognized this device.
    // Just print some messages for RD to debug easily.
    if (check_string(Info->StrFramework, Info->StrSize) == USB_ERR_FAIL) {
        // print warning messages
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_DescriptorInfoGet(): USB string descritpors contain 0x0, some HOST can't accept this.");
    }

    Info->LangIDFramework = desc_custom_info.LangIDFramework;;
    Info->LangIDSize      = desc_custom_info.LangIDSize;

    Info->DescFrameworkFs = desc_custom_info.DescFrameworkFs;
    Info->DescSizeFs      = desc_custom_info.DescSizeFs;
    Info->DescFrameworkHs = desc_custom_info.DescFrameworkHs;
    Info->DescSizeHs      = desc_custom_info.DescSizeHs;
    Info->DescFrameworkSs = desc_custom_info.DescFrameworkSs;
    Info->DescSizeSs      = desc_custom_info.DescSizeSs;
    /* Check and fill the calculated total configuration length. */
    recalculate_config_length(Info);

    return;
}
/**
 * Clear current USB descriptor information.
 * */
void USBD_DescriptorInfoClear(void)
{
    desc_custom_info.DescFrameworkFs = NULL;
    desc_custom_info.DescFrameworkHs = NULL;
    desc_custom_info.DescFrameworkSs = NULL;
    desc_custom_info.StrFramework    = NULL;
    desc_custom_info.LangIDFramework = NULL;
    desc_custom_info.DescSizeFs      = 0;
    desc_custom_info.DescSizeHs      = 0;
    desc_custom_info.DescSizeSs      = 0;
    desc_custom_info.StrSize         = 0;
    desc_custom_info.LangIDSize      = 0;
    return;
}

/**
 * Set current USB descriptor information.
 * */
UINT32 USBD_DescriptorInit(const USBD_DESC_CUSTOM_INFO_s *Info)
{
    UINT32 rval = USB_ERR_SUCCESS;

    if (Info == NULL) {
        rval = USB_ERR_PARAMETER_NULL;
    } else if ((Info->DescFrameworkFs == NULL) || (Info->DescFrameworkHs == NULL) || (Info->StrFramework == NULL) || (Info->LangIDFramework == NULL)) {
        rval = USB_ERR_PARAMETER_NULL;
    } else {
        desc_custom_info.DescFrameworkFs = Info->DescFrameworkFs;
        desc_custom_info.DescFrameworkHs = Info->DescFrameworkHs;
        desc_custom_info.DescFrameworkSs = Info->DescFrameworkSs;
        desc_custom_info.StrFramework    = Info->StrFramework;
        desc_custom_info.LangIDFramework = Info->LangIDFramework;
        desc_custom_info.DescSizeFs      = Info->DescSizeFs;
        desc_custom_info.DescSizeHs      = Info->DescSizeHs;
        desc_custom_info.DescSizeSs      = Info->DescSizeSs;
        desc_custom_info.StrSize         = Info->StrSize;
        desc_custom_info.LangIDSize      = Info->LangIDSize;
    }

    return rval;
}
/** @} */
