/**
 *  @file AmbaIOUTDiag_CmdDramShmoo.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details Dram Shmoo diagnostic command
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"

#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaWDT.h"
#include "AmbaSYS.h"

#include "AmbaIOUTDiag.h"

#include "AmbaNAND_Ctrl.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaCSL_DDRC.h"

/* The shmoo item control structure */
/* Each item is independent to shmoo task */
/* 1 : do shmoo,  else : do not do shmoo */
typedef struct {
    UINT8 pad_dds;
    UINT8 pad_pdds;
    UINT8 pad_ca_dds;
    UINT8 pad_ca_pdds;
    UINT8 pad_term;
    UINT8 pad_vref;
    UINT8 rddly;
    UINT8 wrdly;
    UINT8 dll0;
    UINT8 dll1;
    UINT8 dll2;
    UINT8 ddr4_dds;
    UINT8 ddr4_vref;
    UINT8 ddr4_odt;
    UINT8 lpddr4_pdds;
    UINT8 lpddr4_ca_vref;
    UINT8 lpddr4_dq_vref;
    UINT8 lpddr4_ca_odt;
    UINT8 lpddr4_dq_odt;
} DIAG_DRAM_SHMOO_CTRL_s;

typedef void (*AMBA_DRAM_SHMOO_CHECK_f)(UINT32 * pCheckData);
typedef void (*AMBA_DRAM_SHMOO_STOP_f)(void);
typedef struct {
    AMBA_DRAM_SHMOO_CHECK_f ShmooCheckCb;
    AMBA_DRAM_SHMOO_STOP_f SmooStopCb;
} DIAG_DRAM_SHMOO_CALLBACK_s;

typedef struct {
    UINT32 Duration;        /* The duration to check application result */
    UINT32 StoreDevice;     /* The storage device type to store shmoo result, 1:NAND, 2:SpiNOR, 3:eMMC */
    UINT32 CoreMask;        /* The core mask to shmoo task */
    UINT32 priority;        /* The shmoo task's priority */
} DIAG_DRAM_SHMOO_MISC_s;

void AmbaIOUTDiag_CmdDramShmoo( DIAG_DRAM_SHMOO_CALLBACK_s *pCallBacks,
                                DIAG_DRAM_SHMOO_CTRL_s *pShmooCtrl,
                                DIAG_DRAM_SHMOO_MISC_s *pMiscSetting );

static DIAG_DRAM_SHMOO_CTRL_s ShmooSetting;
static DIAG_DRAM_SHMOO_CALLBACK_s CallBack;
static DIAG_DRAM_SHMOO_MISC_s Misc;

extern AMBA_USER_PARTITION_TABLE_s *pAmbaNandTblUserPart;

