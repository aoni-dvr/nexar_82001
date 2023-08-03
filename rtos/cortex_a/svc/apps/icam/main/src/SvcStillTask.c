/**
 *  @file SvcStillTask.c
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
 *  @details svc still task file
 *
 */

#include "AmbaTypes.h"
#include "AmbaCache.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"

#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_StillCapture.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Event.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcMem.h"
#include "SvcTask.h"
#include "SvcClock.h"
#include "SvcPlat.h"

#include "SvcBuffer.h"
#include "SvcResCfg.h"
#include "SvcDSP.h"
#include "SvcStill.h"
#include "SvcStillCap.h"
#include "SvcStillProc.h"
#include "SvcStillEnc.h"
#include "SvcStillMux.h"
#include "SvcIKCfg.h"
#include "SvcUtil.h"
#include "AmbaDCF.h"
#include "AmbaVfs.h"
#include "AmbaExif.h"
#include "SvcStgMonitor.h"
#include "AmbaMux_Def.h"
#include "SvcRecMain.h"
#include "SvcEnc.h"

#include "SvcBufMap.h"
#include "SvcStillTask.h"
#include "SvcUserPref.h"
#include "SvcRecInfoPack.h"

#define STILL_CAP_PRIORITY  (65)
#define STILL_PROC_PRIORITY (64)
#define STILL_ENC_PRIORITY  (63)
#define STILL_MUX_PRIORITY  (62)

#ifndef CONFIG_SOC_H22
#define RAW_ENC_STACK_SIZE  (0x10000U)
#define RAW_ENC_PRIORITY    (62)
#define RAW_ENC_CORE    (1)
#endif

#define STILL_CAP_CORE  (1)
#define STILL_PROC_CORE (1)
#define STILL_ENC_CORE  (1)
#define STILL_MUX_CORE  (1)

#define SVC_LOG_STL_TSK "STL_TSK"

typedef struct {
    ULONG  TotalBase;
    UINT32 TotalSize;
    ULONG  AvailBase;
    UINT32 AvailSize;
    ULONG  CapBase;
    UINT32 CapSize;
    ULONG  ProcBase;
    ULONG  ProcBaseUV;
    UINT32 ProcSize;
    ULONG  EncBase;
    UINT32 EncSize;
    ULONG  EncQBase;
    UINT32 EncQSize;
    ULONG  MuxBase;
    UINT32 MuxSize;
    ULONG  ExifBase;
    UINT32 ExifSize;
} SVC_STILL_BUF_s;

static SVC_STILL_BUF_s StillBuf GNU_SECTION_NOZEROINIT;

static UINT32 GetStorageDrive(char *pStorageDrive)
{
    SVC_USER_PREF_s *pSvcUserPref;
    UINT32 Rval = SVC_NG;

    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        if (pSvcUserPref->UsbClass == SVC_STG_USBH_CLASS_MSC) {
            *pStorageDrive = 'I';
            Rval = SVC_OK;
        } else {
            *pStorageDrive = pSvcUserPref->MainStgDrive[0];
            Rval = SVC_OK;
        }
    }
    return Rval;
}

static UINT32 SvcStillTask_CreateCap(void)
{
    SVC_CAP_TSK_s TskCfg;
    UINT32 Rval;
    Rval = AmbaWrap_memset(&TskCfg, 0, sizeof(TskCfg));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        Rval = GetStorageDrive(&TskCfg.StorageDrive);
        if (Rval == OK) {
            TskCfg.Priority = STILL_CAP_PRIORITY;
            TskCfg.CpuBits = STILL_CAP_CORE;
            TskCfg.MinStorageSize = StillBuf.MuxSize << 1U;
            Rval = SvcStillCap_Create(&TskCfg);
        }
    }
    return Rval;
}

static UINT32 SvcStillTask_CreateProc(void)
{
    SVC_STL_PROC_CONFIG_s Cfg;
    UINT32 Rval, CtxNum;

    Rval = AmbaWrap_memset(&Cfg, 0, sizeof(Cfg));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        Rval = SvcIKCfg_GetTotalCtxNum(&CtxNum);
        if (Rval == OK) {
            Cfg.Priority = STILL_PROC_PRIORITY;      /* scanning task priority */
            Cfg.CpuBits = STILL_PROC_CORE;       /* core selection which scanning task running at */
            Cfg.WorkBufAddr = StillBuf.ProcBase;
            Cfg.WorkBufSize = StillBuf.ProcSize;
            Cfg.IkCtxId = CtxNum - 1U;
            Rval = SvcStillProc_Create(&Cfg);
        }
    }
    return Rval;
}

static UINT32 GetEncBuffer(void)
{
    UINT32                   Err, Rval = SVC_OK, i;
    const SVC_RES_CFG_s      *pCfg = SvcResCfg_Get();

    if ((StillBuf.EncBase == 0U) || (StillBuf.EncSize == 0U)) {
        if (pCfg->RecNum > 0U) {
            for (i = 0U; i < pCfg->RecNum; i++) {
                /* Y2J always use encode engine0 buffer */
                if (pCfg->RecStrm[i].RecSetting.VEngineId == 0U) {
                    Err = SvcInfoPack_VEncBSBufRequest(i, &StillBuf.EncBase, &StillBuf.EncSize);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_STL_TSK, "!!! fail to get enc buffer\n", 0U, 0U);
                        Rval = SVC_NG;
                    }
                    break;
                }
            }
        } else {
            Err = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_VENC_BS, &StillBuf.EncBase, &StillBuf.EncSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_TSK, "!!! fail to get enc buffer\n", 0U, 0U);
                Rval = SVC_NG;
            }
        }
    }

    if ((StillBuf.EncBase == 0U) || (StillBuf.EncSize == 0U)) {
        SvcLog_NG(SVC_LOG_STL_TSK, "cannot find video encode engine0 buffer", 0U, 0U);
    }

    return Rval;
}

