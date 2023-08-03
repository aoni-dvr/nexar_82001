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
/**   PIMA Class                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    ux_device_class_pima.h                              PORTABLE C      */
/*                                                           5.6          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Thierry Giron, Express Logic Inc.                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains all the header and extern functions used by the  */
/*    USBX PIMA class.                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  11-11-2008     TCRG                     Initial Version 5.2           */
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

#ifndef UX_DEVICE_CLASS_PIMA_H
#define UX_DEVICE_CLASS_PIMA_H
#define UX_PIMA_WITH_MTP_SUPPORT
/* Define PIMA Class constants.  */

#define UX_DEVICE_CLASS_PIMA_CLASS_TRANSFER_TIMEOUT                 				300000
#define UX_DEVICE_CLASS_PIMA_CLASS                                  				0x06
#define UX_DEVICE_CLASS_PIMA_SUBCLASS                               				0X01
#define UX_DEVICE_CLASS_PIMA_PROTOCOL                               				0X01
#define UX_DEVICE_CLASS_PIMA_MAGIC_NUMBER                           				0x50494D41
#define UX_DEVICE_CLASS_PIMA_MAX_PAYLOAD                            				1024
#define UX_DEVICE_CLASS_PIMA_OBJECT_INFO_BUFFER_SIZE                				16384
#define UX_DEVICE_CLASS_PIMA_OBJECT_DATA_BUFFER_SIZE                				UX_SLAVE_REQUEST_BULKIN_DATA_MTP_MAX_LENGTH
#define UX_DEVICE_CLASS_PIMA_DEVICE_INFO_BUFFER_SIZE                				1024
#define UX_DEVICE_CLASS_PIMA_STORAGE_INFO_BUFFER_SIZE               				1024
#define UX_DEVICE_CLASS_PIMA_ARRAY_BUFFER_SIZE                      				1024
#define UX_DEVICE_CLASS_PIMA_MAX_EVENTS_QUEUE                       				16
#define UX_DEVICE_CLASS_PIMA_MAX_STORAGE_IDS                        				1
#define UX_DEVICE_CLASS_PIMA_UNICODE_MAX_LENGTH                     				256
#define UX_DEVICE_CLASS_PIMA_ARRAY_MAX_LENGTH                       				256
#define UX_DEVICE_CLASS_PIMA_DATE_TIME_STRING_MAX_LENGTH            				64
#define UX_DEVICE_CLASS_PIMA_DEVICE_PROPERTIES_ARRAY_MAX_ITEMS						32
#define UX_DEVICE_CLASS_PIMA_OBJECT_PROPERTIES_ARRAY_MAX_ITEMS						128
#define UX_DEVICE_CLASS_PIMA_PROP_VALUE_SIZE          								256
#define UX_DEVICE_CLASS_PIMA_MICROSOFT_VENDOR_COMMAND_CODE							0x54

/* Define PIMA versions.  */
#define UX_DEVICE_CLASS_PIMA_STANDARD_VERSION                       				100
#define UX_DEVICE_CLASS_PIMA_VENDOR_EXTENSION_ID                    				6
#define UX_DEVICE_CLASS_PIMA_EXTENSION_VERSION                      				100
#define UX_DEVICE_CLASS_PIMA_STANDARD_MODE                          				0

/* Define PIMA Reset Request equivalences.  */

#define UX_DEVICE_CLASS_PIMA_REQUEST_RESET_DEVICE                   				0x66

/* Define PIMA command container type.  */

#define UX_DEVICE_CLASS_PIMA_CT_UNDEFINED                           				0x00
#define UX_DEVICE_CLASS_PIMA_CT_COMMAND_BLOCK                       				0x01
#define UX_DEVICE_CLASS_PIMA_CT_DATA_BLOCK                          				0x02
#define UX_DEVICE_CLASS_PIMA_CT_RESPONSE_BLOCK                      				0x03
#define UX_DEVICE_CLASS_PIMA_CT_EVENT_BLOCK                         				0x04

/* Define PIMA Extended Event Data Request payload Format.  */

