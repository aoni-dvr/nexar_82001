#!/bin/bash

# Debug
#set -x
#echo $@

options="${KBUILD_CFLAGS} -nostartfiles -nostdlib "
# Enables the use of a linker plugin during link-time optimization. ld > 2.21
# This option enables the extraction of object files with GIMPLE bytecode out of library archives.
options+="-fuse-linker-plugin "

# to use GNU make's job server mode to determine the number of parallel jobs.
options+="-flto=jobserver "

# Debug
#options+="-flto-report "
#options+="-fdump-ipa-cgraph -fdump-ipa-inline-details "
#options+="-fmem-report "
#options+="-fstack-usage "

for arg in "$@"; do
	case "${arg}" in
	-p|--no-undefined|--gc-sections)
		o="-Wl,${arg}"
		;;
	--script*)
		o="-Wl,${arg}"
		;;
	--start-group|--end-group)
		o="-Wl,${arg}"
		;;
	--whole-archive|--no-whole-archive)
		o="-Wl,${arg}"
		;;
	-Map=*|--cref)
		o="-Wl,${arg}"
		;;
	--wrap=*)
		o="-Wl,${arg}"
		;;
	*)
		o=${arg}
		;;
	esac
	options+="${o} "
done

# Link-time optimization does not work well with generation of debugging information.
# Combining -flto with -g is currently experimental and expected to produce unexpected results.
#options=`echo ${options} | grep -v "\-g\|-ggdb"`

BT4=1

echo "Wating for LTO....."
if [ "${BT4}" = "0" ]; then
	time ${CC} ${options}
else
	time ${CC} ${options}&
	while [ 1 ]; do
		j=`jobs -rp`
		if [ "x$j" == "x" ]; then
			break
		else
			echo -n .""
			sleep 1
		fi
	done
fi

echo ""


