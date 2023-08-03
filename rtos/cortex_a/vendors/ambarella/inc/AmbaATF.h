/**
 *  @file AmbaATF.h
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
 *  @details function definitions for Arm-Trusted-Firmware (ATF)
 *
 */

#ifndef AMBAATF_H
#define AMBAATF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * cf. arm-trusted-firmware/include/lib/psci/psci.h
 */
#define PSCI_VERSION                    (0x84000000u)
#define PSCI_CPU_SUSPEND_AARCH32        (0x84000001u)
#define PSCI_CPU_SUSPEND_AARCH64        (0xC4000001u)
#define PSCI_CPU_OFF                    (0x84000002u)
#define PSCI_CPU_ON_AARCH32             (0x84000003u)
#define PSCI_CPU_ON_AARCH64             (0xC4000003u)
#define PSCI_AFFINITY_INFO_AARCH32      (0x84000004u)
#define PSCI_AFFINITY_INFO_AARCH64      (0xC4000004u)
#define PSCI_MIG_AARCH32                (0x84000005u)
#define PSCI_MIG_AARCH64                (0xC4000005u)
#define PSCI_MIG_INFO_TYPE              (0x84000006u)
#define PSCI_MIG_INFO_UP_CPU_AARCH32    (0x84000007u)
#define PSCI_MIG_INFO_UP_CPU_AARCH64    (0xC4000007u)
#define PSCI_SYSTEM_OFF                 (0x84000008u)
#define PSCI_SYSTEM_RESET               (0x84000009u)
#define PSCI_FEATURES                   (0x8400000Au)
#define PSCI_SYSTEM_SUSPEND_AARCH32     (0x8400000Eu)
#define PSCI_SYSTEM_SUSPEND_AARCH64     (0xC400000Eu)
#define PSCI_STAT_RESIDENCY_AARCH32     (0x84000010u)
#define PSCI_STAT_RESIDENCY_AARCH64     (0xc4000010u)
#define PSCI_STAT_COUNT_AARCH32         (0x84000011u)
#define PSCI_STAT_COUNT_AARCH64         (0xc4000011u)
#define PSCI_SYSTEM_RESET2_AARCH32      (0x84000012u)
#define PSCI_SYSTEM_RESET2_AARCH64      (0xc4000012u)
#define PSCI_MEM_PROTECT                (0x84000013u)
#define PSCI_MEM_CHK_RANGE_AARCH32      (0x84000014u)
#define PSCI_MEM_CHK_RANGE_AARCH64      (0xc4000014u)

/* PSCI v0.2 affinity level state returned by AFFINITY_INFO */
#define PSCI_0_2_AFFINITY_LEVEL_ON          0
#define PSCI_0_2_AFFINITY_LEVEL_OFF         1
#define PSCI_0_2_AFFINITY_LEVEL_ON_PENDING  2

/*
 * cf. arm-trusted-firmware/docs/arm-sip-service.rst
 *     plat/ambarella/include/ambarella_smc.h
 *     plat/ambarella/include/ambarella_def.h
 */
/* Ambarella customized interface for Ambalink */
#define AMBA_SIP_LINUX_SUSPEND_DONE     (0x8200FF02u)
#define AMBA_SIP_BOOT_LINUX             (0x8200FF03u)
#define AMBA_SIP_BOOT_RTOS              (0x8200FF04u)

#define NS_SWITCH_AARCH32               (0)
#define NS_SWITCH_AARCH64               (1)
#define  S_SWITCH_AARCH32               (2)
#define  S_SWITCH_AARCH64               (3)

/* cf. arm-trusted-firmware/plat/ambarella/include/ambarella_smc.h
 *          0x82000000-0x8200FFFF SMC32: SiP Service Calls
 *          0xC2000000-0xC200FFFF SMC64: SiP Service Calls
 */
