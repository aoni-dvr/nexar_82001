/**
 *  @file AmbaHDMI_Def.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Common Definitions & Constants for HDMI APIs
 *
 */

#ifndef AMBA_HDMI_DEF_H
#define AMBA_HDMI_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#define HDMI_ERR_0000           (HDMI_ERR_BASE)             /* Invalid argument */
#define HDMI_ERR_0001           (HDMI_ERR_BASE + 0x1U)      /* Unable to do concurrency protection */
#define HDMI_ERR_0002           (HDMI_ERR_BASE + 0x2U)      /* Controller is busy */
#define HDMI_ERR_0003           (HDMI_ERR_BASE + 0x3U)      /* No device is attached  */
#define HDMI_ERR_0004           (HDMI_ERR_BASE + 0x4U)      /* Unsupported feature */
#define HDMI_ERR_0005           (HDMI_ERR_BASE + 0x5U)      /* Maximum limit reached */
#define HDMI_ERR_0006           (HDMI_ERR_BASE + 0x6U)      /* The specified DI packet is unavailable */
#define HDMI_ERR_0007           (HDMI_ERR_BASE + 0x7U)      /* Unable to get physical address */
#define HDMI_ERR_0008           (HDMI_ERR_BASE + 0x8U)      /* Unavailable to use CEC function */
#define HDMI_ERR_0009           (HDMI_ERR_BASE + 0x9U)      /* Invalid EDID */
#define HDMI_ERR_000A           (HDMI_ERR_BASE + 0xAU)      /* DDC communication error */
#define HDMI_ERR_000B           (HDMI_ERR_BASE + 0xBU)      /* SCDC communication error */
#define HDMI_ERR_000C           (HDMI_ERR_BASE + 0xCU)      /* Unavailable to use CEC function */
#define HDMI_ERR_000D           (HDMI_ERR_BASE + 0xDU)      /* Invalid EDID */
#define HDMI_ERR_00FF           (HDMI_ERR_BASE + 0XFFU)     /* Unexpected error */

/* HDMI error values */
#define HDMI_ERR_NONE           0U
#define HDMI_ERR_ARG            HDMI_ERR_0000
#define HDMI_ERR_MUTEX          HDMI_ERR_0001
#define HDMI_ERR_BUSY           HDMI_ERR_0002
#define HDMI_ERR_NO_DEV         HDMI_ERR_0003
#define HDMI_ERR_NA             HDMI_ERR_0004
#define HDMI_ERR_LIMIT_REACHED  HDMI_ERR_0005
#define HDMI_ERR_PKT_UNAVIL     HDMI_ERR_0006
#define HDMI_ERR_NO_PHYS_ADDR   HDMI_ERR_0007
#define HDMI_ERR_CEC            HDMI_ERR_0008
#define HDMI_ERR_EDID           HDMI_ERR_0009
#define HDMI_ERR_DDC            HDMI_ERR_000A
#define HDMI_ERR_SCDC           HDMI_ERR_000B
#define HDMI_ERR_DEV_CTL        HDMI_ERR_000C    /* error reported by QNX devctl() */
#define HDMI_ERR_OPEN_FILE      HDMI_ERR_000D    /* error reported by QNX open() */

#define HDMI_ERR_UNEXPECTED     HDMI_ERR_00FF

#define HDMI_TX_PORT0           (0U)
#define NUM_HDMI_TX_PORT        (1U)

#define HDMI_CEC_MSG_MAX_SIZE   (16U)       /* Total number of header and data blocks */

#define HDMI_CSC_DATA_SIZE      (18U)

/*
 * Definitions for Hot-Plug Detection State
 */
#define HDMI_HPD_NONE           (0U)        /* No cable is plugged in */
#define HDMI_HPD_ACTIVE         (1U)        /* HDMI/DVI cable is plugged in and in active state */
#define HDMI_HPD_STANDBY        (2U)        /* HDMI/DVI cable is plugged in and in standby state */

#define HDMI_EDID_UNAVAIL       (0U)        /* Unable to get valid EDID data */
#define HDMI_EDID_DVI           (1U)        /* Found EDID data but HDMI VSDB is not found */
#define HDMI_EDID_HDMI          (2U)        /* Found EDID data and HDMI VSDB is found */

/*
 * Definition of video ID code defined in CEA-861 standard
 */
/* Code Short Name                          ID           Aspect Ratio              HxV @ Frate */
#define HDMI_VIC_CUSTOM                     0U      /*   N/A */
#define HDMI_VIC_DMT0659                    1U      /*   4:3             640x480p @ 59.94/60Hz */
#define HDMI_VIC_480P                       2U      /*   4:3             720x480p @ 59.94/60Hz */
#define HDMI_VIC_480P_WIDE                  3U      /*  16:9             720x480p @ 59.94/60Hz */
#define HDMI_VIC_720P                       4U      /*  16:9            1280x720p @ 59.94/60Hz */
#define HDMI_VIC_1080I                      5U      /*  16:9           1920x1080i @ 59.94/60Hz */
#define HDMI_VIC_480I                       6U      /*   4:3       720(1440)x480i @ 59.94/60Hz */
#define HDMI_VIC_480I_WIDE                  7U      /*  16:9       720(1440)x480i @ 59.94/60Hz */
#define HDMI_VIC_240P                       8U      /*   4:3       720(1440)x240p @ 59.94/60Hz */
#define HDMI_VIC_240P_WIDE                  9U      /*  16:9       720(1440)x240p @ 59.94/60Hz */
#define HDMI_VIC_480I4X                     10U     /*   4:3          (2880)x480i @ 59.94/60Hz */
#define HDMI_VIC_480I4X_WIDE                11U     /*  16:9          (2880)x480i @ 59.94/60Hz */
#define HDMI_VIC_240P4X                     12U     /*   4:3          (2880)x240p @ 59.94/60Hz */
#define HDMI_VIC_240P4X_WIDE                13U     /*  16:9          (2880)x240p @ 59.94/60Hz */
#define HDMI_VIC_480P2X                     14U     /*   4:3            1440x480p @ 59.94/60Hz */
#define HDMI_VIC_480P2X_WIDE                15U     /*  16:9            1440x480p @ 59.94/60Hz */
#define HDMI_VIC_1080P                      16U     /*  16:9           1920x1080p @ 59.94/60Hz */
#define HDMI_VIC_576P                       17U     /*   4:3             720x576p @ 50Hz */
#define HDMI_VIC_576P_WIDE                  18U     /*  16:9             720x576p @ 50Hz */
#define HDMI_VIC_720P50                     19U     /*  16:9            1280x720p @ 50Hz */
#define HDMI_VIC_1080I25                    20U     /*  16:9           1920x1080i @ 50Hz (V-Total 1125) */
#define HDMI_VIC_576I                       21U     /*   4:3       720(1440)x576i @ 50Hz */
#define HDMI_VIC_576I_WIDE                  22U     /*  16:9       720(1440)x576i @ 50Hz */
#define HDMI_VIC_288P                       23U     /*   4:3       720(1440)x288p @ 50Hz */
#define HDMI_VIC_288P_WIDE                  24U     /*  16:9       720(1440)x288p @ 50Hz */
#define HDMI_VIC_576I4X                     25U     /*   4:3          (2880)x576i @ 50Hz */
#define HDMI_VIC_576I4X_WIDE                26U     /*  16:9          (2880)x576i @ 50Hz */
#define HDMI_VIC_288P4X                     27U     /*   4:3          (2880)x288p @ 50Hz */
#define HDMI_VIC_288P4X_WIDE                28U     /*  16:9          (2880)x288p @ 50Hz */
#define HDMI_VIC_576P2X                     29U     /*   4:3            1440x576p @ 50Hz */
#define HDMI_VIC_576P2X_WIDE                30U     /*  16:9            1440x576p @ 50Hz */
#define HDMI_VIC_1080P50                    31U     /*  16:9           1920x1080p @ 50Hz */
#define HDMI_VIC_1080P24                    32U     /*  16:9           1920x1080p @ 23.98/24Hz */
#define HDMI_VIC_1080P25                    33U     /*  16:9           1920x1080p @ 25Hz */
#define HDMI_VIC_1080P30                    34U     /*  16:9           1920x1080p @ 29.97/30Hz */
#define HDMI_VIC_480P4X                     35U     /*   4:3          (2880)x480p @ 59.94/60Hz */
#define HDMI_VIC_480P4X_WIDE                36U     /*  16:9          (2880)x480p @ 59.94/60Hz */
#define HDMI_VIC_576P4X                     37U     /*   4:3          (2880)x576p @ 50Hz */
#define HDMI_VIC_576P4X_WIDE                38U     /*  16:9          (2880)x576p @ 50Hz */
#define HDMI_VIC_1080I25_AS                 39U     /*  16:9           1920x1080i @ 50Hz (V-Total 1250) */
#define HDMI_VIC_1080I50                    40U     /*  16:9           1920x1080i @ 100Hz */
#define HDMI_VIC_720P100                    41U     /*  16:9            1280x720p @ 100Hz */
#define HDMI_VIC_576P100                    42U     /*   4:3             720x576p @ 100Hz */
#define HDMI_VIC_576P100_WIDE               43U     /*  16:9             720x576p @ 100Hz */
#define HDMI_VIC_576I50                     44U     /*   4:3       720(1440)x576i @ 100Hz */
#define HDMI_VIC_576I50_WIDE                45U     /*  16:9       720(1440)x576i @ 100Hz */
#define HDMI_VIC_1080I60                    46U     /*  16:9           1920x1080i @ 119.88/120Hz */
#define HDMI_VIC_720P120                    47U     /*  16:9            1280x720p @ 119.88/120Hz */
#define HDMI_VIC_480P119                    48U     /*   4:3             720x480p @ 119.88/120Hz */
#define HDMI_VIC_480P119_WIDE               49U     /*  16:9             720x480p @ 119.88/120Hz */
#define HDMI_VIC_480I59                     50U     /*   4:3       720(1440)x480i @ 119.88/120Hz */
#define HDMI_VIC_480I59_WIDE                51U     /*  16:9       720(1440)x480i @ 119.88/120Hz */
#define HDMI_VIC_576P200                    52U     /*   4:3             720x576p @ 200Hz */
#define HDMI_VIC_576P200_WIDE               53U     /*  16:9             720x576p @ 200Hz */
#define HDMI_VIC_576I100                    54U     /*   4:3       720(1440)x576i @ 200Hz */
#define HDMI_VIC_576I100_WIDE               55U     /*  16:9       720(1440)x576i @ 200Hz */
#define HDMI_VIC_480P239                    56U     /*   4:3             720x480p @ 239.76/240Hz */
#define HDMI_VIC_480P239_WIDE               57U     /*  16:9             720x480p @ 239.76/240Hz */
#define HDMI_VIC_480I119                    58U     /*   4:3       720(1440)x480i @ 239.76/240Hz */
#define HDMI_VIC_480I119_WIDE               59U     /*  16:9       720(1440)x480i @ 239.76/240Hz */
#define HDMI_VIC_720P24                     60U     /*  16:9            1280x720p @ 23.98/24Hz */
#define HDMI_VIC_720P25                     61U     /*  16:9            1280x720p @ 25Hz */
#define HDMI_VIC_720P30                     62U     /*  16:9            1280x720p @ 29.97/30Hz */
#define HDMI_VIC_1080P120                   63U     /*  16:9           1920x1080p @ 119.88/120Hz */
#define HDMI_VIC_1080P100                   64U     /*  16:9           1920x1080p @ 100Hz */

