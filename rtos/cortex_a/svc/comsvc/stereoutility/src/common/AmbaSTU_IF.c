/**
 *  @file AmbaSTU_IF.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details stereo utility
 *
 */
#include "AmbaSTU_IF.h"
#include "AmbaSTU_PGMPacker.h"
#include "AmbaSTU_WarpTableMetadata.h"
#include "AmbaWrap.h"

#define MAX_DISPARITY_SHIFT_VALUE (6U)


typedef struct {
    UINT8 R;
    UINT8 G;
    UINT8 B;
} STU_RGB_t;

static inline UINT32 CastS32toU32(INT32 Val)
{
    return (UINT32)Val;
}

static inline UINT8 CastU16toU8(UINT16 Val)
{
    return (UINT8)Val;
}

static inline UINT8 CastU32toU8(UINT32 Val)
{
    return (UINT8)Val;
}


/**
* This API is used to convert disparity to distance.
* @param [in] pCfg The stereo bar and disparity config. Please refer to AMBA_STU_TO_DIST_CFG_s for more details.
* @param [in] DisparityVal Disparity value.
* @param [out] pDistance Distance.
* @return ErrorCode
*/
UINT32 AmbaSTU_Disparity2Distance(const AMBA_STU_DSI_TO_DIST_CFG_s *pCfg, UINT16 DisparityVal, DOUBLE *pDistance)
{
    UINT32 Rval = STU_OK;

    if (pCfg == NULL) {
        Rval = STU_ERROR_GENERAL;
    } else {
        if (pCfg->DisparityValShift < MAX_DISPARITY_SHIFT_VALUE) {
            const DOUBLE ShiftVal[MAX_DISPARITY_SHIFT_VALUE] = {1.0, 1.0, 1.0, 8.0, 16.0, 32.0};
            *pDistance = ((pCfg->FocalLength*pCfg->BaseLine) / ((DOUBLE)DisparityVal/ShiftVal[pCfg->DisparityValShift]));
        } else {
            Rval = STU_ERROR_GENERAL;
        }
    }

    return Rval;
}


