/**
 *  @file flexidag_drv.c
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
 *  @details Flexidag driver APIs
 *
 */

#include "schdr.h"
#include "schdr_internal.h"
#include "flexidag.h"
#include "flexidag_internal.h"
#include "os_api.h"
#include "dram_mmap.h"
#include "ambacv_kal.h"
#include "schdr_util.h"

/*------------------------------------------------------------------------------------------------*/
/*-= Flexidag related driver interface =----------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
#if defined(AMBACV_KERNEL_SUPPORT) || defined(RTOS_BUILD)
uint32_t drv_flexidag_enable(uint32_t num_slots)
{
    return ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_ENABLE, &num_slots);
} /* drv_flexidag_enable() */

uint32_t  drv_flexidag_create(void **vppHandle, uint32_t *pSlotId)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_create_t arg_create = {0};

    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_create() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if (vppHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_create() : vppHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else if (pSlotId == NULL) {
        console_printU5("[ERROR] drv_flexidag_create() : pSlotId == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        arg_create.is_blocking = 1U;
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_CREATE, &arg_create);
        if(retcode == ERRCODE_NONE) {
            typecast(vppHandle,&arg_create.handle);
            *pSlotId = arg_create.slot;
        }
    }
    return retcode;
} /* drv_flexidag_create() */

uint32_t  drv_flexidag_set_parameter(void *vpHandle, uint32_t parameter_id, uint32_t parameter_value)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_param_t arg_param = {0};

    (void) vpHandle;
    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_set_parameter() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_set_parameter() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else {
        typecast(&arg_param.handle,&vpHandle);
        arg_param.id = parameter_id;
        arg_param.value = parameter_value;
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_SET_PARAMETER, &arg_param);
    }
    return retcode;
} /* drv_flexidag_set_parameter() */

uint32_t  drv_flexidag_add_tbar(void *vpHandle, flexidag_memblk_t *pblk_tbar)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_memblk_t arg_memblk = {0};

    (void) vpHandle;
    (void) pblk_tbar;
    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_add_tbar() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_add_tbar() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if(pblk_tbar == NULL) {
        console_printU5("[ERROR] drv_flexidag_add_tbar() : pblk_tbar == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        typecast(&arg_memblk.handle,&vpHandle);
        arg_memblk.mem_blk.buffer_daddr = pblk_tbar->buffer_daddr;
        arg_memblk.mem_blk.buffer_cacheable = pblk_tbar->buffer_cacheable;
        arg_memblk.mem_blk.buffer_size = pblk_tbar->buffer_size;
        typecast(&arg_memblk.mem_blk.buffer_vaddr,&pblk_tbar->pBuffer);
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_ADD_TBAR, &arg_memblk);
    }
    return retcode;
} /* drv_flexidag_add_tbar() */

uint32_t  drv_flexidag_add_sfb(void *vpHandle, flexidag_memblk_t *pblk_sfb)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_memblk_t arg_memblk = {0};

    (void) vpHandle;
    (void) pblk_sfb;
    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_add_sfb() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_add_sfb() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if(pblk_sfb == NULL) {
        console_printU5("[ERROR] drv_flexidag_add_sfb() : pblk_sfb == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        typecast(&arg_memblk.handle,&vpHandle);
        arg_memblk.mem_blk.buffer_daddr = pblk_sfb->buffer_daddr;
        arg_memblk.mem_blk.buffer_cacheable = pblk_sfb->buffer_cacheable;
        arg_memblk.mem_blk.buffer_size = pblk_sfb->buffer_size;
        typecast(&arg_memblk.mem_blk.buffer_vaddr,&pblk_sfb->pBuffer);
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_ADD_SFB, &arg_memblk);
    }
    return retcode;
} /* drv_flexidag_add_tbar() */

