#include "AmbaTypes.h"
#include "AmbaENET_Def.h"
#include "AmbaReg_ENET.h"
#include "AmbaCSL_ENET.h"

AMBA_ENET_TS_s *pAmbaCSL_EnetPhc[ENET_INSTANCES];
AMBA_ENET_REG_s *pAmbaCSL_EnetReg[ENET_INSTANCES];
AMBA_ENET_STAT_s AmbaCSL_EnetStat[ENET_INSTANCES];

static UINT32 RetVal_EnetRDESGetDmaOwn = ETH_ERR_NONE;
static UINT32 RetVal_EnetTDESGetDmaOwn = ETH_ERR_NONE;
static UINT32 RetVal_EnetTDESGetDmaES  = ETH_ERR_NONE;
static UINT32 RetVal_EnetDmaReset      = ETH_ERR_NONE;
static UINT32 RetVal_EnetMacGetTSAR    = ETH_ERR_NONE;
static UINT32 RetVal_EnetTDESGetTTSS   = ETH_ERR_NONE;
static UINT32 RetVal_EnetDmaGetRI      = ETH_ERR_NONE;
static UINT32 RetVal_EnetDmaGetRU      = ETH_ERR_NONE;
static UINT32 RetVal_EnetDmaGetTI      = ETH_ERR_NONE;
static UINT32 RetVal_EnetDmaGetTU      = ETH_ERR_NONE;
static UINT32 RetVal_EnetDmaGetGLI     = ETH_ERR_NONE;
static UINT32 RetVal_EnetDmaGetTTI     = ETH_ERR_NONE;
static UINT32 RetVal_EnetMacGetMiiData = ETH_ERR_NONE;

void (*fpCB_EnetRDESGetDmaOwn)(void);
void (*fpCB_EnetTDESGetDmaOwn)(void);
void (*fpCB_EnetTDESGetDmaES)(void);
void (*fpCB_EnetDmaReset)(void);
void (*fpCB_EnetMacGetTSAR)(void);
void (*fpCB_EnetTDESGetTTSS)(void);
UINT32 (*fpCB_AmbaRTSL_EnetMiiRead)(void);

UINT32 Get_RetVal_EnetTDESGetDmaOwn(void)
{
    return RetVal_EnetTDESGetDmaOwn;
}

void Set_RetVal_EnetRDESGetDmaOwn(UINT32 RetVal)
{
    RetVal_EnetRDESGetDmaOwn = RetVal;
}
void Set_RetVal_EnetTDESGetDmaOwn(UINT32 RetVal)
{
    RetVal_EnetTDESGetDmaOwn = RetVal;
}
void Set_RetVal_EnetTDESGetDmaES(UINT32 RetVal)
{
    RetVal_EnetTDESGetDmaES = RetVal;
}
void Set_RetVal_EnetDmaReset(UINT32 RetVal)
{
    RetVal_EnetDmaReset = RetVal;
}
void Set_RetVal_EnetMacGetTSAR(UINT32 RetVal)
{
    RetVal_EnetMacGetTSAR = RetVal;
}
void Set_RetVal_EnetTDESGetTTSS(UINT32 RetVal)
{
    RetVal_EnetTDESGetTTSS = RetVal;
}
void Set_RetVal_EnetDmaGetRI(UINT32 RetVal)
{
    RetVal_EnetDmaGetRI = RetVal;
}
void Set_RetVal_EnetDmaGetRU(UINT32 RetVal)
{
    RetVal_EnetDmaGetRU = RetVal;
}
void Set_RetVal_EnetDmaGetTI(UINT32 RetVal)
{
    RetVal_EnetDmaGetTI = RetVal;
}
void Set_RetVal_EnetDmaGetTU(UINT32 RetVal)
{
    RetVal_EnetDmaGetTU = RetVal;
}
void Set_RetVal_EnetDmaGetGLI(UINT32 RetVal)
{
    RetVal_EnetDmaGetGLI = RetVal;
}
void Set_RetVal_EnetDmaGetTTI(UINT32 RetVal)
{
    RetVal_EnetDmaGetTTI = RetVal;
}
void Set_RetVal_EnetMacGetMiiData(UINT32 RetVal)
{
    RetVal_EnetMacGetMiiData = RetVal;
}

