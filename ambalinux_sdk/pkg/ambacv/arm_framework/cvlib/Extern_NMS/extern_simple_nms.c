/**
 *  @file extern_simple_nms.c
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
 *  @detailed Implementation of Extern NMS APIs for Single Input Single Output with Attr Example
 *
 */

#include "cvapi_extern_simple_nms.h"

#include "cvapi_armcvt_nms.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define MAX_HDLR            8U

#define AMBACNN_SUCCESS     0U
#define AMBACNN_ERROR       1U

#define FP_TRANSFER_MODE    0U

// Use hdlr to choose which idx to be used
static ODHdlr_s NMS_Hdlr[MAX_HDLR];
static uint32_t num_nms_hdlr = 0U;

// Profiling
static struct timeval t1, t2;

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
extern_simple_nms_init(int* cid, amba_cnn_c_vcoord_t* osz,
                amba_cnn_c_data_format_t* idf,
                amba_cnn_c_data_format_t* odf, int* data_mode,
                int num_src, const amba_cnn_c_vcoord_t* isz,
                const char* attr, const void* reserved)
{
    if (num_nms_hdlr >= MAX_HDLR) {
        return AMBACNN_ERROR;
    }

    *cid = get_available_hdlr_id(NMS_Hdlr, MAX_HDLR);
    ODHdlr_s* hdlr = &(NMS_Hdlr[*cid]);
    hdlr->active = true;
    num_nms_hdlr++;

    // params
    attr_to_odhdlr(attr, hdlr);

    // input 1 : predictions
    set_data_format(idf, 1U, 2U, 0U, 7U);   // float32

    // output 1 : detections
    set_vcoord(osz, 1U, 1U, hdlr->max_num_bbx, 6U);
    set_data_format(odf, 1U, 2U, 0U, 7U);   // float32

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
extern_simple_nms_iter_init(int cid, amba_cnn_c_vcoord_t* osz,
                     const amba_cnn_c_vcoord_t* isz,
                     int o_batch_size, const int* i_batch_size)
{
    if (!NMS_Hdlr[cid].active) {
        return AMBACNN_ERROR;
    }

    ODHdlr_s* hdlr = &(NMS_Hdlr[cid]);

    // output 1 : detections
    set_vcoord(osz, 1U, 1U, hdlr->max_num_bbx, 6U);

    return AMBACNN_SUCCESS;
}

/*
 * Please turn to AmbaCnn Extern Node document for more detailed elaborations.
 * cid - context identifier
 * dest_data - pointer to output(s) buffer
 * src_data - pointer to input(s) buffer
 */
int
extern_simple_nms_forward(int cid, void* const* dest_data,
                       const void* const* src_data)
{
    gettimeofday(&t1, 0U);

    if (!NMS_Hdlr[cid].active) {
        return AMBACNN_ERROR;
    }

    ODHdlr_s* hdlr = &(NMS_Hdlr[cid]);

    // input from VP
    const float* p_vp_prediction = (const float*)(src_data[0U]);

    // output placeholder
    nms_dex_bbx_t p_nms_dec_box[hdlr->total_bbox];
    uint32_t num_dec;
    uint32_t max_idx;

    memset(p_nms_dec_box, 0x0, hdlr->total_bbox * sizeof(nms_dex_bbx_t));

    // preprocessing before NMS
    simple_bbox_regression(p_vp_prediction,
                           hdlr->total_bbox, hdlr->num_cls,
                           hdlr->obj_thres, hdlr->conf_thres,
                           p_nms_dec_box, &num_dec, &max_idx);

    // NMS
    uint32_t num_objs = bbox_nms(p_nms_dec_box, num_dec, max_idx,
                                 hdlr->overlap_thres, hdlr->max_num_bbx,
                                 dest_data, add_to_grouped_out, bbox_iou);

    printf("There are totally %d bounding boxes detected after NMS.\n",
           num_objs);

    gettimeofday(&t2, 0U);

    // Print forward pass execution time here.
    printf(
      "Function extern_simple_nms_forward timing: %.2f ms.\n",
      (t2.tv_sec - t1.tv_sec) * 1000.0f + (t2.tv_usec - t1.tv_usec) / 1000.0f);


    return AMBACNN_SUCCESS;
}

/*
 * The template-generated ARM CVTask C code will not call this function.
 * But cvflowbackend eval will call this function if fwd_quantized=True.
 * So just do what forward function does if this function is called.
 */
int
extern_simple_nms_quantized_forward(int cid, void* const* dest_data,
                             const void* const* src_data)
{
    return extern_simple_nms_forward(cid, dest_data, src_data);
}

/*
 * The template-generated ARM CVTask C code will not call this function.
 * But AmbaCnn Extern Node requires users to provide this interface.
 */
int
extern_simple_nms_backward(int cid, void* const* dloss_dsrc,
                    const void* const* dloss_ddest)
{
    return AMBACNN_SUCCESS;
}

/*
 * Please turn to AmbaCnn Extern Node document for more detailed elaborations.
 * cid - context identifier
 */
int
extern_simple_nms_release(int cid)
{
    NMS_Hdlr[cid].active = false;
    num_nms_hdlr--;
    return AMBACNN_SUCCESS;
}

