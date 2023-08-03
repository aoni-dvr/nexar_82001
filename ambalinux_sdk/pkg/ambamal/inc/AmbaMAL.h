#ifndef  AMBAMAL_H
#define  AMBAMAL_H

#if defined(CONFIG_THREADX) || defined(CONFIG_QNX) || defined(CONFIG_LINUX)
#include "AmbaTypes.h"
#else
typedef signed char         INT8;           /* 8 bits, [-128, 127] */
typedef short               INT16;          /* 16 bits */
typedef int                 INT32;          /* 32 bits */
typedef long long           INT64;          /* 64 bits */

typedef unsigned char       UINT8;          /* 8 bits, [0, 255] */
typedef unsigned short      UINT16;         /* 16 bits */
typedef unsigned int        UINT32;         /* 32 bits */
typedef unsigned long long  UINT64;         /* 64 bits */

typedef unsigned short      WCHAR;          /* 16 bits */
typedef long unsigned int   SIZE_t;

typedef double              DOUBLE;
typedef long                LONG;
typedef unsigned long       ULONG;
#endif

#define AMBA_MAL_VERSION                   1U                 /**< AmbaMal memory version */

/* AmbaMAL Memory Type */
#define AMBA_MAL_TYPE_INVALID              0x0U               /**< AmbaMal memory type invalidate */
#define AMBA_MAL_TYPE_DEDICATED            0x1U               /**< AmbaMal memory type DEDICATED */
#define AMBA_MAL_TYPE_ALLOCATABLE          0x2U               /**< AmbaMal memory type ALLOCATABLE */

/* AmbaMAL Memory Capability */
#define AMBA_MAL_CAPLTY_INVALID            0x0U               /**< AmbaMal memory capability which is invalidate */
#define AMBA_MAL_CAPLTY_CAN_MAP            0x1U               /**< AmbaMal memory capability which can call map */
#define AMBA_MAL_CAPLTY_CAN_UNMAP          0x2U               /**< AmbaMal memory capability which can call unmap */
#define AMBA_MAL_CAPLTY_CAN_ALLOCATE       0x4U               /**< AmbaMal memory capability which can call allocate */
#define AMBA_MAL_CAPLTY_CAN_FREE           0x8U               /**< AmbaMal memory capability which can call free */
#define AMBA_MAL_CAPLTY_CAN_CACHE_OPS      0x10U              /**< AmbaMal memory capability which can call cache operation */
#define AMBA_MAL_CAPLTY_CAN_ADDR_TRANS     0x20U              /**< AmbaMal memory capability which can call address translation */

/* AmbaMAL Memory ATTRI */
#define AMBA_MAL_ATTRI_UNKNOW              0x0U               /**< AmbaMal memory attribute unknow */
#define AMBA_MAL_ATTRI_CACHE               0x1U               /**< AmbaMal memory attribute cache */
#define AMBA_MAL_ATTRI_NON_CACHE           0x2U               /**< AmbaMal memory attribute non-cache */

/* AmbaMAL Memory ID */
#define AMBA_MAL_ID_CV_FD                  0x0U               /**< AmbaMal memory id CV FD */
#define AMBA_MAL_ID_CV_FD_SHARE            0x1U               /**< AmbaMal memory id CV FD SHARE*/
#define AMBA_MAL_ID_CV_SCHDR               0x2U               /**< AmbaMal memory id CV SCHDR */
#define AMBA_MAL_ID_DSP_DATA               0x3U               /**< AmbaMal memory id DSP DATA */
#define AMBA_MAL_ID_CV_ARMINT              0x4U               /**< AmbaMal memory id CV ARMINT */
#define AMBA_MAL_ID_CV_SDG_IDSP            0x5U               /**< AmbaMal memory id CV SDG IDSP */
#define AMBA_MAL_ID_CV_SDG                 0x6U               /**< AmbaMal memory id CV SDG */
#define AMBA_MAL_ID_IO                     0x7U               /**< AmbaMal memory id IO */

#define AMBA_MAL_ID_MAX                    0x10U              /**< AmbaMal memory id max */

/* AmbaMAL Memory ID legacy*/
#define AMBA_MAL_ID_CV_SYS                  AMBA_MAL_ID_CV_FD
#define AMBA_MAL_ID_CV_RTOS                 AMBA_MAL_ID_CV_FD_SHARE

/**
 * @brief AmbaMAL memory info structure.
 */
typedef struct {
    UINT32 Id;                                                /**< AmbaMAL Memory ID */
    UINT32 Type;                                              /**< AmbaMAL Memory Type */
    UINT32 Capability;                                        /**< AmbaMAL Memory Capability */
    UINT64 PhysAddr;                                          /**< AmbaMal memory physical address */
    UINT64 GlobalAddr;                                        /**< AmbaMal memory global address */
    UINT64 Size;                                              /**< AmbaMal memory size */
    UINT32 Rsv[7];                                            /**< Reserved */
} AMBA_MAL_INFO_s;

