/**
 *  @file SvcCvFlow_AmbaODTable.c
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
 *  @details Table of MEMIO based Ambarella Object Detection
 *
 */

#include "AmbaTypes.h"
#include "AmbaDSP_Liveview.h"

#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_ambaod.h"
#include "cvapi_svccvalgo_ambaperception.h"

#include "SvcCvFlowProfile.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_AmbaOD.h"

extern SVC_CV_FLOW_AMBAOD_NMS_CFG_s AmbaOD_NmsCfg_FC_Day;
extern SVC_CV_FLOW_AMBAOD_NMS_CFG_s AmbaOD_NmsCfg_FC_Night;
extern SVC_CV_FLOW_AMBAOD_NMS_CFG_s AmbaOD_NmsCfg_FC_OD37_Default;

extern SVC_CV_FLOW_AMBAOD_NMS_CFG_s AmbaOD_NmsCfg_EMR_Day;
extern SVC_CV_FLOW_AMBAOD_NMS_CFG_s AmbaOD_NmsCfg_EMR_Night;
extern SVC_CV_FLOW_AMBAOD_NMS_CFG_s AmbaOD_NmsCfg_EMR_OD37_Default;

extern SVC_CV_FLOW_AMBAOD_PCPT_FEATURE_s AmbaOD_Pcpt_2D;
extern SVC_CV_FLOW_AMBAOD_PCPT_FEATURE_s AmbaOD_Pcpt_Det;
extern SVC_CV_FLOW_AMBAOD_PCPT_FEATURE_s AmbaOD_Pcpt_Clf;
extern SVC_CV_FLOW_AMBAOD_PCPT_FEATURE_s AmbaOD_Pcpt_Det_Clf;

SVC_CV_FLOW_AMBAOD_NMS_CFG_s AmbaOD_NmsCfg_FC_Day = {
    /* a10 day */
    .NumConfig = 13,
    .Param = {
        [0]  = { .MaxResult = 50,    .ScoreTh = 0,          .IOUTh = 0,    .IOMTh = 0, },
        [1]  = { .MaxResult = 50,    .ScoreTh = 16384,    .IOUTh = 0,    .IOMTh = 0, },
        [2]  = { .MaxResult = 50,    .ScoreTh = 27197,    .IOUTh = 0,    .IOMTh = 0, },
        [3]  = { .MaxResult = 50,    .ScoreTh = 26214,    .IOUTh = 0,    .IOMTh = 0, },
        [4]  = { .MaxResult = 50,    .ScoreTh = 24576,    .IOUTh = 0,    .IOMTh = 0, },
        [5]  = { .MaxResult = 50,    .ScoreTh = 16384,    .IOUTh = 0,    .IOMTh = 0, },
        [6]  = { .MaxResult = 50,    .ScoreTh = 22938,    .IOUTh = 0,    .IOMTh = 0, },
        [7]  = { .MaxResult = 50,    .ScoreTh = 25231,    .IOUTh = 0,    .IOMTh = 0, },
        [8]  = { .MaxResult = 50,    .ScoreTh = 19661,    .IOUTh = 0,    .IOMTh = 0, },
        [9]  = { .MaxResult = 50,    .ScoreTh = 18022,    .IOUTh = 0,    .IOMTh = 0, },
        [10] = { .MaxResult = 50,    .ScoreTh = 16384,    .IOUTh = 0,    .IOMTh = 0, },
        [11] = { .MaxResult = 50,    .ScoreTh = 29164,    .IOUTh = 0,    .IOMTh = 0, },
        [12] = { .MaxResult = 50,    .ScoreTh = 27197,    .IOUTh = 0,    .IOMTh = 0, },
        [13] = { .MaxResult = 0,      .ScoreTh = 0,         .IOUTh = 0,    .IOMTh = 0, }, /* Initialize */
        [14] = { .MaxResult = 0,      .ScoreTh = 0,         .IOUTh = 0,    .IOMTh = 0, }, /* Initialize */
        [15] = { .MaxResult = 0,      .ScoreTh = 0,         .IOUTh = 0,    .IOMTh = 0, }  /* Initialize */
    }
};

