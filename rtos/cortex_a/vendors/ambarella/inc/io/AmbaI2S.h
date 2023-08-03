/**
 *  @file AmbaI2S.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for I2S Middleware APIs
 *
 */

#ifndef AMBA_I2S_H
#define AMBA_I2S_H

#ifndef AMBA_I2S_DEF_H
#include "AmbaI2S_Def.h"
#endif

/*
 * Defined in AmbaI2S.c
 */
UINT32 AmbaI2S_Config(UINT32 I2sPortIdx, const AMBA_I2S_CTRL_s *pCtrl);
UINT32 AmbaI2S_RxTrigger(UINT32 I2sPortIdx, UINT32 Cmd);
UINT32 AmbaI2S_TxTrigger(UINT32 I2sPortIdx, UINT32 Cmd);
UINT32 AmbaI2S_RxResetFifo(UINT32 I2sPortIdx);
UINT32 AmbaI2S_TxResetFifo(UINT32 I2sPortIdx);

#endif /* AMBA_I2S_H */
