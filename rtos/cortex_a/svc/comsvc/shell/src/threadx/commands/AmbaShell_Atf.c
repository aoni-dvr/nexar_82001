/**
 *  @file AmbaDiag_ATF.c
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details ATF diagnostic command
 *
 */

#include "AmbaTypes.h"

#include "AmbaIntrinsics.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaCache.h"
#include "AmbaPrint.h"
#include "AmbaFS.h"
#include "AmbaATF.h"
#include "AmbaIntrinsics.h"
#include "AmbaMisraFix.h"
#include "AmbaShell_Commands.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define SHELL_ATF_KEY_SIZE  32U
#define OTP_LAYOUT_V2
#else
#define SHELL_ATF_KEY_SIZE  512U
#define OTP_LAYOUT_V1
#endif

static void SHELL_ATFCmdUsage(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    (void)ArgCount;
    (void)pArgVector;
    PrintFunc("Usage:\n");
    PrintFunc("atf genpubkey                   : Generate Public key from N and X value\n");
    PrintFunc("atf writepubkey                 : Write & Lock Public key \n");
    PrintFunc("atf readpubkey                  : Read Public key from OTP\n");
#if defined(OTP_LAYOUT_V2)
    PrintFunc("atf lockeccpubkey               : Lock All ECC Public key\n");
#endif
    PrintFunc("atf permanentlyenablesecureboot : Permanently Enable Secure Boot\n");
}

