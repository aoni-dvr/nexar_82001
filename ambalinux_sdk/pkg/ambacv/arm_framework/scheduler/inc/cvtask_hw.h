/**
 *  @file cvtask_hw.h
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
 *  @details Definitions & Constants for cvtask HW
 *
 */

#ifndef CVTASK_HW_H
#define CVTASK_HW_H

extern  uint32_t                CVTASK_DRAM_END;
extern  uint32_t                AMALGAM_CMA_SIZE;
extern  uint64_t                AMALGAM_CMA_BASE;

#if (defined(CHIP_CV2) || defined(CHIP_CV22) || defined(CHIP_CV25) || defined(CHIP_CV28) || defined(CHIP_CV2A) || defined(CHIP_CV2FS) || defined(CHIP_CV22A) || defined(CHIP_CV22FS) || defined(CHIP_CV5) || defined(CHIP_CV52))
#define ORC_SYNC_COUNTER        127
#endif

#ifndef ORC_SYNC_COUNTER
#define ORC_SYNC_COUNTER        56
#endif

#define CVTASK_SYSPARA_BASE     0x00001000
#define CVTASK_SYSFLOW_SIZE     0x00010000

#define CVTASK_DRAM_BOM         0x00600000
#define CVTASK_DRAM_TOP         (CVTASK_DRAM_END - 0x100000)
#define CVTASK_DRAM_SIZE        (CVTASK_DRAM_TOP - CVTASK_DRAM_BOM)

#define SCHDR_QBUFF_BASE        (CVTASK_DRAM_TOP)
#define SCHDR_QCONF_BASE        (CVTASK_DRAM_END - 0x1000)

#endif  //CVTASK_HW_H
