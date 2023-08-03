#include "AmbaTypes.h"
#include "AmbaENET_Def.h"
#include "AmbaReg_ENET.h"
#include "AmbaRTSL_ENET.h"
#include "AmbaKAL.h"

static UINT32 RetVal_AmbaRTSL_EnetSetRxIsrFunc = KAL_ERR_NONE;
static UINT32 RetVal_AmbaRTSL_EnetGetRegIdx    = KAL_ERR_NONE;
static UINT32 RetVal_AmbaRTSL_EnetGetPhyId     = KAL_ERR_NONE;
static UINT32 RetVal_AmbaRTSL_EnetMiiRead      = KAL_ERR_NONE;
static UINT32 RetVal_AmbaRTSL_EnetMiiWrite     = KAL_ERR_NONE;
static UINT32 RetVal_AmbaRTSL_EnetSetLcFunc    = KAL_ERR_NONE;
static UINT32 RetVal_AmbaRTSL_EnetGetPhyAddr   = KAL_ERR_NONE;

void (*fpCB_EnetGetPhyAddr)(void);

void Set_RetVal_AmbaRTSL_EnetSetRxIsrFunc(UINT32 RetVal)
{
    RetVal_AmbaRTSL_EnetSetRxIsrFunc = RetVal;
}
void Set_RetVal_AmbaRTSL_EnetGetRegIdx(UINT32 RetVal)
{
    RetVal_AmbaRTSL_EnetGetRegIdx = RetVal;
}
void Set_RetVal_AmbaRTSL_EnetGetPhyId(UINT32 RetVal)
{
    RetVal_AmbaRTSL_EnetGetPhyId = RetVal;
}
void Set_RetVal_AmbaRTSL_EnetMiiRead(UINT32 RetVal)
{
    RetVal_AmbaRTSL_EnetMiiRead = RetVal;
}
void Set_RetVal_AmbaRTSL_EnetMiiWrite(UINT32 RetVal)
{
    RetVal_AmbaRTSL_EnetMiiWrite = RetVal;
}
void Set_RetVal_AmbaRTSL_EnetSetLcFunc(UINT32 RetVal)
{
    RetVal_AmbaRTSL_EnetSetLcFunc = RetVal;
}
void Set_RetVal_AmbaRTSL_EnetGetPhyAddr(UINT32 RetVal)
{
    RetVal_AmbaRTSL_EnetGetPhyAddr = RetVal;
}

void AmbaRTSL_EnetInit(AMBA_ENET_REG_s *const pEnetReg)
{

}

UINT32 AmbaRTSL_EnetSetRxIsrFunc(AMBA_RTSL_ENET_RXISRCB_f pISR)
{
    return RetVal_AmbaRTSL_EnetSetRxIsrFunc;
}

UINT32 AmbaRTSL_EnetGetRegIdx(const AMBA_ENET_REG_s *const pEnetReg)
{
    return RetVal_AmbaRTSL_EnetGetRegIdx;
}
UINT32 AmbaRTSL_EnetGetPhyId(AMBA_ENET_REG_s *const pEnetReg, UINT8 PhyAddr)
{
    return RetVal_AmbaRTSL_EnetGetPhyId;
}

void AmbaRTSL_GetLink(AMBA_ENET_REG_s *const pEnetReg, UINT8 PhyAddr, UINT32 *Speed, UINT32 *Duplex)
{

}

UINT16 AmbaRTSL_EnetMiiRead(AMBA_ENET_REG_s *const pEnetReg, UINT8 addr, UINT8 reg)
{
    return RetVal_AmbaRTSL_EnetMiiRead;
}

UINT16 AmbaRTSL_EnetMiiWrite(AMBA_ENET_REG_s *const pEnetReg, UINT8 addr, UINT8 regnum, UINT16 value)
{
    return RetVal_AmbaRTSL_EnetMiiWrite;
}

UINT32 AmbaRTSL_EnetSetLcFunc(AMBA_RTSL_ENET_LINKCB_f pISR)
{
    return RetVal_AmbaRTSL_EnetSetLcFunc;
}

UINT8 AmbaRTSL_EnetGetPhyAddr(AMBA_ENET_REG_s *const pEnetReg)
{
    UINT8 Ret = RetVal_AmbaRTSL_EnetGetPhyAddr;
    if (fpCB_EnetGetPhyAddr != NULL) {
        fpCB_EnetGetPhyAddr();
    }
    return Ret;
}
