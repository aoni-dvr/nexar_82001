/*
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
*/

#include "AmbaTypes.h"
#include "AmbaDSP_Img3astatistic.h"
#include "AmbaDSP_ImgPrint3aStatistic.h"
#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageFilterInternal.h"
#include "AmbaDSP_ImageUtility.h"

#if (defined(__unix__) && (defined(__QNX__) || defined(CONFIG_LINUX)))
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define amba_ik_system_memcpy memcpy   //TBD
//static void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
//{
//
//}
//static void AmbaPrint_PrintStr5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
//{
//
//}


#else
#include "AmbaPrint.h"
//#define amba_ik_system_memcpy AmbaWrap_memcpy   //TBD
#endif

#define DC_U    99999UL /*PRINT DONT CARE FOR UINT*/
#define DC_S    NULL    /*PRINT DONT CARE FOR STRING*/

static UINT32 AAALogInterval = 120UL;

static UINT32 img_3a_process_cfa_statistic_header(const UINT8 *p_src_stitch_num_x,
        const UINT8 *p_src_stitch_num_y,
        const ULONG *src_img_cfa_aaa_stat_addr,
        ik_cfa_3a_data_t  *p_dst_img_cfa_aaa_stat)
{
    UINT32  rval = IK_OK;
    UINT32 i, j;
    UINT32 src_idx;
    const ik_cfa_3a_data_t *p_src_img_cfa_aaa_stat;
    const ik_3a_header_t *p_src_cfa_header;
    ik_3a_header_t *p_dst_cfa_header;

    // misraC
    p_dst_cfa_header = &p_dst_img_cfa_aaa_stat->header;

    for(i=0; i< *p_src_stitch_num_y; i++) {
        for(j=0; j< *p_src_stitch_num_x; j++) { //Need know total slice

            src_idx = j + (*p_src_stitch_num_x *i);
            (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[src_idx],sizeof(ik_cfa_3a_data_t  *));
            //AmbaPrint_PrintUInt5("############## SOURCE CFA 3a p_src_stitch_num_x:%d, p_src_stitch_num_y:%d addr:%p, size:%d", j, i, src_img_cfa_aaa_stat_addr[src_idx], sizeof(ik_cfa_3a_data_t), 0);
            //===== pointer init =====//
            p_src_cfa_header = &p_src_img_cfa_aaa_stat->header;
            p_dst_cfa_header = &p_dst_img_cfa_aaa_stat->header;

            //===== Do check =====//
            if ((*p_src_stitch_num_x!=p_src_cfa_header->total_slices_x) || (*p_src_stitch_num_y!=p_src_cfa_header->total_slices_y)) {
                rval = IK_ERR_0103;
            }
            if ((j!=p_src_cfa_header->slice_index_x) || (i!=p_src_cfa_header->slice_index_y)) {
                rval = IK_ERR_0103;
            }

            //===== Cfa Header =====//
            if((i==0U)&&(j==0U)) {
                (void)amba_ik_system_memcpy(p_dst_cfa_header, p_src_cfa_header, sizeof(ik_3a_header_t));
            } else {
                if(i == 0U) {
                    p_dst_cfa_header->slice_width      += p_src_cfa_header->slice_width;
                    p_dst_cfa_header->awb_tile_num_col += p_src_cfa_header->awb_tile_num_col;
                    p_dst_cfa_header->ae_tile_num_col  += p_src_cfa_header->ae_tile_num_col;
                    p_dst_cfa_header->af_tile_num_col  += p_src_cfa_header->af_tile_num_col;
                }
                if(j == 0U) {
                    p_dst_cfa_header->slice_height     += p_src_cfa_header->slice_height;
                    p_dst_cfa_header->awb_tile_num_row += p_src_cfa_header->awb_tile_num_row;
                    p_dst_cfa_header->ae_tile_num_row  += p_src_cfa_header->ae_tile_num_row;
                    p_dst_cfa_header->af_tile_num_row  += p_src_cfa_header->af_tile_num_row;
                }
            }

            //===== Cfa frame_id =====//
            if((i==0U)&&(j==0U)) {
                p_dst_img_cfa_aaa_stat ->frame_id = p_src_img_cfa_aaa_stat->frame_id;
            } else {
                ////ToDo
            }
        }
    }

    {
        // error checking
        if((p_dst_cfa_header->awb_tile_num_col > AMBA_IK_3A_AWB_TILE_COL_COUNT) ||
           (p_dst_cfa_header->awb_tile_num_row > AMBA_IK_3A_AWB_TILE_ROW_COUNT)) {
            rval = IK_ERR_0103;
        }
        if((p_dst_cfa_header->ae_tile_num_col > AMBA_IK_3A_AE_TILE_COL_COUNT) ||
           (p_dst_cfa_header->ae_tile_num_row > AMBA_IK_3A_AE_TILE_ROW_COUNT)) {
            rval = IK_ERR_0103;
        }
        if((p_dst_cfa_header->af_tile_num_col > AMBA_IK_3A_AF_TILE_COL_COUNT) ||
           (p_dst_cfa_header->af_tile_num_row > AMBA_IK_3A_AF_TILE_ROW_COUNT)) {
            rval = IK_ERR_0103;
        }
    }

    return rval;
}

