/**
 * @file AmbaIPC_SD.h
 *
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @details Definitions for SD controller rpdev driver.
 *
 */

#ifndef _AMBA_IPC_SD_H_
#define _AMBA_IPC_SD_H_

/*-----------------------------------------------------------------------------------------------*\
 * Rpdev SD info data structure.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AmbaRpdev_SdInfo_s_ {
    UINT32    HostID;         /**< HostID */
    UINT32     FromRpmsg : 1,  /**< FromRpmsg */
            IsInit: 1,      /**< IsInit */
            IsSdmem : 1,    /**< IsSdmem */
            IsMmc : 1,      /**< IsMmc */
            IsSdio : 1,     /**< IsSdio */
            Rsv : 27;       /**< Rsv */

    UINT16     BusWidth;       /**< Bus Width */
    UINT16     Hcs;            /**< Hcs */
    UINT32     Rca;            /**< Rca */

    UINT32     Ocr;            /**< Ocr */
    UINT32     Clk;            /**< Clk */
} AmbaRpdev_SdInfo_s;

/*-----------------------------------------------------------------------------------------------*\
 * Rpdev SD command response data structure.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AmbaRpdev_SdResp_s_ {
    UINT32    HostID;     /**< HostID */
    UINT32     OpCode;     /**< OpCode */
    int     Ret;        /**< Ret */
    UINT32  Padding;    /**< Padding */
    UINT32     Resp[4];    /**< Resp */
    char     Buf[512];   /**< Buf */
} AmbaRpdev_SdResp_s;

/*-----------------------------------------------------------------------------------------------*\
 * Rpdev SD command enum.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_RPDEV_SD_CMD_e_ {
    SD_INFO_GET = 0,
    SD_RESP_GET,
    SD_DETECT_INSERT,
    SD_DETECT_EJECT,
    SD_DETECT_CHANGE,
    SD_RPMSG_ACK,
} AMBA_RPDEV_SD_CMD_e;

/*-----------------------------------------------------------------------------------------------*\
 * Rpdev SD message data structure.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_RPDEV_SD_MSG_s_ {
    UINT64      Cmd;    /**< Cmd */
    UINT64      Param;  /**< Param */
} AMBA_RPDEV_SD_MSG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaIPC_SD.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaIPC_SdInit(void);
int AmbaIPC_SdDetectChange(int SlotID);

#endif    /* _AMBA_IPC_SD_H_ */