#define UX_DEVICE_CLASS_PIMA_EEDR_EVENT_CODE                        				0x00
#define UX_DEVICE_CLASS_PIMA_EEDR_TRANSACTION_ID                    				0x02
#define UX_DEVICE_CLASS_PIMA_EEDR_NUMBER_PARAMETERS                 				0x06
#define UX_DEVICE_CLASS_PIMA_EEDR_SIZE_PARAMETER                    				0x08

/* Define PIMA Device Status Data Format.  */

#define UX_DEVICE_CLASS_PIMA_DSD_LENGTH                             				0x00
#define UX_DEVICE_CLASS_PIMA_DSD_CODE                               				0x02
#define UX_DEVICE_CLASS_PIMA_DSD_PARAMETER                          				0x04

/* Define PIMA Response Codes.  */

#define UX_DEVICE_CLASS_PIMA_RC_UNDEFINED                           				0x2000
#define UX_DEVICE_CLASS_PIMA_RC_OK                                  				0x2001
#define UX_DEVICE_CLASS_PIMA_RC_GENERAL_ERROR                       				0x2002
//#define UX_DEVICE_CLASS_PIMA_RC_SESSION_NOT_OPEN                    				0x2003
//#define UX_DEVICE_CLASS_PIMA_RC_INVALID_TRANSACTION_ID              				0x2004
#define UX_DEVICE_CLASS_PIMA_RC_OPERATION_NOT_SUPPORTED             				0x2005
//#define UX_DEVICE_CLASS_PIMA_RC_PARAMETER_NOT_SUPPORTED             				0x2006
//#define UX_DEVICE_CLASS_PIMA_RC_INCOMPLETE_TRANSFER                 				0x2007
//#define UX_DEVICE_CLASS_PIMA_RC_INVALID_STORAGE_ID                  				0x2008
//#define UX_DEVICE_CLASS_PIMA_RC_INVALID_OBJECT_HANDLE               				0x2009
#define UX_DEVICE_CLASS_PIMA_RC_DEVICE_PROP_NOT_SUPPORTED           				0x200A
//#define UX_DEVICE_CLASS_PIMA_RC_INVALID_OBJECT_FORMAT_CODE          				0x200B
//#define UX_DEVICE_CLASS_PIMA_RC_STORE_FULL                          				0x200C
//#define UX_DEVICE_CLASS_PIMA_RC_OBJECT_WRITE_PROTECTED              				0x200D
//#define UX_DEVICE_CLASS_PIMA_RC_STORE_READ_ONLY                     				0x200E
//#define UX_DEVICE_CLASS_PIMA_RC_ACCESS_DENIED                       				0x200F
//#define UX_DEVICE_CLASS_PIMA_RC_NO_THUMBNAIL_PRESENT                				0x2010
//#define UX_DEVICE_CLASS_PIMA_RC_SELF_TEST_FAILED                    				0x2011
//#define UX_DEVICE_CLASS_PIMA_RC_PARTIAL_DELETION                    				0x2012
//#define UX_DEVICE_CLASS_PIMA_RC_STORE_NOT_AVAILABLE                 				0x2013
//#define UX_DEVICE_CLASS_PIMA_RC_FORMAT_UNSUPPORTED                  				0x2014
//#define UX_DEVICE_CLASS_PIMA_RC_NO_VALID_OBJECT_INFO                				0x2015
//#define UX_DEVICE_CLASS_PIMA_RC_INVALID_CODE_FORMAT                 				0x2016
//#define UX_DEVICE_CLASS_PIMA_RC_UNKNOWN_VENDOR_CODE                 				0x2017
//#define UX_DEVICE_CLASS_PIMA_RC_CAPTURE_ALREADY_TERMINATED          				0x2018
//#define UX_DEVICE_CLASS_PIMA_RC_DEVICE_BUSY                         				0x2019
//#define UX_DEVICE_CLASS_PIMA_RC_INVALID_PARENT_OBJECT               				0x201A
//#define UX_DEVICE_CLASS_PIMA_RC_INVALID_DEVICE_PROP_FORMAT          				0x201B
//#define UX_DEVICE_CLASS_PIMA_RC_INVALID_DEVICE_PROP_VALUE           				0x201C
//#define UX_DEVICE_CLASS_PIMA_RC_INVALID_PARAMETER                   				0x201D
//#define UX_DEVICE_CLASS_PIMA_RC_SESSION_ALREADY_OPENED              				0x201E
//#define UX_DEVICE_CLASS_PIMA_RC_TRANSACTION_CANCELED                				0x201F
//#define UX_DEVICE_CLASS_PIMA_RC_DESTINATION_UNSUPPORTED             				0x2020
//#define UX_DEVICE_CLASS_PIMA_RC_OBJECT_ALREADY_OPENED               				0x2021
//#define UX_DEVICE_CLASS_PIMA_RC_OBJECT_ALREADY_CLOSED               				0x2022
//#define UX_DEVICE_CLASS_PIMA_RC_OBJECT_NOT_OPENED                   				0x2023


