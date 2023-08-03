/**
 *  @file AmbaCAN.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
#include "hw/ambarella_can.h"
#include "AmbaMMU.h"
#include "AmbaMisraFix.h"


/**
* This function is used to start a specific CAN controller with given configurations
* @param [in]  Channel ID
* @param [in]  CAN configuration
* @return ErrorCode
*/
UINT32 AmbaCAN_Enable(UINT32 CanCh, const AMBA_CAN_CONFIG_s *pCanConfig)
{
    UINT32 Ret;
    int fd, err = 0;
    can_enable_t enable;

    //NOTE: check null pointer here. The wrapper check data contents.
    if (pCanConfig == NULL) {
        Ret = CAN_ERR_ARG;
    } else {
        fd = open("/dev/can", O_RDWR);
        if (fd != -1) {
            enable.CanCh = CanCh;
            AmbaWrap_memcpy(&enable.Config, pCanConfig, sizeof(AMBA_CAN_CONFIG_s));
            err = devctl(fd, DCMD_CAN_ENABLE, &enable, sizeof(can_enable_t), NULL);
            if (err) {
                printf("DCMD_CAN_ENABLE error, err = 0x%x \n", err);
                Ret = CAN_ERR_ARG;
            } else {
                Ret = enable.Ret;
            }
            close(fd);
        } else {
            printf("Failed to open can \n");
            Ret = CAN_ERR_ARG;//TBD
        }
    }

    return Ret;
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
    UINT32 Ret;
    int fd, err = 0;
    can_msg_t msg;

    //NOTE: check null pointer here. The wrapper check data contents.
    if (pMessage == NULL) {
        Ret = CAN_ERR_ARG;
    } else {
        fd = open("/dev/can", O_RDWR);
        if (fd != -1) {
            msg.CanCh = CanCh;
            msg.Timeout = Timeout;
            err = devctl(fd, DCMD_CAN_READ, &msg, sizeof(can_msg_t), NULL);
            if (err) {
                printf("DCMD_CAN_READ error, err = 0x%x \n", err);
                Ret = CAN_ERR_ARG;
            } else {
                AmbaWrap_memcpy(pMessage, &msg.Message, sizeof(AMBA_CAN_MSG_s));
                Ret = msg.Ret;
            }
            close(fd);
        } else {
            printf("Failed to open can \n");
            Ret = CAN_ERR_ARG;//TBD
        }
    }

    return Ret;
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
    UINT32 Ret;
    int fd, err = 0;
    can_fd_msg_t msg;

    //NOTE: check null pointer here. The wrapper check data contents.
    if (pFdMessage == NULL) {
        Ret = CAN_ERR_ARG;

    } else {
        fd = open("/dev/can", O_RDWR);
        if (fd != -1) {
            msg.CanCh = CanCh;
            msg.Timeout = Timeout;
            err = devctl(fd, DCMD_CAN_FD_READ, &msg, sizeof(can_fd_msg_t), NULL);
            if (err) {
                printf("DCMD_CAN_FD_READ error, err = 0x%x \n", err);
                Ret = CAN_ERR_ARG;
            } else {
                AmbaWrap_memcpy(pFdMessage, &msg.FdMessage, sizeof(AMBA_CAN_FD_MSG_s));
                Ret = msg.Ret;
            }
            close(fd);
        } else {
            printf("Failed to open can \n");
            Ret = CAN_ERR_ARG;//TBD
        }
    }

    return Ret;
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
    UINT32 Ret;
    int fd, err = 0;
    can_msg_t msg;

    //NOTE: check null pointer here. The wrapper check data contents.
    if (pMessage == NULL) {
        Ret = CAN_ERR_ARG;

    } else {
        fd = open("/dev/can", O_RDWR);
        if (fd != -1) {
            msg.CanCh = CanCh;
            msg.Timeout = Timeout;
            AmbaWrap_memcpy(&msg.Message, pMessage, sizeof(AMBA_CAN_MSG_s));
            err = devctl(fd, DCMD_CAN_WRITE, &msg, sizeof(can_msg_t), NULL);
            if (err) {
                printf("DCMD_CAN_WRITE error \n");
                Ret = CAN_ERR_ARG;
            } else {
                Ret = msg.Ret;
            }
            close(fd);
        } else {
            printf("Failed to open can \n");
            Ret = CAN_ERR_ARG;//TBD
        }
    }

    return Ret;
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
    UINT32 Ret;
    int fd, err = 0;
    can_fd_msg_t msg;

    //NOTE: check null pointer here. The wrapper check data contents.
    if (pFdMessage == NULL) {
        Ret = CAN_ERR_ARG;
    } else {
        fd = open("/dev/can", O_RDWR);
        if (fd != -1) {
            msg.CanCh = CanCh;
            msg.Timeout = Timeout;
            AmbaWrap_memcpy(&msg.FdMessage, pFdMessage, sizeof(AMBA_CAN_FD_MSG_s));
            err = devctl(fd, DCMD_CAN_FD_WRITE, &msg, sizeof(can_fd_msg_t), NULL);
            if (err) {
                printf("DCMD_CAN_FD_WRITE error \n");
                Ret = CAN_ERR_ARG;
            } else {
                Ret = msg.Ret;
            }
            close(fd);
        } else {
            printf("Failed to open can \n");
            Ret = CAN_ERR_ARG;//TBD
        }

    }

    return Ret;
}

/**
* This function is used to stop a specific CAN controller.
* @param [in]  Channel ID
* @return ErrorCode
*/
UINT32 AmbaCAN_Disable(UINT32 CanCh)
{
    UINT32 Ret;
    int fd, err = 0;
    can_disable_t disable;

    //NOTE: check null pointer here. The wrapper check data contents.

    fd = open("/dev/can", O_RDWR);
    if (fd != -1) {
        disable.CanCh = CanCh;
        err = devctl(fd, DCMD_CAN_DISABLE, &disable, sizeof(can_disable_t), NULL);
        if (err) {
            printf("DCMD_CAN_DISABLE error, err = 0x%x \n", err);
            Ret = CAN_ERR_ARG;
        } else {
            Ret = disable.Ret;
        }
        close(fd);
    } else {
        printf("Failed to open can \n");
        Ret = CAN_ERR_ARG;//TBD
    }

    return Ret;
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
    UINT32 Ret;
    int fd, err = 0;
    can_info_t info;

    if ((pBitInfo == NULL) || (pNumFilter == NULL) || (pFilter == NULL)) {
        Ret = CAN_ERR_ARG;
    } else {
        fd = open("/dev/can", O_RDWR);
        if (fd != -1) {
            info.CanCh = CanCh;
            err = devctl(fd, DCMD_CAN_GET_INFO, &info, sizeof(can_info_t), NULL);
            if (err) {
                printf("DCMD_CAN_GET_INFO error, err = 0x%x \n", err);
                Ret = CAN_ERR_ARG;
            } else {
                AmbaWrap_memcpy(pBitInfo, &info.BitInfo, sizeof(AMBA_CAN_BIT_INFO_s));
                *pNumFilter = info.NumFilter;
                AmbaWrap_memcpy(pBitInfo, &info.Filter, sizeof(AMBA_CAN_FILTER_s) * info.NumFilter);
                Ret = info.Ret;
            }
            close(fd);
        } else {
            printf("Failed to open can \n");
            Ret = CAN_ERR_ARG;
        }
    }

    return Ret;
}

