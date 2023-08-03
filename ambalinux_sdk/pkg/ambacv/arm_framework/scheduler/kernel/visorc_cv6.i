// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
*  Copyright (c) 2022 Ambarella International LP
*/

#ifndef VISORC_CV6_I_FILE
#define VISORC_CV6_I_FILE

#ifdef CHIP_CV6

#include "os_kernel.h"
#include "ambacv_kal.h"
#include "cache_kernel.h"
#include "visorc_kernel.h"

#define VISORC_BASE     0xFFED000000UL
#define VISORC_SIZE     0x0004000000UL

/* for RCT */
#define CLUSTER_SRESET      0x080228U
#define VORC_SRESET_REG     0x08022CU

/* for VP-ORC */
#define OFS_ORCCFG_TRESET   0x0000U
#define OFS_ORCCFG_INVCACHE 0x0004U
#define OFS_ORCCFG_RESET_PC 0x0008U

/* For chicken bits */
#define OFS_VP_CROP_DISABLE 0x0040U

/* for BUG 21236, NVP only*/
#define OFS_VP_GRACE_PERIOD 0x0044U

/* For CORC current time */
#define CORC_TIME           0x140044U

/* for L2-cache */
#define OFS_REMAP_R0_START  0x0040U
#define OFS_REMAP_R0_LIMIT  0x0044U
#define OFS_REMAP_R0_RBASE  0x0048U
#define OFS_REMAP_R1_START  0x0050U
#define OFS_REMAP_R1_LIMIT  0x0054U
#define OFS_REMAP_R1_RBASE  0x0058U
#define OFS_REMAP_R2_START  0x0060U
#define OFS_REMAP_R2_LIMIT  0x0064U
#define OFS_REMAP_R2_RBASE  0x0068U
#define OFS_REMAP_R3_START  0x0070U
#define OFS_REMAP_R3_LIMIT  0x0074U
#define OFS_REMAP_R3_RBASE  0x0078U

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
    krn_module_printU5("[AMBACV] TARGET CHIP   :   CV6 \n",0U,0U,0U,0U,0U);
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

/*-= Generic reset function for any worker's VISORC and L2 portion =------------------------------*/
void krn_visorc_worker_vorc_and_l2(const ambacv_all_mem_t *mem_all, uint32_t orccfg_regofs, uint32_t orcl2cfg_regofs)
{
    uint32_t  offset;
    uint32_t  vbase, rbase, size;

    /*-= Set up remap registers =------------------------------------------------------------*/
    vbase = 0x00000000U;
    rbase = mem_all->all.base;
    size  = 0x00600000U;
    krn_writel_relaxed(vbase,        visorc_reg(orcl2cfg_regofs + OFS_REMAP_R0_START));
    krn_writel_relaxed(vbase + size, visorc_reg(orcl2cfg_regofs + OFS_REMAP_R0_LIMIT));
    krn_writel_relaxed(rbase | 1U,   visorc_reg(orcl2cfg_regofs + OFS_REMAP_R0_RBASE));

    vbase = HOTLINK_MEM_REGION_VADDR_BASE;
    rbase = (uint32_t)(mem_all->hotlink_region.base);
    size  = (uint32_t)(mem_all->hotlink_region.size);
    krn_writel_relaxed(vbase,        visorc_reg(orcl2cfg_regofs + OFS_REMAP_R1_START));
    krn_writel_relaxed(vbase + size, visorc_reg(orcl2cfg_regofs + OFS_REMAP_R1_LIMIT));
    krn_writel_relaxed(rbase | 1U,   visorc_reg(orcl2cfg_regofs + OFS_REMAP_R1_RBASE));

    krn_writel_relaxed(0x00000000U,  visorc_reg(orcl2cfg_regofs + OFS_REMAP_R2_START));
    krn_writel_relaxed(0x00000000U,  visorc_reg(orcl2cfg_regofs + OFS_REMAP_R2_LIMIT));
    krn_writel_relaxed(0x00000000U,  visorc_reg(orcl2cfg_regofs + OFS_REMAP_R2_RBASE));

    krn_writel_relaxed(0x00000000U,  visorc_reg(orcl2cfg_regofs + OFS_REMAP_R3_START));
    krn_writel_relaxed(0x00000000U,  visorc_reg(orcl2cfg_regofs + OFS_REMAP_R3_LIMIT));
    krn_writel_relaxed(0x00000000U,  visorc_reg(orcl2cfg_regofs + OFS_REMAP_R3_RBASE));

    /*-= Setup and reset ORC =--------------------------------------------------------------------*/
    offset  = (uint32_t)(mem_all->vp.base - mem_all->all.base);
    krn_writel_relaxed(0x00001F00U, visorc_reg(orccfg_regofs + OFS_ORCCFG_TRESET));     // Suspend threads
    krn_writel_relaxed(0x00000003U, visorc_reg(orccfg_regofs + OFS_ORCCFG_INVCACHE));   // Invalidate I and D caches
    krn_writel_relaxed(offset,      visorc_reg(orccfg_regofs + OFS_ORCCFG_RESET_PC));   // Set Reset PC address
} /* krn_visorc_worker_vorc_and_l2() */

