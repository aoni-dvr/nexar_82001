/*
*  @file AmbaWU_WarpUtility.c
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
* This file includes sample code and is only for internal testing and evaluation.  If you
* distribute this sample code (whether in source, object, or binary code form), it will be
* without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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


#include "AmbaWU_WarpUtility.h"
#include "AmbaWU_SystemApi.h"
#include "AmbaWU_WarpExecuteUtility.h"

#define SFT (16UL)
#define WARP_FRAC_BIT_NUM (4U)

typedef struct {
    // Input warp table info
    AMBA_IK_GRID_POINT_s *pInGridPoint;
    UINT32   HorGridNum;
    UINT32   VerGridNum;
    UINT32   TileWidthExp;
    UINT32   TileHeightExp;
    // Target pixel in s12.4 format
    INT32   X;
    INT32   Y;
    // Ouput vector
    AMBA_IK_GRID_POINT_s *pOutGridPoint;
} WU_GRID_ITPL_INFO_s;

static UINT32 AmbaWU_WarpGridItpl(const WU_GRID_ITPL_INFO_s *pWarpGridInterpolate)
{
    UINT32 xx, yy, hor_grid_num, ver_grid_num, tile_width_exp, tile_height_exp;
    UINT32 x0, y0, horDistance, verDistance;
    AMBA_IK_GRID_POINT_s ul, grid_top, bottom;
    const AMBA_IK_GRID_POINT_s *ulp, *urp, *llp, *lrp;

    hor_grid_num = pWarpGridInterpolate->HorGridNum;
    ver_grid_num = pWarpGridInterpolate->VerGridNum;
    tile_width_exp = pWarpGridInterpolate->TileWidthExp;
    tile_height_exp = pWarpGridInterpolate->TileHeightExp;

    if (pWarpGridInterpolate->X < 0) {
        xx = 0U;
    } else if ((UINT32)pWarpGridInterpolate->X > (UINT32)wu_shift_uint64_left((UINT64)hor_grid_num-1ULL, (UINT64)tile_width_exp+(UINT64)WARP_FRAC_BIT_NUM)) {
        xx = (UINT32)wu_shift_uint64_left((UINT64)hor_grid_num-1ULL, (UINT64)tile_width_exp+(UINT64)WARP_FRAC_BIT_NUM);
    } else {
        xx = (UINT32)pWarpGridInterpolate->X;
    }

    if (pWarpGridInterpolate->Y < 0) {
        yy = 0U;
    } else if ((UINT32)pWarpGridInterpolate->Y > (UINT32)wu_shift_uint64_left((UINT64)ver_grid_num-1ULL, (UINT64)tile_height_exp+(UINT64)WARP_FRAC_BIT_NUM)) {
        yy = (UINT32)wu_shift_uint64_left((UINT64)ver_grid_num-1ULL, (UINT64)tile_height_exp+(UINT64)WARP_FRAC_BIT_NUM);
    } else {
        yy = (UINT32)pWarpGridInterpolate->Y;
    }

    x0 = (UINT32)wu_shift_uint64_right((UINT64)xx, (UINT64)tile_width_exp+(UINT64)WARP_FRAC_BIT_NUM);
    y0 = (UINT32)wu_shift_uint64_right((UINT64)yy, (UINT64)tile_height_exp+(UINT64)WARP_FRAC_BIT_NUM);

    ul.X = (INT16)(INT32)x0;
    ul.Y = (INT16)(INT32)y0;

    horDistance = xx - (UINT32)wu_shift_uint64_left((UINT64)x0, (UINT64)tile_width_exp+(UINT64)WARP_FRAC_BIT_NUM); // s12.4
    verDistance = yy - (UINT32)wu_shift_uint64_left((UINT64)y0, (UINT64)tile_height_exp+(UINT64)WARP_FRAC_BIT_NUM); // s12.4

    ulp = &pWarpGridInterpolate->pInGridPoint[((UINT32)(UINT16)ul.Y*hor_grid_num) + (UINT32)(UINT16)ul.X];
    llp = &ulp[hor_grid_num];
    urp = &ulp[1];
    lrp = &llp[1];

    grid_top.X = (ulp->X) + (INT16)wu_shift_int64_right((INT64)(UINT64)horDistance*((INT64)urp->X-(INT64)ulp->X), (UINT64)tile_width_exp+(UINT64)WARP_FRAC_BIT_NUM);
    bottom.X = (llp->X) + (INT16)wu_shift_int64_right((INT64)(UINT64)horDistance*((INT64)lrp->X-(INT64)llp->X), (UINT64)tile_width_exp+(UINT64)WARP_FRAC_BIT_NUM);
    pWarpGridInterpolate->pOutGridPoint->X = grid_top.X + (INT16)wu_shift_int64_right((INT64)(UINT64)verDistance*((INT64)bottom.X-(INT64)grid_top.X), (UINT64)tile_height_exp+(UINT64)WARP_FRAC_BIT_NUM);
    grid_top.Y = (ulp->Y) + (INT16)wu_shift_int64_right((INT64)(UINT64)horDistance*((INT64)urp->Y-(INT64)ulp->Y), (UINT64)tile_width_exp+(UINT64)WARP_FRAC_BIT_NUM);
    bottom.Y = (llp->Y) + (INT16)wu_shift_int64_right((INT64)(UINT64)horDistance*((INT64)lrp->Y-(INT64)llp->Y), (UINT64)tile_width_exp+(UINT64)WARP_FRAC_BIT_NUM);
    pWarpGridInterpolate->pOutGridPoint->Y = grid_top.Y + (INT16)wu_shift_int64_right((INT64)(UINT64)verDistance*((INT64)bottom.Y-(INT64)grid_top.Y), (UINT64)tile_height_exp+(UINT64)WARP_FRAC_BIT_NUM);

    return 0U;
}
static inline void rotationDelta(INT32 x/*fract bits 4*/, INT32 y, INT32 *xp, INT32 *yp, INT32 costheta/*s16.16*/, INT32 sintheta )
{
    INT64 misra_i64;
    INT32 x1, y1;
    // difference between input and output coordinates for rotation of "theta" radians around origin
    misra_i64 = ((INT64)x*(INT64)costheta) - ((INT64)y*(INT64)sintheta);
    x1 = (INT32)wu_shift_int64_right(misra_i64, (UINT64)SFT);

    misra_i64 = ((INT64)x*(INT64)sintheta) + ((INT64)y*(INT64)costheta);
    y1 = (INT32)wu_shift_int64_right(misra_i64, (UINT64)SFT);

    *xp = x1 - x;
    *yp = y1 - y;
}

