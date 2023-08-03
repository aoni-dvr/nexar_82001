#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"

/* ssp */

/* framework */
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcTask.h"
#include "SvcUtil.h"

/* app-shared */
#include "SvcSysStat.h"
#include "SvcRecMain.h"
#include "SvcResCfg.h"
#include "SvcVinSrc.h"
#include "AmbaVIN_Priv.h"
#include "SvcImg.h"
#include "SvcDataCmp.h"

/* app-icam*/
#include "SvcRecTask.h"
#include "SvcAppStat.h"

#include "SvcShmooRecTask.h"
#include "SvcUserPref.h"
#ifdef CONFIG_ICAM_YUVFILE_FEED
#include "SvcLvFeedFileYuvTask.h"
#endif


#define SVC_SHMOO_REC_TASK_PRI                (70U)
#define SVC_SHMOO_REC_TASK_CPU_BITS           (0x01U)
#define SVC_SHMOO_REC_BITSCMP_TASK_PRI        (65U)
#define SVC_SHMOO_REC_BITSCMP_TASK_CPU_BITS   (0x01U)

#define SVC_LOG_SHMOO_REC_TASK "ShmooRecTask"

static void ShmooRecTask_StatusCallback(UINT32 StatIdx, void *pInfo);

static AMBA_KAL_EVENT_FLAG_t ShmooRecTaskEvent;
#define SHMOO_REC_TASK_UCODE_ALL          (0x01U)
#define SHMOO_REC_TASK_UCODE_STAGE1_DONE  (0x02U)
#define SHMOO_REC_TASK_DSP_BOOT_DONE      (0x04U)

#define SHMOO_REC_TASK_MAX_PAIR_NUM       (4U)

typedef struct {

    UINT32 RecPairId;
    UINT32 RecCmpBits;
    UINT32 CmpId[CONFIG_ICAM_MAX_REC_STRM];
    SVC_DATA_CMP_HDLR_s RecBitsCmpHdlr;

} SVC_SHMOO_REC_CMPINFO_s;

typedef struct {

    UINT32 RecPairs;
    UINT32 FoVBits;

} SVC_SHMOO_REC_INFO_s;


static SVC_SHMOO_REC_CMPINFO_s gShmooRecCtrl[SHMOO_REC_TASK_MAX_PAIR_NUM];
static SVC_SHMOO_REC_INFO_s    gShmooRecInfoCtrl;

// static UINT32 gFoVBits = 0U;



static void ShmooRecTask_StatusCallback(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal;
    SVC_APP_STAT_MENU_s *pStatus = NULL;
    SVC_APP_STAT_UCODE_s   *pUcodeStatus = NULL;
    SVC_APP_STAT_DSP_BOOT_s *pDspBootStatus = NULL;


    AmbaMisra_TouchUnused(pInfo);   /* Misra-c fixed */
    AmbaMisra_TouchUnused(pStatus); /* Misra-c fixed */

    AmbaMisra_TouchUnused(pUcodeStatus);
    AmbaMisra_TouchUnused(pDspBootStatus);

   if (StatIdx == SVC_APP_STAT_UCODE){
        AmbaMisra_TypeCast(&pUcodeStatus, &pInfo);
        if(pUcodeStatus->Status == SVC_APP_STAT_UCODE_ALL_DONE){
            RetVal = AmbaKAL_EventFlagSet(&ShmooRecTaskEvent, SHMOO_REC_TASK_UCODE_ALL);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "ShmooRecTask_StatusCallback() err, AmbaKAL_EventFlagSet REC_TASK_UCODE_ALL failed with 0x%x", RetVal, 0U);
            }
        } else if (pUcodeStatus->Status == SVC_APP_STAT_UCODE_LOAD_STAGE1_DONE){
            RetVal = AmbaKAL_EventFlagSet(&ShmooRecTaskEvent, SHMOO_REC_TASK_UCODE_STAGE1_DONE);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "ShmooRecTask_StatusCallback() err, AmbaKAL_EventFlagSet REC_TASK_UCODE_ALL failed with 0x%x", RetVal, 0U);
            }
        } else {
            /**/
        }
    } else if (StatIdx == SVC_APP_STAT_DSP_BOOT){
        AmbaMisra_TypeCast(&pDspBootStatus, &pInfo);
        if (pDspBootStatus->Status == SVC_APP_STAT_DSP_BOOT_DONE){
            RetVal = AmbaKAL_EventFlagSet(&ShmooRecTaskEvent, SHMOO_REC_TASK_DSP_BOOT_DONE);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "ShmooRecTask_StatusCallback() err, AmbaKAL_EventFlagSet REC_TASK_DSP_BOOT_DONE failed with 0x%x", RetVal, 0U);
            }
        }
    } else {
        /* Do nothing */
    }

}


