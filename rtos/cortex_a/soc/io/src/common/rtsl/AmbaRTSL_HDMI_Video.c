// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
 * @file AmbaRTSL_HDMI_Video.c
 *
 * @copyright Copyright (c) 2021 Ambarella International LP
 *
 * @details HDMI Video RTSL Device Driver
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#ifdef CONFIG_LINUX
#include "osal.h"
#endif
#include "AmbaRTSL_HDMI.h"
#include "AmbaCSL_HDMI.h"

#define FRATE_24HZ      AMBA_VIDEO_FIELD_RATE_24HZ
#define FRATE_25HZ      AMBA_VIDEO_FIELD_RATE_25HZ
#define FRATE_30HZ      AMBA_VIDEO_FIELD_RATE_30HZ
#define FRATE_50HZ      AMBA_VIDEO_FIELD_RATE_50HZ
#define FRATE_60HZ      AMBA_VIDEO_FIELD_RATE_60HZ
#define FRATE_100HZ     AMBA_VIDEO_FIELD_RATE_100HZ
#define FRATE_120HZ     AMBA_VIDEO_FIELD_RATE_120HZ
#define FRATE_200HZ     AMBA_VIDEO_FIELD_RATE_200HZ
#define FRATE_240HZ     AMBA_VIDEO_FIELD_RATE_240HZ

#define HV_POL_P_P      AMBA_VIDEO_SYNC_H_POS_V_POS
#define HV_POL_N_N      AMBA_VIDEO_SYNC_H_NEG_V_NEG

