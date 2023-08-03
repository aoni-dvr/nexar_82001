#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaTypes.h"

#include "src/AmbaRTSL_ENET.c"

#define AMBA_COPY     (0U)
#define AMBA_NOCOPY   (1U)

UINT32 EnetIrq[ENET_INSTANCES] = { [0]=1U };

UINT32 AmbaRTSL_EnetMiiRead_array[6] = {0};
UINT32 AmbaRTSL_EnetMiiRead_array_cnt = 0;
extern void Set_RetVal_EnetDmaGetRI(UINT32 RetVal);
extern void Set_RetVal_EnetDmaGetRU(UINT32 RetVal);
extern void Set_RetVal_EnetDmaGetTI(UINT32 RetVal);
extern void Set_RetVal_EnetDmaGetTU(UINT32 RetVal);
extern void Set_RetVal_EnetDmaGetGLI(UINT32 RetVal);
extern void Set_RetVal_EnetDmaGetTTI(UINT32 RetVal);
extern UINT32 (*fpCB_AmbaRTSL_EnetMiiRead)(void);

/****************************************************/
/*                Callback Func                     */
/****************************************************/
UINT32 CB_AmbaRTSL_EnetMiiRead(void)
{
    if (AmbaRTSL_EnetMiiRead_array_cnt != 0U) {
        AmbaRTSL_EnetMiiRead_array_cnt--;
        return AmbaRTSL_EnetMiiRead_array[AmbaRTSL_EnetMiiRead_array_cnt];
    }
    return 0U;
}

/****************************************************/
/*                init Func                         */
/****************************************************/
static void init(void)
{
    fpCB_AmbaRTSL_EnetMiiRead = CB_AmbaRTSL_EnetMiiRead;
}

/****************************************************/
/*                dummy Func                        */
/****************************************************/
void DummyReg_Cb(AMBA_ENET_REG_s *const pEnetReg)
{

}

void DummyArg_Cb(const UINT32 Arg)
{

}

/****************************************************/
/*                Local Stub Func                   */
/****************************************************/
UINT32 AmbaRTSL_RtcGetSystemTime(void)
{
    return 0;
}

/****************************************************/
/*                Main Test                         */
/****************************************************/
void TEST_AmbaRTSL_EnetSetTsIsrFunc(void)
{
    printf("TEST_AmbaRTSL_EnetSetTsIsrFunc 1\n");
    /* arrange */
    /* act */
    AmbaRTSL_EnetSetTsIsrFunc(NULL, 0);

    printf("TEST_AmbaRTSL_EnetSetTsIsrFunc done\n");
}

void TEST_AmbaRTSL_EnetSetTxIsrFunc(void)
{
    printf("TEST_AmbaRTSL_EnetSetTxIsrFunc 1\n");
    /* arrange */
    /* act */
    AmbaRTSL_EnetSetTxIsrFunc(NULL);

    printf("TEST_AmbaRTSL_EnetSetTxIsrFunc done\n");
}

void TEST_AmbaRTSL_EnetSetRxIsrFunc(void)
{
    printf("TEST_AmbaRTSL_EnetSetRxIsrFunc 1\n");
    /* arrange */
    /* act */
    AmbaRTSL_EnetSetRxIsrFunc(NULL);

    printf("TEST_AmbaRTSL_EnetSetRxIsrFunc done\n");
}

void TEST_AmbaRTSL_EnetSetLcFunc(void)
{
    printf("TEST_AmbaRTSL_EnetSetLcFunc 1\n");
    /* arrange */
    /* act */
    AmbaRTSL_EnetSetLcFunc(NULL);

    printf("TEST_AmbaRTSL_EnetSetLcFunc done\n");
}

void TEST_AmbaRTSL_EnetMiiRead(void)
{
    printf("TEST_AmbaRTSL_EnetMiiRead 1\n");
    /* arrange */
    /* act */
    AmbaRTSL_EnetMiiRead(NULL, 0, 0);

    printf("TEST_AmbaRTSL_EnetMiiRead done\n");
}

void TEST_AmbaRTSL_EnetMiiWrite(void)
{
    printf("TEST_AmbaRTSL_EnetMiiWrite 1\n");
    /* arrange */
    /* act */
    AmbaRTSL_EnetMiiWrite(NULL, 0, 0, 0);

    printf("TEST_AmbaRTSL_EnetMiiWrite done\n");
}

void TEST_AmbaRTSL_EnetGetPhyId(void)
{
    printf("TEST_AmbaRTSL_EnetGetPhyId 1\n");
    /* arrange */
    /* act */
    AmbaRTSL_EnetGetPhyId(NULL, 0);

    printf("TEST_AmbaRTSL_EnetGetPhyId done\n");
}

