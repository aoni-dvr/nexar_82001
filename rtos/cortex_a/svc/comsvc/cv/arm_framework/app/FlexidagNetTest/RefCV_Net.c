/**
 *  @file RefCV.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details SVC COMSVC CV
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
#include "AmbaShell.h"
#include "cvapi_visutil.h"
#include "cvapi_flexidag_unittest.h"
#include "cvapi_memio_interface.h"
#include "cvapi_svccvalgo_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"

#define MAX_THREAD_NUM              4U
#define MAX_GOLDEN_NUM              4U
#define REF_CV_BUF_NUM              4U

#define REF_MEMIO_PICINFO           0U
#define REF_MEMIO_RAW               1U
#define REF_MEMIO_MAX               2U

#define MAX_STR_TOKEN_NUMBER        (32U)
#define MAX_STR_TOKEN_LENGTH        (128U)
#define READ_BUF_SIZE               (1024U)

#define OUTPUT_TYPE_RAW             0U
#define OUTPUT_TYPE_MAX             2U

typedef struct {
    AMBA_CV_FLEXIDAG_HANDLE_s       fd_gen_handle;
    AMBA_CV_FLEXIDAG_INIT_s         init;
    flexidag_memblk_t               bin_buf;
    uint32_t                        memio_type;
    uint32_t                        num_raw;
    uint32_t                        raw_pitch;
    uint32_t                        raw_width;
    uint32_t                        raw_height;
    uint32_t                        num_per_raw;
    uint32_t                        output_from_arm;
    uint32_t                        output_type;
    flexidag_memblk_t               raw_buf_y[MAX_GOLDEN_NUM];
    flexidag_memblk_t               raw_buf_uv[MAX_GOLDEN_NUM];
    flexidag_memblk_t               result_buf[MAX_GOLDEN_NUM];
    AMBA_CV_FLEXIDAG_IO_s           in_buf[REF_CV_BUF_NUM];
    AMBA_CV_FLEXIDAG_IO_s           out_buf[REF_CV_BUF_NUM];
} REF_CV_NET_HANDLE_s;

typedef struct {
    uint32_t frame_id;
    uint32_t token_id;
} REF_CV_NET_FRAME_s;

typedef struct {
    uint32_t                        id;
    uint32_t                        init;
    uint32_t                        num_runs;
    uint32_t                        cur_runs;
    uint32_t                        num_err;
    uint32_t                        RunFlag;
    uint32_t                        ResultFlag;
    uint32_t                        output_file;
    REF_CV_NET_HANDLE_s             handle;
    AMBA_KAL_TASK_t                 task;
    AMBA_KAL_TASK_t                 result_task;
    AMBA_KAL_MSG_QUEUE_t            msg_queue;
    AMBA_KAL_MSG_QUEUE_t            result_msg_queue;
    uint32_t                        queue_buffer[REF_CV_BUF_NUM];
    REF_CV_NET_FRAME_s              result_queue_buffer[REF_CV_BUF_NUM];
} REF_CV_NET_PARAMS_s;

static REF_CV_NET_PARAMS_s net_thread_params[MAX_THREAD_NUM];
static flexidag_memblk_t g_temp_buf;
static char output_path[128U] = "./";
static uint32_t num_of_thread;
static uint32_t net_task_loop;
static uint32_t net_task_result_loop;

static UINT32 RefCV_NetSprintfStr(char *str, const char *str_src)
{
    SIZE_t len;

    len = AmbaUtility_StringLength(str_src);
    AmbaUtility_StringCopy(str, len + 1U, str_src);

    return len;
}

static uint32_t RefCV_NetSprintfUint1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0)
{
    uint32_t rval;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 1, &arg0);

    return rval;
}

static uint32_t RefCV_NetSprintfUint2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1)
{
    uint32_t rval;
    uint32_t args[2];

    args[0] = arg0;
    args[1] = arg1;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 2, args);

    return rval;
}

static uint32_t RefCV_NetFlexidagOpen(REF_CV_NET_HANDLE_s *handle)
{
    uint32_t ret = 0U;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    AMBA_CV_FLEXIDAG_LOG_MSG_s set;
    const AMBA_CV_FLEXIDAG_LOG_MSG_s *pset;
    const void *param_set;

    set.flexidag_msg_entry = 1U;        //no requirement to send msg from flexidag to cvtask
    set.cvtask_msg_entry = 1U;              //no requirement to send internal cvtask msg
    set.arm_cpu_map = 0xFU;
    set.arm_log_entry = 32U;
    set.orc_log_entry = 32U;
    pset = &set;
    AmbaMisra_TypeCast(&param_set, &pset);
    ret = AmbaCV_FlexidagSetParamSet(&handle->fd_gen_handle, FLEXIDAG_PARAMSET_LOG_MSG, param_set, sizeof(AMBA_CV_FLEXIDAG_LOG_MSG_s));
    if (ret == 0U) {
        ret = AmbaCV_FlexidagOpen(&handle->bin_buf, fd_gen_handle);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("RefCV_NetFlexidagOpen : AmbaCV_FlexidagOpen fail ", 0U, 0U, 0U, 0U, 0U);
            ret = 1U;
        }
    }


    return ret;
}

static uint32_t RefCV_NetFlexidagInit(REF_CV_NET_HANDLE_s *handle)
{
    uint32_t ret = 0U;
    const AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;

    if(handle->init.state_buf.pBuffer == NULL) {
        ret = RefCV_MemblkAlloc(fd_gen_handle->mem_req.flexidag_state_buffer_size, &handle->init.state_buf);
        //AmbaPrint_PrintUInt5("RefCV_MemblkAlloc state_buf pa = 0x%x size = 0x%x ",handle->init.state_buf.buffer_daddr, handle->init.state_buf.buffer_size, 0U, 0U, 0U);
    } else {
        //printf("RefCV_NetFlexidagInit : name = %s handle->init.state_buf.pBuffer != NULL\n", handle->name);
    }

    if (ret != 0U) {
        AmbaPrint_PrintUInt5("RefCV_NetFlexidagInit : RefCV_MemblkAlloc state_buf fail", 0U, 0U, 0U, 0U, 0U);
        ret = 1U;
    } else {
        if(handle->init.temp_buf.pBuffer == NULL) {
            ret = AmbaWrap_memcpy(&handle->init.temp_buf,&g_temp_buf,sizeof(flexidag_memblk_t));
            //AmbaPrint_PrintUInt5("RefCV_MemblkAlloc temp_buf pa = 0x%x size = 0x%x ",handle->init.temp_buf.buffer_daddr, handle->init.temp_buf.buffer_size, 0U, 0U, 0U);
        } else {
            //printf("RefCV_NetFlexidagInit : name = %s handle->init.temp_buf.pBuffer != NULL\n", handle->name);
        }
        if(ret == 0U) {
            ret = AmbaCV_FlexidagInit(fd_gen_handle, &handle->init);
            if (ret != 0U) {
                AmbaPrint_PrintUInt5("RefCV_NetFlexidagInit : AmbaCV_FlexidagInit fail", 0U, 0U, 0U, 0U, 0U);
                ret = 1U;
            }
        }
    }

    return ret;
}

static uint32_t RefCV_NetFlexidagRunPicinfo(REF_CV_NET_HANDLE_s *handle, uint32_t frame_id, flexidag_cb cb, void *cb_param, const cv_pic_info_t *in, uint32_t *token_id)
{
    uint32_t ret = 0U, i;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    cv_pic_info_t *pDataIn;
    uint32_t U32DataIn;
    uint32_t index = frame_id%REF_CV_BUF_NUM;

    handle->in_buf[index].num_of_buf = 1U;
    if(handle->in_buf[index].buf[0].pBuffer == NULL) {
        ret = RefCV_MemblkAlloc(sizeof(cv_pic_info_t), &handle->in_buf[index].buf[0]);
        //AmbaPrint_PrintUInt5("RefCV_MemblkAlloc in_buf[%d].buf[0] pa = 0x%x size = 0x%x ",index, handle->in_buf[index].buf[0].buffer_daddr, handle->in_buf[index].buf[0].buffer_size, 0U, 0U);
    }

    if(ret != 0U) {
        AmbaPrint_PrintStr5("RefCV_NetFlexidagRunPicinfo : RefCV_MemblkAlloc fail ", NULL, NULL, NULL, NULL, NULL);
    } else {
        AmbaMisra_TypeCast(&pDataIn, &handle->in_buf[index].buf[0].pBuffer);
        ret = AmbaWrap_memcpy(pDataIn, in, sizeof(cv_pic_info_t));
        if(ret != 0U) {
            AmbaPrint_PrintStr5("RefCV_NetFlexidagRunPicinfo : AmbaWrap_memcpy fail ", NULL, NULL, NULL, NULL, NULL);
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
                AmbaPrint_PrintStr5("RefCV_NetFlexidagRunPicinfo : RefCV_MemblkClean fail ", NULL, NULL, NULL, NULL, NULL);
                ret = 1U;
            } else {
                handle->out_buf[index].num_of_buf = fd_gen_handle->mem_req.flexidag_num_outputs;
                for(i = 0U; i < handle->out_buf[index].num_of_buf; i++) {
                    if(handle->out_buf[index].buf[i].pBuffer == NULL) {
                        ret = RefCV_MemblkAlloc(fd_gen_handle->mem_req.flexidag_output_buffer_size[i], &handle->out_buf[index].buf[i]);
                        //AmbaPrint_PrintUInt5("RefCV_MemblkAlloc out_buf[%d].buf[%d] pa = 0x%x size = 0x%x ",index, i, handle->out_buf[index].buf[i].buffer_daddr, handle->out_buf[index].buf[i].buffer_size, 0U);
                    }

                    if (ret != 0U) {
                        break;
                    }
                }

                if (ret == 0U) {
                    ret = AmbaCV_FlexidagRunNonBlock(fd_gen_handle, cb, cb_param, &handle->in_buf[index], &handle->out_buf[index], token_id);
                    if (ret != 0U) {
                        AmbaPrint_PrintStr5("RefCV_NetFlexidagRunPicinfo : AmbaCV_FlexidagRun fail ", NULL, NULL, NULL, NULL, NULL);
                        ret = 1U;
                    }
                }
            }
        }
    }
    return ret;
}

static uint32_t RefCV_NetFlexidagRunRaw(REF_CV_NET_HANDLE_s *handle, uint32_t frame_id, flexidag_cb cb, void *cb_param, const memio_source_recv_multi_raw_t *in, uint32_t *token_id)
{
    uint32_t ret = 0U, i;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    memio_source_recv_raw_t *pDataIn;
    uint32_t index = frame_id%REF_CV_BUF_NUM;

    handle->in_buf[index].num_of_buf = in->num_io;
    for(i = 0U ; i < in->num_io ; i++) {
        if(handle->in_buf[index].buf[i].pBuffer == NULL) {
            ret = RefCV_MemblkAlloc(sizeof(memio_source_recv_raw_t), &handle->in_buf[index].buf[i]);
        }

        if (ret != 0U) {
            AmbaPrint_PrintStr5("RefCV_NetFlexidagRunRaw : RefCV_MemblkAlloc in_buf fail", NULL, NULL, NULL, NULL, NULL);
            ret = 1U;
            break;
        } else {
            AmbaMisra_TypeCast(&pDataIn, &handle->in_buf[index].buf[i].pBuffer);
            ret |= AmbaWrap_memcpy(pDataIn, &in->io[i], sizeof(memio_source_recv_raw_t));
            ret |= RefCV_MemblkClean(&handle->in_buf[index].buf[i]);
        }
    }

    if(ret == 0U) {
        handle->out_buf[index].num_of_buf = fd_gen_handle->mem_req.flexidag_num_outputs;
        for(i = 0U; i < fd_gen_handle->mem_req.flexidag_num_outputs; i++) {
            if(handle->out_buf[index].buf[i].pBuffer == NULL) {
                ret = RefCV_MemblkAlloc(fd_gen_handle->mem_req.flexidag_output_buffer_size[i], &handle->out_buf[index].buf[i]);
            }

            if (ret != 0U) {
                AmbaPrint_PrintStr5("RefCV_NetFlexidagRunRaw : RefCV_MemblkAlloc out_buf fail", NULL, NULL, NULL, NULL, NULL);
                ret = 1U;
                break;
            }
        }

        if (ret == 0U) {
            ret = AmbaCV_FlexidagRunNonBlock(fd_gen_handle, cb, cb_param, &handle->in_buf[index], &handle->out_buf[index], token_id);
            if (ret != 0U) {
                AmbaPrint_PrintStr5("RefCV_NetFlexidagRunRaw : AmbaCV_FlexidagRun fail", NULL, NULL, NULL, NULL, NULL);
                ret = 1U;
            }
        }
    }

    return ret;
}

static uint32_t RefCV_NetFlexidagClose(REF_CV_NET_HANDLE_s *handle)
{
    uint32_t ret = 0U;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;

    ret = AmbaCV_FlexidagClose(fd_gen_handle);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("RefCV_NetFlexidagClose : AmbaCV_FlexidagClose fail", 0U, 0U, 0U, 0U, 0U);
        ret = 1U;
    }
    return ret;
}

static uint32_t RefCV_NetResultRaw(uint32_t id,uint32_t frame_id, const AMBA_CV_FLEXIDAG_IO_s *pOut, const flexidag_memblk_t *pResultBlk, uint32_t output_file)
{
    uint32_t Rval = 0U,crc_out = 0U,crc_result = 0U,i,ret = 0U;
    char out_path[128];
    AMBA_FS_FILE *Fp = NULL;
    uint32_t NumSuccess,len;
    const flexidag_memblk_t *pOutBlk;
    char *pResultAddr;
    const uint8_t *ptr;

    if(output_file == 1U) {
        len = 0U;
        len = RefCV_NetSprintfStr(&out_path[len], output_path);
        len += RefCV_NetSprintfUint2(&out_path[len], sizeof(out_path), "flexidag_out%d_%d_%d.bin", id, frame_id);
        ret = AmbaFS_FileOpen(out_path, "wb", &Fp);
        if(ret != 0U) {
            AmbaPrint_PrintUInt5("RefCV_NetResultRaw AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    pResultAddr = pResultBlk->pBuffer;
    for(i = 0U; i < pOut->num_of_buf; i++) {
        pOutBlk = &pOut->buf[i];

        AmbaMisra_TypeCast(&ptr, &pOutBlk->pBuffer);
        crc_out = AmbaUtility_Crc32(ptr, pOutBlk->buffer_size);
        AmbaMisra_TypeCast(&ptr, &pResultAddr);
        crc_result = AmbaUtility_Crc32(ptr, pOutBlk->buffer_size);

        if(output_file == 1U) {
            ret = AmbaFS_FileWrite(pOutBlk->pBuffer, 1U, pOutBlk->buffer_size, Fp, &NumSuccess);
            if( (ret !=  0U) || (NumSuccess != pOutBlk->buffer_size)) {
                AmbaPrint_PrintUInt5("RefCV_NetResultRaw AmbaFS_FileWrite fail NumSuccess = 0x%x buffer_size = 0x%x", NumSuccess, pOutBlk->buffer_size, 0U, 0U, 0U);
            }
        }
        if(crc_out != crc_result) {
            Rval = 1U;
            AmbaPrint_PrintUInt5("RefCV_NetResultRaw id (%d) frame (%d) out (%d) error crc_out (%x) crc_result (%x)",id, frame_id, i, crc_out, crc_result);
        }
        pResultAddr = &pResultAddr[pOutBlk->buffer_size];
    }
    if(output_file == 1U) {
        ret |= AmbaFS_FileClose(Fp);
    }
    (void)ret;
    return Rval;
}


static void* RefCV_NetResultMain(void *arg)
{
    uint32_t ret,ret1 = 0U,buf_index,raw_index,i;
    REF_CV_NET_PARAMS_s  *param;
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info;
    REF_CV_NET_FRAME_s frame;

    (void) arg;
    AmbaMisra_TypeCast(&param, &arg);
    param->ResultFlag = 1U;
    while(net_task_result_loop == 1U) {
        ret1 |= AmbaKAL_MsgQueueReceive(&param->result_msg_queue, &frame, AMBA_KAL_WAIT_FOREVER);
        ret1 |= AmbaWrap_memset(&run_info,0,sizeof(run_info));
        ret1 |= AmbaCV_FlexidagWaitRunFinish(&param->handle.fd_gen_handle, frame.token_id, &run_info);
        ret1 |= AmbaKAL_MsgQueueSend(&param->msg_queue, &ret, AMBA_KAL_NO_WAIT);
        if(run_info.output_not_generated == 0U) {
            AmbaPrint_PrintUInt5("RefCV_NetResultMain (%d) frame_id = %d cv_run_time = %d us, over_head = %d us",param->id, frame.frame_id, run_info.end_time - run_info.start_time, ((run_info.api_end_time - run_info.api_start_time) - (run_info.end_time - run_info.start_time)), 0U);
            buf_index = frame.frame_id%REF_CV_BUF_NUM;
            raw_index = frame.frame_id%(param->handle.num_raw);
            if(param->handle.output_from_arm == 0U) {
                for(i = 0U; i < param->handle.out_buf[buf_index].num_of_buf; i++) {
                    ret1 |= RefCV_MemblkInvalid(&param->handle.out_buf[buf_index].buf[i]);
                }
            }
            if(param->handle.output_type == OUTPUT_TYPE_RAW) {
                ret = RefCV_NetResultRaw(param->id, frame.frame_id,&param->handle.out_buf[buf_index], &param->handle.result_buf[raw_index],param->output_file);
            } else {
                AmbaPrint_PrintUInt5("RefCV_NetResultMain (%d) output result not support",param->id, 0U, 0U, 0U, 0U);
                ret = 1U;
            }
            if(ret != 0U) {
                param->num_err = param->num_err + 1U;
            }
            param->cur_runs = param->cur_runs + 1U;
            ret1 |= AmbaWrap_memset(param->handle.out_buf[buf_index].buf[0].pBuffer, 0x0, param->handle.out_buf[buf_index].buf[0].buffer_size);
            ret1 |= RefCV_MemblkClean(&param->handle.out_buf[buf_index].buf[0]);
        }
        if( (frame.frame_id + 1U) == param->num_runs) {
            param->ResultFlag = 0U;
            AmbaPrint_PrintUInt5("RefCV_NetResultMain (%d) finish ",param->id, 0U, 0U, 0U, 0U);
        }
        (void) ret1;
    }

    return NULL;
}

static void* RefCV_NetRun(void *arg)
{
    uint32_t i,j,ret,ret1 = 0U,raw_index;
    REF_CV_NET_PARAMS_s  *param;
    memio_source_recv_picinfo_t memio_pic_info = {0};
    cv_pic_info_t *pic_info = &memio_pic_info.pic_info;
    memio_source_recv_multi_raw_t raw;
    char log_path[128];
    uint32_t token_id = 0U, len;
    void *cb_param;
    REF_CV_NET_FRAME_s frame;
    uint32_t YuvWidth,YuvHeight,YuvPitch;

    (void) arg;
    AmbaMisra_TypeCast(&param, &arg);
    param->RunFlag  = 1U;
    AmbaPrint_PrintUInt5("RefCV_NetRun (%d) start ",param->id, 0U, 0U, 0U, 0U);

    YuvWidth = param->handle.raw_width;
    YuvHeight = param->handle.raw_height;
    YuvPitch = param->handle.raw_pitch;
    pic_info->capture_time = 0U;
    pic_info->channel_id = 0U;
    pic_info->frame_num = 0U;
    pic_info->pyramid.image_width_m1 = (uint16_t)(YuvWidth - 1U);
    pic_info->pyramid.image_height_m1 = (uint16_t)(YuvHeight - 1U);
    pic_info->pyramid.image_pitch_m1 = (YuvWidth - 1U);

    for (i=0U ; i<5U ; i++) {
        pic_info->pyramid.half_octave[i].ctrl.roi_pitch = (uint16_t)YuvPitch;
        pic_info->pyramid.half_octave[i].roi_start_col = (int16_t)0;
        pic_info->pyramid.half_octave[i].roi_start_row = (int16_t)0;
        pic_info->pyramid.half_octave[i].roi_width_m1 = (uint16_t)(YuvWidth - 1U);
        pic_info->pyramid.half_octave[i].roi_height_m1 = (uint16_t)(YuvHeight - 1U);
        pic_info->pyramid.half_octave[i].ctrl.disable = 0U;
    }

    ret = RefCV_NetFlexidagInit(&param->handle);
    if(ret != 0U) {
        AmbaPrint_PrintUInt5("RefCV_NetRun (%d) RefCV_NetFlexidagInit fail ",param->id, 0U, 0U, 0U, 0U);
    } else {
        for (i = 0U; i < param->num_runs; i++) {
            raw_index = i%(param->handle.num_raw);
            AmbaMisra_TypeCast(&cb_param, &param);

            if(param->handle.memio_type == REF_MEMIO_PICINFO) {
                for (j=0U ; j<5U ; j++) {
                    pic_info->rpLumaLeft[j] = param->handle.raw_buf_y[raw_index].buffer_daddr;
                    pic_info->rpChromaLeft[j] = param->handle.raw_buf_y[raw_index].buffer_daddr + (YuvPitch * YuvHeight);
                    pic_info->rpLumaRight[j] = pic_info->rpLumaLeft[j];
                    pic_info->rpChromaRight[j] = pic_info->rpChromaLeft[j];
                }
                ret = RefCV_NetFlexidagRunPicinfo(&param->handle, i, NULL, cb_param, pic_info, &token_id);
            } else if (param->handle.memio_type == REF_MEMIO_RAW) {
                raw.num_io = param->handle.num_per_raw;
                raw.io[0].addr = param->handle.raw_buf_y[raw_index].buffer_daddr;
                raw.io[0].size = param->handle.raw_buf_y[raw_index].buffer_size;
                raw.io[0].pitch = 0U;
                if(param->handle.num_per_raw == 2U) {
                    raw.io[1].addr = param->handle.raw_buf_uv[raw_index].buffer_daddr;
                    raw.io[1].size = param->handle.raw_buf_uv[raw_index].buffer_size;
                    raw.io[1].pitch = 0U;
                }
                ret = RefCV_NetFlexidagRunRaw(&param->handle, i, NULL, cb_param, &raw, &token_id);
            } else {
                AmbaPrint_PrintUInt5("RefCV_NetRun (%d) RefCV_NetFlexidagRun memio_type is not support ",param->id, ret, 0U, 0U, 0U);
                ret = 1U;
            }
            if (ret == 0U) {
                frame.frame_id = i;
                frame.token_id = token_id;
                ret1 |= AmbaKAL_MsgQueueSend(&param->result_msg_queue, &frame, AMBA_KAL_NO_WAIT);
                ret1 |= AmbaKAL_MsgQueueReceive(&param->msg_queue, &ret, 1000);
            } else {
                AmbaPrint_PrintUInt5("RefCV_NetRun (%d) RefCV_NetFlexidagRun fail ret = %x ",param->id, ret, 0U, 0U, 0U);
            }
        }
        while(param->ResultFlag != 0U) {
            ret1 |= AmbaKAL_TaskSleep(100U);
        }
        len = 0U;
        len = RefCV_NetSprintfStr(&log_path[len], output_path);
        ret1 |= RefCV_NetSprintfUint1(&log_path[len], sizeof(log_path), "flexidag_log%d.txt", param->id);
        AmbaPrint_PrintStr5("RefCV_NetRun (%d) AmbaCV_FlexidagDumpLog %s ",log_path, NULL, NULL, NULL, NULL);
        ret = AmbaCV_FlexidagDumpLog(&param->handle.fd_gen_handle,log_path, FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("RefCV_NetRun (%d) AmbaCV_FlexidagDumpLog fail ",param->id, 0U, 0U, 0U, 0U);
        }

        ret = RefCV_NetFlexidagClose(&param->handle);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("RefCV_NetRun (%d) AmbaCV_FlexidagClose fail ",param->id, 0U, 0U, 0U, 0U);
        }
        (void) ret1;
    }

    param->RunFlag = 0U;
    while(net_task_loop == 1U) {
        ret1 |= AmbaKAL_TaskSleep(1000U);
    }
    (void) ret1;

    return NULL;
}

static void RefCV_NetStop(void)
{
    uint32_t i,ret = 0U,ret1 = 0U, all_stop = 0U;
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
    AMBA_KAL_TASK_INFO_s taskInfo;
#endif

    do {
        uint32_t still_run = 0U;
        for(i = 0U; i < num_of_thread; i ++) {
            if( net_thread_params[i].RunFlag != 0U ) {
                still_run = 1U;
                break;
            }
        }
        if(still_run == 0U) {
            all_stop = 1U;
        }
        ret1 |= AmbaKAL_TaskSleep(250U); /* Sleep 250ms */
    } while (all_stop == 0U);

    for(i = 0U; i < num_of_thread; i ++) {
        AmbaPrint_PrintUInt5("RefCV_NetMain thread (%d) total (%d) error (%d) ",net_thread_params[i].id, net_thread_params[i].cur_runs, net_thread_params[i].num_err, 0U, 0U);
    }

    for(i = 0U; i < num_of_thread; i ++) {
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
        ret1 |= AmbaKAL_TaskQuery(&net_thread_params[i].task, &taskInfo);
        if ((taskInfo.TaskState != TX_COMPLETED) && (taskInfo.TaskState != TX_TERMINATED)) {
            ret  = AmbaKAL_TaskTerminate(&net_thread_params[i].task);
            if ( ret  != KAL_ERR_NONE ) {
                AmbaPrint_PrintUInt5("RefCV_NetMain AmbaKAL_TaskTerminate fail (%d)", ret, 0U, 0U, 0U, 0U);
            }
        }
#endif
        ret = AmbaKAL_TaskDelete(&net_thread_params[i].task);
        if ( ret != KAL_ERR_NONE ) {
            AmbaPrint_PrintUInt5("RefCV_NetMain AmbaKAL_TaskDelete fail (%d)", ret, 0U, 0U, 0U, 0U);
        }

#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
        ret1 |= AmbaKAL_TaskQuery(&net_thread_params[i].result_task, &taskInfo);
        if ((taskInfo.TaskState != TX_COMPLETED) && (taskInfo.TaskState != TX_TERMINATED)) {
            ret  = AmbaKAL_TaskTerminate(&net_thread_params[i].result_task);
            if ( ret  != KAL_ERR_NONE ) {
                AmbaPrint_PrintUInt5("RefCV_NetMain AmbaKAL_TaskTerminate fail (%d)", ret, 0U, 0U, 0U, 0U);
            }
        }
#endif
        ret = AmbaKAL_TaskDelete(&net_thread_params[i].result_task);
        if ( ret != KAL_ERR_NONE ) {
            AmbaPrint_PrintUInt5("RefCV_NetMain AmbaKAL_TaskDelete fail (%d)", ret, 0U, 0U, 0U, 0U);
        }
        (void) ret1;
    }
    net_task_loop = 0U;
    net_task_result_loop = 0U;
}

