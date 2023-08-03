#!/usr/bin/python

import os, sys
import argparse
import fileinput
import bitarray
import bitarray.util

import plat

from util import Bitfield, Register
from remove_comments import remove_comments
from collections import Counter
from pprint import pprint
from enum import Enum

__version__ = "v1.21"
DEBUG = True

class ModeType(Enum):
    ASM_TO_INI = 0
    H_TO_ASM = 1

def version():
    s = ""
    s += "rtos_dram_asm_converter: %s\n" % __version__
    s += "parser components:\n"
    s += "\n".join(["\t"+p.version() for p in plat.PARSER.values()])
    return s

def valid_platform(p):
    try:
        p = p.upper()
        if p in plat.PLATFORM_LIST:
            return p
        else:
            raise ValueError
    except ValueError:
        msg = "Not a valid platform: {0!r}\n".format(p)
        msg += "Currently supported platform: %s" % str(PLATFORM_LIST)
        raise argparse.ArgumentTypeError(msg)

def valid_mode(m):
    try:
        m = int(m)
        if (m >= 0) and (m < len(ModeType)):
            return m
        else:
            raise ValueError
    except ValueError:
        msg = "Not a valid mode: {0!r}\n".format(m)
        msg += "Currently supported mode:\n"
        for option in ModeType:
            msg += "%d: %s\n" % (option.value, option.name)
        raise argparse.ArgumentTypeError(msg)

def arg_parse():
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("-m", "--mode", help="Mode (0: asm->ini, 1: h->asm)", nargs=1, type=valid_mode, required=True)
    parser.add_argument("-p", "--platform", help="Specify platform [%s]" % ("/".join(p for p in plat.PARSER.keys())), nargs=1, type=valid_platform, required=True)
    parser.add_argument("-o", "--output", help="Output file", nargs='?', type=str, required=True)
    parser.add_argument("-t", "--template", help="Specify asm template file for h->asm substitution", nargs=1, type=str)
    parser.add_argument("-v", "--version", help="Show version", action="version", version=version())
    parser.add_argument("-d", "--debug", help="Enable more debug messages", action="store_true")
    parser.add_argument("file", help="Specify file(s)", nargs='+', type=str)
    args = parser.parse_args()
    return args

def update_asm_sym(asm_dict, h_mapped_dict, platform):
    global DEBUG
    for asm_sym, h_dict in h_mapped_dict.items():
        val = list(h_dict.values())[0]  # use 1-st value?
        if len(h_dict) > 1:
            diff = False
            print("[Warning] Multiple symbols in .h mapped to %s in .asm! " % asm_sym, end='')
            if not plat.all_equal(h_dict.values()):
                diff = True
                print("(also with different values!)")
            else:
                print("(but same value)")

            if DEBUG or diff:
                for h_sym, h_val in h_dict.items():
                    extra_str = " ---> will use this" if (h_val == val) else ""
                    print("          %s (0x%08x)%s" % (h_sym, h_val, extra_str))

        if asm_dict.get(asm_sym):
            asm_dict[asm_sym][0] = val
            asm_dict[asm_sym][1] = True
        else:
            print("[Warning] %s is not in asm template! (Info could be lost in translations...)" % asm_sym)

def update_ini_sym(d, platform):
    out = {}

    # ===== Common parsing =====
    # Search for PLL
    for k, v in plat.PLL_FREQ_MAP.items():
        if k in d:
            freq = plat.calculate_PLL_freq(d[k][0], k)
            out.update({v: "%d MHz" % freq})
            del d[k]

    # ===== Platform parsing =====
    plat.PARSER[platform].parse(out, d, platform)

    return out

def get_h_parameters(f):
    d = {}
    mapped_d = {}
    dup = Counter()     # For keeping duplicated sym

    for line in f.readlines():
        line = line.strip()
        if line == "": continue

        token = line.split()
        if (token[0] == "#define") and (len(token) == 3):
            sym = token[1].strip()
            val = token[2].strip()
            val = int(val, 16)
            #print("%s = %s" % (sym, val))

            if plat.PARSER[platform].H_TO_ASM_MAP.get(sym):
                mapped_sym = plat.PARSER[platform].H_TO_ASM_MAP[sym]
            else:
                print("[Warning] %s is not in h->asm mapping table! (Info could be lost in translations...)" % sym)
                continue

            # Create duplicated key, add "_DUPX" to sym key and map to "HostX" in H_TO_ASM_MAP
            if d.get(sym):
                dup[sym] += 1
                if dup[sym] == 1:
                    temp = d[sym]
                    new_sym = "%s_DUP0" % sym
                    d.update({new_sym: temp})
                    plat.PARSER[platform].H_TO_ASM_MAP.update({new_sym: mapped_sym})
                new_sym = "%s_DUP%d" % (sym, dup[sym])
                # sym: [val, updated]
                d.update({new_sym: [val, False]})
                # For CV2
                mapped_sym = [x.replace("Ddrc0", "Ddrc%d" % dup[sym]) for x in mapped_sym]
                # For CV5x
                mapped_sym = [x.replace("Host0", "Host%d" % dup[sym]) for x in mapped_sym]
                # Final update
                plat.PARSER[platform].H_TO_ASM_MAP.update({new_sym: mapped_sym})
            else:
                # sym: [val, updated]
                d.update({sym: [val, False]})

            # Update table at asm side to see if 1 .asm sym has multiple .h sym
            for msym in mapped_sym:
                if mapped_d.get(msym):
                    mapped_d[msym].update({sym: val})
                else:
                    mapped_d.update({msym: {sym: val}})

    #pprint(plat.PARSER[platform].H_TO_ASM_MAP)

    # Clear duplicated original key
    for k in d:
        if f in dup:
            del d[k]

    return d, mapped_d

