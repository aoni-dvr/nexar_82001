#ifndef __AMBACV_IOCTL_H__
#define __AMBACV_IOCTL_H__

#include <linux/ioctl.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include "ambacv_ioctl.h"

#define IDSPORC_SYNC_COUNT      12
#define FNV1A_32_INIT_HASH      0x811C9DC5U

typedef struct
{
  char     *pBuffer;
  char     *pBuffer_end;
  uint64_t  buffer_daddr;
  uint64_t  buffer_daddr_end;
  uint64_t  buffer_size;        /* Explicitly allow 4GB-1 max DRAM size */
  uint32_t  buffer_cacheable;
} kernel_memblk_t;

void * ambacv_p2v(uint64_t phy);
uint64_t ambacv_v2p(void *virt);
void * ambacv_c2v(uint64_t ca);
uint64_t ambacv_v2c(void *virt);
uint64_t ambacv_p2c(uint64_t pa);
uint64_t ambacv_c2p(uint64_t ca);

void  ambacv_mmap_add_master(void *virt, uint64_t phy, uint64_t size, uint32_t is_cacheable);
void  ambacv_mmap_add_rtos(void *virt, uint64_t phy, uint64_t size, uint32_t is_cacheable);
void  ambacv_mmap_add_dsp_data(void *virt, uint64_t phy, uint64_t size, uint32_t is_cacheable);

extern ambacv_mem_t  ambacv_global_mem;
extern ambacv_mem_region_t ambacv_debug_port;
extern ambacv_log_t  ambacv_global_log;
extern int    enable_orc_cache, scheduler_id;
extern uint32_t ambacv_state;

void schdrmsg_set_event_time(void);
uint32_t schdrmsg_get_event_time(void);
int  schdrmsg_get_sysconfig(unsigned long arg);
int  schdrmsg_start_visorc (unsigned long arg);
int  schdrmsg_resume_visorc (unsigned long arg);
int  schdrmsg_stop_visorc(void);
uint32_t schdrmsg_calc_hash(void* buf, uint32_t len, uint32_t hval);
int  schdrmsg_recv_orcmsg(unsigned long);
uint32_t schdrmsg_get_orcarm_wi(void);
int  schdrmsg_send_armmsg(unsigned long, int);
uint32_t schdrmsg_find_entry(unsigned long);
void schdrmsg_log_msg(armvis_msg_t *msg);
void schdrmsg_init(void);
void schdrmsg_shutdown(void);

void ambacv_cache_clean(void *ptr, uint64_t size);
void ambacv_cache_invalidate(void *ptr, uint64_t size);
#define ambacv_cache_barrier()

void visorc_init(void);
void visorc_shutdown(void);
int  visorc_finalize_loading(unsigned long arg);
int  visorc_start(unsigned int flag, visorc_init_params_t *sysconfig);
int  visorc_stop(void);
void visorc_kick(void);
void visorc_armidsp_queue_kick(void);
void visorc_armvin_queue_kick(void);
void visorc_softreset(int unit_id);
uint32_t  visorc_get_curtime(void);

void idsporc_init(void);
void idsporc_shutdown(void);
int  idsporc_config(unsigned long);
int  idsporc_send_vpmsg(unsigned long);
int  idsporc_send_asyncmsg(unsigned long);

void vinorc_init(void);
void vinorc_shutdown(void);
int  vinorc_config(unsigned long);
int  vinorc_send_vinmsg(unsigned long);

void cache_invld_schdrmsg(armvis_msg_t *msg, int phase);
void cache_clean_schdrmsg(armvis_msg_t *msg);
int  cache_ioctl_clean(unsigned long);
int  cache_ioctl_invld(unsigned long);
int  cache_handle_cma_v2p(unsigned long);
int  cache_handle_cma_p2v(unsigned long);

irqreturn_t ambacv_isr(int irq, void *devid);
void orc_send_sync_count(int id, int val);
visorc_init_params_t *ambacv_get_sysinit_addr(void);

#endif //__AMBACV_IOCTL_H__
