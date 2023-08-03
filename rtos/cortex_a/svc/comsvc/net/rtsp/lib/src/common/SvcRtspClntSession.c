/**
 *  @file SvcRtspClntSession.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details rtsp client session module
 *
 */

#include "AmbaTypes.h"
#include <AmbaKAL.h>
#include "AmbaPrint.h"
#include "AmbaRTC.h"
#if defined(CONFIG_NETX_ENET)
#include "NetStack.h"
#include "NetXStack.h"
#include "nx_api.h"
#include "nx_bsd.h"
#elif defined(CONFIG_LWIP_ENET)
#define AMBA_ERROR_CODE_H
#define LWIP_PATCH_MISRA   // for misra depress
#include "lwip/sockets.h"
#include "lwip/errno.h"
#elif defined(CONFIG_LINUX)
#include <arpa/inet.h>
#endif

#include "SvcRtspServer.h"
#include "SvcRtspRandom.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"

#if defined(CONFIG_LWIP_ENET)
#define IP4ADDR_STRLEN_MAX  16
typedef struct {
    UINT32 addr;
} ip4_addr_t;

static CHAR* ip4addr_ntoa_r(const ip4_addr_t *addr, CHAR *buf, INT32 buflen)
{
    UINT32 s_addr;
    CHAR inv[3];
    CHAR *rp;
    UINT8 *ap;
    UINT8 rem;
    UINT32 n;
    UINT32 i;
    INT32 len = 0;
    UINT32 RetVal = OK;
    CHAR *RetBuf = NULL;
    ULONG SrcUL;

    s_addr = addr->addr;

    rp = buf;
    ap = (UINT8 *)&s_addr;

    for (n = 0U; n < 4U; n++) {
        i = 0U;

        do {
            rem = *ap % (UINT8)10;
            *ap /= (UINT8)10;
            inv[i] = (CHAR)('0' + rem);
            i++;
        } while ((UINT8)0 != *ap);

        while (0U != i--) {
            if (len++ >= buflen) {
                RetVal = ERR_NA;
                break;
            }

            *rp = inv[i];
            rp = &rp[1];
        }

        if ((ERR_NA == RetVal) || (len >= buflen)) {
            RetVal = ERR_NA;
            break;
        }
        len++;

        *rp = '.';
        rp = &rp[1];
        ap = &ap[1];
    }

    if (ERR_NA != RetVal) {
        AmbaMisra_TypeCast(&SrcUL, &rp);
        SrcUL -= 1U;
        AmbaMisra_TypeCast(&rp, &SrcUL);
        *rp = '\0';
        RetBuf = buf;
    }

    return RetBuf;
}

static CHAR* ip4addr_ntoa(const ip4_addr_t *addr)
{
  static CHAR str[IP4ADDR_STRLEN_MAX];

  return ip4addr_ntoa_r(addr, str, IP4ADDR_STRLEN_MAX);
}

static CHAR* inet_ntoa(ip4_addr_t addr)
{
    return ip4addr_ntoa((const ip4_addr_t*)&(addr));
}
#endif

static UINT32 IsDigital(const char c)
{
    UINT32 Rval = 0U;

    switch((UINT8)c) {
        case (UINT8)'0' :
        case (UINT8)'1' :
        case (UINT8)'2' :
        case (UINT8)'3' :
        case (UINT8)'4' :
        case (UINT8)'5' :
        case (UINT8)'6' :
        case (UINT8)'7' :
        case (UINT8)'8' :
        case (UINT8)'9' :
            Rval = 1U;
            break;
        default :
            Rval = 0U;
            break;
    }

    return Rval;
}

static DOUBLE StrTod(const char *pStr)
{
    static const INT32 maxExponent = 511;
    static const DOUBLE ExpLtb[] = {
        10.0,
        100.0,
        1.0e4,
        1.0e8,
        1.0e16,
        1.0e32,
        1.0e64,
        1.0e128,
        1.0e256
    };
    static INT32 MaxMantissaNum = 18;
    UINT32 ErrCode = 0U;
    DOUBLE Rval = 0.0;

    if (pStr != NULL) {

        INT32  Idx = 0, StartIdx, DecIdx = 0;
        INT32  IsNagtive = 0, ExpIsNagtive = 0;
        INT32  Mantissa = 0, ExpVal = 0, Tmp;
        DOUBLE Exp = 0.0;

        // Pass front-space char
        {
            while (pStr[Idx] == ' ') {
                 Idx += 1;
            }
        }

        // Check the sign char
        {
            if (pStr[Idx] == '-') {
                IsNagtive = 1;
                Idx += 1;
            } else {
                if (pStr[Idx] == '+') {
                    Idx += 1;
                } else {
                    if ((0U == IsDigital(pStr[Idx])) && // not digital
                        (pStr[Idx] == '.')) {                   // not decimal point
                        ErrCode = 1U;
                    }
                }
                IsNagtive = 0;
            }
        }

        // Get the start index
        StartIdx = Idx;

        // Get the MANTISSA and Decimal Point
        if (ErrCode == 0U) {
            DecIdx   = -1;
            Mantissa = 0;

            while (pStr[Idx] != '\0') {

                if (0U == IsDigital(pStr[Idx])) {

                    if ((pStr[Idx] != '.') ||   // not decimal point. maybe 'e', 'E' or '\0'
                        (DecIdx >= 0)) {        // format : '-.' case
                        break;
                    }

                    DecIdx = Mantissa;
                }

                Mantissa ++;
                Idx ++;
            }

            if (Mantissa <= 0) {
                ErrCode = 1;
            }
        }

        // Get the exponent
        if (ErrCode == 0U) {
            INT32 fracExp;

            if (DecIdx < 0) {
                DecIdx = Mantissa;
            } else {
                Mantissa -= 1;      // reject the decimal point from mantissa part
            }

            // The mantissa part is too larger to ignore others.
            if (Mantissa > MaxMantissaNum) {
                fracExp = DecIdx - MaxMantissaNum;
                Mantissa = MaxMantissaNum;
            } else {
                fracExp = DecIdx - Mantissa;
            }

            // Check 'e' or 'E' char
            if ((pStr[Idx] == 'e') || (pStr[Idx] == 'E')) {
                Idx ++;

                if (pStr[Idx] == '-') {
                    ExpIsNagtive = 1;
                    Idx += 1;
                } else {
                    if (pStr[Idx] == '+') {
                        Idx += 1;
                    } else {
                        if (0U == IsDigital(pStr[Idx])) {
                            ErrCode = 1U;
                        }
                    }
                    ExpIsNagtive = 0;
                }

                if (ErrCode == 0U) {
                    while (1U == IsDigital(pStr[Idx])) {
                        Tmp = (INT32)(UINT8)pStr[Idx];
                        Tmp -= (INT32)(UINT8)'0';
                        ExpVal = ( ExpVal * 10 ) + Tmp;
                        Idx ++;
                    }
                }
            }

            if (ErrCode == 0U) {
                const DOUBLE *pTbl;

                if (ExpIsNagtive == 1) {
                    ExpVal = fracExp - ExpVal;
                } else {
                    ExpVal = fracExp + ExpVal;
                }

                if (ExpVal < 0) {
                    ExpIsNagtive = 1;
                    ExpVal = -ExpVal;
                } else {
                    ExpIsNagtive = 0;
                }
                if (ExpVal > maxExponent) {
                    ExpVal = maxExponent;
                }

                pTbl = ExpLtb;
                Exp = 1.0;
                while (ExpVal != 0) {
                    if ( ( ExpVal % 2 ) > 0 ) {
                        Exp *= *(pTbl);
                    }
                    pTbl++;
                    ExpVal /= 2;
                }
            }
        }

        // Calculate the value by MANTISSA part.
        if (ErrCode == 0U) {
            INT32 Frac1 = 0, Frac2 = 0;

            Idx = StartIdx;

            while (Mantissa > 9) {
                if (pStr[Idx] == '.') {
                    Idx ++;
                }

                Tmp = (INT32)(UINT8)pStr[Idx];
                Tmp -= (INT32)(UINT8)'0';
                Frac1 = ( Frac1 * 10 ) + Tmp;

                Idx ++;
                Mantissa --;
            }

            while (Mantissa > 0) {
                if (pStr[Idx] == '.') {
                    Idx ++;
                }

                Tmp = (INT32)(UINT8)pStr[Idx];
                Tmp -= (INT32)(UINT8)'0';
                Frac2 = ( Frac2 * 10 ) + Tmp;

                Idx ++;
                Mantissa --;
            }

            Rval = ( 1.0e9 * (DOUBLE)(Frac1) ) + (DOUBLE)(Frac2);
            if (IsNagtive == 1) {
                Rval = -Rval;
            }
        }

        // Calculate the value by exponent
        if (ErrCode == 0U) {
            if (Exp > 0.0) {
                if (ExpIsNagtive == 1) {
                    Rval /= Exp;
                } else {
                    Rval *= Exp;
                }
            }
        }
    }

    return Rval;
}

static char *StrStr(const char *pString1, const char *pString2)
{
    char *pStr = NULL;

    if ((pString1 != NULL) && (pString2 != NULL)) {
        UINT32 CmpIdx = 0U, IsHit = 0U;
        UINT32 Str1Idx = 0U, Str2Idx = 0U;

        while ( pString1[CmpIdx] != '\0' ) {
            Str1Idx = CmpIdx;
            Str2Idx = 0;

            while ( (pString1[Str1Idx] != '\0' ) &&
                    (pString2[Str2Idx] != '\0' ) &&
                    (pString1[Str1Idx] == pString2[Str2Idx]) ) {
                Str1Idx += 1U;
                Str2Idx += 1U;
                IsHit = 1U;
            }

            if ( pString2[Str2Idx] != '\0' ) {
                CmpIdx += 1U;
                IsHit = 0U;
            } else {
                break;
            }
        }

        if (IsHit == 1U) {
            struct { union { char *pChar; const char *pConstChar; } d; } Cvt;
            Cvt.d.pConstChar = &(pString1[CmpIdx]);
            pStr = Cvt.d.pChar;
        }
    }

    return pStr;
}

#define DAYS_PER_WEEK   7U
#define MONS_PER_YEAR   12U

static UINT32 ToLower(CHAR c)
{
    UINT32  Ch = (UINT8)c;

    if (((c) >= 'A') && ((c) <= 'Z')) {
        Ch += 0x20U;
    }

    return Ch;
}

static UINT32 StrNCaseCmp(const CHAR* Str1, const CHAR* Str2, UINT32 StrSize)
{
    UINT32 RetVal = 0U, Idx;
    CHAR Char1 = '\0', Char2 = '\0';

    if (0U != StrSize) {
        for (Idx=0U; Idx<StrSize; Idx++) {
            Char1 = Str1[Idx];
            Char2 = Str2[Idx];

            if ((('\0'==Char1) || ('\0'==Char2)) || \
                (ToLower(Char1) != ToLower(Char2))) {
                break;
            }
        }
        RetVal = ToLower(Char1) - ToLower(Char2);
    } else {
        //Do nothing
    }
    return RetVal;
}