uint32_t  drv_flexidag_open(void *vpHandle, flexidag_memblk_t *pFlexiBlk, flexidag_memory_requirements_t *pFlexiRequirements)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_open_t arg_open = {0};

    (void) vpHandle;
    (void) pFlexiBlk;
    (void) pFlexiRequirements;
    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_open() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_open() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if(pFlexiBlk == NULL) {
        console_printU5("[ERROR] drv_flexidag_open() : pFlexiBlk == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else if(pFlexiRequirements == NULL) {
        console_printU5("[ERROR] drv_flexidag_open() : pFlexiRequirements == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
#ifdef ASIL_COMPLIANCE
        retcode = schdr_check_flexibin(pFlexiBlk->pBuffer, pFlexiBlk->buffer_size);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] drv_flexidag_open() : invalid flexibin", 0U, 0U, 0U, 0U, 0U);
        } else
#endif
        {

            arg_open.is_blocking = 1U;
            typecast(&arg_open.handle,&vpHandle);
            arg_open.mem_blk.buffer_daddr = pFlexiBlk->buffer_daddr;
            arg_open.mem_blk.buffer_cacheable = pFlexiBlk->buffer_cacheable;
            arg_open.mem_blk.buffer_size = pFlexiBlk->buffer_size;
            typecast(&arg_open.mem_blk.buffer_vaddr,&pFlexiBlk->pBuffer);
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_OPEN, &arg_open);
            if(retcode == ERRCODE_NONE) {
                retcode = thread_memcpy(pFlexiRequirements, &arg_open.mem_req, sizeof(flexidag_memory_requirements_t));
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] drv_flexidag_open() : thread_memcpy fail", 0U, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_FLEXIDAG_MEMCPY_FAIL;
                }
            }
        }
    }
    return retcode;
} /* drv_flexidag_open() */

uint32_t  drv_flexidag_set_state_buffer(void *vpHandle, flexidag_memblk_t *pFlexiBlk)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_memblk_t arg_memblk = {0};

    (void) vpHandle;
    (void) pFlexiBlk;
    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_set_state_buffer() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_set_state_buffer() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if(pFlexiBlk == NULL) {
        console_printU5("[ERROR] drv_flexidag_set_state_buffer() : pFlexiBlk == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        typecast(&arg_memblk.handle,&vpHandle);
        arg_memblk.mem_blk.buffer_daddr = pFlexiBlk->buffer_daddr;
        arg_memblk.mem_blk.buffer_cacheable = pFlexiBlk->buffer_cacheable;
        arg_memblk.mem_blk.buffer_size = pFlexiBlk->buffer_size;
        typecast(&arg_memblk.mem_blk.buffer_vaddr,&pFlexiBlk->pBuffer);
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_SET_STATE_BUFFER, &arg_memblk);
    }
    return retcode;
} /* drv_flexidag_set_state_buffer() */

uint32_t  drv_flexidag_set_temp_buffer(void *vpHandle, flexidag_memblk_t *pFlexiBlk)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_memblk_t arg_memblk = {0};

    (void) vpHandle;
    (void) pFlexiBlk;
    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_set_temp_buffer() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_set_temp_buffer() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if(pFlexiBlk == NULL) {
        console_printU5("[ERROR] drv_flexidag_set_temp_buffer() : pFlexiBlk == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        typecast(&arg_memblk.handle,&vpHandle);
        arg_memblk.mem_blk.buffer_daddr = pFlexiBlk->buffer_daddr;
        arg_memblk.mem_blk.buffer_cacheable = pFlexiBlk->buffer_cacheable;
        arg_memblk.mem_blk.buffer_size = pFlexiBlk->buffer_size;
        typecast(&arg_memblk.mem_blk.buffer_vaddr,&pFlexiBlk->pBuffer);
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_SET_TEMP_BUFFER, &arg_memblk);
    }
    return retcode;
} /* drv_flexidag_set_temp_buffer() */

uint32_t  drv_flexidag_init(void *vpHandle)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_handle_t arg_handle = {0};

    (void) vpHandle;
    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_init() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_init() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else {
        arg_handle.is_blocking = 1U;
        typecast(&arg_handle.handle,&vpHandle);
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_INIT, &arg_handle);
    }
    return retcode;
} /* drv_flexidag_init() */

uint32_t  drv_flexidag_prep_run(void *vpHandle, uint32_t *pTokenId)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_pre_run_t arg_pre_run = {0};

    (void) vpHandle;
    (void) pTokenId;
    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_prep_run() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_prep_run() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else {
        typecast(&arg_pre_run.handle,&vpHandle);
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_PREP_RUN, &arg_pre_run);
        if((retcode == ERRCODE_NONE) && (pTokenId != NULL)) {
            *pTokenId = arg_pre_run.token_id;
        }
    }
    return retcode;
} /* drv_flexidag_prep_run() */

