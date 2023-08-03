/**
 *  @file AmbaB8_Depacker.h
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
 *  @details Definitions & Constants for B8 Prescaler Control APIs
 *
 */

#ifndef AMBA_B8_DEPACKER_H
#define AMBA_B8_DEPACKER_H

typedef struct {
    UINT16  ActiveWidth;
    UINT16  ActiveLine;
    UINT16  TotalCycle;         /* Total cycle of a line */
    UINT16  TotalLine;
    UINT32  PatternVal;
    UINT8   TestMode;           /* TestMode */
} B8_DEPACKER_TEST_CONFIG_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB8_Depacker.c
\*---------------------------------------------------------------------------*/
void AmbaB8_DepackerConfig(UINT32 ChipID);
UINT32 AmbaB8_DepackerTestMode(UINT32 ChipID, const B8_DEPACKER_TEST_CONFIG_s* pDepackerTestConfig);
UINT32 AmbaB8_DepackerReset(UINT32 ChipID);
UINT32 AmbaB8_DepackerGetStatPkt(UINT32 ChipID, UINT32 *pPassPkt, UINT32 *pErrPkt, UINT32 *pCorPkt);
UINT32 AmbaB8_DepackerGetErrorPkt(UINT32 ChipID);
UINT32 AmbaB8_DepackerGetCorrectedPkt(UINT32 ChipID);
UINT32 AmbaB8_DepackerGetPassPkt(UINT32 ChipID);
void AmbaB8_DepackerResetPktCounter(UINT32 ChipID);

#endif  /* AMBA_B8_DEPACKER_H */