/**
* This API is used get the default color palette of the disparity map.
* @param [in] pColorOrder The OSD color palette order. Please refer to AMBA_STU_OSD_PALETTE_ORDER_s for more details.
* @param [out] pColorPalette OSD color palette.
* @return ErrorCode
*/
UINT32 AmbaSTU_GetDefaultColorPalette(const AMBA_STU_OSD_PALETTE_ORDER_s *pColorOrder, AMBA_STU_8BITS_COLOR_PALETTE_s *pColorPalette)
{
    UINT32 Rval = STU_OK;

    static const STU_RGB_t DsiColorMap[256] = {
            { 255U, 0U, 0U }, { 255U, 31U, 0U }, { 255U, 63U, 0U }, { 255U, 95U, 0U },
            { 255U, 127U, 0U }, { 255U, 159U, 0U }, { 255U, 191U, 0U }, { 255U, 223U, 0U },
            { 255U, 255U, 0U }, { 224U, 255U, 0U }, { 192U, 255U, 0U }, { 160U, 255U, 0U },
            { 128U, 255U, 0U }, { 96U, 255U, 0U }, { 64U, 255U, 0U }, { 32U, 255U, 0U },
            { 0U, 255U, 0U }, { 0U, 255U, 15U }, { 0U, 255U, 31U }, { 0U, 255U, 47U },
            { 0U, 255U, 63U }, { 0U, 255U, 79U }, { 0U, 255U, 95U }, { 0U, 255U, 111U },
            { 0U, 255U, 127U }, { 0U, 255U, 143U }, { 0U, 255U, 159U }, { 0U, 255U, 175U },
            { 0U, 255U, 191U }, { 0U, 255U, 207U }, { 0U, 255U, 223U }, { 0U, 255U, 239U },
            { 0U, 255U, 255U }, { 0U, 248U, 255U }, { 0U, 240U, 255U }, { 0U, 232U, 255U },
            { 0U, 224U, 255U }, { 0U, 216U, 255U }, { 0U, 208U, 255U }, { 0U, 200U, 255U },
            { 0U, 192U, 255U }, { 0U, 184U, 255U }, { 0U, 176U, 255U }, { 0U, 168U, 255U },
            { 0U, 160U, 255U }, { 0U, 152U, 255U }, { 0U, 144U, 255U }, { 0U, 136U, 255U },
            { 0U, 128U, 255U }, { 0U, 120U, 255U }, { 0U, 112U, 255U }, { 0U, 104U, 255U },
            { 0U, 96U, 255U }, { 0U, 88U, 255U }, { 0U, 80U, 255U }, { 0U, 72U, 255U },
            { 0U, 64U, 255U }, { 0U, 56U, 255U }, { 0U, 48U, 255U }, { 0U, 40U, 255U },
            { 0U, 32U, 255U }, { 0U, 24U, 255U }, { 0U, 16U, 255U }, { 0U, 8U, 255U },
            { 0U, 0U, 255U }, { 3U, 0U, 255U }, { 7U, 0U, 255U }, { 11U, 0U, 255U },
            { 15U, 0U, 255U }, { 19U, 0U, 255U }, { 23U, 0U, 255U }, { 27U, 0U, 255U },
            { 31U, 0U, 255U }, { 35U, 0U, 255U }, { 39U, 0U, 255U }, { 43U, 0U, 255U },
            { 47U, 0U, 255U }, { 51U, 0U, 255U }, { 55U, 0U, 255U }, { 59U, 0U, 255U },
            { 63U, 0U, 255U }, { 67U, 0U, 255U }, { 71U, 0U, 255U }, { 75U, 0U, 255U },
            { 79U, 0U, 255U }, { 83U, 0U, 255U }, { 87U, 0U, 255U }, { 91U, 0U, 255U },
            { 95U, 0U, 255U }, { 99U, 0U, 255U }, { 103U, 0U, 255U }, { 107U, 0U, 255U },
            { 111U, 0U, 255U }, { 115U, 0U, 255U }, { 119U, 0U, 255U }, { 123U, 0U, 255U },
            { 127U, 0U, 255U }, { 131U, 0U, 255U }, { 135U, 0U, 255U }, { 139U, 0U, 255U },
            { 143U, 0U, 255U }, { 147U, 0U, 255U }, { 151U, 0U, 255U }, { 155U, 0U, 255U },
            { 159U, 0U, 255U }, { 163U, 0U, 255U }, { 167U, 0U, 255U }, { 171U, 0U, 255U },
            { 175U, 0U, 255U }, { 179U, 0U, 255U }, { 183U, 0U, 255U }, { 187U, 0U, 255U },
            { 191U, 0U, 255U }, { 195U, 0U, 255U }, { 199U, 0U, 255U }, { 203U, 0U, 255U },
            { 207U, 0U, 255U }, { 211U, 0U, 255U }, { 215U, 0U, 255U }, { 219U, 0U, 255U },
            { 223U, 0U, 255U }, { 227U, 0U, 255U }, { 231U, 0U, 255U }, { 235U, 0U, 255U },
            { 239U, 0U, 255U }, { 243U, 0U, 255U }, { 247U, 0U, 255U }, { 251U, 0U, 255U },
            { 255U, 0U, 255U }, { 255U, 0U, 254U }, { 255U, 0U, 252U }, { 255U, 0U, 250U },
            { 255U, 0U, 248U }, { 255U, 0U, 246U }, { 255U, 0U, 244U }, { 255U, 0U, 242U },
            { 255U, 0U, 240U }, { 255U, 0U, 238U }, { 255U, 0U, 236U }, { 255U, 0U, 234U },
            { 255U, 0U, 232U }, { 255U, 0U, 230U }, { 255U, 0U, 228U }, { 255U, 0U, 226U },
            { 255U, 0U, 224U }, { 255U, 0U, 222U }, { 255U, 0U, 220U }, { 255U, 0U, 218U },
            { 255U, 0U, 216U }, { 255U, 0U, 214U }, { 255U, 0U, 212U }, { 255U, 0U, 210U },
            { 255U, 0U, 208U }, { 255U, 0U, 206U }, { 255U, 0U, 204U }, { 255U, 0U, 202U },
            { 255U, 0U, 200U }, { 255U, 0U, 198U }, { 255U, 0U, 196U }, { 255U, 0U, 194U },
            { 255U, 0U, 192U }, { 255U, 0U, 190U }, { 255U, 0U, 188U }, { 255U, 0U, 186U },
            { 255U, 0U, 184U }, { 255U, 0U, 182U }, { 255U, 0U, 180U }, { 255U, 0U, 178U },
            { 255U, 0U, 176U }, { 255U, 0U, 174U }, { 255U, 0U, 172U }, { 255U, 0U, 170U },
            { 255U, 0U, 168U }, { 255U, 0U, 166U }, { 255U, 0U, 164U }, { 255U, 0U, 162U },
            { 255U, 0U, 160U }, { 255U, 0U, 158U }, { 255U, 0U, 156U }, { 255U, 0U, 154U },
            { 255U, 0U, 152U }, { 255U, 0U, 150U }, { 255U, 0U, 148U }, { 255U, 0U, 146U },
            { 255U, 0U, 144U }, { 255U, 0U, 142U }, { 255U, 0U, 140U }, { 255U, 0U, 138U },
            { 255U, 0U, 136U }, { 255U, 0U, 134U }, { 255U, 0U, 132U }, { 255U, 0U, 130U },
            { 255U, 0U, 128U }, { 255U, 0U, 126U }, { 255U, 0U, 124U }, { 255U, 0U, 122U },
            { 255U, 0U, 120U }, { 255U, 0U, 118U }, { 255U, 0U, 116U }, { 255U, 0U, 114U },
            { 255U, 0U, 112U }, { 255U, 0U, 110U }, { 255U, 0U, 108U }, { 255U, 0U, 106U },
            { 255U, 0U, 104U }, { 255U, 0U, 102U }, { 255U, 0U, 100U }, { 255U, 0U, 98U },
            { 255U, 0U, 96U }, { 255U, 0U, 94U }, { 255U, 0U, 92U }, { 255U, 0U, 90U },
            { 255U, 0U, 88U }, { 255U, 0U, 86U }, { 255U, 0U, 84U }, { 255U, 0U, 82U },
            { 255U, 0U, 80U }, { 255U, 0U, 78U }, { 255U, 0U, 76U }, { 255U, 0U, 74U },
            { 255U, 0U, 72U }, { 255U, 0U, 70U }, { 255U, 0U, 68U }, { 255U, 0U, 66U },
            { 255U, 0U, 64U }, { 255U, 0U, 62U }, { 255U, 0U, 60U }, { 255U, 0U, 58U },
            { 255U, 0U, 56U }, { 255U, 0U, 54U }, { 255U, 0U, 52U }, { 255U, 0U, 50U },
            { 255U, 0U, 48U }, { 255U, 0U, 46U }, { 255U, 0U, 44U }, { 255U, 0U, 42U },
            { 255U, 0U, 40U }, { 255U, 0U, 38U }, { 255U, 0U, 36U }, { 255U, 0U, 34U },
            { 255U, 0U, 32U }, { 255U, 0U, 30U }, { 255U, 0U, 28U }, { 255U, 0U, 26U },
            { 255U, 0U, 24U }, { 255U, 0U, 22U }, { 255U, 0U, 20U }, { 255U, 0U, 18U },
            { 255U, 0U, 16U }, { 255U, 0U, 14U }, { 255U, 0U, 12U }, { 255U, 0U, 10U },
            { 255U, 0U, 8U }, { 255U, 0U, 6U }, { 255U, 0U, 4U }, { 255U, 0U, 2U }, };
    if ((pColorOrder == NULL) || (pColorPalette == NULL)) {
        Rval = STU_ERROR_GENERAL;
    } else {
        UINT32 i, J;
        UINT32 Shift[4] = {0,0,0,0};
        for (i = 0U; i < 4U; i++) {
            Shift[pColorOrder->Color[i]] = i;
        }
        J = 0U;
        for (i = Shift[AMBA_STU_OSD_RED]; i < 1024U; i+=4U) {
            pColorPalette->ColorLUT[i] = DsiColorMap[J].R;
            J++;
        }
        J = 0U;
        for (i = Shift[AMBA_STU_OSD_GREEN]; i < 1024U; i+=4U) {
            pColorPalette->ColorLUT[i] = DsiColorMap[J].G;
            J++;
        }
        J = 0U;
        for (i = Shift[AMBA_STU_OSD_BLUE]; i < 1024U; i+=4U) {
            pColorPalette->ColorLUT[i] = DsiColorMap[J].B;
            J++;
        }
        J = 0U;
        for (i = Shift[AMBA_STU_OSD_ALPHA]; i < 1024U; i+=4U) {
            pColorPalette->ColorLUT[i] = 0x80U;
            J++;
        }
    }
    return Rval;
}

