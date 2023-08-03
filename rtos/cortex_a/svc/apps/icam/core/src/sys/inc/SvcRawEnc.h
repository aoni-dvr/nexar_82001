/*
*  @file SvcRawEnc.h
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
*  @details  svc raw encode
*
*/

#ifndef SVC_RAW_ENC_H
#define SVC_RAW_ENC_H

#define SVC_RAW_ENC_MAX_BUF_NUM (0x2U)

#define SVC_RAW_ENC_CODE_PRE_RAW    (1U)
#define SVC_RAW_ENC_CODE_POST_RAW   (2U)
#define SVC_RAW_ENC_CODE_PRE_HDS    (3U)
#define SVC_RAW_ENC_CODE_POST_HDS   (4U)
#define SVC_RAW_ENC_CODE_PRE_ITN    (5U)
#define SVC_RAW_ENC_CODE_POST_ITN   (6U)
#define SVC_RAW_ENC_CODE_PRE_AAA    (7U)
#define SVC_RAW_ENC_CODE_UPD_AAA    (8U)
#define SVC_RAW_ENC_CODE_POST_AAA   (9U)
#define SVC_RAW_ENC_CODE_PRE_FEED   (10U)
#define SVC_RAW_ENC_CODE_POST_FEED  (11U)
#define SVC_RAW_ENC_CODE_UPD_IQPATH (12U)
#define SVC_RAW_ENC_CODE_NUM        (13U)
typedef void (*SVC_RAW_ENC_CALLBACK_f)(UINT32 ProcCode, void *pData);

typedef struct {
    UINT32 ImgChanId;
    void  *pAaaData;
} SVC_RAW_ENC_UPD_AAA_INFO;

typedef struct {
#define SVC_RAW_ENC_UPD_ADJ_PATH         (0)
#define SVC_RAW_ENC_UPD_IMG_PATH         (1)
#define SVC_RAW_ENC_UPD_AAA_PATH         (2)
#define SVC_RAW_ENC_UPD_VIDEO_PATH       (3)
#define SVC_RAW_ENC_UPD_VIDEO_MSM_PATH   (4)
#define SVC_RAW_ENC_UPD_VIDEO_MSH_PATH   (5)
#define SVC_RAW_ENC_UPD_PHOTO_PATH       (6)
#define SVC_RAW_ENC_UPD_PHOTO_MSM_PATH   (7)
#define SVC_RAW_ENC_UPD_PHOTO_MSH_PATH   (8)
#define SVC_RAW_ENC_UPD_SLISO_PATH       (9)
#define SVC_RAW_ENC_UPD_SHISO_PATH      (10)
#define SVC_RAW_ENC_UPD_VIDCC_PATH      (11)
#define SVC_RAW_ENC_UPD_STLCC_PATH      (12)
#define SVC_RAW_ENC_UPD_SCEEN_PATH      (13)
#define SVC_RAW_ENC_UPD_DVID_PATH       (14)
#define SVC_RAW_ENC_UPD_DSTL_PATH       (15)
#define SVC_RAW_ENC_UPD_VID_PARAM_PATH  (16)
#define SVC_RAW_ENC_UPD_STL_PARAM_PATH  (17)
    UINT32 ID;
    UINT32 TableNo;
    UINT32 CCSetPathNo;
    char  *pPath;
} SVC_RAW_ENC_UPD_IQPATH;

UINT32         SvcRawEnc_MemQry(const SVC_RES_CFG_s *pCfg, UINT32 *pMemSize);
UINT32         SvcRawEnc_Create(const SVC_RES_CFG_s *pCfg);
UINT32         SvcRawEnc_MemCfg(UINT8 *pMemBuf, UINT32 MemSize);
UINT32         SvcRawEnc_ItnMemCfg(UINT8 *pMemBuf, UINT32 MemSize);
UINT32         SvcRawEnc_Delete(void);
UINT32         SvcRawEnc_DefIdspCfg(UINT32 NumIK, UINT32 *pViewZoneIDs, AMBA_IK_MODE_CFG_s *pIKModeArr);
UINT32         SvcRawEnc_Execute(void);
SVC_RES_CFG_s *SvcRawEnc_ResCfgGet(void);
UINT32         SvcRawEnc_VoutDevGet(const void *pDrv, UINT32 *pID);
void           SvcRawEnc_CallBackRegister(SVC_RAW_ENC_CALLBACK_f pCallBack);
void           SvcRawEnc_CmdInstall(void);
void           SvcRawEnc_DumpCmdUsage(AMBA_FS_FILE *pFile, UINT32 ArgCount, char * const *pArgVector);
void           SvcRawEnc_SetStrmCapFileName(UINT32 StreamID, const char *pFileName, UINT32 Length);
UINT32         SvcRawEnc_GetStrmCapFileName(UINT32 StreamID, char *pFileName, UINT32 Length);

static inline UINT32 SvcRawEnc_BitGet(UINT32 Val) { return ((Val < 32U) ? (UINT32)(0x1UL << Val) : 0U); }

#endif /* SVC_RAW_ENC_H */

