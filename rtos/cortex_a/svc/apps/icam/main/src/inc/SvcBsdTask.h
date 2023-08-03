/**
 *  @file SvcBsdTask.h
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
 *  @details svc BSD task header
 *
 */

#ifndef SVC_BSD_TASK_H
#define SVC_BSD_TASK_H

#include "AmbaOD_2DBbx.h"
#include "AmbaSurround.h"

#define BSD_LEFT    (0U)
#define BSD_RIGHT   (1U)
#define BSD_BOTH    (2U)

#ifndef TRUE
#define TRUE  1U
#define FALSE 0U
#endif

/* for SR */
#define BSD_TIME_CLOCK 12288
#define CAR_WIDTH_SR (1810U) // mm
#define CAR_LENGTH_SR (4434U) // mm

/* for BSD */
#define BSD_OD_DELAY 82.0 //ms
#define BSD_FPS 12.0 //
#define BSD_SR_DELAY 10.0 //ms
#define BSD_OUTPUT_DELAY 10.0 //ms

#if 0//default
#define BSD_SPEED_STRIDE      5.0     // units:m/s
#define BSD_TTC_MIN           1500.0  // units:msec
#define BSD_TTC_STRIDE        500.0   // units:msec
#define BSD_LV1_THRESH_X      5000.0  // default level1 x thresh distance from tail of subject car to nearest surface of target car, units:mm
#define BSD_LV1_THRESH_Y      30000.0 // default level1 y thresh distance from tail of subject car to nearest surface of target car, units:mm
#define BSD_LV2_THRESH_X      5000.0  // default level2 x thresh distance from tail of subject car to nearest surface of target car, units:mm
#define BSD_LV2_THRESH_Y      3000.0  // default level2 y thresh distance from tail of subject car to nearest surface of target car, units:mm
#define BSD_OFF_THRESH_X      5000.0  // default level2 x thresh distance from center of subject car to nearest surface of target car, units:mm
#define BSD_OFF_THRESH_Y      1000.0  // default level2 y thresh distance from center of subject car to nearest surface of target car, units:mm
#define BSD_MIN_WORKING_SPEED 20.0    // units:km/hr
#else
#define BSD_LV1_THRESH_X      4000.0  // default level1 x thresh distance from tail of subject car to nearest surface of target car, units:mm
#define BSD_LV1_THRESH_Y      20000.0 // default level1 y thresh distance from tail of subject car to nearest surface of target car, units:mm
#define BSD_LV2_THRESH_X      4000.0  // default level2 x thresh distance from tail of subject car to nearest surface of target car, units:mm
#define BSD_LV2_THRESH_Y      7000.0  // default level2 y thresh distance from tail of subject car to nearest surface of target car, units:mm
#define BSD_OFF_THRESH_X      3000.0  // default level2 x thresh distance from center of subject car to nearest surface of target car, units:mm
#define BSD_OFF_THRESH_Y      1000.0  // default level2 y thresh distance from center of subject car to nearest surface of target car, units:mm
#define BSD_MIN_WORKING_SPEED 20.0    // units:km/hr
#define BSD_SUP_THRESH_X      900.0   // default suppression x thresh distance from center of subject car to nearest surface of target car, units:mm
#endif

#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
#if defined(CONFIG_ICAM_32BITS_OSD_USED)
#define LV1_CAT 200U
#define LV2_CAT 201U
#define LV1_CLS_CAT 202U
#define LV2_CLS_CAT 203U

#define LV1_COLOR 3U
#define LV2_COLOR 67U
#define LV1_CLS_COLOR 3U
#define LV2_CLS_COLOR 67U

#define Z1_COLOR 0xFFBFFF00U
#define Z2_COLOR 0x80ff0000U
#else
#define LV1_CAT 200U
#define LV2_CAT 201U
#define LV1_CLS_CAT 202U
#define LV2_CLS_CAT 203U

#define LV1_COLOR 3U
#define LV2_COLOR 67U
#define LV1_CLS_COLOR 3U
#define LV2_CLS_COLOR 67U

#define Z1_COLOR 28U
#define Z2_COLOR 249
#endif

typedef struct {
    UINT32                   Chan;
    UINT32                   FovIdx;
    AMBA_OD_2DBBX_LIST_s     *pBbx;
    const char               **Class_name;
    AMBA_SR_SROBJECT_DATA_s  *pSRData;
} SVC_BSD_DRAW_INFO_s;
#endif

UINT32 SvcBsdTask_Init(void);
UINT32 SvcBsdTask_Start(void);
UINT32 SvcBsdTask_Stop(void);
void   SvcBsdTask_OsdEnable(UINT32 Flag);
UINT32 SvcBsdTask_GetCalDataReady(void);
UINT32 SvcBsdTask_GetStatus(UINT8 *pEnable);
void   SvcBsdTask_SetBsdChan(UINT32 FovId, UINT32 Divisor);
UINT32 SvcBsdTask_GetBsdChan(void);
void   SvcBsdTask_SetBsdWithClipCanBus(UINT32 Value);
UINT32 SvcBSDTask_IsLiveView(void);
void   SvcBsdTask_DisplayLine(void);

#endif /* SVC_BSD_TASK_H */
