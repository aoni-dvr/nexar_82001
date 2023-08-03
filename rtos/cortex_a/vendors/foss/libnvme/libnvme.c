/*
 * Copyright (c) 2017, Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 * Please see COPYING file for license text.
 */

#include "AmbaTypes.h"
#include "AmbaCache.h"
#include "AmbaPrint.h"
#include "nvme.h"
#include "nvme_internal.h"
#include "libnvme.h"
#include "AmbaWrap.h"

static nvme_perf_t nt = {
      .ns_id = 1,
      .qd = 2,
      .io_size = 512 * 1024,
};

UINT32 LibNVME_Enable(LIBNVME_ENABLE_PARM_s *parm)
{
    struct nvme_ctrlr_opts opts;
    struct nvme_ctrlr_stat cstat;
    struct nvme_ns_stat nsstat;
    struct nvme_qpair_stat qpstat;
    nvme_sys_config_s config = {
        //.StackCacheMemory        = parm->StackCacheMemory,
        //.StackCacheMemorySize    = parm->StackCacheMemorySize,
        .StackNonCacheMemory     = parm->StackNonCacheMemory,
        .StackNonCacheMemorySize = parm->StackNonCacheMemorySize,
    };
    int i, ret;
    nvme_perf_io_t *io;

    // create memory pool that inbound for external device
    nvme_mem_pool_init(&config);

    /* Initialize the controller options */
    memset(&opts, 0, sizeof(struct nvme_ctrlr_opts));
    opts.io_queues = 1;

    /* Prove NVMe controller */
    AmbaPrint_PrintStr5("Opening NVMe controller", 0, 0, 0, 0, 0);
    nt.ctrlr = nvme_ctrlr_open(NULL, &opts);
    if (!nt.ctrlr) {
        AmbaPrint_PrintStr5("Open NVMe controller failed", 0 ,0, 0, 0, 0);
        return -1;
    }

    /* Get information */
    if (nvme_ctrlr_stat(nt.ctrlr, &cstat) != 0) {
        AmbaPrint_PrintStr5("Get NVMe controller %s info failed", 0, 0, 0, 0, 0);
        return -1;;
    }

    AmbaPrint_PrintUInt5("[SSD] Max transfer size: %d bytes", cstat.max_xfer_size, 0, 0, 0, 0);

    nt.nr_ns = cstat.nr_ns;
    nt.max_qd = cstat.max_qd;

    if (cstat.io_qpairs != opts.io_queues) {
        AmbaPrint_PrintUInt5("Number of IO qpairs limited to %d\n", cstat.io_qpairs, 0, 0, 0, 0);
    }

    /* Open the name space */
    nt.ns = nvme_ns_open(nt.ctrlr, nt.ns_id);
    if (!nt.ns) {
        AmbaPrint_PrintUInt5("Open NVMe controller name space %d failed\n",nt.ns_id, 0, 0, 0, 0);
        return -1;
    }

    if (nvme_ns_stat(nt.ns, &nsstat) != 0) {
        AmbaPrint_PrintUInt5("Get name space %u info failed\n", nt.ns_id, 0, 0, 0, 0);
        return -1;
    }

    nt.sectsize = nsstat.sector_size;
    AmbaPrint_PrintUInt5("[SSD] Sector size: %d", nt.sectsize, 0, 0, 0, 0);
    nt.nr_sectors = nsstat.sectors;
    AmbaPrint_PrintUInt5("[SSD] Sector number: %d", nt.nr_sectors, 0, 0, 0, 0);

    if (nt.io_size % nt.sectsize) {
        AmbaPrint_PrintUInt5("Invalid I/O size %d : must be a multiple of the sector size %d",
            nt.io_size,
            nt.sectsize,
            0,
            0,
            0);
        return -1;
    }

    if (nt.max_qd < (unsigned int)nt.qd) {
        AmbaPrint_PrintUInt5("Queue depth has to be less than the maximum queue entries authorized (%d)",
            nt.max_qd, 0, 0, 0, 0);
        return -1;
    }

    /* Get an I/O queue pair */
    nt.qpair = nvme_ioqp_get(nt.ctrlr, 0, 0);
    if (!nt.qpair) {
        AmbaPrint_PrintUInt5("Allocate I/O qpair failed", 0, 0, 0, 0, 0);
        return -1;
    }

    ret = nvme_qpair_stat(nt.qpair, &qpstat);
    if (ret) {
        AmbaPrint_PrintUInt5("Get I/O qpair information failed", 0, 0, 0, 0, 0);
        return -1;
    }
    AmbaPrint_PrintUInt5("[SSD] Qpair %d, depth: %d", qpstat.id, qpstat.qd, 0, 0, 0);

    /* Allocate I/Os */
    nt.io = calloc(nt.qd, sizeof(nvme_perf_io_t));
    if (!nt.io) {
        AmbaPrint_PrintUInt5("Allocate I/O array failed", 0, 0, 0, 0, 0);
        return -1;
    }

    /* Allocate I/O buffers */
    for (i = 0; i < nt.qd; i++) {
        io = &nt.io[i];
        io->size = nt.io_size / nt.sectsize;
        io->buf = nvme_zmalloc(nt.io_size, nt.sectsize); // non-cache
        if (!io->buf) {
            AmbaPrint_PrintUInt5("io buffer allocation failed", 0, 0, 0, 0, 0);
            return -1;
        }
        nvme_perf_ioq_add(&nt.free_ioq, io);
    }

    AmbaPrint_PrintUInt5("AmbaNVME_LibEnable(): Done", 0, 0, 0, 0, 0);

    return 0;
}

