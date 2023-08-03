/*
 * cam_util.h
 *
 * Copyright 2019 Ambarella Inc.
 *
 */

#ifndef _CAM_UTIL_H
#define _CAM_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"

uint32_t CamUtil_Init(uint32_t id);
uint32_t CamUtil_Release(uint32_t id);
uint32_t CamUtil_GetFrame(uint32_t id, memio_source_recv_picinfo_t *pInfo);
uint32_t CamUtil_SendOSD(uint32_t id, memio_sink_send_out_t *out);

#ifdef __cplusplus
}
#endif

#endif	/* _CAM_UTIL_H */
