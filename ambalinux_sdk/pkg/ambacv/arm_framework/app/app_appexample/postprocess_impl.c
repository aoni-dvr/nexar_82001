#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>

#include "cvtask_ossrv.h"

#include "NMS.h"
#include "postprocess_impl.h"
#include "osdutil.h"
#include "mem_util.h"
#include "dag_util.h"
#include "cam_util.h"
#include "preference.h"

typedef struct {
    int osd_size;
    uint32_t osd_index;
    OSD_INFO_s osd_info;
    flexidag_memblk_t osdbuf[OSD_AMOUNT_MAX]; //multi-buffering
} OSD_PARAMS_s;

typedef struct {
    /* NMS */
    void *nms_hndlr;
    /* Class */
    flexidag_memblk_t pcls_list;
    flexidag_memblk_t cls_name_buf;
    const char cls_name[NUM_CLS][16];
} NMS_PARAMS_s;

typedef struct {
    /* FlexiDAG result */
    flexidag_memblk_t od_results;

    /* user preference */
    uint32_t dst_output_type;
} PROGRAM_PARAMS_s;

static OSD_PARAMS_s g_OSD[MAX_FLOW_AMOUNT] = {0};
static NMS_PARAMS_s g_NMS[MAX_FLOW_AMOUNT] = {
{
/* Specify the name of each class. */
/****
 * You have to change this depending on your NN flow.
 ****/
    .cls_name = {
        "id_0",
        "id_1",
        "id_2",
        "id_3",
        "id_4",
        "id_5",
        "id_6",
    },
}
};
static PROGRAM_PARAMS_s g_params[MAX_FLOW_AMOUNT] = {0};

static uint32_t SaveResult_Dag(uint32_t id, uint32_t frameId, AMBA_CV_FLEXIDAG_IO_s *result)
{
    int i;
    FILE *out;
    char out_fn[128] = {0};

    for(i = 0; i < result->num_of_buf; i++) {
        sprintf(out_fn, "./result_%u_%u_%u.bin", id, frameId, i);
        out = fopen(out_fn,"wb");
        if (out == NULL) {
            printf("%s[%u]: fail to open %s!\n", __func__, id, out_fn);
            return 10;
        }

        fwrite(result->buf[i].pBuffer, 1, result->buf[i].buffer_size, out);
        fclose(out);
    }

    return 0;
}

static uint32_t SaveResult_NMS(uint32_t id, uint32_t frameId, amba_od_out_t *od_results)
{
    uint32_t retcode = 0;
    amba_od_candidate_t *bbx = NULL;
    FILE *out;
    uint32_t i,x1,y1,x2,y2;
    ptr32_t *cls_list;

    /* Retrieve output */
    if (od_results == NULL) {
        fprintf(stderr, "od_results is NULL!!\n");
        return 1;
    } else {
        char out_fn[128];

        sprintf(out_fn, "./result_%u_%u.bin", id, frameId);
        out = fopen(out_fn, "wb");
        if (out == NULL) {
            printf("SaveResult_NMS[%u]: fail to open file [%s]!\n", id, out_fn);
            return 2;
        }
    }

    bbx = (amba_od_candidate_t *)((uint8_t *)od_results + od_results->objects_offset);
    //printf("od_results=%p, VisBoxNum=%u, pVisBox=%p\n", od_results, od_results->num_objects, bbx);
    cls_list = (ptr32_t *)ambacv_p2v(od_results->class_name);

    /* write header info */
    {
        char *cls_name;

        //Number of object(4bytes)| class name(N*16bytes)
        fwrite(&od_results->num_objects, 1, sizeof(uint32_t), out);
        for (i=0; i<NUM_CLS; i++) {
            cls_name = (char *)ambacv_p2v(cls_list[i]);
            fwrite(cls_name, 1, 16, out);
        }
    }

    /* write box */
    for (i=0; i<od_results->num_objects; i++) {
        fwrite(&bbx[i], 1, sizeof(amba_od_candidate_t),out);
    }
    fclose(out);

    return retcode;
}


/**
 * Generate OSD content.
 * This function will generate OSD buffer content and send it to RTOS for rendering.
 **/
