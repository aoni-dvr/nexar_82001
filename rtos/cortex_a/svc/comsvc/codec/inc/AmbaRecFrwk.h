/**
*  @file AmbaRecFrwk.h
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
*  @details Amba record framework interface
*
*/

#ifndef AMBA_REC_FRWK_H
#define AMBA_REC_FRWK_H

#include "AmbaMux_Def.h"

typedef struct {
    /* record source information */
    UINT32  SrcBit;
    UINT32  SrcType;
    UINT32  SrcSubType;
    void    *pSrcData;
    ULONG   SrcBufBase;
    UINT32  SrcBufSize;

    /* record box information */
    UINT32  IsBoxDataValid;
    UINT32  BoxBit;
    UINT32  BoxType;
    UINT32  BoxSubType;

    #define AMBA_BOX_BUF_CHAIN_MAX       (8U)
    UINT32  BoxBufNum;
    struct {
        ULONG   Base;
        UINT32  Size;
    } BoxBufChain[AMBA_BOX_BUF_CHAIN_MAX];

    ULONG   BoxDataAddr;
    UINT32  BoxDataSize;

    #define AMBA_BOX_BUF_UPDATE_MAX      (1U)
    UINT32  BoxUpdateCnt;
    struct {
        UINT64  Offset;    /* file offset */
        ULONG   Base;
        UINT32  Size;
    } BoxUpdate[AMBA_BOX_BUF_UPDATE_MAX];

    /* other information */
    #define AMBA_REC_DDESC_FLAG_EOS      (0x00000001U)
    #define AMBA_REC_DDESC_FLAG_SPLIT    (0x00000002U)
    UINT32  DescFlags;
} AMBA_REC_FRWK_DESC_s;

#endif  /* AMBA_REC_FRWK_H */
