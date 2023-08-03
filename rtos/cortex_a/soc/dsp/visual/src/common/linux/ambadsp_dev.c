/**
 *  @file ambadsp_dev.c
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
 *  @details Ambadsp dev APIs in Linux
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/of_reserved_mem.h>
#include <linux/of_device.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
#include <plat/chip.h>
#endif
#include <linux/mm.h>
#include <linux/of_irq.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include "dsp_osal.h"
#include "AmbaDSP_Def.h"
#include "ambadsp_ioctl.h"

MODULE_AUTHOR("Edgar Lai");
MODULE_LICENSE("GPL");

#define DSP_DEV_NAME    "ambadsp"

static unsigned int     ambadsp_major;
static struct class*    ambadsp_class;
static struct device*   ambadsp_device;

/* For UserSpace to map memory(phys2virt) */
static int ambadsp_mmap(struct file *filp, struct vm_area_struct *vma)
{
    int rval = 0;
    uint32_t Rval;
    uint32_t phys_Addr, buf_size;
    uint32_t is_cached = 0U, is_mapped = 0U;
(void)filp;

    phys_Addr = vma->vm_pgoff << 12;
    buf_size = vma->vm_end - vma->vm_start;

    Rval = dsp_query_buf_info(phys_Addr, buf_size, &is_cached);
    if (Rval == DSP_ERR_NONE) {
        if (is_mapped == 0U) {
            if (is_cached == 0U) {
                vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
                //vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);  // for register
            } else {
                // Default is cached
            }
            rval = remap_pfn_range(vma,
                                   vma->vm_start,
                                   vma->vm_pgoff,
                                   buf_size,
                                   vma->vm_page_prot);
            if (rval != 0) {
                printk("ambadsp_mmap : remap_pfn_range %d\n", rval);
            }
        } else {
            // LUT
        }
    } else {
        printk("ambadsp_mmap : dsp_query_buf_info() fail[0x%X]\n", Rval);
    }

    return rval;
}