#define HDMI_VIC_720P24_WIDE                65U     /*  64:27           1280x720p @ 23.98/24Hz */
#define HDMI_VIC_720P25_WIDE                66U     /*  64:27           1280x720p @ 25Hz */
#define HDMI_VIC_720P30_WIDE                67U     /*  64:27           1280x720p @ 29.97/30Hz */
#define HDMI_VIC_720P50_WIDE                68U     /*  64:27           1280x720p @ 50Hz */
#define HDMI_VIC_720P60_WIDE                69U     /*  64:27           1280x720p @ 59.94/60Hz */
#define HDMI_VIC_720P100_WIDE               70U     /*  64:27           1280x720p @ 100Hz */
#define HDMI_VIC_720P120_WIDE               71U     /*  64:27           1280x720p @ 119.88/120Hz */
#define HDMI_VIC_1080P24_WIDE               72U     /*  64:27          1920x1080p @ 23.98/24Hz */
#define HDMI_VIC_1080P25_WIDE               73U     /*  64:27          1920x1080p @ 25Hz */
#define HDMI_VIC_1080P30_WIDE               74U     /*  64:27          1920x1080p @ 29.97/30Hz */
#define HDMI_VIC_1080P50_WIDE               75U     /*  64:27          1920x1080p @ 50Hz */
#define HDMI_VIC_1080P60_WIDE               76U     /*  64:27          1920x1080p @ 59.94/60Hz */
#define HDMI_VIC_1080P100_WIDE              77U     /*  64:27          1920x1080p @ 100Hz */
#define HDMI_VIC_1080P120_WIDE              78U     /*  64:27          1920x1080p @ 119.88/120Hz */
#define HDMI_VIC_720P24_ULTRAWIDE           79U     /*  64:27           1680x720p @ 23.98/24Hz */
#define HDMI_VIC_720P25_ULTRAWIDE           80U     /*  64:27           1680x720p @ 25Hz */
#define HDMI_VIC_720P30_ULTRAWIDE           81U     /*  64:27           1680x720p @ 29.97/30Hz */
#define HDMI_VIC_720P50_ULTRAWIDE           82U     /*  64:27           1680x720p @ 50Hz */
#define HDMI_VIC_720P60_ULTRAWIDE           83U     /*  64:27           1680x720p @ 59.94/60Hz */
#define HDMI_VIC_720P100_ULTRAWIDE          84U     /*  64:27           1680x720p @ 100Hz */
#define HDMI_VIC_720P120_ULTRAWIDE          85U     /*  64:27           1680x720p @ 119.88/120Hz */
#define HDMI_VIC_1080P24_ULTRAWIDE          86U     /*  64:27          2560x1080p @ 23.98/24Hz */
#define HDMI_VIC_1080P25_ULTRAWIDE          87U     /*  64:27          2560x1080p @ 25Hz */
#define HDMI_VIC_1080P30_ULTRAWIDE          88U     /*  64:27          2560x1080p @ 29.97/30Hz */
#define HDMI_VIC_1080P50_ULTRAWIDE          89U     /*  64:27          2560x1080p @ 50Hz */
#define HDMI_VIC_1080P60_ULTRAWIDE          90U     /*  64:27          2560x1080p @ 59.94/60Hz */
#define HDMI_VIC_1080P100_ULTRAWIDE         91U     /*  64:27          2560x1080p @ 100Hz */
#define HDMI_VIC_1080P120_ULTRAWIDE         92U     /*  64:27          2560x1080p @ 119.88/120Hz */
#define HDMI_VIC_2160P24                    93U     /*  16:9           3840x2160p @ 23.98/24Hz */
#define HDMI_VIC_2160P25                    94U     /*  16:9           3840x2160p @ 25Hz */
#define HDMI_VIC_2160P30                    95U     /*  16:9           3840x2160p @ 29.97/30Hz */
#define HDMI_VIC_2160P50                    96U     /*  16:9           3840x2160p @ 50Hz */
#define HDMI_VIC_2160P60                    97U     /*  16:9           3840x2160p @ 59.94/60Hz */
#define HDMI_VIC_2160P24_ULTRAWIDE          98U     /*  256:135        4096x2160p @ 23.98/24Hz */
#define HDMI_VIC_2160P25_ULTRAWIDE          99U     /*  256:135        4096x2160p @ 25Hz */
#define HDMI_VIC_2160P30_ULTRAWIDE          100U    /*  256:135        4096x2160p @ 29.97/30Hz */
#define HDMI_VIC_2160P50_ULTRAWIDE          101U    /*  256:135        4096x2160p @ 50Hz */
#define HDMI_VIC_2160P60_ULTRAWIDE          102U    /*  256:135        4096x2160p @ 59.94/60Hz */
#define HDMI_VIC_2160P24_WIDE               103U    /*  64:27          3840x2160p @ 23.98/24Hz */
#define HDMI_VIC_2160P25_WIDE               104U    /*  64:27          3840x2160p @ 25Hz */
#define HDMI_VIC_2160P30_WIDE               105U    /*  64:27          3840x2160p @ 29.97/30Hz */
#define HDMI_VIC_2160P50_WIDE               106U    /*  64:27          3840x2160p @ 50Hz */
#define HDMI_VIC_2160P60_WIDE               107U    /*  64:27          3840x2160p @ 59.94/60Hz */
/* 0, 108 - 127 Reserved by CEA-861-F */
/* Assume one base block (up to 4 DTDs) and one CEA extension (up to 6 DTDs) */
#define HDMI_VIC_DTD0                       118U    /* 1st detailed timing. preferred timing. */
#define HDMI_VIC_DTD1                       119U    /* 2nd detailed timing. */
#define HDMI_VIC_DTD2                       120U    /* 3rd detailed timing. */
#define HDMI_VIC_DTD3                       121U    /* 4th detailed timing. */
#define HDMI_VIC_DTD4                       122U    /* 5th detailed timing. */
#define HDMI_VIC_DTD5                       123U    /* 6th detailed timing. */
#define HDMI_VIC_DTD6                       124U    /* 7th detailed timing. */
#define HDMI_VIC_DTD7                       125U    /* 8th detailed timing. */
#define HDMI_VIC_DTD8                       126U    /* 9th detailed timing. */
#define HDMI_VIC_DTD9                       127U    /* 10th detailed timing. */

