/**
 *  @file RefFlow_Common.h
 *
 * Copyright (c) 2019 Ambarella International LP
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
 *  @details header file of reference flow common definitions
 *
 */

#ifndef REF_FLOW_COMMON_H
#define REF_FLOW_COMMON_H

#include "AmbaTypes.h"

#define RF_FUNC_NONE      (0x00000000U)
#define RF_FUNC_LDWS      (0x00010000U)
#define RF_FUNC_FC        (0x00020000U)
#define RF_FUNC_BSD       (0x00030000U)
#define RF_FUNC_PSD       (0x00040000U)
#define RF_FUNC_RCTA      (0x00050000U)
#define RF_FUNC_LD        (0x00060000U)
#define RF_FUNC_RMG       (0x00070000U)
#define RF_FUNC_AC        (0x00080000U)


#define RF_DATA_FMT_HDR_VER (0U)
#define RF_DATA_FMT_HDR_MGC (0xCAFEBEEFU)


typedef struct {
    UINT32 SrcW;        /**< Source image width */
    UINT32 SrcH;        /**< Source image height */
    UINT32 RoiW;        /**< ROI width at source image */
    UINT32 RoiH;        /**< ROI height at source image */
    UINT32 RoiX;        /**< ROI X offset at source image */
    UINT32 RoiY;        /**< ROI Y offset at source image */
} RF_LOG_CROP_INFO_s;

typedef struct {
    UINT32 Pitch;                        /**< Buffer pitch which is the width to change to next image row */
    UINT32 Width;                        /**< Image width */
    UINT32 Height;                       /**< Image height */
    UINT32 TimeStamp;                    /**< Time stamp of detection frame */
    UINT32 FrameNum;                     /**< frame number of detection frame */
    RF_LOG_CROP_INFO_s CropInfo;         /**< Crop information of segmentation buffer */
    UINT32 EncodeMode;                   /**< 0: Raw data, 1: Run length encode */
    UINT32 BufferSize;                   /**< Seg buffer size */
    // UINT8 SegBufer[BufferSize];
} RF_LOG_SEG_BUF_HEADER_s;

typedef struct {
    UINT32 MsgCode;                                   /**< message code */
    UINT32 CaptureTime;                               /**< capture time of detection frame, default 0 */
    UINT32 FrameNum;                                  /**< frame number of detection frame, default 0 */
    UINT32 NumBbx;                                    /**< total number of object */
    UINT32 Width;                                     /**< Width for BBox coordinate */
    UINT32 Height;                                    /**< Height for BBox coordinate */
    RF_LOG_CROP_INFO_s CropInfo;                      /**< Crop information of OD ROI (the maximam ROI) */
    UINT32 RoiNum;                                    /**< Number of OD ROI */
    RF_LOG_CROP_INFO_s Roi[8U];                       /**< OD ROI information */
    // AMBA_OD_2DBBX_s Bbx[NumBbx];
} RF_LOG_OD_2DBBOX_HEADER_s;

typedef struct {
    UINT32 ObjStartX;                                  /**< Start x from pObjTable */
    UINT32 ObjStartY;                                  /**< Start y from pObjTable */
    UINT32 ObjPitch;                                   /**< Pitch of pObjTable */
    UINT32 ObjWidth;                                   /**< Width of pObjTable */
    UINT32 ObjHeight;                                  /**< Height of pObjTable */
    UINT32 EncodeMode;                                 /**< 0: Raw data, 1: Run length encode */
    UINT32 BufferSize;                                 /**< Buffer size of pObjTable */
    // UINT8 ObjTable[BufferSize];
} RF_LOG_OD_2ND_OBJ_SEG_INFO;

typedef struct {
    UINT32 X;                                          /**< x coordinate */
    UINT32 Y;                                          /**< y coordinate */
    UINT32 Confidence;                                 /**< Confidence */
} RF_LOG_OD_2ND_OBJ_KP_COORD_s;

typedef struct {
    UINT32 CoordNum;                                   /**< Number of Coordinate */
    //RF_LOG_OD_2ND_OBJ_KP_COORD_s Coord[CoordNum];
} RF_LOG_OD_2ND_OBJ_KP_INFO_s;

typedef struct {
    UINT32 ObjIdx;                                     /**< Correspond to index of order of Bbx[NumBbx] in RF_LOG_OD_2DBBOX_HEADER_s */
    UINT32 DataType;                                   /**< 0: 3d (AMBA_OD_3DBBX_s),
                                                            1: MK (RF_LOG_OD_2ND_OBJ_SEG_INFO),
                                                            2: KP (RF_LOG_OD_2ND_OBJ_KP_INFO_s),
                                                            3: CL (UINT32) */
    // AMBA_OD_3DBBX_s or RF_LOG_OD_2ND_OBJ_SEG_INFO or RF_LOG_OD_2ND_OBJ_KP_INFO_s or UINT32;
} RF_LOG_OD_2ND_DATA_s;

typedef struct {
    UINT32 ObjNum;                                    /**< Total number of object */
    UINT32 CaptureTime;                               /**< Capture time of detection frame, default 0 */
    UINT32 FrameNum;                                  /**< Frame number of detection frame, default 0 */
    // RF_LOG_OD_2ND_DATA_s Od2ndData[ObjNum];
} RF_LOG_OD_2ND_DATA_HEADER_s;

