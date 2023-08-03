#!/bin/bash

#
# Note: Need to source .config in caller
#

set -e
#set -x

LDFLAGS_amba_common='-p --no-undefined'
# Garbage collection of unused section
LDFLAGS_amba_common="${LDFLAGS_amba_common} --gc-sections"

# warning: ... uses variable-size enums yet the output is to use 32-bit enums; use of enum values across objects may fail
# Use -fno-short-enums causes linking warnings.
#LDFLAGS_amba_common+=' --no-enum-size-warning'

# warning: ... uses 4-byte wchar_t yet the output is to use 2-byte wchar_t; use of wchar_t values across objects may fail
# Use -fshort-wchar for CC, so we can have L"text" style declaration. However libc use 4(linux)/2(cygwin) for wchar_t, there would be a warning while
# linking. So we supress it.
#LDFLAGS_amba_common+=' --no-wchar-size-warning'

# Not good for multi-definitions, the same as "--allow-multiple-definition"
#LDFLAGS_amba_common+=' -z muldefs'
# Options
#LDFLAGS_amba_common+=' --strip-all'

#LDFLAGS_amba_common+=' -nostartfiles'

#if [ "${CONFIG_CC_NOSTD}" != "" ]; then
#    LDFLAGS_amba_common+=' -nostdlib -nodefaultlibs'
#else
#    # Path of libc.a libstdc++.a
#    PATH_LIBC=$(dirname $(readlink -f $(${CC} ${KBUILD_CFLAGS} -print-file-name=libc.a)))
#    #Path of libgcc.a
#    PATH_LIBGCC=$(dirname $(readlink -f $(${CC} ${KBUILD_CFLAGS} -print-file-name=libgcc.a)))
#
#    LDFLAGS_amba_common+=" -L${PATH_LIBC} -L${PATH_LIBGCC}"
#fi

export LDFLAGS_amba_common

