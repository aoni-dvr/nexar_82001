/**
 *  @file AmbaUSBD_CdcAcm.c
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
 *  @details USB wrapper functions for CDC-ACM Device Class
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <usbx/ux_device_stack.h>
#include <AmbaUSBD_CdcAcm.h>
#include <AmbaUSB_Utility.h>
#include <AmbaUSB_ErrCode.h>

static UX_SLAVE_CLASS_CDC_ACM *cdc_instance = NULL;

static UX_SLAVE_CLASS_CDC_ACM *voidp_to_instancep(const void* VoidPtr)
{
    const void *src_ptr = VoidPtr;
    UX_SLAVE_CLASS_CDC_ACM *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(void*));

    return ptr_ret;
}

static void instance_activate(const void *Instance)
{
    /* Save the CDC instance. */
    cdc_instance = voidp_to_instancep(Instance);
    USB_UtilityPrint(USB_PRINT_FLAG_INFO, "ACM Active");
}

static void instance_deactivate(const void *Instance)
{
    (void)Instance;
    /* Reset the CDC instance. */
    cdc_instance = UX_NULL;
    USB_UtilityPrint(USB_PRINT_FLAG_INFO, "ACM DEActive");
}

UINT32 USBD_CdcAcmStackRegister(UINT32 ConfigurationIndex, UINT32 IntfaceIndex)
{
    UINT32 uret;
    static UX_SLAVE_CLASS_CDC_ACM_PARAMETER cdc_param = { NULL };
    static UCHAR uxd_cdc_acm_name[] =  "ux_slave_class_cdc_acm";

    /* Set the parameters for callback when insertion/extraction of a CDC device.  */
    cdc_param.instance_activate   = instance_activate;
    cdc_param.instance_deactivate = instance_deactivate;

    // register device class
    uret = _ux_device_stack_class_register(uxd_cdc_acm_name,
                                           _ux_device_class_cdc_acm_entry,
                                           ConfigurationIndex,
                                           IntfaceIndex,
                                           &cdc_param);

    if (uret != 0U) {
        uret = USBX_ERR_CLASS_REGISTER_FAIL;
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBD_CdcAcmStackRegister(): Failed to init USBX device class with error %x\n", uret, 0, 0, 0, 0);
    } else {
        // print debug message
        USB_UtilityPrint(USB_PRINT_FLAG_INFO, "USBD_CdcAcmStackRegister(): Finish init USBX device class");
    }

    return uret;
}
/** \addtogroup Public_APIs
 *  @{
 * */

/**
 * This function checks if the terminal is opened by the Host.
 * @param pIsOpen [Output] Indicate if the terminal is opened by Host:
- 0: The terminal is NOT opened by the Host
- 1: The terminal is opened by the Host.
*/
UINT32 AmbaUSBD_CDCACMIsTerminalOpen(UINT32 *pIsOpen)
{
    UINT32 uret = USB_ERR_SUCCESS;
    if (pIsOpen != NULL) {
        if ((udc_cdc_acm_get_control_bitmap(0) & 0x03U) == 0x03U) {
            *pIsOpen = 1;
        } else {
            *pIsOpen = 0;
        }
    } else {
        uret = USB_ERR_PARAMETER_INVALID;
    }
    return uret;
}
/**
 * This function request the USB CDC ACM class driver to send data to the USB host within the speciﬁed timeout.
 * @param pBuffer [Input] The buffer to write
 * @param RequestLength [Input] The data size for write
 * @param pActualLength [Output] The actual data size written to Host
 * @param Timeout [Input] Timeout for transfer. 0 menas "No Wait". 0xFFFFFFFF means "Wait Forever".
*/
UINT32 AmbaUSBD_CDCACMWrite(UCHAR *pBuffer, ULONG RequestLength, ULONG *pActualLength, ULONG Timeout)
{
    UINT32 uret = 0;

    if ((pBuffer == NULL) || (pActualLength == NULL)) {
        uret = USB_ERR_PARAMETER_NULL;
    } else {
        if (cdc_instance != NULL) {
            uret = _ux_device_class_cdc_acm_write_ex(cdc_instance, pBuffer, RequestLength, pActualLength, Timeout);
            if (uret == (UINT32)TX_NO_INSTANCE) {
                uret = USB_ERR_TRANSFER_TIMEOUT;
            } else if ((uret == (UINT32)UX_TRANSFER_BUS_RESET) || (uret == (UINT32)UX_CACHE_RANGE_ERROR)) {
                uret = USB_ERR_TRANSFER_BUS_RESET;
            } else {
                // pass vcast
            }
        } else {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "AmbaUSBD_CDC_ACM_Write(): Instance is null");
            uret = USB_ERR_NO_INSTANCE;
        }
    }

    return uret;
}

