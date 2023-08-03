/**
 *  @file AmbaCEHU.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Definitions & Constants for CEHU (Chip Error Handling Unit) Controller Middleware APIs
 *
 */

#ifndef AMBA_CEHU_H
#define AMBA_CEHU_H

#define AMBA_NUM_CEHU_ERRORS     (155U) //!< Number of CEHU errors
#define AMBA_NUM_CEHU_INSTANCES  (2U)   //!< Number of CEHU instances

typedef void (*AMBA_CEHU_INT_CALLBACK_f)(UINT32 InstanceID); //!< Prototype of CEHU Interrupt callback function

// Defines for CEHU errors
#define CEHU_ERR_NONE           (0U)
#define CEHU_ERR_PARAM          (1U)
#define CEHU_ERR_CLR_FIRST_ERR  (2U)
#define CEHU_ERR_REACH_MAX      (3U)
#define CEHU_ERR_UNEXPECTED     (4U)
#define CEHU_ERR_MUTEX          (5U)
#define CEHU_ERR_INT_DISABLE    (6U)
#define CEHU_ERR_INT_ENABLE     (7U)
#define CEHU_ERR_INT_CONFIG     (8U)

UINT32 AmbaCEHU_MaskEnable(UINT32 InstanceID, UINT32 ErrorID);
UINT32 AmbaCEHU_MaskDisable(UINT32 InstanceID, UINT32 ErrorID);
UINT32 AmbaCEHU_MaskGet(UINT32 InstanceID, UINT32 ErrorID, UINT32 *Value);
UINT32 AmbaCEHU_ErrorGet(UINT32 InstanceID, UINT32 ErrorID, UINT32 *Value);
UINT32 AmbaCEHU_SafetyModeGet(UINT32 InstanceID, UINT32 ErrorID, UINT32 *Value);
UINT32 AmbaCEHU_SafetyModeSet(UINT32 InstanceID, UINT32 ErrorID, UINT32 Value);
UINT32 AmbaCEHU_ErrorClear(UINT32 InstanceID, UINT32 ErrorID);
UINT32 AmbaCEHU_IntCallbackRegister(AMBA_CEHU_INT_CALLBACK_f pCallBackFunc);
UINT32 AmbaCEHU_InterruptDisable(void);
UINT32 AmbaCEHU_InterruptEnable(void);

#endif /* AMBA_CEHU_H */

