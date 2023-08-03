#include <stdio.h>
#include <stdlib.h>
#include "AmbaKAL.h"
#include "AmbaDrvEntry.h"
#include "AmbaSPI.h"
#include "AmbaRTSL_SPI.h"
#include "AmbaCSL_SPI.h"
#include "AmbaSPI_Def.h"

extern void Set_RetVal_EventFlagCreate(UINT32 RetVal);
extern void Set_RetVal_EventFlagClear(UINT32 RetVal);
extern void Set_RetVal_EventFlagGet(UINT32 RetVal);
extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);

AMBA_SPI_REG_s *pAmbaSPI_MasterReg[AMBA_NUM_SPI_MASTER];
AMBA_SPI_REG_s *pAmbaSPI_SlaveReg[AMBA_NUM_SPI_SLAVE];

void TEST_AmbaSPI_DrvEntry(void)
{
    AmbaSPI_DrvEntry();

    Set_RetVal_EventFlagCreate(KAL_ERR_0000);
    AmbaSPI_DrvEntry();
    Set_RetVal_EventFlagCreate(KAL_ERR_NONE);

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaSPI_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    printf("TEST_AmbaSPI_DrvEntry\n");
}

void TEST_AmbaSPI_MasterTransferD8(void)
{
    UINT32 MasterID = AMBA_NUM_SPI_MASTER - 1U;
    UINT32 SlaveMask = 0U;
    AMBA_SPI_CONFIG_s Config = {0};
    AMBA_SPI_CONFIG_s *pConfig = &Config;
    UINT32 BufSize = 1U;
    UINT8 TxBuf = 0U;
    UINT8 *pTxBuf = &TxBuf;
    UINT8 RxBuf = 0U;
    UINT8 *pRxBuf = &RxBuf;
    UINT32 ActualSize = 0U;
    UINT32 *pActualSize = &ActualSize;
    UINT32 TimeOut = 0U;
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, NULL, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, NULL, NULL, pActualSize, TimeOut);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiMasterConfig(MasterID, SlaveMask, NULL);

    AmbaRTSL_SpiMasterTransferD8(MasterID, BufSize, NULL, pRxBuf);

    AmbaRTSL_SpiMasterTransferD8(MasterID, BufSize, NULL, NULL);

    pConfig->BaudRate = 1U;
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    pConfig->DataFrameSize = 1U;
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    SlaveMask = 1U;
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    pConfig->ClkMode = 4U;
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    pConfig->ClkMode = SPI_CPOL_LO_CPHA_LO;
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    pConfig->ClkMode = SPI_CPOL_LO_CPHA_HI;
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    pConfig->ClkMode = SPI_CPOL_HI_CPHA_LO;
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    pConfig->ClkMode = SPI_CPOL_HI_CPHA_HI;
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    pConfig->CsPolarity = SPI_CS_ACTIVE_HI;
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    pConfig->CsPolarity = SPI_CS_ACTIVE_LO;
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    pConfig->FrameBitOrder = SPI_XFER_LSB_FIRST;
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    pAmbaSPI_MasterReg[MasterID]->Ctrl0.DataFrameSize = 8U;
    AmbaRTSL_SpiMasterTransferD8(MasterID, BufSize, pTxBuf, pRxBuf);
    pAmbaSPI_MasterReg[MasterID]->Ctrl0.DataFrameSize = 9U;
    /* AmbaRTSL_SPI.c */

    Set_RetVal_EventFlagGet(KAL_ERR_0000);
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_TIMEOUT);
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_NONE);

    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, NULL, TimeOut);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    BufSize = 0U;
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, NULL, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiMasterTransferD8(MasterID, BufSize, pTxBuf, pRxBuf);

    MasterID = AMBA_NUM_SPI_MASTER;
    AmbaSPI_MasterTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiMasterConfig(MasterID, SlaveMask, pConfig);

    AmbaRTSL_SpiMasterTransferD8(MasterID, BufSize, pTxBuf, pRxBuf);

    printf("TEST_AmbaSPI_MasterTransferD8\n");
    return;
}


