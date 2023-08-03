/*
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

#include "AmbaTypes.h"
#include "AmbaDSP_ImgDebugUtility.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_ImageFilterInternal.h"
#include "AmbaDSP_ImgDebugPrint.h"
#include "AmbaDSP_ImgArchSystemAPI.h"

//UINT32  AmbaIK_SetDiagCaseId(const AMBA_IK_MODE_CFG_s *pMode, UINT8 DiagCaseId);
UINT32 AmbaIK_SetDebugLevel(UINT32 DebugLevel);
UINT32 AmbaIK_GetWarpDebugLevel(UINT32 *WarpDebugLevel);
UINT32 AmbaIK_PrintIkWorkingBufferInfo(const void* p_buffer);
UINT32 AmbaIK_SetDebugLogId(amba_ik_debug_id_e debug_id, uint8 enable);
UINT32 AmbaIK_SetMotionDetect(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_MOTION_DETECT_s *pMotionDetect);
UINT32 AmbaIK_GetMotionDetect(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_MOTION_DETECT_s *pMotionDetect);
UINT32 AmbaIK_SetMotionDetectPosDep(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_POS_DSP33_t *pMotionDetectPosDep);
UINT32 AmbaIK_GetMotionDetectPosDep(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_POS_DSP33_t *pMotionDetectPosDep);
UINT32 AmbaIK_SetMotionDetectAndMctf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_MOTION_DETECT_AND_MCTF_s *pMotionDetectAndMctf);
UINT32 AmbaIK_GetMotionDetectAndMctf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_MOTION_DETECT_AND_MCTF_s *pMotionDetectAndMctf);
UINT32 AmbaIK_SetMctfIntl(uint32 context_id, const ik_mctf_internal_t *p_mctf_internal);
UINT32 AmbaIK_GetMctfIntl(uint32 context_id, ik_mctf_internal_t *p_mctf_internal);
UINT32 AmbaIK_SetStaticBadPxlCorrIntl(uint32 context_id, const ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal);
UINT32 AmbaIK_GetStaticBadPxlCorrIntl(uint32 context_id, ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal);
UINT32 AmbaIK_SetY2yUseCcEnableInfo(uint32 context_id, const uint32 enable);
UINT32 AmbaIK_GetY2yUseCcEnableInfo(uint32 context_id, uint32 *enable);

//UINT32 AmbaIK_SetWarpDebugLevel(UINT32 WarpDebugLevel);

UINT32  AmbaIK_SetDiagCaseId(const AMBA_IK_MODE_CFG_s *pMode, const UINT8 DiagCaseId)
{
    return ik_set_diag_case_id(pMode->ContextId, DiagCaseId);
}

UINT32 AmbaIK_SetDebugLevel(UINT32 DebugLevel)
{
    return ik_set_debug_level(DebugLevel);
}

UINT32 AmbaIK_SetWarpDebugLevel(UINT32 WarpDebugLevel)
{
    return ik_set_warp_debug_level(WarpDebugLevel);
}

// Following are added for misraC 8.7
UINT32 AmbaIK_GetWarpDebugLevel(UINT32 *WarpDebugLevel)
{
    return ik_get_warp_debug_level(WarpDebugLevel);
}

// Following are added for misraC 8.7
UINT32 AmbaIK_PrintIkWorkingBufferInfo(const void* p_buffer)
{
    return ik_print_ik_working_buffer_info(p_buffer);
}

// Following are added for misraC 8.7
UINT32 AmbaIK_SetDebugLogId(amba_ik_debug_id_e debug_id, uint8 enable)
{
    return ik_set_debug_log_id((uint32)debug_id, enable);
}

// Following are added for misraC 8.7
UINT32 AmbaIK_SetMotionDetect(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_MOTION_DETECT_s *pMotionDetect)
{
    const ik_motion_detect_t *p_ik_motion_detect;

    (void)amba_ik_system_memcpy(&p_ik_motion_detect, &pMotionDetect, sizeof(void *));
    return ik_set_motion_detect((UINT32)pMode->ContextId, p_ik_motion_detect);
}

// Following are added for misraC 8.7
UINT32 AmbaIK_GetMotionDetect(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_MOTION_DETECT_s *pMotionDetect)
{
    ik_motion_detect_t *p_ik_motion_detect;

    (void)amba_ik_system_memcpy(pMotionDetect, pMotionDetect, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_motion_detect, &pMotionDetect, sizeof(void *));
    return ik_get_motion_detect((UINT32)pMode->ContextId, p_ik_motion_detect);
}

// Following are added for misraC 8.7
UINT32 AmbaIK_SetMotionDetectPosDep(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_POS_DSP33_t *pMotionDetectPosDep)
{
    const ik_pos_dep33_t *p_pos_dep;

    (void)amba_ik_system_memcpy(&p_pos_dep, &pMotionDetectPosDep, sizeof(void *));
    return ik_set_motion_detect_pos_dep((UINT32)pMode->ContextId, p_pos_dep);
}

// Following are added for misraC 8.7
UINT32 AmbaIK_GetMotionDetectPosDep(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_POS_DSP33_t *pMotionDetectPosDep)
{
    ik_pos_dep33_t *p_pos_dep;

    (void)amba_ik_system_memcpy(pMotionDetectPosDep, pMotionDetectPosDep, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_pos_dep, &pMotionDetectPosDep, sizeof(void *));
    return ik_get_motion_detect_pos_dep((UINT32)pMode->ContextId, p_pos_dep);
}


// Following are added for misraC 8.7
UINT32 AmbaIK_SetMotionDetectAndMctf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_MOTION_DETECT_AND_MCTF_s *pMotionDetectAndMctf)
{
    const ik_motion_detect_and_mctf_t *p_ik_motion_detect_and_mctf;

    (void)amba_ik_system_memcpy(&p_ik_motion_detect_and_mctf, &pMotionDetectAndMctf, sizeof(void *));
    return ik_set_motion_detect_and_mctf((UINT32)pMode->ContextId, p_ik_motion_detect_and_mctf);
}

// Following are added for misraC 8.7
UINT32 AmbaIK_GetMotionDetectAndMctf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_MOTION_DETECT_AND_MCTF_s *pMotionDetectAndMctf)
{
    ik_motion_detect_and_mctf_t *p_ik_motion_detect_and_mctf;

    (void)amba_ik_system_memcpy(pMotionDetectAndMctf, pMotionDetectAndMctf, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_motion_detect_and_mctf, &pMotionDetectAndMctf, sizeof(void *));
    return ik_get_motion_detect_and_mctf((UINT32)pMode->ContextId, p_ik_motion_detect_and_mctf);
}

// Following are added for misraC 8.7
UINT32 AmbaIK_SetMctfIntl(uint32 context_id, const ik_mctf_internal_t *p_mctf_internal)
{
    return amba_ik_set_mctf_internal(context_id, p_mctf_internal);
}

// Following are added for misraC 8.7
UINT32 AmbaIK_GetMctfIntl(uint32 context_id, ik_mctf_internal_t *p_mctf_internal)
{
    return amba_ik_get_mctf_internal(context_id, p_mctf_internal);
}

// Following are added for misraC 8.7
UINT32 AmbaIK_SetStaticBadPxlCorrIntl(uint32 context_id, const ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal)
{
    return ik_set_static_bad_pxl_corr_itnl(context_id, p_static_bpc_internal);
}

// Following are added for misraC 8.7
UINT32 AmbaIK_GetStaticBadPxlCorrIntl(uint32 context_id, ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal)
{
    return ik_get_static_bad_pxl_corr_itnl(context_id, p_static_bpc_internal);
}

// Following are added for misraC 8.7
UINT32 AmbaIK_SetY2yUseCcEnableInfo(uint32 context_id, const uint32 enable)
{
    return ik_set_y2y_use_cc_enable_info(context_id, enable);
}

// Following are added for misraC 8.7
UINT32 AmbaIK_GetY2yUseCcEnableInfo(uint32 context_id, uint32 *enable)
{
    return ik_get_y2y_use_cc_enable_info(context_id, enable);
}

