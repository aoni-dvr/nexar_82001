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

#ifndef __ASSEMBLER__
#include "AmbaReg_DDRC.h"
#endif

/*
 * Common Definition and Structure
 */
/**
 * Mapping to share the same spaces in BST/BLD/APP
 */
#define DDRCT_SCRATCHPAD_START                  0x20e0030000ULL
#define DDRCT_SCRATCHPAD_SIZE                   (16 *1024)
#define DDRCT_SCRATCHPAD_END                    (DDRCT_SCRATCHPAD_START + DDRCT_SCRATCHPAD_SIZE)
/* Map/Partition of ScratchPad Usage */
#define DDRCT_LOG_SPACE                         DDRCT_SCRATCHPAD_START
#define DDRCT_LOG_SIZE                          (12 * 1024)
#define DDRCT_STACK_SPACE                       (DDRCT_LOG_SPACE + DDRCT_LOG_SIZE)
#define DDRCT_STACK_SIZE                        ((4 * 1024) - DDRCT_RESULT_SIZE)
#define DDRCT_RESULT_SPACE                      (DDRCT_SCRATCHPAD_END - DDRCT_RESULT_SIZE)          // Last 128 byte for training result
#define DDRCT_RESULT_SIZE                       128
/* Map/Partition of ScratchPad-Result Usage */
#define DDRCT_RESULT_MR30_START                 DDRCT_RESULT_SPACE
#define DDRCT_RESULT_MR30_SIZE                  32  // 2 DDRc * 2 Die * 2 Channel * 4 bytes = 32 bytes
#define DDRCT_RESULT_WCK2DQX_START              (DDRCT_RESULT_MR30_START + DDRCT_RESULT_MR30_SIZE)
#define DDRCT_RESULT_WCK2DQX_SIZE               32  // 2 DDRc * 2 Die * 2 Channel * 4 bytes = 32 bytes
// Store Valid code from the end
#define DDRCT_RESULT_VALID_MAGIC_CODE           0x61626D41U  /* Amba */
#define DDRCT_RESULT_MR30_VALID                 (DDRCT_RESULT_SPACE + DDRCT_RESULT_SIZE - 4)
#define DDRCT_RESULT_WCK2DQX_VALID              (DDRCT_RESULT_MR30_VALID - 4)
#define DDRCT_RESULT_MR30_ADDR(ddrc,die,channel)        (DDRCT_RESULT_MR30_START + (ddrc * 16) + (die * 8) + (channel * 4))
#define DDRCT_RESULT_WCK2DQX_ADDR(ddrc,die,channel)     (DDRCT_RESULT_WCK2DQX_START + (ddrc * 16) + (die * 8) + (channel * 4))

/* Definitions of DDRC DLL byte */
#define AMBA_DDRC_DLL_BYTE_0        0U      /* DLL_Read */
#define AMBA_DDRC_DLL_BYTE_1        1U      /* DLL_Sync */
#define AMBA_DDRC_DLL_BYTE_2        2U      /* DLL_Write */

