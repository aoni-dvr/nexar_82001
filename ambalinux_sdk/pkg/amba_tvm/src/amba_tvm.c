/*******************************************************************************
 * amba_tvm.c
 *
 * History:
 *    2020/05/06  - [Monica Yang] created
 *
 * Copyright [2020] Ambarella International LP.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
******************************************************************************/

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>

#include "nnctrl.h"
#include "AmbaRTOSWrapper.h"
#include "cvtask_ossrv.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"

#include "amba_tvm_priv.h"

#ifndef ALIGN_32_BYTE
#define ALIGN_32_BYTE(x) ((((x) + 31) >> 5) << 5)
#endif

typedef struct {
        struct list_head list;
        flexidag_memblk_t memblk;
} amba_mem_blk_t;


static tvm_context_t G_tvm_ctx = {.init_done = 0,};
static pthread_mutex_t tvm_dev_lock = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t list_lock;
static struct list_head head;


static inline void LIST_LOCK(pthread_mutex_t *lock)
{
        if (pthread_mutex_lock(lock) < 0) {
                perror("mutex_lock");
        }
}

static inline void LIST_UNLOCK(pthread_mutex_t *lock)
{
        if (pthread_mutex_unlock(lock) < 0) {
                perror("mutex_unlock");
        }
}

int GetAmbaTVMLibVersion(void)
{
        return AMBA_TVM_LIB_VERSION;
}

static UINT32 _LoadFlexiDagBin(const char* path, flexidag_memblk_t* MemBlk)
{
        UINT32 BinSize;
        UINT32 rval = 0U;

        AmbaCV_UtilityFileSize(path, &BinSize);

        if (BinSize != 0U) {
                rval = AmbaCV_UtilityCmaMemAlloc(BinSize, 1, MemBlk);
                if(rval != 0) {
                        tvm_error("_LoadFlexiDagBin: AmbaCV_UtilityCmaMemAlloc fail. rval =%u\n", rval);
                } else {
                        rval = AmbaCV_UtilityFileLoad(path, MemBlk);
                        if (rval != 0U) {
                                AmbaPrint_PrintUInt5("_LoadFlexiDagBin: AmbaCV_UtilityFileLoad failed:%x", rval, 0U, 0U, 0U, 0U);
                        }
                }
        } else {
                AmbaPrint_PrintUInt5("_LoadFlexiDagBin: Open flexibin failed", 0U, 0U, 0U, 0U, 0U);
                rval = 1U;
        }

        return rval;
}

static flexidag_memblk_t* tvm_virt_to_memblk(void *virt)
{
        int rval = 0;
        amba_mem_blk_t *pblk = NULL, *_pblk = NULL;
        flexidag_memblk_t *pfound = NULL;

        do {
                if (virt == 0) {
                        tvm_error("invalid address\n");
                        rval = -1;
                        break;
                }

                LIST_LOCK(&list_lock);
                if (!list_empty(&head)) {
                        list_for_each_entry_safe(pblk, _pblk, &head, list) {
                                if (pblk->memblk.pBuffer == virt) {
                                        pfound = &pblk->memblk;
                                        break;
                                }
                        }
                }
                LIST_UNLOCK(&list_lock);
        } while(0);

        if (pfound == 0x0) {
                tvm_error("tvm_virt_to_memblk failed!!\n");
        }

        return pfound;
}

static int tvm_alloc_cv_mem(uint32_t *psize, flexidag_memblk_t* pbuf, uint8_t cache_en)
{
        int rval = 0;

        rval = AmbaCV_UtilityCmaMemAlloc(*psize, cache_en, pbuf);
        if(rval != 0) {
                tvm_error("tvm_alloc_cv_mem: AmbaCV_UtilityCmaMemAlloc fail. rval =%u\n", rval);
        } else {
                memset(pbuf->pBuffer, 0, *psize);
        }

        return rval;
}

static int tvm_free_engine_cv_mem(tvm_net_match_t *pnet)
{
        int i = 0, rval = 0;
        flexidag_memblk_t* pbuf;

        for (i = 0; i < pnet->in_num; i++) {
                pbuf = &pnet->in_pair[i].fd_blk;
                if (pbuf->reserved_expansion != DEVICE_TYPE_AMBA) {
                        if (AmbaCV_UtilityCmaMemFree(pbuf) != 0) {
                                tvm_error("tvm_free_engine_cv_mem: in_pair[%d] free failed!\n", i);
                                rval = -1;
                        }
                }
        }

        for (i = 0; i < pnet->out_num; i++) {
                pbuf = &pnet->out_pair[i].fd_blk;
                if (pbuf->reserved_expansion != DEVICE_TYPE_AMBA) {
                        if (AmbaCV_UtilityCmaMemFree(pbuf) != 0) {
                                tvm_error("tvm_free_engine_cv_mem: out_pair[%d] free failed!\n", i);
                                rval = -1;
                        }
                }
        }

        pbuf = &pnet->state_blk;
        if (pbuf->pBuffer != NULL) {
                if (AmbaCV_UtilityCmaMemFree(pbuf) != 0) {
                        tvm_error("tvm_free_engine_cv_mem: state_blk free failed!\n");
                        rval = -1;
                }
        }

        pbuf = &pnet->temp_blk;
        if (pbuf->pBuffer != NULL) {
                if (AmbaCV_UtilityCmaMemFree(pbuf) != 0) {
                        tvm_error("tvm_free_engine_cv_mem: temp_blk free failed!\n");
                        rval = -1;
                }
        }

        pbuf = &pnet->net_blk;
        if (pbuf->pBuffer != NULL) {
                if (AmbaCV_UtilityCmaMemFree(pbuf) != 0) {
                        tvm_error("tvm_free_engine_cv_mem: net_blk free failed!\n");
                        rval = -1;
                }
        }

        return rval;
}

