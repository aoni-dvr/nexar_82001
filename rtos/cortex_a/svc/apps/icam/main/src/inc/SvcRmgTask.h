/**
 *  @file SvcRmgTask.h
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
 *  @details svc RMG (Rear Mirror Guard) task header
 *
 */

#ifndef SVC_RMG_TASK_H
#define SVC_RMG_TASK_H

#include "AmbaOD_2DBbx.h"
#include "AmbaSurround.h"

#define RmgModeLiveview  0U
#define RmgModeDecode    1U
#define RmgModeNum       2U

#define RmgCatDebug    100U
#define RmgCatNone     101U
#define RmgCatEgo      102U
#define RmgCatSide     106U
#define RmgCatNum      111U

#define RmgColorEgo     28U /* yellow */
#define RmgColorSide    32U /* purple */
#define RmgColorDebug    0U /* black  */

#define RmgColorZone   0xFF00CC00U /* green */

typedef struct {
    UINT32                   Chan;
    UINT32                   FovIdx;
    AMBA_OD_2DBBX_LIST_s     *pBbx;
    const char               **Class_name;
    AMBA_SR_SROBJECT_DATA_s  *pSRData;
} SVC_RMG_DRAW_INFO_s;

UINT32 SvcRmgTask_Init(void);
UINT32 SvcRmgTask_Start(void);
UINT32 SvcRmgTask_Stop(void);
void   SvcRmgTask_SetDebugBbx(UINT32 Level);
void   SvcRmgTask_OsdEnable(UINT32 Enable);

#endif /* SVC_RMG_TASK_H */
