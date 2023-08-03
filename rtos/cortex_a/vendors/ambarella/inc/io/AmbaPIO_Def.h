/**
 * @file AmbaPIO_Def.h
 * Definitions & Constants
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHPIOER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AMBA_PIO_DEF_H
#define AMBA_PIO_DEF_H

/* PIO error values */
#define PIO_ERR_NONE            0U
#define PIO_MODULE_ID           ((UINT16)(PIO_ERR_BASE >> 16U))
#define PIO_ERR_ENOMEM          (PIO_ERR_BASE + 12U)     /* Out of memory */
#define PIO_ERR_EINVAL          (PIO_ERR_BASE + 22U)     /* Invalid argument */
#define PIO_ERR_ENODATA         (PIO_ERR_BASE + 61U)     /* No data available */
#define PIO_ERR_ETIMEDOUT       (PIO_ERR_BASE + 110U)    /* timed out */
#define PIO_ERR_OSERR           (PIO_ERR_BASE + 500U)    /* System resource not available */

typedef struct {
    ULONG   PhyAddr;
    ULONG   VirAddr;
    UINT32  Size;
    UINT32  Cacheable;
} AMBA_PIO_s;

#endif /* AMBA_PIO_DEF_H */
