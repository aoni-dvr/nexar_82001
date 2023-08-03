/**
 *  @file AmbaCSL_DebugPort.h
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for DSP Debug Port CSL APIs
 *
 */

#ifndef AMBA_CSL_DEBUG_PORT_H
#define AMBA_CSL_DEBUG_PORT_H

#include "debugport/AmbaDebugPort.h"
#include "debugport/AmbaDebugPort_ORC.h"
#include "debugport/AmbaDebugPort_IDSP.h"
#include "debugport/AmbaDebugPort_DRAM.h"

#define AMBA_IDSP_SECTION1  1U
#define AMBA_IDSP_SECTION2  2U
#define AMBA_IDSP_SECTION3  3U
#define AMBA_IDSP_SECTION4  4U
#define AMBA_IDSP_SECTION5  5U
#define AMBA_IDSP_SECTION6  6U
#define AMBA_IDSP_SECTION7  7U
#define AMBA_IDSP_SECTION8  8U
#define AMBA_IDSP_SECTION9  9U
#define AMBA_IDSP_SECTION10 10U
#define AMBA_IDSP_SECTION11 11U
#define AMBA_IDSP_SECTION12 12U
#define AMBA_IDSP_SECTION13 13U
#define AMBA_IDSP_SECTION14 14U
#define AMBA_IDSP_SECTION15 15U

/*
 * Defined in AmbaMmioBase.asm
 */
#ifdef CONFIG_LINUX
extern AMBA_DBG_PORT_ORC_REG_s *pAmbaORC_Reg[4];
extern AMBA_DBG_PORT_DRAM_REG_s *pAmbaDRAM_Reg;
#else
extern AMBA_DBG_PORT_ORC_REG_s *const pAmbaORC_Reg[4];
extern AMBA_DBG_PORT_DRAM_REG_s *const pAmbaDRAM_Reg;
#endif
extern AMBA_DBG_PORT_IDSP_CONTROLLER_REG_s *pAmbaIDSP_CtrlReg;
extern AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s *pAmbaIDSP_VinMainReg;
extern AMBA_DBG_PORT_IDSP_VIN_GLOBAL_REG_s *pAmbaIDSP_VinGlobalReg;
extern AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_REG_s *pAmbaIDSP_VinMasterSyncReg;

/*
 * Macro Definitions
 */
static inline void AmbaCSL_DebugPortSetOrcPC(UINT32 i, UINT32 d)
{
    pAmbaORC_Reg[i]->OrcResetPC = d;
}
static inline void AmbaCSL_DebugPortEnableORC(UINT32 i, UINT32 d)
{
    pAmbaORC_Reg[i]->OrcThreadReset = d;
}
static inline void AmbaCSL_DebugPortResetORC(UINT32 i, UINT32 d)
{
    pAmbaORC_Reg[i]->OrcInvalidIcache = d;
}
static inline UINT32 AmbaCSL_GetOrcTimer(void)
{
    return pAmbaORC_Reg[0]->CurrentTimer;
}

static inline void AmbaCSL_DebugPortSetSyncCounter(UINT32 i, UINT32 d)
{
    pAmbaDRAM_Reg->SyncCounter[i] = (((d) << 12U) | (d));
}

static inline void AmbaCSL_IdspSetAddrExtensionData(UINT32 d)
{
    pAmbaIDSP_CtrlReg->AddressExtension = d;
}
static inline void AmbaCSL_IdspSetAddrExtension(UINT32 Section, UINT32 FilterID)
{
    pAmbaIDSP_CtrlReg->AddressExtension = ((Section << 12U) | (FilterID << 4U));
}
static inline void AmbaCSL_IdspSectionReset(UINT32 d)
{
    pAmbaIDSP_CtrlReg->SectionReset = d;
}
static inline void AmbaCSL_IdspResetVinMasterSyncGeneration(UINT32 d)
{
    pAmbaIDSP_CtrlReg->SectionReset = (0x1U & d);
}
static inline void AmbaCSL_IdspResetPipMasterSyncGeneration(UINT32 d)
{
    pAmbaIDSP_CtrlReg->SectionReset = (0x80000000U & (d << 31U));
}
static inline void AmbaCSL_IdspResetVinMipiPhy(void)
{
    pAmbaIDSP_CtrlReg->SectionReset = ((UINT32)1U << 24U);
}
static inline void AmbaCSL_IdspResetPipMipiPhy(void)
{
    pAmbaIDSP_CtrlReg->SectionReset = ((UINT32)2U << 24U);
}
static inline void AmbaCSL_IdspResetVinPipelineSection(void)
{
    pAmbaIDSP_CtrlReg->SectionReset = ((UINT32)1U << 1U);
}
static inline void AmbaCSL_IdspResetPipPipelineSection(void)
{
    pAmbaIDSP_CtrlReg->SectionReset = (((UINT32)1U << (AMBA_IDSP_SECTION10 - 1U)) << 1U);
}
static inline void AmbaCSL_IdspReleaseResetAll(void)
{
    pAmbaIDSP_CtrlReg->SectionReset = (0x0U);
}

static inline UINT32 AmbaCSL_IdspGetAddrExtensionData(void)
{
    return pAmbaIDSP_CtrlReg->AddressExtension;
}
static inline UINT32 AmbaCSL_IdspGetSection(void)
{
    return ((pAmbaIDSP_CtrlReg->AddressExtension & 0xf000U) >> 12U);
}
static inline UINT32 AmbaCSL_IdspGetSensorType(void)
{
    return pAmbaIDSP_VinGlobalReg->GlobalConfig.SensorType;
}

#endif /* AMBA_CSL_DEBUG_PORT_H */