/* BITFIELD definitions */
#define READ_DELAY_BITS             5U      // 5 bits
#define READ_DELAY_POS_COARSE       15U     // [19:15]
#define READ_DELAY_POS_FINE         25U     // [29:25]
#define READ_DELAY_MASK             ((1U << READ_DELAY_BITS) - 1U)
#define DQ_VREF_BITS                6U      // 6 bits
#define DQ_VREF_POS1                0U      // [5:0]
#define DQ_VREF_POS2                16U     // [21:16]
#define DQ_VREF_MASK                ((1U << DQ_VREF_BITS) - 1U)
#define LP4_MR14_BITS               7U
#define LP4_MR14_MASK               ((1U << LP4_MR14_BITS) - 1U)
#define LP4_MR12_BITS               7U
#define LP4_MR12_MASK               ((1U << LP4_MR12_BITS) - 1U)
#define WRITE_DELAY_BITS            5U      // 5 bits
#define WRITE_DELAY_POS_COARSE      10U     // [14:10]
#define WRITE_DELAY_POS_FINE        20U     // [24:20]
#define WRITE_DELAY_MASK            ((1U << WRITE_DELAY_BITS) - 1U)
#define DQS_VREF_BITS               6U      // 6 bits
#define DQS_VREF_POS1               6U      // [11:6]
#define DQS_VREF_POS2               22U     // [27:22]
#define DQS_VREF_MASK               ((1U << DQS_VREF_BITS) - 1U)
#define WCK_DLY_BITS                5U      // 5 bits
#define WCK_DLY_POS_COARSE          5U      // Delay0[9:5]
#define WCK_DLY_POS_FINE            0U      // Delay0[4:0]
#define WCK_DLY_MASK                ((1U << WCK_DLY_BITS) - 1U)
#define DQS_WRITE_DLY_BITS          5U      // 5 bits
#define DQS_WRITE_DLY_POS_COARSE    15U     // Delay0[19:15]
#define DQS_WRITE_DLY_POS_FINE      10U     // Delay0[14:10]
#define DQS_WRITE_DLY_MASK          ((1U << DQS_WRITE_DLY_BITS) - 1U)
#define DQS_GATE_DLY_BITS           5U      // 5 bits
#define DQS_GATE_DLY_POS_COARSE     5U      // Delay1[9:5]
#define DQS_GATE_DLY_POS_FINE       0U      // Delay1[4:0]
#define DQS_GATE_DLY_MASK           ((1U << DQS_GATE_DLY_BITS) - 1U)
#define CK_DLY_BITS                 5U      // 5 bits
#define CK_DLY_POSA_COARSE          5U      // CK_Delay[9:5]
#define CK_DLY_POSA_FINE            0U      // CK_Delay[4:0]
#define CK_DLY_POSB_COARSE          21U     // CK_Delay[25:21]
#define CK_DLY_POSB_FINE            16U     // CK_Delay[20:16]
#define CK_DLY_MASK                 ((1U << CK_DLY_BITS) - 1U)
#define CKE_DLY_BITS                5U      // 5 bits
#define CKE_DLY_POSA_COARSE         0U      // CKE_Coarse[4:0]
#define CKE_DLY_POSB_COARSE         8U      // CKE_Coarse[12:8]
#define CKE_DLY_POS0A_FINE          0U      // CKE_Fine[4:0]
#define CKE_DLY_POS0B_FINE          8U      // CKE_Fine[12:8]
#define CKE_DLY_POS1A_FINE          16U     // CKE_Fine[20:16]
#define CKE_DLY_POS1B_FINE          24U     // CKE_Fine[28:24]
#define CKE_DLY_MASK                ((1U << CKE_DLY_BITS) - 1U)
#define CA_DLY_BITS                 5U      // 5 bits
#define CA_DLY_POSA_COARSE          0U      // CA_Coarse[4:0]
#define CA_DLY_POSB_COARSE          8U      // CA_Coarse[12:8]
#define CA_DLY_POS0_ADDR0_FINE      0U      // CA_Fine[0][4:0]
#define CA_DLY_POS0_ADDR1_FINE      8U      // CA_Fine[0][12:8]
#define CA_DLY_POS0_ADDR2_FINE      16U     // CA_Fine[0][20:16]
#define CA_DLY_POS0_ADDR3_FINE      24U     // CA_Fine[0][28:24]
#define CA_DLY_POS0_ADDR4_FINE      0U      // CA_Fine[1][4:0]
#define CA_DLY_POS0_ADDR5_FINE      8U      // CA_Fine[1][12:8]
#define CA_DLY_POS0_ADDR6_FINE      16U     // CA_Fine[1][20:16]
#define CA_DLY_MASK                 ((1U << CA_DLY_BITS) - 1U)
#define LP5_MR14_BITS               7U
#define LP5_MR14_MASK               ((1U << LP5_MR14_BITS) - 1U)
#define LP5_MR15_BITS               7U
#define LP5_MR15_MASK               ((1U << LP5_MR15_BITS) - 1U)
#define LP5_MR12_BITS               7U
#define LP5_MR12_MASK               ((1U << LP5_MR12_BITS) - 1U)


#ifndef __ASSEMBLER__
/*
 * Inline Function Definitions
 */

