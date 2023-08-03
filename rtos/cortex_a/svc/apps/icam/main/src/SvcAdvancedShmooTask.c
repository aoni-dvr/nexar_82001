#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"
#include "AmbaShell.h"
#include "AmbaSYS.h"

/* framework */
#include "SvcTask.h"
#include "SvcLog.h"
#include "SvcErrCode.h"

/* app-shared */
#include "SvcSysStat.h"
#include "SvcRecMain.h"

/* app-icam */
#include "SvcAppStat.h"
#include "SvcTaskList.h"
#include "SvcShmooCvTask.h"
#include "SvcShmooRecTask.h"
#include "SvcUserPref.h"

#include "SvcPref.h"
#include "AmbaDramShmoo.h"
#include "SvcAdvancedShmooTask.h"
#include "SvcWrap.h"
#include "SvcStgMonitor.h"
#ifdef CONFIG_SOC_CV2FS
#include "SvcSafeStateMgr.h"
#endif
#include "SvcCmd.h"
#include "AmbaNVM_Partition.h"
#include "SvcVinTree.h"


#define SVC_LOG_SHMOO_TASK "ADV_SHMOO"
#define SHMOO_TASK_APP_START_ENCODE         (  0x1U  )
#define SHMOO_TASK_APP_START_CV             (  0x2U  )
#define SHMOO_TASK_DONE                     (  0x4U  )
#define SVC_ADVANCED_SHMOO_TASK_PRI         (  100U  )
#define SVC_ADVANCED_SHMOO_TASK_CPU_BITS    (  0x1U  )

#define MODEINFO    (0U)
#define ITEMINFO    (1U)
#define MISCINFO    (2U)
#define SHMOOSTART  (3U)

static AMBA_KAL_EVENT_FLAG_t ShmooTaskEvent;
static UINT32 gShmooMode = 0xFFFFFFFFU;
static UINT32* gShmooResultBuffer = NULL;
static AMBA_FS_FILE *pFile = NULL;

typedef struct{
    UINT32 RecMode;
    UINT32 CvMode;
    UINT32 RecNum;
    UINT32 CvNum;
    UINT32 RecCmpBits[4U];
} SVC_ADV_SHMOO_INFO_s;

static SVC_ADV_SHMOO_INFO_s gAdvShmooCtrl = {0};


static void AdvancedShmoo_StatusCallback(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal;
    SVC_APP_STAT_SHMOO_REC_s *pShmooRecStatus = NULL;
    SVC_APP_STAT_SHMOO_CV_s *pShmooCvStatus = NULL;

    AmbaMisra_TouchUnused(pInfo);
    AmbaMisra_TouchUnused(pShmooRecStatus);
    AmbaMisra_TouchUnused(pShmooCvStatus);

    if(StatIdx == SVC_APP_STAT_SHMOO_REC){
        AmbaMisra_TypeCast(&pShmooRecStatus, &pInfo);
        if(pShmooRecStatus->Status == SVC_APP_STAT_SHMOO_REC_ON){
            RetVal = AmbaKAL_EventFlagSet(&ShmooTaskEvent, SHMOO_TASK_APP_START_ENCODE);
            if(RetVal == SVC_NG){
                SvcLog_OK(SVC_LOG_SHMOO_TASK, "Set SHMOO_TASK_APP_START_ENCODE Flag failed", 0U, 0U);
            }
        }
    } else if(StatIdx == SVC_APP_STAT_SHMOO_CV){
        AmbaMisra_TypeCast(&pShmooCvStatus, &pInfo);
        if(pShmooCvStatus->Status  == SVC_APP_STAT_SHMOO_CV_ON){
            RetVal = AmbaKAL_EventFlagSet(&ShmooTaskEvent, SHMOO_TASK_APP_START_CV);
            if(RetVal == SVC_NG){
                SvcLog_OK(SVC_LOG_SHMOO_TASK, "Set SHMOO_TASK_APP_START_CV Flag failed", 0U, 0U);
            }
        }
    } else {
        /**/
    }
}


static void AdvancedShmoo_CheckCallback(UINT32 *pHaveError)
{

    if((gShmooMode & SHMOO_TASK_APP_START_ENCODE) > 0U ){
        // SvcRecMain_Control(SVC_RCM_SHMOO_CTRL, 1, NULL, pHaveError); /*NumStrm is used for condition check, not real value*/
        SvcShmooRecTask_DataCmpCheck(pHaveError, 0U, 0U);
    }
#ifdef CONFIG_BUILD_CV
    if ((gShmooMode & SHMOO_TASK_APP_START_CV) > 0U){
        SvcShmooCvTask_DataCmpCheck(pHaveError, 0U);
    }
#endif
    AmbaMisra_TouchUnused(pHaveError);

}

static inline UINT32 IsolatingBits(UINT32 orig, UINT32 from, UINT32 to)
{
  UINT32 one = 1U;
  UINT32 mask = ( (one <<((to-from)+one))-one) << from;
  return (orig & mask) >> from;
}