void SvcShmooRecTask_DataCmpCheck(UINT32 *pHaveError, UINT32 StopFlag, UINT32 PrintReport)
{

    UINT32                      RetVal,IsStop= 0U, i;
    SVC_DATA_CMP_REPORT_s       Report;
    static UINT32 prevCount[SHMOO_REC_TASK_MAX_PAIR_NUM] = {0U}, zeroCount[SHMOO_REC_TASK_MAX_PAIR_NUM] = {0U};
    static UINT32 VinOff = 0U;


    for(i=0U; i<gShmooRecInfoCtrl.RecPairs; i++){
        if(gShmooRecCtrl[i].RecCmpBits != 0U){

            RetVal = SvcDataCmp_ReportGet(&(gShmooRecCtrl[i].RecBitsCmpHdlr), &Report);
            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "SvcDataCmp_ReportGet failed %u", RetVal, 0U);
            } else {
                if(PrintReport == 1U){
                    AmbaPrint_PrintUInt5("EncodePairs:[%u], ToatalCmp:[%u], ErrCount:[%u]", i, (UINT32) Report.CmpCount, (UINT32) Report.ErrCount, 0U, 0U);
                } else {

                    if((UINT32)Report.CmpCount == 0U){
                        zeroCount[i] = 1U;
                    }

                    if((zeroCount[i] > 0U) || ( (prevCount[i] > 0U) && (prevCount[i] == (UINT32)Report.CmpCount) )){
                        *pHaveError |= 1U;
                        IsStop |= 1U;
                        SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "Dsp should assert now", 0U, 0U);
                    } else {
                        prevCount[i] = (UINT32)Report.CmpCount;
                        *pHaveError = (Report.ErrCount == 0U) ? (*pHaveError | 0U) : (*pHaveError | 1U);
                    }
                }

            }


            if((*pHaveError == 1U) || (IsStop == 1U) || (StopFlag == 1U)){
#if defined(CONFIG_ICAM_YUVFILE_FEED)
                if(VinOff == 0U){
                    RetVal = SvcLvFeedFileYuvTask_Stop(gShmooRecInfoCtrl.FoVBits);
                    if(RetVal != SVC_OK){
                        SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "SvcLvFeedFileYuvTask_Stop failed %u", RetVal, 0U);
                    } else {
                        VinOff = 1U;
                    }
                }
#endif
                SvcRecMain_Stop(gShmooRecCtrl[i].RecCmpBits,0U);
                RetVal = SvcDataCmp_Delete(&(gShmooRecCtrl[i].RecBitsCmpHdlr));
                if (RetVal != SVC_OK) {
                    SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "SvcDataCmp_Delete failed %u", RetVal, 0U);
                }
                SvcLog_OK(SVC_LOG_SHMOO_REC_TASK, "Stop Video encode and bitstream comparison", 0U, 0U);
            }
        }

    }


}

