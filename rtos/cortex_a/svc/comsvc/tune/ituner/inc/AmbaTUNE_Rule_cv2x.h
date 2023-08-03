/*
*  @file AmbaTUNE_Rule_cv2x.h
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

#ifndef AMBA_TUNE_RULE_CV2X_H
#define AMBA_TUNE_RULE_CV2X_H
#include "AmbaTypes.h"
#include "AmbaTUNE_ImgCalibItuner_cv2x.h"
#define SKIP_VALID_CHECK (0xFFFFFFFFFFFFFFFFULL)
typedef struct {
    TUNE_REG_s *RegList;
    UINT8 RegCount;
} TUNE_Rule_Info_t;


typedef enum {
    ITUNER_ENC = 0,
    ITUNER_DEC,
} TUNE_OpMode_e;

INT32 TUNE_Rule_Get_Info(TUNE_Rule_Info_t *RuleInfo);
void ituner_Opmode_Set(TUNE_OpMode_e misra_OpMode);
//TUNE_OpMode_e ituner_Opmode_Get(void);
const char *TUNE_Rule_LU_Tuning_Mode_Str(TUNING_MODE_e Tuning_Mode);
//const char *TUNE_Rule_LU_Tuning_Mode_Ext_Str(AMBA_ITN_TUNING_MODE_EXT_e Tuning_Mode_Ext);
//AMBA_ITN_TUNING_MODE_EXT_e TUNE_Rule_LU_Tuning_Mode_Ext(const char *Key);
//TUNING_MODE_e TUNE_Rule_LU_Tuning_Mode(const char *Key);
#endif /* _AMBA_TUNE_HDLR_H_ */


