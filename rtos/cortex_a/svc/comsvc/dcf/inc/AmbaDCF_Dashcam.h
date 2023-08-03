/**
*  @file AmbaDCF_Dashcam.h
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

#ifndef AMBA_DCF_DASHCAM_H
#define AMBA_DCF_DASHCAM_H

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
typedef struct {
    UINT64 datetime;
    UINT64 filesize;
    UINT8 readonly;
    char filename[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    AMBA_DCF_FILE_TYPE_e obj_type;
} SvcDCF_DashcamRefEmem_t;
#endif

UINT32 SvcDCF_DashcamConfigDrive(UINT8 DriveID, const char * pRootDrive);
//UINT32 SvcDCF_DashcamConfigRootFolder(UINT8 RootFolderID, const char *pFolderName);
UINT32 SvcDCF_DashcamConfigFolder(UINT8 FolderID, const char *pFolderName);
UINT32 SvcDCF_DashcamInit(UINT8 DriveID, AMBA_DCF_MOV_FMT_e MovFmt);
UINT32 SvcDCF_DashcamScanRootStep1(UINT8 DriveID, UINT32 *pLastValidDirNo);
UINT32 SvcDCF_DashcamScanRootStep2(UINT8 DriveID, UINT32 LastValidDirNo);
UINT32 SvcDCF_DashcamDestroy(UINT8 DriveID);
UINT32 SvcDCF_DashcamCreateFileName(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, char *pFileName, UINT32 vinid, UINT32 StreamId);
UINT32 SvcDCF_DashcamUpdateFile(const char *pFileName);
UINT32 SvcDCF_DashcamDeleteFile(const char *pFileName);
UINT32 SvcDCF_DashcamGetFileName(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, char *pFileName, UINT32 Index);
UINT32 SvcDCF_DashcamGetFileAmount(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, UINT32 *pFileAmount);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
const char *SvcDCF_DashcamGetPivPath(UINT32 streamid);
SvcDCF_DashcamRefEmem_t *SvcDCF_DashcamGetFileList(UINT8 DriveID);
UINT32 SvcDCF_DashcamSetFileReadOnly(const char *pFileName, UINT8 readonly);
UINT32 SvcDCF_DashcamScanFileAttr(void);
#endif

#endif  /* AMBA_DCF_DASHCAM_H */