static const CHAR* AllowedCommands = "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, GET_PARAMETER, SET_PARAMETER";

static void ParseTransportHeader(const CHAR *Buf,
                 UINT32 *StreamingMode,
                 CHAR **StreamingModeString,
                 CHAR *DestinationAddressStr,
                 UINT8 *DestinationTTL,
                 UINT16 *ClientRTPPortNum,
                 UINT16 *ClientRTCPPortNum,
                 UINT8 *RtpChannelId,
                 UINT8 *RtcpChannelId
                 )
{
    const CHAR *Fields;
    const CHAR *pBuf;
    CHAR Field[64], Str1[64];
    UINT32 p1 = 0U, p2 = 0U, idx, idx2;
    UINT32 ttl = 0U, rtpCid = 0U, rtcpCid = 0U, RetVal = ERR_NA;
    UINT32 i = 0U;
    ULONG  SrcAddr;

    (void)DestinationAddressStr;

    *StreamingMode = RTP_UDP;
    *StreamingModeString = NULL;
    *DestinationTTL = (UINT8)255;
    *ClientRTPPortNum = (UINT16)0;
    *ClientRTCPPortNum = (UINT16)1;
    *RtpChannelId = (UINT8)0xFF;
    *RtcpChannelId = (UINT8)0xFF;

    while ('\0' != Buf[i]) {
        if (StrNCaseCmp(&(Buf[i]), "Transport: ", 11) == 0U) {
            RetVal = OK;
            break;
        }
        i++;
    }

    if (OK == RetVal) {
        pBuf = &(Buf[i]);
        /* Then, run through each of the Fields, looking for ones we handle */
        AmbaMisra_TypeCast(&SrcAddr, &pBuf);
        SrcAddr += 11U;
        AmbaMisra_TypeCast(&Fields, &SrcAddr);

        AmbaPrint_PrintStr5("Buf %s, Fields %s", pBuf, Fields, NULL, NULL, NULL);
        {
            static CHAR Args[UTIL_MAX_STR_TOKEN_NUMBER][UTIL_MAX_STR_TOKEN_LENGTH] GNU_SECTION_NOZEROINIT;
            UINT32 ArgCount = 0U;

            if (AmbaUtility_StringToken(Fields, ';', Args, &ArgCount) == OK) {
                for (idx=0; idx<ArgCount; idx++) {
                    AmbaPrint_PrintStr5("arg %s", Args[idx], NULL, NULL, NULL, NULL);
                    {
                        if (AmbaUtility_StringCompare(Args[idx], "RTP/AVP/TCP", 11UL) == 0) {
                            *StreamingMode = RTP_TCP;
                        } else if (StrNCaseCmp(Args[idx], "destination=", 12U) == 0U) {
                            AmbaUtility_StringCopy(Field, 64U, Args[idx]);
                            AmbaUtility_StringCopy(DestinationAddressStr, 32U, &Field[12]);
                            AmbaPrint_PrintStr5("ParseTransportHeader, DestinationAddressStr %s", \
                                                DestinationAddressStr, NULL, NULL, NULL, NULL);
                        } else if (StrNCaseCmp(Args[idx], "ttl", 3U) == 0U) {
                            AmbaUtility_StringCopy(Field, 64U, Args[idx]);
                            AmbaPrint_PrintStr5("ParseTransportHeader, Field %s", Field, NULL, NULL, NULL, NULL);
                            (VOID)AmbaUtility_StringToUInt32(&Field[3], &ttl);
                            *DestinationTTL = (UINT8)ttl;
                        } else if (StrNCaseCmp(Args[idx], "client_port=", 12U) == 0U) {
                            AmbaUtility_StringCopy(Field, 64U, Args[idx]);
                            AmbaPrint_PrintStr5("ParseTransportHeader, Field %s", Field, NULL, NULL, NULL, NULL);
                            if (AmbaWrap_memset(Str1, 0, sizeof(Str1))!= 0U) { }

                            for (idx2=12U; Field[idx2]!='\0'; idx2++) {
                                if (Field[idx2] == '-') {
                                    break;
                                } else {
                                    Str1[idx2-12U] = Field[idx2];
                                }
                            }

                            Str1[idx2-12U] = '\0';
                            AmbaPrint_PrintStr5("ParseTransportHeader, Str1 %s", Str1, NULL, NULL, NULL, NULL);
                            RetVal = AmbaUtility_StringToUInt32(Str1, &p1);

                            if (Field[idx2] == '-') {
                                if (AmbaWrap_memset(Str1, 0, sizeof(Str1))!= 0U) { }
                                AmbaUtility_StringCopy(Str1, 64U, &Field[idx2+1U]);

                                AmbaPrint_PrintStr5("ParseTransportHeader, 2 Str1 %s", Str1, NULL, NULL, NULL, NULL);
                                RetVal = AmbaUtility_StringToUInt32(Str1, &p2);

                                *ClientRTPPortNum = (UINT16)p1;
                                p2 = p1+1U;
                                *ClientRTCPPortNum = (UINT16)((*StreamingMode == RAW_UDP) ? 0U : p2);
                                AmbaPrint_PrintUInt5("ClientRTCPPortNum %d, ClientRTPPortNum %d", \
                                                    *ClientRTCPPortNum, *ClientRTPPortNum, 0U, 0U, 0U);
                            } else {
                                *ClientRTPPortNum = (UINT16)p1;
                                *ClientRTCPPortNum = (UINT16)((*StreamingMode==RAW_UDP) ? 0U : (p1+1U));
                            }
                        } else if (StrNCaseCmp(Args[idx], "interleaved=", 12U) == 0U) {
                            AmbaUtility_StringCopy(Field, 64U, Args[idx]);
                            AmbaPrint_PrintStr5("ParseTransportHeader, Field %s", Field, NULL, NULL, NULL, NULL);

                            if (AmbaWrap_memset(Str1, 0, sizeof(Str1))!= 0U) { }
                            for (idx2=12U; Field[idx2]!='\0'; idx2++) {
                                if (Field[idx2] == '-') {
                                    break;
                                } else {
                                    Str1[idx2-12U] = Field[idx2];
                                }
                            }
                            Str1[idx2-12U] = '\0';

                            AmbaPrint_PrintStr5("ParseTransportHeader, 3 Str1 %s", Str1, NULL, NULL, NULL, NULL);
                            (VOID)AmbaUtility_StringToUInt32(Str1, &rtpCid);

                            if (Field[idx2] == '-') {
                                (VOID)AmbaUtility_StringToUInt32(&Field[idx2+1U], &rtcpCid);
                                AmbaPrint_PrintUInt5("rtcpCid %d", rtcpCid, 0U, 0U, 0U, 0U);
                            } else {
                                AmbaPrint_PrintUInt5("rtcpCid can't be found", rtcpCid, 0U, 0U, 0U, 0U);
                            }

                            *RtpChannelId = (UINT8)rtpCid;
                            *RtcpChannelId = (UINT8)rtcpCid;
                        }else {
                            // Do nothing
                        }
                    }
                }
            }
        }
    }
    (VOID)RetVal;
}

static UINT32 ParseRtspRequestString(CHAR const* ReqStr,
                   UINT32 ReqStrSize,
                   CHAR* ResultCmdName,
                   UINT32 ResultCmdNameMaxSize,
                   CHAR* ResultURLPreSuffix,
                   UINT32 ResultURLPreSuffixMaxSize,
                   CHAR* ResultURLSuffix,
                   UINT32 ResultURLSuffixMaxSize,
                   CHAR* ResultCSeq,
                   UINT32 ResultCSeqMaxSize,
                   UINT32* ContentLength)
{
    UINT32 ParseSucceeded;
    UINT32 i, j, k;
    CHAR StrTmp[64];
    UINT32 RetVal = OK;
    CHAR c;
    UINT32 k1, n, k2;
    UINT32 num;
    UINT32 idx;

    *ContentLength = 0;
    AmbaPrint_PrintStr5("%s, ReqStr %s", __func__, ReqStr, NULL, NULL, NULL);

    /* Read everything up to the first space as the command name */
    ParseSucceeded = 0U;
    for (i=0U; (i<(ResultCmdNameMaxSize-1U)) && (i<ReqStrSize); ++i) {
        c = ReqStr[i];
        if ((c==' ') || (c=='\t')) {
            ParseSucceeded = 1;
            break;
        }

        ResultCmdName[i] = c;
    }
    ResultCmdName[i] = '\0';

    if (0U != ParseSucceeded) {
        j = i+1U;
        while ((j<ReqStrSize) && ((ReqStr[j]==' ') || (ReqStr[j]=='\t'))) {
            ++j;
        }

        while (j < (ReqStrSize - 8U)) {
            if (((ReqStr[j]=='r') || (ReqStr[j]=='R')) && \
                ((ReqStr[j+1U]=='t') || (ReqStr[j+1U]=='T')) && \
                ((ReqStr[j+2U]=='s') || (ReqStr[j+2U]=='S')) && \
                ((ReqStr[j+3U]=='p') || (ReqStr[j+3U]=='P')) && \
                (ReqStr[j+4U]==':') && (ReqStr[j+5U]=='/')) {
                j += 6U;
                if (ReqStr[j] == '/') {
                    ++j;

                    while ((j<ReqStrSize) && (ReqStr[j]!='/') && (ReqStr[j]!=' ')) {
                        ++j;
                    }
                } else {
                    --j;
                }

                i = j;
                break;
            }
            j++;
        }

        /* Look for the URL suffix (before the following "RTSP/") */
        ParseSucceeded = 0U;
        k = i + 1U;
        while (k < (ReqStrSize - 5U)) {
            if ((ERR_NA==RetVal) || (1U==ParseSucceeded)) {
                break;
            }

            if (((ReqStr[k]=='R') && (ReqStr[k+1U]=='T')) && \
                ((ReqStr[k+2U]=='S') && (ReqStr[k+3U]=='P')) && \
                (ReqStr[k+4U]=='/')) {
                --k;
                while ((k>=i) && (ReqStr[k]==' ')) {
                    /* go back over all spaces before "RTSP/" */
                    --k;
                }

                k1 = k;
                while ((k1>i) && (ReqStr[k1]!='/')) {
                  --k1;
                }

                if (((k-k1) + 1U) > ResultURLSuffixMaxSize) {
                    RetVal = ERR_NA;
                    continue;
                }

                n = 0U;
                k2 = k1+1U;
                while (k2 <= k) {
                    ResultURLSuffix[n] = ReqStr[k2];
                    n++;
                    k2++;
                }
                ResultURLSuffix[n] = '\0';

                if (k1 > (ResultURLPreSuffixMaxSize+i)) {
                    RetVal = ERR_NA;
                    continue;
                }

                n = 0U;
                k2 = i+1U;
                while (k2 <= (k1-1U)) {
                    ResultURLPreSuffix[n] = ReqStr[k2];
                    n++;
                    k2++;
                }
                ResultURLPreSuffix[n] = '\0';

                i = k + 7U;
                ParseSucceeded = 1U;
            }
            k++;
        }

        if (ERR_NA != RetVal) {
            if (ParseSucceeded == 0U) {
                RetVal = ERR_NA;
            } else {
                ParseSucceeded = 0U;
                j = i;
                while (j < (ReqStrSize - 5U)) {
                    if ((((ReqStr[j]=='C') && (ReqStr[j+1U]=='S')) && \
                        ((ReqStr[j+2U]=='e') && (ReqStr[j+3U]=='q'))) && \
                        (ReqStr[j+4U]==':')) {
                        j += 5U;

                        while ((j<ReqStrSize) && ((ReqStr[j]==' ') || (ReqStr[j] == '\t'))) {
                            ++j;
                        }

                        n = 0U;
                        while ((n < (ResultCSeqMaxSize - 1U)) && (j < ReqStrSize)) {
                            c = ReqStr[j];
                            if ((c=='\r') || (c=='\n')) {
                              ParseSucceeded = 1;
                              break;
                            }

                            ResultCSeq[n] = c;
                            ++j;
                            n++;
                        }

                        ResultCSeq[n] = '\0';
                        break;
                    }
                    j++;
                }

                if (ParseSucceeded == 0U) {
                    RetVal = ERR_NA;
                } else {
                    j = i;
                    while (j < (ReqStrSize - 15U)) {
                        if ((ReqStr[j]=='C') &&
                            (ReqStr[j + 1U]=='o') &&
                            (ReqStr[j + 2U]=='n') &&
                            (ReqStr[j + 3U]=='t') &&
                            (ReqStr[j + 4U]=='e') &&
                            (ReqStr[j + 5U]=='n') &&
                            (ReqStr[j + 6U]=='t') &&
                            (ReqStr[j + 7U]=='-') &&
                            ((ReqStr[j + 8U]=='L') || (ReqStr[j + 8U] == 'l')) &&
                            (ReqStr[j + 9U]=='e') &&
                            (ReqStr[j + 10U]=='n') &&
                            (ReqStr[j + 11U]=='g') &&
                            (ReqStr[j + 12U]=='t') &&
                            (ReqStr[j + 13U]=='h') &&
                            (ReqStr[j + 14U]==':')) {
                            j += 15U;
                            while ((j<ReqStrSize) && ((ReqStr[j]== ' ') || (ReqStr[j]=='\t'))) {
                                ++j;
                            }

                            AmbaPrint_PrintStr5("%s, 2, ReqStr %s", __func__, (const CHAR*)&ReqStr[j], NULL, NULL, NULL);
                            {
                                idx = j;
                                while ((idx<ReqStrSize) && (ReqStr[idx]!=' ') && (ReqStr[idx]!='\t')) {
                                    StrTmp[idx] = ReqStr[idx];
                                    ++idx;
                                }
                                StrTmp[idx] = '\0';

                                RetVal = AmbaUtility_StringToUInt32(&StrTmp[0], &num);
                                if (OK == RetVal) {
                                    *ContentLength = num;
                                    AmbaPrint_PrintUInt5("ContentLength %d", *ContentLength, 0U, 0U, 0U, 0U);
                                }
                            }
                        }
                        j++;
                    }
                }
            }
        }
    }

    return RetVal;
}

