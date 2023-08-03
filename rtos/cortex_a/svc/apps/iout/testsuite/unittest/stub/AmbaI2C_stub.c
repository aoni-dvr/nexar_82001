#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaI2C.h"

UINT32 AmbaI2C_MasterReadAfterWrite(UINT32 MasterID, UINT32 BusSpeed, UINT32 NumTxTransaction, const AMBA_I2C_TRANSACTION_s * pTxTransaction, const AMBA_I2C_TRANSACTION_s * pRxTransaction, UINT32 * pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = I2C_ERR_NONE;

    return RetVal;
}

UINT32 AmbaI2C_MasterWrite(UINT32 MasterID, UINT32 BusSpeed, const AMBA_I2C_TRANSACTION_s *pTxTransaction, UINT32 *pActualTxSize, UINT32 TimeOut)
{
    UINT32 RetVal = I2C_ERR_NONE;

    return RetVal;
}


