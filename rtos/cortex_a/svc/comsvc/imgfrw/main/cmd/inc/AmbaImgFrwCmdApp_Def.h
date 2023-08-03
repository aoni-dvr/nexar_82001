/**
 *  @file AmbaImgFrwCmdApp_Def.h
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
 *  @details Constants and Definitions for AmbaShell Image Framework Command App
 *
 */

#ifndef AMBA_IMG_FRW_CMD_APP_DEF_H
#define AMBA_IMG_FRW_CMD_APP_DEF_H

#ifndef AMBA_IMG_FRW_LIB_COMSVC
#if 0
#define AMBSH_MAGIC    0xCafeU

typedef struct AMBA_IMG_FRW_CMD_s_ {
    UINT32                         Magic;
    const char                     *pName;
    AMBA_SHELL_PROC_f              Proc;
    struct AMBA_IMG_FRW_CMD_s_    *pNext;
} AMBA_IMG_FRW_CMD_s;

UINT32 fvar_strcmp(const char *pStr1, const char *pStr2);

 INT32 fvar_atoi(const char *pStr);
UINT32 fvar_atou(const char *pStr);

typedef enum /*_AMBA_IMG_FRW_CMD_PRN_ID_e_*/ {
    F_PRINT_ID_DBG = 0,
    F_PRINT_ID_MSG,
    F_PRINT_ID_ERR
} AMBA_IMG_FRW_CMD_PRN_ID_e;

#define F_PRINT_FLAG_DBG    (((UINT32) 1U) << (UINT32) F_PRINT_ID_DBG)
#define F_PRINT_FLAG_MSG    (((UINT32) 1U) << (UINT32) F_PRINT_ID_MSG)
#define F_PRINT_FLAG_ERR    (((UINT32) 1U) << (UINT32) F_PRINT_ID_ERR)

  void AmbaImgFrwCmd_PrintEnable(UINT32 Flag);

  void AmbaImgFrwCmd_PrintEx(UINT32 Id, const char *pStr, UINT32 var_ul, UINT32 var_base);
  void AmbaImgFrwCmd_Print(UINT32 Id, const char *pStr, UINT32 var_ul);
  void AmbaImgFrwCmd_PrintStr(UINT32 Id, const char *pStr);
  void AmbaImgFrwCmd_PrintEx2(UINT32 Id, const char *pStr, UINT32 var0_ul, UINT32 var0_base, UINT32 var1_ul, UINT32 var1_base);
  void AmbaImgFrwCmd_Print2(UINT32 Id, const char *pStr, UINT32 var0_ul, UINT32 var1_ul);

UINT32 AmbaImgFrwCmd_Attach(void);
UINT32 AmbaImgFrwCmd_Add(AMBA_IMG_FRW_CMD_s *pCmd);

  void AmbaShell_CommandAmbaImgFrw(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
#else
#include "AmbaImgFrwCmd_Def.h"
#include "AmbaImgFrwCmdVar.h"
#include "AmbaImgFrwCmdMain.h"
#endif

#else

#include "AmbaImgFrwCmd_Def.h"
#include "AmbaImgFrwCmdVar.h"
#include "AmbaImgFrwCmdMain.h"

#endif

#endif /* AMBA_IMG_FRW_CMD_APP_DEF_H */