static UINT32 TmAbsTime(const AMBA_RTC_DATE_TIME_s *pCalendar, CHAR *Buf, UINT32 Size)
{
    // static const CHAR format[] = "Date: %3s, %3s %02d %04d %02d:%02d:%02d GMT\r\n";
    UINT32 RetVal = OK;
    static CHAR const AbWeekdayName[][4] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    static CHAR const AbMonthName[][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    if (pCalendar == NULL) {
        RetVal = ERR_NA;
    } else {
        if (AmbaWrap_memset(Buf, 0, Size)!= 0U) { }

        // (void)AmbaAdvSnPrint(Buf, Size, format, ((pCalendar->WeekDay>= DAYS_PER_WEEK) ? "???" : AbWeekdayName[pCalendar->WeekDay]),
        //                      (((pCalendar->Month==0U) || (pCalendar->Month>MONS_PER_YEAR)) ? "???" : AbMonthName[pCalendar->Month-1U]),
        //                      pCalendar->Day, pCalendar->Year, pCalendar->Hour, pCalendar->Minute, pCalendar->Second);
        {
            char    *StrBuf = Buf;
            UINT32  BufSize = Size;
            UINT32  CurStrLen;

            AmbaUtility_StringAppend(StrBuf, BufSize, "Date: ");
            AmbaUtility_StringAppend(StrBuf, BufSize, (((pCalendar->WeekDay>= DAYS_PER_WEEK) ? "???" : AbWeekdayName[pCalendar->WeekDay])));
            AmbaUtility_StringAppend(StrBuf, BufSize, ", ");
            AmbaUtility_StringAppend(StrBuf, BufSize, ((((pCalendar->Month==0U) || (pCalendar->Month>MONS_PER_YEAR)) ? "???" : AbMonthName[pCalendar->Month-1U])));
            AmbaUtility_StringAppend(StrBuf, BufSize, " ");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                char    NumBuf[16];
                UINT32  NumLen;

                if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(pCalendar->Day), 10U);
                NumLen = AmbaUtility_StringLength(NumBuf);

                while (NumLen < 2UL) {
                    AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                    NumLen++;
                }

                AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, " ");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                char    NumBuf[16];
                UINT32  NumLen;

                if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(pCalendar->Year), 10U);
                NumLen = AmbaUtility_StringLength(NumBuf);

                while (NumLen < 4UL) {
                    AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                    NumLen++;
                }

                AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, " ");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                char    NumBuf[16];
                UINT32  NumLen;

                if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(pCalendar->Hour), 10U);
                NumLen = AmbaUtility_StringLength(NumBuf);

                while (NumLen < 2UL) {
                    AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                    NumLen++;
                }

                AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, ":");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                char    NumBuf[16];
                UINT32  NumLen;

                if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(pCalendar->Minute), 10U);
                NumLen = AmbaUtility_StringLength(NumBuf);

                while (NumLen < 2UL) {
                    AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                    NumLen++;
                }

                AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, ":");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                char    NumBuf[16];
                UINT32  NumLen;

                if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(pCalendar->Second), 10U);
                NumLen = AmbaUtility_StringLength(NumBuf);

                while (NumLen < 2UL) {
                    AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                    NumLen++;
                }

                AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, " GMT\r\n");

        }
    }

    return RetVal;
}

static CHAR const* DateHeader(void)
{
#define MAX_DATE_LENGTH 45U
    static CHAR DateBuf[MAX_DATE_LENGTH];
    AMBA_RTC_DATE_TIME_s TimeSpec;
    UINT32 RetVal, RetVal2;

    RetVal = AmbaRTC_GetSysTime(&TimeSpec);
    RetVal2 = TmAbsTime(&TimeSpec, DateBuf, MAX_DATE_LENGTH);
    if ((0U==RetVal) && (OK==RetVal2)) {
        // Do nothing
    } else {
        {
            if (AmbaWrap_memset(DateBuf, 0, MAX_DATE_LENGTH)!= 0U) { }

            // (void)AmbaAdvSnPrint(DateBuf, MAX_DATE_LENGTH, "Date: Wed, Jan 01 2014 00:00:00 GMT\r\n");
            AmbaUtility_StringAppend(DateBuf, MAX_DATE_LENGTH, "Date: Wed, Jan 01 2014 00:00:00 GMT\r\n");

            AmbaPrint_PrintStr5("%s, DateBuf %s", __func__, DateBuf, NULL, NULL, NULL);
        }
    }

    return DateBuf;
}

#if defined(CONFIG_LWIP_ENET)
static UINT32 LwIP_GetIP(UINT32 *Ip)
{
    UINT32 RetVal = 0U;

    if (NULL == Ip) {
        RetVal = 1U;
    } else {
        extern UINT32 sys_netif_get_by_index(UINT8 idx);
        *Ip = htonl(sys_netif_get_by_index(2));

        if (0U == *Ip) {
            RetVal = 1U;
        }
    }

    return RetVal;
}
#endif

/**
* Returns own IPv4 address.
* @param [in] socket descriptor to get the address for, ignored for NetX and LwIP.
* @return non zero IP address if ok / 0 in case of an error.
*/
static UINT32 GetOwnIpAddress(int Socket)
{
    UINT32 IpAddress;
    UINT32 RetVal = ERR_ARG;

#if defined(CONFIG_NETX_ENET)
    UINT32 Mask;
    NETX_STACK_CONFIG_s *pNetXConfig;
    AmbaMisra_TouchUnused(&Socket);

    if (NetXStack_GetConfig(0, &pNetXConfig) == OK) {
        if (nx_ip_address_get(pNetXConfig->pNxIp, &IpAddress, &Mask) == OK) {
            RetVal = OK;
        }
    }

#elif defined(CONFIG_LWIP_ENET)
    AmbaMisra_TouchUnused(&Socket);
    RetVal = LwIP_GetIP(&IpAddress);

#else // LINUX/POSIX
    struct sockaddr_in SockAddr = {};
    socklen_t AddrLen = sizeof(SockAddr);
    if (getsockname(Socket, (struct sockaddr*)&SockAddr, &AddrLen) == 0) {
        if (SockAddr.sin_family == AF_INET)
        {
            IpAddress = ntohl(SockAddr.sin_addr.s_addr);
            RetVal = OK;
        }
    }
#endif

    if (RetVal == OK)
    {
        return IpAddress;
    }
    else
    {
        AmbaPrint_PrintUInt5("Error: Failed to get own IP address\n",
            0U, 0U, 0U, 0U, 0U);
        return 0U;
    }
}

static void HandleCmdBad(const SVC_RTSP_CLNTSESSION_s* Clnt, CHAR const* Cseq)
{
    (VOID)Cseq;
    {
        if (AmbaWrap_memset((CHAR*)Clnt->ResponseBuf, 0, SvcRtspServer_GetBufferSize())!= 0U) { }

        // (void)AmbaAdvSnPrint((CHAR*)Clnt->ResponseBuf, SvcRtspServer_GetBufferSize(), "RTSP/1.0 400 Bad Request\r\n%sAllow: %s\r\n\r\n", DateHeader(), AllowedCommands);
        {
            char    *StrBuf = (CHAR*)Clnt->ResponseBuf;
            UINT32  BufSize = SvcRtspServer_GetBufferSize();

            AmbaUtility_StringAppend(StrBuf, BufSize, "RTSP/1.0 400 Bad Request\r\n");
            AmbaUtility_StringAppend(StrBuf, BufSize, (DateHeader()));
            AmbaUtility_StringAppend(StrBuf, BufSize, "Allow: ");
            AmbaUtility_StringAppend(StrBuf, BufSize, (AllowedCommands));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n\r\n");
        }

        AmbaPrint_PrintStr5("%s, ResponseBuf %s", __func__, (CHAR*)Clnt->ResponseBuf, NULL, NULL, NULL);
    }
}

