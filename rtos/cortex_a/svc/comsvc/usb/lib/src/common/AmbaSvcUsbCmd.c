/**
 *  @file AmbaSvcUsbCmd.c
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
 *  @details Ambarella USB Shell APIs
 */

#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaShell_Commands.h"

#include "AmbaSvcUsb.h"

#define USB_CMD_NG      (1U)

static void USB_TestUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("USB device test command:\n");
    PrintFunc("  init_device [item] : Legacy USB test cases\n");
    PrintFunc("        msc       : start MSC device class\n");
    PrintFunc("        mtp       : start MTP device class\n");
}

static UINT32 AppUsb_TestCmd(UINT32 Argc, char * const * Argv)
{
    static char  MainDrive = 'c';

    UINT32  RetVal = OK;

    if (2U <= Argc) {
        if (AmbaUtility_StringCompare(Argv[1], "init_device", 11) == 0) {
            if (4U <= Argc) {
                if ((Argv[3][0] == 'c') || (Argv[3][0] == 'C') ||
                    (Argv[3][0] == 'd') || (Argv[3][0] == 'D')) {
                    MainDrive = Argv[3][0];
                } else {
                    /* do nothing */
                }
            }

            if (AmbaUtility_StringCompare(Argv[2], "msc", 3) == 0) {
                if (AmbaFS_UnMount(MainDrive) == OK) {
                    if (AmbaSvcUsb_DeviceSysInit() == OK) {
                        AmbaSvcUsb_DeviceClassSet(AMBA_USBD_CLASS_MSC);
                        AmbaSvcUsb_DeviceClassMscLoad(MainDrive);
                        AmbaSvcUsb_DeviceClassStart();
                    }
                }
            } else if (AmbaUtility_StringCompare(Argv[2], "mtp", 3) == 0) {
                if (AmbaSvcUsb_DeviceSysInit() == OK) {
                    AmbaSvcUsb_DeviceClassSet(AMBA_USBD_CLASS_MTP);
                    if (AmbaSvcUsb_DeviceClassMtpInit(MainDrive) != OK){
                        /* do nothing */
                    }
                    AmbaSvcUsb_DeviceClassStart();
                }
            } else {
                RetVal = USB_CMD_NG;
            }
        } else if (AmbaUtility_StringCompare(Argv[1], "free_device", 11) == 0) {
            AmbaSvcUsb_DeviceClassStop();
            AmbaSvcUsb_DeviceClassMscUnload(MainDrive);
            AmbaSvcUsb_DeviceSysDeInit();
            if (AmbaFS_Mount(MainDrive) != OK) {
                AmbaPrint_PrintUInt5("## fail to mount drive", 0, 0, 0, 0, 0);
            }
        } else if (AmbaUtility_StringCompare(Argv[1], "init_host", 9) == 0) {
            if (AmbaUtility_StringCompare(Argv[2], "msc", 3) == 0) {
                AMBA_SVC_USBH_s  HostMSC;

                HostMSC.SelectHcd = AMBA_USB_HOST_HCD;
                HostMSC.OCPolarity = 0U;
                HostMSC.Phy0Owner = 1U;
                HostMSC.pfnCallback = NULL;
                AmbaSvcUsb_HostClassStart(&HostMSC);
            } else {
                /* Do nothing */
            }
        } else {
            RetVal = USB_CMD_NG;
        }
    } else {
        RetVal = USB_CMD_NG;
    }

    return RetVal;
}

/**
* shell command entry of usb device/host
* @param [in] ArgCount count of argument
* @param [in] pArgVector array of argument
* @param [in] PrintFunc print function of shell
* @return none
*/
void AmbaShell_CommandUsb(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (AppUsb_TestCmd(ArgCount, pArgVector) != OK) {
        USB_TestUsage(PrintFunc);
    }
}
