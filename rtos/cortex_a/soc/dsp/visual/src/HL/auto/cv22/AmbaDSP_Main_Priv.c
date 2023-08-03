/**
 *  @file AmbaDSP_Main_Priv.c
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
 *  @details Implementation of SSP general API
 *
 */

#include "AmbaDSP_Log.h"
#include "AmbaDSP_Int.h"
#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_ApiDump.h"
#include "AmbaDSP_ApiCheck.h"
#include "AmbaDSP_EncodeAPI.h"
#include "AmbaDSP_ContextUtility.h"
#include "dsp_priv_api.h"

//#define SUPPORT_BURST_TILE
#define HIER_ROI_EXTRA_ROW      (16U)
#define HIER_ROI_EXTRA_COL      (16U)
static inline UINT32 HEIR_2X_DOWNSCALE(UINT32 x)
{
    return (((((x) >> 1U) + 1U) >> 1U) << 1U);
}

#ifdef SUPPORT_BURST_TILE
static inline UINT32 HEIR_IMG_SIZE(UINT32 FullSize, UINT32 Idx)
{
    return (((FullSize + ((1U<<Idx)-1U)) >> Idx) << 1U);
}

static inline UINT32 GET_TILE_PITCH(UINT32 Width)
{
    UINT32 Pitch;

    if (512U >= Width) {
        Pitch = 512U;
    } else if (1024U >= Width) {
        Pitch = 1024U;
    } else if (2048U >= Width) {
        Pitch = 2048U;
    } else if (4096U >= Width) {
        Pitch = 4096U;
    } else if (8192U >= Width) {
        Pitch = 8192U;
    } else if (16384U >= Width) {
        Pitch = 16384U;
    } else {
        Pitch = 8192U;
    }

    return Pitch;
}

static inline UINT32 GET_BASE_ALIGNMENT(UINT32 Pitch)
{
    UINT32 PageSize;
    UINT32 Alignment;

    PageSize = DSP_GetPageSize();

    if (PageSize == 1024U) {
        Alignment = ((2U*32U)*Pitch);
    } else if ((PageSize == 2048U) || (PageSize == 4096U)) {
        Alignment = ((2U*64U)*Pitch);
    } else {
        Alignment = ((2U*128U)*Pitch);
    }
    return Alignment;
}

static inline UINT16 QuiteHierTiledHeight(const UINT16 *pMaxMainWidth, const UINT16 *pMaxMainHeight)
{
    UINT32 i;
    UINT32 TileRowsHeight, BufHeightAdj, BufPitch, BufSize;
    UINT32 PageSize;

    PageSize = DSP_GetPageSize();

    if (PageSize == 1024U) {
        TileRowsHeight = 32U*2U;
    } else if ((PageSize == 2048U) || (PageSize == 4096U)) {
        TileRowsHeight = 64U*2U;
    } else {
        TileRowsHeight = 128U*2U;
    }

    for (i = 0U; i<AMBA_DSP_MAX_HIER_NUM; i+=2U) {
        BufPitch = GET_TILE_PITCH(ALIGN_NUM(HEIR_IMG_SIZE((UINT32)(*pMaxMainWidth), ((i>>1U)+1U)) + HIER_ROI_EXTRA_COL, 8U));
        BufHeightAdj = ALIGN_NUM(HEIR_IMG_SIZE((UINT32)(*pMaxMainHeight), ((i>>1U)+1U)) + HIER_ROI_EXTRA_ROW, 16U) + 16U;
        BufHeightAdj = ((BufHeightAdj + (TileRowsHeight - 1U))/TileRowsHeight)*TileRowsHeight;
        BufSize += (BufPitch * BufHeightAdj);
    }

    BufPitch = GET_TILE_PITCH(ALIGN_NUM(HEIR_IMG_SIZE((UINT32)(*pMaxMainWidth), 1U) + HIER_ROI_EXTRA_COL, 8U));
    BufSize += GET_BASE_ALIGNMENT(BufPitch);

    for (i = 1U; i<AMBA_DSP_MAX_HIER_NUM; i+=2U) {
        BufPitch = GET_TILE_PITCH(ALIGN_NUM(HEIR_IMG_SIZE((UINT32)(*pMaxMainWidth), (((i - 1U)>>1U)+1U)) + HIER_ROI_EXTRA_COL, 8U));
        BufHeightAdj = ALIGN_NUM(HEIR_IMG_SIZE((UINT32)(*pMaxMainHeight), (((i - 1U)>>1U)+1U)) + HIER_ROI_EXTRA_ROW, 16U) + 16U;
        BufHeightAdj = ((BufHeightAdj + (TileRowsHeight - 1U))/TileRowsHeight)*TileRowsHeight;
        BufSize += (BufPitch * BufHeightAdj);
    }

    BufPitch = ALIGN_NUM(GET_TILE_PITCH(HEIR_IMG_SIZE((UINT32)(*pMaxMainWidth), 1U) + HIER_ROI_EXTRA_COL), 8U);
    BufHeightAdj = ((BufSize + (BufPitch - 1U))/BufPitch); //based on minimum tile pitch
    BufHeightAdj = ((BufHeightAdj + (TileRowsHeight - 1U))/TileRowsHeight)*TileRowsHeight;

    return BufHeightAdj;
}
#endif