#define NUM_HDMI_VIC                        128U    /* Number of pre-defined video ID codes */

#define HDMI_VIC_4320P30                    196U    /*  16:9           7680x4320p @ 29.97/30Hz */

/**
 *  NOTE:
 *  A Video Timing with a vertical frequency that is an integer multiple of 6.00 Hz (i.e., 24.00, 30.00, 60.00, 120.00 or
 *  240.00 Hz) is considered to be the same as a Video Timing with the equivalent detailed timing information but where
 *  the vertical frequency is adjusted by a factor of 1000/1001 (i.e., 24/1.001, 30/1.001, 60/1.001, 120/1.001 or
 *  240/1.001). That is, they are considered two versions of the same Video Timing but with slightly different pixel clock
 *  frequencies. The vertical frequencies of the 240p, 480p, and 480i Video Formats are typically adjusted by a factor of
 *  exactly 1000/1001 for NTSC video compatibility, while the 576p, 576i, and the HDTV Video Formats are not.
 *
 *  To avoid fractional frame rate conversions in Source and Sinks, Sources should use the exact vertical frequencies
 *  of 25.000 Hz, 50.000 Hz, 100.000 Hz, 120.000 Hz, 200.000 Hz, and 240.000 Hz at 25 Hz, 50 Hz, 100 Hz, 120 Hz,
 *  200 Hz, and 240 Hz, respectively. Likewise, Sources should use the exact vertical frequencies of (24 * 1000) / 1001
 *  Hz, (30 * 1000) / 1001 Hz, (60 * 1000) / 1001 Hz, (120 * 1000) / 1001 Hz, and (240 * 1000) / 1001 Hz at 23.98 Hz,
 *  29.97 Hz, 59.94 Hz, 119.88 Hz, 239.76 Hz, respectively.
 */
#define HDMI_FRAME_RATE_NORMAL              0x00U   /* May adjust frame rate by a factor of 1000/1001 for NTSC video compatibility */
#define HDMI_FRAME_RATE_STRICT              0x80U   /* Never adjust frame rate */
static inline UINT8 HDMI_GET_RAW_VIC(UINT8 VIC)
{
    UINT8 RetVal;
    if ((VIC == 196U) || (VIC == 204U)) {
        RetVal = VIC;
    } else {
        RetVal = ((VIC) & ~(UINT8)HDMI_FRAME_RATE_STRICT);
    }
    return RetVal;
}

