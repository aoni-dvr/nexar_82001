/**
 *  @file SvcCalibModuleCfg.c
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
 *  @details svc calibration module configuration
 *
 */

#include "AmbaTypes.h"
#include "AmbaEEPROM.h"
#include "AmbaDSP_ImageFilter.h"
#include "SvcCalibMgr.h"
#include "SvcCalibModMgr.h"
#include "SvcCalibModuleCfg.h"

/**
 * storage module configuration getting function
 * @return pointer of storage module configuration
 */
SVC_MODULE_CFG_s* SvcModuleCfg_Get(void)
{
    static SVC_MODULE_CFG_s      g_SvcModuleCfg;
    static SVC_MODULE_ITEM_CFG_s g_SvcModuleItemCfg[] = {
        { SVC_MODULE_ITEM_BPC, SvcModule_BpcCmdFunc },
    };
    static SVC_MODULE_ITEM_UPD_s g_SvcModuleUpdItem[] = {
        { 1U, SVC_MODULE_ITEM_VIG,           SVC_CALIB_VIGNETTE_ID, SVC_MODULE_ITEM_VIG_SIZE,
            // Configure Vignette Update Rule
            {
                { 1U, 0x1U, 0x1U, 0U }, /* Storage module Vin_0 SerDes_0 update to TableIdx_0 */
                { 1U, 0x1U, 0x2U, 1U }, /* Storage module Vin_0 SerDes_1 update to TableIdx_1 */
                { 1U, 0x1U, 0x4U, 2U }, /* Storage module Vin_0 SerDes_2 update to TableIdx_2 */
                { 1U, 0x1U, 0x8U, 3U }, /* Storage module Vin_0 SerDes_3 update to TableIdx_3 */
                { 1U, 0x2U, 0x1U, 0U }, /* Storage module Vin_1 SerDes_0 update to TableIdx_0 */
                { 1U, 0x2U, 0x2U, 1U }, /* Storage module Vin_1 SerDes_1 update to TableIdx_1 */
                { 1U, 0x2U, 0x4U, 2U }, /* Storage module Vin_1 SerDes_2 update to TableIdx_2 */
                { 1U, 0x2U, 0x8U, 3U }, /* Storage module Vin_1 SerDes_3 update to TableIdx_3 */
                { 1U, 0x4U, 0x1U, 0U }, /* Storage module Vin_3 SerDes_0 update to TableIdx_0 */
                { 1U, 0x4U, 0x2U, 1U }, /* Storage module Vin_3 SerDes_1 update to TableIdx_1 */
                { 1U, 0x4U, 0x4U, 2U }, /* Storage module Vin_3 SerDes_2 update to TableIdx_2 */
                { 1U, 0x4U, 0x8U, 3U }, /* Storage module Vin_3 SerDes_3 update to TableIdx_3 */
                { 1U, 0x8U, 0x1U, 0U }, /* Storage module Vin_4 SerDes_0 update to TableIdx_0 */
                { 1U, 0x8U, 0x2U, 1U }, /* Storage module Vin_4 SerDes_1 update to TableIdx_1 */
                { 1U, 0x8U, 0x4U, 2U }, /* Storage module Vin_4 SerDes_2 update to TableIdx_2 */
                { 1U, 0x8U, 0x8U, 3U }, /* Storage module Vin_4 SerDes_3 update to TableIdx_3 */
            },
        },
        { 1U, SVC_MODULE_ITEM_BPC,           SVC_CALIB_BPC_ID,      SVC_MODULE_ITEM_BPC_SIZE,
            // Configure Bad pixel Update Rule
            {
                { 1U, 0x1U, 0x1U, 0U }, /* Storage module Vin_0 SerDes_0 update to TableIdx_0 */
                { 1U, 0x1U, 0x2U, 1U }, /* Storage module Vin_0 SerDes_1 update to TableIdx_1 */
                { 1U, 0x1U, 0x4U, 2U }, /* Storage module Vin_0 SerDes_2 update to TableIdx_2 */
                { 1U, 0x1U, 0x8U, 3U }, /* Storage module Vin_0 SerDes_3 update to TableIdx_3 */
                { 1U, 0x2U, 0x1U, 0U }, /* Storage module Vin_1 SerDes_0 update to TableIdx_0 */
                { 1U, 0x2U, 0x2U, 1U }, /* Storage module Vin_1 SerDes_1 update to TableIdx_1 */
                { 1U, 0x2U, 0x4U, 2U }, /* Storage module Vin_1 SerDes_2 update to TableIdx_2 */
                { 1U, 0x2U, 0x8U, 3U }, /* Storage module Vin_1 SerDes_3 update to TableIdx_3 */
                { 1U, 0x4U, 0x1U, 0U }, /* Storage module Vin_3 SerDes_0 update to TableIdx_0 */
                { 1U, 0x4U, 0x2U, 1U }, /* Storage module Vin_3 SerDes_1 update to TableIdx_1 */
                { 1U, 0x4U, 0x4U, 2U }, /* Storage module Vin_3 SerDes_2 update to TableIdx_2 */
                { 1U, 0x4U, 0x8U, 3U }, /* Storage module Vin_3 SerDes_3 update to TableIdx_3 */
                { 1U, 0x8U, 0x1U, 0U }, /* Storage module Vin_4 SerDes_0 update to TableIdx_0 */
                { 1U, 0x8U, 0x2U, 1U }, /* Storage module Vin_4 SerDes_1 update to TableIdx_1 */
                { 1U, 0x8U, 0x4U, 2U }, /* Storage module Vin_4 SerDes_2 update to TableIdx_2 */
                { 1U, 0x8U, 0x8U, 3U }, /* Storage module Vin_4 SerDes_3 update to TableIdx_3 */
            },
        },
        { 1U, SVC_MODULE_ITEM_WHITE_BALANCE, SVC_CALIB_WB_ID, 0U,
            // Configure white balance Update Rule
            {
                { 1U, 0x1U, 0x1U, 0U }, /* Storage module Vin_0 SerDes_0 update to TableIdx_0 */
                { 1U, 0x1U, 0x2U, 1U }, /* Storage module Vin_0 SerDes_1 update to TableIdx_1 */
                { 1U, 0x1U, 0x4U, 2U }, /* Storage module Vin_0 SerDes_2 update to TableIdx_2 */
                { 1U, 0x1U, 0x8U, 3U }, /* Storage module Vin_0 SerDes_3 update to TableIdx_3 */
                { 1U, 0x2U, 0x1U, 0U }, /* Storage module Vin_1 SerDes_0 update to TableIdx_0 */
                { 1U, 0x2U, 0x2U, 1U }, /* Storage module Vin_1 SerDes_1 update to TableIdx_1 */
                { 1U, 0x2U, 0x4U, 2U }, /* Storage module Vin_1 SerDes_2 update to TableIdx_2 */
                { 1U, 0x2U, 0x8U, 3U }, /* Storage module Vin_1 SerDes_3 update to TableIdx_3 */
                { 1U, 0x4U, 0x1U, 0U }, /* Storage module Vin_3 SerDes_0 update to TableIdx_0 */
                { 1U, 0x4U, 0x2U, 1U }, /* Storage module Vin_3 SerDes_1 update to TableIdx_1 */
                { 1U, 0x4U, 0x4U, 2U }, /* Storage module Vin_3 SerDes_2 update to TableIdx_2 */
                { 1U, 0x4U, 0x8U, 3U }, /* Storage module Vin_3 SerDes_3 update to TableIdx_3 */
                { 1U, 0x8U, 0x1U, 0U }, /* Storage module Vin_4 SerDes_0 update to TableIdx_0 */
                { 1U, 0x8U, 0x2U, 1U }, /* Storage module Vin_4 SerDes_1 update to TableIdx_1 */
                { 1U, 0x8U, 0x4U, 2U }, /* Storage module Vin_4 SerDes_2 update to TableIdx_2 */
                { 1U, 0x8U, 0x8U, 3U }, /* Storage module Vin_4 SerDes_3 update to TableIdx_3 */
            },
        },
    };

    g_SvcModuleCfg.ItemCfgNum = (UINT32)(sizeof(g_SvcModuleItemCfg)) / ((UINT32)(sizeof(g_SvcModuleItemCfg[0])));
    g_SvcModuleCfg.pItemCfg   = g_SvcModuleItemCfg;
    g_SvcModuleCfg.ItemUpdNum = (UINT32)(sizeof(g_SvcModuleUpdItem)) / ((UINT32)(sizeof(g_SvcModuleUpdItem[0])));
    g_SvcModuleCfg.pItemUpd   = g_SvcModuleUpdItem;

    return &g_SvcModuleCfg;
}
