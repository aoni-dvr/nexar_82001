/**
 *  @file AmbaImgMainFov.h
 *
 *  Copyright (c) [2020] Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Constants and Definitions for Amba Image Main FOV
 *
 */

#ifndef AMBA_IMG_MAIN_FOV_H
#define AMBA_IMG_MAIN_FOV_H

#define AMBA_IMG_NUM_FOV_ADJ_CHANNEL 8U

typedef enum /*_AMBA_IMG_MAIN_FOV_CMD_e_*/ {
    FOV_TASK_CMD_STOP = 0,
    FOV_TASK_CMD_START
} AMBA_IMG_MAIN_FOV_CMD_e;

extern UINT32 AmbaImgMainFov_DebugCount[AMBA_IMG_NUM_FOV_ADJ_CHANNEL];
extern AMBA_IMG_CHANNEL_ID_s AmbaImgMainFov_ImageChanId[AMBA_IMG_NUM_FOV_ADJ_CHANNEL];
extern UINT32 AmbaImgMainFov_LastIndex;

UINT32 AmbaImgMainFov_Init(void);

  void AmbaImgMainFov_Dispatch(UINT32 VinId, UINT64 Param);

#endif  /* AMBA_IMG_MAIN_FOV_H */