#define BUFFER_SIZE 2048U
#define SHMOO_FLAG 0U
#define SHMOO_STATE 1U
static UINT32 ShmooDataBuffer[BUFFER_SIZE/4U] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT8 BstBuffer[8 * 1024] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT8 TrainDataBuffer[4 * 1024] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static UINT32 WriteShmooData(UINT8 *pDataBuf)
{
    AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetDevInfo();
    UINT32 BstPageCount =  0U;
    UINT32 RetVal = OK;
    BstPageCount = GetRoundUpValU32(AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE, pDev->MainByteSize);

    /* 1. Read BST data (block 0, page 0) */
    if (OK != AmbaNAND_Read(0, BstPageCount, BstBuffer, NULL, 1000000U)) {
        AmbaPrint_PrintStr5("Nand Read BST Page0 fail\r\n", NULL, NULL, NULL, NULL, NULL);
    } else {
        if (OK != AmbaNAND_Read(63, 1, TrainDataBuffer, NULL, 1000000U)) {
            AmbaPrint_PrintStr5("Nand Read BST Page63 fail\r\n", NULL, NULL, NULL, NULL, NULL);
        } else {
            /* 2. Erase the block 0 */
            if (AmbaNAND_EraseBlock(0, 1000000U) != OK) {
                AmbaPrint_PrintStr5("Nand Erase Block0 fail\r\n", NULL, NULL, NULL, NULL, NULL);
            } else {
                /* 3. Write back BST data (block 0, page 0) */
                if (AmbaNAND_Program(0, BstPageCount, BstBuffer, NULL, 1000000U) != OK) {
                    AmbaPrint_PrintStr5("Nand Write back BST data fail\r\n", NULL, NULL, NULL, NULL, NULL);
                } else {
                    /* 4. Write back sys and user partition */
                    RetVal = AmbaNVM_WriteSysPartitionTable(AMBA_NVM_NAND, NULL, 1000000U);
                    AmbaNVM_WriteUserPartitionTable(AMBA_NVM_NAND, NULL, pAmbaNandTblUserPart->PTBNumber, 1000000U);

                    /* 5. Write training resulet to page63 (block 0, page 63) */
                    if (OK != AmbaNAND_Program(62, 1, (UINT8 *) pDataBuf, NULL, 1000000U)) {
                        AmbaPrint_PrintStr5("Nand Write Page62 fail\r\n", NULL, NULL, NULL, NULL, NULL);
                    } else {
                        if (OK != AmbaNAND_Program(63, 1, (UINT8 *) TrainDataBuffer, NULL, 1000000U)) {
                            AmbaPrint_PrintStr5("Nand Write Page63 fail\r\n", NULL, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
        }
    }

    return RetVal;
}

static UINT32 LoadShmooData(UINT8 *pDataBuf)
{
    UINT32 RetVal = OK;
    RetVal = OK;
    if (OK != AmbaNAND_Read(62, 1, pDataBuf, NULL, 1000000U)) {
        AmbaPrint_PrintStr5("Nand Read Page62 fail\r\n", NULL, NULL, NULL, NULL, NULL);
    }
    return RetVal;
}

/*
 * shmoo state format :
 * [0:3]: dir, 0: +, 1: -
 * [4:11]: item
 * [12]: 0: read, 1: write
 */
#define SHMOO_START             0x4F4D4853U  /* Amba */

#define SHMOO_DIR_MASK          0x1U
#define SHMOO_ITEM_MASK         0xF0U
#define SHMOO_ITEM_OFFSET       0x4U
#define READ_NUM                0x5
#define WRITE_NUM               0xb
#define SHMOO_READ              0x0000
#define SHMOO_PAD_TERM          0x0000
#define SHMOO_PAD_VREF          0x0010
#define SHMOO_RDDLY             0x0020
#define SHMOO_DLL0              0x0030
#define SHMOO_LPDDR4_PDDS       0x0040
#define SHMOO_READ_DONE         0x0ff0
#define SHMOO_WRITE             0x1000
#define SHMOO_PAD_DDS           0x1000
#define SHMOO_PAD_PDDS          0x1010
#define SHMOO_PAD_CA_DDS        0x1020
#define SHMOO_PAD_CA_PDDS       0x1030
#define SHMOO_WRDLY             0x1040
#define SHMOO_DLL2              0x1050
#define SHMOO_LPDDR4_CA_VREF    0x1060
#define SHMOO_LPDDR4_DQ_VREF    0x1070
#define SHMOO_LPDDR4_CA_ODT     0x1080
#define SHMOO_LPDDR4_DQ_ODT     0x1090
#define SHMOO_DLL1              0x10a0
#define SHMOO_DONE              0x2000

static UINT32 ReadItemCheck(UINT32 ItemId)
{
    UINT32 RetVal = 0U;

    switch (ItemId << SHMOO_ITEM_OFFSET) {
    case SHMOO_PAD_TERM:
        RetVal = (ShmooSetting.pad_term) ? 1U : 0U;
        break;
    case SHMOO_PAD_VREF:
        RetVal = (ShmooSetting.pad_vref) ? 1U : 0U;
        break;
    case SHMOO_RDDLY:
        RetVal = (ShmooSetting.rddly) ? 1U : 0U;
        break;
    case SHMOO_DLL0:
        RetVal = (ShmooSetting.dll0) ? 1U : 0U;
        break;
    case SHMOO_LPDDR4_PDDS:
        RetVal = (ShmooSetting.lpddr4_pdds) ? 1U : 0U;
        break;
    default:
        break;
    }

    return RetVal;
}

static UINT32 WriteItemCheck(UINT32 ItemId)
{
    UINT32 RetVal = 0U;

    switch (SHMOO_WRITE | (ItemId << SHMOO_ITEM_OFFSET)) {
    case SHMOO_PAD_DDS:
        RetVal = (ShmooSetting.pad_dds) ? 1U : 0U;
        break;
    case SHMOO_PAD_PDDS:
        RetVal = (ShmooSetting.pad_pdds) ? 1U : 0U;
        break;
    case SHMOO_PAD_CA_DDS:
        RetVal = (ShmooSetting.pad_ca_dds) ? 1U : 0U;
        break;
    case SHMOO_PAD_CA_PDDS:
        RetVal = (ShmooSetting.pad_ca_pdds) ? 1U : 0U;
        break;
    case SHMOO_WRDLY:
        RetVal = (ShmooSetting.wrdly) ? 1U : 0U;
        break;
    case SHMOO_DLL2:
        RetVal = (ShmooSetting.dll2) ? 1U : 0U;
        break;
    case SHMOO_LPDDR4_CA_VREF:
        RetVal = (ShmooSetting.lpddr4_ca_vref) ? 1U : 0U;
        break;
    case SHMOO_LPDDR4_DQ_VREF:
        RetVal = (ShmooSetting.lpddr4_dq_vref) ? 1U : 0U;
        break;
    case SHMOO_LPDDR4_CA_ODT:
        RetVal = (ShmooSetting.lpddr4_ca_odt) ? 1U : 0U;
        break;
    case SHMOO_LPDDR4_DQ_ODT:
        RetVal = (ShmooSetting.lpddr4_dq_odt) ? 1U : 0U;
        break;
    case SHMOO_DLL1:
        RetVal = (ShmooSetting.dll1) ? 1U : 0U;
        break;
    default:
        break;
    }
    return RetVal;
}

/* Definitions of DDRC DLL byte */
#define AMBA_DDRC_DLL_BYTE_0        0U      /* DLL_Read */
#define AMBA_DDRC_DLL_BYTE_1        1U      /* DLL_Sync */
#define AMBA_DDRC_DLL_BYTE_2        2U      /* DLL_Write */

static const UINT8 DiagDdrPhaseSetting[64] = {  /* for DLL write/sync/read phase shift  */
    0x3F, 0x3E, 0x3D, 0x3C, 0x3B, 0x3A, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
    0x2F, 0x2E, 0x2D, 0x2C, 0x2B, 0x2A, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};

static void IoDiag_PrintFormattedRegValPerChan(const char *pFmtString, UINT32 RegValue, UINT32 ChanValue)
{
    char StrBuf[64];
    UINT32 ArgUINT32[4];

    ArgUINT32[0] = RegValue;
    ArgUINT32[1] = ChanValue;

    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 2U, ArgUINT32);
    AmbaPrint_PrintStr5("%s", StrBuf, NULL, NULL, NULL, NULL);
}

static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value)
{
    char StrBuf[64];
    UINT32 ArgUINT32[3];

    ArgUINT32[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    AmbaPrint_PrintStr5("%s", StrBuf, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
}

static UINT32 IoDiag_DdrGetDllValIndex(UINT32 Param)
{
    UINT32 i, RetVal = ~OK;

    for (i = (UINT32)sizeof(DiagDdrPhaseSetting); i > 0U; i--) {
        if (Param == (UINT32)DiagDdrPhaseSetting[i - 1U]) {
            RetVal = (i - 1U);
            break;
        }
    }

    return RetVal;
}

static void IoDiag_DdrSetDLL( UINT32 DllByte, UINT32 DllValue)
{
    UINT32 DllRegVal = AmbaCSL_DdrcGetDll0Setting(0U);

    switch (DllByte) {
    case AMBA_DDRC_DLL_BYTE_0:
        DllRegVal &= 0xffff00UL;
        DllRegVal |= (DllValue & 0xffUL);
        break;

    case AMBA_DDRC_DLL_BYTE_1:
        DllRegVal &= 0xff00ffUL;
        DllRegVal |= (DllValue & 0xffUL) << 8UL;
        break;

    case AMBA_DDRC_DLL_BYTE_2:
        DllRegVal &= 0x00ffffUL;
        DllRegVal |= (DllValue & 0xffUL) << 16UL;
        break;

    default:
        DllRegVal &= 0xffffffUL;
        break;
    }

    AmbaCSL_DdrcSetDll0Setting(0U, DllRegVal);
    AmbaCSL_DdrcSetDll1Setting(0U, DllRegVal);
    AmbaCSL_DdrcSetDll2Setting(0U, DllRegVal);
    AmbaCSL_DdrcSetDll3Setting(0U, DllRegVal);
}

static UINT32 DoShmooWorkDLL(UINT32 DllByte, UINT32 Decrease)
{
#define RANGE_DLL_INDEX_MIN     0
#define RANGE_DLL_INDEX_MAX     ((UINT32)sizeof(DiagDdrPhaseSetting) - 1)
    UINT32 DllRegVal = AmbaCSL_DdrcGetDll0Setting(0U);
    UINT32 DllValue = 0xffffffffUL;
    UINT32 Index, Done = 0U;
    UINT32 BufferInd = 0U;

    switch (DllByte) {
    case AMBA_DDRC_DLL_BYTE_0:
        DllValue = (DllRegVal & 0x0000ffUL);
        break;
    case AMBA_DDRC_DLL_BYTE_1:
        DllValue = (DllRegVal & 0x00ff00UL) >> 8UL;
        break;
    case AMBA_DDRC_DLL_BYTE_2:
        DllValue = (DllRegVal & 0xff0000UL) >> 16UL;
        break;
    default:
        // Error handler return 0?
        return 0;
    }

    Index = IoDiag_DdrGetDllValIndex(DllValue);
    if (Index == 0xFFFFFFFFUL) {
        Index = 0;  // Set to reasonable range if the return DLL is out of our scan range
    }

    /* Check range */
    if (((Index == RANGE_DLL_INDEX_MAX) && (Decrease == 0U)) ||
        ((Index == RANGE_DLL_INDEX_MIN) && (Decrease == 1U))) {
        /* Next step value is outside boundary */
        Done = 1;
    } else {
        /* Move to next step value */
        if (Decrease == 0U) {
            Index = Index + 1;
        } else {
            Index = Index - 1;
        }
        IoDiag_DdrSetDLL(DllByte, DiagDdrPhaseSetting[Index]);
        BufferInd = ((SHMOO_DLL0>>4U) + 2U + Decrease) << 2U;
        ShmooDataBuffer[BufferInd] = AmbaCSL_DdrcGetDll0Setting(0U);
        IoDiag_PrintFormattedInt("[DDRC]DLL = 0x%06X\n", AmbaCSL_DdrcGetDll0Setting(0U));
    }
    return Done;
}

static UINT32 DoShmooWorkPadTerm(UINT32 Decrease)
{
#define RANGE_PADTERM_MIN     0
#define RANGE_PADTERM_MAX     7
    UINT32 CurRegValue = 0, NewRegValue = 0, NewRegValue3 = 0, PadTerm = 0, PadTerm3 = 0;
    UINT32 i, Done = 0U;

    PadTerm = AmbaCSL_DdrcGetPadTerm(0U);
    if ((PadTerm & 0x1U) == 0U) {
        CurRegValue = (PadTerm >> 5U) & 0x7U; // bit[0] == 0, term = bit[7:5]
    } else {
        CurRegValue = 0;    // bit[0] == 1 (no term)
    }
    PadTerm3 = AmbaCSL_DdrcGetPadTerm3(0U);
    //IoDiag_PrintFormattedInt("pad: %x\n", PadTerm, TempPrintFunc);
    //IoDiag_PrintFormattedInt("pad3: %x\n", PadTerm3, TempPrintFunc);

    /* Check range */
    if (((CurRegValue == RANGE_PADTERM_MAX) && (Decrease == 0U)) ||
        ((CurRegValue == RANGE_PADTERM_MIN) && (Decrease == 1U))) {
        /* Next step value is outside boundary */
        Done = 1;
    } else {
        /* Move to next step value */
        if (Decrease == 0U) {
            i = CurRegValue + 1;
        } else {
            i = CurRegValue - 1;
        }
        NewRegValue = PadTerm & ~((0x7U << 5U) | (0x1U));           // Clear bit[7:5], bit[1] for DQ[31:16]
        NewRegValue3 = PadTerm3 & ~((0x7U << 17U) | (0x1U << 15U)); // Clear bit[19:17], bit[15] for DQ[15:0]
        if (i == 0U) {
            NewRegValue = NewRegValue | (0x1U);
            NewRegValue3 = NewRegValue3 | (0x1U << 15U);
        } else {
            NewRegValue = NewRegValue | (i << 5U);
            NewRegValue3 = NewRegValue3 | (i << 17U);
        }
        AmbaCSL_DdrcSetPadTerm(0U, NewRegValue);
        AmbaCSL_DdrcSetPadTerm3(0U, NewRegValue3);
        NewRegValue = (AmbaCSL_DdrcGetPadTerm(0U) >> 5U) & 0x7U;
        NewRegValue3 = (AmbaCSL_DdrcGetPadTerm3(0U) >> 17U) & 0x7U;
        IoDiag_PrintFormattedRegValPerChan("[DDRC][DQ ODT]PadTerm = 0x%08x (value = 0x%x)\n", AmbaCSL_DdrcGetPadTerm(0U), NewRegValue);
        IoDiag_PrintFormattedRegValPerChan("[DDRC][DQ ODT]PadTerm3 = 0x%08x (value = 0x%x)\n", AmbaCSL_DdrcGetPadTerm3(0U), NewRegValue3);
    }
    return Done;
}

static UINT32 DoShmooWorkPadDds(UINT32 Decrease)
{
#define RANGE_PADDDS_MIN     0
#define RANGE_PADDDS_MAX     7
    UINT32 CurRegValue = 0, NewRegValue = 0, NewRegValue3 = 0, PadTerm = 0, PadTerm3 = 0;
    UINT32 i, Done = 0U;

    PadTerm = AmbaCSL_DdrcGetPadTerm(0U);
    CurRegValue = (PadTerm >> 2U) & 0x7U; // dds = bit[4:2]
    PadTerm3 = AmbaCSL_DdrcGetPadTerm3(0U);

    /* Check range */
    if (((CurRegValue == RANGE_PADDDS_MAX) && (Decrease == 0U)) ||
        ((CurRegValue == RANGE_PADDDS_MIN) && (Decrease == 1U))) {
        /* Next step value is outside boundary */
        Done = 1;
    } else {
        /* Move to next step value */
        if (Decrease == 0U) {
            i = CurRegValue + 1;
        } else {
            i = CurRegValue - 1;
        }
        NewRegValue = PadTerm & ~(0x7U << 2U);          // Clear bit[4:2] for DQ[31:16]
        NewRegValue = NewRegValue | (i << 2U);
        NewRegValue3 = PadTerm3 & ~(0x7U << 12U);       // Clear bit[14:12], for DQ[15:0]
        NewRegValue3 = NewRegValue3 | (i << 12U);
        AmbaCSL_DdrcSetPadTerm(0U, NewRegValue);
        AmbaCSL_DdrcSetPadTerm3(0U, NewRegValue3);
        NewRegValue = (AmbaCSL_DdrcGetPadTerm(0U) >> 2U) & 0x7U;
        NewRegValue3 = (AmbaCSL_DdrcGetPadTerm3(0U) >> 12U) & 0x7U;
        IoDiag_PrintFormattedRegValPerChan("[DDRC][DQ DS]PadTerm = 0x%08x (value = 0x%x)\n", AmbaCSL_DdrcGetPadTerm(0U), NewRegValue);
        IoDiag_PrintFormattedRegValPerChan("[DDRC][DQ DS]PadTerm3 = 0x%08x (value = 0x%x)\n", AmbaCSL_DdrcGetPadTerm3(0U), NewRegValue3);
    }
    return Done;
}

static UINT32 DoShmooWorkLp4Vref(UINT32 Decrease)
{
#define RANGE_LP4VREF_MIN     0
#define RANGE_LP4VREF_MAX     63
    UINT32 CurRegValue = 0, NewRegValue = 0;
    UINT32 i, Done = 0U;

    CurRegValue = AmbaCSL_DdrcGetDqVref(0U) & 0x3fU; /* bit[6] is reserved */

    /* Check range */
    if (((CurRegValue == RANGE_LP4VREF_MAX) && (Decrease == 0U)) ||
        ((CurRegValue == RANGE_LP4VREF_MIN) && (Decrease == 1U))) {
        /* Next step value is outside boundary */
        Done = 1;
    } else {
        /* Move to next step value */
        if (Decrease == 0U) {
            i = CurRegValue + 1;
        } else {
            i = CurRegValue - 1;
        }
        NewRegValue = (i << 21U) | (i << 14U) | (i << 7U) | (i);
        AmbaCSL_DdrcSetDqVref(0U, NewRegValue);
        NewRegValue = AmbaCSL_DdrcGetDqVref(0U) & 0x3FU;
        IoDiag_PrintFormattedRegValPerChan("[DDRC][DQ Vref]TrainDqVref = 0x%08x (value = 0x%x)\n", AmbaCSL_DdrcGetDqVref(0U), NewRegValue);
    }
    return Done;
}

static UINT32 DoShmooWorkReadDly(UINT32 Decrease)
{
#define RANGE_READDELAY_MIN     0
#define RANGE_READDELAY_MAX     15
    UINT32 CurRegValue = 0, NewRegValue = 0;
    UINT32 i, Done = 0U;

    CurRegValue = (AmbaCSL_DdrcGetDqReadDly(0U) & 0x1EU) >> 1;

    /* Check range */
    if (((CurRegValue == RANGE_READDELAY_MAX) && (Decrease == 0U)) ||
        ((CurRegValue == RANGE_READDELAY_MIN) && (Decrease == 1U))) {
        /* Next step value is outside boundary */
        Done = 1;
    } else {
        /* Move to next step value */
        if (Decrease == 0U) {
            i = CurRegValue + 1;
        } else {
            i = CurRegValue - 1;
        }
        NewRegValue = 0x08421UL;
        NewRegValue = NewRegValue | (i << 16U) | (i << 11U) | (i << 6U) | (i << 1U);
        AmbaCSL_DdrcSetDqReadDly(0U, NewRegValue);
        NewRegValue = (AmbaCSL_DdrcGetDqReadDly(0U) & 0x1EU) >> 1;
        IoDiag_PrintFormattedRegValPerChan("[DDRC][Read Delay]TrainDqReadDly = 0x%08x (value = 0x%x)\n", AmbaCSL_DdrcGetDqReadDly(0U), NewRegValue);
    }
    return Done;
}

static UINT32 DoShmooWorkWriteDly(UINT32 Decrease)
{
#define RANGE_WRITEDELAY_MIN     0
#define RANGE_WRITEDELAY_MAX     127
    UINT32 CurRegValue = 0, NewRegValue = 0;
    UINT32 i, Done = 0U;

    CurRegValue = AmbaCSL_DdrcGetDqWriteDly(0U) & 0x7fU;

    /* Check range */
    if (((CurRegValue == RANGE_WRITEDELAY_MAX) && (Decrease == 0U)) ||
        ((CurRegValue == RANGE_WRITEDELAY_MIN) && (Decrease == 1U))) {
        /* Next step value is outside boundary */
        Done = 1;
    } else {
        /* Move to next step value */
        if (Decrease == 0U) {
            i = CurRegValue + 1;
        } else {
            i = CurRegValue - 1;
        }
        NewRegValue = 0xC0000000UL;
        NewRegValue = NewRegValue | (i << 21U) | (i << 14U) | (i << 7U) | (i);
        AmbaCSL_DdrcSetDqWriteDly(0U, NewRegValue);
        IoDiag_PrintFormattedRegValPerChan("[DDRC][Write Delay]TrainWriteDly = 0x%08x (value = 0x%x)\n", AmbaCSL_DdrcGetDqWriteDly(0U), (AmbaCSL_DdrcGetDqWriteDly(0U) & 0x7FU));
    }
    return Done;
}

static UINT32 DoShmooWorkLp4Mr3(UINT32 Decrease)
{
#define RANGE_LP4MR3_MIN     1
#define RANGE_LP4MR3_MAX     6
    UINT32 CurRegValue = 0, NewRegValue = 0, OriRegValue = 0;
    UINT32 i, Done = 0U;

    AmbaCSL_DdrcSetModeReg(0U, 0x00030000);
    while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
        ;
    }
    OriRegValue = AmbaCSL_DdrcGetModeReg(0U) & 0xFFFFU;
    CurRegValue = (OriRegValue >> 3U) & 0x7U; /* bit[5:3] */

    /* Check range */
    if (((CurRegValue == RANGE_LP4MR3_MAX) && (Decrease == 0U)) ||
        ((CurRegValue == RANGE_LP4MR3_MIN) && (Decrease == 1U))) {
        /* Next step value is outside boundary */
        Done = 1;
    } else {
        /* Move to next step value */
        if (Decrease == 0U) {
            i = CurRegValue + 1;
        } else {
            i = CurRegValue - 1;
        }
        NewRegValue = OriRegValue & ~(0x7U << 3U);      // Clear bit[5:3] for PDDS
        NewRegValue = 0x01030000UL | NewRegValue | (i << 3U);
        AmbaCSL_DdrcSetModeReg(0U, NewRegValue);
        (void)AmbaKAL_TaskSleep(100);
        AmbaCSL_DdrcSetModeReg(0U, 0x00030000);
        while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
            ;
        }
        IoDiag_PrintFormattedRegValPerChan("[LPDDR4][MR3 PDDS]ModeReg = 0x%08x (value = 0x%x)\n", AmbaCSL_DdrcGetModeReg(0U), ((AmbaCSL_DdrcGetModeReg(0U) >> 3U) & 0x7U));
    }
    return Done;
}

static UINT32 DoShmooWorkLp4Mr11Ca(UINT32 Decrease)
{
#define RANGE_LP4MR11CA_MIN     0
#define RANGE_LP4MR11CA_MAX     6
    UINT32 CurRegValue = 0, NewRegValue = 0, OriRegValue = 0;
    UINT32 i, Done = 0U;

    AmbaCSL_DdrcSetModeReg(0U, 0x000b0000);
    while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
        ;
    }
    OriRegValue = AmbaCSL_DdrcGetModeReg(0U) & 0xFFFFU;
    CurRegValue = (OriRegValue >> 4U) & 0x7U; /* bit[6:4] */

    /* Check range */
    if (((CurRegValue == RANGE_LP4MR11CA_MAX) && (Decrease == 0U)) ||
        ((CurRegValue == RANGE_LP4MR11CA_MIN) && (Decrease == 1U))) {
        /* Next step value is outside boundary */
        Done = 1;
    } else {
        /* Move to next step value */
        if (Decrease == 0U) {
            i = CurRegValue + 1;
        } else {
            i = CurRegValue - 1;
        }
        NewRegValue = OriRegValue & ~(0x7U << 4U);      // Clear bit[6:4] for CA ODT
        NewRegValue = 0x010b0000UL | NewRegValue | (i << 4U);
        AmbaCSL_DdrcSetModeReg(0U, NewRegValue);
        (void)AmbaKAL_TaskSleep(100);
        AmbaCSL_DdrcSetModeReg(0U, 0x000b0000);
        while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
            ;
        }
        IoDiag_PrintFormattedRegValPerChan("[LPDDR4][MR11 CA]ModeReg = 0x%08x (value = 0x%x)\n", AmbaCSL_DdrcGetModeReg(0U), ((AmbaCSL_DdrcGetModeReg(0U) >> 4U) & 0x7U));
    }
    return Done;
}

static UINT32 DoShmooWorkLp4Mr11Dq(UINT32 Decrease)
{
#define RANGE_LP4MR11DQ_MIN     0
#define RANGE_LP4MR11DQ_MAX     6
    UINT32 CurRegValue = 0, NewRegValue = 0, OriRegValue = 0;
    UINT32 i, Done = 0U;

    AmbaCSL_DdrcSetModeReg(0U, 0x000b0000);
    while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
        ;
    }
    OriRegValue = AmbaCSL_DdrcGetModeReg(0U) & 0xFFFFU;
    CurRegValue = OriRegValue & 0x7U; /* bit[2:0] */

    /* Check range */
    if (((CurRegValue == RANGE_LP4MR11DQ_MAX) && (Decrease == 0U)) ||
        ((CurRegValue == RANGE_LP4MR11DQ_MIN) && (Decrease == 1U))) {
        /* Next step value is outside boundary */
        Done = 1;
    } else {
        /* Move to next step value */
        if (Decrease == 0U) {
            i = CurRegValue + 1;
        } else {
            i = CurRegValue - 1;
        }
        NewRegValue = OriRegValue & ~(0x7U);      // Clear bit[2:0] for DQ ODT
        NewRegValue = 0x010b0000UL | NewRegValue | i;
        AmbaCSL_DdrcSetModeReg(0U, NewRegValue);
        (void)AmbaKAL_TaskSleep(100);
        AmbaCSL_DdrcSetModeReg(0U, 0x000b0000);
        while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
            ;
        }
        IoDiag_PrintFormattedRegValPerChan("[LPDDR4][MR11 DQ]ModeReg = 0x%08x (value = 0x%x)\n", AmbaCSL_DdrcGetModeReg(0U), (AmbaCSL_DdrcGetModeReg(0U) & 0x7U));
    }
    return Done;
}

