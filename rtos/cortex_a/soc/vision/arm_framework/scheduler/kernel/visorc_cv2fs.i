// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
*  Copyright (c) 2021 Ambarella International LP
*/

#ifndef VISORC_CV2FS_I_FILE
#define VISORC_CV2FS_I_FILE

#ifdef CHIP_CV2FS
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
#define VP_CROP_DISABLE 0x082002CU

/* For CORC current time */
#define CORC_TIME       0x160044U

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

/*For dummy registers in VP */
#define ARITH1_PU_DUMMY_OFFSET      0x800000U
#define INTERP_PU_DUMMY_OFFSET      0x801000U
#define INTEG_PU_DUMMY_OFFSET       0x802000U
#define MINMAX_PU_DUMMY_OFFSET      0x803000U
#define GEN_PU_DUMMY_OFFSET         0x804000U
#define CMP_PU_DUMMY_OFFSET         0x805000U
#define LOGIC1_PU_DUMMY_OFFSET      0x806000U
#define COUNT_PU_DUMMY_OFFSET       0x807000U
#define WARP_PU_DUMMY_OFFSET        0x808000U
#define SEGMENTS_PU_DUMMY_OFFSET    0x809000U
#define STATISTICS_PU_DUMMY_OFFSET  0x80a000U
#define COPY_PU_DUMMY_OFFSET        0x80b000U
#define TRAN1_PU_DUMMY_OFFSET       0x80c000U
#define TRAN2_PU_DUMMY_OFFSET       0x80d000U
#define RESAMP_PU_DUMMY_OFFSET      0x80e000U
#define ICE1_PU_DUMMY_OFFSET        0x80f000U
#define ICE2_PU_DUMMY_OFFSET        0x810000U
#define VP_MAIN_PU_DUMMY_OFFSET     0x820008U
#define VP_XMEM_PU_DUMMY_OFFSET     0x860004U

/*For dummy registers in VMEM */
#define VMEM_DUMMY_OFFSET           0x13fff8U


/*For dummy registers in STORC */
#define STORC_DUMMY_OFFSET           0x010008U

/*cehu registers */
#define CEHU_ARITH1_DP_OFFSET       0x800FFCU
#define CEHU_INTERP_DP_OFFSET       0x801FFCU
#define CEHU_INTEG_DP_OFFSET        0x802FFCU
#define CEHU_MINMAX_DP_OFFSET       0x803FFCU
#define CEHU_GEN_DP_OFFSET          0x804FFCU
#define CEHU_COMPARE_DP_OFFSET      0x805FFCU
#define CEHU_LOGIC1_DP_OFFSET       0x806FFCU
#define CEHU_COUNT_DP_OFFSET        0x807FFCU
#define CEHU_WARP_DP_OFFSET         0x808FFCU
#define CEHU_SEGMENTS_DP_OFFSET     0x8097F0U
#define CEHU_STATISTICS_DP_OFFSET   0x80AFFCU
#define CEHU_COPY_DP_OFFSET         0x80BFFCU
#define CEHU_TRANS1_DP_OFFSET       0x80CFFCU
#define CEHU_TRANS2_DP_OFFSET       0x80DFFCU
#define CEHU_RESAMP_DP_OFFSET       0x80EFFCU
#define CEHU_ICE1_DP_OFFSET         0x80FFFCU
#define CEHU_ICE2_DP_OFFSET         0x810FFCU
#define CEHU_VPMAIN_DP_OFFSET       0x820FFCU
#define CEHU_XMEM_DP_OFFSET         0x860FFCU
#define CEHU_VMEM_DP_OFFSET         0x13FFF4U
#define CEHU_OL2C_DP_OFFSET         0x0A00C8U
#define CEHU_FEX_DP_OFFSET          0x255FFCU

static void *visorc_io_base = NULL;

static void *visorc_reg(uint32_t index)
{
    uint8_t *src;
    void *dst;

    krn_typecast(&src,&visorc_io_base);
    dst = &src[index];

    return dst;
}

