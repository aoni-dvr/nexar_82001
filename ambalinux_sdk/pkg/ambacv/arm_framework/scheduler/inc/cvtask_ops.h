/**
 *  @file cvtask_ops.h
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
 *  @details Definitions & Constants for cvtask ops
 *
 */

#ifndef CVTASK_OPS_H_FILE
#define CVTASK_OPS_H_FILE

#ifndef DISABLE_ARM_CVTASK
uint32_t is_blockable_cvtask(uint32_t cvtask_id);
uint32_t cvtask_set_cluster_id(uint32_t cid);
uint32_t cvtask_execute_request(void *arg);
uint32_t cvtask_init_modules(void);
uint32_t cvtask_log_flush(uint64_t state_addr);
uint32_t cvtask_get_scratchpad_size(void);
uint32_t cvtask_get_total_class(void);
#else
static inline uint32_t is_blockable_cvtask(uint32_t cvtask_id)
{
    (void) cvtask_id;
    return ERRCODE_NONE;
}
static inline uint32_t cvtask_set_cluster_id(uint32_t cluster_id)
{
    (void) cluster_id;
    return ERRCODE_NONE;
}
static inline uint32_t cvtask_execute_request(void *arg)
{
    (void) arg;
    return ERRCODE_NONE;
}
static inline uint32_t cvtask_init_modules(void)
{
    return ERRCODE_NONE;
}
static inline uint32_t cvtask_log_flush(uint64_t state_addr)
{
    (void) state_addr;
    return ERRCODE_NONE;
}
static inline uint32_t cvtask_get_scratchpad_size(void)
{
    return 0U;
}
static inline uint32_t cvtask_get_total_class(void)
{
    return 0U;
}
#endif

#endif /* ?CVTASK_OPS_H_FILE */

