/**
 *  @file msg.h
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
 *  @details Definitions & Constants for Message APIs
 *
 */

#ifndef MSG_H
#define MSG_H

#define FNV1A_32_INIT_HASH              0x811C9DC5U

uint32_t schdrmsg_start(const visorc_init_params_t *sysconfig, uint32_t is_booting_proc);
uint32_t schdrmsg_send_msg(armvis_msg_t *msg, uint32_t size);
uint32_t schdrmsg_send_vpmsg(const idsp_vis_picinfo_t *msg);
void schdrmsg_log_msg(armvis_msg_t *msg);
uint32_t schdrmsg_stop(void);
uint32_t schdrmsg_waitfor_shutdown(void);
uint32_t schdrmsg_config_idsp(uint64_t vp_addr, uint64_t asyc_addr);
uint32_t schdrmsg_config_vin(uint64_t vin_addr);
uint32_t schdrmsg_get_orcarm_wi(uint32_t *ri);
uint32_t schdrmsg_calc_hash(const void* buf, uint32_t len, uint32_t hval);

#endif //MSG_H

