/**
 * @file EnetUtility.h
 * Enet common utility header
 *
 * @ingroup enet
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef AMBA_ENET_UTIL_H
#define AMBA_ENET_UTIL_H

/* for debug */
void EnetInfo(const char *func, const UINT32 line, const UINT16 ModuleID, const char *color, const char *msg);
void EnetDebug(const char *fmt, ...);
void EnetIsrDebug(const char *fmt, ...);
void ambhw_dump_buffer(const char *msg, const UINT8 *data, UINT32 length);
#ifdef CONFIG_NETX_ENET
#define NetXDebug       EnetDebug
#endif
#ifdef CONFIG_AMBA_AVB
extern void AvbDebug(const char *fmt, ...);
/*Enable AVB debug*/
//#define AVBDEBUG
#endif

/* utility API */
#if defined(CONFIG_THREADX)
UINT32 AmbaNetStack_Digits2IP(UINT32 a, UINT32 b, UINT32 c, UINT32 d);
UINT32 AmbaNetStack_InsertVlan(void **Addr, UINT16 *pLen, UINT16 tci, UINT16 type);
UINT32 AmbaNetStack_RemoveVlan(void **Addr, UINT16 *pLen, UINT16 *type);
UINT32 AmbaNetStack_InsertEthHdr(void **Addr, UINT16 *pLen, UINT16 tci, UINT16 type);
UINT32 AmbaNetStack_RemoveEthHdr(void **Addr, UINT16 *pLen);
UINT16 AmbaNetStack_Htons(UINT16 n);
UINT32 AmbaNetStack_Htonl(UINT32 n);
UINT16 AmbaNetStack_Ntohs(UINT16 n);
UINT32 AmbaNetStack_Ntohl(UINT32 n);
#endif

#endif /* AMBA_ENET_UTIL_H */