static void* RefCV_NetMain(void *args)
{
    uint32_t i,ret = 0U,ret1 = 0U;
    static uint8_t refcv_net_stack[MAX_THREAD_NUM][0x4000] GNU_SECTION_NOZEROINIT;
    static uint8_t refcv_net_result_stack[MAX_THREAD_NUM][0x4000] GNU_SECTION_NOZEROINIT;
    const REF_CV_NET_PARAMS_s *para_ptr;
    char task_name[32] =    "RevCV_Net";

    (void) args;
    AmbaMisra_TouchUnused(args);
    for(i = 0U; i < num_of_thread; i ++) {
        para_ptr = &net_thread_params[i];
        //task create
        net_task_loop = 1U;
        ret = AmbaKAL_TaskCreate(&net_thread_params[i].task,                         /* pTask */
                                 task_name,                                                      /* pTaskName */
                                 100,     /* Priority */
                                 RefCV_NetRun,                                                 /* void (*EntryFunction)(uint32_t) */
                                 para_ptr,                                                   /* EntryArg */
                                 &refcv_net_stack[i][0],                                            /* pStackBase */
                                 sizeof(refcv_net_stack[i]),                                        /* StackByteSize */
                                 0U);
        if(ret == 0U) {
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
            ret1 |= AmbaKAL_TaskSetSmpAffinity(&net_thread_params[i].task, 0x1U);
#endif
            ret1 |= AmbaKAL_TaskResume(&net_thread_params[i].task);
        } else {
            AmbaPrint_PrintUInt5("RefCV_NetMain AmbaKAL_TaskCreate task fail (%d)", ret, 0U, 0U, 0U, 0U);
        }

        //result task create
        net_task_result_loop = 1U;
        ret = AmbaKAL_TaskCreate(&net_thread_params[i].result_task,                         /* pTask */
                                 task_name,                                                      /* pTaskName */
                                 100,     /* Priority */
                                 RefCV_NetResultMain,                                                 /* void (*EntryFunction)(uint32_t) */
                                 para_ptr,                                                   /* EntryArg */
                                 &refcv_net_result_stack[i][0],                                            /* pStackBase */
                                 sizeof(refcv_net_result_stack[i]),                                        /* StackByteSize */
                                 0U);
        if(ret == 0U) {
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
            ret1 |= AmbaKAL_TaskSetSmpAffinity(&net_thread_params[i].result_task, 0x1U);
#endif
            ret1 |= AmbaKAL_TaskResume(&net_thread_params[i].result_task);
        } else {
            AmbaPrint_PrintUInt5("RefCV_NetMain AmbaKAL_TaskCreate result_task fail (%d)", ret, 0U, 0U, 0U, 0U);
        }
        (void) ret1;
    }

    ret1 |= AmbaKAL_TaskSleep(200U);
    RefCV_NetStop();

    AmbaPrint_PrintUInt5("RefCV_NetMain end ", 0U, 0U, 0U, 0U, 0U);
    (void) ret1;
    return NULL;
}

