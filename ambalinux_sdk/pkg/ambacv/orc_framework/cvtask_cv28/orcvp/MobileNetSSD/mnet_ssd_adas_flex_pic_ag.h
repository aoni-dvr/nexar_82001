/*
 * Copyright (c) 2017-2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use,
 * reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed
 * license agreement or nondisclosure agreement with Ambarella International LP
 * or its authorized affiliates. In the absence of such an agreement, you agree
 * to promptly notify and return this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MNET_SSD_ADAS_FLEX_PIC_AG_H_
#define _MNET_SSD_ADAS_FLEX_PIC_AG_H_

// Enable printing
#define CVTASK_PRINT

// Enable/disable poking
#undef __PVCN_1007__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1018__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1029__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1040__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1051__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1062__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1073__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1084__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1293__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1304__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1315__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1326__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1337__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1348__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1359__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1370__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1381__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1392__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1403__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1414__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1425__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1436__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1626__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1670__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1681__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1692__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1703__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1714__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1725__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1736__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1747__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1758__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1769__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1780__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1791__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1802__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1813__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1830__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1831__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1832__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1833__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1834__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1835__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1836__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1879__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1890__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1901__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1912__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1923__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1934__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1945__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1956__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1967__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1978__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1989__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2000__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2011__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2022__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2094__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2100__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2105__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2111__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2116__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2122__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2127__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2133__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2138__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2144__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2149__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2155__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2160__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2166__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2171__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2177__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2182__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2188__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2193__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2199__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2204__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2210__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2215__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2221__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2226__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2232__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2237__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2243__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2281__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2292__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2359__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2365__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2370__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2376__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2381__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2387__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2392__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2398__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2403__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2409__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2414__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2420__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2425__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2431__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2436__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2442__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2447__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2453__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2458__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2464__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2469__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2475__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2480__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2486__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2491__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2497__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2502__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2508__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2513__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2519__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2556__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2564__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2592__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2600__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2608__DRAM_PITCH_MODIFIABLE
#undef __PVCN_263__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2663__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2666__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2683__DRAM_PITCH_MODIFIABLE
#undef __PVCN_2686__DRAM_PITCH_MODIFIABLE
#undef __PVCN_270__DRAM_PITCH_MODIFIABLE
#undef __PVCN_332__DRAM_PITCH_MODIFIABLE
#undef __PVCN_343__DRAM_PITCH_MODIFIABLE
#undef __PVCN_354__DRAM_PITCH_MODIFIABLE
#undef __PVCN_365__DRAM_PITCH_MODIFIABLE
#undef __PVCN_376__DRAM_PITCH_MODIFIABLE
#undef __PVCN_387__DRAM_PITCH_MODIFIABLE
#undef __PVCN_398__DRAM_PITCH_MODIFIABLE
#undef __PVCN_409__DRAM_PITCH_MODIFIABLE
#undef __PVCN_543__DRAM_PITCH_MODIFIABLE
#undef __PVCN_549__DRAM_PITCH_MODIFIABLE
#undef __PVCN_554__DRAM_PITCH_MODIFIABLE
#undef __PVCN_560__DRAM_PITCH_MODIFIABLE
#undef __PVCN_565__DRAM_PITCH_MODIFIABLE
#undef __PVCN_571__DRAM_PITCH_MODIFIABLE
#undef __PVCN_576__DRAM_PITCH_MODIFIABLE
#undef __PVCN_582__DRAM_PITCH_MODIFIABLE
#undef __PVCN_587__DRAM_PITCH_MODIFIABLE
#undef __PVCN_593__DRAM_PITCH_MODIFIABLE
#undef __PVCN_598__DRAM_PITCH_MODIFIABLE
#undef __PVCN_604__DRAM_PITCH_MODIFIABLE
#undef __PVCN_609__DRAM_PITCH_MODIFIABLE
#undef __PVCN_615__DRAM_PITCH_MODIFIABLE
#undef __PVCN_620__DRAM_PITCH_MODIFIABLE
#undef __PVCN_626__DRAM_PITCH_MODIFIABLE
#undef __PVCN_631__DRAM_PITCH_MODIFIABLE
#undef __PVCN_637__DRAM_PITCH_MODIFIABLE
#undef __PVCN_642__DRAM_PITCH_MODIFIABLE
#undef __PVCN_648__DRAM_PITCH_MODIFIABLE
#undef __PVCN_653__DRAM_PITCH_MODIFIABLE
#undef __PVCN_659__DRAM_PITCH_MODIFIABLE
#undef __PVCN_664__DRAM_PITCH_MODIFIABLE
#undef __PVCN_670__DRAM_PITCH_MODIFIABLE
#undef __PVCN_675__DRAM_PITCH_MODIFIABLE
#undef __PVCN_681__DRAM_PITCH_MODIFIABLE
#undef __PVCN_686__DRAM_PITCH_MODIFIABLE
#undef __PVCN_692__DRAM_PITCH_MODIFIABLE
#undef __PVCN_703__DRAM_PITCH_MODIFIABLE
#undef __PVCN_714__DRAM_PITCH_MODIFIABLE
#undef __PVCN_719__DRAM_PITCH_MODIFIABLE
#undef __PVCN_725__DRAM_PITCH_MODIFIABLE
#undef __PVCN_755__DRAM_PITCH_MODIFIABLE
#undef __PVCN_756__DRAM_PITCH_MODIFIABLE
#undef __PVCN_757__DRAM_PITCH_MODIFIABLE
#undef __PVCN_789__DRAM_PITCH_MODIFIABLE
#undef __PVCN_800__DRAM_PITCH_MODIFIABLE
#undef __PVCN_811__DRAM_PITCH_MODIFIABLE
#undef __PVCN_822__DRAM_PITCH_MODIFIABLE
#undef __PVCN_833__DRAM_PITCH_MODIFIABLE
#undef __PVCN_844__DRAM_PITCH_MODIFIABLE
#undef __PVCN_855__DRAM_PITCH_MODIFIABLE
#undef __PVCN_866__DRAM_PITCH_MODIFIABLE
#undef __PVCN_877__DRAM_PITCH_MODIFIABLE
#undef __PVCN_888__DRAM_PITCH_MODIFIABLE
#undef __PVCN_899__DRAM_PITCH_MODIFIABLE
#undef __PVCN_910__DRAM_PITCH_MODIFIABLE
#undef __PVCN_925__DRAM_PITCH_MODIFIABLE
#undef __PVCN_926__DRAM_PITCH_MODIFIABLE
#undef __PVCN_927__DRAM_PITCH_MODIFIABLE
#undef __PVCN_928__DRAM_PITCH_MODIFIABLE
#undef __PVCN_929__DRAM_PITCH_MODIFIABLE
#undef __PVCN_930__DRAM_PITCH_MODIFIABLE
#undef __PVCN_963__DRAM_PITCH_MODIFIABLE
#undef __PVCN_974__DRAM_PITCH_MODIFIABLE
#undef __PVCN_985__DRAM_PITCH_MODIFIABLE
#undef __PVCN_996__DRAM_PITCH_MODIFIABLE
#undef __VCN_2726__DRAM_PITCH_MODIFIABLE
#undef CONV2_1__SEP_____BN_CONV2_1__SEP__SCALE_MUL___MULI___14_____BN_CONV2_1__SEP__SCALE_MUL_____SCALE___MULI___336_DRAM_PITCH_MODIFIABLE
#undef CONV3_1__DW_____BN_CONV3_1__DW__SCALE_MUL___MULI___23_____BN_CONV3_1__DW__SCALE_MUL_____SCALE___RELU3_1__DW_DRAM_PITCH_MODIFIABLE
#undef CONV4_1__SEP_____BN_CONV4_1__SEP__SCALE_MUL___MULI___38_____BN_CONV4_1__SEP__SCALE_MUL_____SCALE___MULI___344_DRAM_PITCH_MODIFIABLE
#undef CONV4_2__SEP_____BN_CONV4_2__SEP__SCALE_MUL___MULI___44_____BN_CONV4_2__SEP__SCALE_MUL_____SCALE___RELU4_2__SEP_DRAM_PITCH_MODIFIABLE
#undef CONV5_1__SEP_____BN_CONV5_1__SEP__SCALE_MUL___MULI___50_____BN_CONV5_1__SEP__SCALE_MUL_____SCALE___RELU5_1__SEP_DRAM_PITCH_MODIFIABLE
#undef CONV5_2__SEP_____BN_CONV5_2__SEP__SCALE_MUL___MULI___56_____BN_CONV5_2__SEP__SCALE_MUL_____SCALE___RELU5_2__SEP_DRAM_PITCH_MODIFIABLE
#undef CONV5_4__SEP_____BN_CONV5_4__SEP__SCALE_MUL___MULI___68_____BN_CONV5_4__SEP__SCALE_MUL_____SCALE___RELU5_4__SEP_DRAM_PITCH_MODIFIABLE
#undef CONV5_5__SEP_____BN_CONV5_5__SEP__SCALE_MUL___MULI___74_____BN_CONV5_5__SEP__SCALE_MUL_____SCALE___RELU5_5__SEP_DRAM_PITCH_MODIFIABLE
#undef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136______MULI___370___1_DRAM_PITCH_MODIFIABLE
#undef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136___CROP___320_DRAM_PITCH_MODIFIABLE
#undef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136___CROP___321_DRAM_PITCH_MODIFIABLE
#undef CONV5_5_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
#undef CONV5_5_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
#undef CONV5_6__DW_____BN_CONV5_6__DW__SCALE_MUL___MULI___79_____BN_CONV5_6__DW__SCALE_MUL_____SCALE___MULI___358_DRAM_PITCH_MODIFIABLE
#undef CONV5_6__DW_____BN_CONV5_6__DW__SCALE_MUL___MULI___79_____BN_CONV5_6__DW__SCALE_MUL_____SCALE___RELU5_6__DW_DRAM_PITCH_MODIFIABLE
#undef CONV6__DW_____BN_CONV6__DW__SCALE_MUL___MULI___85_____BN_CONV6__DW__SCALE_MUL_____SCALE___MULI___360_DRAM_PITCH_MODIFIABLE
#undef CONV6__SEP_____BN_CONV6__SEP__SCALE_MUL___MULI___88_____BN_CONV6__SEP__SCALE_MUL_____SCALE___RELU6__SEP_DRAM_PITCH_MODIFIABLE
#undef CONV6_MBOX_CONF___CONV2I___90______7_1_____BN_CONV7_1__SCALE_MUL___MULI___93_____BN_CONV7_1__SCALE_MUL_____SCALE___LOC___CONV2I___138______MULI___367___8___9_DRAM_PITCH_MODIFIABLE
#undef CONV6_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
#undef CONV7_2_____BN_CONV7_2__SCALE_MUL___MULI___96_____BN_CONV7_2__SCALE_MUL_____SCALE___RELU7_2_DRAM_PITCH_MODIFIABLE
#undef CONV7_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
#undef CONV7_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
#undef CONV8_2_____BN_CONV8_2__SCALE_MUL___MULI___104_____BN_CONV8_2__SCALE_MUL_____SCALE___RELU8_2_DRAM_PITCH_MODIFIABLE
#undef CONV8_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
#undef CONV8_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
#undef CONV9_2_____BN_CONV9_2__SCALE_MUL___MULI___112_____BN_CONV9_2__SCALE_MUL_____SCALE___RELU9_2_DRAM_PITCH_MODIFIABLE
#undef CONV9_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
#undef CONV9_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
#define DATA_DRAM_PITCH_MODIFIABLE
#define DATA_UV_DRAM_PITCH_MODIFIABLE
#undef MBOX_CONF_FLATTEN__DRAM_PITCH_MODIFIABLE
#undef MBOX_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
#undef MBOX_LOC_DRAM_PITCH_MODIFIABLE
#undef RELU7_1_DRAM_PITCH_MODIFIABLE

// for errcode_enum_t
#include <cvtask_interface.h>

/* Containers for memory buffers */
typedef struct mnet_ssd_adas_flex_pic_ag_DRAM_temporary_scratchpad         mnet_ssd_adas_flex_pic_ag_dram_t;
typedef struct mnet_ssd_adas_flex_pic_ag_CMEM_temporary_scratchpad         mnet_ssd_adas_flex_pic_ag_cmem_t;
typedef struct mnet_ssd_adas_flex_pic_ag_instance_private_storage          mnet_ssd_adas_flex_pic_ag_ips_t;
typedef struct mnet_ssd_adas_flex_pic_ag_instance_private_uncached_storage mnet_ssd_adas_flex_pic_ag_ipus_t;

