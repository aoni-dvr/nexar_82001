/**
 *  @file AmbaMAL_Dev.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details AmbaMAL dev APIs
 *
 */

#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <devctl.h>
#include <sys/mman.h>
#include <sys/cache.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "libfdt.h"
#include "AmbaMAL.h"
#include "../AmbaMAL_Ioctl.h"
#if defined(AST_SUPPORT)
#include "AmbaAST.h"
#endif

/*
 * Define THREAD_POOL_PARAM_T such that we can avoid a compiler
 * warning when we use the dispatch_*() functions below
 */
#define THREAD_POOL_PARAM_T dispatch_context_t

#include <sys/iofunc.h>
#include <sys/dispatch.h>

static resmgr_connect_funcs_t    connect_funcs;
static resmgr_io_funcs_t         io_funcs;
static iofunc_attr_t             attr;

static AMBA_MAL_IOCTL_INFO_s AmbaMalPriv[AMBA_MAL_ID_MAX];
static AMBA_MAL_SEG_INFO_s AmbaMalSegList[AMBA_MAL_SEG_NUM];

#define AmbaMisra_TypeCast(a,b) memcpy(a, b, sizeof(void *));

static UINT32 AmbaMAL_KrnPhys2Global(UINT64 PhysAddr, UINT64 *pGlobalAddr)
{
    UINT32 Ret = 0U;
    UINT32 i, Found = 0U;

    *pGlobalAddr = 0xFF00000000000000UL;
    for (i = 0U; i < AMBA_MAL_SEG_NUM; i++) {
        if (0x0U != AmbaMalSegList[i].Size) {
            if ((PhysAddr >= AmbaMalSegList[i].PhysAddr) && (PhysAddr < AmbaMalSegList[i].PhysAddr + AmbaMalSegList[i].Size)) {
                *pGlobalAddr = AmbaMalSegList[i].GlobalAddr + (PhysAddr - AmbaMalSegList[i].PhysAddr);
                Found = 1U;
                break;
            }
        }
    }

    if(Found == 0U) {
        Ret = 1U;
    }
    return Ret;
}

int AmbaMAL_KrnIoctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int nbytes, status;
    union {
        AMBA_MAL_IOCTL_VERSION_s ReqVer;
        AMBA_MAL_IOCTL_INFO_s ReqInfo;
        AMBA_MAL_IOCTL_SEG_INFO_s ReqSegInfo;
    } *arg;

    /*
     Let common code handle DCMD_ALL_* cases.
     You can do this before or after you intercept devctls, depending
     on your intentions.  Here we aren't using any predefined values,
     so let the system ones be handled first. See note 2.
    */
    if (EOK != (status = iofunc_devctl_verify(ctp, msg, ocb, _IO_DEVCTL_VERIFY_LEN))) {
        return status;
    }
    iofunc_attr_unlock(&attr);
    status = nbytes = 0;

    /*
     Note this assumes that you can fit the entire data portion of
     the devctl into one message.  In reality you should probably
     perform a MsgReadv() once you know the type of message you
     have received to get all of the data, rather than assume
     it all fits in the message.  We have set in our main routine
     that we'll accept a total message size of up to 2 KB, so we
     don't worry about it in this example where we deal with ints.
    */

    /* Get the data from the message. See Note 3. */
    arg = _DEVCTL_DATA(msg->i);

    switch (msg->i.dcmd) {
    case AMBA_MAL_GET_VERSION : {
        arg->ReqVer.Version = AMBA_MAL_KERNEL_VERSION;
        arg->ReqVer.Number = AMBA_MAL_ID_MAX;
        nbytes = sizeof(arg->ReqVer);;
        break;
    }

    case AMBA_MAL_GET_INFO : {
        uint32_t id;

        if(arg->ReqInfo.Info.Id < AMBA_MAL_ID_MAX) {
            id = arg->ReqInfo.Info.Id;
            memcpy(&arg->ReqInfo, &AmbaMalPriv[id], sizeof(AmbaMalPriv[id]));
            nbytes = sizeof(arg->ReqInfo);;
        } else {
            status = (int32_t)-1;
        }
        break;
    }

    case AMBA_MAL_GET_SEG_INFO : {
        if(arg->ReqSegInfo.SegNum == AMBA_MAL_SEG_NUM) {
            memcpy(&arg->ReqSegInfo.SegList, &AmbaMalSegList, sizeof(AmbaMalSegList));
            nbytes = sizeof(arg->ReqSegInfo);;
        } else {
            status = (int32_t)-1;
        }
        break;
    }

    default:
        printf("[ERROR] AmbaMAL_KrnIoctl() : unknown IOCTL 0x%x \n", msg->i.dcmd);
        status = (int32_t)-1;
        break;
    }

    /* Clear the return message. Note that we saved our data past
       this location in the message. */
    memset(&msg->o, 0, sizeof(msg->o));

    /*
     If you wanted to pass something different to the return
     field of the devctl() you could do it through this member.
     See note 5.
    */
    msg->o.ret_val = status;

    /* Indicate the number of bytes and return the message */
    msg->o.nbytes = nbytes;

    iofunc_attr_lock(&attr);
    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + nbytes));
}

