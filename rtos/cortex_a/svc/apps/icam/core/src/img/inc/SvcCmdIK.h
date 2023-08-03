/**
*  @file SvcCmdIK.h
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
*  @details svc boot device
*
*/

#ifndef SVC_SHELL_CMD_H
#define SVC_SHELL_CMD_H

void SvcCmdIK_Install(void);
void SvcCmdIK_ItunerOn(ULONG BufAddr, UINT32 BufSize);
void SvcCmdIK_DumpMemQry(UINT32 *pBufSize);
void SvcCmdIK_DumpMemCfg(ULONG BufAddr, UINT32 BufSize);
void SvcCmdIK_SaveIsoCfg(UINT32 IkCtxID, UINT32 PreCnt, const char *pFileName);
void SvcCmdIK_SaveItuner(UINT32 IkCtxID, char *pFileName);
void SvcCmdIK_LoadItuner(UINT32 IkCtxID, char *pFileName);

#define SVC_CMD_IK_SAVE_IDSP_CFG_NORMAL      (0x0U)
#define SVC_CMD_IK_SAVE_IDSP_CFG_STILL_CAP   (0x1U)
void SvcCmdIK_SaveIdspCfg(UINT32 ViewZoneID, UINT32 DumpLevel, UINT32 CtrlFlg, char *pDumpPath);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcCmdIK_Idsp.c
\*-----------------------------------------------------------------------------------------------*/
#define SVC_CMD_IK_IDSP_CFG_SIZE    (0x80000U)

typedef UINT32 (*SVC_CMD_IK_IDSP_CFG_UPD_FUNC_f)(void *pCfgBuf, void *pBuf);

typedef struct {
    UINT32                         Ofst_Addr;
    UINT32                         Ofst_Pitch;
    UINT32                         Ofst_Width;
    UINT32                         Ofst_Height;
    SVC_CMD_IK_IDSP_CFG_UPD_FUNC_f pFunc;
} SVC_CMD_IK_IDSP_DUMP_INFO_s;

typedef void (*SVC_CMD_IK_IDSP_DUMP_FUNC_f)(UINT8 *pCfgBuf, UINT8 *pDumpBuf, UINT32 DumpBufSize, const void *pDumpFuncInfo, char *pPrefixStr);

typedef struct {
    UINT32                       Level;
    SVC_CMD_IK_IDSP_DUMP_FUNC_f  pFunc;
    UINT32                       NumOfInfo;
    SVC_CMD_IK_IDSP_DUMP_INFO_s *pInfo;
} SVC_CMD_IK_IDSP_DUMP_FUNC_s;

typedef struct {
    void   *pMode;
    UINT8  *pBuf;
    UINT32  BufSize;
    UINT8 **pCfgAddr;
} SVC_CMD_IK_IDSP_CFG;

UINT32 SvcCmdIK_DumpIdspInit(UINT32 *pDumpFuncNum, SVC_CMD_IK_IDSP_DUMP_FUNC_s **pDumFunc);
void   SvcCmdIK_DumpIdspUsage(void);
UINT32 SvcCmdIK_GetIdspCfg(UINT32 DumpLevel, UINT32 CtrlFlg, SVC_CMD_IK_IDSP_CFG *pCfg);

#endif  /* SVC_SHELL_CMD_H */