void TEST_AmbaRTSL_EnetGetPhyAddr(void)
{
    const UINT8 TotalReadNum = 2;

    printf("TEST_AmbaRTSL_EnetGetPhyAddr 1\n");
    /* arrange */
    /* act */
    AmbaRTSL_EnetGetPhyAddr(NULL);

    printf("TEST_AmbaRTSL_EnetGetPhyAddr 2\n");
    /* arrange */
    AmbaRTSL_EnetMiiRead_array_cnt = TotalReadNum;
    AmbaRTSL_EnetMiiRead_array[1] =  0U;
    AmbaRTSL_EnetMiiRead_array[0] =  0xFFFFU;
    /* act */
    AmbaRTSL_EnetGetPhyAddr(NULL);

    printf("TEST_AmbaRTSL_EnetGetPhyAddr 3\n");
    /* arrange */
    AmbaRTSL_EnetMiiRead_array_cnt = TotalReadNum;
    AmbaRTSL_EnetMiiRead_array[1] =  0U;
    AmbaRTSL_EnetMiiRead_array[0] =  0xFFF0U;
    /* act */
    AmbaRTSL_EnetGetPhyAddr(NULL);

    printf("TEST_AmbaRTSL_EnetGetPhyAddr done\n");
}

void TEST_AmbaRTSL_GetLink(void)
{
    AMBA_ENET_REG_s EnetReg;
    UINT32 Speed, Duplex;
    UINT32 i;
    const UINT8 TotalReadNum = 6; //AmbaRTSL_GetLink, it calls AmbaRTSL_EnetMiiRead() total 6 times.

    printf("TEST_AmbaRTSL_GetLink 1\n");
    /* arrange */
    AmbaRTSL_EnetMiiRead_array_cnt = 0U;
    /* act */
    AmbaRTSL_GetLink(&EnetReg, 0U, &Speed, &Duplex);

    printf("TEST_AmbaRTSL_GetLink 2\n");
    /* arrange */
    AmbaRTSL_EnetMiiRead_array_cnt = TotalReadNum;
    for (i = 0; i < TotalReadNum; i++) {
        AmbaRTSL_EnetMiiRead_array[i] =  0xFFFFU & ~0x4U;
    }
    /* act */
    AmbaRTSL_GetLink(&EnetReg, 0U, &Speed, &Duplex);

    printf("TEST_AmbaRTSL_GetLink 3\n");
    /* arrange */
    AmbaRTSL_EnetMiiRead_array_cnt = TotalReadNum;
    for (i = 0; i < TotalReadNum; i++) {
        AmbaRTSL_EnetMiiRead_array[i] =  0xFFFFU & ~(0x0800U | 0x0400U);
    }
    /* act */
    AmbaRTSL_GetLink(&EnetReg, 0U, &Speed, &Duplex);

    printf("TEST_AmbaRTSL_GetLink 4\n");
    /* arrange */
    AmbaRTSL_EnetMiiRead_array_cnt = TotalReadNum;
    for (i = 0; i < TotalReadNum; i++) {
        AmbaRTSL_EnetMiiRead_array[i] =  0xFFFFU & ~(0x0800U | 0x0400U | 0x0100U | 0x0080U);
    }
    /* act */
    AmbaRTSL_GetLink(&EnetReg, 0U, &Speed, &Duplex);

    printf("TEST_AmbaRTSL_GetLink 5\n");
    /* arrange */
    AmbaRTSL_EnetMiiRead_array_cnt = TotalReadNum;
    for (i = 0; i < TotalReadNum; i++) {
        AmbaRTSL_EnetMiiRead_array[i] = 0xFFFFU;
    }
    /* act */
    AmbaRTSL_GetLink(&EnetReg, 0U, &Speed, &Duplex);

    printf("TEST_AmbaRTSL_GetLink 6\n");
    /* arrange */
    AmbaRTSL_EnetMiiRead_array_cnt = TotalReadNum;
    AmbaRTSL_EnetMiiRead_array[5] =  0xFFFFU & ~0x4U;
    AmbaRTSL_EnetMiiRead_array[4] =  0xFFFFU;
    AmbaRTSL_EnetMiiRead_array[3] =  0xFFFFU;
    AmbaRTSL_EnetMiiRead_array[2] =  0xFFFFU;
    AmbaRTSL_EnetMiiRead_array[1] =  0xFFFFU;
    AmbaRTSL_EnetMiiRead_array[0] =  0xFFFFU;
    /* act */
    AmbaRTSL_GetLink(&EnetReg, 0U, &Speed, &Duplex);

    /* restore */
    AmbaRTSL_EnetMiiRead_array[0] = 0U;
    AmbaRTSL_EnetMiiRead_array[1] = 0U;
    AmbaRTSL_EnetMiiRead_array_cnt = 0U;

    printf("TEST_AmbaRTSL_GetLink done\n");
}

