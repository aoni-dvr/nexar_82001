/**
 *  @file AmbaMonWatchdog.h
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Constants and Definitions for Amba Monitor Watchdog
 *
 */

#ifndef AMBA_MONITOR_WATCHDOG_H
#define AMBA_MONITOR_WATCHDOG_H

typedef struct /*_AMBA_MON_WATCHDOG_PORT_LINK_s_*/ {
    struct AMBA_MON_WATCHDOG_PORT_s_    *Up;
    struct AMBA_MON_WATCHDOG_PORT_s_    *Down;
} AMBA_MON_WATCHDOG_PORT_LINK_s;

typedef struct AMBA_MON_WATCHDOG_PORT_s_ {
    UINT32                            Magic;
    AMBA_KAL_MUTEX_t                  Mutex;
    UINT32                            Period;
    UINT32                            Time2Wait;
    void                              *pUser;
    struct AMBA_MON_WATCHDOG_ID_s_    *pWdogId;
    AMBA_MON_WATCHDOG_PORT_LINK_s     WaitLink;
    AMBA_MON_WATCHDOG_PORT_LINK_s     Link;
} AMBA_MON_WATCHDOG_PORT_s;

typedef struct /*_AMBA_MON_WATCHDOG_ID_LINK_s_*/ {
    struct AMBA_MON_WATCHDOG_ID_s_    *Up;
    struct AMBA_MON_WATCHDOG_ID_s_    *Down;
} AMBA_MON_WATCHDOG_ID_LINK_s;

typedef struct AMBA_MON_WATCHDOG_ID_s_ {
    UINT32                         Magic;
    AMBA_KAL_MUTEX_t               Mutex;
    const char                     *pName;
    AMBA_MON_WATCHDOG_PORT_s       *pPortList;
    AMBA_MON_WATCHDOG_ID_LINK_s    Link;
} AMBA_MON_WATCHDOG_ID_s;

UINT32 AmbaMonWatchdog_Init(void);

#endif /* AMBA_MONITOR_WATCHDOG_H */
