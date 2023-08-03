/*
 * Copyright (c) 2017, Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 * Please see COPYING file for license text.
 */

/*
 * I/O descriptor.
 */
typedef struct nvme_perf_io {

    /* For I/O queueing */
    struct nvme_perf_io *next;
    struct nvme_perf_io *prev;

    /* I/O offset, size and buffer */
    void            *buf;
    long long       ofst;
    size_t          size;

} nvme_perf_io_t;

/*
 * I/O descriptor queue.
 */
typedef struct nvme_perf_ioq {
    nvme_perf_io_t      *head;
    nvme_perf_io_t      *tail;
} nvme_perf_ioq_t;

/*
 * Run parameters.
 */
typedef struct nvme_perf {

    /*
     * Log level to run at.
     */
    int         log_level;

    /*
     * Terminate on signal.
     */
    int         abort;

    /*
     * Device and I/O parameters.
     */
    char            *path;
    int         cpu;
    int         ns_id;
    int         qd;
    int         rw;
    int         rnd;
    size_t          io_size;
    int         run_secs;
    int         memstat;

    /*
     * Device data.
     */
    //struct pci_slot_match   slot;
    //char            ctrlr_name[1024];
    size_t          sectsize;
    unsigned long long  nr_sectors;
    unsigned int        max_qd;
    unsigned int        nr_ns;

    struct nvme_ctrlr   *ctrlr;
    struct nvme_ns      *ns;
    struct nvme_qpair   *qpair;

    /*
     * I/O control.
     */
    unsigned long long  io_ofst;
    nvme_perf_io_t      *io;
    nvme_perf_ioq_t     free_ioq;
    nvme_perf_ioq_t     pend_ioq;
    nvme_perf_ioq_t     pend_rq;
    nvme_perf_ioq_t     pend_wq;

    /*
     * I/O stats.
     */
    unsigned long long  start;
    unsigned long long  end;
    unsigned long long  io_count;
    unsigned long long  io_bytes;
    unsigned long long  read_count;
    unsigned long long  read_bytes;
    unsigned long long  write_count;
    unsigned long long  write_bytes;

} nvme_perf_t;

/*
 * Test if an I/O queue is empty.
 */
static inline int nvme_perf_ioq_empty(nvme_perf_ioq_t *ioq)
{
    return ioq->head == NULL;
}

/*
 * Add an I/Os at the end of a queue.
 */
static inline void nvme_perf_ioq_add(nvme_perf_ioq_t *ioq,
                     nvme_perf_io_t *io)
{

    io->next = NULL;
    if (ioq->head) {
        ioq->tail->next = io;
        io->prev = ioq->tail;
    } else {
        ioq->head = io;
        io->prev = NULL;
    }

    ioq->tail = io;

    return;
}

/*
 * Get the first I/Os in a queue.
 */
static inline nvme_perf_io_t *nvme_perf_ioq_get(nvme_perf_ioq_t *ioq)
{
    nvme_perf_io_t *io;

    if (ioq->head) {

        io = ioq->head;
        ioq->head = io->next;
        if (ioq->head)
            ioq->head->prev = NULL;
        else
            ioq->tail = NULL;

        io->prev = NULL;
        io->next = NULL;

    } else
        io = NULL;

    return io;
}

/*
 * Remove an I/O from a queue.
 */
static inline void nvme_perf_ioq_remove(nvme_perf_ioq_t *ioq,
                    nvme_perf_io_t *io)
{
    nvme_perf_io_t *iop;

    if (ioq->head == io) {
        nvme_perf_ioq_get(ioq);
    } else if (ioq->tail == io) {
        ioq->tail = io->prev;
        ioq->tail->next = NULL;
    } else {
        iop = io->next;
        iop->prev = io->prev;
        io->prev->next = iop;
    }

    io->prev = NULL;
    io->next = NULL;
}



typedef struct {
    UINT8           *StackCacheMemory;       // [Input] static memory for nvme stack usage (cacheable)
    UINT32          StackCacheMemorySize;    // [Input] Size of static memory for nvme stack usage (cacheable)
    UINT8           *StackNonCacheMemory;    // [Input] static memory for nvme stack usage (non-cacheable)
    UINT32          StackNonCacheMemorySize; // [Input] Size of static memory for nvme stack usage (non-cacheable)
} LIBNVME_ENABLE_PARM_s;

typedef struct {
    UINT32  WriteProtect;
    UINT32  SectorSize;
    UINT32  Lba;
} LIBNVME_INFO_s;

UINT32 LibNVME_Enable(LIBNVME_ENABLE_PARM_s *parm);
UINT32 LibNVME_Read(UINT8 *pBuf, UINT32 sector, UINT32 sectors);
UINT32 LibNVME_Write(UINT8 *pBuf, UINT32 sector, UINT32 sectors);
UINT32 LibNVME_Info(LIBNVME_INFO_s *info);