static void AdvancedShmoo_PrintResult(const char *ItemName, UINT32 Org, UINT32 Left, UINT32 Right, UINT32 From, UINT32 To)
{
    char printBuf[512U];
    char space[20U];
    UINT32 RetVal,i,LengthCount, NumSuccess, one=1U;
    INT32 IsolateValue[3U] = {0}; // 0: Seeds, 1:Min, 2:Max

    if(Org < 50U){
        LengthCount = 15U - SvcWrap_strlen(ItemName);
        for(i=0; i<LengthCount; i++){
            space[i] = ' ';
        }
        space[LengthCount] = '\0';

        if( Org == SHMOO_OFFSET_DLL_ORI ){
            UINT32 SignedBit[3] = {0U}, Value;
            INT32 TmpValue = 0;
            SignedBit[0] = gShmooResultBuffer[Org] & (one << To);
            SignedBit[1] = gShmooResultBuffer[Left] &  (one << To);
            SignedBit[2] = gShmooResultBuffer[Right] &  (one << To);

            for(i=0; i<3U; i++){
                Value = i;
                AmbaMisra_TouchUnused(&Value);
                switch (Value){
                    case 0U:
                    TmpValue = (INT32) IsolatingBits(gShmooResultBuffer[Org], From, To -1U);
                    break;
                    case 1U:
                    TmpValue = (INT32) IsolatingBits(gShmooResultBuffer[Left], From, To -1U);
                    break;
                    case 2U:
                    TmpValue = (INT32) IsolatingBits(gShmooResultBuffer[Right], From, To -1U);
                    break;
                    default:
                    AmbaMisra_TouchUnused(&Value);
                    break;
                }
                IsolateValue[i] = (SignedBit[i] == 0U) ? TmpValue : (0 - TmpValue);
            }

        } else{
            IsolateValue[0U] = (INT32) IsolatingBits(gShmooResultBuffer[Org], From, To);
            IsolateValue[1U] = (INT32) IsolatingBits(gShmooResultBuffer[Left], From, To);
            IsolateValue[2U] = (INT32) IsolatingBits(gShmooResultBuffer[Right], From, To);

#if 0
            if(Org == SHMOO_OFFSET_DQ_READ_DLY_ORI){ // Special case for dqReadDly
                UINT32 TempU;
                for(i=0; i<3U; i++){
                    TempU = (UINT32) IsolateValue[i];
                    TempU = TempU >> 1U;
                    IsolateValue[i] = (INT32) TempU;
                }
            }
#endif
        }

        SVC_WRAP_SNPRINT "%s[%2d:%2d]%s      0x%08X / %3d        0x%08X / %3d         0x%08X / %3d"
            SVC_SNPRN_ARG_S printBuf
            SVC_SNPRN_ARG_CSTR   ItemName                   SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32 To                         SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32 From                       SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_CSTR   space                      SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32 gShmooResultBuffer[Org]    SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_INT32  IsolateValue[0U]           SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32 gShmooResultBuffer[Left]   SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_INT32  IsolateValue[1U]           SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32 gShmooResultBuffer[Right]  SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_INT32  IsolateValue[2U]           SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_BSIZE  512U
            SVC_SNPRN_ARG_RLEN   &RetVal
            SVC_SNPRN_ARG_E
    } else {
        LengthCount = SvcWrap_strlen(ItemName) + 1U;
        SvcWrap_strcpy(printBuf, LengthCount, ItemName);
    }


    AmbaPrint_PrintStr5("%s",printBuf, NULL,NULL,NULL,NULL);
    AmbaMisra_TouchUnused(&NumSuccess);

    if(pFile != NULL){
        LengthCount = SvcWrap_strlen(printBuf);
        printBuf[LengthCount] = '\n';
        LengthCount++;
        printBuf[LengthCount] = '\0';
        RetVal = AmbaFS_FileWrite(printBuf, LengthCount, 1U, pFile, &NumSuccess);
        if(RetVal != SVC_OK){
            SvcLog_NG("SVC_LOG_SHMOO_TASK", "File Write failed %u NumSuccess = %u", RetVal, NumSuccess);
        }
    }

}

