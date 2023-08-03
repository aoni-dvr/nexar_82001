/**
 *  @file SvcCvFlow_CnnTestbedTable.c
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
 *  @details Table of cnn_testbed SvcCvAlgo examples
 *
 */

#include "SvcCvFlow_CnnTestbed.h"

const SVC_CV_FLOW_CNN_TESTBED_MODE_INFO_s SvcCvFlow_CnnTestbed_ModeInfo[TESTBED_NUM_MODE] = {
    [TESTBED_CVALGO_SSD] = {
        // The default values are for 01000_mnet_ssd_adas_cf_pic
        .pAlgoObj = &CvAlgoObj_SSD,
        .TxRateCtrl = 1,
        .OsdPriority = 2,
        .SsdNmsCtrl = {
            .PriorBox  = "./mbox_priorbox.bin",
            .ClassNum  = 7,
            .ModelType = MODEL_TYPE_CAFFE,
            .Win = {
                .RoiWidth      = 1360,
                .RoiHeight     = 764,
                .RoiStartX     = 530,
                .RoiStartY     = 232,
                .NetworkWidth  = 300,
                .NetworkHeight = 300,
                .VisWidth      = 960,
                .VisHeight     = 540,
            }
        },
        .SegWinCtrl = {
            .RoiWidth      = 0,
            .RoiHeight     = 0,
            .RoiStartX     = 0,
            .RoiStartY     = 0,
            .NetworkWidth  = 0,
            .NetworkHeight = 0,
        },
    },
    [TESTBED_CVALGO_SEG] = {
        // The default values are for 02008_segnet_basic_prune90_cf_pic
        .pAlgoObj = &CvAlgoObj_SingleFD,
        .TxRateCtrl = 5,
        .OsdPriority = 1,
        .SsdNmsCtrl = {
            .PriorBox  = {0},
            .ClassNum  = 0,
            .ModelType = 0,
            .Win = {
                .RoiWidth      = 0,
                .RoiHeight     = 0,
                .RoiStartX     = 0,
                .RoiStartY     = 0,
                .NetworkWidth  = 0,
                .NetworkHeight = 0,
                .VisWidth      = 0,
                .VisHeight     = 0,
            },
        },
        .SegWinCtrl = {
            .RoiWidth      = 680,
            .RoiHeight     = 386,
            .RoiStartX     = 100,
            .RoiStartY     = 13,
            .NetworkWidth  = 480,
            .NetworkHeight = 360,
        },
    },
};

