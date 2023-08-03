/**
 *  @file RefCV.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details SVC COMSVC CV
 *
 */

#include "AmbaIntrinsics.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaMMU.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "AmbaShell.h"
#include "cvapi_visutil.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"
#include "idsp_vis_msg.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_ambaod.h"
#include "cvapi_svccvalgo_ambaperception.h"
#include "cvapi_amba_od37_fc_category.h"
#include "cvapi_amba_odv37_interface_private.h"



#define RAW_YUV_INPUT
#ifdef RAW_YUV_INPUT
#define FRAME_W                     (1280U)
#define FRAME_H                     (640U)
#define MAX_FILE_Y_SIZE             (0x0200000)
#define MAX_FILE_UV_SIZE            (0x0200000)
#define MAX_FILE_SIZE               (MAX_FILE_Y_SIZE + MAX_FILE_UV_SIZE)
#else
#define FRAME_W                     (3840U)
#define FRAME_H                     (2160U)
#define MAX_FILE_SIZE               (3872*2166*3)
#endif

static SVC_CV_ALGO_HANDLE_s Handler;
static SVC_CV_ALGO_HANDLE_s* Hdlr = &Handler;

static AmbaPcptCfgNext NextStepCfg;
//#define DBG {AmbaPrint_PrintInt5("UT@%d", __LINE__, 0, 0, 0, 0);AmbaKAL_TaskSleep(50);}
//extern UINT32 RefCV_UT_GetCVBuf(UINT8** ppU8, UINT32 Size, UINT32* AlignedSize);

static inline UINT32 ALIGN32(UINT32 X) {return ((X + 31U) & 0xFFFFFFE0U);}

