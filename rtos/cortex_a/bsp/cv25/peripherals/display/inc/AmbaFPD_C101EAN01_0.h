/**
 *  @file AmbaFPD_C101EAN01_0.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Definitions & Constants for AUO FPD C101EAN01.0 APIs.
 *
 */

#ifndef AMBA_FPD_C101EAN01_H
#define AMBA_FPD_C101EAN01_H

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the control signal types.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FPD_C101EAN01_1280_720_60HZ            0U      /* 1280x720@59.94Hz */
#define AMBA_FPD_C101EAN01_1280_720_A60HZ           1U      /* 1280x720@60.00Hz */
#define AMBA_FPD_C101EAN01_NUM_MODE                 2U

/*-----------------------------------------------------------------------------------------------*\
 * FPD DISPLAY CONFIG
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32                                  OutputMode;
    UINT8                                   BitOrder;
    UINT8                                   DataValidPolarity;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s       DisplayTiming;
} AMBA_FPD_C101EAN01_CONFIG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFPD_C101EAN01.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_FPD_OBJECT_s AmbaFPD_C101EAN01Obj;

#endif /* AMBA_FPD_C101EAN01_H */
