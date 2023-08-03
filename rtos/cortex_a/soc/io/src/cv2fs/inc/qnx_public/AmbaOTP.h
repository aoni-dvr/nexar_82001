/**
 *  @file AmbaOTP.h
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
 *  @details Definitions & Constants for OTP (One Time Programming) Controller Middleware APIs
 *
 */

#ifndef AMBA_OTP_H
#define AMBA_OTP_H

#ifndef AMBA_OTP_DEF_H
#include "AmbaOTP_Def.h"
#endif

UINT32 AmbaOTP_PublicKeyGet(UINT8 *Buffer, UINT32 BufferSize, UINT32 KeyIndex, UINT32 *KeyStatus);
UINT32 AmbaOTP_PublicKeySet(const UINT8 *Buffer, UINT32 BufferSize, UINT32 KeyIndex);
UINT32 AmbaOTP_PublicKeyRevoke(UINT32 KeyIndex);
UINT32 AmbaOTP_AmbaUniqueIDGet(UINT8 *Buffer, UINT32 BufferSize);
UINT32 AmbaOTP_CustomerUniIDSet(const UINT8 *Buffer, UINT32 BufferSize);
UINT32 AmbaOTP_CustomerUniIDGet(UINT8 *Buffer, UINT32 BufferSize);
UINT32 AmbaOTP_MonoCounterGet(UINT32 *Counter);
UINT32 AmbaOTP_MonoCounterIncrease(void);
UINT32 AmbaOTP_SecureBootEnable(void);
UINT32 AmbaOTP_AesKeySet(const UINT8 *Buffer, UINT32 BufferSize, UINT32 KeyIndex);
UINT32 AmbaOTP_AesKeyGet(UINT8 *Buffer, UINT32 BufferSize, UINT32 KeyIndex);
UINT32 AmbaOTP_ReservedDXGet(UINT8 *Buffer, UINT32 BufferSize, UINT32 Index);
UINT32 AmbaOTP_ReservedDXSet(const UINT8 *Buffer, UINT32 BufferSize, UINT32 Index);
UINT32 AmbaOTP_ReservedB2Get(UINT8 *Buffer, UINT32 BufferSize);
UINT32 AmbaOTP_ReservedB2Set(const UINT8 *Buffer, UINT32 BufferSize);

#endif /* AMBA_OTP_H */

