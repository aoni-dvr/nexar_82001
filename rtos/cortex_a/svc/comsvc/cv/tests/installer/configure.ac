# Copyright (c) 2017-2018 Ambarella, Inc.
#
# This file and its contents ("Software") are protected by intellectual
# property rights including, without limitation, U.S. and/or foreign copyrights.
# This Software is also the confidential and proprietary information of
# Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
# distribute, modify, or otherwise prepare derivative works of this Software
# or any portion thereof except pursuant to a signed license agreement or
# nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
# In the absence of such an agreement, you agree to promptly notify and
# return this Software to Ambarella, Inc.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
# MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
# EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

AC_INIT()

test -z "$ucode_dir" && ucode_dir=$srcdir/../../orc_framework/
AC_ARG_VAR([ucode_dir], [Ucode directory])

test -z "$cv_common_dir" && cv_common_dir=$srcdir/../../cv_common/
AC_ARG_VAR([cv_common_dir], [CV project common directory])

### Library Directories ####################################################################################################
test -z "$install_group" && install_group="cvsuser"
AC_ARG_VAR([install_group], [(UNIX group to use for installation)])

test -z "$install_root" && install_root="../../../../../../../$CV_CORE_DIR/cv_common/libs"
AC_ARG_VAR([install_root], [Location to place libraries compiled from this diag])

test -z "$lib_root" && lib_root="$srcdir/../../cv_common/libs"
AC_ARG_VAR([lib_root], [Location to read compiled libraries from])

### CV/ORC configuration ###################################################################################################

### Use this to run from source (SL0)

if [ test $PROJECT = cv2a ]; then
test -z "$diag_vp_cvtasks" && diag_vp_cvtasks=""
test -z "$vp_cvtask_libs" && vp_cvtask_libs=""
test -z "$diag_vp_any_cvtasks" && diag_vp_any_cvtasks=""
test -z "$default_sod_cvlib_libs" && default_sod_cvlib_libs=" "
test -z "$default_sod_cvlib_srcs" && default_sod_cvlib_srcs=" "
test -z "$default_vp_cvlib_libs" && default_vp_cvlib_libs="vissec"
test -z "$default_vp_cvlib_srcs" && default_vp_cvlib_srcs=""
fi

if [ test $PROJECT = cv2fs ]; then
test -z "$diag_vp_cvtasks" && diag_vp_cvtasks=""
test -z "$vp_cvtask_libs" && vp_cvtask_libs=""
test -z "$diag_vp_any_cvtasks" && diag_vp_any_cvtasks=""
test -z "$default_sod_cvlib_libs" && default_sod_cvlib_libs=" "
test -z "$default_sod_cvlib_srcs" && default_sod_cvlib_srcs=" "
test -z "$default_vp_cvlib_libs" && default_vp_cvlib_libs="vissec"
test -z "$default_vp_cvlib_srcs" && default_vp_cvlib_srcs=""
fi

if [ test $PROJECT = cv22a ]; then
test -z "$diag_vp_cvtasks" && diag_vp_cvtasks=""
test -z "$vp_cvtask_libs" && vp_cvtask_libs=""
test -z "$diag_vp_any_cvtasks" && diag_vp_any_cvtasks=""
test -z "$default_sod_cvlib_libs" && default_sod_cvlib_libs=" "
test -z "$default_sod_cvlib_srcs" && default_sod_cvlib_srcs=" "
test -z "$default_vp_cvlib_libs" && default_vp_cvlib_libs="vissec"
test -z "$default_vp_cvlib_srcs" && default_vp_cvlib_srcs=""
fi

if [ test $PROJECT = cv22fs ]; then
test -z "$diag_vp_cvtasks" && diag_vp_cvtasks=""
test -z "$vp_cvtask_libs" && vp_cvtask_libs=""
test -z "$diag_vp_any_cvtasks" && diag_vp_any_cvtasks=""
test -z "$default_sod_cvlib_libs" && default_sod_cvlib_libs=" "
test -z "$default_sod_cvlib_srcs" && default_sod_cvlib_srcs=" "
test -z "$default_vp_cvlib_libs" && default_vp_cvlib_libs="vissec"
test -z "$default_vp_cvlib_srcs" && default_vp_cvlib_srcs=""
fi