static UINT32 SvcStillTask_CreateEnc(void)
{
    UINT32 Rval;
    SVC_STL_ENC_CONFIG_s EncCfg;
    const SVC_RES_CFG_s  *pResCfg = SvcResCfg_Get();

    Rval = AmbaWrap_memset(&EncCfg, 0, sizeof(EncCfg));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        EncCfg.Priority = STILL_ENC_PRIORITY;
        EncCfg.CpuBits = STILL_ENC_CORE;
        EncCfg.QAddr = StillBuf.EncQBase;
        Rval = SvcStillEnc_Create(&EncCfg);
    }
    if (Rval == SVC_OK) {
        Rval = SvcStillEnc_SetStreamIndex((UINT16)pResCfg->RecNum);
    }
    if (Rval == SVC_OK) {
        Rval = GetEncBuffer();
    }
    if (Rval == SVC_OK) {
        Rval = SvcStillEnc_SetJpegBufer(StillBuf.EncBase, StillBuf.EncSize);
    }
    return Rval;
}

static UINT32 SvcStillTask_CreateMux(void)
{
    SVC_STL_MUX_CONFIG_s MuxCfg;
    UINT32 Rval;

    Rval = AmbaWrap_memset(&MuxCfg, 0, sizeof(MuxCfg));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        Rval = GetEncBuffer();
    }
    if (Rval == SVC_OK) {
        Rval = GetStorageDrive(&MuxCfg.StorageDrive);
        if (Rval == OK) {
            MuxCfg.Priority = STILL_MUX_PRIORITY;
            MuxCfg.CpuBits = STILL_MUX_CORE;
            MuxCfg.StillBufAddr = StillBuf.MuxBase;
            MuxCfg.StillBufSize = StillBuf.MuxSize;
            MuxCfg.VideoBufAddr = StillBuf.EncBase;
            MuxCfg.VideoBufSize = StillBuf.EncSize;
            MuxCfg.YuvBufAddr = StillBuf.ProcBase;
            MuxCfg.YuvBufSize = StillBuf.ProcSize;
            MuxCfg.RawBufAddr = StillBuf.CapBase;
            MuxCfg.RawBufSize = StillBuf.CapSize;
            MuxCfg.ExifBufAddr = StillBuf.ExifBase;
            MuxCfg.ExifBufSize = StillBuf.ExifSize;
            Rval = SvcStillMux_Create(&MuxCfg);
        }
    }
    return Rval;
}

static UINT32 SetupMemory(UINT32 TaskSelect)
{
    UINT32 Rval, Size;
    Rval = AmbaWrap_memset(&StillBuf, 0, sizeof(StillBuf));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_TRANSIENT_BUF, &StillBuf.TotalBase, &StillBuf.TotalSize);
    }
    if (Rval == OK) {
        const SVC_RES_CFG_MAX_s  *pMaxCfg = SvcResCfg_GetMax();
        SvcStillLog("[SetupMemory] still buffer base %p, size 0x%x", StillBuf.TotalBase, StillBuf.TotalSize, 0, 0, 0);
        StillBuf.AvailBase = StillBuf.TotalBase;
        StillBuf.AvailSize = StillBuf.TotalSize;
        if ((TaskSelect & SVC_STILL_CAP) == SVC_STILL_CAP) {
            SVC_DSP_FRAME_s Setup;
            Rval = AmbaWrap_memset(&Setup, 0, sizeof(Setup));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            } else {
                Size = (pMaxCfg->StillRawCapBufSize > pMaxCfg->StillYuvCapBufSize)? pMaxCfg->StillRawCapBufSize: pMaxCfg->StillYuvCapBufSize;
                if (Size <= StillBuf.AvailSize) {
                    StillBuf.CapBase = StillBuf.AvailBase;
#ifdef SVC_STL_ENABLE_CLEAN_RAW
                    StillBuf.CapSize = Size << 1; /* with clean raw */
#else
                    StillBuf.CapSize = Size;
#endif
                    StillBuf.AvailBase += Size;
                    StillBuf.AvailSize -= Size;
                    SvcStillLog("[SetupMemory] cap buffer base %p, size 0x%x", StillBuf.CapBase, StillBuf.CapSize, 0, 0, 0);
                } else {
                    SvcLog_NG(SVC_LOG_STL_TSK, "cap buffer needed 0x%x, but avail 0x%x", Size, StillBuf.AvailSize);
                }
            }
        }
        if ((TaskSelect & SVC_STILL_PROC) == SVC_STILL_PROC) {
            SVC_STL_PROC_SETUP_s ProcSetup;
            UINT32 MaxMainW = (UINT32)pMaxCfg->StillWin.Width;
            UINT32 MaxMainH = (UINT32)pMaxCfg->StillWin.Height;
            Rval = AmbaWrap_memset(&ProcSetup, 0, sizeof(ProcSetup));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            } else {
                ProcSetup.MaxMainYuvW = MaxMainW;
                ProcSetup.MaxMainYuvH = MaxMainH;
                ProcSetup.ScrnYuvW = SVC_STL_SCRN_W;
                ProcSetup.ScrnYuvH = (UINT16)((SVC_STL_SCRN_W * MaxMainH) / MaxMainW);

                ProcSetup.QviewYuvW = pMaxCfg->StillQviewWin.Width;
                ProcSetup.QviewYuvH = pMaxCfg->StillQviewWin.Height;

                Rval = SvcStillProc_QueryMem(&ProcSetup, &Size);
                if (SVC_OK == Rval) {
                    if (Size <= StillBuf.AvailSize) {
                        StillBuf.ProcBase = StillBuf.AvailBase;
                        StillBuf.ProcSize = Size;
                        StillBuf.AvailBase += Size;
                        StillBuf.AvailSize -= Size;
                        SvcStillLog("[SetupMemory] proc buffer base %p, size 0x%x", StillBuf.ProcBase, StillBuf.ProcSize, 0, 0, 0);
                    } else {
                        SvcLog_NG(SVC_LOG_STL_TSK, "proc buffer needed 0x%x, but avail 0x%x", Size, StillBuf.AvailSize);
                    }
                }
            }
        }
        if ((TaskSelect & SVC_STILL_MUX) == SVC_STILL_MUX) {
            SVC_STL_MUX_SETUP_s MuxSetup;
            UINT32 MaxMainW = (UINT32)pMaxCfg->StillWin.Width;
            UINT32 MaxMainH = (UINT32)pMaxCfg->StillWin.Height;
            Rval = AmbaWrap_memset(&MuxSetup, 0, sizeof(MuxSetup));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            } else {
                MuxSetup.MaxWidth = MaxMainW;
                MuxSetup.MaxHeight = MaxMainH;
                MuxSetup.MaxScrW = SVC_STL_SCRN_W;
                MuxSetup.MaxScrH = (SVC_STL_SCRN_W * MaxMainH) / MaxMainW;
                MuxSetup.NumFrameBuf = 1;
                Rval = SvcStillMux_QueryMem(&MuxSetup, &Size);
#if 0 // To enable saving raw, user need to prepare more buffer here.
                Size = (pMaxCfg->StillRawCapBufSize > Size)? pMaxCfg->StillRawCapBufSize: Size;
#endif
                if (Size <= StillBuf.AvailSize) {
                    StillBuf.MuxBase = StillBuf.AvailBase;
                    StillBuf.MuxSize = Size;
                    StillBuf.AvailBase += Size;
                    StillBuf.AvailSize -= Size;
                    SvcStillLog("[SetupMemory] mux buffer base %p, size 0x%x", StillBuf.MuxBase, StillBuf.MuxSize, 0, 0, 0);
                    Rval = AmbaExifFormatter_GetHeaderSize(&Size);
                    if (SVC_OK == Rval) {
                        if (Size <= StillBuf.AvailSize) {
                            StillBuf.ExifBase = StillBuf.AvailBase;
                            StillBuf.ExifSize = Size;
                            StillBuf.AvailBase += Size;
                            StillBuf.AvailSize -= Size;
                            SvcStillLog("[SetupMemory] exif buffer base %p, size 0x%x", StillBuf.ExifBase, StillBuf.ExifSize, 0, 0, 0);
                        } else {
                            SvcLog_NG(SVC_LOG_STL_TSK, "exif buffer needed 0x%x, but avail 0x%x", Size, StillBuf.AvailSize);
                        }
                    }
                } else {
                    SvcLog_NG(SVC_LOG_STL_TSK, "mux buffer needed 0x%x, but avail 0x%x", Size, StillBuf.AvailSize);
                }
            }
        }
        if ((TaskSelect & SVC_STILL_ENC) == SVC_STILL_ENC) {
            Size = SIZE_JPEG_Q_TABLE;
            if (Size <= StillBuf.AvailSize) {
                StillBuf.EncQBase = StillBuf.AvailBase;
                StillBuf.EncQSize = Size;
                StillBuf.AvailBase += Size;
                StillBuf.AvailSize -= Size;
                SvcStillLog("[SetupMemory] Q table base %p, size 0x%x", StillBuf.EncQBase, StillBuf.EncQSize, 0, 0, 0);
            } else {
                SvcLog_NG(SVC_LOG_STL_TSK, "Q table needed 0x%x, but avail 0x%x", Size, StillBuf.AvailSize);
            }
        }
    }
    return Rval;
}

