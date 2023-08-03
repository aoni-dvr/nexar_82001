/**
 *  @file AmbaMonFrwCmdVar.h
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Constants and Definitions for AmbaShell Monitor Framework Command Variable
 *
 */

#ifndef AMBA_MONITOR_FRW_CMD_VAR_H
#define AMBA_MONITOR_FRW_CMD_VAR_H

typedef enum /*_AMBA_MON_FRW_CMD_VAR_LEADING_e_*/ {
    SFVAR_LEADING_NONE = 0,
    SFVAR_LEADING_ZERO,
    SFVAR_LEADING_SPACE
} AMBA_MON_FRW_CMD_VAR_LEADING_e;

typedef enum /*_AMBA_MON_FRW_CMD_PRN_ID_e_*/ {
    SF_PRINT_ID_DBG = 0,
    SF_PRINT_ID_MSG,
    SF_PRINT_ID_ERR
} AMBA_MON_FRW_CMD_PRN_ID_e;

#define SF_PRINT_FLAG_DBG    (((UINT32) 1U) << (UINT32) SF_PRINT_ID_DBG)
#define SF_PRINT_FLAG_MSG    (((UINT32) 1U) << (UINT32) SF_PRINT_ID_MSG)
#define SF_PRINT_FLAG_ERR    (((UINT32) 1U) << (UINT32) SF_PRINT_ID_ERR)

 INT32 sfvar_atoi(const char *pStr);
UINT32 sfvar_atou(const char *pStr);

 INT64 sfvar_atoll(const char *pStr);
UINT64 sfvar_atoul(const char *pStr);

UINT32 sfvar_strlen(const char *pStr);
UINT32 sfvar_strcmp(const char *pStr1, const char *pStr2);

  void sfvar_utoa(UINT32 Value, char *pStr, UINT32 Base, UINT32 Count, UINT32 Flag);
  void sfvar_itoa( INT32 Value, char *pStr, UINT32 Base, UINT32 Count, UINT32 Flag);

  void sfvar_ultoa(UINT64 Value, char *pStr, UINT32 Base, UINT32 Count, UINT32 Flag);
  void sfvar_lltoa( INT64 Value, char *pStr, UINT32 Base, UINT32 Count, UINT32 Flag);

  void sfvar_print(const char *pFmt, const UINT32 *Argc, const char * const *Argv);
  void sfvar_print_ex(const char *pStr, UINT32 var_ul, UINT32 var_base);
  void sfvar_print64_ex(const char *pStr, UINT64 var_ull, UINT32 var_base);
  void sfvar_print_ex2(const char *pStr, UINT32 var0_ul, UINT32 var0_base, UINT32 var1_ul, UINT32 var1_base);
  void sfvar_print64_ex2(const char *pStr, UINT64 var0_ull, UINT32 var0_base, UINT64 var1_ull, UINT32 var1_base);

  void AmbaMonFrwCmd_PrintEnable(UINT32 Flag);

  void AmbaMonFrwCmd_PrintEx(UINT32 Id, const char *pStr, UINT32 var_ul, UINT32 var_base);
  void AmbaMonFrwCmd_Print64Ex(UINT32 Id, const char *pStr, UINT64 var_ull, UINT32 var_base);
  void AmbaMonFrwCmd_Print(UINT32 Id, const char *pStr, UINT32 var_ul);
  void AmbaMonFrwCmd_PrintStr(UINT32 Id, const char *pStr);
  void AmbaMonFrwCmd_PrintEx2(UINT32 Id, const char *pStr, UINT32 var0_ul, UINT32 var0_base, UINT32 var1_ul, UINT32 var1_base);
  void AmbaMonFrwCmd_Print64Ex2(UINT32 Id, const char *pStr, UINT64 var0_ull, UINT32 var0_base, UINT64 var1_ull, UINT32 var1_base);
  void AmbaMonFrwCmd_Print2(UINT32 Id, const char *pStr, UINT32 var0_ul, UINT32 var1_ul);

#endif /* AMBA_MONITOR_FRW_CMD_VAR_H */