static void AdvancedShmoo_DumpResult(void)
{
    SVC_USER_PREF_s *pSvcUserPref;
    const AMBA_DRAM_SHMOO_CTRL_s *pShmooItem;
    SVC_STG_MONI_DRIVE_INFO_s  DriveInfo;
    const void *pVoid;
    UINT32 RetVal;
    char MainDrive;
    char ResultPath[32U] = "C:\\DDR_Tuning_Result.txt\0";

    (void)AmbaKAL_TaskSleep(3000U); //Sleep 3s, wait for SD Card init

    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {

        if(pSvcUserPref->ShmooInfo.FileWrite == 1U){
            MainDrive = pSvcUserPref->MainStgDrive[0];
            RetVal = SvcStgMonitor_GetDriveInfo(MainDrive, &DriveInfo);
            if((RetVal != SVC_NG) && (DriveInfo.IsExist == 1U)){
                ResultPath[0] = MainDrive;
                RetVal = AmbaFS_FileOpen(ResultPath,"w",&pFile);
                if(RetVal != SVC_OK){
                    SvcLog_NG("SVC_LOG_SHMOO_TASK", "File Open Failed",0U,0U);
                    (void)AmbaFS_FileClose(pFile);
                }
            } else{
                SvcLog_NG("SVC_LOG_SHMOO_TASK", "Can not find C Drive",0U,0U);
            }
        }

        AdvancedShmoo_PrintResult("---------------------------------------------------------------------------------------------------",100U,0U,0U,0U,0U);
        AdvancedShmoo_PrintResult("ItemName                       Seeds                     Min                     Max",100U,0U,0U,0U,0U);
        AdvancedShmoo_PrintResult("---------------------------------------------------------------------------------------------------",100U,0U,0U,0U,0U);

        pVoid = &(pSvcUserPref->ShmooInfo.ShmooItems);
        AmbaMisra_TypeCast(&pShmooItem, &pVoid);

        if(pShmooItem->pad_dds == 1U){
            AdvancedShmoo_PrintResult("PAD_DDS",SHMOO_OFFSET_PADTERM_ORI,SHMOO_OFFSET_PAD_DDS_1, SHMOO_OFFSET_PAD_DDS_0,2,4);
            AdvancedShmoo_PrintResult("PAD_TERM3_DDS",SHMOO_OFFSET_PADTERM3_ORI,SHMOO_OFFSET_PAD3_DDS_1, SHMOO_OFFSET_PAD3_DDS_0,12,14);
        }
        if(pShmooItem->pad_term == 1U){
            AdvancedShmoo_PrintResult("PAD_TERM",SHMOO_OFFSET_PADTERM_ORI,SHMOO_OFFSET_PAD_TERM_1, SHMOO_OFFSET_PAD_TERM_0,5,7);
            AdvancedShmoo_PrintResult("PAD_TERM3",SHMOO_OFFSET_PADTERM3_ORI,SHMOO_OFFSET_PAD_TERM3_1, SHMOO_OFFSET_PAD_TERM3_0,17,19);
        }
        if(pShmooItem->pad_pdds == 1U){
            AdvancedShmoo_PrintResult("PAD2_PDDS",SHMOO_OFFSET_PADTERM2_ORI,SHMOO_OFFSET_PAD2_PDDS_1, SHMOO_OFFSET_PAD2_PDDS_0,8,10);
            AdvancedShmoo_PrintResult("PAD3_PDDS",SHMOO_OFFSET_PADTERM3_ORI,SHMOO_OFFSET_PAD3_PDDS_1, SHMOO_OFFSET_PAD3_PDDS_0,20,22);
        }
        if(pShmooItem->pad_ca_dds == 1U){
            AdvancedShmoo_PrintResult("PAD2_CA_DDS_B",SHMOO_OFFSET_PADTERM2_ORI,SHMOO_OFFSET_PAD2_CA_DDS_B_1, SHMOO_OFFSET_PAD2_CA_DDS_B_0,0,2);
            AdvancedShmoo_PrintResult("PAD2_CA_DDS_A",SHMOO_OFFSET_PADTERM2_ORI,SHMOO_OFFSET_PAD2_CA_DDS_A_1, SHMOO_OFFSET_PAD2_CA_DDS_A_0,16,18);
        }
        if(pShmooItem->pad_ca_pdds == 1U){
            AdvancedShmoo_PrintResult("PAD2_CA_PDDS_B",SHMOO_OFFSET_PADTERM2_ORI,SHMOO_OFFSET_PAD2_CA_PDDS_B_1, SHMOO_OFFSET_PAD2_CA_PDDS_B_0,12,14);
            AdvancedShmoo_PrintResult("PAD2_CA_PDDS_A",SHMOO_OFFSET_PADTERM2_ORI,SHMOO_OFFSET_PAD2_CA_PDDS_A_1, SHMOO_OFFSET_PAD2_CA_PDDS_A_0,24,26);
        }
        if(pShmooItem->pad_vref == 1U){
            AdvancedShmoo_PrintResult("ReadVref",SHMOO_OFFSET_DQ_VREF_ORI,SHMOO_OFFSET_READ_VREF_1, SHMOO_OFFSET_READ_VREF_0,0,5);
        }
        if(pShmooItem->rddly == 1U){ //Value needs to shift right 1, >> 1U
            AdvancedShmoo_PrintResult("READ_DLY",SHMOO_OFFSET_DQ_READ_DLY_ORI,SHMOO_OFFSET_READ_DELAY_1, SHMOO_OFFSET_READ_DELAY_0,1,4);
        }
        if(pShmooItem->wrdly == 1U){
            AdvancedShmoo_PrintResult("WR_DLY",SHMOO_OFFSET_DQ_WRTITE_DLY_ORI,SHMOO_OFFSET_WRITE_DLY_1, SHMOO_OFFSET_WRITE_DLY_0,0,6);
        }
        if(pShmooItem->dll0 == 1U){
            AdvancedShmoo_PrintResult("READ_DLL",SHMOO_OFFSET_DLL_ORI,SHMOO_OFFSET_READ_DLL_1, SHMOO_OFFSET_READ_DLL_0,0,5);
        }
        if(pShmooItem->dll1 == 1U){
            AdvancedShmoo_PrintResult("SYNC_DLL",SHMOO_OFFSET_DLL_ORI,SHMOO_OFFSET_SYNC_DLL_1, SHMOO_OFFSET_SYNC_DLL_0,8,13);
        }
        if(pShmooItem->dll2 == 1U){
            AdvancedShmoo_PrintResult("WRITE_DLL",SHMOO_OFFSET_DLL_ORI,SHMOO_OFFSET_WRITE_DLL_1, SHMOO_OFFSET_WRITE_DLL_0,16,21);
        }
        if(pShmooItem->lpddr4_pdds == 1U){
            AdvancedShmoo_PrintResult("MR3",SHMOO_OFFSET_MR3_ORI,SHMOO_OFFSET_MR3_1, SHMOO_OFFSET_MR3_0,3,5);
        }
        if(pShmooItem->lpddr4_ca_vref == 1U){
            AdvancedShmoo_PrintResult("MR12",SHMOO_OFFSET_MR12_ORI,SHMOO_OFFSET_MR12_1, SHMOO_OFFSET_MR12_0,0,5);
        }
        if(pShmooItem->lpddr4_dq_vref == 1U){
            AdvancedShmoo_PrintResult("MR14",SHMOO_OFFSET_MR14_ORI,SHMOO_OFFSET_MR14_1, SHMOO_OFFSET_MR14_0,0,5);
        }
        if(pShmooItem->lpddr4_ca_odt == 1U){
            AdvancedShmoo_PrintResult("MR11_CA",SHMOO_OFFSET_MR11_ORI,SHMOO_OFFSET_MR11_CA_1, SHMOO_OFFSET_MR11_CA_0,4,6);
        }
        if(pShmooItem->lpddr4_dq_odt == 1U){
            AdvancedShmoo_PrintResult("MR11_DQ",SHMOO_OFFSET_MR11_ORI,SHMOO_OFFSET_MR11_DQ_1, SHMOO_OFFSET_MR11_DQ_0,0,2);
        }

        if(pFile != NULL){
            (void)AmbaFS_FileClose(pFile);
        }
    }
}

