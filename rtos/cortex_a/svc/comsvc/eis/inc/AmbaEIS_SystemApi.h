#ifndef AMBA_EIS_SYS_API_H
#define AMBA_EIS_SYS_API_H


#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"

#include "AmbaErrorCode.h"
#include "AmbaFS.h"

INT32 AmbaEis_DbToS32(DOUBLE in);

UINT32 AmbaEis_Save_File(const char *FileName, UINT32 Size, void *Buf);
UINT32 AmbaEis_Load_file(const char *FileName, UINT32 Size, void *Buf);


UINT32 AmbaEis_StringToInt32 (const char *nptr, INT32 *pValue);
void AmbaEis_Log_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4,UINT32 Arg5);
void AmbaEis_Log_STR_DBG(const char *pModule, const char *pFormat, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);
void AmbaEis_Log_INT_DBG(const char *pModule, const char *pFormat, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
void AmbaEis_Log_NG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4,UINT32 Arg5);
void AmbaEis_Log_FT_DBG(const char *pModule, const char *pFormat, FLOAT Value1, FLOAT Value2, UINT32 Precision);
void AmbaEis_Log_DB_DBG(const char *pModule, const char *pFormat, DOUBLE Value1, DOUBLE Value2, UINT32 Precision);



#endif
