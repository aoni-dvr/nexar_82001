#include "gnss_task.h"

// msg task
static AMBA_KAL_TASK_t gnss_task;
#define GNSS_TASK_STACK_SIZE (32 * 1024)
static unsigned char gnss_task_stack[GNSS_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static int debug_enable = 0;

#define SIZE_UART_BUFFER (4096U)

static void *gnss_task_entry(void *argv);

static int running = 0;
int gnss_task_start(void)
{
    unsigned int ret = 0;
    static int inited = 0;
    AMBA_UART_CONFIG_s UartConfig = {0};
    static UINT8 uart_rx_buffer[SIZE_UART_BUFFER] GNU_SECTION_NOZEROINIT;
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    if (inited) {
        return 0;
    }
    SvcUserPref_Get(&pSvcUserPref);
    //config uart
    UartConfig.NumDataBits   = AMBA_UART_DATA_8_BIT;
    UartConfig.ParityBitMode = AMBA_UART_PARITY_NONE;
    UartConfig.NumStopBits   = AMBA_UART_STOP_1_BIT;
#if defined(CONFIG_BSP_CV25_NEXAR_D161)
    AmbaUART_Config(pSvcUserPref->GnssSetting.UartChannel, 9600, &UartConfig);
    AmbaUART_HookDeferredRxBuf(pSvcUserPref->GnssSetting.UartChannel, SIZE_UART_BUFFER, &uart_rx_buffer[0]);
    AmbaUART_ClearDeferredRxBuf(pSvcUserPref->GnssSetting.UartChannel);
    gnss_set_data_rate(pSvcUserPref->GnssSetting.DataRate);
    gnss_set_dsp_uart_baudrate(pSvcUserPref->GnssSetting.DataRate);
    gnss_set_satellite_id(pSvcUserPref->GnssSetting.Id);
    gnss_set_gst_enable(1);
#else
#if defined(CONFIG_BSP_H32_NEXAR_D081)
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_75_UART2_RXD);
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_76_UART2_TXD);
#endif
    AmbaUART_Config(pSvcUserPref->GnssSetting.UartChannel, 115200, &UartConfig);
    AmbaUART_HookDeferredRxBuf(pSvcUserPref->GnssSetting.UartChannel, SIZE_UART_BUFFER, &uart_rx_buffer[0]);
    AmbaUART_ClearDeferredRxBuf(pSvcUserPref->GnssSetting.UartChannel);
#endif
    running = 1;
    ret = AmbaKAL_TaskCreate(&gnss_task,
                            "gnss_task",
                            150,
                            (AMBA_KAL_TASK_ENTRY_f)gnss_task_entry,
                            0,
                            gnss_task_stack,
                            GNSS_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("gnss_task create fail");
        running = 0;
        return -1;
    }
    //agnss_task_start();
    inited = 1;

    return 0;
}

int gnss_task_stop(void)
{
    int Rval = 0;

    running = 0;
    AmbaKAL_TaskSleep(200);
    Rval = AmbaKAL_TaskTerminate(&gnss_task);
    if (Rval == OK) {
        Rval = AmbaKAL_TaskDelete(&gnss_task);
    }

    return 0;
}

int gnss_task_set_debug(int enable)
{
    debug_enable = enable;

    return 0;
}

static int connected = 0;
int gnss_task_get_connected(void)
{
    return connected;
}

int gnss_task_uart_write2(const unsigned char *data, const unsigned int len)
{
    unsigned int SentSize;
    const UINT8 *pTxBuf;
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    SvcUserPref_Get(&pSvcUserPref);
    AmbaMisra_TypeCast32(&pTxBuf, &data);
    if (AmbaUART_Write(pSvcUserPref->GnssSetting.UartChannel, 0U, len, pTxBuf, &SentSize, 5000) != UART_ERR_NONE) {
        return -1;
    }

    return SentSize;
}

int gnss_task_uart_write(const unsigned char *data, const unsigned int len)
{
#if !defined(CONFIG_BSP_CV25_NEXAR_D161)
    return 0;
#else
    unsigned int SentSize;
    const UINT8 *pTxBuf;
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    SvcUserPref_Get(&pSvcUserPref);
    AmbaMisra_TypeCast32(&pTxBuf, &data);
    if (AmbaUART_Write(pSvcUserPref->GnssSetting.UartChannel, 0U, len, pTxBuf, &SentSize, 5000) != UART_ERR_NONE) {
        return -1;
    }

    return SentSize;
#endif
}

