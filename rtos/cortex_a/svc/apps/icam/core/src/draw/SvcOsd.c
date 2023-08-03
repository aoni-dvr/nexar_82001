/**
 *  @file SvcOsd.c
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
 *  @details svc application cv handler
 *
 */

#include "AmbaKAL.h"
#include "AmbaGDMA.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VOUT.h"
#if defined(CONFIG_BUILD_COMMON_SERVICE_DRAW)
#include "AmbaDraw.h"
#endif
#include "AmbaCache.h"
#include "AmbaNAND.h"
#include "AmbaUtility.h"
#include "AmbaFPD.h"
#include "AmbaDef.h"
#include "AmbaMMU.h"
#include "AmbaPrint.h"

#include "SvcNvm.h"
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcVinSrc.h"
#include "SvcWrap.h"
#include "SvcOsd.h"
#include "SvcResCfg.h"
#include "SvcCvImgUtil.h"
#include "SvcPlat.h"

#if !defined(CONFIG_BUILD_COMMON_SERVICE_DRAW)
#define AMBA_DRAW_BUFFER_8BIT_CLUT_MODE     OSD_8BIT_CLUT_MODE

typedef struct {
    UINT8   PixelFormat;
    UINT32  Width;
    UINT32  Height;
    UINT32  Pitch;
    void    *RGBAddr;
    void    *YAddr;
    void    *UVAddr;
    void    *AlphaYAddr;
    void    *AlphaUVAddr;
} AMBA_DRAW_BUFFER_INFO_s;
#endif

typedef struct {
    AMBA_DRAW_BUFFER_INFO_s  DrawBufInfo;

    UINT32                   FrameSize;
    UINT16                   LineObjId;
    UINT16                   RectObjId;
    UINT16                   StrObjId;
    UINT16                   CircleObjId;
} SVC_OSD_CTRL_s;

#define SVC_LOG_OSD                 "OSD"

#define SVC_OSD_BUF_NUM             (4U)
#define SVC_OSD_MODE                AMBA_DRAW_BUFFER_8BIT_CLUT_MODE
// #define SVC_OSD_BUF_ALIGN           (64U)

#define SVC_OSD_BUF_MAX_WIDTH_VOUT_A    (1920U)
#define SVC_OSD_BUF_MAX_HEIGHT_VOUT_A   (1080U)

#if defined CONFIG_ICAM_VOUTB_OSD_BUF_FHD
#define SVC_OSD_BUF_MAX_WIDTH_VOUT_B    (1920U)
#define SVC_OSD_BUF_MAX_HEIGHT_VOUT_B   (1080U)
#else
#define SVC_OSD_BUF_MAX_WIDTH_VOUT_B    (960U)  /* CV limitation */
#define SVC_OSD_BUF_MAX_HEIGHT_VOUT_B   (540U)  /* CV limitation */
#endif


#define SVC_OSD_BUF_MAX_WIDTH           (1920U)
#define SVC_OSD_BUF_MAX_HEIGHT          (1080U)

#if (SVC_OSD_PIXEL_SIZE_SFT == 0U)
#define SVC_OSD_BUF_SIZE            (SVC_OSD_BUF_MAX_WIDTH * SVC_OSD_BUF_MAX_HEIGHT)
#else
#define SVC_OSD_BUF_SIZE            ((SVC_OSD_BUF_MAX_WIDTH * SVC_OSD_BUF_MAX_HEIGHT) << SVC_OSD_PIXEL_SIZE_SFT)
#endif

#if defined(CONFIG_BUILD_COMMON_SERVICE_DRAW)
#define SVC_MAX_FONTS_BIN_FILE_SIZE         (64U * 1024U)
static UINT8        *g_SvcOsdFontBuf = NULL;
#endif

#define SVC_OSD_CLUT_ENTRY_SIZE     (256U)
#define SVC_OSD_EXTBUF_NOTCONFIG  (0U)
#define SVC_OSD_EXTBUF_CONFIGED   (1U)
#define SVC_OSD_EXTBUF_ENABLED    (2U)

typedef struct {
    UINT8                    MemType;
    UINT8                    *pAddr[SVC_OSD_BUF_NUM];
} SVC_OSD_BUFFER_s;

static UINT8          *g_BmpBuf = NULL; /* g_BmpBuf is just used to load bmp data from FLASH. It will be copied out */

static SVC_OSD_CTRL_s  g_SvcOsdCtrl[SVC_OSD_CHAN_NUM];
static ULONG           g_OsdClut_BaseAddr = 0U;
static UINT8           OSDExtBufferStatus[SVC_OSD_CHAN_NUM] = {SVC_OSD_EXTBUF_NOTCONFIG};
static SvcOSD_ExtBuffer_CFG_s    g_OSDExtBufferCFG[SVC_OSD_CHAN_NUM] = {0};
static AMBA_KAL_MUTEX_t          g_OSDExMutex[SVC_OSD_CHAN_NUM] = {0};
static SVC_OSD_CONFIG_s g_SvcOsdConfig[SVC_OSD_CHAN_NUM] = {0};
static SVC_OSD_BUFFER_s g_SvcOsdBuffer[SVC_OSD_CHAN_NUM] = {0};
static ULONG            g_SvcOsdCLUTAddr[SVC_OSD_CHAN_NUM] = {0U};

#define SVC_OSD_ASCII_PTN_ROW       (SVC_OSD_CHAR_HEIGHT)
#define SVC_OSD_ASCII_PTN_COL       (SVC_OSD_CHAR_WIDTH)
#define SVC_OSD_ASCII_PTN_SIZE      (SVC_OSD_ASCII_PTN_ROW)
#define SVC_OSD_ASCII_BEGIN         (0x20U)
#define SVC_OSD_ASCII_END           (0x7EU)
#define SVC_OSD_ASCII_NUM           ((SVC_OSD_ASCII_END - SVC_OSD_ASCII_BEGIN) + 1U)
#define SVC_OSD_ASCII_PADDING_WIDTH (SVC_OSD_CHAR_GAP)

static UINT8 SvcGuiPalAsciiPtn[SVC_OSD_ASCII_NUM][SVC_OSD_ASCII_PTN_SIZE] = {
    /* ' ' */ [ 0U] = { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U },
    /* '!' */ [ 1U] = { 0x00U, 0x04U, 0x04U, 0x04U, 0x04U, 0x00U, 0x00U, 0x04U, 0x00U, 0x00U },
    /* '"' */ [ 2U] = { 0x00U, 0x0AU, 0x0AU, 0x0AU, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U },
    /* '#' */ [ 3U] = { 0x00U, 0x0AU, 0x0AU, 0x1FU, 0x0AU, 0x1FU, 0x0AU, 0x0AU, 0x00U, 0x00U },
    /* '$' */ [ 4U] = { 0x00U, 0x04U, 0x1EU, 0x05U, 0x0EU, 0x14U, 0x0FU, 0x04U, 0x00U, 0x00U },
    /* '%' */ [ 5U] = { 0x00U, 0x03U, 0x13U, 0x08U, 0x04U, 0x02U, 0x19U, 0x18U, 0x00U, 0x00U },
    /* '&' */ [ 6U] = { 0x00U, 0x06U, 0x09U, 0x05U, 0x02U, 0x15U, 0x09U, 0x16U, 0x00U, 0x00U },
    /* ''' */ [ 7U] = { 0x00U, 0x06U, 0x02U, 0x04U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U },
    /* '(' */ [ 8U] = { 0x00U, 0x08U, 0x04U, 0x02U, 0x02U, 0x02U, 0x04U, 0x08U, 0x00U, 0x00U },
    /* ')' */ [ 9U] = { 0x00U, 0x02U, 0x04U, 0x08U, 0x08U, 0x08U, 0x04U, 0x02U, 0x00U, 0x00U },
    /* '*' */ [10U] = { 0x00U, 0x00U, 0x04U, 0x15U, 0x0EU, 0x15U, 0x04U, 0x00U, 0x00U, 0x00U },
    /* '+' */ [11U] = { 0x00U, 0x00U, 0x04U, 0x04U, 0x1FU, 0x04U, 0x04U, 0x00U, 0x00U, 0x00U },
    /* ',' */ [12U] = { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x06U, 0x04U, 0x02U, 0x00U, 0x00U },
    /* '-' */ [13U] = { 0x00U, 0x00U, 0x00U, 0x00U, 0x1FU, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U },
    /* '.' */ [14U] = { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x06U, 0x06U, 0x00U, 0x00U },
    /* '/' */ [15U] = { 0x00U, 0x00U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U, 0x00U, 0x00U, 0x00U },
    /* '0' */ [16U] = { 0x00U, 0x0EU, 0x11U, 0x19U, 0x15U, 0x13U, 0x11U, 0x0EU, 0x00U, 0x00U },
    /* '1' */ [17U] = { 0x00U, 0x04U, 0x06U, 0x04U, 0x04U, 0x04U, 0x04U, 0x0EU, 0x00U, 0x00U },
    /* '2' */ [18U] = { 0x00U, 0x0EU, 0x11U, 0x10U, 0x08U, 0x04U, 0x02U, 0x1FU, 0x00U, 0x00U },
    /* '3' */ [19U] = { 0x00U, 0x1FU, 0x08U, 0x04U, 0x08U, 0x10U, 0x11U, 0x0EU, 0x00U, 0x00U },
    /* '4' */ [20U] = { 0x00U, 0x08U, 0x0CU, 0x0AU, 0x09U, 0x1FU, 0x08U, 0x08U, 0x00U, 0x00U },
    /* '5' */ [21U] = { 0x00U, 0x1FU, 0x01U, 0x0FU, 0x10U, 0x10U, 0x11U, 0x0EU, 0x00U, 0x00U },
    /* '6' */ [22U] = { 0x00U, 0x0CU, 0x02U, 0x01U, 0x0FU, 0x11U, 0x11U, 0x0EU, 0x00U, 0x00U },
    /* '7' */ [23U] = { 0x00U, 0x1FU, 0x10U, 0x08U, 0x04U, 0x02U, 0x02U, 0x02U, 0x00U, 0x00U },
    /* '8' */ [24U] = { 0x00U, 0x0EU, 0x11U, 0x11U, 0x0EU, 0x11U, 0x11U, 0x0EU, 0x00U, 0x00U },
    /* '9' */ [25U] = { 0x00U, 0x0EU, 0x11U, 0x11U, 0x0EU, 0x10U, 0x08U, 0x06U, 0x00U, 0x00U },
    /* ':' */ [26U] = { 0x00U, 0x00U, 0x06U, 0x06U, 0x00U, 0x06U, 0x06U, 0x00U, 0x00U, 0x00U },
    /* ';' */ [27U] = { 0x00U, 0x00U, 0x06U, 0x06U, 0x00U, 0x06U, 0x02U, 0x04U, 0x00U, 0x00U },
    /* '<' */ [28U] = { 0x00U, 0x08U, 0x04U, 0x02U, 0x01U, 0x02U, 0x04U, 0x08U, 0x00U, 0x00U },
    /* '=' */ [29U] = { 0x00U, 0x00U, 0x00U, 0x1FU, 0x00U, 0x1FU, 0x00U, 0x00U, 0x00U, 0x00U },
    /* '>' */ [30U] = { 0x00U, 0x02U, 0x04U, 0x08U, 0x10U, 0x08U, 0x04U, 0x02U, 0x00U, 0x00U },
    /* '?' */ [31U] = { 0x00U, 0x0EU, 0x11U, 0x10U, 0x08U, 0x04U, 0x00U, 0x04U, 0x00U, 0x00U },
    /* '@' */ [32U] = { 0x00U, 0x0EU, 0x11U, 0x00U, 0x16U, 0x15U, 0x15U, 0x0EU, 0x00U, 0x00U },
    /* 'A' */ [33U] = { 0x00U, 0x0EU, 0x11U, 0x11U, 0x11U, 0x1FU, 0x11U, 0x11U, 0x00U, 0x00U },
    /* 'B' */ [34U] = { 0x00U, 0x0FU, 0x11U, 0x11U, 0x0FU, 0x11U, 0x11U, 0x0FU, 0x00U, 0x00U },
    /* 'C' */ [35U] = { 0x00U, 0x0EU, 0x11U, 0x01U, 0x01U, 0x01U, 0x11U, 0x0EU, 0x00U, 0x00U },
    /* 'D' */ [36U] = { 0x00U, 0x07U, 0x09U, 0x11U, 0x11U, 0x11U, 0x09U, 0x07U, 0x00U, 0x00U },
    /* 'E' */ [37U] = { 0x00U, 0x1FU, 0x01U, 0x01U, 0x0FU, 0x01U, 0x01U, 0x1FU, 0x00U, 0x00U },
    /* 'F' */ [38U] = { 0x00U, 0x1FU, 0x01U, 0x01U, 0x0FU, 0x01U, 0x01U, 0x01U, 0x00U, 0x00U },
    /* 'G' */ [39U] = { 0x00U, 0x0EU, 0x11U, 0x01U, 0x1DU, 0x11U, 0x11U, 0x1EU, 0x00U, 0x00U },
    /* 'H' */ [40U] = { 0x00U, 0x11U, 0x11U, 0x11U, 0x1FU, 0x11U, 0x11U, 0x11U, 0x00U, 0x00U },
    /* 'I' */ [41U] = { 0x00U, 0x0EU, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x0EU, 0x00U, 0x00U },
    /* 'J' */ [42U] = { 0x00U, 0x1CU, 0x08U, 0x08U, 0x08U, 0x08U, 0x09U, 0x06U, 0x00U, 0x00U },
    /* 'K' */ [43U] = { 0x00U, 0x11U, 0x09U, 0x05U, 0x03U, 0x05U, 0x09U, 0x11U, 0x00U, 0x00U },
    /* 'L' */ [44U] = { 0x00U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x1FU, 0x00U, 0x00U },
    /* 'M' */ [45U] = { 0x00U, 0x11U, 0x1BU, 0x15U, 0x15U, 0x11U, 0x11U, 0x11U, 0x00U, 0x00U },
    /* 'N' */ [46U] = { 0x00U, 0x11U, 0x11U, 0x13U, 0x15U, 0x19U, 0x11U, 0x11U, 0x00U, 0x00U },
    /* '0' */ [47U] = { 0x00U, 0x0EU, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x0EU, 0x00U, 0x00U },
    /* 'P' */ [48U] = { 0x00U, 0x0FU, 0x11U, 0x11U, 0x0FU, 0x01U, 0x01U, 0x01U, 0x00U, 0x00U },
    /* 'Q' */ [49U] = { 0x00U, 0x0EU, 0x11U, 0x11U, 0x11U, 0x15U, 0x09U, 0x16U, 0x00U, 0x00U },
    /* 'R' */ [50U] = { 0x00U, 0x0FU, 0x11U, 0x11U, 0x0FU, 0x05U, 0x09U, 0x11U, 0x00U, 0x00U },
    /* 'S' */ [51U] = { 0x00U, 0x1EU, 0x01U, 0x01U, 0x0EU, 0x10U, 0x10U, 0x0FU, 0x00U, 0x00U },
    /* 'T' */ [52U] = { 0x00U, 0x1FU, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x00U, 0x00U },
    /* 'U' */ [53U] = { 0x00U, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x0EU, 0x00U, 0x00U },
    /* 'V' */ [54U] = { 0x00U, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x0AU, 0x04U, 0x00U, 0x00U },
    /* 'W' */ [55U] = { 0x00U, 0x11U, 0x11U, 0x11U, 0x11U, 0x15U, 0x15U, 0x0AU, 0x00U, 0x00U },
    /* 'X' */ [56U] = { 0x00U, 0x11U, 0x11U, 0x0AU, 0x04U, 0x0AU, 0x11U, 0x11U, 0x00U, 0x00U },
    /* 'Y' */ [57U] = { 0x00U, 0x11U, 0x11U, 0x0AU, 0x04U, 0x04U, 0x04U, 0x04U, 0x00U, 0x00U },
    /* 'Z' */ [58U] = { 0x00U, 0x1FU, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U, 0x1FU, 0x00U, 0x00U },
    /* '[' */ [59U] = { 0x0EU, 0x02U, 0x02U, 0x02U, 0x02U, 0x02U, 0x0EU, 0x00U, 0x00U, 0x00U },
    /* '\' */ [60U] = { 0x00U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U, 0x00U, 0x00U, 0x00U, 0x00U },
    /* ']' */ [61U] = { 0x0EU, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x0EU, 0x00U, 0x00U, 0x00U },
    /* '^' */ [62U] = { 0x00U, 0x00U, 0x00U, 0x00U, 0x11U, 0x0AU, 0x04U, 0x00U, 0x00U, 0x00U },
    /* '_' */ [63U] = { 0x1FU, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U },
    /* '`' */ [64U] = { 0x00U, 0x00U, 0x00U, 0x00U, 0x08U, 0x04U, 0x02U, 0x00U, 0x00U, 0x00U },
    /* 'a' */ [65U] = { 0x00U, 0x00U, 0x00U, 0x0EU, 0x10U, 0x1EU, 0x11U, 0x1EU, 0x00U, 0x00U },
    /* 'b' */ [66U] = { 0x00U, 0x01U, 0x01U, 0x0DU, 0x13U, 0x11U, 0x11U, 0x0FU, 0x00U, 0x00U },
    /* 'c' */ [67U] = { 0x00U, 0x00U, 0x00U, 0x0EU, 0x01U, 0x01U, 0x11U, 0x0EU, 0x00U, 0x00U },
    /* 'd' */ [68U] = { 0x00U, 0x10U, 0x10U, 0x16U, 0x19U, 0x11U, 0x11U, 0x1EU, 0x00U, 0x00U },
    /* 'e' */ [69U] = { 0x00U, 0x00U, 0x00U, 0x0EU, 0x11U, 0x1FU, 0x01U, 0x0EU, 0x00U, 0x00U },
    /* 'f' */ [70U] = { 0x00U, 0x0CU, 0x12U, 0x02U, 0x07U, 0x02U, 0x02U, 0x02U, 0x00U, 0x00U },
    /* 'g' */ [71U] = { 0x00U, 0x00U, 0x00U, 0x1EU, 0x11U, 0x11U, 0x1EU, 0x10U, 0x10U, 0x0EU },
    /* 'h' */ [72U] = { 0x00U, 0x01U, 0x01U, 0x0DU, 0x13U, 0x11U, 0x11U, 0x11U, 0x00U, 0x00U },
    /* 'i' */ [73U] = { 0x00U, 0x04U, 0x00U, 0x06U, 0x04U, 0x04U, 0x04U, 0x0EU, 0x00U, 0x00U },
    /* 'j' */ [74U] = { 0x00U, 0x08U, 0x00U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x09U, 0x06U },
    /* 'k' */ [75U] = { 0x00U, 0x01U, 0x01U, 0x09U, 0x05U, 0x03U, 0x05U, 0x09U, 0x00U, 0x00U },
    /* 'l' */ [76U] = { 0x00U, 0x06U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x0EU, 0x00U, 0x00U },
    /* 'm' */ [77U] = { 0x00U, 0x00U, 0x00U, 0x0BU, 0x15U, 0x15U, 0x11U, 0x11U, 0x00U, 0x00U },
    /* 'n' */ [78U] = { 0x00U, 0x00U, 0x00U, 0x0DU, 0x13U, 0x11U, 0x11U, 0x11U, 0x00U, 0x00U },
    /* 'o' */ [79U] = { 0x00U, 0x00U, 0x00U, 0x0EU, 0x11U, 0x11U, 0x11U, 0x0EU, 0x00U, 0x00U },
    /* 'p' */ [80U] = { 0x00U, 0x00U, 0x00U, 0x0FU, 0x11U, 0x11U, 0x0FU, 0x01U, 0x01U, 0x01U },
    /* 'q' */ [81U] = { 0x00U, 0x00U, 0x00U, 0x16U, 0x19U, 0x19U, 0x16U, 0x10U, 0x10U, 0x10U },
    /* 'r' */ [82U] = { 0x00U, 0x00U, 0x00U, 0x0DU, 0x13U, 0x01U, 0x01U, 0x01U, 0x00U, 0x00U },
    /* 's' */ [83U] = { 0x00U, 0x00U, 0x00U, 0x0EU, 0x01U, 0x0EU, 0x10U, 0x0FU, 0x00U, 0x00U },
    /* 't' */ [84U] = { 0x00U, 0x02U, 0x07U, 0x02U, 0x02U, 0x02U, 0x12U, 0x0CU, 0x00U, 0x00U },
    /* 'u' */ [85U] = { 0x00U, 0x00U, 0x00U, 0x11U, 0x11U, 0x11U, 0x19U, 0x16U, 0x00U, 0x00U },
    /* 'v' */ [86U] = { 0x00U, 0x00U, 0x00U, 0x11U, 0x11U, 0x11U, 0x0AU, 0x04U, 0x00U, 0x00U },
    /* 'w' */ [87U] = { 0x00U, 0x00U, 0x00U, 0x11U, 0x11U, 0x15U, 0x15U, 0x0BU, 0x00U, 0x00U },
    /* 'x' */ [88U] = { 0x00U, 0x00U, 0x00U, 0x11U, 0x0AU, 0x04U, 0x0AU, 0x11U, 0x00U, 0x00U },
    /* 'y' */ [89U] = { 0x00U, 0x00U, 0x00U, 0x11U, 0x11U, 0x11U, 0x1EU, 0x10U, 0x10U, 0x0EU },
    /* 'z' */ [90U] = { 0x00U, 0x00U, 0x00U, 0x1FU, 0x08U, 0x04U, 0x02U, 0x1FU, 0x00U, 0x00U },
    /* '{' */ [91U] = { 0x08U, 0x04U, 0x04U, 0x02U, 0x04U, 0x04U, 0x08U, 0x00U, 0x00U, 0x00U },
    /* '|' */ [92U] = { 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x00U, 0x00U, 0x00U },
    /* '}' */ [93U] = { 0x02U, 0x04U, 0x04U, 0x08U, 0x04U, 0x04U, 0x02U, 0x00U, 0x00U, 0x00U },
    /* '~' */ [94U] = { 0x00U, 0x00U, 0x00U, 0x08U, 0x15U, 0x02U, 0x00U, 0x00U, 0x00U, 0x00U },
};