#define HDMI_VIC_CUSTOM_NORMAL                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_CUSTOM)                 /*   N/A */
#define HDMI_VIC_CUSTOM_STRICT                    ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_CUSTOM)                 /*   N/A */
#define HDMI_VIC_640_480_60P                      ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_DMT0659)                /*   4:3             640x480p @ 59.94Hz */
#define HDMI_VIC_640_480_A60P                     ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_DMT0659)                /*   4:3             640x480p @ 60Hz */
#define HDMI_VIC_720_480_60P                      ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480P)                   /*   4:3             720x480p @ 59.94Hz */
#define HDMI_VIC_720_480_A60P                     ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480P)                   /*   4:3             720x480p @ 60Hz */
#define HDMI_VIC_720_480_60P_WIDE                 ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480P_WIDE)              /*  16:9             720x480p @ 59.94Hz */
#define HDMI_VIC_720_480_A60P_WIDE                ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480P_WIDE)              /*  16:9             720x480p @ 60Hz */
#define HDMI_VIC_1280_720_60P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P)                   /*  16:9            1280x720p @ 59.94Hz */
#define HDMI_VIC_1280_720_A60P                    ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_720P)                   /*  16:9            1280x720p @ 60Hz */
#define HDMI_VIC_1920_1080_60I                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080I)                  /*  16:9           1920x1080i @ 59.94Hz */
#define HDMI_VIC_1920_1080_A60I                   ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_1080I)                  /*  16:9           1920x1080i @ 60Hz */
#define HDMI_VIC_1440_480_60I                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480I)                   /*   4:3       720(1440)x480i @ 59.94Hz */
#define HDMI_VIC_1440_480_A60I                    ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480I)                   /*   4:3       720(1440)x480i @ 60Hz */
#define HDMI_VIC_1440_480_60I_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480I_WIDE)              /*  16:9       720(1440)x480i @ 59.94Hz */
#define HDMI_VIC_1440_480_A60I_WIDE               ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480I_WIDE)              /*  16:9       720(1440)x480i @ 60Hz */
#define HDMI_VIC_1440_240_60P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_240P)                   /*   4:3       720(1440)x240p @ 59.94Hz */
#define HDMI_VIC_1440_240_A60P                    ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_240P)                   /*   4:3       720(1440)x240p @ 60Hz */
#define HDMI_VIC_1440_240_60P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_240P_WIDE)              /*  16:9       720(1440)x240p @ 59.94Hz */
#define HDMI_VIC_1440_240_A60P_WIDE               ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_240P_WIDE)              /*  16:9       720(1440)x240p @ 60Hz */
#define HDMI_VIC_2880_480_60I                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480I4X)                 /*   4:3          (2880)x480i @ 59.94Hz */
#define HDMI_VIC_2880_480_A60I                    ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480I4X)                 /*   4:3          (2880)x480i @ 60Hz */
#define HDMI_VIC_2880_480_60I_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480I4X_WIDE)            /*  16:9          (2880)x480i @ 59.94Hz */
#define HDMI_VIC_2880_480_A60I_WIDE               ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480I4X_WIDE)            /*  16:9          (2880)x480i @ 60Hz */
#define HDMI_VIC_2880_240_60P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_240P4X)                 /*   4:3          (2880)x240p @ 59.94Hz */
#define HDMI_VIC_2880_240_A60P                    ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_240P4X)                 /*   4:3          (2880)x240p @ 60Hz */
#define HDMI_VIC_2880_240_60P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_240P4X_WIDE)            /*  16:9          (2880)x240p @ 59.94Hz */
#define HDMI_VIC_2880_240_A60P_WIDE               ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_240P4X_WIDE)            /*  16:9          (2880)x240p @ 60Hz */
#define HDMI_VIC_1440_480_60P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480P2X)                 /*   4:3            1440x480p @ 59.94Hz */
#define HDMI_VIC_1440_480_A60P                    ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480P2X)                 /*   4:3            1440x480p @ 60Hz */
#define HDMI_VIC_1440_480_60P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480P2X_WIDE)            /*  16:9            1440x480p @ 59.94Hz */
#define HDMI_VIC_1440_480_A60P_WIDE               ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480P2X_WIDE)            /*  16:9            1440x480p @ 60Hz */
#define HDMI_VIC_1920_1080_60P                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P)                  /*  16:9           1920x1080p @ 59.94Hz */
#define HDMI_VIC_1920_1080_A60P                   ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_1080P)                  /*  16:9           1920x1080p @ 60Hz */
#define HDMI_VIC_720_576_50P                      ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576P)                   /*   4:3             720x576p @ 50Hz */
#define HDMI_VIC_720_576_50P_WIDE                 ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576P_WIDE)              /*  16:9             720x576p @ 50Hz */
#define HDMI_VIC_1280_720_50P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P50)                 /*  16:9            1280x720p @ 50Hz */
#define HDMI_VIC_1920_1080_50I                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080I25)                /*  16:9           1920x1080i @ 50Hz (V-Total 1125) */
#define HDMI_VIC_1440_576_50I                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576I)                   /*   4:3       720(1440)x576i @ 50Hz */
#define HDMI_VIC_1440_576_50I_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576I_WIDE)              /*  16:9       720(1440)x576i @ 50Hz */
#define HDMI_VIC_1440_288_50P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_288P)                   /*   4:3       720(1440)x288p @ 50Hz */
#define HDMI_VIC_1440_288_50P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_288P_WIDE)              /*  16:9       720(1440)x288p @ 50Hz */
#define HDMI_VIC_2880_576_50I                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576I4X)                 /*   4:3          (2880)x576i @ 50Hz */
#define HDMI_VIC_2880_576_50I_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576I4X_WIDE)            /*  16:9          (2880)x576i @ 50Hz */
#define HDMI_VIC_2880_288_50P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_288P4X)                 /*   4:3          (2880)x288p @ 50Hz */
#define HDMI_VIC_2880_288_50P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_288P4X_WIDE)            /*  16:9          (2880)x288p @ 50Hz */
#define HDMI_VIC_1440_576_50P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576P2X)                 /*   4:3            1440x576p @ 50Hz */
#define HDMI_VIC_1440_576_50P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576P2X_WIDE)            /*  16:9            1440x576p @ 50Hz */
#define HDMI_VIC_1920_1080_50P                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P50)                /*  16:9           1920x1080p @ 50Hz */
#define HDMI_VIC_1920_1080_24P                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P24)                /*  16:9           1920x1080p @ 23.98Hz */
#define HDMI_VIC_1920_1080_A24P                   ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_1080P24)                /*  16:9           1920x1080p @ 24Hz */
#define HDMI_VIC_1920_1080_25P                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P25)                /*  16:9           1920x1080p @ 25Hz */
#define HDMI_VIC_1920_1080_30P                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P30)                /*  16:9           1920x1080p @ 29.97Hz */
#define HDMI_VIC_1920_1080_A30P                   ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_1080P30)                /*  16:9           1920x1080p @ 30Hz */
#define HDMI_VIC_2880_480_60P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480P4X)                 /*   4:3          (2880)x480p @ 59.94Hz */
#define HDMI_VIC_2880_480_A60P                    ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480P4X)                 /*   4:3          (2880)x480p @ 60Hz */
#define HDMI_VIC_2880_480_60P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480P4X_WIDE)            /*  16:9          (2880)x480p @ 59.94Hz */
#define HDMI_VIC_2880_480_A60P_WIDE               ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480P4X_WIDE)            /*  16:9          (2880)x480p @ 60Hz */
#define HDMI_VIC_2880_576_50P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576P4X)                 /*   4:3          (2880)x576p @ 50Hz */
#define HDMI_VIC_2880_576_50P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576P4X_WIDE)            /*  16:9          (2880)x576p @ 50Hz */
#define HDMI_VIC_1920_1080_50I_AS                 ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080I25_AS)             /*  16:9           1920x1080i @ 50Hz (V-Total 1250) */
#define HDMI_VIC_1920_1080_100I                   ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080I50)                /*  16:9           1920x1080i @ 100Hz */
#define HDMI_VIC_1280_720_100P                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P100)                /*  16:9            1280x720p @ 100Hz */
#define HDMI_VIC_720_576_100P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576P100)                /*   4:3             720x576p @ 100Hz */
#define HDMI_VIC_720_576_100P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576P100_WIDE)           /*  16:9             720x576p @ 100Hz */
#define HDMI_VIC_1440_576_100I                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576I50)                 /*   4:3       720(1440)x576i @ 100Hz */
#define HDMI_VIC_1440_576_100I_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576I50_WIDE)            /*  16:9       720(1440)x576i @ 100Hz */
#define HDMI_VIC_1920_1080_120I                   ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080I60)                /*  16:9           1920x1080i @ 119.880Hz */
#define HDMI_VIC_1920_1080_A120I                  ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_1080I60)                /*  16:9           1920x1080i @ 120Hz */
#define HDMI_VIC_1280_720_120P                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P120)                /*  16:9            1280x720p @ 119.88Hz */
#define HDMI_VIC_1280_720_A120P                   ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_720P120)                /*  16:9            1280x720p @ 120Hz */
#define HDMI_VIC_720_480_120P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480P119)                /*   4:3             720x480p @ 119.88Hz */
#define HDMI_VIC_720_480_A120P                    ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480P119)                /*   4:3             720x480p @ 120Hz */
#define HDMI_VIC_720_480_120P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480P119_WIDE)           /*  16:9             720x480p @ 119.88Hz */
#define HDMI_VIC_720_480_A120P_WIDE               ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480P119_WIDE)           /*  16:9             720x480p @ 120Hz */
#define HDMI_VIC_1440_480_120I                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480I59)                 /*   4:3       720(1440)x480i @ 119.88Hz */
#define HDMI_VIC_1440_480_A120I                   ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480I59)                 /*   4:3       720(1440)x480i @ 120Hz */
#define HDMI_VIC_1440_480_120I_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480I59_WIDE)            /*  16:9       720(1440)x480i @ 119.88Hz */
#define HDMI_VIC_1440_480_A120I_WIDE              ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480I59_WIDE)            /*  16:9       720(1440)x480i @ 120Hz */
#define HDMI_VIC_720_576_200P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576P200)                /*   4:3             720x576p @ 200Hz */
#define HDMI_VIC_720_576_200P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576P200_WIDE)           /*  16:9             720x576p @ 200Hz */
#define HDMI_VIC_1440_576_200I                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576I100)                /*   4:3       720(1440)x576i @ 200Hz */
#define HDMI_VIC_1440_576_200I_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_576I100_WIDE)           /*  16:9       720(1440)x576i @ 200Hz */
#define HDMI_VIC_720_480_240P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480P239)                /*   4:3             720x480p @ 239.76Hz */
#define HDMI_VIC_720_480_A240P                    ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480P239)                /*   4:3             720x480p @ 240Hz */
#define HDMI_VIC_720_480_240P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480P239_WIDE)           /*  16:9             720x480p @ 239.76Hz */
#define HDMI_VIC_720_480_A240P_WIDE               ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480P239_WIDE)           /*  16:9             720x480p @ 240Hz */
#define HDMI_VIC_1440_480_240I                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480I119)                /*   4:3       720(1440)x480i @ 239.76Hz */
#define HDMI_VIC_1440_480_A240I                   ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480I119)                /*   4:3       720(1440)x480i @ 240Hz */
#define HDMI_VIC_1440_480_240I_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_480I119_WIDE)           /*  16:9       720(1440)x480i @ 239.76Hz */
#define HDMI_VIC_1440_480_A240I_WIDE              ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_480I119_WIDE)           /*  16:9       720(1440)x480i @ 240Hz */
#define HDMI_VIC_1280_720_24P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P24)                 /*  16:9            1280x720p @ 23.98Hz */
#define HDMI_VIC_1280_720_A24P                    ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_720P24)                 /*  16:9            1280x720p @ 24Hz */
#define HDMI_VIC_1280_720_25P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P25)                 /*  16:9            1280x720p @ 25Hz */
#define HDMI_VIC_1280_720_30P                     ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P30)                 /*  16:9            1280x720p @ 29.97Hz */
#define HDMI_VIC_1280_720_A30P                    ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_720P30)                 /*  16:9            1280x720p @ 30Hz */
#define HDMI_VIC_1920_1080_120P                   ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P120)               /*  16:9           1920x1080p @ 119.88Hz */
#define HDMI_VIC_1920_1080_A120P                  ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_1080P120)               /*  16:9           1920x1080p @ 120Hz */
#define HDMI_VIC_1920_1080_100P                   ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P100)               /*  16:9           1920x1080p @ 100Hz */
#define HDMI_VIC_1280_720_24P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P24_WIDE)            /*  64:27           1280x720p @ 23.98Hz */
#define HDMI_VIC_1280_720_A24P_WIDE               ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_720P24_WIDE)            /*  64:27           1280x720p @ 24Hz */
#define HDMI_VIC_1280_720_25P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P25_WIDE)            /*  64:27           1280x720p @ 25Hz */
#define HDMI_VIC_1280_720_30P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P30_WIDE)            /*  64:27           1280x720p @ 29.97Hz */
#define HDMI_VIC_1280_720_A30P_WIDE               ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_720P30_WIDE)            /*  64:27           1280x720p @ 30Hz */
#define HDMI_VIC_1280_720_50P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P50_WIDE)            /*  64:27           1280x720p @ 50Hz */
#define HDMI_VIC_1280_720_60P_WIDE                ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P60_WIDE)            /*  64:27           1280x720p @ 59.94Hz */
#define HDMI_VIC_1280_720_A60P_WIDE               ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_720P60_WIDE)            /*  64:27           1280x720p @ 60Hz */
#define HDMI_VIC_1280_720_100P_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P100_WIDE)           /*  64:27           1280x720p @ 100Hz */
#define HDMI_VIC_1280_720_120P_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P120_WIDE)           /*  64:27           1280x720p @ 119.88Hz */
#define HDMI_VIC_1280_720_A120P_WIDE              ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_720P120_WIDE)           /*  64:27           1280x720p @ 120Hz */
#define HDMI_VIC_1920_1080_24P_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P24_WIDE)           /*  64:27          1920x1080p @ 23.98Hz */
#define HDMI_VIC_1920_1080_A24P_WIDE              ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_1080P24_WIDE)           /*  64:27          1920x1080p @ 24Hz */
#define HDMI_VIC_1920_1080_25P_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P25_WIDE)           /*  64:27          1920x1080p @ 25Hz */
#define HDMI_VIC_1920_1080_30P_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P30_WIDE)           /*  64:27          1920x1080p @ 29.97Hz */
#define HDMI_VIC_1920_1080_A30P_WIDE              ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_1080P30_WIDE)           /*  64:27          1920x1080p @ 30Hz */
#define HDMI_VIC_1920_1080_50P_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P50_WIDE)           /*  64:27          1920x1080p @ 50Hz */
#define HDMI_VIC_1920_1080_60P_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P60_WIDE)           /*  64:27          1920x1080p @ 59.94Hz */
#define HDMI_VIC_1920_1080_A60P_WIDE              ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_1080P60_WIDE)           /*  64:27          1920x1080p @ 60Hz */
#define HDMI_VIC_1920_1080_100P_WIDE              ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P100_WIDE)          /*  64:27          1920x1080p @ 100Hz */
#define HDMI_VIC_1920_1080_120P_WIDE              ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P120_WIDE)          /*  64:27          1920x1080p @ 119.88Hz */
#define HDMI_VIC_1920_1080_A120P_WIDE             ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_1080P120_WIDE)          /*  64:27          1920x1080p @ 120Hz */
#define HDMI_VIC_1680_720_24P_ULTRA               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P24_ULTRAWIDE)       /*  64:27           1680x720p @ 23.98Hz */
#define HDMI_VIC_1680_720_A24P_ULTRA              ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_720P24_ULTRAWIDE)       /*  64:27           1680x720p @ 24Hz */
#define HDMI_VIC_1680_720_25P_ULTRA               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P25_ULTRAWIDE)       /*  64:27           1680x720p @ 25Hz */
#define HDMI_VIC_1680_720_30P_ULTRA               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P30_ULTRAWIDE)       /*  64:27           1680x720p @ 29.97Hz */
#define HDMI_VIC_1680_720_A30P_ULTRA              ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_720P30_ULTRAWIDE)       /*  64:27           1680x720p @ 30Hz */
#define HDMI_VIC_1680_720_50P_ULTRA               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P50_ULTRAWIDE)       /*  64:27           1680x720p @ 50Hz */
#define HDMI_VIC_1680_720_60P_ULTRA               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P60_ULTRAWIDE)       /*  64:27           1680x720p @ 59.94Hz */
#define HDMI_VIC_1680_720_A60P_ULTRA              ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_720P60_ULTRAWIDE)       /*  64:27           1680x720p @ 60Hz */
#define HDMI_VIC_1680_720_100P_ULTRA              ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P100_ULTRAWIDE)      /*  64:27           1680x720p @ 100Hz */
#define HDMI_VIC_1680_720_120P_ULTRA              ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_720P120_ULTRAWIDE)      /*  64:27           1680x720p @ 119.88Hz */
#define HDMI_VIC_1680_720_A120P_ULTRA             ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_720P120_ULTRAWIDE)      /*  64:27           1680x720p @ 120Hz */
#define HDMI_VIC_2560_1080_24P_ULTRA              ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P24_ULTRAWIDE)      /*  64:27          2560x1080p @ 23.98Hz */
#define HDMI_VIC_2560_1080_A24P_ULTRA             ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_1080P24_ULTRAWIDE)      /*  64:27          2560x1080p @ 24Hz */
#define HDMI_VIC_2560_1080_25P_ULTRA              ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P25_ULTRAWIDE)      /*  64:27          2560x1080p @ 25Hz */
#define HDMI_VIC_2560_1080_30P_ULTRA              ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P30_ULTRAWIDE)      /*  64:27          2560x1080p @ 29.97Hz */
#define HDMI_VIC_2560_1080_A30P_ULTRA             ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_1080P30_ULTRAWIDE)      /*  64:27          2560x1080p @ 30Hz */
#define HDMI_VIC_2560_1080_50P_ULTRA              ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P50_ULTRAWIDE)      /*  64:27          2560x1080p @ 50Hz */
#define HDMI_VIC_2560_1080_60P_ULTRA              ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P60_ULTRAWIDE)      /*  64:27          2560x1080p @ 59.94Hz */
#define HDMI_VIC_2560_1080_A60P_ULTRA             ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_1080P60_ULTRAWIDE)      /*  64:27          2560x1080p @ 60Hz */
#define HDMI_VIC_2560_1080_100P_ULTRA             ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P100_ULTRAWIDE)     /*  64:27          2560x1080p @ 100Hz */
#define HDMI_VIC_2560_1080_120P_ULTRA             ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_1080P120_ULTRAWIDE)     /*  64:27          2560x1080p @ 119.88Hz */
#define HDMI_VIC_2560_1080_A120P_ULTRA            ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_1080P120_ULTRAWIDE)     /*  64:27          2560x1080p @ 120Hz */
#define HDMI_VIC_3840_2160_24P                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P24)                /*  16:9           3840x2160p @ 23.98Hz */
#define HDMI_VIC_3840_2160_A24P                   ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_2160P24)                /*  16:9           3840x2160p @ 24Hz */
#define HDMI_VIC_3840_2160_25P                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P25)                /*  16:9           3840x2160p @ 25Hz */
#define HDMI_VIC_3840_2160_30P                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P30)                /*  16:9           3840x2160p @ 29.97Hz */
#define HDMI_VIC_3840_2160_A30P                   ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_2160P30)                /*  16:9           3840x2160p @ 30Hz */
#define HDMI_VIC_3840_2160_50P                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P50)                /*  16:9           3840x2160p @ 50Hz */
#define HDMI_VIC_3840_2160_60P                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P60)                /*  16:9           3840x2160p @ 59.94Hz */
#define HDMI_VIC_3840_2160_A60P                   ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_2160P60)                /*  16:9           3840x2160p @ 60Hz */
#define HDMI_VIC_4096_2160_24P_ULTRA              ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P24_ULTRAWIDE)      /*  256:135        4096x2160p @ 23.98Hz */
#define HDMI_VIC_4096_2160_A24P_ULTRA             ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_2160P24_ULTRAWIDE)      /*  256:135        4096x2160p @ 24Hz */
#define HDMI_VIC_4096_2160_25P_ULTRA              ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P25_ULTRAWIDE)      /*  256:135        4096x2160p @ 25Hz */
#define HDMI_VIC_4096_2160_30P_ULTRA              ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P30_ULTRAWIDE)      /*  256:135        4096x2160p @ 29.97Hz */
#define HDMI_VIC_4096_2160_A30P_ULTRA             ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_2160P30_ULTRAWIDE)      /*  256:135        4096x2160p @ 30Hz */
#define HDMI_VIC_4096_2160_50P_ULTRA              ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P50_ULTRAWIDE)      /*  256:135        4096x2160p @ 50Hz */
#define HDMI_VIC_4096_2160_60P_ULTRA              ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P60_ULTRAWIDE)      /*  256:135        4096x2160p @ 59.94Hz */
#define HDMI_VIC_4096_2160_A60P_ULTRA             ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_2160P60_ULTRAWIDE)      /*  256:135        4096x2160p @ 60Hz */
#define HDMI_VIC_3840_2160_24P_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P24_WIDE)           /*  64:27          3840x2160p @ 23.98Hz */
#define HDMI_VIC_3840_2160_A24P_WIDE              ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_2160P24_WIDE)           /*  64:27          3840x2160p @ 24Hz */
#define HDMI_VIC_3840_2160_25P_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P25_WIDE)           /*  64:27          3840x2160p @ 25Hz */
#define HDMI_VIC_3840_2160_30P_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P30_WIDE)           /*  64:27          3840x2160p @ 29.97Hz */
#define HDMI_VIC_3840_2160_A30P_WIDE              ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_2160P30_WIDE)           /*  64:27          3840x2160p @ 30Hz */
#define HDMI_VIC_3840_2160_50P_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P50_WIDE)           /*  64:27          3840x2160p @ 50Hz */
#define HDMI_VIC_3840_2160_60P_WIDE               ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_2160P60_WIDE)           /*  64:27          3840x2160p @ 59.94Hz */
#define HDMI_VIC_3840_2160_A60P_WIDE              ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_2160P60_WIDE)           /*  64:27          3840x2160p @ 60Hz */
/* Assume one base block (up to 4 DTDs) and one CEA extension (up to 6 DTDs) */
#define HDMI_VIC_DTD0_NORMAL                      ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_DTD0)                   /* 1st detailed timing. preferred timing. */
#define HDMI_VIC_DTD0_STRICT                      ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_DTD0)                   /* 1st detailed timing. preferred timing. */
#define HDMI_VIC_DTD1_NORMAL                      ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_DTD1)                   /* 2nd detailed timing. */
#define HDMI_VIC_DTD1_STRICT                      ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_DTD1)                   /* 2nd detailed timing. */
#define HDMI_VIC_DTD2_NORMAL                      ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_DTD2)                   /* 3rd detailed timing. */
#define HDMI_VIC_DTD2_STRICT                      ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_DTD2)                   /* 3rd detailed timing. */
#define HDMI_VIC_DTD3_NORMAL                      ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_DTD3)                   /* 4th detailed timing. */
#define HDMI_VIC_DTD3_STRICT                      ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_DTD3)                   /* 4th detailed timing. */
#define HDMI_VIC_DTD4_NORMAL                      ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_DTD4)                   /* 5th detailed timing. */
#define HDMI_VIC_DTD4_STRICT                      ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_DTD4)                   /* 5th detailed timing. */
#define HDMI_VIC_DTD5_NORMAL                      ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_DTD5)                   /* 6th detailed timing. */
#define HDMI_VIC_DTD5_STRICT                      ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_DTD5)                   /* 6th detailed timing. */
#define HDMI_VIC_DTD6_NORMAL                      ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_DTD6)                   /* 7th detailed timing. */
#define HDMI_VIC_DTD6_STRICT                      ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_DTD6)                   /* 7th detailed timing. */
#define HDMI_VIC_DTD7_NORMAL                      ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_DTD7)                   /* 8th detailed timing. */
#define HDMI_VIC_DTD7_STRICT                      ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_DTD7)                   /* 8th detailed timing. */
#define HDMI_VIC_DTD8_NORMAL                      ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_DTD8)                   /* 9th detailed timing. */
#define HDMI_VIC_DTD8_STRICT                      ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_DTD8)                   /* 9th detailed timing. */
#define HDMI_VIC_DTD9_NORMAL                      ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_DTD9)                   /* 10th detailed timing. */
#define HDMI_VIC_DTD9_STRICT                      ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_DTD9)                   /* 10th detailed timing. */
#define HDMI_VIC_7680_4320_30P                    ((UINT8)HDMI_FRAME_RATE_NORMAL | (UINT8)HDMI_VIC_4320P30)                /*  16:9           7680x4320p @ 29.97Hz */
/* #define HDMI_VIC_7680_4320_A30P                   ((UINT8)HDMI_FRAME_RATE_STRICT | (UINT8)HDMI_VIC_4320P30)  */              /*  16:9           7680x4320p @ 30Hz */

