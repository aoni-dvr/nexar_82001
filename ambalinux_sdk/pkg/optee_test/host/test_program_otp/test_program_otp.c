/*
 * Copyright (C) 2018 Ambarella Inc.
 * All rights reserved.
 *
 * Author: Zhi He <zhe@ambarella.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "tee_client_api.h"

#include "program_otp_ca.h"

enum {
    EProgramOTPOperation_Invalid = 0x00,
    EProgramOTPOperation_WriteROTPubkey,
    EProgramOTPOperation_ReadROTPubkey,
    EProgramOTPOperation_ReadROTPubkeyDigest,
    EProgramOTPOperation_LockROTPubKey,
    EProgramOTPOperation_ReadAmbaUniqueID,
    EProgramOTPOperation_ReadMonoCounter,
    EProgramOTPOperation_IncreaseMonoCounter,
    EProgramOTPOperation_WriteCustomerID,
    EProgramOTPOperation_ReadCustomerID,
    EProgramOTPOperation_PermanentlyEnableSecureBoot,
    EProgramOTPOperation_GenerateHWUniqueEncryptionKey,
    EProgramOTPOperation_WriteAESKey,
    EProgramOTPOperation_ReadAESKey,
    EProgramOTPOperation_WriteECCKey,
    EProgramOTPOperation_ReadECCKey,

    EProgramOTPOperation_WriteUsrSlotG0,
    EProgramOTPOperation_ReadUsrSlotG0,
    EProgramOTPOperation_WriteHex2UsrSlotG0,
    EProgramOTPOperation_WriteUsrSlotG1,
    EProgramOTPOperation_ReadUsrSlotG1,
    EProgramOTPOperation_WriteHex2UsrSlotG1,

    EProgramOTPOperation_WriteTestRegion,
    EProgramOTPOperation_ReadTestRegion,
    EProgramOTPOperation_WriteHex2TestRegion,

    EProgramOTPOperation_RevokeROTKey,

    EProgramOTPOperation_QueryOTPSetting,

    /* optional */
    EProgramOTPOperation_SetJTAGEFuse,
    EProgramOTPOperation_LockZoneA,

    EProgramOTPOperation_EnAntiRollback,

    EProgramOTPOperation_WriteUsrDataG0,
    EProgramOTPOperation_ReadUsrDataG0,

    EProgramOTPOperation_WriteSysconfig,
    EProgramOTPOperation_QuerySysconfig,

    EProgramOTPOperation_WriteMiscConfig,
    EProgramOTPOperation_QueryMiscConfig,

    EProgramOTPOperation_WriteCSTSeedCUK,
    EProgramOTPOperation_QueryCSTSeed,
    EProgramOTPOperation_QueryCSTCUK,

    EProgramOTPOperation_WriteUSRCUK,
    EProgramOTPOperation_QueryUSRCUK,

    EProgramOTPOperation_DisSecureUSBBoot,

    EProgramOTPOperation_QueryBSTVer,
    EProgramOTPOperation_IncreaseBSTVer,

    EProgramOTPOperation_LockAllROTPubKeys,

    EProgramOTPOperation_DailyTest,
};

typedef struct {
    unsigned int operation;

    const char *pubkey_filename;
    const char *usrslot_filename;
    const char *test_region_filename;
    const char *hexstring;

    unsigned int lock;
    unsigned int index;
    unsigned int simulate_otp_write;

    unsigned int config_u32_0;
    unsigned int config_u32_1;
    unsigned int write_test_region_bit;

    const char *customer_uid_filename;
} test_program_otp_t;

static void __print_test_program_otp_helps(void)
{
    printf("test_program_otp's options\n");
    printf("\t'--writepubkey [%%d] [%%s] [%%d]': '--writepubkey' will write (%%d'th) ROT public key (file name %%s), lock (%%d)\n");
    printf("\t'--readpubkey [%%d]': '--readpubkey' will read (%%d'th) ROT public key\n");
    printf("\t'--readpubkeydigest [%%d]': '--readpubkeydigest' will read (%%d'th) ROT public key's digest\n");
    printf("\t'--readambauniqueid: read amba unique id\n");
    printf("\t'--readmonocounter [%%d]': read mono counter, (%%d'th counter)\n");
    printf("\t'--increasemonocounter [%%d]': increase mono counter, (%%d'th counter)\n");
    printf("\t'--writecustomeruid [%%s] [%%d]': write customer id (file name %%s) (lock or not %%d)\n");
    printf("\t'--readcustomeruid': read customer id\n");
    printf("\t'--permanentlyenablesecureboot': permanently enable secure boot\n");
    printf("\t'--generatehuk': generate HW unique encryption key\n");
    printf("\t'--writeaeskey [%%d] [%%d]': '--writeaeskey' will write (%%d'th) aes key, (lock or not %%d)\n");
    printf("\t'--writeecckey [%%d] [%%d]': '--writeecckey' will write (%%d'th) ecc key, (lock or not %%d)\n");
    printf("\t'--writeusrslotg0 [%%d] [%%s] [%%d]': '--writeusrslotg0' will write (%%d'th) usr slot g0 (file name %%s), (lock or not %%d)\n");
    printf("\t'--readusrslotg0 [%%d]': '--readusrslotg0' will read (%%d'th) usr slot g0 region\n");
    printf("\t'--writehex2usrslotg0 [%%d] [%%s]': '--writehex2usrslotg0' will write (%%d'th) usr slot g0 with hex string\n");
    printf("\t'--writeusrslotg1 [%%d] [%%s] [%%d]': '--writeusrslotg1' will write (%%d'th) usr slot g1 (file name %%s), (lock or not %%d)\n");
    printf("\t'--readusrslotg1 [%%d]': '--readusrslotg1' will read (%%d'th) usr slot g1 region\n");
    printf("\t'--writehex2usrslotg1 [%%d] [%%s]': '--writehex2usrslotg1' will write (%%d'th) usr slot g1 with hex string\n");
    printf("\t'--writetestregion [%%s] [%%d]': '--writetestregion' will write test region (file name %%s), (lock or not %%d)\n");
    printf("\t'--readtestregion': '--readtestregion' will read test region\n");
    printf("\t'--writehex2testregion [%%s]': '--writehex2testregion' will write test region with hex string\n");
    printf("\t'--revokekey [%%d]': '--revokekey' will revoke (%%d'th) key\n");
    printf("\t'--queryotpsetting': '--queryotpsetting' will query otp setting\n");
    printf("\t'--setjtagefuse': '--setjtagefuse' will set jtag efuse\n");
    printf("\t'--writesysconfig [%%x] [%%x] [%%d]': '--writesysconfig' will write sysconfig, first is sysconfig, second is sysconfig_mask, last is lock\n");
    printf("\t'--querysysconfig': '--querysysconfig' will query sysconfig\n");
    printf("\t'--writemiscconfig [%%x]': '--writemiscconfig' will write misc config, the parameter is misc config\n");
    printf("\t'--querymiscconfig': '--querymiscconfig' will query misc config\n");
    printf("\t'--writecstseedcuk': '--writecstseedcuk' will write customer planted seed and cuk\n");
    printf("\t'--querycstseed': '--querycstseed' will query customer planted seed\n");
    printf("\t'--querycstcuk': '--querycstcuk' will query customer planted cuk\n");
    printf("\t'--writeusrcuk': '--writeusrcuk' will write user planted cuk\n");
    printf("\t'--queryusrcuk': '--queryusrcuk' will query user planted cuk\n");
    printf("\t'--increasebstver': '--increasebstver' will increase bst version\n");
    printf("\t'--querybstver': '--querybstver' will query bst version\n");
    printf("\t'--enantirollback': '--enantirollback' will enable bst anti rollback\n");
    printf("\t'--dissecureusbboot': '--dissecureusbboot' will disable secure usb boot\n");
    printf("\t'--lockallrotpubkeys': '--lockallrotpubkeys' will lock all rot public keys\n");
    printf("\t'--lockzonea': '--lockzonea' will lockzonea, include sysconfig\n");
    printf("\t'--simulate': '--simulate' will simulate OTP write, it only print the OTP write addr and size, do not write OTP actually\n");
    printf("\t'--dailytest [%%d]': '--dailytest' will do a daily test, it will cover all APIs, and [%%d] measns try to write one bit in test region\n");
    printf("\t'--help': print help\n\n");
}