static void AdvancedShmoo_StopCallback(UINT32 * pShmooData)
{
    UINT32 HaveError, PrefBufSize;
    SVC_USER_PREF_s *pSvcUserPref;
    ULONG   PrefBufAddr;

    AmbaMisra_TouchUnused(&HaveError);
    SvcLog_OK(SVC_LOG_SHMOO_TASK, "Shoom Task Done. Stop app bitscmp function", 0U, 0U);
    if((gShmooMode &  SHMOO_TASK_APP_START_ENCODE) > 0U){
        // SvcRecMain_Control(SVC_RCM_SHMOO_CTRL, 0U, NULL, NULL); /*NumStrm is used for condition check, not real value*/
        SvcShmooRecTask_DataCmpCheck(&HaveError, 1U, 0U);
    }
#ifdef CONFIG_BUILD_CV
    if ((gShmooMode & SHMOO_TASK_APP_START_CV) > 0U){
        SvcShmooCvTask_DataCmpCheck(&HaveError, 1U);
    }
#endif
    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        pSvcUserPref->ShmooInfo.ShmooTask = 0U;
        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
        if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
            SvcLog_NG(SVC_LOG_SHMOO_TASK, "SvcPref_Save failed!!", 0U, 0U);
        }
    } else{
        SvcLog_NG(SVC_LOG_SHMOO_TASK, "Get User Pref failed", 0U, 0U);
    }

    gShmooResultBuffer = pShmooData;
    if(SVC_OK != AmbaKAL_EventFlagSet(&ShmooTaskEvent, SHMOO_TASK_DONE))
    {
        SvcLog_OK(SVC_LOG_SHMOO_TASK, "Set SHMOO_TASK_DONE Flag failed", 0U, 0U);
    }
}

static void *AdvancedShmoo_TaskEntry(void * EntryArg)
{

    UINT32 RetVal, ActualFlag, CtrlID;
    SVC_USER_PREF_s *pSvcUserPref;
    AMBA_DRAM_SHMOO_CTRL_s ShmooDramParam = {0};
    AMBA_DRAM_SHMOO_CALLBACK_s ShmooCB = {AdvancedShmoo_CheckCallback, AdvancedShmoo_StopCallback};
    AMBA_DRAM_SHMOO_MISC_s ShmooMisc = {0};

    AmbaMisra_TouchUnused(EntryArg);

    RetVal = SvcSysStat_Register(SVC_APP_STAT_SHMOO_REC, AdvancedShmoo_StatusCallback, &CtrlID);
    RetVal = SvcSysStat_Register(SVC_APP_STAT_SHMOO_CV, AdvancedShmoo_StatusCallback, &CtrlID);

    if(RetVal == SVC_OK){

        if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
            gShmooMode = pSvcUserPref->ShmooInfo.ShmooMode;
            RetVal |= AmbaWrap_memcpy(&ShmooDramParam, &pSvcUserPref->ShmooInfo.ShmooItems, sizeof(AMBA_DRAM_SHMOO_CTRL_s));
            RetVal |= AmbaWrap_memcpy(&ShmooMisc, &pSvcUserPref->ShmooInfo.ShmooMisc, sizeof(AMBA_DRAM_SHMOO_MISC_s));
        } else{
            SvcLog_NG(SVC_LOG_SHMOO_TASK, "Get User Pref failed", 0U, 0U);
        }

        if(RetVal == SVC_OK){

            RetVal = AmbaKAL_EventFlagGet(&ShmooTaskEvent,
                                        gShmooMode,
                                        AMBA_KAL_FLAGS_ALL,
                                        AMBA_KAL_FLAGS_CLEAR_NONE,
                                        &ActualFlag,
                                        AMBA_KAL_WAIT_FOREVER);
            if(RetVal == SVC_OK){

                (void)AmbaKAL_TaskSleep(5000); //Wait for 5s to make sure that app flow is already doing bitscmp.
                SvcLog_OK(SVC_LOG_SHMOO_TASK, "Start calling AmbaIOUTDiag_CmdDramShmoo", 0U, 0U);
#ifdef CONFIG_SOC_CV2FS
                (void)SvcSafeStateMgr_SndDrmShmInfo(&ShmooDramParam, &ShmooMisc, sizeof(AMBA_DRAM_SHMOO_CTRL_s), sizeof(AMBA_DRAM_SHMOO_MISC_s));
#else
                AmbaDramShmoo_Start(&ShmooCB, &ShmooDramParam, &ShmooMisc);

#endif
                AmbaMisra_TouchUnused(&ShmooCB);
                RetVal = AmbaKAL_EventFlagGet(&ShmooTaskEvent,
                        SHMOO_TASK_DONE,
                        AMBA_KAL_FLAGS_ALL,
                        AMBA_KAL_FLAGS_CLEAR_NONE,
                        &ActualFlag,
                        AMBA_KAL_WAIT_FOREVER);

                if(RetVal == SVC_OK){
                    AdvancedShmoo_DumpResult();
                }
            }

        } else {
            SvcLog_NG(SVC_LOG_SHMOO_TASK, "Copy ShmooInfo from UserPref failed", 0U, 0U);
        }

    } else {
        SvcLog_NG(SVC_LOG_SHMOO_TASK, "Register SysStat SHMOO failed", 0U, 0U);
    }

    return NULL;
}

UINT32 SvcAdvancedShmooTask_Start(void)
{

    UINT32 RetVal;
    static char ShmooTaskEventName[32U] = "AdvancedShmooTaskEvent";
    static SVC_TASK_CTRL_s ShmooTaskCtrl GNU_SECTION_NOZEROINIT;
    #define SHMOOSTACKSIZE 0x3000U
    static UINT8 ShmooTaskStack[SHMOOSTACKSIZE] GNU_SECTION_NOZEROINIT;
    static char ShmooTaskName[16] = "AdvancedShmoo";

    SvcLog_OK(SVC_LOG_SHMOO_TASK, "Create Advanced Shmoo Task", 0U, 0U);
    RetVal = AmbaKAL_EventFlagCreate(&ShmooTaskEvent, ShmooTaskEventName);
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_SHMOO_TASK, "Create event flag failed %d", RetVal, 0U);
    } else {

        ShmooTaskCtrl.Priority   = SVC_ADVANCED_SHMOO_TASK_PRI;
        ShmooTaskCtrl.EntryFunc  = AdvancedShmoo_TaskEntry;
        ShmooTaskCtrl.pStackBase = ShmooTaskStack;
        ShmooTaskCtrl.StackSize  = SHMOOSTACKSIZE;
        ShmooTaskCtrl.CpuBits    = SVC_ADVANCED_SHMOO_TASK_CPU_BITS;
        ShmooTaskCtrl.EntryArg   = (UINT32) 0U;
        RetVal = SvcTask_Create(ShmooTaskName, &ShmooTaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_SHMOO_TASK, "SvcAdvancedShmooTask create failed", 0U, 0U);
        }
    }

    return RetVal;

}