void krn_visorc_reset_nvp(const ambacv_all_mem_t *mem_all, uint32_t nvp_id, uint32_t reset_release)
{
    if (nvp_id < 6) {
        uint32_t  nvp_orccfg_regofs;
        uint32_t  nvp_orcl2cfg_regofs;
        uint32_t  nvp_copcfg_regofs;

        nvp_orccfg_regofs   = 0x00440000U + (nvp_id * 0x00030000U);
        nvp_orcl2cfg_regofs = 0x00420000U + (nvp_id * 0x00030000U);
        nvp_copcfg_regofs   = 0x00430000U + (nvp_id * 0x00030000U);

        if (reset_release == 0U) {
            uint32_t  mask1, mask2;

            mask1 = 0x00020000U << nvp_id;
            mask2 = 0x00000001U << nvp_id;
            krn_writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  // Reset NVP#
            krn_writel_relaxed(mask1,       visorc_reg(CLUSTER_SRESET));  //  |
            krn_writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  //  |
            krn_writel_relaxed(0x00000000U, visorc_reg(VORC_SRESET_REG)); //  |
            krn_writel_relaxed(mask2,       visorc_reg(VORC_SRESET_REG)); //  |
            krn_writel_relaxed(0x00000000U, visorc_reg(VORC_SRESET_REG)); // -/

            krn_visorc_worker_vorc_and_l2(mem_all, nvp_orccfg_regofs, nvp_orcl2cfg_regofs);
            krn_writel_relaxed(0x00020020U, visorc_reg(nvp_copcfg_regofs + OFS_VP_CROP_DISABLE)); // Set disable-check-73; disable-size-check.
            krn_writel_relaxed(0x00000500U, visorc_reg(nvp_copcfg_regofs + OFS_VP_GRACE_PERIOD)); // Set grace period to 20

        } /* if (stage == 0U) */
        else if (reset_release == 1U) {
            krn_writel_relaxed(0x00001C03U, visorc_reg(nvp_orccfg_regofs + OFS_ORCCFG_TRESET));   // Reset threads
        } /* if (stage == 1U) */
        else { /* if (stage > 1U) */
            ; // Do nothing, MISRA-15.7
        } /* if (stage > 1U) */
    } /* if (nvp_id < 6) */
} /* krn_visorc_reset_nvp() */