/* Define PIMA Object Protection Status Values.  */

#define UX_DEVICE_CLASS_PIMA_OPS_NO_PROTECTION                      				0x0000
#define UX_DEVICE_CLASS_PIMA_OPS_READ_ONLY                          				0x0001


/* Define PIMA vendor process info. */
typedef struct UX_SLAVE_CLASS_PIMA_VENDOR_PROCESS_INFO_STRUCT{
    UINT32 OpCode;
    UINT32 TransactionID;
    UINT32 parmCnt;
    UINT32 parm[5];
} UX_SLAVE_CLASS_PIMA_VENDOR_PROCESS_INFO;

/* Define PIMA event info structure.  */

typedef struct
{
    UINT                   code;
    UINT                   session_id;
    UINT                   transaction_id;
    UINT                   length_change;
    UINT                   parameter_number;
    UINT                   parameter_1;
    UINT                   parameter_2;
    UINT                   parameter_3;

} UX_SLAVE_CLASS_PIMA_EVENT;

/* Define PIMA object info structure.  */

typedef struct
{

    UINT                    storage_id;
    UINT                    format;
    UINT                    protection_status;
    UINT                    compressed_size;
    UINT                    thumb_format;
    UINT                    thumb_compressed_size;
    UINT                    thumb_pix_width;
    UINT                    thumb_pix_height;
    UINT                    image_pix_width;
    UINT                    image_pix_height;
    UINT                    image_bit_depth;
    UINT                    parent_object;
    UINT                    association_type;
    UINT                    association_desc;
    UINT                    sequence_number;
    UCHAR                   filename[UX_DEVICE_CLASS_PIMA_UNICODE_MAX_LENGTH];
    UCHAR                   capture_date[UX_DEVICE_CLASS_PIMA_DATE_TIME_STRING_MAX_LENGTH];
    UCHAR                   modification_date[UX_DEVICE_CLASS_PIMA_DATE_TIME_STRING_MAX_LENGTH];
    UCHAR                   keywords[UX_DEVICE_CLASS_PIMA_UNICODE_MAX_LENGTH];
    UINT                    state;
    UINT                    offset;
    UINT                    transfer_status;
    UINT                    handle_id;
    UINT                    length;
    UCHAR                   *buffer;

} UX_SLAVE_CLASS_PIMA_OBJECT;

#define UX_SLAVE_CLASS_PIMA_OBJECT_DATA_LENGTH ((15 * sizeof(UINT)) + \
                                                    UX_DEVICE_CLASS_PIMA_UNICODE_MAX_LENGTH + \
                                                    UX_DEVICE_CLASS_PIMA_DATE_TIME_STRING_MAX_LENGTH + \
                                                    UX_DEVICE_CLASS_PIMA_DATE_TIME_STRING_MAX_LENGTH + \
                                                    UX_DEVICE_CLASS_PIMA_UNICODE_MAX_LENGTH)

/* Define PIMA session info structure.  Not used in the device. Here for structure compatibility. */

typedef struct
{

    ULONG                   ux_device_class_pima_session_id;

} UX_SLAVE_CLASS_PIMA_SESSION;

typedef struct
{
	UCHAR *data;
	ULONG size;
	ULONG result;
	ULONG zlp_required;
	UX_SLAVE_ENDPOINT *ux_endpoint_in;
} UDC_PIMA_BULK_SEND_INFO;