static inline UINT8 equal_op_u32(UINT32 compare1, UINT32 compare2)
{
    return (compare1 == compare2) ? 1U : 0U;
}

/**
* Add two input warp tables A and B, and output one warp table.
* @param [in]  pWarpAddIn : input warp table and info of A and B.
* @param [in,out]  pWarpAddResult : output warp table and info of result.
* @return OK - success, NG - fail
*/
UINT32 AmbaWU_WarpAdd(const AMBA_WU_WARP_ADD_IN_s *pWarpAddIn, AMBA_WU_WARP_ADD_RESULT_s *pWarpAddResult)
{
    UINT32 Rval = WU_OK;
    UINT32 x, y;
    INT32 off_x_on_B, off_y_on_B, xx, yy;
    const AMBA_IK_GRID_POINT_s *p_in1_gp;
    AMBA_IK_GRID_POINT_s *p_out_gp;
    WU_GRID_ITPL_INFO_s info;
    const AMBA_IK_WARP_INFO_s *pInputAInfo = &pWarpAddIn->InputAInfo;
    const AMBA_IK_WARP_INFO_s *pInputBInfo = &pWarpAddIn->InputBInfo;
    AMBA_IK_WARP_INFO_s *pResultInfo = &pWarpAddResult->ResultInfo;
    INT32 MisraI32;
    UINT8 invalid_flag;//Check One or more parameter value is invalid //Coverity complexity hates lots of conditions in if
    invalid_flag =  equal_op_u32(pInputAInfo->HorGridNum, 0U) + equal_op_u32(pInputAInfo->VerGridNum, 0U) +
                    equal_op_u32(pInputAInfo->VinSensorGeo.Width, 0U) + equal_op_u32(pInputAInfo->VinSensorGeo.Height, 0U) +
                    equal_op_u32(pInputAInfo->VinSensorGeo.HSubSample.FactorNum, 0U) + equal_op_u32(pInputAInfo->VinSensorGeo.HSubSample.FactorDen, 0U) +
                    equal_op_u32(pInputAInfo->VinSensorGeo.VSubSample.FactorNum, 0U) + equal_op_u32(pInputAInfo->VinSensorGeo.VSubSample.FactorDen, 0U) +
                    equal_op_u32(pInputBInfo->HorGridNum, 0U) + equal_op_u32(pInputBInfo->VerGridNum, 0U) +
                    equal_op_u32(pInputBInfo->VinSensorGeo.Width, 0U) + equal_op_u32(pInputBInfo->VinSensorGeo.Height, 0U) +
                    equal_op_u32(pInputBInfo->VinSensorGeo.HSubSample.FactorNum, 0U) + equal_op_u32(pInputBInfo->VinSensorGeo.HSubSample.FactorDen, 0U) +
                    equal_op_u32(pInputBInfo->VinSensorGeo.VSubSample.FactorNum, 0U) + equal_op_u32(pInputBInfo->VinSensorGeo.VSubSample.FactorDen, 0U);
    /*
            do sanity check
            1.  In1 and In2 have same grid number and tile size. (ver.1)
            2.  In1 and In2 have same vin_sensor_geo. (ver.1)
            3.  Out has same grid and tile size and vin_sensor_geo with In1.
    */

    if ((pWarpAddIn==NULL) || (pWarpAddIn->InputAInfo.pWarp==NULL) || (pWarpAddIn->InputBInfo.pWarp==NULL) || (pWarpAddResult==NULL) || (pWarpAddResult->ResultInfo.pWarp==NULL)) {
        wu_print_uint32_5("[WU][Error] One or more of your input pointer is NULL !!!", DC_U, DC_U, DC_U, DC_U, DC_U);
        wu_print_str_5("Function:%s", __func__, DC_S, DC_S, DC_S, DC_S);
        Rval = WU_ERROR_GENERAL;
    } else if (invalid_flag!=0U) {
        wu_print_uint32_5("[WU][Error] One or more parameter value is invalid !!!", DC_U, DC_U, DC_U, DC_U, DC_U);
        wu_print_str_5("Function:%s", __func__, DC_S, DC_S, DC_S, DC_S);
        wu_print_uint32_5("pInputAInfo:", DC_U, DC_U, DC_U, DC_U, DC_U);
        wu_print_uint32_5("  HorGridNum:%d, VerGridNum:%d, VinSensorGeo.Width:%d, VinSensorGeo.Height:%d", pInputAInfo->HorGridNum, pInputAInfo->VerGridNum, pInputAInfo->VinSensorGeo.Width, pInputAInfo->VinSensorGeo.Height, DC_U);
        wu_print_uint32_5("  VinSensorGeo.HSubSample.FactorNum:%d, VinSensorGeo.HSubSample.FactorDen:%d, VinSensorGeo.VSubSample.FactorNum:%d, VinSensorGeo.VSubSample.FactorDen:%d", pInputAInfo->VinSensorGeo.HSubSample.FactorNum, pInputAInfo->VinSensorGeo.HSubSample.FactorDen, pInputAInfo->VinSensorGeo.VSubSample.FactorNum, pInputAInfo->VinSensorGeo.VSubSample.FactorDen, DC_U);
        wu_print_uint32_5("pInputBInfo:", DC_U, DC_U, DC_U, DC_U, DC_U);
        wu_print_uint32_5("  HorGridNum:%d, VerGridNum:%d, VinSensorGeo.Width:%d, VinSensorGeo.Height:%d", pInputBInfo->HorGridNum, pInputBInfo->VerGridNum, pInputBInfo->VinSensorGeo.Width, pInputBInfo->VinSensorGeo.Height, DC_U);
        wu_print_uint32_5("  VinSensorGeo.HSubSample.FactorNum:%d, VinSensorGeo.HSubSample.FactorDen:%d, VinSensorGeo.VSubSample.FactorNum:%d, VinSensorGeo.VSubSample.FactorDen:%d", pInputBInfo->VinSensorGeo.HSubSample.FactorNum, pInputBInfo->VinSensorGeo.HSubSample.FactorDen, pInputBInfo->VinSensorGeo.VSubSample.FactorNum, pInputBInfo->VinSensorGeo.VSubSample.FactorDen, DC_U);
        Rval = WU_ERROR_GENERAL;
    } else {
        pResultInfo->HorGridNum = pInputAInfo->HorGridNum;
        pResultInfo->VerGridNum = pInputAInfo->VerGridNum;
        pResultInfo->TileWidthExp = pInputAInfo->TileWidthExp;
        pResultInfo->TileHeightExp = pInputAInfo->TileHeightExp;
        pResultInfo->VinSensorGeo = pInputAInfo->VinSensorGeo;

        off_x_on_B = (INT32)wu_shift_int64_left(((INT64)(UINT64)pInputAInfo->VinSensorGeo.StartX-(INT64)(UINT64)pInputBInfo->VinSensorGeo.StartX)*(INT64)(UINT64)pInputBInfo->VinSensorGeo.HSubSample.FactorNum, (UINT64)WARP_FRAC_BIT_NUM)/(INT32)pInputBInfo->VinSensorGeo.HSubSample.FactorDen; // .4 format
        off_y_on_B = (INT32)wu_shift_int64_left(((INT64)(UINT64)pInputAInfo->VinSensorGeo.StartY-(INT64)(UINT64)pInputBInfo->VinSensorGeo.StartY)*(INT64)(UINT64)pInputBInfo->VinSensorGeo.VSubSample.FactorNum, (UINT64)WARP_FRAC_BIT_NUM)/(INT32)pInputBInfo->VinSensorGeo.VSubSample.FactorDen; // .4 format

        for (y = 0; y < pResultInfo->VerGridNum; y++) {
            for (x = 0; x < pResultInfo->HorGridNum; x++) {
                p_in1_gp = &pInputAInfo->pWarp[(pInputAInfo->HorGridNum*y) + x];
                //step 1
                xx = off_x_on_B + ((((INT32)p_in1_gp->X + (INT32)(UINT32)wu_shift_uint64_left(wu_shift_uint64_left((UINT64)x, (UINT64)pInputAInfo->TileWidthExp), (UINT64)WARP_FRAC_BIT_NUM))*(INT32)pInputAInfo->VinSensorGeo.HSubSample.FactorDen*(INT32)pInputBInfo->VinSensorGeo.HSubSample.FactorNum)/((INT32)pInputAInfo->VinSensorGeo.HSubSample.FactorNum*(INT32)pInputBInfo->VinSensorGeo.HSubSample.FactorDen)); // s12.4
                yy = off_y_on_B + ((((INT32)p_in1_gp->Y + (INT32)(UINT32)wu_shift_uint64_left(wu_shift_uint64_left((UINT64)y, (UINT64)pInputAInfo->TileHeightExp), (UINT64)WARP_FRAC_BIT_NUM))*(INT32)pInputAInfo->VinSensorGeo.VSubSample.FactorDen*(INT32)pInputBInfo->VinSensorGeo.VSubSample.FactorNum)/((INT32)pInputAInfo->VinSensorGeo.VSubSample.FactorNum*(INT32)pInputBInfo->VinSensorGeo.VSubSample.FactorDen)); // s12.4

                p_out_gp = &pResultInfo->pWarp[(pResultInfo->HorGridNum*y) + x];

                info.pInGridPoint = pInputBInfo->pWarp;
                info.HorGridNum = pInputBInfo->HorGridNum;
                info.VerGridNum = pInputBInfo->VerGridNum;
                info.TileWidthExp = pInputBInfo->TileWidthExp;
                info.TileHeightExp = pInputBInfo->TileHeightExp;
                info.X = xx;
                info.Y = yy;
                info.pOutGridPoint = p_out_gp;
                //step 2
                (void)AmbaWU_WarpGridItpl(&info);

                //step 3
                if ((pInputAInfo->VinSensorGeo.HSubSample.FactorNum != pInputBInfo->VinSensorGeo.HSubSample.FactorNum) ||
                    (pInputAInfo->VinSensorGeo.HSubSample.FactorDen != pInputBInfo->VinSensorGeo.HSubSample.FactorDen)) {
                    MisraI32 = ((INT32)p_out_gp->X*(INT32)pInputBInfo->VinSensorGeo.HSubSample.FactorDen*(INT32)pInputAInfo->VinSensorGeo.HSubSample.FactorNum) / ((INT32)pInputBInfo->VinSensorGeo.HSubSample.FactorNum*(INT32)pInputAInfo->VinSensorGeo.HSubSample.FactorDen);
                    p_out_gp->X = (INT16)MisraI32;
                }
                if ((pInputAInfo->VinSensorGeo.VSubSample.FactorNum != pInputBInfo->VinSensorGeo.VSubSample.FactorNum) ||
                    (pInputAInfo->VinSensorGeo.VSubSample.FactorDen != pInputBInfo->VinSensorGeo.VSubSample.FactorDen)) {
                    MisraI32 = ((INT32)p_out_gp->Y*(INT32)pInputBInfo->VinSensorGeo.VSubSample.FactorDen*(INT32)pInputAInfo->VinSensorGeo.VSubSample.FactorNum) / ((INT32)pInputBInfo->VinSensorGeo.VSubSample.FactorNum*(INT32)pInputAInfo->VinSensorGeo.VSubSample.FactorDen);
                    p_out_gp->Y = (INT16)MisraI32;
                }

                p_out_gp->X += p_in1_gp->X;
                p_out_gp->Y += p_in1_gp->Y;
            }
        }
    }
    return Rval;
}