static int tvm_load_external_io_mem(tvm_net_match_t *pnet,
        amba_engine_io_t *engine_input, amba_engine_io_t *engine_output)
{
        int rval = 0;

        //tvm_log("tvm_load_external_io_mem ...\n");

        do {
                uint32_t i = 0, j = 0, idx = 0;
                uint32_t in_num = engine_input->num;
                uint32_t out_num = engine_output->num;
                flexidag_memblk_t *pblk = NULL;

                AmbaDLTensor *inputs = engine_input->tensors;
                const char** in_names = engine_input->names;
                AmbaDLTensor *outputs = engine_output->tensors;
                const char** out_names = engine_output->names;

                for (i = 0; i < in_num; ++ i) {
                        /* Flexidag does not provide IO name!!! */
                        /* Input sequence MUST be the same as   the flexidag's. */
#if 0
                        idx = -1;
                        const char* input_name = *(in_names + i);
                        for (j = 0; j < in_num; ++ j) {
                                if (strcmp(input_name, net_in->in_desc[j].name) == 0) {
                                        idx = j;
                                        break;
                                }
                        }
                        if (idx < 0) {
                                tvm_error("cannot find matched tensor name %s\n", input_name);
                                rval = -1;
                                break;
                        }
#endif
                        if ((inputs[i].device_id & 0xFF) != DEVICE_ID_AMBA_DSP) {
                                pblk = tvm_virt_to_memblk(inputs[i].data_virt);
                                pnet->in_pair[i].fd_blk.pBuffer =
                                        (uint8_t*)(inputs[i].data_virt);
                                pnet->in_pair[i].fd_blk.buffer_daddr =
                                        (uint32_t)(pblk->buffer_daddr);
                                pnet->in_pair[i].fd_blk.buffer_cacheable =
                                        pblk->buffer_cacheable;
                                pnet->in_pair[i].fd_blk.buffer_size =
                                        pblk->buffer_size;
                                /* Use reserved_expansion to determine this memblk is from external or internal */
                                /* If it is external, it is free in AmbaDeviceFree(). */
                                pnet->in_pair[i].fd_blk.reserved_expansion =
                                        DEVICE_TYPE_AMBA;
                        } else {
#if 0
                                pnet->in_pair[i].virt_addr = (unsigned long)inputs[i].data_virt;
                                net_in->in_desc[idx].update_pitch = (inputs[i].device_id & 0xFFFF00) >> 8;
                                if ((inputs[i].device_id & 0xFFFF00) == 0) {
                                        net_in->in_desc[idx].virt = \
                                                (uint8_t*)cavalry_mem_phys_to_virt((unsigned long)inputs[i].data_virt);
                                }
#endif
                        }
                }
                if (rval) break;

                for (i = 0; i < out_num; ++ i) {
#if 0
                        idx = -1;
                        const char* output_name = *(out_names + i);
                        for (j = 0; j < out_num; ++ j) {
                                if (strcmp(output_name, net_out->out_desc[j].name) == 0) {
                                        idx = j;
                                        break;
                                }
                        }
                        if (idx < 0) {
                                tvm_error("cannot find matched tensor name %s\n", output_name);
                                rval = -1;
                                break;
                        }
#endif
                        pblk = tvm_virt_to_memblk(outputs[i].data_virt);
                        pnet->out_pair[i].fd_blk.pBuffer =
                                (uint8_t*)(outputs[i].data_virt);
                        pnet->out_pair[i].fd_blk.buffer_daddr =
                                (uint32_t)(pblk->buffer_daddr);
                        pnet->out_pair[i].fd_blk.buffer_cacheable =
                                pblk->buffer_cacheable;
                        pnet->out_pair[i].fd_blk.buffer_size =
                                pblk->buffer_size;
                        /* Use reserved_expansion to determine this memblk is from external or internal */
                        /* If it is external, it is free in AmbaDeviceFree(). */
                        pnet->out_pair[i].fd_blk.reserved_expansion =
                                DEVICE_TYPE_AMBA;
                }
                if (rval) break;
        } while(0);

        //tvm_log("tvm_load_external_io_mem done ...\n");

        return rval;
}

static int tvm_alloc_internal_io_mem(tvm_net_match_t *pnet,
        amba_engine_io_t *engine_input, amba_engine_io_t *engine_output)
{
        uint32_t i = 0, j = 0, size = 0, dsize = 0;
        int rval = 0;

        //tvm_log("tvm_alloc_internal_io_mem ...\n");

        pnet->in_num = engine_input->num;
        for (i = 0; i < pnet->in_num; i++) {
#if 0
                size = 1;
                dsize = (engine_input->tensors[i].dtype_bits * engine_input->tensors[i].dtype_lanes + 7) / 8;
                for (j = 0; j < engine_input->tensors[i].ndim - 1; ++j) {
                        size *= engine_input->tensors[i].shape[j];
                }

                size *= (ALIGN_32_BYTE(engine_input->tensors[i].shape[j] * dsize));
#else
                /* Suppose the input is flattened if it is not pitch alignment! */
                size = engine_input->tensors[i].size;
#endif

                /* size is padded size */
                rval = tvm_alloc_cv_mem(&size, &pnet->in_pair[i].fd_blk,
                                        pnet->cache_en);
                if(rval != 0) {
                        tvm_error("tvm_alloc_cv_mem fail. rval =%u\n", rval);
                }
        }

        pnet->out_num = engine_output->num;
        for (i = 0; i < pnet->out_num; i++) {
                size = pnet->handle.mem_req.flexidag_output_buffer_size[i];
                rval = tvm_alloc_cv_mem(&size, &pnet->out_pair[i].fd_blk,
                                        pnet->cache_en);
                if(rval != 0) {
                        tvm_error("tvm_alloc_cv_mem fail. rval =%u\n", rval);
                }
        }

        return rval;
}

