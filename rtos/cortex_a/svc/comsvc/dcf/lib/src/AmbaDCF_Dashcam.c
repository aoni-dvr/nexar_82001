/**
*  @file AmbaDCF_Dashcam.c
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
*  @details DCF (Design rule for Camera File system v2.0) related APIs
*
*/

#include "AmbaUtility.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"
#include "AmbaPrint.h"
//#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"

#include "AmbaDCF.h"
#include "AmbaDCF_Dashcam.h"
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "custom.h"
#include "../../../../apps/icam/cardv/system/linux_api_event.h"
#include "../../../../../soc/io/src/common/inc/AmbaTime.h"
#endif
#define DEBUG_DCF               0

#define CAR_DCF_MAX_CNT         10000U
#define INDEX_0                 0U
#define INDEX_1                 1U
#define FOLDER_LEN              16U

/*************************************************************************
 * structures, variables
 ************************************************************************/

#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
typedef struct {
    UINT64 datetime;
    char filename[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    AMBA_DCF_FILE_TYPE_e obj_type;
} SvcDCF_DashcamRefEmem_t;
#endif

typedef struct {
    AMBA_KAL_MUTEX_t MutexID;
} DCF_INFO_s;

static const char slash[] = {"\\"};
static const char colon[] = {":"};
static char video      [] = {".mov"};
static char Drive[AMBA_DCF_DRIVER_MAX_NUM][FOLDER_LEN] = {"0"};

static char RootFolderName[AMBA_DCF_FOLDER_MAX_NUM][FOLDER_LEN] = {"0","0"};
static char FolderName[AMBA_DCF_FOLDER_MAX_NUM][FOLDER_LEN] = {"0","0"};
static SvcDCF_DashcamRefEmem_t SvcDCF_DashcamElementTable[AMBA_DCF_DRIVER_MAX_NUM][AMBA_DCF_FOLDER_MAX_NUM][AMBA_DCF_FOLDER_MAX_NUM][CAR_DCF_MAX_CNT + 1U] __attribute__((section(".bss.noinit")));
static UINT32 SvcDCF_DashcamElementCnt[AMBA_DCF_DRIVER_MAX_NUM][AMBA_DCF_FOLDER_MAX_NUM][AMBA_DCF_FOLDER_MAX_NUM] __attribute__((section(".bss.noinit")));
static DCF_INFO_s      DCFInfo[AMBA_DCF_DRIVER_MAX_NUM] __attribute__((section(".bss.noinit")));

static void SvcDcf_DashcamNG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#define LOG_BUF_SIZE        512U
#define LOG_ARGC            2U
#define SvcWrap_sprintfStr       AmbaUtility_StringPrintStr

    char        LogBuf[LOG_BUF_SIZE];
    const char  *ArgS[LOG_ARGC];
    UINT32      Rval, Argc;

    Argc = 0;
    ArgS[Argc] = pModule;
    Argc++;
    ArgS[Argc] = pFormat;
    Argc++;

    Rval = SvcWrap_sprintfStr(LogBuf, LOG_BUF_SIZE, "\033""[0;31m[%s|NG]: %s", Argc, ArgS);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, 0U, 0U, 0U);
        //AmbaPrint_Flush();
    }
}

static void DcfDashcam_strcpy(char *dest, const char *src)
{
    UINT32 len;

    len = (UINT32)AmbaUtility_StringLength(src);
    if(src[len] == '\0') {
       len++;
    }

    AmbaUtility_StringCopy(dest, len, src);
}

static UINT64 StrToUint64(const char *str)
{
    UINT64 Value = 0U;

    if (AmbaUtility_StringToUInt64(str, &Value) != OK){
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);        
        AmbaPrint_PrintStr5("StrToUint64 NG, %s", str, NULL, NULL, NULL, NULL);
    }

    return Value;
}

static UINT32 SvcDCF_DashcamGetDriveID(UINT8 *pDriveID, const char *pFileName)
{
    UINT8 DriveIDNo;
    char FileName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    DcfDashcam_strcpy(FileName, pFileName);

    for (DriveIDNo = 0U; DriveIDNo < (UINT8)AMBA_DCF_DRIVER_MAX_NUM; DriveIDNo ++) {
        if(AmbaUtility_StringCompare(&FileName[0], &Drive[DriveIDNo][0], 1U) == OK_INT32) {
            *pDriveID = DriveIDNo;
            break;
        }
    }

    return OK_UINT32;
}

static UINT32 SvcDCF_DashcamGetRootFolderID(UINT8 *pRootFolderID, const char *pFileName)
{
    UINT8 RootFolderID = 0U;
    UINT32 RootFolderLength = 0U;

    char FileName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    DcfDashcam_strcpy(FileName, pFileName);

    if(RootFolderName[RootFolderID][0U] != '0') {
        for (RootFolderID = 0U; RootFolderID < (UINT8)AMBA_DCF_FOLDER_MAX_NUM; RootFolderID ++) {
            if(RootFolderName[RootFolderID][0U] != '0') {
                //AmbaPrint_PrintStr5("SvcDCF_DashcamGetRootFolderID %s", &FileName[3U], NULL, NULL, NULL, NULL);
                RootFolderLength = (UINT32)AmbaUtility_StringLength(RootFolderName[RootFolderID]);
                if(AmbaUtility_StringCompare(&FileName[3U], &RootFolderName[RootFolderID][0], RootFolderLength) == OK_INT32) {
                    *pRootFolderID = RootFolderID;
                    break;
                }
            }
        }
    } else {
        *pRootFolderID = RootFolderID;
    }

    return OK_UINT32;
}

static UINT32 SvcDCF_DashcamGetFolderID(UINT8 *pFolderID, const char *pFileName)
{
    UINT8  RootFolderID = 0U;
    UINT32 RootFolderLength = 0U;
    UINT8  FolderID = 0U;
    UINT32 FolderLength = 0U;
    char FileName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    DcfDashcam_strcpy(FileName, pFileName);

    if(RootFolderName[0U][0U] != '0') {
        if( SvcDCF_DashcamGetRootFolderID(&RootFolderID, pFileName) != OK){
            SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
        }

        RootFolderLength = (UINT32)AmbaUtility_StringLength(RootFolderName[RootFolderID]);
        RootFolderLength = RootFolderLength + 1U;
    }

    for (FolderID = 0U; FolderID < (UINT8)AMBA_DCF_FOLDER_MAX_NUM; FolderID ++) {
        //AmbaPrint_PrintStr5("SvcDCF_DashcamGetFolderID %s", &FileName[(3U + RootFolderLength)], NULL, NULL, NULL, NULL);
        if(FolderName[FolderID][0U] != '0') {
            FolderLength = (UINT32)AmbaUtility_StringLength(FolderName[FolderID]);
            if(AmbaUtility_StringCompare(&FileName[(3U + RootFolderLength)], &FolderName[FolderID][0U], FolderLength) == OK_INT32) {
                *pFolderID = FolderID;
                break;
            }
        }
    }

    return OK_UINT32;
}

