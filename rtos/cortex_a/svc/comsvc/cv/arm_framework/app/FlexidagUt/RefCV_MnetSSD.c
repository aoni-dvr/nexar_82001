/**
 *  @file RefCV_MnetSSD.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Implementation of MnetSSD Test
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaMMU.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "AmbaShell.h"
#include "cvapi_visutil.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_ambaod.h"
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"
#include "idsp_roi_msg.h"
#if defined(CONFIG_AMBALINK_BOOT_OS) || defined(CONFIG_LINUX)
#include "AmbaIPC_FlexidagIO.h"
#endif


#define MAX_THREAD_NUM              1U
#define MAX_GOLDEN_NUM              4U

#define MNETSSD_CVTASK_UUID             2
#define MNETSSD_HL_CVTASK_UUID          4
#define AMBAOD_MAX_BBX_NUM  200
static UINT8 frequency;
static UINT32 run_mode = REF_CV_RUN_MODE_RTOS;

typedef struct {
    amba_od_out_t Info;
    amba_od_candidate_t Bbx[AMBAOD_MAX_BBX_NUM];
} REF_CV_OD_s;

static VIS_UTIL_BBX_LIST_MSG_s mnet_golden[MAX_GOLDEN_NUM] = {0};
static REF_CV_RING_PARAMS_s mnet_thread_params[MAX_THREAD_NUM];
static flexidag_memblk_t file_regionY[MAX_GOLDEN_NUM];

static uint32_t idsp_pyramid_scale = 0U;
static uint32_t roi_start_col = 810U;
static uint32_t roi_start_row = 390U;
static uint32_t source_vin = 0U;

typedef struct {
    uint32_t raw_w;
    uint32_t raw_h;

    uint32_t roi_w;
    uint32_t roi_h;

    uint32_t roi_start_col;
    uint32_t roi_start_row;

    uint32_t net_in_w;
    uint32_t net_in_h;
} st_win_ctx_t;

static UINT32 roi_handling(const AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle, const cv_pic_info_t *idsp_pic_info)
{
    UINT32 retcode = ERRCODE_NONE;
    uint32_t roi_w_m1, roi_h_m1;
    st_win_ctx_t win_ctx = {0};
    uint16_t flow_id;
    AMBA_CV_FLEXIDAG_MSG_s msg;
    static uint32_t network_input_width = 300U;
    static uint32_t network_input_height = 300U;

    (void) fd_gen_handle;
    roi_w_m1 = idsp_pic_info->pyramid.half_octave[idsp_pyramid_scale].roi_width_m1;
    roi_h_m1 = idsp_pic_info->pyramid.half_octave[idsp_pyramid_scale].roi_height_m1;

    if (roi_start_col == 9999U) {
        roi_start_col = (roi_w_m1 + 1U - network_input_width) / 2U;
    } else if (roi_start_col > 0x7FFFFFFFU) {
        roi_start_col = 0U;
    } else if (roi_start_col > (roi_w_m1 + 1U - network_input_width)) {
        roi_start_col = (roi_w_m1 + 1U - network_input_width);
    } else {
        /* do nothing */
    }

    if (roi_start_row == 9999U) {
        roi_start_row = (roi_h_m1 + 1U - network_input_height) / 2U;
    } else if (roi_start_row > 0x7FFFFFFFU) {
        roi_start_row = 0U;
    } else if (roi_start_row > (roi_h_m1 + 1U - network_input_height)) {
        roi_start_row = (roi_h_m1 + 1U - network_input_height);
    } else {
        /* do nothing */
    }

    //AmbaPrint_PrintUInt5("roi_handling: pyramid_scale = %d, start_col = %d, start_row= %d\n",idsp_pyramid_scale,roi_start_col,roi_start_row, 0U, 0U);
    //AmbaPrint_PrintUInt5("roi_handling: roi_w_m1 = %d, roi_h_m1 = %d\n", roi_w_m1, roi_h_m1, 0U, 0U, 0U);

    win_ctx.raw_w = ((uint32_t)idsp_pic_info->pyramid.half_octave[0].roi_width_m1) + 1U;
    win_ctx.raw_h = ((uint32_t)idsp_pic_info->pyramid.half_octave[0].roi_height_m1) + 1U;
    win_ctx.roi_w = roi_w_m1 + 1U;
    win_ctx.roi_h = roi_h_m1 + 1U;

    win_ctx.roi_start_col    = roi_start_col;
    win_ctx.roi_start_row    = roi_start_row;
    win_ctx.net_in_w         = network_input_width;
    win_ctx.net_in_h         = network_input_height;

    retcode |= AmbaCV_FlexidagGetFlowIdByUUID(fd_gen_handle, MNETSSD_HL_CVTASK_UUID, &flow_id);
    msg.flow_id = flow_id;
    msg.vpMessage = &win_ctx;
    msg.length = sizeof(win_ctx);
    retcode |= AmbaCV_FlexidagSendMsg(fd_gen_handle,&msg);
    return retcode;
}

static UINT32 RefCVRingRunPicinfo(REF_CV_RING_HANDLE_s *handle, uint32_t frame_id, const cv_pic_info_t *in, AMBA_CV_FLEXIDAG_RUN_INFO_s *run_info)
{
    UINT32 ret = 0U, i;
    const AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    cv_pic_info_t *pDataIn;
    UINT32 U32DataIn;
    UINT32 index = frame_id%REF_CV_BUF_NUM;

    AmbaMisra_TypeCast(&pDataIn, &handle->in_buf[index].buf[0].pBuffer);
    ret |= AmbaWrap_memcpy(pDataIn, in, sizeof(cv_pic_info_t));
    if(ret != 0U) {
        AmbaPrint_PrintStr5("RefCVRingRunPicinfo : name = %s AmbaWrap_memcpy fail ", handle->name, NULL, NULL, NULL, NULL);
        ret = 1U;
    } else {
        U32DataIn = handle->in_buf[index].buf[0].buffer_daddr;
        for (i = 0U ; i < MAX_HALF_OCTAVES ; i++) {
            pDataIn->rpLumaLeft[i] = in->rpLumaLeft[i] - U32DataIn;
            pDataIn->rpLumaRight[i] = in->rpLumaRight[i] - U32DataIn;
            pDataIn->rpChromaLeft[i] = in->rpChromaLeft[i] - U32DataIn;
            pDataIn->rpChromaRight[i] = in->rpChromaRight[i] - U32DataIn;
        }
        ret = RefCV_MemblkClean(&handle->in_buf[index].buf[0]);
        if(ret != 0U) {
            AmbaPrint_PrintStr5("RefCVRingRunPicinfo : name = %s RefCV_MemblkClean fail ", handle->name, NULL, NULL, NULL, NULL);
            ret = 1U;
        } else {
            if( handle->out_buf[index].num_of_buf != fd_gen_handle->mem_req.flexidag_num_outputs ) {
                AmbaPrint_PrintUInt5("RefCVRingRunPicinfo : mem_req.num_outputs (0x%x) != out_buf.num_of_buf (0x%x)", fd_gen_handle->mem_req.flexidag_num_outputs, handle->out_buf[index].num_of_buf, 0U, 0U, 0U);
                ret = 1U;
            } else {
                for(i = 0U; i < handle->out_buf[index].num_of_buf; i++) {
                    if(fd_gen_handle->mem_req.flexidag_output_buffer_size[i] > handle->out_buf[index].buf[i].buffer_size) {
                        AmbaPrint_PrintUInt5("RefCVRingRunPicinfo : mem_req.output_size[%d] (0x%x) > out_buf (0x%x)", i, fd_gen_handle->mem_req.flexidag_output_buffer_size[i], handle->out_buf[index].buf[i].buffer_size, 0U, 0U);
                        ret = 1U;
                    }

                    if (ret != 0U) {
                        break;
                    }
                }

                if (ret == 0U) {
                    ret |= roi_handling(fd_gen_handle, pDataIn);
                    ret |= AmbaCV_FlexidagRun(fd_gen_handle, &handle->in_buf[index], &handle->out_buf[index], run_info);
                    if (ret != 0U) {
                        AmbaPrint_PrintStr5("RefCVRingRunPicinfo : name = %s AmbaCV_FlexidagRun fail ", handle->name, NULL, NULL, NULL, NULL);
                        ret = 1U;
                    }
                }
            }
        }
    }
    return ret;
}

static UINT32 RefCVRingRunNonBlockingPicinfo(REF_CV_RING_HANDLE_s *handle, uint32_t frame_id, flexidag_cb cb, void *cb_param, const cv_pic_info_t *in, UINT32 *token_id)
{
    UINT32 ret = 0U, i;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    cv_pic_info_t *pDataIn;
    UINT32 U32DataIn;

    UINT32 index = frame_id%REF_CV_BUF_NUM;

    AmbaMisra_TypeCast(&pDataIn, &handle->in_buf[index].buf[0].pBuffer);
    ret = AmbaWrap_memcpy(pDataIn, in, sizeof(cv_pic_info_t));
    if(ret != 0U) {
        AmbaPrint_PrintStr5("RefCVRingRunNonBlockingPicinfo : name = %s AmbaWrap_memcpy fail ", handle->name, NULL, NULL, NULL, NULL);
        ret = 1U;
    } else {
        U32DataIn = handle->in_buf[index].buf[0].buffer_daddr;
        for (i = 0U ; i < MAX_HALF_OCTAVES ; i++) {
            pDataIn->rpLumaLeft[i] = in->rpLumaLeft[i] - U32DataIn;
            pDataIn->rpLumaRight[i] = in->rpLumaRight[i] - U32DataIn;
            pDataIn->rpChromaLeft[i] = in->rpChromaLeft[i] - U32DataIn;
            pDataIn->rpChromaRight[i] = in->rpChromaRight[i] - U32DataIn;
        }
        ret = RefCV_MemblkClean(&handle->in_buf[index].buf[0]);
        if(ret != 0U) {
            AmbaPrint_PrintStr5("RefCVRingRunNonBlockingPicinfo : name = %s RefCV_MemblkClean fail ", handle->name, NULL, NULL, NULL, NULL);
            ret = 1U;
        } else {
            if( handle->out_buf[index].num_of_buf != fd_gen_handle->mem_req.flexidag_num_outputs ) {
                AmbaPrint_PrintUInt5("RefCVRingRunNonBlockingPicinfo : mem_req.num_outputs (0x%x) != out_buf.num_of_buf (0x%x)", fd_gen_handle->mem_req.flexidag_num_outputs, handle->out_buf[index].num_of_buf, 0U, 0U, 0U);
                ret = 1U;
            } else {
                for(i = 0U; i < handle->out_buf[index].num_of_buf; i++) {
                    if(fd_gen_handle->mem_req.flexidag_output_buffer_size[i] > handle->out_buf[index].buf[i].buffer_size) {
                        AmbaPrint_PrintUInt5("RefCVRingRunNonBlockingPicinfo : mem_req.output_size[%d] (0x%x) > out_buf (0x%x)", i, fd_gen_handle->mem_req.flexidag_output_buffer_size[i], handle->out_buf[index].buf[i].buffer_size, 0U, 0U);
                        ret = 1U;
                    }

                    if (ret != 0U) {
                        break;
                    }
                }

                if (ret == 0U) {
                    ret |= roi_handling(fd_gen_handle, pDataIn);
                    ret |= AmbaCV_FlexidagRunNonBlock(fd_gen_handle, cb, cb_param, &handle->in_buf[index], &handle->out_buf[index], token_id);
                    if (ret != 0U) {
                        AmbaPrint_PrintStr5("RefCVRingRunNonBlockingPicinfo : name = %s AmbaCV_FlexidagRun fail ", handle->name, NULL, NULL, NULL, NULL);
                        ret = 1U;
                    }
                }
            }
        }
    }
    return ret;
}