/**
* This API is used to convert 16 bits unpacked disparity data to OSD index.
* @param [in] pDsiInfo The unpacked disparity data. Please refer to AMBA_STU_DSI_INFO_s for more details.
* @param [in] pColor8BitsIdxLut The look up table that used to convert disparity to the OSD index.
* @param [in] pDsiFeedStartPos The start position of the disparity map. Please refer to AMBA_STU_POINT_INT_2D_s for more details.
* @param [in] pColorTblSize The size of the disparity map. Please refer to AMBA_STU_SIZE_s for more details.
* @param [out] pColorTbl OSD color index map.
* @return ErrorCode
*/
UINT32 AmbaSTU_ConvDsi16BitsToColorIdx(const AMBA_STU_DSI_INFO_s *pDsiInfo, const UINT32 *pColor8BitsIdxLut, const AMBA_STU_POINT_INT_2D_s *pDsiFeedStartPos, const AMBA_STU_SIZE_s *pColorTblSize, UINT8 *pColorTbl)
{
    UINT32 Rval = STU_OK;
    UINT32 i, J;
    UINT8 *pColorTblLineStartPos;
    UINT16 *pDisparityTblPos;
    UINT32 Shift;
    UINT32 DsiWidth;
    UINT32 DsiHeight;
    if ((pDsiInfo == NULL) || (pDsiFeedStartPos == NULL) || (pColorTblSize == NULL) || (pColorTbl == NULL)) {
        Rval = STU_ERROR_GENERAL;
    } else {
        pColorTblLineStartPos = &pColorTbl[(pColorTblSize->Width * CastS32toU32(pDsiFeedStartPos->Y)) + CastS32toU32(pDsiFeedStartPos->X)];

        pDisparityTblPos = pDsiInfo->pDisparityTbl;
        Shift = pDsiInfo->Shift;
        DsiWidth = pDsiInfo->Size.Width;
        DsiHeight = pDsiInfo->Size.Height;
        if (pColor8BitsIdxLut == NULL) {
            // Skip mapping
            for (J = 0U; J < DsiHeight; J++) {
                for (i = 0U; i < DsiWidth; i++) {
                    pColorTblLineStartPos[i] = CastU16toU8(pDisparityTblPos[i] >> Shift);
                }
                pColorTblLineStartPos = &pColorTblLineStartPos[pColorTblSize->Width];
                pDisparityTblPos = &pDisparityTblPos[DsiWidth];
            }
        } else {
            for (J = 0U; J < DsiHeight; J++) {
                for (i = 0U; i < DsiWidth; i++) {
                    pColorTblLineStartPos[i] = CastU32toU8(pColor8BitsIdxLut[pDisparityTblPos[i] >> Shift]);
                }
                pColorTblLineStartPos = &pColorTblLineStartPos[pColorTblSize->Width];
                pDisparityTblPos = &pDisparityTblPos[DsiWidth];
            }
        }
    }
    return Rval;
}

