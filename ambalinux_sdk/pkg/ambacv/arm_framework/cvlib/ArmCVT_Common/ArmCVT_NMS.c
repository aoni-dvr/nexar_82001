/**
 *  @file ArmCVT_NMS.c
 *
 *  @copyright Copyright (c) 2022 Ambarella International LP
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
 *  @detailed Implementation of Non-maximum Suppression for Object Detection
 *
 */

#include "cvapi_armcvt_nms.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Exp          2.71828182845904

#ifndef CUST_FLT_MAX
#define CUST_FLT_MAX 3.402823e+38
#endif

#ifndef CUST_FLT_MIN
#define CUST_FLT_MIN 1.175494e-38
#endif

#define MAX_ATTR_LEN 200U

/*******************************************************************************
 *
 *                      0. private functions for this file
 *
 ******************************************************************************/

#ifndef MIN
static inline float MIN(float a, float b) {
    return (a < b) ? a : b;
}
#endif

#ifndef MAX
static inline float MAX(float a, float b) {
    return (a > b) ? a : b;
}
#endif

static inline float double2float(double val) {
    float flt_val, sign;
    double abs_val;

    sign = (val < 0.0f) ? -1.0f : 1.0f;
    abs_val = fabs(val);

    if (abs_val > CUST_FLT_MAX) {
        flt_val = sign * (float) CUST_FLT_MAX;
    } else if (abs_val < CUST_FLT_MIN) {
        flt_val = sign * (float) CUST_FLT_MIN;
    } else {
        flt_val = (float) val;
    }

    return flt_val;
}

static inline float find_max(const float* cls, uint32_t len, uint32_t* pos) {
    uint32_t idx;
    float max = 0.0f;

    for (idx = 0U; idx < len; idx++) {
        if (cls[idx] > max) {
            max = cls[idx];
            *pos = idx;
        }
    }

    return max;
}

/*******************************************************************************
 *
 *                              1. NMS Public APIs
 *
 ******************************************************************************/

/*
 * Function: bbox_nms
 * ------------------
 * Perform non maximum suppression for all the decoded bounding boxes
 * in nms_dec_box. Add the non supressed boxes (final detection results) to
 * AmbaCnn Extern ARM CVTask output buffer dest_data with max number of detected
 * boxes max_det_num.
 * 
 * %                                  decoded bboxes                           %
 *   nms_dec_box: nms_dex_bbx_t array with decoded bounding boxes inside, sorted
 *                in large to small conf order with prev_idx and next_idx of
 *                each element in nms_dec_box set properly => nms_dec_box is
 *                actually an array storing the nodes of a doubly linked list
 *   nms_num_dec: number of nodes in the doubly linked list we will traverse
 *   nms_max_idx: the idx in array nms_dec_box with max conf, which is the dlist
 *                head node we will start the traversal from
 * %                                  config parameters                        %
 * overlap_thres: if two boxes overlap more than this, less conf one is deleted
 *   max_det_num: max num of bounding boxes in output detection results
 * %                                  output reference                         %
 *     dest_data: pointer to AmbaCnn Extern ARM CVTask output buffer
 * %                                  user-provided functions                  %
 * append_to_out: a user-provided function which takes nms_dex_bbx_t pointer and
 *                current num of detections, and adds the bbox the ptr points to
 *                into dest_data
 *  calc_overlap: a user-provided function which takes two pointers to two
 *                bounding box infos and returns the overlap ratio in float
 *
 *       returns: number of detected bounding boxes after NMS.
 */
uint32_t bbox_nms(nms_dex_bbx_t* nms_dec_box,
                  uint32_t nms_num_dec, uint32_t nms_max_idx,
                  float overlap_thres, uint32_t max_det_num,
                  void* const* dest_data,
                  void (*append_to_out)(const nms_dex_bbx_t*, uint32_t,
                                        void* const*),
                  float (*calc_overlap)(const void*, const void*)) {
    uint32_t i, num_detected = 0U;
    uint32_t idx = nms_max_idx;

    /* go through all the decoded bboxes */
    for (i = 0U; i < nms_num_dec; i++) {

        if (nms_dec_box[idx].conf != 0.0f) {  /* if not already suppressed */

            if (num_detected >= max_det_num) {
                printf("BBX overflows (max = %d) !\n", max_det_num);
                return max_det_num;
            }

            /* copy decoded box info into output arrays */
            append_to_out(&nms_dec_box[idx], num_detected, dest_data);

            num_detected++;

            /* do nms for all the boxes after w.r.t. the current one */
            non_max_suppression(nms_dec_box, idx, nms_num_dec - i - 1,
                                overlap_thres, calc_overlap);

        }

        idx = nms_dec_box[idx].next_idx;
    }

    return num_detected;
}

