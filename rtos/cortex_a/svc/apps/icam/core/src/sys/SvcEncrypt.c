/**
 *  @file SvcEncrypt.c
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
 *  @details svc data encryption/decryption
 *
 */

#include "AmbaKAL.h"
#include "AmbaUtility.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#include "SvcEncrypt.h"

#ifdef CONFIG_BUILD_FOSS_MBEDTLS
#include "libc/time.h"
#include "mbedtls/aes.h"
#include "mbedtls/md.h"
#include "mbedtls/platform_util.h"
#endif

#define SVC_LOG_ENCRYPT "ENCRYPT"
#define ENCRYPT_TIMEOUT (5000U)

typedef struct {
    UINT32              EncryptType;
    ULONG               BufBase;
    ULONG               BufSize;
    ULONG               WAddr;    /* write address */
    AMBA_KAL_MUTEX_t    Mutex;
} SVC_ENCRYPT_CTRL_s;

typedef struct {
    ULONG    BufBase;
    ULONG    BufSize;
    ULONG    DataAddr;
    ULONG    DataSize;
} SVC_ENCRYPT_COPY_s;

static SVC_ENCRYPT_CTRL_s EncCtrl;
static UINT32             IsInit = 0U;

static UINT32 DataCopy(const SVC_ENCRYPT_COPY_s *pSrc, const SVC_ENCRYPT_COPY_s *pDst)
{
    UINT32       Rval = SVC_OK, Err;
    ULONG        SrcRSize, SrcCAddr, SrcCSize;
    ULONG        DstCAddr, DstCSize;
    const void   *pSrcAddr;
    void         *pDstAddr;

    AmbaMisra_TouchUnused(&Rval);

    SrcCAddr = pSrc->DataAddr;
    SrcRSize = pSrc->DataSize;
    if ((pSrc->DataAddr + pSrc->DataSize) > (pSrc->BufBase + pSrc->BufSize)) {
        SrcCSize = (pSrc->BufBase + pSrc->BufSize) - pSrc->DataAddr;
    } else {
        SrcCSize = pSrc->DataSize;
    }

    DstCAddr = pDst->DataAddr;
    if ((pDst->DataAddr + SrcCSize) > (pDst->BufBase + pDst->BufSize)) {
        DstCSize = (pDst->BufBase + pDst->BufSize) - pDst->DataAddr;
    } else {
        DstCSize = SrcCSize;
    }

    while (SrcRSize > 0U) {
        if (SrcCSize == DstCSize) {
            AmbaMisra_TypeCast(&pSrcAddr, &SrcCAddr);
            AmbaMisra_TypeCast(&pDstAddr, &DstCAddr);
            Err = AmbaWrap_memcpy(pDstAddr, pSrcAddr, (SIZE_t)DstCSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_ENCRYPT, "AmbaWrap_memcpy failed %u", Err, 0U);
                Rval = SVC_NG;
            }

            SrcCAddr += DstCSize;
            if (SrcCAddr >= (pSrc->BufBase + pSrc->BufSize)) {
                SrcCAddr -= pSrc->BufSize;
            }
            DstCAddr += DstCSize;
            if (DstCAddr >= (pDst->BufBase + pDst->BufSize)) {
                DstCAddr -= pDst->BufSize;
            }
            SrcRSize -= DstCSize;
            SrcCSize  = pSrc->DataSize - DstCSize;
            if ((DstCAddr + SrcCSize) > (pDst->BufBase + pDst->BufSize)) {
                DstCSize = (pDst->BufBase + pDst->BufSize) - DstCAddr;
            } else {
                DstCSize = SrcCSize;
            }

        } else {
            AmbaMisra_TypeCast(&pSrcAddr, &SrcCAddr);
            AmbaMisra_TypeCast(&pDstAddr, &DstCAddr);
            Err = AmbaWrap_memcpy(pDstAddr, pSrcAddr, (SIZE_t)DstCSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_ENCRYPT, "AmbaWrap_memcpy failed %u", Err, 0U);
                Rval = SVC_NG;
            }

            SrcCAddr += DstCSize;
            DstCAddr  = pDst->BufBase;
            DstCSize  = SrcCSize - DstCSize;

            AmbaMisra_TypeCast(&pSrcAddr, &SrcCAddr);
            AmbaMisra_TypeCast(&pDstAddr, &DstCAddr);
            Err = AmbaWrap_memcpy(pDstAddr, pSrcAddr, (SIZE_t)DstCSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_ENCRYPT, "AmbaWrap_memcpy failed %u", Err, 0U);
                Rval = SVC_NG;
            }

            SrcCAddr += DstCSize;
            if (SrcCAddr >= (pSrc->BufBase + pSrc->BufSize)) {
                SrcCAddr -= pSrc->BufSize;
            }
            DstCAddr += DstCSize;
            if (DstCAddr >=( pDst->BufBase + pDst->BufSize)) {
                DstCAddr -= pDst->BufSize;
            }
            SrcRSize -= SrcCSize;
            SrcCSize  = pSrc->DataSize - SrcCSize;
            if ((DstCAddr + SrcCSize) > (pDst->BufBase + pDst->BufSize)) {
                DstCSize = (pDst->BufBase + pDst->BufSize) - DstCAddr;
            } else {
                DstCSize = SrcCSize;
            }
        }
    }

    return Rval;
}

