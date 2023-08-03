
#ifndef UTIL_VSL_COMMON_H
#define UTIL_VSL_COMMON_H

#include "AmbaTypes.h"
#include "AmbaOD_2DBbx.h"
#include "AmbaSurround.h"
#include "Util_DrawFunction.h"
#include "AmbaSR_Lane.h"
#include "RefFlow_FC.h"

/**< Define error code*/
#define UTIL_VSL_ERR_OK (0U)
#define UTIL_VSL_ERR_NG (1U)

#define UTIL_VSL_RGBA_ARRAY_SIZE (4U)

/**< The 4th element indicates transparency.
 * The smaller the value, the more transparent. */
//                                          (idx)  /**< (  R,   G,   B,   A) */
#define UTIL_VSL_COLOR_IDX_RED              ( 1U)  /**< (255,   0,   0, 255) */
#define UTIL_VSL_COLOR_IDX_GREEN            ( 2U)  /**< (  0, 255,   0, 255) */
#define UTIL_VSL_COLOR_IDX_BLUE             ( 3U)  /**< (  0,   0, 255, 255) */
#define UTIL_VSL_COLOR_IDX_CYAN             ( 4U)  /**< (  0, 255, 255, 255) */
#define UTIL_VSL_COLOR_IDX_MAGENTA          ( 5U)  /**< (255,   0, 255, 255) */
#define UTIL_VSL_COLOR_IDX_YELLOW           ( 6U)  /**< (255, 255,   0, 255) */
#define UTIL_VSL_COLOR_IDX_128_128_128      ( 7U)  /**< (128, 128, 128, 255) */
#define UTIL_VSL_COLOR_IDX_160_82_45        ( 8U)  /**< (160,  82,  45, 255) */
#define UTIL_VSL_COLOR_IDX_156_102_31       ( 9U)  /**< (156, 102,  31, 255) */
#define UTIL_VSL_COLOR_IDX_255_128_0        (10U)  /**< (255, 128,   0, 255) */
#define UTIL_VSL_COLOR_IDX_178_34_34        (11U)  /**< (178,  34,  34, 255) */
#define UTIL_VSL_COLOR_IDX_BLACK            (12U)  /**< (  0,   0,   0, 255) */
#define UTIL_VSL_COLOR_IDX_PURPLE           (13U)  /**< (160,  32, 240, 255) */
#define UTIL_VSL_COLOR_IDX_128_128_128_128  (14U)  /**< (128, 128, 128, 128) */
#define UTIL_VSL_COLOR_IDX_BROWN            (15U)  /**< (139,  69,  19, 255) */
// Add additional color by user.
#define UTIL_VSL_COLOR_IDX_MAX              (256U)


/**
 * BEV configure (up to user).
 * Set width and height of BEV.
 * Set an original point (0,0) and scale on the image.
 */
typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT32 InitX;
    UINT32 InitY;
    UINT32 Scale; // mm/pixel (e.g. 1280*720(pixel) -> 128m*72m).
} UTIL_VSL_BEV_CFG_s;

typedef struct {
    UINT32 Id;
    UINT32 Cat;
    UINT16 ObjStartX;
    UINT16 ObjStartY;
    UINT16 ObjPitch;
    UINT16 ObjWidth;
    UINT16 ObjHeight;
    UINT8 *pObjTable;
} UTIL_VSL_MK_OBJ_INFO_s;

typedef struct {
    UINT32 ObjNum;
    UTIL_VSL_MK_OBJ_INFO_s ObjNNSegInfo[AMBA_OD_2DBBX_MAX_BBX_NUM];
} UTIL_VSL_MK_BBX_s;



/**
* Map a uint8 color index to RGBT value.
* @param [in] a uint8 color index.
* @param [in/out] RGBA matrix (A: transparency).
* @return success "0" or fail "1".
*/
UINT32 UtilVSL_GetRGBAValue(UINT8 ColorIdx, UINT8 RGBA[UTIL_VSL_RGBA_ARRAY_SIZE]);

