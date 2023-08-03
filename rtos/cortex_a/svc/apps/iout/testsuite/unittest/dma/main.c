#include <stdio.h>
#include <stdlib.h>
#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaDrvEntry.h"
#include "AmbaDMA.h"
#include "AmbaRTSL_DMA.h"
#include "AmbaCSL_DMA.h"
#include "AmbaReg_ScratchpadNS.h"

extern void Set_RetVal_EventFlagCreate(UINT32 RetVal);
extern void Set_RetVal_EventFlagClear(UINT32 RetVal);
extern void Set_RetVal_EventFlagGet(UINT32 RetVal);
extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);
extern void Set_RetVal_memset(UINT32 RetVal);

AMBA_DMA_CHAN_REG_s DmaChanReg = {0};
AMBA_DMA_CHAN_REG_s *pDmaChanReg = &DmaChanReg;
AMBA_SCRATCHPAD_NS_REG_s AmbaScratchpadNS_Reg = {0};
AMBA_SCRATCHPAD_NS_REG_s *const pAmbaScratchpadNS_Reg = &AmbaScratchpadNS_Reg;

void TEST_AmbaDMA_DrvEntry(void)
{
    AmbaDMA_DrvEntry();

    Set_RetVal_EventFlagCreate(KAL_ERR_0000);
    AmbaDMA_DrvEntry();
    Set_RetVal_EventFlagCreate(KAL_ERR_NONE);

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaDMA_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    Set_RetVal_memset(1U);
    AmbaDMA_DrvEntry();
    Set_RetVal_memset(0U);

    printf("TEST_AmbaDMA_DrvEntry\n");
}

void TEST_AmbaDMA_ChannelAllocate(void)
{
    UINT32 DmaChanType = AMBA_NUM_DMA_CHANNEL_TYPE - 1U;
    UINT32 DmaChanNo = 0U;
    UINT32 *pDmaChanNo = &DmaChanNo;
    AmbaDMA_ChannelAllocate(DmaChanType, pDmaChanNo);

    AmbaDMA_ChannelAllocate(DmaChanType, NULL);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaDMA_ChannelAllocate(DmaChanType, pDmaChanNo);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaDMA_ChannelAllocate(DmaChanType, pDmaChanNo);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    /* AmbaRTSL_DMA.c */
    AmbaRTSL_DmaChanAllocate(DmaChanType, NULL);

    DmaChanType = AMBA_NUM_DMA_CHANNEL_TYPE;
    AmbaDMA_ChannelAllocate(DmaChanType, pDmaChanNo);

    /* AmbaRTSL_DMA.c */
    AmbaRTSL_DmaChanAllocate(DmaChanType, pDmaChanNo);

    printf("TEST_AmbaDMA_ChannelAllocate\n");
}

void TEST_AmbaDMA_ChannelRelease(void)
{
    UINT32 DmaChanNo = AMBA_NUM_DMA_CHANNEL - 1U;
    AmbaDMA_ChannelRelease(DmaChanNo);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaDMA_ChannelRelease(DmaChanNo);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaDMA_ChannelRelease(DmaChanNo);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    DmaChanNo = AMBA_NUM_DMA_CHANNEL;
    AmbaDMA_ChannelRelease(DmaChanNo);

    /* AmbaRTSL_DMA.c */
    AmbaRTSL_DmaChanRelease(DmaChanNo);

    printf("TEST_AmbaDMA_ChannelRelease\n");
}

void TEST_AmbaDMA_Transfer(void)
{
    UINT32 DmaChanNo = AMBA_NUM_DMA_CHANNEL - 1U;
    AMBA_DMA_DESC_s DmaDesc = {0};
    AMBA_DMA_DESC_s *pDmaDesc = &DmaDesc;
    AmbaDMA_Transfer(DmaChanNo, pDmaDesc);

    AmbaDMA_Transfer(DmaChanNo, NULL);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaDMA_Transfer(DmaChanNo, pDmaDesc);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaDMA_Transfer(DmaChanNo, pDmaDesc);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaDMA_Transfer(DmaChanNo, pDmaDesc);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    /* AmbaRTSL_DMA.c */
    void *pNextDesc;
    pDmaDesc->pNextDesc = pNextDesc;
    AmbaDMA_Transfer(DmaChanNo, pDmaDesc);

    DmaChanNo = AMBA_DMA_CHANNEL_7;
    AmbaDMA_Transfer(DmaChanNo, pDmaDesc);

    AmbaRTSL_DmaTransfer(DmaChanNo, NULL);

    DmaChanNo = AMBA_NUM_DMA_CHANNEL;
    AmbaDMA_Transfer(DmaChanNo, pDmaDesc);

    /* AmbaRTSL_DMA.c */
    AmbaRTSL_DmaTransfer(DmaChanNo, pDmaDesc);

    printf("TEST_AmbaDMA_Transfer\n");
}

void TEST_AmbaDMA_Wait(void)
{
    UINT32 DmaChanNo = AMBA_NUM_DMA_CHANNEL - 1U;
    UINT32 TimeOut = 0U;
    AmbaDMA_Wait(DmaChanNo, TimeOut);

    Set_RetVal_EventFlagGet(KAL_ERR_0000);
    AmbaDMA_Wait(DmaChanNo, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_TIMEOUT);
    AmbaDMA_Wait(DmaChanNo, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_NONE);

    DmaChanNo = AMBA_NUM_DMA_CHANNEL;
    AmbaDMA_Wait(DmaChanNo, TimeOut);

    printf("TEST_AmbaDMA_Wait\n");
}

