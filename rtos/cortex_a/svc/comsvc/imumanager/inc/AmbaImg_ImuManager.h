/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaImg_ImuManager.h
 *
 *  @Copyright      :: Copyright (C) 2016 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Constants and Definitions IMU Manager
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef IMG_IMU_MANAGER_H
#define IMG_IMU_MANAGER_H



#include "AmbaTypes.h"
#include "AmbaIMU.h"
#include "AmbaShell.h"

#define IMU_ERR_MANAGER_OK          (IMG_IMUMGR_ERR_BASE + 0U)
#define IMU_ERR_MANAGER_TSK_ERR     (IMG_IMUMGR_ERR_BASE + 1U)
#define IMU_ERR_MANAGER_CFG_ERR     (IMG_IMUMGR_ERR_BASE + 2U)
#define IMU_ERR_MANAGER_FIFO_ERR    (IMG_IMUMGR_ERR_BASE + 3U)
#define IMU_ERR_MANAGER_IMU_ERR     (IMG_IMUMGR_ERR_BASE +  4U)


#define AMBA_IMG_IMUMGR_INFO_NUM    2048U   // approximately 8 p30 frames at 8k sampling




typedef enum {
    AMBA_IMU_EVENT_ID_TIMER = 0,
    AMBA_IMU_EVENT_ID_ISR   = 1,
    AMBA_IMU_EVENT_ID_SOF   = 2,
    AMBA_IMU_EVENT_ID_MOF_TIMER = 3,
} AMBA_IMU_EVENT_ID_e;

typedef enum {
    AMBA_IMU_EVENT_FLAG_TIMER           = (1U << (UINT8)AMBA_IMU_EVENT_ID_TIMER),
    AMBA_IMU_EVENT_FLAG_IMU_ISR         = (1U << (UINT8)AMBA_IMU_EVENT_ID_ISR),
    AMBA_IMU_EVENT_FLAG_SOF             = (1U << (UINT8)AMBA_IMU_EVENT_ID_SOF),
    AMBA_IMU_EVENT_FLAG_MOF_TIMER       = (1U << (UINT8)AMBA_IMU_EVENT_ID_MOF_TIMER),
} AMBA_IMU_EVENT_FLAG_e;


typedef enum {
    AMBA_IMUMGR_READ_REG = 0,
    AMBA_IMUMGR_READ_FIFO,
    AMBA_IMUMGR_READ_TOTAL
} AMBA_IMG_IMUMGR_READ_MODE_e;



typedef struct {
    UINT32  VinId;              /* Which Vin */
    UINT64  CapSeqNo;           /* Capture sequence number */

    FLOAT   ExposureTime[4];    /* Exposure time in msec, HDR is considered */
    FLOAT   ReadoutTime;        /* data read out time in msec */
    UINT32  FrameTime;          /* Total frame time */
    UINT64  SofHiResSystemTime; /* Sof system Tick (high resolution) */
    UINT64  EofHiResSystemTime; /* Eof system Tick (high resolution) */
    UINT32  SofSystemTime;      /* Sof system Tick (low resolution) */
    UINT32  EofSystemTime;      /* Eof system Tick (low resolution) */

    UINT32  UserData;           /* other user data */
    UINT32  Reserved[8];
} AMBA_IMG_IMUMGR_EVENT_DATA_s;

typedef AMBA_IMU_DATA_s AMBA_IMG_IMUMGR_IMU_DATA_s;

typedef struct {
    UINT32  SofSystemTime;      /* latched at Mof, updated when FrameSync detected */
    UINT32  EofSystemTime;      /* latched at Mof, updated when FrameSync detected */
    UINT64  RawCapPts;          /* latched at Mof, updated when FrameSync detected */
    UINT32  PosIndex;           /* updated at each FiFo write */
    UINT32  SystemTimeTick;     /* updated at each FiFo write */

    UINT32  FrameTime;          /* latched at Mof, updated when FrameSync detected */
    FLOAT   ExposureTime[4];    /* latched at Mof, updated when FrameSync detected */
    FLOAT   ReadOutTime;        /* latched at Mof, updated when FrameSync detected */

    UINT32  Reserved[4];
} AMBA_IMG_IMUMGR_SYNC_DATA_s;

typedef struct {
    AMBA_IMG_IMUMGR_SYNC_DATA_s     Sync;
    AMBA_IMG_IMUMGR_IMU_DATA_s      Data;
} AMBA_IMG_IMUMGR_FIFO_INFO_s;