/* 
 * Function: non_max_suppression
 * -----------------------------
 * Given current node nms_dec_box[curr], go through all the num_after nodes
 * after current node in doubly linked list, which have smaller conf scores,
 * suppress the ones of the same class as current node and overlap with current
 * node more than overlap_thres, via setting their conf scores to 0.0f, where
 * overlap ratios are calculated by user-provided calc_overlap function.
 *
 *   nms_dec_box: nms_dex_bbx_t array with decoded bounding boxes inside, sorted
 *                in large to small conf order with prev_idx and next_idx of
 *                each element in nms_dec_box set properly => nms_dec_box is
 *                actually an array storing the nodes of a doubly linked list
 *          curr: current node's index in nms_dec_box
 *     num_after: num of nodes after current node in the doubly linked list
 *                with conf scores smaller than current node
 * overlap_thres: bboxes overlap more than overlap_thres with current node's
 *                bbox will be suppressed.
 *  calc_overlap: a user-provided function which takes two pointers to two
 *                bounding box infos and returns the overlap ratio in float
 *
 *       returns: void (modify @nms_dec_box array in place,
 *                      setting suppressed conf scores to 0.0f)
 */
void non_max_suppression(nms_dex_bbx_t* nms_dec_box, uint32_t curr,
                         uint32_t num_after, float overlap_thres,
                         float (*calc_overlap)(const void*, const void*)) {
    uint32_t j;
    float overlap;

    /* find the next node of curr node */
    uint32_t next = nms_dec_box[curr].next_idx;

    /* go through all the nodes after curr node */
    for (j = 1U; j <= num_after; j++) {

        /* skip if they are not the same class */
        if (nms_dec_box[next].cls != nms_dec_box[curr].cls) {
            next = nms_dec_box[next].next_idx;
            continue;
        }

        /* skip if they are not overlapped */
        if ((nms_dec_box[next].x1 > nms_dec_box[curr].x2) ||
            (nms_dec_box[next].x2 < nms_dec_box[curr].x1) ||
            (nms_dec_box[next].y1 > nms_dec_box[curr].y2) ||
            (nms_dec_box[next].y2 < nms_dec_box[curr].y1)) {
            next = nms_dec_box[next].next_idx;
            continue;
        }

        /* calculate the overlap ratio */
        overlap = calc_overlap(&nms_dec_box[curr], &nms_dec_box[next]);

        /* node after => non-max  (dlist is sorted in large to small conf)  */
        if (overlap > overlap_thres) {      /* overlap too much */
            /* suppress */
            nms_dec_box[next].conf = 0.0f;  /* conf 0 will be ignored later */
        }

        next = nms_dec_box[next].next_idx;
    }
}

/*******************************************************************************
 *
 *              2. Necessary Preprocessing Function Prior to NMS
 *
 ******************************************************************************/

