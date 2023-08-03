/**
 *  @file AmbaB8_Scratchpad.h
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
 *  @details Definitions & Constants for B8 Scratchpad Control APIs
 *
 */

#ifndef AMBA_B8_SCRATCHPAD_H
#define AMBA_B8_SCRATCHPAD_H

#define B8_ECC_ENC_CHANNEL0         0U
#define B8_ECC_ENC_CHANNEL1         1U
#define B8_ECC_ENC_CHANNEL2         2U
#define B8_ECC_ENC_CHANNEL3         3U
#define B8_ECC_ENC_CHANNEL4         4U
#define B8_ECC_ENC_CHANNEL5         5U
#define B8_ECC_ENC_CHANNEL6         6U
#define B8_ECC_ENC_CHANNEL7         7U
#define B8_NUM_ECC_ENC_CHANNEL      8U

#define B8_ECC_DEC_CHANNEL0         0U
#define B8_ECC_DEC_CHANNEL1         1U
#define B8_ECC_DEC_CHANNEL2         2U
#define B8_ECC_DEC_CHANNEL3         3U
#define B8_ECC_DEC_CHANNEL4         4U
#define B8_ECC_DEC_CHANNEL5         5U
#define B8_NUM_ECC_DEC_CHANNEL      6U

/*-----------------------------------------------------------------------------------------------*\
 * Scratchpad Configuration
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32 Reserved;
} B8_SCRATCHPAD_CONFIG_s;

typedef struct {
    UINT32 EncEnable[B8_NUM_ECC_ENC_CHANNEL];   /* enable/disable 0-th ECC enc */
    UINT32 DecEnable[B8_NUM_ECC_DEC_CHANNEL];   /* enable/disable 0-th ECC dec */
    UINT32 NumCheckSymbol;                      /* t = 1 or 4 or 8 */
} B8_ECC_CONFIG_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB8_Scratchpad.c
\*---------------------------------------------------------------------------*/
void AmbaB8_ScratchpadConfig(UINT32 ChipID, const B8_SCRATCHPAD_CONFIG_s* pScratchpadConfig);
UINT32 AmbaB8_EccConfig(UINT32 ChipID, const B8_ECC_CONFIG_s* pConfig);
UINT32 AmbaB8_EccEncReset(UINT32 ChipID, UINT32 Channel);
UINT32 AmbaB8_EccDecReset(UINT32 ChipID, UINT32 Channel);
UINT32 AmbaB8_VoutInputSrcEnable(UINT32 ChipID, UINT32 Channel);

#endif  /* AMBA_B8_SCRATCHPAD_H */
