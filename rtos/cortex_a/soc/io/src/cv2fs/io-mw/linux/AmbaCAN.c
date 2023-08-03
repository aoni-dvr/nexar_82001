/**
 *  @file AmbaCAN.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details CAN bus control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaCAN.h"

/**
* This function is used to start a specific CAN controller with given configurations
* @param [in]  Channel ID
* @param [in]  CAN configuration
* @return ErrorCode
*/
UINT32 AmbaCAN_Enable(UINT32 CanCh, const AMBA_CAN_CONFIG_s *pCanConfig)
{
    /* FIXME */
    (void)CanCh;
    (void)pCanConfig;
    return 0;
}

/**
* This function is used to receive a CAN message
* @param [in]  Channel ID
* @param [in/out]  Pointer to receiving contents
* @param [in]  Receiving timeout in ms
* @return ErrorCode
*/
UINT32 AmbaCAN_Read(UINT32 CanCh, AMBA_CAN_MSG_s *pMessage, UINT32 Timeout)
{
    /* FIXME */
    (void)CanCh;
    (void)pMessage;
    (void)Timeout;
    return 0;
}

/**
* This function is used to receive a CAN FD message
* @param [in]  Channel ID
* @param [in/out]  Pointer to receiving contents
* @param [in]  Receiving timeout in ms
* @return ErrorCode
*/
UINT32 AmbaCAN_ReadFd(UINT32 CanCh, AMBA_CAN_FD_MSG_s *pFdMessage, UINT32 Timeout)
{
    /* FIXME */
    (void)CanCh;
    (void)pFdMessage;
    (void)Timeout;
    return 0;
}

/**
* This function is used to send a CAN message.
* @param [in]  Channel ID
* @param [in]  Pointer to sending contents
* @param [in]  Sending timeout in ms
* @return ErrorCode
*/
UINT32 AmbaCAN_Write(UINT32 CanCh, const AMBA_CAN_MSG_s *pMessage, UINT32 Timeout)
{
    /* FIXME */
    (void)CanCh;
    (void)pMessage;
    (void)Timeout;
    return 0;
}

/**
* This function is used to send a CAN FD message.
* @param [in]  Channel ID
* @param [in]  Pointer to sending contents
* @param [in]  Sending timeout in ms
* @return ErrorCode
*/
UINT32 AmbaCAN_WriteFd(UINT32 CanCh, const AMBA_CAN_FD_MSG_s *pFdMessage, UINT32 Timeout)
{
    /* FIXME */
    (void)CanCh;
    (void)pFdMessage;
    (void)Timeout;
    return 0;
}

/**
* This function is used to stop a specific CAN controller.
* @param [in]  Channel ID
* @return ErrorCode
*/
UINT32 AmbaCAN_Disable(UINT32 CanCh)
{
    /* FIXME */
    (void)CanCh;
    return 0;
}

/**
* This function is used to acquire a specific CAN controller's configuration,
* which contains bit setting, filter number and filter info.
* @param [in]  Channel ID
* @param [in/out]  Pointer to the bit setting
* @param [in/out]  Pointer to the filter number
* @param [in/out]  Channel ID Pointer to the filter info
* @return ErrorCode
*/
UINT32 AmbaCAN_GetInfo(UINT32 CanCh, AMBA_CAN_BIT_INFO_s * pBitInfo, UINT32 * pNumFilter, AMBA_CAN_FILTER_s * pFilter)
{
    /* FIXME */
    (void)CanCh;
    (void)pBitInfo;
    (void)pNumFilter;
    (void)pFilter;
    return 0;
}

