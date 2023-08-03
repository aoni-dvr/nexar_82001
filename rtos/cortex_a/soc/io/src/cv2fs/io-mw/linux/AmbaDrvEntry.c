/**
 *  @file AmbaDrvEntry.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Ambarella Driver Entry Functions
 *
 */

#include "AmbaDef.h"
#include "AmbaDrvEntry.h"

/**
 *  AmbaDrvEntry - Initializing certain objects the driver uses, and setting up various per-driver system resources.
 */
void AmbaDrvEntry(void)
{
    static AMBA_DRV_ENTRY_f DrvEntryList[NUM_DRV_ENTRY] = {
        [0] = AmbaVIN_DrvEntry,
        [1] = AmbaVout_DrvEntry,
        [2] = AmbaDMA_DrvEntry,
        [3] = AmbaGDMA_DrvEntry,
        [4] = AmbaGPIO_DrvEntry,
        [5] = AmbaUART_DrvEntry,
        [6] = AmbaI2C_DrvEntry,
        [7] = AmbaI2S_DrvEntry,
        [8] = AmbaINT_DrvEntry,
        [9] = AmbaWDT_DrvEntry,
        [10] = AmbaPWM_DrvEntry,
        [11] = AmbaRTC_DrvEntry,
        [12] = AmbaSD_DrvEntry,
        [13] = AmbaSPI_DrvEntry,
        [14] = AmbaSYS_DrvEntry,
        [15] = AmbaTMR_DrvEntry,
        [16] = AmbaCEHU_DrvEntry,
        [17] = AmbaTempSensor_DrvEntry,
    };

    UINT32 i, RetVal = OK;

    for (i = 0U; i < NUM_DRV_ENTRY; i ++) {
        if (DrvEntryList[i] != NULL) {
            RetVal = DrvEntryList[i]();
            if (RetVal != OK) {
                AmbaAssert();
            }
        }
    }
}