UINT32 SvcStillTask_Delete(UINT32 TaskSelect)
{
    UINT32 Rval = SVC_OK;
    if ((TaskSelect & SVC_STILL_CAP) == SVC_STILL_CAP) {
        Rval = SvcStillCap_Delete();
    }
    if ((Rval == SVC_OK) && ((TaskSelect & SVC_STILL_PROC) == SVC_STILL_PROC)) {
        Rval = SvcStillProc_Delete();
    }
    if ((Rval == SVC_OK) && ((TaskSelect & SVC_STILL_ENC) == SVC_STILL_ENC)) {
        Rval = SvcStillEnc_Delete();
    }
    if ((Rval == SVC_OK) && ((TaskSelect & SVC_STILL_MUX) == SVC_STILL_MUX)) {
        Rval = SvcStillMux_Delete();
    }
    return Rval;
}

UINT32 SvcStillTask_Create(UINT32 TaskSelect)
{
    UINT32 Rval = SetupMemory(TaskSelect);

    if ((Rval == SVC_OK) && ((TaskSelect & SVC_STILL_CAP) == SVC_STILL_CAP)) {
        Rval = SvcStillTask_CreateCap();
    }
    if ((Rval == SVC_OK) && ((TaskSelect & SVC_STILL_PROC) == SVC_STILL_PROC)) {
        Rval = SvcStillTask_CreateProc();
    }
    if ((Rval == SVC_OK) && ((TaskSelect & SVC_STILL_ENC) == SVC_STILL_ENC)) {
        Rval = SvcStillTask_CreateEnc();
    }
    if ((Rval == SVC_OK) && ((TaskSelect & SVC_STILL_MUX) == SVC_STILL_MUX)) {
        Rval = SvcStillTask_CreateMux();
    }
    return Rval;
}

UINT32 SvcStillTask_Capture(const SVC_CAP_CTRL_s *pCtrl)
{
    SVC_CAP_CTRL_s CapCtrl;
    UINT32 Rval;

    Rval = AmbaWrap_memset(&CapCtrl, 0, sizeof(CapCtrl));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        Rval = AmbaWrap_memcpy(&CapCtrl, pCtrl, sizeof(SVC_CAP_CTRL_s));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
        } else {
            CapCtrl.Cfg.BufferAddr = StillBuf.CapBase;
            Rval = SvcStillCap_CapStart(&CapCtrl);
        }
    }
    return Rval;
}

#ifndef CONFIG_SOC_H22

#define ACT_R2Y         (1U)
#define ACT_Y2J         (2U)
#define ACT_SAVE_YUV    (3U)
#define ACT_SAVE_JPG    (4U)

#define JPEG_Q_LEVEL    (95)

#define RDY_TO_LIVEVIEW (1)
#define RAW_ENC_DONE    (2)

typedef struct {
    UINT8   Action;
    UINT8   RawCompress;
    UINT8   Res1[2];
    ULONG   Addr;
    ULONG   Addr2;
    UINT16  Pitch;
    UINT16  Width;
    UINT16  Height;
    UINT16  Res2;
    UINT32  Size;
} RAW_ENC_MSG_s;

typedef struct {
    RAW_ENC_MSG_s           Input;
    AMBA_IK_MODE_CFG_s      ImgMode;
    AMBA_DSP_ISOCFG_CTRL_s  IsoCfg;
    AMBA_KAL_MSG_QUEUE_t    RawEncQue;
    RAW_ENC_MSG_s           RawEncQueBuf[SVC_STL_MUX_NUM_FILES];
    SVC_TASK_CTRL_s         TaskCtrl;
    AMBA_KAL_EVENT_FLAG_t   Flag;
    UINT8                   SaveYUV;
    UINT8                   SaveJPEG;
    UINT32                  JpegSize;
    UINT16                  Pitch;
    UINT16                  Width;
    UINT16                  Height;
} RAW_ENC_CTRL_s;

