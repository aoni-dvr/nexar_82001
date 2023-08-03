/**
*  @file ut2.c
*
* Copyright (c) [2020] Ambarella International LP
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
* This file includes sample code and is only for internal testing and evaluation.  If you
* distribute this sample code (whether in source, object, or binary code form), it will be
* without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
*  @detail xxxx
*
*/
#include <stdio.h>            /* for printf() */
#include <stdlib.h>           /* for malloc() */
#include <string.h>           /* for memset(), memcmp() */
#include <pthread.h>          /* for pthread_create() */
#include <unistd.h>           /* for usleep() */
#include <time.h>             /* for clock() */

#include "AmbaEth_api.h"      /* Amba Ethernet API */

#define UT2_STRING_WARN     "[UT2][WARN]"
#define UT2_STRING_ERR      "[UT2][ERR]"
#define UT2_STRING          "[UT2]"
#define DUMP_FILENAME       "RXDUMP"

#define MAX_FILENAME_LEN    (256)
#define MAX_TX_DEPTH        (4)   /* feeding speed control */

uint32_t g_ch;

typedef struct {
    uint32_t  num;
    uint32_t  size[AMBA_ETH_MAX_IO];
    void      *buf[AMBA_ETH_MAX_IO];
} ETH_UT2_BUF_s;

typedef struct {
    ETH_UT2_BUF_s in;
    ETH_UT2_BUF_s out_golden;   /* CNN output golden data */
} ETH_UT2_DATA_s;

static ETH_UT2_DATA_s g_data;
static uint32_t       g_check_result_flag;
static uint32_t       g_dump_rx_flag;
static pthread_t      g_tx_thread, g_rx_thread;

static uint32_t available_tx = MAX_TX_DEPTH;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*---------------------------------------------------------------------------*\
 * Static functions
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: read_settings
 *
 *  @Description:: Read settings
 *
 *  @Input      ::
 *     key:        KEY string
 *
 *  @Output     ::
 *     value:      VALUE string
 *
 *  @Return     ::
 *     int32_t     OK(0)/NG(-1)
