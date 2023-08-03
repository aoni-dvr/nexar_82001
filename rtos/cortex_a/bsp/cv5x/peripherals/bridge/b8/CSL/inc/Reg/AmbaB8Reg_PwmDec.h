/**
 *  @file AmbaReg_PwmDec.h
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
 *  @details Definitions & Constants for B6 PWM Decoder Control Registers
 *
 */

#ifndef AMBA_B8_REG_PWM_DEC_H
#define AMBA_B8_REG_PWM_DEC_H

/*-----------------------------------------------------------------------------------*\
 * B6 PWM DEC : Decoder Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Ch0CreditPwr:           3;      /* [2:0] Credit number */
    UINT32  Ch0WdChunkOption:       1;      /* [3] */
    UINT32  Reserved0:              28;     /* [31:4] */
} B8_PWM_DEC_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PWM DEC : Interrupt Status Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Ch0CrcErr:              1;      /* [0] CRC fails on lane#0. */
    UINT32  Ch0SequenceErr:         1;      /* [1] Sequential ID fails on lane#0 */
    UINT32  Ch0SymbolErr:           1;      /* [2] PHY symbol error occurs on lane#0 */
    UINT32  Reserved0:              29;     /* [31:3] */
} B8_PWM_DEC_INT_STATUS_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PWM DEC : Interrupt Mask Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Ch0IntMask:             3;      /* [2:0] Interrupt mask */
    UINT32  Reserved0:              29;     /* [31:3] */
} B8_PWM_DEC_INT_MASK_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 PWM DEC : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_PWM_DEC_CTRL_REG_s          Ctrl;          /* 0x00(RW): Decoder Control */
    volatile B8_PWM_DEC_INT_STATUS_REG_s    IntStatus;     /* 0x04(RWC): Interrupt Status */
    volatile B8_PWM_DEC_INT_MASK_REG_s      IntMask;       /* 0x08(RW): Interrupt Mask */
} B8_PWM_DEC_REG_s;

#endif /* AMBA_B8_REG_PWM_DEC_H */
