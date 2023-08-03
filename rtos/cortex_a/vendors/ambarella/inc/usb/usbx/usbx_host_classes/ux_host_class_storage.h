/**************************************************************************/
/*                                                                        */
/*            Copyright (c) 1996-2012 by Express Logic Inc.               */
/*                                                                        */
/*  This software is copyrighted by and is the sole property of Express   */
/*  Logic, Inc.  All rights, title, ownership, or other interests         */
/*  in the software remain the property of Express Logic, Inc.  This      */
/*  software may only be used in accordance with the corresponding        */
/*  license agreement.  Any unauthorized use, duplication, transmission,  */
/*  distribution, or disclosure of this software is expressly forbidden.  */
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */
/*  written consent of Express Logic, Inc.                                */
/*                                                                        */
/*  Express Logic, Inc. reserves the right to modify this software        */
/*  without notice.                                                       */
/*                                                                        */
/*  Express Logic, Inc.                     info@expresslogic.com         */
//  11423 West Bernardo Court               http://www.expresslogic.com   //
/*  San Diego, CA  92127                                                  */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** USBX Component                                                        */
/**                                                                       */
/**   Storage Class                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    ux_host_class_storage.h                             PORTABLE C      */
/*                                                           5.6          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Thierry Giron, Express Logic Inc.                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains all the header and extern functions used by the  */
/*    USBX storage class.                                                 */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  07-01-2007     TCRG                     Initial Version 5.0           */
/*  07-04-2008     TCRG                     Modified comment(s),          */
/*                                            resulting in version 5.1    */
/*  11-11-2008     TCRG                     Modified comment(s),          */
/*                                            resulting in version 5.2    */
/*  07-10-2009     TCRG                     Modified comment(s), ensure   */
/*                                            the SCSI CBW and CSW buffers*/
/*                                            are aligned on 32bit and do */
/*                                            not rely on the compiler    */
/*                                            packing pragma some DMA     */
/*                                            engines of certain OHCI     */
/*                                            controllers (SH7727 in      */
/*                                            particular) requires all    */
/*                                            OHCI data buffers to be     */
/*                                            aligned on 32 bits even     */
/*                                            though OHCI does not demand */
/*                                            it, and added trace logic,  */
/*                                            resulting in version 5.3    */
/*  06-13-2010     TCRG                     Modified comment(s),          */
/*                                            resulting in version 5.4    */
/*  09-01-2011     TCRG                     Modified comment(s),          */
/*                                            resulting in version 5.5    */
/*  10-10-2012     TCRG                     Modified comment(s),          */
/*                                            resulting in version 5.6    */
/*                                                                        */
/**************************************************************************/

#ifndef UX_HOST_CLASS_STORAGE_H
#define UX_HOST_CLASS_STORAGE_H


/* Include the FileX API or the FileX stub API in cases where FileX is not
   available.  */

#if 0
#ifndef  UX_NO_FILEX
#include "fx_api.h"
#else
#include "filex_stub.h"
#endif
#endif

/* Define User configurable Storage Class constants.  */

#ifndef UX_MAX_HOST_LUN
#define UX_MAX_HOST_LUN                                     16
#endif

#ifndef UX_HOST_CLASS_STORAGE_MAX_MEDIA
#define UX_HOST_CLASS_STORAGE_MAX_MEDIA                     1
#endif

#ifndef UX_HOST_CLASS_STORAGE_MAX_LUN
#define UX_HOST_CLASS_STORAGE_MAX_LUN                     1
#endif

#ifndef UX_HOST_CLASS_STORAGE_MEMORY_BUFFER_SIZE
#define UX_HOST_CLASS_STORAGE_MEMORY_BUFFER_SIZE            (1024 * 8)
#endif

#ifndef UX_HOST_CLASS_STORAGE_MAX_TRANSFER_SIZE
#define UX_HOST_CLASS_STORAGE_MAX_TRANSFER_SIZE             (1024UL * 512UL)
#endif

#ifndef UX_HOST_CLASS_STORAGE_THREAD_STACK_SIZE
#define UX_HOST_CLASS_STORAGE_THREAD_STACK_SIZE             UX_THREAD_STACK_SIZE
#endif


#define UX_HOST_CLASS_STORAGE_TRANSPORT_ERROR               1
#define UX_HOST_CLASS_STORAGE_COMMAND_ERROR                 2
#define UX_HOST_CLASS_STORAGE_SENSE_ERROR                   3

