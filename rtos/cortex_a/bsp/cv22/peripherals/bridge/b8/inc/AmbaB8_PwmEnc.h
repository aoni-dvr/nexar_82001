/**
 *  @file AmbaB8_PwmEnc.h
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
 *  @details Definitions & Constants for B8 PWMEnc Control APIs
 *
 */

#ifndef AMBA_B8_PWMENC_H
#define AMBA_B8_PWMENC_H

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB8_PwmEnc.c
\*---------------------------------------------------------------------------*/

UINT32 AmbaB8_PwmEncClearErrStatus(UINT32 ChipID);
UINT32 AmbaB8_PwmEncMaskErrStatus(UINT32 ChipID, UINT32 Enable);
UINT32 AmbaB8_PwmEncTxReset(UINT32 ChipID);
UINT32 AmbaB8_PwmEncSendResetPkt(UINT32 ChipID);
UINT32 AmbaB8_PwmEncSetCreditPwr(UINT32 ChipID, UINT32 CreditPwr);
UINT32 AmbaB8_PwmEncSetChunkSize(UINT32 ChipID, UINT32 ChunkSize);
UINT32 AmbaB8_PwmEncSetDisableAck(UINT32 ChipID, UINT32 DisableAck);
UINT32 AmbaB8_PwmEncEnable(UINT32 ChipID, UINT32 ReplayTimes);
UINT32 AmbaB8_PwmEncRegWrite(UINT32 ChipID, const volatile void *pReg, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, const void *pTxDataBuf);
UINT32 AmbaB8_PwmEncRegRead(UINT32 ChipID, const volatile void *pReg, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, void *pRxDataBuf);

#endif  /* AMBA_B8_PWMENC_H */
