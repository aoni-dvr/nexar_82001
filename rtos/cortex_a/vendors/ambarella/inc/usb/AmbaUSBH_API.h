/**
 *  @file AmbaUSBH_API.h
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
 *  @details USB Host API header file.
 */

#ifndef AMBAUSB_HOST_API_H
#define AMBAUSB_HOST_API_H

#ifndef AMBAUSB_GENERIC_H
#include <AmbaUSB_Generic.h>
#endif

// HCD select

#define USBH_HCD_EHCI               0U
#define USBH_HCD_OHCI               1U
#define USBH_HCD_EHCI_OHCI          2U
#define USBH_HCD_XHCI               3U

// Define USB HCD Test Mode

#define USBH_TEST_MODE_J_STATE                           1U
#define USBH_TEST_MODE_K_STATE                           2U
#define USBH_TEST_MODE_SE0_NAK                           3U
#define USBH_TEST_MODE_PACKET                            4U
#define USBH_TEST_MODE_FORCE_ENABLE                      5U
#define USBH_TEST_MODE_SUSPEND                           6U
#define USBH_TEST_MODE_RESUME                            7U
#define USBH_TEST_MODE_GET_DESCRIPTOR                    8U
#define USBH_TEST_MODE_SET_FEATURE_S1                    9U
#define USBH_TEST_MODE_SET_FEATURE_S2                   10U

// USB Host Class ID

typedef enum {
    USBH_CLASS_NONE = 0,
    USBH_CLASS_STORAGE,  //!< Storage
    USBH_CLASS_SIMPLE,   //!< Simple class
    USBH_CLASS_CUSTOM,   //!< Customized Class
    USBH_CLASS_NUM
} USBH_CLASS_e;


// USB Host start configurations
typedef UINT32 (*USBH_SYSTEM_EVENT_CB_f)(UINT32 Code, UINT32 SubCode);
typedef UINT32 (*USBH_CLASS_START_CB_f)(USBH_CLASS_e ClassID);
typedef UINT32 (*USBH_CLASS_STOP_CB_f)(USBH_CLASS_e ClassID);
#define USBH_EVENT_DEVICE_NO_RESPONSE  0U
#define USBH_EVENT_DEVICE_NOT_SUPPORT  1U
#define USBH_EVENT_DEVICE_OVER_CURRENT 2U
#define USBH_EVENT_HUB_NOT_SUPPORT     3U
#define USBH_EVENT_DEVICE_INSERTED     4U
#define USBH_EVENT_DEVICE_REMOVED      5U

typedef struct {
    UINT8                      *StackCacheMemory;         // [Input] static memory for USBX stack usage (cacheable)
    UINT32                      StackCacheMemorySize;     // [Input] Size of static memory for USBX stack usage (cacheable)
    UINT8                      *StackNonCacheMemory;      // [Input] static memory for USBX stack usage (non-cacheable)
    UINT32                      StackNonCacheMemorySize;  // [Input] Size of static memory for USBX stack usage (non-cacheable)
    UINT32                      CoreMask;           // [Input] KAL Core Mask for creation of task
    USBX_TASKINFO_s             EnumTskInfo;        // [Input] Enumeration tasks info
    USBX_TASKINFO_s             HcdTskInfo;         // [Input] HCD (Host controller driver) tasks info
    USBH_SYSTEM_EVENT_CB_f      SystemEventCallback;// [Input] Callbacks for notified callbacks
    USBH_CLASS_e                ClassID;            // [Input] Class ID
    USBH_CLASS_START_CB_f       ClassStartCallback; // [Input] Class Start function
    USBH_CLASS_START_CB_f       ClassStopCallback;  // [Input] Class Stop function
    UINT8                       SelectHcd;          // [Input] 0: EHCI 1: OHCI 2:EHCI+OHCI
    UINT8                       EhciOcPolarity;     // [Input] 0: High active 1: Low active
    UINT8                       Phy0Owner;          // [Input] 0: device 1: host
} USBH_CLASS_CONFIG_s;

// USB Host Storage Class - Connected device information

typedef struct {
    UINT32  Present;
    UINT32  Format;
    UINT32  Lun;
    UINT32  SectorSize;
    UINT32  Lba;
    UINT32  WriteProtect;
} USBH_STORAGE_INFO_s;

// API functions.
// System
UINT32 AmbaUSBH_SystemClassStart(const USBH_CLASS_CONFIG_s *Config);
UINT32 AmbaUSBH_SystemClassStop(VOID);
UINT32 AmbaUSBH_SystemSetTestMode(UINT32 TestMode);

// MSC
UINT32 AmbaUSBH_StorageGetStatus(USBH_STORAGE_INFO_s *pInfo);
UINT32 AmbaUSBH_StorageRead(UINT8 *pBuffer, UINT32 StartSector, UINT32 Sectors);
UINT32 AmbaUSBH_StorageWrite(UINT8 *pBuffer, UINT32 StartSector, UINT32 Sectors);
UINT32 AmbaUSBH_StorageStart(USBH_CLASS_e ClassID);
UINT32 AmbaUSBH_StorageStop(USBH_CLASS_e ClassID);

#endif