static UINT32 DoShmooWorkLp4Mr12(UINT32 Decrease)
{
#define RANGE_LP4MR12_MIN     0
#define RANGE_LP4MR12_MAX     50
    UINT32 CurRegValue = 0, NewRegValue = 0;
    UINT32 i, Done = 0U;

    AmbaCSL_DdrcSetModeReg(0U, 0x000c0000);
    while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
        ;
    }
    CurRegValue = AmbaCSL_DdrcGetModeReg(0U) & 0x3fU; /* bit[5:0] */

    /* Check range */
    if (((CurRegValue == RANGE_LP4MR12_MAX) && (Decrease == 0U)) ||
        ((CurRegValue == RANGE_LP4MR12_MIN) && (Decrease == 1U))) {
        /* Next step value is outside boundary */
        Done = 1;
    } else {
        /* Move to next step value */
        if (Decrease == 0U) {
            i = CurRegValue + 1;
        } else {
            i = CurRegValue - 1;
        }
        NewRegValue = 0x010c0000UL | i;
        AmbaCSL_DdrcSetModeReg(0U, NewRegValue);
        (void)AmbaKAL_TaskSleep(100);
        AmbaCSL_DdrcSetModeReg(0U, 0x000c0000);
        while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
            ;
        }
        IoDiag_PrintFormattedRegValPerChan("[LPDDR4][MR12 Vref]ModeReg = 0x%08x (value = 0x%x)\n", AmbaCSL_DdrcGetModeReg(0U), (AmbaCSL_DdrcGetModeReg(0U) & 0x3FU));
    }
    return Done;
}

