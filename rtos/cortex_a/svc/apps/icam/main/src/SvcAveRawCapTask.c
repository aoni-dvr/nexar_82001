/**
 *  @file SvcAveRawCapTask.c
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
 *  @details svc ave raw cap task
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaMisraFix.h"
#include "AmbaFS.h"
#include "AmbaSensor.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaDef.h"
#include "AmbaDSP_ImageFilter.h"

#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcCmd.h"
#include "SvcLog.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcRawCap.h"
#include "SvcResCfg.h"

typedef struct {
    UINT8  VinID;
    // UINT32 *pTotalRawBuf;
    // UINT16 *pAvgRawBuf; /* For 16-bit RAW */
    UINT32  CapCount;   /* Used to count how may raw is captured. Will be reset if CapCount % CapNum == 0 */
    UINT32  CapNum;     /* User specified number of being captured. */
    UINT32  SaveCount;  /* Everytime CapCount meet CapNum, the RAW will be saved. SaveCount will ++ then */
    UINT8   AvgTileFlag;
    UINT8   SaveRawFlag;
} AVE_RAW_CAP_TASK_CTRL;

typedef struct {
    UINT32 Address;     //Raw data address
    UINT32 Pitch;       //Pitch
    UINT32 Width;       //Width = Pitch/2
    UINT32 Height;      //Height
    UINT32 TileCountX;  //TileCountX, eg. TileCountX = 7
    UINT32 TileCountY;  //TileCountY, eg. TileCountX = 4
    UINT32 CountX;      //CountX, eg. CountX = 0, range is 0~6
    UINT32 CountY;      //CountY, eg. CountY = 0, range is 0~3
    UINT32 AvgW;        //AvgW, define the width of the area used to calculate the avg. inside a tile
    UINT32 AvgH;        //AvgH, define the height of the area used to calculate the avg. inside a tile
} TILE_AVG_s;

static void AveRawCapTask_CfgMem(void);
static void AveRawCapTask_Capture(void);
// static void AveRawCapTask_SaveRaw(UINT8 *pSrcBuf, UINT32 Pitch, UINT32 Width, UINT32 Height);
static void AveRawCapTask_SaveRaw(UINT8 *pSrcBuf, UINT32 RawSize);

static void AveRawCapTask_CmdInstall(void);
static void AveRawCapTask_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void AveRawCapTask_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc);

static UINT32 CameraIQ_TileAvg(TILE_AVG_s *pTileAvg, UINT16 *pAvg);
static void   CameraIQ_TotalAvgBuffer(ULONG Address, UINT32 Pitch, UINT32 Width, UINT16 Height);
static void   CameraIQ_CalcAvgBuffer(UINT32 Width, UINT16 Height);
static void   CameraIQ_DumpAvgBuffer(ULONG Address, UINT32 Pitch, UINT32 Width, UINT16 Height);

static UINT16 TileAvgBuf[300][300] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT32 TotalRawDumpBuf[1120][1936] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT16 AvgRawDumpBuf[1120][1936] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static AVE_RAW_CAP_TASK_CTRL AveTaskCtrl;
static UINT32 AveTaskStatus = 0U;

#define SVC_LOG_AVE_RAW_CAP_TASK "SVC_AVE_RAW_CAP_TASK"

#define STATUS_INIT_DONE            (0x1U)
#define STATUS_CFG_MEM_DONE         (0x2U)
#define STATUS_CAP_ON_GOING         (0x4U)
#define STATUS_CMD_INSTALLED        (0x8U)

UINT32 SvcAveRawCapTask_Init(void)
{
    /* reset the setting */
    AmbaWrap_memset(&AveTaskCtrl, 0, sizeof(AveTaskCtrl));

    /* Install test command */
    if ((AveTaskStatus & STATUS_CMD_INSTALLED) == 0U) {
        AveRawCapTask_CmdInstall();
        AveTaskStatus |= STATUS_CMD_INSTALLED;
    }

    AveTaskStatus |= STATUS_INIT_DONE;

    return SVC_OK;
}