static VOID HandleCmdNotFound(SVC_RTSP_CLNTSESSION_s* Clnt, CHAR const* Cseq)
{
    UINT32 SysTickCnt;
    {
        if (AmbaWrap_memset((CHAR*)Clnt->ResponseBuf, 0, SvcRtspServer_GetBufferSize())!= 0U) { }

        // (void)AmbaAdvSnPrint((CHAR*)Clnt->ResponseBuf, SvcRtspServer_GetBufferSize(), "RTSP/1.0 404 Stream Not Found\r\nCSeq: %s\r\n%s\r\n",
        //                      DateHeader(), Cseq);
        {
            char    *StrBuf = (CHAR*)Clnt->ResponseBuf;
            UINT32  BufSize = SvcRtspServer_GetBufferSize();

            AmbaUtility_StringAppend(StrBuf, BufSize, "RTSP/1.0 404 Stream Not Found\r\nCSeq: ");
            AmbaUtility_StringAppend(StrBuf, BufSize, (DateHeader()));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
            AmbaUtility_StringAppend(StrBuf, BufSize, (Cseq));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
        }

        AmbaPrint_PrintStr5("%s, ResponseBuf %s", __func__, (CHAR*)Clnt->ResponseBuf, NULL, NULL, NULL);
    }

    (VOID)AmbaKAL_GetSysTickCount(&SysTickCnt);

    Clnt->LastActivity = SysTickCnt;
}

static void HandleCmdNotEnoughBandwidth(SVC_RTSP_CLNTSESSION_s* Clnt, const CHAR* Cseq)
{
    UINT32 SysTickCnt;
    {
        if (AmbaWrap_memset((CHAR*)Clnt->ResponseBuf, 0, SvcRtspServer_GetBufferSize())!= 0U) { }

        // (void)AmbaAdvSnPrint((CHAR*)Clnt->ResponseBuf, SvcRtspServer_GetBufferSize(), "RTSP/1.0 404 Stream Not Found\r\nCSeq: %s\r\n%s\r\n",
        //                      DateHeader(), Cseq);
        {
            char    *StrBuf = (CHAR*)Clnt->ResponseBuf;
            UINT32  BufSize = SvcRtspServer_GetBufferSize();

            AmbaUtility_StringAppend(StrBuf, BufSize, "RTSP/1.0 404 Stream Not Found\r\nCSeq: ");
            AmbaUtility_StringAppend(StrBuf, BufSize, (DateHeader()));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
            AmbaUtility_StringAppend(StrBuf, BufSize, (Cseq));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
        }

        AmbaPrint_PrintStr5("%s, ResponseBuf %s", __func__, (CHAR*)Clnt->ResponseBuf, NULL, NULL, NULL);
    }

    (void)AmbaKAL_GetSysTickCount(&SysTickCnt);

    Clnt->LastActivity = SysTickCnt;
}

static void HandleCmdNotSupported(const SVC_RTSP_CLNTSESSION_s* Clnt,CHAR const* Cseq)
{
    {
        if (AmbaWrap_memset((CHAR*)Clnt->ResponseBuf, 0, SvcRtspServer_GetBufferSize())!= 0U) { }

        // (void)AmbaAdvSnPrint((CHAR*)Clnt->ResponseBuf, SvcRtspServer_GetBufferSize(), "RTSP/1.0 405 Method Not Allowed\r\nCSeq: %s\r\n%sAllow: %s\r\n\r\n",
        //                      Cseq, DateHeader(), AllowedCommands);
        {
            char    *StrBuf = (CHAR*)Clnt->ResponseBuf;
            UINT32  BufSize = SvcRtspServer_GetBufferSize();

            AmbaUtility_StringAppend(StrBuf, BufSize, "RTSP/1.0 405 Method Not Allowed\r\nCSeq: ");
            AmbaUtility_StringAppend(StrBuf, BufSize, (Cseq));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
            AmbaUtility_StringAppend(StrBuf, BufSize, (DateHeader()));
            AmbaUtility_StringAppend(StrBuf, BufSize, "Allow: ");
            AmbaUtility_StringAppend(StrBuf, BufSize, (AllowedCommands));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n\r\n");
        }

        AmbaPrint_PrintStr5("%s, ResponseBuf %s", __func__, (CHAR*)Clnt->ResponseBuf, NULL, NULL, NULL);
    }
}