static UINT32 RefCVPincinfoResult(UINT32 id, UINT32 frame, const flexidag_memblk_t *pBlk)
{
    UINT32 i, Rval = 0U,value = 0U, len = 0U;
    const REF_CV_OD_s *pAmbaOD;
    VIS_UTIL_BBX_LIST_MSG_s BBX = {0};
    char print_buf[256];
    UINT32 index = frame%MAX_GOLDEN_NUM ;

    AmbaMisra_TypeCast(&pAmbaOD, &pBlk->pBuffer);
    BBX.NumBbx = pAmbaOD->Info.num_objects;
    for (i = 0U ; i < pAmbaOD->Info.num_objects; i++) {
        BBX.Bbx[i].X = (UINT16)pAmbaOD->Bbx[i].bb_start_col;
        BBX.Bbx[i].Y = (UINT16)pAmbaOD->Bbx[i].bb_start_row;
        BBX.Bbx[i].W = (UINT16)(pAmbaOD->Bbx[i].bb_width_m1 + 1U);
        BBX.Bbx[i].H = (UINT16)(pAmbaOD->Bbx[i].bb_height_m1 + 1U);
        BBX.Bbx[i].Cat = (UINT16)pAmbaOD->Bbx[i].clsId;
        BBX.Bbx[i].Score = pAmbaOD->Bbx[i].score;
    }

    if(mnet_golden[index].NumBbx != BBX.NumBbx) {
        Rval = 1U;
    }
    len = RefCVSprintfUint3(&print_buf[value], sizeof(print_buf), "%d frame (%d) error : Result (%d) ",id, frame,(UINT32)BBX.NumBbx);
    value = value + len;
    for (i = 0U ; i<BBX.NumBbx ; i++) {
        len = RefCVSprintfUint5(&print_buf[value], sizeof(print_buf),"(%d, %d)-(%d,%d) c:%d ", \
                                ((UINT32)BBX.Bbx[i].X)\
                                , ((UINT32)BBX.Bbx[i].Y)\
                                , ((UINT32)BBX.Bbx[i].X + (UINT32)BBX.Bbx[i].W)\
                                , ((UINT32)BBX.Bbx[i].Y + (UINT32)BBX.Bbx[i].H)\
                                , ((UINT32)BBX.Bbx[i].Cat));
        value = value + len;
        if( (mnet_golden[index].Bbx[i].X != BBX.Bbx[i].X) || (mnet_golden[index].Bbx[i].Y != BBX.Bbx[i].Y)
            ||(mnet_golden[index].Bbx[i].W != BBX.Bbx[i].W) || (mnet_golden[index].Bbx[i].H != BBX.Bbx[i].H)) {
            Rval = 1U;
        }
    }
    if(Rval == 1U) {
        AmbaPrint_PrintStr5("%s ",print_buf, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static void RefCVMnetSSDRunCallback(void *vpHandle, uint32_t flexidag_output_num, flexidag_memblk_t *pblk_Output, void *vpParameter)
{
    REF_CV_RING_PARAMS_s  *param;
    UINT32 ret = 0U;

    (void) vpHandle;
    (void) flexidag_output_num;
    (void) pblk_Output;
    (void) vpParameter;
    AmbaMisra_TouchUnused(vpHandle);
    AmbaMisra_TouchUnused(pblk_Output);
    AmbaMisra_TouchUnused(vpParameter);
    AmbaMisra_TypeCast(&param, &vpParameter);
    //AmbaPrint_PrintUInt5("RefCVMnetSSDRunCallback (%d) flexidag_output_num (%d) ",param->id, flexidag_output_num, 0U, 0U, 0U);
    if(flexidag_output_num == 1U) {
        ret = AmbaKAL_MsgQueueSend(&param->msg_queue, &flexidag_output_num, AMBA_KAL_NO_WAIT);
    }
    (void) ret;
}

static UINT32 RefCVMnetSSDLoadFile(REF_CV_RING_PARAMS_s  *param)
{
    UINT32 ret = 0U,size_align = 0U,i,pos,tmp_pos;
#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100)
    const char data_path[128] = "d:\\flexidag_mnetssd_sensor/golden/";
#else
    const char data_path[128] = "c:\\flexidag_mnetssd_sensor/golden/";
#endif
    char file_nameY[128];

    // load input Y file
    for(i = 0U; i < MAX_GOLDEN_NUM; i ++) {
        pos = 0U;
        tmp_pos = RefCVSprintfStr(&file_nameY[pos], data_path);
        pos = pos + tmp_pos;
        tmp_pos = RefCVSprintfUint1(&file_nameY[pos], sizeof(file_nameY), "%d.yuv", i);
        pos = pos + tmp_pos;
        (void) pos;
        if(ret == 0U) {
            ret = AmbaCV_UtilityFileSize(file_nameY, &size_align);
        }

        if(ret == 0U) {
            if(file_regionY[i].pBuffer == NULL) {
                ret = RefCV_MemblkAlloc(size_align, &file_regionY[i]);
            }
        } else {
            AmbaPrint_PrintUInt5("RefCVMnetSSDLoadFile (%d) AmbaCV_UtilityFileSize input Y fail ",param->id, 0U, 0U, 0U, 0U);
        }

        if(ret == 0U) {
            ret = AmbaCV_UtilityFileLoad(file_nameY, &file_regionY[i]);
        } else {
            AmbaPrint_PrintUInt5("RefCVMnetSSDLoadFile (%d) RefCV_MemblkAlloc input Y fail ",param->id, 0U, 0U, 0U, 0U);
        }

        if(ret != 0U) {
            AmbaPrint_PrintUInt5("RefCVMnetSSDLoadFile (%d) AmbaCV_UtilityFileLoad input Y fail ",param->id, 0U, 0U, 0U, 0U);
        }
    }

    // load bin file
    if(run_mode == REF_CV_RUN_MODE_RTOS) {
        if(ret == 0U) {
            ret = AmbaCV_UtilityFileSize(param->flexidag_path, &size_align);
        }

        if(ret == 0U) {
            if(param->handle.bin_buf.pBuffer == NULL) {
                ret = RefCV_MemblkAlloc(size_align, &param->handle.bin_buf);
            }
        } else {
            AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) AmbaCV_UtilityFileSize bin fail ",param->id, 0U, 0U, 0U, 0U);
        }

        if(ret == 0U) {
            ret = AmbaCV_UtilityFileLoad(param->flexidag_path, &param->handle.bin_buf);
        } else {
            AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) RefCV_MemblkAlloc bin fail ",param->id, 0U, 0U, 0U, 0U);
        }

        if(ret != 0U) {
            AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) AmbaCV_UtilityFileLoad bin fail ",param->id, 0U, 0U, 0U, 0U);
        }
    }

    return ret;
}

static uint32_t RefCVMnetSSDAllocBuf(UINT32 id,REF_CV_RING_HANDLE_s *handle)
{
    UINT32 ret = 0U,i;
    static flexidag_memblk_t g_temp_buf;

    //state buffer
    if(handle->init.state_buf.pBuffer == NULL) {
        ret = RefCV_MemblkAlloc(handle->fd_gen_handle.mem_req.flexidag_state_buffer_size, &handle->init.state_buf);
    }

    if(ret != 0U) {
        AmbaPrint_PrintUInt5("RefCVBisenetAllocBuf (%d) state buffer alloc fail ",id, 0U, 0U, 0U, 0U);
    }

    //temp buffer
    if(ret == 0U) {
        if((g_temp_buf.pBuffer == NULL) && (handle->fd_gen_handle.mem_req.flexidag_temp_buffer_size != 0U)) {
            ret |= RefCV_MemblkAlloc(handle->fd_gen_handle.mem_req.flexidag_temp_buffer_size, &g_temp_buf);
        }
        ret |= AmbaWrap_memcpy(&handle->init.temp_buf, &g_temp_buf, sizeof(g_temp_buf));
    }
    if(ret != 0U) {
        AmbaPrint_PrintUInt5("RefCVBisenetAllocBuf (%d) temp buffer alloc fail ",id, 0U, 0U, 0U, 0U);
    }

    for (i = 0; i < REF_CV_BUF_NUM; i++) {
        //input buffer
        handle->in_buf[i].num_of_buf = 1U;
        if(handle->in_buf[i].buf[0].pBuffer == NULL) {
            ret = RefCV_MemblkAlloc(sizeof(memio_source_recv_picinfo_t), &handle->in_buf[i].buf[0]);
        }
        if(ret != 0U) {
            AmbaPrint_PrintUInt5("RefCVBisenetAllocBuf (%d) in_buf buffer (%d) (%d) alloc fail ",id, i, 0U, 0U, 0U);
        }

        //output buffer
        handle->out_buf[i].num_of_buf = 2U;
        if(handle->out_buf[i].buf[0].pBuffer == NULL) {
            ret = RefCV_MemblkAlloc(handle->fd_gen_handle.mem_req.flexidag_output_buffer_size[0], &handle->out_buf[i].buf[0]);
        }
        if(ret != 0U) {
            AmbaPrint_PrintUInt5("RefCVBisenetAllocBuf (%d) out_buf buffer (%d) (%d) alloc fail ",id, i, 0U, 0U, 0U);
        }

        handle->out_buf[i].buf[1].pBuffer = NULL;   //dependency_only
        handle->out_buf[i].buf[1].buffer_daddr = 0U;
        handle->out_buf[i].buf[1].buffer_size = 0U;
    }

    return ret;
}

