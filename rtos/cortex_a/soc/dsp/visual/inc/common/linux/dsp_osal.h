/**
 *  @file dsp_osal.h
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
 *  @details Definitions & Constants for DSP OS abstract layer
 *
 */

#ifndef DSP_OSAL_H
#define DSP_OSAL_H

#include "osal.h"
#include "AmbaTypes.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_Priv.h"
#include "AmbaDSP_uCode.h"

//#define SUPPORT_OSAL_MQ

#define USE_ISR_EVENT_ATTACH

#define DSP_BUF_POOL_BINARY     (0U)
#define DSP_BUF_POOL_PROTOCOL   (1U)
#define DSP_BUF_POOL_WORK       (2U)
#define DSP_BUF_POOL_DATA       (3U)
#define DSP_BUF_POOL_DBG_LOG    (4U)
#define DSP_BUF_POOL_STATISTIC  (5U)
#define DSP_BUF_POOL_NUM        (6U)

/******************************************************************************/
/*                                  print                                     */
/******************************************************************************/
UINT32 dsp_osal_get_u64msb(UINT64 U64Val);
UINT32 dsp_osal_get_u64lsb(UINT64 U64Val);

void dsp_osal_print_flush(void);
void dsp_osal_print_stop_and_flush(void);
void dsp_osal_printS5(const char *fmt, const char *argv0,
                      const char *argv1, const char *argv2,
                      const char *argv3, const char *argv4);

void dsp_osal_printU5(const char *fmt, uint32_t argv0,
                      uint32_t argv1, uint32_t argv2,
                      uint32_t argv3, uint32_t argv4);

void dsp_osal_module_printS5(uint32_t module_id, const char *fmt,
                             const char *argv0, const char *argv1,
                             const char *argv2, const char *argv3,
                             const char *argv4);

void dsp_osal_module_printU5(uint32_t module_id, const char *fmt,
                             uint32_t argv0, uint32_t argv1,
                             uint32_t argv2, uint32_t argv3,
                             uint32_t argv4);

void dsp_osal_module_printI5(uint32_t module_id, const char *fmt,
                             int32_t argv0, int32_t argv1,
                             int32_t argv2, int32_t argv3,
                             int32_t argv4);

void dsp_osal_print_err_line(uint32_t rval, const char *fn, uint32_t line_num);

/******************************************************************************/
/*                                  OS relative                               */
/******************************************************************************/
typedef irqreturn_t (*osal_isr_entry_f)(int32_t IntID, void *UserArg);

typedef struct semaphore        osal_sem_t;
typedef struct mutex            osal_mutex_t;
typedef struct task_struct      osal_thread_t;
typedef struct {
    atomic_t                    flag;
    struct wait_queue_head      wait_queue;
} osal_even_t;
typedef struct {
    ULONG                       spin_lock_flag;
    spinlock_t                  dsp_spin_lock;
} osal_spinlock_t;
typedef struct {
#define DSP_MQ_MTX_TIMEOUT          (5000U)
    osal_mutex_t                mtx;
#define DSP_MQ_FLG_MSG_ARRIVED      (0x1U)
#define DSP_MQ_FLG_MSG_CONSUMED     (0x2U)
    osal_even_t                 event;
#define DSP_MQ_MAX_NUM              (0xFFFFU)
    uint16_t                    wp;
    uint16_t                    rp;
    uint32_t                    msg_num;
    uint32_t                    msg_unit_size;
    uint32_t                    is_empty;
    void                        *mq_buf;
} osal_mq_t;
typedef struct {
    uint32_t                    PendingNum;
    uint32_t                    FreeNum;
} osal_mq_info_t;

#ifdef AMBA_INT_RISING_EDGE_TRIGGER
#define IRQ_TRIG_RISE_EDGE  (AMBA_INT_RISING_EDGE_TRIGGER)
#else
#define IRQ_TRIG_RISE_EDGE  (1)
#endif
#ifdef AMBA_INT_IRQ
#define IRQ_TYPE_IRQ  (AMBA_INT_IRQ)
#else
#define IRQ_TYPE_IRQ  (0)
#endif
typedef struct {
    uint32_t    trigger_type;
    uint32_t    irq_type;
    uint32_t    cpu_targets;
} osal_irq_t;

