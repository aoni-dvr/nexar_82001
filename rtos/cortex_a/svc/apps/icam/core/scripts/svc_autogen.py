#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import csv
import os
import shutil
import struct

# general ucode (cv2x, cv2xfs, h32)
UCPAM_IDX_DEFBIN   = 0
UCPAM_IDX_ORCCODE  = 1
UCPAM_IDX_ORCME0   = 2
UCPAM_IDX_ORCMD0   = 3
# cv5x only
UCPAM_IDX_ORCVIN   = 4
UCPAM_IDX_ORCIDSP0 = 5
UCPAM_IDX_ORCIDSP1 = 6
UCPAM_IDX_ORCME1   = 7
UCPAM_IDX_ORCMD1   = 8
#
UCPAM_IDX_MAX      = 9

def pref_prefix(info_file):
    prefix = 'xxx'
    ver = ''

    exist = os.popen('which repo').read()
    if exist != '':
        info = os.popen('repo info').read()
        if info != '':
            lines = info.splitlines()
            for idx in range(len(lines)):
                if 'Manifest branch' in lines[idx]:
                    ver = lines[idx]
                    break

            # store branch information to .info_svc
            if ver != '':
                ver = ver.rpartition(' ')
                prefix = ver[2]
                prefix = prefix[:16]
            else:
                prefix = 'unknown'

            if 'ambalink' in prefix:
                prefix = 'xxx'
            else:
                finfo = open(info_file, 'w')
                finfo.write(prefix + '\n')
                finfo.close()

    if prefix == 'xxx':
        if os.path.isfile(info_file) == True:
            with open(info_file, 'r') as finfo:
                prefix = finfo.readline()
                prefix = prefix.rstrip('\n')

    return prefix

def ucname2index(name):
    uc_list = [['default_binary.bin', UCPAM_IDX_DEFBIN,   'UCODE_FILE_DEFBIN'  ],
               ['orccode.bin',        UCPAM_IDX_ORCCODE,  'UCODE_FILE_ORCCODE' ],
               ['orcme.bin',          UCPAM_IDX_ORCME0,   'UCODE_FILE_ORCME0'  ],
               ['orcme0.bin',         UCPAM_IDX_ORCME0,   'UCODE_FILE_ORCME0'  ],
               ['orcme1.bin',         UCPAM_IDX_ORCME1,   'UCODE_FILE_ORCME1'  ],
               ['orcmdxf.bin',        UCPAM_IDX_ORCMD0,   'UCODE_FILE_ORCMD0'  ],
               ['orcmdxf0.bin',       UCPAM_IDX_ORCMD0,   'UCODE_FILE_ORCMD0'  ],
               ['orcmdxf1.bin',       UCPAM_IDX_ORCMD1,   'UCODE_FILE_ORCMD1'  ],
               ['orcvin.bin',         UCPAM_IDX_ORCVIN,   'UCODE_FILE_ORCVIN'  ],
               ['orcidsp0.bin',       UCPAM_IDX_ORCIDSP0, 'UCODE_FILE_ORCIDSP0'],
               ['orcidsp1.bin',       UCPAM_IDX_ORCIDSP1, 'UCODE_FILE_ORCIDSP1']]

    result = [-1, '']

    for i in range(len(uc_list)):
        if uc_list[i][0] == name:
            result[0] = uc_list[i][1]
            result[1] = uc_list[i][2]

    return result

def ucode_readme(readme_file, bin_file, part_idx, part_size):
    offset = 0

    file = open(readme_file, 'r')
    lines = file.readlines()
    file.close()

    for line in lines:
        line = line.strip()
        # CVxx
        if 'PARTIAL_OFFSET' in line or 'RODATA_OFFSET' in line:
            result = line.split(' ')
            part_size[part_idx] = int(result[1], 10) - offset
            offset = offset + part_size[part_idx]
            part_idx = part_idx + 1
            if 'RODATA_OFFSET' in line:
                break

    filesz = os.path.getsize(bin_file)
    part_size[part_idx] = filesz - offset

    print('filesz, offset' + str(filesz) + ' ' + str(offset))
    return part_size

