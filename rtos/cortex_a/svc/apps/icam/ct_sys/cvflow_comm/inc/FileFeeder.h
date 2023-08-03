/**
*  @file FileFeeder.h
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
*   @details The FileFeeder example (The input data comes from file)
*
*/

#ifndef DF_FILE_FEEDER_H
#define DF_FILE_FEEDER_H

#include "CtCvAlgoWrapper.h"

#define MAX_FEEDER_FN_LEN          (128U)

typedef struct {
  UINT32    Slot;
  UINT32    MemPoolId;
  UINT32    Num;
  char      Filename[FLEXIDAG_MAX_OUTPUTS][MAX_FEEDER_FN_LEN];
} FILE_FEEDER_CFG_s;

typedef struct {
  UINT32    Slot;
  UINT32    MemPoolId;
  char      FilenameY[MAX_FEEDER_FN_LEN];
  char      FilenameUV[MAX_FEEDER_FN_LEN];
  UINT32    PyramidScale;
  UINT32    RoiPitch;
  UINT32    RoiWidth;
  UINT32    RoiHeight;
  UINT32    OffsetX;
  UINT32    OffsetY;
  UINT32    DataWidth;
  UINT32    DataHeight;
} IDSPFILE_FEEDER_CFG_s;

typedef struct {
  UINT32    Slot;
  UINT32    MemPoolId;
  char      Filename[MAX_FEEDER_FN_LEN];
} SECRET_FEEDER_CFG_s;

/******************************************************************************
 *  Defined in FileFeeder.c
 ******************************************************************************/

UINT32 FileFeeder_Start(const FILE_FEEDER_CFG_s *pFeederCfg);
UINT32 IdspFileFeeder_Start(const IDSPFILE_FEEDER_CFG_s *pFeederCfg);
UINT32 SecretFeeder_Start(const SECRET_FEEDER_CFG_s *pFeederCfg);

#endif /* DF_FILE_FEEDER_H */

