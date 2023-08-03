/**
 *  @file AmbaDSP.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for Ambarella DSP Driver
 *
 */
#ifndef AMBA_DSP_H
#define AMBA_DSP_H

#include "AmbaTypes.h"
#include "AmbaDSP_Def.h"
#include "AmbaDSP_EventInfo.h"

/* Error definition */
#define DSP_ERR_NONE                        (0U)
#define DSP_ERR_0000                        (SSP_ERR_BASE       ) // Invalid argument (null pointer)
#define DSP_ERR_0001                        (SSP_ERR_BASE + 0x1U) // Input argument out of range
#define DSP_ERR_0002                        (SSP_ERR_BASE + 0x2U) // Buffer alignment error
#define DSP_ERR_0003                        (SSP_ERR_BASE + 0x3U) // OS resource error
#define DSP_ERR_0004                        (SSP_ERR_BASE + 0x4U) // Invalid timing
#define DSP_ERR_0005                        (SSP_ERR_BASE + 0x5U) // Queue full
#define DSP_ERR_0006                        (SSP_ERR_BASE + 0x6U) // Liveview routing fail
#define DSP_ERR_0007                        (SSP_ERR_BASE + 0x7U) // Internal write command error
#define DSP_ERR_0008                        (SSP_ERR_BASE + 0x8U) // not support or not implement

UINT32 AmbaDSP_MainInit(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig);

UINT32 AmbaDSP_MainGetDefaultSysCfg(AMBA_DSP_SYS_CONFIG_s *pDspSysConfig);

UINT32 AmbaDSP_MainSuspend(void);

UINT32 AmbaDSP_MainResume(void);

UINT32 AmbaDSP_MainSetWorkArea(ULONG WorkAreaAddr, UINT32 WorkSize);

UINT32 AmbaDSP_MainGetDspVerInfo(AMBA_DSP_VERSION_INFO_s *Info);

UINT32 AmbaDSP_MainMsgParseEntry(UINT32 EntryArg);

UINT32 AmbaDSP_MainWaitVinInterrupt(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut);

UINT32 AmbaDSP_MainWaitVoutInterrupt(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut);

UINT32 AmbaDSP_MainWaitFlag(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut);

UINT32 AmbaDSP_ResourceLimitConfig(const AMBA_DSP_RESOURCE_s *pResource);

UINT32 AmbaDSP_CalHierBufferSize(const UINT16 *pMaxMainWidth, const UINT16 *pMaxMainHeight,
                                 const UINT16 *pMaxHierWidth, const UINT16 *pMaxHierHeight,
                                 const UINT16 OctaveMode,
                                 UINT32 *pHierBufSize, UINT16 *pHierBufWidth, UINT16 *pHierBufHeight);


UINT32 AmbaDSP_GetCmprRawBufInfo(UINT16 Width, UINT16 CmprRate, UINT16 *pRawWidth, UINT16 *pRawPitch);

#define DSP_REGION_VOUT_IDX     (0U) //Support VOUT feature
#define DSP_REGION_LV_IDX       (1U) //Support Liveview feature
#define DSP_REGION_ENC_IDX      (2U) //Support Encode feature
#define DSP_REGION_DEC_IDX      (3U) //Support Decode feature
#define DSP_REGION_NUM          (4U)
UINT32 AmbaDSP_ParLoadConfig(UINT32 Enable, UINT32 Data);

UINT32 AmbaDSP_ParLoadRegionUnlock(UINT16 RegionIdx, UINT16 SubRegionIdx);

UINT32 AmbaDSP_CalcEncMvBufInfo(UINT16 Width, UINT16 Height, UINT32 Option, UINT32 *pBufSize);

UINT32 AmbaDSP_CalVpMsgBufferSize(const UINT32 *NumMsgs, UINT32 *MsgSize);

#define DSP_PROTECT_AREA_DATA   (0U)
#define NUM_DSP_PROTECT_AREA    (1U)
UINT32 AmbaDSP_MainSetProtectArea(UINT32 Type, ULONG AreaAddr, UINT32 Size, UINT32 IsCached);

#define DSP_BUF_TYPE_WORK       (0U)
#define DSP_BUF_TYPE_LOG        (1U)
#define DSP_BUF_TYPE_DATA       (2U)
#define DSP_BUF_TYPE_STAT       (3U) //internal use
#define DSP_BUF_TYPE_BIN        (4U) //internal use
#define DSP_BUF_TYPE_EVT        (5U) //internal use
#define NUM_DSP_BUF_TYPE        (6U)
UINT32 AmbaDSP_MainGetBufInfo(UINT32 Type, AMBA_DSP_BUF_INFO_s *pBufInfo);

#define DSP_CACHE_OP_CLEAN      (0U)
#define DSP_CACHE_OP_INVLD      (1U)
#define NUM_DSP_CACHE_OP        (2U)
UINT32 AmbaDSP_MainCacheBufOp(UINT32 Operation, ULONG Addr, UINT32 Size);

/* Internal use only */
UINT32 AmbaDSP_VirtToPhys(ULONG VirtAddr, UINT32 *pPhysAddr);
UINT32 AmbaDSP_VirtToCli(ULONG VirtAddr, UINT32 *pCliAddr);
UINT32 AmbaDSP_CliToVirt(UINT32 CliAddr, ULONG *pVirtAddr);

#define DSP_STATUS_TYPE_OP_MODE     (0U)
#define DSP_STATUS_TYPE_ENC_MODE    (1U)
#define DSP_STATUS_TYPE_DEC_MODE    (2U)
#define NUM_DSP_STATUS_TYPE         (3U)
UINT32 AmbaDSP_GetStatus(UINT32 Type, UINT32 Id, UINT32 *pData0, UINT32 *pData1);

UINT32 AmbaDSP_ClockUpdate(UINT32 ClockId, UINT32 Stage, UINT32 Freq);

#endif  /* AMBA_DSP_H */
