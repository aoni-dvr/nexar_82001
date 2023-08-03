#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaDef.h"

#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"

#include "AmbaCSL_DDRC.h"
#include "AmbaDramCommon.h"
#include "AmbaDDRC.h"

#define PRINT_BUF_SIZE 1024

void Diag_PrintFormattedRegVal(const char *pFmtString, volatile const void *pRegAddr, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    const UINT32 *pUINT32;
    UINT64 ArgUINT64[3];

    AmbaMisra_TypeCast64(&pUINT32, &pRegAddr);
    AmbaMisra_TypeCast64(&ArgUINT64[0], &pUINT32);
    ArgUINT64[1] = (UINT64) *pUINT32;
    (void)AmbaUtility_StringPrintUInt64(StrBuf, sizeof(StrBuf), pFmtString, 2U, ArgUINT64);
    PrintFunc(StrBuf);
}

void Diag_PrintFormattedRegValPerChan(const char *pFmtString, UINT32 DdrcId, UINT32 RegValue, UINT32 ChanValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[128];
    UINT32 ArgUINT32[5];

    ArgUINT32[0] = DdrcId;
    ArgUINT32[1] = RegValue;
    ArgUINT32[2] = ChanValue;

    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 3U, ArgUINT32);
    PrintFunc(StrBuf);
}

void Diag_PrintSingleU32Str(const char *pFmt, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    static char StrBuf[PRINT_BUF_SIZE];
    UINT32 args[1];

    args[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmt, 1U, args);
    PrintFunc(StrBuf);
}

void Diag_PrintSingleU64Str(const char *pFmt, UINT64 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    static char StrBuf[PRINT_BUF_SIZE];
    UINT64 args[1];

    args[0] = Value;
    (void)AmbaUtility_StringPrintUInt64(StrBuf, sizeof(StrBuf), pFmt, 1U, args);
    PrintFunc(StrBuf);
}

void Diag_PrintU32Str(const char *pFmt, UINT32 arg_count, UINT32* pArgs, AMBA_SHELL_PRINT_f PrintFunc)
{
    static char StrBuf[PRINT_BUF_SIZE];
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmt, arg_count, pArgs);
    PrintFunc(StrBuf);
}

void Diag_PrintU64Str(const char *pFmt, UINT32 arg_count, UINT64* pArgs, AMBA_SHELL_PRINT_f PrintFunc)
{
    static char StrBuf[PRINT_BUF_SIZE];
    (void)AmbaUtility_StringPrintUInt64(StrBuf, sizeof(StrBuf), pFmt, arg_count, pArgs);
    PrintFunc(StrBuf);
}

void Diag_PrintMemory(UINT64 AddrStart, UINT64 AddrEnd, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32* Address = (UINT32*)AddrStart;
    UINT32 i = 0U;
    UINT32 args[5];

    for (i = 0; AddrStart < AddrEnd; i += 4) {
        args[0] = (i * 4);
        args[1] = Address[i];
        args[2] = Address[i+1];
        args[3] = Address[i+2];
        args[4] = Address[i+3];
        Diag_PrintU32Str("0x%08X: 0x%08X 0x%08X 0x%08X 0x%08X\r\n", 5, args, PrintFunc);
        AddrStart += 16U;
    }
}

UINT32 Diag_DdrcGetModeReg(UINT32 DdrcId, UINT32 BitMask)
{
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    if (BitMask & 0xffffU) {
        return AmbaDDRC_GetModeReg(DdrcId) & 0xffU;
    } else if (BitMask & 0xffff0000U) {
        return (AmbaDDRC_GetModeReg(DdrcId) >> 8) & 0xffU;
    }
#else
    if (BitMask & 0xffffU) {
        return AmbaCSL_DdrcGetModeReg(DdrcId) & 0xffU;
    } else if (BitMask & 0xffff0000U) {
        return (AmbaCSL_DdrcGetModeReg(DdrcId) >> 8) & 0xffU;
    }
#endif

    return 0;
}

void Diag_DdrcSetModeRegWait(UINT32 DdrcId, UINT32 BitMask, UINT32 Val)
{
    UINT32  Value = 0;

    Val |= 0x18000000U; // Set bit [27~28] to be default both channel
    if (BitMask & 0xffffU) {
        if ((BitMask & 0xffff0000U) == 0) {
            Val &= ~(0x2U << 27U);  // Single channel 0
        }
    } else if (BitMask & 0xffff0000U) {
        Val &= ~(0x1U << 27U);      // Single channel 1
    }

#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    AmbaDDRC_SetModeReg(DdrcId, Val);
    /* Wait busy bit gone */
    while ((0x0U != (AmbaDDRC_GetModeReg(DdrcId) & 0x80000000U)) && (Value++ < 100)) {
        (void)AmbaKAL_TaskSleep(10);
    }
#else
    AmbaCSL_DdrcSetModeReg(DdrcId, Val);
    /* Wait busy bit gone */
    while ((0x0U != (AmbaCSL_DdrcGetModeReg(DdrcId) & 0x80000000U)) && (Value++ < 100)) {
        (void)AmbaKAL_TaskSleep(10);
    }
#endif
}

UINT32 Diag_DdrcIsSingleDie(UINT32 DdrcId, AMBA_SHELL_PRINT_f PrintFunc)
{
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    UINT32 data;
    AMBA_DDRC_CONFIG0_REG_s* config0;

    data = AmbaDDRC_Get(DdrcId, DDRC_OFST_CONFIG0);
    config0 = (AMBA_DDRC_CONFIG0_REG_s*)&data;
    AmbaMisra_TouchUnused(&PrintFunc);
#if 0
    Diag_PrintSingleU32Str("[DBG]DramType          =%x\n", config0->DramType          , PrintFunc);
    Diag_PrintSingleU32Str("[DBG]DramSize          =%x\n", config0->DramSize          , PrintFunc);
    Diag_PrintSingleU32Str("[DBG]BusMode           =%x\n", config0->BusMode           , PrintFunc);
    Diag_PrintSingleU32Str("[DBG]AccuMaskMode      =%x\n", config0->AccuMaskMode      , PrintFunc);
    Diag_PrintSingleU32Str("[DBG]MaxPostedRefCredit=%x\n", config0->MaxPostedRefCredit, PrintFunc);
    Diag_PrintSingleU32Str("[DBG]MinPostedRefCredit=%x\n", config0->MinPostedRefCredit, PrintFunc);
    Diag_PrintSingleU32Str("[DBG]HighskewOffset    =%x\n", config0->HighskewOffset    , PrintFunc);
    Diag_PrintSingleU32Str("[DBG]Lp5BankGrpMode    =%x\n", config0->Lp5BankGrpMode    , PrintFunc);
    Diag_PrintSingleU32Str("[DBG]IndDqDqsReadEnable=%x\n", config0->IndDqDqsReadEnable, PrintFunc);
    Diag_PrintSingleU32Str("[DBG]WckSuspendMode    =%x\n", config0->WckSuspendMode    , PrintFunc);
    Diag_PrintSingleU32Str("[DBG]UseMsbForChbMrw   =%x\n", config0->UseMsbForChbMrw   , PrintFunc);
    Diag_PrintSingleU32Str("[DBG]DieMapMode        =%x\n", config0->DieMapMode        , PrintFunc);
    Diag_PrintSingleU32Str("[DBG]BgMapMode         =%x\n", config0->BgMapMode         , PrintFunc);
    Diag_PrintSingleU32Str("[DBG]DuelDieEn         =%x\n", config0->DuelDieEn         , PrintFunc);
    Diag_PrintSingleU32Str("[DBG]Reserved1         =%x\n", config0->Reserved1         , PrintFunc);
#endif
    if(config0->DuelDieEn == 0)
        return 1;
#else
    AmbaMisra_TouchUnused(&PrintFunc);
    if(pAmbaDDRC_Reg[DdrcId]->Config0.DuelDieEn == 0)
        return 1;
#endif
    return 0;
}

