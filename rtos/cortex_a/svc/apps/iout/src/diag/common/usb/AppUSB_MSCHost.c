/**
 *  @file AppUSB_MSCHost.c
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
 *  @details USB MSC Host application.
 */

#include "AppUSB.h"
#ifndef AMBAUSB_HOST_API_H
#include "AmbaUSBH_API.h"
#endif
#include "AmbaUSB_ErrCode.h"

#define MSCH_MEM_COPY              (512 * 1024)

static UINT32 usbh_cb_system_event(const UINT32 Code, const UINT32 SubCode)
{
    (VOID) SubCode;
    switch (Code) {
    case USBH_EVENT_DEVICE_NO_RESPONSE:
        AppUsb_PrintUInt5("[USBH] Device No Respond", 0, 0, 0, 0, 0);
        break;
    case USBH_EVENT_DEVICE_NOT_SUPPORT:
        AppUsb_PrintUInt5("[USBH] Device Not Support", 0, 0, 0, 0, 0);
        break;
    case USBH_EVENT_DEVICE_OVER_CURRENT:
        AppUsb_PrintUInt5("[USBH] Device Over Current", 0, 0, 0, 0, 0);
        break;
    case USBH_EVENT_HUB_NOT_SUPPORT:
        AppUsb_PrintUInt5("[USBH] Hub Not Support", 0, 0, 0, 0, 0);
        break;
    case USBH_EVENT_DEVICE_INSERTED:
        AppUsb_PrintUInt5("[USBH] Device Inserted", 0, 0, 0, 0, 0);
        break;
    case USBH_EVENT_DEVICE_REMOVED:
        AppUsb_PrintUInt5("[USBH] Device Removed", 0, 0, 0, 0, 0);
        break;
    default:
        AppUsb_PrintUInt5("[USBH] Unknown Event: 0x%X", Code, 0, 0, 0, 0);
        break;
    }
    ;
    return USB_ERR_SUCCESS;
}

static INT32 test_file_copy(const char *pFilePathSource, const char *pFilePathDestin)
{
    AMBA_FS_FILE *file_in = NULL;
    AMBA_FS_FILE *file_out = NULL;
    AMBA_FS_FILE_INFO_s file_info;
    UINT64 source_length  = 0;
    UINT64 file_size      = 0;
    UINT32 read_size      = 0;
    UINT32 write_size     = 0;
    UINT8 *test_buffer;
    UINT32 test_buffer_size;
    INT32  rval = 0;
    UINT32 fs_rval = 0;
    static UINT8 data_buffer[MSCH_MEM_COPY] __attribute__((section(".bss.noinit")));
    UINT32 t1, t2;
    UINT64 throughput_bps;
    UINT64 throughput_kbps;
    UINT64 throughput_mbps;

    AppUsb_PrintStr5("test_file_copy(): input = %s, output = %s", pFilePathSource, pFilePathDestin, NULL, NULL, NULL);

    if ((pFilePathSource == NULL) || (pFilePathDestin == NULL)) {
        rval = -1;
    } else {
        if (AmbaFS_FileOpen(pFilePathSource, "r", &file_in) != 0U) {
            AppUsb_PrintStr5("test_file_copy(): can't open %s for read.", pFilePathSource, NULL, NULL, NULL, NULL);
            rval = -1;
        } else {
            if (AmbaFS_GetFileInfo(pFilePathSource, &file_info) == 0U) {
                source_length = file_info.Size;
                file_size = source_length;
                if (AmbaFS_FileOpen(pFilePathDestin, "w", &file_out) != 0U) {
                    AppUsb_PrintStr5("test_file_copy(): can't open %s for write.", pFilePathDestin, NULL, NULL, NULL, NULL);
                    rval = -1;
                }
            } else {
                rval = -1;
            }
        }

        if (rval == 0) {

            test_buffer_size = sizeof(data_buffer);
            AppUsb_MemoryZeroSet(data_buffer, test_buffer_size);

            test_buffer = data_buffer;

            (void)AmbaKAL_GetSysTickCount(&t1);

            do {
                fs_rval = AmbaFS_FileRead(test_buffer, 1U, sizeof(data_buffer), file_in, &read_size);
                if ((read_size > 0U) && (fs_rval == 0U)) {
                    fs_rval = AmbaFS_FileWrite(test_buffer, 1, read_size, file_out, &write_size);
                    if ((write_size != read_size) || (fs_rval != 0U))  {
                        rval = -1;
                    }
                } else {
                    rval = -1;
                }

                if (rval != 0) {
                    break;
                } else {
                    source_length -= (UINT64)read_size;
                }
            } while (read_size > 0U);

            (void)AmbaKAL_GetSysTickCount(&t2);

            if (AmbaFS_FileEof(file_in) != 1U) {
                AppUsb_PrintUInt5("Not reach source file EOF, something wrong", 0, 0, 0, 0, 0);
            }

            if (AmbaFS_FileClose(file_in) != 0U) {
                AppUsb_PrintStr5("test_file_copy(): Can't close %s.", pFilePathSource, NULL, NULL, NULL, NULL);
            }

            if (AmbaFS_FileClose(file_out) != 0U) {
                AppUsb_PrintStr5("test_file_copy(): Can't close %s.", pFilePathDestin, NULL, NULL, NULL, NULL);
            }

            AppUsb_PrintUInt5("test_file_copy(): END. Close FILEs", 0, 0, 0, 0, 0);
            if (rval == 0) {
                UINT64 time_diff = (UINT64)t2 - (UINT64)t1;
                throughput_bps = (file_size * 1000ULL) / time_diff;
                throughput_kbps = throughput_bps / 1024U; // MBytes / sec
                throughput_mbps = throughput_kbps / 1024U; // MBytes / sec
                if (throughput_mbps != 0U) {
                    AppUsb_PrintUInt5("test_file_copy(): %d MB/s, file size %d bytes.", (UINT32)throughput_mbps, (UINT32)file_size, 0, 0, 0);
                } else if (throughput_kbps != 0U) {
                    AppUsb_PrintUInt5("test_file_copy(): %d KB/s, file size %d bytes.", (UINT32)throughput_kbps, (UINT32)file_size, 0, 0, 0);
                } else {
                    AppUsb_PrintUInt5("test_file_copy(): %d B/s, file size %d bytes.", (UINT32)throughput_bps, (UINT32)file_size, 0, 0, 0);
                }
            }
        }
    }

    return rval;
}

