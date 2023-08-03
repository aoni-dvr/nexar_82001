/**
 *  @file AmbaDramAdjust.c
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
 *  @details Dram adjust functions
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"

#include "AmbaDramAdjust.h"

/**
 *  AmbaDramAdjust_Init
 *  @return none
 */
void AmbaDramAdjust_Init(void)
{
    /* do nothing */
}

/**
 *  AmbaDramAdjust_Start
 *  @param[in] pConfig settings to start the adjust task
 *  @return 0-OK, Other-ErrorCode
 */
UINT32 AmbaDramAdjust_Start(AMBA_DRAM_ADJUST_CONFIG_s *pConfig)
{
    char    CmdBuf[128];
    UINT32  RetVal = ERR_NONE;

    sprintf(CmdBuf, "echo %u > /proc/ambarella/lp5adj", (pConfig->Duration / 1000U));
    RetVal = system(CmdBuf);
    if (RetVal != ERR_NONE) {
        AmbaPrint_PrintUInt5("## fail to start dram adjust", 0U, 0U, 0U, 0U, 0U);
    }

    return RetVal;
}

/**
 *  AmbaDramAdjust_Stop
 *  @return none
 */
void AmbaDramAdjust_Stop(void)
{
    UINT32  RetVal = ERR_NONE;

    RetVal = system("echo 0 > /proc/ambarella/lp5adj");
    if (RetVal != ERR_NONE) {
        AmbaPrint_PrintUInt5("## fail to stop dram adjust", 0U, 0U, 0U, 0U, 0U);
    }
}

/**
 *  AmbaDramAdjust_Show
 *  @return none
 */
void AmbaDramAdjust_Show(void)
{
    UINT32  RetVal = ERR_NONE;

    RetVal = system("echo -1 > /proc/ambarella/lp5adj");
    if (RetVal != ERR_NONE) {
        AmbaPrint_PrintUInt5("## fail to switch print result of dram adjust", 0U, 0U, 0U, 0U, 0U);
    }
}

