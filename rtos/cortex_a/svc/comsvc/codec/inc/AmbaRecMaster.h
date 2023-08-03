/**
*  @file AmbaRecMaster.h
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
*  @details Amba record master
*
*/

#ifndef AMBA_REC_MASTER_H
#define AMBA_REC_MASTER_H

/* record source ID */
#define AMBA_RMA_SRC_NUM             (8U)

/* state of record master */
#define AMBA_RMA_STATE_INVALID       (0U)
#define AMBA_RMA_STATE_IDLE          (1U)
#define AMBA_RMA_STATE_AVSYNC        (2U)
#define AMBA_RMA_STATE_RUN           (3U)

/* control type of record master */
#define AMBA_RMAC_STATE_GET          (0U)
#define AMBA_RMAC_GO                 (1U)
#define AMBA_RMAC_HALT               (2U)
#define AMBA_RMAC_SPLIT_TIME         (3U)
#define AMBA_RMAC_SCH_OFFSET         (4U)
#define AMBA_RMAC_WAIT_VEOS          (5U)

typedef struct {
    UINT32  SrcBits;
    UINT32  SchFrameOffset;
} AMBA_RMAC_SCHOFF_PARAM;

typedef struct {
    /* info from recorder to user */
    UINT32  StreamId;
    UINT32  RscType;
    UINT32  FrameType;
    UINT32  TileIdx;
    UINT32  NumTile;
    UINT32  SliceIdx;
    UINT32  NumSlice;
    ULONG   SrcBufBase;
    UINT32  SrcBufSize;
    ULONG   SrcDataAddr;
    UINT32  SrcDataSize;

    /* info back from user to recorder */
    ULONG   DstBufBase;
    UINT32  DstBufSize;
    ULONG   DstDataAddr;
    UINT32  DstDataSize;
} AMBA_RMAC_ENCRYPT_s;

typedef struct {
    UINT32  StreamId;
    /* update size of the bitsdata that already been copied from bitsbuf to RecBox/RecDst */
    UINT32 (*pfnProcBitsSizeUpdate)(UINT32 StreamId, UINT32 RscType, UINT32 Size);

    /* encrypt the bitstream data */
    void   (*pfnEncrypt)(AMBA_RMAC_ENCRYPT_s *pInfo);
} AMBA_RMA_USR_CFG_s;

typedef struct AMBA_REC_MASTER {
    /* fill by user */
    AMBA_RMA_USR_CFG_s  UsrCfg;

    /* fill internally */
    void   (*pfnNotify)(const struct AMBA_REC_MASTER *pRmaCtrl, UINT32 SrcBit);
    UINT8  PrivData[CONFIG_AMBA_REC_RMA_PRIV_SIZE];
} AMBA_REC_MASTER_s;

UINT32 AmbaRecMaster_Create(AMBA_REC_MASTER_s *pRmaCtrl, UINT32 Priority, UINT32 CpuBits);
UINT32 AmbaRecMaster_Destroy(const AMBA_REC_MASTER_s *pRmaCtrl);
void   AmbaRecMaster_Control(const AMBA_REC_MASTER_s *pRmaCtrl, UINT32 CtrlType, void *pParam);

void   AmbaRecMaster_SrcHook(const AMBA_REC_MASTER_s *pRmaCtrl, AMBA_REC_SRC_s *pRecSrc);
void   AmbaRecMaster_BoxHook(const AMBA_REC_MASTER_s *pRmaCtrl, AMBA_REC_BOX_s *pRecBox);
void   AmbaRecMaster_DstHook(const AMBA_REC_MASTER_s *pRmaCtrl, AMBA_REC_DST_s *pRecDst);

#endif  /* AMBA_REC_MASTER_H */