void krn_visorc_reset_gvp(const ambacv_all_mem_t *mem_all, uint32_t gvp_id, uint32_t reset_release)
{
    if (gvp_id < 2) {
        uint32_t  gvp_orccfg_regofs;
        uint32_t  gvp_orcl2cfg_regofs;
        uint32_t  gvp_copcfg_regofs;

        gvp_orccfg_regofs   = 0x00560000U + (gvp_id * 0x00030000U);
        gvp_orcl2cfg_regofs = 0x00540000U + (gvp_id * 0x00030000U);
        gvp_copcfg_regofs   = 0x00550000U + (gvp_id * 0x00030000U);

        if (reset_release == 0U) {
            uint32_t  mask1, mask2;

            mask1 = 0x00800000U << gvp_id;
            mask2 = 0x00000040U << gvp_id;
            krn_writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  // Reset GVP#
            krn_writel_relaxed(mask1,       visorc_reg(CLUSTER_SRESET));  //  |
            krn_writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  //  |
            krn_writel_relaxed(0x00000000U, visorc_reg(VORC_SRESET_REG)); //  |
            krn_writel_relaxed(mask2,       visorc_reg(VORC_SRESET_REG)); //  |
            krn_writel_relaxed(0x00000000U, visorc_reg(VORC_SRESET_REG)); // -/

            krn_visorc_worker_vorc_and_l2(mem_all, gvp_orccfg_regofs, gvp_orcl2cfg_regofs);
            krn_writel_relaxed(0x00020000U, visorc_reg(gvp_copcfg_regofs + OFS_VP_CROP_DISABLE)); // Set disable-check-73; disabling size check may not be needed.
        } /* if (stage == 0U) */
        else if (reset_release == 1U) {
            krn_writel_relaxed(0x00001C03U, visorc_reg(gvp_orccfg_regofs + OFS_ORCCFG_TRESET));   // Reset threads
        } /* if (stage == 1U) */
        else { /* if (stage > 1U) */
            ; // Do nothing, MISRA-15.7
        } /* if (stage > 1U) */
    } /* if (gvp_id < 2) */
} /* krn_visorc_reset_gvp() */

void krn_visorc_reset_fex(const ambacv_all_mem_t *mem_all, uint32_t reset_release)
{
    uint32_t  fex0_orccfg_regofs;
    uint32_t  fex0_orcl2cfg_regofs;
    //uint32_t  fex0_copcfg_regofs;
    uint32_t  fex1_orccfg_regofs;
    uint32_t  fex1_orcl2cfg_regofs;
    //uint32_t  fex1_copcfg_regofs;

    fex0_orccfg_regofs    = 0x003E0000U;
    fex0_orcl2cfg_regofs  = 0x003C0000U;
    //fex0_copcfg_regofs    = 0x003A0000U;  // Currently unused, placed here for documentation
    fex1_orccfg_regofs    = 0x003F0000U;
    fex1_orcl2cfg_regofs  = 0x003D0000U;
    //fex1_copcfg_regofs    = 0x003B0000U;  // Currently unused, placed here for documentation

    if (reset_release == 0U) {
        krn_writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  // Reset FEX0+1
        krn_writel_relaxed(0x00008000U, visorc_reg(CLUSTER_SRESET));  //  |
        krn_writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  // -/

        krn_visorc_worker_vorc_and_l2(mem_all, fex0_orccfg_regofs, fex0_orcl2cfg_regofs);
        krn_visorc_worker_vorc_and_l2(mem_all, fex1_orccfg_regofs, fex1_orcl2cfg_regofs);
    } /* if (stage == 0U) */
    else if (reset_release == 1U) {
        krn_writel_relaxed(0x00001C03U, visorc_reg(fex0_orccfg_regofs + OFS_ORCCFG_TRESET));   // Reset threads
        krn_writel_relaxed(0x00001C03U, visorc_reg(fex1_orccfg_regofs + OFS_ORCCFG_TRESET));   // Reset threads
    } /* if (stage == 1U) */
    else { /* if (stage > 1U) */
        ; // Do nothing, MISRA-15.7
    } /* if (stage > 1U) */
} /* krn_visorc_reset_fex() */

