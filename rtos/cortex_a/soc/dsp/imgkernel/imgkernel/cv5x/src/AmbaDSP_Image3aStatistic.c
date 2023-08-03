/*
*  @file AmbaDSP_Image3aStatistic.c
*
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
* MERCHANTABILITY, AND FITNESS FOR A PARTICUAR PURPOSE ARE DISCLAIMED.
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
#include "AmbaPrint.h"
#include "AmbaDSP_Img3astatistic.h"
#include "AmbaWrap.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageFilterInternal.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Image3aStatistics.h"
#include "AmbaDSP_ImgArchSystemAPI.h"

#define DC_U    99999U /*PRINT DONT CARE FOR UINT*/
//#define DC_S    NULL

//static UINT32 CFADebugFlag = 0U;
//static UINT32 PGDebugFlag = 0U;
//static UINT32 HDRDebugFlag = 0U;
//static UINT32 AAALogInterval = 120U;

static inline UINT32 GetU64Msb(uintptr U64Val)
{
    return (UINT32)((U64Val >> 32U) & 0xFFFFFFFFU);
}
static inline UINT32 GetU64Lsb(uintptr U64Val)
{
    return (UINT32)(U64Val & 0xFFFFFFFFU);
}

static inline uint8 equal_op_u32(uint32 compare1, uint32 compare2)
{
    return (compare1 == compare2) ? 1U : 0U;
}

static inline uint8 greater_op_u32(uint32 compare1, uint32 compare2)
{
    return (compare1 > compare2) ? 1U : 0U;
}

