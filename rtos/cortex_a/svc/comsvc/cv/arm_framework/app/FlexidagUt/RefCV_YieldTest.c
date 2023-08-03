/**
 *  @file RefCV_YieldTest.c
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
 *  @details Implementation of Yield Test
 *
 */

#include "AmbaIntrinsics.h"
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
#include "AmbaNVM_Partition.h"
#include "cvapi_visutil.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"

#define MAX_THREAD_NUM              4U

//#define FROM_RFS                0

static REF_CV_RING_PARAMS_s yield_test_thread_params[MAX_THREAD_NUM];

static UINT32 RefCVRingRunRaw(REF_CV_RING_HANDLE_s *handle, uint32_t frame_id, AMBA_CV_FLEXIDAG_RUN_INFO_s *run_info)
{
    UINT32 ret = 0U, i;
    const AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    UINT32 index = frame_id%REF_CV_BUF_NUM;
    void *ptr;
    UINT32 ret1 = 0U;

    if(handle->in_buf[index].buf[0].pBuffer != NULL) {
        AmbaMisra_TypeCast(&ptr, &handle->in_buf[index].buf[0].pBuffer);
        ret1 |= AmbaWrap_memset(ptr,(INT32)frame_id,2048);
        ret1 |= RefCV_MemblkClean(&handle->in_buf[index].buf[0]);
    } else {
        AmbaPrint_PrintUInt5("RefCVRunRaw : handle->in_buf[%d].buf[0].pBuffer == NULL ", index, 0U, 0U, 0U, 0U);
    }
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
    (void) ret1;
    return ret;
}

static UINT32 RefCVRingRunNonBlockingRaw(REF_CV_RING_HANDLE_s *handle, uint32_t frame_id, flexidag_cb cb, void *cb_param, UINT32 *token_id)
{
    UINT32 ret = 0U,ret1 = 0U, i;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    UINT32 index = frame_id%REF_CV_BUF_NUM;
    void *ptr;

    if(handle->in_buf[index].buf[0].pBuffer != NULL) {
        AmbaMisra_TypeCast(&ptr, &handle->in_buf[index].buf[0].pBuffer);
        ret1 |= AmbaWrap_memset(ptr,(INT32)frame_id,2048);
        ret1 |= RefCV_MemblkClean(&handle->in_buf[index].buf[0]);
    } else {
        AmbaPrint_PrintUInt5("RefCV_RunNonBlockingRaw : handle->in_buf[%d].buf[0].pBuffer == NULL ", index, 0U, 0U, 0U, 0U);
    }
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
    (void) ret1;
    return ret;
}

static void RefCVYieldTestRunCallback(void *vpHandle, uint32_t flexidag_output_num, flexidag_memblk_t *pblk_Output, void *vpParameter)
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
    (void) ret;
}