void TEST_AmbaSPI_MasterTransferD16(void)
{
    UINT32 MasterID = AMBA_NUM_SPI_MASTER - 1U;
    UINT32 SlaveMask = 1U;
    AMBA_SPI_CONFIG_s Config = {0};
    AMBA_SPI_CONFIG_s *pConfig = &Config;
    UINT32 BufSize = 1U;
    UINT16 TxBuf = 0U;
    UINT16 *pTxBuf = &TxBuf;
    UINT16 RxBuf = 0U;
    UINT16 *pRxBuf = &RxBuf;
    UINT32 ActualSize = 0U;
    UINT32 *pActualSize = &ActualSize;
    UINT32 TimeOut = 0U;
    AmbaSPI_MasterTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_MasterTransferD16(MasterID, SlaveMask, pConfig, BufSize, NULL, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_MasterTransferD16(MasterID, SlaveMask, pConfig, BufSize, NULL, NULL, pActualSize, TimeOut);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiMasterTransferD16(MasterID, BufSize, NULL, pRxBuf);

    AmbaRTSL_SpiMasterTransferD16(MasterID, BufSize, NULL, NULL);

    pConfig->BaudRate = 1U;
    pConfig->DataFrameSize = 1U;
    AmbaSPI_MasterTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    pAmbaSPI_MasterReg[MasterID]->Ctrl0.DataFrameSize = 9U;
    AmbaRTSL_SpiMasterTransferD16(MasterID, BufSize, pTxBuf, pRxBuf);
    pAmbaSPI_MasterReg[MasterID]->Ctrl0.DataFrameSize = 0U;
    /* AmbaRTSL_SPI.c */

    Set_RetVal_EventFlagGet(KAL_ERR_0000);
    AmbaSPI_MasterTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_TIMEOUT);
    AmbaSPI_MasterTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_NONE);

    AmbaSPI_MasterTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, NULL, TimeOut);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaSPI_MasterTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSPI_MasterTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaSPI_MasterTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    BufSize = 0U;
    AmbaSPI_MasterTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_MasterTransferD16(MasterID, SlaveMask, NULL, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiMasterTransferD16(MasterID, BufSize, pTxBuf, pRxBuf);

    MasterID = AMBA_NUM_SPI_MASTER;
    AmbaSPI_MasterTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiMasterTransferD16(MasterID, BufSize, pTxBuf, pRxBuf);

    printf("TEST_AmbaSPI_MasterTransferD16\n");
    return;
}

