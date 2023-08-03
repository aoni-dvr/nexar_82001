/**
*  @file main.c
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
*  @detail xxxx
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "schdr_api.h"
#include "cvtask_api.h"
#include "ambint.h"
#include "cvapi_ambacv_flexidag.h"

#include "main.h"

#define OPTIONS         "s:"

#define DEFAULT_CVMEM_SIZE    (96)   // 96 MB
#define ARM_LOG_CVFLOW_COMM    "CvflowComm"


static uint32_t cvmem_size = DEFAULT_CVMEM_SIZE;

static void cvflow_comm_print_help(const char *exe)
{
    printf("\nusage: %s [args]\n", exe);
    printf("\t-s [MemSize]\n");
    printf("\t\t The unit is MB. (default is 96MB)\n");
    printf("\t\t ./cvflow_comm -s 128 => Set CvMem to 128 MB\n");

}

static void cvflow_comm_parse_opt(int argc, char **argv)
{
    int c;

    optind = 1;
    while ((c = getopt(argc, argv, OPTIONS)) != -1) {
        switch (c) {
        case 's':
            cvmem_size = strtoul(optarg, NULL, 0);
            ArmLog_DBG(ARM_LOG_CVFLOW_COMM, "cvmem_size = %d MB", cvmem_size, 0U);
            break;
        default:
            cvflow_comm_print_help(argv[0]);
            exit(-1);
        }
    }
}

int main(int argc, char **argv)
{
    uint32_t mempool_id = 0, ret = 0;
    flexidag_memblk_t  total_buf = {0};
    static UINT32 schdr_flag = 0U;

    cvflow_comm_parse_opt(argc, argv);

    /* 1. Schdr start */
    if(schdr_flag == 0U) {
        AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg = {0};
        cfg.cpu_map           = 0xD;
        cfg.log_level         = LVL_DEBUG;
        ret = AmbaCV_FlexidagSchdrStart(&cfg);
        if (ret == 0U) {
            schdr_flag = 1;
        } else {
            ArmLog_ERR(ARM_LOG_CVFLOW_COMM, "## AmbaCV_FlexidagSchdrStart() fail", 0U, 0U);
        }
    }

    /* 2. create memory pool */
    if (ret == 0) {
        ret = AmbaCV_UtilityCmaMemAlloc(cvmem_size << 20, 1, &total_buf);
        if (ret != 0) {
            ArmLog_ERR(ARM_LOG_CVFLOW_COMM, "## Can't allocate %u from CmaMem", cvmem_size << 20, 0U);
        } else {
            ret = ArmMemPool_Create(&total_buf, &mempool_id);
        }
    }

    /* 3. create command mode thread (Blocking function) */
    if (ret == 0) {
        ret = shell_thread_create(mempool_id);
    }

    /* 4. free memory */
    if (total_buf.pBuffer != NULL) {
        ret = ArmMemPool_Delete(mempool_id);
        ret = AmbaCV_UtilityCmaMemFree(&total_buf);
    }

    return 0;
}