static void RefCV_NetGetFileSize(const char* path, uint32_t* Size)
{
    uint32_t Ret;
    uint32_t Fsize = 0U;
    uint64_t Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("RefCV_NetGetFileSize AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("RefCV_NetGetFileSize AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("RefCV_NetGetFileSize AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            Fsize = (uint32_t)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("RefCV_NetGetFileSize AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    *Size = Fsize;
}

static void RefCV_NetLoadFile(const char* path, const flexidag_memblk_t* blk)
{
    uint32_t Ret;
    uint32_t Fsize = 0U, OpRes;
    uint64_t Fpos = 0U;
    AMBA_FS_FILE *Fp;

    (void) blk;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintStr5("RefCV_NetLoadFile AmbaFS_FileOpen %s fail ", path, NULL, NULL, NULL, NULL);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintStr5("RefCV_NetLoadFile AmbaFS_FileSeek %s fail ", path, NULL, NULL, NULL, NULL);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintStr5("RefCV_NetLoadFile AmbaFS_FileTell %s fail ", path, NULL, NULL, NULL, NULL);
        }
    }
    if (Ret == 0U) {
        Fsize = (uint32_t)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            AmbaPrint_PrintStr5("RefCV_NetLoadFile AmbaFS_FileSeek %s fail ", path, NULL, NULL, NULL, NULL);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileRead(blk->pBuffer, 1U, (uint32_t)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintStr5("RefCV_NetLoadFile AmbaFS_FileRead %s fail ", path, NULL, NULL, NULL, NULL);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintStr5("RefCV_NetLoadFile AmbaFS_FileClose %s fail ", path, NULL, NULL, NULL, NULL);
        }
    }
    if (Ret == 0U) {
        Ret = RefCV_MemblkClean(blk);
        if(Ret !=  0U) {
            AmbaPrint_PrintStr5("RefCV_NetLoadFile RefCV_MemblkClean %s fail ", path, NULL, NULL, NULL, NULL);
        }
    }
}

