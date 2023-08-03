/**
 *  @file AmbaImgMain_Internal.h
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  @details Constants and Definitions for Amba Image Main Internal
 *
 */

#ifndef AMBA_IMG_MAIN_INTERNAL_H
#define AMBA_IMG_MAIN_INTERNAL_H

#define AMBA_IMG_MAIN_MEM_ADDR    ULONG

#define AMBA_IMG_MAIN_DBG_COUNT     0U
#define AMBA_IMG_MAIN_DBG_STILL_TIMING    0U

//#define AMBA_IMG_PRINT_ENABLE_FLAG    (PRINT_FLAG_MSG | PRINT_FLAG_ERR | PRINT_FLAG_DBG)
#define AMBA_IMG_PRINT_ENABLE_FLAG    (PRINT_FLAG_MSG | PRINT_FLAG_ERR)

#define AMBA_IMG_MAIN_DGC_BOTH

extern AMBA_IMG_CHANNEL_s **pAmbaImgMainChannel;
extern UINT32 AmbaImgMain_DebugCount;
extern UINT32 AmbaImgMain_DebugStillTiming;
extern UINT32 AmbaImgMain_ImgAaaCore[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
extern UINT32 AmbaImgMain_SensorIdTable[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_SENSOR];
extern AMBA_IMG_CHANNEL_ID_s GNU_SECTION_NOZEROINIT AmbaImgMain_Algo2ChanTable[AMBA_IMG_NUM_VIN_CHANNEL*AMBA_IMG_NUM_VIN_ALGO*2UL];
extern UINT32 AmbaImgMain_LastMasterSyncTable[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_SENSOR];
#ifdef CONFIG_BUILD_IMGFRW_SMC
extern UINT32 AmbaImgMain_ImgSmcI2sId[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
#endif
extern UINT32 AmbaImgMain_VinToneCurveUpd[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

#if 0
/* mem api */
void AmbaImgMain_MemV2P(void *pParam1, const AMBA_IMG_MAIN_MEM_ADDR Param2);
void AmbaImgMain_MemP2P(void *pParam1, const void *pParam2);
void AmbaImgMain_MemP2V(void *pParam1, const void *pParam2);
void AmbaImgMain_MemF2V(void *pParam1, const void *pParam2);
#endif

#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV5) && !defined(CONFIG_SOC_CV52) && !defined(CONFIG_SOC_CV5X)
#define AAA_HEADER_CFA_AWB    Header
#define AAA_HEADER_CFA_AE     Header
#define AAA_HEADER_CFA_AF     Header
#define AAA_HEADER_RGB_AE     Header
#define AAA_HEADER_RGB_AF     Header

#define AAA_HEADER_CFA_ISO_TAG    MemInfo.Ctx.pCfa->Header.IsoConfigTag
#define AAA_HEADER_RGB_ISO_TAG    MemInfo.Ctx.pRgb->Header.IsoConfigTag

#define IMG_MAIN_VIN_TONE_CURVE_RUM_TIME    1U
#else
#define AAA_HEADER_CFA_AWB    Header.Awb
#define AAA_HEADER_CFA_AE     Header.CfaAe
#define AAA_HEADER_CFA_AF     Header.CfaAf
#define AAA_HEADER_RGB_AE     Header.PgAe
#define AAA_HEADER_RGB_AF     Header.PgAf

#define AAA_HEADER_CFA_ISO_TAG    0U
#define AAA_HEADER_RGB_ISO_TAG    0U

#define IMG_MAIN_VIN_TONE_CURVE_RUM_TIME    0U
#define IMG_MAIN_FE_WB_IR_GAIN
#endif

#endif  /* AMBA_IMG_MAIN_INTERNAL_H */
