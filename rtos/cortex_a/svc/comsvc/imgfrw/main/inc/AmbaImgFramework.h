/**
 *  @file AmbaImgFramework.h
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
 *  @details Constants and Definitions for Amba Image Framework
 *
 */

#ifndef AMBA_IMG_FRW_H
#define AMBA_IMG_FRW_H

#ifndef AMBA_IMG_FRW_LIB_COMSVC
#if 0
#include "AmbaImgFramework_Chan.h"

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgChannel
\*---------------------------------------------------------------------------*/
  void AmbaImgChannel_Register(AMBA_IMG_CHANNEL_s **pImageChannel);
UINT32 AmbaImgChannel_TotalZoneIdGet(UINT32 VinId);
UINT32 AmbaImgChannel_VrMap(UINT32 VinId, AMBA_IMG_CHANNEL_s * const *pImageChannel);

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgSystem
\*---------------------------------------------------------------------------*/
UINT32 AmbaImgSystem_Init(void);
UINT32 AmbaImgSystem_Debug(UINT32 VinId, UINT32 Flag);
UINT32 AmbaImgSystem_Reset(UINT32 VinId);

UINT32 AmbaImgSystem_ContextIdGet(UINT32 ZoneIndex);
UINT32 AmbaImgSystem_RingGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SYSTEM_RING_PIPE_s **pPipe, UINT32 RawCapSeq);

UINT32 AmbaImgSystem_ImageModeSet(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const AMBA_IK_MODE_CFG_s *pImgMode);
UINT32 AmbaImgSystem_ImageModeGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, AMBA_IK_MODE_CFG_s **pImgMode);

UINT32 AmbaImgSystem_BlcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const INT32 *pOffset);
UINT32 AmbaImgSystem_BlcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, INT32 *pOffset);

UINT32 AmbaImgSystem_WbGainMix(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 Enable);
UINT32 AmbaImgSystem_WbGainFine(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 Enable);

UINT32 AmbaImgSystem_WgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pWgc);
UINT32 AmbaImgSystem_IgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pIgc);
UINT32 AmbaImgSystem_DgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pDgc);
UINT32 AmbaImgSystem_GgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pGgc);

UINT32 AmbaImgSystem_WgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pWgc);
UINT32 AmbaImgSystem_IgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pIgc);
UINT32 AmbaImgSystem_DgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pDgc);
UINT32 AmbaImgSystem_GgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pGgc);

UINT32 AmbaImgSystem_FineWgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pWgc);
UINT32 AmbaImgSystem_FineIgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pIgc);
UINT32 AmbaImgSystem_FineDgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pDgc);
UINT32 AmbaImgSystem_FineGgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pGgc);

UINT32 AmbaImgSystem_FineWgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pWgc);
UINT32 AmbaImgSystem_FineIgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pIgc);
UINT32 AmbaImgSystem_FineDgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pDgc);
UINT32 AmbaImgSystem_FineGgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pGgc);

UINT32 AmbaImgSystem_R2yUserBuffer(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Size, UINT32 Count, UINT8 *pBuf);
UINT32 AmbaImgSystem_R2yUserDelay(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 R2yUserDelay, UINT32 R2yDelay);

UINT32 AmbaImgSystem_PostAikWait(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SYSTEM_PAIK_INFO_s **pPostAikInfo);
UINT32 AmbaImgSystem_PostAik(const AMBA_IMG_SYSTEM_PAIK_INFO_s *pPostAikInfo);

UINT32 AmbaImgSystem_R2yUserWait(AMBA_IMG_SYSTEM_PAIK_INFO_s *pPostAikInfo);
UINT32 AmbaImgSystem_R2yUserDone(UINT64 PostAikId);

UINT32 AmbaImgSystem_AikExec(AMBA_IMG_CHANNEL_ID_s ImageChanId);

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgEvent
\*---------------------------------------------------------------------------*/
UINT32 AmbaImgEvent_Init(void);

/*-----------------------------------------------------------------------------------------------*\
 * Definitions in AmbaImgMessage.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaImgMessage_Init(void);

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgStatistics
\*---------------------------------------------------------------------------*/
UINT32 AmbaImgStatistics_Init(void);
UINT32 AmbaImgStatistics_Debug(UINT32 VinId, UINT32 Flag);
UINT32 AmbaImgStatistics_Reset(UINT32 VinId);
UINT32 AmbaImgStatistics_Inter(UINT32 VinId, UINT32 Enable);

