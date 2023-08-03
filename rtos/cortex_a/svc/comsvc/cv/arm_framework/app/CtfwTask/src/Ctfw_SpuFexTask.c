#include "Ctfw_TaskUtil.h"
#include "Ctfw_SpuFexTask.h"
#include "cvapi_svccvalgo_ambaspufex.h"
#include "AmbaMisraFix.h"

typedef struct {
    SVC_CV_ALGO_HANDLE_s ScaHandle;
    AMBA_STEREO_FD_HANDLE_s StereoHandle;

    CTFW_SPU_FEX_TASK_CONFIG_t SpuFexTaskConfig;

    UINT32 SpuFexCfgInit;
} SPU_FEX_TASK_WORK_BUFF_s;

typedef struct {
    flexidag_memblk_t TaskWorkBuff;
    flexidag_memblk_t ScaAlgoBuf;
} SPU_FEX_PRIVATE_s;

#define ALLOC_SPU_FEX_SCA_REQ_BUFF_SIZE  (2UL<<20UL)

//AmbaCV_FlexidagOpen = 0x1163704 0x1d48f4 0x0 0x0 0x0
#define ALLOC_SPU_FEX_OUT_SPU_BUFF_SIZE  (18UL<<20UL)
#define ALLOC_SPU_FEX_OUT_FEX_BUFF_SIZE  (2UL<<20UL)
#define ALLOC_SPU_FEX_OUT_BUFF_SIZE      (ALLOC_SPU_FEX_OUT_SPU_BUFF_SIZE+ALLOC_SPU_FEX_OUT_FEX_BUFF_SIZE)

#define CTFW_TASK_NAME "SpuFex"

static UINT32 CftwSpuFex_QuerySize(UINT32 *pTaskStackSize, UINT32 *pAlgBuffMapSize, UINT32 *pAlgWorkSize, UINT32 *pAlgOutputSize)
{
    UINT32 RetVal = ARM_OK;
    UINT32 AlgBuffMapSize = 0U;
    UINT32 AlgWorkSize = 0U;
    UINT32 AlgOutputSize = 0U;

    *pTaskStackSize = 16384U;
    *pAlgBuffMapSize = 0U;
    *pAlgWorkSize = 0U;
    *pAlgOutputSize = 0U;

    // AlgBuffMapSize
    {
        AlgBuffMapSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(SPU_FEX_PRIVATE_s));
    }

    // AlgWorkSize
    {
        {
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(SPU_FEX_TASK_WORK_BUFF_s));
        }
        {
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)ALLOC_SPU_FEX_SCA_REQ_BUFF_SIZE);
        }
    }

    // AlgOutputSize
    {
        AlgOutputSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(CTFW_SPU_FEX_TASK_OUT_t));
        AlgOutputSize += CTFW_CV_MEM_ALIGN((UINT32)ALLOC_SPU_FEX_OUT_BUFF_SIZE);
    }

    *pAlgBuffMapSize = AlgBuffMapSize;
    *pAlgWorkSize = AlgWorkSize;
    *pAlgOutputSize = AlgOutputSize;

    return RetVal;
}

static UINT32 CftwSpuFex_SetName(char TaskName[])
{
    AmbaUtility_StringAppend(TaskName, MAX_CTFW_NAME_LEN, CTFW_TASK_NAME);
    return ARM_OK;
}

