/*
 * Copyright (c) 2017-2018 Ambarella, Inc.
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

AC_INIT()

test -z "$common_dir" && common_dir=$srcdir/common
AC_ARG_VAR([common_dir], [Common directory])

test -z "$amalgam_diag" && amalgam_diag=0
AC_ARG_VAR([amalgam_diag], [Compile for amalgam diags (default = 0)])

test -z "$use_ace_platform" && use_ace_platform=0
AC_ARG_VAR([use_ace_platform], [Compile for use with the ACE platform (default = 0)])

test -z "$chide_diag" && chide_diag=0
AC_ARG_VAR([chide_diag], [Compile for CHIDE diags (default = 0)])

AC_CONFIG_FILES([
        include.mk
        core_vp/include.mk
	])

AC_OUTPUT