static UINT32 img_3a_process_header(const UINT8 *p_src_stitch_num_x,
                                    const UINT8 *p_src_stitch_num_y,
                                    const ULONG  *src_img_aaa_stat_addr,
                                    AMBA_IK_3A_HEADER_s  *p_dst_aaa_header)
{
    UINT32  rval = IK_OK;
    UINT32  i,j;
    const ik_3a_header_t *p_src_aaa_header;
    UINT32 src_idx;
    uint8 bool_result;

    //update dist header.
    for(i=0; i< *p_src_stitch_num_y; i++) {
        for(j=0; j< *p_src_stitch_num_x; j++) { //Need know total slice

            src_idx = j + (*p_src_stitch_num_x *i);
            (void)amba_ik_system_memcpy(&p_src_aaa_header, &src_img_aaa_stat_addr[src_idx],sizeof(ik_3a_header_t *));

            //===== Cfa Header =====//

            if((i==0U)&&(j==0U)) {
                (void)amba_ik_system_memcpy(p_dst_aaa_header, p_src_aaa_header, sizeof(ik_3a_header_t));

                p_dst_aaa_header->VinHistogram.BayerPattern = (UINT8)p_src_aaa_header->vin_histogram.bayer_pattern;
                p_dst_aaa_header->VinHistogram.IrMode = (UINT8)p_src_aaa_header->vin_histogram.ir_mode;
                p_dst_aaa_header->VinHistogram.CropEn = (UINT8)p_src_aaa_header->vin_histogram.crop_en;
                p_dst_aaa_header->VinHistogram.Reserved0 = 0U;
                p_dst_aaa_header->VinHistogram.CropColStart = (UINT16)p_src_aaa_header->vin_histogram.crop_col_start;
                p_dst_aaa_header->VinHistogram.CropRowStart = (UINT16)p_src_aaa_header->vin_histogram.crop_row_start;
                p_dst_aaa_header->VinHistogram.CropWidth = (UINT16)p_src_aaa_header->vin_histogram.crop_width+1U;
                p_dst_aaa_header->VinHistogram.CropHeight = (UINT16)p_src_aaa_header->vin_histogram.crop_height+1U;

                p_dst_aaa_header->Awb.AwbEnable = (UINT8)p_src_aaa_header->awb.awb_enable;
                p_dst_aaa_header->Awb.AwbTileNumCol = (UINT8)p_src_aaa_header->awb.awb_tile_num_col+1U;
                p_dst_aaa_header->Awb.AwbTileNumRow = (UINT8)p_src_aaa_header->awb.awb_tile_num_row+1U;
                p_dst_aaa_header->Awb.Reserved = 0U;
                p_dst_aaa_header->Awb.AwbTileColStart = (UINT16)p_src_aaa_header->awb.awb_tile_col_start;
                p_dst_aaa_header->Awb.AwbTileRowStart = (UINT16)p_src_aaa_header->awb.awb_tile_row_start;
                p_dst_aaa_header->Awb.AwbTileWidth = (UINT16)p_src_aaa_header->awb.awb_tile_width+1U;
                p_dst_aaa_header->Awb.AwbTileHeight = (UINT16)p_src_aaa_header->awb.awb_tile_height+1U;
                p_dst_aaa_header->Awb.AwbTileActiveWidth = (UINT16)p_src_aaa_header->awb.awb_tile_active_width+1U;
                p_dst_aaa_header->Awb.AwbTileActiveHeight = (UINT16)p_src_aaa_header->awb.awb_tile_active_height+1U;

                p_dst_aaa_header->CfaAe.AeEnable = (UINT8)p_src_aaa_header->cfa_ae.ae_enable;
                p_dst_aaa_header->CfaAe.AeTileNumCol = (UINT8)p_src_aaa_header->cfa_ae.ae_tile_num_col+1U;
                p_dst_aaa_header->CfaAe.AeTileNumRow = (UINT8)p_src_aaa_header->cfa_ae.ae_tile_num_row+1U;
                p_dst_aaa_header->CfaAe.Reserved = 0U;
                p_dst_aaa_header->CfaAe.AeTileColStart = (UINT16)p_src_aaa_header->cfa_ae.ae_tile_col_start;
                p_dst_aaa_header->CfaAe.AeTileRowStart = (UINT16)p_src_aaa_header->cfa_ae.ae_tile_row_start;
                p_dst_aaa_header->CfaAe.AeTileWidth = (UINT16)p_src_aaa_header->cfa_ae.ae_tile_width+1U;
                p_dst_aaa_header->CfaAe.AeTileHeight = (UINT16)p_src_aaa_header->cfa_ae.ae_tile_height+1U;
                p_dst_aaa_header->CfaAe.AeMinThresh = (UINT16)p_src_aaa_header->cfa_ae.ae_min_thresh;
                p_dst_aaa_header->CfaAe.AeMaxThresh = (UINT16)p_src_aaa_header->cfa_ae.ae_max_thresh;

                p_dst_aaa_header->PgAe.AeEnable = (UINT8)p_src_aaa_header->pg_ae.ae_enable;
                p_dst_aaa_header->PgAe.AeTileNumCol = (UINT8)p_src_aaa_header->pg_ae.ae_tile_num_col+1U;
                p_dst_aaa_header->PgAe.AeTileNumRow = (UINT8)p_src_aaa_header->pg_ae.ae_tile_num_row+1U;
                p_dst_aaa_header->PgAe.AeTileYShift = (UINT8)p_src_aaa_header->pg_ae.ae_tile_y_shift;
                p_dst_aaa_header->PgAe.AeTileColStart = (UINT16)p_src_aaa_header->pg_ae.ae_tile_col_start;
                p_dst_aaa_header->PgAe.AeTileRowStart = (UINT16)p_src_aaa_header->pg_ae.ae_tile_row_start;
                p_dst_aaa_header->PgAe.AeTileWidth = (UINT16)p_src_aaa_header->pg_ae.ae_tile_width+1U;
                p_dst_aaa_header->PgAe.AeTileHeight = (UINT16)p_src_aaa_header->pg_ae.ae_tile_height+1U;
                p_dst_aaa_header->PgAe.Reserved = 0U;

                p_dst_aaa_header->CfaAf.AfEnable = (UINT8)p_src_aaa_header->cfa_af.af_enable;
                p_dst_aaa_header->CfaAf.AfTileNumCol = (UINT8)p_src_aaa_header->cfa_af.af_tile_num_col+1U;
                p_dst_aaa_header->CfaAf.AfTileNumRow = (UINT8)p_src_aaa_header->cfa_af.af_tile_num_row+1U;
                p_dst_aaa_header->CfaAf.Reserved = 0U;
                p_dst_aaa_header->CfaAf.AfTileColStart = (UINT16)p_src_aaa_header->cfa_af.af_tile_col_start;
                p_dst_aaa_header->CfaAf.AfTileRowStart = (UINT16)p_src_aaa_header->cfa_af.af_tile_row_start;
                p_dst_aaa_header->CfaAf.AfTileWidth = (UINT16)p_src_aaa_header->cfa_af.af_tile_width+1U;
                p_dst_aaa_header->CfaAf.AfTileHeight = (UINT16)p_src_aaa_header->cfa_af.af_tile_height+1U;
                p_dst_aaa_header->CfaAf.AfTileActiveWidth = (UINT16)p_src_aaa_header->cfa_af.af_tile_active_width+1U;
                p_dst_aaa_header->CfaAf.AfTileActiveHeight = (UINT16)p_src_aaa_header->cfa_af.af_tile_active_height+1U;

                p_dst_aaa_header->PgAf.AfEnable = (UINT8)p_src_aaa_header->pg_af.af_enable;
                p_dst_aaa_header->PgAf.AfTileNumCol = (UINT8)p_src_aaa_header->pg_af.af_tile_num_col+1U;
                p_dst_aaa_header->PgAf.AfTileNumRow = (UINT8)p_src_aaa_header->pg_af.af_tile_num_row+1U;
                p_dst_aaa_header->PgAf.Reserved = 0U;
                p_dst_aaa_header->PgAf.AfTileColStart = (UINT16)p_src_aaa_header->pg_af.af_tile_col_start;
                p_dst_aaa_header->PgAf.AfTileRowStart = (UINT16)p_src_aaa_header->pg_af.af_tile_row_start;
                p_dst_aaa_header->PgAf.AfTileWidth = (UINT16)p_src_aaa_header->pg_af.af_tile_width+1U;
                p_dst_aaa_header->PgAf.AfTileHeight = (UINT16)p_src_aaa_header->pg_af.af_tile_height+1U;
                p_dst_aaa_header->PgAf.AfTileActiveWidth = (UINT16)p_src_aaa_header->pg_af.af_tile_active_width+1U;
                p_dst_aaa_header->PgAf.AfTileActiveHeight = (UINT16)p_src_aaa_header->pg_af.af_tile_active_height+1U;

            } else {
                if(i == 0U) {
                    p_dst_aaa_header->VinHistogram.CropWidth += p_src_aaa_header->vin_histogram.crop_width+1U;
                    p_dst_aaa_header->Awb.AwbTileNumCol += p_src_aaa_header->awb.awb_tile_num_col+1U;
                    p_dst_aaa_header->CfaAe.AeTileNumCol += p_src_aaa_header->cfa_ae.ae_tile_num_col+1U;
                    p_dst_aaa_header->PgAe.AeTileNumCol += p_src_aaa_header->pg_ae.ae_tile_num_col+1U;
                    p_dst_aaa_header->CfaAf.AfTileNumCol += p_src_aaa_header->cfa_af.af_tile_num_col+1U;
                    p_dst_aaa_header->PgAf.AfTileNumCol += p_src_aaa_header->pg_af.af_tile_num_col+1U;
                }
                if(j == 0U) {
                    p_dst_aaa_header->VinHistogram.CropHeight += p_src_aaa_header->vin_histogram.crop_height+1U;
                    p_dst_aaa_header->Awb.AwbTileNumRow += p_src_aaa_header->awb.awb_tile_num_row+1U;
                    p_dst_aaa_header->CfaAe.AeTileNumRow += p_src_aaa_header->cfa_ae.ae_tile_num_row+1U;
                    p_dst_aaa_header->PgAe.AeTileNumRow += p_src_aaa_header->pg_ae.ae_tile_num_row+1U;
                    p_dst_aaa_header->CfaAf.AfTileNumRow += p_src_aaa_header->cfa_af.af_tile_num_row+1U;
                    p_dst_aaa_header->PgAf.AfTileNumRow += p_src_aaa_header->pg_af.af_tile_num_row+1U;
                }
            }
        }
    }
    bool_result = greater_op_u32((uint32)p_dst_aaa_header->Awb.AwbTileNumCol, AMBA_IK_3A_AWB_TILE_COLUMN_COUNT) +
                  greater_op_u32((uint32)p_dst_aaa_header->Awb.AwbTileNumRow, AMBA_IK_3A_AWB_TILE_ROW_COUNT);
    if(bool_result != 0U) {
        AmbaPrint_PrintUInt5("[IK] AAA transform error, Combined AWB tiles %d x %d > Maximum %d x %d\n",
                             p_dst_aaa_header->Awb.AwbTileNumCol, p_dst_aaa_header->Awb.AwbTileNumRow,
                             AMBA_IK_3A_AWB_TILE_COLUMN_COUNT, AMBA_IK_3A_AWB_TILE_ROW_COUNT, DC_U);
        rval = IK_ERR_0103;
    }

    bool_result = greater_op_u32((uint32)p_dst_aaa_header->CfaAe.AeTileNumCol, AMBA_IK_3A_AE_TILE_COLUMN_COUNT) +
                  greater_op_u32((uint32)p_dst_aaa_header->CfaAe.AeTileNumRow, AMBA_IK_3A_AE_TILE_ROW_COUNT);
    if(bool_result != 0U) {
        AmbaPrint_PrintUInt5("[IK] AAA transform error, Combined CFA AE tiles %d x %d > Maximum %d x %d\n",
                             p_dst_aaa_header->CfaAe.AeTileNumCol, p_dst_aaa_header->CfaAe.AeTileNumRow,
                             AMBA_IK_3A_AE_TILE_COLUMN_COUNT, AMBA_IK_3A_AE_TILE_ROW_COUNT, DC_U);
        rval = IK_ERR_0103;
    }

    bool_result = greater_op_u32((uint32)p_dst_aaa_header->CfaAf.AfTileNumCol, AMBA_IK_3A_CFA_AF_TILE_COLUMN_COUNT) +
                  greater_op_u32((uint32)p_dst_aaa_header->CfaAf.AfTileNumRow, AMBA_IK_3A_AF_TILE_ROW_COUNT);
    if(bool_result != 0U) {
        AmbaPrint_PrintUInt5("[IK] AAA transform error, Combined CFA AF tiles %d x %d > Maximum %d x %d\n",
                             p_dst_aaa_header->CfaAf.AfTileNumCol, p_dst_aaa_header->CfaAf.AfTileNumRow,
                             AMBA_IK_3A_CFA_AF_TILE_COLUMN_COUNT, AMBA_IK_3A_AF_TILE_ROW_COUNT, DC_U);
        rval = IK_ERR_0103;
    }

    bool_result = greater_op_u32((uint32)p_dst_aaa_header->PgAe.AeTileNumCol, AMBA_IK_3A_AE_TILE_COLUMN_COUNT) +
                  greater_op_u32((uint32)p_dst_aaa_header->PgAe.AeTileNumRow, AMBA_IK_3A_AE_TILE_ROW_COUNT);
    if(bool_result != 0U) {
        AmbaPrint_PrintUInt5("[IK] AAA transform error, Combined PG AE tiles %d x %d > Maximum %d x %d\n",
                             p_dst_aaa_header->PgAe.AeTileNumCol, p_dst_aaa_header->PgAe.AeTileNumRow,
                             AMBA_IK_3A_AE_TILE_COLUMN_COUNT, AMBA_IK_3A_AE_TILE_ROW_COUNT, DC_U);
        rval = IK_ERR_0103;
    }

    bool_result = greater_op_u32((uint32)p_dst_aaa_header->PgAf.AfTileNumCol, AMBA_IK_3A_PG_AF_TILE_COLUMN_COUNT) +
                  greater_op_u32((uint32)p_dst_aaa_header->PgAf.AfTileNumRow, AMBA_IK_3A_AF_TILE_ROW_COUNT);
    if(bool_result != 0U) {
        AmbaPrint_PrintUInt5("[IK] AAA transform error, Combined PG AF tiles %d x %d > Maximum %d x %d\n",
                             p_dst_aaa_header->PgAf.AfTileNumCol, p_dst_aaa_header->PgAf.AfTileNumRow,
                             AMBA_IK_3A_PG_AF_TILE_COLUMN_COUNT, AMBA_IK_3A_AF_TILE_ROW_COUNT, DC_U);
        rval = IK_ERR_0103;
    }

    return rval;
}