void Diag_DdrSetDLL(UINT32 DdrcId, UINT32 BitMask, UINT32 DllByte, UINT32 DllValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 DllRegVal = 0, Mask, i;

    /* Get original value */
    if (DllByte == AMBA_DDRC_DLL_BYTE_0) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        DllRegVal = AmbaDDRC_Get(DdrcId, DDRC_OFST_DLLCTRLSEL0D0);
#else
        DllRegVal = AmbaCSL_DdrcGetD0Dll0(DdrcId);
#endif
    } else if (DllByte == AMBA_DDRC_DLL_BYTE_1) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        DllRegVal = AmbaDDRC_Get(DdrcId, DDRC_OFST_DLLCTRLSEL1D0);
#else
        DllRegVal = AmbaCSL_DdrcGetD0Dll1(DdrcId);
#endif
    } else if (DllByte == AMBA_DDRC_DLL_BYTE_2) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        DllRegVal = AmbaDDRC_Get(DdrcId, DDRC_OFST_DLLCTRLSEL2D0);
#else
        DllRegVal = AmbaCSL_DdrcGetD0Dll2(DdrcId);
#endif
    }

    /* Update field accroding to BitMask */
    DllValue = DllValue & 0xffU;
    for (i = 0; i < 4; i++) {
        Mask = 0xffU << (i * 8);
        if (BitMask & Mask) {
            DllRegVal &= (~Mask);
            DllRegVal |= (DllValue << (i * 8));
        }
    }

    /* Set new value */
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    if (DllByte == AMBA_DDRC_DLL_BYTE_0) {
        AmbaDDRC_Set(DdrcId, DDRC_OFST_DLLCTRLSEL0D0, DllRegVal);
        AmbaDDRC_Set(DdrcId, DDRC_OFST_DLLCTRLSEL0D1, DllRegVal);
        DllRegVal = AmbaDDRC_Get(DdrcId, DDRC_OFST_DLLCTRLSEL0D0);
    } else if (DllByte == AMBA_DDRC_DLL_BYTE_1) {
        AmbaDDRC_Set(DdrcId, DDRC_OFST_DLLCTRLSEL1D0, DllRegVal);
        AmbaDDRC_Set(DdrcId, DDRC_OFST_DLLCTRLSEL1D1, DllRegVal);
        DllRegVal = AmbaDDRC_Get(DdrcId, DDRC_OFST_DLLCTRLSEL1D0);
    } else if (DllByte == AMBA_DDRC_DLL_BYTE_2) {
        AmbaDDRC_Set(DdrcId, DDRC_OFST_DLLCTRLSEL2D0, DllRegVal);
        AmbaDDRC_Set(DdrcId, DDRC_OFST_DLLCTRLSEL2D1, DllRegVal);
        DllRegVal = AmbaDDRC_Get(DdrcId, DDRC_OFST_DLLCTRLSEL2D0);
    }
#else
    if (DllByte == AMBA_DDRC_DLL_BYTE_0) {
        AmbaCSL_DdrcSetD0Dll0(DdrcId, DllRegVal);
        AmbaCSL_DdrcSetD1Dll0(DdrcId, DllRegVal);
        DllRegVal = AmbaCSL_DdrcGetD0Dll0(DdrcId);
    } else if (DllByte == AMBA_DDRC_DLL_BYTE_1) {
        AmbaCSL_DdrcSetD0Dll1(DdrcId, DllRegVal);
        AmbaCSL_DdrcSetD1Dll1(DdrcId, DllRegVal);
        DllRegVal = AmbaCSL_DdrcGetD0Dll1(DdrcId);
    } else if (DllByte == AMBA_DDRC_DLL_BYTE_2) {
        AmbaCSL_DdrcSetD0Dll2(DdrcId, DllRegVal);
        AmbaCSL_DdrcSetD1Dll2(DdrcId, DllRegVal);
        DllRegVal = AmbaCSL_DdrcGetD0Dll2(DdrcId);
    }
#endif

    Diag_PrintFormattedRegValPerChan("[DDRC%d][DLL%d]Reg = 0x%08x\n", DdrcId, DllByte, DllRegVal, PrintFunc);
}

UINT32 Diag_DdrcGetByteDly(UINT32 DdrcId, UINT32 DieId, UINT32 ByteNum, UINT32 Index)
{
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    if (DieId == 0U) {
        if (ByteNum == 0U) {
            return AmbaDDRC_Get(DdrcId, DDRC_OFST_BYTE0DIE0DLY(Index));
        } else if (ByteNum == 1U) {
            return AmbaDDRC_Get(DdrcId, DDRC_OFST_BYTE1DIE0DLY(Index));
        } else if (ByteNum == 2U) {
            return AmbaDDRC_Get(DdrcId, DDRC_OFST_BYTE2DIE0DLY(Index));
        } else if (ByteNum == 3U) {
            return AmbaDDRC_Get(DdrcId, DDRC_OFST_BYTE3DIE0DLY(Index));
        }
    } else {
        if (ByteNum == 0U) {
            return AmbaDDRC_Get(DdrcId, DDRC_OFST_BYTE0DIE1DLY(Index));
        } else if (ByteNum == 1U) {
            return AmbaDDRC_Get(DdrcId, DDRC_OFST_BYTE1DIE1DLY(Index));
        } else if (ByteNum == 2U) {
            return AmbaDDRC_Get(DdrcId, DDRC_OFST_BYTE2DIE1DLY(Index));
        } else if (ByteNum == 3U) {
            return AmbaDDRC_Get(DdrcId, DDRC_OFST_BYTE3DIE1DLY(Index));
        }
    }
#else
    if (DieId == 0U) {
        if (ByteNum == 0U) {
            return AmbaCSL_DdrcGetByte0D0Dly(DdrcId, Index);
        } else if (ByteNum == 1U) {
            return AmbaCSL_DdrcGetByte1D0Dly(DdrcId, Index);
        } else if (ByteNum == 2U) {
            return AmbaCSL_DdrcGetByte2D0Dly(DdrcId, Index);
        } else if (ByteNum == 3U) {
            return AmbaCSL_DdrcGetByte3D0Dly(DdrcId, Index);
        }
    } else {
        if (ByteNum == 0U) {
            return AmbaCSL_DdrcGetByte0D1Dly(DdrcId, Index);
        } else if (ByteNum == 1U) {
            return AmbaCSL_DdrcGetByte1D1Dly(DdrcId, Index);
        } else if (ByteNum == 2U) {
            return AmbaCSL_DdrcGetByte2D1Dly(DdrcId, Index);
        } else if (ByteNum == 3U) {
            return AmbaCSL_DdrcGetByte3D1Dly(DdrcId, Index);
        }
    }
#endif
    return 0;
}

UINT32 Diag_DdrcGetDqVref(UINT32 DdrcId, UINT32 ByteNum)
{
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    if (ByteNum == 0U) {
        return (AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF0) >> DQ_VREF_POS1) & DQ_VREF_MASK;
    } else if (ByteNum == 1U) {
        return (AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF0) >> DQ_VREF_POS2) & DQ_VREF_MASK;
    } else if (ByteNum == 2U) {
        return (AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF1) >> DQ_VREF_POS1) & DQ_VREF_MASK;
    } else if (ByteNum == 3U) {
        return (AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF1) >> DQ_VREF_POS2) & DQ_VREF_MASK;
    }
