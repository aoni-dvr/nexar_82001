/**
 *  @file AmbaPSD_CornerTrace.h
 *
 * Copyright (c) 2019 Ambarella International LP
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
 *  @details psd corner tracker
 *
 */

#ifndef AMBA_PSD_CORNERTRACE_H
#define AMBA_PSD_CORNERTRACE_H

#include "AmbaPSD_ParkingSpaceDetect.h"
#include "AmbaTypes.h"

#define HISTORY_STORE (1U)
#define MAX_VIEW    (4U)

// all real world coordinate is in mm

#define CORNER_STATUS_DETECTED      (0U)
#define CORNER_STATUS_ESTIMATE      (2U)

typedef struct {
    DOUBLE MergeTh;        /**< Merge corner threshold in mm, default is 500 */
    DOUBLE TraceTh;        /**< Trace corner threshold in mm, default is 500 */
    DOUBLE TraceAreaX0;    /**< x0 position of left bottom area for tracing corner, unit in mm */
    DOUBLE TraceAreaY0;    /**< y0 position of left bottom area for tracing corner, unit in mm */
    DOUBLE TraceAreaX1;    /**< x1 position of right top area for tracing corner, unit in mm */
    DOUBLE TraceAreaY1;    /**< y1 position of right top area for tracing corner, unit in mm */
} ParkingSpaceCornerCfg_s;

typedef struct {
    ParkingSpaceCornerCfg_s Cfg;            /**< Config of PSCT */
    AMBA_PSD_PROC_OUTPUT_DATA_s PreList;    /**< Previous list of parking space data (internal use) */
    UINT32 PreValid;                        /**< Validation of previous parking space data, 0 is not valid (internal use) */
    UINT16 UIDCurIdx;                       /**< Unique ID (internal use) */
    AMBA_PSD_PROC_OUTPUT_DATA_s TmpCur;     /**< Temporary parking space data for current (internal use) */
    AMBA_PSD_PROC_OUTPUT_DATA_s TmpOut;     /**< Temporary parking space data for output (internal use) */
} ParkingSpaceCornerHdlr_s;


UINT32 AmbaPSD_CornerSR_Init(const ParkingSpaceCornerCfg_s *Cfg, ParkingSpaceCornerHdlr_s *Hdlr);
UINT32 AmbaPSD_CornerSR_Process(ParkingSpaceCornerHdlr_s *Hdlr, AMBA_PSD_PROC_OUTPUT_DATA_s *CornerLists, AMBA_PSD_PROC_OUTPUT_DATA_s *OutputList);
UINT32 AmbaPSD_CornerSR_Reset(ParkingSpaceCornerHdlr_s *Hdlr);

#endif //AMBA_PSD_CORNERTRACE_H