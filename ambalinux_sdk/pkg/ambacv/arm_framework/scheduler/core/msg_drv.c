/**
 *  @file schdrmsg_drv.c
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
 *  @details Scheduler message driver APIs
 *
 */

#include "os_api.h"
#include "dram_mmap.h"
#include "schdr.h"
#include "schdr_internal.h"
#include "msg.h"
#include "flexidag.h"
#include "cvapi_logger_interface.h"
#include "schdr_api.h"
#include "ambacv_kal.h"

#define MSG_INDEX_MASK          (ARMVIS_MESSAGE_NUM - 1U)

static thread_t             rx_thread;
static uint32_t             run_rx_thread = 0U;
static uint32_t             msg_param_init = 0U;
static uint32_t             msg_recv_ri = 0U;
static uint32_t             msg_recv_w_seqno = 0U;

/**
 *  @brief      Calculate hash by FNV-1a.
 *              hash = offset_basis
 *              for each octet_of_data to be hashed
 *                  hash = hash xor octet_of_data
 *                  hash = hash * FNV_prime
 *              return hash
 *
 *  @param      buf : [IN] ptr of buffer to do hash
 *  @param      len : [IN] size of buffer to do hash
 *
 *  @return     hash value
 */
uint32_t schdrmsg_calc_hash(const void* buf, uint32_t len, uint32_t hval)
{
    uint8_t *ptr = NULL;
    uint32_t count = 0U;
    uint32_t new_hval = 0U;

    (void) ptr;
    (void) buf;
    if((buf != NULL) && (len != 0U)) {
        typecast(&ptr, &buf);
        new_hval = hval;
        while (count < len) {
            new_hval = new_hval ^(uint32_t)ptr[count];
            new_hval = new_hval + (new_hval<<1) + (new_hval<<4) + (new_hval<<7) + (new_hval<<8) + (new_hval<<24);
            count++;
        }
    }
    return new_hval;
}

#if defined(AMBACV_KERNEL_SUPPORT) || defined(RTOS_BUILD)
static uint32_t schdrmsg_recv_orcmsg(uint32_t ri, uint32_t *wi)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_get_value_t arg;

    arg.index = ri;
    retcode = (uint32_t) ambacv_ioctl(ambacv_fd, AMBACV_RECV_ORCMSG, &arg);
    if(retcode != ERRCODE_NONE) {
        console_printU5("[ERROR] schdrmsg_recv_orcmsg() : AMBACV_RECV_ORCMSG fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
    }
    *wi = (uint32_t)arg.value;
    return retcode;
}

uint32_t schdrmsg_config_idsp(uint64_t vp_addr, uint64_t asyc_addr)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_cfg_addr_t arg;

    arg.addr[0] = vp_addr;
    arg.addr[1] = asyc_addr;
    retcode = ambacv_ioctl(ambacv_fd, AMBACV_CONFIG_IDSP, &arg);
    if(retcode != ERRCODE_NONE) {
        console_printU5("[ERROR] schdrmsg_config_idsp() : AMBACV_CONFIG_IDSP fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
    }
    return retcode;
}

