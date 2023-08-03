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
 *  @details refcode CV
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "schdr_api.h"
#include "cvtask_api.h"
#include "ambint.h"
#include "cvapi_visutil.h"
#include "cvapi_memio_interface.h"
#include "cvapi_svccvalgo_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_amba_od_interface.h"

#define OPTIONS                     "p:l:"

#define REFCV_NET_KEY1              0xDD00
#define REFCV_NET_KEY2              0xEE00

#define MAX_THREAD_NUM              8U
#define MAX_GOLDEN_NUM              4U
#define REF_CV_BUF_NUM              4U

#define REF_MEMIO_PICINFO           0U
#define REF_MEMIO_RAW               1U
#define REF_MEMIO_MAX               2U

#define MAX_STR_TOKEN_NUMBER        (5U)
#define MAX_STR_TOKEN_LENGTH        (128U)
#define READ_BUF_SIZE               (2048U)

#define OUTPUT_TYPE_RAW             0U
#define OUTPUT_TYPE_OPEN_OD         1U
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
    uint32_t                        ResultFlag;
    uint32_t                        RunFlag;
    uint32_t                        output_file;
    REF_CV_NET_HANDLE_s             handle;
    pthread_t                       task;
    pthread_t                       result_task;
    int                             msg_queue[MAX_THREAD_NUM];
    int                             result_msg_queue[MAX_THREAD_NUM];
} REF_CV_NET_PARAMS_s;

typedef struct {
    long mtype ;
    REF_CV_NET_FRAME_s frame;
} REF_CV_NET_FRAME_MSG_s;

typedef struct {
    long mtype ;
    uint32_t result;
} REF_CV_NET_RESULT_MSG_s;


static REF_CV_NET_PARAMS_s net_thread_params[MAX_THREAD_NUM];
static flexidag_memblk_t g_temp_buf;
static char script_path[128] = "";
static uint32_t log_entry = 512U;

static uint32_t Crc32Table[] = {
    0x00000000U, 0x77073096U, 0xee0e612cU, 0x990951baU, 0x076dc419U, 0x706af48fU,
    0xe963a535U, 0x9e6495a3U, 0x0edb8832U, 0x79dcb8a4U, 0xe0d5e91eU, 0x97d2d988U,
    0x09b64c2bU, 0x7eb17cbdU, 0xe7b82d07U, 0x90bf1d91U, 0x1db71064U, 0x6ab020f2U,
    0xf3b97148U, 0x84be41deU, 0x1adad47dU, 0x6ddde4ebU, 0xf4d4b551U, 0x83d385c7U,
    0x136c9856U, 0x646ba8c0U, 0xfd62f97aU, 0x8a65c9ecU, 0x14015c4fU, 0x63066cd9U,
    0xfa0f3d63U, 0x8d080df5U, 0x3b6e20c8U, 0x4c69105eU, 0xd56041e4U, 0xa2677172U,
    0x3c03e4d1U, 0x4b04d447U, 0xd20d85fdU, 0xa50ab56bU, 0x35b5a8faU, 0x42b2986cU,
    0xdbbbc9d6U, 0xacbcf940U, 0x32d86ce3U, 0x45df5c75U, 0xdcd60dcfU, 0xabd13d59U,
    0x26d930acU, 0x51de003aU, 0xc8d75180U, 0xbfd06116U, 0x21b4f4b5U, 0x56b3c423U,
    0xcfba9599U, 0xb8bda50fU, 0x2802b89eU, 0x5f058808U, 0xc60cd9b2U, 0xb10be924U,
    0x2f6f7c87U, 0x58684c11U, 0xc1611dabU, 0xb6662d3dU, 0x76dc4190U, 0x01db7106U,
    0x98d220bcU, 0xefd5102aU, 0x71b18589U, 0x06b6b51fU, 0x9fbfe4a5U, 0xe8b8d433U,
    0x7807c9a2U, 0x0f00f934U, 0x9609a88eU, 0xe10e9818U, 0x7f6a0dbbU, 0x086d3d2dU,
    0x91646c97U, 0xe6635c01U, 0x6b6b51f4U, 0x1c6c6162U, 0x856530d8U, 0xf262004eU,
    0x6c0695edU, 0x1b01a57bU, 0x8208f4c1U, 0xf50fc457U, 0x65b0d9c6U, 0x12b7e950U,
    0x8bbeb8eaU, 0xfcb9887cU, 0x62dd1ddfU, 0x15da2d49U, 0x8cd37cf3U, 0xfbd44c65U,
    0x4db26158U, 0x3ab551ceU, 0xa3bc0074U, 0xd4bb30e2U, 0x4adfa541U, 0x3dd895d7U,
    0xa4d1c46dU, 0xd3d6f4fbU, 0x4369e96aU, 0x346ed9fcU, 0xad678846U, 0xda60b8d0U,
    0x44042d73U, 0x33031de5U, 0xaa0a4c5fU, 0xdd0d7cc9U, 0x5005713cU, 0x270241aaU,
    0xbe0b1010U, 0xc90c2086U, 0x5768b525U, 0x206f85b3U, 0xb966d409U, 0xce61e49fU,
    0x5edef90eU, 0x29d9c998U, 0xb0d09822U, 0xc7d7a8b4U, 0x59b33d17U, 0x2eb40d81U,
    0xb7bd5c3bU, 0xc0ba6cadU, 0xedb88320U, 0x9abfb3b6U, 0x03b6e20cU, 0x74b1d29aU,
    0xead54739U, 0x9dd277afU, 0x04db2615U, 0x73dc1683U, 0xe3630b12U, 0x94643b84U,
    0x0d6d6a3eU, 0x7a6a5aa8U, 0xe40ecf0bU, 0x9309ff9dU, 0x0a00ae27U, 0x7d079eb1U,
    0xf00f9344U, 0x8708a3d2U, 0x1e01f268U, 0x6906c2feU, 0xf762575dU, 0x806567cbU,
    0x196c3671U, 0x6e6b06e7U, 0xfed41b76U, 0x89d32be0U, 0x10da7a5aU, 0x67dd4accU,
    0xf9b9df6fU, 0x8ebeeff9U, 0x17b7be43U, 0x60b08ed5U, 0xd6d6a3e8U, 0xa1d1937eU,
    0x38d8c2c4U, 0x4fdff252U, 0xd1bb67f1U, 0xa6bc5767U, 0x3fb506ddU, 0x48b2364bU,
    0xd80d2bdaU, 0xaf0a1b4cU, 0x36034af6U, 0x41047a60U, 0xdf60efc3U, 0xa867df55U,
    0x316e8eefU, 0x4669be79U, 0xcb61b38cU, 0xbc66831aU, 0x256fd2a0U, 0x5268e236U,
    0xcc0c7795U, 0xbb0b4703U, 0x220216b9U, 0x5505262fU, 0xc5ba3bbeU, 0xb2bd0b28U,
    0x2bb45a92U, 0x5cb36a04U, 0xc2d7ffa7U, 0xb5d0cf31U, 0x2cd99e8bU, 0x5bdeae1dU,
    0x9b64c2b0U, 0xec63f226U, 0x756aa39cU, 0x026d930aU, 0x9c0906a9U, 0xeb0e363fU,
    0x72076785U, 0x05005713U, 0x95bf4a82U, 0xe2b87a14U, 0x7bb12baeU, 0x0cb61b38U,
    0x92d28e9bU, 0xe5d5be0dU, 0x7cdcefb7U, 0x0bdbdf21U, 0x86d3d2d4U, 0xf1d4e242U,
    0x68ddb3f8U, 0x1fda836eU, 0x81be16cdU, 0xf6b9265bU, 0x6fb077e1U, 0x18b74777U,
    0x88085ae6U, 0xff0f6a70U, 0x66063bcaU, 0x11010b5cU, 0x8f659effU, 0xf862ae69U,
    0x616bffd3U, 0x166ccf45U, 0xa00ae278U, 0xd70dd2eeU, 0x4e048354U, 0x3903b3c2U,
    0xa7672661U, 0xd06016f7U, 0x4969474dU, 0x3e6e77dbU, 0xaed16a4aU, 0xd9d65adcU,
    0x40df0b66U, 0x37d83bf0U, 0xa9bcae53U, 0xdebb9ec5U, 0x47b2cf7fU, 0x30b5ffe9U,
    0xbdbdf21cU, 0xcabac28aU, 0x53b39330U, 0x24b4a3a6U, 0xbad03605U, 0xcdd70693U,
    0x54de5729U, 0x23d967bfU, 0xb3667a2eU, 0xc4614ab8U, 0x5d681b02U, 0x2a6f2b94U,
    0xb40bbe37U, 0xc30c8ea1U, 0x5a05df1bU, 0x2d02ef8dU
};