static void
nvme_perf_read_end(void *arg,
         const struct nvme_cpl *cpl)
{
    nvme_perf_io_t *io = arg;

    nvme_perf_ioq_remove(&nt.pend_rq, io);
    nvme_perf_ioq_add(&nt.free_ioq, io);

    nt.io_count++;
    nt.read_count++;
    nt.read_bytes += nt.io_size;
}

static void
nvme_perf_write_end(void *arg,
         const struct nvme_cpl *cpl)
{
    nvme_perf_io_t *io = arg;

    // create read/write queue to let user can request read/write simultaneously
    nvme_perf_ioq_remove(&nt.pend_wq, io);
    nvme_perf_ioq_add(&nt.free_ioq, io);

    nt.io_count++;
    nt.write_count++;
    nt.write_bytes += nt.io_size;
}

UINT32 LibNVME_Read(UINT8 *pBuf, UINT32 sector, UINT32 sectors)
{
    nvme_perf_io_t *io;
    UINT32 ret, check_misalign, read_times = 1, read_left = 0, read_count;
    static UINT32 Misalign = 0U;

    io = (nvme_perf_io_t *) nvme_perf_ioq_get(&nt.free_ioq);

    if (io != NULL) {
        void  *tmp_buf;
        size_t rs = sectors * nt.sectsize;

        nvme_perf_ioq_add(&nt.pend_rq, io);

        io->ofst = sector;
        io->size = sectors;

        ret = pBuf;
        check_misalign = (ret & ((UINT32) CACHE_LINE_SIZE - 1U));

        if(rs >= 128 * 1024 && Misalign == 0U && check_misalign){
            Misalign = 1U;
            AmbaPrint_PrintUInt5("LibNVME_Read: The address of pBuf 0x%x is not 64 byte alignment. Use memcpy would impact performance severel. To reduce overhead, this message would be shown one time.", ret, 0, 0, 0, 0);
        }

        if(check_misalign == 0U) {
            tmp_buf = io->buf; // save data buffer
            io->buf = pBuf; // to avoid memcpy, replace with caller's buffer (cache-memory).

            // should invalidate buffer before submit the request
            // or, the data won't be updated.
            AmbaCache_DataInvalidate((ULONG)(io->buf), rs);

            /* Submit a I/O reqeust */
            ret = nvme_ns_read(nt.ns, nt.qpair,
                       io->buf,
                       io->ofst,//start lba
                       io->size,//lba count
                       nvme_perf_read_end, io, 0);

            if (ret) {
                AmbaPrint_PrintUInt5("Submit I/O failed", 0, 0, 0, 0, 0);
                nvme_perf_ioq_remove(&nt.pend_rq, io);
                nvme_perf_ioq_add(&nt.free_ioq, io);
                nt.abort = 1;
                return -1;
            }
            /* Poll for completion */
            while (!nvme_perf_ioq_empty(&nt.pend_rq))
                nvme_ioqp_poll(nt.qpair, nt.qd);

            // restore data buffer
            io->buf = tmp_buf;
        } else {
            if (rs > nt.io_size) {
                read_times = rs / nt.io_size;
                read_left = rs % nt.io_size;
                if (read_left != 0U) {
                    read_times += 1U;
                }
                if(read_left == 0U && rs != 0U) {
                    read_left = nt.io_size;
                }
                // To get better thruput, buffer and size is determined by the caller
                //AmbaPrint_PrintUInt5("Request size %d > io max size %d", rs, nt.io_size, 0, 0, 0);
            } else {
                read_left = rs;
            }

            read_count = nt.io_size / nt.sectsize;
            for (UINT32 i = 0U; i < read_times; i++) {
                if(i == (read_times - 1)) {
                    read_count = read_left / nt.sectsize;
                }
                /* Submit a I/O reqeust */
                ret = nvme_ns_read(nt.ns, nt.qpair,
                           io->buf,
                           io->ofst + i * read_count,//start lba
                           read_count,//lba count
                           nvme_perf_read_end, io, 0);

                if (ret) {
                    AmbaPrint_PrintUInt5("Submit I/O failed", 0, 0, 0, 0, 0);
                    nvme_perf_ioq_remove(&nt.pend_rq, io);
                    nvme_perf_ioq_add(&nt.free_ioq, io);
                    nt.abort = 1;
                    return -1;
                }
                /* Poll for completion */
                while (!nvme_perf_ioq_empty(&nt.pend_rq))
                    nvme_ioqp_poll(nt.qpair, nt.qd);

                // Copy the data from io->buf to pBuf
                AmbaWrap_memcpy(pBuf + i * nt.io_size, io->buf, read_count * nt.sectsize);
            }
        }

    } else {
        AmbaPrint_PrintUInt5("AmbaNVME_LibRead(): No free space for io request", 0, 0, 0, 0, 0);
    }

    return 0;
}

