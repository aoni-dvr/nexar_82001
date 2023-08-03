/**
 *  @file DiagMPrint.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Print/Module-Print diagnostic code
 *
 */

#include <stdio.h>
#include <string.h>
#include "diag.h"
#include "AmbaGPIO_Def.h"
#include "AmbaPrint.h"
#include <time.h>
#include <stdint.h>

static void IoDiag_MPrintCmdUsage(char * const *pArgVector)
{
    printf("Usage: ");
    printf("       ");
    printf(pArgVector[0]);
    printf(" [test case]        : run selected test case\n");
    printf(" 0                  : AmbaPrint_PrintUInt5() \n");
    printf(" 1                  : AmbaPrint_PrintInt5() \n");
    printf(" 2                  : AmbaPrint_PrintStr5() \n");
    printf(" 3                  : enable module print for ID 0 \n");
    printf(" 4                  : AmbaPrint_ModulePrintStr5() for ID 0 \n");
    printf(" 5                  : AmbaPrint_ModulePrintUInt5() for ID 0 \n");
    printf(" 6                  : AmbaPrint_ModulePrintInt5() for ID 0 \n");
    printf(" 7                  : profile of many prints \n");
    printf(" test               : run case 0-6 \n");
}

static uint32_t getTick(void)
{
    struct timespec ts;
    uint32_t ticks = 0U;
    clock_gettime( CLOCK_MONOTONIC, &ts );
    ticks  = ts.tv_nsec / 1000000;
    ticks += ts.tv_sec * 1000;
    return ticks;
}

static void test_many_print(void)
{
    uint32_t i;
    uint32_t t1, t2, t_total;
    uint32_t test_count = 200;
    uint32_t boundary = 100; // in ms

    t1 = getTick();

    for (i = 0; i < test_count; i++) {
        AmbaPrint_PrintUInt5("xxxxxxxxxxxxxxxxyyyyyyyyyyyyyyyyzzzzzzzzzzzzzzzzzaaaaaaaaaaaaaaddddddddddbbb %d %d %d %d %d",9,8,7,6,5);
    }

    t2 = getTick();
    t_total = t2 - t1;

    if (t_total > boundary) {
        AmbaPrint_PrintUInt5("[NG] printing %d times takes (%d, %d) = %d ms > %d ms", test_count, t1, t2, t_total, boundary);
    } else {
        AmbaPrint_PrintUInt5("[OK] printing %d times takes (%d, %d) = %d ms < %d ms", test_count, t1, t2, t_total, boundary);
    }
    AmbaPrint_Flush();
}

int DoMPrintDiag(int argc, char *argv[])
{
    if (argc < 3) {
        IoDiag_MPrintCmdUsage(argv);
    } else {
        if (strcmp("test", argv[2]) == 0) {
            AmbaPrint_PrintUInt5("%d %d %d %d %d",9,8,7,6,5);
            AmbaPrint_PrintUInt5("%d %d %d %d %d",1,2,3,4,5);
            AmbaPrint_PrintInt5("%d %d %d %d %d",-1,-2,-3,-4,-5);
            AmbaPrint_PrintInt5("%d %d %d %d %d",-9,-8,-7,-6,-5);
            AmbaPrint_PrintStr5("%s %s %s %s %s","Arg1", "Arg2", "Arg3", "Arg4", "Arg5");
            AmbaPrint_PrintStr5("%s %s %s %s %s","H", "E", "L", "L", "O");
            AmbaPrint_ModuleSetAllowList(0,1);
            AmbaPrint_ModulePrintStr5(0, "M %s %s %s %s %s","Arg1", "Arg2", "Arg3", "Arg4", "Arg5");
            AmbaPrint_ModulePrintStr5(0, "M %s %s %s %s %s","H", "E", "L", "L", "O");
            AmbaPrint_ModulePrintUInt5(0, "M %d %d %d %d %d",9,8,7,6,5);
            AmbaPrint_ModulePrintUInt5(0, "M %d %d %d %d %d",1,2,3,4,5);
            AmbaPrint_ModulePrintInt5(0, "M %d %d %d %d %d",-1,-2,-3,-4,-5);
            AmbaPrint_ModulePrintInt5(0, "M %d %d %d %d %d",-9,-8,-7,-6,-5);
        } else if (strcmp("0", argv[2]) == 0) {
            AmbaPrint_PrintUInt5("%d %d %d %d %d",9,8,7,6,5);
            AmbaPrint_PrintUInt5("%d %d %d %d %d",1,2,3,4,5);
        } else if (strcmp("1", argv[2]) == 0) {
            AmbaPrint_PrintInt5("%d %d %d %d %d",-1,-2,-3,-4,-5);
            AmbaPrint_PrintInt5("%d %d %d %d %d",-9,-8,-7,-6,-5);
        } else if (strcmp("2", argv[2]) == 0) {
            AmbaPrint_PrintStr5("%s %s %s %s %s","Arg1", "Arg2", "Arg3", "Arg4", "Arg5");
            AmbaPrint_PrintStr5("%s %s %s %s %s","H", "E", "L", "L", "O");
        } else if (strcmp("3", argv[2]) == 0) {
            AmbaPrint_ModuleSetAllowList(0,1);
        } else if (strcmp("4", argv[2]) == 0) {
            AmbaPrint_ModulePrintStr5(0, "M %s %s %s %s %s","Arg1", "Arg2", "Arg3", "Arg4", "Arg5");
            AmbaPrint_ModulePrintStr5(0, "M %s %s %s %s %s","H", "E", "L", "L", "O");
        } else if (strcmp("5", argv[2]) == 0) {
            AmbaPrint_ModulePrintUInt5(0, "M %d %d %d %d %d",9,8,7,6,5);
            AmbaPrint_ModulePrintUInt5(0, "M %d %d %d %d %d",1,2,3,4,5);
        } else if (strcmp("6", argv[2]) == 0) {
            AmbaPrint_ModulePrintInt5(0, "M %d %d %d %d %d",-1,-2,-3,-4,-5);
            AmbaPrint_ModulePrintInt5(0, "M %d %d %d %d %d",-9,-8,-7,-6,-5);
        } else if (strcmp("7", argv[2]) == 0) {
            test_many_print();
        } else {
            IoDiag_MPrintCmdUsage(argv);
        }
    }
    return 0;
}
