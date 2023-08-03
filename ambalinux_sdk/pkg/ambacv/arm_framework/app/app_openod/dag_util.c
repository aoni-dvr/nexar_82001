#include <stdio.h>
#include <stdlib.h>

#include "mem_util.h"
#include "dag_util.h"

/**
 * Create a flexidag instance.
 * This function will load and open a flexidag bin file.
 **/
uint32_t DagUtil_CreateDag(char *path, FLEXIDAG_INSTANCE_s *fd_inst)
{
    uint32_t ret = 0;
    uint32_t size_align;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle;

    fd_gen_handle = &fd_inst->handle;

    ret = AmbaCV_UtilityFileSize(path, &size_align);
    if (ret != 0) {
        printf("DagUtil_CreateDag: AmbaCV_UtilityFileSize() fail. path=%s\n", path);
        ret = 1U;
    } else {
        if(fd_inst->bin_buf.pBuffer == NULL) {
            ret = MemUtil_MemblkAlloc(size_align, &fd_inst->bin_buf);
        }

        if (ret != 0) {
            printf("DagUtil_CreateDag: MemUtil_MemblkAlloc() fail. path = %s\n", path);
            ret = 1U;
        } else {
            ret = AmbaCV_UtilityFileLoad(path, &fd_inst->bin_buf);
            if (ret != 0) {
                printf("DagUtil_CreateDag: AmbaCV_UtilityFileLoad() fail. path = %s\n", path);
                ret = 1U;
            } else {
                ret = AmbaCV_FlexidagOpen(&fd_inst->bin_buf, fd_gen_handle);
                if (ret != 0) {
                    printf("DagUtil_CreateDag: AmbaCV_FlexidagOpen() fail. path = %s size_align = %d\n", path, size_align);
                    ret = 1U;
                }
            }
        }
    }

    return ret;
}

/**
 * init a flexidag instance.
 * This function will prepare resouorce for a flexidag instance.
 **/
uint32_t DagUtil_InitDag(FLEXIDAG_INSTANCE_s *fd_inst, flexidag_memblk_t *temp_buf)
{
    uint32_t ret = 0;
    uint32_t i;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle;

    fd_gen_handle = &fd_inst->handle;

    /* prepare state buffer */
    if(fd_inst->init.state_buf.pBuffer == NULL) {
        ret = MemUtil_MemblkAlloc(CACHE_ALIGN(fd_gen_handle->mem_req.flexidag_state_buffer_size), &fd_inst->init.state_buf);
        if (ret != 0) {
            printf("DagUtil_InitDag: MemUtil_MemblkAlloc state_buf fail.\n");
            ret = 1U;
        }
    }

    /* setup temp buffer */
    if (ret == 0) {
        if(fd_inst->init.temp_buf.pBuffer == NULL) {
            //all dags share same temp buffer
            fd_inst->init.temp_buf.buffer_size = temp_buf->buffer_size;
            fd_inst->init.temp_buf.buffer_daddr = temp_buf->buffer_daddr;
            fd_inst->init.temp_buf.buffer_cacheable = temp_buf->buffer_cacheable;
            fd_inst->init.temp_buf.pBuffer = temp_buf->pBuffer;
            if (ret != 0) {
                printf("DagUtil_InitDag: assign temp_buf fail\n");
                ret = 1U;
            }
        }
    }

    /* input should be configured separately (it is dag specific) */
    if (ret == 0) {
        fd_inst->in.num_of_buf = 0;
    }

    /* prepare output buffer */
    if (ret == 0) {
        fd_inst->out.num_of_buf = fd_gen_handle->mem_req.flexidag_num_outputs;
        for(i = 0; i < fd_gen_handle->mem_req.flexidag_num_outputs; i++) {
            if(fd_inst->out.buf[i].pBuffer == NULL) {
                ret = MemUtil_MemblkAlloc(CACHE_ALIGN(fd_gen_handle->mem_req.flexidag_output_buffer_size[i]), &fd_inst->out.buf[i]);
            }

            if (ret != 0) {
                printf("DagUtil_InitDag: MemUtil_MemblkAlloc() out_buf %d fail.\n", i);
                ret = 1U;
                break;
            }
        }
    }

    /* init */
    if (ret == 0) {
        ret = AmbaCV_FlexidagInit(fd_gen_handle, &fd_inst->init);
        if (ret != 0) {
            printf("DagUtil_InitDag: AmbaCV_FlexidagInit fail.\n");
            ret = 1U;
        }
    }

    return ret;
}

/**
 * release a flexidag instance.
 * This function will close and release resouorce for a flexidag instance except input buffer.
 **/
uint32_t DagUtil_ReleaseDag(FLEXIDAG_INSTANCE_s *fd_inst)
{
    uint32_t ret = 0U;
    uint32_t i;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle;

    fd_gen_handle = &fd_inst->handle;

    /* close flexidag */
    ret = AmbaCV_FlexidagClose(fd_gen_handle);
    if (ret != 0) {
        printf("DagUtil_ReleaseDag: AmbaCV_FlexidagClose fail.\n");
        ret = 1U;
    }

    /* release resource */
    ret = MemUtil_MemblkFree(&fd_inst->bin_buf);
    if(ret != 0 ) {
        printf("DagUtil_ReleaseDag: MemUtil_MemblkFree bin_buf fail.\n");
        ret = 1U;
    }

    ret = MemUtil_MemblkFree(&fd_inst->init.state_buf);
    if(ret != 0) {
        printf("DagUtil_ReleaseDag: MemUtil_MemblkFree state_buf fail.\n");
        ret = 1U;
    }

    for(i = 0; i < fd_inst->out.num_of_buf; i++) {
        if(fd_inst->out.buf[i].pBuffer != NULL) {
            ret = MemUtil_MemblkFree(&fd_inst->out.buf[i]);
            if(ret != 0) {
                printf("DagUtil_ReleaseDag: MemUtil_MemblkFree out_buf %u fail.\n", i);
                ret = 1U;
                break;
            }
        }
    }

    return ret;
}