static void RefCV_NetParamInit(REF_CV_NET_PARAMS_s *params,uint32_t id)
{
    static char name[32] = "RevCV_Net";
    uint32_t ret1 = 0U;

    params->id = id;
    params->cur_runs = 0U;
    params->num_err = 0U;
    params->RunFlag = 0U;
    params->ResultFlag = 0U;
    ret1 |= AmbaWrap_memset(&params->handle, 0x0, sizeof(REF_CV_NET_HANDLE_s));
    if(params->init == 0U) {
        ret1 |= AmbaKAL_MsgQueueCreate(&params->msg_queue, name, sizeof(uint32_t), params->queue_buffer, sizeof(params->queue_buffer));
        ret1 |= AmbaKAL_MsgQueueCreate(&params->result_msg_queue, name, sizeof(REF_CV_NET_FRAME_s), params->result_queue_buffer, sizeof(params->result_queue_buffer));
        params->init = 1U;
    }
    (void) ret1;
}

static void RefCV_NetLoad(const char* path,flexidag_memblk_t *blk)
{
    uint32_t buf_size;
    uint32_t ret1 = 0U;

    RefCV_NetGetFileSize(path, &buf_size);
    ret1 |= RefCV_MemblkAlloc(buf_size, blk);
    RefCV_NetLoadFile(path, blk);

    (void) ret1;
}