def get_asm_parameters(f):
    d = {}

    for line in f.readlines():
        line = line.strip()
        if line == "": continue

        if ".word" in line:
            token = line.split(".word")
            sym = token[0].strip()[:-1]
            val = token[1].strip()
            #print("%s = %s" % (sym, val))

            try:
                # sym: [val, updated]
                d.update({sym: [int(val, 16), False]})
            except Exception as e:
                print("[Error] %s: %s" % (sys._getframe().f_code.co_name, e))

    return d

def write_asm(f_read, f_write, d):
    for line in f_read:
        line = line.strip()
        token = line.split()
        if len(token) != 3:
            print(line, file=f_write)
        else:
            mod = False
            sym = token[0][:-1]
            if sym in d:
                if d[sym][1]:
                    val = int(token[2], 16)
                    #token[2] = "0x%08x" % d[sym][0]
                    token[2] = ("0x%08x" % d[sym][0]) + ("\t// [MODIFIED!]" if (d[sym][0] != val) else "")
                    print("%s --- val=%x, d[sym][0]=%x" % (line, val, d[sym][0]))
                    s = "%s\t\t\t%s\t%s" % (token[0], token[1], token[2])
                    #s = "\t".join(token)
                    print(s, file=f_write)
                    mod = True

            if not mod:
                print(line, file=f_write)

def write_ini(f, d):
    tab = 0

    print('<?xml version="1.0" encoding="utf-8"?>', file=f)
    print('<root version="20160107">', file=f)
    tab += 1

    for order_key in plat.INI_ITEM_ORDER:
        if d.get(order_key):
            k = order_key
            v = d[order_key]
            for t in range(tab):
                print('\t', end='', file=f)
            print('<%s value="%s"/>' % (k, v), file=f)

    tab -= 1
    print('</root>', file=f)


if __name__ == '__main__':
    args = arg_parse()
    platform = args.platform[0]
    mode = args.mode[0]
    debug = args.debug

    if debug:
        DEBUG = True
    else:
        DEBUG = False

    #pprint(INI_ITEM_ORDER)

    output = args.output
    if (mode == ModeType.ASM_TO_INI.value):
        #output += ".ini"
        print("Translating from .asm to .ini ...")
    elif (mode == ModeType.H_TO_ASM.value):
        #output += ".asm"
        print("Translating from .h to .asm ...")
        template = args.template[0] if args.template else None
    print("Output to: %s" % os.path.abspath(output))

    for i, f in enumerate(args.file):
        print("%d. %s" % (i+1, f))
        print("==============================")

        # Remove comments of .asm/.h
        new_f = remove_comments(open(f).read())
        f_temp = f + ".temp"
        f_handle = open(f_temp, "w")
        f_handle.write(new_f)
        f_handle.close()

        if (mode == ModeType.ASM_TO_INI.value):
            f_handle = open(f_temp, "r")
            para = get_asm_parameters(f_handle)
            f_handle.close()
            os.remove(f_temp)

            sym = update_ini_sym(para, platform)

            f_handle = open(output, 'w')
            write_ini(f_handle, sym)
            f_handle.close()

        elif (mode == ModeType.H_TO_ASM.value):
            f_handle = open(f_temp, "r")
            para, mapped_para = get_h_parameters(f_handle)
            f_handle.close()
            os.remove(f_temp)

            dram_type = plat.PARSER[platform].get_dram_type(para)
            print(dram_type)

            # Decide the temp asm for use
            if template:
                f_asm = os.path.abspath(template)
            else:
                try:
                    f_asm = os.path.dirname(os.path.abspath(__file__))
                    f_asm += plat.PARSER[platform].ASM_TEMPLATE[dram_type]
                except:
                    print("No suitable asm template for %s %s..." % (platform, dram_type))

            # Remove comments of .asm
            new_f = remove_comments(open(f_asm).read())
            f_asm_temp = f_asm + ".temp"
            f_handle = open(f_asm_temp, "w")
            f_handle.write(new_f)
            f_handle.close()
            # Read from temp asm
            f_handle = open(f_asm_temp, "r")
            temp = get_asm_parameters(f_handle)
            f_handle.close()
            os.remove(f_asm_temp)

            #update_asm_sym(temp, para, platform)
            update_asm_sym(temp, mapped_para, platform)

            f_out = open(output, "w")
            f_handle = open(f_asm, "r")
            write_asm(f_handle, f_out, temp)
            f_handle.close()
            f_out.close()

