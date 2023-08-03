/**
 *  @file RefCode_FileFeed.h
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details header for avb file feed
 *
 */


#ifndef AMBA_AVB_FILEFEED_H
#define AMBA_AVB_FILEFEED_H
#ifdef CONFIG_AMBA_AVB

#define MAX_FILE_FEED_NUM 2U

typedef struct {
    UINT32  Type;
    char    Prefix[AMBA_CFS_MAX_FILENAME_LENGTH];
    UINT8*  pBuffer;
    UINT32  BufferSize;
    AMBA_FIFO_HDLR_s* pFifo;
    UINT32  TimeScale;
    UINT32  TimePerFrame;
} FILE_FEED_CFG_s;

extern FILE_FEED_CFG_s FileFeedConfig[MAX_FILE_FEED_NUM];
extern  UINT8 InitFileFeedConfig;

UINT32 RefCode_FileFeed_Start(UINT32 Id);
UINT32 RefCode_FileFeed_Stop(UINT32 Id);
#endif /* CONFIG_AMBA_AVB */

#endif /* AMBA_AVB_FILEFEED_H */