static void HandleCmdUnsupportedTransport(SVC_RTSP_CLNTSESSION_s* Clnt,CHAR const* Cseq)
{
    UINT32 SysTickCnt;
    {
        if (AmbaWrap_memset((CHAR*)Clnt->ResponseBuf, 0, SvcRtspServer_GetBufferSize())!= 0U) { }

        // (void)AmbaAdvSnPrint((CHAR*)Clnt->ResponseBuf, SvcRtspServer_GetBufferSize(), "RTSP/1.0 461 Unsupported Transport\r\nCSeq: %s\r\n%s\r\n",
        //                      Cseq, DateHeader());
        {
            char    *StrBuf = (CHAR*)Clnt->ResponseBuf;
            UINT32  BufSize = SvcRtspServer_GetBufferSize();

            AmbaUtility_StringAppend(StrBuf, BufSize, "RTSP/1.0 461 Unsupported Transport\r\nCSeq: ");
            AmbaUtility_StringAppend(StrBuf, BufSize, (Cseq));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
            AmbaUtility_StringAppend(StrBuf, BufSize, (DateHeader()));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
        }

        AmbaPrint_PrintStr5("%s, ResponseBuf %s", __func__, (CHAR*)Clnt->ResponseBuf, NULL, NULL, NULL);
    }

    (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
    Clnt->LastActivity = SysTickCnt;
}

static void HandleCmdOptions(SVC_RTSP_CLNTSESSION_s* Clnt, const CHAR* Cseq)
{
    UINT32 SysTickCnt;

    {
        if (AmbaWrap_memset((CHAR*)Clnt->ResponseBuf, 0, SvcRtspServer_GetBufferSize())!= 0U) { }

        // (void)AmbaAdvSnPrint((CHAR*)Clnt->ResponseBuf, SvcRtspServer_GetBufferSize(), "RTSP/1.0 200 OK\r\nCseq: %s\r\nPublic: %s\r\n\r\n",
        //                      Cseq, AllowedCommands);
        {
            char    *StrBuf = (CHAR*)Clnt->ResponseBuf;
            UINT32  BufSize = SvcRtspServer_GetBufferSize();

            AmbaUtility_StringAppend(StrBuf, BufSize, "RTSP/1.0 200 OK\r\nCseq: ");
            AmbaUtility_StringAppend(StrBuf, BufSize, (Cseq));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\nPublic: ");
            AmbaUtility_StringAppend(StrBuf, BufSize, (AllowedCommands));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n\r\n");
        }

        AmbaPrint_PrintStr5("%s, res %s, Cseq %s", __func__, (CHAR*)Clnt->ResponseBuf, Cseq, NULL, NULL);
    }

    (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
    Clnt->LastActivity = SysTickCnt;
}

static void HandleCmdDescribe(SVC_RTSP_CLNTSESSION_s* Clnt, CHAR const* Cseq,
             CHAR const* UrlPreSuffix, CHAR const* UrlSuffix,
             CHAR const* FullRequestStr)
{
    UINT32 SdpDescriptionSize, SysTickCnt;
    CHAR RtspURL[254] = {'\0'};
    MEDIA_SESSION_s* MediaSession;
    CHAR* SdpDescription = NULL;
    CHAR UrlTotalSuffix[RTSP_PARAM_STRISVC_FRWK_NG_MAX];
    const SVC_RTSP_SERVER_s *RtspSrv;
    UINT32 Len;
    const VOID *pVoid;

    (VOID)FullRequestStr;

    Len = AmbaUtility_StringLength(UrlPreSuffix);
    Len += AmbaUtility_StringLength(UrlSuffix);
    Len += 2U;
    if (Len > sizeof(UrlTotalSuffix)) {
        HandleCmdBad(Clnt, Cseq);
    } else {
        UrlTotalSuffix[0] = '\0';
        if (UrlPreSuffix[0] != '\0') {
            AmbaUtility_StringAppend(UrlTotalSuffix, RTSP_PARAM_STRISVC_FRWK_NG_MAX, UrlPreSuffix);
            AmbaUtility_StringAppend(UrlTotalSuffix, RTSP_PARAM_STRISVC_FRWK_NG_MAX, "/");
        }

        AmbaUtility_StringAppend(UrlTotalSuffix, RTSP_PARAM_STRISVC_FRWK_NG_MAX, UrlSuffix);

        AmbaMisra_TypeCast(&RtspSrv, &Clnt->Srv);
        pVoid = SvcRtspServer_LookupMediaSession(RtspSrv, UrlTotalSuffix);
        AmbaMisra_TypeCast(&MediaSession, &pVoid);
        if (MediaSession == NULL) {
            HandleCmdNotFound(Clnt, Cseq);
        } else {
            Clnt->MediaSession = MediaSession;
            SdpDescription = SvcMediaSession_GenSDPDescription(Clnt->Srv, MediaSession, GetOwnIpAddress(Clnt->Sd));
            if (SdpDescription == NULL) {
                {
                    if (AmbaWrap_memset((CHAR*)Clnt->ResponseBuf, 0, SvcRtspServer_GetBufferSize())!= 0U) { }

                    // (void)AmbaAdvSnPrint((CHAR*)Clnt->ResponseBuf, SvcRtspServer_GetBufferSize(), "RTSP/1.0 404 File Not Found, Or In Incorrect Format\r\n"
                    //                      "CSeq: %s\r\n"
                    //                      "%s\r\n", Cseq, DateHeader());
                    {
                        char    *StrBuf = (CHAR*)Clnt->ResponseBuf;
                        UINT32  BufSize = SvcRtspServer_GetBufferSize();

                        AmbaUtility_StringAppend(StrBuf, BufSize, "RTSP/1.0 404 File Not Found, Or In Incorrect Format\r\nCSeq: ");
                        AmbaUtility_StringAppend(StrBuf, BufSize, (Cseq));
                        AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
                        AmbaUtility_StringAppend(StrBuf, BufSize, (DateHeader()));
                        AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
                    }

                    AmbaPrint_PrintStr5("%s, ResponseBuf %s", __func__, (CHAR*)Clnt->ResponseBuf, NULL, NULL, NULL);
                }
            }else {
                SdpDescriptionSize = AmbaUtility_StringLength(SdpDescription);

                AmbaMisra_TypeCast(&RtspSrv, &Clnt->Srv);
                (VOID)SvcRtspServer_GetRTSP_URL(RtspSrv, MediaSession, RtspURL, sizeof(RtspURL), GetOwnIpAddress(Clnt->Sd));

                {
                    if (AmbaWrap_memset((CHAR*)Clnt->ResponseBuf, 0, SvcRtspServer_GetBufferSize())!= 0U) { }

                    // (void)AmbaAdvSnPrint((CHAR*)Clnt->ResponseBuf, SvcRtspServer_GetBufferSize(), "RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
                    //                         "%s"
                    //                         "Content-Base: %s/\r\n"
                    //                         "Content-Type: application/sdp\r\n"
                    //                         "Content-Length: %d\r\n\r\n"
                    //                         "%s", Cseq, DateHeader(), RtspURL, SdpDescriptionSize, SdpDescription);
                    {
                        char    *StrBuf = (CHAR*)Clnt->ResponseBuf;
                        UINT32  BufSize = SvcRtspServer_GetBufferSize();
                        UINT32  CurStrLen;

                        AmbaUtility_StringAppend(StrBuf, BufSize, "RTSP/1.0 200 OK\r\nCSeq: ");
                        AmbaUtility_StringAppend(StrBuf, BufSize, (Cseq));
                        AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
                        AmbaUtility_StringAppend(StrBuf, BufSize, (DateHeader()));
                        AmbaUtility_StringAppend(StrBuf, BufSize, "Content-Base: ");
                        AmbaUtility_StringAppend(StrBuf, BufSize, (RtspURL));
                        AmbaUtility_StringAppend(StrBuf, BufSize, "/\r\nContent-Type: application/sdp\r\nContent-Length: ");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(SdpDescriptionSize), 10U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n\r\n");
                        AmbaUtility_StringAppend(StrBuf, BufSize, (SdpDescription));
                        AmbaUtility_StringAppend(StrBuf, BufSize, "");
                    }

                    AmbaPrint_PrintStr5("%s, ResponseBuf %s", __func__, (CHAR*)Clnt->ResponseBuf, NULL, NULL, NULL);
                }
                (void)SvcMediaSession_FreeSDPDescription(MediaSession);
                (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
                Clnt->LastActivity = SysTickCnt;
            }
        }
    }
    AmbaMisra_TouchUnused(SdpDescription);
}

static void HandleCmdSetup(SVC_RTSP_CLNTSESSION_s* Clnt, CHAR const* Cseq,
          CHAR const* UrlPreSuffix, CHAR const* UrlSuffix,
          CHAR const* FullRequestStr)
{
    CHAR const* StreamName = UrlPreSuffix;
    CHAR const* TrackId = UrlSuffix;
    const MEDIA_SESSION_s* MediaSession;
    UINT32 StreamingMode;
    CHAR* StreamingModeString = NULL;
    CHAR ClntDestAddrStr[32];
    UINT8 ClntDestTTL;
    UINT16 ClientRTPPortNum, ClientRTCPPortNum;
    UINT8 RtpChannelId, RtcpChannelId;
    UINT32 SysTickCnt, RetVal=OK;
    INT32 Rval;
    CHAR DstStr[16] = {'\0'};
    CHAR SrcStr[16] = {'\0'};
    UINT32 SrvRtpPort = 0;
    UINT32 SrvRtcpPort = 0;
    struct sockaddr_in SourceAddr;
    struct sockaddr_in const *pSockAddrIn = &SourceAddr;
    struct sockaddr *pSockAddr = NULL;
    struct in_addr SrcAddr, DstAddr;

#if defined(CONFIG_NETX_ENET)
    INT32 SockLen = (INT32)sizeof(SourceAddr);
#elif defined(CONFIG_LWIP_ENET)
    socklen_t SockLen = sizeof(SourceAddr);
    ip4_addr_t Ip4SrcAddr, Ip4DstAddr;
#else
    socklen_t SockLen = sizeof(SourceAddr);
#endif
    const VOID *pVoid;
    const SVC_RTSP_SERVER_s *RtspSrv;

    if (AmbaWrap_memset(&SourceAddr, 0, sizeof(SourceAddr))!= 0U) { }

    AmbaMisra_TypeCast(&RtspSrv, &Clnt->Srv);
    pVoid = SvcRtspServer_LookupMediaSession(RtspSrv, StreamName);

    AmbaMisra_TypeCast(&MediaSession, &pVoid);
    if (MediaSession == NULL) {
        HandleCmdNotFound(Clnt, Cseq);
        RetVal = ERR_NA;
    } else {
        ParseTransportHeader(FullRequestStr, &StreamingMode, &StreamingModeString,
                 ClntDestAddrStr, &ClntDestTTL,
                 &ClientRTPPortNum, &ClientRTCPPortNum,
                 &RtpChannelId, &RtcpChannelId);

        if(StreamingMode == RTP_UDP) {
            AmbaMisra_TypeCast(&pSockAddr, &pSockAddrIn);
            if (getsockname(Clnt->Sd, pSockAddr, &SockLen) < 0) {
                AmbaPrint_PrintStr5("getsockname() failed", NULL, NULL, NULL, NULL, NULL);
            }

            Rval = SvcMediaSession_SetupUDPTransport(MediaSession, TrackId, Clnt->SessionId, Clnt->ClientAddr.sin_addr,
                                        ClientRTPPortNum, ClientRTCPPortNum,
                                        &SrvRtpPort, &SrvRtcpPort, Clnt);
            if (Rval == -1) {
                HandleCmdNotEnoughBandwidth(Clnt, Cseq);
                RetVal = ERR_NA;
            } else if (Rval == -2) {
                HandleCmdNotFound(Clnt, Cseq);
                RetVal = ERR_NA;
            } else if (Rval == -3) {
                HandleCmdBad(Clnt, Cseq);
                RetVal = ERR_NA;
            } else {
                // Do nothing
            }

            if (ERR_NA != RetVal) {
#if defined(CONFIG_LWIP_ENET)
                SrcAddr.s_addr = htonl(SourceAddr.sin_addr.s_addr);
                DstAddr.s_addr = htonl(Clnt->ClientAddr.sin_addr.s_addr);

                AmbaSvcWrap_MisraMemcpy(&Ip4SrcAddr, &SrcAddr, sizeof(Ip4SrcAddr));
                AmbaSvcWrap_MisraMemcpy(&Ip4DstAddr, &DstAddr, sizeof(Ip4DstAddr));
#else
                SrcAddr.s_addr = htonl(SourceAddr.sin_addr.s_addr);
                DstAddr.s_addr = htonl(Clnt->ClientAddr.sin_addr.s_addr);
#endif
                {
                    if (AmbaWrap_memset(DstStr, 0, 16U)!= 0U) { }
#if defined(CONFIG_LWIP_ENET)
                    // (void)AmbaAdvSnPrint(DstStr, 16, "%s", inet_ntoa(Ip4DstAddr));
                    AmbaUtility_StringAppend(DstStr, 16U, inet_ntoa(Ip4DstAddr));
#else
                    // (void)AmbaAdvSnPrint(DstStr, 16, "%s", inet_ntoa(DstAddr));
                    AmbaUtility_StringAppend(DstStr, 16U, inet_ntoa(DstAddr));
#endif

                    AmbaPrint_PrintStr5("%s, DstStr %s", __func__, DstStr, NULL, NULL, NULL);
                }

                {
                    if (AmbaWrap_memset(SrcStr, 0, 16U)!= 0U) { }
#if defined(CONFIG_LWIP_ENET)
                    // (void)AmbaAdvSnPrint(SrcStr, 16, "%s", inet_ntoa(Ip4SrcAddr));
                    AmbaUtility_StringAppend(SrcStr, 16U, inet_ntoa(Ip4SrcAddr));
#else
                    // (void)AmbaAdvSnPrint(SrcStr, 16, "%s", inet_ntoa(SrcAddr));
                    AmbaUtility_StringAppend(SrcStr, 16U, inet_ntoa(SrcAddr));
#endif
                    AmbaPrint_PrintStr5("%s, SrcStr %s", __func__, SrcStr, NULL, NULL, NULL);
                }

                {
                    if (AmbaWrap_memset((CHAR*)Clnt->ResponseBuf, 0, SvcRtspServer_GetBufferSize())!= 0U) { }

                    // (void)AmbaAdvSnPrint((CHAR*)Clnt->ResponseBuf, SvcRtspServer_GetBufferSize(), "RTSP/1.0 200 OK\r\n"
                    //                         "CSeq: %s\r\n"
                    //                         "%s"
                    //                         "Transport: RTP/AVP;unicast;destination=%s;source=%s;client_port=%d-%d;server_port=%u-%u\r\n"
                    //                         "Session: %08X\r\n\r\n",
                    //                         Cseq, DateHeader(), DstStr, SrcStr, ClientRTPPortNum, ClientRTCPPortNum, SrvRtpPort, SrvRtcpPort, Clnt->SessionId);

                    {
                        char    *StrBuf = (CHAR*)Clnt->ResponseBuf;
                        UINT32  BufSize = SvcRtspServer_GetBufferSize();
                        UINT32  CurStrLen;

                        AmbaUtility_StringAppend(StrBuf, BufSize, "RTSP/1.0 200 OK\r\nCSeq: ");
                        AmbaUtility_StringAppend(StrBuf, BufSize, (Cseq));
                        AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
                        AmbaUtility_StringAppend(StrBuf, BufSize, ( DateHeader()));
                        AmbaUtility_StringAppend(StrBuf, BufSize, "Transport: RTP/AVP;unicast;destination=");
                        AmbaUtility_StringAppend(StrBuf, BufSize, (DstStr));
                        AmbaUtility_StringAppend(StrBuf, BufSize, ";source=");
                        AmbaUtility_StringAppend(StrBuf, BufSize, (SrcStr));
                        AmbaUtility_StringAppend(StrBuf, BufSize, ";client_port=");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(ClientRTPPortNum), 10U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, "-");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(ClientRTCPPortNum), 10U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, ";server_port=");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(SrvRtpPort), 10U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, "-");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(SrvRtcpPort), 10U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, "\r\nSession: ");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            char    NumBuf[16];
                            UINT32  NumLen;

                            if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                            (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(Clnt->SessionId), 16U);
                            NumLen = AmbaUtility_StringLength(NumBuf);

                            while (NumLen < 8UL) {
                                AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                                NumLen++;
                            }

                            AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n\r\n");
                    }
                    AmbaPrint_PrintStr5("%s, ResponseBuf %s", __func__, (CHAR*)Clnt->ResponseBuf, NULL, NULL, NULL);
                }
            }
        }else {
            HandleCmdUnsupportedTransport(Clnt, Cseq);
        }

        if (ERR_NA != RetVal) {
            if (Clnt->Status == SVC_RTSP_CLNT_INIT) {
                Clnt->Status = SVC_RTSP_CLNT_READY;
            }

            (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
            Clnt->LastActivity = SysTickCnt;
        }
    }
    (VOID)RetVal;
#if defined(CONFIG_LWIP_ENET)
    if (NULL != pSockAddr) {
        (VOID)pSockAddr->sa_len;
    }
#endif
}

