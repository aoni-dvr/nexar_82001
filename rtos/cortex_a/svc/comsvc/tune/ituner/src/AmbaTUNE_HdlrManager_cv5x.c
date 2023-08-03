/*
*  @file AmbaTUNE_HdlrManager_cv5x.c
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

#include "AmbaTUNE_HdlrManager_cv5x.h"
#include "AmbaTUNE_SystemApi_cv5x.h"
#include "AmbaTUNE_USBHdlr_cv5x.h"
#include "AmbaTUNE_TextHdlr_cv5x.h"
#ifndef ITN_NULL
#define ITN_NULL (void*)0
#endif

extern AMBA_KAL_MUTEX_t gTune_Mutex;
AMBA_KAL_MUTEX_t gTune_Mutex = {0};

INT32 ITuner_Save_Data(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s *Ext_File_Param);
INT32 ITuner_Load_Data(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s *Ext_File_Param);

typedef struct {
    INT32 (*Init)(const Ituner_Initial_Config_t *pMemPool);
    INT32 (*Save_IDSP)(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_ITN_Save_Param_s *pSaveParam);
    INT32 (*Load_IDSP)(const AMBA_ITN_Load_Param_s *pLoadParam);
    INT32 (*Save_Data)(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s *pExt_File_Param);
    INT32 (*Load_Data)(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s *pExt_File_Param);
    INT32 (*Execute_IDSP)(const AMBA_IK_MODE_CFG_s *pMode);
    INT32 (*Get_SystemInfo)(AMBA_ITN_SYSTEM_s *pSystem);
    INT32 (*Set_SystemInfo)(const AMBA_ITN_SYSTEM_s *pSystem);
    INT32 (*Get_ItunerInfo)(AMBA_ITN_ITUNER_INFO_s *pItunerInfo);
    void (*Get_VideoHdrRawInfo)(AMBA_ITN_VIDEO_HDR_RAW_INFO_s *pRawInfo);
    void (*Set_VideoHdrRawInfo)(const AMBA_ITN_VIDEO_HDR_RAW_INFO_s *pRawInfo);
} Tune_Func_s;

static inline INT32 AmbaTUNE_Warpper_USB_Load_IDSP(const AMBA_ITN_Load_Param_s *Load_Param);
static inline INT32 AmbaTUNE_Warpper_USB_Save_IDSP(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_ITN_Save_Param_s *Save_Param);
static inline INT32 AmbaTUNE_Warpper_Text_Load_IDSP(const AMBA_ITN_Load_Param_s *Load_Param);
static inline INT32 AmbaTUNE_Warpper_Text_Save_IDSP(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_ITN_Save_Param_s *Save_Param);

static const Tune_Func_s gUSBHdlr_Func = {
    .Init = USBHdlr_Init,
    .Save_IDSP = AmbaTUNE_Warpper_USB_Save_IDSP,
    .Load_IDSP = AmbaTUNE_Warpper_USB_Load_IDSP,
    .Save_Data = USBHdlr_Save_Data,
    .Load_Data = USBHdlr_Load_Data,
    .Execute_IDSP = USBHdlr_Execute_IDSP,
    .Get_SystemInfo = USBHdlr_Get_SystemInfo,
    .Set_SystemInfo = USBHdlr_Set_SystemInfo,
    .Get_VideoHdrRawInfo = ITN_NULL,
    .Set_VideoHdrRawInfo = ITN_NULL,
};

static const Tune_Func_s gTextHdlr_Func = {
    .Init = TextHdlr_Init,
    .Save_IDSP = AmbaTUNE_Warpper_Text_Save_IDSP,
    .Load_Data = TextHdlr_Load_Data,
    .Save_Data = TextHdlr_Save_Data,
    .Load_IDSP = AmbaTUNE_Warpper_Text_Load_IDSP,
    .Execute_IDSP = TextHdlr_Execute_IDSP,
    .Get_SystemInfo = TextHdlr_Get_SystemInfo,
    .Set_SystemInfo = TextHdlr_Set_SystemInfo,
    .Get_VideoHdrRawInfo = AmbaItuner_Get_VideoHdrRawInfo,
    .Set_VideoHdrRawInfo = AmbaItuner_Set_VideoHdrRawInfo,
};

typedef enum {
    TUNE_VALID = 0,
    TUNE_INVALID = 1
} TuneHdlr_Status_e;
typedef struct {
    AMBA_ITN_Hdlr_Mode_e Active_Parser_Mode;
    TuneHdlr_Status_e Parser_Status;
    AMBA_KAL_MUTEX_t *Parser_Mutex;
} TuneHdlr_s;

static TuneHdlr_s gTune_Hdlr = {
    .Active_Parser_Mode = TEXT_TUNE,
    .Parser_Status = TUNE_INVALID,
    .Parser_Mutex = ITN_NULL,
};

static const Tune_Func_s *gTune_Func[MAX_TUNE] = {
    &gTextHdlr_Func,
    &gUSBHdlr_Func,
};
void OSAL_MUTEX_CREATE(void);
void OSAL_MUTEX_LOCK(void);
void OSAL_MUTEX_UNLOCK(void);


void OSAL_MUTEX_CREATE(void)
{
#if (defined(__unix__) && !defined(__QNX__))
#else
    char str[20] = "Parser_Mutex";
    (void)AmbaKAL_MutexCreate(gTune_Hdlr.Parser_Mutex, &str[0]);
#endif
}
void OSAL_MUTEX_LOCK(void)
{
#if (defined(__unix__) && !defined(__QNX__))
#else
    (void)AmbaKAL_MutexTake(gTune_Hdlr.Parser_Mutex, 0xFFFFFFFFU);//AMBA_KAL_WAIT_FOREVER
#endif
}
void OSAL_MUTEX_UNLOCK(void)
{
#if (defined(__unix__) && !defined(__QNX__))
#else
    (void)AmbaKAL_MutexGive(gTune_Hdlr.Parser_Mutex);
#endif
}

static inline INT32 AmbaTUNE_Warpper_USB_Load_IDSP(const AMBA_ITN_Load_Param_s *Load_Param)
{
    if (Load_Param==ITN_NULL) {
        // FIXME
    }
    return 0;//USBHdlr_Load_IDSP((TUNE_USB_Load_Param_s*) &Load_Param->USB.Buffer);
}

static inline INT32 AmbaTUNE_Warpper_USB_Save_IDSP(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_ITN_Save_Param_s *Save_Param)
{
    if ((pMode==ITN_NULL) || (Save_Param==ITN_NULL)) {
        // FIXME
    }
    return 0;//USBHdlr_Save_IDSP(pMode, (TUNE_USB_Save_Param_s*) &Save_Param->USB.Buffer);
}
static inline INT32 AmbaTUNE_Warpper_Text_Load_IDSP(const AMBA_ITN_Load_Param_s *Load_Param)
{
    return TextHdlr_Load_IDSP(Load_Param->Text.FilePath);
}

static inline INT32 AmbaTUNE_Warpper_Text_Save_IDSP(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_ITN_Save_Param_s *Save_Param)
{
    return TextHdlr_Save_IDSP(pMode, Save_Param->Text.Filepath);
}


static void AmbaTUNE_Check_Mutex_Created(void)
{
    if (gTune_Hdlr.Parser_Mutex == ITN_NULL) {
        gTune_Hdlr.Parser_Mutex = &gTune_Mutex;
        OSAL_MUTEX_CREATE();
    }
}

/**
* Query the iTuner buffer size
* @param [in,out]  pSize: queried size of iTuner buffer the service would use
* @return OK - success, NG - fail
*/
UINT32 AmbaItn_QueryItuner(UINT32 *pSize)
{
    UINT32 Ret = 0U;

    *pSize = (UINT32)(sizeof(ITUNER_Calib_Table_s)) + 32U;

    return Ret;
}