/**
 * This function request the USB CDC ACM class driver to receive data from the USB host within the speciﬁed timeout.
 * pBuffer    [Input] The buffer to read
 * @param RequestLength [Input] The data size for read
 * @param pActualLength [Output] The actual data size read from Host
 * @param Timeout [Input] Timeout for transfer. 0 menas "No Wait". 0xFFFFFFFF means "Wait Forever".
*/
UINT32 AmbaUSBD_CDCACMRead(UCHAR *pBuffer, ULONG RequestLength, ULONG *pActualLength, ULONG Timeout)
{
    UINT32 uret = 0;

    if ((pBuffer == NULL) || (pActualLength == NULL)) {
        uret = USB_ERR_PARAMETER_NULL;
    } else {
        if (cdc_instance != NULL) {
            uret = _ux_device_class_cdc_acm_read_ex(cdc_instance, pBuffer, RequestLength, pActualLength, Timeout);
            if (uret == (UINT32)TX_NO_INSTANCE) {
                uret = USB_ERR_TRANSFER_TIMEOUT;
            } else if ((uret == (UINT32)UX_TRANSFER_BUS_RESET) || (uret == (UINT32)UX_CACHE_RANGE_ERROR)) {
                uret = USB_ERR_TRANSFER_BUS_RESET;
            } else {
                // pass vcast
            }
        } else {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "AmbaUSBD_CDC_ACM_Read(): Instance is null");
            uret = USB_ERR_NO_INSTANCE;
        }
    }

    return uret;
}

/*!
* This function is used for USB device system to start CDCACM class. Applications should pass it as ClassStartCb parameter when calling AmbaUSBD_SystemClassStart().
* @param pDescInfo [Input] Descriptor information.
*/
UINT32 AmbaUSBD_CDCACMClassStart(const USBD_DESC_CUSTOM_INFO_s *pDescInfo)
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
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_CDCACMClassStart(): Failed to init SS device framework with error 0x%x", uret, 0, 0, 0, 0);
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
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_CDCACMClassStart(): Failed to init USBX CDC-ACM device framework with error %x\n", uret, 0, 0, 0, 0);
                uret = USB_UtilityUXCode2AmbaCode(uret);
            } else {
                USB_UtilityPrint(USB_PRINT_FLAG_INFO, "AmbaUSBD_CDCACMClassStart(): Finish init USBX CDC-ACM device framework");
                // start class
                uret = USBD_CdcAcmStackRegister(1, 0);
            }
        }
    }
    return uret;
}
/**
 * This function is used for USB device system to stop CDCACM class.
 * Applications should pass it as ClassStopCb parameter when calling AmbaUSBD_ClassStart().
*/
UINT32 AmbaUSBD_CDCACMClassStop(void)
{
    UINT32 uret;

    if (USBD_SystemIsClassConfiged() != USB_ERR_SUCCESS) {
        uret = USB_ERR_NO_INIT;
    } else {

        uret = _ux_device_stack_class_clear();

        if (uret != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "usbd_cdc_acm_stop(): Failed to clear CDC-ACM class (0x%X)", uret, 0, 0, 0, 0);
            uret = USBX_ERR_CLASS_DEINIT_FAIL;
        } else {
            // print debug message
            USB_UtilityPrint(USB_PRINT_FLAG_INFO, "usbd_cdc_acm_stop(): CDC-ACM class clear successfully.");
        }

        if (uret == (UINT32)UX_SUCCESS) {
            // free device stack
            uret = _ux_device_stack_free();
            if (uret != ((UINT32)UX_SUCCESS)) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_CDCACMClassStop(): Failed to free USBX device class with error %x\n", uret, 0, 0, 0, 0);
                uret = USBX_ERR_CLASS_DEINIT_FAIL;
            }
        }
    }
    return uret;
}
/** @} */

