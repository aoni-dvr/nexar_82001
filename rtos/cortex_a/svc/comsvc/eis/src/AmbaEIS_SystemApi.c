
#include "AmbaEIS_SystemApi.h"
#include "AmbaMisraFix.h"


#define EIS_LONG_MAX    (0x7FFFFFFFU)
#define EIS_LONG_MIN    (0x80000000U)

#define DC_U 99999UL
#define DC_S NULL

#define LOG_BUF_SIZE            512U
#define LOG_ARGC                5U

//#define AmbaMisra_TypeCast32(a, b) AmbaWrap_memcpy(a, b, sizeof(void *))
//#define AmbaFS_FileOpen(a, b, c)   fopen(a, b);
//#define AmbaFS_FileWrite(a, b, c, d, e) fwrite(a, b, c, d);
//#define AmbaFS_FileClose(a) fclose(a)


#define EIS_SprintfStr      AmbaUtility_StringPrintStr

#define LOG_EIS_SysApi            "EIS_SysApi"

static inline INT32 AmbaEis_Casting_char_to_int32(char source_char) //For CERT
{
    INT32 dst_int32 = 0;
    if(0U != AmbaWrap_memcpy(&dst_int32, &source_char, sizeof(char))) {
        AmbaEis_Log_NG(LOG_EIS_SysApi,"AmbaWrap_memcpy dst_int32 Error!!",0U, 0U, 0U, 0U, 0U);
    }
    return dst_int32;
}


INT32 AmbaEis_DbToS32(DOUBLE in){
    DOUBLE out;
    if (in >= 0.0) {
        out = (in + 0.5);
    } else {
        out = (in - 0.5);
    }

    return (INT32)out;
}


static UINT32 AmbaEis_IsSpace(char c)
{
    UINT8 cval = (UINT8)c;
    UINT32 Rval;
    if (((cval >= 9U) && (cval <= 13U)) || (cval == 32U)) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}

static UINT32 AmbaEis_IsDigit(char c)
{
    UINT8 cval = (UINT8)c;
    UINT32 Rval;
    if ((cval >= 48U) && (cval <= 57U)) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}

static UINT32 AmbaEis_IsAlpha(char c)
{
    UINT8 cval = (UINT8)c;
    UINT32 Rval;
    if (((cval >= 65U) && (cval <= 90U)) || ((cval >= 97U) && (cval <= 122U))) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}

static UINT32 AmbaEis_IsUpper(char c)
{
    UINT8 cval = (UINT8)c;
    UINT32 Rval;
    if ((cval >= 65U) && (cval <= 90U)) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}



static INT64 AmbaEis_StringToLong(const char *nptr, char **endptr, INT32 base)
{
    register const char *s = nptr;
    UINT32 IdxS = 0;
    register LONG acc;
    register INT32 c;
    register LONG cutoff;
    register INT32 neg = 0, any, cutlim;
    UINT32 MisraCBreakFlag = 0U;
    ULONG MisraULTmp;
    LONG MisraLTmp;
    INT32 MisraBase = base;
    do {
        c = AmbaEis_Casting_char_to_int32(s[IdxS]);
        IdxS++;
    } while (AmbaEis_IsSpace((char)c) == 1U);
    if (c == (INT32)'-') {
        neg = 1;
        c = AmbaEis_Casting_char_to_int32(s[IdxS]);
        IdxS++;
    } else if (c == (INT32)'+') {
        c = AmbaEis_Casting_char_to_int32(s[IdxS]);
        IdxS++;
    } else {
        // Do nothing
    }
    if (((MisraBase == 0) || (MisraBase == 16)) && (c == (INT32)'0') && ((s[IdxS] == 'x') || (s[IdxS] == 'X'))) {
        c = AmbaEis_Casting_char_to_int32(s[IdxS + 1U]);
        IdxS += 2U;
        MisraBase = 16;
    }

    if (MisraBase == 0) {
        if (c == (INT32)'0') {
            MisraBase = 8;
        } else {
            MisraBase = 10;
        }
    }
    if (neg != 0) {
        MisraLTmp = (LONG)EIS_LONG_MIN;
        cutoff = -MisraLTmp;
    } else {
        cutoff = (LONG)EIS_LONG_MAX;
    }
    cutlim = cutoff % (LONG)MisraBase;
    MisraULTmp = (ULONG)cutoff / (ULONG)MisraBase;
    cutoff = (LONG)MisraULTmp;
    acc = 0;
    any = 0;
    for(;;) {
        if (AmbaEis_IsDigit((char)c) != 0U) {
            c = c - (INT32)'0';
        } else if (AmbaEis_IsAlpha((char)c) != 0U) {
            if (AmbaEis_IsUpper((char)c) != 0U) {
                c = c - (INT32)'A' + 10;
            } else {
                c = c - (INT32)'a' + 10;
            }
        } else {
            MisraCBreakFlag = 1U;
        }
        if (MisraCBreakFlag == 0U) {
            if (c >= MisraBase) {
                MisraCBreakFlag = 1U;
            } else {
                if ((any < 0) || (acc > cutoff) || ((acc == cutoff) && (c > cutlim))) {
                    any = -1;
                } else {
                    any = 1;
                    acc *= (LONG)MisraBase;
                    acc += (LONG)c;
                }
                c = AmbaEis_Casting_char_to_int32(s[IdxS]);
                IdxS++;
            }
        }
        if (MisraCBreakFlag != 0U) {
            break;
        }
    }
    if (any < 0) {
        if (neg != 0) {
            acc = (LONG)EIS_LONG_MIN;
        } else {
            acc = (LONG)EIS_LONG_MAX;
        }
    } else if (neg != 0) {
        acc = -acc;
    } else {
        // Do nothing
    }
    if (endptr != NULL) {
        if (any == 0) {
            
            if(0U != AmbaWrap_memcpy(endptr, &s[IdxS - 1U], sizeof(char *))){
                AmbaEis_Log_NG(LOG_EIS_SysApi,"AmbaWrap_memcpy endptr Error!!",0U, 0U, 0U, 0U, 0U);
            }
        }
        else {
            if(0U != AmbaWrap_memcpy(endptr, nptr, sizeof(char *))){
                AmbaEis_Log_NG(LOG_EIS_SysApi,"AmbaWrap_memcpy endptr Error!!",0U, 0U, 0U, 0U, 0U);
            }
        }

    }
    return (INT64)(acc);
}



