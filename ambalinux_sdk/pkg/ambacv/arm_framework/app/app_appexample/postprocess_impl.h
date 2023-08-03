/*
 * postprocess_impl.h
 *
 * Copyright 2019 Ambarella Inc.
 *
 */

#ifndef _POSTPROCESS_IMPL_H
#define _POSTPROCESS_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"


uint32_t PostProcess_Init(uint32_t id, char *fl_path);
uint32_t PostProcess_Release(uint32_t id);
uint32_t PostProcess_Impl(uint32_t id, memio_source_recv_picinfo_t *in, AMBA_CV_FLEXIDAG_IO_s *result);

#ifdef __cplusplus
}
#endif

#endif /* _POSTPROCESS_IMPL_H */

