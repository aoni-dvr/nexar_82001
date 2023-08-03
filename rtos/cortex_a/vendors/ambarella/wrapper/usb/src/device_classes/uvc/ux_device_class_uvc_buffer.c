/**
 *  @file ux_device_class_uvc_buffer.c
 *
 *  @copyright Copyright (c) 2017 Ambarella, Inc.
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
 *  @details buffers of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"

UX_SLAVE_CLASS_UVC uvcd_class_memory __attribute__((section(".bss.noinit.usbx")));
UINT8 uvcd_main_thread_stack_memory[MAIN_THREAD_STACK_SIZE] __attribute__((section(".bss.noinit.usbx")));
UINT8 uvcd_event_thread_stack_memory[EVENT_THREAD_STACK_SIZE] __attribute__((section(".bss.noinit.usbx")));
UX_SLAVE_REQUEST_WRAPPER uvcd_wrapper_pool_memory[UVC_MAX_REQUEST_NUM] __attribute__((aligned(AMBA_CACHE_LINE_SIZE))) __attribute__((section(".bss.noinit.usbx")));
UX_SLAVE_CLASS_UVC_STATUS uvcd_status_memory[UX_DEVICE_CLASS_UVC_MAX_STATUS_QUEUE] __attribute__((section(".bss.noinit.usbx")));
UVCD_VS_INSTANCE uvcd_vs_memory[UVCD_MAX_VS_INSTANCE] __attribute__((section(".bss.noinit.usbx")));