static UINT64 SvcDCF_DashcamFnameToDatetime(const char *pFileName)
{
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    #define FN_Length 24U
#endif

    char fn[64];
    char temp[] = {'0','\0'};
    UINT32 FileStartIndex;
    UINT64 c_datetime = 0U;

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    int i = 0;
    FileStartIndex = 0;
    for (i = AmbaUtility_StringLength(pFileName) - 1; i >= 0; i--)  {
        if (pFileName[i] == '\\') {
            FileStartIndex = i + 1;
            break;
        }
    }
#else
    FileStartIndex = (UINT32)AmbaUtility_StringLength(pFileName) - FN_Length;
#endif
    DcfDashcam_strcpy(fn, &pFileName[FileStartIndex]);

    //year
    temp[0] = fn[0];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    temp[0] = fn[1];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    temp[0] = fn[2];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    temp[0] = fn[3];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    //month
    temp[0] = fn[4];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    temp[0] = fn[5];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    //date
    temp[0] = fn[6];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    temp[0] = fn[7];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    //hour
    temp[0] = fn[9];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    temp[0] = fn[10];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    //minute
    temp[0] = fn[11];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    temp[0] = fn[12];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    //second
    temp[0] = fn[13];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    temp[0] = fn[14];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    temp[0] = '0' + (fn[15] - 'A');
    c_datetime = c_datetime + StrToUint64(temp);
#else
    //hour
    temp[0] = fn[8];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    temp[0] = fn[9];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    //minute
    temp[0] = fn[10];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    temp[0] = fn[11];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    //second
    temp[0] = fn[12];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    temp[0] = fn[13];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    //VIN ID
    temp[0] = fn[15];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    temp[0] = fn[16];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    //stream ID
    temp[0] = fn[18];
    c_datetime = c_datetime + StrToUint64(temp);
    c_datetime = c_datetime * 10U;

    temp[0] = fn[19];
    c_datetime = c_datetime + StrToUint64(temp);
#endif

#if DEBUG_DCF
    {
        char AdvStrBuf[128];
        UINT32 CurStrLen;

        if( AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf)) != OK) {
            SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
        }
        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "[DCF_DEBUG]: fn ");
        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), fn);
        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), " to datetime ");
        CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
        if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
            (void)AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                    c_datetime, 10U);
        }
        AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }
#endif

    return c_datetime;
}

static AMBA_DCF_FILE_TYPE_e SvcDCF_DashcamCheckFileType(const char *pFileName)
{
    UINT32 i;
    ULONG  Addr;
    char tmp_mp4[]={".mp4"};
    char tmp_mov[]={".mov"};
    char tmp_jpg[]={".jpg"};
    AMBA_DCF_FILE_TYPE_e obj_type = AMBA_DCF_NUM_FILE_TYPE;
    const char *fn;

    for (i = ((UINT32)AmbaUtility_StringLength(pFileName) - 1U); i > 0U; i--) {
        if (pFileName[i] == '.') {
            break;
        }
    }

    AmbaMisra_TypeCast(&Addr, &pFileName);
    Addr = Addr + i;
    AmbaMisra_TypeCast(&fn, &Addr);

    if (i > 0U) {
        if (AmbaUtility_StringCompare(fn, tmp_mp4, (UINT32)AmbaUtility_StringLength(tmp_mp4)) == 0) {
            obj_type = AMBA_DCF_FILE_TYPE_VIDEO;
        } else if (AmbaUtility_StringCompare(fn, tmp_mov, (UINT32)AmbaUtility_StringLength(tmp_mov)) == 0) {
            obj_type = AMBA_DCF_FILE_TYPE_VIDEO;
        } else if (AmbaUtility_StringCompare(fn, tmp_jpg, (UINT32)AmbaUtility_StringLength(tmp_jpg)) == 0) {
            obj_type = AMBA_DCF_FILE_TYPE_IMAGE;
        } else {
            obj_type = AMBA_DCF_NUM_FILE_TYPE;
        }
    }

    return obj_type;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static int convert_rtos_path_to_linux_impl(const char *rtos_path, char *linux_path)
{
    unsigned int i = 0;

    if (linux_path == NULL || rtos_path == NULL) {
        return -1;
    }
    AmbaUtility_StringAppend(linux_path, 64, "/mnt/extsd/");
    AmbaUtility_StringAppend(linux_path, 64, rtos_path + 3);
    for (i = 0; i < AmbaUtility_StringLength(linux_path); i++) {
        if (*(linux_path + i) == '\\') {
            *(linux_path + i) = '/';
        }
    }

    return 0;
}
#endif

static void SvcDCF_DashcamInsertFileToIndex(UINT32 index, UINT8 DeviceID, UINT8 RootFolderID, UINT8 FolderID, UINT64 datetime, const char *pFileName, AMBA_DCF_FILE_TYPE_e obj_type, UINT8 isScan)
{
    UINT32 size;

    SvcDCF_DashcamElementCnt[DeviceID][RootFolderID][FolderID] ++;
#if DEBUG_DCF
    {
        char AdvStrBuf[128];
        UINT32 CurStrLen;

        if(AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf)) != OK) {
            SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
        }
        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "[DCF_DEBUG]: fn ");
        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), pFileName);
        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), " to datetime ");
        CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
        if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
            (void)AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                    datetime, 10U);
        }
        AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("Insert element to table index %d", index, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
#endif

    if((SvcDCF_DashcamElementCnt[DeviceID][RootFolderID][FolderID] - index) == 0U){
        /* Insert element on empty */

        SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][index].datetime = datetime;
        AmbaUtility_StringCopy(SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][index].filename,
            sizeof(SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][index].filename),
            pFileName);
        SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][index].obj_type = obj_type;
    } else {
        /* Insert element on index */
        size = (UINT32)sizeof(SvcDCF_DashcamRefEmem_t) * ((SvcDCF_DashcamElementCnt[DeviceID][RootFolderID][FolderID] - index) + 1U);

        if (AmbaWrap_memmove(&(SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][index+1U]),
                             &(SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][index]),
                             size) != OK){
            SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
        }

        SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][index].datetime = datetime;
        AmbaUtility_StringCopy(SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][index].filename,
            sizeof(SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][index].filename),
            pFileName);
        SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][index].obj_type = obj_type;
    }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    if (isScan == 0) {
        AMBA_FS_FILE_INFO_s fileInfo;

        AmbaFS_GetFileInfo(pFileName, &fileInfo);
        SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][index].filesize = fileInfo.Size;
        SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][index].readonly = (fileInfo.Attr & AMBA_FS_ATTR_RDONLY) ? 1 : 0;
