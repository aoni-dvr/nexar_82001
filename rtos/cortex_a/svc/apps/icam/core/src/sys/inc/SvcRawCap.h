/*
*  @file SvcRawCap.h
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
*  @details svc raw capture functions
*
*/

#ifndef SVC_RAW_CAP_H
#define SVC_RAW_CAP_H

#define SVC_RAW_CAP_DBG_MSG_ON (1U)

#define SVC_RAW_CAP_CMPR_TYPE_NONE  (0U)
#define SVC_RAW_CAP_CMPR_TYPE_6P75  (1U)
#define SVC_RAW_CAP_CMPR_TYPE_NUM   (2U)

#define SVC_RAW_CAP_CFG_RAW        (0x1U)
#define SVC_RAW_CAP_CFG_HDS        (0x2U)
#define SVC_RAW_CAP_CFG_TUNER      (0x4U)
#define SVC_RAW_CAP_CFG_3A         (0x8U)
#define SVC_RAW_CAP_CFG_NON_BMP    (0x10U)
#define SVC_RAW_CAP_CFG_NON_UNPACK (0x20U)

#define SVC_RAW_CAP_BUF_SIZE    (0x1C9C380U)

#define SVC_RAW_CAP_VIN_TYPE_CFA    (0U)
#define SVC_RAW_CAP_VIN_TYPE_YUV    (1U)
#define SVC_RAW_CAP_VIN_TYPE_NUM    (2U)

typedef struct {
    UINT32 Pitch;
    UINT32 Width;
    UINT32 Height;
    UINT8  *pBuf;
} SVC_RAW_CAP_INFO_s;

typedef struct {
    UINT32 VinID;
    UINT32 ChanID;
    UINT32 CapSeq;
    UINT8 *pDataBuf;
    UINT32 DataBufSize;
} SVC_RAW_CAP_AAA_DATA_INFO_s;

#define SVC_RAW_CAP_CMD_AAA_INFO_GET    (0U)
#define SVC_RAW_CAP_CMD_AAA_DATA_CAP    (1U)
#define SVC_RAW_CAP_CMD_NUM             (2U)
typedef UINT32 (*SVC_RAW_CAP_EXTRA_CAP_FUNC_f)(UINT32 CmdID, void *pData);

UINT32 SvcRawCap_Create(UINT32 TaskPriority, UINT32 TaskCpuBits);
UINT32 SvcRawCap_Delete(void);
UINT32 SvcRawCap_CfgCap(UINT32 VinSelectBits, UINT32 CfgSelectBits);
UINT32 SvcRawCap_CfgMem(UINT8 *pBuf, UINT32 BufSize);
UINT32 SvcRawCap_CfgItnMem(UINT8 *pBuf, UINT32 BufSize);
UINT32 SvcRawCap_CfgItn(UINT32 IkSelectBits, const char *pFileName);
UINT32 SvcRawCap_CfgVinType(UINT32 VinID, UINT32 Type);
UINT32 SvcRawCap_CfgRaw(UINT32 VinID, const char *pFileName);
UINT32 SvcRawCap_CfgHds(UINT32 VinID, const char *pFileName);
UINT32 SvcRawCap_Cfg3A(UINT32 VinID, UINT32 ImgChanSelectBits, const char *pFileName);
UINT32 SvcRawCap_CfgExtCapCb(SVC_RAW_CAP_EXTRA_CAP_FUNC_f pFunc);
UINT32 SvcRawCap_Info(UINT32 VinID, SVC_RAW_CAP_INFO_s *pRawInfo, SVC_RAW_CAP_INFO_s *pHdsInfo);
void   SvcRawCap_MemQry(UINT32 CapNum, UINT32 *pBufSize);
UINT32 SvcRawCap_CapSeq(UINT32 CapNum, UINT32 IsBlocked);
UINT32 SvcRawCap_Dump(void);
UINT32 SvcRawCap_GetRaw(UINT32 VinID, SVC_RAW_CAP_INFO_s *pRawInfo);
void   SvcRawCap_CmdAppEntry(UINT32 ArgCount, char * const *pArgVector);

#endif
