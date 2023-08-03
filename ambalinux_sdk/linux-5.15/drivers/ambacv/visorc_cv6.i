#include <linux/module.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/of_irq.h>
#include <linux/uaccess.h>
#include "ambacv_kal.h"

#define VISORC_BASE     0xFFED000000
#define VISORC_SIZE     0x0004000000

/* for RCT */
#define CLUSTER_SRESET      0x080228
#define VORC_SRESET_REG     0x08022C

/* for VP-ORC */
#define OFS_ORCCFG_TRESET   0x0000
#define OFS_ORCCFG_INVCACHE 0x0004
#define OFS_ORCCFG_RESET_PC 0x0008

/* for SMEM */
#define SYNC_CTR_BASE       0x05F000

/* For chicken bits */
#define OFS_VP_CROP_DISABLE 0x0042

/* For CORC current time */
#define CORC_TIME           0x140044

/* for L2-cache */
#define OFS_REMAP_R0_START  0x0040
#define OFS_REMAP_R0_LIMIT  0x0044
#define OFS_REMAP_R0_RBASE  0x0048
#define OFS_REMAP_R1_START  0x0050
#define OFS_REMAP_R1_LIMIT  0x0054
#define OFS_REMAP_R1_RBASE  0x0058
#define OFS_REMAP_R2_START  0x0060
#define OFS_REMAP_R2_LIMIT  0x0064
#define OFS_REMAP_R2_RBASE  0x0068
#define OFS_REMAP_R3_START  0x0070
#define OFS_REMAP_R3_LIMIT  0x0074
#define OFS_REMAP_R3_RBASE  0x0078

#define visorc_reg(x)   (visorc_io_base + (x))

static void __iomem     *visorc_io_base;
static unsigned int     vp_irq;
extern uint32_t debug_level;

#define DEBUG_ISR
#ifdef  DEBUG_ISR
#define ILOG            printk
#else
#define ILOG(...)
#endif

static void __init vp_of_init(struct device_node *np)
{
    vp_irq = irq_of_parse_and_map(np, 0);
    if (vp_irq <= 0) {
        pr_err("[AMBACV] Error!! can't setup VP0 irq\n");
    }
}

static const struct of_device_id __vp_of_table = {
    .compatible = "ambarella,vp-module",
    .data = vp_of_init,
};

void visorc_init(void)
{
    struct device_node *np;
    const struct of_device_id *match;
    of_init_fn_1 init_func;

    if (visorc_io_base == NULL) {
        visorc_io_base = ioremap(VISORC_BASE, VISORC_SIZE);
        if (visorc_io_base == NULL) {
            pr_err("ioremap VISORC registers failed\n");
        }
    }

    for_each_matching_node_and_match(np, &__vp_of_table, &match) {
        if (!of_device_is_available(np)) {
            continue;
        }

        init_func = match->data;
        init_func(np);
    }

    /* set up sod/vp binary memory regions */
    ambacv_global_mem.sod.base  = 0;
    ambacv_global_mem.sod.size  = 0;
    ambacv_global_mem.vp.base   = ambacv_global_mem.all.base + 0x00400000;
    ambacv_global_mem.vp.size   = 0x00200000;
    ambacv_debug_port.base      = VISORC_BASE;
    ambacv_debug_port.size      = VISORC_SIZE;
    pr_info("[AMBACV] TARGET CHIP:   CV6 \n");
}

void visorc_shutdown(void)
{
    irq_dispose_mapping(vp_irq);
    if (visorc_io_base != NULL) {
        iounmap(visorc_io_base);
        visorc_io_base = NULL;
    }
}

int visorc_finalize_loading(unsigned long arg)
{
    ambacv_mem_t mem;
    int ret;

    ret = copy_from_user(&mem, (void*)arg, 4 * sizeof(mem.sod));
    if (ret != 0)
        return ret;

    ambacv_cache_clean(ambacv_c2v(mem.vp.base),   mem.vp.size);
    pr_info("\tVP:   [0x%08x ++ 0x%08x]\n",
            (uint32_t)ambacv_c2p(mem.vp.base),   (uint32_t)mem.vp.size);

    return 0;
}

