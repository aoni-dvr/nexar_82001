#ifndef __AMBARELLA_DRAM_H__
#define __AMBARELLA_DRAM_H__

#if defined(CONFIG_ATF_AMBALINK)
typedef struct {
    uint32_t  ClientRequestStatis[32];    /* Statistics for each client's number of requests */
    uint32_t  ClientBurstStatis[32];      /* Statistics for each client's number of bursts */
    uint32_t  ClientMaskWriteStatis[32];  /* Statistics for each client's number of masked write bursts */
} AMBA_DRAMC_STATIS_s;
#endif

#endif