/*
 * Function: openod_bbox_regression
 * --------------------------------
 * OpenOD's preprocessing step prior to NMS computation.
 * Preprocess VP predicted outputs before really doing NMS:
 *   1. Filter out bboxes whose arm_conf[odd_index] < obj_thres
 *   2. Filter out bboxes whose max pos is 0 (detected class is background)
 *   3. Filter out bboxes whose max odm_conf < conf_thres
 *   4. Save the non filtered out bboxes in nms_dec_box array
 *   5. Maintain the remaining bboxes in large conf to small conf order
 *      as a doubly linked list (dlist) by setting the prev_id and next_id
 *      of each element in nms_dec_box array properly.
 * Return the number of nodes and the head position in the doubly linked list.
 *
 *   %                                 VP outputs                             %
 *   arm_conf: float array with total_bbox * 2 nums to represent all the
 *             VP predicted bboxes anchorbox refinement module (arm) conf scores
 *   arm_loc : float array with total_bbox * 4 nums - xc, yc, w, h of all the
 *             VP predicted bboxes anchorbox refinement module (arm) locations
 *   odm_conf: float array with total_bbox * num_cls nums to represent all the
 *             VP predicted bboxes conf scores in all classes
 *   odm_loc : float array with total_bbox * 4 nums - xc, yc, w, h of all the
 *             VP predicted bboxes object detection module (odm) locations
 *   %                                 prior box                              %
 *   pbox: float array with total_bbox * 4 nums - x1, y1, x2, y2 of prior boxes
 *   var : float array with total_bbox * 4 nums - var for xc, yc, w, h of boxes
 *   %                                 config parameters                      %
 *   total_bbox: number of total bounding boxes VP predicted
 *   num_cls   : number of classes
 *   obj_thres : threshold for arm_conf to be kept
 *   conf_thres: threshold for odm_conf to be kept
 *   %                                 outputs reference                      %
 *   nms_dec_box  : nms_dex_bbx_t array with len = total_bbox for this function
 *                  to fill in the decoded bounding boxes from VP outs for NMS
 *   p_nms_num_dec: pointer to an uint32_t num for this function to fill in
 *                  the num of decoded bboxes (num nodes in dlist)
 *   p_nms_max_idx: pointer to an uint32_t num for this function to fill in
 *                  the idx in array nms_dec_box with max conf (dlist head)
 *
 *   returns: void (outputs are written to @nms_dec_box, @p_nms_num_dec, and
 *                  @p_nms_max_idx)
 */
void openod_bbox_regression(const float* arm_conf, const float* arm_loc,
                            const float* odm_conf, const float* odm_loc,
                            const float* pbox, const float* var,
                            uint32_t total_bbox, uint32_t num_cls,
                            float obj_thres, float conf_thres,
                            nms_dex_bbx_t* nms_dec_box,
                            uint32_t* p_nms_num_dec, uint32_t* p_nms_max_idx) {
    float max, arm_conf_tmp, decbox[4U];
    uint32_t i, ix4, pos = 0U;
    uint32_t num_dec_bbx_l = 0U, max_idx_l = 0U;

    /* go through all the bboxes VP predicted */
    for (i = 0U; i < total_bbox; i++) {
        arm_conf_tmp = arm_conf[(i * 2U) + 1U];

        if (arm_conf_tmp > obj_thres) {
            max = find_max(&odm_conf[i * num_cls], num_cls, &pos);

            if ((pos != 0U) && (max > conf_thres)) {
                ix4 = i * 4U;

                /* decode bounding box from VP predicted locations and
                   prior anchor boxes parameters */
                openod_decode_bbox(&pbox[ix4], &var[ix4],
                                   &arm_loc[ix4], &odm_loc[ix4], decbox);

                /* place the decoded bbox in the proper place of sorted
                   doubly linked list (from large to small in conf) */
                max_idx_l = place_decoded_bbox(nms_dec_box, num_dec_bbx_l,
                                               decbox, pos, max, max_idx_l);

                num_dec_bbx_l++;
            }
        }
    }

    /* num of decoded bboxes in the sorted doubly linked list */
    *p_nms_num_dec = num_dec_bbx_l;

    /* the head of the sorted doubly linked list */
    *p_nms_max_idx = max_idx_l;
}