SVC_CV_FLOW_AMBAOD_NMS_CFG_s AmbaOD_NmsCfg_FC_Night = {
    /* a10 night */
    .NumConfig = 13,
    .Param = {
        [0]  = { .MaxResult = 50,    .ScoreTh = 0,          .IOUTh = 0,    .IOMTh = 0, },
        [1]  = { .MaxResult = 50,    .ScoreTh = 18350,    .IOUTh = 0,    .IOMTh = 0, },
        [2]  = { .MaxResult = 50,    .ScoreTh = 27197,    .IOUTh = 0,    .IOMTh = 0, },
        [3]  = { .MaxResult = 50,    .ScoreTh = 28836,    .IOUTh = 0,    .IOMTh = 0, },
        [4]  = { .MaxResult = 50,    .ScoreTh = 21299,    .IOUTh = 0,    .IOMTh = 0, },
        [5]  = { .MaxResult = 50,    .ScoreTh = 16384,    .IOUTh = 0,    .IOMTh = 0, },
        [6]  = { .MaxResult = 50,    .ScoreTh = 24248,    .IOUTh = 0,    .IOMTh = 0, },
        [7]  = { .MaxResult = 50,    .ScoreTh = 26870,    .IOUTh = 0,    .IOMTh = 0, },
        [8]  = { .MaxResult = 50,    .ScoreTh = 18678,    .IOUTh = 0,    .IOMTh = 0, },
        [9]  = { .MaxResult = 50,    .ScoreTh = 20316,    .IOUTh = 0,    .IOMTh = 0, },
        [10] = { .MaxResult = 50,    .ScoreTh = 20316,    .IOUTh = 0,    .IOMTh = 0, },
        [11] = { .MaxResult = 50,    .ScoreTh = 28180,    .IOUTh = 0,    .IOMTh = 0, },
        [12] = { .MaxResult = 50,    .ScoreTh = 26214,    .IOUTh = 0,    .IOMTh = 0, },
        [13] = { .MaxResult = 0,      .ScoreTh = 0,         .IOUTh = 0,    .IOMTh = 0, }, /* Initialize */
        [14] = { .MaxResult = 0,      .ScoreTh = 0,         .IOUTh = 0,    .IOMTh = 0, }, /* Initialize */
        [15] = { .MaxResult = 0,      .ScoreTh = 0,         .IOUTh = 0,    .IOMTh = 0, }  /* Initialize */
    }
};

SVC_CV_FLOW_AMBAOD_NMS_CFG_s AmbaOD_NmsCfg_FC_OD37_Default = {
    /* OD v37 perception */
    .NumConfig = 16,
    .Param = {
        [0]  = { .MaxResult = 100,    .ScoreTh = 204,   .IOUTh = 128,    .IOMTh = 0,  },
        [1]  = { .MaxResult = 100,    .ScoreTh = 204,   .IOUTh = 128,    .IOMTh = 0,  },
        [2]  = { .MaxResult = 100,    .ScoreTh = 192,   .IOUTh = 128,    .IOMTh = 0,  },
        [3]  = { .MaxResult = 100,    .ScoreTh = 204,   .IOUTh = 128,    .IOMTh = 0,  },
        [4]  = { .MaxResult = 100,    .ScoreTh = 192,   .IOUTh = 128,    .IOMTh = 0,  },
        [5]  = { .MaxResult = 100,    .ScoreTh = 192,   .IOUTh = 128,    .IOMTh = 0,  },
        [6]  = { .MaxResult = 100,    .ScoreTh = 179,   .IOUTh = 128,    .IOMTh = 26, },
        [7]  = { .MaxResult = 100,    .ScoreTh = 204,   .IOUTh = 128,    .IOMTh = 26, },
        [8]  = { .MaxResult = 100,    .ScoreTh = 204,   .IOUTh = 77,     .IOMTh = 0,  },
        [9]  = { .MaxResult = 100,    .ScoreTh = 192,   .IOUTh = 26,     .IOMTh = 0,  },
        [10] = { .MaxResult = 100,    .ScoreTh = 204,   .IOUTh = 26,     .IOMTh = 0,  },
        [11] = { .MaxResult = 100,    .ScoreTh = 179,   .IOUTh = 26,     .IOMTh = 0,  },
        [12] = { .MaxResult = 100,    .ScoreTh = 204,   .IOUTh = 77,     .IOMTh = 0,  },
        [13] = { .MaxResult = 100,    .ScoreTh = 204,   .IOUTh = 26,     .IOMTh = 0,  },
        [14] = { .MaxResult = 100,    .ScoreTh = 192,   .IOUTh = 128,    .IOMTh = 0,  },
        [15] = { .MaxResult = 100,    .ScoreTh = 204,   .IOUTh = 26,     .IOMTh = 0,  }
    }
};