#if 1
        {
            ipc_event_s event;
            (void)AmbaWrap_memset(&event, 0, sizeof(event));
            event.event_id = NOTIFY_FILE_CREATED;
            convert_rtos_path_to_linux_impl(pFileName, event.arg.file_arg.path);
            event.arg.file_arg.file_size = fileInfo.Size;
            event.arg.file_arg.file_time = 0;
            linux_api_service_notify_event(event);
        }
#endif
    } else {
        SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][index].filesize = 0xffffffffffffffffULL;
        SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][index].readonly = 0xff;
    }
#endif
}

static void SvcDCF_DashcamUpdate(UINT8 DeviceID, UINT8 RootFolderID, UINT8 FolderID, UINT64 datetime, const char *pFileName, AMBA_DCF_FILE_TYPE_e obj_type, UINT8 isScan)
{
    UINT32 StopLoop = 0U;


#if DEBUG_DCF
    AmbaPrint_PrintUInt5("SvcDCF_DashcamUpdate DeviceID %d RootFolderID %d FolderID %d", DeviceID, RootFolderID, FolderID, 0U, 0U);
#endif

    for (UINT32 i=INDEX_1; i<CAR_DCF_MAX_CNT; i++) {
        if (SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][i].datetime == 0U) {
            /* find empty element */
            SvcDCF_DashcamInsertFileToIndex(i, DeviceID, RootFolderID, FolderID,  datetime, pFileName, obj_type, isScan);
            StopLoop = 1U;
        } else if (datetime > SvcDCF_DashcamElementTable[DeviceID][RootFolderID][FolderID][i].datetime) {
            /* file datetime > element datetime */
        } else {
            /* file datetime <= element datetime */
            SvcDCF_DashcamInsertFileToIndex(i, DeviceID, RootFolderID, FolderID,  datetime, pFileName, obj_type, isScan);
            StopLoop = 1U;
        }

        if(StopLoop == 1U){
            break;
        }
    }
}

static void SvcDCF_DashcamSacnFiles(UINT8 DriveID, UINT8 RootFolderID, UINT8 FolderID)
{
    UINT32 rval;//, ret;// i;
    AMBA_FS_DTA_t ff_dta;
    char search_str[64];
    char dirname[64];
    char filename[64];
    AMBA_DCF_FILE_TYPE_e obj_type;
    //char tmp_wild[]={'*','.','*','\0'};

    /* check fir dir-level */
    AmbaUtility_StringCopy(search_str, sizeof(search_str), &Drive[DriveID][0]);
    AmbaUtility_StringAppend(search_str, (UINT32)sizeof(search_str), colon);
    AmbaUtility_StringAppend(search_str, (UINT32)sizeof(search_str), slash);
    if(RootFolderName[RootFolderID][0U] != '0') {
        AmbaUtility_StringAppend(search_str, (UINT32)sizeof(search_str), RootFolderName[RootFolderID]);
        rval = AmbaFS_MakeDir(search_str);
        if(rval == 0x00110004U) {
            //folder is exist, and do nothing
        } else if (rval != OK) {
            SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
        } else {
            //Success to create folder
        }
        AmbaUtility_StringAppend(search_str, (UINT32)sizeof(search_str), slash);
    }
    AmbaUtility_StringAppend(search_str, (UINT32)sizeof(search_str), FolderName[FolderID]);
    rval = AmbaFS_MakeDir(search_str);
    if(rval == 0x00110004U) {
        //folder is exist, and do nothing
    } else if (rval != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    } else {
        //Success to create folder
    }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    if (FolderID == 0 || FolderID == 1) {
        AmbaUtility_StringAppend(search_str, sizeof(search_str), slash);
        AmbaUtility_StringAppend(search_str, sizeof(search_str), DCF_THUMB_DIR_NAME);
        (void)AmbaFS_MakeDir(search_str);

        AmbaUtility_StringCopy(search_str, sizeof(search_str), &Drive[DriveID][0]);
        AmbaUtility_StringAppend(search_str, sizeof(search_str), colon);
        AmbaUtility_StringAppend(search_str, sizeof(search_str), slash);
        if (RootFolderName[RootFolderID][0U] != '0') {
            AmbaUtility_StringAppend(search_str, sizeof(search_str), RootFolderName[RootFolderID]);
            (void)AmbaFS_MakeDir(search_str);
            AmbaUtility_StringAppend(search_str, sizeof(search_str), slash);
        }
        AmbaUtility_StringAppend(search_str, sizeof(search_str), FolderName[FolderID]);
        (void)AmbaFS_MakeDir(search_str);
        AmbaUtility_StringAppend(search_str, sizeof(search_str), slash);
        AmbaUtility_StringAppend(search_str, sizeof(search_str), DCF_IMU_DIR_NAME);
        (void)AmbaFS_MakeDir(search_str);

        AmbaUtility_StringCopy(search_str, sizeof(search_str), &Drive[DriveID][0]);
        AmbaUtility_StringAppend(search_str, sizeof(search_str), colon);
        AmbaUtility_StringAppend(search_str, sizeof(search_str), slash);
        if (RootFolderName[RootFolderID][0U] != '0') {
            AmbaUtility_StringAppend(search_str, sizeof(search_str), RootFolderName[RootFolderID]);
            (void)AmbaFS_MakeDir(search_str);
            AmbaUtility_StringAppend(search_str, sizeof(search_str), slash);
        }
        AmbaUtility_StringAppend(search_str, sizeof(search_str), FolderName[FolderID]);
        (void)AmbaFS_MakeDir(search_str);
        AmbaUtility_StringAppend(search_str, sizeof(search_str), slash);
        AmbaUtility_StringAppend(search_str, sizeof(search_str), DCF_GNSS_DIR_NAME);
        (void)AmbaFS_MakeDir(search_str);
    }
#endif
    /* refresh sub-dir of first-level */
    AmbaUtility_StringCopy(search_str, sizeof(search_str), &Drive[DriveID][0]);
    AmbaUtility_StringAppend(search_str, (UINT32)sizeof(search_str), colon);
    AmbaUtility_StringAppend(search_str, (UINT32)sizeof(search_str), slash);
    if(RootFolderName[RootFolderID][0U] != '0') {
        AmbaUtility_StringAppend(search_str, (UINT32)sizeof(search_str), RootFolderName[RootFolderID]);
        AmbaUtility_StringAppend(search_str, (UINT32)sizeof(search_str), slash);
    }
    AmbaUtility_StringAppend(search_str, (UINT32)sizeof(search_str), FolderName[FolderID]);
    AmbaUtility_StringAppend(search_str, (UINT32)sizeof(search_str), slash);
    AmbaUtility_StringCopy(dirname, sizeof(dirname), search_str);
    if (FolderID == 0 || FolderID == 1) {
        obj_type = AMBA_DCF_FILE_TYPE_VIDEO;
        AmbaUtility_StringAppend(search_str, sizeof(search_str), "*.mp4");
    } else {    
        obj_type = AMBA_DCF_FILE_TYPE_IMAGE;
        AmbaUtility_StringAppend(search_str, sizeof(search_str), "*.jpg");
    }
#if DEBUG_DCF
    AmbaPrint_PrintStr5("search_str %s", search_str, NULL, NULL, NULL, NULL);
#endif

    rval = AmbaFS_FileSearchFirst(search_str, (UINT8)(AMBA_FS_ATTR_ARCH | AMBA_FS_ATTR_RDONLY | AMBA_FS_ATTR_NONE | AMBA_FS_ATTR_HIDDEN), &ff_dta);
    if(rval == OK_UINT32) {
        do {
#if AMBA_FS_ATTR_VOLUME != 0U
            if ((ff_dta.Attribute & (UINT32)AMBA_FS_ATTR_VOLUME) == 0U) {
#endif
                /* for exFAT (64GB SD card) */
                if ((UINT32)AmbaUtility_StringLength((char *)ff_dta.LongName) != 0U) {
                    AmbaUtility_StringCopy((char *)ff_dta.FileName, sizeof(ff_dta.LongName), (char *)ff_dta.LongName);
                }

                obj_type = SvcDCF_DashcamCheckFileType((char *)ff_dta.FileName);

                AmbaUtility_StringCopy(filename, sizeof(filename), dirname);
                AmbaUtility_StringAppend(filename, (UINT32)sizeof(filename), (char *)ff_dta.FileName);

#if DEBUG_DCF
                AmbaPrint_PrintStr5("SvcDCF_DashcamSacnFiles filename %s", filename, NULL, NULL, NULL, NULL);
#endif

                if (obj_type < AMBA_DCF_NUM_FILE_TYPE) {
                    SvcDCF_DashcamUpdate(DriveID, RootFolderID, FolderID, SvcDCF_DashcamFnameToDatetime(filename), filename, obj_type, 1);
                }
#if AMBA_FS_ATTR_VOLUME != 0U
            }
#endif

            rval = AmbaFS_FileSearchNext(&ff_dta);

        } while (rval == OK_UINT32);
    }
}