static void SvcOsd_BufInit(UINT32 Chan, const SVC_OSD_CONFIG_s *pConfig);
static void SvcOsd_DspOsdConfig(UINT32 Chan, const UINT8 *pOsdBuf);
static void SvcOsd_AsciiDraw(const UINT8 *pAsciiPtn, UINT8 ColorVal, UINT32 BufPitch, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuf);

static inline void Svc_VoutOsdCtrl(UINT32 Chan, UINT32 Enable)
{
    UINT64 AttachedRawSeq;
    (void)AmbaDSP_VoutOsdCtrl((UINT8)Chan, (UINT8)Enable, 0U/*SyncWithVin*/, &AttachedRawSeq);
}

static UINT32 SvcOsd_LoadRomData(const void *pDestBuff, UINT32 BufSize, const char *pPath)
{
    UINT32 Rval;
    UINT32 FileSize;
    UINT8 *pDataBuf = NULL;

    Rval = SvcNvm_GetRomFileSize(AMBA_USER_PARTITION_SYS_DATA, pPath, &FileSize); //FIXME, fileSize should be a parameter
    if((Rval == OK) && (FileSize != 0U) && (BufSize >= FileSize)){
        AmbaMisra_TypeCast(&pDataBuf, &pDestBuff);
        Rval = SvcNvm_ReadRomFileCrc(AMBA_USER_PARTITION_SYS_DATA, pPath, 0U, FileSize, pDataBuf, 5000);
    } else {
        AmbaPrint_PrintUInt5("SvcOsd_LoadRomData(): BufSize=%u FileSize=%u", BufSize, FileSize, 0U, 0U, 0U);
        Rval = SVC_NG;
    }

    if(Rval != 0U) {
        AmbaPrint_PrintStr5("Load %s from ROMFS error", pPath, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

/**
 * Svc Osd initialization function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] pConfig Used for configuring SvcOsd module
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_Init(UINT32 Chan, const SVC_OSD_CONFIG_s *pConfig)
{
    UINT32                 Rval = SVC_OK;
    static UINT32          Init = 0U;

    if (Init == 0U) {
        /* AmbaDraw_Init */
#if defined(CONFIG_BUILD_COMMON_SERVICE_DRAW)
        {
            static UINT8  InitAmbaDrawBuf[10240] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

            AMBA_DRAW_INIT_CONFIG_s  InitConfig;
            ULONG                    AddrU32;
            UINT32                   NeededSize;

            if (SVC_OK != AmbaDraw_GetInitDefaultCfg(&InitConfig)) {
                SvcLog_NG(SVC_LOG_OSD, "Fail to AmbaDraw_GetInitDefaultCfg", 0U, 0U);
                Rval = SVC_NG;
            }

            AmbaSvcWrap_MisraMemset(InitAmbaDrawBuf, 0, sizeof(InitAmbaDrawBuf));
            InitConfig.MaxLineNum   = 10U;
            InitConfig.MaxRectNum   = 10U;
            InitConfig.MaxStringNum = 10U;
            InitConfig.MaxCircleNum = 10U;
            InitConfig.BufferAddr   = InitAmbaDrawBuf;
            InitConfig.BufferSize   = sizeof(InitAmbaDrawBuf);
            InitConfig.FontAddr     = g_SvcOsdFontBuf;

            if (SVC_OK != SvcOsd_LoadRomData(InitConfig.FontAddr, SVC_MAX_FONTS_BIN_FILE_SIZE, "fonts.bin")) {
                SvcLog_NG(SVC_LOG_OSD, "Fail to SvcOsd_LoadRomData", 0U, 0U);
                Rval = SVC_NG;
            } else {
                const AMBA_DRAW_BMPFONT_BIN_HEADER_s *pBmpFont = NULL;
                AmbaMisra_TypeCast(&pBmpFont, &InitConfig.FontAddr);
                InitConfig.MaxFontPageNum = pBmpFont->Pages;
            }

            if (AmbaDraw_GetInitBufferSize(&InitConfig, &NeededSize) == SVC_OK) {
                if (InitConfig.BufferSize < NeededSize) {
                    SvcLog_NG(SVC_LOG_OSD, "alloc/needed(%u/%u) size", InitConfig.BufferSize, NeededSize);
                    Rval = ERR_ARG;
                }
            }

            InitConfig.FontBufSize  = SVC_MAX_FONTS_BIN_FILE_SIZE;
            if (SVC_OK != AmbaDraw_Init(&InitConfig)) {
                SvcLog_NG(SVC_LOG_OSD, "Fail to AmbaDraw_Init", 0U, 0U);
                Rval = SVC_NG;
            } else {
                AmbaMisra_TypeCast(&AddrU32, &InitConfig.BufferAddr);
                SvcLog_OK(SVC_LOG_OSD, "AmbaDraw_Init(%u) success: OsdBuffer:0x%08x", Chan, AddrU32);
            }
        }
#endif

        Init = 1U;
    }

#if defined(CONFIG_BUILD_COMMON_SERVICE_DRAW)
    {
        AMBA_DRAW_LINE_CFG_s    LineObjCfg = {0, 0, 100, 100, 2U, 0U, .LineColor.Data = {0U}};
        AMBA_DRAW_RECT_CFG_s    RectObjCfg = {100U, 100U, 2U, .LineColor.Data = {0U}, .FillColor.Data = {0U}, .Shadow.Enable = 0};
        AMBA_DRAW_STRING_CFG_s  StrObjCfg = {128U, 54U, .Color.Data = {0U}, (UINT8)AMBA_DRAW_ALIGN_V_TOP | (UINT8)AMBA_DRAW_ALIGN_H_LEFT, 0U, {'\0'}, .Shadow.Enable = 0};
        AMBA_DRAW_CIRCLE_CFG_s  CircleObjCfg = {100U, 2U, .LineColor.Data = {0U}, .FillColor.Data = {0U}};

        if (SVC_OK == Rval) {
            if (SVC_OK != AmbaDraw_CreateLine(&LineObjCfg, &g_SvcOsdCtrl[Chan].LineObjId)) {
                SvcLog_NG(SVC_LOG_OSD, "Fail to AmbaDraw_CreateLine", 0U, 0U);
                Rval = SVC_NG;
            }
        }

        if (SVC_OK == Rval) {
            if (SVC_OK != AmbaDraw_CreateRect(&RectObjCfg, &g_SvcOsdCtrl[Chan].RectObjId)) {
                SvcLog_NG(SVC_LOG_OSD, "Fail to AmbaDraw_CreateRect", 0U, 0U);
                Rval = SVC_NG;
            }
        }

        if (SVC_OK == Rval) {
            if (SVC_OK != AmbaDraw_CreateString(&StrObjCfg, &g_SvcOsdCtrl[Chan].StrObjId)) {
                SvcLog_NG(SVC_LOG_OSD, "Fail to AmbaDraw_CreateString", 0U, 0U);
                Rval = SVC_NG;
            }
        }

        if (SVC_OK == Rval) {
            if (SVC_OK != AmbaDraw_CreateCircle(&CircleObjCfg, &g_SvcOsdCtrl[Chan].CircleObjId)) {
                SvcLog_NG(SVC_LOG_OSD, "Fail to AmbaDraw_CreateCircle", 0U, 0U);
                Rval = SVC_NG;
            }
        }
    }
#endif

    AmbaMisra_TouchUnused(&Rval);

    if (Rval == SVC_OK) {
        if (Chan < SVC_OSD_CHAN_NUM) {
            AmbaSvcWrap_MisraMemcpy(&g_SvcOsdConfig[Chan], pConfig, sizeof(SVC_OSD_CONFIG_s));
            if (Chan == 0U) {
                g_SvcOsdConfig[Chan].OsdSize.Width  = GetMinValU16(g_SvcOsdConfig[Chan].OsdSize.Width, SVC_OSD_BUF_MAX_WIDTH_VOUT_A);
                g_SvcOsdConfig[Chan].OsdSize.Height = GetMinValU16(g_SvcOsdConfig[Chan].OsdSize.Height, SVC_OSD_BUF_MAX_HEIGHT_VOUT_A);
                g_SvcOsdConfig[Chan].OsdSize.Width  = (UINT16) GetAlignedValU32(g_SvcOsdConfig[Chan].OsdSize.Width, 32U);
                SvcLog_DBG(SVC_LOG_OSD, "Because of HW limitation, VOUT_A  OSD window size (Width, Height) will be adjust to (%d, %d)", g_SvcOsdConfig[Chan].OsdSize.Width, g_SvcOsdConfig[Chan].OsdSize.Height);
            } else {    /* Chan == 1U */
                if ((pConfig->OsdSize.Width != SVC_OSD_BUF_MAX_WIDTH_VOUT_B) || (pConfig->OsdSize.Height != SVC_OSD_BUF_MAX_HEIGHT_VOUT_B)) {
                    g_SvcOsdConfig[Chan].OsdSize.Width  = SVC_OSD_BUF_MAX_WIDTH_VOUT_B;
                    g_SvcOsdConfig[Chan].OsdSize.Height = SVC_OSD_BUF_MAX_HEIGHT_VOUT_B;
                    g_SvcOsdConfig[Chan].OsdSize.Width  = (UINT16) GetAlignedValU32(g_SvcOsdConfig[Chan].OsdSize.Width, 32U);
                    SvcLog_DBG(SVC_LOG_OSD, "Because of CV limitation, VOUT_B OSD window size (Width, Height) must bt (%d, %d)", g_SvcOsdConfig[Chan].OsdSize.Width, g_SvcOsdConfig[Chan].OsdSize.Height);
                }
            }

            g_SvcOsdBuffer[Chan].MemType = g_SvcOsdConfig[Chan].MemType;
            for (UINT32 BufIdx = 0U; BufIdx < SVC_OSD_BUF_NUM; BufIdx++) {
                UINT32 AddrOffset = (UINT32) g_SvcOsdConfig[Chan].OsdSize.Width * (UINT32) g_SvcOsdConfig[Chan].OsdSize.Height * BufIdx;
                AddrOffset = AddrOffset << SVC_OSD_PIXEL_SIZE_SFT;
#if !defined(CONFIG_ICAM_32BITS_OSD_USED)
                AddrOffset += (sizeof(UINT32) * SVC_OSD_CLUT_ENTRY_SIZE);
#endif
                g_SvcOsdBuffer[Chan].pAddr[BufIdx] = &g_SvcOsdConfig[Chan].pOsdMem[AddrOffset];
            }

            if (SVC_OK != AmbaWrap_memset(&g_SvcOsdCtrl[Chan].DrawBufInfo, 0, sizeof(AMBA_DRAW_BUFFER_INFO_s))) {
                SvcLog_NG(SVC_LOG_OSD, "Fail to AmbaWrap_memset", 0U, 0U);
                Rval = SVC_NG;
            }
            g_SvcOsdCtrl[Chan].DrawBufInfo.PixelFormat = SVC_OSD_MODE;
            g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr     = g_SvcOsdBuffer[Chan].pAddr[0];  /* Draw on second buffer first. 1st buffer will be empty */
            g_SvcOsdCtrl[Chan].DrawBufInfo.Width       = g_SvcOsdConfig[Chan].OsdSize.Width;
            g_SvcOsdCtrl[Chan].DrawBufInfo.Pitch       = g_SvcOsdConfig[Chan].OsdSize.Width;
            g_SvcOsdCtrl[Chan].DrawBufInfo.Height      = g_SvcOsdConfig[Chan].OsdSize.Height;
            g_SvcOsdCtrl[Chan].FrameSize = (g_SvcOsdCtrl[Chan].DrawBufInfo.Pitch *
                                            g_SvcOsdCtrl[Chan].DrawBufInfo.Height);


            SvcOsd_BufInit(Chan, pConfig);
            SvcOsd_DspOsdConfig(Chan, g_SvcOsdBuffer[Chan].pAddr[0]);
        }
    }

    return Rval;
}

/**
 * Svc Osd draw rectangle function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] Xstart horizontal position X of drawing start
 * @param [in] Ystart vertical position Y of drawing start
 * @param [in] Xend horizontal position X of drawing start
 * @param [in] Yend vertical position Y of drawing start
 * @param [in] Color drawing color
 * @param [in] Thickness thichness of line
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_DrawRect(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, UINT32 Color, UINT32 Thickness)
{
    UINT32 Rval = SVC_OK;
#if defined(CONFIG_BUILD_COMMON_SERVICE_DRAW)
    AMBA_DRAW_RECT_CFG_s RectObjCfg = {0U};
    UINT32 MinX, DiffX;
    UINT32 MinY, DiffY;

    if (g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr == NULL) {
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawRect(%u): pRGBAddr == NULL", Chan, 0U);
        Rval = SVC_NG;
    }
    if (Xend >= Xstart) {
        MinX = Xstart;
        DiffX = Xend - Xstart;
    } else {
        MinX = Xend;
        DiffX = Xstart - Xend;
    }
    if (Yend >= Ystart) {
        MinY = Ystart;
        DiffY = Yend - Ystart;
    } else {
        MinY = Yend;
        DiffY = Ystart - Yend;
    }

    /* correct width and height to avoid draw over boundary. */
    if (MinX < Thickness) {
        MinX = Thickness;
        DiffX = DiffX - (Thickness - MinX);
    }
    if (MinY < Thickness) {
        MinY = Thickness;
        DiffY = DiffY - (Thickness - MinY);
    }

    if ((MinX + DiffX + Thickness) > g_SvcOsdCtrl[Chan].DrawBufInfo.Width) {
        DiffX = g_SvcOsdCtrl[Chan].DrawBufInfo.Width - MinX - Thickness;
    }
    if ((MinY + DiffY + Thickness) > g_SvcOsdCtrl[Chan].DrawBufInfo.Height) {
        DiffY = g_SvcOsdCtrl[Chan].DrawBufInfo.Height - MinY - Thickness;
    }

    RectObjCfg.Width             = DiffX;
    RectObjCfg.Height            = DiffY;
    RectObjCfg.LineWidth         = Thickness;
    RectObjCfg.Shadow.Enable     = 0U;
    RectObjCfg.LineColor.Data[0] = (UINT8)Color;
    RectObjCfg.LineColor.Data[1] = (UINT8)(Color >> 8U);
    RectObjCfg.LineColor.Data[2] = (UINT8)(Color >> 16U);
    RectObjCfg.LineColor.Data[3] = (UINT8)(Color >> 24U);

    /*AmbaPrint_PrintUInt5("AmbaDraw_UpdateRect Chan %u X %u Y %u W %u H %u",
                          Chan,
                          MinX,
                          MinY,
                          RectObjCfg.Width,
                          RectObjCfg.Height);*/

    if (SVC_OK == Rval) {
        if (SVC_OK != AmbaDraw_UpdateRect(&RectObjCfg, g_SvcOsdCtrl[Chan].RectObjId)) {
            SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawRect(%u): Update NG", 0U, 0U);
            Rval = SVC_NG;
        }
    }

    if (SVC_OK == Rval) {
        if (SVC_OK != AmbaDraw_DrawBuffer(g_SvcOsdCtrl[Chan].RectObjId, (INT32)MinX, (INT32)MinY, &g_SvcOsdCtrl[Chan].DrawBufInfo, NULL, 0U)) {
            SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawRect(%u): Draw NG", 0U, 0U);
            Rval = SVC_NG;
        }
    }
#else
    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&Xstart);
    AmbaMisra_TouchUnused(&Ystart);
    AmbaMisra_TouchUnused(&Xend);
    AmbaMisra_TouchUnused(&Yend);
    AmbaMisra_TouchUnused(&Color);
    AmbaMisra_TouchUnused(&Thickness);