/*
 * Definitions for HDMI Audio & Speaker Information
 */
/* Code Short Name                          ID      Encoding Standard */
#define HDMI_AIC_NA                         0U      /* N/A */
#define HDMI_AIC_LPCM                       1U      /* L-PCM: IEC 60958-3 */
#define HDMI_AIC_AC3                        2U      /* AC-3: ATSC A/52B excluding Annex E */
#define HDMI_AIC_MPEG1                      3U      /* MPEG-1: ISO/IEC 11172-3 Layer 1 or Layer 2 */
#define HDMI_AIC_MP3                        4U      /* MP3: ISO/IEC 11172-3 Layer 3 */
#define HDMI_AIC_MPEG2                      5U      /* MPEG2: ISO/IEC 13818-3 */
#define HDMI_AIC_AACLC                      6U      /* AAC LC: ISO/IEC 14496-3 */
#define HDMI_AIC_DTS                        7U      /* DTS: ETSI TS 102 114 */
#define HDMI_AIC_ATRAC                      8U      /* ATRAC: IEC 61909 */
#define HDMI_AIC_DSD                        9U      /* One Bit Audio: ISO/IEC 14496-3 subpart 10 */
#define HDMI_AIC_EAC3                       10U     /* Enahnced AC-3: ATSC A/52B with Annex E */
#define HDMI_AIC_DTS_HD                     11U     /* DTS-HD: DVD Forum DTS-HD */
#define HDMI_AIC_MLP                        12U     /* MAT: DVD Forum MLP */
#define HDMI_AIC_DST                        13U     /* DST: ISO/IEC 14496-3 subpart 10 */
#define HDMI_AIC_WMA_PRO                    14U     /* WMA Pro: WMA Pro Decoder Specification */