static UINT32 RefCVYieldTestLoadFile(REF_CV_RING_PARAMS_s  *param)
{
    UINT32 ret = 0U,size_align = 0U;
#ifdef FROM_RFS
    UINT8 *ptr;

    ret = AmbaNVM_GetRomFileSize(AMBA_NVM_NAND, AMBA_NVM_ROM_REGION_SYS_DATA, "flexibin0.fbin", &size_align);
    if(ret == 0U) {
        if(param->handle.bin_buf.pBuffer == NULL) {
            ret = RefCV_MemblkAlloc((uint32_t)((size_align + 127U) & 0xFFFFFF80U), &param->handle.bin_buf);
        }
        if(ret == 0U) {
            AmbaMisra_TypeCast(&ptr, &param->handle.bin_buf.pBuffer);
            ret |= AmbaNVM_ReadRomFile(AMBA_NVM_NAND, AMBA_NVM_ROM_REGION_SYS_DATA, "flexibin0.fbin", 0U, size_align, ptr, 5000U);
            ret |= RefCV_MemblkClean(&param->handle.bin_buf);
            if(ret != 0U) {
                AmbaPrint_PrintUInt5("RefCVYieldTestLoadFile (%d) AmbaNVM_ReadRomFile fail ",param->id, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("RefCVYieldTestLoadFile (%d) RefCV_MemblkAlloc fail ",param->id, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("RefCVYieldTestLoadFile (%d) AmbaNVM_GetRomFileSize fail ",param->id, 0U, 0U, 0U, 0U);
    }
#else

    // load bin file
    ret = AmbaCV_UtilityFileSize(param->flexidag_path, &size_align);

    if(ret == 0U) {
        if(param->handle.bin_buf.pBuffer == NULL) {
            ret = RefCV_MemblkAlloc(size_align, &param->handle.bin_buf);
        }
    } else {
        AmbaPrint_PrintUInt5("RefCVYieldTestLoadFile (%d) AmbaCV_UtilityFileSize bin fail ",param->id, 0U, 0U, 0U, 0U);
    }

    if(ret == 0U) {
        ret = AmbaCV_UtilityFileLoad(param->flexidag_path, &param->handle.bin_buf);
    } else {
        AmbaPrint_PrintUInt5("RefCVYieldTestLoadFile (%d) RefCV_MemblkAlloc bin fail ",param->id, 0U, 0U, 0U, 0U);
    }

    if(ret != 0U) {
        AmbaPrint_PrintUInt5("RefCVYieldTestLoadFile (%d) AmbaCV_UtilityFileLoad bin fail ",param->id, 0U, 0U, 0U, 0U);
    }
#endif

    return ret;
}

static uint32_t RefCVYieldTestAllocBuf(UINT32 id,REF_CV_RING_HANDLE_s *handle)
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
        handle->in_buf[i].num_of_buf = 1U;
        for (j = 0U; j < handle->in_buf[i].num_of_buf; j++) {
            if(handle->in_buf[i].buf[j].pBuffer == NULL) {
                ret = RefCV_MemblkAlloc(2048, &handle->in_buf[i].buf[j]);
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

static void RefCVYieldTestErrorCb(void *vpHandle, flexidag_error_struct_t *pErrorStruct, void *vpParameter)
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

static UINT32 RefCVRawResult(const REF_CV_RING_PARAMS_s  *param, UINT32 frame, const flexidag_memblk_t *pBlk0, const flexidag_memblk_t *pBlk1)
{
    UINT32 Rval = 0U;
    const UINT8 *ptr0,*ptr1;

    (void) param;
    AmbaMisra_TypeCast(&ptr0, &pBlk0->pBuffer);
    AmbaMisra_TypeCast(&ptr1, &pBlk1->pBuffer);
    if((ptr0[0] != (UINT8)(frame + 20U)) || (ptr0[1023] != (UINT8)(frame + 20U)) || (ptr0[2047] != (UINT8)(frame + 20U))) {
        AmbaPrint_PrintUInt5("RefCVRawResult error : (%d) frame = %d : out0 (%d %d %d) ",param->id, frame, ptr0[0], ptr0[1023], ptr0[2047]);
        Rval = 1U;
    }

    if((ptr1[0] != (UINT8)(frame + 2U)) || (ptr1[1023] != (UINT8)(frame + 2U)) || (ptr1[2047] != (UINT8)(frame + 2U))) {
        AmbaPrint_PrintUInt5("RefCVRawResult error : (%d) frame = %d : out1 (%d %d %d) ",param->id, frame, ptr1[0], ptr1[1023], ptr1[2047]);
        Rval = 1U;
    }
    return Rval;
}

static void* RefCVFileInRaw(void* arg)
{
    UINT32 i,j,ret = 0U;
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info;
    REF_CV_RING_PARAMS_s  *param;
    char log_path[128];
    UINT32 token_id,buf_index;
    AMBA_CV_FLEXIDAG_MSG_s msg;
    void *cb_param;
    AMBA_CV_FLEXIDAG_LOG_MSG_s set;
    const AMBA_CV_FLEXIDAG_LOG_MSG_s *pset;
    const void *param_set;
    AMBA_CV_FLEXIDAG_PERF_s flow_id_perf[4];
    UINT16 flow_id;
    UINT32 ret1 = 0U,ret2 = 0U;

    (void) arg;
    AmbaMisra_TypeCast(&param, &arg);
    param->RunFlag  = 1U;
    AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) start ",param->id, 0U, 0U, 0U, 0U);

    ret = RefCVYieldTestLoadFile(param);
    if (ret != 0U) {
        AmbaPrint_PrintStr5("RefCVFileInPicinfo : name = %s AmbaCV_UtilityFileLoad fail path = %s", param->handle.name, param->flexidag_path, NULL, NULL, NULL);
    } else {
        set.flexidag_msg_entry = 8U;        //no requirement to send msg from flexidag to cvtask
        set.cvtask_msg_entry = 8U;          //no requirement to send internal cvtask msg
        set.arm_cpu_map = 0x1U;
        set.arm_log_entry = 0U;
        set.orc_log_entry = 1024U;
        pset = &set;
        AmbaMisra_TypeCast(&param_set, &pset);
        ret1 |= AmbaCV_FlexidagSetParamSet(&param->handle.fd_gen_handle, FLEXIDAG_PARAMSET_LOG_MSG, param_set, sizeof(AMBA_CV_FLEXIDAG_LOG_MSG_s));
        ret = AmbaCV_FlexidagOpen(&param->handle.bin_buf, &param->handle.fd_gen_handle);
        if(ret != 0U) {
            AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) AmbaCV_FlexidagOpen fail ",param->id, 0U, 0U, 0U, 0U);
        } else {
            ret = RefCVYieldTestAllocBuf(param->id, &param->handle);
            if(ret != 0U) {
                AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) RefCVYieldTestAllocBuf fail ",param->id, 0U, 0U, 0U, 0U);
            } else {
                AmbaPrint_PrintUInt5("yield_test_thread_params[%d].bin_buf pa         = 0x%x size = 0x%x ",param->id,param->handle.bin_buf.buffer_daddr, param->handle.bin_buf.buffer_size, 0U, 0U);
                AmbaPrint_PrintUInt5("yield_test_thread_params[%d].state_buf pa       = 0x%x size = 0x%x ",param->id,param->handle.init.state_buf.buffer_daddr, param->handle.init.state_buf.buffer_size, 0U, 0U);
                AmbaPrint_PrintUInt5("yield_test_thread_params[%d].temp_buf pa        = 0x%x size = 0x%x ",param->id,param->handle.init.temp_buf.buffer_daddr, param->handle.init.temp_buf.buffer_size, 0U, 0U);
                for(j = 0U; j < REF_CV_BUF_NUM; j ++) {
                    AmbaPrint_PrintUInt5("yield_test_thread_params[%d].in_buf pa[%d]          = 0x%x size = 0x%x ",param->id, j,param->handle.in_buf[j].buf[0].buffer_daddr, param->handle.in_buf[j].buf[0].buffer_size, 0U);
                    AmbaPrint_PrintUInt5("yield_test_thread_params[%d].out_buf pa[%d]         = 0x%x size = 0x%x ",param->id, j,param->handle.out_buf[j].buf[0].buffer_daddr, param->handle.out_buf[j].buf[0].buffer_size, 0U);
                }
                ret = AmbaCV_FlexidagInit(&param->handle.fd_gen_handle, &param->handle.init);;
                if(ret != 0U) {
                    AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) AmbaCV_FlexidagInit fail ",param->id, 0U, 0U, 0U, 0U);
                } else {
                    AmbaMisra_TypeCast(&cb_param, &param);
                    ret1 |= AmbaCV_FlexidagSetErrorCb(&param->handle.fd_gen_handle, RefCVYieldTestErrorCb, cb_param);
                    ret1 |= AmbaCV_FlexidagPerf(&param->handle.fd_gen_handle, FLEXIDAG_PERF_START, (uint32_t)CVCORE_VP0, flow_id_perf, 4);
                    for (i = 0U; i < param->num_runs; i++) {
                        //AmbaPrint_PrintUInt5("RefCVFileInRaw (%d): RefCVRunRaw run %d ",param->id, i, 0U, 0U, 0U);
                        ret1 |= AmbaCV_FlexidagGetFlowIdByUUID(&param->handle.fd_gen_handle, 1000, &flow_id);
                        msg.flow_id = flow_id;
                        msg.vpMessage = &i;
                        msg.length = sizeof(i);
                        ret1 |= AmbaCV_FlexidagSendMsg(&param->handle.fd_gen_handle,&msg);

                        ret1 |= AmbaCV_FlexidagGetFlowIdByUUID(&param->handle.fd_gen_handle, 1001, &flow_id);
                        msg.flow_id = flow_id;
                        msg.vpMessage = &i;
                        msg.length = sizeof(i);
                        ret1 |= AmbaCV_FlexidagSendMsg(&param->handle.fd_gen_handle,&msg);

                        ret1 |= AmbaWrap_memset(&run_info,0,sizeof(run_info));
                        buf_index = i%REF_CV_BUF_NUM;
                        if(param->blocking_run == 1U) {
                            ret = RefCVRingRunRaw(&param->handle, i, &run_info);
                        } else {
                            AmbaMisra_TypeCast(&cb_param, &param);
                            ret = RefCVRingRunNonBlockingRaw(&param->handle, i, RefCVYieldTestRunCallback, cb_param, &token_id);
                            if (ret == 0U) {
                                ret1 |= AmbaKAL_MsgQueueReceive(&param->msg_queue, &ret, AMBA_KAL_WAIT_FOREVER);
                                ret1 |= AmbaCV_FlexidagWaitRunFinish(&param->handle.fd_gen_handle, token_id, &run_info);
                            }
                        }
                        if (ret == 1U) {
                            break;
                        }
                        if(run_info.output_not_generated == 0U) {
                            AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) frame_id = %d cv_run_time = %d us, over_head = %d us",param->id, i, run_info.end_time - run_info.start_time, ((run_info.api_end_time - run_info.api_start_time) - (run_info.end_time - run_info.start_time)), 0U);
                            ret1 |= RefCV_MemblkInvalid(&param->handle.out_buf[buf_index].buf[0]);
                            ret1 |= RefCV_MemblkInvalid(&param->handle.out_buf[buf_index].buf[1]);
                            ret1 |= RefCVRawResult(param, i, &param->handle.out_buf[buf_index].buf[0], &param->handle.out_buf[buf_index].buf[1]);
                            param->cur_runs = param->cur_runs + 1U;
                            ret1 |= AmbaWrap_memset(param->handle.out_buf[buf_index].buf[0].pBuffer, 0x0, param->handle.out_buf[buf_index].buf[0].buffer_size);
                            ret1 |= RefCV_MemblkClean(&param->handle.out_buf[buf_index].buf[0]);
                            ret1 |= AmbaWrap_memset(param->handle.out_buf[buf_index].buf[1].pBuffer, 0x0, param->handle.out_buf[buf_index].buf[0].buffer_size);
                            ret1 |= RefCV_MemblkClean(&param->handle.out_buf[buf_index].buf[1]);
                            ret1 |= AmbaKAL_TaskSleep(param->sleep_usec);
                        }
                    }
                    ret1 |= AmbaCV_FlexidagPerf(&param->handle.fd_gen_handle, FLEXIDAG_PERF_STOP, (uint32_t)CVCORE_VP0, flow_id_perf, 4);
                    for(i = 0U; i < 4U; i++) {
                        AmbaPrint_PrintUInt5("RefCVFileInRaw (%d)  flow_id(%d)  runs(%d)    total_run_time(%d)    total_time(%d)",param->id, i,flow_id_perf[i].runs,flow_id_perf[i].total_run_time,flow_id_perf[i].total_time);
                    }

#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100)
                    ret2 |= RefCVSprintfUint1(&log_path[0], sizeof(log_path), "d:\\flexidag_log%d.txt", param->id);
#else
                    ret2 |= RefCVSprintfUint1(&log_path[0], sizeof(log_path), "c:\\flexidag_log%d.txt", param->id);
#endif
                    ret = AmbaCV_FlexidagDumpLog(&param->handle.fd_gen_handle,NULL, FLEXILOG_CORE0|FLEXILOG_VIS_CVTASK);
                    if (ret != 0U) {
                        AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) AmbaCV_FlexidagDumpLog fail ",param->id, 0U, 0U, 0U, 0U);
                    }

                    ret = AmbaCV_FlexidagClose(&param->handle.fd_gen_handle);
                    if (ret != 0U) {
                        AmbaPrint_PrintUInt5("RefCVFileInRaw (%d) AmbaCV_FlexidagClose fail ",param->id, 0U, 0U, 0U, 0U);
                    }
                }
            }
        }
    }

    param->RunFlag = 0U;
    while(param->RunFlag != 2U) {
        ret1 |= AmbaKAL_TaskSleep(100U);
    }
    (void) ret1;
    (void) ret2;
    return NULL;
}

