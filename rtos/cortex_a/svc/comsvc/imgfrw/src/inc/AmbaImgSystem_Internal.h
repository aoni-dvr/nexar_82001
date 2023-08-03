/**
 *  @file AmbaImgSystem_Internal.h
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
 *  @details Constants and Definitions for Amba Image System Internal
 *
 */

#ifndef AMBA_IMG_SYSTEM_INTERNAL_H
#define AMBA_IMG_SYSTEM_INTERNAL_H

typedef enum /*_AMBA_IMG_SYSTEM_WB_GAIN_OP_e_*/ {
    WB_GAIN_OP_MIX = 0,
    WB_GAIN_OP_FINE
} AMBA_IMG_SYSTEM_WB_GAIN_OP_e;

typedef enum /*_AMBA_IMG_SYSTEM_GAIN_TYPE_e_*/ {
    GAIN_TYPE_NORMAL = 1,
    GAIN_TYPE_FINE
} AMBA_IMG_SYSTEM_GAIN_TYPE_e;

typedef enum /*_AMBA_IMG_SYSTEM_GAIN_CMD_e_*/ {
    GAIN_CMD_WGC = 1,
    GAIN_CMD_IGC,
    GAIN_CMD_DGC,
    GAIN_CMD_GGC
} AMBA_IMG_SYSTEM_GAIN_CMD_e;

typedef enum /*_AMBA_IMG_SYSTEM_GAIN_e_*/ {
    GAIN_WGC = ((UINT32) GAIN_CMD_WGC) << 28U,
    GAIN_IGC = ((UINT32) GAIN_CMD_IGC) << 28U,
    GAIN_DGC = ((UINT32) GAIN_CMD_DGC) << 28U,
    GAIN_GGC = ((UINT32) GAIN_CMD_GGC) << 28U
} AMBA_IMG_SYSTEM_GAIN_e;

typedef union /*_AMBA_IMG_SYSTEM_GAIN_CMD_MSG_u_*/ {
    UINT32 Data;
    struct {
        UINT32 Type:           2;
        UINT32 Reserved:      26;
        UINT32 Cmd:            4;
    } Bits;
} AMBA_IMG_SYSTEM_GAIN_CMD_MSG_u;

typedef struct /*_AMBA_IMG_SYSTEM_GAIN_CMD_MSG_s_*/ {
    AMBA_IMG_SYSTEM_GAIN_CMD_MSG_u    Ctx;
} AMBA_IMG_SYSTEM_GAIN_CMD_MSG_s;

typedef enum /*_AMBA_IMG_SYSTEM_SLOW_SHUTTER_e_*/ {
    SS_SYNC_BOTH = 0,
    SS_SYNC_DEC_ONLY
} AMBA_IMG_SYSTEM_SLOW_SHUTTER_e;

typedef enum /*_AMBA_IMG_SYSTEM_AIK_CMD_e_*/ {
    AIK_CMD_EXECUTE_ATTACH = 0,
    AIK_CMD_EXECUTE_NO_WARP_ATTACH,
    AIK_CMD_EXECUTE_WARP_ONLY
} AMBA_IMG_SYSTEM_AIK_CMD_e;

#define AIK_EXECUTE_ATTACH            (((UINT32) AIK_CMD_EXECUTE_ATTACH)         << 28U)
#define AIK_EXECUTE_NO_WARP_ATTACH    (((UINT32) AIK_CMD_EXECUTE_NO_WARP_ATTACH) << 28U)
#define AIK_EXECUTE_WARP_ONLY         (((UINT32) AIK_CMD_EXECUTE_WARP_ONLY)      << 28U)

typedef union /*_AMBA_IMG_SYSTEM_AIK_CMD_MSG_u_*/ {
    UINT32 Data;
    struct {
        UINT32  Mode:         1;
        UINT32  Reserved:    27;
        UINT32  Cmd:          4;
    } Bits;
} AMBA_IMG_SYSTEM_AIK_CMD_MSG_u;

typedef struct /*_AMBA_IMG_SYSTEM_AIK_CMD_MSG_s_*/ {
    AMBA_IMG_SYSTEM_AIK_CMD_MSG_u    Ctx;
} AMBA_IMG_SYSTEM_AIK_CMD_MSG_s;

UINT32 AmbaImgSystem_RawCapSeqPut(UINT32 VinId, UINT32 RawCapSeq);

UINT32 AmbaImgSystem_RingPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SYSTEM_RING_PIPE_s *pPipe, UINT32 RawCapSeq);

UINT32 AmbaImgSystem_ActAikCtxGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SYSTEM_AIK_CTX_s *pActAikCtx);

UINT32 AmbaImgSystem_AikStatusPut(const AMBA_IMG_SYSTEM_ZONE_s *pZone, const AMBA_IMG_SYSTEM_AIK_CTX_s *pCtx);
UINT32 AmbaImgSystem_AikStatusGet(const AMBA_IMG_SYSTEM_ZONE_s *pZone, AMBA_IMG_SYSTEM_AIK_CTX_s *pCtx);

//UINT32 AmbaImgSystem_R2yUserBufferRst(AMBA_IMG_CHANNEL_ID_s ImageChanId);

UINT32 AmbaImgSystem_PostAikPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 RawCapSeq, UINT32 PostAikCnt);
UINT32 AmbaImgSystem_PostAikGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SYSTEM_PAIK_INFO_s **pPostAikInfo);
UINT32 AmbaImgSystem_PostAikGo(AMBA_IMG_CHANNEL_ID_s ImageChanId);

UINT32 AmbaImgSystem_VinCaptureLineGet(UINT32 VinId, UINT32 ChainId);
UINT32 AmbaImgSystem_HdrDgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pDgc);
UINT32 AmbaImgSystem_AikProcess(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 User);
UINT32 AmbaImgSystem_EFovProcess(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 User);

#endif  /* AMBA_IMG_SYSTEM_INTERNAL_H */
