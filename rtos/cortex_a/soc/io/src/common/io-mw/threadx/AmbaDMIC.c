/**
 *  @file AmbaDMIC.c
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
 *  @details DMIC driver APIs
 *
 */

#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaDMIC.h"
#include "AmbaRTSL_DMIC.h"

static AMBA_KAL_MUTEX_t AmbaDmicMutex;

/**
 *  AmbaDMIC_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 */
UINT32 AmbaDMIC_DrvEntry(void)
{
    static char AmbaDmicMutexName[16] = "AmbaDmicMutex";
    UINT32 RetVal = DMIC_ERR_NONE;

    /* Create Mutex */
    if (AmbaKAL_MutexCreate(&AmbaDmicMutex, AmbaDmicMutexName) != KAL_ERR_NONE) {
        RetVal = DMIC_ERR_MUTEX;
    }

    return RetVal;
}

/**
 *  AmbaDMIC_Enable - Enable dmic device driver
 *  @return error code
 */
UINT32 AmbaDMIC_Enable(void)
{
    UINT32 RetVal = DMIC_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaDmicMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = DMIC_ERR_MUTEX;
    } else {
        /* Enable DMIC */
        RetVal = AmbaRTSL_DmicEnable(NULL);

        if (AmbaKAL_MutexGive(&AmbaDmicMutex) != KAL_ERR_NONE) {
            RetVal = DMIC_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaDMIC_Disable - Disable dmic device driver
 *  @return error code
 */
UINT32 AmbaDMIC_Disable(void)
{
    UINT32 RetVal = DMIC_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaDmicMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = DMIC_ERR_MUTEX;
    } else {
        /* Disable DMIC*/
        RetVal = AmbaRTSL_DmicDisable();

        if (AmbaKAL_MutexGive(&AmbaDmicMutex) != KAL_ERR_NONE) {
            RetVal = DMIC_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaDMIC_DcBlocking - Remove the dmic dc offset
 *  @param[in] Level index to select a pre-defined iir filtering
 *  @return error code
 */
UINT32 AmbaDMIC_DcBlocking(UINT32 Level)
{
    static const UINT32 DcBlockTable[6][17] = {
        [0] = {0x40000000U, 0x00000000U, 0x00000000U, 0xC0000000U, 0x00000000U, 0xC0107000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U},
        [1] = {0x40000000U, 0x00000000U, 0x00000000U, 0xC0000000U, 0x00000000U, 0xC051F000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U},
        [2] = {0x40000000U, 0x00000000U, 0x00000000U, 0xC0000000U, 0x00000000U, 0xC0A3E000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U},
        [3] = {0x40000000U, 0x00000000U, 0x00000000U, 0xC0000000U, 0x00000000U, 0xC147B000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U},
        [4] = {0x40000000U, 0x00000000U, 0x00000000U, 0xC0000000U, 0x00000000U, 0xC1EB9000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U},
        [5] = {0x40000000U, 0x00000000U, 0x00000000U, 0xC0000000U, 0x00000000U, 0xC3334000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U}
    };
    UINT32 RetVal = DMIC_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaDmicMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = DMIC_ERR_MUTEX;
    } else {
        /* Remove DC offset process */
        if (Level <= 5U) {
            RetVal = AmbaRTSL_DmicEnable(DcBlockTable[Level]);
        } else {
            RetVal = DMIC_ERR_ARG;
        }

        if (AmbaKAL_MutexGive(&AmbaDmicMutex) != KAL_ERR_NONE) {
            RetVal = DMIC_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaDMIC_ApplyCustomFilter - Apply custome filter setting into Custom_IIR_Coefficient
 *  @param[in] pCoeff coefficients for user-defined iir filtering
 *  @return error code
 */
UINT32 AmbaDMIC_ApplyCustomFilter(const UINT32 *pCoeff)
{
    UINT32 RetVal = DMIC_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaDmicMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = DMIC_ERR_MUTEX;
    } else {
        if (pCoeff != NULL) {
            RetVal = AmbaRTSL_DmicEnable(pCoeff);
        } else {
            RetVal = DMIC_ERR_ARG;
        }

        if (AmbaKAL_MutexGive(&AmbaDmicMutex) != KAL_ERR_NONE) {
            RetVal = DMIC_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

