/*
*  @file AmbaTUNE_USBCtrl_cv2fs.c
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
*/



#include "AmbaTUNE_USBCtrl.h"
#include "AmbaTUNE_HdlrManager_cv2fs.h"
#include "AmbaTUNE_USBHdlr_cv2fs.h"
#include "AmbaImg_Proc.h"
#include "AmbaImg_External_CtrlFunc.h"
#include "AmbaMisraFix.h"

#define usbdatactrl_print_uint32_5 AmbaPrint_PrintUInt5
#define usbdatactrl_print_str_5    AmbaPrint_PrintStr5
#define usbdatactrl_memset         AmbaWrap_memset
#define usbdatactrl_memcpy         AmbaWrap_memcpy
// #define usbdatactrl_fclose AmbaFS_FileClose
// #define usbdatactrl_print_int32_5 AmbaPrint_PrintInt5
#define USBDataCtrl_OK                     0
#define USBDataCtrl_ERR_VERSION           20
// #define USBDataCtrl_ERR_ItunerInit        21
// #define USBDataCtrl_ERR_TableInit         22

#define DIR_PC_TO_ARM                     0U
#define DIR_ARM_TO_PC                     1U


#define MAX_ITUNER_DATA_SIZE         300000U
#define USB_ITUNER_BUFFER           3331328U
#define MAXDataBufferSize           3000000U // 6000*4000/8 ITUNER_MAX_FPN_MAP_SIZE
#define WarpGridSize                 196608U // 4 * 256 * 192
#define VigTableSize                   8192U // 4 * 128 * 4 * 4
#define CaRedTableSize                24576U // 4 * 64 * 96
#define CaBlueTableSize               24576U // 4 * 64 * 96

#define USBDataCtrl_IDSP_PRIORITY        69U
#define USBDataCtrl_IDSP_POOLSIZE     61440U //600 * 1024

#define AmageAcceptVersion              150U
#define USBDataCtrl_LOADIDSP             16U
#define USBDataCtrl_SAVEIDSP             17U
#define USBDataCtrl_EXECUTEIDSP          18U

#define USBDataCtrl_IDSP_TASKNAME "USBDataCtrl_IDSP_Tsk"
#define USBDataCtrl_IDSP_QUENAME  "USBDataCtrl_IDSP_Que"

#ifndef ENABLE
#define ENABLE  1U
#endif

#ifndef DISABLE
#define DISABLE 0U
#endif

typedef enum {
    AmageModeConfig = 'A',
    IKModeConfig    = 'B',
    GDataParameters = 'C',
    Ituner          = 'D',
    AAAControl      = 'E',
    AEControl       = 'F',
    CC              = 'G',
    // DZOOM           = 'H',
    WARP            = 'I',
    CA              = 'J',
    FPN             = 'K',
    VIG             = 'L',
    //RAW_Load        = 'M',
    // JPG_Save        = 'N',
    // YUV_Save        = 'O',
    EndProc         = 'Z',
    Default         = 'z'
} USBHdlr_ITEM_e;


typedef enum {
    VIDEO       = 0,
    VIDEOHDR    = 1,
    // STILL       = 2,
    HISO        = 3
} USBHdlr_MODE_e;


// typedef struct{
//     UINT8 SendFlag;
//     UINT8 GetFlag;
// }ItemFlag_s;

// typedef struct{
//     ItemFlag_s  TuningModeflg;
//     ItemFlag_s  Itunerflag;
//     ItemFlag_s  CCflag;
//     ItemFlag_s  Warpflag;
//     ItemFlag_s  FPNflag;
//     ItemFlag_s  CAflag;
//     ItemFlag_s  Vignetteflag;
//     ItemFlag_s  RAWflag;
//     ItemFlag_s  Amplinearflag;
// }ItemCheckFlag_s;

typedef struct {
    USBHdlr_MODE_e TuningMode;
    UINT8  USBHdlrInitFlg;
    UINT8  IKInitFlg;
    UINT8  CalibTableInitFlag;
    UINT8  TskInitFlg;
    USBHdlr_ITEM_e  Item;
    UINT32 AmageVersion;
    UINT32 AmageSendDataSize;
    //ItemCheckFlag_s ItemFlg;
} USBDataCtrl_FlowCfg_s;

typedef struct {
    INT32 AgcGain;
    INT32 ShutterTime;
    //INT32 IrisIndex;
    INT32 Dgain;
} USBDataCtrl_AECtrl_s;


typedef struct {
    UINT8 TunningMode;
    UINT32 AmageVersion;
} USBDataCtrl_ModeCfg_s;

typedef struct {
    AMBA_IK_MODE_CFG_s Mode;
} USBDataCtrl_TEST_IS2_s;

typedef struct {
    UINT32 CfgUpdate;
} USBDataCtrl_Schdlr_Cfg_s;

typedef struct {
    UINT32 Type;
} USBDataCtrl_EVENT_MSG_s;


typedef struct {
    AMBA_KAL_MSG_QUEUE_t    MessageQue;
    char                    QueName[32];
    INT32                   Running;                    /**< Runing state of thread */
    AMBA_KAL_TASK_t         Task;
    char                    TaskName[32];
} USBDataCtrl_Tsk_Ctrl;


typedef struct {
    UINT8*     addr;
    UINT32     size;
} USBDataCtrl_DataCfg_s;

typedef struct {
    UINT8* addr;
    UINT8  isSet;
} ITUNER_TABLE_ADDR_s;

typedef struct {
    UINT32* addr;
    UINT8  isSet;
} ITUNER_TABLEL_ADDR_s;

typedef struct {
    ITUNER_TABLE_ADDR_s FPNMap;
    ITUNER_TABLE_ADDR_s WarpGrid;
    ITUNER_TABLE_ADDR_s CaRedGrid;
    ITUNER_TABLE_ADDR_s CaBlueGrid;
    ITUNER_TABLEL_ADDR_s VignetteTable;
    ITUNER_TABLEL_ADDR_s CcThreeD;
} CalibTableAddr_s;