static uint32_t DrawOsd(uint32_t id, uint32_t frameId, amba_od_out_t *od_results)
{
    uint32_t ret = 0, osd_idx;
    unsigned char *osd_buf;
    int *Hndlr = NULL;
    unsigned int Rval;
    amba_od_candidate_t *bbx = NULL;
    static struct timeval prevous_time = {0};

    /* Retrieve output */
    if (od_results != NULL) {
        bbx = (amba_od_candidate_t *)((uint8_t *)od_results + od_results->objects_offset);
        //printf("od_results=%p, VisBoxNum=%u, pVisBox=%p\n",
        //    od_results, od_results->num_objects, bbx);
    } else {
        fprintf(stderr, "od_results is NULL!!\n");
    }

#if 0
    /* check boundary */
    if (0) {
        if (od_results != NULL) {
            uint32_t i, X, Y, W, H;
            for (i=0; i<od_results->num_objects; i++) {
                X = bbx[i].bb_start_col;
                Y = bbx[i].bb_start_row;
                W = bbx[i].bb_width_m1+1;
                H = bbx[i].bb_height_m1+1;
                fprintf(stderr,"[%u] (%u,%u) w=%u h=%u, c=%u, s=%u\n", i, X, Y, W, H,
                    bbx[i].cls,bbx[i].score);
                if ((X+W) > g_OSD[id].osd_info.OsdWin.Width) {
                    fprintf(stderr,"over width limit\n",0,0,0,0,0);
                }
                if ((Y+H) > g_OSD[id].osd_info.OsdWin.Height) {
                    fprintf(stderr,"over hight limit\n",0,0,0,0,0);
                }
            }
        }
    }
#endif

    /* prepare OSD output */
    osd_idx = g_OSD[id].osd_index;
    osd_buf = (unsigned char *)g_OSD[id].osdbuf[osd_idx].pBuffer;

    Rval = OSD_Init(id, &Hndlr, g_OSD[id].osd_info.OsdWin.Width, g_OSD[id].osd_info.OsdWin.Height, osd_buf);
    if (Rval != 0) {
        printf("[DrawOsd] OSD_Init fail\n");
    } else {
        char Text[128] = {0};
        struct timeval current;

        if (od_results != NULL) {
            uint32_t i,x1,y1,x2,y2;
            //ptr32_t *cls_list;

            //cls_list = (ptr32_t *)ambacv_p2v(od_results->class_name);
            for (i=0; i<od_results->num_objects; i++) {
                //if (bbx[i].cls == 0xff) { //Don't show ROI window out.
                //    continue;
                //}
                x1 = bbx[i].bb_start_col;
                y1 = bbx[i].bb_start_row;
                x2 = x1 + bbx[i].bb_width_m1+1;
                y2 = y1 + bbx[i].bb_height_m1+1;
                if ((x2 > g_OSD[id].osd_info.OsdWin.Width) || (y2 > g_OSD[id].osd_info.OsdWin.Height)) {
                    printf("[DrawOsd] bbx[%u] out of boundary!\n", i);
                } else {
                    if (bbx[i].cls == 0xff) { //Roi window
                        OSD_DrawRect(Hndlr,x1,y1,x2,y2,2, NUM_CLS);
                    } else {
                        OSD_DrawRect(Hndlr,x1,y1,x2,y2,2, bbx[i].cls);
                        /* Draw class name */
                        //{
                        //    char *cls_name;
                        //    cls_name = (char *)ambacv_p2v(cls_list[bbx[i].cls]);
                        //    OSD_DrawString(Hndlr,x1,y1,cls_name,1, bbx[i].cls);
                        //}
                    }
                }
            }
        } else {
            sprintf(Text, "There is no valid od result!");
            OSD_DrawString(Hndlr,10,250,Text,2, NUM_CLS);
        }
        gettimeofday(&current, NULL);
        if((prevous_time.tv_sec != 0) || (prevous_time.tv_sec != 0)) {
            int diff, fps;

            diff = (current.tv_sec - prevous_time.tv_sec) * 1000000 + (current.tv_usec - prevous_time.tv_usec);
            fps = 1000000/diff;

            sprintf(Text, "FR:%d fps (%d us)", fps, diff);
            OSD_DrawString(Hndlr,20,520,Text,2,NUM_CLS);
        }

        prevous_time.tv_sec = current.tv_sec;
        prevous_time.tv_usec = current.tv_usec;

        OSD_Release(Hndlr);
    }
    {
        /* Send OSD buffer to RTOS */
        uint32_t osd_idx = g_OSD[id].osd_index;
        memio_sink_send_out_t out = {0};

        out.cvtask_frameset_id = frameId;
        out.num_of_io = 1;
        out.io[0].addr = g_OSD[id].osdbuf[osd_idx].buffer_daddr;
        out.io[0].size = g_OSD[id].osd_size;
        CamUtil_SendOSD(id, &out);

        //update active osd buffer index
        osd_idx++;
        if (osd_idx >= g_OSD[id].osd_info.OsdAmount) {
            osd_idx = 0;
        }
        g_OSD[id].osd_index = osd_idx;
    }
    return ret;
}