#define HDMI_AIC_EXT                        16U     /* AICs after this offset are the Audio Format Code Extension */
#define HDMI_AIC_NOT_IN_USE0                (HDMI_AIC_EXT + 1U)     /* Not in use */
#define HDMI_AIC_NOT_IN_USE1                (HDMI_AIC_EXT + 2U)     /* Not in use */
#define HDMI_AIC_NOT_IN_USE2                (HDMI_AIC_EXT + 3U)     /* Not in use */
#define HDMI_AIC_MPEG4_HEAAC                (HDMI_AIC_EXT + 4U)     /* MPEG-4 HE AAC: ISO/IEC 14496-3 */
#define HDMI_AIC_MPEG4_HEAACV2              (HDMI_AIC_EXT + 5U)     /* MPEG-4 HE AAC v2: ISO/IEC 23003-1 */
#define HDMI_AIC_MPEG4_AAC_LC               (HDMI_AIC_EXT + 6U)     /* MPEG-4 AAC LC: ISO/IEC 14496-3 */
#define HDMI_AIC_DRA                        (HDMI_AIC_EXT + 7U)     /* DRA: GB/T 22726 */
#define HDMI_AIC_MPEG4_HEAAC_SURROUND       (HDMI_AIC_EXT + 8U)     /* MPEG-4 HE AAC(ISO/IEC 14496-3) + MPEG Surround(ISO/IEC 23003-1) */
#define HDMI_AIC_RESERVED                   (HDMI_AIC_EXT + 9U)     /* Reserved */
#define HDMI_AIC_MPEG4_AACLC_SURROUND       (HDMI_AIC_EXT + 10U)    /* MPEG-4 AAC LC(ISO/IEC 14496-3) + MPEG Surround(ISO/IEC 23003-1) */
#define NUM_HDMI_AIC                        (HDMI_AIC_EXT + 32U)    /* Total Number of audio format codes */

#define HDMI_TMDS_CLK_25174825              0U      /* TMDS Clock =  25.20 / 1.001 MHz */
#define HDMI_TMDS_CLK_25200000              1U      /* TMDS Clock =  25.20         MHz */
#define HDMI_TMDS_CLK_27000000              2U      /* TMDS Clock =  27.00         MHz */
#define HDMI_TMDS_CLK_27027000              3U      /* TMDS Clock =  27.00 * 1.001 MHz */
#define HDMI_TMDS_CLK_54000000              4U      /* TMDS Clock =  54.00         MHz */
#define HDMI_TMDS_CLK_54054000              5U      /* TMDS Clock =  54.00 * 1.001 MHz */
#define HDMI_TMDS_CLK_59340659              6U      /* TMDS Clock =  59.40 / 1.001 MHz */
#define HDMI_TMDS_CLK_59400000              7U      /* TMDS Clock =  59.40         MHz */
#define HDMI_TMDS_CLK_72000000              8U      /* TMDS Clock =  72.00         MHz */
#define HDMI_TMDS_CLK_74175824              9U      /* TMDS Clock =  74.25 / 1.001 MHz */
#define HDMI_TMDS_CLK_74250000              10U     /* TMDS Clock =  74.25         MHz */
#define HDMI_TMDS_CLK_108000000             11U     /* TMDS Clock = 108.00         MHz */
#define HDMI_TMDS_CLK_108108000             12U     /* TMDS Clock = 108.00 * 1.001 MHz */
#define HDMI_TMDS_CLK_148351648             13U     /* TMDS Clock = 148.50 / 1.001 MHz */
#define HDMI_TMDS_CLK_148500000             14U     /* TMDS Clock = 148.50         MHz */
#define HDMI_TMDS_CLK_296703296             15U     /* TMDS Clock = 297.00 / 1.001 MHz */
#define HDMI_TMDS_CLK_297000000             16U     /* TMDS Clock = 297.00         MHz */
#define HDMI_TMDS_CLK_593406593             17U     /* TMDS Clock = 594.00 / 1.001 MHz */
#define HDMI_TMDS_CLK_594000000             18U     /* TMDS Clock = 594.00         MHz */
#define HDMI_TMDS_CLK_OTHER                 19U     /* Other TMDS Clock */
#define NUM_HDMI_TMDS_CLOCK                 20U     /* Total Number of TMDS Clock frequencies */

/*
 * Definitions for HDMI Data Island Packets
 */
#define HDMI_PACKET_SEND_NONE               0U
#define HDMI_PACKET_SEND_ONCE               1U
#define HDMI_PACKET_SEND_EVERY_FRAME        2U
#define NUM_HDMI_PACKET_SEND_MODE           3U

#define HDMI_PACKET_NULL                    0x00U   /* NULL packet */
#define HDMI_PACKET_AUDIO_CLOCK_REGEN       0x01U   /* Audio Clock Regeneration packet */
#define HDMI_PACKET_AUDIO_SAMPLE            0x02U   /* Audio Sample packet */
#define HDMI_PACKET_GENERAL_CTRL            0x03U   /* General Control packet */
#define HDMI_PACKET_ACP                     0x04U   /* Audio Content Protection packet */
#define HDMI_PACKET_ISRC1                   0x05U   /* ISRC packet */
#define HDMI_PACKET_ISRC2                   0x06U   /* ISRC packet */
#define HDMI_PACKET_ONE_BIT_AUDIO_SAMPLE    0x07U   /* One Bit Audio Sample packet */
#define HDMI_PACKET_DST_AUDIO               0x08U   /* DST Audio packet */
#define HDMI_PACKET_HBR_AUDIO_STREAM        0x09U   /* High-Bitrate Audio Stream packet */
#define HDMI_PACKET_GAMUT_METADATA          0x0AU   /* Gamut Metadata packet */
#define NUM_HDMI_PACKET_TYPE                0x0BU   /* Number of packet types (excludes InfoFrame packets) */