static void AveRawCapTask_CfgMem(void)
{
#if 0
    UINT32 RetVal = SVC_OK;
    ULONG  MemBase = 0UL;
    UINT32 MemSize = 0U;

    SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    RetVal = SvcBuffer_LockFreeSpace(SVC_MEM_TYPE_CA, &MemBase, &MemSize);
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_AVE_RAW_CAP_TASK, "cfg mem failed, lock free space err 0x%x", RetVal, 0U);
    } else {
        UINT32 VinID = AveTaskCtrl.VinID;
        UINT32 WorkSize = 0U, TotalWorkSize;
        /* Maximum support 16 bits RAW, and needs two buffers. One for Avg, one for total */
        WorkSize = GetAlignedValU32(pCfg->VinCfg[VinID].CapWin.Width * pCfg->VinCfg[VinID].CapWin.Height * 2U, 32);
        TotalWorkSize = WorkSize * 3U;

        if (MemSize > TotalWorkSize) {
            AmbaMisra_TypeCast(&AveTaskCtrl.pAvgRawBuf, &MemBase);
            SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "cfg mem, pAvgRawBuf base = 0x%x, size = 0x%x", MemBase, WorkSize);

            MemBase += GetAlignedValU32(WorkSize, 32);

            AmbaMisra_TypeCast(&AveTaskCtrl.pTotalRawBuf, &MemBase);
            SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "cfg mem, pTotalRawBuf   base = 0x%x, size = 0x%x", MemBase, WorkSize * 2U);
        } else {
            SvcLog_NG(SVC_LOG_AVE_RAW_CAP_TASK, "cfg mem failed, memory not enough (%u) < (%u)", MemSize, TotalWorkSize);
        }

    }

    RetVal = SvcBuffer_UnLockFreeSpace(SVC_MEM_TYPE_CA);
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_AVE_RAW_CAP_TASK, "cfg mem failed, unlock free space err 0x%x", RetVal, 0U);
    }
#else
    /* Do nothing now */

#endif
}

