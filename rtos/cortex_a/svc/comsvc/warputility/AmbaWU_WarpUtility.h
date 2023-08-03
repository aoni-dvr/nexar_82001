/*
*  @file AmbaWU_WarpUtility.h
*
* Copyright (c) 2020 Ambarella International LP
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
*/

/* Coverit 4.10 hopes use this to avoid repeatedly include header*/
#ifndef AmbaWU_WarpUtility
#define AmbaWU_WarpUtility

#include "AmbaDSP_ImageFilter.h"

#define WU_OK             (0U)
#define WU_ERROR_GENERAL  (WU_ERR_BASE)

typedef struct {
    AMBA_IK_WARP_INFO_s InputAInfo;
    AMBA_IK_WARP_INFO_s InputBInfo;
} AMBA_WU_WARP_ADD_IN_s;

typedef struct {
    AMBA_IK_WARP_INFO_s ResultInfo;
} AMBA_WU_WARP_ADD_RESULT_s;

typedef struct {
    UINT32 CropX;
    UINT32 CropY;
    AMBA_IK_WARP_INFO_s InputInfo;
    AMBA_IK_WINDOW_GEOMETRY_s FovInfo;
} AMBA_WU_WARP_RESIZE_IN_s;

typedef struct {
    AMBA_IK_WARP_INFO_s ResultInfo;
} AMBA_WU_WARP_RESIZE_RESULT_s;

typedef struct {
    DOUBLE Theta;
    UINT32 CenterX;
    UINT32 CenterY;
    AMBA_IK_WARP_INFO_s InputInfo;
} AMBA_WU_WARP_ROTATE_IN_s;

typedef struct {
    AMBA_IK_WARP_INFO_s ResultInfo;
} AMBA_WU_WARP_ROTATE_RESULT_s;

/**
* Add two input warp tables A and B, and output one warp table.
* @param [in]  pWarpAddIn : input warp table and info of A and B.
* @param [in,out]  pWarpAddResult : output warp table and info of result.
* @return OK - success, NG - fail
*/
UINT32 AmbaWU_WarpAdd(const AMBA_WU_WARP_ADD_IN_s *pWarpAddIn, AMBA_WU_WARP_ADD_RESULT_s *pWarpAddResult);

/**
* Resize input warp table, and output one warp table.
* @param [in]  pWarpResizeIn : input warp table and info.
* @param [in,out]  pWarpResizeResult : output warp table and info of result.
* @return OK - success, NG - fail
*/
UINT32 AmbaWU_WarpResize(const AMBA_WU_WARP_RESIZE_IN_s *pWarpResizeIn, const AMBA_WU_WARP_RESIZE_RESULT_s *pWarpResizeResult);

/**
* Rotate input warp table, and output one warp table.
* @param [in]  pWarpRotateIn : input warp table and info.
* @param [in,out]  pWarpRotateResult : output warp table and info of result.
* @return OK - success, NG - fail
*/
UINT32 AmbaWU_WarpRotate(const AMBA_WU_WARP_ROTATE_IN_s *pWarpRotateIn, const AMBA_WU_WARP_ROTATE_RESULT_s *pWarpRotateResult);

#endif