#define DUMMP_WRITE_DATA_PATTERN    0x12345678
static void vp_dummy_reg_write(void)
{
    // Write fix pattern to VP dummy registers
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(ARITH1_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(INTERP_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(INTEG_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(MINMAX_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(GEN_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(CMP_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(LOGIC1_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(COUNT_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(WARP_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(SEGMENTS_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(STATISTICS_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(COPY_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(TRAN1_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(TRAN2_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(RESAMP_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(ICE1_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(ICE2_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(VP_MAIN_PU_DUMMY_OFFSET));
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(VP_XMEM_PU_DUMMY_OFFSET));

    // Write fix pattern to VMEM dummy registers
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(VMEM_DUMMY_OFFSET));

    // Write fix pattern to STORC dummy registers
    krn_writel_relaxed(DUMMP_WRITE_DATA_PATTERN, visorc_reg(STORC_DUMMY_OFFSET));
}

static void pre_reset_proc(void)
{
    vp_dummy_reg_write();
}

static uint32_t post_reset_proc_part1(void)
{
    uint32_t val;
    uint32_t retcode = 0U;

    // Read back VP dummy registers and check with default value
    // There is no WARP_PU_DUMMY and SEGMENTS_PU_DUMMY registers

    val = krn_readl_relaxed(visorc_reg(ARITH1_PU_DUMMY_OFFSET));
    if (val != 0xcafef00dU) {
        retcode |= 0x1U;
    }

    val = krn_readl_relaxed(visorc_reg(INTERP_PU_DUMMY_OFFSET));
    if (val != 0xc0cac01aU) {
        retcode |= 0x2U;
    }

    val = krn_readl_relaxed(visorc_reg(INTEG_PU_DUMMY_OFFSET));
    if (val != 0x0U) {
        retcode |= 0x4U;
    }

    val = krn_readl_relaxed(visorc_reg(MINMAX_PU_DUMMY_OFFSET));
    if (val != 0xfe52c67bU) {
        retcode |= 0x8U;
    }

    val = krn_readl_relaxed(visorc_reg(GEN_PU_DUMMY_OFFSET));
    if (val != 0x00000000U) {
        retcode |= 0x10U;
    }

    val = krn_readl_relaxed(visorc_reg(CMP_PU_DUMMY_OFFSET));
    if (val != 0x63a87ad4U) {
        retcode |= 0x20U;
    }

    val = krn_readl_relaxed(visorc_reg(LOGIC1_PU_DUMMY_OFFSET));
    if (val != 0xc0cac01aU) {
        retcode |= 0x40U;
    }

    val = krn_readl_relaxed(visorc_reg(COUNT_PU_DUMMY_OFFSET));
    if (val != 0xc0cac01aU) {
        retcode |= 0x80U;
    }

    val = krn_readl_relaxed(visorc_reg(STATISTICS_PU_DUMMY_OFFSET));
    if (val != 0xc0cac01aU) {
        retcode |= 0x400U;
    }

    val = krn_readl_relaxed(visorc_reg(COPY_PU_DUMMY_OFFSET));
    if (val != 0xc09100d9U) {
        retcode |= 0x800U;
    }

    val = krn_readl_relaxed(visorc_reg(TRAN1_PU_DUMMY_OFFSET));
    if (val != 0xc0ffee44U) {
        retcode |= 0x1000U;
    }

    val = krn_readl_relaxed(visorc_reg(TRAN2_PU_DUMMY_OFFSET));
    if (val != 0xc0ffee44U) {
        retcode |= 0x2000U;
    }

    val = krn_readl_relaxed(visorc_reg(RESAMP_PU_DUMMY_OFFSET));
    if (val != 0x5aca3b12U) {
        retcode |= 0x4000U;
    }

    val = krn_readl_relaxed(visorc_reg(ICE1_PU_DUMMY_OFFSET));
    if (val != 0xb32ba980U) {
        retcode |= 0x8000U;
    }

    return retcode;
}

static uint32_t post_reset_proc_part2(void)
{
    uint32_t val;
    uint32_t retcode = 0U;

    val = krn_readl_relaxed(visorc_reg(ICE2_PU_DUMMY_OFFSET));
    if (val != 0xb32ba980U) {
        retcode |= 0x10000U;
    }

    val = krn_readl_relaxed(visorc_reg(VP_MAIN_PU_DUMMY_OFFSET));
    if (val != 0x00000000U) {
        retcode |= 0x20000U;
    }

    val = krn_readl_relaxed(visorc_reg(VP_XMEM_PU_DUMMY_OFFSET));
    if (val != 0x00000000U) {
        retcode |= 0x40000U;
    }

    // Read back VMEM dummy registers and check with default value
    val = krn_readl_relaxed(visorc_reg(VMEM_DUMMY_OFFSET));
    if (val != 0x007fU) {
        retcode |= 0x80000U;
    }

    // Read back STORC dummy registers and check with default value
    val = krn_readl_relaxed(visorc_reg(STORC_DUMMY_OFFSET));
    if (val != 0x0U) {
        retcode |= 0x100000U;
    }

    if (retcode == 0U) {
        // reset to defaul value ok, write dummy reg
        vp_dummy_reg_write();
    }

    return retcode;
}

static uint32_t post_reset_proc_part3(void)
{

    /* Clear vp, vmem and ol2c safety register. */
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_ARITH1_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_INTERP_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_INTEG_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_MINMAX_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_GEN_DP_OFFSET));
    /* #sm_mask_failure_point=1, sm_failure_point=107, reset_cycles_m1=3 , mask out vp error for COMPARE_DP */
    krn_writel_relaxed(0x23001ae1U, visorc_reg(CEHU_COMPARE_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_LOGIC1_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_COUNT_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_WARP_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_SEGMENTS_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_STATISTICS_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_COPY_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_TRANS1_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_TRANS2_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_RESAMP_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_ICE1_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_ICE2_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_VPMAIN_DP_OFFSET));
    krn_writel_relaxed(0x00000011U, visorc_reg(CEHU_XMEM_DP_OFFSET));
    krn_writel_relaxed(0x00000001U, visorc_reg(CEHU_VMEM_DP_OFFSET));
    krn_writel_relaxed(0x00000000U, visorc_reg(CEHU_OL2C_DP_OFFSET));
    krn_writel_relaxed(0x00040000U, visorc_reg(CEHU_FEX_DP_OFFSET));

    return 0U;
}

static uint32_t post_reset_proc(void)
{
    uint32_t retcode = 0U;

    retcode = post_reset_proc_part1();
    retcode |= post_reset_proc_part2();
    retcode |= post_reset_proc_part3();

    return retcode;
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
    krn_module_printU5("[AMBACV] TARGET CHIP   :   CV2FS \n",0U,0U,0U,0U,0U);
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

        pre_reset_proc();

        /* soft-reset vp0, vmem0 and storc. reset storc/fex also reset visorc audio clock coutner. */
        krn_writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));
        krn_writel_relaxed(0x00000380U, visorc_reg(CLUSTER_SRESET));
        krn_writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));

        retcode = post_reset_proc();
        if(retcode != 0x0U) {
            krn_printU5("[ERROR] krn_visorc_start(): post_reset_proc failed with retcode = 0x%x", retcode, 0x0U, 0x0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_VISORC_HW_UNAVAILABLE;
        }

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
            krn_writel_relaxed(0x00001F00U, visorc_reg(VP_TRESET));

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

            /* To be consistent with CV2FS although it is not necessary. */
            sysconfig->arm_base_timestamp = krn_readl_relaxed(visorc_reg(CORC_TIME));
            retcode = krn_cache_clean(&sysconfig->arm_base_timestamp, sizeof(sysconfig->arm_base_timestamp));

            /* Bit 8 needs to be reset to properly reset the FEX hardware,
            but it has the unintended side effect of resetting the actual VISORC counter.
            Read the coding orc's timer immediately before starting the actual visorc and
            set_cur_time right after visorc boot. */
            /* release sod/vp core */
            krn_writel_relaxed(0x0000001FU, visorc_reg(VP_TRESET));
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
#if 1
    krn_module_printU5("[AMBACV]: Skip softreset %d\n", unit_id, 0U, 0U, 0U, 0U);
#else
    krn_module_printU5("[AMBACV]: softreset %d\n", unit_id, 0U, 0U, 0U, 0U);
    krn_writel_relaxed(0x0000000U, visorc_reg(CLUSTER_SRESET));
    /* reset vp0 and vmem0 */
    krn_writel_relaxed(0x0000300U, visorc_reg(CLUSTER_SRESET));
    krn_writel_relaxed(0x0000000U, visorc_reg(CLUSTER_SRESET));

    /*-= Reset chicken bits =-*/
    krn_writel_relaxed(0x00100010U, visorc_reg(VP_CROP_DISABLE));
#endif
}

uint32_t krn_visorc_get_curtime(void)
{
    return krn_readl_relaxed(visorc_reg(CORC_TIME));
} /* krn_visorc_get_curtime() */

#endif //CHIP_CV2FS

#endif /* ?VISORC_CV2FS_I_FILE */
