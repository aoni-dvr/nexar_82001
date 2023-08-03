/**
 * @file AmbaIPC_LinkCtrl.h
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
 * @details LinkCtrl is a RPMSG device to cowork with Linux for handling LinkCtrl commands, e.g. reboot, hibernation.
 *
 */

#ifndef AMBAIPC_LINKCTRL_H
#define AMBAIPC_LINKCTRL_H


#if defined __cplusplus
extern "C" {
#endif

    /*-----------------------------------------------------------------------------------------------*\
     * LinkCtrl of Rpdev functions.
    \*-----------------------------------------------------------------------------------------------*/
    typedef enum _AMBA_RPDEV_LINK_CTRL_CMD_e_ {
        LINK_CTRL_CMD_SUSPEND_PREPARE = 0,
        LINK_CTRL_CMD_SUSPEND_DONE,
        LINK_CTRL_CMD_SUSPEND_EXIT,
        LINK_CTRL_CMD_SUSPEND_ACK,
        LINK_CTRL_CMD_SUSPEND,
        LINK_CTRL_CMD_GPIO_LINUX_ONLY_LIST,
        LINK_CTRL_CMD_GET_MEM_INFO,
        LINK_CTRL_CMD_SET_RTOS_MEM,
        LINK_CTRL_CMD_SET_WIFI_CONF,
        LINK_CTRL_CMD_REBOOT
    } AMBA_RPDEV_LINK_CTRL_CMD_e;

    typedef struct _AMBA_RPDEV_LINK_CTRL_CMD_s_ {
        UINT32  Cmd;    /**< Cmd */
        UINT64  Param1; /**< Param1 */
        UINT64  Param2; /**< Param2 */
    } AMBA_RPDEV_LINK_CTRL_CMD_s;

    typedef struct _LINK_CTRL_MEMINFO_CMD_s_ {
        UINT32  Cmd;                /**< Cmd */
        UINT64  RtosStart;          /**< RtosStart */
        UINT64  RtosEnd;            /**< RtosEnd */
        UINT64  RtosSystemStart;    /**< RtosSystemStart */
        UINT64  RtosSystemEnd;      /**< RtosSystemEnd */
        UINT64  CachedHeapStart;    /**< CachedHeapStart */
        UINT64  CachedHeapEnd;      /**< CachedHeapEnd */
        UINT64  NonCachedHeapStart; /**< NonCachedHeapStart */
        UINT64  NonCachedHeapEnd;   /**< NonCachedHeapEnd */
    } LINK_CTRL_MEMINFO_CMD_s;

    typedef struct _LINK_CTRL_WIFI_CONF_s_ {
        UINT32  Cmd;    /**< Cmd */
        char Buf[512];  /**< Buf */
    } LINK_CTRL_WIFI_CONF_s;

    typedef struct _AMBA_RPDEV_LINK_CTRL_OBJ_s_ {
        const char          *pName;     /**< pName */
        AMBA_IPC_HANDLE     Channel;    /**< Channel */
    } AMBA_RPDEV_LINK_CTRL_OBJ_s;



    /**
     * @brief Initialize LinkCtrl RPMSG device to cowork with Linux for handling LinkCtrl commands, e.g. reboot, hibernation.
     */
    void AmbaIPC_LinkCtrl_Init(void);

#ifdef CONFIG_AMBALINK_RPMSG_HIBER
    /**
     *  @brief  Ack Linux that ThreadX is done.
     *  @return 0 - OK, others - NG
     */
    INT32 AmbaIPC_LinkCtrl_RpmsgAck(void);
#endif /* CONFIG_AMBALINK_RPMSG_HIBER */

#if defined __cplusplus
}
#endif

#endif  /* AMBAIPC_LINKCTRL_H */

