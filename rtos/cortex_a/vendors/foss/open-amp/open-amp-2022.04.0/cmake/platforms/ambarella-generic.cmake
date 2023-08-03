# Modify to match your needs.  These setttings can also be overridden at the
# command line.  (eg. cmake -DCMAKE_C_FLAGS="-O3")

set (CMAKE_SYSTEM_PROCESSOR "arm"            CACHE STRING "")
set (MACHINE                "Ambarella"      CACHE STRING "")
if (NOT CROSS_PREFIX)
  set (CROSS_PREFIX         "arm-none-eabi-" CACHE STRING "")
endif()

set (CMAKE_C_FLAGS          ""               CACHE STRING "")

#include (cross_generic_gcc)
set (CMAKE_SYSTEM_NAME      "Generic" CACHE STRING "")

include (CMakeForceCompiler)

set (CMAKE_C_COMPILER   ${CROSS_PREFIX}gcc)
set (CMAKE_CXX_COMPILER ${CROSS_PREFIX}g++)

set (CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM  NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM	ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY	ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE	ONLY)

# vim: expandtab:ts=2:sw=2:smartindent