/**
* Initialization of the iTuner service
* @param [in]  pBuffer: pointer of iTuner buffer
* @param [in]  ItunerSize: size of iTuner buffer
* @return OK - success, NG - fail
*/
UINT32 AmbaItn_Init(void *pBuffer, UINT32 ItunerSize)
{
    UINT32 Rval;
    INT32 Ret;
    Ituner_Initial_Config_t InitialConfig;
    UINTPTR MisraUptr;
    UINT32 BufferSize;

    (void)AmbaItn_QueryItuner(&BufferSize);
    if (ItunerSize < BufferSize) {
        ituner_print_uint32_5("input ituner buffer is not enough: input size: %d, needed size: %d", ItunerSize, BufferSize, DC_U, DC_U, DC_U);
        Ret = -1;
    } else {
        AmbaTUNE_Check_Mutex_Created();
        OSAL_MUTEX_LOCK();
        gTune_Hdlr.Parser_Status = TUNE_VALID;
        InitialConfig.pItunerBuffer = pBuffer;
        // sbp 32 alignment
        // We don't initial it to solve buffer clean issue
        //(void)ituner_memset(pBuffer, 0x0, ItunerSize);
        (void)ituner_memcpy(&MisraUptr, &InitialConfig.pItunerBuffer, sizeof(UINTPTR));
        MisraUptr = (MisraUptr+31UL)-((MisraUptr+31UL) % 32UL);
        (void)ituner_memcpy(&InitialConfig.pItunerBuffer, &MisraUptr, sizeof(void*));
        //if (gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Set_InitialConfig != ITN_NULL) {
        //    gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Set_InitialConfig(pTuneInitialConfig);
        //}
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Init(&InitialConfig);
        OSAL_MUTEX_UNLOCK();
    }

    if (Ret == 0) {
        Rval = TUNE_OK;
    } else {
        Rval = TUNE_ERROR_GENERAL;
    }

    return Rval;
}

