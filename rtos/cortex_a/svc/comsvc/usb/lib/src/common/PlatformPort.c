/**
 *  @file PlatformPort.c
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
 *  @details USB Platform Dependent functions for APP/MW.
 */

#include "AmbaPrint.h"
#include "AmbaSvcUsb.h"
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaFS.h>
#include <AmbaGPIO_Def.h>
#include <AmbaGPIO.h>
#include "AmbaShell.h"
#include "AmbaSvcWrap.h"

#include "PlatformPort.h"

#define APP_SLOT_SD0                    2   /* SD/MMC controller: (C) */
#define APP_SLOT_SD1                    3   /* SD/MMC controller: (D) */
#ifdef CONFIG_USB_AMAGE_USED
#include "AmbaTUNE_USBCtrl.h"
#endif

/**
* drive info get
* @param [in] slot_id slot id of sd card
* @param [out] pDevInf drive info
* @return 0-OK, 1-NG
*/
INT32 AppUsb_GetDriveInfo(UINT8 slot_id, APP_USB_DRIVE_INFO *pDevInf)
{
    AMBA_FS_DRIVE_INFO_t dev_info;
    INT32 status = (INT32)OK;
    UINT32 rval;
    char drive = 'c';

    switch (slot_id) {
    case APP_SLOT_SD0:
        drive = 'c';
        break;
    case APP_SLOT_SD1:
        drive = 'd';
        break;
    default:
        AmbaPrint_PrintUInt5("AppUsb_GetDriveInfo(): unsupported slot %c.", (UINT32)slot_id + 0x41U, 0, 0, 0, 0); // 'A' = 0x41
        status = -1;
        break;
    }

    if (status == (INT32)OK) {

        rval = AmbaFS_GetDriveInfo(drive, &dev_info);

        if (rval == 0U) {
            pDevInf->Cls = dev_info.TotalClusters;
            pDevInf->Ecl = dev_info.EmptyClusters;
            pDevInf->Bps = dev_info.BytesPerSector;
            pDevInf->Spc = dev_info.SectorsPerCluster;
            pDevInf->Cpg = dev_info.ClustersPerGroup;
            pDevInf->Ecg = dev_info.EmptyClusterGroups;
            pDevInf->Fmt = dev_info.FormatType;
        } else {
            AmbaPrint_PrintStr5("AppUsb_GetDriveInfo(): Can't get drive %c information...", &drive, NULL, NULL, NULL, NULL);
            AmbaPrint_PrintUInt5("AppUsb_GetDriveInfo(): code %d.",  rval, 0, 0, 0, 0);
            status = -1;
        }
    }

    return status;
}

/**
* file info get
* @param [in] pName file name
* @param [out] pStat file status
* @return 0-OK, 1-NG
*/
UINT32 AppUsb_GetFileInfo(const char *pName, APP_USB_FILE_INFO *pStat)
{
    return AmbaFS_GetFileInfo(pName, pStat);
}

/**
* drive mount
* @param [in] slot_id slot id of sd card
* @return 0-OK, 1-NG
*/
INT32 AppUsb_MountDrive(INT32 slot_id)
{
    char drive = 'c';
    INT32 status = (INT32)OK;
    UINT32 rval = 0;

    switch (slot_id) {
    case APP_SLOT_SD0:
        drive = 'c';
        break;
    case APP_SLOT_SD1:
        drive = 'd';
        break;
    default:
        AmbaPrint_PrintUInt5("AppUsb_MountDrive(): unsupported slot %c.", (UINT32)slot_id + 0x41U, 0, 0, 0, 0); // 'A' = 0x41
        status = -1;
        break;
    }

    if (status == (INT32)OK) {
        rval = AmbaFS_Mount(drive);
        if (rval != 0U) {
            status = -1;
        }
    }

    return status;
}

/**
* drive un-mount
* @param [in] slot_id slot id of sd card
* @return 0-OK, 1-NG
*/
INT32 AppUsb_UnmountDrive(INT32 slot_id)
{
    char drive = 'c';
    INT32 status = (INT32)OK;
    UINT32 rval = 0;

    switch (slot_id) {
    case APP_SLOT_SD0:
        drive = 'c';
        break;
    case APP_SLOT_SD1:
        drive = 'd';
        break;
    default:
        AmbaPrint_PrintUInt5("AppUsb_UnmountDrive(): unsupported slot %c.", (UINT32)slot_id + 0x41U, 0, 0, 0, 0); // 'A' = 0x41
        status = -1;
        break;
    }

    if (status == (INT32)OK) {
        rval = AmbaFS_UnMount(drive);
        if (rval != 0U) {
            status = -1;
        }
    }

    return status;
}

