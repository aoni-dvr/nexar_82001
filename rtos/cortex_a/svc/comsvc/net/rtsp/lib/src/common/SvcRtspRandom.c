/**
 *  @file SvcRtspRandom.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details rtsp random module
 *
 */

#include "SvcRtspRandom.h"
#include "AmbaMisraFix.h"
#include "AmbaSvcWrap.h"

#define TYPE_0      0        /* linear congruential */
#define TYPE_3      3        /* x**31 + x**3 + 1 */
#define DEG_3       31
#define SEP_3       3

/* Initially, everything is set up as if from:
   our_initstate(1, &randtbl, 128);
   Note that this initialization takes advantage of the fact that srandom()
   advances the front and rear pointers 10*rand_deg times, and hence the
   rear pointer which starts at 0 will also end up at zero; thus the zeroeth
   element of the state information, which contains info about the current
   position of the rear pointer is just
   MAX_TYPES * (rptr - state) + TYPE_3 == TYPE_3. */
static ULONG randtbl[DEG_3 + 1] = {
    (ULONG)TYPE_3,
    0x9a319039UL, 0x32d9c024UL, 0x9b663182UL, 0x5da1f342UL, 0xde3b81e0UL, 0xdf0a6fb5UL,
    0xf103bc02UL, 0x48f340fbUL, 0x7449e56bUL, 0xbeb1dbb0UL, 0xab5c5918UL, 0x946554fdUL,
    0x8c2e680fUL, 0xeb3d799fUL, 0xb11ee0b7UL, 0x2d436b86UL, 0xda672e2aUL, 0x1588ca88UL,
    0xe369735dUL, 0x904f35f7UL, 0xd7158fd6UL, 0x6fa6f051UL, 0x616e6b96UL, 0xac94efdcUL,
    0x36413f93UL, 0xc622c298UL, 0xf5a42ab8UL, 0x8a88d77bUL, 0xf5ad9d0eUL, 0x8999220bUL,
    0x27fb47b9UL,
};

 /* fptr and rptr are two pointers into the state info, a front and a rear
    pointer.  These two pointers are always rand_sep places aparts, as they
    cycle cyclically through the state information.  (Yes, this does mean we
    could get away with just one pointer, but the code for random() is more
    efficient this way).  The pointers are left positioned as they would be
    from the call
    our_initstate(1, randtbl, 128);
    (The position of the rear pointer, rptr, is really 0 (as explained above
    in the initialization of randtbl) because the state table pointer is set
    to point to randtbl[1] (as explained below). */
static ULONG *fptr = &randtbl[SEP_3 + 1];
static ULONG *rptr = &randtbl[1];


/* The following things are the pointer to the state information table, the
   type of the current generator, the degree of the current polynomial being
   used, and the separation between the two pointers.  Note that for efficiency
   of random(), we remember the first location of the state information, not
   the zeroeth.  Hence it is valid to access state[-1], which is used to
   store the type of the R.N.G.  Also, we remember the last location, since
   this is more efficient than indexing every time to find the address of
   the last element to see if the front and rear pointers have wrapped. */
static ULONG *state = &randtbl[1];
static INT32 rand_type = TYPE_3;

/* srandom:
   Initialize the random number generator based on the given seed.  If the
   type is the trivial no-state-information type, just remember the seed.
   Otherwise, initializes state[] based on the given "seed" via a linear
   congruential generator.  Then, the pointers are set to known locations
   that are exactly rand_sep places apart.  Lastly, it cycles the state
   information a given number of times to get rid of any initial dependencies
   introduced by the L.C.R.N.G.  Note that the initialization of randtbl[]
   for default usage relies on values produced by this routine. */
void our_srandom(UINT32 x)
{
    INT32 i;
    static INT32 rand_deg = DEG_3;
    static INT32 rand_sep = SEP_3;

    if (rand_type == TYPE_0) {
        state[0] = x;
    } else {
        state[0] = x;
        for (i = 1; i < rand_deg; i++) {
            state[i] = (1103515245UL*state[i-1]) + 12345UL;
        }

        fptr = &state[rand_sep];
        rptr = &state[0];

        for (i = 0; i < (10*rand_deg); i++) {
            (void)our_random();
        }
    }
}

/* random:
   If we are using the trivial TYPE_0 R.N.G., just do the old linear
   congruential bit.  Otherwise, we do our fancy trinomial stuff, which is
   the same in all the other cases due to all the global variables that have
   been set up.  The basic operation is to add the number at the rear pointer
   into the one at the front pointer.  Then both pointers are advanced to
   the next location cyclically in the table.  The value returned is the sum
   generated, reduced to 31 bits by throwing away the "least random" low bit.
   Note: the code takes advantage of the fact that both the front and
   rear pointers can't wrap on the same call by not testing the rear
   pointer if the front one has wrapped.
   Returns a 31-bit random number. */
UINT32 our_random(void)
{
    static ULONG *end_ptr = &randtbl[DEG_3 + 1];

    UINT32  i;
    ULONG   SrcUL, SrcULnd, SrcULRptr = 0U;

    if (rand_type == TYPE_0) {
        state[0] = ((state[0]*1103515245UL) + 12345UL) & 0x7fffffffUL;
        i = (UINT32)state[0];
    } else {
        *fptr += *rptr;
        i = (UINT32)((*fptr >> 1) & 0x7fffffffUL);  /* chucking least random bit */

        AmbaMisra_TypeCast(&SrcUL, &fptr);
        SrcUL += sizeof(fptr[0]);
        AmbaMisra_TypeCast(&fptr, &SrcUL);

        AmbaMisra_TypeCast(&SrcULnd, &end_ptr);
        if (SrcUL >= SrcULnd) {
            fptr = state;
            AmbaMisra_TypeCast(&SrcULRptr, &rptr);
            SrcULRptr += sizeof(rptr[0]);
        } else {
            AmbaMisra_TypeCast(&SrcULRptr, &rptr);
            SrcULRptr += sizeof(rptr[0]);
            AmbaMisra_TypeCast(&rptr, &SrcULRptr);

            if (SrcULRptr >= SrcULnd) {
                rptr = state;
            }
        }
    }

    (void)SrcULRptr;

    return (i);
}

UINT32 our_random32(void)
{
    UINT32 random1 = our_random();
    UINT32 random2 = our_random();

    return (UINT32)((random2<<31) | random1);
}

