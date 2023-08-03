/******************************************************************************/
/*  Copyright 2017 Ambarella Inc.                                             */
/*  Author: Alfred Lee                                                        */
/*  Email:  cilee@ambarella.com                                               */
/******************************************************************************/
#ifndef CVAPI_TRACKER_INTERFACE_H_FILE
#define CVAPI_TRACKER_INTERFACE_H_FILE

#include <cvapi_common.h>

#define MAX_TRACKING_OBJS 256
enum {
    CTYPE_PEDESTRIAN = 0,
    CTYPE_CAR = 1,
    CTYPE_CYCLIST = 3,
};

typedef struct {
    coord_t lt;     /* coordinate of top-left of bounding box*/
    coord_t rd;     /* coordinate of bottom-right of bounding box*/

    uint32_t id;    /* tracking obj id */
    uint32_t ctype; /* classified type */
    uint32_t stfr;  /* start frame number */
    float score;    /* weighted detection score*/

} track_obj_t;

typedef struct {
    uint32_t num_of_obj;
    track_obj_t track_list[MAX_TRACKING_OBJS];

} tracker_output_t;


#endif
