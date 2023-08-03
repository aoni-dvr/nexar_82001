/*
 * dag_impl.h
 *
 * Copyright 2019 Ambarella Inc.
 *
 */

#ifndef _DAG_IMPL_H
#define _DAG_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif


#include "cvapi_memio_interface.h"

uint32_t DagFlow_Init(uint32_t id, char *fl_path);
uint32_t DagFlow_Release(uint32_t id);
uint32_t DagFlow_Process(uint32_t id, memio_source_recv_picinfo_t *in, AMBA_CV_FLEXIDAG_IO_s **result);

#ifdef __cplusplus
}
#endif

#endif	/* _DAG_IMPL_H */
