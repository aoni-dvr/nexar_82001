/**
 *  @file AmbaImg_Proc.h
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
 *
 *  @Description    :: Definitions & Constants for Sample AE(Auto Exposure) and AWB(Auto White Balance) algorithms
 *
 */

#ifndef AMBA_IMG_PROC
#define AMBA_IMG_PROC

#include "AmbaKAL.h"
#include "AmbaDSP_Image3aStatistics.h"
#include "AmbaDSP_ImageFilter.h"

//#include "AmbaDSP_ImgDef.h"

#define DISABLE            0U
#define ENABLE             1U

#define    CE_EV_MOD          1U
#define    CE_BOOSTRTO_MOD    2U
#define    CE_FOGSTR_MOD      3U
#define    CE_MAX_MOD         4U

#define MAX_AEB_NUM  9U
#define MAX_VIEW_NUM 16U
#define MAX_EXP_GROUP_NUM 4
#define MAX_STAT_COUNT  5

#define BFO_CE_GAIN_DELY_FRAM 2U

/* Image Pipe Mode */
#define IP_EXPERSS_MODE      0U
#define IP_HYBRID_MODE       1U

/* Image Related Mode */
#define IP_MODE_VIDEO      0U
#define IP_MODE_STILL      1U

#define IP_MODE_LISO_STILL 2
#define IP_MODE_HISO_STILL 3
#define IP_MODE_LISO_VIDEO 4
#define IP_MODE_HISO_VIDEO 5

#define IP_MODE_LISO_VIDEO 4
#define IP_MODE_HISO_VIDEO 5

/* Flow Related Mode */
#define IP_PREVIEW_MODE    0U
#define IP_FASTFOCUS_MODE  1U
#define IP_CAPTURE_MODE    2U
#define IP_PREFLASH_MODE   3U

/* Pipeline Related Mode */
#define IP_PIPELINE_NORMAL_MODE       1
#define IP_PIPELINE_SGOP_MODE         2
#define IP_PIPELINE_LOWPOWER_MODE     3
#define IP_PIPELINE_DRAMEFFCY_MODE    4
#define IP_PIPELINE_SGOP_MCTFOFF_MODE 5


/** vin_changed type */
#define VIN_CHG_NORMAL        0U
#define VIN_CHG_FAST          1U
#define VIN_CHG_FLOCKED       2U
#define VIN_CHG_BG_DONE       3U
/********************/

#define IP_CAP_NO_AEB           0x0000
#define IP_CAP_AEB              0x0008

#define AMBA_IMG_AEAWBADJ_TSK_PRIORITY          25
#define AMBA_IMG_AEAWBADJ_TSK_STACK_SIZE        32768
#define AMBA_IMG_AEAWBADJ_TSK_CORE_BIT_MAP      0x0001

#define AMBA_IMG_AE_TSK_PRIORITY                20
#define AMBA_IMG_AE_TSK_STACK_SIZE              16384
#define AMBA_IMG_AE_TSK_CORE_BIT_MAP            0x0001
#define AMBA_IMG_AWB_TSK_PRIORITY               30
#define AMBA_IMG_AWB_TSK_STACK_SIZE             2048
#define AMBA_IMG_AWB_TSK_CORE_BIT_MAP           0x0001
#define AMBA_IMG_ADJ_TSK_PRIORITY               50
#define AMBA_IMG_ADJ_TSK_STACK_SIZE             8192
#define AMBA_IMG_ADJ_TSK_CORE_BIT_MAP           0x0001
#define AMBA_IMG_OB_TSK_PRIORITY                45
#define AMBA_IMG_OB_TSK_STACK_SIZE              8192
#define AMBA_IMG_OB_TSK_CORE_BIT_MAP  0x0001

#define IMG_NORMAL_MODE        (UINT32)0   //Without HDR
#define IMG_DSP_HDR_MODE_0     (UINT32)1   //DSP HDR, 2 exposures
#define IMG_DSP_HDR_MODE_1     (UINT32)2   //DSP HDR, 3 exposures