static long ambadsp_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    uint32_t retcode = DSP_ERR_NONE;
    uint32_t nbytes = 0U;
    void *pVoidArg = NULL;
    void *pVirtAddr = NULL;

    switch (cmd) {
        /******************   Ultility  ******************/
        case AMBADSP_GET_VERSION:
        {
            nbytes = sizeof(AMBA_DSP_VERSION_INFO_s);
        }
            break;
        case AMBADSP_GET_DSP_BIN_ADDR:
        {
            nbytes = sizeof(AMBA_DSP_BIN_ADDR_s);
        }
            break;
        case AMBADSP_GET_DEFAULT_SYSCFG:
        case AMBADSP_MAIN_INIT:
        {
            nbytes = sizeof(AMBA_DSP_SYS_CONFIG_s);
        }
            break;
        case AMBADSP_MAIN_SUSPEND:
        case AMBADSP_MAIN_RESUME:
        {
            nbytes = 0U;
        }
            break;
        case AMBADSP_SET_WORK_MEMORY:
        {
            nbytes = sizeof(dsp_mem_t);
        }
            break;
        case AMBADSP_MAIN_MSG_PARSE:
        {
            nbytes = sizeof(uint32_t);
        }
            break;
        case AMBADSP_MAIN_WAIT_VIN_INT:
        case AMBADSP_MAIN_WAIT_VOUT_INT:
        case AMBADSP_MAIN_WAIT_FLAG:
        {
            nbytes = sizeof(dsp_wait_sig_t);
        }
            break;
        case AMBADSP_RESC_LIMIT_CONFIG:
        {
            nbytes = sizeof(dsp_resc_limit_cfg_t);
        }
            break;
        case AMBADSP_DEBUG_DUMP:
        {
            nbytes = sizeof(dsp_debug_dump_t);
        }
            break;
        case AMBADSP_CALC_HIER_BUF:
        {
            nbytes = sizeof(dsp_hier_buf_calc_t);
        }
            break;
        case AMBADSP_CALC_RAW_PITCH:
        {
            nbytes = sizeof(dsp_raw_pitch_calc_t);
        }
            break;
        case AMBADSP_PARLOAD_CONFIG:
        {
            nbytes = sizeof(dsp_partial_load_cfg_t);
        }
            break;
        case AMBADSP_PARLOAD_REGION_UNLOCK:
        {
            nbytes = sizeof(dsp_partial_load_unlock_t);
        }
            break;
        case AMBADSP_SYS_DRV_CFG:
        {
            nbytes = sizeof(dsp_sys_drv_cfg_t);
        }
            break;
        case AMBADSP_SET_PROTECT_AREA:
        {
            nbytes = sizeof(dsp_protect_area_t);
        }
            break;
        case AMBADSP_EXT_RAW_BUF_TBL_CFG:
        {
            nbytes = sizeof(dsp_ext_buf_tbl_t);
        }
            break;
        case AMBADSP_CALC_ENC_MV_BUF:
        {
            nbytes = sizeof(dsp_enc_mv_buf_calc_t);
        }
            break;
        case AMBADSP_PARSE_ENC_MV_BUF:
        {
            nbytes = sizeof(dsp_enc_mv_buf_parse_t);
        }
            break;
        case AMBADSP_CALC_VP_MSG_BUF:
        {
            nbytes = sizeof(dsp_vp_msg_buf_t);
        }
            break;
        case AMBADSP_GET_PROTECT_BUF:
        {
            nbytes = sizeof(AMBA_DSP_PROTECT_s);
        }
            break;
        case AMBADSP_GET_BUF_INFO:
        {
            nbytes = sizeof(dsp_buf_info_t);
        }
            break;
        case AMBADSP_CACHE_BUF_OPERATE:
        {
            nbytes = sizeof(dsp_cache_buf_op_t);
        }
            break;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
        case AMBADSP_MAIN_SAFETY_CHECK:
        {
            nbytes = sizeof(dsp_main_safety_check_t);
        }
            break;
        case AMBADSP_MAIN_SAFETY_CONFIG:
        {
            nbytes = sizeof(dsp_main_safety_config_t);
        }
            break;
#endif
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        case AMBADSP_SLICE_CFG_CALC:
        {
            nbytes = sizeof(dsp_slice_cfg_t);
        }
            break;
#endif
        case AMBADSP_GET_STATUS:
        {
            nbytes = sizeof(dsp_status_t);
        }
            break;
        case AMBADSP_EVENT_HDLR_CONFIG:
        {
            nbytes = sizeof(dsp_evnt_hdlr_cfg_t);
        }
            break;
        case AMBADSP_EVENT_HDLR_RESET:
        {
            nbytes = sizeof(uint16_t);
        }
            break;
        case AMBADSP_EVENT_HDLR_REGISTER:
        case AMBADSP_EVENT_HDLR_DEREGISTER:
        {
            nbytes = sizeof(dsp_evnt_hdlr_op_t);
        }
            break;
        case AMBADSP_EVENT_GET_DATA:
        {
            nbytes = sizeof(dsp_evnt_data_t);
        }
            break;
        case AMBADSP_EVENT_RELEASE_INFO_POOL:
            nbytes = sizeof(uint16_t);
            break;
        case AMBADSP_VOUT_RESET:
        {
            nbytes = sizeof(uint8_t);
        }
            break;
        case AMBADSP_VOUT_MIXER_CFG:
        {
            nbytes = sizeof(dsp_vout_mixer_cfg_t);
        }
            break;
        case AMBADSP_VOUT_MIXER_BGC_CFG:
        {
            nbytes = sizeof(dsp_vout_mixer_bgc_cfg_t);
        }
            break;
        case AMBADSP_VOUT_MIXER_HLC_CFG:
        {
            nbytes = sizeof(dsp_vout_mixer_hlc_cfg_t);
        }
            break;
        case AMBADSP_VOUT_MIXER_CSC_CFG:
        {
            nbytes = sizeof(dsp_vout_mixer_csc_cfg_t);
        }
            break;
        case AMBADSP_VOUT_MIXER_CSC_MATRIX_CFG:
        {
            nbytes = sizeof(dsp_vout_mixer_csc_matrix_cfg_t);
        }
            break;
        case AMBADSP_VOUT_MIXER_CTRL:
        {
            nbytes = sizeof(uint8_t);
        }
            break;
        case AMBADSP_VOUT_OSD_BUF_CFG:
        {
            nbytes = sizeof(dsp_vout_osd_buf_cfg_t);
        }
            break;
        case AMBADSP_VOUT_OSD_CTRL:
        {
            nbytes = sizeof(dsp_vout_osd_ctrl_t);
        }
            break;
        case AMBADSP_VOUT_DISP_CFG:
        {
            nbytes = sizeof(dsp_vout_disp_cfg_t);
        }
            break;
        case AMBADSP_VOUT_DISP_CTRL:
        {
            nbytes = sizeof(uint8_t);
        }
            break;
        case AMBADSP_VOUT_DISP_GAMMA_CFG:
        {
            nbytes = sizeof(dsp_vout_disp_gamma_cfg_t);
        }
            break;
        case AMBADSP_VOUT_DISP_GAMMA_CTRL:
        {
            nbytes = sizeof(dsp_vout_disp_gamma_ctrl_t);
        }
            break;
        case AMBADSP_VOUT_VIDEO_CFG:
        {
            nbytes = sizeof(dsp_vout_video_cfg_t);
        }
            break;
        case AMBADSP_VOUT_VIDEO_CTRL:
        {
            nbytes = sizeof(dsp_vout_video_ctrl_t);
        }
            break;
        case AMBADSP_VOUT_MIXER_BIND_CFG:
        {
            nbytes = sizeof(dsp_vout_mixer_bind_cfg_t);
        }
            break;
        case AMBADSP_VOUT_DVE_CFG:
        {
            nbytes = sizeof(dsp_vout_dve_cfg_t);
        }
            break;
        case AMBADSP_VOUT_DVE_CTRL:
        {
            nbytes = sizeof(uint8_t);
        }
            break;
        case AMBADSP_VOUT_DRV_CFG:
        {
            nbytes = sizeof(dsp_vout_drv_cfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_CFG:
        {
            nbytes = sizeof(dsp_liveview_cfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_CTRL:
        {
            nbytes = sizeof(dsp_liveview_ctrl_t);
        }
            break;
        case AMBADSP_LIVEVIEW_UPDATE_CFG:
        {
            nbytes = sizeof(dsp_liveview_update_cfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_UPDATE_ISOCFG:
        {
            nbytes = sizeof(dsp_liveview_update_isocfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_UPDATE_VINCFG:
        {
            nbytes = sizeof(dsp_liveview_update_vincfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_UPDATE_PYMDCFG:
        {
            nbytes = sizeof(dsp_liveview_update_pymdcfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_VIN_CAP_CFG:
        {
            nbytes = sizeof(dsp_liveview_vin_cap_cfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_VIN_POST_CFG:
        {
            nbytes = sizeof(dsp_liveview_vin_post_cfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_FEED_RAW:
        {
            nbytes = sizeof(dsp_liveview_feed_raw_data_t);
        }
            break;
        case AMBADSP_LIVEVIEW_FEED_YUV:
        {
            nbytes = sizeof(dsp_liveview_feed_yuv_data_t);
        }
            break;
        case AMBADSP_LIVEVIEW_YUVSTRM_SYNC:
        {
            nbytes = sizeof(dsp_liveview_yuvstrm_sync_t);
        }
            break;
        case AMBADSP_LIVEVIEW_UPDATE_GEOCFG:
        {
            nbytes = sizeof(dsp_liveview_update_geocfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_UPDATE_VZ_SRC:
        {
            nbytes = sizeof(dsp_liveview_update_vz_src_t);
        }
            break;
        case AMBADSP_LIVEVIEW_UPDATE_SIDEBAND:
        {
            nbytes = sizeof(dsp_liveview_update_sideband_t);
        }
            break;
        case AMBADSP_LIVEVIEW_SLICE_CFG:
        {
            nbytes = sizeof(dsp_liveview_slice_cfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_VZ_POSTPONE_CFG:
        {
            nbytes = sizeof(dsp_liveview_vz_postpone_cfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_UPDATE_VIN_STATE:
        {
            nbytes = sizeof(dsp_liveview_update_vin_state_t);
        }
            break;
        case AMBADSP_LIVEVIEW_PARSE_VP_MSG:
        {
            nbytes = sizeof(ULONG);
        }
            break;
        case AMBADSP_LIVEVIEW_VIN_DRV_CFG:
        {
            nbytes = sizeof(dsp_liveview_vin_drv_cfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_IK_DRV_CFG:
        {
            nbytes = sizeof(dsp_liveview_ik_drv_cfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_SLICE_CFG_CALC:
        {
            nbytes = sizeof(dsp_liveview_slice_cfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_GET_IDSPCFG:
        {
            nbytes = sizeof(dsp_liveview_idsp_cfg_t);
        }
            break;
        case AMBADSP_LIVEVIEW_SLOW_SHUTTER_CTRL:
        {
            nbytes = sizeof(dsp_liveview_slow_shutter_ctrl_t);
        }
            break;
        case AMBADSP_LIVEVIEW_DROP_REPEAT_CTRL:
        {
            nbytes = sizeof(dsp_liveview_drop_repeat_ctrl_t);
        }
            break;
        case AMBADSP_VIDEO_ENC_CFG:
        {
            nbytes = sizeof(dsp_video_enc_cfg_t);
        }
            break;
        case AMBADSP_VIDEO_ENC_START:
        {
            nbytes = sizeof(dsp_video_enc_start_t);
        }
            break;
        case AMBADSP_VIDEO_ENC_STOP:
        {
            nbytes = sizeof(dsp_video_enc_stop_t);
        }
            break;
        case AMBADSP_VIDEO_ENC_FRMRATE_CTRL:
        {
            nbytes = sizeof(dsp_video_enc_frmrate_ctrl_t);
        }
            break;
        case AMBADSP_VIDEO_ENC_BLEND_CTRL:
        {
            nbytes = sizeof(dsp_video_enc_blend_ctrl_t);
        }
            break;
        case AMBADSP_VIDEO_ENC_QUALITY_CTRL:
        {
            nbytes = sizeof(dsp_video_enc_quality_ctrl_t);
        }
            break;
        case AMBADSP_VIDEO_ENC_FEED_YUV:
        {
            nbytes = sizeof(dsp_video_enc_feed_yuv_data_t);
        }
            break;
        case AMBADSP_VIDEO_ENC_GRP_CFG:
        {
            nbytes = sizeof(AMBA_DSP_VIDEO_ENC_GROUP_s);
        }
            break;
        case AMBADSP_VIDEO_ENC_MV_CFG:
        {
            nbytes = sizeof(dsp_video_enc_mv_cfg_t);
        }
            break;
        case AMBADSP_VIDEO_ENC_DESC_FMT_CFG:
        {
            nbytes = sizeof(dsp_video_enc_desc_fmt_cfg_t);
        }
            break;
        case AMBADSP_STL_DATA_CAP_CFG:
        {
            nbytes = sizeof(dsp_stl_data_cap_cfg_t);
        }
            break;
        case AMBADSP_STL_UPDATE_CAP_BUF:
        {
            nbytes = sizeof(dsp_stl_update_cap_buf_t);
        }
            break;
        case AMBADSP_STL_DATA_CAP_CTRL:
        {
            nbytes = sizeof(dsp_stl_data_cap_ctrl_t);
        }
            break;
        case AMBADSP_STL_Y2Y:
        {
            nbytes = sizeof(dsp_stl_y2y_t);
        }
            break;
        case AMBADSP_STL_ENC_CTRL:
        {
            nbytes = sizeof(dsp_stl_enc_ctrl_t);
        }
            break;
        case AMBADSP_STL_YUV_EXTBUF_CACL:
        {
            nbytes = sizeof(dsp_stl_yuv_extbuf_clac_t);
        }
            break;
        case AMBADSP_STL_R2Y:
        {
            nbytes = sizeof(dsp_stl_r2y_t);
        }
            break;
        case AMBADSP_STL_R2R:
        {
            nbytes = sizeof(dsp_stl_r2r_t);
        }
            break;
        case AMBADSP_VIDEO_DEC_CFG:
        {
            nbytes = sizeof(dsp_video_dec_cfg_t);
        }
            break;
        case AMBADSP_VIDEO_DEC_START:
        {
            nbytes = sizeof(dsp_video_dec_start_t);
        }
            break;
        case AMBADSP_VIDEO_DEC_STOP:
        {
            nbytes = sizeof(dsp_video_dec_stop_t);
        }
            break;
        case AMBADSP_VIDEO_DEC_TRICKPLAY:
        {
            nbytes = sizeof(dsp_video_dec_trickplay_t);
        }
            break;
        case AMBADSP_VIDEO_DEC_BITS_UPDATE:
        {
            nbytes = sizeof(dsp_video_dec_bits_update_t);
        }
            break;
        case AMBADSP_VIDEO_DEC_POST_CTRL:
        {
            nbytes = sizeof(dsp_video_dec_post_ctrl_t);
        }
            break;
        case AMBADSP_STILL_DEC_START:
        {
            nbytes = sizeof(dsp_still_dec_start_t);
        }
            break;
        case AMBADSP_STILL_DEC_STOP:
            nbytes = 0;
            break;
        case AMBADSP_STILL_DEC_Y2Y:
        {
            nbytes = sizeof(dsp_still_dec_y2y_t);
        }
            break;
        case AMBADSP_STILL_DEC_BLEND:
        {
            nbytes = sizeof(dsp_still_dec_blend_t);
        }
            break;
        case AMBADSP_STILL_DEC_DISP_YUV:
        {
            nbytes = sizeof(dsp_still_dec_disp_yuv_t);
        }
            break;
        case AMBADSP_DIAG_CASE:
        {
            nbytes = sizeof(uint32_t);
        }
            break;
        default:
        {
            dsp_osal_printU5("[ERR] ambadsp_ioctl : unknown IOCTL 0x%X", cmd, 0U, 0U, 0U, 0U);
            retcode = DSP_ERR_0001;
        }
            break;
    }

    /* Copy from user */
    if ((retcode == DSP_ERR_NONE) &&
        (nbytes > 0U)) {
        pVirtAddr = kzalloc(nbytes, GFP_KERNEL);
        (void)dsp_osal_copy_from_user(pVirtAddr, (void *)arg, nbytes);
        dsp_osal_typecast(&pVoidArg, &pVirtAddr);
    }

    /* ioctl impl */
    if (retcode == DSP_ERR_NONE) {
        retcode = dsp_ioctl_impl(f, cmd, pVoidArg);
    }

    /* Copy to user */
    if ((retcode == DSP_ERR_NONE) &&
        (nbytes > 0U)) {
        (void)dsp_osal_copy_to_user((void *)arg, pVoidArg, nbytes);
    }

    if (pVirtAddr != NULL) {
        kfree(pVirtAddr);
    }

    return retcode;
}

static void ambadsp_of_init(struct device_node *np)
{
    (void)np;
    // DO NOTHING
}

static int ambadsp_release(struct inode *inode, struct file *file)
{
(void)inode;
(void)file;

    return 0;
}

static const struct file_operations ambadsp_fops = {
    .owner = THIS_MODULE,
    .mmap = ambadsp_mmap,
    .unlocked_ioctl = ambadsp_ioctl,
    .release = ambadsp_release,
};

//#define USE_OF_TABLE
#ifdef USE_OF_TABLE
static const struct of_device_id __ambadsp_of_table = {
    .compatible = "ambarella,dspdrv",
    .data = ambadsp_of_init,
};
#else
#define DTS_DSP_DRV_NODE_NAME   "/dspdrv"
#endif

static int  __init ambadsp_init(void)
{
#ifdef USE_OF_TABLE
    struct device_node *np = NULL;
#endif
    struct device_node *dsp_dev_node = NULL;
    const struct of_device_id *match;
    of_init_fn_1 init_func;

    printk("ambadsp: module init\n");

    ambadsp_major = register_chrdev(0, DSP_DEV_NAME, &ambadsp_fops);
    if (ambadsp_major < 0) {
        printk("ambadsp: failed to register device %d.\n", ambadsp_major);
        return ambadsp_major;
    }

    ambadsp_class = class_create(THIS_MODULE, DSP_DEV_NAME);
    if (IS_ERR(ambadsp_class)) {
        unregister_chrdev(ambadsp_major, DSP_DEV_NAME);
        printk("ambadsp: failed to create class.\n");
        return PTR_ERR(ambadsp_class);
    }

    ambadsp_device = device_create(ambadsp_class,
                                   NULL,
                                   MKDEV(ambadsp_major, 0),
                                   NULL,
                                   DSP_DEV_NAME);
    if (IS_ERR(ambadsp_device)) {
        class_destroy(ambadsp_class);
        unregister_chrdev(ambadsp_major, DSP_DEV_NAME);
        printk("ambadsp: falied to create device.\n");
        return PTR_ERR(ambadsp_device);
    }

#ifndef USE_OF_TABLE
    dsp_dev_node = of_find_node_by_path(DTS_DSP_DRV_NODE_NAME);
#else
    for_each_matching_node_and_match(np, &__ambadsp_of_table, &match) {
        if (!of_device_is_available(np)) {
            continue;
        }

        init_func = match->data;
        init_func(np);
        dsp_dev_node = np;
    }
#endif

    (void)dsp_osal_kernel_init(dsp_dev_node);

    return 0;
}

static void __exit ambadsp_exit(void)
{

    dsp_osal_kernel_exit();
    ambadsp_device->bus = NULL; /* Remove bus link */
    device_destroy(ambadsp_class, MKDEV(ambadsp_major, 0));
    class_destroy(ambadsp_class);
    unregister_chrdev(ambadsp_major, DSP_DEV_NAME);
    printk("ambadsp: module exit\n");
    return;
}


module_init(ambadsp_init);
module_exit(ambadsp_exit);