#endif

    return Rval;
}

/**
 * Svc Osd draw solid rectangle function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] Xstart horizontal position X of drawing start
 * @param [in] Ystart vertical position Y of drawing start
 * @param [in] Xend horizontal position X of drawing start
 * @param [in] Yend vertical position Y of drawing start
 * @param [in] Color drawing color
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_DrawSolidRect(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, UINT32 Color)
{
    UINT32 RetVal = SVC_OK;

    if (Chan < SVC_OSD_CHAN_NUM) {
        UINT32 i;
        UINT32 X1, X2, X;

        UINT8 *pRGBAddr;
        UINT8 *pCleanAddr;
        INT32 Color32;

        AmbaMisra_TypeCast(&pRGBAddr, &g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr);
        Color32 = (INT32)Color;

        X1 = Xstart;
        X2 = Xend;

        if (X1 > X2) {
            X = X1;
            X1 = X2;
            X2 = X;
        }

        if((Xstart > g_SvcOsdConfig[Chan].OsdSize.Width) ||  (Ystart > g_SvcOsdConfig[Chan].OsdSize.Height) ){
            SvcLog_NG(SVC_LOG_OSD, "[SvcOsd_DrawSolidRect] Xstart:[%u], Ystart:[%u]", Xstart, Ystart);
            RetVal = SVC_NG;
        }

        if( (Xend > g_SvcOsdConfig[Chan].OsdSize.Width) || (Yend > g_SvcOsdConfig[Chan].OsdSize.Height) ){
            SvcLog_NG(SVC_LOG_OSD, "[SvcOsd_DrawSolidRect] Xend:[%u], Yend:[%u]", Xend, Yend);
            RetVal = SVC_NG;
        }

        if (RetVal == SVC_OK){
            for (i = Ystart; i < Yend; i++) {
                pCleanAddr = &pRGBAddr[(i * g_SvcOsdConfig[Chan].OsdSize.Width) + X1];
                RetVal = AmbaWrap_memset(pCleanAddr, Color32, sizeof(UINT8) * (X2 - X1));
                if (SVC_NG == RetVal) {
                    break;
                }
            }

            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_OSD, "SvcOsd_ClearBlock() failed. AmbaWrap_memset err with %d", RetVal, 0U);
            }
        }
    }

    return RetVal;
}

/**
 * Svc Osd draw string function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] Xstart horizontal position X of drawing start
 * @param [in] Ystart vertical position Y of drawing start
 * @param [in] FontSize font size
 * @param [in] Color drawing color
 * @param [in] pString string to be shown with OSD
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_DrawString(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 FontSize, UINT32 Color, const char *pString)
{
    UINT32 Rval = SVC_OK;

    if (Chan >= SVC_OSD_CHAN_NUM) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawString() failed. invalid chan(%d)", Chan, 0U);
    }

    if (pString == NULL) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawString() failed. invalid pString", 0U, 0U);
    }

    if ((Rval == SVC_OK) && (pString != NULL) && (Chan < SVC_OSD_CHAN_NUM)) {
        UINT8 *pBuf = NULL;
        AmbaMisra_TypeCast(&(pBuf), &(g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr));

        if (pBuf == NULL) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawString() failed. invalid osd buffer!", 0U, 0U);
        } else {
            UINT32 StrLen;
            UINT32 CharWidth, CharHeight, GapWidth, StrWidth, StrHeight;
            UINT32 BufPitch;

            StrLen = SvcWrap_strlen(pString);
            GapWidth   = FontSize * SVC_OSD_ASCII_PADDING_WIDTH;
            CharWidth  = FontSize * SVC_OSD_ASCII_PTN_COL;
            CharHeight = FontSize * SVC_OSD_ASCII_PTN_ROW;
            StrWidth   = ( CharWidth + GapWidth ) * StrLen;
            StrHeight  = CharHeight;

            if ((Ystart + StrHeight) > g_SvcOsdConfig[Chan].OsdSize.Height) {
                Rval = SVC_NG;
                SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawString() failed. draw height(%d) is out-of range(%d)!", (Ystart + StrHeight), g_SvcOsdConfig[Chan].OsdSize.Height);
            }

            if ((Xstart + CharWidth) > g_SvcOsdConfig[Chan].OsdSize.Width) {
                Rval = SVC_NG;
                SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawString() failed. draw width(%d) is out-of range(%d)!", (Xstart + CharWidth), g_SvcOsdConfig[Chan].OsdSize.Width);
            } else {
                if ((Xstart + StrWidth) > g_SvcOsdConfig[Chan].OsdSize.Width) {
                    StrWidth = g_SvcOsdConfig[Chan].OsdSize.Width - Xstart;
                    StrLen   = StrWidth / ( CharWidth + GapWidth );
                }
            }

            if (Rval == SVC_OK) {
                UINT32 Idx, CharVal, PtnIdx;
                UINT8  ColorVal[4];
                UINT8 *pCurBuf;

                ColorVal[0] = (UINT8)Color;
                BufPitch = g_SvcOsdCtrl[Chan].DrawBufInfo.Pitch;
                BufPitch &= 0x0000FFFFU;

                for (Idx = 0U; Idx < StrLen; Idx ++) {
                    pCurBuf = &(pBuf[((BufPitch * Ystart) + Xstart)+(Idx * (CharWidth + GapWidth ))]);
                    CharVal = (UINT32)pString[Idx];
                    CharVal &= 0x000000FFU;

                    if ((CharVal >= SVC_OSD_ASCII_BEGIN) && (CharVal <= SVC_OSD_ASCII_END)) {
                        PtnIdx = CharVal - SVC_OSD_ASCII_BEGIN; PtnIdx &= 0x000000FFU;
                        SvcOsd_AsciiDraw(SvcGuiPalAsciiPtn[PtnIdx], ColorVal[0], BufPitch, CharWidth, CharHeight, pCurBuf);
                    } else {
                        SvcLog_DBG(SVC_LOG_OSD, "SvcOsd_DrawString() warning. not support ascii char(%d)", CharVal, 0U);
                    }
                }
            }
        }
    }

    return Rval;
}

UINT32 SvcOsd_GetFontRes(UINT32 font_size, UINT32 *Width, UINT32 *Height)
{

    UINT32 RetVal=SVC_OK;

    if (font_size == 0U) {
        RetVal = SVC_NG;
    } else {
        if((Width != NULL) && (Height != NULL)) {
            *Width = font_size * (SVC_OSD_ASCII_PTN_COL + SVC_OSD_ASCII_PADDING_WIDTH);
            *Height = font_size * SVC_OSD_ASCII_PTN_ROW;
        } else{
            RetVal = SVC_NG;
        }
    }

    return RetVal;
}

/**
 * Svc Osd draw circle function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] Xstart horizontal position X of drawing start
 * @param [in] Ystart vertical position Y of drawing start
 * @param [in] Radius radius of circle
 * @param [in] LineWidth line width
 * @param [in] Color drawing color
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_DrawCircle(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Radius, UINT32 LineWidth, UINT32 Color)
{
    UINT32                   Rval = SVC_OK;
#if defined(CONFIG_BUILD_COMMON_SERVICE_DRAW)
    AMBA_DRAW_CIRCLE_CFG_s   ObjCirCle = {0U};

    ObjCirCle.Radius            = Radius;
    ObjCirCle.LineWidth         = LineWidth;
    ObjCirCle.LineColor.Data[0] = (UINT8)Color;
    ObjCirCle.LineColor.Data[1] = (UINT8)(Color >> 8U);
    ObjCirCle.LineColor.Data[2] = (UINT8)(Color >> 16U);
    ObjCirCle.LineColor.Data[3] = (UINT8)(Color >> 24U);

    if (SVC_OK != AmbaDraw_UpdateCircle(&ObjCirCle, g_SvcOsdCtrl[Chan].CircleObjId)) {
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawCircle(%u): Update NG", 0U, 0U);
        Rval = SVC_NG;
    }

    if (SVC_OK == Rval){
        if (SVC_OK != AmbaDraw_DrawBuffer(g_SvcOsdCtrl[Chan].CircleObjId, (INT32)Xstart, (INT32)Ystart, &g_SvcOsdCtrl[Chan].DrawBufInfo, NULL, 0U)) {
            SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawString(%u): Draw NG", 0U, 0U);
            Rval = SVC_NG;
        }
    }
#else
    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&Xstart);
    AmbaMisra_TouchUnused(&Ystart);
    AmbaMisra_TouchUnused(&Radius);
    AmbaMisra_TouchUnused(&LineWidth);
    AmbaMisra_TouchUnused(&Color);
#endif
    return Rval;
}

/**
 * Svc Osd draw line function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] X1 X coordinate of the top-left or the top-right end-point of the line, depending on the line's angle. (correspond to the top-left corner of the object)
 * @param [in] Y1 Y coordinate of the top-left or the top-right end-point of the line, depending on the line's angle. (correspond to the top-left corner of the object)
 * @param [in] X2 X coordinate of the bottom-left or the bottom-right end-point of the line, depending on the line's angle. (correspond to the top-left corner of the object)
 * @param [in] Y2 Y coordinate of the bottom-left or the bottom-right end-point of the line, depending on the line's angle. (correspond to the top-left corner of the object)
 * @param [in] LineWidth line width
 * @param [in] Color drawing color
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_DrawLine(UINT32 Chan, UINT32 X1, UINT32 Y1, UINT32 X2, UINT32 Y2, UINT32 LineWidth, UINT32 Color)
{
    UINT32               Rval = SVC_OK;
#if defined(CONFIG_BUILD_COMMON_SERVICE_DRAW)
    INT32                MinX, MinY;
    INT32                X_1, Y_1, X_2, Y_2;
    AMBA_DRAW_LINE_CFG_s ObjCfg = {0};

    if (Y2 > Y1) {
        X_1 = (INT32)X1;
        X_2 = (INT32)X2;
        Y_1 = (INT32)Y1;
        Y_2 = (INT32)Y2;
    } else {
        X_1 = (INT32)X2;
        X_2 = (INT32)X1;
        Y_1 = (INT32)Y2;
        Y_2 = (INT32)Y1;
    }


    if (X_2 > X_1) {
        ObjCfg.X1 = 0;
        ObjCfg.Y1 = 0;
        ObjCfg.X2 = X_2-X_1;
        ObjCfg.Y2 = Y_2-Y_1;
        MinX = X_1;
        MinY = Y_1;
    } else {
        ObjCfg.X1 = X_1-X_2;
        ObjCfg.Y1 = 0;
        ObjCfg.X2 = 0;
        ObjCfg.Y2 = Y_2-Y_1;
        MinX = X_2;
        MinY = Y_1;
    }

    ObjCfg.LineWidth = LineWidth;
    ObjCfg.DashInterval = 0;
    ObjCfg.LineColor.Data[0] = (UINT8)Color;
    ObjCfg.LineColor.Data[1] = (UINT8)(Color >> 8U);
    ObjCfg.LineColor.Data[2] = (UINT8)(Color >> 16U);
    ObjCfg.LineColor.Data[3] = (UINT8)(Color >> 24U);


    //Do not draw
    if ((ObjCfg.X2 == ObjCfg.X1) && (ObjCfg.Y2 == ObjCfg.Y1)) {
        Rval = SVC_NG;
    }

    if (SVC_OK == Rval){
        if (SVC_OK != AmbaDraw_UpdateLine(&ObjCfg, g_SvcOsdCtrl[Chan].LineObjId)) {
            SvcLog_NG(SVC_LOG_OSD, "AmbaDraw_UpdateLine(%u): Update NG", 0U, 0U);
            AmbaPrint_PrintUInt5("[Ori] [X1, X2, Y1, Y2] %d, %d, %d, %d",
                    X1 ,
                    X2 ,
                    Y1 ,
                    Y2 ,
                    0U);
            AmbaPrint_PrintInt5("[Update] [X_1, X_2, Y_1, Y_2] %d, %d, %d, %d",
                    X_1 ,
                    X_2 ,
                    Y_1 ,
                    Y_2 ,
                    0);
            AmbaPrint_PrintInt5("[ObjCfg] [X1, X2, Y1, Y2] %d, %d, %d, %d",
                    ObjCfg.X1 ,
                    ObjCfg.X2 ,
                    ObjCfg.Y1 ,
                    ObjCfg.Y2 ,
                    0);
            Rval = SVC_NG;
        }
    }

    if (SVC_OK == Rval){
        if (SVC_OK != AmbaDraw_DrawBuffer(g_SvcOsdCtrl[Chan].LineObjId, (INT32)MinX, (INT32)MinY, &g_SvcOsdCtrl[Chan].DrawBufInfo, NULL, 0U)) {
            SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawLine(%u): Draw NG", 0U, 0U);
            AmbaPrint_PrintUInt5("[Ori] [X1, X2, Y1, Y2] %d, %d, %d, %d",
                    X1 ,
                    X2 ,
                    Y1 ,
                    Y2 ,
                    0U);
            AmbaPrint_PrintInt5("[Update] [X_1, X_2, Y_1, Y_2] %d, %d, %d, %d",
                    X_1 ,
                    X_2 ,
                    Y_1 ,
                    Y_2 ,
                    0);
            AmbaPrint_PrintInt5("[ObjCfg X1, X2, Y1, Y2] %d, %d, %d, %d",
                    ObjCfg.X1 ,
                    ObjCfg.X2 ,
                    ObjCfg.Y1 ,
                    ObjCfg.Y2 ,
                    0);
            AmbaPrint_PrintInt5("[MinX, MinY] %d, %d",
                    MinX ,
                    MinY ,
                    0 ,
                    0 ,
                    0);
            Rval = SVC_NG;
        }
    }
#else
    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&X1);
    AmbaMisra_TouchUnused(&Y1);
    AmbaMisra_TouchUnused(&X2);
    AmbaMisra_TouchUnused(&Y2);
    AmbaMisra_TouchUnused(&LineWidth);
    AmbaMisra_TouchUnused(&Color);
#endif
    return Rval;
}

/**
 * Svc Osd clear function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_Clear(UINT32 Chan)
{
    if (Chan < SVC_OSD_CHAN_NUM) {
        AmbaSvcWrap_MisraMemset(g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr, 0, g_SvcOsdCtrl[Chan].FrameSize);
    }

    return SVC_OK;
}

/**
 * Svc Osd clear block function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] Xstart horizontal position X of drawing start
 * @param [in] Ystart vertical position Y of drawing start
 * @param [in] Xend horizontal position X of drawing end
 * @param [in] Yend vertical position Y of drawing end
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_ClearBlock(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend)
{
    UINT32 RetVal = SVC_NG;
    UINT32 i;

    UINT8 *pRGBAddr;
    UINT8 *pCleanAddr;

    AmbaMisra_TypeCast(&pRGBAddr, &g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr);

    for (i = Ystart; i < Yend; i++) {
        pCleanAddr = &pRGBAddr[(i * g_SvcOsdCtrl[Chan].DrawBufInfo.Pitch) + Xstart];
        RetVal = AmbaWrap_memset(pCleanAddr, 0, sizeof(UINT8) * (Xend - Xstart));
        if (SVC_NG == RetVal) {
            break;
        }
    }

    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_ClearBlock() failed. AmbaWrap_memset err with %d", RetVal, 0U);
    }

    return RetVal;
}

/**
 * Svc Osd load bitmap function
 * @param [in] pDestBuf user defined buffer to load bitmap
 * @param [in] BufSize buffer size of pDestBuf
 * @param [in] pPath bitmap path in ROM
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_LoadBmp(void *pDestBuf, UINT32 BufSize, const char *pPath)
{
    UINT32 RetVal;
    UINT32 Offset;

    if (g_BmpBuf == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_LoadBmp failed. initial SvcOsd_Init first", 0U, 0U);
    } else {

        RetVal = SvcOsd_LoadRomData(g_BmpBuf, sizeof(UINT8) * SVC_OSD_BUF_SIZE, pPath);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_OSD, "SvcOsd_LoadBmp failed. Load Rom Data err %d", RetVal, 0U);
        } else {
            /* Find the offset of bmp data */
            RetVal = AmbaWrap_memcpy(&Offset, &g_BmpBuf[10], sizeof(UINT32));

            if (SVC_OK == RetVal) {
                RetVal = AmbaWrap_memcpy(pDestBuf, &g_BmpBuf[Offset], sizeof(UINT8) * BufSize);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_OSD, "SvcOsd_LoadBmp failed. memcpy err %d", RetVal, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_OSD, "SvcOsd_LoadBmp failed. Load rom data err %d", RetVal, 0U);
            }
        }
    }

    return RetVal;
}