#define SCAN_P          AMBA_VIDEO_SCAN_PROGRESSIVE
#define SCAN_I          AMBA_VIDEO_SCAN_INTERLACED

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define NUM_CEA861_DTD  73U
#define MAX_CEA861_VIC  205U
#else
#define NUM_CEA861_DTD  72U
#define MAX_CEA861_VIC  107U
#endif
static const AMBA_HDMI_VIDEO_TIMING_s AmbaHdmiVicDtd[NUM_CEA861_DTD]  = {
    /* Video_ID_Code    PCLK {    Hblank    }{   Vblank  }{  Active  }  Frame_Rate {Hpol  Vpol} Scan    Repetition */
    {  1,  0,  0,  0,  25175,   16,  96,  48, 10,  2,  33,  640,  480,  FRATE_60HZ, HV_POL_N_N, SCAN_P, 0 }, /* VESA DMT */
    {  8,  9,  0,  0,  27000,   38, 124, 114,  4,  3,  15, 1440,  240,  FRATE_60HZ, HV_POL_N_N, SCAN_P, 1 }, /* 240P */
    {  8,  9,  0,  0,  27000,   38, 124, 114,  5,  3,  15, 1440,  240,  FRATE_60HZ, HV_POL_N_N, SCAN_P, 1 }, /* 240P */
    { 12, 13,  0,  0,  54000,   76, 248, 228,  4,  3,  15, 2880,  240,  FRATE_60HZ, HV_POL_N_N, SCAN_P, 1 }, /* 240P 4X */
    { 12, 13,  0,  0,  54000,   76, 248, 228,  5,  3,  15, 2880,  240,  FRATE_60HZ, HV_POL_N_N, SCAN_P, 1 }, /* 240P 4X */
    { 23, 24,  0,  0,  27000,   24, 126, 138,  2,  3,  19, 1440,  288,  FRATE_50HZ, HV_POL_N_N, SCAN_P, 1 }, /* 288P */
    { 23, 24,  0,  0,  27000,   24, 126, 138,  3,  3,  19, 1440,  288,  FRATE_50HZ, HV_POL_N_N, SCAN_P, 1 }, /* 288P */
    { 23, 24,  0,  0,  27000,   24, 126, 138,  4,  3,  19, 1440,  288,  FRATE_50HZ, HV_POL_N_N, SCAN_P, 1 }, /* 288P */
    { 27, 28,  0,  0,  54000,   48, 252, 276,  2,  3,  19, 2880,  288,  FRATE_50HZ, HV_POL_N_N, SCAN_P, 1 }, /* 288P 4X */
    { 27, 28,  0,  0,  54000,   48, 252, 276,  3,  3,  19, 2880,  288,  FRATE_50HZ, HV_POL_N_N, SCAN_P, 1 }, /* 288P 4X */
    { 27, 28,  0,  0,  54000,   48, 252, 276,  4,  3,  19, 2880,  288,  FRATE_50HZ, HV_POL_N_N, SCAN_P, 1 }, /* 288P 4X */
    {  6,  7,  0,  0,  27000,   38, 124, 114,  4,  3,  15, 1440,  480,  FRATE_60HZ, HV_POL_N_N, SCAN_I, 1 }, /* 480I */
    { 50, 51,  0,  0,  54000,   38, 124, 114,  4,  3,  15, 1440,  480, FRATE_120HZ, HV_POL_N_N, SCAN_I, 1 }, /* 480I60 */
    { 58, 59,  0,  0, 108000,   38, 124, 114,  4,  3,  15, 1440,  480, FRATE_240HZ, HV_POL_N_N, SCAN_I, 1 }, /* 480I120 */
    { 10, 11,  0,  0,  54000,   76, 248, 228,  4,  3,  15, 2880,  480,  FRATE_60HZ, HV_POL_N_N, SCAN_I, 1 }, /* 480I 4X */
    {  2,  3,  0,  0,  27000,   16,  62,  60,  9,  6,  30,  720,  480,  FRATE_60HZ, HV_POL_N_N, SCAN_P, 0 }, /* 480P */
    { 48, 49,  0,  0,  54000,   16,  62,  60,  9,  6,  30,  720,  480, FRATE_120HZ, HV_POL_N_N, SCAN_P, 0 }, /* 480P120 */
    { 56, 57,  0,  0, 108000,   16,  62,  60,  9,  6,  30,  720,  480, FRATE_240HZ, HV_POL_N_N, SCAN_P, 0 }, /* 480P240 */
    { 14, 15,  0,  0,  54000,   32, 124, 120,  9,  6,  30, 1440,  480,  FRATE_60HZ, HV_POL_N_N, SCAN_P, 1 }, /* 480P 2X */
    { 35, 36,  0,  0, 108000,   64, 248, 240,  9,  6,  30, 2880,  480,  FRATE_60HZ, HV_POL_N_N, SCAN_P, 0 }, /* 480P 4X */
    { 21, 22,  0,  0,  27000,   24, 126, 138,  2,  3,  19, 1440,  576,  FRATE_50HZ, HV_POL_N_N, SCAN_I, 1 }, /* 576I */
    { 44, 45,  0,  0,  54000,   24, 126, 138,  2,  3,  19, 1440,  576, FRATE_100HZ, HV_POL_N_N, SCAN_I, 1 }, /* 576I50 */
    { 54, 55,  0,  0, 108000,   24, 126, 138,  2,  3,  19, 1440,  576, FRATE_200HZ, HV_POL_N_N, SCAN_I, 1 }, /* 576I100 */
    { 25, 26,  0,  0,  54000,   48, 252, 276,  2,  3,  19, 2880,  576,  FRATE_50HZ, HV_POL_N_N, SCAN_I, 1 }, /* 576I 4X */
    { 17, 18,  0,  0,  27000,   12,  64,  68,  5,  5,  39,  720,  576,  FRATE_50HZ, HV_POL_N_N, SCAN_P, 0 }, /* 576P */
    { 42, 43,  0,  0,  54000,   12,  64,  68,  5,  5,  39,  720,  576, FRATE_100HZ, HV_POL_N_N, SCAN_P, 0 }, /* 576P100 */
    { 52, 53,  0,  0, 108000,   12,  64,  68,  5,  5,  39,  720,  576, FRATE_200HZ, HV_POL_N_N, SCAN_P, 0 }, /* 576P200 */
    { 29, 30,  0,  0,  54000,   24, 128, 136,  5,  5,  39, 1440,  576,  FRATE_50HZ, HV_POL_N_N, SCAN_P, 1 }, /* 576P 2X */
    { 37, 38,  0,  0, 108000,   48, 256, 272,  5,  5,  39, 2880,  576,  FRATE_50HZ, HV_POL_N_N, SCAN_P, 1 }, /* 576P 4X */

    {  0, 60, 65,  0,  59400, 1760,  40, 220,  5,  5,  20, 1280,  720,  FRATE_24HZ, HV_POL_P_P, SCAN_P, 0 }, /* 720P24 */
    {  0, 61, 66,  0,  74250, 2420,  40, 220,  5,  5,  20, 1280,  720,  FRATE_25HZ, HV_POL_P_P, SCAN_P, 0 }, /* 720P25 */
    {  0, 62, 67,  0,  74250, 1760,  40, 220,  5,  5,  20, 1280,  720,  FRATE_30HZ, HV_POL_P_P, SCAN_P, 0 }, /* 720P30 */
    {  0, 19, 68,  0,  74250,  440,  40, 220,  5,  5,  20, 1280,  720,  FRATE_50HZ, HV_POL_P_P, SCAN_P, 0 }, /* 720P50 */
    {  0,  4, 69,  0,  74250,  110,  40, 220,  5,  5,  20, 1280,  720,  FRATE_60HZ, HV_POL_P_P, SCAN_P, 0 }, /* 720P60 */
    {  0, 41, 70,  0, 148500,  440,  40, 220,  5,  5,  20, 1280,  720, FRATE_100HZ, HV_POL_P_P, SCAN_P, 0 }, /* 720P100 */
    {  0, 47, 71,  0, 148500,  110,  40, 220,  5,  5,  20, 1280,  720, FRATE_120HZ, HV_POL_P_P, SCAN_P, 0 }, /* 720P120 */
    {  0,  0, 79,  0,  59400, 1360,  40, 220,  5,  5,  20, 1680,  720,  FRATE_24HZ, HV_POL_P_P, SCAN_P, 0 }, /* 720P24 (1680) */
    {  0,  0, 80,  0,  59400, 1228,  40, 220,  5,  5,  20, 1680,  720,  FRATE_25HZ, HV_POL_P_P, SCAN_P, 0 }, /* 720P25 (1680) */
    {  0,  0, 81,  0,  59400,  700,  40, 220,  5,  5,  20, 1680,  720,  FRATE_30HZ, HV_POL_P_P, SCAN_P, 0 }, /* 720P30 (1680) */
    {  0,  0, 82,  0,  82500,  260,  40, 220,  5,  5,  20, 1680,  720,  FRATE_50HZ, HV_POL_P_P, SCAN_P, 0 }, /* 720P50 (1680) */
    {  0,  0, 83,  0,  99000,  260,  40, 220,  5,  5,  20, 1680,  720,  FRATE_60HZ, HV_POL_P_P, SCAN_P, 0 }, /* 720P60 (1680) */
    {  0,  0, 84,  0, 165000,   60,  40, 220,  5,  5,  95, 1680,  720, FRATE_100HZ, HV_POL_P_P, SCAN_P, 0 }, /* 720P100 (1680) */
    {  0,  0, 85,  0, 198000,   60,  40, 220,  5,  5,  95, 1680,  720, FRATE_120HZ, HV_POL_P_P, SCAN_P, 0 }, /* 720P120 (1680) */
    {  0,  5,  0,  0,  74250,   88,  44, 148,  2,  5,  15, 1920, 1080,  FRATE_60HZ, HV_POL_P_P, SCAN_I, 0 }, /* 1080I30 */
    {  0, 46,  0,  0, 148500,   88,  44, 148,  2,  5,  15, 1920, 1080, FRATE_120HZ, HV_POL_P_P, SCAN_I, 0 }, /* 1080I60 */
    {  0, 39,  0,  0,  72000,   32, 168, 184, 23,  5,  57, 1920, 1080,  FRATE_50HZ, HV_POL_P_P, SCAN_I, 0 }, /* 1080I25 (AS 4933.1) */
    {  0, 20,  0,  0,  74250,  528,  44, 148,  2,  5,  15, 1920, 1080,  FRATE_50HZ, HV_POL_P_P, SCAN_I, 0 }, /* 1080I25 */
    {  0, 40,  0,  0, 148500,  528,  44, 148,  2,  5,  15, 1920, 1080, FRATE_100HZ, HV_POL_P_P, SCAN_I, 0 }, /* 1080I50 */
    {  0, 32, 72,  0,  74250,  638,  44, 148,  4,  5,  36, 1920, 1080,  FRATE_24HZ, HV_POL_P_P, SCAN_P, 0 }, /* 1080P24 */
    {  0, 33, 73,  0,  74250,  528,  44, 148,  4,  5,  36, 1920, 1080,  FRATE_25HZ, HV_POL_P_P, SCAN_P, 0 }, /* 1080P25 */
    {  0, 34, 74,  0,  74250,   88,  44, 148,  4,  5,  36, 1920, 1080,  FRATE_30HZ, HV_POL_P_P, SCAN_P, 0 }, /* 1080P30 */
    {  0, 31, 75,  0, 148500,  528,  44, 148,  4,  5,  36, 1920, 1080,  FRATE_50HZ, HV_POL_P_P, SCAN_P, 0 }, /* 1080P50 */
    {  0, 16, 76,  0, 148500,   88,  44, 148,  4,  5,  36, 1920, 1080,  FRATE_60HZ, HV_POL_P_P, SCAN_P, 0 }, /* 1080P60 */
    {  0, 64, 77,  0, 297000,  528,  44, 148,  4,  5,  36, 1920, 1080, FRATE_100HZ, HV_POL_P_P, SCAN_P, 0 }, /* 1080P100 */
    {  0, 63, 78,  0, 297000,   88,  44, 148,  4,  5,  36, 1920, 1080, FRATE_120HZ, HV_POL_P_P, SCAN_P, 0 }, /* 1080P120 */
    {  0,  0, 86,  0,  99000,  998,  44, 148,  4,  5,  11, 2560, 1080,  FRATE_24HZ, HV_POL_P_P, SCAN_P, 0 }, /* 1080P24 (2560) */
    {  0,  0, 87,  0,  90000,  448,  44, 148,  4,  5,  36, 2560, 1080,  FRATE_25HZ, HV_POL_P_P, SCAN_P, 0 }, /* 1080P25 (2560) */
    {  0,  0, 88,  0, 118800,  768,  44, 148,  4,  5,  36, 2560, 1080,  FRATE_30HZ, HV_POL_P_P, SCAN_P, 0 }, /* 1080P30 (2560) */
    {  0,  0, 89,  0, 185625,  548,  44, 148,  4,  5,  36, 2560, 1080,  FRATE_50HZ, HV_POL_P_P, SCAN_P, 0 }, /* 1080P50 (2560) */
    {  0,  0, 90,  0, 198000,  248,  44, 148,  4,  5,  11, 2560, 1080,  FRATE_60HZ, HV_POL_P_P, SCAN_P, 0 }, /* 1080P60 (2560) */
    {  0,  0, 91,  0, 371250,  218,  44, 148,  4,  5, 161, 2560, 1080, FRATE_100HZ, HV_POL_P_P, SCAN_P, 0 }, /* 1080P100 (2560) */
    {  0,  0, 92,  0, 495000,  548,  44, 148,  4,  5, 161, 2560, 1080, FRATE_120HZ, HV_POL_P_P, SCAN_P, 0 }, /* 1080P120 (2560) */
    {  0, 93, 103,  0, 297000, 1276,  88, 296,  8, 10,  72, 3840, 2160,  FRATE_24HZ, HV_POL_P_P, SCAN_P, 0 }, /* 2160P24 */
    {  0, 94, 104,  0, 297000, 1056,  88, 296,  8, 10,  72, 3840, 2160,  FRATE_25HZ, HV_POL_P_P, SCAN_P, 0 }, /* 2160P25 */
    {  0, 95, 105,  0, 297000,  176,  88, 296,  8, 10,  72, 3840, 2160,  FRATE_30HZ, HV_POL_P_P, SCAN_P, 0 }, /* 2160P30 */
    {  0, 96, 106,  0, 594000, 1056,  88, 296,  8, 10,  72, 3840, 2160,  FRATE_50HZ, HV_POL_P_P, SCAN_P, 0 }, /* 2160P50 */
    {  0, 97, 107,  0, 594000,  176,  88, 296,  8, 10,  72, 3840, 2160,  FRATE_60HZ, HV_POL_P_P, SCAN_P, 0 }, /* 2160P60 */
    {  0,  0,  0, 98, 297000, 1020,  88, 296,  8, 10,  72, 4096, 2160,  FRATE_24HZ, HV_POL_P_P, SCAN_P, 0 }, /* 2160P24 (4096) */
    {  0,  0,  0, 99, 297000,  968,  88, 128,  8, 10,  72, 4096, 2160,  FRATE_25HZ, HV_POL_P_P, SCAN_P, 0 }, /* 2160P25 (4096) */
    {  0,  0,  0, 100, 297000,   88,  88, 128,  8, 10,  72, 4096, 2160,  FRATE_30HZ, HV_POL_P_P, SCAN_P, 0 }, /* 2160P30 (4096) */
    {  0,  0,  0, 101, 594000,  968,  88, 128,  8, 10,  72, 4096, 2160,  FRATE_50HZ, HV_POL_P_P, SCAN_P, 0 }, /* 2160P50 (4096) */
    {  0,  0,  0, 102, 594000,   88,  88, 128,  8, 10,  72, 4096, 2160,  FRATE_60HZ, HV_POL_P_P, SCAN_P, 0 }, /* 2160P60 (4096) */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    {  0, 196, 204, 0, 1188000, 552, 176, 592, 16, 20,  44, 7680, 4320,  FRATE_30HZ, HV_POL_P_P, SCAN_P, 0 }, /* 4320P30 */
#endif
};