static void HandleCmdGetParameter(SVC_RTSP_CLNTSESSION_s* Clnt, CHAR const* Cseq,
              CHAR const* FullRequestStr)
{
    UINT32 SysTickCnt;

    (VOID)FullRequestStr;
    {
        if (AmbaWrap_memset((CHAR*)Clnt->ResponseBuf, 0, SvcRtspServer_GetBufferSize())!= 0U) { }

        // (void)AmbaAdvSnPrint((CHAR*)Clnt->ResponseBuf, SvcRtspServer_GetBufferSize(), "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sSession: %08X\r\n\r\n",
        //                      Cseq, DateHeader(), Clnt->SessionId);
        {
            char    *StrBuf = (CHAR*)Clnt->ResponseBuf;
            UINT32  BufSize = SvcRtspServer_GetBufferSize();
            UINT32  CurStrLen;

            AmbaUtility_StringAppend(StrBuf, BufSize, "RTSP/1.0 200 OK\r\nCSeq: ");
            AmbaUtility_StringAppend(StrBuf, BufSize, (Cseq));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
            AmbaUtility_StringAppend(StrBuf, BufSize, (DateHeader()));
            AmbaUtility_StringAppend(StrBuf, BufSize, "Session: ");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                char    NumBuf[16];
                UINT32  NumLen;

                if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(Clnt->SessionId), 16U);
                NumLen = AmbaUtility_StringLength(NumBuf);

                while (NumLen < 8UL) {
                    AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                    NumLen++;
                }

                AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n\r\n");

        }
        AmbaPrint_PrintStr5("%s, ResponseBuf %s", __func__, (CHAR*)Clnt->ResponseBuf, NULL, NULL, NULL);
    }

    (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
    Clnt->LastActivity = SysTickCnt;
}

static void HandleCmdSetParameter(SVC_RTSP_CLNTSESSION_s* Clnt, CHAR const* Cseq,
              CHAR const* FullRequestStr)
{
    UINT32 SysTickCnt;
    (VOID)FullRequestStr;

    /* For Ambarella RTSP extension, we remove all RTSP-standard parameter and
        bypass the un-recognized parameter to upper lever */
    {
        if (AmbaWrap_memset((CHAR*)Clnt->ResponseBuf, 0, SvcRtspServer_GetBufferSize())!= 0U) { }
        // (void)AmbaAdvSnPrint((CHAR*)Clnt->ResponseBuf, SvcRtspServer_GetBufferSize(), "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sSession: %08X\r\n\r\n",
        //                      Cseq, DateHeader(), Clnt->SessionId);
        {
            char    *StrBuf = (CHAR*)Clnt->ResponseBuf;
            UINT32  BufSize = SvcRtspServer_GetBufferSize();
            UINT32  CurStrLen;

            AmbaUtility_StringAppend(StrBuf, BufSize, "RTSP/1.0 200 OK\r\nCSeq: ");
            AmbaUtility_StringAppend(StrBuf, BufSize, (Cseq));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
            AmbaUtility_StringAppend(StrBuf, BufSize, (DateHeader()));
            AmbaUtility_StringAppend(StrBuf, BufSize, "Session: ");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                char    NumBuf[16];
                UINT32  NumLen;

                if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(Clnt->SessionId), 16U);
                NumLen = AmbaUtility_StringLength(NumBuf);

                while (NumLen < 8UL) {
                    AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                    NumLen++;
                }

                AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n\r\n");

        }

        AmbaPrint_PrintStr5("%s, ResponseBuf %s", __func__, (CHAR*)Clnt->ResponseBuf, NULL, NULL, NULL);
    }

    (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
    Clnt->LastActivity = SysTickCnt;
}

static void GetSeekTime(const CHAR *requestString, UINT32 *SeekTime)
{
    const CHAR *RangeHead;
    DOUBLE TempDouble;
    CHAR SeekStr[32] = {'\0'};
    const CHAR *RangeTail;
    ULONG RangeSize, SrcUL, SrcULnd;
    DOUBLE TmpDouble;

    RangeHead = StrStr(requestString,"Range");
    if(RangeHead != NULL) {
        RangeHead = StrStr(RangeHead, "=");
        if(RangeHead != NULL) {
            /* omit '=' */
            {
                AmbaMisra_TypeCast(&SrcUL, &RangeHead);
                SrcUL += 1U;
                AmbaMisra_TypeCast(&RangeHead, &SrcUL);
            }

            while(*RangeHead == ' ') {
                /* omit spaces */
                {
                    AmbaMisra_TypeCast(&SrcUL, &RangeHead);
                    SrcUL += 1U;
                    AmbaMisra_TypeCast(&RangeHead, &SrcUL);
                }
            }

            RangeTail = StrStr(RangeHead, "-");
            if(RangeTail != NULL) {
                AmbaMisra_TypeCast(&SrcUL, &RangeTail);
                AmbaMisra_TypeCast(&SrcULnd, &RangeHead);
                RangeSize = (SrcUL-SrcULnd) / sizeof(CHAR);
                AmbaUtility_StringCopy(SeekStr, RangeSize, RangeHead);

                TempDouble = StrTod(SeekStr);
                TmpDouble = (DOUBLE)1000*TempDouble;
                *SeekTime = (UINT32)TmpDouble;
            }
        }
    }
}
static void HandleCmdPlay(SVC_RTSP_CLNTSESSION_s* Clnt, CHAR const* Cseq,
                                CHAR const* FullRequestStr)
{
    CHAR RtspUrl[64] = {'\0'};
    CHAR RtpInfo[512] = {'\0'};
    UINT32 SeekTime = 0U;
    UINT32 SysTickCnt, RetVal = OK;
    const SVC_RTSP_SERVER_s *pRtspSrv;

    if (Clnt->MediaSession == NULL) {
        HandleCmdBad(Clnt, Cseq);
    } else {
        GetSeekTime(FullRequestStr, &SeekTime);
        AmbaMisra_TypeCast(&pRtspSrv, &(Clnt->Srv));
        (VOID)SvcRtspServer_GetRTSP_URL(pRtspSrv, Clnt->MediaSession, RtspUrl, 64, GetOwnIpAddress(Clnt->Sd));

        {
            if (AmbaWrap_memset(RtpInfo, 0, 512U)!= 0U) { }
            // (void)AmbaAdvSnPrint(RtpInfo, 512, "RTP-Info: ");
            AmbaUtility_StringAppend(RtpInfo, 512U, "RTP-Info: ");

            AmbaPrint_PrintStr5("%s, RtpInfo %s", __func__, RtpInfo, NULL, NULL, NULL);
        }

        AmbaPrint_PrintStr5("%s, RtspUrl %s", __func__, RtspUrl, NULL, NULL, NULL);
        if (AmbaUtility_StringCompare(Clnt->MediaSession->StreamName, "live", AmbaUtility_StringLength("live")) == 0) {
            (VOID)SvcMediaSession_StartStream(Clnt->MediaSession, Clnt->SessionId, SVC_RTSP_SESSION_PLAY, RtspUrl, RtpInfo, 512U);
        } else {
            if (Clnt->Status == SVC_RTSP_CLNT_READY) {
                (VOID)SvcMediaSession_StartStream(Clnt->MediaSession, Clnt->SessionId, SVC_RTSP_SESSION_PLAY, RtspUrl, RtpInfo, 512U);

                AmbaMisra_TypeCast(&pRtspSrv, &(Clnt->Srv));
                (VOID)SvcRtspServer_PlaybackCmd(pRtspSrv, (UINT32)SVC_RTSP_PLAYBACK_PLAY, &SeekTime);
            } else if ((Clnt->Status == SVC_RTSP_CLNT_PAUSE) && (SeekTime != 0U)) {
                (VOID)SvcMediaSession_StartStream(Clnt->MediaSession, Clnt->SessionId, SVC_RTSP_SESSION_SEEK, RtspUrl, RtpInfo, 512U);

                AmbaMisra_TypeCast(&pRtspSrv, &(Clnt->Srv));
                (VOID)SvcRtspServer_PlaybackCmd(pRtspSrv, (UINT32)SVC_RTSP_PLAYBACK_PLAY, &SeekTime);
            } else if ((Clnt->Status == SVC_RTSP_CLNT_PAUSE) && (SeekTime == 0U)) {
                SeekTime = Clnt->PauseNpt;
                (VOID)SvcMediaSession_StartStream(Clnt->MediaSession, Clnt->SessionId, SVC_RTSP_SESSION_RESUME, RtspUrl, RtpInfo, 512U);

                AmbaMisra_TypeCast(&pRtspSrv, &(Clnt->Srv));
                (VOID)SvcRtspServer_PlaybackCmd(pRtspSrv, (UINT32)SVC_RTSP_PLAYBACK_RESUME, NULL);
            } else if (Clnt->Status == SVC_RTSP_CLNT_PLAYING) {
                AmbaPrint_PrintUInt5("HandleCmdPlay: outstanding PLAY is not supported", \
                                    0U, 0U, 0U, 0U, 0U);
                HandleCmdNotSupported(Clnt, Cseq);
                RetVal = ERR_NA;
            } else {
                AmbaPrint_PrintUInt5("HandleCmdPlay: should not be here !!!!", \
                                0U, 0U, 0U, 0U, 0U);
                HandleCmdNotSupported(Clnt, Cseq);
                RetVal = ERR_NA;
            }

            if (ERR_NA != RetVal) {
                Clnt->StartNpt = SeekTime;
                (void)AmbaKAL_GetSysTickCount(&Clnt->StartSysTick);
            }
        }

        if (ERR_NA != RetVal) {
            Clnt->Status = SVC_RTSP_CLNT_PLAYING;
            {
                if (AmbaWrap_memset((CHAR*)Clnt->ResponseBuf, 0, SvcRtspServer_GetBufferSize())!= 0U) { }

                // (void)AmbaAdvSnPrint((CHAR*)Clnt->ResponseBuf, SvcRtspServer_GetBufferSize(), "RTSP/1.0 200 OK\r\n"
                //                         "CSeq: %s\r\n"
                //                         "%s"
                //                         "Range: npt=%.3f-\r\n"
                //                         "Session: %08X\r\n"
                //                         "%s\r\n\r\n",
                //                         Cseq, DateHeader(), (FLOAT)SeekTime/(FLOAT)1000, Clnt->SessionId, RtpInfo);
                {
                    char    *StrBuf = (CHAR*)Clnt->ResponseBuf;
                    UINT32  BufSize = SvcRtspServer_GetBufferSize();
                    UINT32  CurStrLen;

                    AmbaUtility_StringAppend(StrBuf, BufSize, "RTSP/1.0 200 OK\r\nCSeq: ");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (Cseq));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (DateHeader()));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "Range: npt=");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        (void)AmbaUtility_FloatToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (FLOAT)((FLOAT)SeekTime/(FLOAT)1000), 3U);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, "-\r\nSession: ");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        char    NumBuf[16];
                        UINT32  NumLen;

                        if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                        (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(Clnt->SessionId), 16U);
                        NumLen = AmbaUtility_StringLength(NumBuf);

                        while (NumLen < 8UL) {
                            AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                            NumLen++;
                        }

                        AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (RtpInfo));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n\r\n");
                }

                AmbaPrint_PrintStr5("%s, ResponseBuf %s", __func__, (CHAR*)Clnt->ResponseBuf, NULL, NULL, NULL);
            }

            (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
            (void)AmbaKAL_GetSysTickCount(&Clnt->LastActivity);
        }
    }
}

