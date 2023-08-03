/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
 * Copyright 2020, Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */
#include <stdio.h>
#include "AmbaTypes.h"
#include "proto.h"

#define NUM_DEVICES  2
#define DEVICE_IDX_OTP       0
#define DEVICE_IDX_OTP_INFO  1

static int dev_id_array [NUM_DEVICES] = {-1, -1};

static char buffer_info[2048] = {'\0'};

int main(int argc, char *argv[])
{
    static iofunc_attr_t ioattr_array[NUM_DEVICES];
    static char *dev_name_array [NUM_DEVICES] = {
        "/dev/otp",
        "/dev/otpinfo"
    };

    int id;
    resmgr_connect_funcs_t connect_funcs;
    resmgr_io_funcs_t io_funcs;
    dispatch_t *dpp;
    resmgr_attr_t rattr;
    dispatch_context_t *ctp;
    int flag_leave = 0;
    int i;

    (void)argc;

    /* Initialize the dispatch interface */
    dpp = dispatch_create();
    if (!dpp) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
              "OTP error: Failed to create dispatch interface\n");
    } else {
        /* Initialize the resource manager attributes */
        memset(&rattr, 0, sizeof(rattr));
        rattr.nparts_max = 1;
        rattr.msg_max_size = 2048;

        /* Initialize the connect functions */
        iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                         _RESMGR_IO_NFUNCS, &io_funcs);
        io_funcs.devctl = otp_io_devctl;
        io_funcs.read   = otp_io_read;

        for (i = 0; i < NUM_DEVICES; i++) {
            iofunc_attr_init(&ioattr_array[i], S_IFCHR | 0600, NULL, NULL);
            ioattr_array[i].nbytes = sizeof(buffer_info);

            /* Attach the device name */
            id = resmgr_attach(dpp, &rattr, dev_name_array[i], _FTYPE_ANY, 0,
                               &connect_funcs, &io_funcs, &ioattr_array[i]);
            if (id == -1) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                      "OTP error: Failed to attach pathname %s\n", dev_name_array[i]);
                flag_leave = 1;
            } else {
                dev_id_array[i] = id;
            }
        }

        if (flag_leave == 0) {

            /* Allocate a context structure */
            ctp = dispatch_context_alloc(dpp);

            if (0x0 != otp_dev_init()) {
                fprintf(stderr, "otp_dev init fail %s(%d)\n", __func__, __LINE__);
            }

            /* Run in the background */
            if (procmgr_daemon(EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE
                               | PROCMGR_DAEMON_NODEVNULL ) == -1) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "%s:  procmgr_daemon",
                      argv[0]);

            } else {

                printf("otp driver init done\n");

                while (1) {
                    if ((ctp = dispatch_block(ctp)) == NULL) {
                        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                              "OTP error: Block error\n");
                        break;
                    } else {
                        dispatch_handler(ctp);
                    }
                }
            }
        }
    }
    return EXIT_SUCCESS;
}

static int flag_info_read = 0;
static char buffer_string[512];

static void otp_add_hex_info(const char *Title, UINT8 *Data, UINT32 Size)
{

    UINT32 i;
    strcat(buffer_info, Title);
    strcat(buffer_info, "\n");
    for (i = 0; i < Size; i += 16) {
        strcat(buffer_info, "    ");
        sprintf(buffer_string, "%02X:%02X:%02X:%02X:", Data[0], Data[1], Data[2], Data[3]);
        strcat(buffer_info, buffer_string);
        sprintf(buffer_string, "%02X:%02X:%02X:%02X:", Data[4], Data[5], Data[6], Data[7]);
        strcat(buffer_info, buffer_string);
        sprintf(buffer_string, "%02X:%02X:%02X:%02X:", Data[8], Data[9], Data[10], Data[11]);
        strcat(buffer_info, buffer_string);
        sprintf(buffer_string, "%02X:%02X:%02X:%02X:", Data[12], Data[13], Data[14], Data[15]);
        strcat(buffer_info, buffer_string);
        strcat(buffer_info, "\n");
    }
}

static void otp_add_pubkey_info(UINT32 KeyStatus[3])
{
    UINT32 i;
    strcat(buffer_info, "Public Key:\n");
    for (i = 0; i < 3; i++) {
        UINT32 status = KeyStatus[i];
        UINT32 flag_locked = 0;
        UINT32 flag_valid = 1;
        if ((status & 0x01) != 0) {
            flag_locked = 1;
        }
        if ((status & 0x02) != 0) {
            flag_valid = 0;
        }
        if (flag_locked == 0) {
            sprintf(buffer_string, "    Key %d: not used\n", i);
        } else {
            if (flag_valid == 1) {
                sprintf(buffer_string, "    Key %d: valid\n", i);
            } else {
                sprintf(buffer_string, "    Key %d: invalid\n", i);
            }
        }
        strcat(buffer_info, buffer_string);
    }
}