static UINT32 AmbaMAL_KrnNodeParse(const void *pFdt, INT32 Node, AMBA_MAL_INFO_s *pInfo, const char **ppName)
{
    UINT32 Ret = 0U;
    const struct fdt_property *pProp = NULL;
    const char *pChr1 = NULL;
    const UINT32 *pValue = NULL;
    const UINT64 *pValue64 = NULL;
    INT32 Len = 0;
    INT32 SubOffset;
    const fdt32_t *php;
    UINT32 handle;

    /*Parse ID */
    pProp = fdt_get_property(pFdt, Node, "amb_mal_id", &Len);
    if (pProp) {
        pChr1 = (const char *)&pProp->data[0];
        AmbaMisra_TypeCast(&pValue, &pChr1);
        pInfo->Id = fdt32_to_cpu(pValue[0]);
        if (pInfo->Id >= AMBA_MAL_ID_MAX) {
            printf("[ERROR] AmbaMAL_KrnNodeParse() : Id(%d) is over AMBA_MAL_ID_MAX(%d)\n", pInfo->Id, AMBA_MAL_ID_MAX);
            Ret = 1U;
        }
    } else {
        Ret = 1U;
    }

    if(Ret == 0U) {
        /*Find handle */
        php = fdt_getprop(pFdt, Node, "memory-region", &Len);
        if (php) {
            handle = fdt32_to_cpu(*php);
            SubOffset = fdt_node_offset_by_phandle(pFdt, handle);
            *ppName = fdt_get_name(pFdt, SubOffset, &Len);
        } else {
            Ret = 1U;
        }
    }

    if(Ret == 0U) {
        /*Parse type and cap */
        pProp = fdt_get_property(pFdt, SubOffset, "reusable", &Len);
        if (pProp) {
            pInfo->Type = AMBA_MAL_TYPE_ALLOCATABLE;
            pInfo->Capability = 0x3FU;
        } else {
            pInfo->Type = AMBA_MAL_TYPE_DEDICATED;
            pInfo->Capability = 0x33U;
        }

        /*Parse base and size */
        pProp = fdt_get_property(pFdt, SubOffset, "reg", &Len);
        if ((pProp != NULL) && (Len == 8)) {
            pChr1 = (const char *)&pProp->data[0];
            AmbaMisra_TypeCast(&pValue, &pChr1);
            pInfo->PhysAddr = (UINT64)fdt32_to_cpu(pValue[0]);
            pInfo->Size = (UINT64)fdt32_to_cpu(pValue[1]);
        } else if ((pProp != NULL) && (Len == 16)) {
            pChr1 = (const char *)&pProp->data[0];
            AmbaMisra_TypeCast(&pValue64, &pChr1);
            pInfo->PhysAddr = (UINT64)fdt64_to_cpu(pValue64[0]);
            pInfo->Size = (UINT64)fdt64_to_cpu(pValue64[1]);
        }
    }

    return Ret;
}