uint32_t RefCV_NetCrc32(const uint8_t *pBuffer, uint32_t Size)
{
    const uint8_t *p;
    uint32_t Crc;
    uint32_t idx;

    p = pBuffer;
    Crc = ~0U;

    while (Size > 0U) {
        idx = (Crc ^ (uint32_t)*p) & 0x0FFU;
        Crc = Crc32Table[idx] ^ (Crc >> 8U);
        p++;
        Size--;
    }

    return Crc ^ ~0U;
}

static uint32_t RefCV_NetMemblkClean(flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;

    ret = AmbaCV_UtilityCmaMemClean(buf);
    if(ret != 0U) {
        printf("RefCV_NetMemblkClean : AmbaCV_UtilityCmaMemClean fail \n");
    }
    return ret;
}

static uint32_t RefCV_NetMemblkInvalid(flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;

    ret = AmbaCV_UtilityCmaMemInvalid(buf);
    if(ret != 0U) {
        printf("RefCV_NetMemblkClean : AmbaCV_UtilityCmaMemInvalid fail \n");
    }
    return ret;
}


static uint32_t RefCV_NetMemblkAlloc(uint32_t buf_size, flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;

    if(buf_size != 0) {
        if( AmbaCV_UtilityCmaMemAlloc(buf_size, 1, buf) != 0) {
            printf("RefCV_NetMemblkAlloc : Out of memory \n");
            ret = 1U;
        } else {
            AmbaCV_UtilityCmaMemClean(buf);
        }
    }
    return ret;
}

static uint32_t RefCV_NetMemblkFree(flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;

    if( AmbaCV_UtilityCmaMemFree(buf) != 0) {
        printf("RefCV_NetMemblkFree : AmbaCV_UtilityCmaMemFree fail \n");
        ret = 1U;
    }
    return ret;
}

static void RefCV_NetFlexidagRunCb(void *vpHandle, uint32_t flexidag_output_num, flexidag_memblk_t *pblk_Output, void *vpParameter)
{

    (void) vpHandle;
    (void) flexidag_output_num;
    (void) pblk_Output;
    (void) vpParameter;
}