static void otp_update_info(void)
{
    static UINT8 buffer_data[512];
    static UINT32 key_status[3];
    if (flag_info_read == 0) {
        buffer_info[0] = '\0';
        (void)AmbaRTSL_OtpAmbaUniqueIDRead(buffer_data, 16);
        otp_add_hex_info("Ambarella Unique ID:", buffer_data, 16);

        (void)AmbaRTSL_OtpPublicKeyRead(buffer_data, 512, 0, &key_status[0]);
        (void)AmbaRTSL_OtpPublicKeyRead(buffer_data, 512, 1, &key_status[1]);
        (void)AmbaRTSL_OtpPublicKeyRead(buffer_data, 512, 2, &key_status[2]);
        otp_add_pubkey_info(key_status);
        flag_info_read = 1;
    }
}

int otp_io_read (resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
    int nret = 0;
    size_t nleft;
    size_t nbytes;
    int nparts;
    int status;

    //printf("otp_io_read(): id = %d\n", ctp->id);

    if (dev_id_array[DEVICE_IDX_OTP_INFO] == ctp->id) {
        if ((status = iofunc_read_verify (ctp, msg, ocb, NULL)) != EOK) {
            return (status);
        }

        if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE) {
            return (ENOSYS);
        }

        otp_update_info();

        /*
        * On all reads (first and subsequent), calculate
        * how many bytes we can return to the client,
        * based upon the number of bytes available (nleft)
        * and the client's buffer size
        */
        nleft = ocb->attr->nbytes - ocb->offset;
        nbytes = min (_IO_READ_GET_NBYTES(msg), nleft);

        if (nbytes > 0) {

            /* set up the return data IOV */
            SETIOV (ctp->iov, buffer_info + ocb->offset, nbytes);

            /* set up the number of bytes (returned by client's read()) */
            _IO_SET_READ_NBYTES (ctp, nbytes);

            /*
            * advance the offset by the number of bytes
            * returned to the client.
            */
            ocb->offset += nbytes;
            nparts = 1;

        } else {

            /*
            * they've asked for zero bytes or they've already previously
            * read everything
            */
            _IO_SET_READ_NBYTES (ctp, 0);
            nparts = 0;

        }

        /* mark the access time as invalid (we just accessed it) */
        if (msg->i.nbytes > 0) {
            ocb->attr->flags |= IOFUNC_ATTR_ATIME;
        }

        nret = _RESMGR_NPARTS (nparts);

    } else {
        nret = ENOSYS;
    }

    return nret;
}

