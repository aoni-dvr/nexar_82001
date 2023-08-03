/**
 *  @file extern_openod_nms.c
 *
 *  @copyright Copyright (c) 2022-Present Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @detailed Implementation of Extern NMS APIs for OpenOD
 *
 */

#include "cvapi_extern_openod_nms.h"

#include "cvapi_armcvt_nms.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define MAX_HDLR            8U

#define AMBACNN_SUCCESS     0U
#define AMBACNN_ERROR       1U

#define FP_TRANSFER_MODE    0U

#define IMG_W               1024U
#define IMG_H               640U

// Use hdlr to choose which idx to be used
static ODHdlr_s OpenOD_Hdlr[MAX_HDLR];
static uint32_t num_openod_hdlr = 0U;

// Profiling
static struct timeval t1, t2;

// Parameters
extern float cpu_params_start[];
extern float cpu_params_end[];

/*
 * Please turn to AmbaCnn Extern Node document for more detailed elaborations.
 * cid - context identifier
 * osz - output size
 * idf - input data format
 * odf - output data format
 * data_mode - data transfer mode
 * num_src - number of inputs
 * isz - input size
 * attr - attributes
 * reserved - reserved pointer
 */
int
extern_openod_nms_init(int* cid, amba_cnn_c_vcoord_t* osz,
                       amba_cnn_c_data_format_t* idf,
                       amba_cnn_c_data_format_t* odf, int* data_mode,
                       int num_src, const amba_cnn_c_vcoord_t* isz,
                       const char* attr, const void* reserved)
{
    if (num_openod_hdlr >= MAX_HDLR) {
        return AMBACNN_ERROR;
    }

    float* params_start = cpu_params_start;
    float* params_end = cpu_params_end;
    uint32_t params_size = (uint32_t) (params_end - params_start);

    *cid = get_available_hdlr_id(OpenOD_Hdlr, MAX_HDLR);
    ODHdlr_s* hdlr = &(OpenOD_Hdlr[*cid]);
    hdlr->active = true;
    num_openod_hdlr++;

    // prior boxes
    hdlr->prior_box = params_start;
    hdlr->prior_var = params_start + (params_size >> 1U);

    // params
    hdlr->total_bbox = params_size >> 3U;
    hdlr->num_cls = 4U;
    hdlr->obj_thres = 0.1f;
    hdlr->conf_thres = 0.4f;
    hdlr->overlap_thres = 0.45f;
    hdlr->max_num_bbx = 150U;

    // input 1 : arm_conf
    set_data_format(&idf[0U], 1U, 2U, 0U, 7U);   // float32

    // input 2 : arm_loc
    set_data_format(&idf[1U], 1U, 2U, 0U, 7U);   // float32

    // input 3 : odm_conf
    set_data_format(&idf[2U], 1U, 2U, 0U, 7U);   // float32

    // input 4 : odm_loc
    set_data_format(&idf[3U], 1U, 2U, 0U, 7U);   // float32

    // output 1 : num_objects
    set_vcoord(&osz[0U], 1U, 1U, 1U, 1U);
    set_data_format(&odf[0U], 0U, 2U, 0U, 0U);   // uint32

    // output 2 : decbox
    set_vcoord(&osz[1U], 1U, 1U, hdlr->max_num_bbx, 4U);
    set_data_format(&odf[1U], 1U, 2U, 0U, 7U);   // float32

    // output 3 : clsId
    set_vcoord(&osz[2U], 1U, 1U, 1U, hdlr->max_num_bbx);
    set_data_format(&odf[2U], 0U, 2U, 0U, 0U);   // uint32

    // output 4 : score
    set_vcoord(&osz[3U], 1U, 1U, 1U, hdlr->max_num_bbx);
    set_data_format(&odf[3U], 1U, 2U, 0U, 7U);   // float32

    *data_mode = FP_TRANSFER_MODE;

    return AMBACNN_SUCCESS;
}

/*
 * Please turn to AmbaCnn Extern Node document for more detailed elaborations.
 * cid - context identifier
 * osz - output size
 * isz - input size
 * o_batch_size - output batch size
 * i_batch_size - input batch size
 */
int
extern_openod_nms_iter_init(int cid, amba_cnn_c_vcoord_t* osz,
                            const amba_cnn_c_vcoord_t* isz,
                            int o_batch_size, const int* i_batch_size)
{
    if (!OpenOD_Hdlr[cid].active) {
        return AMBACNN_ERROR;
    }

    ODHdlr_s* hdlr = &(OpenOD_Hdlr[cid]);

    // output 1 : num_objects
    set_vcoord(osz, 1U, 1U, 1U, 1U);

    // output 2 : decbox
    set_vcoord(&osz[1U], 1U, 1U, hdlr->max_num_bbx, 4U);

    // output 3 : clsId
    set_vcoord(&osz[2U], 1U, 1U, 1U, hdlr->max_num_bbx);

    // output 4 : score
    set_vcoord(&osz[3U], 1U, 1U, 1U, hdlr->max_num_bbx);

    return AMBACNN_SUCCESS;
}

