/**
 * @file AmbaIQParamAR0233SHDR2_12bTo20b_CV2FS_DefaultParams.c
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

AAA_PARAM_s AmbaIQParamAR0233SHDR2_12bTo20bDefParams = {

        0x17110900,        // struct_version_number
        0x20082700,        // param_version_number

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
      2,                   //video_ae_speed : 0~6
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
{    ANTI_FLICKER_NO_60HZ,    //flicker_mode;
     1234,                 //video_shutter_max_idx;

    {    //min_agc_db
              0,

        //max_agc_db
        //   <1/120  <=1/100  <=1/50   <=1/25   <=1/12.5  <=1/6.2   >1/6.2    // Main_Shutter
        {{      12,      12,      12,      12,      12,      12,      12},    //  <1/120
         {      12,      12,      12,      12,      12,      12,      12},    //  <=1/100
         {       6,       6,      23,      23,      23,      23,      23},    //  <=1/50
         {       6,       6,       6,      18,      18,      18,      18},    //  <=1/25
         {      12,      12,      12,      12,      12,      12,      12},    //  <=1/12.5
         {      12,      12,      12,      12,      12,      12,      12},    //  <=1/6.2
         {      12,      12,      12,      12,      12,      12,      12},    //  >1/6.2
        //     PIC   <=1/100  <=1/50   <=1/25   <=1/12.5  <=1/6.2   >1/6.2
         {       6,      12,      18,      30,      30,      30,      30}},   // PHOTO_PREVIEW

        //max_dgain_db
        //   <1/120  <=1/100  <=1/50   <=1/25   <=1/12.5  <=1/6.2   >1/6.2    // Main_Shutter
        {{       0,       0,       0,       0,       0,       0,       0},    //  <1/120
         {       0,       0,       0,       0,       0,       0,       0},    //  <=1/100
         {       0,       0,       0,       0,       0,       0,       0},    //  <=1/50
         {       0,       0,       0,       0,       0,       0,       0},    //  <=1/25
         {       0,       0,       0,       0,       0,       0,       0},    //  <=1/12.5
         {       0,       0,       0,       0,       0,       0,       0},    //  <=1/6.2
         {       0,       0,       0,       0,       0,       0,       0},    //  >1/6.2

        //     PIC   <=1/100  <=1/50   <=1/25   <=1/12.5  <=1/6.2   >1/6.2
         {       1,       1,       1,       1,       1,       1,       1}}},  // PHOTO_PREVIEW

         AA_TOTAL_01VIN_01AE,        //vin_total;
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
     //                               y_a_min_slope,a_min,y_a_max_slope,a_max, //
     //                                                                  y_b_min_slope,b_min,y_b_max_slope,b_max, weighting //
    {{   600,   5500,   8000,  16000,   -2600,  10900,   -9510,  23000,  8400, -1320,   5500,  13010, 1 },    // 0    INCANDESCENT 1700,   5500,   8200,  13900,   -4400,  12900,   -8200,  21200,  6300,   600,   2100,  12500
     {  4500,   6170,   6350,  10000,   -7210,  15930,  -13635,  28105, 12930,-11230,   8585,  -1470, 2 },    // 1    D4000
     {  5430,   6850,   5100,   7980,   -6400,  14840,   -5690,  16220,  5023, -2385,  10003,  -6975, 8 },    // 2    D5000
     {  6140,   7700,   4120,   6710,   -5320,  13185,   -6760,  18010,  9425,-12260,  13780, -14990,16 },    // 3    SUNNY
     {  6350,   8146,   3840,   6385,   -4700,  12135,   -4460,  14050,  7870,-10510,  19040, -23855, 8 },    // 4    CLOUDY
     {  6600,   8750,   7500,  10500,   -3800,  14200,   -4500,  19000,  4800, -1900,  10000,  -7000, 0 },    // 5    D9000
     {  9000,  11000,   4400,   5800,   -5000,  16000,   -6000,  21000,  2200, -1200,   5200,  -6000, 0 },    // 6    D10000, Sky
     {     0,      0,      0,      0,       0,      0,       0,      0,     0,     0,      0,      0, 0 },    // 7    FLASH
     {  7000,   8800,   6200,   8500,   -2300,  10500,   -2610,  13270,  5600, -5300,   2200,   4520, 2 },    // 8    FLUORESCENT
     {  6800,  10700,   8790,  11800,  -16000,  38000,   -6000,  23500,  3200, -1800,   6000,   2000, 0 },    // 9    FLUORESCENT_2
     {     0,      0,      0,      0,       0,      0,       0,      0,     0,     0,      0,      0, 0 },    // 10    FLUORESCENT_3
     {     0,      0,      0,      0,       0,      0,       0,      0,     0,     0,      0,      0, 0 },    // 11    FLUORESCENT_4
     {     0,      0,      0,      0,       0,      0,       0,      0,     0,     0,      0,      0, 0 },    // 12    WATER_1
     {     0,      0,      0,      0,       0,      0,       0,      0,     0,     0,      0,      0, 0 }},    // 13    WATER_2
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
    {{{6262, 4096, 6156},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //AUTOMATIC
     {{3619, 4096,11107},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //INCANDESCENT
     {{5517, 4096, 7267},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //D4000
     {{6262, 4096, 6156},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //D5000
     {{6857, 4096, 5207},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //SUNNY
     {{7101, 4096, 5070},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //CLOUDY
     {{8147, 4096, 6109},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //D9000
     {{8147, 4096, 6109},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //D10000
     {{8147, 4096, 6109},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //FLASH
     {{7854, 4096, 6953},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //FLUORESCENT
     {{8700, 4096, 9900},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //FLUORESCENT_2
     {{8936, 4096, 6690},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //FLUORESCENT_3
     {{8936, 4096, 6690},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}, //FLUORESCENT_4
     {{8147, 4096, 6109},{WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN}}},//WATER

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
    {   2455,  // LV  0
        2583,  // LV  1
        2711,  // LV  2
        2839,  // LV  3
        2967,  // LV  4
        3083,  // LV  5
        3192,  // LV  6
        3320,  // LV  7
        3423,  // LV  8
        3521,  // LV  9
        3617,  // LV 10
        3719,  // LV 11
        3821,  // LV 12
        3925,  // LV 13
        4032,  // LV 14
        4160,  // LV 15
        4288,  // LV 16
        4416,  // LV 17
        4544,  // LV 18
        4672,  // LV 19
        4800}, // LV 20

    //Still LV_Luma_control
    {   2455,  // LV  0
        2583,  // LV  1
        2711,  // LV  2
        2839,  // LV  3
        2967,  // LV  4
        3083,  // LV  5
        3192,  // LV  6
        3320,  // LV  7
        3423,  // LV  8
        3521,  // LV  9
        3617,  // LV 10
        3719,  // LV 11
        3821,  // LV 12
        3925,  // LV 13
        4032,  // LV 14
        4160,  // LV 15
        4288,  // LV 16
        4416,  // LV 17
        4544,  // LV 18
        4672,  // LV 19
        4800}, // LV 20

    {{0},{0},{0},0,0,{{0}},0,0},
    {//Scene control parameter settings
                {//light_condition
                {//SCENE_OFF : 0
                    //auto_knee    start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,         128,    255,                0,             310,                 372,         682},
                    //gamma        start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,           0,      8,               64,             652,                 714,        1024},  //64, 652, 714, 1024
                    //bst_ratio       start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {DISABLE,           0,      8,               64,             652,                 714,        1024},
                    //CbCr         start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,            0,      8,                0,             205,                 410,        1229},
                    //LC00        start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,          128,    255,                0,              32,                714,        1024},
                    //LC01       start_lvl end_lvl     histo_min_no histo_mid_min_no     histo_mid_max_no histo_max_no
                    {ENABLE,          128,    255,               64,             192,                320,         448}},

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