static inline void Unpack10bitsTo16bits(const UINT8 *pIn10Bits, UINT16 *pOut16Bits)
{
    pOut16Bits[0] = ((((UINT16) pIn10Bits[1]) & 0x03U) << 8) | ((UINT16) (pIn10Bits[0]));
    pOut16Bits[1] = ((((UINT16) pIn10Bits[2]) & 0x0FU) << 6) | ((((UINT16) pIn10Bits[1]) & 0xfCU) >> 2);
    pOut16Bits[2] = ((((UINT16) pIn10Bits[3]) & 0x3FU) << 4) | ((((UINT16) pIn10Bits[2]) & 0xf0U) >> 4);
    pOut16Bits[3] = (((UINT16) pIn10Bits[4]) << 2) | ((((UINT16) pIn10Bits[3]) & 0xC0U) >> 6);
}

/**
* This API is used to convert 10 bits packed disparity data to 16 bits unpacked disparity data.
* @param [in] pIn10BitsTbl The packed disparity data.
* @param [in] Width The disparity map width.
* @param [out] pOut16BitsTbl The unpacked disparity data.
* @return ErrorCode
*/
UINT32 AmbaSTU_Unpack10BitsTo16Bits(const UINT8 *pIn10BitsTbl, UINT32 Width, UINT16 *pOut16BitsTbl)
{
    UINT32 Rval = STU_OK;
    UINT32 X;
    const UINT8 *pLineSrc = pIn10BitsTbl;
    UINT16 *pDst = pOut16BitsTbl;
    if ((pIn10BitsTbl == NULL) || (pOut16BitsTbl == NULL)) {
        Rval = STU_ERROR_GENERAL;
    } else {
        for (X = 0U; X < Width; X += 4U) {
            Unpack10bitsTo16bits(pLineSrc, pDst);
            pLineSrc = &pLineSrc[5];
            pDst = &pDst[4];
        }
    }
    return Rval;
}

/**
* This API is used get the warp table.
* @param [in] pStereoCamWarpInfo The stereo calibration data.
* @param [out] pWarpTbl The warp table.
* @return ErrorCode
*/
UINT32 AmbaSTU_GetWarpTblAddr(const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfo, AMBA_IK_GRID_POINT_s **pWarpTbl)
{
    UINT32 Rval = STU_OK;
    const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfoMisraC = pStereoCamWarpInfo;
    ULONG DataStartAddr, WarpTblStartAddr;
    if ((pStereoCamWarpInfo == NULL) || (pWarpTbl == NULL)) {
        Rval = STU_ERROR_GENERAL;
    } else {
        if ((pStereoCamWarpInfo->Method.Version[0] == 2U) &&
            (pStereoCamWarpInfo->Method.Version[1] == 1U) &&
            (pStereoCamWarpInfo->Method.Version[2] == 0U) &&
            (pStereoCamWarpInfo->Method.Version[3] == 0U)) {
            if (0U != AmbaWrap_memcpy(&DataStartAddr, &pStereoCamWarpInfoMisraC, sizeof(void *))) {
                Rval = STU_ERROR_GENERAL;
            }
            WarpTblStartAddr = DataStartAddr + pStereoCamWarpInfo->Method.V2100.HBase.TableOffset;
            if (0U != AmbaWrap_memcpy(pWarpTbl, &WarpTblStartAddr, sizeof(AMBA_IK_GRID_POINT_s *))) {
                Rval = STU_ERROR_GENERAL;
            }
        } else if ((pStereoCamWarpInfo->Method.Version[0] == 2U) &&
                (pStereoCamWarpInfo->Method.Version[1] == 2U) &&
                (pStereoCamWarpInfo->Method.Version[2] == 0U) &&
                (pStereoCamWarpInfo->Method.Version[3] == 0U)) {
            if (0U != AmbaWrap_memcpy(&DataStartAddr, &pStereoCamWarpInfoMisraC, sizeof(void *))) {
                Rval = STU_ERROR_GENERAL;
            }
            WarpTblStartAddr = DataStartAddr + pStereoCamWarpInfo->Method.V2200.HBase.TableOffset;
            if (0U != AmbaWrap_memcpy(pWarpTbl, &WarpTblStartAddr, sizeof(AMBA_IK_GRID_POINT_s *))) {
                Rval = STU_ERROR_GENERAL;
            }
        } else if ((pStereoCamWarpInfo->Method.Version[0] == 2U) &&
                (pStereoCamWarpInfo->Method.Version[1] == 3U) &&
                (pStereoCamWarpInfo->Method.Version[2] == 0U) &&
                (pStereoCamWarpInfo->Method.Version[3] == 0U)) {
            if (0U != AmbaWrap_memcpy(&DataStartAddr, &pStereoCamWarpInfoMisraC, sizeof(void *))) {
                Rval = STU_ERROR_GENERAL;
            }
            WarpTblStartAddr = DataStartAddr + pStereoCamWarpInfo->Method.V2300.HBase.TableOffset;
            if (0U != AmbaWrap_memcpy(pWarpTbl, &WarpTblStartAddr, sizeof(AMBA_IK_GRID_POINT_s *))) {
                Rval = STU_ERROR_GENERAL;
            }
        } else if ((pStereoCamWarpInfo->Method.Version[0] == 2U) &&
                (pStereoCamWarpInfo->Method.Version[1] == 3U) &&
                (pStereoCamWarpInfo->Method.Version[2] == 1U) &&
                (pStereoCamWarpInfo->Method.Version[3] == 0U)) {
            if (0U != AmbaWrap_memcpy(&DataStartAddr, &pStereoCamWarpInfoMisraC, sizeof(void *))) {
                Rval = STU_ERROR_GENERAL;
            }
            WarpTblStartAddr = DataStartAddr + pStereoCamWarpInfo->Method.V2310.HBase.TableOffset;
            if (0U != AmbaWrap_memcpy(pWarpTbl, &WarpTblStartAddr, sizeof(AMBA_IK_GRID_POINT_s *))) {
                Rval = STU_ERROR_GENERAL;
            }
        } else {
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV2FS)
            const AMBA_CAL_GRID_POINT_s *pAstWarpTbl;
            pAstWarpTbl = &pStereoCamWarpInfo->Method.VAst.WarpTbl.WarpVector[0];
            if (0U != AmbaWrap_memcpy(pWarpTbl, &pAstWarpTbl, sizeof(AMBA_IK_GRID_POINT_s *))) {
                Rval = STU_ERROR_GENERAL;
            }
#else
            Rval = STU_ERROR_GENERAL;
#endif
        }
    }
    return Rval;
}