static int tvm_net_io_cfg(tvm_net_match_t *pnet,
                amba_engine_io_t *engine_input, amba_engine_io_t *engine_output)
{
        uint32_t i = 0, j = 0, size = 0, dsize = 0;
        int rval = 0;
        uint32_t device_type = engine_input->tensors[0].device_type;

        //tvm_log("tvm_net_io_cfg ...\n");

        pnet->in_num = engine_input->num;
        for (i = 0; i < pnet->in_num; i++) {
                strncpy(pnet->in_pair[i].port_name, engine_input->names[i], TVM_IO_NAME_MAX - 1);

                pnet->in_pair[i].virt_addr      = (uint8_t *) pnet->in_pair[i].fd_blk.pBuffer;
                pnet->in_pair[i].dram_addr      = pnet->in_pair[i].fd_blk.buffer_daddr;
                pnet->in_pair[i].dram_size      = pnet->in_pair[i].fd_blk.buffer_size;
                pnet->in_pair[i].data_size      = pnet->in_pair[i].fd_blk.buffer_size;

                pnet->in_pair[i].dim.plane      = engine_input->tensors[i].shape[0];
                pnet->in_pair[i].dim.depth      = engine_input->tensors[i].shape[1];
                pnet->in_pair[i].dim.height     = engine_input->tensors[i].shape[2];
                pnet->in_pair[i].dim.width      = engine_input->tensors[i].shape[3];

                pnet->in_pair[i].dim.pitch      =
                        (ALIGN_32_BYTE(pnet->in_pair[i].dim.width * dsize));

                j = 0;
                dsize = (engine_input->tensors[i].dtype_bits * engine_input->tensors[i].dtype_lanes + 7) / 8;
                while (dsize != 1) {
                        j++;
                        dsize >>= 1;
                }
                pnet->in_pair[i].data_fmt.size  = j;

#if 0
                tvm_log("Input: %u [%s] dim: (%u, %u, %u, %u), pitch: %u, "
                        "data_fmt: (%d, %d, %d, %d), "
                        "size: %u\n",
                        i, pnet->in_pair[i].port_name,
                        pnet->in_pair[i].dim.plane, pnet->in_pair[i].dim.depth,
                        pnet->in_pair[i].dim.height, pnet->in_pair[i].dim.width,
                        pnet->in_pair[i].dim.pitch,
                        pnet->in_pair[i].data_fmt.sign, pnet->in_pair[i].data_fmt.size,
                        pnet->in_pair[i].data_fmt.expoffset, pnet->in_pair[i].data_fmt.expbits,
                        pnet->in_pair[i].data_size);
#endif
        }

        pnet->out_num = engine_output->num;
        for (i = 0; i < pnet->out_num; i++) {
                strncpy(pnet->out_pair[i].port_name, engine_output->names[i], TVM_IO_NAME_MAX - 1);

                pnet->out_pair[i].virt_addr = (uint8_t *) pnet->out_pair[i].fd_blk.pBuffer;
                pnet->out_pair[i].dram_addr = pnet->out_pair[i].fd_blk.buffer_daddr;
                pnet->out_pair[i].dram_size = pnet->out_pair[i].fd_blk.buffer_size;
                pnet->out_pair[i].data_size = pnet->out_pair[i].fd_blk.buffer_size;

#if 0
                tvm_log("Output: ndim: %d\n", engine_output->tensors[i].ndim);
                if (engine_output->tensors[i].ndim == 5) {
                        tvm_log("Output: dim: (%d, %u, %u, %u, %u)\n",
                                engine_output->tensors[i].shape[0],
                                engine_output->tensors[i].shape[1],
                                engine_output->tensors[i].shape[2],
                                engine_output->tensors[i].shape[3],
                                engine_output->tensors[i].shape[4]);
                }
#endif

                if (engine_output->tensors[i].ndim == 5) {
                        pnet->out_pair[i].dim.plane      = engine_output->tensors[i].shape[1];
                        pnet->out_pair[i].dim.depth      = engine_output->tensors[i].shape[2];
                        pnet->out_pair[i].dim.height     = engine_output->tensors[i].shape[3];
                        pnet->out_pair[i].dim.width      = engine_output->tensors[i].shape[4];
                } else if (engine_output->tensors[i].ndim == 4) {
                        pnet->out_pair[i].dim.plane      = engine_output->tensors[i].shape[0];
                        pnet->out_pair[i].dim.depth      = engine_output->tensors[i].shape[1];
                        pnet->out_pair[i].dim.height     = engine_output->tensors[i].shape[2];
                        pnet->out_pair[i].dim.width      = engine_output->tensors[i].shape[3];
                } else if (engine_output->tensors[i].ndim == 3) {
                        pnet->out_pair[i].dim.plane      = 1;
                        pnet->out_pair[i].dim.depth      = engine_output->tensors[i].shape[0];
                        pnet->out_pair[i].dim.height     = engine_output->tensors[i].shape[1];
                        pnet->out_pair[i].dim.width      = engine_output->tensors[i].shape[2];
                } else if (engine_output->tensors[i].ndim == 2) {
                        pnet->out_pair[i].dim.plane      = 1;
                        pnet->out_pair[i].dim.depth      = 1;
                        pnet->out_pair[i].dim.height     = engine_output->tensors[i].shape[0];
                        pnet->out_pair[i].dim.width      = engine_output->tensors[i].shape[1];
                } else if (engine_output->tensors[i].ndim == 1) {
                        pnet->out_pair[i].dim.plane      = 1;
                        pnet->out_pair[i].dim.depth      = 1;
                        pnet->out_pair[i].dim.height     = 1;
                        pnet->out_pair[i].dim.width      = engine_output->tensors[i].shape[0];
                } else {
                        tvm_error("unsupported ndim (%d)\n", engine_output->tensors[i].ndim);
                }

                dsize = (engine_output->tensors[i].dtype_bits * engine_output->tensors[i].dtype_lanes + 7) / 8;

                j = 0;
                while (dsize != 1) {
                        j++;
                        dsize >>= 1;
                }
                pnet->out_pair[i].data_fmt.size = j;

#if 0
                tvm_log("Output: %u [%s] dim: (%u, %u, %u, %u), pitch: %u, "
                        "data_fmt: (%d, %d, %d, %d), "
                        "size: %u\n",
                        i, pnet->out_pair[i].port_name,
                        pnet->out_pair[i].dim.plane, pnet->out_pair[i].dim.depth,
                        pnet->out_pair[i].dim.height, pnet->out_pair[i].dim.width,
                        pnet->out_pair[i].dim.pitch,
                        pnet->out_pair[i].data_fmt.sign, pnet->out_pair[i].data_fmt.size,
                        pnet->out_pair[i].data_fmt.expoffset, pnet->out_pair[i].data_fmt.expbits,
                        pnet->out_pair[i].data_size);
#endif
        }


        return rval;
}

