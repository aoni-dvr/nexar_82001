/******************************************************************************/
/*  Copyright 2022 Ambarella Inc.                                             */
/*  Author: Tsun-Han Huang                                                    */
/*  Email:  thhuang@ambarella.com                                             */
/******************************************************************************/

#ifndef CVAPI_ARMCVT_NMS_H_FILE
#define CVAPI_ARMCVT_NMS_H_FILE

#include <stdbool.h>         /* for bool     */
#include <stdint.h>          /* for uint32_t */

#ifdef __cplusplus
extern "C" {
#endif

/* a bounding box node in doubly linked list */
typedef struct {
    float x1;
    float y1;
    float x2;
    float y2;

    uint32_t  cls;
    float     conf;
    uint32_t  prev_idx;
    uint32_t  next_idx;
} nms_dex_bbx_t;

/* wrap parameters in object detection handler */
typedef struct {
    bool active;
    float* prior_box;
    float* prior_var;
    uint32_t total_bbox;
    uint32_t num_cls;
    float obj_thres;
    float conf_thres;
    float overlap_thres;
    uint32_t max_num_bbx;
} ODHdlr_s;

/* helper func to get available ODHdlr_s in ODHdlr_s array with len = length */
inline uint32_t get_available_hdlr_id(ODHdlr_s* OD_Hdlr, uint32_t length) {
    for(uint32_t i = 0U; i < length; i++) {
        if(!OD_Hdlr[i].active) {
            return i;
        }
    }
    return 0U;
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
                  float (*calc_overlap)(const void*, const void*));

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
                         float (*calc_overlap)(const void*, const void*));

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
                            uint32_t* p_nms_num_dec, uint32_t* p_nms_max_idx);

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
                            uint32_t* p_nms_num_dec, uint32_t* p_nms_max_idx);

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
                        float* decbox);

/*
 * Function: decode_xywh_from_prior
 * --------------------------------
 * Caculate xywh from prior xywh and var and VP predicted loc
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
                            const float* loc, float* decbox);

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
void xyxy_to_xywh(const float* pbox, float* decbox);

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
void xywh_to_xyxy(const float* pbox, float* decbox);

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
void bbox_boundary_cap(float* decbox, float min, float max);

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
                            uint32_t before_max_idx);

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
float bbox_overlap(const void* bbox_1, const void* bbox_2);

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
float bbox_iou(const void* bbox_1, const void* bbox_2);

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
                    void* const* dest_data);

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
                        void* const* dest_data);

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
void attr_to_odhdlr(const char* attr, ODHdlr_s* hdlr);

#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif  /* CVAPI_ARMCVT_NMS_H_FILE */