static UINT32 CftwSpuFex_Init(const void *pAlgConfig, flexidag_memblk_t FlexiBinBlk[],
                              UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap, CtfwTaskConfig_t *pTaskSetting)
{
    UINT32 RetVal = ARM_OK;
    SPU_FEX_PRIVATE_s *pPrivatePtr;

    AmbaMisra_TouchUnused(FlexiBinBlk);

    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);

    // Task env setting
    {
        pTaskSetting->ProcInterval = 1U;
        pTaskSetting->TaskPriority = 50U;
        pTaskSetting->TaskCoreSel = 0U;
    }

    // Partitioning memory pool
    {
        {
            RetVal |= ArmMemPool_Allocate(MemPoolID, (UINT32)sizeof(SPU_FEX_TASK_WORK_BUFF_s), &pPrivatePtr->TaskWorkBuff);
        }
    }

    // Algo private init
    if ( RetVal == ARM_OK ) {
        SPU_FEX_TASK_WORK_BUFF_s *pTaskWorkPtr;
        SVC_CV_ALGO_HANDLE_s *pScaHandle;
        const AMBA_STEREO_FD_HANDLE_s *pStereoHdlr;
        const CTFW_SPU_FEX_TASK_CONFIG_t *pSpuFexTaskConfig;

        SVC_CV_ALGO_QUERY_CFG_s ScaQueryCfg = {NULL, 1U, NULL};
        SVC_CV_ALGO_CREATE_CFG_s ScaCreateCfg = {
            .NumFD = 0,
            .pBin = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
            .OutputNum = 0,
            .OutputSz = {0, 0, 0, 0, 0, 0, 0, 0},
            .pExtCreateCfg = NULL
        };

        flexidag_memblk_t FlexiBin;

        flexidag_memblk_t *pAlgoBlk;

        AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);
        RetVal |= AmbaWrap_memset(pTaskWorkPtr, 0, sizeof(SPU_FEX_TASK_WORK_BUFF_s));

        pAlgoBlk = &pPrivatePtr->ScaAlgoBuf;

        pScaHandle = &pTaskWorkPtr->ScaHandle;
        pStereoHdlr = &pTaskWorkPtr->StereoHandle;

        AmbaMisra_TypeCast(&pSpuFexTaskConfig, &pAlgConfig);
        RetVal |= AmbaWrap_memcpy(&pTaskWorkPtr->SpuFexTaskConfig, pSpuFexTaskConfig,
                                  sizeof(pTaskWorkPtr->SpuFexTaskConfig));

        //pScaHandle->pAlgoObj = &AmbaSpuFexAlgoObj;
        //        pHandle->ScaHandle.pAlgoObj = pScaHandle->pAlgoObj;

        //ArmLog_STR(CTFW_TASK_NAME, "## call sca.query", Rval);
        //sca_query
        ScaQueryCfg.pAlgoObj = &AmbaSpuFexAlgoObj;
        RetVal |= AmbaSpuFexAlgoObj.Query(pScaHandle, &ScaQueryCfg);
        ArmLog_DBG(CTFW_TASK_NAME, "## ScaQueryCfg.TotalReqBufSz=%d, Rval=%d", ScaQueryCfg.TotalReqBufSz, RetVal);
        RetVal |= ArmMemPool_Allocate(MemPoolID, ScaQueryCfg.TotalReqBufSz, pAlgoBlk);

        AmbaMisra_TypeCast(&pScaHandle->pAlgoCtrl, &pStereoHdlr);

        if (RetVal == ARM_OK) {

            //ArmLog_STR(CTFW_TASK_NAME, "## call create sca.create"); fflush(stdout);
            RetVal |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memset", AmbaWrap_memset(&ScaCreateCfg, 0, sizeof(ScaCreateCfg)));
            RetVal |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memset", AmbaWrap_memset(&FlexiBin, 0, sizeof(FlexiBin)));
            pScaHandle->pAlgoCtrl = NULL;
            ScaCreateCfg.pAlgoBuf = pAlgoBlk;
            ScaCreateCfg.pBin[0U] = &FlexiBin;
            RetVal |= AmbaSpuFexAlgoObj.Create(pScaHandle, &ScaCreateCfg);
        }
    }

    return RetVal;
}

static void Config_AmbaSpuFex(SVC_CV_ALGO_HANDLE_s *pScaHandle,
                              const idsp_pyramid_t *pPyramid, const CTFW_SPU_FEX_TASK_CONFIG_t *pSpuFexTaskConfig)
{
    UINT32 Idx;
    AMBA_CV_SPUFEX_CFG_s SpuFexCfg;
    UINT32 RetVal = ARM_OK;

    //set pyramid info
    SpuFexCfg.PyramidInfo = *pPyramid;

    //set spu default config
    for(Idx = 0U; Idx < MAX_HALF_OCTAVES; Idx++) {
        SpuFexCfg.SpuScaleCfg[Idx] = pSpuFexTaskConfig->SpuScaleCfg[Idx];
    }

    //set fex default config
    for(Idx = 0U; Idx < MAX_HALF_OCTAVES; Idx++) {
        SpuFexCfg.FexScaleCfg[Idx] = pSpuFexTaskConfig->FexScaleCfg[Idx];
    }

    {
        SVC_CV_ALGO_CTRL_CFG_s CtrlCfg;
        UINT32 SpuDisplayMode = pSpuFexTaskConfig->SpuDisplayMode;
        const UINT32 *pSpuDisplayMode = &SpuDisplayMode;
        const AMBA_CV_SPUFEX_CFG_s *pSpuFexCfg = &SpuFexCfg;

        CtrlCfg.pExtCtrlCfg = NULL;

        CtrlCfg.CtrlType = AMBA_SCA_SPUFEX_CTRL_CFG;
        AmbaMisra_TypeCast(&CtrlCfg.pCtrlParam, &pSpuFexCfg);
        RetVal |= AmbaSpuFexAlgoObj.Control(pScaHandle, &CtrlCfg);

        CtrlCfg.CtrlType = AMBA_SCA_SPUFEX_CTRL_DISPLAY_MODE;
        AmbaMisra_TypeCast(&CtrlCfg.pCtrlParam, &pSpuDisplayMode);
        RetVal |= AmbaSpuFexAlgoObj.Control(pScaHandle, &CtrlCfg);
    }

    (void)RetVal;

    return;
}