static int tvm_init_one_net(tvm_net_match_t *pnet,
        amba_engine_io_t *engine_input, amba_engine_io_t *engine_output)
{
        int rval = 0;
        uint32_t device_type = engine_input->tensors[0].device_type;
        uint32_t size = 0;

        if (pnet == NULL) {
                tvm_error("start net with NULL pointer\n");
                return -1;
        }

        pnet->cache_en = 1;

        do {
                /* 1. Load and open flexidag (call AmbaCV_FlexidagOpen())  */
                rval = _LoadFlexiDagBin(pnet->net_fn, &pnet->net_blk);
                if(rval != 0) {
                        tvm_error("_LoadFlexiDagBin fail. rval =%u\n", rval);
                } else {
                        //tvm_log("AmbaCV_FlexidagOpen %s ...\n", pnet->net_fn);
                        rval = AmbaCV_FlexidagOpen(&pnet->net_blk, &pnet->handle);
                        if(rval != 0U) {
                                tvm_error("AmbaCV_FlexidagOpen fail (Rval 0x%x)\n", rval);
                       }
                }

                pnet->net_id = (unsigned long) &pnet->handle;

                /* 2. Allocate memory for State and Temp buffer */
                if (rval == 0U) {     // State
                        size = pnet->handle.mem_req.flexidag_state_buffer_size;
                        if (size != 0U) {
                                rval = tvm_alloc_cv_mem(&size, &pnet->state_blk,
                                                        pnet->cache_en);
                        }
                }

                if (rval == 0U) {     // Temp
                        size = pnet->handle.mem_req.flexidag_temp_buffer_size;
                        if (size != 0U) {
                                rval = tvm_alloc_cv_mem(&size, &pnet->temp_blk,
                                                        pnet->cache_en);
                        } else {
                                memset(&pnet->temp_blk, 0x0, sizeof(flexidag_memblk_t));
                        }
                }

                /* 3. Initialize flexidag (call AmbaCV_FlexidagInit()) */
                if (rval == 0U) {
                        AMBA_CV_FLEXIDAG_INIT_s init;

                        memset(&init, 0, sizeof(AMBA_CV_FLEXIDAG_INIT_s));

                        init.state_buf = pnet->state_blk;
                        init.temp_buf  = pnet->temp_blk;
                        rval = AmbaCV_FlexidagInit(&pnet->handle, &init);
                        if(rval != 0U) {
                                tvm_error("AmbaCV_FlexidagInit fail (Rval 0x%x)\n", rval);
                        }
                }

                if (device_type == DEVICE_TYPE_AMBA) {
                        if (tvm_load_external_io_mem(pnet,
                                engine_input, engine_output) < 0) {
                                tvm_error("tvm_load_external_io_mem\n");
                                rval = -1;
                                break;
                        }
                } else {
                        if (tvm_alloc_internal_io_mem(pnet,
                                engine_input, engine_output) < 0) {
                                tvm_error("tvm_alloc_internal_io_mem\n");
                                rval = -1;
                                break;
                        }
                }

                if (tvm_net_io_cfg(pnet, engine_input, engine_output) < 0) {
                        tvm_error("tvm_net_io_cfg\n");
                        rval = -1;
                        break;
                }
        } while (0);

#if 0
        if (rval == 0) {
                pnet->net_bw_sz = net_cf.bandwidth_total;
        }
#endif

        return rval;
}

static int tvm_run_one_net(tvm_net_match_t *pnet)
{
        double one_vp_us = 0.0;
        uint32_t rval = 0, result = 0, size = 0;
        int i = 0;
        int out_num = 0;
        struct net_input_cfg net_in;
        AMBA_CV_FLEXIDAG_IO_s In, Out;
        AMBA_CV_FLEXIDAG_RUN_INFO_s run_info;
        memio_source_recv_raw_t InputRaw[8];

        memset(&net_in, 0, sizeof(net_in));

        do {
                /* update input node addr in live mode */
                In.num_of_buf = pnet->in_num;
                for (i = 0; i < pnet->in_num; ++i) {
                        InputRaw[i].addr        = pnet->in_pair[i].dram_addr;
                        InputRaw[i].size        = pnet->in_pair[i].dram_size;
                        InputRaw[i].pitch       = pnet->in_pair[i].dim.pitch;

                        size = sizeof(flexidag_memblk_t);
                        rval = tvm_alloc_cv_mem(&size, &In.buf[i], 1);
                        if(rval != 0) {
                                tvm_error("tvm_alloc_cv_mem fail. rval =%u\n", rval);
                        }

                        memcpy(In.buf[i].pBuffer, &InputRaw[i], sizeof(memio_source_recv_raw_t));

                        if (AmbaCV_UtilityCmaMemClean(&In.buf[i]) != 0) {
                                tvm_error("AmbaCV_UtilityCmaMemClean\n");
                                rval = -1;
                                break;
                        }
                }

                Out.num_of_buf = pnet->out_num;
                for (i = 0; i < pnet->out_num; ++i) {
                        memcpy(&Out.buf[i], &pnet->out_pair[i].fd_blk, sizeof(flexidag_memblk_t));
                        if (AmbaCV_UtilityCmaMemClean(&Out.buf[i]) != 0) {
                                tvm_error("AmbaCV_UtilityCmaMemClean\n");
                                rval = -1;
                                break;
                        }
                }

                result = AmbaCV_FlexidagRun(&pnet->handle, &In, &Out, &run_info);

                one_vp_us += run_info.end_time - run_info.start_time;
                if (result != 0){
                        tvm_error("AmbaCV_FlexidagRun failed ret: %d\n", result);
                        rval = -1;
                        break;
                }

                //AmbaCV_FlexidagDumpLog(&pnet->handle, NULL, FLEXILOG_ECHO_TO_CONSOLE | FLEXILOG_VIS_CVTASK);

                if (pnet->cache_en) {
                        out_num = pnet->out_num;
                        for (i = 0; i < out_num; ++i) {
                                if (AmbaCV_UtilityCmaMemInvalid(&pnet->out_pair[i].fd_blk) != 0) {
                                        tvm_error("AmbaCV_UtilityCmaMemInvalid\n");
                                        rval = -1;
                                        break;
                                }
                        }
                        if (rval) break;
                }

                if (rval < 0) {
                        tvm_error("AmbaCV_FlexidagRun\n");
                        rval = -1;
                        break;
                } else {
                        pnet->net_bw_us = (uint32_t)one_vp_us;
                        for (i = 0; i < In.num_of_buf; ++i) {
                                if (AmbaCV_UtilityCmaMemFree(&In.buf[i]) != 0) {
                                        tvm_error("tvm_run_one_net: AmbaCV_UtilityCmaMemFree failed!\n");
                                        rval = -1;
                                }
                        }
                }
        } while (0);

        return rval;
}