/**
* Callback of Amage communication
* @param [in] Parameter1
* @param [in] Parameter2
* @param [in] Parameter3
* @param [in] Parameter4
* @param [in] Parameter5
* @param [in] Length
* @param [in] Direction
* @return 0-OK, 1-NG
*/
UINT32 AppMtpd_CbAmageCommunication(ULONG Parameter1,
                                    ULONG Parameter2,
                                    ULONG Parameter3,
                                    ULONG Parameter4,
                                    ULONG Parameter5,
                                    ULONG* Length,
                                    ULONG* Direction)
{
    /*
        Operation code :
            MTP_AmageCommunication 0x9999;
                Parameter 1: direction;
                Parameter 2: data size;
                Parameter 3: item_mode
    */

#ifdef CONFIG_USB_AMAGE_USED
    UINT32 *pLength = NULL;
    UINT32 *pDirection = NULL;
    *Direction = Parameter1;
    *Length = Parameter2;

    AmbaSvcWrap_MisraMemcpy(&pLength, &Length, sizeof(UINT32));
    AmbaSvcWrap_MisraMemcpy(&pDirection, &Direction, sizeof(UINT32));

    if(0 != AmbaItn_USBCtrl_Communication((UINT32)Parameter1,(UINT32)Parameter2,(UINT32) Parameter3,(UINT32) Parameter4,(UINT32) Parameter5, pLength, pDirection)) {
        AmbaPrint_PrintUInt5("call AmbaItn_USBCtrl_Communication failed",  0, 0, 0, 0, 0);
    }
#else
    (void)Parameter1;
    (void)Parameter2;
    (void)Parameter3;
    (void)Parameter4;
    (void)Parameter5;
    (void)Length;
    (void)Direction;
    AmbaMisra_TouchUnused(Length);
    AmbaMisra_TouchUnused(Direction);
#endif
    return OK;
}

/**
* data get of Amage communication
* @param [in] ObjectBuffer object buffer
* @param [in] ObjectOffset object offset
* @param [in] ObjectLengthRequested requested object length
* @param [in] ObjectActualLength actual object length
* @return 0-OK, 1-NG
*/
UINT32 AppMtpd_CbAmageDataGet(UCHAR *ObjectBuffer, ULONG ObjectOffset, ULONG ObjectLengthRequested, ULONG *ObjectActualLength)
{
#ifdef CONFIG_USB_AMAGE_USED
    UINT8 *pObjectBuffer = NULL;
    UINT32 *pObjectActualLength = NULL;
    *ObjectActualLength = 0U;
    *ObjectBuffer = 0U;
    AmbaSvcWrap_MisraMemcpy(&pObjectBuffer, &ObjectBuffer, sizeof(UINT32));
    AmbaSvcWrap_MisraMemcpy(&pObjectActualLength, &ObjectActualLength, sizeof(UINT32));

    if(0 != AmbaItn_USBCtrl_Save(pObjectBuffer,(UINT32)ObjectOffset, (UINT32)ObjectLengthRequested, pObjectActualLength)) {
        AmbaPrint_PrintUInt5("[USB][Platform]call AmbaItn_USBCtrl_Save failed",  0, 0, 0, 0, 0);
    }
#else
    AmbaMisra_TouchUnused(ObjectBuffer);
    AmbaMisra_TouchUnused(&ObjectOffset);
    AmbaMisra_TouchUnused(&ObjectLengthRequested);
    AmbaMisra_TouchUnused(ObjectActualLength);
    // AmbaPrint_PrintStr5("AppMtpd_CbAmageDataGet(): ", NULL, NULL, NULL, NULL, NULL);
#endif
    return OK;
}

/**
* data send of Amage communication
* @param [in] ObjectBuffer object buffer
* @param [in] ObjectOffset object offset
* @param [in] ObjectLength object length
* @return 0-OK, 1-NG
*/
UINT32 AppMtpd_CbAmageDataSend(UCHAR *ObjectBuffer, ULONG ObjectOffset, ULONG ObjectLength)
{
#ifdef CONFIG_USB_AMAGE_USED
    UINT8 *pObjectBuffer = NULL;

    /* CERT STR30-C */
    AmbaMisra_TouchUnused(ObjectBuffer);
    AmbaMisra_TouchUnused(pObjectBuffer);

    AmbaSvcWrap_MisraMemcpy(&pObjectBuffer, &ObjectBuffer, sizeof(UINT32));

    if(0 != AmbaItn_USBCtrl_Load(pObjectBuffer,(UINT32)ObjectOffset, (UINT32)ObjectLength)) {
        AmbaPrint_PrintUInt5("[USB][Platform]call USBHdlr_AmageDataSend failed",  0, 0, 0, 0, 0);
    }
#else
    AmbaMisra_TouchUnused(ObjectBuffer);
    AmbaMisra_TouchUnused(&ObjectOffset);
    AmbaMisra_TouchUnused(&ObjectLength);
#endif
    return OK;
}
