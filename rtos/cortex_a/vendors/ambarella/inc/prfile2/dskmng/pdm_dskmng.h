/*
    Copyright(C) 2005 eSOL Co., Ltd. All rights reserved.

    This software is protected by the law and the agreement concerning
    a Japanese country copyright method, an international agreement,
    and other intellectual property right and may be used and copied
    only in accordance with the terms of such license and with the inclusion
    of the above copyright notice.

    This software or any other copies thereof may not be provided
    or otherwise made available to any other person.  No title to
    and ownership of the software is hereby transferred.

    The information in this software is subject to change without
    notice and should not be construed as a commitment by eSOL Co.,Ltd.
*/
/****************************************************************************
[pdm_dskmng.h] - .
****************************************************************************/
#ifndef __PDM_DSKMNG_H__
#define __PDM_DSKMNG_H__

#include "prfile2/pdm_common.h"
#include "prfile2/pdm_uplyrif.h"
#include "dskmng/pdm_critical.h"

/****************************************************************************
  Internal Definitions and Declarations

  NOTES:
   - These definitions and declarations are only for internal use in PrFILE2;
     User don't have to use them in user applications.
****************************************************************************/
/*---------------------------------------------------------------------------
  Non Blocking Protocol
---------------------------------------------------------------------------*/
typedef int     PDM_NBC_T;

/*---------------------------------------------------------------------------
  PDM_PARTITION -
---------------------------------------------------------------------------*/
struct PDM_PARTITION {

    /* Partition status */
    PDM_U_LONG      status;

    /* Pointer to current Disk (Disk information) */
    PDM_DISK        *p_disk;

    /* Partition handle signature */
    PDM_U_LONG      signature;

    /* Partition ID */
    PDM_U_SHORT     part_id;

    /* Open partition count */
    PDM_U_SHORT     open_part_cnt;

    /* The handle of locks disk */
    PDM_PARTITION   *part_lock_handle;

    /* Start sector of current partition */
    PDM_U_LONG      start_sector;

    /* Total sector of current partition */
    PDM_U_LONG      total_sector;

    /* MBR (EPBR) sector of current partition */
    PDM_U_LONG      mbr_sector;

    /* Partition type of current partition */
    PDM_U_CHAR      partition_type;

    /* Last error code of driver */
    PDM_ERROR       driver_last_error;

    /* A pointer to the Current Volume */
    void            *p_vol;

    /* Non Block Protocol Counter for detect status */
    volatile PDM_NBC_T  nbc_detect; /* !! Do not access this value !! */

    PDM_PARTITION_EVENT_CALLBACK    event_callback;
    void                            *event_callback_param;
};

/*---------------------------------------------------------------------------
  PDM_DISK_CALLBACK -
---------------------------------------------------------------------------*/
typedef void  (*PDM_DISK_CALLBACK)(void);

/*---------------------------------------------------------------------------
  PDM_DISK -
---------------------------------------------------------------------------*/
struct PDM_DISK {

    /* Disk status */
    PDM_U_LONG          status;

    /* Driver interface */
    PDM_DISK_TBL        disk_tbl;

    /* Disk handle signature */
    PDM_U_LONG          signature;

    /* Open disk count */
    PDM_U_SHORT         open_disk_cnt;

    /* Locked count of disk */
    PDM_U_SHORT         disk_lock_cnt;

    /* The handle of locks disk */
    PDM_DISK            *disk_lock_handle;

#if PDM_CRITICAL_SECTION_ENABLE
    /* Semaphore ID for driver access */
    PDM_CRITICAL        critical;
#endif /* PDM_CRITICAL_SECTION_ENABLE */

    /* Disk information */
    PDM_DISK_INFO       disk_info;

    /* Pointer of init_disk_tbl */
    PDM_INIT_DISK       *p_init_disk_tbl;

    /* Pointer to current partition (Partition information) */
    PDM_PARTITION       *p_cur_part;

    /* Call back function to erasing block */
    PDM_DISK_CALLBACK   p_erase_func;

    /* Media insert/eject flag */
    PDM_BOOL            is_insert;  /* !! Do not access this value !! */

    /* Non Block Protocol Counter for 'is_insert' */
    volatile PDM_NBC_T  nbc;        /* !! Do not access this value !! */
    volatile PDM_NBC_T  nbc_detect; /* !! Do not access this value !! */
    volatile PDM_NBC_T  nbc_req;    /* !! Do not access this value !! */
};

/*---------------------------------------------------------------------------
  PDM_PARTITION_HANDLE -
---------------------------------------------------------------------------*/
typedef struct {

    /* Partition handle signature */
    PDM_U_LONG      signature;

    /* Partition handle pointer */
    PDM_PARTITION   *handle;

} PDM_PARTITION_HANDLE;