static UINT32 SvcDCF_DashcamFoundIdxByFilanme(UINT8 DriveID, UINT8 RootFolderID, UINT8 FolderID, const char *pFileName)
{
    UINT32 rval = OK_UINT32;

    /* find from first */
    for (UINT32 i = INDEX_1; i <= SvcDCF_DashcamElementCnt[DriveID][RootFolderID][FolderID]; i++) {
        if(AmbaUtility_StringCompare(pFileName, SvcDCF_DashcamElementTable[DriveID][RootFolderID][FolderID][i].filename, (UINT32)AmbaUtility_StringLength(pFileName)) == OK_INT32) {
            rval = i;
            break;
        }
    }

    return rval;
}

static UINT32 SvcDCF_DashcamDelElemByIdx(UINT8 DriveID, UINT8 RootFolderID, UINT8 FolderID, UINT32 Index)
{
    UINT32 rval = OK_UINT32;
    UINT32 size;

    if (SvcDCF_DashcamElementCnt[DriveID][RootFolderID][FolderID] == 0U) {
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        if (Index == INDEX_0) {
            rval = NG_UINT32;
        }
    }

    if(rval != NG_UINT32) {
        if (Index <= CAR_DCF_MAX_CNT) {
            if(Index == SvcDCF_DashcamElementCnt[DriveID][RootFolderID][FolderID]){
                /* Delete element on latest index */
                AmbaPrint_ModulePrintUInt5(AMBA_DCF_PRINT_MODULE_ID, "Delete element on latest index %d", Index, 0U, 0U, 0U, 0U);
                if(AmbaWrap_memset(&(SvcDCF_DashcamElementTable[DriveID][RootFolderID][FolderID][Index]), 0, sizeof(SvcDCF_DashcamRefEmem_t)) != OK) {
                    SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
                }
                SvcDCF_DashcamElementCnt[DriveID][RootFolderID][FolderID] --;
                rval = OK_UINT32;
            } else {
                /* Delete element on index */
                AmbaPrint_ModulePrintUInt5(AMBA_DCF_PRINT_MODULE_ID, "Delete element on index %d", Index, 0U, 0U, 0U, 0U);
                size = (UINT32)sizeof(SvcDCF_DashcamRefEmem_t) * (SvcDCF_DashcamElementCnt[DriveID][RootFolderID][FolderID] - Index);

                if(AmbaWrap_memmove(&(SvcDCF_DashcamElementTable[DriveID][RootFolderID][FolderID][Index]),
                                    &(SvcDCF_DashcamElementTable[DriveID][RootFolderID][FolderID][Index+1U]),
                                    size) != OK ){
                    SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
                }

                //clear latest element value
                if(AmbaWrap_memset(&(SvcDCF_DashcamElementTable[DriveID][RootFolderID][FolderID][SvcDCF_DashcamElementCnt[DriveID][RootFolderID][FolderID]]),
                                   0,
                                   sizeof(SvcDCF_DashcamRefEmem_t)) != OK) {
                    SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
                }

                SvcDCF_DashcamElementCnt[DriveID][RootFolderID][FolderID] --;
                rval = OK_UINT32;
            }
        }
    }

    return rval;
}

UINT32 SvcDCF_DashcamConfigDrive(UINT8 DriveID, const char * pRootDrive)
{
    Drive[DriveID][0] = pRootDrive[0];

    return OK_UINT32;
}

#if 0
UINT32 SvcDCF_DashcamConfigRootFolder(UINT8 RootFolderID, const char *pFolderName)
{
    AmbaUtility_StringCopy(RootFolderName[RootFolderID], FOLDER_LEN, pFolderName);

    return OK_UINT32;
}
#endif


UINT32 SvcDCF_DashcamConfigFolder(UINT8 FolderID, const char *pFolderName)
{
    AmbaUtility_StringCopy(FolderName[FolderID], FOLDER_LEN, pFolderName);

    return OK_UINT32;
}

