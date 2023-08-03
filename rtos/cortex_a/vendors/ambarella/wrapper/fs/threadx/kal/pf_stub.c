/*
     Copyright (C) 2004 By eSOL Co.,Ltd. Tokyo, Japan

     This software is protected by the law and the agreement concerning
     a Japanese country copyright method, an international agreement,
     and other intellectual property right and may be used and copied
     only in accordance with the terms of such license and with the inclusion
     of the above copyright notice.

     This software or any other copies thereof may not be provided
     or otherwise made available to any other person.  No title to
     and ownership of the software is hereby transferred.

     The information in this software is subject to change without
     notice and should not be construed as a commitment by eSOL Co.,Ltd.
 */
/****************************************************************************
[pf_stub.c] - PrFILE2 API Stub routines.

 NOTES:
  - This file is sample implementation of 'pf_system.c'.
    USER MUST MODIFY THIS FILE FOR USER'S ENVIRONMENT.

****************************************************************************/
#include "prfile2/pf_config.h"
#include "prfile2/pf_apicmn.h"

#if PF_USE_API_STUB

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#include "pf_stub_standard.h"

#ifdef EB_PRFILE2_SUPPORT_PFS_IF
#include "pf_stub_extret.h"
#endif /* EB_PRFILE2_SUPPORT_PFS_IF */

#if PF_UNICODE_INTERFACE_SUPPORT
#include "pf_stub_unicode.h"
#endif /* PF_UNICODE_INTERFACE_SUPPORT */

#if PF_EXAPI_SUPPORT
#include "pf_stub_module.h"
#endif /* PF_EXAPI_SUPPORT */

static UINT8 _AmbaFS_MsgQueue[sizeof(PF_MSG)];
static UINT8 _AmbaFS_Stack[8192];

static void PrFILE_TaskEntry(UINT32 Arg)
{
    pfstub_entry();
}

/*---------------------------------------------------------------------------
 pfstub_init_stub - Initialize API stub function.

 [Synopsis]
    void  pfstub_init_stub(void);
        pIdStub      API stub parameter table structure.

 [Description]
    Initialize API stub function.

    Note that this function is system-dependent.

 [Return Value]
    None

 ----------------------------------------------------------------------------*/
void  pfstub_init_stub(void)
{
    static PFK_CMBX _AmbaFS_KalMailboxCreate;
    static PFK_CTSK _AmbaFS_KalTaskCreate;

    if (PF_SYS_SET_STUB.state == PF_STUB_NOINIT) {
        /* Create mail box for API stub */
        _AmbaFS_KalMailboxCreate.pMsgQueueBase = _AmbaFS_MsgQueue;
        _AmbaFS_KalMailboxCreate.MsgSize = sizeof(PF_MSG);
        _AmbaFS_KalMailboxCreate.MaxNumMsg = 1;
        PF_SYS_SET_STUB.mbxID = pfk_create_mailbox(&_AmbaFS_KalMailboxCreate);

        /* Create task for API stub */
        _AmbaFS_KalTaskCreate.pTaskName     = "PrFILE2";
        _AmbaFS_KalTaskCreate.Priority      = (KAL_TASK_PRIORITY_LOWEST + KAL_TASK_PRIORITY_HIGHEST) >> 1;
        _AmbaFS_KalTaskCreate.EntryFunction = PrFILE_TaskEntry;
        _AmbaFS_KalTaskCreate.EntryArg      = 0;
        _AmbaFS_KalTaskCreate.pStackBase    = _AmbaFS_Stack;
        _AmbaFS_KalTaskCreate.StackByteSize = sizeof(_AmbaFS_Stack);
        PF_SYS_SET_STUB.taskID = pfk_create_task(&_AmbaFS_KalTaskCreate);

        pfk_start_task(PF_SYS_SET_STUB.taskID, STUB_TSK_STARTCODE);

        /* Clear all status and mark initialized flag */
        PF_SYS_SET_STUB.state = PF_STUB_INIT;
    }

}

/*---------------------------------------------------------------------------
 pfstub_final_stub - Finalize API stub function.

 [Synopsis]
    void  pfstub_final_stub(void);
        pIdStub      API stub parameter table structure.

 [Description]
    Finalize API stub function.

    Note that this function is system-dependent.

 [Return Value]
    None

 ----------------------------------------------------------------------------*/
void  pfstub_final_stub(void)
{
    if (PF_SYS_SET_STUB.state & PF_STUB_INIT) {
        /* Stop and delete task for API stub */
        pfk_terminate_task(PF_SYS_SET_STUB.taskID);
        pfk_delete_task(PF_SYS_SET_STUB.taskID);

        /* Delete mail box for API stub */
        pfk_delete_mailbox(PF_SYS_SET_STUB.mbxID);

        /* Clear all status */
        PF_SYS_SET_STUB.state = PF_STUB_NOINIT;
    }
}

