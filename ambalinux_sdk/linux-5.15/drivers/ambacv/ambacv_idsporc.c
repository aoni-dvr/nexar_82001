#include <linux/mutex.h>
#include <linux/uaccess.h>
#include "ambacv_kal.h"
#include "idsp_vis_msg.h"

static struct mutex        idsp;
static dsp_msg_q_info_t   *vp_qcb;
static idsp_vis_picinfo_t *vp_queue;
static dsp_cmd_t          *as_queue;
extern uint32_t debug_level;

void idsporc_init(void)
{
	mutex_init(&idsp);
}

void idsporc_shutdown(void)
{
}

int idsporc_config(unsigned long arg)
{
	ambacv_cfg_addr_t cfg;
	int ret;

	ret = copy_from_user(&cfg, (void*)arg, sizeof(ambacv_cfg_addr_t));
	if (ret != 0) {
		pr_err("Error(%d) in schdrmsg_config_idsp\n", ret);
		return ret;
	}

	vp_qcb = (dsp_msg_q_info_t*)ambacv_c2v(cfg.addr[0]);
	ambacv_cache_invalidate(vp_qcb, sizeof(*vp_qcb));
	ambacv_cache_barrier();
	vp_queue = (idsp_vis_picinfo_t*)ambacv_c2v(vp_qcb->base_daddr);
	if (debug_level > 0)
	{
		pr_info("IDSP vp-queue: control-block=%llX base=%llX, size=%d\n", ambacv_c2p(cfg.addr[0]),
		       ambacv_c2p(vp_qcb->base_daddr), vp_qcb->max_num_msg);
	} /* if (debug_level > 0) */

	as_queue = (dsp_cmd_t*)ambacv_c2v(cfg.addr[1]);

	if (debug_level > 0)
	{
		pr_info("IDSP async-queue base=%llX\n", ambacv_c2p(cfg.addr[1]));
	} /* if (debug_level > 0) */

	return 0;
}

int idsporc_send_vpmsg(unsigned long arg)
{
	idsp_vis_picinfo_t *dst;
	int ret;
	ambacv_idspvis_msg_t msg;

	ret = copy_from_user(&msg, (void*)arg, sizeof(ambacv_idspvis_msg_t));
	if (ret != 0) {
		pr_err("Error(%d) in idsporc_send_vpmsg\n", ret);
		return ret;
	}
	mutex_lock(&idsp);
	dst = &vp_queue[vp_qcb->write_index];
	memcpy(dst, (void*)&msg.msg, sizeof(*dst));
	ambacv_cache_clean(dst, sizeof(*dst));

	if (++vp_qcb->write_index == vp_qcb->max_num_msg) {
		vp_qcb->write_index = 0;
	}

	//update write index of vp-queue
#ifdef CHIP_CV6
	ambacv_cache_clean(&vp_qcb->base_daddr_cpy, ARM_CACHELINE_SIZE);
#else /* !CHIP_CV6 */
	ambacv_cache_clean(&vp_qcb->base_daddr, ARM_CACHELINE_SIZE);
#endif /* ?CHIP_CV6 */

	mutex_unlock(&idsp);

	// kick VISORC
	visorc_armidsp_queue_kick();
	return 0;
}

int idsporc_send_asyncmsg(unsigned long arg)
{
	static uint32_t seq_no;
	dsp_header_cmd_t header;
	int ret, size;
	ambacv_asynvis_msg_t asyn_msg;

	// sanity-check the first cmd
	ret = copy_from_user(&asyn_msg, (void*)arg, sizeof(ambacv_asynvis_msg_t));
	if (ret != 0) {
		pr_err("Error(%d) in idsproc_send_asyncmsg0\n", ret);
		return ret;
	}
	memcpy(&header, (void*)&asyn_msg.cmd[0], sizeof(header));
	if (header.cmd_code != CMD_DSP_HEADER) {
		pr_err("Invalid asyncmsg header type %X\n", header.cmd_code);
		return -EINVAL;
	}
	size = (header.num_cmds + 1) * sizeof(dsp_cmd_t);
	if (size > 4096) {
		pr_err("Too much cmds in asycmsg %X\n", size);
		return -EINVAL;
	}

	mutex_lock(&idsp);
	memcpy(as_queue, (void*)&asyn_msg.cmd[0], size);
	seq_no++;
	pr_info("send async msg, size=%d, seq=%d\n", size, seq_no);
	((dsp_header_cmd_t*)as_queue)->cmd_seq_num = seq_no;
	ambacv_cache_clean(as_queue, size);

	// kick IDSPORC
	orc_send_sync_count(IDSPORC_SYNC_COUNT, 0x1001);
	mutex_unlock(&idsp);

	return 0;
}