SVC_CV_FLOW_AMBAOD_NMS_CFG_s AmbaOD_NmsCfg_EMR_Day = {
    /* a3 day */
    .NumConfig = 4,
    .Param = {
        [0]  = { .MaxResult = 50,    .ScoreTh = 32768,   .IOUTh = 0,    .IOMTh = 0, },
        [1]  = { .MaxResult = 50,    .ScoreTh = 14746,   .IOUTh = 0,    .IOMTh = 0, },
        [2]  = { .MaxResult = 50,    .ScoreTh = 26214,   .IOUTh = 0,    .IOMTh = 0, },
        [3]  = { .MaxResult = 50,    .ScoreTh = 18022,   .IOUTh = 0,    .IOMTh = 0, },
        [4]  = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [5]  = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [6]  = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [7]  = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [8]  = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [9]  = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [10] = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [11] = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [12] = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [13] = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [14] = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [15] = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, }   /* Initialize */
    }
};

SVC_CV_FLOW_AMBAOD_NMS_CFG_s AmbaOD_NmsCfg_EMR_Night = {
    /* a3 night */
    .NumConfig = 4,
    .Param = {
        [0]  = { .MaxResult = 50,    .ScoreTh = 32768,   .IOUTh = 0,    .IOMTh = 0, },
        [1]  = { .MaxResult = 50,    .ScoreTh = 16712,   .IOUTh = 0,    .IOMTh = 0, },
        [2]  = { .MaxResult = 50,    .ScoreTh = 21299,   .IOUTh = 0,    .IOMTh = 0, },
        [3]  = { .MaxResult = 50,    .ScoreTh = 26542,   .IOUTh = 0,    .IOMTh = 0, },
        [4]  = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [5]  = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [6]  = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [7]  = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [8]  = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [9]  = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [10] = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [11] = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [12] = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [13] = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [14] = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [15] = { .MaxResult = 0,      .ScoreTh = 0,        .IOUTh = 0,    .IOMTh = 0, }   /* Initialize */
    }
};