static UINT32 RefCVYieldTestParaInit(UINT32 thread_id, UINT32 num_of_frame, UINT32 blocking_run)
{
    UINT32 i, ret = 0U;
#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100)
    const char flexidag_path[128]= "d:\\flexidag_yield_test/flexibin/flexibin0.fbin";
#else
    const char flexidag_path[128]= "c:\\flexidag_yield_test/flexibin/flexibin0.fbin";
#endif
    char name[32] = "YieldTest";
    static UINT32 yield_test_init[MAX_THREAD_NUM] = {0U};
    UINT32 ret1 = 0U;

    if(thread_id > MAX_THREAD_NUM) {
        AmbaPrint_PrintUInt5("[error] thread_num(%d) > MAX_THREAD_NUM ",thread_id,MAX_THREAD_NUM, 0U, 0U, 0U);
        ret = 1U;
    } else {
        for(i = 0U; i < thread_id; i ++) {
            if(yield_test_init[i] == 0U) {
                ret1 |= AmbaWrap_memset(&yield_test_thread_params[i].handle, 0x0, sizeof(REF_CV_RING_HANDLE_s));

                yield_test_thread_params[i].id = i;
                yield_test_thread_params[i].num_runs   = num_of_frame;
                yield_test_thread_params[i].blocking_run = blocking_run;
                yield_test_thread_params[i].sleep_usec = (UINT32)(20U*i);
                yield_test_thread_params[i].RunFlag    = 0U;
                AmbaUtility_StringCopy(&yield_test_thread_params[i].flexidag_path[0], sizeof(yield_test_thread_params[i].flexidag_path), flexidag_path);
                ret1 |= RefCVSprintfUint1(&yield_test_thread_params[i].handle.name[0], sizeof(yield_test_thread_params[i].handle.name), "00%02d", i);
                yield_test_thread_params[i].handle.id = i;
                ret |= AmbaKAL_MsgQueueCreate(&yield_test_thread_params[i].msg_queue, name, sizeof(UINT32), yield_test_thread_params[i].queue_buffer, sizeof(yield_test_thread_params[i].queue_buffer));
            } else {
                ret |= AmbaWrap_memset(&yield_test_thread_params[i].handle.fd_gen_handle, 0x0, sizeof(AMBA_CV_FLEXIDAG_HANDLE_s));
                yield_test_thread_params[i].num_runs   = num_of_frame;
                yield_test_thread_params[i].blocking_run = blocking_run;
                yield_test_thread_params[i].cur_runs = 0U;
                yield_test_thread_params[i].num_err = 0U;
                yield_test_thread_params[i].RunFlag = 0U;
            }

            if(ret == 0U) {
                yield_test_init[i] = 1U;
                AmbaPrint_PrintUInt5("yield_test_thread_params[%d].num_runs           = %d ",i,yield_test_thread_params[i].num_runs, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("yield_test_thread_params[%d].sleep_usec         = %d ",i,yield_test_thread_params[i].sleep_usec, 0U, 0U, 0U);
                AmbaPrint_PrintStr5("yield_test_thread_params.flexidag_path           = %s ",yield_test_thread_params[i].flexidag_path, NULL, NULL, NULL, NULL);
            }
        }
    }
    (void) ret1;
    return ret;
}

static void* RefCVYieldTestRun(void* arg)
{
    UINT32 i,ret = 0U,all_stop = 0U;
    static UINT8 refcv_yield_test_stack[MAX_THREAD_NUM][0x4000] GNU_SECTION_NOZEROINIT;
    const REF_CV_RING_PARAMS_s *para_ptr;
    char task_name[32] =  "refcv_yield_test";
    AMBA_KAL_TASK_INFO_s taskInfo;
    const UINT32 *pthread_id;
    UINT32 thread_id;
    UINT32 ret1 = 0U;

    (void) arg;
    AmbaMisra_TypeCast(&pthread_id, &arg);
    thread_id = *pthread_id;
    for(i = 0U; i < thread_id; i ++) {
        para_ptr = &yield_test_thread_params[i];
        ret = AmbaKAL_TaskCreate(&yield_test_thread_params[i].task,                        /* pTask */
                                 task_name,                                                   /* pTaskName */
                                 100,     /* Priority */
                                 RefCVFileInRaw,                                              /* void (*EntryFunction)(UINT32) */
                                 para_ptr,                                             /* EntryArg */
                                 &refcv_yield_test_stack[i][0],                                      /* pStackBase */
                                 sizeof(refcv_yield_test_stack[i]),                                  /* StackByteSize */
                                 0U);

        if(ret == 0U) {
            ret1 |= AmbaKAL_TaskSetSmpAffinity(&yield_test_thread_params[i].task, 0x1U);
            ret1 |= AmbaKAL_TaskResume(&yield_test_thread_params[i].task);
        } else {
            AmbaPrint_PrintUInt5("RefCVYieldTestRun AmbaKAL_TaskCreate fail (%d)", ret, 0U, 0U, 0U, 0U);
        }
    }

    ret1 |= AmbaKAL_TaskSleep(200U);
    do {
        UINT32 still_run = 0U;
        for(i = 0U; i < thread_id; i ++) {
            if( yield_test_thread_params[i].RunFlag != 0U ) {
                still_run = 1U;
                break;
            }
        }
        if(still_run == 0U) {
            all_stop = 1U;
        }
        ret1 |= AmbaKAL_TaskSleep(250U); /* Sleep 250ms */
    } while (all_stop == 0U);

    for(i = 0U; i < thread_id; i ++) {
        AmbaPrint_PrintUInt5("RefCVYieldTestRun thread (%d) total (%d) error (%d) ",yield_test_thread_params[i].id, yield_test_thread_params[i].cur_runs, yield_test_thread_params[i].num_err, 0U, 0U);
    }

    for(i = 0U; i < thread_id; i ++) {
        yield_test_thread_params[i].RunFlag = 2U;
        ret1 |= AmbaKAL_TaskQuery(&yield_test_thread_params[i].task, &taskInfo);
        if ((taskInfo.TaskState != TX_COMPLETED) && (taskInfo.TaskState != TX_TERMINATED)) {
            ret  = AmbaKAL_TaskTerminate(&yield_test_thread_params[i].task);
            if ( ret  != KAL_ERR_NONE ) {
                AmbaPrint_PrintUInt5("RefCVYieldTestRun AmbaKAL_TaskTerminate fail (%d)", ret, 0U, 0U, 0U, 0U);
            }
        }
        ret = AmbaKAL_TaskDelete(&yield_test_thread_params[i].task);
        if ( ret != KAL_ERR_NONE ) {
            AmbaPrint_PrintUInt5("RefCVYieldTestRun AmbaKAL_TaskDelete fail (%d)", ret, 0U, 0U, 0U, 0U);
        }
    }
    AmbaPrint_PrintUInt5("RefCVYieldTestRun end ", 0U, 0U, 0U, 0U, 0U);
    (void) ret1;
    return NULL;
}

void RefCV_YieldTest(UINT32 thread_id, UINT32 num_of_frame, UINT32 blocking_run)
{
    UINT32 Rval;
    static UINT8 refcv_yield_test_main_stack[0x4000] GNU_SECTION_NOZEROINIT;
    static AMBA_KAL_TASK_t main_task;
    static UINT8 main_task_init = 0U;
    char task_name[32] =  "yield_test_main";
    AMBA_KAL_TASK_INFO_s taskInfo;
    UINT32 thread_num = 1;
    UINT32 ret1 = 0U;

    if (main_task_init == 1U) {
        ret1 |= AmbaKAL_TaskQuery(&main_task, &taskInfo);
        if ((taskInfo.TaskState != TX_COMPLETED) && (taskInfo.TaskState != TX_TERMINATED)) {
            Rval  = AmbaKAL_TaskTerminate(&main_task);
            if ( Rval  != KAL_ERR_NONE ) {
                AmbaPrint_PrintUInt5("RefCV_YieldTest AmbaKAL_TaskTerminate fail (%d)", Rval, 0U, 0U, 0U, 0U);
            }
        }
        Rval = AmbaKAL_TaskDelete(&main_task);
        if ( Rval != KAL_ERR_NONE ) {
            AmbaPrint_PrintUInt5("RefCV_YieldTest AmbaKAL_TaskDelete fail (%d)", Rval, 0U, 0U, 0U, 0U);
        }
    }

    Rval = RefCVYieldTestParaInit(thread_id, num_of_frame, blocking_run);
    if(Rval == 0U) {
        Rval = AmbaKAL_TaskCreate(&main_task,                                       /* pTask */
                                  task_name,                                                   /* pTaskName */
                                  100,     /* Priority */
                                  RefCVYieldTestRun,                                             /* void (*EntryFunction)(UINT32) */
                                  &thread_num,                                                   /* EntryArg */
                                  &refcv_yield_test_main_stack[0],                                /* pStackBase */
                                  sizeof(refcv_yield_test_main_stack),                                /* StackByteSize */
                                  0U);
        if(Rval == 0U) {
            ret1 |= AmbaKAL_TaskSetSmpAffinity(&main_task, 0x1U);
            ret1 |= AmbaKAL_TaskResume(&main_task);
        } else {
            AmbaPrint_PrintUInt5("RefCV_YieldTest AmbaKAL_TaskCreate fail (%d)", Rval, 0U, 0U, 0U, 0U);
        }

        if(Rval == 0U) {
        }
    }
    (void) ret1;
    main_task_init = 1U;

}
