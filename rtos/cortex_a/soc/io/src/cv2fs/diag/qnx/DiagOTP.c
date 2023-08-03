/**
 *  @file DiagOTP.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *  Copyright 2020, Ambarella International LP
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
 *  @details OTP (One Time Programming) diagnostic code
 *
 */

#include <stdio.h>
#include <string.h>
#include "diag.h"
#include "AmbaOTP.h"

static void IoDiag_OtpCmdUsage(void)
{
    printf("Usage:\n");
    printf("   read_key <C1/C2/C3>             : Dump otp RSA key data from C1/C2/C3 zone\n");
    printf("   write_key <C1/C2/C3> [filename] : Write otp RSA key data to C1/C2/C3 zone from input file\n");
    printf("   read_aes_key <C1/C2/C3/C4>      : Dump otp AES key data from C1/C2/C3/C4 zone\n");
    printf("   write_aes_key <C1/C2/C3/C4>     : Write otp AES key data to C1/C2/C3/C4 zone from fixed data\n");
    printf("   amba_unique_id                  : read Amba Unique ID from OTP\n");
    printf("   ctm_uid_read                    : read Customer Unique ID from OTP\n");
    printf("   ctm_uid_write [filename]        : write Customer Unique ID to OTP from input file\n");
    printf("   counter_read                    : read monotnoic counter\n");
    printf("   counter_inc                     : increase monotnoic counter\n");
}

static void otp_public_key_dump(UINT32 KeyIndex)
{

    UINT32 uret;
    UINT32 key_status;
    static UINT8  buffer[512];

    uret = AmbaOTP_PublicKeyGet(buffer, 512, KeyIndex, &key_status);
    if (uret == 0U) {
        UINT32 i;
        printf("OTP public key (%d):\n", KeyIndex);
        printf("    ");
        for (i = 0; i < 512; i++) {
            if ((i != 0U) && ((i % 16) == 0U)) {
                printf("\n");
                printf("    ");
            }
            printf("%02X:", buffer[i]);
        }

        printf("\n");
        printf("    Key Status: 0x%X\n", key_status);

    } else {
        printf("Fail to get OTP public key, code 0x%X\n", uret);
    }
}

static void otp_public_key_write(UINT32 KeyIndex, const char *FileName)
{
    UINT32 uret;
    static UINT8  buffer[512];
    FILE *fd;

    // open file for reading data
    fd = fopen(FileName, "rb");
    if (fd == NULL) {
        printf("fail to open %s\n", FileName);
    } else {
        // read key data
        size_t bytes_read = fread(buffer, 1, 512, fd);
        if (bytes_read != 512UL) {
            printf("file must contain 512 bytes data!\n");
        } else {
            uret = AmbaOTP_PublicKeySet(buffer, 512, KeyIndex);
            if (uret == 0U) {
                printf("Success to set OTP public key %d\n", KeyIndex);
            } else {
                printf("Fail to set OTP public key %d, code 0x%X\n", KeyIndex, uret);
            }
        }
        fclose(fd);
    }
}

static void otp_aes_key_dump(UINT32 KeyIndex)
{

    UINT32 uret;
    static UINT8  buffer[32];

    memset(buffer, 0xFF, 32);

    uret = AmbaOTP_AesKeyGet(buffer, 32, KeyIndex);
    if (uret == 0U) {
        UINT32 i;
        printf("OTP AES key (%d):\n", KeyIndex);
        printf("    ");
        for (i = 0; i < 32; i++) {
            if ((i != 0U) && ((i % 16) == 0U)) {
                printf("\n");
                printf("    ");
            }
            printf("%02X:", buffer[i]);
        }

        printf("\n");

    } else {
        printf("Fail to get OTP AES key, code 0x%X\n", uret);
    }
}

static void otp_aes_key_write(UINT32 KeyIndex)
{
    UINT32 uret;
    static UINT8 *buffer = NULL;
    static UINT8 key1[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    };
    static UINT8 key2[32] = {
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    };
    static UINT8 key3[32] = {
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    };
    static UINT8 key4[32] = {
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
    };

    switch (KeyIndex) {
    case 0:
        buffer = key1;
        break;
    case 1:
        buffer = key2;
        break;
    case 2:
        buffer = key3;
        break;
    case 3:
        buffer = key4;
        break;
    default:
        break;
    }

    if (buffer != NULL) {
        uret = AmbaOTP_AesKeySet(buffer, 32, KeyIndex);
        if (uret == 0U) {
            printf("Success to set OTP AES key %d\n", KeyIndex);
        } else {
            printf("Fail to set OTP AES key %d, code 0x%X\n", KeyIndex, uret);
        }
    } else {
        printf("Unsupported Key Index %d\n", KeyIndex);
    }
}

static void otp_amba_unique_id_dump(void)
{

    UINT32 uret;
    static UINT8 buffer[16];

    uret = AmbaOTP_AmbaUniqueIDGet(buffer, 16);
    if (uret == 0U) {
        UINT32 i;
        printf("OTP Ambarella Ubique ID:\n");
        printf("    ");
        for (i = 0; i < 16; i++) {
            printf("%02X:", buffer[i]);
        }

        printf("\n");

    } else {
        printf("Fail to get OTP Ambarella Unique ID, code 0x%X\n", uret);
    }
}

