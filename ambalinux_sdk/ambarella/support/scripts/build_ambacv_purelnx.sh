#!/bin/sh

###############################
# 1. Source code Setup:
###############################
# For ucode
# cd /test/arm
# repo init -u ssh://ambus-git.ambarella.com:29418/ambarella/manifest -b cv -m h2evk_linux_SL6.xml
# repo sync

# For ARM
# mkdir -p ambalink_4_9 && cd ambalink_4_9
# repo init -u ssh://xxxxxx@amcode.ambarella.com:29418/boss_sdk/manifest -b ambalink_sdk -m ambalink_sdk_4_9_cv1_bringup.xml && repo sync

###############################
# 2. Compile ARM Linux code
###############################
if [ ! -e ambarella ]; then
	echo "Please cd to ambalink_sdk root directory !"
	return -1;
fi

# Just for workaround
#ln -sf /cv1/sequence/test_sequence/release_lib/release_1.02/arm/linux /cv1/sequence/test_sequence/release_lib/stable/arm/linux
ln -sf /dump10/cychen/dag_lib /cv1/sequence/test_sequence/release_lib/stable/arm/linux

make -C ambarella O=../output/h2_purelnx h2_purelnx_cv1_defconfig
make -C  output/h2_purelnx

if [ $? != 0 ]; then
	echo "build error: $?"
	return -1;
fi

export CROSS_COMPILE=aarch64-linux-gnu-
export ARM_LINUX_TOOLCHAIN_DIR=$PWD/output/h2_purelnx/host/opt/ext-toolchain
export TOOLCHAIN_PATH=$ARM_LINUX_TOOLCHAIN_DIR/bin
make -C ambarella/bootloader/boards/h2bub sync_build_mkcfg
make -C ambarella/bootloader/boards/h2bub h2bub_amboot_only_config
make -C ambarella/bootloader/boards/h2bub


###########################################
# 3. ORC Compilation and run test on board
###########################################
# On Server:
# remoteconfig /dump10/cychen/pace/SL6/tests/utadder -opt pairing_key=0x12345678 -opt pairing_server=10.2.4.133 && make build && make -C orc run

# On BUB
# modprobe ambarella-eth && ifconfig eth0 hw ether 12:22:33:ab:ab:ab up && udhcpc && udhcpc
# cd /usr/bin/baseline &&  ./baseline -s 10.2.4.133 -k 0x12345678 -f utadder.sfb -t utadder.tbar


###############################
# 4. Details steps (FYI)
###############################

#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# 4.1 ORC Compilation
#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# cd /test/amalgam
# remoteconfig /path-to-pace-sl6-directory/tests/utadder -opt pairing_key=0x12345678 -opt pairing_server=aa.bb.cc.dd
# make code
# make data

#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# 4.2 Running the test: make sure your server and H2EVK are on the same subnet
#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

###>
# On Server:
# cd /test/amalgam

# On BUB
# Config H2BUB to connect intranet. The 2nd udhcpc is for TW intranet.
#> modprobe ambarella-eth && ifconfig eth0 hw ether 12:22:33:ab:ab:ab up && udhcpc && udhcpc
## or
# ifconfig eth0 down
# ifconfig eth0 hw ether 12:22:33:ab:ab:ab up
## The last bit of first octel can't be 1 due to multi-cast.
## Or add "mac-address = [00 00 00 00 00 00]" in dts.
# udhcpc -i eth0

# Run cv app
#> cd /usr/bin/baseline
#> ./baseline -s aa.bb.cc.dd -k 0x12345678 -f utadder.sfb -t utadder.tbar

