/*
 * Copyright (c) 2007, 2014, 2015 QNX Software Systems. All rights reserved.
 * Copyright (c) 2020, Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <io-pkt/iopkt_driver.h>
#include <sys/io-pkt.h>
#include <sys/syspage.h>
#include <sys/device.h>
#include <device_qnx.h>
#include <net/if_ether.h>
#include <net/if_media.h>
#include <net/netbyte.h>
#if 0
#include <net80211/ieee80211_var.h>
#endif

#include "AmbaENET.h"
#include "AmbaRTSL_ENET.h"
int sam_entry(void *dll_hdl, struct _iopkt_self *iopkt, char *options);

int sam_init(struct ifnet *);
void sam_stop(struct ifnet *, int);

void sam_start(struct ifnet *);
int sam_ioctl(struct ifnet *, unsigned long, caddr_t);

const struct sigevent * sam_isr(void *, int);
int sam_process_interrupt(void *, struct nw_work_thread *);
int sam_enable_interrupt(void *);

void sam_shutdown(void *);


struct _iopkt_drvr_entry IOPKT_DRVR_ENTRY_SYM(sam) = IOPKT_DRVR_ENTRY_SYM_INIT(sam_entry);

#ifdef VARIANT_a
#include <nw_dl.h>
/* This is what gets specified in the stack's dl.c */
struct nw_dll_syms sam_syms[] = {
    {"iopkt_drvr_entry", &IOPKT_DRVR_ENTRY_SYM(sam)},
    {NULL, NULL}
};
#endif

const uint8_t etherbroadcastaddr[ETHER_ADDR_LEN] =
{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

struct sam_dev {
    struct device        sc_dev;    /* common device */
#if 1
    struct ethercom        sc_ec;    /* common ethernet */
#else
    struct ieee80211com    sc_ic;    /* common 80211 */
#endif
    nic_config_t        cfg;    /* nic information */
    /* whatever else you need follows */
    struct _iopkt_self    *sc_iopkt;
    int            sc_iid;
    int            sc_irq;
    int            sc_intr_cnt;
    int            sc_intr_spurious;
    struct _iopkt_inter    sc_inter;
    void            *sc_sdhook;
};

int sam_attach(struct device *, struct device *, void *);
int sam_detach(struct device *, int);

CFATTACH_DECL(sam,
              sizeof(struct sam_dev),
              NULL,
              sam_attach,
              sam_detach,
              NULL);

#if defined(CONFIG_BUILD_SSP_ENET)
#define AMBA_HW_PTP
//#define RX_SKIP_IOPKT
//#define TX_DUMP
//#define RX_DUMP
//#define DEBUGMBUF
//#define DEBUGIP

static UINT32 tx_pause = 0;
struct cache_ctrl enet_cache;
static UINT32 _Idx = 0;
static ULONG reg_base = ETH0_REG_BASE;
static UINT32 g_reaped = 0U, g_seeded = 0U;
static volatile uint32_t irq_flags;
static struct mbuf *txm[BOARD_ETH_TX_FRAMES];
static struct mbuf *rxm[BOARD_ETH_RX_FRAMES];
static int enet_fd;
static struct callout eth_callout;
static mdi_t *enet_mdi;

#if defined(AMBA_HW_PTP)
static ptp_extts_t rx_ts[BOARD_ETH_RX_FRAMES];
static ptp_extts_t tx_ts[BOARD_ETH_TX_FRAMES];
#endif

typedef struct {
    void                *dll_hdl;
    struct _iopkt_self  *iopkt;
    char                *options;
    nic_config_t        *cfg;
} attach_args_t;

static attach_args_t amba_attach_args;
static int amba_debug = 0;
static int amba_parse_options(attach_args_t *attach_args)
{
    nic_config_t *cfg  = attach_args->cfg;
    const char *optstring = attach_args->options;
    char *options, *freeptr, *value;
    int opt;
    static char *amba_opts [] = {
        "amba_debug",  // 0
        "amba_idx",    // 1
        "tx_pause",    // 2
        NULL
    };

    if (optstring == NULL) {
        return 0;
    }

    /* getsubopt() is destructive */
    freeptr = options = strdup(optstring);

    while (options && *options != '\0') {
        opt = getsubopt(&options, amba_opts, &value);
        switch (opt) {

        case 0:
            amba_debug = strtoul(value, 0, 0);
            fprintf(stderr, "amba_debug %d\n", amba_debug);
            break;

        case 1:
            _Idx = strtoul(value, 0, 0);
            if (_Idx == 1U) {
                reg_base = ETH1_REG_BASE;
            }
            break;

        case 2:
            tx_pause = strtoul(value, 0, 0);
            fprintf(stderr, "tx_pause %d\n", tx_pause);
            break;
        default:
            if (nic_parse_options(cfg, value) != EOK) {
                fprintf(stderr, "Unknown option %s", value);
                free(freeptr, M_DEVBUF);
                return EINVAL;
            }
        }
    }
    free(freeptr, M_DEVBUF);
    return EOK;
}

static void amba_isr(UINT32 IntID, UINT32 UserArg)
{
    (void)IntID;
    (void)UserArg;
    atomic_set(&irq_flags, in32(reg_base + 0x1014U));
    volatile uint32_t MACR15 = in32(reg_base + 0x3cU);

    /*GLI*/
    if (irq_flags & 0x4000000) {
        /*Mask GLI*/
        out32(reg_base + 0x3cU, MACR15|0xf);
    }

    /*TTI*/
    if (irq_flags & 0x20000000) {
        /*Mask TTI*/
        out32(reg_base + 0x3c, MACR15|0x200);
    }

    /* Interrupt Enable Register */
    out32(reg_base + 0x101cU, 0);
}

static void EnetUserTaskCreate(UINT32 Idx)
{
#if defined(QNX_ETH_TSK)
    void EnetTxTaskEntry(UINT32 Idx);
    typedef void *(*pthread_f) (void *);
    pthread_t tid;
    pthread_create(&tid, NULL, (pthread_f)&EnetTxTaskEntry, NULL);
#endif
    (void)Idx;
}

static int amba_mmap_reg(uint64_t phy, size_t len, int prot)
{
    int ret = 0;

    void *virt = mmap_device_memory((void *) phy,
                                    len,
                                    prot,
                                    MAP_FIXED,
                                    phy);

    if (virt == MAP_FAILED) {
        ret = -1;
        fprintf(stderr, "%s ENET %s(%d) error %s\n", ANSI_RED, __func__, __LINE__, ANSI_RESET);
    }

    return ret;
}

#if defined(TX_DUMP) || defined(RX_DUMP)
static inline void amba_dump_buffer(const char *msg, const UINT8 *data, UINT32 length)
{
    UINT32 i;
    //UINT32 len = (length > 0x60) ? 0x60 : length;
    UINT32 len = length;

    if (msg != NULL) {
        fprintf(stderr, "[%s]", msg);
    }
    for (i = 0U; i < len; i++) {
        if (i % 16U == 0U) {
            fprintf(stderr, "\r\n%03X:", i);
        } else if (i % 8U == 0U) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, " %02x", data[i]);
    }
    if (len < length) {
        fprintf(stderr, "\r\n...(Skip %d Bytes)", length - len);
    }
    fprintf(stderr, "\r\n");
}
#endif

#ifdef DEBUGIP
static void debug_ip_tx(struct mbuf *m)
{
    if ((m->m_data[0xc] == 0x08) && (m->m_data[0xd] == 0x06)) {
        if (m->m_data[0x15] == 0x01) {
            fprintf(stderr, ANSI_YELLOW "[TX] ARP Req\n" ANSI_RESET);
        } else {
            fprintf(stderr, ANSI_YELLOW "[TX] ARP Reply\n" ANSI_RESET);
        }
    }

    if ((m->m_data[0xc] == 0x08) && (m->m_data[0xd] == 0x00)) {
        if (m->m_data[0x17] == 0x01) {
            if (m->m_data[0x22] == 0x08) {
                fprintf(stderr, ANSI_YELLOW "[TX] ICMP Req %d\n" ANSI_RESET, m->m_data[0x13]);
            } else {
                fprintf(stderr, ANSI_YELLOW "[TX] ICMP Reply %d\n" ANSI_RESET, m->m_data[0x13]);
            }
        }
        if (m->m_data[0x17] == 0x11) {
            fprintf(stderr, ANSI_YELLOW "[TX] UDP %d\n" ANSI_RESET, m->m_data[0x13]);
        }
        if (m->m_data[0x17] == 0x06) {
            fprintf(stderr, ANSI_YELLOW "[TX] TCP %d\n" ANSI_RESET, m->m_data[0x13]);
        }
    }
}

