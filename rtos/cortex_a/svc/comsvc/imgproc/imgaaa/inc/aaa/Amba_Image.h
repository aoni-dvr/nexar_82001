/**
 * @file Amba_Image.h
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
 * @Description    :: Implementation of Amba_Image
 */
#ifndef AMBA_IMAGE_H__
#define AMBA_IMAGE_H__

#define IMGPROC_PARTIAL_LOAD_All 0U
#define IMGPROC_PARTIAL_LOAD_VIDEO_PART 1U
#define IMGPROC_PARTIAL_LOAD_STILL_PART 2U
#define IMGPROC_PARTIAL_LOAD_SECTION_NUM 3U

UINT32 App_Image_Init(UINT32 viewIDCount);
UINT32 App_Image_Init_Iq_Params(UINT32 viewID, INT32 sensorID);
UINT32 App_Image_Init_Iq_Params_Section(UINT32 viewID, INT32 sensorID, UINT8 section);
UINT32 App_Image_Iq_Params_Preload(UINT32 viewID, INT32 sensorID);
UINT32 App_Image_Iq_Params_load_Section(UINT32 viewID, INT32 sensorID, UINT8 section);

#endif