void SvcAdvancedShmooTask_StatusCheck(UINT32 StatusID, ULONG ResultAddr, UINT32 *pHaveError)
{
    /*StatusID = 0 , Check App Status, assigned value to HaveError*/
    /*Statud ID = 1, Stop the flow */

    UINT32 HaveError = 0U;
    UINT32 *pAddr = NULL;

    if(StatusID == 0U){

        AdvancedShmoo_CheckCallback(&HaveError);
        /*Send the value to cortex_r*/
        *pHaveError = HaveError;

    } else {
        AmbaMisra_TypeCast(&pAddr, &ResultAddr);
        AdvancedShmoo_StopCallback(pAddr);
    }

}

static SVC_VIN_TREE_QUEUE_s ParsingQueue;   /* A queue which depth is 20 and can contain maximum 10 character */

static UINT32 AdvancedShmoo_ParsingQueuePut(const char *pModeString, UINT32 *pRdIdx)
{
    UINT32 RdIdx = *pRdIdx;
    UINT32 QueueWrIdx = ParsingQueue.WrIdx;
    const char *pSubString;
    const char RightBracketStr[2U] = "]";
    UINT8 NodeStrSize = 0U;
    UINT8 i;

    UINT32 RetVal;
    UINT32 WrapRval;

    RdIdx = RdIdx + 1U;

    pSubString = &pModeString[RdIdx];
    for (i = 0; i < SVC_VIN_TREE_QUEUE_CHAR_SIZE; i++) {
        RetVal = AmbaWrap_memcmp(&pSubString[i], RightBracketStr, sizeof(char) * 1U, &WrapRval);
        if ((SVC_OK == RetVal) && (SVC_OK == WrapRval)) {
            /* put into queue */
            RetVal = AmbaWrap_memcpy(&ParsingQueue.Word[QueueWrIdx], pSubString, sizeof(char) * NodeStrSize);
            if (SVC_OK == RetVal) {
                ParsingQueue.Word[QueueWrIdx][NodeStrSize] = '\0';
                QueueWrIdx = QueueWrIdx + 1U;

                ParsingQueue.WrIdx = QueueWrIdx;
            }
            break;
        } else {
            NodeStrSize++;
        }
    }
    RdIdx = RdIdx + NodeStrSize;

    /* Update read index */
    *pRdIdx = RdIdx;

    return SVC_VIN_TREE_QUEUE_RESULT_CONT;

}

static UINT32 AdvancedShmoo_ParsingQueueStop(const char *pModeString, UINT32 *pRdIdx)
{
    UINT32 RdIdx = *pRdIdx;
    (void) pModeString;

    RdIdx = RdIdx + 1U;

    /* Update read index */
    *pRdIdx = RdIdx;

    return SVC_VIN_TREE_QUEUE_RESULT_OK;
}


static UINT32 AdvancedShmoo_ParsingQueue(const char *pModeString, UINT32 *pRdIdx)
{
    UINT32 RdIdx = *pRdIdx;
    (void) pModeString;
    /* Do nothing */

    RdIdx = RdIdx + 1U;

    /* Update read index */
    *pRdIdx = RdIdx;

    return SVC_VIN_TREE_QUEUE_RESULT_CONT;
}

static SVC_VIN_TREE_NODE_s* AdvancedShmoo_ParsingCvMode(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan)
{

    UINT32 CvEnabled = 0U;
    const char *pCvEnabledStr = &pWord[3U];
    AmbaMisra_TouchUnused(pInputNode);
    AmbaMisra_TouchUnused(pOutputChan);
    AmbaMisra_TouchUnused(&InputChan);

    if( SvcWrap_strtoul(pCvEnabledStr, &CvEnabled)  == SVC_OK){
        gAdvShmooCtrl.CvMode = CvEnabled;
        *pRdIdx = *pRdIdx + 1U;
    } else {
        SvcLog_NG(SVC_LOG_SHMOO_TASK, "Parse CV enabled flag error", 0U, 0U);
    }

    return NULL;

}

static SVC_VIN_TREE_NODE_s* AdvancedShmoo_ParsingRecMode(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan)
{

    UINT32 RecEnabled = 0U;
    const char *pRecEnabledStr = &pWord[4U];
    AmbaMisra_TouchUnused(pInputNode);
    AmbaMisra_TouchUnused(pOutputChan);
    AmbaMisra_TouchUnused(&InputChan);

    if( SvcWrap_strtoul(pRecEnabledStr, &RecEnabled)  == SVC_OK){
        gAdvShmooCtrl.RecMode = RecEnabled;
        *pRdIdx = *pRdIdx + 1U;
    } else {
        SvcLog_NG(SVC_LOG_SHMOO_TASK, "Parse Rec enabled flag error", 0U, 0U);
    }

    return NULL;

}

static SVC_VIN_TREE_NODE_s* AdvancedShmoo_ParsingNumber(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan)
{

    UINT32 Number = 0U;
    const char *pNumStr = &pWord[4U];
    AmbaMisra_TouchUnused(pInputNode);
    AmbaMisra_TouchUnused(pOutputChan);
    AmbaMisra_TouchUnused(&InputChan);

    if( SvcWrap_strtoul(pNumStr, &Number)  == SVC_OK){
        if(InputChan == 0U){
            gAdvShmooCtrl.CvNum = Number;
        } else if(InputChan == 1U){
            gAdvShmooCtrl.RecNum = Number;
        } else{
            /**/
        }
        *pRdIdx = *pRdIdx + 1U;
    } else {
        SvcLog_NG(SVC_LOG_SHMOO_TASK, "Parse CV Num error", 0U, 0U);
    }

    return NULL;

}

