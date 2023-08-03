#!/bin/sh
#This script is used to pass command to amba_svc process.
#$1: amba_svc command/parameters

if [ $1 == 'help' ]; then
    echo ''
    echo '## ambash commands list ##'
    echo ''
    echo '  ambash_cv'
    echo '  ambash_dmesg'
    echo '  ambash_eject'
    echo '  ambash_format'
    echo '  ambash_imgfrw'
    echo '  ambash_monfrw'
    echo '  ambash_imgproc'
    echo '  ambash_readl'
    echo '  ambash_savebin'
    echo '  ambash_sysinfo'
    echo '  ambash_thruput'
    echo '  ambash_writel'
    echo '  ambash_vol'
    echo ''
    echo '  svc_app'
    echo '  svc_button'
    echo '  svc_cv'
    echo '  svc_display_task'
    echo '  svc_dsp'
    echo '  svc_flow'
    echo '  svc_fwupdate'
    echo '  svc_gui'
    echo '  svc_pymdcap'
    echo '  svc_rec'
    echo '  svc_pbk'
    echo '  svc_audio'
    echo '  svc_rec_task'
    echo '  svc_sys'    
    echo '  svc_cal'
    echo '  svc_ik'
    echo '  svc_rawcap'
    echo '  svc_vinerr'
    echo ''
    echo '  diag_ddr'
    echo ''
else
    PTYS=$(cat /tmp/icam.pts | tr -d '\n')
    echo $@ > ${PTYS}
    sleep 0.1
fi