static void _DumpCLResult(const SVC_CV_ALGO_OUTPUT_s *pEventData, const char* InPath, UINT32 CLType)
{
    const UINT8 *pList = NULL, *pTS = NULL;
    UINT8 *pTop1 = NULL;
    const AmbaPcptCfgNext *pNext;
    UINT32 i, Num, MaxBatch = 0;
    AMBA_FS_FILE *FP = NULL;
    ULONG Addr;

    AmbaMisra_TypeCast(&pNext, &(pEventData->pExtOutput));

    switch (CLType) {
    case CALLBACK_EVENT_TS_OUTPUT:
        pList = pNext->RunObjsTS;
        AmbaMisra_TypeCast(&pTop1, &(pNext->pTSOut->buf[0].pBuffer));
        // TS and SL share the same batch.
        MaxBatch = AMBANET_ODV37_TS_BATCH_SZ - 32U;
        break;
    case CALLBACK_EVENT_SL_OUTPUT:
        pList = pNext->RunObjsTS;
        AmbaMisra_TypeCast(&pTS, &(pNext->pTSOut->buf[0].pBuffer));
        AmbaMisra_TypeCast(&Addr, &pTS);
        Addr = Addr + (ULONG) (ALIGN32(AMBANET_ODV37_TS_BATCH_SZ));
        AmbaMisra_TypeCast(&pTop1, &Addr);
        // TS and SL share the same batch.
        MaxBatch = AMBANET_ODV37_TS_BATCH_SZ - 32U;
        break;
    case CALLBACK_EVENT_TLC_OUTPUT:
        pList = pNext->RunObjsTL;
        AmbaMisra_TypeCast(&pTop1, &(pNext->pTLCOut->buf[0].pBuffer));
        MaxBatch = AMBANET_ODV37_TL_BATCH_SZ;
        break;
    case CALLBACK_EVENT_TLS_OUTPUT:
        pList = pNext->RunObjsTL;
        AmbaMisra_TypeCast(&pTop1, &(pNext->pTLSOut->buf[0].pBuffer));
        MaxBatch = AMBANET_ODV37_TL_BATCH_SZ;
        break;
    case CALLBACK_EVENT_AR_OUTPUT:
        pList = pNext->RunObjsAR;
        AmbaMisra_TypeCast(&pTop1, &(pNext->pAROut->buf[0].pBuffer));
        MaxBatch = AMBANET_ODV37_AR_BATCH_SZ;
        break;
    case CALLBACK_EVENT_VB_OUTPUT:
        pList = pNext->RunObjsVB;
        AmbaMisra_TypeCast(&pTop1, &(pNext->pVBOut->buf[0].pBuffer));
        MaxBatch = AMBANET_ODV37_VB_BATCH_SZ;
        break;
    default:
        AmbaPrint_PrintUInt5("Unknown type: 0x%x", CLType, 0U, 0U, 0U, 0U);
        break;
    }

    if ((pList != NULL) && (pTop1 != NULL)) {
        (void) AmbaFS_FileOpen(InPath, "wb", &FP);
        if (FP != NULL) {
            for(i = 0; i < MaxBatch; i++) {
                if (pList[i] == 0xFFU) {
                    break;
                }
                if ((CLType == CALLBACK_EVENT_SL_OUTPUT) && (pTS != NULL)) {
                    if (pTS[i] == AMBANET_TSC3_CAT0_SPEEDLIMIT) {
                        AmbaPrint_PrintUInt5("obj: %d, speedlimit: %d", pList[i], pTop1[i], 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("obj: %d, top-1: %d", pList[i], pTop1[i], 0U, 0U, 0U);
                }
            }

            (void)AmbaFS_FileWrite(pTop1, i, 1U, FP, &Num);

            (void)AmbaFS_FileClose(FP);
        }
    }
}

static void _DumpTRResult(const SVC_CV_ALGO_OUTPUT_s *pEventData, const char* InPath)
{
    amba_odv37_step9_out_t *pTROut;
    UINT32 Num;
    AMBA_FS_FILE *FP = NULL;

    AmbaMisra_TypeCast(&pTROut, &pEventData->pExtOutput);

    AmbaPrint_PrintUInt5("Receive TR output: %d",
                         pTROut->num_objects, 0U, 0U, 0U, 0U);

    (void) AmbaFS_FileOpen(InPath, "wb", &FP);
    if (FP != NULL) {
        (void)AmbaFS_FileWrite(pTROut,
                               ALIGN128(sizeof(amba_odv37_step9_out_t)) +
                               ((UINT32)AMBA_ODV37_TRACK_FEATURE_SIZE * pTROut->num_objects),
                               1U, FP, &Num);
        (void)AmbaFS_FileClose(FP);
    }
}

static void _DumpMKResult(const SVC_CV_ALGO_OUTPUT_s *pEventData, const char* InPath)
{
    amba_is_out_t *pMKOut;
    const UINT32 *pAddr;
    const UINT16 *pW, *pH;
    UINT32 i, Num, Addr;
    AMBA_FS_FILE *FP = NULL;
    void *pVoid;

    AmbaMisra_TypeCast(&pMKOut, &pEventData->pExtOutput);
    AmbaMisra_TypeCast(&Addr, &pMKOut);
    i = Addr + pMKOut->list_offset_width;
    AmbaMisra_TypeCast(&pW, &i);
    i = Addr + pMKOut->list_offset_height;
    AmbaMisra_TypeCast(&pH, &i);
    i = Addr + pMKOut->list_offset_address;
    AmbaMisra_TypeCast(&pAddr, &i);

    AmbaPrint_PrintUInt5("Receive MK output: %d",
                         pMKOut->num_objects, 0U, 0U, 0U, 0U);

    (void) AmbaFS_FileOpen(InPath, "wb", &FP);
    if (FP != NULL) {
        (void)AmbaFS_FileWrite(pMKOut, ALIGN128(sizeof(amba_is_out_t)) +
                               (ALIGN32(AMBANET_ODV37_MK_BATCH_SZ) * 8U),
                               1U, FP, &Num);
        for (i = 0; i < pMKOut->num_objects; i++) {
            //AmbaPrint_PrintUInt5("output @ 0x%x", *pAddr, 0U, 0U, 0U, 0U);
            Addr = *pAddr;
            AmbaMisra_TypeCast(&pVoid, &Addr);
            (void)AmbaFS_FileWrite(pVoid, (UINT32) (ALIGN32(*pW) * (*pH)), 1U, FP, &Num);
            pAddr++;
            pW++;
            pH++;
        }
        (void)AmbaFS_FileClose(FP);
    }
}

static void _Dump3DResult(const SVC_CV_ALGO_OUTPUT_s *pEventData, const char* InPath)
{
    amba_odv37_step8_out_t *p3DOut;
    obj_t *p3DObj;
    UINT32 Num;
    AMBA_FS_FILE *FP = NULL;
    UINT32 Addr;

    AmbaMisra_TypeCast(&p3DOut, &pEventData->pExtOutput);
    AmbaMisra_TypeCast(&Addr, &p3DOut);
    Addr = Addr + p3DOut->objects_offset;
    AmbaMisra_TypeCast(&p3DObj, &Addr);

    AmbaPrint_PrintUInt5("Receive 3D output: %d",
                         p3DOut->num_objects, 0U, 0U, 0U, 0U);

    (void) AmbaFS_FileOpen(InPath, "wb", &FP);
    if (FP != NULL) {
        (void)AmbaFS_FileWrite(p3DOut,
                               ALIGN128(sizeof(amba_odv37_step8_out_t)),
                               1U, FP, &Num);
        (void)AmbaFS_FileWrite(p3DObj, sizeof(obj_t) * p3DOut->num_objects,
                               1U, FP, &Num);
        (void)AmbaFS_FileClose(FP);
    }
}

static void _DumpKPResult(const SVC_CV_ALGO_OUTPUT_s *pEventData, const char* InPath)
{
    amba_kp_out_t *pKPOut;
    UINT32 Num;
    AMBA_FS_FILE *FP = NULL;

    AmbaMisra_TypeCast(&pKPOut, &pEventData->pExtOutput);

    AmbaPrint_PrintUInt5("Receive KP output: %d",
                         pKPOut->num_objects, 0U, 0U, 0U, 0U);

    (void) AmbaFS_FileOpen(InPath, "wb", &FP);
    if (FP != NULL) {
        (void)AmbaFS_FileWrite(pKPOut,
                               ALIGN128(sizeof(amba_kp_out_t)) +
                               (sizeof(amba_kp_candidate_t) * pKPOut->num_objects),
                               1U, FP, &Num);
        (void)AmbaFS_FileClose(FP);
    }
}

static void _DumpODResult(const SVC_CV_ALGO_OUTPUT_s *pEventData, const char* InPath)
{
    static const char* kitti_12c_name[] = {
        "Person",
        "Rider",
        "Bicycle",
        "Motorcycle",
        "Car",
        "Truck",
        "Red",
        "Green",
        "Sign",
        "Other",
        "Arrow",
        "SpeedBump",
        "Crosswalk",
        "StopLine",
        "YieldLine",
        "YieldMark"
    };
    static UINT32 OutCount = 0;
    UINT32 i, j, k, l, m, n, o, p, q;
    UINT32 BBXAddr;
    const amba_od_candidate_t* Obj;
    const amba_od_out_t *BBX;
    char OutputStr[128];
    char TempStr[128];
    char OutFN[64];
    UINT32 Arg[5];
    UINT32 Num;
    AMBA_FS_FILE *FP = NULL;
    AmbaPcptCfgNext *pNext;

    AmbaMisra_TypeCast(&BBX, &(pEventData->pOutput->buf[0].pBuffer));
    AmbaMisra_TypeCast(&pNext, &(pEventData->pExtOutput));

    AmbaUtility_StringCopy(OutFN, 64U, InPath);
    AmbaUtility_StringAppendUInt32(OutFN, 64U, OutCount, 10U);
    AmbaUtility_StringAppend(OutFN, 64U, ".txt");
    (void) AmbaFS_FileOpen(OutFN, "wb", &FP);
    AmbaPrint_PrintUInt5("Bbx PTS = %d FN: %d NumObj %d",
                         BBX->capture_time,
                         BBX->frame_num,
                         BBX->num_objects, 0U, 0U);

    AmbaMisra_TypeCast(&BBXAddr, &BBX);
    BBXAddr = BBXAddr + BBX->objects_offset;
    AmbaMisra_TypeCast(&Obj, &BBXAddr);

    j = 0U;
    k = 0U;
    l = 0U;
    m = 0U;
    n = 0U;
    o = 0U;
    p = 0U;
    q = 0U;
    for (i = 0U; i < BBX->num_objects; i++) {
        AmbaPrint_PrintUInt5("Bbx cls = %d @ %d %d %d %d",
                             Obj->clsId, (UINT32) Obj->bb_start_col,
                             (UINT32) Obj->bb_start_row, Obj->bb_width_m1,
                             Obj->bb_height_m1);
        AmbaUtility_StringCopy(OutputStr, 128U, kitti_12c_name[Obj->clsId]);
        Arg[0] = (UINT32)Obj->bb_start_col;
        Arg[1] = (UINT32)Obj->bb_start_row;
        Arg[2] = (UINT32)Obj->bb_width_m1 + 1U;
        Arg[3] = (UINT32)Obj->bb_height_m1 + 1U;
        Arg[4] = (UINT32)Obj->score;
        (void)AmbaUtility_StringPrintUInt32(TempStr, 128U,  " 0 0 0 %d %d %d %d 0 0 0 0 0 0 0 %d\n", 5U, Arg);
        AmbaUtility_StringAppend(OutputStr, 128U, TempStr);
        //AmbaPrint_PrintStr5("%s", OutputStr, NULL, NULL, NULL, NULL);
        if (FP != NULL) {
            (void)AmbaFS_FileWrite(OutputStr, AmbaUtility_StringLength(OutputStr),
                                   1U, FP, &Num);
        }

        /* Prepare the list for sub networks (KP, 3D, MK). */
        if ((Obj->clsId == AMBANET_OD37_FC_CAT0_PERSON) ||
            (Obj->clsId == AMBANET_OD37_FC_CAT1_RIDER)) {
            if (j < AMBANET_ODV37_KP_BATCH_SZ) {
                pNext->RunObjsKP[j] = (UINT8) i;
                j++;
            }
        }

        if ((Obj->clsId == AMBANET_OD37_FC_CAT2_BICYCLE) ||
            (Obj->clsId == AMBANET_OD37_FC_CAT3_MOTORCYCLE) ||
            (Obj->clsId == AMBANET_OD37_FC_CAT4_CAR) ||
            (Obj->clsId == AMBANET_OD37_FC_CAT5_TRUCK)) {
            if (k < AMBANET_ODV37_3D_BATCH_SZ) {
                pNext->RunObjs3D[k] = (UINT8) i;
                k++;
            }
        }

        if ((Obj->clsId != AMBANET_OD37_FC_CAT6_RED) &&
            (Obj->clsId != AMBANET_OD37_FC_CAT7_GREEN)) {
            if (l < AMBANET_ODV37_MK_BATCH_SZ)  {
                pNext->RunObjsMK[l] = (UINT8) i;
                l++;
            }
        }

        if (Obj->clsId == AMBANET_OD37_FC_CAT8_SIGN) {
            if (m < AMBANET_ODV37_TS_BATCH_SZ) {
                pNext->RunObjsTS[m] = (UINT8) i;
                m++;
            }
        }

        if ((Obj->clsId == AMBANET_OD37_FC_CAT6_RED) ||
            (Obj->clsId == AMBANET_OD37_FC_CAT7_GREEN)) {
            if (n < AMBANET_ODV37_TL_BATCH_SZ) {
                pNext->RunObjsTL[n] = (UINT8) i;
                n++;
            }
        }

        if (Obj->clsId == AMBANET_OD37_FC_CAT10_ARROW) {
            if (o < AMBANET_ODV37_AR_BATCH_SZ) {
                pNext->RunObjsAR[o] = (UINT8) i;
                o++;
            }
        }

        if ((Obj->clsId == AMBANET_OD37_FC_CAT2_BICYCLE) ||
            (Obj->clsId == AMBANET_OD37_FC_CAT3_MOTORCYCLE) ||
            (Obj->clsId == AMBANET_OD37_FC_CAT4_CAR) ||
            (Obj->clsId == AMBANET_OD37_FC_CAT5_TRUCK)) {
            if (p < AMBANET_ODV37_VB_BATCH_SZ) {
                pNext->RunObjsVB[p] = (UINT8) i;
                p++;
            }
        }

        if (q < AMBANET_ODV37_TR_BATCH_SZ) {
            pNext->RunObjsTR[q] = (UINT8) i;
            q++;
        }

        Obj = &Obj[1];
    }

    /* Use 0xFF to terminate. */
    if (j < AMBANET_ODV37_KP_BATCH_SZ) {
        pNext->RunObjsKP[j] = 0xFF;
    }
    if (k < AMBANET_ODV37_3D_BATCH_SZ) {
        pNext->RunObjs3D[k] = 0xFF;
    }
    if (l < AMBANET_ODV37_MK_BATCH_SZ) {
        pNext->RunObjsMK[l] = 0xFF;
    }
    if (m < AMBANET_ODV37_TS_BATCH_SZ) {
        pNext->RunObjsTS[m] = 0xFF;
    }
    if (n < AMBANET_ODV37_TL_BATCH_SZ) {
        pNext->RunObjsTL[n] = 0xFF;
    }
    if (o < AMBANET_ODV37_AR_BATCH_SZ) {
        pNext->RunObjsAR[o] = 0xFF;
    }
    if (p < AMBANET_ODV37_VB_BATCH_SZ) {
        pNext->RunObjsVB[p] = 0xFF;
    }
    if (q < AMBANET_ODV37_TR_BATCH_SZ) {
        pNext->RunObjsTR[q] = 0xFF;
    }

    if (FP != NULL) {
        (void)AmbaFS_FileClose(FP);
    }
}

static char* _GetNextFile(const char* InPath)
{
    static char FN[128];
    static AMBA_FS_FILE *FP = NULL;
    static UINT32 FL_EOF = 0U;
    UINT32 Rval = 0U;
    UINT32 Cnt = 0U;
    UINT32 RbNum;
    char* Rp;
    char Ch[1];
    UINT32 FileNameEnd;
    if (FL_EOF == 0U) {
        if (FP == NULL) {
            Rval = AmbaFS_FileOpen(InPath, "rb", &FP);
            if (Rval != 0U) {
                AmbaPrint_PrintUInt5("_GetNextFile open failed", 0U, 0U, 0U, 0U, 0U);
            }
        }
        FileNameEnd = 0U;
        while(FileNameEnd == 0U) {
            if (Rval == 0U) {
                Rval = AmbaFS_FileRead(Ch, 1U, 1U, FP, &RbNum);
                if ((Rval != 0U) || (RbNum == 0U)) {
                    AmbaPrint_PrintUInt5("End of file list", 0U, 0U, 0U, 0U, 0U);
                    (void) AmbaFS_FileClose(FP);
                    FL_EOF = 1U;
                    FP = NULL;
                    FileNameEnd = 1U;
                } else {
                    if ((Ch[0] == '\n') ||
                        (Ch[0] == '\r') ||
                        (Ch[0] == '\0')) {
                        // new line
                        if (Cnt == 0U) {
                            continue;
                        } else {
                            FileNameEnd = 1U;
                        }
                    } else {
                        // normal char
                        FN[Cnt] = Ch[0];
                        Cnt++;
                    }
                }
            }
        }
        Cnt++;
        FN[Cnt] = '\0';
        Rp = FN;
    } else {
        Rp = NULL;
    }
    if (Cnt == 1U) {
        Rp = NULL;
    }
    return Rp;
}

static void _ReadInputFile(const char* path, void* buf)
{
    UINT32 Ret;
    UINT32 Fsize = 0U, OpRes;
    UINT64 Fpos = 0U;
    UINT32 U32Buf;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintStr5("_ReadInputFile AmbaFS_FileOpen fail: %s",
                            path, NULL, NULL, NULL, NULL);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileRead(buf, 1U, (UINT32)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileRead fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        AmbaMisra_TypeCast(&U32Buf, &buf);
        Ret = AmbaCache_DataClean(U32Buf & CACHE_LINE_MASK, (Fsize + CACHE_LINE_SIZE - 1U) & CACHE_LINE_MASK);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaCache_DataClean fail (0x%x, %d)", U32Buf & CACHE_LINE_MASK, (Fsize + CACHE_LINE_SIZE - 1U) & CACHE_LINE_MASK, 0U, 0U, 0U);
        }
    }
}

static void _Config_SendLicense(SVC_CV_ALGO_HANDLE_s* pHdlr, const char *LicenseFn)
{
    void* vp;
    UINT32 msg[258];
    const UINT32* pU32;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    msg[0] = AMBANET_MSG_TYPE_LICENSE; // it's a user defined message code which should be the same as used in visorc code
    pU32 = &msg[1];
    AmbaMisra_TypeCast(&vp, &pU32);
    _ReadInputFile(LicenseFn, vp);

    pU32 = &msg[0];
    MsgCfg.CtrlType = 0;
    MsgCfg.pExtCtrlCfg = NULL;
    AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pU32);
    (void)SvcCvAlgo_Control(pHdlr, &MsgCfg);
}


