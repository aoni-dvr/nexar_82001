/**
 * @file AmbaIQParamIMX224_CV22_AdjTableParam.c
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
 * @Description    :: Initialization Adjustment Parameters for Noise Control and Color Tuning
 *
*/

#include "AmbaTypes.h"
#include "AmbaImg_AaaDef.h"

ADJ_TABLE_PARAM_s AmbaIQParamIMX224AdjTableParam = {
        0x14010300,        // struct_version_number
        0x15090300,        // param_version_number

        {//video
        1,    //max_table_count
        2,    //max_table_same
        //table 0
        {{ADJ_R3840x2160P030V1C1X01, ADJ_R_P_V_C_X__ },
        //table 1
        { ADJ_R_P_V_C_X__ },
        //table 2
        { ADJ_R2560X1080P030V1C1X01, ADJ_R2304X1296P030V1C1X01 },
        //table 3
        { ADJ_SENSOR_X3PC, ADJ_SENSOR_X8PC }}},


        {//photo_preview
        1,    //max_table_count
        2,    //max_table_same
        //table 0
        {{ADJ_SENSOR_X1PC },
        //table 1
        { ADJ_SENSOR_X2PC },
        //table 2
        { ADJ_SENSOR_X4PC },
        //table 3
        { ADJ_SENSOR_X3PC, ADJ_SENSOR_X8PC }}},
};

