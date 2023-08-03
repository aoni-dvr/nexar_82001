/*
*  @file AmbaDSP_ImageFilterInternal.h
*
* Copyright (c) 2020 Ambarella International LP
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
*/


#ifndef AMBA_IK_IMAGE_FILTER_INTERNAL_H
#define AMBA_IK_IMAGE_FILTER_INTERNAL_H

#include "AmbaDSP_ImageFilter.h"

typedef struct {
    UINT32   Enable;
    UINT32   GridArrayWidth;
    UINT32   GridArrayHeight;
    UINT32   HorzGridSpacingExponent;
    UINT32   VertGridSpacingExponent;
    INT16   *PwarpHorizontalTable;
} AMBA_IK_WARP_2ND_INTERNA_INFO_s;

typedef struct {
    // Warp part
    UINT32   Enable;
    UINT32   GridArrayWidth;
    UINT32   GridArrayHeight;
    UINT32   HorzGridSpacingExponent;
    UINT32   VertGridSpacingExponent;
    UINT32   VertWarpGridArrayWidth;
    UINT32   VertWarpGridArrayHeight;
    UINT32   VertWarpHorzGridSpacingExponent;
    UINT32   VertWarpVertGridSpacingExponent;
    INT16   *pWarpHorizontalTable;
    INT16   *pWarpVerticalTable;
} AMBA_IK_WARP_INTERNAL_INFO_s;

typedef struct {
    UINT32  HorzWarpEnable;
    UINT32  VertWarpEnable;
    UINT32   HorzPassGridArrayWidth;
    UINT32   HorzPassGridArrayHeight;
    UINT32   HorzPassHorzGridSpacingExponent;
    UINT32   HorzPassVertGridSpacingExponent;
    UINT32   VertPassGridArrayWidth;
    UINT32   VertPassGridArrayHeight;
    UINT32   VertPassHorzGridSpacingExponent;
    UINT32   VertPassVertGridSpacingExponent;
    INT16   WarpHorzTableRed[1536];
    INT16   WarpVertTableRed[1536];
    INT16   WarpHorzTableBlue[1536];
    INT16   WarpVertTableBlue[1536];
} AMBA_IK_CAWARP_INTERNAL_INFO_s;

typedef struct {
    UINT32 Enable;
    UINT32 PixelMapWidth;
    UINT32 PixelMapHeight;
    UINT32 PixelMapPitch;
    UINT8 *pMap;
} AMBA_IK_SBP_INTERNAL_INFO_s;

UINT32 AmbaIK_SetWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WARP_INTERNAL_INFO_s *pWarpInternal);
UINT32 AmbaIK_GetWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WARP_INTERNAL_INFO_s *pWarpInternal);
UINT32 AmbaIK_SetCaWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CAWARP_INTERNAL_INFO_s *pCawarpInternal);
UINT32 AmbaIK_GetCaWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CAWARP_INTERNAL_INFO_s *pCawarpInternal);
UINT32 AmbaIK_SetStaticBpcInternal(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_SBP_INTERNAL_INFO_s *pSbpInternal);
UINT32 AmbaIK_GetStaticBpcInternal(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_SBP_INTERNAL_INFO_s *pSbpInternal);

/**
* Transfer CFA statistic from hw
* @param [in]  In, 3a statistic data
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32  AmbaIK_TransferCFAAaaStatData(const void *In);
/**
* Transfer PG statistic from hw
* @param [in]  In, 3a statistic data
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32  AmbaIK_TransferPGAaaStatData(const void *In);
/**
* Transfer CFA histogram statistic from hw
* @param [in]  In, 3a statistic data
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32  AmbaIK_TransCFAHistStatData(const void *In);

/**
* Dump debug data
* @param [in]  pMode, Set ContextId
* @param [in]  DumpFolder, Set dump path
* @param [in]  DumpLevel, Set dump level
* @param [in]  pBuffer, Set dump bufer
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32 AmbaIK_DebugDump(const AMBA_IK_MODE_CFG_s *pMode, char *DumpFolder, INT32 DumpLevel, UINT8  *pBuffer);

#endif