static RAW_ENC_CTRL_s RawEnc GNU_SECTION_NOZEROINIT;

static UINT8  RawEnc_Created = 0;

static inline UINT32 RAWEMC_ALIGN(UINT32 AlignVal, UINT32 Size)
{
    UINT32 RetVal = 0U;

    if ((AlignVal >= 2U) && (Size > 0U)) {
        RetVal = ( Size + ( AlignVal - 1U ) ) & ( ~ ( AlignVal - 1U ) );
    }

    return RetVal;
}

static void R2y(const RAW_ENC_MSG_s *pMsg)
{
    AMBA_DSP_RAW_BUF_s RawInfo;
    AMBA_DSP_BUF_s CeInfo;
    AMBA_DSP_YUV_IMG_BUF_s SspYuvDst;
    AMBA_IK_EXECUTE_CONTAINER_s CfgExecInfo;
    UINT32 Rval;

    Rval = AmbaWrap_memset(&CfgExecInfo, 0, sizeof(CfgExecInfo));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        Rval = AmbaWrap_memset(&RawEnc.IsoCfg, 0, sizeof(RawEnc.IsoCfg));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        } else {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            Rval = SvcStillProc_SetIkTile(&RawEnc.ImgMode, 0x1000U, pMsg->Width, pMsg->Height, pMsg->Width, pMsg->Height);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_TSK, "SvcStillProc_SetIkTile return 0x%x", Rval, 0);
            }
#endif
            Rval = AmbaIK_ExecuteConfig(&RawEnc.ImgMode, &CfgExecInfo);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_TSK, "AmbaIK_ExecuteConfig return 0x%x", Rval, 0);
            }
            if (Rval == OK) {
                RawEnc.IsoCfg.CtxIndex = RawEnc.ImgMode.ContextId;
                RawEnc.IsoCfg.CfgIndex = CfgExecInfo.IkId;
                AmbaMisra_TypeCast(&RawEnc.IsoCfg.CfgAddress, &CfgExecInfo.pIsoCfg);

                SvcLog_DBG(SVC_LOG_STL_TSK, "w %u h %u", pMsg->Width, pMsg->Height);
                SvcLog_DBG(SVC_LOG_STL_TSK, "pitch %u compress %u", pMsg->Pitch, pMsg->RawCompress);

                Rval = AmbaWrap_memset(&RawInfo, 0, sizeof(RawInfo));
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                } else {
                    RawInfo.Compressed = pMsg->RawCompress;     /* 1 - compressed raw data, 0 - uncompressed raw data */
                    RawInfo.BaseAddr = pMsg->Addr;       /* raw buffer address */
                    RawInfo.Pitch = pMsg->Pitch;          /* raw buffer pitch */
                    RawInfo.Window.Width =  pMsg->Width;         /* Window position and size */
                    RawInfo.Window.Height =  pMsg->Height;
                    Rval = AmbaWrap_memset(&CeInfo, 0, sizeof(CeInfo));
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                    } else {
                        if (pMsg->Addr2 != 0U) {
                            UINT16 RawWidth, Pitch;
                            Rval = AmbaDSP_GetCmprRawBufInfo((pMsg->Width >> 2U), (UINT16)pMsg->RawCompress, &RawWidth, &Pitch);
                            CeInfo.Pitch = Pitch;
                        }
                        if (Rval == OK) {
                            CeInfo.BaseAddr = pMsg->Addr2;
                            CeInfo.Window.Width =  pMsg->Width >> 2U;         /* Window position and size */
                            CeInfo.Window.Height =  RawInfo.Window.Height;
                            Rval = AmbaWrap_memset(&SspYuvDst, 0, sizeof(SspYuvDst));
                            if (Rval != OK) {
                                SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                            } else {
                                SspYuvDst.DataFmt = AMBA_DSP_YUV420;
                                SspYuvDst.Pitch = (UINT16)RAWEMC_ALIGN(16U, (UINT32)pMsg->Width);
                                SspYuvDst.Window.Width = pMsg->Width;
                                SspYuvDst.Window.Height = pMsg->Height;
                                SspYuvDst.BaseAddrY = StillBuf.ProcBase;
                                SspYuvDst.BaseAddrUV = StillBuf.ProcBase + ((ULONG)SspYuvDst.Pitch * (ULONG)pMsg->Height);
                                StillBuf.ProcBaseUV = SspYuvDst.BaseAddrUV;

                                SvcClock_FeatureCtrl(1U, SVC_CLK_FEA_BIT_MCTF);
                                SvcLog_OK(SVC_LOG_STL_TSK, "r2y", 0, 0U);
                                Rval = AmbaDSP_StillRaw2Yuv(&RawInfo, &CeInfo, &SspYuvDst, &RawEnc.IsoCfg, 0, NULL);
                                if (OK != Rval) {
                                    SvcLog_NG(SVC_LOG_STL_TSK, "AmbaDSP_StillRaw2Yuv failed! return 0x%x", Rval, 0U);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

static void Y2j(const RAW_ENC_MSG_s *pMsg)
{
    AMBA_DSP_STLENC_CTRL_s EncCtrl;
    const SVC_RES_CFG_s  *pResCfg = SvcResCfg_Get();
    UINT32 Rval;
    ULONG Addr;
    static UINT8 JpegQTable[] GNU_ALIGNED_CACHESAFE = {
        16, 11, 10, 16, 124, 140, 151, 161,
        12, 12, 14, 19, 126, 158, 160, 155,
        14, 13, 16, 24, 140, 157, 169, 156,
        14, 17, 22, 29, 151, 187, 180, 162,
        18, 22, 37, 56, 168, 109, 103, 177,
        24, 35, 55, 64, 181, 104, 113, 192,
        49, 64, 78, 87, 103, 121, 120, 101,
        72, 92, 95, 98, 112, 100, 103, 99 ,
        17, 18, 24, 47, 99 , 99 , 99 , 99 ,
        18, 21, 26, 66, 99 , 99 , 99 , 99 ,
        24, 26, 56, 99, 99 , 99 , 99 , 99 ,
        47, 66, 99, 99, 99 , 99 , 99 , 99 ,
        99, 99, 99, 99, 99 , 99 , 99 , 99 ,
        99, 99, 99, 99, 99 , 99 , 99 , 99 ,
        99, 99, 99, 99, 99 , 99 , 99 , 99 ,
        99, 99, 99, 99, 99 , 99 , 99 , 99
    };
    const UINT8 *pTemp = &JpegQTable[0];

    Rval = AmbaWrap_memset(&EncCtrl, 0, sizeof(AMBA_DSP_STLENC_CTRL_s));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        EncCtrl.YuvBuf.DataFmt = AMBA_DSP_YUV420;
        EncCtrl.YuvBuf.Pitch = (UINT16)RAWEMC_ALIGN(16U, (UINT32)pMsg->Width);
        EncCtrl.YuvBuf.Window.Width = pMsg->Width;
        EncCtrl.YuvBuf.Window.Height = pMsg->Height;
        EncCtrl.YuvBuf.Window.OffsetX = 0;
        EncCtrl.YuvBuf.Window.OffsetY = 0;
        EncCtrl.YuvBuf.BaseAddrY = pMsg->Addr;
        EncCtrl.YuvBuf.BaseAddrUV = pMsg->Addr2;
        Rval = GetEncBuffer();
        if (Rval == OK) {
            EncCtrl.BitsBufAddr = StillBuf.EncBase;
            EncCtrl.BitsBufSize = StillBuf.EncSize;
            EncCtrl.QualityLevel = JPEG_Q_LEVEL;
            SvcStill_CalJpegDqt(&JpegQTable[0], (INT32)EncCtrl.QualityLevel);
            AmbaMisra_TypeCast(&Addr, &pTemp);
            EncCtrl.QTblAddr = Addr;                   /* Pointer to Q-table array, size of each Q-table is 128 bytes */
            Rval = SvcPlat_CacheClean(Addr, sizeof(JpegQTable));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] SvcPlat_CacheClean return 0x%x", __LINE__, Rval);
            } else {
                EncCtrl.EncWidth = pMsg->Width;                   /* Main JPEG width */
                EncCtrl.EncHeight = pMsg->Height;                  /* Main JPEG height */
                SvcClock_FeatureCtrl(1, SVC_CLK_FEA_BIT_CODEC);
                Rval = AmbaDSP_StillEncodeCtrl((UINT16)pResCfg->RecNum, &EncCtrl, NULL);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_TSK, "AmbaDSP_StillEncodeCtrl() failed. return 0x%x", Rval, 0U);
                }
            }
        }
    }
}

