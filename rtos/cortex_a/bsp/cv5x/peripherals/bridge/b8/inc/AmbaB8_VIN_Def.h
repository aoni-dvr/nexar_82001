/**
 *  @file AmbaB8_VIN_Def.h
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
 *  @details Definitions & Constants for B6 VIN Control Definitions
 *
 */

#ifndef AMBA_B8_VIN_DEF_H
#define AMBA_B8_VIN_DEF_H

/* VIN interface */
#define B8_VIN_LVDS                             0U      /* Serial SLVS */
#define B8_VIN_LVCMOS                           1U      /* Parallel LVCMOS */
#define B8_VIN_MIPI                             2U      /* MIPI */

/* MIPI clock mode */
#define B8_VIN_MIPI_NON_CONTINUOUS_CLOCK_MODE   0U
#define B8_VIN_MIPI_CONTINUOUS_CLOCK_MODE       1U

#define B8_VIN_COLOR_SPACE_RGB                  0U
#define B8_VIN_COLOR_SPACE_YUV                  1U

#define B8_VIN_YUV_ORDER_Y0_CB_Y1_CR            0U
#define B8_VIN_YUV_ORDER_CR_Y0_CB_Y1            1U
#define B8_VIN_YUV_ORDER_CB_Y0_CR_Y1            2U
#define B8_VIN_YUV_ORDER_Y0_CR_Y1_CB            3U

#define B8_VIN_DVP_WIDE_1_PIXEL                 0U
#define B8_VIN_DVP_WIDE_2_PIXELS                1U

#define B8_VIN_DVP_LATCH_EDGE_RISING            0U
#define B8_VIN_DVP_LATCH_EDGE_FALLING           1U
#define B8_VIN_DVP_LATCH_EDGE_BOTH              2U

typedef struct {
    UINT32 DataRate;    /* receiving data rate per lane */
    UINT32 ClockMode;   /* MIPI clock mode, 0U: non-continuous, 1U: continuous
                           defined as B8_VIN_MIPI_xxx */
} B8_VIN_MIPI_DPHY_CTRL_s;

typedef struct {
    UINT16  StartX;     /* Crop Start Col */
    UINT16  StartY;     /* Crop Start Row */
    UINT16  EndX;       /* Crop End Col */
    UINT16  EndY;       /* Crop End Row */
} B8_VIN_WINDOW_s;


#endif  /* AMBA_B8_VIN_DEF_H */