#else
    if (ByteNum == 0U) {
        return (AmbaCSL_DdrcGetRdVref0(DdrcId) >> DQ_VREF_POS1) & DQ_VREF_MASK;
    } else if (ByteNum == 1U) {
        return (AmbaCSL_DdrcGetRdVref0(DdrcId) >> DQ_VREF_POS2) & DQ_VREF_MASK;
    } else if (ByteNum == 2U) {
        return (AmbaCSL_DdrcGetRdVref1(DdrcId) >> DQ_VREF_POS1) & DQ_VREF_MASK;
    } else if (ByteNum == 3U) {
        return (AmbaCSL_DdrcGetRdVref1(DdrcId) >> DQ_VREF_POS2) & DQ_VREF_MASK;
    }
#endif
    return 0;
}

UINT32 Diag_DdrcGetDqsVrefPerByte(UINT32 DdrcId, UINT32 ByteNum)
{
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    if (ByteNum == 0U) {
        return (AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF0) >> DQS_VREF_POS1) & DQS_VREF_MASK;
    } else if (ByteNum == 1U) {
        return (AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF0) >> DQS_VREF_POS2) & DQS_VREF_MASK;
    } else if (ByteNum == 2U) {
        return (AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF1) >> DQS_VREF_POS1) & DQS_VREF_MASK;
    } else if (ByteNum == 3U) {
        return (AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF1) >> DQS_VREF_POS2) & DQS_VREF_MASK;
    }
    return 0;
#else
    if (ByteNum == 0U) {
        return (AmbaCSL_DdrcGetRdVref0(DdrcId) >> DQS_VREF_POS1) & DQS_VREF_MASK;
    } else if (ByteNum == 1U) {
        return (AmbaCSL_DdrcGetRdVref0(DdrcId) >> DQS_VREF_POS2) & DQS_VREF_MASK;
    } else if (ByteNum == 2U) {
        return (AmbaCSL_DdrcGetRdVref1(DdrcId) >> DQS_VREF_POS1) & DQS_VREF_MASK;
    } else if (ByteNum == 3U) {
        return (AmbaCSL_DdrcGetRdVref1(DdrcId) >> DQS_VREF_POS2) & DQS_VREF_MASK;
    }
    return 0;
#endif
}

void Diag_DdrcSetByteDly(UINT32 DdrcId, UINT32 DieId, UINT32 ByteNum, UINT32 Index, UINT32 Val)
{
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    if (DieId == 0U) {
        if (ByteNum == 0U) {
            AmbaDDRC_Set(DdrcId, DDRC_OFST_BYTE0DIE0DLY(Index), Val);
        } else if (ByteNum == 1U) {
            AmbaDDRC_Set(DdrcId, DDRC_OFST_BYTE1DIE0DLY(Index), Val);
        } else if (ByteNum == 2U) {
            AmbaDDRC_Set(DdrcId, DDRC_OFST_BYTE2DIE0DLY(Index), Val);
        } else if (ByteNum == 3U) {
            AmbaDDRC_Set(DdrcId, DDRC_OFST_BYTE3DIE0DLY(Index), Val);
        }
    } else {
        if (ByteNum == 0U) {
            AmbaDDRC_Set(DdrcId, DDRC_OFST_BYTE0DIE1DLY(Index), Val);
        } else if (ByteNum == 1U) {
            AmbaDDRC_Set(DdrcId, DDRC_OFST_BYTE1DIE1DLY(Index), Val);
        } else if (ByteNum == 2U) {
            AmbaDDRC_Set(DdrcId, DDRC_OFST_BYTE2DIE1DLY(Index), Val);
        } else if (ByteNum == 3U) {
            AmbaDDRC_Set(DdrcId, DDRC_OFST_BYTE3DIE1DLY(Index), Val);
        }
    }
#else
    if (DieId == 0U) {
        if (ByteNum == 0U) {
            AmbaCSL_DdrcSetByte0D0Dly(DdrcId, Index, Val);
        } else if (ByteNum == 1U) {
            AmbaCSL_DdrcSetByte1D0Dly(DdrcId, Index, Val);
        } else if (ByteNum == 2U) {
            AmbaCSL_DdrcSetByte2D0Dly(DdrcId, Index, Val);
        } else if (ByteNum == 3U) {
            AmbaCSL_DdrcSetByte3D0Dly(DdrcId, Index, Val);
        }
    } else {
        if (ByteNum == 0U) {
            AmbaCSL_DdrcSetByte0D1Dly(DdrcId, Index, Val);
        } else if (ByteNum == 1U) {
            AmbaCSL_DdrcSetByte1D1Dly(DdrcId, Index, Val);
        } else if (ByteNum == 2U) {
            AmbaCSL_DdrcSetByte2D1Dly(DdrcId, Index, Val);
        } else if (ByteNum == 3U) {
            AmbaCSL_DdrcSetByte3D1Dly(DdrcId, Index, Val);
        }
    }
#endif
}

void Diag_DdrcSetDqVref(UINT32 DdrcId, UINT32 ByteNum, UINT32 Val)
{
    UINT32 CurRegValue = 0U, NewRegValue = 0U;

    if (ByteNum == 0U) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        CurRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF0);
#else
        CurRegValue = AmbaCSL_DdrcGetRdVref0(DdrcId);
#endif
        NewRegValue = CurRegValue & ~(DQ_VREF_MASK << DQ_VREF_POS1);
        NewRegValue = NewRegValue | (Val << DQ_VREF_POS1);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        AmbaDDRC_Set(DdrcId, DDRC_OFST_RDVREF0, NewRegValue);
#else
        AmbaCSL_DdrcSetRdVref0(DdrcId, NewRegValue);
#endif
    } else if (ByteNum == 1U) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        CurRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF0);
#else
        CurRegValue = AmbaCSL_DdrcGetRdVref0(DdrcId);
#endif
        NewRegValue = CurRegValue & ~(DQ_VREF_MASK << DQ_VREF_POS2);
        NewRegValue = NewRegValue | (Val << DQ_VREF_POS2);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        AmbaDDRC_Set(DdrcId, DDRC_OFST_RDVREF0, NewRegValue);
#else
        AmbaCSL_DdrcSetRdVref0(DdrcId, NewRegValue);
#endif
    } else if (ByteNum == 2U) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        CurRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF1);
#else
        CurRegValue = AmbaCSL_DdrcGetRdVref1(DdrcId);
#endif
        NewRegValue = CurRegValue & ~(DQ_VREF_MASK << DQ_VREF_POS1);
        NewRegValue = NewRegValue | (Val << DQ_VREF_POS1);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        AmbaDDRC_Set(DdrcId, DDRC_OFST_RDVREF1, NewRegValue);
#else
        AmbaCSL_DdrcSetRdVref1(DdrcId, NewRegValue);
#endif
    } else if (ByteNum == 3U) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        CurRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF1);
#else
        CurRegValue = AmbaCSL_DdrcGetRdVref1(DdrcId);
#endif
        NewRegValue = CurRegValue & ~(DQ_VREF_MASK << DQ_VREF_POS2);
        NewRegValue = NewRegValue | (Val << DQ_VREF_POS2);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        AmbaDDRC_Set(DdrcId, DDRC_OFST_RDVREF1, NewRegValue);
#else
        AmbaCSL_DdrcSetRdVref1(DdrcId, NewRegValue);
#endif
    }
}

void Diag_DdrcSetDqsVrefPerByte(UINT32 DdrcId, UINT32 ByteNum, UINT32 Val)
{
    UINT32 CurRegValue = 0U, NewRegValue = 0U;

    if (ByteNum == 0U) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        CurRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF0);
#else
        CurRegValue = AmbaCSL_DdrcGetRdVref0(DdrcId);