static void img_3a_process_cfa_awb_statistic(UINT32 current_idx_x,
        UINT32 current_idx_y,
        const UINT8 *p_src_stitch_num_x,
        const ULONG *src_img_cfa_aaa_stat_addr,
        ik_cfa_3a_data_t  *p_dst_img_cfa_aaa_stat)
{
    const ik_cfa_3a_data_t *p_src_img_cfa_aaa_stat;
    UINT32 src_idx;
    const void *p_src_cfa;
    const ik_3a_header_t *p_src_cfa_header;
    const ik_cfa_awb_t *p_src_awb;
    ik_cfa_awb_t *p_dst_awb;
    UINT32 offset = 0;
    UINT32 m, n, k;
    ULONG misra_ul_addr;

    src_idx = current_idx_x + (*p_src_stitch_num_x*current_idx_y);
    (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[src_idx],sizeof(ik_cfa_3a_data_t  *));

    //===== pointer init =====//
    (void)amba_ik_system_memcpy(&p_src_cfa, &p_src_img_cfa_aaa_stat, sizeof(void *));
    (void)amba_ik_system_memcpy(&p_src_cfa_header, &p_src_cfa, sizeof(ik_3a_header_t *));

    for(m = 0; m < current_idx_x; m++) {
        (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[m],sizeof(ik_cfa_3a_data_t  *));
        offset += p_src_img_cfa_aaa_stat->header.awb_tile_num_col;
    }

    for(n = 0; n < current_idx_y; n++) {
        (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[*p_src_stitch_num_x*n],sizeof(ik_cfa_3a_data_t  *));
        offset += ((UINT32)p_dst_img_cfa_aaa_stat->header.awb_tile_num_col*(UINT32)p_src_img_cfa_aaa_stat->header.awb_tile_num_row);
    }

    p_dst_awb = &p_dst_img_cfa_aaa_stat->awb[0];
    (void)amba_ik_system_memcpy(&misra_ul_addr, &p_dst_awb, sizeof(ULONG));
    misra_ul_addr = misra_ul_addr + (offset * sizeof(ik_cfa_awb_t));
    (void)amba_ik_system_memcpy(&p_dst_awb, &misra_ul_addr, sizeof(ULONG));

    (void)amba_ik_system_memcpy(&misra_ul_addr, &p_src_cfa, sizeof(ULONG));
    misra_ul_addr = misra_ul_addr + sizeof(ik_3a_header_t) + sizeof(UINT16);
    (void)amba_ik_system_memcpy(&p_src_cfa, &misra_ul_addr, sizeof(ULONG));
    (void)amba_ik_system_memcpy(&p_src_awb, &p_src_cfa, sizeof(ik_cfa_awb_t *));

    for(k=0; k < p_src_cfa_header->awb_tile_num_row; k++) {
        (void)amba_ik_system_memcpy(p_dst_awb, p_src_awb, sizeof(ik_cfa_awb_t)*p_src_cfa_header->awb_tile_num_col);
        //p_src_awb = p_src_awb + p_src_cfa_header->awb_tile_num_col;
        //p_dst_awb = p_dst_awb + p_dst_cfa_header->awb_tile_num_col;

        (void)amba_ik_system_memcpy(&misra_ul_addr, &p_src_awb, sizeof(ULONG));
        misra_ul_addr = misra_ul_addr + (p_src_cfa_header->awb_tile_num_col * sizeof(ik_cfa_awb_t));
        (void)amba_ik_system_memcpy(&p_src_awb, &misra_ul_addr, sizeof(ULONG));

        (void)amba_ik_system_memcpy(&misra_ul_addr, &p_dst_awb, sizeof(ULONG));
        misra_ul_addr = misra_ul_addr + (p_dst_img_cfa_aaa_stat->header.awb_tile_num_col * sizeof(ik_cfa_awb_t));
        (void)amba_ik_system_memcpy(&p_dst_awb, &misra_ul_addr, sizeof(ULONG));
    }
}

static void img_3a_process_cfa_ae_statistic(UINT32 current_idx_x,
        UINT32 current_idx_y,
        const UINT8 *p_src_stitch_num_x,
        const ULONG *src_img_cfa_aaa_stat_addr,
        ik_cfa_3a_data_t  *p_dst_img_cfa_aaa_stat)
{
    const ik_cfa_3a_data_t *p_src_img_cfa_aaa_stat;
    UINT32 src_idx;
    const void *p_src_cfa;
    const ik_3a_header_t *p_src_cfa_header;
    const ik_cfa_ae_t *p_src_ae;
    ik_cfa_ae_t *p_dst_ae;
    UINT32 offset = 0;
    UINT32 m, n, k;
    ULONG misra_ul_addr;

    src_idx = current_idx_x + (*p_src_stitch_num_x*current_idx_y);
    (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[src_idx],sizeof(ik_cfa_3a_data_t  *));

    //===== pointer init =====//
    (void)amba_ik_system_memcpy(&p_src_cfa, &p_src_img_cfa_aaa_stat, sizeof(void *));
    (void)amba_ik_system_memcpy(&p_src_cfa_header, &p_src_cfa, sizeof(ik_3a_header_t *));

    for(m = 0; m < current_idx_x; m++) {
        (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[m],sizeof(ik_cfa_3a_data_t  *));
        offset += p_src_img_cfa_aaa_stat->header.ae_tile_num_col;
    }

    for(n = 0; n < current_idx_y; n++) {
        (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[*p_src_stitch_num_x*n],sizeof(ik_cfa_3a_data_t  *));
        offset += ((UINT32)p_dst_img_cfa_aaa_stat->header.ae_tile_num_col*(UINT32)p_src_img_cfa_aaa_stat->header.ae_tile_num_row);
    }

    p_dst_ae = &p_dst_img_cfa_aaa_stat->ae[0];
    (void)amba_ik_system_memcpy(&misra_ul_addr, &p_dst_ae, sizeof(ULONG));
    misra_ul_addr = misra_ul_addr + (offset * sizeof(ik_cfa_ae_t));
    (void)amba_ik_system_memcpy(&p_dst_ae, &misra_ul_addr, sizeof(ULONG));

    (void)amba_ik_system_memcpy(&misra_ul_addr, &p_src_cfa, sizeof(ULONG));
    misra_ul_addr = misra_ul_addr + sizeof(ik_3a_header_t) + sizeof(UINT16) + (sizeof(ik_cfa_awb_t) * p_src_cfa_header->awb_tile_num_row * p_src_cfa_header->awb_tile_num_col);
    (void)amba_ik_system_memcpy(&p_src_cfa, &misra_ul_addr, sizeof(ULONG));
    (void)amba_ik_system_memcpy(&p_src_ae, &p_src_cfa, sizeof(ik_cfa_ae_t *));

    for(k=0; k<p_src_cfa_header->ae_tile_num_row; k++) {
        (void)amba_ik_system_memcpy(p_dst_ae, p_src_ae, sizeof(ik_cfa_ae_t)*p_src_cfa_header->ae_tile_num_col);
        (void)amba_ik_system_memcpy(&misra_ul_addr, &p_src_ae, sizeof(ULONG));
        misra_ul_addr = misra_ul_addr + (p_src_cfa_header->ae_tile_num_col * sizeof(ik_cfa_ae_t));
        (void)amba_ik_system_memcpy(&p_src_ae, &misra_ul_addr, sizeof(ULONG));

        (void)amba_ik_system_memcpy(&misra_ul_addr, &p_dst_ae, sizeof(ULONG));
        misra_ul_addr = misra_ul_addr + (p_dst_img_cfa_aaa_stat->header.ae_tile_num_col * sizeof(ik_cfa_ae_t));
        (void)amba_ik_system_memcpy(&p_dst_ae, &misra_ul_addr, sizeof(ULONG));
    }
}

