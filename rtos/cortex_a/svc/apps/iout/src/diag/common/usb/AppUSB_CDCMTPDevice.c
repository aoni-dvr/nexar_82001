/**
 *  @file AppUSB_CDCMTPDevice.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details USB CDC + MTP Class with single instance for APP/MW.
 */
/**
 * \page page1 CDC + MTP Device application
*/
#include "AppUSB.h"
#ifndef APPUSB_UTIL_H
#include "AppUSB_Util.h"
#endif
#include "AppUSB_SoftwareFifo.h"
#include "AmbaUSB_ErrCode.h"
#include <AmbaMisraFix.h>

extern UINT32 USBD_MtpStackRegister(UINT32 Configuration, UINT32 InterfaceIndex);

/*!
* This function is used for USB device system to start CDC + MTP class. Applications should pass it as ClassStartCb parameter when calling AmbaUSBD_SystemClassStart().
* @param pDescInfo [Input] Descriptor information.
*/
UINT32 AppCdcMtpd_ClassStart(const USBD_DESC_CUSTOM_INFO_s *pDescInfo)
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

    } else {

        if (pDescInfo->DescFrameworkSs != NULL) {
            // call _ux_device_stack_initialize for SS
            uret =  _ux_device_stack_ss_initialize(pDescInfo->DescFrameworkSs, pDescInfo->DescSizeSs);
        } else {
            uret = 0;
        }
        if (uret != 0U) {
            AppUsb_PrintUInt5("AppCdcMtpd_ClassStart(): Failed to init SS device framework with error 0x%x", uret, 0, 0, 0, 0);
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
                AppUsb_PrintUInt5("AppCdcMtpd_ClassStart(): Failed to init USBX CDC-ACM device framework with error %x\n", uret, 0, 0, 0, 0);
            } else {
                // start class
                uret = AppCdcAcmMultid_StackRegister(1, 0);
                if (uret == 0U) {
                    uret = USBD_MtpStackRegister(1, 4);
                    if (uret == 0U) {
                        AppUsb_Print("AppCdcMtpd_ClassStart(): Finish init USBX CDC-ACM device framework");
                    } else {
                        AppUsb_Print("AppCdcMtpd_ClassStart(): Failed to register MTP protocol stack.");
                    }
                } else {
                    AppUsb_Print("AppCdcMtpd_ClassStart(): Failed to register CdcAcm protocol stack.");
                }
            }
        }
    }
    return uret;
}
/**
 * This function is used for USB device system to stop CDC + MSC class.
 * Applications should pass it as ClassStopCb parameter when calling AmbaUSBD_ClassStart().
*/
UINT32 AppCdcMtpd_ClassStop(void)
{
    UINT32 uret;

    uret = _ux_device_stack_class_clear();

    if (uret != 0U) {
        AppUsb_PrintUInt5("AppCdcMtpd_ClassStop(): Failed to clear CDC + MTP class (0x%X)", uret, 0, 0, 0, 0);
        uret = USBX_ERR_CLASS_DEINIT_FAIL;
    } else {
        // print debug message
        AppUsb_Print("AppCdcMtpd_ClassStop(): CDC + MTP class clear successfully.");
    }

    if (uret == (UINT32)UX_SUCCESS) {
        // free device stack
        uret = _ux_device_stack_free();
        if (uret != ((UINT32)UX_SUCCESS)) {
            AppUsb_PrintUInt5("AppCdcMtpd_ClassStop(): Failed to free USBX device class with error %x\n", uret, 0, 0, 0, 0);
            uret = USBX_ERR_CLASS_DEINIT_FAIL;
        }
    }

    return uret;
}

// -------------------------------------------------------
//
// Application code
//
//--------------------------------------------------------

UINT32 AppCdcMtpd_Start(void)
{
    UINT32 uret = AppCdcAcmMultid_Start();
    if (uret == 0U) {
        if (AppMtpd_Init() != 0U) {
            AppUsb_PrintUInt5("AppCdcMtpd_Start(): can't init MTP device, 0x%X", uret, 0, 0, 0, 0);
        }
    }
    return 0;
}
UINT32 AppCdcMtpd_Stop(void)
{
    return 0;
}