void TEST_AmbaSPI_MasterDmaTransferD8(void)
{
    UINT32 MasterID = AMBA_NUM_SPI_MASTER - 1U;
    UINT32 SlaveMask = 1U;
    AMBA_SPI_CONFIG_s Config = {0};
    AMBA_SPI_CONFIG_s *pConfig = &Config;
    UINT32 BufSize = 1U;
    UINT8 TxBuf = 0U;
    UINT8 *pTxBuf = &TxBuf;
    UINT8 RxBuf = 0U;
    UINT8 *pRxBuf = &RxBuf;
    UINT32 ActualSize = 0U;
    UINT32 *pActualSize = &ActualSize;
    UINT32 TimeOut = 0U;
    AmbaSPI_MasterDmaTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_MasterDmaTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, NULL, pActualSize, TimeOut);

    AmbaSPI_MasterDmaTransferD8(MasterID, SlaveMask, pConfig, BufSize, NULL, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_MasterDmaTransferD8(MasterID, SlaveMask, pConfig, BufSize, NULL, NULL, pActualSize, TimeOut);

    AmbaSPI_MasterDmaTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, NULL, TimeOut);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaSPI_MasterDmaTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSPI_MasterDmaTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaSPI_MasterDmaTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    BufSize = 0U;
    AmbaSPI_MasterDmaTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_MasterDmaTransferD8(MasterID, SlaveMask, NULL, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    MasterID = AMBA_NUM_SPI_MASTER;
    AmbaSPI_MasterDmaTransferD8(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    printf("TEST_AmbaSPI_MasterDmaTransferD8\n");
    return;
}

void TEST_AmbaSPI_MasterDmaTransferD16(void)
{
    UINT32 MasterID = AMBA_NUM_SPI_MASTER - 1U;
    UINT32 SlaveMask = 1U;
    AMBA_SPI_CONFIG_s Config = {0};
    AMBA_SPI_CONFIG_s *pConfig = &Config;
    UINT32 BufSize = 1U;
    UINT16 TxBuf = 0U;
    UINT16 *pTxBuf = &TxBuf;
    UINT16 RxBuf = 0U;
    UINT16 *pRxBuf = &RxBuf;
    UINT32 ActualSize = 0U;
    UINT32 *pActualSize = &ActualSize;
    UINT32 TimeOut = 0U;
    AmbaSPI_MasterDmaTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_MasterDmaTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, NULL, pActualSize, TimeOut);

    AmbaSPI_MasterDmaTransferD16(MasterID, SlaveMask, pConfig, BufSize, NULL, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_MasterDmaTransferD16(MasterID, SlaveMask, pConfig, BufSize, NULL, NULL, pActualSize, TimeOut);

    AmbaSPI_MasterDmaTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, NULL, TimeOut);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaSPI_MasterDmaTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSPI_MasterDmaTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaSPI_MasterDmaTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    BufSize = 0U;
    AmbaSPI_MasterDmaTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_MasterDmaTransferD16(MasterID, SlaveMask, NULL, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    MasterID = AMBA_NUM_SPI_MASTER;
    AmbaSPI_MasterDmaTransferD16(MasterID, SlaveMask, pConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    printf("TEST_AmbaSPI_MasterDmaTransferD16\n");
    return;
}

void TEST_AmbaSPI_MasterGetInfo(void)
{
    UINT32 MasterID = AMBA_NUM_SPI_MASTER - 1U;
    UINT32 NumSlaves = 0U;
    UINT32 *pNumSlaves = &NumSlaves;
    UINT32 Status = 0U;
    UINT32 *pStatus = &Status;
    AmbaSPI_MasterGetInfo(MasterID, pNumSlaves, pStatus);

    AmbaSPI_MasterGetInfo(MasterID, pNumSlaves, NULL);

    AmbaSPI_MasterGetInfo(MasterID, NULL, pStatus);

    /* AmbaRTSL_SPI.c */
    pAmbaSPI_MasterReg[MasterID]->Status = 0x60U;
    AmbaSPI_MasterGetInfo(MasterID, pNumSlaves, pStatus);

    pAmbaSPI_MasterReg[MasterID]->Status = 0x1U;
    AmbaSPI_MasterGetInfo(MasterID, pNumSlaves, pStatus);

    AmbaRTSL_SpiMasterGetInfo(MasterID, NULL, pStatus, NULL);

    AmbaRTSL_SpiMasterGetInfo(MasterID, NULL, NULL, NULL);

    MasterID = AMBA_NUM_SPI_MASTER;
    AmbaSPI_MasterGetInfo(MasterID, pNumSlaves, pStatus);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiMasterGetInfo(MasterID, pNumSlaves, pStatus, NULL);

    printf("TEST_AmbaSPI_MasterGetInfo\n");
    return;
}

void TEST_AmbaSPI_SlaveConfig(void)
{
    UINT32 SlaveID = AMBA_NUM_SPI_SLAVE - 1U;
    AMBA_SPI_CONFIG_s Config = {0};
    AMBA_SPI_CONFIG_s * pConfig = &Config;
    AmbaSPI_SlaveConfig(SlaveID, pConfig);

    AmbaSPI_SlaveConfig(SlaveID, NULL);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiSlaveConfig(SlaveID, NULL);

    pConfig->BaudRate = 1U;
    AmbaSPI_SlaveConfig(SlaveID, pConfig);

    pConfig->DataFrameSize = 1U;
    AmbaSPI_SlaveConfig(SlaveID, pConfig);

    pConfig->ClkMode = 4U;
    AmbaSPI_SlaveConfig(SlaveID, pConfig);

    pConfig->ClkMode = SPI_CPOL_LO_CPHA_LO;
    AmbaSPI_SlaveConfig(SlaveID, pConfig);

    pConfig->ClkMode = SPI_CPOL_LO_CPHA_HI;
    AmbaSPI_SlaveConfig(SlaveID, pConfig);

    pConfig->ClkMode = SPI_CPOL_HI_CPHA_LO;
    AmbaSPI_SlaveConfig(SlaveID, pConfig);

    pConfig->ClkMode = SPI_CPOL_HI_CPHA_HI;
    AmbaSPI_SlaveConfig(SlaveID, pConfig);

    pConfig->CsPolarity = SPI_CS_ACTIVE_HI;
    AmbaSPI_SlaveConfig(SlaveID, pConfig);

    pConfig->CsPolarity = SPI_CS_ACTIVE_LO;
    AmbaSPI_SlaveConfig(SlaveID, pConfig);

    pConfig->FrameBitOrder = SPI_XFER_LSB_FIRST;
    AmbaSPI_SlaveConfig(SlaveID, pConfig);
    /* AmbaRTSL_SPI.c */

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaSPI_SlaveConfig(SlaveID, pConfig);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSPI_SlaveConfig(SlaveID, pConfig);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    SlaveID = AMBA_NUM_SPI_SLAVE;
    AmbaSPI_SlaveConfig(SlaveID, pConfig);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiSlaveConfig(SlaveID, pConfig);

    printf("TEST_AmbaSPI_SlaveConfig\n");
    return;
}

void TEST_AmbaSPI_SlaveTransferD8(void)
{
    UINT32 SlaveID = AMBA_NUM_SPI_SLAVE - 1U;
    UINT32 BufSize = 1U;
    UINT8 TxBuf = 0U;
    UINT8 *pTxBuf = &TxBuf;
    UINT8 RxBuf = 0U;
    UINT8 *pRxBuf = &RxBuf;
    UINT32 ActualSize = 0U;
    UINT32 *pActualSize = &ActualSize;
    UINT32 TimeOut = 0U;
    AmbaSPI_SlaveTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_SlaveTransferD8(SlaveID, BufSize, NULL, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_SlaveTransferD8(SlaveID, BufSize, NULL, NULL, pActualSize, TimeOut);

    Set_RetVal_EventFlagGet(KAL_ERR_0000);
    AmbaSPI_SlaveTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_TIMEOUT);
    AmbaSPI_SlaveTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_NONE);

    AmbaSPI_SlaveTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf, NULL, TimeOut);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaSPI_SlaveTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSPI_SlaveTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaSPI_SlaveTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiSlaveTransferD8(SlaveID, BufSize, NULL, NULL);

    pAmbaSPI_SlaveReg[SlaveID]->Ctrl0.DataFrameSize = 9U;
    AmbaRTSL_SpiSlaveTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf);
    pAmbaSPI_SlaveReg[SlaveID]->Ctrl0.DataFrameSize = 0U;
    /* AmbaRTSL_SPI.c */

    BufSize = 0U;
    AmbaSPI_SlaveTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiSlaveTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf);

    SlaveID = AMBA_NUM_SPI_SLAVE;
    AmbaSPI_SlaveTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiSlaveTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf);

    printf("TEST_AmbaSPI_SlaveTransferD8\n");
    return;
}