/**
 * Svc Osd load bitmap function
 * @param [in] pDestBuf user defined buffer to load bitmap
 * @param [in] Pitch pDestBuf pitch
 * @param [in] Width pDestBuf Width
 * @param [in] Height pDestBuf Height
 * @param [in] pPath bitmap path in ROM
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_LoadBmpReverse(void *pDestBuf, UINT32 Pitch, UINT32 Width, UINT32 Height, const char *pPath)
{
    UINT32 RetVal;
    UINT32 Offset;
    UINT8  *pBuf;

    AmbaMisra_TypeCast(&pBuf, &pDestBuf);


    if (g_BmpBuf == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_LoadBmpReverse failed. initial SvcOsd_Init first", 0U, 0U);
    } else {

        RetVal = SvcOsd_LoadRomData(g_BmpBuf, sizeof(UINT8) * SVC_OSD_BUF_SIZE, pPath);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_OSD, "SvcOsd_LoadBmpReverse failed. Load Rom Data err %d", RetVal, 0U);
        } else {
            /* Find the offset of bmp data */
            RetVal = AmbaWrap_memcpy(&Offset, &g_BmpBuf[10], sizeof(UINT32));

            if (SVC_OK == RetVal) {
                UINT32 i, j;
                AmbaSvcWrap_MisraMemset(pDestBuf, 0, sizeof(UINT8) * Pitch * Height);

                j = Height - 1U;
                for (i = 0U; i < Height; i++) {
                    const UINT8 *pSrcAddr = &g_BmpBuf[Offset + (j * Width)];
                    UINT8 *pDstAddr = &pBuf[i * Pitch];
                    AmbaSvcWrap_MisraMemcpy(pDstAddr, pSrcAddr, sizeof(UINT8) * Width);
                    j = j - 1U;
                }
            } else {
                SvcLog_NG(SVC_LOG_OSD, "SvcOsd_LoadBmpReverse failed. Load rom data err %d", RetVal, 0U);
            }
        }
    }

    return RetVal;
}

/**
 * Svc Osd draw bitmap into buffer function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] Xstart horizontal position X of drawing start
 * @param [in] Ystart vertical position Y of drawing start
 * @param [in] Xend horizontal position X of drawing end
 * @param [in] Yend vertical position Y of drawing end
 * @param [in] pBmpBuf buffer which contains bitmap data
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_DrawBmp(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, const void *pBmpInfo)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i, j;

    UINT32 CopiedBmpWidth = Xend - Xstart;
    UINT32 CopiedBmpHeight = Yend - Ystart;

    UINT8 *pRGBAddr;
    UINT8 *pRGBDstAddr;


    const SVC_OSD_BMP_s *pBmpInfoAddr;
    const UINT8 *pBmpAddr;
    const UINT8 *pBmpSrcAddr;

    if (pBmpInfo == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawBmp(%u): pBmpBuf == NULL", Chan, 0U);
    }

    if(RetVal != SVC_NG) {
        AmbaMisra_TypeCast(&pRGBAddr, &g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr);
        AmbaMisra_TypeCast(&pBmpInfoAddr,&pBmpInfo);
        AmbaMisra_TypeCast(&pBmpAddr, &(pBmpInfoAddr->data));

        j = CopiedBmpHeight - 1U;
        for (i = Ystart; i < Yend; i++) {
            pRGBDstAddr = &pRGBAddr[(i * g_SvcOsdCtrl[Chan].DrawBufInfo.Pitch) + Xstart];
            pBmpSrcAddr = &pBmpAddr[j * CopiedBmpWidth];
            RetVal = AmbaWrap_memcpy(pRGBDstAddr, pBmpSrcAddr, sizeof(UINT8) * CopiedBmpWidth);
            j = j - 1U;
            if (SVC_NG == RetVal) {
                break;
            }
        }

        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawBmp() failed. AmbaWrap_memcpy err with %d", RetVal, 0U);
        }
    }

    return RetVal;
}

/**
 * Svc Osd draw binary data into buffer function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] Xstart horizontal position X of drawing start
 * @param [in] Ystart vertical position Y of drawing start
 * @param [in] Xend horizontal position X of drawing end
 * @param [in] Yend vertical position Y of drawing end
 * @param [in] pBinBuf buffer which contains binary data
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_DrawBin(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, const void *pBinBuf)
{
    UINT32 RetVal = SVC_NG;
    UINT32 i, j;

    UINT32 CopiedBinWidth = Xend - Xstart;

    UINT8 *pRGBAddr;
    UINT8 *pRGBDstAddr;

    const UINT8 *pBinAddr;
    const UINT8 *pBinSrcAddr;

    AmbaMisra_TypeCast(&pRGBAddr, &g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr);
    AmbaMisra_TypeCast(&pBinAddr, &pBinBuf);

    j = 0U;
    for (i = Ystart; i < Yend; i++) {
        pRGBDstAddr = &pRGBAddr[(i * g_SvcOsdCtrl[Chan].DrawBufInfo.Pitch) + Xstart];
        pBinSrcAddr = &pBinAddr[j * CopiedBinWidth];
        RetVal = AmbaWrap_memcpy(pRGBDstAddr, pBinSrcAddr, sizeof(UINT8) * CopiedBinWidth);
        j = j + 1U;
        if (SVC_NG == RetVal) {
            break;
        }
    }

    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawBin() failed. AmbaWrap_memcpy err with %d", RetVal, 0U);
    }

    return RetVal;
}

/**
 * Svc Osd draw binary data into buffer with software rescaling
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] Xstart horizontal position X of drawing start
 * @param [in] Ystart vertical position Y of drawing start
 * @param [in] Xend horizontal position X of drawing end
 * @param [in] Yend vertical position Y of drawing end
 * @param [in] pRescaleCfg configuration of rescaling
 * @return 0-OK, 1-NG
 */