UINT32 LibNVME_Write(UINT8 *pBuf, UINT32 sector, UINT32 sectors)
{
    nvme_perf_io_t *io;
    UINT32 ret, check_misalign, write_times = 1, write_left = 0, write_count;
    static UINT32 Misalign = 0U;

    io = (nvme_perf_io_t *) nvme_perf_ioq_get(&nt.free_ioq);

    if (io != NULL) {
        void  *tmp_buf;
        size_t rs = sectors * nt.sectsize;

        nvme_perf_ioq_add(&nt.pend_wq, io);

        io->ofst = sector;
        io->size = sectors;

        ret = pBuf;
        check_misalign = (ret & ((UINT32) CACHE_LINE_SIZE - 1U));

        if(rs >= 128 * 1024 && Misalign == 0U && check_misalign){
            Misalign = 1U;
            AmbaPrint_PrintUInt5("LibNVME_Write: The address of pBuf 0x%x is not 64 byte alignment. Use memcpy would impact performance severel. To reduce overhead, this message would be shown one time.", ret, 0, 0, 0, 0);
        }

        if(check_misalign == 0U) {
            tmp_buf = io->buf; // save data buffer
            io->buf = pBuf; // to avoid memcpy, replace with caller's buffer.
            AmbaCache_DataFlush((ULONG)io->buf, rs); // Flush cache before submit the write request

            /* Submit a I/O reqeust */
            ret = nvme_ns_write(nt.ns, nt.qpair,
                       io->buf,
                       io->ofst,//start lba
                       io->size,//lba count
                       nvme_perf_write_end, io, 0);

            if (ret) {
                AmbaPrint_PrintUInt5("Submit I/O failed", 0, 0, 0, 0, 0);
                nvme_perf_ioq_remove(&nt.pend_wq, io);
                nvme_perf_ioq_add(&nt.free_ioq, io);
                nt.abort = 1;
                return -1;
            }
            /* Poll for completion */
            while (!nvme_perf_ioq_empty(&nt.pend_wq))
                nvme_ioqp_poll(nt.qpair, nt.qd);

            // restore data buffer
            io->buf = tmp_buf;
        } else {
            if (rs > nt.io_size) {
                write_times = rs / nt.io_size;
                write_left = rs % nt.io_size;
                if (write_left != 0U) {
                    write_times += 1U;
                }
                if(write_left == 0U && rs != 0U) {
                    write_left = nt.io_size;
                }
                // To get better thruput, buffer and size is determined by the caller
                //AmbaPrint_PrintUInt5("Request size %d > io max size %d", rs, nt.io_size, 0, 0, 0);
            } else {
                write_left = rs;
            }

            write_count = nt.io_size / nt.sectsize;
            for (UINT32 i = 0U; i < write_times; i++) {
                if(i == (write_times - 1)) {
                    write_count = write_left / nt.sectsize;
                }
                // Copy data to io->buf
                AmbaWrap_memcpy(io->buf, pBuf + i * nt.io_size, write_count * nt.sectsize);

                /* Submit a I/O reqeust */
                ret = nvme_ns_write(nt.ns, nt.qpair,
                           io->buf,
                           io->ofst + i * write_count,//start lba
                           write_count,//lba count
                           nvme_perf_write_end, io, 0);

                if (ret) {
                    AmbaPrint_PrintUInt5("Submit I/O failed", 0, 0, 0, 0, 0);
                    nvme_perf_ioq_remove(&nt.pend_wq, io);
                    nvme_perf_ioq_add(&nt.free_ioq, io);
                    nt.abort = 1;
                    return -1;
                }
                /* Poll for completion */
                while (!nvme_perf_ioq_empty(&nt.pend_wq))
                    nvme_ioqp_poll(nt.qpair, nt.qd);
            }
        }

    } else {
        AmbaPrint_PrintUInt5("AmbaNVME_LibWrite(): No free space for io request", 0, 0, 0, 0, 0);
    }

    return 0;
}

UINT32 LibNVME_Info(LIBNVME_INFO_s *info)
{
    memset(info, 0, sizeof(LIBNVME_INFO_s));
    info->Lba        = nt.nr_sectors;
    info->SectorSize = nt.sectsize;

    return 0;
}
