/**
*  @file FileFeeder.c
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
*   @details The FileFeeder example (The input data comes from file)
*
*/

#include "FileFeeder.h"

#define ARM_LOG_FILE_FEEDER         "FileFeeder"

#define MAX_YUV_SIZE                (0x870000U)   // 8MB for 4096 x 2160
#define EVENT_FREE_INPUT            (0x00000001)

#define DEFAULT_FILE_FEEDER_TIMEOUT (5000)  // timout = 5000 ms

static ArmEventFlag_t   g_FileFeederEventFlag;


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: InitMemblk
 *
 *  @Description:: Initial the content of flexidag_memblk_t
 *
 *  @Input      ::
 *    pMemblk:     Pointer to flexidag_memblk_t
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static inline void InitMemblk(flexidag_memblk_t *pMemblk)
{
    pMemblk->pBuffer = NULL;
    pMemblk->buffer_daddr     = 0U;
    pMemblk->buffer_cacheable = 0U;
    pMemblk->buffer_size      = 0U;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FreeInputCallback
 *
 *  @Description:: Callback for free input raw
 *
 *  @Input      ::
 *    Event:       The callback event
 *    pEventData:  Pointer to SVC_CV_ALGO_OUTPUT_s
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      OK or NG
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FreeInputCallback(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval;

    if (Event == CALLBACK_EVENT_FREE_INPUT) {
        Rval = ArmEventFlag_Set(&g_FileFeederEventFlag, EVENT_FREE_INPUT);
        if (Rval != ARM_OK) {
            ArmLog_ERR(ARM_LOG_FILE_FEEDER, "## ArmEventFlag_Set fail (EVENT_FREE_INPUT)", 0U, 0U);
        }
    }

    (void) pEventData;

    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CreateResource
 *
 *  @Description:: Create resource - eventflag
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 CreateResource(void)
{
    UINT32 Rval = ARM_OK;
    static UINT8 IsCreated = 0U;
    static char EventFlagName[32] = "FileFeederEventFlag";

    /* Create eventflag */
    if (IsCreated == 0U) {
        Rval = ArmEventFlag_Create(&g_FileFeederEventFlag, EventFlagName);
        if (Rval != ARM_OK) {
            ArmLog_ERR(ARM_LOG_FILE_FEEDER, "## ArmEventFlag_Create fail", 0U, 0U);
        } else {
            IsCreated = 1U;
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FileFeeder_Start
 *
 *  @Description:: Load binary from file
 *
 *  @Input      ::
 *    FeederCfg:   The FileFeeder Config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32  FileFeeder_Start(const FILE_FEEDER_CFG_s *pFeederCfg)
{
    UINT32 Rval = ARM_OK;
    flexidag_memblk_t RawDataBuf[FLEXIDAG_MAX_OUTPUTS];

    for (UINT32 i = 0U; i < (UINT32) FLEXIDAG_MAX_OUTPUTS; i++) {   // init RawDataBuf[]
        InitMemblk(&RawDataBuf[i]);
    }

    /* 1. Sanity check for parameters */
    if (pFeederCfg == NULL) {
        ArmLog_ERR(ARM_LOG_FILE_FEEDER, "## FileFeeder_Start fail (pFeederCfg is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pFeederCfg->Num > (UINT32) FLEXIDAG_MAX_OUTPUTS) {
            ArmLog_ERR(ARM_LOG_FILE_FEEDER, "## FileFeeder_Start fail (invalid input num %u)", pFeederCfg->Num, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Create eventflag */
    if (Rval == ARM_OK) {
        Rval = CreateResource();
    }

    /* 3. Load data from file */
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        for (UINT32 i = 0U; i < pFeederCfg->Num; i ++) {
            UINT32 FileSize, ReadSize;
            const char *pFilename = &(pFeederCfg->Filename[i][0]);
            flexidag_memblk_t *pRawDataBuf = &(RawDataBuf[i]);

            /* 3-1. Query file size */
            Rval = ArmFIO_GetSize(pFilename, &FileSize);

            /* 3-2. allocate buffer */
            if (Rval == ARM_OK) {
                Rval = ArmMemPool_Allocate(pFeederCfg->MemPoolId, (UINT32) FileSize, pRawDataBuf);
            }

            /* 3-3. Load data from file */
            if (Rval == ARM_OK) {
                Rval = ArmFIO_Load(pRawDataBuf->pBuffer, (UINT32) FileSize, pFilename, &ReadSize);
                if (Rval == ARM_OK) {
                    if(pRawDataBuf->buffer_cacheable != 0U) {
                        (void) ArmMemPool_CacheClean(pRawDataBuf);
                    }
                    ArmLog_DBG(ARM_LOG_FILE_FEEDER, "load data, daddr = 0x%x size = %d", pRawDataBuf->buffer_daddr, (UINT32) ReadSize);
                } else {
                    ArmLog_ERR(ARM_LOG_FILE_FEEDER, "## fail to load data[%d], ret = %d", i, (UINT32) ReadSize);
                }
            }
        }
    }

    /* 4. Register callback for FREE INPUT */
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        Rval = CtCvAlgoWrapper_RegCb(pFeederCfg->Slot, 0U, FreeInputCallback);
    }

    /* 5. Call CtCvAlgoWrapper_Feed */
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        memio_source_recv_multi_raw_t MultiRaw = {0};

        MultiRaw.num_io = pFeederCfg->Num;
        for (UINT32 i = 0U; i < pFeederCfg->Num; i ++) {
            MultiRaw.io[i].addr      = RawDataBuf[i].buffer_daddr;   //physical address
            MultiRaw.io[i].size      = RawDataBuf[i].buffer_size;
            MultiRaw.io[i].pitch     = 0;
            MultiRaw.io[i].batch_cnt = 0;
        }

        Rval = CtCvAlgoWrapper_FeedRaw(pFeederCfg->Slot, &MultiRaw, NULL);
    }

    /* 6. Wait to free input */
    if (Rval == ARM_OK) {
        UINT32 ActualFlags = 0U;
        Rval = ArmEventFlag_Wait(&g_FileFeederEventFlag, EVENT_FREE_INPUT, ARM_EF_OR_CLEAR, &ActualFlags, DEFAULT_FILE_FEEDER_TIMEOUT);
    }

    /* 7. Free buffer */
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        for (UINT32 i = 0U; i < pFeederCfg->Num; i ++) {
            if (Rval == ARM_OK) {
                Rval = ArmMemPool_Free(pFeederCfg->MemPoolId, &RawDataBuf[i]);
            }
        }
    }

    return Rval;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IdspFileFeeder_Start
 *
 *  @Description:: Load binary from file and pack up to picinfo format
 *
 *                   Y                                  UV
 *
 *          <---- PydPitch ---->               <---- PydPitch ---->
 *      ^   --------------------           ^   --------------------     ^
 *      |  |                    |         Y/2 |<- X ->             |    |
 *      Y  |<- X ->             |          V  |        ------      | RoiH/2
 *      |  |                    |             |       |DataUV|     |    |
 *      V  |                    |             |        ------      |    |
 *         |        ------      |             |____________________|    V
 *         |       |DataY |     |
 *         |        ------      |
 *         |____________________|
 *
 *
 *      [PydScale] -    0     1     2     3     4    5
 *      [RoiPitch] - 1984  1408  1024   704   512  384
 *      [RoiWidth] - 1920  1360   960   680   480  340
 *      [RoiHeight]- 1080   764   540   382   272  160
 *
 *  @Input      ::
 *    FeederCfg:   The IdspFileFeeder Config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 IdspFileFeeder_Start(const IDSPFILE_FEEDER_CFG_s *pFeederCfg)
{
    UINT32 Rval = ARM_OK;
    flexidag_memblk_t PicinfoBufY, PicinfoBufUV, TempBuf;

    // Init PicinfoBufY, PicinfoBufUV, TempBuf
    InitMemblk(&PicinfoBufY);
    InitMemblk(&PicinfoBufUV);
    InitMemblk(&TempBuf);

    /* 1. Sanity check for parameters */
    if (pFeederCfg == NULL) {
        ArmLog_ERR(ARM_LOG_FILE_FEEDER, "## FileFeeder_Start fail (pFeederCfg is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pFeederCfg->PyramidScale >= MAX_HALF_OCTAVES) {
            ArmLog_ERR(ARM_LOG_FILE_FEEDER, "## IdspFileFeeder_Start fail (invalid PyramidScale %u)", pFeederCfg->PyramidScale, 0U);
            Rval = ARM_NG;
        } else {
            UINT32 CtRoiPitch[6]  = {1984, 1408, 1024, 704, 512, 384};
            UINT32 CtRoiWidth[6]  = {1920, 1360,  960, 680, 480, 340};
            UINT32 CtRoiHeight[6] = {1080,  764,  540, 382, 272, 160};
            UINT32 PyramidScale = pFeederCfg->PyramidScale;

            // Sanity check for cnn_testbed pyramid setting (We only have one sensor mode)
            if (PyramidScale < 6U) {
                if (pFeederCfg->RoiPitch != CtRoiPitch[PyramidScale]) {
                    ArmLog_WARN(ARM_LOG_FILE_FEEDER, "The RoiPitch (%d) doens't match to cnn_testbed setting (%d)", pFeederCfg->RoiPitch, CtRoiPitch[PyramidScale]);
                }
                if (pFeederCfg->RoiWidth != CtRoiWidth[PyramidScale]) {
                    ArmLog_WARN(ARM_LOG_FILE_FEEDER, "The RoiWidth (%d) doens't match to cnn_testbed setting (%d)", pFeederCfg->RoiWidth, CtRoiWidth[PyramidScale]);
                }
                if (pFeederCfg->RoiHeight != CtRoiHeight[PyramidScale]) {
                    ArmLog_WARN(ARM_LOG_FILE_FEEDER, "The RoiHeight (%d) doens't match to cnn_testbed setting (%d)", pFeederCfg->RoiHeight, CtRoiHeight[PyramidScale]);
                }
            }
        }
    }

    /* 2. Create eventflag */
    if (Rval == ARM_OK) {
        Rval = CreateResource();
    }

    /* 3. Allocate Y/UV buffer and temp buffer */
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        Rval = ArmMemPool_Allocate(pFeederCfg->MemPoolId, MAX_YUV_SIZE, &PicinfoBufY);
    }
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        Rval = ArmMemPool_Allocate(pFeederCfg->MemPoolId, MAX_YUV_SIZE/2U, &PicinfoBufUV);
    }
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        UINT32 FileSizeY;   // Y should have bigger size

        Rval = ArmFIO_GetSize(pFeederCfg->FilenameY, &FileSizeY);
        if (Rval == ARM_OK) {
            Rval = ArmMemPool_Allocate(pFeederCfg->MemPoolId, FileSizeY, &TempBuf);
        }
    }

    /* 4. Load YUV data from file */
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        for (UINT32 i = 0U; i < 2U; i ++) {  // 2 for Y and UV
            UINT32 Size;
            const char * pFilename;
            UINT32 Pitch = 0U, OffX = 0U, OffY = 0U, DataW = 0U, DataH = 0U;
            flexidag_memblk_t *pBuf;

            if (i == 0U) {   // Y
                pFilename = pFeederCfg->FilenameY;
                Pitch     = pFeederCfg->RoiPitch;
                OffX      = pFeederCfg->OffsetX;
                OffY      = pFeederCfg->OffsetY;
                DataW     = pFeederCfg->DataWidth;
                DataH     = pFeederCfg->DataHeight;
                pBuf      = &PicinfoBufY;

            } else {    // UV
                pFilename = pFeederCfg->FilenameUV;
                Pitch     = pFeederCfg->RoiPitch;
                OffX      = pFeederCfg->OffsetX & 0xFFFFFFFEU;
                OffY      = (pFeederCfg->OffsetY & 0xFFFFFFFEU ) / 2U;
                DataW     = pFeederCfg->DataWidth;
                DataH     = pFeederCfg->DataHeight / 2U;
                pBuf      = &PicinfoBufUV;
            }

            /* 4-1. Open Y/UV file */
            Rval = ArmFIO_Load(TempBuf.pBuffer, TempBuf.buffer_size, pFilename, &Size);
            if (Rval == ARM_OK) {
                if (Size == (DataW * DataH)) {

                    /* 4-2. Copy Y/UV Data to Y/UV buffer line by line */
                    for (UINT32 CopyH = 0U; CopyH < DataH; CopyH ++) {
                        UINT32 SrcOffset = DataW * CopyH;
                        UINT32 DstOffset = OffX + (Pitch * (OffY + CopyH));

                        (void) ArmStdC_memcpy(&pBuf->pBuffer[DstOffset], &TempBuf.pBuffer[SrcOffset], DataW);
                    }

                    /* 4-3. Flush cache */
                    if (pBuf->buffer_cacheable != 0U) {
                        (void) ArmMemPool_CacheClean(pBuf);
                    }
                } else {
                    ArmLog_ERR(ARM_LOG_FILE_FEEDER, "## The binary size(%d) doesn't match the user setting(DataW * DataH = %d)", (UINT32) Size, DataW * DataH);
                    Rval = ARM_NG;
                }
            }
        }

    }

    /* 5. Register callback for FREE INPUT */
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        Rval = CtCvAlgoWrapper_RegCb(pFeederCfg->Slot, 0U, FreeInputCallback);
    }

    /* 6. Call CtCvAlgoWrapper_Feed */
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        memio_source_recv_picinfo_t PicInfo = {0};

        PicInfo.pic_info.rpLumaLeft[pFeederCfg->PyramidScale]    = PicinfoBufY.buffer_daddr;
        PicInfo.pic_info.rpLumaRight[pFeederCfg->PyramidScale]   = PicinfoBufY.buffer_daddr;
        PicInfo.pic_info.rpChromaLeft[pFeederCfg->PyramidScale]  = PicinfoBufUV.buffer_daddr;
        PicInfo.pic_info.rpChromaRight[pFeederCfg->PyramidScale] = PicinfoBufUV.buffer_daddr;
        PicInfo.pic_info.pyramid.half_octave[pFeederCfg->PyramidScale].ctrl.roi_pitch = (UINT16) pFeederCfg->RoiPitch;
        PicInfo.pic_info.pyramid.half_octave[pFeederCfg->PyramidScale].roi_width_m1   = (UINT16) pFeederCfg->RoiWidth - 1U;
        PicInfo.pic_info.pyramid.half_octave[pFeederCfg->PyramidScale].roi_height_m1  = (UINT16) pFeederCfg->RoiHeight - 1U;

        ArmLog_DBG(ARM_LOG_FILE_FEEDER, "rpLumaLeft = 0x%x, rpChromaLeft = 0x%x", PicinfoBufY.buffer_daddr, PicinfoBufUV.buffer_daddr);

        Rval = CtCvAlgoWrapper_FeedPicinfo(pFeederCfg->Slot, &PicInfo, NULL);
    }

    /* 7. Wait to free input */
    if (Rval == ARM_OK) {
        UINT32 ActualFlags = 0U;
        Rval = ArmEventFlag_Wait(&g_FileFeederEventFlag, EVENT_FREE_INPUT, ARM_EF_OR_CLEAR, &ActualFlags, DEFAULT_FILE_FEEDER_TIMEOUT);
    }

    /* 8. Free buffer */
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        Rval = ArmMemPool_Free(pFeederCfg->MemPoolId, &PicinfoBufY);
    }
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        Rval = ArmMemPool_Free(pFeederCfg->MemPoolId, &PicinfoBufUV);
    }
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        Rval = ArmMemPool_Free(pFeederCfg->MemPoolId, &TempBuf);
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SecretFeeder_Start
 *
 *  @Description:: Load secret binary and send to flexidag
 *
 *  @Input      ::
 *    FeederCfg:   The SecretFeeder Config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32  SecretFeeder_Start(const SECRET_FEEDER_CFG_s *pFeederCfg)
{
    UINT32 Rval = ARM_OK;
    flexidag_memblk_t SecretDataBuf;

    InitMemblk(&SecretDataBuf);

    /* 1. Sanity check for parameters */
    if (pFeederCfg == NULL) {
        ArmLog_ERR(ARM_LOG_FILE_FEEDER, "## SecretFeeder_Start fail (pFeederCfg is null)", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Load data from file */
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        UINT32 FileSize, ReadSize;
        const char *pFilename = &(pFeederCfg->Filename[0]);
        flexidag_memblk_t *pSecretDataBuf = &(SecretDataBuf);

        /* 2-1. Query file size */
        Rval = ArmFIO_GetSize(pFilename, &FileSize);

        /* 2-2. allocate buffer */
        if (Rval == ARM_OK) {
            Rval = ArmMemPool_Allocate(pFeederCfg->MemPoolId, (UINT32) FileSize, pSecretDataBuf);
        }

        /* 2-3. Load data from file */
        if (Rval == ARM_OK) {
            Rval = ArmFIO_Load(pSecretDataBuf->pBuffer, (UINT32) FileSize, pFilename, &ReadSize);
            if (Rval == ARM_OK) {
                if(pSecretDataBuf->buffer_cacheable != 0U) {
                    (void) ArmMemPool_CacheClean(pSecretDataBuf);
                }
                ArmLog_DBG(ARM_LOG_FILE_FEEDER, "load secret data, daddr = 0x%x size = %d", pSecretDataBuf->buffer_daddr, (UINT32) ReadSize);
            } else {
                ArmLog_ERR(ARM_LOG_FILE_FEEDER, "## fail to load secret data, ret = %d", (UINT32) ReadSize, 0U);
            }
        }
    }

    /* 3. Call CtCvAlgoWrapper_Control */
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        Rval = CtCvAlgoWrapper_Control(pFeederCfg->Slot, CTRL_TYPE_SEND_SECRET, SecretDataBuf.pBuffer);
        if (Rval != ARM_OK) {
            ArmLog_ERR(ARM_LOG_FILE_FEEDER, "## CtCvAlgoWrapper_Control fail", 0U, 0U);
        }
    }

    /* 4. Free buffer */
    if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
        Rval = ArmMemPool_Free(pFeederCfg->MemPoolId, &SecretDataBuf);
    }

    return Rval;
}

