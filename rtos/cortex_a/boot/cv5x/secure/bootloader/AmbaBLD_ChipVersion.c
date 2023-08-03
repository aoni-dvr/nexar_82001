/**
 *  @file AmbaBLD_ChipVersion.c
 *
 *  Copyright (c) 2022 Ambarella International LP
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
 *  @details check chip version information
 *
 */

#include "AmbaTypes.h"
#include "AmbaIOUtility.h"
#include "AmbaBLD.h"
#include <AmbaRTSL_OTP.h>
#include <AmbaRTSL_CV5XChipVer.h>

void BLD_ChipVersionCheck(void)
{
    UINT8 wafer_id[6] = {0, 0, 0, 0, 0, 0};
    UINT8 flag_engineer_version = 1;
    UINT32 flag_version = CV5X_CHIP_VER_UNKNOWN;
    UINT32 bit_91;
    UINT32 bit_89;
    UINT32 i;
    UINT32 uret;

    uret = AmbaRTSL_OtpWaferIDRead(wafer_id, 6);
    if (uret == 0U) {
        uret = AmbaRTSL_OtpBitRead(91, &bit_91);
    }
    if (uret == 0U) {
        uret = AmbaRTSL_OtpBitRead(89, &bit_89);
    }

    if (uret == 0U) {
        for (i = 0; i < 6U; i++) {
            if (wafer_id[i] != 0U) {
                // if Wafer ID contains non-zero value, it is not enginner sample
                flag_engineer_version = 0;
                break;
            }
        }

        if (flag_engineer_version == 0U) {
            // check version of non-engineer-sample
            // according to OPS, A0 = [0,1], A1 = [1, 0], ENGNIEER = [0,0]
            if ((bit_91 == 0U) && (bit_89 == 1U)) {
                flag_version = CV5X_CHIP_VER_A0;
            } else if ((bit_91 == 1U) && (bit_89 == 0U)) {
                flag_version = CV5X_CHIP_VER_A1;
            } else if ((bit_91 == 0U) && (bit_89 == 0U)) {
                flag_version = CV5X_CHIP_VER_ENGINEER;
            } else {
                flag_version = CV5X_CHIP_VER_AX;
            }
        } else {
            flag_version = CV5X_CHIP_VER_ENGINEER;
        }
    }
    // save version information into Scratchpad SRAM (0x20E003.1000)
    IO_UtilityRegWrite32(0x20E0031000, flag_version);
}