UINT32 SvcOsd_DrawBin_Rescale(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, SVC_OSD_SW_RESCALE_CFG_s *pRescaleCfg)
{
    UINT32 RetVal, OsdPitch;
    ULONG  RGBAddr;
    SVC_BIT_MAP_RESCALE_REPT_s RsCfg;

    AmbaMisra_TouchUnused(pRescaleCfg);

    RsCfg.pSrc             = pRescaleCfg->pSrc;
    RsCfg.SrcWidth         = pRescaleCfg->SrcWidth;
    RsCfg.SrcHeight        = pRescaleCfg->SrcHeight;
    RsCfg.SrcPitch         = pRescaleCfg->SrcPitch;
    RsCfg.pDst             = pRescaleCfg->pDst;
    RsCfg.DstWidth         = pRescaleCfg->DstWidth;
    RsCfg.DstHeight        = pRescaleCfg->DstHeight;
    RsCfg.DstPitch         = pRescaleCfg->DstPitch;
    RsCfg.HorizantalMapLen = pRescaleCfg->HorizantalMapLen;
    RsCfg.pHorizantalMap   = pRescaleCfg->pHorizantalMap;
    RsCfg.VerticalMapLen   = pRescaleCfg->VerticalMapLen;
    RsCfg.pVerticalMap     = pRescaleCfg->pVerticalMap;
    RsCfg.ColorOffset      = pRescaleCfg->ColorOffset;
    RsCfg.pClut            = pRescaleCfg->pClut;
    RsCfg.NumColor         = pRescaleCfg->NumColor;
    RsCfg.SrcStartY        = pRescaleCfg->SrcStartY;

    OsdPitch = ((UINT32)(g_SvcOsdConfig[Chan].OsdSize.Width) << SVC_OSD_PIXEL_SIZE_SFT);
    AmbaMisra_TypeCast(&RGBAddr, &g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr);
    RGBAddr += (Ystart * OsdPitch);
    RGBAddr += (Xstart << SVC_OSD_PIXEL_SIZE_SFT);
    AmbaMisra_TypeCast(&RsCfg.pDst, &RGBAddr);

    RsCfg.DstWidth  = Xend - Xstart;
    RsCfg.DstHeight = Yend - Ystart;
    RsCfg.DstPitch  = OsdPitch;
    RetVal = SvcCvImgUtil_RescaleBitMap_Rept(&RsCfg);

    return RetVal;
}

/**
 * Svc Osd draw segmetation result into buffer with software rescaling
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] Pointer to the segmentation argmax output arrayB
 * @param [in] The ROI width
 * @param [in] The ROI height
 * @param [in] The offset x in ROI domain
 * @param [in] The offset y in ROI domain
 * @param [in] The segmentation network width in ROI domain
 * @param [in] The segmentation network height in ROI domain
 * @return 0-OK, 1-NG
 */
UINT32 SvcOsd_DrawSeg(UINT32 Chan, const UINT8 *pArgMaxOut, UINT32 RoiWidth, UINT32 RoiHeight, UINT32 RoiStartX, UINT32 RoiStartY,
  UINT32 NetworkWidth, UINT32 NetworkHeight)
{
  #define SVC_OSD_SEG_CLUT_START_IDX      (40U)

  UINT32 Rval = SVC_OK;

  /* 1. Sanity check*/
  if (Chan >= SVC_OSD_CHAN_NUM) {
      SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawSeg(%u): invalid Chan", Chan, 0U);
      Rval = SVC_NG;
  } else if (g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr == NULL) {
      SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawSeg(%u): pRGBAddr == NULL", Chan, 0U);
      Rval = SVC_NG;
  } else if (pArgMaxOut == NULL) {
      SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawSeg(%u): pArgMaxOut=NULL", Chan, 0U);
      Rval = SVC_NG;
  } else {
      /* 2. Draw segmentation result pixel by pixel */
      UINT32 osd_width = 0U, osd_height = 0U;
      UINT32 osd_start_x = 0U, osd_start_y = 0U;
      UINT32 osd_network_w = 0U, osd_network_h = 0U;
      UINT32 osd_pixel_idx = 0U, seg_pixel_idx = 0U;
      UINT32 net_pitch = 0U;
      UINT8 *pOsdBuf;

      if (Chan == VOUT_IDX_A) {
        osd_width  = SVC_OSD_BUF_MAX_WIDTH_VOUT_A;
        osd_height = SVC_OSD_BUF_MAX_HEIGHT_VOUT_A;
      } else {
        osd_width  = SVC_OSD_BUF_MAX_WIDTH_VOUT_B;
        osd_height = SVC_OSD_BUF_MAX_HEIGHT_VOUT_B;
      }

      AmbaMisra_TypeCast(&(pOsdBuf), &(g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr));
      net_pitch = (NetworkWidth + 31U) & 0xFFFFFFE0U;

      /* calcuate the coordinate in OSD domain */
      osd_network_w = (NetworkWidth * osd_width) / RoiWidth;
      osd_network_h = (NetworkHeight * osd_height) / RoiHeight;
      osd_start_x   = (RoiStartX * osd_width) / RoiWidth;
      osd_start_y   = (RoiStartY * osd_height) / RoiHeight;

      /* Fill OSD pixel by pixel */
      for (UINT32 idx_h = 0U; idx_h < osd_network_h; idx_h++) {
        for (UINT32 idx_w = 0U; idx_w < osd_network_w; idx_w++) {
          osd_pixel_idx = ((osd_start_y + idx_h) * osd_width) + (osd_start_x + idx_w);
          seg_pixel_idx = (((idx_h * RoiHeight) / osd_height) * net_pitch) + ((idx_w * RoiWidth) / osd_width);
          pOsdBuf[osd_pixel_idx] = (UINT8)(pArgMaxOut[seg_pixel_idx]) + SVC_OSD_SEG_CLUT_START_IDX;
        }
      }
  }

  return Rval;
}

/**
 * Svc Osd flush function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_Flush(UINT32 Chan)
{
    ULONG  AddrU32;
    UINT32 Rval = SVC_OK;

    if ((OSDExtBufferStatus[Chan] & SVC_OSD_EXTBUF_CONFIGED) == 0U) {
        if (g_SvcOsdBuffer[Chan].MemType == SVC_OSD_MEM_TYPE_CACHE) {
            AmbaMisra_TypeCast(&AddrU32, &g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr);
            Rval = SvcPlat_CacheClean(AddrU32, g_SvcOsdCtrl[Chan].FrameSize);
        }

        if (SVC_OK == Rval) {
            UINT32 BufIdx = 0U;
            for (BufIdx = 0U; BufIdx < SVC_OSD_BUF_NUM; BufIdx++) {
                if (g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr == g_SvcOsdBuffer[Chan].pAddr[BufIdx]) {
                    UINT32 NextBufIdx = (BufIdx + 1U) % SVC_OSD_BUF_NUM;
                    SvcOsd_DspOsdConfig(Chan, g_SvcOsdBuffer[Chan].pAddr[BufIdx]);
                    g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr = g_SvcOsdBuffer[Chan].pAddr[NextBufIdx];
                    Svc_VoutOsdCtrl(Chan, 1);
                    break;
                }
            }
            if (BufIdx == SVC_OSD_BUF_NUM) {
                SvcLog_NG(SVC_LOG_OSD, "SvcOsd_Flush err. Unknown OSD buffer", 0U, 0U);
                Rval = SVC_NG;
            }
        }
    }

    return Rval;
}

/**
 * Svc Osd set external color look up table function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] pCLUT customized look up table
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_SetExtCLUT(UINT32 Chan, const UINT32 *pCLUT)
{
    UINT32 Rval = SVC_OK;
    if (Chan >= SVC_OSD_CHAN_NUM) {
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_SetExtCLUT: invalid Channel(%d). Amount:%d", Chan, SVC_OSD_CHAN_NUM);
        Rval = SVC_NG;
    } else if (pCLUT == NULL) {
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_SetExtCLUT: invalid CLUT(NULL).", 0U, 0U);
        Rval = SVC_NG;
    } else {
        AmbaMisra_TypeCast(&g_OsdClut_BaseAddr, &pCLUT);   //TBD
    }
    return Rval;
}

/**
 * Svc Osd set external osd configuration function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] pBufConfig external OSD configuration
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_SetExtBufInfo(UINT32 Chan, const SvcOSD_ExtBuffer_CFG_s *pBufConfig)
{
    UINT32 RetVal = SVC_OK;
    if (Chan >= SVC_OSD_CHAN_NUM) {
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOSD_SetExtBufInfo: invalid Channel(%d). Amount:%d", Chan, SVC_OSD_CHAN_NUM);
        RetVal = SVC_NG;
    } else if (pBufConfig == NULL) {
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOSD_SetExtBufInfo: invalid input ExtBufConfig(NULL).", 0U, 0U);
        RetVal = SVC_NG;
    } else if ((OSDExtBufferStatus[Chan] & SVC_OSD_EXTBUF_CONFIGED) != 0U) {
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] %s: Already configed.", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        AmbaSvcWrap_MisraMemcpy(&g_OSDExtBufferCFG[Chan], pBufConfig, sizeof(SvcOSD_ExtBuffer_CFG_s));

        //Check limitation
        {
            INT32 Rval = 0;
            UINT16 ActWidth = 0, ActHeight = 0;
            //UINT8 Interlace = 0;

            if(Chan == VOUT_IDX_A) {
                ActWidth = g_SvcOsdConfig[Chan].OsdSize.Width;
                ActHeight = g_SvcOsdConfig[Chan].OsdSize.Height;
            } else {
                //Interlace = SSPUTSystemInfo.FratePrevB.Interlace;
                ActWidth = 1920U;
                ActHeight = 1080U;
            }

            if((pBufConfig->WindowOffsetX + pBufConfig->WindowWidth) > ActWidth) {
                SvcLog_NG(SVC_LOG_OSD, "SvcOSD_SetExtBufInfo: Window OffsetX(%u)+Width(%u) over boundary!",
                        pBufConfig->WindowOffsetX, pBufConfig->WindowWidth);

                Rval = (INT32)ActWidth - (INT32)pBufConfig->WindowWidth;
                if (Rval < 0) {
                    Rval = 0;
                }
                SvcLog_NG(SVC_LOG_OSD, "SvcOSD_SetExtBufInfo: Adjust OffsetX as %u", (UINT32)Rval, 0U);
                g_OSDExtBufferCFG[Chan].WindowOffsetX = (UINT16)Rval;
            }

            if((pBufConfig->WindowOffsetY + pBufConfig->WindowHeight) > ActHeight) {
                SvcLog_NG(SVC_LOG_OSD, "SvcOSD_SetExtBufInfo: Window OffsetY(%u)+Height(%u) over boundary!",
                        pBufConfig->WindowOffsetY, pBufConfig->WindowHeight);

                Rval = (INT32)ActHeight - (INT32)pBufConfig->WindowHeight;
                if (Rval < 0) {
                    Rval = 0;
                }

                SvcLog_NG(SVC_LOG_OSD, "SvcOSD_SetExtBufInfo: Adjust OffsetY as %u", (UINT32)Rval, 0U);
                g_OSDExtBufferCFG[Chan].WindowOffsetY = (UINT16)Rval;
            }

            {
                char TempName[] = "SvcOSD_ExtMutex";
                (void)AmbaKAL_MutexCreate(&g_OSDExMutex[Chan], TempName);
            }
        }
        OSDExtBufferStatus[Chan] |= SVC_OSD_EXTBUF_CONFIGED;
    }
    return RetVal;
}

/**
 * Svc Osd update external buffer address function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] pOSDBufAddr external OSD buffer address
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_UpdateExtBuf(UINT32 Chan, const UINT8 *pOSDBufAddr)
{
    UINT32                          Rval = SVC_OK;
    AMBA_DSP_VOUT_OSD_BUF_CONFIG_s  OsdCfg = {0};

    if (Chan >= SVC_OSD_CHAN_NUM) {
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_UpdateExtBuf: invalid Channel(%d). Amount:%d", Chan, SVC_OSD_CHAN_NUM);
        Rval = SVC_NG;
    } else if ((OSDExtBufferStatus[Chan] & SVC_OSD_EXTBUF_CONFIGED) == 0U) {
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_UpdateExtBuf: Not yet configed.", 0U, 0U);
        Rval = SVC_NG;
    } else if (pOSDBufAddr == NULL) {
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_UpdateExtBuf: invalid input OSDBufAddr(NULL).", 0U, 0U);
        Rval = SVC_NG;
    } else {
        AMBA_KAL_MUTEX_t *Mutex = &g_OSDExMutex[Chan];

        if (AmbaKAL_MutexTake(Mutex, AMBA_KAL_WAIT_FOREVER) != SVC_OK) {
            Rval = SVC_NG;
        } else {

            OsdCfg.Pitch          = (UINT16)g_OSDExtBufferCFG[Chan].BufferPitch;
            OsdCfg.InputWidth     = (UINT16)g_OSDExtBufferCFG[Chan].BufferWidth;
            OsdCfg.InputHeight    = (UINT16)g_OSDExtBufferCFG[Chan].BufferHeight;
            OsdCfg.Window.OffsetX = g_OSDExtBufferCFG[Chan].WindowOffsetX;
            OsdCfg.Window.OffsetY = g_OSDExtBufferCFG[Chan].WindowOffsetY;
            OsdCfg.Window.Width   = g_OSDExtBufferCFG[Chan].WindowWidth;
            OsdCfg.Window.Height  = g_OSDExtBufferCFG[Chan].WindowHeight;

            OsdCfg.Window.Height = (OsdCfg.Window.Height >> g_SvcOsdConfig[Chan].VoutInterlace);
            OsdCfg.FieldRepeat   = g_SvcOsdConfig[Chan].VoutInterlace;

            AmbaMisra_TypeCast(&OsdCfg.BaseAddr, &pOSDBufAddr);
            OsdCfg.DataFormat = g_OSDExtBufferCFG[Chan].PixelFormat;
            if (OsdCfg.DataFormat == OSD_8BIT_CLUT_MODE) {
                (void)SvcPlat_CacheClean(g_OsdClut_BaseAddr, SVC_OSD_CLUT_ENTRY_SIZE * sizeof(UINT32));
                OsdCfg.CLUTAddr = g_OsdClut_BaseAddr;
            } else {
                OsdCfg.CLUTAddr = 0U;
            }
            (void)SvcPlat_CacheClean(OsdCfg.BaseAddr, g_OSDExtBufferCFG[Chan].BufferSize);

            Rval = AmbaDSP_VoutOsdConfigBuf((UINT8)Chan, &OsdCfg);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_OSD, "[ERROR] %s: AmbaDSP_VoutOsdBufSetup() fail!", 0U, 0U);
            } else {
                Svc_VoutOsdCtrl(Chan, 1);
                OSDExtBufferStatus[Chan] |= SVC_OSD_EXTBUF_ENABLED;
            }
            (void)AmbaKAL_MutexGive(Mutex);
        }
    }
    return Rval;
}

/**
 * Svc Osd release external OSD buffer function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_ReleaseExtOSD(UINT32 Chan)
{
    UINT32 Rval = SVC_OK;
    if (Chan >= SVC_OSD_CHAN_NUM) {
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_ReleaseExtOSD: invalid Channel(%d). Amount:%d", Chan, SVC_OSD_CHAN_NUM);
        Rval = SVC_NG;
    } else if ((OSDExtBufferStatus[Chan] & SVC_OSD_EXTBUF_CONFIGED) == 0U) {
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] %s: Not yet configed.", 0U, 0U);
        Rval = SVC_NG;
    } else {
        UINT8 *pU8 = &OSDExtBufferStatus[Chan];
        SvcOSD_ExtBuffer_CFG_s *pExtBuffer_CFG = &g_OSDExtBufferCFG[Chan];
        AMBA_KAL_MUTEX_t *Mutex = &g_OSDExMutex[Chan];

        if ((OSDExtBufferStatus[Chan] & SVC_OSD_EXTBUF_ENABLED) != 0U) {
            Svc_VoutOsdCtrl(Chan, 0U);
        }

        //reset
        if (AmbaKAL_MutexTake(Mutex, AMBA_KAL_WAIT_FOREVER) != SVC_OK) {
            Rval = SVC_NG;
        } else {
            AmbaSvcWrap_MisraMemset(pU8, 0, SVC_OSD_CHAN_NUM * sizeof(UINT8));
            AmbaSvcWrap_MisraMemset(pExtBuffer_CFG, 0, SVC_OSD_CHAN_NUM * sizeof(SvcOSD_ExtBuffer_CFG_s));
            (void)AmbaKAL_MutexGive(Mutex);
        }
        (void)AmbaKAL_MutexDelete(Mutex);
    }
    return Rval;
}

/**
 * Svc Osd set working buffer function
 * @param [in] pOsdWorkMem working buffer address
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_SetWorkBuf(UINT8 *pOsdWorkMem)
{
    UINT32 Rval = SVC_OK;

    AmbaMisra_TouchUnused(pOsdWorkMem);

    if (pOsdWorkMem == NULL) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_SetWorkBuf failed. Input working buffer not be NULL", 0U, 0U);
    } else if (g_BmpBuf != NULL) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_SetWorkBuf failed. working buffer has been set", 0U, 0U);
    } else {
        UINT64 AddrU64 = 0U;
        UINT8  *pWorkMem;

        AmbaMisra_TypeCast(&AddrU64, &pOsdWorkMem);
        /* Get bmp working buffer address base */
        AmbaMisra_TypeCast(&pWorkMem, &AddrU64);
        /* Assign the working buffer */
        g_BmpBuf        = pWorkMem;