/*-= Generic reset function for any worker's VISORC and L2 portion =------------------------------*/
void visorc_worker_vorc_and_l2(const ambacv_mem_t *mem_all, uint32_t orccfg_regofs, uint32_t orcl2cfg_regofs)
{
    uint32_t  offset;
    uint32_t  vbase, rbase, size;

    /*-= Set up remap registers =------------------------------------------------------------*/
    vbase = 0x00000000U;
    rbase = mem_all->all.base;
    size  = 0x00600000U;
    writel_relaxed(vbase,        visorc_reg(orcl2cfg_regofs + OFS_REMAP_R0_START));
    writel_relaxed(vbase + size, visorc_reg(orcl2cfg_regofs + OFS_REMAP_R0_LIMIT));
    writel_relaxed(rbase | 1U,   visorc_reg(orcl2cfg_regofs + OFS_REMAP_R0_RBASE));

    vbase = HOTLINK_MEM_REGION_VADDR_BASE;
    rbase = (uint32_t)(mem_all->hotlink_region.base);
    size  = (uint32_t)(mem_all->hotlink_region.size);
    writel_relaxed(vbase,        visorc_reg(orcl2cfg_regofs + OFS_REMAP_R1_START));
    writel_relaxed(vbase + size, visorc_reg(orcl2cfg_regofs + OFS_REMAP_R1_LIMIT));
    writel_relaxed(rbase | 1U,   visorc_reg(orcl2cfg_regofs + OFS_REMAP_R1_RBASE));

    writel_relaxed(0x00000000U,  visorc_reg(orcl2cfg_regofs + OFS_REMAP_R2_START));
    writel_relaxed(0x00000000U,  visorc_reg(orcl2cfg_regofs + OFS_REMAP_R2_LIMIT));
    writel_relaxed(0x00000000U,  visorc_reg(orcl2cfg_regofs + OFS_REMAP_R2_RBASE));

    writel_relaxed(0x00000000U,  visorc_reg(orcl2cfg_regofs + OFS_REMAP_R3_START));
    writel_relaxed(0x00000000U,  visorc_reg(orcl2cfg_regofs + OFS_REMAP_R3_LIMIT));
    writel_relaxed(0x00000000U,  visorc_reg(orcl2cfg_regofs + OFS_REMAP_R3_RBASE));

    /*-= Setup and reset ORC =--------------------------------------------------------------------*/
    offset  = (uint32_t)(mem_all->vp.base - mem_all->all.base);
    writel_relaxed(0x00001F00U, visorc_reg(orccfg_regofs + OFS_ORCCFG_TRESET));     // Suspend threads
    writel_relaxed(0x00000003U, visorc_reg(orccfg_regofs + OFS_ORCCFG_INVCACHE));   // Invalidate I and D caches
    writel_relaxed(offset,      visorc_reg(orccfg_regofs + OFS_ORCCFG_RESET_PC));   // Set Reset PC address
} /* visorc_orc_recfg() */

