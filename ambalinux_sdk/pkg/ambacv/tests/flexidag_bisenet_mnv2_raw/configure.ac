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

test -z "$ucode_dir" && ucode_dir=$srcdir/../../orc_framework/
AC_ARG_VAR([ucode_dir], [Ucode directory])

test -z "$amalgam_diag" && amalgam_diag=0
AC_ARG_VAR([amalgam_diag], [Compile for amalgam diags (default = 0), diag is designed for hardware])

test -z "$dram_size" && dram_size=0x20000000
AC_ARG_VAR([dram_size], [Total dram size])

test -z "$emulator" && emulator=0
AC_ARG_VAR([emulator], [emulator / arch mode])

test -z "$pairing_key" && pairing_key=0x`tr -cd 0-9a-f < /dev/urandom | head -c 16`
AC_ARG_VAR([pairing_key], $pairing_key)

test -z "$pairing_server" && pairing_server=127.0.0.1
AC_ARG_VAR([pairing_server], [Key pairing server])

### Common Directories #####################################################################################################

test -z "$cv_common_dir" && cv_common_dir=$srcdir/../../cv_common/
AC_ARG_VAR([cv_common_dir], [CV project common directory])

test -z "$use_ace_platform" && use_ace_platform=1
AC_ARG_VAR([use_ace_platform], [sets ACE mode, default 1 for this diag])

### Library Directories ####################################################################################################

test -z "$install_root" && install_root="./libs/"
AC_ARG_VAR([install_root], [Location to place libraries compiled from this diag])

test -z "$lib_root" && lib_root=$srcdir/../../cv_common/libs
AC_ARG_VAR([lib_root], [Location to read compiled libraries from])

### ARM CVTask List ########################################################################################################

test -z "$armA_cvtask_build" && armA_cvtask_build=""
AC_ARG_VAR([armA_cvtask_build], [ARM/CVTasks to compile into this test for ARM cluster-A])

test -z "$armA_cvtask_lib" && armA_cvtask_lib=""
AC_ARG_VAR([armA_cvtask_lib], [ARM/CVTasks to link into this test for ARM cluster-A])

### CV/ORC configuration ###################################################################################################

test -z "$build_scheduler" && build_scheduler=0
AC_ARG_VAR([build_scheduler], [Build scheduler from source])

test -z "$diag_sod_cvtasks" && diag_sod_cvtasks=""
AC_ARG_VAR([diag_sod_cvtasks], SOD/CVTasks to compile into this diag)

test -z "$diag_vp_cvtasks" && diag_vp_cvtasks=""
AC_ARG_VAR([diag_vp_cvtasks], VP/CVTasks to compile into this diag)

test -z "$diag_sod_any_cvtasks" && diag_sod_any_cvtasks=""
AC_ARG_VAR([diag_sod_any_cvtasks], ORCANY/CVTasks to link from OrcAny)

test -z "$diag_vp_any_cvtasks" && diag_vp_any_cvtasks=""
AC_ARG_VAR([diag_vp_any_cvtasks], ORCANY/CVTasks to link from OrcAny)

test -z "$sod_cvtask_libs" && sod_cvtask_libs=""
AC_ARG_VAR([sod_cvtask_libs], [Compiled SOD cvtask libraries to link])

test -z "$vp_cvtask_libs" && vp_cvtask_libs=""
AC_ARG_VAR([vp_cvtask_libs], [Compiled VP cvtask libraries to link])

### Files to generate from *.in files ######################################################################################

AC_CONFIG_FILES([
	Makefile
	arm/cvtask_list.inc
	arm/flexidag_openseg_ag_superdag.mnft
])

AC_CONFIG_COMMANDS([ConfigFlexidag0],
[
  mkdir -p flexidag0;
  cd flexidag0;
  remoteconfig $srcdir/flexidag0 -opt test_srcdir=$srcdir/flexidag0;
  cd ..;
]
)


AC_OUTPUT()
