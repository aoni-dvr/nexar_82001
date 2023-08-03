/**
 *  @file cvapi_visutil.h
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
 *  @details cvapi_visutil
 *
 */

#ifndef CVAPI_VISUTIL_H
#define CVAPI_VISUTIL_H
#pragma pack(push)
#pragma pack(1)

typedef struct {
    uint32_t full_input_width;                  /*< input width, default 3840*/
    uint32_t full_input_height;                 /*< input height, default 2160 */
    uint32_t osd_width;                         /*< osd width, default 960 */
    uint32_t osd_height;                        /*< osd height, default 480 */
    uint32_t osd_channel;                       /*< osd channel, default 2 (TV) */
} VIS_UTIL_CFG_s;

/// message code

#define VIS_UTIL_MSG_BBX                (0x20000001U)


#define VIS_UTIL_MAX_BBX_NUM       (150)

typedef struct {
    uint16_t Cat;                                       /*< object catigory */
    uint16_t Fid;                                       /*< in which field object detected, default 0 */
    uint32_t ObjectId;                                  /*< object id. only valid if tracker applied */
    uint32_t Score;                                     /*< sorce of object */
    uint16_t X;                                         /*< x offset in fulll frame coordination */
    uint16_t Y;                                         /*< y offset in fulll frame coordination */
    uint16_t W;                                         /*< width in fulll frame coordination */
    uint16_t H;                                         /*< height in fulll frame coordination */
} VIS_UTIL_BBX_s;

typedef struct {
    uint32_t MsgCode;                                   /*< message code */
    uint32_t Source;                                    /*< indicator od vin source, default 0 */
    uint32_t CaptureTime;                               /*< capture time of detection frame, default 0 */
    uint32_t FrameNum;                                  /*< frame number of detection frame, default 0 */
    uint32_t NumBbx;                                    /*< total number of object */
    VIS_UTIL_BBX_s Bbx[VIS_UTIL_MAX_BBX_NUM];           /*< object list */
} VIS_UTIL_BBX_LIST_MSG_s;


/**
 * config vis util
 * @param cfg - pointer to config structure
 */
int32_t VisUtil_SetConfig(const VIS_UTIL_CFG_s* cfg);

/**
 * process vis util messages
 * @param buf - pointer to messgae buffer
 * @param len - length of message
 */
int32_t VisUtil_ProcessMessage(const void* buf, int32_t len);

#pragma pack(pop)
#endif //CVAPI_VISUTIL_H