static void AveRawCapTask_Capture(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 RawCapCfg = SVC_RAW_CAP_CFG_RAW | SVC_RAW_CAP_CFG_NON_BMP;
    SVC_RAW_CAP_INFO_s RawInfo = {0};
    AMBA_SENSOR_CHANNEL_s     SsChan = {0};
    AMBA_SENSOR_STATUS_INFO_s SsStatus = {0};
    UINT32 VinID = AveTaskCtrl.VinID;

    if ((AveTaskStatus & STATUS_CFG_MEM_DONE) == 0U) {
        /* Init memory */
        AveRawCapTask_CfgMem();
        AveTaskStatus |= STATUS_CFG_MEM_DONE;
    }

    if ((AveTaskStatus & STATUS_CAP_ON_GOING) == 0U) {
        AveTaskStatus |= STATUS_CAP_ON_GOING;
    }

    /* Check sensor hdr case */
    SsChan.VinID = VinID;
    RetVal = AmbaSensor_GetStatus(&SsChan, &SsStatus);
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_AVE_RAW_CAP_TASK, "capture failed, get sensor status err 0x%x", RetVal, 0U);
    } else if ((RetVal == SVC_OK) && (SsStatus.ModeInfo.HdrInfo.ActiveChannels > 1U)) {
        RawCapCfg |= SVC_RAW_CAP_CFG_HDS;
    } else {
        /* Do nothing */
    }

    /* Configure raw capture */
    if (RetVal == SVC_OK) {
        RetVal = SvcRawCap_CfgCap((0x1UL << VinID), RawCapCfg);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_AVE_RAW_CAP_TASK, "capture failed, cfg cap err 0x%x", RetVal, 0U);
        }
    }

    /* Configure vin type */
    if (RetVal == SVC_OK) {
        RetVal = SvcRawCap_CfgVinType(VinID, SVC_RAW_CAP_VIN_TYPE_CFA);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_AVE_RAW_CAP_TASK, "capture failed, cfg vin type err 0x%x", RetVal, 0U);
        }
    }

    /* Configure raw */
    if (RetVal == SVC_OK) {
        RetVal = SvcRawCap_CfgRaw(VinID, "raw");
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_AVE_RAW_CAP_TASK, "capture failed, cfg vin type err 0x%x", RetVal, 0U);
        }
    }

    /* Configure hds */
    if ((RetVal == SVC_OK) && ((RawCapCfg & SVC_RAW_CAP_CFG_HDS) > 0U)) {
        RetVal = SvcRawCap_CfgHds(VinID, "hds");
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_AVE_RAW_CAP_TASK, "capture failed, cfg vin type err 0x%x", RetVal, 0U);
        }
    }

    /* Capture raw, hds */
    if (RetVal == SVC_OK) {
        RetVal = SvcRawCap_CapSeq(1U /* CapNum */, 1U  /* Blocking */);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_AVE_RAW_CAP_TASK, "capture failed, cap seq err 0x%x", RetVal, 0U);
        }
        
    }

    /* Check raw status */
    if (RetVal == SVC_OK) {
        RetVal = SvcRawCap_GetRaw(VinID, &RawInfo);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_AVE_RAW_CAP_TASK, "capture failed, cap seq err 0x%x", RetVal, 0U);
        } else {
            UINT32 MemBase = 0U;
            AmbaMisra_TypeCast(&MemBase, &RawInfo.pBuf);
            SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "capture, Pitch %u", RawInfo.Pitch, 0U);
            SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "capture, Width %u Height %u", RawInfo.Width, RawInfo.Height);
            SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "capture, MemBase 0x%x", MemBase, 0U);
        }
    }

    /* Dump Avg Raw Buffer when CapCount == CapNum. AveTaskStatus will be &= ~STATUS_CAP_ON_GOING then */
    if ((RetVal == SVC_OK) && ((AveTaskStatus & STATUS_CAP_ON_GOING) != 0U) ) {
        CameraIQ_DumpAvgBuffer((UINT32) RawInfo.pBuf, RawInfo.Pitch, RawInfo.Width, RawInfo.Height);
    }

    if ((RetVal == SVC_OK) && ((AveTaskStatus & STATUS_CAP_ON_GOING) == 0U) && (AveTaskCtrl.SaveRawFlag != 0U)) {   
#if 1
        AveRawCapTask_SaveRaw((UINT8 *) &AvgRawDumpBuf[0], 1936U*1108U * 2U);
#else
        AveRawCapTask_SaveRaw((UINT8 *) AveTaskCtrl.pAvgRawBuf, RawInfo.Pitch, RawInfo.Width, RawInfo.Height);
#endif
    }
}

#if 1
static void AveRawCapTask_SaveRaw(UINT8 *pSrcBuf, UINT32 RawSize)
{
    AMBA_FS_FILE *pFile;
    char FilePath[32];
    UINT32 ReturnSize = 0U;

    (void) SvcWrap_sprintfU32(FilePath, 32, "C:\\ave_%d.raw", 1, &AveTaskCtrl.SaveCount);
    AveTaskCtrl.SaveCount++;

    AmbaFS_FileOpen(FilePath, "wb", &pFile);
    AmbaFS_FileWrite(pSrcBuf, 1, RawSize, pFile, &ReturnSize);
    AmbaFS_FileClose(pFile);

    SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "Raw Path:", 0U, 0U);
    SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, FilePath, 0U, 0U);
}
#else
static void AveRawCapTask_SaveRaw(UINT8 *pSrcBuf, UINT32 Pitch, UINT32 Width, UINT32 Height)
{
    UINT32 RetVal = SVC_OK;
    AMBA_FS_FILE *pFile;
    char FilePath[32];

    UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum;
    SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    UINT32 WrIdx = 0U, WrLineSize = 0, WrSize = 0U;
    UINT32 RawCmprType = 0U;

    /* Find first FOV to get the raw-compression type */
    if (SVC_OK == SvcResCfg_GetFovIdxsInVinID(AveTaskCtrl.VinID, FovIdxs, &FovNum)) {
        RawCmprType = pCfg->FovCfg[FovIdxs[0U]].PipeCfg.RawCompression;
    }

    switch (RawCmprType) {
#if defined(IK_RAW_COMPACT_8B)
        case IK_RAW_COMPACT_8B:
        WrLineSize = Width;
        break;
#endif
#if defined(IK_RAW_COMPACT_10B)
        case IK_RAW_COMPACT_10B:
        WrLineSize = Width * 10U / 8U;
        break;
#endif
#if defined(IK_RAW_COMPACT_12B)
        case IK_RAW_COMPACT_12B:
        WrLineSize = Width * 12U / 8U;
        break;
#endif
#if defined(IK_RAW_COMPACT_14B)
        case IK_RAW_COMPACT_14B:
        WrLineSize = Width * 14U / 8U;
        break;
#endif
        default:
        WrLineSize = Width * 16U / 8U;
        break;
    }

    (void) SvcWrap_sprintfU32(FilePath, 32, "C:\\ave_%d.raw", 1, &AveTaskCtrl.SaveCount);
    AveTaskCtrl.SaveCount++;

    AmbaFS_FileOpen(FilePath, "wb", &pFile);
    SvcLog_DBG(SVC_LOG_SAVE_RAW_CAP_TASK, "save raw, Width = %u, Height = %u", Width, Height);
    SvcLog_DBG(SVC_LOG_SAVE_RAW_CAP_TASK, "save raw, WrLineSize = %u", WrLineSize, 0U);

    while (WrIdx != Height) {
        RetVal = AmbaFS_FileWrite(&pSrcBuf[Pitch * WrIdx], 1, WrLineSize, pFile, &WrSize);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_AVE_RAW_CAP_TASK, "save raw failed, file write err 0x%x", RetVal, 0U);
            break;
        }
        WrIdx++;
    }
}
#endif

