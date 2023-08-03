#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaDMA.h"

static UINT32 RetVal_ChannelAllocate = 0;
static UINT32 RetVal_ChannelRelease = 0;
static UINT32 RetVal_Transfer = 0;
static UINT32 RetVal_Wait = 0;
static UINT32 RetVal_DmaStop = 0;
static UINT32 RetVal_DmaGetTrfByteCount = 0;
static UINT32 RetVal_DmaChanNo = 0;

void Set_RetVal_DmaChanNo(UINT32 RetVal)
{
    RetVal_DmaChanNo = RetVal;
}

void Set_RetVal_ChannelAllocate(UINT32 RetVal)
{
    RetVal_ChannelAllocate = RetVal;
}

void Set_RetVal_ChannelRelease(UINT32 RetVal)
{
    RetVal_ChannelRelease = RetVal;
}

void Set_RetVal_Transfer(UINT32 RetVal)
{
    RetVal_Transfer = RetVal;
}

void Set_RetVal_Wait(UINT32 RetVal)
{
    RetVal_Wait = RetVal;
}

void Set_RetVal_DmaStop(UINT32 RetVal)
{
    RetVal_DmaStop = RetVal;
}

void Set_RetVal_DmaGetTrfByteCount(UINT32 RetVal)
{
    RetVal_DmaGetTrfByteCount = RetVal;
}

UINT32 AmbaDMA_ChannelAllocate(UINT32 DmaChanType, UINT32 *pDmaChanNo)
{
    *pDmaChanNo = RetVal_DmaChanNo;
    return RetVal_ChannelAllocate;
}

UINT32 AmbaDMA_ChannelRelease(UINT32 DmaChanNo)
{
    return RetVal_ChannelRelease;
}

UINT32 AmbaDMA_Transfer(UINT32 DmaChanNo, AMBA_DMA_DESC_s *pDmaDesc)
{
    return RetVal_Transfer;
}

UINT32 AmbaDMA_Wait(UINT32 DmaChanNo, UINT32 TimeOut)
{
    return RetVal_Wait;
}

UINT32 AmbaRTSL_DmaStop(UINT32 DmaChanNo)
{
    return RetVal_DmaStop;
}

UINT32 AmbaRTSL_DmaGetTrfByteCount(UINT32 DmaChanNo, UINT32 *pActualSize)
{
    return RetVal_DmaGetTrfByteCount;
}

