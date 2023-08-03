/**
 *  @file AmbaCSL_DDRC.h
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
 *  @details Definitions & Constants for DDR Controller CSL APIs
 *
 */

#ifndef AMBA_CSL_DDRC_H
#define AMBA_CSL_DDRC_H

#include "AmbaReg_DDRC.h"

/*
 * Inline Function Definitions
 */
static inline UINT32 AmbaCSL_DdrcGetDramType(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->Config.DramType;
}
static inline UINT32 AmbaCSL_DdrcGetDramSize(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->Config.DramSize;
}
static inline UINT32 AmbaCSL_DdrcGetDqBusWidth(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->Config.DqBusWidth;
}
static inline UINT32 AmbaCSL_DdrcGetPageSize(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->Config.PageSize;
}

static inline void AmbaCSL_DdrcSetTrainScratchPad(UINT32 DdrcId, UINT32 Val)
{
    pAmbaDDRC_Reg[DdrcId]->TrainSwScratchpad = Val;
}
static inline void AmbaCSL_DdrcSetModeReg(UINT32 DdrcId, UINT32 Val)
{
    pAmbaDDRC_Reg[DdrcId]->ModeReg = Val;
}
static inline void AmbaCSL_DdrcSetDqWriteDly(UINT32 DdrcId, UINT32 Val)
{
    pAmbaDDRC_Reg[DdrcId]->TrainDqWriteDly = Val;
}
static inline void AmbaCSL_DdrcSetDqVref(UINT32 DdrcId, UINT32 Val)
{
    pAmbaDDRC_Reg[DdrcId]->TrainDqCaVref = Val;
}
static inline void AmbaCSL_DdrcSetDqReadDly(UINT32 DdrcId, UINT32 Value)
{
    pAmbaDDRC_Reg[DdrcId]->TrainDqReadDly = Value;
}
static inline void AmbaCSL_DdrcSetDll0Setting(UINT32 DdrcId, UINT32 Val)
{
    pAmbaDDRC_Reg[DdrcId]->DdrioDll0 = Val;
}
static inline void AmbaCSL_DdrcSetDll1Setting(UINT32 DdrcId, UINT32 Val)
{
    pAmbaDDRC_Reg[DdrcId]->DdrioDll1 = Val;
}
static inline void AmbaCSL_DdrcSetDll2Setting(UINT32 DdrcId, UINT32 Val)
{
    pAmbaDDRC_Reg[DdrcId]->DdrioDll2 = Val;
}
static inline void AmbaCSL_DdrcSetDll3Setting(UINT32 DdrcId, UINT32 Val)
{
    pAmbaDDRC_Reg[DdrcId]->DdrioDll3 = Val;
}
static inline void AmbaCSL_DdrcSetPadTerm(UINT32 DdrcId, UINT32 Val)
{
    pAmbaDDRC_Reg[DdrcId]->PadTerm = Val;
}
static inline void AmbaCSL_DdrcSetPadTerm2(UINT32 DdrcId, UINT32 Val)
{
    pAmbaDDRC_Reg[DdrcId]->PadTerm2 = Val;
}
static inline void AmbaCSL_DdrcSetPadTerm3(UINT32 DdrcId, UINT32 Val)
{
    pAmbaDDRC_Reg[DdrcId]->PadTerm3 = Val;
}

static inline UINT32 AmbaCSL_DdrcGetTrainScratchPad(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->TrainSwScratchpad;
}
static inline UINT32 AmbaCSL_DdrcGetDqWriteDly(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->TrainDqWriteDly;
}
static inline UINT32 AmbaCSL_DdrcGetDqReadDly(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->TrainDqReadDly;
}
static inline UINT32 AmbaCSL_DdrcGetDqVref(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->TrainDqCaVref;
}
static inline UINT32 AmbaCSL_DdrcGetDll0Setting(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->DdrioDll0;
}
static inline UINT32 AmbaCSL_DdrcGetDll1Setting(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->DdrioDll1;
}
static inline UINT32 AmbaCSL_DdrcGetDll2Setting(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->DdrioDll2;
}
static inline UINT32 AmbaCSL_DdrcGetDll3Setting(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->DdrioDll3;
}
static inline UINT32 AmbaCSL_DdrcGetModeReg(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->ModeReg;
}
static inline UINT32 AmbaCSL_DdrcGetPadTerm(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->PadTerm;
}
static inline UINT32 AmbaCSL_DdrcGetPadTerm2(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->PadTerm2;
}
static inline UINT32 AmbaCSL_DdrcGetPadTerm3(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->PadTerm3;
}

#endif /* AMBA_CSL_DDRC_H */