uint32_t dsp_osal_sem_init(osal_sem_t *sem, char *pName, int32_t pshared, uint32_t value);
uint32_t dsp_osal_sem_deinit(osal_sem_t *sem);
uint32_t dsp_osal_sem_post(osal_sem_t *sem);
uint32_t dsp_osal_sem_wait(osal_sem_t *sem, uint32_t timeout);
uint32_t dsp_osal_mutex_init(osal_mutex_t *mutex, char *pName);
uint32_t dsp_osal_mutex_deinit(osal_mutex_t *mutex);
uint32_t dsp_osal_mutex_lock(osal_mutex_t *mutex, uint32_t timeout);
uint32_t dsp_osal_mutex_unlock(osal_mutex_t *mutex);
uint32_t dsp_osal_eventflag_init(osal_even_t *event, char *pName);
uint32_t dsp_osal_eventflag_set(osal_even_t *event, uint32_t flag);
uint32_t dsp_osal_eventflag_get(osal_even_t *event, uint32_t reqflag, uint32_t all, uint32_t clear, uint32_t *actflag, uint32_t timeout);
uint32_t dsp_osal_eventflag_clear(osal_even_t *event, uint32_t flag);
uint32_t dsp_osal_spinlock(osal_spinlock_t *pSpinlock);
uint32_t dsp_osal_spinunlock(osal_spinlock_t *pSpinlock);
#define DSP_CMD_LOCK_TIMEOUT    (1000U)
uint32_t dsp_osal_cmdlock(osal_mutex_t *mutex, osal_spinlock_t *pSpinlock);
uint32_t dsp_osal_cmdunlock(osal_mutex_t *mutex, const osal_spinlock_t *pSpinlock);

uint32_t dsp_osal_mq_create(osal_mq_t *mq, char *name, uint32_t msg_size, void *msg_q_buf, uint32_t msg_q_buf_size);
uint32_t dsp_osal_mq_send(osal_mq_t *mq, void *msg, uint32_t time_out);
uint32_t dsp_osal_mq_recv(osal_mq_t *mq, void *msg, uint32_t time_out);
uint32_t dsp_osal_mq_query(osal_mq_t *mq, osal_mq_info_t *mq_info);

uint32_t dsp_osal_sleep(uint32_t msec);
uint32_t dsp_osal_get_sys_tick(uint32_t *msec);

/******************************************************************************/
/*                                  Memory                                    */
/******************************************************************************/
uint32_t dsp_osal_cache_addr_align(uint32_t addr);
uint32_t dsp_osal_cache_size_align(uint32_t size);
uint32_t dsp_osal_cache_clean(const void *ptr, uint32_t size);
uint32_t dsp_osal_cache_invalidate(const void *ptr, uint32_t size);
uint32_t dsp_osal_virt2phys(const ULONG Virt, ULONG *pPhys);
uint32_t dsp_osal_phys2virt(const ULONG Phys, ULONG *pVirt);
uint32_t dsp_osal_virt2cli(const ULONG Virt, uint32_t *pCli);
uint32_t dsp_osal_cli2virt(const uint32_t Cli, ULONG *pVirt);


/******************************************************************************/
/*                                  Register Read/Write                       */
/******************************************************************************/
uint32_t dsp_osal_readl_relaxed(const void *reg);
void dsp_osal_writel_relaxed(uint32_t val, const void *reg);

uint32_t dsp_osal_copy_to_user(void *to, const void *from, uint32_t n);
uint32_t dsp_osal_copy_from_user(void *to, const void *from, uint32_t n);

void *dsp_osal_ioremap(uint32_t pa, uint32_t size);
void dsp_osal_iounmap(void *va, uint32_t size);
uint32_t dsp_query_buf_info(uint32_t phys_Addr, uint32_t buf_size, uint32_t *is_cached);

uint32_t dsp_osal_irq_config(uint32_t irq_id, const osal_irq_t *irq_cfg, osal_isr_entry_f hdlr, uint32_t arg);
uint32_t dsp_osal_irq_enable(uint32_t irq_id);
uint32_t dsp_osal_irq_disable(uint32_t irq_id);
#define DSP_IRQ_TYPE_CODE       (0U)
#define DSP_IRQ_TYPE_VIN_SOF    (1U)
#define DSP_IRQ_TYPE_VIN_EOF    (2U)
#define DSP_IRQ_TYPE_VOUT       (3U)
#define NUM_DSP_IRQ_TYPE        (4U)
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define DSP_MAX_IRQ_CODE_NUM    (5U)
#else
#define DSP_MAX_IRQ_CODE_NUM    (4U)
#endif
#define DSP_MAX_IRQ_VIN_NUM     (AMBA_DSP_MAX_VIN_NUM)
#define DSP_MAX_IRQ_VOUT_NUM    (AMBA_DSP_MAX_VOUT_NUM)
uint32_t dsp_osal_irq_id_query(uint32_t type, uint32_t idx, uint32_t *irq_id);

uint32_t dsp_osal_cma_alloc(void *arg, uint64_t owner);
uint32_t dsp_osal_cma_free(void *arg);
uint32_t dsp_osal_cma_sync_cache(void *arg);
uint32_t dsp_osal_cma_get_usage(void *arg);