#if defined(CONFIG_BUILD_COMMON_SERVICE_DRAW)
        /* Get font memory address base */
        AddrU64 += sizeof(UINT8) * SVC_OSD_BUF_SIZE;
        AmbaMisra_TypeCast(&pWorkMem, &AddrU64);
        /* Assign the working buffer */
        g_SvcOsdFontBuf = pWorkMem;
#endif
    }
    return Rval;
}

/**
 * Svc Osd get OSD buffer size function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [out] pWidth osd buffer width
 * @param [out] pHeight osd buffer height
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_GetOsdBufSize(UINT32 Chan, UINT32 *pWidth, UINT32 *pHeight)
{
    UINT32 Rval = SVC_OK;

    if ((pWidth == NULL) || (pHeight == NULL)) {
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_GetOsdBufSize failed. Input should not be NULL", 0U, 0U);
        Rval = SVC_NG;
    } else {
        if (Chan < SVC_OSD_CHAN_NUM) {
            *pWidth  = g_SvcOsdConfig[Chan].OsdSize.Width;
            *pHeight = g_SvcOsdConfig[Chan].OsdSize.Height;
        } else {
            SvcLog_NG(SVC_LOG_OSD, "SvcOsd_GetOsdBufSize failed. Input Channel %d should be < %d", Chan, SVC_OSD_CHAN_NUM);
            Rval = SVC_NG;
        }
    }

    return Rval;
}

/**
 * Svc Osd get OSD color look up table information
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [out] pCLUT pointer to color look up table
 * @param [out] pNumColor max number of entries of color lookup table
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_GetOsdCLUT(UINT32 Chan, UINT32 **pCLUT, UINT32 *pNumColor)
{
    UINT32 Rval = SVC_OK;

    if (Chan >= SVC_OSD_CHAN_NUM) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_GetClut: invalid Channel(%d). Amount:%d", Chan, SVC_OSD_CHAN_NUM);
    }

    if (pCLUT == NULL) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_GetClut: invalid pCLUT", 0U, 0U);
    }

    if ((Rval == SVC_OK) && (pCLUT != NULL) && (Chan < SVC_OSD_CHAN_NUM)) {

        if ((OSDExtBufferStatus[Chan] & SVC_OSD_EXTBUF_ENABLED) > 0U) {
            UINT32 *pExtOsdClut;

            AmbaMisra_TypeCast(&(pExtOsdClut), &(g_OsdClut_BaseAddr)); AmbaMisra_TouchUnused(pExtOsdClut);
            if (pExtOsdClut == NULL) {
                Rval = SVC_NG;
                SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_GetClut: external OSD colo lookup table should not null!", 0U, 0U);
            } else {
                *pCLUT = &pExtOsdClut[0];
                *pNumColor = SVC_OSD_CLUT_ENTRY_SIZE;
            }

        } else {
            UINT32 *pOsdClut;

            AmbaMisra_TypeCast(&(pOsdClut), &(g_SvcOsdCLUTAddr));
            AmbaMisra_TouchUnused(pOsdClut);
            *pCLUT = pOsdClut;
            *pNumColor = SVC_OSD_CLUT_ENTRY_SIZE;
        }
    }

    return Rval;
}

/**
 * Svc Osd get OSD color from lookup table
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] Idx desired index in look up table
 * @param [out] pColor color of desired index in look up table
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_GetOsdColor(UINT32 Chan, UINT32 Idx, UINT32 *pColor)
{
    UINT32 Rval = SVC_OK;

    if (Chan >= SVC_OSD_CHAN_NUM) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_GetClut: invalid Channel(%d). Amount:%d", Chan, SVC_OSD_CHAN_NUM);
    }

    if (Idx >= SVC_OSD_CLUT_ENTRY_SIZE) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_GetClut: invalid Index(%d). Amount:%d", Idx, SVC_OSD_CLUT_ENTRY_SIZE);
    }

    if (pColor == NULL) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_GetClut: invalid output Color value", 0U, 0U);
    }

    if ((Rval == SVC_OK) && (pColor != NULL) && (Idx < SVC_OSD_CLUT_ENTRY_SIZE) && (Chan < SVC_OSD_CHAN_NUM)) {

        if ((OSDExtBufferStatus[Chan] & SVC_OSD_EXTBUF_ENABLED) > 0U) {
            UINT32 *pExtOsdClut;

            AmbaMisra_TypeCast(&(pExtOsdClut), &(g_OsdClut_BaseAddr)); AmbaMisra_TouchUnused(pExtOsdClut);
            if (pExtOsdClut == NULL) {
                Rval = SVC_NG;
                SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_GetClut: external OSD colo lookup table should not null!", 0U, 0U);
            } else {
                *pColor = pExtOsdClut[Idx];
            }

        } else {
            const UINT32 *pCLUT;
            AmbaMisra_TypeCast(&pCLUT, &g_SvcOsdCLUTAddr[Chan]);
            if (pCLUT != NULL) {
                *pColor = pCLUT[Idx];
            }
        }
    }

    return Rval;
}

/**
 * Convert color index to color setting for specific OSD format
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] Idx index for look up table
 * @param [out] pColor color setting. 8 bits OSD - color index, 32 bits OSD - color
 * return none
 */
void SvcOsd_ColorIdxToColorSetting(UINT32 Chan, UINT32 Idx, UINT32 *pColor)
{
    if (NULL != pColor) {
        if (Idx < SVC_OSD_CLUT_ENTRY_SIZE) {
            *pColor = Idx;
        } else {
            *pColor = 0x0;
        }
    }

    AmbaMisra_TouchUnused(&Chan);
}

/**
 * Svc Osd transfer dimension from Vout(Mixer) to OSD function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] pVideoWin Vout(Mixer) dimension
 * @param [out] pOsdWin Osd dimension
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_TransferLivWinToOsdWin(UINT32 Chan, SVC_OSD_WINDOW_s *pVideoWin, SVC_OSD_WINDOW_s *pOsdWin)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_DISP_STRM_s *pDispStrm = NULL;

    AmbaMisra_TouchUnused(pVideoWin);

    if (Chan >= SVC_OSD_CHAN_NUM) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_TransferFovWinToOsdWin: invalid Channel(%d). Amount:%d", Chan, SVC_OSD_CHAN_NUM);
    }

    if ((pVideoWin == NULL) || (pOsdWin == NULL)) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_TransferFovWinToOsdWin: Input Window should not be NULL", 0U, 0U);
    }

    for (i = 0U; i < pResCfg->DispNum; i++) {
        if (pResCfg->DispStrm[i].VoutID == Chan) {
            pDispStrm = &(pResCfg->DispStrm[i]);
        }
    }

    if (pDispStrm == NULL) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_TransferFovWinToOsdWin: VOUT %d is not configured", Chan, 0U);
    }

    if (SVC_OK == Rval) {
        DOUBLE RatioHor, RatioVer;
        UINT32 OsdBufWidth = 0U, OsdBufHeight = 0U;
        UINT32 MixerWidth, MixerHeight;
        UINT32 VideoWinWidth, VideoWinHeight, VideoWinOffsetX, VideoWinOffsetY;
        UINT32 WinOffsetX, WinOffsetY, WinWidth, WinHeight;
        DOUBLE OsdOffsetX, OsdOffsetY, OsdWidth, OsdHeight;

        MixerWidth  = (UINT32) pDispStrm->StrmCfg.MaxWin.Width;
        MixerHeight = (UINT32) pDispStrm->StrmCfg.MaxWin.Height;
        VideoWinWidth  = (UINT32) pDispStrm->StrmCfg.Win.Width;
        VideoWinHeight = (UINT32) pDispStrm->StrmCfg.Win.Height;
        VideoWinOffsetX = (MixerWidth - VideoWinWidth) >> 1U;
        VideoWinOffsetY = (MixerHeight - VideoWinHeight) >> 1U;

        (void) SvcOsd_GetOsdBufSize(Chan, &OsdBufWidth, &OsdBufHeight);

        RatioHor = ((DOUBLE) MixerWidth) / ((DOUBLE) OsdBufWidth);
        RatioVer = ((DOUBLE) MixerHeight) / ((DOUBLE) OsdBufHeight);

        /* Mixer coordinate */
        WinOffsetX = VideoWinOffsetX + (UINT32) pVideoWin->OffsetX;
        WinOffsetY = VideoWinOffsetY + (UINT32) pVideoWin->OffsetY;
        WinWidth   = (UINT32) pVideoWin->Width;
        WinHeight  = (UINT32) pVideoWin->Height;
        /* Mixer coordinate -> Osd Buffer coordinate */
        OsdOffsetX = (DOUBLE) WinOffsetX / RatioHor;
        OsdOffsetY = (DOUBLE) WinOffsetY / RatioVer;
        OsdWidth   = (DOUBLE) WinWidth / RatioHor;
        OsdHeight  = (DOUBLE) WinHeight / RatioVer;

        pOsdWin->OffsetX = (UINT16) OsdOffsetX;
        pOsdWin->OffsetY = (UINT16) OsdOffsetY;
        pOsdWin->Width   = (UINT16) OsdWidth;
        pOsdWin->Height  = (UINT16) OsdHeight;
    }

    return Rval;
}

/**
 * Svc Osd query OSD buffer size function.
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [out] pMemSize memory size which osd will use. Based on OSD draw buffer number
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_MemSizeQuery(UINT32 Chan, UINT32 *pMemSize)
{
    UINT32 RetVal = SVC_OK;
    UINT32 PixelSize = 1U;

    if (Chan < SVC_OSD_CHAN_NUM) {
        if (pMemSize == NULL) {
            SvcLog_NG(SVC_LOG_OSD, "SvcOsd_MemSizeQuery failed. pMemSize cannot be NULL", 0U, 0U);
            RetVal = SVC_NG;
        }
        *pMemSize = 0U;
        PixelSize = PixelSize << SVC_OSD_PIXEL_SIZE_SFT;
#if !defined(CONFIG_ICAM_32BITS_OSD_USED)
        /* color look up table size (256 entries) */
        *pMemSize += sizeof(UINT32) * 256U;
