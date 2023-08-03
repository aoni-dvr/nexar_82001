#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "AmbaTypes.h"

#include "src/AmbaENET.c"

#define AMBA_COPY     (0U)
#define AMBA_NOCOPY   (1U)

UINT32 PhyInitCb(const UINT32 PhyId);
UINT32 LinkUpCb(const UINT32 Idx, UINT32 LinkSpeed);
UINT32 LinkDownCb(const UINT32 Idx);
UINT32 RxCb(const UINT32 Idx, const UINT16 RxLen);
static void init_locked(UINT32 val);

UINT32 AmbaKAL_SemaphoreCreate_array[5] = {0};
UINT32 AmbaKAL_SemaphoreCreate_array_cnt = 0;
UINT32 GcovrAlarmFlag = 0U;

AMBA_ENET_TS_s g_enet_ts = { .Sec = 0, .Ns = 0 };
UINT32 g_u32_buf[1];
UINT8 g_u8_buf[1];
AMBA_ENET_DMA_s g_pRxDma = { .Buf = g_u8_buf, .Locked = 1U };
AMBA_ENET_DMA_s g_pTxDma = { .Buf = g_u8_buf, .Locked = 1U };
AMBA_ENET_TDES_s g_pTDES[1] = { [0]={.TDES2_Buf1 = 1U, .TDES0 = 1U} };
AMBA_ENET_RDES_s g_pRDES[1] = { [0]={.RDES2_Buf1 = 1U, .RDES0 = 1U} };
AMBA_ENET_DES_s g_pDes = { .FrameSize = 1U, .TDESCnt = 1, .RDESCnt = 1, .pTxDma = &g_pTxDma, .pRxDma = &g_pRxDma, .pTDES = (AMBA_ENET_TDES_s*)&g_pTDES, .pRDES = (AMBA_ENET_RDES_s*)&g_pRDES };
AMBA_ENET_CONFIG_s g_pAmbaEnetConfig = { .pDes = &g_pDes, .LinkSpeed = 1U, .pPhyInitCb = PhyInitCb, .pLinkUp = LinkUpCb, .pLinkDown = LinkDownCb, .pRxCb = RxCb };

static UINT32 AmbaCSL_EnetTDESGetDmaOwn_count = 0;
static UINT32 AmbaKAL_TaskSleep_FuncSel = 0;
static UINT32 IO_UtilityCrc32_flag = 0;

extern void Set_RetVal_AmbaRTSL_EnetGetPhyAddr(UINT32 RetVal);
extern void Set_NoCopy_AmbaWrap_memcpy(UINT32 RetVal);
extern UINT32 Get_RetVal_EnetTDESGetDmaOwn(void);
extern void Set_RetVal_EnetRDESGetDmaOwn(UINT32 RetVal);
extern void Set_RetVal_EnetTDESGetDmaOwn(UINT32 RetVal);
extern void Set_RetVal_EnetTDESGetDmaES(UINT32 RetVal);
extern void Set_RetVal_EnetDmaReset(UINT32 RetVal);
extern void Set_RetVal_EnetMacGetTSAR(UINT32 RetVal);
extern void Set_RetVal_EnetTDESGetTTSS(UINT32 RetVal);
extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_SemaphoreTake(UINT32 RetVal);
extern void (*fpCB_EnetGetPhyAddr)(void);
extern void (*fpCB_EnetRDESGetDmaOwn)(void);
extern void (*fpCB_EnetTDESGetDmaOwn)(void);
extern void (*fpCB_EnetTDESGetDmaES)(void);
extern void (*fpCB_EnetDmaReset)(void);
extern void (*fpCB_EnetMacGetTSAR)(void);
extern void (*fpCB_EnetTDESGetTTSS)(void);
extern void (*fpCB_TaskSleep)(void);
extern void (*fpCB_MutexCreate)(void);
extern void (*fpCB_SemaphoreCreate)(void);
extern void (*fpCB_SemaphoreTake)(void);

static void handler(INT32 v)
{
    printf("[Alarm] Timer out!! Force it leave the infinite loop.\n");
    GcovrAlarmFlag = 1U;
}

static void setAlarm(void)
{
    signal(SIGALRM, handler);
    alarm(1);
    printf("[Alarm] Set 1 sec alarm for leaving infinite loop.\n");
}

/****************************************************/
/*                Callback Func                     */
/****************************************************/
static UINT32 UserPreInitCb(const UINT32 Idx)
{
    return 0;
}

UINT32 PhyInitCb(const UINT32 PhyId)
{
    return 0;
}

UINT32 LinkUpCb(const UINT32 Idx, UINT32 LinkSpeed)
{
    return 0;
}

UINT32 LinkDownCb(const UINT32 Idx)
{
    return 0;
}

UINT32 RxCb(const UINT32 Idx, const UINT16 RxLen)
{
    return 0;
}

void CB_EnetGetPhyAddr(void)
{
    Set_RetVal_AmbaRTSL_EnetGetPhyAddr(0U);
}

void CB_EnetRDESGetDmaOwn(void)
{
    Set_RetVal_EnetRDESGetDmaOwn(0U);
}

void CB_EnetTDESGetDmaOwn(void)
{
    if (Get_RetVal_EnetTDESGetDmaOwn() == 1U) {
        if (AmbaCSL_EnetTDESGetDmaOwn_count > 0) {
            AmbaCSL_EnetTDESGetDmaOwn_count--;
        } else {
            Set_RetVal_EnetTDESGetDmaOwn(0U);
        }
    }
}

void CB_EnetTDESGetDmaES(void)
{
    Set_RetVal_EnetTDESGetDmaES(0U);
}