#endif
        NewRegValue = CurRegValue & ~(DQS_VREF_MASK << DQS_VREF_POS1);
        NewRegValue = NewRegValue | (Val << DQS_VREF_POS1);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        AmbaDDRC_Set(DdrcId, DDRC_OFST_RDVREF0, NewRegValue);
#else
        AmbaCSL_DdrcSetRdVref0(DdrcId, NewRegValue);
#endif
    } else if (ByteNum == 1U) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        CurRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF0);
#else
        CurRegValue = AmbaCSL_DdrcGetRdVref0(DdrcId);
#endif
        NewRegValue = CurRegValue & ~(DQS_VREF_MASK << DQS_VREF_POS2);
        NewRegValue = NewRegValue | (Val << DQS_VREF_POS2);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        AmbaDDRC_Set(DdrcId, DDRC_OFST_RDVREF0, NewRegValue);
#else
        AmbaCSL_DdrcSetRdVref0(DdrcId, NewRegValue);
#endif
    } else if (ByteNum == 2U) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        CurRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF1);
#else
        CurRegValue = AmbaCSL_DdrcGetRdVref1(DdrcId);
#endif
        NewRegValue = CurRegValue & ~(DQS_VREF_MASK << DQS_VREF_POS1);
        NewRegValue = NewRegValue | (Val << DQS_VREF_POS1);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        AmbaDDRC_Set(DdrcId, DDRC_OFST_RDVREF1, NewRegValue);
#else
        AmbaCSL_DdrcSetRdVref1(DdrcId, NewRegValue);
#endif
    } else if (ByteNum == 3U) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        CurRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF1);
#else
        CurRegValue = AmbaCSL_DdrcGetRdVref1(DdrcId);
#endif
        NewRegValue = CurRegValue & ~(DQS_VREF_MASK << DQS_VREF_POS2);
        NewRegValue = NewRegValue | (Val << DQS_VREF_POS2);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        AmbaDDRC_Set(DdrcId, DDRC_OFST_RDVREF1, NewRegValue);
#else
        AmbaCSL_DdrcSetRdVref1(DdrcId, NewRegValue);
#endif
    }
}

UINT32 Diag_DdrGetLp4Vref(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            return Diag_DdrcGetDqVref(DdrcId, i);
        }
    }

    return 0;
}

UINT32 Diag_DdrGetLp4MR14(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0;

    Diag_DdrcSetModeRegWait(DdrcId, /*BitMask*/0xffffffff, 0x9a0e0000); // Since the controller supports both channel data in a read, we can use full mask
    CurRegValue = Diag_DdrcGetModeReg(DdrcId, BitMask) & LP4_MR14_MASK; /* bit[6:0] */
    if (CurRegValue > 64U) {
        CurRegValue = CurRegValue - 64U + 30U;  /* bit[6] is set, we should remove bit[6] and remap bit[5:0] to 31~80 */
    }

    return CurRegValue;
}

UINT32 Diag_DdrGetLp4MR12(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0;

    Diag_DdrcSetModeRegWait(DdrcId, /*BitMask*/0xffffffff, 0x9a0c0000); // Since the controller supports both channel data in a read, we can use full mask
    CurRegValue = Diag_DdrcGetModeReg(DdrcId, BitMask) & LP4_MR12_MASK; /* bit[6:0] */
    if (CurRegValue > 64U) {
        CurRegValue = CurRegValue - 64U + 30U;  /* bit[6] is set, we should remove bit[6] and remap bit[5:0] to 31~80 */
    }

    return CurRegValue;
}

UINT32 Diag_DdrGetLp4WriteDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i, j;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            /* Get only consider Die 0 here */
            CurRegValue = Diag_DdrcGetByteDly(DdrcId, 0, i, 1U);
            Coarse = (CurRegValue >> WRITE_DELAY_POS_COARSE) & WRITE_DELAY_MASK;
            Fine = (CurRegValue >> WRITE_DELAY_POS_FINE) & WRITE_DELAY_MASK;
            /* Add fine of DQ[0~3] */
            CurRegValue = Diag_DdrcGetByteDly(DdrcId, 0, i, 2U);
            for (j = 0U; j < 4; j++) {
                Fine += (CurRegValue >> (j*8)) & WRITE_DELAY_MASK;
            }
            /* Add fine of DQ[4~7] */
            CurRegValue = Diag_DdrcGetByteDly(DdrcId, 0, i, 3U);
            for (j = 0U; j < 4; j++) {
                Fine += (CurRegValue >> (j*8)) & WRITE_DELAY_MASK;
            }
            /* Average fine of DM and DQ[0~7] */
            Fine /= 9;
            CurDelay = (Coarse * 12U) + Fine;
            return CurDelay;
        }
    }

    return 0;
}

UINT32 Diag_DdrGetDqsVref(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            return Diag_DdrcGetDqsVrefPerByte(DdrcId, i);
        }
    }

    return 0;
}

UINT32 Diag_DdrGetWckDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            /* Get only consider Die 0 here */
            CurRegValue = Diag_DdrcGetByteDly(DdrcId, 0, i, 0U);
            Coarse = (CurRegValue >> WCK_DLY_POS_COARSE) & WCK_DLY_MASK;
            Fine = (CurRegValue >> WCK_DLY_POS_FINE) & WCK_DLY_MASK;
            CurDelay = (Coarse * 12U) + Fine;
            return CurDelay;
        }
    }

    return 0;
}

UINT32 Diag_DdrGetDqsWriteDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            /* Get only consider Die 0 here */
            CurRegValue = Diag_DdrcGetByteDly(DdrcId, 0, i, 0U);
            Coarse = (CurRegValue >> DQS_WRITE_DLY_POS_COARSE) & DQS_WRITE_DLY_MASK;
            Fine = (CurRegValue >> DQS_WRITE_DLY_POS_FINE) & DQS_WRITE_DLY_MASK;
            CurDelay = (Coarse * 12U) + Fine;
            return CurDelay;
        }
    }

    return 0;
}

UINT32 Diag_DdrGetDqsGateDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            /* Get only consider Die 0 here */
            CurRegValue = Diag_DdrcGetByteDly(DdrcId, 0, i, 1U);
            Coarse = (CurRegValue >> DQS_GATE_DLY_POS_COARSE) & DQS_GATE_DLY_MASK;
            Fine = (CurRegValue >> DQS_GATE_DLY_POS_FINE) & DQS_GATE_DLY_MASK;
            CurDelay = (Coarse * 12U) + Fine;
            return CurDelay;
        }
    }

    return 0;
}

UINT32 Diag_DdrGetCkDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0;

#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    CurRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_CKDLY);
#else
    CurRegValue = AmbaCSL_DdrcGetCkDly(DdrcId);
#endif
    if (BitMask & 0xffffU) {
        Coarse = (CurRegValue >> CK_DLY_POSA_COARSE) & CK_DLY_MASK;
        Fine = (CurRegValue >> CK_DLY_POSA_FINE) & CK_DLY_MASK;
    } else if (BitMask & 0xffff0000U) {
        Coarse = (CurRegValue >> CK_DLY_POSB_COARSE) & CK_DLY_MASK;
        Fine = (CurRegValue >> CK_DLY_POSB_FINE) & CK_DLY_MASK;
    }
    CurDelay = (Coarse * 12U) + Fine;

    return CurDelay;
}

UINT32 Diag_DdrGetCkeDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, CurRegValueFine = 0, Coarse = 0, Fine = 0, CurDelay = 0;

#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    CurRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_CKEDLYCOARSE);  // CKE coarse
    CurRegValueFine = AmbaDDRC_Get(DdrcId, DDRC_OFST_CKEDLYFINE);   // CKE fine