static UINT32 AssignOutputBuff(const flexidag_memblk_t *pAlgOutputData,
                               flexidag_memblk_t *pSpuOut, flexidag_memblk_t *pFexOut)
{
    UINT32 RetVal = ARM_OK;
    CTFW_SPU_FEX_TASK_OUT_t *pTaskout;
    ULONG BuffShiftSize;

    //          Buffer layout
    // -----------------------
    // CTFW_SPU_FEX_TASK_OUT_t Info
    // .
    // .
    // -----------------------
    // SPU
    // .
    // .
    // Fex
    // -----------------------
    // .
    // .

    AmbaMisra_TypeCast(&pTaskout, &pAlgOutputData->pBuffer);

    // Assign Spu output
    BuffShiftSize = CTFW_CV_MEM_ALIGN((UINT32)sizeof(CTFW_SPU_FEX_TASK_OUT_t));
    RetVal |= Ctfw_ShiftFlexiMemblkBuff(pAlgOutputData, BuffShiftSize, &pTaskout->SpuOut);
    pTaskout->SpuOut.buffer_size = (UINT32)ALLOC_SPU_FEX_OUT_SPU_BUFF_SIZE;
    *pSpuOut = pTaskout->SpuOut;

    // Assign Fex output
    BuffShiftSize += CTFW_CV_MEM_ALIGN(pTaskout->SpuOut.buffer_size);
    RetVal |= Ctfw_ShiftFlexiMemblkBuff(pAlgOutputData, BuffShiftSize, &pTaskout->FexOut);
    pTaskout->FexOut.buffer_size = (UINT32)ALLOC_SPU_FEX_OUT_FEX_BUFF_SIZE;
    *pFexOut = pTaskout->FexOut;

    return RetVal;
}

#if 0
static void EchoFexResult(const AMBA_CV_FEX_DATA_s *pOut, const UINT32 ScaleId)
{
    ULONG ULFex, KeyPointCntAddr, KeyPointAddr;
    const UINT8 *pKeyPointCnt;
    const AMBA_CV_FEX_COORD_s *pKeyPoints;
    UINT32 i, j;
    const AMBA_CV_FEX_DATA_s *pFex;

    AmbaMisra_TypeCast(&pFex, &pOut);
    AmbaMisra_TypeCast(&ULFex, &pFex);

    if (pFex->PrimaryList[ScaleId].Enable != 0U) {
        KeyPointCntAddr = ULFex + pFex->PrimaryList[ScaleId].KeypointsCountOffset;
        KeyPointAddr = ULFex + pFex->PrimaryList[ScaleId].KeypointsOffset;
        AmbaMisra_TypeCast(&pKeyPointCnt, &KeyPointCntAddr);
        AmbaMisra_TypeCast(&pKeyPoints, &KeyPointAddr);

        //for (i = 0U; i < 64U; i++) {
        for (i = 32U; i < 33U; i++) {
            ArmLog_DBG(CTFW_TASK_NAME, "PrimaryList Bucket[%d], KeyPtNum = %d", i, pKeyPointCnt[i]);
            for (j = 0U; j < pKeyPointCnt[i]; j++) {
                ArmLog_DBG(CTFW_TASK_NAME, "KeyPoint[%d]:", j, 0U);
                ArmLog_DBG(CTFW_TASK_NAME, "%d, %d", pKeyPoints[(i * CV_FEX_MAX_KEYPOINTS) + j].X,
                           pKeyPoints[(i * CV_FEX_MAX_KEYPOINTS) + j].Y);
            }
        }
    } else {
        ArmLog_DBG(CTFW_TASK_NAME, "FEX PrimaryList[%d] is Disabled", ScaleId, 0U);
    }
    if (pFex->SecondaryList[ScaleId].Enable != 0U) {
        KeyPointCntAddr = ULFex + pFex->SecondaryList[ScaleId].KeypointsCountOffset;
        KeyPointAddr = ULFex + pFex->SecondaryList[ScaleId].KeypointsOffset;
        AmbaMisra_TypeCast(&pKeyPointCnt, &KeyPointCntAddr);
        AmbaMisra_TypeCast(&pKeyPoints, &KeyPointAddr);
        //for (i = 0U; i < 64U; i++) {
        for (i = 32U; i < 33U; i++) {
            ArmLog_DBG(CTFW_TASK_NAME, "SecondaryList Bucket[%d], KeyPtNum = %d", i, pKeyPointCnt[i]);
            for (j = 0U; j < pKeyPointCnt[i]; j++) {
                ArmLog_DBG(CTFW_TASK_NAME, "KeyPoint[%d]:", j, 0U);
                ArmLog_DBG(CTFW_TASK_NAME, "%d, %d", pKeyPoints[(i * CV_FEX_MAX_KEYPOINTS) + j].X,
                           pKeyPoints[(i * CV_FEX_MAX_KEYPOINTS) + j].Y);
            }
        }
    } else {
        ArmLog_DBG(CTFW_TASK_NAME, "FEX SecondaryList[%d] is Disabled", ScaleId, 0U);
    }
}
#endif