static void debug_ip_rx(struct mbuf *m)
{
    if ((m->m_data[0xc] == 0x08) && (m->m_data[0xd] == 0x06)) {
        if (m->m_data[0x15] == 0x01) {
            fprintf(stderr, ANSI_MAGENTA "[RX] ARP Req\n" ANSI_RESET);
        } else {
            fprintf(stderr, ANSI_MAGENTA "[RX] ARP Reply\n" ANSI_RESET);
        }
    }

    if ((m->m_data[0xc] == 0x08) && (m->m_data[0xd] == 0x00)) {
        if (m->m_data[0x17] == 0x01) {
            if (m->m_data[0x22] == 0x08) {
                fprintf(stderr, ANSI_MAGENTA "[RX] ICMP Req %d\n" ANSI_RESET, m->m_data[0x13]);
            } else {
                fprintf(stderr, ANSI_MAGENTA "[RX] ICMP Reply %d\n" ANSI_RESET, m->m_data[0x13]);
            }
        }
        if (m->m_data[0x17] == 0x11) {
            fprintf(stderr, ANSI_MAGENTA "[RX] UDP %d\n" ANSI_RESET, m->m_data[0x13]);
        }
        if (m->m_data[0x17] == 0x06) {
            fprintf(stderr, ANSI_MAGENTA "[RX] TCP %d\n" ANSI_RESET, m->m_data[0x13]);
        }
    }
}
#endif

static void *amba_desc_calloc(size_t size)
{
    void *desc;
    off64_t offset;

    desc = mmap (NULL, size, PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_SHARED, enet_fd, 0);
    if (desc == MAP_FAILED) {
        fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
        return NULL;
    }
    if (mem_offset64(desc, NOFD, 1, &offset, 0) == -1) {
        fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
        return NULL;
    }
    memset(desc, 0x00, size);
    if (smmu_map_driver_memory((_Uint64t)offset, size) != 0) {
        fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
        return NULL;
    }

    return desc;
}

/* reap/m_freem last pkt */
static void amba_reap_tx(UINT32 Idx, UINT32 full_reap)
{
    extern AMBA_ENET_CONFIG_s EnetConfig[ENET_INSTANCES];
    AMBA_ENET_CONFIG_s *pEnetConfig = &EnetConfig[Idx];
    UINT32 i, TxDirty;
    AMBA_ENET_DES_s *pDes = pEnetConfig->pDes;
    AMBA_ENET_TDES_s *pTDES;

    TxDirty = g_reaped % pDes->TDESCnt;
    for (i = 0U; i < pDes->TDESCnt; i++) {
        /* owner=host, reap TX */
        pTDES = &pDes->pTDES[TxDirty];

        if (AmbaCSL_EnetTDESGetDmaOwn(pTDES) == 0U) {
            if (txm[TxDirty] != NULL) {
#if defined(DEBUGMBUF)
                AMBA_ENET_STAT_s *pEnetStat = &AmbaCSL_EnetStat[Idx];
                AmbaCSL_EnetTDESDumpStatus(pTDES, pEnetStat);
                fprintf(stderr, "%d txm[%d] 0x%lx reaped\n", g_reaped, TxDirty, mbuf_phys(txm[TxDirty]));
#endif
                m_freem(txm[TxDirty]);
                txm[TxDirty] = NULL;
                g_reaped++;
                TxDirty = g_reaped % pDes->TDESCnt;
                continue;
            }
        }
        TxDirty = (TxDirty + 1U) % pDes->TDESCnt;

        /*quick reap*/
        if ((full_reap == 0) && (g_reaped == g_seeded)) {
            break;
        }

        /* force go through ALL tx descriptors */
        continue;
    }
}

static struct mbuf* amba_mbuf_defrag(struct mbuf *m, struct ifnet *ifp)
{
    if (m->m_next != NULL) {
        struct mbuf *m2 = m_getcl(M_NOWAIT, MT_DATA, M_PKTHDR);
        if (m2 == NULL) {
            fprintf(stderr, "%s ENET %s(%d) error m_getcl, seeded %d reaped %d %s\n", ANSI_RED, __func__, __LINE__, g_seeded, g_reaped, ANSI_RESET);
            m_freem(m);
            ifp->if_oerrors++;
            return NULL;
        }

        m_copydata(m, 0, m->m_pkthdr.len, mtod(m2, caddr_t));
        m2->m_pkthdr.len = m2->m_len = m->m_pkthdr.len;
        m_freem(m);
        m = m2;
    }
    return m;
}

static void amba_monitor(void *arg)
{
//    UINT32 value;
//    AmbaEnet_PhyRead(_Idx, 0U, 0x2, &value);
//    fprintf(stderr, "phytool read eth0/0/0x2 value 0x%04x \n", value);
//    MDI_MonitorPhy(enet_mdi);
    (void)arg;
    /* start next housekeeping 2 seconds later */
    callout_msec(&eth_callout, 2000, amba_monitor, 0);
}

static uint16_t amba_mii_read(void *handle, uint8_t phy_id, uint8_t location)
{
    (void)handle;
    UINT32 value;

    AmbaEnet_PhyRead(_Idx, (UINT32)phy_id, (UINT32)location, &value);

    return (uint16_t)value;
}

static void amba_mii_write(void *handle, uint8_t phy_id, uint8_t location, uint16_t data)
{
    (void)handle;
    (void)AmbaEnet_PhyWrite(_Idx, (UINT32)phy_id, (UINT32)location, (UINT32)data);
}

static void amba_mii_callback(void *handle, uint8_t phyaddr, uint8_t linkstate)
{
    (void)handle;
    (void)phyaddr;
    (void)linkstate;
}

static void amba_init(const UINT32 Idx, struct sam_dev *sam)
{
    extern AMBA_ENET_CONFIG_s EnetConfig[ENET_INSTANCES];
    AMBA_ENET_CONFIG_s *pEnetConfig = &EnetConfig[Idx];

    (void) AmbaEnet_SetConfig(Idx, &(EnetConfig[Idx]));

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    (void)amba_mmap_reg(CONFIG_DTB_LOADADDR, CONFIG_DTB_SIZE, PROT_READ | PROT_WRITE);
#endif

    (void)amba_mmap_reg(reg_base, 0x2000, PROT_READ | PROT_WRITE | PROT_NOCACHE);
    if (smmu_register_mmio_device(reg_base, 0x2000) != 0) {
        fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
    }
    (void)amba_mmap_reg(AHB_SCRATCHPAD_BASE, 0x100, PROT_READ | PROT_WRITE | PROT_NOCACHE);
    (void)amba_mmap_reg(RCT_BASE, 0x1000, PROT_READ | PROT_WRITE | PROT_NOCACHE);

    enet_cache.fd = NOFD;
    cache_init(0, &enet_cache, NULL);

    if (ThreadCtl(_NTO_TCTL_IO_PRIV, 0) == -1) {
        //cache operation results in a SIGILL (illegal instruction)
        fprintf(stderr, "%s ENET %s(%d) error %s\n", ANSI_RED, __func__, __LINE__, ANSI_RESET);
    }

    memcpy(pEnetConfig->Mac, sam->cfg.current_address, ETHER_ADDR_LEN);
    (void)AmbaEnet_Init(pEnetConfig);

    EnetUserTaskCreate(_Idx);
    fprintf(stderr, "%02x:%02x:%02x:%02x:%02x:%02x\n",
            pEnetConfig->Mac[0], pEnetConfig->Mac[1],
            pEnetConfig->Mac[2], pEnetConfig->Mac[3],
            pEnetConfig->Mac[4], pEnetConfig->Mac[5]);

    /* start housekeeping 3 seconds later */
    callout_init(&eth_callout);
    callout_msec(&eth_callout, 3000, amba_monitor, 0);

    /* 802.3 MDIO library */
    MDI_Register_Extended(sam, amba_mii_write, amba_mii_read, amba_mii_callback, &enet_mdi, NULL, 0, 0);
    //MDI_InitPhy(enet_mdi, 0);
    //MDI_EnableMonitor(enet_mdi, 0);
}