/* Read AMBA Unique ID in OTP */
#define SMC32_AMBA_SIP_OTP_GET_UNIQUE_ID                   (0x82000501u)
/* Read Public key from OTP */
#define SMC32_AMBA_SIP_OTP_GET_PUKEY                       (0x82000503u)
/* Write Public key into OTP */
#define SMC32_AMBA_SIP_OTP_SET_PUKEY                       (0x82000504u)
/* Read Customer Unique ID from OTP */
#define SMC32_AMBA_SIP_OTP_GET_CUSTOMER_ID                 (0x82000505u)
/* Write Customer Unique ID into OTP */
#define SMC32_AMBA_SIP_OTP_SET_CUSTOMER_ID                 (0x82000506u)
/* Read the monotonic counter from OTP */
#define SMC32_AMBA_SIP_OTP_GET_COUNTER                     (0x82000507u)
/* Increase the monotonic counter in OTP */
#define SMC32_AMBA_SIP_OTP_ADD_COUNTER                     (0x82000508u)
/* Permanently Enable Secure Boot */
#define SMC32_AMBA_SIP_OTP_PERMANENTLY_ENABLE_SECURE_BOOT  (0x82000509u)
/* Read HW Unique Encryption Key */
#define SMC32_AMBA_SIP_OTP_GET_HUK                         (0x82000520u)
/* Generate HW Unique Encryption Key */
#define SMC32_AMBA_SIP_OTP_SET_HUK_NONCE                   (0x82000521u)
/* Read AES Key */
#define SMC32_AMBA_SIP_OTP_GET_AES_KEY                     (0x82000523u)
/* Write AES Key */
#define SMC32_AMBA_SIP_OTP_SET_AES_KEY                     (0x82000524u)
/* Read Reserved DX, User Slot G0 */
#define SMC32_AMBA_SIP_OTP_GET_G0                          (0x82000527u)
#define SMC32_AMBA_SIP_OTP_GET_DX                          (0x82000527u)
/* Write Reserved DX, User Slot G0 */
#define SMC32_AMBA_SIP_OTP_SET_G0                          (0x82000528u)
#define SMC32_AMBA_SIP_OTP_SET_DX                          (0x82000528u)
/* Read User Slot G1 */
#define SMC32_AMBA_SIP_OTP_GET_G1                          (0x82000529u)
/* Write User Slot G1 */
#define SMC32_AMBA_SIP_OTP_SET_G1                          (0x8200052au)
/* Revoke Public key from OTP */
#define SMC32_AMBA_SIP_OTP_REVOKE_PUKEY                    (0x8200052bu)
/* Read Reserved B2, Test Region */
#define SMC32_AMBA_SIP_OTP_GET_TEST_REG                    (0x8200052du)
#define SMC32_AMBA_SIP_OTP_GET_B2                          (0x8200052du)
/* Write Reserved B2, Test Region */
#define SMC32_AMBA_SIP_OTP_SET_TEST_REG                    (0x8200052eu)
#define SMC32_AMBA_SIP_OTP_SET_B2                          (0x8200052eu)
/* Enable BST anti-rollback */
#define SMC32_AMBA_SIP_OTP_EN_ANTI_RB                      (0x8200052fu)
/* Query OTP Setting */
#define SMC32_AMBA_QUERY_OTP_SETTING                       (0x82000530u)
/* Set JTAG efuse */
#define SMC32_AMBA_OTP_SET_JTAG_EFUSE                      (0x82000531u)
/* Lock Public Key */
#define SMC32_AMBA_OTP_LOCK_PUKEY                          (0x82000533u)
/* OTP layout V2: disable Secure USB boot */
#define SMC32_AMBA_OTP_DIS_SECURE_USB_BOOT                 (0x8200053fu)
/* Read bst version counter */
#define SMC32_AMBA_OTP_GET_BST_VER                         (0x82000540u)
/* Increase bst version counter */
#define SMC32_AMBA_OTP_INCREASE_BST_VER                    (0x82000541u)

#endif  /* AMBAATF_H */