static void *gnss_task_entry(void *argv)
{
    char input[MAX_GNSS_RAW];
    int i = 0;
    unsigned char rx_buf __attribute__((aligned(4096))) = 0;
    unsigned int read_size = 0;
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    SvcUserPref_Get(&pSvcUserPref);
    while (running) {
        AmbaUART_Read(pSvcUserPref->GnssSetting.UartChannel, AMBA_UART_FLOW_CTRL_NONE, 1, &rx_buf, &read_size, 2000);
        if (read_size > 0) {
            unsigned char c = rx_buf;
            if (c == '$') {
                i = 0;
            }
            if (i >= MAX_GNSS_RAW) {
                i = 0;
                memset(input, 0, MAX_GNSS_RAW);
                continue;
            }
            input[i] = c;
            i++;
            if (c == '\n') {
                if (input[0] == '$') {
                    connected = 1;
#if defined(CONFIG_APP_FLOW_AONI_GNSS_MODULE)
                    gnss_parser_set_connected(1);
#endif
                    if (i >= MAX_GNSS_RAW) {
                    	i = 0;
                    	continue;
                    }
                    input[i] = '\0';
                    if (*(input + strlen(input) - 1) == '\n') {
                        *(input + strlen(input) - 1) = '\0';
                    }
                    if (*(input + strlen(input) - 1) == '\r') {
                        *(input + strlen(input) - 1) = '\0';
                    }
                    if (debug_enable) {
                        debug_line(input);
                    }
                    stream_share_send_data(0, (unsigned char *)input, strlen(input) + 1, 0);
                    if (dqa_test_script.gps_debug) {
                        static FILE *pFile = NULL;
                        if (pFile == NULL) {
                            pFile = fopen(SD_SLOT":\\gps_nmea.log", "w");
                        }
                        if (pFile) {
                            fwrite(input, strlen(input), 1, pFile);
                            fwrite("\n", 1, 1, pFile);
                            fflush(pFile);
                        }
                    }
#if defined(CONFIG_APP_FLOW_AONI_GNSS_MODULE)
                    gnss_parser_dispatch(input);
#endif
                    i = 0;
                }
            }
            if (i == MAX_GNSS_RAW) {
                i = 0;
            }
        } else {
            connected = 0;
#if defined(CONFIG_APP_FLOW_AONI_GNSS_MODULE)
            gnss_parser_set_connected(0);
            gnss_parser_reset_data();
#endif
            msleep(1);
        }
    }

    return NULL;
}

static AMBA_KAL_TASK_t gnss_debug_task;
#define GNSS_DEBUG_TASK_STACK_SIZE (32 * 1024)
static unsigned char gnss_debug_task_stack[GNSS_DEBUG_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static void *gnss_debug_task_entry(void *argv);
static int debug_running = 0;

int gnss_debug_task_start(void)
{
    unsigned int ret = 0;

    debug_running = 1;
    ret = AmbaKAL_TaskCreate(&gnss_debug_task,
                            "gnss_debug_task",
                            150,
                            (AMBA_KAL_TASK_ENTRY_f)gnss_debug_task_entry,
                            0,
                            gnss_debug_task_stack,
                            GNSS_DEBUG_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("gnss_debug_task create fail");
        debug_running = 0;
        return -1;
    }

    return 0;
}

int gnss_debug_task_stop(void)
{
    int Rval = 0;

    debug_running = 0;
    AmbaKAL_TaskSleep(200);
    Rval = AmbaKAL_TaskTerminate(&gnss_debug_task);
    if (Rval == OK) {
        Rval = AmbaKAL_TaskDelete(&gnss_debug_task);
    }

    return 0;
}

static void *gnss_debug_task_entry(void *argv)
{
    FILE *pFile = NULL;

    while (debug_running) {
        if (pFile == NULL) {
            pFile = fopen(SD_SLOT":\\gps_info.log", "w");
        }
        gnss_parser_debug_result(pFile);
        sleep(1);
    }

    return NULL;
}

