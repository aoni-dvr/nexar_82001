/*
 * Copyright (c) 2022 Ambarella International LP
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
 */

#include "AmbaDef.h"
#include "AmbaDrvEntry.h"

/**
 *  AmbaDrvEntry - Initializing certain objects the driver uses, and setting up various per-driver system resources.
 */
void AmbaDrvEntry(void)
{
    static const AMBA_DRV_ENTRY_f DrvEntryList[] = {
        [0] = AmbaSYS_DrvEntry,
        [1] = AmbaTMR_DrvEntry,
        [2] = AmbaRTC_DrvEntry,
        [3] = AmbaI2C_DrvEntry,
        [4] = AmbaSPI_DrvEntry,
        [5] = AmbaGPIO_DrvEntry,
        [6] = AmbaPWM_DrvEntry,
        [7] = AmbaDMA_DrvEntry,
        [8] = AmbaGDMA_DrvEntry,
        [9] = AmbaVIN_DrvEntry,
        [10] = AmbaVout_DrvEntry,
    };

    SIZE_t i, entries;
    UINT32 RetVal = OK;

    entries = sizeof(DrvEntryList) / sizeof(DrvEntryList[0]);

    for (i = 0U; i < entries; i ++) {
        if (DrvEntryList[i] != NULL) {
            RetVal = DrvEntryList[i]();
            if (RetVal != OK) {
                AmbaAssert();
            }
        }
    }
}
