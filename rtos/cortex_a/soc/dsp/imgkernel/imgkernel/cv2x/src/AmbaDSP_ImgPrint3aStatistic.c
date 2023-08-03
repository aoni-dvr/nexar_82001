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

//#include "stdio.h"  //sprintf // PP dirty mark
#include "ik_data_type.h"
//#include "AmbaPrint.h"
#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_Img3astatistic.h"//TBD

//#define amba_ik_system_print(...)// AmbaPrint  //TBD
#if (defined(__unix__) && !defined(__QNX__) && !defined(CONFIG_LINUX))
#include "stdio.h"
#define amba_ik_system_sprint sprintf
#else
#define amba_ik_system_sprint(...) 0//sprintf  //TBD
#endif

static uint8 g_ikc_aaa_cfa_header_flag = 0;
static uint8 g_ikc_aaa_cfa_awb_flag = 0;
static uint8 g_ikc_aaa_cfa_ae_flag = 0;
static uint8 g_ikc_aaa_cfa_af_flag = 0;
static uint8 g_ikc_aaa_cfa_histo_flag = 0;

static uint8 g_ikc_aaa_pg_header_flag = 0;
static uint8 g_ikc_aaa_pg_af_flag = 0;
static uint8 g_ikc_aaa_pg_ae_flag = 0;
static uint8 g_ikc_aaa_pg_histo_flag = 0;

static uint8 g_ikc_aaa_hdr_histo_flag = 0;

static void img_statistic_print_header(const ik_3a_header_t *p_header)
{
    if (p_header==NULL) {
        // unused parameter compile error??
    } else {
        amba_ik_system_print_uint32_5("[IK] total_slices_x %d", p_header->total_slices_x, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] total_slices_y %d", p_header->total_slices_y, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] slice_index_x %d", p_header->slice_index_x, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] slice_index_y %d", p_header->slice_index_y, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] slice_width %d", p_header->slice_width, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] slice_height %d", p_header->slice_height, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] slice_start_x %d", p_header->slice_start_x, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] slice_start_y %d", p_header->slice_start_y, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] chan_index %d", p_header->chan_index, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] raw_pic_seq_num %d", p_header->raw_pic_seq_num, DC_U, DC_U, DC_U, DC_U);

        amba_ik_system_print_uint32_5("[IK] awb_tile_num_col %d", p_header->awb_tile_num_col, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] awb_tile_num_row %d", p_header->awb_tile_num_row, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] ae_tile_num_col %d", p_header->ae_tile_num_col, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] ae_tile_num_row %d", p_header->ae_tile_num_row, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] af_tile_num_col %d", p_header->af_tile_num_col, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] af_tile_num_row %d", p_header->af_tile_num_row, DC_U, DC_U, DC_U, DC_U);

        amba_ik_system_print_uint32_5("[IK] awb_tile_col_start %d", p_header->awb_tile_col_start, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] awb_tile_row_start %d", p_header->awb_tile_row_start, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] awb_tile_width %d", p_header->awb_tile_width, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] awb_tile_height %d", p_header->awb_tile_height, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] awb_tile_active_width %d", p_header->awb_tile_active_width, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] awb_tile_active_height %d", p_header->awb_tile_active_height, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] awb_rgb_shift %d", p_header->awb_rgb_shift, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] awb_y_shift %d", p_header->awb_y_shift, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] awb_min_max_shift %d", p_header->awb_min_max_shift, DC_U, DC_U, DC_U, DC_U);

        amba_ik_system_print_uint32_5("[IK] ae_tile_col_start %d", p_header->ae_tile_col_start, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] ae_tile_row_start %d", p_header->ae_tile_row_start, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] ae_tile_width %d", p_header->ae_tile_width, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] ae_tile_height %d", p_header->ae_tile_height, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] ae_y_shift %d", p_header->ae_y_shift, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] ae_linear_y_shift %d", p_header->ae_linear_y_shift, DC_U, DC_U, DC_U, DC_U);

        amba_ik_system_print_uint32_5("[IK] af_tile_col_start %d", p_header->af_tile_col_start, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] af_tile_row_start %d", p_header->af_tile_row_start, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] af_tile_width %d", p_header->af_tile_width, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] af_tile_height %d", p_header->af_tile_height, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] af_tile_active_width %d", p_header->af_tile_active_width, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] af_tile_active_height %d", p_header->af_tile_active_height, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] af_y_shift %d", p_header->af_y_shift, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] af_cfa_y_shift %d", p_header->af_cfa_y_shift, DC_U, DC_U, DC_U, DC_U);
    }
}


