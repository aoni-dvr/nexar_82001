/**
 *  @file SvcWdt.c
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
*  @details svc watching dog
*
*/

#ifndef SVC_WDT_H
#define SVC_WDT_H

#include "AmbaWDT.h"

#define SVC_WDT_SWITCH_ENA     (0U)
#define SVC_WDT_EXPIRE         (1U)

/* ----------------------------------------------------------------------------------------------------------------------- */
/*  in user partition table[0], there's reserved var in UINT8                                                              */
/*  watchdog flag is saved as below:                                                                                       */
/*                                                                                                                         */
/*  [0] SVC_WDT_SWITCH_ENA : flag for watchdog in bootloader,                                                              */
/*                           ena/disable system switch to another system (backup) when the origin system failed to boot.   */
/*  [1] SVC_WDT_EXPIRE     : this flag is for watchdog in bootloader,                                                      */
/*                           it will be set as 1 in bootloader, if it's not cleared as 0 in app,                           */
/*                           after reboot, bootloader will know the last time app failed to boot up.                       */
/* ----------------------------------------------------------------------------------------------------------------------- */

extern UINT32 SvcWdt_FeedTest;
extern UINT32 SvcWdt_FeedPrint;
extern UINT32 SvcWdt_FeedInterval;

UINT32 SvcWdt_Info(void);
UINT32 SvcWdt_GetFlag(UINT32 ID, UINT32* Ena);
UINT32 SvcWdt_SetFlag(UINT32 ID, UINT32  Ena);

void SvcWdt_WdtStop(void);
void SvcWdt_AppWdtStart(void);
void SvcWdt_AppWdtTaskCreate(void);
void SvcWdt_BldWdtClearExpireFlag(void);


#endif  /* SVC_WDT_H */