#define HDMI_INFOFRAME_RESERVED             0U
#define HDMI_INFOFRAME_VENDOR_SPECIFIC      1U      /* Vendor-Specific InfoFrame packet */
#define HDMI_INFOFRAME_AVI                  2U      /* Auxiliary Video Information InfoFrame packet */
#define HDMI_INFOFRAME_SPD                  3U      /* Source Product Descriptor InfoFrame packet */
#define HDMI_INFOFRAME_AUDIO                4U      /* Audio InfoFrame packet */
#define HDMI_INFOFRAME_MPEG_SOURCE          5U      /* MPEG Source InfoFrame packet */
#define NUM_HDMI_INFOFRAME_TYPE             6U      /* Number of InfoFrame types (CEA-861-D) */

#define HDMI_INFOFRAME_SIZE_MAX             27U     /* Maximum size of InfoFrame packet payload */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define HDMI_INFOFRAME_SIZE_AVI             14U     /* Payload size of AVI InfoFrame packet (HDMI2.1) */
#else
#define HDMI_INFOFRAME_SIZE_AVI             13U     /* Payload size of AVI InfoFrame packet */
#endif
#define HDMI_INFOFRAME_SIZE_SPD             25U     /* Payload size of Source Product Description InfoFrame packet */
#define HDMI_INFOFRAME_SIZE_AUDIO           10U     /* Payload size of Audio InfoFrame packet */
#define HDMI_INFOFRAME_SIZE_MPEG_SOURCE     10U     /* Payload size of MPEG Source InfoFrame packet */

typedef struct {
    UINT32  Type:                   7;              /* Packet/InfoFrame Type */
    UINT32  InfoFrameFlag:          1;              /* InfoFrame Packet Type Indicator */
    UINT32  Version:                8;              /* Version of Packet/InfoFrame format */
    UINT32  Length:                 8;              /* Number of bytes in packet payload */
    UINT32  Checksum:               8;              /* Checksum of the packet */
} AMBA_HDMI_PACKET_HEADER_s;

typedef struct {
    UINT32  Header;                                 /* Packet header */
    UINT8   Payload[HDMI_INFOFRAME_SIZE_MAX];       /* Packet payload */
    UINT8   Reserved;                               /* Reserved for 32-bit alignment */
} AMBA_HDMI_DATA_ISLAND_PACKET_s;

/*
 * Definitions for Video Configuration
 */
#define HDMI_ASPECT_RATIO_4x3       (0x00040003U)   /* Picture Aspect Ratio 4:3 */
#define HDMI_ASPECT_RATIO_16x9      (0x00100009U)   /* Picture Aspect Ratio 16:9 */
#define HDMI_ASPECT_RATIO_64x27     (0x0040001BU)   /* Picture Aspect Ratio 64:27 */
#define HDMI_ASPECT_RATIO_256x135   (0x01000087U)   /* Picture Aspect Ratio 256:135 */

#define HDMI_QRANGE_DEFAULT         (0U)            /* RGB/YCC with a default quantization range depends on video format */
#define HDMI_QRANGE_LIMITED         (1U)            /* RGB/YCC with a limited quantization range of 220 levels (16 to 235) */
#define HDMI_QRANGE_FULL            (2U)            /* RGB/YCC with a full quantization range of 256 levels (0 to 255) */

#define HDMI_PIXEL_FORMAT_RGB_444   (0U)            /* RGB 4:4:4 chroma subsampling. Support 8 bits per color channel. */
#define HDMI_PIXEL_FORMAT_YCC_444   (1U)            /* YCbCr 4:4:4 chroma subsampling. Support 8 bits per color channel. */
#define HDMI_PIXEL_FORMAT_YCC_422   (2U)            /* YCbCr 4:2:2 chroma subsampling. Support 12 bits per color channel. */
#define HDMI_PIXEL_FORMAT_YCC_420   (3U)            /* YCbCr 4:2:0 chroma subsampling. Support 8 bits per color channel. */

#define HDMI_COLOR_DEPTH_24BPP      (0U)            /* 24 bits/pixel for 8-bit RGB/YCC 4:4:4 or 12-bit YCC 4:2:2 */

typedef struct  {
    UINT8   Vic4_3;                         /* Video ID Code for 4:3 picture aspect ratio */
    UINT8   Vic16_9;                        /* Video ID Code for 16:9 picture aspect ratio */
    UINT8   Vic64_27;                       /* Video ID Code for 64:27 picture aspect ratio */
    UINT8   Vic256_135;                     /* Video ID Code for 256:135 picture aspect ratio */
    UINT32  PixelClock;                     /* Unit: KHz */
    UINT16  HsyncFrontPorch;                /* Horizontal synchronization front porch */
    UINT16  HsyncPulseWidth;                /* Horizontal synchronization pulse width */
    UINT16  HsyncBackPorch;                 /* Horizontal synchronization back porch */
    UINT16  VsyncFrontPorch;                /* Vertical synchronization front porch */
    UINT16  VsyncPulseWidth;                /* Vertical synchronization pulse width */
    UINT16  VsyncBackPorch;                 /* Vertical synchronization back porch */
    UINT16  ActivePixels;                   /* Unit: cycles */
    UINT16  ActiveLines;                    /* Unit: lines */
    UINT8   FieldRate;                      /* Field rate for informational purposes only */
    UINT8   SyncPolarity;                   /* Polarity of H Sync and V Sync */
    UINT8   ScanFormat;                     /* Video scanning format */
    UINT8   PixelRepeat;                    /* 1:Pixel data send 2 times */
} AMBA_HDMI_VIDEO_TIMING_s;

typedef struct {
    UINT8   VideoIdCode;                    /* Base video format ID code */
    UINT8   PixelFormat;                    /* RGB/YCbCr color space */
    UINT8   ColorDepth;                     /* HDMI color depth (8/10/12 bits) */
    UINT8   QuantRange;                     /* Quantization rage of RGB/YCC */
    AMBA_HDMI_VIDEO_TIMING_s    CustomDTD;  /* Custom timings. Valid for HDMI_VIC_RESERVED */
} AMBA_HDMI_VIDEO_CONFIG_s;

/*
 * Definitions for Audio Configuration
 */
#define HDMI_AUDIO_FS_32K                   0U      /* Sample rate =  32000 */
#define HDMI_AUDIO_FS_44P1K                 1U      /* Sample rate =  44100 */
#define HDMI_AUDIO_FS_48K                   2U      /* Sample rate =  48000 */
#define HDMI_AUDIO_FS_88P2K                 3U      /* Sample rate =  88200 */
#define HDMI_AUDIO_FS_96K                   4U      /* Sample rate =  96000 */
#define HDMI_AUDIO_FS_176P4K                5U      /* Sample rate = 176400 */
#define HDMI_AUDIO_FS_192K                  6U      /* Sample rate = 192000 */
#define NUM_HDMI_AUDIO_FS                   7U      /* Total Number of Audio sample rates */

