/*
 * Copyright (c) 2017-2017 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CVAPI_FILE_FEEDER_INTERFACE_H_FILE
#define CVAPI_FILE_FEEDER_INTERFACE_H_FILE

#define TASKNAME_FILE_FEEDER "ARM_FEEDER_TASK"

/*******************************************************************************
 *                               INPUT BUFFER
 * No input
 *
 *******************************************************************************
 *                              OUTPUT BUFFER
 * One output buffer
 *
 *******************************************************************************
 *                              CVTABLE LABELS
 * FEEDER_IONAME_xxx: required
 * FEEDER_SOURCE_xxx: required
 *
 *******************************************************************************
 *                       SYSFLOW CONFIGURATION SYNTAX
 * 0: filefeeder ID. Superdag must provide the following labels where xxx is
 *    the numerical value of the ID.
 *      FEEDER_IONAME_xxx: name of filefeeder's output buffer
 *      FEEDER_SOURCE_xxx: name of the file(s) that provide the data
 * 1: frame width
 * 2: frame height
 * 3: frame pitch
 * 4: total number of frames to be generated
 * 5: frame-number offset
 * 6: number of frames provided by each input file
 * 7: byte offset from the beginning of each source file
 *
 * If FEEDER_SOURCE_xxx is defined as "FOO.bin",
 * then data of all frames are read from "FOO.bin".
 * Config[5] and Config[6] are ignored.
 *
 * If FEEDER_SOURCE_xxx is defined as "FOO_%03d.bin",
 * then data of frame N is read from file "FOO_yyy.bin" where
 *      yyy = (N + config[5]) / config[6]
 *
 ******************************************************************************/

#endif /* !CVAPI_FILE_FEEDER_INTERFACE_H_FILE */