static USBDataCtrl_Tsk_Ctrl     IDSPTskCtrl      = {0};



static USBDataCtrl_TEST_IS2_s   gTestIS2         = {0};


static USBDataCtrl_DataCfg_s    ItunerStream     = {.addr = NULL, .size = 0};
static USBDataCtrl_FlowCfg_s    USBDataCtrlCfg;


static CalibTableAddr_s         CalibTable;



static INT32 USBDataCtrl_EndProc(const UINT8* pDataBufferAddr)
{
//    static USBDataCtrl_Schdlr_Cfg_s SchdlrUpdateCtrl = {0};
    INT32 Ret = USBDataCtrl_OK;
    USBDataCtrl_EVENT_MSG_s Msg;

    const USBDataCtrl_Schdlr_Cfg_s *pSchdlrCfgs = NULL;

    AmbaMisra_TypeCast(&pSchdlrCfgs, &pDataBufferAddr);

    // (void)usbdatactrl_memcpy(&pSchdlrCfgs,&pDataBufferAddr,sizeof(USBDataCtrl_Schdlr_Cfg_s));
//    SchdlrUpdateCtrl.CfgUpdate = pSchdlrCfgs->CfgUpdate;


    // SchdlrUpdateCtrl.CfgUpdate = ((USBDataCtrl_Schdlr_Cfg_s *)(pDataBufferAddr))->CfgUpdate;

    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_EndProc",NULL,NULL,NULL,NULL);

    Msg.Type = USBDataCtrl_EXECUTEIDSP;

    if(OK != AmbaKAL_MsgQueueSend(&IDSPTskCtrl.MessageQue, &Msg, AMBA_KAL_NO_WAIT)) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call %s: AmbaKAL_MsgQueueSend() Fail ","USBDataCtrl_EndProc",NULL,NULL,NULL,NULL);
        AmbaPrint_Flush();
    }


    return Ret;
}

static INT32 USBDataCtrl_CaLoad(const UINT8* pDataBufferAddr)
{
    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_CaLoad",NULL,NULL,NULL,NULL);

    if(CalibTable.CaRedGrid.addr == NULL) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() CaRedGrid addr error","USBDataCtrl_CaLoad",NULL,NULL,NULL,NULL);
    } else {
        if(0U != usbdatactrl_memcpy((UINT8 *)CalibTable.CaRedGrid.addr,pDataBufferAddr,CaRedTableSize)) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() CaRedGrid usbdatactrl_memcpy error",__func__,NULL,NULL,NULL,NULL);
        }
        CalibTable.CaRedGrid.isSet = 1;
    }
    if(CalibTable.CaBlueGrid.addr ==NULL) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() VignetteTable addr error","USBDataCtrl_CaLoad",NULL,NULL,NULL,NULL);
    } else {
        if(0U != usbdatactrl_memcpy((UINT8 *)CalibTable.CaBlueGrid.addr,&pDataBufferAddr[CaRedTableSize],CaBlueTableSize)) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() CaBlueGrid usbdatactrl_memcpy error",__func__,NULL,NULL,NULL,NULL);
        }
        CalibTable.CaBlueGrid.isSet = 1;
    }

    return USBDataCtrl_OK;

}



static INT32 USBDataCtrl_VigLoad(const UINT8* pDataBufferAddr)
{
    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_VigLoad",NULL,NULL,NULL,NULL);

    if((CalibTable.VignetteTable.addr == NULL)) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() VignetteTable addr error","USBDataCtrl_VigLoad",NULL,NULL,NULL,NULL);
    } else {
        if(0U != usbdatactrl_memcpy((UINT8 *)CalibTable.VignetteTable.addr,pDataBufferAddr,VigTableSize)) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() usbdatactrl_memcpy error",__func__,NULL,NULL,NULL,NULL);
        }
        CalibTable.VignetteTable.isSet = 1;
    }

    return USBDataCtrl_OK;

}

static INT32 USBDataCtrl_WarpLoad(const UINT8* pDataBufferAddr)
{
    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_WarpLoad",NULL,NULL,NULL,NULL);

    if((CalibTable.WarpGrid.addr == NULL)) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() WarpGrid addr error","USBDataCtrl_WarpLoad",NULL,NULL,NULL,NULL);
    } else {
        if (0U != usbdatactrl_memcpy((UINT8 *)CalibTable.WarpGrid.addr,pDataBufferAddr,WarpGridSize)) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() usbdatactrl_memcpy error",__func__,NULL,NULL,NULL,NULL);
        }
        CalibTable.WarpGrid.isSet = 1;
    }

    return USBDataCtrl_OK;
}


static INT32 USBDataCtrl_CCLoad(const UINT8* pDataBufferAddr)
{
    ITUNER_COLOR_CORRECTION_s  Temp;
    const char* str = "From USB";

    AmbaItuner_Get_ColorCorrection((ITUNER_COLOR_CORRECTION_s *) &Temp);
    ituner_strncpy(Temp.ThreeDPath,  str, 8);
    AmbaItuner_Set_ColorCorrection((ITUNER_COLOR_CORRECTION_s *) &Temp);

    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_CCLoad",NULL,NULL,NULL,NULL);
    //usbdatactrl_print_uint32_5("[USBDataCtrl][DEBUG] USBDataCtrl_CCLoad TuningMode = %d, AmageVersion = %d", USBDataCtrlCfg.TuningMode, USBDataCtrlCfg.AmageVersion,0,0,0);

//    (void)usbdatactrl_memcpy((UINT8 *)CalibTable.CcThreeD.addr,&pDataBufferAddr[18752U],IK_CC_3D_SIZE);
    if(0U != usbdatactrl_memcpy((UINT8 *)CalibTable.CcThreeD.addr,pDataBufferAddr,IK_CC_3D_SIZE)) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() usbdatactrl_memcpy error",__func__,NULL,NULL,NULL,NULL);
    }

    if((CalibTable.CcThreeD.addr == NULL)) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() CcThreeD addr error","USBDataCtrl_CCLoad",NULL,NULL,NULL,NULL);
    } else {
        CalibTable.CcThreeD.isSet = 1;
    }

    return USBDataCtrl_OK;
}

