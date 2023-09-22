/**
 * @file AmbaIPC_Client.c
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
 * @details The module of Ambarella Network Support
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaLink.h"
#include "AmbaLinkInternal.h"
#include "AmbaLinkPrivate.h"
#include "libwrapc.h"
#include "AmbaIPC_List.h"
#include "AmbaCSL_TMR.h"

#include "AmbaIPC_Rpc.h"

#define AMBAIPC_CLIENT_WAIT    1000
typedef struct _AMBA_IPC_client_HANDLE_s_ {
    AMBA_IPC_PORT_s    port;    /**< port */
    AMBA_IPC_PKT_s     hdr;     /**< header */
    int                pkt_len; /**< packet length */
    AMBA_IPC_COMMUICATION_MODE_e    *proc_mode;   ///< record the communication modes for procedures
    AMBA_IPC_PKT_s     pkt;     /**< must be the last one */
} AMBA_IPC_CLIENT_HANDLE_s;

unsigned int Seq_Num = 0;
static int clear_count = 0; //clear timeout_list when clear_count is large than the threshold.
AMBA_KAL_MUTEX_t Seq_Lock;

#define GET_SEQ_NUM(x)                                          \
    do {                                                        \
        AmbaKAL_MutexTake(&Seq_Lock, AMBA_KAL_WAIT_FOREVER);    \
        x = Seq_Num++;                                          \
        AmbaKAL_MutexGive(&Seq_Lock);                           \
    } while (0)

typedef struct _TIMEOUT_RECORD_s_ {
    int             seq;    /**< seq */
    AMBA_IPC_LIST_s list;   /**< list */
} TIMEOUT_RECORD_s;

#define CLEAR_THRESHOLD 100

extern  unsigned int crc_result[]        __attribute__((weak));
extern  unsigned int priv_crc_result[]   __attribute__((weak));
extern  int rpc_prog_id[]                __attribute__((weak));
extern  int priv_rpc_prog_id[]           __attribute__((weak));
extern  int rpc_num                      __attribute__((weak));
extern  int priv_rpc_num                 __attribute__((weak));
extern  AMBA_LINK_CTRL_s AmbaLinkCtrl    __attribute__((weak));
extern AMBA_IPC_PORT_s *find_port(int id);
#ifdef RPC_DEBUG
extern void AmbaIPC_RecordStats(AMBA_IPC_XPRT_s *xprt, AMBA_IPC_COMMUICATION_MODE_e mode);
#endif

static LIST_HEAD(timeout_list);