/*
 * Function: simple_bbox_regression
 * --------------------------------
 * Simple preprocessing step prior to NMS computation. Simplified version of
 * bbox_regression.
 * VP outputs are grouped together in a single output. No prior boxes and
 * no vars are required for decoding.
 *
 *   1. Filter out bboxes whose obj_conf < conf_thres
 *   2. Filter out bboxes whose obj_conf * cls_conf = conf < conf_thres
 *   3. Save the non filtered out bboxes in nms_dec_box array
 *   4. Maintain the remaining bboxes in large conf to small conf order
 *      as a doubly linked list (dlist) by setting the prev_id and next_id
 *      of each element in nms_dec_box array properly.
 *
 * Return the number of nodes and the head position in the doubly linked list.
 *
 *   %                              VP grouped output                         %
 *   vp_prediction: float array with total_bbox * (4 + 1 + num_cls) nums being
 *                  (xc, yc, w, h), (obj_conf), (conf scores in all classes)
 *                  for all the bboxes
 *   %                              config parameters                         %
 *   total_bbox: number of total bounding boxes VP predicted
 *   num_cls   : number of classes
 *   obj_thres : threshold for obj_conf to be kept
 *   conf_thres: threshold for conf (obj_conf * cls_conf) to be kept
 *   %                              outputs reference                         %
 *   nms_dec_box  : nms_dex_bbx_t array with len = total_bbox for this function
 *                  to fill in the decoded bounding boxes from VP outs for NMS
 *   p_nms_num_dec: pointer to an uint32_t num for this function to fill in
 *                  the num of decoded bboxes (num nodes in dlist)
 *   p_nms_max_idx: pointer to an uint32_t num for this function to fill in
 *                  the idx in array nms_dec_box with max conf (dlist head)
 *
 *   returns: void (outputs are written to @nms_dec_box, @p_nms_num_dec, and
 *                  @p_nms_max_idx)
 */
void simple_bbox_regression(const float* vp_prediction,
                            uint32_t total_bbox, uint32_t num_cls,
                            float obj_thres, float conf_thres,
                            nms_dex_bbx_t* nms_dec_box,
                            uint32_t* p_nms_num_dec, uint32_t* p_nms_max_idx) {
    float obj_conf, cls_conf, conf, decbox[4U];
    uint32_t W = 5U + num_cls;   /* len([x, y, w, h, obj_conf]) = 5U */
    uint32_t i, ixW, j;
    uint32_t num_dec_bbx_l = 0U, max_idx_l = 0U;

    /* go through all the bboxes VP predicted */
    for (i = 0U; i < total_bbox; i++) {
        ixW = i * W;
        obj_conf = vp_prediction[ixW + 4U];

        if (obj_conf > obj_thres) {
            /* convert VP predicted bbox location's form */
            xywh_to_xyxy(&vp_prediction[ixW], decbox);

            for (j = 0U; j < num_cls; j++) {
                cls_conf = vp_prediction[ixW + 5U + j];
                conf = obj_conf * cls_conf;

                if (conf > conf_thres) {
                    /* place the decoded bbox in the proper place of sorted
                       doubly linked list (from large to small in conf) */
                    max_idx_l = place_decoded_bbox(nms_dec_box, num_dec_bbx_l,
                                                   decbox, j, conf, max_idx_l);

                    num_dec_bbx_l++;
                }
            }
        }
    }

    /* num of decoded bboxes in the sorted doubly linked list */
    *p_nms_num_dec = num_dec_bbx_l;

    /* the head of the sorted doubly linked list */
    *p_nms_max_idx = max_idx_l;
}

/*******************************************************************************
 *
 *                            3. Decode VP Outputs
 *
 ******************************************************************************/

/*
 * Function: openod_decode_bbox
 * ----------------------------
 * OpenOD's decoding VP outputs step.
 * Decode VP predicted bounding box locations arm_loc[4] and odm_loc[4]
 *                                  into (x1, y1, x2, y2)
 *                                  in the range of [0.0f, 1.0f]
 *                                  with pbox[4] and var[4].
 * Save the decoded bounding box location x1, y1, x2, y2 in decbox[4].
 *
 *      1.            2.              3.              4.               5.
 * pbox -> prior_xywh -> arm_dec_xywh -> odm_dec_xywh -> decbox (xyxy) -> [0, 1]
 *
 *   pbox: prior box {x1, y1, x2, y2}
 *   var:  var for xc, yc, w, h
 *   arm_loc: anchor refinement module (arm) location xc, yc, w, h
 *   odm_loc: object detection  module (odm) location xc, yc, w, h
 *   decbox: float array with len 4 for this function to fill in decoded
 *           results x1, y1, x2, y2
 *
 *   returns: void (outputs are saved in @decbox)
 */
