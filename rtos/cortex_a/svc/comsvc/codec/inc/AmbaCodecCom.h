/**
*  @file AmbaCodecCom.h
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
*  @details amba codec common
*
*/

#ifndef AMBA_CODECCOM_H
#define AMBA_CODECCOM_H

#define AMBA_CODEC_PRINT_MODULE_ID              ((UINT16)(CODEC_ERR_BASE >> 16U))
#define AMBA_PLAYER_PRINT_MODULE_ID             ((UINT16)(PLAYER_ERR_BASE >> 16U))
#define AMBA_RECODER_PRINT_MODULE_ID            ((UINT16)(RECODER_ERR_BASE >> 16U))
#define AMBA_AUDIO_PRINT_MODULE_ID              ((UINT16)(AUDIO_ERR_BASE >> 16U))

#define CODEC_OK                                (0U)
#define CODEC_ERROR_ARG                         (CODEC_ERR_BASE)
#define CODEC_ERROR_GENERAL_ERROR               (CODEC_ERR_BASE + 0x00000001U)
#define CODEC_ERROR_TIMEOUT                     (CODEC_ERR_BASE + 0x00000002U)

#define PLAYER_OK                               (0U)
#define PLAYER_ERROR_ARG                        (PLAYER_ERR_BASE)
#define PLAYER_ERROR_GENERAL_ERROR              (PLAYER_ERR_BASE + 0x00000001U)

#define RECODER_OK                              (0U)
#define RECODER_ERROR_ARG                       (RECODER_ERR_BASE)
#define RECODER_ERROR_GENERAL_ERROR             (RECODER_ERR_BASE + 0x00000001U)

#define AUDIO_OK                                (0U)
#define AUDIO_ERROR_ARG                         (RECODER_ERR_BASE)
#define AUDIO_ERROR_GENERAL_ERROR               (RECODER_ERR_BASE + 0x00000001U)

#define REFCODE_OK                              (0U)
#define REFCODE_GENERAL_ERR                     (PLAYER_ERR_BASE + 0x00000001U)
#define REFCODE_IO_ERR                          (PLAYER_ERR_BASE + 0x00000002U)


/* AmbaBitsFifo */
typedef struct {
    UINT32  TaskPriority;
    UINT32  TaskCpuBits;
    ULONG   BufAddr;
    UINT32  BufSize;
} AMBA_BFIFO_INIT_s;

UINT32 AmbaBitsFifo_EvalMemSize(UINT32 *pSize);
UINT32 AmbaBitsFifo_Init(const AMBA_BFIFO_INIT_s *pInit);

#endif  /* AMBA_CODECCOM_H */
