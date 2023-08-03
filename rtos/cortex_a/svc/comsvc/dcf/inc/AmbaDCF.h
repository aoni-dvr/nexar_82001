/**
*  @file AmbaDCF.h
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
*  @details Definitions & Constants for DCF (Design rule for Camera File system v2.0) related APIs
*
*/

#ifndef AMBA_DCF_H
#define AMBA_DCF_H

#define AMBA_DCF_PRINT_MODULE_ID            ((UINT16)(DCF_ERR_BASE >> 16U))

#define AMBA_DCF_MAX_FILE_FULL_PATH_LEN     128U

#define OK_INT32                            0
#define NG_INT32                            -1

#define OK_UINT32                           0U
#define NG_UINT32                           1U

#define DCF_CAMERA                          0U
#define DCF_DASHCAM                         1U

typedef enum {
    AMBA_DCF_MOV_FMT_MOV = 0,
    AMBA_DCF_MOV_FMT_MP4
} AMBA_DCF_MOV_FMT_e;

typedef enum {
    AMBA_DCF_FILE_TYPE_IMAGE = 0,
    AMBA_DCF_FILE_TYPE_VIDEO,
    AMBA_DCF_FILE_TYPE_AUDIO,
    AMBA_DCF_FILE_TYPE_ALL,

    AMBA_DCF_NUM_FILE_TYPE
} AMBA_DCF_FILE_TYPE_e;

typedef enum {
    /* C/D/I drive */
    AMBA_DCF_DRIVE_0,
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    AMBA_DCF_DRIVE_1,
    AMBA_DCF_DRIVE_2,
#endif
    AMBA_DCF_DRIVER_MAX_NUM
} AMBA_DCF_DRIVE_e;

typedef enum {
    AMBA_DCF_FOLDER_0,
    AMBA_DCF_FOLDER_1,
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    AMBA_DCF_FOLDER_2,
#endif
    AMBA_DCF_FOLDER_MAX_NUM
} AMBA_DCF_FOLDER_e;

void   AmbaDCF_ConfigFormat(UINT8 Format);
UINT32 AmbaDCF_ConfigDrive(UINT8 DriveID, const char * pRootDrive);
UINT32 AmbaDCF_ConfigFolder(UINT8 FolderID, const char *pFolderName);
UINT32 AmbaDCF_Init(UINT8 DriveID, AMBA_DCF_MOV_FMT_e MovFmt);
UINT32 AmbaDCF_ScanRootStep1(UINT8 DriveID, UINT32 *pLastValidDirNo);
UINT32 AmbaDCF_ScanRootStep2(UINT8 DriveID, UINT32 LastValidDirNo);
UINT32 AmbaDCF_Destroy(UINT8 DriveID);
UINT32 AmbaDCF_CreateFileName(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, char *pFileName, UINT32 vinid, UINT32 StreamId);
UINT32 AmbaDCF_UpdateFile(const char *pFileName);
UINT32 AmbaDCF_DeleteFile(const char *pFileName);
UINT32 AmbaDCF_GetFileName(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, char *pFileName, UINT32 Index);
UINT32 AmbaDCF_GetFileAmount(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, UINT32 *pFileAmount);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
const char *AmbaDCF_GetPivPath(UINT8 StreamID);
void *AmbaDCF_GetFileList(UINT8 DriveID);
UINT32 AmbaDCF_SetFileReadOnly(const char *pFileName, UINT8 Readonly);
UINT8 AmbaDCF_IsReady(void);
void AmbaDCF_SetReady(UINT8 ready);
UINT32 AmbaDCF_DashcamScanFileAttr(void);
#endif
#endif  /* AMBA_DCF_H */
