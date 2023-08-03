/**
 *  @file RefCV_Bisenet.c
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
 *  @details Implementation of Bisenet Test
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
#include "cvapi_svccvalgo_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"

#define MAX_THREAD_NUM              1U
#define MAX_GOLDEN_NUM              4U
#define BISENET_CVTASK_UUID         4
#define BISENET_CVTABLE             "DATA_SOURCE_00_IONAME_000"
#define BISENET_CVNAME              "FLEXIDAG_OPENSEG_AG_INP_0"

#define BISENET_INSTANCE            "FLEXIDAG_OPENSEG_AG_SUPERDAG"
#define BISENET_CVTASK              "flexidag_openseg_ag"

static UINT32 bisenet_golden[MAX_GOLDEN_NUM] = {0};
static REF_CV_RING_PARAMS_s bisenet_thread_params[MAX_THREAD_NUM];
static flexidag_memblk_t file_regionY[MAX_GOLDEN_NUM];
static flexidag_memblk_t file_regionUV[MAX_GOLDEN_NUM];

static UINT32 RefCVRingRunRaw(REF_CV_RING_HANDLE_s *handle, uint32_t frame_id, const memio_source_recv_multi_raw_t *in, AMBA_CV_FLEXIDAG_RUN_INFO_s *run_info)
{
    UINT32 ret = 0U, i;
    const AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    memio_source_recv_raw_t *pDataIn;
    UINT32 index = frame_id%REF_CV_BUF_NUM;

    if( handle->in_buf[index].num_of_buf != in->num_io ) {
        AmbaPrint_PrintUInt5("RefCVRunRaw : in->num_io (0x%x) != in_buf.num_of_buf (0x%x)", in->num_io, handle->in_buf[index].num_of_buf, 0U, 0U, 0U);
        ret = 1U;
    } else {
        for(i = 0U ; i < in->num_io ; i++) {
            AmbaMisra_TypeCast(&pDataIn, &handle->in_buf[index].buf[i].pBuffer);
            ret = AmbaWrap_memcpy(pDataIn, &in->io[i], sizeof(memio_source_recv_raw_t));
            if (ret != 0U) {
                AmbaPrint_PrintStr5("RefCVRunRaw : name = %s AmbaWrap_memcpy fail ", handle->name, NULL, NULL, NULL, NULL);
                ret = 1U;
            } else {
                ret = RefCV_MemblkClean(&handle->in_buf[index].buf[i]);
                if (ret != 0U) {
                    AmbaPrint_PrintStr5("RefCVRunRaw : name = %s RefCV_MemblkClean fail ", handle->name, NULL, NULL, NULL, NULL);
                    ret = 1U;

                }
            }

            if(ret != 0U) {
                break;
            }
        }

        if(ret == 0U) {
            if( handle->out_buf[index].num_of_buf != fd_gen_handle->mem_req.flexidag_num_outputs ) {
                AmbaPrint_PrintUInt5("RefCVRunRaw : mem_req.num_outputs (0x%x) != out_buf.num_of_buf (0x%x)", fd_gen_handle->mem_req.flexidag_num_outputs, handle->out_buf[index].num_of_buf, 0U, 0U, 0U);
                ret = 1U;
            } else {
                for(i = 0U; i < handle->out_buf[index].num_of_buf; i++) {
                    if(fd_gen_handle->mem_req.flexidag_output_buffer_size[i] > handle->out_buf[index].buf[i].buffer_size) {
                        AmbaPrint_PrintUInt5("RefCVRunRaw : mem_req.output_size[%d] (0x%x) > out_buf (0x%x)", i, fd_gen_handle->mem_req.flexidag_output_buffer_size[i], handle->out_buf[index].buf[i].buffer_size, 0U, 0U);
                        ret = 1U;
                    }

                    if(ret != 0U) {
                        break;
                    }
                }

                if (ret == 0U) {
                    ret = AmbaCV_FlexidagRun(fd_gen_handle, &handle->in_buf[index], &handle->out_buf[index], run_info);
                    if (ret != 0U) {
                        AmbaPrint_PrintStr5("RefCVRunRaw : name = %s AmbaCV_FlexidagRun fail ", handle->name, NULL, NULL, NULL, NULL);
                        ret = 1U;
                    }
                }
            }
        }
    }

    return ret;
}

static UINT32 RefCVRingRunNonBlockingRaw(REF_CV_RING_HANDLE_s *handle, uint32_t frame_id, flexidag_cb cb, void *cb_param, const memio_source_recv_multi_raw_t *in, UINT32 *token_id)
{
    UINT32 ret = 0U, i;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    memio_source_recv_raw_t *pDataIn;
    UINT32 index = frame_id%REF_CV_BUF_NUM;

    if( handle->in_buf[index].num_of_buf != in->num_io ) {
        AmbaPrint_PrintUInt5("RefCV_RunNonBlockingRaw : in->num_io (0x%x) != in_buf.num_of_buf (0x%x)", in->num_io, handle->in_buf[index].num_of_buf, 0U, 0U, 0U);
        ret = 1U;
    } else {
        for(i = 0U ; i < in->num_io ; i++) {
            AmbaMisra_TypeCast(&pDataIn, &handle->in_buf[index].buf[i].pBuffer);
            ret = AmbaWrap_memcpy(pDataIn, &in->io[i], sizeof(memio_source_recv_raw_t));
            if (ret != 0U) {
                AmbaPrint_PrintStr5("RefCV_RunNonBlockingRaw : name = %s AmbaWrap_memcpy fail ", handle->name, NULL, NULL, NULL, NULL);
                ret = 1U;
            } else {
                ret = RefCV_MemblkClean(&handle->in_buf[index].buf[i]);
                if (ret != 0U) {
                    AmbaPrint_PrintStr5("RefCV_RunNonBlockingRaw : name = %s RefCV_MemblkClean fail ", handle->name, NULL, NULL, NULL, NULL);
                    ret = 1U;

                }
            }

            if(ret != 0U) {
                break;
            }
        }

        if(ret == 0U) {
            if( handle->out_buf[index].num_of_buf != fd_gen_handle->mem_req.flexidag_num_outputs ) {
                AmbaPrint_PrintUInt5("RefCV_RunNonBlockingRaw : mem_req.num_outputs (0x%x) != out_buf.num_of_buf (0x%x)", fd_gen_handle->mem_req.flexidag_num_outputs, handle->out_buf[index].num_of_buf, 0U, 0U, 0U);
                ret = 1U;
            } else {
                for(i = 0U; i < handle->out_buf[index].num_of_buf; i++) {
                    if(fd_gen_handle->mem_req.flexidag_output_buffer_size[i] > handle->out_buf[index].buf[i].buffer_size) {
                        AmbaPrint_PrintUInt5("RefCV_RunNonBlockingRaw : mem_req.output_size[%d] (0x%x) > out_buf (0x%x)", i, fd_gen_handle->mem_req.flexidag_output_buffer_size[i], handle->out_buf[index].buf[i].buffer_size, 0U, 0U);
                        ret = 1U;
                    }

                    if(ret != 0U) {
                        break;
                    }
                }

                if (ret == 0U) {
                    ret = AmbaCV_FlexidagRunNonBlock(fd_gen_handle, cb, cb_param, &handle->in_buf[index], &handle->out_buf[index], token_id);
                    if (ret != 0U) {
                        AmbaPrint_PrintStr5("RefCV_RunNonBlockingRaw : name = %s AmbaCV_FlexidagRun fail ", handle->name, NULL, NULL, NULL, NULL);
                        ret = 1U;
                    }
                }
            }
        }
    }

    return ret;
}

static UINT32 RefCVRawResult(REF_CV_RING_PARAMS_s  *param, UINT32 frame, const flexidag_memblk_t *pBlk)
{
    UINT32 Rval = 0U, Crc;
    const UINT8 *ptr;
    UINT32 index = frame%MAX_GOLDEN_NUM ;

    AmbaMisra_TypeCast(&ptr, &pBlk->pBuffer);
    Crc = AmbaUtility_Crc32(ptr, pBlk->buffer_size);
    if(Crc != bisenet_golden[index]) {
        Rval = 1U;
    }

    if(Rval == 1U) {
        AmbaPrint_PrintUInt5("RefCVRawResult error : (%d) crc = %08x ",param->id, Crc, 0U, 0U, 0U);
        param->num_err = param->num_err + 1U;
    }
    return Rval;
}

static void RefCVBisenetRunCallback(void *vpHandle, uint32_t flexidag_output_num, flexidag_memblk_t *pblk_Output, void *vpParameter)
{
    uint32_t ret = 0U;
    REF_CV_RING_PARAMS_s  *param;

    (void) vpHandle;
    (void) flexidag_output_num;
    (void) pblk_Output;
    (void) vpParameter;
    AmbaMisra_TouchUnused(vpHandle);
    AmbaMisra_TouchUnused(pblk_Output);
    AmbaMisra_TouchUnused(vpParameter);
    AmbaMisra_TypeCast(&param, &vpParameter);
    ret = AmbaKAL_MsgQueueSend(&param->msg_queue, &ret, AMBA_KAL_NO_WAIT);
    (void)ret;
}

static UINT32 S_LoadFile(const char* pFileName, flexidag_memblk_t* pMemBlk)
{
    UINT32 ret = 0U;
    UINT32 size_align = 0U;

    ret = AmbaCV_UtilityFileSize(pFileName, &size_align);

    if(ret == 0U) {
        if(pMemBlk->pBuffer == NULL) {
            ret = RefCV_MemblkAlloc(size_align, pMemBlk);
        }
    } else {
        AmbaPrint_PrintStr5("S_LoadFile (%s) AmbaCV_UtilityFileSize fail ",pFileName, NULL, NULL, NULL, NULL);
    }

    if(ret == 0U) {
        ret = AmbaCV_UtilityFileLoad(pFileName, pMemBlk);
    } else {
        AmbaPrint_PrintStr5("S_LoadFile (%s) RefCV_MemblkAlloc fail ",pFileName, NULL, NULL, NULL, NULL);
    }

    if(ret != 0U) {
        AmbaPrint_PrintStr5("S_LoadFile (%d) AmbaCV_UtilityFileLoad fail ",pFileName, NULL, NULL, NULL, NULL);
    }

    return ret;
}

static UINT32 RefCVBisenetLoadFile(REF_CV_RING_PARAMS_s  *param)
{
    UINT32 ret = 0U,i,pos,tmp_pos;
#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100)
    const char data_path[128] = "d:\\flexidag_bisenet_mnv2_raw/golden/";
#else
    const char data_path[128] = "c:\\flexidag_bisenet_mnv2_raw/golden/";
#endif
    char file_nameY[128];
    char file_nameUV[128];

    // load input Y file
    for(i = 0U; i < MAX_GOLDEN_NUM; i ++) {
        pos = 0U;
        tmp_pos = RefCVSprintfStr(&file_nameY[pos], data_path);
        pos = pos + tmp_pos;
        tmp_pos = RefCVSprintfUint1(&file_nameY[pos], sizeof(file_nameY), "%d_y.bin", i);
        pos = pos + tmp_pos;
        (void) pos;
        if (ret == 0U) {
            ret = S_LoadFile(file_nameY, &file_regionY[i]);
            if(ret != 0U) {
                AmbaPrint_PrintUInt5("RefCVBisenetLoadFile (%d) S_LoadFile input Y fail ",param->id, 0U, 0U, 0U, 0U);
            }
        }
    }

    // load input UV file
    for(i = 0U; i < MAX_GOLDEN_NUM; i ++) {
        pos = 0U;
        tmp_pos = RefCVSprintfStr(&file_nameUV[pos], data_path);
        pos = pos + tmp_pos;
        tmp_pos = RefCVSprintfUint1(&file_nameUV[pos], sizeof(file_nameUV), "%d_uv.bin", i);
        pos = pos + tmp_pos;
        (void) pos;
        if (ret == 0U) {
            ret = S_LoadFile(file_nameUV, &file_regionUV[i]);
            if(ret != 0U) {
                AmbaPrint_PrintUInt5("RefCVBisenetLoadFile (%d) S_LoadFile input UV fail ",param->id, 0U, 0U, 0U, 0U);
            }
        }

    }

    // load bin file
    if (ret == 0U) {
        ret = S_LoadFile(param->flexidag_path, &param->handle.bin_buf);
        if(ret != 0U) {
            AmbaPrint_PrintUInt5("RefCVBisenetLoadFile (%d) S_LoadFile bin fail ",param->id, 0U, 0U, 0U, 0U);
        }
    }

    return ret;
}

static uint32_t RefCVBisenetAllocBuf(UINT32 id,REF_CV_RING_HANDLE_s *handle)
{
    UINT32 ret = 0U,i,j;
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

    for (i = 0U; i < REF_CV_BUF_NUM; i++) {
        //input buffer
        handle->in_buf[i].num_of_buf = 2U;
        for (j = 0U; j < handle->in_buf[i].num_of_buf; j++) {
            if(handle->in_buf[i].buf[j].pBuffer == NULL) {
                ret = RefCV_MemblkAlloc(sizeof(memio_source_recv_raw_t), &handle->in_buf[i].buf[j]);
            }
            if(ret != 0U) {
                AmbaPrint_PrintUInt5("RefCVBisenetAllocBuf (%d) in_buf buffer (%d) (%d) alloc fail ",id, i, j, 0U, 0U);
            }
        }

        //output buffer
        handle->out_buf[i].num_of_buf = handle->fd_gen_handle.mem_req.flexidag_num_outputs;
        for (j = 0U; j < handle->out_buf[i].num_of_buf; j++) {
            if(handle->out_buf[i].buf[j].pBuffer == NULL) {
                ret = RefCV_MemblkAlloc(handle->fd_gen_handle.mem_req.flexidag_output_buffer_size[j], &handle->out_buf[i].buf[j]);
            }
            if(ret != 0U) {
                AmbaPrint_PrintUInt5("RefCVBisenetAllocBuf (%d) out_buf buffer (%d) (%d) alloc fail ",id, i, j, 0U, 0U);
            }
        }
    }

    return ret;
}

static void RefCVBisenetErrorCb(void *vpHandle, flexidag_error_struct_t *pErrorStruct, void *vpParameter)
{
    const REF_CV_RING_PARAMS_s  *param;

    (void) vpHandle;
    (void) pErrorStruct;
    (void) vpParameter;
    AmbaMisra_TouchUnused(vpHandle);
    AmbaMisra_TouchUnused(pErrorStruct);
    AmbaMisra_TouchUnused(vpParameter);
    AmbaMisra_TypeCast(&param, &vpParameter);
    AmbaPrint_PrintUInt5("RefCVBisenetErrorCb : id (%d) frame (%d) sys_flow_id (%d)",param->id, pErrorStruct->cvtask_frameset_id, pErrorStruct->sysflow_index, 0U, 0U);
    AmbaPrint_PrintUInt5(pErrorStruct->pFormatString, pErrorStruct->arg0, pErrorStruct->arg1, pErrorStruct->arg2, pErrorStruct->arg3, pErrorStruct->arg4);
}

static void RefCVFileInRun(REF_CV_RING_PARAMS_s  *param, UINT16 flow_id)
{
    UINT32 i,ret = 0U;
    void *cb_param;
    AMBA_CV_FLEXIDAG_MSG_s msg;
    UINT32 token_id,buf_index;
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info;
    memio_source_recv_multi_raw_t raw;

    for (i = 0U; i < param->num_runs; i++) {
        //AmbaPrint_PrintUInt5("RefCVFileInRaw (%d): RefCVRunRaw run %d ",param->id, i, 0U, 0U, 0U);
        msg.flow_id = flow_id;
        msg.vpMessage = &i;
        msg.length = sizeof(i);
        ret |= AmbaCV_FlexidagSendMsg(&param->handle.fd_gen_handle,&msg);
        ret |= AmbaWrap_memset(&run_info,0,sizeof(run_info));

        raw.num_io = 2U;
        raw.io[0].addr = file_regionY[(i%MAX_GOLDEN_NUM)].buffer_caddr;
        raw.io[0].size = file_regionY[(i%MAX_GOLDEN_NUM)].buffer_size;
        raw.io[0].pitch = 0U;

        raw.io[1].addr = file_regionUV[(i%MAX_GOLDEN_NUM)].buffer_caddr;
        raw.io[1].size = file_regionUV[(i%MAX_GOLDEN_NUM)].buffer_size;
        raw.io[1].pitch = 0U;

        buf_index = i%REF_CV_BUF_NUM;
        if(param->blocking_run == 1U) {
            ret = RefCVRingRunRaw(&param->handle, i, &raw, &run_info);
        } else {
            AmbaMisra_TypeCast(&cb_param, &param);
            ret |= RefCVRingRunNonBlockingRaw(&param->handle, i, RefCVBisenetRunCallback, cb_param, &raw, &token_id);
            if (ret == 0U) {
                ret |= AmbaKAL_MsgQueueReceive(&param->msg_queue, &ret, AMBA_KAL_WAIT_FOREVER);
                ret |= AmbaCV_FlexidagWaitRunFinish(&param->handle.fd_gen_handle, token_id, &run_info);
            }
        }

        if(run_info.output_not_generated == 0U) {
            AmbaPrint_PrintUInt5("RefCVFileInRun (%d) frame_id = %d cv_run_time = %d us, over_head = %d us",param->id, i, run_info.end_time - run_info.start_time, ((run_info.api_end_time - run_info.api_start_time) - (run_info.end_time - run_info.start_time)), 0U);
            ret |= RefCV_MemblkInvalid(&param->handle.out_buf[buf_index].buf[0]);
            ret |= RefCVRawResult(param, i, &param->handle.out_buf[buf_index].buf[0]);
            param->cur_runs = param->cur_runs + 1U;
            ret |= AmbaWrap_memset(param->handle.out_buf[buf_index].buf[0].pBuffer, 0x0, param->handle.out_buf[buf_index].buf[0].buffer_size);

            ret |= RefCV_MemblkClean(&param->handle.out_buf[buf_index].buf[0]);
            ret |= AmbaKAL_TaskSleep(param->sleep_usec);
        }
    }
    (void) ret;
}

static void* RefCVFileInRaw(void *arg)
{
    UINT32 i,j,ret = 0U,ret1 = 0U;
    REF_CV_RING_PARAMS_s  *param;
    void *cb_param;
    UINT16 flow_id;
    AMBA_CV_FLEXIDAG_LOG_MSG_s set;
    const AMBA_CV_FLEXIDAG_LOG_MSG_s *pset;
    const void *param_set;
    const char *cvtable_name = BISENET_CVTABLE;
    const void *cvtable_data;
    const char *cvtable_ptr;
    UINT32 cvtable_size;
    AMBA_CV_FLEXIDAG_PERF_s flow_id_perf[4];
    AMBA_CV_FLEXIDAG_NAME_s flowtable_name;
    char cvtask_name[64] = BISENET_CVTASK;
    char instance_name[64] = BISENET_INSTANCE;
    uint16_t flow_id_array[4];
    uint32_t num_found;
    char log_path[128];

    (void) arg;
    AmbaMisra_TypeCast(&param, &arg);
    param->RunFlag  = 1U;
    AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) start ",param->id, 0U, 0U, 0U, 0U);

    ret = RefCVBisenetLoadFile(param);
    if (ret != 0U) {
        AmbaPrint_PrintStr5("RefCVFileInPicinfo : name = %s AmbaCV_UtilityFileLoad fail path = %s", param->handle.name, param->flexidag_path, NULL, NULL, NULL);
    } else {
        set.flexidag_msg_entry = 1U;        //no requirement to send msg from flexidag to cvtask
        set.cvtask_msg_entry = 0U;          //no requirement to send internal cvtask msg
        set.arm_cpu_map = 0x1U;
        set.arm_log_entry = 0U;
        set.orc_log_entry = 1024U;
        pset = &set;
        AmbaMisra_TypeCast(&param_set, &pset);
        ret |= AmbaCV_FlexidagSetParamSet(&param->handle.fd_gen_handle, FLEXIDAG_PARAMSET_LOG_MSG, param_set, sizeof(AMBA_CV_FLEXIDAG_LOG_MSG_s));
        ret |= AmbaCV_FlexidagOpen(&param->handle.bin_buf, &param->handle.fd_gen_handle);
        if(ret != 0U) {
            AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) AmbaCV_FlexidagOpen fail ",param->id, 0U, 0U, 0U, 0U);
        } else {
            ret |= RefCVBisenetAllocBuf(param->id, &param->handle);
            if(ret != 0U) {
                AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) RefCVBisenetAllocBuf fail ",param->id, 0U, 0U, 0U, 0U);
            } else {
                AmbaPrint_PrintUInt5("bisenet_thread_params[%d].bin_buf pa         = 0x%x size = 0x%x ",param->id,param->handle.bin_buf.buffer_daddr, param->handle.bin_buf.buffer_size, 0U, 0U);
                AmbaPrint_PrintUInt5("bisenet_thread_params[%d].state_buf pa       = 0x%x size = 0x%x ",param->id,param->handle.init.state_buf.buffer_daddr, param->handle.init.state_buf.buffer_size, 0U, 0U);
                AmbaPrint_PrintUInt5("bisenet_thread_params[%d].temp_buf pa        = 0x%x size = 0x%x ",param->id,param->handle.init.temp_buf.buffer_daddr, param->handle.init.temp_buf.buffer_size, 0U, 0U);
                for(j = 0U; j < REF_CV_BUF_NUM; j ++) {
                    AmbaPrint_PrintUInt5("bisenet_thread_params[%d].in_buf pa[%d]          = 0x%x size = 0x%x ",param->id, j,param->handle.in_buf[j].buf[0].buffer_daddr, param->handle.in_buf[j].buf[0].buffer_size, 0U);
                    AmbaPrint_PrintUInt5("bisenet_thread_params[%d].out_buf pa[%d]         = 0x%x size = 0x%x ",param->id, j,param->handle.out_buf[j].buf[0].buffer_daddr, param->handle.out_buf[j].buf[0].buffer_size, 0U);
                }
                ret |= AmbaCV_FlexidagInit(&param->handle.fd_gen_handle, &param->handle.init);;
                if(ret != 0U) {
                    AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) AmbaCV_FlexidagInit fail ",param->id, 0U, 0U, 0U, 0U);
                } else {
                    ret |= AmbaCV_FlexidagFindCvtable(&param->handle.fd_gen_handle, cvtable_name, &cvtable_data, &cvtable_size);
                    if(ret == 0U) {
                        AmbaMisra_TypeCast(&cvtable_ptr, &cvtable_data);
                        AmbaPrint_PrintUInt5("RefCVFileInRaw (%d): cvtable_size %d ",param->id, cvtable_size, 0U, 0U, 0U);
                        if(AmbaUtility_StringCompare(cvtable_ptr, BISENET_CVNAME, 25U) == 0) {
                            AmbaPrint_PrintStr5("cvtable_name(%s) = %s",cvtable_name, cvtable_ptr, NULL, NULL, NULL);
                        } else {
                            AmbaPrint_PrintStr5("[ERROR] cvtable_name(%s) is not currect ",cvtable_name, NULL, NULL, NULL, NULL);
                        }
                    }

                    flowtable_name.instance = instance_name;
                    flowtable_name.cvtask = cvtask_name;
                    flowtable_name.algorithm = NULL;
                    flowtable_name.step = NULL;
                    ret |= AmbaCV_FlexidagGetFlowIdByName(&param->handle.fd_gen_handle, flowtable_name, 4, flow_id_array, &num_found);
                    ret |= AmbaCV_FlexidagGetFlowIdByUUID(&param->handle.fd_gen_handle, BISENET_CVTASK_UUID, &flow_id);
                    if(flow_id != flow_id_array[0]) {
                        AmbaPrint_PrintUInt5("[ERROR] bisenet_mnv2_raw flow id (%d) flow_id_array[0] (%d) is not currect ",flow_id, flow_id_array[0], 0U, 0U, 0U);
                    } else {
                        AmbaPrint_PrintUInt5("bisenet_mnv2_raw flow id (%d)  ",flow_id, 0U, 0U, 0U, 0U);
                    }

                    AmbaMisra_TypeCast(&cb_param, &param);
                    ret |= AmbaCV_FlexidagSetErrorCb(&param->handle.fd_gen_handle, RefCVBisenetErrorCb, cb_param);
                    ret |= AmbaCV_FlexidagPerf(&param->handle.fd_gen_handle, FLEXIDAG_PERF_START, (uint32_t)CVCORE_VP0, flow_id_perf, 4);
                    RefCVFileInRun(param, flow_id);
                    ret |= AmbaCV_FlexidagPerf(&param->handle.fd_gen_handle, FLEXIDAG_PERF_STOP, (uint32_t)CVCORE_VP0, flow_id_perf, 4);
                    for(i = 0U; i < 4U; i++) {
                        AmbaPrint_PrintUInt5("RefCVFileInRaw (%d)  flow_id(%d)  runs(%d)    total_run_time(%d)    total_time(%d)",param->id, i,flow_id_perf[i].runs,flow_id_perf[i].total_run_time,flow_id_perf[i].total_time);
                    }

#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100)
                    ret1 |= RefCVSprintfUint1(&log_path[0], sizeof(log_path), "d:\\flexidag_bisenet_mnv2_raw/flexidag_log%d.txt", param->id);
#else
                    ret1 |= RefCVSprintfUint1(&log_path[0], sizeof(log_path), "c:\\flexidag_bisenet_mnv2_raw/flexidag_log%d.txt", param->id);
#endif
                    ret |= AmbaCV_FlexidagDumpLog(&param->handle.fd_gen_handle,log_path, FLEXILOG_CORE0|FLEXILOG_VIS_CVTASK);
                    ret |= AmbaCV_FlexidagClose(&param->handle.fd_gen_handle);
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

static void RefCVBisenetGoldenInit(void)
{
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    bisenet_golden[0] = 0xf3647648U;
    bisenet_golden[1] = 0xc806f96dU;
    bisenet_golden[2] = 0x3f943256U;
    bisenet_golden[3] = 0x1e66fd8dU;
#elif defined(CONFIG_SOC_CV28)
    bisenet_golden[0] = 0x8707bb19U;
    bisenet_golden[1] = 0x2f8c2e71U;
    bisenet_golden[2] = 0x9f1fb42bU;
    bisenet_golden[3] = 0x5452d506U;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    bisenet_golden[0] = 0xf3647648U;
    bisenet_golden[1] = 0xc806f96dU;
    bisenet_golden[2] = 0x3f943256U;
    bisenet_golden[3] = 0x1e66fd8dU;
#else
    bisenet_golden[0] = 0xbd29bf68U;
    bisenet_golden[1] = 0x553ad91fU;
    bisenet_golden[2] = 0xdca92e6cU;
    bisenet_golden[3] = 0x329d7a64U;
#endif
}

static UINT32 RefCVBisenetParaInit(UINT32 thread_id, UINT32 num_of_frame, UINT32 blocking_run)
{
    UINT32 i, ret = 0U,ret1 = 0U;
#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100)
    const char flexidag_path[128]= "d:\\flexidag_bisenet_mnv2_raw/flexibin/flexibin0.bin";
#else
    const char flexidag_path[128]= "c:\\flexidag_bisenet_mnv2_raw/flexibin/flexibin0.bin";
#endif
    char name[32] = "Bisenet";
    static UINT32 bisenet_init[MAX_THREAD_NUM] = {0U};

    if(thread_id > MAX_THREAD_NUM) {
        AmbaPrint_PrintUInt5("[error] thread_num(%d) > MAX_THREAD_NUM ",thread_id,MAX_THREAD_NUM, 0U, 0U, 0U);
        ret = 1U;
    } else {
        for(i = 0U; i < thread_id; i ++) {
            if(bisenet_init[i] == 0U) {
                ret |= AmbaWrap_memset(&bisenet_thread_params[i].handle, 0x0, sizeof(REF_CV_RING_HANDLE_s));

                bisenet_thread_params[i].id = i;
                bisenet_thread_params[i].num_runs   = num_of_frame;
                bisenet_thread_params[i].blocking_run = blocking_run;
                bisenet_thread_params[i].sleep_usec = (UINT32)(20U*i);
                bisenet_thread_params[i].RunFlag    = 0U;
                AmbaUtility_StringCopy(&bisenet_thread_params[i].flexidag_path[0], sizeof(bisenet_thread_params[i].flexidag_path), flexidag_path);
                ret1 |= RefCVSprintfUint1(&bisenet_thread_params[i].handle.name[0], sizeof(bisenet_thread_params[i].handle.name), "00%02d", i);
                bisenet_thread_params[i].handle.id = i;
                ret |= AmbaKAL_MsgQueueCreate(&bisenet_thread_params[i].msg_queue, name, sizeof(UINT32), bisenet_thread_params[i].queue_buffer, sizeof(bisenet_thread_params[i].queue_buffer));
            } else {
                ret |= AmbaWrap_memset(&bisenet_thread_params[i].handle.fd_gen_handle, 0x0, sizeof(AMBA_CV_FLEXIDAG_HANDLE_s));
                bisenet_thread_params[i].num_runs   = num_of_frame;
                bisenet_thread_params[i].blocking_run = blocking_run;
                bisenet_thread_params[i].cur_runs = 0U;
                bisenet_thread_params[i].num_err = 0U;
                bisenet_thread_params[i].RunFlag = 0U;
            }

            if(ret == 0U) {
                bisenet_init[i] = 1U;
                AmbaPrint_PrintUInt5("bisenet_thread_params[%d].num_runs           = %d ",i,bisenet_thread_params[i].num_runs, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("bisenet_thread_params[%d].sleep_usec         = %d ",i,bisenet_thread_params[i].sleep_usec, 0U, 0U, 0U);
                AmbaPrint_PrintStr5("bisenet_thread_params.flexidag_path           = %s ",bisenet_thread_params[i].flexidag_path, NULL, NULL, NULL, NULL);
            }
        }
    }
    (void) ret1;
    return ret;
}

static UINT32 thread_num = 0U;

static void* RefCVBisenetRun(void *args)
{
    UINT32 i,ret = 0U,ret1 = 0U,all_stop = 0U;
    static UINT8 refcv_bisenet_stack[MAX_THREAD_NUM][0x4000] GNU_SECTION_NOZEROINIT;
    const REF_CV_RING_PARAMS_s *para_ptr;
    static char task_name[32] =  "refcv_bisenet";
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
    AMBA_KAL_TASK_INFO_s taskInfo;
#endif

    (void) args;
    AmbaMisra_TouchUnused(args);
    for(i = 0U; i < thread_num; i ++) {
        para_ptr = &bisenet_thread_params[i];
        ret = AmbaKAL_TaskCreate(&bisenet_thread_params[i].task,                        /* pTask */
                                 task_name,                                                   /* pTaskName */
                                 100,     /* Priority */
                                 RefCVFileInRaw,                                              /* void (*EntryFunction)(UINT32) */
                                 para_ptr,                                             /* EntryArg */
                                 &refcv_bisenet_stack[i][0],                                      /* pStackBase */
                                 sizeof(refcv_bisenet_stack[i]),                                  /* StackByteSize */
                                 0U);
        if(ret == 0U) {
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
            ret |= AmbaKAL_TaskSetSmpAffinity(&bisenet_thread_params[i].task, 0x1U);
#endif
            ret |= AmbaKAL_TaskResume(&bisenet_thread_params[i].task);
        } else {
            AmbaPrint_PrintUInt5("RefCVBisenetRun AmbaKAL_TaskCreate fail (%d)", ret, 0U, 0U, 0U, 0U);
        }
    }

    ret |= AmbaKAL_TaskSleep(200U);
    do {
        UINT32 still_run = 0U;
        for(i = 0U; i < thread_num; i ++) {
            if( bisenet_thread_params[i].RunFlag != 0U ) {
                still_run = 1U;
                break;
            }
        }
        if(still_run == 0U) {
            all_stop = 1U;
        }
        ret |= AmbaKAL_TaskSleep(250U); /* Sleep 250ms */
    } while (all_stop == 0U);

    for(i = 0U; i < thread_num; i ++) {
        AmbaPrint_PrintUInt5("RefCVBisenetRun thread (%d) total (%d) error (%d) ",bisenet_thread_params[i].id, bisenet_thread_params[i].cur_runs, bisenet_thread_params[i].num_err, 0U, 0U);
    }

    for(i = 0U; i < thread_num; i ++) {
        bisenet_thread_params[i].RunFlag = 2U;
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
        ret1 |= AmbaKAL_TaskQuery(&bisenet_thread_params[i].task, &taskInfo);
        if ((taskInfo.TaskState != TX_COMPLETED) && (taskInfo.TaskState != TX_TERMINATED)) {
            ret |= AmbaKAL_TaskTerminate(&bisenet_thread_params[i].task);
            if ( ret  != KAL_ERR_NONE ) {
                AmbaPrint_PrintUInt5("RefCVBisenetRun AmbaKAL_TaskTerminate fail (%d)", ret, 0U, 0U, 0U, 0U);
            }
        }
#endif
        ret |= AmbaKAL_TaskDelete(&bisenet_thread_params[i].task);
        if ( ret != KAL_ERR_NONE ) {
            AmbaPrint_PrintUInt5("RefCVBisenetRun AmbaKAL_TaskDelete fail (%d)", ret, 0U, 0U, 0U, 0U);
        }
    }
    AmbaPrint_PrintUInt5("RefCVBisenetRun end ", 0U, 0U, 0U, 0U, 0U);
    (void) ret1;
    return NULL;
}

