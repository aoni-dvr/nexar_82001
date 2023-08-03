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
 *  @details Definitions & Constants for FlexidagIO IPC Services
 *
 */

#ifndef AMBAIPC_FLEXIDAGIO_H
#define AMBAIPC_FLEXIDAGIO_H

#define FLEXIDAGIO_OK    (0U)
#define FLEXIDAGIO_ERR   (1U)

#define FLEXIDAGIO_RESULT_OUTTYPE_RAW   (0U)
#define FLEXIDAGIO_RESULT_OUTTYPE_OD    (1U)
#define FLEXIDAGIO_RESULT_OUTTYPE_SEG   (2U)

#define AMBA_IPC_FLEXIDAGIO_NUM                 8U
#define AMBA_IPC_FLEXIDAGIO_MAXMSGS             32U

#define AMBA_IPC_FLEXIDAGIO_INPUT_HEAD          8U
#define AMBA_IPC_FLEXIDAGIO_INPUT_DATA          (1024U - AMBA_IPC_FLEXIDAGIO_INPUT_HEAD)

#define AMBA_IPC_FLEXIDAGIO_RESULT_HEAD         8U
#define AMBA_IPC_FLEXIDAGIO_RESULT_DATA         (1024U - AMBA_IPC_FLEXIDAGIO_RESULT_HEAD)


typedef struct{
    unsigned int Channel;
    unsigned int OutType;
} __attribute__((packed)) AMBA_IPC_FLEXIDAGIO_CONFIG_s;

typedef struct{
    unsigned int Channel;
    unsigned int DataLen;
    unsigned char  Data[AMBA_IPC_FLEXIDAGIO_INPUT_DATA];
} __attribute__((packed)) AMBA_IPC_FLEXIDAGIO_INPUT_s;

typedef struct{
    unsigned int Channel;
    unsigned int DataLen;
    unsigned char  Data[AMBA_IPC_FLEXIDAGIO_RESULT_DATA];
} __attribute__((packed)) AMBA_IPC_FLEXIDAGIO_RESULT_s;


typedef uint32_t (*AMBA_IPC_FLEXIDAGIO_CONFIG_f)(uint32_t Channel, uint32_t OutType);
typedef uint32_t (*AMBA_IPC_FLEXIDAGIO_RESULT_f)(uint32_t Channel, const void *pData, uint32_t Len);


uint32_t AmbaIPC_FlexidagIO_Init(uint32_t Channel, AMBA_IPC_FLEXIDAGIO_CONFIG_f ConfigFun);
uint32_t AmbaIPC_FlexidagIO_Deinit(uint32_t Channel);
uint32_t AmbaIPC_FlexidagIO_Delete(uint32_t Channel);
uint32_t AmbaIPC_FlexidagIO_Config(uint32_t Channel, uint32_t OutType);
uint32_t AmbaIPC_FlexidagIO_SetInput(uint32_t Channel, void *pData, uint32_t Len);
uint32_t AmbaIPC_FlexidagIO_GetInput(uint32_t Channel, void *pData, uint32_t *Len);
uint32_t AmbaIPC_FlexidagIO_GetResult_SetCB(uint32_t Channel, AMBA_IPC_FLEXIDAGIO_RESULT_f ResultFun);
uint32_t AmbaIPC_FlexidagIO_SetResult(uint32_t Channel, void *pData, uint32_t Len);
uint32_t AmbaIPC_FlexidagIO_GetResult(uint32_t Channel, void *pData, uint32_t *Len);

#endif /* AMBAIPC_FLEXIDAGIO_H */