static UINT32 RecDataRdyCB(const void *pEventData)
{


    UINT32                  RetVal, i;
    SVC_DATA_CMP_SEND_s     BitsCmp;
    const UINT64          *pPts;
    AMBA_DSP_ENC_PIC_RDY_s   *pEvt;
    SVC_SHMOO_REC_CMPINFO_s *pShmooRecCtrl = NULL;
    SVC_REC_MAIN_INFO_s RecInfo;

    AmbaMisra_TypeCast(&pEvt, &pEventData);
    for(i=0U; i<gShmooRecInfoCtrl.RecPairs; i++){
        if( ((gShmooRecCtrl[i].RecCmpBits >>  pEvt->StreamId) & 1U ) == 1U){
            pShmooRecCtrl = &gShmooRecCtrl[i];
            break;
        }
    }

    // SvcLog_OK(SVC_LOG_SHMOO_REC_TASK, "### StreamId:[%u], FrameNum:[%u] ###", pEvt->StreamId, pEvt->FrmNo);
    if(pShmooRecCtrl != NULL){
        const AMBA_RSC_USR_CFG_s    *pRsc = NULL;
        SvcRecMain_InfoGet(&RecInfo);

        for(i=0; i<RecInfo.pFwkStrmCfg->NumRsc; i++){
            if(RecInfo.pFwkStrmCfg->RscCfgArr[i]->SrcType == AMBA_RSC_TYPE_VIDEO ){
                pRsc = RecInfo.pFwkStrmCfg->RscCfgArr[i];
                break;
            }
        }

        if(pRsc != NULL){
            BitsCmp.CmpId        = (UINT8)pShmooRecCtrl->CmpId[pEvt->StreamId];
            BitsCmp.NumData      = 2U;

            pPts = &(pEvt->Pts);
            AmbaMisra_TypeCast(&(BitsCmp.Data[0].DataAddr), &pPts);
            BitsCmp.Data[0].DataSize = sizeof(UINT64);
            AmbaMisra_TypeCast(&(BitsCmp.Data[0].BufBase), &pPts);
            BitsCmp.Data[0].BufSize  = sizeof(UINT64);

            AmbaMisra_TypeCast(&(BitsCmp.Data[1].DataAddr), &(pEvt->StartAddr));
            BitsCmp.Data[1].DataSize = pEvt->PicSize;
            BitsCmp.Data[1].BufBase  = pRsc->BsBufBase;
            BitsCmp.Data[1].BufSize  = pRsc->BsBufSize;

            RetVal = SvcDataCmp_DataSend(&(pShmooRecCtrl->RecBitsCmpHdlr), &BitsCmp);
            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "SvcDataCmp_DataSend HandlerID:[%u] failed %u", pShmooRecCtrl->RecPairId, RetVal);
            }
        }
    }

    return 0U;

}


static void ShmooPreStart(void)
{
    UINT32 NumStrm = 0U;


    UINT32 RetVal, i, PairCount;
    SVC_USER_PREF_s *pSvcUserPref;



    /* Disable 3A */
    SvcImg_StatisticsMask(0U);
    SvcImg_SyncEnable(0U,0U);


    RetVal = SvcUserPref_Get(&pSvcUserPref);
    if(RetVal == SVC_OK){
        SVC_DATA_CMP_TASK_CREATE_s  Create;
        gShmooRecInfoCtrl.RecPairs = pSvcUserPref->ShmooInfo.RecNum;
        for(PairCount=0U; PairCount<gShmooRecInfoCtrl.RecPairs; PairCount++){
            NumStrm = 0U;
            gShmooRecCtrl[PairCount].RecCmpBits = pSvcUserPref->ShmooInfo.RecCmpBits[PairCount];
            gShmooRecCtrl[PairCount].RecPairId = PairCount;

            for(i=0U; i< CONFIG_ICAM_MAX_REC_STRM; i++){
                if( ((gShmooRecCtrl[PairCount].RecCmpBits >> i) & 1U ) == 1U ){
                    gShmooRecCtrl[PairCount].CmpId[i] = NumStrm;
                    // AmbaPrint_PrintUInt5("#### gShmooRecCtrl[%u].CmpId[%u] = %u ####", PairCount, i, NumStrm, 0U, 0U);
                    NumStrm ++;
                }
            }

            Create.CmpNum       = (UINT8)NumStrm;
            Create.DataNum      = 2U;
            Create.TaskPriority = SVC_SHMOO_REC_BITSCMP_TASK_PRI;
            Create.TaskCpuBits  = SVC_SHMOO_REC_BITSCMP_TASK_CPU_BITS;

            RetVal = SvcDataCmp_Create(&(gShmooRecCtrl[PairCount].RecBitsCmpHdlr), &Create);

            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "SvcDataCmp_Create failed %u", RetVal, 0U);
            } else {
                SvcLog_OK(SVC_LOG_SHMOO_REC_TASK, "SvcDataCmp_Create [%u] Success, CmpBits:[%u]", PairCount, gShmooRecCtrl[PairCount].RecCmpBits);
            }
        }
    }

    RetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_DATA_RDY, RecDataRdyCB);
    if (RetVal != OK) {
        SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "## fail to register enc_data_rdy, RetVal(%u)", RetVal, 0U);
    }



    // {
    //     UINT32 BrateCtrl;
    //     BrateCtrl = AMBA_DSP_BRC_DISABLE_AND_FIX_QP;
    //     SvcEnc_ParamCtrl(SVC_ENC_PMT_BRATECTRL, (UINT16)NumStrm, StrmIdxArr, &BrateCtrl);
    // }

    SvcLog_OK(SVC_LOG_SHMOO_REC_TASK, "BitStream Compare On!", 0U, 0U);



}

