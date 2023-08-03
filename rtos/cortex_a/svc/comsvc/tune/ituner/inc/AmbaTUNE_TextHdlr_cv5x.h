/*
*  @file AmbaTUNE_TextHdlr_cv5x.h
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

#ifndef AMBA_TEXT_HDLR_CV5X_H
#define AMBA_TEXT_HDLR_CV5X_H
/*******************************************************************************
 * Header include
 ******************************************************************************/
#include "AmbaTypes.h"
#include "AmbaTUNE_ImgCalibItuner_cv5x.h"

/*******************************************************************************
* Program
******************************************************************************/
/**
 * @brief TextHdlr Initial
 * If you want to use any TextHdlr_xxx(), please call this function first.
 *
 * @param None
 *
 * @return 0 (Success) / -1 (Failure)
 */

//#include "AmbaImgCalibItuner_cv2fs.h" // Note: Ituner_Ext_File_Type_e Need



INT32 TextHdlr_Init(const Ituner_Initial_Config_t *pItunerInit);
//void TextHdlr_Change_Parser_Mode(AMBA_ITN_Hdlr_Mode_e Parser_Mode);
INT32 TextHdlr_Save_IDSP(const AMBA_IK_MODE_CFG_s *pMode, const char *Filepath);
INT32 TextHdlr_Load_IDSP(const char *Filepath);
INT32 TextHdlr_Save_Data(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s *Ext_File_Param);
INT32 TextHdlr_Load_Data(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s *Ext_File_Param);
INT32 TextHdlr_Execute_IDSP(const AMBA_IK_MODE_CFG_s *pMode);
INT32 TextHdlr_Get_SystemInfo(AMBA_ITN_SYSTEM_s *System);
INT32 TextHdlr_Set_SystemInfo(const AMBA_ITN_SYSTEM_s *System);
INT32 TextHdlr_Get_AeInfo(ITUNER_AE_INFO_s *AeInfo);
INT32 TextHdlr_Set_AeInfo(const ITUNER_AE_INFO_s *AeInfo);
INT32 TextHdlr_Get_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo);
INT32 TextHdlr_Set_WbSimInfo(const ITUNER_WB_SIM_INFO_s *WbSimInfo);
INT32 TextHdlr_Get_ItunerInfo(AMBA_ITN_ITUNER_INFO_s *ItunerInfo);
#endif /* ifndef AMBA_TEXT_HDLR_H*/