static UINT32 AES_Encrypt(SVC_ENCRYPT_PROCESS_s *pProcess)
{
#define AES_SIZE      (16U)   /* 128 bits */
#define AES_KEY_SIZE  (32U)   /* 256 bitss */
    UINT32              Rval = SVC_OK, Err, MtxTake = 0U;
    static UINT8        AesKey[AES_KEY_SIZE] =
    {
        0x34, 0x45, 0x34, 0x43, 0x46, 0x39, 0x36, 0x36, 0x45, 0x35, 0x38, 0x30, 0x38, 0x45, 0x30, 0x46,
        0x34, 0x45, 0x39, 0x35, 0x41, 0x34, 0x32, 0x31, 0x39, 0x46, 0x34, 0x38, 0x35, 0x36, 0x39, 0x34,
    };

    AmbaMisra_TouchUnused(&Rval);

    if (pProcess != NULL) {
        /* update WAddr */
        if ((Rval == SVC_OK) && (pProcess->OutputAddr == 0U)) {
            Err = AmbaKAL_MutexTake(&(EncCtrl.Mutex), ENCRYPT_TIMEOUT);
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(SVC_LOG_ENCRYPT, "AmbaKAL_MutexTake failed (%u)", Err, 0U);
                Rval = SVC_NG;
            } else {
                MtxTake = 1U;
            }

            pProcess->OutputAddr    = EncCtrl.WAddr;
            pProcess->OutputSize    = pProcess->InputTotalSize;
            pProcess->OutputBufBase = EncCtrl.BufBase;
            pProcess->OutputBufSize = EncCtrl.BufSize;
            EncCtrl.WAddr += pProcess->OutputSize;
            if (EncCtrl.WAddr >= (EncCtrl.BufBase + EncCtrl.BufSize)) {
                EncCtrl.WAddr -= EncCtrl.BufSize;
            }

            /* bufsize should always >= OutputSize */
            if (pProcess->OutputSize > EncCtrl.BufSize) {
                SvcLog_NG(SVC_LOG_ENCRYPT, "BufSize is not enough %u/%u", pProcess->OutputSize, EncCtrl.BufSize);
                Rval = SVC_NG;
            }

            if (MtxTake == 1U) {
                Err = AmbaKAL_MutexGive(&(EncCtrl.Mutex));
                if (Err != KAL_ERR_NONE) {
                    SvcLog_NG(SVC_LOG_ENCRYPT, "AmbaKAL_MutexGive failed (%u)", Err, 0U);
                }
            }
        } else {
            pProcess->OutputSize = pProcess->InputTotalSize;
        }

        if (Rval == SVC_OK) {
            ULONG                OffSet, RSize, Addr;
            UINT8                Input[AES_SIZE], Output[AES_SIZE];
            UINT8                InputOverflow, OutputOverflow;
            const UINT8          *pInput;
            UINT8                *pOutput;
            INT32                MbetlsErr;
            SVC_ENCRYPT_COPY_s   Src, Dst;
            mbedtls_aes_context  AesCtx;

            /* init aes */
            mbedtls_aes_init(&AesCtx);

            /* set aes key */
            if (pProcess->ProcType == SVC_ENCRYPT_PROC_ENC) {
                MbetlsErr = mbedtls_aes_setkey_enc(&AesCtx, AesKey, AES_KEY_SIZE * 8U);
                if (MbetlsErr != 0) {
                    SvcLog_NG(SVC_LOG_ENCRYPT, "mbedtls_aes_setkey_enc failed (%u)", (UINT32)MbetlsErr, 0U);
                }
            } else {
                MbetlsErr = mbedtls_aes_setkey_dec(&AesCtx, AesKey, AES_KEY_SIZE * 8U);
                if (MbetlsErr != 0) {
                    SvcLog_NG(SVC_LOG_ENCRYPT, "mbedtls_aes_setkey_dec failed (%u)", (UINT32)MbetlsErr, 0U);
                }
            }

            OffSet = pProcess->InputOffset;
            RSize  = pProcess->InputProcSize;

            /* copy the offset data */
            if (OffSet > 0U) {
                Src.BufBase  = pProcess->InputBufBase;
                Src.BufSize  = pProcess->InputBufSize;
                Src.DataAddr = pProcess->InputAddr;
                Src.DataSize = OffSet;

                Dst.BufBase  = pProcess->OutputBufBase;
                Dst.BufSize  = pProcess->OutputBufSize;
                Dst.DataAddr = pProcess->OutputAddr;

                if (Src.DataAddr != Dst.DataAddr) {
                    Err = DataCopy(&Src, &Dst);
                    if (Err != KAL_ERR_NONE) {
                        SvcLog_NG(SVC_LOG_ENCRYPT, "DataCopy failed (%u)", Err, 0U);
                        Rval = SVC_NG;
                    }
                }
            }

            while (RSize >= AES_SIZE) {
                InputOverflow  = 0U;
                OutputOverflow = 0U;

                if ((pProcess->InputAddr + OffSet) < (pProcess->InputBufBase + pProcess->InputBufSize)) {
                    if (((pProcess->InputAddr + OffSet) + AES_SIZE) > (pProcess->InputBufBase + pProcess->InputBufSize)) {
                        InputOverflow = 1U;
                    }
                }

                if ((pProcess->OutputAddr + OffSet) < (pProcess->OutputBufBase + pProcess->OutputBufSize)) {
                    if (((pProcess->OutputAddr + OffSet) + AES_SIZE) > (pProcess->OutputBufBase + pProcess->OutputBufSize)) {
                        OutputOverflow = 1U;
                    }
                }

                /* copy data from source buffer to aes temp buffer */
                if (InputOverflow > 0U) {
                    Src.BufBase  = pProcess->InputBufBase;
                    Src.BufSize  = pProcess->InputBufSize;
                    Src.DataAddr = pProcess->InputAddr + OffSet;
                    if (Src.DataAddr >= (pProcess->InputBufBase + pProcess->InputBufSize)) {
                        Src.DataAddr -= pProcess->InputBufSize;
                    }
                    Src.DataSize = AES_SIZE;

                    pInput = Input;
                    AmbaMisra_TypeCast(&(Dst.BufBase), &pInput);
                    Dst.BufSize  = AES_SIZE;
                    Dst.DataAddr = Dst.BufBase;

                    Err = DataCopy(&Src, &Dst);
                    if (Err != KAL_ERR_NONE) {
                        SvcLog_NG(SVC_LOG_ENCRYPT, "DataCopy failed (%u)", Err, 0U);
                        AmbaPrint_Flush();
                        Rval = SVC_NG;
                    }
                } else {
                    Addr = pProcess->InputAddr + OffSet;
                    if (Addr >= (pProcess->InputBufBase + pProcess->InputBufSize)) {
                        Addr -= pProcess->InputBufSize;
                    }
                    AmbaMisra_TypeCast(&pInput, &Addr);
                }

                if (OutputOverflow > 0U) {
                    pOutput = Output;
                } else {
                    Addr = pProcess->OutputAddr + OffSet;
                    if (Addr >= (pProcess->OutputBufBase + pProcess->OutputBufSize)) {
                        Addr -= pProcess->OutputBufSize;
                    }
                    AmbaMisra_TypeCast(&pOutput, &Addr);

                }

                /* encryption 128 bits */
                if (pProcess->ProcType == SVC_ENCRYPT_PROC_ENC) {
                    MbetlsErr = mbedtls_aes_crypt_ecb(&AesCtx, MBEDTLS_AES_ENCRYPT, pInput, pOutput);
                    if (MbetlsErr != 0) {
                        SvcLog_NG(SVC_LOG_ENCRYPT, "mbedtls_aes_crypt_ecb failed (%u)", (UINT32)MbetlsErr, 0U);
                    }
                } else {
                    MbetlsErr = mbedtls_aes_crypt_ecb(&AesCtx, MBEDTLS_AES_DECRYPT, pInput, pOutput);
                    if (MbetlsErr != 0) {
                        SvcLog_NG(SVC_LOG_ENCRYPT, "mbedtls_aes_crypt_ecb failed (%u)", (UINT32)MbetlsErr, 0U);
                    }
                }

                /* copy data from aes temp buffer to output buffer */
                if (OutputOverflow > 0U) {
                    AmbaMisra_TypeCast(&(Src.BufBase), &pOutput);
                    Src.BufSize  = AES_SIZE;
                    Src.DataAddr = Src.BufBase;
                    Src.DataSize = AES_SIZE;

                    Dst.BufBase  = pProcess->OutputBufBase;
                    Dst.BufSize  = pProcess->OutputBufSize;
                    Dst.DataAddr = pProcess->OutputAddr + OffSet;
                    if (Dst.DataAddr >= (pProcess->OutputBufBase + pProcess->OutputBufSize)) {
                        Dst.DataAddr -= pProcess->OutputBufSize;
                    }

                    Err = DataCopy(&Src, &Dst);
                    if (Err != KAL_ERR_NONE) {
                        SvcLog_NG(SVC_LOG_ENCRYPT, "DataCopy failed (%u)", Err, 0U);
                        AmbaPrint_Flush();
                        Rval = SVC_NG;
                    }
                }

                /* update counters */
                OffSet += AES_SIZE;
                RSize  -= AES_SIZE;
            }

            /* copy the rest data */
            if ((pProcess->InputTotalSize - OffSet) > 0U) {
                Src.BufBase  = pProcess->InputBufBase;
                Src.BufSize  = pProcess->InputBufSize;
                Src.DataAddr = pProcess->InputAddr + OffSet;
                if (Src.DataAddr >= (pProcess->InputBufBase + pProcess->InputBufSize)) {
                    Src.DataAddr -= pProcess->InputBufSize;
                }
                Src.DataSize = pProcess->InputTotalSize - OffSet;

                Dst.BufBase  = pProcess->OutputBufBase;
                Dst.BufSize  = pProcess->OutputBufSize;
                Dst.DataAddr = pProcess->OutputAddr + OffSet;
                if (Dst.DataAddr >= (pProcess->OutputBufBase + pProcess->OutputBufSize)) {
                    Dst.DataAddr -= pProcess->OutputBufSize;
                }

                if (Src.DataAddr != Dst.DataAddr) {
                    Err = DataCopy(&Src, &Dst);
                    if (Err != KAL_ERR_NONE) {
                        SvcLog_NG(SVC_LOG_ENCRYPT, "DataCopy failed (%u)", Err, 0U);
                        Rval = SVC_NG;
                    }
                }
            }

            mbedtls_aes_free(&AesCtx);
        }
    }

    return Rval;
}

