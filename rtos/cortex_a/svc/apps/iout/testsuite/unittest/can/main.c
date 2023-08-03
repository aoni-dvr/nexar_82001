#include "can_ctest.h"

#define IoDiagCanMaxBufSize      4096
#define IoDiagCanFDMaxBufSize    4096

static UINT8 IoDiagCanRingBuf[IoDiagCanMaxBufSize] __attribute__ ((aligned(AMBA_CACHE_LINE_SIZE))) __attribute__((section(".bss.noinit")));
static UINT8 IoDiagCanFDRingBuf[IoDiagCanFDMaxBufSize] __attribute__ ((aligned(AMBA_CACHE_LINE_SIZE))) __attribute__((section(".bss.noinit")));

static AMBA_CAN_FILTER_s AmbaShellCan_Filter[2] = {
    [0] = {
        .IdFilter = 0x1fffffffU,
        .IdMask = 0x1fffffffU,
    },

    [1] = {
        .IdFilter = 0x7ffU,
        .IdMask = 0x7ffU,
    },

};

static void IoDiag_CanTxComplCb(UINT32 CanCh, UINT32 TrackID)
{
    static UINT32 IoDiag_CanTxComplCounter = 0;
    AmbaPrint_PrintUInt5("CH:[%d] Tx Complete [%d], Track ID = 0x%x", CanCh, IoDiag_CanTxComplCounter, TrackID, 0, 0);
    IoDiag_CanTxComplCounter++;
}

/* Common CANC and CAN 2.0 configurations */
AMBA_SHELL_CAN_CTRL_s IoDiag_CanCtrl = {
    //.ChannelId = AMBA_CAN_CHANNEL0,
    .Config = {                                     /* Common CANC and CAN 2.0 configurations */
        .OpMode             = 0,                    /* 0: Normal mode; 1: Inside-loopback mode; 2: Outside-loopback mode; 3: Listen Mode */
        .EnableDMA          = 1,
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        .EnableAa           = 0,                    /* Auto Answer Mode */
#endif

        .TimeQuanta = {                             /* Bit-rate = RefClock / (( 1 + (PropSeg + PhaseSeg1 + 1) + (PhaseSeg2 + 1)) * (Prescaler + 1)) */
            .PhaseSeg2      = 1,                    /* E.g., To enable 1M bps transfer, it might set PropSeg = 0, Seg2 = 1, Seg1 = 4, SJW = 0, and  */
            .PhaseSeg1      = 4,                    /*       BRP = 2. Then, the bit-rate is 24M/((1 + 2 + 5) * 3) = 1M bps.      */
            .SJW            = 0,
            .BRP            = 2,
            .PropSeg        = 0,
        },

        .MaxRxRingBufSize   = IoDiagCanMaxBufSize,   /* maximum receive ring-buffer size in bytes */
        .pRxRingBuf         = IoDiagCanRingBuf,      /* Pointer to the Rx ring buffer */

        .FdTimeQuanta       = {
            .PhaseSeg2      = 1,                     /* E.g., To enable 1M bps transfer, it might set PropSeg = 0, Seg2 = 1, Seg1 = 2, SJW = 0, and  */
            .PhaseSeg1      = 2,                     /*       BRP = 2. Then, the bit-rate is 24M/((1 + 2 + 3) * 2) = 2M bps.      */
            .SJW            = 0,
            .BRP            = 1,
            .PropSeg        = 0,
        },
        .FdSsp              = {
            .TdcEnable      = 1,                     /* Tx delay compensation enable */
            .Mode           = 0,                     /* 0: Delay measurement mode. 1: Fixed value mode */
            .Offset         = 1,                     /* SSP offset (Delay measurement mode) in reference clock period*/
            .Position       = 0,                     /* SSP position (Fixed value mode) in minimum time quanta */
        },

        .FdMaxRxRingBufSize = IoDiagCanFDMaxBufSize,  /* maximum receive ring-buffer size in bytes for CAN FD */
        .pFdRxRingBuf       = IoDiagCanFDRingBuf,     /* Pointer to the Rx ring buffer for CAN FD */

        .NumIdFilter        = 2,                      /* Number of ID filters */
        .pIdFilter          = &AmbaShellCan_Filter,  /* Pointer to the ID filters */
        .TxComplFunc        = IoDiag_CanTxComplCb,    /* Pointer to tx complete function */

    },
};

void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    printf(pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);

    return;
}

INT32 main(void)
{
    can_ctest_enable_main();
    can_ctest_getinfo_main();
    can_ctest_read_main();
    can_ctest_write_main();
    can_ctest_writefd_main();
    can_ctest_txcallback_main();
    can_ctest_disable_main();
    can_ctest_isr_main();
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    can_ctest_getsafestate_main();
    can_ctest_setsafestate_main();
#endif
    return 0;
}

