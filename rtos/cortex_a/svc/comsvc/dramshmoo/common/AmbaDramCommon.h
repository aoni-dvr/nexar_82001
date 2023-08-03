#ifndef AMBA_DRAM_COMMON_H
#define AMBA_DRAM_COMMON_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

void Diag_PrintFormattedRegVal(const char *pFmtString, volatile const void *pRegAddr, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_PrintFormattedRegValPerChan(const char *pFmtString, UINT32 DdrcId, UINT32 RegValue, UINT32 ChanValue, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_PrintSingleU32Str(const char *pFmt, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_PrintSingleU64Str(const char *pFmt, UINT64 Value, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_PrintU32Str(const char *pFmt, UINT32 arg_count, UINT32* pArgs, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_PrintU64Str(const char *pFmt, UINT32 arg_count, UINT64* pArgs, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_PrintMemory( UINT64 AddrStart, UINT64 AddrEnd, AMBA_SHELL_PRINT_f PrintFunc);
UINT32 Diag_DdrcGetModeReg(UINT32 DdrcId, UINT32 BitMask);
void Diag_DdrcSetModeRegWait(UINT32 DdrcId, UINT32 BitMask, UINT32 Val);
UINT32 Diag_DdrcIsSingleDie(UINT32 DdrcId, AMBA_SHELL_PRINT_f PrintFunc);

void Diag_DdrSetDLL(UINT32 DdrcId, UINT32 BitMask, UINT32 DllByte, UINT32 DllValue, AMBA_SHELL_PRINT_f PrintFunc);
UINT32 Diag_DdrcGetByteDly(UINT32 DdrcId, UINT32 DieId, UINT32 ByteNum, UINT32 Index);
UINT32 Diag_DdrcGetDqVref(UINT32 DdrcId, UINT32 ByteNum);
UINT32 Diag_DdrcGetDqsVrefPerByte(UINT32 DdrcId, UINT32 ByteNum);
void Diag_DdrcSetByteDly(UINT32 DdrcId, UINT32 DieId, UINT32 ByteNum, UINT32 Index, UINT32 Val);
void Diag_DdrcSetDqVref(UINT32 DdrcId, UINT32 ByteNum, UINT32 Val);
void Diag_DdrcSetDqsVrefPerByte(UINT32 DdrcId, UINT32 ByteNum, UINT32 Val);
UINT32 Diag_DdrGetLp4Vref(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrGetLp4MR14(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrGetLp4MR12(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrGetLp4WriteDly(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrGetDqsVref(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrGetWckDly(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrGetDqsWriteDly(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrGetDqsGateDly(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrGetCkDly(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrGetCkeDly(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrGetCaDly(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrGetLp5MR14(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrGetLp5MR15(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrGetLp5MR12(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrGetLp4ReadDly(UINT32 DdrcId, UINT32 BitMask);
UINT32 Diag_DdrcGetMisc2(UINT32 DdrcId);
void Diag_DdrSetLp4ReadDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrSetLp4Vref(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrSetLp4MR14(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrSetLp4MR12(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrSetLp4WriteDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrSetDqsVref(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrSetWckDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrSetDqsWriteDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrSetDqsGateDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrSetCkDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrSetCkeDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrSetCaDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrSetLp5MR14(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrSetLp5MR15(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrSetLp5MR12(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc);
void Diag_DdrcSetMisc2(UINT32 DdrcId, UINT32 Val);

#endif