uint32_t PostProcess_Impl(uint32_t id, memio_source_recv_picinfo_t *in, AMBA_CV_FLEXIDAG_IO_s *result)
{
    uint32_t ret = 0U;

    amba_od_candidate_t *od_candidates;
    uint32_t box_ammount;
    amba_od_out_t *od_res = (amba_od_out_t *)(g_params[id].od_results.pBuffer);

    memset(od_res, 0, sizeof(amba_od_out_t));
    od_candidates = (amba_od_candidate_t *)(g_params[id].od_results.pBuffer + sizeof(amba_od_out_t));

    if (g_params[id].dst_output_type == DST_OUTPUT_TYPE_FILE_DAG) {
        ret = SaveResult_Dag(id, in->pic_info.frame_num, result);
        if (ret != 0) {
            printf("[PostProcess_Impl] SaveResult_Dag() failed!!\n");
        }
    } else {
        /* process output */
        ret = NMS_Process(id, g_NMS[id].nms_hndlr, (float *)result->buf[0].pBuffer,(float *)result->buf[1].pBuffer, od_candidates, &box_ammount);
        if (ret != 0) {
            printf("[PostProcess_Impl] fail to do postprocess()!!\n");
        } else {
            od_res->length = sizeof(amba_od_out_t);
            od_res->num_objects = box_ammount;
            od_res->objects_offset = (uint32_t)sizeof(amba_od_out_t);
            od_res->frame_num = in->pic_info.frame_num;
            od_res->capture_time = in->pic_info.capture_time;
            od_res->class_name = (ptr32_t) g_NMS[id].pcls_list.buffer_daddr;
            if (g_params[id].dst_output_type == DST_OUTPUT_TYPE_OSD) {
                ret = DrawOsd(id, in->pic_info.frame_num, od_res);
                if (ret != 0) {
                    printf("[PostProcess_Impl] DrawOsd() failed!!\n");
                }
            } else if (g_params[id].dst_output_type == DST_OUTPUT_TYPE_FILE_NMS) {
                ret = SaveResult_NMS(id, in->pic_info.frame_num, od_res);
                if (ret != 0) {
                    printf("[PostProcess_Impl] SaveResult_NMS() failed!!\n");
                }
            }
        }
    }

    return ret;
}