UINT32 AmbaImgStatistics_Entry(UINT32 VinId, UINT32 ZoneId, const void **pCfa, const void **pRgb, UINT32 RawCapSeq);
UINT32 AmbaImgStatistics_Push(UINT32 VinId);

UINT32 AmbaImgStatistics_ZoneIdGet(AMBA_IMG_CHANNEL_ID_s ImageChanId);
UINT32 AmbaImgStatistics_GZoneIdGet(AMBA_IMG_CHANNEL_ID_s ImageChanId);
UINT32 AmbaImgStatistics_TZoneIdGet(UINT32 VinId);

UINT32 AmbaImgStatistics_OpenEx(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_STATISTICS_PORT_s *pPort, AMBA_IMG_STATISTICS_FUNC_f pFunc);
UINT32 AmbaImgStatistics_Open(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_STATISTICS_PORT_s *pPort);
UINT32 AmbaImgStatistics_Request(AMBA_IMG_STATISTICS_PORT_s *pPort, UINT32 Flag);
UINT32 AmbaImgStatistics_Close(AMBA_IMG_STATISTICS_PORT_s *pPort);

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgSensorHAL
\*---------------------------------------------------------------------------*/
UINT32 AmbaImgSensorHAL_Init(void);
UINT32 AmbaImgSensorHAL_Debug(UINT32 VinId, UINT32 Flag);
UINT32 AmbaImgSensorHAL_Reset(UINT32 VinId);
UINT32 AmbaImgSensorHAL_Stop(UINT32 VinId);

UINT32 AmbaImgSensorHAL_InfoGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_HAL_INFO_s **pInfo);
UINT32 AmbaImgSensorHAL_RingGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData, UINT32 RawCapSeq);
#ifdef CONFIG_BUILD_IMGFRW_FIFO_PROT
UINT32 AmbaImgSensorHAL_FifoPutCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId);
#endif
UINT32 AmbaImgSensorHAL_TimingMarkEn(UINT32 VinId, UINT32 Enable);
UINT32 AmbaImgSensorHAL_TimingMarkChk(UINT32 VinId);
UINT32 AmbaImgSensorHAL_TimingMarkClr(UINT32 VinId);
UINT32 AmbaImgSensorHAL_TimingMarkPutE(UINT32 VinId, const char *pName);
  void AmbaImgSensorHAL_TimingMarkPut(UINT32 VinId, const char *pName);
  void AmbaImgSensorHAL_TimingMarkPrt(UINT32 VinId);

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgSensorSync
\*---------------------------------------------------------------------------*/
UINT32 AmbaImgSensorSync_Init(void);
UINT32 AmbaImgSensorSync_Debug(UINT32 VinId, UINT32 Flag);
UINT32 AmbaImgSensorSync_Reset(UINT32 VinId);
UINT32 AmbaImgSensorSync_Stop(UINT32 VinId);

UINT32 AmbaImgSensorSync_RawCapSeqPut(UINT32 VinId, UINT32 RawCapSeq);
UINT32 AmbaImgSensorSync_TimingCb(UINT32 VinId, AMBA_IMG_SENSOR_SYNC_TIMING_CB_f pFunc);

UINT32 AmbaImgSensorSync_UserBuffer(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Size, UINT32 Count, UINT8 *pMem);
 void *AmbaImgSensorSync_UserBufferGet(AMBA_IMG_CHANNEL_ID_s ImageChanId);

UINT32 AmbaImgSensorSync_Sof(UINT32 VinId);
UINT32 AmbaImgSensorSync_Eof(UINT32 VinId);

UINT32 AmbaImgSensorSync_Wait(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_SYNC_WAIT_PORT_s *pPort, UINT32 Mode);
UINT32 AmbaImgSensorSync_WaitPush(AMBA_IMG_SENSOR_SYNC_WAIT_PORT_s *pPort);

UINT32 AmbaImgSensorSync_Request(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 Mode);
UINT32 AmbaImgSensorSync_SnapSequence(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_SYNC_SNAP_PORT_s *pPort, AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 Mode);

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgSensorDrv
\*---------------------------------------------------------------------------*/
UINT32 AmbaImgSensorDrv_Init(void);

