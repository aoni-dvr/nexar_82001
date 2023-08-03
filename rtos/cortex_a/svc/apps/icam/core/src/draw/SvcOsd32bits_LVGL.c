/**
 *  @file SvcOsd32bits_LVGL.c
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

#include "AmbaGDMA.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaCache.h"
#include "AmbaNAND.h"
#include "AmbaUtility.h"
#include "AmbaFPD.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
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

#include "LvglWrap.h"

#define AMBA_DRAW_BUFFER_32BIT_ARGB_8888    OSD_32BIT_ARGB_8888

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
} SVC_OSD_DRAW_BUFFER_INFO_s;

typedef struct {
    SVC_OSD_DRAW_BUFFER_INFO_s  DrawBufInfo;

    UINT32                   FrameSize;
    UINT16                   LineObjId;
    UINT16                   RectObjId;
    UINT16                   StrObjId;
    UINT16                   CircleObjId;
} SVC_OSD_CTRL_s;

#define SVC_LOG_OSD                 "OSD"

#define SVC_OSD_BUF_NUM             (4U)
#define SVC_OSD_MODE                AMBA_DRAW_BUFFER_32BIT_ARGB_8888

#ifdef CONFIG_ICAM_REBEL_USAGE
#define SVC_OSD_BUF_MAX_WIDTH_VOUT_A    (960U)
#define SVC_OSD_BUF_MAX_HEIGHT_VOUT_A   (540U)
#else
#define SVC_OSD_BUF_MAX_WIDTH_VOUT_A    (1920U)
#define SVC_OSD_BUF_MAX_HEIGHT_VOUT_A   (1080U)
#endif
#if defined CONFIG_ICAM_VOUTB_OSD_BUF_FHD
#define SVC_OSD_BUF_MAX_WIDTH_VOUT_B    (1920U)
#define SVC_OSD_BUF_MAX_HEIGHT_VOUT_B   (1080U)
#elif defined CONFIG_ICAM_VOUTB_OSD_BUF_HD
#define SVC_OSD_BUF_MAX_WIDTH_VOUT_B    (1280U)
#define SVC_OSD_BUF_MAX_HEIGHT_VOUT_B   (720U)
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

#define SVC_OSD_CLUT_ENTRY_SIZE   (256U)
#define SVC_OSD_EXTBUF_NOTCONFIG  (0U)
#define SVC_OSD_EXTBUF_CONFIGED   (1U)
#define SVC_OSD_EXTBUF_ENABLED    (2U)

typedef UINT32 SVC_OSD_PIXEL_BITS;

typedef struct {
    UINT8                    MemType;
    UINT8                    *pAddr[SVC_OSD_BUF_NUM];
} SVC_OSD_BUFFER_s;

static UINT8          *g_BmpBuf = NULL; /* g_BmpBuf is just used to load bmp data from FLASH. It will be copied out */