/**
 *  AmbaRTSL_HdmiDtdCopy - Copy a detailed timing descriptor
 *  @param[out] pDstDtd pointer to store one DTD
 *  @param[in] pSrcDtd pointer to load one DTD
 *  @return error code
 */
UINT32 AmbaRTSL_HdmiDtdCopy(AMBA_HDMI_VIDEO_TIMING_s *pDstDtd, const AMBA_HDMI_VIDEO_TIMING_s *pSrcDtd)
{
    const UINT32 *pSrc;
    UINT32 *pDst, Size;
    UINT32 RetVal = HDMI_ERR_NONE;

    if ((pDstDtd == NULL) || (pSrcDtd == NULL)) {
        RetVal = HDMI_ERR_ARG;
    } else {
        pDstDtd->PixelClock = pSrcDtd->PixelClock;
#ifdef CONFIG_LINUX
        OSAL_memcpy(&pDst, &pDstDtd, sizeof(pDstDtd));
        OSAL_memcpy(&pSrc, &pSrcDtd, sizeof(pSrcDtd));
#else
        AmbaMisra_TypeCast(&pDst, &pDstDtd);
        AmbaMisra_TypeCast(&pSrc, &pSrcDtd);
#endif
        Size = (UINT32)sizeof(AMBA_HDMI_VIDEO_TIMING_s) & ~0x3U;
        while (Size != 0U) {
            *pDst = *pSrc;
            pDst++;
            pSrc++;
            Size -= (UINT32)sizeof(UINT32);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_HdmiVicTag - Tag a suitable VIC to the input video timing
 *  @param[in] pDTD pointer to a DTD
 *  @return error code
 */
UINT32 AmbaRTSL_HdmiVicTag(AMBA_HDMI_VIDEO_TIMING_s *pDtd)
{
    UINT32 i, RetVal = HDMI_ERR_NA;

    if (pDtd == NULL) {
        RetVal = HDMI_ERR_ARG;
    } else {
        pDtd->Vic256_135 = 0U;
        pDtd->Vic64_27 = 0U;
        pDtd->Vic16_9 = 0U;
        pDtd->Vic4_3 = 0U;

        for (i = 0U; i < NUM_CEA861_DTD; i ++) {
            if ((AmbaHdmiVicDtd[i].PixelClock == pDtd->PixelClock) &&
                (AmbaHdmiVicDtd[i].ActiveLines == pDtd->ActiveLines) &&
                (AmbaHdmiVicDtd[i].ActivePixels == pDtd->ActivePixels) &&
                (AmbaHdmiVicDtd[i].HsyncFrontPorch == pDtd->HsyncFrontPorch) &&
                (AmbaHdmiVicDtd[i].HsyncPulseWidth == pDtd->HsyncPulseWidth) &&
                (AmbaHdmiVicDtd[i].HsyncBackPorch == pDtd->HsyncBackPorch) &&
                (AmbaHdmiVicDtd[i].VsyncFrontPorch == pDtd->VsyncFrontPorch) &&
                (AmbaHdmiVicDtd[i].VsyncPulseWidth == pDtd->VsyncPulseWidth) &&
                (AmbaHdmiVicDtd[i].VsyncBackPorch == pDtd->VsyncBackPorch)) {
                pDtd->Vic256_135 = AmbaHdmiVicDtd[i].Vic256_135;
                pDtd->Vic64_27 = AmbaHdmiVicDtd[i].Vic64_27;
                pDtd->Vic16_9 = AmbaHdmiVicDtd[i].Vic16_9;
                pDtd->Vic4_3 = AmbaHdmiVicDtd[i].Vic4_3;
                pDtd->FieldRate = AmbaHdmiVicDtd[i].FieldRate;
                RetVal = HDMI_ERR_NONE;
                break;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_HdmiVicGet - Get video timings by input VIC
 *  @param[in] VideoIdCode Video id code
 *  @param[out] pDTD pointer to a DTD
 *  @return error code
 */
UINT32 AmbaRTSL_HdmiVicGet(UINT32 VideoID, AMBA_HDMI_VIDEO_TIMING_s *pDTD)
{
    UINT32 i, RetVal = HDMI_ERR_NA;

    if ((VideoID == HDMI_VIC_CUSTOM) || (VideoID > MAX_CEA861_VIC) || (pDTD == NULL)) {
        RetVal = HDMI_ERR_ARG;
    } else {
        for (i = 0U; i < NUM_CEA861_DTD; i ++) {
            if ((AmbaHdmiVicDtd[i].Vic4_3 == VideoID) ||
                (AmbaHdmiVicDtd[i].Vic16_9 == VideoID) ||
                (AmbaHdmiVicDtd[i].Vic64_27 == VideoID) ||
                (AmbaHdmiVicDtd[i].Vic256_135 == VideoID)) {
                RetVal = AmbaRTSL_HdmiDtdCopy(pDTD, &AmbaHdmiVicDtd[i]);
                break;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_HdmiConfigVideo - Set HDMI video output format
 *  @param[in] pVideoTiming pointer to a DTD
 *  @return error code
 */
UINT32 AmbaRTSL_HdmiConfigVideo(const AMBA_HDMI_VIDEO_TIMING_s *pVideoTiming)
{
    UINT32 RetVal = HDMI_ERR_NONE;

    switch (pVideoTiming->SyncPolarity) {
    case AMBA_VIDEO_SYNC_H_POS_V_POS:
        AmbaCSL_HdmiSetHsyncActiveHigh();
        AmbaCSL_HdmiSetVsyncActiveHigh();
        break;
    case AMBA_VIDEO_SYNC_H_POS_V_NEG:
        AmbaCSL_HdmiSetHsyncActiveHigh();
        AmbaCSL_HdmiSetVsyncActiveLow();
        break;
    case AMBA_VIDEO_SYNC_H_NEG_V_POS:
        AmbaCSL_HdmiSetHsyncActiveLow();
        AmbaCSL_HdmiSetVsyncActiveHigh();
        break;
    case AMBA_VIDEO_SYNC_H_NEG_V_NEG:
        AmbaCSL_HdmiSetHsyncActiveLow();
        AmbaCSL_HdmiSetVsyncActiveLow();
        break;
    default:
        RetVal = HDMI_ERR_ARG;
        break;
    }

    if (RetVal == HDMI_ERR_NONE) {
        AmbaCSL_HdmiSetVideoScanFormat(pVideoTiming->ScanFormat);

        /* Active region */
        AmbaCSL_HdmiSetVideoActivePixels(pVideoTiming->ActivePixels);

        if (pVideoTiming->ScanFormat == AMBA_VIDEO_SCAN_INTERLACED) {
            AmbaCSL_HdmiSetVideoActiveLines(pVideoTiming->ActiveLines >> 1U);
        } else {
            AmbaCSL_HdmiSetVideoActiveLines(pVideoTiming->ActiveLines);
        }

        /* Horizontal blanking */
        AmbaCSL_HdmiSetVideoHSyncFrontPorch(pVideoTiming->HsyncFrontPorch);
        AmbaCSL_HdmiSetVideoHSyncPulseWidth(pVideoTiming->HsyncPulseWidth);
        AmbaCSL_HdmiSetVideoHSyncBackPorch(pVideoTiming->HsyncBackPorch);

        /* Vertical blanking */
        AmbaCSL_HdmiSetVideoVSyncFrontPorch(pVideoTiming->VsyncFrontPorch);
        AmbaCSL_HdmiSetVideoVSyncPulseWidth(pVideoTiming->VsyncPulseWidth);
        AmbaCSL_HdmiSetVideoVSyncBackPorch(pVideoTiming->VsyncBackPorch);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_HdmiSetVideoDataSource - Set HDMI video data source mode.
 *  @param[in] SourceMode Optional video data source mode selection
 *                        0x0 = Normal mode. Using VOUT sync signals and video data.
 *                        0x1 = Debug mode using internal generated sync signals and video data.
 *                        0x2 = Debug mode using VOUT sync signals and internal videodata.
 *                        0x3 = Reserved.
 */
void AmbaRTSL_HdmiSetVideoDataSource(UINT8 SourceMode)
{
    AmbaCSL_HdmiSetVideoDataSource(SourceMode);
}