#endif
        if (Chan == 0U) {
            *pMemSize += (SVC_OSD_BUF_MAX_WIDTH_VOUT_A * SVC_OSD_BUF_MAX_HEIGHT_VOUT_A) * SVC_OSD_BUF_NUM * PixelSize;
        } else {
            *pMemSize += (SVC_OSD_BUF_MAX_WIDTH_VOUT_B * SVC_OSD_BUF_MAX_HEIGHT_VOUT_B) * SVC_OSD_BUF_NUM * PixelSize;
        }
    } else {
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_MemSizeQuery failed. Chan %d cannot >= %d", Chan, SVC_OSD_CHAN_NUM);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 * Svc Osd query OSD working buffer size function.
 * @param [out] pMemSize memory size which osd will use.
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_WorkMemSizeQuery(UINT32 *pMemSize)
{
    UINT32 RetVal = SVC_OK;

    if (pMemSize == NULL) {
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_WorkMemSizeQuery failed. pMemSize cannot be NULL", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        *pMemSize = SVC_OSD_BUF_SIZE;
#if defined(CONFIG_BUILD_COMMON_SERVICE_DRAW)
        *pMemSize += SVC_MAX_FONTS_BIN_FILE_SIZE;
#endif
    }

    return RetVal;
}

static void SvcOsd_BufInit(UINT32 Chan, const SVC_OSD_CONFIG_s *pConfig)
{
    UINT32 *pCLUT;
    ULONG   PhyAddr;
    const UINT32  DefaultCLUT[SVC_OSD_CLUT_ENTRY_SIZE] = {
  //0xAARRGGBB ,    Idx ( RED, GRN, BLU )
    0x80000000U, // 000 ( 000, 000, 000 )
    0x80800000U, // 001 ( 128, 000, 000 )
    0x80008000U, // 002 ( 000, 128, 000 )
    0x80808000U, // 003 ( 128, 128, 000 )
    0x80000080U, // 004 ( 000, 000, 128 )
    0x80800080U, // 005 ( 128, 000, 128 )
    0x80008080U, // 006 ( 000, 128, 128 )
    0x80c0c0c0U, // 007 ( 192, 192, 192 )
    0x80c0dcc0U, // 008 ( 192, 220, 192 )
    0x80a6caf0U, // 009 ( 166, 202, 240 )
    0x80402000U, // 010 ( 064, 032, 000 )
    0x80602000U, // 011 ( 096, 032, 000 )
    0x80802000U, // 012 ( 128, 032, 000 )
    0x80a02000U, // 013 ( 160, 032, 000 )
    0x80c02000U, // 014 ( 192, 032, 000 )
    0x80e02000U, // 015 ( 224, 032, 000 )
    0x80004000U, // 016 ( 000, 064, 000 )
    0x80204000U, // 017 ( 032, 064, 000 )
    0x80404000U, // 018 ( 064, 064, 000 )
    0x80604000U, // 019 ( 096, 064, 000 )
    0x80804000U, // 020 ( 128, 064, 000 )
    0x80a04000U, // 021 ( 160, 064, 000 )
    0x80c04000U, // 022 ( 192, 064, 000 )
    0x80e04000U, // 023 ( 224, 064, 000 )
    0x80006000U, // 024 ( 000, 096, 000 )
    0x80206000U, // 025 ( 032, 096, 000 )
    0x80406000U, // 026 ( 064, 096, 000 )
    0x80606000U, // 027 ( 096, 096, 000 )

#ifdef SVC_OSD_STD_256_COLOR
    0x80806000U, // 028 ( 128, 096, 000 )
    0x80a06000U, // 029 ( 160, 096, 000 )
    0x80c06000U, // 030 ( 192, 096, 000 )
    0x80e06000U, // 031 ( 224, 096, 000 )
    0x80008000U, // 032 ( 000, 128, 000 )
    0x80208000U, // 033 ( 032, 128, 000 )
    0x80408000U, // 034 ( 064, 128, 000 )
    0x80608000U, // 035 ( 096, 128, 000 )
    0x80808000U, // 036 ( 128, 128, 000 )
    0x80a08000U, // 037 ( 160, 128, 000 )
    0x80c08000U, // 038 ( 192, 128, 000 )
    0x80e08000U, // 039 ( 224, 128, 000 )
    0x8000a000U, // 040 ( 000, 160, 000 )
    0x8020a000U, // 041 ( 032, 160, 000 )
    0x8040a000U, // 042 ( 064, 160, 000 )
    0x8060a000U, // 043 ( 096, 160, 000 )
#else
    // For OD
    0xFFBFFF00U, // 028 ( 191, 255, 000 )
    0xFF00CC00U, // 029 ( 000, 204, 000 )
    0xFF0040FFU, // 030 ( 000,  64, 255 )
    0xFF00BFFFU, // 031 ( 000, 191, 255 )
    0xFFFF80FFU, // 032 ( 255, 128, 255 )
    0xFFFFBF00U, // 033 ( 255, 191, 000 )
    0xFFFF6600U, // 034 ( 255, 102, 000 )
    0xFF00CC00U, // 035 ( 000, 204, 000 )
    0xFFFFFF00U, // 036 ( 255, 255, 000 )
    0xFFFF0000U, // 037 ( 255, 255, 000 )
    0xFFFF00FFU, // 038 ( 255, 000, 255 )
    0xFF8000FFU, // 039 ( 128, 000, 255 )

    // For Segmentation
    0x00000000U, // 040 ( 000, 000, 000 )
    0x000080FFU, // 041 ( 000, 128, 255 )
    0x8000FF00U, // 042 ( 000, 255, 000 )
    0x80FFFF00U, // 043 ( 255, 255, 000 )
#endif
    0x8080a000U, // 044 ( 128, 160, 000 )
    0x80a0a000U, // 045 ( 160, 160, 000 )
    0x80c0a000U, // 046 ( 192, 160, 000 )
    0x80e0a000U, // 047 ( 224, 160, 000 )
    0x8000c000U, // 048 ( 000, 192, 000 )
    0x8020c000U, // 049 ( 032, 192, 000 )
    0x8040c000U, // 050 ( 064, 192, 000 )
    0x8060c000U, // 051 ( 096, 192, 000 )
    0x8080c000U, // 052 ( 128, 192, 000 )
    0x80a0c000U, // 053 ( 160, 192, 000 )
    0x80c0c000U, // 054 ( 192, 192, 000 )
    0x80e0c000U, // 055 ( 224, 192, 000 )
    0x8000e000U, // 056 ( 000, 224, 000 )
    0x8020e000U, // 057 ( 032, 224, 000 )
    0x8040e000U, // 058 ( 064, 224, 000 )
    0x8060e000U, // 059 ( 096, 224, 000 )
    0x8080e000U, // 060 ( 128, 224, 000 )
    0x80a0e000U, // 061 ( 160, 224, 000 )
    0x80c0e000U, // 062 ( 192, 224, 000 )
    0x80e0e000U, // 063 ( 224, 224, 000 )
    0x80000040U, // 064 ( 000, 000, 064 )
    0x80200040U, // 065 ( 032, 000, 064 )
    0x80400040U, // 066 ( 064, 000, 064 )
    0x80600040U, // 067 ( 096, 000, 064 )
    0x80800040U, // 068 ( 128, 000, 064 )
    0x80a00040U, // 069 ( 160, 000, 064 )
    0x80c00040U, // 070 ( 192, 000, 064 )
    0x80e00040U, // 071 ( 224, 000, 064 )
    0x80002040U, // 072 ( 000, 032, 064 )
    0x80202040U, // 073 ( 032, 032, 064 )
    0x80402040U, // 074 ( 064, 032, 064 )
    0x80602040U, // 075 ( 096, 032, 064 )
    0x80802040U, // 076 ( 128, 032, 064 )
    0x80a02040U, // 077 ( 160, 032, 064 )
    0x80c02040U, // 078 ( 192, 032, 064 )
    0x80e02040U, // 079 ( 224, 032, 064 )
    0x80004040U, // 080 ( 000, 064, 064 )
    0x80204040U, // 081 ( 032, 064, 064 )
    0x80404040U, // 082 ( 064, 064, 064 )
    0x80604040U, // 083 ( 096, 064, 064 )
    0x80804040U, // 084 ( 128, 064, 064 )
    0x80a04040U, // 085 ( 160, 064, 064 )
    0x80c04040U, // 086 ( 192, 064, 064 )
    0x80e04040U, // 087 ( 224, 064, 064 )
    0x80006040U, // 088 ( 000, 096, 064 )
    0x80206040U, // 089 ( 032, 096, 064 )
    0x80406040U, // 090 ( 064, 096, 064 )
    0x80606040U, // 091 ( 096, 096, 064 )
    0x80806040U, // 092 ( 128, 096, 064 )
    0x80a06040U, // 093 ( 160, 096, 064 )
    0x80c06040U, // 094 ( 192, 096, 064 )
    0x80e06040U, // 095 ( 224, 096, 064 )
    0x80008040U, // 096 ( 000, 128, 064 )
    0x80208040U, // 097 ( 032, 128, 064 )
    0x80408040U, // 098 ( 064, 128, 064 )
    0x80608040U, // 099 ( 096, 128, 064 )
    0x80808040U, // 100 ( 128, 128, 064 )
    0x80a08040U, // 101 ( 160, 128, 064 )
    0x80c08040U, // 102 ( 192, 128, 064 )
    0x80e08040U, // 103 ( 224, 128, 064 )
    0x8000a040U, // 104 ( 000, 160, 064 )
    0x8020a040U, // 105 ( 032, 160, 064 )
    0x8040a040U, // 106 ( 064, 160, 064 )
    0x8060a040U, // 107 ( 096, 160, 064 )
    0x8080a040U, // 108 ( 128, 160, 064 )
    0x80a0a040U, // 109 ( 160, 160, 064 )
    0x80c0a040U, // 110 ( 192, 160, 064 )
    0x80e0a040U, // 111 ( 224, 160, 064 )
    0x8000c040U, // 112 ( 000, 192, 064 )
    0x8020c040U, // 113 ( 032, 192, 064 )
    0x8040c040U, // 114 ( 064, 192, 064 )
    0x8060c040U, // 115 ( 096, 192, 064 )
    0x8080c040U, // 116 ( 128, 192, 064 )
    0x80a0c040U, // 117 ( 160, 192, 064 )
    0x80c0c040U, // 118 ( 192, 192, 064 )
    0x80e0c040U, // 119 ( 224, 192, 064 )
    0x8000e040U, // 120 ( 000, 224, 064 )
    0x8020e040U, // 121 ( 032, 224, 064 )
    0x8040e040U, // 122 ( 064, 224, 064 )
    0x8060e040U, // 123 ( 096, 224, 064 )
    0x8080e040U, // 124 ( 128, 224, 064 )
    0x80a0e040U, // 125 ( 160, 224, 064 )
    0x80c0e040U, // 126 ( 192, 224, 064 )
    0x80e0e040U, // 127 ( 224, 224, 064 )
    0x80000080U, // 128 ( 000, 000, 128 )
    0x80200080U, // 129 ( 032, 000, 128 )
    0x80400080U, // 130 ( 064, 000, 128 )
    0x80600080U, // 131 ( 096, 000, 128 )
    0x80800080U, // 132 ( 128, 000, 128 )
    0x80a00080U, // 133 ( 160, 000, 128 )
    0x80c00080U, // 134 ( 192, 000, 128 )
    0x80e00080U, // 135 ( 224, 000, 128 )
    0x80002080U, // 136 ( 000, 032, 128 )
    0x80202080U, // 137 ( 032, 032, 128 )
    0x80402080U, // 138 ( 064, 032, 128 )
    0x80602080U, // 139 ( 096, 032, 128 )
    0x80802080U, // 140 ( 128, 032, 128 )
    0x80a02080U, // 141 ( 160, 032, 128 )
    0x80c02080U, // 142 ( 192, 032, 128 )
    0x80e02080U, // 143 ( 224, 032, 128 )
    0x80004080U, // 144 ( 000, 064, 128 )
    0x80204080U, // 145 ( 032, 064, 128 )
    0x80404080U, // 146 ( 064, 064, 128 )
    0x80604080U, // 147 ( 096, 064, 128 )
    0x80804080U, // 148 ( 128, 064, 128 )
    0x80a04080U, // 149 ( 160, 064, 128 )
    0x80c04080U, // 150 ( 192, 064, 128 )
    0x80e04080U, // 151 ( 224, 064, 128 )
    0x80006080U, // 152 ( 000, 096, 128 )
    0x80206080U, // 153 ( 032, 096, 128 )
    0x80406080U, // 154 ( 064, 096, 128 )
    0x80606080U, // 155 ( 096, 096, 128 )
    0x80806080U, // 156 ( 128, 096, 128 )
    0x80a06080U, // 157 ( 160, 096, 128 )


#if defined(CONFIG_ICAM_PROJECT_SURROUND)
    0xFFFF0000U,// 158 (255,    0,  0  )
    0xFFFF8000U,// 159 (255,  128,  0  )
    0XFFFFFFFFU,// 160 (255,  255,  255)
    0XFFFCFCFCU,// 161 (252,  252,  252)
    0XFFFAFAFAU,// 162 (250,  250,  250)
    0XFFF8F8F8U,// 163 (248,  248,  248)
    0XFFF6F6F6U,// 164 (246,  246,  246)
    0XFFF4F4F4U,// 165 (244,  244,  244)
    0XFFF2F2F2U,// 166 (242,  242,  242)
    0XFFEFEFEFU,// 167 (239,  239,  239)
    0XFFEDEDEDU,// 168 (237,  237,  237)
    0XFFEAEAEAU,// 169 (234,  234,  234)
    0XFFE8E8E8U,// 170 (232,  232,  232)
    0XFFE5E5E5U,// 171 (229,  229,  229)
    0XFFE2E2E2U,// 172 (226,  226,  226)
    0XFFDFDFDFU,// 173 (223,  223,  223)
    0XFFDCDCDCU,// 174 (220,  220,  220)
    0XFFD9D9D9U,// 175 (217,  217,  217)
    0XFFD6D6D6U,// 176 (214,  214,  214)
    0XFFD3D3D3U,// 177 (211,  211,  211)
    0XFFD0D0D0U,// 178 (208,  208,  208)
    0XFFCCCCCCU,// 179 (204,  204,  204)
    0XFFC9C9C9U,// 180 (201,  201,  201)
    0XFFC6C6C6U,// 181 (198,  198,  198)
    0XFFC2C2C2U,// 182 (194,  194,  194)
    0XFFBEBEBEU,// 183 (190,  190,  190)
    0XFFBBBBBBU,// 184 (187,  187,  187)
    0XFFB7B7B7U,// 185 (183,  183,  183)
    0XFFB4B4B4U,// 186 (180,  180,  180)
    0XFFB0B0B0U,// 187 (176,  176,  176)
    0XFFACACACU,// 188 (172,  172,  172)
    0XFFA8A8A8U,// 189 (168,  168,  168)
    0XFFA5A5A5U,// 190 (165,  165,  165)
    0XFFA1A1A1U,// 191 (161,  161,  161)
    0XFF9D9D9DU,// 192 (157,  157,  157)
    0XFF999999U,// 193 (153,  153,  153)
    0XFF959595U,// 194 (149,  149,  149)
    0XFF919191U,// 195 (145,  145,  145)
    0XFF8D8D8DU,// 196 (141,  141,  141)
    0XFF898989U,// 197 (137,  137,  137)
    0XFF858585U,// 198 (133,  133,  133)
    0XFF818181U,// 199 (129,  129,  129)
    0XFF7E7E7EU,// 200 (126,  126,  126)
    0XFF7A7A7AU,// 201 (122,  122,  122)
    0XFF767676U,// 202 (118,  118,  118)
    0XFF727272U,// 203 (114,  114,  114)
    0XFF6E6E6EU,// 204 (110,  110,  110)
    0XFF6A6A6AU,// 205 (106,  106,  106)
    0XFF666666U,// 206 (102,  102,  102)
    0XFF646464U,// 207 (100,  100,  100)
    0XFF626262U,// 208 ( 98,   98,   98)
    0XFF5E5E5EU,// 209 ( 94,   94,   94)
    0XFF5A5A5AU,// 210 ( 90,   90,   90)
    0XFF575757U,// 211 ( 87,   87,   87)
    0XFF535353U,// 212 ( 83,   83,   83)
    0XFF4F4F4FU,// 213 ( 79,   79,   79)
    0XFF4B4B4BU,// 214 ( 75,   75,   75)
    0XFF484848U,// 215 ( 72,   72,   72)
    0XFF444444U,// 216 ( 68,   68,   68)
    0XFF414141U,// 217 ( 65,   65,   65)
    0XFF3D3D3DU,// 218 ( 61,   61,   61)
    0XFF393939U,// 219 ( 57,   57,   57)
    0XFF363636U,// 220 ( 54,   54,   54)
    0XFF333333U,// 221 ( 51,   51,   51)
    0XFF2F2F2FU,// 222 ( 47,   47,   47)
    0XFF2C2C2CU,// 223 ( 44,   44,   44)
    0XFF232323U,// 224 ( 35,   35,   35)
    0XFF1D1D1DU,// 225 ( 29,   29,   29)
    0XFF171717U,// 226 ( 23,   23,   23)
    0XFF121212U,// 227 ( 18,   18,   18)
    0XFF0D0D0DU,// 228 ( 13,   13,   13)
    0XFF090909U,// 229 (  9,    9,    9)
    0XFF050505U,// 230 (  5,    5,    5)
#else
    0x80c06080U, // 158 ( 192, 096, 128 )
    0x80e06080U, // 159 ( 224, 096, 128 )
    0x80008080U, // 160 ( 000, 128, 128 )
    0x80208080U, // 161 ( 032, 128, 128 )
    0x80408080U, // 162 ( 064, 128, 128 )
    0x80608080U, // 163 ( 096, 128, 128 )
    0x80808080U, // 164 ( 128, 128, 128 )
    0x80a08080U, // 165 ( 160, 128, 128 )
    0x80c08080U, // 166 ( 192, 128, 128 )
    0x80e08080U, // 167 ( 224, 128, 128 )
    0x8000a080U, // 168 ( 000, 160, 128 )
    0x8020a080U, // 169 ( 032, 160, 128 )
    0x8040a080U, // 170 ( 064, 160, 128 )
    0x8060a080U, // 171 ( 096, 160, 128 )
    0x8080a080U, // 172 ( 128, 160, 128 )
    0x80a0a080U, // 173 ( 160, 160, 128 )
    0x80c0a080U, // 174 ( 192, 160, 128 )
    0x80e0a080U, // 175 ( 224, 160, 128 )
    0x8000c080U, // 176 ( 000, 192, 128 )
    0x8020c080U, // 177 ( 032, 192, 128 )
    0x8040c080U, // 178 ( 064, 192, 128 )
    0x8060c080U, // 179 ( 096, 192, 128 )
    0x8080c080U, // 180 ( 128, 192, 128 )
    0x80a0c080U, // 181 ( 160, 192, 128 )
    0x80c0c080U, // 182 ( 192, 192, 128 )
    0x80e0c080U, // 183 ( 224, 192, 128 )
    0x8000e080U, // 184 ( 000, 224, 128 )
    0x8020e080U, // 185 ( 032, 224, 128 )
    0x8040e080U, // 186 ( 064, 224, 128 )
    0x8060e080U, // 187 ( 096, 224, 128 )
    0x8080e080U, // 188 ( 128, 224, 128 )
    0x80a0e080U, // 189 ( 160, 224, 128 )
    0x80c0e080U, // 190 ( 192, 224, 128 )
    0x80e0e080U, // 191 ( 224, 224, 128 )
    0x800000c0U, // 192 ( 000, 000, 192 )
    0x802000c0U, // 193 ( 032, 000, 192 )
    0x804000c0U, // 194 ( 064, 000, 192 )
    0x806000c0U, // 195 ( 096, 000, 192 )
    0x808000c0U, // 196 ( 128, 000, 192 )
    0x80a000c0U, // 197 ( 160, 000, 192 )
    0x80c000c0U, // 198 ( 192, 000, 192 )
    0x80e000c0U, // 199 ( 224, 000, 192 )
    0x800020c0U, // 200 ( 000, 032, 192 )
    0x802020c0U, // 201 ( 032, 032, 192 )
    0x804020c0U, // 202 ( 064, 032, 192 )
    0x806020c0U, // 203 ( 096, 032, 192 )
    0x808020c0U, // 204 ( 128, 032, 192 )
    0x80a020c0U, // 205 ( 160, 032, 192 )
    0x80c020c0U, // 206 ( 192, 032, 192 )
    0x80e020c0U, // 207 ( 224, 032, 192 )
    0x800040c0U, // 208 ( 000, 064, 192 )
    0x802040c0U, // 209 ( 032, 064, 192 )
    0x804040c0U, // 210 ( 064, 064, 192 )
    0x806040c0U, // 211 ( 096, 064, 192 )
    0x808040c0U, // 212 ( 128, 064, 192 )
    0x80a040c0U, // 213 ( 160, 064, 192 )
    0x80c040c0U, // 214 ( 192, 064, 192 )
    0x80e040c0U, // 215 ( 224, 064, 192 )
    0x800060c0U, // 216 ( 000, 096, 192 )
    0x802060c0U, // 217 ( 032, 096, 192 )
    0x804060c0U, // 218 ( 064, 096, 192 )
    0x806060c0U, // 219 ( 096, 096, 192 )
    0x808060c0U, // 220 ( 128, 096, 192 )
    0x80a060c0U, // 221 ( 160, 096, 192 )
    0x80c060c0U, // 222 ( 192, 096, 192 )
    0x80e060c0U, // 223 ( 224, 096, 192 )
    0x800080c0U, // 224 ( 000, 128, 192 )
    0x802080c0U, // 225 ( 032, 128, 192 )
    0x804080c0U, // 226 ( 064, 128, 192 )
    0x806080c0U, // 227 ( 096, 128, 192 )
    0x808080c0U, // 228 ( 128, 128, 192 )
    0x80a080c0U, // 229 ( 160, 128, 192 )
    0x80c080c0U, // 230 ( 192, 128, 192 )
#endif
    0x80e080c0U, // 231 ( 224, 128, 192 )
    0x8000a0c0U, // 232 ( 000, 160, 192 )
    0x8020a0c0U, // 233 ( 032, 160, 192 )
    0x8040a0c0U, // 234 ( 064, 160, 192 )
    0x8060a0c0U, // 235 ( 096, 160, 192 )
    0x8080a0c0U, // 236 ( 128, 160, 192 )
    0x80a0a0c0U, // 237 ( 160, 160, 192 )
    0x80c0a0c0U, // 238 ( 192, 160, 192 )
    0x80e0a0c0U, // 239 ( 224, 160, 192 )
    0x8000c0c0U, // 240 ( 000, 192, 192 )
    0x8020c0c0U, // 241 ( 032, 192, 192 )
    0x8040c0c0U, // 242 ( 064, 192, 192 )
    0x8060c0c0U, // 243 ( 096, 192, 192 )
    0x8080c0c0U, // 244 ( 128, 192, 192 )
    0x80a0c0c0U, // 245 ( 160, 192, 192 )
    0x80fffbf0U, // 246 ( 255, 251, 240 )
    0x80a0a0a4U, // 247 ( 160, 160, 164 )
#ifdef SVC_OSD_STD_256_COLOR
    0x80808080U, // 248 ( 128, 128, 128 )
#else
    // For black
    0xFF000000U, // 248 ( 000, 000, 000 )
#endif
    0x80ff0000U, // 249 ( 255, 000, 000 )
    0x8000ff00U, // 250 ( 000, 255, 000 )
    0x80ffff00U, // 251 ( 255, 255, 000 )
    0x800000ffU, // 252 ( 000, 000, 255 )
    0x80ff00ffU, // 253 ( 255, 000, 255 )
    0x8000ffffU, // 254 ( 000, 255, 255 )
#ifdef SVC_OSD_STD_256_COLOR
    0x80ffffffU, // 255 ( 255, 255, 255 )
#else
    0xffffffffU, // 255 ( 255, 255, 255 )
#endif
};

    /* Color look up table is from the allocated buffer */
    if (g_SvcOsdCtrl[Chan].DrawBufInfo.PixelFormat == AMBA_DRAW_BUFFER_8BIT_CLUT_MODE) {

        AmbaMisra_TypeCast(&g_SvcOsdCLUTAddr[Chan], &g_SvcOsdConfig[Chan].pOsdMem);
        AmbaMisra_TypeCast(&pCLUT, &g_SvcOsdConfig[Chan].pOsdMem);

        if (NULL != pConfig->pClut) {
            if (0U != AmbaWrap_memcpy(pCLUT, pConfig->pClut, pConfig->ClutSize)) {
                SvcLog_NG(SVC_LOG_OSD, "Fail to memset CLUT", 0U, 0U);
            }
        } else {
            if (0U != AmbaWrap_memcpy(pCLUT, DefaultCLUT, sizeof(UINT32) * SVC_OSD_CLUT_ENTRY_SIZE)) {
                SvcLog_NG(SVC_LOG_OSD, "Fail to memset CLUT", 0U, 0U);
            }
        }

        if (SVC_OK == AmbaMMU_VirtToPhys(g_SvcOsdCLUTAddr[Chan], &PhyAddr)) {
            SvcLog_DBG(SVC_LOG_OSD, "g_SvcOsdCLUTAddr (Virt) 0x%x -> (Phy) 0x%x", g_SvcOsdCLUTAddr[Chan], PhyAddr);
        } else {
            SvcLog_NG(SVC_LOG_OSD, "g_SvcOsdCLUTAddr AmbaMMU_VirtToPhys() failed", 0U, 0U);
        }

        SvcOsd_SetClut(0U, 0x00FF8080U);    // Configure Color Lookup Table Index 0 Alpha as 0%

        if (SVC_OK != SvcPlat_CacheClean(g_SvcOsdCLUTAddr[Chan], SVC_OSD_CLUT_ENTRY_SIZE * sizeof(UINT32))) {
            SvcLog_NG(SVC_LOG_OSD, "Fail to SvcPlat_CacheClean", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_BufInit(%u): Not supported OSD mode.", Chan, 0U);
    }

    /* OSD Draw buffer */
    {
        UINT32 MemSize = 0U;
        if (Chan == VOUT_IDX_A) {
            MemSize = (SVC_OSD_BUF_MAX_WIDTH_VOUT_A * SVC_OSD_BUF_MAX_HEIGHT_VOUT_A) * sizeof(UINT8);
        } else {
            MemSize = (SVC_OSD_BUF_MAX_WIDTH_VOUT_B * SVC_OSD_BUF_MAX_HEIGHT_VOUT_B) * sizeof(UINT8);
        }
        for (UINT32 i = 0U; i < SVC_OSD_BUF_NUM; i++) {
            if (SVC_OK != AmbaWrap_memset(g_SvcOsdBuffer[Chan].pAddr[i], 0, MemSize)) {
                SvcLog_NG(SVC_LOG_OSD, "SvcOsd_BufInit(%u): Falied to memset the buffer of idx %d", Chan, i);
            }
        }
    }

}

void SvcOsd_SetClut(UINT32 Idx, UINT32 Color)
{
    UINT32 Chan, i, *pCLUT;

    for (Chan = 0U; Chan < SVC_OSD_CHAN_NUM; Chan++) {
        if (g_SvcOsdCLUTAddr[Chan] != 0U) {
            AmbaMisra_TypeCast(&pCLUT, &g_SvcOsdCLUTAddr[Chan]);
            if (pCLUT != NULL) {
                if (Idx >= SVC_OSD_CLUT_ENTRY_SIZE){
                    for (i = 0U; i < SVC_OSD_CLUT_ENTRY_SIZE; i++) {
                        pCLUT[i] = Color;
                    }
                } else {
                    pCLUT[Idx] = Color;
                }
            }
        }
    }
}

static void SvcOsd_DspOsdConfig(UINT32 Chan, const UINT8 *pOsdBuf)
{
    AMBA_DSP_VOUT_OSD_BUF_CONFIG_s  OsdCfg = {0U};
    ULONG                           Addr;
    const UINT8                     *pU8;

    AmbaSvcWrap_MisraMemset(&OsdCfg, 0, sizeof(OsdCfg));

    pU8 = pOsdBuf;
    AmbaMisra_TypeCast(&Addr, &pU8);
    OsdCfg.BaseAddr               = Addr;
    OsdCfg.Window.OffsetX         = 0U;
    OsdCfg.Window.OffsetY         = 0U;
    OsdCfg.InputWidth             = g_SvcOsdConfig[Chan].OsdSize.Width;
    OsdCfg.InputHeight            = g_SvcOsdConfig[Chan].OsdSize.Height;
    OsdCfg.Window.Width           = g_SvcOsdConfig[Chan].VoutSize.Width;
    OsdCfg.Window.Height          = g_SvcOsdConfig[Chan].VoutSize.Height >> g_SvcOsdConfig[Chan].VoutInterlace;
    OsdCfg.FieldRepeat            = g_SvcOsdConfig[Chan].VoutInterlace;
    OsdCfg.Pitch                  = (UINT16) g_SvcOsdCtrl[Chan].DrawBufInfo.Pitch;
    OsdCfg.DataFormat             = SVC_OSD_FORMAT;
    OsdCfg.CLUTAddr               = g_SvcOsdCLUTAddr[Chan];

    OsdCfg.SwapByteEnable         = 0U;
    OsdCfg.PremultipliedEnable    = 0U;
    OsdCfg.GlobalBlendEnable      = 0U;
    OsdCfg.TransparentColorEnable = 0U;
    OsdCfg.TransparentColor       = 0U;

    if (SVC_OK != AmbaDSP_VoutOsdConfigBuf((UINT8)Chan, &OsdCfg)) {
        SvcLog_NG(SVC_LOG_OSD, "AmbaDSP_VoutOsdConfigBuf failed", 0U, 0U);
    }
}

static void SvcOsd_AsciiDraw(const UINT8 *pAsciiPtn, UINT8 ColorVal, UINT32 BufPitch, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuf)
{
    UINT32 Rval = SVC_OK;

    if (pAsciiPtn == NULL) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_AsciiDraw(): pAsciiPtn == NULL", 0U, 0U);
    }

    if ((BufPitch == 0U) || (BufWidth == 0U) || (BufHeight == 0U)) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_AsciiDraw(): invalid buffer window", 0U, 0U);
    }

    if (pBuf == NULL) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_AsciiDraw(): pBuf == NULL", 0U, 0U);
    }

    if ((Rval == SVC_OK) && (pAsciiPtn != NULL) && (pBuf != NULL) && (BufWidth != 0U) && (BufHeight != 0U)) {
        UINT32 RowIdx, ColIdx, PtnRow, PtnCol, PtnColVal;

        for (RowIdx = 0U; RowIdx < BufHeight; RowIdx ++) {

            PtnRow = ( RowIdx * SVC_OSD_ASCII_PTN_ROW ) / BufHeight;
            if (PtnRow >= SVC_OSD_ASCII_PTN_ROW) {
                PtnRow = SVC_OSD_ASCII_PTN_ROW - 1U;
            }

            PtnColVal = pAsciiPtn[PtnRow];
            PtnColVal &= 0x1FU;

            for (ColIdx = 0U; ColIdx < BufWidth; ColIdx ++) {

                PtnCol = ( ColIdx * SVC_OSD_ASCII_PTN_COL ) / BufWidth;
                if (PtnCol >= SVC_OSD_ASCII_PTN_COL) {
                    PtnCol = SVC_OSD_ASCII_PTN_COL - 1U;
                }

                if ( ( PtnColVal & ( 0x1UL << PtnCol ) ) > 0U ) {
                    pBuf[(RowIdx * BufPitch) + ColIdx] = ColorVal;
                }
            }
        }
    } else {
        AmbaMisra_TouchUnused(SvcGuiPalAsciiPtn);
    }
}