static INT32 USBDataCtrl_CaSave(UINT8* pDataBufferAddr)
{
    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_CaSave",NULL,NULL,NULL,NULL);

    if(0 != AmbaItuner_Refresh(&gTestIS2.Mode)) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call AmbaItuner_Refresh() Fail ",NULL,NULL,NULL,NULL,NULL);
    }

    if(CalibTable.CaRedGrid.addr != NULL) {
        if(0U != usbdatactrl_memcpy(pDataBufferAddr, (UINT8*)CalibTable.CaRedGrid.addr, CaRedTableSize)) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() CaRedGrid usbdatactrl_memcpy error",__func__,NULL,NULL,NULL,NULL);
        }
    } else {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] VignetteTable addr is null ",NULL,NULL,NULL,NULL,NULL);
    }

    if(CalibTable.CaBlueGrid.addr != NULL) {
        if(0U != usbdatactrl_memcpy(&pDataBufferAddr[CaRedTableSize], (UINT8*)CalibTable.CaBlueGrid.addr, CaBlueTableSize)) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() CaBlueGrild usbdatactrl_memcpy error",__func__,NULL,NULL,NULL,NULL);
        }
    } else {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] VignetteTable addr is null ",NULL,NULL,NULL,NULL,NULL);
    }


    return USBDataCtrl_OK;
}


static INT32 USBDataCtrl_VigSave(UINT8* pDataBufferAddr)
{


    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_VigSave",NULL,NULL,NULL,NULL);

    if(0 != AmbaItuner_Refresh(&gTestIS2.Mode)) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call AmbaItuner_Refresh() Fail ",NULL,NULL,NULL,NULL,NULL);
    }

    if(CalibTable.VignetteTable.addr != NULL) {
        if(0U != usbdatactrl_memcpy(pDataBufferAddr, (UINT8*)CalibTable.VignetteTable.addr, VigTableSize)) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() usbdatactrl_memcpy error",__func__,NULL,NULL,NULL,NULL);
        }
    } else {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] VignetteTable addr is null ",NULL,NULL,NULL,NULL,NULL);
    }
    return USBDataCtrl_OK;
}

static INT32 USBDataCtrl_WarpSave(UINT8* pDataBufferAddr)
{
    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_WarpSave",NULL,NULL,NULL,NULL);

    if(0 != AmbaItuner_Refresh(&gTestIS2.Mode)) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call AmbaItuner_Refresh() Fail ",NULL,NULL,NULL,NULL,NULL);
    }
    if(CalibTable.WarpGrid.addr != NULL) {
        if(0U != usbdatactrl_memcpy(pDataBufferAddr, (UINT8*)CalibTable.WarpGrid.addr, WarpGridSize)) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() usbdatactrl_memcpy error",__func__,NULL,NULL,NULL,NULL);
        }
    } else {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] WarpGrid addr is null ",NULL,NULL,NULL,NULL,NULL);
    }


    return USBDataCtrl_OK;

}


static INT32 USBDataCtrl_CCSave(UINT8* pDataBufferAddr)
{


    // AMBA_FS_FILE *Fid;
    // char *FileNameReg = "c:\\AmageSave_Reg.bin";
    // char *FileName3D = "c:\\AmageSave_3D.bin";

    // ITUNER_COLOR_CORRECTION_s ColorCorrection;

    static AMBA_IK_COLOR_CORRECTION_s *pCc_Threed GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    // (void)pDataBufferAddr;

    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_CCSave",NULL,NULL,NULL,NULL);

    // usbdatactrl_memcpy(pDataBufferAddr,(UINT8 *)CalibTable.CcReg.addr,IK_CC_REG_SIZE);
    // usbdatactrl_memcpy((pDataBufferAddr + (IK_CC_REG_SIZE)),(UINT8 *)CalibTable.CcThreeD.addr,IK_CC_3D_SIZE);


    if(0 != AmbaItuner_Refresh(&gTestIS2.Mode)) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call AmbaItuner_Refresh() Fail ",NULL,NULL,NULL,NULL,NULL);
    }

    AmbaItuner_Get_CcThreeD(&pCc_Threed);

    if(0U != usbdatactrl_memcpy(pDataBufferAddr, (UINT8*)(pCc_Threed->MatrixThreeDTable), IK_CC_3D_SIZE)) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() CC_3D usbdatactrl_memcpy error",__func__,NULL,NULL,NULL,NULL);
    }
    // (void)usbdatactrl_memcpy(&pDataBufferAddr[18752U], (UINT8*)(Cc_Threed.MatrixThreeDTable), IK_CC_3D_SIZE);



    return USBDataCtrl_OK;
}

static INT32 USBDataCtrl_AELoad(const UINT8* pDataBufferAddr)
{
    AMBA_AE_INFO_s  AeInfo;
    UINT32          chNo = 0;

    const USBDataCtrl_AECtrl_s *pAECtrl = NULL;
    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_AELoad",NULL,NULL,NULL,NULL);

    AmbaMisra_TypeCast(&pAECtrl, &pDataBufferAddr);

    // (void)usbdatactrl_memcpy(&pAECtrl, &pDataBufferAddr,sizeof(USBDataCtrl_AECtrl_s));

    if((USBDataCtrlCfg.TuningMode == VIDEO )||(USBDataCtrlCfg.TuningMode == VIDEOHDR)) {

        FLOAT FMisra;
        FMisra = (FLOAT)pAECtrl ->AgcGain;
        AeInfo.AgcGain      =  FMisra/ 1000.0f;

        FMisra = (FLOAT)pAECtrl->ShutterTime;
        AeInfo.ShutterTime  = FMisra/ 1000000.0f;

        AeInfo.Dgain        = pAECtrl->Dgain;

        (void)AmbaImgProc_AESetExpInfo(chNo, 0U, IP_MODE_VIDEO, &AeInfo);
    }

    (void)AmbaImgProc_SetMEMWBCommand(0U, ENABLE);

    return USBDataCtrl_OK;
}

