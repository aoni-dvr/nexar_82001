#ifndef  AMBAMAL_KRNPRIV_H
#define  AMBAMAL_KRNPRIV_H

#include "AmbaMAL_Ioctl.h"

typedef struct {
    UINT32 Id;
    UINT64 PhysAddr;
    void *pVirtAddr;
    UINT64 Size;
    UINT32 RefCnt;
    void *Owner;
    struct list_head List;
} AMBA_MAL_MMB_s;

typedef struct {
    void *pVirtAddr;
    UINT64 BasePfn;
    UINT64 Count;
    unsigned long *pBitmap;
    UINT32 OrderPerBit;
    UINT32 Align;
    struct mutex AmaMutex;
} AMBA_MAL_AMA_s;

typedef struct {
    AMBA_MAL_INFO_s Info;
    /* CMA */
    struct device *pDev;
    /* MMB */
    struct list_head MmbList;
    struct list_head FreeMmbList;
    struct mutex MmbMutex;
    /* AMA */
    UINT32  IsAma;
    AMBA_MAL_AMA_s Ama;
} AMBA_MAL_INFO_PRIV_s;

extern AMBA_MAL_INFO_PRIV_s AmbaMalPriv[AMBA_MAL_ID_MAX];
extern AMBA_MAL_SEG_INFO_s AmbaMalSegList[AMBA_MAL_SEG_NUM];

extern struct device*   AmbaMalDevice;

/* KrnSeg API */
UINT32 AmbaMAL_KrnPhys2Global(UINT64 PhysAddr, UINT64 *pGlobalAddr);

UINT32 AmbaMAL_KrnGlobal2Phys(UINT64 GlobalAddr, UINT64 *pPhysAddr);

/* KrnCmaOps API */
UINT32 AmbaMAL_KrnCmaOpsInit(void);

void AmbaMAL_KrnCmaOpsDeInit(void);

UINT32 AmbaMAL_KrnCmaOpsRegister(AMBA_MAL_INFO_PRIV_s *pPriv, struct device_node *pNode);

void* AmbaMAL_KrnCmaOpsAlloc(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 Size, UINT64 *pPhysAddr);

void AmbaMAL_KrnCmaOpsFree(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, void *pVirtAddr, UINT64 RealSize);

/* KrnAmaOps API */
UINT32 AmbaMAL_KrnAmaOpsRegister(AMBA_MAL_INFO_PRIV_s *pPriv, struct device_node *pNode);

void* AmbaMAL_KrnAmaOpsAlloc(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 Size, UINT64 *pPhysAddr);

void AmbaMAL_KrnAmaOpsFree(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, void *pVirtAddr, UINT64 RealSize);

/* KrnMmb API */
void AmbaMAL_KrnMmbInit(AMBA_MAL_INFO_PRIV_s *pPriv);

UINT32 AmbaMAL_KrnMmbAdd(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, void *pVirtAddr, UINT64 Size, void *Owner);

UINT32 AmbaMAL_KrnMmbRelease(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, UINT64 Size);

UINT32 AmbaMAL_KrnMmbReference(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, UINT64 Size, AMBA_MAL_MMB_s **ppMmb);

UINT32 AmbaMAL_KrnMmbUnReference(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, UINT64 Size, UINT32 *pRlease, AMBA_MAL_MMB_s *pRleaseMmb);

void AmbaMAL_KrnMmbReleaseByOwner(AMBA_MAL_INFO_PRIV_s *pPriv, void *Owner, void (*ReleaseCb)(AMBA_MAL_INFO_PRIV_s *pReleasePriv, AMBA_MAL_MMB_s *pReleaseMmb));

UINT32 AmbaMAL_KrnMmbP2V(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, void **ppVirtAddr);

UINT32 AmbaMAL_KrnMmbV2P(AMBA_MAL_INFO_PRIV_s *pPriv, void *pVirtAddr, UINT64 *pPhysAddr);

void AmbaMAL_KrnMmbDump(AMBA_MAL_INFO_PRIV_s *pPriv, struct seq_file *m);

void AmbaMAL_KrnMmbGetUsedSize(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 *pUsedSize);

/* KrnCma API */
UINT32 AmbaMAL_KrnCmaAlloc(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 Size, UINT32 Align, UINT64 *pPhysAddr, UINT64 *pRealSize, void *Owner);

UINT32 AmbaMAL_KrnCmaFree(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, UINT64 RealSize);

UINT32 AmbaMAL_KrnCmaMap(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, UINT64 Size, UINT64 Attri, void **ppVirtAddr);

UINT32 AmbaMAL_KrnCmaUnMap(AMBA_MAL_INFO_PRIV_s *pPriv, void *pVirtAddr, UINT64 Size);

UINT32 AmbaMAL_KrnCmaMapUser(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, UINT64 Size, UINT64 Attri, struct vm_area_struct *vma);

void AmbaMAL_KrnCmaReleaseCb(AMBA_MAL_INFO_PRIV_s *pPriv, AMBA_MAL_MMB_s *pMmb);

/* KrnDedicate API */
UINT32 AmbaMAL_KrnDedicateMap(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, UINT64 Size, UINT64 Attri, void **ppVirtAddr);

UINT32 AmbaMAL_KrnDedicateUnMap(AMBA_MAL_INFO_PRIV_s *pPriv, void *pVirtAddr, UINT64 Size);

/* KrnCache API */
UINT32 AmbaMAL_KrnCacheClean(AMBA_MAL_INFO_PRIV_s *pPriv, void *pVirtAddr, UINT64 Size);

UINT32 AmbaMAL_KrnCacheInvalid(AMBA_MAL_INFO_PRIV_s *pPriv, void *pVirtAddr, UINT64 Size);

#endif  //AMBAMAL_KRNPRIV_H
