/**
*  @file IdspFeeder.h
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
*   @details The FileFeeder example (The input data comes from idsp)
*
*/

#ifndef DF_IDSP_FEEDER_H
#define DF_IDSP_FEEDER_H

#include "SvcCvAlgo.h"
#include "CtCvAlgoWrapper.h"

#include "AmbaDSP_EventInfo.h"

typedef struct {
  UINT64                  CapPts;
  UINT64                  CapSequence;
  AMBA_DSP_YUV_IMG_BUF_s  *YuvImgBuf;
  AMBA_DSP_WINDOW_s       *Window;
} FEED_MQ_ITEM_s;

typedef struct {
  UINT32    Slot;
  UINT32    FrameRateDivisor;
} IDSP_FEEDER_CFG_s;

/******************************************************************************
 *  Defined in IdspFeeder.c
 ******************************************************************************/

UINT32 IdspFeeder_Start(const IDSP_FEEDER_CFG_s *pFeederCfg);
UINT32 IdspFeeder_Stop(UINT32 Slot);
UINT32 IdspFeeder_Feed(FEED_MQ_ITEM_s *pMqItem);

#endif /* DF_IDSP_FEEDER_H */

