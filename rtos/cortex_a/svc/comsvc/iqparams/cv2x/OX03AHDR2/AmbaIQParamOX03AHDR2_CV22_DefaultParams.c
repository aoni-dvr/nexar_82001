/**
 * @file AmbaIQParamOX03AHDR2_CV22_DefaultParams.c
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
 * @Description    :: Default AAA parameters
 *
*/
#include "AmbaTypes.h"
#include "AmbaImg_AaaDef.h"

AAA_PARAM_s AmbaIQParamOV2775HDR2DefParams = {

        0x17110900,        // struct_version_number
        0x19090500,        // param_version_number

/* AAA algorithm enable */
{
    1,        // ae_op
    1,        // awb_op
    0,        // af_op
    1,        // adj op
    0,
    0,
    0,
    0
},

/* AE control parameter settings */
{     0,                   //def_ae_target;
      DISABLE,             //reserved,
      DISABLE,             //slow_shutter;
      0,0,                 //reserved,
      1,                   //photo_slow_shutter;
      0,                   //ReplaceAGCgainToDigitalGain,
      1,                   //video_ae_speed : 0~6
      0,                   //photo_ae_speed : 0~6
      AE_METER_CENTER,     //metering_mode;
      0,                   //backlight;
      0,                   //ev_bias
      FLASH_ALWAYS_OFF,    //video_flash;
      DISABLE,             //reserved,

      FLASH_ALWAYS_OFF,    //still_flash;
      AE_FLASH_NORMAL,     //still_flash_type
      4,                   //still_flash_flicker_chk : 0~4    0:no check   1: 1/120~1/60   2: 1/120~1/30   3: 1/120~1/15   4: 1/120~1/7.5
      DISABLE,             //still_night_shot
      DISABLE,             //still_is
      4,                   //still_flicker_chk : 0~4    0:no check   1: 1/120~1/60   2: 1/120~1/30   3: 1/120~1/15   4: 1/120~1/7.5
      AE_IRIS_AUTO,        //still_iris;
      AE_ISO_AUTO,         //still_iso;
      AE_SHUTTER_AUTO,     //still_shutter;
      AE_P_MODE_OFF,       //still_p_mode;
      0,                   //still_p_mode_str    1EV : 128
      DISABLE,             //still_normal_post_proc
      DISABLE,             //still_flash_post_proc
      DISABLE,             //still_continous_post

      DISABLE,             //time_lapse_enable;
      60,                  //time_lapse_skip_frames;

      ENABLE,              //dual_main_video_enable;
      ENABLE,              //dual_second_video_enable;
      120,                 //dual_second_video_ae_target;

      0,                   //LongExposureLimitBias;
      3,                   //dual_second_video_ae_speed;
      AE_METER_CENTER,     //dual_second_video_metering_mode;

      32                   //still_continue_speed;       //0~128
},

/* AE EV Table parameter settings */
{    ANTI_FLICKER_60HZ,    //flicker_mode;
     2047,                 //video_shutter_max_idx;

    {    //min_agc_db
              4,

        //max_agc_db
        //   <1/120  <=1/100  <=1/50   <=1/25   <=1/12.5  <=1/6.2   >1/6.2    // Main_Shutter
        {{      30,      30,      30,      30,      30,      30,      30},    //  <1/120
         {      18,      30,      30,      30,      30,      30,      30},    //  <=1/100
         {      12,      18,      30,      30,      30,      30,      30},    //  <=1/50
         {      10,      10,      10,      30,      30,      30,      30},    //  <=1/25
         {       6,       6,      12,      18,      30,      30,      30},    //  <=1/12.5
         {       6,       6,       6,      12,      18,      30,      30},    //  <=1/6.2
         {       6,       6,       6,       6,      12,      18,      30},    //  >1/6.2
        //     PIC   <=1/100  <=1/50   <=1/25   <=1/12.5  <=1/6.2   >1/6.2
         {       6,      12,      18,      30,      30,      30,      30}},   // PHOTO_PREVIEW

        //max_dgain_db
        //   <1/120  <=1/100  <=1/50   <=1/25   <=1/12.5  <=1/6.2   >1/6.2    // Main_Shutter
        {{       3,       3,       3,       3,       3,       3,       3},    //  <1/120
         {       0,       3,       3,       3,       3,       3,       3},    //  <=1/100
         {       0,       0,       3,       3,       3,       3,       3},    //  <=1/50
         {       0,       0,       0,       3,       3,       3,       3},    //  <=1/25
         {       0,       0,       0,       0,       3,       3,       3},    //  <=1/12.5
         {       0,       0,       0,       0,       0,       3,       3},    //  <=1/6.2
         {       0,       0,       0,       0,       0,       0,       3},    //  >1/6.2

        //     PIC   <=1/100  <=1/50   <=1/25   <=1/12.5  <=1/6.2   >1/6.2
         {       1,       1,       1,       1,       1,       1,       1}}},  // PHOTO_PREVIEW

         AA_TOTAL_01VIN_02AE,        //vin_total;
         AA_NUM01VIN_NUM01AE,        //NumVinAe;
         AA_TILE_01_FRAME            //tiles_side_num;
},

/* AE algorithm parameter settings */
{
    {2560+64,    //still_iso_6
     2432+64,    //still_iso_12
     2304+64,    //still_iso_25
     2176+64,    //still_iso_50
     2048+64,    //still_iso_100
     1920+64,    //still_iso_200
     1792+64,    //still_iso_400
     1664+64,    //still_iso_800
     1536+64,    //still_iso_1600
     1408+64,    //still_iso_3200
     1280+64,    //still_iso_6400
     1152+64,    //still_iso_12800
     1024+64,0,0,0},   //still_iso_25600

    {2560+64,    //still_iso_6
     2432+64,    //still_iso_12
     2304+64,    //still_iso_25
     2176+64,    //still_iso_50
     2048+64,    //still_iso_100
     1920+64,    //still_iso_200
     1792+64,    //still_iso_400
     1664+64,    //still_iso_800
     1536+64,    //still_iso_1600
     1408+64,    //still_iso_3200
     1280+64,    //still_iso_6400
     1152+64,    //still_iso_12800
     1024+64,0,0,0},   //still_iso_25600


    { 4400,                         //global_dgain;
         0,       //video_dark_luma
         0,       //photo_preview_dark_luma
         0,       //still_dark_luma

       884,       //still_shutter_min_normal          1/2 sec
       256,       //still_shutter_min_night             2 sec
      1012,       //still_shutter_min_flash          1/30 sec
       756,       //still_shutter_min_flash_slow    1/7.5 sec
      1012,       //still_shutter_min_continue       1/30 sec
      1140,       //still_shutter_min_burst          1/60 sec
      1920,       //still_shutter_max_normal       1/4096 sec;
      1920,       //still_shutter_max_flash        1/4096 sec;

    {   0,       //video_enable_ae_face_detection;
      154,       //video_face_detection_exposure_target_gain to nirmal ae target, unit:128
       16,       //video_face_luma_priority 0 ~ 16
       15,       //video_get_face_delay_fps relate to 30fps
       30},      //video_after_get_face_delay_fps  relate to 30fps
    {   0,       //still_enable_ae_face_detection;
      160,       //still_face_detection_exposure_target_gain to nirmal ae target, unit:128
       16,       //still_face_luma_priority 0 ~ 16
       15,       //still_get_face_delay_fps relate to 30fps
       30},      //still_after_get_face_delay_fps  relate to 30fps


       17,       //high_light_lv_no;
       17,       //outdoor_lv_no;
        6,       //low_light_lv_no;

    {//flash_focus_distance_target_ratio    unit:128
      0,          //start;
      500,        //end;
    //  ratio offset adapt    ratio offset adapt
    {{{  128,   250,   100},{  256,   150,   200},}}},
    },

       96,    //count
    {{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  //AE_METER_CENTER
       1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1,
       1, 1, 1, 1, 2, 3, 3, 2, 1, 1, 1, 1,
       1, 1, 1, 2, 3, 5, 5, 3, 2, 1, 1, 1,
       1, 1, 1, 2, 3, 5, 5, 3, 2, 1, 1, 1,
       1, 1, 2, 3, 4, 5, 5, 4, 3, 2, 1, 1,
       1, 2, 3, 4, 4, 4, 4, 4, 4, 3, 2, 1,
       2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2},

     { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    //AE_METER_AVERAGE
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    //AE_METER_SPOT
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 1, 3, 3, 1, 0, 0, 0, 0,
       0, 0, 0, 0, 1, 3, 3, 1, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

     { 0}    //LumaInfo
},

/* AWB control parameter settings */
{
    2,                         // method : 1: gray world, 2:white_patch,
    128,                       // speed
    6,                        // 30fps video_skip_frames;
    4,                         // 30fps photo_skip_frames;
    WB_AUTOMATIC,              // awb_menu_mode
    WB_MENU_REGION,            // awb_menu_mode_type
    AWB_METER_AVERAGE,         // metering_mode;
    AWB_NO_WHITE_LAST_GAIN,    // video_no_white;
    AWB_NO_WHITE_LAST_GAIN,    // still_no_white;

    96,                        // still_continue_speed;       //0~128

    //vin_0
    {AWB_TILE_ALL}             // tile_side_chk;
},

/* AWB algorithm parameter settings */
{
    {14,//  white_region_lut
     //gr_min ,gr_max,gb_min, gb_max,                          //
     //                               y_a_min,a_min_slope,y_a_max,a_max_slope, //
     //                                                                  y_b_min,b_min_slope,y_b_max,b_max_slope, weighting //

    {{  3214,    8000,    9076,    17000,    -6139,    16471,    -10328,     30000,     5374,  -575,    3255,  14000,    1},
     {    4908,    8785,    7243,    12500,    -6338,    16634,     -5675,     22000,     4216, -1341,    7053,    2718,    2},
     {    5136,  10251,    5672,    11000,    -7315,    17996,     -4950,     20500,     3673, -3007,    4053,    4257,    4},
     {    6048,  11163,    4737,    10000,    -9091,    20886,     -2594,     18000,     4408, -6614,    5557,     114,    8},
     {    6504,  11652,    4214,    9000,    -6583,    16928,     -3135,     16000,     2852, -3357,    5272,    -659,    4},        // 4    CLOUDY
     {     0,      0,      0,      0,       0,      0,       0,      0,     0,     0,      0,      0,     0},    // 5    D9000
     {     0,      0,      0,      0,       0,      0,       0,      0,     0,     0,      0,      0,     0},    // 6    D10000, Sky
     {     0,      0,      0,      0,       0,      0,       0,      0,     0,     0,      0,      0,     0},    // 7    FLASH
     {  9143,    12499,    5523,    8366,    -3291,    13242,    -2687,    15583,    3291,    -2753,    4095,    -1885,    1},    // 8    FLUORESCENT
     {  9246,   12510,  5340,   7983,   -2811,  11964,  -6693,  27565,  4342,  -6203,   7891,  -11386,  1},    // 9    FLUORESCENT_2
     {  7700,   9000,   8800,  11200,   -4500,  17500,   -4500,  20000,  3000,  3000,   3300,    5000,  0},    // 10    FLUORESCENT_3
     {     0,      0,      0,      0,       0,      0,       0,      0,     0,     0,      0,      0, 0  },    // 11    FLUORESCENT_4
     {     0,      0,      0,      0,       0,      0,       0,      0,     0,     0,      0,      0, 0  },    // 12    WATER_1
     {     0,      0,      0,      0,       0,      0,       0,      0,     0,     0,      0,      0, 0  }},    // 13    WATER_2
    {  1,  1,  1,  1,  2,  3,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,
       5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
       5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
       5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  4,  3,  2,  1,  1,  0}},

    {{ 0,12},    //LUT num. AUTOMATIC  INDOOR
      { 0, 1},    //LUT num. INCANDESCENT
      { 1, 1},    //LUT num. D4000
      { 2, 1},    //LUT num. D5000
      { 2, 2},    //LUT num. SUNNY
      { 3, 2},    //LUT num. CLOUDY
      { 5, 1},    //LUT num. D9000
      { 6, 1},    //LUT num. D10000
      { 7, 1},    //LUT num. FLASH
      { 8, 1},    //LUT num. FLUORESCENT
      { 9, 1},    //LUT num. FLUORESCENT_2
      {10, 1},    //LUT num. FLUORESCENT_3
     {11, 1},    //LUT num. FLUORESCENT_4
     {12, 2},    //LUT num. WATER
     { 2, 5}},    //LUT num. AUTOMATIC  OUTDOOR

    // WB Gain            R ratio       G ratio       B ratio
    {{{8956, 4096, 6621},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //AUTOMATIC
      {{5127, 4096,12792},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //INCANDESCENT
      {{7758, 4096, 9014},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //D4000
      {{8646, 4096, 7575},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //D5000
      {{9377, 4096, 6629},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //SUNNY
      {{10181,4096, 6020},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //CLOUDY
      {{7720, 4096, 5570},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //D9000
      {{7720, 4096, 5570},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //D10000
      {{7580, 4096, 5980},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //FLASH
      {{10342, 4096,7041},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //FLUORESCENT
      {{10261, 4096,6870},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //FLUORESCENT_2
      {{9130, 4096, 8300},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //FLUORESCENT_3
     {{9130, 4096, 8300},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //FLUORESCENT_4
     {{7720, 4096, 5570},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}},//WATER

       384,    //count
   {{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    //AWB_METER_CENTER
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 3, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 4, 4, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 4, 5, 5, 4, 3, 3, 2, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 2, 2, 3, 4, 5, 5, 5, 5, 4, 3, 2, 2, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 2, 2, 3, 4, 5, 5, 5, 5, 4, 3, 2, 2, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 2, 2, 3, 4, 5, 5, 5, 5, 4, 3, 2, 2, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 2, 3, 3, 4, 4, 5, 5, 5, 5, 4, 4, 3, 3, 2, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 4, 4, 4, 3, 3, 2, 2, 1, 1, 1, 1,
      1, 1, 1, 2, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 2, 1, 1, 1,
      1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 2, 2, 1, 1,
      1, 2, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 2, 1,
      2, 2, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 2, 2},

    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    //AWB_METER_AVERAGE
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    //AWB_METER_SPOT
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
},

/* flash ae awb algorithm parameter settings */
{
0
},

/* sensor parameters settings */
{
    0,            //default_diff_table_num;
    NULL,         //default_value;
    0,            //start_idx
    0,            //secon_table_num
    NULL,         //default_value;
},

/* AF control parameter settings */
{
    7,                  //(CAF_CAP | SAF_CAP | ZTRACK_CAP)
    {AF_FULL_RANGE, CAF_AREA_NORMAL,  AF_PS_LEVEL0, 0},  //continuous AF setting;
    {AF_FULL_RANGE, SAF_AREA_MULTI_9, AF_PS_LEVEL0, 0},  // still AF setting
    {0, 0, 0, 0},   // reserve
    {0, 0, 0, 0},   // reserve
    {0, 0, 0, 0}    // reserve
},

/* EIS parameters settings */
{
    1,            // video eis sampling rate
    5,            // still eis sampling rate
    10,           // max dzoom percentage for EIS range
    10,           // min dzoom percentage for EIS range
    1,            // flag for keeping dzoom factor when EIS is disabled
    100,          // x eis strength (0~100)
    100,          // y eis strength (0~100)
    100,          // x rsc strength (0~100)
    100,          // y rsc strength (0~100)
    15            // minumum frame rate
},


/* Scene mode parameter settings */
{
     //scene_mode_idx
    {SCENE_OFF ,SCENE_FLASH ,SCENE_TV_OFF ,SCENE_AV_OFF ,SCENE_SV_OFF ,SCENE_TV_ONLY ,SCENE_AV_ONLY ,SCENE_SV_ONLY ,
     SCENE_NIGHT ,SCENE_NIGHT_PORTRAIT ,SCENE_SPORTS ,SCENE_LANDSCAPE ,SCENE_PORTRAIT ,SCENE_SUNSET ,SCENE_SAND_SNOW ,SCENE_FLOWER ,
     SCENE_FIRE_WORK ,SCENE_WATER ,SCENE_BACK_LIGHT ,SCENE_BACK_LIGHT_PORTRAIT ,SCENE_TRIPOD ,SCENE_BLUE_SKY ,SCENE_MACRO ,SCENE_MACRO_TEXT ,
     SCENE_ARENA ,SCENE_D_LIGHTING ,SCENE_MUSEUM ,SCENE_BEACH ,SCENE_CHILDREN ,SCENE_PARTY ,SCENE_FISHEYE ,SCENE_INDOOR ,
     SCENE_THROUGH_GLASS ,SCENE_PANNING ,SCENE_PHOTO_FRAME ,SCENE_LOMO ,SCENE_SELF_PORTRAIT ,SCENE_CAR_DV ,SCENE_CAR_DV1, SCENE_CAR_HDR},                                                                                                               //sc_set04
     40,              //max_table_count;

     {SCENE_OFF},    //video_scene_mode;
     {SCENE_OFF},    //still_scene_mode;

    //Video LV_Luma_control
     {    2489,  // LV  0
        2617,  // LV  1
        2745,  // LV  2
        2873,  // LV  3
        3001,  // LV  4
        3128,  // LV  5
        3256,  // LV  6
        3377,  // LV  7
        3499,  // LV  8
        3630,  // LV  9
        3756,  // LV 10
        3887,  // LV 11
        4014,  // LV 12
        4142,  // LV 13
        4270,  // LV 14
        4398,  // LV 15
        4526,  // LV 16
        4654,  // LV 17
        4782,  // LV 18
        4910,  // LV 19
        5039}, // LV 20

    //Still LV_Luma_control
    {    2489,  // LV  0
        2617,  // LV  1
        2745,  // LV  2
        2873,  // LV  3
        3001,  // LV  4
        3128,  // LV  5
        3256,  // LV  6
        3377,  // LV  7
        3499,  // LV  8
        3630,  // LV  9
        3756,  // LV 10
        3887,  // LV 11
        4014,  // LV 12
        4142,  // LV 13
        4270,  // LV 14
        4398,  // LV 15
        4526,  // LV 16
        4654,  // LV 17
        4782,  // LV 18
        4910,  // LV 19
        5039}, // LV 20

    {{0},{0},{0},0,0,{{0}},0,0},
    {//Scene control parameter settings
                {//light_condition
                {//SCENE_OFF : 0
                    //auto_knee    start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,            0,     32,               200,             600,               1200,        2400},
                    //gamma        start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,            0,     32,               200,             600,               1200,        2400},
                    //bst_ratio       start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {DISABLE,           0,     52,                64,             600,                714,        2400},
                    //CbCr         start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,            0,     32,               200,             600,               1200,        2400},
                    //LC00        start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {DISABLE,         128,    255,                 0,              32,                714,        1024},
                    //LC01       start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {DISABLE,         128,    255,                64,             192,                320,         448}},

                {//BACK_LIGHT : 1
                    //auto_knee    start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,          192,    255,                0,             240,                 320,         640},
                    //gamma        start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {DISABLE,           0,     64,              128,             448,                 512,         928},
                    //bst_ratio       start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {DISABLE,           0,     64,              128,             448,                 512,         928},
                    //CbCr         start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,            0,      8,                0,             205,                 410,        1229},
                    //LC00        start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,          128,    255,                0,              32,                714,        1024},
                    //LC01       start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,          128,    255,               64,             192,                320,         448}},

                {//D_LIGHTING : 2
                    //auto_knee    start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,          192,    255,               0,             240,                  320,         640},
                    //gamma        start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {DISABLE,           0,     64,             128,             448,                  512,         928},
                    //bst_ratio       start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,            0,     64,             128,             448,                  512,         928},
                    //CbCr         start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,            0,      8,               0,             205,                  410,        1229},
                    //LC00        start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,          128,    255,                0,              32,                714,        1024},
                    //LC01       start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,          128,    255,               64,             192,                320,         448}},

                {//CAR_DV : 3
                    //auto_knee    start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,          192,    255,               0,             240,                  320,         640},
                    //gamma        start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {DISABLE,           0,     64,             128,             448,                  512,         928},
                    //bst_ratio       start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,            0,     64,             128,             448,                  512,         928},
                    //CbCr         start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,            0,      8,               0,             205,                  410,        1229},
                    //LC00        start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,          128,    255,                0,              32,                714,        1024},
                    //LC01       start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,          128,    255,               64,             192,                320,         448}}},



           //detect_priority
           {SCENE_BACK_LIGHT_PORTRAIT,
            SCENE_NIGHT_PORTRAIT,
            SCENE_PORTRAIT,
            SCENE_LANDSCAPE,
            SCENE_NIGHT,
            SCENE_MACRO,
            SCENE_OFF},

                7,            //scene_detect_no
               15,            //skip_frames

               //          lv_no       distance    motion
               // face     min  max    min  max    min  max   histo_min_no  min_thrd_lvl   histo_max_no  max_thrd_lvl
               {{    1,      0,   0,     0,   0,     0,   0,       482,          26,             482,         128},   //SCENE_BACK_LIGHT_PORTRAIT
                {    0,   1100,   0,     0,   0,     0,   0,         0,           0,               0,           0},   //SCENE_LANDSCAPE
                {    1,      0, 500,     0,   0,     0,   0,         0,           0,               0,           0},   //SCENE_NIGHT_PORTRAIT
                {    0,      0, 500,     0,   0,     0,   0,         0,           0,               0,           0},   //SCENE_NIGHT
                {    1,      0,   0,     0,   0,     0,   0,         0,           0,               0,           0},   //SCENE_PORTRAIT
                {    0,      0,   0,     0,   0,     0,   0,         0,           0,               0,           0}}}  //SCENE_MACRO
}
};