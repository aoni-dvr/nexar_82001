/**
 *  @file AmbaUserGpioConfig.c
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
 *  @details GPIO pin configuration
 *
 */

#include "AmbaGPIO.h"

extern const AMBA_GPIO_DEFAULT_s GpioPinGrpConfig;
const AMBA_GPIO_DEFAULT_s GpioPinGrpConfig = {
    .PinGroupRegVal = {
        [AMBA_GPIO_GROUP0] = {
            .PinFuncRegVal = {
                [0] = 0xffffff00U,                  /* GPIO pin [31:0]: GPIO(0) or Alternate functions */
                [1] = 0x00000000U,                  /* GPIO pin [31:0]: GPIO(0) or Alternate functions */
                [2] = 0x00000000U,                  /* GPIO pin [31:0]: GPIO(0) or Alternate functions */
            },
            .PinIoTypeRegVal = 0x000000ffU,         /* GPIO pin [31:0]: Input(0) or Output(1) pin */
            .PinStateProtectRegVal = 0xffffffffU,   /* GPIO pin [31:0]: Read-only(0) or Read-writeable(1) pin state */
            .PinStateRegVal = 0x00000000U,          /* GPIO pin [31:0]: Low(0) or High(1) pin state */
            .PinPullFuncEnableRegVal = 0x00000000U,
            .PinPullFuncTypeRegVal = 0x00000000U,
            .PinDriverStrengthRegVal = {
                [0] = 0xffffffffU,
                [1] = 0x00000000U
            }
        },

        [AMBA_GPIO_GROUP1] = {
            .PinFuncRegVal = {
                [0] = 0x003fffffU,                  /* GPIO pin [63:32]: GPIO(0) or Alternate functions */
                [1] = 0x00000000U,                  /* GPIO pin [63:32]: GPIO(0) or Alternate functions */
                [2] = 0xffc00000U,                  /* GPIO pin [63:32]: GPIO(0) or Alternate functions */
            },
            .PinIoTypeRegVal = 0x00000000U,         /* GPIO pin [63:32]: Input(0) or Output(1) pin */
            .PinStateProtectRegVal = 0xffffffffU,   /* GPIO pin [63:32]: Read-only(0) or Read-writeable(1) pin state */
            .PinStateRegVal =  0x00000000U,         /* GPIO pin [63:32]: Low(0) or High(1) pin state */
            .PinPullFuncEnableRegVal = 0x00000000U,
            .PinPullFuncTypeRegVal = 0x00000000U,
            .PinDriverStrengthRegVal = {
                [0] = 0xffffffffU,
                [1] = 0x00000000U
            }
        },

#if defined(CONFIG_ENABLE_SPINOR_BOOT)
        [AMBA_GPIO_GROUP2] = {
            .PinFuncRegVal = {
                [0] = 0x00ffe180U,                  /* GPIO pin [95:64]: GPIO(0) or Alternate functions */
                [1] = 0xffffffc0U,                  /* GPIO pin [95:64]: GPIO(0) or Alternate functions */
                [2] = 0x0000003bU,                  /* GPIO pin [95:64]: GPIO(0) or Alternate functions */
            },
            .PinIoTypeRegVal = 0x00000004U,         /* GPIO pin [95:64]: Input(0) or Output(1) pin */
            .PinStateProtectRegVal = 0xffffffffU,   /* GPIO pin [95:64]: Read-only(0) or Read-writeable(1) pin state */
            .PinStateRegVal = 0x00000000U,          /* GPIO pin [95:64]: Low(0) or High(1) pin state */
            .PinPullFuncEnableRegVal = 0x00000000U,
            .PinPullFuncTypeRegVal = 0x00000000U,
            .PinDriverStrengthRegVal = {
                [0] = 0xffffffffU,
                [1] = 0x00000000U
            }
        },
#else
        [AMBA_GPIO_GROUP2] = {
            .PinFuncRegVal = {
                [0] = 0x00000000U,                  /* GPIO pin [95:64]: GPIO(0) or Alternate functions */
                [1] = 0xffffffc0U,                  /* GPIO pin [95:64]: GPIO(0) or Alternate functions */
                [2] = 0x0000003bU,                  /* GPIO pin [95:64]: GPIO(0) or Alternate functions */
            },
            .PinIoTypeRegVal = 0x00000004U,         /* GPIO pin [95:64]: Input(0) or Output(1) pin */
            .PinStateProtectRegVal = 0xffffffffU,   /* GPIO pin [95:64]: Read-only(0) or Read-writeable(1) pin state */
            .PinStateRegVal = 0x00000000U,          /* GPIO pin [95:64]: Low(0) or High(1) pin state */
            .PinPullFuncEnableRegVal = 0x00000000U,
            .PinPullFuncTypeRegVal = 0x00000000U,
            .PinDriverStrengthRegVal = {
                [0] = 0xffffffffU,
                [1] = 0x00000000U
            }
        },
#endif
        [AMBA_GPIO_GROUP3] = {
            .PinFuncRegVal = {
                [0] = 0xffe98000U,                  /* GPIO pin [127:96]: GPIO(0) or Alternate functions */
                [1] = 0x00107fffU,                  /* GPIO pin [127:96]: GPIO(0) or Alternate functions */
                [2] = 0x00000000U,                  /* GPIO pin [127:96]: GPIO(0) or Alternate functions */
            },
            .PinIoTypeRegVal = 0x00020000U,         /* GPIO pin [127:96]: Input(0) or Output(1) pin */
            .PinStateProtectRegVal = 0xffffffffU,   /* GPIO pin [127:96]: Read-only(0) or Read-writeable(1) pin state */
            .PinStateRegVal = 0x00000000U,          /* GPIO pin [127:96]: Low(0) or High(1) pin state */
            .PinPullFuncEnableRegVal = 0x00000007U,
            .PinPullFuncTypeRegVal = 0x00000007U,
            .PinDriverStrengthRegVal = {
                [0] = 0xffffffffU,
                [1] = 0x00000000U
            }
        },

        [AMBA_GPIO_GROUP4] = {
            .PinFuncRegVal = {
                [0] = 0x3fffffffU,                  /* GPIO pin [159:128]: GPIO(0) or Alternate functions */
                [1] = 0x00000000U,                  /* GPIO pin [159:128]: GPIO(0) or Alternate functions */
                [2] = 0x00000000U,                  /* GPIO pin [159:128]: GPIO(0) or Alternate functions */
            },
            .PinIoTypeRegVal = 0xDC000000U,         /* GPIO pin [159:128]: Input(0) or Output(1) pin */
            .PinStateProtectRegVal = 0xffffffffU,   /* GPIO pin [159:128]: Read-only(0) or Read-writeable(1) pin state */
            .PinStateRegVal = 0x00000000U,          /* GPIO pin [159:128]: Low(0) or High(1) pin state */
            .PinPullFuncEnableRegVal = 0x00000000U,
            .PinPullFuncTypeRegVal = 0x00000000U,
            .PinDriverStrengthRegVal = {
                [0] = 0xffffffffU,
                [1] = 0x00000000U
            }
        }
    }
};