void visorc_reset_nvp(const ambacv_mem_t *mem_all, uint32_t nvp_id, uint32_t reset_release)
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
            writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  // Reset NVP#
            writel_relaxed(mask1,       visorc_reg(CLUSTER_SRESET));  //  |
            writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  //  |
            writel_relaxed(0x00000000U, visorc_reg(VORC_SRESET_REG)); //  |
            writel_relaxed(mask2,       visorc_reg(VORC_SRESET_REG)); //  |
            writel_relaxed(0x00000000U, visorc_reg(VORC_SRESET_REG)); // -/

            visorc_worker_vorc_and_l2(mem_all, nvp_orccfg_regofs, nvp_orcl2cfg_regofs);
            writel_relaxed(0x00020000U, visorc_reg(nvp_copcfg_regofs + OFS_VP_CROP_DISABLE)); // Set disable-check-73; disabling size check may not be needed.
        } /* if (stage == 0U) */
        else if (reset_release == 1U) {
            writel_relaxed(0x00001C03U, visorc_reg(nvp_orccfg_regofs + OFS_ORCCFG_TRESET));   // Reset threads
        } /* if (stage == 1U) */
        else { /* if (stage > 1U) */
            ; // Do nothing, MISRA-15.7
        } /* if (stage > 1U) */
    } /* if (nvp_id < 6) */
} /* visorc_reset_nvp() */

void visorc_reset_gvp(const ambacv_mem_t *mem_all, uint32_t gvp_id, uint32_t reset_release)
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
            writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  // Reset GVP#
            writel_relaxed(mask1,       visorc_reg(CLUSTER_SRESET));  //  |
            writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  //  |
            writel_relaxed(0x00000000U, visorc_reg(VORC_SRESET_REG)); //  |
            writel_relaxed(mask2,       visorc_reg(VORC_SRESET_REG)); //  |
            writel_relaxed(0x00000000U, visorc_reg(VORC_SRESET_REG)); // -/

            visorc_worker_vorc_and_l2(mem_all, gvp_orccfg_regofs, gvp_orcl2cfg_regofs);
            writel_relaxed(0x00020000U, visorc_reg(gvp_copcfg_regofs + OFS_VP_CROP_DISABLE)); // Set disable-check-73; disabling size check may not be needed.
        } /* if (stage == 0U) */
        else if (reset_release == 1U) {
            writel_relaxed(0x00001C03U, visorc_reg(gvp_orccfg_regofs + OFS_ORCCFG_TRESET));   // Reset threads
        } /* if (stage == 1U) */
        else { /* if (stage > 1U) */
            ; // Do nothing, MISRA-15.7
        } /* if (stage > 1U) */
    } /* if (gvp_id < 2) */
} /* visorc_reset_gvp() */

void visorc_reset_fex(const ambacv_mem_t *mem_all, uint32_t reset_release)
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
        writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  // Reset FEX0+1
        writel_relaxed(0x00008000U, visorc_reg(CLUSTER_SRESET));  //  |
        writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  // -/

        visorc_worker_vorc_and_l2(mem_all, fex0_orccfg_regofs, fex0_orcl2cfg_regofs);
        visorc_worker_vorc_and_l2(mem_all, fex1_orccfg_regofs, fex1_orcl2cfg_regofs);
    } /* if (stage == 0U) */
    else if (reset_release == 1U) {
        writel_relaxed(0x00001C03U, visorc_reg(fex0_orccfg_regofs + OFS_ORCCFG_TRESET));   // Reset threads
        writel_relaxed(0x00001C03U, visorc_reg(fex1_orccfg_regofs + OFS_ORCCFG_TRESET));   // Reset threads
    } /* if (stage == 1U) */
    else { /* if (stage > 1U) */
        ; // Do nothing, MISRA-15.7
    } /* if (stage > 1U) */
} /* visorc_reset_fex() */

void visorc_reset_fma(const ambacv_mem_t *mem_all, uint32_t reset_release)
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
        writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  // Reset FMA0+1
        writel_relaxed(0x00010000U, visorc_reg(CLUSTER_SRESET));  //  |
        writel_relaxed(0x00000000U, visorc_reg(CLUSTER_SRESET));  // -/

        visorc_worker_vorc_and_l2(mem_all, fma0_orccfg_regofs, fma0_orcl2cfg_regofs);
        visorc_worker_vorc_and_l2(mem_all, fma1_orccfg_regofs, fma1_orcl2cfg_regofs);
    } /* if (stage == 0U) */
    else if (reset_release == 1U) {
        writel_relaxed(0x00001C03U, visorc_reg(fma0_orccfg_regofs + OFS_ORCCFG_TRESET));   // Reset threads
        writel_relaxed(0x00001C03U, visorc_reg(fma1_orccfg_regofs + OFS_ORCCFG_TRESET));   // Reset threads
    } /* if (stage == 1U) */
    else { /* if (stage > 1U) */
        ; // Do nothing, MISRA-15.7
    } /* if (stage > 1U) */
} /* visorc_reset_fma() */

