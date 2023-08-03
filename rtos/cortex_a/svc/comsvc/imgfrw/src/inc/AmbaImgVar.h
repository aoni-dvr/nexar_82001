/**
 *  @file AmbaImgVar.h
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
 *  @details Constants and Definitions for Amba Image Variable
 *
 */

#ifndef AMBA_IMAGE_VAR_H
#define AMBA_IMAGE_VAR_H

typedef enum /*_AMBA_IMG_VAR_LEADING_e_*/ {
    VAR_LEADING_NONE = 0,
    VAR_LEADING_ZERO,
    VAR_LEADING_SPACE
} AMBA_IMG_VAR_LEADING_e;

UINT32 var_strlen(const char *pString);
UINT32 var_strcmp(const char *pString1, const char *pString2);
UINT32 var_strcat(char *pString1, const char *pString2);

  void var_utoa(UINT32 Value, char *pString, UINT32 Base, UINT32 Count, UINT32 Flag);
  void var_itoa( INT32 Value, char *pString, UINT32 Base, UINT32 Count, UINT32 Flag);

  void var_ultoa(UINT64 Value, char *pString, UINT32 Base, UINT32 Count, UINT32 Flag);
  void var_lltoa( INT64 Value, char *pString, UINT32 Base, UINT32 Count, UINT32 Flag);

  void var_print(const char *pFmt, const UINT32 *Argc, const char * const *Argv);
  void var_print_ex(const char *pStr, UINT32 var_ul, UINT32 var_base);
  void var_print64_ex(const char *pStr, UINT64 var_ull, UINT32 var_base);
  void var_print_ex2(const char *pStr, UINT32 var0_ul, UINT32 var0_base, UINT32 var1_ul, UINT32 var1_base);
  void var_print64_ex2(const char *pStr, UINT64 var0_ull, UINT32 var0_base, UINT64 var1_ull, UINT32 var1_base);

  void AmbaImgPrint_Enable(UINT32 Flag);
  void AmbaImgPrint_EnableGet(UINT32 *pFlag);

  void AmbaImgPrintEx(UINT32 Id, const char *pStr, UINT32 var_ul, UINT32 var_base);
  void AmbaImgPrint64Ex(UINT32 Id, const char *pStr, UINT64 var_ull, UINT32 var_base);
  void AmbaImgPrint(UINT32 Id, const char *pStr, UINT32 var_ul);
  void AmbaImgPrintStr(UINT32 Id, const char *pStr);
  void AmbaImgPrintEx2(UINT32 Id, const char *pStr, UINT32 var0_ul, UINT32 var0_base, UINT32 var1_ul, UINT32 var1_base);
  void AmbaImgPrint64Ex2(UINT32 Id, const char *pStr, UINT64 var0_ull, UINT32 var0_base, UINT64 var1_ull, UINT32 var1_base);
  void AmbaImgPrint2(UINT32 Id, const char *pStr, UINT32 var0_ul, UINT32 var1_ul);
  void AmbaImgPrint_Flush(void);

#endif  /* AMBA_IMAGE_VAR_H */