/* Arguments to mnet_ssd_adas_flex_pic_ag_run/init() */
typedef struct mnet_ssd_adas_flex_pic_ag_required_fields {
    mnet_ssd_adas_flex_pic_ag_dram_t *DRAM_temporary_scratchpad;
    mnet_ssd_adas_flex_pic_ag_cmem_t *CMEM_temporary_scratchpad;
    mnet_ssd_adas_flex_pic_ag_ipus_t *instance_private_uncached_storage;
    uint32_t data_addr;
    uint32_t data_uv_addr;
    uint32_t mbox_loc_addr;
    uint32_t mbox_conf_flatten_addr;
} mnet_ssd_adas_flex_pic_ag_required_fields_t;

/* Arguments to mnet_ssd_adas_flex_pic_ag_run/reset() */
typedef struct mnet_ssd_adas_flex_pic_ag_optional_fields {
#ifdef __PVCN_1007__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1007__dram_pitch_enable;
    uint32_t __pvcn_1007__dram_pitch_value;
#endif
#ifdef __PVCN_1018__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1018__dram_pitch_enable;
    uint32_t __pvcn_1018__dram_pitch_value;
#endif
#ifdef __PVCN_1029__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1029__dram_pitch_enable;
    uint32_t __pvcn_1029__dram_pitch_value;
#endif
#ifdef __PVCN_1040__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1040__dram_pitch_enable;
    uint32_t __pvcn_1040__dram_pitch_value;
#endif
#ifdef __PVCN_1051__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1051__dram_pitch_enable;
    uint32_t __pvcn_1051__dram_pitch_value;
#endif
#ifdef __PVCN_1062__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1062__dram_pitch_enable;
    uint32_t __pvcn_1062__dram_pitch_value;
#endif
#ifdef __PVCN_1073__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1073__dram_pitch_enable;
    uint32_t __pvcn_1073__dram_pitch_value;
#endif
#ifdef __PVCN_1084__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1084__dram_pitch_enable;
    uint32_t __pvcn_1084__dram_pitch_value;
#endif
#ifdef __PVCN_1293__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1293__dram_pitch_enable;
    uint32_t __pvcn_1293__dram_pitch_value;
#endif
#ifdef __PVCN_1304__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1304__dram_pitch_enable;
    uint32_t __pvcn_1304__dram_pitch_value;
#endif
#ifdef __PVCN_1315__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1315__dram_pitch_enable;
    uint32_t __pvcn_1315__dram_pitch_value;
#endif
#ifdef __PVCN_1326__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1326__dram_pitch_enable;
    uint32_t __pvcn_1326__dram_pitch_value;
#endif
#ifdef __PVCN_1337__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1337__dram_pitch_enable;
    uint32_t __pvcn_1337__dram_pitch_value;
#endif
#ifdef __PVCN_1348__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1348__dram_pitch_enable;
    uint32_t __pvcn_1348__dram_pitch_value;
#endif
#ifdef __PVCN_1359__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1359__dram_pitch_enable;
    uint32_t __pvcn_1359__dram_pitch_value;
#endif
#ifdef __PVCN_1370__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1370__dram_pitch_enable;
    uint32_t __pvcn_1370__dram_pitch_value;
#endif
#ifdef __PVCN_1381__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1381__dram_pitch_enable;
    uint32_t __pvcn_1381__dram_pitch_value;
#endif
#ifdef __PVCN_1392__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1392__dram_pitch_enable;
    uint32_t __pvcn_1392__dram_pitch_value;
#endif
#ifdef __PVCN_1403__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1403__dram_pitch_enable;
    uint32_t __pvcn_1403__dram_pitch_value;
#endif
#ifdef __PVCN_1414__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1414__dram_pitch_enable;
    uint32_t __pvcn_1414__dram_pitch_value;
#endif
#ifdef __PVCN_1425__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1425__dram_pitch_enable;
    uint32_t __pvcn_1425__dram_pitch_value;
#endif
#ifdef __PVCN_1436__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1436__dram_pitch_enable;
    uint32_t __pvcn_1436__dram_pitch_value;
#endif
#ifdef __PVCN_1626__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1626__dram_pitch_enable;
    uint32_t __pvcn_1626__dram_pitch_value;
#endif
#ifdef __PVCN_1670__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1670__dram_pitch_enable;
    uint32_t __pvcn_1670__dram_pitch_value;
#endif
#ifdef __PVCN_1681__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1681__dram_pitch_enable;
    uint32_t __pvcn_1681__dram_pitch_value;
#endif
#ifdef __PVCN_1692__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1692__dram_pitch_enable;
    uint32_t __pvcn_1692__dram_pitch_value;
#endif
#ifdef __PVCN_1703__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1703__dram_pitch_enable;
    uint32_t __pvcn_1703__dram_pitch_value;
#endif
#ifdef __PVCN_1714__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1714__dram_pitch_enable;
    uint32_t __pvcn_1714__dram_pitch_value;
#endif
#ifdef __PVCN_1725__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1725__dram_pitch_enable;
    uint32_t __pvcn_1725__dram_pitch_value;
#endif
#ifdef __PVCN_1736__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1736__dram_pitch_enable;
    uint32_t __pvcn_1736__dram_pitch_value;
#endif
#ifdef __PVCN_1747__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1747__dram_pitch_enable;
    uint32_t __pvcn_1747__dram_pitch_value;
#endif
#ifdef __PVCN_1758__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1758__dram_pitch_enable;
    uint32_t __pvcn_1758__dram_pitch_value;
#endif
#ifdef __PVCN_1769__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1769__dram_pitch_enable;
    uint32_t __pvcn_1769__dram_pitch_value;
#endif
#ifdef __PVCN_1780__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1780__dram_pitch_enable;
    uint32_t __pvcn_1780__dram_pitch_value;
#endif
#ifdef __PVCN_1791__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1791__dram_pitch_enable;
    uint32_t __pvcn_1791__dram_pitch_value;
#endif
#ifdef __PVCN_1802__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1802__dram_pitch_enable;
    uint32_t __pvcn_1802__dram_pitch_value;
#endif
#ifdef __PVCN_1813__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1813__dram_pitch_enable;
    uint32_t __pvcn_1813__dram_pitch_value;
#endif
#ifdef __PVCN_1830__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1830__dram_pitch_enable;
    uint32_t __pvcn_1830__dram_pitch_value;
#endif
#ifdef __PVCN_1831__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1831__dram_pitch_enable;
    uint32_t __pvcn_1831__dram_pitch_value;
#endif
#ifdef __PVCN_1832__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1832__dram_pitch_enable;
    uint32_t __pvcn_1832__dram_pitch_value;
#endif
#ifdef __PVCN_1833__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1833__dram_pitch_enable;
    uint32_t __pvcn_1833__dram_pitch_value;
#endif
#ifdef __PVCN_1834__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1834__dram_pitch_enable;
    uint32_t __pvcn_1834__dram_pitch_value;
#endif
#ifdef __PVCN_1835__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1835__dram_pitch_enable;
    uint32_t __pvcn_1835__dram_pitch_value;
#endif
#ifdef __PVCN_1836__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1836__dram_pitch_enable;
    uint32_t __pvcn_1836__dram_pitch_value;
#endif
#ifdef __PVCN_1879__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1879__dram_pitch_enable;
    uint32_t __pvcn_1879__dram_pitch_value;
#endif
#ifdef __PVCN_1890__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1890__dram_pitch_enable;
    uint32_t __pvcn_1890__dram_pitch_value;
#endif
#ifdef __PVCN_1901__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1901__dram_pitch_enable;
    uint32_t __pvcn_1901__dram_pitch_value;
#endif
#ifdef __PVCN_1912__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1912__dram_pitch_enable;
    uint32_t __pvcn_1912__dram_pitch_value;
#endif
#ifdef __PVCN_1923__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1923__dram_pitch_enable;
    uint32_t __pvcn_1923__dram_pitch_value;
#endif
#ifdef __PVCN_1934__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1934__dram_pitch_enable;
    uint32_t __pvcn_1934__dram_pitch_value;
#endif
#ifdef __PVCN_1945__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1945__dram_pitch_enable;
    uint32_t __pvcn_1945__dram_pitch_value;
#endif
#ifdef __PVCN_1956__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1956__dram_pitch_enable;
    uint32_t __pvcn_1956__dram_pitch_value;
#endif
#ifdef __PVCN_1967__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1967__dram_pitch_enable;
    uint32_t __pvcn_1967__dram_pitch_value;
#endif
#ifdef __PVCN_1978__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1978__dram_pitch_enable;
    uint32_t __pvcn_1978__dram_pitch_value;
#endif
#ifdef __PVCN_1989__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1989__dram_pitch_enable;
    uint32_t __pvcn_1989__dram_pitch_value;
#endif
#ifdef __PVCN_2000__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2000__dram_pitch_enable;
    uint32_t __pvcn_2000__dram_pitch_value;
#endif
#ifdef __PVCN_2011__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2011__dram_pitch_enable;
    uint32_t __pvcn_2011__dram_pitch_value;
#endif
#ifdef __PVCN_2022__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2022__dram_pitch_enable;
    uint32_t __pvcn_2022__dram_pitch_value;
#endif
#ifdef __PVCN_2094__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2094__dram_pitch_enable;
    uint32_t __pvcn_2094__dram_pitch_value;
#endif
#ifdef __PVCN_2100__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2100__dram_pitch_enable;
    uint32_t __pvcn_2100__dram_pitch_value;
#endif
#ifdef __PVCN_2105__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2105__dram_pitch_enable;
    uint32_t __pvcn_2105__dram_pitch_value;
#endif
#ifdef __PVCN_2111__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2111__dram_pitch_enable;
    uint32_t __pvcn_2111__dram_pitch_value;
#endif
#ifdef __PVCN_2116__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2116__dram_pitch_enable;
    uint32_t __pvcn_2116__dram_pitch_value;
#endif
#ifdef __PVCN_2122__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2122__dram_pitch_enable;
    uint32_t __pvcn_2122__dram_pitch_value;
#endif
#ifdef __PVCN_2127__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2127__dram_pitch_enable;
    uint32_t __pvcn_2127__dram_pitch_value;
#endif
#ifdef __PVCN_2133__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2133__dram_pitch_enable;
    uint32_t __pvcn_2133__dram_pitch_value;
#endif
#ifdef __PVCN_2138__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2138__dram_pitch_enable;
    uint32_t __pvcn_2138__dram_pitch_value;
#endif
#ifdef __PVCN_2144__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2144__dram_pitch_enable;
    uint32_t __pvcn_2144__dram_pitch_value;
#endif
#ifdef __PVCN_2149__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2149__dram_pitch_enable;
    uint32_t __pvcn_2149__dram_pitch_value;
#endif
#ifdef __PVCN_2155__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2155__dram_pitch_enable;
    uint32_t __pvcn_2155__dram_pitch_value;
#endif
#ifdef __PVCN_2160__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2160__dram_pitch_enable;
    uint32_t __pvcn_2160__dram_pitch_value;
#endif
#ifdef __PVCN_2166__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2166__dram_pitch_enable;
    uint32_t __pvcn_2166__dram_pitch_value;
#endif
#ifdef __PVCN_2171__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2171__dram_pitch_enable;
    uint32_t __pvcn_2171__dram_pitch_value;
#endif
#ifdef __PVCN_2177__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2177__dram_pitch_enable;
    uint32_t __pvcn_2177__dram_pitch_value;
#endif
#ifdef __PVCN_2182__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2182__dram_pitch_enable;
    uint32_t __pvcn_2182__dram_pitch_value;
#endif
#ifdef __PVCN_2188__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2188__dram_pitch_enable;
    uint32_t __pvcn_2188__dram_pitch_value;
#endif
#ifdef __PVCN_2193__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2193__dram_pitch_enable;
    uint32_t __pvcn_2193__dram_pitch_value;
#endif
#ifdef __PVCN_2199__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2199__dram_pitch_enable;
    uint32_t __pvcn_2199__dram_pitch_value;
#endif
#ifdef __PVCN_2204__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2204__dram_pitch_enable;
    uint32_t __pvcn_2204__dram_pitch_value;
#endif
#ifdef __PVCN_2210__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2210__dram_pitch_enable;
    uint32_t __pvcn_2210__dram_pitch_value;
#endif
#ifdef __PVCN_2215__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2215__dram_pitch_enable;
    uint32_t __pvcn_2215__dram_pitch_value;
#endif
#ifdef __PVCN_2221__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2221__dram_pitch_enable;
    uint32_t __pvcn_2221__dram_pitch_value;
#endif
#ifdef __PVCN_2226__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2226__dram_pitch_enable;
    uint32_t __pvcn_2226__dram_pitch_value;
#endif
#ifdef __PVCN_2232__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2232__dram_pitch_enable;
    uint32_t __pvcn_2232__dram_pitch_value;
#endif
#ifdef __PVCN_2237__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2237__dram_pitch_enable;
    uint32_t __pvcn_2237__dram_pitch_value;
#endif
#ifdef __PVCN_2243__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2243__dram_pitch_enable;
    uint32_t __pvcn_2243__dram_pitch_value;
#endif
#ifdef __PVCN_2281__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2281__dram_pitch_enable;
    uint32_t __pvcn_2281__dram_pitch_value;
#endif
#ifdef __PVCN_2292__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2292__dram_pitch_enable;
    uint32_t __pvcn_2292__dram_pitch_value;
#endif
#ifdef __PVCN_2359__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2359__dram_pitch_enable;
    uint32_t __pvcn_2359__dram_pitch_value;
#endif
#ifdef __PVCN_2365__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2365__dram_pitch_enable;
    uint32_t __pvcn_2365__dram_pitch_value;
#endif
#ifdef __PVCN_2370__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2370__dram_pitch_enable;
    uint32_t __pvcn_2370__dram_pitch_value;
#endif
#ifdef __PVCN_2376__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2376__dram_pitch_enable;
    uint32_t __pvcn_2376__dram_pitch_value;
#endif
#ifdef __PVCN_2381__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2381__dram_pitch_enable;
    uint32_t __pvcn_2381__dram_pitch_value;
#endif
#ifdef __PVCN_2387__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2387__dram_pitch_enable;
    uint32_t __pvcn_2387__dram_pitch_value;
#endif
#ifdef __PVCN_2392__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2392__dram_pitch_enable;
    uint32_t __pvcn_2392__dram_pitch_value;
#endif
#ifdef __PVCN_2398__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2398__dram_pitch_enable;
    uint32_t __pvcn_2398__dram_pitch_value;
#endif
#ifdef __PVCN_2403__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2403__dram_pitch_enable;
    uint32_t __pvcn_2403__dram_pitch_value;
#endif
#ifdef __PVCN_2409__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2409__dram_pitch_enable;
    uint32_t __pvcn_2409__dram_pitch_value;
#endif
#ifdef __PVCN_2414__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2414__dram_pitch_enable;
    uint32_t __pvcn_2414__dram_pitch_value;
#endif
#ifdef __PVCN_2420__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2420__dram_pitch_enable;
    uint32_t __pvcn_2420__dram_pitch_value;
#endif
#ifdef __PVCN_2425__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2425__dram_pitch_enable;
    uint32_t __pvcn_2425__dram_pitch_value;
#endif
#ifdef __PVCN_2431__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2431__dram_pitch_enable;
    uint32_t __pvcn_2431__dram_pitch_value;
#endif
#ifdef __PVCN_2436__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2436__dram_pitch_enable;
    uint32_t __pvcn_2436__dram_pitch_value;
#endif
#ifdef __PVCN_2442__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2442__dram_pitch_enable;
    uint32_t __pvcn_2442__dram_pitch_value;
#endif
#ifdef __PVCN_2447__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2447__dram_pitch_enable;
    uint32_t __pvcn_2447__dram_pitch_value;
#endif
#ifdef __PVCN_2453__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2453__dram_pitch_enable;
    uint32_t __pvcn_2453__dram_pitch_value;
#endif
#ifdef __PVCN_2458__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2458__dram_pitch_enable;
    uint32_t __pvcn_2458__dram_pitch_value;
#endif
#ifdef __PVCN_2464__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2464__dram_pitch_enable;
    uint32_t __pvcn_2464__dram_pitch_value;
#endif
#ifdef __PVCN_2469__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2469__dram_pitch_enable;
    uint32_t __pvcn_2469__dram_pitch_value;
#endif
#ifdef __PVCN_2475__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2475__dram_pitch_enable;
    uint32_t __pvcn_2475__dram_pitch_value;
#endif
#ifdef __PVCN_2480__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2480__dram_pitch_enable;
    uint32_t __pvcn_2480__dram_pitch_value;
#endif
#ifdef __PVCN_2486__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2486__dram_pitch_enable;
    uint32_t __pvcn_2486__dram_pitch_value;
#endif
#ifdef __PVCN_2491__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2491__dram_pitch_enable;
    uint32_t __pvcn_2491__dram_pitch_value;
#endif
#ifdef __PVCN_2497__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2497__dram_pitch_enable;
    uint32_t __pvcn_2497__dram_pitch_value;
#endif
#ifdef __PVCN_2502__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2502__dram_pitch_enable;
    uint32_t __pvcn_2502__dram_pitch_value;
#endif
#ifdef __PVCN_2508__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2508__dram_pitch_enable;
    uint32_t __pvcn_2508__dram_pitch_value;
#endif
#ifdef __PVCN_2513__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2513__dram_pitch_enable;
    uint32_t __pvcn_2513__dram_pitch_value;
#endif
#ifdef __PVCN_2519__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2519__dram_pitch_enable;
    uint32_t __pvcn_2519__dram_pitch_value;
#endif
#ifdef __PVCN_2556__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2556__dram_pitch_enable;
    uint32_t __pvcn_2556__dram_pitch_value;
#endif
#ifdef __PVCN_2564__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2564__dram_pitch_enable;
    uint32_t __pvcn_2564__dram_pitch_value;
#endif
#ifdef __PVCN_2592__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2592__dram_pitch_enable;
    uint32_t __pvcn_2592__dram_pitch_value;
#endif
#ifdef __PVCN_2600__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2600__dram_pitch_enable;
    uint32_t __pvcn_2600__dram_pitch_value;
#endif
#ifdef __PVCN_2608__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2608__dram_pitch_enable;
    uint32_t __pvcn_2608__dram_pitch_value;
#endif
#ifdef __PVCN_263__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_263__dram_pitch_enable;
    uint32_t __pvcn_263__dram_pitch_value;
#endif
#ifdef __PVCN_2663__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2663__dram_pitch_enable;
    uint32_t __pvcn_2663__dram_pitch_value;
#endif
#ifdef __PVCN_2666__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2666__dram_pitch_enable;
    uint32_t __pvcn_2666__dram_pitch_value;
#endif
#ifdef __PVCN_2683__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2683__dram_pitch_enable;
    uint32_t __pvcn_2683__dram_pitch_value;
#endif
#ifdef __PVCN_2686__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_2686__dram_pitch_enable;
    uint32_t __pvcn_2686__dram_pitch_value;
#endif
#ifdef __PVCN_270__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_270__dram_pitch_enable;
    uint32_t __pvcn_270__dram_pitch_value;
#endif
#ifdef __PVCN_332__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_332__dram_pitch_enable;
    uint32_t __pvcn_332__dram_pitch_value;
#endif
#ifdef __PVCN_343__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_343__dram_pitch_enable;
    uint32_t __pvcn_343__dram_pitch_value;
#endif
#ifdef __PVCN_354__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_354__dram_pitch_enable;
    uint32_t __pvcn_354__dram_pitch_value;
#endif
#ifdef __PVCN_365__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_365__dram_pitch_enable;
    uint32_t __pvcn_365__dram_pitch_value;
#endif
#ifdef __PVCN_376__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_376__dram_pitch_enable;
    uint32_t __pvcn_376__dram_pitch_value;
#endif
#ifdef __PVCN_387__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_387__dram_pitch_enable;
    uint32_t __pvcn_387__dram_pitch_value;
#endif
#ifdef __PVCN_398__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_398__dram_pitch_enable;
    uint32_t __pvcn_398__dram_pitch_value;
#endif
#ifdef __PVCN_409__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_409__dram_pitch_enable;
    uint32_t __pvcn_409__dram_pitch_value;
#endif
#ifdef __PVCN_543__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_543__dram_pitch_enable;
    uint32_t __pvcn_543__dram_pitch_value;
#endif
#ifdef __PVCN_549__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_549__dram_pitch_enable;
    uint32_t __pvcn_549__dram_pitch_value;
#endif
#ifdef __PVCN_554__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_554__dram_pitch_enable;
    uint32_t __pvcn_554__dram_pitch_value;
#endif
#ifdef __PVCN_560__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_560__dram_pitch_enable;
    uint32_t __pvcn_560__dram_pitch_value;
#endif
#ifdef __PVCN_565__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_565__dram_pitch_enable;
    uint32_t __pvcn_565__dram_pitch_value;
#endif
#ifdef __PVCN_571__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_571__dram_pitch_enable;
    uint32_t __pvcn_571__dram_pitch_value;
#endif
#ifdef __PVCN_576__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_576__dram_pitch_enable;
    uint32_t __pvcn_576__dram_pitch_value;
#endif
#ifdef __PVCN_582__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_582__dram_pitch_enable;
    uint32_t __pvcn_582__dram_pitch_value;
#endif
#ifdef __PVCN_587__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_587__dram_pitch_enable;
    uint32_t __pvcn_587__dram_pitch_value;
#endif
#ifdef __PVCN_593__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_593__dram_pitch_enable;
    uint32_t __pvcn_593__dram_pitch_value;
#endif
#ifdef __PVCN_598__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_598__dram_pitch_enable;
    uint32_t __pvcn_598__dram_pitch_value;
#endif
#ifdef __PVCN_604__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_604__dram_pitch_enable;
    uint32_t __pvcn_604__dram_pitch_value;
#endif
#ifdef __PVCN_609__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_609__dram_pitch_enable;
    uint32_t __pvcn_609__dram_pitch_value;
#endif
#ifdef __PVCN_615__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_615__dram_pitch_enable;
    uint32_t __pvcn_615__dram_pitch_value;
#endif
#ifdef __PVCN_620__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_620__dram_pitch_enable;
    uint32_t __pvcn_620__dram_pitch_value;
#endif
#ifdef __PVCN_626__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_626__dram_pitch_enable;
    uint32_t __pvcn_626__dram_pitch_value;
#endif
#ifdef __PVCN_631__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_631__dram_pitch_enable;
    uint32_t __pvcn_631__dram_pitch_value;
#endif
#ifdef __PVCN_637__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_637__dram_pitch_enable;
    uint32_t __pvcn_637__dram_pitch_value;
#endif
#ifdef __PVCN_642__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_642__dram_pitch_enable;
    uint32_t __pvcn_642__dram_pitch_value;
#endif
#ifdef __PVCN_648__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_648__dram_pitch_enable;
    uint32_t __pvcn_648__dram_pitch_value;
#endif
#ifdef __PVCN_653__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_653__dram_pitch_enable;
    uint32_t __pvcn_653__dram_pitch_value;
#endif
#ifdef __PVCN_659__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_659__dram_pitch_enable;
    uint32_t __pvcn_659__dram_pitch_value;
#endif
#ifdef __PVCN_664__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_664__dram_pitch_enable;
    uint32_t __pvcn_664__dram_pitch_value;
#endif
#ifdef __PVCN_670__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_670__dram_pitch_enable;
    uint32_t __pvcn_670__dram_pitch_value;
#endif
#ifdef __PVCN_675__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_675__dram_pitch_enable;
    uint32_t __pvcn_675__dram_pitch_value;
#endif
#ifdef __PVCN_681__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_681__dram_pitch_enable;
    uint32_t __pvcn_681__dram_pitch_value;
#endif
#ifdef __PVCN_686__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_686__dram_pitch_enable;
    uint32_t __pvcn_686__dram_pitch_value;
#endif
#ifdef __PVCN_692__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_692__dram_pitch_enable;
    uint32_t __pvcn_692__dram_pitch_value;
#endif
#ifdef __PVCN_703__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_703__dram_pitch_enable;
    uint32_t __pvcn_703__dram_pitch_value;
#endif
#ifdef __PVCN_714__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_714__dram_pitch_enable;
    uint32_t __pvcn_714__dram_pitch_value;
#endif
#ifdef __PVCN_719__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_719__dram_pitch_enable;
    uint32_t __pvcn_719__dram_pitch_value;
#endif
#ifdef __PVCN_725__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_725__dram_pitch_enable;
    uint32_t __pvcn_725__dram_pitch_value;
#endif
#ifdef __PVCN_755__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_755__dram_pitch_enable;
    uint32_t __pvcn_755__dram_pitch_value;
#endif
#ifdef __PVCN_756__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_756__dram_pitch_enable;
    uint32_t __pvcn_756__dram_pitch_value;
#endif
#ifdef __PVCN_757__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_757__dram_pitch_enable;
    uint32_t __pvcn_757__dram_pitch_value;
#endif
#ifdef __PVCN_789__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_789__dram_pitch_enable;
    uint32_t __pvcn_789__dram_pitch_value;
#endif
#ifdef __PVCN_800__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_800__dram_pitch_enable;
    uint32_t __pvcn_800__dram_pitch_value;
#endif
#ifdef __PVCN_811__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_811__dram_pitch_enable;
    uint32_t __pvcn_811__dram_pitch_value;
#endif
#ifdef __PVCN_822__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_822__dram_pitch_enable;
    uint32_t __pvcn_822__dram_pitch_value;
#endif
#ifdef __PVCN_833__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_833__dram_pitch_enable;
    uint32_t __pvcn_833__dram_pitch_value;
#endif
#ifdef __PVCN_844__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_844__dram_pitch_enable;
    uint32_t __pvcn_844__dram_pitch_value;
#endif
#ifdef __PVCN_855__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_855__dram_pitch_enable;
    uint32_t __pvcn_855__dram_pitch_value;
#endif
#ifdef __PVCN_866__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_866__dram_pitch_enable;
    uint32_t __pvcn_866__dram_pitch_value;
#endif
#ifdef __PVCN_877__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_877__dram_pitch_enable;
    uint32_t __pvcn_877__dram_pitch_value;
#endif
#ifdef __PVCN_888__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_888__dram_pitch_enable;
    uint32_t __pvcn_888__dram_pitch_value;
#endif
#ifdef __PVCN_899__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_899__dram_pitch_enable;
    uint32_t __pvcn_899__dram_pitch_value;
#endif
#ifdef __PVCN_910__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_910__dram_pitch_enable;
    uint32_t __pvcn_910__dram_pitch_value;
#endif
#ifdef __PVCN_925__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_925__dram_pitch_enable;
    uint32_t __pvcn_925__dram_pitch_value;
#endif
#ifdef __PVCN_926__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_926__dram_pitch_enable;
    uint32_t __pvcn_926__dram_pitch_value;
#endif
#ifdef __PVCN_927__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_927__dram_pitch_enable;
    uint32_t __pvcn_927__dram_pitch_value;
#endif
#ifdef __PVCN_928__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_928__dram_pitch_enable;
    uint32_t __pvcn_928__dram_pitch_value;
#endif
#ifdef __PVCN_929__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_929__dram_pitch_enable;
    uint32_t __pvcn_929__dram_pitch_value;
#endif
#ifdef __PVCN_930__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_930__dram_pitch_enable;
    uint32_t __pvcn_930__dram_pitch_value;
#endif
#ifdef __PVCN_963__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_963__dram_pitch_enable;
    uint32_t __pvcn_963__dram_pitch_value;
#endif
#ifdef __PVCN_974__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_974__dram_pitch_enable;
    uint32_t __pvcn_974__dram_pitch_value;
#endif
#ifdef __PVCN_985__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_985__dram_pitch_enable;
    uint32_t __pvcn_985__dram_pitch_value;
#endif
#ifdef __PVCN_996__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_996__dram_pitch_enable;
    uint32_t __pvcn_996__dram_pitch_value;
#endif
#ifdef __VCN_2726__DRAM_PITCH_MODIFIABLE
    uint32_t __vcn_2726__dram_pitch_enable;
    uint32_t __vcn_2726__dram_pitch_value;
#endif
#ifdef CONV2_1__SEP_____BN_CONV2_1__SEP__SCALE_MUL___MULI___14_____BN_CONV2_1__SEP__SCALE_MUL_____SCALE___MULI___336_DRAM_PITCH_MODIFIABLE
    uint32_t conv2_1__sep_____bn_conv2_1__sep__scale_mul___muli___14_____bn_conv2_1__sep__scale_mul_____scale___muli___336_dram_pitch_enable;
    uint32_t conv2_1__sep_____bn_conv2_1__sep__scale_mul___muli___14_____bn_conv2_1__sep__scale_mul_____scale___muli___336_dram_pitch_value;
#endif
#ifdef CONV3_1__DW_____BN_CONV3_1__DW__SCALE_MUL___MULI___23_____BN_CONV3_1__DW__SCALE_MUL_____SCALE___RELU3_1__DW_DRAM_PITCH_MODIFIABLE
    uint32_t conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dram_pitch_enable;
    uint32_t conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dram_pitch_value;
#endif
#ifdef CONV4_1__SEP_____BN_CONV4_1__SEP__SCALE_MUL___MULI___38_____BN_CONV4_1__SEP__SCALE_MUL_____SCALE___MULI___344_DRAM_PITCH_MODIFIABLE
    uint32_t conv4_1__sep_____bn_conv4_1__sep__scale_mul___muli___38_____bn_conv4_1__sep__scale_mul_____scale___muli___344_dram_pitch_enable;
    uint32_t conv4_1__sep_____bn_conv4_1__sep__scale_mul___muli___38_____bn_conv4_1__sep__scale_mul_____scale___muli___344_dram_pitch_value;
#endif
#ifdef CONV4_2__SEP_____BN_CONV4_2__SEP__SCALE_MUL___MULI___44_____BN_CONV4_2__SEP__SCALE_MUL_____SCALE___RELU4_2__SEP_DRAM_PITCH_MODIFIABLE
    uint32_t conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dram_pitch_enable;
    uint32_t conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dram_pitch_value;
#endif
#ifdef CONV5_1__SEP_____BN_CONV5_1__SEP__SCALE_MUL___MULI___50_____BN_CONV5_1__SEP__SCALE_MUL_____SCALE___RELU5_1__SEP_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_1__sep_____bn_conv5_1__sep__scale_mul___muli___50_____bn_conv5_1__sep__scale_mul_____scale___relu5_1__sep_dram_pitch_enable;
    uint32_t conv5_1__sep_____bn_conv5_1__sep__scale_mul___muli___50_____bn_conv5_1__sep__scale_mul_____scale___relu5_1__sep_dram_pitch_value;
#endif
#ifdef CONV5_2__SEP_____BN_CONV5_2__SEP__SCALE_MUL___MULI___56_____BN_CONV5_2__SEP__SCALE_MUL_____SCALE___RELU5_2__SEP_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dram_pitch_enable;
    uint32_t conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dram_pitch_value;
#endif
#ifdef CONV5_4__SEP_____BN_CONV5_4__SEP__SCALE_MUL___MULI___68_____BN_CONV5_4__SEP__SCALE_MUL_____SCALE___RELU5_4__SEP_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dram_pitch_enable;
    uint32_t conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dram_pitch_value;
#endif
#ifdef CONV5_5__SEP_____BN_CONV5_5__SEP__SCALE_MUL___MULI___74_____BN_CONV5_5__SEP__SCALE_MUL_____SCALE___RELU5_5__SEP_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dram_pitch_enable;
    uint32_t conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dram_pitch_value;
#endif
#ifdef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136______MULI___370___1_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dram_pitch_enable;
    uint32_t conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dram_pitch_value;
#endif
#ifdef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136___CROP___320_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dram_pitch_enable;
    uint32_t conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dram_pitch_value;
#endif
#ifdef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136___CROP___321_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dram_pitch_enable;
    uint32_t conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dram_pitch_value;
#endif
#ifdef CONV5_5_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_5_mbox_conf_flat_dram_pitch_enable;
    uint32_t conv5_5_mbox_conf_flat_dram_pitch_value;
#endif
#ifdef CONV5_5_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_5_mbox_loc_flat_dram_pitch_enable;
    uint32_t conv5_5_mbox_loc_flat_dram_pitch_value;
#endif
#ifdef CONV5_6__DW_____BN_CONV5_6__DW__SCALE_MUL___MULI___79_____BN_CONV5_6__DW__SCALE_MUL_____SCALE___MULI___358_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___muli___358_dram_pitch_enable;
    uint32_t conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___muli___358_dram_pitch_value;
#endif
#ifdef CONV5_6__DW_____BN_CONV5_6__DW__SCALE_MUL___MULI___79_____BN_CONV5_6__DW__SCALE_MUL_____SCALE___RELU5_6__DW_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___relu5_6__dw_dram_pitch_enable;
    uint32_t conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___relu5_6__dw_dram_pitch_value;
#endif
#ifdef CONV6__DW_____BN_CONV6__DW__SCALE_MUL___MULI___85_____BN_CONV6__DW__SCALE_MUL_____SCALE___MULI___360_DRAM_PITCH_MODIFIABLE
    uint32_t conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch_enable;
    uint32_t conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch_value;
#endif
#ifdef CONV6__SEP_____BN_CONV6__SEP__SCALE_MUL___MULI___88_____BN_CONV6__SEP__SCALE_MUL_____SCALE___RELU6__SEP_DRAM_PITCH_MODIFIABLE
    uint32_t conv6__sep_____bn_conv6__sep__scale_mul___muli___88_____bn_conv6__sep__scale_mul_____scale___relu6__sep_dram_pitch_enable;
    uint32_t conv6__sep_____bn_conv6__sep__scale_mul___muli___88_____bn_conv6__sep__scale_mul_____scale___relu6__sep_dram_pitch_value;
#endif
#ifdef CONV6_MBOX_CONF___CONV2I___90______7_1_____BN_CONV7_1__SCALE_MUL___MULI___93_____BN_CONV7_1__SCALE_MUL_____SCALE___LOC___CONV2I___138______MULI___367___8___9_DRAM_PITCH_MODIFIABLE
    uint32_t conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9_dram_pitch_enable;
    uint32_t conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9_dram_pitch_value;
#endif
#ifdef CONV6_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
    uint32_t conv6_mbox_loc_flat_dram_pitch_enable;
    uint32_t conv6_mbox_loc_flat_dram_pitch_value;
#endif
#ifdef CONV7_2_____BN_CONV7_2__SCALE_MUL___MULI___96_____BN_CONV7_2__SCALE_MUL_____SCALE___RELU7_2_DRAM_PITCH_MODIFIABLE
    uint32_t conv7_2_____bn_conv7_2__scale_mul___muli___96_____bn_conv7_2__scale_mul_____scale___relu7_2_dram_pitch_enable;
    uint32_t conv7_2_____bn_conv7_2__scale_mul___muli___96_____bn_conv7_2__scale_mul_____scale___relu7_2_dram_pitch_value;
#endif
#ifdef CONV7_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
    uint32_t conv7_2_mbox_conf_flat_dram_pitch_enable;
    uint32_t conv7_2_mbox_conf_flat_dram_pitch_value;
#endif
#ifdef CONV7_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
    uint32_t conv7_2_mbox_loc_flat_dram_pitch_enable;
    uint32_t conv7_2_mbox_loc_flat_dram_pitch_value;
#endif
#ifdef CONV8_2_____BN_CONV8_2__SCALE_MUL___MULI___104_____BN_CONV8_2__SCALE_MUL_____SCALE___RELU8_2_DRAM_PITCH_MODIFIABLE
    uint32_t conv8_2_____bn_conv8_2__scale_mul___muli___104_____bn_conv8_2__scale_mul_____scale___relu8_2_dram_pitch_enable;
    uint32_t conv8_2_____bn_conv8_2__scale_mul___muli___104_____bn_conv8_2__scale_mul_____scale___relu8_2_dram_pitch_value;
#endif
#ifdef CONV8_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
    uint32_t conv8_2_mbox_conf_flat_dram_pitch_enable;
    uint32_t conv8_2_mbox_conf_flat_dram_pitch_value;
#endif
#ifdef CONV8_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
    uint32_t conv8_2_mbox_loc_flat_dram_pitch_enable;
    uint32_t conv8_2_mbox_loc_flat_dram_pitch_value;
#endif
#ifdef CONV9_2_____BN_CONV9_2__SCALE_MUL___MULI___112_____BN_CONV9_2__SCALE_MUL_____SCALE___RELU9_2_DRAM_PITCH_MODIFIABLE
    uint32_t conv9_2_____bn_conv9_2__scale_mul___muli___112_____bn_conv9_2__scale_mul_____scale___relu9_2_dram_pitch_enable;
    uint32_t conv9_2_____bn_conv9_2__scale_mul___muli___112_____bn_conv9_2__scale_mul_____scale___relu9_2_dram_pitch_value;
#endif
#ifdef CONV9_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
    uint32_t conv9_2_mbox_conf_flat_dram_pitch_enable;
    uint32_t conv9_2_mbox_conf_flat_dram_pitch_value;
#endif
#ifdef CONV9_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
    uint32_t conv9_2_mbox_loc_flat_dram_pitch_enable;
    uint32_t conv9_2_mbox_loc_flat_dram_pitch_value;
#endif
#ifdef DATA_DRAM_PITCH_MODIFIABLE
    uint32_t data_dram_pitch_enable;
    uint32_t data_dram_pitch_value;
#endif
#ifdef DATA_UV_DRAM_PITCH_MODIFIABLE
    uint32_t data_uv_dram_pitch_enable;
    uint32_t data_uv_dram_pitch_value;
#endif
#ifdef MBOX_CONF_FLATTEN__DRAM_PITCH_MODIFIABLE
    uint32_t mbox_conf_flatten__dram_pitch_enable;
    uint32_t mbox_conf_flatten__dram_pitch_value;
#endif
#ifdef MBOX_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
    uint32_t mbox_conf_flatten_dram_pitch_enable;
    uint32_t mbox_conf_flatten_dram_pitch_value;
#endif
#ifdef MBOX_LOC_DRAM_PITCH_MODIFIABLE
    uint32_t mbox_loc_dram_pitch_enable;
    uint32_t mbox_loc_dram_pitch_value;
#endif
#ifdef RELU7_1_DRAM_PITCH_MODIFIABLE
    uint32_t relu7_1_dram_pitch_enable;
    uint32_t relu7_1_dram_pitch_value;
#endif
} mnet_ssd_adas_flex_pic_ag_optional_fields_t;

/* Core functions */
// Reads CVTable
errcode_enum_t mnet_ssd_adas_flex_pic_ag_init (
    mnet_ssd_adas_flex_pic_ag_ips_t             *ctxt
);
// run() is NOT re-entrant with respect to run() calls for other DAGs
errcode_enum_t mnet_ssd_adas_flex_pic_ag_run (
    mnet_ssd_adas_flex_pic_ag_ips_t             *ctxt,
    mnet_ssd_adas_flex_pic_ag_required_fields_t *r_args,
    mnet_ssd_adas_flex_pic_ag_optional_fields_t *o_args
);
// Reset all fields to their respective default values
errcode_enum_t mnet_ssd_adas_flex_pic_ag_reset (
    mnet_ssd_adas_flex_pic_ag_optional_fields_t *o_args
);

/* Concretize struct definitions */
#include "mnet_ssd_adas_flex_pic_ag_private.h"
#endif