static void amba_deinit(const UINT32 Idx)
{
    extern AMBA_ENET_CONFIG_s EnetConfig[ENET_INSTANCES];
    AMBA_ENET_CONFIG_s *pEnetConfig = &EnetConfig[Idx];
    AMBA_ENET_DES_s *pDes = pEnetConfig->pDes;
    size_t s1, s2;
    UINT32 i;

    /* tx dma buf*/
    for (i = 0; i < pDes->TDESCnt; i++) {
        if (txm[i] != NULL) {
            m_freem(txm[i]);
            txm[i] = NULL;
        }
    }

    /* rx dma buf*/
    for (i = 0; i < pDes->RDESCnt; i++) {
        if (rxm[i] != NULL) {
            m_freem(rxm[i]);
            rxm[i] = NULL;
        }
    }

    /* tx dma desc */
    s1 = sizeof(AMBA_ENET_TDES_s) * BOARD_ETH_TX_FRAMES;
    s2 = sizeof(AMBA_ENET_DMA_s) * BOARD_ETH_TX_FRAMES;
    munmap((void *)pDes->pTDES, s1 + s2);

    /* rx dma desc */
    s1 = sizeof(AMBA_ENET_RDES_s) * BOARD_ETH_RX_FRAMES;
    s2 = sizeof(AMBA_ENET_DMA_s) * BOARD_ETH_RX_FRAMES;
    munmap((void *)pDes->pRDES, s1 + s2);

    /* fd */
    if (enet_fd) {
        close(enet_fd);
    }

    /* cache_ctrl */
    cache_fini(&enet_cache);

    /* callout */
    callout_stop(&eth_callout);

    /* 802.3 MDIO library */
    MDI_DeRegister(&enet_mdi);
}

