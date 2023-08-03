/**
 *  @file cavalry.h
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
 *  @details Definitions & Constants for Cavalry APIs
 *
 */

#ifndef CAVALRY_H
#define CAVALRY_H

void cavalry_system_init(void);
uint32_t cavalry_resume(uint32_t version);
int32_t drv_cavalry_query_buf(void *pQueryBuf);
int32_t drv_cavalry_start_vp(void *vpUserData);
int32_t drv_cavalry_stop_vp(void *vpUserData);
int32_t drv_cavalry_run_dags(void *vpParams);
int32_t drv_cavalry_start_log(void *vpUserData);
int32_t drv_cavalry_stop_log(void *vpUserData);
int32_t drv_cavalry_early_quit(void *pEarlyQuit);
int32_t drv_cavalry_alloc_mem(void *pCavalryMem);
int32_t drv_cavalry_free_mem(void *pCavalryMem);
int32_t drv_cavalry_sync_cache_mem(void *pCavalryCacheMem);
int32_t drv_cavalry_get_usage_mem(void *pCavalryUsageMem);
int32_t drv_cavalry_run_hotlink_slot(void *pCavalryHotlinkSlot);
int32_t drv_cavalry_set_hotlink_slot_cfg(void *pCavalrySlotCfg);
int32_t drv_cavalry_get_hotlink_slot_cfg(void *pCavalrySlotCfg);
int32_t drv_cavalry_enable(uint32_t num_slots);
int32_t drv_cavalry_associate_buf(uint64_t cavalry_base_daddr);
int32_t drv_cavalry_set_memory_block(armvis_msg_t *pMsg);
int32_t drv_cavalry_process_reply(const armvis_msg_t *pMsg);

#endif //CAVALRY_H