static INT32 USBDataCtrl_AESave(const UINT8* pDataBufferAddr)
{

    AMBA_AE_INFO_s  AeInfo;
    UINT32          chNo = 0;
    USBDataCtrl_AECtrl_s AECtrl;
    AMBA_AE_INFO_s*  pAeInfo = NULL;

    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_AESave",NULL,NULL,NULL,NULL);

    (void)AmbaImgProc_AEGetExpInfo(chNo, 0U, IP_MODE_VIDEO, &AeInfo);
    if((USBDataCtrlCfg.TuningMode == VIDEO )||(USBDataCtrlCfg.TuningMode == VIDEOHDR)) {

        INT32 IMisra;
        FLOAT FMisra;
        FMisra = (AeInfo.AgcGain * 1000.0f);
        IMisra = (INT32)FMisra;
        AECtrl.AgcGain = IMisra;

        FMisra = (AeInfo.ShutterTime * 1000000.0f);
        IMisra = (INT32)FMisra;
        AECtrl.ShutterTime  = IMisra;

        AECtrl.Dgain = AeInfo.Dgain;


    }

    AmbaMisra_TypeCast(&pAeInfo, &pDataBufferAddr);
    if(0U != usbdatactrl_memcpy(pAeInfo, &AECtrl, sizeof(USBDataCtrl_AECtrl_s))) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() usbdatactrl_memcpy error",__func__,NULL,NULL,NULL,NULL);
    }

    return USBDataCtrl_OK;
}

static INT32 USBDataCtrl_AAALoad(const UINT8* pDataBufferAddr)
{
    AMBA_AAA_OP_INFO_s  AaaOpInfoTmp = {0};
    UINT32              ViewCount;

    const AMBA_AAA_OP_INFO_s *pAaaOpInfo = NULL;


    AmbaMisra_TypeCast(&pAaaOpInfo, &pDataBufferAddr);

    AaaOpInfoTmp.AeOp  = pAaaOpInfo->AeOp;
    AaaOpInfoTmp.AwbOp = pAaaOpInfo->AwbOp;
    AaaOpInfoTmp.AfOp  = pAaaOpInfo->AfOp;
    AaaOpInfoTmp.AdjOp = pAaaOpInfo->AdjOp;

    (void)AmbaImgProc_GetTotalViewCount(&ViewCount);

    for(UINT32 i = 0; i < ViewCount; i++) {
        (void)AmbaImgProc_SetAAAOPInfo(i, &AaaOpInfoTmp);
        AmbaPrint_PrintUInt5("ChNo[%u]: --- AeOp : %d, AwbOp : %d, AfOp : %d, AdjOp : %d ---", \
                             i, \
                             AaaOpInfoTmp.AeOp, \
                             AaaOpInfoTmp.AwbOp, \
                             AaaOpInfoTmp.AfOp, \
                             AaaOpInfoTmp.AdjOp);
    }
// #ifdef SSPUT_SCHDLR_LOCK
//     USBDataCtrl_Schdlr_Lock();
// #endif
    return USBDataCtrl_OK;
}

static INT32 USBDataCtrl_AAASave(const UINT8* pDataBufferAddr)
{
    AMBA_AAA_OP_INFO_s  AaaOpInfoTmp;
    AMBA_AAA_OP_INFO_s *pAaaOpInfo = NULL;

    UINT32              ViewCount;

    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_AAASave",NULL,NULL,NULL,NULL);

    (void)AmbaImgProc_GetTotalViewCount(&ViewCount);
    for(UINT32 i = 0; i < ViewCount; i++) {
        (void)AmbaImgProc_GetAAAOPInfo(i, &AaaOpInfoTmp);
        AmbaPrint_PrintUInt5("ChNo[%u]: --- AeOp : %d, AwbOp : %d, AfOp : %d, AdjOp : %d ---", \
                             i, \
                             AaaOpInfoTmp.AeOp, \
                             AaaOpInfoTmp.AwbOp, \
                             AaaOpInfoTmp.AfOp, \
                             AaaOpInfoTmp.AdjOp);
    }
    AmbaMisra_TypeCast(&pAaaOpInfo, &pDataBufferAddr);

    (void)AmbaImgProc_GetAAAOPInfo(0, &AaaOpInfoTmp);
    if(0U != usbdatactrl_memcpy(pAaaOpInfo, &AaaOpInfoTmp, sizeof(AMBA_AAA_OP_INFO_s))) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() usbdatactrl_memcpy error",__func__,NULL,NULL,NULL,NULL);
    }

// #ifdef SSPUT_SCHDLR_LOCK
//     USBDataCtrl_Schdlr_UnLock();
// #endif

    return USBDataCtrl_OK;
}