static UINT32          g_SvcOsdClutBuf[SVC_OSD_CLUT_ENTRY_SIZE] GNU_ALIGNED_CACHESAFE = {
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
#else
#if defined(CONFIG_ICAM_REBEL_CUSTOMER_NN)
    0xFF44B5FFU, // 028 ( 191, 255, 000 )   Vehicle
    0xFFFFD22FU, // 029 ( 000, 204, 000 )   Motorbike/bicycle
    0xFFFF9C2FU, // 030 ( 000,  64, 255 )   Pedestrian
#if defined(CONFIG_ICAM_REBEL_CUSTOMER_NN_LT6911)
    0xFFDCFF77U, // 031 ( 000, 191, 255 )   Traffic sign
#else //Rebel + imx424 not to draw traffic sign
    0x00000000U, // 031 ( 000, 000, 000 )   Traffic sign
#endif
    0xFF809E49U, // 032 ( 255, 128, 255 )   Traffic light
    0xFFD29B37U, // 033 ( 255, 191, 000 )   Road marker
    0xFFFFD913U, // 034 ( 255, 102, 000 )   Obstacle
#else
    // For OD
    0xFFBFFF00U, // 028 ( 191, 255, 000 )
    0xFF00CC00U, // 029 ( 000, 204, 000 )
    0xFF0040FFU, // 030 ( 000,  64, 255 )
    0xFF00BFFFU, // 031 ( 000, 191, 255 )
    0xFFFF80FFU, // 032 ( 255, 128, 255 )
    0xFFFFBF00U, // 033 ( 255, 191, 000 )
    0xFFFF6600U, // 034 ( 255, 102, 000 )
#endif
    0xFF00CC00U, // 035 ( 000, 204, 000 )
    0xFFFFFF00U, // 036 ( 255, 255, 000 )
    0xFFFF0000U, // 037 ( 255, 255, 000 )
    0xFFFF00FFU, // 038 ( 255, 000, 255 )
    0xFF8000FFU, // 039 ( 128, 000, 255 )
#if defined(CONFIG_ICAM_REBEL_CUSTOMER_NN)
    0x00000000U, // 040 ( 000, 000, 000 )
    0x00000000U, // 041 ( 000, 000, 000 )   merge
    0x00000000U, // 042 ( 000, 000, 000 )   split
    0x00000000U, // 043 ( 000, 000, 000 )   freespace
    0x00000000U, // 044 ( 000, 000, 000 )   crosswalk
#else
    0xFF00a000U, // 040 ( 000, 160, 000 )
    0xFF0080FFU, // 041 ( 000, 128, 255 )
    0xFF00FF00U, // 042 ( 000, 255, 000 )
    0xFFFFFF00U, // 043 ( 255, 255, 000 )
    0xFF80a000U, // 044 ( 128, 160, 000 )
#endif
    0xFFa0a000U, // 045 ( 160, 160, 000 )
    0xFFc0a000U, // 046 ( 192, 160, 000 )
    0xFF008080U, // 047 ( 000, 128, 128 )

    // For Segmentation
    0x00000000U, // 048 ( 000, 000, 000 )
    0x000080FFU, // 049 ( 000, 128, 255 )
    0x8000FF00U, // 050 ( 000, 255, 000 )
    0x80FFFF00U, // 051 ( 255, 255, 000 )
    0x8080c000U, // 052 ( 128, 192, 000 )
    0x80a0c000U, // 053 ( 160, 192, 000 )
    0x80c0c000U, // 054 ( 192, 192, 000 )
#endif
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
    0x80c06080U, // 158 ( 192, 096, 128 )

#if defined(CONFIG_ICAM_PROJECT_SURROUND)
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

static SVC_OSD_CTRL_s  g_SvcOsdCtrl[SVC_OSD_CHAN_NUM];
static UINT8           OSDExtBufferStatus[SVC_OSD_CHAN_NUM] = {SVC_OSD_EXTBUF_NOTCONFIG};
static SvcOSD_ExtBuffer_CFG_s    g_OSDExtBufferCFG[SVC_OSD_CHAN_NUM] = {0};
static AMBA_KAL_MUTEX_t          g_OSDExMutex[SVC_OSD_CHAN_NUM];
static SVC_OSD_CONFIG_s g_SvcOsdConfig[SVC_OSD_CHAN_NUM] = {0};
static SVC_OSD_BUFFER_s g_SvcOsdBuffer[SVC_OSD_CHAN_NUM] = {0};

static void SvcOsd_BufInit(UINT32 Chan);
static void SvcOsd_DspOsdConfig(UINT32 Chan, const UINT8 *pOsdBuf);
static void SvcOsd_UINT32ParaSwap(UINT32 *para1, UINT32 *para2);


typedef struct{
    UINT32 width;
    UINT32 height;
}Lv_size;

typedef struct{
    INT16 x;
    INT16 y;
}Lv_point;



static Lv_size Vout_Res[2]; 
static AMBA_KAL_MUTEX_t  g_LvglMutex;
static UINT32 line_count[2] = {0};
static Lv_size Font_Res[3] = {{10,12},{14,15},{30,31}};





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
    UINT32  Rval = SVC_OK;
    static UINT32 init = 0U;
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
                g_SvcOsdBuffer[Chan].pAddr[BufIdx] = &g_SvcOsdConfig[Chan].pOsdMem[AddrOffset];
            }

            if (SVC_OK != AmbaWrap_memset(&g_SvcOsdCtrl[Chan].DrawBufInfo, 0, sizeof(SVC_OSD_DRAW_BUFFER_INFO_s))) {
                SvcLog_NG(SVC_LOG_OSD, "Fail to AmbaWrap_memset", 0U, 0U);
                Rval = SVC_NG;
            }

            if (Rval != SVC_NG) {
                g_SvcOsdCtrl[Chan].DrawBufInfo.PixelFormat = SVC_OSD_MODE;
                g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr     = g_SvcOsdBuffer[Chan].pAddr[0];  /* Draw on second buffer first. 1st buffer will be empty */
                g_SvcOsdCtrl[Chan].DrawBufInfo.Width       = g_SvcOsdConfig[Chan].OsdSize.Width;
                g_SvcOsdCtrl[Chan].DrawBufInfo.Pitch       = ((UINT32)(g_SvcOsdConfig[Chan].OsdSize.Width) << SVC_OSD_PIXEL_SIZE_SFT);
                g_SvcOsdCtrl[Chan].DrawBufInfo.Height      = g_SvcOsdConfig[Chan].OsdSize.Height;
                g_SvcOsdCtrl[Chan].FrameSize = (g_SvcOsdCtrl[Chan].DrawBufInfo.Pitch *
                                                g_SvcOsdCtrl[Chan].DrawBufInfo.Height);

                SvcOsd_BufInit(Chan);
                SvcOsd_DspOsdConfig(Chan, g_SvcOsdBuffer[Chan].pAddr[0]);

                Vout_Res[Chan].width = g_SvcOsdCtrl[Chan].DrawBufInfo.Width;
                Vout_Res[Chan].height = g_SvcOsdCtrl[Chan].DrawBufInfo.Height;
                Rval = LvglWrap_Init(Chan ,g_SvcOsdBuffer[Chan].pAddr, g_SvcOsdCtrl[Chan].DrawBufInfo.Width, g_SvcOsdCtrl[Chan].DrawBufInfo.Height); 
                if(init == 0U){
                    char TempName[] = "SvcOSD_LvglMutex";
                    Rval = AmbaKAL_MutexCreate(&g_LvglMutex, TempName);
                    if (Rval != 0U) {
                        SvcLog_NG(SVC_LOG_OSD, "Fail to AmbaKAL_MutexCreate. ErrCode(0x%08X)", Rval, 0U);
                    }
                    init = 1U;
                }
            }
        }

    return Rval;
}