SVC_CV_FLOW_AMBAOD_NMS_CFG_s AmbaOD_NmsCfg_EMR_OD37_Default = {
    /* OD v37 perception */
    .NumConfig = 6,
    .Param = {
        [0]  = { .MaxResult = 50,    .ScoreTh = 204,   .IOUTh = 128,  .IOMTh = 0, },
        [1]  = { .MaxResult = 50,    .ScoreTh = 204,   .IOUTh = 128,  .IOMTh = 0, },
        [2]  = { .MaxResult = 50,    .ScoreTh = 192,   .IOUTh = 128,  .IOMTh = 0, },
        [3]  = { .MaxResult = 50,    .ScoreTh = 204,   .IOUTh = 128,  .IOMTh = 0, },
        [4]  = { .MaxResult = 50,    .ScoreTh = 192,   .IOUTh = 128,  .IOMTh = 0, },
        [5]  = { .MaxResult = 50,    .ScoreTh = 192,   .IOUTh = 128,  .IOMTh = 0, },
        [6]  = { .MaxResult = 0,     .ScoreTh = 0,     .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [7]  = { .MaxResult = 0,     .ScoreTh = 0,     .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [8]  = { .MaxResult = 0,     .ScoreTh = 0,     .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [9]  = { .MaxResult = 0,     .ScoreTh = 0,     .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [10] = { .MaxResult = 0,     .ScoreTh = 0,     .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [11] = { .MaxResult = 0,     .ScoreTh = 0,     .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [12] = { .MaxResult = 0,     .ScoreTh = 0,     .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [13] = { .MaxResult = 0,     .ScoreTh = 0,     .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [14] = { .MaxResult = 0,     .ScoreTh = 0,     .IOUTh = 0,    .IOMTh = 0, },  /* Initialize */
        [15] = { .MaxResult = 0,     .ScoreTh = 0,     .IOUTh = 0,    .IOMTh = 0, }   /* Initialize */
    }
};

SVC_CV_FLOW_AMBAOD_PCPT_FEATURE_s AmbaOD_Pcpt_2D = {
    .EnableKP     = 0U,
    .Enable3D     = 0U,
    .EnableMask   = 0U,
    .OutBaseWidth = 0U,    .OutBaseHeight = 0U,
    .EnableTS     = 0U,
    .EnableSL     = 0U,
    .EnableTLC    = 0U,                             /* Use 2D TL detection */
    .EnableTLS    = 0U,
    .EnableAR     = 0U,
};

SVC_CV_FLOW_AMBAOD_PCPT_FEATURE_s AmbaOD_Pcpt_Det = {
    .EnableKP     = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN),
    .Enable3D     = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN),
    .EnableMask   = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN),
    .OutBaseWidth = 903,    .OutBaseHeight = 450,   /* for 960 x 540 OSD */
    .EnableTS     = 0U,
    .EnableSL     = 0U,
    .EnableTLC    = 0U,                             /* Use 2D TL detection */
    .EnableTLS    = 0U,
    .EnableAR     = 0U,
};

SVC_CV_FLOW_AMBAOD_PCPT_FEATURE_s AmbaOD_Pcpt_Clf = {
    .EnableKP     = 0U,
    .Enable3D     = 0U,
    .EnableMask   = 0U,
    .OutBaseWidth = 0U,    .OutBaseHeight = 0U,
    .EnableTS     = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN),
    .EnableSL     = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN),
    .EnableTLC    = 0U,                             /* Use 2D TL detection */
    .EnableTLS    = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN),
    .EnableAR     = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN),
};

SVC_CV_FLOW_AMBAOD_PCPT_FEATURE_s AmbaOD_Pcpt_Det_Clf = {
    .EnableKP     = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN),
    .Enable3D     = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN),
    .EnableMask   = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN),
    .OutBaseWidth = 903,    .OutBaseHeight = 450,   /* for 960 x 540 OSD */
    .EnableTS     = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN),
    .EnableSL     = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN),
    .EnableTLC    = 0U,                             /* Use 2D TL detection */
    .EnableTLS    = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN),
    .EnableAR     = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN),
};

const SVC_CV_FLOW_AMBAOD_MODE_INFO_s SvcCvFlow_AmbaOD_ModeInfo[AMBAOD_NUM_MODE] = {
    [AMBAOD_FDAG_FC] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAODFC
                    .pAlgoObj = &AmbaODAlgoObj,
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "flexibin_ambaod_fc.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaod_fc.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL},
#elif defined CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "flexibin_ambaperception_2d.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
#else
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL},
#endif
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH, .NetHeight = AMBAOD_FC_NET_HEIGHT,
#ifdef  CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_2D,
#else
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_Day,
                    &AmbaOD_NmsCfg_FC_Night,
                    NULL,
                },
                .pFeatureCtrl = NULL,
#endif
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        }
    },
    [AMBAOD_FDAG_FC_2SCALE] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAODFC
                    .pAlgoObj = &AmbaODAlgoObj,
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "flexibin_ambaod_fc_2scale.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaod_fc.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
#elif defined CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "flexibin_ambaperception_2d_2scale.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
#else
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
#endif
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH, .NetHeight = AMBAOD_FC_NET_HEIGHT,
#ifdef  CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_2D,
#else
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_Day,
                    &AmbaOD_NmsCfg_FC_Night,
                    NULL,
                },
                .pFeatureCtrl = NULL,
