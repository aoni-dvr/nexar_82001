/*
 * Copyright (c) 2017-2018 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CVAPI_COMMON_H_FILE
#define CVAPI_COMMON_H_FILE

#ifndef AMBINT_H
#include "ambint.h"
#endif

typedef uint32_t relative_ptr_t;

/* IDSP Pyramid maximum half octaves */
#define MAX_HALF_OCTAVES    13U

/* HLOD configurations: used by HLOD and FMA TT */
#define NUM_PASS3_OBJ_CATEGORIES    (2)
#define MAX_OBJ_PER_CAT             (256)
#define MAX_TOTAL_DET               (MAX_OBJ_PER_CAT * NUM_PASS3_OBJ_CATEGORIES)
#define MAX_FEATURES_PER_BLOCK      (32U)
#define MAX_BRIEF_DESC_SIZE         (32U)
#define MAX_HIST_SIZE               (1600U)
#define AC_CVAPI_MAX_CAMERA_IDS     (32U)


typedef struct { /* coord_s */
    uint16_t x;
    uint16_t y;
} coord_t;

/* *
 *  roi_t
 *  @brief
 *  Define the ROI region.
 */
typedef struct {
    uint16_t    m_start_col;
    uint16_t    m_start_row;
    uint16_t    m_width_m1;
    uint16_t    m_height_m1;

} roi_t;

#endif /* CVAPI_COMMON_H_FILE */

