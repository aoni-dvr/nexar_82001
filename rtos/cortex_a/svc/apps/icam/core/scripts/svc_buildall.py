#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os

is_cv = os.popen('find . -maxdepth 1 -type d -name cv').read()
if is_cv != '':
    make = 'make KBUILD_KCONFIG=refapp/svc/var/Kconfig KBUILD_AMBA_MKFILE=refapp/svc/Makefile.Amba CONFIG_CC_AMBAVER_LIB_DATE=y'
else:
    make = 'make KBUILD_KCONFIG=refapp/svc/var/Kconfig.nocv KBUILD_AMBA_MKFILE=refapp/svc/Makefile.Amba CONFIG_CC_AMBAVER_LIB_DATE=y'  

lines = os.popen('ls configs/refapp/svc').read()
if lines != '':
    lines = lines.splitlines()
    for line in lines:
        if 'defconfig' in line:
            # remove output
            cmd = 'rm -rf output;rm -rf output.64'
            sys.stderr.write(cmd + '\n')
            os.system(cmd)
            # buid
            cmd = make + ' refapp/svc/' + line + ';'
            if is_cv != '':
                cmd = cmd + make + ' diags;' + make + ' -j'
            else:
                cmd = cmd + make + ' -j'
            sys.stderr.write(cmd + '\n')
            os.system(cmd)
