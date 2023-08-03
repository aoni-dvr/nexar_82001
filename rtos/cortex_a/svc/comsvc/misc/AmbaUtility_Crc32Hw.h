/**
 *  @file AmbaUtility_Crc32Hw.h
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
 *  @details Header file for Ambarella Utility CRC32 functions.
 *
 */

#ifndef AMBA_UTILITY_CRC32_HW_H
#define AMBA_UTILITY_CRC32_HW_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

void CRC32X(UINT32 *crc, UINT64 value);
void CRC32W(UINT32 *crc, UINT32 value);
void CRC32H(UINT32 *crc, UINT16 value);
void CRC32B(UINT32 *crc, UINT8 value);

/*
#define CRC32W(crc, value) __asm__("crc32w %0, %0, %1":"+r"(crc):"r"(value))
#define CRC32H(crc, value) __asm__("crc32h %0, %0, %1":"+r"(crc):"r"(value))
#define CRC32B(crc, value) __asm__("crc32b %0, %0, %1;":"+r"(crc):"r"(value))
*/


UINT32 AmbaUtility_Crc32Hw(const UINT8 *pBuffer, UINT32 Size);
UINT32 AmbaUtility_Crc32AddHw(const UINT8 *pBuffer, UINT32 Size, UINT32 Crc);

#endif