static uint32_t RefCV_NetFlexidagOpen(REF_CV_NET_HANDLE_s *handle)
{
    uint32_t ret = 0U;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    AMBA_CV_FLEXIDAG_LOG_MSG_s set;

    set.flexidag_msg_entry = 1U;        //no requirement to send msg from flexidag to cvtask
    set.cvtask_msg_entry = 1U;              //no requirement to send internal cvtask msg
    set.arm_cpu_map = 0xFU;
    set.arm_log_entry = 32U;
    set.orc_log_entry = log_entry;
    (void) AmbaCV_FlexidagSetParamSet(&handle->fd_gen_handle, FLEXIDAG_PARAMSET_LOG_MSG, (void *)&set, sizeof(AMBA_CV_FLEXIDAG_LOG_MSG_s));
    ret = AmbaCV_FlexidagOpen(&handle->bin_buf, fd_gen_handle);
    if (ret != 0U) {
        printf("RefCV_NetFlexidagOpen : AmbaCV_FlexidagOpen fail \n");
        ret = 1U;
    }


    return ret;
}

static uint32_t RefCV_NetFlexidagInit(REF_CV_NET_HANDLE_s *handle)
{
    uint32_t ret = 0U;
    const AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;

    if(handle->init.state_buf.pBuffer == NULL) {
        ret = RefCV_NetMemblkAlloc(fd_gen_handle->mem_req.flexidag_state_buffer_size, &handle->init.state_buf);
        //printf("RefCV_NetMemblkAlloc state_buf pa = 0x%x size = 0x%x ",handle->init.state_buf.buffer_daddr, handle->init.state_buf.buffer_size);
    } else {
        //printf("RefCV_NetFlexidagInit : name = %s handle->init.state_buf.pBuffer != NULL\n", handle->name);
    }

    if (ret != 0U) {
        printf("RefCV_NetFlexidagInit : RefCV_NetMemblkAlloc state_buf fail \n");
        ret = 1U;
    } else {
        if(handle->init.temp_buf.pBuffer == NULL) {
            memcpy(&handle->init.temp_buf,&g_temp_buf,sizeof(flexidag_memblk_t));
            //printf("RefCV_NetMemblkAlloc temp_buf pa = 0x%x size = 0x%x ",handle->init.temp_buf.buffer_daddr, handle->init.temp_buf.buffer_size);
        } else {
            //printf("RefCV_NetFlexidagInit : name = %s handle->init.temp_buf.pBuffer != NULL\n", handle->name);
        }
        ret = AmbaCV_FlexidagInit(fd_gen_handle, &handle->init);
        if (ret != 0U) {
            printf("RefCV_NetFlexidagInit : AmbaCV_FlexidagInit fail \n");
            ret = 1U;
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
        ret = RefCV_NetMemblkAlloc(sizeof(cv_pic_info_t), &handle->in_buf[index].buf[0]);
        //printf("RefCV_NetMemblkAlloc in_buf[%d].buf[0] pa = 0x%x size = 0x%x ",index, handle->in_buf[index].buf[0].buffer_daddr, handle->in_buf[index].buf[0].buffer_size);
    }

    if(ret != 0U) {
        printf("RefCV_NetFlexidagRunPicinfo : RefCV_NetMemblkAlloc fail \n");
    } else {
        pDataIn = (cv_pic_info_t *)handle->in_buf[index].buf[0].pBuffer;
        memcpy(pDataIn, in, sizeof(cv_pic_info_t));
        U32DataIn = handle->in_buf[index].buf[0].buffer_daddr;
        for (i = 0U ; i < MAX_HALF_OCTAVES ; i++) {
            pDataIn->rpLumaLeft[i] = in->rpLumaLeft[i] - U32DataIn;
            pDataIn->rpLumaRight[i] = in->rpLumaRight[i] - U32DataIn;
            pDataIn->rpChromaLeft[i] = in->rpChromaLeft[i] - U32DataIn;
            pDataIn->rpChromaRight[i] = in->rpChromaRight[i] - U32DataIn;
        }
        ret = RefCV_NetMemblkClean(&handle->in_buf[index].buf[0]);
        if(ret != 0U) {
            printf("RefCV_NetFlexidagRunPicinfo : RefCV_NetMemblkClean fail \n");
            ret = 1U;
        } else {
            handle->out_buf[index].num_of_buf = fd_gen_handle->mem_req.flexidag_num_outputs;
            for(i = 0U; i < handle->out_buf[index].num_of_buf; i++) {
                if(handle->out_buf[index].buf[i].pBuffer == NULL) {
                    ret = RefCV_NetMemblkAlloc(fd_gen_handle->mem_req.flexidag_output_buffer_size[i], &handle->out_buf[index].buf[i]);
                    //printf("RefCV_NetMemblkAlloc out_buf[%d].buf[%d] pa = 0x%x size = 0x%x ",index, i, handle->out_buf[index].buf[i].buffer_daddr, handle->out_buf[index].buf[i].buffer_size);
                }

                if (ret != 0U) {
                    break;
                }
            }

            if (ret == 0U) {
                ret = AmbaCV_FlexidagRunNonBlock(fd_gen_handle, cb, cb_param, &handle->in_buf[index], &handle->out_buf[index], token_id);
                if (ret != 0U) {
                    printf("RefCV_NetFlexidagRunPicinfo : AmbaCV_FlexidagRun fail \n");
                    ret = 1U;
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
            ret = RefCV_NetMemblkAlloc(sizeof(memio_source_recv_raw_t), &handle->in_buf[index].buf[i]);
        }

        if (ret != 0U) {
            printf("RefCV_NetFlexidagRunRaw : RefCV_MemblkAlloc in_buf fail \n");
            ret = 1U;
            break;
        } else {
            pDataIn = (memio_source_recv_raw_t *)handle->in_buf[index].buf[i].pBuffer;
            memcpy(pDataIn, &in->io[i], sizeof(memio_source_recv_raw_t));
            (void) RefCV_NetMemblkClean(&handle->in_buf[index].buf[i]);
        }
    }

    if(ret == 0U) {
        handle->out_buf[index].num_of_buf = fd_gen_handle->mem_req.flexidag_num_outputs;
        for(i = 0U; i < fd_gen_handle->mem_req.flexidag_num_outputs; i++) {
            if(handle->out_buf[index].buf[i].pBuffer == NULL) {
                ret = RefCV_NetMemblkAlloc(fd_gen_handle->mem_req.flexidag_output_buffer_size[i], &handle->out_buf[index].buf[i]);
            }

            if (ret != 0U) {
                printf("RefCV_NetFlexidagRunRaw : RefCV_MemblkAlloc out_buf fail \n");
                ret = 1U;
                break;
            }
        }

        if (ret == 0U) {
            ret = AmbaCV_FlexidagRunNonBlock(fd_gen_handle, cb, cb_param, &handle->in_buf[index], &handle->out_buf[index], token_id);
            if (ret != 0U) {
                printf("RefCV_NetFlexidagRunRaw : AmbaCV_FlexidagRun fail \n");
                ret = 1U;
            }
        }
    }

    return ret;
}

static uint32_t RefCV_NetFlexidagClose(REF_CV_NET_HANDLE_s *handle)
{
    uint32_t ret = 0U,i,j;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;

    ret = AmbaCV_FlexidagClose(fd_gen_handle);
    if (ret != 0U) {
        printf("RefCV_NetFlexidagClose : AmbaCV_FlexidagClose fail \n");
        ret = 1U;
    }

    if(RefCV_NetMemblkFree(&handle->bin_buf) != 0U ) {
        printf("RefCV_NetFlexidagClose : AmbaCV_UtilityCmaMemFree bin_buf fail \n");
        ret = 1U;
    }

    if(RefCV_NetMemblkFree(&handle->init.state_buf) != 0U ) {
        printf("RefCV_NetFlexidagClose : AmbaCV_UtilityCmaMemFree state_buf fail \n");
        ret = 1U;
    }

    for(i = 0U; i < handle->num_raw; i++) {
        if(RefCV_NetMemblkFree(&handle->raw_buf_y[i]) != 0U ) {
            printf("RefCV_NetFlexidagClose : AmbaCV_UtilityCmaMemFree raw_buf y fail \n");
            ret = 1U;
        }

        if(handle->num_per_raw == 2U) {
            if(RefCV_NetMemblkFree(&handle->raw_buf_uv[i]) != 0U ) {
                printf("RefCV_NetFlexidagClose : AmbaCV_UtilityCmaMemFree raw_buf uv fail \n");
                ret = 1U;
            }
        }

        if(RefCV_NetMemblkFree(&handle->result_buf[i]) != 0U ) {
            printf("RefCV_NetFlexidagClose : AmbaCV_UtilityCmaMemFree result_buf fail \n");
            ret = 1U;
        }
    }

    for(j = 0U; j < REF_CV_BUF_NUM; j++) {
        for(i = 0U; i < handle->in_buf[j].num_of_buf; i++) {
            (void) RefCV_NetMemblkFree(&handle->in_buf[j].buf[i]);
        }
    }

    for(j = 0U; j < REF_CV_BUF_NUM; j++) {
        for(i = 0U; i < handle->out_buf[j].num_of_buf; i++) {
            (void) RefCV_NetMemblkFree(&handle->out_buf[j].buf[i]);
        }
    }
    return ret;
}

static uint32_t RefCV_NetResultRaw(uint32_t id,uint32_t frame_id, const AMBA_CV_FLEXIDAG_IO_s *pOut, const flexidag_memblk_t *pResultBlk, uint32_t output_file)
{
    uint32_t Rval = 0U,crc_out = 0U,crc_result = 0U,i,ret;
    char out_path[64];
    FILE *Fp = NULL;
    uint32_t NumSuccess;
    const flexidag_memblk_t *pOutBlk;
    char *pResultAddr;
    uint32_t ResultSize;
    const uint8_t *ptr;

    if(output_file == 1U) {
        sprintf(&out_path[0], "./flexidag_out%d_%d_%d.bin", id, frame_id);
        sprintf(&out_path[0], "./flexidag_out%d_%d_%d.bin", id, frame_id);
        Fp = fopen(out_path, "wb");
        if(Fp == NULL) {
            printf("RefCV_NetResultRaw AmbaFS_FileOpen fail \n");
            ret = 1U;
        }
    }
    pResultAddr = pResultBlk->pBuffer;
    ResultSize = pResultBlk->buffer_size;
    for(i = 0U; i < pOut->num_of_buf; i++) {
        pOutBlk = &pOut->buf[i];
        crc_out = RefCV_NetCrc32((uint8_t *)pOutBlk->pBuffer, pOutBlk->buffer_size);
        if (ResultSize >= pOutBlk->buffer_size) {
            crc_result = RefCV_NetCrc32((uint8_t *)pResultAddr, pOutBlk->buffer_size);
        } else {
             crc_result = 0U;
        }

        if(output_file == 1U) {
            if(Fp != NULL) {
                NumSuccess = fwrite(pOutBlk->pBuffer, 1U, pOutBlk->buffer_size, Fp);
                if( NumSuccess != pOutBlk->buffer_size ) {
                    printf("RefCV_NetResultRaw AmbaFS_FileWrite fail NumSuccess = 0x%x buffer_size = 0x%x \n", NumSuccess, pOutBlk->buffer_size);
                }
            }
        }
        if(crc_out != crc_result) {
            Rval = 1U;
            printf("RefCV_NetResultRaw id (%d) frame (%d) out (%d) error crc_out (%x) crc_result (%x) \n",id, frame_id, i, crc_out, crc_result);
        }
        if (ResultSize >= pOutBlk->buffer_size) {
            pResultAddr = &pResultAddr[pOutBlk->buffer_size];
            ResultSize = ResultSize - pOutBlk->buffer_size;
        }
    }
    if(output_file == 1U) {
        (void) fclose(Fp);
    }
    return Rval;
}

static void* RefCV_NetResultMain(void *arg)
{
    uint32_t ret,buf_index,raw_index,loop = 1U,i;
    REF_CV_NET_PARAMS_s  *param = (REF_CV_NET_PARAMS_s  *)arg;
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info;
    REF_CV_NET_FRAME_MSG_s frame_msg;
    REF_CV_NET_RESULT_MSG_s result_msg;

    param->ResultFlag = 1U;
    while(loop == 1U) {
        msgrcv( param->msg_queue[param->id], &frame_msg, sizeof(frame_msg) - sizeof(long), REFCV_NET_KEY1, 0) ;
        result_msg.mtype = REFCV_NET_KEY2;
        msgsnd(param->result_msg_queue[param->id], &result_msg, sizeof(result_msg) - sizeof(long), IPC_NOWAIT);
        memset(&run_info,0,sizeof(run_info));
        (void) AmbaCV_FlexidagWaitRunFinish(&param->handle.fd_gen_handle, frame_msg.frame.token_id, &run_info);

        if(run_info.output_not_generated == 0U) {
            printf("RefCV_NetResultMain (%d) frame_id = %d cv_run_time = %d us, over_head = %d us \n",param->id, frame_msg.frame.frame_id, run_info.end_time - run_info.start_time, ((run_info.api_end_time - run_info.api_start_time) - (run_info.end_time - run_info.start_time)));
            buf_index = frame_msg.frame.frame_id%REF_CV_BUF_NUM;
            raw_index = frame_msg.frame.frame_id%(param->handle.num_raw);
            for(i = 0U; i < param->handle.out_buf[buf_index].num_of_buf; i++) {
                (void) RefCV_NetMemblkInvalid(&param->handle.out_buf[buf_index].buf[i]);
            }
            if(param->handle.output_type == OUTPUT_TYPE_RAW) {
                ret = RefCV_NetResultRaw(param->id, frame_msg.frame.frame_id,&param->handle.out_buf[buf_index], &param->handle.result_buf[raw_index],param->output_file);
            } else {
                printf("RefCV_NetResultMain (%d) output result not support \n",param->id);
                ret = 1U;
            }
            if(ret != 0U) {
                param->num_err = param->num_err + 1U;
            }
            param->cur_runs = param->cur_runs + 1U;
            memset(param->handle.out_buf[buf_index].buf[0].pBuffer, 0x0, param->handle.out_buf[buf_index].buf[0].buffer_size);
            (void) RefCV_NetMemblkClean(&param->handle.out_buf[buf_index].buf[0]);
        }
        if((frame_msg.frame.frame_id+1) == param->num_runs) {
            param->ResultFlag = 0U;
            printf("RefCV_NetResultMain (%d) finish \n",param->id);
        }
    }
}

static void* RefCV_NetRun(void *arg)
{
    uint32_t i,j,ret,loop = 1U,raw_index,value;
    REF_CV_NET_PARAMS_s  *param = (REF_CV_NET_PARAMS_s  *)arg;
    memio_source_recv_picinfo_t memio_pic_info = {0};
    cv_pic_info_t *pic_info = &memio_pic_info.pic_info;
    memio_source_recv_multi_raw_t raw;
    char log_path[128];
    uint32_t token_id = 0U;
    void *cb_param = (void *)param;
    uint32_t YuvWidth,YuvHeight,YuvPitch;
    REF_CV_NET_FRAME_MSG_s frame_msg;
    REF_CV_NET_RESULT_MSG_s result_msg;

    param->RunFlag  = 1U;
    printf("RefCV_NetRun (%d) start \n",param->id);

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
        printf("RefCV_NetRun (%d) RefCV_NetFlexidagInit fail \n",param->id);
    } else {
        for (i = 0U; i < param->num_runs; i++) {
            raw_index = i%(param->handle.num_raw);
            if(param->handle.memio_type == REF_MEMIO_PICINFO) {
                for (j=0U ; j<5U ; j++) {
                    pic_info->rpLumaLeft[j] = param->handle.raw_buf_y[raw_index].buffer_daddr;
                    pic_info->rpChromaLeft[j] = param->handle.raw_buf_y[raw_index].buffer_daddr + (YuvPitch * YuvHeight);
                    pic_info->rpLumaRight[j] = pic_info->rpLumaLeft[j];
                    pic_info->rpChromaRight[j] = pic_info->rpChromaLeft[j];
                }
                ret = RefCV_NetFlexidagRunPicinfo(&param->handle, i, RefCV_NetFlexidagRunCb, cb_param, pic_info, &token_id);
            } else if (param->handle.memio_type == REF_MEMIO_RAW) {
                raw.num_io = param->handle.num_per_raw;
                raw.io[0].addr = param->handle.raw_buf_y[raw_index].buffer_daddr;
                raw.io[0].size = param->handle.raw_buf_y[raw_index].buffer_size;

                if(param->handle.num_per_raw == 2U) {
                    raw.io[1].addr = param->handle.raw_buf_uv[raw_index].buffer_daddr;
                    raw.io[1].size = param->handle.raw_buf_uv[raw_index].buffer_size;
                }
                ret = RefCV_NetFlexidagRunRaw(&param->handle, i, RefCV_NetFlexidagRunCb, cb_param, &raw, &token_id);
            } else {
                printf("RefCV_NetRun (%d) RefCV_NetFlexidagRun memio_type is not support \n",param->id, ret);
                ret = 1U;
            }
            if (ret == 0U) {
                frame_msg.mtype = REFCV_NET_KEY1;
                frame_msg.frame.frame_id = i;
                frame_msg.frame.token_id = token_id;
                msgsnd(param->msg_queue[param->id], &frame_msg, sizeof(frame_msg) - sizeof(long), IPC_NOWAIT);
                msgrcv( param->result_msg_queue[param->id], &result_msg, sizeof(result_msg) - sizeof(long), REFCV_NET_KEY2, 0) ;
            } else {
                printf("RefCV_NetRun (%d) RefCV_NetFlexidagRun fail ret = %x \n",param->id, ret);
            }
        }
        while(param->ResultFlag != 0U) {

        }
        sprintf(&log_path[0], "./flexidag_log%d.txt", param->id);
        ret = AmbaCV_FlexidagDumpLog(&param->handle.fd_gen_handle,log_path, FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK);
        if (ret != 0U) {
            printf("RefCV_NetRun (%d) AmbaCV_FlexidagDumpLog fail \n",param->id);
        }

        ret = RefCV_NetFlexidagClose(&param->handle);
        if (ret != 0U) {
            printf("RefCV_NetRun (%d) AmbaCV_FlexidagClose fail \n",param->id);
        }
    }
    param->RunFlag = 0U;
    while(loop == 1U) {
        (void) usleep(1000U);
    }
}

static void RefCV_NetMain(uint32_t num_of_thread)
{
    uint32_t i,ret = 0U, all_stop = 0U;

    for(i = 0U; i < num_of_thread; i ++) {
        //task create
        pthread_create(&net_thread_params[i].task, NULL, RefCV_NetRun, &net_thread_params[i]);
        //result task create
        pthread_create(&net_thread_params[i].result_task, NULL, RefCV_NetResultMain, &net_thread_params[i]);
    }

    usleep(200U);
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
        (void) usleep(250U); /* Sleep 250ms */
    } while (all_stop == 0U);
    for(i = 0U; i < num_of_thread; i ++) {
        printf("RefCV_NetMain thread (%d) total (%d) error (%d) \n",net_thread_params[i].id, net_thread_params[i].cur_runs, net_thread_params[i].num_err);
    }
    if(g_temp_buf.pBuffer != NULL) {
        (void) RefCV_NetMemblkFree(&g_temp_buf);
    }
    printf("RefCV_NetMain end ");
}

static void RefCV_NetGetFileSize(const char* path, uint32_t* Size)
{
    uint64_t size;
    FILE *ifp;

    ifp = fopen(path, "rb");
    if (ifp == NULL) {
        printf("RefCV_NetLoadFile fopen %s fail \n", path);
    } else {
        (void) fseek(ifp, 0, SEEK_END);
        size = ftell(ifp);
        (void) fseek(ifp, 0, SEEK_SET);

    }
    fclose(ifp);
    *Size = (uint32_t)(size & 0xFFFFFFFFU);
}

static void RefCV_NetLoadFile(const char* path, void* buf)
{
    uint64_t size;
    FILE *ifp;

    ifp = fopen(path, "rb");
    if (ifp == NULL) {
        printf("RefCV_NetLoadFile fopen %s fail \n", path);
    } else {
        (void) fseek(ifp, 0, SEEK_END);
        size = ftell(ifp);
        (void) fseek(ifp, 0, SEEK_SET);
        if(size != fread(buf, 1, size, ifp)) {
            printf("RefCV_NetLoadFile : fread %s fail ", path);
        }
    }
    fclose(ifp);
}

static void RefCV_NetParamInit(REF_CV_NET_PARAMS_s *params,uint32_t id)
{
    char name[32] = "RevCV_Net";

    params->id = id;
    params->cur_runs = 0U;
    params->num_err = 0U;
    params->RunFlag = 0U;
    params->ResultFlag = 0U;
    memset(&params->handle, 0x0, sizeof(REF_CV_NET_HANDLE_s));
    if(params->init == 0U) {
        params->msg_queue[params->id] = msgget((key_t)(id | REFCV_NET_KEY1), 0666 | IPC_CREAT);
        params->result_msg_queue[params->id] = msgget((key_t)(id | REFCV_NET_KEY2), 0666 | IPC_CREAT);
        params->init = 1U;
    }
}

static void RefCV_NetLoad(const char* path,flexidag_memblk_t *blk)
{
    uint32_t buf_size;

    RefCV_NetGetFileSize(path, &buf_size);
    (void) RefCV_NetMemblkAlloc(buf_size, blk);
    (void) RefCV_NetLoadFile(path, blk->pBuffer);
    (void) RefCV_NetMemblkClean(blk);
}

static uint32_t RefCV_NetContextParser(const char* path,REF_CV_NET_PARAMS_s *params)
{
    uint32_t Rval;
    uint32_t i, j = 0U,value;
    uint32_t ReadSize, ByteRead, ArgCount;
    uint64_t size;
    FILE *File = NULL;
    char ReadBuffer[READ_BUF_SIZE + 32U];
    char LineBuffer[256U];
    char *saveptr = NULL;
    char *token,*key;

    File = fopen(path, "r");
    if (File == NULL) {
        printf("RefCV_NetContextParser : %s open failed!!. \n", path);
    } else {
        (void) fseek(File, 0, SEEK_END);
        size = ftell(File);
        (void) fseek(File, 0, SEEK_SET);
        ReadSize = (uint32_t)size;
        memset(ReadBuffer, 0, sizeof(ReadBuffer));
        memset(LineBuffer, 0, sizeof(LineBuffer));

        if (ReadSize > READ_BUF_SIZE) {
            printf("RefCV_NetContextParser : Buffer size should be %u \n", ReadSize);
            ReadSize = READ_BUF_SIZE;
        }

        fread(ReadBuffer, 1, ReadSize, File);
        fclose(File);
        for (i = 0U; i < ReadSize; i++) {
            if(ReadBuffer[i] != '\n') {
                if(ReadBuffer[i] != (char)0xD) {
                    LineBuffer[j] = ReadBuffer[i];
                    j++;
                }
            } else {
                LineBuffer[j] = '\0';
                /* Parse the input string to multiple tokens */
                token = strtok_r(LineBuffer," ",&saveptr);
                key = strtok_r(NULL," ",&saveptr);
                printf("RefCV_NetContextParser : token = %s key = %s \n", token,key);
                if (strncmp(token, "NUM_RUNS", 8U) == 0) {
                    value = strtoul(key, NULL, 0);
                    params->num_runs = value;
                } else if (strncmp(token, "MEMIO_TYPE", 10U) == 0) {
                    value = strtoul(key, NULL, 0);
                    if(value >= REF_MEMIO_MAX) {
                        printf("RefCV_NetContextParser : MEMIO_TYPE error \n");
                    } else {
                        params->handle.memio_type = value;
                    }
                } else if (strncmp(token, "FLEXIDAG_PATH", 13U) == 0) {
                    RefCV_NetLoad(key,&params->handle.bin_buf);
                } else if (strncmp(token, "RAW_PITCH", 9U) == 0) {
                    value = strtoul(key, NULL, 0);
                    params->handle.raw_pitch = value;
                } else if (strncmp(token, "RAW_WIDTH", 9U) == 0) {
                    value = strtoul(key, NULL, 0);
                    params->handle.raw_width = value;
                } else if (strncmp(token, "RAW_HEIGHT", 9U) == 0) {
                    value = strtoul(key, NULL, 0);
                    params->handle.raw_height = value;
                } else if (strncmp(token, "RAW0_Y_PATH", 11U) == 0) {
                    if(strncmp(key, "NULL", 4U) != 0) {
                        RefCV_NetLoad(key,&params->handle.raw_buf_y[0]);
                        params->handle.num_raw = params->handle.num_raw + 1U;
                        params->handle.num_per_raw = params->handle.num_per_raw + 1U;
                    }
                } else if (strncmp(token, "RAW0_UV_PATH", 12U) == 0) {
                    if(strncmp(key, "NULL", 4U) != 0) {
                        RefCV_NetLoad(key,&params->handle.raw_buf_uv[0]);
                        params->handle.num_per_raw = params->handle.num_per_raw + 1U;
                    }
                } else if (strncmp(token, "RAW1_Y_PATH", 11U) == 0) {
                    if(strncmp(key, "NULL", 4U) != 0) {
                        RefCV_NetLoad(key,&params->handle.raw_buf_y[1]);
                        params->handle.num_raw = params->handle.num_raw + 1U;
                    }
                } else if (strncmp(token, "RAW1_UV_PATH", 12U) == 0) {
                    if(strncmp(key, "NULL", 4U) != 0) {
                        RefCV_NetLoad(key,&params->handle.raw_buf_uv[1]);
                    }
                } else if (strncmp(token, "RAW2_Y_PATH", 11U) == 0) {
                    if(strncmp(key, "NULL", 4U) != 0) {
                        RefCV_NetLoad(key,&params->handle.raw_buf_y[2]);
                        params->handle.num_raw = params->handle.num_raw + 1U;
                    }
                } else if (strncmp(token, "RAW2_UV_PATH", 12U) == 0) {
                    if(strncmp(key, "NULL", 4U) != 0) {
                        RefCV_NetLoad(key,&params->handle.raw_buf_uv[2]);
                    }
                } else if (strncmp(token, "RAW3_Y_PATH", 11U) == 0) {
                    if(strncmp(key, "NULL", 4U) != 0) {
                        RefCV_NetLoad(key,&params->handle.raw_buf_y[3]);
                        params->handle.num_raw = params->handle.num_raw + 1U;
                    }
                } else if (strncmp(token, "RAW3_UV_PATH", 12U) == 0) {
                    if(strncmp(key, "NULL", 4U) != 0) {
                        RefCV_NetLoad(key,&params->handle.raw_buf_uv[3]);
                    }
                } else if (strncmp(token, "RESULT0_PATH", 12U) == 0) {
                    if(strncmp(key, "NULL", 4U) != 0) {
                        RefCV_NetLoad(key,&params->handle.result_buf[0]);
                    }
                } else if (strncmp(token, "RESULT1_PATH", 12U) == 0) {
                    if(strncmp(key, "NULL", 4U) != 0) {
                        RefCV_NetLoad(key,&params->handle.result_buf[1]);
                    }
                } else if (strncmp(token, "RESULT2_PATH", 12U) == 0) {
                    if(strncmp(key, "NULL", 4U) != 0) {
                        RefCV_NetLoad(key,&params->handle.result_buf[2]);
                    }
                } else if (strncmp(token, "RESULT3_PATH", 12U) == 0) {
                    if(strncmp(key, "NULL", 4U) != 0) {
                        RefCV_NetLoad(key,&params->handle.result_buf[3]);
                    }
                } else if (strncmp(token, "OUTPUT_FILE", 11U) == 0) {
                    value = strtoul(key, NULL, 0);
                    params->output_file = value;
                } else if (strncmp(token, "OUTPUT_FROM_ARM", 15U) == 0) {
                    value = strtoul(key, NULL, 0);
                    params->handle.output_from_arm = value;
                } else if (strncmp(token, "OUTPUT_TYPE", 11U) == 0) {
                    value = strtoul(key, NULL, 0);
                    if(value >= OUTPUT_TYPE_MAX) {
                        printf("RefCV_NetContextParser : OUTPUT_TYPE error \n");
                    } else {
                        params->handle.output_type = value;
                    }
                } else {
                    printf("RefCV_NetContextParser : [ERROR] Unkown pattern %s \n", token);
                    Rval = 1U;
                }
                memset(LineBuffer, 0, sizeof(LineBuffer));
                j = 0U;
            }
        }
    }
    return Rval;
}

static uint32_t RefCV_NetScriptParser(const char* path,uint32_t *num_of_thread)
{
    uint32_t Rval;
    uint32_t i, j = 0U;
    uint32_t ReadSize;
    FILE *File = NULL;
    uint64_t size;
    char ReadBuffer[READ_BUF_SIZE + 32U];
    char LineBuffer[256U];

    *num_of_thread = 0U;
    File = fopen(path, "r");
    if (File == NULL) {
        printf("RefCV_NetScriptParser : %s open failed! \n", path);
    } else {
        (void) fseek(File, 0, SEEK_END);
        size = ftell(File);
        (void) fseek(File, 0, SEEK_SET);
        ReadSize = (uint32_t)size;
        memset(ReadBuffer, 0, sizeof(ReadBuffer));
        memset(LineBuffer, 0, sizeof(LineBuffer));

        if (ReadSize > READ_BUF_SIZE) {
            printf("RefCV_NetScriptParser : Buffer size should be %u \n", ReadSize);
            ReadSize = READ_BUF_SIZE;
        }

        fread(ReadBuffer, 1, ReadSize, File);
        fclose(File);
        if (ReadSize > 0U) {
            for (i = 0U; i < ReadSize; i++) {
                if(ReadBuffer[i] != '\n') {
                    if(ReadBuffer[i] != (char)0xD) {
                        LineBuffer[j] = ReadBuffer[i];
                        j++;
                    }
                } else {
                    LineBuffer[j] = '\0';
                    printf("RefCV_NetScriptParser : file = %s \n", LineBuffer);
                    if(*num_of_thread < MAX_THREAD_NUM) {
                        RefCV_NetParamInit(&net_thread_params[*num_of_thread],*num_of_thread);
                        (void)RefCV_NetContextParser(&LineBuffer[0],&net_thread_params[*num_of_thread]);
                    } else {
                        printf("RefCV_NetScriptParser : file over MAX_THREAD_NUM(%d) \n",MAX_THREAD_NUM,0U);
                    }
                    memset(LineBuffer, 0, sizeof(LineBuffer));
                    j = 0U;
                    *num_of_thread = *num_of_thread + 1U;
                }
            }
        }
    }
    return Rval;
}

static void RefCV_NetPrintHelp(const char *exe)
{
    printf("\nusage: %s [args]\n", exe);
    printf("\t-p [flexidag script path]\n");
    printf("\t-l [flexidag log entry]\n");
    printf("\nfor example: %s \n", exe);
}

static void RefCV_NetOpt(int argc, char **argv)
{
    int c;

    optind = 1;
    while ((c = getopt(argc, argv, OPTIONS)) != -1) {
        switch (c) {
        case 'p':
            strcpy(script_path, optarg);
            break;
        case 'l':
            log_entry = strtoul(optarg, NULL, 0);
            break;
        default:
            RefCV_NetPrintHelp(argv[0]);
            exit(-1);
        }
    }
}

static void RefCV_NetSignalHandlerShutdown(int sig)
{
    printf("%s: Got signal %d, program exits!\n",__FILE__,sig);
    exit(0);
}

int main(int argc, char **argv)
{
    uint32_t Rval = 0U,num_of_thread,max_temp_size = 0U,i;
    AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg;

    /* Allow ourselves to be shut down gracefully by a signal */
    signal(SIGTERM, RefCV_NetSignalHandlerShutdown);
    signal(SIGHUP, RefCV_NetSignalHandlerShutdown);
    signal(SIGUSR1, RefCV_NetSignalHandlerShutdown);
    signal(SIGQUIT, RefCV_NetSignalHandlerShutdown);
    signal(SIGINT, RefCV_NetSignalHandlerShutdown);
    signal(SIGKILL, RefCV_NetSignalHandlerShutdown);

    RefCV_NetOpt(argc, argv);

    /* init scheduler */
    cfg.cpu_map = 0xD;
    cfg.log_level = LVL_DEBUG;
    (void) AmbaCV_FlexidagSchdrStart(&cfg);

    (void) RefCV_NetScriptParser(script_path,&num_of_thread);
    for(i = 0U ; i < num_of_thread; i++) {
        Rval = RefCV_NetFlexidagOpen(&net_thread_params[i].handle);
        if (Rval == 0U) {
            if(max_temp_size < net_thread_params[i].handle.fd_gen_handle.mem_req.flexidag_temp_buffer_size) {
                max_temp_size = net_thread_params[i].handle.fd_gen_handle.mem_req.flexidag_temp_buffer_size;
            }
        } else {
            printf("RefCV_Net RefCV_NetFlexidagOpen(%d) fail \n", i);
        }
    }
    if(max_temp_size != 0U) {
        (void) RefCV_NetMemblkAlloc(max_temp_size, &g_temp_buf);
    }
    RefCV_NetMain(num_of_thread);
}