static SVC_VIN_TREE_NODE_s* AdvancedShmoo_ParsingRecId(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan)
{

    static UINT32 pairCount = 0U;
    UINT32 CmpBits = 0U, Number = 0U;
    UINT32 StrIndex = 6U, StrCount =0U, StrLength;
    char NumStr[16];


    AmbaMisra_TouchUnused(pInputNode);
    AmbaMisra_TouchUnused(pOutputChan);
    AmbaMisra_TouchUnused(&InputChan);
    StrLength = (UINT32) SvcWrap_strlen(pWord);

    while(1U){

        while(((StrIndex + StrCount) < StrLength) && (pWord[StrIndex + StrCount] != '_') ){
            StrCount ++;
        }
        AmbaWrap_memcpy(&NumStr[0], &pWord[StrIndex], StrCount);
        NumStr[StrCount] = '\0';
        if( SvcWrap_strtoul(&NumStr[0], &Number)  == SVC_OK){
            CmpBits |= ( 1U << Number);
        } else {
            AmbaPrint_PrintStr5("%s", NumStr, NULL,NULL,NULL,NULL);
            SvcLog_NG("test","NG",0U,0U);
        }

        StrIndex += StrCount;

        if(StrIndex == StrLength){
            break;
        } else {
            StrIndex ++;
            StrCount = 0U;
        }
    }

    gAdvShmooCtrl.RecCmpBits[pairCount] = CmpBits;
    *pRdIdx = *pRdIdx + 1U;
    pairCount ++;
    return NULL;

}

static UINT32 AdvancedShmoo_CleanQueue(void)
{
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(&ParsingQueue, 0, sizeof(ParsingQueue));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SHMOO_TASK, "AdvancedShmoo_CleanQueue() err. AmbaWrap_memset() failed with %d", RetVal, 0U);
    }

    return RetVal;
}

static UINT32 AdvancedShmoo_ModeInfoParse(const char* pModeString)
{

    #define SVC_VIN_TREE_PARSING_FORMAT_NUM        (4U)
    #define SVC_VIN_TREE_PARSING_SUPPORT_WORD_NUM  (4U)
    static const SVC_VIN_TREE_QUEUE_PROC_s SvcAdvancedShmooQueueProc[SVC_VIN_TREE_PARSING_FORMAT_NUM] = {
        { "$",    1,   AdvancedShmoo_ParsingQueue        },  /* The key word to start generate queue */
        { "[",    1,   AdvancedShmoo_ParsingQueuePut     },  /* The key word to put the next character into queue */
        { "]",    1,   AdvancedShmoo_ParsingQueue        },  /* The key word to stop putting */
        { "#",    1,   AdvancedShmoo_ParsingQueueStop    },  /* The key word to end generate queue */
    };

    static const SVC_VIN_TREE_PARSING_PROC_s SvcAdvancedShmooParsingProc[SVC_VIN_TREE_PARSING_SUPPORT_WORD_NUM] = {
        [0] = { "CV_",  3,   AdvancedShmoo_ParsingCvMode     },
        [1] = { "REC_", 4,   AdvancedShmoo_ParsingRecMode    },
        { "NUM_", 4,   AdvancedShmoo_ParsingNumber     },
        {"RECID_", 6,  AdvancedShmoo_ParsingRecId}
    };

    const char *pSubString, *pSubQueueStr;
    UINT32 RdIdx = 0;
    UINT32 WrapRval;
    UINT32 RetVal, PrevType = 0U;

    UINT32 i;
    UINT32 QueueResult, TreeCreateDone = 0U;

    while (TreeCreateDone == 0U) {
        const char ExclamationMarkStr[2U] = "!";
        QueueResult = SVC_VIN_TREE_QUEUE_RESULT_CONT;

        pSubString = &pModeString[RdIdx];
        /* Check whether parsing end */
        RetVal = AmbaWrap_memcmp(pSubString, ExclamationMarkStr, sizeof(char) * 1U, &WrapRval);
        if (SVC_OK == WrapRval) {
            TreeCreateDone = 1;
        }

        if (TreeCreateDone == 0U) {
            /* Generate queue */
            while (QueueResult != SVC_VIN_TREE_QUEUE_RESULT_OK) {
                /* Each parsing step started address */
                pSubString = &pModeString[RdIdx];
                for (i = 0; i < SVC_VIN_TREE_PARSING_FORMAT_NUM; i++) {
                    RetVal = AmbaWrap_memcmp(pSubString, SvcAdvancedShmooQueueProc[i].Word, sizeof(char) * SvcAdvancedShmooQueueProc[i].WordNum, &WrapRval);
                    if (SVC_OK == WrapRval) {
                        QueueResult = SvcAdvancedShmooQueueProc[i].pFuncEntry(pModeString, &RdIdx);
                        break;
                    }
                }
            }

            /* Compare the key word in queue */
            while (ParsingQueue.WrIdx != ParsingQueue.RdIdx) {
                UINT32 QueueRdIdx = ParsingQueue.RdIdx;
                pSubQueueStr = ParsingQueue.Word[QueueRdIdx];
                for (i = 0; i < SVC_VIN_TREE_PARSING_SUPPORT_WORD_NUM; i++) {
                    /* If key word matching */
                    RetVal = AmbaWrap_memcmp(pSubQueueStr, SvcAdvancedShmooParsingProc[i].Word, sizeof(char) * SvcAdvancedShmooParsingProc[i].WordNum, &WrapRval);
                    if (SVC_OK == WrapRval) {
                        (void) SvcAdvancedShmooParsingProc[i].pFuncEntry(ParsingQueue.Word[QueueRdIdx], &ParsingQueue.RdIdx, NULL, PrevType, NULL);
                        PrevType = i;
                        break;
                    }
                }
            }
        }

        RetVal = AdvancedShmoo_CleanQueue();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_SHMOO_TASK, "SvcAdvancedShmoo_Create() Clean Queue err (%d)", RetVal, 0U);
        }
    }

    return SVC_OK;

}

