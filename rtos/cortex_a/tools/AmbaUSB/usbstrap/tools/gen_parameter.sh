#!/bin/bash

usage() {
	cat << EOF
gen_parameter.sh [infile] [outfile]
	infile		'csf' jtag script like 'xxx.csf'
	outfile		C header file like 'parameter.h'
EOF
	exit
}

if [ $# -ne 2 ];then
	usage
fi

if [ ! -e $1 ];then
	echo "error: $1 not exist"
	exit
fi

infile=$1
outfile=$2

cat > $outfile << EOF
// `basename $infile`
static const struct dram_param_t dram_param[] = {
EOF
cat $1 \
	|sed 's/#/\/\//' \
	|sed '/^[A-Z]/ s/^/{ /'\
	|sed '/^{/ s/$/ },/'\
	|sed 's/MWriteS32 0x100000/CMD_DDRC_WRITE 0x/g' \
	|sed 's/MWriteS32 0x20ed08/CMD_RCT_WRITE 0x/g' \
	|sed 's/MWriteS32 0x20ed18/CMD_DDRH0_WRITE 0x/g' \
	|sed 's/MWriteS32 0x20ed19/CMD_DDRH1_WRITE 0x/g' \
	|sed 's/ 0x/, 0x/g'\
	|sed 's/0x100000/0x0000/g' \
	|sed 's/^(//g' \
	|sed 's/^)//g' \
	|sed 's/^.*while.*$/{ CMD_USLEEP, 0, 1000},/g' \
	|sed 's/Wait 1\.ms/CMD_USLEEP, 0, 1000/g' \
	>>$outfile

cat >>$outfile <<EOF
};
EOF
