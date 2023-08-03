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
/**   System                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    ux_system.h                                         PORTABLE C      */
/*                                                           5.6          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Thierry Giron, Express Logic Inc.                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains all the header and extern functions used by the  */
/*    USBX main system component.                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  07-01-2007     TCRG                     Initial Version 5.0           */
/*  11-11-2008     TCRG                     Modified comment(s),          */
/*                                            resulting in version 5.2    */
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

#ifndef UX_SYSTEM_HOST_H
#define UX_SYSTEM_HOST_H


/* Define System component external data references.  */

extern UX_SYSTEM *_ux_system;
extern UX_SYSTEM_HOST *_ux_system_host;
extern UX_SYSTEM_SLAVE *_ux_system_slave;
extern UX_SYSTEM_OTG *_ux_system_otg;

extern UCHAR _ux_system_device_descriptor_structure[14];

#endif

