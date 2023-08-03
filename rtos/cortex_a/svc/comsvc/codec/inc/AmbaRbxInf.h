/**
*  @file AmbaRbxInf.h
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
*  @details amba record box interface
*
*/

#ifndef AMBA_RBX_INF_H
#define AMBA_RBX_INF_H

#include "AmbaRecFrwk.h"

typedef struct {
    UINT64  ProcCount;
    UINT64  ProcSize;
} AMBA_RBX_STATIS_s;

typedef struct {
    ULONG   MemBase;
    UINT32  MemSize;
    UINT32  BoxType;
    UINT32  SubType;
    UINT32  InSrcBits;

    UINT32  VdCoding;
    UINT32  VdWidth;
    UINT32  VdHeight;
    UINT32  VdOrientation;
    UINT32  VdTimeScale;
    UINT32  VdNumUnitsInTick;

    UINT32  AuCoding;
    UINT32  AuVolume;
    UINT32  AuWBitsPerSample;
    UINT32  AuChannels;
    UINT32  AuBrate;
    UINT32  AuSample;
    UINT32  AuFormat;   /* 0; little, 1: big endian */
    UINT32  AuFrameSize;

    UINT32  TExist;
    UINT32  IsTimeLapse;

    UINT32  IsEncrypt;
    UINT32  SplitTime;

    AMBA_REC_EVAL_s  MiaInfo;
} AMBA_RBX_USR_CFG_s;

typedef struct AMBA_REC_BOX {
    /* fill by user */
    AMBA_RBX_USR_CFG_s  UsrCfg;

    /* fill internally */
    UINT32              BoxBit;
    UINT32              (*pfnProc)(const struct AMBA_REC_BOX *pRbxCtrl, AMBA_REC_FRWK_DESC_s *pDesc);
    struct AMBA_REC_BOX  *pNext;
    UINT8               PrivData[CONFIG_AMBA_REC_RBX_PRIV_SIZE];
} AMBA_REC_BOX_s;

#endif  /* AMBA_RBX_INF_H */
