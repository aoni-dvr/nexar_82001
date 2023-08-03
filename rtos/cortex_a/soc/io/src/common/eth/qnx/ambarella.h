/**
 * @file ambarella.h
 *
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @details qnx wrapper layer header file
 *
 */

#include <stdint.h>
#include <string.h>
#include <semaphore.h>

#include <pthread.h>
#include <sched.h>
#include <atomic.h>
#include <bpfilter.h>
#include <hw/inout.h>
#include <sys/mman.h>
#include <sys/cache.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <sys/sockio.h>
#include <sys/neutrino.h>
#include <sys/mbuf.h>
#include <net/ifdrvcom.h>
#include <net/bpf.h>
#include <netdrvr/nic_mutex.h>
#include <netdrvr/nicsupport.h>
#include <netdrvr/smmu.h>
#include <netdrvr/ptp.h>
#include <netdrvr/mdi.h>
#include <libfdt.h>
#include "AmbaFDT.h"

//#define ETHDEBUG
//#define ETHDEBUG_ISR
//#define ETHDEBUG_DUMP
//#define QNX_ETH_TSK

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define ETH0_REG_BASE           0x20e000e000UL
#define ETH1_REG_BASE           0x20e0022000UL
#else
#define ETH0_REG_BASE           0xe000e000U
#define ETH1_REG_BASE           0xe0032000U
#endif
#define BOARD_ETH_TX_FRAMES     32U
#define BOARD_ETH_RX_FRAMES     64U
#define TX_CHECKSUM_OFFLOAD     0x0U
#define TX_COPY                 0U
#define RX_COPY                 0U

#if !defined(SIOCGMIIREG)
#define SIOCGMIIREG    0x8948          /* Read MII PHY register */
#endif

#if !defined(SIOCSMIIREG)
#define SIOCSMIIREG    0x8949          /* Write MII PHY register. */
#endif

#if !defined(SIOCDEVPRIVATE)
#define SIOCDEVPRIVATE 0x89F0          /* private ioctl 0x89F0 ~0x89FF */
#endif

struct mii_ioctl_data {
    uint16_t        phy_id;
    uint16_t        reg_num;
    uint16_t        val_in;
    uint16_t        val_out;
};

struct dcom_mii_config {
    char                    ifdc_name[IFNAMSIZ];
    unsigned long           ifdc_cmd;
    struct mii_ioctl_data   ifdc_data;
};

#if !defined(CONFIG_BUILD_SSP_ENET)
#define CONFIG_BUILD_SSP_ENET
#endif
#if !defined(CONFIG_BUILD_COMMON_SERVICE_NET)
#define CONFIG_BUILD_COMMON_SERVICE_NET
#endif
#if !defined(AMBA_INT_SPI_ID90_ENET_SBD)
#define AMBA_INT_SPI_ID90_ENET_SBD      90
#endif
#if !defined(AMBA_INT_SPI_ID92_ENET_SBD1)
#define AMBA_INT_SPI_ID92_ENET_SBD1     92
#endif
#if !defined(AMBA_INT_SPI_ID096_ENET0_SBD)
#define AMBA_INT_SPI_ID096_ENET0_SBD    96
#endif
#if !defined(AMBA_INT_SPI_ID165_ENET1_SBD)
#define AMBA_INT_SPI_ID165_ENET1_SBD    165
#endif
#if !defined(ETH_ERR_BASE)
#define ETH_ERR_BASE                    0x00130000U
#endif
#if !defined(CACHE_LINE_SIZE)
#define CACHE_LINE_SIZE                 64U
#endif
#if !defined(RCT_BASE)
#define RCT_BASE                        0xed080000U
#endif
#if !defined(AHB_SCRATCHPAD_BASE)
#if defined(CONFIG_SOC_CV2)
#define AHB_SCRATCHPAD_BASE             0xe8001000U
#else
#define AHB_SCRATCHPAD_BASE             0xe0022000U
#endif
#endif
#if defined(CONFIG_SOC_CV2FS)
#define ETH_INT_VEC                     AMBA_INT_SPI_ID096_ENET0_SBD
#define ETH1_INT_VEC                    AMBA_INT_SPI_ID165_ENET1_SBD
#else
#define ETH_INT_VEC                     AMBA_INT_SPI_ID90_ENET_SBD
#endif

typedef int8_t    INT8;
typedef int16_t   INT16;
typedef int32_t   INT32;
typedef int64_t   INT64;

typedef uint8_t   UINT8;
typedef uint16_t  UINT16;
typedef uint32_t  UINT32;
typedef uint64_t  UINT64;
typedef uint64_t  ULONG;

typedef size_t    SIZE_t;

#define AmbaWrap_memcpy memcpy
#define AmbaWrap_memset memset

#define AmbaPrint_PrintUInt5(fmt, p1, p2, p3, p4, p5) fprintf(stderr, fmt"\n", p1, p2, p3, p4, p5)
#define AmbaPrint_PrintStr5(fmt, p1, p2, p3, p4, p5) fprintf(stderr, fmt"\n", p1, p2, p3, p4, p5)
#define AmbaPrint_ModulePrintUInt5(m, fmt, p1, p2, p3, p4, p5) fprintf(stderr, fmt"\n", p1, p2, p3, p4, p5)
#define AmbaPrint_ModulePrintStr5(m, fmt, p1, p2, p3, p4, p5) fprintf(stderr, fmt"\n", p1, p2, p3, p4, p5)
//#define AmbaMisra_TypeCast32(p1, p2) fprintf(stderr, "err: %s calls AmbaMisra_TypeCast32", __func__)

static inline UINT32 AmbaWrap_memcmp(const void *p1, const void *p2, SIZE_t n, INT32 *pV)
{
    *pV = memcmp(p1, p2, n);

    return 0;
}
