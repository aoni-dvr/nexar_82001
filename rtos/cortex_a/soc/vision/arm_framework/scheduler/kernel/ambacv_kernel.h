/**
 *  @file visorc_kernel.h
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
 *  @details Definitions & Constants for Visorc kernel
 *
 */

#ifndef  AMBACV_KERNEL_H
#define  AMBACV_KERNEL_H

void krn_ambacv_set_sysinit(visorc_init_params_t *ptr);
visorc_init_params_t *krn_ambacv_get_sysinit(void);
ambacv_log_t *krn_ambacv_get_log(void);
ambacv_all_mem_t *krn_ambacv_get_mem(void);
void krn_schdrmsg_log_msg(const armvis_msg_t *msg);

uint32_t krn_ambacv_find_token(uint32_t *token);
uint32_t krn_ambacv_wait_token(uint32_t token_id);
uint32_t krn_ambacv_release_token(uint32_t token_id, uint32_t function_retcode);

uint32_t krn_ambacv_ioctl(void *f, uint32_t cmd, void * arg);
uint32_t krn_ambacv_init(void);
void krn_ambacv_exit(void);

#endif  //AMBACV_KERNEL_H