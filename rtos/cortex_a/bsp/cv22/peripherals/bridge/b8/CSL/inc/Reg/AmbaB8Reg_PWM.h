/**
 *  @file AmbaB8Reg_I2C.h
 *
 *  @copyright Copyright (c) 2017 Ambarella, Inc.
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
 *  @details Definitions & Constants for B6 PWM Registers
 *
 */

#ifndef AMBA_B8_REG_PWM_H
#define AMBA_B8_REG_PWM_H

/*-----------------------------------------------------------------------------------------------*\
 * B6 PWM : Enable Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Enable:         1;      /* [0]: 1 - PWM Enabled; 0 - Disable */
    UINT32  Divider:        30;     /* [30:1]: Clock divider of PWM (PWM_CLK = SOURCE_CLK/(divider+1)) */
    UINT32  Reserved0:      1;      /* [31]: Reserved */
} B8_PWM_ENABLE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 PWM : Enable Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Xoff:           16;     /* [15:0]: Banked xoff data pattern for PWM */
    UINT32  Xon:            16;     /* [31:16]: Banked xon data pattern for PWM */
} B8_PWM_DATA_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 PWM : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile UINT32                         Reserved0;          /* 0x00: Reserved */
    volatile B8_PWM_ENABLE_REG_s       Pwm0Enable;         /* 0x04(RW): PWM0 Enable */
    volatile UINT32                         Reserved1;          /* 0x08: Reserved */
    volatile B8_PWM_ENABLE_REG_s       Pwm1Enable;         /* 0x0C(RW): PWM1 Enable */
    volatile UINT32                         Reserved2[4];       /* 0x10-0x1C: Reserved */
    volatile B8_PWM_DATA_REG_s         Pwm0Data;           /* 0x20(RW): Register Bank for PWM0 */
    volatile B8_PWM_DATA_REG_s         Pwm1Data;           /* 0x24(RW): Register Bank for PWM1 */
} B8_PWM_REG_s;

#endif /* AMBA_B8_REG_PWM_H */
