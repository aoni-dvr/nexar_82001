/*
 * Copyright (c) 2020 Ambarella International LP
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

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDRAMC.h"
#include "AmbaRTSL_DRAMC.h"

static AMBA_KAL_MUTEX_t AmbaDramcMutex;

/**
 *  AmbaDRAMC_DrvEntry - DRAMC driver initializations
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaDRAMC_DrvEntry(void)
{
    UINT32 RetVal = DRAMC_ERR_NONE;

    /* Create Mutexes */
    if (AmbaKAL_MutexCreate(&AmbaDramcMutex, NULL) != KAL_ERR_NONE) {
        RetVal = DRAMC_ERR_UNEXPECTED;
    }

    return RetVal;
}

/**
 *  AmbaDRAMC_EnableStatis - Enable Dramc client statistics calculating
 *  @return error code
 */
UINT32 AmbaDRAMC_EnableStatis(void)
{
    UINT32 RetVal = DRAMC_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaDramcMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = DRAMC_ERR_MUTEX;
    } else {
        AmbaRTSL_DramcEnableStatisCtrl();

        if (AmbaKAL_MutexGive(&AmbaDramcMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = DRAMC_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaDRAMC_GetStatisInfo - Get dram clients statistics info
 *  @param[out] StatisData data of dram client statistics
 *  @return error code
 */
UINT32 AmbaDRAMC_GetStatisInfo(AMBA_DRAMC_STATIS_s *StatisData)
{
    UINT32 RetVal = DRAMC_ERR_NONE;

    if (StatisData == NULL) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaDramcMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = DRAMC_ERR_MUTEX;
        } else {
            AmbaRTSL_DramcGetStatisInfo(StatisData);

            if (AmbaKAL_MutexGive(&AmbaDramcMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = DRAMC_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
/**
 * Get dram host number
 *
 * @param [out] pHostNum
 * @return error code
 */
UINT32 AmbaDRAMC_GetHostNum(UINT32 *pHostNum)
{
    UINT32 RetVal = DRAMC_ERR_NONE;

    if (pHostNum == NULL) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        RetVal = AmbaRTSL_DramcGetHostNum(pHostNum);
    }

    return RetVal;
}
#endif