static void ShmooPostStart(void)
{
    UINT32 RetVal, FovSrc;

    AmbaMisra_TouchUnused(&RetVal);


    RetVal = SvcResCfg_GetFovSrc(0U, &FovSrc);
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "SvcResCfg_GetFovSrc with %d", RetVal, 0U);
    }

    if (RetVal == SVC_OK) {
        /*For Advanced Shmoo Mem input case*/
        if ((FovSrc == SVC_VIN_SRC_MEM_YUV420) || (FovSrc == SVC_VIN_SRC_MEM_YUV422) || (FovSrc == SVC_VIN_SRC_MEM_DEC)) {
            #if defined(CONFIG_ICAM_YUVFILE_FEED)
            UINT32                  i, FovBits = 0U, Bits = 1U;
            const SVC_RES_CFG_s     *pCfg = SvcResCfg_Get();
            SVC_FEEDYUV_CREATE_s    CreateInfo = {0};

            CreateInfo.FovNum          = pCfg->FovNum;
            CreateInfo.UpdateInterval  = 50U;

            for(i = 0U; i < CreateInfo.FovNum; i++){
                CreateInfo.FovInfo[i].FovIdx         = i;
                CreateInfo.FovInfo[i].BufNum         = 1U;
                CreateInfo.FovInfo[i].FileName[0][0] = '\0';

                FovBits |= (Bits << i);
            }
            gShmooRecInfoCtrl.FoVBits = FovBits;

            RetVal = SvcLvFeedFileYuvTask_TaskCreate(&CreateInfo);
            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "SvcLvFeedFileYuvTask_TaskCreate failed!!", RetVal, 0U);
            }

            RetVal = SvcLvFeedFileYuvTask_Start(FovBits);
            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "SvcLvFeedFileYuvTask_Start failed!!", RetVal, 0U);
            }

            #endif
        }
    }

    if (RetVal == SVC_OK) {
        SVC_APP_STAT_SHMOO_REC_s  ShmooStatus;

        ShmooStatus.Status = SVC_APP_STAT_SHMOO_REC_ON;

        RetVal = SvcSysStat_Issue(SVC_APP_STAT_SHMOO_REC, &ShmooStatus);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "Issue STAT_SHMOO failed", 0U, 0U);
        }
    }
}


