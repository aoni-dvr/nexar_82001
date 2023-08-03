/**
*  @file AmbaDmux.h
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
*  @details amba demuxer functions
*
*/

#ifndef AMBA_DMUX_H
#define AMBA_DMUX_H

#include "SvcFormatDef.h"
#include "SvcIso.h"
#include "SvcFormat.h"

typedef struct {
    ULONG               BufAddr;
    UINT32              BufSize;
} AMBA_DMUX_BUF_s;

typedef struct {
    UINT32              DemuxNum;   /* the number of demuxer */
    AMBA_DMUX_BUF_s     DmxBuf;
} AMBA_DMUX_INIT_s;

typedef struct {
    SVC_MOVIE_INFO_s    *pMovInfo;
    UINT8               ContainerType;
    UINT32              StartTime;
    UINT32              Direction;
    UINT32              Speed;
    SVC_STREAM_HDLR_s   *pStreamHdlr;
} AMBA_DMUX_CREATE_s;

typedef struct {
#define AMBA_DMUX_PRIV_SIZE  (0x50U)
    UINT8  PrivData[AMBA_DMUX_PRIV_SIZE];
} AMBA_DMUX_CTRL;

UINT32 AmbaDmux_EvalMemSize(UINT32 DemuxNum, UINT32 *pSize);
UINT32 AmbaDmux_Init(const AMBA_DMUX_INIT_s *pInitInfo);

UINT32 AmbaDmux_Parse(UINT32 MediaType, const char *FileName, SVC_STREAM_HDLR_s *pStreamHdlr, SVC_MOVIE_INFO_s *pMovInfo, UINT8 *pContainerType);

#define AMBA_DMUX_MTYPE_IMAGE  (1U)
#define AMBA_DMUX_MTYPE_MOVIE  (2U)

#define AMBA_DMUX_CTYPE_MP4    (1U)
#define AMBA_DMUX_CTYPE_FMP4   (2U)

UINT32 AmbaDmux_Create(AMBA_DMUX_CTRL *pDmxCtrl, const AMBA_DMUX_CREATE_s *pCreate);
UINT32 AmbaDmux_Start(AMBA_DMUX_CTRL *pDmxCtrl);
UINT32 AmbaDmux_Stop(AMBA_DMUX_CTRL *pDmxCtrl);
UINT32 AmbaDmux_Process(AMBA_DMUX_CTRL *pDmxCtrl, UINT8 FrameCount, UINT8 *pEvent);
UINT32 AmbaDmux_Delete(AMBA_DMUX_CTRL *pDmxCtrl);
UINT32 AmbaDmux_FeedFrame(AMBA_DMUX_CTRL *pDmxCtrl, UINT8 TrackId, UINT32 TargetTime, UINT8 FrameType);

#endif  /* AMBA_DMUX_H */