def ucode_parse(ucode_list, socx):
    ucode_path = ucode_list
    if '/ucode.list' in ucode_path:
        ucode_path = ucode_path.replace('/ucode.list', '')
    print('# ' + ucode_path + ' #')

    index = 0
    offset = 0
    filesz = 0
    uc_param = []
    orccode_size  = [0, 0, 0]
    orcme0_size   = [0, 0, 0]
    orcme1_size   = [0, 0, 0]
    orcmd0_size   = [0, 0, 0]
    orcmd1_size   = [0, 0, 0]
    defbin_size   = [0, 0, 0]
    orcvin_size   = [0, 0, 0]
    orcidsp0_size = [0, 0, 0]
    orcidsp1_size = [0, 0, 0]

    # default_binary (UCPAM_IDX_DEFBIN)
    readme_file = ucode_path + '/default_binary.readme'
    bin_file = ucode_path + '/default_binary.bin'
    if os.path.isfile(readme_file) == True and ucode_partial == 'y':
        file = open(readme_file, 'r')
        lines = file.readlines()
        file.close()

        # parse offset of partial length
        offset = 0
        for line in lines:
            line = line.strip()
            if 'cabac_init_ctx_daddr;' in line:
                break;

            if 'uint8' in line:
                offset = offset + 1
            if 'uint16' in line:
                offset = offset + 2
            if 'uint32' in line:
                offset = offset + 4
            if 'char' in line:
                char_line = line
                line = line.partition('[')
                line = line[2]
                line = line.partition(']')
                line = line[0]
                offset = offset + int(line[0], 10)

        if offset != 0:
            # read partial length from bin file
            file = open(bin_file, 'rb')
            file.seek(offset, 0)
            offset = struct.unpack('<I', file.read(4))
            offset = offset[0]
            file.close()

            filesz = os.path.getsize(bin_file)
            defbin_size[0] = offset
            defbin_size[1] = filesz - offset
    else:
        if os.path.isfile(bin_file) == True:
            defbin_size[0] = os.path.getsize(bin_file)
    uc_param.append(defbin_size)

    # orccode (UCPAM_IDX_ORCCODE)
    index = 0
    if ucode_partial == 'y' and socx == 'cv5x':
        index = 1

    bin_file = ucode_path + '/orccode.bin'
    readme_file = ucode_path + '/orccode.readme'

    if os.path.isfile(bin_file) == True:
        if os.path.isfile(readme_file) == True and ucode_partial == 'y':
            orccode_size = ucode_readme(readme_file, bin_file, index, orccode_size)
        else:
            orccode_size[index] = os.path.getsize(bin_file)
        uc_param.append(orccode_size)

    # orcme/orcme0 (UCPAM_IDX_ORCME0)
    index = 0
    if ucode_partial == 'y' and socx == 'cv5x':
        index = 1

    bin_file = ucode_path + '/orcme.bin'
    if os.path.isfile(bin_file) != True:
        bin_file = ucode_path + '/orcme0.bin'
        if os.path.isfile(bin_file) != True:
            bin_file = ''

    if bin_file != '':
        orcme0_size[index] = os.path.getsize(bin_file)
        uc_param.append(orcme0_size)

    # orcmdxf/orcmdxf0 (UCPAM_IDX_ORCMD0)
    index = 0
    if ucode_partial == 'y' and socx == 'cv5x':
        index = 1

    bin_file = ucode_path + '/orcmdxf.bin'
    if os.path.isfile(bin_file) != True:
        bin_file = ucode_path + '/orcmdxf0.bin'
        if os.path.isfile(bin_file) != True:
            bin_file = ''

    if bin_file != '':
        orcmd0_size[index] = os.path.getsize(bin_file)
        uc_param.append(orcmd0_size)

    # orcvin (UCPAM_IDX_ORCVIN)
    index = 0

    bin_file = ucode_path + '/orcvin.bin'
    if os.path.isfile(bin_file) != True:
        bin_file = ucode_path + '/orcvin0.bin'
        if os.path.isfile(bin_file) != True:
            bin_file = ''

    if bin_file != '':
        readme_file = ucode_path + '/orcvin.readme'
        if os.path.isfile(readme_file) == True and ucode_partial == 'y':
            orcvin_size = ucode_readme(readme_file, bin_file, index, orcvin_size)
        else:
            orcvin_size[index] = os.path.getsize(bin_file)
        uc_param.append(orcvin_size)

    # orcidsp0/orcidsp1 (UCPAM_IDX_ORCIDSP0/UCPAM_IDX_ORCIDSP1)
    index = 0

    bin_file = ucode_path + '/orcidsp0.bin'
    readme_file = ucode_path + '/orcidsp.readme'
    if os.path.isfile(bin_file) == True:
        if os.path.isfile(readme_file) == True and ucode_partial == 'y':
            orcidsp0_size = ucode_readme(readme_file, bin_file, index, orcidsp0_size)
        else:
            orcidsp0_size[index] = os.path.getsize(bin_file)
        uc_param.append(orcidsp0_size)

    bin_file = ucode_path + '/orcidsp1.bin'
    if os.path.isfile(bin_file) == True:
        if os.path.isfile(readme_file) == True and ucode_partial == 'y':
            orcidsp1_size = ucode_readme(readme_file, bin_file, index, orcidsp1_size)
        else:
            orcidsp1_size[index] = os.path.getsize(bin_file)
        uc_param.append(orcidsp1_size)

    # orcme1 (UCPAM_IDX_ORCME1)
    index = 0
    if ucode_partial == 'y' and socx == 'cv5x':
        index = 1

    bin_file = ucode_path + '/orcme1.bin'
    if os.path.isfile(bin_file) == True:
        orcme1_size[index] = os.path.getsize(bin_file)
        uc_param.append(orcme1_size)

    # orcmdx1 (UCPAM_IDX_ORCMD1)
    index = 0
    if ucode_partial == 'y' and socx == 'cv5x':
        index = 1

    bin_file = ucode_path + '/orcmdxf1.bin'
    if os.path.isfile(bin_file) == True:
        orcmd1_size[index] = os.path.getsize(bin_file)
        uc_param.append(orcmd1_size)

    print(uc_param)
    return uc_param