#define IMG_SENSOR_HDR_MODE_0  (UINT32)10  //Sensor HDR
#define IMG_SENSOR_HDR_MODE_1  (UINT32)20  //Sensor HDR, 3 exposures
#define IMG_SENSOR_HDR_MODE_2  (UINT32)30  //Sensor HDR, 4 exposures

/*-----------------------------------------------------------------------------------------------*\
 * AE Detail Info
 \*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT16     EvIndex;
    UINT16     NfIndex;
    INT16      ShutterIndex;
    INT16      AgcIndex;
    INT16      IrisIndex;
    INT32      Dgain;
    UINT16     IsoValue;
    UINT16     Flash;
    UINT16     Mode;
    FLOAT      ShutterTime;
    FLOAT      AgcGain;
    UINT16     Target;
    UINT16     LumaStat;
    INT16      LimitStatus;
    UINT16     Reserved[2];
    FLOAT      SensorDgain;

    FLOAT      HdrRatio;
    INT16      FogStrength;
//    UINT32     DgainInfo[4];
} AMBA_AE_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Digital Gain Info
 \*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32     DgainInfo[4];
} AMBA_DGAIN_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * AEB Detail Info
 \*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT8    Num;        // 1~5
    INT8    EvBias[5];    // unit ev is 32, +4 31/32 EV ~ -4 31/32 EV
} AMBA_AEB_INFO_s;


/*-----------------------------------------------------------------------------------------------*\
 * luma, R, G and B values of each tile
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT16 R;
    UINT16 G;
    UINT16 B;
    UINT16 Y;
    UINT16 Y256;
} AMBA_AWB_TILES_VALUE_s;

typedef struct {
    UINT16 Rows;
    UINT16 Cols;
    AMBA_AWB_TILES_VALUE_s *pTilesValue;
} AMBA_AWB_TILES_INFO_s;

typedef struct {
    UINT16 Rows;
    UINT16 Cols;
    UINT16 TilesValue[AMBA_IK_3A_AE_TILE_ROW_COUNT * AMBA_IK_3A_AE_TILE_COL_COUNT * MAX_VIEW_NUM];
} AMBA_AE_TILES_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * 3A Status Info
 \*-----------------------------------------------------------------------------------------------*/
 #define AMBA_AAA_STATUS_s AMBA_3A_STATUS_s
typedef struct {
   UINT8 Ae;
   UINT8 Awb;
   UINT8 Af;
} AMBA_3A_STATUS_s;

/*-----------------------------------------------------------------------------------------------*\
 * 3A Operation Info
 \*-----------------------------------------------------------------------------------------------*/
 #define AMBA_3A_OP_INFO_s AMBA_AAA_OP_INFO_s //FIXME (For SDK7, 3A related code is modified to AMBA_AAA_OP_INFO_s, this define is for others)
typedef struct {
    UINT8  AeOp;
    UINT8  AwbOp;
    UINT8  AfOp;
    UINT8  AdjOp;
    UINT8  Reserved;
    UINT8  Reserved1;
    UINT8  Reserved2;
    UINT8  Reserved3;
} AMBA_AAA_OP_INFO_s;

// VR 3A stat.
typedef struct {
    UINT16  LinYAvg;
}AMBA_IP_CFA_AE_s;

typedef struct {
    UINT16  RGain;
    UINT16  BGain;
}AMBA_IP_CFA_AWB_s;

typedef struct {
    UINT16  HisBinY[64];
}AMBA_IP_CFA_HISTO_s;

typedef struct {
    UINT16  HisBinG[128];
}AMBA_IP_VIN_HISTO_s;

typedef struct {
    UINT16  HisBinY[64];
}AMBA_IP_PG_HISTO_s;

typedef struct {
    UINT16     EvIndex;
    UINT16     NfIndex;
    INT32      Dgain;
    FLOAT      ShutterTime;
    FLOAT      AgcGain;
} AMBA_VR_AE_INFO_s;

