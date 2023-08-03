/**
 *  @file Res_PBK.c
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
 *  @details svc resolution config
 *
 */

#ifndef RES_PBK_X_X_H       /* Dir 4.10 */
#define RES_PBK_X_X_H

#include "AmbaVIN.h"
#include "AmbaSensor.h"
#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaFPD.h"
#include "AmbaHDMI_Def.h"

static const SVC_RES_CFG_s g_ResPbkCfg[] = {
{
#define MAX_DEC_WIDTH  (7680U)
#define MAX_DEC_HEIGHT (4320U)
    .FormatName = "(PBK) Max Resolution 8K",
    .VinTree = {
        "$[DEC_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 0U,
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, MAX_DEC_WIDTH, MAX_DEC_HEIGHT},
            .ActWin = {0},
            .MainWin = {MAX_DEC_WIDTH, MAX_DEC_HEIGHT},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = {.MaxHorWarpCompensation = 64U, .WarpLumaWaitLine = 64U, .PipeMode = SVC_VDOPIPE_DRAMEFCY,}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAX_DEC_WIDTH, MAX_DEC_HEIGHT}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
},
};

#endif /* RES_PBK_X_X_H */
