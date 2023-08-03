/**
 *  @file AmbaUSBD_Msc.h
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
 *  @details USB Device Mass Storage Calss functions header file.
 */

#ifndef AMBAUSBD_MSC_H
#define AMBAUSBD_MSC_H

#include <usbx/usbx_device_classes/ux_device_class_storage.h>

#define MSC_DEV_MAX_NUM USBD_MAX_SLOT_NUMBER

typedef struct {
    /* scardmgr slot */
    UINT8  SlotName[64];
    UINT8  SlotLetter;

    /* MSC */
    UINT8  MscLun;
    UINT8 *MscVendor;
    UINT8 *MscProduct;
    UINT8 *MscVision;
    UINT8 *MscSpecific;
    UINT32 Mount;
    //SYSTIM  msc_rwtime;

    UINT8  DeviceType;     /* supports direct-access and CD-ROM */
    UINT8  Removable;
    UINT8  WriteProtect;

    UINT32 TotalSect;      /* total sector */
    UINT32 SectSize;       /* size of sector b*/
    UINT32 Status;         /* media status */
    UINT32 Format;         /* media format */
} MSC_DRIVE_INFO_s;

void   USBD_MscSetKernelInfo(UX_SLAVE_CLASS_STORAGE* info);
UINT32 USBD_MscStackRegister(UINT32 ConfigurationIndex, UINT32 InterfaceIndex);

#endif