static void SaveFile(const RAW_ENC_MSG_s *pMsg)
{
    UINT32 Rval, Size = pMsg->Size, NumSuccess;
    ULONG Addr = pMsg->Addr;
    char FileName[SVC_STL_MUX_MAX_FILENAME_LEN] = {0};
    UINT8 i, FileNum = 1;
    AMBA_VFS_FILE_s File;

    Rval = AmbaDCF_CreateFileName(0, 0, AMBA_DCF_FILE_TYPE_IMAGE, FileName, 0, 0);
    if (OK != Rval) {
        SvcLog_NG(SVC_LOG_STL_TSK, "get file name failed. return 0x%x", Rval, 0);
    }
    if (Rval == OK) {
        if (pMsg->Action == ACT_SAVE_YUV) {
            FileNum = 2;
        }
        for (i = 0; i < FileNum; i ++) {
            if (pMsg->Action == ACT_SAVE_YUV) {
                ULONG StrLen = AmbaUtility_StringLength(FileName);
                if (StrLen > 3UL) {
                    if (i == 0U) {
                        FileName[StrLen - 3U] = 'y';
                        FileName[StrLen - 2U] = '\0';
                    } else {
                        FileName[StrLen - 1U] = 'u';
                        FileName[StrLen]      = 'v';
                        FileName[StrLen + 1U] = '\0';
                        Addr = pMsg->Addr2;
                        Size /= 2U;
                    }
                } else {
                    SvcStillLog("[STL_TSK] SaveFile ERR: StrLen %p", StrLen, 0, 0, 0, 0);
                }
            }
            Rval = AmbaVFS_Open(FileName, "w", 1U, &File);
            if (Rval == OK) {
                ULONG *pBuf;
                AmbaMisra_TypeCast(&pBuf, &Addr);
                Rval = AmbaVFS_Write(pBuf, Size, 1U, &File, &NumSuccess);
            }
            if (Rval == SVC_OK) {
                Rval = AmbaVFS_Sync(&File);
            }
            if (Rval == SVC_OK) {
                Rval = AmbaVFS_Close(&File);
            }
            if (Rval == SVC_OK) {
                SvcLog_OK(FileName, "file closed", 0, 0);
            }
        }
        if (pMsg->Action == ACT_SAVE_JPG) {
            Rval = AmbaKAL_EventFlagSet(&RawEnc.Flag, RAW_ENC_DONE);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_PROC, "Event flag set failed with 0x%x", Rval, 0U);
            }
        }
    }
}

static void* RawEncode_TaskEntry(void* EntryArg)
{
    RAW_ENC_MSG_s Msg;
    UINT32 Rval;
    ULONG ArgVal = 0U;

    AmbaMisra_TouchUnused(EntryArg);

    AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

    while (ArgVal == 0U) {
        Rval = AmbaKAL_MsgQueueReceive(&RawEnc.RawEncQue, &Msg, AMBA_KAL_WAIT_FOREVER);
        if (OK == Rval) {
            if (Msg.Action == ACT_R2Y) {
                R2y(&Msg);
            }
            if (Msg.Action == ACT_Y2J) {
                Y2j(&Msg);
            }
            if ((Msg.Action == ACT_SAVE_YUV) || (Msg.Action == ACT_SAVE_JPG)) {
                SaveFile(&Msg);
            }
        }

        AmbaMisra_TouchUnused(&ArgVal);
    }

    return NULL;
}