if [ test $PROJECT = cv2 ]; then
test -z "$diag_vp_cvtasks" && diag_vp_cvtasks=""
test -z "$vp_cvtask_libs" && vp_cvtask_libs=""
test -z "$diag_vp_any_cvtasks" && diag_vp_any_cvtasks=""
test -z "$default_sod_cvlib_libs" && default_sod_cvlib_libs=" "
test -z "$default_sod_cvlib_srcs" && default_sod_cvlib_srcs=" "
test -z "$default_vp_cvlib_libs" && default_vp_cvlib_libs="vissec"
test -z "$default_vp_cvlib_srcs" && default_vp_cvlib_srcs=""
fi

if [ test $PROJECT = cv22 ]; then
test -z "$diag_vp_cvtasks" && diag_vp_cvtasks=""
test -z "$vp_cvtask_libs" && vp_cvtask_libs=""
test -z "$diag_vp_any_cvtasks" && diag_vp_any_cvtasks=""
test -z "$default_sod_cvlib_libs" && default_sod_cvlib_libs=" "
test -z "$default_sod_cvlib_srcs" && default_sod_cvlib_srcs=" "
test -z "$default_vp_cvlib_libs" && default_vp_cvlib_libs="vissec"
test -z "$default_vp_cvlib_srcs" && default_vp_cvlib_srcs=""
fi

if [ test $PROJECT = cv25 ]; then
test -z "$diag_vp_cvtasks" && diag_vp_cvtasks=""
test -z "$vp_cvtask_libs" && vp_cvtask_libs=""
test -z "$diag_vp_any_cvtasks" && diag_vp_any_cvtasks=""
test -z "$default_sod_cvlib_libs" && default_sod_cvlib_libs=" "
test -z "$default_sod_cvlib_srcs" && default_sod_cvlib_srcs=" "
test -z "$default_vp_cvlib_libs" && default_vp_cvlib_libs="vissec"
test -z "$default_vp_cvlib_srcs" && default_vp_cvlib_srcs=""
fi

if [ test $PROJECT = cv28 ]; then
test -z "$diag_vp_cvtasks" && diag_vp_cvtasks=""
test -z "$vp_cvtask_libs" && vp_cvtask_libs=""
test -z "$diag_vp_any_cvtasks" && diag_vp_any_cvtasks=""
test -z "$default_sod_cvlib_libs" && default_sod_cvlib_libs=" "
test -z "$default_sod_cvlib_srcs" && default_sod_cvlib_srcs=" "
test -z "$default_vp_cvlib_libs" && default_vp_cvlib_libs="vissec"
test -z "$default_vp_cvlib_srcs" && default_vp_cvlib_srcs=""
fi

if [ test $PROJECT = cv5 ]; then
test -z "$diag_vp_cvtasks" && diag_vp_cvtasks=""
test -z "$vp_cvtask_libs" && vp_cvtask_libs=""
test -z "$diag_vp_any_cvtasks" && diag_vp_any_cvtasks=""
test -z "$default_sod_cvlib_libs" && default_sod_cvlib_libs=" "
test -z "$default_sod_cvlib_srcs" && default_sod_cvlib_srcs=" "
test -z "$default_vp_cvlib_libs" && default_vp_cvlib_libs="vissec"
test -z "$default_vp_cvlib_srcs" && default_vp_cvlib_srcs=""
fi

