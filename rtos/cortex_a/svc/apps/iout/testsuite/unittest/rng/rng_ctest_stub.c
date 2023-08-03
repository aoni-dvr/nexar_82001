#include "rng_ctest.h"


void IO_UtilityRegWrite32(UINT64 Address, UINT32 Value)
{

}

UINT32 IO_UtilityRegRead32(UINT64 Address)
{
    static UINT32 value = 0;
    return (++value) % 2; // for AmbaCSL_RngIsBusy() to change done value;
}


void AmbaDelayCycles(UINT32 Delay)
{

}

void AmbaMisra_TouchUnused(void * pArg)
{

}