#else
    CurRegValue = AmbaCSL_DdrcGetCkeDlyCoars(DdrcId);  // CKE coarse
    CurRegValueFine = AmbaCSL_DdrcGetCkeDlyFine(DdrcId);   // CKE fine
#endif
    if (BitMask & 0xffffU) {
        Coarse = (CurRegValue >> CKE_DLY_POSA_COARSE) & CKE_DLY_MASK;
        Fine = (CurRegValueFine >> CKE_DLY_POS0A_FINE) & CKE_DLY_MASK;
    } else if (BitMask & 0xffff0000U) {
        Coarse = (CurRegValue >> CKE_DLY_POSB_COARSE) & CKE_DLY_MASK;
        Fine = (CurRegValueFine >> CKE_DLY_POS0B_FINE) & CKE_DLY_MASK;
    }
    CurDelay = (Coarse * 12U) + Fine;

    return CurDelay;
}

UINT32 Diag_DdrGetCaDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, CurRegValueFine = 0, Coarse = 0, Fine = 0, CurDelay = 0;

#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    CurRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_CADLYCOARSE);                // CA coarse
#else
    CurRegValue = AmbaCSL_DdrcGetCaDlyCoars(DdrcId);                // CA coarse
#endif
    if (BitMask & 0xffffU) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        CurRegValueFine = AmbaDDRC_Get(DdrcId, DDRC_OFST_CADLYFINECHADIE0(0)); // CA fine
#else
        CurRegValueFine = AmbaCSL_DdrcGetCaDlyFineChAD0(DdrcId, 0); // CA fine
#endif
        Coarse = (CurRegValue >> CA_DLY_POSA_COARSE) & CA_DLY_MASK;
        Fine = (CurRegValueFine >> CA_DLY_POS0_ADDR0_FINE) & CA_DLY_MASK;
    } else if (BitMask & 0xffff0000U) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        CurRegValueFine = AmbaDDRC_Get(DdrcId, DDRC_OFST_CADLYFINECHBDIE0(0)); // CA fine
#else
        CurRegValueFine = AmbaCSL_DdrcGetCaDlyFineChBD0(DdrcId, 0); // CA fine
#endif
        Coarse = (CurRegValue >> CA_DLY_POSB_COARSE) & CA_DLY_MASK;
        Fine = (CurRegValueFine >> CA_DLY_POS0_ADDR0_FINE) & CA_DLY_MASK;
    }
    CurDelay = (Coarse * 12U) + Fine;

    return CurDelay;
}

UINT32 Diag_DdrGetLp5MR14(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0;

    Diag_DdrcSetModeRegWait(DdrcId, /*BitMask*/0xffffffff, 0x9a0e0000); // Since the controller supports both channel data in a read, we can use full mask
    CurRegValue = Diag_DdrcGetModeReg(DdrcId, BitMask) & LP5_MR14_MASK; /* bit[6:0] */

    return CurRegValue;
}

UINT32 Diag_DdrGetLp5MR15(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0;

    Diag_DdrcSetModeRegWait(DdrcId, /*BitMask*/0xffffffff, 0x9a0f0000); // Since the controller supports both channel data in a read, we can use full mask
    CurRegValue = Diag_DdrcGetModeReg(DdrcId, BitMask) & LP5_MR15_MASK; /* bit[6:0] */

    return CurRegValue;
}

UINT32 Diag_DdrGetLp5MR12(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0;

    Diag_DdrcSetModeRegWait(DdrcId, /*BitMask*/0xffffffff, 0x9a0c0000); // Since the controller supports both channel data in a read, we can use full mask
    CurRegValue = Diag_DdrcGetModeReg(DdrcId, BitMask) & LP5_MR12_MASK; /* bit[6:0] */

    return CurRegValue;
}

UINT32 Diag_DdrcGetMisc2(UINT32 DdrcId)
{
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    return AmbaDDRC_Get(DdrcId, DDRC_OFST_DDRCMISC2);
#else
    return AmbaCSL_DdrcGetMisc2(DdrcId);
#endif
}

UINT32 Diag_DdrGetLp4ReadDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i, j;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            /* Get only consider Die 0 here */
            CurRegValue = Diag_DdrcGetByteDly(DdrcId, 0, i, 1U);
            Coarse = (CurRegValue >> READ_DELAY_POS_COARSE) & READ_DELAY_MASK;
            Fine = (CurRegValue >> READ_DELAY_POS_FINE) & READ_DELAY_MASK;
            /* Add fine of DQ[0~3] */
            CurRegValue = Diag_DdrcGetByteDly(DdrcId, 0, i, 4U);
            for (j = 0U; j < 4; j++) {
                Fine += (CurRegValue >> (j*8)) & WRITE_DELAY_MASK;
            }
            /* Add fine of DQ[4~7] */
            CurRegValue = Diag_DdrcGetByteDly(DdrcId, 0, i, 5U);
            for (j = 0U; j < 4; j++) {
                Fine += (CurRegValue >> (j*8)) & WRITE_DELAY_MASK;
            }
            /* Average fine of DM and DQ[0~7] */
            Fine /= 9;
            CurDelay = (Coarse * 12U) + Fine;
            return CurDelay;
        }
    }

    return 0;
}

void Diag_DdrSetLp4ReadDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, NewRegValueFine = 0, Coarse = 0, Fine = 0, CurDelay = 0, i, Mask;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

    for (i = 0U; i < 4U; i++) {
        Mask = 0xffU << (i * 8);
        if (Mask & BitMask) {
            /* Die 1 */
            NewRegValue = Diag_DdrcGetByteDly(DdrcId, 1, i, 1U);
            NewRegValue &= ~((READ_DELAY_MASK << READ_DELAY_POS_FINE) | (READ_DELAY_MASK << READ_DELAY_POS_COARSE));    // Clear [29:25], [19:15]
            NewRegValue |= (Fine << READ_DELAY_POS_FINE) | (Coarse << READ_DELAY_POS_COARSE);
            NewRegValueFine = (Fine << 24U) | (Fine << 16U) | (Fine << 8U) | (Fine << 0U);
            Diag_DdrcSetByteDly(DdrcId, 1, i, 1U, NewRegValue);          // DQ Read coarse, DM Read fine
            Diag_DdrcSetByteDly(DdrcId, 1, i, 4U, NewRegValueFine);      // DQ Read fine
            Diag_DdrcSetByteDly(DdrcId, 1, i, 5U, NewRegValueFine);      // DQ Read fine
            /* Die 0 */
            NewRegValue = Diag_DdrcGetByteDly(DdrcId, 0, i, 1U);
            NewRegValue &= ~((READ_DELAY_MASK << READ_DELAY_POS_FINE) | (READ_DELAY_MASK << READ_DELAY_POS_COARSE));    // Clear [29:25], [19:15]
            NewRegValue |= (Fine << READ_DELAY_POS_FINE) | (Coarse << READ_DELAY_POS_COARSE);
            NewRegValueFine = (Fine << 24U) | (Fine << 16U) | (Fine << 8U) | (Fine << 0U);
            Diag_DdrcSetByteDly(DdrcId, 0, i, 1U, NewRegValue);          // DQ Read coarse, DM Read fine
            Diag_DdrcSetByteDly(DdrcId, 0, i, 4U, NewRegValueFine);      // DQ Read fine
            Diag_DdrcSetByteDly(DdrcId, 0, i, 5U, NewRegValueFine);      // DQ Read fine

            CurDelay = Diag_DdrGetLp4ReadDly(DdrcId, Mask);
            Diag_PrintFormattedRegValPerChan("[DDRC%d][DqRead Delay%d]Delay1,4,5 = 0x%08x,", DdrcId, i, NewRegValue, PrintFunc);
            Diag_PrintFormattedRegValPerChan(" 0x%08x, 0x%08x (value = 0x%x)\n", NewRegValueFine, NewRegValueFine, CurDelay, PrintFunc);
        }
    }
}

