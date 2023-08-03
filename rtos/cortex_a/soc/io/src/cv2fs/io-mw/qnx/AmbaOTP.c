/**
 *  @file AmbaOTP.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details OTP (One Time Programming) Middleware APIs
 *
 */

#include <fcntl.h>
#include <stdio.h>

#include <AmbaTypes.h>
#include <hw/ambarella_otp.h>
#include <AmbaOTP.h>

UINT32 AmbaOTP_PublicKeyGet(UINT8 *Buffer, UINT32 BufferSize, UINT32 KeyIndex, UINT32 *KeyStatus)
{

    int fd, err;
    otp_msg_pubkey_t msg;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else if (BufferSize < sizeof(msg.data)) {
        uret = OTP_ERR_PARAM;
    } else {

        msg.index = KeyIndex;
        msg.status = 0x00FF;

        err = devctl(fd, DCMD_OTP_PUBKEY_GET, &msg, sizeof(msg), NULL);
        if (err != 0) {
            printf("DCMD_OTP_PUBKEY_GET error %d\n", err);
            uret = (UINT32) err;
        } else {
            *KeyStatus = msg.status;
            memcpy(Buffer, msg.data, sizeof(msg.data));
        }

        close(fd);
    }
    return uret;
}

UINT32 AmbaOTP_PublicKeySet(const UINT8 *Buffer, UINT32 BufferSize, UINT32 KeyIndex)
{
    int fd, err;
    otp_msg_pubkey_t msg;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else if (BufferSize < sizeof(msg.data)) {
        uret = OTP_ERR_PARAM;
    } else {

        msg.index = KeyIndex;
        msg.status = 0x00FF;
        memcpy(msg.data, Buffer, sizeof(msg.data));

        err = devctl(fd, DCMD_OTP_PUBKEY_SET, &msg, sizeof(msg), NULL);
        if (err != 0) {
            printf("DCMD_OTP_PUBKEY_SET error %d\n", err);
            uret = (UINT32) err;
        }

        close(fd);
    }
    return uret;
}

UINT32 AmbaOTP_AmbaUniqueIDGet(UINT8 *Buffer, UINT32 BufferSize)
{
    int fd, err;
    otp_msg_amba_unique_id_t msg;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else if (BufferSize < sizeof(msg.data)) {
        uret = OTP_ERR_PARAM;
    } else {
        err = devctl(fd, DCMD_OTP_AMBA_UNI_ID_GET, &msg, sizeof(msg), NULL);
        if (err != 0) {
            printf("DCMD_OTP_AMBA_UNI_ID_GET error %d\n", err);
            uret = (UINT32) err;
        } else {
            memcpy(Buffer, msg.data, sizeof(msg.data));
        }

        close(fd);
    }
    return uret;
}

UINT32 AmbaOTP_CustomerUniIDSet(const UINT8 *Buffer, UINT32 BufferSize)
{

    int fd, err;
    otp_msg_customer_unique_id_t msg;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else if (BufferSize < sizeof(msg.data)) {
        uret = OTP_ERR_PARAM;
    } else {

        memcpy(msg.data, Buffer, sizeof(msg.data));

        err = devctl(fd, DCMD_OTP_CUSTOMER_UNI_ID_SET, &msg, sizeof(msg), NULL);
        if (err != 0) {
            printf("DCMD_OTP_CUSTOMER_UNI_ID_SET error %d\n", err);
            uret = (UINT32) err;
        }

        close(fd);
    }
    return uret;

}

UINT32 AmbaOTP_CustomerUniIDGet(UINT8 *Buffer, UINT32 BufferSize)
{
    int fd, err;
    otp_msg_customer_unique_id_t msg;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else if (BufferSize < sizeof(msg.data)) {
        uret = OTP_ERR_PARAM;
    } else {
        err = devctl(fd, DCMD_OTP_CUSTOMER_UNI_ID_GET, &msg, sizeof(msg), NULL);
        if (err != 0) {
            printf("DCMD_OTP_CUSTOMER_UNI_ID_GET error %d\n", err);
            uret = (UINT32) err;
        } else {
            memcpy(Buffer, msg.data, sizeof(msg.data));
        }

        close(fd);
    }
    return uret;
}

UINT32 AmbaOTP_MonoCounterGet(UINT32 *Counter)
{
    int fd, err;
    unsigned int msg;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else {
        err = devctl(fd, DCMD_OTP_MONO_CNT_READ, &msg, sizeof(msg), NULL);
        if (err != 0) {
            printf("DCMD_OTP_MONO_CNT_READ error %d\n", err);
            uret = (UINT32) err;
        } else {
            *Counter = msg;
        }

        close(fd);
    }
    return uret;
}

UINT32 AmbaOTP_MonoCounterIncrease(void)
{
    int fd, err;
    UINT32 dummy = 0;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else {
        err = devctl(fd, DCMD_OTP_MONO_CNT_INCREASE, &dummy, sizeof(dummy), NULL);
        if (err != 0) {
            printf("DCMD_OTP_MONO_CNT_INCREASE error %d\n", err);
            uret = (UINT32) err;
        }

        close(fd);
    }
    return uret;
}

