/**
 *  @file AmbaMemPoolWrap.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Utils of memory pool
 *
 */

#include "AmbaMisraFix.h"
#include "tx_api_SMP.h"
#include <stdlib.h>

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
#include <stdio.h> //debug
#define DMSG printf
#else
#define DMSG(...) //...
#endif

static TX_BYTE_POOL g_AmbaMemPoolUtil_Pool = {0};
static int inited = 0;

/*  @Return     ::
 *          int : 0 on success, non-0 error code otherwise
 */
int AmbaMemPoolUtil_Init(void)
{
    int Rval = 0;
    extern void *__memutilpool_start, *__memutilpool_end;

    if (inited == 0) {
        void *pBaseAddr, *pEndAddr;
        UINT32 Base, End;

        pBaseAddr = &__memutilpool_start;
        AmbaMisra_TypeCast32(&Base, &pBaseAddr);
        pEndAddr = &__memutilpool_end;
        AmbaMisra_TypeCast32(&End, &pEndAddr);

        DMSG("memutilpool start:%p(0x%08x) end:%p(0x%08x) size:0x%08x\n",pBaseAddr,Base,pEndAddr,End,End-Base);

        Rval = tx_byte_pool_create(&g_AmbaMemPoolUtil_Pool,       /* Pointer to a memory pool Control Block */
                                   NULL,            /* Pointer to the name of the memory pool */
                                   pBaseAddr,       /* Pointer to a memory pool */
                                   (End - Base));   /* Total number of bytes of the memory pool */
        if (Rval == 0) {
            inited = 1;
        }
    }

    return Rval;
}

/*  @Return     ::
 *          int : 0 on success, non-0 error code otherwise
 */
int AmbaMemPoolUtil_Release(void)
{
    int Rval;

    Rval = tx_byte_pool_delete(&g_AmbaMemPoolUtil_Pool);
    inited = 0;

    return Rval;
}

void *AmbaMemPoolUtil_malloc(unsigned int size)
{
    int Rval;
    void *pMem = NULL;

    Rval = AmbaMemPoolUtil_Init();
    if (Rval !=0) {//error
        return NULL;
    }

    Rval = tx_byte_allocate(&g_AmbaMemPoolUtil_Pool, &pMem, size, 100);
    if (Rval != 0) {
        return NULL;
    }

    DMSG("malloc: ptr(%p) size:%u\n",pMem,size);
    return pMem;
}

void *AmbaMemPoolUtil_calloc(unsigned int nitems, unsigned int size)
{
    int Rval;
    unsigned int tsize;
    void *pMem = NULL;

    Rval = AmbaMemPoolUtil_Init();
    if (Rval !=0) {//error
        return NULL;
    }

    tsize = nitems * size;
    Rval = tx_byte_allocate(&g_AmbaMemPoolUtil_Pool, &pMem, tsize, 100);
    if (Rval != 0) {
        return NULL;
    }

    TX_MEMSET(pMem, 0, tsize);

    DMSG("calloc: ptr(%p) nitems:%u size:%u\n",pMem,nitems,size);
    return pMem;
}

void AmbaMemPoolUtil_free(void *ptr)
{
    DMSG("free: ptr(%p)\n",ptr);
    tx_byte_release(ptr);
}