static void *USBDataCtrl_IDSP_TskEntry(void *Param)
{

    INT32  whileloop = 1;
    USBDataCtrl_EVENT_MSG_s Msg;

    AmbaMisra_TouchUnused(Param);

    while (whileloop > 0) {

        if(OK != AmbaKAL_MsgQueueReceive(&IDSPTskCtrl.MessageQue, &Msg, AMBA_KAL_WAIT_FOREVER)) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call %s: AmbaKAL_MsgQueueReceive() Fail ","USBDataCtrl_IDSP_TskEntry",NULL,NULL,NULL,NULL);
            AmbaPrint_Flush();
            whileloop = 0;
        } else {
            if (IDSPTskCtrl.Running == 0) {
                IDSPTskCtrl.Running = 1;
                if(Msg.Type == USBDataCtrl_LOADIDSP) {
                    // usbdatactrl_print_uint32_5("[USBDataCtrl][DEBUG] USBDataCtrl_LoadIDSP_TskEntry Param %d ", Param, 0, 0,0,0);
                    if(0 != AmbaItuner_Refresh(&gTestIS2.Mode)) {
                        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call %s: AmbaItuner_Refresh() Fail ","USBDataCtrl_IDSP_TskEntry",NULL,NULL,NULL,NULL);
                    }
                    if(0!= USBHdlr_Load_IDSP(ItunerStream.addr,ItunerStream.size)) {
                        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call %s: USBHdlr_Load_IDSP() Fail ","USBDataCtrl_IDSP_TskEntry",NULL,NULL,NULL,NULL);
                    }
                    IDSPTskCtrl.Running = 0;
                } else if (Msg.Type == USBDataCtrl_SAVEIDSP) {
                    if(0!= USBHdlr_Save_IDSP(&gTestIS2.Mode,ItunerStream.addr)) {
                        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call %s: USBHdlr_Save_IDSP() Fail ","USBDataCtrl_IDSP_TskEntry",NULL,NULL,NULL,NULL);
                    }
                    IDSPTskCtrl.Running = 0;
                } else if (Msg.Type == USBDataCtrl_EXECUTEIDSP) {
                    if (0 != USBHdlr_Execute_IDSP(&gTestIS2.Mode)) {
                        usbdatactrl_print_str_5("[USBDataCtrl][Error] Call USBHdlr_Execute_IDSP() Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
                    }
// #ifdef SSPUT_SCHDLR_LOCK

//                     if (0 == USBDataCtrl_Schdlr_Update()) {
//                         usbdatactrl_print_str_5("[USBDataCtrl] Schedlr Update ", DC_S, DC_S, DC_S, DC_S, DC_S);
//                     }
// #endif
                    IDSPTskCtrl.Running = 0;
                } else {
                    usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() AmbaKAL_MsgQueueReceive unknow type","USBDataCtrl_IDSP_TskEntry",NULL,NULL,NULL,NULL);
                }
            }
            // AmbaKAL_EventFlagClear(&IDSPTskCtrl.Flag, 1);
        }
    }
    return NULL;
}

static INT32 USBDataCtrl_CalibTable_Init(void)
{
    ITUNER_Calib_Table_s* Ituner_Calib_Table = NULL;

    // initial CalibTable

    CalibTable.FPNMap.addr          = NULL;
    CalibTable.WarpGrid.addr        = NULL;
    CalibTable.CcThreeD.addr        = NULL;
    CalibTable.VignetteTable.addr   = NULL;
    CalibTable.FPNMap.isSet         = 0;
    CalibTable.WarpGrid.isSet       = 0;
    CalibTable.CcThreeD.isSet       = 0;
    CalibTable.VignetteTable.isSet  = 0;
    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_CalibTable_Init",NULL,NULL,NULL,NULL);


    if(0 != AmbaItuner_Get_Calib_Table(&Ituner_Calib_Table)) {
        usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] call AmbaItuner_Get_Calib_Table failed",NULL,NULL,NULL,NULL,NULL);
    }


    CalibTable.FPNMap.addr   = (UINT8*)Ituner_Calib_Table->FPNMap;
    CalibTable.WarpGrid.addr = (UINT8*)Ituner_Calib_Table->WarpGrid;

    CalibTable.CaBlueGrid.addr  = (UINT8*)Ituner_Calib_Table->CawarpBlueGrid;
    CalibTable.CaRedGrid.addr  = (UINT8*)Ituner_Calib_Table->CawarpRedGrid;

    CalibTable.CcThreeD.addr = (UINT32*)Ituner_Calib_Table->Cc3d;
    CalibTable.VignetteTable.addr = (UINT32*)Ituner_Calib_Table->pVignetteMap;

    return USBDataCtrl_OK;
}

static INT32 USBDataCtrl_Tsk_Init(void)
{
    static USBDataCtrl_EVENT_MSG_s  CfsMQBuf[2]      = {0};
    static UINT8 USBDataCtrlTskBufferk[USBDataCtrl_IDSP_POOLSIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    void* ptr = NULL;
    UINT8* pBuffer = (UINT8*)(&USBDataCtrlTskBufferk[0]);



    if(USBDataCtrlCfg.TskInitFlg != 0U) {
        usbdatactrl_print_str_5("[USBDataCtrl][WARNING] Tsk Already Init ","USBDataCtrl_Tsk_Init",NULL,NULL,NULL,NULL);
    } else {
        if(0U != usbdatactrl_memset(pBuffer, 0, (UINT32)USBDataCtrl_IDSP_POOLSIZE * sizeof(UINT8))) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() usbdatactrl_memset error",__func__,NULL,NULL,NULL,NULL);
        }
        usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_Tsk_Init",NULL,NULL,NULL,NULL);
        AmbaUtility_StringCopy(IDSPTskCtrl.TaskName, 30U, USBDataCtrl_IDSP_TASKNAME);
        AmbaUtility_StringCopy(IDSPTskCtrl.QueName, 30U, USBDataCtrl_IDSP_QUENAME);

        if( OK !=AmbaKAL_MsgQueueCreate(&IDSPTskCtrl.MessageQue, IDSPTskCtrl.QueName, sizeof(USBDataCtrl_EVENT_MSG_s),CfsMQBuf,sizeof(CfsMQBuf))) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call %s: AmbaKAL_MsgQueueCreate() Fail ","USBDataCtrl_Tsk_Init",NULL,NULL,NULL,NULL);
        }



        AmbaMisra_TypeCast(&ptr, &pBuffer);
        // (void)AmbaWrap_memcpy(&ptr, &pBuffer, sizeof(UINT32));
        if (0U != AmbaKAL_TaskCreate(&IDSPTskCtrl.Task,                        /* pTask */
                                     IDSPTskCtrl.TaskName,            /* pTaskName */
                                     USBDataCtrl_IDSP_PRIORITY,            /* Priority */
                                     USBDataCtrl_IDSP_TskEntry,            /* void (*EntryFunction)(UINT32) */
                                     NULL,                                        /* EntryArg */
                                     ptr,   /* pStackBase */
                                     //    (void*) (&USBDataCtrlTskBufferk[0]),
                                     sizeof(USBDataCtrlTskBufferk),            /* StackByteSize */
                                     0U)) {                                    /* AutoStart */
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call %s: AmbaKAL_TaskCreate() Fail ","USBDataCtrl_Tsk_Init",NULL,NULL,NULL,NULL);
            AmbaPrint_Flush();
        }


        (void)AmbaKAL_TaskSetSmpAffinity(&IDSPTskCtrl.Task, 0x1U);
        (void)AmbaKAL_TaskResume(&IDSPTskCtrl.Task);
        USBDataCtrlCfg.TskInitFlg = 1;
    }


    return USBDataCtrl_OK;
}