void CB_EnetDmaReset(void)
{
    Set_RetVal_EnetDmaReset(0U);
}

void CB_EnetMacGetTSAR(void)
{
    Set_RetVal_EnetMacGetTSAR(0U);
}

void CB_EnetTDESGetTTSS(void)
{
    Set_RetVal_EnetTDESGetTTSS(0U);
}

void CB_TaskSleep(void)
{
    if (AmbaKAL_TaskSleep_FuncSel == 1) {
        init_locked(0);
        AmbaKAL_TaskSleep_FuncSel = 0;
    } else if (AmbaKAL_TaskSleep_FuncSel == 2) {
        EnetInited[0] = 1;
        AmbaKAL_TaskSleep_FuncSel = 0;
    }
}

void CB_MutexCreate(void)
{
    Set_RetVal_MutexCreate(0U);
}

UINT32 CB_SemaphoreCreate(void)
{
    if (AmbaKAL_SemaphoreCreate_array_cnt != 0U) {
        AmbaKAL_SemaphoreCreate_array_cnt--;
        return AmbaKAL_SemaphoreCreate_array[AmbaKAL_SemaphoreCreate_array_cnt];
    }
    return 0U;
}

void CB_SemaphoreTake(void)
{
    Set_RetVal_SemaphoreTake(0U);
}

/****************************************************/
/*                init Func                         */
/****************************************************/
static void init(void)
{
    fpCB_EnetRDESGetDmaOwn = CB_EnetRDESGetDmaOwn;
    fpCB_EnetTDESGetDmaOwn = CB_EnetTDESGetDmaOwn;
    fpCB_EnetTDESGetDmaES  = CB_EnetTDESGetDmaES;
    fpCB_EnetDmaReset      = CB_EnetDmaReset;
    fpCB_EnetMacGetTSAR    = CB_EnetMacGetTSAR;
    fpCB_EnetTDESGetTTSS   = CB_EnetTDESGetTTSS;
    fpCB_TaskSleep         = CB_TaskSleep;
    fpCB_MutexCreate       = CB_MutexCreate;
    fpCB_SemaphoreCreate   = CB_SemaphoreCreate;
    fpCB_SemaphoreTake     = CB_SemaphoreTake;
}

static void init_pAmbaEnetConfig(void)
{
    UINT32 i;
    for (i = 0; i < ENET_INSTANCES; i++) {
        pAmbaEnetConfig[i] = &g_pAmbaEnetConfig;
    }
}

static void deinit_pAmbaEnetConfig(void)
{
    UINT32 i;
    for (i = 0; i < ENET_INSTANCES; i++) {
        pAmbaEnetConfig[i] = NULL;
    }

}

static void init_locked(UINT32 val)
{
    UINT32 i;
    for (i = 0; i < ENET_INSTANCES; i++) {
        pAmbaEnetConfig[i]->pDes->pTxDma[0].Locked = val;
    }
}

/****************************************************/
/*                Local Stub Func                   */
/****************************************************/
UINT32 IO_UtilityCrc32(const UINT8 *pBuffer, UINT32 Size)
{
    return IO_UtilityCrc32_flag;
}

void ambhw_dump_buffer(const char *msg, const UINT8 *data, UINT32 length)
{

}

/****************************************************/
/*                Main Test                         */
/****************************************************/
void TEST_AmbaEnet_SetConfig(void)
{
    AMBA_ENET_CONFIG_s pEnetConfig[1];

    printf("TEST_AmbaEnet_SetConfig 1\n");
    /* arrange */
    /* act */
    AmbaEnet_SetConfig(0U, NULL);

    printf("TEST_AmbaEnet_SetConfig 2\n");
    /* arrange */
    /* act */
    AmbaEnet_SetConfig(0U, pEnetConfig);

    printf("TEST_AmbaEnet_SetConfig done\n");
}

void TEST_AmbaEnet_GetConfig(void)
{
    UINT32 Idx;
    AMBA_ENET_CONFIG_s ppEnetConfig[1][1];

    printf("TEST_AmbaEnet_GetConfig 1\n");
    /* arrange */
    Idx = ENET_INSTANCES + 1U;
    /* act */
    AmbaEnet_GetConfig(Idx, (AMBA_ENET_CONFIG_s **) ppEnetConfig);

    printf("TEST_AmbaEnet_GetConfig 2\n");
    /* arrange */
    Idx = 0U;
    /* act */
    AmbaEnet_GetConfig(Idx, NULL);

    printf("TEST_AmbaEnet_GetConfig 3\n");
    /* arrange */
    Idx = 0U;
    deinit_pAmbaEnetConfig();
    /* act */
    AmbaEnet_GetConfig(Idx, (AMBA_ENET_CONFIG_s **) ppEnetConfig);
    init_pAmbaEnetConfig();

    printf("TEST_AmbaEnet_GetConfig done\n");
}

