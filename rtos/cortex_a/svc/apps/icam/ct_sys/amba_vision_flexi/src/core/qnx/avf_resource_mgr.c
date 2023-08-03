/**
*  @file avf_resource_mgr.c
*
* Copyright (c) [2020] Ambarella International LP
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
* This file includes sample code and is only for internal testing and evaluation.  If you
* distribute this sample code (whether in source, object, or binary code form), it will be
* without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
*  @detail xxxx
*
*/
#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include "avf_resource_mgr.h"

/* FIXME: Sample code from user guide */
int io_read (resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb);
int io_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);
int io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);

static char *buffer = "AVF-daemon\n";

static resmgr_connect_funcs_t   connect_funcs;
static resmgr_io_funcs_t        io_funcs;
static iofunc_attr_t            attr;

int global_integer = 0;

int avf_resource_mgr()
{
  /* declare variables we'll be using */
  resmgr_attr_t        resmgr_attr;
  dispatch_t           *dpp;
  dispatch_context_t   *ctp;
  int                  id;

  /* initialize dispatch interface */
  if((dpp = dispatch_create()) == NULL) {
    fprintf(stderr, "%s: Unable to allocate dispatch handle.\n",
            "avf");
    return EXIT_FAILURE;
  }

  /* initialize resource manager attributes */
  memset(&resmgr_attr, 0, sizeof resmgr_attr);
  resmgr_attr.nparts_max = 1;
  resmgr_attr.msg_max_size = 2048;

  /* initialize functions for handling messages, including
     our read handlers */
  iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                   _RESMGR_IO_NFUNCS, &io_funcs);
  io_funcs.read = io_read;
  io_funcs.read64 = io_read;
  io_funcs.write = io_write;
  io_funcs.write64 = io_write;
  /* For handling _IO_DEVCTL, sent by devctl() */
  io_funcs.devctl = io_devctl;

  /* initialize attribute structure used by the device */
  iofunc_attr_init(&attr, S_IFNAM | 0666, 0, 0);
  attr.nbytes = strlen(buffer)+1;

  /* attach our device name */
  if((id = resmgr_attach(dpp, &resmgr_attr, "/dev/avf", _FTYPE_ANY, 0,
               &connect_funcs, &io_funcs, &attr)) == -1) {
    fprintf(stderr, "%s: Unable to attach name.\n", "avf");
    return EXIT_FAILURE;
  }

  /* allocate a context structure */
  ctp = dispatch_context_alloc(dpp);

  /* start the resource manager message loop */
  while(1) {
    if((ctp = dispatch_block(ctp)) == NULL) {
      fprintf(stderr, "block error\n");
      return EXIT_FAILURE;
    }
    dispatch_handler(ctp);
  }
  return EXIT_SUCCESS;
}

int io_read (resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
  size_t      nleft;
  size_t      nbytes;
  int         nparts;
  int         status;

  if ((status = iofunc_read_verify (ctp, msg, ocb, NULL)) != EOK)
    return (status);

  if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
    return (ENOSYS);

  /*
   *  On all reads (first and subsequent), calculate
   *  how many bytes we can return to the client,
   *  based upon the number of bytes available (nleft)
   *  and the client's buffer size
   */

  nleft = ocb->attr->nbytes - ocb->offset;
  nbytes = min (_IO_READ_GET_NBYTES(msg), nleft);

  if (nbytes > 0) {
    /* set up the return data IOV */
    SETIOV (ctp->iov, buffer + ocb->offset, nbytes);

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

  if (msg->i.nbytes > 0)
    ocb->attr->flags |= IOFUNC_ATTR_ATIME;

  return (_RESMGR_NPARTS (nparts));
}

int io_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb)
{
  int     status;
  char    *buf;
  size_t  nbytes;

  if ((status = iofunc_write_verify(ctp, msg, ocb, NULL)) != EOK)
      return (status);

  if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
      return(ENOSYS);

  /* Extract the length of the client's message. */
  nbytes = _IO_WRITE_GET_NBYTES(msg);

  /* Filter out malicious write requests that attempt to write more
     data than they provide in the message. */
  if(nbytes > (size_t)ctp->info.srcmsglen - (size_t)ctp->offset - sizeof(io_write_t)) {
      return EBADMSG;
  }

  /* set up the number of bytes (returned by client's write()) */
  _IO_SET_WRITE_NBYTES (ctp, nbytes);

  buf = (char *) malloc(nbytes + 1);
  if (buf == NULL)
      return(ENOMEM);

  /*
   *  Reread the data from the sender's message buffer.
   *  We're not assuming that all of the data fit into the
   *  resource manager library's receive buffer.
   */

  resmgr_msgread(ctp, buf, nbytes, sizeof(msg->i));
  buf [nbytes] = '\0'; /* just in case the text is not NULL terminated */
  printf ("Received %zu bytes = '%s'\n", nbytes, buf);
  free(buf);

  if (nbytes > 0)
      ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

  return (_RESMGR_NPARTS (0));
}

int io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
  int     nbytes, status, previous;

  union {  /* See note 1 */
      data_t  data;
      int     data32;
      /* ... other devctl types you can receive */
  } *rx_data;

  /*
   Let common code handle DCMD_ALL_* cases.
   You can do this before or after you intercept devctls, depending
   on your intentions.  Here we aren't using any predefined values,
   so let the system ones be handled first. See note 2.
  */
  if ((status = iofunc_devctl_default(ctp, msg, ocb)) !=
       _RESMGR_DEFAULT) {
      return(status);
  }
  status = nbytes = 0;

  /*
   Note this assumes that you can fit the entire data portion of
   the devctl into one message.  In reality you should probably
   perform a MsgReadv() once you know the type of message you
   have received to get all of the data, rather than assume
   it all fits in the message.  We have set in our main routine
   that we'll accept a total message size of up to 2 KB, so we
   don't worry about it in this example where we deal with ints.
  */

  /* Get the data from the message. See Note 3. */
  rx_data = _DEVCTL_DATA(msg->i);

  /*
   Three examples of devctl operations:
   SET: Set a value (int) in the server
   GET: Get a value (int) from the server
   SETGET: Set a new value and return the previous value
  */
  switch (msg->i.dcmd) {
  case MY_DEVCTL_SETVAL:
      global_integer = rx_data->data32;
      nbytes = 0;
      break;

  case MY_DEVCTL_GETVAL:
      rx_data->data32 = global_integer; /* See note 4 */
      nbytes = sizeof(rx_data->data32);
      break;

  case MY_DEVCTL_SETGET:
      previous = global_integer;
      global_integer = rx_data->data.tx;

      /* See note 4. The rx data overwrites the tx data
         for this command. */

      rx_data->data.rx = previous;
      nbytes = sizeof(rx_data->data.rx);
      break;

  default:
      return(ENOSYS);
  }

  /* Clear the return message. Note that we saved our data past
     this location in the message. */
  memset(&msg->o, 0, sizeof(msg->o));

  /*
   If you wanted to pass something different to the return
   field of the devctl() you could do it through this member.
   See note 5.
  */
  msg->o.ret_val = status;

  /* Indicate the number of bytes and return the message */
  msg->o.nbytes = nbytes;
  return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + nbytes));
}