void krn_visorc_reset_fma(const ambacv_all_mem_t *mem_all, uint32_t reset_release)
{
    uint32_t  fma0_orccfg_regofs;
    uint32_t  fma0_orcl2cfg_regofs;
    //uint32_t  fma0_copcfg_regofs;
    uint32_t  fma1_orccfg_regofs;
    uint32_t  fma1_orcl2cfg_regofs;
    //uint32_t  fma1_copcfg_regofs;

    fma0_orccfg_regofs    = 0x00300000U;
    fma0_orcl2cfg_regofs  = 0x002E0000U;
    //fma0_copcfg_regofs    = 0x002C0000U;  // Currently unused, placed here for documentation
    fma1_orccfg_regofs    = 0x00310000U;
    fma1_orcl2cfg_regofs  = 0x002F0000U;
    //fma1_copcfg_regofs    = 0x002D0000U;  // Currently unused, placed here for documentation

    if (reset_release == 0U) {
        krn_writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  // Reset FMA0+1
        krn_writel_relaxed(0x00010000U, visorc_reg(CLUSTER_SRESET));  //  |
        krn_writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  // -/

        krn_visorc_worker_vorc_and_l2(mem_all, fma0_orccfg_regofs, fma0_orcl2cfg_regofs);
        krn_visorc_worker_vorc_and_l2(mem_all, fma1_orccfg_regofs, fma1_orcl2cfg_regofs);
    } /* if (stage == 0U) */
    else if (reset_release == 1U) {
        krn_writel_relaxed(0x00001C03U, visorc_reg(fma0_orccfg_regofs + OFS_ORCCFG_TRESET));   // Reset threads
        krn_writel_relaxed(0x00001C03U, visorc_reg(fma1_orccfg_regofs + OFS_ORCCFG_TRESET));   // Reset threads
    } /* if (stage == 1U) */
    else { /* if (stage > 1U) */
        ; // Do nothing, MISRA-15.7
    } /* if (stage > 1U) */
} /* krn_visorc_reset_fma() */

uint32_t krn_visorc_reset_mgmt(const ambacv_all_mem_t *mem_all, uint32_t reset_release)
{
    uint32_t  retcode;
    uint32_t  mgmt_orccfg_regofs;
    uint32_t  mgmt_orcl2cfg_regofs;

    retcode = ERRCODE_NONE;
    mgmt_orccfg_regofs    = 0x00100000U;
    mgmt_orcl2cfg_regofs  = 0x00110000U;

    if (reset_release == 0U) {
        uint32_t  reg_val;
        krn_visorc_worker_vorc_and_l2(mem_all, mgmt_orccfg_regofs, mgmt_orcl2cfg_regofs);
        reg_val = krn_readl_relaxed(visorc_reg(mgmt_orcl2cfg_regofs + OFS_REMAP_R0_START));
        if (reg_val != 0U) {
            const void *ptr;
            uint64_t reg = 0UL;

            ptr = visorc_reg(mgmt_orcl2cfg_regofs + OFS_REMAP_R0_START);
            krn_typecast(&reg,&ptr);
            krn_printU5("[ERROR] krn_visorc_start() : setup register(0x%x) failed, write=0x%x read=0x%x", reg, 0x0U, reg_val, 0U, 0U);
            retcode = ERR_DRV_SCHDR_VISORC_HW_UNAVAILABLE;
        } /* if (reg_val != 0U) */
    } /* if (stage == 0U) */
    else if (reset_release == 1U) {
        krn_writel_relaxed(0x00001C03U, visorc_reg(mgmt_orccfg_regofs + OFS_ORCCFG_TRESET));   // Reset threads
    } /* if (stage == 1U) */
    else { /* if (stage > 1U) */
        ; // Do nothing, MISRA-15.7
    } /* if (stage > 1U) */

    return retcode;

} /* krn_visorc_reset_mgmt() */

