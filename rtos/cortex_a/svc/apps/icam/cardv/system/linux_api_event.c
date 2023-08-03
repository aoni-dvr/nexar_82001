#include "linux_api_event.h"

int linux_api_service_notify_event(ipc_event_s event)
{
    static CLIENT_ID_t clnt = NULL;

    if (app_helper.linux_booted == 0 || dqa_test_script.ddr_shmoo_test || dqa_test_script.product_line_mode) {
        return -1;
    }
    if (clnt == NULL) {
        clnt = AmbaIPC_ClientCreate(AMBA_IPC_HOST_LINUX, AMBA_RPC_PROG_LU_LINUX_API_SERVICE_PROG_ID, AMBA_RPC_PROG_LU_LINUX_API_SERVICE_VER);
        if (clnt == NULL) {
            debug_line("[%s]AmbaIPC_ClientCreate fail, event_id: %d", __func__, event.event_id);
            return -1;
        }
    }
    if (AmbaIPC_ClientCall(clnt, AMBA_RPC_PROG_LU_LINUX_API_SERVICE_IPC, (void *)&event, sizeof(ipc_event_s) + 1, NULL, 0, 0) != AMBA_IPC_REPLY_SUCCESS) {
        debug_line("%s send fail, event_id: %d", __func__, event.event_id);
        return -1;
    }

    return 0;
}