UINT32 SvcDCF_DashcamInit(UINT8 DriveID, AMBA_DCF_MOV_FMT_e MovFmt)
{
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    char FolderName_Normal[] = {DCF_VIDEO_DIR_NAME};
    char FolderName_Event[]  = {DCF_EVENT_DIR_NAME};
    char FolderName_Photo[]  = {DCF_PHOTO_DIR_NAME};
#else
    char FolderName_Normal[] = {"NORMAL"};
    char FolderName_Event[]  = {"EVENT"};
#endif
    char video_mov[]         = {".mov"};
    char video_mp4[]         = {".mp4"};

    #if 0
    AmbaUtility_StringCopy(RootFolderName[0U], FOLDER_LEN, "FRONT_CAMERA");
    AmbaUtility_StringCopy(RootFolderName[1U], FOLDER_LEN, "REAR_CAMERA");
    #endif

    if (SvcDCF_DashcamConfigFolder(0U, FolderName_Normal) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }

    if (SvcDCF_DashcamConfigFolder(1U, FolderName_Event) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    if (SvcDCF_DashcamConfigFolder(2U, FolderName_Photo) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
#endif
    /* create mutex */
    if (AmbaKAL_MutexCreate(&DCFInfo[DriveID].MutexID, NULL) != OK_UINT32) {
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Can not create mutex", NULL, NULL, NULL, NULL, NULL);
    }

    if (AmbaWrap_memset(SvcDCF_DashcamElementTable, 0, sizeof(SvcDCF_DashcamElementTable)) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }

    /* init table link */
    if(MovFmt == AMBA_DCF_MOV_FMT_MOV){
        AmbaUtility_StringCopy(video, sizeof(video), video_mov);
    } else {
        AmbaUtility_StringCopy(video, sizeof(video), video_mp4);
    }

    for (UINT32 i=0U; i<(UINT32)AMBA_DCF_DRIVER_MAX_NUM; i++) {
        for (UINT32 j=0U; j<(UINT32)AMBA_DCF_FOLDER_MAX_NUM; j++) {
            for (UINT32 k=0U; k<(UINT32)AMBA_DCF_FOLDER_MAX_NUM; k++) {
                SvcDCF_DashcamElementCnt[i][j][k] = 0U;
            }
        }
    }

    #if 0
    if (AmbaKAL_MutexGive(&DCFInfo[DriveID].MutexID) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
    #endif

    return OK_UINT32;
}

UINT32 SvcDCF_DashcamDestroy(UINT8 DriveID)
{
    if(DriveID == 0U) {
    }

    /* delete mutex */
    if (AmbaKAL_MutexDelete(&DCFInfo[DriveID].MutexID) != OK_UINT32) {
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Can not delete mutex", NULL, NULL, NULL, NULL, NULL);
    }

    return OK_UINT32;
}

UINT32 SvcDCF_DashcamScanRootStep1(UINT8 DriveID, UINT32 *pLastValidDirNo)
{
    UINT8 FolderID;
#if DEBUG_DCF
    char AdvStrBuf[200];
    UINT32 CurStrLen;
#endif

    *pLastValidDirNo = 0U;

    for (FolderID = (UINT8)AMBA_DCF_FOLDER_0; FolderID < (UINT8)AMBA_DCF_FOLDER_MAX_NUM; FolderID ++) {
        if(FolderName[FolderID][0U] != '0'){
            SvcDCF_DashcamSacnFiles(DriveID, 0U, FolderID);
#if DEBUG_DCF
            for (UINT32 i=INDEX_1; i<=SvcDCF_DashcamElementCnt[DriveID][0U][FolderID]; i++) {

                (void)AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf));
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "[DCF_DEBUG]: index ");
                AmbaUtility_StringAppendUInt32(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), i, 10U);
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), " datetime ");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    (void)AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            SvcDCF_DashcamElementTable[DriveID][0U][FolderID][i].datetime, 10U);
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), " filename ");
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), SvcDCF_DashcamElementTable[DriveID][0U][FolderID][i].filename);

                AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
            }
#endif
        }
    }


    return OK_UINT32;
}

UINT32 SvcDCF_DashcamScanRootStep2(UINT8 DriveID, UINT32 LastValidDirNo)
{
    UINT8 RootFolderID;
    UINT8 FolderID;
#if DEBUG_DCF
    char AdvStrBuf[200];
    UINT32 CurStrLen;
#endif

    if(LastValidDirNo == 0U) {
    }

    for (RootFolderID = 1U; RootFolderID < (UINT8)AMBA_DCF_FOLDER_MAX_NUM; RootFolderID ++) {
        if(RootFolderName[RootFolderID][0U] != '0'){
            for (FolderID = (UINT8)AMBA_DCF_FOLDER_0; FolderID < (UINT8)AMBA_DCF_FOLDER_MAX_NUM; FolderID ++) {
                if(FolderName[FolderID][0U] != '0'){
                    SvcDCF_DashcamSacnFiles(DriveID, RootFolderID, FolderID);
#if DEBUG_DCF
                    for (UINT32 i=INDEX_1; i<=SvcDCF_DashcamElementCnt[DriveID][RootFolderID][FolderID]; i++) {

                        (void)AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf));
                        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "[DCF_DEBUG]: index ");
                        AmbaUtility_StringAppendUInt32(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), i, 10U);
                        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), " datetime ");
                        CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                        if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                            (void)AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                                    SvcDCF_DashcamElementTable[DriveID][RootFolderID][FolderID][i].datetime, 10U);
                        }
                        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), " filename ");
                        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), SvcDCF_DashcamElementTable[DriveID][RootFolderID][FolderID][i].filename);

                        AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
                    }
#endif
                }
            }
        }
    }

    return OK_UINT32;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static char piv_path[4][64] = {0};

const char *SvcDCF_DashcamGetPivPath(UINT32 streamid)
{
    return piv_path[streamid];
}

typedef struct _video_split_time_s_ {
    int updated;
    AMBA_RTC_DATE_TIME_s time;
} video_split_time_s;
static video_split_time_s VideoSplitTime[CONFIG_ICAM_MAX_REC_STRM] = {0};
void SvcDCF_DashcamUpdateVideoSplitTime(UINT32 streamid, AMBA_RTC_DATE_TIME_s time)
{
    if (streamid >= CONFIG_ICAM_MAX_REC_STRM) {
        return;
    }
    VideoSplitTime[streamid].time = time;
    VideoSplitTime[streamid].updated = 1;
}
#endif

