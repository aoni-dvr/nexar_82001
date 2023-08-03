/**
 *  @file flexidag_internal.h
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
 *  @details Definitions & Constants for Flexidag internal APIs
 *
 */

#ifndef FLEXIDAG_INTERNAL_H
#define FLEXIDAG_INTERNAL_H

/*-= Flexidag interface =-*/
uint32_t drv_flexidag_system_init(void);
uint32_t drv_flexidag_enable(uint32_t num_slots);
uint32_t drv_flexidag_create(void **vppHandle, uint32_t *pSlotId);
uint32_t drv_flexidag_set_parameter(void *vpHandle, uint32_t parameter_id, uint32_t parameter_value);
uint32_t drv_flexidag_add_tbar(void *vpHandle, flexidag_memblk_t *pblk_tbar);
uint32_t drv_flexidag_add_sfb(void *vpHandle, flexidag_memblk_t *pblk_sfb);
uint32_t drv_flexidag_open(void *vpHandle, flexidag_memblk_t *pFlexiBlk, flexidag_memory_requirements_t *pFlexiRequirements);
uint32_t drv_flexidag_set_state_buffer(void *vpHandle, flexidag_memblk_t *pFlexiBlk);
uint32_t drv_flexidag_set_temp_buffer(void *vpHandle, flexidag_memblk_t *pFlexiBlk);
uint32_t drv_flexidag_init(void *vpHandle);
uint32_t drv_flexidag_prep_run(void *vpHandle, uint32_t *pTokenId);
uint32_t drv_flexidag_set_input_buffer(void *vpHandle, uint32_t input_num, flexidag_memblk_t *pFlexiBlk);
uint32_t drv_flexidag_set_feedback_buffer(void *vpHandle, uint32_t feedback_num, flexidag_memblk_t *pFlexiBlk);
uint32_t drv_flexidag_set_output_buffer(void *vpHandle, uint32_t output_num, flexidag_memblk_t *pFlexiBlk);
uint32_t drv_flexidag_run(void *vpHandle, flexidag_runinfo_t *pRunInfo);
uint32_t drv_flexidag_run_noblock(void *vpHandle, uint32_t *pTokenId);
uint32_t drv_flexidag_wait_run_finish(void *vpHandle, uint32_t token_id, flexidag_runinfo_t *pRunInfo);
uint32_t drv_flexidag_close(void *vpHandle);
uint32_t drv_flexidag_show_info(void *vpHandle);

#endif //FLEXIDAG_INTERNAL_H