static int __init_test_program_otp_params(int argc, char **argv, test_program_otp_t *context)
{
    int i = 0;

    for (i = 1; i < argc; i++) {
        if (!strcmp("--writepubkey", argv[i])) {
            if ((i + 3) < argc) {
                int ret0;
                int ret1;
                ret0 = sscanf(argv[i + 1], "%d", &context->index);
                ret1 = sscanf(argv[i + 3], "%d", &context->lock);
                context->pubkey_filename = argv[i + 2];
                if ((1 == ret0) && (1 == ret1)) {
                    printf("[input argument] --writepubkey, key index = %d, filename = %s, lock %d.\n",
                        context->index, context->pubkey_filename, context->lock);
                } else {
                    printf("[input argument] --writepubkey: should follow key index (%%d),  filename (%%s) and lock (%%d).\n");
                    return (-1);
                }
                i += 3;
            } else {
                printf("[input argument] --writepubkey: should follow key index (%%d),  filename (%%s) and lock (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_WriteROTPubkey;
        } else if (!strcmp("--readpubkeydigest", argv[i])) {
            if ((i + 1) < argc) {
                int ret = sscanf(argv[i + 1], "%d", &context->index);
                if (1 == ret) {
                    printf("[input argument] --readpubkeydigest, key index = %d.\n",
                        context->index);
                } else {
                    printf("[input argument] --readpubkeydigest: should follow key index (%%d).\n");
                    return (-1);
                }
                i ++;
            } else {
                printf("[input argument] --readpubkeydigest: should follow key index (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_ReadROTPubkeyDigest;
        } else if (!strcmp("--readpubkey", argv[i])) {
            if ((i + 1) < argc) {
                int ret = sscanf(argv[i + 1], "%d", &context->index);
                if (1 == ret) {
                    printf("[input argument] --readpubkey, key index = %d.\n",
                        context->index);
                } else {
                    printf("[input argument] --readpubkey: should follow key index (%%d).\n");
                    return (-1);
                }
                i ++;
            } else {
                printf("[input argument] --readpubkey: should follow key index (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_ReadROTPubkey;
        } else if (!strcmp("--readambauniqueid", argv[i])) {
            context->operation = EProgramOTPOperation_ReadAmbaUniqueID;
        } else if (!strcmp("--readmonocounter", argv[i])) {
            if ((i + 1) < argc) {
                int ret = sscanf(argv[i + 1], "%d", &context->index);
                if (1 == ret) {
                    printf("[input argument] --readmonocounter, index = %d.\n",
                        context->index);
                } else {
                    printf("[input argument] --readmonocounter: should follow index (%%d).\n");
                    return (-1);
                }
                i ++;
            } else {
                printf("[input argument] --readmonocounter: should follow index (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_ReadMonoCounter;
        } else if (!strcmp("--increasemonocounter", argv[i])) {
            if ((i + 1) < argc) {
                int ret = sscanf(argv[i + 1], "%d", &context->index);
                if (1 == ret) {
                    printf("[input argument] --increasemonocounter, index = %d.\n",
                        context->index);
                } else {
                    printf("[input argument] --increasemonocounter: should follow index (%%d).\n");
                    return (-1);
                }
                i ++;
            } else {
                printf("[input argument] --increasemonocounter: should follow index (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_IncreaseMonoCounter;
        } else if (!strcmp("--writecustomeruid", argv[i])) {
            if ((i + 2) < argc) {
                context->customer_uid_filename = argv[i + 1];
                context->lock = atoi(argv[i + 2]);
                i += 2;
            } else {
                printf("[input argument] --writecustomeruid: should follow unique id filename (%%s) and lock (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_WriteCustomerID;
        } else if (!strcmp("--readcustomeruid", argv[i])) {
            context->operation = EProgramOTPOperation_ReadCustomerID;
        } else if (!strcmp("--permanentlyenablesecureboot", argv[i])) {
            context->operation = EProgramOTPOperation_PermanentlyEnableSecureBoot;
        } else if (!strcmp("--generatehuk", argv[i])) {
            context->operation = EProgramOTPOperation_GenerateHWUniqueEncryptionKey;
        } else if (!strcmp("--writeaeskey", argv[i])) {
            if ((i + 2) < argc) {
                int ret;
                ret = sscanf(argv[i + 1], "%d", &context->index);
                context->lock = atoi(argv[i + 2]);
                if (1 == ret) {
                    printf("[input argument] --writeaeskey, key index = %d, lock %d.\n",
                        context->index, context->lock);
                } else {
                    printf("[input argument] --writeaeskey: should follow key index (%%d) and lock (%%d).\n");
                    return (-1);
                }
                i += 2;
            } else {
                printf("[input argument] --writeaeskey: should follow key index (%%d) and lock (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_WriteAESKey;
        } else if (!strcmp("--writeecckey", argv[i])) {
            if ((i + 2) < argc) {
                int ret;
                ret = sscanf(argv[i + 1], "%d", &context->index);
                context->lock = atoi(argv[i + 2]);
                if (1 == ret) {
                    printf("[input argument] --writeecckey, key index = %d, lock %d.\n",
                        context->index, context->lock);
                } else {
                    printf("[input argument] --writeecckey: should follow key index (%%d) and lock (%%d).\n");
                    return (-1);
                }
                i += 2;
            } else {
                printf("[input argument] --writeecckey: should follow key index (%%d) and lock (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_WriteECCKey;
        } else if (!strcmp("--writeusrslotg0", argv[i])) {
            if ((i + 2) < argc) {
                int ret = sscanf(argv[i + 1], "%d", &context->index);
                context->usrslot_filename = argv[i + 2];
                context->lock = atoi(argv[i + 3]);
                if (1 == ret) {
                    printf("[input argument] --writeusrslotg0, usr slot index = %d, filename = %s, lock = %d.\n",
                        context->index, context->usrslot_filename, context->lock);
                } else {
                    printf("[input argument] --writeusrslotg0: should follow usr slot index (%%d), filename (%%s) and lock (%%d).\n");
                    return (-1);
                }
                i += 3;
            } else {
                printf("[input argument] --writeusrslotg0: should follow usr slot index (%%d), filename (%%s) and lock (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_WriteUsrSlotG0;
        } else if (!strcmp("--readusrslotg0", argv[i])) {
            if ((i + 1) < argc) {
                int ret = sscanf(argv[i + 1], "%d",
                    &context->index);
                if (1 == ret) {
                    printf("[input argument] --readusrslotg0, index = %d.\n",
                        context->index);
                } else {
                    printf("[input argument] --readusrslotg0: should follow index (%%d).\n");
                    return (-1);
                }
                i ++;
            } else {
                printf("[input argument] --readusrslotg0: should follow index (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_ReadUsrSlotG0;
        } else if (!strcmp("--writehex2usrslotg0", argv[i])) {
            if ((i + 2) < argc) {
                int ret = sscanf(argv[i + 1], "%d", &context->index);
                context->hexstring = argv[i + 2];
                if (1 == ret) {
                    printf("[input argument] --writehex2usrslotg0, usr slot index = %d, hex string = %s.\n",
                    context->index, context->hexstring);
                } else {
                    printf("[input argument] --writehex2usrslotg0: should follow usr slot (%%d) and hex string (%%s).\n");
                    return (-1);
                }
                i += 2;
            } else {
                printf("[input argument] --writehex2usrslotg0: should follow usr slot index (%%d) and hex string (%%s).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_WriteHex2UsrSlotG0;
        } else if (!strcmp("--writeusrslotg1", argv[i])) {
            if ((i + 3) < argc) {
                int ret;
                ret = sscanf(argv[i + 1], "%d", &context->index);
                context->usrslot_filename = argv[i + 2];
                context->lock = atoi(argv[i + 3]);
                if (1 == ret) {
                    printf("[input argument] --writeusrslotg1, usr slot index = %d, filename = %s, lock = %d.\n",
                        context->index, context->usrslot_filename, context->lock);
                } else {
                    printf("[input argument] --writeusrslotg1: should follow usr slot index (%%d), filename (%%s) and lock (%%d).\n");
                    return (-1);
                }
                i += 3;
            } else {
                printf("[input argument] --writeusrslotg1: should follow usr slot index (%%d), filename (%%s) and lock (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_WriteUsrSlotG1;
        } else if (!strcmp("--readusrslotg1", argv[i])) {
            if ((i + 1) < argc) {
                int ret = sscanf(argv[i + 1], "%d",
                    &context->index);
                if (1 == ret) {
                    printf("[input argument] --readusrslotg1, index = %d.\n",
                        context->index);
                } else {
                    printf("[input argument] --readusrslotg1: should follow index (%%d).\n");
                    return (-1);
                }
                i ++;
            } else {
                printf("[input argument] --readusrslotg1: should follow index (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_ReadUsrSlotG1;
        } else if (!strcmp("--writehex2usrslotg1", argv[i])) {
            if ((i + 2) < argc) {
                int ret = sscanf(argv[i + 1], "%d", &context->index);
                context->hexstring = argv[i + 2];
                if (1 == ret) {
                    printf("[input argument] --writehex2usrslotg1, usr slot index = %d, hex string = %s.\n",
                    context->index, context->hexstring);
                } else {
                    printf("[input argument] --writehex2usrslotg1: should follow usr slot (%%d) and hex string (%%s).\n");
                    return (-1);
                }
                i += 2;
            } else {
                printf("[input argument] --writehex2usrslotg1: should follow usr slot index (%%d) and hex string (%%s).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_WriteHex2UsrSlotG1;
        } else if (!strcmp("--writetestregion", argv[i])) {
            if ((i + 2) < argc) {
                context->test_region_filename = argv[i + 1];
                context->lock = atoi(argv[i + 2]);
                printf("[input argument] --writetestregion, test region filename = %s, lock = %d.\n",
                    context->test_region_filename, context->lock);
                i += 2;
            } else {
                printf("[input argument] --writetestregion: should follow test region filename (%%s) and lock (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_WriteTestRegion;
        } else if (!strcmp("--readtestregion", argv[i])) {
            context->operation = EProgramOTPOperation_ReadTestRegion;
        } else if (!strcmp("--writehex2testregion", argv[i])) {
            if ((i + 1) < argc) {
                context->hexstring = argv[i + 1];
                printf("[input argument] --writehex2testregion, hex string = %s.\n",
                    context->hexstring);
                i ++;
            } else {
                printf("[input argument] --writehex2testregion: should follow hex string (%%s).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_WriteHex2TestRegion;
        }  else if (!strcmp("--revokekey", argv[i])) {
            if ((i + 1) < argc) {
                int ret = sscanf(argv[i + 1], "%d",
                    &context->index);
                if (1 == ret) {
                    printf("[input argument] --revokekey, key index = %d.\n",
                        context->index);
                } else {
                    printf("[input argument] --revokekey: should follow key index (%%d).\n");
                    return (-1);
                }
                i ++;
            } else {
                printf("[input argument] --revokekey: should follow key index (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_RevokeROTKey;
        } else if (!strcmp("--dailytest", argv[i])) {
            if ((i + 1) < argc) {
                int ret = sscanf(argv[i + 1], "%d", &context->write_test_region_bit);
                if (1 == ret) {
                    printf("[input argument] --dailytest, write test region bit = %d.\n",
                    context->index);
                } else {
                    printf("[input argument] --dailytest: should follow write test region bit (%%d).\n");
                    return (-1);
                }
                i ++;
            } else {
                printf("[input argument] --dailytest: should follow write test region bit (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_DailyTest;
        } else if (!strcmp("--queryotpsetting", argv[i])) {
            context->operation = EProgramOTPOperation_QueryOTPSetting;
        } else if (!strcmp("--setjtagefuse", argv[i])) {
            context->operation = EProgramOTPOperation_SetJTAGEFuse;
        } else if (!strcmp("--lockzonea", argv[i])) {
            context->operation = EProgramOTPOperation_LockZoneA;
        } else if (!strcmp("--writeusrdatag0", argv[i])) {
            if ((i + 1) < argc) {
                int ret = sscanf(argv[i + 1], "%d", &context->lock);
                if (1 == ret) {
                    printf("[input argument] --writeusrdatag0, lock = %d.\n", context->lock);
                } else {
                    printf("[input argument] --writeusrdatag0: should follow lock (%%d).\n");
                    return (-1);
                }
                i ++;
            } else {
                printf("[input argument] --writeusrdatag0: should follow lock (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_WriteUsrDataG0;
        } else if (!strcmp("--readusrdatag0", argv[i])) {
            context->operation = EProgramOTPOperation_ReadUsrDataG0;
        } else if (!strcmp("--writesysconfig", argv[i])) {
            if ((i + 3) < argc) {
                int ret0 = 0, ret1 = 0;
                ret0 = sscanf(argv[i + 1], "%x", &context->config_u32_0);
                ret1 = sscanf(argv[i + 2], "%x", &context->config_u32_1);
                context->lock = atoi(argv[i + 3]);
                if ((1 == ret0) && (1 == ret1)) {
                    printf("[input argument] --writesysconfig, 0x%x, 0x%x lock %d.\n",
                        context->config_u32_0, context->config_u32_1, context->lock);
                } else {
                    printf("[input argument] --writesysconfig: should follow sysconfig (%%x), sysconfig mask (%%x) and lock (%%d).\n");
                    return (-1);
                }
                i += 3;
            } else {
                printf("[input argument] --writesysconfig: should follow sysconfig (%%x), sysconfig mask (%%x) and lock (%%d).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_WriteSysconfig;
        } else if (!strcmp("--querysysconfig", argv[i])) {
            context->operation = EProgramOTPOperation_QuerySysconfig;
        } else if (!strcmp("--writemiscconfig", argv[i])) {
            if ((i + 1) < argc) {
                int ret = sscanf(argv[i + 1], "%x", &context->config_u32_0);
                if (1 == ret) {
                    printf("[input argument] --writemiscconfig, 0x%x.\n",
                        context->config_u32_0);
                } else {
                    printf("[input argument] --writemiscconfig: should follow misc config (%%x).\n");
                    return (-1);
                }
                i += 2;
            } else {
                printf("[input argument] --writemiscconfig: should follow misc config (%%x).\n");
                return (-1);
            }
            context->operation = EProgramOTPOperation_WriteMiscConfig;
        } else if (!strcmp("--querymiscconfig", argv[i])) {
            context->operation = EProgramOTPOperation_QueryMiscConfig;
        } else if (!strcmp("--writecstseedcuk", argv[i])) {
            context->operation = EProgramOTPOperation_WriteCSTSeedCUK;
        } else if (!strcmp("--querycstseed", argv[i])) {
            context->operation = EProgramOTPOperation_QueryCSTSeed;
        } else if (!strcmp("--querycstcuk", argv[i])) {
            context->operation = EProgramOTPOperation_QueryCSTCUK;
        } else if (!strcmp("--writeusrcuk", argv[i])) {
            context->operation = EProgramOTPOperation_WriteUSRCUK;
        } else if (!strcmp("--queryusrcuk", argv[i])) {
            context->operation = EProgramOTPOperation_QueryUSRCUK;
        } else if (!strcmp("--increasebstver", argv[i])) {
            context->operation = EProgramOTPOperation_IncreaseBSTVer;
        } else if (!strcmp("--querybstver", argv[i])) {
            context->operation = EProgramOTPOperation_QueryBSTVer;
        } else if (!strcmp("--enantirollback", argv[i])) {
            context->operation = EProgramOTPOperation_EnAntiRollback;
        } else if (!strcmp("--dissecureusbboot", argv[i])) {
            context->operation = EProgramOTPOperation_DisSecureUSBBoot;
        } else if (!strcmp("--lockallrotpubkeys", argv[i])) {
            context->operation = EProgramOTPOperation_LockAllROTPubKeys;
        } else if (!strcmp("--simulate", argv[i])) {
            context->simulate_otp_write = 1;
        } else if (!strcmp("--help", argv[i])) {
            __print_test_program_otp_helps();
            return 1;
        } else {
            printf("error: NOT processed option(%s).\n", argv[i]);
            __print_test_program_otp_helps();
            return (-1);
        }
    }

    return 0;
}

static void __print_content_u16be_u8(
    unsigned char *p_content, unsigned int len)
{
    printf("u16 big endian:\n");
    print_memory_u16_be(p_content, len);
    printf("\t\tu8:\n");
    print_memory_u8(p_content, len);
}

static void __query_otp_version(void)
{
    int ret = 0;
    otp_version_t ver = {0};

    ret = do_query_version(&ver);
    if (ret < 0) {
        printf("[error]: do_query_version failed\n");
        return;
    }

    printf("otp api version %d.%d.%d:\n",
        ver.api_ver_major, ver.api_ver_minor, ver.api_ver_patch);
    printf("otp driver version %d.%d.%d:\n",
        ver.driver_ver_major, ver.driver_ver_minor, ver.driver_ver_patch);
    printf("last modificaton date %d-%d-%d:\n",
        ver.date_year, ver.date_month, ver.date_day);
}

static int __load_and_write_rot_pubkey(
    const char *keyfile, unsigned int index,
    unsigned int lock, unsigned int simulate)
{
    TEEC_Result result;
    unsigned int ret = 0;
    FILE *pf = NULL;
    unsigned char key_buf[ROT_PUBKEY_BITS / 8] = {0};

    if (keyfile) {
        pf = fopen(keyfile, "rb");
    }
    if (pf) {
        ret = fread(key_buf, 1, sizeof(key_buf), pf);
        if (ret == (unsigned int) sizeof(key_buf)) {
            printf("write pub key[%d], lock[%d]\n", index, lock);
            __print_content_u16be_u8((unsigned char *) key_buf, (int) sizeof(key_buf));
            result = do_write_otp_pubkey(key_buf,
                (unsigned int) sizeof(key_buf), index,
                lock, simulate);
            if (result != TEEC_SUCCESS) {
                printf("error: do_write_otp_pubkey failed, result 0x%x\n", result);
                return (-1);
            }
        } else {
            printf("error: file size not expected: %d, %d\n", ret, (int) sizeof(key_buf));
        }
        fclose(pf);
    } else {
        if (keyfile) {
            printf("file open failed: %s, write empty content, simulate\n", keyfile);
        }
        result = do_write_otp_pubkey(key_buf,
            (unsigned int) sizeof(key_buf), index,
            0, 1);
        if (result != TEEC_SUCCESS) {
            printf("error: do_write_otp_pubkey failed, result 0x%x\n", result);
            return (-1);
        }
    }

    return 0;
}

static int __read_rot_pubkey_digest(unsigned int index,
    int print_content)
{
    unsigned char digest[32] = {0};
    unsigned int locked = 0, revoked = 0;
    TEEC_Result result;

    result = do_read_otp_pubkey_digest((unsigned char *) digest, sizeof(digest),
        index, &locked, &revoked);
    if (result != TEEC_SUCCESS) {
        printf("do_read_otp_pubkey_digest failed, result 0x%x\n", result);
        return (-1);
    }
    if (print_content) {
        printf("rot pubkey[%d]: locked(%d), revoked(%d), digest:\n",
            index, locked, revoked);
        __print_content_u16be_u8((unsigned char *) digest, sizeof(digest));
    }
    return 0;
}

static int __read_rot_pubkey(int index, int print_content)
{
    unsigned char key_buf[ROT_PUBKEY_BITS / 8] = {0};
    unsigned int locked = 0, revoked = 0;
    TEEC_Result result;

    result = do_read_otp_pubkey((unsigned char *) key_buf, sizeof(key_buf),
        index, &locked, &revoked);
    if (result != TEEC_SUCCESS) {
        printf("do_read_otp_pubkey failed, result 0x%x\n", result);
        return (-1);
    }
    if (print_content) {
        printf("rot pubkey[%d]: locked(%d), revoked(%d), content:\n",
            index, locked, revoked);
        __print_content_u16be_u8((unsigned char *) key_buf, sizeof(key_buf));
    }
    return 0;
}

static int __read_amba_unique_id(int print_content)
{
    TEEC_Result result;
    unsigned char p[UNIQUE_ID_BITS / 8] = {0};

    result = do_read_otp_amba_unique_id(p, sizeof(p));
    if (result != TEEC_SUCCESS) {
        printf("do_read_otp_amba_unique_id failed, result 0x%x\n", result);
        return (-1);
    }
    if (print_content) {
        printf("query otp unique id:\n");
        __print_content_u16be_u8((unsigned char *) p, sizeof(p));
    }
    return 0;
}

static int __read_mono_counter(unsigned int index,
    int print_content)
{
    TEEC_Result result;
    unsigned int counter = 0;

    result = do_read_otp_mono_counter(&counter, index);
    if (result != TEEC_SUCCESS) {
        printf("do_read_otp_mono_counter failed, result 0x%x\n", result);
        return (-1);
    }
    if (print_content) {
        printf("mono counter[%d] %d\n", index, counter);
    }
    return 0;
}

static int __increase_mono_counter(unsigned int index,
    unsigned int simulate)
{
    TEEC_Result result;

    result = do_increase_otp_mono_counter(index, simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_increase_otp_mono_counter failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static int __write_customer_id(
    const char *file, unsigned int lock, unsigned int simulate)
{
    TEEC_Result result;
    unsigned int ret = 0;
    unsigned char buf[CUSTOMER_ID_BITS / 8] = {0};
    FILE *pf = NULL;

    pf = fopen(file, "rb");
    if (pf) {
        ret = fread(buf, 1, sizeof(buf), pf);
        if (ret == (unsigned int) sizeof(buf)) {
            printf("write customer id\n");
            __print_content_u16be_u8((unsigned char *) buf, (unsigned int) sizeof(buf));
            result = do_write_otp_customer_id(buf, (unsigned int) sizeof(buf),
                lock, simulate);
            if (result != TEEC_SUCCESS) {
                printf("do_write_otp_customer_id failed, result 0x%x\n", result);
                return (-1);
            }
        } else {
            printf("error: file size not expected: %d, %d\n",
                ret, (unsigned int) sizeof(buf));
            return (-2);
        }
    } else {
        printf("not valid file: %s, write empty context\n", file);
        result = do_write_otp_customer_id(buf, (unsigned int) sizeof(buf), 0, simulate);
        if (result != TEEC_SUCCESS) {
            printf("do_write_otp_customer_id failed, result 0x%x\n", result);
            return (-1);
        }
    }
    return 0;
}

static int __read_customer_id(int print_content)
{
    TEEC_Result result;
    unsigned int locked;
    unsigned char buf[CUSTOMER_ID_BITS / 8] = {0};

    result = do_read_otp_customer_id(buf,
        (unsigned int) sizeof(buf), &locked);
    if (result != TEEC_SUCCESS) {
        printf("do_read_otp_customer_id failed, result 0x%x\n", result);
        return (-1);
    }
    if (print_content) {
        printf("customer id (locked %d)\n", locked);
        __print_content_u16be_u8((unsigned char *) buf, (unsigned int) sizeof(buf));
    }
    return 0;
}

static int __permanently_enable_secure_boot(unsigned int simulate)
{
    TEEC_Result result;

    result = do_permanently_enable_secure_boot(simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_permanently_enable_secure_boot failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static int __generate_hw_unique_encryption_key(unsigned int simulate)
{
    TEEC_Result result;

    result = do_generate_hw_unique_encryption_key(simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_generate_hw_unique_encryption_key failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static int __revoke_rot_pubkey(unsigned int key_index, unsigned int simulate)
{
    TEEC_Result result;

    result = do_revoke_key(key_index, simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_revoke_key[%d] failed, result 0x%x\n", key_index, result);
        return (-1);
    }
    return 0;
}

static int __lock_rot_pubkey(unsigned int key_index, unsigned int simulate)
{
    TEEC_Result result;

    result = do_lock_rot_pubkey(key_index, simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_lock_rot_pubkey(%d) failed, result 0x%x\n",
            key_index, result);
        return (-1);
    }
    return 0;
}

static int __write_aes_key(unsigned int key_index,
    unsigned int lock, unsigned int simulate)
{
    TEEC_Result result;
    unsigned char test_key_content[AES_KEY_BITS / 8] = {
        0x0,
    };

    result = do_write_aes_key(test_key_content, sizeof(test_key_content), key_index,
        lock, simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_write_aes_key failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static int __write_ecc_key(unsigned int key_index,
    unsigned int lock, unsigned int simulate)
{
    TEEC_Result result;
    unsigned char test_key_content[ECC_KEY_BITS / 8] = {
        0x0,
    };

    result = do_write_ecc_key(test_key_content, sizeof(test_key_content), key_index,
        lock, simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_write_ecc_key failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static int __write_usr_slot_g0(unsigned int index,
    unsigned int lock, unsigned int simulate)
{
    TEEC_Result result;
    unsigned char test_content[USR_SLOT_G0_BITS / 8] = {
        0x0,
    };

    result = do_write_usr_slot_g0(test_content, sizeof(test_content),
        index, lock, simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_write_usr_slot_g0 failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static int __read_usr_slot_g0(unsigned int index,
    int print_content)
{
    TEEC_Result result;
    unsigned int locked = 0;
    unsigned char test_content[USR_SLOT_G0_BITS / 8] = {
        0x0,
    };

    result = do_read_usr_slot_g0(test_content, sizeof(test_content),
        index, &locked);
    if (result != TEEC_SUCCESS) {
        printf("do_read_usr_slot_g0 failed, result 0x%x\n", result);
        return (-1);
    }
    if (print_content) {
        printf("usr slot g0 [%d], locked %d.\n", index, locked);
        __print_content_u16be_u8((unsigned char *) test_content, sizeof(test_content));
    }
    return 0;
}

static int __write_usr_slot_g1(unsigned int index,
    unsigned int lock, unsigned int simulate)
{
    TEEC_Result result;
    unsigned char test_content[USR_SLOT_G1_BITS / 8] = {
        0x0,
    };

    result = do_write_usr_slot_g1(test_content, sizeof(test_content),
        index, lock, simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_write_usr_slot_g0 failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static int __read_usr_slot_g1(unsigned int index,
    int print_content)
{
    TEEC_Result result;
    unsigned int locked = 0;
    unsigned char test_content[USR_SLOT_G1_BITS / 8] = {
        0x0,
    };

    result = do_read_usr_slot_g1(test_content, sizeof(test_content),
        index, &locked);
    if (result != TEEC_SUCCESS) {
        printf("do_read_usr_slot_g1 failed, result %d\n", result);
        return (-1);
    }
    if (print_content) {
        printf("usr slot grp1 [%d], locked %d.\n", index, locked);
        __print_content_u16be_u8((unsigned char *) test_content, sizeof(test_content));
    }
    return 0;
}

static void __convert_hex_str_2_hex_bin(
    const char *hex_str, unsigned int  str_len,
    unsigned char *hex_mem)
{
    unsigned int v = 0;
    int ret = 0;

    while (str_len >= 2) {
        ret = sscanf(hex_str, "%2x", &v);
        if (1 != ret) {
            break;
        }
        hex_mem[0] = v & 0xff;
        hex_str += 2;
        hex_mem ++;
        str_len -= 2;
    }
}

static void __write_hex_2_usr_slot_g0(
    const char *hex, int index, unsigned int lock, unsigned int simulate)
{
    int ret = 0;
    unsigned int str_len = 0;

    unsigned char test_content[USR_SLOT_G0_BITS / 8] = {
        0x0,
    };

    str_len = strlen(hex);
    str_len = str_len & 0xfffffffe;
    if (str_len > (sizeof(test_content))) {
        str_len = sizeof(test_content);
    }
    __convert_hex_str_2_hex_bin(
        hex, str_len,
        test_content);

    printf("write usr slot g0 [%d]:\n", index);
    __print_content_u16be_u8((unsigned char *) test_content, sizeof(test_content));

    ret = do_write_usr_slot_g0(test_content, sizeof(test_content),
        index, lock, simulate);
    if (ret < 0) {
        printf("do_write_usr_slot_g0 failed\n");
        return;
    }
}

static void __write_hex_2_usr_slot_g1(
    const char *hex, int index, unsigned int lock,
    unsigned int simulate)
{
    int ret = 0;
    unsigned int str_len = 0;

    unsigned char test_content[USR_SLOT_G1_BITS / 8] = {
        0x0,
    };

    str_len = strlen(hex);
    str_len = str_len & 0xfffffffe;
    if (str_len > (sizeof(test_content))) {
        str_len = sizeof(test_content);
    }
    __convert_hex_str_2_hex_bin(
        hex, str_len,
        test_content);

    printf("write usr slot g1 [%d]:\n", index);
    __print_content_u16be_u8((unsigned char *) test_content, sizeof(test_content));

    ret = do_write_usr_slot_g1(test_content, sizeof(test_content),
        index, lock, simulate);
    if (ret < 0) {
        printf("do_write_usr_slot_g1 failed\n");
        return;
    }
}

static int __write_test_region(unsigned int lock, unsigned int simulate)
{
    TEEC_Result result;
    unsigned char test_content[TEST_REGION_BITS / 8] = {
        0x0,
    };

    result = do_write_test_region(test_content, sizeof(test_content),
        lock, simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_write_test_region failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static int __read_test_region(int print_content)
{
    TEEC_Result result;
    unsigned int locked = 0;
    unsigned char test_content[TEST_REGION_BITS / 8] = {
        0x0,
    };

    result = do_read_test_region(test_content, sizeof(test_content),
        &locked);
    if (result != TEEC_SUCCESS) {
        printf("do_read_test_region failed, result 0x%x\n", result);
        return (-1);
    }
    if (print_content) {
        printf("test region, locked %d.\n", locked);
        __print_content_u16be_u8((unsigned char *) test_content, sizeof(test_content));
    }
    return 0;
}

static void __write_hex_2_test_region(
    const char *hex, unsigned int lock, unsigned int simulate)
{
    int ret = 0;
    unsigned int str_len = 0;

    unsigned char test_content[TEST_REGION_BITS / 8] = {
        0x0,
    };

    str_len = strlen(hex);
    str_len = str_len & 0xfffffffe;
    if (str_len > (sizeof(test_content))) {
        str_len = sizeof(test_content);
    }
    __convert_hex_str_2_hex_bin(
        hex, str_len,
        test_content);

    printf("write test region:\n");
    __print_content_u16be_u8((unsigned char *) test_content, sizeof(test_content));

    ret = do_write_test_region(test_content, sizeof(test_content),
        lock, simulate);
    if (ret < 0) {
        printf("do_write_test_region failed\n");
        return;
    }
}

static void __print_otp_setting(otp_setting_t *setting)
{
    unsigned int i = 0;

    printf("otp layout version: %d\n", setting->otp_layout_ver);

    printf("otp lock bits: 0x%08x\n", setting->lock_bits);
    printf("otp data invalid bits: 0x%08x\n", setting->invalid_bits);
    printf("sysconfig[0~31]: 0x%08x\n", setting->sysconfig);
    printf("sysconfig[32~63]: 0x%08x\n", setting->sysconfig_mask);

    if (setting->secure_boot_permanent_en) {
        printf("secure boot is permanently enabled\n");
    } else {
        printf("secure boot is NOT permanently enabled\n");
    }

    if (setting->jtag_efuse) {
        printf("JTAG efuse is set\n");
    } else {
        printf("JTAG efuse is not set\n");
    }

    if (setting->sysconfig_locked) {
        printf("sysconfig is locked\n");
    } else {
        printf("sysconfig is not locked\n");
    }

    if (setting->huk_locked) {
        printf("HUK is locked\n");
    } else {
        printf("HUK is not locked\n");
    }

    if (setting->customer_id_locked) {
        printf("customer id is locked\n");
    } else {
        printf("customer id is not locked\n");
    }

    if (1 == setting->otp_layout_ver) {
        if (setting->zone_a_locked) {
            printf("zone a is locked\n");
        } else {
            printf("zone a is not locked\n");
        }
    } else if (2 == setting->otp_layout_ver) {
        if (setting->cst_seed_cuk_locked) {
            printf("customer planted seed and cuk is locked\n");
        } else {
            printf("customer planted seed and cuk is not locked\n");
        }

        if (setting->usr_cuk_locked) {
            printf("usr planted cuk is locked\n");
        } else {
            printf("usr planted cuk is not locked\n");
        }

        if (setting->anti_rollback_en) {
            printf("bst anti rollback is enabled\n");
        } else {
            printf("bst anti rollback is not enabled\n");
        }

        if (setting->secure_usb_boot_dis) {
            printf("secure usb boot is disabled\n");
        } else {
            printf("secure usb boot is not disabled\n");
        }
    }

    if (setting->all_rot_key_lock_together) {
        if (setting->lock_bits & (1 << setting->rot_pubkey_lock_base)) {
            printf("all rot keys are locked\n");
        } else {
            printf("all rot keys are not locked\n");
        }
    } else {
        for (i = 0; i < setting->num_of_rot_pub_keys; i ++) {
            if (setting->lock_bits & (1 << (setting->rot_pubkey_lock_base + i))) {
                printf("rot pubkey [%d] is locked\n", i);
            } else {
                printf("rot pubkey [%d] is NOT locked\n", i);
            }
        }
    }

    for (i = 0; i < setting->num_of_rot_pub_keys; i ++) {
        if (i == setting->not_revokeable_key_index) {
            printf("%d th pubkey is not revokeable\n",
                setting->not_revokeable_key_index);
            continue;
        }
        if (setting->invalid_bits & (1U << i)) {
            printf("rot pubkey [%d] is revoked\n", i);
        } else {
            printf("rot pubkey [%d] is NOT revoked\n", i);
        }
    }

    for (i = 0; i < setting->num_of_aes_keys; i ++) {
        if (setting->lock_bits & (1 << (setting->aes_key_lock_base + i))) {
            printf("aes key [%d] is locked\n", i);
        } else {
            printf("aes key [%d] is NOT locked\n", i);
        }
    }

    for (i = 0; i < setting->num_of_ecc_keys; i ++) {
        if (setting->lock_bits & (1 << (setting->ecc_key_lock_base + i))) {
            printf("ecc key [%d] is locked\n", i);
        } else {
            printf("ecc key [%d] is NOT locked\n", i);
        }
    }

    for (i = 0; i < setting->num_of_usr_slot_g0; i ++) {
        if (setting->lock_bits & (1 << (setting->usr_slot_g0_lock_base + i))) {
            printf("usr slot g0 [%d] is locked\n", i);
        } else {
            printf("usr slot g0 [%d] is NOT locked\n", i);
        }
    }

    for (i = 0; i < setting->num_of_usr_slot_g1; i ++) {
        if (setting->lock_bits & (1 << (setting->usr_slot_g1_lock_base + i))) {
            printf("usr slot g1 [%d] is locked\n", i);
        } else {
            printf("usr slot g1 [%d] is NOT locked\n", i);
        }
    }
}

static void __query_otp_setting(void)
{
    TEEC_Result result;
    otp_setting_t setting = {0};

    result = do_query_otp_setting(&setting);
    if (result != TEEC_SUCCESS) {
        printf("do_query_otp_setting failed, result %d\n", result);
        return;
    }

    __print_otp_setting(&setting);
}

static int __set_jtag_efuse(unsigned int simulate)
{
    TEEC_Result result;

    result = do_set_jtag_efuse(simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_set_jtag_efuse failed, result %d\n", result);
        return (-1);
    }
    return 0;
}

static int __lock_zone_a(unsigned int simulate)
{
    TEEC_Result result;

    result = do_lock_zone_a(simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_lock_zone_a failed, result %d\n", result);
        return (-1);
    }
    return 0;
}

static int __write_sysconfig(
    unsigned int config_val, unsigned int mask_val,
    unsigned int lock, unsigned int simulate)
{
    TEEC_Result result;
    unsigned int content[2] = {0};

    content[0] = config_val;
    content[1] = mask_val;
    result = do_write_sysconfig((unsigned char *) &content[0], sizeof(content),
        lock, simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_write_sysconfig failed, result %d\n", result);
        return (-1);
    }
    return 0;
}

static int __read_sysconfig(int print_content)
{
    TEEC_Result result;
    unsigned int locked = 0;
    unsigned int content[2] = {0};

    result = do_read_sysconfig((unsigned char *) &content[0], sizeof(content), &locked);
    if (result != TEEC_SUCCESS) {
        printf("do_read_sysconfig failed, result %d\n", result);
        return (-1);
    }

    if (print_content) {
        printf("read sysconfig, locked (%d): 0x%08x, 0x%08x\n",
            locked, content[0], content[1]);
    }
    return 0;
}

static int __write_misc_config(unsigned int config_val, unsigned int simulate)
{
    TEEC_Result result;

    result = do_write_misc_config(config_val, simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_write_misc_config failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static int __read_misc_config(int print_content)
{
    TEEC_Result result;
    unsigned int available_bits = 0;
    unsigned int config = 0;

    result = do_read_misc_config(&config, &available_bits);
    if (result != TEEC_SUCCESS) {
        printf("do_read_misc_config failed, result 0x%x\n", result);
        return (-1);
    }

    if (print_content) {
        printf("read misc config (avaiable bits %d): 0x%08x\n",
            available_bits, config);
    }
    return 0;
}

static int __read_cst_seed(int print_content)
{
    TEEC_Result result;
    unsigned int locked = 0;
    unsigned int seed = 0;

    result = do_read_cst_seed((unsigned char *) &seed, sizeof(seed), &locked);
    if (result != TEEC_SUCCESS) {
        printf("do_read_cst_seed failed, result 0x%x\n", result);
        return (-1);
    }

    if (print_content) {
        printf("query customer planted seed: 0x%08x\n", seed);
        if (locked) {
            printf("customer planted seed is locked\n");
        } else {
            printf("customer planted seed is not locked\n");
        }
    }
    return 0;
}

static int __read_cst_cuk(int print_content)
{
    TEEC_Result result;
    unsigned int locked = 0;
    unsigned char cuk[CST_PLANTED_CUK_BITS / 8] = {0};

    result = do_read_cst_cuk(&cuk[0], sizeof(cuk), &locked);
    if (result != TEEC_SUCCESS) {
        printf("do_read_cst_cuk failed, result 0x%x\n", result);
        return (-1);
    }

    if (print_content) {
        printf("query customer planted cuk:\n");
        __print_content_u16be_u8((unsigned char *) cuk, (unsigned int) sizeof(cuk));
        if (locked) {
            printf("customer planted cuk is locked\n");
        } else {
            printf("customer planted cuk is not locked\n");
        }
    }
    return 0;
}

static int __write_cst_seed_cuk(unsigned int lock, unsigned int simulate)
{
    TEEC_Result result;
    uint8_t seed_cuk[(CST_PLANTED_SEED_BITS + CST_PLANTED_CUK_BITS) / 8] = {0};

    result = do_write_cst_seed_cuk(seed_cuk, (unsigned int) sizeof(seed_cuk),
        lock, simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_write_cst_seed_cuk failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static int __read_usr_cuk(int print_content)
{
    TEEC_Result result;
    unsigned int locked = 0;
    unsigned char cuk[USR_PLANTED_CUK_BITS / 8] = {0};

    result = do_read_usr_cuk(&cuk[0], sizeof(cuk), &locked);
    if (result != TEEC_SUCCESS) {
        printf("do_read_usr_cuk failed, result 0x%x\n", result);
        return (-1);
    }

    if (print_content) {
        printf("query user planted cuk:\n");
        __print_content_u16be_u8((unsigned char *) cuk, (unsigned int) sizeof(cuk));
        if (locked) {
            printf("user planted cuk is locked\n");
        } else {
            printf("user planted cuk is not locked\n");
        }
    }
    return 0;
}

static int __write_usr_cuk(unsigned int lock, unsigned int simulate)
{
    TEEC_Result result;
    uint8_t cuk[USR_PLANTED_CUK_BITS / 8] = {0};

    result = do_write_usr_cuk(cuk, (unsigned int) sizeof(cuk),
        lock, simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_write_usr_cuk failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static int __read_bst_version(int print_content)
{
    TEEC_Result result;
    unsigned int version = 0;

    result = do_read_bst_version(&version);
    if (result != TEEC_SUCCESS) {
        printf("do_read_bst_version failed, result 0x%x\n", result);
        return (-1);
    }

    if (print_content) {
        printf("query bst version %d:\n", version);
    }
    return 0;
}

static int __increase_bst_version(unsigned int simulate)
{
    TEEC_Result result;

    result = do_increase_bst_version(simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_increase_bst_version failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static int __en_anti_rollback(unsigned int simulate)
{
    TEEC_Result result;

    result = do_enable_bst_anti_rollback(simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_enable_bst_anti_rollback failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static int __dis_secure_usb_boot(unsigned int simulate)
{
    TEEC_Result result;

    result = do_disable_secure_usb_boot(simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_disable_secure_usb_boot failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static int __lock_all_rot_pub_key(unsigned int simulate)
{
    TEEC_Result result;

    if (!simulate) {
        printf("!!!lock all rot public key, be caution, it will lock all rot public keys in OTP\n");
    }
    result = do_lock_rot_pubkey(ROT_KEY_NUM, simulate);
    if (result != TEEC_SUCCESS) {
        printf("do_lock_rot_pubkey(ROT_KEY_NUM) failed, result 0x%x\n", result);
        return (-1);
    }
    return 0;
}

static void __add_one_bit(unsigned char *p, unsigned int size)
{
    unsigned int i = 0, j = 0;
    for (i = 0; i < size; i ++) {
        for (j = 0; j < 8; j ++) {
            if (!(p[i] & (1 << j))) {
                p[i] |= (1 << j);
                return;
            }
        }
    }
    return;
}

static int otp_daily_test(int write_test_region)
{
    TEEC_Result result;
    int ret = 0;
    unsigned int i = 0;
    otp_setting_t setting = {0};

    unsigned int tot_test_num, pass_num, failed_num, not_tested_num;

    // query first
    result = do_query_otp_setting(&setting);
    if (result != TEEC_SUCCESS) {
        printf("do_query_otp_setting failed, result %d\n", result);
        return (-1);
    }
    __print_otp_setting(&setting);

    tot_test_num = 0;
    pass_num = 0;
    failed_num = 0;
    not_tested_num = 0;

    printf("\n\nstart otp daily test\n");

    printf("check 'read amba unique id' ...\n");
    ret = __read_amba_unique_id(0);
    tot_test_num ++;
    if (!ret) {
        printf("\tpass\n");
        pass_num ++;
    } else {
        printf("\tfailed\n");
        failed_num ++;
    }

    printf("check 'permanently enable secure boot' ...\n");
    if (setting.secure_boot_permanent_en) {
        printf("\t\tsecure boot is already permanently enabled\n");
        not_tested_num ++;
    } else {
        ret = __permanently_enable_secure_boot(1);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }
    }

    printf("check 'JTAG efuse' ...\n");
    if (setting.jtag_efuse) {
        printf("\t\tJTAG efuse is set\n");
        not_tested_num ++;
    } else {
        ret = __set_jtag_efuse(1);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }
    }

    printf("check 'sysconfig' ...\n");
    if (setting.sysconfig_locked) {
        printf("\t\tsysconfig is locked\n");
        not_tested_num ++;
    } else {
        printf("simulate write sysconfig\n");
        ret = __write_sysconfig(
            0, 0,
            1, 1);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }

        printf("query sysconfig\n");
        ret = __read_sysconfig(1);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }
    }

    printf("check 'gen HUK' ...\n");
    if (setting.huk_locked) {
        printf("\t\tHUK is locked\n");
        not_tested_num ++;
    } else {
        ret = __generate_hw_unique_encryption_key(1);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }
    }

    printf("check 'customer id' ...\n");
    if (setting.customer_id_locked) {
        printf("\t\tcustomer id is locked\n");
        not_tested_num ++;
    } else {
        printf("simulate write customer id\n");
        ret = __write_customer_id(
            NULL, 1, 1);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }

        printf("query customer id\n");
        ret = __read_customer_id(0);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }
    }

    printf("check 'rot key' ...\n");
    if (setting.all_rot_key_lock_together) {
        if (setting.lock_bits & (1 << setting.rot_pubkey_lock_base)) {
            printf("\t\tall rot keys are locked\n");
            not_tested_num += setting.num_of_rot_pub_keys;
        } else {
            for (i = 0; i < setting.num_of_rot_pub_keys; i ++) {
                printf("simulate write rot pubkey [%d]\n", i);
                ret = __load_and_write_rot_pubkey(
                    NULL, (int) i,
                    0, 1);
                tot_test_num ++;
                if (!ret) {
                    printf("\tpass\n");
                    pass_num ++;
                } else {
                    printf("\tfailed\n");
                    failed_num ++;
                }

                if (i != setting.not_revokeable_key_index) {
                    if (setting.invalid_bits & (1U << i)) {
                        printf("\t\trot pubkey [%d] is already revoked\n", i);
                        not_tested_num ++;
                    } else {
                        printf("simulate revoke rot pubkey [%d]\n", i);
                        ret = __revoke_rot_pubkey(
                            i, 1);
                        tot_test_num ++;
                        if (!ret) {
                            printf("\tpass\n");
                            pass_num ++;
                        } else {
                            printf("\tfailed\n");
                            failed_num ++;
                        }
                    }
                }
            }

            printf("simulate lock all rot pubkey\n");
            ret = __lock_rot_pubkey(
                setting.num_of_rot_pub_keys, 1);
            tot_test_num ++;
            if (!ret) {
                printf("\tpass\n");
                pass_num ++;
            } else {
                printf("\tfailed\n");
                failed_num ++;
            }
        }
    } else {
        for (i = 0; i < setting.num_of_rot_pub_keys; i ++) {
            if (setting.lock_bits & (1 << (setting.rot_pubkey_lock_base + i))) {
                printf("\t\trot pubkey [%d] is locked\n", i);
                not_tested_num ++;
            } else {
                printf("simulate write rot pubkey [%d]\n", i);
                ret = __load_and_write_rot_pubkey(
                    NULL, (int) i,
                    0, 1);
                tot_test_num ++;
                if (!ret) {
                    printf("\tpass\n");
                    pass_num ++;
                } else {
                    printf("\tfailed\n");
                    failed_num ++;
                }

                printf("simulate lock rot pubkey [%d]\n", i);
                ret = __lock_rot_pubkey(i, 1);
                tot_test_num ++;
                if (!ret) {
                    printf("\tpass\n");
                    pass_num ++;
                } else {
                    printf("\tfailed\n");
                    failed_num ++;
                }

                if (i != setting.not_revokeable_key_index) {
                    if (setting.invalid_bits & (1U << i)) {
                        printf("\t\trot pubkey [%d] is already revoked\n", i);
                        not_tested_num ++;
                    } else {
                        printf("simulate revoke rot pubkey [%d]\n", i);
                        ret = __revoke_rot_pubkey(
                            i, 1);
                        tot_test_num ++;
                        if (!ret) {
                            printf("\tpass\n");
                            pass_num ++;
                        } else {
                            printf("\tfailed\n");
                            failed_num ++;
                        }
                    }
                }
            }
        }
    }

    for (i = 0; i < setting.num_of_rot_pub_keys; i ++) {
        printf("query rot pubkey [%d]\n", i);
        ret = __read_rot_pubkey(i, 0);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }
    }

    printf("check 'aes key' ...\n");
    for (i = 0; i < setting.num_of_aes_keys; i ++) {
        if (setting.lock_bits & (1 << (setting.aes_key_lock_base + i))) {
            printf("\t\taes key [%d] is locked\n", i);
            not_tested_num ++;
        } else {
            printf("simulate write aes key [%d]\n", i);
            ret = __write_aes_key(
                i, 0, 1);
            tot_test_num ++;
            if (!ret) {
                printf("\tpass\n");
                pass_num ++;
            } else {
                printf("\tfailed\n");
                failed_num ++;
            }
        }
    }

    printf("check 'ecc key' ...\n");
    for (i = 0; i < setting.num_of_ecc_keys; i ++) {
        if (setting.lock_bits & (1 << (setting.ecc_key_lock_base + i))) {
            printf("\t\tecc key [%d] is locked\n", i);
            not_tested_num ++;
        } else {
            printf("simulate write ecc key [%d]\n", i);
            ret = __write_ecc_key(
                i, 0, 1);
            tot_test_num ++;
            if (!ret) {
                printf("\tpass\n");
                pass_num ++;
            } else {
                printf("\tfailed\n");
                failed_num ++;
            }
        }
    }

    printf("check 'usr slot g0' ...\n");
    for (i = 0; i < setting.num_of_usr_slot_g0; i ++) {
        if (setting.lock_bits & (1 << (setting.usr_slot_g0_lock_base + i))) {
            printf("\t\tusr slot g0 [%d] is locked\n", i);
            not_tested_num ++;
        } else {
            printf("simulate write usr slot g0 [%d]\n", i);
            ret = __write_usr_slot_g0(i, 0, 1);
            tot_test_num ++;
            if (!ret) {
                printf("\tpass\n");
                pass_num ++;
            } else {
                printf("\tfailed\n");
                failed_num ++;
            }

            printf("read usr slot g0 [%d]\n", i);
            ret = __read_usr_slot_g0(i, 0);
            tot_test_num ++;
            if (!ret) {
                printf("\tpass\n");
                pass_num ++;
            } else {
                printf("\tfailed\n");
                failed_num ++;
            }
        }
    }

    printf("check 'usr slot g1' ...\n");
    for (i = 0; i < setting.num_of_usr_slot_g1; i ++) {
        if (setting.lock_bits & (1 << (setting.usr_slot_g1_lock_base + i))) {
        printf("\t\tusr slot g1 [%d] is locked\n", i);
        not_tested_num ++;
        } else {
            printf("simulate write usr slot g1 [%d]\n", i);
            ret = __write_usr_slot_g1(i, 0, 1);
            tot_test_num ++;
            if (!ret) {
                printf("\tpass\n");
                pass_num ++;
            } else {
                printf("\tfailed\n");
                failed_num ++;
            }

            printf("read usr slot g1 [%d]\n", i);
            ret = __read_usr_slot_g1(i, 0);
            tot_test_num ++;
            if (!ret) {
                printf("\tpass\n");
                pass_num ++;
            } else {
                printf("\tfailed\n");
                failed_num ++;
            }
        }
    }

    printf("check 'mono counters' ...\n");
    for (i = 0; i < NUM_OF_MONO_CNT; i ++) {
        printf("simulate increase mono counter [%d]\n", i);
        ret =  __increase_mono_counter(i, 1);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }
        printf("read mono counter [%d]\n", i);
        ret =  __read_mono_counter(i, 0);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }
    }

    printf("check 'misc config' ...\n");
    printf("simulate write misc config\n");
    ret =  __write_misc_config(0, 1);
    tot_test_num ++;
    if (!ret) {
        printf("\tpass\n");
        pass_num ++;
    } else {
        printf("\tfailed\n");
        failed_num ++;
    }
    printf("read misc config\n");
    ret =  __read_misc_config(0);
    tot_test_num ++;
    if (!ret) {
        printf("\tpass\n");
        pass_num ++;
    } else {
        printf("\tfailed\n");
        failed_num ++;
    }

    if (2 == setting.otp_layout_ver) {
        printf("check 'cst seed cuk' ...\n");
        if (setting.cst_seed_cuk_locked) {
            printf("cst seed cuk is already locked\n");
            not_tested_num ++;
        } else {
            printf("simulate write cst seed cuk\n");
            ret =  __write_cst_seed_cuk(0, 1);
            tot_test_num ++;
            if (!ret) {
                printf("\tpass\n");
                pass_num ++;
            } else {
                printf("\tfailed\n");
                failed_num ++;
            }
        }

        printf("read cst seed\n");
        ret =  __read_cst_seed(0);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }
        printf("read cst cuk\n");
        ret =  __read_cst_cuk(0);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }

        printf("check 'usr cuk' ...\n");
        if (setting.usr_cuk_locked) {
            printf("usr cuk is already locked\n");
            not_tested_num ++;
        } else {
            printf("simulate write usr cuk\n");
            ret =  __write_usr_cuk(0, 1);
            tot_test_num ++;
            if (!ret) {
                printf("\tpass\n");
                pass_num ++;
            } else {
                printf("\tfailed\n");
                failed_num ++;
            }
        }
        printf("read usr cuk\n");
        ret =  __read_usr_cuk(0);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }

        printf("check 'bst anti rollback' ...\n");
        if (setting.anti_rollback_en) {
            printf("bst anti rollback is already enabled\n");
            not_tested_num ++;
        } else {
            printf("simulate enable bst anti rollback\n");
            ret =  __en_anti_rollback(1);
            tot_test_num ++;
            if (!ret) {
                printf("\tpass\n");
                pass_num ++;
            } else {
                printf("\tfailed\n");
                failed_num ++;
            }
        }

        printf("check 'secure usb boot' ...\n");
        if (setting.secure_usb_boot_dis) {
            printf("secure usb boot is already disabled\n");
            not_tested_num ++;
        } else {
            printf("simulate disable secure usb boot\n");
            ret =  __dis_secure_usb_boot(1);
            tot_test_num ++;
            if (!ret) {
                printf("\tpass\n");
                pass_num ++;
            } else {
                printf("\tfailed\n");
                failed_num ++;
            }
        }

        printf("check 'bst version' ...\n");
        printf("simulate increase bst version\n");
        ret =  __increase_bst_version(1);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }
        printf("read bst version\n");
        ret =  __read_bst_version(0);
        tot_test_num ++;
        if (!ret) {
            printf("\tpass\n");
            pass_num ++;
        } else {
            printf("\tfailed\n");
            failed_num ++;
        }
    }

    printf("check 'test region' ...\n");
    {
        unsigned int locked = 0;
        unsigned int simulate = 0;
        unsigned char test_content[TEST_REGION_BITS / 8] = {
            0x0,
        };

        printf("read test region\n");
        result = do_read_test_region(test_content, sizeof(test_content),
            &locked);
        tot_test_num ++;
        if (result != TEEC_SUCCESS) {
            printf("do_read_test_region failed, result 0x%x\n", result);
            failed_num ++;
            printf("\tfailed\n");
        } else {
            pass_num ++;
            printf("\tpass\n");
        }

        if (locked) {
            printf("\t\ttest region is locked\n");
            not_tested_num ++;
        } else {
            if (write_test_region) {
                simulate = 0;
                printf("\t\ttest region current content:\n");
                __print_content_u16be_u8((unsigned char *) &test_content[0], sizeof(test_content));

                // add one bit
                __add_one_bit((unsigned char *) &test_content[0],
                    (unsigned int) sizeof(test_content));

                printf("\t\ttest region, add one bit, current content:\n");
                __print_content_u16be_u8((unsigned char *) &test_content[0], sizeof(test_content));

            } else {
                simulate = 1;
            }

            printf("write test region, simulate %d\n", simulate);
            result = do_write_test_region(test_content, sizeof(test_content),
                0, simulate);
            tot_test_num ++;
            if (result != TEEC_SUCCESS) {
                printf("do_write_test_region failed, result 0x%x\n", result);
                failed_num ++;
                printf("\tfailed\n");
            } else {
                if (!write_test_region) {
                    pass_num ++;
                    printf("\tpass\n");
                } else {
                    // check content
                    unsigned char read_back[TEST_REGION_BITS / 8] = {
                        0x0,
                    };

                    printf("read test region\n");
                    result = do_read_test_region(read_back, sizeof(read_back),
                        &locked);
                    if (result != TEEC_SUCCESS) {
                        failed_num ++;
                        printf("\tfailed\n");
                    } else {
                        if (!memcmp(&test_content[0], &read_back[0], sizeof(read_back))) {
                            printf("\tpass\n");
                            pass_num ++;
                        } else {
                            failed_num ++;
                            printf("\tfailed\n");
                            printf("\t\ttest region, read back content:\n");
                            __print_content_u16be_u8((unsigned char *) &read_back[0], sizeof(read_back));
                        }
                    }
                }
            }
        }
    }

    printf("otp daily test result: total test num %d, pass %d, failed %d, not-tested %d\n",
        tot_test_num, pass_num, failed_num, not_tested_num);

    return 0;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    test_program_otp_t context;

    memset(&context, 0x0, sizeof(context));

    if (argc > 1) {
        ret = __init_test_program_otp_params(argc, argv, &context);
        if (ret) {
            return (-2);
        }
    } else {
        __print_test_program_otp_helps();
        return (1);
    }

    __query_otp_version();

    switch (context.operation) {

        case EProgramOTPOperation_WriteROTPubkey:
            __load_and_write_rot_pubkey(context.pubkey_filename,
                context.index, context.lock, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_ReadROTPubkeyDigest:
            __read_rot_pubkey_digest(context.index, 1);
            break;

        case EProgramOTPOperation_ReadROTPubkey:
            __read_rot_pubkey(context.index, 1);
            break;

        case EProgramOTPOperation_ReadAmbaUniqueID:
            __read_amba_unique_id(1);
            break;

        case EProgramOTPOperation_ReadMonoCounter:
            __read_mono_counter(context.index, 1);
            break;

        case EProgramOTPOperation_IncreaseMonoCounter:
            __increase_mono_counter(context.index,
                context.simulate_otp_write);
            break;

        case EProgramOTPOperation_WriteCustomerID:
            __write_customer_id(context.customer_uid_filename,
                context.lock, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_ReadCustomerID:
            __read_customer_id(1);
            break;

        case EProgramOTPOperation_PermanentlyEnableSecureBoot:
            __permanently_enable_secure_boot(context.simulate_otp_write);
            break;

        case EProgramOTPOperation_GenerateHWUniqueEncryptionKey:
            __generate_hw_unique_encryption_key(context.simulate_otp_write);
            break;

        case EProgramOTPOperation_WriteAESKey:
            __write_aes_key(context.index, context.lock, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_WriteECCKey:
            __write_ecc_key(context.index, context.lock, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_WriteUsrSlotG0:
            __write_usr_slot_g0(context.index, context.lock, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_ReadUsrSlotG0:
            __read_usr_slot_g0(context.index, 1);
            break;

        case EProgramOTPOperation_WriteHex2UsrSlotG0:
            __write_hex_2_usr_slot_g0(context.hexstring,
                context.index, context.lock, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_WriteUsrSlotG1:
            __write_usr_slot_g1(context.index, context.lock, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_ReadUsrSlotG1:
            __read_usr_slot_g1(context.index, 1);
            break;

        case EProgramOTPOperation_WriteHex2UsrSlotG1:
            __write_hex_2_usr_slot_g1(context.hexstring,
                context.index, context.lock, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_WriteTestRegion:
            __write_test_region(context.lock, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_ReadTestRegion:
            __read_test_region(1);
            break;

        case EProgramOTPOperation_WriteHex2TestRegion:
            __write_hex_2_test_region(context.hexstring,
                context.lock, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_RevokeROTKey:
            __revoke_rot_pubkey(context.index, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_QueryOTPSetting:
            __query_otp_setting();
            break;

        case EProgramOTPOperation_SetJTAGEFuse:
            __set_jtag_efuse(context.simulate_otp_write);
            break;

        case EProgramOTPOperation_LockZoneA: {
                char y_or_n = 'n';
                printf("Please read below carefully first:\n");
                printf("  Zone A in OTP include sysconfig, so if Zone A is locked, sysconfig will not be changed anymore.\n");
                printf("  sysconfig include efuse bits of system: 'permanently enable secure bit' and 'JTAG eFuse bit', it's recommended to set those bit first, then lock Zone A.\n");
                printf("  'permanently enable secure boot bit' is efuse bit for secure boot, after set, secure boot is always enabled on chip, chip is secure and difficult to debug.\n");
                printf("  'JTAG efuse bit' is efuse bit for JTAG, it's recommend to set it before lock zone a.\n");
                printf("\n");
                printf("Do you really decide to lock zone A for this chip?\n");

                ret = scanf("%c", &y_or_n);
                if ((1 == ret) && ('y' == y_or_n)) {
                    __lock_zone_a(context.simulate_otp_write);
                }
            }
            break;

        case EProgramOTPOperation_WriteSysconfig:
            __write_sysconfig(
                context.config_u32_0, context.config_u32_1,
                context.lock, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_QuerySysconfig:
            __read_sysconfig(1);
            break;

        case EProgramOTPOperation_WriteMiscConfig:
            __write_misc_config(context.config_u32_0, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_QueryMiscConfig:
            __read_misc_config(1);
            break;

        case EProgramOTPOperation_WriteCSTSeedCUK:
            __write_cst_seed_cuk(context.lock, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_QueryCSTSeed:
            __read_cst_seed(1);
            break;

        case EProgramOTPOperation_QueryCSTCUK:
            __read_cst_cuk(1);
            break;

        case EProgramOTPOperation_WriteUSRCUK:
            __write_usr_cuk(context.lock, context.simulate_otp_write);
            break;

        case EProgramOTPOperation_QueryUSRCUK:
            __read_usr_cuk(1);
            break;

        case EProgramOTPOperation_IncreaseBSTVer:
            __increase_bst_version(context.simulate_otp_write);
            break;

        case EProgramOTPOperation_QueryBSTVer:
            __read_bst_version(1);
            break;

        case EProgramOTPOperation_EnAntiRollback:
            __en_anti_rollback(context.simulate_otp_write);
            break;

        case EProgramOTPOperation_DisSecureUSBBoot:
            __dis_secure_usb_boot(context.simulate_otp_write);
            break;

        case EProgramOTPOperation_LockAllROTPubKeys:
            __lock_all_rot_pub_key(context.simulate_otp_write);
            break;

        case EProgramOTPOperation_DailyTest:
            otp_daily_test(context.write_test_region_bit);
            break;

        default:
            break;
    }

    return 0;
}


