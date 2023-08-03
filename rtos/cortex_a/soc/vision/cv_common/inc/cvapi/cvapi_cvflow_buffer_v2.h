/*
 * Copyright (c) 2017-2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use,
 * reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed
 * license agreement or nondisclosure agreement with Ambarella International LP
 * or its authorized affiliates. In the absence of such an agreement, you agree
 * to promptly notify and return this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CVAPI_CVFLOW_BUFFER_V2_H
#define CVAPI_CVFLOW_BUFFER_V2_H
#ifdef ORCVIS
#include <cvtask_interface.h>
#else
#include <cvtask_ossrv.h>
#endif

/*******************************************************************************
 *                                          ~DIAGRAM OF HEADER STRUCTURE~
 *
 *             +--cvflow_header_t--+
 *             |         N         |
 *             +-------------------+
 *             |       MAX_M       |
 *             +-------------------+
 *             |                   |
 *             |         o+--------------------------------------+
 *             |                   |                             |
 *             +-------------------+                             |
 *             |                   |                             |
 *  +-------------------+o         |                             |
 *  |          |                   |                             |
 *  |          +-------------------+                             |
 *  |          |              # = M|                             |
 *  |          |    +---+o         |                             |
 *  |          |    |              |                             |
 *  |          |    |    o+--------------+                       |
 *  |          |    |              |     |                       |
 *  |          |    |    o         |     |                       |
 *  |          |    |              |     |                       |
 *  |          |    |   ...        |     |                       |
 *  |          |    |              |     |                       |
 *  |          +-------------------+     |                       |
 *  |               |                    |                       |
 *  |               |                    |                       v
 *  |           +   +-->---------------+ +->---------------+     +-------------+
 *  |           |       |payload_unit_t|    |payload_unit_t|     |buffer_info_t|
 *  |           |       +--------------+    +--------------+     +-------------+
 *  |  packing  |       +--------------+    +--------------+     +-------------+
 *  | dimension |       |payload_unit_t|    |payload_unit_t| ... |buffer_info_t|
 *  |   # = N   |       +--------------+    +--------------+     +-------------+
 *  |           |       +--------------+    +--------------+     +-------------+
 *  |           |       |payload_unit_t|    |payload_unit_t|     |buffer_info_t|
 *  |           v       +--------------+    +--------------+     +--------------
 *  |                           .                   .        .          .
 *  |                           .                   .         .         .
 *  |                           .                   .          .        .
 *  |
 *  |
 *  |
 *  +------------------>+-batch_info_t-+ +->+-batch_info_t-+
 *                      |              | |  |              |
 *                      |       o+-------+  |       o      | ...
 *                      |              |    |              |
 *                      +--------------+    +--------------+
 *                      |      K_0     |    |      K_1     |
 *                      +--------------+    +--------------+
 *                      |       # = K_0|    |       # = K_1|
 *                      |              |    |              |
 *                      |   +-------+  |    |   +-------+  |
 *                      |   |o_arg_t|  |    |   |o_arg_t|  |
 *                      |   +-------+  |    |   +-------+  |
 *                      |   +-------+  |    |   +-------+  |
 *                      |   |o_arg_t|  |    |   |o_arg_t|  |
 *                      |   +-------+  |    |   +-------+  |
 *                      |      ...     |    |      ...     |
 *                      |              |    |              |
 *                      +--------------+    +--------------+
 *
 *                      +--------------------------------------->
 *                                        batch
 *                                      dimension
 *                                        # = M
 *
 *******************************************************************************
 */

#define HEADER_VERSION_NUMBER (2U)
#define INVALID_OPT_ARG (0xFFFFFFFFU)
#define INVALID_TRACKING_ID (-1U)
#define ALIAS_MAX_LENGTH (96U)

// Structure containing per-buffer information
typedef struct {
    rptr32_t next_ptr;            // relative pointer to next buffer_info_t
    uint32_t pdim[4];             // pitched dimensions
    uint32_t dim[4];              // dimensions
    int32_t df[4];                // data format
    int32_t tracking_id;          // primitive node ID
    char name[NAME_MAX_LENGTH];   // VAS name of buffer
    char alias[ALIAS_MAX_LENGTH]; // CnnGen name of buffer
} cvflow_buffer_info_t;

// Structure containing per-iteration information
typedef struct {
    uint32_t key;
    uint32_t value;
} cvflow_o_arg_t;
typedef struct {
    rptr32_t next_ptr;    // relative pointer to next cvflow_batch_info_t
    uint32_t num_kv;      // number of key-value pairs
    cvflow_o_arg_t kvs[]; // size: num_kv
} cvflow_batch_info_t;