UINT32 SvcDCF_DashcamCreateFileName(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, char *pFileName, UINT32 vinid, UINT32 StreamId)
{
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    char dash[2] = {'-','\0'};
#endif
    char image[5]={'.','j','p','g','\0'};
    char fn[64];
    char temp_fn[64];
    AMBA_RTC_DATE_TIME_s Time;
    UINT8 TempRootFolderID = FolderID >> 4U;
    UINT8 TempFolderID = FolderID & 0x0FU;

#if DEBUG_DCF
    AmbaPrint_PrintUInt5("SvcDCF_DashcamCreateFileName FolderID %d", FolderID, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("TempRootFolderID %d TempFolderID %d", TempRootFolderID, TempFolderID, 0U, 0U, 0U);
#endif

    if (AmbaKAL_MutexTake(&DCFInfo[DriveID].MutexID, AMBA_KAL_WAIT_FOREVER) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }

    AmbaUtility_StringCopy(fn, sizeof(fn), &Drive[DriveID][0]);
    AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), colon);
    AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), slash);
    if(RootFolderName[TempRootFolderID][0U] != '0'){
        AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), RootFolderName[TempRootFolderID]);
        AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), slash);
    }
    AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), FolderName[TempFolderID]);
    AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), slash);

    /* default create file */
    /** Correct RTC time */
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    if (VideoSplitTime[StreamId].updated) {
        Time = VideoSplitTime[StreamId].time;
    } else {
        if (AmbaRTC_GetSysTime(&Time) != OK) {
            SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
        }
    }
#else
    if (AmbaRTC_GetSysTime(&Time) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
#endif
#if 0//DEBUG_DCF
    AmbaPrint_PrintUInt5("date %04d %02d %02d", Time.Year, Time.Month, Time.Day, 0U, 0U);
    AmbaPrint_PrintUInt5("time %02d %02d %02d", Time.Hour, Time.Minute, Time.Second, 0U, 0U);
#endif
    //date-
    if (AmbaUtility_StringPrintUInt32(temp_fn, (UINT32)sizeof(temp_fn), "%04u", 1, &Time.Year) != 4U) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
    AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), temp_fn);
    if (AmbaUtility_StringPrintUInt32(temp_fn, (UINT32)sizeof(temp_fn), "%02u", 1, &Time.Month) != 2U) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
    AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), temp_fn);
    if (AmbaUtility_StringPrintUInt32(temp_fn, (UINT32)sizeof(temp_fn), "%02u", 1, &Time.Day) != 2U) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
    AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), temp_fn);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    AmbaUtility_StringAppend(fn, sizeof(fn), "_");
#endif

    //time-
    if (AmbaUtility_StringPrintUInt32(temp_fn, (UINT32)sizeof(temp_fn), "%02u", 1, &Time.Hour) != 2U) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
    AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), temp_fn);
    if (AmbaUtility_StringPrintUInt32(temp_fn, (UINT32)sizeof(temp_fn), "%02u", 1, &Time.Minute) != 2U) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
    AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), temp_fn);
    if (AmbaUtility_StringPrintUInt32(temp_fn, (UINT32)sizeof(temp_fn), "%02u", 1, &Time.Second) != 2U) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
    AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), temp_fn);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    if (StreamId == 0) {
        AmbaUtility_StringAppend(fn, sizeof(fn), "A");
    } else if (StreamId == 1) {
        AmbaUtility_StringAppend(fn, sizeof(fn), "C");
    } else if (StreamId == 2) {
        AmbaUtility_StringAppend(fn, sizeof(fn), "B");
    } else if (StreamId == 3) {
        AmbaUtility_StringAppend(fn, sizeof(fn), "D");
    }
    if (FolderID == 1) {
        AmbaUtility_StringAppend(fn, sizeof(fn), "_SOS");
    }
#else
    AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), dash);

    //vinid-
    if (AmbaUtility_StringPrintUInt32(temp_fn, (UINT32)sizeof(temp_fn), "%02u", 1, &vinid) != 2U) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
    AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), temp_fn);
    AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), dash);

    //StreamId
    if (AmbaUtility_StringPrintUInt32(temp_fn, (UINT32)sizeof(temp_fn), "%02u", 1, &StreamId) != 2U) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
    AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), temp_fn);
#endif
    //.file type
    if(FileType == AMBA_DCF_FILE_TYPE_VIDEO){
        AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), video);
    } else {
        AmbaUtility_StringAppend(fn, (UINT32)sizeof(fn), image);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        AmbaWrap_memset(piv_path[StreamId], 0, sizeof(piv_path[StreamId]));
        AmbaUtility_StringCopy(piv_path[StreamId], sizeof(fn), fn);
#endif
    }

    AmbaUtility_StringCopy(pFileName, sizeof(fn), fn);

#if 0//DEBUG_DCF
    AmbaPrint_PrintStr5("SvcDCF_DashcamCreateFileName filename %s", pFileName, NULL, NULL, NULL, NULL);
#endif

    if (AmbaKAL_MutexGive(&DCFInfo[DriveID].MutexID) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }

    return OK_UINT32;
}

UINT32 SvcDCF_DashcamUpdateFile(const char *pFileName)
{
    UINT8 DriveID = 0U, RootFolderID = 0U, FolderID = 0U;
    UINT64 datetime = 0U;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    AMBA_DCF_FILE_TYPE_e obj_type;

    obj_type = SvcDCF_DashcamCheckFileType(pFileName);
#endif

    if(SvcDCF_DashcamGetDriveID(&DriveID, pFileName) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (SvcDCF_DashcamGetRootFolderID(&RootFolderID, pFileName) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (SvcDCF_DashcamGetFolderID(&FolderID, pFileName) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }

    if (AmbaKAL_MutexTake(&DCFInfo[DriveID].MutexID, AMBA_KAL_WAIT_FOREVER) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
    datetime = SvcDCF_DashcamFnameToDatetime(pFileName);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    SvcDCF_DashcamUpdate(DriveID, RootFolderID, FolderID, datetime, pFileName, obj_type, 0);
#else
    SvcDCF_DashcamUpdate(DriveID, RootFolderID, FolderID, datetime, pFileName, AMBA_DCF_FILE_TYPE_VIDEO);
#endif
    if (AmbaKAL_MutexGive(&DCFInfo[DriveID].MutexID) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }

#if DEBUG_DCF
    {
        char AdvStrBuf[200];
        UINT32 CurStrLen;

        for (UINT32 i=INDEX_1; i<=SvcDCF_DashcamElementCnt[DriveID][RootFolderID][FolderID]; i++) {
            // [DCF_DEBUG]: index %d datetime %llu filename %s

            (void)AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf));
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "[DCF_DEBUG]: index ");
            AmbaUtility_StringAppendUInt32(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), i, 10U);
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), " datetime ");
            CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
            if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                (void)AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                        SvcDCF_DashcamElementTable[DriveID][RootFolderID][FolderID][i].datetime, 10U);
            }
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), " filename ");
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), SvcDCF_DashcamElementTable[DriveID][RootFolderID][FolderID][i].filename);

            AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
        }
        AmbaPrint_Flush();
    }