static INT32 USBDataCtrl_AmageModeConfig_Init(const UINT8* pDataBufferAddr)
{
    INT32 ret = 0;
    const USBDataCtrl_ModeCfg_s *pModeCfg = NULL;

    AmbaMisra_TypeCast(&pModeCfg, &pDataBufferAddr);

    // (void)AmbaWrap_memcpy(&pModeCfg, &pDataBufferAddr, sizeof(UINT32));


    switch(pModeCfg->TunningMode) {

    case ((UINT8)VIDEO):
        USBDataCtrlCfg.TuningMode = VIDEO;
        break;
    case ((UINT8)VIDEOHDR):
        USBDataCtrlCfg.TuningMode = VIDEOHDR;
        break;
    case ((UINT8)HISO):
        USBDataCtrlCfg.TuningMode = HISO;
        break;
    default:
        USBDataCtrlCfg.TuningMode = VIDEO;
        break;
    }

    USBDataCtrlCfg.AmageVersion =  pModeCfg->AmageVersion;
    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_AmageModeConfig_Init",NULL,NULL,NULL,NULL);
    usbdatactrl_print_uint32_5("[USBDataCtrl][DEBUG] USBDataCtrl_AmageModeConfig_Init TuningMode = %d, AmageVersion = %d", pModeCfg->TunningMode, USBDataCtrlCfg.AmageVersion,0,0,0);


    if(USBDataCtrlCfg.AmageVersion != AmageAcceptVersion) {
        usbdatactrl_print_uint32_5("[USBDataCtrl][DEBUG] AmageVersion Mismatch Tool(%d) USBDataCtrl(%d)  ", AmageAcceptVersion, USBDataCtrlCfg.AmageVersion, 0,0,0);
        USBDataCtrlCfg.USBHdlrInitFlg = 0;
        ret = USBDataCtrl_ERR_VERSION;
    } else {
        usbdatactrl_print_uint32_5("[USBDataCtrl][DEBUG] AmageVersion Tool v.%d", AmageAcceptVersion, 0, 0, 0, 0);
    }

    USBDataCtrlCfg.USBHdlrInitFlg = 1;

    return ret;
}


static INT32 USBDataCtrl_IKModeConfig_Init(const UINT8* pDataBufferAddr) //ituner init
{
    // (void)pDataBufferAddr;
    const USBDataCtrl_TEST_IS2_s* pTestIs2 = NULL;
    static UINT8 UsbItunerMemory[USB_ITUNER_BUFFER] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE; // sizeof(ITUNER_Calib_Table_s) + 32

    if(0U != usbdatactrl_memset(&gTestIS2,0,sizeof(USBDataCtrl_TEST_IS2_s))) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() usbdatactrl_memset error",__func__,NULL,NULL,NULL,NULL);
    }

    AmbaMisra_TypeCast(&pTestIs2, &pDataBufferAddr);

    // (void)usbdatactrl_memcpy(&pTestIs2,&pDataBufferAddr,sizeof(USBDataCtrl_TEST_IS2_s));

    gTestIS2.Mode = pTestIs2->Mode;




    // (void)usbdatactrl_memcpy(&gTestIS2,((USBDataCtrl_TEST_IS2_s *)pDataBufferAddr),sizeof(USBDataCtrl_TEST_IS2_s));
    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_IKModeConfig_Init",NULL,NULL,NULL,NULL);
    //usbdatactrl_print_uint32_5("[USBDataCtrl][DEBUG] USBDataCtrl_IKModeConfig_Init IK pMode.ContextID = %d, USB %d",gTestIS2.Mode.ContextId ,0,0,0,0);

    if(USBDataCtrlCfg.IKInitFlg == 0U) {
        if(0U != usbdatactrl_memset(UsbItunerMemory, 0, USB_ITUNER_BUFFER)) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() usbdatactrl_memset error",__func__,NULL,NULL,NULL,NULL);
        }
        if (0UL != AmbaItn_Init(&UsbItunerMemory[0], USB_ITUNER_BUFFER)) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call %s: AmbaTUNE_Init() Fail ","USBDataCtrl_IKModeConfig_Init",NULL,NULL,NULL,NULL);

        } else {
            USBDataCtrlCfg.IKInitFlg = 1;
        }

    } else {
        usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] Already init IK ",NULL,NULL,NULL,NULL,NULL);

    }

    if(USBDataCtrlCfg.CalibTableInitFlag == 0U) {
        if(USBDataCtrl_OK!= USBDataCtrl_CalibTable_Init()) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call %s: AmbaTUNE_Init() Fail ","USBDataCtrl_IKModeConfig_Init",NULL,NULL,NULL,NULL);

        } else {
            USBDataCtrlCfg.CalibTableInitFlag = 1;
        }

    } else {
        usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] Already init CalibTable ",NULL,NULL,NULL,NULL,NULL);

    }
    return USBDataCtrl_OK;
}


static INT32 USBDataCtrl_ItunerLoad(UINT8* pDataBufferAddr) //LoadIDSP
{
    // INT32 Ret;
    USBDataCtrl_EVENT_MSG_s Msg;

    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_ItunerLoad",NULL,NULL,NULL,NULL);

    ItunerStream.addr = pDataBufferAddr;
    ItunerStream.size = USBDataCtrlCfg.AmageSendDataSize;



    Msg.Type = USBDataCtrl_LOADIDSP;

    if(OK != AmbaKAL_MsgQueueSend(&IDSPTskCtrl.MessageQue, &Msg, AMBA_KAL_NO_WAIT)) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call %s: AmbaKAL_MsgQueueSend() Fail ","USBDataCtrl_ItunerLoad",NULL,NULL,NULL,NULL);
        AmbaPrint_Flush();
    }

    return USBDataCtrl_OK;
}