static int32_t RefCV_RawSubParser1(REF_CV_NET_PARAMS_s *params,const char *pToken,const char *pValue)
{
    int32_t compare = 0;
    uint32_t value;
    uint32_t ret1 = 0U;

    if (AmbaUtility_StringCompare(pToken, "RAW_PITCH", 9U) == 0) {
        ret1 |= AmbaUtility_StringToUInt32(pValue, &value);
        params->handle.raw_pitch = value;
    } else if (AmbaUtility_StringCompare(pToken, "RAW_WIDTH", 9U) == 0) {
        ret1 |= AmbaUtility_StringToUInt32(pValue, &value);
        params->handle.raw_width = value;
    } else if (AmbaUtility_StringCompare(pToken, "RAW_HEIGHT", 9U) == 0) {
        ret1 |= AmbaUtility_StringToUInt32(pValue, &value);
        params->handle.raw_height = value;
    } else if (AmbaUtility_StringCompare(pToken, "RAW0_Y_PATH", 11U) == 0) {
        if(AmbaUtility_StringCompare(pValue, "NULL", 4U) != 0) {
            RefCV_NetLoad(pValue,&params->handle.raw_buf_y[0]);
            params->handle.num_raw = params->handle.num_raw + 1U;
            params->handle.num_per_raw = params->handle.num_per_raw + 1U;
        }
    } else if (AmbaUtility_StringCompare(pToken, "RAW0_UV_PATH", 12U) == 0) {
        if(AmbaUtility_StringCompare(pValue, "NULL", 4U) != 0) {
            RefCV_NetLoad(pValue,&params->handle.raw_buf_uv[0]);
            params->handle.num_per_raw = params->handle.num_per_raw + 1U;
        }
    } else if (AmbaUtility_StringCompare(pToken, "RAW1_Y_PATH", 11U) == 0) {
        if(AmbaUtility_StringCompare(pValue, "NULL", 4U) != 0) {
            RefCV_NetLoad(pValue,&params->handle.raw_buf_y[1]);
            params->handle.num_raw = params->handle.num_raw + 1U;
        }
    } else if (AmbaUtility_StringCompare(pToken, "RAW1_UV_PATH", 12U) == 0) {
        if(AmbaUtility_StringCompare(pValue, "NULL", 4U) != 0) {
            RefCV_NetLoad(pValue,&params->handle.raw_buf_uv[1]);
        }
    } else {
        compare = 1;
    }

    (void) ret1;
    return compare;

}