static void img_3a_process_cfa_af_statistic(UINT32 current_idx_x,
        UINT32 current_idx_y,
        const UINT8 *p_src_stitch_num_x,
        const ULONG *src_img_cfa_aaa_stat_addr,
        ik_cfa_3a_data_t  *p_dst_img_cfa_aaa_stat)
{
    const ik_cfa_3a_data_t *p_src_img_cfa_aaa_stat;
    UINT32 src_idx;
    const void *p_src_cfa;
    const ik_3a_header_t *p_src_cfa_header;
    const ik_cfa_af_t *p_src_af;
    ik_cfa_af_t *p_dst_af;
    UINT32 offset = 0;
    UINT32 m, n, k;
    ULONG misra_ul_addr;

    src_idx = current_idx_x + (*p_src_stitch_num_x*current_idx_y);
    (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[src_idx],sizeof(ik_cfa_3a_data_t  *));

    //===== pointer init =====//
    (void)amba_ik_system_memcpy(&p_src_cfa, &p_src_img_cfa_aaa_stat, sizeof(void *));
    (void)amba_ik_system_memcpy(&p_src_cfa_header, &p_src_cfa, sizeof(ik_3a_header_t *));

    for(m = 0; m < current_idx_x; m++) {
        (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[m],sizeof(ik_cfa_3a_data_t  *));
        offset += p_src_img_cfa_aaa_stat->header.af_tile_num_col;
    }

    for(n = 0; n < current_idx_y; n++) {
        (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[*p_src_stitch_num_x*n],sizeof(ik_cfa_3a_data_t  *));
        offset += ((UINT32)p_dst_img_cfa_aaa_stat->header.af_tile_num_col*(UINT32)p_src_img_cfa_aaa_stat->header.af_tile_num_row);
    }

    p_dst_af = &p_dst_img_cfa_aaa_stat->af[0];
    (void)amba_ik_system_memcpy(&misra_ul_addr, &p_dst_af, sizeof(ULONG));
    misra_ul_addr = misra_ul_addr + (offset * sizeof(ik_cfa_af_t));
    (void)amba_ik_system_memcpy(&p_dst_af, &misra_ul_addr, sizeof(ULONG));

    (void)amba_ik_system_memcpy(&misra_ul_addr, &p_src_cfa, sizeof(ULONG));
    misra_ul_addr = misra_ul_addr + sizeof(ik_3a_header_t) + sizeof(UINT16)\
                    + (sizeof(ik_cfa_awb_t) * p_src_cfa_header->awb_tile_num_row * p_src_cfa_header->awb_tile_num_col)\
                    + (sizeof(ik_cfa_ae_t) * p_src_cfa_header->ae_tile_num_row * p_src_cfa_header->ae_tile_num_col);
    (void)amba_ik_system_memcpy(&p_src_cfa, &misra_ul_addr, sizeof(ULONG));
    (void)amba_ik_system_memcpy(&p_src_af, &p_src_cfa, sizeof(ik_cfa_af_t *));

    for(k=0; k<p_src_cfa_header->af_tile_num_row; k++) {

        (void)amba_ik_system_memcpy(p_dst_af, p_src_af, sizeof( ik_cfa_af_t)*p_src_cfa_header->af_tile_num_col);
        (void)amba_ik_system_memcpy(&misra_ul_addr, &p_src_af, sizeof(ULONG));
        misra_ul_addr = misra_ul_addr + (p_src_cfa_header->af_tile_num_col * sizeof(ik_cfa_af_t));
        (void)amba_ik_system_memcpy(&p_src_af, &misra_ul_addr, sizeof(ULONG));

        (void)amba_ik_system_memcpy(&misra_ul_addr, &p_dst_af, sizeof(ULONG));
        misra_ul_addr = misra_ul_addr + (p_dst_img_cfa_aaa_stat->header.af_tile_num_col * sizeof(ik_cfa_af_t));
        (void)amba_ik_system_memcpy(&p_dst_af, &misra_ul_addr, sizeof(ULONG));
    }
}

static void img_3a_process_cfa_histo_statistic(UINT32 current_idx_x,
        UINT32 current_idx_y,
        const UINT8 *p_src_stitch_num_x,
        const ULONG *src_img_cfa_aaa_stat_addr,
        ik_cfa_3a_data_t  *p_dst_img_cfa_aaa_stat)
{
    const ik_cfa_3a_data_t *p_src_img_cfa_aaa_stat;
    UINT32 src_idx;
    const void *p_src_cfa;
    const ik_3a_header_t *p_src_cfa_header;
    const ik_cfa_histo_t *p_src_histo;
    ik_cfa_histo_t *p_dst_histo;
    ULONG misra_ul_addr;
    UINT32 k;

    src_idx = current_idx_x + (*p_src_stitch_num_x*current_idx_y);
    (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[src_idx],sizeof(ik_cfa_3a_data_t  *));

    //===== pointer init =====//
    (void)amba_ik_system_memcpy(&p_src_cfa, &p_src_img_cfa_aaa_stat, sizeof(void *));
    (void)amba_ik_system_memcpy(&p_src_cfa_header, &p_src_cfa, sizeof(ik_3a_header_t *));

    p_dst_histo = &p_dst_img_cfa_aaa_stat->histogram;

    (void)amba_ik_system_memcpy(&misra_ul_addr, &p_src_cfa, sizeof(ULONG));
    misra_ul_addr = misra_ul_addr + sizeof(ik_3a_header_t) + sizeof(UINT16)\
                    + (sizeof(ik_cfa_awb_t) * p_src_cfa_header->awb_tile_num_row * p_src_cfa_header->awb_tile_num_col)\
                    + (sizeof(ik_cfa_ae_t) * p_src_cfa_header->ae_tile_num_row * p_src_cfa_header->ae_tile_num_col)\
                    + (sizeof( ik_cfa_af_t) * p_src_cfa_header->af_tile_num_row * p_src_cfa_header->af_tile_num_col);
    (void)amba_ik_system_memcpy(&p_src_cfa, &misra_ul_addr, sizeof(ULONG));
    (void)amba_ik_system_memcpy(&p_src_histo, &p_src_cfa, sizeof(ik_cfa_histo_t *));

    if((current_idx_y == 0U)&&(current_idx_x == 0U)) {
        (void)amba_ik_system_memcpy(p_dst_histo, p_src_histo, sizeof(ik_cfa_histo_t));
    } else {
        for(k=0; k< 64U; k++) {
            p_dst_histo->his_bin_r[k] += p_src_histo->his_bin_r[k];
            p_dst_histo->his_bin_g[k] += p_src_histo->his_bin_g[k];
            p_dst_histo->his_bin_b[k] += p_src_histo->his_bin_b[k];
            p_dst_histo->his_bin_y[k] += p_src_histo->his_bin_y[k];
        }
    }
}


