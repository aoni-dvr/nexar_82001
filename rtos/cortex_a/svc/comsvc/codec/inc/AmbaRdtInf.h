/**
*  @file AmbaRdtInf.h
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
*  @details Amba record destination interface
*
*/

#ifndef AMBA_RDT_INF_H
#define AMBA_RDT_INF_H

#define AMBA_RDT_TYPE_FILE           (0U)
#define AMBA_RDT_TYPE_EVT            (1U)
#define AMBA_RDT_TYPE_NET            (2U)

#define AMBA_RDT_STYPE_RTSP          (0U)
#define AMBA_RDT_STYPE_NML           (1U)
#define AMBA_RDT_STYPE_EMG           (2U)

#define AMBA_REC_MAX_FILE_NAME_LEN   (128U)

typedef struct {
    UINT64  StoreCount;
    UINT64  StoreSize;
} AMBA_RDT_STATIS_s;

typedef struct {
#define AMBA_RDT_FSTATUS_FOPEN   (0U)
#define AMBA_RDT_FSTATUS_FCLOSE  (1U)
    UINT32  FileStatus;
    char    *FileName;
    UINT32  RecStrmId;
    UINT32  DstType;
    UINT32  SubType;
    UINT32  IsLastFile;
    UINT64  VidCapPts;
} AMBA_RDT_FSTATUS_INFO_s;

typedef struct {
    ULONG              MemBase;
    UINT32             MemSize;
    UINT32             SlotSize;
    UINT32             DstType;
    UINT32             SubType;
    UINT32             InSrcBits;
    UINT32             InBoxBits;
    UINT32             StreamID;
    UINT32             TaskPriority;
    UINT32             TaskCpuBits;
    UINT32             MaxRecPeriod;
    AMBA_REC_EVAL_s    RecEval;
    UINT32             NumRsc;
    UINT32             BootToRec;
    AMBA_RBX_USR_CFG_s  *pRbxUsrCfg;

    UINT32          (*pfnFetchFName)(char *pNameBuf, UINT32 MaxNameLen, UINT32 StreamID);
    UINT32          (*pfnUpdateFName)(const char *pNameBuf, UINT32 StreamID);
    UINT32          (*pfnCheckStorage)(UINT32 StreamID);
    UINT32          (*pfnFStatusNotify)(const AMBA_RDT_FSTATUS_INFO_s *pInfo);
} AMBA_RDT_USR_CFG_s;

typedef struct AMBA_REC_DST {
    /* fill by user */
    AMBA_RDT_USR_CFG_s  UsrCfg;

    /* fill internally */
    UINT32              (*pfnStore)(const struct AMBA_REC_DST *pRdtCtrl, const AMBA_REC_FRWK_DESC_s *pDesc);
    struct AMBA_REC_DST  *pNext;
    UINT8               PrivData[CONFIG_AMBA_REC_RDT_PRIV_SIZE];
} AMBA_REC_DST_s;

#endif  /* AMBA_RDT_INF_H */