static INT32 USBDataCtrl_ItunerSave(UINT8* pDataBufferAddr) //LoadIDSP
{
    // INT32 Ret;
    USBDataCtrl_EVENT_MSG_s Msg;

    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s() ","USBDataCtrl_ItunerSave",NULL,NULL,NULL,NULL);

    ItunerStream.addr = pDataBufferAddr;
    ItunerStream.size = USBDataCtrlCfg.AmageSendDataSize;

    Msg.Type = USBDataCtrl_SAVEIDSP;

    if(OK != AmbaKAL_MsgQueueSend(&IDSPTskCtrl.MessageQue, &Msg, AMBA_KAL_NO_WAIT)) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] Call %s: AmbaKAL_MsgQueueSend() Fail ","USBDataCtrl_ItunerSave",NULL,NULL,NULL,NULL);
        AmbaPrint_Flush();
    }


    return USBDataCtrl_OK;
}



static INT32 USBDataCtrl_CN_ARMToPC(UINT32* length)
{

    //usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] USBDataCtrl_CN_ARMToPC ",NULL,NULL,NULL,NULL,NULL);
    switch(USBDataCtrlCfg.Item) {

    case GDataParameters:
        *length = MAX_ITUNER_DATA_SIZE;
        break;

    case Ituner:
        *length = MAX_ITUNER_DATA_SIZE;
        break;

    case CC:
        *length = IK_CC_3D_SIZE;
        break;

    case WARP:
        *length = WarpGridSize;
        break;

    case VIG:
        *length = VigTableSize;
        break;

    case CA:
        *length = (CaRedTableSize + CaBlueTableSize);
        break;

    case AAAControl:
        *length = sizeof(AMBA_AAA_OP_INFO_s);
        break;


    case AEControl:
        *length = sizeof(USBDataCtrl_AECtrl_s);
        break;

    default:
        usbdatactrl_print_str_5("[USBDataCtrl][WARNING] %s: default", "USBDataCtrl_CN_ARMToPC", NULL, NULL, NULL, NULL);
        *length = MAX_ITUNER_DATA_SIZE;
        break;
    }
    return USBDataCtrl_OK;
}

static INT32 USBDataCtrl_CN_PCToARM(void)
{
    //usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] USBDataCtrl_CN_PCToARM ",NULL,NULL,NULL,NULL,NULL);

    return USBDataCtrl_OK;
}




static UINT8* USBDataCtrl_GetDataBufferAddr(void)
{

    static UINT8 USBDataBuffer[MAXDataBufferSize] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    UINT8* RetAddr;


    RetAddr = &USBDataBuffer[0];
    return RetAddr;
}



static INT32 USBDataCtrl_ItemSave_Proc(UINT8* pDataBufferAddr)
{

    INT32 StatusRet = USBDataCtrl_OK;
    // (void)pDataBufferAddr;
    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s: USBDataCtrl_ItemSave_Proc", __func__, NULL, NULL, NULL, NULL);
    usbdatactrl_print_uint32_5("[USBDataCtrl][DEBUG] Item: %c ", (UINT8) USBDataCtrlCfg.Item, 0, 0, 0, 0);

    switch(USBDataCtrlCfg.Item) {

    case GDataParameters:
        StatusRet = USBHdlr_Save_Reg_Params(pDataBufferAddr);
        USBDataCtrlCfg.IKInitFlg = 0;
        USBDataCtrlCfg.CalibTableInitFlag = 0;
        break;

    case Ituner:
        StatusRet = USBDataCtrl_ItunerSave(pDataBufferAddr);
        break;

    case CC:
        StatusRet = USBDataCtrl_CCSave(pDataBufferAddr);
        USBDataCtrlCfg.IKInitFlg = 0;
        USBDataCtrlCfg.CalibTableInitFlag = 0;

        break;

    case WARP:
        StatusRet = USBDataCtrl_WarpSave(pDataBufferAddr);
        break;

    case VIG:
        StatusRet = USBDataCtrl_VigSave(pDataBufferAddr);
        break;

    case CA:
        StatusRet = USBDataCtrl_CaSave(pDataBufferAddr);
        break;


    case AAAControl:
        StatusRet = USBDataCtrl_AAASave(pDataBufferAddr);
        break;

    case AEControl:
        StatusRet = USBDataCtrl_AESave(pDataBufferAddr);
        break;

    default:
        usbdatactrl_print_str_5("[USBDataCtrl][WARNING] %s: default", "USBDataCtrl_ItemSave_Proc", NULL, NULL, NULL, NULL);
        break;
    }

    return StatusRet ;
}



