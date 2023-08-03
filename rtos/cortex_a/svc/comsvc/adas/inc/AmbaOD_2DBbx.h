/**
 * @file AmbaOD_2DBbx.h
 *
 * Copyright (c) 2018 Ambarella International LP
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
 *  @details AmbaOD_2DBbx
 *
 */

#ifndef AMBA_OD_2DBBX_H
#define AMBA_OD_2DBBX_H

#pragma pack(push)
#pragma pack(1)

#define AMBA_OD_2DBBX_MAX_BBX_NUM       (150)

typedef struct {
    UINT16 Cat;                                       /**< object catigory */
    UINT16 Fid;                                       /**< in which field object detected, default 0, only valid in multiROI case */
    UINT32 ObjectId;                                  /**< object id. only valid if tracker applied */
    UINT32 Score;                                     /**< sorce of object */
    UINT16 X;                                         /**< x offset in fulll frame coordination */
    UINT16 Y;                                         /**< y offset in fulll frame coordination */
    UINT16 W;                                         /**< width in fulll frame coordination */
    UINT16 H;                                         /**< height in fulll frame coordination */
} AMBA_OD_2DBBX_s;

typedef struct {
    UINT32 MsgCode;                                   /**< message code */
    UINT32 CaptureTime;                               /**< capture time of detection frame, default 0 */
    UINT32 FrameNum;                                  /**< frame number of detection frame, default 0 */
    UINT32 NumBbx;                                    /**< total number of object */
    AMBA_OD_2DBBX_s Bbx[AMBA_OD_2DBBX_MAX_BBX_NUM];   /**< object list */
} AMBA_OD_2DBBX_LIST_s;

#pragma pack(pop)
#endif //AMBA_OD_2DBBX_H
