/**
 * @file AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_ScSet03Param.c
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

#include "AmbaTypes.h"
#include "AmbaImg_AaaDef.h"

SCENE_DATA_s SceneDataS03OV2312_SIMULTANEOUS_IR_HJ6074L[8] = {

    {
     0x18072500,              // struct_version_number
     0x18072500,              // param_version_number
     SCENE_FIRE_WORK,
     {// default parameter
      SYSTEM_DEFAULT,        //color_table
      SYSTEM_DEFAULT},       //digital_effect

     {// AE parameter
      //video_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
       (INT32)SYSTEM_DEFAULT,               //slow_shutter;
       0, 0,
       FLASH_ALWAYS_OFF       //flash
      },

      //photo_preview_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
       (INT32)SYSTEM_DEFAULT,               //slow_shutter;
       0, 0,
       FLASH_ALWAYS_OFF       //flash
      },

       //still_exposure_control
       {{// still_ISO_control
         AE_ISO_MIN,     // min_iso_value;
         400,            // max_iso_value-ISO_AUT
         1600,            // max_iso_value-ISO_AUTO_HISO

         {//Zoom_LUT
          0,                //start;
          0,                //end;
          // vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt
          {{{   0,   128,   490},{   128,   128,  490},{   256,   128,  490},{   384,   128,  490},{   512,  128,   490},}}},

         4,                     //table_count
         AE_EXPO_CHK_SHUTTER,      //expo_lut_chk
         //vector  :               0                          128                          256                          384
         // SHUTTER        ISO  IRIS     SHUTTER        ISO  IRIS    SHUTTER         ISO  IRIS    SHUTTER         ISO  IRIS
         {{   { 244,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  0
          {   { 372,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  1
          {   { 500,        800,    1,       372,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  2
          {   { 628,        800,    1,       500,        800,    1,      372,         800,    1,      244,         800,    1}}, // LV  3
          {   { 756,        800,    1,       628,        800,    1,      500,         800,    1,      372,         800,    1}}, // LV  4
          {   { 884,        800,    1,       756,        600,    1,      628,         800,    1,      500,         800,    1}}, // LV  5
          {   {1012,        800,    1,       884,        400,    1,      756,         600,    1,      628,         800,    1}}, // LV  6
          {   {1012,        400,    1,      1012,        300,    1,      884,         400,    1,      756,         600,    1}}, // LV  7
          {   {1012,        200,    1,      1012,        200,    1,     1012,         300,    1,      884,         400,    1}}, // LV  8
          {   {1012,        100,    1,      1012,        150,    1,     1012,         200,    1,     1012,         300,    1}}, // LV  9
          {   {1140, AE_ISO_MIN,    1,      1012,        100,    1,     1012,         150,    1,     1012,         200,    1}}, // LV 10
          {   {1268, AE_ISO_MIN,   82,      1140, AE_ISO_MIN,    1,     1012,         100,    1,     1012,         150,    1}}, // LV 11
          {   {1396, AE_ISO_MIN,   82,      1268, AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1,     1012,         100,    1}}, // LV 12
          {   {1524, AE_ISO_MIN,   82,      1396, AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1}}, // LV 13
          {   {1652, AE_ISO_MIN,   82,      1524, AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1}}, // LV 14
          {   {1780, AE_ISO_MIN,   82,      1652, AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1}}, // LV 15
          {   {1908, AE_ISO_MIN,   82,      1780, AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1}}, // LV 16
          {   {2036, AE_ISO_MIN,   82,      1908, AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1}}, // LV 17
          {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1}}, // LV 18
          {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1}}, // LV 19
          {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1}}},// LV 20

         {//motion_iso_ratio //unit:128
          0,                //start;
          128,                //end;
          // vector  offset   adapt   vector  offset  adapt
          {{{  128,    16,     64},{    256,     64,     0},}}}
        },

        SYSTEM_DEFAULT,        // still_night_shot
        SYSTEM_DEFAULT,        // still_is
        FLASH_ALWAYS_OFF,      // still_flash
        AE_FLASH_NORMAL        // still_flash_type
       }},

     {// AWB parameter
      SYSTEM_DEFAULT,        // awb_menu_mode
      WB_MENU_REGION,        // awb_menu_mode_type
     },

     {// ADJ parameter
      LIGHT_CONDITION_OFF,//light_condition

      // Video
      // AWB                                   AE
      // low       d50       high     target  satura  Gamma l_expo a_knee  CbCr
      {{ {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [0]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [1]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [2]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [3]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [4]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [5]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  190,   128}}, // [6]:
       { {128,128,  128,128,  128,128,   120,    128,   128,   128,  210,   128}}, // [7]:
       { {128,128,  128,128,  128,128,   112,    128,   128,   128,  230,   128}}, // [8]:
       { {128,128,  128,128,  128,128,   104,    128,   128,   128,  240,   128}}, // [9]:
       { {128,128,  128,128,  128,128,    96,    136,   128,   128,  240,   128}}, // [10]:
       { {128,128,  128,128,  128,128,    88,    117,   128,   128,  240,   128}}, // [11]:
       { {128,128,  128,128,  128,128,    80,    152,   128,   128,  240,   128}}, // [12]:
       { {128,128,  128,128,  128,128,    72,    160,   128,   128,  240,   128}}, // [13]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [14]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [15]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [16]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [17]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [18]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [19]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [20]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [21]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [22]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [23]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}},// [24]:

      // Still
      // AWB                                   AE
      // low       d50       high     target  satura  Gamma l_expo a_knee  CbCr
      {{ {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [0]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [1]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [2]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [3]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [4]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [5]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  190,   128}}, // [6]:
       { {128,128,  128,128,  128,128,   120,    128,   128,   128,  210,   128}}, // [7]:
       { {128,128,  128,128,  128,128,   112,    128,   128,   128,  230,   128}}, // [8]:
       { {128,128,  128,128,  128,128,   104,    128,   128,   128,  240,   128}}, // [9]:
       { {128,128,  128,128,  128,128,    96,    136,   128,   128,  240,   128}}, // [10]:
       { {128,128,  128,128,  128,128,    88,    117,   128,   128,  240,   128}}, // [11]:
       { {128,128,  128,128,  128,128,    80,    152,   128,   128,  240,   128}}, // [12]:
       { {128,128,  128,128,  128,128,    72,    160,   128,   128,  240,   128}}, // [13]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [14]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [15]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [16]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [17]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [18]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [19]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [20]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [21]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [22]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}, // [23]:
       { {128,128,  128,128,  128,128,    64,    160,   128,   128,  240,   128}}},// [24]:
     },
     0.0, //HdrMaxBlendRatio
{ // HDR parameter
       0
     }
    },


    {
     0x18072500,              // struct_version_number
     0x18072500,              // param_version_number
     SCENE_WATER,
     {// default parameter
      SYSTEM_DEFAULT,        //color_table
      SYSTEM_DEFAULT},       //digital_effect

     {// AE parameter
      //video_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
        (INT32)SYSTEM_DEFAULT,               //slow_shutter;
        0, 0,
        SYSTEM_DEFAULT         //flash
      },

      //photo_preview_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
       (INT32)SYSTEM_DEFAULT,               //slow_shutter;
       0, 0,
       SYSTEM_DEFAULT         //flash
      },

      //still_exposure_control
      {{// still_ISO_control
        AE_ISO_MIN,     // min_iso_value;
        400,            // max_iso_value-ISO_AUT
        1600,            // max_iso_value-ISO_AUTO_HISO

        {//Zoom_LUT
         0,                //start;
         0,                //end;
         // vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt
         {{{   0,   128,   490},{   128,   128,  490},{   256,   128,  490},{   384,   128,  490},{   512,   128,  490},}}},

        4,                     //table_count
        AE_EXPO_CHK_SHUTTER,      //expo_lut_chk
        //vector  :               0                          128                          256                          384
        // SHUTTER        ISO  IRIS     SHUTTER        ISO  IRIS    SHUTTER         ISO  IRIS    SHUTTER         ISO  IRIS
        {{   { 244,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  0
         {   { 372,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  1
         {   { 500,        800,    1,       372,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  2
         {   { 628,        800,    1,       500,        800,    1,      372,         800,    1,      244,         800,    1}}, // LV  3
         {   { 756,        800,    1,       628,        800,    1,      500,         800,    1,      372,         800,    1}}, // LV  4
         {   { 884,        800,    1,       756,        600,    1,      628,         800,    1,      500,         800,    1}}, // LV  5
         {   {1012,        800,    1,       884,        400,    1,      756,         600,    1,      628,         800,    1}}, // LV  6
         {   {1012,        400,    1,      1012,        300,    1,      884,         400,    1,      756,         600,    1}}, // LV  7
         {   {1012,        200,    1,      1012,        200,    1,     1012,         300,    1,      884,         400,    1}}, // LV  8
         {   {1012,        100,    1,      1012,        150,    1,     1012,         200,    1,     1012,         300,    1}}, // LV  9
         {   {1140, AE_ISO_MIN,    1,      1012,        100,    1,     1012,         150,    1,     1012,         200,    1}}, // LV 10
         {   {1268, AE_ISO_MIN,   82,      1140, AE_ISO_MIN,    1,     1012,         100,    1,     1012,         150,    1}}, // LV 11
         {   {1396, AE_ISO_MIN,   82,      1268, AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1,     1012,         100,    1}}, // LV 12
         {   {1524, AE_ISO_MIN,   82,      1396, AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1}}, // LV 13
         {   {1652, AE_ISO_MIN,   82,      1524, AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1}}, // LV 14
         {   {1780, AE_ISO_MIN,   82,      1652, AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1}}, // LV 15
         {   {1908, AE_ISO_MIN,   82,      1780, AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1}}, // LV 16
         {   {2036, AE_ISO_MIN,   82,      1908, AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1}}, // LV 17
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1}}, // LV 18
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1}}, // LV 19
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1}}},// LV 20

        {//motion_iso_ratio //unit:128
         0,                //start;
         128,                //end;
        // vector  offset   adapt   vector  offset  adapt
        {{{  128,    16,     64},{    256,     64,     0},}}}
        },

          SYSTEM_DEFAULT,        // still_night_shot
          SYSTEM_DEFAULT,        // still_is
          SYSTEM_DEFAULT,        // still_flash
          AE_FLASH_NORMAL        // still_flash_type
       }},

     {// AWB parameter
      WB_WATER,              // awb_menu_mode
      WB_MENU_REGION,        // awb_menu_mode_type
     },

     {// ADJ parameter
      LIGHT_CONDITION_OFF,//light_condition

      // Video
      // AWB                                   AE
      // low       d50       high     target  satura  Gamma l_expo a_knee  CbCr
      {{ {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [0]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [1]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [2]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [3]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [4]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [5]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  190,   128}}, // [6]:
       { {128,128,  128,128,  128,128,   128,    129,   128,   128,  210,   128}}, // [7]:
       { {128,128,  128,128,  128,128,   128,    130,   128,   128,  230,   128}}, // [8]:
       { {128,128,  128,128,  128,128,   128,    131,   128,   128,  240,   128}}, // [9]:
       { {128,128,  128,128,  128,128,   128,    132,   128,   128,  240,   128}}, // [10]:
       { {128,128,  128,128,  128,128,   128,    133,   128,   128,  240,   128}}, // [11]:
       { {128,128,  128,128,  128,128,   128,    134,   128,   128,  240,   128}}, // [12]:
       { {128,128,  128,128,  128,128,   128,    135,   128,   128,  240,   128}}, // [13]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [14]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [15]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [16]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [17]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [18]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [19]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [20]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [21]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [22]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [23]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}},// [24]:

      // Still
      // AWB                                   AE
      // low       d50       high     target  satura  Gamma l_expo a_knee  CbCr
      {{ {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [0]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [1]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [2]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [3]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [4]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [5]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  190,   128}}, // [6]:
       { {128,128,  128,128,  128,128,   128,    129,   128,   128,  210,   128}}, // [7]:
       { {128,128,  128,128,  128,128,   128,    130,   128,   128,  230,   128}}, // [8]:
       { {128,128,  128,128,  128,128,   128,    131,   128,   128,  240,   128}}, // [9]:
       { {128,128,  128,128,  128,128,   128,    132,   128,   128,  240,   128}}, // [10]:
       { {128,128,  128,128,  128,128,   128,    133,   128,   128,  240,   128}}, // [11]:
       { {128,128,  128,128,  128,128,   128,    134,   128,   128,  240,   128}}, // [12]:
       { {128,128,  128,128,  128,128,   128,    135,   128,   128,  240,   128}}, // [13]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [14]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [15]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [16]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [17]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [18]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [19]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [20]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [21]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [22]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}, // [23]:
       { {128,128,  128,128,  128,128,   128,    136,   128,   128,  240,   128}}},// [24]:
     },
     0.0, //HdrMaxBlendRatio
{ // HDR parameter
       0
     }
    },


    {
     0x18072500,              // struct_version_number
     0x18072500,              // param_version_number
     SCENE_BACK_LIGHT,
     {// default parameter
      SYSTEM_DEFAULT,        //color_table
      SYSTEM_DEFAULT},       //digital_effect

     {// AE parameter
      //video_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
       (INT32)SYSTEM_DEFAULT,               //slow_shutter;
       0, 0,
       SYSTEM_DEFAULT         //flash
      },

      //photo_preview_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
       (INT32)SYSTEM_DEFAULT,               //slow_shutter;
       0, 0,
       SYSTEM_DEFAULT         //flash
      },

      //still_exposure_control
      {{// still_ISO_control
        AE_ISO_MIN,     // min_iso_value;
        400,            // max_iso_value-ISO_AUT
        1600,            // max_iso_value-ISO_AUTO_HISO

        {//Zoom_LUT
         0,                //start;
         0,                //end;
         // vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt
         {{{    0,   128,  490},{   128,   128,  490},{   256,   128,  490},{   384,   128,  490},{   512,   128,  490},}}},

        4,                     //table_count
        AE_EXPO_CHK_SHUTTER,      //expo_lut_chk
        //vector  :               0                          128                          256                          384
        // SHUTTER        ISO  IRIS     SHUTTER        ISO  IRIS    SHUTTER         ISO  IRIS    SHUTTER         ISO  IRIS
        {{   { 244,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  0
         {   { 372,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  1
         {   { 500,        800,    1,       372,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  2
         {   { 628,        800,    1,       500,        800,    1,      372,         800,    1,      244,         800,    1}}, // LV  3
         {   { 756,        800,    1,       628,        800,    1,      500,         800,    1,      372,         800,    1}}, // LV  4
         {   { 884,        800,    1,       756,        600,    1,      628,         800,    1,      500,         800,    1}}, // LV  5
         {   {1012,        800,    1,       884,        400,    1,      756,         600,    1,      628,         800,    1}}, // LV  6
         {   {1012,        400,    1,      1012,        300,    1,      884,         400,    1,      756,         600,    1}}, // LV  7
         {   {1012,        200,    1,      1012,        200,    1,     1012,         300,    1,      884,         400,    1}}, // LV  8
         {   {1012,        100,    1,      1012,        150,    1,     1012,         200,    1,     1012,         300,    1}}, // LV  9
         {   {1140, AE_ISO_MIN,    1,      1012,        100,    1,     1012,         150,    1,     1012,         200,    1}}, // LV 10
         {   {1268, AE_ISO_MIN,   82,      1140, AE_ISO_MIN,    1,     1012,         100,    1,     1012,         150,    1}}, // LV 11
         {   {1396, AE_ISO_MIN,   82,      1268, AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1,     1012,         100,    1}}, // LV 12
         {   {1524, AE_ISO_MIN,   82,      1396, AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1}}, // LV 13
         {   {1652, AE_ISO_MIN,   82,      1524, AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1}}, // LV 14
         {   {1780, AE_ISO_MIN,   82,      1652, AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1}}, // LV 15
         {   {1908, AE_ISO_MIN,   82,      1780, AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1}}, // LV 16
         {   {2036, AE_ISO_MIN,   82,      1908, AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1}}, // LV 17
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1}}, // LV 18
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1}}, // LV 19
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1}}},// LV 20

        {//motion_iso_ratio //unit:128
         0,                //start;
         128,                //end;
         // vector  offset   adapt   vector  offset  adapt
         {{{  128,     16,     64},{    256,    64,     0},}}}
       },

       SYSTEM_DEFAULT,        // still_night_shot
       SYSTEM_DEFAULT,        // still_is
       SYSTEM_DEFAULT,        // still_flash
       AE_FLASH_NORMAL        // still_flash_type
      }},

      {// AWB parameter
       WB_SUNNY,              // awb_menu_mode
       WB_MENU_REGION,        // awb_menu_mode_type
      },

      {// ADJ parameter
       1,//light_condition

       // Video
       // AWB                          AE               gamma            l_expo           a_knee          CbCr
       // low       d50       high     target  satura   min  mid  max    min  mid  max    min  mid  max   min  mid  max
       {{ {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [0]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [1]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [2]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [3]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [4]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [5]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    82, 128, 180,  128, 162, 192}}, // [6]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,   100, 128, 180,  128, 162, 192}}, // [7]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,   128, 128, 180,  128, 162, 192}}, // [8]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,   128, 128, 180,  128, 162, 192}}, // [9]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 120, 255,    64, 120, 255,   128, 128, 180,  128, 162, 192}}, // [10]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 112, 255,    64, 112, 255,   128, 128, 180,  128, 162, 192}}, // [11]:
        { {128,128,  128,128,  128,128,   128,    128,    56,  96, 255,    56,  96, 255,   128, 128, 180,  128, 162, 192}}, // [12]:
        { {128,128,  128,128,  128,128,   128,    128,    48,  80, 255,    48,  80, 255,   128, 128, 180,  128, 162, 192}}, // [13]:
        { {128,128,  128,128,  128,128,   128,    128,    20,  40, 128,    20,  40, 128,   128, 128, 180,  128, 162, 192}}, // [14]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [15]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [16]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [17]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [18]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [19]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [20]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [21]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [22]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [23]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}},// [24]:

       // Still
       // AWB                          AE               gamma            l_expo           a_knee          CbCr
       // low       d50       high     target  satura   min  mid  max    min  mid  max    min  mid  max   min  mid  max
       {{ {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [0]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [1]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [2]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [3]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [4]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [5]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    82, 128, 180,  128, 162, 192}}, // [6]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,   100, 128, 180,  128, 162, 192}}, // [7]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,   128, 128, 180,  128, 162, 192}}, // [8]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,   128, 128, 180,  128, 162, 192}}, // [9]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 120, 255,    64, 120, 255,   128, 128, 180,  128, 162, 192}}, // [10]:
        { {128,128,  128,128,  128,128,   128,    128,    64, 112, 255,    64, 112, 255,   128, 128, 180,  128, 162, 192}}, // [11]:
        { {128,128,  128,128,  128,128,   128,    128,    56,  96, 255,    56,  96, 255,   128, 128, 180,  128, 162, 192}}, // [12]:
        { {128,128,  128,128,  128,128,   128,    128,    48,  80, 255,    48,  80, 255,   128, 128, 180,  128, 162, 192}}, // [13]:
        { {128,128,  128,128,  128,128,   128,    128,    20,  40, 128,    20,  40, 128,   128, 128, 180,  128, 162, 192}}, // [14]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [15]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [16]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [17]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [18]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [19]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [20]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [21]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [22]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [23]:
        { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}},// [24]:
       },

       0.0, //HdrMaxBlendRatio
{ // HDR parameter
         0
       }
      },


    {
     0x18072500,              // struct_version_number
     0x18072500,              // param_version_number
     SCENE_BACK_LIGHT_PORTRAIT,
     {// default parameter
      SYSTEM_DEFAULT,        //color_table
      SYSTEM_DEFAULT},       //digital_effect

     {// AE parameter
      //video_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
        (INT32)SYSTEM_DEFAULT,               //slow_shutter;
        0, 0,
        SYSTEM_DEFAULT         //flash
      },

      //photo_preview_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
       (INT32)SYSTEM_DEFAULT,               //slow_shutter;
       0, 0,
       SYSTEM_DEFAULT         //flash
      },

      //still_exposure_control
      {{// still_ISO_control
        AE_ISO_MIN,     // min_iso_value;
        400,            // max_iso_value-ISO_AUT
        1600,            // max_iso_value-ISO_AUTO_HISO

        {//Zoom_LUT
         0,                //start;
         0,                //end;
         // vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt
         {{{ 0, 128, 490},{ 128, 128, 490},{ 256, 128, 490},{ 384, 128, 490},{ 512, 128, 490},}}},

        4,                     //table_count
        AE_EXPO_CHK_SHUTTER,      //expo_lut_chk
        //vector  :               0                          128                          256                          384
        // SHUTTER        ISO  IRIS     SHUTTER        ISO  IRIS    SHUTTER         ISO  IRIS    SHUTTER         ISO  IRIS
        {{   { 244,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  0
         {   { 372,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  1
         {   { 500,        800,    1,       372,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  2
         {   { 628,        800,    1,       500,        800,    1,      372,         800,    1,      244,         800,    1}}, // LV  3
         {   { 756,        800,    1,       628,        800,    1,      500,         800,    1,      372,         800,    1}}, // LV  4
         {   { 884,        800,    1,       756,        600,    1,      628,         800,    1,      500,         800,    1}}, // LV  5
         {   {1012,        800,    1,       884,        400,    1,      756,         600,    1,      628,         800,    1}}, // LV  6
         {   {1012,        400,    1,      1012,        300,    1,      884,         400,    1,      756,         600,    1}}, // LV  7
         {   {1012,        200,    1,      1012,        200,    1,     1012,         300,    1,      884,         400,    1}}, // LV  8
         {   {1012,        100,    1,      1012,        150,    1,     1012,         200,    1,     1012,         300,    1}}, // LV  9
         {   {1140, AE_ISO_MIN,    1,      1012,        100,    1,     1012,         150,    1,     1012,         200,    1}}, // LV 10
         {   {1268, AE_ISO_MIN,   82,      1140, AE_ISO_MIN,    1,     1012,         100,    1,     1012,         150,    1}}, // LV 11
         {   {1396, AE_ISO_MIN,   82,      1268, AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1,     1012,         100,    1}}, // LV 12
         {   {1524, AE_ISO_MIN,   82,      1396, AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1}}, // LV 13
         {   {1652, AE_ISO_MIN,   82,      1524, AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1}}, // LV 14
         {   {1780, AE_ISO_MIN,   82,      1652, AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1}}, // LV 15
         {   {1908, AE_ISO_MIN,   82,      1780, AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1}}, // LV 16
         {   {2036, AE_ISO_MIN,   82,      1908, AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1}}, // LV 17
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1}}, // LV 18
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1}}, // LV 19
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1}}},// LV 20

        {//motion_iso_ratio //unit:128
         0,                //start;
         128,                //end;
        // vector  offset   adapt   vector  offset  adapt
        {{{ 128, 16, 64},{ 256, 64, 0},}}}
       },

       SYSTEM_DEFAULT,        // still_night_shot
       SYSTEM_DEFAULT,        // still_is
       SYSTEM_DEFAULT,        // still_flash
       AE_FLASH_NORMAL        // still_flash_type
      }},

     {// AWB parameter
      WB_SUNNY,              // awb_menu_mode
      WB_MENU_REGION,        // awb_menu_mode_type
     },

     {// ADJ parameter
     1,//light_condition

     // Video
     // AWB                          AE               gamma            l_expo           a_knee          CbCr
     // low       d50       high     target  satura   min  mid  max    min  mid  max    min  mid  max   min  mid  max
     {{ {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [0]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [1]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [2]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [3]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [4]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [5]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    82, 128, 180,  128, 162, 192}}, // [6]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,   100, 128, 180,  128, 162, 192}}, // [7]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,   128, 128, 180,  128, 162, 192}}, // [8]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,   128, 128, 180,  128, 162, 192}}, // [9]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 120, 255,    64, 120, 255,   128, 128, 180,  128, 162, 192}}, // [10]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 112, 255,    64, 112, 255,   128, 128, 180,  128, 162, 192}}, // [11]:
      { {128,128,  128,128,  128,128,   128,    128,    56,  96, 255,    56,  96, 255,   128, 128, 180,  128, 162, 192}}, // [12]:
      { {128,128,  128,128,  128,128,   128,    128,    48,  80, 255,    48,  80, 255,   128, 128, 180,  128, 162, 192}}, // [13]:
      { {128,128,  128,128,  128,128,   128,    128,    20,  40, 128,    20,  40, 128,   128, 128, 180,  128, 162, 192}}, // [14]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [15]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [16]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [17]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [18]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [19]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [20]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [21]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [22]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [23]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}},// [24]:

     // Still
     // AWB                          AE               gamma            l_expo           a_knee          CbCr
     // low       d50       high     target  satura   min  mid  max    min  mid  max    min  mid  max   min  mid  max
     {{ {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [0]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [1]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [2]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [3]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [4]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    64, 128, 180,  128, 162, 192}}, // [5]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,    82, 128, 180,  128, 162, 192}}, // [6]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,   100, 128, 180,  128, 162, 192}}, // [7]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,   128, 128, 180,  128, 162, 192}}, // [8]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 128, 255,    64, 128, 255,   128, 128, 180,  128, 162, 192}}, // [9]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 120, 255,    64, 120, 255,   128, 128, 180,  128, 162, 192}}, // [10]:
      { {128,128,  128,128,  128,128,   128,    128,    64, 112, 255,    64, 112, 255,   128, 128, 180,  128, 162, 192}}, // [11]:
      { {128,128,  128,128,  128,128,   128,    128,    56,  96, 255,    56,  96, 255,   128, 128, 180,  128, 162, 192}}, // [12]:
      { {128,128,  128,128,  128,128,   128,    128,    48,  80, 255,    48,  80, 255,   128, 128, 180,  128, 162, 192}}, // [13]:
      { {128,128,  128,128,  128,128,   128,    128,    20,  40, 128,    20,  40, 128,   128, 128, 180,  128, 162, 192}}, // [14]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [15]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [16]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [17]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [18]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [19]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [20]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [21]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [22]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}, // [23]:
      { {128,128,  128,128,  128,128,   128,    128,     0,   0,   0,     0,   0,   0,   128, 128, 180,  128, 162, 192}}},// [24]:
     },
     0.0, //HdrMaxBlendRatio
{ // HDR parameter
       0
     }
    },


    {
     0x18072500,              // struct_version_number
     0x18072500,              // param_version_number
     SCENE_TRIPOD,
     {// default parameter
      SYSTEM_DEFAULT,        //color_table
      SYSTEM_DEFAULT},       //digital_effect

     {// AE parameter
      //video_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
       (INT32)SYSTEM_DEFAULT,               //slow_shutter;
       0, 0,
       FLASH_ALWAYS_OFF       //flash
      },

      //photo_preview_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
       (INT32)SYSTEM_DEFAULT,               //slow_shutter;
       0, 0,
       FLASH_ALWAYS_OFF       //flash
      },

      //still_exposure_control
      {{// still_ISO_control
        AE_ISO_MIN,     // min_iso_value;
        400,            // max_iso_value-ISO_AUT
        1600,            // max_iso_value-ISO_AUTO_HISO

        {//Zoom_LUT
         0,                //start;
         0,                //end;
         // vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt
         {{{    0,   128,   490},{  128,   128,   490},{  256,   128,   490},{ 384,   128,   490},{  512,   128,   490},}}},

        4,                     //table_count
        AE_EXPO_CHK_SHUTTER,      //expo_lut_chk
        //vector  :               0                          128                          256                          384
        // SHUTTER        ISO  IRIS     SHUTTER        ISO  IRIS    SHUTTER         ISO  IRIS    SHUTTER         ISO  IRIS
        {{   { 244,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  0
         {   { 372,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  1
         {   { 500,        800,    1,       372,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  2
         {   { 628,        800,    1,       500,        800,    1,      372,         800,    1,      244,         800,    1}}, // LV  3
         {   { 756,        800,    1,       628,        800,    1,      500,         800,    1,      372,         800,    1}}, // LV  4
         {   { 884,        800,    1,       756,        600,    1,      628,         800,    1,      500,         800,    1}}, // LV  5
         {   {1012,        800,    1,       884,        400,    1,      756,         600,    1,      628,         800,    1}}, // LV  6
         {   {1012,        400,    1,      1012,        300,    1,      884,         400,    1,      756,         600,    1}}, // LV  7
         {   {1012,        200,    1,      1012,        200,    1,     1012,         300,    1,      884,         400,    1}}, // LV  8
         {   {1012,        100,    1,      1012,        150,    1,     1012,         200,    1,     1012,         300,    1}}, // LV  9
         {   {1140, AE_ISO_MIN,    1,      1012,        100,    1,     1012,         150,    1,     1012,         200,    1}}, // LV 10
         {   {1268, AE_ISO_MIN,   82,      1140, AE_ISO_MIN,    1,     1012,         100,    1,     1012,         150,    1}}, // LV 11
         {   {1396, AE_ISO_MIN,   82,      1268, AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1,     1012,         100,    1}}, // LV 12
         {   {1524, AE_ISO_MIN,   82,      1396, AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1}}, // LV 13
         {   {1652, AE_ISO_MIN,   82,      1524, AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1}}, // LV 14
         {   {1780, AE_ISO_MIN,   82,      1652, AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1}}, // LV 15
         {   {1908, AE_ISO_MIN,   82,      1780, AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1}}, // LV 16
         {   {2036, AE_ISO_MIN,   82,      1908, AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1}}, // LV 17
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1}}, // LV 18
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1}}, // LV 19
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1}}},// LV 20

        {//motion_iso_ratio //unit:128
         0,                //start;
         128,                //end;
         // vector  offset   adapt   vector  offset  adapt
         {{{  128,     16,     64},{   256,     64,     0},}}}
       },

       SYSTEM_DEFAULT,        // still_night_shot
       SYSTEM_DEFAULT,        // still_is
       FLASH_ALWAYS_OFF,      // still_flash
       AE_FLASH_NORMAL        // still_flash_type
      }},

     {// AWB parameter
      SYSTEM_DEFAULT,        // awb_menu_mode
      WB_MENU_REGION,        // awb_menu_mode_type
     },

     {// ADJ parameter
      LIGHT_CONDITION_OFF,//light_condition

      // Video
      // AWB                                   AE
      // low       d50       high     target  satura  Gamma l_expo a_knee  CbCr
      {{ {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [0]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [1]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [2]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [3]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [4]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [5]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [6]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [7]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [8]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [9]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [10]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [11]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [12]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [13]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [14]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [15]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [16]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [17]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [18]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [19]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [20]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [21]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [22]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [23]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}},// [24]:

      // Still
      // AWB                                   AE
      // low       d50       high     target  satura  Gamma l_expo a_knee  CbCr
      {{ {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [0]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [1]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [2]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [3]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [4]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [5]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [6]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [7]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [8]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [9]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [10]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [11]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [12]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [13]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [14]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [15]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [16]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [17]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [18]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [19]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [20]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [21]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [22]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [23]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}},// [24]:
     },
     0.0, //HdrMaxBlendRatio
{ // HDR parameter
       0
     }
    },


    {
     0x18072500,              // struct_version_number
     0x18072500,              // param_version_number
     SCENE_BLUE_SKY,
     {// default parameter
      SYSTEM_DEFAULT,        //color_table
      SYSTEM_DEFAULT},       //digital_effect

     {// AE parameter
      //video_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
       (INT32)SYSTEM_DEFAULT,               //slow_shutter;
       0, 0,
       FLASH_ALWAYS_OFF       //flash
      },

      //photo_preview_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
      //      iso      60hz-shutter      50hz-shutter  iris
      {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
      (INT32)SYSTEM_DEFAULT,               //slow_shutter;
      0, 0,
      FLASH_ALWAYS_OFF       //flash
      },

      //still_exposure_control
      {{// still_ISO_control
        AE_ISO_MIN,     // min_iso_value;
        400,            // max_iso_value-ISO_AUT
        1600,            // max_iso_value-ISO_AUTO_HISO

        {//Zoom_LUT
         0,                //start;
         0,                //end;
        // vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt
        {{{    0,   128,  490},{   128,   128,  490},{   256,   128,  490},{   384,   128,  490},{   512,   128,  490},}}},

        4,                     //table_count
        AE_EXPO_CHK_SHUTTER,      //expo_lut_chk
        //vector  :               0                          128                          256                          384
        // SHUTTER        ISO  IRIS     SHUTTER        ISO  IRIS    SHUTTER         ISO  IRIS    SHUTTER         ISO  IRIS
        {{   { 244,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  0
         {   { 372,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  1
         {   { 500,        800,    1,       372,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  2
         {   { 628,        800,    1,       500,        800,    1,      372,         800,    1,      244,         800,    1}}, // LV  3
         {   { 756,        800,    1,       628,        800,    1,      500,         800,    1,      372,         800,    1}}, // LV  4
         {   { 884,        800,    1,       756,        600,    1,      628,         800,    1,      500,         800,    1}}, // LV  5
         {   {1012,        800,    1,       884,        400,    1,      756,         600,    1,      628,         800,    1}}, // LV  6
         {   {1012,        400,    1,      1012,        300,    1,      884,         400,    1,      756,         600,    1}}, // LV  7
         {   {1012,        200,    1,      1012,        200,    1,     1012,         300,    1,      884,         400,    1}}, // LV  8
         {   {1012,        100,    1,      1012,        150,    1,     1012,         200,    1,     1012,         300,    1}}, // LV  9
         {   {1140, AE_ISO_MIN,    1,      1012,        100,    1,     1012,         150,    1,     1012,         200,    1}}, // LV 10
         {   {1268, AE_ISO_MIN,   82,      1140, AE_ISO_MIN,    1,     1012,         100,    1,     1012,         150,    1}}, // LV 11
         {   {1396, AE_ISO_MIN,   82,      1268, AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1,     1012,         100,    1}}, // LV 12
         {   {1524, AE_ISO_MIN,   82,      1396, AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1}}, // LV 13
         {   {1652, AE_ISO_MIN,   82,      1524, AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1}}, // LV 14
         {   {1780, AE_ISO_MIN,   82,      1652, AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1}}, // LV 15
         {   {1908, AE_ISO_MIN,   82,      1780, AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1}}, // LV 16
         {   {2036, AE_ISO_MIN,   82,      1908, AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1}}, // LV 17
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1}}, // LV 18
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1}}, // LV 19
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1}}},// LV 20

        {//motion_iso_ratio //unit:128
          0,                //start;
          128,                //end;
        // vector  offset   adapt   vector  offset  adapt
        {{{ 128, 16, 64},{ 256, 64, 0},}}}
       },

       SYSTEM_DEFAULT,        // still_night_shot
       SYSTEM_DEFAULT,        // still_is
       FLASH_ALWAYS_OFF,      // still_flash
       AE_FLASH_NORMAL        // still_flash_type
      }},

     {// AWB parameter
      WB_SUNNY,              // awb_menu_mode
      WB_MENU_REGION,        // awb_menu_mode_type
     },

     {// ADJ parameter
      LIGHT_CONDITION_OFF,//light_condition

      // Video
      // AWB                                   AE
      // low       d50       high     target  satura  Gamma l_expo a_knee  CbCr
      {{ {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [0]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [1]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [2]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [3]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [4]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [5]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [6]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [7]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [8]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [9]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [10]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [11]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [12]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [13]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [14]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [15]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [16]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [17]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [18]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [19]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [20]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [21]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [22]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}, // [23]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,  170,   128}}},// [24]:

      // Still
      // AWB                                   AE
      // low       d50       high     target  satura  Gamma l_expo a_knee  CbCr
      {{ {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [0]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [1]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [2]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [3]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [4]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [5]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [6]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [7]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [8]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [9]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [10]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [11]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [12]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [13]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [14]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [15]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [16]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [17]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [18]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [19]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [20]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [21]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [22]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}, // [23]:
       { {128,128,  128,130,  128,132,   128,    128,   128,   128,   64,   128}}},// [24]:
     },
     0.0, //HdrMaxBlendRatio
{ // HDR parameter
       0
     }
    },


    {
     0x18072500,              // struct_version_number
     0x18072500,              // param_version_number
     SCENE_MACRO,
     {// default parameter
      SYSTEM_DEFAULT,        //color_table
      SYSTEM_DEFAULT},       //digital_effect

     {// AE parameter
      //video_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
       (INT32)SYSTEM_DEFAULT,               //slow_shutter;
       0, 0,
       SYSTEM_DEFAULT         //flash
      },

      //photo_preview_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
       (INT32)SYSTEM_DEFAULT,               //slow_shutter;
       0, 0,
       SYSTEM_DEFAULT         //flash
      },

      //still_exposure_control
      {{// still_ISO_control
        AE_ISO_MIN,     // min_iso_value;
        400,            // max_iso_value-ISO_AUT
        1600,            // max_iso_value-ISO_AUTO_HISO

        {//Zoom_LUT
         0,                //start;
         0,                //end;
         // vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt
         {{{    0,   128,  490},{   128,   128,  490},{   256,   128,  490},{   384,   128,  490},{   512,   128,  490},}}},

        4,                     //table_count
        AE_EXPO_CHK_SHUTTER,      //expo_lut_chk
        //vector  :               0                          128                          256                          384
        // SHUTTER        ISO  IRIS     SHUTTER        ISO  IRIS    SHUTTER         ISO  IRIS    SHUTTER         ISO  IRIS
        {{   { 244,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  0
         {   { 372,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  1
         {   { 500,        800,    1,       372,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  2
         {   { 628,        800,    1,       500,        800,    1,      372,         800,    1,      244,         800,    1}}, // LV  3
         {   { 756,        800,    1,       628,        800,    1,      500,         800,    1,      372,         800,    1}}, // LV  4
         {   { 884,        800,    1,       756,        600,    1,      628,         800,    1,      500,         800,    1}}, // LV  5
         {   {1012,        800,    1,       884,        400,    1,      756,         600,    1,      628,         800,    1}}, // LV  6
         {   {1012,        400,    1,      1012,        300,    1,      884,         400,    1,      756,         600,    1}}, // LV  7
         {   {1012,        200,    1,      1012,        200,    1,     1012,         300,    1,      884,         400,    1}}, // LV  8
         {   {1012,        100,    1,      1012,        150,    1,     1012,         200,    1,     1012,         300,    1}}, // LV  9
         {   {1140, AE_ISO_MIN,    1,      1012,        100,    1,     1012,         150,    1,     1012,         200,    1}}, // LV 10
         {   {1268, AE_ISO_MIN,   82,      1140, AE_ISO_MIN,    1,     1012,         100,    1,     1012,         150,    1}}, // LV 11
         {   {1396, AE_ISO_MIN,   82,      1268, AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1,     1012,         100,    1}}, // LV 12
         {   {1524, AE_ISO_MIN,   82,      1396, AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1}}, // LV 13
         {   {1652, AE_ISO_MIN,   82,      1524, AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1}}, // LV 14
         {   {1780, AE_ISO_MIN,   82,      1652, AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1}}, // LV 15
         {   {1908, AE_ISO_MIN,   82,      1780, AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1}}, // LV 16
         {   {2036, AE_ISO_MIN,   82,      1908, AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1}}, // LV 17
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1}}, // LV 18
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1}}, // LV 19
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1}}},// LV 20

        {//motion_iso_ratio //unit:128
         0,                //start;
         128,                //end;
         // vector  offset   adapt   vector  offset  adapt
         {{{  128,     16,     64},{   256,     64,     0},}}}
       },

       SYSTEM_DEFAULT,        // still_night_shot
       SYSTEM_DEFAULT,        // still_is
       SYSTEM_DEFAULT,        // still_flash
       AE_FLASH_NORMAL        // still_flash_type
      }},

     {// AWB parameter
      SYSTEM_DEFAULT,        // awb_menu_mode
      WB_MENU_REGION,        // awb_menu_mode_type
     },

     {// ADJ parameter
      LIGHT_CONDITION_OFF,//light_condition

      // Video
      // AWB                                   AE
      // low       d50       high     target  satura  Gamma l_expo a_knee  CbCr
      {{ {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [0]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [1]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [2]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [3]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [4]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [5]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [6]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [7]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [8]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [9]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [10]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [11]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [12]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [13]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [14]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [15]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [16]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [17]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [18]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [19]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [20]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [21]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [22]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}, // [23]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,  170,   128}}},// [24]:

      // Still
      // AWB                                   AE
      // low       d50       high     target  satura  Gamma l_expo a_knee  CbCr
      {{ {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [0]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [1]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [2]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [3]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [4]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [5]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [6]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [7]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [8]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [9]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [10]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [11]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [12]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [13]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [14]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [15]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [16]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [17]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [18]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [19]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [20]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [21]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [22]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}, // [23]:
       { {128,128,  128,128,  128,128,   128,    128,   128,   128,   64,   128}}},// [24]:
     },
     0.0, //HdrMaxBlendRatio
{ // HDR parameter
       0
     }
    },


    {
     0x18072500,              // struct_version_number
     0x18072500,              // param_version_number
     SCENE_MACRO_TEXT,
     {// default parameter
      SYSTEM_DEFAULT,        //color_table
      DIGITAL_BW},           //digital_effect

     {// AE parameter
      //video_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
       (INT32)SYSTEM_DEFAULT,               //slow_shutter;
       0, 0,
       FLASH_ALWAYS_OFF       //flash
      },

      //photo_preview_exposure_control
      {AE_APERTURE_NO,        //video_AV_TV_mode
       //      iso      60hz-shutter      50hz-shutter  iris
       {AE_ISO_MIN, AE_SHUTTER_1D120, AE_SHUTTER_1D100,    1},
       (INT32)SYSTEM_DEFAULT,               //slow_shutter;
       0, 0,
       FLASH_ALWAYS_OFF       //flash
      },

      //still_exposure_control
      {{// still_ISO_control
        AE_ISO_MIN,     // min_iso_value;
        400,            // max_iso_value-ISO_AUT
        1600,            // max_iso_value-ISO_AUTO_HISO

        {//Zoom_LUT
         0,                //start;
         0,                //end;
         // vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt   vector offset adapt
         {{{    0,   128,  490},{   128,   128,  490},{   256,   128,  490},{   384,   128,  490},{   512,   128,  490},}}},

        4,                     //table_count
        AE_EXPO_CHK_SHUTTER,      //expo_lut_chk
        //vector  :               0                          128                          256                          384
        // SHUTTER        ISO  IRIS     SHUTTER        ISO  IRIS    SHUTTER         ISO  IRIS    SHUTTER         ISO  IRIS
        {{   { 244,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  0
         {   { 372,        800,    1,       244,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  1
         {   { 500,        800,    1,       372,        800,    1,      244,         800,    1,      244,         800,    1}}, // LV  2
         {   { 628,        800,    1,       500,        800,    1,      372,         800,    1,      244,         800,    1}}, // LV  3
         {   { 756,        800,    1,       628,        800,    1,      500,         800,    1,      372,         800,    1}}, // LV  4
         {   { 884,        800,    1,       756,        600,    1,      628,         800,    1,      500,         800,    1}}, // LV  5
         {   {1012,        800,    1,       884,        400,    1,      756,         600,    1,      628,         800,    1}}, // LV  6
         {   {1012,        400,    1,      1012,        300,    1,      884,         400,    1,      756,         600,    1}}, // LV  7
         {   {1012,        200,    1,      1012,        200,    1,     1012,         300,    1,      884,         400,    1}}, // LV  8
         {   {1012,        100,    1,      1012,        150,    1,     1012,         200,    1,     1012,         300,    1}}, // LV  9
         {   {1140, AE_ISO_MIN,    1,      1012,        100,    1,     1012,         150,    1,     1012,         200,    1}}, // LV 10
         {   {1268, AE_ISO_MIN,   82,      1140, AE_ISO_MIN,    1,     1012,         100,    1,     1012,         150,    1}}, // LV 11
         {   {1396, AE_ISO_MIN,   82,      1268, AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1,     1012,         100,    1}}, // LV 12
         {   {1524, AE_ISO_MIN,   82,      1396, AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1,     1140,  AE_ISO_MIN,    1}}, // LV 13
         {   {1652, AE_ISO_MIN,   82,      1524, AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1,     1268,  AE_ISO_MIN,    1}}, // LV 14
         {   {1780, AE_ISO_MIN,   82,      1652, AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1,     1396,  AE_ISO_MIN,    1}}, // LV 15
         {   {1908, AE_ISO_MIN,   82,      1780, AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1,     1524,  AE_ISO_MIN,    1}}, // LV 16
         {   {2036, AE_ISO_MIN,   82,      1908, AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1,     1652,  AE_ISO_MIN,    1}}, // LV 17
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1,     1780,  AE_ISO_MIN,    1}}, // LV 18
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     1908,  AE_ISO_MIN,    1}}, // LV 19
         {   {2036, AE_ISO_MIN,   82,      2036, AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1,     2036,  AE_ISO_MIN,    1}}},// LV 20

        {//motion_iso_ratio //unit:128
         0,                //start;
         128,                //end;
        // vector  offset   adapt   vector  offset  adapt
        {{{  128,     16,     64},{   256,     64,     0},}}}
       },

       SYSTEM_DEFAULT,        // still_night_shot
       SYSTEM_DEFAULT,        // still_is
       FLASH_ALWAYS_OFF,      // still_flash
       AE_FLASH_NORMAL        // still_flash_type
      }},

     {// AWB parameter
      SYSTEM_DEFAULT,        // awb_menu_mode
      WB_MENU_REGION,        // awb_menu_mode_type
     },

     {// ADJ parameter
      LIGHT_CONDITION_OFF,//light_condition

      // Video
      // AWB                                   AE
      // low       d50       high     target  satura  Gamma l_expo a_knee  CbCr
      {{ {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [0]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [1]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [2]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [3]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [4]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [5]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [6]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [7]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [8]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [9]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [10]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [11]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [12]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [13]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [14]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [15]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [16]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [17]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [18]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [19]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [20]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [21]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [22]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}, // [23]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,  170,   128}}},// [24]:

      // Still
      // AWB                                   AE
      // low       d50       high     target  satura  Gamma l_expo a_knee  CbCr
      {{ {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [0]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [1]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [2]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [3]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [4]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [5]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [6]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [7]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [8]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [9]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [10]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [11]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [12]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [13]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [14]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [15]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [16]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [17]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [18]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [19]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [20]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [21]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [22]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}, // [23]:
       { {128,128,  128,128,  128,128,   256,     64,   128,   128,   64,   128}}},// [24]:
     },
     0.0, //HdrMaxBlendRatio
{ // HDR parameter
       0
     }
    }
};