static UINT32 AdvancedShmoo_SetInfo(UINT32 SetFlag, UINT32 ArgCount, char * const *pArgVector)
{

    SVC_USER_PREF_s *pSvcUserPref;

    UINT32 RetVal = SVC_OK, i;

    RetVal = SvcUserPref_Get(&pSvcUserPref);

    if(RetVal == SVC_OK){
        if((SetFlag == MODEINFO) && (ArgCount == 6U)){
            UINT32 RecMode, CvMode, FileWrite, FormatID;
            UINT8 Mode = 0U;
            // RetVal |= SvcWrap_strtoul(pArgVector[2U], &RecMode);
            // RetVal |= SvcWrap_strtoul(pArgVector[3U], &CvMode);
            (void) AdvancedShmoo_ModeInfoParse(pArgVector[2U]);
            (void) AdvancedShmoo_ModeInfoParse(pArgVector[3U]);
            RecMode = gAdvShmooCtrl.RecMode;
            CvMode = gAdvShmooCtrl.CvMode;
            RetVal |= SvcWrap_strtoul(pArgVector[4U], &FileWrite);
            RetVal |= SvcWrap_strtoul(pArgVector[5U], &FormatID);

            if(RetVal == SVC_OK){
                Mode = (RecMode < 1U) ? Mode: (Mode | 1U);
                AmbaMisra_TouchUnused(&CvMode);
            #ifndef CONFIG_SOC_H32
                Mode = (CvMode < 1U)  ? Mode: (Mode | (1U << 1U));
            #endif
                pSvcUserPref->ShmooInfo.ShmooMode = Mode;
                pSvcUserPref->ShmooInfo.FileWrite = (UINT8) FileWrite;
                pSvcUserPref->FormatId = (UINT8) FormatID;
                pSvcUserPref->ShmooInfo.RecNum = gAdvShmooCtrl.RecNum;
                pSvcUserPref->ShmooInfo.CvNum = gAdvShmooCtrl.CvNum;
                for(i=0; i<gAdvShmooCtrl.RecNum; i++){
                    pSvcUserPref->ShmooInfo.RecCmpBits[i] = gAdvShmooCtrl.RecCmpBits[i];
                }
            }
        } else if ((SetFlag == ITEMINFO) && (ArgCount == 21U)){
            UINT8 ItemTmpBuffer[19] = {0};
            UINT32 u32v;
            for(i=0U; i < 19U; i++){
                RetVal |= SvcWrap_strtoul(pArgVector[i+2U], &u32v);
                if(RetVal == SVC_OK){
                    ItemTmpBuffer[i] = (UINT8) u32v & 0xFFU;
                } else {
                    SvcLog_NG(SVC_LOG_SHMOO_TASK, "Invalid Shmoo Item Value", 0U, 0U);
                    break;
                }
            }
            if(RetVal == SVC_OK){
                RetVal = AmbaWrap_memcpy(&(pSvcUserPref->ShmooInfo.ShmooItems), &ItemTmpBuffer[0], sizeof(AMBA_DRAM_SHMOO_CTRL_s));
                if(RetVal != SVC_OK){
                    SvcLog_NG(SVC_LOG_SHMOO_TASK, "Copy Shmoo item info to preference error", 0U, 0U);
                }
            }

        } else if ((SetFlag == MISCINFO) && (ArgCount == 5U)){
            UINT32 TempMisc[5]={0};
            for(i=0U; i<3U; i++){
                RetVal |= SvcWrap_strtoul(pArgVector[i+2U], &TempMisc[i]);
                if(RetVal != SVC_OK){
                    SvcLog_NG(__func__, "Invalid Shmoo MisC Value", 0U, 0U);
                    break;
                }
            }

            if(RetVal == SVC_OK){
                TempMisc[3] = SVC_ADVANCED_SHMOO_TASK_PRI;
                TempMisc[4] = AMBA_USER_PARTITION_RESERVED0;
                RetVal = AmbaWrap_memcpy(&(pSvcUserPref->ShmooInfo.ShmooMisc), &TempMisc[0], sizeof(AMBA_DRAM_SHMOO_MISC_s));
                if(RetVal != SVC_OK){
                    SvcLog_NG(SVC_LOG_SHMOO_TASK, "Copy Misc info to preference error", 0U, 0U);
                }
            }

        } else if ((SetFlag == SHMOOSTART) && (ArgCount == 2U)){
            pSvcUserPref->ShmooInfo.ShmooTask = 1U;
        } else {
            SvcLog_NG(SVC_LOG_SHMOO_TASK, "Set [%u] Info, but Argument error:[%u]",SetFlag, ArgCount-2U);
        }

        if(RetVal == SVC_OK){
            ULONG   PrefBufAddr;
            UINT32  PrefBufSize;
            SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
            if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                SvcLog_NG(SVC_LOG_SHMOO_TASK, "SvcPref_Save failed!!", 0U, 0U);
            }
        }


    }

    return RetVal;


}


static void CmdAdvancedShmooUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("svc_advshmoo commands:\n");
    PrintFunc(" dump_shmoo_info:                dump advanced shmoo info \n");
    PrintFunc(" rec_report:                     dump Record bitscmp report \n");
    PrintFunc(" cv_report:                      dump Cv bitscmp report \n");

}