uint32_t  drv_flexidag_set_input_buffer(void *vpHandle, uint32_t input_num, flexidag_memblk_t *pFlexiBlk)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_io_memblk_t arg_io_memblk = {0};

    (void) vpHandle;
    (void) pFlexiBlk;
    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_set_input_buffer() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_set_input_buffer() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if(pFlexiBlk == NULL) {
        console_printU5("[ERROR] drv_flexidag_set_input_buffer() : pFlexiBlk == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        typecast(&arg_io_memblk.handle,&vpHandle);
        arg_io_memblk.num = input_num;
        arg_io_memblk.mem_blk.buffer_daddr = pFlexiBlk->buffer_daddr;
        arg_io_memblk.mem_blk.buffer_cacheable = pFlexiBlk->buffer_cacheable;
        arg_io_memblk.mem_blk.buffer_size = pFlexiBlk->buffer_size;
        typecast(&arg_io_memblk.mem_blk.buffer_vaddr,&pFlexiBlk->pBuffer);
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_SET_INPUT_BUFFER, &arg_io_memblk);
    }
    return retcode;
} /* drv_flexidag_set_input_buffer() */

uint32_t  drv_flexidag_set_feedback_buffer(void *vpHandle, uint32_t feedback_num, flexidag_memblk_t *pFlexiBlk)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_io_memblk_t arg_io_memblk = {0};

    (void) vpHandle;
    (void) pFlexiBlk;
    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_set_feedback_buffer() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_set_feedback_buffer() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if(pFlexiBlk == NULL) {
        console_printU5("[ERROR] drv_flexidag_set_feedback_buffer() : pFlexiBlk == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        typecast(&arg_io_memblk.handle,&vpHandle);
        arg_io_memblk.num = feedback_num;
        arg_io_memblk.mem_blk.buffer_daddr = pFlexiBlk->buffer_daddr;
        arg_io_memblk.mem_blk.buffer_cacheable = pFlexiBlk->buffer_cacheable;
        arg_io_memblk.mem_blk.buffer_size = pFlexiBlk->buffer_size;
        typecast(&arg_io_memblk.mem_blk.buffer_vaddr,&pFlexiBlk->pBuffer);
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_SET_FEEDBACK_BUFFER, &arg_io_memblk);
    }
    return retcode;
} /* drv_flexidag_set_feedback_buffer() */

uint32_t  drv_flexidag_set_output_buffer(void *vpHandle, uint32_t output_num, flexidag_memblk_t *pFlexiBlk)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_io_memblk_t arg_io_memblk = {0};
    uint32_t slot_id = FLEXIDAG_INVALID_SLOT_ID;
    const flexidag_memblk_t *pTempOut = NULL;
    static flexidag_memblk_t flexidag_output_temp[FLEXIDAG_MAX_SLOTS][FLEXIDAG_MAX_OUTPUTS] GNU_SECTION_NOZEROINIT;

    (void) vpHandle;
    (void) pFlexiBlk;
    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_set_output_buffer() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_set_output_buffer() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if(pFlexiBlk == NULL) {
        console_printU5("[ERROR] drv_flexidag_set_output_buffer() : pFlexiBlk == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        slot_id = flexidag_find_slot_by_vphandle(vpHandle);
        if(slot_id == FLEXIDAG_INVALID_SLOT_ID) {
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        } else {
            typecast(&arg_io_memblk.handle,&vpHandle);
            arg_io_memblk.num = output_num;
            arg_io_memblk.mem_blk.buffer_daddr = pFlexiBlk->buffer_daddr;
            arg_io_memblk.mem_blk.buffer_cacheable = pFlexiBlk->buffer_cacheable;
            arg_io_memblk.mem_blk.buffer_size = pFlexiBlk->buffer_size;
            typecast(&arg_io_memblk.mem_blk.buffer_vaddr,&pFlexiBlk->pBuffer);
            if(thread_memcpy(&flexidag_output_temp[slot_id][output_num], pFlexiBlk, sizeof(flexidag_memblk_t)) != ERRCODE_NONE) {
                retcode = ERR_DRV_FLEXIDAG_MEMCPY_FAIL;
            } else {
                pTempOut = &flexidag_output_temp[slot_id][output_num];
                typecast(&arg_io_memblk.orig_memBlk,&pTempOut);
                retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_SET_OUTPUT_BUFFER, &arg_io_memblk);
            }
        }
    }
    return retcode;
} /* drv_flexidag_set_output_buffer() */