void TEST_AmbaEnet_Init(void)
{
    AMBA_ENET_DES_s pDes[] = {[0] = {.FrameSize = 1536U + 1U, .RDESCnt = 1, .TDESCnt = 1, .pRDES = (AMBA_ENET_RDES_s*)&g_pRDES, .pTDES = (AMBA_ENET_TDES_s*)&g_pTDES, .pRxDma = &g_pRxDma, .pTxDma = &g_pTxDma}};
    AMBA_ENET_CONFIG_s pEnetConfig[] = {
        [0] = {.Idx = ENET_INSTANCES},
        [1] = {.Idx = 0U, .pDes = NULL},
        [2] = {.Idx = 0U, .pDes = pDes, .Mac[0] = 0x01U},
        [3] = {.Idx = 0U, .pDes = pDes, .Mac[0] = 0x0U, .Mac[1] = 0x0U, .Mac[2] = 0x0U, .Mac[3] = 0x0U, .Mac[4] = 0x0U, .Mac[5] = 0x0U, .pPreInitCb = UserPreInitCb},
        [4] = {.Idx = 1U, .pDes = pDes, .Mac[0] = 0x0U, .Mac[1] = 0x0U, .Mac[2] = 0x0U, .Mac[3] = 0x0U, .Mac[4] = 0x0U, .Mac[5] = 0x0U, .pPreInitCb = UserPreInitCb}
    };

    printf("TEST_AmbaEnet_Init 1\n");
    /* arrange */
    /* act */
    AmbaEnet_Init(NULL);

    printf("TEST_AmbaEnet_Init 2\n");
    /* arrange */
    /* act */
    AmbaEnet_Init(&pEnetConfig[0]);

    printf("TEST_AmbaEnet_Init 3\n");
    /* arrange */
    /* act */
    AmbaEnet_Init(&pEnetConfig[1]);

    printf("TEST_AmbaEnet_Init 4\n");
    /* arrange */
    /* act */
    AmbaEnet_Init(&pEnetConfig[2]);

    printf("TEST_AmbaEnet_Init 5\n");
    /* arrange */
    AmbaKAL_SemaphoreCreate_array_cnt = 2U;
    AmbaKAL_SemaphoreCreate_array[1] = 1U;
    AmbaKAL_SemaphoreCreate_array[0] = 0U;
    /* act */
    AmbaEnet_Init(&pEnetConfig[3]);

    printf("TEST_AmbaEnet_Init 6\n");
    /* arrange */
    AmbaKAL_SemaphoreCreate_array_cnt = 3U;
    AmbaKAL_SemaphoreCreate_array[2] = 0U;
    AmbaKAL_SemaphoreCreate_array[1] = 1U;
    AmbaKAL_SemaphoreCreate_array[0] = 0U;
    /* act */
    AmbaEnet_Init(&pEnetConfig[3]);

    printf("TEST_AmbaEnet_Init 7\n");
    /* arrange */
    AmbaKAL_SemaphoreCreate_array_cnt = 4U;
    AmbaKAL_SemaphoreCreate_array[3] = 0U;
    AmbaKAL_SemaphoreCreate_array[2] = 0U;
    AmbaKAL_SemaphoreCreate_array[1] = 1U;
    AmbaKAL_SemaphoreCreate_array[0] = 0U;
    /* act */
    AmbaEnet_Init(&pEnetConfig[3]);

    printf("TEST_AmbaEnet_Init 8\n");
    /* arrange */
    AmbaKAL_SemaphoreCreate_array_cnt = 5U;
    AmbaKAL_SemaphoreCreate_array[4] = 0U;
    AmbaKAL_SemaphoreCreate_array[3] = 0U;
    AmbaKAL_SemaphoreCreate_array[2] = 0U;
    AmbaKAL_SemaphoreCreate_array[1] = 1U;
    AmbaKAL_SemaphoreCreate_array[0] = 0U;
    Set_RetVal_MutexCreate(1U);
    /* act */
    AmbaEnet_Init(&pEnetConfig[3]);

    printf("TEST_AmbaEnet_Init 9\n");
    /* arrange */
    AmbaKAL_SemaphoreCreate_array_cnt = 1U;
    AmbaKAL_SemaphoreCreate_array[0] = 0U;
    /* act */
    AmbaEnet_Init(&pEnetConfig[3]);

    printf("TEST_AmbaEnet_Init 10\n");
    /* arrange */
    AmbaKAL_SemaphoreCreate_array_cnt = 1U;
    AmbaKAL_SemaphoreCreate_array[0] = 0U;
    Set_RetVal_EnetDmaReset(1U);
    /* act */
    AmbaEnet_Init(&pEnetConfig[3]);

    printf("TEST_AmbaEnet_Init 11\n");
    /* arrange */
    AmbaKAL_SemaphoreCreate_array_cnt = 1U;
    AmbaKAL_SemaphoreCreate_array[0] = 0U;
    /* act */
    AmbaEnet_Init(&pEnetConfig[4]);

    printf("TEST_AmbaEnet_Init done\n");
}