static void AveRawCapTask_CmdInstall(void)
{
    AMBA_SHELL_COMMAND_s  SvcCmd;

    UINT32  RetVal;

    SvcCmd.pName    = "svc_ave_raw_task";
    SvcCmd.MainFunc = AveRawCapTask_CmdEntry;
    SvcCmd.pNext    = NULL;

    RetVal = SvcCmd_CommandRegister(&SvcCmd);
    if (SHELL_ERR_SUCCESS != RetVal) {
        SvcLog_NG(SVC_LOG_AVE_RAW_CAP_TASK, "## fail to install svc_ave_raw_task command", 0U, 0U);
    }
}

static void AveRawCapTask_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;
    if (1U < ArgCount) {
        if (0 == SvcWrap_strcmp("setting", pArgVector[1U])) {
            if (ArgCount >= 4) {
                if (0 == SvcWrap_strcmp("vin", pArgVector[2U])) {
                    UINT32 VinID;
                    SvcWrap_strtoul(pArgVector[3U], &VinID);
                    AveTaskCtrl.VinID = (UINT8) VinID;
                }
            }
        } else if (0 == SvcWrap_strcmp("cap", pArgVector[1U])) {
            SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "cap count (%u)", AveTaskCtrl.CapCount, 0U);
            AveRawCapTask_Capture();
        } else if (0 == SvcWrap_strcmp("ave", pArgVector[1U])) {
            if (ArgCount < 5U) {
               SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "ave_raw [Enb] [AveNum] [AveTile]", 0U, 0U);
               SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "ave_raw 1 20 1", 0U, 0U);
            } else {
                UINT32 U32Val = 0U;

                if (ArgCount >= 3U) {
                    SvcWrap_strtoul(pArgVector[2U], &U32Val);
                    AveTaskCtrl.SaveRawFlag = U32Val;
                    SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "AveRaw Dump (%u)", AveTaskCtrl.SaveRawFlag, 0U);
                }
                if (ArgCount >= 4U) {
                    SvcWrap_strtoul(pArgVector[3U], &U32Val);
                    AveTaskCtrl.CapNum = U32Val;
                    SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "AveRaw CapNum (%u)", AveTaskCtrl.CapNum, 0U);
                }
                if (ArgCount >= 5U) {
                    SvcWrap_strtoul(pArgVector[4U], &U32Val);
                    AveTaskCtrl.AvgTileFlag = U32Val;
                    SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "AveRaw AvgTileFlag (%u)", AveTaskCtrl.AvgTileFlag, 0U);
                }
            }
        } else {
            RetVal = SVC_NG;
        }
    }

    if (RetVal != SVC_OK) {
        AveRawCapTask_CmdUsage(PrintFunc);
    }
}