uint32_t  drv_flexidag_run(void *vpHandle, flexidag_runinfo_t *pRunInfo)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_run_t arg_run = {0};

    (void) vpHandle;
    (void) pRunInfo;
    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_run() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_run() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if(pRunInfo == NULL) {
        console_printU5("[ERROR] drv_flexidag_run() : pRunInfo == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        typecast(&arg_run.handle,&vpHandle);
        arg_run.is_blocking = 1U;
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_RUN, &arg_run);
        if(retcode == ERRCODE_NONE) {
            if(thread_memcpy(pRunInfo, &arg_run.info, sizeof(flexidag_runinfo_t)) != ERRCODE_NONE) {
                retcode = ERR_DRV_FLEXIDAG_MEMCPY_FAIL;
            }
        }
    }
    return retcode;
} /* drv_flexidag_run() */

uint32_t  drv_flexidag_run_noblock(void *vpHandle, uint32_t *pTokenId)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_run_t arg_run = {0};

    (void) vpHandle;
    (void) pTokenId;
    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_run_noblock() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_run_noblock() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if(pTokenId == NULL) {
        console_printU5("[ERROR] drv_flexidag_run_noblock() : pTokenId == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        typecast(&arg_run.handle,&vpHandle);
        arg_run.is_blocking = 0U;
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_RUN, &arg_run);
        if(retcode == ERRCODE_NONE) {
            *pTokenId = arg_run.token_id;
        }
    }
    return retcode;
} /* drv_flexidag_run() */

uint32_t drv_flexidag_wait_run_finish(void *vpHandle, uint32_t token_id, flexidag_runinfo_t *pRunInfo)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_run_t arg_run = {0};

    (void) vpHandle;
    (void) pRunInfo;
    if ((schdr_sys_state.state != SCHDR_STATE_ACTIVE) || ((schdr_sys_state.module_init & FLEXIDAG_MODULE) == 0U)) {
        console_printU5("[ERROR] drv_flexidag_wait_run_finish() : flexidag not started state %d module_init 0x%x \n", schdr_sys_state.state, schdr_sys_state.module_init, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
    } else if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_wait_run_finish() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if(pRunInfo == NULL) {
        console_printU5("[ERROR] drv_flexidag_wait_run_finish() : pRunInfo == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        typecast(&arg_run.handle,&vpHandle);
        arg_run.token_id = token_id;
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_WAIT_RUN_FINISH, &arg_run);
        if(retcode == ERRCODE_NONE) {
            if( thread_memcpy(pRunInfo, &arg_run.info, sizeof(flexidag_runinfo_t)) != ERRCODE_NONE) {
                retcode = ERR_DRV_FLEXIDAG_MEMCPY_FAIL;
            }
        }
    }
    return retcode;
} /* drv_flexidag_wait_run_finish() */

uint32_t  drv_flexidag_close(void *vpHandle)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_handle_t arg_handle = {0};

    (void) vpHandle;
    if(vpHandle == NULL) {
        console_printU5("[ERROR] drv_flexidag_close() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else {
        arg_handle.is_blocking = 1U;
        typecast(&arg_handle.handle,&vpHandle);
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_CLOSE, &arg_handle);
    }
    return retcode;
} /* drv_flexidag_close() */

uint32_t  drv_flexidag_show_info(void *vpHandle)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_handle_t arg_handle = {0};

    (void) vpHandle;
    if(vpHandle == NULL) {
        arg_handle.handle = 0UL;
    } else {
        typecast(&arg_handle.handle,&vpHandle);
    }
    retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_INFO, &arg_handle);
    return retcode;
} /* drv_flexidag_show_info() */

uint32_t drv_flexidag_process_kernel_reply(armvis_msg_t *pMsg)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) pMsg;
    if(pMsg == NULL) {
        console_printU5("[ERROR] drv_flexidag_process_kernel_reply() : pMsg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_VISMSG_INVALID;
    } else {
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_FLEXIDAG_HANDLE_MSG, pMsg);
    }
    return retcode;
} /* drv_flexidag_process_kernel_reply() */
#endif