void TEST_AmbaEnet_Tx(void)
{
    UINT32 Idx;
    UINT16 txLen;

    printf("TEST_AmbaEnet_Tx 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    EnetTxCur[Idx] = 1U;
    txLen = 1U;
    /* act */
    AmbaEnet_Tx(Idx, txLen);

    printf("TEST_AmbaEnet_Tx 2\n");
    /* arrange */
    Idx = 0U;
    EnetTxCur[Idx] = 1U;
    txLen = pAmbaEnetConfig[Idx]->pDes->FrameSize + 1U;
    /* act */
    AmbaEnet_Tx(Idx, txLen);

    printf("TEST_AmbaEnet_Tx 3\n");
    /* arrange */
    Idx = 0U;
    EnetTxCur[Idx] = 1U;
    txLen = 1U;
    EnetInited[Idx] = 0U;
    /* act */
    AmbaEnet_Tx(Idx, txLen);

    printf("TEST_AmbaEnet_Tx 4\n");
    /* arrange */
    Idx = 0U;
    EnetTxCur[Idx] = 1U;
    txLen = 1U;
    EnetInited[Idx] = 1U;
    init_pAmbaEnetConfig();
    /* act */
    AmbaEnet_Tx(Idx, txLen);

    printf("TEST_AmbaEnet_Tx done\n");
}

void TEST_AmbaEnet_TxWait(void)
{
    UINT32 pSec[1] = {1U};
    UINT32 pNs[1] = {1U};
    UINT32 Idx;

    printf("TEST_AmbaEnet_Tx 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_TxWait(Idx, 0U, NULL, NULL);

    printf("TEST_AmbaEnet_TxWait 2\n");
    /* arrange */
    Idx = 0U;
    /* act */
    AmbaEnet_TxWait(Idx, 0U, NULL, NULL);

    printf("TEST_AmbaEnet_TxWait 3\n");
    /* arrange */
    Idx = 0U;
    /* act */
    AmbaEnet_TxWait(Idx, 0U, pSec, NULL);

    printf("TEST_AmbaEnet_TxWait 4\n");
    /* arrange */
    EnetInited[Idx] = 1U;
    Set_RetVal_EnetTDESGetTTSS(1U);
    /* act */
    AmbaEnet_TxWait(Idx, 0U, pSec, pNs);

    printf("TEST_AmbaEnet_TxWait 5\n");
    /* arrange */
    EnetInited[Idx] = 1U;
    /* act */
    AmbaEnet_TxWait(Idx, 0U, pSec, pNs);

    printf("TEST_AmbaEnet_TxWait done\n");
}

void TEST_AmbaEnet_SetupTxDesc(void)
{
    UINT32 Idx;
    void *Addr;
    UINT32 Len;
    UINT8 FS, LS, CacheClean = 1;

    printf("TEST_AmbaEnet_SetupTxDesc 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    Addr = NULL;
    Len = 0U;
    FS = 1U;
    LS = 1U;
    /* act */
    AmbaEnet_SetupTxDesc(Idx, Addr, Len, FS, LS, CacheClean);

    printf("TEST_AmbaEnet_SetupTxDesc 2\n");
    /* arrange */
    Idx = 0U;
    Addr = NULL;
    Len = 0U;
    FS = 1U;
    LS = 1U;
    /* act */
    AmbaEnet_SetupTxDesc(Idx, Addr, Len, FS, LS, CacheClean);

    printf("TEST_AmbaEnet_SetupTxDesc 3\n");
    /* arrange */
    Idx = 0U;
    Addr = g_u32_buf;
    Len = 0U;
    FS = 1U;
    LS = 1U;
    /* act */
    AmbaEnet_SetupTxDesc(Idx, Addr, Len, FS, LS, CacheClean);

    printf("TEST_AmbaEnet_SetupTxDesc 4\n");
    /* arrange */
    Idx = 0U;
    Addr = g_u32_buf;
    Len = 1U;
    FS = 1U;
    LS = 1U;
    EnetInited[Idx] = 0U;
    /* act */
    AmbaEnet_SetupTxDesc(Idx, Addr, Len, FS, LS, CacheClean);

    printf("TEST_AmbaEnet_SetupTxDesc 5\n");
    /* arrange */
    Idx = 0U;
    Addr = g_u32_buf;
    Len = 1U;
    FS = 2U;
    LS = 1U;
    EnetInited[Idx] = 1U;
    /* act */
    AmbaEnet_SetupTxDesc(Idx, Addr, Len, FS, LS, CacheClean);

    printf("TEST_AmbaEnet_SetupTxDesc 6\n");
    /* arrange */
    Idx = 0U;
    Addr = g_u32_buf;
    Len = 1U;
    FS = 1U;
    LS = 2U;
    EnetInited[Idx] = 1U;
    /* act */
    AmbaEnet_SetupTxDesc(Idx, Addr, Len, FS, LS, CacheClean);

    printf("TEST_AmbaEnet_SetupTxDesc 7\n");
    /* arrange */
    Idx = 0U;
    Addr = g_u32_buf;
    Len = 1U;
    FS = 1U;
    LS = 1U;
    EnetInited[Idx] = 1U;
    AmbaCSL_EnetTDESGetDmaOwn_count = 101;
    Set_RetVal_EnetTDESGetDmaOwn(1U);
    AmbaKAL_TaskSleep_FuncSel = 1;
    Set_RetVal_SemaphoreTake(1U);
    init_locked(1);
    /* act */
    AmbaEnet_SetupTxDesc(Idx, Addr, Len, FS, LS, CacheClean);

    printf("TEST_AmbaEnet_SetupTxDesc 8\n");
    /* arrange */
    Idx = 0U;
    Addr = g_u32_buf;
    Len = 1U;
    FS = 1U;
    LS = 1U;
    EnetInited[Idx] = 1U;
    AmbaCSL_EnetTDESGetDmaOwn_count = 1000;
    Set_RetVal_EnetTDESGetDmaOwn(1U);
    AmbaKAL_TaskSleep_FuncSel = 1;
    Set_RetVal_SemaphoreTake(1U);
    init_locked(1);
    /* act */
    AmbaEnet_SetupTxDesc(Idx, Addr, Len, FS, LS, CacheClean);
    init_locked(1);

    printf("TEST_AmbaEnet_SetupTxDesc done\n");
}

void TEST_AmbaEnet_GetTxBuf(void)
{
    UINT32 Idx;
    void *Addr;

    printf("TEST_AmbaEnet_GetTxBuf 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_GetTxBuf(Idx, NULL);

    printf("TEST_AmbaEnet_GetTxBuf 2\n");
    /* arrange */
    Idx = 0;
    /* act */
    AmbaEnet_GetTxBuf(Idx, NULL);

    printf("TEST_AmbaEnet_GetTxBuf 3\n");
    /* arrange */
    Idx = 0;
    Addr = g_u32_buf;
    EnetInited[Idx] = 0;
    /* act */
    AmbaEnet_GetTxBuf(Idx, &Addr);

    printf("TEST_AmbaEnet_GetTxBuf 4\n");
    /* arrange */
    Idx = 0;
    Addr = g_u32_buf;
    EnetInited[Idx] = 1;
    AmbaCSL_EnetTDESGetDmaOwn_count = 0;
    Set_RetVal_EnetTDESGetDmaOwn(1U);
    AmbaKAL_TaskSleep_FuncSel = 1;
    Set_RetVal_SemaphoreTake(1U);
    /* act */
    AmbaEnet_GetTxBuf(Idx, &Addr);
    init_locked(1);

    printf("TEST_AmbaEnet_GetTxBuf done\n");
}

void TEST_AmbaEnet_GetRxBuf(void)
{
    UINT32 Idx;
    void *Addr;

    printf("TEST_AmbaEnet_GetRxBuf 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_GetRxBuf(Idx, NULL);

    printf("TEST_AmbaEnet_GetRxBuf 2\n");
    /* arrange */
    Idx = 0;
    /* act */
    AmbaEnet_GetRxBuf(Idx, NULL);

    printf("TEST_AmbaEnet_GetRxBuf 3\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 0;
    Addr = g_u32_buf;
    /* act */
    AmbaEnet_GetRxBuf(Idx, &Addr);

    printf("TEST_AmbaEnet_GetRxBuf 4\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    Addr = g_u32_buf;
    /* act */
    AmbaEnet_GetRxBuf(Idx, &Addr);

    printf("TEST_AmbaEnet_GetRxBuf done\n");
}

void TEST_AmbaEnet_SetPPS(void)
{
    UINT32 Idx;
    const UINT32 PPSINT = 0;
    const UINT32 PPSWIDTH = 0;

    printf("TEST_AmbaEnet_SetPPS 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_SetPPS(Idx, PPSINT, PPSWIDTH);

    printf("TEST_AmbaEnet_SetPPS 2\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 0;
    /* act */
    AmbaEnet_SetPPS(Idx, PPSINT, PPSWIDTH);

    printf("TEST_AmbaEnet_SetPPS 3\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    /* act */
    AmbaEnet_SetPPS(Idx, PPSINT, PPSWIDTH);

    printf("TEST_AmbaEnet_SetPPS done\n");
}

void TEST_AmbaEnet_SetTargetTsPPS(void)
{
    UINT32 Idx;
    const UINT32 Sec;
    UINT32 Ns;
    UINT8 PPSCMD;

    printf("TEST_AmbaEnet_SetTargetTsPPS 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_SetTargetTsPPS(Idx, Sec, Ns, PPSCMD);

    printf("TEST_AmbaEnet_SetTargetTsPPS 2\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 0;
    /* act */
    AmbaEnet_SetTargetTsPPS(Idx, Sec, Ns, PPSCMD);

    printf("TEST_AmbaEnet_SetTargetTsPPS 3\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    PPSCMD = 7;
    /* act */
    AmbaEnet_SetTargetTsPPS(Idx, Sec, Ns, PPSCMD);

    printf("TEST_AmbaEnet_SetTargetTsPPS 4\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    PPSCMD = 0;
    /* act */
    AmbaEnet_SetTargetTsPPS(Idx, Sec, Ns, PPSCMD);

    printf("TEST_AmbaEnet_SetTargetTsPPS 5\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    PPSCMD = 0;
    Ns = NS_PER_SECOND + 1;
    /* act */
    AmbaEnet_SetTargetTsPPS(Idx, Sec, Ns, PPSCMD);

    printf("TEST_AmbaEnet_SetTargetTsPPS 6\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    PPSCMD = 0;
    Ns = 0;
    /* act */
    AmbaEnet_SetTargetTsPPS(Idx, Sec, Ns, PPSCMD);

    printf("TEST_AmbaEnet_SetTargetTsPPS done\n");
}

void TEST_AmbaEnet_SetTargetTsIrq(void)
{
    UINT32 Idx;
    const UINT32 Sec = 0;
    UINT32 Ns;

    printf("TEST_AmbaEnet_SetTargetTsIrq 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_SetTargetTsIrq(Idx, Sec, Ns);

    printf("TEST_AmbaEnet_SetTargetTsIrq 2\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 0;
    /* act */
    AmbaEnet_SetTargetTsIrq(Idx, Sec, Ns);

    printf("TEST_AmbaEnet_SetTargetTsIrq 3\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    Ns = NS_PER_SECOND + 1;
    /* act */
    AmbaEnet_SetTargetTsIrq(Idx, Sec, Ns);

    printf("TEST_AmbaEnet_SetTargetTsIrq 4\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    Ns = 0;
    /* act */
    AmbaEnet_SetTargetTsIrq(Idx, Sec, Ns);

    printf("TEST_AmbaEnet_SetTargetTsIrq done\n");
}

void TEST_AmbaEnet_GetRxTs(void)
{
    UINT32 Idx;
    UINT32 *pSec = g_u32_buf;
    UINT32 *pNs = g_u32_buf;

    printf("TEST_AmbaEnet_GetRxTs 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_GetRxTs(Idx, pSec, pNs);

    printf("TEST_AmbaEnet_GetRxTs 2\n");
    /* arrange */
    Idx = 0;
    /* act */
    AmbaEnet_GetRxTs(Idx, pSec, pNs);

    printf("TEST_AmbaEnet_GetRxTs 3\n");
    /* arrange */
    Idx = 0;
    /* act */
    AmbaEnet_GetRxTs(Idx, NULL, pNs);

    printf("TEST_AmbaEnet_GetRxTs 4\n");
    /* arrange */
    Idx = 0;
    /* act */
    AmbaEnet_GetRxTs(Idx, pSec, NULL);

    printf("TEST_AmbaEnet_GetRxTs 5\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 0;
    /* act */
    AmbaEnet_GetRxTs(Idx, pSec, pNs);

    printf("TEST_AmbaEnet_GetRxTs 6\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    /* act */
    AmbaEnet_GetRxTs(Idx, pSec, pNs);

    printf("TEST_AmbaEnet_GetRxTs done\n");
}

void TEST_AmbaEnet_PhcSetTs(void)
{
    UINT32 Idx;
    UINT32 Sec = 0;
    UINT32 Ns;

    printf("TEST_AmbaEnet_PhcSetTs 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_PhcSetTs(Idx, Sec, Ns);

    printf("TEST_AmbaEnet_PhcSetTs 2\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 0;
    /* act */
    AmbaEnet_PhcSetTs(Idx, Sec, Ns);

    printf("TEST_AmbaEnet_PhcSetTs 3\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    Ns = NS_PER_SECOND + 1;
    /* act */
    AmbaEnet_PhcSetTs(Idx, Sec, Ns);

    printf("TEST_AmbaEnet_PhcSetTs 4\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    Ns = 0;
    /* act */
    AmbaEnet_PhcSetTs(Idx, Sec, Ns);

    printf("TEST_AmbaEnet_PhcSetTs done\n");
}

void TEST_AmbaEnet_PhcUpdaTs(void)
{
    UINT32 Idx;
    UINT8 ADDSUB;
    UINT32 Sec;
    UINT32 Ns;

    printf("TEST_AmbaEnet_PhcUpdaTs 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_PhcUpdaTs(Idx, ADDSUB, Sec, Ns);

    printf("TEST_AmbaEnet_PhcUpdaTs 2\n");
    /* arrange */
    Idx = 0;
    ADDSUB = 2;
    /* act */
    AmbaEnet_PhcUpdaTs(Idx, ADDSUB, Sec, Ns);

    printf("TEST_AmbaEnet_PhcUpdaTs 3\n");
    /* arrange */
    Idx = 0;
    ADDSUB = 0;
    EnetInited[Idx] = 0;
    /* act */
    AmbaEnet_PhcUpdaTs(Idx, ADDSUB, Sec, Ns);

    printf("TEST_AmbaEnet_PhcUpdaTs 4\n");
    /* arrange */
    Idx = 0;
    ADDSUB = 0;
    EnetInited[Idx] = 1;
    Ns = NS_PER_SECOND + 1;
    /* act */
    AmbaEnet_PhcUpdaTs(Idx, ADDSUB, Sec, Ns);

    printf("TEST_AmbaEnet_PhcUpdaTs 5\n");
    /* arrange */
    Idx = 0;
    ADDSUB = 0;
    EnetInited[Idx] = 1;
    Ns = 0;
    /* act */
    AmbaEnet_PhcUpdaTs(Idx, ADDSUB, Sec, Ns);

    printf("TEST_AmbaEnet_PhcUpdaTs done\n");
}

void TEST_AmbaEnet_PhcAdjFreq(void)
{
    UINT32 Idx;
    INT32 ppb;

    printf("TEST_AmbaEnet_PhcAdjFreq 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_PhcAdjFreq(Idx, ppb);

    printf("TEST_AmbaEnet_PhcAdjFreq 2\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 0;
    /* act */
    AmbaEnet_PhcAdjFreq(Idx, ppb);

    printf("TEST_AmbaEnet_PhcAdjFreq 3\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    ppb = -1;
    /* act */
    AmbaEnet_PhcAdjFreq(Idx, ppb);

    printf("TEST_AmbaEnet_PhcAdjFreq 4\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    ppb = 1;
    /* act */
    AmbaEnet_PhcAdjFreq(Idx, ppb);

    printf("TEST_AmbaEnet_PhcAdjFreq done\n");
}

void TEST_AmbaEnet_McastRxHash(void)
{
    UINT32 Idx;
    UINT8 *Addr = g_u8_buf;

    printf("TEST_AmbaEnet_McastRxHash 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_McastRxHash(Idx, Addr);

    printf("TEST_AmbaEnet_McastRxHash 2\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 0;
    /* act */
    AmbaEnet_McastRxHash(Idx, Addr);

    printf("TEST_AmbaEnet_McastRxHash 3\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    /* act */
    AmbaEnet_McastRxHash(Idx, Addr);

    printf("TEST_AmbaEnet_McastRxHash 4\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    IO_UtilityCrc32_flag = 1;
    /* act */
    AmbaEnet_McastRxHash(Idx, Addr);

    printf("TEST_AmbaEnet_McastRxHash done\n");
}

void TEST_AmbaEnet_IfUp(void)
{
    UINT32 Idx;
    UINT32 *Speed = g_u32_buf;

    printf("TEST_AmbaEnet_IfUp 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_IfUp(Idx, Speed);

    printf("TEST_AmbaEnet_IfUp 2\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 0;
    /* act */
    AmbaEnet_IfUp(Idx, Speed);

    printf("TEST_AmbaEnet_IfUp 3\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    Set_RetVal_AmbaRTSL_EnetGetPhyAddr(32U);
    fpCB_EnetGetPhyAddr = CB_EnetGetPhyAddr;
    /* act */
    AmbaEnet_IfUp(Idx, Speed);
    Set_RetVal_AmbaRTSL_EnetGetPhyAddr(0U);
    fpCB_EnetGetPhyAddr = NULL;

    printf("TEST_AmbaEnet_IfUp done\n");
}

void TEST_AmbaEnet_IfDown(void)
{
    UINT32 Idx;
    printf("TEST_AmbaEnet_IfDown 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_IfDown(Idx);

    printf("TEST_AmbaEnet_IfDown 2\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 0;
    /* act */
    AmbaEnet_IfDown(Idx);

    printf("TEST_AmbaEnet_IfDown 3\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    /* act */
    AmbaEnet_IfDown(Idx);

    printf("TEST_AmbaEnet_IfDown done\n");
}

void TEST_AmbaEnet_PhyRead(void)
{
    UINT32 Idx;
    UINT32 PhyAddr;
    UINT32 Offset;
    UINT32 *Value = g_u32_buf;

    printf("TEST_AmbaEnet_PhyRead 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_PhyRead(Idx, PhyAddr, Offset, Value);

    printf("TEST_AmbaEnet_PhyRead 2\n");
    /* arrange */
    Idx = 0;
    /* act */
    AmbaEnet_PhyRead(Idx, PhyAddr, Offset, NULL);

    printf("TEST_AmbaEnet_PhyRead 3\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 0;
    /* act */
    AmbaEnet_PhyRead(Idx, PhyAddr, Offset, Value);

    printf("TEST_AmbaEnet_PhyRead 4\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    PhyAddr = 32U;
    /* act */
    AmbaEnet_PhyRead(Idx, PhyAddr, Offset, Value);

    printf("TEST_AmbaEnet_PhyRead 5\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    PhyAddr = 0U;
    Offset = 32U;
    /* act */
    AmbaEnet_PhyRead(Idx, PhyAddr, Offset, Value);

    printf("TEST_AmbaEnet_PhyRead 6\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    PhyAddr = 0U;
    Offset = 0U;
    /* act */
    AmbaEnet_PhyRead(Idx, PhyAddr, Offset, Value);

    printf("TEST_AmbaEnet_PhyRead done\n");
}

void TEST_AmbaEnet_PhyWrite(void)
{
    UINT32 Idx;
    UINT32 PhyAddr;
    UINT32 Offset;
    UINT32 Value;

    printf("TEST_AmbaEnet_PhyWrite 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_PhyWrite(Idx, PhyAddr, Offset, Value);

    printf("TEST_AmbaEnet_PhyWrite 2\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 0;
    /* act */
    AmbaEnet_PhyWrite(Idx, PhyAddr, Offset, Value);

    printf("TEST_AmbaEnet_PhyWrite 3\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    PhyAddr = 32U;
    /* act */
    AmbaEnet_PhyWrite(Idx, PhyAddr, Offset, Value);

    printf("TEST_AmbaEnet_PhyWrite 4\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    PhyAddr = 0U;
    Offset = 32U;
    /* act */
    AmbaEnet_PhyWrite(Idx, PhyAddr, Offset, Value);

    printf("TEST_AmbaEnet_PhyWrite 5\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    PhyAddr = 0U;
    Offset = 0U;
    Value = 0x10000U;
    /* act */
    AmbaEnet_PhyWrite(Idx, PhyAddr, Offset, Value);

    printf("TEST_AmbaEnet_PhyWrite 6\n");
    /* arrange */
    Idx = 0;
    EnetInited[Idx] = 1;
    PhyAddr = 0U;
    Offset = 0U;
    Value = 0U;
    /* act */
    AmbaEnet_PhyWrite(Idx, PhyAddr, Offset, Value);

    printf("TEST_AmbaEnet_PhyWrite done\n");
}

void TEST_AmbaEnet_PhcGetPpb(void)
{
    UINT32 Idx;

    printf("TEST_AmbaEnet_PhcGetPpb 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_PhcGetPpb(Idx);

    printf("TEST_AmbaEnet_PhcGetPpb 2\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    Set_RetVal_EnetMacGetTSAR(DEFAULT_ADDEND);
    /* act */
    AmbaEnet_PhcGetPpb(Idx);

    printf("TEST_AmbaEnet_PhcGetPpb done\n");
}

void TEST_AmbaEnet_GetPhc(void)
{
    UINT32 Idx;
    UINT32 *pSec = g_u32_buf;
    UINT32 *pNs = g_u32_buf;

    printf("TEST_AmbaEnet_GetPhc 1\n");
    /* arrange */
    Idx = ENET_INSTANCES;
    /* act */
    AmbaEnet_GetPhc(Idx, pSec, pNs);

    printf("TEST_AmbaEnet_GetPhc 2\n");
    /* arrange */
    Idx = 0;
    /* act */
    AmbaEnet_GetPhc(Idx, NULL, pNs);

    printf("TEST_AmbaEnet_GetPhc 3\n");
    /* arrange */
    Idx = 0;
    /* act */
    AmbaEnet_GetPhc(Idx, pSec, NULL);

    printf("TEST_AmbaEnet_GetPhc 4\n");
    /* arrange */
    Idx = 0;
    pAmbaCSL_EnetPhc[Idx] = &g_enet_ts;
    /* act */
    AmbaEnet_GetPhc(Idx, pSec, pNs);

    printf("TEST_AmbaEnet_GetPhc done\n");
}

void TEST_AmbaEth_TxConfirmation(void)
{
    UINT32 Idx = 0;
    UINT32 *TxResult = g_u32_buf;

    printf("TEST_AmbaEth_TxConfirmation 1\n");
    /* arrange */
    EnetTxCur[Idx] = 0U;
    /* act */
    AmbaEth_TxConfirmation(Idx, TxResult);

    printf("TEST_AmbaEth_TxConfirmation 2\n");
    /* arrange */
    EnetTxCur[Idx] = 3U;
    init_locked(1);
    AmbaCSL_EnetTDESGetDmaOwn_count = 0;
    Set_RetVal_EnetTDESGetDmaOwn(0U);
    Set_RetVal_EnetTDESGetDmaES(1U);
    /* act */
    AmbaEth_TxConfirmation(Idx, TxResult);

    printf("TEST_AmbaEth_TxConfirmation 3\n");
    /* arrange */
    EnetTxCur[Idx] = 0U;
    // EnetTxDirty[Idx] = 2U; // set by TEST_AmbaEth_TxConfirmation 2
    /* act */
    AmbaEth_TxConfirmation(Idx, TxResult);

    printf("TEST_AmbaEth_TxConfirmation done\n");
}

void TEST_EnetRxTaskEntry(void)
{
    UINT32 Idx = 0;

    printf("TEST_EnetRxTaskEntry 1\n");
    /* arrange */
    EnetInited[Idx] = 0U;
    AmbaKAL_TaskSleep_FuncSel = 2;
    setAlarm();
    /* act */
    EnetRxTaskEntry(Idx);
    EnetInited[0] = 0U;
    GcovrAlarmFlag = 0U;

    printf("TEST_EnetRxTaskEntry 2\n");
    /* arrange */
    EnetInited[Idx] = 0U;
    AmbaKAL_TaskSleep_FuncSel = 2;
    Set_RetVal_EnetRDESGetDmaOwn(1U);
    setAlarm();
    /* act */
    EnetRxTaskEntry(Idx);
    EnetInited[0] = 0U;
    GcovrAlarmFlag = 0U;

    printf("TEST_EnetRxTaskEntry done\n");
}

void TEST_EnetTxTaskEntry(void)
{
    UINT32 Idx = 0;

    printf("TEST_EnetTxTaskEntry 1\n");
    /* arrange */
    EnetInited[Idx] = 0U;
    AmbaKAL_TaskSleep_FuncSel = 2;
    setAlarm();
    /* act */
    EnetTxTaskEntry(Idx);
    GcovrAlarmFlag = 0U;

    printf("TEST_EnetTxTaskEntry done\n");
}

void TEST_Eth_RxIrqHdlr(void)
{
    printf("TEST_Eth_RxIrqHdlr 1\n");
    /* arrange */
    /* act */
    Eth_RxIrqHdlr(pAmbaCSL_EnetReg[0]);

    printf("TEST_Eth_RxIrqHdlr done\n");
}

void TEST_Enet_PlugCb(void)
{
    printf("TEST_Enet_PlugCb 1\n");
    /* arrange */
    /* act */
    Enet_PlugCb(pAmbaCSL_EnetReg[0]);

    printf("TEST_Enet_PlugCb done\n");
}

void TEST_EnetSetCIC(void)
{
    UINT32 Idx, CIC;

    printf("TEST_EnetSetCIC 1\n");
    /* arrange */
    Idx = 0;
    CIC = 0;
    /* act */
    EnetSetCIC(Idx, CIC);

    printf("TEST_EnetSetCIC done\n");
}

void TEST_EnetDumpCb(void)
{
    UINT32 Idx;

    printf("TEST_EnetDumpCb 1\n");
    /* arrange */
    Idx = 0;
    Set_NoCopy_AmbaWrap_memcpy(AMBA_NOCOPY);
    /* act */
    EnetDumpCb(Idx);
    Set_NoCopy_AmbaWrap_memcpy(AMBA_COPY);

    printf("TEST_EnetDumpCb done\n");
}

void TEST_EnetSetDC(void)
{
    UINT32 Idx;
    UINT8 DC;

    printf("TEST_EnetSetDC 1\n");
    /* arrange */
    Idx = 0;
    DC = 0;
    /* act */
    EnetSetDC(Idx, DC);

    printf("TEST_EnetSetDC done\n");
}

int main(void)
{
    init();
    init_pAmbaEnetConfig();

    TEST_AmbaEnet_SetConfig();
    TEST_AmbaEnet_GetConfig();
    TEST_AmbaEnet_Init();
    TEST_AmbaEnet_Tx();
    TEST_AmbaEnet_TxWait();
    TEST_AmbaEnet_SetupTxDesc();
    TEST_AmbaEnet_GetTxBuf();
    TEST_AmbaEnet_GetRxBuf();
    TEST_AmbaEnet_SetPPS();
    TEST_AmbaEnet_SetTargetTsPPS();
    TEST_AmbaEnet_SetTargetTsIrq();
    TEST_AmbaEnet_GetRxTs();
    TEST_AmbaEnet_PhcSetTs();
    TEST_AmbaEnet_PhcUpdaTs();
    TEST_AmbaEnet_PhcAdjFreq();
    TEST_AmbaEnet_McastRxHash();
    TEST_AmbaEnet_IfUp();
    TEST_AmbaEnet_IfDown();
    TEST_AmbaEnet_PhyRead();
    TEST_AmbaEnet_PhyWrite();
    TEST_AmbaEnet_PhcGetPpb();
    TEST_AmbaEnet_GetPhc();
    TEST_AmbaEth_TxConfirmation();

    TEST_EnetRxTaskEntry();
    TEST_EnetTxTaskEntry();
    TEST_Eth_RxIrqHdlr();
    TEST_Enet_PlugCb();
    TEST_EnetSetCIC();
    TEST_EnetDumpCb();
    TEST_EnetSetDC();

    return 0;
}
