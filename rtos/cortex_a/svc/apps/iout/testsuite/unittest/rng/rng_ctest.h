#include <threadx/amba_osal.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);

INT32 rng_ctest_init_main(void);
INT32 rng_ctest_dataget_main(void);
INT32 rng_ctest_deinit_main(void);

extern void IO_UtilityRegWrite32(UINT64 Address, UINT32 Value);
extern UINT32 IO_UtilityRegRead32(UINT64 Address);
extern void AmbaDelayCycles(UINT32 Delay);
extern void AmbaMisra_TouchUnused(void * pArg);