static int tvm_check_io_format(tvm_net_io_match_t *io, AmbaDLTensor *t)
{
        int rval = 0;
        uint32_t no_padding_size = 0;

        do {
                if (8 * (1<< io->data_fmt.size) != t->dtype_bits) {
                        rval = -1;
                        tvm_error("data bits of input tensor [%s] (%d,%d,%d,%d) should be %d != %d\n",
                                io->port_name, io->dim.plane, io->dim.depth, io->dim.height, io->dim.width, t->dtype_bits, 8 * (1<< io->data_fmt.size));
                        break;
                }
                no_padding_size = io->dim.plane * io->dim.depth * io->dim.height * io->dim.width * (1<< io->data_fmt.size);
                if (t->size != no_padding_size ) {
                        tvm_error("input tensor [%s] (%d,%d,%d,%d) has incorrect size [%d != %d].\n",
                                        io->port_name, io->dim.plane, io->dim.depth, io->dim.height, io->dim.width, t->size, no_padding_size);
                        rval = -1;
                        break;
                }
        } while(0);

        return rval;
}

static tvm_net_match_t *tvm_get_net_pair(unsigned long net_id)
{
        tvm_context_t *pctrl = &G_tvm_ctx;
        tvm_net_match_t *pnet = NULL, *safe = NULL;
        int found = 0;

        TVM_LOCK(&pctrl->net_list_lock);
        if (!list_empty(&pctrl->net_pair_list)) {
                list_for_each_entry_safe(pnet, safe, &pctrl->net_pair_list, net_node) {
                        if (pnet->net_id == net_id) {
                                found = 1;
                                break;
                        }
                }
        }
        TVM_UNLOCK(&pctrl->net_list_lock);

        if (!found) {
                tvm_error("Not found net_id: 0x%lx\n", net_id);
                pnet = NULL;
        }

        return pnet;
}

int InitAmbaTVM(void)
{
        int rval = 0;
        tvm_context_t *pctrl = &G_tvm_ctx;
        uint32_t status;
        AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg;

        TVM_LOCK(&tvm_dev_lock);
        do {
                if (pctrl->init_done) break;
                memset(pctrl, 0, sizeof(tvm_context_t));

                rval = AmbaCV_SchdrDrvState(&status);
                if (rval != 0) {
                        tvm_error("AmbaCV_SchdrDrvState failed. rval = %d\n", rval);
                } else {
                        if (status == 0x0) {
                                /* init scheduler */
                                cfg.cpu_map = 0x9;
                                cfg.log_level = LVL_DEBUG;
                                rval = AmbaCV_FlexidagSchdrStart(&cfg);
                                if (rval != 0) {
                                        tvm_error("AmbaCV_FlexidagSchdrStart failed. rval = %d\n", rval);
                                }
                        }
                }

                if (pthread_mutex_init(&pctrl->net_list_lock, NULL) < 0) {
                        perror("net list lock init");
                        rval = -1;
                        break;
                }
                INIT_LIST_HEAD(&pctrl->net_pair_list);

                if (pthread_mutex_init(&pctrl->mod_env_lock, NULL) < 0) {
                        perror("mod env lock init");
                        rval = -1;
                        break;
                }

                if (pthread_mutex_init(&list_lock, NULL) < 0) {
                        perror("list_lock init");
                        rval = -1;
                        break;
                } else {
                        INIT_LIST_HEAD(&head);
                }

                pctrl->init_done = 1;
                tvm_log("Init Ambarella target device\n");
        } while (0);
        ++pctrl->mod_num;
        TVM_UNLOCK(&tvm_dev_lock);

        return rval;
}

int InitAmbaEngine(amba_engine_cfg_t *engine_cfg,
        amba_engine_io_t *engine_input, amba_engine_io_t *engine_output)
{
        int rval = 0;
        tvm_context_t *pctrl = &G_tvm_ctx;
        tvm_net_match_t *pnet = NULL;
        char *ptr = NULL;

        do {
                if (engine_cfg == NULL) {
                        tvm_error("input null pointer.\n");
                        rval = -1;
                        break;
                }
                if ((pnet = (tvm_net_match_t*)malloc(sizeof(tvm_net_match_t))) == NULL) {
                        perror("pnet node\n");
                        rval = -1;
                        break;
                }
                memset(pnet, 0, sizeof(tvm_net_match_t));
                pnet->net_id = TVM_INVALID_VALUE;
                strncpy(pnet->net_name, engine_cfg->engine_name, TVM_NET_NAME_MAX - 1);
                ptr = getenv(engine_cfg->engine_filepath);
                if (ptr) {
                        pnet->net_fn = ptr;
                } else {
                        pnet->net_fn = engine_cfg->engine_filepath;
                        tvm_warning("Please check orcvp.bin file [%s] and "
                                "call ConfigAmbaEngineLocation in USER APP!\n", pnet->net_fn);
                }

                rval = tvm_init_one_net(pnet, engine_input, engine_output);

                INIT_LIST_HEAD(&pnet->net_node);
                TVM_LOCK(&pctrl->net_list_lock);
                list_add_tail(&pnet->net_node, &pctrl->net_pair_list);
                TVM_UNLOCK(&pctrl->net_list_lock);

                if (rval < 0) {
                        tvm_error("failed to init network\n");
                        break;
                }
                engine_cfg->engine_id = pnet->net_id;
        }while (0);

        return rval;
}

