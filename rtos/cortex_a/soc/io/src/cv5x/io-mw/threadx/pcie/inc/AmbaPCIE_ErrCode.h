/**
 *  @file AmbaPCIE_ErrCode.h
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
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details PCIe error code header file.
 */

#ifndef AMBAPCIE_ERRCODE_H
#define AMBAPCIE_ERRCODE_H

#ifndef AMBA_ERROR_CODE_H
#include <AmbaErrorCode.h>
#endif


#define EOK             0      /* no error */

#define PCIE_ERR_BASE 0xFFU // TBD

#define PCIE_ERR_START                   PCIE_ERR_BASE
#define PCIE_ERR_SUCCESS                 0U
#define PCIE_ERR_ARG                     (PCIE_ERR_START + 0x16U) //invalid argument
#define PCIE_ERR_INPROGRESS              (PCIE_ERR_START + 0x73U)
#define PCIE_ERR_FLOW                    (PCIE_ERR_START + 0xFFU)
#define PCIE_ERR_FAIL                    (PCIE_ERR_START + 0xFFFFU)

#define PCIE_ERR_0001       PCIE_ERR_ARG
#define PCIE_ERR_FFFF       PCIE_ERR_FAIL


#endif /* AMBAPCIE_ERRCODE_H */
