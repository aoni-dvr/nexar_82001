/**
 *  @file RefPlayer.h
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
 *  @details Implementation of reference player
 *
 */

#ifndef REF_PLAYER_H
#define REF_PLAYER_H

#include <AmbaTypes.h>
#include <AmbaCFS.h>
// #include <AmbaDemuxer.h>
#include <AmbaShell.h>

#define AMBA_DEMUXER_MAX_FORMAT_PER_PIPE    (2U)

#define REF_PLAYER_VIDEO_INIT_MODE_NORMAL   (0U)                    /**< Video Dec init mode: Normal playback */
#define REF_PLAYER_VIDEO_INIT_MODE_PYRAMID  (1U)                    /**< Video Dec init mode: Play video and output pyramid YUV */
#define REF_PLAYER_VIDEO_INIT_MODE_NUM      (2U)                    /**< Video Dec init mode number */

#define REF_PLAYER_MODE_NORMAL              (0U)                    /**< Video Dec mode: Play MP4 generated by RefMuxer. */
#define REF_PLAYER_MODE_RAW                 (1U)                    /**< Video Dec mode: Play raw file with .nhnt/.udta files. */
#define REF_PLAYER_MODE_COMPATIBLE          (2U)                    /**< Video Dec mode: Compatible mode. Play MP4 generated by external muxer. */
#define REF_PLAYER_MODE_NUM                 (3U)                    /**< Video Dec mode number. */

#define REF_PLAYER_FORMAT_TYPE_FMP4         (0U)                    /**< Format type in Normal Mode: FMP4 */
#define REF_PLAYER_FORMAT_TYPE_MP4          (1U)                    /**< Format type in Normal Mode: MP4 */
#define REF_PLAYER_FORMAT_TYPE_H264         (2U)                    /**< Format type in Raw Mode: H264 */
#define REF_PLAYER_FORMAT_TYPE_H265         (3U)                    /**< Format type in Raw Mode: H265 */
#define REF_PLAYER_FORMAT_TYPE_CMPT         (4U)                    /**< Format type in Compatible Mode: MP4 */
#define REF_PLAYER_FORMAT_TYPE_NUM          (5U)                    /**< Format type number */

/**
 *  Demuxer file info
 */
typedef struct {
    UINT8 FormatType;                                                               /**< Format type. Ex: REF_PLAYER_FORMAT_TYPE_FMP4. */
    char Prefix[AMBA_CFS_MAX_FILENAME_LENGTH];                                      /**< File full path excluding filename extension. */
} REF_PLAYER_DEMUXER_FILE_INFO_s;

extern UINT32 RefPlayer_Init(UINT32 InitMode);

extern UINT32 RefPlayer_SetPlayMode(UINT32 PlayMode);

extern void AmbaShell_CommandPlayerTest(UINT32 ArgCount, char * const * ArgVector, AMBA_SHELL_PRINT_f PrintFunc);

#endif