static void otp_customer_unique_id_dump(void)
{
    UINT32 uret;
    static UINT8 buffer[16];

    uret = AmbaOTP_CustomerUniIDGet(buffer, 16);
    if (uret == 0U) {
        UINT32 i;
        printf("OTP Customer Ubique ID:\n");
        printf("    ");
        for (i = 0; i < 16; i++) {
            printf("%02X:", buffer[i]);
        }

        printf("\n");

    } else {
        printf("Fail to get OTP Customer Unique ID, code 0x%X\n", uret);
    }
}

static void otp_customer_unique_id_write(const char *FileName)
{
    UINT32 uret;
    static UINT8  buffer[16];
    FILE *fd;

    // open file for reading data
    fd = fopen(FileName, "rb");
    if (fd == NULL) {
        printf("fail to open %s\n", FileName);
    } else {
        // read data
        size_t bytes_read = fread(buffer, 1, 16, fd);
        if (bytes_read != 16UL) {
            printf("file must contain 16 bytes data!\n");
        } else {
            uret = AmbaOTP_CustomerUniIDSet(buffer, 16);
            if (uret == 0U) {
                printf("Success to set OTP Customer Unique ID\n");
            } else {
                printf("Fail to set OTP Customer Unique ID, code 0x%X\n", uret);
            }
        }
        fclose(fd);
    }
}

static void otp_mono_counter_dump(void)
{
    UINT32 uret;
    UINT32 counter_value;

    uret = AmbaOTP_MonoCounterGet(&counter_value);
    if (uret == 0U) {
        printf("Monotonic Counter: %d\n", counter_value);
    } else {
        printf("Fail to get OTP Monotonic Counter, code 0x%X\n", uret);
    }
}

static void otp_mono_counter_increase(void)
{
    UINT32 uret;
    uret = AmbaOTP_MonoCounterIncrease();
    if (uret == 0U) {
        printf("OTP Monotonic Counter Increased.\n");
    } else {
        printf("Fail to get OTP Monotonic Counter, code 0x%X\n", uret);
    }
}

int DoOtpDiag(int argc, char *argv[])
{
    if (argc >= 3) {
        if ((strcmp("read_key", argv[2]) == 0) && (argc == 4)) {
            UINT32 key_index = 0x0FFU;
            if (strcmp("C1", argv[3]) == 0) {
                key_index = 0;
            } else if (strcmp("C2", argv[3]) == 0) {
                key_index = 1;
            } else if (strcmp("C3", argv[3]) == 0) {
                key_index = 2;
            } else {
                IoDiag_OtpCmdUsage();
            }

            if (key_index != 0x0FFU) {
                otp_public_key_dump(key_index);
            }
        } else if ((strcmp("write_key", argv[2]) == 0) && (argc == 5)) {
            UINT32 key_index = 0x0FFU;
            if (strcmp("C1", argv[3]) == 0) {
                key_index = 0;
            } else if (strcmp("C2", argv[3]) == 0) {
                key_index = 1;
            } else if (strcmp("C3", argv[3]) == 0) {
                key_index = 2;
            } else {
                IoDiag_OtpCmdUsage();
            }

            if (key_index != 0x0FFU) {
                otp_public_key_write(key_index, argv[4]);
            }
        } else if ((strcmp("read_aes_key", argv[2]) == 0) && (argc == 4)) {
            UINT32 key_index = 0x0FFU;
            if (strcmp("C1", argv[3]) == 0) {
                key_index = 0;
            } else if (strcmp("C2", argv[3]) == 0) {
                key_index = 1;
            } else if (strcmp("C3", argv[3]) == 0) {
                key_index = 2;
            } else if (strcmp("C4", argv[3]) == 0) {
                key_index = 3;
            } else {
                IoDiag_OtpCmdUsage();
            }

            if (key_index != 0x0FFU) {
                otp_aes_key_dump(key_index);
            }
        } else if ((strcmp("write_aes_key", argv[2]) == 0) && (argc == 4)) {
            UINT32 key_index = 0x0FFU;
            if (strcmp("C1", argv[3]) == 0) {
                key_index = 0;
            } else if (strcmp("C2", argv[3]) == 0) {
                key_index = 1;
            } else if (strcmp("C3", argv[3]) == 0) {
                key_index = 2;
            } else if (strcmp("C4", argv[3]) == 0) {
                key_index = 3;
            } else {
                IoDiag_OtpCmdUsage();
            }

            if (key_index != 0x0FFU) {
                otp_aes_key_write(key_index);
            }
        } else if (strcmp("amba_unique_id", argv[2]) == 0) {
            otp_amba_unique_id_dump();
        } else if (strcmp("ctm_uid_read", argv[2]) == 0) {
            otp_customer_unique_id_dump();
        } else if ((strcmp("ctm_uid_write", argv[2]) == 0) && (argc == 4)) {
            otp_customer_unique_id_write(argv[3]);
        } else if (strcmp("counter_read", argv[2]) == 0) {
            otp_mono_counter_dump();
        } else if (strcmp("counter_inc", argv[2]) == 0) {
            otp_mono_counter_increase();
        } else {
            IoDiag_OtpCmdUsage();
        }
    } else {
        IoDiag_OtpCmdUsage();
    }

    return 0;
}