void TEST_AmbaDMA_GetIntCount(void)
{
    UINT32 DmaChanNo = AMBA_NUM_DMA_CHANNEL - 1U;
    AmbaDMA_GetIntCount(DmaChanNo);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaDMA_GetIntCount(DmaChanNo);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaDMA_GetIntCount(DmaChanNo);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    DmaChanNo = AMBA_NUM_DMA_CHANNEL;
    AmbaDMA_GetIntCount(DmaChanNo);

    printf("TEST_AmbaDMA_GetIntCount\n");
}

void TEST_AmbaDMA_ResetIntCount(void)
{
    UINT32 DmaChanNo = AMBA_NUM_DMA_CHANNEL - 1U;
    AmbaDMA_ResetIntCount(DmaChanNo);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaDMA_ResetIntCount(DmaChanNo);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaDMA_ResetIntCount(DmaChanNo);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    DmaChanNo = AMBA_NUM_DMA_CHANNEL;
    AmbaDMA_ResetIntCount(DmaChanNo);

    printf("TEST_AmbaDMA_ResetIntCount\n");
}

void TEST_AmbaRTSL_DmaStop(void)
{
    UINT32 DmaChanNo = AMBA_NUM_DMA_CHANNEL - 1U;
    AmbaRTSL_DmaStop(DmaChanNo);

    DmaChanNo = AMBA_DMA_CHANNEL_7;
    AmbaRTSL_DmaStop(DmaChanNo);

    pDmaChanReg->Ctrl = 0x80000000U;
    AmbaRTSL_DmaStop(DmaChanNo);
    pDmaChanReg->Ctrl = 0U;

    DmaChanNo = AMBA_NUM_DMA_CHANNEL;
    AmbaRTSL_DmaStop(DmaChanNo);

    printf("TEST_AmbaRTSL_DmaStop\n");
}

void TEST_AmbaRTSL_DmaGetTrfByteCount()
{
    UINT32 DmaChanNo = AMBA_NUM_DMA_CHANNEL - 1U;
    UINT32 ActualSize = 0U;
    UINT32 *pActualSize = &ActualSize;
    AmbaRTSL_DmaGetTrfByteCount(DmaChanNo, pActualSize);

    AmbaRTSL_DmaGetTrfByteCount(DmaChanNo, NULL);

    DmaChanNo = AMBA_DMA_CHANNEL_7;
    AmbaRTSL_DmaGetTrfByteCount(DmaChanNo, pActualSize);

    pDmaChanReg->Ctrl = 0x80000000U;
    AmbaRTSL_DmaGetTrfByteCount(DmaChanNo, pActualSize);
    pDmaChanReg->Ctrl = 0U;

    DmaChanNo = AMBA_NUM_DMA_CHANNEL;
    AmbaRTSL_DmaGetTrfByteCount(DmaChanNo, pActualSize);

    printf("TEST_AmbaRTSL_DmaGetTrfByteCount\n");
}

void TEST_AmbaRTSL_DmaSetChanType()
{
    UINT32 DmaChanNo = 0U;
    UINT32 DmaChanType = 0U;
    AmbaRTSL_DmaSetChanType(DmaChanNo, DmaChanType);

    printf("TEST_AmbaRTSL_DmaSetChanType\n");
}

void TEST_AmbaRTSL_DmaGetIrqStatus()
{
    UINT32 DmaChanNo = AMBA_DMA_CHANNEL_7;
    AmbaRTSL_DmaGetIrqStatus(DmaChanNo);

    DmaChanNo = AMBA_NUM_DMA_CHANNEL;
    AmbaRTSL_DmaGetIrqStatus(DmaChanNo);

    printf("TEST_AmbaRTSL_DmaGetIrqStatus\n");
}

void TEST_AmbaCSL_DmaGetChanFunc(void)
{
    UINT32 DmaChanNo = AMBA_DMA_CHANNEL_7;
    AmbaCSL_DmaGetChanFunc(DmaChanNo);

    DmaChanNo = AMBA_NUM_DMA_CHANNEL;
    AmbaCSL_DmaGetChanFunc(DmaChanNo);

    printf("TEST_AmbaCSL_DmaGetChanFunc\n");
}


int main(void)
{
    for (UINT8 i = 0U; i < 2U; i++) {
        pAmbaDMA_Reg[i] = malloc(sizeof(AMBA_DMA_REG_s));
    }

    /* avoid AmbaDMA_DrvEntry to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    TEST_AmbaDMA_DrvEntry();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    TEST_AmbaDMA_ChannelAllocate();
    TEST_AmbaDMA_ChannelRelease();
    TEST_AmbaDMA_Transfer();
    TEST_AmbaDMA_Wait();
    TEST_AmbaDMA_GetIntCount();
    TEST_AmbaDMA_ResetIntCount();

    TEST_AmbaRTSL_DmaStop();
    TEST_AmbaRTSL_DmaGetTrfByteCount();
    TEST_AmbaRTSL_DmaSetChanType();
    TEST_AmbaRTSL_DmaGetIrqStatus();

    TEST_AmbaCSL_DmaGetChanFunc();

    return 0;
}