int SetAmbaEngineInput(amba_engine_cfg_t *engine_cfg,
        const char *input_name, AmbaDLTensor *input)
{
        int rval = 0;
        tvm_net_match_t *pnet = NULL;
        struct io_dim *p_dim = NULL;
        struct io_data_fmt *p_dt_fmt = NULL;
        tvm_net_io_match_t *io = NULL;
        uint32_t data_size = 0, padding_size = 0, no_padding_size = 0;
        uint8_t *virt_addr = NULL;
        uint8_t *in_addr = NULL;
        uint32_t one_line = 0;
        uint32_t p = 0, d = 0, h = 0;
        int i = 0;
        int input_idx = -1;

        //tvm_log("SetAmbaEngineInput start ...\n");

        do {
                if (engine_cfg == NULL || input_name == NULL || input == NULL) {
                        tvm_error("input null pointer.\n");
                        rval = -1;
                        break;
                }
                if ((pnet = tvm_get_net_pair(engine_cfg->engine_id)) == NULL) {
                        tvm_error("cannot find network 0x%lx\n", engine_cfg->engine_id);
                        rval = -1;
                        break;
                }
                for (i = 0; i < pnet->in_num; ++ i) {
                        if (strcmp(input_name, pnet->in_pair[i].port_name) == 0) {
                                input_idx = i;
                                break;
                        }
                }
                if (input_idx == -1) {
                        tvm_error("cannot find matched input [%s]\n", input_name);
                        rval = -1;
                        break;
                }
                io = &pnet->in_pair[input_idx];

                p_dim = &io->dim;
                p_dt_fmt = &io->data_fmt;

                if (tvm_check_io_format(io, input)) {
                        rval = -1;
                        tvm_error("tvm_check_io_format\n");
                        break;
                }

                padding_size = io->data_size;
                one_line = p_dim->width * (1 << p_dt_fmt->size);
                no_padding_size = p_dim->depth * p_dim->height * one_line;

                data_size = input->size;

                in_addr = (uint8_t*)input->data_virt;
                virt_addr = io->virt_addr;

                //tvm_log("input->device_type = %d, data_size = %u, padding_size = %u, no_padding_size = %u\n",
                //        input->device_type, data_size, padding_size, no_padding_size);

                /* do memcpy and padding for kDLCPU */
                if (input->device_type != DEVICE_TYPE_AMBA) {
                        if (data_size == padding_size) {
                                memcpy(virt_addr, in_addr, data_size);
                        } else if (data_size == no_padding_size){
                                for (p = 0; p < p_dim->plane; p++) {
                                        for (d = 0; d < p_dim->depth; d++) {
                                                for (h = 0; h < p_dim->height; h++) {
                                                        memcpy(virt_addr, in_addr, one_line);
                                                        virt_addr += p_dim->pitch;
                                                        in_addr += one_line;
                                                }
                                        }
                                }
                        }
                }
                /* clean cache in file mode or kDLCPU live mode */
                if ((input->device_id & 0xFF)  != DEVICE_ID_AMBA_DSP && pnet->cache_en) {
                        if (AmbaCV_UtilityCmaMemClean(&io->fd_blk) != 0 ) {
                                tvm_error("AmbaCV_UtilityCmaMemClean\n");
                                rval = -1;
                                break;
                        }
                }
#if 0
                /* update input physical addr for kDLAmba live mode */
                if (input->device_type == DEVICE_TYPE_AMBA &&
                        (input->device_id & 0xFF)  == DEVICE_ID_AMBA_DSP) {
                        io->dram_addr = (unsigned long)in_addr;
                        if ((input->device_id & 0xFFFF00) == 0) {
                                io->virt_addr = (uint8_t*)cavalry_mem_phys_to_virt((unsigned long)in_addr);
                        }
                }
#endif
        } while(0);

        return rval;
}

int RunAmbaEngine(amba_engine_cfg_t *engine_cfg, amba_perf_t *perf)
{
        int rval = 0;
        tvm_net_match_t *pnet = NULL;
        unsigned long net_id = engine_cfg->engine_id;

        //tvm_log("RunAmbaEngine start ...\n");

        do {
                if (engine_cfg == NULL) {
                        tvm_error("input null pointer.\n");
                        rval = -1;
                        break;
                }
                if ((pnet = tvm_get_net_pair(net_id)) == NULL) {
                        tvm_error("cannot find network 0x%lx\n", net_id);
                        rval = -1;
                        break;
                }
                rval = tvm_run_one_net(pnet);
                if (perf != NULL) {
                        perf->cvflow_time_us = pnet->net_bw_us;
                }
        } while(0);

        return rval;
}

