/**
*  @file SvcEncrypt.h
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

#ifndef SVC_ENCRYPT_H
#define SVC_ENCRYPT_H

typedef struct {
    UINT32   EncryptType;
#define SVC_ENCRYPT_TYPE_AES  0U
#define SVC_ENCRYPT_TYPE_NUM  1U

    ULONG    BufBase;
    ULONG    BufSize;
} SVC_ENCRYPT_INIT_s;

typedef struct {
    /* fill by the users */
    UINT32   ProcType;
#define SVC_ENCRYPT_PROC_ENC  0U    /* encryption */
#define SVC_ENCRYPT_PROC_DEC  1U    /* decryption */

    ULONG    InputBufBase;    /* Input buffer base address */
    ULONG    InputBufSize;    /* Input buffer size */
    ULONG    InputAddr;       /* Input data address */
    ULONG    InputTotalSize;  /* Input data total size */
    ULONG    InputOffset;     /* the offset bytes to bypass encryption */
    ULONG    InputProcSize;   /* actual encryption size */

    /* user can specify the output buffer info, if the value is 0
       encrypt data will store in the internal buffer and upadate the internal
       buffer info to user */
    ULONG    OutputBufBase;   /* Output buffer base address */
    ULONG    OutputBufSize;   /* Output buffer size */
    ULONG    OutputAddr;      /* Output data address */

    /* fill by the module */
    ULONG    OutputSize;      /* Output data size */
} SVC_ENCRYPT_PROCESS_s;

UINT32  SvcEncrypt_Init(const SVC_ENCRYPT_INIT_s *pInit);
UINT32  SvcEncrypt_DeInit(void);
UINT32  SvcEncrypt_Process(SVC_ENCRYPT_PROCESS_s *pProcess);

#endif  /* SVC_ENCRYPT_H */