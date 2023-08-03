/**
 *  @file PlatformPort.h
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
 *  @details USB Platform dependent header file.
 */

#ifndef PLATFORM_PORT_H
#define PLATFORM_PORT_H

// plaform define for CV22SDK
#define APP_USB_FILE_INFO      AMBA_FS_FILE_INFO_s
#define APP_USB_FS_DTA         AMBA_FS_DTA_t
#define AppUsb_MakeDir         AmbaFS_MakeDir
#define AppUsb_RemoveDir       AmbaFS_RemoveDir
#define AppUsb_FileFindFirst   AmbaFS_FileSearchFirst
#define AppUsb_FileFindNext    AmbaFS_FileSearchNext

#define APP_USB_PHY_NUM        1
#define APP_USB_SUPPORT_IDDIG0 0

/* Standard file attribute */
#define APP_FS_ATTR_RDONLY     AMBA_FS_ATTR_RDONLY
#define APP_FS_ATTR_HIDDEN     AMBA_FS_ATTR_HIDDEN
#define APP_FS_ATTR_SYSTEM     AMBA_FS_ATTR_SYSTEM
#define APP_FS_ATTR_VOLUME     AMBA_FS_ATTR_VOLUME
#define APP_FS_ATTR_DIR        AMBA_FS_ATTR_DIR
#define APP_FS_ATTR_ARCH       AMBA_FS_ATTR_ARCH

/* Other file attribute */
#define APP_FS_ATTR_NONE       AMBA_FS_ATTR_NONE
#define APP_FS_ATTR_ALL        AMBA_FS_ATTR_ALL
#define APP_FS_ATTR_FILE_ONLY  AMBA_FS_ATTR_FILE_ONLY

#define APP_FS_DTA_t AMBA_FS_DTA_t

#define APP_FS_SEEK_START AMBA_FS_SEEK_START

typedef struct {
    /* total number of logical clusters */
    UINT32 Cls;
    /* number of empty clusters */
    UINT32 Ecl;
    /* bytes per logical sector */
    UINT32 Bps;
    /* logical sectors per cluster */
    UINT32 Spc;
    /* clusters per cluster group */
    UINT32 Cpg;
    /* number of empty cluster groups */
    UINT32 Ecg;
    /* format type */
    UINT32 Fmt;
} APP_USB_DRIVE_INFO;

INT32  AppUsb_GetDriveInfo(UINT8 slot_id, APP_USB_DRIVE_INFO *pDevInf);
UINT32 AppUsb_GetFileInfo(const char *pName, APP_USB_FILE_INFO *pStat);
INT32  AppUsb_MountDrive(INT32 slot_id);
INT32  AppUsb_UnmountDrive(INT32 slot_id);

UINT32 AppMtpd_CbAmageCommunication(ULONG Parameter1, ULONG Parameter2, ULONG Parameter3, ULONG Parameter4, ULONG Parameter5, ULONG* Length, ULONG* Direction);
UINT32 AppMtpd_CbAmageDataGet(UCHAR *ObjectBuffer, ULONG ObjectOffset, ULONG ObjectLengthRequested, ULONG *ObjectActualLength);
UINT32 AppMtpd_CbAmageDataSend(UCHAR *ObjectBuffer, ULONG ObjectOffset, ULONG ObjectLength);

#endif  /* PLATFORM_PORT_H */