void img_3a_statistic_source_cfa_print(ik_cfa_3a_data_t * p_src_cfa_aaa_stat)
{

    uint32  i,j,idx;
    uint32  w,h;
    char line_buf[512];
    uint8  *p_src_cfa;
    const char *line_buf_end = "\0";

    ik_3a_header_t *p_cfa_header;
    ik_cfa_awb_t *p_awb;
    ik_cfa_ae_t   *p_ae;
    ik_cfa_af_t   *p_af;
    const ik_cfa_histo_t *p_histo;

    p_cfa_header = &p_src_cfa_aaa_stat->header;

    p_src_cfa = (uint8 *)p_src_cfa_aaa_stat;

    p_src_cfa = p_src_cfa + sizeof(ik_3a_header_t) + sizeof(uint16);
    p_awb = (ik_cfa_awb_t *)p_src_cfa;

    p_src_cfa = p_src_cfa + ( sizeof(ik_cfa_awb_t) * p_cfa_header->awb_tile_num_row * p_cfa_header->awb_tile_num_col );
    p_ae = (ik_cfa_ae_t *)p_src_cfa;

    p_src_cfa = p_src_cfa + ( sizeof(ik_cfa_ae_t) * p_cfa_header->ae_tile_num_row * p_cfa_header->ae_tile_num_col );
    p_af = ( ik_cfa_af_t *)p_src_cfa;

    p_src_cfa = p_src_cfa + ( sizeof( ik_cfa_af_t) * p_cfa_header->af_tile_num_row * p_cfa_header->af_tile_num_col );
    p_histo = (ik_cfa_histo_t *)p_src_cfa;

    if(p_awb == NULL) {
        //TBD
    }
    if(p_ae == NULL) {
        //TBD
    }
    if(p_af == NULL) {
        //TBD
    }
    if(p_histo == NULL) {
        //TBD
    }

    if( g_ikc_aaa_cfa_header_flag == 1U ) {
        amba_ik_system_print_uint32_5("[IK] src_cfa_aaa_stat %p size %d Header size %d", p_src_cfa_aaa_stat, sizeof(ik_cfa_3a_data_t), sizeof(ik_3a_header_t), DC_U, DC_U, DC_U);
        img_statistic_print_header(p_cfa_header);
    }

    if( g_ikc_aaa_cfa_awb_flag == 1U ) {
        w = p_cfa_header->awb_tile_num_col;
        h = p_cfa_header->awb_tile_num_row;

        amba_ik_system_print_uint32_5("[IK] Current CFA AWB statistics (tile_num_col: %d, tile_num_row: %d)", w, h, DC_U, DC_U, DC_U);
        amba_ik_system_print_str_5("[IK] ---  Red statistics   ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w) + i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_awb[idx].sum_r);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---  Green statistics    ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_awb[idx].sum_g);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---  Blue statistics     ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_awb[idx].sum_b);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---  count_min_thresh   ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_awb[idx].count_min);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

        amba_ik_system_print_str_5("[IK] ---  count_max_thresh    ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_awb[idx].count_max);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
    }

    if( g_ikc_aaa_cfa_ae_flag == 1U ) {
        ////AE////
        w = p_cfa_header->ae_tile_num_col;
        h = p_cfa_header->ae_tile_num_row;

        amba_ik_system_print_uint32_5("[IK] Current CFA AE statistics (tile_num_col: %d, tile_num_row: %d)", w, h, DC_U, DC_U, DC_U);
        amba_ik_system_print_str_5("[IK] Current CFA AE lin_y", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_ae[idx].lin_y);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

        amba_ik_system_print_str_5("[IK] Current CFA AE count_min_thresh", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_ae[idx].count_min);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

        amba_ik_system_print_str_5("[IK] Current CFA AE count_max_thresh", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_ae[idx].count_max);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
    }

    if( g_ikc_aaa_cfa_af_flag == 1U ) {

        w = p_cfa_header->af_tile_num_col;
        h = p_cfa_header->af_tile_num_row;

        amba_ik_system_print_uint32_5("[IK] Current CFA AF statistics (tile_num_col: %d, tile_num_row: %d)", w, h, DC_U, DC_U, DC_U);
        amba_ik_system_print_str_5("[IK] ---  sum_y statistics    ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_af[idx].sum_y);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---  focus_value1 statistics     ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_af[idx].sum_fv1);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---  focus_value2 statistics     ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_af[idx].sum_fv2);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

    }

    if( g_ikc_aaa_cfa_histo_flag == 1U ) {
        ////histogram////
        amba_ik_system_print_str_5("[IK] Current CFA histogram statistics", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_str_5("[IK] ---  R histogram   ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j * 8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_r[idx]);
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---  G histogram     ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j * 8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_g[idx]);
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

        amba_ik_system_print_str_5("[IK] ---  B histogram     ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j * 8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_b[idx]);
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

        amba_ik_system_print_str_5("[IK] ---  Y histogram     ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j * 8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_y[idx]);
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ", DC_S, DC_S, DC_S, DC_S, DC_S);

    }

}


void img_3a_statistic_source_pg_print(ik_pg_3a_data_t * p_src_pg_aaa_stat)
{

    uint32  i,j,idx;
    uint32  w,h;
    char line_buf[512];
    uint8  *p_src_pg;
    const char *line_buf_end = "\0";

    ik_3a_header_t *p_pg_header;
    ik_pg_af_t        *p_af;
    ik_pg_ae_t        *p_ae;
    const ik_pg_histo_t     *p_histo;

    p_pg_header = &p_src_pg_aaa_stat->header;

    p_src_pg = (uint8 *)p_src_pg_aaa_stat;

    p_src_pg = p_src_pg + sizeof(ik_3a_header_t) + sizeof(uint16);
    p_af = (ik_pg_af_t *)p_src_pg;

    p_src_pg = p_src_pg + ( sizeof(ik_pg_af_t) * p_pg_header->af_tile_num_row * p_pg_header->af_tile_num_col );
    p_ae = (ik_pg_ae_t *)p_src_pg;

    p_src_pg = p_src_pg + ( sizeof(ik_pg_ae_t) * p_pg_header->ae_tile_num_row * p_pg_header->ae_tile_num_col );
    p_histo = (ik_pg_histo_t *)p_src_pg;

    if( g_ikc_aaa_pg_header_flag == 1U ) {
        amba_ik_system_print_uint32_5("[IK] src_pg_aaa_stat %p Header", p_src_pg_aaa_stat, DC_U, DC_U, DC_U, DC_U);
        img_statistic_print_header(p_pg_header);
    }
    if( g_ikc_aaa_pg_af_flag == 1U ) {
        w = p_pg_header->af_tile_num_col;
        h = p_pg_header->af_tile_num_row;

        amba_ik_system_print_uint32_5("[IK] Current PG AF statistics (tile_num_col: %d, tile_num_row: %d)", w, h, DC_U, DC_U, DC_U);
        amba_ik_system_print_str_5("[IK] ---  sum_y statistics    ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_af[idx].sum_y);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---  focus_value1 statistics     ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_af[idx].sum_fv1);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---  focus_value2 statistics     ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_af[idx].sum_fv2);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
    }
    if( g_ikc_aaa_pg_ae_flag == 1U ) {
        w = p_pg_header->ae_tile_num_col;
        h = p_pg_header->ae_tile_num_row;

        amba_ik_system_print_uint32_5("[IK] Current PG AE statistics (tile_num_col: %d, tile_num_row: %d)", w, h, DC_U, DC_U, DC_U);
        amba_ik_system_print_str_5("[IK] Current PG AE sum_y", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_ae[idx].sum_y);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
    }
    if( g_ikc_aaa_pg_histo_flag == 1U ) {
        amba_ik_system_print_str_5("[IK] Current PG histogram statistics", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_str_5("[IK] ---  R histogram   ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j*8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_r[idx]);
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---  G histogram     ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j*8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_g[idx]);
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

        amba_ik_system_print_str_5("[IK] ---  B histogram     ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j*8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_b[idx]);
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

        amba_ik_system_print_str_5("[IK] ---  Y histogram     ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j*8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_y[idx]);
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ", DC_S, DC_S, DC_S, DC_S, DC_S);

    }

}

void img_3a_statistic_source_hist_print(ik_cfa_histogram_stat_t * p_src_hist_aaa_stat)
{
    uint32  i,j,idx;
    char line_buf[512];
    const char *line_buf_end = "\0";

    const ik_cfa_histo_cfg_info_t *p_hist_cfg_info = &p_src_hist_aaa_stat->cfg_info;
    const ik_cfa_histogram_t *p_histogram = &p_src_hist_aaa_stat->cfa_hist;

    if(g_ikc_aaa_hdr_histo_flag == 1U ) {
        amba_ik_system_print_uint32_5("[IK] src_hist_aaa_stat %p Header",p_src_hist_aaa_stat, DC_U, DC_U, DC_U, DC_U);

        amba_ik_system_print_uint32_5("[IK] total_exposures %d",p_hist_cfg_info->total_exposures, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] vin_stats_type %d",p_hist_cfg_info->vin_stats_type, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] blend_index %d",p_hist_cfg_info->blend_index, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] stats_left %d",p_hist_cfg_info->stats_left, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] stats_width %d",p_hist_cfg_info->stats_width, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] stats_top %d",p_hist_cfg_info->stats_top, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] stats_height %d",p_hist_cfg_info->stats_height, DC_U, DC_U, DC_U, DC_U);

        ////histogram////
        amba_ik_system_print_str_5("[IK] Current histogram statistics", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_str_5("[IK] ---  R histogram   ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<16U; j++) {
            for (i=0; i<8U; i++) {
                idx = (j*8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histogram->histo_bin_r[idx]);
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---  G histogram       ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<16U; j++) {
            for (i=0; i<8U; i++) {
                idx = (j*8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histogram->histo_bin_g[idx]);
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

        amba_ik_system_print_str_5("[IK] ---  B histogram       ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<16U; j++) {
            for (i=0; i<8U; i++) {
                idx = (j*8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histogram->histo_bin_b[idx]);
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

    }
}

void img_3a_statistic_cfa_print(ik_cfa_3a_data_t * p_cfa_aaa_stat_t)
{

    uint32  i,j,idx;
    uint32  w,h;
    char line_buf[512];
    const char *line_buf_end = "\0";

    ik_3a_header_t *p_cfa_header;
    ik_cfa_awb_t *p_awb;
    ik_cfa_ae_t   *p_ae;
    ik_cfa_af_t   *p_af;
    const ik_cfa_histo_t *p_histo;

    uint32 total_count=0;

    p_cfa_header = &p_cfa_aaa_stat_t->header;

    p_awb = &p_cfa_aaa_stat_t->awb[0];

    p_ae = &p_cfa_aaa_stat_t->ae[0];

    p_af = &p_cfa_aaa_stat_t->af[0];

    p_histo = &p_cfa_aaa_stat_t->histogram;

    if( g_ikc_aaa_cfa_header_flag == 1U ) {
        amba_ik_system_print_uint32_5("[IK] Dst_cfa_aaa_stat %p Header", p_cfa_aaa_stat_t, DC_U, DC_U, DC_U, DC_U);
        img_statistic_print_header(p_cfa_header);
    }

    if( g_ikc_aaa_cfa_awb_flag == 1U ) {
        w = p_cfa_header->awb_tile_num_col;
        h = p_cfa_header->awb_tile_num_row;

        amba_ik_system_print_uint32_5("[IK] Current CFA AWB statistics (tile_num_col: %d, tile_num_row: %d)", w, h, DC_U, DC_U, DC_U);
        amba_ik_system_print_str_5("[IK] ---  Red statistics   ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_awb[idx].sum_r);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---  Green statistics    ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_awb[idx].sum_g);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---    Blue statistics  ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_awb[idx].sum_b);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---    count_min_thresh   ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_awb[idx].count_min);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

        amba_ik_system_print_str_5("[IK] ---  count_max_thresh    ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_awb[idx].count_max);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
    }

    if( g_ikc_aaa_cfa_ae_flag == 1U ) {
        ////AE////
        w = p_cfa_header->ae_tile_num_col;
        h = p_cfa_header->ae_tile_num_row;

        amba_ik_system_print_uint32_5("[IK] Current CFA AE statistics (tile_num_col: %d, tile_num_row: %d)", w, h, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_str_5("[IK] Current CFA AE lin_y", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_ae[idx].lin_y);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

        amba_ik_system_print_str_5("[IK] Current CFA AE count_min_thresh", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_ae[idx].count_min);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

        amba_ik_system_print_str_5("[IK] Current CFA AE count_max_thresh", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_ae[idx].count_max);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
    }

    if( g_ikc_aaa_cfa_af_flag == 1U ) {

        w = p_cfa_header->af_tile_num_col;
        h = p_cfa_header->af_tile_num_row;

        amba_ik_system_print_uint32_5("[IK] Current CFA AF statistics (tile_num_col: %d, tile_num_row: %d)", w, h, DC_U, DC_U, DC_U);
        amba_ik_system_print_str_5("[IK] ---  sum_y statistics    ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_af[idx].sum_y);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---    focus_value1 statistics  ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_af[idx].sum_fv1);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---    focus_value2 statistics  ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_af[idx].sum_fv2);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

    }

    if( g_ikc_aaa_cfa_histo_flag == 1U ) {
        ////histogram////
        amba_ik_system_print_str_5("[IK] Current CFA histogram statistics", DC_S, DC_S, DC_S, DC_S, DC_S);
        total_count = 0;
        amba_ik_system_print_str_5("[IK] ---  R histogram    ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j * 8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_r[idx]);
                total_count += p_histo->his_bin_r[idx];
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_uint32_5("[IK] R histogram total_count:%d", total_count, DC_U, DC_U, DC_U, DC_U);
        total_count = 0;
        amba_ik_system_print_str_5("[IK] ---    G histogram  ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j * 8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_g[idx]);
                total_count += p_histo->his_bin_g[idx];
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_uint32_5("[IK] G histogram total_count:%d", total_count, DC_U, DC_U, DC_U, DC_U);
        total_count = 0;
        amba_ik_system_print_str_5("[IK] ---    B histogram  ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j * 8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_b[idx]);
                total_count += p_histo->his_bin_b[idx];
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_uint32_5("[IK] B histogram total_count:%d", total_count, DC_U, DC_U, DC_U, DC_U);
        total_count = 0;
        amba_ik_system_print_str_5("[IK] ---    Y histogram  ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j * 8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_y[idx]);
                total_count += p_histo->his_bin_y[idx];
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_uint32_5("[IK] Y histogram total_count:%d", total_count, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_str_5("[IK] ", DC_S, DC_S, DC_S, DC_S, DC_S);

    }

}

void img_3a_statistic_pg_print(ik_pg_3a_data_t * p_pg_aaa_stat)
{

    uint32  i,j,idx;
    uint32  w,h;
    char line_buf[512];
    const char *line_buf_end = "\0";

    ik_3a_header_t *p_pg_header;
    ik_pg_af_t        *p_af;
    ik_pg_ae_t        *p_ae;
    const ik_pg_histo_t     *p_histo;
    uint32 total_count=0;

    p_pg_header = &p_pg_aaa_stat->header;

    p_af = &p_pg_aaa_stat->af[0];

    p_ae = &p_pg_aaa_stat->ae[0];

    p_histo = &p_pg_aaa_stat->histogram;

    if( g_ikc_aaa_pg_header_flag == 1U ) {
        amba_ik_system_print_uint32_5("[IK] dst_pg_aaa_stat %p Header", p_pg_aaa_stat, DC_U, DC_U, DC_U, DC_U);
        img_statistic_print_header(p_pg_header);
    }
    if( g_ikc_aaa_pg_af_flag == 1U ) {
        w = p_pg_header->af_tile_num_col;
        h = p_pg_header->af_tile_num_row;

        amba_ik_system_print_uint32_5("[IK] Current PG AF statistics (tile_num_col: %d, tile_num_row: %d)", w, h, DC_U, DC_U, DC_U);
        amba_ik_system_print_str_5("[IK] ---  sum_y statistics    ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_af[idx].sum_y);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---    focus_value1 statistics  ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_af[idx].sum_fv1);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---    focus_value2 statistics  ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_af[idx].sum_fv2);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
    }
    if( g_ikc_aaa_pg_ae_flag == 1U ) {
        w = p_pg_header->ae_tile_num_col;
        h = p_pg_header->ae_tile_num_row;

        amba_ik_system_print_uint32_5("[IK] Current PG AE statistics (tile_num_col: %d, tile_num_row: %d)", w, h, DC_U, DC_U, DC_U);
        amba_ik_system_print_str_5("[IK] Current PG AE sum_y", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<h; j++) {
            for (i=0; i<w; i++) {
                idx = (j*w)+ i;
                (void)amba_ik_system_sprint(&line_buf[6U*i],"%6d ",p_ae[idx].sum_y);
            }
            (void)amba_ik_system_sprint(&line_buf[6U*w],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
    }
    if( g_ikc_aaa_pg_histo_flag == 1U ) {
        amba_ik_system_print_str_5("[IK] Current PG histogram statistics", DC_S, DC_S, DC_S, DC_S, DC_S);
        total_count = 0;
        amba_ik_system_print_str_5("[IK] ---  R histogram    ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j*8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_r[idx]);
                total_count += p_histo->his_bin_r[idx];
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_uint32_5("[IK] R histogram total_count:%d", total_count, DC_U, DC_U, DC_U, DC_U);
        total_count = 0;
        amba_ik_system_print_str_5("[IK] ---    G histogram  ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j*8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_g[idx]);
                total_count += p_histo->his_bin_g[idx];
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_uint32_5("[IK] G histogram total_count:%d", total_count, DC_U, DC_U, DC_U, DC_U);
        total_count = 0;
        amba_ik_system_print_str_5("[IK] ---    B histogram  ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j*8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_b[idx]);
                total_count += p_histo->his_bin_b[idx];
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_uint32_5("[IK] B histogram total_count:%d", total_count, DC_U, DC_U, DC_U, DC_U);
        total_count = 0;
        amba_ik_system_print_str_5("[IK] ---    Y histogram  ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for ( j = 0; j < 8U; j++) {
            for ( i = 0; i < 8U; i++) {
                idx = (j*8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histo->his_bin_y[idx]);
                total_count += p_histo->his_bin_y[idx];
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_uint32_5("[IK] Y histogram total_count:%d", total_count, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_str_5("[IK] ", DC_S, DC_S, DC_S, DC_S, DC_S);

    }

}

void img_3a_statistic_hist_print(ik_cfa_histogram_stat_t * p_hist_aaa_stat)
{
    uint32  i,j,idx;
    char line_buf[512];
    const char *line_buf_end = "\0";

    const ik_cfa_histo_cfg_info_t *p_hist_cfg_info = &p_hist_aaa_stat->cfg_info;
    const ik_cfa_histogram_t *p_histogram = &p_hist_aaa_stat->cfa_hist;

    if(g_ikc_aaa_hdr_histo_flag == 1U ) {
        amba_ik_system_print_uint32_5("[IK] dst_hist_aaa_stat %p Header",p_hist_aaa_stat, DC_U, DC_U, DC_U, DC_U);

        amba_ik_system_print_uint32_5("[IK] total_exposures %d",p_hist_cfg_info->total_exposures, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] vin_stats_type %d",p_hist_cfg_info->vin_stats_type, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] blend_index %d",p_hist_cfg_info->blend_index, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] stats_left %d",p_hist_cfg_info->stats_left, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] stats_width %d",p_hist_cfg_info->stats_width, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] stats_top %d",p_hist_cfg_info->stats_top, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] stats_height %d",p_hist_cfg_info->stats_height, DC_U, DC_U, DC_U, DC_U);

        ////histogram////
        amba_ik_system_print_str_5("[IK] Current histogram statistics", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_str_5("[IK] ---  R histogram    ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<16U; j++) {
            for (i=0; i<8U; i++) {
                idx = (j*8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histogram->histo_bin_r[idx]);
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }
        amba_ik_system_print_str_5("[IK] ---    G histogram    ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<16U; j++) {
            for (i=0; i<8U; i++) {
                idx = (j*8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histogram->histo_bin_g[idx]);
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

        amba_ik_system_print_str_5("[IK] ---    B histogram    ---", DC_S, DC_S, DC_S, DC_S, DC_S);
        for (j=0; j<16U; j++) {
            for (i=0; i<8U; i++) {
                idx = (j*8U)+ i;
                (void)amba_ik_system_sprint(&line_buf[12U*i],"%12d ",p_histogram->histo_bin_b[idx]);
            }
            (void)amba_ik_system_sprint(&line_buf[12U*8U],"%s", line_buf_end);
            amba_ik_system_print_str_5("[IK] %s",line_buf, DC_S, DC_S, DC_S, DC_S);
        }

    }
}


void  img_set_3a_statisitc_debug_flag(uint8 cfa_pg, uint32 aaa_set_debug_flag, uint32 interval)
{
    if (cfa_pg == 0U) {
        g_ikc_aaa_cfa_header_flag = ((aaa_set_debug_flag & 0x1U) > 0U) ? 1U : 0U;
        g_ikc_aaa_cfa_awb_flag = ((aaa_set_debug_flag & 0x2U) > 0U) ? 1U : 0U;
        g_ikc_aaa_cfa_ae_flag = ((aaa_set_debug_flag & 0x4U) > 0U) ? 1U : 0U;
        g_ikc_aaa_cfa_af_flag = ((aaa_set_debug_flag & 0x8U) > 0U) ? 1U : 0U;
        g_ikc_aaa_cfa_histo_flag = ((aaa_set_debug_flag & 0x10U) > 0U) ? 1U : 0U;

        amba_ik_system_print_uint32_5("[IK] g_ikc_aaa_cfa_header_flag %d", g_ikc_aaa_cfa_header_flag, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] g_ikc_aaa_cfa_awb_flag %d", g_ikc_aaa_cfa_awb_flag, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] g_ikc_aaa_cfa_ae_flag %d", g_ikc_aaa_cfa_ae_flag, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] g_ikc_aaa_cfa_af_flag %d", g_ikc_aaa_cfa_af_flag, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] g_ikc_aaa_cfa_histo_flag %d", g_ikc_aaa_cfa_histo_flag, DC_U, DC_U, DC_U, DC_U);

    } else if(cfa_pg == 1U) {
        g_ikc_aaa_pg_header_flag = ((aaa_set_debug_flag & 0x1U) > 0U) ? 1U : 0U;
        g_ikc_aaa_pg_histo_flag = ((aaa_set_debug_flag & 0x2U) > 0U) ? 1U : 0U;
        g_ikc_aaa_pg_ae_flag = ((aaa_set_debug_flag & 0x4U) > 0U) ? 1U : 0U;
        g_ikc_aaa_pg_af_flag = ((aaa_set_debug_flag & 0x8U) > 0U) ? 1U : 0U;

        amba_ik_system_print_uint32_5("[IK] g_ikc_aaa_pg_header_flag %d", g_ikc_aaa_pg_header_flag, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] g_ikc_aaa_pg_histo_flag %d", g_ikc_aaa_pg_histo_flag, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] g_ikc_aaa_pg_ae_flag %d", g_ikc_aaa_pg_ae_flag, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK] g_ikc_aaa_pg_af_flag %d", g_ikc_aaa_pg_af_flag, DC_U, DC_U, DC_U, DC_U);

    } else if(cfa_pg == 2U) {
        g_ikc_aaa_hdr_histo_flag = ((aaa_set_debug_flag & 0x1U) > 0U) ? 1U : 0U;
        amba_ik_system_print_uint32_5("[IK] g_ikc_aaa_hdr_histo_flag %d", g_ikc_aaa_hdr_histo_flag, DC_U, DC_U, DC_U, DC_U);
    }
    amba_ik_system_print_uint32_5("[IK] interval %d", interval, DC_U, DC_U, DC_U, DC_U);
}



