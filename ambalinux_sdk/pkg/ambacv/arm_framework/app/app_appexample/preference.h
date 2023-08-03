  /*
 * cam_util.h
 *
 * Copyright 2019 Ambarella Inc.
 *
 */

#ifndef _PERFERENCE_H
#define _PERFERENCE_H

#include "idsp_roi_msg.h"
#include "cvapi_flexidag.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CHNUM   (1)
#define MAX_FLOW_AMOUNT     (1)
#define MAX_FLEXIDAG_AMOUNT (2)

#define DAG_INPUT_TYPE_PIC         (0)
#define DAG_INPUT_TYPE_RAW         (1)
#define DAG_INPUT_TYPE_NUM         (2)

#define SRC_INPUT_TYPE_LIVEVIEW    (0)
#define SRC_INPUT_TYPE_PLAYBACK    (1)
#define SRC_INPUT_TYPE_FILE        (2)
#define SRC_INPUT_TYPE_NUM         (3)

#define DST_OUTPUT_TYPE_OSD        (0)
#define DST_OUTPUT_TYPE_FILE_DAG   (1)
#define DST_OUTPUT_TYPE_FILE_NMS   (2)
#define DST_OUTPUT_TYPE_NUM        (3)

#define OSD_TYPE_8_BIT             (8)
#define OSD_TYPE_32_BIT            (32)

#define OSD_AMOUNT_MAX             (4)

typedef struct {
    uint32_t Height;
    uint32_t Width;
    uint32_t Pitch;
} WINDOW_INFO_s;

typedef struct {
    char *FileName;
    uint32_t FrameId;
    flexidag_memblk_t Buffer;
} FILE_INPUT_INFO_s;

typedef struct {
    uint32_t OsdType;             /* according to OSD setting from RTOS, choose the proper OSD type (8:8 bits / 32: 32 bits)(you can ignore this if you do not choose osd output) */
    WINDOW_INFO_s OsdWin;         /* window of OSD frames */
    uint32_t OsdAmount;           /* amount of OSD buffer */
} OSD_INFO_s;

uint32_t Preference_GetSrcInputType(uint32_t Id, uint32_t *SrcInputType);
uint32_t Preference_SetSrcInputType(uint32_t Id, uint32_t SrcInputType);
uint32_t Preference_GetDstOutputType(uint32_t Id, uint32_t *DstOutputType);
uint32_t Preference_SetDstOutputType(uint32_t Id, uint32_t DstOutputType);
uint32_t Preference_GetDagInputType(uint32_t Id, uint32_t *DagInputType);
uint32_t Preference_SetDagInputType(uint32_t Id, uint32_t DagInputType);
uint32_t Preference_GetSrcChan(uint32_t Id, uint32_t *SrcChan);
uint32_t Preference_SetSrcChan(uint32_t Id, uint32_t SrcChan);
uint32_t Preference_GetFlexidagPath(uint32_t Id, char **FlexiDAGPath);
uint32_t Preference_SetFlexidagPath(uint32_t Id, char *FlexiDAGPath);
uint32_t Preference_GetFileInputInfo(uint32_t Id, FILE_INPUT_INFO_s *FileInputInfo);
uint32_t Preference_SetFileInputInfo(uint32_t Id, FILE_INPUT_INFO_s *FileInputInfo);
uint32_t Preference_GetInputInfo(uint32_t Id, WINDOW_INFO_s *InputInfo);
uint32_t Preference_SetInputInfo(uint32_t Id, WINDOW_INFO_s *InputInfo);
uint32_t Preference_GetOSDInfo(uint32_t Id, OSD_INFO_s *OsdInfo);
uint32_t Preference_SetOSDInfo(uint32_t Id, OSD_INFO_s *OsdInfo);
uint32_t Preference_GetDAGRoiCfg(uint32_t Id, amba_roi_config_t *RoiCfg);
uint32_t Preference_SetDAGRoiCfg(uint32_t Id, amba_roi_config_t *RoiCfg);



#ifdef __cplusplus
}
#endif

#endif	/* _PERFERENCE_H */