static int32_t RefCV_RawSubParser2(REF_CV_NET_PARAMS_s *params,const char *pToken,const char *pValue)
{
    int32_t compare = 0;

    if (AmbaUtility_StringCompare(pToken, "RAW2_Y_PATH", 11U) == 0) {
        if(AmbaUtility_StringCompare(pValue, "NULL", 4U) != 0) {
            RefCV_NetLoad(pValue,&params->handle.raw_buf_y[2]);
            params->handle.num_raw = params->handle.num_raw + 1U;
        }
    } else if (AmbaUtility_StringCompare(pToken, "RAW2_UV_PATH", 12U) == 0) {
        if(AmbaUtility_StringCompare(pValue, "NULL", 4U) != 0) {
            RefCV_NetLoad(pValue,&params->handle.raw_buf_uv[2]);
        }
    } else if (AmbaUtility_StringCompare(pToken, "RAW3_Y_PATH", 11U) == 0) {
        if(AmbaUtility_StringCompare(pValue, "NULL", 4U) != 0) {
            RefCV_NetLoad(pValue,&params->handle.raw_buf_y[3]);
            params->handle.num_raw = params->handle.num_raw + 1U;
        }
    } else if (AmbaUtility_StringCompare(pToken, "RAW3_UV_PATH", 12U) == 0) {
        if(AmbaUtility_StringCompare(pValue, "NULL", 4U) != 0) {
            RefCV_NetLoad(pValue,&params->handle.raw_buf_uv[3]);
        }
    } else {
        compare = 1;
    }

    return compare;

}

static int32_t RefCV_RawParser(REF_CV_NET_PARAMS_s *params,const char *pToken,const char *pValue)
{
    int32_t compare = 0;

    if (RefCV_RawSubParser1(params, pToken, pValue) == 0) {

    } else if (RefCV_RawSubParser2(params, pToken, pValue) == 0) {

    } else {
        compare = 1;
    }

    return compare;
}

static int32_t RefCV_ResultParser(REF_CV_NET_PARAMS_s *params,const char *pToken,const char *pValue)
{
    int32_t compare = 0;

    if (AmbaUtility_StringCompare(pToken, "RESULT0_PATH", 12U) == 0) {
        if(AmbaUtility_StringCompare(pValue, "NULL", 4U) != 0) {
            RefCV_NetLoad(pValue,&params->handle.result_buf[0]);
        }
    } else if (AmbaUtility_StringCompare(pToken, "RESULT1_PATH", 12U) == 0) {
        if(AmbaUtility_StringCompare(pValue, "NULL", 4U) != 0) {
            RefCV_NetLoad(pValue,&params->handle.result_buf[1]);
        }
    } else if (AmbaUtility_StringCompare(pToken, "RESULT2_PATH", 12U) == 0) {
        if(AmbaUtility_StringCompare(pValue, "NULL", 4U) != 0) {
            RefCV_NetLoad(pValue,&params->handle.result_buf[2]);
        }
    } else if (AmbaUtility_StringCompare(pToken, "RESULT3_PATH", 12U) == 0) {
        if(AmbaUtility_StringCompare(pValue, "NULL", 4U) != 0) {
            RefCV_NetLoad(pValue,&params->handle.result_buf[3]);
        }
    } else {
        compare = 1;
    }

    return compare;
}

