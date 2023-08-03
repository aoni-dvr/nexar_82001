/**
 * @file AmbaIQParamIMX577_CV22_ImageParam.c
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
 * @Description    :: Default Image IQ parameters
 *
*/

#include "AmbaTypes.h"
#include "AmbaImg_Adjustment.h"

IMG_PARAM_s AmbaIQParamIMX577ImageParam = {
    /* Version number*/
    0x19042200,

    /* cfa_leakage_filter_video */
    { 0, 0, 0, 0, 0, 0},
    /* cfa_leakage_filter_still */
    { 0, 0, 0, 0, 0, 16383},

    /* aaa_statistics_info */
    {
      64,         //awb_tile_num_col
      64,         //awb_tile_num_row
      0,          //awb_tile_col_start
      0,          //awb_tile_row_start
      64,         //awb_tile_width
      64,         //awb_tile_height
      64,         //awb_tile_active_width
      64,         //awb_tile_active_height
      0,          //awb_pix_min_value
      16383,      //awb_pix_max_value

      24,         //ae_tile_num_col
      16,         //ae_tile_num_row
      8,          //ae_tile_col_start
      0,          //ae_tile_row_start
      170,        //ae_tile_width
      256,        //ae_tile_height
      0,          //ae_pix_min_value
      16383,      //ae_pix_max_value

      24,         //af_tile_num_col
      16,         //af_tile_num_row
      8,          //af_tile_col_start
      0,          //af_tile_row_start
      170,        //af_tile_width
      256,        //af_tile_height
      170,        //af_tile_active_width
      256,        //af_tile_active_height
      0           //SixMinusCfaInputShift
    },

    /* ColorCorrVideoAddr */
    0,
    /* ColorCorrStillAddr */
    0,

    /* rgb_yuv_matrix_video_tv s2.0*/
    {
        {218,     732,     74,     //ITU 709-Full
        -117,    -395,    512,
         512,    -465,    -47},
         0,   128, 128
    },

    /* rgb_yuv_matrix_video_pc s2.0*/
    {
        {218,     732,     74,     //ITU 709-Full
        -117,    -395,    512,
         512,    -465,    -47},
         0,  128,  128
    },

    /* rgb_yuv_matrix_still s2.0*/
    {
          {306,     601,     117,     //ITU 601-Full
          -173,  -339,  512,
           512,  -429,  -83},
           0,   128,  128
    },

    /* chroma_scale_video */
    {
      1,
      {
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024
      }
    },

    /* chroma_scale_still */
    {
      1,
      {
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,
        1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024
      }
    },

    /* HIsoCfaLeakageFilter */
     { 0, 0, 0, 0, 0, 16383},
    /* HIsoLi2CfaLeakageFilter */
     { 0, 0, 0, 0, 0, 16383},

    /* HistogramMode */
     HISTOGRAM_CFA_Mode,
    /* HdrFrameNum */
     0,

    /* HdrExposNum */
     0,

    /* WbPositionCheck */
     48,    //  16:Sensor    32:Front End    48:Before CE    64:After CE

    /* ChkExposMode */
     0,    //  0 : none,  1 : Agc_Control,  2 : Shutter_Control,  3 : Agc_Shutter_Control, >= 128 : Fixed Ratio

    /* SensorHdrCompressInfo */
    {   0},

    /* CeInPutTable*/
    8.0,

    /*CeOutPutTable*/
    8.22,

};

