/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   ThreadX MISRA Compliance                                            */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** _tx_version_id                                                        */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

//#ifdef TX_MISRA_ENABLE
#if 1
#define TX_THREAD_INIT
//CHAR  _tx_version_id[100] =  "Copyright (c) Microsoft Corporation. All rights reserved.  * ThreadX 6.1       MISRA C Compliant *";

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_trace.h"
/* TBD */
#include <string.h>

/* Lynn */
static void _tx_TypeCast32(void * pNewType, const void * pOldType)
{
    UINT32 *pDes = pNewType;
    const UINT32 *pSrc = pOldType;

    *pDes = *pSrc;

    return;
}

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  VOID  _tx_misra_memset(VOID *ptr, UINT value, UINT size);            */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
VOID  _tx_misra_memset(VOID *ptr, UINT value, UINT size)
{
    memset(ptr, value, size);
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  UCHAR  *_tx_misra_uchar_pointer_add(UCHAR *ptr, UINT32 amount);       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
UCHAR  *_tx_misra_uchar_pointer_add(UCHAR *ptr, UINT32 amount)
{
    ptr =  ptr + amount;
    return(ptr);
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  UCHAR  *_tx_misra_uchar_pointer_sub(UCHAR *ptr, UINT32 amount);       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
UCHAR  *_tx_misra_uchar_pointer_sub(UCHAR *ptr, UINT32 amount)
{
    ptr =  ptr - amount;
    return(ptr);
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  UINT32  _tx_misra_uchar_pointer_dif(UCHAR *ptr1, UCHAR *ptr2);        */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
UINT32  _tx_misra_uchar_pointer_dif(UCHAR *ptr1, UCHAR *ptr2)
{

    UINT32   value;

    value =  ptr1 - ptr2;
    return(value);
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  UINT32  _tx_misra_pointer_to_ulong_convert(VOID *ptr);                */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
UINT32  _tx_misra_pointer_to_ulong_convert(VOID *ptr)
{
    UINT32 value = 0U;

    _tx_TypeCast32(&value, &ptr);

    return value;
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  UINT32  *_tx_misra_ulong_pointer_add(UINT32 *ptr, UINT32 amount);       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
UINT32  *_tx_misra_ulong_pointer_add(UINT32 *ptr, UINT32 amount)
{
    ptr =  ptr + amount;
    return(ptr);
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  UINT32  *_tx_misra_ulong_pointer_sub(UINT32 *ptr, UINT32 amount);       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
UINT32  *_tx_misra_ulong_pointer_sub(UINT32 *ptr, UINT32 amount)
{

    ptr =  ptr - amount;
    return(ptr);
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  UINT32  _tx_misra_ulong_pointer_dif(UINT32 *ptr1, UINT32 *ptr2);        */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
UINT32   _tx_misra_ulong_pointer_dif(UINT32 *ptr1, UINT32 *ptr2)
{
    UINT32   value;

    value =  ptr1 - ptr2;
    return(value);
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  VOID  *_tx_misra_ulong_to_pointer_convert(UINT32 input);              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
VOID  *_tx_misra_ulong_to_pointer_convert(UINT32 input)
{
    VOID * ptr = 0;

    _tx_TypeCast32(&ptr, &input);

    return ptr;
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  VOID  _tx_misra_message_copy(UINT32 **source, UINT32 **destination,    */
/**                                                           UINT size); */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
VOID  _tx_misra_message_copy(UINT32 **source, UINT32 **destination, UINT size)
{

    UINT32   *s, *d;
    UINT    z;

    s =  *source;
    d =  *destination;
    z =  size;

    *(d) = *(s);
    (d)++;
    (s)++;
    if ((z) > ((UINT) 1)) {
        (z)--;
        while ((z)) {
            *(d) =  *(s);
            (d)++;
            (s)++;
            (z)--;
        }
    }

    *source = s;
    *destination = d;
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  UINT32  _tx_misra_timer_pointer_dif(TX_TIMER_INTERNAL **ptr1,         */
/**                                           TX_TIMER_INTERNAL **ptr2);  */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
UINT32  _tx_misra_timer_pointer_dif(TX_TIMER_INTERNAL **ptr1, TX_TIMER_INTERNAL **ptr2)
{

    UINT32   value;

    value =  ptr1 - ptr2;
    return(value);
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  TX_TIMER_INTERNAL **_tx_misra_timer_pointer_add(TX_TIMER_INTERNAL    */
/**                                                  **ptr1, UINT32 size); */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
TX_TIMER_INTERNAL  **_tx_misra_timer_pointer_add(TX_TIMER_INTERNAL **ptr1, UINT32 amount)
{
    ptr1 =  ptr1 + amount;
    return(ptr1);
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  VOID  _tx_misra_user_timer_pointer_get(TX_TIMER_INTERNAL             */
/**                              *internal_timer, TX_TIMER **user_timer); */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
VOID _tx_misra_user_timer_pointer_get(TX_TIMER_INTERNAL *internal_timer, TX_TIMER **user_timer)
{

    UCHAR       *working_ptr;
    TX_TIMER    *temp_timer;


    working_ptr =  (UCHAR *) internal_timer;

    temp_timer =  (TX_TIMER *) working_ptr;
    working_ptr =  working_ptr - (((UCHAR *) &temp_timer -> tx_timer_internal) - ((UCHAR *) &temp_timer -> tx_timer_id));
    *user_timer =  (TX_TIMER *) working_ptr;
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  VOID  _tx_misra_thread_stack_check(TX_THREAD *thread_ptr,            */
/**                                              VOID **highest_stack);   */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
VOID  _tx_misra_thread_stack_check(TX_THREAD *thread_ptr, VOID **highest_stack)
{

    TX_INTERRUPT_SAVE_AREA

    TX_DISABLE
    if (((thread_ptr)) && ((thread_ptr) -> tx_thread_id == TX_THREAD_ID)) {
        if (((UINT32 *) (thread_ptr) -> tx_thread_stack_ptr) < ((UINT32 *) *highest_stack)) {
            *highest_stack =  (thread_ptr) -> tx_thread_stack_ptr;
        }
        if ((*((UINT32 *) (thread_ptr) -> tx_thread_stack_start) != TX_STACK_FILL) ||
            (*((UINT32 *) (((UCHAR *) (thread_ptr) -> tx_thread_stack_end) + 1)) != TX_STACK_FILL) ||
            (((UINT32 *) *highest_stack) < ((UINT32 *) (thread_ptr) -> tx_thread_stack_start))) {
            TX_RESTORE
            _tx_thread_stack_error_handler((thread_ptr));
            TX_DISABLE
        }
        if (*(((UINT32 *) *highest_stack) - 1) != TX_STACK_FILL) {
            TX_RESTORE
            _tx_thread_stack_analyze((thread_ptr));
            TX_DISABLE
        }
    }
    TX_RESTORE
}


#ifdef TX_ENABLE_EVENT_TRACE

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  VOID  _tx_misra_trace_event_insert(UINT32 event_id,                   */
/**           VOID *info_field_1, UINT32 info_field_2, UINT32 info_field_3, */
/**           UINT32 info_field_4, UINT32 filter, UINT32 time_stamp);        */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
VOID  _tx_misra_trace_event_insert(UINT32 event_id, VOID *info_field_1, UINT32 info_field_2, UINT32 info_field_3, UINT32 info_field_4, UINT32 filter, UINT32 time_stamp)
{

    TX_TRACE_BUFFER_ENTRY     *trace_event_ptr;
    UINT32                      trace_system_state;
    UINT32                      trace_priority;
    TX_THREAD                 *trace_thread_ptr;


    trace_event_ptr =  _tx_trace_buffer_current_ptr;
    if ((trace_event_ptr) && (_tx_trace_event_enable_bits & ((UINT32) (filter)))) {
        TX_TRACE_PORT_EXTENSION
        trace_system_state =  (UINT32) _tx_thread_system_state;
        trace_thread_ptr =    _tx_thread_current_ptr;

        if (trace_system_state == 0) {
            trace_priority =  trace_thread_ptr -> tx_thread_priority;
            trace_priority =  trace_priority | 0x80000000UL | (trace_thread_ptr -> tx_thread_preempt_threshold << 16);
        } else if (trace_system_state < 0xF0F0F0F0UL) {
            trace_priority =    (UINT32) trace_thread_ptr;
            trace_thread_ptr =  (TX_THREAD *) 0xFFFFFFFFUL;
        } else {
            trace_thread_ptr =  (TX_THREAD *) 0xF0F0F0F0UL;
            trace_priority =    0;
        }
        trace_event_ptr -> tx_trace_buffer_entry_thread_pointer =       (UINT32) trace_thread_ptr;
        trace_event_ptr -> tx_trace_buffer_entry_thread_priority =      (UINT32) trace_priority;
        trace_event_ptr -> tx_trace_buffer_entry_event_id =             (UINT32) (event_id);
        trace_event_ptr -> tx_trace_buffer_entry_time_stamp =           (UINT32) (time_stamp);
#ifdef TX_MISRA_ENABLE
        trace_event_ptr -> tx_trace_buffer_entry_info_1 =               (UINT32) (info_field_1);
        trace_event_ptr -> tx_trace_buffer_entry_info_2 =               (UINT32) (info_field_2);
        trace_event_ptr -> tx_trace_buffer_entry_info_3 =               (UINT32) (info_field_3);
        trace_event_ptr -> tx_trace_buffer_entry_info_4 =               (UINT32) (info_field_4);
#else
        trace_event_ptr -> tx_trace_buffer_entry_information_field_1 =  (UINT32) (info_field_1);
        trace_event_ptr -> tx_trace_buffer_entry_information_field_2 =  (UINT32) (info_field_2);
        trace_event_ptr -> tx_trace_buffer_entry_information_field_3 =  (UINT32) (info_field_3);
        trace_event_ptr -> tx_trace_buffer_entry_information_field_4 =  (UINT32) (info_field_4);
#endif
        trace_event_ptr++;
        if (trace_event_ptr >= _tx_trace_buffer_end_ptr) {
            trace_event_ptr =  _tx_trace_buffer_start_ptr;
            _tx_trace_buffer_current_ptr =  trace_event_ptr;
            _tx_trace_header_ptr -> tx_trace_header_buffer_current_pointer =  (UINT32) trace_event_ptr;
            if (_tx_trace_full_notify_function)
                (_tx_trace_full_notify_function)((VOID *) _tx_trace_header_ptr);
        } else {
            _tx_trace_buffer_current_ptr =  trace_event_ptr;
            _tx_trace_header_ptr -> tx_trace_header_buffer_current_pointer =  (UINT32) trace_event_ptr;
        }
    }
}

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  UINT32  _tx_misra_time_stamp_get(VOID);                               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
UINT32  _tx_misra_time_stamp_get(VOID)
{

    /* Return time stamp.  */
    return(0);
}

#endif


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  UINT  _tx_misra_always_true(void);                                   */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
UINT  _tx_misra_always_true(void)
{
    return(TX_TRUE);
}


/******************************************************************************************/
/******************************************************************************************/
/**                                                                                       */
/**  UCHAR  **_tx_misra_indirect_void_to_uchar_pointer_convert(VOID **return_ptr);        */
/**                                                                                       */
/******************************************************************************************/
/******************************************************************************************/
UCHAR  **_tx_misra_indirect_void_to_uchar_pointer_convert(VOID **return_ptr)
{

    /* Return an indirect UCHAR pointer.  */
    return((UCHAR **) ((VOID *) return_ptr));
}


/***************************************************************************************/
/***************************************************************************************/
/**                                                                                    */
/**  UCHAR  **_tx_misra_uchar_to_indirect_uchar_pointer_convert(UCHAR *pointer);       */
/**                                                                                    */
/***************************************************************************************/
/***************************************************************************************/
UCHAR  **_tx_misra_uchar_to_indirect_uchar_pointer_convert(UCHAR *pointer)
{

    /* Return an indirect UCHAR pointer.  */
    return((UCHAR **) ((VOID *) pointer));
}


/***********************************************************************************/
/***********************************************************************************/
/**                                                                                */
/**  UCHAR  *_tx_misra_block_pool_to_uchar_pointer_convert(TX_BLOCK_POOL *pool);   */
/**                                                                                */
/***********************************************************************************/
/***********************************************************************************/
UCHAR  *_tx_misra_block_pool_to_uchar_pointer_convert(TX_BLOCK_POOL *pool)
{

    /* Return a UCHAR pointer.  */
    return((UCHAR *) ((VOID *) pool));
}


/******************************************************************************************/
/******************************************************************************************/
/**                                                                                       */
/**  TX_BLOCK_POOL  *_tx_misra_void_to_block_pool_pointer_convert(VOID *pointer);         */
/**                                                                                       */
/******************************************************************************************/
/******************************************************************************************/
TX_BLOCK_POOL  *_tx_misra_void_to_block_pool_pointer_convert(VOID *pointer)
{

    /* Return a block pool pointer.  */
    return((TX_BLOCK_POOL *) ((VOID *) pointer));
}


/*****************************************************************************/
/*****************************************************************************/
/**                                                                          */
/**  UCHAR  *_tx_misra_void_to_uchar_pointer_convert(VOID *pointer);         */
/**                                                                          */
/*****************************************************************************/
/*****************************************************************************/
UCHAR  *_tx_misra_void_to_uchar_pointer_convert(VOID *pointer)
{

    /* Return a UCHAR pointer.  */
    return((UCHAR *) ((VOID *) pointer));
}


/************************************************************************************/
/************************************************************************************/
/**                                                                                 */
/**  TX_BLOCK_POOL *_tx_misra_uchar_to_block_pool_pointer_convert(UCHAR *pointer);  */
/**                                                                                 */
/************************************************************************************/
/************************************************************************************/
TX_BLOCK_POOL  *_tx_misra_uchar_to_block_pool_pointer_convert(UCHAR *pointer)
{

    /* Return a UCHAR pointer.  */
    return((TX_BLOCK_POOL *) ((VOID *) pointer));
}


/**************************************************************************************/
/**************************************************************************************/
/**                                                                                   */
/**  UCHAR  **_tx_misra_void_to_indirect_uchar_pointer_convert(VOID *pointer);        */
/**                                                                                   */
/**************************************************************************************/
/**************************************************************************************/
UCHAR  **_tx_misra_void_to_indirect_uchar_pointer_convert(VOID *pointer)
{

    /* Return a UCHAR pointer.  */
    return((UCHAR **) ((VOID *) pointer));
}


/*****************************************************************************************/
/*****************************************************************************************/
/**                                                                                      */
/**  TX_BYTE_POOL  *_tx_misra_void_to_byte_pool_pointer_convert(VOID *pointer);          */
/**                                                                                      */
/*****************************************************************************************/
/*****************************************************************************************/
TX_BYTE_POOL  *_tx_misra_void_to_byte_pool_pointer_convert(VOID *pointer)
{

    /* Return a byte pool pointer.  */
    return((TX_BYTE_POOL *) ((VOID *) pointer));
}


/***************************************************************************************/
/***************************************************************************************/
/**                                                                                    */
/**  UCHAR  *_tx_misra_byte_pool_to_uchar_pointer_convert(TX_BYTE_POOL *pool);         */
/**                                                                                    */
/***************************************************************************************/
/***************************************************************************************/
UCHAR  *_tx_misra_byte_pool_to_uchar_pointer_convert(TX_BYTE_POOL *pool)
{

    /* Return a UCHAR pointer.  */
    return((UCHAR *) ((VOID *) pool));
}


/*****************************************************************************************/
/*****************************************************************************************/
/**                                                                                      */
/**  ALIGN_TYPE  *_tx_misra_uchar_to_align_type_pointer_convert(UCHAR *pointer);         */
/**                                                                                      */
/*****************************************************************************************/
/*****************************************************************************************/
ALIGN_TYPE  *_tx_misra_uchar_to_align_type_pointer_convert(UCHAR *pointer)
{

    /* Return an align time pointer.  */
    return((ALIGN_TYPE *) ((VOID *) pointer));
}


/****************************************************************************************************/
/****************************************************************************************************/
/**                                                                                                 */
/**  TX_BYTE_POOL  **_tx_misra_uchar_to_indirect_byte_pool_pointer_convert(UCHAR *pointer);         */
/**                                                                                                 */
/****************************************************************************************************/
/****************************************************************************************************/
TX_BYTE_POOL  **_tx_misra_uchar_to_indirect_byte_pool_pointer_convert(UCHAR *pointer)
{

    /* Return a byte pool pointer.  */
    return((TX_BYTE_POOL **) ((VOID *) pointer));
}


/**************************************************************************************************/
/**************************************************************************************************/
/**                                                                                               */
/**  TX_EVENT_FLAGS_GROUP  *_tx_misra_void_to_event_flags_pointer_convert(VOID *pointer);         */
/**                                                                                               */
/**************************************************************************************************/
/**************************************************************************************************/
TX_EVENT_FLAGS_GROUP  *_tx_misra_void_to_event_flags_pointer_convert(VOID *pointer)
{

    /* Return event flags pointer.  */
    return((TX_EVENT_FLAGS_GROUP *) ((VOID *) pointer));
}


/*****************************************************************************/
/*****************************************************************************/
/**                                                                          */
/**  UINT32  *_tx_misra_void_to_ulong_pointer_convert(VOID *pointer);         */
/**                                                                          */
/*****************************************************************************/
/*****************************************************************************/
UINT32  *_tx_misra_void_to_ulong_pointer_convert(VOID *pointer)
{

    /* Return a UINT32 pointer. */
    return((UINT32 *) ((VOID *) pointer));
}


/********************************************************************************/
/********************************************************************************/
/**                                                                             */
/**  TX_MUTEX  *_tx_misra_void_to_mutex_pointer_convert(VOID *pointer);         */
/**                                                                             */
/********************************************************************************/
/********************************************************************************/
TX_MUTEX  *_tx_misra_void_to_mutex_pointer_convert(VOID *pointer)
{

    /* Return a mutex pointer.  */
    return((TX_MUTEX *) ((VOID *) pointer));
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  UINT  _tx_misra_status_get(UINT status);                             */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
UINT  _tx_misra_status_get(UINT status)
{
    (void) status;
    /* Return a successful status.  */
    return(TX_SUCCESS);
}


/********************************************************************************/
/********************************************************************************/
/**                                                                             */
/**  TX_QUEUE  *_tx_misra_void_to_queue_pointer_convert(VOID *pointer);         */
/**                                                                             */
/********************************************************************************/
/********************************************************************************/
TX_QUEUE  *_tx_misra_void_to_queue_pointer_convert(VOID *pointer)
{

    /* Return queue pointer.  */
    return((TX_QUEUE *) ((VOID *) pointer));
}


/****************************************************************************************/
/****************************************************************************************/
/**                                                                                     */
/**  TX_SEMAPHORE  *_tx_misra_void_to_semaphore_pointer_convert(VOID *pointer);         */
/**                                                                                     */
/****************************************************************************************/
/****************************************************************************************/
TX_SEMAPHORE  *_tx_misra_void_to_semaphore_pointer_convert(VOID *pointer)
{

    /* Return semaphore pointer.  */
    return((TX_SEMAPHORE *) ((VOID *) pointer));
}


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**  VOID  *_tx_misra_uchar_to_void_pointer_convert(UCHAR *pointer);      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
VOID  *_tx_misra_uchar_to_void_pointer_convert(UCHAR *pointer)
{

    /* Return a VOID pointer.  */
    return((VOID *) ((VOID *) pointer));
}


/*********************************************************************************/
/*********************************************************************************/
/**                                                                              */
/**  TX_THREAD  *_tx_misra_ulong_to_thread_pointer_convert(UINT32 value);         */
/**                                                                              */
/*********************************************************************************/
/*********************************************************************************/
TX_THREAD  *_tx_misra_ulong_to_thread_pointer_convert(UINT32 value)
{
    TX_THREAD * ptr = 0;

    _tx_TypeCast32(&ptr, &value);

    /* Return a thread pointer.  */
    return ptr;
}


/***************************************************************************************************/
/***************************************************************************************************/
/**                                                                                                */
/**  VOID  *_tx_misra_timer_indirect_to_void_pointer_convert(TX_TIMER_INTERNAL **pointer);         */
/**                                                                                                */
/***************************************************************************************************/
/***************************************************************************************************/
VOID  *_tx_misra_timer_indirect_to_void_pointer_convert(TX_TIMER_INTERNAL **pointer)
{

    /* Return a void pointer.  */
    return((VOID *) ((VOID *) pointer));
}


/***************************************************************************************/
/***************************************************************************************/
/**                                                                                    */
/**  CHAR  *_tx_misra_const_char_to_char_pointer_convert(const char *pointer);         */
/**                                                                                    */
/***************************************************************************************/
/***************************************************************************************/
CHAR  *_tx_misra_const_char_to_char_pointer_convert(const char *pointer)
{

    /* Return a CHAR pointer.  */
    return((CHAR *) ((VOID *) pointer));
}


/**********************************************************************************/
/**********************************************************************************/
/**                                                                               */
/**  TX_THREAD  *_tx_misra_void_to_thread_pointer_convert(void *pointer);         */
/**                                                                               */
/**********************************************************************************/
/**********************************************************************************/
TX_THREAD  *_tx_misra_void_to_thread_pointer_convert(void *pointer)
{

    /* Return thread pointer.  */
    return((TX_THREAD *) ((VOID *) pointer));
}


#ifdef TX_ENABLE_EVENT_TRACE

/************************************************************************************************/
/************************************************************************************************/
/**                                                                                             */
/**  UCHAR  *_tx_misra_object_to_uchar_pointer_convert(TX_TRACE_OBJECT_ENTRY *pointer);         */
/**                                                                                             */
/************************************************************************************************/
/************************************************************************************************/
UCHAR  *_tx_misra_object_to_uchar_pointer_convert(TX_TRACE_OBJECT_ENTRY *pointer)
{

    /* Return a UCHAR pointer.  */
    return((UCHAR *) ((VOID *) pointer));
}


/************************************************************************************************/
/************************************************************************************************/
/**                                                                                             */
/**  TX_TRACE_OBJECT_ENTRY  *_tx_misra_uchar_to_object_pointer_convert(UCHAR *pointer);         */
/**                                                                                             */
/************************************************************************************************/
/************************************************************************************************/
TX_TRACE_OBJECT_ENTRY  *_tx_misra_uchar_to_object_pointer_convert(UCHAR *pointer)
{

    /* Return an object entry pointer.  */
    return((TX_TRACE_OBJECT_ENTRY *) ((VOID *) pointer));
}


/******************************************************************************************/
/******************************************************************************************/
/**                                                                                       */
/**  TX_TRACE_HEADER  *_tx_misra_uchar_to_header_pointer_convert(UCHAR *pointer);         */
/**                                                                                       */
/******************************************************************************************/
/******************************************************************************************/
TX_TRACE_HEADER  *_tx_misra_uchar_to_header_pointer_convert(UCHAR *pointer)
{

    /* Return a trace header pointer.  */
    return((TX_TRACE_HEADER *) ((VOID *) pointer));
}


/***********************************************************************************************/
/***********************************************************************************************/
/**                                                                                            */
/**  TX_TRACE_BUFFER_ENTRY  *_tx_misra_uchar_to_entry_pointer_convert(UCHAR *pointer);         */
/**                                                                                            */
/***********************************************************************************************/
/***********************************************************************************************/
TX_TRACE_BUFFER_ENTRY  *_tx_misra_uchar_to_entry_pointer_convert(UCHAR *pointer)
{

    /* Return a trace buffer entry pointer.  */
    return((TX_TRACE_BUFFER_ENTRY *) ((VOID *) pointer));
}


/***********************************************************************************************/
/***********************************************************************************************/
/**                                                                                            */
/**  UCHAR  *_tx_misra_entry_to_uchar_pointer_convert(TX_TRACE_BUFFER_ENTRY *pointer);         */
/**                                                                                            */
/***********************************************************************************************/
/***********************************************************************************************/
UCHAR  *_tx_misra_entry_to_uchar_pointer_convert(TX_TRACE_BUFFER_ENTRY *pointer)
{

    /* Return a UCHAR pointer.  */
    return((UCHAR *) ((VOID *) pointer));
}

#endif


/***********************************************************************************************/
/***********************************************************************************************/
/**                                                                                            */
/**  UCHAR  *_tx_misra_char_to_uchar_pointer_convert(CHAR *pointer);                           */
/**                                                                                            */
/***********************************************************************************************/
/***********************************************************************************************/
UCHAR  *_tx_misra_char_to_uchar_pointer_convert(CHAR *pointer)
{

    /* Return a UCHAR pointer.  */
    return((UCHAR *) ((VOID *) pointer));
}


#endif

