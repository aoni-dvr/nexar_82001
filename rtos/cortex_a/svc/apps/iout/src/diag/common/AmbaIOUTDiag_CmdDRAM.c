/**
 *  @file AmbaIOUTDiag_CmdDRAM.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details DRAM diagnostic command.
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaDef.h"

#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"

#include "AmbaIOUTDiag.h"
#include "AmbaCSL_RNG.h"

#define TASK_NUM        2U
#define TASK_NAME_LEN   16U

typedef struct {
    UINT8               Stack[TASK_NUM][4096];
    AMBA_KAL_TASK_t     Task[TASK_NUM];
    char                TaskName[TASK_NUM][TASK_NAME_LEN];
    ULONG               MemAddr[TASK_NUM];
    UINT32              MemSize[TASK_NUM];
    UINT32              LoopCount[TASK_NUM];
    UINT32              Silence[TASK_NUM];
    UINT32              TaskCreated[TASK_NUM];
    UINT32              TaskRunning[TASK_NUM];
} AMBA_DIAG_MEM_TASK_s;

typedef struct {
    const char *pName;
    UINT32 (*pFunc)(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count);
} AMBT_DIAG_MEM_TEST_s;

static AMBA_SHELL_PRINT_f DiagDramPrintFunc = NULL;
static AMBA_DIAG_MEM_TASK_s DiagDramTask GNU_ALIGNED_CACHESAFE = {0};

static UINT32 IoDiag_DramGetRandomVal(void)
{
    UINT32 Val;

#if 0 /* PRNG solution */
    (void)AmbaWrap_rand(&Val);
#else /* HRNG solution */
    AmbaCSL_RngEnable();
    AmbaCSL_RngStart();

    AmbaDelayCycles(24000U);

    Val = AmbaCSL_RngGetData0();
#endif

    return Val;
}

