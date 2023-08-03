/**
 *  @file SvcUsb.h
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
 *  @details USB header file.
 */

#ifndef SVC_USB_H
#define SVC_USB_H

#include "bsp.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define SVC_USB_HOST_PWR        (AMBA_USB0_XHCI_PWR)
#define SVC_USB_HOST_OC         (AMBA_USB0_XHCI_OC)
#define SVC_USB_SUPER_SPEED     (1U)

#define SPEC_VERSION_FS         0x00, 0x02 // 0x0200
#define SPEC_VERSION_HS         0x10, 0x02 // 0x0210
#define SPEC_VERSION_SS         0x20, 0x03 // 0x0320
#else
#define SVC_USB_HOST_PWR        (AMBA_USB0_EHCI_PWR)
#define SVC_USB_HOST_OC         (AMBA_USB0_EHCI_OC)

#define SPEC_VERSION_FS         0x00, 0x02 // 0x0200
#define SPEC_VERSION_HS         0x00, 0x02 // 0x0200
#define SPEC_VERSION_SS         0x00, 0x02 // 0x0200
#endif

#define USB_BOS_DESC_LENGTH     0x2A, 0x00 // (USB_DESC_LENGTH_BOS +
//  USB_DESC_LENGTH_CAP_USB20EX
//  USB_DESC_LENGTH_CAP_SS_USB +
//  USB_DESC_LENGTH_CAP_SSP_USB)

#endif /* SVC_USB_H */
