/******************************************************************************/
/*  Copyright 2017 Ambarella Inc.                                             */
/*  Author: Pietro Versari                                                    */
/*  Email:  pversari@ambarella.com                                            */
/******************************************************************************/
#ifndef CVTASK_TERRAIN_MAPPING_SR_INTERFACE_INTERNAL_H_FILE
#define CVTASK_TERRAIN_MAPPING_SR_INTERFACE_INTERNAL_H_FILE

#include <cvapi_common.h>
#include <cvapi_terrain_mapping_sr_interface.h>


#define TM_SR_XY_Z_NAME    TM_SR_CVTASK_NAME "_XY_Z"


/* API Message Types */
typedef enum TM_SR_MESSAGE_TYPES
{
    TM_SR_API_INIT_PARAMS = 0,
    TM_SR_API_CONFIG_PARAMS,
    TM_SR_API_CONFIG_ROI
} TM_SR_MESSAGE_TYPES;

/**
 *  tm_api_msg_header
 *  @brief
 *  Message header used in each API message
 *  Note that the header has to be the first element of the message
 */
typedef struct tm_sr_api_msg_header_s {
    uint32_t    tm_sr_msg_type; // TM_SR_MESSAGE_TYPES
} tm_sr_api_msg_header;


typedef union tm_sr_lut_cot_s {
  uint16_t    lut[TM_SR_LUT_COT_WIDTH];
  uint8_t     align[ALIGN_32_BYTE(TM_SR_LUT_COT_WIDTH*2)];
} tm_sr_lut_cot_t;


typedef struct tm_sr_dim_s {
  uint32_t        dem_w;
  uint32_t        dem_h;

  uint32_t        half_octave;
} tm_sr_dim_t;


typedef struct tm_sr_precomp_params_s {
  // Cam to DEM
  uint16_t matrix  [TM_SR_NUM_CAM][3][4]; // float16
  uint16_t max_val [TM_SR_NUM_CAM][3]; // int16_t (viene usato come int, quindi non va messo un numero che poi diventa negativo interpretandolo come int)
  uint16_t min_val [TM_SR_NUM_CAM][3]; // int16_t

  // Stats
  uint16_t occupied_th[ALIGN_16_BYTE(TM_SR_DEM_W)*TM_SR_DEM_H]; // float16 // NOTE: paddo a 16 elementi grossi 2 byte, quindi e' come se paddassi a 32
  uint16_t norm_stat; // float16 // TODO: rimuovi (occhio che devono essere pari altrimenti aggiunge del padding)

  // Curbs
  uint16_t max_height_th_cd; // float16
  uint16_t min_height_var_th_cd; // float16
  uint16_t max_height_var_th_cd; // float16

} tm_sr_precomp_params_t;


typedef struct tm_sr_init_msg_s {
    tm_sr_api_msg_header header;
    tm_sr_precomp_params_t params;
    tm_sr_roi_t roi;
    tm_sr_lut_cot_t cot;

    uint32_t half_octave;
    uint16_t half_octave_width;  // mi servono per sapere le dimensioni dei buffer successivi
    uint16_t half_octave_height;

    // offset ptr
    relative_ptr_t    sin2_u;
    relative_ptr_t    sincos_u;
    relative_ptr_t    cos2_u;
    relative_ptr_t    sin_v;
    relative_ptr_t    cos_v;
} tm_sr_init_msg_t;

typedef struct tm_sr_params_msg_s {
    tm_sr_api_msg_header header;
    tm_sr_precomp_params_t params;
    tm_sr_roi_t roi;
} tm_sr_params_msg_t;



typedef struct tm_sr_tmp_dsi_output_s {
  uint16_t dsi_width;
  uint16_t dsi_height;
//   tm_sr_th_params_t tm_sr_th_params; // Lo metto qui? Cosi configuro un solo cvtask e questo manda le informazioni a chi dipende da lui..
  relative_ptr_t world_nxy;
  relative_ptr_t world_nz;
} tm_sr_tmp_dsi_output_t;


typedef struct tm_sr_th_params_msg_s {
    tm_sr_api_msg_header header;
    tm_sr_th_params_t params;
} tm_sr_th_params_msg_t;


#endif /* CVTASK_TERRAIN_MAPPING_SR_INTERFACE_INTERNAL_H_FILE */
