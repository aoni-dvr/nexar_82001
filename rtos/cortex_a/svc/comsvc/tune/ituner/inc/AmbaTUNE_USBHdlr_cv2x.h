/*
*  @file AmbaTUNE_USBHdlr_cv2x.h
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

#ifndef AMBA_USB_HDLR_CV2X_H
#define AMBA_USB_HDLR_CV2X_H
/*******************************************************************************
 * Header include
 ******************************************************************************/
#include "AmbaTypes.h"
#include "AmbaTUNE_ImgCalibItuner_cv2x.h"


#define usbhdlr_print_uint32_5 AmbaPrint_PrintUInt5
#define usbhdlr_print_int32_5 AmbaPrint_PrintInt5
#define usbhdlr_print_str_5 AmbaPrint_PrintStr5
#define usbhdlr_memset AmbaWrap_memset
#define usbhdlr_memcpy AmbaWrap_memcpy

/************************ Error Code*************************************/

#define USBHDLR_OK                     0
#define USBHDLR_ERR_VERSION           20
#define USBHDLR_ERR_ITUNERINIT        21
#define USBHDLR_ERR_REGPARAM          22
#define USBHDLR_ERR_READTEXT          23
#define USBHDLR_ERR_SAVETEXT          24
#define USBHDLR_ERR_LOADCC            25
#define USBHDLR_ERR_SAVECC            26

INT32 USBHdlr_Init(const Ituner_Initial_Config_t *pItunerInit);
INT32 USBHdlr_Save_Reg_Params(UINT8 *pDataBuffer);
INT32 USBHdlr_Load_IDSP(UINT8 *pDataBufferAddr,UINT32 ItunerDataSize);
INT32 USBHdlr_Save_IDSP(const AMBA_IK_MODE_CFG_s *pMode, UINT8 *pDataBufferAddr);
INT32 USBHdlr_Save_Data(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s* Ext_File_Param);
INT32 USBHdlr_Load_Data(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s* Ext_File_Param);
INT32 USBHdlr_Get_SystemInfo(AMBA_ITN_SYSTEM_s *System);
INT32 USBHdlr_Set_SystemInfo(const AMBA_ITN_SYSTEM_s *System);
#if 0
INT32 USBHdlr_Get_AeInfo(ITUNER_AE_INFO_s *AeInfo);
INT32 USBHdlr_Set_AeInfo(const ITUNER_AE_INFO_s *AeInfo);
INT32 USBHdlr_Get_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo);
INT32 USBHdlr_Set_WbSimInfo(const ITUNER_WB_SIM_INFO_s *WbSimInfo);
#endif
INT32 USBHdlr_Execute_IDSP(const AMBA_IK_MODE_CFG_s *pMode);


#endif /* ifndef _AMBA_TEXT_HDLR_H_*/