// Structure containing payload-specific information
typedef struct {
    rptr32_t payload_ptr;         // pointer to payload
    uint32_t pitch;               // mutable pitch of buffer
} cvflow_payload_unit_t;

// Header
typedef struct {
    uint32_t version;
    uint32_t num_io;
    uint32_t max_batch_cnt;  // maximum batch size
    rptr32_t buf_info;       // pointer to cvflow_buffer_info_t
    rptr32_t batch_info;     // pointer to cvflow_batch_info_t
    rptr32_t payload_ptrs[]; // array of pointers to array of payload_unit_t
} cvflow_header_t;

/* Functions for header allocation/initialization */
/**
 * header_get_size()
 *
 * This calculates the total memory footprint of a header.
 *
 * @param num_io        number of buffers
 * @param batch_cnt     maximum batch size
 * @param io_sizes      array containing sizes of buffers (length: num_io)
 * @param num_kvs       array containing no. of key-value pairs in per-iteration
 *                      information (length: batch_cnt) (NULL is ignored)
 *
 * @return              size of header in no. of bytes
 *
 */
static inline uint32_t header_get_size(
    uint32_t num_io, uint32_t batch_cnt,
    const uint32_t *io_sizes, const uint32_t *num_kvs);
/**
 * header_init()
 *
 * This initializes the memory to which `hdr' points into a traversable header.
 * The size of the provided memory must be at least as big as the size given by
 * header_get_size(). There is no guarantee as to the order in which structures
 * are packed.
 *
 * @param hdr           pointer to header
 * @param num_io        number of buffers
 * @param batch_cnt     batch size
 * @param io_sizes      array containing sizes of buffers (length: num_io)
 * @param num_kvs       array containing no. of key-value pairs in per-iteration
 *                      information (length: batch_cnt) (NULL is ignored)
 *
 * @return              size of header in no. of bytes
 *
 */
static inline errcode_enum_t header_init(cvflow_header_t *hdr,
        uint32_t num_io, uint32_t batch_cnt,
        const uint32_t *io_sizes, const uint32_t *num_kvs);

/* Functions for parsing information */
/**
 * header_get_batch_size()
 *
 * This probes the header for a batch size. The batch size comes from either
 * external input (e.g. recv_raw struct) or from the 4th dimension of payload.
 *
 * If an error occurred, the return value is set to 0.
 *
 * @param hdr           pointer to header
 * @param default_size  default batch size from, for example, recv_raw struct
 *
 * @return              the consolidated batch size
 */
static inline uint32_t header_get_batch_size(cvflow_header_t *hdr, uint32_t default_size);
/**
 * header_get_count()
 *
 * This gives the number of buffers in the header.
 *
 * @param hdr           pointer to header
 *
 * @return              no. of buffers
 */
static inline uint32_t header_get_count(cvflow_header_t *hdr);
/**
 * header_get_payload
 *
 * This gives the absolute pointer to a payload.
 *
 * 0 is returned upon an error.
 *
 * @param hdr           pointer to header
 * @param io_idx        index into `num_io' buffers
 * @param batch_idx     index into `batch_cnt' iterations (a.k.a. a batch)
 *
 * @return              absolute pointer to payload
 */
#ifdef ORCVIS
static inline rptr32_t header_get_payload(cvflow_header_t *hdr,
        uint32_t io_idx, uint32_t batch_idx);
#else /* RTOS: ARM64 or ARM32 */
static inline uintptr_t header_get_payload(cvflow_header_t *hdr,
        uint32_t io_idx, uint32_t batch_idx);
#endif
/**
 * header_get_payload_pitch()
 *
 * This gives the pitch of a payload, as specified from the payload-specific
 * section. The optional argument section is not searched.
 *
 * 0 is returned upon an error.
 *
 * @param hdr           pointer to header
 * @param io_idx        index into `num_io' buffers
 * @param batch_idx     index into `batch_cnt' iterations (a.k.a. a batch)
 *
 * @return              pitch of specified payload
 */
static inline uint32_t header_get_payload_pitch(cvflow_header_t *hdr,
        uint32_t io_idx, uint32_t batch_idx);
/**
 * header_find_opt_arg()
 *
 * This searches through the optional argument section. Pointer to result is
 * returned.
 *
 * If not found, NULL is returned.
 *
 * @param hdr           pointer to header
 * @param key           ID of optional argument
 * @param batch_idx     index into `batch_cnt' iterations (a.k.a. a batch)
 *
 * @return              pointer to found optional argument
 */