#if defined(AST_SUPPORT)
static void AmbaMAL_KrnSegListInit(void)
{
    AMBA_AST_SEG0_INFO_s *pAmbaAstInfo;
    UINT32 Prot = PROT_READ|PROT_WRITE;

    pAmbaAstInfo = mmap_device_memory(NULL, 0x1000U, Prot, 0, AMBA_AST_INFO_OFFSET);
    if(pAmbaAstInfo != MAP_FAILED) {
        AMBA_AST_SEG_INFO_s *pSegInfo;
        UINT32 i;

        memset(AmbaMalSegList, 0, sizeof(AmbaMalSegList));

        pSegInfo = pAmbaAstInfo->SegList[pAmbaAstInfo->ActiveDomain];
        for (i = 0U; i < AMBA_AST_DOMAIN_SEG_NUM; i++) {
            if (0U != pSegInfo[i].SegSize) {
                AmbaMalSegList[i].PhysAddr = pSegInfo[i].DomainAddr;
                AmbaMalSegList[i].GlobalAddr = (pSegInfo[i].PhysAddr & 0xFFFFFFFFFFUL) | ((UINT64)(pSegInfo[i].GlobalSegId & 0xFFUL) << 56U);
                AmbaMalSegList[i].Size = pSegInfo[i].SegSize;
                printf("AmbaMalSegList[%d] PhysAddr 0x%llx GlobalAddr 0x%llx Size 0x%llx \n",i ,AmbaMalSegList[i].PhysAddr,AmbaMalSegList[i].GlobalAddr,AmbaMalSegList[i].Size);
            }
        }
        if(munmap_device_memory(pAmbaAstInfo, 0x1000U) == -1) {
            printf("[ERROR] AmbaMAL_KrnSegListInit() : could not unmap seg0 errno = %d\n", errno);
        }
    } else {
        printf("[ERROR] AmbaMAL_KrnSegListInit() : could not map seg0 errno = %d\n", errno);
    }
}
#else
static void AmbaMAL_KrnSegListInit(void)
{
    memset(AmbaMalSegList, 0, sizeof(AmbaMalSegList));
    AmbaMalSegList[0].PhysAddr = 0x0UL;
    AmbaMalSegList[0].GlobalAddr = 0x0UL;
    AmbaMalSegList[0].Size = 0xFFFFFFFFFFUL;
}
#endif

static void AmbaMAL_KrnOfInit(void)
{
    UINT32 Ret = 0U;
    const void *pFdt;
    INT32 Rval, Offset, Node;

    AmbaMAL_KrnSegListInit();
    pFdt = mmap( NULL, CONFIG_DTB_SIZE, PROT_READ | PROT_WRITE,MAP_SHARED|MAP_PHYS, NOFD, (off64_t)CONFIG_DTB_LOADADDR);
    Rval = fdt_check_header(pFdt);
    if (Rval == 0) {
        Offset = fdt_node_offset_by_compatible(pFdt, 0,"ambarella,mal");
        if (Offset >= 0) {
            for (Node = fdt_first_subnode(pFdt, Offset); Node >= 0; Node = fdt_next_subnode(pFdt, Node)) {
                AMBA_MAL_INFO_s Info = {0};
                const char *pName = NULL;

                Ret = AmbaMAL_KrnNodeParse(pFdt, Node, &Info, &pName);
                /*Update table */
                if((Ret == 0U) && (Info.Size > 0)) {
                    AmbaMalPriv[Info.Id].Info.Id = Info.Id;
                    AmbaMalPriv[Info.Id].Info.Type = Info.Type;
                    AmbaMalPriv[Info.Id].Info.Capability = Info.Capability;
                    AmbaMalPriv[Info.Id].Info.PhysAddr = Info.PhysAddr;
                    AmbaMalPriv[Info.Id].Info.Size = Info.Size;
                    Ret = AmbaMAL_KrnPhys2Global(Info.PhysAddr, &AmbaMalPriv[Info.Id].Info.GlobalAddr);
                    if(Ret != 0U) {
                        printf("[ERROR] AmbaMAL_KrnOfInit() : AmbaMAL_KrnPhys2Global fail Id(%d) phys: 0x%llx \n", Info.Id,Info.PhysAddr);
                    }
                    sprintf(AmbaMalPriv[Info.Id].Name, "/ram/%s",pName);
                    printf("AmbaMAL_KrnOfInit : id[%d] name [%s] base[0x%llx] size[0x%llx] type [0x%x] cape [0x%x]\n", Info.Id, pName, Info.PhysAddr, Info.Size, Info.Type, Info.Capability);
                } else if (Ret != 0U) {
                    printf("[ERROR] AmbaMAL_KrnOfInit() : AmbaMAL_KrnNodeParse name %s fail Ret 0x%x \n",fdt_get_name(pFdt, Node, NULL),Ret);
                }
            }
        } else {
            printf("[ERROR] AmbaMAL_KrnOfInit() : couldnt find ambarella,mal compatible\n");
        }
    } else {
        printf("[ERROR] AmbaMAL_KrnOfInit() : fdt_check_header fail \n");
    }
}