static void _Config_AmbaPcpt(SVC_CV_ALGO_HANDLE_s* pHdlr, const char* LicenseFn)
{
    AMBA_FS_FILE *Fp;
    const char *pCfgFile = "ambaod37_cfg_mt.bin";
    uint32_t BytesRead, FileSize;
    uint32_t Ret;
    uint8_t Buf[sizeof(amba_od_nms_configs_t) + \
            sizeof(amba_od_grouping_t) + \
            (sizeof(ambanet_roi_config_t) * 2U)];
    uint8_t *pBuf = Buf;
    const amba_od_nms_configs_t *pNms;
    const amba_od_grouping_t *pGroup;
    ambanet_roi_config_t *pRoiCfg[2];          // For ARM
    amba_roi_config_t RoiMsg;
    const amba_roi_config_t *pRoiMsg = &RoiMsg;     // For ORC
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    _Config_SendLicense(pHdlr, LicenseFn);

    MsgCfg.pExtCtrlCfg = NULL;
    MsgCfg.CtrlType = 0;

    Ret = AmbaFS_FileOpen(pCfgFile, "rb", &Fp);
    {
        /* To fix Misra-C not initialized variable issue. */
        RoiMsg.msg_type            = AMBA_ROI_CONFIG_MSG;
        RoiMsg.image_pyramid_index = 0;
        RoiMsg.source_vin          = 0;
        RoiMsg.roi_start_col       = 0;
        RoiMsg.roi_start_row       = 0;
        RoiMsg.roi_width           = 0;
        RoiMsg.roi_height          = 0;
    }

    if (Ret != 0U) {
        AmbaPrint_PrintStr5("can't read config file!!", NULL, NULL, NULL, NULL, NULL);
    } else {
        (void) AmbaCV_UtilityFileSize(pCfgFile, &FileSize);

        Ret = AmbaFS_FileRead(pBuf, 1U, (UINT32)FileSize, Fp, &BytesRead);
        if ((BytesRead == 0U) || (Ret !=  0U)) {
            AmbaPrint_PrintUInt5("Error while reading file!! (%d)", Ret, 0U, 0U, 0U, 0U);
        }

        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("Error while closing file!! (%d)", Ret, 0U, 0U, 0U, 0U);
        }

        AmbaMisra_TypeCast(&pNms, &pBuf);

        pBuf     = &Buf[sizeof(amba_od_nms_configs_t)];
        AmbaMisra_TypeCast(&pGroup, &pBuf);

        pBuf     = &Buf[sizeof(amba_od_nms_configs_t) + \
                        sizeof(amba_od_grouping_t)];
        AmbaMisra_TypeCast(&pRoiCfg[0], &pBuf);

        pBuf     = &Buf[sizeof(amba_od_nms_configs_t) + \
                        sizeof(amba_od_grouping_t) + \
                        sizeof(ambanet_roi_config_t)];
        AmbaMisra_TypeCast(&pRoiCfg[1], &pBuf);

        RoiMsg.msg_type                = AMBA_ROI_CONFIG_MSG;
        RoiMsg.image_pyramid_index     = pRoiCfg[0]->pyramid_index;
        RoiMsg.source_vin              = 0;
        RoiMsg.roi_start_col           = pRoiCfg[0]->roi_start_col;
        RoiMsg.roi_start_row           = pRoiCfg[0]->roi_start_row;
        RoiMsg.roi_width               = pRoiCfg[0]->roi_width;
        RoiMsg.roi_height              = pRoiCfg[0]->roi_height;

        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pRoiMsg);
        (void)SvcCvAlgo_Control(pHdlr, &MsgCfg);
        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pNms);
        (void)SvcCvAlgo_Control(pHdlr, &MsgCfg);
        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pGroup);
        (void)SvcCvAlgo_Control(pHdlr, &MsgCfg);
        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pRoiCfg[0]);
        (void)SvcCvAlgo_Control(pHdlr, &MsgCfg);
     }
}