UINT32  SvcEncrypt_Init(const SVC_ENCRYPT_INIT_s *pInit)
{
    UINT32 Rval = SVC_OK, Err;

    AmbaMisra_TouchUnused(&Rval);

    if (IsInit == 0U) {
        /* parameters check */
        if (Rval == SVC_OK) {
            if (pInit != NULL) {
                if (pInit->EncryptType >= SVC_ENCRYPT_TYPE_NUM) {
                    SvcLog_NG(SVC_LOG_ENCRYPT, "invalid EncryptType %u", pInit->EncryptType, 0U);
                    Rval = SVC_NG;
                }
                if (pInit->BufBase == 0U) {
                    SvcLog_NG(SVC_LOG_ENCRYPT, "BufBase is 0", 0U, 0U);
                    Rval = SVC_NG;
                }
                if (pInit->BufSize == 0U) {
                    SvcLog_NG(SVC_LOG_ENCRYPT, "BufSize is 0", 0U, 0U);
                    Rval = SVC_NG;
                }
            } else {
                SvcLog_NG(SVC_LOG_ENCRYPT, "pInit is null", 0U, 0U);
                Rval = SVC_NG;
            }
        }

        if ((Rval == SVC_OK) && (pInit != NULL)) {
            Err = AmbaWrap_memset(&EncCtrl, 0, sizeof(EncCtrl));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_ENCRYPT, "AmbaWrap_memset failed %u", Err, 0U);
            }

            EncCtrl.EncryptType = pInit->EncryptType;
            EncCtrl.BufBase     = pInit->BufBase;
            EncCtrl.BufSize     = pInit->BufSize;
            EncCtrl.WAddr       = pInit->BufBase;
        }

        /* create mutex */
        if (Rval == SVC_OK) {
            static char MtxName[] = "SvcEncryptMutex";

            Err = AmbaKAL_MutexCreate(&(EncCtrl.Mutex), MtxName);
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(SVC_LOG_ENCRYPT, "AmbaKAL_MutexCreate failed (%u)", Err, 0U);
                Rval = SVC_NG;
            }
        }

        if (Rval == SVC_OK) {
            IsInit = 1U;
        }
    }

    return Rval;
}