typedef struct {
    UINT8                  FrameId;
    UINT8                  AeWeighting;
    UINT8                  AwbWeighting;
    AMBA_IP_CFA_AWB_s      Awb;
    AMBA_IP_CFA_AE_s      *pAe;
    AMBA_IP_CFA_HISTO_s    Histo;       //CFA
    AMBA_IP_VIN_HISTO_s    VinHisto[3]; //VIN
    AMBA_IP_PG_HISTO_s     PgHisto;     //PG
    AMBA_3A_STATUS_s       VideoStatus;
    AMBA_3A_STATUS_s       StillStatus;
}AMBA_IP_AAA_DATA_s;

// VR results
typedef struct {
    UINT8                  AeInfoNum; //
    AMBA_VR_AE_INFO_s      *pAeInfo;  //linear mode : 1, HDR mode: 2
    AMBA_IP_CFA_AWB_s      AwbGain;
    AMBA_IP_CFA_HISTO_s    Histo;       //CFA
    AMBA_IP_VIN_HISTO_s    VinHisto[3]; //VIN
    AMBA_IP_PG_HISTO_s     PgHisto;     //PG
    AMBA_3A_STATUS_s       VideoStatus;
    AMBA_3A_STATUS_s       StillStatus;
}AMBA_IP_AAA_RESULTS_s;

//MVIN
typedef struct {
    DOUBLE GainR;
    DOUBLE GainG;
    DOUBLE GainB;
}AMBA_MVIN_BL_RGB_GAIN_s;

typedef struct {
    UINT32                   StatCount;
    AMBA_IK_3A_HEADER_s      Header;
    AMBA_IK_CFA_AWB_s        Awb[AMBA_IK_3A_AWB_TILE_ROW_COUNT * AMBA_IK_3A_AWB_TILE_COL_COUNT * MAX_VIEW_NUM];
    AMBA_IK_CFA_AE_s         Ae[AMBA_IK_3A_AE_TILE_ROW_COUNT * AMBA_IK_3A_AE_TILE_COL_COUNT * MAX_VIEW_NUM];
    AMBA_IK_CFA_HISTO_s      CfaHist;
    AMBA_IK_PG_HISTO_s       PgHist;
    AMBA_IK_CFA_HIST_STAT_s  VinHist[3];//long, mid, short
    UINT32                   StatisticMap[MAX_VIEW_NUM];

}AMBA_IP_GLOBAL_AAA_DATA_s;

/*-----------------------------------------------------------------------------------------------*\
 * WB simulation Info
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT16  LumaIdx;
    UINT16  OutDoorIdx;
    UINT16  HighLightIdx;
    UINT16  LowLightIdx;
    UINT32  AwbRatio[2];
    UINT32  Reserved;
    UINT32  Reserved1;
    UINT32  Reserved2;
    UINT32  Reserved3;
} WB_SIM_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * RAW Info
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT16  OffsetX;                    /* Horizontal offset of the window */
    UINT16  OffsetY;                    /* Vertical offset of the window */
    UINT16  Width;                      /* Number of pixels per line in the window */
    UINT16  Height;                     /* Number of lines in the window */
} AMBA_IMG_RAW_WINDOW_s;

typedef struct {
    UINT16*  pBaseAddr;                  //pointer to raw buffer
    UINT16   Pitch;                      //raw buffer pitch
    UINT8    BayerPattern;               // Bayer pattern for Sensor/RGB input only.
                                        // 0:RG, 1:BG, 2:GR, 3:GB
    AMBA_IMG_RAW_WINDOW_s   OBWindow;   //OB Window position and size
} AMBA_IMG_RAW_INFO_s;




/**
 * Sensor information
 */
#define MAX_SUBCHANNEL_NUM   (4)
typedef struct {
    FLOAT ShutterTime[MAX_SUBCHANNEL_NUM];     /**< Shutter time in sec unit */
    UINT32 ShutterLine[MAX_SUBCHANNEL_NUM];    /**< Shutter time exposure line unit */
} AMBA_IMG_SENSOR_SHUTTER_INFO_s;