uint32_t visorc_reset_mgmt(const ambacv_mem_t *mem_all, uint32_t reset_release)
{
    uint32_t  mgmt_orccfg_regofs;
    uint32_t  mgmt_orcl2cfg_regofs;

    mgmt_orccfg_regofs    = 0x00100000U;
    mgmt_orcl2cfg_regofs  = 0x00110000U;

    if (reset_release == 0U) {
        visorc_worker_vorc_and_l2(mem_all, mgmt_orccfg_regofs, mgmt_orcl2cfg_regofs);
    } /* if (stage == 0U) */
    else if (reset_release == 1U) {
        writel_relaxed(0x00001C03U, visorc_reg(mgmt_orccfg_regofs + OFS_ORCCFG_TRESET));   // Reset threads
    } /* if (stage == 1U) */
    else { /* if (stage > 1U) */
        ; // Do nothing, MISRA-15.7
    } /* if (stage > 1U) */

    return 0U;

} /* visorc_reset_mgmt() */

int visorc_start(unsigned int flag, visorc_init_params_t *sysconfig)
{
    ambacv_mem_t *mem_all = &ambacv_global_mem;

    /*-= Reset all worker cores =-----------------------------------------*/
    visorc_reset_nvp(mem_all, 0, 0);          // Reset, but do not enable
    visorc_reset_nvp(mem_all, 1, 0);          //  |
    visorc_reset_nvp(mem_all, 2, 0);          //  |
    visorc_reset_nvp(mem_all, 3, 0);          //  |
    visorc_reset_nvp(mem_all, 4, 0);          //  |
    visorc_reset_nvp(mem_all, 5, 0);          //  |
    visorc_reset_gvp(mem_all, 0, 0);          //  |
    visorc_reset_gvp(mem_all, 1, 0);          //  |
    visorc_reset_fex(mem_all, 0);             //  | * FEX0 and FEX1 are tied together
    visorc_reset_fma(mem_all, 0);             // -/ * FMA0 and FMA1 are tied together
    visorc_reset_mgmt(mem_all, 0);  // Reset, also perform readback check

    sysconfig->arm_base_timestamp = readl(visorc_reg(CORC_TIME));
    (void) ambacv_cache_clean(&sysconfig->arm_base_timestamp, sizeof(sysconfig->arm_base_timestamp));

    /*-= Release and run all cores =----------------------------------*/
    visorc_reset_nvp(mem_all, 0, 1);          // Bring cores out of suspend/reset
    visorc_reset_nvp(mem_all, 1, 1);          //  |
    visorc_reset_nvp(mem_all, 2, 1);          //  |
    visorc_reset_nvp(mem_all, 3, 1);          //  |
    visorc_reset_nvp(mem_all, 4, 1);          //  |
    visorc_reset_nvp(mem_all, 5, 1);          //  |
    visorc_reset_gvp(mem_all, 0, 1);          //  |
    visorc_reset_gvp(mem_all, 1, 1);          //  |
    visorc_reset_fex(mem_all, 1);             //  | * FEX0 and FEX1 are tied together
    visorc_reset_fma(mem_all, 1);             // -/ * FMA0 and FMA1 are tied together
    visorc_reset_mgmt(mem_all, 1);  // Reset, also perform readback check

    return 0;
}

int visorc_stop(void)
{
    return 0;
}

