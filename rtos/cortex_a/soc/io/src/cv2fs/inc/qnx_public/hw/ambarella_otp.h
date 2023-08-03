/*
 * $QNXLicenseC:
 * Copyright 2007, 2008, 2018, QNX Software Systems.
 * Copyright 2020, Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#ifndef _OTP_LIB_H_INCLUDED
#define _OTP_LIB_H_INCLUDED


#include <stdint.h>
#include <AmbaKAL.h>

typedef struct {
    UINT8        data[512];
    UINT32       index;
    UINT32       status;
} otp_msg_pubkey_t;

typedef struct {
    UINT8        data[16];
} otp_msg_amba_unique_id_t;

typedef struct {
    UINT8        data[16];
} otp_msg_customer_unique_id_t;

typedef struct {
    UINT32 index;
} otp_msg_pubkey_revoke_t;

typedef struct {
    UINT8        data[32];
    UINT32       index;
} otp_msg_aeskey_t;

typedef struct {
    UINT8        data[32];
    UINT32       index;
} otp_msg_dx_zone_t;

typedef struct {
    UINT8        data[16];
} otp_msg_b2_zone_t;

/*
 * The following devctls are used by a client application
 * to control the OTP interface.
 */
#include <devctl.h>

#define AMBA_OTP_PUBKEY_GET          1
#define AMBA_OTP_PUBKEY_SET          2
#define AMBA_OTP_AMBA_UNI_ID_GET     3
#define AMBA_OTP_CUSTOMER_UNI_ID_GET 4
#define AMBA_OTP_CUSTOMER_UNI_ID_SET 5
#define AMBA_OTP_MONO_CNT_READ       6
#define AMBA_OTP_MONO_CNT_INCREASE   7
#define AMBA_OTP_SECUREN_BOOT_SET    8
#define AMBA_OTP_PUBKEY_REVOKE       9
#define AMBA_OTP_AESKEY_SET         10
#define AMBA_OTP_RSVD_DX_GET        11
#define AMBA_OTP_RSVD_DX_SET        12
#define AMBA_OTP_RSVD_B2_GET        13
#define AMBA_OTP_RSVD_B2_SET        14
#define AMBA_OTP_AESKEY_GET         15

#define _DCMD_OTP   _DCMD_MISC

#define DCMD_OTP_PUBKEY_SET            __DIOT(_DCMD_OTP, AMBA_OTP_PUBKEY_SET, otp_msg_pubkey_t)
#define DCMD_OTP_PUBKEY_GET            __DIOTF(_DCMD_OTP, AMBA_OTP_PUBKEY_GET, otp_msg_pubkey_t)
#define DCMD_OTP_AMBA_UNI_ID_GET       __DIOF(_DCMD_OTP, AMBA_OTP_AMBA_UNI_ID_GET, otp_msg_amba_unique_id_t)
#define DCMD_OTP_CUSTOMER_UNI_ID_GET   __DIOF(_DCMD_OTP, AMBA_OTP_CUSTOMER_UNI_ID_GET, otp_msg_customer_unique_id_t)
#define DCMD_OTP_CUSTOMER_UNI_ID_SET   __DIOT(_DCMD_OTP, AMBA_OTP_CUSTOMER_UNI_ID_SET, otp_msg_customer_unique_id_t)
#define DCMD_OTP_MONO_CNT_READ         __DIOF(_DCMD_OTP, AMBA_OTP_MONO_CNT_READ, unsigned int)
#define DCMD_OTP_MONO_CNT_INCREASE     __DIOT(_DCMD_OTP, AMBA_OTP_MONO_CNT_INCREASE, int)
#define DCMD_OTP_SECURE_BOOT_SET       __DIOT(_DCMD_OTP, AMBA_OTP_SECUREN_BOOT_SET, int)
#define DCMD_OTP_PUBKEY_REVOKE         __DIOT(_DCMD_OTP, AMBA_OTP_PUBKEY_REVOKE, otp_msg_pubkey_revoke_t)
#define DCMD_OTP_AESKEY_SET            __DIOT(_DCMD_OTP, AMBA_OTP_AESKEY_SET, otp_msg_aeskey_t)
#define DCMD_OTP_AESKEY_GET            __DIOTF(_DCMD_OTP, AMBA_OTP_AESKEY_GET, otp_msg_aeskey_t)
#define DCMD_OTP_RSVD_DX_GET           __DIOTF(_DCMD_OTP, AMBA_OTP_RSVD_DX_GET, otp_msg_dx_zone_t)
#define DCMD_OTP_RSVD_DX_SET           __DIOT(_DCMD_OTP, AMBA_OTP_RSVD_DX_SET, otp_msg_dx_zone_t)
#define DCMD_OTP_RSVD_B2_GET           __DIOTF(_DCMD_OTP, AMBA_OTP_RSVD_B2_GET, otp_msg_b2_zone_t)
#define DCMD_OTP_RSVD_B2_SET           __DIOT(_DCMD_OTP, AMBA_OTP_RSVD_B2_SET, otp_msg_b2_zone_t)

#endif