typedef struct UX_HOST_CLASS_STORAGE_STRUCT
{

    struct UX_HOST_CLASS_STORAGE_STRUCT
                    *ux_host_class_storage_next_instance;
    UX_HOST_CLASS   *ux_host_class_storage_class;
    UX_DEVICE       *ux_host_class_storage_device;
    UX_INTERFACE    *ux_host_class_storage_interface;
    UX_ENDPOINT     *ux_host_class_storage_bulk_out_endpoint;
    UX_ENDPOINT     *ux_host_class_storage_bulk_in_endpoint;
    UX_ENDPOINT     *ux_host_class_storage_interrupt_endpoint;
    UCHAR           *ux_host_class_storage_cbw;
    UCHAR           *ux_host_class_storage_saved_cbw;
    UCHAR           *ux_host_class_storage_csw;
    UINT            ux_host_class_storage_state;
    UINT            ux_host_class_storage_media_type;
    UINT            ux_host_class_storage_removable_media;
    UINT            ux_host_class_storage_write_protected_media;
    UINT            ux_host_class_storage_max_lun;
    UINT            ux_host_class_storage_lun;
    UINT            ux_host_class_storage_lun_types[UX_MAX_HOST_LUN];
    ULONG           ux_host_class_storage_sector_size;
    ULONG           ux_host_class_storage_lba;
    UINT            (*ux_host_class_storage_transport) (struct UX_HOST_CLASS_STORAGE_STRUCT *storage, UCHAR * data_pointer);
    ULONG           ux_host_class_storage_sense_code;
    UCHAR *         ux_host_class_storage_memory;
    TX_SEMAPHORE    ux_host_class_storage_semaphore;
} UX_HOST_CLASS_STORAGE;


/* Define Host Storage Class Media structure.  */

typedef struct
{

    //FX_MEDIA        ux_host_class_storage_media;
    ULONG           ux_host_class_storage_media_used;
    ULONG           ux_host_class_storage_media_status;
    ULONG           ux_host_class_storage_media_lun;
    ULONG           ux_host_class_storage_media_partition_start;
    ULONG           ux_host_class_storage_media_sector_size;
    ULONG           ux_host_class_storage_media_lba;
    ULONG           ux_host_class_storage_media_wp;
    VOID            *ux_host_class_storage_media_memory;

} UX_HOST_CLASS_STORAGE_MEDIA;


/* Define Storage Class function prototypes.  */

UINT    _ux_host_class_storage_activate(UX_HOST_CLASS_COMMAND *command);
VOID    _ux_host_class_storage_cbw_initialize(UX_HOST_CLASS_STORAGE *storage, UINT direction,
                                       ULONG data_transfer_length, UINT command_length);
UINT    _ux_host_class_storage_configure(UX_HOST_CLASS_STORAGE *storage);
UINT    _ux_host_class_storage_deactivate(UX_HOST_CLASS_COMMAND *command);
UINT    _ux_host_class_storage_device_initialize(UX_HOST_CLASS_STORAGE *storage);
UINT    _ux_host_class_storage_device_reset(UX_HOST_CLASS_STORAGE *storage);
UINT    _ux_host_class_storage_device_support_check(UX_HOST_CLASS_STORAGE *storage);
UINT    _ux_host_class_storage_endpoints_get(UX_HOST_CLASS_STORAGE *storage);
UINT    _ux_host_class_storage_entry(UX_HOST_CLASS_COMMAND *command);
UINT    _ux_host_class_storage_max_lun_get(UX_HOST_CLASS_STORAGE *storage);
UINT    _ux_host_class_storage_media_capacity_get(UX_HOST_CLASS_STORAGE *storage);
UINT    _ux_host_class_storage_media_characteristics_get(UX_HOST_CLASS_STORAGE *storage);
UINT    _ux_host_class_storage_media_format_capacity_get(UX_HOST_CLASS_STORAGE *storage);
UINT    _ux_host_class_storage_media_mount(UX_HOST_CLASS_STORAGE *storage, ULONG sector);
UINT  _ux_host_class_storage_media_log_info(UX_HOST_CLASS_STORAGE *storage);
UINT    _ux_host_class_storage_media_protection_check(UX_HOST_CLASS_STORAGE *storage);
UINT    _ux_host_class_storage_media_read(UX_HOST_CLASS_STORAGE *storage, ULONG sector_start,
                                        ULONG sector_count, UCHAR *data_pointer);
UINT    _ux_host_class_storage_media_write(UX_HOST_CLASS_STORAGE *storage, ULONG sector_start,
                                        ULONG sector_count, UCHAR *data_pointer);
UINT    _ux_host_class_storage_request_sense(UX_HOST_CLASS_STORAGE *storage);
UINT    _ux_host_class_storage_sense_code_translate(UX_HOST_CLASS_STORAGE *storage, UINT status);
UINT    _ux_host_class_storage_start_stop(UX_HOST_CLASS_STORAGE *storage,
                                            ULONG start_stop_signal);
VOID    _ux_host_class_storage_thread_entry(ULONG class_address);
UINT    _ux_host_class_storage_transport(UX_HOST_CLASS_STORAGE *storage, UCHAR *data_pointer);
//UINT    _ux_host_class_storage_transport_bo(UX_HOST_CLASS_STORAGE *storage, UCHAR *data_pointer);
//UINT    _ux_host_class_storage_transport_cb(UX_HOST_CLASS_STORAGE *storage, UCHAR *data_pointer);
//UINT    _ux_host_class_storage_transport_cbi(UX_HOST_CLASS_STORAGE *storage, UCHAR *data_pointer);
UINT    _ux_host_class_storage_unit_ready_test(UX_HOST_CLASS_STORAGE *storage);

#endif