void openod_decode_bbox(const float* pbox, const float* var,
                        const float* arm_loc, const float* odm_loc,
                        float* decbox) {
    float prior_xywh[4], arm_dec_xywh[4], odm_dec_xywh[4];

    /* 1. prior bbox: xyxy -> xywh */
    xyxy_to_xywh(pbox, prior_xywh);

    /* 2. decode bbox: prior xywh -> decoded xywh from arm_loc */
    decode_xywh_from_prior(prior_xywh, var, arm_loc, arm_dec_xywh);

    /* 3. finetune bbox: decoded xywh -> finetuned xywh from odm_loc */
    decode_xywh_from_prior(arm_dec_xywh, var, odm_loc, odm_dec_xywh);

    /* 4. decoded bbox: finetuned xywh -> decoded xyxy */
    xywh_to_xyxy(odm_dec_xywh, decbox);

    /* 5. boundary checking: numbers should be between 0.0f and 1.0f */
    bbox_boundary_cap(decbox, 0.0f, 1.0f);
}

/*
 * Function: decode_xywh_from_prior
 * --------------------------------
 * Caculate xywh from prior xywh and var and VP predicted loc
 *   xc =     var_x * loc_x  * prior_w + prior_x
 *   yc =     var_y * loc_y  * prior_h + prior_y
 *   w  = exp(var_w * loc_w) * prior_w
 *   h  = exp(var_h * loc_h) * prior_h
 *
 *   pbox:   float array with 4 nums representing prior box in xc, yc, w, h
 *   var:    float array with 4 nums representing var for xc, yc, w, h
 *   loc:    float array with 4 nums representing predicted loc in xc, yc, w, h
 *   decbox: float array with len 4 for this function to fill in decoded
 *           results xc, yc, w, h
 *
 *   returns: void (outputs are saved in @decbox)
 */
void decode_xywh_from_prior(const float* pbox, const float* var,
                            const float* loc, float* decbox) {
    uint32_t i;
    float fltmp;
    double exponent, temp;

    /* decode x and y */
    for (i = 0U; i <= 1U; i++) {
        decbox[i] = (var[i] * loc[i] * pbox[i + 2U]) + pbox[i];
    }

    /* decode w and h */
    for (i = 2U; i <= 3U; i++) {
        fltmp = var[i] * loc[i];
        exponent = (double) fltmp;
        temp = pow(Exp, exponent);
        decbox[i] = double2float(temp) * pbox[i];
    }
}

/*****************************************************************************
 % xc, yc, w, h <-> x1, y1, x2, y2 conversion (1: top-left, 2: bottom-right) %
 *****************************************************************************/

/*
 * Function: xyxy_to_xywh
 * ----------------------
 * Convert bbox location representation way from xyxy to xywh.
 *
 *   pbox: an array with 4 float numbers x1, y1, x2, y2 representing
 *         boinding box's top-left point and bottom-right point
 *   decbox: float array with len 4 for this function to fill in
 *           converted results xc, yc, w, h
 *
 *   returns: void (outputs are saved in @decbox)
 */
void xyxy_to_xywh(const float* pbox, float* decbox) {
    decbox[0U] = (pbox[0U] + pbox[2U]) / 2.0f;  /* x center */
    decbox[1U] = (pbox[1U] + pbox[3U]) / 2.0f;  /* y center */
    decbox[2U] = pbox[2U] - pbox[0U];           /*     w    */
    decbox[3U] = pbox[3U] - pbox[1U];           /*     h    */
}

/*
 * Function: xywh_to_xyxy
 * ----------------------
 * Convert bbox location representation way from xywh to xyxy.
 *
 *   pbox: an array with 4 float numbers xc, yc, w, h representing
 *         boinding box's center point, width, and height
 *   decbox: float array with len 4 for this function to fill in
 *           converted results x1, y1, x2, y2
 *
 *   returns: void (outputs are saved in @decbox)
 */
void xywh_to_xyxy(const float* pbox, float* decbox) {
    decbox[0U] = pbox[0U] - (pbox[2U] / 2.0f);  /* x left   */
    decbox[1U] = pbox[1U] - (pbox[3U] / 2.0f);  /* y top    */
    decbox[2U] = pbox[0U] + (pbox[2U] / 2.0f);  /* x right  */
    decbox[3U] = pbox[1U] + (pbox[3U] / 2.0f);  /* y bottom */
}