uint32_t dsp_osal_cma_p2v(uint32_t phy, uint32_t *pCacheFlag, ULONG *pAddr);
uint32_t dsp_osal_cma_v2p(ULONG virt, uint32_t *pCacheFlag);

/******************************************************************************/
/*                                  DSP                                       */
/******************************************************************************/
typedef struct {
    ULONG CodeAddr;
    ULONG MeAddr;
    ULONG MdxfAddr;
    ULONG DefBinAddr;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    ULONG Code1Addr;
    ULONG Code2Addr;
    ULONG Code3Addr;
    ULONG Me1Addr;
    ULONG Mdxf1Addr;
#endif
} osal_ucode_addr_t;

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
void dsp_osal_safety_init(void);
#endif
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
void dsp_osal_orc_set_pc(const osal_ucode_addr_t *pBasePhysAddr, const uint8_t IdspBinShareMode);
#else
void dsp_osal_orc_set_pc(const osal_ucode_addr_t *pBasePhysAddr);
#endif
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define DSP_ORC_CODE_IDX    (0U)
#define DSP_ORC_VIN_IDX     (1U)
#define DSP_ORC_IDSP0_IDX   (2U)
#define DSP_ORC_IDSP1_IDX   (3U)
#define DSP_ORC_ME0_IDX     (4U)
#define DSP_ORC_ME1_IDX     (5U)
#define DSP_ORC_MDXF0_IDX   (6U)
#define DSP_ORC_MDXF1_IDX   (7U)
#else
#define DSP_ORC_CODE_IDX    (0U)
#define DSP_ORC_ME0_IDX     (1U)
#define DSP_ORC_MDXF0_IDX   (2U)
#endif
void dsp_osal_orc_enable(UINT32 OrcMask);
void dsp_osal_orc_shutdown(void);
void dsp_osal_dbgport_set_sync_cnt(uint32_t sync_cnt_id, uint32_t val);
void dsp_osal_dbgport_set_smem_grp(void);
uint32_t dsp_osal_get_orc_timer(void);
uint32_t dsp_osal_get_dram_page(void);
void dsp_osal_get_cmd_req_timeout_cfg(uint32_t *pEnable, uint32_t *pTime);

uint32_t dsp_osal_get_ucode_base_addr(osal_ucode_addr_t *pBase);
uint32_t dsp_osal_get_ucode_end_addr(osal_ucode_addr_t *pEnd);
UINT32 dsp_osal_alloc_prot_buf(ULONG *pBase, UINT32 ReqSize, UINT32 Align);
UINT32 dsp_osal_get_stat_buf(ULONG *pBase, UINT32 *pSize);
UINT32 dsp_osal_alloc_stat_buf(ULONG *pBase, UINT32 ReqSize, UINT32 Align);
UINT32 dsp_osal_alloc_add_buf_addr(UINT32 Type, ULONG phys_addr, UINT32 size, UINT32 IsCached);
UINT32 dsp_osal_get_data_buf(ULONG *pBase, UINT32 *pSize, UINT32 *pIsCached);
UINT32 dsp_osal_get_bin_buf(ULONG *pBase, UINT32 *pSize, UINT32 *pIsCached);

/******************************************************************************/
/*                                  DebugPort                                 */
/******************************************************************************/
void dsp_osal_delay_cycles(UINT32 Delay);

void dsp_osal_dbgport_get_vin_enable_status(UINT32 VinId, UINT32 *pEnable);

#ifndef AMBA_SYS_FEATURE_DSP
#define AMBA_SYS_FEATURE_DSP    (0x01UL)
#endif
#ifndef AMBA_SYS_FEATURE_MCTF
#define AMBA_SYS_FEATURE_MCTF   (0x02UL)
#endif
#ifndef AMBA_SYS_FEATURE_CODEC
#define AMBA_SYS_FEATURE_CODEC  (0x03UL)
#endif
#ifndef AMBA_SYS_FEATURE_HEVC
#define AMBA_SYS_FEATURE_HEVC   (0x04UL)
#endif
#define AMBA_DSP_CHK_DSP    (1UL<<AMBA_SYS_FEATURE_DSP)
#define AMBA_DSP_CHK_MCTF   (1UL<<AMBA_SYS_FEATURE_MCTF)
#define AMBA_DSP_CHK_CODEC  (1UL<<AMBA_SYS_FEATURE_CODEC)
#define AMBA_DSP_CHK_HEVC   (1UL<<AMBA_SYS_FEATURE_HEVC)
UINT32 dsp_osal_dbgport_get_clk_enable_status(UINT32 ClkId, UINT32 *pEnable);

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
UINT32 dsp_osal_dbgport_get_reset_status_info(const UINT32 Idx, UINT32 *pAddr, UINT32 *pDefValue);
#endif

