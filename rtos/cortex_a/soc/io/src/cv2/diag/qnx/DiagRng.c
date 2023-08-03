/**
 *  @file DiagRng.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *  Copyright 2020, Ambarella International LP
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
 *  @details RNG (Random Number Generator) diagnostic code
 *
 */

#include <stdio.h>
#include <string.h>
#include "diag.h"
#include "AmbaTypes.h"
#include "AmbaRNG.h"

static void IoDiag_RngCmdUsage(char * const *pArgVector)
{
    printf("Usage: ");
    printf("       ");
    printf(pArgVector[0]);
    printf(" gen                         : generate 32-bit random number\n");
}

int DoRngDiag(int argc, char *argv[])
{
    UINT32 Retstatus;
    if (argc >= 3) {
        if (strcmp("start", argv[2]) == 0) {
            Retstatus = AmbaRNG_Init();
            if (Retstatus != RNG_ERR_NONE) {
                printf("Failed to start RNG = 0x%x \n", Retstatus);
            }
        } else if (strcmp("gen", argv[2]) == 0) {
            UINT32 data[4] = {0};
            Retstatus = AmbaRNG_DataGet(&data[0], &data[1], &data[2], &data[3], NULL);
            if (Retstatus != RNG_ERR_NONE) {
                printf("Failed to generate numbers = 0x%x \n", Retstatus);
            } else {
                printf("[0] 0x%x, [1] 0x%x, [2] 0x%x, [3] 0x%x\n", data[0], data[1], data[2], data[3]);
            }
        } else if (strcmp("stop", argv[2]) == 0) {
            Retstatus = AmbaRNG_Deinit();
            if (Retstatus != RNG_ERR_NONE) {
                printf("Failed to stop RNG = 0x%x \n", Retstatus);
            }
        } else {
            IoDiag_RngCmdUsage(argv);
        }
    } else {
        IoDiag_RngCmdUsage(argv);
    }
    return 0;
}