/******************************************************************/
/** Data Type for log                                             */
/******************************************************************/
/** Refer to RF_LOG_SEG_BUF_HEADER_s */
#define RF_COM_DFMT_SEG_V000                  (0x00000100U)            /**< 0x00000100 to 0x000001FF: Reserved for RF_LOG_SEG_BUF_HEADER_s*/
#define RF_COM_DFMT_SEG                       (RF_FUNC_NONE | RF_COM_DFMT_SEG_V000)
#define RF_COM_DFMT_SEG_COLOR_V000            (0x00001100U)            /**< 0x00001100 to 0x000011FF: Reserved for RF_LOG_SEG_BUF_HEADER_s*/
#define RF_COM_DFMT_SEG_COLOR                 (RF_FUNC_NONE | RF_COM_DFMT_SEG_COLOR_V000)
#define RF_COM_DFMT_SEG_TYPE_V000             (0x00002100U)            /**< 0x00002100 to 0x000021FF: Reserved for RF_LOG_SEG_BUF_HEADER_s*/
#define RF_COM_DFMT_SEG_TYPE                  (RF_FUNC_NONE | RF_COM_DFMT_SEG_TYPE_V000)
#define RF_COM_DFMT_SEG_INSTANCE_V000         (0x00003100U)            /**< 0x00003100 to 0x000031FF: Reserved for RF_LOG_SEG_BUF_HEADER_s*/
#define RF_COM_DFMT_SEG_INSTANCE              (RF_FUNC_NONE | RF_COM_DFMT_SEG_INSTANCE_V000)

/** Refer to RF_LOG_OD_2DBBOX_HEADER_s */
#define RF_COM_DFMT_2DBBOX_V000               (0x00000200U)            /**< 0x00000200 to 0x000002FF: Reserved for RF_LOG_OD_2DBBOX_HEADER_s, V000 is invalid now */
#define RF_COM_DFMT_2DBBOX_V001               (0x00000201U)            /**< 0x00000200 to 0x000002FF: Reserved for RF_LOG_OD_2DBBOX_HEADER_s */
#define RF_COM_DFMT_2DBBOX                    (RF_FUNC_NONE | RF_COM_DFMT_2DBBOX_V001)

/** Refer to RF_LOG_OD_2ND_DATA_HEADER_s */
#define RF_COM_DFMT_3DBBOX_V000               (0x00000300U)            /**< 0x00000300 to 0x000003FF: Reserved for RF_LOG_OD_2ND_DATA_HEADER_s */
#define RF_COM_DFMT_3DBBOX                    (RF_FUNC_NONE | RF_COM_DFMT_3DBBOX_V000)
#define RF_COM_DFMT_MKLBBOX_V000              (0x00000400U)            /**< 0x00000400 to 0x000004FF: Reserved for RF_LOG_OD_2ND_DATA_HEADER_s */
#define RF_COM_DFMT_MKLBBOX                   (RF_FUNC_NONE | RF_COM_DFMT_MKLBBOX_V000)
#define RF_COM_DFMT_MKSBBOX_V000              (0x00000500U)            /**< 0x00000500 to 0x000005FF: Reserved for RF_LOG_OD_2ND_DATA_HEADER_s */
#define RF_COM_DFMT_MKSBBOX                   (RF_FUNC_NONE | RF_COM_DFMT_MKSBBOX_V000)
#define RF_COM_DFMT_OD_KP_V000                (0x00000600U)            /**< 0x00000600 to 0x000006FF: Reserved for RF_LOG_OD_2ND_DATA_HEADER_s */
#define RF_COM_DFMT_OD_KP                     (RF_FUNC_NONE | RF_COM_DFMT_OD_KP_V000)
#define RF_COM_DFMT_OD_TS_V000                (0x00000700U)            /**< 0x00000700 to 0x000007FF: Reserved for RF_LOG_OD_2ND_DATA_HEADER_s */
#define RF_COM_DFMT_OD_TS                     (RF_FUNC_NONE | RF_COM_DFMT_OD_TS_V000)
#define RF_COM_DFMT_OD_SL_V000                (0x00000800U)            /**< 0x00000800 to 0x000008FF: Reserved for RF_LOG_OD_2ND_DATA_HEADER_s */
#define RF_COM_DFMT_OD_SL                     (RF_FUNC_NONE | RF_COM_DFMT_OD_SL_V000)
#define RF_COM_DFMT_OD_TLC_V000               (0x00000900U)            /**< 0x00000900 to 0x000009FF: Reserved for RF_LOG_OD_2ND_DATA_HEADER_s */
#define RF_COM_DFMT_OD_TLC                    (RF_FUNC_NONE | RF_COM_DFMT_OD_TLC_V000)
#define RF_COM_DFMT_OD_TLS_V000               (0x00000A00U)            /**< 0x00000A00 to 0x00000AFF: Reserved for RF_LOG_OD_2ND_DATA_HEADER_s */
#define RF_COM_DFMT_OD_TLS                    (RF_FUNC_NONE | RF_COM_DFMT_OD_TLS_V000)
#define RF_COM_DFMT_OD_AR_V000                (0x00000B00U)            /**< 0x00000B00 to 0x00000BFF: Reserved for RF_LOG_OD_2ND_DATA_HEADER_s */
#define RF_COM_DFMT_OD_AR                     (RF_FUNC_NONE | RF_COM_DFMT_OD_AR_V000)

#pragma pack(1)
typedef struct {
    UINT32 Magic;
    UINT32 HdrVer;
    UINT32 DataFmt;
    UINT32 Count;
    UINT32 Channel;
    UINT32 CapPTS;   /**< 0x0U for invalid */
    UINT32 DataSize;
    void *pDataAddr;
} RF_LOG_HEADER_V0_s;
#pragma pack()

#define RF_LOG_HEADER_s RF_LOG_HEADER_V0_s

/** it's only support blocking function */
typedef UINT32 (*REF_FLOW_LOG_FP)(RF_LOG_HEADER_s *pHeader);

#endif