void TEST_AmbaSPI_SlaveTransferD16(void)
{
    UINT32 SlaveID = AMBA_NUM_SPI_SLAVE - 1U;
    UINT32 BufSize = 1U;
    UINT16 TxBuf = 0U;
    UINT16 *pTxBuf = &TxBuf;
    UINT16 RxBuf = 0U;
    UINT16 *pRxBuf = &RxBuf;
    UINT32 ActualSize = 0U;
    UINT32 *pActualSize = &ActualSize;
    UINT32 TimeOut = 0U;
    AmbaSPI_SlaveTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_SlaveTransferD16(SlaveID, BufSize, NULL, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_SlaveTransferD16(SlaveID, BufSize, NULL, NULL, pActualSize, TimeOut);

    Set_RetVal_EventFlagGet(KAL_ERR_0000);
    AmbaSPI_SlaveTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_TIMEOUT);
    AmbaSPI_SlaveTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_NONE);

    AmbaSPI_SlaveTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, NULL, TimeOut);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaSPI_SlaveTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSPI_SlaveTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaSPI_SlaveTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiSlaveTransferD16(SlaveID, BufSize, NULL, NULL);

    pAmbaSPI_SlaveReg[SlaveID]->Ctrl0.DataFrameSize = 9U;
    AmbaRTSL_SpiSlaveTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf);
    pAmbaSPI_SlaveReg[SlaveID]->Ctrl0.DataFrameSize = 0U;
    /* AmbaRTSL_SPI.c */

    BufSize = 0U;
    AmbaSPI_SlaveTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiSlaveTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf);

    SlaveID = AMBA_NUM_SPI_SLAVE;
    AmbaSPI_SlaveTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiSlaveTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf);

    printf("TEST_AmbaSPI_SlaveTransferD16\n");
    return;
}

void TEST_AmbaSPI_SlaveDmaTransferD8(void)
{
    UINT32 SlaveID = AMBA_NUM_SPI_SLAVE - 1U;
    UINT32 BufSize = 9U;
    UINT8 TxBuf = 0U;
    UINT8 *pTxBuf = &TxBuf;
    UINT8 RxBuf = 0U;
    UINT8 *pRxBuf = &RxBuf;
    UINT32 ActualSize = 0U;
    UINT32 *pActualSize = &ActualSize;
    UINT32 TimeOut = 0U;

    AmbaSPI_SlaveDmaTransferD8(SlaveID, BufSize, NULL, NULL, pActualSize, TimeOut);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaSPI_SlaveDmaTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSPI_SlaveDmaTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    BufSize = 0U;
    AmbaSPI_SlaveDmaTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    SlaveID = AMBA_NUM_SPI_SLAVE;
    AmbaSPI_SlaveDmaTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    printf("TEST_AmbaSPI_SlaveDmaTransferD8\n");
    return;
}