static UINT32 DoShmooWorkLp4Mr14(UINT32 Decrease)
{
#define RANGE_LP4MR14_MIN     0
#define RANGE_LP4MR14_MAX     50
    UINT32 CurRegValue = 0, NewRegValue = 0;
    UINT32 i, Done = 0U;

    /* Get current value */
    AmbaCSL_DdrcSetModeReg(0U, 0x000e0000);
    while (0x0U != (AmbaCSL_DdrcGetModeReg(0) & 0x80000000U)) {
        ;
    }
    CurRegValue = AmbaCSL_DdrcGetModeReg(0) & 0x3fU; /* bit[5:0] */

    /* Check range */
    if (((CurRegValue == RANGE_LP4MR14_MAX) && (Decrease == 0U)) ||
        ((CurRegValue == RANGE_LP4MR14_MIN) && (Decrease == 1U))) {
        /* Next step value is outside boundary */
        Done = 1;
    } else {
        /* Move to next step value */
        if (Decrease == 0U) {
            i = CurRegValue + 1;
        } else {
            i = CurRegValue - 1;
        }
        NewRegValue = 0x010e0000UL | i;
        AmbaCSL_DdrcSetModeReg(0U, NewRegValue);
        (void)AmbaKAL_TaskSleep(100);
        AmbaCSL_DdrcSetModeReg(0U, 0x000e0000);
        while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
            ;
        }
        IoDiag_PrintFormattedRegValPerChan("[LPDDR4][MR14 Vref]ModeReg = 0x%08x (value = 0x%x)\n", AmbaCSL_DdrcGetModeReg(0U), (AmbaCSL_DdrcGetModeReg(0U) & 0x3FU));
    }
    return Done;
}