static UINT32  img_3a_process_cfa_statistic(const UINT8 *p_src_stitch_num_x,
        const UINT8 *p_src_stitch_num_y,
        const ULONG *src_img_cfa_aaa_stat_addr,
        ik_cfa_3a_data_t  *p_dst_img_cfa_aaa_stat)
{
    UINT32  rval = IK_OK;
    UINT32  i, j;

    rval = img_3a_process_cfa_statistic_header(p_src_stitch_num_x, p_src_stitch_num_y, src_img_cfa_aaa_stat_addr, p_dst_img_cfa_aaa_stat);

    for(i=0; i< *p_src_stitch_num_y; i++) {
        for(j=0; j< *p_src_stitch_num_x; j++) { //Need know total slice
            //===== Cfa Awb =====//
            img_3a_process_cfa_awb_statistic(j, i, p_src_stitch_num_x, src_img_cfa_aaa_stat_addr, p_dst_img_cfa_aaa_stat);
            //===== Cfa Ae =====//
            img_3a_process_cfa_ae_statistic(j, i, p_src_stitch_num_x, src_img_cfa_aaa_stat_addr, p_dst_img_cfa_aaa_stat);
            //===== Cfa Af =====//
            img_3a_process_cfa_af_statistic(j, i, p_src_stitch_num_x, src_img_cfa_aaa_stat_addr, p_dst_img_cfa_aaa_stat);
            //===== Cfa histogram =====//
            img_3a_process_cfa_histo_statistic(j, i, p_src_stitch_num_x, src_img_cfa_aaa_stat_addr, p_dst_img_cfa_aaa_stat);
        }
    }
    return rval;
}

static UINT32 img_3a_process_pg_statistic_header(const UINT8 *p_src_stitch_num_x,
        const UINT8 *p_src_stitch_num_y,
        const ULONG *src_img_pg_aaa_stat_addr,
        ik_pg_3a_data_t  *p_dst_img_pg_aaa_stat)
{
    UINT32  rval = IK_OK;
    UINT32 i, j;
    UINT32 src_idx;
    const ik_pg_3a_data_t *p_src_img_pg_aaa_stat;
    const ik_3a_header_t *p_src_pg_header;
    ik_3a_header_t *p_dst_pg_header;

    // misraC
    p_dst_pg_header = &p_dst_img_pg_aaa_stat->header;

    for(i=0; i< *p_src_stitch_num_y; i++) {
        for(j=0; j< *p_src_stitch_num_x; j++) { //Need know total slice

            src_idx = j + (*p_src_stitch_num_x *i);
            (void)amba_ik_system_memcpy(&p_src_img_pg_aaa_stat, &src_img_pg_aaa_stat_addr[src_idx],sizeof(ik_pg_3a_data_t  *));
            //AmbaPrint_PrintUInt5("############## SOURCE CFA 3a p_src_stitch_num_x:%d, p_src_stitch_num_y:%d addr:%p, size:%d", j, i, src_img_cfa_aaa_stat_addr[src_idx], sizeof(ik_cfa_3a_data_t), 0);
            //===== pointer init =====//
            p_src_pg_header = &p_src_img_pg_aaa_stat->header;
            p_dst_pg_header = &p_dst_img_pg_aaa_stat->header;
            //===== Do check =====//
            if ((*p_src_stitch_num_x!=p_src_pg_header->total_slices_x) || (*p_src_stitch_num_y!=p_src_pg_header->total_slices_y)) {
                rval = IK_ERR_0103;
            }
            if ((j!=p_src_pg_header->slice_index_x) || (i!=p_src_pg_header->slice_index_y)) {
                rval = IK_ERR_0103;
            }

            //===== Cfa Header =====//

            if((i==0U)&&(j==0U)) {
                (void)amba_ik_system_memcpy(p_dst_pg_header, p_src_pg_header, sizeof(ik_3a_header_t));
            } else {
                if(i == 0U) {
                    p_dst_pg_header->slice_width      += p_src_pg_header->slice_width;
                    p_dst_pg_header->awb_tile_num_col += p_src_pg_header->awb_tile_num_col;
                    p_dst_pg_header->ae_tile_num_col  += p_src_pg_header->ae_tile_num_col;
                    p_dst_pg_header->af_tile_num_col  += p_src_pg_header->af_tile_num_col;
                }
                if(j == 0U) {
                    p_dst_pg_header->slice_height     += p_src_pg_header->slice_height;
                    p_dst_pg_header->awb_tile_num_row += p_src_pg_header->awb_tile_num_row;
                    p_dst_pg_header->ae_tile_num_row  += p_src_pg_header->ae_tile_num_row;
                    p_dst_pg_header->af_tile_num_row  += p_src_pg_header->af_tile_num_row;
                }
            }
            //===== Cfa frame_id =====//
            if((i==0U)&&(j==0U)) {
                p_dst_img_pg_aaa_stat ->frame_id = p_src_img_pg_aaa_stat->frame_id;
            } else {
                ////ToDo
            }
        }
    }

    {
        // error checking
        if((p_dst_pg_header->ae_tile_num_col > AMBA_IK_3A_AE_TILE_COL_COUNT) ||
           (p_dst_pg_header->af_tile_num_col > AMBA_IK_3A_AF_TILE_COL_COUNT)) {
            rval = IK_ERR_0103;
        }
        if((p_dst_pg_header->ae_tile_num_row > AMBA_IK_3A_AE_TILE_ROW_COUNT) ||
           (p_dst_pg_header->af_tile_num_row > AMBA_IK_3A_AF_TILE_ROW_COUNT)) {
            rval = IK_ERR_0103;
        }
    }

    return rval;
}

