/**
*  @file SvcCalibMgr.h
 *
 * Copyright (c) [2020] Ambarella International LP
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
*  @details Header file for Calibration Manager control
*
*/

#ifndef SVC_CALIB_MGR_H
#define SVC_CALIB_MGR_H

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcCalibMgr.c
\*-----------------------------------------------------------------------------------------------*/
#define SVC_LOG_CAL_OK           (0x10000U)
#define SVC_LOG_CAL_NG           (0x20000U)
#define SVC_LOG_CAL_API          (0x40000U)
#define SVC_LOG_CAL_DBG          (0x80000U)
#define SVC_LOG_CAL_DEF_FLG      (SVC_LOG_CAL_OK | SVC_LOG_CAL_NG | SVC_LOG_CAL_API)
#define SVC_LOG_CAL_HL_TITLE_0   ("\033""[38;2;255;125;38m")
#define SVC_LOG_CAL_HL_TITLE_1   ("\033""[38;2;255;255;128m")
#define SVC_LOG_CAL_HL_TITLE_2   ("\033""[38;2;168;255;255m")
#define SVC_LOG_CAL_HL_TITLE_0_U ("\033""[38;2;255;125;38m""\033""[4m")
#define SVC_LOG_CAL_HL_TITLE_1_U ("\033""[38;2;255;255;128m""\033""[4m")
#define SVC_LOG_CAL_HL_TITLE_2_U ("\033""[38;2;168;255;255m""\033""[4m")
#define SVC_LOG_CAL_HL_NUM       ("\033""[38;2;153;217;234m")
#define SVC_LOG_CAL_HL_STR       ("\033""[38;2;255;174;201m")
#define SVC_LOG_CAL_HL_DEF_FC    ("\033""[39m")
#define SVC_LOG_CAL_HL_END       ("\033""[0m")

typedef struct {
    UINT32 VinID;
    UINT32 SensorID;
    UINT32 VinSelectBits;
    UINT32 SensorSelectBits;
    UINT32 ExtendData;
    UINT32 FmtID;
    UINT32 FovID;
    UINT32 FmtSelectBits;
    UINT32 FovSelectBits;
} SVC_CALIB_CHANNEL_s;

typedef struct {
    UINT32 StartX;        // Unit in pixel. Before downsample.
    UINT32 StartY;        // Unit in pixel. Before downsample.
    UINT32 Width;         // Unit in pixel. After downsample.
    UINT32 Height;        // Unit in pixel. After downsample.
    struct {
        UINT32 FactorNum; // subsamping factor (numerator)
        UINT32 FactorDen; // subsamping factor (denominator)
    } HSubSample;
    struct {
        UINT32 FactorNum; // subsamping factor (numerator)
        UINT32 FactorDen; // subsamping factor (denominator)
    } VSubSample;
} SVC_CALIB_VIN_SENSOR_GEO_s;

typedef struct {
    UINT32 GainR;
    UINT32 GainG;
    UINT32 GainB;
} SVC_CALIB_GAIN_INFO_s;

#define SVC_CALIB_EMPTY        (0U)
#define SVC_CALIB_VIGNETTE_ID  (1U)
#define SVC_CALIB_LDC_ID       (2U)
#define SVC_CALIB_CA_ID        (3U)
#define SVC_CALIB_BPC_ID       (4U)
#define SVC_CALIB_WB_ID        (5U)
#define SVC_CALIB_MAX_OBJ_NUM (16U)

typedef UINT32 (*SVC_CALIB_CMD_FUNC_f)(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4);
typedef UINT32 (*SVC_CALIB_SHELL_ENTRY_f)(UINT32 ArgCount, char * const *pArgVector);

typedef struct {
    UINT32                          Enable;
    UINT32                          ID;
    char                            Name[16];
    UINT32                          Version;
#define SVC_CALIB_MAX_TABLE_NUM     (32U)
    UINT32                          NumOfTable;
#define SVC_CALIB_DATA_SOURCE_NAND     (0x0U)
#define SVC_CALIB_DATA_SOURCE_ROMFS    (0x1U)
#define SVC_CALIB_DATA_SOURCE_FILE     (0x2U)
#define SVC_CALIB_DATA_SOURCE_USER     (0x3U)
    UINT32                          DataSource;
    char                            DataSourcePath[64];
    UINT8                          *pShadowBuf;
    UINT32                          ShadowBufSize;
    UINT8                          *pWorkingBuf;
    UINT32                          WorkingBufSize;
    UINT32                          CalcBufSize;
    const SVC_CALIB_CMD_FUNC_f      CmdFunc;
    const SVC_CALIB_SHELL_ENTRY_f   ShellEntry;
} SVC_CALIB_OBJ_s;