static inline void AmbaCSL_DdrcSetCaDlyCoars(UINT32 HostId, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->CaDlyCoarse = Val;
}
static inline void AmbaCSL_DdrcSetCaDlyFineChAD0(UINT32 HostId, UINT32 Index, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->CaDlyFineChADie0[Index] = Val;
}
static inline void AmbaCSL_DdrcSetCaDlyFineChAD1(UINT32 HostId, UINT32 Index, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->CaDlyFineChADie1[Index] = Val;
}
static inline void AmbaCSL_DdrcSetCaDlyFineChBD0(UINT32 HostId, UINT32 Index, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->CaDlyFineChBDie0[Index] = Val;
}
static inline void AmbaCSL_DdrcSetCaDlyFineChBD1(UINT32 HostId, UINT32 Index, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->CaDlyFineChBDie1[Index] = Val;
}
static inline void AmbaCSL_DdrcSetCkeDlyCoars(UINT32 HostId, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->CkeDlyCoarse = Val;
}
static inline void AmbaCSL_DdrcSetCkeDlyFine(UINT32 HostId, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->CkeDlyFine = Val;
}
static inline void AmbaCSL_DdrcSetCkDly(UINT32 HostId, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->CkDly = Val;
}
static inline void AmbaCSL_DdrcSetD0Dll0(UINT32 HostId, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->DllCtrlSel0D0 = Val;
}
static inline void AmbaCSL_DdrcSetD1Dll0(UINT32 HostId, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->DllCtrlSel0D1 = Val;
}
static inline void AmbaCSL_DdrcSetD0Dll1(UINT32 HostId, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->DllCtrlSel1D0 = Val;
}
static inline void AmbaCSL_DdrcSetD1Dll1(UINT32 HostId, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->DllCtrlSel1D1 = Val;
}
static inline void AmbaCSL_DdrcSetD0Dll2(UINT32 HostId, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->DllCtrlSel2D0 = Val;
}
static inline void AmbaCSL_DdrcSetD1Dll2(UINT32 HostId, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->DllCtrlSel2D1 = Val;
}
static inline void AmbaCSL_DdrcSetRdVref0(UINT32 HostId, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->RdVref0 = Val;
}
static inline void AmbaCSL_DdrcSetRdVref1(UINT32 HostId, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->RdVref1 = Val;
}
static inline void AmbaCSL_DdrcSetByte0D0Dly(UINT32 HostId, UINT32 Index, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->Byte0Die0Dly[Index] = Val;
}
static inline void AmbaCSL_DdrcSetByte1D0Dly(UINT32 HostId, UINT32 Index, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->Byte1Die0Dly[Index] = Val;
}
static inline void AmbaCSL_DdrcSetByte2D0Dly(UINT32 HostId, UINT32 Index, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->Byte2Die0Dly[Index] = Val;
}
static inline void AmbaCSL_DdrcSetByte3D0Dly(UINT32 HostId, UINT32 Index, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->Byte3Die0Dly[Index] = Val;
}
static inline void AmbaCSL_DdrcSetByte0D1Dly(UINT32 HostId, UINT32 Index, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->Byte0Die1Dly[Index] = Val;
}
static inline void AmbaCSL_DdrcSetByte1D1Dly(UINT32 HostId, UINT32 Index, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->Byte1Die1Dly[Index] = Val;
}
static inline void AmbaCSL_DdrcSetByte2D1Dly(UINT32 HostId, UINT32 Index, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->Byte2Die1Dly[Index] = Val;
}
static inline void AmbaCSL_DdrcSetByte3D1Dly(UINT32 HostId, UINT32 Index, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->Byte3Die1Dly[Index] = Val;
}
static inline void AmbaCSL_DdrcSetModeReg(UINT32 DdrcId, UINT32 Val)
{
    pAmbaDDRC_Reg[DdrcId]->ModeReg = Val;
}
static inline void AmbaCSL_DdrcSetTrainScratchPad(UINT32 HostId, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->TrnSwScratchpad = Val;
}
static inline void AmbaCSL_DdrcSetUInstruction(UINT32 HostId, UINT32 Index, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->Uinstruction[Index] = Val;
}
static inline void AmbaCSL_DdrcSetMisc2(UINT32 HostId, UINT32 Val)
{
    pAmbaDDRC_Reg[HostId]->DdrcMisc2 = Val;
}