static void img_3a_process_pg_ae_statistic(UINT32 current_idx_x,
        UINT32 current_idx_y,
        const UINT8 *p_src_stitch_num_x,
        const ULONG *src_img_pg_aaa_stat_addr,
        ik_pg_3a_data_t  *p_dst_img_pg_aaa_stat)
{
    const ik_pg_3a_data_t *p_src_img_pg_aaa_stat;
    UINT32 src_idx;
    const void *p_src_pg;
    const ik_3a_header_t *p_src_pg_header;
    const ik_pg_ae_t *p_src_ae;
    ik_pg_ae_t *p_dst_ae;
    UINT32 offset = 0;
    UINT32 m, n, k;
    ULONG misra_ul_addr;

    src_idx = current_idx_x + (*p_src_stitch_num_x*current_idx_y);
    (void)amba_ik_system_memcpy(&p_src_img_pg_aaa_stat, &src_img_pg_aaa_stat_addr[src_idx],sizeof(ik_pg_3a_data_t  *));

    //===== pointer init =====//
    (void)amba_ik_system_memcpy(&p_src_pg, &p_src_img_pg_aaa_stat, sizeof(void *));
    (void)amba_ik_system_memcpy(&p_src_pg_header, &p_src_pg, sizeof(ik_3a_header_t *));

    for(m = 0; m < current_idx_x; m++) {
        (void)amba_ik_system_memcpy(&p_src_img_pg_aaa_stat, &src_img_pg_aaa_stat_addr[m], sizeof(ik_pg_3a_data_t  *));
        offset += p_src_img_pg_aaa_stat->header.ae_tile_num_col;
    }

    for(n = 0; n < current_idx_y; n++) {
        (void)amba_ik_system_memcpy(&p_src_img_pg_aaa_stat, &src_img_pg_aaa_stat_addr[*p_src_stitch_num_x*n],sizeof(ik_pg_3a_data_t  *));
        offset += ((UINT32)p_dst_img_pg_aaa_stat->header.ae_tile_num_col*(UINT32)p_src_img_pg_aaa_stat->header.ae_tile_num_row);
    }

    p_dst_ae = &p_dst_img_pg_aaa_stat->ae[0];
    (void)amba_ik_system_memcpy(&misra_ul_addr, &p_dst_ae, sizeof(ULONG));
    misra_ul_addr = misra_ul_addr + (offset * sizeof(ik_pg_ae_t));
    (void)amba_ik_system_memcpy(&p_dst_ae, &misra_ul_addr, sizeof(ULONG));

    (void)amba_ik_system_memcpy(&misra_ul_addr, &p_src_pg, sizeof(ULONG));
    misra_ul_addr = misra_ul_addr + sizeof(ik_3a_header_t) + sizeof(UINT16) + (sizeof(ik_pg_af_t) * p_src_pg_header->af_tile_num_row * p_src_pg_header->af_tile_num_col);
    (void)amba_ik_system_memcpy(&p_src_pg, &misra_ul_addr, sizeof(ULONG));
    (void)amba_ik_system_memcpy(&p_src_ae, &p_src_pg, sizeof(ik_pg_ae_t *));

    for(k=0; k<p_src_pg_header->ae_tile_num_row; k++) {
        (void)amba_ik_system_memcpy(p_dst_ae, p_src_ae, sizeof(ik_pg_ae_t)*p_src_pg_header->ae_tile_num_col);
        (void)amba_ik_system_memcpy(&misra_ul_addr, &p_src_ae, sizeof(ULONG));
        misra_ul_addr = misra_ul_addr + (p_src_pg_header->ae_tile_num_col * sizeof(ik_pg_ae_t));
        (void)amba_ik_system_memcpy(&p_src_ae, &misra_ul_addr, sizeof(ULONG));

        (void)amba_ik_system_memcpy(&misra_ul_addr, &p_dst_ae, sizeof(ULONG));
        misra_ul_addr = misra_ul_addr + (p_dst_img_pg_aaa_stat->header.ae_tile_num_col * sizeof(ik_pg_ae_t));
        (void)amba_ik_system_memcpy(&p_dst_ae, &misra_ul_addr, sizeof(ULONG));
    }
}


static void img_3a_process_pg_af_statistic(UINT32 current_idx_x,
        UINT32 current_idx_y,
        const UINT8 *p_src_stitch_num_x,
        const ULONG *src_img_pg_aaa_stat_addr,
        ik_pg_3a_data_t  *p_dst_img_pg_aaa_stat)
{
    const ik_pg_3a_data_t *p_src_img_pg_aaa_stat;
    UINT32 src_idx;
    const void *p_src_pg;
    const ik_3a_header_t *p_src_pg_header;
    const ik_pg_af_t *p_src_af;
    ik_pg_af_t *p_dst_af;
    UINT32 offset = 0;
    UINT32 m, n, k;
    ULONG misra_ul_addr;

    src_idx = current_idx_x + (*p_src_stitch_num_x*current_idx_y);
    (void)amba_ik_system_memcpy(&p_src_img_pg_aaa_stat, &src_img_pg_aaa_stat_addr[src_idx],sizeof(ik_pg_3a_data_t  *));

    //===== pointer init =====//
    (void)amba_ik_system_memcpy(&p_src_pg, &p_src_img_pg_aaa_stat, sizeof(void *));
    (void)amba_ik_system_memcpy(&p_src_pg_header, &p_src_pg, sizeof(ik_3a_header_t *));

    for(m = 0; m < current_idx_x; m++) {
        (void)amba_ik_system_memcpy(&p_src_img_pg_aaa_stat, &src_img_pg_aaa_stat_addr[m], sizeof(ik_pg_3a_data_t  *));
        offset += p_src_img_pg_aaa_stat->header.af_tile_num_col;
    }

    for(n = 0; n < current_idx_y; n++) {
        (void)amba_ik_system_memcpy(&p_src_img_pg_aaa_stat, &src_img_pg_aaa_stat_addr[*p_src_stitch_num_x*n],sizeof(ik_pg_3a_data_t  *));
        offset += ((UINT32)p_dst_img_pg_aaa_stat->header.af_tile_num_col*(UINT32)p_src_img_pg_aaa_stat->header.af_tile_num_row);
    }

    p_dst_af = &p_dst_img_pg_aaa_stat->af[0];
    (void)amba_ik_system_memcpy(&misra_ul_addr, &p_dst_af, sizeof(ULONG));
    misra_ul_addr = misra_ul_addr + (offset * sizeof(ik_pg_af_t));
    (void)amba_ik_system_memcpy(&p_dst_af, &misra_ul_addr, sizeof(ULONG));

    (void)amba_ik_system_memcpy(&misra_ul_addr, &p_src_pg, sizeof(ULONG));
    misra_ul_addr = misra_ul_addr + sizeof(ik_3a_header_t) + sizeof(UINT16);
    (void)amba_ik_system_memcpy(&p_src_pg, &misra_ul_addr, sizeof(ULONG));
    (void)amba_ik_system_memcpy(&p_src_af, &p_src_pg, sizeof(ik_pg_af_t *));


    for(k=0; k<p_src_pg_header->af_tile_num_row; k++) {
        (void)amba_ik_system_memcpy( p_dst_af, p_src_af, sizeof(ik_pg_af_t)*p_src_pg_header->af_tile_num_col);
        (void)amba_ik_system_memcpy(&misra_ul_addr, &p_src_af, sizeof(ULONG));
        misra_ul_addr = misra_ul_addr + (p_src_pg_header->af_tile_num_col * sizeof(ik_pg_af_t));
        (void)amba_ik_system_memcpy(&p_src_af, &misra_ul_addr, sizeof(ULONG));

        (void)amba_ik_system_memcpy(&misra_ul_addr, &p_dst_af, sizeof(ULONG));
        misra_ul_addr = misra_ul_addr + (p_dst_img_pg_aaa_stat->header.af_tile_num_col * sizeof(ik_pg_af_t));
        (void)amba_ik_system_memcpy(&p_dst_af, &misra_ul_addr, sizeof(ULONG));

    }
}

