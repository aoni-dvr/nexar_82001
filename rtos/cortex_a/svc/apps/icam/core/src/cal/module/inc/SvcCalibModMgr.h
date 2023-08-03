/**
*  @file SvcCalibModMgr.h
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
*  @details Header file for SVC Module Storage Manager
*
*/

#ifndef SVC_CALIB_MODULE_MGR_H
#define SVC_CALIB_MODULE_MGR_H

#define SVC_MODULE_VERSION                  (0x2U)

#define SVC_MODULE_MINOR_VERSION            (0x0U)

#define SVC_MODULE_PROJECT_AVM              (0x1U)
#define SVC_MODULE_PROJECT_EMR              (0x2U)

#define SVC_MODULE_ITEM_MAX_NUM             (16U)

#define SVC_MODULE_ITEM_CMD_MEM_QRY         (1U)
#define SVC_MODULE_ITEM_CMD_GET             (2U)
#define SVC_MODULE_ITEM_CMD_SET             (3U)
#define SVC_MODULE_ITEM_CMD_NUM             (4U)
typedef UINT32 (*SVC_MODULE_CMD_FUNC_f)(UINT32 CmdID, void *pData, void *pParam1, void *pParam2, void *pParam3, void *pParam4);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcModuleMgr.c
\*-----------------------------------------------------------------------------------------------*/

typedef struct {
    UINT32                ItemVersion;
    SVC_MODULE_CMD_FUNC_f pFunc;
} SVC_MODULE_ITEM_CFG_s;

typedef struct {
    UINT32                ItemVersion;
    UINT32                MinorVersion;
    UINT32                ProjectID;
    void                 *pData;
    UINT32                DataSize;
} SVC_MODULE_ITEM_DATA_INFO_s;

UINT32 SvcModule_MemQry(const AMBA_EEPROM_CHANNEL_s *pChan, UINT32 NumOfCfg, SVC_MODULE_ITEM_CFG_s *pCfg, UINT32 *pMemSize);
UINT32 SvcModule_Create(const AMBA_EEPROM_CHANNEL_s *pChan, UINT8 *pMemBuf, UINT32 MemSize, void **pModuleCtrl);
UINT32 SvcModule_Delete(void *pModuleCtrl);
UINT32 SvcModule_Config(void *pModuleCtrl, UINT32 NumOfCfg, const SVC_MODULE_ITEM_CFG_s *pCfg);
UINT32 SvcModule_Load(void *pModuleCtrl);
UINT32 SvcModule_Save(void *pModuleCtrl);
UINT32 SvcModule_Reset(void *pModuleCtrl);
UINT32 SvcModule_ItemGet(void *pModuleCtrl, SVC_MODULE_ITEM_DATA_INFO_s *pDataInfo);
UINT32 SvcModule_ItemSet(void *pModuleCtrl, SVC_MODULE_ITEM_DATA_INFO_s *pDataInfo);
void   SvcModule_Info(void *pModuleCtrl);
void   SvcModule_InfoAll(void);
void   SvcModule_DebugMsg(UINT32 Enable);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcModuleMgr.h
\*-----------------------------------------------------------------------------------------------*/
#define SVC_MODULE_ITEM_VIG                 (0x04U)
#define SVC_MODULE_ITEM_VIG_SIZE            (9UL << 10UL)

#define SVC_MODULE_ITEM_CAM_INTRINSIC       (0x01U)
#define SVC_MODULE_ITEM_WHITE_BALANCE       (0x57U)
#define SVC_MODULE_ITEM_LDC_CURVE           (0x02U)
#define SVC_MODULE_ITEM_CA_CURVE            (0x03U)

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcModuleBpc.c
\*-----------------------------------------------------------------------------------------------*/
#define SVC_MODULE_ITEM_BPC                 ( 0x05U )
#define SVC_MODULE_ITEM_BPC_MAX_W           ( 1952UL )
#define SVC_MODULE_ITEM_BPC_MAX_H           ( 1108UL )
#define SVC_MODULE_ITEM_BPC_MAX_P           ( ( ( SVC_MODULE_ITEM_BPC_MAX_W >> 3UL ) + 31UL ) & 0xFFFFFFE0UL )
#define SVC_MODULE_ITEM_BPC_SIZE            ( SVC_MODULE_ITEM_BPC_MAX_P * SVC_MODULE_ITEM_BPC_MAX_H )

typedef struct {
    SVC_CALIB_VIN_SENSOR_GEO_s CalibGeo;
    UINT8 *pSbpTbl;
    UINT32 SbpTblSize;
    UINT32 EnableOB;
} SVC_MODULE_BPC_TBL_DATA_s;

UINT32 SvcModule_BpcCmdFunc(UINT32 CmdID, void *pData, void *pParam1, void *pParam2, void *pParam3, void *pParam4);

#endif /* SVC_CALIB_MODULE_MGR_H */