def config_parse(build_path, src_path, svc_path):
    soc = ''
    socx = ''
    board = ''
    project = ''
    csv_custom = ''
    csv_file = ''
    ucode_list = ''
    gui_list = ''
    iqdef_list = ''
    iq_list = ''
    audio_list = ''
    cv_list = ''
    app_name = ''
    ucode_partial = ''
    opers = ''
    clk_ln = ''
    uc_bist = ''

    config_file = build_path + '/.config'
    with open(config_file, 'rt') as fin:
        cfg_lines = [row for row in fin]

    for cfg_line in cfg_lines:
        if soc == '':
            if 'CONFIG_SOC_' in cfg_line:
                if '=y' in cfg_line:
                    if 'CV2' in cfg_line:
                        soc = 'cv2'
                        socx = soc
                    if 'CV2FS' in cfg_line:
                        soc = 'cv2fs'
                        socx = soc
                    if 'CV22' in cfg_line:
                        soc = 'cv22'
                        socx = soc
                    if 'CV25' in cfg_line:
                        soc = 'cv25'
                        socx = soc
                    if 'CV28' in cfg_line:
                        soc = 'cv28'
                        socx = soc
                    if 'H22' in cfg_line:
                        soc = 'h22'
                        socx = soc
                    if 'H32' in cfg_line:
                        soc = 'h32'
                        socx = soc
                    if 'CV5' in cfg_line:
                        if 'CV52' in cfg_line:
                            soc = 'cv52'
                        else:
                            soc = 'cv5'
                        socx = 'cv5x'

        if board == '':
            if 'CONFIG_BSP_' in cfg_line:
                if '=y' in cfg_line:
                    if 'BUB' in cfg_line:
                        board = 'bub'
                    if 'DK' in cfg_line:
                        board = 'dk'
                    if 'EVK' in cfg_line:
                        board = 'evk'
                    if 'RDB' in cfg_line:
                        board = 'dk'
                    if 'NEXAR_D161V2' in cfg_line:
                        board = 'nexar_d161v2'
                    elif 'NEXAR_D161' in cfg_line:
                        board = 'nexar_d161'
                    if 'NEXAR_D080' in cfg_line:
                        board = 'nexar_d080'
                    if 'NEXAR_D081' in cfg_line:
                        board = 'nexar_d081'
        if project == '':
            if 'CONFIG_ICAM_PROJECT_NAME' in cfg_line:
                idx = cfg_line.rfind('=', 0, len(cfg_line))
                if idx != -1:
                    sta = cfg_line.partition('=')
                    project = sta[2].strip()
                    project = project.strip('"')
        if csv_custom == '':
            if 'CONFIG_ICAM_CUSTOM_PLATCSV' in cfg_line:
                sta = cfg_line.partition('=')
                csv_custom = sta[2].strip()
                csv_custom = csv_custom.strip('"')
        if ucode_list == '':
            if 'CONFIG_ICAM_UCODE_ROMLIST' in cfg_line:
                sta = cfg_line.partition('=')
                ucode_list = sta[2].strip()
                ucode_list = ucode_list.strip('"')
                if '${srctree}' in ucode_list:
                    ucode_list = ucode_list.replace('${srctree}', src_path)
                if '${AMBA_CHIP_ID}' in ucode_list:
                    if socx == 'cv5x':
                        ucode_list = ucode_list.replace('${AMBA_CHIP_ID}', 'cv5')
                    else:
                        ucode_list = ucode_list.replace('${AMBA_CHIP_ID}', soc)
        if gui_list == '':
            if 'CONFIG_ICAM_GUI_ROMLIST' in cfg_line:
                sta = cfg_line.partition('=')
                gui_list = sta[2].strip()
                gui_list = gui_list.strip('"')
                if '${srctree}' in gui_list:
                    gui_list = gui_list.replace('${srctree}', src_path)
        if iqdef_list == '':
            if 'CONFIG_ICAM_IQDEF_ROMLIST' in cfg_line:
                sta = cfg_line.partition('=')
                iqdef_list = sta[2].strip()
                iqdef_list = iqdef_list.strip('"')
                if '${srctree}' in iqdef_list:
                    iqdef_list = iqdef_list.replace('${srctree}', src_path)
        if iq_list == '':
            if 'CONFIG_ICAM_IQ_ROMLIST' in cfg_line:
                sta = cfg_line.partition('=')
                iq_list = sta[2].strip()
                iq_list = iq_list.strip('"')
                if '${srctree}' in iq_list:
                    iq_list = iq_list.replace('${srctree}', src_path)
                if '${AMBA_CHIP_ID}' in iq_list:
                    iq_list = iq_list.replace('${AMBA_CHIP_ID}', socx)
        if audio_list == '':
            if 'CONFIG_ICAM_AUDIO_ROMLIST' in cfg_line:
                sta = cfg_line.partition('=')
                audio_list = sta[2].strip()
                audio_list = audio_list.strip('"')
                if '${srctree}' in audio_list:
                    audio_list = audio_list.replace('${srctree}', src_path)
        if cv_list == '':
            if 'CONFIG_ICAM_CV_ROMLIST' in cfg_line:
                sta = cfg_line.partition('=')
                cv_list = sta[2].strip()
                cv_list = cv_list.strip('"')
                if '${srctree}' in cv_list:
                    cv_list = cv_list.replace('${srctree}', src_path)
        if app_name == '':
            if 'CONFIG_SVC_APP_NAME' in cfg_line:
                idx = cfg_line.rfind('=', 0, len(cfg_line))
                if idx != -1:
                    sta = cfg_line.partition('=')
                    app_name = sta[2].strip()
                    app_name = app_name.strip('"')
        if ucode_partial == '':
            if 'CONFIG_ICAM_UCODE_PARTIAL_LOAD' in cfg_line:
                idx = cfg_line.rfind('=', 0, len(cfg_line))
                if idx != -1:
                    sta = cfg_line.partition('=')
                    ucode_partial = sta[2].strip()
                    ucode_partial = ucode_partial.strip('"')
        if opers == '':
            if 'CONFIG_THREADX=y' in cfg_line:
                opers = 'tx32'
            if 'CONFIG_QNX=y' in cfg_line:
                opers = 'qnx'
            if 'CONFIG_LINUX=y' in cfg_line:
                opers = 'linux'
        else:
            if 'CONFIG_THREADX64=y' in cfg_line:
                opers = 'tx64'

        if clk_ln == '':
            if 'CONFIG_ICAM_CLK_LINE' in cfg_line:
                idx = cfg_line.rfind('=', 0, len(cfg_line))
                if idx != -1:
                    sta = cfg_line.partition('=')
                    clk_ln = sta[2].strip()
                    clk_ln = clk_ln.strip('"')
                    clk_ln = int(clk_ln, 16)
        if uc_bist == '':
            if 'CONFIG_ICAM_BIST_UCODE' in cfg_line:
                idx = cfg_line.rfind('=', 0, len(cfg_line))
                if idx != -1:
                    sta = cfg_line.partition('=')
                    uc_bist = sta[2].strip()
                    uc_bist = uc_bist.strip('"')

    csv_file = svc_path + '/apps/' + app_name + '/main/src/config/'
    if csv_custom == '':
        csv_file = csv_file + socx + '/' + soc + '_' + board + '.csv'
    else:
        csv_file = csv_file + socx + '/' + csv_custom

    return (soc, board, project, csv_file, ucode_list, gui_list, iqdef_list, iq_list, audio_list, cv_list, app_name, ucode_partial, opers, clk_ln, uc_bist, socx)


