// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
*  Copyright (c) 2021 Ambarella International LP
*/

#ifndef VISORC_CV1_I_FILE
#define VISORC_CV1_I_FILE

#ifdef CHIP_CV1

#include <stddef.h>
#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <AmbaINT.h>
#include <AmbaINT_Def.h>
#include "os_api.h"
#include "schdr.h"

#define VP0_IRQ         AMBA_INT_SPI_ID138_VP0_EXCEPTION
#define VP1_IRQ         AMBA_INT_SPI_ID139_VP1_EXCEPTION

#define VISORC_BASE     0xed000000

#define SOD_TRESET      0x000000
#define SOD_CACHE       0x000004
#define SOD_RESET_PC    0x000008
#define VP_TRESET       0x010000
#define VP_CACHE        0x010004
#define VP_RESET_PC     0x010008
#define VP_CTX_INFO_0   0x010030
#define VP_CTX_INFO_1   0x010034
#define VP_CTX_INFO_2   0x010038
#define VP_CTX_INFO_3   0x01003C
#define CLUSTER_SRESET  0x080228

#define VP0_INT_STATUS  0x82000C
#define VP0_INT_CLEAR   0x820010
#define VP0_INT_ENABLE  0x820014
#define VP0_DPAR        0x820028
#define VP0_CTRL        0x820064
#define VP0_RESET_FLAG  0xa00020

#define VP1_INT_STATUS  0x92000C
#define VP1_INT_CLEAR   0x920010
#define VP1_INT_ENABLE  0x920014
#define VP1_DPAR        0x920028
#define VP1_CTRL        0x920064
#define VP1_RESET_FLAG  0xe00020

#define visorc_reg(x)   (VISORC_BASE + (x))

#define SYNC_CNT_ADDR   0x47c00
#define syncnt_reg(x)   (VISORC_BASE + SYNC_CNT_ADDR + 4*(x))

#define DEBUG_ISR
#ifdef  DEBUG_ISR
#define ILOG(fmt, val)  console_printU5(fmt, val, 0, 0, 0, 0)
#else
#define ILOG(...)
#endif

static uint32_t readl(uint32_t reg)
{
    return *(volatile uint32_t *)reg;
}

static void writel(uint32_t val, uint32_t reg)
{
    *(volatile uint32_t*)reg = val;
}

static void writel_relaxed(uint32_t val, uint32_t reg)
{
    *(volatile uint32_t*)reg = val;
}

static void vp0_isr(int32_t id)
{
    uint32_t value;

    value = readl(visorc_reg(VP0_INT_STATUS));
    ILOG("ENTER VP0 ISR, status 0x%X", value);

    value = readl(visorc_reg(VP_CTX_INFO_0));
    ILOG("CONTEXT SWTICH INFO_0 get: 0x%x", value);
    if (value & 0x10) {
        writel(0x00000000, visorc_reg(CLUSTER_SRESET));
        writel(0x00001400, visorc_reg(CLUSTER_SRESET));
        writel(0x00000000, visorc_reg(CLUSTER_SRESET));
        writel(0xA5AFE5E7, visorc_reg(VP0_RESET_FLAG));

        value &= ~0x10;
        writel(value, visorc_reg(VP_CTX_INFO_0));
        ILOG("CONTEXT SWTICH INFO_0 set: 0x%x", value);
        writel(0x00000002, visorc_reg(VP0_CTRL));
    }

    value = (readl(visorc_reg(VP0_INT_CLEAR))) & ~1;
    writel(value, visorc_reg(VP0_INT_CLEAR));
    ILOG("VP0 ISR, clear 0x%X", value);
}

static void vp1_isr(int32_t irq)
{
    uint32_t value;

    value = readl(visorc_reg(VP1_INT_STATUS));
    ILOG("ENTER VP1 ISR, status 0x%X", value);

    value = readl(visorc_reg(VP_CTX_INFO_2));
    ILOG("CONTEXT SWTICH INFO_2 get: 0x%x", value);
    if (value & 0x10) {
        writel(0x00000000, visorc_reg(CLUSTER_SRESET));
        writel(0x00002800, visorc_reg(CLUSTER_SRESET));
        writel(0x00000000, visorc_reg(CLUSTER_SRESET));
        writel(0xA5AFE5E7, visorc_reg(VP1_RESET_FLAG));

        value &= ~0x10;
        writel(value, visorc_reg(VP_CTX_INFO_2));
        ILOG("CONTEXT SWTICH INFO_2 set: 0x%x", value);
        writel(0x00000002, visorc_reg(VP1_CTRL));
    }

    value = (readl(visorc_reg(VP1_INT_CLEAR))) & ~1;
    writel(value, visorc_reg(VP1_INT_CLEAR));
    ILOG("VP1 ISR, clear 0x%X", value);
}