void Diag_DdrSetLp4Vref(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, RegIndex = 0, i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            Diag_DdrcSetDqVref(DdrcId, i, Val);

            if (i < 2) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
                NewRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF0);
#else
                NewRegValue = AmbaCSL_DdrcGetRdVref0(DdrcId);
#endif
                RegIndex = 0;
            } else {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
                NewRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF1);
#else
                NewRegValue = AmbaCSL_DdrcGetRdVref1(DdrcId);
#endif
                RegIndex = 1;
            }
            Diag_PrintFormattedRegValPerChan("[DDRC%d][DQ Vref%d]TrainDqVref%d =", DdrcId, i, RegIndex, PrintFunc);
            Diag_PrintFormattedRegValPerChan(" 0x%08x (value = 0x%x)\n", NewRegValue, Diag_DdrcGetDqVref(DdrcId, i), 0, PrintFunc);
        }
    }
}

void Diag_DdrSetLp4MR14(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0;
    UINT32 ModeRegSetting = 0;

    Val &= LP4_MR14_MASK;
    if (Val <= 50) {
        NewRegValue = 0x9f0e0000UL | Val;
    } else {
        NewRegValue = 0x9f0e0040UL | (Val - 30); /* Remap 51~80 to bit[5:0] and set bit[6] */
    }
    Diag_DdrcSetModeRegWait(DdrcId, BitMask, NewRegValue);

    NewRegValue = Diag_DdrGetLp4MR14(DdrcId, BitMask);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    ModeRegSetting = AmbaDDRC_GetModeReg(DdrcId);
#else
    ModeRegSetting = AmbaCSL_DdrcGetModeReg(DdrcId);
#endif

    Diag_PrintFormattedRegValPerChan("[LPDDR4_%d][MR14 Vref]Lpddr4DqVref = 0x%08x (value = 0x%x)\n", DdrcId, ModeRegSetting, NewRegValue, PrintFunc);
}

void Diag_DdrSetLp4MR12(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0;
    UINT32 ModeRegSetting = 0;

    Val &= LP4_MR12_MASK;
    if (Val <= 50) {
        NewRegValue = 0x9f0c0000UL | Val;
    } else {
        NewRegValue = 0x9f0c0040UL | (Val - 30); /* Remap 51~80 to bit[5:0] and set bit[6] */
    }
    Diag_DdrcSetModeRegWait(DdrcId, BitMask, NewRegValue);

    NewRegValue = Diag_DdrGetLp4MR12(DdrcId, BitMask);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    ModeRegSetting = AmbaDDRC_GetModeReg(DdrcId);
#else
    ModeRegSetting = AmbaCSL_DdrcGetModeReg(DdrcId);
#endif

    Diag_PrintFormattedRegValPerChan("[LPDDR4_%d][MR12 Vref]Lpddr4CaVref = 0x%08x (value = 0x%x)\n", DdrcId, ModeRegSetting, NewRegValue, PrintFunc);
}

void Diag_DdrSetLp4WriteDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, NewRegValueFine = 0, Coarse = 0, Fine = 0, CurDelay = 0, i, Mask;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

    for (i = 0U; i < 4U; i++) {
        Mask = 0xffU << (i * 8);
        if (Mask & BitMask) {
            /* Die 1 */
            NewRegValue = Diag_DdrcGetByteDly(DdrcId, 1, i, 1U);
            NewRegValue &= ~((WRITE_DELAY_MASK << WRITE_DELAY_POS_FINE) | (WRITE_DELAY_MASK << WRITE_DELAY_POS_COARSE));    // Clear [24:20], [14:10]
            NewRegValue |= (Fine << WRITE_DELAY_POS_FINE) | (Coarse << WRITE_DELAY_POS_COARSE);
            NewRegValueFine = (Fine << 24U) | (Fine << 16U) | (Fine << 8U) | (Fine << 0U);
            Diag_DdrcSetByteDly(DdrcId, 1, i, 1U, NewRegValue);          // DQ Write coarse, DM Write fine
            Diag_DdrcSetByteDly(DdrcId, 1, i, 2U, NewRegValueFine);      // DQ Write fine
            Diag_DdrcSetByteDly(DdrcId, 1, i, 3U, NewRegValueFine);      // DQ Write fine
            /* Die 0 */
            NewRegValue = Diag_DdrcGetByteDly(DdrcId, 0, i, 1U);
            NewRegValue &= ~((WRITE_DELAY_MASK << WRITE_DELAY_POS_FINE) | (WRITE_DELAY_MASK << WRITE_DELAY_POS_COARSE));    // Clear [24:20], [14:10]
            NewRegValue |= (Fine << WRITE_DELAY_POS_FINE) | (Coarse << WRITE_DELAY_POS_COARSE);
            NewRegValueFine = (Fine << 24U) | (Fine << 16U) | (Fine << 8U) | (Fine << 0U);
            Diag_DdrcSetByteDly(DdrcId, 0, i, 1U, NewRegValue);          // DQ Write coarse, DM Write fine
            Diag_DdrcSetByteDly(DdrcId, 0, i, 2U, NewRegValueFine);      // DQ Write fine
            Diag_DdrcSetByteDly(DdrcId, 0, i, 3U, NewRegValueFine);      // DQ Write fine

            CurDelay = Diag_DdrGetLp4WriteDly(DdrcId, Mask);
            Diag_PrintFormattedRegValPerChan("[DDRC%d][DqWrite Delay%d]Delay1,2,3 = 0x%08x,", DdrcId, i, NewRegValue, PrintFunc);
            Diag_PrintFormattedRegValPerChan(" 0x%08x, 0x%08x (value = 0x%x)\n", NewRegValueFine, NewRegValueFine, CurDelay, PrintFunc);
        }
    }
}

void Diag_DdrSetDqsVref(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, RegIndex = 0, i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            Diag_DdrcSetDqsVrefPerByte(DdrcId, i, Val);

            if (i < 2) {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
                NewRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF0);
#else
                NewRegValue = AmbaCSL_DdrcGetRdVref0(DdrcId);
#endif
                RegIndex = 0;
            } else {
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
                NewRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_RDVREF1);
#else
                NewRegValue = AmbaCSL_DdrcGetRdVref1(DdrcId);
#endif
                RegIndex = 1;
            }
            Diag_PrintFormattedRegValPerChan("[DDRC%d][DQS Vref%d]TrainDqVref%d =", DdrcId, i, RegIndex, PrintFunc);
            Diag_PrintFormattedRegValPerChan(" 0x%08x (value = 0x%x)\n", NewRegValue, Diag_DdrcGetDqsVrefPerByte(DdrcId, i), 0, PrintFunc);
        }
    }
}

