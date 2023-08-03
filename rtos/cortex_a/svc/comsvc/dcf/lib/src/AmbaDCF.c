/**
*  @file AmbaDCF.c
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
#include "AmbaTypes.h"
#include "AmbaDCF.h"
#include "AmbaDCF_Camera.h"
#include "AmbaDCF_Dashcam.h"

static UINT32 rtval GNU_SECTION_NOZEROINIT;
static UINT8 DcfFormatId GNU_SECTION_NOZEROINIT;

/**
 * Config DCF format
 * @param [in] Format DCF format
 */
void AmbaDCF_ConfigFormat(UINT8 Format)
{
    DcfFormatId = Format;
}

/**
 * Config storage driver character
 * @param [in] DriveID driver ID
 * @param [in] *pRootDrive pointer to driver character
 * @return ErrorCode
 */
UINT32 AmbaDCF_ConfigDrive(UINT8 DriveID, const char * pRootDrive)
{
    if(DcfFormatId == DCF_DASHCAM) {
        rtval = SvcDCF_DashcamConfigDrive(DriveID, pRootDrive);
    } else if (DcfFormatId == DCF_CAMERA) {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        rtval = SvcDCF_CameraConfigDrive(DriveID, pRootDrive);
#endif
    } else {
        //pass vcast
    }

    return rtval;
}

/**
 * Config DCF folder name
 * @param [in] FolderID folder ID
 * @param [in] *pFolderName pointer to folder name
 * @return ErrorCode
 */
UINT32 AmbaDCF_ConfigFolder(UINT8 FolderID, const char *pFolderName)
{
    if(DcfFormatId == DCF_DASHCAM) {
        rtval = SvcDCF_DashcamConfigFolder(FolderID, pFolderName);
    } else if(DcfFormatId == DCF_CAMERA) {
        //rtval = SvcDCF_CameraConfigFolder(FolderID, pFolderName);
    } else {
        //pass vcast
    }

    return rtval;
}

/**
 * DCF init
 * @param [in] DriveID driver ID
 * @param [in] MovFmt DCF video format
 * @return ErrorCode
 */
UINT32 AmbaDCF_Init(UINT8 DriveID, AMBA_DCF_MOV_FMT_e MovFmt)
{
    if(DcfFormatId == DCF_DASHCAM) {
        rtval = SvcDCF_DashcamInit(DriveID, MovFmt);
    } else if(DcfFormatId == DCF_CAMERA) {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        rtval = SvcDCF_CameraInit(DriveID, MovFmt);
#endif
    } else {
        //pass vcast
    }

    return rtval;
}

/**
 * DCF scan step 1
 * @param [in]  DriveID driver ID
 * @param [out] *pLastValidDirNo pointer to CameraDCF MAX valid director
 * @return ErrorCode
 */
UINT32 AmbaDCF_ScanRootStep1(UINT8 DriveID, UINT32 *pLastValidDirNo)
{
    if(DcfFormatId == DCF_DASHCAM) {
        rtval = SvcDCF_DashcamScanRootStep1(DriveID, pLastValidDirNo);
    } else if(DcfFormatId == DCF_CAMERA) {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        rtval = SvcDCF_CameraScanRootStep1(DriveID, pLastValidDirNo);
#endif
    } else {
        //pass vcast
    }

    return rtval;
}

/**
 * DCF scan step 2
 * @param [in]  DriveID driver ID
 * @param [in]  LastValidDirNo CameraDCF MAX valid director
 * @return ErrorCode
 */
UINT32 AmbaDCF_ScanRootStep2(UINT8 DriveID, UINT32 LastValidDirNo)
{
    if(DcfFormatId == DCF_DASHCAM) {
        rtval = SvcDCF_DashcamScanRootStep2(DriveID, LastValidDirNo);
    } else if(DcfFormatId == DCF_CAMERA) {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        rtval = SvcDCF_CameraScanRootStep2(DriveID, LastValidDirNo);
#endif
    } else {
        //pass vcast
    }

    return rtval;
}

/**
 * DCF destroy
 * @param [in]  DriveID driver ID
 * @return ErrorCode
 */
UINT32 AmbaDCF_Destroy(UINT8 DriveID)
{
    if(DcfFormatId == DCF_DASHCAM) {
        rtval = SvcDCF_DashcamDestroy(DriveID);
    } else if(DcfFormatId == DCF_CAMERA) {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        rtval = SvcDCF_CameraDestroy(DriveID);
#endif
    } else {
        //pass vcast
    }

    return rtval;
}

