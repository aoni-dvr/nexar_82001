// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
*  Copyright (c) 2021 Ambarella International LP
*/

#ifndef VISORC_CV25_I_FILE
#define VISORC_CV25_I_FILE

#ifdef CHIP_CV25
#include "os_kernel.h"
#include "ambacv_kal.h"
#include "cache_kernel.h"
#include "visorc_kernel.h"

#define VISORC_BASE     0xed000000U
#define VISORC_SIZE     0x01000000U

#define VISORC_SYNC     127
#define IDSPORC_SYNC     12

/* for VP-ORC */
#define VP_TRESET       0x010000U
#define VP_CACHE        0x010004U
#define VP_RESET_PC     0x010008U

/* for SMEM */
#define SYNC_CTR_BASE   0x05F000U

/* for RCT */
#define CLUSTER_SRESET  0x080228U

/* For chicken bits */
#define VP_CROP_DISABLE 0x0820028U

/* For CORC current time */
#define CORC_TIME       0x030044U

/* for L2-cache */
#define REMAP_R0_START  0x0a0040U
#define REMAP_R0_LIMIT  0x0a0044U
#define REMAP_R0_RBASE  0x0a0048U
#define REMAP_R1_START  0x0a0050U
#define REMAP_R1_LIMIT  0x0a0054U
#define REMAP_R1_RBASE  0x0a0058U
#define REMAP_R2_START  0x0a0060U
#define REMAP_R2_LIMIT  0x0a0064U
#define REMAP_R2_RBASE  0x0a0068U
#define REMAP_R3_START  0x0a0070U
#define REMAP_R3_LIMIT  0x0a0074U
#define REMAP_R3_RBASE  0x0a0078U

static void *visorc_io_base = NULL;

static void *visorc_reg(uint32_t index)
{
    uint8_t *src;
    void *dst;

    krn_typecast(&src,&visorc_io_base);
    dst = &src[index];

    return dst;
}

uint32_t krn_visorc_init(void)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_all_mem_t *mem_all;

    if (visorc_io_base == NULL) {
        visorc_io_base = krn_ioremap(VISORC_BASE, VISORC_SIZE);
        if (visorc_io_base == NULL) {
            krn_printU5("[ERROR] krn_visorc_init() : krn_ioremap VISORC registers failed\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_MAP_REG_FAIL;
        }
    }

    /* set up sod/vp binary memory regions */
    mem_all = krn_ambacv_get_mem();
    mem_all->sod.base  = 0;
    mem_all->sod.size  = 0;
    mem_all->vp.base   = (uint64_t)(mem_all->all.base + VP_MEM_REGION_OFFSET);
    mem_all->vp.size   = (uint64_t)(VP_MEM_REGION_SIZE);
    mem_all->debug_port.base = VISORC_BASE;
    mem_all->debug_port.size = VISORC_SIZE;
    krn_module_printU5("[AMBACV] TARGET CHIP   :   CV25 \n",0U,0U,0U,0U,0U);
    return retcode;
}

void krn_visorc_shutdown(void)
{
    if (visorc_io_base != NULL) {
        krn_iounmap(visorc_io_base, VISORC_SIZE);
        visorc_io_base = NULL;
    }
}

uint32_t krn_visorc_finalize_loading(const ambacv_mem_t* arg)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) arg;
    if(arg == NULL) {
        krn_printU5("[ERROR] krn_visorc_finalize_loading() : arg == NULL", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        if(arg->vp.base != 0U) {
            retcode = krn_cache_clean(krn_c2v(arg->vp.base), arg->vp.size);
            krn_module_printU5("\tVP:   [0x%llx ++ 0x%llx]\n",
                               krn_c2p(arg->vp.base), arg->vp.size, 0U, 0U, 0U);
        } else {
            retcode = ERR_DRV_SCHDR_VISORC_BIN_LOAD_START_INVALID;
        }
    }

    return retcode;
}

