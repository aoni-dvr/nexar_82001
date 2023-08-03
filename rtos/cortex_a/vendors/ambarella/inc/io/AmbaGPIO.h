/**
 *  @file AmbaGPIO.h
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
 *  @details Definitions & Constants for GPIO Middleware APIs
 *
 */

#ifndef AMBA_GPIO_H
#define AMBA_GPIO_H

#ifndef AMBA_GPIO_DEF_H
#include "AmbaGPIO_Def.h"
#endif

/*
 * Defined in AmbaGPIO.c
 */
UINT32 AmbaGPIO_LoadDefaultRegVals(const AMBA_GPIO_DEFAULT_s * pDefaultParam);
UINT32 AmbaGPIO_SetFuncGPI(UINT32 PinID);
UINT32 AmbaGPIO_SetFuncGPO(UINT32 PinID, UINT32 PinLevel);
UINT32 AmbaGPIO_SetFuncAlt(UINT32 PinID);
UINT32 AmbaGPIO_SetDriveStrength(UINT32 PinID, UINT32 DriveStrength);
UINT32 AmbaGPIO_SetPullUpOrDown(UINT32 PinID, UINT32 PullUpOrDown);
UINT32 AmbaGPIO_IntSetType(UINT32 PinID, UINT32 IntConfig);
UINT32 AmbaGPIO_IntHookHandler(UINT32 PinID, AMBA_GPIO_ISR_f IntFunc, UINT32 IntFuncArg);
UINT32 AmbaGPIO_IntEnable(UINT32 PinID);
UINT32 AmbaGPIO_IntDisable(UINT32 PinID);
UINT32 AmbaGPIO_GetInfo(UINT32 PinID, AMBA_GPIO_INFO_s *pPinInfo);

#endif /* AMBA_GPIO_H */