static void AdvancedShmoo_InfoDump(void)
{
    SVC_USER_PREF_s *pSvcUserPref;
    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        AmbaPrint_PrintUInt5("",0U,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("FormatId           :   %u",pSvcUserPref->FormatId,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("ShmooMode          :   %u",pSvcUserPref->ShmooInfo.ShmooMode,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("RecNum             :   %u",pSvcUserPref->ShmooInfo.RecNum,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("CvNum              :   %u",pSvcUserPref->ShmooInfo.CvNum,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("RecCmpBits[0]      :   %u",pSvcUserPref->ShmooInfo.RecCmpBits[0],0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("RecCmpBits[1]      :   %u",pSvcUserPref->ShmooInfo.RecCmpBits[1],0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("RecCmpBits[2]      :   %u",pSvcUserPref->ShmooInfo.RecCmpBits[2],0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("RecCmpBits[3]      :   %u",pSvcUserPref->ShmooInfo.RecCmpBits[3],0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("pad_dds            :   %u",pSvcUserPref->ShmooInfo.ShmooItems.pad_dds,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("pad_pdds           :   %u",pSvcUserPref->ShmooInfo.ShmooItems.pad_pdds,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("pad_ca_dds         :   %u",pSvcUserPref->ShmooInfo.ShmooItems.pad_ca_dds,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("pad_ca_pdds        :   %u",pSvcUserPref->ShmooInfo.ShmooItems.pad_ca_pdds,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("pad_term           :   %u",pSvcUserPref->ShmooInfo.ShmooItems.pad_term,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("pad_vref           :   %u",pSvcUserPref->ShmooInfo.ShmooItems.pad_vref,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("rddly              :   %u",pSvcUserPref->ShmooInfo.ShmooItems.rddly,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("wrdly              :   %u",pSvcUserPref->ShmooInfo.ShmooItems.wrdly,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("dll0               :   %u",pSvcUserPref->ShmooInfo.ShmooItems.dll0,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("dll1               :   %u",pSvcUserPref->ShmooInfo.ShmooItems.dll1,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("dll2               :   %u",pSvcUserPref->ShmooInfo.ShmooItems.dll2,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("ddr4_dds           :   %u",pSvcUserPref->ShmooInfo.ShmooItems.ddr4_dds,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("ddr4_vref          :   %u",pSvcUserPref->ShmooInfo.ShmooItems.ddr4_vref,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("ddr4_odt           :   %u",pSvcUserPref->ShmooInfo.ShmooItems.ddr4_odt,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("lpddr4_pdds        :   %u",pSvcUserPref->ShmooInfo.ShmooItems.lpddr4_pdds,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("lpddr4_ca_vref     :   %u",pSvcUserPref->ShmooInfo.ShmooItems.lpddr4_ca_vref,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("lpddr4_dq_vref     :   %u",pSvcUserPref->ShmooInfo.ShmooItems.lpddr4_dq_vref,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("lpddr4_ca_odt      :   %u",pSvcUserPref->ShmooInfo.ShmooItems.lpddr4_ca_odt,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("lpddr4_dq_odt      :   %u",pSvcUserPref->ShmooInfo.ShmooItems.lpddr4_dq_odt,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("Duration           :   %u",pSvcUserPref->ShmooInfo.ShmooMisc.ShmooDuration,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("StoreDevice        :   %u",pSvcUserPref->ShmooInfo.ShmooMisc.ShmooDevice,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("CoreMask           :   %u",pSvcUserPref->ShmooInfo.ShmooMisc.ShmooCoreMask,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("Priority           :   %u",pSvcUserPref->ShmooInfo.ShmooMisc.ShmooPriority,0U,0U,0U,0U);
        AmbaPrint_PrintUInt5("PartitionID        :   %u",pSvcUserPref->ShmooInfo.ShmooMisc.ShmooPartitionID,0U,0U,0U,0U);

    }
}

static void CmdAdvShmooEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{

    UINT32 RetVal;

    if (0 == SvcWrap_strcmp("set_mode", pArgVector[1U])){
        RetVal = AdvancedShmoo_SetInfo(MODEINFO, ArgCount, pArgVector);
        if(RetVal != SVC_OK){
            SvcLog_NG(SVC_LOG_SHMOO_TASK, "set mode failed", 0U, 0U);
        }

    } else if (0 == SvcWrap_strcmp("set_shmoo_item", pArgVector[1U])){
        RetVal = AdvancedShmoo_SetInfo(ITEMINFO, ArgCount, pArgVector);
        if(RetVal != SVC_OK){
            SvcLog_NG(SVC_LOG_SHMOO_TASK, "set shmoo item failed", 0U, 0U);
        }

    } else if (0 == SvcWrap_strcmp("set_misc_info", pArgVector[1U])){
        RetVal = AdvancedShmoo_SetInfo(MISCINFO, ArgCount, pArgVector);
        if(RetVal != SVC_OK){
            SvcLog_NG(SVC_LOG_SHMOO_TASK, "set misc info failed", 0U, 0U);
        }
    } else if (0 == SvcWrap_strcmp("shmoo_start", pArgVector[1U])){
        RetVal = AdvancedShmoo_SetInfo(SHMOOSTART, ArgCount, pArgVector);
        if(RetVal == SVC_OK){
            SvcLog_DBG(SVC_LOG_SHMOO_TASK, "Rebooting......", 0U, 0U);
            if (SYS_ERR_NONE != AmbaSYS_Reboot()) {
                SvcLog_NG(SVC_LOG_SHMOO_TASK, "AmbaSYS_Reboot() failed!!", 0U, 0U);
            }
        }
    } else if (0 == SvcWrap_strcmp("dump_shmoo_info", pArgVector[1U])){
            AdvancedShmoo_InfoDump();
    } else if(0 == SvcWrap_strcmp("cv_report", pArgVector[1U])){
#ifdef CONFIG_BUILD_CV
            if((gShmooMode & SHMOO_TASK_APP_START_CV) > 0U){
                UINT32 haveError;
                SvcShmooCvTask_ShmooCompareReport(&haveError, 1U);
            }
#endif
    } else if(0 == SvcWrap_strcmp("rec_report", pArgVector[1U])){
            if((gShmooMode & SHMOO_TASK_APP_START_ENCODE) > 0U){
                UINT32 haveError;
                SvcShmooRecTask_DataCmpCheck(&haveError, 0U, 1U);
            }
    }  else {
        CmdAdvancedShmooUsage(PrintFunc);
    }


}


void SvcCmdAdvShmoo_Install(void)
{
    AMBA_SHELL_COMMAND_s  SvcCmdAdvShmoo;

    UINT32  Rval;

    SvcCmdAdvShmoo.pName    = "svc_advshmoo";
    SvcCmdAdvShmoo.MainFunc = CmdAdvShmooEntry;
    SvcCmdAdvShmoo.pNext    = NULL;

    Rval = SvcCmd_CommandRegister(&SvcCmdAdvShmoo);
    if (SHELL_ERR_SUCCESS != Rval) {
        SvcLog_NG(SVC_LOG_SHMOO_TASK, "## fail to install svc advanced shmoo command", 0U, 0U);
    }
}