void AmbaShell_CommandAtf(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    /* The buffer of stored public key */
    static UINT8 pukey[SHELL_ATF_KEY_SIZE] __attribute__((aligned(4096))) = {0U};

    if (ArgCount < 2U) {
        SHELL_ATFCmdUsage(ArgCount, pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "genpubkey", AmbaUtility_StringLength("genpubkey")) == 0) {
#if defined(OTP_LAYOUT_V2)
            AmbaPrint_PrintStr5("Not support genpubkey in CV5x", NULL, NULL, NULL, NULL, NULL);
#else
            if (ArgCount == 4U) {
                AMBA_FS_FILE *pFile = NULL;
                UINT8 N[256] = {0}, X[256] = {0}, Buffer[SHELL_ATF_KEY_SIZE];
                char ReadByte[8] = "0x00";
                char PubKeyFn[16] = "C:\\pubkey.bin";
                UINT32 Hex, Num, idx, BufIdx;

                idx = sizeof(N) - 1U;
                if (AMBA_FS_ERR_NONE == AmbaFS_FileOpen(pArgVector[2], "r", &pFile)) {
                    if (AMBA_FS_ERR_NONE == AmbaFS_FileRead(Buffer, sizeof(UINT8), SHELL_ATF_KEY_SIZE, pFile, &Num)) {
                        (void )AmbaFS_FileClose(pFile);
                        if (Num == SHELL_ATF_KEY_SIZE) {
                            BufIdx = 0U;
                            while (BufIdx < (Num - 1U)) {
                                ReadByte[2] = (char )Buffer[BufIdx];
                                ReadByte[3] = (char )Buffer[BufIdx + 1U];
                                (void )AmbaUtility_StringToUInt32(ReadByte, &Hex);

                                N[idx] = (UINT8)(Hex & 0xFFU);
                                idx --;
                                BufIdx += 2U;
                            }
                        }
                    }
                }

                idx = sizeof(X) - 1U;
                if (AMBA_FS_ERR_NONE == AmbaFS_FileOpen(pArgVector[3], "r", &pFile)) {
                    if (AMBA_FS_ERR_NONE == AmbaFS_FileRead(Buffer, sizeof(UINT8), SHELL_ATF_KEY_SIZE, pFile, &Num)) {
                        (void )AmbaFS_FileClose(pFile);
                        if (Num == SHELL_ATF_KEY_SIZE) {
                            BufIdx = 0U;
                            while (BufIdx < (Num - 1U)) {
                                ReadByte[2] = (char )Buffer[BufIdx];
                                ReadByte[3] = (char )Buffer[BufIdx + 1U];
                                (void )AmbaUtility_StringToUInt32((char *)ReadByte, &Hex);

                                X[idx] = (UINT8)(Hex & 0xFFU);
                                idx --;
                                BufIdx += 2U;
                            }
                        }
                    }
                }
                PubKeyFn[0] = pArgVector[2][0];
                if (AMBA_FS_ERR_NONE == AmbaFS_FileOpen(PubKeyFn, "wb", &pFile)) {
                    if (AMBA_FS_ERR_NONE == AmbaFS_FileWrite(N, sizeof(UINT8), sizeof(N), pFile, &Num)) {
                        if (AMBA_FS_ERR_NONE == AmbaFS_FileWrite(X, sizeof(UINT8), sizeof(X), pFile, &Num)) {
                            AmbaPrint_PrintStr5("Gen %s success\n", PubKeyFn, NULL, NULL, NULL, NULL);
                        } else {
                            PrintFunc("Write X values fail\n");
                        }
                    } else {
                        PrintFunc("Write N values fail\n");
                    }
                    (void )AmbaFS_FileClose(pFile);
                } else {
                    AmbaPrint_PrintStr5("Open %s fail\n", PubKeyFn, NULL, NULL, NULL, NULL);
                }
            } else {
                PrintFunc("Usage: atf genpubkey [N value filename] [X value filename]\n");
            }
#endif
        } else if (AmbaUtility_StringCompare(pArgVector[1], "writepubkey", AmbaUtility_StringLength("writepubkey")) == 0) {
            if (ArgCount == 4U) {
                /* Write Public key into OTP */
                const void *ptr_pukey = pukey;
                UINT32 key_index = 0, pukey_addr;
                UINT32 ret;
                AMBA_FS_FILE *pFile;

                if (AMBA_FS_ERR_NONE == AmbaFS_FileOpen(pArgVector[2], "rb", &pFile)) {
                    if (AMBA_FS_ERR_NONE == AmbaFS_FileRead(pukey, sizeof(UINT8), sizeof(pukey), pFile, &ret)) {
                        if (ret == SHELL_ATF_KEY_SIZE) {
                            AmbaMisra_TypeCast32(&pukey_addr, &ptr_pukey);
                            (void )AmbaCache_DataClean((UINT32)pukey_addr, ret);
                            (void )AmbaUtility_StringToUInt32(pArgVector[3], &key_index);
                            if (key_index < 3U) {
                                ret = AmbaSmcCall(SMC32_AMBA_SIP_OTP_SET_PUKEY, (UINT32)pukey_addr, sizeof(pukey), key_index, 0U);
                                AmbaPrint_PrintUInt5("SMC32_AMBA_SIP_OTP_SET_PUKEY ret 0x%x result 0x%x 0x%x 0x%x 0x%x\n", ret, pukey[0], pukey[1], pukey[2], pukey[3]);
                            }
#if defined(OTP_LAYOUT_V1)
                            if (ret == 0U) {
                                ret = AmbaSmcCall(SMC32_AMBA_OTP_LOCK_PUKEY, key_index, 0U, 0U, 0U);
                                AmbaPrint_PrintUInt5("SMC32_AMBA_OTP_LOCK_PUKEY %d ret 0x%x \n", key_index, ret, 0U, 0U, 0U);
                            }
#endif
                        }
                    }
                }
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "readpubkey", AmbaUtility_StringLength("readpubkey")) == 0) {
            if (ArgCount == 3U) {
                /* Read Public key from OTP */
                const void *ptr_pukey = pukey;
                UINT32 key_index = 0U, pukey_addr;
                UINT32 ret, i;

                AmbaMisra_TypeCast32(&pukey_addr, &ptr_pukey);
                (void )AmbaCache_DataClean((UINT32)pukey_addr, sizeof(pukey));
                (void )AmbaUtility_StringToUInt32(pArgVector[2], &key_index);
                if (key_index < 3U) {
                    ret = AmbaSmcCall(SMC32_AMBA_SIP_OTP_GET_PUKEY, pukey_addr, sizeof(pukey), key_index, 0U);
                    (void )AmbaCache_DataInvalidate((UINT32)pukey_addr, sizeof(pukey));
                    AmbaPrint_PrintUInt5("SMC32_AMBA_SIP_OTP_GET_PUKEY ret 0x%x", ret, 0U, 0U, 0U, 0U);
                    for (i = 0U; i <= (SHELL_ATF_KEY_SIZE - 4U); i += 4U) {
                        AmbaPrint_PrintUInt5("\t0x%02x 0x%02x 0x%02x 0x%02x", pukey[i], pukey[i+1U], pukey[i+2U], pukey[i+3U], 0U);
                        AmbaPrint_Flush();
                    }
                }
            }
#if defined(OTP_LAYOUT_V2)
        } else if (AmbaUtility_StringCompare(pArgVector[1], "lockeccpubkey", AmbaUtility_StringLength("lockeccpubkey")) == 0) {
            UINT32 ret;
            ret = AmbaSmcCall(SMC32_AMBA_OTP_LOCK_PUKEY, 16, 0, 0, 0);
            AmbaPrint_PrintUInt5("SMC32_AMBA_OTP_LOCK_PUKEY ret 0x%x \n", ret, 0U, 0U, 0U, 0U);
#endif
        } else if (AmbaUtility_StringCompare(pArgVector[1], "permanentlyenablesecureboot", AmbaUtility_StringLength("permanentlyenablesecureboot")) == 0) {
            /* Permanently Enable Secure Boot */
            UINT32 ret;
            ret = AmbaSmcCall(SMC32_AMBA_SIP_OTP_PERMANENTLY_ENABLE_SECURE_BOOT, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("SMC32_AMBA_SIP_OTP_PERMANENTLY_ENABLE_SECURE_BOOT ret 0x%x \n", ret, 0U, 0U, 0U, 0U);
        } else {
            SHELL_ATFCmdUsage(ArgCount, pArgVector, PrintFunc);
        }
    }
    return;
}