#define SVC_CALIB_CALC_START        (0U)
#define SVC_CALIB_CALC_DONE         (1U)
#define SVC_CALIB_CALC_WB_INFO      (2U)
#define SVC_CALIB_CALC_MEM_LOCK     (3U)
#define SVC_CALIB_CALC_MEM_UNLOCK   (4U)
#define SVC_CALIB_CALC_STG_DRIVER   (5U)
#define SVC_CALIB_CALC_CMD_NUM      (6U)

typedef struct {
    SVC_CALIB_CHANNEL_s            CalibChan;
    UINT32                         WbIndex;
    UINT32                         FlickerMode;
    SVC_CALIB_GAIN_INFO_s          CurGain;
} SVC_CALIB_CALC_WB_INFO_s;

typedef struct {
    UINT32 ReqMemNum;
#define SVC_CALIB_CALC_MEM_MAX_NUM  (2U)
    UINT32 ReqMemType[SVC_CALIB_CALC_MEM_MAX_NUM];
    UINT8 *pReqMem[SVC_CALIB_CALC_MEM_MAX_NUM];
    UINT32 ReqMemSize[SVC_CALIB_CALC_MEM_MAX_NUM];
} SVC_CALIB_CALC_MEM_INFO_s;

typedef struct {
    char MainStgDriver;
} SVC_CALIB_CALC_STORAGE_DRIVER_s;

typedef UINT32 (*SVC_CALIB_CALC_CB_f)(UINT32 CalID, UINT32 CalcCmd, void *pInfo);

typedef struct {
    UINT32              NumOfCalObj;
    SVC_CALIB_OBJ_s    *pCalObj;
    UINT32              CalcBufSize;
    UINT8              *pCalcBuf;
    SVC_CALIB_CALC_CB_f CalcCbFunc;
} SVC_CALIB_CFG_s;

typedef struct {
    SVC_CALIB_CHANNEL_s CalChan;
    void               *pBuf;
    UINT32              BufSize;
    UINT32              DbgMsgOn;
} SVC_CALIB_TBL_INFO_s;

#define SVC_CALIB_CMPR_NONE   (0x0U)
#define SVC_CALIB_CMPR_LZ77   (0x1U)

typedef struct {
    UINT32  CmprType;               // Compression type
    void   *pSource;                // Source data
    UINT32  SourceSize;             // Source data size
    void   *pDestination;           // Destination buffer
    UINT32  DestinationSize;        // Destination buffer size
    UINT32 *pCmprSize;              // Output compression size
    struct {
        UINT32 SearchWin;       // Lz77 max search window size
    } Lz77;
} SVC_CALIB_COMPRESS_s;

typedef struct {
    UINT32 CmprType;                // Compression type
    void   *pSource;                // Source data - compression data
    UINT32  SourceSize;             // Source data size
    void   *pDestination;           // Destination buffer
    UINT32  DestinationSize;        // Destination buffer size
    UINT32 *pDeCmprSize;            // Output de-compression size
} SVC_CALIB_DECOMPRESS_s;

#define SVC_CALIB_CMD_MEM_QUERY         (1U)
#define SVC_CALIB_CMD_DATA_LOAD         (2U)
#define SVC_CALIB_CMD_ITEM_CREATE       (3U)
#define SVC_CALIB_CMD_ITEM_DATA_INIT    (4U)
#define SVC_CALIB_CMD_ITEM_TBL_CFG      (5U)
#define SVC_CALIB_CMD_ITEM_TBL_UPDATE   (6U)
#define SVC_CALIB_CMD_ITEM_TBL_GET      (7U)
#define SVC_CALIB_CMD_ITEM_TBL_SET      (8U)
#define SVC_CALIB_CMD_NUM               (9U)