#endif
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        }
    },
    [AMBAOD_FDAG_EMR] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAODEM
                    .pAlgoObj = &AmbaODAlgoObj,
#else
    #ifdef CONFIG_CV_FLEXIDAG_AMBAOD37FC
                    .pAlgoObj = &AmbaOD37AlgoObj,
    #else
                   .pAlgoObj = NULL,
    #endif
#endif
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "flexibin_ambaod_em.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaod_em.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_EMR_NET_WIDTH, .NetHeight = AMBAOD_EMR_NET_HEIGHT,
                .pNmsCfg = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAOD37FC
                    &AmbaOD_NmsCfg_EMR_OD37_Default,
                    NULL,
                    NULL,
#else
                    &AmbaOD_NmsCfg_EMR_Day,
                    &AmbaOD_NmsCfg_EMR_Night,
                    NULL,
#endif
                },
                .pFeatureCtrl = NULL,
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        }
    },
    [AMBAOD_FDAG_EMR_SMALL] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAODEM
                    .pAlgoObj = &AmbaODAlgoObj,
#else
  #ifdef CONFIG_CV_FLEXIDAG_AMBAOD37FC
                    .pAlgoObj = &AmbaOD37AlgoObj,
  #else
                   .pAlgoObj = NULL,
  #endif
#endif
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "flexibin_ambaod_em_1024x512.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexibin_ambaod_em.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_EMR_NET_WIDTH, .NetHeight = AMBAOD_EMR_NET_HEIGHT,
                .pNmsCfg = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAOD37FC
                    &AmbaOD_NmsCfg_EMR_OD37_Default,
                    NULL,
                    NULL,
#else
                    &AmbaOD_NmsCfg_EMR_Day,
                    &AmbaOD_NmsCfg_EMR_Night,
                    NULL,
#endif
                },
                .pFeatureCtrl = NULL,
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        }
    },
    [AMBAOD_FDAG_EMR_1408x512] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAODEM
                    .pAlgoObj = &AmbaODAlgoObj,
#else
    #ifdef CONFIG_CV_FLEXIDAG_AMBAOD37FC
                    .pAlgoObj = &AmbaOD37AlgoObj,
    #else
                   .pAlgoObj = NULL,
    #endif
#endif
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "flexibin_ambaod_em_1408x512.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexibin_ambaod_em.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = 1408, .NetHeight = 512,
                .pNmsCfg = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAOD37FC
                    &AmbaOD_NmsCfg_EMR_OD37_Default,
                    NULL,
                    NULL,
#else
                    &AmbaOD_NmsCfg_EMR_Day,
                    &AmbaOD_NmsCfg_EMR_Night,
                    NULL,
#endif
                },
                .pFeatureCtrl = NULL,
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        }
    },
    [AMBAOD_FDAG_EMR_896x512] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAODEM
                    .pAlgoObj = &AmbaODAlgoObj,
#else
    #ifdef CONFIG_CV_FLEXIDAG_AMBAOD37FC
                    .pAlgoObj = &AmbaOD37AlgoObj,
    #else
                   .pAlgoObj = NULL,
    #endif
#endif
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "flexibin_ambaod_em_896x512.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexibin_ambaod_em.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = 896, .NetHeight = 512,
                .pNmsCfg = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAOD37FC
                    &AmbaOD_NmsCfg_EMR_OD37_Default,
                    NULL,
                    NULL,
#else
                    &AmbaOD_NmsCfg_EMR_Day,
                    &AmbaOD_NmsCfg_EMR_Night,
                    NULL,