/*---------------------------------------------------------------------------
 pfstub_call_function - API stub function call.

 [Synopsis]
    void  pfstub_call_function(FCPFNC *fnctab);
        fnctab       PrFILE2 function parameter table structure.

 [Description]
    Call each PrFILE2 API routine.

    Note that this function is system-dependent.

 [Return Value]
    None

 ----------------------------------------------------------------------------*/
void  pfstub_call_function(FCPFNC *fnctab)
{
    if (IS_STANDARD_INTERFACE(fnctab->func_no)) {
        pfstub_call_function_standard(fnctab);
    }
#if PF_UNICODE_INTERFACE_SUPPORT
    else if (IS_UNICODE_INTERFACE(fnctab->func_no)) {
        pfstub_call_function_unicode(fnctab);
    }
#endif /* PF_UNICODE_INTERFACE_SUPPORT */
#ifdef EB_PRFILE2_SUPPORT_PFS_IF
    else if (IS_EXTRET_INTERFACE(fnctab->func_no)) {
        pfstub_call_function_extret(fnctab);
    }
#endif /* EB_PRFILE2_SUPPORT_PFS_IF */
#if PF_EXAPI_SUPPORT
    else if (IS_MODULE_INTERFACE(fnctab->func_no)) {
        pfstub_call_function_module(fnctab);
    }
#endif /* PF_EXAPI_SUPPORT */
    else {
        /* In normal operation, here is never reached. */
        fnctab->ret_val = (void*)PFS_ERR;     /* function number error    */
    }

    return;
}

/*---------------------------------------------------------------------------
 pfstub_entry - API stub entry.

 [Synopsis]
    void  pfstub_entry(void);

 [Description]
    API stub entry routine.

    Note that this function is system-dependent.

 [Return Value]
    None

 ----------------------------------------------------------------------------*/
void  pfstub_entry(void)
{
    /* Since this function is created as API stub task, it never returns. */

    PF_MSG      *apl_msg = NULL;        /* message from an application */
    FCPFNC      *fnctab;                /* function parameter table    */
    PFK_MSG*    wk_pmsg;
    void*       cnv_pmsg;
    long        ret;

    for (;;) {
        /* receive a message from an application */
        ret = pfk_receive_message(PF_SYS_SET_STUB.mbxID, &wk_pmsg);
        if (!ret) {
            cnv_pmsg = wk_pmsg;
            apl_msg = cnv_pmsg;

            /* set to PrFILE2 API function parameter table address */
            fnctab = (FCPFNC *)&apl_msg->data;
            pf_vol_set.cur_ctxid = fnctab->tid;

            /* call API stub function routine */
            pfstub_call_function(fnctab);

            /* wake up APL task */
            pfk_wakeup_task(pf_vol_set.cur_ctxid);
        }
    }
}

/*---------------------------------------------------------------------------
 pfstub_com_massage - API stub communication function.

 [Synopsis]
    PF_S_LONG  pfstub_com_massage(FCPFNC *fcpfnc);
        fcpfnc       PrFILE2 function parameter table structure.

 [Description]
    send request message to FATFS and  receive result message from FATFS.

    Note that this function is system-dependent.

 [Return Value]
    PFS_OK   = Normal termination
    PFS_ERR  = Error occured

 ----------------------------------------------------------------------------*/
PF_S_LONG pfstub_com_massage(FCPFNC *fcpfnc)
{
    PF_S_LONG   err;        /* error code                */
    PF_S_LONG   tsk_id;     /* task id                   */
    PF_MSG      msg_stack;  /* request message structure */

    /* get application task ID */
    err = pfk_get_task_id(&tsk_id);
    if (err) {
        return (PFS_ERR);
    }
    fcpfnc->tid = tsk_id;

    /* copy parameter to message structure */
    pf_memcpy(&msg_stack. data, fcpfnc, sizeof(FCPFNC));

    /*************************/
    /* send a message to FCP */
    /*************************/
    err = pfk_send_message(PF_SYS_SET_STUB.mbxID, (PFK_MSG*)&msg_stack);
    if (err) {
        return (PFS_ERR);
    }

    /* wait until wake up by FATFS */
    err = pfk_sleep_task();
    if (err) {
        return (PFS_ERR);
    }

    /* copy result from message area */
    pf_memcpy(fcpfnc, &msg_stack.data, sizeof(FCPFNC));

    return (PFS_OK);
}

#endif /* PF_USE_API_STUB */