static void img_3a_process_pg_histo_statistic(UINT32 current_idx_x,
        UINT32 current_idx_y,
        const UINT8 *p_src_stitch_num_x,
        const ULONG *src_img_pg_aaa_stat_addr,
        ik_pg_3a_data_t  *p_dst_img_pg_aaa_stat)
{
    const ik_pg_3a_data_t *p_src_img_pg_aaa_stat;
    UINT32 src_idx;
    const void *p_src_pg;
    const ik_3a_header_t *p_src_pg_header;
    const ik_pg_histo_t *p_src_histo;
    ik_pg_histo_t *p_dst_histo;
    ULONG misra_ul_addr;
    UINT32 k;

    src_idx = current_idx_x + (*p_src_stitch_num_x*current_idx_y);
    (void)amba_ik_system_memcpy(&p_src_img_pg_aaa_stat, &src_img_pg_aaa_stat_addr[src_idx],sizeof(ik_pg_3a_data_t  *));

    //===== pointer init =====//
    (void)amba_ik_system_memcpy(&p_src_pg, &p_src_img_pg_aaa_stat, sizeof(void *));
    (void)amba_ik_system_memcpy(&p_src_pg_header, &p_src_pg, sizeof(ik_3a_header_t *));

    p_dst_histo = &p_dst_img_pg_aaa_stat->histogram;

    (void)amba_ik_system_memcpy(&misra_ul_addr, &p_src_pg, sizeof(ULONG));
    misra_ul_addr = misra_ul_addr + sizeof(ik_3a_header_t) + sizeof(UINT16)\
                    + (sizeof(ik_pg_af_t) * p_src_pg_header->af_tile_num_row * p_src_pg_header->af_tile_num_col)\
                    + (sizeof(ik_pg_ae_t) * p_src_pg_header->ae_tile_num_row * p_src_pg_header->ae_tile_num_col);
    (void)amba_ik_system_memcpy(&p_src_pg, &misra_ul_addr, sizeof(ULONG));
    (void)amba_ik_system_memcpy(&p_src_histo, &p_src_pg, sizeof(ik_pg_histo_t *));

    if((current_idx_y == 0U)&&(current_idx_x == 0U)) {
        (void)amba_ik_system_memcpy(p_dst_histo,p_src_histo,sizeof(ik_pg_histo_t));
    } else {
        for(k=0; k< 64U; k++) {
            p_dst_histo->his_bin_r[k] += p_src_histo->his_bin_r[k];
            p_dst_histo->his_bin_g[k] += p_src_histo->his_bin_g[k];
            p_dst_histo->his_bin_b[k] += p_src_histo->his_bin_b[k];
            p_dst_histo->his_bin_y[k] += p_src_histo->his_bin_y[k];
        }
    }
}

static UINT32  img_3a_process_pg_statistic(const UINT8 *p_src_stitch_num_x,
        const UINT8 *p_src_stitch_num_y,
        const ULONG *src_img_pg_aaa_stat_addr,
        ik_pg_3a_data_t  *p_dst_img_pg_aaa_stat)
{
    UINT32   rval = IK_OK;
    UINT32  i, j;

    rval = img_3a_process_pg_statistic_header(p_src_stitch_num_x, p_src_stitch_num_y, src_img_pg_aaa_stat_addr, p_dst_img_pg_aaa_stat);

    for(i=0; i< *p_src_stitch_num_y; i++) {
        for(j=0; j< *p_src_stitch_num_x; j++) {
            //===== Pg Ae =====//
            img_3a_process_pg_ae_statistic(j, i, p_src_stitch_num_x, src_img_pg_aaa_stat_addr, p_dst_img_pg_aaa_stat);
            //===== Pg Af =====//
            img_3a_process_pg_af_statistic(j, i, p_src_stitch_num_x, src_img_pg_aaa_stat_addr, p_dst_img_pg_aaa_stat);
            //===== histogram =====//
            img_3a_process_pg_histo_statistic(j, i, p_src_stitch_num_x, src_img_pg_aaa_stat_addr, p_dst_img_pg_aaa_stat);
        }
    }
    return rval;
}


