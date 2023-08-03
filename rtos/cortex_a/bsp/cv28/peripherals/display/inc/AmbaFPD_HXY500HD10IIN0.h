/**
 *  @file AmbaFPD_HXY500HD10IIN0.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Definitions & Constants for DZXtech FPD panel HXY500HD10IIN0 APIs.
 *
 */

#ifndef AMBA_FPD_HXY500HD10IIN0_H
#define AMBA_FPD_HXY500HD10IIN0_H

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the control signal types.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FPD_HXY500HD10IIN0_60HZ           0U  /* 720x1280@59.94Hz */
#define AMBA_FPD_HXY500HD10IIN0_A60HZ          1U  /* 720x1280@60.00Hz */
#define AMBA_FPD_HXY500HD10IIN0_NUM_MODE       2U

/*-----------------------------------------------------------------------------------------------*\
 * FPD DISPLAY CONFIG
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32                                  OutputMode;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s       DisplayTiming;
    AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s  BlankPacket;
} AMBA_FPD_HXY500HD10IIN0_CONFIG_s;

typedef struct {
    UINT32 Address;
    UINT8 NumData;
    UINT8 Data[3];
} AMBA_FPD_HXY500HD10IIN0_COMMAND_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFPD_HXY500HD10IIN0.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_FPD_OBJECT_s AmbaFPD_HXY500HD10IIN0Obj;

#endif /* AMBA_FPD_HXY500HD10IIN0_H */