void TEST_AmbaRTSL_EnetINTEnable(void)
{
    UINT32 idx = 0;

    printf("TEST_AmbaRTSL_EnetINTEnable 1\n");
    /* arrange */
    /* act */
    AmbaRTSL_EnetINTEnable(idx);

    printf("TEST_AmbaRTSL_EnetINTEnable done\n");
}

void TEST_AmbaRTSL_EnetINTDisable(void)
{
    UINT32 idx = 0;

    printf("TEST_AmbaRTSL_EnetINTDisable 1\n");
    /* arrange */
    /* act */
    AmbaRTSL_EnetINTDisable(idx);

    printf("TEST_AmbaRTSL_EnetINTDisable done\n");
}

void TEST_AmbaRTSL_EnetInit(void)
{
    // AMBA_ENET_REG_s *pEnetReg;

    printf("TEST_AmbaRTSL_EnetInit 1\n");
    /* arrange */
    /* act */
    AmbaRTSL_EnetInit(NULL);

    printf("TEST_AmbaRTSL_EnetInit done\n");
}

void TEST_AmbaRTSL_EnetGetRegIdx(void)
{
    AMBA_ENET_REG_s *pAmbaCSL_EnetReg_dummy[ENET_INSTANCES] = { [0]=(AMBA_ENET_REG_s*)0x12341234U };

    printf("TEST_AmbaRTSL_EnetGetRegIdx 1\n");
    /* arrange */
    /* act */
    AmbaRTSL_EnetGetRegIdx(pAmbaCSL_EnetReg[0]);

    printf("TEST_AmbaRTSL_EnetGetRegIdx 2\n");
    /* arrange */
    /* act */
    AmbaRTSL_EnetGetRegIdx(pAmbaCSL_EnetReg_dummy[0]);

    printf("TEST_AmbaRTSL_EnetGetRegIdx done\n");
}

void TEST_Eth_IrqHdlr(void)
{
    UINT32 IntID, UserArg = 0;

    pAmbaRTSL_EnetRxIsrCb = (AMBA_RTSL_ENET_RXISRCB_f) DummyReg_Cb;
    pAmbaRTSL_EnetLcCb    = (AMBA_RTSL_ENET_LINKCB_f)  DummyReg_Cb;
    pAmbaRTSL_EnetTxIsrCb = (AMBA_RTSL_ENET_TXISRCB_f) DummyReg_Cb;
    pAmbaRTSL_EnetTsIsrCb = DummyArg_Cb;

    printf("TEST_Eth_IrqHdlr 1\n");
    /* arrange */
    IntID = 0;
    Set_RetVal_EnetDmaGetRU(1U);
    Set_RetVal_EnetDmaGetTU(1U);
    /* act */
    Eth_IrqHdlr(IntID, UserArg);

    printf("TEST_Eth_IrqHdlr 2\n");
    /* arrange */
    IntID = EnetIrq[0];
    Set_RetVal_EnetDmaGetRI(1U);
    Set_RetVal_EnetDmaGetTI(1U);
    Set_RetVal_EnetDmaGetGLI(1U);
    Set_RetVal_EnetDmaGetTTI(1U);
    /* act */
    Eth_IrqHdlr(IntID, UserArg);

    printf("TEST_Eth_IrqHdlr done\n");
}

int main(void)
{
    init();
    TEST_AmbaRTSL_EnetSetTsIsrFunc();
    TEST_AmbaRTSL_EnetSetTxIsrFunc();
    TEST_AmbaRTSL_EnetSetRxIsrFunc();
    TEST_AmbaRTSL_EnetSetLcFunc();
    TEST_AmbaRTSL_EnetMiiRead();
    TEST_AmbaRTSL_EnetMiiWrite();
    TEST_AmbaRTSL_EnetGetPhyId();
    TEST_AmbaRTSL_EnetGetPhyAddr();
    TEST_AmbaRTSL_GetLink();
    TEST_AmbaRTSL_EnetINTEnable();
    TEST_AmbaRTSL_EnetINTDisable();
    TEST_AmbaRTSL_EnetInit();
    TEST_AmbaRTSL_EnetGetRegIdx();

    TEST_Eth_IrqHdlr();

    return 0;
}