/**
* This API is used to get the PGM header size.
* @param [out] pMaxHeaderSize The PGM header size.
* @return ErrorCode
*/
UINT32 AmbaSTU_GetPGMMaxHeaderSize(UINT32 *pMaxHeaderSize)
{
    *pMaxHeaderSize =
    PGM_BIT16_HEADER_SIZE +
    PGM_BASELIBE_HEADER_SIZE + 10U +
    PGM_DSI_FMT_HEADER_SIZE + 3U +
    PGM_SCALED_FOCAL_LENGTH_SIZE + 21U +
    PGM_OPTICAL_CENTER_HEADER_SIZE + 21U +
    PGM_PROJECTION_MODEL_HEADER_SIZE + 9U +
    PGM_SCALE_HEADER_SIZE + 1U +
    /*RESOLUTION*/15U;
    return STU_OK;
}

/**
* This API is used to convert disparity map to PGM file format.
* @param [in] pMetaData The meta data. Please refer to AMBA_STU_PGM_METADATA_S for more details.
* @param [in] pUnpackedDsiTbl The unpacked disparity map.
* @param [in] DsiTblSize The disparity map size.
* @param [out] pPGMDataBuf The PGM file format data.
* @param [out] pPGMDataSize The PGM file format data size.
* @return ErrorCode
*/
UINT32 AmbaSTU_DsiToPGM(const AMBA_STU_PGM_METADATA_S *pMetaData, const UINT16 *pUnpackedDsiTbl, UINT32 DsiTblSize, UINT8 *pPGMDataBuf, UINT32 *pPGMDataSize)
{
    UINT32 Sof = 0U;
    UINT32 Eof = 0U;
    UINT32 Rval = STU_OK;
    UINT32 Depth;
    STU_PGMGen16BitHeader(pPGMDataBuf, Sof, &Eof);
    Sof = Eof;
#if 0
    Rval |= STU_PGMGenBaselineHeader(pPGMDataBuf, Sof, pMetaData->Baseline, &Eof);
    Sof = Eof;

    Rval |= STU_PGMGenScaledFocalLength(pPGMDataBuf, Sof, pMetaData->PixelFocalLength[0], pMetaData->PixelFocalLength[1], &Eof);
    Sof = Eof;

    Rval |= STU_PGMGenOpticalCenter(pPGMDataBuf, Sof, &pMetaData->OpticalCenterOfRightCam, &Eof);
    Sof = Eof;
#endif
    Rval |= STU_PGMGenDsiFmtHeader(pPGMDataBuf, Sof, pMetaData->IntegerBitNum, pMetaData->DecimalBitNum, &Eof);
    Sof = Eof;

    Rval |= STU_PGMGenProjectionModel(pPGMDataBuf, Sof, pMetaData->ProjectionModel, &Eof);
    Sof = Eof;

    Rval |= STU_PGMGenScale(pPGMDataBuf, Sof, pMetaData->Scale, &Eof);
    Sof = Eof;

    Depth = pMetaData->IntegerBitNum + pMetaData->DecimalBitNum;
    Rval |= STU_PGMGenResolutionHeader(pPGMDataBuf, Sof, pMetaData->Width, pMetaData->Height, Depth, &Eof);
    Sof = Eof;
    Rval |= STU_PGMReverseDsi(pPGMDataBuf, Sof, pUnpackedDsiTbl, DsiTblSize, &Eof);
    *pPGMDataSize = Sof + DsiTblSize;
    return Rval;
}

