AC_INIT()

test -z "$amalgam_diag" && amalgam_diag=1
AC_ARG_VAR([amalgam_diag], [Compile for amalgam diags (default = 1)])

test -z "$ucode_dir" && ucode_dir=$srcdir/../../../orc_framework/
AC_ARG_VAR([ucode_dir], [Ucode directory])

test -z "$test_srcdir" && test_srcdir=$srcdir
AC_ARG_VAR([test_srcdir], [Explicit test directory for compiling diag-only cvtasks])

test -z "$cv_common_dir" && cv_common_dir=$srcdir/../../../cv_common/
AC_ARG_VAR([cv_common_dir], [CV project common directory])

test -z "$emulator" && emulator=1
AC_ARG_VAR([emulator], sets emulator mode)

if [ test -z "$use_ace_platform" ]; then
  use_ace_platform=0;
else
  if [ test -z "$pairing_key" ]; then
    echo " "
    echo "Please specify pairing key using \"-opt pairing_key=0xXXXXX\" format !"
    echo ""
    exit
  fi
  if [ test -z "$pairing_server" ]; then
    echo " "
    echo "Please specify pairing server using \"-opt pairing_server=xx.xx.xx.xx\" format !"
    echo ""
    exit
  fi
fi
AC_ARG_VAR([use_ace_platform], sets ACE mode)
AC_ARG_VAR([pairing_key], sets pairing key for ACE)
AC_ARG_VAR([pairing_server], sets pairing server for ACE)
AC_ARG_VAR([dram_size], Total dram size)

test -z "$diag_sod_cvtasks" && diag_sod_cvtasks=""
AC_ARG_VAR([diag_sod_cvtasks], SOD/CVTasks to compile into this diag)

test -z "$diag_vp_cvtasks" && diag_vp_cvtasks=""
AC_ARG_VAR([diag_vp_cvtasks], VP/CVTasks to compile into this diag)

test -z "$diag_vp_any_cvtasks" && diag_vp_any_cvtasks=""
AC_ARG_VAR([diag_vp_any_cvtasks], ORCANY/CVTasks to link from OrcAny)

test -z "$diag_sod_any_cvtasks" && diag_sod_any_cvtasks=""
AC_ARG_VAR([diag_sod_any_cvtasks], ORCANY/CVTasks to link from OrcAny)

test -z "$vp_cvtask_libs" && vp_cvtask_libs=""
AC_ARG_VAR([vp_cvtask_libs], [Compiled VP cvtask libraries to link])

test -z "$sod_cvtask_libs" && sod_cvtask_libs=""
AC_ARG_VAR([sod_cvtask_libs], [Compiled SOD cvtask libraries to link])

test -z "$build_scheduler" && build_scheduler=0
AC_ARG_VAR([build_scheduler], [Build scheduler from source])

### Library Directories ####################################################################################################

test -z "$install_root" && install_root="./libs/"
AC_ARG_VAR([install_root], [Location to place libraries compiled from this diag])

test -z "$lib_root" && lib_root="$srctree/$CV_CORE_DIR/cv_common/libs"
AC_ARG_VAR([lib_root], [Location to read compiled libraries from])

### Files to generate from *.in files ######################################################################################

AC_CONFIG_FILES([
	Makefile
	])

AC_CONFIG_COMMANDS([configBuild],
[
  mkdir -p build
  cd build
  remoteconfig $ucode_dir                             \
    -opt flexidag_mode=1                              \
    -opt test_srcdir="$test_srcdir"                   \
    -opt diag_sod_cvtasks="$diag_sod_cvtasks"         \
    -opt diag_vp_cvtasks="$diag_vp_cvtasks"           \
    -opt diag_vp_any_cvtasks="$diag_vp_any_cvtasks"   \
    -opt diag_sod_any_cvtasks="$diag_sod_any_cvtasks" \
    -opt lib_root=$lib_root                           \
    -opt install_root=$install_root                   \
    -opt vp_cvtask_libs="$vp_cvtask_libs"             \
    -opt sod_cvtask_libs="$sod_cvtask_libs"
  cd ..
],
[
  ucode_dir=$ucode_dir
  test_srcdir=$test_srcdir
  diag_sod_cvtasks="$diag_sod_cvtasks"
  diag_vp_cvtasks="$diag_vp_cvtasks"
  diag_vp_any_cvtasks="$diag_vp_any_cvtasks"
  diag_sod_any_cvtasks="$diag_sod_any_cvtasks"
  lib_root=$lib_root
  install_root=$install_root
  vp_cvtask_libs="$vp_cvtask_libs"
  sod_cvtask_libs="$sod_cvtask_libs"
]
)

AC_OUTPUT

