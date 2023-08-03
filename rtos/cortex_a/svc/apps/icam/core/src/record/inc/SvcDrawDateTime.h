/**
*  @file SvcDrawStopwatch.h
*
*  @copyright Copyright (c) 2015 Ambarella, Inc.
*
*  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
*  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
*  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
*  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
*  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
*  return this Software to Ambarella, Inc.
*
*  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
*  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
*  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*  @details svc draw stopwatch
*
*/

#ifndef SVC_DRAW_DATETIME_H
#define SVC_DRAW_DATETIME_H

#define SVC_DRAW_DATETIME_IDX   0U

typedef struct {
    UINT16   Width;           /* Number of pixels per line in the window */
    UINT16   Height;          /* Number of lines in the window */
    UINT32   BaseAddrY;       /* Luma (Y) data buffer address */
    UINT32   BaseAddrUV;      /* Chroma (UV) buffer address */
    UINT32   BaseAddrAY;      /* Alpha Y data buffer address */
    UINT32   BaseAddrAUV;     /* Alpha UV data buffer address */
    UINT16   Pitch;           /* YUV data buffer pitch */
    UINT16   RealWidth;
}SVC_DRAW_DATETIME_BUF_INFO;

typedef struct {
    SVC_DRAW_DATETIME_BUF_INFO DateTime;
}SVC_DRAW_DATETIME_INFO;

void SvcDrawDateTime_Init(void);
void SvcDrawDateTime_InfoGet(UINT32 StreamId, UINT32 StreamWidth, UINT32 StreamHeight, SVC_DRAW_DATETIME_INFO *pInfo, const UINT32 RecBlend);
void SvcDrawDateTime_BufUpdate(UINT16 StreamId, UINT16 BlendAreaIdx, UINT32 *pAction);

#endif  /* SVC_DRAW_DATETIME_H */