/**
 * @brief AmbaMAL memory buffer structure.
 */
typedef struct {
    UINT32 Id;                                                /**< AmbaMAL Memory ID */
    UINT64 PhysAddr;                                          /**< AmbaMal memory physical address */
    UINT64 GlobalAddr;                                        /**< AmbaMal memory global address */
    UINT64 Size;                                              /**< AmbaMal memory request size */
    UINT64 RealSize;                                          /**< AmbaMal memory real get size */
    UINT32 Align;                                             /**< AmbaMal memory alignment */
    UINT32 Rsv[6];                                            /**< Reserved */
} AMBA_MAL_BUF_s;

/**
 *  @brief      Init AmbaMal.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Init(void);

/**
 *  @brief      Allocate buffer from AmbaMAL Memory ID (Only for AMBA_MAL_TYPE_ALLOCATABLE).
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      Size : [IN] Size to allocate.
 *
 *  @param      Align : [IN] Base and Size to align.
 *
 *  @param      pBuf : [OUT] Return buffer structure.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Alloc(UINT32 Id, UINT64 Size, UINT32 Align, AMBA_MAL_BUF_s *pBuf);

/**
 *  @brief      Map buffer to get virtual address for CPU used.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      PhysAddr : [IN] Physical address.
 *
 *  @param      Size : [IN] Size to map.
 *
 *  @param      Attri : [IN] Attribute of mmap.
 *
 *  @param      ppVirtAddr : [OUT] Return virtual address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Map(UINT32 Id, UINT64 PhysAddr, UINT64 Size, UINT64 Attri, void **ppVirtAddr);

/**
 *  @brief      UnMap buffer.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      pVirtAddr : [IN] Virtual address.
 *
 *  @param      Size : [IN] Size to unmap.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Unmap(UINT32 Id, void *pVirtAddr, UINT64 Size);

/**
 *  @brief      Free buffer to AmbaMAL Memory ID (Only for AMBA_MAL_TYPE_ALLOCATABLE).
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      pBuf : [IN] Buffer structure.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Free(UINT32 Id, AMBA_MAL_BUF_s *pBuf);

/**
 *  @brief      Data cache clean.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      pVirtAddr : [IN] Virtual address to clean.
 *
 *  @param      Size : [IN] Size to clean.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_CacheClean(UINT32 Id, void *pVirtAddr, UINT64 Size);

/**
 *  @brief      Data cache invalidate.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      pVirtAddr : [IN] Virtual address to invalidate.
 *
 *  @param      Size : [IN] Size to invalidate.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_CacheInvalid(UINT32 Id, void *pVirtAddr, UINT64 Size);

/**
 *  @brief      Get AmbaMAL Memory info by ID.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      pInfo : [OUT] AmbaMAL memory info structure of AmbaMAL Memory ID.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_GetInfo(UINT32 Id, AMBA_MAL_INFO_s *pInfo);

/**
 *  @brief      Get AmbaMAL Memory info by Physical address.
 *
 *  @param      PhysAddr : [IN] Physical address.
 *
 *  @param      pInfo : [OUT] AmbaMAL memory info structure of AmbaMAL Physical address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_GetInfoByPhys(UINT64 PhysAddr, AMBA_MAL_INFO_s *pInfo);

/**
 *  @brief      Physical to virtual address.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      PhysAddr : [IN] Physical address.
 *
 *  @param      Attri : [IN] AmbaMAL Memory ATTRI.
 *
 *  @param      ppVirtAddr : [OUT] Virtual address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Phys2Virt(UINT32 Id, UINT64 PhysAddr, UINT32 Attri, void **ppVirtAddr);

/**
 *  @brief      Virtual to physical address.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      pVirtAddr : [IN] Virtual address.
 *
 *  @param      pPhysAddr : [OUT] Physical address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Virt2Phys(UINT32 Id, void *pVirtAddr, UINT64 *pPhysAddr);

/**
 *  @brief      Physical to global address.
 *
 *  @param      PhysAddr : [IN] Physical address.
 *
 *  @param      pGlobalAddr : [OUT] Global address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Phys2Global(UINT64 PhysAddr, UINT64 *pGlobalAddr);

/**
 *  @brief      Global to physical address.
 *
 *  @param      GlobalAddr : [IN] Global address.
 *
 *  @param      pPhysAddr : [OUT] Physical address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Global2Phys(UINT64 GlobalAddr, UINT64 *pPhysAddr);

/**
 *  @brief      Dump memory list by Id.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @return     0            on success.    \n
 */
void AmbaMAL_Dump(UINT32 Id);

#endif  //AMBAMAL_H