static void * SvcShmooRecTask_TaskEntry(void * Arg)
{
    UINT32    RetVal = SVC_OK;

    AmbaMisra_TouchUnused(Arg);
    AmbaMisra_TouchUnused(&RetVal);

    /* wait dsp boot done */
    if (RetVal == SVC_OK) {
        UINT32 ActualFlag = 0U, WaitFlag;

        #ifdef CONFIG_ICAM_UCODE_PARTIAL_LOAD
        WaitFlag = SHMOO_REC_TASK_UCODE_STAGE1_DONE;
        #else
        WaitFlag = SHMOO_REC_TASK_DSP_BOOT_DONE | SHMOO_REC_TASK_UCODE_ALL;
        #endif

        SvcLog_OK(SVC_LOG_SHMOO_REC_TASK, "Wait for DSP and Ucode", 0U, 0U);
        RetVal = AmbaKAL_EventFlagGet(&ShmooRecTaskEvent,
                                    WaitFlag,
                                    AMBA_KAL_FLAGS_ALL,
                                    AMBA_KAL_FLAGS_CLEAR_NONE,
                                    &ActualFlag,
                                    AMBA_KAL_WAIT_FOREVER);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "Wait REC_TASK_UCODE_ALL and REC_TASK_DSP_BOOT_DONE (%u) failed with %d", WaitFlag, RetVal);
            RetVal = SVC_NG;
        }
    }

    if (RetVal == SVC_OK) {
        SVC_USER_PREF_s *pSvcUserPref;
        UINT32 FovSrc, RecBits = 0U, i;

        RetVal = SvcResCfg_GetFovSrc(0U, &FovSrc);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "SvcResCfg_GetFovSrc with %d", RetVal, 0U);
        }

        SvcRecTask_StartMV();

        ShmooPreStart();

        if(SVC_OK == SvcUserPref_Get(&pSvcUserPref)){
            for(i=0U; i<SHMOO_REC_TASK_MAX_PAIR_NUM; i++){
                RecBits |= pSvcUserPref->ShmooInfo.RecCmpBits[i];
            }
        }

        if ((FovSrc == SVC_VIN_SRC_MEM_YUV420) || (FovSrc == SVC_VIN_SRC_MEM_YUV422) || (FovSrc == SVC_VIN_SRC_MEM_DEC)) 
        {
            for(i=0U; i<CONFIG_ICAM_MAX_REC_STRM; i++){
                if( ((RecBits >> i) & 1U) == 1U ){
                    SvcRecMain_Start( 1U << i, 1U);
                }
            }

        } else {
            SvcRecMain_Start(RecBits, 1U);
        }

        ShmooPostStart();

    }

    return NULL;
}


UINT32 SvcShmooRecTask_ShmooTask(void)
{

    UINT32 RetVal, CtrlID;
    static SVC_TASK_CTRL_s ShmooRecTaskCtrl GNU_SECTION_NOZEROINIT;
    #define RECHMOOSTACKSIZE 0x3000U
    static UINT8 ShmooRecTaskStack[RECHMOOSTACKSIZE] GNU_SECTION_NOZEROINIT;
    static char ShmooRecTaskEventName[32] = "ShmooRecTaskEvent";
    static char ShmooRecTaskName[16] = "ShmooRecTask";

    RetVal = AmbaKAL_EventFlagCreate(&ShmooRecTaskEvent, ShmooRecTaskEventName);

    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "Create event flag failed %d", RetVal, 0U);
    }

    RetVal |= SvcSysStat_Register(SVC_APP_STAT_UCODE,    ShmooRecTask_StatusCallback, &CtrlID);
    RetVal |= SvcSysStat_Register(SVC_APP_STAT_DSP_BOOT, ShmooRecTask_StatusCallback, &CtrlID);


    // {
    //     /* fix qp and bitrate */
    //     const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    //     UINT32 RecBits = pCfg->RecBits, BrateCtrl, NumStrm;
    //     UINT16 StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM] = {0};

    //     SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, RecBits, &NumStrm, StrmIdxArr);
    //     BrateCtrl = AMBA_DSP_BRC_DISABLE_AND_FIX_QP;
    //     SvcEnc_ParamCtrl(SVC_ENC_PMT_BRATECTRL, (UINT16)NumStrm, StrmIdxArr, &BrateCtrl);

    // }

    /*NULL wrtie , need to set before config*/
    {
        UINT32 ONOFF = 1U;
        SvcRecMain_Control(SVC_RCM_PMT_NLWR_SET, 0, NULL, &ONOFF);
    }

    if (RetVal != SVC_OK){
        SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "Ucode and dsp boot stat register failed", 0U, 0U);
    } else {
        ShmooRecTaskCtrl.Priority   = SVC_SHMOO_REC_TASK_PRI;
        ShmooRecTaskCtrl.EntryFunc  = SvcShmooRecTask_TaskEntry;
        ShmooRecTaskCtrl.pStackBase = ShmooRecTaskStack;
        ShmooRecTaskCtrl.StackSize  = RECHMOOSTACKSIZE;
        ShmooRecTaskCtrl.CpuBits    = SVC_SHMOO_REC_TASK_CPU_BITS;
        ShmooRecTaskCtrl.EntryArg   = (UINT32) 0U;
    }

    RetVal = SvcTask_Create(ShmooRecTaskName, &ShmooRecTaskCtrl);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SHMOO_REC_TASK, "SvcShmooRecTask create failed", 0U, 0U);
    }
    return RetVal;

}