/*
 * Function: bbox_boundary_cap
 * ---------------------------
 * Cap the bounding box location numbers in [min, max]
 *
 *   decbox: an array with 4 float numbers x1, y1, x2, y2 representing
 *           boinding box's top-left point and bottom-right point
 *   min: min possible value for x1, y1, x2, and y2
 *   max: max possible value for x1, y1, x2, and y2
 *
 *   returns: void (cap and modify the values of @decbox directly)
 */
void bbox_boundary_cap(float* decbox, float min, float max) {
    uint32_t i;
    for (i = 0U; i < 4U; i++) {
        decbox[i]  = (decbox[i] < min) ? min : decbox[i]; /* down to min */
        decbox[i]  = (decbox[i] > max) ? max : decbox[i]; /*  up  to max */
    }
}

/*******************************************************************************
 *
 *                        4. Doubly Linked List Helper
 *
 ******************************************************************************/

/*
 * Function: place_decoded_bbox
 * ----------------------------
 * Append the to-be-added bbox (bbox_loc, cls, conf) to the end of
 *   nms_dec_box array as the cur_idx-th element.
 * Set its and before-node's and after-node's prev_idx and next_idx properly
 *   for the dlist to be sorted in large conf to small conf order.
 * Return the index in array nms_dec_box with max confidence score
 *   (the head position of the dlist) after we insert the to-be-added bbox.
 *
 *   nms_dec_box: nms_dex_bbx_t array with decoded bounding boxes, len > cur_idx
 *   cur_idx: the index which the to-be-added bbox will be in array nms_dec_box
 *            (= num of already decoded bboxes in nms_dec_box before we append)
 *   bbox_loc: float array with 4 nums x1, y1, x2, y2 of to-be-added bbox
 *   cls: an uint32 num representing the class of the detected object
 *   conf: a  float num representing the confidence score of the detected object
 *   before_max_idx: the idx in array nms_dec_box with max conf before we append
 *
 *   returns: the index in array nms_dec_box with max conf score after we append
 *            and insert the to-be-added bbox into the doubly linked list(dlist)
 */
uint32_t place_decoded_bbox(nms_dex_bbx_t* nms_dec_box, uint32_t cur_idx,
                            float* bbox_loc, uint32_t cls, float conf,
                            uint32_t before_max_idx) {
    uint32_t count, idx;
    uint32_t after_max_idx = before_max_idx;

    /* copy the decoded bounding box to nms_dec_box[cur_idx] */
    memcpy(&nms_dec_box[cur_idx], bbox_loc, sizeof(float) * 4U);
    nms_dec_box[cur_idx].cls = cls;
    nms_dec_box[cur_idx].conf = conf;

    /* insert bbox into doubly linked list accoding to conf (insertion sort) */
    if (cur_idx == 0U) {
        /* first node
           both prev and next point to self */
        nms_dec_box[cur_idx].prev_idx = cur_idx;
        nms_dec_box[cur_idx].next_idx = cur_idx;
        after_max_idx = cur_idx;
    } else {
        if (conf > nms_dec_box[before_max_idx].conf) {
            /* current conf > max conf
               put the current one before the max one as the new first node */
            nms_dec_box[cur_idx].prev_idx = cur_idx;
            nms_dec_box[cur_idx].next_idx = before_max_idx;
            nms_dec_box[before_max_idx].prev_idx = cur_idx;
            after_max_idx = cur_idx;  /* new max */
        } else {
            count = cur_idx;  /* how many nodes are there in dlist now */
            idx = before_max_idx;      /* start from the head of dlist */
            while (count > 0U) {
                if (conf > nms_dec_box[idx].conf) {
                    /* find the one whose conf is less than current conf
                       put the current one before the smaller conf one */
                    nms_dec_box[cur_idx].prev_idx = nms_dec_box[idx].prev_idx;
                    nms_dec_box[cur_idx].next_idx = idx;
                    nms_dec_box[nms_dec_box[idx].prev_idx].next_idx = cur_idx;
                    nms_dec_box[idx].prev_idx = cur_idx;
                    break;
                }
                count--;
                if (count == 0U) {
                    /* current one is the smallest one
                       put the current one after the current end as new end */
                    nms_dec_box[cur_idx].prev_idx = idx;
                    nms_dec_box[cur_idx].next_idx = cur_idx;
                    nms_dec_box[idx].next_idx = cur_idx;
                }
                idx = nms_dec_box[idx].next_idx;
            }
        }
    }

    return after_max_idx;
}

