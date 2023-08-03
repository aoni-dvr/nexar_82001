/**
 *  @file SvcWarp.c
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
 *  @details Implementation of Warp Table
 *
 */


#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaWrap.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaSTU_IF.h"

#include "SvcErrCode.h"
#include "SvcResCfg.h"
#include "SvcWarp.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcPlat.h"
#include "AmbaCache.h"
#include "AmbaMisraFix.h"


#define SVC_VERSION           (0x20180401UL)
#define SVC_TILEWIDTH         (32U)
#define SVC_TILEHEIGHT        (32U)
#define SVC_TILEWIDTHEXP      (5U)
#define SVC_TILEHEIGHTEXP     (5U)
#define SVC_DUMP_WARPINFO     (1U)

#define SVC_MODULE_CONTROL_MAX_SENSOR_NUM     (18U)
#define SVC_WARP_REMAP_BUF_SIZE               (393984U)
#define SVC_MAX_FOV                           (2U)


static SVC_WARP_TABLE_s WarpTableInfo[SVC_MODULE_CONTROL_MAX_SENSOR_NUM];
static AMBA_IK_WARP_INFO_s WarpInfo;

UINT32 SvcWarp_QueryWarpSize(UINT32 *pMemSize)
{
    UINT32 WorkingSize;
    UINT32 WarpSize;

    WorkingSize = SVC_WARP_REMAP_BUF_SIZE;
    WarpSize = sizeof(AMBA_IK_GRID_POINT_s) * MAX_WARP_TBL_LEN * 2U;
    *pMemSize = WorkingSize + WarpSize;

    return SVC_OK;
}