#define UDC_PIMA_BULK_SEND_START 0x10
#define UDC_PIMA_BULK_SEND_DONE  0x01

typedef UINT (*udc_pima_cb_prop_list_get)(VOID *pima, ULONG *params, UCHAR **device_proplist, ULONG *device_prop_list_length);

/* Define PIMA structure.  */

typedef struct UX_SLAVE_CLASS_PIMA_STRUCT
{

    UX_SLAVE_INTERFACE      *ux_slave_class_pima_interface;
    UX_SLAVE_ENDPOINT       *bulk_in_endpoint;
    UX_SLAVE_ENDPOINT       *bulk_out_endpoint;
    UX_SLAVE_ENDPOINT       *interrupt_endpoint;
    UINT                    state;
    ULONG                   session_id;
    ULONG                   current_object_handle;
    ULONG                   transaction_id;
    UCHAR                   *manufacturer;
    UCHAR                   *model;
    UCHAR                   *device_version;
    UCHAR                   *serial_number;
    ULONG                   storage_id;
    ULONG                   storage_type;
    ULONG                   storage_file_system_type;
    ULONG                   storage_access_capability;
    ULONG                   storage_max_capacity_low;
    ULONG                   storage_max_capacity_high;
    ULONG                   storage_free_space_low;
    ULONG                   storage_free_space_high;
    ULONG                   storage_free_space_image;
    UCHAR                   *storage_description;
    UCHAR                   *storage_volume_label;
    TX_SEMAPHORE            semaphore;
    TX_THREAD               interrupt_thread;
    UCHAR                   *interrupt_thread_stack;
    TX_SEMAPHORE            interrupt_thread_semaphore;
    UX_SLAVE_CLASS_PIMA_EVENT
                            *event_array;
    UX_SLAVE_CLASS_PIMA_EVENT
                            *event_array_head;
    UX_SLAVE_CLASS_PIMA_EVENT
                            *event_array_tail;
    UX_SLAVE_CLASS_PIMA_EVENT
                            *event_array_end;
    USHORT					*device_properties_list;
    USHORT					*supported_capture_formats_list;
    USHORT					*supported_image_formats_list;
    USHORT					*object_properties_list;
    USHORT                  *supported_operation_list;
    UINT                    (*device_reset)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima);

    UINT                    (*device_prop_desc_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG device_property, UCHAR **device_prop_dataset, ULONG *device_prop_dataset_length);
    UINT                    (*device_prop_value_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG device_property, UCHAR **device_prop_value, ULONG *device_prop_value_length);
    UINT                    (*device_prop_value_set)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG device_property, UCHAR *device_prop_value, ULONG device_prop_value_length);
    UINT                    (*device_prop_value_reset)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG device_property);
    udc_pima_cb_prop_list_get  prop_list_get;
    UINT                    (*storage_format)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG storage_id);
    UINT                    (*storage_info_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG storage_id);
    UINT                    (*object_number_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_format_code, ULONG object_association, ULONG *object_number);
    UINT                    (*object_handles_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG format_code,
                                                                        ULONG association,
                                                                        ULONG **handles_array,
                                                                        ULONG *handles_number);
    UINT                    (*object_info_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, UX_SLAVE_CLASS_PIMA_OBJECT **object);
    UINT                    (*object_data_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, UCHAR *object_buffer, ULONG object_offset,
                                                                ULONG object_length_requested, ULONG *object_actual_length, USHORT opcode);
    UINT                    (*object_data_get_ex)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, UCHAR *object_buffer, UINT64 object_offset,
                                                                    UINT64 object_length_requested, UINT64 *object_actual_length, USHORT opcode);
    UINT                    (*object_thumb_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, UCHAR *object_buffer, ULONG object_offset,
                                                                ULONG object_length_requested, ULONG *object_actual_length);
    UINT                    (*object_info_send)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, UX_SLAVE_CLASS_PIMA_OBJECT *object, ULONG storage_id, ULONG parent_object_handle, ULONG *object_handle);
    UINT                    (*object_data_send)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, ULONG phase, UCHAR *object_buffer, ULONG object_offset,
                                                                ULONG object_length);
    UINT                    (*object_data_send_ex)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, ULONG phase, UCHAR *object_buffer, UINT64 object_offset,
                                                                    UINT64 object_length);
    UINT                    (*object_delete)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle);
    UINT                    (*object_prop_desc_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_property, ULONG object_format_code, UCHAR **object_prop_value_dataset, ULONG *object_prop_value_dataset_length);
    UINT                    (*object_prop_value_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, ULONG object_property, UCHAR **object_prop_value, ULONG *object_prop_value_length);
    UINT                    (*object_prop_value_set)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, ULONG object_property, UCHAR *object_prop_value, ULONG object_prop_value_length);
    UINT                    (*object_references_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, UCHAR **object_handle_array, ULONG *object_handle_array_length);
    UINT                    (*object_references_set)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, UCHAR *object_handle_array, ULONG object_handle_array_length);
    UINT                    (*object_custom_command)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG parameter1, ULONG parameter2, ULONG parameter3, ULONG parameter4, ULONG parameter5, ULONG* Length, ULONG* dir);
    UINT                    (*object_custom_data_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, UCHAR *ObjectBuffer, ULONG ObjectOffset,ULONG ObjectLengthRequested, ULONG *ObjectActualLength);
    UINT                    (*object_custom_data_send)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, UCHAR *ObjectBuffer, ULONG ObjectOffset, ULONG ObjectLength);
    UINT                    (*object_vendor_process)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, struct UX_SLAVE_CLASS_PIMA_VENDOR_PROCESS_INFO_STRUCT *vendor_cmd_info);
    VOID                    *application;
    VOID                    (*transfer_canceled)(void);
    UINT                    (*initiate_capture)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG storage_id, ULONG object_format_code);
    UINT                    (*open_capture_initiate)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG storage_id, ULONG object_format_code);
    UINT                    (*open_capture_terminate)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG trans_id);

	TX_THREAD               bulk_send_thread;
	VOID                    *bulk_send_thread_stack;
	TX_EVENT_FLAGS_GROUP    bulk_send_flag;
	VOID                    *bulk_send_buffer_raw;
	VOID                    *bulk_send_buffer;
	UINT                    bulk_send_buffer_size;
	UDC_PIMA_BULK_SEND_INFO bulk_send_info;

    UINT32                  vendor_extension_id;
    UINT16                  vendor_extension_version;
    UCHAR                   *vendor_extension_desc;

    UINT32                  ObjectXferExtensionVer;
    UINT32                  CancelOnGoing;

} UX_SLAVE_CLASS_PIMA;

