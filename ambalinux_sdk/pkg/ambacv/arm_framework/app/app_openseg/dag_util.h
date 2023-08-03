/*
 * dag_util.h
 *
 * Copyright 2019 Ambarella Inc.
 *
 */

#ifndef _DAG_UTIL_H
#define _DAG_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cvapi_ambacv_flexidag.h"

typedef struct {
    AMBA_CV_FLEXIDAG_HANDLE_s  handle;
    AMBA_CV_FLEXIDAG_INIT_s    init;
    flexidag_memblk_t          bin_buf;
    AMBA_CV_FLEXIDAG_IO_s      in;
    AMBA_CV_FLEXIDAG_IO_s      out;
} FLEXIDAG_INSTANCE_s;

uint32_t DagUtil_CreateDag(char *path, FLEXIDAG_INSTANCE_s *fd_inst);
uint32_t DagUtil_InitDag(FLEXIDAG_INSTANCE_s *fd_inst, flexidag_memblk_t *temp_buf);
uint32_t DagUtil_ReleaseDag(FLEXIDAG_INSTANCE_s *fd_inst);

#ifdef __cplusplus
}
#endif

#endif	/* _DAG_UTIL_H */
