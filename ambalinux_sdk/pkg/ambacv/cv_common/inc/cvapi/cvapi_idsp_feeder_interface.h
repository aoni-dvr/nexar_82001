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

#ifndef CVAPI_IDSP_FEEDER_INTERFACE_H_FILE
#define CVAPI_IDSP_FEEDER_INTERFACE_H_FILE

#define TASKNAME_IDSP_FEEDER    "IDSP_FEEDER"

/*******************************************************************************
 *                               INPUT BUFFER
 * No input buffer
 *
 *******************************************************************************
 *                              OUTPUT BUFFER
 * No output buffer
 *
 *******************************************************************************
 *                              CVTABLE LABELS
 * X: stands for fedder instance ID given by config[0]
 *
 * IdspFeederX_input: required
 *      source file name for feeding idsp data
 * IdspFeederX_frame_start: optional
 *      starting frame number used to feed idsp data, default 0
 * IdspFeederX_frame_end: required
 *      ending frame number used to feed idsp data
 * IMU_SOURCE_000:
 *      source file name for feeding gyro data
 *
 *******************************************************************************
 *                       SYSFLOW CONFIGURATION SYNTAX
 * 0: instance ID for the feeder
 * 1: frame-interval in milli-second for timer-mode
	If feeder can't complete the data fetching within this interval,
	the previous frame is repeated.
	"0" means on-demand mode: feeder sends the next frame as soon as it
	completes the fetching.
 * 7: enable feeding of gyro data
 *
 ******************************************************************************/

#endif /* !CVAPI_IDSP_FEEDER_INTERFACE_H_FILE */