void RefCV_Bisenet(UINT32 thread_id, UINT32 num_of_frame, UINT32 blocking_run, UINT8 freq)
{
    UINT32 Rval = 0U,ret1 = 0U;
    static UINT8 refcv_bisenet_main_stack[0x4000] GNU_SECTION_NOZEROINIT;
    static AMBA_KAL_TASK_t main_task;
    static UINT8 main_task_init = 0U;
    static char task_name[32] =  "bisenet_main";
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
    AMBA_KAL_TASK_INFO_s taskInfo;
#endif

    if (main_task_init == 1U) {
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
        ret1  = AmbaKAL_TaskQuery(&main_task, &taskInfo);
        if ((taskInfo.TaskState != TX_COMPLETED) && (taskInfo.TaskState != TX_TERMINATED)) {
            Rval  |= AmbaKAL_TaskTerminate(&main_task);
            if ( Rval  != KAL_ERR_NONE ) {
                AmbaPrint_PrintUInt5("RefCV_Bisenet AmbaKAL_TaskTerminate fail (%d)", Rval, 0U, 0U, 0U, 0U);
            }
        }
#endif
        Rval  |= AmbaKAL_TaskDelete(&main_task);
        if ( Rval != KAL_ERR_NONE ) {
            AmbaPrint_PrintUInt5("RefCV_Bisenet AmbaKAL_TaskDelete fail (%d)", Rval, 0U, 0U, 0U, 0U);
        }
    }

    (void) freq;
    RefCVBisenetGoldenInit();
    Rval  |= RefCVBisenetParaInit(thread_id, num_of_frame, blocking_run);
    if(Rval == 0U) {
        thread_num = thread_id;
        Rval  |= AmbaKAL_TaskCreate(&main_task,                                       /* pTask */
                                    task_name,                                                   /* pTaskName */
                                    100,     /* Priority */
                                    RefCVBisenetRun,                                             /* void (*EntryFunction)(UINT32) */
                                    NULL,                                                   /* EntryArg */
                                    &refcv_bisenet_main_stack[0],                                /* pStackBase */
                                    sizeof(refcv_bisenet_main_stack),                                /* StackByteSize */
                                    0U);
        if(Rval == 0U) {
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
            Rval  |= AmbaKAL_TaskSetSmpAffinity(&main_task, 0x1U);
#endif
            Rval  |= AmbaKAL_TaskResume(&main_task);
        } else {
            AmbaPrint_PrintUInt5("RefCV_Bisenet AmbaKAL_TaskCreate fail (%d)", Rval, 0U, 0U, 0U, 0U);
        }

        if(Rval == 0U) {
        }
    }
    main_task_init = 1U;
    (void) ret1;
}