void Diag_DdrSetWckDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i, Mask;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

    for (i = 0U; i < 4U; i++) {
        Mask = 0xffU << (i * 8);
        if (Mask & BitMask) {
            /* Die 1 */
            NewRegValue = Diag_DdrcGetByteDly(DdrcId, 1, i, 0U);
            NewRegValue &= ~((WCK_DLY_MASK << WCK_DLY_POS_COARSE) | (WCK_DLY_MASK << WCK_DLY_POS_FINE));
            NewRegValue |= (Coarse << WCK_DLY_POS_COARSE) | (Fine << WCK_DLY_POS_FINE);
            Diag_DdrcSetByteDly(DdrcId, 1, i, 0U, NewRegValue);      // WCK coarse, WCK fine
            /* Die 0 */
            NewRegValue = Diag_DdrcGetByteDly(DdrcId, 0, i, 0U);
            NewRegValue &= ~((WCK_DLY_MASK << WCK_DLY_POS_COARSE) | (WCK_DLY_MASK << WCK_DLY_POS_FINE));
            NewRegValue |= (Coarse << WCK_DLY_POS_COARSE) | (Fine << WCK_DLY_POS_FINE);
            Diag_DdrcSetByteDly(DdrcId, 0, i, 0U, NewRegValue);      // WCK coarse, WCK fine

            CurDelay = Diag_DdrGetWckDly(DdrcId, Mask);
            Diag_PrintFormattedRegValPerChan("[DDRC%d][Wck Delay%d]Delay0 = 0x%08x,", DdrcId, i, NewRegValue, PrintFunc);
            Diag_PrintFormattedRegValPerChan(" (value = 0x%x)\n", CurDelay, 0, 0, PrintFunc);
        }
    }
}

void Diag_DdrSetDqsWriteDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i, Mask;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

    for (i = 0U; i < 4U; i++) {
        Mask = 0xffU << (i * 8);
        if (Mask & BitMask) {
            /* Die 1 */
            NewRegValue = Diag_DdrcGetByteDly(DdrcId, 1, i, 0U);
            NewRegValue &= ~((DQS_WRITE_DLY_MASK << DQS_WRITE_DLY_POS_COARSE) | (DQS_WRITE_DLY_MASK << DQS_WRITE_DLY_POS_FINE));
            NewRegValue |= (Coarse << DQS_WRITE_DLY_POS_COARSE) | (Fine << DQS_WRITE_DLY_POS_FINE);
            Diag_DdrcSetByteDly(DdrcId, 1, i, 0U, NewRegValue);      // DQS Write coarse, DQS Write fine
            /* Die 0 */
            NewRegValue = Diag_DdrcGetByteDly(DdrcId, 0, i, 0U);
            NewRegValue &= ~((DQS_WRITE_DLY_MASK << DQS_WRITE_DLY_POS_COARSE) | (DQS_WRITE_DLY_MASK << DQS_WRITE_DLY_POS_FINE));
            NewRegValue |= (Coarse << DQS_WRITE_DLY_POS_COARSE) | (Fine << DQS_WRITE_DLY_POS_FINE);
            Diag_DdrcSetByteDly(DdrcId, 0, i, 0U, NewRegValue);      // DQS Write coarse, DQS Write fine

            CurDelay = Diag_DdrGetDqsWriteDly(DdrcId, Mask);
            Diag_PrintFormattedRegValPerChan("[DDRC%d][DqsWrite Delay%d]Delay0 = 0x%08x,", DdrcId, i, NewRegValue, PrintFunc);
            Diag_PrintFormattedRegValPerChan(" (value = 0x%x)\n", CurDelay, 0, 0, PrintFunc);
        }
    }
}

void Diag_DdrSetDqsGateDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i, Mask;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

    for (i = 0U; i < 4U; i++) {
        Mask = 0xffU << (i * 8);
        if (Mask & BitMask) {
            /* Die 1 */
            NewRegValue = Diag_DdrcGetByteDly(DdrcId, 1, i, 1U);
            NewRegValue &= ~((DQS_GATE_DLY_MASK << DQS_GATE_DLY_POS_COARSE) | (DQS_GATE_DLY_MASK << DQS_GATE_DLY_POS_FINE));
            NewRegValue |= (Coarse << DQS_GATE_DLY_POS_COARSE) | (Fine << DQS_GATE_DLY_POS_FINE);
            Diag_DdrcSetByteDly(DdrcId, 1, i, 1U, NewRegValue);      // DQS Gate coarse, DQS Gate fine
            /* Die 0 */
            NewRegValue = Diag_DdrcGetByteDly(DdrcId, 0, i, 1U);
            NewRegValue &= ~((DQS_GATE_DLY_MASK << DQS_GATE_DLY_POS_COARSE) | (DQS_GATE_DLY_MASK << DQS_GATE_DLY_POS_FINE));
            NewRegValue |= (Coarse << DQS_GATE_DLY_POS_COARSE) | (Fine << DQS_GATE_DLY_POS_FINE);
            Diag_DdrcSetByteDly(DdrcId, 0, i, 1U, NewRegValue);      // DQS Gate coarse, DQS Gate fine

            CurDelay = Diag_DdrGetDqsGateDly(DdrcId, Mask);
            Diag_PrintFormattedRegValPerChan("[DDRC%d][DqsGate Delay%d]Delay1 = 0x%08x,", DdrcId, i, NewRegValue, PrintFunc);
            Diag_PrintFormattedRegValPerChan(" (value = 0x%x)\n", CurDelay, 0, 0, PrintFunc);
        }
    }
}

void Diag_DdrSetCkDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    NewRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_CKDLY);
#else
    NewRegValue = AmbaCSL_DdrcGetCkDly(DdrcId);
#endif
    if (BitMask & 0xffffU) {
        NewRegValue &= ~0xffffU;
        NewRegValue |= (Coarse << CK_DLY_POSA_COARSE) | (Fine << CK_DLY_POSA_FINE);
    }
    if (BitMask & 0xffff0000U) {
        NewRegValue &= ~0xffff0000U;
        NewRegValue |= (Coarse << CK_DLY_POSB_COARSE) | (Fine << CK_DLY_POSB_FINE);
    }
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    AmbaDDRC_Set(DdrcId, DDRC_OFST_CKDLY, NewRegValue);
#else
    AmbaCSL_DdrcSetCkDly(DdrcId, NewRegValue);
#endif

    CurDelay = Diag_DdrGetCkDly(DdrcId, BitMask);
    Diag_PrintFormattedRegValPerChan("[DDRC%d][Ck Dly]Reg = 0x%08x (value = 0x%x)\n", DdrcId, NewRegValue, CurDelay, PrintFunc);
}

void Diag_DdrSetCkeDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, NewRegValueFine = 0, Coarse = 0, Fine = 0, CurDelay = 0;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    NewRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_CKEDLYCOARSE);
    NewRegValueFine = AmbaDDRC_Get(DdrcId, DDRC_OFST_CKEDLYFINE);
#else
    NewRegValue = AmbaCSL_DdrcGetCkeDlyCoars(DdrcId);
    NewRegValueFine = AmbaCSL_DdrcGetCkeDlyFine(DdrcId);
#endif
    if (BitMask & 0xffffU) {
        NewRegValue &= ~(CKE_DLY_MASK << CKE_DLY_POSA_COARSE);
        NewRegValue |= (Coarse << CKE_DLY_POSA_COARSE);
        NewRegValueFine &= ~((CKE_DLY_MASK << CKE_DLY_POS0A_FINE) | (CKE_DLY_MASK << CKE_DLY_POS1A_FINE));
        NewRegValueFine |= (Fine << CKE_DLY_POS0A_FINE) | (Fine << CKE_DLY_POS1A_FINE);
    }
    if (BitMask & 0xffff0000U) {
        NewRegValue &= ~(CKE_DLY_MASK << CKE_DLY_POSB_COARSE);
        NewRegValue |= (Coarse << CKE_DLY_POSB_COARSE);
        NewRegValueFine &= ~((CKE_DLY_MASK << CKE_DLY_POS0B_FINE) | (CKE_DLY_MASK << CKE_DLY_POS1B_FINE));
        NewRegValueFine |= (Fine << CKE_DLY_POS0B_FINE) | (Fine << CKE_DLY_POS1B_FINE);
    }
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    AmbaDDRC_Set(DdrcId, DDRC_OFST_CKEDLYCOARSE, NewRegValue);
    AmbaDDRC_Set(DdrcId, DDRC_OFST_CKEDLYFINE, NewRegValueFine);
