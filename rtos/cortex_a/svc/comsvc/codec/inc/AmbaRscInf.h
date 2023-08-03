/**
*  @file AmbaRscInf.h
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
*  @details Amba record source interface
*
*/

#ifndef AMBA_RSC_INF_H
#define AMBA_RSC_INF_H

typedef struct {
    UINT64  RxCount;
    UINT64  RxSize;
    UINT64  RxHwTime;
    UINT64  RxLtHwTime;
    UINT64  RxLtHwTimeCarry;
    UINT64  RxLtExpTime;
    UINT64  RxLtExpTimeCarry;

    UINT32  QueLen;
} AMBA_RSC_STATIS_s;

typedef struct {
    ULONG   MemBase;
    UINT32  MemSize;
    UINT32  SrcType;
    UINT32  SubType;
    UINT32  TimeScale;
    UINT32  NumUnitsInTick;
    ULONG   BsBufBase;
    UINT32  BsBufSize;
    DOUBLE  FramePerFrag;
    UINT32  StreamID;
} AMBA_RSC_USR_CFG_s;

typedef struct AMBA_REC_SRC {
    /* fill by user */
    AMBA_RSC_USR_CFG_s  UsrCfg;

    /* fill internally */
    UINT32  SrcBit;
    UINT32  (*pfnEnque)(const struct AMBA_REC_SRC *pRscCtrl, void *pDataInfo);
    UINT32  (*pfnDeque)(const struct AMBA_REC_SRC *pRscCtrl, AMBA_REC_FRWK_DESC_s *pDesc);
    UINT32  (*pfnQueSize)(const struct AMBA_REC_SRC *pRscCtrl);
    UINT8   PrivData[CONFIG_AMBA_REC_RSC_PRIV_SIZE];
} AMBA_REC_SRC_s;

#endif  /* AMBA_RSC_INF_H */