static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    PrintFunc(StrBuf);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramCompare
 *
 *  @Description:: Compare memory content
 *
 *  @Input      ::
 *      pBufA:  start address of memory region
 *      pBufB:  start address of memory region
 *      Count:  size of memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramCompare(const UINT32 *pBufA, const UINT32 *pBufB, UINT32 Count)
{
    UINT32 i, RetVal = OK;

    for (i = 0; i < Count; i++) {
        if (*pBufA != *pBufB) {
            IoDiag_PrintFormattedInt("0x%08x !=", *pBufA, DiagDramPrintFunc);
            IoDiag_PrintFormattedInt(" 0x%08x", *pBufB, DiagDramPrintFunc);
            IoDiag_PrintFormattedInt(" at (0x%08x) ", i*4, DiagDramPrintFunc);
            RetVal = ~OK;
            break;
        }
        pBufA++;
        pBufB++;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramStuckAddrTest
 *
 *  @Description:: Test with Stuck Address values
 *
 *  @Input      ::
 *      pBuf:   start address of memory region
 *      Count:  size of memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramStuckAddrTest(UINT32 *pBuf, UINT32 Count)
{
    UINT32 *p1, Addr;
    UINT32 i, j, RetVal = OK;

    for (j = 0U; j < 16U; j++) {
        p1 = pBuf;
        for (i = 0U; i < Count; i++) {
            AmbaMisra_TypeCast32(&Addr, &p1);
            if (((j + i) % 2U) == 0U) {
                *p1 = Addr;
            } else {
                *p1 = ~Addr;
            }
            p1++;
        }

        p1 = pBuf;
        for (i = 0; i < Count; i++) {
            AmbaMisra_TypeCast32(&Addr, &p1);
            if (((j + i) % 2U) == 0U) {
                if (*p1 != Addr) {
                    RetVal = ~OK;
                }
            } else {
                if (*p1 != ~Addr) {
                    RetVal = ~OK;
                }
            }

            if (RetVal != OK) {
                IoDiag_PrintFormattedInt("0x%08x !=", *p1, DiagDramPrintFunc);
                IoDiag_PrintFormattedInt(" 0x%08x", (((j + i) % 2U) == 0U) ? Addr : ~Addr, DiagDramPrintFunc);
                IoDiag_PrintFormattedInt(" at (0x%08x) ", i*4, DiagDramPrintFunc);
                break;
            }

            p1++;
        }
        if (RetVal != OK) {
            break;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramRandomValTest
 *
 *  @Description:: Test with Random values
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramRandomValTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 Rand, *p1 = pBufA, *p2 = pBufB;
    UINT32 i, RetVal;

    for (i = 0U; i < Count; i++) {
        Rand = IoDiag_DramGetRandomVal();
        *p1 = Rand;
        *p2 = Rand;
        p1++;
        p2++;
    }

    RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramXorValTest
 *
 *  @Description:: Test with XORed values
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramXorValTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1 = pBufA, *p2 = pBufB, Rand;
    UINT32 i, RetVal;

    Rand = IoDiag_DramGetRandomVal();
    for (i = 0U; i < Count; i++) {
        *p1 ^= Rand;
        *p2 ^= Rand;
        p1++;
        p2++;
    }

    RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramSubValTest
 *
 *  @Description:: Test with SUBed values
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramSubValTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1 = pBufA, *p2 = pBufB, Rand;
    UINT32 i, RetVal;

    Rand = IoDiag_DramGetRandomVal();
    for (i = 0U; i < Count; i++) {
        *p1 -= Rand;
        *p2 -= Rand;
        p1++;
        p2++;
    }

    RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramMulValTest
 *
 *  @Description:: Test with MULed values
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramMulValTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1 = pBufA, *p2 = pBufB, Rand;
    UINT32 i, RetVal;

    Rand = IoDiag_DramGetRandomVal();
    for (i = 0U; i < Count; i++) {
        *p1 *= Rand;
        *p2 *= Rand;
        p1++;
        p2++;
    }

    RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramDivValTest
 *
 *  @Description:: Test with DIVed values
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramDivValTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1 = pBufA, *p2 = pBufB, Rand;
    UINT32 i, RetVal;

    Rand = IoDiag_DramGetRandomVal();
    for (i = 0U; i < Count; i++) {
        if (Rand == 0U) {
            Rand++;
        }
        *p1 /= Rand;
        *p2 /= Rand;
        p1++;
        p2++;
    }

    RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramOrValTest
 *
 *  @Description:: Test with ORed values
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramOrValTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1 = pBufA, *p2 = pBufB, Rand;
    UINT32 i, RetVal;

    Rand = IoDiag_DramGetRandomVal();
    for (i = 0U; i < Count; i++) {
        *p1 |= Rand;
        *p2 |= Rand;
        p1++;
        p2++;
    }

    RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramAndValTest
 *
 *  @Description:: Test with ANDed values
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramAndValTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1 = pBufA, *p2 = pBufB, Rand;
    UINT32 i, RetVal;

    Rand = IoDiag_DramGetRandomVal();
    for (i = 0U; i < Count; i++) {
        *p1 &= Rand;
        *p2 &= Rand;
        p1++;
        p2++;
    }

    RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramSeqIncValTest
 *
 *  @Description:: Test with Sequential Increment values
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramSeqIncValTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1 = pBufA, *p2 = pBufB, Rand;
    UINT32 i, RetVal;

    Rand = IoDiag_DramGetRandomVal();
    for (i = 0U; i < Count; i++) {
        *p1 = (i + Rand);
        *p2 = (i + Rand);
        p1++;
        p2++;
    }

    RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramSolidBitsTest
 *
 *  @Description:: Test with Solid Bits pattern
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramSolidBitsTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1, *p2, q = 0U, r;
    UINT32 i, j, RetVal = OK;

    for (j = 0U; j < 64U; j++) {
        q = ~q;
        p1 = pBufA;
        p2 = pBufB;
        r = q;
        for (i = 0; i < Count; i++) {
            *p1 = r;
            *p2 = r;
            p1++;
            p2++;
            r = ~r;
        }

        RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);
        if (RetVal != OK) {
            break;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramZerosOnesTest
 *
 *  @Description:: Test with Zeros Ones pattern
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramZerosOnesTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1, *p2, q = 0U, r = 0xffffffffU;
    UINT32 i, j, RetVal = OK;

    for (j = 0U; j < 256U; j++) {
        p1 = pBufA;
        p2 = pBufB;
        for (i = 0; (i + 8) < Count; i+=8) {
            // 16 0x00 bytes
            *p1++ = *p2++ = q;
            *p1++ = *p2++ = q;
            *p1++ = *p2++ = q;
            *p1++ = *p2++ = q;
            // 16 0xff bytes
            *p1++ = *p2++ = r;
            *p1++ = *p2++ = r;
            *p1++ = *p2++ = r;
            *p1++ = *p2++ = r;
        }
        // Set remaining elements to 0
        for (; i < Count; i++) {
            *p1++ = *p2++ = q;
        }

        RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);
        if (RetVal != OK) {
            break;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramCheckerboardTest
 *
 *  @Description:: Test with Checkerboard pattern
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramCheckerboardTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1, *p2, q = 0xaaaaaaaaU, r;
    UINT32 i, j, RetVal = OK;

    for (j = 0U; j < 64U; j++) {
        q = ~q;
        p1 = pBufA;
        p2 = pBufB;
        r = q;
        for (i = 0U; i < Count; i++) {
            *p1 = r;
            *p2 = r;
            p1++;
            p2++;
            r = ~r;
        }

        RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);
        if (RetVal != OK) {
            break;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramBlockSeqTest
 *
 *  @Description:: Test with Block Sequential values
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramBlockSeqTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1, *p2, q;
    UINT32 i, j, RetVal = OK;

    for (j = 0U; j < 256U; j++) {
        p1 = pBufA;
        p2 = pBufB;
        q = (UINT32) ((j) | (j << 8U) | (j << 16U) | (j << 24U));
        for (i = 0U; i < Count; i++) {
            *p1 = q;
            *p2 = q;
            p1++;
            p2++;
        }

        RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);
        if (RetVal != OK) {
            break;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramWalkZerosTest
 *
 *  @Description:: Test with Walking Zeroes
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramWalkZerosTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1, *p2, q;
    UINT32 i, j, RetVal = OK;

    /* Walk it up. */
    for (j = 0U; j < 32U; j++) {
        p1 = pBufA;
        p2 = pBufB;
        q = 0x1UL << j;
        for (i = 0U; i < Count; i++) {
            *p1 = q;
            *p2 = q;
            p1++;
            p2++;
        }

        RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);
        if (RetVal != OK) {
            break;
        }
    }

    /* Walk it back down. */
    if (RetVal == OK) {
        for (j = 32U; j > 0U; j--) {
            p1 = pBufA;
            p2 = pBufB;
            q = 0x1UL << (j - 1UL);
            for (i = 0U; i < Count; i++) {
                *p1 = q;
                *p2 = q;
                p1++;
                p2++;
            }

            RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);
            if (RetVal != OK) {
                break;
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramWalkOnesTest
 *
 *  @Description:: Test with Walking Ones
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramWalkOnesTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1, *p2, q;
    UINT32 i, j, RetVal = OK;

    /* Walk it up. */
    for (j = 0U; j < 32U; j++) {
        p1 = pBufA;
        p2 = pBufB;
        q = 0xffffffffUL ^ (0x1UL << j);
        for (i = 0U; i < Count; i++) {
            *p1 = q;
            *p2 = q;
            p1++;
            p2++;
        }

        RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);
        if (RetVal != OK) {
            break;
        }
    }

    /* Walk it back down. */
    if (RetVal == OK) {
        for (j = 32U; j > 0U; j--) {
            p1 = pBufA;
            p2 = pBufB;
            q = 0xffffffffUL ^ (0x1UL << (j - 1UL));
            for (i = 0U; i < Count; i++) {
                *p1 = q;
                *p2 = q;
                p1++;
                p2++;
            }

            RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);
            if (RetVal != OK) {
                break;
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramBitSpreadTest
 *
 *  @Description:: Test with Bit Spread
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramBitSpreadTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1, *p2, q;
    UINT32 i, j, RetVal = OK;

    /* Walk it up. */
    for (j = 0U; j < 32U; j++) {
        p1 = pBufA;
        p2 = pBufB;
        q = (0x1UL << j) | (0x1UL << (j + 2UL));
        for (i = 0U; i < Count; i++) {
            *p1 = q;
            *p2 = q;
            p1++;
            p2++;
            q = ~q;
        }

        RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);
        if (RetVal != OK) {
            break;
        }
    }

    /* Walk it back down. */
    if (RetVal == OK) {
        for (j = 32U; j > 0U; j--) {
            p1 = pBufA;
            p2 = pBufB;
            q = (0x1UL << (j - 1UL)) | (0x1UL << (j + 1UL));
            for (i = 0U; i < Count; i++) {
                *p1 = q;
                *p2 = q;
                p1++;
                p2++;
                q = ~q;
            }

            RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);
            if (RetVal != OK) {
                break;
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramBitFlipTest
 *
 *  @Description:: Test with Bit Flip
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramBitFlipTest(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    UINT32 *p1, *p2, q, r;
    UINT32 i, j, k, RetVal = OK;

    for (k = 0U; k < 32U; k++) {
        q = 0x1UL << k;
        for (j = 0U; j < 8U; j++) {
            q = ~q;
            p1 = pBufA;
            p2 = pBufB;
            r = q;
            for (i = 0U; i < Count; i++) {
                *p1 = r;
                *p2 = r;
                p1++;
                p2++;
                r = ~r;
            }

            RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);
            if (RetVal != OK) {
                break;
            }
        }
        if (RetVal != OK) {
            break;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_Dram8bitWrite
 *
 *  @Description:: 8-bit Writes
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_Dram8bitWrite(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    static UINT32 DramTestWord;
    UINT8 *p1;
    const UINT8 *t;
    UINT32 *p2;
    const UINT32 *q = &DramTestWord;
    UINT32 i, RetVal;

    AmbaMisra_TypeCast32(&t, &q);

    /* 8-bit write to pBufB */
    AmbaMisra_TypeCast32(&p1, &pBufB);
    p2 = pBufA;
    for (i = 0U; i < Count; i++) {
        DramTestWord = IoDiag_DramGetRandomVal();
        *p2 = DramTestWord;
        p2++;

        p1[0] = t[0];
        p1[1] = t[1];
        p1[2] = t[2];
        p1[3] = t[3];
        p1++;
        p1++;
        p1++;
        p1++;
    }

    RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);
    if (RetVal == OK) {
        /* 8-bit write to pBufA */
        AmbaMisra_TypeCast32(&p1, &pBufA);
        p2 = pBufB;
        for (i = 0U; i < Count; i++) {
            DramTestWord = IoDiag_DramGetRandomVal();
            *p2 = DramTestWord;
            p2++;

            p1[0] = t[0];
            p1[1] = t[1];
            p1[2] = t[2];
            p1[3] = t[3];
            p1++;
            p1++;
            p1++;
            p1++;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_Dram16bitWrite
 *
 *  @Description:: 16-bit Writes
 *
 *  @Input      ::
 *      pBufA:  start address of memory region A
 *      pBufB:  start address of memory region B
 *      Count:  size of each memory region (in number of 4-bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_Dram16bitWrite(UINT32 *pBufA, UINT32 *pBufB, UINT32 Count)
{
    static UINT32 DramTestWord;
    UINT16 *p1;
    const UINT16 *t;
    UINT32 *p2;
    const UINT32 *q = &DramTestWord;
    UINT32 i, RetVal;

    AmbaMisra_TypeCast32(&t, &q);

    /* 16-bit write to pBufB */
    AmbaMisra_TypeCast32(&p1, &pBufB);
    p2 = pBufA;
    for (i = 0U; i < Count; i++) {
        DramTestWord = IoDiag_DramGetRandomVal();
        *p2 = DramTestWord;
        p2++;

        p1[0] = t[0];
        p1[1] = t[1];
        p1++;
        p1++;
    }

    RetVal = IoDiag_DramCompare(pBufA, pBufB, Count);
    if (RetVal == OK) {
        /* 16-bit write to pBufA */
        AmbaMisra_TypeCast32(&p1, &pBufA);
        p2 = pBufB;
        for (i = 0U; i < Count; i++) {
            DramTestWord = IoDiag_DramGetRandomVal();
            *p2 = DramTestWord;
            p2++;

            p1[0] = t[0];
            p1[1] = t[1];
            p1++;
            p1++;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramTestAll
 *
 *  @Description:: Run all kinds of memory tests
 *
 *  @Input      ::
 *      MemAddr:    start address of memory region
 *      MemSize:    size of memory region
 *      Silence:    Skip message of test item and successful cases, only show fail cases
 *      PrintFunc:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramTestAll(ULONG MemAddr, UINT32 MemSize, UINT32 Silence, AMBA_SHELL_PRINT_f PrintFunc)
{
#define PRINT_LOG(x)    {if (Silence == 0U) {PrintFunc(x);}}

    static AMBT_DIAG_MEM_TEST_s MemTestList[18] = {
        { "Random Value",           IoDiag_DramRandomValTest },
        { "Compare XOR",            IoDiag_DramXorValTest },
        { "Compare SUB",            IoDiag_DramSubValTest },
        { "Compare MUL",            IoDiag_DramMulValTest },
        { "Compare DIV",            IoDiag_DramDivValTest },
        { "Compare OR",             IoDiag_DramOrValTest },
        { "Compare AND",            IoDiag_DramAndValTest },
        { "Sequential Increment",   IoDiag_DramSeqIncValTest },
        { "Solid Bits",             IoDiag_DramSolidBitsTest },
        { "Effing ZeroOnes",        IoDiag_DramZerosOnesTest },
        { "Block Sequential",       IoDiag_DramBlockSeqTest },
        { "Checkerboard",           IoDiag_DramCheckerboardTest },
        { "Bit Spread",             IoDiag_DramBitSpreadTest },
        { "Bit Flip",               IoDiag_DramBitFlipTest },
        { "Walking Ones",           IoDiag_DramWalkOnesTest },
        { "Walking Zeroes",         IoDiag_DramWalkZerosTest },
        { "8-bit Writes",           IoDiag_Dram8bitWrite },
        { "16-bit Writes",          IoDiag_Dram16bitWrite },
    };
    UINT32 *pBuf[2];
    UINT32 i, Count, RetVal = OK;

    if ((MemAddr == 0x0U) || (MemSize == 0x0U)) {
        RetVal = ~OK;
    } else {
        if (Silence == 0U) {
            PRINT_LOG("[Diag][DRAM] MEM: ");
            IoDiag_PrintFormattedInt("0x%08X", MemAddr, PrintFunc);
            IoDiag_PrintFormattedInt("-0x%08X", MemAddr + (MemSize - 1U), PrintFunc);
            IoDiag_PrintFormattedInt(" (%u Bytes)\n", MemSize, PrintFunc);
        }

        AmbaMisra_TypeCast(&pBuf[0], &MemAddr);
        Count = MemSize / sizeof(UINT32);

        PRINT_LOG("[Diag][DRAM] Stuck Address .. ");
//        (void)AmbaKAL_TaskSleep(100U);

        RetVal = IoDiag_DramStuckAddrTest(pBuf[0], Count);
        if (RetVal != OK) {
            PrintFunc("FAIL!\n");
        } else {
            PRINT_LOG("PASS!\n");
            Count >>= 1U;
            pBuf[1] = &pBuf[0][Count];

            for (i = 0U; i < (UINT32)(sizeof(MemTestList) / sizeof((MemTestList)[0])); i++) {
                PRINT_LOG("[Diag][DRAM] ");
                PRINT_LOG(MemTestList[i].pName);
                PRINT_LOG(" .. ");
//                (void)AmbaKAL_TaskSleep(100U);

                RetVal = MemTestList[i].pFunc(pBuf[0], pBuf[1], Count);
                if (RetVal != OK) {
                    PrintFunc("FAIL!\n");
                    break;
                } else {
                    PRINT_LOG("PASS!\n");
                }
            }
        }
    }

    return RetVal;
}

/**
 *  IoDiag_WdtMonitorTaskEntry - WDT monitor task entry
 *  @param[in] EntryArg Argument
 */
static void * IoDiag_DramTestTaskEntry(void * EntryArg)
{
    UINT32 TaskIndex = 0U;
    UINT32 RetVal = OK;

    AmbaMisra_TypeCast32(&TaskIndex, &EntryArg);

    if (TaskIndex < TASK_NUM) {
        while (0U != DiagDramTask.LoopCount[TaskIndex]) {
            RetVal = IoDiag_DramTestAll(DiagDramTask.MemAddr[TaskIndex], DiagDramTask.MemSize[TaskIndex],
                                        DiagDramTask.Silence[TaskIndex], DiagDramPrintFunc);
            IoDiag_PrintFormattedInt("[Diag][DRAM][%d]", TaskIndex, DiagDramPrintFunc);
            IoDiag_PrintFormattedInt(" ### Test %d: ", DiagDramTask.LoopCount[TaskIndex], DiagDramPrintFunc);
            IoDiag_PrintFormattedInt(" %d\n", RetVal, DiagDramPrintFunc);
            DiagDramTask.LoopCount[TaskIndex]--;
        }
        // Mark task end
        DiagDramTask.TaskRunning[TaskIndex] = 0;
    }

    return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_DramTestTaskStart
 *
 *  @Description:: Create a test to run memory tests
 *
 *  @Input      ::
 *      MemAddr:    start address of memory region
 *      MemSize:    size of memory region
 *      PrintFunc:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK/~OK
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_DramTestTaskStart(ULONG MemAddr, UINT32 MemSize, UINT32 LoopCount, UINT32 CpuAffinity, UINT32 Silence, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = ~OK;
    void * EntryArg;
    UINT32 ArgUINT32[1], i;

    for (i = 0; i < TASK_NUM; i++) {
        if (DiagDramTask.TaskRunning[i] == 0U) {
            // Found idle task to run
            if (DiagDramTask.TaskCreated[i] == 1U) {
                // Task has been used, delete it before re-create
                (void)AmbaKAL_TaskTerminate(&DiagDramTask.Task[i]);
                (void)AmbaKAL_TaskDelete(&DiagDramTask.Task[i]);
            }
            // Setup parameter for new task
            if (DiagDramPrintFunc == NULL) {
                DiagDramPrintFunc = PrintFunc;
            }
            DiagDramTask.MemAddr[i] = MemAddr;
            DiagDramTask.MemSize[i] = MemSize;
            DiagDramTask.LoopCount[i] = LoopCount;
            DiagDramTask.Silence[i] = Silence;
            DiagDramTask.TaskCreated[i] = 1;
            DiagDramTask.TaskRunning[i] = 1;
            ArgUINT32[0] = i;
            (void)AmbaUtility_StringPrintUInt32(DiagDramTask.TaskName[i], TASK_NAME_LEN, "DiagDram%d", 1U, ArgUINT32);
            AmbaMisra_TypeCast32(&EntryArg, &i);
            AmbaMisra_TouchUnused(EntryArg);
            // Create new task
            RetVal = AmbaKAL_TaskCreate(&DiagDramTask.Task[i],
                                        DiagDramTask.TaskName[i],
                                        125,
                                        IoDiag_DramTestTaskEntry,
                                        EntryArg,
                                        DiagDramTask.Stack[i],
                                        sizeof(DiagDramTask.Stack[i]),
                                        0U);
            if (RetVal != OK) {
                IoDiag_PrintFormattedInt("[Diag][DRAM] AmbaKAL_TaskCreate failed (%u)\n", RetVal, DiagDramPrintFunc);
            }

            if (RetVal == OK) {
                RetVal = AmbaKAL_TaskSetSmpAffinity(&DiagDramTask.Task[i], CpuAffinity);
                if (RetVal != OK) {
                    IoDiag_PrintFormattedInt("[Diag][DRAM] AmbaKAL_TaskSetSmpAffinity failed (%u)\n", RetVal, DiagDramPrintFunc);
                }
            }

            if (RetVal == OK) {
                RetVal = AmbaKAL_TaskResume(&DiagDramTask.Task[i]);
                if (RetVal != OK) {
                    IoDiag_PrintFormattedInt("[Diag][DRAM] AmbaKAL_TaskResume failed (%u)\n", RetVal, DiagDramPrintFunc);
                }
            }

            if (RetVal == OK) {
                PrintFunc("[Diag][DRAM] ");
                PrintFunc(DiagDramTask.TaskName[i]);
                PrintFunc(" starts.\n");
            }
            break;
        }
    }
    if (i == TASK_NUM) {
        PrintFunc("[Diag][DRAM] All tasks are running\n");
    }

    return RetVal;
}

static void IoDiag_DramCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" unittest      : Start DRAM testing with pre-defined configuration\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [addr] [size] : Start DRAM testing with user-defined configuration\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [addr] [size] task [loop_count] [cpu_affinity] [silent mode]: Start DRAM testing with Task for loop_count times\n");
}

/**
 *  DRAM diagnostic command
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return none
 */
void AmbaIOUTDiag_CmdDRAM(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    static UINT8 DramTestBuf[0x100000U] __attribute__((section(".bss.noinit"))) __attribute__((aligned(0x100000U)));
    const UINT8 *pBuf = &DramTestBuf[0];
    ULONG MemAddr = 0U;
    UINT32 MemSize = 0U, NextArgIdx = 0U, RunByNewTask = 0U, LoopCount = 1U;
    UINT32 CpuAffinity = 3U, Silence = 0U;

    if (ArgCount < 2U) {
        IoDiag_DramCmdUsage(pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "unittest", 8U) == 0) {
            AmbaMisra_TypeCast(&MemAddr, &pBuf);
            MemSize = 0x100000U;
            NextArgIdx = 2U;
        } else if (ArgCount >= 3U) {
            (void)AmbaUtility_StringToUInt64(pArgVector[1], (UINT64 *)&MemAddr);
            (void)AmbaUtility_StringToUInt32(pArgVector[2], &MemSize);
            NextArgIdx = 3U;
        }

        if (ArgCount > NextArgIdx) {
            /* Parse more arguments */
            if (AmbaUtility_StringCompare(pArgVector[NextArgIdx], "task", 4U) == 0) {
                RunByNewTask = 1U;
                NextArgIdx++;
            }
            if (ArgCount > NextArgIdx) {
                (void)AmbaUtility_StringToUInt32(pArgVector[NextArgIdx], &LoopCount);
                NextArgIdx++;
            }
            if (ArgCount > NextArgIdx) {
                (void)AmbaUtility_StringToUInt32(pArgVector[NextArgIdx], &CpuAffinity);
                NextArgIdx++;
            }
            if (ArgCount > NextArgIdx) {
                (void)AmbaUtility_StringToUInt32(pArgVector[NextArgIdx], &Silence);
                NextArgIdx++;
            }
        }
        if (RunByNewTask == 1U) {
            (void)IoDiag_DramTestTaskStart(MemAddr, MemSize, LoopCount, CpuAffinity, Silence, PrintFunc);
        } else {
            (void)IoDiag_DramTestAll(MemAddr, MemSize, 0U, PrintFunc);
        }
    }
}