uint32_t krn_visorc_start(uint32_t flag, visorc_init_params_t *sysconfig)
{
    const ambacv_all_mem_t *mem_all;
    uint32_t retcode = ERRCODE_NONE;

    (void) flag;
    if(sysconfig == NULL) {
        retcode = ERR_DRV_SCHDR_VISORC_INIT_INVALID;
        krn_printU5("[ERROR] krn_visorc_start() : sysconfig == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        mem_all = krn_ambacv_get_mem();

        /*-= Reset all worker cores =-----------------------------------------*/
        krn_visorc_reset_nvp(mem_all, 0, 0);          // Reset, but do not enable
        krn_visorc_reset_nvp(mem_all, 1, 0);          //  |
        krn_visorc_reset_nvp(mem_all, 2, 0);          //  |
        krn_visorc_reset_nvp(mem_all, 3, 0);          //  |
        krn_visorc_reset_nvp(mem_all, 4, 0);          //  |
        krn_visorc_reset_nvp(mem_all, 5, 0);          //  |
        krn_visorc_reset_gvp(mem_all, 0, 0);          //  |
        krn_visorc_reset_gvp(mem_all, 1, 0);          //  |
        krn_visorc_reset_fex(mem_all, 0);             //  | * FEX0 and FEX1 are tied together
        krn_visorc_reset_fma(mem_all, 0);             // -/ * FMA0 and FMA1 are tied together
        retcode = krn_visorc_reset_mgmt(mem_all, 0);  // Reset, also perform readback check

        if (retcode == ERRCODE_NONE) {
            /* To be consistent with CV2 although it is not necessary. */
            sysconfig->arm_base_timestamp = krn_readl_relaxed(visorc_reg(CORC_TIME));
            retcode = krn_cache_clean(&sysconfig->arm_base_timestamp, sizeof(sysconfig->arm_base_timestamp));

            /*-= Release and run all cores =----------------------------------*/
            krn_visorc_reset_nvp(mem_all, 0, 1);          // Bring cores out of suspend/reset
            krn_visorc_reset_nvp(mem_all, 1, 1);          //  |
            krn_visorc_reset_nvp(mem_all, 2, 1);          //  |
            krn_visorc_reset_nvp(mem_all, 3, 1);          //  |
            krn_visorc_reset_nvp(mem_all, 4, 1);          //  |
            krn_visorc_reset_nvp(mem_all, 5, 1);          //  |
            krn_visorc_reset_gvp(mem_all, 0, 1);          //  |
            krn_visorc_reset_gvp(mem_all, 1, 1);          //  |
            krn_visorc_reset_fex(mem_all, 1);             //  | * FEX0 and FEX1 are tied together
            krn_visorc_reset_fma(mem_all, 1);             // -/ * FMA0 and FMA1 are tied together
            retcode = krn_visorc_reset_mgmt(mem_all, 1);  // Reset, also perform readback check
        } /* if (retcode == CVSCH_ERRCODE_NONE) */
    }

    return retcode;
}

void krn_visorc_kick(void)
{
    uint32_t  arm2morc_reg = 0x3200004U;

    krn_writel_relaxed(0x00000001U, visorc_reg(arm2morc_reg));
}

void krn_visorc_armidsp_async_queue_kick(void)
{
    uint32_t  arm2morc_reg = 0x3200004U;

    krn_writel_relaxed(0x00000004U, visorc_reg(arm2morc_reg));
}

void krn_visorc_armidsp_queue_kick(void)
{
    uint32_t  arm2morc_reg = 0x3200004U;

    krn_writel_relaxed(0x00000004U, visorc_reg(arm2morc_reg));
}

void krn_visorc_armvin_queue_kick(void)
{
    uint32_t  arm2morc_reg = 0x3200004U;

    krn_writel_relaxed(0x00000008U, visorc_reg(arm2morc_reg));
}


void krn_visorc_softreset(uint32_t unit_id)
{
    const ambacv_all_mem_t *mem_all;

    mem_all = krn_ambacv_get_mem();

    switch (unit_id) {
    case CVCORE_NVP0:
        krn_visorc_reset_nvp(mem_all, 0, 0);  // Reset
        krn_visorc_reset_nvp(mem_all, 0, 1);  // -/
        krn_module_printU5("[AMBACV]: softreset NVP0\n", 0U, 0U, 0U, 0U, 0U);
        break;
    case CVCORE_NVP1:
        krn_visorc_reset_nvp(mem_all, 1, 0);  // Reset
        krn_visorc_reset_nvp(mem_all, 1, 1);  // -/
        krn_module_printU5("[AMBACV]: softreset NVP1\n", 0U, 0U, 0U, 0U, 0U);
        break;
    case CVCORE_NVP2:
        krn_visorc_reset_nvp(mem_all, 2, 0);  // Reset
        krn_visorc_reset_nvp(mem_all, 2, 1);  // -/
        krn_module_printU5("[AMBACV]: softreset NVP2\n", 0U, 0U, 0U, 0U, 0U);
        break;
    case CVCORE_NVP3:
        krn_visorc_reset_nvp(mem_all, 3, 0);  // Reset
        krn_visorc_reset_nvp(mem_all, 3, 1);  // -/
        krn_module_printU5("[AMBACV]: softreset NVP3\n", 0U, 0U, 0U, 0U, 0U);
        break;
    case CVCORE_NVP4:
        krn_visorc_reset_nvp(mem_all, 4, 0);  // Reset
        krn_visorc_reset_nvp(mem_all, 4, 1);  // -/
        krn_module_printU5("[AMBACV]: softreset NVP4\n", 0U, 0U, 0U, 0U, 0U);
        break;
    case CVCORE_NVP5:
        krn_visorc_reset_nvp(mem_all, 5, 0);  // Reset
        krn_visorc_reset_nvp(mem_all, 5, 1);  // -/
        krn_module_printU5("[AMBACV]: softreset NVP5\n", 0U, 0U, 0U, 0U, 0U);
        break;
    case CVCORE_GVP0:
        krn_visorc_reset_gvp(mem_all, 0, 0);  // Reset
        krn_visorc_reset_gvp(mem_all, 0, 1);  // -/
        krn_module_printU5("[AMBACV]: softreset GVP0\n", 0U, 0U, 0U, 0U, 0U);
        break;
    case CVCORE_GVP1:
        krn_visorc_reset_gvp(mem_all, 1, 0);  // Reset
        krn_visorc_reset_gvp(mem_all, 1, 1);  // -/
        krn_module_printU5("[AMBACV]: softreset GVP1\n", 0U, 0U, 0U, 0U, 0U);
        break;
    case CVCORE_FEX0:
    case CVCORE_FEX1:
        krn_visorc_reset_fex(mem_all, 0);     // Reset
        krn_visorc_reset_fex(mem_all, 1);     // -/
        krn_module_printU5("[AMBACV]: softreset FEX0+FEX1\n", 0U, 0U, 0U, 0U, 0U);
        break;
    case CVCORE_FMA0:
    case CVCORE_FMA1:
        krn_visorc_reset_fma(mem_all, 0);     // Reset
        krn_visorc_reset_fma(mem_all, 1);     // -/
        krn_module_printU5("[AMBACV]: softreset FMA0+FMA1\n", 0U, 0U, 0U, 0U, 0U);
        break;
    default:
        krn_module_printU5("[AMBACV]: softreset %d : FAILED (invalid unit_id)\n", unit_id, 0U, 0U, 0U, 0U);
        break;
    } /* switch (unit_id) */
}

uint32_t krn_visorc_get_curtime(void)
{
    return krn_readl_relaxed(visorc_reg(CORC_TIME));
} /* krn_visorc_get_curtime() */

#endif //CHIP_CV6

#endif /* ?VISORC_CV6_I_FILE */