/* called from pPreInitCb */
void amba_setup_dma_desc(UINT32 Idx)
{
    extern AMBA_ENET_CONFIG_s EnetConfig[ENET_INSTANCES];
    AMBA_ENET_CONFIG_s *pEnetConfig = &EnetConfig[Idx];
    AMBA_ENET_DES_s *pDes;
    struct mbuf *mbf;
    off64_t phys;
    void *ptr;
    size_t s1, s2;
    UINT32 i;

    pDes = pEnetConfig->pDes;

    /*enet_fd*/
    enet_fd = posix_typed_mem_open("below4G", O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
    if (enet_fd == -1) {
        fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
        return;
    }

    /* tx dma desc */
    s1 = sizeof(AMBA_ENET_TDES_s) * BOARD_ETH_TX_FRAMES;
    s2 = sizeof(AMBA_ENET_DMA_s) * BOARD_ETH_TX_FRAMES;
    ptr = amba_desc_calloc(s1 + s2);
    pDes->pTDES = ptr;
    if (pDes->pTDES == NULL) {
        fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
        return;
    }

    pDes->pTxDma = ptr + s1;
    if (pDes->pTxDma == NULL) {
        fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
        return;
    }

    /* rx dma desc */
    s1 = sizeof(AMBA_ENET_RDES_s) * BOARD_ETH_RX_FRAMES;
    s2 = sizeof(AMBA_ENET_DMA_s) * BOARD_ETH_RX_FRAMES;
    ptr = amba_desc_calloc(s1 + s2);
    pDes->pRDES = ptr;
    if (pDes->pRDES == NULL) {
        fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
        return;
    }

    pDes->pRxDma = ptr + s1;
    if (pDes->pRxDma == NULL) {
        fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
        return;
    }

    /* tx dma buf*/
    for (i = 0; i < pDes->TDESCnt; i++) {
#if (TX_COPY == 0)
        pDes->pTxDma[i].Buf = NULL;
        txm[i] = NULL;
#else
        mbf = m_getcl(M_DONTWAIT, MT_DATA, M_PKTHDR);
        if (mbf == NULL) {
            fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
            return;
        }
        pDes->pTxDma[i].Buf = (UINT8 *)mbf->m_data;
        txm[i] = mbf;
        phys = mbuf_phys(mbf);
        CACHE_FLUSH(&enet_cache, mbf->m_data, phys, mbf->m_ext.ext_size);
#if defined(DEBUGMBUF)
        fprintf(stderr, "txm[%d] 0x%lx\n", i, phys);
#endif
#endif
    }

    /* rx dma buf*/
    for (i = 0; i < pDes->RDESCnt; i++) {
        mbf = m_getcl(M_DONTWAIT, MT_DATA, M_PKTHDR);
        if (mbf == NULL) {
            fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
            return;
        }
        pDes->pRxDma[i].Buf = (UINT8 *)mbf->m_data;
        rxm[i] = mbf;
        phys = mbuf_phys(mbf);
        CACHE_FLUSH(&enet_cache, mbf->m_data, phys, mbf->m_ext.ext_size);
#if defined(DEBUGMBUF)
        fprintf(stderr, "rxm[%d] 0x%lx\n", i, phys);
#endif
    }
}

#if defined(AMBA_HW_PTP)
static int amba_ptp_get_rx_timestamp(ptp_extts_t *ts)
{
    UINT32 i;

    if (ts == NULL) {
        fprintf(stderr, ANSI_RED "ENET %s(%d) NULL ts\n" ANSI_RESET, __func__, __LINE__);
        return 0;
    }

    for (i = 0; i < BOARD_ETH_RX_FRAMES; i++) {
        if ((ts->msg_type == rx_ts[i].msg_type) &&
            (ts->sequence_id == rx_ts[i].sequence_id) &&
            (ts->sport_id == rx_ts[i].sport_id) &&
            !memcmp(ts->clock_identity, rx_ts[i].clock_identity,
                    sizeof(ts->clock_identity))) {
            ts->ts.nsec = rx_ts[i].ts.nsec;
            ts->ts.sec = rx_ts[i].ts.sec;
            if (amba_debug) {
                fprintf(stderr, "-%d rx_ts[%d] 0x%x %u.%u\n", rx_ts[i].sequence_id, i,
                        rx_ts[i].msg_type, rx_ts[i].ts.sec, rx_ts[i].ts.nsec);
            }
            return 1;
        }
    }
    if (amba_debug) {
        fprintf(stderr, "ENET %s(%d) seq %d type %d NOT FOUND after PTP_SET_TIME\n", __func__, __LINE__, ts->sequence_id, ts->msg_type);
    }
    return 0;
}

static void amba_ptp_add_rx_timestamp(UINT32 Idx, UINT32 RxCur, ptpv2hdr_t *ph)
{
    extern AMBA_ENET_CONFIG_s EnetConfig[ENET_INSTANCES];
    AMBA_ENET_CONFIG_s *pEnetConfig = &EnetConfig[Idx];
    AMBA_ENET_DES_s *pDes = pEnetConfig->pDes;
    AMBA_ENET_RDES_s *pRDES = &pDes->pRDES[RxCur];
    static UINT32 rx_ts_cnt = 0;

    rx_ts[rx_ts_cnt].msg_type = ph->messageId & 0x0f;
    rx_ts[rx_ts_cnt].sequence_id = ntohs(ph->sequenceId);
    rx_ts[rx_ts_cnt].sport_id = ntohs(ph->sportId);
    memcpy(rx_ts[rx_ts_cnt].clock_identity, ph->clockIdentity, sizeof(rx_ts[rx_ts_cnt].clock_identity));

    rx_ts[rx_ts_cnt].ts.nsec = pRDES->RDES6_RTSL;
    rx_ts[rx_ts_cnt].ts.sec = pRDES->RDES7_RTSH;
    if (amba_debug) {
        fprintf(stderr, "+%d rx_ts[%d] 0x%x %u.%u\n", rx_ts[rx_ts_cnt].sequence_id, rx_ts_cnt,
                rx_ts[rx_ts_cnt].msg_type, rx_ts[rx_ts_cnt].ts.sec, rx_ts[rx_ts_cnt].ts.nsec);
    }
    rx_ts_cnt = (rx_ts_cnt + 1) % BOARD_ETH_RX_FRAMES;
}

static int amba_ptp_get_tx_timestamp(ptp_extts_t *ts)
{
    UINT32 i;

    if (ts == NULL) {
        return 0;
    }

    for (i = 0; i < BOARD_ETH_TX_FRAMES; i++) {
        if ((ts->msg_type == tx_ts[i].msg_type) &&
            (ts->sequence_id == tx_ts[i].sequence_id) &&
            (ts->sport_id == tx_ts[i].sport_id) &&
            !memcmp(ts->clock_identity, tx_ts[i].clock_identity,
                    sizeof(ts->clock_identity))) {
            ts->ts.nsec = tx_ts[i].ts.nsec;
            ts->ts.sec = tx_ts[i].ts.sec;
            if (amba_debug) {
                fprintf(stderr, "-%d tx_ts[%d] 0x%x %u.%u\n", tx_ts[i].sequence_id, i,
                        tx_ts[i].msg_type, tx_ts[i].ts.sec, tx_ts[i].ts.nsec);
            }
            return 1;
        }
    }
    return 0;
}

static void amba_ptp_add_tx_timestamp(UINT32 Idx, UINT32 TxCur, ptpv2hdr_t *ph)
{
    extern AMBA_ENET_CONFIG_s EnetConfig[ENET_INSTANCES];
    AMBA_ENET_CONFIG_s *pEnetConfig = &EnetConfig[Idx];
    AMBA_ENET_DES_s *pDes = pEnetConfig->pDes;
    AMBA_ENET_TDES_s *pTDES = &pDes->pTDES[TxCur];
    static UINT32 tx_ts_cnt = 0;
    int i;

    for (i = 0U; i < 0xfff; i++) {
        if (AmbaCSL_EnetTDESGetTTSS(pTDES) != 0U) {
            break;
        }
        usleep(1);
        //(void) sched_yield(); //yield() leads qnx710 100% cpu usage
    }

    if (AmbaCSL_EnetTDESGetTTSS(pTDES) == 0U) {
        fprintf(stderr, "ENET %s(%d) skipped msg_type %d\n", __func__, __LINE__, ph->messageId & 0x0f);
        return;
    }

    tx_ts[tx_ts_cnt].msg_type = ph->messageId & 0x0f;
    tx_ts[tx_ts_cnt].sequence_id = ntohs(ph->sequenceId);
    tx_ts[tx_ts_cnt].sport_id = ntohs(ph->sportId);
    memcpy(tx_ts[tx_ts_cnt].clock_identity, ph->clockIdentity, sizeof(tx_ts[tx_ts_cnt].clock_identity));

    tx_ts[tx_ts_cnt].ts.nsec = pTDES->TDES6_TTSL;
    tx_ts[tx_ts_cnt].ts.sec = pTDES->TDES7_TTSH;
    if (amba_debug) {
        fprintf(stderr, "+%d tx_ts[%d] 0x%x %u.%u\n", tx_ts[tx_ts_cnt].sequence_id, tx_ts_cnt,
                tx_ts[tx_ts_cnt].msg_type, tx_ts[tx_ts_cnt].ts.sec, tx_ts[tx_ts_cnt].ts.nsec);
    }
    tx_ts_cnt = (tx_ts_cnt + 1) % BOARD_ETH_TX_FRAMES;
}

int amba_ptp_ioctl(struct ifdrv *ifd)
{
    ptp_time_t time;
    const AMBA_ENET_TS_s *const pPhcTs = pAmbaCSL_EnetPhc[_Idx];
    ptp_comp_t comp;
    int ppb;
    ptp_extts_t ts;
    uint8_t found;

    switch(ifd->ifd_cmd) {
    case PTP_GET_RX_TIMESTAMP:
    case PTP_GET_TX_TIMESTAMP:
        if (ifd->ifd_len != sizeof(ts)) {
            return EINVAL;
        }

        if (ISSTACK) {
            if (copyin((((uint8_t *)ifd) + sizeof(*ifd)),
                       &ts, sizeof(ts))) {
                return EINVAL;
            }
        } else {
            memcpy(&ts, (((uint8_t *)ifd) + sizeof(*ifd)), sizeof(ts));
        }

        if (ifd->ifd_cmd == PTP_GET_RX_TIMESTAMP) {
            found = amba_ptp_get_rx_timestamp(&ts);
        } else {
            found = amba_ptp_get_tx_timestamp(&ts);
        }

        if (found) {
            if (ISSTACK) {
                return (copyout(&ts, (((uint8_t *)ifd) + sizeof(*ifd)),
                                sizeof(ts)));
            } else {
                memcpy((((uint8_t *)ifd) + sizeof(*ifd)), &ts, sizeof(ts));
                return EOK;
            }
        }
        return ENOENT;
        break;

    case PTP_GET_TIME:
        if (ifd->ifd_len != sizeof(time)) {
            fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
            return EINVAL;
        }
        time.sec = pPhcTs->Sec;
        time.nsec = pPhcTs->Ns;

        if (ISSTACK) {
            return (copyout(&time, (((uint8_t *)ifd) + sizeof(*ifd)),
                            sizeof(time)));
        } else {
            memcpy((((uint8_t *)ifd) + sizeof(*ifd)), &time, sizeof(time));
            return EOK;
        }
        break;

    case PTP_SET_TIME:
        if (ifd->ifd_len != sizeof(time)) {
            fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
            return EINVAL;
        }
        if (ISSTACK) {
            if (copyin((((uint8_t *)ifd) + sizeof(*ifd)),
                       &time, sizeof(time))) {
                fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
                return EINVAL;
            }
        } else {
            memcpy(&time, (((uint8_t *)ifd) + sizeof(*ifd)), sizeof(time));
        }
        if (amba_debug) {
            fprintf(stderr, "PTP_SET_TIME %u.%u\n", time.sec, time.nsec);
        }
        /* Clock has changed so all old ts are invalid */
        memset(rx_ts, 0, sizeof(rx_ts));
        memset(tx_ts, 0, sizeof(tx_ts));
        (void)AmbaEnet_PhcSetTs(_Idx, time.sec, time.nsec);
        return EOK;
        break;

    case PTP_SET_COMPENSATION:
        if (ifd->ifd_len != sizeof(comp)) {
            fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
            return EINVAL;
        }
        if (ISSTACK) {
            if (copyin((((uint8_t *)ifd) + sizeof(*ifd)),
                       &comp, sizeof(comp))) {
                fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
                return EINVAL;
            }
        } else {
            memcpy(&comp, (((uint8_t *)ifd) + sizeof(*ifd)), sizeof(comp));
        }
        ppb = comp.comp;
        if (comp.positive == 0) {
            ppb = -ppb;
        }
        if (amba_debug) {
            fprintf(stderr, "PTP_SET_COMPENSATION %d ppb\n", ppb);
        }
        (void)AmbaEnet_PhcAdjFreq(_Idx, ppb);
        return EOK;
        break;

    case PTP_GET_COMPENSATION:
        fprintf(stderr, "PTP_GET_COMPENSATION\n");
        return ENOTTY;
        break;

    default:
        break;
    }

    fprintf(stderr, "ioctl return ENOTTY\n");
    return ENOTTY;
}
#endif //#if defined(AMBA_HW_PTP)

static void amba_tx(UINT32 Idx, struct mbuf *m, struct ifnet *ifp, UINT32 copy)
{
    struct mbuf *mbf = m;

    if (0 == copy) {
        extern UINT32 GetTxCur(UINT32 Idx);
        int i = 0, FS, LS;
        UINT32 TxCur;
        off64_t phys;

#ifdef DEBUGIP
        debug_ip_tx(m);
#endif

        /* quick reap mbuf */
        amba_reap_tx(Idx, 0);

        while (mbf != NULL) {
            phys = mbuf_phys(mbf);
            CACHE_FLUSH(&enet_cache, mbf->m_data, phys, mbf->m_len);
#if defined(TX_DUMP)
            amba_dump_buffer("TX", (const UINT8 *)mbf->m_data, mbf->m_len);
#endif

            if (mbf == m) {
                FS = 1U;
            } else {
                FS = 0U;
            }

            if (mbf->m_next == NULL) {
                LS = 1U;
                ifp->if_opackets++;
            } else {
                LS = 0U;
            }

            /* seed mbuf */
            TxCur = GetTxCur(_Idx);
#if defined(FORCE_FREEMBUF)
            if (txm[TxCur] != NULL) {
                /* reap ALL */
                amba_reap_tx(Idx, 1);
                if (txm[TxCur] != NULL) {
                    /* force-free mbuf */
                    m_freem(txm[TxCur]);
                    txm[TxCur] = NULL;
                    g_reaped++;
#if 1 //defined(DEBUGMBUF)
                    fprintf(stderr, "ENET %s(%d) seeded %d reaped %d\n", __func__, __LINE__, g_seeded, g_reaped);
#endif
                }
            }
#else
            /*wait for dma done, do not force-free mbuf*/
            while (txm[TxCur] != NULL) {
                printf("ENET %s(%d) seeded %d reaped %d\n", __func__, __LINE__, g_seeded, g_reaped);

                /* reap ALL */
                amba_reap_tx(Idx, 1);
                if (txm[TxCur] != NULL) {
                    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[_Idx];
                    fprintf(stderr, "ENET %s(%d) seeded %d reaped %d\n", __func__, __LINE__, g_seeded, g_reaped);
                    /*force tx poll*/
                    AmbaCSL_EnetDmaStopTX(pEnetReg);
                    usleep(1);
                    AmbaCSL_EnetDmaStartTX(pEnetReg);
                    usleep(1);
                    AmbaCSL_EnetDmaSetTxPoll(pEnetReg);
                }
            }
#endif

            txm[TxCur] = mbf;
            g_seeded++;
#if defined(DEBUGMBUF)
            fprintf(stderr, "%d txm[%d] 0x%lx seeded\n", g_seeded, TxCur, phys);
#endif

            /* tx pause: do dummy loop*/
            for (i = 0; i < (int)tx_pause; i++) {
                __asm__ __volatile__ ("NOP");
            }
            mem_barrier();

            AmbaEnet_SetupTxDesc(Idx, (void *)phys, mbf->m_len, FS, LS, 0U);

#if defined(AMBA_HW_PTP)
            if (LS == 1U) {
                struct ether_header *eh = (struct ether_header *)mbf->m_data;
                uint8_t msg_type = ~0x0;
                ptpv2hdr_t *ph;
                /* gPTP: eth.type==0x88f7 */
                if (ntohs(eh->ether_type) == 0x88F7) {
                    ph = (ptpv2hdr_t *)(mbf->m_data + sizeof(*eh));
                    msg_type = ph->messageId & 0x0f;
                }
                /* PTPv2: udp port==319 */
                else if ((ntohs(eh->ether_type) == 0x0800) &&
                         (mbf->m_data[0x17] == 0x11) &&
                         (mbf->m_data[0x24] == 0x01) && (mbf->m_data[0x25] == 0x3f)) {
                    /* 14 + 20 + 8 = 42 */
                    ph = (ptpv2hdr_t *)(mbf->m_data + 42);
                    msg_type = ph->messageId & 0x0f;
                }
                /* 0:SYNC, 1:DelayReq, 2:PdelayReq, 3: PdelayResp */
                if (msg_type < 0x04U) {
                    amba_ptp_add_tx_timestamp(Idx, TxCur, ph);
                }
            }
#endif

#if NBPFILTER > 0
            /* Pass the packet to any BPF listeners */
            if (ifp->if_bpf) {
                bpf_mtap(ifp->if_bpf, m);
            }
#endif
            i++;
            mbf = mbf->m_next;
        }
    } else {
        /* copy and send */
        void *ptr, *_ptr;
        int len = 0;
        off64_t phys;

#ifdef DEBUGIP
        debug_ip_tx(mbf);
#endif

        AmbaEnet_GetTxBuf(_Idx, &_ptr);
        ptr = _ptr;

        while (mbf != NULL) {
            memcpy(ptr, mbf->m_data, mbf->m_len);
            ptr += mbf->m_len;
            len += mbf->m_len;
            mbf = mbf->m_next;
        }
        (void)mem_offset64((const void *)_ptr, NOFD, 1, &phys, 0);
        CACHE_FLUSH(&enet_cache, _ptr, phys, len);
#if defined(TX_DUMP)
        amba_dump_buffer("TX", (const UINT8 *)_ptr, len);
#endif
        AmbaEnet_Tx(_Idx, len);
        ifp->if_opackets++;

        /*m_freem will free an entire mbuf chain*/
        m_freem(m);
    }
}


/* called from io-pkt */
int sam_process_interrupt(void *arg, struct nw_work_thread *wtp)
{
    int copy = RX_COPY;
    extern void QNX_EthIdrHdlr(UINT32 Idx);
    extern UINT32 GetRxCur(UINT32 Idx);
    extern void EnetRxCurAdv(UINT32 Idx);
    extern AMBA_ENET_CONFIG_s EnetConfig[ENET_INSTANCES];
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[_Idx];
    AMBA_ENET_CONFIG_s *pEnetConfig = &EnetConfig[_Idx];
    AMBA_ENET_DES_s *pDes = pEnetConfig->pDes;
    AMBA_ENET_RDES_s *pRDES;
    UINT32 RxCur = GetRxCur(_Idx);
    UINT8 *pBuf;
    UINT32 i;
    struct sam_dev *sam;
    struct ifnet *ifp;
    struct mbuf *mbf, *new_mbf;
    off64_t phys;

    sam = arg;
    ifp = &sam->sc_ec.ec_if;

    QNX_EthIdrHdlr(_Idx);
    for (i = 0U; i < pDes->RDESCnt; i++) {
        /* owner=host, handle RX */
        pRDES = &pDes->pRDES[RxCur];
        if (AmbaCSL_EnetRDESGetDmaOwn(pRDES) == 0U) {
            if (0 == copy) {
                /* Send mbf up */
                mbf = rxm[RxCur];
                pBuf = pDes->pRxDma[RxCur].Buf;
                if (pBuf != (UINT8 *)mbf->m_data) {
                    fprintf(stderr, ANSI_RED "err: rxm[%d].m_data 0x%p != pRxDma[%d] 0x%p\n" ANSI_RESET, RxCur, mbf->m_data, RxCur, pBuf);
                    break;
                }
                new_mbf = m_getcl_wtp(M_DONTWAIT, MT_DATA, M_PKTHDR, wtp);
                if (!new_mbf) {
                    ifp->if_ierrors++;
                    fprintf(stderr, ANSI_RED "ENET %s(%d) m_getcl_wtp failed\n" ANSI_RESET, __func__, __LINE__);
                    break;
                }
                ifp->if_ipackets++;
                mbf->m_pkthdr.rcvif = ifp;
                mbf->m_pkthdr.len = mbf->m_len = AmbaCSL_EnetRDESGetFL(pRDES) - 4U;
                phys = mbuf_phys(mbf);
#if defined(DEBUGMBUF)
                fprintf(stderr, "rxm[%d] 0x%lx size %d\n", RxCur, phys, mbf->m_len);
#endif
                CACHE_INVAL(&enet_cache, mbf->m_data, phys, mbf->m_ext.ext_size);
#ifdef DEBUGIP
                debug_ip_rx(mbf);
#endif
#if defined(RX_DUMP)
                amba_dump_buffer("RX", (const UINT8 *)mbf->m_data, mbf->m_len);
#endif

#if defined(AMBA_HW_PTP)
                {
                    struct ether_header *eh = (struct ether_header *)mbf->m_data;
                    uint8_t msg_type = ~0x0;
                    ptpv2hdr_t *ph;
                    /* gPTP: eth.type==0x88f7 */
                    if (ntohs(eh->ether_type) == 0x88F7) {
                        ph = (ptpv2hdr_t *)(mbf->m_data + sizeof(*eh));
                        msg_type = ph->messageId & 0x0f;
                    }
                    /* PTPv2: udp port==319 */
                    else if ((ntohs(eh->ether_type) == 0x0800) &&
                             (mbf->m_data[0x17] == 0x11) &&
                             (mbf->m_data[0x24] == 0x01) && (mbf->m_data[0x25] == 0x3f)) {
                        /* 14 + 20 + 8 = 42 */
                        ph = (ptpv2hdr_t *)(mbf->m_data + 42);
                        msg_type = ph->messageId & 0x0f;
                    }
                    /* 0:SYNC, 1:DelayReq, 2:PdelayReq, 3: PdelayResp */
                    if (msg_type < 0x04U) {
                        amba_ptp_add_rx_timestamp(_Idx, RxCur, ph);
                    }
                }
#endif

#if NBPFILTER > 0
                /* Pass this up to any BPF listeners. e.g. ptpd-avb */
                if (ifp->if_bpf) {
                    bpf_mtap(ifp->if_bpf, mbf);
                }
#endif

#if defined(RX_SKIP_IOPKT)
                if ((mbf->m_len > 1500) && (mbf->m_data[0x17] == 0x11)) { //UDP iperf traffic
                    static UINT32 bytes = 0U, total = 0U, datagrams = 0U;
                    static UINT32 tv1 = 0U;
                    UINT32 tv2 = 0U;
                    struct timespec tp;
                    static char dummy[1536];

                    if (bytes == 0U) {
                        clock_gettime(CLOCK_REALTIME, &tp);
                        tv1 = tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
                    }

                    bytes += mbf->m_len;
                    total += mbf->m_len;
                    datagrams++;
                    if(bytes >= mbf->m_len*1024U*8U) {
                        UINT32 msec, kbits;
                        UINT32 kbps;
                        clock_gettime(CLOCK_REALTIME, &tp);
                        tv2 = tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
                        // 8/1024 = 1/128
                        kbits = bytes >> 7;
                        msec = tv2 - tv1;
                        kbits = kbits*1000U;
                        kbps = kbits/msec;
                        bytes = 0;
                        /*
                        [8192] 12402688 96896000/101*1000= 959366 kbps
                        [16384] 24805376 96896000/100*1000= 968960 kbps
                        */
                        fprintf(stderr, "[%d] %d %d/%d*1000= %d kbps\n", datagrams, total, kbits, msec, kbps);
                    }
                    memcpy(dummy, mbf->m_data, mbf->m_len);
                    m_freem(mbf);
                } else
#endif
                    (*ifp->if_input)(ifp, mbf);

                phys = mbuf_phys(new_mbf);
                pDes->pRxDma[RxCur].Buf = (UINT8 *)new_mbf->m_data;
                rxm[RxCur] = new_mbf;
                AmbaCSL_EnetRDES2Init(pRDES, (UINT32)phys);
                CACHE_FLUSH(&enet_cache, new_mbf->m_data, phys, new_mbf->m_ext.ext_size);
            } else {
                /* copy and send to io-pkt */
                struct mbuf *m2 = rxm[RxCur];
                mbf = m_getcl_wtp(M_DONTWAIT, MT_DATA, M_PKTHDR, wtp);
                if (!mbf) {
                    ifp->if_ierrors++;
                    fprintf(stderr, ANSI_RED "ENET %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
                    break;
                }

                ifp->if_ipackets++;
                mbf->m_pkthdr.rcvif = ifp;
                m2->m_pkthdr.len = m2->m_len = mbf->m_pkthdr.len = mbf->m_len = AmbaCSL_EnetRDESGetFL(pRDES) - 4U;
                phys = mbuf_phys(mbf);
#if defined(DEBUGMBUF)
                fprintf(stderr, "rxm[%d] 0x%lx size %d\n", RxCur, phys, mbf->m_len);
#endif
                CACHE_FLUSH(&enet_cache, mbf->m_data, phys, mbf->m_ext.ext_size);
                CACHE_INVAL(&enet_cache, m2->m_data, phys, m2->m_len);
                memcpy(mbf->m_data, m2->m_data, mbf->m_len);
#ifdef DEBUGIP
                debug_ip_rx(mbf);
#endif
#if defined(RX_DUMP)
                amba_dump_buffer("RX", (const UINT8 *)mbf->m_data, mbf->m_len);
#endif
                (*ifp->if_input)(ifp, mbf);
            }

            if (sam->cfg.verbose) {
                AMBA_ENET_STAT_s *pEnetStat = &AmbaCSL_EnetStat[_Idx];
                AmbaCSL_EnetRDESDumpStatus(pRDES, pEnetStat);
            }
            /* RX done, set owner=DMA to get next frame */
            AmbaCSL_EnetRDESSetDmaOwn(pRDES);

            EnetRxCurAdv(_Idx);
            RxCur = GetRxCur(_Idx);
            continue;
        }
        break;
    }
    AmbaCSL_EnetDmaRxIrqEnable(pEnetReg);

    sam->sc_intr_cnt++;
    if (sam->cfg.verbose) {
        fprintf(stderr, "[%d] 0x%08x\n", sam->sc_intr_cnt, irq_flags);
    }

    return 1;
}

static int amba_ioctl(struct ifnet *ifp, unsigned long cmd, caddr_t data)
{
    int error = ENOTTY;
    struct ifdrv_com *ifdc;
    struct drvcom_config *dcfgp;
    struct dcom_mii_config *mii_conf;
    struct mii_ioctl_data *mii_data;
    struct sam_dev *sam = ifp->if_softc;

    switch (cmd) {
    case SIOCSDRVSPEC: /* set driver spec */
        fprintf(stderr, "SIOCSDRVSPEC\n");
    /*fall through */
    case SIOCGDRVSPEC: /* get driver spec */
#if defined(AMBA_HW_PTP)
    {
        struct ifdrv *ifd = (struct ifdrv *)data;
        error = amba_ptp_ioctl(ifd);
    }
#endif
    break;

    case SIOCGDRVCOM: /* driver common info */
        ifdc = (struct ifdrv_com *)data;
        switch (ifdc->ifdc_cmd) {
        case DRVCOM_CONFIG:
            dcfgp = (struct drvcom_config *)ifdc;

            if (ifdc->ifdc_len != sizeof(nic_config_t)) {
                error = EINVAL;
                break;
            }
            memcpy(&dcfgp->dcom_config, &sam->cfg, sizeof(sam->cfg));
            error = 0;
            break;

        case SIOCGMIIREG: /* PhyRead */
            mii_conf = (struct dcom_mii_config *)ifdc;
            mii_data = &(mii_conf->ifdc_data);
            error = AmbaEnet_PhyRead(_Idx, mii_data->phy_id, mii_data->reg_num, (UINT32 *)&mii_data->val_out);
            break;

        case SIOCSMIIREG: /* PhyWrite */
            mii_conf = (struct dcom_mii_config *)ifdc;
            mii_data = &(mii_conf->ifdc_data);
            error = AmbaEnet_PhyWrite(_Idx, mii_data->phy_id, mii_data->reg_num, (UINT32)mii_data->val_in);
            break;
#if defined(ETHDEBUG) || defined(AVBDEBUG) || defined(ETHDEBUG_ISR)
        case SIOCDEVPRIVATE: { /* private ioctl 0x89F0 ~0x89FF */
            extern UINT32 ENETQUIET;
            mii_conf = (struct dcom_mii_config *)ifdc;
            mii_data = &(mii_conf->ifdc_data);
            ENETQUIET = mii_data->phy_id;
            fprintf(stderr, "ENETQUIET %d\n", ENETQUIET);
            error = 0;
            break;
        }
#endif
        }
        break;
    }
    return error;
}
#endif
/*
 * Initial driver entry point.
 */
int
sam_entry(void *dll_hdl,  struct _iopkt_self *iopkt, char *options)
{
    int        instance, single;
    struct device    *dev;
    void        *attach_args;

    /* parse options */

    /* do options imply single? */
    single = 1;

    /* initialize to whatever you want to pass to sam_attach() */
    attach_args = NULL;

    for (instance = 0;;) {
        /* Apply detection criteria */

        /* Found one */
        dev = NULL; /* No Parent */
#if defined(CONFIG_BUILD_SSP_ENET)
        amba_attach_args.dll_hdl = dll_hdl;
        amba_attach_args.iopkt = iopkt;
        amba_attach_args.options = options;
        attach_args = &amba_attach_args;
        if (dev_attach("eth", options, &sam_ca, attach_args,
#else
        if (dev_attach("sam", options, &sam_ca, attach_args,
#endif
                       &single, &dev, NULL) != EOK) {
            break;
        }
        dev->dv_dll_hdl = dll_hdl;
        instance++;


        if (/* done_detection || */ single)
            break;
    }

    if (instance > 0)
        return EOK;

    return ENODEV;
}

int
sam_attach(struct device *parent, struct device *self, void *aux)
{
    int            err;
    struct sam_dev        *sam;
    struct ifnet        *ifp;
    uint8_t            enaddr[ETHER_ADDR_LEN];
    struct qtime_entry    *qtp;

    /* initialization and attach */

    sam = (struct sam_dev *)self;
    ifp = &sam->sc_ec.ec_if;

    sam->sc_iopkt = iopkt_selfp;

    /*
     * CAUTION: As an example we attach to the system timer interrupt.
     * This would be the network hardware interrupt in a real
     * driver. When this sample driver is run it masks and unmasks
     * the system timer interrupt in io-pkt. This may cause problems
     * with other timer calls in other drivers, potentially even
     * leading to a deadlock. It is safe to run by itself in io-pkt.
     */
    qtp = SYSPAGE_ENTRY(qtime);
    sam->sc_irq = qtp->intr;

    if ((err = interrupt_entry_init(&sam->sc_inter, 0, NULL,
                                    IRUPT_PRIO_DEFAULT)) != EOK)
        return err;

    sam->sc_inter.func   = sam_process_interrupt;
    sam->sc_inter.enable = sam_enable_interrupt;
    sam->sc_inter.arg    = sam;

    sam->sc_iid = -1; /* not attached yet */

    /* set capabilities */
#if 0
    ifp->if_capabilities_rx = IFCAP_CSUM_IPv4 | IFCAP_CSUM_TCPv4 | IFCAP_CSUM_UDPv4;
    ifp->if_capabilities_tx = IFCAP_CSUM_IPv4 | IFCAP_CSUM_TCPv4 | IFCAP_CSUM_UDPv4;

    sam->sc_ec.ec_capabilities |= ETHERCAP_JUMBO_MTU;
#endif

#if defined(CONFIG_BUILD_SSP_ENET)
    {
        const UINT8 null_mac[6] = { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U };
        attach_args_t *attach_args = aux;
        attach_args->cfg = &sam->cfg;
        (void)parent;
        sam->cfg.verbose = 0;
        sam->cfg.mtu = 1500;
        sam->cfg.mru = 1500;
        sam->cfg.duplex = 1;
        sam->cfg.media_rate = 1000*1000;
        sam->cfg.flags = NIC_FLAG_MULTICAST;
        amba_parse_options(attach_args);

        if (memcmp(sam->cfg.current_address, null_mac, ETHER_ADDR_LEN)) {
            memcpy(enaddr, sam->cfg.current_address, ETHER_ADDR_LEN);
        } else {
            /* Vmware_xx:xx:xx */
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            srand(ts.tv_sec+ts.tv_nsec);
            sam->cfg.current_address[0] = 0x00;
            sam->cfg.current_address[1] = 0x0c;
            sam->cfg.current_address[2] = 0x29;
            sam->cfg.current_address[3] = rand() % 256;
            sam->cfg.current_address[4] = rand() % 256;
            sam->cfg.current_address[5] = rand() % 256;
            memcpy(enaddr, sam->cfg.current_address, ETHER_ADDR_LEN);
        }

        ifp->if_capabilities_rx = IFCAP_CSUM_IPv4 | IFCAP_CSUM_TCPv4 | IFCAP_CSUM_UDPv4;
        ifp->if_capabilities_tx = IFCAP_CSUM_IPv4 | IFCAP_CSUM_TCPv4 | IFCAP_CSUM_UDPv4;
        sam->sc_ec.ec_capabilities |= ETHERCAP_JUMBO_MTU;
        amba_init(_Idx, sam);
    }
#endif
    ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;

    /* Set callouts */
    ifp->if_ioctl = sam_ioctl;
    ifp->if_start = sam_start;
    ifp->if_init = sam_init;
    ifp->if_stop = sam_stop;
    IFQ_SET_READY(&ifp->if_snd);

    ifp->if_softc = sam;

    /* More callouts for 80211... */

    strcpy(ifp->if_xname, sam->sc_dev.dv_xname);
    if_attach(ifp);

#if !defined(CONFIG_BUILD_SSP_ENET)
    {
        int i;
        for (i = 0; i < ETHER_ADDR_LEN; i++)
            enaddr[i] = i;
    }
#endif
#if 1
    /* Normal ethernet */
    ether_ifattach(ifp, enaddr);
#else
    /* 80211 */
    memcpy(sam->sc_ic.ic_myaddr, enaddr, ETHER_ADDR_LEN);
    ieee80211_ifattach(&sam->sc_ic);
#endif
    sam->sc_sdhook = shutdownhook_establish(sam_shutdown, sam);

    return EOK;
}

void sam_set_multicast(struct sam_dev *sam)
{
#if !defined(CONFIG_BUILD_SSP_ENET)
    struct ethercom            *ec = &sam->sc_ec;
    struct ifnet            *ifp = &ec->ec_if;
    struct ether_multi        *enm;
    struct ether_multistep        step;

    ifp->if_flags &= ~IFF_ALLMULTI;

    ETHER_FIRST_MULTI(step, ec, enm);
    while (enm != NULL) {
        if (memcmp(enm->enm_addrlo, enm->enm_addrhi, ETHER_ADDR_LEN)) {
            /*
             * We must listen to a range of multicast addresses.
             * For now, just accept all multicasts, rather than
             * trying to filter out the range.
             * At this time, the only use of address ranges is
             * for IP multicast routing.
             */
            ifp->if_flags |= IFF_ALLMULTI;
            break;
        }
        /* Single address */
        printf("Add %02x:%02x:%02x:%02x:%02x:%02x to mcast filter\n",
               enm->enm_addrlo[0],enm->enm_addrlo[1],
               enm->enm_addrlo[2],enm->enm_addrlo[3],
               enm->enm_addrlo[4],enm->enm_addrlo[5]);
        ETHER_NEXT_MULTI(step, enm);    //Case 00181342 patch
    }

    if ((ifp->if_flags & IFF_ALLMULTI) != 0) {
        printf("Enable multicast promiscuous\n");
    } else {
        printf("Disable multicast promiscuous\n");
    }
#else
    struct ethercom *ec = &sam->sc_ec;
    struct ifnet *ifp = &ec->ec_if;
    AMBA_ENET_REG_s *pEnetReg = pAmbaCSL_EnetReg[_Idx];

    ifp->if_flags |= IFF_ALLMULTI;
    sam->cfg.flags = NIC_FLAG_MULTICAST;

    /* Pass All Multicast */
    AmbaCSL_EnetMacSetHMCPM(pEnetReg, 0U, 1U);
    /* offline hashed mcast for ptp */
    //AmbaCSL_EnetMacSetHashLo(pEnetReg, 0x50100003U);
    //AmbaCSL_EnetMacSetHashHi(pEnetReg, 0x01040001U);
#endif
}

int
sam_init(struct ifnet *ifp)
{
    int        ret;
    struct sam_dev    *sam;

    /*
     * - enable hardware.
     *   - look at ifp->if_capenable_[rx/tx]
     *   - enable promiscuous / multicast filter.
     * - attach to interrupt.
     */

    sam = ifp->if_softc;

    if(memcmp(sam->cfg.current_address, LLADDR(ifp->if_sadl), ifp->if_addrlen)) {
        memcpy(sam->cfg.current_address, LLADDR(ifp->if_sadl), ifp->if_addrlen);
        /* update the hardware */
    }

#if defined(CONFIG_BUILD_SSP_ENET)
    if(memcmp(sam->cfg.permanent_address, LLADDR(ifp->if_sadl), ifp->if_addrlen)) {
        memcpy(sam->cfg.permanent_address, LLADDR(ifp->if_sadl), ifp->if_addrlen);
    }
    (void)AmbaEnet_IfUp(_Idx, (UINT32 *)&ret);
    if_link_state_change(ifp, LINK_STATE_UP);

    if (ifp->if_capenable_tx != 0) {
        extern AMBA_ENET_CONFIG_s EnetConfig[ENET_INSTANCES];
        AMBA_ENET_CONFIG_s *pEnetConfig = &EnetConfig[_Idx];
        AMBA_ENET_DES_s *pDes = pEnetConfig->pDes;
        AMBA_ENET_TDES_s *pTDES;
        UINT32 CIC;

        if (ifp->if_capenable_tx == IFCAP_CSUM_IPv4) {
            CIC = 0x1; /* offload ip */
        } else {
            CIC = 0x3; /* offload udp/tcp, ping will not WORK */
            fprintf(stderr, "WARN: ping will not WORK\n");
        }

        for (UINT32 i = 0; i < pDes->TDESCnt; i++) {
            pTDES = &pDes->pTDES[i];
            AmbaCSL_EnetTDESSetCIC(pTDES, CIC);
        }
    }
#endif
    if (sam->sc_iid == -1) {
#if defined(CONFIG_BUILD_SSP_ENET)
        if (_Idx == 0U) {
            sam->sc_irq = ETH_INT_VEC;
        }
#if defined(ETH1_INT_VEC)
        else {
            sam->sc_irq = ETH1_INT_VEC;
        }
#endif
#endif
        if ((ret = InterruptAttach_r(sam->sc_irq, sam_isr,
                                     sam, sizeof(*sam), _NTO_INTR_FLAGS_TRK_MSK)) < 0) {
            return -ret;
        }
        sam->sc_iid = ret;
    }

    sam_set_multicast(sam);
    ifp->if_flags |= IFF_RUNNING;

    return EOK;
}

void
sam_stop(struct ifnet *ifp, int disable)
{
    struct sam_dev    *sam;

    /*
     * - Cancel any pending io
     * - Clear any interrupt source registers
     * - Clear any interrupt pending registers
     * - Release any queued transmit buffers.
     */

    sam = ifp->if_softc;

    if (disable) {
        if (sam->sc_iid != -1) {
            InterruptDetach(sam->sc_iid);
            sam->sc_iid = -1;
        }
        /* rxdrain */
    }

    ifp->if_flags &= ~IFF_RUNNING;
}

void
sam_start(struct ifnet *ifp)
{
    struct sam_dev        *sam;
    struct mbuf        *m;
    struct nw_work_thread    *wtp;

    sam = ifp->if_softc;
    wtp = WTP;


    for (;;) {
        IFQ_POLL(&ifp->if_snd, m);
        if (m == NULL)
            break;

        /*
         * Can look at m to see if you have the resources
         * to transmit it.
         */

        IFQ_DEQUEUE(&ifp->if_snd, m);
#if !defined(CONFIG_BUILD_SSP_ENET)
        /* You're now committed to transmitting it */
        if (sam->cfg.verbose) {
            printf("Packet sent\n");
        }
        m_freem(m);

        ifp->if_opackets++;  // for ifconfig -v
        // or if error:  ifp->if_oerrors++;
#else
        //NW_SIGLOCK_P(&ifp->if_snd_ex, iopkt_selfp, wtp);
        //ifp->if_flags_tx |= IFF_OACTIVE;
        //ifp->if_flags_tx &= ~IFF_OACTIVE;
        //amba_tx(_Idx, m, TX_COPY);

        amba_tx(_Idx, amba_mbuf_defrag(m, ifp), ifp, TX_COPY);
        (void)sam;
#endif
    }

    NW_SIGUNLOCK_P(&ifp->if_snd_ex, iopkt_selfp, wtp);
}

int
sam_ioctl(struct ifnet *ifp, unsigned long cmd, caddr_t data)
{
    struct sam_dev    *sam;
    int        error;

    sam = ifp->if_softc;
    error = 0;

#if defined(CONFIG_BUILD_SSP_ENET)
    error = amba_ioctl(ifp, cmd, data);
    if (error == 0) {
        return error;
    }
#endif
    switch (cmd) {
    default:
        error = ether_ioctl(ifp, cmd, data);
        if (error == ENETRESET) {
            /*
             * Multicast list has changed; set the
             * hardware filter accordingly.
             */
            if ((ifp->if_flags & IFF_RUNNING) == 0) {
                /*
                 * Interface is currently down: sam_init()
                 * will call sam_set_multicast() so
                 * nothing to do
                 */
            } else {
                /*
                 * interface is up, recalculate and
                 * reprogram the hardware.
                 */
                sam_set_multicast(sam);
            }
            error = 0;
        }
        break;
    }

#if defined(CONFIG_BUILD_SSP_ENET)
    if (sam->cfg.verbose) {
        if (((error != 0)) && (ifp->if_flags & IFF_RUNNING)) {
            fprintf(stderr, "ioctl 0x%x returned %d\n", (UINT32)cmd, error);
        }
    }
#endif
    return error;
}

int
sam_detach(struct device *dev, int flags)
{
    struct sam_dev    *sam;
    struct ifnet    *ifp;

    /*
     * Clean up everything.
     *
     * The interface is going away but io-pkt is staying up.
     */
    sam = (struct sam_dev *)dev;
    ifp = &sam->sc_ec.ec_if;

    sam_stop(ifp, 1);
#if 1
    ether_ifdetach(ifp);
#else
    ieee80211_ifdetach(&sam->sc_ic);
#endif

    if_detach(ifp);

    shutdownhook_disestablish(sam->sc_sdhook);
#if defined(CONFIG_BUILD_SSP_ENET)
    (void)flags;
    amba_deinit(_Idx);
#endif
    return EOK;
}

void
sam_shutdown(void *arg)
{
    struct sam_dev    *sam;

    /* All of io-pkt is going away.  Just quiet hardware. */

    sam = arg;

    sam_stop(&sam->sc_ec.ec_if, 1);
}

#ifndef HW_MASK
const struct sigevent *
sam_isr(void *arg, int iid)
{
    struct sam_dev        *sam;
    struct _iopkt_inter    *ient;

    sam = arg;
    ient = &sam->sc_inter;

    /*
     * Close window where this is referenced in sam_enable_interrupt().
     * We may get an interrupt, return a sigevent and have another
     * thread start processing on SMP before the InterruptAttach()
     * has returned.
     */
    sam->sc_iid = iid;

    InterruptMask(sam->sc_irq, iid);
#if defined(CONFIG_BUILD_SSP_ENET)
    amba_isr(sam->sc_irq, 0U);
#endif

    return interrupt_queue(sam->sc_iopkt, ient);
}
#else
const struct sigevent *
sam_isr(void *arg, int iid)
{
    struct sam_dev        *sam;
    struct _iopkt_self    *iopkt;
    const struct sigevent    *evp;
    struct inter_thread    *itp;

    sam = arg;
    iopkt = sam->sc_iopkt;
    evp = NULL;

#ifdef READ_CAUSE_IN_ISR
    /*
     * Trade offs.
     * - Doing this here means another register read across the bus.
     * - If not sharing interrupts, this boils down to exactly the
     *   same amount of work but doing more of it in the isr.
     * - If sharing interupts, can short circuit some work in the
     *   stack here.
     * - Maybe trade off is to only do it if we're detecting
     *   spurious interrupts which should happen under heavy
     *   shared interrupt load?
     */
#ifdef READ_CAUSE_ONLY_ON_SPURIOUS
    if (ient->spurrious) {
#endif
        if (ient->on_list == 0 &&
            (sam->sc_intr_cause = i82544->reg[I82544_ICR]) == 0) {
            return NULL; /* Not ours */
        }
        sam->sc_flag |= CAUSE_VALID;
#ifdef READ_CAUSE_ONLY_ON_SPURIOUS
    }
#endif
#endif

    /*
     * We have to make sure the interrupt is masked regardless
     * of our on_list status.  This is because of a window where
     * a shared (spurious) interrupt comes after on_list
     * is knocked down but before the enable() callout is made.
     * If enable() then happened to run after we masked, we
     * could end up on the list without the interrupt masked
     * which would cause the kernel more than a little grief
     * if one of our real interrupts then came in.
     *
     * This window doesn't exist when using kermask since the
     * interrupt isn't unmasked until all the enable()s run
     * (mask count is tracked by kernel).
     */

    /*
     * If this was controling real hardware, mask of
     * interrupts here. eg from i82544 driver:
     */
    i82544->reg[I82544_IMC] = 0xffffffff;

    return interrupt_queue(sam->sc_iopkt, ient);
}
#endif
#if !defined(CONFIG_BUILD_SSP_ENET)
int
sam_process_interrupt(void *arg, struct nw_work_thread *wtp)
{
    struct sam_dev        *sam;
    struct mbuf            *m;
    struct ifnet        *ifp;
    struct ether_header    *eh;

    sam = arg;
    ifp = &sam->sc_ec.ec_if;


    if ((sam->sc_intr_cnt++ % 1000) == 0) {
        /* Send a packet up */
        m = m_getcl_wtp(M_DONTWAIT, MT_DATA, M_PKTHDR, wtp);

        if (!m) {
            ifp->if_ierrors++;  // for ifconfig -v
            return 1;
        }

        m->m_pkthdr.len = m->m_len = sizeof(*eh);

        // ip_input() needs this
        m->m_pkthdr.rcvif = ifp;

        // dummy up a broadcasted IP packet for testing
        eh = mtod(m, struct ether_header *);
        eh->ether_type = ntohs(ETHERTYPE_IP);
        memcpy(eh->ether_dhost, etherbroadcastaddr, ETHER_ADDR_LEN);

        ifp->if_ipackets++; // for ifconfig -v

        (*ifp->if_input)(ifp, m);

        printf("sam_process_interrupt %d\n", sam->sc_intr_cnt);
    }

    /*
     * return of 1 means were done.
     *
     * If we notice we're taking a long time (eg. processed
     * half our rx descriptors) we could early out with a
     * return of 0 which lets other interrupts be processed
     * without calling our interrupt_enable func.  This
     * func will be called again later.
     */
    return 1;
}
#endif
#ifndef HW_MASK
int
sam_enable_interrupt(void *arg)
{
    struct sam_dev    *sam;

    sam = arg;
    InterruptUnmask(sam->sc_irq, sam->sc_iid);

    return 1;
}
#else
int
sam_enable_interrupt(void *arg)
{
    struct sam_dev    *sam;

    sam = arg;
    /* eg from i82544 driver */

    i82544->reg[I82544_IMS] = i82544->intrmask;

    return 1;
}
#endif



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