static UINT32  img_3a_process_cfa_statistic(const UINT8 *p_src_stitch_num_x,
        const UINT8 *p_src_stitch_num_y,
        const ULONG  *src_img_cfa_aaa_stat_addr,
        AMBA_IK_CFA_3A_DATA_s  *p_dst_img_cfa_aaa_stat)
{

    UINT32  rval = IK_OK;
    const void *p_src_cfa;
    UINT32  i,j,k;
    const ik_3a_header_t *p_src_cfa_header;
    AMBA_IK_3A_HEADER_s *p_dst_cfa_header;
    const ik_cfa_3a_data_t *p_src_img_cfa_aaa_stat;
    UINT32 src_idx;
    ULONG misra_addr;
    UINT32 total_awb_width = 0;
    UINT32 total_cfa_ae_width = 0;
    UINT32 total_cfa_af_width = 0;
    //UINT32 total_pg_ae_width = 0;
    //UINT32 total_pg_af_width = 0;
    uint8 bool_result;
    //update dist header.
    p_dst_cfa_header = &p_dst_img_cfa_aaa_stat->Header;

    rval = img_3a_process_header(p_src_stitch_num_x, p_src_stitch_num_y, src_img_cfa_aaa_stat_addr, p_dst_cfa_header);
    if (rval == IK_OK) {
        (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[0],sizeof(ik_cfa_3a_data_t*));
        p_dst_img_cfa_aaa_stat ->FrameId = p_src_img_cfa_aaa_stat->frame_id;

        total_awb_width = p_dst_cfa_header->Awb.AwbTileNumCol;
        total_cfa_ae_width = p_dst_cfa_header->CfaAe.AeTileNumCol;
        //total_pg_ae_width = p_dst_cfa_header->PgAe.AeTileNumCol;
        total_cfa_af_width = p_dst_cfa_header->CfaAf.AfTileNumCol;
        //total_pg_af_width = p_dst_cfa_header->PgAf.AfTileNumCol;

        for(i=0; i< *p_src_stitch_num_y; i++) {
            for(j=0; j< *p_src_stitch_num_x; j++) { //Need know total slice

                src_idx = j + (*p_src_stitch_num_x *i);
                //p_src_img_cfa_aaa_stat = (ik_cfa_3a_data_t  *)src_img_cfa_aaa_stat_addr[src_idx];
                (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[src_idx],sizeof(ik_cfa_3a_data_t  *));


                //===== pointer init =====//

                //p_src_cfa = p_src_img_cfa_aaa_stat;
                //p_src_cfa_header = (ik_3a_header_t *)p_src_cfa;
                (void)amba_ik_system_memcpy(&p_src_cfa, &p_src_img_cfa_aaa_stat, sizeof(void *));
                (void)amba_ik_system_memcpy(&p_src_cfa_header, &p_src_cfa, sizeof(ik_3a_header_t *));

                //===== Cfa Awb =====//
                {
                    const ik_cfa_awb_t *p_src_awb;
                    ik_cfa_awb_t *p_dst_awb;
                    UINT32 offset = 0;
                    UINT32 m;
                    const void *misra_ptr;

                    //    for(m = 0; m < i;m++)
                    //        for(n = 0; n < stitch_tile_x; n++)
                    //        offset += p_dst_cfa_header->awb_tile_num_col * p_src_img_cfa_aaa_stat[m][n]->header.awb_tile_num_col;

                    for(m = 0; m < j; m++) {
                        //p_src_img_cfa_aaa_stat = (ik_cfa_3a_data_t  *)src_img_cfa_aaa_stat_addr[m];
                        (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[m],sizeof(ik_cfa_3a_data_t  *));
                        offset += ((UINT32)p_src_img_cfa_aaa_stat->header.awb.awb_tile_num_col+1U);
                    }
                    for(m = 0; m < i; m++) {
                        (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[*p_src_stitch_num_x * m],sizeof(ik_cfa_3a_data_t  *));
                        offset += ((UINT32)p_src_img_cfa_aaa_stat->header.awb.awb_tile_num_row+1U) * total_awb_width;
                    }

                    //p_dst_awb = &p_dst_img_cfa_aaa_stat->awb[0] + offset;
                    //p_src_cfa = p_src_cfa + sizeof(ik_3a_header_t) + sizeof(UINT16);
                    //p_src_awb = (ik_cfa_awb_t *)p_src_cfa;

                    misra_ptr = &p_dst_img_cfa_aaa_stat->Awb[0];
                    (void)amba_ik_system_memcpy(&p_dst_awb, &misra_ptr, sizeof(void*));
                    (void)amba_ik_system_memcpy(&misra_addr, &p_dst_awb, sizeof(void*));
                    misra_addr = misra_addr + (offset * sizeof(ik_cfa_awb_t));
                    (void)amba_ik_system_memcpy(&p_dst_awb, &misra_addr, sizeof(void*));

                    (void)amba_ik_system_memcpy(&misra_addr, &p_src_cfa, sizeof(void*));
                    misra_addr = misra_addr + sizeof(ik_3a_header_t) + sizeof(UINT16);
                    (void)amba_ik_system_memcpy(&p_src_cfa, &misra_addr, sizeof(void*));
                    (void)amba_ik_system_memcpy(&p_src_awb, &p_src_cfa, sizeof(ik_cfa_awb_t *));

                    for(k=0; k < ((UINT32)p_src_cfa_header->awb.awb_tile_num_row+1U); k++) {
                        (void)amba_ik_system_memcpy(p_dst_awb, p_src_awb, sizeof(ik_cfa_awb_t)*((UINT32)p_src_cfa_header->awb.awb_tile_num_col+1UL));
                        //p_src_awb = p_src_awb + p_src_cfa_header->awb_tile_num_col;
                        //p_dst_awb = p_dst_awb + p_dst_cfa_header->awb_tile_num_col;

                        (void)amba_ik_system_memcpy(&misra_addr, &p_src_awb, sizeof(void*));
                        misra_addr = misra_addr + (((UINT32)p_src_cfa_header->awb.awb_tile_num_col+1UL) * sizeof(ik_cfa_awb_t));
                        (void)amba_ik_system_memcpy(&p_src_awb, &misra_addr, sizeof(void*));

                        (void)amba_ik_system_memcpy(&misra_addr, &p_dst_awb, sizeof(void*));
                        misra_addr = misra_addr + (p_dst_cfa_header->Awb.AwbTileNumCol * sizeof(ik_cfa_awb_t));
                        (void)amba_ik_system_memcpy(&p_dst_awb, &misra_addr, sizeof(void*));
                    }
                }
                //===== Cfa Ae =====//
                {
                    const ik_cfa_ae_t *p_src_ae;
                    ik_cfa_ae_t *p_dst_ae;
                    UINT32 offset = 0;
                    UINT32  m;//,n=0;
                    const void *misra_ptr;

                    //    for(m = 0; m < i;m++)
                    //        for(n = 0; n < stitch_tile_x; n++)
                    //        offset += p_dst_cfa_header->awb_tile_num_col * p_src_img_cfa_aaa_stat[m][n]->header.awb_tile_num_col;

                    for(m = 0; m < j; m++) {
                        //p_src_img_cfa_aaa_stat = (ik_cfa_3a_data_t  *)src_img_cfa_aaa_stat_addr[m];
                        (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[m],sizeof(ik_cfa_3a_data_t  *));
                        offset += ((UINT32)p_src_img_cfa_aaa_stat->header.cfa_ae.ae_tile_num_col+1U);
                    }
                    for(m = 0; m < i; m++) {
                        (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[*p_src_stitch_num_x * m],sizeof(ik_cfa_3a_data_t  *));
                        offset += ((UINT32)p_src_img_cfa_aaa_stat->header.cfa_ae.ae_tile_num_row+1U) * total_cfa_ae_width;
                    }

                    misra_ptr = &p_dst_img_cfa_aaa_stat->Ae[0];
                    (void)amba_ik_system_memcpy(&p_dst_ae, &misra_ptr, sizeof(void*));
                    (void)amba_ik_system_memcpy(&misra_addr, &p_dst_ae, sizeof(void*));
                    misra_addr = misra_addr + (offset * sizeof(ik_cfa_ae_t));
                    (void)amba_ik_system_memcpy(&p_dst_ae, &misra_addr, sizeof(void*));

                    (void)amba_ik_system_memcpy(&misra_addr, &p_src_cfa, sizeof(void*));
                    misra_addr = misra_addr + (sizeof(ik_cfa_awb_t) * ((UINT32)p_src_cfa_header->awb.awb_tile_num_row+1UL) * ((UINT32)p_src_cfa_header->awb.awb_tile_num_col+1UL));
                    (void)amba_ik_system_memcpy(&p_src_cfa, &misra_addr, sizeof(void*));
                    (void)amba_ik_system_memcpy(&p_src_ae, &p_src_cfa, sizeof(ik_cfa_ae_t *));

                    for(k=0; k<((UINT32)p_src_cfa_header->cfa_ae.ae_tile_num_row+1U); k++) {
                        (void)amba_ik_system_memcpy(p_dst_ae, p_src_ae, sizeof(ik_cfa_ae_t)*((UINT32)p_src_cfa_header->cfa_ae.ae_tile_num_col+1UL));
                        //p_src_ae = p_src_ae + p_src_cfa_header->ae_tile_num_col;
                        //p_dst_ae = p_dst_ae + p_dst_cfa_header->ae_tile_num_col;

                        (void)amba_ik_system_memcpy(&misra_addr, &p_src_ae, sizeof(void*));
                        misra_addr = misra_addr + (((UINT32)p_src_cfa_header->cfa_ae.ae_tile_num_col+1UL) * sizeof(ik_cfa_ae_t));
                        (void)amba_ik_system_memcpy(&p_src_ae, &misra_addr, sizeof(void*));

                        (void)amba_ik_system_memcpy(&misra_addr, &p_dst_ae, sizeof(void*));
                        misra_addr = misra_addr + (p_dst_cfa_header->CfaAe.AeTileNumCol * sizeof(ik_cfa_ae_t));
                        (void)amba_ik_system_memcpy(&p_dst_ae, &misra_addr, sizeof(void*));
                    }
                }

                //===== Cfa Af =====//
                {
                    const ik_cfa_af_t *p_src_af;
                    ik_cfa_af_t *p_dst_af;
                    UINT32 offset = 0;
                    UINT32  m;//,n=0;
                    const void *misra_ptr;

                    //    for(m = 0; m < i;m++)
                    //        for(n = 0; n < stitch_tile_x; n++)
                    //        offset += p_dst_cfa_header->awb_tile_num_col * p_src_img_cfa_aaa_stat[m][n]->header.awb_tile_num_col;

                    for(m = 0; m < j; m++) {
                        //p_src_img_cfa_aaa_stat = (ik_cfa_3a_data_t  *)src_img_cfa_aaa_stat_addr[m];
                        (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[m],sizeof(ik_cfa_3a_data_t  *));
                        offset += ((UINT32)p_src_img_cfa_aaa_stat->header.cfa_af.af_tile_num_col+1U);
                    }
                    for(m = 0; m < i; m++) {
                        (void)amba_ik_system_memcpy(&p_src_img_cfa_aaa_stat, &src_img_cfa_aaa_stat_addr[*p_src_stitch_num_x * m],sizeof(ik_cfa_3a_data_t  *));
                        offset += ((UINT32)p_src_img_cfa_aaa_stat->header.cfa_af.af_tile_num_row+1U) * total_cfa_af_width;
                    }

                    //p_dst_af = &p_dst_img_cfa_aaa_stat->af[0] + offset;
                    //p_src_cfa = p_src_cfa + (sizeof(ik_cfa_ae_t) * p_src_cfa_header->ae_tile_num_row * p_src_cfa_header->ae_tile_num_col);
                    //p_src_af = ( ik_cfa_af_t *)p_src_cfa;

                    misra_ptr = &p_dst_img_cfa_aaa_stat->Af[0];
                    (void)amba_ik_system_memcpy(&p_dst_af, &misra_ptr, sizeof(void*));
                    (void)amba_ik_system_memcpy(&misra_addr, &p_dst_af, sizeof(void*));
                    misra_addr = misra_addr + (offset * sizeof(ik_cfa_af_t));
                    (void)amba_ik_system_memcpy(&p_dst_af, &misra_addr, sizeof(void*));

                    (void)amba_ik_system_memcpy(&misra_addr, &p_src_cfa, sizeof(void*));
                    misra_addr = misra_addr + (sizeof(ik_cfa_ae_t) * ((UINT32)p_src_cfa_header->cfa_ae.ae_tile_num_row+1UL) * ((UINT32)p_src_cfa_header->cfa_ae.ae_tile_num_col+1UL));
                    (void)amba_ik_system_memcpy(&p_src_cfa, &misra_addr, sizeof(void*));
                    (void)amba_ik_system_memcpy(&p_src_af, &p_src_cfa, sizeof(ik_cfa_af_t *));

                    for(k=0; k<((UINT32)p_src_cfa_header->cfa_af.af_tile_num_row+1U); k++) {

                        (void)amba_ik_system_memcpy(p_dst_af, p_src_af, sizeof( ik_cfa_af_t)*((UINT32)p_src_cfa_header->cfa_af.af_tile_num_col+1UL));
                        //p_src_af = p_src_af + p_src_cfa_header->af_tile_num_col;
                        //p_dst_af = p_dst_af + p_dst_cfa_header->af_tile_num_col;
                        (void)amba_ik_system_memcpy(&misra_addr, &p_src_af, sizeof(void*));
                        misra_addr = misra_addr + (((UINT32)p_src_cfa_header->cfa_af.af_tile_num_col+1UL) * sizeof(ik_cfa_af_t));
                        (void)amba_ik_system_memcpy(&p_src_af, &misra_addr, sizeof(void*));

                        (void)amba_ik_system_memcpy(&misra_addr, &p_dst_af, sizeof(void*));
                        misra_addr = misra_addr + (p_dst_cfa_header->CfaAf.AfTileNumCol * sizeof(ik_cfa_af_t));
                        (void)amba_ik_system_memcpy(&p_dst_af, &misra_addr, sizeof(void*));
                    }
                }
                //===== Cfa histogram =====//
                {
                    const ik_cfa_histo_t *p_src_histo;
                    ik_cfa_histo_t *p_dst_histo;
                    const void *misra_ptr;

                    misra_ptr = &p_dst_img_cfa_aaa_stat->Histogram;
                    (void)amba_ik_system_memcpy(&p_dst_histo, &misra_ptr, sizeof(void*));
                    //p_src_cfa = p_src_cfa + ( sizeof( ik_cfa_af_t) * p_src_cfa_header->af_tile_num_row * p_src_cfa_header->af_tile_num_col );
                    //p_src_histo = (ik_cfa_histo_t *)p_src_cfa;

                    (void)amba_ik_system_memcpy(&misra_addr, &p_src_cfa, sizeof(void*));
                    misra_addr = misra_addr + ( sizeof( ik_cfa_af_t) * ((UINT32)p_src_cfa_header->cfa_af.af_tile_num_row+1UL) * ((UINT32)p_src_cfa_header->cfa_af.af_tile_num_col+1UL));
                    (void)amba_ik_system_memcpy(&p_src_cfa, &misra_addr, sizeof(void*));
                    (void)amba_ik_system_memcpy(&p_src_histo, &p_src_cfa, sizeof(ik_cfa_histo_t *));

                    bool_result = equal_op_u32(i, 0U) + equal_op_u32(j, 0U);
                    if(bool_result == 2U) { // (0 , 0)
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
            }
        }

    }
    return rval;
}

static UINT32  img_3a_process_pg_statistic(const UINT8 *p_src_stitch_num_x,
        const UINT8 *p_src_stitch_num_y,
        const ULONG  *src_img_pg_aaa_stat_addr,
        AMBA_IK_PG_3A_DATA_s  *p_dst_img_pg_aaa_stat)
{
    UINT32 rval = IK_OK;
    const UINT8  *p_src_pg;
    UINT32  i,j,k;
    const ik_3a_header_t *p_src_pg_header;
    AMBA_IK_3A_HEADER_s *p_dst_pg_header;
    const ik_pg_3a_data_t  *p_src_img_pg_aaa_stat;
    UINT32 src_idx;
    ULONG misra_addr;
    //UINT32 total_awb_width = 0;
    //UINT32 total_cfa_ae_width = 0;
    //UINT32 total_cfa_af_width = 0;
    UINT32 total_pg_ae_width = 0;
    UINT32 total_pg_af_width = 0;

    //update dist header.
    p_dst_pg_header = &p_dst_img_pg_aaa_stat->Header;

    rval = img_3a_process_header(p_src_stitch_num_x, p_src_stitch_num_y, src_img_pg_aaa_stat_addr, p_dst_pg_header);
    if (rval == IK_OK) {
        (void)amba_ik_system_memcpy(&p_src_img_pg_aaa_stat, &src_img_pg_aaa_stat_addr[0],sizeof(ik_pg_3a_data_t*));
        p_dst_img_pg_aaa_stat ->FrameId = p_src_img_pg_aaa_stat->frame_id;

        //total_awb_width = p_dst_pg_header->Awb.AwbTileNumCol;
        //total_cfa_ae_width = p_dst_pg_header->CfaAe.AeTileNumCol;
        total_pg_ae_width = p_dst_pg_header->PgAe.AeTileNumCol;
        //total_cfa_af_width = p_dst_pg_header->CfaAf.AfTileNumCol;
        total_pg_af_width = p_dst_pg_header->PgAf.AfTileNumCol;

        for(i=0; i< *p_src_stitch_num_y; i++) {
            for(j=0; j< *p_src_stitch_num_x; j++) {

                src_idx = j + (*p_src_stitch_num_x *i);
                //p_src_img_pg_aaa_stat = (ik_pg_3a_data_t  *)src_img_pg_aaa_stat_addr[src_idx];
                (void)amba_ik_system_memcpy(&p_src_img_pg_aaa_stat, &src_img_pg_aaa_stat_addr[src_idx], sizeof(ik_pg_3a_data_t  *));

                //===== pointer init =====//

                (void)amba_ik_system_memcpy(&p_src_pg, &p_src_img_pg_aaa_stat, sizeof(void *));
                (void)amba_ik_system_memcpy(&p_src_pg_header, &p_src_pg, sizeof(ik_3a_header_t *));

                //===== Pg Af =====//
                {
                    const ik_pg_af_t *p_src_af;
                    ik_pg_af_t *p_dst_af;
                    UINT32 offset = 0;
                    UINT32  m;//,n=0;
                    const void *misra_ptr;

                    //    for(m = 0; m < i;m++)
                    //        for(n = 0; n < stitch_tile_x; n++)
                    //        offset += p_dst_cfa_header->awb_tile_num_col * p_src_img_cfa_aaa_stat[m][n]->header.awb_tile_num_col;

                    for(m = 0; m < j; m++) {
                        //p_src_img_pg_aaa_stat = (ik_pg_3a_data_t  *)src_img_pg_aaa_stat_addr[m];
                        (void)amba_ik_system_memcpy(&p_src_img_pg_aaa_stat, &src_img_pg_aaa_stat_addr[m], sizeof(ik_pg_3a_data_t  *));
                        offset += ((UINT32)p_src_img_pg_aaa_stat->header.pg_af.af_tile_num_col+1U);
                    }
                    for(m = 0; m < i; m++) {
                        (void)amba_ik_system_memcpy(&p_src_img_pg_aaa_stat, &src_img_pg_aaa_stat_addr[*p_src_stitch_num_x * m],sizeof(ik_cfa_3a_data_t  *));
                        offset += ((UINT32)p_src_img_pg_aaa_stat->header.pg_af.af_tile_num_row+1U) * total_pg_af_width;
                    }

                    //p_dst_af = &p_dst_img_pg_aaa_stat->af[0] + offset;
                    //p_src_pg = p_src_pg + sizeof(ik_3a_header_t) + sizeof(UINT16);
                    //p_src_af = (ik_pg_af_t *)p_src_pg;

                    misra_ptr = &p_dst_img_pg_aaa_stat->Af[0];
                    (void)amba_ik_system_memcpy(&p_dst_af, &misra_ptr, sizeof(void*));
                    (void)amba_ik_system_memcpy(&misra_addr, &p_dst_af, sizeof(void*));
                    misra_addr = misra_addr + (offset * sizeof(ik_pg_af_t));
                    (void)amba_ik_system_memcpy(&p_dst_af, &misra_addr, sizeof(void*));

                    (void)amba_ik_system_memcpy(&misra_addr, &p_src_pg, sizeof(void*));
                    misra_addr = misra_addr + sizeof(ik_3a_header_t) + sizeof(UINT16);
                    (void)amba_ik_system_memcpy(&p_src_pg, &misra_addr, sizeof(void*));
                    (void)amba_ik_system_memcpy(&p_src_af, &p_src_pg, sizeof(ik_pg_af_t *));


                    for(k=0; k<((UINT32)p_src_pg_header->pg_af.af_tile_num_row+1U); k++) {
                        (void)amba_ik_system_memcpy( p_dst_af, p_src_af, sizeof(ik_pg_af_t)*((UINT32)p_src_pg_header->pg_af.af_tile_num_col+1UL));
                        //p_src_af = p_src_af + p_src_pg_header->af_tile_num_col;
                        //p_dst_af = p_dst_af + p_dst_pg_header->af_tile_num_col;

                        (void)amba_ik_system_memcpy(&misra_addr, &p_src_af, sizeof(void*));
                        misra_addr = misra_addr + (((UINT32)p_src_pg_header->pg_af.af_tile_num_col+1UL) * sizeof(ik_pg_af_t));
                        (void)amba_ik_system_memcpy(&p_src_af, &misra_addr, sizeof(void*));

                        (void)amba_ik_system_memcpy(&misra_addr, &p_dst_af, sizeof(void*));
                        misra_addr = misra_addr + (p_dst_pg_header->PgAf.AfTileNumCol * sizeof(ik_pg_af_t));
                        (void)amba_ik_system_memcpy(&p_dst_af, &misra_addr, sizeof(void*));


                    }
                }
                //===== Pg Ae =====//
                {
                    const ik_pg_ae_t *p_src_ae;
                    ik_pg_ae_t *p_dst_ae;

                    UINT32 offset = 0;
                    UINT32  m;//,n=0;
                    const void *misra_ptr;

                    //    for(m = 0; m < i;m++)
                    //        for(n = 0; n < stitch_tile_x; n++)
                    //        offset += p_dst_cfa_header->awb_tile_num_col * p_src_img_cfa_aaa_stat[m][n]->header.awb_tile_num_col;

                    for(m = 0; m < j; m++) {
                        //p_src_img_pg_aaa_stat = (ik_pg_3a_data_t  *)src_img_pg_aaa_stat_addr[m];
                        (void)amba_ik_system_memcpy(&p_src_img_pg_aaa_stat, &src_img_pg_aaa_stat_addr[m], sizeof(ik_pg_3a_data_t  *));
                        offset += ((UINT32)p_src_img_pg_aaa_stat->header.pg_ae.ae_tile_num_col+1U);
                    }
                    for(m = 0; m < i; m++) {
                        (void)amba_ik_system_memcpy(&p_src_img_pg_aaa_stat, &src_img_pg_aaa_stat_addr[*p_src_stitch_num_x * m],sizeof(ik_cfa_3a_data_t  *));
                        offset += ((UINT32)p_src_img_pg_aaa_stat->header.pg_ae.ae_tile_num_row+1U) * total_pg_ae_width;
                    }

                    //p_dst_ae = &p_dst_img_pg_aaa_stat->ae[0] + offset;
                    //p_src_pg = p_src_pg + (sizeof(ik_pg_af_t) * p_src_pg_header->af_tile_num_row * p_src_pg_header->af_tile_num_col);
                    //p_src_ae = (ik_pg_ae_t *)p_src_pg;

                    misra_ptr = &p_dst_img_pg_aaa_stat->Ae[0];
                    (void)amba_ik_system_memcpy(&p_dst_ae, &misra_ptr, sizeof(void*));
                    (void)amba_ik_system_memcpy(&misra_addr, &p_dst_ae, sizeof(void*));
                    misra_addr = misra_addr + (offset * sizeof(ik_pg_ae_t));
                    (void)amba_ik_system_memcpy(&p_dst_ae, &misra_addr, sizeof(void*));

                    (void)amba_ik_system_memcpy(&misra_addr, &p_src_pg, sizeof(void*));
                    misra_addr = misra_addr + (sizeof(ik_pg_af_t) * ((UINT32)p_src_pg_header->pg_af.af_tile_num_row+1UL) * ((UINT32)p_src_pg_header->pg_af.af_tile_num_col+1UL));
                    (void)amba_ik_system_memcpy(&p_src_pg, &misra_addr, sizeof(void*));
                    (void)amba_ik_system_memcpy(&p_src_ae, &p_src_pg, sizeof(ik_pg_ae_t *));

                    for(k=0; k<((UINT32)p_src_pg_header->pg_ae.ae_tile_num_row+1U); k++) {
                        (void)amba_ik_system_memcpy(p_dst_ae, p_src_ae, sizeof(ik_pg_ae_t)*((UINT32)p_src_pg_header->pg_ae.ae_tile_num_col+1UL));
                        //p_src_ae = p_src_ae + p_src_pg_header->ae_tile_num_col;
                        //p_dst_ae = p_dst_ae + p_dst_pg_header->ae_tile_num_col;

                        (void)amba_ik_system_memcpy(&misra_addr, &p_src_ae, sizeof(void*));
                        misra_addr = misra_addr + (((UINT32)p_src_pg_header->pg_ae.ae_tile_num_col+1UL) * sizeof(ik_pg_ae_t));
                        (void)amba_ik_system_memcpy(&p_src_ae, &misra_addr, sizeof(void*));

                        (void)amba_ik_system_memcpy(&misra_addr, &p_dst_ae, sizeof(void*));
                        misra_addr = misra_addr + (p_dst_pg_header->PgAe.AeTileNumCol * sizeof(ik_pg_ae_t));
                        (void)amba_ik_system_memcpy(&p_dst_ae, &misra_addr, sizeof(void*));
                    }
                }
                //===== histogram =====//
                {
                    const ik_pg_histo_t *p_src_histogram;
                    ik_pg_histo_t *p_dst_histogram;
                    const void *misra_ptr;

                    misra_ptr = &p_dst_img_pg_aaa_stat->Histogram;
                    (void)amba_ik_system_memcpy(&p_dst_histogram, &misra_ptr, sizeof(void*));
                    //p_src_pg = p_src_pg + (sizeof(ik_pg_ae_t) * p_src_pg_header->ae_tile_num_row * p_src_pg_header->ae_tile_num_col);
                    //p_src_histogram = (ik_pg_histo_t *)p_src_pg;

                    (void)amba_ik_system_memcpy(&misra_addr, &p_src_pg, sizeof(void*));
                    misra_addr = misra_addr + (sizeof(ik_pg_ae_t) * ((UINT32)p_src_pg_header->pg_ae.ae_tile_num_row+1UL) * ((UINT32)p_src_pg_header->pg_ae.ae_tile_num_col+1U));
                    (void)amba_ik_system_memcpy(&p_src_pg, &misra_addr, sizeof(void*));
                    (void)amba_ik_system_memcpy(&p_src_histogram, &p_src_pg, sizeof(ik_pg_histo_t *));


                    if((i == 0U)&&(j == 0U)) {
                        (void)amba_ik_system_memcpy(p_dst_histogram,p_src_histogram,sizeof(ik_pg_histo_t));
                    } else {
                        for(k=0; k< 64U; k++) {
                            p_dst_histogram->his_bin_r[k] += p_src_histogram->his_bin_r[k];
                            p_dst_histogram->his_bin_g[k] += p_src_histogram->his_bin_g[k];
                            p_dst_histogram->his_bin_b[k] += p_src_histogram->his_bin_b[k];
                            p_dst_histogram->his_bin_y[k] += p_src_histogram->his_bin_y[k];
                        }
                    }
                }
            }
        }

    }
    return rval;
}


static UINT32  img_3a_process_hist_statistic(const UINT8 *p_type,
        const UINT8 *p_src_stitch_num_x,
        const UINT8 *p_src_stitch_num_y,
        const ULONG  *src_img_hist_aaa_stat_addr,
        AMBA_IK_CFA_HIST_STAT_s  *p_dst_img_hist_aaa_stat)
{
    UINT32 rval = IK_OK;
    UINT32 i,j,k;
    AMBA_IK_3A_HEADER_s *p_dst_vin_stat_header;
    const ik_cfa_histogram_stat_t  *p_src_img_hist_aaa_stat;
    UINT32 src_idx;
    const void *misra_ptr;

    //update dist header.
    p_dst_vin_stat_header = &p_dst_img_hist_aaa_stat->Header;
    rval = img_3a_process_header(p_src_stitch_num_x, p_src_stitch_num_y, src_img_hist_aaa_stat_addr, p_dst_vin_stat_header);
    if (rval == IK_OK) {

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
                    misra_ptr = &p_dst_img_hist_aaa_stat->Data;
                    (void)amba_ik_system_memcpy(&p_dst_histogram,&misra_ptr,sizeof(void*));
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
                                p_dst_histogram->histo_bin_ir[k] = 0U;
                            }
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

    if (In == NULL) {
        AmbaPrint_PrintUInt5("[IK] AmbaIK_TransferCFAAaaStatData NULL input ptr\n",DC_U,DC_U,DC_U,DC_U,DC_U);
        Rval = IK_ERR_0005;
    } else {
        (void)amba_ik_system_memcpy(&pAaaStatData, &In, sizeof(void *));
        if ((pAaaStatData->SrcSliceX * pAaaStatData->SrcSliceY) > AMBA_IK_3A_SLICE_MAX_COUNT) {
            AmbaPrint_PrintUInt5("[IK] AAA transform error, stitching tiles %d x %d > Maximum %d\n",
                                 pAaaStatData->SrcSliceX, pAaaStatData->SrcSliceY,
                                 AMBA_IK_3A_SLICE_MAX_COUNT, DC_U, DC_U);
            Rval = IK_ERR_0103;
        } else {
            for(i=0; i< pAaaStatData->SrcSliceY; i++) {
                for(j=0; j< pAaaStatData->SrcSliceX; j++) {
                    if(pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)] == 0U) {
                        AmbaPrint_PrintUInt5("[IK] AAA transform error, CheckSource, =====pAaaStatData->SrcAddr[%d] ==== 0x%X%X\n", j + (i * pAaaStatData->SrcSliceX), GetU64Msb(pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)]), GetU64Lsb(pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)]), DC_U, DC_U);
                        CheckSource |= 1U;
                        Rval = IK_ERR_0005;
                    }
                    if(pAaaStatData->DstAddr == 0U) {
                        AmbaPrint_PrintUInt5("[IK] AAA transform error, CheckSource, =====pAaaStatData->DstAddr ==== 0x%X%X\n", GetU64Msb(pAaaStatData->DstAddr), GetU64Lsb(pAaaStatData->DstAddr), DC_U, DC_U, DC_U);
                        CheckSource |= 1U;
                        Rval = IK_ERR_0005;
                    }
                }
            }

            if( CheckSource == 0U) {
                AMBA_IK_CFA_3A_DATA_s *p_ik_cfa_3a_data;
                (void)amba_ik_system_memcpy(&p_ik_cfa_3a_data, &pAaaStatData->DstAddr, sizeof(AMBA_IK_CFA_3A_DATA_s *));
                Rval |= img_3a_process_cfa_statistic(&pAaaStatData->SrcSliceX, &pAaaStatData->SrcSliceY, pAaaStatData->SrcAddr, p_ik_cfa_3a_data);

            }
            cfa_count++;
        }
    }

    return Rval;
}