/**
* Resize input warp table, and output one warp table.
* @param [in]  pWarpResizeIn : input warp table and info.
* @param [in,out]  pWarpResizeResult : output warp table and info of result.
* @return OK - success, NG - fail
*/
UINT32 AmbaWU_WarpResize(const AMBA_WU_WARP_RESIZE_IN_s *pWarpResizeIn, const AMBA_WU_WARP_RESIZE_RESULT_s *pWarpResizeResult)
{
    UINT32 Rval = WU_OK;
    UINT32 HorPhaseInc, VerPhaseInc; // .16
    UINT32 OffsetX, OffsetY; // .4
    UINT32 x, y;
    AMBA_IK_GRID_POINT_s *p_out_gp;
    WU_GRID_ITPL_INFO_s info;
    UINT32 CropX;
    UINT32 CropY;
    AMBA_IK_WINDOW_GEOMETRY_s FovInfo;
    const AMBA_IK_WARP_INFO_s *pInputInfo;
    const AMBA_IK_WARP_INFO_s *pResultInfo;
    INT64 MisraI64;

    if ((pWarpResizeIn==NULL) || (pWarpResizeIn->InputInfo.pWarp==NULL) || (pWarpResizeResult==NULL) || (pWarpResizeResult->ResultInfo.pWarp==NULL)) {
        wu_print_uint32_5("[WU][Error] One or more of your input pointer is NULL !!!", DC_U, DC_U, DC_U, DC_U, DC_U);
        wu_print_str_5("Function:%s", __func__, DC_S, DC_S, DC_S, DC_S);
        Rval = WU_ERROR_GENERAL;
    } else {
        CropX = pWarpResizeIn->CropX;
        CropY = pWarpResizeIn->CropY;
        pInputInfo = &pWarpResizeIn->InputInfo;
        pResultInfo = &pWarpResizeResult->ResultInfo;
        if ((pInputInfo->HorGridNum==0UL) || (pInputInfo->VerGridNum==0UL) ||
            (pInputInfo->VinSensorGeo.Width==0UL) || (pInputInfo->VinSensorGeo.Height==0UL) ||
            (pInputInfo->VinSensorGeo.HSubSample.FactorNum==0UL) || (pInputInfo->VinSensorGeo.HSubSample.FactorDen==0UL) ||
            (pInputInfo->VinSensorGeo.VSubSample.FactorNum==0UL) || (pInputInfo->VinSensorGeo.VSubSample.FactorDen==0UL) ||
            (pResultInfo->HorGridNum==0UL) || (pResultInfo->VerGridNum==0UL)) {
            wu_print_uint32_5("[WU][Error] One or more parameter value is invalid !!!", DC_U, DC_U, DC_U, DC_U, DC_U);
            wu_print_str_5("Function:%s", __func__, DC_S, DC_S, DC_S, DC_S);
            wu_print_uint32_5("pInputInfo:", DC_U, DC_U, DC_U, DC_U, DC_U);
            wu_print_uint32_5("  HorGridNum:%d, VerGridNum:%d, VinSensorGeo.Width:%d, VinSensorGeo.Height:%d", pInputInfo->HorGridNum, pInputInfo->VerGridNum, pInputInfo->VinSensorGeo.Width, pInputInfo->VinSensorGeo.Height, DC_U);
            wu_print_uint32_5("  VinSensorGeo.HSubSample.FactorNum:%d, VinSensorGeo.HSubSample.FactorDen:%d, VinSensorGeo.VSubSample.FactorNum:%d, VinSensorGeo.VSubSample.FactorDen:%d", pInputInfo->VinSensorGeo.HSubSample.FactorNum, pInputInfo->VinSensorGeo.HSubSample.FactorDen, pInputInfo->VinSensorGeo.VSubSample.FactorNum, pInputInfo->VinSensorGeo.VSubSample.FactorDen, DC_U);
            wu_print_uint32_5("pResultInfo:", DC_U, DC_U, DC_U, DC_U, DC_U);
            wu_print_uint32_5("  HorGridNum:%d, VerGridNum:%d", pResultInfo->HorGridNum, pResultInfo->VerGridNum, DC_U, DC_U, DC_U);
            Rval = WU_ERROR_GENERAL;
        } else {
            if((pWarpResizeIn->FovInfo.Width==0UL) || (pWarpResizeIn->FovInfo.Height==0UL)) {
                FovInfo.StartX = pResultInfo->VinSensorGeo.StartX;
                FovInfo.StartY = pResultInfo->VinSensorGeo.StartY;
                FovInfo.Width  = pResultInfo->VinSensorGeo.Width;
                FovInfo.Height = pResultInfo->VinSensorGeo.Height;
            } else {
                FovInfo.StartX = pWarpResizeIn->FovInfo.StartX;
                FovInfo.StartY = pWarpResizeIn->FovInfo.StartY;
                FovInfo.Width  = pWarpResizeIn->FovInfo.Width;
                FovInfo.Height = pWarpResizeIn->FovInfo.Height;
            }
            HorPhaseInc = (UINT32)wu_shift_uint64_left((UINT64)pInputInfo->VinSensorGeo.Width, (UINT64)SFT) / FovInfo.Width; // .16
            VerPhaseInc = (UINT32)wu_shift_uint64_left((UINT64)pInputInfo->VinSensorGeo.Height, (UINT64)SFT) / FovInfo.Height; // .16
            OffsetX = (UINT32)wu_shift_uint64_right((UINT64)FovInfo.StartX*(UINT64)HorPhaseInc, (UINT64)SFT-(UINT64)WARP_FRAC_BIT_NUM); // .4
            OffsetY = (UINT32)wu_shift_uint64_right((UINT64)FovInfo.StartY*(UINT64)VerPhaseInc, (UINT64)SFT-(UINT64)WARP_FRAC_BIT_NUM); // .4

            for(y = 0; y < pResultInfo->VerGridNum; y++) {
                for(x = 0; x < pResultInfo->HorGridNum; x++) {
                    p_out_gp = &pResultInfo->pWarp[(pResultInfo->HorGridNum*y) + x];
                    info.pInGridPoint = pInputInfo->pWarp;
                    info.HorGridNum = pInputInfo->HorGridNum;
                    info.VerGridNum = pInputInfo->VerGridNum;
                    info.TileWidthExp = pInputInfo->TileWidthExp;
                    info.TileHeightExp = pInputInfo->TileHeightExp;
                    info.X = (INT32)(UINT32)wu_shift_uint64_right(((UINT64)x<<(UINT64)pResultInfo->TileWidthExp)*(UINT64)HorPhaseInc, (UINT64)SFT-(UINT64)WARP_FRAC_BIT_NUM) - (INT32)OffsetX; // .4
                    info.Y = (INT32)(UINT32)wu_shift_uint64_right(((UINT64)y<<(UINT64)pResultInfo->TileHeightExp)*(UINT64)VerPhaseInc, (UINT64)SFT-(UINT64)WARP_FRAC_BIT_NUM) - (INT32)OffsetY; // .4
                    info.pOutGridPoint = p_out_gp;
                    info.X += (INT32)(UINT32)wu_shift_uint64_left((UINT64)CropX, (UINT64)WARP_FRAC_BIT_NUM);
                    info.Y += (INT32)(UINT32)wu_shift_uint64_left((UINT64)CropY, (UINT64)WARP_FRAC_BIT_NUM);
                    (void)AmbaWU_WarpGridItpl(&info);

                    MisraI64 = wu_shift_int64_left((INT64)p_out_gp->X, (UINT64)SFT) / (INT64)(UINT64)HorPhaseInc;
                    p_out_gp->X = (INT16)MisraI64;
                    MisraI64 = wu_shift_int64_left((INT64)p_out_gp->Y, (UINT64)SFT) / (INT64)(UINT64)VerPhaseInc;
                    p_out_gp->Y = (INT16)MisraI64;
                }
            }
        }
    }
    return Rval;
}