void orc_send_sync_count(int id, int val)
{
    writel_relaxed(val, visorc_reg(SYNC_CTR_BASE) + 4 * id);
}

void visorc_kick(void)
{
    uint32_t  arm2morc_reg = 0x3200004U;

    writel_relaxed(0x00000001U, visorc_reg(arm2morc_reg));
}

void visorc_armidsp_queue_kick(void)
{
    uint32_t  arm2morc_reg = 0x3200004U;

    writel_relaxed(0x00000004U, visorc_reg(arm2morc_reg));
}

void visorc_armvin_queue_kick(void)
{
    uint32_t  arm2morc_reg = 0x3200004U;

    writel_relaxed(0x00000008U, visorc_reg(arm2morc_reg));
}

void visorc_softreset(int unit_id)
{
    ambacv_mem_t *mem_all = &ambacv_global_mem;

    if (debug_level > 0) {
        pr_info("[AMBACV]: softreset %d\n", unit_id);
    } /* if (debug_level > 0) */
    switch (unit_id) {
    case CVCORE_NVP0:
        visorc_reset_nvp(mem_all, 0, 0);  // Reset
        visorc_reset_nvp(mem_all, 0, 1);  // -/
        pr_info("[AMBACV]: softreset NVP0\n");
        break;
    case CVCORE_NVP1:
        visorc_reset_nvp(mem_all, 1, 0);  // Reset
        visorc_reset_nvp(mem_all, 1, 1);  // -/
        pr_info("[AMBACV]: softreset NVP1\n");
        break;
    case CVCORE_NVP2:
        visorc_reset_nvp(mem_all, 2, 0);  // Reset
        visorc_reset_nvp(mem_all, 2, 1);  // -/
        pr_info("[AMBACV]: softreset NVP2\n");
        break;
    case CVCORE_NVP3:
        visorc_reset_nvp(mem_all, 3, 0);  // Reset
        visorc_reset_nvp(mem_all, 3, 1);  // -/
        pr_info("[AMBACV]: softreset NVP3\n");
        break;
    case CVCORE_NVP4:
        visorc_reset_nvp(mem_all, 4, 0);  // Reset
        visorc_reset_nvp(mem_all, 4, 1);  // -/
        pr_info("[AMBACV]: softreset NVP4\n");
        break;
    case CVCORE_NVP5:
        visorc_reset_nvp(mem_all, 5, 0);  // Reset
        visorc_reset_nvp(mem_all, 5, 1);  // -/
        pr_info("[AMBACV]: softreset NVP5\n");
        break;
    case CVCORE_GVP0:
        visorc_reset_gvp(mem_all, 0, 0);  // Reset
        visorc_reset_gvp(mem_all, 0, 1);  // -/
        pr_info("[AMBACV]: softreset GVP0\n");
        break;
    case CVCORE_GVP1:
        visorc_reset_gvp(mem_all, 1, 0);  // Reset
        visorc_reset_gvp(mem_all, 1, 1);  // -/
        pr_info("[AMBACV]: softreset GVP1\n");
        break;
    case CVCORE_FEX0:
    case CVCORE_FEX1:
        visorc_reset_fex(mem_all, 0);     // Reset
        visorc_reset_fex(mem_all, 1);     // -/
        pr_info("[AMBACV]: softreset FEX0+FEX1\n");
        break;
    case CVCORE_FMA0:
    case CVCORE_FMA1:
        visorc_reset_fma(mem_all, 0);     // Reset
        visorc_reset_fma(mem_all, 1);     // -/
        pr_info("[AMBACV]: softreset FMA0+FMA1\n");
        break;
    default:
        pr_info("[AMBACV]: softreset %d : FAILED (invalid unit_id)\n", unit_id);
        break;
    } /* switch (unit_id) */
}

uint32_t  visorc_get_curtime(void)
{
    return readl(visorc_reg(CORC_TIME));
} /* visorc_get_curtime() */

