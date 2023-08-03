/**
 *  @file AmbaCSL_OTP.h
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
 *  @details Definitions & Constants for OTP (One-Time Programming) control APIs
 *
 */

#ifndef AMBA_CSL_OTP_H
#define AMBA_CSL_OTP_H

#include "AmbaReg_ScratchpadS.h"

/*-----------------------------------------------------------------------------------------------*\
 * Macro Definitions
\*-----------------------------------------------------------------------------------------------*/

static inline UINT32 AmbaCSL_OtpCtrl1Read(void) { return pAmbaScratchpadS_Reg->OtpCtrl1; }
static inline UINT32 AmbaCSL_OtpObsvRead(void) { return pAmbaScratchpadS_Reg->OtpObsv; }
static inline UINT32 AmbaCSL_OtpDataRead(void) { return pAmbaScratchpadS_Reg->OtpReadData0; }
static inline UINT32 AmbaCSL_OtpCrc8Read(void) { return pAmbaScratchpadS_Reg->OtpReadData1; }

static inline void AmbaCSL_OtpCtrl1Write(UINT32 Value) { pAmbaScratchpadS_Reg->OtpCtrl1 = Value; }

#endif /* AMBA_CSL_SPI_H */