UINT32  AmbaIK_TransferPGAaaStatData(const void *In)
{
    UINT32 Rval = IK_OK;
    const DSP_EVENT_3A_TRANSFER_s *pAaaStatData;
    UINT32  i, j;
    static UINT32 pg_count=0;
    UINT32 CheckSource = 0;

    if (In == NULL) {
        AmbaPrint_PrintUInt5("[IK] AmbaIK_TransferPGAaaStatData NULL input ptr\n",DC_U,DC_U,DC_U,DC_U,DC_U);
        Rval = IK_ERR_0005;
    } else {
        (void)amba_ik_system_memcpy(&pAaaStatData, &In, sizeof(void *));
        if ((pAaaStatData->SrcSliceX * pAaaStatData->SrcSliceY) > AMBA_IK_3A_SLICE_MAX_COUNT) {
            AmbaPrint_PrintUInt5("[IK] AAA transform error, stitching tiles %d x %d > Maximum %d\n",
                                 pAaaStatData->SrcSliceX, pAaaStatData->SrcSliceY,
                                 AMBA_IK_3A_SLICE_MAX_COUNT, DC_U, DC_U);
            Rval = IK_ERR_0103;
        } else {
            for(i=0; i< pAaaStatData->SrcSliceY; i++) {
                for(j=0; j< pAaaStatData->SrcSliceX; j++) {

                    if(pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)] == 0U) {
                        AmbaPrint_PrintUInt5("[IK] AAA transform error, CheckSource, =====pAaaStatData->SrcAddr[%d] ==== 0x%X%X\n", j + (i * pAaaStatData->SrcSliceX), GetU64Msb(pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)]), GetU64Lsb(pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)]), DC_U, DC_U);
                        CheckSource |= 1U;
                        Rval = IK_ERR_0005;
                    }
                    if(pAaaStatData->DstAddr == 0U) {
                        AmbaPrint_PrintUInt5("[IK] AAA transform error, CheckSource, =====pAaaStatData->DstAddr ==== 0x%X%X\n", GetU64Msb(pAaaStatData->DstAddr), GetU64Lsb(pAaaStatData->DstAddr), DC_U, DC_U, DC_U);
                        CheckSource |= 1U;
                        Rval = IK_ERR_0005;
                    }
                }
            }

            if( CheckSource == 0U) {
                AMBA_IK_PG_3A_DATA_s *p_ik_pg_3a_data;
                (void)amba_ik_system_memcpy(&p_ik_pg_3a_data, &pAaaStatData->DstAddr, sizeof(AMBA_IK_PG_3A_DATA_s *));
                Rval |= img_3a_process_pg_statistic(&pAaaStatData->SrcSliceX, &pAaaStatData->SrcSliceY, pAaaStatData->SrcAddr, p_ik_pg_3a_data);
            }
            pg_count++;
        }
    }

    return Rval;

}

