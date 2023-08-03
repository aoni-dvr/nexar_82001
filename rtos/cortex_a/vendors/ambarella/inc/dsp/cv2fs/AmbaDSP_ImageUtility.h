/*
*  @file AmbaDSP_ImageUtility.h
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

#ifndef AMBA_IK_IMAGE_UTILITY_H
#define AMBA_IK_IMAGE_UTILITY_H

#include "AmbaTypes.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaKAL.h"
#include "AmbaErrorCode.h"

#ifndef SUPPORT_IK_SAFETY
#define SUPPORT_IK_SAFETY
#endif

// success
#ifndef IK_OK
#define IK_OK OK
#endif
// error 0xFFFF0
#ifndef IK_ERR_0000
#define IK_ERR_0000 (IK_ERR_BASE)                // General error
#endif
#ifndef IK_ERR_0001
#define IK_ERR_0001 (IK_ERR_0000 | 0x00000001UL) // IK Arch un-init error
#endif
#ifndef IK_ERR_0002
#define IK_ERR_0002 (IK_ERR_0000 | 0x00000002UL) // Specified ContextId un-init error
#endif
#ifndef IK_ERR_0003
#define IK_ERR_0003 (IK_ERR_0000 | 0x00000003UL) // Invalid ContextId error
#endif
#ifndef IK_ERR_0004
#define IK_ERR_0004 (IK_ERR_0000 | 0x00000004UL) // Invalid api for specified context ability error
#endif
#ifndef IK_ERR_0005
#define IK_ERR_0005 (IK_ERR_0000 | 0x00000005UL) // Null pointer error
#endif
#ifndef IK_ERR_0006
#define IK_ERR_0006 (IK_ERR_0000 | 0x00000006UL) // Invalid buffer alignment error. Size or address are not aligned to design document specified value
#endif
#ifndef IK_ERR_0007
#define IK_ERR_0007 (IK_ERR_0000 | 0x00000007UL) // Invalid buffer size too small error
#endif
#ifndef IK_ERR_0008
#define IK_ERR_0008 (IK_ERR_0000 | 0x00000008UL) // One or more parameters are out of valid range error
#endif
#ifndef IK_ERR_0009
#define IK_ERR_0009 (IK_ERR_0000 | 0x00000009UL) // Image kernel Default Binary un-init error
#endif

// error 0xFFFF1
#ifndef IK_ERR_0100
#define IK_ERR_0100 (IK_ERR_0000 | 0x00000100UL) // One or more mandatory filters are not set
#endif
#ifndef IK_ERR_0101
#define IK_ERR_0101 (IK_ERR_0000 | 0x00000101UL) // Invalid window geometry error
#endif
#ifndef IK_ERR_0102
#define IK_ERR_0102 (IK_ERR_0000 | 0x00000102UL) // Invalid CC table version
#endif
#ifndef IK_ERR_0103
#define IK_ERR_0103 (IK_ERR_0000 | 0x00000103UL) // Invalid AAA setting
#endif
#ifndef IK_ERR_0104
#define IK_ERR_0104 (IK_ERR_0000 | 0x00000104UL) // Wrong calculation on warp
#endif
#ifndef IK_ERR_0105
#define IK_ERR_0105 (IK_ERR_0000 | 0x00000105UL) // Wrong calculation on ca
#endif
#ifndef IK_ERR_0106
#define IK_ERR_0106 (IK_ERR_0000 | 0x00000106UL) // Wrong calculation on sbp
#endif
#ifndef IK_ERR_0107
#define IK_ERR_0107 (IK_ERR_0000 | 0x00000107UL) // Wrong calculation on vig
#endif
#ifndef IK_ERR_0108
#define IK_ERR_0108 (IK_ERR_0000 | 0x00000108UL) // Invalid anti-aliasing setting
#endif
#ifndef IK_ERR_0109
#define IK_ERR_0109 (IK_ERR_0000 | 0x00000109UL) // Invalid wb gain setting
#endif
#ifndef IK_ERR_010A
#define IK_ERR_010A (IK_ERR_0000 | 0x0000010AUL) // Invalid ce setting
#endif

// error 0xFFFF2
#ifndef IK_ERR_0200
#define IK_ERR_0200 (IK_ERR_0000 | 0x00000200UL) // General Safety error
#endif
#ifndef IK_ERR_0201
#define IK_ERR_0201 (IK_ERR_0000 | 0x00000201UL) // Safety error arch-init fail
#endif
#ifndef IK_ERR_0203
#define IK_ERR_0203 (IK_ERR_0000 | 0x00000203UL) // Safety error context memory fence error
#endif
#ifndef IK_ERR_0204
#define IK_ERR_0204 (IK_ERR_0000 | 0x00000204UL) // Safety error CR and flow_tbl memory fence error
#endif
#ifndef IK_ERR_0205
#define IK_ERR_0205 (IK_ERR_0000 | 0x00000205UL) // Safety error ring buffer error
#endif
#ifndef IK_ERR_0206
#define IK_ERR_0206 (IK_ERR_0000 | 0x00000206UL) // Safety error default binary error
#endif
#ifndef IK_ERR_0207
#define IK_ERR_0207 (IK_ERR_0000 | 0x00000207UL) // Safety error system API error
#endif


typedef struct {
    UINT32 ContextNumber;
    UINT32 Reserved;
    struct {
        UINT32 ConfigNumber;
        AMBA_IK_ABILITY_s *pAbility;
    } ConfigSetting[MAX_CONTEXT_NUM];
} AMBA_IK_CONTEXT_SETTING_s;


#define AMBA_IK_SAFETY_ERROR_MEM_FENCE      0UL
#define AMBA_IK_SAFETY_ERROR_RING_BUF       1UL
#define AMBA_IK_SAFETY_ERROR_CRC_MISMATCH   2UL
#define AMBA_IK_FORCE_SAFETY_ERROR          3UL
#define AMBA_IK_SAFETY_SYS_API_ERROR        4UL


#ifdef SUPPORT_IK_SAFETY

typedef struct {
    UINT32 UpdateInterval;
} AMBA_IK_SAFETY_INFO_s;


UINT32 AmbaIK_SetSafetyInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_SAFETY_INFO_s *pSafetyInfo);

#endif

/**
* Calculates the memory usage based on user's requirement
* @param [in]  pSettingOfEachContext, Context setting which user wants to assign in Image Kernel
* @param [in,out]  pTotalCtxSize, Returned IK required working buffer memory size
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_QueryArchMemorySize(const AMBA_IK_CONTEXT_SETTING_s *pSettingOfEachContext, const SIZE_t *pTotalCtxSize);

/**
* Specify the memory space for IK.
* @param [in]  pSettingOfEachContext, Context setting which user wants to assign in Image Kernel
* @param [in]  pMemAddr, Memory buffer address for Image Kernel working buffer
* @param [in]  MemSize, Memory buffer size for Image Kernel Working buffer
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitArch(const AMBA_IK_CONTEXT_SETTING_s *pSettingOfEachContext, void *pMemAddr, SIZE_t MemSize);

/**
* Specify the def binary address for IK.
* @param [in]  pBinDataAddr, Memory buffer address for def binary
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitDefBinary(void *pBinDataAddr);

/**
* Initialize the image kernel print debug log function.
* @param [in]  void
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitPrintFunc(void);

/**
* Initialize the image kernel filter parameter check function
* @param [in]  void
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitCheckParamFunc(void);

/**
* Initialize ability of the specified context
* @param [in]  pMode, Set ContextId
* @param [in]  pAbility, Set ability of this context.
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitContext(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_ABILITY_s *pAbility);

/**
* Get the ability of context ID
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAbility, Get ability of this context.
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetContextAbility(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_ABILITY_s *pAbility);

/**
* To trigger image kernel to compose user assigned image quality parameters of context ID to configuration
* @param [in]  pMode, Set ContextId
* @param [in,out]  pExecuteContainer, Returned composed config stored in container
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_ExecuteConfig(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_EXECUTE_CONTAINER_s *pExecuteContainer);

/**
* Calculates the memory usage based for debug config dump
* @param [in,out]  pSize, Returned required memory size for debug config dump
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_QueryDebugConfigSize(const SIZE_t *pSize);

/**
* Dump debug config of specified config
* @param [in]  pMode, Set ContextId
* @param [in]  PreviousCount, Previous count of composed config
* @param [in,out]  pDumpConfigBuffer, Memory buffer pointer prepared by USER to store debug config dump
* @param [in]  Size, the size for debug config dump
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_DumpDebugConfig(const AMBA_IK_MODE_CFG_s *pMode, UINT32 PreviousCount, const void *pDumpConfigBuffer, SIZE_t Size);

/**
*  Get a theoretical clock value under single channel case
* @param [in]  pQueryIdspClockInfo, Query input information
* @param [in,out]  pQueryIdspClockRst, Queried output result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_QueryIdspClock(const AMBA_IK_QUERY_IDSP_CLOCK_INFO_s *pQueryIdspClockInfo, const AMBA_IK_QUERY_IDSP_CLOCK_RST_s *pQueryIdspClockRst);

/**
*  Query the calculate geometry settings required buffer sizes
* @param [in,out]  pBufSizeInfo, returned required buffer size
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_QueryCalcGeoBufSize(const AMBA_IK_QUERY_CALC_GEO_BUF_SIZE_s *pBufSizeInfo);

/**
*  Calculate the geometry settings
* @param [in]  pAbility, Set ability of this context.
* @param [in,out]  pInfo, Calc input information and working buffer and working buffer size
* @param [in,out]  pGeoRst, output buffer and output buffer size
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_CalcGeoSettings(const AMBA_IK_ABILITY_s *pAbility, const AMBA_IK_CALC_GEO_SETTINGS_s *pInfo, const AMBA_IK_CALC_GEO_RESULT_s *pGeoRst);

/**
*  Query the warp resources
* @param [in,out]  pQueryParams, Query warp working resources
* @param [in,out]  pQueryResult, Query warp result resources
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_CalcWarpResources(const AMBA_IK_QUERY_WARP_s *pQueryParams, AMBA_IK_QUERY_WARP_RESULT_s *pQueryResult);

/**
*  Warp table pre-process
* @param [in]  pWarpPreProcIn, Calc input information and working buffer
* @param [in,out]  pWarpPreProcOut, output buffer
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_PreProcWarpTbl(const AMBA_IK_IN_WARP_PRE_PROC_s *pWarpPreProcIn, AMBA_IK_OUT_WARP_PRE_PROC_s *pWarpPreProcOut);

/**
*  CAWarp table pre-process
* @param [in]  pCaPreProcIn, Calc input information and working buffer
* @param [in,out]  pCaPreProcOut, output buffer
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_PreProcCAWarpTbl(const AMBA_IK_IN_CAWARP_PRE_PROC_s *pCaPreProcIn, AMBA_IK_OUT_CAWARP_PRE_PROC_s *pCaPreProcOut);

/**
* Query specified frame information with IkId
* @param [in]  pMode, Set ContextId
* @param [in]  IkId, IK config ring ID
* @param [in,out]  pFrameInfo, FrameInfo buffer pointer prepared by USER to store FrameInfo dump
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_QueryFrameInfo(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 IkId, AMBA_IK_QUERY_FRAME_INFO_s *pFrameInfo);

/**
* Inject selection safety error in purpose
* @param [in]  pMode, Set ContextId
* @param [in]  ErrSelect, Select error type
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InjectSaftyError(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 ErrSelect);

#endif