UINT32 AmbaCSL_EnetRDESGetDmaOwn(const AMBA_ENET_RDES_s *const pEnetRDES)
{
    UINT32 Ret = RetVal_EnetRDESGetDmaOwn;
    if (fpCB_EnetRDESGetDmaOwn != NULL) {
        fpCB_EnetRDESGetDmaOwn();
    }
    return Ret;
}

UINT32 AmbaCSL_EnetTDESGetDmaOwn(const AMBA_ENET_TDES_s *const pEnetTDES)
{
    UINT32 Ret = RetVal_EnetTDESGetDmaOwn;
    if (fpCB_EnetTDESGetDmaOwn != NULL) {
        fpCB_EnetTDESGetDmaOwn();
    }
    return Ret;
}

UINT32 AmbaCSL_EnetTDESGetDmaES(const AMBA_ENET_TDES_s *const pEnetTDES)
{
    UINT32 Ret = RetVal_EnetTDESGetDmaES;
    if (fpCB_EnetTDESGetDmaES != NULL) {
        fpCB_EnetTDESGetDmaES();
    }
    return Ret;
}

UINT32 AmbaCSL_EnetDmaReset(AMBA_ENET_REG_s *const pEnetReg)
{
    UINT32 Ret = RetVal_EnetDmaReset;
    if (fpCB_EnetDmaReset != NULL) {
        fpCB_EnetDmaReset();
    }
    return Ret;
}

UINT32 AmbaCSL_EnetMacGetTSAR(const AMBA_ENET_REG_s *const pEnetReg)
{
    UINT32 Ret = RetVal_EnetMacGetTSAR;
    if (fpCB_EnetMacGetTSAR != NULL) {
        fpCB_EnetMacGetTSAR();
    }
    return Ret;
}

UINT32 AmbaCSL_EnetTDESGetTTSS(const AMBA_ENET_TDES_s *const pEnetTDES)
{
    UINT32 Ret = RetVal_EnetTDESGetTTSS;
    if (fpCB_EnetTDESGetTTSS != NULL) {
        fpCB_EnetTDESGetTTSS();
    }
    return Ret;
}

UINT16 AmbaCSL_EnetRDESGetFL(const AMBA_ENET_RDES_s *const pEnetRDES)
{
    return 0U;
}

void AmbaCSL_EnetRDESDumpStatus(const AMBA_ENET_RDES_s *pEnetRDES, AMBA_ENET_STAT_s *pEnetStat)
{

}

void AmbaCSL_EnetRDESSetDmaOwn(AMBA_ENET_RDES_s *pEnetRDES)
{

}

void AmbaCSL_EnetRDES0Init(AMBA_ENET_RDES_s *pEnetRDES)
{

}

void AmbaCSL_EnetRDES1Init(AMBA_ENET_RDES_s *pEnetRDES)
{

}

void AmbaCSL_EnetRDES2Init(AMBA_ENET_RDES_s *pEnetRDES, UINT32 Buf1)
{

}

void AmbaCSL_EnetRDES3Init(AMBA_ENET_RDES_s *pEnetRDES, UINT32 Buf2)
{

}

void AmbaCSL_EnetRDESSetRBS1(AMBA_ENET_RDES_s *pEnetRDES, UINT16 RBS1)
{

}

void AmbaCSL_EnetTDESDumpStatus(const AMBA_ENET_TDES_s *pEnetTDES, AMBA_ENET_STAT_s *pEnetStat)
{

}

void AmbaCSL_EnetTDES0Init(AMBA_ENET_TDES_s *pEnetTDES)
{

}

void AmbaCSL_EnetTDES1Init(AMBA_ENET_TDES_s *pEnetTDES)
{

}

void AmbaCSL_EnetTDES2Init(AMBA_ENET_TDES_s *pEnetTDES, UINT32 Buf1)
{

}

void AmbaCSL_EnetTDES3Init(AMBA_ENET_TDES_s *pEnetTDES, UINT32 Buf2)
{

}