inline static UINT32 STU_HsvToRgb(const FLOAT pHsv[3U], FLOAT pRgb[3U])
{
    UINT32 Rval = STU_OK;
    FLOAT H = pHsv[0U];
    FLOAT S = pHsv[1U];
    FLOAT V = pHsv[2U];
    FLOAT C  = V * S;
    FLOAT H2 = 6.0f * H;
    FLOAT X;

    // Compute X
    {
        DOUBLE H2Div2Modulo;
        DOUBLE H2Div2ModuloM1;
        DOUBLE AbsH2Div2ModuloM1;

        if (0U != AmbaWrap_fmod(H2, 2.0, &H2Div2Modulo)) {
            Rval = STU_ERROR_GENERAL;
        }
        H2Div2ModuloM1 = H2Div2Modulo - 1.0;

        if (0U != AmbaWrap_fabs(H2Div2ModuloM1, &AbsH2Div2ModuloM1)) {
            Rval = STU_ERROR_GENERAL;
        }

        X = C*(1.0f - (FLOAT)AbsH2Div2ModuloM1);
    }

    if ( (0.0 <= H2) && (H2 < 1.0) ) {
        pRgb[0U] = C;
        pRgb[1U] = X;
        pRgb[2U] = 0.0f;
    } else if ( (1.0 <= H2) && (H2 < 2.0) ) {
        pRgb[0U] = X;
        pRgb[1U] = C;
        pRgb[2U] = 0.0f;
    } else if ( (2.0 <= H2) && (H2 < 3.0) ) {
        pRgb[0U] = 0.0f;
        pRgb[1U] = C;
        pRgb[2U] = X;
    } else if ( (3.0 <= H2) && (H2 < 4.0) ) {
        pRgb[0U] = 0.0f;
        pRgb[1U] = X;
        pRgb[2U] = C;
    } else if ( (4.0 <= H2) && (H2 < 5.0) ) {
        pRgb[0U] = X;
        pRgb[1U] = 0.0f;
        pRgb[2U] = C;
    } else if ( (5.0 <= H2) && (H2 <= 6.0) ) {
        pRgb[0U] = C;
        pRgb[1U] = 0.0f;
        pRgb[2U] = X;
    } else if ( H2 > 6.0 ) {
        pRgb[0U] = 1.0f;
        pRgb[1U] = 0.0f;
        pRgb[2U] = 0.0f;
    } else if ( H2 < 0.0 ) {
        pRgb[0U] = 0.0f;
        pRgb[1U] = 1.0f;
        pRgb[2U] = 0.0f;
    } else {
        // Nothing to do
    }

    return Rval;
}

inline static UINT32 STU_DofToFlowXY(const UINT16 *pOfVal, FLOAT *pXVal, FLOAT *pYVal)
{
    UINT32 Rval = STU_OK;
    UINT16 TmpValU16;
    INT8 TmpValS8;

    TmpValU16 = *pOfVal;
    TmpValU16 >>= 8U;
    TmpValU16 &= 0x00FFU;
    TmpValS8 = (INT8)TmpValU16;
    *pYVal = (FLOAT)TmpValS8/8.0f;

    TmpValU16 = *pOfVal;
    TmpValU16 &= 0x00FFU;
    TmpValS8 = (INT8)TmpValU16;
    *pXVal = (FLOAT)TmpValS8/8.0f;

    return Rval;
}

inline static UINT32 STU_FlowXYToHsv(const FLOAT *pXVal, const FLOAT *pYVal, FLOAT pHsv[3U])
{
    UINT32 Rval = STU_OK;
    FLOAT TmpValFT;
    DOUBLE TmpValDB;
    DOUBLE Mag;
    DOUBLE Dir;
    DOUBLE H_DB;

    FLOAT MaxFlow = 35.77708764f; //# Note sqrt ( 32.0 x 32.0 + 16.0 x 16.0 )
    FLOAT N = 8.0f; // multiplier
    FLOAT Radian360 = 6.28318530718f; // 2 x PI

    TmpValFT = (*pXVal*(*pXVal)) + (*pYVal*(*pYVal));
    TmpValDB = (DOUBLE)TmpValFT;

    if (0U != AmbaWrap_sqrt(TmpValDB, &Mag)) {
        Rval = STU_ERROR_GENERAL;
    }
    if (0U != AmbaWrap_atan2(*pYVal, *pXVal, &Dir)) {
        Rval = STU_ERROR_GENERAL;
    }

    TmpValDB = Dir/Radian360;
    TmpValDB += 1.0;
    if (0U != AmbaWrap_fmod(TmpValDB, 1.0, &H_DB)) {
        Rval = STU_ERROR_GENERAL;
    }
    pHsv[0U] = (FLOAT)H_DB;

    TmpValDB = (Mag*N)/MaxFlow;
    TmpValDB = ((TmpValDB > 0.0) ? TmpValDB:0.0);
    pHsv[1U] = ((TmpValDB < 1.0) ? (FLOAT)TmpValDB:1.0f);

    TmpValFT = N - pHsv[1U];
    TmpValDB = (DOUBLE)TmpValFT;
    TmpValDB = ((TmpValDB > 0.0) ? TmpValDB:0.0);
    pHsv[2U] = ((TmpValDB < 1.0) ? (FLOAT)TmpValDB:1.0f);

    return Rval;
}