/*******************************************************************************
 *
 *                  5. Example Implementation of `calc_overlap`
 *
 ******************************************************************************/

/*
 * Function: bbox_overlap
 * ----------------------
 * Calculate the overlap ratio of bbox_1 and bbox_2:
 *   intersection / second bounding box.
 * Assume that area of bbox_2 > 0.
 *
 *   bbox_1: array with 4 float nums - bbox1's x1, y1, x2, y2
 *   bbox_2: array with 4 float nums - bbox2's x1, y1, x2, y2
 *
 *   returns: interection of bbox_1 and bbox_2 over area of bbox_2
 */
float bbox_overlap(const void* bbox_1, const void* bbox_2) {
    const float* bbox1 = (const float*) bbox_1;
    const float* bbox2 = (const float*) bbox_2;
    float xx1 = MAX(bbox1[0U], bbox2[0U]);  /* larger  x1 */
    float xx2 = MIN(bbox1[2U], bbox2[2U]);  /* smaller x2 */
    float yy1 = MAX(bbox1[1U], bbox2[1U]);  /* larger  y1 */
    float yy2 = MIN(bbox1[3U], bbox2[3U]);  /* smaller y2 */
    float width = MAX(0.0f, xx2 - xx1);
    float height = MAX(0.0f, yy2 - yy1);
    float area = (bbox2[2U] - bbox2[0U]) * (bbox2[3U] - bbox2[1U]);
    return (width * height) / area;
}

/*
 * Function: bbox_iou
 * ------------------
 * Calculate the IOU of bbox_1 and bbox_2: intersection / union.
 * Assume that the area of union > 0.
 *
 *   bbox_1: array with 4 float nums - bbox1's x1, y1, x2, y2
 *   bbox_2: array with 4 float nums - bbox2's x1, y1, x2, y2
 *
 *   returns: IOU of bbox_1 and bbox_2
 */
float bbox_iou(const void* bbox_1, const void* bbox_2) {
    const float* bbox1 = (const float*) bbox_1;
    const float* bbox2 = (const float*) bbox_2;
    float xx1 = MAX(bbox1[0U], bbox2[0U]);  /* larger  x1 */
    float xx2 = MIN(bbox1[2U], bbox2[2U]);  /* smaller x2 */
    float yy1 = MAX(bbox1[1U], bbox2[1U]);  /* larger  y1 */
    float yy2 = MIN(bbox1[3U], bbox2[3U]);  /* smaller y2 */
    float width = MAX(0.0f, xx2 - xx1);
    float height = MAX(0.0f, yy2 - yy1);
    float Intersection = width * height;
    float area1 = (bbox1[2U] - bbox1[0U]) * (bbox1[3U] - bbox1[1U]);
    float area2 = (bbox2[2U] - bbox2[0U]) * (bbox2[3U] - bbox2[1U]);
    float Union = area1 + area2 - Intersection;
    return Intersection / Union;
}

/*******************************************************************************
 *
 *                  6. Example Implementation of `append_to_out`
 *
 ******************************************************************************/

/*
 * Function: add_to_outputs
 * ------------------------
 * Append a newly detected bounding box nms_dec_box into AmbaCnn Extern ARM CVT
 * output buffer dest_data which already had num_detected bounding boxes.
 *
 *    nms_dec_box: a nms_dex_bbx_t pointer to the to-be-added bounding box
 *   num_detected: number of already detected bounding boxes in dest_data
 *                 for this function to determine the index of this to-be-added
 *                 bbox in dest_data[i] after we append *nms_dec_box
 *      dest_data: AmbaCnn Extern ARM CVTask output destination buffer
 *                 [1]: for decoded bbox's x1, y1, x2, y2 (in float)
 *                 [2]: for decoded bbox's class ID (in uint32_t)
 *                 [3]: for decoded bbox's conf score (in float)
 *
 *        returns: void (fill the to-be-added bbox into @dest_data directly)
 */