/* Define PIMA initialization command structure.  */

typedef struct
{

    UCHAR                   *manufacturer;
    UCHAR                   *model;
    UCHAR                   *device_version;
    UCHAR                   *serial_number;
    ULONG                   storage_id;
    ULONG                   storage_type;
    ULONG                   storage_file_system_type;
    ULONG                   storage_access_capability;
    ULONG                   storage_max_capacity_low;
    ULONG                   storage_max_capacity_high;
    ULONG                   storage_free_space_low;
    ULONG                   storage_free_space_high;
    ULONG                   storage_free_space_image;
    UCHAR                   *storage_description;
    UCHAR                   *storage_volume_label;
    USHORT					*device_properties_list;
    USHORT					*supported_capture_formats_list;
    USHORT					*supported_image_formats_list;
    USHORT					*object_properties_list;
    USHORT                  *supported_operation_list;
    UINT                    (*device_reset)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima);
    UINT                    (*device_prop_desc_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG device_property, UCHAR **device_prop_dataset, ULONG *device_prop_dataset_length);
    UINT                    (*device_prop_value_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG device_property, UCHAR **device_prop_value, ULONG *device_prop_value_length);
    UINT                    (*device_prop_value_set)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG device_property, const UCHAR *device_prop_value, ULONG device_prop_value_length);
    UINT                    (*device_prop_value_reset)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG device_property);
    udc_pima_cb_prop_list_get  prop_list_get;
    UINT                    (*storage_format)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG storage_id);
    UINT                    (*storage_info_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG storage_id);
    UINT                    (*object_number_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_format_code, ULONG object_association, ULONG *object_number);
    UINT                    (*object_handles_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG format_code,
                                                                        ULONG association,
                                                                        ULONG **handles_array,
                                                                        ULONG *handles_number);
    UINT                    (*object_info_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, UX_SLAVE_CLASS_PIMA_OBJECT **object);
    UINT                    (*object_data_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, UCHAR *object_buffer, ULONG object_offset,
                                                                ULONG object_length_requested, ULONG *object_actual_length, USHORT opcode);
    UINT                    (*object_data_get_ex)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, UCHAR *object_buffer, UINT64 object_offset,
                                                                    UINT64 object_length_requested, UINT64 *object_actual_length, USHORT opcode);
    UINT                    (*object_thumb_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, UCHAR *object_buffer, ULONG object_offset,
                                                                    ULONG object_length_requested, ULONG *object_actual_length);
    UINT                    (*object_info_send)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, const UX_SLAVE_CLASS_PIMA_OBJECT *object, ULONG storage_id, ULONG parent_object_handle, ULONG *object_handle);
    UINT                    (*object_data_send)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, ULONG phase ,UCHAR *object_buffer, ULONG object_offset,
                                                                ULONG object_length);
    UINT                    (*object_data_send_ex)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, ULONG phase ,const UCHAR *object_buffer, UINT64 object_offset,
                                                                    UINT64 object_length);
    UINT                    (*object_delete)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle);
    UINT                    (*object_prop_desc_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, ULONG object_property, UCHAR **object_prop_dataset, ULONG *object_prop_dataset_length);
    UINT                    (*object_prop_value_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, ULONG object_property, UCHAR **object_prop_value, ULONG *object_prop_value_length);
    UINT                    (*object_prop_value_set)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, ULONG object_property, UCHAR *object_prop_value, ULONG object_prop_value_length);
    UINT                    (*object_references_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, UCHAR **object_handle_array, ULONG *object_handle_array_length);
    UINT                    (*object_references_set)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG object_handle, UCHAR *object_handle_array, ULONG object_handle_array_length);
    UINT                    (*custom_command)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG parameter1, ULONG parameter2, ULONG parameter3, ULONG parameter4, ULONG parameter5, ULONG* Length, ULONG* dir);
    UINT                    (*custom_data_get)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, UCHAR *ObjectBuffer, ULONG ObjectOffset,ULONG ObjectLengthRequested, ULONG *ObjectActualLength);
    UINT                    (*custom_data_send)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, UCHAR *ObjectBuffer, ULONG ObjectOffset, ULONG ObjectLength);
    UINT                    (*vendor_handler)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, struct UX_SLAVE_CLASS_PIMA_VENDOR_PROCESS_INFO_STRUCT *vendor_cmd_info);
    VOID                    *application;
    VOID                    (*transfer_canceled)(void);
    UINT                    (*initiate_capture)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG storage_id, ULONG object_format_code);
    UINT                    (*open_capture_initiate)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG storage_id, ULONG object_format_code);
    UINT                    (*open_capture_terminate)(struct UX_SLAVE_CLASS_PIMA_STRUCT *pima, ULONG trans_id);

    UINT32                  vendor_extension_id;
    UINT16                  vendor_extension_version;
    UCHAR                   *vendor_extension_desc;

    UINT32                  ObjectXferExtensionVer;

} UX_SLAVE_CLASS_PIMA_PARAMETER;