int GetAmbaEngineOutput(amba_engine_cfg_t *engine_cfg,
        const char *output_name, AmbaDLTensor *output)
{
        int rval = 0;
        tvm_net_match_t *pnet = NULL;
        struct io_dim *p_dim = NULL;
        struct io_data_fmt *p_dt_fmt = NULL;
        tvm_net_io_match_t *io = NULL;
        uint32_t data_size = 0, padding_size = 0, no_padding_size = 0;
        uint8_t *virt_addr = NULL;
        uint8_t *out_addr = NULL;
        uint32_t one_line = 0;
        uint32_t p = 0, d = 0, h = 0;
        int i = 0, output_index = -1;
        uint32_t *ptr;

        //tvm_log("GetAmbaEngineOutput start ...\n");

        do {
                if (engine_cfg == NULL || output_name == NULL ||output == NULL) {
                        tvm_error("input null pointer.\n");
                        rval = -1;
                        break;
                }
                if ((pnet = tvm_get_net_pair(engine_cfg->engine_id)) == NULL) {
                        tvm_error("cannot find network %ld\n", engine_cfg->engine_id);
                        rval = -1;
                        break;
                }
                for (i = 0; i < pnet->out_num; ++ i) {
                        if (strcmp(output_name, pnet->out_pair[i].port_name) == 0) {
                                output_index = i;
                                break;
                        }
                }
                if (output_index == -1) {
                        tvm_error("cannot find matched output [%s]\n", output_name);
                        rval = -1;
                        break;
                }
                io = &pnet->out_pair[output_index];
                p_dim = &io->dim;
                p_dt_fmt = &io->data_fmt;

                if (tvm_check_io_format(io, output)) {
                        rval = -1;
                        tvm_error("tvm_check_io_format\n");
                        break;
                }

#if 0
                /* io->data_size includes size of header */
                outbuf = (uint8_t *) io->fd_blk.pBuffer;
                virt_addr = outbuf + ((cvflow_port_header_t*) outbuf)->buffer_offset[i];
                desc_n = (cvflow_buffer_desc_t*) (virt_addr - sizeof(cvflow_buffer_desc_t));

                tvm_log("df   = %u, %u, %u, %u\n",
                        desc_n->df[0], desc_n->df[1], desc_n->df[2], desc_n->df[3]);
                tvm_log("dims = %u, %u, %u, %u\n",
                        desc_n->dims[0], desc_n->dims[1], desc_n->dims[2], desc_n->dims[3]);
                tvm_log("num_bytes = %u\n", desc_n->num_bytes);
#endif

                virt_addr = (uint8_t *) io->fd_blk.pBuffer;
#if 0
                padding_size = io->data_size;
#else
                /* orc cvtask is 64 bytes alignment */
                /* cv22 vp output is 32-bytes alignment */
                padding_size = pnet->handle.mem_req.flexidag_output_buffer_size[output_index];
#endif
                one_line = p_dim->width * (1 << p_dt_fmt->size);
                no_padding_size = p_dim->plane * p_dim->depth * p_dim->height * one_line;

                data_size = output->size;
                out_addr = (uint8_t*)output->data_virt;

                //tvm_log("data_size = %u, padding_size = %u, no_padding_size = %u\n", data_size, padding_size, no_padding_size);
                /* do memcpy and padding for kDLCPU */
                if (output->device_type != DEVICE_TYPE_AMBA) {
#if 1
                        /* Suppose outputs are always flattened. */
                        memcpy(out_addr, virt_addr, data_size);
#else
                        if (data_size == padding_size) {
                                memcpy(out_addr, virt_addr, data_size);
                        } else if (data_size == no_padding_size) {
                                for (p = 0; p < p_dim->plane; p++) {
                                        for (d = 0; d < p_dim->depth; d++) {
                                                for (h = 0; h < p_dim->height; h++) {
                                                        memcpy(out_addr, virt_addr, one_line);
                                                        virt_addr += p_dim->pitch;
                                                        out_addr += one_line;
                                                }
                                        }
                                }
                        }
#endif
                }
        } while(0);

        return rval;
}

int DeleteAmbaTVM(amba_engine_cfg_t *engine_cfgs, uint32_t num)
{
        int rval = 0;
        tvm_context_t *pctrl = &G_tvm_ctx;
        tvm_net_match_t *pnet = NULL;
        unsigned long net_id = -1;

        TVM_LOCK(&tvm_dev_lock);

        /* delete single network
         * this part will be passed if num == 0 */
        for (uint32_t i = 0; i < num; ++i) {
                net_id = engine_cfgs[i].engine_id;
                if ((pnet = tvm_get_net_pair(net_id)) == NULL) {
                        tvm_error("cannot find network 0x%lx\n", net_id);
                } else {
                        if (AmbaCV_FlexidagClose((AMBA_CV_FLEXIDAG_HANDLE_s *) net_id)) {
                                tvm_error("exit network [0x%lx]\n", net_id);
                        }
                        if (tvm_free_engine_cv_mem(pnet)) {
                                tvm_error("free network [0x%lx] cv mem\n", net_id);
                        }

                        TVM_LOCK(&pctrl->net_list_lock);
                        list_del(&pnet->net_node);
                        TVM_UNLOCK(&pctrl->net_list_lock);

                        free(pnet);
                        pnet = NULL;
                }
        }

        /* shut down schedulerv when all network are freed or no net is created successfully */
        --pctrl->mod_num;
        if (0 == pctrl->mod_num) {
                rval = AmbaCV_SchdrShutdown(0);
                if(rval != 0) {
                        tvm_error("DeleteAmbaTVM: AmbaCV_SchdrShutdown fail. rval =%u\n", rval);
                } else {
                        pctrl->init_done = 0;
                }
        }
        TVM_UNLOCK(&tvm_dev_lock);

        return rval;
}

int CheckAmbaEngineInputName(amba_engine_cfg_t *engine_cfg,
        const char *input_name)
{
        int rval = 0;

#if 0
        struct net_input_cfg net_in;
        int input_index = -1, i = 0;

        do {
                if (engine_cfg == NULL || input_name == NULL) {
                        tvm_error("input null pointer.\n");
                        rval = -1;
                        break;
                }
                if (nnctrl_get_net_io_cfg(engine_cfg->engine_id, &net_in, NULL)) {
                        tvm_error("failed to get net io cfg\n");
                        rval = -1;
                        break;
                }
                for (i = 0; i < net_in.in_num; ++ i) {
                        if (strcmp(input_name, net_in.in_desc[i].name) == 0) {
                                input_index = i;
                                break;
                        }
                }
                if (input_index < 0) {
                        tvm_error("cannot find matched input [%s]\n", input_name);
                        rval = -1;
                        break;
                }
        } while(0);
#endif
        return rval;
}

int CheckAmbaEngineOutputName(amba_engine_cfg_t *engine_cfg,
        const char *output_name)
{
        int rval = 0;

#if 0
        struct net_output_cfg net_out;
        int output_index = -1, i = 0;

        do {
                if (engine_cfg == NULL || output_name == NULL) {
                        tvm_error("output null pointer.\n");
                        rval = -1;
                        break;
                }
                if (nnctrl_get_net_io_cfg(engine_cfg->engine_id, NULL, &net_out)) {
                        tvm_error("failed to get net io cfg\n");
                        rval = -1;
                        break;
                }
                for (i = 0; i < net_out.out_num; ++ i) {
                        if (strcmp(output_name, net_out.out_desc[i].name) == 0) {
                                output_index = i;
                                break;
                        }
                }
                if (output_index < 0) {
                        tvm_error("cannot find matched output [%s]\n", output_name);
                        rval = -1;
                        break;
                }
        } while(0);
#endif
        return rval;
}

