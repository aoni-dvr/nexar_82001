/******************************************************************************/
/*  Copyright 2022 Ambarella Inc.                                             */
/*  Structs copied from: CVTools/AmbaCnn/include/api/                         */
/*  Helpers added by: Tsun-Han Huang                                          */
/*  Email: thhuang@ambarella.com                                              */
/******************************************************************************/

#ifndef CVAPI_AMBA_CNN_C_TYPES_H_FILE
#define CVAPI_AMBA_CNN_C_TYPES_H_FILE

#ifdef __cplusplus
extern "C" {
#endif

/* AmbaCnn C data-format struct */
typedef struct amba_cnn_c_data_format_s {
  int undef;
  int sign;
  int datasize;
  int expoff;
  int expbits;
  int bitvector;
} amba_cnn_c_data_format_t;

/* AmbaCnn C data-format setting function */
inline void set_data_format(amba_cnn_c_data_format_t* df,
                            int sign, int datasize, int expoff, int expbits) {
    df->undef = 0U;
    df->sign = sign;
    df->datasize = datasize;
    df->expoff = expoff;
    df->expbits = expbits;
    df->bitvector = 0U;
}

/* AmbaCnn C vcoord struct */
typedef struct amba_cnn_c_vcoord_s {
  int w;
  int h;
  int d;
  int p;
} amba_cnn_c_vcoord_t;

/* AmbaCnn C vcoord struct setting function */
inline void set_vcoord(amba_cnn_c_vcoord_t* sz,
                       int p, int d, int h, int w) {
    sz->p = p;
    sz->d = d;
    sz->h = h;
    sz->w = w;
}

#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif  /* CVAPI_AMBA_CNN_C_TYPES_H_FILE */