UINT32 UtilVSL_DrawFrameCntPTS(UINT32 FrameCnt, UINT32 PTS, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

/**
* Run drawing horizontal line
* @param [in] Width     Width of bounding box domain (invalid at current version)
* @param [in] Height    Height of bounding box domain (invalid at current version)
* @param [in] pCalCfg   Calibration configure data
* @param [in] BufWidth  Width of buffer domain
* @param [in] BufHeight Height of buffer domain
* @param [in/out] a uint8 buffer
* @return Errorcode
*/
UINT32 UtilVSL_DrawHrztLine(UINT32 Width, UINT32 Height, const AMBA_CAL_EM_CALC_COORD_CFG_s *pCalCfg, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

/**
* Run drawing horizontal line V1
* @param [in] Width     Width of bounding box domain (invalid at current version)
* @param [in] Height    Height of bounding box domain (invalid at current version)
* @param [in] pCalCfg   Calibration configure V1 data
* @param [in] BufWidth  Width of buffer domain
* @param [in] BufHeight Height of buffer domain
* @param [in/out] a uint8 buffer
* @return Errorcode
*/
UINT32 UtilVSL_DrawHrztLineV1(UINT32 Width, UINT32 Height, const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCalCfg, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

/**
* Run drawing horizontal line
* @param [in] Width     Width of bounding box domain (invalid at current version)
* @param [in] Height    Height of bounding box domain (invalid at current version)
* @param [in] pCalCfg   Calibration configure data
* @param [in] BufWidth  Width of buffer domain
* @param [in] BufHeight Height of buffer domain
* @param [in/out] a uint8 buffer
* @return Errorcode
*/
UINT32 UtilVSL_DrawAuxLine(UINT32 Width, UINT32 Height, const AMBA_CAL_EM_CALC_COORD_CFG_s *pCalCfg, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

/**
* Run drawing bounding box system function
* @param [in] Width     Width of bounding box domain (source domain)
* @param [in] Height    Height of bounding box domain (source domain)
* @param [in] pBbxIn    Input bounding box data
* @param [in] BufWidth  Width of buffer domain
* @param [in] BufHeight Height of buffer domain
* @param [in/out] address of uint8 buffer
* @return success "0" or fail "1"
*/
UINT32 UtilVSL_DrawBbox(UINT32 Width, UINT32 Height, const AMBA_OD_2DBBX_LIST_s *pBbxIn, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

/**
* Run drawing bird-eye-view(BEV) SR system
* @param [in] pBevCfg BEV configure
* @param [in] pBbxIn  Bounding box data
* @param [in] pSRIn   Surround reconstruction data
* @param [in/out] a U8 buffer
* @return Errorcode
*/
UINT32 UtilVSL_DrawBEVSR(const UTIL_VSL_BEV_CFG_s *pBevCfg, const AMBA_OD_2DBBX_LIST_s *pBbxIn, const AMBA_SR_SROBJECT_DATA_s *pSRIn, UINT8 *pBuffer);

UINT32 UtilVSL_DrawLanePoints(UINT8 *pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_SR_LANE_RECONSTRUCT_INFO_s* pLaneInfo);
UINT32 UtilVSL_DrawLaneCenterLinePoints(UINT8 *pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_SR_LANE_RECONSTRUCT_INFO_s* pLaneInfo);

UINT32 UtilVSL_DrawLaneLineFunc(UINT8 *pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_CAL_EM_CALC_COORD_CFG_s* pCalCfg, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_SR_LANE_RECONSTRUCT_INFO_s* pLaneInfo);
UINT32 UtilVSL_DrawLaneCenterLineFunc(UINT8 *pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_CAL_EM_CALC_COORD_CFG_s* pCalCfg, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_SR_LANE_RECONSTRUCT_INFO_s* pLaneInfo);

UINT32 UtilVSL_DrawBEVLane(UINT8 *pBuffer, const UTIL_VSL_BEV_CFG_s* pBevCfg, const AMBA_SR_LANE_RECONSTRUCT_INFO_s* pLaneInfo);

UINT32 UtilVSL_DrawBbox3D(UINT32 Width, UINT32 Height, const REF_FLOW_FC_SR_IMAGE_3D_MODEL_s *pSrImage3D, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

UINT32 UtilVSL_DrawBboxMk(const RF_LOG_OD_2DBBOX_HEADER_s *pBbxHeader, const UTIL_VSL_MK_BBX_s *pMkIn, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

#endif // UTIL_VSL_COMMON_H