static UINT32 YuvDataReadyCb(const void *pEventInfo)
{
    UINT32 Rval;
    const AMBA_DSP_YUV_IMG_BUF_s *pInfo;
    AmbaMisra_TypeCast(&pInfo, &pEventInfo);
    SvcStillLog("[YuvDataReadyCb] y addr %p uv %p Pitch %u w %u h %u", pInfo->BaseAddrY, pInfo->BaseAddrUV, pInfo->Pitch, pInfo->Window.Width, pInfo->Window.Height);
    RawEnc.Pitch = pInfo->Pitch;
    RawEnc.Width = pInfo->Window.Width;
    RawEnc.Height = pInfo->Window.Height;

    Rval = AmbaKAL_EventFlagSet(&RawEnc.Flag, RDY_TO_LIVEVIEW);
    if (OK != Rval) {
        SvcLog_NG(SVC_LOG_STL_PROC, "Event flag set failed with 0x%x", Rval, 0U);
    }
    if (OK == Rval) {
        RawEnc.Input.Action = ACT_Y2J;
        RawEnc.Input.Addr = pInfo->BaseAddrY;
        RawEnc.Input.Addr2 = pInfo->BaseAddrUV;
        Rval = AmbaKAL_MsgQueueSend(&RawEnc.RawEncQue, &RawEnc.Input, 1000);
    }
    if ((OK == Rval) && (RawEnc.SaveYUV == 1U)) {
        RawEnc.Input.Action = ACT_SAVE_YUV;
        RawEnc.Input.Addr = pInfo->BaseAddrY;
        RawEnc.Input.Addr2 = pInfo->BaseAddrUV;
        RawEnc.Input.Size = (UINT32)pInfo->Pitch * (UINT32)pInfo->Window.Height;
        Rval = AmbaKAL_MsgQueueSend(&RawEnc.RawEncQue, &RawEnc.Input, 1000);
    }
    return Rval;
}

static UINT32 JpegDataReadyCb(const void *pEventInfo)
{
    UINT32 Rval;
    AMBA_DSP_ENC_PIC_RDY_s Info;

    Rval = AmbaWrap_memcpy(&Info, pEventInfo, sizeof(AMBA_DSP_ENC_PIC_RDY_s));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
    } else {
        SvcStillLog("[JpegDataReadyCb] addr %p size %u", Info.StartAddr, Info.PicSize, 0, 0, 0);
        RawEnc.JpegSize = Info.PicSize;
        if ((Info.FrameType == PIC_FRAME_JPG) && (RawEnc.SaveJPEG == 1U)) {
            RawEnc.Input.Action = ACT_SAVE_JPG;
            RawEnc.Input.Addr = Info.StartAddr;
            RawEnc.Input.Size = Info.PicSize;
            Rval = AmbaKAL_MsgQueueSend(&RawEnc.RawEncQue, &RawEnc.Input, 1000);
        }
        if (Rval == OK) {
            SvcClock_FeatureCtrl(0, SVC_CLK_FEA_BIT_CODEC);
            if (RawEnc.SaveJPEG == 0U) {
                Rval = AmbaKAL_EventFlagSet(&RawEnc.Flag, RAW_ENC_DONE);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaKAL_EventFlagSet return 0x%x", __LINE__, Rval);
                }
            }
        }
    }
    return Rval;
}

static UINT32 RawEncode_Create(void)
{
    UINT32                  Rval;
    static UINT8            RawEncStack[RAW_ENC_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char             MsgQueName[] = "RawEncQue";
    static char             FlgName[] = "RawEncFlg";

    if (RawEnc_Created == 0U) {
        Rval = SetupMemory(SVC_STILL_CAP | SVC_STILL_PROC | SVC_STILL_ENC | SVC_STILL_MUX);

        if (Rval == OK) {
            Rval = AmbaWrap_memset(&RawEnc, 0, sizeof(RawEnc));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            } else {

                /* task create */
                RawEnc.TaskCtrl.Priority    = RAW_ENC_PRIORITY;
                RawEnc.TaskCtrl.EntryFunc   = RawEncode_TaskEntry;
                RawEnc.TaskCtrl.EntryArg    = 0U;
                RawEnc.TaskCtrl.pStackBase  = RawEncStack;
                RawEnc.TaskCtrl.StackSize   = RAW_ENC_STACK_SIZE;
                RawEnc.TaskCtrl.CpuBits     = RAW_ENC_CORE;

                Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_STL_YUV_DATA_RDY, YuvDataReadyCb);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_TSK, "AmbaDSP_EventHandlerRegister event %u fail, return 0x%x", AMBA_DSP_EVENT_STL_YUV_DATA_RDY, Rval);
                }
                if (Rval == SVC_OK) {
                    Rval = AmbaKAL_MsgQueueCreate(&RawEnc.RawEncQue, MsgQueName, (UINT32)sizeof(SVC_CAP_MSG_s), RawEnc.RawEncQueBuf, (UINT32)sizeof(RawEnc.RawEncQueBuf));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_STL_TSK, "RawEncQue created failed! return 0x%x", Rval, 0U);
                    }
                }
                if (Rval == OK) {
                    Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_JPEG_DATA_RDY, JpegDataReadyCb);
                    if (OK != Rval) {
                        SvcLog_NG(SVC_LOG_STL_TSK, "AmbaDSP_EventHandlerRegister fail, return 0x%x", Rval, 0);
                    }
                }
                if (Rval == SVC_OK) {
                    Rval = AmbaKAL_EventFlagCreate(&RawEnc.Flag, FlgName);
                    if (OK != Rval) {
                        SvcLog_NG(SVC_LOG_STL_TSK, "Event flag create failed with 0x%x", Rval, 0U);
                    }
                }
                if (Rval == SVC_OK) {
                    Rval = AmbaKAL_EventFlagClear(&RawEnc.Flag, 0xFFFFFFFFU);
                    if (OK != Rval) {
                        SvcLog_NG(SVC_LOG_STL_TSK, "Event flag clear failed with 0x%x", Rval, 0U);
                    }
                }
                if (Rval == SVC_OK) {
                    Rval = SvcTask_Create("SvcStillProc", &RawEnc.TaskCtrl);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_STL_TSK, "RawEncode_Create created failed! return 0x%x", Rval, 0U);
                    } else {
                        RawEnc_Created = 1;
                    }
                }
            }
        }
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}

static UINT32 SetIkPipeModeHiso(AMBA_IK_MODE_CFG_s *pIkModeCfg, UINT32 PipeMode)
{
    UINT32 Rval;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    Rval = AmbaIK_SetPipeMode(pIkModeCfg, PipeMode);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_TSK, "AmbaIK_SetPipeMode return 0x%x", Rval, 0);
    }
