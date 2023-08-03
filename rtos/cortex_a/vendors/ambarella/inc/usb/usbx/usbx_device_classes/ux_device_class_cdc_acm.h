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
/**   CDC Class                                                           */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    ux_device_class_cdc_acm.h                           PORTABLE C      */
/*                                                           5.6          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Thierry Giron, Express Logic Inc.                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the equivalences for the USBX Device Class CDC    */
/*    ACM component.                                                      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  07-01-2007     TCRG                     Initial Version 5.0           */
/*  11-11-2008     TCRG                     Modified comment(s),          */
/*                                            updated CDC parameter       */
/*                                            typedef, removed unused     */
/*                                            function prototypes,        */
/*                                            and added new               */
/*                                            function prototypes,        */
/*                                            resulting in  version 5.2   */
/*  07-10-2009     TCRG                     Modified comment(s), and      */
/*                                            added trace logic,          */
/*                                            resulting in version 5.3    */
/*  06-13-2010     TCRG                     Modified comment(s),          */
/*                                            resulting in version 5.4    */
/*  09-01-2011     TCRG                     Modified comment(s),          */
/*                                            resulting in version 5.5    */
/*  10-10-2012     TCRG                     Modified comment(s),          */
/*                                            resulting in version 5.6    */
/*                                                                        */
/**************************************************************************/

#ifndef UX_DEVICE_CLASS_CDC_ACM_H
#define UX_DEVICE_CLASS_CDC_ACM_H


/* Define Slave CDC Class Calling Parameter structure */

typedef struct
{
    VOID                    (*instance_activate)(const VOID *arg);
    VOID                    (*instance_deactivate)(const VOID *arg);

} UX_SLAVE_CLASS_CDC_ACM_PARAMETER;

/* Define CDC Class structure.  */

typedef struct
{
    UX_SLAVE_INTERFACE                  *interface;
    UX_SLAVE_CLASS_CDC_ACM_PARAMETER    parameter;
    TX_MUTEX                            endpoint_in_mutex;
    TX_MUTEX                            endpoint_out_mutex;

} UX_SLAVE_CLASS_CDC_ACM;


/* Define Device CDC Class prototypes.  */

UINT  _ux_device_class_cdc_acm_activate(UX_SLAVE_CLASS_COMMAND *command);
VOID  _ux_device_class_cdc_acm_control_request(VOID);
UINT  _ux_device_class_cdc_acm_deactivate(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_cdc_acm_entry(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_cdc_acm_initialize(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_cdc_acm_write(UX_SLAVE_CLASS_CDC_ACM *cdc_acm, UCHAR *buffer,
                                ULONG requested_length, ULONG *actual_length);
UINT  _ux_device_class_cdc_acm_read(UX_SLAVE_CLASS_CDC_ACM *cdc_acm, UCHAR *buffer,
                                ULONG requested_length, ULONG *actual_length);
UINT  _ux_device_class_cdc_acm_reset(UX_SLAVE_CLASS_COMMAND *command);
UINT  udc_cdc_acm_get_control_bitmap(UINT32 instance_id);
//VOID  udc_cdc_acm_clear_control_bitmap(UINT32 instance_id);
//VOID  udc_cdc_acm_clear_control_bitmap_all(void);
UINT _ux_device_class_cdc_acm_write_ex(UX_SLAVE_CLASS_CDC_ACM *cdc_acm, UCHAR *buffer,
                                       ULONG requested_length, ULONG *actual_length, ULONG timeout);
UINT _ux_device_class_cdc_acm_read_ex(UX_SLAVE_CLASS_CDC_ACM *cdc_acm, UCHAR *buffer,
                                      ULONG requested_length, ULONG *actual_length, ULONG timeout);


// aligned buffer
// non-aligned buffer
extern UX_SLAVE_CLASS_CDC_ACM uxd_cdc_acm_class_memory;

#endif /* UX_DEVICE_CLASS_CDC_ACM_H */