int32_t visorc_start(uint32_t base, uint32_t flag)
{
    int32_t i;
    uint32_t reg_val
    AMBA_INT_CONFIG_s irq_config;

    /* soft-reset all clusters*/
    /* have to skip bit[9] since it resets audio tick counter as well */
    writel_relaxed(0x00000000, visorc_reg(CLUSTER_SRESET));
    writel_relaxed(0x00003D80, visorc_reg(CLUSTER_SRESET));
    writel_relaxed(0x00000000, visorc_reg(CLUSTER_SRESET));

    /* setup remap registers */
    writel_relaxed(0x00000000, visorc_reg(0xa0040));
    writel_relaxed(0x00600000, visorc_reg(0xa0044));
    writel_relaxed(base | 0x1, visorc_reg(0xa0048));
    writel_relaxed(0x00000000, visorc_reg(0xa0058));
    writel_relaxed(0x00000000, visorc_reg(0xa0068));
    writel_relaxed(0x00000000, visorc_reg(0xa0078));

    reg_val = readl_relaxed(visorc_reg(0xa0040));
    if (reg_val != 0x00000000) {
        console_printU5("[ERROR] visorc_start(): setup register(0x%x) failed, write=0x%x read=0x%x", visorc_reg(0xa0040), 0x00000000, reg_val, 0, 0);
        return -1;
    }

    /* setup reset vector */
    writel_relaxed(0x200000, visorc_reg(SOD_RESET_PC));
    writel_relaxed(0x400000, visorc_reg(VP_RESET_PC));

    /* invalidate I-cache */
    writel_relaxed(0x00000001, visorc_reg(SOD_CACHE));
    writel_relaxed(0x00000001, visorc_reg(VP_CACHE));
    writel_relaxed(0x00000000, visorc_reg(SOD_CACHE));
    writel_relaxed(0x00000000, visorc_reg(VP_CACHE));

    /* set VP dram_fromat to 2 */
    writel_relaxed(0x00000010, visorc_reg(VP0_DPAR));
    writel_relaxed(0x00000010, visorc_reg(VP1_DPAR));

    if (flag) {
        irq_config.IntSense   = AMBA_INT_RISING_EDGE_TRIGGER;
        irq_config.IntType    = AMBA_INT_IRQ;
        irq_config.TargetCore = 0x01;

        irq_config.IsrEntry = vp0_isr;
        AmbaINT_IsrHook(VP0_IRQ, &irq_config);
        AmbaINT_SetCpuTarget(VP0_IRQ, 1);
        AmbaINT_IntEnable(VP0_IRQ);

        irq_config.IsrEntry = vp1_isr;
        AmbaINT_IsrHook(VP1_IRQ, &irq_config);
        AmbaINT_SetCpuTarget(VP1_IRQ, 1);
        AmbaINT_IntEnable(VP1_IRQ);

        /* enable VMEM write from VIS-ORC */
        writel_relaxed(0x00000002, visorc_reg(VP0_CTRL));
        writel_relaxed(0x00000002, visorc_reg(VP1_CTRL));
    }

    /* reset orc-sod/vp */
    writel_relaxed(0x00000F00, visorc_reg(SOD_TRESET));
    writel_relaxed(0x00000F00, visorc_reg(VP_TRESET));

    /* reset all sync counters */
    for (i = 0; i < 0x40; i++) {
        if (1 <= i && i <= 49) continue;
        writel_relaxed(0x1020, syncnt_reg(i));
    }
    for (i = 0; i < 0x40; i++) {
        if (1 <= i && i <= 49) continue;
        writel_relaxed(0x0000, syncnt_reg(i));
    }
    for (i = 0; i < 0x40; i++) {
        if (1 <= i && i <= 49) continue;
        writel_relaxed(0x1020, syncnt_reg(i));
    }
    for (i = 0; i < 0x40; i++) {
        if (1 <= i && i <= 49) continue;
        writel_relaxed(0x0000, syncnt_reg(i));
    }

    writel_relaxed(0x00000E0F, visorc_reg(VP_TRESET));
    writel_relaxed(0x0000000E, visorc_reg(VP_TRESET));
    writel_relaxed(0x00000E0F, visorc_reg(SOD_TRESET));
    writel_relaxed(0x0000000E, visorc_reg(SOD_TRESET));

    /* release sod/vp core */
    writel_relaxed(0x0000000F, visorc_reg(SOD_TRESET));
    writel_relaxed(0x0000000F, visorc_reg(VP_TRESET));

    return 0;
}

void visorc_stop(void)
{
    AmbaINT_IntDisable(VP0_IRQ);
    AmbaINT_IntDisable(VP1_IRQ);

    /* disable VMEM write from VIS-ORC */
    writel_relaxed(0x00000000, visorc_reg(VP0_CTRL));
    writel_relaxed(0x00000000, visorc_reg(VP1_CTRL));
}

void visorc_send_sync_count(int32_t id)
{
    writel_relaxed(0x1400, syncnt_reg(id));
}

uint32_t  visorc_get_curtime(void)
{
    return krn_readl_relaxed(visorc_reg(0x30044U));
} /* visorc_get_curtime() */

#endif //CHIP_CV1

#endif /* ?VISORC_CV1_I_FILE */