void TEST_AmbaSPI_SlaveDmaTransferD16(void)
{
    UINT32 SlaveID = AMBA_NUM_SPI_SLAVE - 1U;
    UINT32 BufSize = 9U;
    UINT16 TxBuf = 0U;
    UINT16 *pTxBuf = &TxBuf;
    UINT16 RxBuf = 0U;
    UINT16 *pRxBuf = &RxBuf;
    UINT32 ActualSize = 0U;
    UINT32 *pActualSize = &ActualSize;
    UINT32 TimeOut = 0U;
    AmbaSPI_SlaveDmaTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_SlaveDmaTransferD16(SlaveID, BufSize, NULL, pRxBuf, pActualSize, TimeOut);

    AmbaSPI_SlaveDmaTransferD16(SlaveID, BufSize, NULL, NULL, pActualSize, TimeOut);

    AmbaSPI_SlaveDmaTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, NULL, TimeOut);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaSPI_SlaveDmaTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSPI_SlaveDmaTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaSPI_SlaveDmaTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiSlaveDmaTranD16(SlaveID, BufSize, NULL, NULL);

    BufSize = 0U;
    AmbaSPI_SlaveDmaTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiSlaveDmaTranD16(SlaveID, BufSize, pTxBuf, pRxBuf);

    SlaveID = AMBA_NUM_SPI_SLAVE;
    AmbaSPI_SlaveDmaTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiSlaveDmaTranD16(SlaveID, BufSize, pTxBuf, pRxBuf);

    printf("TEST_AmbaSPI_SlaveDmaTransferD16\n");
    return;
}

void TEST_AmbaSPI_SlaveGetInfo(void)
{
    UINT32 SlaveID = AMBA_NUM_SPI_SLAVE - 1U;
    UINT32 NumSlaves = 0U;
    UINT32 *pNumSlaves = &NumSlaves;
    UINT32 Status = 0U;
    UINT32 *pStatus = &Status;
    AmbaSPI_SlaveGetInfo(SlaveID, pStatus);

    AmbaSPI_SlaveGetInfo(SlaveID, NULL);

    /* AmbaRTSL_SPI.c */
    pAmbaSPI_SlaveReg[SlaveID]->Status = 0x60U;
    AmbaSPI_SlaveGetInfo(SlaveID, pStatus);

    pAmbaSPI_SlaveReg[SlaveID]->Status = 0x1U;
    AmbaSPI_SlaveGetInfo(SlaveID, pStatus);

    AmbaRTSL_SpiSlaveGetInfo(SlaveID, NULL, NULL);

    SlaveID = AMBA_NUM_SPI_SLAVE;
    AmbaSPI_SlaveGetInfo(SlaveID, pStatus);

    /* AmbaRTSL_SPI.c */
    AmbaRTSL_SpiSlaveGetInfo(SlaveID, pStatus, NULL);

    printf("TEST_AmbaSPI_SlaveGetInfo\n");
    return;
}

void TEST_SPI_MasterISR(void)
{
    UINT32 Irq = 0U;
    UINT32 SpiChanNo = 0U;
    SPI_MasterISR(Irq, SpiChanNo);

    printf("TEST_SPI_MasterISR\n");
    return;
}

void TEST_SPI_SlaveISR(void)
{
    UINT32 Irq = 0U;
    UINT32 SpiChanNo = 0U;
    SPI_SlaveISR(Irq, SpiChanNo);

    printf("TEST_SPI_SlaveISR\n");
    return;
}

int main(void)
{
    for (UINT8 i = 0U; i < AMBA_NUM_SPI_MASTER; i++) {
        pAmbaSPI_MasterReg[i] = malloc(sizeof(AMBA_SPI_REG_s));
    }

    for (UINT8 i = 0U; i < AMBA_NUM_SPI_SLAVE; i++) {
        pAmbaSPI_SlaveReg[i] = malloc(sizeof(AMBA_SPI_REG_s));
    }

    /* avoid AmbaSPI_DrvEntry to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    TEST_AmbaSPI_DrvEntry();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    TEST_AmbaSPI_MasterTransferD8();
    TEST_AmbaSPI_MasterTransferD16();
    TEST_AmbaSPI_MasterDmaTransferD8();
    TEST_AmbaSPI_MasterDmaTransferD16();
    TEST_AmbaSPI_MasterGetInfo();
    TEST_AmbaSPI_SlaveConfig();
    TEST_AmbaSPI_SlaveTransferD8();
    TEST_AmbaSPI_SlaveTransferD16();
    TEST_AmbaSPI_SlaveDmaTransferD8();
    TEST_AmbaSPI_SlaveDmaTransferD16();
    TEST_AmbaSPI_SlaveGetInfo();

    TEST_SPI_MasterISR();
    TEST_SPI_SlaveISR();

    return 0;
}