#else
    Rval = OK;
#endif
    AmbaMisra_TouchUnused(pIkModeCfg);
    AmbaMisra_TouchUnused(&PipeMode);
    return Rval;
}

/**
* get still capture ik ctx id
* @param [in] IsHiso 0 - LISO, 1- HISO
* @param [in] CeNeeded 0 - no ce data needed, 1- ce data needed
* @param [out] pIkCtxId pointer to context id
* @return 0-OK, 1-NG
*/
UINT32 SvcStillTask_GetIkCtxId(UINT8 IsHiso, UINT8 CeNeeded, UINT32 *pIkCtxId)
{
    UINT32 CtxNum, Rval;
    Rval = SvcIKCfg_GetTotalCtxNum(&CtxNum);
    if (Rval == OK) {
        if (IsHiso == 1U) {
            *pIkCtxId = CtxNum - 1U;
        } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            if (CeNeeded == 0U) {
                *pIkCtxId = CtxNum - 3U;
            } else {
                *pIkCtxId = CtxNum - 2U;
            }
#else
            AmbaMisra_TouchUnused(&CeNeeded);
            *pIkCtxId = CtxNum - 2U;
#endif
        }
        SvcLog_OK(SVC_LOG_STL_TSK, "ContextId %u", *pIkCtxId, 0);
    }
    return Rval;
}

/**
* get still capture raw/ce buffer info
* @param [out] pRawInfo pointer to raw buffer info
* @param [out] pCeInfo pointer to ce buffer info
* @param [in] IsHiso 0 - LISO, 1- HISO
* @param [in] AlignOutputHeight 0 - do not align 16 for output height, 1- align 16 for output height
* @return 0-OK, 1-NG
*/
UINT32 SvcStillTask_GetCapBuffer(AMBA_DSP_RAW_BUF_s *pRawInfo, AMBA_DSP_BUF_s *pCeInfo, UINT8 IsHiso, UINT8 AlignOutputHeight)
{
    UINT32 Rval;
    UINT16 Pitch = 0, RawWidth;
    if (StillBuf.CapSize == 0U) {
        SvcStillLog("[SvcStillTask_GetCapBuffer] capture buffer size %u, addr %p", StillBuf.CapSize, StillBuf.CapBase, 0, 0, 0);
        Rval = SVC_NG;
    } else {
        Rval = RawEncode_Create();
        if (Rval == OK) {
            UINT32 CtxNum;
            AMBA_IK_VIN_SENSOR_INFO_s SensorInfo;
            AMBA_IK_WINDOW_SIZE_INFO_s Window;
            Rval = AmbaWrap_memset(&RawEnc.ImgMode, 0, sizeof(RawEnc.ImgMode));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            } else {
                Rval = AmbaWrap_memset(&Window, 0, sizeof(Window));
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_TSK, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                } else {
                    Rval = SvcIKCfg_GetTotalCtxNum(&CtxNum);
                    if (Rval == OK) {
                        if (IsHiso == 1U) {
                            RawEnc.ImgMode.ContextId = CtxNum - 1U;
                        } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                            if (pCeInfo == NULL) {
                                RawEnc.ImgMode.ContextId = CtxNum - 3U;
                            } else {
                                RawEnc.ImgMode.ContextId = CtxNum - 2U;
                            }
#else
                            RawEnc.ImgMode.ContextId = CtxNum - 2U;
#endif
                        }
                        SvcLog_DBG(SVC_LOG_STL_TSK, "ContextId %u", RawEnc.ImgMode.ContextId, 0);
                        Rval = AmbaIK_GetVinSensorInfo(&RawEnc.ImgMode, &SensorInfo);
                        SvcLog_DBG(SVC_LOG_STL_TSK, "get compression %u", SensorInfo.Compression, 0);
                        if (Rval == OK) {
                            Rval = AmbaIK_GetWindowSizeInfo(&RawEnc.ImgMode, &Window);
                            SvcLog_DBG(SVC_LOG_STL_TSK, "    raw w %u h %u", Window.VinSensor.Width, Window.VinSensor.Height);
                            SvcLog_DBG(SVC_LOG_STL_TSK, "    main w %u h %u", Window.Main.Width, Window.Main.Height);
                        }
                        if (Rval == OK) {
                            Rval = AmbaDSP_GetCmprRawBufInfo((UINT16)Window.VinSensor.Width, (UINT16)SensorInfo.Compression, &RawWidth, &Pitch);
                        }
                        if (Rval == OK) {
                            UINT32 PipeMode;

                            RawEnc.Input.Pitch = Pitch;
                            RawEnc.Input.Width = (UINT16)Window.VinSensor.Width;
                            pRawInfo->Window.Width = (UINT16)Window.VinSensor.Width;
                            if (AlignOutputHeight == 1U) {
                                RawEnc.Input.Height = (UINT16)RAWEMC_ALIGN(16U, Window.VinSensor.Height);
                            } else {
                                RawEnc.Input.Height = (UINT16)Window.VinSensor.Height;
                            }
                            pRawInfo->Window.Height = RawEnc.Input.Height;
                            RawEnc.Input.RawCompress = (UINT8)SensorInfo.Compression;
                            pRawInfo->Compressed = (UINT8)SensorInfo.Compression;
                            RawEnc.Input.Addr = StillBuf.CapBase;
                            pRawInfo->BaseAddr = StillBuf.CapBase;
                            SvcStillLog("[SvcStillTask_GetCapBuffer] raw buffer %p", StillBuf.CapBase, 0, 0, 0, 0);
                            if (pCeInfo == NULL) {
                                RawEnc.Input.Addr2 = 0;
                                PipeMode = 0; // linear
                            } else {
                                pCeInfo->BaseAddr = StillBuf.CapBase + ((ULONG)Pitch * (ULONG)pRawInfo->Window.Height);
                                RawEnc.Input.Addr2 = pCeInfo->BaseAddr;
                                SvcStillLog("[SvcStillTask_GetCapBuffer] ce buffer %p", pCeInfo->BaseAddr, 0, 0, 0, 0);
                                PipeMode = 1; // linear + ce
                            }
                            SvcLog_DBG(SVC_LOG_STL_TSK, "w %u h %u", RawEnc.Input.Width, RawEnc.Input.Height);
                            SvcLog_DBG(SVC_LOG_STL_TSK, "pitch %u compress %u", RawEnc.Input.Pitch, RawEnc.Input.RawCompress);
                            if (IsHiso == 1U) {
                                Rval = SetIkPipeModeHiso(&RawEnc.ImgMode, PipeMode);
                            }
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 RawEncode_Delete(void)
{
    UINT32 Rval;

    if (RawEnc_Created == 1U) {
        Rval = SvcTask_Destroy(&RawEnc.TaskCtrl);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_TSK, "RawEncode_Task delete failed! return 0x%x", Rval, 0U);
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_EventFlagDelete(&RawEnc.Flag);
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_MsgQueueDelete(&RawEnc.RawEncQue);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_TSK, "RawEncQue delete failed! return 0x%x", Rval, 0U);
            }
        }
        if (Rval == SVC_OK) {
            Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_STL_YUV_DATA_RDY, YuvDataReadyCb);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_TSK, "AmbaDSP_EventHandlerUnRegister event %u fail, return 0x%x", AMBA_DSP_EVENT_STL_YUV_DATA_RDY, Rval);
            }
        }
        if (Rval == SVC_OK) {
            Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_JPEG_DATA_RDY, JpegDataReadyCb);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_TSK, "AmbaDSP_EventHandlerUnRegister fail, return 0x%x", Rval, 0);
            }
        }
        if (Rval == SVC_OK) {
            RawEnc_Created = 0;
        }
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}