void AmbaCSL_EnetTDESSetCIC(AMBA_ENET_TDES_s *pEnetTDES, UINT8 CIC)
{

}

void AmbaCSL_EnetTDESSetDC(AMBA_ENET_TDES_s *pEnetTDES, UINT8 DC)
{

}

void AmbaCSL_EnetTDESSetTBS1(AMBA_ENET_TDES_s *pEnetTDES, UINT16 TBS1)
{

}

void AmbaCSL_EnetTDESSetDmaOwn(AMBA_ENET_TDES_s *pEnetTDES)
{

}

void AmbaCSL_EnetTDESSetLSFS(AMBA_ENET_TDES_s *pEnetTDES, UINT8 LS, UINT8 FS)
{

}

void AmbaCSL_EnetDmaInitR0(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetDmaInitR6(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetDmaInitR7(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetDmaInitR10(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetDmaStopTX(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetDmaSetTxDES(AMBA_ENET_REG_s *const pEnetReg, UINT32 TDESLA)
{

}

void AmbaCSL_EnetDmaDumpStatus(const AMBA_ENET_REG_s *const pEnetReg, AMBA_ENET_STAT_s *pEnetStat)
{

}

UINT32 AmbaCSL_EnetDmaGetRI(const AMBA_ENET_REG_s *const pEnetReg)
{
    return RetVal_EnetDmaGetRI;
}

UINT32 AmbaCSL_EnetDmaGetRU(const AMBA_ENET_REG_s *const pEnetReg)
{
    return RetVal_EnetDmaGetRU;
}

UINT32 AmbaCSL_EnetDmaGetTI(const AMBA_ENET_REG_s *const pEnetReg)
{
    return RetVal_EnetDmaGetTI;
}

UINT32 AmbaCSL_EnetDmaGetTU(const AMBA_ENET_REG_s *const pEnetReg)
{
    return RetVal_EnetDmaGetTU;
}

UINT32 AmbaCSL_EnetDmaGetGLI(const AMBA_ENET_REG_s *const pEnetReg)
{
    return RetVal_EnetDmaGetGLI;
}

UINT32 AmbaCSL_EnetDmaGetTTI(const AMBA_ENET_REG_s *const pEnetReg)
{
    return RetVal_EnetDmaGetTTI;
}

void AmbaCSL_EnetDmaRxIrqEnable(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetDmaRxIrqDisable(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetDmaStopRX(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetDmaSetRxDES(AMBA_ENET_REG_s *const pEnetReg, UINT32 RDESLA)
{

}

UINT32 AmbaCSL_EnetDmaGetDmaCurTxDes(const AMBA_ENET_REG_s *const pEnetReg)
{
    return 0U;
}

UINT32 AmbaCSL_EnetDmaGetDmaCurRxDes(const AMBA_ENET_REG_s *const pEnetReg)
{
    return 0U;
}

void AmbaCSL_EnetDmaStartRX(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetDmaSetRxPoll(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetDmaStartTX(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetDmaSetTxPoll(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetDmaClearStatus(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetMacSetAddrHi(AMBA_ENET_REG_s *const pEnetReg, UINT32 ADDRHI)
{

}

void AmbaCSL_EnetMacSetAddrLo(AMBA_ENET_REG_s *const pEnetReg, UINT32 ADDRLO)
{

}

void AmbaCSL_EnetMacLinkSetup(AMBA_ENET_REG_s *const pEnetReg, UINT32 SPEED, UINT32 DUPLEX)
{

}

UINT32 AmbaCSL_EnetMacGetPhcTsAddr(AMBA_ENET_REG_s *const pEnetReg, volatile void **Addr)
{
    return 0U;
}

void AmbaCSL_EnetMacSetJEJD(AMBA_ENET_REG_s *const pEnetReg, UINT8 JE, UINT8 JD)
{

}

void AmbaCSL_EnetMacSetTSTR(AMBA_ENET_REG_s *const pEnetReg, UINT32 TSTR)
{

}

void AmbaCSL_EnetMacSetTTSLO(AMBA_ENET_REG_s *const pEnetReg, UINT32 TTSLO)
{

}

void AmbaCSL_EnetMacSetTSIM(AMBA_ENET_REG_s *const pEnetReg, UINT8 TSIM)
{

}

void AmbaCSL_EnetMacSetTSTRIG(AMBA_ENET_REG_s *const pEnetReg, UINT8 TSTRIG)
{

}

void AmbaCSL_EnetMacSetPPSCMD0(AMBA_ENET_REG_s *const pEnetReg, UINT8 PPSCMD0)
{

}

void AmbaCSL_EnetMacSetPPSINT(AMBA_ENET_REG_s *const pEnetReg, UINT32 PPSINT)
{

}

void AmbaCSL_EnetMacSetPPSWIDTH(AMBA_ENET_REG_s *const pEnetReg, UINT32 PPSWIDTH)
{

}

void AmbaCSL_EnetMacSetTSS(AMBA_ENET_REG_s *const pEnetReg, UINT32 TSS)
{

}

void AmbaCSL_EnetMacSetTSSS(AMBA_ENET_REG_s *const pEnetReg, UINT8 ADDSUB, UINT32 TSSS)
{

}

void AmbaCSL_EnetMacTSINIT(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetMacTSUPDT(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetMacSetTSAR(AMBA_ENET_REG_s *const pEnetReg, UINT32 TSAR)
{

}

void AmbaCSL_EnetMacSetTSADDREG(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetMacSetHashLo(AMBA_ENET_REG_s *const pEnetReg, UINT32 HTL)
{

}

void AmbaCSL_EnetMacSetHashHi(AMBA_ENET_REG_s *const pEnetReg, UINT32 HTH)
{

}

void AmbaCSL_EnetMacTxDisable(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetMacRxDisable(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetMacInitR1(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetMacInitR6(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetMacInitR15(AMBA_ENET_REG_s *const pEnetReg)
{

}

void AmbaCSL_EnetMacInitR448(AMBA_ENET_REG_s *const pEnetReg)
{

}

UINT32 AmbaCSL_EnetMacGetMacR14(const AMBA_ENET_REG_s *const pEnetReg)
{

}

UINT32 AmbaCSL_EnetMacGetMacR15(const AMBA_ENET_REG_s *const pEnetReg)
{
    return 0U;
}

UINT32 AmbaCSL_EnetMacGetMacR49(const AMBA_ENET_REG_s *const pEnetReg)
{
    return 0U;
}

UINT32 AmbaCSL_EnetMacGetMacR54(const AMBA_ENET_REG_s *const pEnetReg)
{
    return 0U;
}

UINT32 AmbaCSL_EnetMacGetMacR56(const AMBA_ENET_REG_s *const pEnetReg)
{
    return 0U;
}

UINT32 AmbaCSL_EnetMacGetMacR458(const AMBA_ENET_REG_s *const pEnetReg)
{
    return 0U;
}

void AmbaCSL_EnetMacMiiRead(AMBA_ENET_REG_s *const pEnetReg, UINT8 GR, UINT8 PA)
{

}

UINT32 AmbaCSL_EnetMacGetMiiData(AMBA_ENET_REG_s *const pEnetReg)
{
    UINT32 Ret = RetVal_EnetMacGetMiiData;
    if (fpCB_AmbaRTSL_EnetMiiRead != NULL) {
        RetVal_EnetMacGetMiiData = fpCB_AmbaRTSL_EnetMiiRead();
    }
    return RetVal_EnetMacGetMiiData;
}

void AmbaCSL_EnetMacSetMiiData(AMBA_ENET_REG_s *const pEnetReg, UINT32 GB)
{

}

void AmbaCSL_EnetMacSetSSINC(AMBA_ENET_REG_s *const pEnetReg, UINT32 SSINC)
{

}

void AmbaCSL_EnetMacSetTRGTMODSEL0(AMBA_ENET_REG_s *const pEnetReg, UINT8 TRGTMODSEL0)
{

}

void AmbaCSL_EnetMacMiiWrite(AMBA_ENET_REG_s *const pEnetReg, UINT8 GR, UINT8 PA)
{

}
