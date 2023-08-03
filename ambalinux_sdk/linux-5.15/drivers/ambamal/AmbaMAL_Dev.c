/**
 *  @file AmbaMAL_Dev.c
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
 *  @details Ambarella MAL (Memory Abstraction Layer) APIs
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/of_reserved_mem.h>
#include <linux/of_device.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include "AmbaMAL.h"
#include "AmbaMAL_Ioctl.h"
#include "AmbaMAL_KrnPriv.h"
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
#include <plat/iav_helper.h>
#else
#include <soc/ambarella/iav_helper.h>
#endif
#if defined(AST_SUPPORT)
#include "AmbaAST.h"
#endif

MODULE_AUTHOR("Sam_Syu");
MODULE_LICENSE("GPL");

#define DEVICE_NAME     "ambamal"

static INT32           AmbaMalMajor;
static struct class*    AmbaMalClass;
static struct proc_dir_entry* AmbaMalProcStatus = NULL;

struct device*   AmbaMalDevice;
AMBA_MAL_INFO_PRIV_s AmbaMalPriv[AMBA_MAL_ID_MAX] = {0};
AMBA_MAL_SEG_INFO_s AmbaMalSegList[AMBA_MAL_SEG_NUM] = {0};

extern struct proc_dir_entry *get_ambarella_proc_dir(void);

static int AmbaMAL_KrnStatusShow(struct seq_file *m, void *v)
{
    UINT32 i;

    for(i = 0U; i < AMBA_MAL_ID_MAX; i++) {
        if(AmbaMalPriv[i].Info.Type != AMBA_MAL_TYPE_INVALID) {
            AmbaMAL_KrnMmbDump(&AmbaMalPriv[i], m);
        }
    }

    return 0;
}


static int AmbaMAL_KrnStatusOpen(struct inode *inode, struct file *file)
{
    return single_open(file, AmbaMAL_KrnStatusShow, PDE_DATA(inode));
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
static const struct file_operations AmbaMalStatusFops = {
    .open = AmbaMAL_KrnStatusOpen,
    .read = seq_read,
    .llseek = seq_lseek,
};
#else
static const struct proc_ops AmbaMalStatusFops = {
    .proc_open = AmbaMAL_KrnStatusOpen,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
};
#endif

static INT32 AmbaMAL_KrnMMap(struct file *filp, struct vm_area_struct *vma)
{
    INT32 Rval;
    UINT64 Base = 0UL, Size = 0UL, End = 0UL;
    UINT32 i,Found = 0U;

    Base = vma->vm_pgoff << 12;
    Size = vma->vm_end - vma->vm_start;
    End  =  Base + Size;

    for(i = 0U; i < AMBA_MAL_ID_MAX; i++) {
        if((Base >= AmbaMalPriv[i].Info.PhysAddr) && (End <= (AmbaMalPriv[i].Info.PhysAddr + AmbaMalPriv[i].Info.Size))) {
            Found = 1U;
            break;
        }
    }

    if(Found == 0U) {
        printk("[ERROR] AmbaMAL_KrnMMap() : mmap source not found (base=0x%llx size=0x%llx)\n", Base, Size);
        Rval = 1;
    } else {
        if((AmbaMalPriv[i].Info.Capability & AMBA_MAL_CAPLTY_CAN_MAP) != 0U) {
            UINT32 Attri = 0U;

            if (filp->f_flags & O_DSYNC) {
                Attri = AMBA_MAL_ATTRI_NON_CACHE;
                vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
            } else {
                Attri = AMBA_MAL_ATTRI_CACHE;
            }
            Rval = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, vma->vm_end - vma->vm_start, vma->vm_page_prot);
            if (Rval != 0) {
                printk("[ERROR] AmbaMAL_KrnMMap() : Id [%d]  mmap fail (base=0x%llx size=0x%llx) rval(%d) \n", i, Base, Size, Rval);
            } else {
                if(AmbaMalPriv[i].Info.Type == AMBA_MAL_TYPE_ALLOCATABLE) {
                    Rval = AmbaMAL_KrnCmaMapUser(&AmbaMalPriv[i], Base, Size, Attri, vma);
                }
            }
        } else {
            printk("[ERROR] AmbaMAL_KrnMMap() : Id [%d] cap [0x%x] dont have map capability\n", i, AmbaMalPriv[i].Info.Capability);
            Rval = 1;
        }
    }

    return Rval;
}

static long AmbaMAL_KrnIoctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    INT32 Rval = 0;
    UINT32 Ret = 0U;
    AMBA_MAL_IOCTL_VERSION_s ReqVer;
    AMBA_MAL_INFO_s ReqInfo;
    AMBA_MAL_BUF_s ReqBuf;
    AMBA_MAL_IOCTL_CACHE_s ReqCache;
    AMBA_MAL_IOCTL_USAGE_s ReqUsage;
    UINT32 SegNum;

    switch (cmd) {
    case AMBA_MAL_GET_VERSION : {
        ReqVer.Version = AMBA_MAL_KERNEL_VERSION;
        ReqVer.Number = AMBA_MAL_ID_MAX;
        Rval = copy_to_user((void*)arg, &ReqVer, sizeof(ReqVer));
        break;
    }
    case AMBA_MAL_GET_INFO : {
        Rval = copy_from_user(&ReqInfo, (void*)arg, sizeof(ReqInfo));
        if(Rval == 0) {
            if(ReqInfo.Id < AMBA_MAL_ID_MAX) {
                Rval = copy_to_user((void*)arg, &AmbaMalPriv[ReqInfo.Id].Info, sizeof(AmbaMalPriv[ReqInfo.Id].Info));
            } else {
                Rval = -1;
            }
        }
        break;
    }
    case AMBA_MAL_ALLOC_BUF : {
        Rval = copy_from_user(&ReqBuf, (void*)arg, sizeof(ReqBuf));
        if(Rval == 0) {
            UINT64 PhysAddr = 0UL, RealSize = 0UL;

            if(ReqBuf.Id < AMBA_MAL_ID_MAX) {
                Ret = AmbaMAL_KrnCmaAlloc(&AmbaMalPriv[ReqBuf.Id], ReqBuf.Size, ReqBuf.Align, &PhysAddr, &RealSize, (void *)f);
                if(Ret == 0U) {
                    ReqBuf.PhysAddr  = PhysAddr;
                    ReqBuf.RealSize  = RealSize;
                    if(AmbaMAL_KrnPhys2Global(PhysAddr, &ReqBuf.GlobalAddr) == 0U) {
                        Rval = copy_to_user((void*)arg, &ReqBuf, sizeof(ReqBuf));
                    } else {
                        Rval = -1;
                    }
                } else {
                    Rval = -1;
                }
            } else {
                Rval = -1;
                printk("[ERROR] AMBA_MAL_ALLOC_BUF : Alloc fail Id [%d] Size 0x%llx \n", ReqBuf.Id, ReqBuf.Size);
                AmbaMAL_Dump(ReqBuf.Id);
            }
        }
        break;
    }
    case AMBA_MAL_FREE_BUF : {
        Rval = copy_from_user(&ReqBuf, (void*)arg, sizeof(ReqBuf));
        if(Rval == 0) {
            if(ReqBuf.Id < AMBA_MAL_ID_MAX) {
                Ret = AmbaMAL_KrnCmaFree(&AmbaMalPriv[ReqBuf.Id], ReqBuf.PhysAddr, ReqBuf.RealSize);
                if(Ret != 0U) {
                    Rval = -1;
                }
            } else {
                Rval = -1;
                printk("[ERROR] AMBA_MAL_FREE_BUF : Free fail Id [%d] PhysAddr 0x%llx Size 0x%llx \n", ReqBuf.Id, ReqBuf.PhysAddr,ReqBuf.RealSize);
                AmbaMAL_Dump(ReqBuf.Id);
            }
        }
        break;
    }
    case AMBA_MAL_CACHE_OPS : {
        UINT32 Ret = 0U;
        void *pVirtAddr = NULL;

        Rval = copy_from_user(&ReqCache, (void*)arg, sizeof(ReqCache));
        if(Rval == 0) {
            if(ReqCache.Id < AMBA_MAL_ID_MAX) {
                Ret = AmbaMAL_Phys2Virt(ReqCache.Id, ReqCache.PhysAddr, AMBA_MAL_ATTRI_CACHE, &pVirtAddr);
                if(Ret == 0U) {
                    if(ReqCache.Ops == 1U) {
                        Ret = AmbaMAL_CacheClean(ReqCache.Id, pVirtAddr, ReqCache.Size);
                    } else {
                        Ret = AmbaMAL_CacheInvalid(ReqCache.Id, pVirtAddr, ReqCache.Size);
                    }
                } else {
                    printk("[ERROR] AMBA_MAL_CACHE_OPS : AmbaMAL_Phys2Virt fail Ops [%d] Id [%d] PhysAddr 0x%llx \n", ReqCache.Ops, ReqCache.Id,ReqCache.PhysAddr);
                    AmbaMAL_Dump(ReqCache.Id);
                }
            } else {
                Rval = -1;
            }
        }

        if(Ret != 0U) {
            Rval = -1;
        }
        break;
    }
    case AMBA_MAL_GET_USAGE : {
        Rval = copy_from_user(&ReqUsage, (void*)arg, sizeof(ReqUsage));
        if(Rval == 0) {
            if(ReqUsage.Id < AMBA_MAL_ID_MAX) {
                if(AmbaMalPriv[ReqUsage.Id].Info.Type == AMBA_MAL_TYPE_ALLOCATABLE) {
                    AmbaMAL_KrnMmbGetUsedSize(&AmbaMalPriv[ReqUsage.Id], &ReqUsage.UsedLen);
                    ReqUsage.FreeLen = AmbaMalPriv[ReqUsage.Id].Info.Size - ReqUsage.UsedLen;
                } else if (AmbaMalPriv[ReqUsage.Id].Info.Type == AMBA_MAL_TYPE_DEDICATED) {
                    ReqUsage.UsedLen = 0UL;
                    ReqUsage.FreeLen = AmbaMalPriv[ReqUsage.Id].Info.Size;
                } else {
                    Rval = -1;
                }

                if(Rval == 0U) {
                    Rval = copy_to_user((void*)arg, &ReqUsage, sizeof(ReqUsage));
                }
            } else {
                Rval = -1;
            }
        }

        break;
    }
    case AMBA_MAL_GET_SEG_INFO : {
        AMBA_MAL_IOCTL_SEG_INFO_s *pSegInfo = (AMBA_MAL_IOCTL_SEG_INFO_s *)arg;

        Rval = copy_from_user(&SegNum, (void*)arg, sizeof(UINT32));
        if(Rval == 0) {
            if(SegNum == AMBA_MAL_SEG_NUM) {
                Rval = copy_to_user((void*)pSegInfo->SegList, &AmbaMalSegList, sizeof(AmbaMalSegList));
            } else {
                Rval = -1;
            }
        }
        break;
    }
    default:
        printk("[ERROR] AmbaMAL_KrnIoctl() : unknown IOCTL 0x%x\n", cmd);
        Rval = -1;
        break;
    }

    if(Rval != 0) {
        printk("[ERROR] AmbaMAL_KrnIoctl() : cmd 0x%x fail ret 0x%x\n", cmd, Rval);
    }
    return Rval;
}

static int AmbaMAL_Release(struct inode *inode, struct file *file)
{
    UINT32 i;

    (void) inode;
    for(i = 0U; i < AMBA_MAL_ID_MAX; i++) {
        if(AmbaMalPriv[i].Info.Type == AMBA_MAL_TYPE_ALLOCATABLE) {
            AmbaMAL_KrnMmbReleaseByOwner(&AmbaMalPriv[i], (void *)file, AmbaMAL_KrnCmaReleaseCb);
        }
    }
    return 0;
}


static const struct file_operations ambamal_fops = {
    .owner = THIS_MODULE,
    .mmap = AmbaMAL_KrnMMap,
    .unlocked_ioctl = AmbaMAL_KrnIoctl,
    .release = AmbaMAL_Release,
};

static UINT32 AmbaMAL_KrnNodeParse(struct device_node *pSubNode, AMBA_MAL_INFO_s *pInfo, UINT32 *pIsAma)
{
    UINT32 Ret = 0U;
    struct device_node *pNode;
    __be32 *pReg;
    __be64 *pReg64;
    INT32 Len;
    const char *sub_name = of_node_full_name(pSubNode);

    if (of_property_read_u32(pSubNode, "amb_mal_id", (UINT32 *)&pInfo->Id) == 0) {
        if(pInfo->Id < AMBA_MAL_ID_MAX) {
            if (of_get_property(pSubNode, "dedicate-alloc", NULL) != NULL) {
                *pIsAma = 1U;
            }

            pNode = of_parse_phandle(pSubNode, "memory-region", 0);
            if (pNode != NULL) {
                const char *handle_name = of_node_full_name(pNode);

                pReg = (__be32 *) of_get_property(pNode, "reg", &Len);
                if (pReg && (Len == (2 * sizeof(u32)))) {
                    pInfo->PhysAddr = be32_to_cpu(pReg[0]);
                    pInfo->Size = be32_to_cpu(pReg[1]);
                } else if (pReg && (Len == (4 * sizeof(u32)))) {
                    pReg64 = (__be64 *)pReg;
                    pInfo->PhysAddr = be64_to_cpu(pReg64[0]);
                    pInfo->Size = be64_to_cpu(pReg64[1]);
                } else {
                    Ret = 1U;
                    printk(KERN_DEBUG "AmbaMal : node [%s] name [%s] id[%d] is no region\n",sub_name, handle_name, pInfo->Id);
                }

                if(Ret == 0U) {
                    if (of_get_property(pNode, "reusable", NULL) != NULL) {
                        pInfo->Type = AMBA_MAL_TYPE_ALLOCATABLE;
                        pInfo->Capability = 0x3FU;
                        if(*pIsAma == 1U) {
                            Ret = 1U;
                            printk("[ERROR] AmbaMAL_KrnOfInit() : node [%s] memory type is reusable but have dedicate-alloc attribute\n",sub_name);
                        }
                    } else if (of_get_property(pNode, "no-map", NULL) != NULL) {
                        if(*pIsAma == 1U) {
                            pInfo->Type = AMBA_MAL_TYPE_ALLOCATABLE;
                            pInfo->Capability = 0x3FU;
                        } else {
                            pInfo->Type = AMBA_MAL_TYPE_DEDICATED;
                            pInfo->Capability = 0x33U;
                        }
                    } else {
                        Ret = 1U;
                        printk("[ERROR] AmbaMAL_KrnOfInit() : node [%s] amb_mal_region memory type unknow \n",sub_name);
                    }
                }

                if(Ret == 0U) {
                    printk("AmbaMal : node [%s] name [%s] id[%d] base[0x%llx] size[0x%llx] type [0x%x] cape [0x%x] IsAma [%d]\n",sub_name, handle_name, pInfo->Id, pInfo->PhysAddr, pInfo->Size, pInfo->Type, pInfo->Capability,*pIsAma);
                }
            } else {
                Ret = 1U;
                printk("[ERROR] AmbaMAL_KrnOfInit() : node [%s] don't have amb_mal_region \n",sub_name);
            }
        } else {
            Ret = 1U;
            printk("[ERROR] AmbaMAL_KrnOfInit() : node [%s] id(%d) over AMBA_MAL_ID_MAX(%d) \n",sub_name,pInfo->Id,AMBA_MAL_ID_MAX);
        }
    } else {
        Ret = 1U;
        printk("[ERROR] AmbaMAL_KrnOfInit() : node [%s] don't have amb_mal_id \n",sub_name);
    }

    return Ret;
}

#if defined(AST_SUPPORT)
static void AmbaMAL_KrnSegListInit(void)
{
    AMBA_AST_SEG0_INFO_s *pAmbaAstInfo;
    pgprot_t Prot = __pgprot(PROT_NORMAL);

    if(AMBA_AST_DOMAIN_SEG_NUM > AMBA_MAL_SEG_NUM) {
        printk("[ERROR] AmbaMAL_KrnSegListInit() : AMBA_AST_DOMAIN_SEG_NUM(%d) > AMBA_MAL_SEG_NUM(%d) \n",AMBA_AST_DOMAIN_SEG_NUM,AMBA_MAL_SEG_NUM);
    } else {
        pAmbaAstInfo = __ioremap(AMBA_AST_INFO_OFFSET, 0x1000U, Prot);
        if(pAmbaAstInfo != NULL) {
            AMBA_AST_SEG_INFO_s *pSegInfo;
            UINT32 i;

            memset(AmbaMalSegList, 0, sizeof(AmbaMalSegList));

            pSegInfo = pAmbaAstInfo->SegList[pAmbaAstInfo->ActiveDomain];
            for (i = 0U; i < AMBA_AST_DOMAIN_SEG_NUM; i++) {
                if (0U != pSegInfo[i].SegSize) {
                    AmbaMalSegList[i].PhysAddr = pSegInfo[i].DomainAddr;
                    AmbaMalSegList[i].GlobalAddr = (pSegInfo[i].PhysAddr & 0xFFFFFFFFFFUL) | ((UINT64)(pSegInfo[i].GlobalSegId & 0xFFUL) << 56U);
                    AmbaMalSegList[i].Size = pSegInfo[i].SegSize;
                    printk(KERN_DEBUG "AmbaMalSegList[%d] PhysAddr 0x%llx GlobalAddr 0x%llx Size 0x%llx \n",i ,AmbaMalSegList[i].PhysAddr,AmbaMalSegList[i].GlobalAddr,AmbaMalSegList[i].Size);
                }
            }
            iounmap(pAmbaAstInfo);
        } else {
            printk("[ERROR] AmbaMAL_KrnSegListInit() : could not map seg0 \n");
        }
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

static void __init AmbaMAL_KrnOfInit(struct device_node *pNp)
{
    struct device_node *pSubNode;

    AmbaMAL_KrnSegListInit();
    for_each_available_child_of_node(pNp, pSubNode) {
        AMBA_MAL_INFO_s Info = {0};
        UINT32  IsAma = 0U;
        UINT32 Ret = 0U;

        Ret = AmbaMAL_KrnNodeParse(pSubNode, &Info, &IsAma);
        if(Ret == 0U) {
            AmbaMalPriv[Info.Id].Info.Id = Info.Id;
            AmbaMalPriv[Info.Id].Info.Type = Info.Type;
            AmbaMalPriv[Info.Id].Info.Capability = Info.Capability;
            AmbaMalPriv[Info.Id].Info.PhysAddr = Info.PhysAddr;
            AmbaMalPriv[Info.Id].Info.Size = Info.Size;
            AmbaMalPriv[Info.Id].IsAma = IsAma;
            Ret = AmbaMAL_KrnPhys2Global(Info.PhysAddr, &AmbaMalPriv[Info.Id].Info.GlobalAddr);
            if(Ret != 0U) {
                printk("[ERROR] AmbaMAL_KrnOfInit() : AmbaMAL_KrnPhys2Global fail Id(%d) phys: 0x%llx \n", Info.Id,Info.PhysAddr);
            }
            AmbaMAL_KrnMmbInit(&AmbaMalPriv[Info.Id]);
            if(AmbaMalPriv[Info.Id].Info.Type == AMBA_MAL_TYPE_ALLOCATABLE) {
                if(AmbaMalPriv[Info.Id].IsAma == 0U) {
                    if( AmbaMAL_KrnCmaOpsRegister(&AmbaMalPriv[Info.Id], pSubNode) != 0 ) {
                        printk("[ERROR] AmbaMAL_KrnOfInit() : AmbaMAL_KrnCmaOpsRegister fail Id(%d) \n", Info.Id);
                    }
                } else {
                    if( AmbaMAL_KrnAmaOpsRegister(&AmbaMalPriv[Info.Id], pSubNode) != 0 ) {
                        printk("[ERROR] AmbaMAL_KrnOfInit() : AmbaMAL_KrnAmaOpsRegister fail Id(%d) \n", Info.Id);
                    }
                }
            }
        }
    }

}

static const struct of_device_id __ambamal_of_table = {
    .compatible = "ambarella,mal",
    .data = AmbaMAL_KrnOfInit,
};

static INT32  __init AmbaMAL_KrnInit(void)
{
    struct device_node *pNp;
    const struct of_device_id *pMatch;
    of_init_fn_1 init_func;
    struct device_node *pNode;

    pNode = of_find_compatible_node(NULL, NULL, "ambarella,mal");
    if (!pNode) {
        of_node_put(pNode);
        printk("[AmbaMal] no device node \n");
        return -ENODEV;
    } else if (!of_device_is_available(pNode)) {
        of_node_put(pNode);
        printk("[ERROR] AmbaMAL_KrnInit() : device node is disabled \n");
        return -ENODEV;
    }

    AmbaMalMajor = register_chrdev(0, DEVICE_NAME, &ambamal_fops);
    if (AmbaMalMajor < 0) {
        printk("[ERROR] AmbaMAL_KrnInit() : failed to register device %d.\n", AmbaMalMajor);
        return AmbaMalMajor;
    }

    AmbaMalClass = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(AmbaMalClass)) {
        unregister_chrdev(AmbaMalMajor, DEVICE_NAME);
        printk("[ERROR] AmbaMAL_KrnInit() : failed to create class.\n");
        return PTR_ERR(AmbaMalClass);
    }

    AmbaMalDevice = device_create(AmbaMalClass, NULL, MKDEV(AmbaMalMajor, 0),
                                  NULL, DEVICE_NAME);
    if (IS_ERR(AmbaMalDevice)) {
        class_destroy(AmbaMalClass);
        unregister_chrdev(AmbaMalMajor, DEVICE_NAME);
        printk("[ERROR] AmbaMAL_KrnInit() : falied to create device.\n");
        return PTR_ERR(AmbaMalDevice);
    }

    AmbaMAL_KrnCmaOpsInit();

    for_each_matching_node_and_match(pNp, &__ambamal_of_table, &pMatch) {
        if (!of_device_is_available(pNp)) {
            continue;
        }

        init_func = pMatch->data;
        init_func(pNp);
    }

    AmbaMalProcStatus = proc_create_data("AmbaMalStatus", S_IRUGO, get_ambarella_proc_dir(), &AmbaMalStatusFops, NULL);

    printk("AmbaMal: module init\n");
    return 0;
}

static void __exit AmbaMAL_KrnExit(void)
{
    AmbaMAL_KrnCmaOpsDeInit();

    device_destroy(AmbaMalClass, MKDEV(AmbaMalMajor, 0));
    class_destroy(AmbaMalClass);
    unregister_chrdev(AmbaMalMajor, DEVICE_NAME);
    if (AmbaMalProcStatus != NULL) {
        remove_proc_entry("AmbaMalStatus", get_ambarella_proc_dir());
        AmbaMalProcStatus = NULL;
    } /* if (scheduler_support.cvstatus_proc  != NULL) */
    printk("AmbaMal: module exit\n");
    return;
}

module_init(AmbaMAL_KrnInit);
module_exit(AmbaMAL_KrnExit);