/*
 * Please turn to AmbaCnn Extern Node document for more detailed elaborations.
 * cid - context identifier
 * dest_data - pointer to output(s) buffer
 * src_data - pointer to input(s) buffer
 */
int
extern_openod_nms_forward(int cid, void* const* dest_data,
                          const void* const* src_data)
{
    gettimeofday(&t1, 0U);

    if (!OpenOD_Hdlr[cid].active) {
        return AMBACNN_ERROR;
    }

    ODHdlr_s* hdlr = &(OpenOD_Hdlr[cid]);

    // inputs from VP
    const float* p_arm_conf = (const float*)(src_data[0U]);
    const float* p_arm_loc = (const float*)(src_data[1U]);
    const float* p_odm_conf = (const float*)(src_data[2U]);
    const float* p_odm_loc = (const float*)(src_data[3U]);

    // output placeholder
    nms_dex_bbx_t p_nms_dec_box[hdlr->total_bbox];
    uint32_t num_dec;
    uint32_t max_idx;

    memset(p_nms_dec_box, 0x0, hdlr->total_bbox * sizeof(nms_dex_bbx_t));

    // preprocessing before NMS
    openod_bbox_regression(p_arm_conf, p_arm_loc, p_odm_conf, p_odm_loc,
                           hdlr->prior_box, hdlr->prior_var,
                           hdlr->total_bbox, hdlr->num_cls,
                           hdlr->obj_thres, hdlr->conf_thres,
                           p_nms_dec_box, &num_dec, &max_idx);

    uint32_t* p_num_objects = (uint32_t*)(dest_data[0U]);

    // NMS
    *p_num_objects = bbox_nms(p_nms_dec_box, num_dec, max_idx,
                              hdlr->overlap_thres, hdlr->max_num_bbx,
                              dest_data, add_to_outputs, bbox_overlap);

    /* The following code is for demo purpose : show the results during Flexi-DAG run. */
    printf("******** numbers with further exact x/y/w/h conversions to match SCA program results\n");

    const char* const cls_name[4U] = {
        "background",
        "car",
        "cycle",
        "person"
    };

    // Print what are in dest_data[0] / dest_data[1] / dest_data[2] / dest_data[3] here.
    printf("num_objects : %d\n", *((uint32_t*)(dest_data[0U])));
    printf("ROI bdbox :  0  0 1024 640 score : 1.0\n");

    for (uint32_t iii = 0U; iii < *p_num_objects; iii++) {
        uint32_t iiix4 = iii * 4U;

        float x1 = ((float*)dest_data[1U])[iiix4] * IMG_W;
        float y1 = ((float*)dest_data[1U])[iiix4 + 1U] * IMG_H;
        float x2 = ((float*)dest_data[1U])[iiix4 + 2U] * IMG_W;
        float y2 = ((float*)dest_data[1U])[iiix4 + 3U] * IMG_H;

        printf("%s", cls_name[((uint32_t*)dest_data[2U])[iii]]);
        printf(" bdbox : %d %d %d %d", (int)x1, (int)y1, (int)(x2 - x1), (int)(y2 - y1));
        printf(" score : %f\n", ((float*)dest_data[3U])[iii]);
    }

    gettimeofday(&t2, 0U);

    // Print forward pass execution time here.
    printf(
      "extern forward timing: %.2f ms\n",
      (t2.tv_sec - t1.tv_sec) * 1000.0f + (t2.tv_usec - t1.tv_usec) / 1000.0f);

    return AMBACNN_SUCCESS;
}

/*
 * The template-generated ARM CVTask C code will not call this function.
 * But cvflowbackend eval will call this function if fwd_quantized=True.
 * So just do what forward function does if this function is called.
 */
int
extern_openod_nms_quantized_forward(int cid, void* const* dest_data,
                                    const void* const* src_data)
{
    return extern_openod_nms_forward(cid, dest_data, src_data);
}

/*
 * The template-generated ARM CVTask C code will not call this function.
 * But AmbaCnn Extern Node requires users to provide this interface.
 */
int
extern_openod_nms_backward(int cid, void* const* dloss_dsrc,
                           const void* const* dloss_ddest)
{
    return AMBACNN_SUCCESS;
}

/*
 * Please turn to AmbaCnn Extern Node document for more detailed elaborations.
 * cid - context identifier
 */
int
extern_openod_nms_release(int cid)
{
    OpenOD_Hdlr[cid].active = false;
    num_openod_hdlr--;
    return AMBACNN_SUCCESS;
}