def romlist_generate(soc, socx, opers, romlist_file, lists):
    dst = open(romlist_file, 'w')
    dst.write('<romfs>\n')
    dst.write('  <top>./</top>\n')

    top = ""
    for list_file in lists:
        if list_file != '':
            dst.write('\n  <!- ' + list_file + ' ->\n')
            with open(list_file, 'rt') as fin:
                list_lines = [row for row in fin]

            for list_line in list_lines:
                if '<!-' not in list_line:
                    if '<top>' in list_line:
                        top = list_line
                        top = top.strip()
                        top = top.partition('<top>')
                        top = top[2]
                        top = top.partition('</top>')
                        top = top[0]
                        if list_file == iq_list:
                            top = top + socx + '/'
                            if opers != 'tx32':
                                top = top + opers + '/'
                        if top[len(top) - 1] != '/':
                            top = top + '/'
                    if '<file>' in list_line:
                        file = list_line
                        file = file.strip()
                        file = file.partition('<file>')
                        file = file[0] + file[1] + top + file[2]
                        dst.write('  ' + file + '\n')

    dst.write('</romfs>\n')
    dst.close()

def securitylist_generate(security_path, soc, romlist_security_file, lists, ucode_param):
    if os.path.exists(security_path) == 0:
        os.makedirs(security_path)
    else:
        os.system('rm -rf ' + security_path + '/*')

    dst = open(romlist_security_file, 'w')
    dst.write('<romfs>\n')
    dst.write('  <top>./</top>\n')

    top = ""
    ucode_path = ""
    ucode_flist = [ ]
    for list_file in lists:
        if list_file != '':
            dst.write('\n  <!- ' + list_file + ' ->\n')
            with open(list_file, 'rt') as fin:
                list_lines = [row for row in fin]

            for list_line in list_lines:
                if '<!-' not in list_line:
                    if '<top>' in list_line:
                        top = list_line
                        top = top.strip()
                        top = top.partition('<top>')
                        top = top[2]
                        top = top.partition('</top>')
                        top = top[0]
                        if top[len(top) - 1] != '/':
                            top = top + '/'
                    if '<file>' in list_line:
                        file = list_line
                        file = file.strip()
                        file = file.partition('<file>')

                        sPath = top + file[2]
                        sPath = sPath.partition('<alias>')
                        fname = sPath[2]
                        fname = fname.partition('</alias>')
                        fname = fname[0]
                        if 'out/cv' in top:
                            sPath = build_path + '/' + sPath[0]
                        else:
                            sPath = src_path + '/' + sPath[0]

                        skip = 0
                        part_num = 0
                        ucode_bin = 0
                        if sPath.find('ambarella/ucode') != -1:
                            ucode_path = top

                            if fname.find('tf_') != -1 or fname.find('_test_bin') != -1:
                                skip = 1
                            else:
                                ucode_bin = 1

                                if fname == 'orccode.bin':
                                    part_size = ucode_param[UCPAM_IDX_ORCCODE]
                                    for i in range(len(part_size)):
                                        if 0 < part_size[i]:
                                            part_num = part_num + 1

                                if fname == 'orcidsp0.bin':
                                    part_size = ucode_param[UCPAM_IDX_ORCIDSP0]
                                    for i in range(len(part_size)):
                                        if 0 < part_size[i]:
                                            part_num = part_num + 1

                                if fname == 'orcidsp1.bin':
                                    part_size = ucode_param[UCPAM_IDX_ORCIDSP1]
                                    for i in range(len(part_size)):
                                        if 0 < part_size[i]:
                                            part_num = part_num + 1

                                if fname == 'default_binary.bin':
                                    part_size = ucode_param[UCPAM_IDX_DEFBIN]
                                    for i in range(len(part_size)):
                                        if 0 < part_size[i]:
                                            part_num = part_num + 1

                                if fname == 'orcvin.bin':
                                    part_size = ucode_param[UCPAM_IDX_ORCVIN]
                                    for i in range(len(part_size)):
                                        if 0 < part_size[i]:
                                            part_num = part_num + 1

                        plist = ['', '', '']
                        if 1 < part_num:
                            # ucode partition
                            pname = fname
                            pname = pname.partition('.')
                            sfile = open(sPath, 'rb')
                            for i in range(len(part_size)):
                                if part_size[i] != 0:
                                    plist[i] = pname[0] + '_p' + str(i) + pname[1] + pname[2]
                                    dPath = security_path + '/' + plist[i]
                                    dfile = open(dPath, 'wb')
                                    if fname == 'default_binary.bin' and i == 0:
                                        # clear crc value in header of default binary to
                                        #   skip crc checking of ucode since we make sure it's safe
                                        clear_crc = struct.pack('I', 0)
                                        dfile.write(clear_crc)
                                        sfile.seek(4)
                                        data = sfile.read(part_size[i] - 4)
                                    else:
                                        data = sfile.read(part_size[i])
                                    dfile.write(data)
                                    dfile.close()
                                    dst.write('  <file>' + dPath + '<alias>' + plist[i] + '</alias></file>\n')
                            sfile.close()
                            ucode_flist.append((fname, plist))
                        else:
                            if skip == 0:
                                if ucode_bin == 1:
                                    part_size = [0, 0, 0]
                                    index = ucname2index(fname)
                                    if index[0] != -1 and index[0] < UCPAM_IDX_MAX:
                                        part_size = ucode_param[index[0]]

                                    for i in range(len(part_size)):
                                        if 0 < part_size[i]:
                                            plist[i] = fname
                                    ucode_flist.append((fname, plist))

                                dPath = security_path + '/' + fname
                                if os.path.isfile(sPath) == True:
                                    shutil.copyfile(sPath, dPath)
                                else:
                                    print('\n\033[91m!!! \033[0m' + sPath + '\033[91m is missing\033[0m')
                                dst.write('  <file>' + dPath + '<alias>' + fname + '</alias></file>\n')

    # include test frame files (cv2xfs only)
    if ucode_path != "" and uc_bist == 'y':
        dst.write('\n  <!- test frame files ->\n')

        ucode_path = src_path + '/' + ucode_path
        tf_path = security_path + '/tf'
        os.makedirs(tf_path)
        os.popen('cp ' + ucode_path + 'tf*.bin ' + tf_path)
        os.popen('cp ' + ucode_path + '*_test_*.bin ' + tf_path)
        os.popen('rm -rf ' + tf_path + '/*_err_*.bin ')
        tf_list = os.popen('ls -d ' + tf_path + '/*').read()
        tf_list = tf_list.splitlines()
        for line in tf_list:
            lz4name = line.rpartition('/')
            tf_name = lz4name[2]
            lz4name = tf_name
            lz4name = lz4name.rpartition('.')
            lz4name = lz4name[0]
            lz4name = security_path + '/' + lz4name + '.lz4'
            #print(lz4name + '...')
            os.popen('lz4 -B4 ' + line + ' ' + lz4name)
            dst.write('  <file>' + lz4name + '<alias>' + tf_name + '</alias></file>\n')
        os.popen('rm -rf ' + tf_path)

    dst.write('</romfs>\n')
    dst.close()

    return (ucode_flist)