/**
* This API is used to convert dense optical flow map to ARGB buffer.
* @param [in] pOf Input dense optical flow. Please refer to AMBA_STU_DOF_S for more details.
* @param [out] pArgbDataBuf Address of ARGB buffer. The content is arranged as [A0][R0][G0][B0], [A1][R1][G1][B1] ..., 8 bits per element. Value of alpha channel is remained the same during processing.
* @return ErrorCode
*/
UINT32 AmbaSTU_DofToRgb(const AMBA_STU_DOF_S *pOf, UINT8 *pArgbDataBuf)
{
    UINT32 Rval = STU_OK;

    const UINT16 *pOfDataAddr;

    UINT32 OfPitchElement;
    UINT32 ArgbPitchElement;

    UINT32 RowIdx;
    UINT32 ColIdx;

    UINT32 OutDataDepth = 4U;

    if ((pOf == NULL) || (pArgbDataBuf == NULL)) {
        Rval = STU_ERROR_GENERAL;
    } else {
        if (0U != AmbaWrap_memcpy(&pOfDataAddr, &pOf->pData, sizeof(pOf->pData))) {
            Rval = STU_ERROR_GENERAL;
        }

        OfPitchElement = pOf->Pitch / sizeof(UINT16);
        ArgbPitchElement = pOf->Width;

        for (RowIdx = 0; RowIdx < pOf->Height; RowIdx++ ) {
            UINT32 OfDataIndex;
            UINT32 ArgbDataIndex;

            OfDataIndex = (RowIdx*OfPitchElement);
            ArgbDataIndex = (RowIdx*ArgbPitchElement)*OutDataDepth;

            for (ColIdx = 0; ColIdx < pOf->Width; ColIdx++ ) {
                FLOAT Rgb[3U] = {0.0f, 0.0f, 0.0f};

                // Convert optical flow to rgb value
                if ( pOfDataAddr[OfDataIndex] != 0xFFFFU ) {
                    FLOAT X, Y;
                    FLOAT Hsv[3U];

                    Rval |= STU_DofToFlowXY(&pOfDataAddr[OfDataIndex], &X, &Y);
                    Rval |= STU_FlowXYToHsv(&X, &Y, Hsv);
                    Rval |= STU_HsvToRgb(Hsv, Rgb);
                }

                // Fill RGB value to ARGB buffer
                {
                    FLOAT TmpValFT;

                    TmpValFT = Rgb[0U] * 255.0f;
                    pArgbDataBuf[ArgbDataIndex + 1U] = (UINT8)TmpValFT;
                    TmpValFT = Rgb[1U] * 255.0f;
                    pArgbDataBuf[ArgbDataIndex + 2U] = (UINT8)TmpValFT;
                    TmpValFT = Rgb[2U] * 255.0f;
                    pArgbDataBuf[ArgbDataIndex + 3U] = (UINT8)TmpValFT;
                }

                OfDataIndex++;
                ArgbDataIndex+=OutDataDepth;
            }
        }
    }

    return Rval;
}

static UINT32 GetCalibMetaData(const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfo, STEREO_CALIBRATION_METADATA_t **pMetaData)
{
    UINT32 RetVal = STU_OK;
    ULONG DataStartAddr = 0U;
    ULONG MetaDataStartAddr = 0U;

    if ((pStereoCamWarpInfo == NULL) || (pMetaData == NULL)) {
        RetVal = STU_ERROR_GENERAL;
    } else {
        if ((pStereoCamWarpInfo->Method.Version[0] == 2U) &&
            (pStereoCamWarpInfo->Method.Version[1] == 1U) &&
            (pStereoCamWarpInfo->Method.Version[2] == 0U) &&
            (pStereoCamWarpInfo->Method.Version[3] == 0U)) {
            RetVal |= AmbaWrap_memcpy(&DataStartAddr, &pStereoCamWarpInfo, sizeof(void *));
            MetaDataStartAddr = DataStartAddr + pStereoCamWarpInfo->Method.V2100.HBase.ExifOffset;
        } else if ((pStereoCamWarpInfo->Method.Version[0] == 2U) &&
                (pStereoCamWarpInfo->Method.Version[1] == 2U) &&
                (pStereoCamWarpInfo->Method.Version[2] == 0U) &&
                (pStereoCamWarpInfo->Method.Version[3] == 0U)) {
            RetVal |= AmbaWrap_memcpy(&DataStartAddr, &pStereoCamWarpInfo, sizeof(void *));
            MetaDataStartAddr = DataStartAddr + pStereoCamWarpInfo->Method.V2200.HBase.ExifOffset;
        } else if ((pStereoCamWarpInfo->Method.Version[0] == 2U) &&
                (pStereoCamWarpInfo->Method.Version[1] == 3U) &&
                (pStereoCamWarpInfo->Method.Version[2] == 0U) &&
                (pStereoCamWarpInfo->Method.Version[3] == 0U)) {
            RetVal |=AmbaWrap_memcpy(&DataStartAddr, &pStereoCamWarpInfo, sizeof(void *));
            MetaDataStartAddr = DataStartAddr + pStereoCamWarpInfo->Method.V2300.HBase.ExifOffset;
        } else {
            RetVal = STU_ERROR_GENERAL;
        }

        if (RetVal == STU_OK) {
            // Shift by size of exIf header
            MetaDataStartAddr += 4U;
            if (0U != AmbaWrap_memcpy(pMetaData, &MetaDataStartAddr, sizeof(void *))) {
                RetVal = STU_ERROR_GENERAL;
            }
        }
    }

    return RetVal;
}

