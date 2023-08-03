/**
 *  @file AmbaDDRC_Def.h
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
 *  @details Definitions & Constants for DRAM controller APIs
 *
 */

#ifndef AMBA_DDRC_DEF_H
#define AMBA_DDRC_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

/* DRAM error values */
#define DDRC_ERR_0000          (DRAM_ERR_BASE)             /* Invalid argument */
#define DDRC_ERR_0001          (DRAM_ERR_BASE + 1U)        /* Unexpected error */
#define DDRC_ERR_0002          (DRAM_ERR_BASE + 2U)        /* Address translation error */
#define DDRC_ERR_0003          (DRAM_ERR_BASE + 3U)        /* Address translation error */
#define DDRC_ERR_0004          (DRAM_ERR_BASE + 4U)

#define DDRC_ERR_NONE          OK
#define DDRC_ERR_ARG           DDRC_ERR_0000
#define DDRC_ERR_UNEXPECTED    DDRC_ERR_0001
#define DDRC_ERR_AT            DDRC_ERR_0002
#define DDRC_ERR_MUTEX         DDRC_ERR_0003
#define DDRC_ERR_IO            DDRC_ERR_0004

#endif /* AMBA_DDRC_DEF_H */
