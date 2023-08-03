#!/bin/bash

#(Write By PaulYeh) 

export PATH=/opt/cov-analysis-linux64-2020.12/bin:/opt/cov-reports-2020.12/bin:$PATH

Script=`readlink -f $0`
ScriptDir=`dirname $Script`

rm -rf "template-*" "rtos_config.xml*" "configure-log.txt"

# for gcc 32bit
cov-configure --config rtos_config.xml --template --comptype gcc --compiler arm-none-eabi-gcc 
# for gcc 64bit 
cov-configure --config rtos_config.xml --template --comptype gcc --compiler aarch64-none-elf-gcc
# for cv
cov-configure --config rtos_config.xml --template --comptype gcc --compiler vis-gcc
# for qnx 
cov-configure --config rtos_config.xml --template --comptype qnxcc --compiler qcc