void add_to_outputs(const nms_dex_bbx_t* nms_dec_box, uint32_t num_detected,
                    void* const* dest_data) {
    uint32_t nx4 = num_detected * 4U;

    /* x1, y1, x2, y2 */
    float* decbox = (float*) dest_data[1U];
    decbox[nx4] = nms_dec_box->x1;
    decbox[nx4 + 1U] = nms_dec_box->y1;
    decbox[nx4 + 2U] = nms_dec_box->x2;
    decbox[nx4 + 3U] = nms_dec_box->y2;

    /* class ID */
    uint32_t* clsId = (uint32_t*) dest_data[2U];
    clsId[num_detected] = nms_dec_box->cls;

    /* conf score */
    float* score = (float*) dest_data[3U];
    score[num_detected] = nms_dec_box->conf;
}

/*
 * Function: add_to_grouped_out
 * ----------------------------
 * Append a newly detected bounding box nms_dec_box into AmbaCnn Extern ARM CVT
 * output buffer dest_data which already had num_detected bounding boxes.
 * (dest_data is a single grouped output)
 *
 *    nms_dec_box: a nms_dex_bbx_t pointer to the to-be-added bounding box
 *   num_detected: number of already detected bounding boxes in dest_data
 *                 for this function to determine the index of this to-be-added
 *                 bbox in dest_data[i] after we append *nms_dec_box
 *      dest_data: AmbaCnn Extern ARM CVTask output destination buffer
 *                 [0][0:4] for decoded bbox's x1, y1, x2, y2 (in float)
 *                 [0][4]: for decoded bbox's conf score (in float)
 *                 [0][5]: for decoded bbox's class ID (in float)
 *
 *        returns: void (fill the to-be-added bbox into @dest_data directly)
 */
void add_to_grouped_out(const nms_dex_bbx_t* nms_dec_box, uint32_t num_detected,
                        void* const* dest_data) {
    uint32_t nx6 = num_detected * 6U;

    float* detections = (float*) dest_data[0U];

    /* x1, y1, x2, y2 */
    detections[nx6] = nms_dec_box->x1;
    detections[nx6 + 1U] = nms_dec_box->y1;
    detections[nx6 + 2U] = nms_dec_box->x2;
    detections[nx6 + 3U] = nms_dec_box->y2;

    /* conf score */
    detections[nx6 + 4U] = nms_dec_box->conf;

    /* class ID */
    detections[nx6 + 5U] = (float) nms_dec_box->cls;
}

/*******************************************************************************
 *
 *                  7. Example of Parsing `attr` String
 *
 ******************************************************************************/

/*
 * Function: attr_to_odhdlr
 * ------------------------
 * Parse attr string into fields of ODHdlr_s.
 *
 *   attr: a string int the form of
 *         "total_bbox num_cls obj_thres conf_thres overlap_thres max_num_bbx"
 *         (six numbers, separated by space)
 *   hdlr: pointer to ODHdlr_s whose fields we will set according to parsed
 *         attr nums
 *
 *   retruns: void (set the fields of *hdlr directly)
 */
void attr_to_odhdlr(const char* attr, ODHdlr_s* hdlr) {
    char attr_copy[MAX_ATTR_LEN];
    const char space[2] = " ";
    char* token;

    strcpy(attr_copy, attr);

    token = strtok(attr_copy, space);
    hdlr->total_bbox = atoi(token);

    token = strtok(NULL, space);
    hdlr->num_cls = atoi(token);

    token = strtok(NULL, space);
    hdlr->obj_thres = atof(token);

    token = strtok(NULL, space);
    hdlr->conf_thres = atof(token);

    token = strtok(NULL, space);
    hdlr->overlap_thres = atof(token);

    token = strtok(NULL, space);
    hdlr->max_num_bbx = atoi(token);
}