/*---------------------------------------------------------------------------
  PDM_DISK_HANDLE -
---------------------------------------------------------------------------*/
typedef struct {

    /* Disk handle signature */
    PDM_U_LONG      signature;

    /* Disk handle pointer */
    PDM_DISK        *handle;

} PDM_DISK_HANDLE;


/*---------------------------------------------------------------------------
  PDM_DISK_SET -
---------------------------------------------------------------------------*/
typedef struct {

    /* Number of total attached partition */
    PDM_U_SHORT             num_partition;

    /* Number of allocated disk struct */
    PDM_U_SHORT             num_allocated_disk;

    /* Disk handle information */
    PDM_DISK_HANDLE         disk_handle[PDM_MAX_DISK];

    /* Partition handle information */
    PDM_PARTITION_HANDLE    partition_handle[PDM_MAX_PARTITION];

    /* Disk information */
    PDM_DISK                disk[PDM_MAX_DISK];

    /* Partition information */
    PDM_PARTITION           partition[PDM_MAX_PARTITION];

#if PDM_CRITICAL_SECTION_ENABLE
    /* Semaphore ID for DiskManager access */
    PDM_CRITICAL            critical;
#endif /* PDM_CRITICAL_SECTION_ENABLE */

} PDM_DISK_SET;

extern PDM_DISK_SET pdm_disk_set;


/****************************************************************************
  Macro definitions
****************************************************************************/
/*---------------------------------------------------------------------------
  Block size of DiskManager
---------------------------------------------------------------------------*/
#define PDM_BLOCK_SIZE                      (512)


/*---------------------------------------------------------------------------
  Handle name identifier
---------------------------------------------------------------------------*/
#define PDM_DISK_HANDLE_ID                  (0x00000300UL)
#define PDM_PARTITION_HANDLE_ID             (0x00000400UL)

#define PDM_HANDLE_NONE                     (NULL)

#define PDM_HANDLE_NO_MASK                  (0x000000ffUL)
#define PDM_HANDLE_ID_MASK                  (0x0000ff00UL)
#define PDM_HANDLE_SIG_MASK                 (0x0000ffffUL)
#define PDM_HANDLE_SIG_MAX                  (0x0000ffffUL)
#define PDM_HANDLE_SIG_SHIFT                (16)

#define PDM_HANDLE_DISK_BUILD(disk_no)                                      \
    (((disk_no) & PDM_HANDLE_NO_MASK) |                                     \
      PDM_DISK_HANDLE_ID              |                                     \
     ((pdm_disk_set.disk[(disk_no)].signature & PDM_HANDLE_SIG_MASK)        \
      << PDM_HANDLE_SIG_SHIFT))

#define PDM_HANDLE_DISK_REBUILD(disk_no)                                    \
    (&(pdm_disk_set.disk[(disk_no)]))

#define PDM_HANDLE_PARTITION_BUILD(part_no)                                 \
    (((part_no) & PDM_HANDLE_NO_MASK) |                                     \
     PDM_PARTITION_HANDLE_ID          |                                     \
     ((pdm_disk_set.partition[(part_no)].signature & PDM_HANDLE_SIG_MASK)   \
      << PDM_HANDLE_SIG_SHIFT))

#define PDM_HANDLE_PARTITION_REBUILD(part_no)                               \
    (&(pdm_disk_set.partition[(part_no)]))

/*---------------------------------------------------------------------------
  Calculation of block size
---------------------------------------------------------------------------*/
#define PDM_DIVIDE_WITH_BLOCK_SIZE(a)       ((a) >> 9)

#define PDM_CONVERT_TO_BLOCK_SIZE(a, b) \
                if ((b) == 2) \
                { \
                    *(a) = (*(a) << 1); \
                } \
                else if ((b) == 4) \
                { \
                    *(a) = (*(a) << 2); \
                } \
                else if ((b) == 8) \
                { \
                    *(a) = (*(a) << 3); \
                } \
                else \
                { \
                    \
                } \

#define PDM_CONVERT_TO_SPECIFIED_SIZE(a, b) \
                if ((b) == 2) \
                { \
                    *(a) = (*(a) >> 1); \
                } \
                else if ((b) == 4) \
                { \
                    *(a) = (*(a) >> 2); \
                } \
                else if ((b) == 8) \
                { \
                    *(a) = (*(a) >> 3); \
                } \
                else \
                { \
                    \
                } \

#define PDM_MULTIPLICATION(a, b)            ((a) << (b))

#define PDM_DIVISION(a, b)                  ((a) >> (b))


#endif  /* __PDM_DSKMNG_H__ */