static UINT32 DoShmooWork(UINT32 ShmooState)
{
    /* execute shmoo items with ShmooState */
    //UINT32 Item = (ShmooState & SHMOO_ITEM_MASK) >> SHMOO_ITEM_OFFSET;
    UINT32 Dir = (ShmooState & SHMOO_DIR_MASK); /* 0 : + , 1 : - */
    UINT32 StateItem = (ShmooState & ~SHMOO_DIR_MASK);
    UINT32 RetVal = 0; /* 1: specidied item is done, 0 : specified item is not finished */
    UINT32 Done = 0;

    // Print(StateItem)
    switch (StateItem) {
    /* Read related */
    case SHMOO_PAD_TERM:
        Done = DoShmooWorkPadTerm(Dir);
        break;
    case SHMOO_PAD_VREF:
        Done = DoShmooWorkLp4Vref(Dir);
        break;
    case SHMOO_RDDLY:
        Done = DoShmooWorkReadDly(Dir);
        break;
    case SHMOO_DLL0:
        Done = DoShmooWorkDLL(AMBA_DDRC_DLL_BYTE_0, Dir);
        break;
    case SHMOO_LPDDR4_PDDS:
        Done = DoShmooWorkLp4Mr3(Dir);  // Read MR3 register has problem.
        break;
    /* Write related */
    case SHMOO_PAD_DDS:
        Done = DoShmooWorkPadDds(Dir);
        break;
    case SHMOO_PAD_PDDS:
        break;
    case SHMOO_PAD_CA_DDS:
        break;
    case SHMOO_PAD_CA_PDDS:
        break;
    case SHMOO_WRDLY:
        Done = DoShmooWorkWriteDly(Dir);
        break;
    case SHMOO_DLL2:
        Done = DoShmooWorkDLL(AMBA_DDRC_DLL_BYTE_2, Dir);
        break;
    case SHMOO_LPDDR4_CA_VREF:
        Done = DoShmooWorkLp4Mr12(Dir);
        break;
    case SHMOO_LPDDR4_DQ_VREF:
        Done = DoShmooWorkLp4Mr14(Dir);
        break;
    case SHMOO_LPDDR4_CA_ODT:
        Done = DoShmooWorkLp4Mr11Ca(Dir);
        break;
    case SHMOO_LPDDR4_DQ_ODT:
        Done = DoShmooWorkLp4Mr11Dq(Dir);
        break;
    case SHMOO_DLL1:
        Done = DoShmooWorkDLL(AMBA_DDRC_DLL_BYTE_1, Dir);
        break;

    /* Should be done */
    case SHMOO_DONE:
        Done = 1;
        break;
    default:
        IoDiag_PrintFormattedInt("Invalid state 0x%x\n", StateItem);
    }

    RetVal = Done;
    return RetVal;
}

