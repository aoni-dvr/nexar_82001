/**
 *  @file AmbaReg_Compressor.h
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
 *  @details Definitions & Constants for B6 Compressor Control Registers
 *
 */

#ifndef AMBA_B8_REG_COMPRESSOR_H
#define AMBA_B8_REG_COMPRESSOR_H

#if 0
/*-----------------------------------------------------------------------------------*\
 * B6 Compressor : Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT16  Enable:                 2;      /* [1:0] 1 = enable raw compression */
    UINT16  MantissaBits:           3;      /* [4:2] Number of Mantissa Bits/pixel. 0 - 6 bits, 1 - 7 bits, 2 - 8 bits, 3 - 9 bits */
    UINT16  BlockSize:              1;      /* [5] Block size. 0 - 2 pixels, 1 - 4 pixels */
    UINT16  Reserved:               10;     /* [15:6] */
} B8_COMPRESSOR_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Compressor : Dither Enable Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT16  Enable:                 1;      /* [0] Dither Enable */
    UINT16  Reserved:               15;     /* [15:1] */
} B8_COMPRESSOR_DITHER_ENABLE_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Compressor : Debug Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT16  Debug:                  9;      /* [8:0] Debug register */
    UINT16  Reserved:               7;      /* [15:9] */
} B8_COMPRESSOR_DEBUG_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Compressor : Offset Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT16  Offset:                 14;     /* [13:0] Offset */
    UINT16  Reserved:               2;      /* [15:14] */
} B8_COMPRESSOR_OFFSET_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Compressor : Config Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT16  Done:                   1;      /* [0] Config done */
    UINT16  Enable:                 1;      /* [1] (RO)Config enable */
    UINT16  Reserved:               14;     /* [15:2] */
} B8_COMPRESSOR_CONFIG_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 Compressor : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_COMPRESSOR_CTRL_REG_s               Ctrl;                   /* 0x00(RW): Compressor Control */
    volatile B8_COMPRESSOR_DITHER_ENABLE_REG_s      DitherEnable;           /* 0x02(RW): Dither enable */
    volatile UINT16                                 DitherRandomSeedLowX;   /* 0x04(RW): Dither random seed low x */
    volatile UINT16                                 DitherRandomSeedHighX;  /* 0x06(RW): Dither random seed high x */
    volatile UINT16                                 DitherRandomSeedLowY;   /* 0x08(RW): Dither random seed low y */
    volatile UINT16                                 DitherRandomSeedHighY;  /* 0x0A(RW): Dither random seed high y */
    volatile B8_COMPRESSOR_DEBUG_REG_s              DebugReg;               /* 0x0C(RO): Debug register */
    volatile B8_COMPRESSOR_OFFSET_REG_s             Offset;                 /* 0x0E(RW): Offset */
    volatile B8_COMPRESSOR_CONFIG_REG_s             ConfigDone;             /* 0x10(RW): Config Done */
} B8_COMPRESSOR_REG_s;
#else
typedef struct {
    volatile UINT16      Ctrl;                   /* 0x00(RW): Compressor Control */
    volatile UINT16      DitherEnable;           /* 0x02(RW): Dither enable */
    volatile UINT16      DitherRandomSeedLowX;   /* 0x04(RW): Dither random seed low x */
    volatile UINT16      DitherRandomSeedHighX;  /* 0x06(RW): Dither random seed high x */
    volatile UINT16      DitherRandomSeedLowY;   /* 0x08(RW): Dither random seed low y */
    volatile UINT16      DitherRandomSeedHighY;  /* 0x0A(RW): Dither random seed high y */
    volatile UINT16      DebugReg;               /* 0x0C(RO): Debug register */
    volatile UINT16      Offset;                 /* 0x0E(RW): Offset */
    volatile UINT16      ConfigDone;             /* 0x10(RW): Config Done */
} B8_COMPRESSOR_REG_s;
#endif
#endif /* AMBA_B8_REG_COMPRESSOR_H */