static void* RefCVFileInPicinfo(void *arg)
{
    UINT32 i,j,ret,ret1 = 0U;
    UINT32 YuvWidth = 1920U;
    UINT32 YuvHeight = 1080U;
    memio_source_recv_picinfo_t memio_pic_info = {0};
    cv_pic_info_t *pic_info = &memio_pic_info.pic_info;
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info;
    REF_CV_RING_PARAMS_s  *param;
    char log_path[128];
    UINT32 buf_index;
    UINT16 flow_id;
    AMBA_CV_FLEXIDAG_LOG_MSG_s set;
    const AMBA_CV_FLEXIDAG_LOG_MSG_s *pset;
    const void *param_set;
    AMBA_CV_FLEXIDAG_MSG_s msg;
    amba_roi_config_t roi_cfg;

    (void) arg;
    AmbaMisra_TypeCast(&param, &arg);
    param->RunFlag  = 1U;
    AmbaPrint_PrintUInt5("RefCVFileInPicinfo (%d) start ",param->id, 0U, 0U, 0U, 0U);

    ret = RefCVMnetSSDLoadFile(param);
    if (ret != 0U) {
        AmbaPrint_PrintStr5("RefCVFileInPicinfo : name = %s AmbaCV_UtilityFileLoad fail path = %s", param->handle.name, param->flexidag_path, NULL, NULL, NULL);
    } else {
        set.flexidag_msg_entry = 32U;
        set.cvtask_msg_entry = 32U;
        set.arm_cpu_map = 0xFU;
        set.arm_log_entry = 2048U;
        set.orc_log_entry = 2048U;
        pset = &set;
        AmbaMisra_TypeCast(&param_set, &pset);
        ret |= AmbaCV_FlexidagSetParamSet(&param->handle.fd_gen_handle, FLEXIDAG_PARAMSET_LOG_MSG, param_set, sizeof(AMBA_CV_FLEXIDAG_LOG_MSG_s));
        ret |= AmbaCV_FlexidagOpen(&param->handle.bin_buf, &param->handle.fd_gen_handle);
        if(ret != 0U) {
            AmbaPrint_PrintUInt5("RefCVFileInPicinfo (%d) AmbaCV_FlexidagOpen fail ",param->id, 0U, 0U, 0U, 0U);
        } else {
            ret |= RefCVMnetSSDAllocBuf(param->id, &param->handle);
            if(ret != 0U) {
                AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) RefCVBisenetAllocBuf fail ",param->id, 0U, 0U, 0U, 0U);
            } else {
                AmbaPrint_PrintUInt5("mnet_thread_params[%d].bin_buf pa         = 0x%x size = 0x%x",param->id,param->handle.bin_buf.buffer_daddr, param->handle.bin_buf.buffer_size, 0U, 0U);
                AmbaPrint_PrintUInt5("mnet_thread_params[%d].state_buf pa       = 0x%x size = 0x%x",param->id,param->handle.init.state_buf.buffer_daddr, param->handle.init.state_buf.buffer_size, 0U, 0U);
                AmbaPrint_PrintUInt5("mnet_thread_params[%d].temp_buf pa        = 0x%x size = 0x%x",param->id,param->handle.init.temp_buf.buffer_daddr, param->handle.init.temp_buf.buffer_size, 0U, 0U);
                for(j = 0U; j < REF_CV_BUF_NUM; j ++) {
                    AmbaPrint_PrintUInt5("mnet_thread_params[%d].in_buf pa[%d]     = 0x%x size = 0x%x",param->id,j,param->handle.in_buf[j].buf[0].buffer_daddr, param->handle.in_buf[j].buf[0].buffer_size, 0U);
                    AmbaPrint_PrintUInt5("mnet_thread_params[%d].out_buf pa[%d]     = 0x%x size = 0x%x",param->id,j,param->handle.out_buf[j].buf[0].buffer_daddr, param->handle.out_buf[j].buf[0].buffer_size, 0U);
                }
                ret |= AmbaCV_FlexidagInit(&param->handle.fd_gen_handle, &param->handle.init);
                if(ret != 0U) {
                    AmbaPrint_PrintUInt5("RefCVFileInPicinfo (%d) AmbaCV_FlexidagInit fail ",param->id, 0U, 0U, 0U, 0U);
                } else {
                    pic_info->capture_time = 0U;
                    pic_info->channel_id = 0U;
                    pic_info->frame_num = 0U;
                    pic_info->pyramid.image_width_m1 = (UINT16)(YuvWidth - 1U);
                    pic_info->pyramid.image_height_m1 = (UINT16)(YuvHeight - 1U);
                    pic_info->pyramid.image_pitch_m1 = (YuvWidth - 1U);
                    pic_info->pyramid.half_octave[0].ctrl.roi_pitch = (UINT16)YuvWidth;
                    pic_info->pyramid.half_octave[0].roi_start_col = (INT16)0;
                    pic_info->pyramid.half_octave[0].roi_start_row = (INT16)0;
                    pic_info->pyramid.half_octave[0].roi_width_m1 = (UINT16)(YuvWidth - 1U);
                    pic_info->pyramid.half_octave[0].roi_height_m1 = (UINT16)(YuvHeight - 1U);

                    pic_info->pyramid.half_octave[1].ctrl.disable = 1U;
                    pic_info->pyramid.half_octave[2].ctrl.disable = 1U;
                    pic_info->pyramid.half_octave[3].ctrl.disable = 1U;
                    pic_info->pyramid.half_octave[4].ctrl.disable = 1U;
                    pic_info->pyramid.half_octave[5].ctrl.disable = 1U;

                    ret |= AmbaCV_FlexidagGetFlowIdByUUID(&param->handle.fd_gen_handle, MNETSSD_CVTASK_UUID, &flow_id);
                    roi_cfg.msg_type = AMBA_ROI_CONFIG_MSG;
                    roi_cfg.image_pyramid_index = idsp_pyramid_scale;
                    roi_cfg.source_vin = source_vin;
                    roi_cfg.roi_start_col = roi_start_col;
                    roi_cfg.roi_start_row = roi_start_row;

                    msg.flow_id = flow_id;
                    msg.vpMessage = &roi_cfg;
                    msg.length = sizeof(roi_cfg);
                    ret |= AmbaCV_FlexidagSendMsg(&param->handle.fd_gen_handle,&msg);
                    for (i = 0U; i < param->num_runs; i++) {

                        pic_info->rpLumaLeft[0] = file_regionY[(i%MAX_GOLDEN_NUM)].buffer_daddr;
                        pic_info->rpChromaLeft[0] = file_regionY[(i%MAX_GOLDEN_NUM)].buffer_daddr + (YuvWidth * YuvHeight);
                        pic_info->rpLumaRight[0] = pic_info->rpLumaLeft[0];
                        pic_info->rpChromaRight[0] = pic_info->rpChromaLeft[0];


                        //AmbaPrint_PrintUInt5("RefCVFileInPicinfo (%d): RefCVRingRunPicinfo run %d ",param->id, i, 0U, 0U, 0U);
                        ret |= AmbaWrap_memset(&run_info,0,sizeof(run_info));
                        ret |= RefCVRingRunPicinfo(&param->handle, i, pic_info, &run_info);

                        if (ret != 0U) {
                            AmbaPrint_PrintUInt5("RefCVFileInPicinfo (%d) RefCVRingRunPicinfo %d fail ",param->id, i, 0U, 0U, 0U);
                            break;
                        }
                        if(run_info.output_not_generated == 0U) {
                            buf_index = i%REF_CV_BUF_NUM;
                            AmbaPrint_PrintUInt5("RefCVFileInPicinfo (%d) : frame_id = %d cv_run_time = %d us, over_head = %d us",param->id, i, run_info.end_time - run_info.start_time, ((run_info.api_end_time - run_info.api_start_time) - (run_info.end_time - run_info.start_time)), 0U);
                            ret = RefCVPincinfoResult(param->id, i, &param->handle.out_buf[buf_index].buf[0]);
                            if(ret != 0U) {
                                param->num_err = param->num_err + 1U;
                            }
                            param->cur_runs = param->cur_runs + 1U;
                            ret |= AmbaWrap_memset(param->handle.out_buf[buf_index].buf[0].pBuffer, 0x0, param->handle.out_buf[buf_index].buf[0].buffer_size);
                            ret |= RefCV_MemblkClean(&param->handle.out_buf[buf_index].buf[0]);
                            ret |= AmbaKAL_TaskSleep(param->sleep_usec);
                        }
                    }
#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100)
                    ret1 |= RefCVSprintfUint1(&log_path[0], sizeof(log_path), "d:\\flexidag_mnetssd_sensor/flexidag_log%d.txt", param->id);
#else
                    ret1 |= RefCVSprintfUint1(&log_path[0], sizeof(log_path), "c:\\flexidag_mnetssd_sensor/flexidag_log%d.txt", param->id);
#endif
                    ret |= AmbaCV_FlexidagDumpLog(&param->handle.fd_gen_handle,log_path, FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK);
                    if (ret != 0U) {
                        AmbaPrint_PrintUInt5("RefCVFileInPicinfo (%d) AmbaCV_FlexidagDumpLog fail ",param->id, 0U, 0U, 0U, 0U);
                    }

                    ret |= AmbaCV_FlexidagClose(&param->handle.fd_gen_handle);
                    if (ret != 0U) {
                        AmbaPrint_PrintUInt5("RefCVFileInPicinfo (%d) AmbaCV_FlexidagClose fail ",param->id, 0U, 0U, 0U, 0U);
                    }
                }
            }
        }
    }

    param->RunFlag = 0U;
    while(param->RunFlag != 2U) {
        ret |= AmbaKAL_TaskSleep(100U);
    }
    (void) ret;
    (void) ret1;
    return NULL;
}

static void* RefCVFileInPicinfoResult(void *arg)
{
    UINT32 ret = 0U, ret1 = 0U, buf_index;
    REF_CV_RING_PARAMS_s  *param;
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info;
    REF_CV_FRAME_s frame;

    (void) arg;
    AmbaMisra_TypeCast(&param, &arg);
    while(param->RunFlag != 3U) {
        ret1 |= AmbaKAL_MsgQueueReceive(&param->result_msg_queue, &frame, AMBA_KAL_WAIT_FOREVER);
        if(frame.frame_id == 0xFFFFFFFFU) {
            break;
        } else {
            ret1 |= AmbaWrap_memset(&run_info,0,sizeof(run_info));
            ret1 |= AmbaCV_FlexidagWaitRunFinish(&param->handle.fd_gen_handle, frame.token_id, &run_info);

            if(run_info.output_not_generated == 0U) {
                AmbaPrint_PrintUInt5("RefCVFileInPicinfoResult (%d) frame_id = %d cv_run_time = %d us, over_head = %d us",param->id, frame.frame_id, run_info.end_time - run_info.start_time, ((run_info.api_end_time - run_info.api_start_time) - (run_info.end_time - run_info.start_time)), 0U);
                buf_index = frame.frame_id%REF_CV_BUF_NUM;
                ret = RefCVPincinfoResult(param->id,frame.frame_id, &param->handle.out_buf[buf_index].buf[0]);
                if(ret != 0U) {
                    param->num_err = param->num_err + 1U;
                }
                param->cur_runs = param->cur_runs + 1U;
                ret1 |= AmbaWrap_memset(param->handle.out_buf[buf_index].buf[0].pBuffer, 0x0, param->handle.out_buf[buf_index].buf[0].buffer_size);
                ret1 |= RefCV_MemblkClean(&param->handle.out_buf[buf_index].buf[0]);
            }
        }
        (void) ret1;
    }

    return NULL;
}

