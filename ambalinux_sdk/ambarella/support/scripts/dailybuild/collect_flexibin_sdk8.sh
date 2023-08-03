#!/bin/sh
# $1 : destination path.
# it is something like: /dump17/qaauto/share/SYSTEM_BRANCH_DAILY_BUILD/{date}/{branch}/ambarella_{chip}_ambalink_ambacv_defconfig

echo "Start to collect bin files"
Dst=${1}
echo "Dst path: $Dst"

# APP : flexidag_appexample
rm -rf $Dst/flexidag_appexample/bin/app_appexample/flexibin
mkdir -p $Dst/flexidag_appexample/bin/app_appexample/flexibin
if [ -f cv/mnet_ssd_adas_pic_ag/mbox_priorbox.bin ]
then
    cp -pfr cv/mnet_ssd_adas_pic_ag/mbox_priorbox.bin $Dst/flexidag_appexample/bin/app_appexample/flexibin/
else
    echo "mbox_priorbox.bin of mnet_ssd_adas_pic_ag not found. Skip!"
fi
if [ -f cv/mnet_ssd_adas_pic_ag/flexibin/flexibin0.bin ]
then
    cp -pfr cv/mnet_ssd_adas_pic_ag/flexibin/flexibin0.bin $Dst/flexidag_appexample/bin/app_appexample/flexibin/
else
    echo "flexibin0.bin of mnet_ssd_adas_pic_ag not found. Skip!"
fi

# APP : flexidag_openod
rm -rf $Dst/flexidag_openod/bin/app_openod/flexibin
mkdir -p $Dst/flexidag_openod/bin/app_openod/flexibin
if [ -f cv/flexidag_openod_ag/arm_priorbox.bin ]
then
    cp -pfr cv/flexidag_openod_ag/arm_priorbox.bin $Dst/flexidag_openod/bin/app_openod/flexibin/
else
    echo "arm_priorbox.bin of flexidag_openod_ag not found. Skip!"
fi
if [ -f cv/flexidag_openod_ag/flexibin/flexibin0.bin ]
then
    cp -pfr cv/flexidag_openod_ag/flexibin/flexibin0.bin $Dst/flexidag_openod/bin/app_openod/flexibin/
else
    echo "flexibin0.bin of flexidag_openod_ag not found. Skip!"
fi

# APP : flexidag_openseg
rm -rf $Dst/flexidag_openseg/bin/app_openseg/flexibin
mkdir -p $Dst/flexidag_openseg/bin/app_openseg/flexibin
if [ -f cv/flexidag_openseg_ag/flexibin/flexibin0.bin ]
then
    cp -pfr cv/flexidag_openseg_ag/flexibin/flexibin0.bin $Dst/flexidag_openseg/bin/app_openseg/flexibin/
else
    echo "flexibin0.bin of flexidag_openseg_ag not found. Skip!"
fi

# APP : flexidag_openodseg
rm -rf $Dst/flexidag_openodseg/bin/app_openodseg/flexibin
mkdir -p $Dst/flexidag_openodseg/bin/app_openodseg/flexibin
if [ -f cv/flexidag_openod_ag/arm_priorbox.bin ]
then
    cp -pfr cv/flexidag_openod_ag/arm_priorbox.bin $Dst/flexidag_openodseg/bin/app_openodseg/flexibin/
else
    echo "arm_priorbox.bin of flexidag_openod_ag not found. Skip!"
fi
if [ -f cv/flexidag_openod_ag/flexibin/flexibin0.bin ]
then
    cp -pfr cv/flexidag_openod_ag/flexibin/flexibin0.bin $Dst/flexidag_openodseg/bin/app_openodseg/flexibin/
else
    echo "flexibin0.bin of flexidag_openod_ag not found. Skip!"
fi
if [ -f cv/flexidag_openseg_ag/flexibin/flexibin0.bin ]
then
    cp -pfr cv/flexidag_openseg_ag/flexibin/flexibin0.bin $Dst/flexidag_openodseg/bin/app_openodseg/flexibin/flexibin1.bin
else
    echo "flexibin0.bin of flexidag_openseg_ag not found. Skip!"
fi

echo "Collection done!"

