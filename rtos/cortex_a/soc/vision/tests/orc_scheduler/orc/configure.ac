#
# Copyright (c) 2018-2018 Ambarella, Inc.
#
# This file and its contents ("Software") are protected by intellectual property rights including,
# without limitation, U.S. and/or foreign copyrights.  This Software is also the confidential and
# proprietary information of Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
# distribute, modify, or otherwise prepare derivative works of this Software or any portion thereof
# except pursuant to a signed license agreement or nondisclosure agreement with Ambarella, Inc. or
# its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
# return this Software to Ambarella, Inc.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

AC_INIT()

test -z "$amalgam_diag" && amalgam_diag=0
AC_ARG_VAR([amalgam_diag], [Compile for amalgam diags (default = 0), diag is designed for hardware])

test -z "$ucode_dir" && ucode_dir=$srcdir/../../../orc_framework/
AC_ARG_VAR([ucode_dir], [Ucode directory])

test -z "$cv_common_dir" && cv_common_dir=$srcdir/../../../cv_common/
AC_ARG_VAR([cv_common_dir], [CV project common directory])

AC_ARG_VAR([use_ace_platform], sets ACE mode)
AC_ARG_VAR([pairing_key], sets pairing key for ACE)
AC_ARG_VAR([pairing_server], sets pairing server for ACE)
AC_ARG_VAR([dram_size], Total dram size)
AC_ARG_VAR([emulator], [emulator / arch mode])

### Library Directories ####################################################################################################

test -z "$install_root" && install_root="./libs/"
AC_ARG_VAR([install_root], [Location to place libraries compiled from this diag])

test -z "$lib_root" && lib_root=$srcdir/../../../cv_common/libs
AC_ARG_VAR([lib_root], [Location to read compiled libraries from])

### Files to generate from *.in files ######################################################################################

AC_CONFIG_FILES([
	Makefile
    amalgam/init.amalgam.ppp
])

AC_CONFIG_COMMANDS([configBuild],
[
  mkdir -p build
  cd build
  remoteconfig $ucode_dir                             \
    -opt amalgam_diag=$amalgam_diag                   \
    -opt use_ace_platform=$use_ace_platform           \
    -opt lib_root=$lib_root                           \
    -opt install_root=$install_root
  cd ..
],
[
  amalgam_diag=$amalgam_diag
  ucode_dir=$ucode_dir
  use_ace_platform=$use_ace_platform
  lib_root=$lib_root
  install_root=$install_root
])

AC_OUTPUT()