UINT32 AmbaEis_StringToInt32 (const char *nptr, INT32 *pValue)
{
    INT32 Ret = 0;
    char end_of_str = '\0';
    char *atoi_sep = &end_of_str;


    SIZE_t Length = AmbaUtility_StringLength(nptr);
    if ((nptr == NULL)||(Length == 0UL)){
        Ret = 1;
    }
    else{
        Ret = (INT32) AmbaEis_StringToLong (nptr, &atoi_sep, 10);
    }

    *pValue = Ret;

  return 0;
}


void AmbaEis_Log_FT_DBG(const char *pModule, const char *pFormat, FLOAT Value1, FLOAT Value2, UINT32 Precision)
{

    char LogBuf[LOG_BUF_SIZE];
    char FloatString1[UTIL_MAX_FLOAT_STR_LEN];
    char FloatString2[UTIL_MAX_FLOAT_STR_LEN];
    UINT32      Rval, Argc;
    const char  *SringLog[LOG_ARGC];

    Argc = 0;
    SringLog[Argc] = pModule;
    Argc++;
    SringLog[Argc] = pFormat;
    Argc++;

    (void)AmbaUtility_FloatToStr(FloatString1, UTIL_MAX_FLOAT_STR_LEN, Value1, Precision);
    (void)AmbaUtility_FloatToStr(FloatString2, UTIL_MAX_FLOAT_STR_LEN, Value2, Precision);

    Rval = EIS_SprintfStr(LogBuf, LOG_BUF_SIZE, "[%s|DBG]: %s", Argc, SringLog);
    if (Rval < LOG_BUF_SIZE) {
    AmbaPrint_PrintStr5(LogBuf, FloatString1, FloatString2, NULL, NULL, NULL);
    }

    
}


void AmbaEis_Log_DB_DBG(const char *pModule, const char *pFormat, DOUBLE Value1, DOUBLE Value2, UINT32 Precision)
{

    char LogBuf[LOG_BUF_SIZE];
    char String1[UTIL_MAX_FLOAT_STR_LEN];
    char String2[UTIL_MAX_FLOAT_STR_LEN];
    UINT32 Rval, Argc;
    const char *SringLog[LOG_ARGC];

    Argc = 0;
    SringLog[Argc] = pModule;
    Argc++;
    SringLog[Argc] = pFormat;
    Argc++;

    (void)AmbaUtility_DoubleToStr(String1, UTIL_MAX_FLOAT_STR_LEN, Value1, Precision);
    (void)AmbaUtility_DoubleToStr(String2, UTIL_MAX_FLOAT_STR_LEN, Value2, Precision);

    Rval = EIS_SprintfStr(LogBuf, LOG_BUF_SIZE, "[%s|DBG]: %s", Argc, SringLog);
    if (Rval < LOG_BUF_SIZE) {
    AmbaPrint_PrintStr5(LogBuf, String1, String2, NULL, NULL, NULL);
    }

    
}