UINT32 SvcCalib_MemQry(SVC_CALIB_CFG_s *pCfg);
UINT32 SvcCalib_Create(const SVC_CALIB_CFG_s *pCfg);
UINT32 SvcCalib_DataLoad(UINT32 CalID);
UINT32 SvcCalib_DataLoadAll(void);
UINT32 SvcCalib_DataSave(UINT32 CalID);
UINT32 SvcCalib_ItemDataInit(UINT32 CalID);
UINT32 SvcCalib_ItemDataInitAll(void);
UINT32 SvcCalib_ItemTableCfg(UINT32 CalID, UINT32 CmdID, const void *pCfg);
UINT32 SvcCalib_ItemTableUpdate(UINT32 CalID, const SVC_CALIB_CHANNEL_s *pCalChan, const AMBA_IK_MODE_CFG_s *pImgMode);
UINT32 SvcCalib_ItemTableGet(UINT32 CalID, UINT32 TblId, SVC_CALIB_TBL_INFO_s *pTblInfo);
UINT32 SvcCalib_ItemTableSet(UINT32 CalID, UINT32 TblId, SVC_CALIB_TBL_INFO_s *pTblInfo);
UINT32 SvcCalib_ItemCalcMemSet(UINT8 *pBuf, UINT32 BufSize);
UINT32 SvcCalib_ItemCalcMemGet(UINT32 CalID, UINT32 ReqNum, UINT8 **pBuf, UINT32 *pBufSize);
UINT32 SvcCalib_ItemCalcMemFree(UINT32 CalID);
UINT32 SvcCalib_ObjGet(UINT32 CalID, SVC_CALIB_OBJ_s *pCalObj);
UINT32 SvcCalib_DataCompress(SVC_CALIB_COMPRESS_s *pCtrl);
UINT32 SvcCalib_DataDecompress(SVC_CALIB_DECOMPRESS_s *pCtrl);
void   SvcCalib_CmdAppEntry(UINT32 ArgCount, char * const *pArgVector);

static inline UINT32 SvcCalib_BitGet(UINT32 Val) { return ((Val < 32U) ? (UINT32)(0x1UL << Val) : 0U); }

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcCalibLdc.c
\*-----------------------------------------------------------------------------------------------*/

#define SVC_CALIB_LDC_UPD_BY_VIN_SS     (0U)
#define SVC_CALIB_LDC_UPD_BY_FMT_FOV    (1U)

typedef struct {
    UINT32                     HorGridNum;
    UINT32                     VerGridNum;
    UINT32                     TileWidthExp;
    UINT32                     TileHeightExp;
    SVC_CALIB_VIN_SENSOR_GEO_s VinSensorGeo;
    AMBA_IK_GRID_POINT_s      *pLdcVector;
    UINT32                     LdcVectorLength;
} SVC_CALIB_LDC_TBL_DATA_s;

UINT32 SvcCalib_LdcCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4);
UINT32 SvcCalib_LdcShellFunc(UINT32 ArgCount, char * const *pArgVector);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcCalibCa.c
\*-----------------------------------------------------------------------------------------------*/

typedef struct {
    UINT32                     HorGridNum;
    UINT32                     VerGridNum;
    UINT32                     TileWidthExp;
    UINT32                     TileHeightExp;
    SVC_CALIB_VIN_SENSOR_GEO_s VinSensorGeo;
    AMBA_IK_GRID_POINT_s      *pCaRed;
    UINT32                     CaRedLength;
    AMBA_IK_GRID_POINT_s      *pCaBlue;
    UINT32                     CaBlueLength;
} SVC_CALIB_CA_TBL_DATA_s;

UINT32 SvcCalib_CaCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4);
UINT32 SvcCalib_CaShellFunc(UINT32 ArgCount, char * const *pArgVector);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcCalibVig.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcCalib_VigCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4);
UINT32 SvcCalib_VigShellFunc(UINT32 ArgCount, char * const *pArgVector);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcCalibBpc.c
\*-----------------------------------------------------------------------------------------------*/

typedef struct {
    UINT32 Version;
    AMBA_IK_VIN_SENSOR_GEOMETRY_s CalibGeo;
    UINT8 *pSbpTbl;
    UINT32 SbpTblSize;
    UINT32 SbpCount;
    UINT32 SensorMode;
    UINT32 DataType;
} SVC_CALIB_BPC_TBL_DATA_s;

UINT32 SvcCalib_BpcCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4);
UINT32 SvcCalib_BpcShellFunc(UINT32 ArgCount, char * const *pArgVector);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcCalibWb.c
\*-----------------------------------------------------------------------------------------------*/
#define SVC_CALIB_WB_LCT         (0U)
#define SVC_CALIB_WB_HCT         (1U)
#define SVC_CALIB_WB_CT_NUM      (2U)

typedef struct {
    SVC_CALIB_GAIN_INFO_s OriGain[SVC_CALIB_WB_CT_NUM];
    SVC_CALIB_GAIN_INFO_s RefGain[SVC_CALIB_WB_CT_NUM];
} SVC_CALIB_WB_TBL_DATA_s;

UINT32 SvcCalib_WbCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4);
UINT32 SvcCalib_WbShellFunc(UINT32 ArgCount, char * const *pArgVector);

#endif /* SVC_CALIB_MGR_H */