/**
* HierBuffer query API
* @param [in]  pMaxMainWidth Max hier[0] width
* @param [in]  pMaxMainHeight Max hier[0] height
* @param [in]  pMaxHierWidth Max hier[1] width
* @param [in]  pMaxHierHeight Max hier[1] height
* @param [in]  OctaveMode, B[0:2] octave setting, B[15:8] hier disable bit
* @param [in/out]  pHierBufSize buffer size
* @param [in/out]  pHierBufWidth buffer width
* @param [in/out]  pHierBufHeight buffer height
* @return ErrorCode
*/
UINT32 dsp_cal_hier_buffer_size(const UINT16 *pMaxMainWidth, const UINT16 *pMaxMainHeight,
                                const UINT16 *pMaxHierWidth, const UINT16 *pMaxHierHeight,
                                const UINT16 OctaveMode,
                                UINT32 *pHierBufSize, UINT16 *pHierBufWidth, UINT16 *pHierBufHeight)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U, i, TempSize;
    UINT16 BufWidth, BufHeight;
    UINT16 HeirDisableMask;
    UINT16 MaxWidth = 0U;
    UINT16 OctMode = 0U;

    if ((pMaxMainWidth == NULL) || (pMaxMainHeight == NULL) ||
        (pHierBufSize == NULL) ||
        (pHierBufWidth == NULL) || (pHierBufHeight == NULL)) {
        Rval = DSP_ERR_0000;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    } else {
        //reset
        *pHierBufSize = 0U;
        *pHierBufWidth = 0U;
        *pHierBufHeight = 0U;

        HeirDisableMask = (UINT16)DSP_GetU16Bit(OctaveMode, 8U, 8U);
        OctMode = (UINT16)DSP_GetU16Bit(OctaveMode, 0U, 3U);

#ifdef SUPPORT_BURST_TILE
        if (1U/* HierBurstTile */) {
            BufWidth = *pMaxMainWidth + HIER_ROI_EXTRA_COL;
            BufHeight = QuiteHierTiledHeight(pMaxMainWidth, pMaxMainHeight);
        } else
#endif
        {
            BufWidth = *pMaxMainWidth;
            BufHeight = ALIGN_NUM16(*pMaxMainHeight, 16U);
            for (i=0U; i<AMBA_DSP_MAX_HIER_NUM; i+=2U) {
                BufWidth = BufWidth + HIER_ROI_EXTRA_COL;
                BufHeight = BufHeight + HIER_ROI_EXTRA_ROW;

                if (0U == DSP_GetU16Bit(HeirDisableMask, i, 1U)) {
                    MaxWidth = (MaxWidth < BufWidth)? BufWidth: MaxWidth;
                    *pHierBufSize += ALIGN_NUM((UINT32)BufWidth, DSP_BUF_ALIGNMENT) * BufHeight;
                }
                BufWidth = (UINT16)HEIR_2X_DOWNSCALE(BufWidth);
                BufHeight = (UINT16)HEIR_2X_DOWNSCALE(BufHeight);
            }

            if (OctMode == DSP_HIER_HALF_OCTAVE_MODE) {
                BufWidth = *pMaxHierWidth;
                BufHeight = *pMaxHierHeight;
            } else {
                /*
                 * 20180831, ChenHan's formula reserved for worst case(larger than exactly size)
                 */
                BufWidth = (*pMaxMainWidth)/2U;
                BufHeight = (ALIGN_NUM16(*pMaxMainHeight, 16U))/2U;
            }
            for (i=1U; i<AMBA_DSP_MAX_HIER_NUM; i+=2U) {
                BufWidth = BufWidth + HIER_ROI_EXTRA_COL;
                BufHeight = BufHeight + HIER_ROI_EXTRA_ROW;
                if (0U == DSP_GetU16Bit(HeirDisableMask, i, 1U)) {
                    MaxWidth = (MaxWidth < BufWidth)? BufWidth: MaxWidth;
                    *pHierBufSize += ALIGN_NUM((UINT32)BufWidth, DSP_BUF_ALIGNMENT)* BufHeight;
                }
                BufWidth = (UINT16)HEIR_2X_DOWNSCALE(BufWidth);
                BufHeight = (UINT16)HEIR_2X_DOWNSCALE(BufHeight);
            }

            *pHierBufWidth = ALIGN_NUM16(MaxWidth, DSP_BUF_ALIGNMENT);
            BufWidth = *pHierBufWidth - 1U;
            TempSize = *pHierBufSize + BufWidth;
            *pHierBufHeight = (UINT16)(TempSize/(*pHierBufWidth));
            *pHierBufHeight = ALIGN_NUM16(*pHierBufHeight, 2U);
            *pHierBufSize = (UINT32)(*pHierBufWidth)*(UINT32)(*pHierBufHeight);

//            AmbaLL_LogUInt5("Calc Buf %d %dx%d, Max %dx%d", *pHierBufSize, *pHierBufWidth, *pHierBufHeight, *pMaxMainWidth, *pMaxMainHeight);
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_cal_hier_buffer_size Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 dsp_calc_vp_msg_buf_size(const UINT32 *NumMsgs, UINT32 *MsgSize)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    if ((NumMsgs == NULL) || (MsgSize == NULL)) {
        Rval = DSP_ERR_0000;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    } else {
        *MsgSize = (*NumMsgs)*ALIGN_NUM(AMBA_DSP_HIER_UNIT_SIZE, 128U);
    }
    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_calc_vp_msg_buf_size Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