/**
* start still raw encode for Amage
* @param [out] pYuvInfo pointer to AMBA_DSP_YUV_IMG_BUF_s output
* @param [out] pJpgInfo pointer to AMBA_DSP_ENC_PIC_RDY_s output
* @return 0-OK, 1-NG
*/
UINT32 SvcStillTask_RawEncodeAmage(AMBA_DSP_YUV_IMG_BUF_s *pYuvInfo, AMBA_DSP_ENC_PIC_RDY_s *pJpgInfo)
{
    SVC_STL_RAWENC_CTRL_s Ctrl = {.SaveYUV = 0, .SaveJPEG = 0};
    return SvcStillTask_RawEncode(&Ctrl, pYuvInfo, pJpgInfo);
}

/**
* start still raw encode
* @param [in] pCtrl pointer to SVC_STL_RAWENC_CTRL_s input
* @param [out] pYuvInfo pointer to AMBA_DSP_YUV_IMG_BUF_s output
* @param [out] pJpgInfo pointer to AMBA_DSP_ENC_PIC_RDY_s output
* @return 0-OK, 1-NG
*/
UINT32 SvcStillTask_RawEncode(const SVC_STL_RAWENC_CTRL_s *pCtrl, AMBA_DSP_YUV_IMG_BUF_s *pYuvInfo, AMBA_DSP_ENC_PIC_RDY_s *pJpgInfo)
{
    UINT32 Rval, ActualFlags = 0;
    static UINT8 LiveviewStarted = 1;

    Rval = SvcStillTask_Delete(SVC_STILL_PROC | SVC_STILL_ENC | SVC_STILL_MUX);

    if (((Rval == OK) && (pSvcLivStop != NULL)) && (LiveviewStarted == 1U)) {
        Rval = pSvcLivStop();
        LiveviewStarted = 0;
    }

    if (Rval == OK) {
        RawEnc.Input.Action = ACT_R2Y;
        SvcLog_DBG(SVC_LOG_STL_TSK, "w %u h %u", RawEnc.Input.Width, RawEnc.Input.Height);
        SvcLog_DBG(SVC_LOG_STL_TSK, "pitch %u compress %u", RawEnc.Input.Pitch, RawEnc.Input.RawCompress);
        RawEnc.SaveYUV = pCtrl->SaveYUV;
        RawEnc.SaveJPEG = pCtrl->SaveJPEG;
        Rval = AmbaKAL_MsgQueueSend(&RawEnc.RawEncQue, &RawEnc.Input, 1000);
    }

    if (Rval == OK) {
        Rval = AmbaKAL_EventFlagGet(&RawEnc.Flag, RDY_TO_LIVEVIEW, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, 10000);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_STL_TSK, "Event flag get failed with 0x%x", Rval, 0U);
        }
    }

#if 0 //defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    {
        AMBA_IK_CONTEXT_SETTING_s *pCtxSetting;
        UINT32 Addr;
        Rval = SvcIKCfg_GetSetting(&pCtxSetting);
        if (Rval == OK) {
            AmbaMisra_TypeCast(&Addr, &pCtxSetting);
            SvcLog_OK(SVC_LOG_STL_TSK, "SvcIK_GetSetting return 0x%x", Addr, 0U);
            if (pCtxSetting != NULL) {
                Rval = AmbaIK_InitContext(&RawEnc.ImgMode, pCtxSetting->ConfigSetting[RawEnc.ImgMode.ContextId].pAbility);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_TSK, "AmbaIK_InitContext return 0x%x", Rval, 0U);
                }
            }
        }
    }
#else
    if (((Rval == OK) && (pSvcLivStart != NULL)) && (LiveviewStarted == 0U)) {
        Rval = pSvcLivStart();
        LiveviewStarted = 1;
    }
#endif

    if (Rval == OK) {
        Rval = AmbaKAL_EventFlagGet(&RawEnc.Flag, RAW_ENC_DONE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, 10000);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_STL_TSK, "Event flag get failed with 0x%x", Rval, 0U);
        }
    }

    if (Rval == OK) {
        pJpgInfo->PicSize = RawEnc.JpegSize;
        pJpgInfo->StartAddr = StillBuf.EncBase;
        pYuvInfo->BaseAddrY = StillBuf.ProcBase;
        pYuvInfo->BaseAddrUV = StillBuf.ProcBaseUV;
        pYuvInfo->Window.Width  = RawEnc.Width ;
        pYuvInfo->Window.Height = RawEnc.Height;
        pYuvInfo->Pitch = RawEnc.Pitch;
        pYuvInfo->DataFmt = AMBA_DSP_YUV420;
        Rval = RawEncode_Delete();
    }

    if (Rval == OK) {
        SvcLog_OK(SVC_LOG_STL_TSK, "Raw Encode Complete", 0, 0U);
    }

    return Rval;
}

#endif
