/**
 * @file AmbaIPC_Rpc.c
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
 * @details The RPC module of Ambarella Network Support
 *
 */

#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaLinkInternal.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_List.h"
#ifdef SAM
#include "AmbaCardManager.h"
#endif
#include "AmbaIPC_Rpc.h"
#include "AmbaIPC_RpcProg_LU_Util.h"
#include "AmbaCSL_TMR.h"
#include "libwrapc.h"

#define HASH_TABLE_SIZE 128
#define EXEC_BUF        256
typedef int (*AMBA_IPC_XPRT_f)(AMBA_IPC_PKT_s *, int);

static AMBA_IPC_LIST_s  port_head[HASH_TABLE_SIZE] __attribute__((section(".bss.noinit")));
static UINT32           global_port_id __attribute__((section(".bss.noinit")));
static AMBA_KAL_MUTEX_t global_port_lock __attribute__((section(".bss.noinit")));
static AMBA_IPC_XPRT_f  xprt_func[AMBA_IPC_HOST_MAX] __attribute__((section(".bss.noinit")));
static AMBA_IPC_HANDLE  rpmsg_chnl_linux __attribute__((section(".bss.noinit")));
static CLIENT_ID_t      script_hdr_clnt __attribute__((section(".bss.noinit")));

extern AMBA_KAL_EVENT_FLAG_t AmbaLinkEventFlag;

#ifdef RPC_DEBUG
AMBA_RPC_STATISTIC_s *rpc_stat __attribute__((section(".bss.noinit")));
extern AMBA_LINK_CTRL_s AmbaLinkCtrl;
extern unsigned int AmbaRTSL_PllGetApbClk(void);
#define RPC_PROFILING_ADDR        AmbaLinkCtrl.AmbaRpcProfileAddr
#endif