static inline UINT32 AmbaCSL_DdrcGetDramType(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->Config0.DramType;
}
static inline UINT32 AmbaCSL_DdrcGetDramSize(UINT32 DdrcId)
{
    return pAmbaDDRC_Reg[DdrcId]->Config0.DramSize;
}
static inline UINT32 AmbaCSL_DdrcGetTrainScratchPad(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->TrnSwScratchpad;
}
static inline UINT32 AmbaCSL_DdrcGetCaDlyCoars(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->CaDlyCoarse;
}
static inline UINT32 AmbaCSL_DdrcGetCaDlyFineChAD0(UINT32 HostId, UINT32 Index)
{
    return pAmbaDDRC_Reg[HostId]->CaDlyFineChADie0[Index];
}
static inline UINT32 AmbaCSL_DdrcGetCaDlyFineChAD1(UINT32 HostId, UINT32 Index)
{
    return pAmbaDDRC_Reg[HostId]->CaDlyFineChADie1[Index];
}
static inline UINT32 AmbaCSL_DdrcGetCaDlyFineChBD0(UINT32 HostId, UINT32 Index)
{
    return pAmbaDDRC_Reg[HostId]->CaDlyFineChBDie0[Index];
}
static inline UINT32 AmbaCSL_DdrcGetCaDlyFineChBD1(UINT32 HostId, UINT32 Index)
{
    return pAmbaDDRC_Reg[HostId]->CaDlyFineChBDie1[Index];
}
static inline UINT32 AmbaCSL_DdrcGetCkeDlyCoars(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->CkeDlyCoarse;
}
static inline UINT32 AmbaCSL_DdrcGetCkeDlyFine(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->CkeDlyFine;
}
static inline UINT32 AmbaCSL_DdrcGetCkDly(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->CkDly;
}
static inline UINT32 AmbaCSL_DdrcGetD0Dll0(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->DllCtrlSel0D0;
}
static inline UINT32 AmbaCSL_DdrcGetD1Dll0(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->DllCtrlSel0D1;
}
static inline UINT32 AmbaCSL_DdrcGetD0Dll1(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->DllCtrlSel1D0;
}
static inline UINT32 AmbaCSL_DdrcGetD1Dll1(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->DllCtrlSel1D1;
}
static inline UINT32 AmbaCSL_DdrcGetD0Dll2(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->DllCtrlSel2D0;
}
static inline UINT32 AmbaCSL_DdrcGetD1Dll2(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->DllCtrlSel2D1;
}
static inline UINT32 AmbaCSL_DdrcGetRdVref0(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->RdVref0;
}
static inline UINT32 AmbaCSL_DdrcGetRdVref1(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->RdVref1;
}
static inline UINT32 AmbaCSL_DdrcGetWriteVref0(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->WriteVref0;
}
static inline UINT32 AmbaCSL_DdrcGetWriteVref1(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->WriteVref1;
}
static inline UINT32 AmbaCSL_DdrcGetByte0D0Dly(UINT32 HostId, UINT32 Index)
{
    return pAmbaDDRC_Reg[HostId]->Byte0Die0Dly[Index];
}
static inline UINT32 AmbaCSL_DdrcGetByte1D0Dly(UINT32 HostId, UINT32 Index)
{
    return pAmbaDDRC_Reg[HostId]->Byte1Die0Dly[Index];
}
static inline UINT32 AmbaCSL_DdrcGetByte2D0Dly(UINT32 HostId, UINT32 Index)
{
    return pAmbaDDRC_Reg[HostId]->Byte2Die0Dly[Index];
}
static inline UINT32 AmbaCSL_DdrcGetByte3D0Dly(UINT32 HostId, UINT32 Index)
{
    return pAmbaDDRC_Reg[HostId]->Byte3Die0Dly[Index];
}
static inline UINT32 AmbaCSL_DdrcGetByte0D1Dly(UINT32 HostId, UINT32 Index)
{
    return pAmbaDDRC_Reg[HostId]->Byte0Die1Dly[Index];
}
static inline UINT32 AmbaCSL_DdrcGetByte1D1Dly(UINT32 HostId, UINT32 Index)
{
    return pAmbaDDRC_Reg[HostId]->Byte1Die1Dly[Index];
}
static inline UINT32 AmbaCSL_DdrcGetByte2D1Dly(UINT32 HostId, UINT32 Index)
{
    return pAmbaDDRC_Reg[HostId]->Byte2Die1Dly[Index];
}
static inline UINT32 AmbaCSL_DdrcGetByte3D1Dly(UINT32 HostId, UINT32 Index)
{
    return pAmbaDDRC_Reg[HostId]->Byte3Die1Dly[Index];
}

static inline UINT32 AmbaCSL_DdrcGetModeReg(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->ModeReg;
}
static inline UINT32 AmbaCSL_DdrcGetUInstruction(UINT32 HostId, UINT32 Index)
{
    return pAmbaDDRC_Reg[HostId]->Uinstruction[Index];
}
static inline UINT32 AmbaCSL_DdrcGetMisc2(UINT32 HostId)
{
    return pAmbaDDRC_Reg[HostId]->DdrcMisc2;
}