UINT32 AmbaOTP_SecureBootEnable(void)
{
    int fd, err;
    UINT32 dummy = 0;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else {
        err = devctl(fd, DCMD_OTP_SECURE_BOOT_SET, &dummy, sizeof(dummy), NULL);
        if (err != 0) {
            printf("DCMD_OTP_SECURE_BOOT_SET error %d\n", err);
            uret = (UINT32) err;
        }

        close(fd);
    }
    return uret;
}

UINT32 AmbaOTP_PublicKeyRevoke(UINT32 KeyIndex)
{
    int fd, err;
    otp_msg_pubkey_revoke_t msg;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else {

        msg.index = KeyIndex;

        err = devctl(fd, DCMD_OTP_PUBKEY_REVOKE, &msg, sizeof(msg), NULL);
        if (err != 0) {
            printf("DCMD_OTP_PUBKEY_REVOKE error %d\n", err);
            uret = (UINT32) err;
        }

        close(fd);
    }
    return uret;
}

UINT32 AmbaOTP_AesKeySet(const UINT8 *Buffer, UINT32 BufferSize, UINT32 KeyIndex)
{
    int fd, err;
    otp_msg_aeskey_t msg;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else if (BufferSize < sizeof(msg.data)) {
        uret = OTP_ERR_PARAM;
    } else {

        msg.index = KeyIndex;
        memcpy(msg.data, Buffer, sizeof(msg.data));

        err = devctl(fd, DCMD_OTP_AESKEY_SET, &msg, sizeof(msg), NULL);
        if (err != 0) {
            printf("DCMD_OTP_AESKEY_SET error %d\n", err);
            uret = (UINT32) err;
        }

        close(fd);
    }
    return uret;
}

UINT32 AmbaOTP_AesKeyGet(UINT8 *Buffer, UINT32 BufferSize, UINT32 KeyIndex)
{
    int fd, err;
    otp_msg_aeskey_t msg;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else if (BufferSize < sizeof(msg.data)) {
        uret = OTP_ERR_PARAM;
    } else {

        msg.index = KeyIndex;

        err = devctl(fd, DCMD_OTP_AESKEY_GET, &msg, sizeof(msg), NULL);
        if (err != 0) {
            printf("DCMD_OTP_AESKEY_GET error %d\n", err);
            uret = (UINT32) err;
        } else {
            memcpy(Buffer, msg.data, sizeof(msg.data));
        }

        close(fd);
    }
    return uret;
}

UINT32 AmbaOTP_ReservedDXGet(UINT8 *Buffer, UINT32 BufferSize, UINT32 Index)
{
    int fd, err;
    otp_msg_dx_zone_t msg;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else if (BufferSize < sizeof(msg.data)) {
        uret = OTP_ERR_PARAM;
    } else {

        msg.index = Index;

        err = devctl(fd, DCMD_OTP_RSVD_DX_GET, &msg, sizeof(msg), NULL);
        if (err != 0) {
            printf("DCMD_OTP_RSVD_DX_GET error %d\n", err);
            uret = (UINT32) err;
        } else {
            memcpy(Buffer, msg.data, sizeof(msg.data));
        }

        close(fd);
    }
    return uret;
}

UINT32 AmbaOTP_ReservedDXSet(const UINT8 *Buffer, UINT32 BufferSize, UINT32 Index)
{
    int fd, err;
    otp_msg_dx_zone_t msg;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else if (BufferSize < sizeof(msg.data)) {
        uret = OTP_ERR_PARAM;
    } else {

        msg.index = Index;
        memcpy(msg.data, Buffer, sizeof(msg.data));

        err = devctl(fd, DCMD_OTP_RSVD_DX_SET, &msg, sizeof(msg), NULL);
        if (err != 0) {
            printf("DCMD_OTP_RSVD_DX_SET error %d\n", err);
            uret = (UINT32) err;
        }

        close(fd);
    }
    return uret;
}
UINT32 AmbaOTP_ReservedB2Get(UINT8 *Buffer, UINT32 BufferSize)
{
    int fd, err;
    otp_msg_b2_zone_t msg;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else if (BufferSize < sizeof(msg.data)) {
        uret = OTP_ERR_PARAM;
    } else {

        err = devctl(fd, DCMD_OTP_RSVD_B2_GET, &msg, sizeof(msg), NULL);
        if (err != 0) {
            printf("DCMD_OTP_RSVD_B2_GET error %d\n", err);
            uret = (UINT32) err;
        } else {
            memcpy(Buffer, msg.data, sizeof(msg.data));
        }

        close(fd);
    }
    return uret;
}
UINT32 AmbaOTP_ReservedB2Set(const UINT8 *Buffer, UINT32 BufferSize)
{
    int fd, err;
    otp_msg_b2_zone_t msg;
    UINT32 uret = 0;

    fd = open("/dev/otp", O_RDWR);
    if (fd == -1) {
        uret = OTP_ERR_OPEN;
    } else if (BufferSize < sizeof(msg.data)) {
        uret = OTP_ERR_PARAM;
    } else {

        memcpy(msg.data, Buffer, sizeof(msg.data));

        err = devctl(fd, DCMD_OTP_RSVD_B2_SET, &msg, sizeof(msg), NULL);
        if (err != 0) {
            printf("DCMD_OTP_RSVD_B2_SET error %d\n", err);
            uret = (UINT32) err;
        }

        close(fd);
    }
    return uret;
}