uint32_t schdrmsg_send_vpmsg(const idsp_vis_picinfo_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_idspvis_msg_t arg;

    (void) msg;
    if(msg == NULL) {
        console_printU5("[ERROR] schdrmsg_send_vpmsg() : msg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_IDSP_INVALID;
    } else {
        retcode = thread_memcpy( &arg.msg, msg, sizeof(idsp_vis_picinfo_t));
        if (retcode == ERRCODE_NONE) {
            arg.msg_size = (uint32_t)sizeof(idsp_vis_picinfo_t);
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_SEND_VPMSG, &arg);
        }
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdrmsg_send_vpmsg() : AMBACV_SEND_VPMSG fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t schdrmsg_config_vin(uint64_t vin_addr)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_cfg_addr_t arg;

    arg.addr[0] = vin_addr;
    retcode = ambacv_ioctl(ambacv_fd, AMBACV_CONFIG_VIN, &arg);
    if(retcode != ERRCODE_NONE) {
        console_printU5("[ERROR] schdrmsg_config_vin() : AMBACV_CONFIG_VIN fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
    }
    return retcode;
}

uint32_t schdrmsg_send_vinmsg(const void *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_vinvis_msg_t arg = {0};

    (void) msg;
    if(msg == NULL) {
        console_printU5("[ERROR] schdrmsg_send_vinmsg() : msg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        retcode = thread_memcpy( &arg.msg_payload, msg, sizeof(arg.msg_payload));
        if (retcode == ERRCODE_NONE) {
            arg.msg_code = 0U;
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_SEND_VINMSG, &arg);
        }
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdrmsg_send_vinmsg() : AMBACV_SEND_VINMSG fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t schdrmsg_send_msg(armvis_msg_t *msg, uint32_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    void *ptr;

    (void) size;
    if(msg == NULL) {
        console_printU5("[ERROR] schdrmsg_send_msg() : msg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        typecast(&ptr,&msg);
        msg->hdr.message_type |= MSGTYPEBIT_ARM_TO_VISORC;
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_SEND_ARMMSG, ptr);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdrmsg_send_msg() : AMBACV_SEND_ARMMSG fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t schdrmsg_get_orcarm_wi(uint32_t *ri)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_get_value_t arg = {0};

    arg.index = AMBACV_ID_GET_ORC_CMD_WI;
    arg.value = 0U;
    retcode = ambacv_ioctl(ambacv_fd, AMBACV_GET_VALUE, &arg);
    *ri = (uint32_t)arg.value;

    return retcode;
}

static armvis_message_queue_t *msg_config = NULL;
static armvis_msg_t *msg_queue = NULL;
static uint32_t msg_ri = 0U, msg_wi = 0U;

static uint32_t schdr_wait_msg_init(void)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t i;
    const void *ptr;

    if(msg_param_init == 0U) {
        for (i = 0U; i < 4U; i++) {
            if (schdr_sys_state.visorc_init.MsgQueue_orc_to_arm_daddr[i] != 0U) {
                break;
            }
        }
        if(i < 4U) {
            ptr = ambacv_c2v(schdr_sys_state.visorc_init.MsgQueue_orc_to_arm_daddr[i]);
            if(ptr == NULL) {
                console_printU5("[ERROR] schdr_wait_msg_init() : MsgQueue_orc_to_arm_daddr->address == NULL\n", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_VISMSG_QUEUE_ORC2ARM_INVALID;
            } else {
                typecast(&msg_config,&ptr);
                ptr = ambacv_c2v(msg_config->msg_array_daddr);
                if(ptr == NULL) {
                    console_printU5("[ERROR] schdr_wait_msg_init() : pQueueConfig->address == NULL\n", 0U, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_VISMSG_QUEUE_ORC2ARM_INVALID;
                } else {
                    typecast(&msg_queue,&ptr);
                    msg_ri = msg_recv_ri;
                    msg_recv_w_seqno = msg_recv_ri;
                }
            }
        }
        msg_param_init = 1U;
    }

    return retcode;
}


uint32_t schdr_wait_msg(uint32_t *msg_num)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret,tmp_ri, count = 0U;

    *msg_num = 0U;
    if(run_rx_thread == 0U) {
        if( thread_sleep(10U) != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_wait_msg() : thread_sleep fail\n", 0U, 0U, 0U, 0U, 0U);
        }
    } else {
        retcode = schdr_wait_msg_init();
        if(retcode == ERRCODE_NONE) {
            if(schdr_sys_cfg.internal_thread == 1U) {
                console_printU5("[ERROR] schdr_wait_msg() : invalid to call this API schdr_with_thread %d \n", schdr_sys_cfg.internal_thread, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_INTERNAL_THREAD_STATE_MISMATCHED;
            } else {
                if(ambacv_fd < 0) {
                    ambacv_fd = ambacv_open("/dev/ambacv", O_RDONLY);
                }

                if (ambacv_fd >= 0) {
                    retcode = ambacv_ioctl(ambacv_fd, AMBACV_WAIT_ORCMSG, &ret);
                    if(retcode != ERRCODE_NONE) {
                        console_printU5("[ERROR] schdr_wait_msg() : AMBACV_WAIT_ORCMSG fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
                    }
                    if(retcode == ERRCODE_NONE) {
                        retcode = schdrmsg_recv_orcmsg(msg_ri,&msg_wi);
                        if(retcode != ERRCODE_NONE) {
                            console_printU5("[ERROR] schdr_wait_msg() : schdrmsg_recv_orcmsg fail retcode = 0x%x\n", retcode, 0U, 0U, 0U, 0U);
                        } else {
                            tmp_ri = msg_ri;
                            while(tmp_ri != msg_wi) {
                                count++;
                                tmp_ri++;
                            }
                        }
                    }
                } else {
                    retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
                    console_printU5("[ERROR] schdr_wait_msg() : ambacv dev open fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                }
            }

            if(retcode == ERRCODE_NONE) {
                *msg_num = count;
            } else {
                *msg_num = 0U;
            }
        }
    }

    return retcode;

}

uint32_t schdr_process_msg(void **vppHandle, uint32_t *type, uint32_t *message_retcode)
{
    const armvis_msg_t *msg;
    uint32_t retcode = ERRCODE_NONE;
    uint32_t  slot_id;

    if(schdr_sys_cfg.internal_thread == 1U) {
        console_printU5("[ERROR] schdr_process_msg() : invalid to call this API schdr_with_thread %d \n", schdr_sys_cfg.internal_thread, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INTERNAL_THREAD_STATE_MISMATCHED;
    } else {
        if(run_rx_thread == 0U) {
            console_printU5("[ERROR] schdr_process_msg() : scheduler not start , run_rx_thread  \n", run_rx_thread, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_INTERNAL_THREAD_STATE_MISMATCHED;
        } else {
            if(msg_config == NULL) {
                console_printU5("[ERROR] schdrmsg_process_msg() : msg_config == NULL \n", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_VISMSG_QUEUE_ORC2ARM_INVALID;
            } else if (msg_queue == NULL) {
                console_printU5("[ERROR] schdrmsg_process_msg() : msg_queue == NULL\n", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_VISMSG_QUEUE_ORC2ARM_INVALID;
            } else {
                if(msg_ri == msg_wi) {
                    console_printU5("[ERROR] schdr_process_msg() : no message to proccess ri %d wi %d  \n", msg_ri, msg_wi, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_RECEIVE_VISMSG_UNABLE_TO_FIND;
                } else {
                    /* send message to schdr for processing */
                    msg = &(msg_queue[msg_ri & MSG_INDEX_MASK]);
                    if (is_associated_flexidag(msg) != 0U) {
                        slot_id = ((uint32_t)msg->hdr.message_id - (uint32_t)0x0100U);
                        *vppHandle = flexidag_sys_state[slot_id].pHandle;
                    } else {
                        *vppHandle = NULL;
                    }
                    *type = (uint32_t) msg->hdr.message_type;
                    *message_retcode = schdr_process_request(msg, msg_recv_w_seqno);
                    msg_ri++;
                    msg_recv_w_seqno++;
                }
            }
        }
    }

    return retcode;
}

#endif

static void* schdrmsg_rx_entry(void* arg)
{
    const armvis_message_queue_t *pQueueConfig;
    const armvis_msg_t *queue;
    const armvis_msg_t *msg;
    const void *ptr;
    uint32_t wi, ri = msg_recv_ri;
    uint32_t retcode = ERRCODE_NONE;

    (void) arg;
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "schdrmsg_rx_entry : self-created\n", 0U, 0U, 0U, 0U, 0U);
    if(arg == NULL) {
        console_printU5("[ERROR] schdrmsg_rx_entry() : arg == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        typecast(&pQueueConfig,&arg);

        ptr = ambacv_c2v(pQueueConfig->msg_array_daddr);
        if(ptr == NULL) {
            console_printU5("[ERROR] schdrmsg_rx_entry() : pQueueConfig->address == NULL\n", 0U, 0U, 0U, 0U, 0U);
        } else {
            typecast(&queue,&ptr);
            msg_recv_w_seqno = ri; //queue[ri & MSG_INDEX_MASK].hdr.message_seqno;
            while(run_rx_thread == 1U) {
                /* blocking until new orc message is available */
                retcode = schdrmsg_recv_orcmsg(ri,&wi);
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] schdrmsg_rx_entry() : schdrmsg_recv_orcmsg fail retcode = 0x%x\n", retcode, 0U, 0U, 0U, 0U);
                } else {
                    /* send message to schdr for processing */
                    while ((ri != wi) && (run_rx_thread == 1U)) {
                        msg = &(queue[ri & MSG_INDEX_MASK]);
                        retcode = schdr_process_request(msg, msg_recv_w_seqno);
                        if(retcode != ERRCODE_NONE) {
                            console_printU5("[ERROR] schdrmsg_rx_entry() : schdr_process_request fail retcode = 0x%x\n", retcode, 0U, 0U, 0U, 0U);
                        }
                        ri++;
                        msg_recv_w_seqno++;
                    }
                }
            }
        }
    }
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "schdrmsg_rx_entry : self-terminated \n", 0U, 0U, 0U, 0U, 0U);
    return NULL;
}

uint32_t schdrmsg_start(const visorc_init_params_t *sysconfig, uint32_t is_booting_proc)
{
    uint32_t retcode = ERRCODE_NONE;
    static char rx_stack[RX_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char name[32] = "schdrmsg-rx";
    const void *ptr;
    uint32_t i;

    (void) sysconfig;
    if(sysconfig == NULL) {
        console_printU5("[ERROR] schdrmsg_start() : sysconfig == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISORC_INIT_INVALID;
    } else {
        for (i = 0U; i < 4U; i++) {
            if (sysconfig->MsgQueue_orc_to_arm_daddr[i] != 0U) {
                break;
            }
        }

        if(i < 4U) {
            if(is_booting_proc == 1U) {
                msg_recv_ri = 0U;
            } else {
                retcode = schdrmsg_get_orcarm_wi(&msg_recv_ri);
            }

            if(retcode == 0U) {
                run_rx_thread = 1U;
                ptr = ambacv_c2v(sysconfig->MsgQueue_orc_to_arm_daddr[i]);
                if(ptr == NULL) {
                    console_printU5("[ERROR] schdrmsg_start() : ptr = ambacv_c2v(sysconfig->MsgQueue_orc_to_arm_daddr[i]); == NULL \n", 0U, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_VISMSG_QUEUE_ORC2ARM_INVALID;
                } else {
                    if(schdr_sys_cfg.internal_thread == 1U) {
                        retcode = thread_create(&rx_thread, schdrmsg_rx_entry, ptr,
                                                RX_THREAD_PRIO, schdr_sys_cfg.cpus_map, RX_STACK_SIZE, rx_stack, name);
                        if(retcode != ERRCODE_NONE) {
                            console_printU5("[ERROR] schdrmsg_start() : thread_create fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
                        }
                    }
                }
            }
        } else {
            retcode = ERR_DRV_SCHDR_RECEIVE_VISMSG_QUEUE_UNABLE_TO_FIND;
        }
    }
    return retcode;
}

uint32_t schdrmsg_stop(void)
{
    run_rx_thread = 0U;
    msg_param_init = 0U;
    return ERRCODE_NONE;
}

uint32_t schdrmsg_waitfor_shutdown(void)
{
    uint32_t retcode = ERRCODE_NONE;

    if(schdr_sys_cfg.internal_thread == 1U) {
        retcode = thread_delete(&rx_thread);
    } else {
        uint32_t CheckCnt = 200U;

        while(run_rx_thread == 1U) {
            CheckCnt--;
            if (CheckCnt == 0U) {
                console_printU5("[ERROR] schdrmsg_waitfor_shutdown() : timeout\n", 0, 0, 0, 0, 0);
                break;
            }
            if( thread_sleep(10U) != ERRCODE_NONE) {
                console_printU5("[ERROR] schdrmsg_waitfor_shutdown() : thread_sleep fail\n", 0, 0, 0, 0, 0);
            }
        }
    }

    return retcode;
}

