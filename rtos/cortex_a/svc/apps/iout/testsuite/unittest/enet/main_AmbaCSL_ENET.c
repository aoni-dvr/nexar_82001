#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaTypes.h"

#include "src/AmbaCSL_ENET.c"

static AMBA_ENET_REG_s gEnetReg = {0};
static AMBA_ENET_TDES_s gEnetTDES = {0};
static AMBA_ENET_RDES_s gEnetRDES = {0};
static AMBA_ENET_STAT_s gEnetStat = {0};
static void *pAddr;


/****************************************************/
/*                dummy Func                        */
/****************************************************/


/****************************************************/
/*                Local Stub Func                   */
/****************************************************/


/****************************************************/
/*                Main Test                         */
/****************************************************/
void TEST_WriteLongEnet(void)
{
    printf("TEST_WriteLongEnet 1\n");
    /* arrange */
    /* act */
    WriteLongEnet(NULL, 0);

    printf("TEST_WriteLongEnet done\n");
}

void TEST_AmbaCSL_EnetTDES0Init(void)
{
    printf("TEST_AmbaCSL_EnetTDES0Init 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetTDES0Init(&gEnetTDES);

    printf("TEST_AmbaCSL_EnetTDES0Init done\n");
}

void TEST_AmbaCSL_EnetTDESSetDmaOwn(void)
{
    printf("TEST_AmbaCSL_EnetTDESSetDmaOwn 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetTDESSetDmaOwn(&gEnetTDES);

    printf("TEST_AmbaCSL_EnetTDESSetDmaOwn done\n");
}

void TEST_AmbaCSL_EnetTDESGetDmaOwn(void)
{
    printf("TEST_AmbaCSL_EnetTDESGetDmaOwn 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetTDESGetDmaOwn(&gEnetTDES);

    printf("TEST_AmbaCSL_EnetTDESGetDmaOwn done\n");
}

void TEST_AmbaCSL_EnetTDESGetDmaES(void)
{
    printf("TEST_AmbaCSL_EnetTDESGetDmaES 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetTDESGetDmaES(&gEnetTDES);

    printf("TEST_AmbaCSL_EnetTDESGetDmaES done\n");
}

void TEST_AmbaCSL_EnetTDESGetTTSS(void)
{
    printf("TEST_AmbaCSL_EnetTDESGetTTSS 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetTDESGetTTSS(&gEnetTDES);

    printf("TEST_AmbaCSL_EnetTDESGetTTSS done\n");
}

void TEST_AmbaCSL_EnetTDESSetCIC(void)
{
    printf("TEST_AmbaCSL_EnetTDESSetCIC 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetTDESSetCIC(&gEnetTDES, 0);

    printf("TEST_AmbaCSL_EnetTDESSetCIC done\n");
}

void TEST_AmbaCSL_EnetTDESSetDC(void)
{
    printf("TEST_AmbaCSL_EnetTDESSetDC 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetTDESSetDC(&gEnetTDES, 0);

    printf("TEST_AmbaCSL_EnetTDESSetDC done\n");
}

void TEST_AmbaCSL_EnetTDESSetLSFS(void)
{
    printf("TEST_AmbaCSL_EnetTDESSetLSFS 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetTDESSetLSFS(&gEnetTDES, 0, 0);

    printf("TEST_AmbaCSL_EnetTDESSetLSFS done\n");
}

void TEST_AmbaCSL_EnetTDESSetVLIC(void)
{
    printf("TEST_AmbaCSL_EnetTDESSetVLIC 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetTDESSetVLIC(&gEnetTDES, 0);

    printf("TEST_AmbaCSL_EnetTDESSetVLIC done\n");
}

void TEST_AmbaCSL_EnetTDES1Init(void)
{
    printf("TEST_AmbaCSL_EnetTDES1Init 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetTDES1Init(&gEnetTDES);

    printf("TEST_AmbaCSL_EnetTDES1Init done\n");
}

void TEST_AmbaCSL_EnetTDESSetTBS1(void)
{
    printf("TEST_AmbaCSL_EnetTDESSetTBS1 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetTDESSetTBS1(&gEnetTDES, 0);

    printf("TEST_AmbaCSL_EnetTDESSetTBS1 done\n");
}

void TEST_AmbaCSL_EnetTDES2Init(void)
{
    printf("TEST_AmbaCSL_EnetTDES2Init 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetTDES2Init(&gEnetTDES, 0);

    printf("TEST_AmbaCSL_EnetTDES2Init done\n");
}

void TEST_AmbaCSL_EnetTDES3Init(void)
{
    printf("TEST_AmbaCSL_EnetTDES3Init 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetTDES3Init(&gEnetTDES, 0);

    printf("TEST_AmbaCSL_EnetTDES3Init done\n");
}

void TEST_AmbaCSL_EnetRDES0Init(void)
{
    printf("TEST_AmbaCSL_EnetRDES0Init 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetRDES0Init(&gEnetRDES);

    printf("TEST_AmbaCSL_EnetRDES0Init done\n");
}

void TEST_AmbaCSL_EnetRDESSetDmaOwn(void)
{
    printf("TEST_AmbaCSL_EnetRDESSetDmaOwn 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetRDESSetDmaOwn(&gEnetRDES);

    printf("TEST_AmbaCSL_EnetRDESSetDmaOwn done\n");
}

void TEST_AmbaCSL_EnetRDESGetFL(void)
{
    printf("TEST_AmbaCSL_EnetRDESGetFL 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetRDESGetFL(&gEnetRDES);

    printf("TEST_AmbaCSL_EnetRDESGetFL done\n");
}

void TEST_AmbaCSL_EnetRDESGetDmaOwn(void)
{
    printf("TEST_AmbaCSL_EnetRDESGetDmaOwn 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetRDESGetDmaOwn(&gEnetRDES);

    printf("TEST_AmbaCSL_EnetRDESGetDmaOwn done\n");
}

void TEST_AmbaCSL_EnetRDES1Init(void)
{
    printf("TEST_AmbaCSL_EnetRDES1Init 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetRDES1Init(&gEnetRDES);

    printf("TEST_AmbaCSL_EnetRDES1Init done\n");
}

void TEST_AmbaCSL_EnetRDESSetRBS1(void)
{
    printf("TEST_AmbaCSL_EnetRDESSetRBS1 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetRDESSetRBS1(&gEnetRDES, 0);

    printf("TEST_AmbaCSL_EnetRDESSetRBS1 done\n");
}

void TEST_AmbaCSL_EnetRDES2Init(void)
{
    printf("TEST_AmbaCSL_EnetRDES2Init 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetRDES2Init(&gEnetRDES, 0);

    printf("TEST_AmbaCSL_EnetRDES2Init done\n");
}

void TEST_AmbaCSL_EnetRDES3Init(void)
{
    printf("TEST_AmbaCSL_EnetRDES3Init 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetRDES3Init(&gEnetRDES, 0);

    printf("TEST_AmbaCSL_EnetRDES3Init done\n");
}

void TEST_AmbaCSL_EnetMacLinkSetup(void)
{
    UINT32 SPEED, DUPLEX;

    printf("TEST_AmbaCSL_EnetMacLinkSetup 1\n");
    /* arrange */
    SPEED = SPEED_0;
    DUPLEX = DUPLEX_HALF;
    /* act */
    AmbaCSL_EnetMacLinkSetup(&gEnetReg, SPEED, DUPLEX);

    printf("TEST_AmbaCSL_EnetMacLinkSetup 2\n");
    /* arrange */
    SPEED = SPEED_10;
    DUPLEX = DUPLEX_HALF;
    /* act */
    AmbaCSL_EnetMacLinkSetup(&gEnetReg, SPEED, DUPLEX);

    printf("TEST_AmbaCSL_EnetMacLinkSetup 3\n");
    /* arrange */
    SPEED = SPEED_100;
    DUPLEX = DUPLEX_HALF;
    /* act */
    AmbaCSL_EnetMacLinkSetup(&gEnetReg, SPEED, DUPLEX);

    printf("TEST_AmbaCSL_EnetMacLinkSetup 4\n");
    /* arrange */
    SPEED = SPEED_1000;
    DUPLEX = DUPLEX_FULL;
    /* act */
    AmbaCSL_EnetMacLinkSetup(&gEnetReg, SPEED, DUPLEX);

    printf("TEST_AmbaCSL_EnetMacLinkSetup done\n");
}

void TEST_AmbaCSL_EnetMacSetJEJD(void)
{
    printf("TEST_AmbaCSL_EnetMacSetJEJD 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetJEJD(&gEnetReg, 0, 0);

    printf("TEST_AmbaCSL_EnetMacSetJEJD done\n");
}

void TEST_AmbaCSL_EnetMacTxDisable(void)
{
    printf("TEST_AmbaCSL_EnetMacTxDisable 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacTxDisable(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacTxDisable done\n");
}

void TEST_AmbaCSL_EnetMacRxDisable(void)
{
    printf("TEST_AmbaCSL_EnetMacRxDisable 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacRxDisable(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacRxDisable done\n");
}

void TEST_AmbaCSL_EnetMacInitR1(void)
{
    printf("TEST_AmbaCSL_EnetMacInitR1 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacInitR1(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacInitR1 done\n");
}

void TEST_AmbaCSL_EnetMacSetHMCPM(void)
{
    printf("TEST_AmbaCSL_EnetMacSetHMCPM 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetHMCPM(&gEnetReg, 0, 0);

    printf("TEST_AmbaCSL_EnetMacSetHMCPM done\n");
}

void TEST_AmbaCSL_EnetMacSetHashHi(void)
{
    printf("TEST_AmbaCSL_EnetMacSetHashHi 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetHashHi(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetHashHi done\n");
}

void TEST_AmbaCSL_EnetMacClearHashHi(void)
{
    printf("TEST_AmbaCSL_EnetMacClearHashHi 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacClearHashHi(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacClearHashHi done\n");
}

void TEST_AmbaCSL_EnetMacSetHashLo(void)
{
    printf("TEST_AmbaCSL_EnetMacSetHashLo 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetHashLo(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetHashLo done\n");
}

void TEST_AmbaCSL_EnetMacClearHashLo(void)
{
    printf("TEST_AmbaCSL_EnetMacClearHashLo 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacClearHashLo(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacClearHashLo done\n");
}

void TEST_AmbaCSL_EnetMacMiiWrite(void)
{
    printf("TEST_AmbaCSL_EnetMacMiiWrite 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacMiiWrite(&gEnetReg, 0, 0);

    printf("TEST_AmbaCSL_EnetMacMiiWrite done\n");
}

void TEST_AmbaCSL_EnetMacMiiRead(void)
{
    printf("TEST_AmbaCSL_EnetMacMiiRead 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacMiiRead(&gEnetReg, 0, 0);

    printf("TEST_AmbaCSL_EnetMacMiiRead done\n");
}

void TEST_AmbaCSL_EnetMacSetMiiData(void)
{
    printf("TEST_AmbaCSL_EnetMacSetMiiData 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetMiiData(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetMiiData done\n");
}

void TEST_AmbaCSL_EnetMacGetMiiData(void)
{
    printf("TEST_AmbaCSL_EnetMacGetMiiData 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacGetMiiData(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacGetMiiData done\n");
}

void TEST_AmbaCSL_EnetMacInitR6(void)
{
    printf("TEST_AmbaCSL_EnetMacInitR6 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacInitR6(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacInitR6 done\n");
}

void TEST_AmbaCSL_EnetMacGetMacR8(void)
{
    printf("TEST_AmbaCSL_EnetMacGetMacR8 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacGetMacR8(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacGetMacR8 done\n");
}

void TEST_AmbaCSL_EnetMacGetMacR14(void)
{
    printf("TEST_AmbaCSL_EnetMacGetMacR14 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacGetMacR14(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacGetMacR14 done\n");
}

void TEST_AmbaCSL_EnetMacInitR15(void)
{
    printf("TEST_AmbaCSL_EnetMacInitR15 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacInitR15(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacInitR15 done\n");
}

void TEST_AmbaCSL_EnetMacGetMacR15(void)
{
    printf("TEST_AmbaCSL_EnetMacGetMacR15 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacGetMacR15(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacGetMacR15 done\n");
}

void TEST_AmbaCSL_EnetMacSetTSIM(void)
{
    printf("TEST_AmbaCSL_EnetMacSetTSIM 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetTSIM(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetTSIM done\n");
}

void TEST_AmbaCSL_EnetMacSetAddrHi(void)
{
    printf("TEST_AmbaCSL_EnetMacSetAddrHi 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetAddrHi(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetAddrHi done\n");
}

void TEST_AmbaCSL_EnetMacGetAddrHi(void)
{
    printf("TEST_AmbaCSL_EnetMacGetAddrHi 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacGetAddrHi(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacGetAddrHi done\n");
}

void TEST_AmbaCSL_EnetMacSetAddrLo(void)
{
    printf("TEST_AmbaCSL_EnetMacSetAddrLo 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetAddrLo(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetAddrLo done\n");
}

void TEST_AmbaCSL_EnetMacGetAddrLo(void)
{
    printf("TEST_AmbaCSL_EnetMacGetAddrLo 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacGetAddrLo(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacGetAddrLo done\n");
}

void TEST_AmbaCSL_EnetMacGetMacR49(void)
{
    printf("TEST_AmbaCSL_EnetMacGetMacR49 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacGetMacR49(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacGetMacR49 done\n");
}

void TEST_AmbaCSL_EnetMacGetMacR54(void)
{
    printf("TEST_AmbaCSL_EnetMacGetMacR54 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacGetMacR54(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacGetMacR54 done\n");
}

void TEST_AmbaCSL_EnetMacGetMacR56(void)
{
    printf("TEST_AmbaCSL_EnetMacGetMacR56 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacGetMacR56(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacGetMacR56 done\n");
}

void TEST_AmbaCSL_EnetMacSetVLAN(void)
{
    printf("TEST_AmbaCSL_EnetMacSetVLAN 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetVLAN(&gEnetReg, 0, 0);

    printf("TEST_AmbaCSL_EnetMacSetVLAN done\n");
}

void TEST_AmbaCSL_EnetMacInitR448(void)
{
    printf("TEST_AmbaCSL_EnetMacInitR448 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacInitR448(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacInitR448 done\n");
}

void TEST_AmbaCSL_EnetMacSetTSADDREG(void)
{
    printf("TEST_AmbaCSL_EnetMacSetTSADDREG 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetTSADDREG(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacSetTSADDREG done\n");
}

void TEST_AmbaCSL_EnetMacTSINIT(void)
{
    printf("TEST_AmbaCSL_EnetMacTSINIT 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacTSINIT(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacTSINIT done\n");
}

void TEST_AmbaCSL_EnetMacTSUPDT(void)
{
    printf("TEST_AmbaCSL_EnetMacTSUPDT 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacTSUPDT(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacTSUPDT done\n");
}

void TEST_AmbaCSL_EnetMacSetTSTRIG(void)
{
    printf("TEST_AmbaCSL_EnetMacSetTSTRIG 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetTSTRIG(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetTSTRIG done\n");
}

void TEST_AmbaCSL_EnetMacSetSSINC(void)
{
    printf("TEST_AmbaCSL_EnetMacSetSSINC 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetSSINC(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetSSINC done\n");
}

void TEST_AmbaCSL_EnetMacGetPhcTsAddr(void)
{
    printf("TEST_AmbaCSL_EnetMacGetPhcTsAddr 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacGetPhcTsAddr(&gEnetReg, (volatile void **)&pAddr);

    printf("TEST_AmbaCSL_EnetMacGetPhcTsAddr done\n");
}

void TEST_AmbaCSL_EnetMacGetPhcSec(void)
{
    printf("TEST_AmbaCSL_EnetMacGetPhcSec 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacGetPhcSec(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacGetPhcSec done\n");
}

void TEST_AmbaCSL_EnetMacGetPhcNs(void)
{
    printf("TEST_AmbaCSL_EnetMacGetPhcNs 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacGetPhcNs(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacGetPhcNs done\n");
}

void TEST_AmbaCSL_EnetMacSetTSS(void)
{
    printf("TEST_AmbaCSL_EnetMacSetTSS 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetTSS(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetTSS done\n");
}

void TEST_AmbaCSL_EnetMacSetTSSS(void)
{
    printf("TEST_AmbaCSL_EnetMacSetTSSS 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetTSSS(&gEnetReg, 0, 0);

    printf("TEST_AmbaCSL_EnetMacSetTSSS done\n");
}

void TEST_AmbaCSL_EnetMacGetTSAR(void)
{
    printf("TEST_AmbaCSL_EnetMacGetTSAR 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacGetTSAR(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacGetTSAR done\n");
}

void TEST_AmbaCSL_EnetMacSetTSAR(void)
{
    printf("TEST_AmbaCSL_EnetMacSetTSAR 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetTSAR(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetTSAR done\n");
}

void TEST_AmbaCSL_EnetMacSetTSTR(void)
{
    printf("TEST_AmbaCSL_EnetMacSetTSTR 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetTSTR(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetTSTR done\n");
}

void TEST_AmbaCSL_EnetMacSetTTSLO(void)
{
    printf("TEST_AmbaCSL_EnetMacSetTTSLO 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetTTSLO(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetTTSLO done\n");
}

void TEST_AmbaCSL_EnetMacGetMacR458(void)
{
    printf("TEST_AmbaCSL_EnetMacGetMacR458 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacGetMacR458(&gEnetReg);

    printf("TEST_AmbaCSL_EnetMacGetMacR458 done\n");
}

void TEST_AmbaCSL_EnetMacSetPPSCTRL0(void)
{
    printf("TEST_AmbaCSL_EnetMacSetPPSCTRL0 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetPPSCTRL0(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetPPSCTRL0 done\n");
}

void TEST_AmbaCSL_EnetMacSetPPSCMD0(void)
{
    printf("TEST_AmbaCSL_EnetMacSetPPSCMD0 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetPPSCMD0(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetPPSCMD0 done\n");
}

void TEST_AmbaCSL_EnetMacSetTRGTMODSEL0(void)
{
    printf("TEST_AmbaCSL_EnetMacSetTRGTMODSEL0 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetTRGTMODSEL0(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetTRGTMODSEL0 done\n");
}

void TEST_AmbaCSL_EnetMacSetPPSINT(void)
{
    printf("TEST_AmbaCSL_EnetMacSetPPSINT 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetPPSINT(&gEnetReg, 0U);

    printf("TEST_AmbaCSL_EnetMacSetPPSINT done\n");
}

void TEST_AmbaCSL_EnetMacSetPPSWIDTH(void)
{
    printf("TEST_AmbaCSL_EnetMacSetPPSWIDTH 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetMacSetPPSWIDTH(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetMacSetPPSWIDTH done\n");
}

void TEST_AmbaCSL_EnetDmaReset(void)
{
    printf("TEST_AmbaCSL_EnetDmaReset 1\n");
    /* arrange */
    gEnetReg.DmaR0.SWR = 0x1U;
    /* act */
    AmbaCSL_EnetDmaReset(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaReset 2\n");
    /* arrange */
    gEnetReg.DmaR0.SWR = 0x0U;
    /* act */
    AmbaCSL_EnetDmaReset(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaReset done\n");
}

void TEST_AmbaCSL_EnetDmaInitR0(void)
{
    printf("TEST_AmbaCSL_EnetDmaInitR0 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaInitR0(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaInitR0 done\n");
}

void TEST_AmbaCSL_EnetDmaSetTxPoll(void)
{
    printf("TEST_AmbaCSL_EnetDmaSetTxPoll 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaSetTxPoll(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaSetTxPoll done\n");
}

void TEST_AmbaCSL_EnetDmaSetRxPoll(void)
{
    printf("TEST_AmbaCSL_EnetDmaSetRxPoll 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaSetRxPoll(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaSetRxPoll done\n");
}

void TEST_AmbaCSL_EnetDmaSetRxDES(void)
{
    printf("TEST_AmbaCSL_EnetDmaSetRxDES 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaSetRxDES(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetDmaSetRxDES done\n");
}

void TEST_AmbaCSL_EnetDmaSetTxDES(void)
{
    printf("TEST_AmbaCSL_EnetDmaSetTxDES 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaSetTxDES(&gEnetReg, 0);

    printf("TEST_AmbaCSL_EnetDmaSetTxDES done\n");
}

void TEST_AmbaCSL_EnetDmaGetStatus(void)
{
    printf("TEST_AmbaCSL_EnetDmaGetStatus 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaGetStatus(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaGetStatus done\n");
}

void TEST_AmbaCSL_EnetDmaClearStatus(void)
{
    printf("TEST_AmbaCSL_EnetDmaClearStatus 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaClearStatus(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaClearStatus done\n");
}

void TEST_AmbaCSL_EnetTDESDumpStatus(void)
{
    AMBA_REG_ENET_TDES_s RegEnetTDES = {0};

    printf("TEST_AmbaCSL_EnetTDESDumpStatus 1\n");
    /* arrange */
    RegEnetTDES.TDES0.ES = 0U;
    /* act */
    AmbaCSL_EnetTDESDumpStatus((const AMBA_ENET_TDES_s *)&RegEnetTDES, &gEnetStat);

    printf("TEST_AmbaCSL_EnetTDESDumpStatus 2\n");
    /* arrange */
    RegEnetTDES.TDES0.ES  = 1U;
    RegEnetTDES.TDES0.UF  = 1U;
    RegEnetTDES.TDES0.ED  = 1U;
    RegEnetTDES.TDES0.EC  = 1U;
    RegEnetTDES.TDES0.LC  = 1U;
    RegEnetTDES.TDES0.NC  = 1U;
    RegEnetTDES.TDES0.LOC = 1U;
    RegEnetTDES.TDES0.IPE = 1U;
    RegEnetTDES.TDES0.FF  = 1U;
    RegEnetTDES.TDES0.JT  = 1U;
    RegEnetTDES.TDES0.IHE = 1U;
    /* act */
    AmbaCSL_EnetTDESDumpStatus((const AMBA_ENET_TDES_s *)&RegEnetTDES, &gEnetStat);

    printf("TEST_AmbaCSL_EnetTDESDumpStatus done\n");
}

void TEST_AmbaCSL_EnetRDESDumpStatus(void)
{
    AMBA_REG_ENET_RDES_s RegEnetRDES = {0};

    printf("TEST_AmbaCSL_EnetRDESDumpStatus 1\n");
    /* arrange */
    RegEnetRDES.RDES0.ES = 0U;
    /* act */
    AmbaCSL_EnetRDESDumpStatus((const AMBA_ENET_RDES_s *)&RegEnetRDES, &gEnetStat);

    printf("TEST_AmbaCSL_EnetRDESDumpStatus 2\n");
    /* arrange */
    RegEnetRDES.RDES0.ES        = 1U;
    RegEnetRDES.RDES0.CE        = 1U;
    RegEnetRDES.RDES0.RE        = 1U;
    RegEnetRDES.RDES0.RWT       = 1U;
    RegEnetRDES.RDES0.LC        = 1U;
    RegEnetRDES.RDES0.Timestamp = 1U;
    RegEnetRDES.RDES0.OE        = 1U;
    RegEnetRDES.RDES0.DE        = 1U;
    RegEnetRDES.RDES4.IPHR      = 1U;
    RegEnetRDES.RDES4.IPPE      = 1U;
    /* act */
    AmbaCSL_EnetRDESDumpStatus((const AMBA_ENET_RDES_s *)&RegEnetRDES, &gEnetStat);

    printf("TEST_AmbaCSL_EnetRDESDumpStatus done\n");
}

void TEST_AmbaCSL_EnetDmaDumpStatus(void)
{
    AMBA_ENET_REG_s EnetReg = {0};

    printf("TEST_AmbaCSL_EnetDmaDumpStatus 1\n");
    /* arrange */
    EnetReg.DmaR5.NIS = 1U;
    EnetReg.DmaR5.TI = 1U;
    EnetReg.DmaR5.RI = 1U;
    EnetReg.DmaR5.ERI = 1U;
    EnetReg.DmaR5.AIS = 1U;
    EnetReg.DmaR5.TPS = 1U;
    EnetReg.DmaR5.TJT = 1U;
    EnetReg.DmaR5.OVF = 1U;
    EnetReg.DmaR5.UNF = 1U;
    EnetReg.DmaR5.RU = 1U;
    EnetReg.DmaR5.RPS = 1U;
    EnetReg.DmaR5.RWT = 1U;
    EnetReg.DmaR5.ETI = 1U;
    EnetReg.DmaR5.FBI = 1U;
    EnetReg.DmaR5.GMI = 1U;
    EnetReg.DmaR5.GPI = 1U;
    EnetReg.DmaR5.GLI = 1U;
    EnetReg.DmaR5.TTI = 1U;
    /* act */
    AmbaCSL_EnetDmaDumpStatus(&EnetReg, &gEnetStat);

    printf("TEST_AmbaCSL_EnetDmaDumpStatus 2\n");
    /* arrange */
    EnetReg.DmaR5.NIS = 1U;
    EnetReg.DmaR5.TI = 0U;
    EnetReg.DmaR5.TU = 1U;
    /* act */
    AmbaCSL_EnetDmaDumpStatus(&EnetReg, &gEnetStat);

    printf("TEST_AmbaCSL_EnetDmaDumpStatus done\n");
}

void TEST_AmbaCSL_EnetDmaGetTI(void)
{
    printf("TEST_AmbaCSL_EnetDmaGetTI 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaGetTI(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaGetTI done\n");
}

void TEST_AmbaCSL_EnetDmaGetTU(void)
{
    printf("TEST_AmbaCSL_EnetDmaGetTU 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaGetTU(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaGetTU done\n");
}

void TEST_AmbaCSL_EnetDmaGetRI(void)
{
    printf("TEST_AmbaCSL_EnetDmaGetRI 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaGetRI(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaGetRI done\n");
}

void TEST_AmbaCSL_EnetDmaGetRU(void)
{
    printf("TEST_AmbaCSL_EnetDmaGetRU 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaGetRU(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaGetRU done\n");
}

void TEST_AmbaCSL_EnetDmaGetGLI(void)
{
    printf("TEST_AmbaCSL_EnetDmaGetGLI 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaGetGLI(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaGetGLI done\n");
}

void TEST_AmbaCSL_EnetDmaGetTTI(void)
{
    printf("TEST_AmbaCSL_EnetDmaGetTTI 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaGetTTI(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaGetTTI done\n");
}

void TEST_AmbaCSL_EnetDmaStartTX(void)
{
    printf("TEST_AmbaCSL_EnetDmaStartTX 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaStartTX(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaStartTX done\n");
}

void TEST_AmbaCSL_EnetDmaStartRX(void)
{
    printf("TEST_AmbaCSL_EnetDmaStartRX 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaStartRX(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaStartRX done\n");
}

void TEST_AmbaCSL_EnetDmaStopTX(void)
{
    printf("TEST_AmbaCSL_EnetDmaStopTX 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaStopTX(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaStopTX done\n");
}

void TEST_AmbaCSL_EnetDmaStopRX(void)
{
    printf("TEST_AmbaCSL_EnetDmaStopRX 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaStopRX(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaStopRX done\n");
}

void TEST_AmbaCSL_EnetDmaInitR6(void)
{
    printf("TEST_AmbaCSL_EnetDmaInitR6 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaInitR6(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaInitR6 done\n");
}

void TEST_AmbaCSL_EnetDmaInitR7(void)
{
    printf("TEST_AmbaCSL_EnetDmaInitR7 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaInitR7(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaInitR7 done\n");
}

void TEST_AmbaCSL_EnetDmaTxIrqDisable(void)
{
    printf("TEST_AmbaCSL_EnetDmaTxIrqDisable 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaTxIrqDisable(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaTxIrqDisable done\n");
}

void TEST_AmbaCSL_EnetDmaTxIrqEnable(void)
{
    printf("TEST_AmbaCSL_EnetDmaTxIrqEnable 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaTxIrqEnable(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaTxIrqEnable done\n");
}

void TEST_AmbaCSL_EnetDmaRxIrqDisable(void)
{
    printf("TEST_AmbaCSL_EnetDmaRxIrqDisable 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaRxIrqDisable(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaRxIrqDisable done\n");
}

void TEST_AmbaCSL_EnetDmaRxIrqEnable(void)
{
    printf("TEST_AmbaCSL_EnetDmaRxIrqEnable 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaRxIrqEnable(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaRxIrqEnable done\n");
}

void TEST_AmbaCSL_EnetDmaInitR10(void)
{
    printf("TEST_AmbaCSL_EnetDmaInitR10 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaInitR10(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaInitR10 done\n");
}

void TEST_AmbaCSL_EnetDmaGetDmaCurTxDes(void)
{
    printf("TEST_AmbaCSL_EnetDmaGetDmaCurTxDes 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaGetDmaCurTxDes(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaGetDmaCurTxDes done\n");
}

void TEST_AmbaCSL_EnetDmaGetDmaCurRxDes(void)
{
    printf("TEST_AmbaCSL_EnetDmaGetDmaCurRxDes 1\n");
    /* arrange */
    /* act */
    AmbaCSL_EnetDmaGetDmaCurRxDes(&gEnetReg);

    printf("TEST_AmbaCSL_EnetDmaGetDmaCurRxDes done\n");
}

int main(void)
{
    TEST_WriteLongEnet();
    TEST_AmbaCSL_EnetTDES0Init();
    TEST_AmbaCSL_EnetTDESSetDmaOwn();
    TEST_AmbaCSL_EnetTDESGetDmaOwn();
    TEST_AmbaCSL_EnetTDESGetDmaES();
    TEST_AmbaCSL_EnetTDESGetTTSS();
    TEST_AmbaCSL_EnetTDESSetCIC();
    TEST_AmbaCSL_EnetTDESSetDC();
    TEST_AmbaCSL_EnetTDESSetLSFS();
    TEST_AmbaCSL_EnetTDESSetVLIC();
    TEST_AmbaCSL_EnetTDES1Init();
    TEST_AmbaCSL_EnetTDESSetTBS1();
    TEST_AmbaCSL_EnetTDES2Init();
    TEST_AmbaCSL_EnetTDES3Init();
    TEST_AmbaCSL_EnetRDES0Init();
    TEST_AmbaCSL_EnetRDESSetDmaOwn();
    TEST_AmbaCSL_EnetRDESGetFL();
    TEST_AmbaCSL_EnetRDESGetDmaOwn();
    TEST_AmbaCSL_EnetRDES1Init();
    TEST_AmbaCSL_EnetRDESSetRBS1();
    TEST_AmbaCSL_EnetRDES2Init();
    TEST_AmbaCSL_EnetRDES3Init();
    TEST_AmbaCSL_EnetMacLinkSetup();
    TEST_AmbaCSL_EnetMacSetJEJD();
    TEST_AmbaCSL_EnetMacTxDisable();
    TEST_AmbaCSL_EnetMacRxDisable();
    TEST_AmbaCSL_EnetMacInitR1();
    TEST_AmbaCSL_EnetMacSetHMCPM();
    TEST_AmbaCSL_EnetMacSetHashHi();
    TEST_AmbaCSL_EnetMacClearHashHi();
    TEST_AmbaCSL_EnetMacSetHashLo();
    TEST_AmbaCSL_EnetMacClearHashLo();
    TEST_AmbaCSL_EnetMacMiiWrite();
    TEST_AmbaCSL_EnetMacMiiRead();
    TEST_AmbaCSL_EnetMacSetMiiData();
    TEST_AmbaCSL_EnetMacGetMiiData();
    TEST_AmbaCSL_EnetMacInitR6();
    TEST_AmbaCSL_EnetMacGetMacR8();
    TEST_AmbaCSL_EnetMacGetMacR14();
    TEST_AmbaCSL_EnetMacInitR15();
    TEST_AmbaCSL_EnetMacGetMacR15();
    TEST_AmbaCSL_EnetMacSetTSIM();
    TEST_AmbaCSL_EnetMacSetAddrHi();
    TEST_AmbaCSL_EnetMacGetAddrHi();
    TEST_AmbaCSL_EnetMacSetAddrLo();
    TEST_AmbaCSL_EnetMacGetAddrLo();
    TEST_AmbaCSL_EnetMacGetMacR49();
    TEST_AmbaCSL_EnetMacGetMacR54();
    TEST_AmbaCSL_EnetMacGetMacR56();
    TEST_AmbaCSL_EnetMacSetVLAN();
    TEST_AmbaCSL_EnetMacInitR448();
    TEST_AmbaCSL_EnetMacSetTSADDREG();
    TEST_AmbaCSL_EnetMacTSINIT();
    TEST_AmbaCSL_EnetMacTSUPDT();
    TEST_AmbaCSL_EnetMacSetTSTRIG();
    TEST_AmbaCSL_EnetMacSetSSINC();
    TEST_AmbaCSL_EnetMacGetPhcTsAddr();
    TEST_AmbaCSL_EnetMacGetPhcSec();
    TEST_AmbaCSL_EnetMacGetPhcNs();
    TEST_AmbaCSL_EnetMacSetTSS();
    TEST_AmbaCSL_EnetMacSetTSSS();
    TEST_AmbaCSL_EnetMacGetTSAR();
    TEST_AmbaCSL_EnetMacSetTSAR();
    TEST_AmbaCSL_EnetMacSetTSTR();
    TEST_AmbaCSL_EnetMacSetTTSLO();
    TEST_AmbaCSL_EnetMacGetMacR458();
    TEST_AmbaCSL_EnetMacSetPPSCTRL0();
    TEST_AmbaCSL_EnetMacSetPPSCMD0();
    TEST_AmbaCSL_EnetMacSetTRGTMODSEL0();
    TEST_AmbaCSL_EnetMacSetPPSINT();
    TEST_AmbaCSL_EnetMacSetPPSWIDTH();
    TEST_AmbaCSL_EnetDmaReset();
    TEST_AmbaCSL_EnetDmaInitR0();
    TEST_AmbaCSL_EnetDmaSetTxPoll();
    TEST_AmbaCSL_EnetDmaSetRxPoll();
    TEST_AmbaCSL_EnetDmaSetRxDES();
    TEST_AmbaCSL_EnetDmaSetTxDES();
    TEST_AmbaCSL_EnetDmaGetStatus();
    TEST_AmbaCSL_EnetDmaClearStatus();
    TEST_AmbaCSL_EnetTDESDumpStatus();
    TEST_AmbaCSL_EnetRDESDumpStatus();
    TEST_AmbaCSL_EnetDmaDumpStatus();
    TEST_AmbaCSL_EnetDmaGetTI();
    TEST_AmbaCSL_EnetDmaGetTU();
    TEST_AmbaCSL_EnetDmaGetRI();
    TEST_AmbaCSL_EnetDmaGetRU();
    TEST_AmbaCSL_EnetDmaGetGLI();
    TEST_AmbaCSL_EnetDmaGetTTI();
    TEST_AmbaCSL_EnetDmaStartTX();
    TEST_AmbaCSL_EnetDmaStartRX();
    TEST_AmbaCSL_EnetDmaStopTX();
    TEST_AmbaCSL_EnetDmaStopRX();
    TEST_AmbaCSL_EnetDmaInitR6();
    TEST_AmbaCSL_EnetDmaInitR7();
    TEST_AmbaCSL_EnetDmaTxIrqDisable();
    TEST_AmbaCSL_EnetDmaTxIrqEnable();
    TEST_AmbaCSL_EnetDmaRxIrqDisable();
    TEST_AmbaCSL_EnetDmaRxIrqEnable();
    TEST_AmbaCSL_EnetDmaInitR10();
    TEST_AmbaCSL_EnetDmaGetDmaCurTxDes();
    TEST_AmbaCSL_EnetDmaGetDmaCurRxDes();

    return 0;
}