int AmbaMAL_KrnClose(resmgr_context_t *__ctp, void *__reserved, iofunc_ocb_t *__ocb)
{
    int ret;

    ret = iofunc_close_ocb_default(__ctp, __reserved, __ocb);
    return ret;
}

int main(int argc, char **argv)
{
    /* declare variables we'll be using */
    thread_pool_attr_t   pool_attr;
    resmgr_attr_t        resmgr_attr;
    dispatch_t           *dpp;
    thread_pool_t        *tpp;
    int                  id;

    (void) argc;
    (void) argv;
    /* initialize dispatch interface */
    if((dpp = dispatch_create()) == NULL) {
        printf("%s: Unable to allocate dispatch handle.\n",argv[0]);
        return EXIT_FAILURE;
    }

    /* initialize resource manager attributes */
    memset(&resmgr_attr, 0, sizeof resmgr_attr);
    resmgr_attr.nparts_max = 1;
    resmgr_attr.msg_max_size = 2048;

    /* initialize functions for handling messages */
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                     _RESMGR_IO_NFUNCS, &io_funcs);

    io_funcs.devctl = AmbaMAL_KrnIoctl;
    io_funcs.close_ocb = AmbaMAL_KrnClose;
    /* initialize attribute structure used by the device */
    iofunc_attr_init(&attr, S_IFNAM | 0666, 0, 0);

    /* attach our device name */
    id = resmgr_attach(dpp,            /* dispatch handle        */
                       &resmgr_attr,   /* resource manager attrs */
                       "/dev/ambamal",  /* device name            */
                       _FTYPE_ANY,     /* open type              */
                       0,              /* flags                  */
                       &connect_funcs, /* connect routines       */
                       &io_funcs,      /* I/O routines           */
                       &attr);         /* handle                 */
    if(id == -1) {
        printf("%s: Unable to attach name.\n", argv[0]);
        return EXIT_FAILURE;
    }

    AmbaMAL_KrnOfInit();

    /* initialize thread pool attributes */
    memset(&pool_attr, 0, sizeof pool_attr);
    pool_attr.handle = dpp;
    pool_attr.context_alloc = dispatch_context_alloc;
    pool_attr.block_func = dispatch_block;
    pool_attr.unblock_func = dispatch_unblock;
    pool_attr.handler_func = dispatch_handler;
    pool_attr.context_free = dispatch_context_free;
    pool_attr.lo_water = 2;
    pool_attr.hi_water = 4;
    pool_attr.increment = 1;
    pool_attr.maximum = 50;

    /* allocate a thread pool handle */
    if((tpp = thread_pool_create(&pool_attr,
                                 POOL_FLAG_EXIT_SELF)) == NULL) {
        printf("%s: Unable to initialize thread pool.\n",argv[0]);
        return EXIT_FAILURE;
    }

     /* Start the threads. This function doesn't return. */
    thread_pool_start(tpp);

    printf("ambamal dev main stop \n");
    return EXIT_SUCCESS;
}