static void* RefCVFileInPicinfoNonBlocking(void *arg)
{
    UINT32 i,j,ret = 0U,ret1 = 0U;
    UINT32 YuvWidth = 1920U;
    UINT32 YuvHeight = 1080U;
    memio_source_recv_picinfo_t memio_pic_info = {0};
    cv_pic_info_t *pic_info = &memio_pic_info.pic_info;
    REF_CV_RING_PARAMS_s  *param;
    char log_path[128];
    UINT32 token_id;
    void *cb_param;
    UINT16 flow_id;
    AMBA_CV_FLEXIDAG_LOG_MSG_s set;
    const AMBA_CV_FLEXIDAG_LOG_MSG_s *pset;
    const void *param_set;
    REF_CV_FRAME_s frame;
    AMBA_CV_FLEXIDAG_MSG_s msg;
    amba_roi_config_t roi_cfg;

    (void) arg;
    AmbaMisra_TypeCast(&param, &arg);
    param->RunFlag  = 1U;
    AmbaPrint_PrintUInt5("RefCVFileInPicinfoNonBlocking (%d) start ",param->id, 0U, 0U, 0U, 0U);

    ret = RefCVMnetSSDLoadFile(param);
    if (ret != 0U) {
        AmbaPrint_PrintStr5("RefCVFileInPicinfoNonBlocking : name = %s AmbaCV_UtilityFileLoad fail path = %s", param->handle.name, param->flexidag_path, NULL, NULL, NULL);
    } else {
        set.flexidag_msg_entry = 32U;
        set.cvtask_msg_entry = 32U;
        set.arm_cpu_map = 0xFU;
        set.arm_log_entry = 2048U;
        set.orc_log_entry = 2048U;
        pset = &set;
        AmbaMisra_TypeCast(&param_set, &pset);
        ret1 |= AmbaCV_FlexidagSetParamSet(&param->handle.fd_gen_handle, FLEXIDAG_PARAMSET_LOG_MSG, param_set, sizeof(AMBA_CV_FLEXIDAG_LOG_MSG_s));
        ret = AmbaCV_FlexidagOpen(&param->handle.bin_buf, &param->handle.fd_gen_handle);
        if(ret != 0U) {
            AmbaPrint_PrintUInt5("RefCVFileInPicinfoNonBlocking (%d) AmbaCV_FlexidagOpen fail ",param->id, 0U, 0U, 0U, 0U);
        } else {
            ret = RefCVMnetSSDAllocBuf(param->id, &param->handle);
            if(ret != 0U) {
                AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) RefCVBisenetAllocBuf fail ",param->id, 0U, 0U, 0U, 0U);
            } else {
                AmbaPrint_PrintUInt5("mnet_thread_params[%d].bin_buf pa         = 0x%x size = 0x%x",param->id,param->handle.bin_buf.buffer_daddr, param->handle.bin_buf.buffer_size, 0U, 0U);
                AmbaPrint_PrintUInt5("mnet_thread_params[%d].state_buf pa       = 0x%x size = 0x%x",param->id,param->handle.init.state_buf.buffer_daddr, param->handle.init.state_buf.buffer_size, 0U, 0U);
                AmbaPrint_PrintUInt5("mnet_thread_params[%d].temp_buf pa        = 0x%x size = 0x%x",param->id,param->handle.init.temp_buf.buffer_daddr, param->handle.init.temp_buf.buffer_size, 0U, 0U);
                for(j = 0U; j < REF_CV_BUF_NUM; j ++) {
                    AmbaPrint_PrintUInt5("mnet_thread_params[%d].in_buf pa[%d]     = 0x%x size = 0x%x",param->id,j,param->handle.in_buf[j].buf[0].buffer_daddr, param->handle.in_buf[j].buf[0].buffer_size, 0U);
                    AmbaPrint_PrintUInt5("mnet_thread_params[%d].out_buf pa[%d]     = 0x%x size = 0x%x",param->id,j,param->handle.out_buf[j].buf[0].buffer_daddr, param->handle.out_buf[j].buf[0].buffer_size, 0U);
                }
                ret = AmbaCV_FlexidagInit(&param->handle.fd_gen_handle, &param->handle.init);
                if(ret != 0U) {
                    AmbaPrint_PrintUInt5("RefCVFileInPicinfoNonBlocking (%d) AmbaCV_FlexidagInit fail ",param->id, 0U, 0U, 0U, 0U);
                } else {
                    pic_info->capture_time = 0U;
                    pic_info->channel_id = 0U;
                    pic_info->frame_num = 0U;
                    pic_info->pyramid.image_width_m1 = (UINT16)(YuvWidth - 1U);
                    pic_info->pyramid.image_height_m1 = (UINT16)(YuvHeight - 1U);
                    pic_info->pyramid.image_pitch_m1 = (YuvWidth - 1U);
                    pic_info->pyramid.half_octave[0].ctrl.roi_pitch = (UINT16)YuvWidth;
                    pic_info->pyramid.half_octave[0].roi_start_col = (INT16)0;
                    pic_info->pyramid.half_octave[0].roi_start_row = (INT16)0;
                    pic_info->pyramid.half_octave[0].roi_width_m1 = (UINT16)(YuvWidth - 1U);
                    pic_info->pyramid.half_octave[0].roi_height_m1 = (UINT16)(YuvHeight - 1U);

                    pic_info->pyramid.half_octave[1].ctrl.disable = 1U;
                    pic_info->pyramid.half_octave[2].ctrl.disable = 1U;
                    pic_info->pyramid.half_octave[3].ctrl.disable = 1U;
                    pic_info->pyramid.half_octave[4].ctrl.disable = 1U;
                    pic_info->pyramid.half_octave[5].ctrl.disable = 1U;

                    ret1 |= AmbaCV_FlexidagGetFlowIdByUUID(&param->handle.fd_gen_handle, MNETSSD_CVTASK_UUID, &flow_id);
                    AmbaPrint_PrintUInt5("RefCVFileInPicinfoNonBlocking (%d): AmbaCV_FlexidagSetFrequency %d ",param->id, frequency, 0U, 0U, 0U);

                    roi_cfg.msg_type = AMBA_ROI_CONFIG_MSG;
                    roi_cfg.image_pyramid_index = idsp_pyramid_scale;
                    roi_cfg.source_vin = source_vin;
                    roi_cfg.roi_start_col = roi_start_col;
                    roi_cfg.roi_start_row = roi_start_row;

                    msg.flow_id = flow_id;
                    msg.vpMessage = &roi_cfg;
                    msg.length = sizeof(roi_cfg);
                    ret1 |= AmbaCV_FlexidagSendMsg(&param->handle.fd_gen_handle,&msg);
                    for (i = 0U; i < param->num_runs; i++) {
                        pic_info->rpLumaLeft[0] = file_regionY[(i%MAX_GOLDEN_NUM)].buffer_daddr;
                        pic_info->rpChromaLeft[0] = (file_regionY[(i%MAX_GOLDEN_NUM)].buffer_daddr + (YuvWidth * YuvHeight));
                        pic_info->rpLumaRight[0] = pic_info->rpLumaLeft[0];
                        pic_info->rpChromaRight[0] = pic_info->rpChromaLeft[0];

                        AmbaMisra_TypeCast(&cb_param, &param);
                        ret = RefCVRingRunNonBlockingPicinfo(&param->handle, i, RefCVMnetSSDRunCallback, cb_param, pic_info, &token_id);
                        //AmbaPrint_PrintUInt5("RefCVFileInPicinfoNonBlocking (%d): RefCVRingRunPicinfo run %d token_id = %d",param->id, i, token_id, 0U, 0U);
                        if (ret == 0U) {
                            frame.frame_id = i;
                            frame.token_id = token_id;
                            ret1 |= AmbaKAL_MsgQueueSend(&param->result_msg_queue, &frame, AMBA_KAL_NO_WAIT);
                            ret1 |= AmbaKAL_MsgQueueReceive(&param->msg_queue, &ret, AMBA_KAL_WAIT_FOREVER);
                        }
                        ret1 |= AmbaKAL_TaskSleep(param->sleep_usec);
                    }
#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100)
                    ret1 |= RefCVSprintfUint1(&log_path[0], sizeof(log_path), "d:\\flexidag_mnetssd_sensor/flexidag_log%d.txt", param->id);
#else
                    ret1 |= RefCVSprintfUint1(&log_path[0], sizeof(log_path), "c:\\flexidag_mnetssd_sensor/flexidag_log%d.txt", param->id);
#endif
                    ret = AmbaCV_FlexidagDumpLog(&param->handle.fd_gen_handle,log_path, FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK);
                    if (ret != 0U) {
                        AmbaPrint_PrintUInt5("RefCVFileInPicinfoNonBlocking (%d) AmbaCV_FlexidagDumpLog fail ret=0x%x",param->id, ret, 0U, 0U, 0U);
                    }

                    ret = AmbaCV_FlexidagClose(&param->handle.fd_gen_handle);
                    if (ret != 0U) {
                        AmbaPrint_PrintUInt5("RefCVFileInPicinfoNonBlocking (%d) AmbaCV_FlexidagClose fail ",param->id, 0U, 0U, 0U, 0U);
                    }
                }
            }
        }
    }

    param->RunFlag = 0U;
    while(param->RunFlag != 2U) {
        ret |= AmbaKAL_TaskSleep(100U);
    }
    (void) ret;
    (void) ret1;
    AmbaPrint_PrintUInt5("RefCVFileInPicinfoNonBlocking (%d) end ",param->id, 0U, 0U, 0U, 0U);
    return NULL;
}

#if defined(CONFIG_AMBALINK_BOOT_OS) || defined(CONFIG_LINUX)
extern uint64_t ambacv_c2p(uint64_t ca);
#endif

