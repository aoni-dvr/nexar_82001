/**
 *  @file AmbaCtorDtor.cpp
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
 *  @details Constructor/Destructor sample codes.
 */

/*
 *  Notes:
 *      - Has NO '-fexceptions'
 *          - Can not find define __EXCEPTIONS
 *          - Sample codes without exception
*
 */
//#include <new>
//using namespace std;

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"{
#endif
#include <AmbaTypes.h>
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C"{
#endif
//void AmbaCtorDtor_Init(AMBA_KAL_BYTE_POOL_t *pMpl)
//{
//    pAmbaCtorDtorMpl = pMpl;
//}
#ifdef __cplusplus
}
#endif

void* operator new(size_t sz) throw()
{
    void* p = malloc(sz);

    if (p != NULL) {
        return p;
    } else {
        return NULL;
    }
}

void* operator new[](size_t sz) throw()
{
    void* p = malloc(sz);

    if (p != NULL) {
        return p;
    } else {
        return NULL;
    }
}


void operator delete(void* pmem) throw()
{
    free(pmem);
}

void operator delete[](void* pmem) throw()
{
    free(pmem);
}

void operator delete(void* pmem, size_t sz) throw()
{
    (void)sz;
    free(pmem);
}

void operator delete[](void* pmem, size_t sz) throw()
{
    (void)sz;
    free(pmem);
}

