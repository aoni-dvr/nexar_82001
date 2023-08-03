/**
 *  @file AmbaIRIF_Def.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for InfraRed Interface APIs
 *
 */

#ifndef AMBA_IRIF_DEF_H
#define AMBA_IRIF_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#define IRIF_ERR_0000           (IR_ERR_BASE)           /* Invalid argument */
#define IRIF_ERR_0001           (IR_ERR_BASE + 0X1U)    /* Unable to do concurrency protection */
#define IRIF_ERR_0002           (IR_ERR_BASE + 0X2U)    /* Timeout occurred */
#define IRIF_ERR_00FF           (IR_ERR_BASE + 0XFFU)   /* Unexpected error */

/* IRIF error values */
#define IRIF_ERR_NONE           (OK)
#define IRIF_ERR_ARG            IRIF_ERR_0000
#define IRIF_ERR_MUTEX          IRIF_ERR_0001
#define IRIF_ERR_TIMEOUT        IRIF_ERR_0002
#define IRIF_ERR_UNEXPECTED     IRIF_ERR_00FF

typedef void (*AMBA_IRIF_ISR_f)(void);

#endif /* AMBA_IRIF_DEF_H */
