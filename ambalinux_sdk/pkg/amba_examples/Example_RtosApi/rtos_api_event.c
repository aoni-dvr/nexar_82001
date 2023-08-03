
static event_cb_func event_cb = NULL;

int rtos_api_lib_register_event_cb(event_cb_func func)
{
    event_cb = func;
    return 0;
}

static void ipc_receiver(ipc_event_s *arg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    if (arg == NULL || pRet == NULL) {
        return;
    }
    if (event_cb != NULL) {
        ipc_event_s event;
        memcpy(&event, arg, sizeof(ipc_event_s));
        if (event.event_id == NOTIFY_FILE_CREATED
            || event.event_id == NOTIFY_FILE_DELETED) {
            rtos_api_lib_get_timestamp_from_filepath(event.arg.file_arg.path, &event.arg.file_arg.file_time);
        }
        event_cb(event);
    }
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

int rtos_api_lib_event_init(void)
{
    AMBA_IPC_PROG_INFO_s prog_info[1];
    int ret = 0;

	prog_info->ProcNum = 1;
    prog_info->pProcInfo = malloc(prog_info->ProcNum * sizeof(AMBA_IPC_PROC_s));
    prog_info->pProcInfo[0].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info->pProcInfo[0].Proc = (AMBA_IPC_PROC_f) &ipc_receiver;
    ret = ambaipc_svc_register(AMBA_RPC_PROG_LU_LINUX_API_SERVICE_PROG_ID, AMBA_RPC_PROG_LU_LINUX_API_SERVICE_VER, "ipc_rtos_api", prog_info, 1);

    return 0;
}

int rtos_api_lib_event_deinit(void)
{
    ambaipc_svc_unregister(AMBA_RPC_PROG_LU_LINUX_API_SERVICE_PROG_ID, AMBA_RPC_PROG_LU_LINUX_API_SERVICE_VER);

    return 0;
}

