/**
 *  @file SvcBuild.c
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
 *  @details Module version
 *
 */

#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "SvcBuild.h"

/**
* version dump of firmware
* @return none
*/
void SvcBuild_ShowInfo(void)
{
    const char  OperStr[16] = {
    #if defined(CONFIG_THREADX64)
        "THX64"
    #elif defined(CONFIG_QNX)
        "QNX"
    #elif defined(CONFIG_LINUX)
        "LNX"
    #else
        "THX32"
    #endif
    };
    char DramStr[32] = {0};     // show 'nGB' (ex: 4G or 8G)
    UINT32 DramLeng = AmbaUtility_UInt32ToStr(DramStr, (UINT32)sizeof(DramStr), (UINT32)((UINT64)(CONFIG_DDR_SIZE) / (UINT64)(0x40000000U)), 10U);

    if ((0U == DramLeng) || (DramStr[0] == '0')) {
        // If dram size smaller than 1GB, show dram size value directly
        DramStr[0] = '0';
        DramStr[1] = 'x';
        DramStr[2] = '\0';
        DramLeng = AmbaUtility_UInt64ToStr(&(DramStr[2]), (UINT32)sizeof(DramStr) - 2U, (UINT64)(CONFIG_DDR_SIZE), 16U);
        if (0U == DramLeng) {
            DramStr[0] = 'N';
            DramStr[1] = '/';
            DramStr[2] = 'A';
            DramStr[3] = '\0';
        } else {
            DramStr[DramLeng]      = 'b';
            DramStr[DramLeng + 1U] = 'y';
            DramStr[DramLeng + 2U] = 't';
            DramStr[DramLeng + 3U] = 'e';
            DramStr[DramLeng + 4U] = 's';
            DramStr[DramLeng + 5U] = '\0';
        }
    } else {
        DramStr[DramLeng]      = 'G';
        DramStr[DramLeng + 1U] = 'B';
        DramStr[DramLeng + 2U] = '\0';
    }

    AmbaPrint_PrintStr5("Built amba_svc by %s at %s (%s) %s", AMBA_BUILD_MACHINE
                                                         , AMBA_BUILD_DATE
                                                         , OperStr
                                                         , DramStr
                                                         , NULL);
}