static inline uint32_t *header_find_opt_arg(cvflow_header_t *hdr,
        uint32_t key, uint32_t batch_idx);
/**
 * header_find_io_idx()
 *
 * This searches through the buffer information section for a buffer that
 * matches the provided name. VAS names take precedence over CnnGen aliases.
 *
 * The index of the buffer is returned, if found. Otherwise, -1 is returned.
 *
 * @param hdr           pointer to header
 * @param str           search string
 *
 * @return              index of buffer that matches given name
 */
static inline int32_t header_find_io_idx(cvflow_header_t *hdr, char *str);

/* Functions for modifying header */
/**
 * header_set_payload_info()
 *
 * This modifies the payload-specific structure.
 *
 * @param hdr           pointer to header
 * @param io_idx        index into `num_io' buffers
 * @param batch_idx     index into `batch_cnt' iterations (a.k.a. a batch)
 * @param ptr           desired absolute pointer to data (NULL is ignored)
 * @param pitch         desired pitch (0 is ignored)
 *
 * @return              error code
 */
static inline errcode_enum_t header_set_payload_info(cvflow_header_t *hdr,
        uint32_t io_idx, uint32_t batch_idx, void *ptr, uint32_t pitch);
/**
 * header_set_buffer_info()
 *
 * This modifies the per-buffer structure.
 *
 * @param hdr           pointer to header
 * @param io_idx        index into `num_io' buffers
 * @param batch_idx     index into `batch_cnt' iterations (a.k.a. a batch)
 * @param szd           padded/pitched increment to each buffer (0 is ignored)
 * @param szp           padded/pitched increment to each plane (0 is ignored)
 * @param szh           padded/pitched increment to each column (0 is ignored)
 * @param szw           padded/pitched increment to each row (0 is ignored)
 * @param d             size in depth dimension (0 is ignored)
 * @param p             size in plane dimension (0 is ignored)
 * @param h             size in height dimension (0 is ignored)
 * @param w             size in weight dimension (0 is ignored)
 * @param sign          sign in data format (2 or above is ignored)
 * @param datasize      data size in data format (3 or above is ignored)
 * @param expoffset     exponent offset in data format
 *                      (<-8 and >= 16 are ignored)
 * @param expbits       no. of bits in exponent field, in data format
 *                      (8 or above is ignored)
 * @param tracking_id   primitive tracking ID (INVALID_TRACKING_ID is ignored)
 * @param name          desired name, with a maximum of 32 characters
 *                      (NULL is ignored)
 * @param alias         desired alias, with a maximum of 96 characters
 *                      (NULL is ignored)
 *
 * @return              error code
 */
static inline errcode_enum_t header_set_buffer_info(cvflow_header_t *hdr,
        uint32_t io_idx, uint32_t batch_idx,
        uint32_t szd, uint32_t szp, uint32_t szh, uint32_t szw,
        uint32_t d, uint32_t p, uint32_t h, uint32_t w,
        uint32_t sign, uint32_t datasize, int32_t expoffset, uint32_t expbits,
        uint32_t tracking_id, char *name, char *alias);

/* Functions for strict checking */
/**
 * header_cmp_info()
 *
 * This compares the equality of two headers. Missing information is not
 * considered a mismatch.
 *
 * @param hdr_src       pointer to (source) header
 * @param hdr_tgt       pointer to (target) header
 * @param batch_size    batch size of both headers
 *
 * @return              error code
 */
static inline errcode_enum_t header_cmp_info(
    cvflow_header_t *hdr_src, cvflow_header_t *hdr_tgt, uint32_t batch_size);

/******************************* IMPLEMENTATION *******************************/
// Helper: calculate absolute pointer from `encoded' relative pointer
#ifdef ORCVIS
static rptr32_t decode_ptr(rptr32_t base_ptr, rptr32_t relative_ptr)
{
    uint32_t bit_mask = 0x1U;
    if (relative_ptr == 0U)
        return 0U;

    if (relative_ptr & bit_mask)
        return relative_ptr ^ bit_mask;
    else
        return base_ptr + relative_ptr;
}
#define DECODE_BUF(b, r) \
    ((cvflow_buffer_info_t *) decode_ptr((rptr32_t) (b), (rptr32_t) (r)))
#define DECODE_BATCH(b, r) \
    ((cvflow_batch_info_t *) decode_ptr((rptr32_t) (b), (rptr32_t) (r)))