static INT32 USBDataCtrl_ItemLoad_Proc(UINT8* pDataBufferAddr)
{

    INT32 StatusRet = USBDataCtrl_OK;
    usbdatactrl_print_str_5("[USBDataCtrl][DEBUG] %s: USBDataCtrl_ItemLoad_Proc", __func__, NULL, NULL, NULL, NULL);
    usbdatactrl_print_uint32_5("[USBDataCtrl][DEBUG] Item: %c ", (UINT8) USBDataCtrlCfg.Item, 0, 0, 0, 0);

    (void)pDataBufferAddr;

    switch(USBDataCtrlCfg.Item) {

    case AmageModeConfig:
        StatusRet = USBDataCtrl_AmageModeConfig_Init(pDataBufferAddr);
        if(StatusRet != 0) {
            usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s: call USBDataCtrl_AmageModeConfig_Init", "USBDataCtrl_ItemLoad_Proc", NULL, NULL, NULL, NULL);
        }
        StatusRet = USBDataCtrl_Tsk_Init(); //Creat LoadIDSP Tsk
        break;
    case IKModeConfig:
        StatusRet = USBDataCtrl_IKModeConfig_Init(pDataBufferAddr);
        break;

    case Ituner:
        StatusRet = USBDataCtrl_ItunerLoad(pDataBufferAddr);
        break;

    case CC:
        StatusRet = USBDataCtrl_CCLoad(pDataBufferAddr);
        break;
    case WARP:
        StatusRet = USBDataCtrl_WarpLoad(pDataBufferAddr);
        break;

    case VIG:
        StatusRet = USBDataCtrl_VigLoad(pDataBufferAddr);
        break;

    case CA:
        StatusRet = USBDataCtrl_CaLoad(pDataBufferAddr);
        break;
    case EndProc:
        StatusRet = USBDataCtrl_EndProc(pDataBufferAddr);
        USBDataCtrlCfg.IKInitFlg = 0;
        USBDataCtrlCfg.CalibTableInitFlag = 0;
        break;

    case AAAControl:
        StatusRet = USBDataCtrl_AAALoad(pDataBufferAddr);
        break;

    case AEControl:
        StatusRet = USBDataCtrl_AELoad(pDataBufferAddr);
        break;

    default:
        usbdatactrl_print_str_5("[USBDataCtrl][WARNING] %s() default", "USBDataCtrl_ItemLoad_Proc", NULL, NULL, NULL, NULL);
        break;

    }

    return StatusRet ;
}


INT32 AmbaItn_USBCtrl_Communication (UINT32 Parameter1, UINT32 Parameter2, UINT32 Parameter3, UINT32 Parameter4, UINT32 Parameter5, UINT32* length, UINT32* dir)
{

    INT32 StatusRet = USBDataCtrl_OK;
    *dir = Parameter1;
    *length = Parameter2;

    usbdatactrl_print_uint32_5("[USBDataCtrl][DEBUG] Communication  p1 %d , p2 %d, p3 %c, p4 %d, p5 %d  ",  Parameter1, Parameter2, Parameter3, Parameter4, Parameter5);

    (void)AmbaItn_Change_Parser_Mode(USB_TUNE);

    USBDataCtrlCfg.AmageSendDataSize = Parameter2;
    AmbaMisra_TypeCast(&USBDataCtrlCfg.Item, &Parameter3);

    /*
    switch(Parameter3){
        case (UINT32)AmageModeConfig:
            USBDataCtrlCfg.Item = AmageModeConfig;
            break;
        case (UINT32)IKModeConfig:
            USBDataCtrlCfg.Item = IKModeConfig;
            break;

        case (UINT32)GDataParameters:
            USBDataCtrlCfg.Item = GDataParameters;
            break;

        case (UINT32)Ituner:
            USBDataCtrlCfg.Item = Ituner;
            break;

        case (UINT32)AAAControl:
            USBDataCtrlCfg.Item = AAAControl;
            break;

        case (UINT32)AEControl:
            USBDataCtrlCfg.Item = AEControl;
            break;


        case (UINT32)CC:
            USBDataCtrlCfg.Item = CC;
            break;


        case (UINT32)WARP:
            USBDataCtrlCfg.Item = WARP;
            break;

        case (UINT32)FPN:
            USBDataCtrlCfg.Item = FPN;
            break;

        case (UINT32)VIG:
            USBDataCtrlCfg.Item = VIG;
            break;

        case (UINT32)CA:
            USBDataCtrlCfg.Item = CA;
            break;
        case (UINT32)EndProc:
            USBDataCtrlCfg.Item = EndProc;
            break;

        default:
            usbdatactrl_print_str_5("[USBDataCtrl][WARNING] %s() default", "AmbaItn_USBCtrl_Communication", NULL, NULL, NULL, NULL);
            USBDataCtrlCfg.Item = Default;
            break;

    }
    */

    if (*dir == DIR_ARM_TO_PC) {
        StatusRet = USBDataCtrl_CN_ARMToPC(length);
    }

    if(*dir == DIR_PC_TO_ARM) {
        StatusRet = USBDataCtrl_CN_PCToARM();
    }

    return StatusRet;
}


INT32 AmbaItn_USBCtrl_Save(UINT8 *ObjectBuffer, UINT32 ObjectOffset, UINT32 ObjectLengthRequested, UINT32 *ObjectActualLength)
{
    const UINT8* tmpAddr = NULL;
    UINT8* pDataBufferAddr = NULL;

    // INT32 Copylimit = 10000;
    // INT32 CopyLength = 0;

    *ObjectBuffer = 0;

    pDataBufferAddr = USBDataCtrl_GetDataBufferAddr();

    if (ObjectOffset == 0U) {
        (void)USBDataCtrl_ItemSave_Proc(pDataBufferAddr);
    }

    tmpAddr = (UINT8*) &pDataBufferAddr[ObjectOffset];
    if(0U != usbdatactrl_memcpy(ObjectBuffer, tmpAddr, ObjectLengthRequested)) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() usbdatactrl_memcpy error",__func__,NULL,NULL,NULL,NULL);
    }

    *ObjectActualLength = ObjectLengthRequested;

    return USBDataCtrl_OK;
}

INT32 AmbaItn_USBCtrl_Load(const UINT8 *ObjectBuffer, UINT32 ObjectOffset, UINT32 ObjectLength)
{
    UINT8* pDataBufferAddr;

    // (void)ObjectBuffer;
    // (void)ObjectOffset;
    // (void)ObjectLength;

    pDataBufferAddr = USBDataCtrl_GetDataBufferAddr();

    if(0U != usbdatactrl_memcpy(&pDataBufferAddr[ObjectOffset], ObjectBuffer, ObjectLength)) {
        usbdatactrl_print_str_5("[USBDataCtrl][ERROR] %s() usbdatactrl_memcpy error",__func__,NULL,NULL,NULL,NULL);
    }

    if ((ObjectOffset+ObjectLength) >= USBDataCtrlCfg.AmageSendDataSize) {
        (void) USBDataCtrl_ItemLoad_Proc(pDataBufferAddr);
    }

    return USBDataCtrl_OK;
}
