/**
 *  @file AmbaSysStack.c
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
 *  @details Definitions for all of the system stacks belong to different processor modes
 *
 */

#include "AmbaTypes.h"
#include "AmbaSYS_Ctrl.h"

#define AMBA_SYS_STACK_SIZE     0x100U
#define AMBA_SVC_STACK_SIZE     0x2000U
#define AMBA_IRQ_STACK_SIZE     0x300U
#define AMBA_FIQ_STACK_SIZE     0x400U
#define AMBA_UND_STACK_SIZE     0x200U
#define AMBA_ABT_STACK_SIZE     0x200U

static UINT8 CpuCore0StackSYS[AMBA_SYS_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore0StackSVC[AMBA_SVC_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore0StackIRQ[AMBA_IRQ_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore0StackFIQ[AMBA_FIQ_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore0StackUND[AMBA_UND_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore0StackABT[AMBA_ABT_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));

static UINT8 CpuCore1StackSYS[AMBA_SYS_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore1StackSVC[AMBA_SVC_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore1StackIRQ[AMBA_IRQ_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore1StackFIQ[AMBA_FIQ_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore1StackUND[AMBA_UND_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore1StackABT[AMBA_ABT_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));

static UINT8 CpuCore2StackSYS[AMBA_SYS_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore2StackSVC[AMBA_SVC_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore2StackIRQ[AMBA_IRQ_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore2StackFIQ[AMBA_FIQ_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore2StackUND[AMBA_UND_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore2StackABT[AMBA_ABT_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));

static UINT8 CpuCore3StackSYS[AMBA_SYS_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore3StackSVC[AMBA_SVC_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore3StackIRQ[AMBA_IRQ_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore3StackFIQ[AMBA_FIQ_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore3StackUND[AMBA_UND_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));
static UINT8 CpuCore3StackABT[AMBA_ABT_STACK_SIZE] GNU_SECTION_NOZEROINIT __attribute__((aligned(64)));

UINT8 *const AmbaSysStackBaseSYS[4] = {
    [0] = &CpuCore0StackSYS[AMBA_SYS_STACK_SIZE],
    [1] = &CpuCore1StackSYS[AMBA_SYS_STACK_SIZE],
    [2] = &CpuCore2StackSYS[AMBA_SYS_STACK_SIZE],
    [3] = &CpuCore3StackSYS[AMBA_SYS_STACK_SIZE],
};
UINT8 *const AmbaSysStackBaseSVC[4] = {
    [0] = &CpuCore0StackSVC[AMBA_SVC_STACK_SIZE],
    [1] = &CpuCore1StackSVC[AMBA_SVC_STACK_SIZE],
    [2] = &CpuCore2StackSVC[AMBA_SVC_STACK_SIZE],
    [3] = &CpuCore3StackSVC[AMBA_SVC_STACK_SIZE],
};
UINT8 *const AmbaSysStackBaseIRQ[4] = {
    [0] = &CpuCore0StackIRQ[AMBA_IRQ_STACK_SIZE],
    [1] = &CpuCore1StackIRQ[AMBA_IRQ_STACK_SIZE],
    [2] = &CpuCore2StackIRQ[AMBA_IRQ_STACK_SIZE],
    [3] = &CpuCore3StackIRQ[AMBA_IRQ_STACK_SIZE],
};
UINT8 *const AmbaSysStackBaseFIQ[4] = {
    [0] = &CpuCore0StackFIQ[AMBA_FIQ_STACK_SIZE],
    [1] = &CpuCore1StackFIQ[AMBA_FIQ_STACK_SIZE],
    [2] = &CpuCore2StackFIQ[AMBA_FIQ_STACK_SIZE],
    [3] = &CpuCore3StackFIQ[AMBA_FIQ_STACK_SIZE],
};
UINT8 *const AmbaSysStackBaseUND[4] = {
    [0] = &CpuCore0StackUND[AMBA_UND_STACK_SIZE],
    [1] = &CpuCore1StackUND[AMBA_UND_STACK_SIZE],
    [2] = &CpuCore2StackUND[AMBA_UND_STACK_SIZE],
    [3] = &CpuCore3StackUND[AMBA_UND_STACK_SIZE],
};
UINT8 *const AmbaSysStackBaseABT[4] = {
    [0] = &CpuCore0StackABT[AMBA_ABT_STACK_SIZE],
    [1] = &CpuCore1StackABT[AMBA_ABT_STACK_SIZE],
    [2] = &CpuCore2StackABT[AMBA_ABT_STACK_SIZE],
    [3] = &CpuCore3StackABT[AMBA_ABT_STACK_SIZE],
};

UINT8 *const AmbaSysStackLimitSYS[4] = {
    [0] = &CpuCore0StackSYS[0],
    [1] = &CpuCore1StackSYS[0],
    [2] = &CpuCore2StackSYS[0],
    [3] = &CpuCore3StackSYS[0],
};
UINT8 *const AmbaSysStackLimitSVC[4] = {
    [0] = &CpuCore0StackSVC[0],
    [1] = &CpuCore1StackSVC[0],
    [2] = &CpuCore2StackSVC[0],
    [3] = &CpuCore3StackSVC[0],
};
UINT8 *const AmbaSysStackLimitIRQ[4] = {
    [0] = &CpuCore0StackIRQ[0],
    [1] = &CpuCore1StackIRQ[0],
    [2] = &CpuCore2StackIRQ[0],
    [3] = &CpuCore3StackIRQ[0],
};
UINT8 *const AmbaSysStackLimitFIQ[4] = {
    [0] = &CpuCore0StackFIQ[0],
    [1] = &CpuCore1StackFIQ[0],
    [2] = &CpuCore2StackFIQ[0],
    [3] = &CpuCore3StackFIQ[0],
};
UINT8 *const AmbaSysStackLimitUND[4] = {
    [0] = &CpuCore0StackUND[0],
    [1] = &CpuCore1StackUND[0],
    [2] = &CpuCore2StackUND[0],
    [3] = &CpuCore3StackUND[0],
};
UINT8 *const AmbaSysStackLimitABT[4] = {
    [0] = &CpuCore0StackABT[0],
    [1] = &CpuCore1StackABT[0],
    [2] = &CpuCore2StackABT[0],
    [3] = &CpuCore3StackABT[0],
};