static void GetNextState(UINT32 *pState)
{
    UINT32 StateTmp = *pState;
    UINT32 Item = 0U;
    UINT32 i = 0U;

    if ((0x0 == (StateTmp & SHMOO_DIR_MASK)) && (0x0 != StateTmp) && (SHMOO_READ_DONE != StateTmp)) {
        /* same item, change direction */
        StateTmp = StateTmp | SHMOO_DIR_MASK;
        *pState = StateTmp;
    } else {
        /* do next item */
        if (SHMOO_READ_DONE != StateTmp) {
            StateTmp = StateTmp & ~(SHMOO_DIR_MASK);
            Item = ((StateTmp & SHMOO_ITEM_MASK) >> SHMOO_ITEM_OFFSET) + 1U;
        } else {
            StateTmp = SHMOO_WRITE;
            Item = ((StateTmp & SHMOO_ITEM_MASK) >> SHMOO_ITEM_OFFSET);
        }
        if (StateTmp < SHMOO_WRITE) {
            /* do read shmoo */
            for (i = Item; i < READ_NUM; i++) {
                if (ReadItemCheck(i) == 0x1U) {
                    /* find read shmoo item */
                    break;
                }
            }
            if (i == READ_NUM) {
                *pState = SHMOO_READ_DONE;
            } else {
                Item = (i << SHMOO_ITEM_OFFSET);
                *pState = (StateTmp & ~(SHMOO_ITEM_MASK)) | Item;
            }
        } else {
            /* do write shmoo */
            for (i = Item; i < WRITE_NUM; i++) {
                if (WriteItemCheck(i) == 0x1U) {
                    /* find write shmoo item */
                    break;
                }
            }
            if (i == WRITE_NUM) {
                *pState = SHMOO_DONE;
            } else {
                Item = (i << SHMOO_ITEM_OFFSET);
                *pState = (StateTmp & ~(SHMOO_ITEM_MASK)) | Item;
            }
        }
    }
}