#if defined(CONFIG_TEST_DRAM_SAVE_RESTORE)
static inline volatile UINT32* AmbaCSL_DdrcGetTrainScratchPadAddr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->TrnSwScratchpad;
}
static inline volatile UINT32* AmbaCSL_DdrcGetCaDlyCoarsAddr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->CaDlyCoarse;
}
static inline volatile UINT32* AmbaCSL_DdrcGetCaDlyFineChAD0Addr(UINT32 HostId, UINT32 Index)
{
    return &pAmbaDDRC_Reg[HostId]->CaDlyFineChADie0[Index];
}
static inline volatile UINT32* AmbaCSL_DdrcGetCaDlyFineChAD1Addr(UINT32 HostId, UINT32 Index)
{
    return &pAmbaDDRC_Reg[HostId]->CaDlyFineChADie1[Index];
}
static inline volatile UINT32* AmbaCSL_DdrcGetCaDlyFineChBD0Addr(UINT32 HostId, UINT32 Index)
{
    return &pAmbaDDRC_Reg[HostId]->CaDlyFineChBDie0[Index];
}
static inline volatile UINT32* AmbaCSL_DdrcGetCaDlyFineChBD1Addr(UINT32 HostId, UINT32 Index)
{
    return &pAmbaDDRC_Reg[HostId]->CaDlyFineChBDie1[Index];
}
static inline volatile UINT32* AmbaCSL_DdrcGetCkeDlyCoarsAddr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->CkeDlyCoarse;
}
static inline volatile UINT32* AmbaCSL_DdrcGetCkeDlyFineAddr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->CkeDlyFine;
}
static inline volatile UINT32* AmbaCSL_DdrcGetCkDlyAddr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->CkDly;
}
static inline volatile UINT32* AmbaCSL_DdrcGetD0Dll0Addr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->DllCtrlSel0D0;
}
static inline volatile UINT32* AmbaCSL_DdrcGetD1Dll0Addr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->DllCtrlSel0D1;
}
static inline volatile UINT32* AmbaCSL_DdrcGetD0Dll1Addr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->DllCtrlSel1D0;
}
static inline volatile UINT32* AmbaCSL_DdrcGetD1Dll1Addr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->DllCtrlSel1D1;
}
static inline volatile UINT32* AmbaCSL_DdrcGetD0Dll2Addr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->DllCtrlSel2D0;
}
static inline volatile UINT32* AmbaCSL_DdrcGetD1Dll2Addr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->DllCtrlSel2D1;
}
static inline volatile UINT32* AmbaCSL_DdrcGetRdVref0Addr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->RdVref0;
}
static inline volatile UINT32* AmbaCSL_DdrcGetRdVref1Addr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->RdVref1;
}
static inline volatile UINT32* AmbaCSL_DdrcGetWriteVref0Addr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->WriteVref0;
}
static inline volatile UINT32* AmbaCSL_DdrcGetWriteVref1Addr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->WriteVref1;
}
static inline volatile UINT32* AmbaCSL_DdrcGetByte0D0DlyAddr(UINT32 HostId, UINT32 Index)
{
    return &pAmbaDDRC_Reg[HostId]->Byte0Die0Dly[Index];
}
static inline volatile UINT32* AmbaCSL_DdrcGetByte1D0DlyAddr(UINT32 HostId, UINT32 Index)
{
    return &pAmbaDDRC_Reg[HostId]->Byte1Die0Dly[Index];
}
static inline volatile UINT32* AmbaCSL_DdrcGetByte2D0DlyAddr(UINT32 HostId, UINT32 Index)
{
    return &pAmbaDDRC_Reg[HostId]->Byte2Die0Dly[Index];
}
static inline volatile UINT32* AmbaCSL_DdrcGetByte3D0DlyAddr(UINT32 HostId, UINT32 Index)
{
    return &pAmbaDDRC_Reg[HostId]->Byte3Die0Dly[Index];
}
static inline volatile UINT32* AmbaCSL_DdrcGetByte0D1DlyAddr(UINT32 HostId, UINT32 Index)
{
    return &pAmbaDDRC_Reg[HostId]->Byte0Die1Dly[Index];
}
static inline volatile UINT32* AmbaCSL_DdrcGetByte1D1DlyAddr(UINT32 HostId, UINT32 Index)
{
    return &pAmbaDDRC_Reg[HostId]->Byte1Die1Dly[Index];
}
static inline volatile UINT32* AmbaCSL_DdrcGetByte2D1DlyAddr(UINT32 HostId, UINT32 Index)
{
    return &pAmbaDDRC_Reg[HostId]->Byte2Die1Dly[Index];
}
static inline volatile UINT32* AmbaCSL_DdrcGetByte3D1DlyAddr(UINT32 HostId, UINT32 Index)
{
    return &pAmbaDDRC_Reg[HostId]->Byte3Die1Dly[Index];
}
static inline volatile UINT32* AmbaCSL_DdrcGetModeRegAddr(UINT32 HostId)
{
    return &pAmbaDDRC_Reg[HostId]->ModeReg;
}
#endif

#endif  /* AMBA_INCLUDE_FROM_ASM */

#endif /* AMBA_CSL_DDRC_H */