#endif

    return OK_UINT32;
}


#if defined(CONFIG_APP_FLOW_CARDV_AONI)
UINT32 SvcDCF_DashcamSetFileReadOnly(const char *pFileName, UINT8 readonly)
{
    UINT32 found;
    UINT8 DriveID, FolderID;
    UINT8 RootFolderID = 0;

    (void)SvcDCF_DashcamGetDriveID(&DriveID, pFileName);
    (void)AmbaKAL_MutexTake(&DCFInfo[DriveID].MutexID, AMBA_KAL_WAIT_FOREVER);
    (void)SvcDCF_DashcamGetRootFolderID(&RootFolderID, pFileName);
    (void)SvcDCF_DashcamGetFolderID(&FolderID, pFileName);
    found = SvcDCF_DashcamFoundIdxByFilanme(DriveID, RootFolderID, FolderID, pFileName);
    (void)AmbaKAL_MutexGive(&DCFInfo[DriveID].MutexID);
    if (found != 0U) {
        AMBA_FS_FILE_INFO_s fileInfo;

        if (SvcDCF_DashcamElementTable[DriveID][RootFolderID][FolderID][found].readonly == readonly) {
            return OK_UINT32;
        }
        if (AmbaFS_GetFileInfo(pFileName, &fileInfo) == AMBA_FS_ERR_NONE) {
            if (readonly) {
                fileInfo.Attr |= AMBA_FS_ATTR_RDONLY;
            } else {
                fileInfo.Attr &= ~AMBA_FS_ATTR_RDONLY;
            }
            if (AmbaFS_ChangeFileMode(pFileName, fileInfo.Attr) == AMBA_FS_ERR_NONE) {
                SvcDCF_DashcamElementTable[DriveID][RootFolderID][FolderID][found].readonly = readonly;
                return OK_UINT32;
            }
        }
    }

    return NG_UINT32;
}

UINT32 SvcDCF_DashcamScanFileAttr(void)
{
    UINT32 FolderID = 0, index = 0;

    for (FolderID = AMBA_DCF_FOLDER_0; FolderID <= AMBA_DCF_FOLDER_MAX_NUM; FolderID++) {
        for (index = INDEX_1; index < (CAR_DCF_MAX_CNT + 1U); index++) {
            if (SvcDCF_DashcamElementTable[0][0][FolderID][index].readonly == 0xff) {
                AMBA_FS_FILE_INFO_s fileInfo;
                if (AmbaFS_GetFileInfo(SvcDCF_DashcamElementTable[0][0][FolderID][index].filename, &fileInfo) == AMBA_FS_ERR_NONE) {
                    SvcDCF_DashcamElementTable[0][0][FolderID][index].filesize = fileInfo.Size;
                    SvcDCF_DashcamElementTable[0][0][FolderID][index].readonly = (fileInfo.Attr & AMBA_FS_ATTR_RDONLY) ? 1 : 0;
                }
                AmbaKAL_TaskSleep(100);
            }
        }
    }

    return 0;
}
#endif