static inline void AmbaIPC_Rpc_clrbss(void)
{
    memset(port_head, 0, sizeof(port_head));
    memset(&global_port_id, 0, sizeof(global_port_id));
    memset(&global_port_lock, 0, sizeof(global_port_lock));
    memset(&xprt_func, 0, sizeof(xprt_func));

    rpmsg_chnl_linux = NULL;
    script_hdr_clnt = NULL;

#ifdef RPC_DEBUG
    rpc_stat = NULL;
#endif
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  port_hash
 *
 *  @Description::
 *
 *  @Input      ::
 *      id : port ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*----------------------------------------------------------------------------*/
static inline int port_hash(int id)
{
    return id & (HASH_TABLE_SIZE-1);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  find_port
 *
 *  @Description::  find a local port handle by ID number
 *
 *  @Input      ::
 *      id : port ID
  *
 *  @Output     ::
 *      AMBA_IPC_PORT_s :
 *
 *  @Return     ::
 *
\*----------------------------------------------------------------------------*/
AMBA_IPC_PORT_s *find_port(int id)
{
    AMBA_IPC_PORT_s *port;
    int hash = port_hash(id);
    AMBA_IPC_LIST_s *head = &port_head[hash];

    list_for_each_entry(port, head, AMBA_IPC_PORT_s, list)
        if (port->id == id)
            return port;

    return NULL;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  xprt_init_port
 *
 *  @Description::  initialize a port
 *
 *  @Input      ::
 *      port :   port pointer
 *      context: parameter for callback
 *      cb:      callback when data is available
 *
 *  @Output     ::
 *
 *
 *  @Return     ::
 *
\*----------------------------------------------------------------------------*/
int xprt_init_port(AMBA_IPC_PORT_s *port, void *context, PORT_RECV_f cb)
{
    int id = 0;

    AmbaKAL_MutexTake(&global_port_lock, AMBA_KAL_WAIT_FOREVER);
    if (port->id == 0) {
        // find a unique ID
        while (1) {
            // find an availalbe port id
            id = global_port_id++;
            if (global_port_id > 0x7FFFFFF0)
                global_port_id = HASH_TABLE_SIZE;
            if (!find_port(id))
                break;
        }
        port->id = id;
    }

    port->context = context;
    port->recv = cb;
    AmbaKAL_MutexCreate(&port->lock,"Xprt");
    AmbaKAL_EventFlagCreate(&port->flag,"XprtFlag");
    AmbaIPC_ListAdd(&port->list, &port_head[port_hash(port->id)]);
    AmbaKAL_MutexGive(&global_port_lock);

    return id;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  xprt_exit_port
 *
 *  @Description::  release a port resource
 *
 *  @Input      ::
 *      port :   port pointer
 *
 *  @Output     ::
 *
 *
 *  @Return     ::
 *
\*----------------------------------------------------------------------------*/
void xprt_exit_port(AMBA_IPC_PORT_s *port)
{
    if (port) {
        AmbaKAL_MutexTake(&global_port_lock, AMBA_KAL_WAIT_FOREVER);
        AmbaKAL_MutexDelete(&port->lock);
        AmbaIPC_ListDel(&port->list);
        AmbaKAL_EventFlagDelete(&port->flag);
        AmbaKAL_MutexGive(&global_port_lock);
    }
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  process_pkt
 *
 *  @Description::  process an incoming packet
 *
 *  @Input      ::
 *      pkt :   packet pointer
 *      len :   packet length
 *
 *  @Output     ::
 *
 *
 *  @Return     ::
 *
\*----------------------------------------------------------------------------*/
static int process_pkt(AMBA_IPC_PKT_s *pkt, int len)
{
    int port_id;
    AMBA_IPC_PORT_s *port;
#ifdef RPC_DEBUG
    pkt->xprt.tx_rpc_recv_start = AmbaCSL_TmrGetCurrentVal(PROFILE_TIMER);
#endif

    if (pkt->msg.type == AMBA_IPC_MSG_CALL) {
        port_id = pkt->xprt.server_port;
        port = find_port(port_id);
        if (!port) {
            /* check the svc port with binder */
            port = find_port(AMBA_IPC_BINDING_PORT);
            pkt->xprt.private = pkt->msg.u.call.proc; /* temporarily record the original proc */
            pkt->msg.u.call.proc = AMBA_IPC_BINDER_REBIND;
        }
    } else {
        port_id = pkt->xprt.client_port;
        port = find_port(port_id);
        if (!port) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "zombie reply message with port id %d\n", port_id, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            return 0;
        }
    }
    return port->recv(port->context, pkt, len);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  linux_recv
 *
 *  @Description::  forward incoming package from linux for processing
 *
 *  @Input      ::
 *      IpcHandle :   rpmsg channel handle
 *      AMBA_IPC_MSG_CTRL_s :  message
 *
 *  @Output     ::
 *
 *
 *  @Return     ::
 *
\*----------------------------------------------------------------------------*/
static int linux_recv(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    UNUSED(IpcHandle);
    process_pkt(pMsgCtrl->pMsgData, pMsgCtrl->Length - sizeof(AMBA_IPC_PKT_s));
    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  linux_send
 *
 *  @Description::  send outgoing packet to linux through RPMSG
 *
 *  @Input      ::
 *      pkt :   packet pointer
 *      len :   packet length
 *
 *  @Output     ::
 *
 *
 *  @Return     ::
 *
\*----------------------------------------------------------------------------*/
static int linux_send(AMBA_IPC_PKT_s *pkt, int len)
{
    int IpcStatus;
    AMBA_KAL_EVENT_FLAG_INFO_s EventFlagInfo;

    IpcStatus = AmbaKAL_EventFlagQuery(&AmbaLinkEventFlag, &EventFlagInfo);
    if ((IpcStatus == 0) && (EventFlagInfo.CurrentFlags & AMBALINK_SUSPEND_START)) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "linux is in suspend state! (0x%08x)", EventFlagInfo.CurrentFlags, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        return -1;
    }

#ifdef RPC_DEBUG
    pkt->xprt.tx_rpc_send_end = AmbaCSL_TmrGetCurrentVal(PROFILE_TIMER);
#endif
    AmbaIPC_Send(rpmsg_chnl_linux, pkt, len + sizeof(AMBA_IPC_PKT_s));
    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  xprt_send_pkt
 *
 *  @Description::  send out a packet
 *
 *  @Input      ::
 *      port:   not used
 *      pkt :   packet pointer
 *      len :   packet length
 *
 *  @Output     ::
 *
 *
 *  @Return     ::
 *
\*----------------------------------------------------------------------------*/
int xprt_send_pkt(AMBA_IPC_PORT_s *port, AMBA_IPC_PKT_s *pkt, int size)
{
    int host;
#ifdef RPC_DEBUG
    unsigned int diff;
    unsigned int cur_time;
#endif

    UNUSED(port);

    if (pkt->msg.type == AMBA_IPC_MSG_CALL) {
        host = pkt->xprt.server_addr;
    } else {
        host = pkt->xprt.client_addr;
    }

    if (host < AMBA_IPC_HOST_MAX && xprt_func[host]) {
#ifdef RPC_DEBUG
        cur_time = AmbaCSL_TmrGetCurrentVal(PROFILE_TIMER);
        if( rpc_stat->TxLastInjectTime != 0){
            diff = calc_timer_diff(rpc_stat->TxLastInjectTime, cur_time);
        }
        else {
            diff = 0;
        }
        rpc_stat->TxTotalInjectTime += diff;
        rpc_stat->TxLastInjectTime = cur_time;
#endif
        return xprt_func[host](pkt, size);
    }

    return 0;
}
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_InitProfile
 *
 *  @Description::  init RPC profile
 *
 *  @Input      ::
 *
 *  @Output     ::
 *
 *
 *  @Return     ::
 *
\*----------------------------------------------------------------------------*/
 #ifdef RPC_DEBUG
 void AmbaIPC_InitProfile(void){
    rpc_stat = (AMBA_RPC_STATISTIC_s *)(ULONG)RPC_PROFILING_ADDR;
    memset(rpc_stat, 0, sizeof(AMBA_RPC_STATISTIC_s));

    rpc_stat->MinLkToLuTime = 0xFFFFFFFF;
    rpc_stat->MinLuToLkTime = 0xFFFFFFFF;
    rpc_stat->MinTxToLuTime = 0xFFFFFFFF;
    rpc_stat->MinLuToTxTime = 0xFFFFFFFF;
 }

 /*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_RecordStats
 *
 *  @Description::  record RPC profile
 *
 *  @Input      ::
 *
 *  @Output     ::
 *
 *
 *  @Return     ::
 *
\*----------------------------------------------------------------------------*/
 void AmbaIPC_RecordStats(AMBA_IPC_XPRT_s *xprt, AMBA_IPC_COMMUICATION_MODE_e mode){
    unsigned int diff;
    /* Linux user space to Linux kernel space */
    diff = calc_timer_diff(xprt->lu_to_lk_start, xprt->lu_to_lk_end);
    rpc_stat->LuToLkTime += diff;
    if(diff > rpc_stat->MaxLuToLkTime){
        rpc_stat->MaxLuToLkTime = diff;
    }
    if(diff < rpc_stat->MinLuToLkTime){
        rpc_stat->MinLuToLkTime = diff;
    }
    /* ThreadX RPC Time */
    diff = calc_timer_diff(xprt->tx_rpc_recv_start, xprt->tx_rpc_recv_end);
    rpc_stat->TxRpcRecvTime += diff;

    /* Linux user space to ThreadX */
    diff = calc_timer_diff(xprt->lu_to_lk_start, xprt->tx_rpc_recv_end);
    rpc_stat->LuToTxTime += diff;
    if(diff > rpc_stat->MaxLuToTxTime){
        rpc_stat->MaxLuToTxTime = diff;
    }
    if(diff < rpc_stat->MinLuToTxTime){
        rpc_stat->MinLuToTxTime = diff;
    }
    rpc_stat->LuToTxCount++;

    if (mode == AMBA_IPC_SYNCHRONOUS){
        rpc_stat->RoundTripTime += diff;
        rpc_stat->SynPktCount++;
        /* Count twice for one synchronous packet. */
    }
    else if (mode == AMBA_IPC_ASYNCHRONOUS){
        rpc_stat->OneWayTime += diff;
        rpc_stat->AsynPktCount++;
    }
 }
  /*----------------------------------------------------------------------------*\
 *  @RoutineName::  CalcRpcProfile
 *
 *  @Description::  calculate and dump RPC profile
 *
 *  @Input      ::
 *
 *  @Output     ::
 *
 *
 *  @Return     ::
 *
\*----------------------------------------------------------------------------*/
 void CalcRpcProfile(void){
    double Duration = 0;
    unsigned int ApbFreq = AmbaRTSL_PllGetApbClk();
    double mFactor;

    /* ms Factor. */
    mFactor = (double) (1000 / (double) ApbFreq);

    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "------------------Simplified RPC PROFILE-------------------", NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Sync rpc packets : %u", rpc_stat->SynPktCount/2, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Async rpc packets: %u\n", rpc_stat->AsynPktCount, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    Duration =  ((double) rpc_stat->RoundTripTime / (double) (rpc_stat->SynPktCount/2)) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg roundtrip time for a Syns pkt: %6.6lf ms", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    Duration =  ((double) rpc_stat->OneWayTime / (double) rpc_stat->AsynPktCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg deliver time for an Asyns pkt: %6.6lf ms\n", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Total rpc packets (TX -> LU): %u", rpc_stat->TxToLuCount, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    Duration =  ((double) rpc_stat->TxToLuTime / (double) rpc_stat->TxToLuCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg time (TX -> LU)         : %6.6lf ms", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Total rpc packets (LU -> TX): %u", rpc_stat->LuToTxCount, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    Duration =  ((double) rpc_stat->LuToTxTime / (double) rpc_stat->LuToTxCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg time (LU -> TX)         : %6.6lf ms\n\n", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();


    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "------------------Detailed RPC PROFILE---------------------", NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "-------------------From ThreadX to Linux-------------------", NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    Duration =  (double) rpc_stat->TxToLuCount / ((double) rpc_stat->TxTotalInjectTime * mFactor);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg injection rate in TX : %lf", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    Duration =  ((double) rpc_stat->TxRpcSendTime / (double) rpc_stat->TxToLuCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg rpc time in TX       : %6.6lf ms", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    Duration =  ((double) rpc_stat->LkToLuTime / (double)rpc_stat->TxToLuCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg netlink (LK -> LU)   : %6.6lf ms", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    Duration =  ((double) rpc_stat->MaxLkToLuTime) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Max netlink (LK -> LU)   : %6.6lf ms", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    Duration =  ((double) rpc_stat->MinLkToLuTime) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Min netlink (LK -> LU)   : %6.6lf ms", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    Duration =  ((double) rpc_stat->MaxTxToLuTime) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Max time (TX -> LU)      : %6.6lf ms", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    Duration =  ((double) rpc_stat->MinTxToLuTime) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Min time (TX -> LU)      : %6.6lf ms\n", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "-------------------From Linux to ThreadX-------------------", NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    Duration = (double) rpc_stat->LuToTxCount / ((double) rpc_stat->LuTotalInjectTime * mFactor);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg injection rate in Linux  : %lf", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    Duration =  ((double)rpc_stat->TxRpcRecvTime / (double)rpc_stat->LuToTxCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg rpc time in TX           : %6.6lf ms", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    Duration = ((double)rpc_stat->LuToLkTime / (double) rpc_stat->LuToTxCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg netlink (LU -> LK)       : %6.6lf ms", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    Duration =  ((double)rpc_stat->MaxLuToLkTime) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Max netlink (LU -> LK)       : %6.6lf ms", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    Duration =  ((double) rpc_stat->MinLuToLkTime) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Min netlink (LU -> LK)       : %6.6lf ms", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    Duration =  ((double) rpc_stat->MaxLuToTxTime) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Max time (LU -> TX)          : %6.6lf ms", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    Duration =  ((double) rpc_stat->MinLuToTxTime) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Min time (LU -> TX)          : %6.6lf ms", Duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

 }
 #endif
#ifdef SAM
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  SDCardHandleExecClient
 *
 *  @Description::  When inset or eject SD card, invoke mount/umount command in
 *  Linux through a exec rpc call.
 *
 *  @Input      ::
 *      Slot :  Slot number
 *      EID  :  Event ID
 *  @Output     ::
 *
 *  @Return     ::
 *
\*----------------------------------------------------------------------------*/
static void SDCardHandleExecClient(int Slot, int EID)
{
    AMBA_IPC_REPLY_STATUS_e status;
    char *mountCmd = "/usr/local/share/script/sd_script.sh mount /tmp/SD0 c: ambafs";
    char *umountCmd ="/usr/local/share/script/sd_script.sh umount /tmp/SD0";
    AMBA_RPC_PROG_EXEC_ARG_s *pExec = NULL;
    int IpcStatus;
    UINT8 ExecBuf[EXEC_BUF];
    int size;
    AMBA_KAL_EVENT_FLAG_INFO_s EventFlagInfo;

    size = sizeof(AMBA_RPC_PROG_EXEC_ARG_s)+100*sizeof(char);
    if(size > EXEC_BUF) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "size(%d) is big than %d ",size, EXEC_BUF, 0U, 0U, 0U);
        AmbaPrint_Flush();
        return;
    }
    memset(ExecBuf,0, sizeof(ExecBuf));

    if (AmbaLinkBootType != AMBALINK_COLD_BOOT) {
        IpcStatus = AmbaKAL_EventFlagQuery(&AmbaLinkEventFlag, &EventFlagInfo);
        if (!(EventFlagInfo.CurrentFlags & AMBALINK_BOOT_IPC_READY)) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "IPC is not ready yet! (%d)", IpcStatus, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            return;
        }
    }

    if (rpmsg_chnl_linux == NULL) {
        /* RPC is not initialized yet! */
        return;
    }

    if (!script_hdr_clnt) {
        script_hdr_clnt = AmbaIPC_ClientCreate(AMBA_RPC_PROG_LU_UTIL_HOST, AMBA_RPC_PROG_LU_UTIL_PROG_ID, AMBA_RPC_PROG_LU_UTIL_VER);
        if(script_hdr_clnt == 0) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s Client created failed.", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
    }

    if(Slot == SCM_SLOT_SD0){
        switch (EID) {
            case SCM_CARD_INSERTED:
                pExec = (AMBA_RPC_PROG_EXEC_ARG_s *)ExecBuf;
                strcpy(pExec->command, mountCmd);
                break;
            case SCM_CARD_EJECTED:
                pExec = (AMBA_RPC_PROG_EXEC_ARG_s *)ExecBuf;
                strcpy(pExec->command, umountCmd);
                break;
        }
    }
    if(pExec != NULL) {
        status = AmbaRpcProg_Util_Exec2_Clnt(pExec, NULL, script_hdr_clnt);
        if(status != 0){
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "SDCardHandleExecClient execute failed: status %d", status, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

}
#endif
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_RpcSyncRtc
 *
 *  @Description::  Sync RTC.
 *
 *  @Input      ::  none
 *
 *  @Output     ::  none
 *
 *  @Return     ::  none
 *
\*----------------------------------------------------------------------------*/
void AmbaIPC_RpcSyncRtc(void)
{
    AMBA_IPC_REPLY_STATUS_e Status;
    char *pScript = "/usr/local/share/script/sync_rtc.sh";
    AMBA_RPC_PROG_EXEC_ARG_s *pExec = NULL;
    UINT8 ExecBuf[EXEC_BUF];
    int size;

    if (rpmsg_chnl_linux == NULL) {
        /* RPC is not initialized yet! */
        return;
    }

    if(!script_hdr_clnt) {
        script_hdr_clnt = AmbaIPC_ClientCreate(AMBA_RPC_PROG_LU_UTIL_HOST, AMBA_RPC_PROG_LU_UTIL_PROG_ID, AMBA_RPC_PROG_LU_UTIL_VER);
        if(script_hdr_clnt == 0) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s Client created failed.", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
    }

    size = sizeof(AMBA_RPC_PROG_EXEC_ARG_s) + 100 * sizeof(char);
    if(size > EXEC_BUF) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "size(%d) is big than %d ",size, EXEC_BUF, 0U, 0U, 0U);
        AmbaPrint_Flush();
        return;
    }
    memset(ExecBuf,0, sizeof(ExecBuf));
    pExec = (AMBA_RPC_PROG_EXEC_ARG_s *)ExecBuf;

    strcpy(pExec->command, pScript);
    Status = AmbaRpcProg_Util_Exec2_Clnt(pExec, NULL, script_hdr_clnt);
    if(Status != 0){
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_RpcSyncRTC failed (%d).", Status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_RpcStrError
 *
 *  @Description::  Get the description of error code
 *
 *  @Input      ::
 *
 *  @Output     ::
 *
 *
 *  @Return     ::
 *
\*----------------------------------------------------------------------------*/
const char *AmbaIPC_RpcStrError(int ErrNum)
{
    static const char *errstr[] = {
        "success",
        "rpc program is unavailable",
        "rpc parameter is invalid",
        "system error",
        "timeout",
        "invalid client id",
        "the svc is not registered",
        "the svc is double registered",
        "the memory allocation for svc is failed",
        "ambalink is not ready",
        "crc values of rpc headers are inconsistent",
    };

    if(abs(ErrNum) < AMBA_IPC_STATUS_NUM) {
        return errstr[abs(ErrNum)];
    }
    else {
        return "no such error code";
    }
}
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_RpcInit
 *
 *  @Description::  init RPC modile
 *
 *  @Input      ::
 *
 *  @Output     ::
 *
 *
 *  @Return     ::
 *
\*----------------------------------------------------------------------------*/
void AmbaIPC_RpcInit(void)
{
    int i;

    AmbaIPC_Rpc_clrbss();

    AmbaKAL_MutexCreate(&global_port_lock,"Rpc");
    global_port_id = HASH_TABLE_SIZE;

    /* init list head for each hash entry */
    for (i = 0; i < HASH_TABLE_SIZE; i++)
        INIT_LIST_HEAD(port_head[i]);

#ifdef CONFIG_OPENAMP
    rpmsg_chnl_linux = AmbaIPC_Alloc(RPMSG_DEV_OAMP, "aipc_rpc", linux_recv);
#else
    rpmsg_chnl_linux = AmbaIPC_Alloc(RPMSG_DEV_AMBA, "aipc_rpc", linux_recv);
#endif
    if (rpmsg_chnl_linux == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaIPC_Alloc failed!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return;
    }
    AmbaIPC_RegisterChannel(rpmsg_chnl_linux, NULL);

    /* setup transport functions */
    xprt_func[AMBA_IPC_HOST_LINUX] = linux_send;
    xprt_func[AMBA_IPC_HOST_THREADX] = process_pkt;
#ifdef RPC_DEBUG
    AmbaIPC_InitProfile();
#endif
    AmbaIPC_InitBinder();
    AmbaIPC_InitClntCtrl();
#ifdef SAM
    AmbaSCM_Register(SDCardHandleExecClient);
#endif
}