static void AveRawCapTask_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    AmbaMisra_TouchUnused(&PrintFunc);
    SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "svc_ave_raw_task setting vin [VinID]", 0U, 0U);
    SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "svc_ave_raw_task ave [Enb] [AveNum] [AveTile]", 0U, 0U);
    SvcLog_DBG(SVC_LOG_AVE_RAW_CAP_TASK, "svc_ave_raw_task cap", 0U, 0U);
}

static UINT32 CameraIQ_TileAvg(TILE_AVG_s *pTileAvg, UINT16 *pAvg)  //TileAVg, Eathan
{
    UINT32 RVal = 0;
    UINT32 TileW;
    UINT32 TileH;
    UINT32 TileOffsetX;
    UINT32 TileOffsetY;
    UINT32 AvgPixelCount;
    UINT32 i, j;
    UINT32 XIdx, YIdx;
    UINT16 *SrcAddr;
    UINT32 AddressBase;
    UINT32 AvgTmp[4];

    TileW = pTileAvg->Width / pTileAvg->TileCountX;
    TileH = pTileAvg->Height / pTileAvg->TileCountY;
    AvgPixelCount = pTileAvg->AvgW * pTileAvg->AvgH;
    TileOffsetX = TileW * 2 * pTileAvg->CountX;
    TileOffsetY = TileH * pTileAvg->CountY;

    (void)AmbaWrap_memset(&TileAvgBuf[0], 0, 300*300*2);
    (void)AmbaWrap_memset(&AvgTmp[0], 0, 4*4);
    /*Raw format : 16bit, Raw resolution : 1936x1108; width = patch = 1936*2 */
    AddressBase = pTileAvg->Address + (pTileAvg->Pitch*TileOffsetY) + TileOffsetX;
    for (i=0;i<TileH;i++) {
        SrcAddr = (UINT16 *)(AddressBase + (pTileAvg->Pitch*i));
        for  (j=0;j<TileW;j++) {
            TileAvgBuf[i][j] = *SrcAddr;
            SrcAddr ++;
        }
    }

    TileOffsetX = (TileW - pTileAvg->AvgW) >> 1;
    TileOffsetY = (TileH - pTileAvg->AvgH) >> 1;

    for (i = 0;i < pTileAvg->AvgH; i+=2) {
        for  (j = 0;j< pTileAvg->AvgW; j+=2) {
            XIdx = j + TileOffsetX;
            YIdx = i + TileOffsetY;
            AvgTmp[0] += (UINT16)TileAvgBuf[YIdx][XIdx];
            AvgTmp[1] += (UINT16)TileAvgBuf[YIdx][XIdx+1];
            AvgTmp[2] += (UINT16)TileAvgBuf[YIdx+1][XIdx];
            AvgTmp[3] += (UINT16)TileAvgBuf[YIdx+1][XIdx+1];
        }
    }
    AvgTmp[0] /= (AvgPixelCount>>2);
    AvgTmp[1] /= (AvgPixelCount>>2);
    AvgTmp[2] /= (AvgPixelCount>>2);
    AvgTmp[3] /= (AvgPixelCount>>2);

    pAvg[0] = AvgTmp[0];
    pAvg[1] = AvgTmp[1];
    pAvg[2] = AvgTmp[2];
    pAvg[3] = AvgTmp[3];

    //AmbaPrint_PrintUInt5("RAW Average = %u, %u, %u, %u", pAvg[0], pAvg[1], pAvg[2], pAvg[3], 0U);
    return RVal;
}

static void CameraIQ_TotalAvgBuffer(ULONG Address, UINT32 Pitch, UINT32 Width, UINT16 Height)
{
    UINT16 *SrcAddr;
    UINT32 i,j;

    /*Raw format : 16bit, Raw resolution : 1936x1108; width = patch = 1936*2 */
    for (i=0;i<Height;i++) {
        SrcAddr = (UINT16 *)(Address + (Pitch*i));
        for  (j=0;j<(Width);j++) {
            TotalRawDumpBuf[i][j] += *SrcAddr;
            SrcAddr ++;
        } 
    }
    return ;
}