#define DECODE_PAYLOAD(b, r, n) \
    (((cvflow_payload_unit_t *) decode_ptr((rptr32_t) (b), (rptr32_t) (r))) + n)
#else /* RTOS: ARM64 or ARM32 */
static uintptr_t decode_ptr(uintptr_t base_ptr, uintptr_t relative_ptr)
{
    uintptr_t rptr = relative_ptr;
    uint64_t bit_mask = 0x1U;
    if (relative_ptr == 0U)
        return 0U;

    if (relative_ptr & bit_mask)
        return (uintptr_t) ambacv_p2v(rptr ^ bit_mask);
    else
        return base_ptr + rptr;
}
#define DECODE_BUF(b, r) \
    ((cvflow_buffer_info_t *) decode_ptr((uintptr_t) (b), (uintptr_t) (r)))
#define DECODE_BATCH(b, r) \
    ((cvflow_batch_info_t *) decode_ptr((uintptr_t) (b), (uintptr_t) (r)))
#define DECODE_PAYLOAD(b, r, n) \
    (((cvflow_payload_unit_t *) decode_ptr((uintptr_t) (b), (uintptr_t) (r))) \
     + n)
#endif

// Helper: round up `x' to nearest multiple of `r'
static uint32_t round_up(uint32_t x, uint32_t r)
{
    return ((x - 1U) / r + 1U) * r;
}

// Main
static inline uint32_t header_get_size(
    uint32_t num_io, uint32_t batch_cnt,
    const uint32_t *io_sizes, const uint32_t *num_kvs
)
{
    uint32_t i, partial_sum;
    uint32_t result = 0U;

    // Header
    result += sizeof(cvflow_header_t);
    result += sizeof(rptr32_t) * batch_cnt;

    // Per-buffer information
    result += sizeof(cvflow_buffer_info_t) * num_io;

    // Optional arguments
    if (num_kvs != NULL) {
        result += sizeof(cvflow_batch_info_t) * batch_cnt;
        partial_sum = 0U;
        for (i = 0U; i < batch_cnt; i++) {
            partial_sum += num_kvs[i];
        }
        result += sizeof(cvflow_o_arg_t) * partial_sum;
    }

    // Payload-specific information
    result += sizeof(cvflow_payload_unit_t) * num_io * batch_cnt;

    // Payloads
    result = round_up(result, 128U);
    if (io_sizes == NULL)
        return 0U;
    for (i = 0U; i < num_io; i++) {
        result += round_up(io_sizes[i] * batch_cnt, 128U);
    }

    return result;
}