int otp_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    int err = EOK;
    otp_msg_pubkey_t             *otp_msg_pubkey;
    otp_msg_amba_unique_id_t     *otp_msg_amba_uni_id;
    otp_msg_customer_unique_id_t *otp_msg_customer_uni_id;
    unsigned int *mono_counter;
    otp_msg_pubkey_revoke_t      *otp_msg_pubkey_revoke;
    otp_msg_aeskey_t             *otp_msg_aeskey;
    otp_msg_dx_zone_t            *otp_msg_dx_zone;
    otp_msg_b2_zone_t            *otp_msg_b2_zone;
    UINT32 uret_func;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    nbytes = 0;
    switch (msg->i.dcmd) {
        printf("otp_io_devctl(): command %d\n", msg->i.dcmd);
    case DCMD_OTP_PUBKEY_SET:
        otp_msg_pubkey = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_OtpPublicKeyWrite(otp_msg_pubkey->data,
                                               sizeof(otp_msg_pubkey->data),
                                               otp_msg_pubkey->index);
        if (uret_func == 0U) {
            nbytes = 0;
            // need to update information
            flag_info_read = 1;
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to write public key 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;

    case DCMD_OTP_PUBKEY_GET:
        otp_msg_pubkey = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_OtpPublicKeyRead(otp_msg_pubkey->data,
                                              sizeof(otp_msg_pubkey->data),
                                              otp_msg_pubkey->index,
                                              &otp_msg_pubkey->status);

        if (uret_func == 0U) {
            nbytes = sizeof(otp_msg_pubkey_t);
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to read public key 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_OTP_AMBA_UNI_ID_GET:
        otp_msg_amba_uni_id = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_OtpAmbaUniqueIDRead(otp_msg_amba_uni_id->data,
                    sizeof(otp_msg_amba_uni_id->data));

        if (uret_func == 0U) {
            nbytes = sizeof(otp_msg_amba_unique_id_t);
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to read amba unique id 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_OTP_CUSTOMER_UNI_ID_GET:
        otp_msg_customer_uni_id = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_OtpCustomerUniIDRead(otp_msg_customer_uni_id->data,
                    sizeof(otp_msg_customer_uni_id->data));

        if (uret_func == 0U) {
            nbytes = sizeof(otp_msg_customer_unique_id_t);
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to read customer unique id 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_OTP_CUSTOMER_UNI_ID_SET:
        otp_msg_customer_uni_id = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_OtpCustomerUniIDWrite(otp_msg_customer_uni_id->data,
                    sizeof(otp_msg_customer_uni_id->data));
        if (uret_func == 0U) {
            nbytes = 0;
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to write public key 0x%X\n", uret_func);
            err = (int)uret_func;
        }

        break;
    case DCMD_OTP_MONO_CNT_READ:
        mono_counter = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_OtpMonoCounterRead(mono_counter);

        if (uret_func == 0U) {
            nbytes = sizeof(unsigned int);
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to read monotonic counter 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_OTP_MONO_CNT_INCREASE:
        uret_func = AmbaRTSL_OtpMonoCounterIncrease();
        if (uret_func == 0U) {
            nbytes = 0;
            // need to update information
            flag_info_read = 1;
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to increase monotonic counter 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_OTP_SECURE_BOOT_SET:
        uret_func = AmbaRTSL_OtpSecureBootEnable();
        if (uret_func == 0U) {
            nbytes = 0;
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to set secure boot 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_OTP_PUBKEY_REVOKE:
        otp_msg_pubkey_revoke = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_OtpPublicKeyRevoke(otp_msg_pubkey_revoke->index);
        if (uret_func == 0U) {
            nbytes = 0;
            // need to update information
            flag_info_read = 1;
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to revoke public key %d, code 0x%X\n", otp_msg_pubkey_revoke->index, uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_OTP_AESKEY_SET:
        otp_msg_aeskey = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_OtpAesKeyWrite(otp_msg_aeskey->data,
                                            sizeof(otp_msg_aeskey->data),
                                            otp_msg_aeskey->index);
        if (uret_func == 0U) {
            nbytes = 0;
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to write aes key %d, code 0x%X\n", otp_msg_aeskey->index, uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_OTP_AESKEY_GET:
        otp_msg_aeskey = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_OtpAesKeyRead(otp_msg_aeskey->data,
                                           sizeof(otp_msg_aeskey->data),
                                           otp_msg_aeskey->index);

        if (uret_func == 0U) {
            nbytes = sizeof(otp_msg_aeskey_t);
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to read aes key 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_OTP_RSVD_DX_GET:
        otp_msg_dx_zone = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_OtpReservedDXRead(otp_msg_dx_zone->data,
                                               sizeof(otp_msg_dx_zone->data),
                                               otp_msg_dx_zone->index);

        if (uret_func == 0U) {
            nbytes = sizeof(otp_msg_dx_zone);
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to read dx zone %d, code 0x%X\n", otp_msg_dx_zone->index, uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_OTP_RSVD_DX_SET:
        otp_msg_dx_zone = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_OtpReservedDXWrite(otp_msg_dx_zone->data,
                                                sizeof(otp_msg_dx_zone->data),
                                                otp_msg_dx_zone->index);
        if (uret_func == 0U) {
            nbytes = 0;
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to write dx zone %d, code 0x%X\n", otp_msg_dx_zone->index, uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_OTP_RSVD_B2_GET:
        otp_msg_b2_zone = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_OtpReservedB2Read(otp_msg_b2_zone->data,
                                               sizeof(otp_msg_b2_zone->data));

        if (uret_func == 0U) {
            nbytes = sizeof(otp_msg_b2_zone);
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to read b2 zone, code 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_OTP_RSVD_B2_SET:
        otp_msg_b2_zone = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_OtpReservedB2Write(otp_msg_b2_zone->data,
                                                sizeof(otp_msg_b2_zone->data));
        if (uret_func == 0U) {
            nbytes = 0;
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "OTP error: Failed to write b2 zone, code 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    default:
        nbytes = 0;
        err = EINVAL;
        break;
    }

    if (nbytes == 0) {
        return (err);
    } else {
        msg->o.ret_val = 0;
        msg->o.nbytes = nbytes;
        return (_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + nbytes));
    }
}
