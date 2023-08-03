/**
 * @file AmbaIPC_List.h
 *
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @details Definitions and utilities for link list.
 *
 */

#ifndef AMBA_IPC_LIST_H
#define AMBA_IPC_LIST_H

#if !defined(_STDIO_H_) && !defined(offsetof)
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member)                    \
        (type *)( (char *)ptr - offsetof(type,member))


typedef INT32 (*PORT_RECV_f)(void *context, AMBA_IPC_PKT_s *, INT32 len);

typedef struct _AMBA_IPC_LIST_s_ {
    struct _AMBA_IPC_LIST_s_ *next;     /**< next list */
    struct _AMBA_IPC_LIST_s_ *prev;     /**< previous list */
} AMBA_IPC_LIST_s;

typedef struct {
    AMBA_IPC_LIST_s       list;     /**< list */
    AMBA_KAL_MUTEX_t      lock;     /**< lock */
    AMBA_KAL_EVENT_FLAG_t flag;     /**< flag */
    INT32                   id;       /**< id */
    void                  *context; /**< context */
    PORT_RECV_f           recv;     /**< recv */
} AMBA_IPC_PORT_s;

#define LIST_HEAD(name) AMBA_IPC_LIST_s name = {&(name), &(name)}

#define INIT_LIST_HEAD(name) {name.prev = name.next = &name;}

/*
 * list_add - add a new entry to the list
 */
static inline void AmbaIPC_ListAdd(AMBA_IPC_LIST_s *new, AMBA_IPC_LIST_s *head)
{
    head->next->prev = new;
    new->next = head->next;
    new->prev = head;
    head->next = new;
}

/*
 * list_del - deletes the entry from list.
 */
static inline void AmbaIPC_ListDel(AMBA_IPC_LIST_s *entry)
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
}

/*
 * list_empty - tests whether a list is empty
 */
static inline INT32 AmbaIPC_ListIsEmpty(const AMBA_IPC_LIST_s *head)
{
    return head->next == head;
}

/*
 * list_entry - get the struct for this entry
 * ptr:    the &struct list_head pointer.
 * type:    the type of the struct this is embedded in.
 * member:    the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) container_of(ptr, type, member)

/**
 * list_for_each_entry    -    iterate over list of given type
 * pos:    the type * to use as a loop counter.
 * head:    the head for your list.
 * member:    the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, type, member)            \
    for (pos = list_entry((head)->next, type, member);    \
         &pos->member != (head);                \
         pos = list_entry(pos->member.next, type, member))

#endif  /* AMBA_IPC_LIST_H */
