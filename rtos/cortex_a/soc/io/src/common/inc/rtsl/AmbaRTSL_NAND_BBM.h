/**
 *  @file AmbaRTSL_NAND_BBM.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for NAND Bad Block Management (BBM) RTSL APIs
 *
 */

#ifndef AMBA_RTSL_NAND_BBM_H
#define AMBA_RTSL_NAND_BBM_H

#ifndef AMBA_NAND_BBM_DEF_H
#include "AmbaNAND_BBM_Def.h"
#endif

/*
 * Defined in AmbaRTSL_NAND_BBM.c
 */
extern UINT32 *pAmbaNandTblBadBlk;

UINT32 AmbaRTSL_NandCreateBBT(void);
UINT32 AmbaRTSL_NandCreatePTB(void);
UINT32 AmbaRTSL_NandCreateExtUserPTB(UINT32 StartBlk);
UINT32 AmbaRTSL_NandGetBlkMark(UINT32 BlkAddr);
void  AmbaRTSL_NandSetBlkMark(UINT32 BlkAddr, UINT32 BlkMark);
INT32 AmbaRTSL_NandFindGoodBlkForward(UINT32 BlkAddr);
INT32 AmbaRTSL_NandFindGoodBlkBackwd(UINT32 BlkAddr);
UINT32 AmbaRTSL_NandUpdateBBT(UINT32 Version, UINT32 BlkAddrPrimary, UINT32 BlkAddrMirror);
UINT32 AmbaRTSL_NandLoadBBT(UINT32 BlkAddr);

#endif  /* AMBA_RTSL_NAND_BBM_H */