UINT32 AmbaImgSensorDrv_RegWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT8 *pBuf, UINT8 Size);
UINT32 AmbaImgSensorDrv_AgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pAgc);
UINT32 AmbaImgSensorDrv_DgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pDgc);
UINT32 AmbaImgSensorDrv_ShrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pShr);
UINT32 AmbaImgSensorDrv_SvrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pSvr);
UINT32 AmbaImgSensorDrv_SlsWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pSls);
UINT32 AmbaImgSensorDrv_WgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT64 *pWgc);
UINT32 AmbaImgSensorDrv_GainConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pAgc, UINT32 *pDgc, AMBA_IMG_SENSOR_WB_s *pWgc);
UINT32 AmbaImgSensorDrv_ShutterConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pShr);
UINT32 AmbaImgSensorDrv_SlsConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pSls);

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgVar
\*---------------------------------------------------------------------------*/
typedef enum /*_AMBA_IMG_VAR_LEADING_e_*/ {
    VAR_LEADING_NONE = 0,
    VAR_LEADING_ZERO,
    VAR_LEADING_SPACE
} AMBA_IMG_VAR_LEADING_e;

UINT32 var_strlen(const char *pString);
UINT32 var_strcmp(const char *pString1, const char *pString2);
UINT32 var_strcat(char *pString1, const char *pString2);

  void var_utoa(UINT32 Value, char *pString, UINT32 Base, UINT32 Count, UINT32 Flag);
  void var_itoa( INT32 Value, char *pString, UINT32 Base, UINT32 Count, UINT32 Flag);

  void var_print(const char *pFmt, const UINT32 *Argc, const char * const *Argv);

  void AmbaImgPrint_Enable(UINT32 Enable);
  void AmbaImgPrint_EnableGet(UINT32 *pFlag);

  void AmbaImgPrintEx(UINT32 Id, const char *pStr, UINT32 var_ul, UINT32 var_base);
  void AmbaImgPrint64Ex(UINT32 Id, const char *pStr, UINT64 var_ull, UINT32 var_base);
  void AmbaImgPrint(UINT32 Id, const char *pStr, UINT32 var_ul);
  void AmbaImgPrintStr(UINT32 Id, const char *pStr);
  void AmbaImgPrintEx2(UINT32 Id, const char *pStr, UINT32 var0_ul, UINT32 var0_base, UINT32 var1_ul, UINT32 var1_base);
  void AmbaImgPrint64Ex2(UINT32 Id, const char *pStr, UINT64 var0_ull, UINT32 var0_base, UINT64 var1_ull, UINT32 var1_base);
  void AmbaImgPrint2(UINT32 Id, const char *pStr, UINT32 var0_ul, UINT32 var1_ul);
  void AmbaImgPrint_Flush(void);

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgMem
\*---------------------------------------------------------------------------*/
#define AMBA_IMG_MEM_ADDR  ULONG
#else
#include "../../src/inc/AmbaImgChannel.h"
#include "../../src/inc/AmbaImgSystem.h"
#include "../../src/inc/AmbaImgEvent.h"
#include "../../src/inc/AmbaImgMessage.h"
#include "../../src/inc/AmbaImgSensorHAL.h"
#include "../../src/inc/AmbaImgSensorSync.h"
#include "../../src/inc/AmbaImgSensorDrv.h"
#include "../../src/inc/AmbaImgStatistics.h"
#include "../../src/inc/AmbaImgVar.h"
#include "../../src/inc/AmbaImgMem.h"

#define AMBA_IMG_NUM_VIN_CHANNEL    AMBA_IMGFRW_NUM_VIN_CHANNEL
#define AMBA_IMG_NUM_VIN_SENSOR     AMBA_IMGFRW_NUM_VIN_SENSOR
#define AMBA_IMG_NUM_VIN_ALGO       AMBA_IMGFRW_NUM_VIN_ALGO
#define AMBA_IMG_NUM_FOV_CHANNEL    AMBA_IMGFRW_NUM_FOV_CHANNEL

