/**
*  @file SvcStgMonitor.h
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
*
*/

#ifndef SVC_STG_MONI_H
#define SVC_STG_MONI_H

#define SVC_STG_MONI_TYPE_SD    (0U)
#define SVC_STG_MONI_TYPE_USB   (1U)

#define SVC_STG_USB_CLASS_NONE  (0U)
#define SVC_STG_USBD_CLASS_MSC  (1U)
#define SVC_STG_USBD_CLASS_MTP  (2U)
#define SVC_STG_USBH_CLASS_MSC  (3U)
#define SVC_STG_USB_CLASS_NUM   (4U)

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define SVC_STG_DRIVE_NUM       (1U)
#else
#define SVC_STG_DRIVE_NUM       (3U)
#endif

typedef struct {
    char   Drive;
    UINT8  DriveID;
    UINT8  DriveType;
    UINT8  IsExist;
} SVC_STG_MONI_DRIVE_INFO_s;

typedef void (*STG_MONI_NOTIFY_CB)(char Drive, UINT32 IsInsert);

typedef struct {
    UINT32              StgMonitPriority;
    UINT32              StgMonitCpuBits;
    UINT32              EnableStgMonitCallBack;
    STG_MONI_NOTIFY_CB  StgMonitCallBack;
    UINT32              EnableStgMgr;
    UINT32              StgMgrPriority;
    UINT32              StgMgrCpuBits;
    UINT8               UsbClass;
    char                MainStgDrive;
} SVC_STG_MONI_INIT_INFO_s;

UINT32 SvcStgMonitor_Init(const SVC_STG_MONI_INIT_INFO_s *pInitInfo);
UINT32 SvcStgMonitor_GetDriveInfo(char Drive, SVC_STG_MONI_DRIVE_INFO_s *pDriveInfo);
UINT32 SvcStgMonitor_StgInitStatus(void);

#endif  /* SVC_STG_MONI_H */
