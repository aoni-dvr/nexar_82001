#include "os_kernel.h"
#include "cache_kernel.h"
#include "visorc_kernel.h"
#include "vinorc_kernel.h"

static kmutex_t vinmsg;
static vin_msg_q_info_t *vin_qcb;
static msg_vin_sensor_raw_capture_status_t *vin_msgqueue;

uint32_t krn_vinorc_init(void)
{
    uint32_t retcode = ERRCODE_NONE;

    retcode = krn_mutex_init(&vinmsg);
    return retcode;
}

uint32_t krn_vinorc_config(const ambacv_cfg_addr_t *cfg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr = 0UL;
    const void *ptr = NULL;

    (void) cfg;
    ptr = krn_c2v(cfg->addr[0]);
    if(ptr != NULL) {
        krn_typecast(&vin_qcb, &ptr);
        retcode = krn_cache_invalidate(vin_qcb, sizeof(*vin_qcb));
    } else {
        retcode = ERR_DRV_SCHDR_VISMSG_IDSP_INVALID;
        krn_printU5("[ERROR] krn_vinorc_config() : vin_qcb == NULL", 0U, 0U, 0U, 0U, 0U);
    }

    if(retcode == ERRCODE_NONE) {
        addr = (uint64_t)vin_qcb->base_daddr;
        ptr = krn_c2v(addr);
        if(ptr != NULL) {
            krn_typecast(&vin_msgqueue, &ptr);
        } else {
            retcode = ERR_DRV_SCHDR_VISMSG_IDSP_INVALID;
            krn_printU5("[ERROR] krn_vinorc_config() : vin_msgqueue == NULL", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return retcode;
}

uint32_t krn_vinorc_send_vinmsg(const ambacv_vinvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    msg_vin_sensor_raw_capture_status_t *dst;

    (void) msg;
    retcode = krn_mutex_lock(&vinmsg);
    if(retcode == ERRCODE_NONE) {
        dst = &vin_msgqueue[vin_qcb->write_index];
        retcode = krn_memcpy(dst, msg, sizeof(*dst));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_cache_clean(dst, sizeof(*dst));
            if(retcode == ERRCODE_NONE) {
                if ((++vin_qcb->write_index) == vin_qcb->max_num_msg) {
                    vin_qcb->write_index = 0U;
                }
#ifdef CHIP_CV6
                //update write index of vp-queue
                retcode = krn_cache_clean(&vin_qcb->base_daddr_cpy, ARM_CACHELINE_SIZE);
#else
                //update write index of vp-queue
                retcode = krn_cache_clean(&vin_qcb->base_daddr, ARM_CACHELINE_SIZE);
#endif
            }
        }
        if( krn_mutex_unlock(&vinmsg) != ERRCODE_NONE) {
            krn_printU5("[ERROR] krn_vinorc_send_vinmsg() : mutex_unlock fail ", 0U, 0U, 0U, 0U, 0U);
        }

        // kick VISORC
        krn_visorc_armvin_queue_kick();
    }

    return retcode;

}