// Main
static inline errcode_enum_t header_init(
    cvflow_header_t *hdr,
    uint32_t num_io, uint32_t batch_cnt,
    const uint32_t *io_sizes, const uint32_t *num_kvs
)
{
    uint32_t i, j, offset = 0U, partial_sum;
    cvflow_o_arg_t *o_arg;
    cvflow_payload_unit_t *pl, *pl_base;
    cvflow_buffer_info_t *buf_info;
    cvflow_batch_info_t *batch_info;

    if (hdr == NULL || io_sizes == NULL)
        return ERRCODE_BAD_PARAMETER;
    if (num_io == 0U || batch_cnt == 0U)
        return ERRCODE_BAD_PARAMETER;

    // Header
    hdr->version = HEADER_VERSION_NUMBER;
    hdr->num_io = num_io;
    hdr->max_batch_cnt = batch_cnt;

    offset += sizeof(cvflow_header_t);
    offset += sizeof(rptr32_t) * batch_cnt;
    hdr->buf_info = (rptr32_t) offset;

    offset += sizeof(cvflow_buffer_info_t) * num_io;
    if (num_kvs != NULL)
        hdr->batch_info = (rptr32_t) offset;
    else
        hdr->batch_info = 0U;

    if (num_kvs != NULL) {
        offset += sizeof(cvflow_batch_info_t) * batch_cnt;
        partial_sum = 0U;
        for (i = 0U; i < batch_cnt; i++) {
            partial_sum += num_kvs[i];
        }
        offset += sizeof(cvflow_o_arg_t) * partial_sum;
    }
    for (i = 0U; i < batch_cnt; i++) {
        hdr->payload_ptrs[i] = offset;
        offset += sizeof(cvflow_payload_unit_t) * num_io;
    }

    // Per-buffer information
    buf_info = DECODE_BUF(hdr, hdr->buf_info);
    for (i = 0U; i < num_io - 1; i++) {
        buf_info->next_ptr = sizeof(cvflow_buffer_info_t);
        buf_info->pdim[0] = 0U;
        buf_info->pdim[1] = 0U;
        buf_info->pdim[2] = 0U;
        buf_info->pdim[3] = 0U;
        buf_info->dim[0] = 0U;
        buf_info->dim[1] = 0U;
        buf_info->dim[2] = 0U;
        buf_info->dim[3] = 0U;
        buf_info->df[0] = 2;
        buf_info->df[1] = 3;
        buf_info->df[2] = 16;
        buf_info->df[3] = 8;
        buf_info->tracking_id = INVALID_TRACKING_ID;
        buf_info->name[0] = '\0';
        buf_info->alias[0] = '\0';
        buf_info = DECODE_BUF(buf_info, buf_info->next_ptr);
    }
    buf_info->next_ptr = 0U;

    // Optional arguments
    if (num_kvs != NULL) {
        batch_info = DECODE_BATCH(hdr, hdr->batch_info);
        for (i = 0U; i < batch_cnt; i++) {
            batch_info->next_ptr = sizeof(cvflow_batch_info_t);
            batch_info->next_ptr += sizeof(cvflow_o_arg_t) * num_kvs[i];

            batch_info->num_kv = num_kvs[i];
            o_arg = batch_info->kvs;
            for (j = 0U; j < num_kvs[i]; j++) {
                o_arg->key = INVALID_OPT_ARG;
                o_arg += 1U;
            }

            batch_info = DECODE_BATCH(batch_info, batch_info->next_ptr);
        }
    }

    // Payload-specific information
    offset = round_up(offset, 128U) - (uint32_t) hdr->payload_ptrs[0];
    pl = DECODE_PAYLOAD(hdr, hdr->payload_ptrs[0], 0U);
    for (j = 0U; j < num_io; j++) {
        pl->payload_ptr = offset;
        offset += round_up(io_sizes[j] * batch_cnt, 128U) - sizeof(*pl);
        pl += 1U;
    }
    for (i = 1U; i < batch_cnt; i++) {
        pl_base = DECODE_PAYLOAD(hdr, hdr->payload_ptrs[0], 0U);
        pl = DECODE_PAYLOAD(hdr, hdr->payload_ptrs[i], 0U);
        for (j = 0U; j < num_io; j++) {
            pl->payload_ptr = pl_base->payload_ptr;
            pl->payload_ptr += io_sizes[j] - sizeof(*pl);
            pl += 1U;
            pl_base += 1U;
        }
    }

    return ERRCODE_NONE;
}

// Main
static inline uint32_t header_get_batch_size(cvflow_header_t *hdr, uint32_t default_size)
{
    cvflow_buffer_info_t *buf_info;
    uint32_t i, result = 0U;

    if (hdr == NULL)
        return 0U;
    if (default_size > hdr->max_batch_cnt)
        return 0U;
    if (hdr->buf_info == 0U)
        return default_size;

    // Traverse through per-buffer information
    buf_info = DECODE_BUF(hdr, hdr->buf_info);
    result = buf_info->dim[0U];
    for (i = 1U; i < hdr->num_io; i++) {
        if (buf_info == NULL)
            return 0U;

        // Batch size must be consistent
        if (result != buf_info->dim[0U])
            return 0U;

        buf_info = DECODE_BUF(buf_info, buf_info->next_ptr);
    }

    // Only one source of batch size is allowed
    if (default_size == 1U && result == 1U)
        return 1U;
    else if (default_size == 1U && result != 1U)
        return result;
    else if (default_size != 1U && result == 1U)
        return default_size;
    else
        return 0U;
}

// Main
static inline uint32_t header_get_count(cvflow_header_t *hdr)
{
    if (hdr == NULL)
        return 0U;
    return hdr->num_io;
}

