/**
*  @file iCamTimeProfile.c
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
*  @details icam time profile functions
*
*/

#include "AmbaTypes.h"
#include "AmbaMemMap.h"
#include "AmbaMisraFix.h"
#include "AmbaCSL_RCT.h"

#include "AmbaTimerInfo.h"
#include "iCamTimeProfile.h"

static AMBA_TIMER_INFO_s *g_pTimerInfo;

/**
 *  Initialization of time profile
 *  @return none
 */
void iCamTimeProfile_Init(void)
{
    ULONG  VirtAddr = ((ULONG)AMBA_DRAM_RESERVED_VIRT_ADDR + (ULONG)AMBA_RAM_APPLICATION_SPECIFIC_OFFSET);

    AmbaMisra_TypeCast(&g_pTimerInfo, &VirtAddr);
}

/**
 *  Configuration of time profile
 *  @param[in] ProfileId profile id
 *  @return none
 */
void iCamTimeProfile_Set(UINT32 ProfileId)
{
    if ((ProfileId < STAMP_NUM) && (g_pTimerInfo != NULL)) {
        AmbaCSL_RctTimer1Freeze();
        g_pTimerInfo->A53_APP_Time[ProfileId] = AmbaCSL_RctTimer1GetCounter();
        AmbaCSL_RctTimer1Enable();
    }
}

/**
 *  Get time profile information
 *  @param[in] ppTimeInfo pointer to timer information
 *  @return none
 */
void iCamTimeProfile_Get(void **ppTimeInfo)
{
    *ppTimeInfo = g_pTimerInfo;
}
