/**
 *  @file AmbaFlexidagIO.h
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
 *  @details Header of AmbaFlexidagIO interface
 *
 */

#ifndef AMBAFLEXIDAGIO_H
#define AMBAFLEXIDAGIO_H

#define FLEXIDAGIO_OK    (0U)
#define FLEXIDAGIO_ERR   (1U)

#define FLEXIDAGIO_RESULT_OUTTYPE_RAW   (0U)
#define FLEXIDAGIO_RESULT_OUTTYPE_OD    (1U)
#define FLEXIDAGIO_RESULT_OUTTYPE_SEG   (2U)

#ifdef AMBAIPC
unsigned int AmbaIPC_FlexidagIO_Init(unsigned int Channel);
unsigned int AmbaIPC_FlexidagIO_Deinit(unsigned int Channel);
unsigned int AmbaIPC_FlexidagIO_Config(unsigned int Channel, unsigned int OutType);
unsigned int AmbaIPC_FlexidagIO_GetInput(unsigned int Channel, void *pData, unsigned int *Len);
unsigned int AmbaIPC_FlexidagIO_SetResult(unsigned int Channel, void *pData, unsigned int Len);

#else
#define AMBA_IPC_FLEXIDAGIO_NUM                 8U
#define AMBA_IPC_FLEXIDAGIO_MAXMSGS             32U

#define AMBA_IPC_FLEXIDAGIO_INPUT_HEAD          8U
#define AMBA_IPC_FLEXIDAGIO_INPUT_DATA          (1024U - AMBA_IPC_FLEXIDAGIO_INPUT_HEAD)

#define AMBA_IPC_FLEXIDAGIO_RESULT_HEAD         8U
#define AMBA_IPC_FLEXIDAGIO_RESULT_DATA         (1024U - AMBA_IPC_FLEXIDAGIO_RESULT_HEAD)


typedef struct {
    unsigned int Channel;
    unsigned int OutType;
} __attribute__((packed)) AMBA_IPC_FLEXIDAGIO_CONFIG_s;

typedef struct {
    unsigned int Channel;
    unsigned int DataLen;
    unsigned char  Data[AMBA_IPC_FLEXIDAGIO_INPUT_DATA];
} __attribute__((packed)) AMBA_IPC_FLEXIDAGIO_INPUT_s;

typedef struct {
    unsigned int Channel;
    unsigned int DataLen;
    unsigned char  Data[AMBA_IPC_FLEXIDAGIO_RESULT_DATA];
} __attribute__((packed)) AMBA_IPC_FLEXIDAGIO_RESULT_s;


typedef unsigned int (*AMBA_IPC_FLEXIDAGIO_CONFIG_f)(unsigned int Channel, unsigned int OutType);
typedef unsigned int (*AMBA_IPC_FLEXIDAGIO_RESULT_f)(unsigned int Channel, const void *pData, unsigned int Len);


unsigned int AmbaIPC_FlexidagIO_Init(unsigned int Channel);
unsigned int AmbaIPC_FlexidagIO_Deinit(unsigned int Channel);
unsigned int AmbaIPC_FlexidagIO_Config(unsigned int Channel, unsigned int OutType);
unsigned int AmbaIPC_FlexidagIO_SetInput(unsigned int Channel, void *pData, unsigned int Len);
unsigned int AmbaIPC_FlexidagIO_GetInput(unsigned int Channel, void *pData, unsigned int *Len);
unsigned int AmbaIPC_FlexidagIO_GetResult_SetCB(unsigned int Channel, AMBA_IPC_FLEXIDAGIO_RESULT_f ResultFun);
unsigned int AmbaIPC_FlexidagIO_SetResult(unsigned int Channel, void *pData, unsigned int Len);
unsigned int AmbaIPC_FlexidagIO_GetResult(unsigned int Channel, void *pData, unsigned int *Len);
#endif

#endif /* AMBAFLEXIDAGIO_H */

