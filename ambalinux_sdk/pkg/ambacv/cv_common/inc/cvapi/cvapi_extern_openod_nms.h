/******************************************************************************/
/*  Copyright 2022 Ambarella Inc.                                             */
/*  Author: Tsun-Han Huang                                                    */
/*  Email:  thhuang@ambarella.com                                             */
/******************************************************************************/

#ifndef CVAPI_EXTERN_OPENOD_NMS_H_FILE
#define CVAPI_EXTERN_OPENOD_NMS_H_FILE

#include "cvapi_amba_cnn_c_types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int
extern_openod_nms_init(int* cid, amba_cnn_c_vcoord_t* osz,
                       amba_cnn_c_data_format_t* idf,
                       amba_cnn_c_data_format_t* odf, int* data_mode,
                       int num_src, const amba_cnn_c_vcoord_t* isz,
                       const char* attr,
                       const void* reserved);

extern int
extern_openod_nms_iter_init(int cid, amba_cnn_c_vcoord_t* osz,
                            const amba_cnn_c_vcoord_t* isz,
                            int o_batch_size, const int* i_batch_size);

extern int
extern_openod_nms_forward(int cid, void* const* dest_data,
                          const void* const* src_data);

extern int
extern_openod_nms_quantized_forward(int cid, void* const* dest_data,
                                    const void* const* src_data);

extern int
extern_openod_nms_backward(int cid, void* const* dloss_dsrc,
                           const void* const* dloss_ddest);

extern int
extern_openod_nms_release(int cid);

#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif  /* CVAPI_EXTERN_OPENOD_NMS_H_FILE */
