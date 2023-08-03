/*
* Copyright (c) 2017-2017 Ambarella, Inc.
*
* This file and its contents ("Software") are protected by intellectual property rights including,
* without limitation, U.S. and/or foreign copyrights.  This Software is also the confidential and
* proprietary information of Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
* distribute, modify, or otherwise prepare derivative works of this Software or any portion thereof
* except pursuant to a signed license agreement or nondisclosure agreement with Ambarella, Inc. or
* its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
* return this Software to Ambarella, Inc.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CVAPI_THROTTLER_INTERFACE_H_FILE
#define CVAPI_THROTTLER_INTERFACE_H_FILE

/* This serves as a living document for both ORC_THROTTLER and VP_THROTTLER
 *
 * Each throttler is defined to have one input and one output.  The throttler is
 * designed to connect to another CVTask's output, and make a copy of the data from
 * the input to the output at a particular time.
 *
 * config[0] of the system flow table entry for the (ORC/VP)_THROTTLER will determine
 * which entry from the manifest it will look for.  This will set the io_name of the
 * input and output to THROTTLER_IONAME_###, where ### is replaced by %03d string
 * based on config[0].
 *
 * If the throttler is set to run on-demand (init_freq=255), then the throttler will
 * only run if a message is sent to it.  The content and size of the message does
 * not matter, just the event of reception of a message is what triggers the run.
 */

#define TASKNAME_ORC_THROTTLER    "ORC_THROTTLER"
#define TASKNAME_VP_THROTTLER     "VP_THROTTLER"

#endif /* !CVAPI_IDSP_INTERFACE_H_FILE */

