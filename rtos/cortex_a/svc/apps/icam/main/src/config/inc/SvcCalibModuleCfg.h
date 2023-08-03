/**
*  @file SvcCalibModuleCfg.h
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
*  @details svc calibration module config
*
*/

#ifndef SVC_CALIB_MODULE_CFG_H
#define SVC_CALIB_MODULE_CFG_H

#define SVC_CALIB_TSK_MODULE_MAX_NUM   (16U)

typedef struct {
    UINT32 Enable;
    UINT32 VinSelectBits;
    UINT32 SerDesSelectBits;
    UINT32 TableIdx;
} SVC_MODULE_ITEM_UPD_RULE_s;

typedef struct {
    UINT32                      Enable;
    UINT32                      ItemVersion;
    UINT32                      CalibID;
    UINT32                      ReqMemSize;
    SVC_MODULE_ITEM_UPD_RULE_s  UpdRule[SVC_CALIB_TSK_MODULE_MAX_NUM];
} SVC_MODULE_ITEM_UPD_s;

typedef struct {
    UINT32                 ItemCfgNum;
    SVC_MODULE_ITEM_CFG_s *pItemCfg;
    UINT32                 ItemUpdNum;
    SVC_MODULE_ITEM_UPD_s *pItemUpd;
} SVC_MODULE_CFG_s;

SVC_MODULE_CFG_s* SvcModuleCfg_Get(void);

#endif  /* SVC_CALIB_MODULE_CFG_H */