typedef struct {
    FLOAT AGC[MAX_SUBCHANNEL_NUM];     /**< Agc gain in db unit */
    FLOAT GainR[MAX_SUBCHANNEL_NUM];   /**< Sensor R gain in db unit */
    FLOAT GainGr[MAX_SUBCHANNEL_NUM];  /**< Sensor Gr gain in db unit */
    FLOAT GainGb[MAX_SUBCHANNEL_NUM];  /**< Sensor Gb gain in db unit */
    FLOAT GainB[MAX_SUBCHANNEL_NUM];   /**< Sensor B gain in db unit */
} AMBA_IMG_SENSOR_GAIN_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Image Proc callback function pointers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32   (* Adj_Init)(UINT32 ViewID);
    UINT32   (* QueryActualShutterTime)(UINT32 MainViewID, UINT32 ExposureFrame, const FLOAT *DesiredShutter, AMBA_IMG_SENSOR_SHUTTER_INFO_s *ActualShutter);
    UINT32   (* QueryActualGainFactor)(UINT32 MainViewID, UINT32 ExposureFrame, const AMBA_IMG_SENSOR_GAIN_INFO_s *DesiredGain, AMBA_IMG_SENSOR_GAIN_INFO_s *ActualGain);
    UINT32   (* QueryActualStrobeWidth)(UINT32 MainViewID, const UINT32 *pDesiredWidth, UINT32 *pActualWidth, UINT32 *pStrobeCtrl);
    UINT32   (* HdrDeCmpdFunc)(UINT32 Idx);
    UINT32   (* HdrCmpdFunc)(UINT32 Idx);
} IMG_PROC_FUNC_s;

/*-----------------------------------------------------------------------------------------------*\
 * ImgProc Memory Info
 \*-----------------------------------------------------------------------------------------------*/
typedef struct {
    void   *pMemAddr;
    UINT32 MemSize;
} IMG_PROC_MEM_INFO_s;

typedef struct {
    UINT32 Priority; ///< task priority
    UINT32 StackSize; ///< task stack size
    UINT32 CoreExclusiveBitMap; ///< exclusive bitmap of core. Bit[0] = 1 means core #0 is excluded
    UINT32  Reserved;
} AMBA_IMG_TSK_Info_s;

#define AMBA_DISP_ROI_MAX  256

typedef struct {
    UINT32 AvgR;
    UINT32 AvgG;
    UINT32 AvgB;
} AMBA_DISP_ROI_RGB_Avg_s;

typedef struct {
    UINT16 Status;
    UINT16 Num;
    AMBA_DISP_ROI_RGB_Avg_s ROI[AMBA_DISP_ROI_MAX];
} AMBA_DISP_ROI_RGB_STAT_s;

#define AMBA_VIG_ROI_MAX  256
typedef struct {
    INT32 TableWidth;
    INT32 TableHeight;
    UINT8 GainShift;
    UINT16 *VignetteRedGain;
    UINT16 *VignetteGreenEvenGain;
    UINT16 *VignetteGreenOddGain;
    UINT16 *VignetteBlueGain;
}AMBA_VIG_GAIN_FACTOR_s;

typedef struct {
    INT32 CenterX;
    INT32 CenterY;
} AMBA_VIG_CENTER_s;

typedef struct {
    UINT16 Num;
    AMBA_VIG_CENTER_s ROI[AMBA_VIG_ROI_MAX];
} AMBA_VIG_CENTER_ROI_s;

typedef struct {
    UINT32   (* RunTimeVig_UpdateVignette)(AMBA_VIG_GAIN_FACTOR_s **vigInfo, UINT8 fovUpdateflag);
} AMBA_RUNTIMEVIG_FUNC_s;


typedef struct {
    INT32   ArgCount;
    char **   ArgvArray;

} IMGPROC_TESTCMD_PARAM_s;

typedef struct {
    AMBA_AE_INFO_s      BeforeHdrAEInfo[MAX_EXP_GROUP_NUM];
    AMBA_AE_INFO_s      AfterHdrAEInfo[MAX_EXP_GROUP_NUM];
    AMBA_IK_WB_GAIN_s   WbGain;
    DOUBLE              ShiftAeGain;
    DOUBLE              BlendRatio;
    UINT32              MinBlendRatio;
    UINT32              MaxBlendRatio;
} AMBA_IMGPROC_OFFLINE_AAA_INFO_s;

#endif  /* _AMBA_AE_AWB_ */