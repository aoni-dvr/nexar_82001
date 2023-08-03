/**
 *  @file cvapi_flexidag_unitest.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for AmbaCV Flexidag APIs
 *
 */

#ifndef CVAPI_FLEXIDAG_UNITEST_H
#define CVAPI_FLEXIDAG_UNITEST_H
#include "AmbaTypes.h"
#include "AmbaShell.h"
#include "cvapi_flexidag.h"

void RefCV_UtCommand(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void RefCV_TestHelp(AMBA_SHELL_PRINT_f PrintFunc);
void RefCV_MnetSSD(UINT32 thread_id, UINT32 num_of_frame, UINT32 blocking_run, UINT8 freq, UINT32 mode);
void RefCV_Bisenet(UINT32 thread_id, UINT32 num_of_frame, UINT32 blocking_run, UINT8 freq);
void RefCV_Restart(UINT32 loop);
void RefCV_AccUT(UINT32 params, UINT32 loop);
void RefCV_AccCustomUT(const char *RefCV_custom_bin_path, const char *RefCV_input_path, const char *RefCV_output_path, uint32_t RefCV_input_count, uint32_t RefCV_output_count, uint32_t RefCV_output_offset, UINT32 loop);
void RefCV_AmbaFCI_UT(UINT32 opt);
void SCA_OpenOD_UT(const char* BinPath, const char* LogPath, const char* InFile, const char* OutputFn, UINT32 Opt);
void SCA_OpenSeg_UT(const char* BinPath, const char* LogPath, const char* InFile, const char* OutputFn, UINT32 Opt);
void RefCV_Test(UINT32 thread_id, UINT32 num_of_frame, UINT32 blocking_run);
void RefCV_Net(const char* path);
void SCA_AmbaPcpt_UT(char * const * pArgVector);
void SCA_AmbaOD_UT(const char* BinPath, const char* LogPath, const char* InFile, const char* OutputFn, const char* LicenseFn, UINT32 Opt);
void SCA_AmbaSeg_UT(const char* BinPath, const char* LogPath, const char* InFile, const char* OutputFn, const char* LicenseFn, UINT32 Opt);
#ifdef CONFIG_BUILD_AMBA_ADAS_UNITTEST
void SCA_ADAS_FC_UT(const char* pSegLoggerFile, const char* pODLoggerFile, const char* p3DLoggerFile, const char* pMKLoggerFile, const char* pCanbusFile,
                    const char* pCalibFile, DOUBLE FocalLength, UINT32 ProcStep, const char* pOutFile);
#endif
void RefCV_AmbaHarrisCornerDet_UT(const char* BinPath, const char* LogPath, const char* InFile,
                                  const char* OutMaxHarrisResponseFn, const char* OutHarrisResponseFn, const char* OutHarrisBitOutFn, const char* OutHarrisBriefDescFn);
#ifdef CONFIG_SOC_CV2
void RefCV_AmbaSpuFex_UT(const char* BinPath, const char* InFileL, const char* InFileR, const char* OutputFn, UINT32 FusionDisparity,
                         UINT32 Width, UINT32 Height, UINT32 RoiStartX, UINT32 RoiStartY, UINT32 RoiWidth, UINT32 RoiHeight);
void RefCV_AmbaFex_UT(const char* BinPath, const char* InFileL, const char* InFileR, const char* OutputFn,
                      UINT32 Width, UINT32 Height, UINT32 RoiStartX, UINT32 RoiStartY, UINT32 RoiWidth, UINT32 RoiHeight);
void RefCV_AmbaSpuFusion_UT(const char* BinPath, const char* LogPath, UINT32 LayerNum,
                            const char* InputScale0Fn, const char* InputScale2Fn, const char* InputScale4Fn, const char* OutputFn);
void RefCV_AmbaFma_UT(const char* BinPath, const char* LogPath, const char* InFile, const char* OutputFn);
void RefCV_AmbaSpuFexV1_UT(const char* BinPath, const char* InFileL, const char* InFileR, const char* OutputFn, UINT32 FusionDisparity,
                           UINT32 Width, UINT32 Height, UINT32 RoiStartX, UINT32 RoiStartY, UINT32 RoiWidth, UINT32 RoiHeight);
void RefCV_AmbaFexV1_UT(const char* BinPath, const char* InFileL, const char* InFileR, const char* OutputFn,
                        UINT32 Width, UINT32 Height, UINT32 RoiStartX, UINT32 RoiStartY, UINT32 RoiWidth, UINT32 RoiHeight, UINT32 ScaleId, UINT32 EchoMode);
void RefCV_AmbaSpuFusionV1_UT(const char* BinPath, const char* LogPath, UINT32 LayerNum,
                              const char* InputScale0Fn, const char* InputScale2Fn, const char* InputScale4Fn, const char* OutputFn);
void RefCV_AmbaFmaV1_UT(const char* BinPath, const char* LogPath, const char* InFile, const char* OutputFn, UINT32 ScaleId, UINT32 MvacMode, UINT32 EchoMode);
#ifdef CONFIG_QNX
void RefCV_AmbaVO_UT(void);
void RefCV_AmbaMVAC_UT(void);
#endif
#endif
#ifdef CONFIG_SOC_CV2FS
void RefCV_AmbaFex_UT(const char* BinPath, const char* LogPath, const char* InFile, const char* OutputFexFn);
void RefCV_AmbaSpuFex_UT(const char* BinPath, const char* LogPath, const char* InFile, const char* OutputSpFn, const char* OutputFexFn);
void RefCV_AmbaOfFex_UT(const char* BinPath, const char* LogPath, const char* InFile, const char* OutputOfFn, const char* OutputFexFn);
void RefCV_AmbaSpuFusion_UT(const char* BinPath, const char* LogPath, UINT32 LayerNum,
                            const char* InputScale0Fn, const char* InputScale2Fn, const char* InputScale4Fn, const char* OutputFn);
#endif
void RefCV_AmbaSWFma_UT(const char* InputPath1, const char* InputPath2, const char* OutputPath1, const char* OutputPath2, UINT32 Algo);

void RefCV_YieldTest(UINT32 thread_id, UINT32 num_of_frame, UINT32 blocking_run);
void RefCV_AmbaSofFex_UT(void);

#ifdef CONFIG_CV_FLEXIDAG_STIXEL
void SCA_Stixel_UT(const char* pInFile, const char* pOutFile, UINT32 RoadDetMode);
#endif

#if defined(CONFIG_BUILD_CTFW)
void RefCV_CtfwAc_UT(const char *pInputPath, UINT32 StartFrameIdx,
                     UINT32 EndFrameIdx, UINT32 FrameIdxStep);

#if defined(CONFIG_CV_FLEXIDAG_STIXEL)
void RefCV_CtfwStixel_UT(const char* pInputPath,
                         UINT32 StartFrameIndex, UINT32 EndFrameIndex, UINT32 RoadDetMode);
void RefCV_CtfwAcStixel_UT(const char *pInputPath, UINT32 StartFrameIdx,
                           UINT32 EndFrameIdx, UINT32 FrameIdxStep, UINT32 StixelRoadDetMode);
#endif
#if defined(CONFIG_CV_FLEXIDAG_STIXEL_V2)
void RefCV_CtfwStixelV2_UT(const char* pDispInputPath, const char* pLumaInputPath,
                           UINT32 StartFrameIndex, UINT32 EndFrameIndex, UINT32 RoadDetMode);
#endif
#endif

void SCA_Plain_UT(const char* BinPath, const char* LogPath, const char* InFile, UINT32 Opt);

#ifdef CONFIG_QNX
#ifdef CONFIG_SOC_CV2
void RefCV_AmbaAc_UT(void);
#endif
#endif

UINT32 RefCV_UT_GetCVBuf(UINT8** ppU8, UINT32 Size, UINT32* AlignedSize);
UINT32 RefCV_MemblkInit(void);
UINT32 RefCV_MemblkClean(const flexidag_memblk_t* buf);
UINT32 RefCV_MemblkInvalid(const flexidag_memblk_t* buf);
UINT32 RefCV_MemblkAlloc(UINT32 buf_size, flexidag_memblk_t* buf);
UINT32 RefCV_MemblkFree(flexidag_memblk_t* buf);

#endif //CVAPI_FLEXIDAG_UNITEST_H
