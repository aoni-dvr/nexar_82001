#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys

def sym_scan(intput, output1, output2, start, end, pattern):
    src = open(intput, 'r')
    dst1 = open(output1, 'w')
    dst2 = open(output2, 'w')

    flag = 0
    write = 0
    text = ''

    try:
        for line in src:
            if start in line:
                flag = 1
            elif end in line:
                flag = 2

            write = 1
            if flag == 1:
                if pattern in line:
                    if 'rtos/cortex_a/output' in line:
                        text = line
                    elif 'vendors/ambarella/lib' in line:
                        text = line
                    else:
                        text = line
                        text = text.rstrip('\n')
                        write = 0
                else:
                    if 'rtos/cortex_a/output' in line:
                        text += line
                    elif 'vendors/ambarella/lib' in line:
                        text += line
                    else:
                        write = 0

                if write == 1:
                    list = text.split()
                    text = list[0] + '  ' + str(int(list[2], 16)) + '  ' + list[3] + '\n'
                    if 'libsvc_icam' in text:
                        dst1.write(text)
                    else:
                        dst2.write(text)
                    text = ''

            if flag == 2:
                break;

    finally:
        src.close()
        dst1.close()
        dst2.close()


#
intput = sys.argv[1] + '/out/amba_svc.map'

# data
output1 = sys.argv[1] + '/out/dump_icam.data'
output2 = sys.argv[1] + '/out/dump_ssp.data'
sym_scan(intput, output1, output2, '__ddr_region0_rw_start = .', '__ddr_region0_rw_end = .', '.data.')
# nbss
output1 = sys.argv[1] + '/out/dump_icam.nbss'
output2 = sys.argv[1] + '/out/dump_ssp.nbss'
sym_scan(intput, output1, output2, '__noinit_start = .', '__noinit_end = .', '.bss.noinit')
# bss
output1 = sys.argv[1] + '/out/dump_icam.bss'
output2 = sys.argv[1] + '/out/dump_ssp.bss'
sym_scan(intput, output1, output2, '__bss_start = .', '__bss_end = .', '.bss.')