static int32_t RefCV_OutputParser(REF_CV_NET_PARAMS_s *params,const char *pToken,const char *pValue)
{
    int32_t compare = 0;
    uint32_t value;
    uint32_t ret1 = 0U;

    if (AmbaUtility_StringCompare(pToken, "OUTPUT_FILE", 11U) == 0) {
        ret1 |= AmbaUtility_StringToUInt32(pValue, &value);
        params->output_file = value;
    } else if (AmbaUtility_StringCompare(pToken, "OUTPUT_FROM_ARM", 15U) == 0) {
        ret1 |= AmbaUtility_StringToUInt32(pValue, &value);
        params->handle.output_from_arm = value;
    } else if (AmbaUtility_StringCompare(pToken, "OUTPUT_TYPE", 11U) == 0) {
        ret1 |= AmbaUtility_StringToUInt32(pValue, &value);
        if(value >= OUTPUT_TYPE_MAX) {
            AmbaPrint_PrintUInt5("RefCV_NetContextParser : OUTPUT_TYPE error", 0U, 0U, 0U, 0U, 0U);
        } else {
            params->handle.output_type = value;
        }
    } else if (AmbaUtility_StringCompare(pToken, "OUTPUT_PATH", 11U) == 0) {
        AmbaUtility_StringCopy(output_path, sizeof(output_path), pValue);
        AmbaPrint_PrintStr5("RefCV_NetContextParser : output_path %s", output_path, NULL, NULL, NULL, NULL);
    } else {
        compare = 1;
    }

    (void) ret1;
    return compare;
}

static uint32_t RefCV_NetContextParser(const char* path,REF_CV_NET_PARAMS_s *params)
{
    uint32_t Rval;
    uint32_t i, j = 0U,value;
    uint32_t ReadSize, ByteRead, ArgCount;
    AMBA_FS_FILE *File = NULL;
    AMBA_FS_FILE_INFO_s FileInfo;
    char FileName[128U] = {'\0'};
    const char FileOpMode[3U] = "r";
    char ReadBuffer[READ_BUF_SIZE + 32U];
    char LineBuffer[256U];
    char TokenBuffer[MAX_STR_TOKEN_NUMBER][MAX_STR_TOKEN_LENGTH];
    uint32_t ret1 = 0U;

    AmbaUtility_StringCopy(FileName, sizeof(FileName), path);
    Rval = AmbaFS_FileOpen(FileName, FileOpMode, &File);
    if (File == NULL) {
        AmbaPrint_PrintStr5("RefCV_NetContextParser : %s open failed!!.", FileName, NULL, NULL, NULL, NULL);
    } else {
        ret1 |= AmbaFS_GetFileInfo(FileName, &FileInfo);
        ret1 |= AmbaWrap_memset(ReadBuffer, 0, sizeof(ReadBuffer));
        ret1 |= AmbaWrap_memset(TokenBuffer, 0, sizeof(TokenBuffer));
        ret1 |= AmbaWrap_memset(LineBuffer, 0, sizeof(LineBuffer));
        ReadSize = (uint32_t)FileInfo.Size;

        if (ReadSize > READ_BUF_SIZE) {
            AmbaPrint_PrintUInt5("RefCV_NetContextParser : Buffer size should be %u", ReadSize, 0U, 0U, 0U, 0U);
            ReadSize = READ_BUF_SIZE;
        }

        ret1 |= AmbaFS_FileRead(ReadBuffer, 1U, ReadSize, File, &ByteRead);
        ret1 |= AmbaFS_FileClose(File);
        if ((ReadSize > 0U) && (ByteRead > 0U)) {
            for (i = 0U; i < ReadSize; i++) {
                if(ReadBuffer[i] != '\n') {
                    if(ReadBuffer[i] != (char)0xD) {
                        LineBuffer[j] = ReadBuffer[i];
                        j++;
                    }
                } else {
                    LineBuffer[j] = '\0';
                    /* Parse the input string to multiple tokens */
                    ret1 |= AmbaUtility_StringToken(LineBuffer, ' ', TokenBuffer, &ArgCount);
                    if (AmbaUtility_StringCompare(TokenBuffer[0U], "NUM_RUNS", 8U) == 0) {
                        ret1 |= AmbaUtility_StringToUInt32(TokenBuffer[1U], &value);
                        params->num_runs = value;
                    } else if (AmbaUtility_StringCompare(TokenBuffer[0U], "MEMIO_TYPE", 10U) == 0) {
                        ret1 |= AmbaUtility_StringToUInt32(TokenBuffer[1U], &value);
                        if(value >= REF_MEMIO_MAX) {
                            AmbaPrint_PrintUInt5("RefCV_NetContextParser : MEMIO_TYPE error", 0U, 0U, 0U, 0U, 0U);
                        } else {
                            params->handle.memio_type = value;
                        }
                    } else if (AmbaUtility_StringCompare(TokenBuffer[0U], "FLEXIDAG_PATH", 13U) == 0) {
                        RefCV_NetLoad(TokenBuffer[1U],&params->handle.bin_buf);
                    } else if (RefCV_RawParser(params, TokenBuffer[0U], TokenBuffer[1U]) == 0) {

                    } else if (RefCV_ResultParser(params, TokenBuffer[0U], TokenBuffer[1U]) == 0) {

                    } else if (RefCV_OutputParser(params, TokenBuffer[0U], TokenBuffer[1U]) == 0) {

                    } else {
                        AmbaPrint_PrintStr5("RefCV_NetContextParser : [ERROR] Unkown pattern %s ", TokenBuffer[0U], NULL, NULL, NULL, NULL);
                        Rval = 1U;
                    }
                    ret1 |= AmbaWrap_memset(TokenBuffer, 0, sizeof(TokenBuffer));
                    ret1 |= AmbaWrap_memset(LineBuffer, 0, sizeof(LineBuffer));
                    j = 0U;
                }
            }
        }
    }
    (void) ret1;
    return Rval;
}

