#ifndef AMBA_SR_OBJ_PRIORITY_H
#define AMBA_SR_OBJ_PRIORITY_H

#include "AmbaTypes.h"
#include "AmbaSurround.h"


#define AMBA_SR_OP_MODE_FRONT          (1U)
#define AMBA_SR_OP_MODE_EM_LEFT        (2U)
#define AMBA_SR_OP_MODE_EM_RIGHT       (3U)
#define AMBA_SR_OP_MODE_EM_REAR        (4U)



typedef struct {
    UINT32 Mode;                /**< Refer to AMBA_SR_OP_MODE_XXXXXX */
    AMBA_SR_ROI_s ODRoi;        /**< OD Roi */
} AMBA_SR_OP_INIT_CFG_s;

typedef struct {
    AMBA_OD_2DBBX_LIST_s *BbxList;       /**< Input 2D bbox list */
    AMBA_SR_SROBJECT_DATA_s *SRData;     /**< Input SR object data */
} AMBA_SR_OP_PROC_IN_s;

typedef struct {
    UINT32 Id;           /**< Tracker ID */
    UINT32 ROITouch;     /**< Byte value 0 for none-touch, 1 for touched; byte 0 is top, byte 1 is bottom, byte 2 is left, byte 3 is right */
    DOUBLE Score;        /**< The larger the value, the more important; valid value: 1.0~1000.0, invalid value: 0.0 */
} AMBA_SR_OP_DATA_s;

typedef struct {
    UINT32 ObjNum;       /**< Object number */
    AMBA_SR_OP_DATA_s ObjPriority[AMBA_SR_MAX_OBJECT_NUM];
} AMBA_SR_OP_PROC_OUT_s;



UINT32 AmbaSR_OP_Init(const AMBA_SR_OP_INIT_CFG_s* pCfg, void** pOutHdlr);

UINT32 AmbaSR_OP_Process(const void* pInHdlr, const AMBA_SR_OP_PROC_IN_s* pIn, AMBA_SR_OP_PROC_OUT_s* pOut);

#endif