\*---------------------------------------------------------------------------*/
static int32_t read_settings(char* key, char* value)
{
    FILE *fp;
    char *line = NULL;
    char *pch = NULL;
    size_t len = 0;
    ssize_t read;
    int ret = -1;

    fp = fopen("./settings.txt", "r");
    if (fp == NULL) {
        printf("%s %s No settings.txt\n", UT2_STRING_ERR, __func__);
        return ret;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        pch = strstr(line, key);
        /* get 'key' */
        if (pch) {
            unsigned int index = strlen(key);
            /* skip space character */
            while((0x20 == line[index]) && (index < strlen(line))) {
                index++;
            }

            if (index < strlen(line)) {
                if (line[strlen(line)-1] == '\n') {
                    line[strlen(line)-1] = '\0';
                }
                /* get 'value' */
                strcpy(value, (const char*)(line+index));
                printf("%s Key: %s, Value: %s\n", UT2_STRING, key, value);
                ret = 0;
            } else {
                printf("%s Key, %s, cannnot find value!!\n", UT2_STRING_ERR, key);
            }
            break;
        }
    }

    fclose(fp);
    if (line) {
        free(line);
    }

    return ret;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: load_test_files
 *
 *  @Description:: load input files and golden data
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*---------------------------------------------------------------------------*/
static void load_test_files()
{
    char inKey[64]        = "INPUT0";
    char outGoldenKey[64] = "OUTPUT0";
    char dumpRxKey[64]    = "DUMPRX";
    char valueString[128] = {0};
    FILE *fp;
    int bytes;
    int ret = 0;
    ETH_UT2_DATA_s *p_data = &g_data;

    /* CNN Input File */
    p_data->in.num = 0;
    for (uint32_t i = 0; i < AMBA_ETH_MAX_IO; i++) {
        inKey[5] = '0' + i;
        /* 1.Read Key value */
        ret = read_settings(inKey, valueString);
        if (0 == ret) {
            p_data->in.num++;
            /* 2. Open file */
            fp = fopen(valueString, "rb");
            if (fp == NULL) {
                printf("%s %s open file %s fail\n", UT2_STRING_ERR, __func__, valueString);
                exit(-1);
            }
            /* 3. Read size */
            fseek(fp, 0L, SEEK_END);
            p_data->in.size[i] = ftell(fp);
            fseek(fp, 0L, SEEK_SET);
            printf("%s file %s size %u\n", UT2_STRING, valueString, p_data->in.size[i]);
            /* 4. Allocate mem */
            p_data->in.buf[i] = malloc(p_data->in.size[i]);       /* for send image test */
            if (NULL == p_data->in.buf[i]) {
                printf("%s %s malloc fail\n", UT2_STRING_ERR, __func__);
                exit(-1);
            }
            /* 5. Load file to mem */
            bytes = fread(p_data->in.buf[i], 1, p_data->in.size[i], fp);
            if (bytes != p_data->in.size[i]) {
                printf("%s %s read file %s fail\n", UT2_STRING_ERR, __func__, valueString);
                exit(-1);
            }
            fclose(fp);
        } else {
            printf("%s NN in num %u\n", UT2_STRING, p_data->in.num);
            break;
        }
    }

    if (0 == p_data->in.num) {
        printf("%s %s No input!!\n", UT2_STRING_ERR, __func__);
        exit(-1);
    }

    /* CNN Output Golden Data File */
    p_data->out_golden.num = 0;
    for (uint32_t i = 0; i < AMBA_ETH_MAX_IO; i++) {
        outGoldenKey[6] = '0' + i;
        /* 1.Read Key value */
        ret = read_settings(outGoldenKey, valueString);
        if (0 == ret) {
            p_data->out_golden.num++;
            /* 2. Open file */
            fp = fopen(valueString, "rb");
            if (fp == NULL) {
                printf("%s %s open file %s fail\n", UT2_STRING_WARN, __func__, valueString);
                p_data->out_golden.num = 0;
                break;
            }
            /* 3. Read size */
            fseek(fp, 0L, SEEK_END);
            p_data->out_golden.size[i] = ftell(fp);
            fseek(fp, 0L, SEEK_SET);
            printf("%s file %s size %u\n", UT2_STRING, valueString, p_data->out_golden.size[i]);
            /* 4. Allocate mem */
            p_data->out_golden.buf[i] = malloc(p_data->out_golden.size[i]);
            if (NULL == p_data->out_golden.buf[i]) {
                printf("%s %s malloc fail\n", UT2_STRING_ERR, __func__);
                exit(-1);
            }
            /* 5. Load file to mem */
            bytes = fread(p_data->out_golden.buf[i], 1, p_data->out_golden.size[i], fp);
            if (bytes != p_data->out_golden.size[i]) {
                printf("%s %s read file %s fail\n", UT2_STRING_ERR, __func__, valueString);
                exit(-1);
            }
            fclose(fp);
        } else {
            printf("%s NN out_golden num %u\n", UT2_STRING, p_data->out_golden.num);
            break;
        }
    }

    if (0 == p_data->out_golden.num) {
        printf("%s %s Skip check result!!\n", UT2_STRING_WARN, __func__);
        g_check_result_flag = 0;
    } else {
        g_check_result_flag = 1;
    }

    /* Dump Rx Data for debug */
    {
        ret = read_settings(dumpRxKey, valueString);
        if (0 == ret) {
            ret = memcmp((void *)valueString, (void *)"1", strlen("1"));
            if (0 == ret) {
                g_dump_rx_flag = 1;
                printf("%s Enable dump rx data\n", UT2_STRING);
            }
        }

    }
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: dump_data2file
 *
 *  @Description:: save data to file
 *
 *  @Input      ::
 *    size:        The saved size
 *    p_file_name: The saved file path
 *    p_buf:       The pointer to saved buffer
 *
 *  @Output     ::
 *    p_write_size: The total length to save
 *
 *  @Return     :: OK(0)/NG(-1)
\*---------------------------------------------------------------------------*/
uint32_t dump_data2file(void *p_buf, uint32_t size, const char *p_file_name, uint32_t *p_write_size)
{
    FILE *fp = NULL;
    uint32_t write_size = 0U;
    uint32_t ret = AMBA_ETH_ERR_OK;

    fp = fopen(p_file_name, "wb");
    if (fp != NULL) {
        write_size = fwrite(p_buf, 1, size, fp);
        if (write_size == 0U) {
            printf("%s %s fwrite fail\n", UT2_STRING_ERR, __func__);
            ret = AMBA_ETH_ERR_NG;
        } else {
            *p_write_size = write_size;
        }
        fclose(fp);
    } else {
        printf("%s %s fopen fail\n", UT2_STRING_ERR, __func__);
        ret = AMBA_ETH_ERR_NG;
    }

    return ret;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: get_tx_token
 *
 *  @Description:: get tx token for sending data
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*---------------------------------------------------------------------------*/
static inline void get_tx_token()
{
    uint32_t wait_time = 10000; // 10ms
    do {
        pthread_mutex_lock(&mutex);

        if (available_tx > 0) {
            available_tx--;
            pthread_mutex_unlock(&mutex);
            break;
        } else {
            pthread_mutex_unlock(&mutex);
            //printf("[TOKEN] try to sleep\n");
            usleep(wait_time);
        }
    } while (1);
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: free_tx_token
 *
 *  @Description:: free tx token
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*---------------------------------------------------------------------------*/
static inline void free_tx_token()
{
    pthread_mutex_lock(&mutex);
    if (available_tx >= MAX_TX_DEPTH) {
        printf("%s %s can't free token due to incorrect available_tx %u\n", UT2_STRING_ERR, __func__, available_tx);
        exit(-1);
    } else {
        available_tx++;
    }
    pthread_mutex_unlock(&mutex);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: tx_thread
 *
 *  @Description:: Send data to CV2x
 *
 *  @Input      ::
 *      arg:       The thread arg
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void* tx_thread(void *arg)
{
    int32_t ret = AMBA_ETH_ERR_OK;
    ETH_UT2_DATA_s *p_data = &g_data;

    do {
        AMBA_ETH_SIZE_INFO_s size_info = {0};
        AMBA_ETH_DATA_INFO_s data_info = {0};
        clock_t cur_clock;

        /* Tx speed control */
        get_tx_token();

        /* Fill size info & DataInfo */
        size_info.num = p_data->in.num;
        // data_info.seqNum will be assigned value internally in AmbaEth_Send()
        cur_clock = clock();
        data_info.timeStamp = (uint64_t)cur_clock;
        for (uint32_t i = 0; i < size_info.num; i++) {
            size_info.size[i] = p_data->in.size[i];
            data_info.pBuf[i] = (char *)p_data->in.buf[i];
        }

        /* Test: Always send same test files */
        ret = AmbaEth_Send(g_ch, &size_info, &data_info);
        if (AMBA_ETH_ERR_OK == ret) {
            static uint32_t seq = 0;
            printf("[TX] Seq: %u, TimeStamp:%lu\n", seq, data_info.timeStamp);
            seq++;
        }
    } while (AMBA_ETH_ERR_OK == ret);


    printf("%s Tx thread Done!!\n", UT2_STRING);
    return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: rx_thread
 *
 *  @Description:: Receive data from CV2x
 *
 *  @Input      ::
 *      arg:       The thread arg
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void* rx_thread(void *arg)
{
    int32_t ret = AMBA_ETH_ERR_OK;
    ETH_UT2_DATA_s *p_data = &g_data;

    do {
        AMBA_ETH_SIZE_INFO_s size_info = {0};
        AMBA_ETH_DATA_INFO_s data_info = {0};
        void *out_buf[AMBA_ETH_MAX_IO] = {0};

        /* 1. Get size info from remote PC */
        ret = AmbaEth_GetSize(g_ch, &size_info);

        /* 2. Sanity check */
        if (AMBA_ETH_ERR_OK == ret) {
            if (1 == g_check_result_flag) {
                if (p_data->out_golden.num != size_info.num) {
                    printf("%s %s out_golden_num doess not equal to size_info.num (%u, %u)\n", UT2_STRING_ERR, __func__, p_data->out_golden.num, size_info.num);
                    exit(-1);
                }
                for (uint32_t i = 0; i < size_info.num; i++) {
                    if (p_data->out_golden.size[i] != size_info.size[i]) {
                        printf("%s %s out_golden_size[%u] doess not equal to size_info.size[%u] (%u, %u)\n", UT2_STRING_ERR, __func__, i, i, p_data->out_golden.size[i], size_info.size[i]);
                        exit(-1);
                    }
                }
            }
        }

        /* 3. Allocate mem for receving data */
        if (AMBA_ETH_ERR_OK == ret) {
            for (uint32_t i = 0; i < size_info.num; i++) {
                out_buf[i] = malloc(size_info.size[i]);
                if (NULL == out_buf[i]) {
                    printf("%s %s malloc fail\n", UT2_STRING_ERR, __func__);
                    exit(-1);
                }
                /* Assign RX buffer address */
                data_info.pBuf[i] = (char *)out_buf[i];
            }
        }

        /* 4. Receive data from CV2x */
        if (AMBA_ETH_ERR_OK == ret) {
            ret = AmbaEth_Recv(g_ch, &size_info, &data_info);
        }

        /* 5. Dump Rx data to file for debug */
        if ((AMBA_ETH_ERR_OK == ret) && (1 == g_dump_rx_flag)) {
            static uint32_t file_idx = 0U;
            char file_name[MAX_FILENAME_LEN] = {0};  // format = RXDUMP_IOIDX_FILEIDX.out
            char file_idx_str[4U];
            char io_node_idx_str[3U];
            uint32_t write_size = 0;

            file_idx_str[0U] = '0' + ((file_idx / 100U) % 10U);
            file_idx_str[1U] = '0' + ((file_idx / 10U) % 10U);
            file_idx_str[2U] = '0' + (file_idx % 10U);
            file_idx_str[3U] = '\0';

            for (uint32_t i = 0; i < size_info.num; i++) {
                strcpy(file_name, DUMP_FILENAME);
                strcat(file_name, "_");
                io_node_idx_str[0U] = '0' + ((i / 10U) % 10U);
                io_node_idx_str[1U] = '0' + (i % 10U);
                io_node_idx_str[2U] = '\0';
                strcat(file_name, io_node_idx_str);
                strcat(file_name, "_");
                strcat(file_name, file_idx_str);
                strcat(file_name, ".out");

                printf("[RX] save filename: %s\n", file_name);
                ret = dump_data2file((void *)data_info.pBuf[i], size_info.size[i], file_name, &write_size);
                if (ret == AMBA_ETH_ERR_OK) {
                    if (write_size != size_info.size[i]) {
                        printf("%s %s dump_data2file fail (incorrect size (write=%u, out_data=%u)\n",
                               UT2_STRING_ERR, __func__, write_size, size_info.size[i]);
                        ret = AMBA_ETH_ERR_NG;
                        exit(-1);
                    }
                }
            }

            file_idx ++;
            if (file_idx > 999U) {
                file_idx = 0U;
            }
        }

        /* 6. Compare to golden */
        if ((AMBA_ETH_ERR_OK == ret) && (1 == g_check_result_flag)) {
            for (uint32_t i = 0; i < size_info.num; i++) {
                ret = memcmp(p_data->out_golden.buf[i], (void *)data_info.pBuf[i], size_info.size[i]);
                if (AMBA_ETH_ERR_OK == ret) {
                    //printf("[RX] Data%u is correct!\n", i);
                } else {
                    printf("%s %s Data%u is incorrect!!! ret %d\n", UT2_STRING_ERR, __func__, i, ret);
                    exit(-1);
                }
            }
        }

        /* 7. Free resource */
        if (AMBA_ETH_ERR_OK == ret) {
            printf("[RX] Seq: %u, TimeStamp:%lu\n", data_info.seqNum, data_info.timeStamp);
            free_tx_token();
            for (uint32_t i = 0; i < size_info.num; i++) {
                free(out_buf[i]);
            }
        }

    } while (AMBA_ETH_ERR_OK == ret);

    printf("%s Rx thread Done!!\n", UT2_STRING);
    return NULL;

}


int32_t main(int32_t argc, char **argv)
{
    int32_t ret = 0;

    do {
        if (2 != argc) {
            printf("%s %s input parameter(%d) is not 1\n", UT2_STRING_ERR, __func__, argc - 1);
            break;
        }

        /* 1. Connect to CV2x */
        ret = AmbaEth_Init(argv[1], &g_ch);
        if (ret == AMBA_ETH_ERR_NG) {
            break;
        }

        /* 2. Throughput test */
        {
            void *test_buf;
            uint32_t size = 10 * 1024 * 1024;

            test_buf = malloc(size);
            ret = AmbaEth_TxRxTest(g_ch, (char *)test_buf, size);
            free(test_buf);
            if (ret == AMBA_ETH_ERR_NG) {
                break;
            }
        }

        /* 3. Load test files */
        load_test_files();

        /* 4. Start to TX/RX */
        pthread_create(&g_rx_thread, NULL, rx_thread, NULL);
        pthread_create(&g_tx_thread, NULL, tx_thread, NULL);
        pthread_join(g_tx_thread, NULL);
        printf("%s Tx thread terminated!\n", UT2_STRING);
        pthread_join(g_rx_thread, NULL);
        printf("%s Rx thread terminated!\n", UT2_STRING);

    } while(0);

    return ret;
}