static void *IoDiag_ShmooTaskEntry(void * Arg)
{
    UINT32 State = 0U;
    UINT32 AppCheck = 0U;
    UINT32 ShmooWorkDone = 0U;
    UINT32 i = 0;

    (void)Arg;

    AmbaPrint_PrintStr5("All shmoo start: load data\n", NULL, NULL, NULL, NULL, NULL);

    /* read back shmoo data */
    LoadShmooData((UINT8*)ShmooDataBuffer);

    AmbaPrint_PrintStr5("All shmoo start: do shemoo\n", NULL, NULL, NULL, NULL, NULL);

    /* check current state */
    if (ShmooDataBuffer[SHMOO_FLAG] != SHMOO_START) {
        /* shmoo is just started */
        for (i = 0; i < BUFFER_SIZE/4U; i++) {
            ShmooDataBuffer[i] = 0U;
        }
        ShmooDataBuffer[SHMOO_FLAG] = SHMOO_START;
        WriteShmooData((UINT8*)ShmooDataBuffer);
    } else {
        /* get the last state */
        State = ShmooDataBuffer[SHMOO_STATE];
    }

    IoDiag_PrintFormattedInt("Last state 0x%x\n", State);
    GetNextState(&State);
    if (SHMOO_READ_DONE == State) {
        GetNextState(&State);
    }
    IoDiag_PrintFormattedInt("Current state 0x%x\n", State);
    ShmooDataBuffer[SHMOO_STATE] = State;

    if (State != SHMOO_DONE) {
        WriteShmooData((UINT8*)ShmooDataBuffer);
        (void)AmbaWDT_Start(Misc.Duration << 1U, 10U);
        while ((0x0U == ShmooWorkDone) && (0x0U == AppCheck)) {
            AmbaWDT_Feed();
            ShmooWorkDone = DoShmooWork(State);
            AmbaKAL_TaskSleep(Misc.Duration);
            CallBack.ShmooCheckCb(&AppCheck);
            if (0x0U == AppCheck) {
                AmbaWDT_Feed();
                /* App still works, store back current data */
                WriteShmooData((UINT8*)ShmooDataBuffer);
            } else {
                AmbaKAL_TaskSleep(1000000U);
            }
        }
        AmbaWDT_Stop();
        IoDiag_PrintFormattedInt("Item 0x%x done\n", State);
        AmbaSYS_Reboot();
    } else {
        /* all shmoo done, and store back current data */
        WriteShmooData((UINT8*)ShmooDataBuffer);
        IoDiag_PrintFormattedInt("All shmoo item done\n", (State & ~SHMOO_DIR_MASK));
        CallBack.SmooStopCb();
    }

    return NULL;
}