static void* RefCVFlexidagIOInPicinfo(void *arg)
{
#if defined(CONFIG_AMBALINK_BOOT_OS) || defined(CONFIG_LINUX)
    UINT32 ret = 0U,i;
    UINT32 YuvWidth = 1920U;
    UINT32 YuvHeight = 1080U;
    memio_source_recv_picinfo_t memio_pic_info = {0};
    const memio_source_recv_picinfo_t *pmemio_pic_info = &memio_pic_info;
    cv_pic_info_t *pic_info = &memio_pic_info.pic_info;
    REF_CV_RING_PARAMS_s  *param;
    void* ptr;
    ULONG VirtAddr;
    flexidag_memblk_t buf;
    memio_sink_send_out_t MemIoSinkSendOut = {0};
    UINT32 GetLen;

    (void)arg;
    AmbaMisra_TouchUnused(arg);
    AmbaMisra_TypeCast(&param, &arg);
    param->RunFlag  = 1U;
    AmbaPrint_PrintUInt5("RefCVFlexidagIOInPicinfo (%d) start ",param->id, 0U, 0U, 0U, 0U);

    ret = RefCVMnetSSDLoadFile(param);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("RefCVFlexidagIOInPicinfo (%d) AmbaCV_UtilityFileLoad fail", param->id, 0U, 0U, 0U, 0U);
    }

    ret = AmbaIPC_FlexidagIO_GetResult_SetCB(param->id, NULL);
    if (ret != FLEXIDAGIO_OK) {
        AmbaPrint_PrintUInt5("RefCVFlexidagIOInPicinfo (%d) AmbaIPC_FlexidagIO_GetResult_SetCB fail", param->id, 0U, 0U, 0U, 0U);
    }

    pic_info->capture_time = 0U;
    pic_info->channel_id = 0U;
    pic_info->pyramid.image_width_m1 = (UINT16)(YuvWidth - 1U);
    pic_info->pyramid.image_height_m1 = (UINT16)(YuvHeight - 1U);
    pic_info->pyramid.image_pitch_m1 = (YuvWidth - 1U);
    pic_info->pyramid.half_octave[0].ctrl.roi_pitch = (UINT16)YuvWidth;
    pic_info->pyramid.half_octave[0].roi_start_col = (INT16)0;
    pic_info->pyramid.half_octave[0].roi_start_row = (INT16)0;
    pic_info->pyramid.half_octave[0].roi_width_m1 = (UINT16)(YuvWidth - 1U);
    pic_info->pyramid.half_octave[0].roi_height_m1 = (UINT16)(YuvHeight - 1U);

    pic_info->pyramid.half_octave[1].ctrl.disable = 1U;
    pic_info->pyramid.half_octave[2].ctrl.disable = 1U;
    pic_info->pyramid.half_octave[3].ctrl.disable = 1U;
    pic_info->pyramid.half_octave[4].ctrl.disable = 1U;
    pic_info->pyramid.half_octave[5].ctrl.disable = 1U;

    for (i = 0U; i < param->num_runs; i++) {
        pic_info->frame_num = i;
        pic_info->rpLumaLeft[0] = file_regionY[(i%MAX_GOLDEN_NUM)].buffer_daddr;
        pic_info->rpChromaLeft[0] = (file_regionY[(i%MAX_GOLDEN_NUM)].buffer_daddr + (YuvWidth * YuvHeight));
        pic_info->rpLumaRight[0] = pic_info->rpLumaLeft[0];
        pic_info->rpChromaRight[0] = pic_info->rpChromaLeft[0];

        AmbaMisra_TypeCast(&ptr, &pmemio_pic_info);
        ret = AmbaIPC_FlexidagIO_SetInput(param->id, (void *)ptr, sizeof(memio_source_recv_picinfo_t));
        if (ret != FLEXIDAGIO_OK) {
            AmbaPrint_PrintUInt5("RefCVFlexidagIOInPicinfo (%d) AmbaIPC_FlexidagIO_SetInput fail", param->id, 0U, 0U, 0U, 0U);
        } else {
            // receive cv result
            ret = AmbaIPC_FlexidagIO_GetResult(param->id, &MemIoSinkSendOut, &GetLen);
            if (ret != FLEXIDAGIO_OK) {
                AmbaPrint_PrintUInt5("RefCVFlexidagIOInPicinfo (%d) AmbaIPC_FlexidagIO_GetResult fail", param->id, 0U, 0U, 0U, 0U);
            } else if (GetLen != sizeof(MemIoSinkSendOut)) {
                AmbaPrint_PrintUInt5("RefCVFlexidagIOInPicinfo (%d) AmbaIPC_FlexidagIO_GetResult size fail", param->id, 0U, 0U, 0U, 0U);
            } else {
                buf.buffer_caddr = (ULONG)MemIoSinkSendOut.io[0].addr;
                buf.buffer_daddr = (ULONG)ambacv_c2p((UINT64)buf.buffer_caddr);;
                buf.buffer_size = MemIoSinkSendOut.io[0].size;
                ret = AmbaMMU_PhysToVirt(buf.buffer_daddr, &VirtAddr);
                if (ret != 0U) {
                    AmbaPrint_PrintUInt5("RefCVFlexidagIOInPicinfo (%d) AmbaMMU_PhysToVirt fail addr=0x%x", param->id, MemIoSinkSendOut.io[0].addr, 0U, 0U, 0U);
                } else {
                    AmbaMisra_TypeCast(&buf.pBuffer, &VirtAddr);
                }

                ret = RefCV_MemblkInvalid(&buf);
                ret |= RefCVPincinfoResult(param->id, i, &buf);
            }
        }
        if(ret != 0U) {
            param->num_err = param->num_err + 1U;
        }
        param->cur_runs = param->cur_runs + 1U;
        ret |= AmbaKAL_TaskSleep(param->sleep_usec);
    }

    param->RunFlag = 0U;
    while(param->RunFlag != 2U) {
        ret |= AmbaKAL_TaskSleep(100U);
    }
    (void) ret;
#else
    (void) arg;
    AmbaMisra_TouchUnused(arg);

#endif

    return NULL;
}

#if defined(CONFIG_AMBALINK_BOOT_OS) || defined(CONFIG_LINUX)
static UINT32 RefCVFlexidagIOInPicinfoResultCB(UINT32 Channel, const void *pData, UINT32 GetLen)
{
    UINT32 ret = 0U;
    ULONG VirtAddr;
    memio_sink_send_out_t MemIoSinkSendOut = {0};
    REF_CV_FRAME_s frame;

    if (GetLen != sizeof(MemIoSinkSendOut)) {
        AmbaPrint_PrintUInt5("RefCVFlexidagIOInPicinfoResultCB (%d) AmbaIPC_FlexidagIO_GetResult size fail", Channel, 0U, 0U, 0U, 0U);
        ret = 1U;
    } else {
        ret = AmbaWrap_memcpy(&MemIoSinkSendOut, pData, sizeof(memio_sink_send_out_t));
    }
    frame.frame_buf.buffer_caddr = (ULONG)MemIoSinkSendOut.io[0].addr;
    frame.frame_buf.buffer_daddr = (ULONG)ambacv_c2p((UINT64)frame.frame_buf.buffer_caddr);
    frame.frame_buf.buffer_size = MemIoSinkSendOut.io[0].size;
    frame.frame_id = MemIoSinkSendOut.cvtask_frameset_id;
    ret |= AmbaMMU_PhysToVirt(frame.frame_buf.buffer_daddr, &VirtAddr);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("RefCVFlexidagIOInPicinfoResultCB (%d) AmbaMMU_PhysToVirt fail addr=0x%x", Channel, MemIoSinkSendOut.io[0].addr, 0U, 0U, 0U);
    } else {
        AmbaMisra_TypeCast(&frame.frame_buf.pBuffer, &VirtAddr);
    }

    if ( AmbaKAL_MsgQueueSend(&mnet_thread_params[Channel].result_msg_queue, &frame, AMBA_KAL_NO_WAIT) != OK) {
        AmbaPrint_PrintUInt5("RefCVFlexidagIOInPicinfoResultCB (%d) AmbaKAL_MsgQueueSend() fail", Channel, 0U, 0U, 0U, 0U);
    }

    return ret;
}
#endif

static void* RefCVFlexidagIOInPicinfoResult(void *arg)
{
#if defined(CONFIG_AMBALINK_BOOT_OS) || defined(CONFIG_LINUX)
    UINT32 ret = 0U;
    REF_CV_FRAME_s frame;
    REF_CV_RING_PARAMS_s *param;

    (void)arg;
    AmbaMisra_TouchUnused(arg);
    AmbaMisra_TypeCast(&param, &arg);
    while(param->RunFlag != 3U) {
        if ( AmbaKAL_MsgQueueReceive(&param->result_msg_queue, &frame, AMBA_KAL_WAIT_FOREVER) != OK) {
            AmbaPrint_PrintUInt5("RefCVFlexidagIOInPicinfoResult (%d) AmbaKAL_MsgQueueReceive() fail.", param->id, 0U, 0U, 0U, 0U);
        } else if (frame.frame_id == 0xFFFFFFFFU) {
            break;
        } else {
            // receive cv result
            ret = RefCV_MemblkInvalid(&frame.frame_buf);
            ret |= RefCVPincinfoResult(param->id, frame.frame_id, &frame.frame_buf);
            if(ret != 0U) {
                param->num_err = param->num_err + 1U;
            }
            param->cur_runs = param->cur_runs + 1U;
        }
    }
#else
    (void) arg;
    AmbaMisra_TouchUnused(arg);
#endif

    return NULL;
}


static void* RefCVFlexidagIOInPicinfoNonBlocking(void *arg)
{
#if defined(CONFIG_AMBALINK_BOOT_OS) || defined(CONFIG_LINUX)
    UINT32 ret,i;
    UINT32 YuvWidth = 1920U;
    UINT32 YuvHeight = 1080U;
    memio_source_recv_picinfo_t memio_pic_info = {0};
    const memio_source_recv_picinfo_t *pmemio_pic_info = &memio_pic_info;
    cv_pic_info_t *pic_info = &memio_pic_info.pic_info;
    REF_CV_RING_PARAMS_s  *param;
    void* ptr;

    (void)arg;
    AmbaMisra_TouchUnused(arg);
    AmbaMisra_TypeCast(&param, &arg);
    param->RunFlag  = 1U;

    ret = RefCVMnetSSDLoadFile(param);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("RefCVFlexidagIOInPicinfoNonBlocking (%d) AmbaCV_UtilityFileLoad fail", param->id, 0U, 0U, 0U, 0U);
    }

    ret = AmbaIPC_FlexidagIO_GetResult_SetCB(param->id, RefCVFlexidagIOInPicinfoResultCB);
    if (ret != FLEXIDAGIO_OK) {
        AmbaPrint_PrintUInt5("RefCVFlexidagIOInPicinfoNonBlocking (%d) AmbaIPC_FlexidagIO_GetResult_SetCB fail", param->id, 0U, 0U, 0U, 0U);
    }

    pic_info->capture_time = 0U;
    pic_info->channel_id = 0U;
    pic_info->pyramid.image_width_m1 = (UINT16)(YuvWidth - 1U);
    pic_info->pyramid.image_height_m1 = (UINT16)(YuvHeight - 1U);
    pic_info->pyramid.image_pitch_m1 = (YuvWidth - 1U);
    pic_info->pyramid.half_octave[0].ctrl.roi_pitch = (UINT16)YuvWidth;
    pic_info->pyramid.half_octave[0].roi_start_col = (INT16)0;
    pic_info->pyramid.half_octave[0].roi_start_row = (INT16)0;
    pic_info->pyramid.half_octave[0].roi_width_m1 = (UINT16)(YuvWidth - 1U);
    pic_info->pyramid.half_octave[0].roi_height_m1 = (UINT16)(YuvHeight - 1U);

    pic_info->pyramid.half_octave[1].ctrl.disable = 1U;
    pic_info->pyramid.half_octave[2].ctrl.disable = 1U;
    pic_info->pyramid.half_octave[3].ctrl.disable = 1U;
    pic_info->pyramid.half_octave[4].ctrl.disable = 1U;
    pic_info->pyramid.half_octave[5].ctrl.disable = 1U;

    for (i = 0U; i < param->num_runs; i++) {
        pic_info->frame_num = i;
        pic_info->rpLumaLeft[0] = file_regionY[(i%MAX_GOLDEN_NUM)].buffer_daddr;
        pic_info->rpChromaLeft[0] = (file_regionY[(i%MAX_GOLDEN_NUM)].buffer_daddr + (YuvWidth * YuvHeight));
        pic_info->rpLumaRight[0] = pic_info->rpLumaLeft[0];
        pic_info->rpChromaRight[0] = pic_info->rpChromaLeft[0];

        AmbaMisra_TypeCast( &ptr, &pmemio_pic_info);
        ret = AmbaIPC_FlexidagIO_SetInput(param->id, (void *)ptr, sizeof(memio_source_recv_picinfo_t));
        if (ret != FLEXIDAGIO_OK) {
            AmbaPrint_PrintUInt5("RefCVFlexidagIOInPicinfoNonBlocking (%d) AmbaIPC_FlexidagIO_SetInput fail", param->id, 0U, 0U, 0U, 0U);
        }
        ret |= AmbaKAL_TaskSleep(param->sleep_usec);
    }

    param->RunFlag = 0U;
    while(param->RunFlag != 2U) {
        ret |= AmbaKAL_TaskSleep(100U);
    }
    (void) ret;
