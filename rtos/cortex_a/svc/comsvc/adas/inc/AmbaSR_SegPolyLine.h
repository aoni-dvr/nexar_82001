/**
 *  @file AmbaSR_SegPolyLine.h
 *
 * Copyright (c) 2021 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details header file of SR poly line
 *
 */

#ifndef AMBA_SR_SEGPOLYLINE_H
#define AMBA_SR_SEGPOLYLINE_H

#include "AmbaTypes.h"
#include "AmbaAdasErrorCode.h"

typedef struct {
    UINT32 SegWidth;      /**< in: segmentation buffer width */
    UINT32 SegHeight;     /**< in: segmentation buffer height */
    UINT32 WrkBufSize;    /**< out: required working buffer size */
} AMBA_SR_POLYLINE_QUERY_CFG_s;

typedef struct {
    UINT32 SegWidth;      /**< in: segmentation buffer width */
    UINT32 SegHeight;     /**< in: segmentation buffer height */
    UINT8* pWrkBuf;       /**< in: working buffer address */
    UINT32 WrkBufSize;    /**< in: working buffer size */
} AMBA_SR_POLYLINE_INIT_CFG_s;


typedef struct {
    UINT32 X;    /**< Point X */
    UINT32 Y;    /**< Point Y */
} AMBA_SR_POLYLINE_POINT_DATA_s;

typedef struct {
    UINT32 Id;                                   /**< Line ID */
    UINT32 PointNum;                             /**< Number of points */
    AMBA_SR_POLYLINE_POINT_DATA_s* pPointArr;    /**< Start pointer of point data array */
} AMBA_SR_POLYLINE_LINE_DATA_s;

typedef struct {
    UINT32 LineNum;                            /**< Number of lines */
    AMBA_SR_POLYLINE_LINE_DATA_s* pLineArr;    /**< Start pointer of line data array */
} AMBA_SR_POLYLINE_PROC_OUT_s;

typedef struct {
    UINT8* pBuf;                         /**< Buffer address */
    UINT32 Pitch;                        /**< Buffer pitch which is the width to change to next image row */
    UINT32 Width;                        /**< Image width */
    UINT32 Height;                       /**< Image height */
} AMBA_SR_POLYLINE_BUF_INFO_s;

typedef struct {
    AMBA_SR_POLYLINE_BUF_INFO_s SegBufInfo;    /**< in: seg buffer infomation */
    AMBA_SR_POLYLINE_PROC_OUT_s Result;           /**< out: process result */
} AMBA_SR_POLYLINE_PROCESS_CFG_s;


UINT32 AmbaSR_SegPolyLine_Query(AMBA_SR_POLYLINE_QUERY_CFG_s *pCfg);

UINT32 AmbaSR_SegPolyLine_Init(void** pHdlr, const AMBA_SR_POLYLINE_INIT_CFG_s* pCfg);

UINT32 AmbaSR_SegPolyLine_Process(const void* pHdlr, AMBA_SR_POLYLINE_PROCESS_CFG_s* pCfg);

UINT32 AmbaSR_SegPolyLine_DeInit(const void* pHdlr);

#endif