#else
    AmbaCSL_DdrcSetCkeDlyCoars(DdrcId, NewRegValue);
    AmbaCSL_DdrcSetCkeDlyFine(DdrcId, NewRegValueFine);
#endif

    CurDelay = Diag_DdrGetCkeDly(DdrcId, BitMask);
    Diag_PrintFormattedRegValPerChan("[DDRC%d][Cke Dly]Coarse = 0x%08x, Fine = 0x%08x", DdrcId, NewRegValue, NewRegValueFine, PrintFunc);
    Diag_PrintFormattedRegValPerChan(" (value = 0x%x)\n", CurDelay, 0, 0, PrintFunc);
}

void Diag_DdrSetCaDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, NewRegValueFine = 0, NewRegValueFine2 = 0, Coarse = 0, Fine = 0, CurDelay = 0;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    NewRegValue = AmbaDDRC_Get(DdrcId, DDRC_OFST_CADLYCOARSE);
#else
    NewRegValue = AmbaCSL_DdrcGetCaDlyCoars(DdrcId);
#endif
    NewRegValueFine = (Fine << CA_DLY_POS0_ADDR0_FINE) | (Fine << CA_DLY_POS0_ADDR1_FINE) |
                      (Fine << CA_DLY_POS0_ADDR2_FINE) | (Fine << CA_DLY_POS0_ADDR3_FINE);
    NewRegValueFine2 = (Fine << CA_DLY_POS0_ADDR4_FINE) | (Fine << CA_DLY_POS0_ADDR5_FINE) |
                       (Fine << CA_DLY_POS0_ADDR6_FINE);
    if (BitMask & 0xffffU) {
        NewRegValue &= ~(CA_DLY_MASK << CA_DLY_POSA_COARSE);
        NewRegValue |= (Coarse << CA_DLY_POSA_COARSE);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        AmbaDDRC_Set(DdrcId, DDRC_OFST_CADLYFINECHADIE0(0), NewRegValueFine);
        AmbaDDRC_Set(DdrcId, DDRC_OFST_CADLYFINECHADIE1(0), NewRegValueFine);
        AmbaDDRC_Set(DdrcId, DDRC_OFST_CADLYFINECHADIE0(1), NewRegValueFine2);
        AmbaDDRC_Set(DdrcId, DDRC_OFST_CADLYFINECHADIE1(1), NewRegValueFine2);
#else
        AmbaCSL_DdrcSetCaDlyFineChAD0(DdrcId, 0, NewRegValueFine);
        AmbaCSL_DdrcSetCaDlyFineChAD1(DdrcId, 0, NewRegValueFine);
        AmbaCSL_DdrcSetCaDlyFineChAD0(DdrcId, 1, NewRegValueFine2);
        AmbaCSL_DdrcSetCaDlyFineChAD1(DdrcId, 1, NewRegValueFine2);
#endif
    }
    if (BitMask & 0xffff0000U) {
        NewRegValue &= ~(CA_DLY_MASK << CA_DLY_POSB_COARSE);
        NewRegValue |= (Coarse << CA_DLY_POSB_COARSE);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        AmbaDDRC_Set(DdrcId, DDRC_OFST_CADLYFINECHBDIE0(0), NewRegValueFine);
        AmbaDDRC_Set(DdrcId, DDRC_OFST_CADLYFINECHBDIE1(0), NewRegValueFine);
        AmbaDDRC_Set(DdrcId, DDRC_OFST_CADLYFINECHBDIE0(1), NewRegValueFine2);
        AmbaDDRC_Set(DdrcId, DDRC_OFST_CADLYFINECHBDIE1(1), NewRegValueFine2);
#else
        AmbaCSL_DdrcSetCaDlyFineChBD0(DdrcId, 0, NewRegValueFine);
        AmbaCSL_DdrcSetCaDlyFineChBD1(DdrcId, 0, NewRegValueFine);
        AmbaCSL_DdrcSetCaDlyFineChBD0(DdrcId, 1, NewRegValueFine2);
        AmbaCSL_DdrcSetCaDlyFineChBD1(DdrcId, 1, NewRegValueFine2);
#endif
    }
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    AmbaDDRC_Set(DdrcId, DDRC_OFST_CADLYCOARSE, NewRegValue);
#else
    AmbaCSL_DdrcSetCaDlyCoars(DdrcId, NewRegValue);
#endif

    CurDelay = Diag_DdrGetCaDly(DdrcId, BitMask);
    Diag_PrintFormattedRegValPerChan("[DDRC%d][Ca Dly]Coarse = 0x%08x, Fine0,1 = 0x%08x", DdrcId, NewRegValue, NewRegValueFine, PrintFunc);
    Diag_PrintFormattedRegValPerChan(",0x%08x (value = 0x%x)\n", NewRegValueFine2, CurDelay, 0, PrintFunc);
}

void Diag_DdrSetLp5MR14(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0;
    UINT32 ModeRegSetting = 0;

    Val &= LP5_MR14_MASK;
    NewRegValue = 0x9f0e0000UL | Val;
    Diag_DdrcSetModeRegWait(DdrcId, BitMask, NewRegValue);

    NewRegValue = Diag_DdrGetLp5MR14(DdrcId, BitMask);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    ModeRegSetting = AmbaDDRC_GetModeReg(DdrcId);
#else
    ModeRegSetting = AmbaCSL_DdrcGetModeReg(DdrcId);
#endif
    Diag_PrintFormattedRegValPerChan("[LPDDR5_%d][MR14 Vref]Dq[7:0] = 0x%08x (value = 0x%x)\n", DdrcId, ModeRegSetting, NewRegValue, PrintFunc);
}

void Diag_DdrSetLp5MR15(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0;
    UINT32 ModeRegSetting = 0;

    Val &= LP5_MR15_MASK;
    NewRegValue = 0x9f0f0000UL | Val;
    Diag_DdrcSetModeRegWait(DdrcId, BitMask, NewRegValue);

    NewRegValue = Diag_DdrGetLp5MR15(DdrcId, BitMask);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    ModeRegSetting = AmbaDDRC_GetModeReg(DdrcId);
#else
    ModeRegSetting = AmbaCSL_DdrcGetModeReg(DdrcId);
#endif
    Diag_PrintFormattedRegValPerChan("[LPDDR5_%d][MR15 Vref]Dq[15:8] = 0x%08x (value = 0x%x)\n", DdrcId, ModeRegSetting, NewRegValue, PrintFunc);
}

void Diag_DdrSetLp5MR12(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0;
    UINT32 ModeRegSetting = 0;

    Val &= LP5_MR12_MASK;
    NewRegValue = 0x9f0c0000UL | Val;
    Diag_DdrcSetModeRegWait(DdrcId, BitMask, NewRegValue);

    NewRegValue = Diag_DdrGetLp5MR12(DdrcId, BitMask);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    ModeRegSetting = AmbaDDRC_GetModeReg(DdrcId);
#else
    ModeRegSetting = AmbaCSL_DdrcGetModeReg(DdrcId);
#endif
    Diag_PrintFormattedRegValPerChan("[LPDDR5_%d][MR12 Vref]Ca = 0x%08x (value = 0x%x)\n", DdrcId, ModeRegSetting, NewRegValue, PrintFunc);
}

void Diag_DdrcSetMisc2(UINT32 DdrcId, UINT32 Val)
{
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    AmbaDDRC_Set(DdrcId, DDRC_OFST_DDRCMISC2, Val);
#else
    AmbaCSL_DdrcSetMisc2(DdrcId, Val);
#endif
}