extern AMBA_IMG_SYSTEM_INFO_s AmbaImgSystem_Info[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
extern AMBA_IMG_SENSOR_HAL_AUX_s AmbaImgSensorHAL_AuxInfo[AMBA_IMG_NUM_VIN_CHANNEL];
extern AMBA_IMG_SENSOR_HAL_INFO_s AmbaImgSensorHAL_Info[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
extern AMBA_IMG_SENSOR_HAL_HDR_INFO_s AmbaImgSensorHAL_HdrInfo[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
extern AMBA_IMG_CHANNEL_s *AmbaImgChannel_Entry[AMBA_IMG_NUM_VIN_CHANNEL];
extern AMBA_IMG_CHANNEL_s *AmbaImgChannel_EntryTable[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
extern UINT32 AmbaImgMain_ChanMemInit(void);
extern AMBA_IMG_CHANNEL_s *AmbaImgChannel_UserEntry(UINT32 VinId, UINT32 ChainIndex);
AMBA_IMG_CHANNEL_s **AmbaImgChannel_Select(UINT32 VinId, UINT32 ChainIndex);
UINT32 AmbaImgChannel_SensorIdPut(UINT32 VinId, UINT32 ChainId, UINT32 SensorId);
UINT32 AmbaImgChannel_AlgoIdPut(UINT32 VinId, UINT32 ChainId, UINT32 AlgoId);
UINT32 AmbaImgChannel_ZoneIdPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneId);
UINT32 AmbaImgChannel_ZoneMsbIdPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneMsbId);
UINT32 AmbaImgChannel_VrIdPut(UINT32 VinId, UINT32 ChainId, UINT32 VrId, UINT32 VrAltId);
UINT32 AmbaImgChannel_InterIdPut(UINT32 VinId, UINT32 ChainId, UINT32 InterId, UINT32 InterNum, UINT32 SkipFrame, UINT32 Mode);
UINT32 AmbaImgChannel_PostZoneIdPut(UINT32 VinId, UINT32 ChainId, UINT32 PostZoneId);
UINT32 AmbaImgChannel_PostZoneIdAdd(UINT32 VinId, UINT32 ChainId, UINT32 PostZoneId);
UINT32 AmbaImgChannel_ZoneIsoDisIdPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIsoDisId);
UINT32 AmbaImgChannel_ZoneIsoDisIdAdd(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIsoDisId);
UINT32 AmbaImgChannel_ZoneIsoCb(UINT32 VinId, UINT32 ChainId, AMBA_IMG_CHANNEL_ISO_CB_f pFunc);
UINT32 AmbaImgChannel_SnapAebIdPut(UINT32 VinId, UINT32 ChainId, UINT32 SnapAebId);
UINT32 AmbaImgChannel_SnapAebIdGet(UINT32 VinId, UINT32 ChainId, UINT32 *pSnapAebId);
UINT32 AmbaImgChannel_PipeOutPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 Width, UINT32 Height);
UINT32 AmbaImgChannel_PipeOutGet(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 *pWidth, UINT32 *pHeight);
UINT32 AmbaImgChannel_PipeModePut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 Mode);
UINT32 AmbaImgChannel_PipeModeGet(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 *pMode);
UINT32 AmbaImgChannel_EFovIdPut(UINT32 VinId, UINT32 ChainId, UINT64 EFovId);
UINT32 AmbaImgChannel_EFovIdGet(UINT32 VinId, UINT32 ChainId, UINT64 *pEFovId);
#endif

#else

#include "AmbaImgChannel.h"
#include "AmbaImgSystem.h"
#include "AmbaImgEvent.h"
#include "AmbaImgMessage.h"
#include "AmbaImgSensorHAL.h"
#include "AmbaImgSensorSync.h"
#include "AmbaImgSensorDrv.h"
#include "AmbaImgStatistics.h"
#include "AmbaImgVar.h"
#include "AmbaImgMem.h"

#define AMBA_IMG_NUM_VIN_CHANNEL    AMBA_IMGFRW_NUM_VIN_CHANNEL
#define AMBA_IMG_NUM_VIN_SENSOR     AMBA_IMGFRW_NUM_VIN_SENSOR
#define AMBA_IMG_NUM_VIN_ALGO       AMBA_IMGFRW_NUM_VIN_ALGO
#define AMBA_IMG_NUM_FOV_CHANNEL    AMBA_IMGFRW_NUM_FOV_CHANNEL

