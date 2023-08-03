/**
 *  @file AmbaSYS_Ctrl.h
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
 *  @details Definitions & Constants for internal system control APIs
 *
 */

#ifndef AMBA_SYS_CTRL_H
#define AMBA_SYS_CTRL_H

#ifndef AMBA_SYS_DEF_H
#include "AmbaSYS_Def.h"
#endif

#ifndef AMBA_VIN_H
#include "AmbaVIN.h"
#endif

#ifndef AMBA_VOUT_H
#include "AmbaVOUT.h"
#endif


#define PSCI_CPU_OFF            0x84000002U
#define PSCI_CPU_ON_AARCH32     0x84000003U
#if (defined(CONFIG_THREADX) && defined(CONFIG_ARM64))
#define PSCI_CPU_ON_AARCH64     0xC4000003U
#endif

#ifndef __ASM__

typedef int (*AMBA_SYS_INFO_PRINT_f)(const char *pFmtData, ...);

extern UINT8 *const AmbaSysStackBaseSYS[4];
extern UINT8 *const AmbaSysStackBaseSVC[4];
extern UINT8 *const AmbaSysStackBaseIRQ[4];
extern UINT8 *const AmbaSysStackBaseFIQ[4];
extern UINT8 *const AmbaSysStackBaseUND[4];
extern UINT8 *const AmbaSysStackBaseABT[4];

extern UINT8 *const AmbaSysStackLimitSYS[4];
extern UINT8 *const AmbaSysStackLimitSVC[4];
extern UINT8 *const AmbaSysStackLimitIRQ[4];
extern UINT8 *const AmbaSysStackLimitFIQ[4];
extern UINT8 *const AmbaSysStackLimitUND[4];
extern UINT8 *const AmbaSysStackLimitABT[4];

extern UINT32 SysElapsedTimeStart;

extern UINT32 AmbaRtosCoreId;

/*
 * Defined in tx_execution_profile.c
 */
void AmbaSysProfile_Init(void);
UINT32 AmbaSysProfile_GetTimeStamp(UINT32 CpuId);

/*
 * Defined in AmbaDiag_ThreadX.c
 */
#ifdef CONFIG_ARM64
void AmbaDiag_KalShowTaskInfo(AMBA_SYS_LOG_f LogFunc);
void AmbaDiag_KalShowSemaphoreInfo(AMBA_SYS_LOG_f LogFunc);
void AmbaDiag_KalShowMutexInfo(AMBA_SYS_LOG_f LogFunc);
void AmbaDiag_KalShowMsgQueueInfo(AMBA_SYS_LOG_f LogFunc);
void AmbaDiag_KalShowEventFlagsInfo(AMBA_SYS_LOG_f LogFunc);
void AmbaDiag_KalShowCpuLoadInfo(AMBA_SYS_LOG_f LogFunc);
#else
void AmbaDiag_KalShowTaskInfoA32(AMBA_SYS_LOG_f LogFunc);
void AmbaDiag_KalShowSemaphoreInfoA32(AMBA_SYS_LOG_f LogFunc);
void AmbaDiag_KalShowMutexInfoA32(AMBA_SYS_LOG_f LogFunc);
void AmbaDiag_KalShowMsgQueueInfoA32(AMBA_SYS_LOG_f LogFunc);
void AmbaDiag_KalShowEventFlagsInfoA32(AMBA_SYS_LOG_f LogFunc);
void AmbaDiag_KalShowCpuLoadInfoA32(AMBA_SYS_LOG_f LogFunc);
#endif

void AmbaDiag_KalResetCpuLoadInfo(void);

/*
 * Defined in AmbaDiag_Dram.c
 */
void AmbaDiag_DramResetStatis(void);
UINT32 AmbaDiag_DramShowStatisInfo(AMBA_SYS_LOG_f LogFunc);

/*
 * Defined in AmbaDiag_Int.c
 */
void AmbaDiag_IntEnableProfiler(void);
void AmbaDiag_IntRestartProfiler(void);
UINT32 AmbaDiag_IntShowInfo(UINT32 IntID, AMBA_SYS_LOG_f LogFunc);

#endif

#endif /* AMBA_SYS_CTRL_H */