uint32_t PostProcess_Init(uint32_t id, char *fl_path)
{
    uint32_t ret = 0, i;
    char path[128] = {0};
    FLEXIDAG_INSTANCE_s *inst;
    uint32_t DstOutputType;

    Preference_GetDstOutputType(id, &DstOutputType);
    if (DstOutputType < DST_OUTPUT_TYPE_NUM) {
        g_params[id].dst_output_type = DstOutputType;
    }
    /* load PriorBox before all flow since it takes a while */
    snprintf(path, sizeof(path), "%s/mbox_priorbox.bin",fl_path);
    ret = NMS_LoadPriorBox(path, &g_NMS[id].nms_hndlr);
    if(ret != 0) {
        return ret;
    }
    /* prepare class name list */
    if (ret == 0) {
        ret = MemUtil_MemblkAlloc(CACHE_ALIGN(NUM_CLS * sizeof(ptr32_t)), &g_NMS[id].pcls_list);
        if (ret != 0) {
            printf("[PostProcess_Init] MemUtil_MemblkAlloc pcls_list fail. ret=%u\n", ret);
            ret = 1U;
        } else {
            ret = MemUtil_MemblkAlloc(CACHE_ALIGN(NUM_CLS * 16), &g_NMS[id].cls_name_buf);
            if (ret != 0) {
                printf("[PostProcess_Init] MemUtil_MemblkAlloc cls_name_buf fail. ret=%u\n", ret);
                ret = 1U;
            } else {
                char *pcls_name;
                ptr32_t *cls_slot;
                uint32_t offset;

                cls_slot = (ptr32_t *)g_NMS[id].pcls_list.pBuffer;
                for(i=0; i<NUM_CLS; i++) {
                    offset = 16*i;
                    pcls_name = (char *)(g_NMS[id].cls_name_buf.pBuffer + offset);
                    memcpy(pcls_name, g_NMS[id].cls_name[i], 16);
                    cls_slot[i] = (ptr32_t)(g_NMS[id].cls_name_buf.buffer_daddr + offset);
                }
            }
        }
    }
    /* prepare od_result */
    if (ret == 0) {
        ret = MemUtil_MemblkAlloc(CACHE_ALIGN(sizeof(amba_od_out_t)+(MAX_VIS_NUM*sizeof(amba_od_candidate_t))), &g_params[id].od_results);
        if (ret != 0) {
            printf("[PostProcess_Init] MemUtil_MemblkAlloc od_results fail. ret=%u\n", ret);
            ret = 1U;
        } else {

        }
    }
    /* create OSD resource */
    if ((ret == 0) && (g_params[id].dst_output_type == DST_OUTPUT_TYPE_OSD)) {
        Preference_GetOSDInfo(id, &g_OSD[id].osd_info);
        if (g_OSD[id].osd_info.OsdType == OSD_TYPE_8_BIT) {
            g_OSD[id].osd_size = g_OSD[id].osd_info.OsdWin.Width*g_OSD[id].osd_info.OsdWin.Height;
        } else if (g_OSD[id].osd_info.OsdType == OSD_TYPE_32_BIT) {
            g_OSD[id].osd_size = g_OSD[id].osd_info.OsdWin.Width*g_OSD[id].osd_info.OsdWin.Height*4U;
        } else {
            ret = 1U;
            printf("[PostProcess_Init] Invalid osd type : %d-bit!!\n", g_OSD[id].osd_info.OsdType);
        }
        for (i=0; i<g_OSD[id].osd_info.OsdAmount; i++) {
            ret = MemUtil_MemblkAlloc(CACHE_ALIGN(g_OSD[id].osd_size), &g_OSD[id].osdbuf[i]);
            if(ret != 0) {
                printf("[PostProcess_Init] Fail to create OSD[%u][%d] buffer!! ret=%u\n", id, i, ret);
                return 1U;
            }
        }
    }
    return ret;
}

uint32_t PostProcess_Release(uint32_t id)
{
    int i;
    uint32_t ret = 0;
    if (g_NMS[id].pcls_list.pBuffer != NULL) {
        if (MemUtil_MemblkFree(&g_NMS[id].pcls_list) != 0 ) {
            printf("[PostProcess_Release] MemUtil_MemblkFree pcls_list fail!\n");
            return 4;
        }
    }

    if (g_NMS[id].cls_name_buf.pBuffer != NULL) {
        if (MemUtil_MemblkFree(&g_NMS[id].cls_name_buf) != 0 ) {
            printf("[PostProcess_Release] MemUtil_MemblkFree cls_name_buf fail!\n");
            return 5;
        }
    }

    if (g_params[id].od_results.pBuffer != NULL) {
        if (MemUtil_MemblkFree(&g_params[id].od_results) != 0 ) {
            printf("[PostProcess_Release] MemUtil_MemblkFree od_results fail!\n");
            return 6;
        }
    }
    /* release OSD resource */
    if (g_params[id].dst_output_type == DST_OUTPUT_TYPE_OSD) {
        for (i=0; i<g_OSD[id].osd_info.OsdAmount; i++) {
            if (g_OSD[id].osdbuf[i].pBuffer != NULL) {
                (void)MemUtil_MemblkFree(&(g_OSD[id].osdbuf[i]));
            }
        }
    }
    NMS_Release(g_NMS[id].nms_hndlr);

    return ret;
}