#else
    (void) arg;
    AmbaMisra_TouchUnused(arg);
#endif

    return NULL;
}


static void RefCVMnetSSDGoldenInit(void)
{
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    mnet_golden[0].NumBbx = 4U;
    mnet_golden[0].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[0].Bbx[0].X = (UINT16)810U;
    mnet_golden[0].Bbx[0].Y = (UINT16)390U;
    mnet_golden[0].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[0].Bbx[0].H = (UINT16)(690U-390U);
    mnet_golden[0].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[0].Bbx[1].X = (UINT16)884U;
    mnet_golden[0].Bbx[1].Y = (UINT16)475U;
    mnet_golden[0].Bbx[1].W = (UINT16)(994U - 884U);
    mnet_golden[0].Bbx[1].H = (UINT16)(579U - 475U);
    mnet_golden[0].Bbx[2].Cat = (UINT16)3U;
    mnet_golden[0].Bbx[2].X = (UINT16)1025U;
    mnet_golden[0].Bbx[2].Y = (UINT16)482U;
    mnet_golden[0].Bbx[2].W = (UINT16)(1103U - 1025U);
    mnet_golden[0].Bbx[2].H = (UINT16)(535U - 482U);
    mnet_golden[0].Bbx[3].Cat = (UINT16)3U;
    mnet_golden[0].Bbx[3].X = (UINT16)813U;
    mnet_golden[0].Bbx[3].Y = (UINT16)483U;
    mnet_golden[0].Bbx[3].W = (UINT16)(853U - 813U);
    mnet_golden[0].Bbx[3].H = (UINT16)(563U - 483U);

    mnet_golden[1].NumBbx = 3U;
    mnet_golden[1].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[1].Bbx[0].X = (UINT16)810U;
    mnet_golden[1].Bbx[0].Y = (UINT16)390U;
    mnet_golden[1].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[1].Bbx[0].H = (UINT16)(690U - 390U);
    mnet_golden[1].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[1].Bbx[1].X = (UINT16)908U;
    mnet_golden[1].Bbx[1].Y = (UINT16)513U;
    mnet_golden[1].Bbx[1].W = (UINT16)(979U - 908U);
    mnet_golden[1].Bbx[1].H = (UINT16)(574U - 513U);
    mnet_golden[1].Bbx[2].Cat = (UINT16)3U;
    mnet_golden[1].Bbx[2].X = (UINT16)860U;
    mnet_golden[1].Bbx[2].Y = (UINT16)511U;
    mnet_golden[1].Bbx[2].W = (UINT16)(907U - 860U);
    mnet_golden[1].Bbx[2].H = (UINT16)(548U - 511U);

    mnet_golden[2].NumBbx = 2U;
    mnet_golden[2].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[2].Bbx[0].X = (UINT16)810U;
    mnet_golden[2].Bbx[0].Y = (UINT16)390U;
    mnet_golden[2].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[2].Bbx[0].H = (UINT16)(690U - 390U);
    mnet_golden[2].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[2].Bbx[1].X = (UINT16)852U;
    mnet_golden[2].Bbx[1].Y = (UINT16)504U;
    mnet_golden[2].Bbx[1].W = (UINT16)(1009U - 852U);
    mnet_golden[2].Bbx[1].H = (UINT16)(639U - 504U);

    mnet_golden[3].NumBbx = 4U;
    mnet_golden[3].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[3].Bbx[0].X = (UINT16)810U;
    mnet_golden[3].Bbx[0].Y = (UINT16)390U;
    mnet_golden[3].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[3].Bbx[0].H = (UINT16)(690U - 390U);
    mnet_golden[3].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[3].Bbx[1].X = (UINT16)969U;
    mnet_golden[3].Bbx[1].Y = (UINT16)512U;
    mnet_golden[3].Bbx[1].W = (UINT16)(1103U - 969U);
    mnet_golden[3].Bbx[1].H = (UINT16)(579U - 512U);
    mnet_golden[3].Bbx[2].Cat = (UINT16)3U;
    mnet_golden[3].Bbx[2].X = (UINT16)810U;
    mnet_golden[3].Bbx[2].Y = (UINT16)516U;
    mnet_golden[3].Bbx[2].W = (UINT16)(914U - 810U);
    mnet_golden[3].Bbx[2].H = (UINT16)(604U - 516U);
    mnet_golden[3].Bbx[3].Cat = (UINT16)3U;
    mnet_golden[3].Bbx[3].X = (UINT16)905U;
    mnet_golden[3].Bbx[3].Y = (UINT16)518U;
    mnet_golden[3].Bbx[3].W = (UINT16)(980U - 905U);
    mnet_golden[3].Bbx[3].H = (UINT16)(576U - 518U);
#elif defined(CONFIG_SOC_CV28)
    mnet_golden[0].NumBbx = 4U;
    mnet_golden[0].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[0].Bbx[0].X = (UINT16)810U;
    mnet_golden[0].Bbx[0].Y = (UINT16)390U;
    mnet_golden[0].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[0].Bbx[0].H = (UINT16)(690U-390U);
    mnet_golden[0].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[0].Bbx[1].X = (UINT16)884U;
    mnet_golden[0].Bbx[1].Y = (UINT16)473U;
    mnet_golden[0].Bbx[1].W = (UINT16)(994U - 884U);
    mnet_golden[0].Bbx[1].H = (UINT16)(580U - 473U);
    mnet_golden[0].Bbx[2].Cat = (UINT16)3U;
    mnet_golden[0].Bbx[2].X = (UINT16)1026U;
    mnet_golden[0].Bbx[2].Y = (UINT16)482U;
    mnet_golden[0].Bbx[2].W = (UINT16)(1104U - 1026U);
    mnet_golden[0].Bbx[2].H = (UINT16)(535U - 482U);
    mnet_golden[0].Bbx[3].Cat = (UINT16)3U;
    mnet_golden[0].Bbx[3].X = (UINT16)812U;
    mnet_golden[0].Bbx[3].Y = (UINT16)482U;
    mnet_golden[0].Bbx[3].W = (UINT16)(854U - 812U);
    mnet_golden[0].Bbx[3].H = (UINT16)(564U - 482U);

    mnet_golden[1].NumBbx = 3U;
    mnet_golden[1].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[1].Bbx[0].X = (UINT16)810U;
    mnet_golden[1].Bbx[0].Y = (UINT16)390U;
    mnet_golden[1].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[1].Bbx[0].H = (UINT16)(690U - 390U);
    mnet_golden[1].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[1].Bbx[1].X = (UINT16)908U;
    mnet_golden[1].Bbx[1].Y = (UINT16)515U;
    mnet_golden[1].Bbx[1].W = (UINT16)(981U - 908U);
    mnet_golden[1].Bbx[1].H = (UINT16)(575U - 515U);
    mnet_golden[1].Bbx[2].Cat = (UINT16)3U;
    mnet_golden[1].Bbx[2].X = (UINT16)859U;
    mnet_golden[1].Bbx[2].Y = (UINT16)511U;
    mnet_golden[1].Bbx[2].W = (UINT16)(909U - 859U);
    mnet_golden[1].Bbx[2].H = (UINT16)(548U - 511U);

    mnet_golden[2].NumBbx = 2U;
    mnet_golden[2].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[2].Bbx[0].X = (UINT16)810U;
    mnet_golden[2].Bbx[0].Y = (UINT16)390U;
    mnet_golden[2].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[2].Bbx[0].H = (UINT16)(690U - 390U);
    mnet_golden[2].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[2].Bbx[1].X = (UINT16)846U;
    mnet_golden[2].Bbx[1].Y = (UINT16)504U;
    mnet_golden[2].Bbx[1].W = (UINT16)(1011U - 846U);
    mnet_golden[2].Bbx[1].H = (UINT16)(635U - 504U);

    mnet_golden[3].NumBbx = 3U;
    mnet_golden[3].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[3].Bbx[0].X = (UINT16)810U;
    mnet_golden[3].Bbx[0].Y = (UINT16)390U;
    mnet_golden[3].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[3].Bbx[0].H = (UINT16)(690U - 390U);
    mnet_golden[3].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[3].Bbx[1].X = (UINT16)973U;
    mnet_golden[3].Bbx[1].Y = (UINT16)512U;
    mnet_golden[3].Bbx[1].W = (UINT16)(1104U - 973U);
    mnet_golden[3].Bbx[1].H = (UINT16)(579U - 512U);
    mnet_golden[3].Bbx[2].Cat = (UINT16)3U;
    mnet_golden[3].Bbx[2].X = (UINT16)904U;
    mnet_golden[3].Bbx[2].Y = (UINT16)518U;
    mnet_golden[3].Bbx[2].W = (UINT16)(979U - 904U);
    mnet_golden[3].Bbx[2].H = (UINT16)(576U - 518U);
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    mnet_golden[0].NumBbx = 4U;
    mnet_golden[0].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[0].Bbx[0].X = (UINT16)810U;
    mnet_golden[0].Bbx[0].Y = (UINT16)390U;
    mnet_golden[0].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[0].Bbx[0].H = (UINT16)(690U-390U);
    mnet_golden[0].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[0].Bbx[1].X = (UINT16)884U;
    mnet_golden[0].Bbx[1].Y = (UINT16)475U;
    mnet_golden[0].Bbx[1].W = (UINT16)(994U - 884U);
    mnet_golden[0].Bbx[1].H = (UINT16)(579U - 475U);
    mnet_golden[0].Bbx[2].Cat = (UINT16)3U;
    mnet_golden[0].Bbx[2].X = (UINT16)1025U;
    mnet_golden[0].Bbx[2].Y = (UINT16)482U;
    mnet_golden[0].Bbx[2].W = (UINT16)(1103U - 1025U);
    mnet_golden[0].Bbx[2].H = (UINT16)(535U - 482U);
    mnet_golden[0].Bbx[3].Cat = (UINT16)3U;
    mnet_golden[0].Bbx[3].X = (UINT16)813U;
    mnet_golden[0].Bbx[3].Y = (UINT16)483U;
    mnet_golden[0].Bbx[3].W = (UINT16)(853U - 813U);
    mnet_golden[0].Bbx[3].H = (UINT16)(563U - 483U);

    mnet_golden[1].NumBbx = 3U;
    mnet_golden[1].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[1].Bbx[0].X = (UINT16)810U;
    mnet_golden[1].Bbx[0].Y = (UINT16)390U;
    mnet_golden[1].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[1].Bbx[0].H = (UINT16)(690U - 390U);
    mnet_golden[1].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[1].Bbx[1].X = (UINT16)908U;
    mnet_golden[1].Bbx[1].Y = (UINT16)513U;
    mnet_golden[1].Bbx[1].W = (UINT16)(979U - 908U);
    mnet_golden[1].Bbx[1].H = (UINT16)(574U - 513U);
    mnet_golden[1].Bbx[2].Cat = (UINT16)3U;
    mnet_golden[1].Bbx[2].X = (UINT16)860U;
    mnet_golden[1].Bbx[2].Y = (UINT16)511U;
    mnet_golden[1].Bbx[2].W = (UINT16)(907U - 860U);
    mnet_golden[1].Bbx[2].H = (UINT16)(548U - 511U);

    mnet_golden[2].NumBbx = 2U;
    mnet_golden[2].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[2].Bbx[0].X = (UINT16)810U;
    mnet_golden[2].Bbx[0].Y = (UINT16)390U;
    mnet_golden[2].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[2].Bbx[0].H = (UINT16)(690U - 390U);
    mnet_golden[2].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[2].Bbx[1].X = (UINT16)852U;
    mnet_golden[2].Bbx[1].Y = (UINT16)504U;
    mnet_golden[2].Bbx[1].W = (UINT16)(1009U - 852U);
    mnet_golden[2].Bbx[1].H = (UINT16)(639U - 504U);

    mnet_golden[3].NumBbx = 4U;
    mnet_golden[3].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[3].Bbx[0].X = (UINT16)810U;
    mnet_golden[3].Bbx[0].Y = (UINT16)390U;
    mnet_golden[3].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[3].Bbx[0].H = (UINT16)(690U - 390U);
    mnet_golden[3].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[3].Bbx[1].X = (UINT16)969U;
    mnet_golden[3].Bbx[1].Y = (UINT16)512U;
    mnet_golden[3].Bbx[1].W = (UINT16)(1103U - 969U);
    mnet_golden[3].Bbx[1].H = (UINT16)(579U - 512U);
    mnet_golden[3].Bbx[2].Cat = (UINT16)3U;
    mnet_golden[3].Bbx[2].X = (UINT16)810U;
    mnet_golden[3].Bbx[2].Y = (UINT16)516U;
    mnet_golden[3].Bbx[2].W = (UINT16)(914U - 810U);
    mnet_golden[3].Bbx[2].H = (UINT16)(604U - 516U);
    mnet_golden[3].Bbx[3].Cat = (UINT16)3U;
    mnet_golden[3].Bbx[3].X = (UINT16)905U;
    mnet_golden[3].Bbx[3].Y = (UINT16)518U;
    mnet_golden[3].Bbx[3].W = (UINT16)(980U - 905U);
    mnet_golden[3].Bbx[3].H = (UINT16)(576U - 518U);
#else
    mnet_golden[0].NumBbx = 4U;
    mnet_golden[0].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[0].Bbx[0].X = (UINT16)810U;
    mnet_golden[0].Bbx[0].Y = (UINT16)390U;
    mnet_golden[0].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[0].Bbx[0].H = (UINT16)(690U-390U);
    mnet_golden[0].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[0].Bbx[1].X = (UINT16)884U;
    mnet_golden[0].Bbx[1].Y = (UINT16)472U;
    mnet_golden[0].Bbx[1].W = (UINT16)(994U - 884U);
    mnet_golden[0].Bbx[1].H = (UINT16)(579U - 472U);
    mnet_golden[0].Bbx[2].Cat = (UINT16)3U;
    mnet_golden[0].Bbx[2].X = (UINT16)1025U;
    mnet_golden[0].Bbx[2].Y = (UINT16)482U;
    mnet_golden[0].Bbx[2].W = (UINT16)(1103U - 1025U);
    mnet_golden[0].Bbx[2].H = (UINT16)(535U - 482U);
    mnet_golden[0].Bbx[3].Cat = (UINT16)3U;
    mnet_golden[0].Bbx[3].X = (UINT16)812U;
    mnet_golden[0].Bbx[3].Y = (UINT16)482U;
    mnet_golden[0].Bbx[3].W = (UINT16)(854U - 812U);
    mnet_golden[0].Bbx[3].H = (UINT16)(564U - 482U);

    mnet_golden[1].NumBbx = 3U;
    mnet_golden[1].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[1].Bbx[0].X = (UINT16)810U;
    mnet_golden[1].Bbx[0].Y = (UINT16)390U;
    mnet_golden[1].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[1].Bbx[0].H = (UINT16)(690U - 390U);
    mnet_golden[1].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[1].Bbx[1].X = (UINT16)908U;
    mnet_golden[1].Bbx[1].Y = (UINT16)515U;
    mnet_golden[1].Bbx[1].W = (UINT16)(981U - 908U);
    mnet_golden[1].Bbx[1].H = (UINT16)(575U - 515U);
    mnet_golden[1].Bbx[2].Cat = (UINT16)3U;
    mnet_golden[1].Bbx[2].X = (UINT16)859U;
    mnet_golden[1].Bbx[2].Y = (UINT16)511U;
    mnet_golden[1].Bbx[2].W = (UINT16)(909U - 859U);
    mnet_golden[1].Bbx[2].H = (UINT16)(548U - 511U);

    mnet_golden[2].NumBbx = 2U;
    mnet_golden[2].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[2].Bbx[0].X = (UINT16)810U;
    mnet_golden[2].Bbx[0].Y = (UINT16)390U;
    mnet_golden[2].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[2].Bbx[0].H = (UINT16)(690U - 390U);
    mnet_golden[2].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[2].Bbx[1].X = (UINT16)846U;
    mnet_golden[2].Bbx[1].Y = (UINT16)504U;
    mnet_golden[2].Bbx[1].W = (UINT16)(1011U - 846U);
    mnet_golden[2].Bbx[1].H = (UINT16)(635U - 504U);

    mnet_golden[3].NumBbx = 3U;
    mnet_golden[3].Bbx[0].Cat = (UINT16)255U;
    mnet_golden[3].Bbx[0].X = (UINT16)810U;
    mnet_golden[3].Bbx[0].Y = (UINT16)390U;
    mnet_golden[3].Bbx[0].W = (UINT16)(1110U - 810U);
    mnet_golden[3].Bbx[0].H = (UINT16)(690U - 390U);
    mnet_golden[3].Bbx[1].Cat = (UINT16)3U;
    mnet_golden[3].Bbx[1].X = (UINT16)973U;
    mnet_golden[3].Bbx[1].Y = (UINT16)512U;
    mnet_golden[3].Bbx[1].W = (UINT16)(1104U - 973U);
    mnet_golden[3].Bbx[1].H = (UINT16)(579U - 512U);
    mnet_golden[3].Bbx[2].Cat = (UINT16)3U;
    mnet_golden[3].Bbx[2].X = (UINT16)904U;
    mnet_golden[3].Bbx[2].Y = (UINT16)518U;
    mnet_golden[3].Bbx[2].W = (UINT16)(979U - 904U);
    mnet_golden[3].Bbx[2].H = (UINT16)(576U - 518U);
#endif
}