uint32_t krn_visorc_start(uint32_t flag, visorc_init_params_t *sysconfig)
{
    const ambacv_all_mem_t *mem_all;
    uint32_t retcode = ERRCODE_NONE;
    uint32_t i;
    uint32_t reg_val;
    uint32_t offset;

    (void) flag;
    if(sysconfig == NULL) {
        retcode = ERR_DRV_SCHDR_VISORC_INIT_INVALID;
        krn_printU5("[ERROR] krn_visorc_start() : sysconfig == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        mem_all = krn_ambacv_get_mem();
        /* soft-reset all clusters*/
        /* have to skip bit[8] since it resets audio tick counter as well */
        krn_writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));
        krn_writel_relaxed(0x00000600U, visorc_reg(CLUSTER_SRESET));
        krn_writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));

        /*-= Reset chicken bits =-*/
        krn_writel_relaxed(0x00100010U, visorc_reg(VP_CROP_DISABLE));

        /* setup remap registers */
        krn_writel_relaxed(0x00000000U, visorc_reg(REMAP_R0_START));
        krn_writel_relaxed(0x00600000U, visorc_reg(REMAP_R0_LIMIT));
        krn_writel_relaxed((uint32_t)(mem_all->all.base) | 0x1U, visorc_reg(REMAP_R0_RBASE));

        krn_writel_relaxed(HOTLINK_MEM_REGION_VADDR_BASE, visorc_reg(REMAP_R1_START));
        krn_writel_relaxed(HOTLINK_MEM_REGION_VADDR_BASE + (uint32_t)(mem_all->hotlink_region.size), visorc_reg(REMAP_R1_LIMIT));
        krn_writel_relaxed((uint32_t)(mem_all->hotlink_region.base) | 0x1U, visorc_reg(REMAP_R1_RBASE));

        krn_writel_relaxed(0x00000000U, visorc_reg(REMAP_R2_START));
        krn_writel_relaxed(0x00000000U, visorc_reg(REMAP_R2_LIMIT));
        krn_writel_relaxed(0x00000000U, visorc_reg(REMAP_R2_RBASE));

        krn_writel_relaxed(0x00000000U, visorc_reg(REMAP_R3_START));
        krn_writel_relaxed(0x00000000U, visorc_reg(REMAP_R3_LIMIT));
        krn_writel_relaxed(0x00000000U, visorc_reg(REMAP_R3_RBASE));

        reg_val = krn_readl_relaxed(visorc_reg(REMAP_R0_START));
        if (reg_val != 0x00000000U) {
            const void *ptr;
            uint64_t reg = 0UL;

            ptr = visorc_reg(REMAP_R0_START);
            krn_typecast(&reg,&ptr);
            krn_printU5("[ERROR] krn_visorc_start() : setup register(0x%x) failed, write=0x%x read=0x%x", reg, 0x0U, reg_val, 0U, 0U);
            retcode = ERR_DRV_SCHDR_VISORC_HW_UNAVAILABLE;
        }

        if(retcode == ERRCODE_NONE) {
            /* setup reset vector */
            offset = (uint32_t)(mem_all->vp.base - mem_all->all.base);
            krn_writel_relaxed(offset, visorc_reg(VP_RESET_PC));

            /* invalidate I/D cache */
            krn_writel_relaxed(0x00000003U, visorc_reg(VP_CACHE));

            /* reset orc-sod/vp */
            krn_writel_relaxed(0x00000F00U, visorc_reg(VP_TRESET));

            /* reset cv-related sync counters */
            for (i = 0x70U; i < 0x80U; i++) {
                krn_orc_send_sync_count(i, 0x1020U);
            }
            for (i = 0x70U; i < 0x80U; i++) {
                krn_orc_send_sync_count(i, 0x0000U);
            }
            for (i = 0x70U; i < 0x80U; i++) {
                krn_orc_send_sync_count(i, 0x1020U);
            }
            for (i = 0x70U; i < 0x80U; i++) {
                krn_orc_send_sync_count(i, 0x0000U);
            }

            /* To be consistent with CV2 although it is not necessary. */
            sysconfig->arm_base_timestamp = krn_readl_relaxed(visorc_reg(CORC_TIME));
            retcode = krn_cache_clean(&sysconfig->arm_base_timestamp, sizeof(sysconfig->arm_base_timestamp));
            /* release sod/vp core */
            krn_writel_relaxed(0x0000000FU, visorc_reg(VP_TRESET));
        }
    }

    return retcode;
}

void krn_orc_send_sync_count(uint32_t id, uint32_t val)
{
    krn_writel_relaxed(val, visorc_reg(SYNC_CTR_BASE + (4U * id)));
}

void krn_visorc_kick(void)
{
    krn_orc_send_sync_count(VISORC_SYNC, 0x1400U);
}

void krn_visorc_armidsp_async_queue_kick(void)
{
    krn_orc_send_sync_count(IDSPORC_SYNC, 0x1001U);
}

void krn_visorc_armidsp_queue_kick(void)
{
    krn_orc_send_sync_count(VISORC_SYNC, 0x1400U);
}

void krn_visorc_armvin_queue_kick(void)
{
    krn_orc_send_sync_count(VISORC_SYNC, 0x1400U);
}

void krn_visorc_softreset(uint32_t unit_id)
{
    (void)unit_id;
    krn_module_printU5("[AMBACV]: softreset %d\n", unit_id, 0U, 0U, 0U, 0U);
    krn_writel_relaxed(0x0000000U, visorc_reg(CLUSTER_SRESET));
    krn_writel_relaxed(0x0000600U, visorc_reg(CLUSTER_SRESET));
    krn_writel_relaxed(0x0000000U, visorc_reg(CLUSTER_SRESET));

    /*-= Reset chicken bits =-*/
    krn_writel_relaxed(0x00100010U, visorc_reg(VP_CROP_DISABLE));
}

uint32_t krn_visorc_get_curtime(void)
{
    return krn_readl_relaxed(visorc_reg(CORC_TIME));
} /* krn_visorc_get_curtime() */

#endif //CHIP_CV25

#endif /* ?VISORC_CV25_I_FILE */