/**
* Change or select the iTuner parser mode
* @param [in]  ParserMode: change the parser mode between text and usb
* @return OK - success, NG - fail
*/
UINT32 AmbaItn_Change_Parser_Mode(AMBA_ITN_Hdlr_Mode_e ParserMode)
{
    AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    if (gTune_Hdlr.Active_Parser_Mode == ParserMode) {
        ;
    } else {
        gTune_Hdlr.Active_Parser_Mode = ParserMode;
        gTune_Hdlr.Parser_Status = TUNE_INVALID;
    }
    OSAL_MUTEX_UNLOCK();

    return TUNE_OK;
}

/**
* Save filters input parameters to an iTuner file
* @param [in]  pMode: Set ContextId
* @param [in,out]  pSaveParam: Set file path
* @return OK - success, NG - fail
*/
UINT32 AmbaItn_Save_IDSP(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_ITN_Save_Param_s *pSaveParam)
{
    UINT32 Rval;
    INT32 Ret = 0;
    AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
        ituner_print_str_5("[TEXT HDLR][WARNING]: %s , Parser_Status Invalid", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Ret = -1;
    } else {
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Save_IDSP(pMode, pSaveParam);
    }
    OSAL_MUTEX_UNLOCK();

    if (Ret == 0) {
        Rval = TUNE_OK;
    } else {
        Rval = TUNE_ERROR_GENERAL;
    }
    return Rval;
}

/**
* Load iTuner file to filters input parameters
* @param [in]  pLoadParam: Set file path
* @return OK - success, NG - fail
*/
UINT32 AmbaItn_Load_IDSP(const AMBA_ITN_Load_Param_s *pLoadParam)
{
    UINT32 Rval;
    INT32 Ret = 0;
    AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
        ituner_print_str_5("[TEXT HDLR][WARNING]: %s , Parser_Status Invalid", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Ret = -1;
    } else {
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Load_IDSP(pLoadParam);
    }
    OSAL_MUTEX_UNLOCK();

    if (Ret == 0) {
        Rval = TUNE_OK;
    } else {
        Rval = TUNE_ERROR_GENERAL;
    }
    return Rval;
}

INT32 ITuner_Save_Data(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Ret = 0;
    AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
        ituner_print_str_5("[TEXT HDLR][WARNING]: %s , Parser_Status Invalid", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Ret = -1;
    } else {
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Save_Data(Ext_File_Type, Ext_File_Param);
    }
    OSAL_MUTEX_UNLOCK();
    return Ret;
}