def autofile_generate(auto_file, info_file, partlist, ucode_param):
    dst = open(auto_file, 'w')
    dst.write('/* The file is generated by svc build flow automatically */\n')
    dst.write('#ifndef AUTOGEN_SVC_H\n')
    dst.write('#define AUTOGEN_SVC_H\n\n')

    # git branch
    soc_prefix = soc
    if soc_prefix == 'cv2fs':
        if clk_ln == 1 or clk_ln == 4:
            soc_prefix = 'cv22fs'
        if clk_ln == 2:
            soc_prefix = 'cv22fs15'

    prefix = '#define SVCAG_PREF_PREFIX        "' + soc_prefix + '_' + pref_prefix(info_file) + '"\n'
    dst.write(prefix)
    dst.write('#define SVC_ICAM_PROJECT_CFG     \"' + socx.lower() + '/Res_' + project + '.c\"\n')
    dst.write('#define SVC_ICAM_PROJECT_PBK_CFG     \"' + socx.lower() + '/Res_PBK.c\"\n')

    # read csv
    print('open: %s' % csv_file)
    with open(csv_file, 'rt') as fin:
        cin = csv.reader(fin)
        plat_table = [row for row in cin]

    for line in plat_table:
        if line[0] == '':
            dst.write('#define ' + line[1] + '       ' + line[2] + '\n')
    dst.write('\n')

    # ucode
    if ucode_list != '':
        count = 0
        for line in partlist:
            index = ucname2index(line[0])
            if index[0] != -1 and index[0] < UCPAM_IDX_MAX:
                dst.write('#define ' + index[1] + ' (' + str(index[0]) + 'U)\n')
                count = count + 1
        dst.write('#define UCODE_FILE_NUM_MAX (' + str(count) + 'U)\n\n')

        dst.write('#define SVCAG_UCODE_LIST     {\\\n')
        for line in partlist:
            uparam = ''

            index = ucname2index(line[0])
            if index[0] != -1 and index[0] < UCPAM_IDX_MAX:
                uparam = ucode_param[index[0]]
                dst.write('    [' + index[1] + '] = {\\\n')

            dst.write('        .FileName = "c:\\\\uCode\\\\' + line[0] + '",\\\n')
            dst.write('        .FileSize = 0U,\\\n')
            dst.write('        .BufBase = 0U,\\\n')
            dst.write('        .BufSize = 0U,\\\n')
            dst.write('        .Partials = {\\\n')
            for i in range(3):
                if line[1][i] != '' and uparam != '':
                    dst.write('            {"' + line[1][i] + '", ' + str(uparam[i]) + 'U},\\\n')
                else:
                    dst.write('            {"", 0U},\\\n')
            dst.write('        },\\\n')
            dst.write('    },\\\n')
        dst.write('}\n\n')

    # tf
    if soc == 'cv2fs':
        ucode_path = ucode_list
        if '/ucode.list' in ucode_path:
            ucode_path = ucode_path.replace('/ucode.list', '')

        max_bfsz = 0
        chk_list = [['/tf_c2y_case[0-2]_cfg.bin',  'C2Y_TC', 1],
                    ['/tf_y2y_case[0-9]_cfg.bin',  'Y2Y_TC', 0],
                    ['/tf_y2y_case1[0-1]_cfg.bin', 'Y2Y_TC', 1],
                    ['/*enc_test_*.bin',       'ENC_TC', 1],
                    ['/*dec_test_*.bin',       'DEC_TC', 1],
                    ['/tf_c2y_inp?.bin',       'C2Y_IN', 0],
                    ['/tf_c2y_inp??.bin',      'C2Y_IN', 1],
                    ['/tf_y2y_inp?.bin',       'Y2Y_IN', 0],
                    ['/tf_y2y_inp??.bin',      'Y2Y_IN', 1]]

        tf_num = 0
        for c_line in chk_list:
            tf_list = os.popen('ls -d ' + ucode_path + c_line[0]).read()
            tf_list = tf_list.splitlines()
            for line in tf_list:
                if '_err_' in line:
                    tf_list.remove(line)
            if c_line[2] == 1:
                tf_num = tf_num + len(tf_list)
                dst.write('#define SVCAG_TF_' + c_line[1] + '_NUM     (' + str(tf_num) + 'U)\n')
                tf_num = 0
            else:
                tf_num = len(tf_list)

        dst.write('#define SVCAG_TF_LIST           {\\\n')
        index = 0
        for c_line in chk_list:
            tf_list = os.popen('ls -d ' + ucode_path + c_line[0]).read()
            tf_list = tf_list.splitlines()
            for line in tf_list:
                if '_err_' not in line:
                    fname = line.rpartition('/')
                    fsize = os.stat(line).st_size
                    dst.write('    [' + str(index) +'] = {.FileName = "' + fname[2] + '", .FileSize = ' + str(fsize) + '},\\\n')
                    max_bfsz = max_bfsz + fsize
                    index = index + 1

        dst.write('}\n')

    dst.write('\n#endif\n')
    dst.close()