static INT32  img_3a_process_hist_statistic(const UINT8 *p_type,
        const UINT8 *p_src_stitch_num_x,
        const UINT8 *p_src_stitch_num_y,
        const ULONG *src_img_hist_aaa_stat_addr,
        ik_cfa_histogram_stat_t  *p_dst_img_hist_aaa_stat)
{
    INT32 rval = (INT32)IK_OK;
    UINT32 i,j,k;
    const ik_cfa_histo_cfg_info_t *p_src_hist_header;
    ik_cfa_histo_cfg_info_t *p_dst_hist_header;
    const ik_cfa_histogram_stat_t  *p_src_img_hist_aaa_stat;
    UINT32 src_idx;


    for(i=0; i< *p_src_stitch_num_y; i++) {
        for(j=0; j< *p_src_stitch_num_x; j++) { //Need know total slice

            src_idx = j + (*p_src_stitch_num_x *i);
            //p_src_img_hist_aaa_stat = (ik_cfa_histogram_stat_t  *)src_img_hist_aaa_stat_addr[src_idx];
            (void)amba_ik_system_memcpy(&p_src_img_hist_aaa_stat, &src_img_hist_aaa_stat_addr[src_idx], sizeof(ik_cfa_histogram_stat_t*));
            //AmbaPrint_PrintUInt5("############## SOURCE CFA hist p_src_stitch_num_x:%d, p_src_stitch_num_y:%d addr:%p, size:%d", j, i, src_img_hist_aaa_stat_addr[src_idx], sizeof(ik_cfa_histogram_stat_t), 0);
            //===== pointer init =====//
            p_src_hist_header = &p_src_img_hist_aaa_stat->cfg_info;
            p_dst_hist_header = &p_dst_img_hist_aaa_stat->cfg_info;

            //===== Cfa Header =====//

            if((i==0U)&&(j==0U)) {
                (void)amba_ik_system_memcpy(p_dst_hist_header, p_src_hist_header, sizeof(ik_cfa_histo_cfg_info_t));
            }
            if((i==((*p_src_stitch_num_y)-1UL))&&(j==((*p_src_stitch_num_x)-1UL))) {
                p_dst_hist_header->stats_width = p_src_hist_header->stats_left + p_src_hist_header->stats_width;
                p_dst_hist_header->stats_height = p_src_hist_header->stats_top + p_src_hist_header->stats_height;
            }
        }
    }


    for(i=0; i< *p_src_stitch_num_y; i++) {
        for(j=0; j< *p_src_stitch_num_x; j++) {

            src_idx = j + (*p_src_stitch_num_x *i);
            //p_src_img_hist_aaa_stat = (ik_cfa_histogram_stat_t  *)src_img_hist_aaa_stat_addr[src_idx];
            (void)amba_ik_system_memcpy(&p_src_img_hist_aaa_stat, &src_img_hist_aaa_stat_addr[src_idx], sizeof(ik_cfa_histogram_stat_t*));

            //===== histogram =====//
            {
                const ik_cfa_histogram_t *p_src_histogram;
                ik_cfa_histogram_t *p_dst_histogram;

                p_src_histogram = &p_src_img_hist_aaa_stat->cfa_hist;
                p_dst_histogram = &p_dst_img_hist_aaa_stat->cfa_hist;
                if((i == 0U)&&(j == 0U)) {
                    (void)amba_ik_system_memcpy(p_dst_histogram,p_src_histogram,sizeof(ik_cfa_histogram_t));
                } else {
                    for(k=0; k< 128U; k++) {
                        p_dst_histogram->histo_bin_r[k] += p_src_histogram->histo_bin_r[k];
                        p_dst_histogram->histo_bin_g[k] += p_src_histogram->histo_bin_g[k];
                        p_dst_histogram->histo_bin_b[k] += p_src_histogram->histo_bin_b[k];
                        if (*p_type==6U) {
                            p_dst_histogram->histo_bin_ir[k] += p_src_histogram->histo_bin_ir[k];
                        } else {
                            p_dst_histogram->histo_bin_ir[k] = 0UL;
                        }
                    }
                }
            }
        }
    }
    return rval;

}


UINT32  AmbaIK_TransferCFAAaaStatData(const void *In)
{
    UINT32 Rval = IK_OK;
    const DSP_EVENT_3A_TRANSFER_s *pAaaStatData;
    UINT32 CheckSource = 0;
    UINT32 i, j;
    static UINT32 cfa_count = 0;
    static UINT32 CFADebugFlag = 0UL;

    (void)amba_ik_system_memcpy(&pAaaStatData, &In, sizeof(void *));

    if(CFADebugFlag > 0U) {
        if((cfa_count%AAALogInterval)==0U) {
            for(i=0; i< pAaaStatData->SrcSliceY; i++) {
                for(j=0; j< pAaaStatData->SrcSliceX; j++) {
                    if(pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)] != 0U) {
                        AmbaPrint_PrintUInt5("[IK] =====source cfa slice[%d]==== 0x%X", j + (i * pAaaStatData->SrcSliceX), pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)], DC_U, DC_U, DC_U);
                        //img_3a_statistic_source_cfa_print((ik_cfa_3a_data_t *)pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)]);
                    } else {
                        //
                    }
                    AmbaPrint_PrintStr5("[IK] \n\n", DC_S, DC_S, DC_S, DC_S, DC_S);
                }
            }
        }
    }

    for(i=0; i< pAaaStatData->SrcSliceY; i++) {
        for(j=0; j< pAaaStatData->SrcSliceX; j++) {
            if(pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)] == 0U) {
                AmbaPrint_PrintUInt5("[IK] CheckSource, =====pAaaStatData->SrcAddr[%d] ==== 0x%X", j + (i * pAaaStatData->SrcSliceX), pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)], DC_U, DC_U, DC_U);
                CheckSource |= 1U;
            }
            if(pAaaStatData->DstAddr == 0U) {
                AmbaPrint_PrintUInt5("[IK] CheckSource, =====pAaaStatData->DstAddr ==== 0x%X", pAaaStatData->DstAddr, DC_U, DC_U, DC_U, DC_U);
                CheckSource |= 1U;
            }
        }
    }

    if( CheckSource == 0U) {
        ik_cfa_3a_data_t *p_ik_cfa_3a_data;
        (void)amba_ik_system_memcpy(&p_ik_cfa_3a_data, &pAaaStatData->DstAddr, sizeof(ik_cfa_3a_data_t *));
        Rval |= img_3a_process_cfa_statistic(&pAaaStatData->SrcSliceX, &pAaaStatData->SrcSliceY, pAaaStatData->SrcAddr, p_ik_cfa_3a_data);
        //AmbaPrint_PrintUInt5("############## DEST CFA 3a addr:%p, size:%d", pAaaStatData->DstAddr, sizeof(ik_cfa_3a_data_t), 0, 0, 0);
    }

    if(CFADebugFlag > 0U) {
        if((cfa_count%AAALogInterval)==0U) {
            AmbaPrint_PrintStr5("[IK] \n\n=====dst cfa====\n", DC_S, DC_S, DC_S, DC_S, DC_S);
            //img_3a_statistic_cfa_print((ik_cfa_3a_data_t *)pAaaStatData->DstAddr);
        }
    }
    cfa_count++;

    return Rval;
}

