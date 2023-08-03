#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ambint.h"
#include "cvapi_memio_interface.h"
#include "cvapi_flexidag.h"

#include "preference.h"

typedef struct {
    uint32_t SrcInputType;        /* source of input frames (0:Liveview / 1:file) */
    uint32_t DstOutputType;       /* output type to show cv result (0:OSD on HDMI / 1:file for DAG output result / 2:file for DMS result) */
    uint32_t DagInputType;        /* input type of DAG (0:picinfo type / 1:raw buffer type) */
    uint32_t SrcChan;             /* Source channel from RTOS */
    char *FlexiDAGPath;           /* path of FlexiDAG bin or others necessary files */
    /* Input size */
    WINDOW_INFO_s InputInfo;      /* window of input frames */
    /* File info */
    FILE_INPUT_INFO_s FileInput;  /* information for file input(you can ignore this if you do not choose file input) */
    /* Output - OSD */
    OSD_INFO_s OsdInfo;           /* OSD information */
    /* DAG ROI config */
    amba_roi_config_t RoiCfg;     /* DAG ROI config */
} USER_PREFERENCE_s;

static USER_PREFERENCE_s UserPerference[] = {
{       /* FLOW 0 */
        .SrcInputType  = SRC_INPUT_TYPE_LIVEVIEW,
        .DstOutputType = DST_OUTPUT_TYPE_OSD,
        .DagInputType = DAG_INPUT_TYPE_PIC,
        .SrcChan = 0U,
        .FlexiDAGPath = "./flexibin",
        /* Input size */
        .InputInfo.Height = 764U,
        .InputInfo.Width = 1360U,
        .InputInfo.Pitch = 1408U,
        /* File info */
        .FileInput.FileName = NULL,
        .FileInput.FrameId = 0,
        /* Output - OSD */
        .OsdInfo.OsdType = OSD_TYPE_32_BIT,
        .OsdInfo.OsdWin.Width = 960U,
        .OsdInfo.OsdWin.Height = 540U,
        .OsdInfo.OsdAmount = OSD_AMOUNT_MAX,
        /* DAG roi */
        .RoiCfg.msg_type = AMBA_ROI_CONFIG_MSG,
        .RoiCfg.image_pyramid_index = 1U,
        .RoiCfg.source_vin = 0U,
        .RoiCfg.roi_start_col = 9999U,
        .RoiCfg.roi_start_row = 9999U,
        .RoiCfg.roi_width = 300U,
        .RoiCfg.roi_height = 300U,
    },
};