static UINT32 SvcWarp_BufferInit(ULONG  *BufBaseWarp, UINT32 *BufSizeWorking, ULONG  *BufBaseWorking)
{
    UINT32 RetVal;
    UINT32 TotalSize;
    ULONG BufferBase;

    RetVal = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_STEREO_WARP, &BufferBase, &TotalSize);
    if (SVC_OK != RetVal) {
        AmbaPrint_PrintUInt5("SvcBuffer_Request(SMEM_PF0_ID_STEREO_WARP) failed", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaMisra_TypeCast(BufBaseWorking, &BufferBase);
    *BufSizeWorking = SVC_WARP_REMAP_BUF_SIZE;
    BufBaseWarp[0] = BufferBase + SVC_WARP_REMAP_BUF_SIZE;
    BufBaseWarp[1] = BufBaseWarp[0] + (ULONG)(sizeof(AMBA_IK_GRID_POINT_s) * MAX_WARP_TBL_LEN) ;

    return RetVal;
}

static void DumpWarpInfo(UINT32 FovID, const AMBA_IK_WARP_INFO_s *Info)
{
    AmbaPrint_PrintUInt5("*****Calibration Warp Table*****", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("version: 0x%x", Info->Version, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("ContextId(FovID) = %d ", FovID, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("horizontal grid number: %d", Info->HorGridNum, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("vertical grid number: %d", Info->VerGridNum, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("TileWidthExp: %d", Info->TileWidthExp, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("TileHeightExp: %d", Info->TileHeightExp, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Current VIN width: %d", Info->VinSensorGeo.Width, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Current VIN height: %d", Info->VinSensorGeo.Height, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Current VIN StartX: %d", Info->VinSensorGeo.StartX, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Current VIN StartY: %d", Info->VinSensorGeo.StartY, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Current VIN HSubSample FactorDen: %d", Info->VinSensorGeo.HSubSample.FactorDen, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Current VIN HSubSample FactorNum: %d", Info->VinSensorGeo.HSubSample.FactorNum, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Current VIN VSubSample FactorDen: %d", Info->VinSensorGeo.VSubSample.FactorDen, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Current VIN VSubSample FactorNum: %d", Info->VinSensorGeo.VSubSample.FactorNum, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Enable 2 Stage Compensation: %d", Info->Enb_2StageCompensation, 0U, 0U, 0U, 0U);
}

static UINT32 OpenDefaultWarpTable(const char *pFileName, UINT32 FovID, UINT32 WarpSize)
{
    AMBA_FS_FILE *pFile = NULL;
    AMBA_FS_FILE_INFO_s FileStat;
    UINT32 DataSize;
    UINT32 Rval;

    /* check if warp file exists */
    Rval = AmbaFS_GetFileInfo(pFileName, &FileStat);
    if (Rval != AMBA_FS_ERR_NONE) {
        AmbaPrint_PrintStr5("%s: warp file %s doesn't exist!",__func__, pFileName, NULL, NULL, NULL);
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        /* Open warp binary */
        Rval = AmbaFS_FileOpen(pFileName, "r", &pFile);
        if (pFile == NULL) {
             AmbaPrint_PrintStr5("%s: AmbaFS_FileOpen(%s) fail",__func__, pFileName, NULL, NULL, NULL);
             Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        Rval = AmbaFS_FileRead(&WarpTableInfo[FovID].table[0], 1, WarpSize, pFile, &DataSize);
        if (DataSize != WarpSize){
            AmbaPrint_PrintUInt5("can't read correct warp table.  size:%d, read:%d", WarpSize, DataSize, 0U, 0U, 0U);
            Rval = SVC_NG;
        } else {
            AmbaPrint_PrintUInt5("read warp table.  size:%d, read:%d", WarpSize, DataSize, 0U, 0U, 0U);
            Rval = SVC_OK;
        }

        Rval = AmbaFS_FileClose(pFile);
    }

    return Rval;
}

static UINT32 OpenWarpTableWithHeader(const char *pFileName, UINT32 FovID)
{
    AMBA_FS_FILE *pFile = NULL;
    AMBA_FS_FILE_INFO_s FileStat;
    UINT32 DataSize;
    UINT32 Size;
    UINT32 SizeCnt = 0;
    UINT32 Rval;

    /* check if warp file exists */
    Rval = AmbaFS_GetFileInfo(pFileName, &FileStat);
    if (Rval != AMBA_FS_ERR_NONE) {
        AmbaPrint_PrintStr5("%s: warp file %s doesn't exist!",__func__, pFileName, NULL, NULL, NULL);
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        /* Open warp binary */
        Rval = AmbaFS_FileOpen(pFileName, "r", &pFile);
        if (pFile == NULL) {
             AmbaPrint_PrintStr5("%s: AmbaFS_FileOpen(%s) fail",__func__, pFileName, NULL, NULL, NULL);
             Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        Size = sizeof(WarpTableInfo[FovID].header_base.ver);
        Rval = AmbaFS_FileRead(WarpTableInfo[FovID].header_base.ver, 1, Size, pFile, &DataSize);
        SizeCnt = SizeCnt + Size;
        Size = sizeof(WarpTableInfo[FovID].header_base.header_size);
        Rval = AmbaFS_FileRead(&WarpTableInfo[FovID].header_base.header_size, 1, Size, pFile, &DataSize);
        SizeCnt = SizeCnt + Size;
        Size = sizeof(WarpTableInfo[FovID].header_base.table_size);
        Rval = AmbaFS_FileRead(&WarpTableInfo[FovID].header_base.table_size, 1, Size, pFile, &DataSize);
        SizeCnt = SizeCnt + Size;
        Size = sizeof(WarpTableInfo[FovID].header_spec.horizontal_grid_number);
        Rval = AmbaFS_FileRead(&WarpTableInfo[FovID].header_spec.horizontal_grid_number, 1, Size, pFile, &DataSize);
        SizeCnt = SizeCnt + Size;
        Size = sizeof(WarpTableInfo[FovID].header_spec.vertical_grid_number);
        Rval = AmbaFS_FileRead(&WarpTableInfo[FovID].header_spec.vertical_grid_number, 1, Size, pFile, &DataSize);
        SizeCnt = SizeCnt + Size;
        Size = sizeof(WarpTableInfo[FovID].header_spec.tile_width_q16);
        Rval = AmbaFS_FileRead(&WarpTableInfo[FovID].header_spec.tile_width_q16, 1, Size, pFile, &DataSize);
        SizeCnt = SizeCnt + Size;
        Size = sizeof(WarpTableInfo[FovID].header_spec.tile_height_q16);
        Rval = AmbaFS_FileRead(&WarpTableInfo[FovID].header_spec.tile_height_q16, 1, Size, pFile, &DataSize);
        SizeCnt = SizeCnt + Size;
        Size = sizeof(WarpTableInfo[FovID].header_spec.warp_mode);
        Rval = AmbaFS_FileRead(&WarpTableInfo[FovID].header_spec.warp_mode, 1, Size, pFile, &DataSize);
        SizeCnt = SizeCnt + Size;
        Size = sizeof(WarpTableInfo[FovID].header_spec.id);
        Rval = AmbaFS_FileRead(&WarpTableInfo[FovID].header_spec.id, 1, Size, pFile, &DataSize);
        SizeCnt = SizeCnt + Size;
        Size = sizeof(WarpTableInfo[FovID].header_spec.hash);
        Rval = AmbaFS_FileRead(&WarpTableInfo[FovID].header_spec.hash, 1, Size, pFile, &DataSize);
        SizeCnt = SizeCnt + Size;
        Size = sizeof(WarpTableInfo[FovID].header_spec.table_origin_x);
        Rval = AmbaFS_FileRead(&WarpTableInfo[FovID].header_spec.table_origin_x, 1, Size, pFile, &DataSize);
        SizeCnt = SizeCnt + Size;
        Size = sizeof(WarpTableInfo[FovID].header_spec.table_origin_y);
        Rval = AmbaFS_FileRead(&WarpTableInfo[FovID].header_spec.table_origin_y, 1, Size, pFile, &DataSize);
        SizeCnt = SizeCnt + Size;
        Size = sizeof(WarpTableInfo[FovID].header_spec.vsync_delay);
        Rval = AmbaFS_FileRead(&WarpTableInfo[FovID].header_spec.vsync_delay, 1, Size, pFile, &DataSize);
        SizeCnt = SizeCnt + Size;

#if SVC_DUMP_WARPINFO
        AmbaPrint_PrintStr5("%s: WARP File: %s",__func__, pFileName, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("header_base.ver: %d.%d.%d", WarpTableInfo[FovID].header_base.ver[0], WarpTableInfo[FovID].header_base.ver[1], WarpTableInfo[FovID].header_base.ver[2], 0U, 0U);
        AmbaPrint_PrintUInt5("header_base.header_size: %d", WarpTableInfo[FovID].header_base.header_size, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("header_base.table_size: %d", WarpTableInfo[FovID].header_base.table_size, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("header_spec.horizontal_grid_number: %d", WarpTableInfo[FovID].header_spec.horizontal_grid_number, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("header_spec.vertical_grid_number: %d", WarpTableInfo[FovID].header_spec.vertical_grid_number, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("header_spec.tile_width_q16: %d", WarpTableInfo[FovID].header_spec.tile_width_q16, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("header_spec.tile_height_q16: %d", WarpTableInfo[FovID].header_spec.tile_height_q16, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("header_spec.enb_2stage_compensation: %d", WarpTableInfo[FovID].header_spec.warp_mode.en_2stage_compensation, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("header_spec.id: %d", WarpTableInfo[FovID].header_spec.id, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("header_spec.hash: %d", WarpTableInfo[FovID].header_spec.hash, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("header_spec.table_origin_x: %d", WarpTableInfo[FovID].header_spec.table_origin_x, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("header_spec.table_origin_y: %d", WarpTableInfo[FovID].header_spec.table_origin_y, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("header_spec.vsync_delay: %d", WarpTableInfo[FovID].header_spec.vsync_delay, 0U, 0U, 0U, 0U);
#endif

        if(WarpTableInfo[FovID].header_base.header_size != SizeCnt) {
            AmbaPrint_PrintUInt5("Warp table header size %d is not correct %d", WarpTableInfo[FovID].header_base.header_size, SizeCnt, 0U, 0U, 0U);
            Rval = SVC_NG;
        }

        if (WarpTableInfo[FovID].header_base.table_size > SVC_TABLE_MAXSIZE) {
            AmbaPrint_PrintUInt5("Warp table size %d > %d", WarpTableInfo[FovID].header_base.table_size, SVC_TABLE_MAXSIZE, 0U, 0U, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        Rval = AmbaFS_FileRead(&WarpTableInfo[FovID].table[0], 1, WarpTableInfo[FovID].header_base.table_size, pFile, &DataSize);
        if (DataSize != WarpTableInfo[FovID].header_base.table_size){
            AmbaPrint_PrintUInt5("can't read correct warp table.  size:%d, read:%d", WarpTableInfo[FovID].header_base.table_size, DataSize, 0U, 0U, 0U);
            Rval = SVC_NG;
        } else {
            Rval = SVC_OK;
        }
        Rval = AmbaFS_FileClose(pFile);
    }

    return Rval;
}

#if 0
static UINT32 IK_DumpBufferBin(const UINT8 *pDumpAddress, const char *FileName, UINT32 Size)
{
    UINT32  Rval = IK_OK;
    AMBA_FS_FILE *Fid;
    const char *Fmode = "wb";
    UINT32 NumSuccess;
    UINT64 AddrDumpAddressU64;
    void* pDumpAddrVoid;

    Rval = AmbaWrap_memcpy(&AddrDumpAddressU64, &pDumpAddress, sizeof(UINT8*));
    Rval = AmbaWrap_memcpy(&pDumpAddrVoid, &pDumpAddress, sizeof(void*));
    if ((AddrDumpAddressU64 == 0x0ULL) || (AddrDumpAddressU64 == 0xFFFFFFFFULL)) {
        AmbaPrint_PrintStr5("[IK_TCMD] The File %s dump fail.", FileName, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("              Dump from %p fail, please check the flow or DSP log", (UINT32) AddrDumpAddressU64, 0U, 0U, 0U, 0U);
    } else {

        // invald cache
        Rval = SvcPlat_CacheInvalidate((UINT32)AddrDumpAddressU64, Size);

        Rval = AmbaFS_FileOpen(FileName, Fmode, &Fid);
        if (Fid == NULL ) {
            AmbaPrint_PrintStr5("[IK_TCMD]File open fail. Skip dumping debug data %s", FileName, NULL, NULL, NULL, NULL);
        } else {
            Rval = AmbaFS_FileWrite(pDumpAddrVoid, sizeof(UINT8), Size, Fid, &NumSuccess);

            if ((Rval != IK_OK) || (NumSuccess != Size)) {
                AmbaPrint_PrintStr5("[IK_TCMD] File write fail. The content in file:%s is not reliable.", FileName, NULL, NULL, NULL, NULL);
            }
            Rval = AmbaFS_FileClose(Fid);
        }
    }

    return Rval;
}
#endif

static UINT32 ApplyWarpTable(UINT32 FovID, UINT32 Enable, UINT32 Width, UINT32 Height, UINT32 HGdNum, UINT32 VGdNum, UINT32 TileWExp, UINT32 TileHExp, AMBA_IK_GRID_POINT_s *pWarpTbl)
{
    AMBA_IK_MODE_CFG_s ImgMode = {0};
    AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo;
    UINT32 Rval = SVC_OK;
    UINT32 ActiveWidth = 0U, ActiveHeight = 0U, HorGridNum = 0U, VerGridNum = 0U, TileWidthExp = 0U, TileHeightExp = 0U;
    UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum = 0U;

    AmbaMisra_TouchUnused(pWarpTbl);

    Rval = AmbaWrap_memset(&WindowSizeInfo, 0, sizeof(AMBA_IK_WINDOW_SIZE_INFO_s));

    if (pWarpTbl == NULL) {
        AmbaPrint_PrintStr5("%s: pWarpTbl is NULL!!",__func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        Rval = SvcResCfg_GetFovIdxs(FovIdxs, &FovNum);
        if (SVC_OK != Rval) {
            AmbaPrint_PrintStr5( "%s ## fail to get fov idx and fov num", __func__, NULL, NULL, NULL, NULL);
        }
    }

    if (Rval == SVC_OK) {
        if (FovID >= FovNum) {
            AmbaPrint_PrintUInt5("ApplyWarpTable: Invalid ViewzoneId %u", FovID, 0U, 0U, 0U, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        ImgMode.ContextId = FovID;
        Rval = AmbaIK_GetWindowSizeInfo(&ImgMode, &WindowSizeInfo);
        if ((Width == 0U) && (Height == 0U)){
            ActiveWidth = WindowSizeInfo.VinSensor.Width;
            ActiveHeight = WindowSizeInfo.VinSensor.Height;
        } else {
            ActiveWidth = Width;
            ActiveHeight = Height;
        }

        if((TileWExp == 0U) && (TileHExp == 0U)){
            TileWidthExp    = SVC_TILEWIDTHEXP;
            TileHeightExp   = SVC_TILEHEIGHTEXP;
        } else {
            TileWidthExp    = TileWExp;
            TileHeightExp   = TileHExp;
        }

        if ((HGdNum == 0U) && (VGdNum == 0U)){
            HorGridNum = ((ActiveWidth + SVC_TILEWIDTH - 1U) >> TileWidthExp) + 1U;
            VerGridNum = ((ActiveHeight + SVC_TILEHEIGHT - 1U) >> TileHeightExp) + 1U;
        } else {
            HorGridNum = HGdNum;
            VerGridNum = VGdNum;
        }
    }

    if (Rval == SVC_OK) {
        //collect warpinfo
        if (Enable == 1U) {
#ifndef CONFIG_SOC_CV2FS
            ULONG  BufBaseWarp[SVC_MAX_FOV] = {0U};
            UINT32 BufSizeWorking = 0U;
            ULONG  BufBaseWorking = 0U;
            void *pWarpBuf;

            Rval = SvcWarp_BufferInit(BufBaseWarp, &BufSizeWorking, &BufBaseWorking);
            if ((Rval == SVC_OK) && ((HorGridNum * VerGridNum) < MAX_WARP_TBL_LEN)) {
                AmbaMisra_TypeCast(&pWarpBuf, &BufBaseWarp[FovID]);
                Rval = AmbaWrap_memcpy(pWarpBuf, pWarpTbl, (sizeof(AMBA_IK_GRID_POINT_s) * HorGridNum * VerGridNum));

                WarpInfo.Version                            = SVC_VERSION;
                WarpInfo.HorGridNum                         = HorGridNum;
                WarpInfo.VerGridNum                         = VerGridNum;
                WarpInfo.TileWidthExp                       = TileWidthExp;
                WarpInfo.TileHeightExp                      = TileHeightExp;
                WarpInfo.VinSensorGeo.Width                 = ActiveWidth;
                WarpInfo.VinSensorGeo.Height                = ActiveHeight;
                WarpInfo.VinSensorGeo.StartX                = (UINT32)WindowSizeInfo.VinSensor.StartX;
                WarpInfo.VinSensorGeo.StartY                = (UINT32)WindowSizeInfo.VinSensor.StartY;
                WarpInfo.VinSensorGeo.HSubSample.FactorDen  = WindowSizeInfo.VinSensor.HSubSample.FactorNum;
                WarpInfo.VinSensorGeo.HSubSample.FactorNum  = WindowSizeInfo.VinSensor.HSubSample.FactorDen;
                WarpInfo.VinSensorGeo.VSubSample.FactorDen  = WindowSizeInfo.VinSensor.VSubSample.FactorNum;
                WarpInfo.VinSensorGeo.VSubSample.FactorNum  = WindowSizeInfo.VinSensor.VSubSample.FactorDen;
                WarpInfo.Enb_2StageCompensation             = 1U;
                AmbaMisra_TypeCast(&WarpInfo.pWarp, &pWarpBuf);
            } else {
                Rval = SVC_NG;
            }
#else
            {
                AMBA_IK_WARP_INFO_s InputInfo, ResultInfo;
                AMBA_IK_IN_WARP_PRE_PROC_s TwoStageProcIn;
                AMBA_IK_OUT_WARP_PRE_PROC_s TwoStageProcOut;
                //const AMBA_IK_GRID_POINT_s *pWarpTable= NULL;
                ULONG  BufBaseWarp[SVC_MAX_FOV] = {0U};
                UINT32 BufSizeWorking = 0U;
                ULONG  BufBaseWorking = 0U;

                Rval = SvcWarp_BufferInit(BufBaseWarp, &BufSizeWorking, &BufBaseWorking);
                if ((Rval == SVC_OK) && ((HorGridNum * VerGridNum) < MAX_WARP_TBL_LEN)) {
                    Rval = AmbaWrap_memset(&InputInfo, 0, sizeof(InputInfo));
                    InputInfo.Version                           = SVC_VERSION;
                    InputInfo.HorGridNum                        = HorGridNum;
                    InputInfo.VerGridNum                        = VerGridNum;
                    InputInfo.TileWidthExp                      = TileWidthExp;
                    InputInfo.TileHeightExp                     = TileHeightExp;
                    InputInfo.VinSensorGeo.StartX               = (UINT32)WindowSizeInfo.VinSensor.StartX;
                    InputInfo.VinSensorGeo.StartY               = (UINT32)WindowSizeInfo.VinSensor.StartY;
                    InputInfo.VinSensorGeo.Width                = ActiveWidth;
                    InputInfo.VinSensorGeo.Height               = ActiveHeight;
                    InputInfo.VinSensorGeo.HSubSample.FactorDen = WindowSizeInfo.VinSensor.HSubSample.FactorDen;
                    InputInfo.VinSensorGeo.HSubSample.FactorNum = WindowSizeInfo.VinSensor.HSubSample.FactorNum;
                    InputInfo.VinSensorGeo.VSubSample.FactorDen = WindowSizeInfo.VinSensor.VSubSample.FactorDen;
                    InputInfo.VinSensorGeo.VSubSample.FactorNum = WindowSizeInfo.VinSensor.VSubSample.FactorNum;
                    InputInfo.Enb_2StageCompensation       = 1U;
                    InputInfo.pWarp                             = pWarpTbl;
                    Rval = AmbaWrap_memset(&TwoStageProcIn, 0, sizeof(TwoStageProcIn));
                    TwoStageProcIn.pInputInfo        = &InputInfo;

                    AmbaMisra_TypeCast(&(TwoStageProcIn.pWorkingBuffer), &(BufBaseWorking));
                    TwoStageProcIn.WorkingBufferSize = BufSizeWorking;

                    Rval = AmbaWrap_memset(&ResultInfo, 0, sizeof(ResultInfo));

                    if (BufBaseWarp[FovID] == 0U) {
                        AmbaPrint_PrintUInt5("BufBaseWarp is NULL", 0U, 0U, 0U, 0U, 0U);
                    } else {
                        AmbaPrint_PrintUInt5("BufBaseWarp[%d] = 0x%x", FovID, BufBaseWarp[FovID], 0U, 0U, 0U);
                    }

                    AmbaMisra_TypeCast(&(ResultInfo.pWarp), &(BufBaseWarp[FovID]));

                    Rval = AmbaWrap_memset(&TwoStageProcOut, 0, sizeof(TwoStageProcOut));
                    TwoStageProcOut.pResultInfo = &ResultInfo;

                    Rval = AmbaIK_PreProcWarpTbl(&TwoStageProcIn, &TwoStageProcOut);

                    if (Rval != 0U) {
                        AmbaPrint_PrintUInt5("Fail to calc ldc table - two stage remap fail! ErrCode(0x%08x)", Rval, 0U, 0U, 0U, 0U);
                        Rval = SVC_NG;
                    } else {
                        AmbaPrint_PrintUInt5("Two stage re-map done!", 0U, 0U, 0U, 0U, 0U);

                        WarpInfo.Version                            = SVC_VERSION;
                        WarpInfo.HorGridNum                         = ResultInfo.HorGridNum;
                        WarpInfo.VerGridNum                         = ResultInfo.VerGridNum;
                        WarpInfo.TileWidthExp                       = ResultInfo.TileWidthExp;
                        WarpInfo.TileHeightExp                      = ResultInfo.TileHeightExp;
                        WarpInfo.VinSensorGeo.Width                 = ResultInfo.VinSensorGeo.Width;
                        WarpInfo.VinSensorGeo.Height                = ResultInfo.VinSensorGeo.Height;
                        WarpInfo.VinSensorGeo.StartX                = (UINT32)ResultInfo.VinSensorGeo.StartX;
                        WarpInfo.VinSensorGeo.StartY                = (UINT32)ResultInfo.VinSensorGeo.StartY;
                        WarpInfo.VinSensorGeo.HSubSample.FactorDen  = ResultInfo.VinSensorGeo.HSubSample.FactorDen;
                        WarpInfo.VinSensorGeo.HSubSample.FactorNum  = ResultInfo.VinSensorGeo.HSubSample.FactorNum;
                        WarpInfo.VinSensorGeo.VSubSample.FactorDen  = ResultInfo.VinSensorGeo.VSubSample.FactorDen;
                        WarpInfo.VinSensorGeo.VSubSample.FactorNum  = ResultInfo.VinSensorGeo.VSubSample.FactorNum;
                        WarpInfo.Enb_2StageCompensation             = 0;
                        WarpInfo.pWarp                              = ResultInfo.pWarp;

                        Rval = SVC_OK;
                    }
                } else {
                    Rval = SVC_NG;
                }
            }
#endif
        } else {
            WarpInfo.Version                            = 0xDEADBEEFU;
        }

        //apply warpinfo
        if ((Rval == SVC_OK) && (Enable == 1U)) {
            AmbaPrint_PrintUInt5("Enable WARP", 0U, 0U, 0U, 0U, 0U);
#if SVC_DUMP_WARPINFO
            DumpWarpInfo(FovID, &WarpInfo);
#endif
            Rval = AmbaIK_SetWarpInfo(&ImgMode, &WarpInfo);
            if (Rval != SVC_OK) {
                AmbaPrint_PrintUInt5("SetWarpInfo Fail(%d)", Rval, 0U, 0U, 0U, 0U);
            }

            Rval = AmbaIK_SetWarpEnb(&ImgMode, Enable);
            if (Rval != SVC_OK) {
                AmbaPrint_PrintUInt5("SetWarpEnb Fail(%d)", Rval, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("Disable WARP", 0U, 0U, 0U, 0U, 0U);
            Rval = AmbaIK_SetWarpEnb(&ImgMode, Enable);
            if (Rval != SVC_OK) {
                AmbaPrint_PrintUInt5("SetWarpEnb Fail(%d)", Rval, 0U, 0U, 0U, 0U);
            }
        }
    }

    return Rval;
}

static UINT32 ApplyDefaultWarp(UINT32 FovID, UINT32 Enable, UINT32 Width, UINT32 Height, UINT32 HGdNum, UINT32 VGdNum, UINT32 TileWExp, UINT32 TileHExp, const char *FileName)
{
    AMBA_IK_MODE_CFG_s ImgMode = {0};
    AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo;
    UINT32 Rval = SVC_OK;
    const char *ptable;
    UINT32 ActiveWidth = 0U, ActiveHeight = 0U, HorGridNum = 0U, VerGridNum = 0U, WarpSize = 0U, TileWidthExp = 0U, TileHeightExp = 0U;
    UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum = 0U;

    Rval = AmbaWrap_memset(&WindowSizeInfo, 0, sizeof(AMBA_IK_WINDOW_SIZE_INFO_s));
    if (FileName == NULL) {
        AmbaPrint_PrintStr5("%s: FileName is NULL!!",__func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        Rval = SvcResCfg_GetFovIdxs(FovIdxs, &FovNum);
        if (SVC_OK != Rval) {
            AmbaPrint_PrintStr5( "%s ## fail to get fov idx and fov num", __func__, NULL, NULL, NULL, NULL);
        }
    }

    if (Rval == SVC_OK) {
        if (FovID >= FovNum) {
            AmbaPrint_PrintUInt5("ApplyDefaultWarp: Invalid ViewzoneId %u", FovID, 0U, 0U, 0U, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        ImgMode.ContextId = FovID;
        Rval = AmbaIK_GetWindowSizeInfo(&ImgMode, &WindowSizeInfo);

        if ((Width == 0U) && (Height == 0U)){
            ActiveWidth = WindowSizeInfo.VinSensor.Width;
            ActiveHeight = WindowSizeInfo.VinSensor.Height;
        } else {
            ActiveWidth = Width;
            ActiveHeight = Height;
        }

        if((TileWExp == 0U) && (TileHExp == 0U)){
            TileWidthExp    = SVC_TILEWIDTHEXP;
            TileHeightExp   = SVC_TILEHEIGHTEXP;
        } else {
            TileWidthExp    = TileWExp;
            TileHeightExp   = TileHExp;
        }

        if ((HGdNum == 0U) && (VGdNum == 0U)){
            HorGridNum = ((ActiveWidth + SVC_TILEWIDTH - 1U) >> TileWidthExp) + 1U;
            VerGridNum = ((ActiveHeight + SVC_TILEHEIGHT - 1U) >> TileHeightExp) + 1U;
        } else {
            HorGridNum = HGdNum;
            VerGridNum = VGdNum;
        }
        WarpSize = HorGridNum * VerGridNum * 4U;
    }

    if (Rval == SVC_OK) {
        Rval = OpenDefaultWarpTable(FileName, FovID, WarpSize);
        if (Rval != SVC_OK) {
            AmbaPrint_PrintStr5("%s: Open Warp table %s failed ...",__func__, FileName, NULL, NULL, NULL);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        //collect warpinfo
        if (Enable == 1U) {
            WarpInfo.Version                            = SVC_VERSION;
            WarpInfo.HorGridNum                         = HorGridNum;
            WarpInfo.VerGridNum                         = VerGridNum;
            WarpInfo.TileWidthExp                       = TileWidthExp;
            WarpInfo.TileHeightExp                      = TileHeightExp;
            WarpInfo.VinSensorGeo.Width                 = ActiveWidth;
            WarpInfo.VinSensorGeo.Height                = ActiveHeight;
            WarpInfo.VinSensorGeo.StartX                = (UINT32)WindowSizeInfo.VinSensor.StartX;
            WarpInfo.VinSensorGeo.StartY                = (UINT32)WindowSizeInfo.VinSensor.StartY;
            WarpInfo.VinSensorGeo.HSubSample.FactorDen  = WindowSizeInfo.VinSensor.HSubSample.FactorNum;
            WarpInfo.VinSensorGeo.HSubSample.FactorNum  = WindowSizeInfo.VinSensor.HSubSample.FactorDen;
            WarpInfo.VinSensorGeo.VSubSample.FactorDen  = WindowSizeInfo.VinSensor.VSubSample.FactorNum;
            WarpInfo.VinSensorGeo.VSubSample.FactorNum  = WindowSizeInfo.VinSensor.VSubSample.FactorDen;
            WarpInfo.Enb_2StageCompensation             = 1U;

            ptable = &WarpTableInfo[FovID].table[0];
            AmbaMisra_TypeCast(&WarpInfo.pWarp, &ptable);
        } else {
            WarpInfo.Version                            = 0xDEADBEEFU;
        }

        //apply warpinfo
        if(Enable == 1U) {
            AmbaPrint_PrintUInt5("Enable WARP", 0U, 0U, 0U, 0U, 0U);
#if SVC_DUMP_WARPINFO
            DumpWarpInfo(FovID, &WarpInfo);
#endif

            Rval = AmbaIK_SetWarpInfo(&ImgMode, &WarpInfo);
            if (Rval != SVC_OK) {
                AmbaPrint_PrintUInt5("SetWarpInfo Fail(%d)", Rval, 0U, 0U, 0U, 0U);
            }

            Rval = AmbaIK_SetWarpEnb(&ImgMode, Enable);
            if (Rval != SVC_OK) {
                AmbaPrint_PrintUInt5("SetWarpEnb Fail(%d)", Rval, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("Disable WARP", 0U, 0U, 0U, 0U, 0U);
            Rval = AmbaIK_SetWarpEnb(&ImgMode, Enable);
            if (Rval != SVC_OK) {
                AmbaPrint_PrintUInt5("SetWarpEnb Fail(%d)", Rval, 0U, 0U, 0U, 0U);
            }
        }
    }

    return SVC_OK;
}

static UINT32 ApplyWarp(UINT32 FovID, UINT32 Enable, UINT32 Width, UINT32 Height, UINT32 HGdNum, UINT32 VGdNum, UINT32 TileWExp, UINT32 TileHExp, const char *FileName)
{
    AMBA_IK_MODE_CFG_s ImgMode = {0};
    AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo;
    UINT32 Rval = SVC_OK;
    const char *ptable;
    UINT32 ActiveWidth = 0U, ActiveHeight = 0U, HorGridNum = 0U, VerGridNum = 0U, TileWidthExp = 0U, TileHeightExp = 0U;
    UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum = 0U;

    Rval = AmbaWrap_memset(&WindowSizeInfo, 0, sizeof(AMBA_IK_WINDOW_SIZE_INFO_s));
    if (FileName == NULL) {
        AmbaPrint_PrintStr5("%s: FileName is NULL!!",__func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        Rval = SvcResCfg_GetFovIdxs(FovIdxs, &FovNum);
        if (SVC_OK != Rval) {
            AmbaPrint_PrintStr5( "%s ## fail to get fov idx and fov num", __func__, NULL, NULL, NULL, NULL);
        }
    }

    if (Rval == SVC_OK) {
        if (FovID >= FovNum) {
            AmbaPrint_PrintUInt5("ApplyDefaultWarp: Invalid ViewzoneId %u", FovID, 0U, 0U, 0U, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        ImgMode.ContextId = FovID;
        Rval = AmbaIK_GetWindowSizeInfo(&ImgMode, &WindowSizeInfo);

        if ((Width == 0U) && (Height == 0U)){
            ActiveWidth = WindowSizeInfo.VinSensor.Width;
            ActiveHeight = WindowSizeInfo.VinSensor.Height;
        } else {
            ActiveWidth = Width;
            ActiveHeight = Height;
        }

        if((TileWExp == 0U) && (TileHExp == 0U)){
            TileWidthExp    = SVC_TILEWIDTHEXP;
            TileHeightExp   = SVC_TILEHEIGHTEXP;
        } else {
            TileWidthExp    = TileWExp;
            TileHeightExp   = TileHExp;
        }

        if ((HGdNum == 0U) && (VGdNum == 0U)){
            HorGridNum = ((ActiveWidth + SVC_TILEWIDTH - 1U) >> TileWidthExp) + 1U;
            VerGridNum = ((ActiveHeight + SVC_TILEHEIGHT - 1U) >> TileHeightExp) + 1U;
        } else {
            HorGridNum = HGdNum;
            VerGridNum = VGdNum;
        }
    }

    if (Rval == SVC_OK) {
        Rval = OpenWarpTableWithHeader(FileName, FovID);
        if (Rval != SVC_OK) {
            AmbaPrint_PrintStr5("%s: Open Warp table %s failed ...",__func__, FileName, NULL, NULL, NULL);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        //collect warpinfo
        if (Enable == 1U) {
            WarpInfo.Version                            = SVC_VERSION;
            WarpInfo.HorGridNum                         = HorGridNum;
            WarpInfo.VerGridNum                         = VerGridNum;
            WarpInfo.TileWidthExp                       = TileWidthExp;
            WarpInfo.TileHeightExp                      = TileHeightExp;
            WarpInfo.VinSensorGeo.Width                 = ActiveWidth;
            WarpInfo.VinSensorGeo.Height                = ActiveHeight;
            WarpInfo.VinSensorGeo.StartX                = (UINT32)WindowSizeInfo.VinSensor.StartX;
            WarpInfo.VinSensorGeo.StartY                = (UINT32)WindowSizeInfo.VinSensor.StartY;
            WarpInfo.VinSensorGeo.HSubSample.FactorDen  = WindowSizeInfo.VinSensor.HSubSample.FactorNum;
            WarpInfo.VinSensorGeo.HSubSample.FactorNum  = WindowSizeInfo.VinSensor.HSubSample.FactorDen;
            WarpInfo.VinSensorGeo.VSubSample.FactorDen  = WindowSizeInfo.VinSensor.VSubSample.FactorNum;
            WarpInfo.VinSensorGeo.VSubSample.FactorNum  = WindowSizeInfo.VinSensor.VSubSample.FactorDen;
            WarpInfo.Enb_2StageCompensation             = 1U;

            ptable = &WarpTableInfo[FovID].table[0];
            AmbaMisra_TypeCast(&WarpInfo.pWarp, &ptable);
        } else {
            WarpInfo.Version                            = 0xDEADBEEFU;
        }

        //apply warpinfo
        if(Enable == 1U) {
            AmbaPrint_PrintUInt5("Enable WARP", 0U, 0U, 0U, 0U, 0U);
#if SVC_DUMP_WARPINFO
            DumpWarpInfo(FovID, &WarpInfo);
#endif

            Rval = AmbaIK_SetWarpInfo(&ImgMode, &WarpInfo);
            if (Rval != SVC_OK) {
                AmbaPrint_PrintUInt5("SetWarpInfo Fail(%d)", Rval, 0U, 0U, 0U, 0U);
            }

            Rval = AmbaIK_SetWarpEnb(&ImgMode, Enable);
            if (Rval != SVC_OK) {
                AmbaPrint_PrintUInt5("SetWarpEnb Fail(%d)", Rval, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("Disable WARP", 0U, 0U, 0U, 0U, 0U);
            Rval = AmbaIK_SetWarpEnb(&ImgMode, Enable);
            if (Rval != SVC_OK) {
                AmbaPrint_PrintUInt5("SetWarpEnb Fail(%d)", Rval, 0U, 0U, 0U, 0U);
            }
        }
    }

    return SVC_OK;
}

/**
 * Svc_ApplyStereoWarpV2
 *
 * @param [in] FovID
 * @param [in] StereoWarpInfo
 * @param [in] Enable
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */

UINT32 Svc_ApplyStereoWarpV2(UINT32 FovID, const AMBA_STU_STEREO_CAM_WARP_INFO_s *StereoWarpInfo, UINT32 Enable)
{
    UINT32 Rval = SVC_OK;
    AMBA_IK_GRID_POINT_s *pWarpTbl = NULL;
    UINT32 i, Width = 0U, Height = 0U, HGdNum = 0U, VGdNum = 0U;
    UINT32 TileW, TileH, TileWExp = 0U, TileHExp = 0U;


    if (StereoWarpInfo != NULL) {
        if ((StereoWarpInfo->Method.Version[0] == 2U) &&
            (StereoWarpInfo->Method.Version[1] == 1U) &&
            (StereoWarpInfo->Method.Version[2] == 0U) &&
            (StereoWarpInfo->Method.Version[3] == 0U)) {
            const AMBA_STU_WARP_INFO_HEADER_2100_t *pHeader = &(StereoWarpInfo->Method.V2100.Header);
            Width = pHeader->InputWidth;
            Height = pHeader->InputHeight;
            HGdNum = pHeader->HorizontalTilesNum;
            VGdNum = pHeader->VerticalTilesNum;
            TileW = pHeader->TileWidthQ16 >> 16U;
            TileH = pHeader->TileHeightQ16 >> 16U;

            for (i = 3U; i <= 5U; i++) { /* Tile size could be 8, 16 or 32 */
                if (1U == (TileW >> i)) {
                    TileWExp = i;
                }
                if (1U == (TileH >> i)) {
                    TileHExp = i;
                }
            }
            AmbaPrint_PrintStr5("%s: Stereo warp file V2100", __func__, NULL, NULL, NULL, NULL);
        } else if ((StereoWarpInfo->Method.Version[0] == 2U) &&
                   (StereoWarpInfo->Method.Version[1] == 2U) &&
                   (StereoWarpInfo->Method.Version[2] == 0U) &&
                   (StereoWarpInfo->Method.Version[3] == 0U)) {
            const AMBA_STU_WARP_INFO_HEADER_2200_t *pHeader = &(StereoWarpInfo->Method.V2200.Header);
            Width = pHeader->InputWidth;
            Height = pHeader->InputHeight;
            HGdNum = pHeader->HorizontalTilesNum;
            VGdNum = pHeader->VerticalTilesNum;
            TileW = pHeader->TileWidthQ16 >> 16U;
            TileH = pHeader->TileHeightQ16 >> 16U;

            for (i = 3U; i <= 5U; i++) { /* Tile size could be 8, 16 or 32 */
                if (1U == (TileW >> i)) {
                    TileWExp = i;
                }
                if (1U == (TileH >> i)) {
                    TileHExp = i;
                }
            }
            AmbaPrint_PrintStr5("%s: Stereo warp file V2200", __func__, NULL, NULL, NULL, NULL);
        } else {
#if defined(CONFIG_SOC_CV2)
            const AMBA_CAL_WARP_CALIB_DATA_s *pHeader = &(StereoWarpInfo->Method.VAst.WarpTbl);
            Width = pHeader->CalibSensorGeo.Width;
            Height = pHeader->CalibSensorGeo.Height;
            HGdNum = pHeader->HorGridNum;
            VGdNum = pHeader->VerGridNum;
            TileWExp = pHeader->TileWidthExp;
            TileHExp = pHeader->TileHeightExp;
            AmbaPrint_PrintStr5("%s: Stereo warp file VAst", __func__, NULL, NULL, NULL, NULL);
#endif
            AmbaPrint_PrintStr5("%s: cv2 only", __func__, NULL, NULL, NULL, NULL);
        }

        AmbaPrint_PrintUInt5("Resolution: %d x %d\n", Width, Height, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("TileNum: %d x %d\n", HGdNum, VGdNum, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("TileExp: %d x %d\n", TileWExp, TileHExp, 0U, 0U, 0U);

        Rval = AmbaSTU_GetWarpTblAddr(StereoWarpInfo, &pWarpTbl);
        if (STU_OK != Rval) {
            AmbaPrint_PrintStr5("%s: AmbaSTU_GetWarpTblAddr fail",__func__, NULL, NULL, NULL, NULL);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        Rval = ApplyWarpTable(FovID, Enable, Width, Height, HGdNum, VGdNum, TileWExp, TileHExp, pWarpTbl);
    }

    return Rval;
}

/**
 * Svc_ApplyStereoWarp
 *
 * @param [in] FovID
 * @param [in] FileName
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */

UINT32 Svc_ApplyStereoWarp(UINT32 FovID, const char *FileName)
{
    AMBA_FS_FILE *pFile = NULL;
    AMBA_FS_FILE_INFO_s FileStat;
    UINT32 DataSize, Size, Rval;
    AMBA_IK_GRID_POINT_s *pWarpTbl = NULL;
    UINT32 i, Enable = 1U, Width = 0U, Height = 0U, HGdNum = 0U, VGdNum = 0U;
    UINT32 TileW, TileH, TileWExp = 0U, TileHExp = 0U;
    static AMBA_STU_STEREO_CAM_WARP_INFO_s StereoWarpInfo;

    /* check if warp file exists */
    Rval = AmbaFS_GetFileInfo(FileName, &FileStat);
    if (Rval != AMBA_FS_ERR_NONE) {
        AmbaPrint_PrintStr5("%s: warp file %s doesn't exist!",__func__, FileName, NULL, NULL, NULL);
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        /* Open warp binary */
        Rval = AmbaFS_FileOpen(FileName, "r", &pFile);
        if ((AMBA_FS_ERR_NONE != Rval) || (pFile == NULL)) {
             AmbaPrint_PrintStr5("%s: AmbaFS_FileOpen(%s) fail",__func__, FileName, NULL, NULL, NULL);
             Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        Size = (UINT32) FileStat.Size;
        Size = (Size < sizeof(AMBA_STU_STEREO_CAM_WARP_INFO_s)) ? Size : sizeof(AMBA_STU_STEREO_CAM_WARP_INFO_s);
        Rval = AmbaFS_FileRead(&StereoWarpInfo, 1, Size, pFile, &DataSize);
        if ((AMBA_FS_ERR_NONE != Rval) || (DataSize != Size)) {
            AmbaPrint_PrintStr5("%s: AmbaFS_FileRead(%s) fail",__func__, FileName, NULL, NULL, NULL);
            Rval = SVC_NG;
        }
        Rval = AmbaFS_FileClose(pFile);
    }

    if (Rval == SVC_OK) {
        if ((StereoWarpInfo.Method.Version[0] == 2U) &&
            (StereoWarpInfo.Method.Version[1] == 1U) &&
            (StereoWarpInfo.Method.Version[2] == 0U) &&
            (StereoWarpInfo.Method.Version[3] == 0U)) {
            const AMBA_STU_WARP_INFO_HEADER_2100_t *pHeader = &(StereoWarpInfo.Method.V2100.Header);
            Width = pHeader->InputWidth;
            Height = pHeader->InputHeight;
            HGdNum = pHeader->HorizontalTilesNum;
            VGdNum = pHeader->VerticalTilesNum;
            TileW = pHeader->TileWidthQ16 >> 16U;
            TileH = pHeader->TileHeightQ16 >> 16U;

            for (i = 3U; i <= 5U; i++) { /* Tile size could be 8, 16 or 32 */
                if (1U == (TileW >> i)) {
                    TileWExp = i;
                }
                if (1U == (TileH >> i)) {
                    TileHExp = i;
                }
            }
            AmbaPrint_PrintStr5("%s: Stereo warp file V2100", __func__, NULL, NULL, NULL, NULL);
        } else if ((StereoWarpInfo.Method.Version[0] == 2U) &&
                   (StereoWarpInfo.Method.Version[1] == 2U) &&
                   (StereoWarpInfo.Method.Version[2] == 0U) &&
                   (StereoWarpInfo.Method.Version[3] == 0U)) {
            const AMBA_STU_WARP_INFO_HEADER_2200_t *pHeader = &(StereoWarpInfo.Method.V2200.Header);
            Width = pHeader->InputWidth;
            Height = pHeader->InputHeight;
            HGdNum = pHeader->HorizontalTilesNum;
            VGdNum = pHeader->VerticalTilesNum;
            TileW = pHeader->TileWidthQ16 >> 16U;
            TileH = pHeader->TileHeightQ16 >> 16U;

            for (i = 3U; i <= 5U; i++) { /* Tile size could be 8, 16 or 32 */
                if (1U == (TileW >> i)) {
                    TileWExp = i;
                }
                if (1U == (TileH >> i)) {
                    TileHExp = i;
                }
            }
            AmbaPrint_PrintStr5("%s: Stereo warp file V2200", __func__, NULL, NULL, NULL, NULL);
        } else {
#if defined(CONFIG_SOC_CV2)
            const AMBA_CAL_WARP_CALIB_DATA_s *pHeader = &(StereoWarpInfo.Method.VAst.WarpTbl);
            Width = pHeader->CalibSensorGeo.Width;
            Height = pHeader->CalibSensorGeo.Height;
            HGdNum = pHeader->HorGridNum;
            VGdNum = pHeader->VerGridNum;
            TileWExp = pHeader->TileWidthExp;
            TileHExp = pHeader->TileHeightExp;
            AmbaPrint_PrintStr5("%s: Stereo warp file VAst", __func__, NULL, NULL, NULL, NULL);
#endif
            AmbaPrint_PrintStr5("%s: cv2 only", __func__, NULL, NULL, NULL, NULL);
        }

        AmbaPrint_PrintUInt5("Resolution: %d x %d\n", Width, Height, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("TileNum: %d x %d\n", HGdNum, VGdNum, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("TileExp: %d x %d\n", TileWExp, TileHExp, 0U, 0U, 0U);

        Rval = AmbaSTU_GetWarpTblAddr(&StereoWarpInfo, &pWarpTbl);
        if (STU_OK != Rval) {
            AmbaPrint_PrintStr5("%s: AmbaSTU_GetWarpTblAddr fail",__func__, NULL, NULL, NULL, NULL);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        Rval = ApplyWarpTable(FovID, Enable, Width, Height, HGdNum, VGdNum, TileWExp, TileHExp, pWarpTbl);
    }

    return Rval;
}

/**
 * Svc_ApplyWarpFile
 *
 * @param [in] FovID
 * @param [in] Enable
 * @param [in] Width
 * @param [in] Height
 * @param [in] Header
 * @param [in] HGdNum
 * @param [in] VGdNum
 * @param [in] TileWExp
 * @param [in] TileHExp
 * @param [in] FileName
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */

UINT32 Svc_ApplyWarpFile(UINT32 FovID, UINT32 Enable, UINT32 Width, UINT32 Height, UINT32 Header, UINT32 HGdNum, UINT32 VGdNum, UINT32 TileWExp, UINT32 TileHExp, const char *FileName)
{
    UINT32 Rval;

    if (Header == 1U) {
        Rval = ApplyWarp(FovID, Enable, Width, Height, HGdNum, VGdNum, TileWExp, TileHExp, FileName);
    } else {
        Rval = ApplyDefaultWarp(FovID, Enable, Width, Height, HGdNum, VGdNum, TileWExp, TileHExp, FileName);
    }

    return Rval;
}

/**
 * Svc_ApplyWarpTable
 *
 * @param [in] FovID
 * @param [in] Enable
 * @param [in] Width
 * @param [in] Height
 * @param [in] HGdNum
 * @param [in] VGdNum
 * @param [in] TileWExp
 * @param [in] TileHExp
 * @param [in] pWarpTbl
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */

UINT32 Svc_ApplyWarpTable(UINT32 FovID, UINT32 Enable, UINT32 Width, UINT32 Height, UINT32 HGdNum, UINT32 VGdNum, UINT32 TileWExp, UINT32 TileHExp, AMBA_IK_GRID_POINT_s *pWarpTbl)
{
    UINT32 Rval;

    Rval = ApplyWarpTable(FovID, Enable, Width, Height, HGdNum, VGdNum, TileWExp, TileHExp, pWarpTbl);

    return Rval;
}

