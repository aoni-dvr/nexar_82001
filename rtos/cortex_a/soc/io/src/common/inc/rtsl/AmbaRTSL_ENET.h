/**
 * @file AmbaRTSL_ENET.h
 * ENET mdio, isr
 *
 * @ingroup enet
 *
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AMBA_RTSL_ENET_H
#define AMBA_RTSL_ENET_H
#include "AmbaCSL_ENET.h"

/**
 *  AmbaSoC-accuracy 24Mhz, 41ns 0xffffffff
 *  Desired-accuracy 20Mhz, 50ns 0xffffffff*20/24=0xd5555555
 */
#define DEFAULT_ADDEND 0xd5555555U

typedef void (*AMBA_RTSL_ENET_TSISRCB_f)(const UINT32 Arg);
typedef void (*AMBA_RTSL_ENET_TXISRCB_f)(const AMBA_ENET_REG_s *const pEnetReg);
typedef void (*AMBA_RTSL_ENET_RXISRCB_f)(const AMBA_ENET_REG_s *const pEnetReg);
typedef UINT32 (*AMBA_RTSL_ENET_LINKCB_f)(AMBA_ENET_REG_s *const pEnetReg);

UINT32 AmbaRTSL_EnetSetTsIsrFunc(AMBA_RTSL_ENET_TSISRCB_f pISR, const UINT32 Arg);
UINT32 AmbaRTSL_EnetSetTxIsrFunc(AMBA_RTSL_ENET_TXISRCB_f pISR);
UINT32 AmbaRTSL_EnetSetRxIsrFunc(AMBA_RTSL_ENET_RXISRCB_f pISR);
UINT32 AmbaRTSL_EnetSetLcFunc(AMBA_RTSL_ENET_LINKCB_f pISR);
UINT16 AmbaRTSL_EnetMiiRead(AMBA_ENET_REG_s *const pEnetReg, UINT8 addr, UINT8 reg);
UINT16 AmbaRTSL_EnetMiiWrite(AMBA_ENET_REG_s *const pEnetReg, UINT8 addr, UINT8 regnum, UINT16 value);
UINT32 AmbaRTSL_EnetGetPhyId(AMBA_ENET_REG_s *const pEnetReg, UINT8 PhyAddr);
UINT8 AmbaRTSL_EnetGetPhyAddr(AMBA_ENET_REG_s *const pEnetReg);
void AmbaRTSL_GetLink(AMBA_ENET_REG_s *const pEnetReg, UINT8 PhyAddr, UINT32 *Speed, UINT32 *Duplex);
void AmbaRTSL_EnetInit(AMBA_ENET_REG_s *const pEnetReg);
void AmbaRTSL_EnetINTEnable(UINT32 Idx);
void AmbaRTSL_EnetINTDisable(UINT32 Idx);
UINT32 AmbaRTSL_EnetGetRegIdx(const AMBA_ENET_REG_s *const pEnetReg);

#endif /* AMBA_RTSL_ENET_H */