/* Define PIMA Object decompaction structure.  */

#define UX_DEVICE_CLASS_PIMA_OBJECT_MAX_LENGTH                              512
#define UX_DEVICE_CLASS_PIMA_OBJECT_VARIABLE_OFFSET                         52
#define UX_DEVICE_CLASS_PIMA_OBJECT_ENTRIES                                 15

/* Define Pima Class function prototypes.  */
UINT  _ux_device_class_pima_initialize(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_pima_reset(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_pima_activate(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_pima_deactivate(UX_SLAVE_CLASS_COMMAND *command);
VOID  _ux_device_class_pima_control_complete(UX_SLAVE_TRANSFER *transfer_request);
VOID  _ux_device_class_pima_control_request(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_pima_device_info_send(UX_SLAVE_CLASS_PIMA *pima);
UINT  _ux_device_class_pima_entry(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_pima_event_get(UX_SLAVE_CLASS_PIMA *pima,
                                      UX_SLAVE_CLASS_PIMA_EVENT *pima_event);
UINT  _ux_device_class_pima_event_set(UX_SLAVE_CLASS_PIMA *pima,
                                      UX_SLAVE_CLASS_PIMA_EVENT *pima_event);
VOID  _ux_device_class_pima_interrupt_thread(ULONG pima_class);
UINT  _ux_device_class_pima_response_send(UX_SLAVE_CLASS_PIMA *pima, ULONG response_code,
                                            ULONG number_parameters,
                                            ULONG parameter_1, ULONG parameter_2, ULONG paramater_3);
VOID  _ux_device_class_pima_thread(ULONG pima_class);
VOID  _udc_pima_control_thread(ULONG pima_class);
UINT  _ux_device_class_pima_object_handles_send(UX_SLAVE_CLASS_PIMA *pima,
                                                    ULONG storage_id,
                                                    ULONG object_format_code,
                                                    ULONG object_association);
UINT  _ux_device_class_pima_objects_number_send(UX_SLAVE_CLASS_PIMA *pima,
                                                    ULONG storage_id,
                                                    ULONG object_format_code,
                                                    ULONG object_association);

//UINT  _ux_device_class_pima_device_prop_desc_get(UX_SLAVE_CLASS_PIMA *pima, ULONG device_property_code);
//UINT  _ux_device_class_pima_device_prop_value_get(UX_SLAVE_CLASS_PIMA *pima, ULONG device_property_code);
//UINT  _ux_device_class_pima_device_prop_value_set(UX_SLAVE_CLASS_PIMA *pima, ULONG device_property_code);
//UINT  _ux_device_class_pima_device_prop_value_reset(UX_SLAVE_CLASS_PIMA *pima, ULONG device_property_code);
//UINT  _ux_device_class_pima_device_custom_command(UX_SLAVE_CLASS_PIMA *pima, ULONG param1, ULONG param2, ULONG param3, ULONG param4, ULONG param5);
//UINT  _ux_device_class_pima_object_info_get(UX_SLAVE_CLASS_PIMA *pima, ULONG object_handle);
//UINT  _ux_device_class_pima_object_info_send(UX_SLAVE_CLASS_PIMA *pima, ULONG storage_id, ULONG parent_object_handle);
//UINT  _ux_device_class_pima_object_data_get(UX_SLAVE_CLASS_PIMA *pima, ULONG object_handle);
//UINT  _ux_device_class_pima_object_thumb_get(UX_SLAVE_CLASS_PIMA *pima, ULONG object_handle);
//UINT  _ux_device_class_pima_object_data_send(UX_SLAVE_CLASS_PIMA *pima);
//UINT  _ux_device_class_pima_object_delete(UX_SLAVE_CLASS_PIMA *pima, ULONG object_handle);
//UINT  _ux_device_class_pima_object_add(UX_SLAVE_CLASS_PIMA *pima, ULONG object_handle);
//UINT  _ux_device_class_pima_partial_object_data_get(UX_SLAVE_CLASS_PIMA *pima,
//                                                    ULONG object_handle,
//                                                    ULONG offset_requested,
//                                                    ULONG length_requested);
//
//UINT  _ux_device_class_pima_storage_id_send(UX_SLAVE_CLASS_PIMA *pima);
//UINT  _ux_device_class_pima_storage_info_get(UX_SLAVE_CLASS_PIMA *pima, ULONG storage_id);
//UINT  _ux_device_class_pima_object_props_supported_get(UX_SLAVE_CLASS_PIMA *pima,
//                                                    ULONG object_format_code);
//UINT  _ux_device_class_pima_object_prop_value_get(UX_SLAVE_CLASS_PIMA *pima,
//													ULONG object_handle,
//                                                    ULONG object_property_code);
//UINT  _ux_device_class_pima_object_prop_value_set(UX_SLAVE_CLASS_PIMA *pima,
//													ULONG object_handle,
//                                                    ULONG object_property_code);
//UINT  _ux_device_class_pima_object_prop_desc_get(UX_SLAVE_CLASS_PIMA *pima,
//													ULONG object_property,
//                                                    ULONG object_format_code);
//UINT  _ux_device_class_pima_object_references_get(UX_SLAVE_CLASS_PIMA *pima,
//													ULONG object_handle);
//UINT  _ux_device_class_pima_object_references_set(UX_SLAVE_CLASS_PIMA *pima,
//													ULONG object_handle);
//UINT  _ux_device_class_pima_object_prop_value_get(UX_SLAVE_CLASS_PIMA *pima,
//													ULONG object_handle,
//                                                    ULONG object_property_code);
//
//UINT  _udc_pima_object_prop_list_get(UX_SLAVE_CLASS_PIMA *pima, ULONG *params);
//UINT  _ux_device_class_pima_storage_format(UX_SLAVE_CLASS_PIMA *pima, ULONG storage_id);
//UINT  _ux_device_class_pima_device_reset(UX_SLAVE_CLASS_PIMA *pima);
//ULONG _ux_device_class_pima_get_current_object_size(void);
//UINT  _ux_device_class_pima_set_current_object_size(ULONG size);
//UINT  _ux_device_class_pima_initiate_capture(UX_SLAVE_CLASS_PIMA *pima, ULONG storage_id, ULONG object_format_code);
//UINT  _ux_device_class_pima_open_capture_initiate(UX_SLAVE_CLASS_PIMA *pima, ULONG storage_id, ULONG object_format_code);
//UINT  _ux_device_class_pima_open_capture_terminate(UX_SLAVE_CLASS_PIMA *pima, ULONG trans_id);

UINT32 udc_mtp_enable_protocol_debug(void);
UINT32 udc_mtp_disable_protocol_debug(void);
UINT32 udc_mtp_is_protocol_debug_enabled(void);
struct UX_SLAVE_CLASS_PIMA_STRUCT *udc_mtp_get_context(void);
UINT  _udc_mtp_set_supported_events(USHORT *list);
VOID  udc_pima_bulk_send_thread(ULONG arg);
UINT  udc_pima_wait_bulk_send_done(UX_SLAVE_CLASS_PIMA *instance);
UINT  udc_pima_setup_bulk_send(UX_SLAVE_CLASS_PIMA *instance,
								UX_SLAVE_ENDPOINT *ux_endpoint_in,
								UCHAR *data,
								ULONG size,
								ULONG zlp_required);

#define UXD_PIMA_MAIN_THREAD_STACK_SIZE      (1024*16)
#define UXD_PIMA_SEND_THREAD_STACK_SIZE      (1024*8)
#define UXD_PIMA_INTERRUPT_THREAD_STACK_SIZE (1024*4)
#define UXD_PIMA_READ_BUFFER_SIZE            (1024*256*2)
#define UXD_PIMA_EVENT_ARRAY_SIZE            (UX_DEVICE_CLASS_PIMA_MAX_EVENTS_QUEUE * sizeof(UX_SLAVE_CLASS_PIMA_EVENT))

// aligned buffer
extern UINT8 uxd_pima_read_buffer[UXD_PIMA_READ_BUFFER_SIZE];
// non-aligned buffer
extern UX_SLAVE_CLASS_PIMA uxd_pima_class_memory;
extern UINT8 uxd_pima_main_thread_stack[UXD_PIMA_MAIN_THREAD_STACK_SIZE];
extern UINT8 uxd_pima_send_thread_stack[UXD_PIMA_SEND_THREAD_STACK_SIZE];
extern UINT8 uxd_pima_interrupt_thread_stack[UXD_PIMA_INTERRUPT_THREAD_STACK_SIZE];
extern UX_SLAVE_CLASS_PIMA_EVENT uxd_pima_event_array[UX_DEVICE_CLASS_PIMA_MAX_EVENTS_QUEUE];
#endif