static void CameraIQ_CalcAvgBuffer(UINT32 Width, UINT16 Height)
{
    UINT32 i,j;

    /*Raw format : 16bit, Raw resolution : 1936x1108; width = patch = 1936*2 */    
    for (i=0;i<Height;i++) {
        for  (j=0;j<(Width);j++) {
            if (AveTaskCtrl.CapCount > 0) {
                AvgRawDumpBuf[i][j] = (UINT16) (TotalRawDumpBuf[i][j] / AveTaskCtrl.CapCount); 
            }
        } 
    }
    return ;
}

static void CameraIQ_DumpAvgBuffer(ULONG Address, UINT32 Pitch, UINT32 Width, UINT16 Height)
{
    ULONG AvgRawAddr = 0U;
    TILE_AVG_s GetTileAvg;
    UINT16 GetAvg[4];

    if (AveTaskCtrl.CapNum == 0U) {
        AveTaskStatus &= ~STATUS_CAP_ON_GOING;
    } else {
        if (AveTaskCtrl.CapCount == 0U) {
            //  (void)AmbaWrap_memset(AveTaskCtrl.pTotalRawBuf, 0, sizeof(UINT32) * Width * Height);
            //  (void)AmbaWrap_memset(AveTaskCtrl.pAvgRawBuf, 0, sizeof(UINT16) * Width * Height);
            (void)AmbaWrap_memset(&TotalRawDumpBuf[0], 0, 1936*1120*4);
            (void)AmbaWrap_memset(&AvgRawDumpBuf[0], 0, 1936*1120*2);
        }
        if (AveTaskCtrl.CapCount < AveTaskCtrl.CapNum) {
            CameraIQ_TotalAvgBuffer(Address, Pitch, Width, Height);
            AmbaPrint_PrintUInt5("Accu raw capture (%u %u) Pitch %u Width %u Height %u", AveTaskCtrl.CapCount, AveTaskCtrl.CapNum, Pitch, Width, Height);
            AveTaskCtrl.CapCount ++;
            if (AveTaskCtrl.CapCount != AveTaskCtrl.CapNum) {
                return ;
            } else {
                CameraIQ_CalcAvgBuffer(Width, Height);
                AvgRawAddr = (UINT32)&AvgRawDumpBuf[0];
                AveTaskCtrl.CapCount = 0U;
                AveTaskStatus &= ~STATUS_CAP_ON_GOING;
            }
        }
    }

    if (AveTaskCtrl.AvgTileFlag == 1U) {
        //--------------------------------------------
        //corner left-top area
        GetTileAvg.Address = AvgRawAddr;
        GetTileAvg.Pitch = 1936*2;
        GetTileAvg.Width = 1936;
        GetTileAvg.Height = 1108;
        GetTileAvg.TileCountX = 7;
        GetTileAvg.TileCountY = 4;
        GetTileAvg.CountX = 0;
        GetTileAvg.CountY = 0;
        GetTileAvg.AvgW = 50;
        GetTileAvg.AvgH = 50;

        CameraIQ_TileAvg(&GetTileAvg, &GetAvg[0]);

        AmbaPrint_PrintUInt5("RAW Average at Tile(%u,%u)", GetTileAvg.CountX, GetTileAvg.CountY, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("RAW Average = B:%u, G:%u, G;%u, R:%u", GetAvg[0], GetAvg[1], GetAvg[2], GetAvg[3], 0U);

        //center area
        GetTileAvg.TileCountX = 7;
        GetTileAvg.TileCountY = 7;
        GetTileAvg.CountX = 3;
        GetTileAvg.CountY = 3;

        CameraIQ_TileAvg(&GetTileAvg, &GetAvg[0]);

        AmbaPrint_PrintUInt5("RAW Average at Tile(center)", GetTileAvg.CountX, GetTileAvg.CountY, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("RAW Average = B:%u, G:%u, G;%u, R:%u", GetAvg[2], GetAvg[3], GetAvg[0], GetAvg[1], 0U);
        //AmbaPrint_PrintUInt5("RAW Average = B:%u, G:%u, G;%u, R:%u", GetAvg[0], GetAvg[1], GetAvg[2], GetAvg[3], 0U);
        //--------------------------------------------
    }
}