/**
* Rotate input warp table, and output one warp table.
* @param [in]  pWarpRotateIn : input warp table and info.
* @param [in,out]  pWarpRotateResult : output warp table and info of result.
* @return OK - success, NG - fail
*/
UINT32 AmbaWU_WarpRotate(const AMBA_WU_WARP_ROTATE_IN_s *pWarpRotateIn, const AMBA_WU_WARP_ROTATE_RESULT_s *pWarpRotateResult)
{
    UINT32 Rval = WU_OK;
    INT32 CosTheta, SinTheta;

    const AMBA_IK_GRID_POINT_s *pGP = NULL;
    AMBA_IK_GRID_POINT_s *pOutput = NULL;
    INT32 x0,y0,x,y,dx,dy;
    UINT32 horGrid, vertGrid;
    INT32 Minus_CenterX, Minus_CenterY;
    UINT32 hor_calib2sensor_factor, hor_sensor2calib_factor, ver_calib2sensor_factor, ver_sensor2calib_factor;
    UINT32 i, j;
    DOUBLE Theta;
    UINT32 CenterX;
    UINT32 CenterY;
    const AMBA_IK_WARP_INFO_s *pInputInfo;
    UINT32 HGN;
    UINT32 VGN;
    UINT32 TWE;
    UINT32 THE;
    const AMBA_IK_WARP_INFO_s *pResultInfo;
    DOUBLE misra_d64;

    if ((pWarpRotateIn==NULL) || (pWarpRotateIn->InputInfo.pWarp==NULL) || (pWarpRotateResult==NULL) || (pWarpRotateResult->ResultInfo.pWarp==NULL)) {
        wu_print_uint32_5("[WU][Error] One or more of your input pointer is NULL !!!", DC_U, DC_U, DC_U, DC_U, DC_U);
        wu_print_str_5("Function:%s", __func__, DC_S, DC_S, DC_S, DC_S);
        Rval = WU_ERROR_GENERAL;
    } else {
        pInputInfo = &pWarpRotateIn->InputInfo;
        if ((pInputInfo->HorGridNum==0UL) || (pInputInfo->VerGridNum==0UL) ||
            (pInputInfo->VinSensorGeo.HSubSample.FactorNum==0UL) || (pInputInfo->VinSensorGeo.HSubSample.FactorDen==0UL) ||
            (pInputInfo->VinSensorGeo.VSubSample.FactorNum==0UL) || (pInputInfo->VinSensorGeo.VSubSample.FactorDen==0UL)) {
            wu_print_uint32_5("[WU][Error] One or more parameter value is invalid !!!", DC_U, DC_U, DC_U, DC_U, DC_U);
            wu_print_str_5("Function:%s", __func__, DC_S, DC_S, DC_S, DC_S);
            wu_print_uint32_5("pInputInfo:", DC_U, DC_U, DC_U, DC_U, DC_U);
            wu_print_uint32_5("  HorGridNum:%d, VerGridNum:%d", pInputInfo->HorGridNum, pInputInfo->VerGridNum, DC_U, DC_U, DC_U);
            wu_print_uint32_5("  VinSensorGeo.HSubSample.FactorNum:%d, VinSensorGeo.HSubSample.FactorDen:%d, VinSensorGeo.VSubSample.FactorNum:%d, VinSensorGeo.VSubSample.FactorDen:%d", pInputInfo->VinSensorGeo.HSubSample.FactorNum, pInputInfo->VinSensorGeo.HSubSample.FactorDen, pInputInfo->VinSensorGeo.VSubSample.FactorNum, pInputInfo->VinSensorGeo.VSubSample.FactorDen, DC_U);
            Rval = WU_ERROR_GENERAL;
        } else {
            Theta = pWarpRotateIn->Theta;
            CenterX = pWarpRotateIn->CenterX;
            CenterY = pWarpRotateIn->CenterY;
            HGN = pInputInfo->HorGridNum;
            VGN = pInputInfo->VerGridNum;
            TWE = pInputInfo->TileWidthExp;
            THE = pInputInfo->TileHeightExp;
            pResultInfo = &pWarpRotateResult->ResultInfo;

            Theta = 0.0 - Theta;

            hor_calib2sensor_factor = (UINT32)wu_shift_uint64_left((UINT64)pInputInfo->VinSensorGeo.HSubSample.FactorDen, 13ULL) / pInputInfo->VinSensorGeo.HSubSample.FactorNum;
            hor_sensor2calib_factor = (UINT32)wu_shift_uint64_left((UINT64)pInputInfo->VinSensorGeo.HSubSample.FactorNum, 13ULL) / pInputInfo->VinSensorGeo.HSubSample.FactorDen;
            ver_calib2sensor_factor = (UINT32)wu_shift_uint64_left((UINT64)pInputInfo->VinSensorGeo.VSubSample.FactorDen, 13ULL) / pInputInfo->VinSensorGeo.VSubSample.FactorNum;
            ver_sensor2calib_factor = (UINT32)wu_shift_uint64_left((UINT64)pInputInfo->VinSensorGeo.VSubSample.FactorNum, 13ULL) / pInputInfo->VinSensorGeo.VSubSample.FactorDen;

            misra_d64 = wu_cos(Theta)*65536.0;
            CosTheta = (INT32)misra_d64; // s15.16
            misra_d64 = wu_sin(Theta)*65536.0;
            SinTheta = (INT32)misra_d64; // s15.16

            horGrid = TWE + WARP_FRAC_BIT_NUM;
            vertGrid =  THE + WARP_FRAC_BIT_NUM;
            Minus_CenterX = (INT32)wu_shift_int64_left(0-(INT64)(INT32)CenterX, (UINT64)WARP_FRAC_BIT_NUM);
            Minus_CenterY = (INT32)wu_shift_int64_left(0-(INT64)(INT32)CenterY, (UINT64)WARP_FRAC_BIT_NUM);

            //wu_print_uint32_5("############ CosTheta :%d, SinTheta:%d", CosTheta, SinTheta, DC_I, DC_I, DC_I);
            //wu_print_uint32_5("########## Minus_CenterX:%d, Minus_CenterY:%d, WARP_FRAC_BIT_NUM:%d !!!!", Minus_CenterX, Minus_CenterY, WARP_FRAC_BIT_NUM, DC_I, DC_I);

            // fill in the intermediate points using bilinear interpolation
            for (i=0; i<VGN; i++) {
                for (j=0; j<HGN; j++) {
                    // grid point in image coordinates
                    x0 = (INT32)(UINT32)wu_shift_uint64_left((UINT64)j, (UINT64)horGrid) + Minus_CenterX;
                    y0 = (INT32)(UINT32)wu_shift_uint64_left((UINT64)i, (UINT64)vertGrid) + Minus_CenterY;

                    // mapped points in image coordinates
                    pGP = &pInputInfo->pWarp[(i*HGN) + j];
                    pOutput = &pResultInfo->pWarp[(i*HGN) + j];
                    x = x0 + (INT32)pGP->X;
                    y = y0 + (INT32)pGP->Y;

                    //assume the x,y value won't > 16383(2^14) , so reserve 13bit for binning factor fractional number, bit distribute :fractional (13 + 4) + integer(14) + sign(1) .
                    x = (INT32)wu_shift_int64_right((INT64)x*(INT64)(UINT64)hor_calib2sensor_factor, 13ULL);
                    y = (INT32)wu_shift_int64_right((INT64)y*(INT64)(UINT64)ver_calib2sensor_factor, 13ULL);

                    rotationDelta(x, y, &dx, &dy, CosTheta, SinTheta);

                    dx = (INT32)wu_shift_int64_right((INT64)dx*(INT64)(UINT64)hor_sensor2calib_factor, 13ULL);
                    dy = (INT32)wu_shift_int64_right((INT64)dy*(INT64)(UINT64)ver_sensor2calib_factor, 13ULL);

                    pOutput->X = pGP->X + (INT16)dx;
                    pOutput->Y = pGP->Y + (INT16)dy;
                }
            }
        }
    }
    return Rval;
}