/* Code Short Name                          CA Code    CH1 CH2 CH3 CH4 CH5 CH6 CH7 CH8 */
#define HDMI_CA_2CH                         0U      /* FL  FR  -   -   -   -   -   -   */
#define HDMI_CA_3CH_LFE                     1U      /* FL  FR  LFE -   -   -   -   -   */
#define HDMI_CA_3CH_FC                      2U      /* FL  FR  -   FC  RC  -   -   -   */
#define HDMI_CA_4CH_LFE_FC                  3U      /* FL  FR  LFE FC  RC  -   -   -   */
#define HDMI_CA_3CH_RC                      4U      /* FL  FR  -   -   RC  -   -   -   */
#define HDMI_CA_4CH_LFE_RC                  5U      /* FL  FR  LFE -   RC  -   -   -   */
#define HDMI_CA_4CH_FC_RC                   6U      /* FL  FR  -   FC  RC  -   -   -   */
#define HDMI_CA_5CH_LFE_FC_RC               7U      /* FL  FR  LFE FC  RC  -   -   -   */
#define HDMI_CA_4CH_RL_RR                   8U      /* FL  FR  -   -   RL  RR  -   -   */
#define HDMI_CA_5CH_LFE_RL_RR               9U      /* FL  FR  LFE -   RL  RR  -   -   */
#define HDMI_CA_5CH_FC_RL_RR                10U     /* FL  FR  -   FC  RL  RR  -   -   */
#define HDMI_CA_6CH_LFE_FC_RL_RR            11U     /* FL  FR  LFE FC  RL  RR  -   -   */
#define HDMI_CA_5CH_RL_RR_RC                12U     /* FL  FR  -   -   RL  RR  RC  -   */
#define HDMI_CA_6CH_LFE_RL_RR_RC            13U     /* FL  FR  LFE -   RL  RR  RC  -   */
#define HDMI_CA_6CH_FC_RL_RR_RC             14U     /* FL  FR  -   FC  RL  RR  RC  -   */
#define HDMI_CA_7CH_LFE_FC_RL_RR_RC         15U     /* FL  FR  LFE FC  RL  RR  RC  -   */
#define HDMI_CA_6CH_RL_RR_RLC_RRC           16U     /* FL  FR  -   -   RL  RR  RLC RRC */
#define HDMI_CA_7CH_LFE_RL_RR_RLC_RRC       17U     /* FL  FR  LFE -   RL  RR  RLC RRC */
#define HDMI_CA_7CH_FC_RL_RR_RLC_RRC        18U     /* FL  FR  -   FC  RL  RR  RLC RRC */
#define HDMI_CA_8CH_LFE_FC_RL_RR_RLC_RRC    19U     /* FL  FR  LFE FC  RL  RR  RLC RRC */
#define HDMI_CA_4CH_FLC_FRC                 20U     /* FL  FR  -   -   -   -   FLC FRC */
#define HDMI_CA_5CH_LFE_FLC_FRC             21U     /* FL  FR  LFE -   -   -   FLC FRC */
#define HDMI_CA_5CH_FC_FLC_FRC              22U     /* FL  FR  -   FC  RC  -   FLC FRC */
#define HDMI_CA_6CH_LFE_FC_FLC_FRC          23U     /* FL  FR  LFE FC  RC  -   FLC FRC */
#define HDMI_CA_5CH_RC_FLC_FRC              24U     /* FL  FR  -   -   RC  -   FLC FRC */
#define HDMI_CA_6CH_LFE_RC_FLC_FRC          25U     /* FL  FR  LFE -   RC  -   FLC FRC */
#define HDMI_CA_6CH_FC_RC_FLC_FRC           26U     /* FL  FR  -   FC  RC  -   FLC FRC */
#define HDMI_CA_7CH_LFE_FC_RC_FLC_FRC       27U     /* FL  FR  LFE FC  RC  -   FLC FRC */
#define HDMI_CA_6CH_RL_RR_FLC_FRC           28U     /* FL  FR  -   -   RL  RR  FLC FRC */
#define HDMI_CA_7CH_LFE_RL_RR_FLC_FRC       29U     /* FL  FR  LFE -   RL  RR  FLC FRC */
#define HDMI_CA_7CH_FC_RL_RR_FLC_FRC        30U     /* FL  FR  -   FC  RL  RR  FLC FRC */
#define HDMI_CA_8CH_LFE_FC_RL_RR_FLC_FRC    31U     /* FL  FR  LFE FC  RL  RR  FLC FRC */
#define HDMI_CA_6CH_FC_RL_RR_FCH            32U     /* FL  FR  -   FC  RL  RR  FCH -   */
#define HDMI_CA_7CH_LFE_FC_RL_RR_FCH        33U     /* FL  FR  LFE FC  RL  RR  FCH -   */
#define HDMI_CA_6CH_FC_RL_RR_TC             34U     /* FL  FR  -   FC  RL  RR  -   TC  */
#define HDMI_CA_7CH_LFE_FC_RL_RR_TC         35U     /* FL  FR  LFE FC  RL  RR  -   TC  */
#define HDMI_CA_6CH_RL_RR_FLH_FRH           36U     /* FL  FR  -   -   RL  RR  FLH FRH */
#define HDMI_CA_7CH_LFE_RL_RR_FLH_FRH       37U     /* FL  FR  LFE -   RL  RR  FLH FRH */
#define HDMI_CA_6CH_RL_RR_FLW_FRW           38U     /* FL  FR  -   -   RL  RR  FLW FRW */
#define HDMI_CA_7CH_LFE_RL_RR_FLW_FRW       39U     /* FL  FR  LFE -   RL  RR  FLW FRW */
#define HDMI_CA_7CH_FC_RL_RR_RC_TC          40U     /* FL  FR  -   FC  RL  RR  RC  TC  */
#define HDMI_CA_8CH_LFE_FC_RL_RR_RC_TC      41U     /* FL  FR  LFE FC  RL  RR  RC  TC  */
#define HDMI_CA_7CH_FC_RL_RR_RC_FCH         42U     /* FL  FR  -   FC  RL  RR  RC  FCH */
#define HDMI_CA_8CH_LFE_FC_RL_RR_RC_FCH     43U     /* FL  FR  LFE FC  RL  RR  RC  FCH */
#define HDMI_CA_7CH_FC_RL_RR_FCH_TC         44U     /* FL  FR  -   FC  RL  RR  FCH TC  */
#define HDMI_CA_8CH_LFE_FC_RL_RR_FCH_TC     45U     /* FL  FR  LFE FC  RL  RR  FCH TC  */
#define HDMI_CA_7CH_FC_RL_RR_FLH_FRH        46U     /* FL  FR  -   FC  RL  RR  FLH FRH */
#define HDMI_CA_8CH_LFE_FC_RL_RR_FLH_FRH    47U     /* FL  FR  LFE FC  RL  RR  FLH FRH */
#define HDMI_CA_7CH_FC_RL_RR_FLW_FRW        48U     /* FL  FR  -   FC  RL  RR  FLW FRW */
#define HDMI_CA_8CH_LFE_FC_RL_RR_FLW_FRW    49U     /* FL  FR  LFE FC  RL  RR  FLW FRW */
#define NUM_HDMI_AUDIO_CHANNEL_MAP          50U     /* Number of audio/speaker channel allocations */

typedef struct {
    UINT32  SampleRate; /* Audio sample rate */
    UINT32  SpeakerMap; /* Audio channel/speaker allocation ID */
} AMBA_HDMI_AUDIO_CONFIG_s;

/*
 * Definitions for CEC Transfer Result
 */
#define HDMI_CEC_TX_OK                      0x1U    /* Indicate successful transmitting CEC frame */
#define HDMI_CEC_TX_NG                      0x2U    /* Indicate failed transmitting CEC frame (after retry fail) */
#define HDMI_CEC_RX_OK                      0x4U    /* Indicate successful receiving CEC frame (frame is available) */

typedef void (*AMBA_HDMI_HPD_ISR_f)(UINT32 HdmiPort, UINT32 HotPlugFlag, UINT32 RxSenseFlag);
typedef void (*AMBA_HDMI_CEC_ISR_f)(UINT32 Result);

/* Legacy definitions */
#define AMBA_HDMI_TX_PORT0                  HDMI_TX_PORT0
#define AMBA_NUM_HDMI_TX_PORT               NUM_HDMI_TX_PORT

#define AMBA_HDMI_CEC_MSG_MAX_SIZE          HDMI_CEC_MSG_MAX_SIZE

#define AMBA_HDMI_HPD_NONE                  HDMI_HPD_NONE
#define AMBA_HDMI_HPD_ACTIVE                HDMI_HPD_ACTIVE
#define AMBA_HDMI_HPD_STANDBY               HDMI_HPD_STANDBY

#define AMBA_HDMI_EDID_UNAVAILABLE          HDMI_EDID_UNAVAIL
#define AMBA_HDMI_EDID_NON_HDMI             HDMI_EDID_DVI
#define AMBA_HDMI_EDID_HDMI                 HDMI_EDID_HDMI

#define AMBA_HDMI_ASPECT_RATIO_4x3          HDMI_ASPECT_RATIO_4x3
#define AMBA_HDMI_ASPECT_RATIO_16x9         HDMI_ASPECT_RATIO_16x9
#define AMBA_HDMI_ASPECT_RATIO_64x27        HDMI_ASPECT_RATIO_64x27
#define AMBA_HDMI_ASPECT_RATIO_256x135      HDMI_ASPECT_RATIO_256x135

#define AMBA_HDMI_QRANGE_DEFAULT            HDMI_QRANGE_DEFAULT
#define AMBA_HDMI_QRANGE_LIMITED            HDMI_QRANGE_LIMITED
#define AMBA_HDMI_QRANGE_FULL               HDMI_QRANGE_FULL

#define AMBA_HDMI_PIXEL_FORMAT_RGB_444      HDMI_PIXEL_FORMAT_RGB_444
#define AMBA_HDMI_PIXEL_FORMAT_YCC_444      HDMI_PIXEL_FORMAT_YCC_444
#define AMBA_HDMI_PIXEL_FORMAT_YCC_422      HDMI_PIXEL_FORMAT_YCC_422
#define AMBA_HDMI_PIXEL_FORMAT_YCC_420      HDMI_PIXEL_FORMAT_YCC_420

#define AMBA_HDMI_COLOR_DEPTH_24BPP         HDMI_COLOR_DEPTH_24BPP

#endif /* AMBA_HDMI_DEF_H */