#
# main entry
#
build_path = os.getcwd()

if len(sys.argv) >= 2:
    src_path = sys.argv[1]
    security_path = build_path + '/security'
else:
    src_path = os.path.dirname(build_path)
    security_path = src_path + '/output.64/security'

svc_path = src_path + '/svc'

params = config_parse(build_path, src_path, svc_path)
soc = params[0]
board = params[1]
project = params[2]
csv_file = params[3]
ucode_list = params[4]
gui_list = params[5]
iqdef_list = params[6]
iq_list = params[7]
audio_list = params[8]
cv_list = params[9]
app_name = params[10]
ucode_partial = params[11]
opers = params[12]
clk_ln = params[13]
uc_bist = params[14]
socx = params[15]

auto_file = build_path + '/.svc_autogen'
romlist_file = build_path + '/.svc_romlist'
romlist_security_file = build_path + '/.svc_romlist_security'

info_file = svc_path + '/apps/' + app_name + '/core/icam.info'

print('## ' + soc + '_' + board + '_' + project + '_' + csv_file + '_' + auto_file + '_' + info_file + ' ##')
print('## ' + gui_list + '_' + iqdef_list + '_' + iq_list + '_' + audio_list + '_' + cv_list + ' ##')

if soc != '':
    ## generate .svc_romlist
    if gui_list != '' or iqdef_list != '' or iq_list != '' or audio_list != '':
        lists = [gui_list, iqdef_list, iq_list, audio_list]
        romlist_generate(soc, socx, opers, romlist_file, lists)

    ## generate .svc_security_romlist
    ucode_param = ''
    partlist = ''
    if ucode_list != '' or cv_list != '':
        # ucode parse
        ucode_param = ucode_parse(ucode_list, socx)

        lists = [ucode_list, cv_list]
        partlist = securitylist_generate(security_path, soc, romlist_security_file, lists, ucode_param)

    ## generate .svc_autogen and icam.info
    autofile_generate(auto_file, info_file, partlist, ucode_param)

else:
    print('!!! parameters aren\'t correct')