extern AMBA_IMG_SYSTEM_INFO_s AmbaImgSystem_Info[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
extern AMBA_IMG_SENSOR_HAL_AUX_s AmbaImgSensorHAL_AuxInfo[AMBA_IMG_NUM_VIN_CHANNEL];
extern AMBA_IMG_SENSOR_HAL_INFO_s AmbaImgSensorHAL_Info[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
extern AMBA_IMG_SENSOR_HAL_HDR_INFO_s AmbaImgSensorHAL_HdrInfo[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
extern AMBA_IMG_CHANNEL_s *AmbaImgChannel_Entry[AMBA_IMG_NUM_VIN_CHANNEL];
extern AMBA_IMG_CHANNEL_s *AmbaImgChannel_EntryTable[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
extern UINT32 AmbaImgMain_ChanMemInit(void);
extern AMBA_IMG_CHANNEL_s *AmbaImgChannel_UserEntry(UINT32 VinId, UINT32 ChainIndex);
AMBA_IMG_CHANNEL_s **AmbaImgChannel_Select(UINT32 VinId, UINT32 ChainIndex);
UINT32 AmbaImgChannel_SensorIdPut(UINT32 VinId, UINT32 ChainId, UINT32 SensorId);
UINT32 AmbaImgChannel_AlgoIdPut(UINT32 VinId, UINT32 ChainId, UINT32 AlgoId);
UINT32 AmbaImgChannel_ZoneIdPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneId);
UINT32 AmbaImgChannel_ZoneMsbIdPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneMsbId);
UINT32 AmbaImgChannel_VrIdPut(UINT32 VinId, UINT32 ChainId, UINT32 VrId, UINT32 VrAltId);
UINT32 AmbaImgChannel_InterIdPut(UINT32 VinId, UINT32 ChainId, UINT32 InterId, UINT32 InterNum, UINT32 SkipFrame, UINT32 Mode);
UINT32 AmbaImgChannel_PostZoneIdPut(UINT32 VinId, UINT32 ChainId, UINT32 PostZoneId);
UINT32 AmbaImgChannel_PostZoneIdAdd(UINT32 VinId, UINT32 ChainId, UINT32 PostZoneId);
UINT32 AmbaImgChannel_ZoneIsoDisIdPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIsoDisId);
UINT32 AmbaImgChannel_ZoneIsoDisIdAdd(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIsoDisId);
UINT32 AmbaImgChannel_ZoneIsoCb(UINT32 VinId, UINT32 ChainId, AMBA_IMG_CHANNEL_ISO_CB_f pFunc);
UINT32 AmbaImgChannel_SnapAebIdPut(UINT32 VinId, UINT32 ChainId, UINT32 SnapAebId);
UINT32 AmbaImgChannel_SnapAebIdGet(UINT32 VinId, UINT32 ChainId, UINT32 *pSnapAebId);
UINT32 AmbaImgChannel_PipeOutPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 Width, UINT32 Height);
UINT32 AmbaImgChannel_PipeOutGet(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 *pWidth, UINT32 *pHeight);
UINT32 AmbaImgChannel_PipeModePut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 Mode);
UINT32 AmbaImgChannel_PipeModeGet(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 *pMode);
UINT32 AmbaImgChannel_EFovIdPut(UINT32 VinId, UINT32 ChainId, UINT64 EFovId);
UINT32 AmbaImgChannel_EFovIdGet(UINT32 VinId, UINT32 ChainId, UINT64 *pEFovId);

#endif

typedef enum /*_AMBA_IMG_VAR_MSG_ID_e_*/ {
    PRINT_ID_DBG = 0,
    PRINT_ID_MSG,
    PRINT_ID_ERR,
    PRINT_ID_STAT,

    PRINT_ID_CYCLE,
    PRINT_ID_LOCK,
    PRINT_ID_SYNC,
    PRINT_ID_STATUS,

    PRINT_ID_WARP,
    PRINT_ID_TOTAL
} AMBA_IMG_VAR_MSG_ID_e;

#define PRINT_FLAG_DBG     (((UINT32) 1U) << (UINT32) PRINT_ID_DBG)
#define PRINT_FLAG_MSG     (((UINT32) 1U) << (UINT32) PRINT_ID_MSG)
#define PRINT_FLAG_ERR     (((UINT32) 1U) << (UINT32) PRINT_ID_ERR)
#define PRINT_FLAG_STAT    (((UINT32) 1U) << (UINT32) PRINT_ID_STAT)

#define PRINT_FLAG_CYCLE   (((UINT32) 1U) << (UINT32) PRINT_ID_CYCLE)
#define PRINT_FLAG_LOCK    (((UINT32) 1U) << (UINT32) PRINT_ID_LOCK)
#define PRINT_FLAG_SYNC    (((UINT32) 1U) << (UINT32) PRINT_ID_SYNC)
#define PRINT_FLAG_STATUS  (((UINT32) 1U) << (UINT32) PRINT_ID_STATUS)

#define PRINT_FLAG_WARP    (((UINT32) 1U) << (UINT32) PRINT_ID_WARP)

#endif  /* AMBA_IMG_FRW_H */