INT32 ITuner_Load_Data(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Ret = 0;
    AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
        ituner_print_str_5("[TEXT HDLR][WARNING]: %s , Parser_Status Invalid", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Ret = -1;
    } else {
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Load_Data(Ext_File_Type, Ext_File_Param);
    }
    OSAL_MUTEX_UNLOCK();
    return Ret;
}

/**
* Execute iDSP
* @param [in]  pMode: Set ContextId
* @return OK - success, NG - fail
*/
UINT32 AmbaItn_Execute_IDSP(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval;
    INT32 Ret = 0;
    AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
        ituner_print_str_5("[TEXT HDLR][WARNING]: %s , Parser_Status Invalid", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Ret = -1;
    } else {
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Execute_IDSP(pMode);
    }
    OSAL_MUTEX_UNLOCK();

    if (Ret == 0) {
        Rval = TUNE_OK;
    } else {
        Rval = TUNE_ERROR_GENERAL;
    }
    return Rval;
}

/**
* Get iTuner System parameters
* @param [in,out]  pSystem: iTuner system parameters
* @return OK - success, NG - fail
*/
UINT32 AmbaItn_Get_SystemInfo(AMBA_ITN_SYSTEM_s *pSystem)
{
    UINT32 Rval;
    INT32 Ret = 0;
    AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
        ituner_print_str_5("[TEXT HDLR][WARNING]: %s , Parser_Status Invalid", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Ret = -1;
    } else {
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Get_SystemInfo(pSystem);
    }
    OSAL_MUTEX_UNLOCK();

    if (Ret == 0) {
        Rval = TUNE_OK;
    } else {
        Rval = TUNE_ERROR_GENERAL;
    }
    return Rval;
}

/**
* Set iTuner System parameters
* @param [in]  pSystem: iTuner system parameters
* @return OK - success, NG - fail
*/
UINT32 AmbaItn_Set_SystemInfo(const AMBA_ITN_SYSTEM_s *pSystem)
{
    UINT32 Rval;
    INT32 Ret = 0;
    AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
        ituner_print_str_5("[TEXT HDLR][WARNING]: %s , Parser_Status Invalid", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Ret = -1;
    } else {
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Set_SystemInfo(pSystem);
    }
    OSAL_MUTEX_UNLOCK();

    if (Ret == 0) {
        Rval = TUNE_OK;
    } else {
        Rval = TUNE_ERROR_GENERAL;
    }
    return Rval;
}

/**
* Get iTuner parameter HDR raw info
* @param [in,out]  pRawInfo: iTuner HDR raw info parameters
* @return OK - success, NG - fail
*/
UINT32 AmbaItn_Get_VideoHdrRawInfo(AMBA_ITN_VIDEO_HDR_RAW_INFO_s *pRawInfo)
{
    UINT32 Rval;
    INT32 Ret = 0;
    AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
        ituner_print_str_5("[TEXT HDLR][WARNING]: %s , Parser_Status Invalid", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Ret = -1;
    } else {
        Ret = 0;
        gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Get_VideoHdrRawInfo(pRawInfo);
    }
    OSAL_MUTEX_UNLOCK();

    if (Ret == 0) {
        Rval = TUNE_OK;
    } else {
        Rval = TUNE_ERROR_GENERAL;
    }
    return Rval;
}
#if 0
UINT32 AmbaItn_Set_VideoHdrRawInfo(const AMBA_ITN_VIDEO_HDR_RAW_INFO_s *pRawInfo)
{
    UINT32 Rval;
    INT32 Ret = 0;
    AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
        ituner_print_str_5("[TEXT HDLR][WARNING]: %s , Parser_Status Invalid", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Ret = -1;
    } else {
        Ret = 0;
        gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Set_VideoHdrRawInfo(pRawInfo);
    }
    OSAL_MUTEX_UNLOCK();

    if (Ret == 0) {
        Rval = TUNE_OK;
    } else {
        Rval = TUNE_ERROR_GENERAL;
    }
    return Rval;

}
#endif

