#include "os_kernel.h"
#include "cache_kernel.h"
#include "visorc_kernel.h"
#include "idsporc_kernel.h"

static kmutex_t idspsmg;
static dsp_msg_q_info_t   *vp_qcb;
static idsp_vis_picinfo_t *vp_queue;
static dsp_cmd_t          *as_queue;

uint32_t krn_idsporc_init(void)
{
    uint32_t retcode = ERRCODE_NONE;

    retcode = krn_mutex_init(&idspsmg);
    return retcode;
}

uint32_t krn_idsporc_config(const ambacv_cfg_addr_t *cfg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr = 0UL;
    const void *ptr = NULL;

    (void) cfg;
    ptr = krn_c2v(cfg->addr[0]);
    if(ptr != NULL) {
        krn_typecast(&vp_qcb, &ptr);
        retcode = krn_cache_invalidate(vp_qcb, sizeof(*vp_qcb));
    } else {
        retcode = ERR_DRV_SCHDR_VISMSG_IDSP_INVALID;
        krn_printU5("[ERROR] krn_idsporc_config() : vp_qcb == NULL", 0U, 0U, 0U, 0U, 0U);
    }

    if(retcode == ERRCODE_NONE) {
        addr = (uint64_t)vp_qcb->base_daddr;
        ptr = krn_c2v(addr);
        if(ptr != NULL) {
            uint32_t addr1 = 0U,addr2 = 0U;

            addr1 = (uint32_t)krn_c2p(cfg->addr[0]);
            addr2 =  (uint32_t)krn_c2p(cfg->addr[0]);
            krn_typecast(&vp_queue, &ptr);
            krn_module_printU5("[INFO] krn_idsporc_config() : IDSP vp-queue: control-block=%X base=%X, size=%d", addr1,  addr2,  vp_qcb->max_num_msg, 0U, 0U);
        } else {
            retcode = ERR_DRV_SCHDR_VISMSG_IDSP_INVALID;
            krn_printU5("[ERROR] krn_idsporc_config() : vp_queue == NULL", 0U, 0U, 0U, 0U, 0U);
        }
    }

    if(retcode == ERRCODE_NONE) {
        ptr = krn_c2v(cfg->addr[1]);
        if(ptr != NULL) {
            krn_typecast(&as_queue, &ptr);
            krn_module_printU5("[INFO] krn_idsporc_config() : IDSP async-queue base=%X ", (uint32_t)krn_c2p(cfg->addr[1]),  0U,  0U, 0U, 0U);
        } else {
            retcode = ERR_DRV_SCHDR_VISMSG_IDSP_INVALID;
            krn_printU5("[ERROR] krn_idsporc_config() : as_queue == NULL", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return retcode;
}

uint32_t krn_idsporc_send_vpmsg(const ambacv_idspvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    idsp_vis_picinfo_t *dst;

    (void) msg;
    if((uint32_t)sizeof(*dst) != msg->msg_size) {
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_idsporc_send_vpmsg() : sizeof(*dst) != msg->msg_size ", 0U, 0U, 0U, 0U, 0U);
    } else {
        retcode = krn_mutex_lock(&idspsmg);
        if(retcode == ERRCODE_NONE) {
            dst = &vp_queue[vp_qcb->write_index];
            retcode = krn_memcpy(dst, &msg->msg, sizeof(*dst));
            if(retcode == ERRCODE_NONE) {
                retcode = krn_cache_clean(dst, sizeof(*dst));
                if (((++vp_qcb->write_index) == vp_qcb->max_num_msg)) {
                    vp_qcb->write_index = 0U;
                }

#ifdef CHIP_CV6
                //update write index of vp-queue
                retcode = krn_cache_clean(&vp_qcb->base_daddr_cpy, ARM_CACHELINE_SIZE);
#else
                //update write index of vp-queue
                retcode = krn_cache_clean(&vp_qcb->base_daddr, ARM_CACHELINE_SIZE);
#endif
            }
            if( krn_mutex_unlock(&idspsmg) != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_idsporc_send_vpmsg() : mutex_unlock fail ", 0U, 0U, 0U, 0U, 0U);
            }
            // kick VISORC
            krn_visorc_armidsp_queue_kick();
        }
    }
    return retcode;
}

uint32_t krn_idsporc_send_asyncmsg(const ambacv_asynvis_msg_t *asyn_msg)
{
    uint32_t retcode = ERRCODE_NONE;
    static uint32_t seq_no;
    dsp_header_cmd_t header;
    dsp_header_cmd_t *headr_ptr = NULL;
    uint32_t size;

    (void) asyn_msg;
    // sanity-check the first cmd
    retcode = krn_memcpy(&header, &asyn_msg->cmd[0], sizeof(header));
    if(retcode == ERRCODE_NONE) {
        if (header.cmd_code != (uint32_t)CMD_DSP_HEADER) {
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
            krn_printU5("[ERROR] krn_idsporc_send_asyncmsg() : header.cmd_code(0x%x) != CMD_DSP_HEADER(0x%x) ", header.cmd_code, CMD_DSP_HEADER, 0U, 0U, 0U);
        }

        if(retcode == ERRCODE_NONE) {
            size = (header.num_cmds + 1U) * (uint32_t)sizeof(dsp_cmd_t);
            if (size > 4096U) {
                retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
                krn_printU5("[ERROR] krn_idsporc_send_asyncmsg() : size(0x%x) > 4096 ", size, 0U, 0U, 0U, 0U);
            } else {
                retcode = krn_mutex_lock(&idspsmg);
                if(retcode == ERRCODE_NONE) {
                    retcode = krn_memcpy(as_queue, &asyn_msg->cmd[0], size);
                    if(retcode == ERRCODE_NONE) {
                        seq_no++;
                        krn_module_printU5("[INFO] krn_idsporc_config() : send async msg, size=%d, seq=%d ", size,  seq_no,  0U, 0U, 0U);
                        krn_typecast(&headr_ptr, &as_queue);
                        headr_ptr->cmd_seq_num = seq_no;
                        retcode = krn_cache_clean(as_queue, size);
                        if(retcode == ERRCODE_NONE) {
                            krn_visorc_armidsp_async_queue_kick();
                        }
                    }
                    if( krn_mutex_unlock(&idspsmg) != ERRCODE_NONE) {
                        krn_printU5("[ERROR] krn_idsporc_send_asyncmsg() : mutex_unlock fail ", 0U, 0U, 0U, 0U, 0U);
                    }
                }
            }
        }
    }

    return retcode;
}

