#!/bin/bash
#
#  $0 fn_input fn_output
#

set -e
# Debug
#set -x

#
# CONFIG_FWPROG_XXX_LOADADDR or some configurations that are used,
# but are not always defined must not be used.
#
cfg='
AMBALINK_BOOT_OS_LEGACY_5_7
ATF_HAVE_BL2
ATF_FIP_RTOS
BLD_SEQ_SRTOS
BLD_SEQ_ATF_SRTOS
BLD_SEQ_LINUX
BLD_SEQ_ATF_LINUX
BLD_SEQ_ATF_XEN
BLD_SEQ_SHELL
BLD_SEQ_ATF
DEVICE_TREE_SUPPORT
FWPROG_LNX_LOADADDR
THREADX64
XEN_SUPPORT
XEN_DOM0_TX
XEN_DOM0_LX
XEN_HAVE_DOMU
XEN_DOMU_TX
XEN_DOMU_LX
'
if [ x"$1" != x"" ]; then
    fn_in=$1
else
    echo "No input file"
fi

if [ x"$2" != x"" ]; then
    fn_out=$2
else
    echo "No output file"
fi
cp ${fn_in} ${fn_out}

echo "/* ##### Partially automatic generated. ##### */" >> ${fn_out}
echo "" >> ${fn_out}
for c in ${cfg}; do
    echo "#ifdef CONFIG_${c}" >> ${fn_out}
    echo "#define BLDCFG_${c} (YES)" >> ${fn_out}
    echo "#else" >> ${fn_out}
    echo "#define BLDCFG_${c} (NO)" >> ${fn_out}
    echo "#endif" >> ${fn_out}
    echo "" >> ${fn_out}
done
echo "#endif /* AMBABLD_CFG_H */" >> ${fn_out}

