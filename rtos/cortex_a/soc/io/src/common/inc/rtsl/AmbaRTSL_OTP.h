/**
 *  @file AmbaRTSL_OTP.h
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details Definitions & Constants for OTP (One-Time Programming) RTSL APIs
 *
 */

#ifndef AMBA_RTSL_OTP_H
#define AMBA_RTSL_OTP_H

#include <AmbaCSL_OTP.h>

#define RTSL_ERR_OTP_SUCCESS       0U
#define RTSL_ERR_OTP_WAIT_TIMEOUT  1U //!< Timeout when waiting for OTP controller.
#define RTSL_ERR_OTP_PROGRAMMED    2U //!< The OTP bit is already programmed
#define RTSL_ERR_OTP_PROGRAM_FAIL  3U //!< OTP bit program failed
#define RTSL_ERR_OTP_PARAM         4U //!< Wrong input parameters
#define RTSL_ERR_OTP_REACH_MAX     5U //!< monotonic counter reaches maximum

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define RTSL_ERR_OTP_NO_ALIGN      6U //!< OTP address is not 64-bit aligned
#define RTSL_ERR_OTP_CRC           7U //!< CRC error when reading OTP data
#define RTSL_ERR_OTP_WL_CONFIRM    8U //!< Write Lock and Write Lock Confirm don't match
#define RTSL_ERR_OTP_DI_CONFIRM    9U //!< Data Invalid and Data Invalid Confirm don't match
#define RTSL_ERR_OTP_MC_CONFIRM   10U //!< Monotonic Counter and Monotonic Counter Confirm don't match
#endif

/* OTP_CTRL1_REG bit define */
#define READ_FSM_ENABLE         ((UINT32)(1UL << 22UL))
#define READ_ENABLE             ((UINT32)(1UL << 21UL))
#define DBG_READ_MODE           ((UINT32)(1UL << 20UL))
#define FSM_WRITE_MODE          ((UINT32)(1UL << 18UL))
#define PROG_ENABLE             ((UINT32)(1UL << 17UL))
#define PROG_FSM_ENABLE         ((UINT32)(1UL << 16UL))
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV52)
#define CTRL1_ADDR_MASK         ((UINT32)0x0FFFFU)
#else
#define CTRL1_ADDR_MASK         ((UINT32)0x07FFFU)
#endif

/* OTP_OBSV_REG bit define */
#define WRITE_PROG_DONE         ((UINT32)(1UL << 4UL))
#define WRITE_PROG_FAIL         ((UINT32)(1UL << 3UL))
#define WRITE_PROG_RDY          ((UINT32)(1UL << 2UL))
#define READ_OBSV_RDY           ((UINT32)(1UL << 1UL))
#define READ_OBSV_DONE          ((UINT32)(1UL/* << 0UL*/))


/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_OTP.c
\*-----------------------------------------------------------------------------------------------*/

UINT32 AmbaRTSL_OtpPublicKeyWrite(const UINT8 *KeyBuffer, UINT32 BufferSize, UINT32 KeyIndex);
UINT32 AmbaRTSL_OtpPublicKeyRead(UINT8 *KeyBuffer, UINT32 BufferSize, UINT32 KeyIndex, UINT32 *KeyStatus);
UINT32 AmbaRTSL_OtpPublicKeyRevoke(UINT32 KeyIndex);
UINT32 AmbaRTSL_OtpAmbaUniqueIDRead(UINT8 *Buffer, UINT32 BufferSize);
UINT32 AmbaRTSL_OtpCustomerUniIDWrite(const UINT8 *Buffer, UINT32 BufferSize);
UINT32 AmbaRTSL_OtpCustomerUniIDRead(UINT8 *Buffer, UINT32 BufferSize);
UINT32 AmbaRTSL_OtpMonoCounterRead(UINT32 *Counter);
UINT32 AmbaRTSL_OtpMonoCounterIncrease(void);
UINT32 AmbaRTSL_OtpSecureBootEnable(void);
UINT32 AmbaRTSL_OtpAesKeyWrite(const UINT8 *KeyBuffer, UINT32 BufferSize, UINT32 KeyIndex);
UINT32 AmbaRTSL_OtpAesKeyRead(UINT8 *KeyBuffer, UINT32 BufferSize, UINT32 KeyIndex);
UINT32 AmbaRTSL_OtpReservedDXRead(UINT8 *Buffer, UINT32 BufferSize, UINT32 Index);
UINT32 AmbaRTSL_OtpReservedDXWrite(const UINT8 *Buffer, UINT32 BufferSize, UINT32 Index);
UINT32 AmbaRTSL_OtpReservedB2Read(UINT8 *Buffer, UINT32 BufferSize);
UINT32 AmbaRTSL_OtpReservedB2Write(const UINT8 *Buffer, UINT32 BufferSize);
UINT32 AmbaRTSL_OtpJtagEfuseSet(void);
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
UINT32 AmbaRTSL_OtpTempSensorRead(UINT32 Index, UINT32 *Data_tsl, UINT32 *Data_tsm, UINT32 *Data_tsh);
#endif

UINT32 AmbaRTSL_OtpWaferIDRead(UINT8 *WaferID, UINT32 BufferSize);
UINT32 AmbaRTSL_OtpBitRead(UINT32 BitAddress, UINT32 *BitValue);

#endif /* AMBA_RTSL_OTP_H */
