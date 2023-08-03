/**
*  @file DetCls.h
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
*   @details The detection + classification algorithm
*
*/

/******************************************************************************
 *  SCA spec - DetCls
 ******************************************************************************
 *  [Description]
 *      - This SCA shows how to implement a SCA with multiple flexidags.
 *        The given 2 model are not trained for this purpose so this is not a
 *        real algorithm. It just a demo code.
 *
 *  [Flexidag Info]
 *      - Number : 2
 *      - FD#1
 *          - 01000_mnet_ssd_adas_cf
 *      - FD#2
 *          - 00005_squeezenet_cf_fp32_resize_preproc_raw
 *
 *  [Output Format]
 *      - CVALGO_DETCLS_OUTPUT_s
 *
 *  [Extend Config]
 *      - pExtQueryCfg   : None
 *      - pExtCreateCfg  : None
 *      - pExtDeleteCfg  : None
 *      - pExtFeedCfg    : None
 *      - pExtRegcbCfg   : None
 *
 *  [Supported Control]
 *      - CTRL_TYPE_DMSG    : pCtrlParam = NULL
 */

#ifndef DETCLS_H
#define DETCLS_H

#include "SvcCvAlgo.h"
#include "CvCommFlexi.h"

#include "ArmSsdFusion.h"

#define MAX_CVALGO_DETCLS_BOX_NUM    (100U)

typedef struct {
    UINT32 Class;
    FLOAT  Score;
    UINT32 X;        // The X,Y,W,H is based on (VisWidth, VisHeight) domain
    UINT32 Y;
    UINT32 W;
    UINT32 H;
} CVALGO_DETCLS_BOX_s;

typedef struct {
    UINT32                BoxNum;
    CVALGO_DETCLS_BOX_s   Box[MAX_CVALGO_DETCLS_BOX_NUM];
} CVALGO_DETCLS_OUTPUT_s;


/******************************************************************************
 *  Defined in DetCls.c
 ******************************************************************************/
extern SVC_CV_ALGO_OBJ_s CvAlgoObj_DetCls;

#endif /* DETCLS_H */

