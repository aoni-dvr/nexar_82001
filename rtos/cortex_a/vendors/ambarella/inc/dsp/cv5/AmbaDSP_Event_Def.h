/**
 *  @file AmbaDSP_Event_Def.h
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Header of DSP Driver Event Definition
 *
 */

#ifndef AMBA_DSP_EVENT_DEF_H
#define AMBA_DSP_EVENT_DEF_H

/* Event Index */
#define AMBA_DSP_EVENT_LV_RAW_RDY                   (0U)
#define AMBA_DSP_EVENT_LV_CFA_AAA_RDY               (1U)
#define AMBA_DSP_EVENT_LV_PG_AAA_RDY                (2U)
#define AMBA_DSP_EVENT_LV_HIST_AAA_RDY              (3U)
#define AMBA_DSP_EVENT_LV_YUV_DATA_RDY              (4U)
#define AMBA_DSP_EVENT_LV_PYRAMID_RDY               (5U)
#define AMBA_DSP_EVENT_VOUT_DATA_RDY                (6U)
#define AMBA_DSP_EVENT_VIDEO_DATA_RDY               (7U)
#define AMBA_DSP_EVENT_VIDEO_ENC_START              (8U)
#define AMBA_DSP_EVENT_VIDEO_ENC_STOP               (9U)
#define AMBA_DSP_EVENT_STL_RAW_RDY                  (10U)
#define AMBA_DSP_EVENT_STL_CFA_AAA_RDY              (11U)
#define AMBA_DSP_EVENT_STL_PG_AAA_RDY               (12U)
#define AMBA_DSP_EVENT_STL_RAW_POSTPROC_RDY         (13U)
#define AMBA_DSP_EVENT_STL_YUV_DATA_RDY             (14U)
#define AMBA_DSP_EVENT_JPEG_DATA_RDY                (15U)
#define AMBA_DSP_EVENT_VIDEO_PATH_STATUS            (16U)
#define AMBA_DSP_EVENT_VIDEO_DEC_STATUS             (17U)
#define AMBA_DSP_EVENT_VIDEO_DEC_YUV_DISP_REPORT    (18U)
#define AMBA_DSP_EVENT_VIDEO_DEC_PIC_INFO           (19U)
#define AMBA_DSP_EVENT_STILL_DEC_STATUS             (20U)
#define AMBA_DSP_EVENT_STILL_DEC_YUV_DISP_REPORT    (21U)
#define AMBA_DSP_EVENT_VIN_POST_CONFIG              (22U)
#define AMBA_DSP_EVENT_ERROR                        (23U)
#define AMBA_DSP_EVENT_LV_SYNC_JOB_RDY              (24U)
#define AMBA_DSP_EVENT_VIDEO_MV_DATA_RDY            (25U)
#define AMBA_DSP_EVENT_LV_SIDEBAND_RDY              (26U)   /* Used when VpMsg be activated */
#define AMBA_DSP_EVENT_LV_LNDT_RDY                  (27U)
#define AMBA_DSP_EVENT_LV_MAIN_Y12_RDY              (28U)
#define AMBA_DSP_EVENT_LV_MAIN_IR_RDY               (29U)
#define AMBA_DSP_EVENT_CLK_UPDATE_READY             (30U)   /**< Report clock update ready */
#define AMBA_DSP_EVENT_NUM                          (31U)

#endif  /* AMBA_DSP_EVENT_DEF_H */
