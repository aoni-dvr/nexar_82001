/**
 *  @file AmbaReg_Prescaler.h
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
 *  @details Definitions & Constants for B8 Prescaler Control Registers
 *
 */

#ifndef AMBA_B8_REG_PRESCALER_H
#define AMBA_B8_REG_PRESCALER_H

#if 0
/*-----------------------------------------------------------------------------------*\
 * B8 Prescaler : Prescaler Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Enable:                 1;      /* [0] 1 = Horizontal prescaler enable */
    UINT32  Shift:                  1;      /* [1] 0 = Shift 7 fractional bits, 1 = Shift 6 fractional bits */
    UINT32  Reserved:               14;     /* [15:2] */
    UINT32  Reserved1:              16;     /* for misrac workaround. Prescaler register only occupies 16 bits */
} B8_PRESCALER_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Prescaler : Output Width Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  OutputWidthMinus1:      13;     /* [12:0] Horizontal output width minus 1 */
    UINT32  Reserved:               3;      /* [15:13] */
    UINT32  Reserved1:              16;     /* for misrac workaround. Prescaler register only occupies 16 bits */
} B8_PRESCALER_OUTPUT_WIDTH_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Prescaler : Phase Increment Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Fractional:             13;     /* [12:0] Fractional bits of horizontal phase increment */
    UINT32  Integer:                2;      /* [14:13] Integer bits of horizontal phase increment */
    UINT32  Reserved:               1;      /* [15] */
    UINT32  Reserved1:              16;     /* for misrac workaround. Prescaler register only occupies 16 bits */
} B8_PRESCALER_PHASE_INCREMENT_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 Prescaler : Initial Phase Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Value:                  12;     /* [11:0]: Integer bits of the initial phase */
    UINT32  Reserved:               4;      /* [15:12]: */
    UINT32  Reserved1:              16;     /* for misrac workaround. Prescaler register only occupies 16 bits */
} B8_PRESCALER_INITIAL_PHASE_INT_REG_s;

typedef struct {
    UINT32  Value:                  13;     /* [12:0]: Fractional bits of the initial phase */
    UINT32  Reserved:               3;      /* [15:13]: */
    UINT32  Reserved1:              16;     /* for misrac workaround. Prescaler register only occupies 16 bits */
} B8_PRESCALER_INITIAL_PHASE_FRAC_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Prescaler : Update Done Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Reserved0:              1;      /* [0] */
    UINT32  UpdateDoneOnce:         1;      /* [1] Update Done (Next VSYNC and auto clear) */
    UINT32  UpdateDone:             1;      /* [2] Update Done (Every VSYNC) */
    UINT32  Reserved1:              13;     /* [15:3] */
    UINT32  Reserved2:              16;     /* for misrac workaround. Prescaler register only occupies 16 bits */
} B8_PRESCALER_UPDATE_DONE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 Prescaler : Phase Coefficient Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    struct {
        UINT16  Coef0:                  8;      /* [7:0]: Coefficient 0 */
        UINT16  Coef1:                  8;      /* [15:8]: Coefficient 1 */
    } Coef0_1;

    struct {
        UINT16  Coef2:                  8;      /* [7:0]: Coefficient 2 */
        UINT16  Coef3:                  8;      /* [15:8]: Coefficient 3 */
    } Coef2_3;

    struct {
        UINT16  Coef4:                  8;      /* [7:0]: Coefficient 4 */
        UINT16  Coef5:                  8;      /* [15:8]: Coefficient 5 */
    } Coef4_5;

} B8_PRESCALER_PHASE_COEF_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 Prescaler : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile UINT16     Ctrl;                /* 0x00(RW): Prescaler Control */
    volatile UINT16     OutputWidth;         /* 0x02(RW): Output Width */
    volatile UINT16     PhaseInc;            /* 0x04(RW): Phase Increment */
    volatile UINT16     FirstColorPhaseInt;  /* 0x06(RW): Integer Bits of the Initial Phase of the 1st Color */
    volatile UINT16     FirstColorPhaseFrac; /* 0x08(RW): Fractional Bits of the Initial Phase of the 1st Color */
    volatile UINT16     SecondColorPhaseInt; /* 0x0A(RW): Integer Bits of the Initial Phase of the 2nd Color */
    volatile UINT16     SecondColorPhaseFrac;/* 0x0C(RW): Fractional Bits of the Initial Phase of the 2nd Color */
    volatile UINT16     Reserved[2];         /* 0x0E-0x10: Reserved */
    volatile UINT16     UpdateDone;          /* 0x12(RW): Update Done */
    volatile UINT16     Reserved1[54];       /* 0x14-0x7E: Reserved */
    volatile B8_PRESCALER_PHASE_COEF_REG_s PhaseCoef[16];       /* 0x80-0xFE(RW): Phase 0..15 Coefficients */
} B8_PRESCALER_REG_s;
#else
/*-----------------------------------------------------------------------------------------------*\
 * B8 Prescaler : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile UINT16     Ctrl;                /* 0x00(RW): Prescaler Control */
    volatile UINT16     OutputWidth;         /* 0x02(RW): Output Width */
    volatile UINT16     PhaseInc;            /* 0x04(RW): Phase Increment */
    volatile UINT16     FirstColorPhaseInt;  /* 0x06(RW): Integer Bits of the Initial Phase of the 1st Color */
    volatile UINT16     FirstColorPhaseFrac; /* 0x08(RW): Fractional Bits of the Initial Phase of the 1st Color */
    volatile UINT16     SecondColorPhaseInt; /* 0x0A(RW): Integer Bits of the Initial Phase of the 2nd Color */
    volatile UINT16     SecondColorPhaseFrac;/* 0x0C(RW): Fractional Bits of the Initial Phase of the 2nd Color */
    volatile UINT16     Reserved[2];         /* 0x0E-0x10: Reserved */
    volatile UINT16     UpdateDone;          /* 0x12(RW): Update Done */
    volatile UINT16     Reserved1[54];       /* 0x14-0x7E: Reserved */
    volatile UINT16     PhaseCoef[16];       /* 0x80-0xFE(RW): Phase 0..15 Coefficients */
} B8_PRESCALER_REG_s;

#endif
#endif /* AMBA_B8_REG_PRESCALER_H */