static UINT32 CftwSpuFex_Process(const CtfwFrameMetadata_t *pMetadata, const flexidag_memblk_t* const pAlgInputData[],
                                 const flexidag_memblk_t* const pAlgBuffMap, flexidag_memblk_t* const pAlgOutputData)
{
    UINT32 RetVal = ARM_OK;

    SPU_FEX_TASK_WORK_BUFF_s *pTaskWorkPtr;
    const SPU_FEX_PRIVATE_s *pPrivatePtr;

    SVC_CV_ALGO_HANDLE_s *pScaHandle;
    AMBA_CV_FLEXIDAG_IO_s ScaInBuf;
    AMBA_CV_FLEXIDAG_IO_s ScaOutBuf;
    SVC_CV_ALGO_FEED_CFG_s ScaFeedCfg;

    flexidag_memblk_t SpuOut;
    flexidag_memblk_t FexOut;

    (void) pMetadata;
    AmbaMisra_TouchUnused(pAlgOutputData);

    // Orgnize input
    // payload of input buffer is in memio_source_recv_picinfo_t;

    // Orgnize work buffer
    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);
    AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    // Orgnize output
    RetVal |= AssignOutputBuff(pAlgOutputData, &SpuOut, &FexOut);
    if (RetVal != ARM_OK) {
        ArmLog_ERR(CTFW_TASK_NAME, "AssignOutputBuff Fail", 0U, 0U);
    }
    // Algo private routine
    // ####################################

    pScaHandle = &pTaskWorkPtr->ScaHandle;

    if ( pTaskWorkPtr->SpuFexCfgInit == 0U ) {
        const memio_source_recv_picinfo_t *pSrcPicInfo;
        AmbaMisra_TypeCast(&pSrcPicInfo, &pAlgInputData[0U]->pBuffer);
        Config_AmbaSpuFex(pScaHandle,
                          &pSrcPicInfo->pic_info.pyramid, &pTaskWorkPtr->SpuFexTaskConfig);
        pTaskWorkPtr->SpuFexCfgInit = 1U;
    }

    ScaFeedCfg.pIn = &ScaInBuf;
    ScaFeedCfg.pIn->num_of_buf = 1U;
    ScaFeedCfg.pIn->buf[0U] = *(pAlgInputData[0U]);

    ScaFeedCfg.pOut = &ScaOutBuf;
    ScaFeedCfg.pOut->num_of_buf = 2U;
    ScaFeedCfg.pOut->buf[0U] = SpuOut;
    ScaFeedCfg.pOut->buf[1U] = FexOut;

    RetVal |= pScaHandle->pAlgoObj->Feed(pScaHandle, &ScaFeedCfg);

#if 0
    {
        AMBA_CV_FEX_BUF_s *pFexOut = &FexOut;
        UINT32 ScaleID = 2U;
        if (1)
            EchoFexResult(pFexOut->pBuffer, ScaleID);
    }
#endif

    return RetVal;
}

static UINT32 CftwSpuFex_DeInit(UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap)
{
    UINT32 RetVal = ARM_OK;
    SPU_FEX_PRIVATE_s *pPrivatePtr;
    SPU_FEX_TASK_WORK_BUFF_s *pTaskWorkPtr;

    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);
    AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    {
        SVC_CV_ALGO_HANDLE_s *pScaHandle;
        SVC_CV_ALGO_DELETE_CFG_s DeleteCfg;

        pScaHandle = &pTaskWorkPtr->ScaHandle;

        RetVal |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memset", AmbaWrap_memset(&DeleteCfg, 0, sizeof(DeleteCfg)));
        // AlgoBufMemblk get free inside?
        RetVal |= AmbaSpuFexAlgoObj.Delete(pScaHandle, &DeleteCfg);
    }

    RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->ScaAlgoBuf);
    RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->TaskWorkBuff);

    return RetVal;
}

CtfwTaskObj_t gCtfwSpuFexTaskObj = {
    .pSetName = CftwSpuFex_SetName,
    .pQuerySize = CftwSpuFex_QuerySize,
    .pInit = CftwSpuFex_Init,
    .pProcess = CftwSpuFex_Process,
    .pDeInit = CftwSpuFex_DeInit,
};