static void HandleCmdPause(SVC_RTSP_CLNTSESSION_s* Clnt, CHAR const* Cseq)
{
    UINT32 SysTickCnt;
    const SVC_RTSP_SERVER_s *pRtspSrv;

    if (Clnt->MediaSession == NULL) {
        HandleCmdBad(Clnt, Cseq);
    } else {
        /* if mediaSession belongs to live-group */
        if ((AmbaUtility_StringCompare(Clnt->MediaSession->StreamName, "live", 4U)) == 0) {
            /* need to trigger EOS */
            (void)SvcMediaSession_PauseStream(Clnt->MediaSession, Clnt->SessionId);
        } else {
            /* if mediaSession belongs to playback-group */
            (void)SvcMediaSession_PauseStream(Clnt->MediaSession, Clnt->SessionId);

            if (Clnt->Status == SVC_RTSP_CLNT_PLAYING) {
                (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
                Clnt->PauseNpt = ((Clnt->StartNpt + SysTickCnt) - Clnt->StartSysTick);

                AmbaMisra_TypeCast(&pRtspSrv, &Clnt->Srv);
                (VOID)SvcRtspServer_PlaybackCmd(pRtspSrv, (UINT32)SVC_RTSP_PLAYBACK_PAUSE, NULL);
                Clnt->Status = SVC_RTSP_CLNT_PAUSE;
            }
        }

        (VOID)AmbaKAL_TaskSleep(60U);  /* delayed the PAUSE response so we won't receive next PLAY too early */

        {
            if (AmbaWrap_memset((CHAR*)Clnt->ResponseBuf, 0, SvcRtspServer_GetBufferSize())!= 0U) { }

            // (void)AmbaAdvSnPrint((CHAR*)Clnt->ResponseBuf, SvcRtspServer_GetBufferSize(), "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%s\r\n",
            //                             Cseq, DateHeader());
            {
                char    *StrBuf = (CHAR*)Clnt->ResponseBuf;
                UINT32  BufSize = SvcRtspServer_GetBufferSize();
                // UINT32  CurStrLen;

                AmbaUtility_StringAppend(StrBuf, BufSize, "RTSP/1.0 200 OK\r\nCSeq: ");
                AmbaUtility_StringAppend(StrBuf, BufSize, (Cseq));
                AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
                AmbaUtility_StringAppend(StrBuf, BufSize, (DateHeader()));
                AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
            }

            AmbaPrint_PrintStr5("%s, ResponseBuf %s", __func__, (CHAR*)Clnt->ResponseBuf, NULL, NULL, NULL);
        }

        (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
        Clnt->LastActivity = SysTickCnt;
    }
}

static void HandleCmdTearDown(SVC_RTSP_CLNTSESSION_s* Clnt, CHAR const* Cseq)
{
    UINT32 SysTickCnt;
    const SVC_RTSP_SERVER_s *pRtspSrv;

    if (Clnt->MediaSession != NULL) {
        (VOID)SvcMediaSession_TearDown(Clnt->MediaSession, Clnt->SessionId);
        if ((AmbaUtility_StringCompare(Clnt->MediaSession->StreamName, "live", 4U) != 0) && (Clnt->Status != SVC_RTSP_CLNT_INIT)) {
            AmbaMisra_TypeCast(&pRtspSrv, &Clnt->Srv);
            (VOID)SvcRtspServer_PlaybackCmd(pRtspSrv, (UINT32)SVC_RTSP_PLAYBACK_STOP, NULL);
            Clnt->Status = SVC_RTSP_CLNT_INIT;
        }
    }

    {
        if (AmbaWrap_memset((CHAR*)Clnt->ResponseBuf, 0, SvcRtspServer_GetBufferSize())!= 0U) { }
        // (void)AmbaAdvSnPrint((CHAR*)Clnt->ResponseBuf, SvcRtspServer_GetBufferSize(), "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%s\r\n",
        //                                 Cseq, DateHeader());
        {
            char    *StrBuf = (CHAR*)Clnt->ResponseBuf;
            UINT32  BufSize = SvcRtspServer_GetBufferSize();

            AmbaUtility_StringAppend(StrBuf, BufSize, "RTSP/1.0 200 OK\r\nCSeq: ");
            AmbaUtility_StringAppend(StrBuf, BufSize, (Cseq));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
            AmbaUtility_StringAppend(StrBuf, BufSize, (DateHeader()));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
        }
        AmbaPrint_PrintStr5("%s, ResponseBuf %s", __func__, (CHAR*)Clnt->ResponseBuf, NULL, NULL, NULL);
    }

    Clnt->Active = 0;

    (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
    Clnt->LastActivity = SysTickCnt;
}

static void HandleCmdWithinSession(SVC_RTSP_CLNTSESSION_s* Clnt, CHAR const* CmdName,
              CHAR const* UrlPreSuffix, CHAR const* UrlSuffix,
              CHAR const* Cseq, CHAR const* FullRequestStr)
{
    UINT32 RetVal = OK;
    INT32 UrlSuffixCmp, UrlSuffixCmp2, UrlPreSuffixCmp, UrlPreSuffixCmp2;
    UINT32 UrlPreSuffixLen, UrlSuffixLen;

    {
        UrlSuffixCmp = AmbaUtility_StringCompare(Clnt->MediaSession->StreamName, UrlSuffix, AmbaUtility_StringLength(UrlSuffix));

        UrlPreSuffixLen = AmbaUtility_StringLength(UrlPreSuffix);
        UrlPreSuffixCmp = AmbaUtility_StringCompare(Clnt->MediaSession->StreamName, UrlPreSuffix, UrlPreSuffixLen);

        UrlSuffixLen = AmbaUtility_StringLength(UrlSuffix);
        UrlSuffixCmp2 = AmbaUtility_StringCompare(&(Clnt->MediaSession->StreamName[UrlPreSuffixLen+1U]), UrlSuffix, UrlSuffixLen);

        UrlPreSuffixCmp2 = AmbaUtility_StringCompare(Clnt->MediaSession->StreamName, UrlPreSuffix, UrlPreSuffixLen);


    }
    if ((UrlPreSuffix[0]=='\0') && (UrlSuffix[0]=='*') && (UrlSuffix[1]=='\0')) {
        if (AmbaUtility_StringCompare(CmdName, "GET_PARAMETER", AmbaUtility_StringLength(CmdName)) == 0) {
            HandleCmdGetParameter(Clnt, Cseq, FullRequestStr);
        } else if (AmbaUtility_StringCompare(CmdName, "SET_PARAMETER", AmbaUtility_StringLength(CmdName)) == 0) {
            HandleCmdSetParameter(Clnt, Cseq, FullRequestStr);
        } else {
            HandleCmdNotSupported(Clnt, Cseq);
        }
        RetVal = ERR_NA;
    } else if (Clnt->Status== SVC_RTSP_CLNT_INIT) {
        HandleCmdNotSupported(Clnt, Cseq);
        RetVal = ERR_NA;
    } else if ((UrlSuffixCmp==0) || \
                ((UrlPreSuffixCmp==0) && \
                (Clnt->MediaSession->StreamName[UrlPreSuffixLen]=='/') && \
                (UrlSuffixCmp2==0)) || \
                ((UrlSuffix[0]=='\0') && (UrlPreSuffixCmp2==0))) {
        // Aggregated operation
    } else if ((UrlPreSuffix[0]!='\0') && (UrlSuffix[0]!='\0')) {
        HandleCmdNotFound(Clnt, Cseq);
        RetVal = ERR_NA;
    } else {
        HandleCmdNotFound(Clnt, Cseq);
        RetVal = ERR_NA;
    }

    if (ERR_NA != RetVal) {
        if (AmbaUtility_StringCompare(CmdName, "TEARDOWN", AmbaUtility_StringLength(CmdName)) == 0) {
            HandleCmdTearDown(Clnt, Cseq);
        } else if (AmbaUtility_StringCompare(CmdName, "PLAY", AmbaUtility_StringLength(CmdName)) == 0) {
            HandleCmdPlay(Clnt, Cseq, FullRequestStr);
        } else if (AmbaUtility_StringCompare(CmdName, "PAUSE", AmbaUtility_StringLength(CmdName)) == 0) {
            HandleCmdPause(Clnt, Cseq);
        } else if (AmbaUtility_StringCompare(CmdName, "GET_PARAMETER", AmbaUtility_StringLength(CmdName)) == 0) {
            HandleCmdGetParameter(Clnt, Cseq, FullRequestStr);
        } else if (AmbaUtility_StringCompare(CmdName, "SET_PARAMETER", AmbaUtility_StringLength(CmdName)) == 0) {
            HandleCmdSetParameter(Clnt, Cseq, FullRequestStr);
        } else {
            // Do nothing
        }
    }
}

static inline void resetRequestBuf(SVC_RTSP_CLNTSESSION_s* Clnt)
{
    UINT8  *pBuf = &(Clnt->RequestBuf[0]);
    ULONG  Addr;

    Clnt->RequestBytes = 0U;
    Clnt->RequestBufLeft = SVC_RTSP_REQ_BUFFER_SIZE;

    AmbaMisra_TypeCast(&Addr, &pBuf);
    Addr -= 2U;
    AmbaMisra_TypeCast(&pBuf, &Addr);
    Clnt->LastCRLF = pBuf;
}


/**
* reset the last active time
* @param [in]  client session
* @return OK/ERR_NA.
*/
UINT32 RtspClnt_ResetLstActTime(SVC_RTSP_CLNTSESSION_s* Clnt)
{
    UINT32 SysTickCnt;
    UINT32 RetVal = OK;

    if (Clnt == NULL) {
        AmbaPrint_PrintStr5("Invalid input", NULL, NULL, NULL, NULL, NULL);
        RetVal = ERR_NA;
    } else {
        (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
        Clnt->LastActivity = SysTickCnt;
    }

    return RetVal;
}


/**
* Get the last active time
* @param [in]  client session
* @return OK/ERR_NA.
*/
UINT32 RtspClnt_GetLstActTime(const SVC_RTSP_CLNTSESSION_s* Clnt)
{
    UINT32 RetVal = OK;

    if (Clnt == NULL) {
        AmbaPrint_PrintStr5("Invalid input", NULL, NULL, NULL, NULL, NULL);
        RetVal = ERR_NA;
    } else {
        RetVal = Clnt->LastActivity;
    }

    return RetVal;
}

/**
* Release the client session
* @param [in]  client session
* @return OK/ERR_NA.
*/
UINT32 RtspClnt_Release(SVC_RTSP_CLNTSESSION_s* Clnt)
{
    UINT32 RemainedSubsession = 0;
    UINT32 RetVal = OK;
    SVC_RTSP_SERVER_s *pRtspSrv;

    if (Clnt == NULL) {
        AmbaPrint_PrintStr5("Invalid input", NULL, NULL, NULL, NULL, NULL);
        RetVal = ERR_NA;
    } else {
        if (Clnt->MediaSession != NULL) {
            RemainedSubsession = SvcMediaSession_TearDown(Clnt->MediaSession, Clnt->SessionId);

            if (AmbaUtility_StringCompare(Clnt->MediaSession->StreamName, "live", 4U) != 0) {
                if (Clnt->Status != SVC_RTSP_CLNT_INIT) {
                    AmbaMisra_TypeCast(&pRtspSrv, &Clnt->Srv);
                    (VOID)SvcRtspServer_PlaybackCmd(pRtspSrv, (UINT32)SVC_RTSP_PLAYBACK_STOP, NULL);
                }
            }

            if (RemainedSubsession > 0U) {
                AmbaPrint_PrintStr5("there is still client connect to media Session(%s)", \
                                    Clnt->MediaSession->StreamName, NULL, NULL, NULL, NULL);
            } else {
                SvcMediaSession_Release(Clnt->MediaSession);
            }
        }

        AmbaMisra_TypeCast(&pRtspSrv, &Clnt->Srv);
        (VOID)SvcRtspServer_ClearSd(pRtspSrv, Clnt->Sd);
        Clnt->Sd = -1;
        Clnt->Status = SVC_RTSP_CLNT_INIT;
        Clnt->MediaSession = NULL;
    }

    return RetVal;
}

/**
* Create client session
* @param [in]  server handler
* @param [in]  socket id
* @param [in]  client address
* @return OK/ERR_NA.
*/
UINT32 RtspClnt_Create(const void *pSrv, INT32 Sd, struct sockaddr_in ClientAddr)
{
    SVC_RTSP_CLNTSESSION_s* Clnt = NULL;
    UINT32 SysTickCnt;
    SVC_RTSP_SERVER_s* Srv;
    UINT32 RetVal = OK;

    AmbaMisra_TypeCast(&Srv, &pSrv);
    if (Srv == NULL) {
        AmbaPrint_PrintStr5("Invalid input", NULL, NULL, NULL, NULL, NULL);
        RetVal = ERR_NA;
    }

    if (ERR_NA != RetVal) {
        Clnt = SvcRtspServer_GetClientSlot(Srv);
        if (Clnt != NULL) {
            resetRequestBuf(Clnt);
            Clnt->Srv = Srv;
            Clnt->Active = 1;
            Clnt->Status = SVC_RTSP_CLNT_INIT;
            Clnt->ClientAddr = ClientAddr;
            Clnt->SessionId = our_random32();
            Clnt->Sd = Sd;

            (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
            Clnt->LastActivity = SysTickCnt;
            RetVal = OK;
        } else {
#if defined(CONFIG_NETX_ENET)
            INT Status = soc_close(Sd);
#else
            INT Status = close(Sd);
#endif
            AmbaPrint_PrintStr5("Too many client Session!! close connection", NULL, NULL, NULL, NULL, NULL);
            if (Status < 0){
                AmbaPrint_PrintStr5("soc_close() fail", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }

    return RetVal;
}

/**
* Handle the requests from client
* @param [in]  client handler
*/
void RtspClnt_IncomingHandler(SVC_RTSP_CLNTSESSION_s* Clnt)
{
    INT32 BytesRead;
    UINT32 Rval = OK;
    UINT8 *Crusor;
    const UINT8 *ptrTmp;
    UINT32 EndOfMsg;
    const UINT8 *CurPtr;
    CHAR CmdName[RTSP_PARAM_STRISVC_FRWK_NG_MAX];
    CHAR UrlPreSuffix[RTSP_PARAM_STRISVC_FRWK_NG_MAX];
    CHAR UrlSuffix[RTSP_PARAM_STRISVC_FRWK_NG_MAX];
    CHAR Cseq[RTSP_PARAM_STRISVC_FRWK_NG_MAX] = {0};
    UINT32 ContentLength = 0;
    ULONG SrcUL, SrcULnd;
    ULONG SrcUL0, SrcUL0nd;
    INT32 TearDownCmp, PlayCmp, PauseCmp, GetParamCmp, SetParamCmp;

    if (Clnt == NULL) {
        AmbaPrint_PrintStr5("Invalid input", NULL, NULL, NULL, NULL, NULL);
    } else {
#if defined(CONFIG_NETX_ENET)
        BytesRead = recv(Clnt->Sd, Clnt->RequestBuf, (INT32)Clnt->RequestBufLeft, 0);
#else
        BytesRead = recv(Clnt->Sd, Clnt->RequestBuf, Clnt->RequestBufLeft, 0);
#endif
        EndOfMsg = 0U;
        CurPtr = &Clnt->RequestBuf[Clnt->RequestBytes];
        if (BytesRead < 0) {
#if defined(CONFIG_NETX_ENET)
            /* NetX could return this upon disconnection, don't report ERR if it is disconnection */
            if (_nx_get_errno() != ENOTCONN) {
                AmbaPrint_PrintUInt5("recv error !, %d", (UINT32)_nx_get_errno(), 0U, 0U, 0U, 0U);
            }
#else
            if (errno != ENOTCONN) {
                AmbaPrint_PrintUInt5("recv error !, %d", (UINT32)errno, 0U, 0U, 0U, 0U);
            }
#endif
            Clnt->Active = (UINT8)0;
            Rval = ERR_NA;
        } else if (BytesRead == 0) {
            AmbaPrint_PrintStr5("rtspClientSession client close connection", NULL, NULL, NULL, NULL, NULL);
            Clnt->Active = (UINT8)0;
            Rval = ERR_NA;
        } else if ((UINT32)BytesRead >= Clnt->RequestBufLeft) {
            AmbaPrint_PrintUInt5("request too large. read %d bytes, left %u bytes", (UINT32)BytesRead, Clnt->RequestBufLeft, 0U, 0U, 0U);
            Clnt->Active = (UINT8)0;
            Rval = ERR_NA;
        } else {
            // Do nothing
        }

        if (ERR_NA != Rval) {
            Clnt->RequestBufLeft -= (UINT32)BytesRead;
            Clnt->RequestBytes += (UINT32)BytesRead;

            ptrTmp = Clnt->LastCRLF;
            AmbaMisra_TypeCast(&SrcUL, &ptrTmp);
            SrcUL = + 2;
            AmbaMisra_TypeCast(&Crusor, &SrcUL);

            ptrTmp = &(Clnt->RequestBuf[0]);
            AmbaMisra_TypeCast(&SrcULnd, &ptrTmp);
            if (SrcUL < SrcULnd) {
                Crusor = &(Clnt->RequestBuf[0]);
            }

            AmbaMisra_TypeCast(&SrcUL, &Crusor);
            ptrTmp = &(CurPtr[BytesRead-1]);
            AmbaMisra_TypeCast(&SrcULnd, &ptrTmp);
            while (SrcUL < SrcULnd) {
                if ((*Crusor == (UINT8)'\r') && ((Crusor[1]) == (UINT8)'\n')){
                    AmbaMisra_TypeCast(&SrcUL0, &Crusor);
                    AmbaMisra_TypeCast(&SrcUL0nd, &Clnt->LastCRLF);

                    if ((SrcUL0-SrcUL0nd) == 2U){
                        EndOfMsg = 1U;
                        break;
                    }
                    Clnt->LastCRLF = Crusor;
                }
                ++Crusor;
            }

            if (EndOfMsg == 0U){
                Rval = ERR_NA;
            } else {
                Clnt->RequestBuf[Clnt->RequestBytes] = (UINT8)'\0';
                Rval = ParseRtspRequestString((const CHAR*) Clnt->RequestBuf, Clnt->RequestBytes,
                                CmdName, sizeof(CmdName), UrlPreSuffix, sizeof(UrlPreSuffix), UrlSuffix,
                                sizeof(UrlSuffix), Cseq, sizeof(Cseq), &ContentLength);

                if (Rval == OK) {
                    AmbaMisra_TypeCast(&SrcUL, &CurPtr);
                    SrcUL += (UINT32)BytesRead;

                    AmbaMisra_TypeCast(&SrcULnd, &Crusor);
                    SrcULnd += 2U;
                    SrcULnd += ContentLength;
                    if (SrcUL < SrcULnd) {
                        Rval = ERR_NA;
                    } else {
                        TearDownCmp = AmbaUtility_StringCompare(CmdName, "TEARDOWN", AmbaUtility_StringLength(CmdName));
                        PlayCmp = AmbaUtility_StringCompare(CmdName, "PLAY", AmbaUtility_StringLength(CmdName));
                        PauseCmp = AmbaUtility_StringCompare(CmdName, "PAUSE", AmbaUtility_StringLength(CmdName));
                        GetParamCmp = AmbaUtility_StringCompare(CmdName, "GET_PARAMETER", AmbaUtility_StringLength(CmdName));
                        SetParamCmp = AmbaUtility_StringCompare(CmdName, "SET_PARAMETER", AmbaUtility_StringLength(CmdName));
                        if (AmbaUtility_StringCompare(CmdName, "OPTIONS", AmbaUtility_StringLength(CmdName)) == 0) {
                            HandleCmdOptions(Clnt, Cseq);
                        } else if (AmbaUtility_StringCompare(CmdName, "DESCRIBE", AmbaUtility_StringLength(CmdName)) == 0) {
                            HandleCmdDescribe(Clnt, Cseq, UrlPreSuffix, \
                                        UrlSuffix, (CHAR const*)Clnt->RequestBuf);
                        } else if (AmbaUtility_StringCompare(CmdName, "SETUP", AmbaUtility_StringLength(CmdName)) == 0) {
                            HandleCmdSetup(Clnt, Cseq, UrlPreSuffix, UrlSuffix, \
                                        (CHAR const*)Clnt->RequestBuf);
                        } else if ((TearDownCmp==0)     ||  \
                                    (PlayCmp==0)        ||  \
                                    (PauseCmp==0)       ||  \
                                    (GetParamCmp==0)    ||  \
                                    (SetParamCmp==0)) {
                            HandleCmdWithinSession(Clnt, CmdName, UrlPreSuffix, UrlSuffix, Cseq, \
                                        (CHAR const*)Clnt->RequestBuf);
                        } else {
                            HandleCmdNotSupported(Clnt, Cseq);
                        }
                    }
                } else {
                    AmbaPrint_PrintStr5("bad request", NULL, NULL, NULL, NULL, NULL);
                    HandleCmdBad(Clnt, Cseq);
                }
#if defined(CONFIG_NETX_ENET)
                (VOID)send(Clnt->Sd, (CHAR*)Clnt->ResponseBuf, (INT32)AmbaUtility_StringLength((CHAR*)Clnt->ResponseBuf), 0);
#else
                (VOID)send(Clnt->Sd, (CHAR*)Clnt->ResponseBuf, AmbaUtility_StringLength((CHAR*)Clnt->ResponseBuf), 0);
#endif
                resetRequestBuf(Clnt);
            }
        }

        if(Clnt->Active == (UINT8)0) {
            (VOID)RtspClnt_Release(Clnt);
        }
    }
    (VOID)Rval;
}
