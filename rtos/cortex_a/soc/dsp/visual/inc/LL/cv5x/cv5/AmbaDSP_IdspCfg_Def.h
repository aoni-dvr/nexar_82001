/**
 *  @file AmbaDSP_IdspCfg_Def.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for the APIs of Idsp config
 *
 */

#ifndef AMBADSP_IDSPCFG_DEF_H
#define AMBADSP_IDSPCFG_DEF_H

#include "AmbaTypes.h"

//definition of Section Cfg/Index

/*
 * When PassStep = C2Y,  IdspCfgHdrAddr[0] = Sec2
 *                       IdspCfgHdrAddr[1] = Sec4
 *
 * When PassStep = Warp, IdspCfgHdrAddr[0] = Sec3, IdspCfgHdrAddr[1] = Sec5
 *                       IdspCfgHdrAddr[2] = Sec6, IdspCfgHdrAddr[3] = Sec7
 *                       IdspCfgHdrAddr[4] = Sec9, IdspCfgHdrAddr[5] = Sec11
 *                       IdspCfgHdrAddr[6] = Sec17,IdspCfgHdrAddr[7] = Sec18
 */
#define DSP_SEC2_CFG_INDEX      (0U)
//#define DSP_SEC2_CFG_SIZE       (74752U)    // 512*146
#define DSP_SEC4_CFG_INDEX      (1U)
//#define DSP_SEC4_CFG_SIZE       (12288U)    // 512*24
#define DSP_SEC3_CFG_INDEX      (0U)
//#define DSP_SEC3_CFG_SIZE       (25600U)    // 512*50
#define DSP_SEC5_CFG_INDEX      (1U)
//#define DSP_SEC5_CFG_SIZE       (1536U)     // 512*3
#define DSP_SEC6_CFG_INDEX      (2U)
//#define DSP_SEC6_CFG_SIZE       (1024U)     // 512*2
#define DSP_SEC7_CFG_INDEX      (3U)
//#define DSP_SEC7_CFG_SIZE       (1536U)     // 512*3
#define DSP_SEC9_CFG_INDEX      (4U)
//#define DSP_SEC9_CFG_SIZE       (1024U)     // 512*2
#define DSP_SEC11_CFG_INDEX     (5U)
#define DSP_SEC17_CFG_INDEX     (6U)
//#define DSP_SEC15_CFG_SIZE      (1024U)     // 512*2
#define DSP_SEC18_CFG_INDEX     (7U)
//#define DSP_MCTF_CFG_INDEX      (6U)
//#define DSP_MCTF_CFG_SIZE       (12240U)
//#define DSP_MCTS_CFG_INDEX      (7U)
//#define DSP_MCTS_CFG_SIZE       (13128U)

/* HISO */
#define DSP_HI_SEC2_CFG_INDEX   (0U)
#define DSP_HI_SEC4_CFG_INDEX   (1U)
#define DSP_HI_SEC5_CFG_INDEX   (2U)
#define DSP_HI_SEC6_CFG_INDEX   (3U)
#define DSP_HI_SEC7_CFG_INDEX   (4U)
#define DSP_HI_SEC9_CFG_INDEX   (5U)
#define DSP_HI_MCTF_CFG_INDEX   (6U)
#define DSP_HI_MCTS_CFG_INDEX   (7U)

#define DSP_SEC1_CFG_SIZE       (1536U)     //Vin0, 512*3
#define DSP_SEC10_CFG_SIZE      (1536U)     //Vin1
#define DSP_SEC11_CFG_SIZE      (1536U)     //Vin2
#define DSP_SEC12_CFG_SIZE      (1536U)     //Vin3
#define DSP_SEC13_CFG_SIZE      (1536U)     //Vin4
#define DSP_SEC14_CFG_SIZE      (1536U)     //Vin5

#define DSP_SEC1_MASK_INDEX     (0U)
#define DSP_SEC2_MASK_INDEX     (1U)
#define DSP_SEC3_MASK_INDEX     (2U)
#define DSP_SEC4_MASK_INDEX     (3U)
#define DSP_SEC5_MASK_INDEX     (4U)
#define DSP_SEC6_MASK_INDEX     (5U)
#define DSP_SEC7_MASK_INDEX     (6U)
#define DSP_SEC8_MASK_INDEX     (7U)
#define DSP_SEC9_MASK_INDEX     (8U)
#define DSP_SEC10_MASK_INDEX    (9U)
#define DSP_SEC11_MASK_INDEX    (10U)
#define DSP_SEC12_MASK_INDEX    (11U)
#define DSP_SEC13_MASK_INDEX    (12U)
#define DSP_SEC14_MASK_INDEX    (13U)
#define DSP_SEC15_MASK_INDEX    (14U)
#define DSP_SEC16_MASK_INDEX    (15U)

/* definition of PassStep, pass_step_id_t */
#define DSP_IMG_PASS_VIN            (0U)
#define DSP_IMG_PASS_C2Y            (1U)
#define DSP_IMG_PASS_WARP           (2U)
#define DSP_IMG_PASS_OSD_BLEND      (3U)
#define DSP_IMG_PASS_HI_BASE        (16U)
#define DSP_IMG_PASS_STEP_INVALID   (255U)

#define DSP_IMG_PASS_HISO_STEP_1    (DSP_IMG_PASS_HI_BASE)
#define DSP_IMG_PASS_HISO_STEP_2    (DSP_IMG_PASS_HI_BASE + 1U)
#define DSP_IMG_PASS_HISO_STEP_3    (DSP_IMG_PASS_HI_BASE + 2U)
#define DSP_IMG_PASS_HISO_STEP_4    (DSP_IMG_PASS_HI_BASE + 3U)
#define DSP_IMG_PASS_HISO_STEP_4A   (DSP_IMG_PASS_HI_BASE + 4U)
#define DSP_IMG_PASS_HISO_STEP_5    (DSP_IMG_PASS_HI_BASE + 5U)
#define DSP_IMG_PASS_HISO_STEP_6    (DSP_IMG_PASS_HI_BASE + 6U)
#define DSP_IMG_PASS_HISO_STEP_7    (DSP_IMG_PASS_HI_BASE + 7U)
#define DSP_IMG_PASS_HISO_STEP_8    (DSP_IMG_PASS_HI_BASE + 8U)
#define DSP_IMG_PASS_HISO_STEP_9    (DSP_IMG_PASS_HI_BASE + 9U)
#define DSP_IMG_PASS_HISO_STEP_10   (DSP_IMG_PASS_HI_BASE + 10U)
#define DSP_IMG_PASS_HISO_STEP_11   (DSP_IMG_PASS_HI_BASE + 11U)
#define DSP_IMG_PASS_HISO_STEP_12   (DSP_IMG_PASS_HI_BASE + 12U)
#define DSP_IMG_PASS_HISO_STEP_13   (DSP_IMG_PASS_HI_BASE + 13U)

#endif //AMBADSP_IDSPCFG_DEF_H