/******************************************************************************/
/*                                  Event                                     */
/******************************************************************************/
UINT32 dsp_osal_evnt_data_q_init(void);
UINT32 dsp_osal_evnt_hdlr_cfg(UINT32 EvntId, UINT32 MaxNum, AMBA_DSP_EVENT_HANDLER_f *pEvntHdlrs);
UINT32 dsp_osal_evnt_hdlr_reg(UINT32 EvntId, UINT32 Reg, AMBA_DSP_EVENT_HANDLER_f EvntHdlr);
UINT32 dsp_osal_evnt_hdlr_reset(UINT32 EvntId);
UINT32 dsp_osal_exec_evnt_user_cb(UINT32 HdlrNum, const AMBA_DSP_EVENT_HANDLER_f *pHdlr, UINT32 EvntId, const void *pEvntInfo, const UINT16 EventBufIdx);
UINT32 dsp_osal_fetch_evnt_data(const void *pEvntData);

/******************************************************************************/
/*                                  AAA transfer                              */
/******************************************************************************/
#define DSP_3A_VIDEO            (0U)
#define DSP_3A_STILL            (1U)
#define DSP_3A_DEC              (2U)
#define DSP_HIST_VIDEO          (3U)
#define DSP_HIST_STILL          (4U)
#define DSP_HIST_DEC            (5U)
#define DSP_HIST_VIDEO_RGBIR    (6U)
#define DSP_3A_VIDEO_PG         (7U)

typedef struct {
    UINT8  Type;
    UINT8  SrcSliceX;
    UINT8  SrcSliceY;
    ULONG  SrcAddr[DSP_MAX_3A_SLICE_NUM];
    ULONG  DstAddr;
    UINT16 ChannelIdx;
} AMBA_DSP_EVENT_3A_TRANSFER_s;

UINT32 dsp_osal_aaa_transfer(const void *pEvntData);
extern UINT32 TransferCFAAaaStatData(const void *pData);
extern UINT32 TransferPGAaaStatData(const void *pData);
extern UINT32 TransCFAHistStatData(const void *pData);

/******************************************************************************/
/*                                  Utilities                                 */
/******************************************************************************/
static inline void dsp_osal_typecast(void * pNewType, const void * pOldType)
{
    if (pNewType == NULL) {
        printk("[ERR] dsp_osal_typecast : %s %d : pNewType == NULL \n", __FUNCTION__, __LINE__);
    } else if (pOldType == NULL) {
        printk("[ERR] dsp_osal_typecast : %s %d : pOldType == NULL \n", __FUNCTION__, __LINE__);
    } else {
        memcpy(pNewType, pOldType, sizeof(void *));
    }
}

static inline uint32_t dsp_osal_memcpy(void *pDst, const void *pSrc, size_t num)
{
#ifdef USE_AMBA_UTILITY
    return OSAL_memcpy(pDst, pSrc, num);
#else
    (void)memcpy(pDst, pSrc, num);
    return 0U;
#endif
}

static inline uint32_t dsp_osal_memset(void *ptr, int32_t v, size_t n)
{
#ifdef USE_AMBA_UTILITY
    return OSAL_memset(ptr, v, n);
#else
    (void)memset(ptr, v, n);
    return 0U;
#endif
}

static inline uint32_t dsp_osal_memcmp(const void *ptr1, const void *ptr2, size_t n, void *pVal)
{
#ifdef USE_AMBA_UTILITY
    return OSAL_memcmp(ptr1, ptr2, n, pVal);
#else
    int ret;
    ret = memcmp(ptr1, ptr2, n);
    (void)memcpy(pVal, &ret, sizeof(int));
    return 0U;
#endif
}

uint32_t dsp_osal_str_cmp(const char *pString0, const char *pString1);
uint32_t dsp_osal_str_to_u32(const char *pString, uint32_t *pVal);
uint32_t dsp_osal_u32_to_str(char *pString, uint32_t str_len, uint32_t val, uint32_t radix);
uint32_t dsp_osal_str_print_u32(char *pBuffer, uint32_t BufferSize, const char *pFmtString, uint32_t Count, const uint32_t *pArgs);
uint32_t dsp_osal_str_append(char *pBuffer, uint32_t BufferSize, const char *pSource);
uint32_t dsp_osal_str_append_u32(char *pBuffer, uint32_t BufferSize, uint32_t Value, uint32_t Radix);
uint32_t dsp_osal_str_copy(char *pBuffer, uint32_t BufferSize, const char *pSource);

/******************************************************************************/
/*                                  Resource                                 */
/******************************************************************************/
uint32_t dsp_osal_kernel_init(struct device_node *np);
void dsp_osal_kernel_exit(void);

#endif  //DSP_OSAL_H