UINT32  AmbaIK_TransCFAHistStatData(const void *In)
{
    UINT32 Rval = IK_OK;
    const DSP_EVENT_3A_TRANSFER_s *pHistoStatData;
    UINT32  i, j;
    static UINT32 hdr_count = 0;
    UINT32 CheckSource = 0;

    if (In == NULL) {
        AmbaPrint_PrintUInt5("[IK] AmbaIK_TransCFAHistStatData NULL input ptr\n",DC_U,DC_U,DC_U,DC_U,DC_U);
        Rval = IK_ERR_0005;
    } else {
        (void)amba_ik_system_memcpy(&pHistoStatData, &In, sizeof(void *));
        if ((pHistoStatData->SrcSliceX * pHistoStatData->SrcSliceY) > AMBA_IK_3A_SLICE_MAX_COUNT) {
            AmbaPrint_PrintUInt5("[IK] AAA transform error, stitching tiles %d x %d > Maximum %d\n",
                                 pHistoStatData->SrcSliceX, pHistoStatData->SrcSliceY,
                                 AMBA_IK_3A_SLICE_MAX_COUNT, DC_U, DC_U);
            Rval = IK_ERR_0103;
        } else {
            for(i=0; i< pHistoStatData->SrcSliceY; i++) {
                for(j=0; j< pHistoStatData->SrcSliceX; j++) {

                    if(pHistoStatData->SrcAddr[j + (i * pHistoStatData->SrcSliceX)] == 0U) {
                        AmbaPrint_PrintUInt5("[IK] AAA transform error, CheckSource, =====pHistoStatData->SrcAddr[%d] ==== 0x%X%X\n", j + (i * pHistoStatData->SrcSliceX), GetU64Msb(pHistoStatData->SrcAddr[j + (i * pHistoStatData->SrcSliceX)]), GetU64Lsb(pHistoStatData->SrcAddr[j + (i * pHistoStatData->SrcSliceX)]), DC_U, DC_U);
                        CheckSource |= 1U;
                        Rval = IK_ERR_0005;
                    }
                    if(pHistoStatData->DstAddr == 0U) {
                        AmbaPrint_PrintUInt5("[IK] AAA transform error, CheckSource, =====pHistoStatData->DstAddr ==== 0x%X%X\n", GetU64Msb(pHistoStatData->DstAddr), GetU64Lsb(pHistoStatData->DstAddr), DC_U, DC_U, DC_U);
                        CheckSource |= 1U;
                        Rval = IK_ERR_0005;
                    }
                }
            }

            if( CheckSource == 0U) {
                AMBA_IK_CFA_HIST_STAT_s *p_ik_cfa_histogram_stat;
                (void)amba_ik_system_memcpy(&p_ik_cfa_histogram_stat, &pHistoStatData->DstAddr, sizeof(AMBA_IK_CFA_HIST_STAT_s *));
                Rval |= img_3a_process_hist_statistic(&pHistoStatData->Type, &pHistoStatData->SrcSliceX, &pHistoStatData->SrcSliceY, pHistoStatData->SrcAddr, p_ik_cfa_histogram_stat);
            }

            hdr_count++;
        }
    }

    return Rval;

}