static uint32_t RefCV_NetScriptParser(const char* path,uint32_t *pnum_of_thread)
{
    uint32_t Rval;
    uint32_t i, j = 0U;
    uint32_t ReadSize, ByteRead;
    AMBA_FS_FILE *File = NULL;
    AMBA_FS_FILE_INFO_s FileInfo;
    char FileName[128U] = {'\0'};
    const char FileOpMode[3U] = "r";
    char ReadBuffer[READ_BUF_SIZE + 32U];
    char LineBuffer[256U];
    char TokenBuffer[MAX_STR_TOKEN_NUMBER][MAX_STR_TOKEN_LENGTH];
    uint32_t ret1 = 0U;

    *pnum_of_thread = 0U;
    AmbaUtility_StringCopy(FileName, sizeof(FileName), path);
    Rval = AmbaFS_FileOpen(FileName, FileOpMode, &File);
    if (File == NULL) {
        AmbaPrint_PrintStr5("RefCV_NetScriptParser : %s open failed!", FileName, NULL, NULL, NULL, NULL);
    } else {
        ret1 |= AmbaFS_GetFileInfo(FileName, &FileInfo);
        ret1 |= AmbaWrap_memset(ReadBuffer, 0, sizeof(ReadBuffer));
        ret1 |= AmbaWrap_memset(TokenBuffer, 0, sizeof(TokenBuffer));
        ret1 |= AmbaWrap_memset(LineBuffer, 0, sizeof(LineBuffer));
        ReadSize = (uint32_t)FileInfo.Size;

        if (ReadSize > READ_BUF_SIZE) {
            AmbaPrint_PrintUInt5("RefCV_NetScriptParser : Buffer size should be %u", ReadSize, 0U, 0U, 0U, 0U);
            ReadSize = READ_BUF_SIZE;
        }

        ret1 |= AmbaFS_FileRead(ReadBuffer, 1U, ReadSize, File, &ByteRead);
        ret1 |= AmbaFS_FileClose(File);
        if ((ReadSize > 0U) && (ByteRead > 0U)) {
            for (i = 0U; i < ReadSize; i++) {
                if(ReadBuffer[i] != '\n') {
                    if(ReadBuffer[i] != (char)0xD) {
                        LineBuffer[j] = ReadBuffer[i];
                        j++;
                    }
                } else {
                    LineBuffer[j] = '\0';
                    AmbaPrint_PrintStr5("RefCV_NetScriptParser : file = %s", LineBuffer, NULL, NULL, NULL, NULL);
                    if(*pnum_of_thread < MAX_THREAD_NUM) {
                        RefCV_NetParamInit(&net_thread_params[*pnum_of_thread],*pnum_of_thread);
                        ret1 |= RefCV_NetContextParser(&LineBuffer[0],&net_thread_params[*pnum_of_thread]);
                    } else {
                        AmbaPrint_PrintUInt5("RefCV_NetScriptParser : file over MAX_THREAD_NUM(%d)",MAX_THREAD_NUM, 0U, 0U, 0U,0U);
                    }
                    ret1 |= AmbaWrap_memset(LineBuffer, 0, sizeof(LineBuffer));
                    j = 0U;
                    *pnum_of_thread = *pnum_of_thread + 1U;
                }
            }
        }
    }

    (void) ret1;
    return Rval;
}

void RefCV_Net(const char* path)
{
    uint32_t Rval = 0U,max_temp_size = 0U,i;
    static uint8_t refcv_net_main_stack[0x4000] GNU_SECTION_NOZEROINIT;
    static AMBA_KAL_TASK_t main_task;
    static uint8_t main_task_init = 0U;
    char task_name[32] =  "net_main";
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
    AMBA_KAL_TASK_INFO_s taskInfo;
#endif
    uint32_t ret1 = 0U;

    ret1 |= RefCV_MemblkInit();
    ret1 |= RefCV_NetScriptParser(path,&num_of_thread);
    for(i = 0U ; i < num_of_thread; i++) {
        Rval = RefCV_NetFlexidagOpen(&net_thread_params[i].handle);
        if (Rval == 0U) {
            if(max_temp_size < net_thread_params[i].handle.fd_gen_handle.mem_req.flexidag_temp_buffer_size) {
                max_temp_size = net_thread_params[i].handle.fd_gen_handle.mem_req.flexidag_temp_buffer_size;
            }
        } else {
            AmbaPrint_PrintUInt5("RefCV_Net RefCV_NetFlexidagOpen(%d) fail", i, 0U, 0U, 0U, 0U);
        }
    }
    ret1 |= RefCV_MemblkAlloc(max_temp_size, &g_temp_buf);

    if(num_of_thread > MAX_THREAD_NUM) {
        AmbaPrint_PrintUInt5("RefCV_Net num_of_thread(%d) > MAX_THREAD_NUM(%d)", num_of_thread, MAX_THREAD_NUM, 0U, 0U, 0U);
    } else {
        if (main_task_init == 1U) {
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
            ret1 |= AmbaKAL_TaskQuery(&main_task, &taskInfo);
            if ((taskInfo.TaskState != TX_COMPLETED) && (taskInfo.TaskState != TX_TERMINATED)) {
                Rval  = AmbaKAL_TaskTerminate(&main_task);
                if ( Rval  != KAL_ERR_NONE ) {
                    AmbaPrint_PrintUInt5("RefCV_Net AmbaKAL_TaskTerminate fail (%d)", Rval, 0U, 0U, 0U, 0U);
                }
            }
#endif
            Rval = AmbaKAL_TaskDelete(&main_task);
            if ( Rval != KAL_ERR_NONE ) {
                AmbaPrint_PrintUInt5("RefCV_Net AmbaKAL_TaskDelete fail (%d)", Rval, 0U, 0U, 0U, 0U);
            }
        }

        if(Rval == 0U) {
            Rval = AmbaKAL_TaskCreate(&main_task,                                        /* pTask */
                                      task_name,                                                    /* pTaskName */
                                      100,     /* Priority */
                                      RefCV_NetMain,                                                /* void (*EntryFunction)(uint32_t) */
                                      NULL,                                                    /* EntryArg */
                                      &refcv_net_main_stack[0],                                 /* pStackBase */
                                      sizeof(refcv_net_main_stack),                                 /* StackByteSize */
                                      0U);
            if(Rval == 0U) {
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
                ret1 |= AmbaKAL_TaskSetSmpAffinity(&main_task, 0x1U);
#endif
                ret1 |= AmbaKAL_TaskResume(&main_task);
            } else {
                AmbaPrint_PrintUInt5("RefCV_Net AmbaKAL_TaskCreate fail (%d)", Rval, 0U, 0U, 0U, 0U);
            }
        }
        main_task_init = 1U;
    }
    (void) ret1;
}