// Main
#ifdef ORCVIS
static inline rptr32_t header_get_payload(
    cvflow_header_t *hdr,
    uint32_t io_idx, uint32_t batch_idx
)
{
#else /* RTOS: ARM64 or ARM32 */
static inline uintptr_t header_get_payload(
    cvflow_header_t *hdr,
    uint32_t io_idx, uint32_t batch_idx
)
{
#endif
    cvflow_payload_unit_t *pl;

    if (hdr == NULL)
        return 0U;
    if (io_idx >= hdr->num_io || batch_idx >= hdr->max_batch_cnt)
        return 0U;

    pl = DECODE_PAYLOAD(hdr, hdr->payload_ptrs[batch_idx], io_idx);
#ifdef ORCVIS
    return decode_ptr((rptr32_t) pl, (rptr32_t) pl->payload_ptr);
#else /* RTOS: ARM64 or ARM32 */
    return decode_ptr((uintptr_t) pl, (uintptr_t) pl->payload_ptr);
#endif
}

// Main
static inline uint32_t header_get_payload_pitch(
    cvflow_header_t *hdr,
    uint32_t io_idx, uint32_t batch_idx
)
{
    cvflow_payload_unit_t *pl;

    if (hdr == NULL)
        return 0U;
    if (io_idx >= hdr->num_io || batch_idx >= hdr->max_batch_cnt)
        return 0U;

    pl = DECODE_PAYLOAD(hdr, hdr->payload_ptrs[batch_idx], io_idx);
    return pl->pitch;
}

// Main
static inline uint32_t *header_find_opt_arg(
    cvflow_header_t *hdr,
    uint32_t key, uint32_t batch_idx
)
{
    uint32_t i;
    cvflow_batch_info_t *info;

    if (hdr == NULL)
        return NULL;
    if (hdr->batch_info == 0U)
        return NULL;
    if (batch_idx >= hdr->max_batch_cnt)
        return NULL;

    // Traverse to the right structure
    info = DECODE_BATCH(hdr, hdr->batch_info);
    for (i = 1U; i < batch_idx; i++) {
        if (info == NULL)
            return NULL;
        info = DECODE_BATCH(info, info->next_ptr);
    }

    // Search linearly for the right key
    for (i = 0U; i < info->num_kv; i++) {
        if (info->kvs[i].key == key)
            return &(info->kvs[i].value);
    }
    return NULL;
}

#ifdef ORCVIS
// Helper: copy string from source to destination
static void strncpy(char *dst, const char *src, uint32_t n)
{
    if (dst == NULL || src == NULL)
        return;

    uint32_t i;
    for (i = 0U; i < n; i++) {
        if (*src == '\0')
            break;
        *dst = *src;
        src += 1U;
        dst += 1U;
    }
    return;
}

// Helper: compare two strings
static unsigned int strncmp(char *dst, const char *src, uint32_t n)
{
    if (dst == NULL || src == NULL)
        return 1U;

    uint32_t i;
    for (i = 0U; i < n; i++) {
        if (*dst != *src)
            return 1U;
        if (*src == '\0')
            break;
        src += 1U;
        dst += 1U;
    }
    return 0U;
}
#endif

// Main
static inline int32_t header_find_io_idx(cvflow_header_t *hdr, char *str)
{
    cvflow_buffer_info_t *buf_info;
    uint32_t i;

    if (hdr == NULL || str == NULL)
        return ERRCODE_BAD_PARAMETER;

    // Traverse through per-buffer information
    buf_info = DECODE_BUF(hdr, hdr->buf_info);
    for (i = 0U; i < hdr->num_io - 1; i++) {
        if (buf_info == NULL)
            return ERRCODE_BAD_PARAMETER;

        if (strncmp(buf_info->name, str, NAME_MAX_LENGTH) == 0U)
            return i;

        buf_info = DECODE_BUF(buf_info, buf_info->next_ptr);
    }

    // Traverse through per-buffer information again
    buf_info = DECODE_BUF(hdr, hdr->buf_info);
    for (i = 0U; i < hdr->num_io - 1; i++) {
        if (buf_info == NULL)
            return ERRCODE_BAD_PARAMETER;

        if (strncmp(buf_info->alias, str, ALIAS_MAX_LENGTH) == 0U)
            return i;

        buf_info = DECODE_BUF(buf_info, buf_info->next_ptr);
    }

    return -1;
}

// Main
static inline errcode_enum_t header_set_payload_info(
    cvflow_header_t *hdr,
    uint32_t io_idx, uint32_t batch_idx,
    void *ptr, uint32_t pitch
)
{
#ifdef ORCVIS
    uint32_t bit_mask = 0x1U;
    rptr32_t ptr_cast = (rptr32_t) ptr;
#else
    uint64_t bit_mask = 0x1U;
    uintptr_t ptr_cast = (uintptr_t) ptr;
#endif
    cvflow_payload_unit_t *pl;

    if (hdr == NULL)
        return ERRCODE_BAD_PARAMETER;
    if (io_idx >= hdr->num_io || batch_idx >= hdr->max_batch_cnt)
        return ERRCODE_BAD_PARAMETER;

    pl = DECODE_PAYLOAD(hdr, hdr->payload_ptrs[batch_idx], io_idx);
    if (ptr_cast != 0U)
        pl->payload_ptr = ptr_cast | bit_mask;
    if (pitch != 0U)
        pl->pitch = pitch;
    return ERRCODE_NONE;
}

// Main
static inline errcode_enum_t header_set_buffer_info(
    cvflow_header_t *hdr,
    uint32_t io_idx, uint32_t batch_idx,
    uint32_t szd, uint32_t szp, uint32_t szh, uint32_t szw,
    uint32_t d, uint32_t p, uint32_t h, uint32_t w,
    uint32_t sign, uint32_t datasize, int32_t expoffset, uint32_t expbits,
    uint32_t tracking_id, char *name, char *alias
)
{
    cvflow_buffer_info_t *buf_info;
    uint32_t i;

    if (hdr == NULL)
        return ERRCODE_BAD_PARAMETER;
    if (io_idx >= hdr->num_io || batch_idx >= hdr->max_batch_cnt)
        return ERRCODE_BAD_PARAMETER;

    // Traverse through per-buffer information
    buf_info = DECODE_BUF(hdr, hdr->buf_info);
    for (i = 0U; i < io_idx; i++) {
        if (buf_info == NULL)
            return ERRCODE_BAD_PARAMETER;
        buf_info = DECODE_BUF(buf_info, buf_info->next_ptr);
    }

    // Populate struct
    if (szd != 0U)
        buf_info->pdim[0] = szd;
    if (szp != 0U)
        buf_info->pdim[1] = szp;
    if (szh != 0U)
        buf_info->pdim[2] = szh;
    if (szw != 0U)
        buf_info->pdim[3] = szw;
    if (d != 0U)
        buf_info->dim[0] = d;
    if (p != 0U)
        buf_info->dim[1] = p;
    if (h != 0U)
        buf_info->dim[2] = h;
    if (w != 0U)
        buf_info->dim[3] = w;
    if (sign < 2U)
        buf_info->df[0] = (int32_t) sign;
    if (datasize < 3U)
        buf_info->df[1] = (int32_t) datasize;
    if (expoffset < 16 && expoffset >= -8)
        buf_info->df[2] = expoffset;
    if (expbits < 8U)
        buf_info->df[3] = (int32_t) expbits;
    if (tracking_id != INVALID_TRACKING_ID)
        buf_info->tracking_id = (int32_t) tracking_id;
    if (name != NULL)
        strncpy(buf_info->name, name, NAME_MAX_LENGTH);
    if (alias != NULL)
        strncpy(buf_info->alias, alias, ALIAS_MAX_LENGTH);

    return ERRCODE_NONE;
}

// Helper: compare two cvflow_buffer_info_t structs
static inline errcode_enum_t header_cmp_buf_info(
    cvflow_buffer_info_t *info_src,
    cvflow_buffer_info_t *info_tgt,
    uint32_t num_io
)
{
    uint32_t i;

    if (info_src == NULL || info_tgt == NULL)
        return ERRCODE_BAD_PARAMETER;

    for (i = 0U; i < num_io; i++) {
        // Terminate together
        if (info_src == NULL && info_tgt == NULL)
            break;
        // Something's wrong
        if (info_src == NULL || info_tgt == NULL)
            return ERRCODE_GENERIC;

        // Check for differences
        if (info_src->pdim[0] != info_tgt->pdim[0])
            return ERRCODE_GENERIC;
        if (info_src->pdim[1] != info_tgt->pdim[1])
            return ERRCODE_GENERIC;
        if (info_src->pdim[2] != info_tgt->pdim[2])
            return ERRCODE_GENERIC;
        if (info_src->pdim[3] != info_tgt->pdim[3])
            return ERRCODE_GENERIC;
        if (info_src->dim[0] != info_tgt->dim[0])
            return ERRCODE_GENERIC;
        if (info_src->dim[1] != info_tgt->dim[1])
            return ERRCODE_GENERIC;
        if (info_src->dim[2] != info_tgt->dim[2])
            return ERRCODE_GENERIC;
        if (info_src->dim[3] != info_tgt->dim[3])
            return ERRCODE_GENERIC;
        if (info_src->df[0] != info_tgt->df[0])
            return ERRCODE_GENERIC;
        if (info_src->df[1] != info_tgt->df[1])
            return ERRCODE_GENERIC;
        if (info_src->df[2] != info_tgt->df[2])
            return ERRCODE_GENERIC;
        if (info_src->df[3] != info_tgt->df[3])
            return ERRCODE_GENERIC;
        if (info_src->tracking_id != info_tgt->tracking_id)
            return ERRCODE_GENERIC;
        if (strncmp(info_src->name, info_tgt->name, NAME_MAX_LENGTH))
            return ERRCODE_GENERIC;
        if (strncmp(info_src->alias, info_tgt->alias, ALIAS_MAX_LENGTH))
            return ERRCODE_GENERIC;

        // Move on together
        info_src = DECODE_BUF(info_src, info_src->next_ptr);
        info_tgt = DECODE_BUF(info_tgt, info_tgt->next_ptr);
    }

    return ERRCODE_NONE;
}

// Helper: compare two cvflow_batch_info_t structs
static errcode_enum_t header_cmp_batch_info(
    cvflow_batch_info_t *info_src,
    cvflow_batch_info_t *info_tgt,
    uint32_t batch_cnt
)
{
    cvflow_o_arg_t *kvs_src, *kvs_tgt;
    uint32_t num_kv_src, num_kv_tgt;
    unsigned int i, j, k;

    if (info_src == NULL || info_tgt == NULL)
        return ERRCODE_GENERIC;

    for (i = 0U; i < batch_cnt; i++) {
        // Terminate together
        if (info_src == NULL && info_tgt == NULL)
            break;
        // Something's wrong
        if (info_src == NULL || info_tgt == NULL)
            return ERRCODE_GENERIC;

        // Check for differences
        kvs_src = info_src->kvs;
        kvs_tgt = info_tgt->kvs;
        num_kv_src = info_src->num_kv;
        num_kv_tgt = info_tgt->num_kv;
        j = 0U;
        k = 0U;
        while (j < num_kv_src && k < num_kv_tgt) {
            if (kvs_src->key == kvs_tgt->key) {
                if (kvs_src->value != kvs_tgt->value)
                    return ERRCODE_GENERIC;
            } else if (kvs_src->key < kvs_tgt->key) {
                kvs_src += 1U;
                j += 1U;
            } else if (kvs_src->key > kvs_tgt->key) {
                kvs_tgt += 1U;
                k += 1U;
            }
        }

        // Move on together
        info_src = DECODE_BATCH(info_src, info_src->next_ptr);
        info_tgt = DECODE_BATCH(info_tgt, info_tgt->next_ptr);
    }

    return ERRCODE_NONE;
}

// Helper: compare two cvflow_payload_unit_t structs
static errcode_enum_t header_cmp_pl_info(
    cvflow_payload_unit_t *info_src,
    cvflow_payload_unit_t *info_tgt,
    uint32_t batch_size
)
{
    unsigned int i;

    if (info_src == NULL || info_tgt == NULL)
        return ERRCODE_GENERIC;

    for (i = 0U; i < batch_size; i++) {
        // Check for differences
        if (info_src->pitch != info_tgt->pitch)
            return ERRCODE_GENERIC;

        // Move on
        info_src += 1;
        info_tgt += 1;
    }

    return ERRCODE_NONE;
}

// Main
static inline errcode_enum_t header_cmp_info(
    cvflow_header_t *hdr_src, cvflow_header_t *hdr_tgt, uint32_t batch_size
)
{
    errcode_enum_t status = ERRCODE_NONE;
    unsigned int i, j;

    if (hdr_src == NULL || hdr_tgt == NULL)
        return ERRCODE_GENERIC;

    if (hdr_src->num_io != hdr_tgt->num_io)
        return ERRCODE_GENERIC;
    if (hdr_src->max_batch_cnt != hdr_tgt->max_batch_cnt)
        return ERRCODE_GENERIC;

    if (hdr_src->buf_info != 0U && hdr_tgt->buf_info != 0U) {
        status |= header_cmp_buf_info(
                      DECODE_BUF(hdr_src, hdr_src->buf_info),
                      DECODE_BUF(hdr_tgt, hdr_tgt->buf_info),
                      hdr_src->num_io
                  );
    }

    if (hdr_src->batch_info != 0U && hdr_tgt->batch_info != 0U) {
        status |= header_cmp_batch_info(
                      DECODE_BATCH(hdr_src, hdr_src->batch_info),
                      DECODE_BATCH(hdr_tgt, hdr_tgt->batch_info),
                      batch_size
                  );
    }

    for (i = 0U; i < batch_size; i++) {
        for (j = 0U; j < hdr_src->num_io; j++) {
            status |= header_cmp_pl_info(
                          DECODE_PAYLOAD(hdr_src, hdr_src->payload_ptrs[j], i),
                          DECODE_PAYLOAD(hdr_tgt, hdr_tgt->payload_ptrs[j], i),
                          batch_size
                      );
        }
    }

    return status;
}

#endif