#endif
                },
                .pFeatureCtrl = NULL,
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        }
    },
    [AMBAOD_FDAG_PCPT_OD] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D] = "flexibin_ambaperception_2d.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH, .NetHeight = AMBAOD_FC_NET_HEIGHT,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_2D,
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
    },
    [AMBAOD_FDAG_PCPT_OD_2SC] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D] = "flexibin_ambaperception_2d_2scale.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH, .NetHeight = AMBAOD_FC_NET_HEIGHT,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_2D,
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
    },
    [AMBAOD_FDAG_PCPT_OD_DET] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 5, .FlexiBinBits = 0x1F,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D]  = "flexibin_ambaperception_2d.bin",
                        [AMBANET_OD37_KP]  = "flexibin_ambaperception_kp.bin",
                        [AMBANET_OD37_3D]  = "flexibin_ambaperception_3d.bin",
                        [AMBANET_OD37_MKL] = "flexibin_ambaperception_mkl.bin",
                        [AMBANET_OD37_MKS] = "flexibin_ambaperception_mks.bin",
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            },
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH, .NetHeight = AMBAOD_FC_NET_HEIGHT,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_Det
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
        .FlowCtrl = AMBAOD_FLOW_2STAGE,
    },
    [AMBAOD_FDAG_PCPT_OD_2SC_DET] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 5, .FlexiBinBits = 0x1F,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D]  = "flexibin_ambaperception_2d_2scale.bin",
                        [AMBANET_OD37_KP]  = "flexibin_ambaperception_kp.bin",
                        [AMBANET_OD37_3D]  = "flexibin_ambaperception_3d.bin",
                        [AMBANET_OD37_MKL] = "flexibin_ambaperception_mkl.bin",
                        [AMBANET_OD37_MKS] = "flexibin_ambaperception_mks.bin",
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH, .NetHeight = AMBAOD_FC_NET_HEIGHT,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_Det
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
        .FlowCtrl = AMBAOD_FLOW_2STAGE,
    },
    [AMBAOD_FDAG_PCPT_OD_CLF] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 5, .FlexiBinBits = 0x361,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D]  = "flexibin_ambaperception_2d.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [AMBANET_OD37_TS]  = "flexibin_ambaperception_tsc.bin",
                        [AMBANET_OD37_SL]  = "flexibin_ambaperception_slc.bin",
                        [7] = NULL,
                        [AMBANET_OD37_TLS] = "flexibin_ambaperception_tlsc.bin",
                        [AMBANET_OD37_AR]  = "flexibin_ambaperception_arc.bin",
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH, .NetHeight = AMBAOD_FC_NET_HEIGHT,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_Clf
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
        .FlowCtrl = AMBAOD_FLOW_2STAGE,
    },
    [AMBAOD_FDAG_PCPT_OD_2SC_CLF] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 5, .FlexiBinBits = 0x361,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D]  = "flexibin_ambaperception_2d_2scale.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [AMBANET_OD37_TS]  = "flexibin_ambaperception_tsc.bin",
                        [AMBANET_OD37_SL]  = "flexibin_ambaperception_slc.bin",
                        [7] = NULL,
                        [AMBANET_OD37_TLS] = "flexibin_ambaperception_tlsc.bin",
                        [AMBANET_OD37_AR]  = "flexibin_ambaperception_arc.bin",
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH, .NetHeight = AMBAOD_FC_NET_HEIGHT,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_Clf
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
        .FlowCtrl = AMBAOD_FLOW_2STAGE,
    },
    [AMBAOD_FDAG_PCPT_OD_DET_CLF] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 9, .FlexiBinBits = 0x37F,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D]  = "flexibin_ambaperception_2d.bin",
                        [AMBANET_OD37_KP]  = "flexibin_ambaperception_kp.bin",
                        [AMBANET_OD37_3D]  = "flexibin_ambaperception_3d.bin",
                        [AMBANET_OD37_MKL] = "flexibin_ambaperception_mkl.bin",
                        [AMBANET_OD37_MKS] = "flexibin_ambaperception_mks.bin",
                        [AMBANET_OD37_TS]  = "flexibin_ambaperception_tsc.bin",
                        [AMBANET_OD37_SL]  = "flexibin_ambaperception_slc.bin",
                        [7] = NULL,
                        [AMBANET_OD37_TLS] = "flexibin_ambaperception_tlsc.bin",
                        [AMBANET_OD37_AR]  = "flexibin_ambaperception_arc.bin",
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH, .NetHeight = AMBAOD_FC_NET_HEIGHT,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_Det_Clf
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
        .FlowCtrl = AMBAOD_FLOW_2STAGE,
    },
    [AMBAOD_FDAG_PCPT_OD_2SC_DET_CLF] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 9, .FlexiBinBits = 0x37F,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D]  = "flexibin_ambaperception_2d_2scale.bin",
                        [AMBANET_OD37_KP]  = "flexibin_ambaperception_kp.bin",
                        [AMBANET_OD37_3D]  = "flexibin_ambaperception_3d.bin",
                        [AMBANET_OD37_MKL] = "flexibin_ambaperception_mkl.bin",
                        [AMBANET_OD37_MKS] = "flexibin_ambaperception_mks.bin",
                        [AMBANET_OD37_TS]  = "flexibin_ambaperception_tsc.bin",
                        [AMBANET_OD37_SL]  = "flexibin_ambaperception_slc.bin",
                        [7] = NULL,
                        [AMBANET_OD37_TLS] = "flexibin_ambaperception_tlsc.bin",
                        [AMBANET_OD37_AR]  = "flexibin_ambaperception_arc.bin",
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH, .NetHeight = AMBAOD_FC_NET_HEIGHT,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_Det_Clf
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
        .FlowCtrl = AMBAOD_FLOW_2STAGE,
    },
    [AMBAOD_FDAG_PCPT_OD_RES1] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D] = "flexibin_ambaperception_2d_896x512.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH_RES1, .NetHeight = AMBAOD_FC_NET_HEIGHT_RES1,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_2D,
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
    },
    [AMBAOD_FDAG_PCPT_OD_RES1_2SC] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D] = "flexibin_ambaperception_2d_2scale_896x512.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH_RES1, .NetHeight = AMBAOD_FC_NET_HEIGHT_RES1,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_2D,
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
    },
    [AMBAOD_FDAG_PCPT_OD_RES1_DET] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 5, .FlexiBinBits = 0x1F,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D]  = "flexibin_ambaperception_2d_896x512.bin",
                        [AMBANET_OD37_KP]  = "flexibin_ambaperception_kp.bin",
                        [AMBANET_OD37_3D]  = "flexibin_ambaperception_3d.bin",
                        [AMBANET_OD37_MKL] = "flexibin_ambaperception_mkl.bin",
                        [AMBANET_OD37_MKS] = "flexibin_ambaperception_mks.bin",
                        [5] = NULL,
                        [6] = NULL,
                        [7] = NULL,
                        [8] = NULL,
                        [9] = NULL,
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH_RES1, .NetHeight = AMBAOD_FC_NET_HEIGHT_RES1,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_Det
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
        .FlowCtrl = AMBAOD_FLOW_2STAGE,
    },
    [AMBAOD_FDAG_PCPT_OD_RES1_CLF] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 5, .FlexiBinBits = 0x361,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D]  = "flexibin_ambaperception_2d_896x512.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [AMBANET_OD37_TS]  = "flexibin_ambaperception_tsc.bin",
                        [AMBANET_OD37_SL]  = "flexibin_ambaperception_slc.bin",
                        [7] = NULL,
                        [AMBANET_OD37_TLS] = "flexibin_ambaperception_tlsc.bin",
                        [AMBANET_OD37_AR]  = "flexibin_ambaperception_arc.bin",
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH_RES1, .NetHeight = AMBAOD_FC_NET_HEIGHT_RES1,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_Clf
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
        .FlowCtrl = AMBAOD_FLOW_2STAGE,
    },
    [AMBAOD_FDAG_PCPT_OD_RES1_2SC_CLF] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 5, .FlexiBinBits = 0x361,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D]  = "flexibin_ambaperception_2d_2scale_896x512.bin",
                        [1] = NULL,
                        [2] = NULL,
                        [3] = NULL,
                        [4] = NULL,
                        [AMBANET_OD37_TS]  = "flexibin_ambaperception_tsc.bin",
                        [AMBANET_OD37_SL]  = "flexibin_ambaperception_slc.bin",
                        [7] = NULL,
                        [AMBANET_OD37_TLS] = "flexibin_ambaperception_tlsc.bin",
                        [AMBANET_OD37_AR]  = "flexibin_ambaperception_arc.bin",
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH_RES1, .NetHeight = AMBAOD_FC_NET_HEIGHT_RES1,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_Clf
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
        .FlowCtrl = AMBAOD_FLOW_2STAGE,
    },
    [AMBAOD_FDAG_PCPT_OD_RES1_DET_CLF] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 9, .FlexiBinBits = 0x37F,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D]  = "flexibin_ambaperception_2d_896x512.bin",
                        [AMBANET_OD37_KP]  = "flexibin_ambaperception_kp.bin",
                        [AMBANET_OD37_3D]  = "flexibin_ambaperception_3d.bin",
                        [AMBANET_OD37_MKL] = "flexibin_ambaperception_mkl.bin",
                        [AMBANET_OD37_MKS] = "flexibin_ambaperception_mks.bin",
                        [AMBANET_OD37_TS]  = "flexibin_ambaperception_tsc.bin",
                        [AMBANET_OD37_SL]  = "flexibin_ambaperception_slc.bin",
                        [7] = NULL,
                        [AMBANET_OD37_TLS] = "flexibin_ambaperception_tlsc.bin",
                        [AMBANET_OD37_AR]  = "flexibin_ambaperception_arc.bin",
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH_RES1, .NetHeight = AMBAOD_FC_NET_HEIGHT_RES1,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_Det_Clf
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
        .FlowCtrl = AMBAOD_FLOW_2STAGE,
    },
    [AMBAOD_FDAG_PCPT_OD_RES1_2SC_DET_CLF] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
                    .pAlgoObj = &AmbaPcptAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 9, .FlexiBinBits = 0x37F,
                    .FlexiBinList = {
                        [AMBANET_OD37_2D]  = "flexibin_ambaperception_2d_2scale_896x512.bin",
                        [AMBANET_OD37_KP]  = "flexibin_ambaperception_kp.bin",
                        [AMBANET_OD37_3D]  = "flexibin_ambaperception_3d.bin",
                        [AMBANET_OD37_MKL] = "flexibin_ambaperception_mkl.bin",
                        [AMBANET_OD37_MKS] = "flexibin_ambaperception_mks.bin",
                        [AMBANET_OD37_TS]  = "flexibin_ambaperception_tsc.bin",
                        [AMBANET_OD37_SL]  = "flexibin_ambaperception_slc.bin",
                        [7] = NULL,
                        [AMBANET_OD37_TLS] = "flexibin_ambaperception_tlsc.bin",
                        [AMBANET_OD37_AR]  = "flexibin_ambaperception_arc.bin",
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaperception.lcs"
                    },
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [1] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [2] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [3] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
                [4] = {
                    .pAlgoObj = NULL,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .FlexiBinList = {NULL},
                    .NumLcsBin = 0,
                    .LcsBinList = {NULL},
                    .ExtBinType = {0},
                    .ExtBinList = {NULL}
                },
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBAOD_FC_NET_WIDTH_RES1, .NetHeight = AMBAOD_FC_NET_HEIGHT_RES1,
                .pNmsCfg = {
                    &AmbaOD_NmsCfg_FC_OD37_Default,
                    NULL,
                    NULL,
                },
                .pFeatureCtrl = &AmbaOD_Pcpt_Det_Clf
            },
            [1] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [2] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [3] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            },
            [4] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pNmsCfg = {NULL},
                .pFeatureCtrl = NULL,
            }
        },
        .OutputType = AMBAOD_OUTPUT_TYPE_PERCEPTION,
        .FlowCtrl = AMBAOD_FLOW_2STAGE,
    },
};