static void clear_timeout_list(void)
{
    TIMEOUT_RECORD_s *record = NULL;
    list_for_each_entry(record, &timeout_list, TIMEOUT_RECORD_s, list) {
        AmbaIPC_ListDel(&record->list);
        AmbaLink_Free(MEM_POOL_TIMEOUT_RECORD_BUF, record);
    }
}
/*
* check whether a packet is timeout
*/
static TIMEOUT_RECORD_s *check_timeout(unsigned int id)
{
    TIMEOUT_RECORD_s *record;
    int list_is_empty = AmbaIPC_ListIsEmpty(&timeout_list);

    if(list_is_empty) {
        return NULL;
    } else {
        // check whether a packet is timeout
        list_for_each_entry(record, &timeout_list, TIMEOUT_RECORD_s, list)
            if(record->seq == (int)id)
                return record;
    }

    //the pkt is not in timeoutlist
    if (++clear_count > CLEAR_THRESHOLD)
        clear_timeout_list();

    return NULL;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  ClientRecv
 *
 *  @Description::  Callback to receive reply
 *
 *  @Input      ::
 *      context :
 *      pkt :
 *      len :
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int ClientRecv(void *context, AMBA_IPC_PKT_s *pkt, int len)
{
    AMBA_IPC_CLIENT_HANDLE_s *client = (AMBA_IPC_CLIENT_HANDLE_s *)context;
    TIMEOUT_RECORD_s *record = check_timeout(pkt->xprt.xid);

    if(record) {  //the timeout pkt should be ignored
        AmbaIPC_ListDel(&record->list);
        AmbaLink_Free(MEM_POOL_TIMEOUT_RECORD_BUF, record);
    } else {
        memmove(&client->pkt, pkt, len+AMBA_IPC_HDRLEN);
        client->pkt_len = len;
        AmbaKAL_EventFlagSet(&client->port.flag, 1);
    }
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  ClientWait
 *
 *  @Description::  wait for data ready
 *
 *  @Input      ::
 *      client :
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static inline int ClientWait(AMBA_IPC_CLIENT_HANDLE_s *client, UINT32 timeout)
{
    UINT32 actFlags;

    return AmbaKAL_EventFlagGet(&client->port.flag, 1, 1U, 1U,
                          &actFlags, timeout);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_ClientCrcCheck
 *
 *  @Description::  check CRC values of RPC headers in dual OSes
 *
 *  @Input      ::
 *     svc_crc: CRC value from the server side
 *     prog: program id
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int AmbaIPC_ClientCrcCheck(unsigned int svc_crc, int prog)
{
    int i = 0, index = -1;
    unsigned int crc;

    UNUSED(svc_crc);

    // search in public rpc
    if ((&rpc_num != NULL) && (rpc_prog_id != NULL) && (crc_result != NULL)) {
            for (i = 0; i < rpc_num; i++) {
                    if (rpc_prog_id[i] == prog) {   //look for the index of crc_result
                            index = i;
                            crc = crc_result[i];
                            break;
                    }
            }
    }

    // search in private rpc
    if ((&priv_rpc_num != NULL) && (priv_rpc_prog_id != NULL) && (priv_crc_result != NULL)) {
            for (i = 0; i < priv_rpc_num; i++) {
                    if (priv_rpc_prog_id[i] == prog) {   //look for the index of crc_result
                            index = i;
                            crc = priv_crc_result[i];
                            break;
                    }
            }
    }

    if (index == -1) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "ERROR!! Program id is not found!!", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s Please use the predefined program id in the header file", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            return -1;
    }

    if (crc != svc_crc) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "----- The CRC num %d ------", i, 0U, 0U, 0U, 0U);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "The crc in ThreadX : 0x%08", crc, 0U, 0U, 0U, 0U);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "The crc in Linux   : 0x%08", svc_crc, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            return -1;  //The CRC values are not matching
    }

    return 0;   //All of the RPC header files are consistent in dual OSes
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_SendBinderMsg
 *
 *  @Description::  send binding message
 *
 *  @Input      ::
 *      clnt :
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      AMBA_IPC_REPLY_STATUS_e : see AmbaIPC_Rpc_Def.h
\*-----------------------------------------------------------------------------------------------*/
static AMBA_IPC_REPLY_STATUS_e AmbaIPC_SendBinderMsg(AMBA_IPC_CLIENT_HANDLE_s *client){
    AMBA_IPC_REPLY_STATUS_e client_status;
    unsigned int flag_status;
    TIMEOUT_RECORD_s *record;
    // send binder_bind message
    client->hdr.msg.type = AMBA_IPC_MSG_CALL;
    client->hdr.msg.u.call.proc  = AMBA_IPC_BINDER_BIND;
    client->hdr.xprt.server_port = AMBA_IPC_BINDING_PORT;

    client->pkt = client->hdr;
    client->pkt.xprt.mode = AMBA_IPC_SYNCHRONOUS; /* record the communication mode. */
    GET_SEQ_NUM(client->pkt.xprt.xid);
    xprt_send_pkt(&client->port, &client->pkt, 0);

    /* get the service port of the host from reply */
    flag_status = ClientWait(client, AMBAIPC_CLIENT_WAIT);

    if( flag_status != 0) {
        if( (flag_status & 0xFF) == 0x07) { // timeout occurs
            client_status = AMBA_IPC_REPLY_TIMEOUT;
            record = AmbaLink_Malloc(MEM_POOL_TIMEOUT_RECORD_BUF, sizeof(TIMEOUT_RECORD_s));
            record->seq = client->pkt.xprt.xid;
            AmbaIPC_ListAdd(&record->list, &timeout_list);
            clear_count = 0; //if timeout occurs, recount down.
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s Timeout", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
        else {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "flag_status is %u.", flag_status, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            client_status = AMBA_IPC_REPLY_SYSTEM_ERROR;
        }
    } else { //successfully recive the server response.
        client_status = client->pkt.msg.u.reply.status;
    }

    if( client_status != AMBA_IPC_REPLY_SUCCESS ){
        client->hdr.xprt.server_port = 0;
    }
    return client_status;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  ClientInitConnection
 *
 *  @Description::  After binding successfully, record the server port and do CRC checking
 *
 *  @Input      ::
 *      client :
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
static int ClientInitConnection(AMBA_IPC_CLIENT_HANDLE_s *client){
    int crc_status;
    int proc_num;
    UINT64 *parameters = (UINT64 *)((char *)&client->pkt.msg + sizeof(AMBA_IPC_MSG_s));

    /* record the server port */
    client->hdr.xprt.server_port = parameters[0];
    /* record the procedure information */
    proc_num = parameters[2];
    client->proc_mode = AmbaLink_Malloc(MEM_POOL_CLIENT_MODE_BUF, proc_num*sizeof(AMBA_IPC_COMMUICATION_MODE_e));
    memset(client->proc_mode, 0, proc_num*sizeof(AMBA_IPC_COMMUICATION_MODE_e));
    memcpy(client->proc_mode, &parameters[3], proc_num*sizeof(AMBA_IPC_COMMUICATION_MODE_e));
    /* CRC checking */
    crc_status = AmbaIPC_ClientCrcCheck((unsigned int) parameters[1],
        client->hdr.msg.u.call.prog);
    if ( crc_status != 0 ){
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "CRC values check error!", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }

    return crc_status;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_ClientCreate
 *
 *  @Description::  create a new IPC client
 *
 *  @Input      ::
 *      host :
 *      prog :
 *      vers :
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
CLIENT_ID_t AmbaIPC_ClientCreate(INT32 host, INT32 prog, INT32 vers)
{
    AMBA_IPC_CLIENT_HANDLE_s *client;
    AMBA_IPC_REPLY_STATUS_e bind_status;

    client = (AMBA_IPC_CLIENT_HANDLE_s *) AmbaLink_Malloc(MEM_POOL_CLIENT_BUF,
        sizeof(AMBA_IPC_CLIENT_HANDLE_s) + AmbaLinkCtrl.RpcPktSize);

    if(client == NULL) {
        return 0;
    }

    memset(client, 0, sizeof(AMBA_IPC_CLIENT_HANDLE_s) + AmbaLinkCtrl.RpcPktSize);
    xprt_init_port(&client->port, (void *) client, &ClientRecv);

    /* send binder_bind message */
    client->hdr.msg.u.call.prog  = prog;
    client->hdr.msg.u.call.vers  = vers;
    client->hdr.xprt.client_addr = AMBA_IPC_HOST_THREADX;
    client->hdr.xprt.client_port = client->port.id;
    client->hdr.xprt.server_addr = host;
    // send binder the bind message to get server port and CRC value of the header file
    bind_status = AmbaIPC_SendBinderMsg(client);

    if ( bind_status == AMBA_IPC_REPLY_SUCCESS ){
        if(ClientInitConnection(client)) {
            // crc is not correct, so client creation failed.
            AmbaIPC_ClientDestroy((CLIENT_ID_t) client);
            return 0;
        }
    }
    else {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "binding info: host = %d, prog = %d, vers = %d ! Client needs to bind again next time!", host, prog, vers, 0U, 0U);
        AmbaPrint_Flush();
        AmbaIPC_ClientDestroy((CLIENT_ID_t) client);
        return 0;
#endif
    }

    return (CLIENT_ID_t) client;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_ClientDestroy
 *
 *  @Description::  destroy the IPC client
 *
 *  @Input      ::
 *      client_id :
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_ClientDestroy(CLIENT_ID_t client_id)
{
    AMBA_IPC_CLIENT_HANDLE_s *client = (AMBA_IPC_CLIENT_HANDLE_s*)client_id;

    xprt_exit_port(&client->port);

    if(client->proc_mode)
        AmbaLink_Free(MEM_POOL_CLIENT_MODE_BUF, (void*)client->proc_mode);
    if(client)
        AmbaLink_Free(MEM_POOL_CLIENT_BUF, (void*)client);

    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_ClientCall
 *
 *  @Description::
 *
 *  @Input      ::
 *      client_id :
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
AMBA_IPC_REPLY_STATUS_e AmbaIPC_ClientCall(CLIENT_ID_t client_id, INT32 proc,
        void *in,  int in_len,
        void *out, int out_len,
        UINT32 timeout)
{
    AMBA_IPC_CLIENT_HANDLE_s *client = (AMBA_IPC_CLIENT_HANDLE_s *)client_id;
    AMBA_IPC_REPLY_STATUS_e status, bind_status;
    int wait_status;
    TIMEOUT_RECORD_s *record;
    UINT64 *parameters;
#ifdef RPC_DEBUG
    unsigned int cur_time = AmbaCSL_TmrGetCurrentVal(PROFILE_TIMER);
#endif

    LOCK_PORT(client->port);
    {

        if(client->hdr.xprt.server_port==0){    //binding was failed before
            bind_status = AmbaIPC_SendBinderMsg(client);   //binding again
            if( bind_status == AMBA_IPC_REPLY_SUCCESS ){
               if(!ClientInitConnection(client)){
                    goto SEND_MSG;
                } else{
                    // crc checking failed
                    bind_status = AMBA_IPC_CRC_ERROR;
                    goto BIND_DONE;
                }
            }
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
            else if( bind_status == AMBA_IPC_REPLY_TIMEOUT) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Binding svc program timeout!", NULL, NULL, NULL, NULL, NULL);
                AmbaPrint_Flush();
            }
            else if( bind_status == AMBA_IPC_REPLY_PROG_UNAVAIL ){
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Can not find svc for program 0x%08x", client->hdr.msg.u.call.prog, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
            else {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Other binding problem.", NULL, NULL, NULL, NULL, NULL);
                AmbaPrint_Flush();
            }
#endif
BIND_DONE:
            UNLOCK_PORT(client->port);
            return bind_status;
        }

SEND_MSG:
        // send call message to server port
        client->pkt = client->hdr;
        client->pkt.msg.u.call.proc = proc;
        client->pkt.xprt.mode = client->proc_mode[proc-1]; /* record the communication mode */
        parameters = (UINT64 *)((char *)&client->pkt.msg + sizeof(AMBA_IPC_MSG_s));
        memcpy(parameters, in, in_len);
#ifdef RPC_DEBUG
        client->pkt.xprt.tx_rpc_send_start = cur_time;
#endif
        GET_SEQ_NUM(client->pkt.xprt.xid);
        xprt_send_pkt(&client->port, &client->pkt, in_len);

        // check if this is a batching call
        if (!timeout || (client->proc_mode[proc-1] == AMBA_IPC_ASYNCHRONOUS)) {
            //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "no need to wait the response", NULL, NULL, NULL, NULL, NULL);
            //AmbaPrint_Flush();
            UNLOCK_PORT(client->port);
            return AMBA_IPC_REPLY_SUCCESS;
        }

        wait_status = ClientWait(client, timeout);

        // check whether timeout occurs or not
        if(wait_status == TX_NO_EVENTS) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "recving: timeout!", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            record = AmbaLink_Malloc(MEM_POOL_TIMEOUT_RECORD_BUF, sizeof(TIMEOUT_RECORD_s));
            record->seq = client->pkt.xprt.xid;
            AmbaIPC_ListAdd(&record->list, &timeout_list);
            clear_count = 0; //if timeout occurs, recount down.
            UNLOCK_PORT(client->port);
            return AMBA_IPC_REPLY_TIMEOUT;
        }

        status = client->pkt.msg.u.reply.status;

#ifdef RPC_DEBUG
        client->pkt.xprt.tx_rpc_recv_end = AmbaCSL_TmrGetCurrentVal(PROFILE_TIMER);
        AmbaIPC_RecordStats(&client->pkt.xprt, client->proc_mode[proc-1]);
#endif

        if (status == AMBA_IPC_REPLY_SUCCESS) {
            int len = client->pkt_len;
            if (out_len > len) out_len = len;
            if (out){
                memcpy(out, parameters, out_len);
            }
        }
    }
    UNLOCK_PORT(client->port);

    return status;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  clnt_ctrl_recv
 *
 *  @Description::  sepcial callback fucntion for client ctrl port
 *
 *  @Input      ::
 *      context :
 *      pkt :
 *      len :
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*----------------------------------------------------------------------------------------------*/
static int clnt_ctrl_recv(void *context, AMBA_IPC_PKT_s *pkt, int len)
{
    AMBA_IPC_CLIENT_HANDLE_s *client;
    int port_id;
    AMBA_IPC_PORT_s *port;
    UINT64 *parameters;

    UNUSED(context);
    UNUSED(len);
    switch (pkt->msg.u.call.proc) {
        case AMBA_IPC_CLNT_REBIND:
            port_id = pkt->xprt.client_port;
            port = find_port(port_id);
            if(port) {
                client = (AMBA_IPC_CLIENT_HANDLE_s *) port->context;
                parameters = (UINT64 *)((char *)&pkt->msg + sizeof(AMBA_IPC_MSG_s));
                client->hdr.xprt.server_port = (UINT32)parameters[0];
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s update svc port info", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_Flush();
            }
            break;

        default:
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "\tunknown case: %d", pkt->msg.u.call.proc, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            break;
    }
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_InitClntCtrl
 *
 *  @Description:: Init client controller
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
 void AmbaIPC_InitClntCtrl(void)
 {
    static AMBA_IPC_PORT_s ctrl_port;

    ctrl_port.id = AMBA_IPC_CLNT_CONTROL_PORT;
    xprt_init_port(&ctrl_port, (void *) &ctrl_port, clnt_ctrl_recv);
 }
