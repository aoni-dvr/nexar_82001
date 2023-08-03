#ifndef _NMS_IMPL_H
#define _NMS_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/***
 * For C implementation, user should use the definition in "cvapi_amba_od_interface.h".
 ***/
//#include "cvapi_amba_od_interface.h"

/***
 * For C++ implementation, due to some element name is same as key word,
 * it has to use local definition.
 * However, the size and order should keep the same.
 ***/

#include "ambint.h"
/* AmbaOD output data structure */
typedef struct {
            uint32_t type;                  /* AMBA_OD_OUTPUT_TYPE                */
            uint32_t length;                /* sizeof(amba_od_out_t)              */
            uint32_t num_objects;           /* size of object list                */
            rptr32_t objects_offset;        /* relative pointer to object list    */
            uint32_t num_proposals;         /* size of proposal list              */
            rptr32_t proposals_offset;      /* relative pointer to proposal list  */
            uint32_t frame_num;             /* frame_num of the current frame     */
            uint32_t capture_time;          /* capture time of the frame          */
            uint32_t errorcode;             /* Error code issued by the task.     */
            ptr32_t  class_name;            /* phy addr of class-name list        */
            uint16_t camera_id;             /* camera ID                          */
#define AMBAOD_TYPE_GENERAL     0
#define AMBAOD_TYPE_BARRIER     1
#define AMBAOD_TYPE_POLE        2
            uint16_t network_type;          /* network type                       */
            uint8_t  major_version[32];     /* version string of ambaod           */
            uint8_t  minor_version[32];     /* version string of classifier       */
            ptr64_t  class_name_p64;        /* class-name addr, for RTOS only     */
            uint8_t  padding[128-116];
} amba_od_out_t;

typedef struct {
        uint32_t score;                 /* confidence score                   */
        uint32_t field;                 /* field ID, assigned by step1        */
        uint32_t cls;                 /* class ID, assigned by step4        */
        uint32_t track;                 /* track ID, assigned by tracker      */
        int32_t  bb_start_row;
        int32_t  bb_start_col;
        uint32_t bb_height_m1;
        uint32_t bb_width_m1;
} amba_od_candidate_t;

#define MAX_VIS_NUM         (150U)
#define NUM_CLS             (7U)

uint32_t NMS_LoadPriorBox(char *filename, void **hndlr);
uint32_t NMS_Process(uint32_t ch, void *hndlr, float *pLoc, float *pConf, amba_od_candidate_t *pODOutput, uint32_t *BoxAmount);
uint32_t NMS_Release(void *hndlr);

#ifdef __cplusplus
}
#endif

#endif	/* _NMS_IMPL_H */