if [ test $PROJECT = cv52 ]; then
test -z "$diag_vp_cvtasks" && diag_vp_cvtasks=""
test -z "$vp_cvtask_libs" && vp_cvtask_libs=""
test -z "$diag_vp_any_cvtasks" && diag_vp_any_cvtasks=""
test -z "$default_sod_cvlib_libs" && default_sod_cvlib_libs=" "
test -z "$default_sod_cvlib_srcs" && default_sod_cvlib_srcs=" "
test -z "$default_vp_cvlib_libs" && default_vp_cvlib_libs="vissec"
test -z "$default_vp_cvlib_srcs" && default_vp_cvlib_srcs=""
fi

AC_ARG_VAR([diag_sod_cvtasks], SOD/CVTasks to compile into this diag)
AC_ARG_VAR([diag_vp_cvtasks], VP/CVTasks to compile into this diag)
AC_ARG_VAR([diag_vp_any_cvtasks], ORCANY/CVTasks to link from OrcAny)
AC_ARG_VAR([diag_sod_any_cvtasks], ORCANY/CVTasks to link from OrcAny)
AC_ARG_VAR([sod_cvtask_libs], [Compiled SOD cvtask libraries to link])
AC_ARG_VAR([vp_cvtask_libs], [Compiled VP cvtask libraries to link])
AC_ARG_VAR([default_sod_cvlib_libs], [SOD/CVLIB libraries to link in by default])
AC_ARG_VAR([default_sod_cvlib_srcs], [SOD/CVLIB libraries to compile from source])
AC_ARG_VAR([default_vp_cvlib_libs], [VP/CVLIB libraries to link in by default])
AC_ARG_VAR([default_vp_cvlib_srcs], [VP/CVLIB libraries to compile from source])

### Files to generate from *.in files ######################################################################################
AC_CONFIG_FILES([
	Makefile
	])

AC_CONFIG_COMMANDS([configBuild],
[ 
  mkdir -p build_hw
  cd build_hw
  remoteconfig $ucode_dir                               \
    -opt amalgam_diag=0                                 \
    -opt use_ace_platform=1                             \
    -opt chide_diag=0                                   \
    -opt diag_sod_cvtasks="$diag_sod_cvtasks"           \
    -opt diag_vp_cvtasks="$diag_vp_cvtasks"             \
    -opt diag_vp_any_cvtasks="$diag_vp_any_cvtasks"     \
    -opt diag_sod_any_cvtasks="$diag_sod_any_cvtasks"   \
    -opt sod_default_cvtask_libs=" "                    \
    -opt lib_root=$lib_root                             \
	-opt misra_compliance_mode=1                       \
    -opt install_group=$install_group                   \
    -opt install_root=$install_root                     \
    -opt vp_cvtask_libs="$vp_cvtask_libs"               \
    -opt sod_cvtask_libs="$sod_cvtask_libs"             \
    -opt default_sod_cvlib_libs="$default_sod_cvlib_libs" \
    -opt default_sod_cvlib_srcs="$default_sod_cvlib_srcs" \
    -opt default_vp_cvlib_libs="$default_vp_cvlib_libs"   \
    -opt default_vp_cvlib_srcs="$default_vp_cvlib_srcs"   \
    -opt build_scheduler=1
  make
  make install
  cd ..
],
[
  ucode_dir=$ucode_dir
  diag_sod_cvtasks="$diag_sod_cvtasks"
  diag_vp_cvtasks="$diag_vp_cvtasks"
  diag_vp_any_cvtasks="$diag_vp_any_cvtasks"
  diag_sod_any_cvtasks="$diag_sod_any_cvtasks"
  lib_root=$lib_root
  misra_compliance_mode=$misra_compliance_mode
  install_group=$install_group
  install_root=$install_root
  vp_cvtask_libs="$vp_cvtask_libs"
  sod_cvtask_libs="$sod_cvtask_libs"
  default_sod_cvlib_libs="$default_sod_cvlib_libs"
  default_sod_cvlib_srcs="$default_sod_cvlib_srcs"
  default_vp_cvlib_libs="$default_vp_cvlib_libs"
  default_vp_cvlib_srcs="$default_vp_cvlib_srcs"
])

AC_OUTPUT