uint32_t Preference_GetSrcInputType(uint32_t Id, uint32_t *SrcInputType)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (SrcInputType != NULL) {
            *SrcInputType = UserPerference[Id].SrcInputType;
        } else {
            printf("[Preference_GetSrcInputType] SrcInputType is NULL! \n");
            Rval = 1;
        }
    } else {
        printf("[Preference_GetSrcInputType] Invalid Flow Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_SetSrcInputType(uint32_t Id, uint32_t SrcInputType)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (SrcInputType < SRC_INPUT_TYPE_NUM) {
            UserPerference[Id].SrcInputType = SrcInputType;
        } else {
            printf("[Preference_SetSrcInputType] Invalid SrcInputType (%u)! \n", SrcInputType);
            Rval = 1;
        }
    } else {
        printf("[Preference_SetSrcInputType] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_GetDstOutputType(uint32_t Id, uint32_t *DstOutputType)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (DstOutputType != NULL) {
            *DstOutputType = UserPerference[Id].DstOutputType;
        } else {
            printf("[Preference_GetDstOutputType] DstOutputType is NULL! \n");
            Rval = 1;
        }
    } else {
        printf("[Preference_GetDstOutputType] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_SetDstOutputType(uint32_t Id, uint32_t DstOutputType)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (DstOutputType < DST_OUTPUT_TYPE_NUM) {
            UserPerference[Id].DstOutputType = DstOutputType;
        } else {
            printf("[Preference_SetDstOutputType] Invalid DstOutputType (%u)! \n", DstOutputType);
            Rval = 1;
        }
    } else {
        printf("[Preference_SetDstOutputType] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_GetDagInputType(uint32_t Id, uint32_t *DagInputType)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (DagInputType != NULL) {
            *DagInputType = UserPerference[Id].DagInputType;
        } else {
            printf("[Preference_GetDagInputType] DagInputType is NULL! \n");
            Rval = 1;
        }
    } else {
        printf("[Preference_GetDagInputType] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_SetDagInputType(uint32_t Id, uint32_t DagInputType)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (DagInputType < DAG_INPUT_TYPE_NUM) {
            UserPerference[Id].DagInputType = DagInputType;
        } else {
            printf("[Preference_SetDagInputType] Invalid DagInputType (%u)! \n", DagInputType);
            Rval = 1;
        }
    } else {
        printf("[Preference_SetDagInputType] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_GetSrcChan(uint32_t Id, uint32_t *SrcChan)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (SrcChan != NULL) {
            *SrcChan = UserPerference[Id].SrcChan;
        } else {
            printf("[Preference_GetSrcChan] SrcChan is NULL! \n");
            Rval = 1;
        }
    } else {
        printf("[Preference_GetSrcChan] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_SetSrcChan(uint32_t Id, uint32_t SrcChan)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (SrcChan < CHNUM) {
            UserPerference[Id].SrcChan = SrcChan;
        } else {
            printf("[Preference_SetSrcChan] Invalid SrcChan (%u)! \n", SrcChan);
            Rval = 1;
        }
    } else {
        printf("[Preference_SetSrcChan] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_GetFlexidagPath(uint32_t Id, char **FlexiDAGPath)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        *FlexiDAGPath = UserPerference[Id].FlexiDAGPath;
    } else {
        printf("[Preference_GetFlexidagPath] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_SetFlexidagPath(uint32_t Id, char *FlexiDAGPath)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (FlexiDAGPath != NULL) {
            UserPerference[Id].FlexiDAGPath = FlexiDAGPath;
        } else {
            printf("[Preference_SetFlexidagPath] FlexidagPath is NULL! \n");
            Rval = 1;
        }
    } else {
        printf("[Preference_SetFlexidagPath] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_GetFileInputInfo(uint32_t Id, FILE_INPUT_INFO_s *FileInput)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (FileInput != NULL) {
            memcpy(FileInput, &UserPerference[Id].FileInput, sizeof(FILE_INPUT_INFO_s));
        } else {
            printf("[Preference_GetFileInputInfo] FileInput is NULL! \n");
            Rval = 1;
        }
    } else {
        printf("[Preference_GetFileInputInfo] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_SetFileInputInfo(uint32_t Id, FILE_INPUT_INFO_s *FileInput)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (FileInput != NULL) {
            memcpy(&UserPerference[Id].FileInput, FileInput, sizeof(FILE_INPUT_INFO_s));
        } else {
            printf("[Preference_SetFileInputInfo] FileInput is NULL! \n");
            Rval = 1;
        }
    } else {
        printf("[Preference_SetFileInputInfo] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_GetInputInfo(uint32_t Id, WINDOW_INFO_s *InputInfo)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (InputInfo != NULL) {
            memcpy(InputInfo, &UserPerference[Id].InputInfo, sizeof(WINDOW_INFO_s));
        } else {
            printf("[Preference_GetInputInfo] InputInfo is NULL! \n");
            Rval = 1;
        }
    } else {
        printf("[Preference_GetInputInfo] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_SetInputInfo(uint32_t Id, WINDOW_INFO_s *InputInfo)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (InputInfo != NULL) {
            memcpy(&UserPerference[Id].InputInfo, InputInfo, sizeof(WINDOW_INFO_s));
        } else {
            printf("[Preference_SetInputInfo] InputInfo is NULL! \n");
            Rval = 1;
        }
    } else {
        printf("[Preference_SetInputInfo] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_GetOSDInfo(uint32_t Id, OSD_INFO_s *OsdInfo)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (OsdInfo != NULL) {
            memcpy(OsdInfo, &UserPerference[Id].OsdInfo, sizeof(OSD_INFO_s));
        } else {
            printf("[Preference_GetOSDType] RoiCfg is NULL! \n");
            Rval = 1;
        }
    } else {
        printf("[Preference_GetOSDInfo] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_SetOSDInfo(uint32_t Id, OSD_INFO_s *OsdInfo)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (OsdInfo != NULL) {
            memcpy(&UserPerference[Id].OsdInfo, OsdInfo, sizeof(OSD_INFO_s));
        } else {
            printf("[Preference_SetOSDType] RoiCfg is NULL! \n");
            Rval = 1;
        }
    } else {
        printf("[Preference_SetOSDInfo] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_GetDAGRoiCfg(uint32_t Id, amba_roi_config_t *RoiCfg)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (RoiCfg != NULL) {
            memcpy(RoiCfg, &UserPerference[Id].RoiCfg, sizeof(amba_roi_config_t));
        } else {
            printf("[Preference_GetDAGRoiCfg] RoiCfg is NULL! \n");
            Rval = 1;
        }
    } else {
        printf("[Preference_GetDAGRoiCfg] Invalid Id %d \n", Id);
    }
    return Rval;
}

uint32_t Preference_SetDAGRoiCfg(uint32_t Id, amba_roi_config_t *RoiCfg)
{
    uint32_t Rval = 0;
    if (Id < MAX_FLOW_AMOUNT) {
        if (RoiCfg != NULL) {
            memcpy(&UserPerference[Id].RoiCfg, RoiCfg, sizeof(amba_roi_config_t));
        } else {
            printf("[Preference_SetDAGRoiCfg] RoiCfg is NULL! \n");
            Rval = 1;
        }
    } else {
        printf("[Preference_SetDAGRoiCfg] Invalid Id %d \n", Id);
    }
    return Rval;
}