UINT32  SvcEncrypt_DeInit(void)
{
    UINT32 Rval = SVC_OK, Err;

    AmbaMisra_TouchUnused(&Rval);

    if (IsInit == 1U) {
        if (Rval == SVC_OK) {
            Err = AmbaKAL_MutexDelete(&(EncCtrl.Mutex));
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(SVC_LOG_ENCRYPT, "AmbaKAL_MutexDelete failed (%u)", Err, 0U);
                Rval = SVC_NG;
            }
        }

        if (Rval == SVC_OK) {
            IsInit = 0U;
        }
    }

    return Rval;
}

UINT32  SvcEncrypt_Process(SVC_ENCRYPT_PROCESS_s *pProcess)
{
    UINT32 Rval = SVC_OK;

    AmbaMisra_TouchUnused(&Rval);

    /* parameters check */
    if (Rval == SVC_OK) {
        if (pProcess != NULL) {
            if (pProcess->ProcType > SVC_ENCRYPT_PROC_DEC) {
                SvcLog_NG(SVC_LOG_ENCRYPT, "invalid ProcType %u", pProcess->ProcType, 0U);
                Rval = SVC_NG;
            }
            if (pProcess->InputAddr == 0U) {
                SvcLog_NG(SVC_LOG_ENCRYPT, "InputAddr is 0", 0U, 0U);
                Rval = SVC_NG;
            }
            if (pProcess->InputTotalSize == 0U) {
                SvcLog_NG(SVC_LOG_ENCRYPT, "InputTotalSize is 0", 0U, 0U);
                Rval = SVC_NG;
            }
            if (pProcess->InputProcSize == 0U) {
                SvcLog_NG(SVC_LOG_ENCRYPT, "InputProcSize is 0", 0U, 0U);
                Rval = SVC_NG;
            }
            if ((pProcess->InputOffset + pProcess->InputProcSize) > pProcess->InputTotalSize) {
                SvcLog_NG(SVC_LOG_ENCRYPT, "InputOffset + InputProcSize %u > InputTotalSize %u", (UINT32)pProcess->InputOffset + (UINT32)pProcess->InputProcSize, (UINT32)pProcess->InputTotalSize);
                Rval = SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_ENCRYPT, "pProcess is null", 0U, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        switch (EncCtrl.EncryptType) {
            case SVC_ENCRYPT_TYPE_AES:
                Rval = AES_Encrypt(pProcess);
                break;
            default:
                SvcLog_NG(SVC_LOG_ENCRYPT, "Unknown EncryptType %u", EncCtrl.EncryptType, 0U);
                Rval = SVC_NG;
                break;
        }
    }

    return Rval;
}