static void _GetFileSize(const char* path, UINT32* Size)
{
    UINT32 Ret;
    UINT32 Fsize = 0U;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintStr5("_GetFileSize AmbaFS_FileOpen fail: %s",
                            path, NULL, NULL, NULL, NULL);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_GetFileSize AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_GetFileSize AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_GetFileSize AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    *Size = Fsize;
}

static void _FiLLFlexiDagMemStruct(const UINT8* Addr, UINT32 Size, flexidag_memblk_t *MemBlk)
{
    ULONG addr, Paddr = 0U;

    MemBlk->buffer_cacheable = 1;
    AmbaMisra_TypeCast(&MemBlk->pBuffer, &Addr);
    MemBlk->buffer_size = Size;
    AmbaMisra_TypeCast(&addr, &Addr);
    (void) AmbaMMU_VirtToPhys(addr, &Paddr);
    MemBlk->buffer_daddr = Paddr;
}

static UINT32 _LoadFlexiDagBin(const char* path, flexidag_memblk_t* MemBlk)
{

    UINT8 *pU8;
    UINT32 BinSize;
    UINT32 AlignedBinSize;
    UINT32 ret = 0U;
    ULONG addr, Paddr = 0U;

    _GetFileSize(path, &BinSize);

    if (BinSize != 0U) {

        ret = RefCV_UT_GetCVBuf(&pU8, BinSize, &AlignedBinSize);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("_LoadFlexiDagBin: OOM", 0U, 0U, 0U, 0U, 0U);
        }

        if (ret == 0U) {
            //assign memory
            MemBlk->buffer_cacheable = 1;
            AmbaMisra_TypeCast(&MemBlk->pBuffer, &pU8);
            MemBlk->buffer_size = AlignedBinSize;
            AmbaMisra_TypeCast(&addr, &pU8);
            (void) AmbaMMU_VirtToPhys(addr, &Paddr);
            MemBlk->buffer_daddr = Paddr;
            ret = AmbaCV_UtilityFileLoad(path, MemBlk);
            if (ret != 0U) {
                AmbaPrint_PrintUInt5("_LoadFlexiDagBin: AmbaCV_UtilityFileLoad failed:%x", ret, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        AmbaPrint_PrintUInt5("_LoadFlexiDagBin: Open flexibin failed", 0U, 0U, 0U, 0U, 0U);
        ret = 1U;
    }
    return ret;
}

static UINT32 OutputNum;
static UINT32 OutputSz[8];
static AMBA_KAL_MSG_QUEUE_t PcptUTQueue;

static UINT32 SCA_AmbaPcpt_UT_CB(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    static char OutODFn[64]  = "odout_";
    static char OutKPFn[64]  = "kpout.bin";
    static char Out3DFn[64]  = "3d_obj_out.bin";
    static char OutMKLFn[64] = "mklout.bin";
    static char OutMKSFn[64] = "mksout.bin";
    static char OutTSFn[64]  = "tsout.bin";
    static char OutSLFn[64]  = "slout.bin";
    static char OutTLCFn[64]  = "tlcout.bin";
    static char OutTLSFn[64]  = "tlsout.bin";
    static char OutARFn[64]  = "arout.bin";
    static char OutVBFn[64]  = "vbout.bin";
    static char OutTRFn[64]  = "trout.bin";

    if (Event == CALLBACK_EVENT_FREE_INPUT) {
        AmbaPrint_PrintUInt5("Free the input", 0U, 0U, 0U, 0U, 0U);

        if (KAL_ERR_NONE != AmbaKAL_MsgQueueSend(&PcptUTQueue, &Event, 5000)) {
            AmbaPrint_PrintUInt5("CB MsgQueueSend timeout", 0U, 0U, 0U, 0U, 0U);
        }
    } else if (Event == CALLBACK_EVENT_OUTPUT) {
        _DumpODResult(pEventData, OutODFn);
    } else if (Event == CALLBACK_EVENT_KP_OUTPUT) {
        _DumpKPResult(pEventData, OutKPFn);
    } else if (Event == CALLBACK_EVENT_3D_OUTPUT) {
        _Dump3DResult(pEventData, Out3DFn);
    } else if (Event == CALLBACK_EVENT_MKL_OUTPUT) {
        _DumpMKResult(pEventData, OutMKLFn);
    } else if (Event == CALLBACK_EVENT_MKS_OUTPUT) {
        _DumpMKResult(pEventData, OutMKSFn);
    } else if (Event == CALLBACK_EVENT_TS_OUTPUT) {
        _DumpCLResult(pEventData, OutTSFn, CALLBACK_EVENT_TS_OUTPUT);
    } else if (Event == CALLBACK_EVENT_SL_OUTPUT) {
        _DumpCLResult(pEventData, OutSLFn, CALLBACK_EVENT_SL_OUTPUT);
    } else if (Event == CALLBACK_EVENT_TLC_OUTPUT) {
        _DumpCLResult(pEventData, OutTLCFn, CALLBACK_EVENT_TLC_OUTPUT);
    } else if (Event == CALLBACK_EVENT_TLS_OUTPUT) {
        _DumpCLResult(pEventData, OutTLSFn, CALLBACK_EVENT_TLS_OUTPUT);
    } else if (Event == CALLBACK_EVENT_AR_OUTPUT) {
        _DumpCLResult(pEventData, OutARFn, CALLBACK_EVENT_AR_OUTPUT);
    } else if (Event == CALLBACK_EVENT_VB_OUTPUT) {
        _DumpCLResult(pEventData, OutVBFn, CALLBACK_EVENT_VB_OUTPUT);
    } else if (Event == CALLBACK_EVENT_TR_OUTPUT) {
        _DumpTRResult(pEventData, OutTRFn);
    } else {
        AmbaPrint_PrintUInt5("Unknown callback event: %d", Event, 0U, 0U, 0U, 0U);
    }

    return 0U;
}

static void SCA_AmbaPcpt_UT_Init(char * const * pArgVector, UINT32 Count)
{
    static UINT32 Init = 0U;
    static flexidag_memblk_t AlgoBuf;
    static flexidag_memblk_t FDBinBuf[AMBANET_ODV37_MAX_FD];

    UINT32 AlignSz = 0U;
    UINT32 Ret;
    UINT8* pAlgoBuf = NULL;
    UINT32 AlgoBufSz = 0U;
    SVC_CV_ALGO_QUERY_CFG_s QCfg;
    SVC_CV_ALGO_CREATE_CFG_s CCfg;
    ExtTaskCreateCfg ExtCfg;
    CCF_TSK_CTRL_s Ctrl;

    if (Init == 0U) {

        (void)AmbaWrap_memset(Hdlr, 0, sizeof(SVC_CV_ALGO_HANDLE_s));
        (void)AmbaWrap_memset(&QCfg, 0, sizeof(SVC_CV_ALGO_QUERY_CFG_s));
        (void)AmbaWrap_memset(&CCfg, 0, sizeof(SVC_CV_ALGO_CREATE_CFG_s));
        (void)AmbaWrap_memset(&ExtCfg, 0, sizeof(ExtTaskCreateCfg));
        (void)AmbaWrap_memset(&Ctrl, 0, sizeof(CCF_TSK_CTRL_s));

        ExtCfg.MagicCode    = ExtTaskCreateCfgMagic;

        Ctrl.CoreSel        = 0xE;
        Ctrl.Priority       = 50;
        Ctrl.StackSz        = ((UINT32)16U << 10U); //16K
        ExtCfg.RunTskCtrl   = Ctrl;

        Ctrl.CoreSel        = 0xE;
        Ctrl.Priority       = 51;
        Ctrl.StackSz        = ((UINT32)16U << 10U); //16K
        ExtCfg.CBTskCtrl    = Ctrl;

        ExtCfg.ROIs         = 1;
        ExtCfg.ODSize       = SCA_CT_AMBAOD_SIZE1;

        CCfg.pExtCreateCfg = &ExtCfg;

        QCfg.pAlgoObj = &AmbaPcptAlgoObj;
        AlignSz = 0x03000001;
        QCfg.pExtQueryCfg = &AlignSz;
        Ret = SvcCvAlgo_Query(Hdlr, &QCfg);

        if (Ret == 0U) {
            AlgoBufSz = QCfg.TotalReqBufSz;
            Ret = RefCV_UT_GetCVBuf(&pAlgoBuf, AlgoBufSz, &AlignSz);
            AmbaPrint_PrintUInt5("Alloc Algo Buf Sz:%d", AlignSz, 0U, 0U, 0U, 0U);
        }

        if (Ret == 0U) {
            UINT32 i = 0;

            CCfg.NumFD = Count;

            CCfg.pAlgoBuf = &AlgoBuf;
            _FiLLFlexiDagMemStruct(pAlgoBuf, AlgoBufSz, &AlgoBuf);

            CCfg.pBin[AMBANET_OD37_2D] = &FDBinBuf[i];
            (void)_LoadFlexiDagBin(pArgVector[i + 2U], CCfg.pBin[AMBANET_OD37_2D]);
            AmbaPrint_PrintStr5("FD: %s", pArgVector[i + 2U], NULL, NULL, NULL, NULL);
            i++;

            if (NextStepCfg.EnableKP == 1U) {
                CCfg.pBin[AMBANET_OD37_KP] = &FDBinBuf[i];
                (void)_LoadFlexiDagBin(pArgVector[i + 2U], CCfg.pBin[AMBANET_OD37_KP]);
                AmbaPrint_PrintStr5("FD: %s", pArgVector[i + 2U], NULL, NULL, NULL, NULL);
                i++;
            }

            if (NextStepCfg.Enable3D == 1U) {
                CCfg.pBin[AMBANET_OD37_3D] = &FDBinBuf[i];
                (void)_LoadFlexiDagBin(pArgVector[i + 2U], CCfg.pBin[AMBANET_OD37_3D]);
                AmbaPrint_PrintStr5("FD: %s", pArgVector[i + 2U], NULL, NULL, NULL, NULL);
                i++;
            }

            if (NextStepCfg.EnableMask == 1U) {
                CCfg.pBin[AMBANET_OD37_MKL] = &FDBinBuf[i];
                (void)_LoadFlexiDagBin(pArgVector[i + 2U], CCfg.pBin[AMBANET_OD37_MKL]);
                AmbaPrint_PrintStr5("FD: %s", pArgVector[i + 2U], NULL, NULL, NULL, NULL);
                i++;
                CCfg.pBin[AMBANET_OD37_MKS] = &FDBinBuf[i];
                (void)_LoadFlexiDagBin(pArgVector[i + 2U], CCfg.pBin[AMBANET_OD37_MKS]);
                AmbaPrint_PrintStr5("FD: %s", pArgVector[i + 2U], NULL, NULL, NULL, NULL);
                i++;
            }

            if (NextStepCfg.EnableTS == 1U) {
                CCfg.pBin[AMBANET_OD37_TS] = &FDBinBuf[i];
                (void)_LoadFlexiDagBin(pArgVector[i + 2U], CCfg.pBin[AMBANET_OD37_TS]);
                AmbaPrint_PrintStr5("FD: %s", pArgVector[i + 2U], NULL, NULL, NULL, NULL);
                i++;
                CCfg.pBin[AMBANET_OD37_SL] = &FDBinBuf[i];
                (void)_LoadFlexiDagBin(pArgVector[i + 2U], CCfg.pBin[AMBANET_OD37_SL]);
                AmbaPrint_PrintStr5("FD: %s", pArgVector[i + 2U], NULL, NULL, NULL, NULL);
                i++;
            }

            if (NextStepCfg.EnableTLC == 1U) {
                CCfg.pBin[AMBANET_OD37_TLC] = &FDBinBuf[i];
                (void)_LoadFlexiDagBin(pArgVector[i + 2U], CCfg.pBin[AMBANET_OD37_TLC]);
                AmbaPrint_PrintStr5("FD: %s", pArgVector[i + 2U], NULL, NULL, NULL, NULL);
                i++;
            }

            if (NextStepCfg.EnableTLS == 1U) {
                CCfg.pBin[AMBANET_OD37_TLS] = &FDBinBuf[i];
                (void)_LoadFlexiDagBin(pArgVector[i + 2U], CCfg.pBin[AMBANET_OD37_TLS]);
                AmbaPrint_PrintStr5("FD: %s", pArgVector[i + 2U], NULL, NULL, NULL, NULL);
                i++;
            }

            if (NextStepCfg.EnableAR == 1U) {
                CCfg.pBin[AMBANET_OD37_AR] = &FDBinBuf[i];
                (void)_LoadFlexiDagBin(pArgVector[i + 2U], CCfg.pBin[AMBANET_OD37_AR]);
                AmbaPrint_PrintStr5("FD: %s", pArgVector[i + 2U], NULL, NULL, NULL, NULL);
            }

            if (NextStepCfg.EnableVB == 1U) {
                CCfg.pBin[AMBANET_OD37_VB] = &FDBinBuf[i];
                (void)_LoadFlexiDagBin(pArgVector[i + 2U], CCfg.pBin[AMBANET_OD37_VB]);
                AmbaPrint_PrintStr5("FD: %s", pArgVector[i + 2U], NULL, NULL, NULL, NULL);
            }

            if (NextStepCfg.EnableTR == 1U) {
                CCfg.pBin[AMBANET_OD37_TR] = &FDBinBuf[i];
                (void)_LoadFlexiDagBin(pArgVector[i + 2U], CCfg.pBin[AMBANET_OD37_TR]);
                AmbaPrint_PrintStr5("FD: %s", pArgVector[i + 2U], NULL, NULL, NULL, NULL);
            }

            Ret = SvcCvAlgo_Create(Hdlr, &CCfg);
            OutputNum = CCfg.OutputNum;
            AmbaPrint_PrintUInt5("OutputNum: %u", OutputNum, 0U, 0U, 0U, 0U);
            for (i = 0U ; i < OutputNum ; i++) {
                OutputSz[i] = CCfg.OutputSz[i];
                AmbaPrint_PrintUInt5("OutputSz[%u] = %u", i, OutputSz[i], 0U, 0U, 0U);
            }

        }

        if (Ret == 0U) {
            SVC_CV_ALGO_REGCB_CFG_s CBCfg;
            CBCfg.Mode = 0U;
            CBCfg.Callback = SCA_AmbaPcpt_UT_CB;
            (void)SvcCvAlgo_RegCallback(Hdlr, &CBCfg);

            _Config_AmbaPcpt(Hdlr, pArgVector[Count + 4U]);
        }
        Init = 1U;
    }
}

static void SCA_AmbaPcpt_UT_Run(const char* InPath, const char* LogPath, UINT32 Opt)
{
    static AMBA_CV_FLEXIDAG_IO_s InBuf;
    static AMBA_CV_FLEXIDAG_IO_s OutBuf;
    static UINT8* OutputBuf[8];
    static UINT32 BufferAlloc = 0U;
    static UINT8* FileInputBuf = NULL;
    UINT32 i;
    UINT32 YuvWidth = FRAME_W;
    UINT32 YuvHeight = FRAME_H;
    static memio_source_recv_picinfo_t *MemIOPicInfo = NULL;
    static UINT32 MemIOPicInfoSz = 0U;
    cv_pic_info_t *PicInfo;
    const UINT8* pChar;
    UINT32 U32FileInputBuf;
    UINT32 U32MemIOPicInfo;
    void* vp;
    UINT8* pU8;
    const UINT8* pU8MemIOPicInfo;
    UINT32 AlignedSize;
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;

    if (BufferAlloc == 0U) {
        //alloc input buffer
        (void)RefCV_UT_GetCVBuf(&pU8, sizeof(memio_source_recv_picinfo_t), &AlignedSize);
        MemIOPicInfoSz = AlignedSize;
        AmbaMisra_TypeCast(&MemIOPicInfo, &pU8);

        // alloc frame buffer
        (void)RefCV_UT_GetCVBuf(&pU8, MAX_FILE_SIZE, &AlignedSize);
        FileInputBuf = pU8;

        for (i = 0U; i < OutputNum; i++) {
            (void)RefCV_UT_GetCVBuf(&pU8, OutputSz[i], &AlignedSize);
            OutputBuf[i] = pU8;
        }

        // alloc output buffer
        BufferAlloc = 1U;
    }

    PicInfo = &(MemIOPicInfo->pic_info);

    pChar = FileInputBuf;
    AmbaMisra_TypeCast(&vp, &pChar);
    AmbaMisra_TypeCast(&U32FileInputBuf, &pChar);
    AmbaMisra_TypeCast(&U32MemIOPicInfo, &MemIOPicInfo);
    AmbaMisra_TypeCast(&pU8MemIOPicInfo, &MemIOPicInfo);

    _ReadInputFile(InPath, vp);

    PicInfo->capture_time = 0U;
    PicInfo->channel_id = 0U;
    PicInfo->frame_num = 0U;
    PicInfo->pyramid.image_width_m1 = (UINT16)(YuvWidth - 1U);
    PicInfo->pyramid.image_height_m1 = (UINT16)(YuvHeight - 1U);
    PicInfo->pyramid.image_pitch_m1 = (YuvWidth - 1U);

    for (i = 0U; i < 6U; i++) {
        PicInfo->pyramid.half_octave[i].ctrl.roi_pitch = (UINT16)YuvWidth;
        PicInfo->pyramid.half_octave[i].roi_start_col = (INT16)0;
        PicInfo->pyramid.half_octave[i].roi_start_row = (INT16)0;
        PicInfo->pyramid.half_octave[i].roi_width_m1 = (UINT16)(YuvWidth - 1U);
        PicInfo->pyramid.half_octave[i].roi_height_m1 = (UINT16)(YuvHeight - 1U);
        PicInfo->pyramid.half_octave[i].ctrl.disable = 0U;

        PicInfo->rpLumaLeft[i] = U32FileInputBuf - U32MemIOPicInfo;
        PicInfo->rpChromaLeft[i] = (U32FileInputBuf + (YuvWidth * YuvHeight)) - U32MemIOPicInfo;
        PicInfo->rpLumaRight[i] = PicInfo->rpLumaLeft[i];
        PicInfo->rpChromaRight[i] = PicInfo->rpChromaLeft[i];
    }

#if 0
    /* Small pyramid for classifier */
    U32FileInputBuf += 1280 * 640 * 1.5;
    AmbaMisra_TypeCast(&vp, &U32FileInputBuf);
    _ReadInputFile("OUT_Pri_6.withbbx.avi_snapshot_00.07.075_448x224.yuv", vp);

    PicInfo->pyramid.half_octave[3].ctrl.roi_pitch = (UINT16) 448;
    PicInfo->pyramid.half_octave[3].roi_start_col = (INT16)0;
    PicInfo->pyramid.half_octave[3].roi_start_row = (INT16)0;
    PicInfo->pyramid.half_octave[3].roi_width_m1 = (UINT16)(448 - 1U);
    PicInfo->pyramid.half_octave[3].roi_height_m1 = (UINT16)(224 - 1U);
    PicInfo->pyramid.half_octave[3].ctrl.disable = 0U;

    PicInfo->rpLumaLeft[3] = U32FileInputBuf - U32MemIOPicInfo;
    PicInfo->rpChromaLeft[3] = (U32FileInputBuf + (448 * 224)) - U32MemIOPicInfo;
    PicInfo->rpLumaRight[3] = PicInfo->rpLumaLeft[3];
    PicInfo->rpChromaRight[3] = PicInfo->rpChromaLeft[3];
#endif

    NextStepCfg.RoiIdx      = 0U;
    NextStepCfg.OSDWidth    = FRAME_W;
    NextStepCfg.OSDHeight   = FRAME_H;

    if (NextStepCfg.EnableKP == 1U) {
        NextStepCfg.EnableKP |= 0x2U;
    }
    if (NextStepCfg.Enable3D == 1U) {
        NextStepCfg.Enable3D |= 0x2U;
    }
    if (NextStepCfg.EnableMask == 1U) {
        NextStepCfg.EnableMask |= 0x2U;
    }
    if (NextStepCfg.EnableTS == 1U) {
        NextStepCfg.EnableTS |= 0x2U;
        NextStepCfg.EnableSL |= 0x3U;
    }
    if (NextStepCfg.EnableTLC == 1U) {
        NextStepCfg.EnableTLC |= 0x2U;
    }
    if (NextStepCfg.EnableTLS == 1U) {
        NextStepCfg.EnableTLS |= 0x2U;
    }
    if (NextStepCfg.EnableAR == 1U) {
        NextStepCfg.EnableAR |= 0x2U;
    }
    if (NextStepCfg.EnableVB == 1U) {
        NextStepCfg.EnableVB |= 0x2U;
    }
    if (NextStepCfg.EnableTR == 1U) {
        NextStepCfg.EnableTR |= 0x2U;
    }

    FeedCfg.pIn = &InBuf;
    InBuf.num_of_buf = 1;
    _FiLLFlexiDagMemStruct(pU8MemIOPicInfo, MemIOPicInfoSz, &InBuf.buf[0]);

    FeedCfg.pOut = &OutBuf;
    OutBuf.num_of_buf = 1;
    _FiLLFlexiDagMemStruct(OutputBuf[0], OutputSz[0], &OutBuf.buf[0]);

    FeedCfg.pExtFeedCfg = &NextStepCfg;

    (void)SvcCvAlgo_Feed(Hdlr, &FeedCfg);

    (void)LogPath;
    (void)Opt;
}

void SCA_AmbaPcpt_UT(char * const * pArgVector)
{
    UINT32 Loop, Enable, Count = 1, i, Rval;
    const char* FN;
    const void *Msg;
    static char PcptUTQueueName[] = "PcptUTQueue";
    static UINT32 Runnable[3] = { 0 };

    Rval = AmbaKAL_MsgQueueCreate(&PcptUTQueue,
                                  PcptUTQueueName,
                                  sizeof(UINT32),
                                  Runnable,
                                  (sizeof(UINT32) * 3U));
    if (Rval != KAL_ERR_NONE) {
        AmbaPrint_PrintUInt5("MsgQueueCreate failed", 0U, 0U, 0U, 0U, 0U);
    }

    (void) AmbaUtility_StringToUInt32(pArgVector[1], &Enable);
    if ((Enable & 0x1U) == 0x1U) {
        NextStepCfg.EnableKP = 1U;
        Count += 1U;
    }
    if ((Enable & 0x2U) == 0x2U) {
        NextStepCfg.Enable3D = 1U;
        Count += 1U;
    }
    if ((Enable & 0x4U) == 0x4U) {
        NextStepCfg.EnableMask = 1U;
        Count += 2U;
    }
    if ((Enable & 0x8U) == 0x8U) {
        NextStepCfg.EnableTS = 1U;
        NextStepCfg.EnableSL = 1U;
        Count += 2U;
    }
    if ((Enable & 0x10U) == 0x10U) {
        NextStepCfg.EnableTLC = 1U;
        Count += 1U;
    }
    if ((Enable & 0x20U) == 0x20U) {
        NextStepCfg.EnableTLS = 1U;
        Count += 1U;
    }
    if ((Enable & 0x40U) == 0x40U) {
        NextStepCfg.EnableAR = 1U;
        Count += 1U;
    }
    if ((Enable & 0x80U) == 0x80U) {
        NextStepCfg.EnableVB = 1U;
        Count += 1U;
    }
    if ((Enable & 0x100U) == 0x100U) {
        NextStepCfg.EnableTR = 1U;
        Count += 1U;
    }

    (void) AmbaUtility_StringToUInt32(pArgVector[Count + 5U], &Loop);

    SCA_AmbaPcpt_UT_Init(pArgVector, Count);

    if (KAL_ERR_NONE != AmbaKAL_MsgQueueSend(&PcptUTQueue, &Rval, 5000)) {
        AmbaPrint_PrintUInt5("UT MsgQueueSend timeout", 0U, 0U, 0U, 0U, 0U);
    }

    if (Loop == 1U) {
        FN = _GetNextFile(pArgVector[Count + 3U]);
        while (FN != NULL) {
            if (KAL_ERR_NONE != AmbaKAL_MsgQueueReceive(&PcptUTQueue, &Msg, 10000)) {
                AmbaPrint_PrintUInt5("UT MsgQueueReceive timeout", 0U, 0U, 0U, 0U, 0U);
            } else {
                SCA_AmbaPcpt_UT_Run(FN, pArgVector[Count + 2U], Loop);
                FN = _GetNextFile(pArgVector[Count + 3U]);
            }
        }
    } else {
        for (i = 0; i < Loop; i++) {
            if (KAL_ERR_NONE != AmbaKAL_MsgQueueReceive(&PcptUTQueue, &Msg, 10000)) {
                AmbaPrint_PrintUInt5("UT MsgQueueReceive timeout", 0U, 0U, 0U, 0U, 0U);
            } else {
                SCA_AmbaPcpt_UT_Run(pArgVector[Count + 3U], pArgVector[Count + 2U], Loop);
            }
        }
    }
}