int ConfigAmbaEngineLocation(const char *dirpath)
{
        int rval = 0;
        tvm_context_t *pctrl = &G_tvm_ctx;
        DIR* dir = NULL;
        struct dirent* ptr = NULL;
        char *pos = NULL, *dname = NULL;
        char cus_env_val[TVM_ENV_VAL_MAX_LEN] = "";
        char ld_lib_env[TVM_ENV_VAL_MAX_LEN] = "";
        char *abs_dirpath = NULL;

        do {
                // env of flexibin
                if ((dir = opendir(dirpath)) == NULL) {
                        tvm_error("cannot open folder %s.\n", dirpath);
                        rval = -1; break;
                }
                while((ptr = readdir(dir)) != NULL) {
                        dname = ptr->d_name;
                        pos = strrchr(dname, '.');
                        if (pos && (strcmp(pos + 1, "amba") == 0)) {
                                if (strlen(dname) >= TVM_ENV_KEY_MAX_LEN) {
                                        tvm_error("flexibin filename is too long.\n");
                                        rval = -1; break;
                                }
                                if ((strlen(dirpath) + strlen(dname) + 1) >= TVM_ENV_VAL_MAX_LEN) {
                                        tvm_error("flexibin filenpath is too long.\n");
                                        rval = -1; break;
                                }
                                snprintf(cus_env_val, TVM_ENV_VAL_MAX_LEN, "%s/%s", dirpath, dname);
                                break;
                        }
                }
                if (rval) break;

                // realpath is MT-safe
                if ((abs_dirpath = realpath(dirpath, NULL)) == NULL) {
                        tvm_error("interpreting dirpath.\n");
                        rval = -1; break;
                }
                // setenv is MT-Unsafe
                TVM_LOCK(&pctrl->mod_env_lock);
                if (strlen(cus_env_val)) {
                        if (setenv(dname, cus_env_val, 0) < 0) {
                                tvm_error("interpreting dirpath.\n");
                                rval = -1; break;
                        }
                }
                // LD_LIBRARY_PATH
                char *base_ld_env = getenv("LD_LIBRARY_PATH");

                if (base_ld_env) {
                        if ((strlen(abs_dirpath) + strlen(base_ld_env) + 1) >= TVM_ENV_VAL_MAX_LEN) {
                                tvm_error("dirpath is too deep.\n");
                                rval = -1; break;
                        }
                        snprintf(ld_lib_env, TVM_ENV_VAL_MAX_LEN, "%s:%s", abs_dirpath, base_ld_env);
                } else {
                        if (strlen(abs_dirpath) >= TVM_ENV_VAL_MAX_LEN) {
                                tvm_error("dirpath is too deep.\n");
                                rval = -1; break;
                        }
                        snprintf(ld_lib_env, TVM_ENV_VAL_MAX_LEN, "%s", abs_dirpath);
                }
                if (setenv("LD_LIBRARY_PATH", ld_lib_env, 1) < 0) {
                        tvm_error("interpreting dirpath.\n");
                        rval = -1; break;
                }
                TVM_UNLOCK(&pctrl->mod_env_lock);
        } while(0);

        if (dir) {
                closedir(dir);
        }
        if (abs_dirpath) {
                free(abs_dirpath);
        }

        return rval;
}

void* AmbaDeviceAlloc(unsigned long nbytes, unsigned long alignment)
{
        /* allocated address must be a multiple of alignment;
         * alignment is unnecessary for cavalry_mem_alloc since it's always page-aligned */
        /* cache is always enable */
        uint8_t cache_en = 1;
        uint32_t rval, status;
        AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg;
        amba_mem_blk_t* pBlk = NULL;

        //tvm_log("AmbaDeviceAlloc %d bytes ...\n", nbytes);

        TVM_LOCK(&tvm_dev_lock);
        do {
                rval = AmbaCV_SchdrDrvState(&status);
                if (rval != 0) {
                        tvm_error("AmbaCV_SchdrDrvState failed. rval = %d\n", rval);
                } else {
                        if (status == 0x0) {
                                /* init scheduler */
                                cfg.cpu_map = 0x9;
                                cfg.log_level = LVL_DEBUG;
                                rval = AmbaCV_FlexidagSchdrStart(&cfg);
                                if (rval != 0) {
                                        tvm_error("AmbaCV_FlexidagSchdrStart failed. rval = %d\n", rval);
                                }
                        }
                }
        } while(0);
        TVM_UNLOCK(&tvm_dev_lock);

        pBlk = malloc(sizeof(amba_mem_blk_t));
        if(pBlk == 0) {
                tvm_error("AmbaDeviceAlloc: malloc fail.\n");
        } else {
                rval = tvm_alloc_cv_mem(&nbytes, &pBlk->memblk, cache_en);
                if(rval != 0) {
                        tvm_error("AmbaDeviceAlloc: tvm_alloc_cv_mem fail. rval =%u\n", rval);
                } else {
                        memset(pBlk->memblk.pBuffer, 0, nbytes);
                }
                INIT_LIST_HEAD(&pBlk->list);
                LIST_LOCK(&list_lock);
                list_add_tail(&pBlk->list, &head);
                LIST_UNLOCK(&list_lock);
        }

        return pBlk->memblk.pBuffer;
}

int AmbaDeviceFree(void* ptr)
{
        int rval = 0;
        amba_mem_blk_t *pblk = NULL, *_pblk = NULL;
        flexidag_memblk_t *pfree = NULL;

        do {
                if (!ptr) {
                        tvm_error("invalid ptr address\n");
                        rval = -1;
                        break;
                }

                LIST_LOCK(&list_lock);
                if (!list_empty(&head)) {
                        list_for_each_entry_safe(pblk, _pblk, &head, list) {
                                if (pblk->memblk.pBuffer == ptr) {
                                        if (pblk) {
                                                pfree = &pblk->memblk;
                                                list_del(&pblk->list);
                                                free(pblk);
                                        }
                                        break;
                                }
                        }
                }
                LIST_UNLOCK(&list_lock);

                if (AmbaCV_UtilityCmaMemFree(pfree) != 0) {
                        tvm_error("AmbaDeviceFree: AmbaCV_UtilityCmaMemFree\n");
                        rval = -1;
                        break;
                }
        } while(0);

        return rval;
}