static AMBA_KAL_TASK_t IoDiagSemooTask;

/**
 *  SPI diagnostic command
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 *  @param[in] PrintFunc function to print messages on shell task
 */
void AmbaIOUTDiag_CmdDramShmoo( DIAG_DRAM_SHMOO_CALLBACK_s* pCallBacks,
                                DIAG_DRAM_SHMOO_CTRL_s* pShmooCtrl,
                                DIAG_DRAM_SHMOO_MISC_s* pMiscSetting)
{
    static UINT8 IoDiagSemooStack[0x1000];
    static char TaskName[20] = "DiagDramSemooeTask";
    UINT32 RetVal = 0U;

    CallBack.ShmooCheckCb = pCallBacks->ShmooCheckCb;
    CallBack.SmooStopCb = pCallBacks->SmooStopCb;

    ShmooSetting.pad_dds = pShmooCtrl->pad_dds;
    ShmooSetting.pad_pdds = pShmooCtrl->pad_pdds;
    ShmooSetting.pad_ca_dds = pShmooCtrl->pad_ca_dds;
    ShmooSetting.pad_ca_pdds = pShmooCtrl->pad_ca_pdds;
    ShmooSetting.pad_term = pShmooCtrl->pad_term;
    ShmooSetting.pad_vref = pShmooCtrl->pad_vref;
    ShmooSetting.rddly = pShmooCtrl->rddly;
    ShmooSetting.wrdly = pShmooCtrl->wrdly;
    ShmooSetting.dll0 = pShmooCtrl->dll0;
    ShmooSetting.dll1 = pShmooCtrl->dll1;
    ShmooSetting.dll2 = pShmooCtrl->dll2;
    ShmooSetting.ddr4_dds = pShmooCtrl->ddr4_dds;
    ShmooSetting.ddr4_vref = pShmooCtrl->ddr4_vref;
    ShmooSetting.ddr4_odt = pShmooCtrl->ddr4_odt;
    ShmooSetting.lpddr4_pdds = pShmooCtrl->lpddr4_pdds;
    ShmooSetting.lpddr4_ca_vref = pShmooCtrl->lpddr4_ca_vref;
    ShmooSetting.lpddr4_dq_vref = pShmooCtrl->lpddr4_dq_vref;
    ShmooSetting.lpddr4_ca_odt = pShmooCtrl->lpddr4_ca_odt;
    ShmooSetting.lpddr4_dq_odt = pShmooCtrl->lpddr4_dq_odt;

    if (pMiscSetting->Duration > 178956U) {
        Misc.Duration = 178955U;
    } else {
        Misc.Duration = pMiscSetting->Duration;
    }
    Misc.StoreDevice = pMiscSetting->StoreDevice;

    RetVal = AmbaKAL_TaskCreate(&IoDiagSemooTask, TaskName, pMiscSetting->priority, IoDiag_ShmooTaskEntry, NULL, IoDiagSemooStack, sizeof(IoDiagSemooStack), AMBA_KAL_DONT_START);
    if (RetVal == KAL_ERR_NONE) {
        (void)AmbaKAL_TaskSetSmpAffinity(&IoDiagSemooTask, pMiscSetting->CoreMask);
        (void)AmbaKAL_TaskResume(&IoDiagSemooTask);
        (void)AmbaKAL_TaskSleep(100U);
    }

    return;
}

void AmbaIOUTDiag_CmdStopDramShmoo(void)
{
    AmbaKAL_TaskTerminate(&IoDiagSemooTask);
    AmbaKAL_TaskDelete(&IoDiagSemooTask);
}

AMBA_DRAM_SHMOO_CHECK_f ShmooCheckCb;
AMBA_DRAM_SHMOO_STOP_f SmooStopCb;

void MyCheckFunc(UINT32 *pCheckData)
{
    static UINT32 MyCount = 0;

    MyCount++;
    if (MyCount == 12u) {
        *pCheckData = 1;
    } else {
        *pCheckData = 0;
    }
}
static UINT8 ShmooDone = 0;
void MyStopFunc(void)
{
    AmbaPrint_PrintStr5("All shmoo done\n", NULL, NULL, NULL, NULL, NULL);

    ShmooDone = 1;
}

void AmbaIOUTDiag_CmdAdvShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    DIAG_DRAM_SHMOO_CALLBACK_s CallBacks = {0U};
    DIAG_DRAM_SHMOO_MISC_s MiscSetting = {0U};
    DIAG_DRAM_SHMOO_CTRL_s ShmooCtrl = {0U};

    (void)ArgCount;
    (void)pArgVector;

    CallBacks.ShmooCheckCb = MyCheckFunc;
    CallBacks.SmooStopCb = MyStopFunc;

    MiscSetting.Duration = 1000;
    MiscSetting.StoreDevice = 1;
    MiscSetting.CoreMask = 0x2;
    MiscSetting.priority = 0xff;

    ShmooCtrl.dll0 = 1U;

    PrintFunc("Enable shmoo task\n");
    AmbaIOUTDiag_CmdDramShmoo(&CallBacks, &ShmooCtrl, &MiscSetting);

    while(0x0 == ShmooDone) {
        AmbaKAL_TaskSleep(10000U);
    }
    AmbaIOUTDiag_CmdStopDramShmoo();
}
