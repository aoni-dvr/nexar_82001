/**
 *  @file AmbaIPC_FlexidagIO.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for FlexidagIO RPC Services (RTOS side)
 *
 */

#ifndef AMBAIPC_FLEXIDAGIO_H
#define AMBAIPC_FLEXIDAGIO_H

#define FLEXIDAGIO_OK    (0U)
#define FLEXIDAGIO_ERR   (1U)

#define FLEXIDAGIO_RESULT_OUTTYPE_RAW   (0U)
#define FLEXIDAGIO_RESULT_OUTTYPE_OD    (1U)
#define FLEXIDAGIO_RESULT_OUTTYPE_SEG   (2U)

#define FLEXIDAGIO_RESULT_QUEUE_NUM     4U

typedef UINT32 (*AMBA_IPC_FLEXIDAGIO_CONFIG_f)(UINT32 Channel, UINT32 OutType);
typedef UINT32 (*AMBA_IPC_FLEXIDAGIO_RESULT_f)(UINT32 Channel, const void *pData, UINT32 Len);

UINT32 AmbaIPC_FlexidagIO_Init(UINT32 Channel, AMBA_IPC_FLEXIDAGIO_CONFIG_f ConfigFun);
UINT32 AmbaIPC_FlexidagIO_Deinit(UINT32 Channel);
UINT32 AmbaIPC_FlexidagIO_SetInput(UINT32 Channel, void *pData, UINT32 Len);
UINT32 AmbaIPC_FlexidagIO_GetResult_SetCB(UINT32 Channel, AMBA_IPC_FLEXIDAGIO_RESULT_f ResultFun);
UINT32 AmbaIPC_FlexidagIO_GetResult(UINT32 Channel, void *pData, UINT32 *Len);

#endif /* AMBAIPC_FLEXIDAGIO_H */