void AmbaEis_Log_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4,UINT32 Arg5)
{

    char        LogBuf[LOG_BUF_SIZE];
    const char  *SringLog[LOG_ARGC];
    UINT32      Rval, Argc;

    Argc = 0;
    SringLog[Argc] = pModule;
    Argc++;
    SringLog[Argc] = pFormat;
    Argc++;

    Rval = EIS_SprintfStr(LogBuf, LOG_BUF_SIZE, "[%s|DBG]: %s", Argc, SringLog);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

void AmbaEis_Log_NG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4,UINT32 Arg5)
{

    char        LogBuf[LOG_BUF_SIZE];
    const char  *SringLog[LOG_ARGC];
    UINT32      Rval, Argc;

    Argc = 0;
    SringLog[Argc] = pModule;
    Argc++;
    SringLog[Argc] = pFormat;
    Argc++;

    Rval = EIS_SprintfStr(LogBuf, LOG_BUF_SIZE, "[%s|NG]: %s", Argc, SringLog);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}



void AmbaEis_Log_STR_DBG(const char *pModule, const char *pFormat, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{

    char        LogBuf[LOG_BUF_SIZE];
    const char  *SringLog[LOG_ARGC];
    UINT32      Rval, Argc;

    Argc = 0;
    SringLog[Argc] = pModule;
    Argc++;
    SringLog[Argc] = pFormat;
    Argc++;

    Rval = EIS_SprintfStr(LogBuf, LOG_BUF_SIZE, "[%s|DBG]: %s", Argc, SringLog);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintStr5(LogBuf, pArg1, pArg2, pArg3, pArg4, pArg5);
    }
}

void AmbaEis_Log_INT_DBG(const char *pModule, const char *pFormat, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{

    char        LogBuf[LOG_BUF_SIZE];
    const char  *SringLog[LOG_ARGC];
    UINT32      Rval, Argc;

    Argc = 0;
    SringLog[Argc] = pModule;
    Argc++;
    SringLog[Argc] = pFormat;
    Argc++;

    Rval = EIS_SprintfStr(LogBuf, LOG_BUF_SIZE, "[%s|DBG]: %s", Argc, SringLog);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintInt5(LogBuf, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}




UINT32 AmbaEis_Load_file(const char *FileName, UINT32 Size, void *Buf)
{
    AMBA_FS_FILE *Fid = NULL;
    UINT32 Ret = 0;
    UINT32 MisraU32;

    AmbaMisra_TypeCast32(&MisraU32, &Buf);
    AmbaEis_Log_DBG(LOG_EIS_SysApi, "buf: %d, size: %d", MisraU32, Size, DC_U, DC_U, DC_U);
    (void)AmbaFS_FileOpen(FileName,"rb", &Fid);
    if (Fid == NULL) {
        AmbaEis_Log_STR_DBG(LOG_EIS_SysApi, "call AmbaFS_fopen(%s) Fail", FileName, DC_S, DC_S, DC_S, DC_S);
        Ret = 1U;
    } else {
        (void)AmbaFS_FileRead(Buf, 1, Size, Fid, &MisraU32);
        if (MisraU32 > 0U) {
        }
        else {
            AmbaEis_Log_DBG(LOG_EIS_SysApi, "call AmbaFS_fread() Fail, Buf: %d, Size: %d", MisraU32, Size, DC_U, DC_U, DC_U);
            Ret = 2U;
        }

        (void)AmbaFS_FileClose(Fid);
    }
    return Ret;
}


UINT32 AmbaEis_Save_File(const char *FileName, UINT32 Size, void *Buf)
{
    AMBA_FS_FILE *Fid = NULL;
    UINT32 Ret = 0;
    UINT32 MisraU32;


    (void)AmbaFS_FileOpen(FileName, "wb", &Fid);
    if (Fid == NULL) {
        AmbaEis_Log_STR_DBG(LOG_EIS_SysApi,"call AmbaEis_FileOpen(%s) Fail", FileName, DC_S, DC_S, DC_S, DC_S);
        Ret = 1;
    } else {
        (void)AmbaFS_FileWrite(Buf, Size, 1, Fid, &MisraU32);
        if (MisraU32 > 0U) {

        }
        else {
            AmbaMisra_TypeCast32(&MisraU32, &Buf);
            AmbaEis_Log_DBG(LOG_EIS_SysApi,"call AmbaFS_fwrite() Fail, Buf: %d, Size: %d", MisraU32, Size, DC_U, DC_U, DC_U);
            Ret = 2;
        }

        (void)AmbaFS_FileClose(Fid);
    }

    return Ret;
}






/*UINT32 AmbaEis_StringToUInt32 (const char *nptr, INT32 *pValue)
{

    return = AmbaUtility_StringToUInt32(nptr, pValue);
}
*/