static UINT32 RefCVMnetSSDParaInit(UINT32 thread_id, UINT32 num_of_frame, UINT32 blocking_run)
{
    UINT32 i,ret = 0U,ret1 = 0U;
#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100)
    const char flexidag_path[128]= "d:\\flexidag_mnetssd_sensor/flexibin/flexibin0.bin";
#else
    const char flexidag_path[128]= "c:\\flexidag_mnetssd_sensor/flexibin/flexibin0.bin";
#endif
    static UINT32 mnet_init[MAX_THREAD_NUM] = {0U};
    char name[32];

    if(thread_id > MAX_THREAD_NUM) {
        AmbaPrint_PrintUInt5("[error] thread_num(%d) > MAX_THREAD_NUM ",thread_id,MAX_THREAD_NUM, 0U, 0U, 0U);
        ret = 1U;
    } else {
        for(i = 0U; i < thread_id; i ++) {
            if(mnet_init[i] == 0U) {
                ret |= AmbaWrap_memset(&mnet_thread_params[i].handle, 0x0, sizeof(REF_CV_RING_HANDLE_s));

                mnet_thread_params[i].id = i;
                mnet_thread_params[i].num_runs  = num_of_frame;
                mnet_thread_params[i].sleep_usec = (UINT32)(20U*i);
                mnet_thread_params[i].RunFlag   = 0U;
                AmbaUtility_StringCopy(&mnet_thread_params[i].flexidag_path[0], sizeof(mnet_thread_params[i].flexidag_path), flexidag_path);

                ret1 |= RefCVSprintfUint1(&mnet_thread_params[i].handle.name[0], sizeof(mnet_thread_params[i].handle.name), "00%02d", i);
                mnet_thread_params[i].handle.id = i;
                mnet_thread_params[i].blocking_run = blocking_run;
                ret1 |= RefCVSprintfUint1(&name[0], sizeof(name), "MnetSSD_proc_00%02d", i);
                ret |= AmbaKAL_MsgQueueCreate(&mnet_thread_params[i].msg_queue, name, sizeof(UINT32), mnet_thread_params[i].queue_buffer, sizeof(mnet_thread_params[i].queue_buffer));
                ret1 |= RefCVSprintfUint1(&name[0], sizeof(name), "MnetSSD_result_00%02d", i);
                ret |= AmbaKAL_MsgQueueCreate(&mnet_thread_params[i].result_msg_queue, name, sizeof(REF_CV_FRAME_s), mnet_thread_params[i].result_queue_buffer, sizeof(mnet_thread_params[i].result_queue_buffer));
            } else {
                ret |= AmbaWrap_memset(&mnet_thread_params[i].handle.fd_gen_handle, 0x0, sizeof(AMBA_CV_FLEXIDAG_HANDLE_s));
                mnet_thread_params[i].num_runs  = num_of_frame;
                mnet_thread_params[i].blocking_run = blocking_run;
                mnet_thread_params[i].cur_runs = 0U;
                mnet_thread_params[i].num_err = 0U;
                mnet_thread_params[i].RunFlag = 0U;
            }
            mnet_init[i] = 1U;
            AmbaPrint_PrintUInt5("mnet_thread_params[%d].num_runs           = %d ",i,mnet_thread_params[i].num_runs, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("mnet_thread_params[%d].sleep_usec         = %d ",i,mnet_thread_params[i].sleep_usec, 0U, 0U, 0U);
            AmbaPrint_PrintStr5("mnet_thread_params.flexidag_path           = %s ",mnet_thread_params[i].flexidag_path, NULL, NULL, NULL, NULL);
            (void)ret;
        }
    }
    (void) ret1;
    return ret;
}

static void RefCVMnetSSDStop(UINT32 thread_id)
{
    UINT32 i,ret = 0U,ret1 = 0U, all_stop = 0U;
    REF_CV_FRAME_s frame;
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
    AMBA_KAL_TASK_INFO_s taskInfo;
#endif

    do {
        UINT32 still_run = 0U;
        for(i = 0U; i < thread_id; i ++) {
            if(mnet_thread_params[i].num_runs != mnet_thread_params[i].cur_runs) {
                still_run = 1U;
                break;
            }
        }
        if(still_run == 0U) {
            all_stop = 1U;
        }
        ret |= AmbaKAL_TaskSleep(250U); /* Sleep 250ms */
    } while (all_stop == 0U);

    for(i = 0U; i < thread_id; i ++) {
        AmbaPrint_PrintUInt5("RefCVMnetSSDRun thread (%d) total (%d) error (%d) ",mnet_thread_params[i].id, mnet_thread_params[i].cur_runs, mnet_thread_params[i].num_err, 0U, 0U);
    }

    for(i = 0U; i < thread_id; i ++) {
        mnet_thread_params[i].RunFlag = 2U;
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
        ret1 |= AmbaKAL_TaskQuery(&mnet_thread_params[i].task, &taskInfo);
        if ((taskInfo.TaskState != TX_COMPLETED) && (taskInfo.TaskState != TX_TERMINATED)) {
            ret |= AmbaKAL_TaskTerminate(&mnet_thread_params[i].task);
            if ( ret  != KAL_ERR_NONE ) {
                AmbaPrint_PrintUInt5("RefCVMnetSSDRun AmbaKAL_TaskTerminate fail (%d)", ret, 0U, 0U, 0U, 0U);
            }
        }
#endif
        ret |= AmbaKAL_TaskDelete(&mnet_thread_params[i].task);
        if ( ret != KAL_ERR_NONE ) {
            AmbaPrint_PrintUInt5("RefCVMnetSSDRun AmbaKAL_TaskDelete fail (%d)", ret, 0U, 0U, 0U, 0U);
        }

        if ( mnet_thread_params[i].blocking_run == 0U) {
            mnet_thread_params[i].RunFlag = 3U;
            frame.frame_id = 0xFFFFFFFFU;
            ret1 |= AmbaKAL_MsgQueueSend(&mnet_thread_params[i].result_msg_queue, &frame, AMBA_KAL_NO_WAIT);
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
            ret1 |= AmbaKAL_TaskQuery(&mnet_thread_params[i].result_task, &taskInfo);
            if ((taskInfo.TaskState != TX_COMPLETED) && (taskInfo.TaskState != TX_TERMINATED)) {
                ret1  = AmbaKAL_TaskTerminate(&mnet_thread_params[i].result_task);
                if ( ret1  != KAL_ERR_NONE ) {
                    AmbaPrint_PrintUInt5("RefCVMnetSSDRun AmbaKAL_TaskTerminate fail (%d)", ret, 0U, 0U, 0U, 0U);
                }
            }
#endif
            ret = AmbaKAL_TaskDelete(&mnet_thread_params[i].result_task);
            if ( ret != KAL_ERR_NONE ) {
                AmbaPrint_PrintUInt5("RefCVMnetSSDRun AmbaKAL_TaskDelete fail (%d)", ret, 0U, 0U, 0U, 0U);
            }
        }
    }
    (void) ret1;
    AmbaPrint_PrintUInt5("RefCVMnetSSDRun end ", 0U, 0U, 0U, 0U, 0U);

}

static UINT32 thread_num = 0U;

static void* RefCVMnetSSDRun(void *args)
{
    UINT32 i,ret = 0U;
    static UINT8 refcv_mnet_stack[MAX_THREAD_NUM][0x4000] GNU_SECTION_NOZEROINIT;
    static UINT8 refcv_mnet_result_stack[MAX_THREAD_NUM][0x4000] GNU_SECTION_NOZEROINIT;
    const REF_CV_RING_PARAMS_s *para_ptr;
    static char task_name[32] =    "refcv_mnet";

    (void) args;
    AmbaMisra_TouchUnused(args);
    for(i = 0U; i < thread_num; i ++) {
        para_ptr = &mnet_thread_params[i];
        if ( mnet_thread_params[i].blocking_run == 1U) {
            //task create
            if (run_mode == REF_CV_RUN_MODE_RTOS) {
                ret |= AmbaKAL_TaskCreate(&mnet_thread_params[i].task,           /* pTask */
                                          task_name,                                       /* pTaskName */
                                          100,                                             /* Priority */
                                          RefCVFileInPicinfo,                              /* void (*EntryFunction)(UINT32) */
                                          para_ptr,                                             /* EntryArg */
                                          &refcv_mnet_stack[i][0],                         /* pStackBase */
                                          sizeof(refcv_mnet_stack[i]),                     /* StackByteSize */
                                          0U);
            } else {
                ret |= AmbaKAL_TaskCreate(&mnet_thread_params[i].task,           /* pTask */
                                          task_name,                                       /* pTaskName */
                                          100,                                             /* Priority */
                                          RefCVFlexidagIOInPicinfo,                        /* void (*EntryFunction)(UINT32) */
                                          para_ptr,                                             /* EntryArg */
                                          &refcv_mnet_stack[i][0],                         /* pStackBase */
                                          sizeof(refcv_mnet_stack[i]),                     /* StackByteSize */
                                          0U);
            }

            if(ret == 0U) {
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
                ret |= AmbaKAL_TaskSetSmpAffinity(&mnet_thread_params[i].task, 0x1U);
#endif
                ret |= AmbaKAL_TaskResume(&mnet_thread_params[i].task);
            } else {
                AmbaPrint_PrintUInt5("RefCVMnetSSDRun AmbaKAL_TaskCreate task fail (%d)", ret, 0U, 0U, 0U, 0U);
            }
        } else {
            //task create
            if (run_mode == REF_CV_RUN_MODE_RTOS) {
                ret |= AmbaKAL_TaskCreate(&mnet_thread_params[i].task,           /* pTask */
                                          task_name,                                       /* pTaskName */
                                          100,                                             /* Priority */
                                          RefCVFileInPicinfoNonBlocking,                   /* void (*EntryFunction)(UINT32) */
                                          para_ptr,                                             /* EntryArg */
                                          &refcv_mnet_stack[i][0],                         /* pStackBase */
                                          sizeof(refcv_mnet_stack[i]),                     /* StackByteSize */
                                          0U);
            } else {
                ret |= AmbaKAL_TaskCreate(&mnet_thread_params[i].task,           /* pTask */
                                          task_name,                                       /* pTaskName */
                                          100,                                             /* Priority */
                                          RefCVFlexidagIOInPicinfoNonBlocking,             /* void (*EntryFunction)(UINT32) */
                                          para_ptr,                                             /* EntryArg */
                                          &refcv_mnet_stack[i][0],                         /* pStackBase */
                                          sizeof(refcv_mnet_stack[i]),                     /* StackByteSize */
                                          0U);
            }

            if(ret == 0U) {
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
                ret |= AmbaKAL_TaskSetSmpAffinity(&mnet_thread_params[i].task, 0x1U);
#endif
                ret |= AmbaKAL_TaskResume(&mnet_thread_params[i].task);
            } else {
                AmbaPrint_PrintUInt5("RefCVMnetSSDRun AmbaKAL_TaskCreate task fail (%d)", ret, 0U, 0U, 0U, 0U);
            }

            //result task create
            if (run_mode == REF_CV_RUN_MODE_RTOS) {
                ret |= AmbaKAL_TaskCreate(&mnet_thread_params[i].result_task,    /* pTask */
                                          task_name,                                       /* pTaskName */
                                          100,                                             /* Priority */
                                          RefCVFileInPicinfoResult,                        /* void (*EntryFunction)(UINT32) */
                                          para_ptr,                                             /* EntryArg */
                                          &refcv_mnet_result_stack[i][0],                  /* pStackBase */
                                          sizeof(refcv_mnet_result_stack[i]),              /* StackByteSize */
                                          0U);
            } else {
                ret |= AmbaKAL_TaskCreate(&mnet_thread_params[i].result_task,    /* pTask */
                                          task_name,                                       /* pTaskName */
                                          100,                                             /* Priority */
                                          RefCVFlexidagIOInPicinfoResult,                  /* void (*EntryFunction)(UINT32) */
                                          para_ptr,                                               /* EntryArg */
                                          &refcv_mnet_result_stack[i][0],                  /* pStackBase */
                                          sizeof(refcv_mnet_result_stack[i]),              /* StackByteSize */
                                          0U);
            }

            if(ret == 0U) {
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
                ret |= AmbaKAL_TaskSetSmpAffinity(&mnet_thread_params[i].result_task, 0x1U);
#endif
                ret |= AmbaKAL_TaskResume(&mnet_thread_params[i].result_task);
            } else {
                AmbaPrint_PrintUInt5("RefCVMnetSSDRun AmbaKAL_TaskCreate result_task fail (%d)", ret, 0U, 0U, 0U, 0U);
            }
        }
    }

    ret |= AmbaKAL_TaskSleep(200U);
    RefCVMnetSSDStop(thread_num);
    (void) ret;
    return NULL;
}

void RefCV_MnetSSD(UINT32 thread_id, UINT32 num_of_frame, UINT32 blocking_run, UINT8 freq, UINT32 mode)
{
    UINT32 Rval = 0U,ret1 = 0U;
    static UINT8 refcv_MnetSSD_main_stack[0x4000] GNU_SECTION_NOZEROINIT;
    static AMBA_KAL_TASK_t main_task;
    static UINT8 main_task_init = 0U;
    static char task_name[32] =  "mnetssd_main";
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
    AMBA_KAL_TASK_INFO_s taskInfo;
#endif

    if (main_task_init == 1U) {
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
        ret1 |= AmbaKAL_TaskQuery(&main_task, &taskInfo);
        if ((taskInfo.TaskState != TX_COMPLETED) && (taskInfo.TaskState != TX_TERMINATED)) {
            Rval |= AmbaKAL_TaskTerminate(&main_task);
            if ( Rval  != KAL_ERR_NONE ) {
                AmbaPrint_PrintUInt5("RefCV_Bisenet AmbaKAL_TaskTerminate fail (%d)", Rval, 0U, 0U, 0U, 0U);
            }
        }
#endif
        Rval |= AmbaKAL_TaskDelete(&main_task);
        if ( Rval != KAL_ERR_NONE ) {
            AmbaPrint_PrintUInt5("RefCV_Bisenet AmbaKAL_TaskDelete fail (%d)", Rval, 0U, 0U, 0U, 0U);
        }
    }

    frequency = freq;
    run_mode = mode;
    RefCVMnetSSDGoldenInit();
    Rval |= RefCVMnetSSDParaInit(thread_id, num_of_frame, blocking_run);
    if(Rval == 0U) {
        thread_num = thread_id;
        Rval |= AmbaKAL_TaskCreate(&main_task,                                       /* pTask */
                                   task_name,                                                   /* pTaskName */
                                   100,     /* Priority */
                                   RefCVMnetSSDRun,                                             /* void (*EntryFunction)(UINT32) */
                                   NULL,                                                   /* EntryArg */
                                   &refcv_MnetSSD_main_stack[0],                                /* pStackBase */
                                   sizeof(refcv_MnetSSD_main_stack),                                /* StackByteSize */
                                   0U);
        if(Rval == 0U) {
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
            Rval |= AmbaKAL_TaskSetSmpAffinity(&main_task, 0x1U);
#endif
            Rval |= AmbaKAL_TaskResume(&main_task);
        } else {
            AmbaPrint_PrintUInt5("RefCV_MnetSSD AmbaKAL_TaskCreate fail (%d)", Rval, 0U, 0U, 0U, 0U);
        }

        if(Rval == 0U) {
        }
    }
    main_task_init = 1U;
    (void) ret1;
}