typedef struct {
    UINT32  Rd;
    UINT32  Wr;
} AMBA_IMG_IMUMGR_FIFO_INDEX_s;


typedef struct {
    AMBA_IMG_IMUMGR_FIFO_INDEX_s    Index;
    AMBA_IMG_IMUMGR_FIFO_INFO_s     Info[AMBA_IMG_IMUMGR_INFO_NUM];
} AMBA_IMG_IMUMGR_FIFO_s;

typedef struct {
    UINT32  Magic;
    UINT32  ImuId;
    UINT32  ReadIndex;
    UINT32  SystemTimeTick;
} AMBA_IMG_IMUMGR_FIFO_PORT_s;

typedef struct {
    UINT32  EnablePwmSync;
    UINT32  PwmSyncEdge;
    UINT32  Reserved;
} AMBA_IMG_IMUMGR_PWM_s;

typedef struct {
    UINT32  FiFoDepth;          /* config ImuMgr FiFo depth (in samples) */
    AMBA_IMG_IMUMGR_PWM_s  Config;
} AMBA_IMG_IMUMGR_CONFIG_s;

typedef struct {
    UINT32  FiFoOverFlow;
    UINT32  Reserved;
} AMBA_IMG_IMUMGR_IMU_STATUS_s;                /* Report IMU status (HW) */

typedef struct {
    UINT32  FiFoDepth;          /* ImuMgr FiFo depth (in samples) */
    UINT32  SamplingRate;       /* IMU GYRO sampling rate */
    UINT32  SampleByteSize;     /* ImuMgr FiFo sample size (in bytes) */
    AMBA_IMU_FIFO_CHANNEL_s FiFoEnable; /* IMU fifo channel enables */

    UINT32  FiFoStatus;         /* Report ImuMgr FiFo status (TBD) */
    UINT32  FiFoCurrentIndex;   /* current ImuMgr FiFo index */
    AMBA_IMG_IMUMGR_IMU_STATUS_s ImuStatus;

} AMBA_IMG_IMUMGR_FIFO_STATUS_s ;

typedef struct {
    UINT32  VinId;              /* Which Vin */
    UINT64  RawCapPts;          /* current raw capture pts tick @Mof */
} AMBA_IMG_IMUMGR_MOF_EVENT_DATA_s;

typedef int (*AMBA_IMG_IMUMGR_MOF_CB_FUNC_f)(AMBA_IMG_IMUMGR_MOF_EVENT_DATA_s *pEvent );


/* global function prototypes */
UINT32 AmbaImg_ImuMgr_FiFoReset(UINT32 ImuId);
UINT32 AmbaImg_ImuMgr_CloseFiFoPort(AMBA_IMG_IMUMGR_FIFO_PORT_s *Port);
UINT32 AmbaImg_ImuMgr_OpenFiFoPort(UINT32 ImuId, AMBA_IMG_IMUMGR_FIFO_PORT_s *Port);
UINT32 AmbaImg_ImuMgr_Config(UINT32 ImuId, const AMBA_IMG_IMUMGR_CONFIG_s *Config);
UINT32 AmbaImg_ImuMgr_FiFoIndexGet(const AMBA_IMG_IMUMGR_FIFO_PORT_s *Port, UINT16 Update, UINT32 *Index);
UINT32 AmbaImg_ImuMgr_FiFoDataGet(AMBA_IMG_IMUMGR_FIFO_PORT_s *Port, UINT32 Size, AMBA_IMG_IMUMGR_FIFO_INFO_s *FiFo);
UINT32 AmbaImg_ImuMgr_FiFoStatusGet(const AMBA_IMG_IMUMGR_FIFO_PORT_s *Port, AMBA_IMG_IMUMGR_FIFO_STATUS_s *Status);
UINT32 AmbaImg_ImuMgr_Init(UINT32 Priority, UINT32 CoreInclusion);
UINT32 AmbaImg_ImuMgr_TaskDelete(void);
UINT32 AmbaImg_ImuMgr_SofEventHandler(const void *hdlr, const UINT32 *pEventData);
UINT32 AmbaImg_ImuMgr_EofEventHandler(const void *hdlr, const UINT32 *pEventData);
UINT32 AmbaImg_ImuMgr_MofCallBackFuncReg(AMBA_IMG_IMUMGR_MOF_CB_FUNC_f CallBackFunc);
void AmbaImg_ImuMgr_TestCmd(UINT32 ArgCount,char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);


#endif
