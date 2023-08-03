/**
 *  @file AmbaShell_Reboot.c
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Shell command to reboot system
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaShell_Commands.h"
#include "AmbaSYS.h"

static inline void SHELL_RebootCmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: reboot [yes]\n");
}

/**
 *  AmbaShell_CommandReboot - execute command reboot
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandReboot(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount < 2U) {
        SHELL_RebootCmdUsage(PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "yes", 3U) == 0) {
            PrintFunc("rebooting...\n\n\n");
            (void)AmbaSYS_Reboot();
        }
    }
}