/**
 * get a new valid DCF name
 * @param [in]  DriveID driver ID
 * @param [in]  FolderID folder ID
 * @param [in]  FileType DCF data format
 * @param [out] *pFileName pointer to filename
 * @param [in]  vinid source VIN ID
 * @param [in]  StreamId source stream ID
 * @return ErrorCode
 */
UINT32 AmbaDCF_CreateFileName(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, char *pFileName, UINT32 vinid, UINT32 StreamId)
{
    if(DcfFormatId == DCF_DASHCAM) {
        rtval = SvcDCF_DashcamCreateFileName(DriveID, FolderID, FileType, pFileName, vinid, StreamId);
    } else if(DcfFormatId == DCF_CAMERA) {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        rtval = SvcDCF_CameraCreateFileName(DriveID, FolderID, FileType, pFileName, vinid, StreamId);
#endif
    } else {
        //pass vcast
    }

    return rtval;
}

/**
 * update a filename to DCF system
 * @param [in] *pFileName pointer to filename
 * @return ErrorCode
 */
UINT32 AmbaDCF_UpdateFile(const char *pFileName)
{
    if(DcfFormatId == DCF_DASHCAM) {
        rtval = SvcDCF_DashcamUpdateFile(pFileName);
    } else if(DcfFormatId == DCF_CAMERA) {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        rtval = SvcDCF_CameraUpdateFile(pFileName);
#endif
    } else {
        //pass vcast
    }

    return rtval;
}

/**
 * delete a filename from DCF system
 * @param [in] *pFileName pointer to filename
 * @return ErrorCode
 */
UINT32 AmbaDCF_DeleteFile(const char *pFileName)
{
    if(DcfFormatId == DCF_DASHCAM) {
        rtval = SvcDCF_DashcamDeleteFile(pFileName);
    } else if(DcfFormatId == DCF_CAMERA) {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        rtval = SvcDCF_CameraDeleteFile(pFileName);
#endif
    } else {
        //pass vcast
    }

    return rtval;
}

/**
 * get DCF filename from file index
 * @param [in]  DriveID driver ID
 * @param [in]  FolderID folder ID
 * @param [in]  FileType DCF data format
 * @param [out] *pFileName pointer to filename
 * @param [in]  Index DCF file index
 * @return ErrorCode
 */
UINT32 AmbaDCF_GetFileName(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, char *pFileName, UINT32 Index)
{
    if(DcfFormatId == DCF_DASHCAM) {
        rtval = SvcDCF_DashcamGetFileName(DriveID, FolderID, FileType, pFileName, Index);
    } else if(DcfFormatId == DCF_CAMERA) {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        rtval = SvcDCF_CameraGetFileName(DriveID, FolderID, FileType, pFileName, Index);
#endif
    } else {
        //pass vcast
    }

    return rtval;
}

/**
 * get file numbers format from DCF
 * @param [in]  DriveID driver ID
 * @param [in]  FolderID folder ID
 * @param [in]  FileType DCF data format
 * @param [out] *pFileAmount pointer to file amount
 * @return ErrorCode
 */
UINT32 AmbaDCF_GetFileAmount(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, UINT32 *pFileAmount)
{
    if(DcfFormatId == DCF_DASHCAM) {
        rtval = SvcDCF_DashcamGetFileAmount(DriveID, FolderID, FileType, pFileAmount);
    } else if(DcfFormatId == DCF_CAMERA) {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        rtval = SvcDCF_CameraGetFileAmount(DriveID, FolderID, FileType, pFileAmount);
#endif
    } else {
        //pass vcast
    }

    return rtval;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
const char *AmbaDCF_GetPivPath(UINT8 StreamID)
{
    if(DcfFormatId == DCF_DASHCAM) {
        return SvcDCF_DashcamGetPivPath(StreamID);
    }
    return NULL;
}

void *AmbaDCF_GetFileList(UINT8 DriveID)
{
    if(DcfFormatId == DCF_DASHCAM) {
        return SvcDCF_DashcamGetFileList(DriveID);
    }
    return NULL;
}

UINT32 AmbaDCF_SetFileReadOnly(const char *pFileName, UINT8 Readonly)
{
    if(DcfFormatId == DCF_DASHCAM) {
        return SvcDCF_DashcamSetFileReadOnly(pFileName, Readonly);
    }
    return NG_UINT32;
}

static UINT8 dcf_is_ready = 0;
UINT8 AmbaDCF_IsReady(void)
{
    return dcf_is_ready;
}

void AmbaDCF_SetReady(UINT8 ready)
{
    dcf_is_ready = ready;
}

UINT32 AmbaDCF_DashcamScanFileAttr(void)
{
    if(DcfFormatId == DCF_DASHCAM) {
        return SvcDCF_DashcamScanFileAttr();
    }
    return NG_UINT32;
}
#endif

