/**
 * @file rpmsg.h
 * Copyright (c) 2020 Ambarella International LP
 */
/**
 * Remote processor messaging
 *
 * Copyright (C) 2011 Texas Instruments, Inc.
 * Copyright (C) 2011 Google, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name Texas Instruments nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LINUX_RPMSG_H
#define LINUX_RPMSG_H

/* Used by Ambarella for ack */
#define RPMSG_HDR_FLAGS_ACK     (0x8000u)

/*
 * This struct definition is shared between both linux and itron
 */
struct rpmsg_hdr {
    /** address of source */
    UINT32 src;
    /** address of destination */
    UINT32 dst;
#ifdef CONFIG_AMBALINK_RPMSG_ASIL
    UINT32 id;
#else
    /** reserved */
    UINT32 reserved;
#endif // CONFIG_AMBALINK_RPMSG_ASIL
    /** length */
    UINT16 len;
    /** flags */
    UINT16 flags;

    /**
     * The "data" field should be sync to linux side as a zero-sized array.
     * However, current legacy compilation environment does not allow it.
     * So we use an char array here as a place holder for symbol referencing.
     *
     * It should not have any problem except that care must be taken when
     * this structure is used in sizeof() operator.
     */
    UINT8 data[4];
} __attribute__((packed));

/*
 * This struct definition is shared between both linux and itron
 */
struct rpmsg_ns_msg {
    char name[RPMSG_NAME_LEN];  /**< Name of channel */
    UINT32 addr;                /**< Address of channel */
    UINT32 flags;               /**< Flags */
} __attribute__((packed));

/**
 * struct rpmsg_channel_info - channel info representation
 */
struct rpmsg_channel_info {
    char    name[RPMSG_NAME_LEN];   /**< name of service */
    UINT32  src;                    /**< local address */
    UINT32  dst;                    /**< destination address */
};

#endif /* LINUX_RPMSG_H */