UINT32 SvcDCF_DashcamDeleteFile(const char *pFileName)
{
    UINT32 found, Rval = NG_UINT32;
    UINT8 DriveID = 0U, RootFolderID = 0U, FolderID = 0U;

    if (SvcDCF_DashcamGetDriveID(&DriveID, pFileName) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (SvcDCF_DashcamGetRootFolderID(&RootFolderID, pFileName) != OK ) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (SvcDCF_DashcamGetFolderID(&FolderID, pFileName) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }

#if 1//DEBUG_DCF
    AmbaPrint_PrintStr5("SvcDCF_DashcamDeleteFile filename %s", pFileName, NULL, NULL, NULL, NULL);
#endif

    if (AmbaKAL_MutexTake(&DCFInfo[DriveID].MutexID, AMBA_KAL_WAIT_FOREVER) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }

    found = SvcDCF_DashcamFoundIdxByFilanme(DriveID, RootFolderID, FolderID, pFileName);
    if (found != 0U) {
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        UINT32 startTick = 0, endTick = 0;
        AmbaKAL_GetSysTickCount(&startTick);
        // remove main file
        if (AmbaFS_Remove(pFileName) == AMBA_FS_ERR_NONE) {
            AMBA_DCF_FILE_TYPE_e obj_type;
            AmbaPrint_PrintStr5("Remove %s ok", pFileName, NULL, NULL, NULL, NULL);
            (void)SvcDCF_DashcamDelElemByIdx(DriveID, RootFolderID, FolderID, found);
            Rval = OK_UINT32;
            obj_type = SvcDCF_DashcamCheckFileType(pFileName);
            if (obj_type == AMBA_DCF_FILE_TYPE_VIDEO) {
                char Path[64] = {0};
                unsigned int i = 0;
                for (i = (AmbaUtility_StringLength(pFileName) - 1U); i >= 2U; i--) {
                    if (pFileName[i] == '\\') {                            
                        // remove thumbnail
                        (void)AmbaWrap_memset(Path, 0, sizeof(Path));
                        AmbaWrap_memcpy(Path, pFileName, i + 1);
                        AmbaUtility_StringAppend(Path, sizeof(Path), DCF_THUMB_DIR_NAME);
                        AmbaUtility_StringAppend(Path, sizeof(Path), "\\");
                        AmbaWrap_memcpy(Path + AmbaUtility_StringLength(Path), pFileName + i + 1, AmbaUtility_StringLength(pFileName) - (i + 1) - 4);
                        AmbaUtility_StringAppend(Path, sizeof(Path), "_net.jpg");
                        AmbaKAL_GetSysTickCount(&endTick);
                        if (endTick - startTick > 100) {
                            AmbaPrint_PrintStr5("delete file long", NULL, NULL, NULL, NULL, NULL);
                            AmbaKAL_TaskSleep(1000);
                        }
                        AmbaKAL_GetSysTickCount(&startTick);
                        if (AmbaFS_Remove(Path) == AMBA_FS_ERR_NONE) {
                            AmbaPrint_PrintStr5("Remove %s ok", Path, NULL, NULL, NULL, NULL);
                        } else {
                            AmbaPrint_PrintStr5("Remove %s failed", Path, NULL, NULL, NULL, NULL);
                        }
                        // remove imu dat file
                        (void)AmbaWrap_memset(Path, 0, sizeof(Path));
                        AmbaWrap_memcpy(Path, pFileName, i + 1);
                        AmbaUtility_StringAppend(Path, sizeof(Path), DCF_IMU_DIR_NAME);
                        AmbaUtility_StringAppend(Path, sizeof(Path), "\\");
                        AmbaWrap_memcpy(Path + AmbaUtility_StringLength(Path), pFileName + i + 1, AmbaUtility_StringLength(pFileName) - (i + 1) - 4);
                        AmbaUtility_StringAppend(Path, sizeof(Path), ".dat");
                        AmbaKAL_GetSysTickCount(&endTick);
                        if (endTick - startTick > 100) {                            
                            AmbaPrint_PrintStr5("delete file long", NULL, NULL, NULL, NULL, NULL);
                            AmbaKAL_TaskSleep(1000);
                        }
                        AmbaKAL_GetSysTickCount(&startTick);
                        if (AmbaFS_Remove(Path) == AMBA_FS_ERR_NONE) {
                            AmbaPrint_PrintStr5("Remove %s ok", Path, NULL, NULL, NULL, NULL);
                        } else {
                            AmbaPrint_PrintStr5("Remove %s failed", Path, NULL, NULL, NULL, NULL);
                        }
                        // remove gnss dat file
                        (void)AmbaWrap_memset(Path, 0, sizeof(Path));
                        AmbaWrap_memcpy(Path, pFileName, i + 1);
                        AmbaUtility_StringAppend(Path, sizeof(Path), DCF_GNSS_DIR_NAME);
                        AmbaUtility_StringAppend(Path, sizeof(Path), "\\");
                        AmbaWrap_memcpy(Path + AmbaUtility_StringLength(Path), pFileName + i + 1, AmbaUtility_StringLength(pFileName) - (i + 1) - 4);
                        AmbaUtility_StringAppend(Path, sizeof(Path), ".txt");
                        AmbaKAL_GetSysTickCount(&endTick);
                        if (endTick - startTick > 100) {
                            AmbaPrint_PrintStr5("delete file long", NULL, NULL, NULL, NULL, NULL);
                            AmbaKAL_TaskSleep(1000);
                        }
                        if (AmbaFS_Remove(Path) == AMBA_FS_ERR_NONE) {
                            AmbaPrint_PrintStr5("Remove %s ok", Path, NULL, NULL, NULL, NULL);
                        } else {
                            AmbaPrint_PrintStr5("Remove %s failed", Path, NULL, NULL, NULL, NULL);
                        }
                        break;
                    }
                }
            }
#if 1
            {
                ipc_event_s event;
                (void)AmbaWrap_memset(&event, 0, sizeof(event));
                event.event_id = NOTIFY_FILE_DELETED;
                convert_rtos_path_to_linux_impl(pFileName, event.arg.file_arg.path);
                event.arg.file_arg.file_size = 0;//fileInfo.Size;
                event.arg.file_arg.file_time = 0;
                linux_api_service_notify_event(event);
            }
#endif
        } else {
            AmbaPrint_PrintStr5("Remove %s failed", pFileName, NULL, NULL, NULL, NULL);
        }
#else
        if (SvcDCF_DashcamDelElemByIdx(DriveID, RootFolderID, FolderID, found) != OK) {
            SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
        }
        if (AmbaFS_Remove(pFileName) != OK) {
            SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
        }
#endif
    }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    else {
        AmbaPrint_PrintStr5("Not Found %s", pFileName, NULL, NULL, NULL, NULL);
    }
#endif

    if (AmbaKAL_MutexGive(&DCFInfo[DriveID].MutexID) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }

#if DEBUG_DCF
    {
        char AdvStrBuf[200];
        UINT32 CurStrLen;

        for (UINT32 i=INDEX_1; i<=SvcDCF_DashcamElementCnt[DriveID][RootFolderID][FolderID]; i++) {

            // [DCF_DEBUG]: index %d datetime %llu filename %s
            (void)AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf));
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "[DCF_DEBUG]: index ");
            AmbaUtility_StringAppendUInt32(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), i, 10U);
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), " datetime ");
            CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
            if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                (void)AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                        SvcDCF_DashcamElementTable[DriveID][RootFolderID][FolderID][i].datetime, 10U);
            }
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), " filename ");
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), SvcDCF_DashcamElementTable[DriveID][RootFolderID][FolderID][i].filename);

            AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
        }
        AmbaPrint_Flush();
    }
#endif

    return Rval;
}

UINT32 SvcDCF_DashcamGetFileName(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, char *pFileName, UINT32 Index)
{
    const char  *pName;
    UINT32      Rval = NG_UINT32;
    UINT8 TempRootFolderID = FolderID >> 4U;
    UINT8 TempFolderID = FolderID & 0x0FU;

    if (AmbaKAL_MutexTake(&DCFInfo[DriveID].MutexID, AMBA_KAL_WAIT_FOREVER) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }

    if (FileType < AMBA_DCF_FILE_TYPE_ALL) {
        pName = SvcDCF_DashcamElementTable[DriveID][TempRootFolderID][TempFolderID][Index].filename;
        if (0U == (UINT32)AmbaUtility_StringLength(pName)) {
        } else {
            DcfDashcam_strcpy(pFileName, pName);
            Rval = OK_UINT32;
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_DCF_PRINT_MODULE_ID, "FileType error", 0U, 0U, 0U, 0U, 0U);
    }

    if (AmbaKAL_MutexGive(&DCFInfo[DriveID].MutexID) != OK) {
        SvcDcf_DashcamNG(__func__, "Line %d", __LINE__, 0U);
    }

    return Rval;
}

UINT32 SvcDCF_DashcamGetFileAmount(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, UINT32 *pFileAmount)
{
    UINT8 TempRootFolderID = FolderID >> 4U;
    UINT8 TempFolderID = FolderID & 0x0FU;

    if(FileType >= AMBA_DCF_FILE_TYPE_ALL){
        AmbaPrint_ModulePrintUInt5(AMBA_DCF_PRINT_MODULE_ID, "FileType error", 0U, 0U, 0U, 0U, 0U);
    }

    *pFileAmount = SvcDCF_DashcamElementCnt[DriveID][TempRootFolderID][TempFolderID];

    return OK_INT32;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
SvcDCF_DashcamRefEmem_t *SvcDCF_DashcamGetFileList(UINT8 DriveID)
{
    return &SvcDCF_DashcamElementTable[DriveID][0][0][0];
}
#endif

