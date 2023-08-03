// inter over self
static uint32_t cal_ios(amba_od_candidate_t *objs, amba_od_candidate_t *objt)
{
    uint32_t inter_w_l = 0, inter_h_u = 0;
    uint32_t inter_w_r = 0, inter_h_b = 0;
    uint32_t inter_w = 0, inter_h = 0;
    uint32_t inter_size, size;
    
    if (objs->bb_start_row > objt->bb_start_row){
        inter_h_u = objs->bb_start_row;
    } else {
        inter_h_u = objt->bb_start_row;
    }
    if ((objs->bb_start_row+objs->bb_height_m1) > (objt->bb_start_row+objt->bb_height_m1)){
        inter_h_b = (objt->bb_start_row+objt->bb_height_m1);
    } else {
        inter_h_b = (objs->bb_start_row+objs->bb_height_m1);
    }
    if (inter_h_u<inter_h_b){
        inter_h = inter_h_b - inter_h_u;
    }

    if (objs->bb_start_col > objt->bb_start_col){
        inter_w_l = objs->bb_start_col;
    } else {
        inter_w_l = objt->bb_start_col;
    }
    if ((objs->bb_start_col+objs->bb_width_m1) > (objt->bb_start_col+objt->bb_width_m1)){
        inter_w_r = (objt->bb_start_col+objt->bb_width_m1);
    } else {
        inter_w_r = (objs->bb_start_col+objs->bb_width_m1);
    }
    if (inter_w_l<inter_w_r){
        inter_w = inter_w_r - inter_w_l;
    }

    inter_size = inter_w*inter_h;
    size = objs->bb_width_m1 * objs->bb_height_m1;
    return (inter_size*100)/(size+1);
}

static uint32_t if_bbx_at_boundary(amba_od_candidate_t *obj, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t th)
{
    uint32_t at_bdy = 0;

    if (obj->bb_start_col < (int32_t)(x+th)){
        at_bdy = 1;
    }
    if (obj->bb_start_row < (int32_t)(y+th)){
        at_bdy = 1;
    }
    if ((obj->bb_start_col+obj->bb_width_m1) > (x+w-th)){
        at_bdy = 1;
    }
    if ((obj->bb_start_row+obj->bb_height_m1) > (y+h-th)){
        at_bdy = 1;
    }
    return at_bdy;
}


/**
 * to supress double detection which cropped by small ROI
 * 
 * @param od_out [in,out] the output of amba od. the score of supressed object will be set to 0.
 * @param class [in] class of object to spress, if you need multi class, run multi times.
 * @param th [in] threshold of boundary check, use 32 as default
 * @param x [in] small ROI x
 * @param y [in] small ROI y
 * @param w [in] small ROI w
 * @param h [in] small ROI h
 */
void AmbaOD_FilterDoubleDetect(amba_od_out_t *od_out, uint32_t num_class, uint32_t *class, uint32_t th, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    amba_od_candidate_t *obj;
    uint32_t i, j, k;
    uint32_t num_obj;

    num_obj = od_out->num_objects;
    obj = (amba_od_candidate_t*)((uint32_t) od_out + (uint32_t)od_out->objects_offset);
    // go through all object
    for (i=0 ; i<num_obj ; i++){
        uint32_t class_match = 0;
        for (k=0 ; k<num_class ; k++){
            if (obj[i].class == class[k]){
                class_match = 1;
                break;
            }
        }
        if (class_match == 1){
            // check is on small ROI boundary
            if (if_bbx_at_boundary(&obj[i], x, y, w, h, th) == 1u){
                for (j=0 ; j<num_obj ; j++) {
                    if ((i != j) && (obj[j].class == obj[i].class)) {
                        uint32_t ios;
                        // check if IOS of other object > 85%
                        ios = cal_ios(&obj[i], &obj[j]);
                        if (ios > 85){
                            // set score to 0
                            obj[i].score = 0;
                        }
                    }
                }
            }
        }
    }
}