/** \addtogroup APPLICATION_APIS
 *  @{
 * */
/**
 * Start USB Host MSC class
*/
void AppMsch_Start(UINT8 SelectHcd, UINT8 OCPolarity, UINT8 Phy0Owner)
{
    USBH_CLASS_CONFIG_s ClassConfig;
    UINT32 uret;
    UINT32 module_id;
    UINT32 func_uret;
    UINT32 debug_leve = AppUsb_DebugLevelGet();
    const IOUT_MEMORY_INFO_s *noncache_mem_info;

    AppUsbh_GpioConfig();

    // enable USB/USBX print messages
    module_id = (USB_ERR_BASE >> 16U);
    func_uret = AmbaPrint_ModuleSetAllowList((UINT16)module_id, 1);
    if (func_uret != 0U) {
        AppUsb_PrintUInt5("Can't enable module printfor USB, code 0x%X", func_uret, 0, 0, 0, 0);
    }
    module_id = (USBX_ERR_BASE >> 16U);
    func_uret = AmbaPrint_ModuleSetAllowList((UINT16)module_id, 1);
    if (func_uret != 0U) {
        AppUsb_PrintUInt5("Can't enable module print for USBX, code 0x%X", func_uret, 0, 0, 0, 0);
    }

    if (debug_leve != 0xFFFFFFFFUL) {
        AmbaUSB_SystemPrintFuncRegister(AppUsb_Print, debug_leve);
    }

    noncache_mem_info = AmbaIOUT_NoncacheMemInfoGet(IOUT_MEMID_USB);

    // Configure parameters.
    AppUsb_MemoryZeroSet(&ClassConfig, sizeof(USBH_CLASS_CONFIG_s));

    ClassConfig.ClassID                  = USBH_CLASS_STORAGE;
    ClassConfig.StackCacheMemory         = g_usbx_memory_cached;
    ClassConfig.StackCacheMemorySize     = USBX_CACHE_MEM_SIZE;
    ClassConfig.StackNonCacheMemory      = noncache_mem_info->Ptr;
    ClassConfig.StackNonCacheMemorySize  = (UINT32)noncache_mem_info->Size;
    ClassConfig.CoreMask                 = AMBA_KAL_CPU_CORE_MASK;
    ClassConfig.EnumTskInfo.Priority     = 70;
    ClassConfig.EnumTskInfo.AffinityMask = 0x01;
    ClassConfig.EnumTskInfo.StackSize    = 8 * 1024;
    ClassConfig.HcdTskInfo.Priority      = 70;
    ClassConfig.HcdTskInfo.AffinityMask  = 0x01;
    ClassConfig.HcdTskInfo.StackSize     = 16 * 1024;
    ClassConfig.SystemEventCallback      = usbh_cb_system_event;
    ClassConfig.SelectHcd                = SelectHcd;
    ClassConfig.EhciOcPolarity           = OCPolarity; // High Active
    ClassConfig.Phy0Owner                = Phy0Owner;  // usb0 is host. if device mode is on, init would fail.
    ClassConfig.ClassStartCallback       = AmbaUSBH_StorageStart;
    ClassConfig.ClassStopCallback        = AmbaUSBH_StorageStop;

    uret = AmbaUSBH_SystemClassStart(&ClassConfig);

    AppUsb_PrintUInt5("AmbaUSBH_System_ClassStart: return 0x%x", uret, 0, 0, 0, 0);
}

/**
 * File Copy Test for USB MSC host
*/
void AppMsch_FileCopyTest(const char *pFilePathSource, const char *pFilePathDestin)
{
    if (test_file_copy(pFilePathSource, pFilePathDestin) != 0) {
        // ignore error
    }
}

/** @} */
