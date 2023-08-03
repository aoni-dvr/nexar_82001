#include <linux/mutex.h>
#include <linux/uaccess.h>
#include "ambacv_kal.h"
#include "cmd_msg_vin.h"

static struct mutex        vinmsg;
static vin_msg_q_info_t   *vin_qcb;
static msg_vin_sensor_raw_capture_status_t *vin_msgqueue;
extern uint32_t debug_level;

void vinorc_init(void)
{
	mutex_init(&vinmsg);
}

void vinorc_shutdown(void)
{
}

int vinorc_config(unsigned long arg)
{
	int ret;
	ambacv_cfg_addr_t cfg;

	ret = copy_from_user(&cfg, (void*)arg, sizeof(ambacv_cfg_addr_t));
	if (ret != 0) {
		pr_err("Error(%d) in vinorc_config\n", ret);
		return ret;
	}

	vin_qcb = (vin_msg_q_info_t*)ambacv_c2v(cfg.addr[0]);
	ambacv_cache_invalidate(vin_qcb, sizeof(*vin_qcb));
	ambacv_cache_barrier();
	vin_msgqueue = (msg_vin_sensor_raw_capture_status_t*)ambacv_c2v(vin_qcb->base_daddr);
	if (debug_level > 0)
	{
		pr_info("VIN in-queue: control-block=%llX base=%llX, size=%d\n", ambacv_c2p(cfg.addr[0]),
		       ambacv_c2p(vin_qcb->base_daddr), vin_qcb->max_num_msg);
	} /* if (debug_level > 0) */

	return 0;
}

int vinorc_send_vinmsg(unsigned long arg)
{
	msg_vin_sensor_raw_capture_status_t *dst;
	int ret;
	ambacv_vinvis_msg_t msg;

	ret = copy_from_user(&msg, (void*)arg, sizeof(ambacv_vinvis_msg_t));
	if (ret != 0) {
		pr_err("Error(%d) in vinorc_send_vinmsg\n", ret);
		return ret;
	}
	mutex_lock(&vinmsg);
	dst = &vin_msgqueue[vin_qcb->write_index];
	memcpy(dst, (void*)&msg.msg_payload[0], sizeof(*dst));
	ambacv_cache_clean(dst, sizeof(*dst));

	if (++vin_qcb->write_index == vin_qcb->max_num_msg) {
		vin_qcb->write_index = 0;
	}

	//update write index of vp-queue
#ifdef CHIP_CV6
	ambacv_cache_clean(&vin_qcb->base_daddr_cpy, ARM_CACHELINE_SIZE);
#else /* !CHIP_CV6 */
	ambacv_cache_clean(&vin_qcb->base_daddr, ARM_CACHELINE_SIZE);
#endif /* ?CHIP_CV6 */

	mutex_unlock(&vinmsg);

	// kick VISORC
	visorc_armidsp_queue_kick();
	return 0;

}



