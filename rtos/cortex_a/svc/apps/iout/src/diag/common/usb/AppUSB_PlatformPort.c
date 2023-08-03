/**
 *  @file AppUSB_PlatformPort.c
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
 *  @details USB Platform Dependent functions for APP/MW.
 */

#include "AppUSB.h"
#ifndef AMBAUSB_DEVICE_API_H
#include <AmbaUSBD_API.h>
#endif
#ifndef AMBA_FS_H
#include "AmbaFS.h"
#endif

#define APP_SLOT_SD0                    2   /* SD/MMC controller: (C) */
#define APP_SLOT_SD1                    3   /* SD/MMC controller: (D) */
#ifdef CONFIG_BUILD_SSP_USB_AMAGE
#include "AmbaTUNE_USBCtrl.h"
#endif

/** \addtogroup APPLICATION_APIS
 *  @{
 * */
/**
 * A wrapper function for getting file information through File System.
*/
UINT32 AppUsb_FSFileInfoGet(const char *pName, APP_USB_FILE_INFO *pStat)
{
    return AmbaFS_GetFileInfo(pName, pStat);
}

/**
 * A wrapper function for mounting a drive through File System.
*/
INT32 AppUsb_FSDriveMount(INT32 slot_id)
{
    char drive = 'c';
    INT32 status = 0;
    UINT32 rval = 0;

    switch (slot_id) {
    case APP_SLOT_SD0:
        drive = 'c';
        break;
    case APP_SLOT_SD1:
        drive = 'd';
        break;
    default:
        AppUsb_PrintUInt5("AppUsb_FSDriveMount(): unsupported slot %c.", (UINT32)slot_id + 0x41U, 0, 0, 0, 0); // 'A' = 0x41
        status = -1;
        break;
    }

    if (status == 0) {
        rval = AmbaFS_Mount(drive);
        if (rval != 0U) {
            status = -1;
        }
    }

    return status;
}
/**
 * A wrapper function for un-mounting a drive through File System.
*/
INT32 AppUsb_FSDriveUnmount(INT32 slot_id)
{
    char drive = 'c';
    INT32 status = 0;
    UINT32 rval = 0;

    switch (slot_id) {
    case APP_SLOT_SD0:
        drive = 'c';
        break;
    case APP_SLOT_SD1:
        drive = 'd';
        break;
    default:
        AppUsb_PrintUInt5("AppUsb_FSDriveUnmount(): unsupported slot %c.", (UINT32)slot_id + 0x41U, 0, 0, 0, 0); // 'A' = 0x41
        status = -1;
        break;
    }

    if (status == 0) {
        rval = AmbaFS_UnMount(drive);
        if (rval != 0U) {
            status = -1;
        }
    }

    return status;
}
/**
 * A wrapper function for getting drive information through File System.
*/
INT32 AppUsb_FSDriveInfoGet(INT32 slot_id, APP_USB_DRIVE_INFO *pDevInf)
{
    AMBA_FS_DRIVE_INFO_t dev_info;
    INT32 status = 0;
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
        AppUsb_PrintUInt5("AppUsb_FSDriveInfoGet(): unsupported slot %c.", (UINT32)slot_id + 0x41U, 0, 0, 0, 0); // 'A' = 0x41
        status = -1;
        break;
    }

    if (status == 0) {

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
            AppUsb_PrintStr5("AppUsb_FSDriveInfoGet(): Can't get drive %c information...", &drive, NULL, NULL, NULL, NULL);
            AppUsb_PrintUInt5("AppUsb_FSDriveInfoGet(): code %d.",  rval, 0, 0, 0, 0);
            status = -1;
        }
    }

    return status;
}
/**
 * Amage only.
 * Operation code : MTP_AmageCommunication 0x9999; Parameter 1: direction; Parameter 2: data size; Parameter 3: item_mode
*/
UINT32 AppMtpd_CbAmageCommunication(ULONG Parameter1, ULONG Parameter2, ULONG Parameter3, ULONG Parameter4, ULONG Parameter5, ULONG* Length, ULONG* Direction)
{

#ifdef CONFIG_BUILD_SSP_USB_AMAGE
    UINT32 *pLength = NULL;
    UINT32 *pDirection = NULL;
    *Direction = 0U;
    *Length = 0U;

    (void)AmbaWrap_memcpy(&pLength, &Length, sizeof(UINT32));
    (void)AmbaWrap_memcpy(&pDirection, &Direction, sizeof(UINT32));

    if(0 != AmbaItn_USBCtrl_Communication((UINT32)Parameter1,(UINT32)Parameter2,(UINT32) Parameter3,(UINT32) Parameter4,(UINT32) Parameter5, pLength, pDirection)) {
        AppUsb_PrintUInt5("call USBDataCtrlAmageCommunication failed",  0, 0, 0, 0, 0);
    }
#else
    (VOID) Parameter1;
    (VOID) Parameter2;
    (VOID) Parameter3;
    (VOID) Parameter4;
    (VOID) Parameter5;
    (VOID) Length;
    (VOID) Direction;
#endif
    return 0;
}
/**
 * Amage only.
 * */
UINT32 AppMtpd_CbAmageDataGet(UCHAR *ObjectBuffer, ULONG ObjectOffset, ULONG ObjectLengthRequested, ULONG *ObjectActualLength)
{
#ifdef CONFIG_BUILD_SSP_USB_AMAGE
    UINT8 *pObjectBuffer = NULL;
    UINT32 *pObjectActualLength = NULL;
    *ObjectActualLength = 0U;
    *ObjectBuffer = 0U;
    (void)AmbaWrap_memcpy(&pObjectBuffer, &ObjectBuffer, sizeof(UINT32));
    (void)AmbaWrap_memcpy(&pObjectActualLength, &ObjectActualLength, sizeof(UINT32));

    if(0 != AmbaItn_USBCtrl_Save(pObjectBuffer,(UINT32)ObjectOffset, (UINT32)ObjectLengthRequested, pObjectActualLength)) {
        AppUsb_PrintUInt5("[USB][Platform]call USBHdlr_AmageDataGet failed",  0, 0, 0, 0, 0);
    }
#else
    (VOID) ObjectBuffer;
    (VOID) ObjectOffset;
    (VOID) ObjectLengthRequested;
    (VOID) ObjectActualLength;
#endif
    return 0;
}
/**
 * Amage only.
 * */
UINT32 AppMtpd_CbAmageDataSend(UCHAR *ObjectBuffer, ULONG ObjectOffset, ULONG ObjectLength)
{
#ifdef CONFIG_BUILD_SSP_USB_AMAGE
    const UINT8 *pObjectBuffer = NULL;

    (void)AmbaWrap_memcpy(&pObjectBuffer, &ObjectBuffer, sizeof(UINT32));

    if(0 != AmbaItn_USBCtrl_Load(pObjectBuffer,(UINT32)ObjectOffset, (UINT32)ObjectLength)) {
        AppUsb_PrintUInt5("[USB][Platform]call USBHdlr_AmageDataSend failed",  0, 0, 0, 0, 0);
    }
#else
    (VOID) ObjectBuffer;
    (VOID) ObjectOffset;
    (VOID) ObjectLength;
#endif
    return 0;
}
/** @} */