/**
* This API is used to get optical center of target camera from stereo calibration data.
* @param [in] pStereoCamWarpInfo The stereo calibration data.
* @param [out] pU0 U0.
* @param [out] pV0 V0.
* @return ErrorCode
*/
UINT32 AmbaSTU_GetTargetOpticalCenter(const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfo, DOUBLE *pU0, DOUBLE *pV0)
{
    UINT32 RetVal = STU_OK;

    if ((pStereoCamWarpInfo == NULL) || (pU0 == NULL) || (pV0 == NULL)) {
        RetVal = STU_ERROR_GENERAL;
    } else {
        STEREO_CALIBRATION_METADATA_t *pMetaData;
        RetVal = GetCalibMetaData(pStereoCamWarpInfo, &pMetaData);
        if ( RetVal == STU_OK ) {
            *pU0 = pMetaData->TargetParams.U0;
            *pV0 = pMetaData->TargetParams.V0;
        }
    }

    return RetVal;
}

/**
* This API is used to get intrinsic parameter of target camera from stereo warp table. 
* @param [in] pStereoCamWarpInfo The stereo warp table.
* @param [out] pIntParam Intrinsic parameter.
* @return ErrorCode
*/
UINT32 AmbaSTU_GetTargetIntrinsicParam(const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfo, AMBA_STU_INTRINSIC_PARAM_S *pIntParam)
{
    UINT32 RetVal = STU_OK;

    if ((pStereoCamWarpInfo == NULL) || (pIntParam == NULL)) {
        RetVal = STU_ERROR_GENERAL;
    } else {
        STEREO_CALIBRATION_METADATA_t *pMetaData;
        RetVal = GetCalibMetaData(pStereoCamWarpInfo, &pMetaData);
        if ( RetVal == STU_OK ) {
            pIntParam->U0 = pMetaData->TargetParams.U0;
            pIntParam->V0 = pMetaData->TargetParams.V0;
            pIntParam->Ku = pMetaData->TargetParams.Ku;
            pIntParam->Kv = pMetaData->TargetParams.Kv;
        }
    }

    return RetVal;
}

/**
* This API is used to get baseline of stereo camera from stereo warp table.�
* @param [in] pStereoCamWarpInfo The stereo warp table.
* @param [out] pBaseline Stereo baseline.
* @return ErrorCode
*/
UINT32 AmbaSTU_GetStereoBaseline(const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfo, DOUBLE *pBaseline)
{
    UINT32 RetVal = STU_OK;

    if ((pStereoCamWarpInfo == NULL) || (pBaseline == NULL)) {
        RetVal = STU_ERROR_GENERAL;
    } else {
        STEREO_CALIBRATION_METADATA_t *pMetaData;
        RetVal = GetCalibMetaData(pStereoCamWarpInfo, &pMetaData);
        if ( RetVal == STU_OK ) {
            *pBaseline = pMetaData->Baseline;
        }
    }

    return RetVal;
}

/**
* This API is used to get extrinsic parameter of source camera from stereo calibration data.
* @param [in] pStereoCamWarpInfo The stereo calibration data.
* @param [out] pExtParam Extrinsic parameter.
* @return ErrorCode
*/
UINT32 AmbaSTU_GetSourceExtrinsicParam(const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfo, AMBA_STU_EXTRINSIC_PARAM_S *pExtParam)
{
    UINT32 RetVal = STU_OK;

    if ((pStereoCamWarpInfo == NULL) || (pExtParam == NULL)) {
        RetVal = STU_ERROR_GENERAL;
    } else {
        STEREO_CALIBRATION_METADATA_t *pMetaData;
        RetVal = GetCalibMetaData(pStereoCamWarpInfo, &pMetaData);
        if ( RetVal == STU_OK ) {
            pExtParam->X = pMetaData->SourcePose.Position[0U];
            pExtParam->Y = pMetaData->SourcePose.Position[1U];
            pExtParam->Z = pMetaData->SourcePose.Position[2U];

            pExtParam->Yaw = pMetaData->SourcePose.Angle[0U];
            pExtParam->Pitch = pMetaData->SourcePose.Angle[1U];
            pExtParam->Roll = pMetaData->SourcePose.Angle[2U];
        }
    }

    return RetVal;
}

