/*  calibration data for multiple cameras  */
#ifndef CALINFO_H
#define CALINFO_H

#include <cvapi_idsp_interface.h>

#ifndef MAX_CAMERA_VIEW
    #define MAX_CAMERA_VIEW   13
#else
    #warning "Multiple definition of MAX_CAMERA_VIEW"
#endif



typedef enum CAMERA_TYPES
{
    UNDEFINED = 0,
    PINHOLE_CAMERA,
    SPHERICAL_CAMERA
} CAMERA_TYPES;

typedef enum ROTATION_TYPES
{
    QUATERNION,
    AXIS_ANGLE,
    TAIT_BRYAN
} ROTATION_TYPES;


/**
 *  spherical_camera_t
 *  @brief The structure contains a spherical camera calibration.
 */
typedef struct spherical_camera_t {
    uint32_t id;                                    /* input number into arm vo task. */
    uint32_t width;                                 /* image width */
    uint32_t height;                                /* image height */
    float idu;                                      /* horizontal inv angle factor [px/rad] */
    float idv;                                      /* vertical inv angle factor [px/rad] */
    float u0;                                       /* x coord of principal point [px] */
    float v0;                                       /* y coord of principal point [px] */
    float baseline;                                 /* baseline for stereo cameras, 0.0f for monocular [m] */
    float camera_to_body_orientation[4];            /* a quaternion representing the camera orientation expressed in the body reference system */
    float camera_to_body_position[3];               /* a vector representing the camera position expressed in the body reference system [m] */   
} spherical_camera_t;
#define AC_CVAPI_SPHERICAL_CAMERA_T_VERSION    (0U)            // version of this data type: N.B. update in case the data changes!!


/**
 *  pinhole_camera_t
 *  @brief The structure contains a pinhole camera calibration.
 */
typedef struct pinhole_camera_t {
    uint32_t id;                                    /* input number into arm vo task. */
    uint32_t width;                                 /* image width */
    uint32_t height;                                /* image height */
    float ku;                                       /* horizontal scale factor [px] */
    float kv;                                       /* vertical scale factor [px] */
    float u0;                                       /* x coord of principal point [px] */
    float v0;                                       /* y coord of principal point [px] */
    float baseline;                                 /* baseline for stereo views, 0.0f for monocular [m] */
    float camera_to_body_orientation[4];            /* a quaternion representing the camera orientation expressed in the body reference system */
    float camera_to_body_position[3];               /* a vector representing the camera position expressed in the body reference system [m] */
} pinhole_camera_t;
#define AC_CVAPI_PINHOLE_CAMERA_T_VERSION    (0U)            // version of this data type: N.B. update in case the data changes!!


/**
 *  camera_t
 *  @brief The union can contains every camera_type calibration.
 */
typedef union camera_t {
    pinhole_camera_t   pinhole;
    spherical_camera_t spherical;
} camera_t;


/**
 *  camera_octaves_t
 *  @brief The structure contains the main camera information, like the camera_type and the calibration for each octave.
 */
typedef struct camera_octaves_t {
    uint32_t camera_type;                           /* enum CAMERA_TYPES: it defines the camera type */
    camera_t camera_intr;                           /* Source image intrinsic params */
    uint32_t num_half_octaves;                      /* Number of half_octave to consider */
    uint32_t enabled[MAX_HALF_OCTAVES];             /* specify if the corresponding half_octave is enabled */
    camera_t idsp_intr[MAX_HALF_OCTAVES];           /* half_octave ROI intrinsic: they are referred to the image obtained by applying the ROI */
} camera_octaves_t;


/**
 *  views_calibration_t
 *  @brief The structure contains main camera information for each different views.
 */
typedef struct views_calibration_t {
    uint32_t         num_views;                     /* number of views */
    camera_octaves_t views[MAX_CAMERA_VIEW];        /* calibration data per view */
} views_calibration_t;


/**
 *  camera_calibration_info_t
 *  @brief The structure contains the cameras calibrations loaded from the cvtable.
 */
typedef struct camera_calibration_info_t {
    uint32_t            pinhole_cameras_num;        /* number of pihhole views */
    uint32_t            spherical_cameras_num;      /* number of spherical views */
    pinhole_camera_t    pinhole_views[MAX_CAMERA_VIEW];     /* pinhole cameras calibration data */
    spherical_camera_t  spherical_views[MAX_CAMERA_VIEW];   /* spherical cameras calibration data  */
} camera_calibration_info_t;
#define AC_CVAPI_CAMERA_CALIB_T_VERSION    (0U + \
                                            AC_CVAPI_PINHOLE_CAMERA_T_VERSION + \
                                            AC_CVAPI_SPHERICAL_CAMERA_T_VERSION)
#endif