/**
 * Swap two UINT32 Parameters
 * @param [in] UINT32 Parameter 1
 * @param [in] UINT32 Parameter 2
*/
static void SvcOsd_UINT32ParaSwap(UINT32 *para1, UINT32 *para2)
{
    
    UINT32 temp;

    if( (para1 != NULL) && (para2!=NULL) ){

        temp = *para1;
        *para1 = *para2;
        *para2 = temp;

    }
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
    UINT32 RetVal = SVC_OK;

    UINT32 MinX, DiffX;
    UINT32 MinY, DiffY;

    if (g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr == NULL) {
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawRect(%u): pRGBAddr == NULL", Chan, 0U);
        RetVal = SVC_NG;
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


    if(RetVal == SVC_OK){
        (void)AmbaKAL_MutexTake(&g_LvglMutex, AMBA_KAL_WAIT_FOREVER);
            LvglWrap_DrawRect(Chan, MinX, MinY, DiffX+MinX, DiffY+MinY, Color, Thickness);
        (void)AmbaKAL_MutexGive(&g_LvglMutex);
    }

    return RetVal;
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

    if (Chan >= SVC_OSD_CHAN_NUM) {
        RetVal = SVC_NG;
    } else {
        UINT32 NewXstart, NewYstart, NewXend, NewYend;

        /* To pass misra-c */
        NewXstart = Xstart;
        NewYstart = Ystart;
        NewXend   = Xend;
        NewYend   = Yend;

        if((Xstart>Vout_Res[Chan].width) ||  (Ystart>Vout_Res[Chan].height) ){

            SvcLog_NG(SVC_LOG_OSD, "[SvcOsd_DrawSolidRect] Xstart:[%u], Ystart:[%u]", Xstart, Ystart);

            RetVal = SVC_NG;

        }


        if( (Xend>Vout_Res[Chan].width) || (Yend>Vout_Res[Chan].height) ){

            SvcLog_NG(SVC_LOG_OSD, "[SvcOsd_DrawSolidRect] Xend:[%u], Yend:[%u]", Xend, Yend);

            RetVal = SVC_NG;

        }

        if(RetVal == SVC_OK){
            if(Xstart > Xend){
                SvcOsd_UINT32ParaSwap(&NewXstart, &NewXend);
            }

            if(Ystart > Yend){
                SvcOsd_UINT32ParaSwap(&NewYstart, &NewYend);
            }

            (void)AmbaKAL_MutexTake(&g_LvglMutex, AMBA_KAL_WAIT_FOREVER);
            LvglWrap_DrawSolidRect(Chan, NewXstart, NewYstart, NewXend, NewYend, Color);
            (void)AmbaKAL_MutexGive(&g_LvglMutex);
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
    UINT32 RetVal = SVC_OK;

    if (Chan >= SVC_OSD_CHAN_NUM) {
        RetVal = SVC_NG;
    } else {
        UINT32 StringLength = 0U;

        StringLength = (UINT32) AmbaUtility_StringLength(pString);

        if(FontSize == 1U){
            if( ((Xstart + (Font_Res[0].width*StringLength)) > Vout_Res[Chan].width) ||  ((Ystart + Font_Res[0].height) > Vout_Res[Chan].height)){

                SvcLog_DBG(SVC_LOG_OSD, "[SvcOsd_DrawString] Extra-Small Out of Boundary Xstart:[%u], Ystart:[%u]", Xstart, Ystart);

                RetVal = SVC_NG;

            }
        }

        else if(FontSize == 2U){
            if(  ((Xstart + (Font_Res[1].width*StringLength)) > Vout_Res[Chan].width) ||  ((Ystart + Font_Res[1].height) > Vout_Res[Chan].height)){

                SvcLog_DBG(SVC_LOG_OSD, "[SvcOsd_DrawString] Small-Font Out of Boundary Xstart:[%u], Ystart:[%u]", Xstart, Ystart);

                RetVal = SVC_NG;

            }
        } else{
            if( ((Xstart + (Font_Res[2].width * StringLength)) > Vout_Res[Chan].width) ||((Ystart + Font_Res[2].height) > Vout_Res[Chan].height)) {

                SvcLog_DBG(SVC_LOG_OSD, "[SvcOsd_DrawString] Small-Font Out of Boundary Xstart:[%u], Ystart:[%u]", Xstart, Ystart);

                RetVal = SVC_NG;
            
            }
        }

        if(RetVal==SVC_OK){
            (void)AmbaKAL_MutexTake(&g_LvglMutex, AMBA_KAL_WAIT_FOREVER);
            LvglWrap_DrawString(Chan, Xstart, Ystart, FontSize, Color, pString);
            (void)AmbaKAL_MutexGive(&g_LvglMutex);
        }
    }

    return RetVal;

}

UINT32 SvcOsd_GetFontRes(UINT32 font_size, UINT32 *Width, UINT32 *Height)
{

    UINT32 RetVal=SVC_OK;

    if( (font_size >= 4U) || (font_size <= 0U )){
        RetVal = SVC_NG;
    }
    else{
        if( (Width != NULL) && (Height!=NULL) ){

            *Width = Font_Res[font_size-1U].width;
            *Height = Font_Res[font_size-1U].height;
        }
        else{
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
    UINT32 RetVal = SVC_OK;

    INT32 circle_Xstart = (INT32)Xstart - (INT32)Radius;
    INT32 circle_Ystart = (INT32)Ystart - (INT32)Radius;

    if((circle_Xstart <0 ) ||  (circle_Ystart <0 )){
        SvcLog_NG(SVC_LOG_OSD, "[SvcOsd_DrawCircle] Smaller than zero, Xstart:[%u], Ystart:[%u]", Xstart, Ystart);

        RetVal = SVC_NG;
    }

    if( ((Xstart+Radius)>Vout_Res[Chan].width) ||  ((Ystart+Radius)>Vout_Res[Chan].height)){
        
        SvcLog_NG(SVC_LOG_OSD, "[SvcOsd_DrawCircle] Over Osd-size Xend:[%u], Yend:[%u]", Xstart+Radius, Ystart+Radius);

        RetVal = SVC_NG;
    }

    if(RetVal == SVC_OK){
        (void)AmbaKAL_MutexTake(&g_LvglMutex, AMBA_KAL_WAIT_FOREVER);
            LvglWrap_DrawCircle(Chan, Xstart, Ystart, Radius, LineWidth, Color);
        (void)AmbaKAL_MutexGive(&g_LvglMutex);
    }
    return RetVal;
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
    UINT32 RetVal = SVC_OK;
    UINT32 LineTotal = 0U;
    

    if( ((X1+LineWidth)>Vout_Res[Chan].width) ||  ((Y1+LineWidth)>Vout_Res[Chan].height)){
        
        SvcLog_NG(SVC_LOG_OSD, "[SvcOsd_DrawLine] X1:[%u], Y1:[%u]", X1, Y1);

        RetVal = SVC_NG;

    }


    if(((X2+LineWidth)>Vout_Res[Chan].width) || ((Y2+LineWidth)>Vout_Res[Chan].height)){
        
        SvcLog_NG(SVC_LOG_OSD, "[SvcOsd_DrawLine] X2:[%u], Y2:[%u]", X2, Y2);

        RetVal = SVC_NG;

    }



    if( (line_count[Chan]+2U) > SVC_OSD_MAX_LINE_POINTS){
        SvcLog_NG(SVC_LOG_OSD,"Already more than [%u] points",SVC_OSD_MAX_LINE_POINTS, 0U);

        RetVal = SVC_NG;
    }

    if(RetVal == SVC_OK){

        (void)AmbaKAL_MutexTake(&g_LvglMutex, AMBA_KAL_WAIT_FOREVER);    
        LvglWrap_DrawLine(Chan, X1, Y1, X2, Y2, LineWidth, Color, &LineTotal);
        line_count[Chan] = LineTotal;
        (void)AmbaKAL_MutexGive(&g_LvglMutex);
    }

    return RetVal;
}

/**
 * Svc Osd Draw multiple points and connect all this points function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] Points Structure of x,y coordinate with INT16 value. Pass the address of first point
 * @param [in] PointNUm Number of points to draw
 * @param [in] LineWidth Line Width
 * @param [in] Color Color of the line connects these points
 * return 0-OK, 1-NG
*/

UINT32 SvcOsd_DrawMultiplePointsLine(UINT32 Chan, void *Points, UINT32 PointNum,UINT32 LineWidth, UINT32 Color)
{
    
    UINT32 RetVal = SVC_OK;

/*
Points Structure
typedef struct{
    INT16 x;
    INT16 y;
}Lv_point;
*/

    UINT32 i;
    UINT32 LineTotal = 0U;

    const Lv_point * pPoint;


    if(Points == NULL){
        RetVal = SVC_NG;
    }

    if( (line_count[Chan]+PointNum) > SVC_OSD_MAX_LINE_POINTS){
        SvcLog_NG(SVC_LOG_OSD,"Already more than [%u] points",SVC_OSD_MAX_LINE_POINTS, 0U);
        RetVal = SVC_NG;
    }

    if(PointNum==0U){
        SvcLog_NG(SVC_LOG_OSD,"Point Number is :[%u]",PointNum, 0U);
        RetVal = SVC_NG;
    }

    if(RetVal == SVC_OK){

        AmbaMisra_TypeCast(&pPoint, &Points);
        
        for(i=0; i<PointNum; i++){

            if( (((UINT32)pPoint[i].x+LineWidth)>Vout_Res[Chan].width) ||(  ((UINT32)pPoint[i].y+LineWidth)>Vout_Res[Chan].height)){
                
                SvcLog_NG(SVC_LOG_OSD, "[SvcOsd_DrawMultiplePointsLine] X:[%u], Y:[%u]", (UINT32)pPoint[i].x,(UINT32)pPoint[i].y);
                RetVal = SVC_NG;

            }

        }

        if(RetVal == SVC_OK){
            (void)AmbaKAL_MutexTake(&g_LvglMutex, AMBA_KAL_WAIT_FOREVER);
            LvglWrap_DrawMultiplePointsLine(Chan, Points, PointNum, LineWidth, Color, &LineTotal);
            line_count[Chan] = LineTotal;
            (void)AmbaKAL_MutexGive(&g_LvglMutex);
        }
    }

    return RetVal;

}



/**
 * Svc Osd clear function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_Clear(UINT32 Chan)
{
    UINT32 RetVal = SVC_OK;

    if (Chan >= SVC_OSD_CHAN_NUM) {
        RetVal = SVC_NG;
    } else {
        AmbaSvcWrap_MisraMemset(g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr, 0, g_SvcOsdCtrl[Chan].FrameSize);
        line_count[Chan] = 0U;
    }

    return RetVal;
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

    SVC_OSD_PIXEL_BITS *pRGBAddr;
    SVC_OSD_PIXEL_BITS *pCleanAddr;

    AmbaMisra_TypeCast(&pRGBAddr, &g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr);

    for (i = Ystart; i < Yend; i++) {
        pCleanAddr = &pRGBAddr[(i * g_SvcOsdConfig[Chan].OsdSize.Width) + Xstart];
        RetVal = AmbaWrap_memset(pCleanAddr, 0, (UINT32) ((UINT32)sizeof(SVC_OSD_PIXEL_BITS) * (UINT32)(Xend - Xstart)));
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
    UINT32 Width, Height;
    UINT8  *pBuf;
    AmbaMisra_TouchUnused(&BufSize);

    AmbaMisra_TypeCast(&pBuf, &pDestBuf);

    if (g_BmpBuf == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_LoadBmp failed. initial SvcOsd_Init first", 0U, 0U);
    } else {
        RetVal = SvcOsd_LoadRomData(g_BmpBuf, (UINT32) (sizeof(UINT8) * SVC_OSD_BUF_SIZE), pPath);

        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_OSD, "SvcOsd_LoadBmp failed. Load Rom Data err %d", RetVal, 0U);
        } else {

            RetVal = AmbaWrap_memcpy(&Width, &g_BmpBuf[18], sizeof(UINT32));
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_OSD, "SvcOsd_LoadBmp err. AmbaWrap_memcpy Width failed 0x%x", RetVal, 0U);
            }
            RetVal = AmbaWrap_memcpy(&Height, &g_BmpBuf[22], sizeof(UINT32));
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_OSD, "SvcOsd_LoadBmp err. AmbaWrap_memcpy Height failed 0x%x", RetVal, 0U);
            }

            /* Find the offset of bmp data */
            RetVal = AmbaWrap_memcpy(&Offset, &g_BmpBuf[10], sizeof(UINT32));
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_OSD, "SvcOsd_LoadBmp err. AmbaWrap_memcpy Offset failed 0x%x", RetVal, 0U);
            }

            Width = Width << 2;


            if (SVC_OK == RetVal) {
                UINT32 i, j;
                AmbaSvcWrap_MisraMemset(pDestBuf, 0, sizeof(UINT8) * Width * Height );

                j = Height - 1U;
                for (i = 0U; i < Height; i++) {
                    const UINT8 *pSrcAddr = &g_BmpBuf[Offset + (j * Width)];
                    UINT8 *pDstAddr = &pBuf[i * Width];
                    AmbaSvcWrap_MisraMemcpy(pDstAddr, pSrcAddr, sizeof(UINT8) * Width);
                    j = j - 1U;
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
        RetVal = SvcOsd_LoadRomData(g_BmpBuf, (UINT32) (sizeof(UINT8) * SVC_OSD_BUF_SIZE), pPath );
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
    UINT32 Width;
    UINT32 Height;

    UINT32 NewXstart, NewYstart, NewXend, NewYend;

    /* To pass misra-c */
    NewXstart = Xstart;
    NewYstart = Ystart;
    NewXend   = Xend;
    NewYend   = Yend;


    if(pBmpInfo == NULL){
        RetVal = SVC_NG;
    }


    if(Xstart > Xend){
        SvcOsd_UINT32ParaSwap(&NewXstart, &NewXend);
    }

    if(Ystart > Yend){
        SvcOsd_UINT32ParaSwap(&NewYstart, &NewYend);
    }


    Width = NewXend - NewXstart;
    Height = NewYend - NewYstart;


    if( (NewXstart>Vout_Res[Chan].width) ||  (NewYstart>Vout_Res[Chan].height)) {
        
        SvcLog_NG(SVC_LOG_OSD, "[SvcOsd_DrawBmp] Xstart:[%u], Ystart:[%u]", NewXstart, NewYstart);
        RetVal = SVC_NG;

    }


    if( ((NewXstart+Width)>Vout_Res[Chan].width) || ((NewYstart+Height)>Vout_Res[Chan].height)) {
        
        SvcLog_NG(SVC_LOG_OSD, "[SvcOsd_DrawBmp] Xend:[%u], Yend:[%u]", NewXstart+Width, NewYstart+Height);
        RetVal = SVC_NG;
    }


    if(RetVal == SVC_OK){
        (void)AmbaKAL_MutexTake(&g_LvglMutex, AMBA_KAL_WAIT_FOREVER);

        LvglWrap_DrawBmp(Chan, NewXstart, NewYstart, pBmpInfo);

        (void)AmbaKAL_MutexGive(&g_LvglMutex);

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

    SVC_OSD_PIXEL_BITS *pRGBAddr;
    SVC_OSD_PIXEL_BITS *pRGBDstAddr;

    const SVC_OSD_PIXEL_BITS *pBinAddr;
    const SVC_OSD_PIXEL_BITS *pBinSrcAddr;

    AmbaMisra_TypeCast(&pRGBAddr, &g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr);
    AmbaMisra_TypeCast(&pBinAddr, &pBinBuf);

    j = 0U;
    for (i = Ystart; i < Yend; i++) {
        pRGBDstAddr = &pRGBAddr[(i * g_SvcOsdConfig[Chan].OsdSize.Width) + Xstart];
        pBinSrcAddr = &pBinAddr[j * CopiedBinWidth];
        RetVal = AmbaWrap_memcpy(pRGBDstAddr, pBinSrcAddr, sizeof(SVC_OSD_PIXEL_BITS) * CopiedBinWidth);
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
 * Svc Osd draw binary data into buffer r with software rescaling
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
    RGBAddr += ( (ULONG) Ystart * (ULONG) OsdPitch);
    RGBAddr += ((ULONG) Xstart << SVC_OSD_PIXEL_SIZE_SFT);
    AmbaMisra_TypeCast(&RsCfg.pDst, &RGBAddr);

    RsCfg.DstWidth  = Xend - Xstart;
    RsCfg.DstHeight = Yend - Ystart;
    RsCfg.DstPitch  = OsdPitch;
    RetVal = SvcCvImgUtil_RescaleBitMap_Idx2Color(&RsCfg);

    return RetVal;
}

/**
 * Svc Osd flush function
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * return 0-OK, 1-NG
 */
UINT32 SvcOsd_Flush(UINT32 Chan)
{
    UINT32 Rval = SVC_OK;
    ULONG  AddrU32;

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
    AmbaMisra_TouchUnused(&Chan);
    (void) pCLUT;

    return SVC_NG;
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

            if(Chan == VOUT_IDX_A) {
                ActWidth = g_SvcOsdConfig[Chan].OsdSize.Width;
                ActHeight = g_SvcOsdConfig[Chan].OsdSize.Height;
            } else {
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
                AmbaSvcWrap_MisraMemset(&g_OSDExMutex[Chan], 0, sizeof(AMBA_KAL_MUTEX_t));
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
            OsdCfg.CLUTAddr = 0U;
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

    if (pOsdWorkMem == NULL) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_SetWorkBuf failed. Input working buffer not be NULL", 0U, 0U);
    } else if (g_BmpBuf != NULL) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_SetWorkBuf failed. working buffer has been set", 0U, 0U);
    } else {
        g_BmpBuf = pOsdWorkMem;
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
        SvcLog_NG(SVC_LOG_OSD, "[ERROR] SvcOsd_GetClut: invalid pClut", 0U, 0U);
    }

    if (Rval == SVC_OK) {
        *pCLUT = &g_SvcOsdClutBuf[0];
        *pNumColor = SVC_OSD_CLUT_ENTRY_SIZE;
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
    
    AmbaMisra_TouchUnused(&Idx);
    if ((Rval == SVC_OK) && (Idx < SVC_OSD_CLUT_ENTRY_SIZE)) {
        *pColor = g_SvcOsdClutBuf[Idx];
    }

    return Rval;
}

/**
 * Convert color index to color setting for specific OSD format
 * @param [in] Chan 0 for Vout A and 1 for Vout B
 * @param [in] Idx color index for look up table
 * @param [out] pColor setting. 8 bits OSD - color index, 32 bits OSD - color
 * return none
 */
void SvcOsd_ColorIdxToColorSetting(UINT32 Chan, UINT32 Idx, UINT32 *pColor)
{
    if (NULL != pColor) {
        if (Idx < SVC_OSD_CLUT_ENTRY_SIZE) {
            *pColor = g_SvcOsdClutBuf[Idx];
        } else {
            *pColor = 0x80808080U;
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

        if( (OsdBufWidth != 0U) && (OsdBufHeight != 0U)){
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

    if (Chan < SVC_OSD_CHAN_NUM) {
        if (pMemSize == NULL) {
            SvcLog_NG(SVC_LOG_OSD, "SvcOsd_MemSizeQuery failed. pMemSize cannot be NULL", 0U, 0U);
            RetVal = SVC_NG;
        }else{
            if (Chan == 0U) {
                *pMemSize = (UINT32) ((SVC_OSD_BUF_MAX_WIDTH_VOUT_A * SVC_OSD_BUF_MAX_HEIGHT_VOUT_A) * SVC_OSD_BUF_NUM * sizeof(UINT8));
            } else {
                *pMemSize = (UINT32)((SVC_OSD_BUF_MAX_WIDTH_VOUT_B * SVC_OSD_BUF_MAX_HEIGHT_VOUT_B) * SVC_OSD_BUF_NUM * sizeof(UINT8));
            }
            *pMemSize = *pMemSize << SVC_OSD_PIXEL_SIZE_SFT;
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
    }

    return RetVal;
}

static void SvcOsd_BufInit(UINT32 Chan)
{
    if (g_SvcOsdCtrl[Chan].DrawBufInfo.PixelFormat != AMBA_DRAW_BUFFER_32BIT_ARGB_8888) {
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_BufInit(%u): Not supported OSD mode.", Chan, 0U);
    }

    if (g_SvcOsdConfig[Chan].pOsdMem != NULL) {
        UINT32 MemSize = 0U;
        if (Chan == VOUT_IDX_A) {
            MemSize = (UINT32) ((SVC_OSD_BUF_MAX_WIDTH_VOUT_A * SVC_OSD_BUF_MAX_HEIGHT_VOUT_A) * SVC_OSD_BUF_NUM * sizeof(SVC_OSD_PIXEL_BITS));
        } else {
            MemSize = (UINT32) ((SVC_OSD_BUF_MAX_WIDTH_VOUT_B * SVC_OSD_BUF_MAX_HEIGHT_VOUT_B) * SVC_OSD_BUF_NUM * sizeof(SVC_OSD_PIXEL_BITS));
        }
        if (SVC_OK != AmbaWrap_memset(g_SvcOsdConfig[Chan].pOsdMem, 0, MemSize)) {
            SvcLog_NG(SVC_LOG_OSD, "SvcOsd_BufInit(%u): Falied to memset the buffer", Chan, 0U);
        }
    }
}

void SvcOsd_SetClut(UINT32 Idx, UINT32 Color)
{
    UINT32 i;

    if (Idx >= SVC_OSD_CLUT_ENTRY_SIZE){
        for (i = 0U; i < SVC_OSD_CLUT_ENTRY_SIZE; i++) {
            g_SvcOsdClutBuf[i] = Color;
        }
    } else {
        g_SvcOsdClutBuf[Idx] = Color;
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
    OsdCfg.Pitch                  = OsdCfg.InputWidth << SVC_OSD_PIXEL_SIZE_SFT;
    OsdCfg.DataFormat             = SVC_OSD_FORMAT;
    OsdCfg.CLUTAddr               = 0U;
    OsdCfg.SwapByteEnable         = 0U;
    OsdCfg.PremultipliedEnable    = 0U;
    OsdCfg.GlobalBlendEnable      = 0U;
    OsdCfg.TransparentColorEnable = 0U;
    OsdCfg.TransparentColor       = 0U;

    if (SVC_OK != AmbaDSP_VoutOsdConfigBuf((UINT8)Chan, &OsdCfg)) {
        SvcLog_NG(SVC_LOG_OSD, "AmbaDSP_VoutOsdConfigBuf failed", 0U, 0U);
    }
}



void SvcOsd_LvglDraw(UINT32 VoutID, UINT32 Level)
{


    (void)AmbaKAL_MutexTake(&g_LvglMutex, AMBA_KAL_WAIT_FOREVER);

     LvglWrap_DrawOSD(VoutID, Level);

    (void)AmbaKAL_MutexGive(&g_LvglMutex);
        
}

void SvcOsd_SetLvglBuffer(UINT32 Chan)
{
    UINT32 BufIdx = 0U;

    for (BufIdx = 0U; BufIdx < SVC_OSD_BUF_NUM; BufIdx++) {
        if (g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr == g_SvcOsdBuffer[Chan].pAddr[BufIdx]) {
            LvglWrap_SetBuffer(Chan, BufIdx, g_SvcOsdBuffer[Chan].pAddr[BufIdx]);
            break;
        }
    }

}


void SvcOsd_DrawDiagBoxRect(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, UINT32 Thickness, UINT32 Bg_Color, UINT32 Border_Color)
{

    UINT32 RetVal = SVC_OK;
    UINT32 MinX, DiffX;
    UINT32 MinY, DiffY;

    if (g_SvcOsdCtrl[Chan].DrawBufInfo.RGBAddr == NULL) {
        SvcLog_NG(SVC_LOG_OSD, "SvcOsd_DrawDiagBoxRect(%u): pRGBAddr == NULL", Chan, 0U);
        RetVal = SVC_NG;
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


    if(RetVal == SVC_OK){
        (void)AmbaKAL_MutexTake(&g_LvglMutex, AMBA_KAL_WAIT_FOREVER);
            LvglWrap_DrawDiagBoxRect(Chan, MinX, MinY, DiffX+MinX, DiffY+MinY, Thickness, Bg_Color, Border_Color);
        (void)AmbaKAL_MutexGive(&g_LvglMutex);
    }
}