UINT32  AmbaIK_TransferPGAaaStatData(const void *In)
{
    UINT32     Rval = IK_OK;
    const DSP_EVENT_3A_TRANSFER_s    *pAaaStatData;
    UINT32  i, j;
    static UINT32 pg_count=0;
    UINT32  CheckSource = 0;
    static UINT32 PGDebugFlag = 0UL;

    (void)amba_ik_system_memcpy(&pAaaStatData, &In, sizeof(void *));

    if(PGDebugFlag > 0U) {
        if((pg_count%AAALogInterval)==0U) {
            for(i=0; i< pAaaStatData->SrcSliceY; i++) {
                for(j=0; j< pAaaStatData->SrcSliceX; j++) {
                    if(pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)] != 0U) {
                        AmbaPrint_PrintUInt5("[IK] =====source pg slice[%d]==== 0x%X\n", j + (i * pAaaStatData->SrcSliceX), pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)], DC_U, DC_U, DC_U);
                        //img_3a_statistic_source_pg_print((ik_pg_3a_data_t *)pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)]);
                    } else {
                        //
                    }
                    AmbaPrint_PrintStr5("[IK] \n\n", DC_S, DC_S, DC_S, DC_S, DC_S);
                }
            }
        }
    }

    for(i=0; i< pAaaStatData->SrcSliceY; i++) {
        for(j=0; j< pAaaStatData->SrcSliceX; j++) {

            if(pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)] == 0U) {
                AmbaPrint_PrintUInt5("[IK] CheckSource, =====pAaaStatData->SrcAddr[%d] ==== 0x%X\n", j + (i * pAaaStatData->SrcSliceX), pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)], DC_U, DC_U, DC_U);
                CheckSource |= 1U;
            }
            if(pAaaStatData->DstAddr == 0U) {
                AmbaPrint_PrintUInt5("[IK] CheckSource, =====pAaaStatData->DstAddr ==== 0x%X\n", pAaaStatData->DstAddr, DC_U, DC_U, DC_U, DC_U);
                CheckSource |= 1U;
            }
        }
    }

    if( CheckSource == 0U) {
        ik_pg_3a_data_t *p_ik_pg_3a_data;
        (void)amba_ik_system_memcpy(&p_ik_pg_3a_data, &pAaaStatData->DstAddr, sizeof(ik_pg_3a_data_t *));
        Rval |= img_3a_process_pg_statistic(&pAaaStatData->SrcSliceX, &pAaaStatData->SrcSliceY, pAaaStatData->SrcAddr, p_ik_pg_3a_data);
        //AmbaPrint_PrintUInt5("############## DEST PG 3a addr:%p, size:%d", pAaaStatData->DstAddr, sizeof(ik_pg_3a_data_t), 0, 0, 0);
    }

    if(PGDebugFlag > 0U) {
        if((pg_count%AAALogInterval)==0U) {
            AmbaPrint_PrintStr5("[IK] \n\n=====dst pg====\n", DC_S, DC_S, DC_S, DC_S, DC_S);
            //img_3a_statistic_pg_print((ik_pg_3a_data_t *)pAaaStatData->DstAddr);
        }
    }
    pg_count++;

    return Rval;

}

UINT32  AmbaIK_TransCFAHistStatData(const void *In)
{
    UINT32 Rval = IK_OK;
    const DSP_EVENT_3A_TRANSFER_s *pHistoStatData;
    static UINT32 hdr_count = 0;
    UINT32  i, j;
    ik_cfa_histogram_stat_t *p_ik_cfa_histogram_stat;
    static UINT32 HDRDebugFlag = 0UL;

    (void)amba_ik_system_memcpy(&pHistoStatData, &In, sizeof(void *));

    if(HDRDebugFlag == 1U) {
        if((hdr_count%AAALogInterval)==0U) {
            for(i=0; i< pHistoStatData->SrcSliceY; i++) {
                for(j=0; j< pHistoStatData->SrcSliceX; j++) {
                    AmbaPrint_PrintUInt5("[IK] =====source histo slice[%d]==== 0x%X", j + (i * pHistoStatData->SrcSliceX), pHistoStatData->SrcAddr[j + (i * pHistoStatData->SrcSliceX)], DC_U, DC_U, DC_U);
                    //img_3a_statistic_source_hist_print((ik_cfa_histogram_stat_t *)pHistoStatData->SrcAddr[j + (i * pHistoStatData->SrcSliceX)]);
                    AmbaPrint_PrintStr5("[IK] \n\n", DC_S, DC_S, DC_S, DC_S, DC_S);
                }
            }
        }
    }
    (void)amba_ik_system_memcpy(&p_ik_cfa_histogram_stat, &pHistoStatData->DstAddr, sizeof(ik_cfa_histogram_stat_t *));
    Rval |= (UINT32)img_3a_process_hist_statistic(&pHistoStatData->Type, &pHistoStatData->SrcSliceX, &pHistoStatData->SrcSliceY, pHistoStatData->SrcAddr, p_ik_cfa_histogram_stat);
    //AmbaPrint_PrintUInt5("############## DEST CFA hist addr:%p, size:%d", pHistoStatData->DstAddr, sizeof(ik_cfa_histogram_stat_t), 0, 0, 0);

    if(HDRDebugFlag == 1U) {
        if((hdr_count%AAALogInterval)==0U) {
            AmbaPrint_PrintStr5("[IK] \n\n=====dst histogram====\n", DC_S, DC_S, DC_S, DC_S, DC_S);
            //img_3a_statistic_hist_print((ik_cfa_histogram_stat_t *)pHistoStatData->DstAddr);
        }
    }
    hdr_count++;

    return Rval;

}
/*void AmbaIK_SetDebugAAAStatisitcDebugFlag(UINT8 CFA_PG, UINT32 AAASetDebugFlag, UINT32 Interval);

void AmbaIK_SetDebugAAAStatisitcDebugFlag(UINT8 CFA_PG, UINT32 AAASetDebugFlag, UINT32 Interval){

    if(CFA_PG == 0U){
        CFADebugFlag = AAASetDebugFlag;
        AmbaPrint_PrintStr5("[IK] Set Debug for AAA Cfa statistic on", DC_S, DC_S, DC_S, DC_S, DC_S);
    }else if(CFA_PG == 1U){
        PGDebugFlag = AAASetDebugFlag;
        AmbaPrint_PrintStr5("[IK] Set Debug for AAA Pg statistic on", DC_S, DC_S, DC_S, DC_S, DC_S);
    }else if(CFA_PG == 2U){
        HDRDebugFlag = AAASetDebugFlag;
        AmbaPrint_PrintStr5("[IK] Set Debug for AAA hdr statistic on", DC_S, DC_S, DC_S, DC_S, DC_S);
    }else{
        CFADebugFlag = 0;
        AmbaPrint_PrintUInt5("[IK] Unknow CFA_PG mode %d for statistic, disable it", CFA_PG, DC_U, DC_U, DC_U, DC_U);
    }
    AAALogInterval = Interval;

    img_set_3a_statisitc_debug_flag(CFA_PG, AAASetDebugFlag, Interval);

}*/


