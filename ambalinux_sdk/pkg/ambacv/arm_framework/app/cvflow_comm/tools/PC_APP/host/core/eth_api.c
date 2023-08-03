/**
*  @file eth_api.c
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
#include <stdio.h>                /* for printf() */
#include <string.h>               /* for memset(), memcmp() */
#include <sys/time.h>             /* for gettimeofday() */

#include "AmbaHost_api.h"         /* for AmbaHost_connection_open() */
#include "AmbaHost_connection.h"  /* for get_hccb(), AmbaHost_xport_XXX() */
#include "AmbaEth_api.h"

typedef struct _ETH_CH {
    int32_t txCid;
    int32_t rxCid;
} ETH_CH_s;

#define GETTIMESTAMP(s, e)  ((e.tv_usec - s.tv_usec) + (e.tv_sec - s.tv_sec)*1000000)
#define MAX_ETH_CHANNEL     (MAX_CONNECTION_NUM >> 1)
#define TXRX_TEST_SIZE      (10 * 1024 * 1024)
#define MAX_HEADER_SIZE     (10)

static ETH_CH_s   g_channel[MAX_ETH_CHANNEL];
static uint32_t   g_ch_index = 0;

static const char HEADER_TX_TEST[]    = "+TEST";
static const char HEADER_SIZE_INFO[]  = "+SIZE";
static const char HEADER_TIME_STAMP[] = "+TIME";
static const char HEADER_DATA[]       = "+DATA";

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaEth_TxRxTest
 *
 *  @Description:: Tx/Rx throughput test
 *
 *  @Input      ::
 *    ch:          The channel id
 *    pBuff:       The test buffer
 *    size:        The test buffer size
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t AmbaEth_TxRxTest(uint32_t ch, char *pBuff, uint32_t size)
{
    int32_t rval = AMBA_ETH_ERR_OK;
    uint32_t test_size = TXRX_TEST_SIZE/2; // 5MB
    struct timeval tv_start[2], tv_end[2];
    ETH_CH_s *p_ch;
    hccb *handle_rx, *handle_tx;
    hXport xport_rx, xport_tx;

    /* 1. Sanity check */
    if (ch >= MAX_ETH_CHANNEL) {
        printf("[ERR]%s channel >= max limitation !\n", __func__);
        rval = AMBA_ETH_ERR_NG;
    }

    if (NULL == pBuff) {
        printf("[ERR]%s NULL pointer!\n", __func__);
        rval = AMBA_ETH_ERR_NG;
    }

    if (size < TXRX_TEST_SIZE) {
        printf("[ERR]%s Test buffer requirement: %u(%u)\n", __func__, TXRX_TEST_SIZE, size);
        rval = AMBA_ETH_ERR_NG;
    }

    if (AMBA_ETH_ERR_OK == rval) {
        p_ch = &g_channel[ch];
        handle_rx = get_hccb(p_ch->rxCid);
        if (handle_rx == NULL) {
            printf("[ERR]%s Cannot get rx handle!\n", __func__);
            rval = AMBA_ETH_ERR_NG;
        }
    }

    if (AMBA_ETH_ERR_OK == rval) {
        handle_tx = get_hccb(p_ch->txCid);
        if (handle_tx == NULL) {
            printf("[ERR]%s Cannot get tx handle!\n", __func__);
            rval = AMBA_ETH_ERR_NG;
        }
    }

    /* 2. RX Throughput(CV2x-->PC) */
    if (AMBA_ETH_ERR_OK == rval) {
        int32_t ret;
        char header_buf[MAX_HEADER_SIZE] = {0};

        xport_rx = handle_rx->xport;

        /* Receive Header, '+TEST' */
        AmbaHost_xport_lock(xport_rx);
        AmbaHost_xport_recv(xport_rx, header_buf, strlen(HEADER_TX_TEST));
        AmbaHost_xport_unlock(xport_rx);

        ret = memcmp((void *)header_buf, (void *)HEADER_TX_TEST, strlen(HEADER_TX_TEST));
        if (0 != ret) {
            printf("[ERR]%s Header %s != %s\n", __func__, header_buf, HEADER_TX_TEST);
            rval = AMBA_ETH_ERR_NG;
        }
    }

    if (AMBA_ETH_ERR_OK == rval) {
        int32_t ret;
        memset((void *)&pBuff[0], 0x00, test_size); // Reset buffer
        memset((void *)&pBuff[test_size], 0xAB, test_size); // RX comparing

        /* Receive Data; expected data is 0xAB */
        gettimeofday(&tv_start[0], NULL);
        AmbaHost_xport_lock(xport_rx);
        AmbaHost_xport_recv(xport_rx, &pBuff[0], test_size);
        AmbaHost_xport_unlock(xport_rx);
        gettimeofday(&tv_end[0], NULL);

        ret = memcmp((void *)&pBuff[0], (void *)&pBuff[test_size], test_size);
        if (0 != ret) {
            printf("[ERR]%s data is incorrect!\n", __func__);
            rval = AMBA_ETH_ERR_NG;
        }
    }

    /* 3. TX Throughput(PC-->CV2x) */
    if (AMBA_ETH_ERR_OK == rval) {
        xport_tx = handle_tx->xport;

        /* Send Header */
        AmbaHost_xport_lock(xport_tx);
        AmbaHost_xport_send(xport_tx, HEADER_TX_TEST, strlen(HEADER_TX_TEST));
        AmbaHost_xport_unlock(xport_tx);

        /* Send Data; expected data of CV2x is 0xCD */
        memset((void *)&pBuff[0], 0xCD, test_size); // Send 0xCD

        gettimeofday(&tv_start[1], NULL);
        AmbaHost_xport_lock(xport_tx);
        AmbaHost_xport_send(xport_tx, &pBuff[0], test_size);
        AmbaHost_xport_unlock(xport_tx);
        gettimeofday(&tv_end[1], NULL);
    }

    /* 4. Show RX/TX throughput */
    if (AMBA_ETH_ERR_OK == rval) {
        uint32_t mbps;
        mbps = (test_size * 8) / (GETTIMESTAMP(tv_start[0],tv_end[0]));
        printf("\n");
        if (mbps > 500) {
            printf("RX: Connection okay.\n");
        } else {
            printf("Poor speed!! RX: %uMbps", mbps);
        }
        mbps = (test_size * 8) / (GETTIMESTAMP(tv_start[1],tv_end[1]));
        if (mbps > 500) {
            printf("TX: Connection okay.\n");
        } else {
            printf("Poor speed!! TX: %uMbps", mbps);
        }
        printf("\n");
    }

    return rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaEth_GetSize
 *
 *  @Description:: Get size info from board side
 *
 *  @Input      ::
 *    ch:          The channel id
 *
 *  @Output     ::
 *    pSizeInfo:   The data size information
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t AmbaEth_GetSize(uint32_t ch, AMBA_ETH_SIZE_INFO_s *pSizeInfo)
{
    int32_t rval = AMBA_ETH_ERR_OK;
    ETH_CH_s *p_ch;
    hccb *handle_rx;
    hXport xport_rx;

    /* 1. Sanity check */
    if (ch >= MAX_ETH_CHANNEL) {
        printf("[ERR]%s channel >= max limitation !\n", __func__);
        rval = AMBA_ETH_ERR_NG;
    }

    if (NULL == pSizeInfo) {
        printf("[ERR]%s NULL pointer!\n", __func__);
        rval = AMBA_ETH_ERR_NG;
    }

    if (AMBA_ETH_ERR_OK == rval) {
        p_ch = &g_channel[ch];
        handle_rx = get_hccb(p_ch->rxCid);
        if (handle_rx == NULL) {
            printf("[ERR]%s Cannot get rx handle!\n", __func__);
            rval = AMBA_ETH_ERR_NG;
        }
    }

    /* 2. Receive Header, '+SIZE' */
    if (AMBA_ETH_ERR_OK == rval) {
        int32_t ret;
        char header_buf[MAX_HEADER_SIZE] = {0};

        xport_rx = handle_rx->xport;

        AmbaHost_xport_lock(xport_rx);
        AmbaHost_xport_recv(xport_rx, header_buf, strlen(HEADER_SIZE_INFO));
        AmbaHost_xport_unlock(xport_rx);

        ret = memcmp((void *)header_buf, (void *)HEADER_SIZE_INFO, strlen(HEADER_SIZE_INFO));
        if (0 != ret) {
            printf("[ERR]%s Header %s != %s\n", __func__, header_buf, HEADER_SIZE_INFO);
            rval = AMBA_ETH_ERR_NG;
        }
    }

    /* 3. Receive Data, size information */
    if (AMBA_ETH_ERR_OK == rval) {
        char *pBuf = (char *)pSizeInfo;

        AmbaHost_xport_lock(xport_rx);
        AmbaHost_xport_recv(xport_rx, pBuf, sizeof(AMBA_ETH_SIZE_INFO_s));
        AmbaHost_xport_unlock(xport_rx);
    }

    return rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaEth_Recv
 *
 *  @Description:: Receive data from board side
 *
 *  @Input      ::
 *    ch:          The channel id
 *    pSizeInfo:   The data size information
 *
 *  @Output     ::
 *    pDataInfo:   The data information
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t AmbaEth_Recv(uint32_t ch, const AMBA_ETH_SIZE_INFO_s *pSizeInfo, AMBA_ETH_DATA_INFO_s *pDataInfo)
{
    int32_t rval = AMBA_ETH_ERR_OK;
    uint32_t i;
    ETH_CH_s *p_ch;
    hccb *handle_rx;
    hXport xport_rx;

    /* 1. Sanity check */
    if (ch >= MAX_ETH_CHANNEL) {
        printf("[ERR]%s channel >= max limitation !\n", __func__);
        rval = AMBA_ETH_ERR_NG;
    }

    if ((NULL == pSizeInfo) || (NULL == pDataInfo)) {
        printf("[ERR]%s NULL pointer!\n", __func__);
        rval = AMBA_ETH_ERR_NG;
    }

    if (AMBA_ETH_ERR_OK == rval) {
        if ((pSizeInfo->num > AMBA_ETH_MAX_IO) || (pSizeInfo->num == 0)) {
            printf("[ERR]%s SizeInfo Num: %u\n", __func__, pSizeInfo->num);
            rval = AMBA_ETH_ERR_NG;
        }
    }

    if (AMBA_ETH_ERR_OK == rval) {
        p_ch = &g_channel[ch];
        handle_rx = get_hccb(p_ch->rxCid);
        if (handle_rx == NULL) {
            printf("[ERR]%s Cannot get rx handle!\n", __func__);
            rval = AMBA_ETH_ERR_NG;
        }
    }

    /* 2. Receive time stamp */
    if (AMBA_ETH_ERR_OK == rval) {
        int32_t ret;
        char header_buf[MAX_HEADER_SIZE] = {0};

        xport_rx = handle_rx->xport;

        /* Header, '+TIME' */
        AmbaHost_xport_lock(xport_rx);
        AmbaHost_xport_recv(xport_rx, header_buf, strlen(HEADER_TIME_STAMP));
        AmbaHost_xport_unlock(xport_rx);

        ret = memcmp((void *)header_buf, (void *)HEADER_TIME_STAMP, strlen(HEADER_TIME_STAMP));
        if (0 != ret) {
            printf("[ERR]%s Header %s != %s\n", __func__, header_buf, HEADER_TIME_STAMP);
            rval = AMBA_ETH_ERR_NG;
        }
    }

    if (AMBA_ETH_ERR_OK == rval) {
        char *pBuf = (char *)(&(pDataInfo->seqNum));

        /* SeqNum */
        AmbaHost_xport_lock(xport_rx);
        AmbaHost_xport_recv(xport_rx, pBuf, sizeof(pDataInfo->seqNum));
        AmbaHost_xport_unlock(xport_rx);

        /* TimeStamp */
        pBuf = (char *)(&(pDataInfo->timeStamp));
        AmbaHost_xport_lock(xport_rx);
        AmbaHost_xport_recv(xport_rx, pBuf, sizeof(pDataInfo->timeStamp));
        AmbaHost_xport_unlock(xport_rx);
    }

    /* 3. Receive data */
    if (AMBA_ETH_ERR_OK == rval) {
        for (i = 0; i < pSizeInfo->num; i++) {

            /* Header, '+DATA' */
            if (AMBA_ETH_ERR_OK == rval) {
                int32_t ret;
                char header_buf[MAX_HEADER_SIZE] = {0};

                AmbaHost_xport_lock(xport_rx);
                AmbaHost_xport_recv(xport_rx, header_buf, strlen(HEADER_DATA));
                AmbaHost_xport_unlock(xport_rx);

                ret = memcmp((void *)header_buf, (void *)HEADER_DATA, strlen(HEADER_DATA));
                if (0 != ret) {
                    printf("[ERR]%s Header %s != %s\n", __func__, header_buf, HEADER_DATA);
                    rval = AMBA_ETH_ERR_NG;
                }
            }

            /* Data */
            if (AMBA_ETH_ERR_OK == rval) {
                char *pBuf = pDataInfo->pBuf[i];
                AmbaHost_xport_lock(xport_rx);
                AmbaHost_xport_recv(xport_rx, pBuf, pSizeInfo->size[i]);
                AmbaHost_xport_unlock(xport_rx);
            }

            /* Check RX status */
            if (AMBA_ETH_ERR_NG == rval) {
                break;
            }
        }
    }

    return rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaEth_Send
 *
 *  @Description:: Send size info and data to board side
 *
 *  @Input      ::
 *    ch:          The channel id
 *    pSizeInfo:   The data size information
 *    pDataInfo:   The data information
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t AmbaEth_Send(uint32_t ch, const AMBA_ETH_SIZE_INFO_s *pSizeInfo, const AMBA_ETH_DATA_INFO_s *pDataInfo)
{
    int32_t rval = AMBA_ETH_ERR_OK;
    uint32_t i;
    ETH_CH_s *p_ch;
    hccb *handle_tx;
    hXport xport_tx;

    /* 1. Sanity check */
    if (ch >= MAX_ETH_CHANNEL) {
        printf("[ERR]%s channel >= max limitation !\n", __func__);
        rval = AMBA_ETH_ERR_NG;
    }

    if ((NULL == pSizeInfo) || (NULL == pDataInfo)) {
        printf("[ERR]%s NULL pointer!\n", __func__);
        rval = AMBA_ETH_ERR_NG;
    }

    if (AMBA_ETH_ERR_OK == rval) {
        if ((pSizeInfo->num > AMBA_ETH_MAX_IO) || (pSizeInfo->num == 0)) {
            printf("[ERR]%s SizeInfo Num: %u\n", __func__, pSizeInfo->num);
            rval = AMBA_ETH_ERR_NG;
        }
    }

    if (AMBA_ETH_ERR_OK == rval) {
        p_ch = &g_channel[ch];
        handle_tx = get_hccb(p_ch->txCid);
        if (handle_tx == NULL) {
            printf("[ERR]%s Cannot get tx handle!\n", __func__);
            rval = AMBA_ETH_ERR_NG;
        }
    }

    /* 2. Send */
    if (AMBA_ETH_ERR_OK == rval) {
        char *pBuf;
        uint32_t seq_num;

        xport_tx = handle_tx->xport;
        /* Header, '+SIZE' */
        AmbaHost_xport_lock(xport_tx);
        AmbaHost_xport_send(xport_tx, HEADER_SIZE_INFO, strlen(HEADER_SIZE_INFO));
        AmbaHost_xport_unlock(xport_tx);

        /* Size Info */
        pBuf = (char *)pSizeInfo;
        AmbaHost_xport_lock(xport_tx);
        AmbaHost_xport_send(xport_tx, pBuf, sizeof(AMBA_ETH_SIZE_INFO_s));
        AmbaHost_xport_unlock(xport_tx);

        /* Header, '+TIME' */
        AmbaHost_xport_lock(xport_tx);
        AmbaHost_xport_send(xport_tx, HEADER_TIME_STAMP, strlen(HEADER_TIME_STAMP));
        AmbaHost_xport_unlock(xport_tx);

        /* SeqNum */
        pBuf = (char *)(&(seq_num));
        AmbaHost_xport_lock(xport_tx);
        seq_num = AmbaHost_xport_next_seqnum(xport_tx);
        AmbaHost_xport_send(xport_tx, pBuf, sizeof(seq_num));
        AmbaHost_xport_unlock(xport_tx);
        /* TimeStamp */
        pBuf = (char *)(&(pDataInfo->timeStamp));
        AmbaHost_xport_lock(xport_tx);
        AmbaHost_xport_send(xport_tx, pBuf, sizeof(pDataInfo->timeStamp));
        AmbaHost_xport_unlock(xport_tx);

        for (i = 0; i < pSizeInfo->num; i++) {
            char *pBuf = pDataInfo->pBuf[i];

            /* Header, '+DATA' */
            AmbaHost_xport_lock(xport_tx);
            AmbaHost_xport_send(xport_tx, HEADER_DATA, strlen(HEADER_DATA));
            AmbaHost_xport_unlock(xport_tx);

            /* Data */
            AmbaHost_xport_lock(xport_tx);
            AmbaHost_xport_send(xport_tx, pBuf, pSizeInfo->size[i]);
            AmbaHost_xport_unlock(xport_tx);
        }
    }

    return rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaEth_Init
 *
 *  @Description:: Create a client socket to reach out to the server socket
 *
 *  @Input      ::
 *    pAddr:       The server's IP
 *
 *  @Output     ::
 *    pCh:         The communication channel id
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t AmbaEth_Init(const char *pAddr, uint32_t *pCh)
{
    int32_t rval = AMBA_ETH_ERR_OK;
    ETH_CH_s *p_ch;

    /* Sanity check */
    if (g_ch_index >= MAX_ETH_CHANNEL) {
        printf("[ERR]%s channel >= max limitation !\n", __func__);
        rval = AMBA_ETH_ERR_NG;
    }

    if ((NULL == pAddr) || (NULL == pCh)) {
        printf("[ERR]%s NULL pointer!\n", __func__);
        rval = AMBA_ETH_ERR_NG;
    }

    if (AMBA_ETH_ERR_OK == rval) {
        *pCh = g_ch_index;
        p_ch = &g_channel[*pCh];
        printf("Connect to RX ...\n");
        p_ch->rxCid = AmbaHost_connection_open(pAddr, 0);
        if (p_ch->rxCid <= AMBA_ETH_ERR_OK) {
            printf("[ERR]%s AmbaHst_connection_open fails with err %d\n", __func__, p_ch->rxCid);
            rval = AMBA_ETH_ERR_NG;
        }
    }

    if (AMBA_ETH_ERR_OK == rval) {
        printf("Connect to TX ...\n");
        p_ch->txCid = AmbaHost_connection_open(pAddr, 0);
        if (p_ch->txCid <= AMBA_ETH_ERR_OK) {
            printf("[ERR]%s AmbaHost_connection_open fails with err %d\n", __func__, p_ch->txCid);
            rval = AMBA_ETH_ERR_NG;
        }
    }

    if (AMBA_ETH_ERR_OK == rval) {
        printf("Channel %u is connected.\n", *pCh);
        g_ch_index++;
    }

    return rval;
}

