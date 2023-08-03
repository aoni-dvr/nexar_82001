import re
import bitarray
import bitarray.util

# import parser of soc here
import parser_cv3
import parser_cv5x
import parser_cv2
import parser_cv22
import parser_cv25
import parser_cv28

from util import Bitfield, Register
from collections import OrderedDict
from itertools import groupby
from operator import itemgetter

#PLATFORM_LIST = ['CV22', 'CV25', 'CV28', 'CV5', 'CV3']
#PLATFORM_LIST = ['CV5', 'CV3']
PLATFORM_LIST = ['CV2', 'CV22', 'CV25', 'CV28', 'CV5']
PARSER = {
    "CV2": parser_cv2,
    "CV22": parser_cv22,
    "CV25": parser_cv25,
    "CV28": parser_cv28,
    "CV5": parser_cv5x
#    "CV3": parser_cv3
}

HOST_COUNT = {
    "CV2": 2,
    "CV22": 1,
    "CV25": 1,
    "CV28": 1,
    "CV5": 2,
    "CV3": 8
}

DIE_COUNT = {
    "CV2": 1,
    "CV22": 1,
    "CV25": 1,
    "CV28": 1,
    "CV5": 2,
    "CV3": 2
}

CH_COUNT = {
    "CV2": 1,
    "CV22": 1,
    "CV25": 1,
    "CV28": 1,
    "CV5": 2,
    "CV3": 2
}

BYTE_COUNT = {
    "CV2": 1,
    "CV22": 1,
    "CV25": 1,
    "CV28": 1,
    "CV5": 4,
    "CV3": 4
}

PLL_FREQ_MAP = {
    "PLL_CortexCtrlParam": "CPU_FREQUENCY",
    "PLL_CoreCtrlParam": "CORE_FREQUENCY",
    #"PLL_DdrCtrlParam": "DRAM_FREQUENCY"    # Calculate in soc parser
}

INI_ITEM_NUM_MANDATORY = 0
INI_ITEM_NUM_OPTIONAL_DDR = 29
INI_ITEM_ORDER = OrderedDict({
    # ========== Mandatory part ==========
    "CHIP_NAME":                    INI_ITEM_NUM_MANDATORY + 1,
    "CPU_FREQUENCY":                INI_ITEM_NUM_MANDATORY + 2,
    "CORE_FREQUENCY":               INI_ITEM_NUM_MANDATORY + 3,
    "IDSP_FREQUENCY":               INI_ITEM_NUM_MANDATORY + 4,
    "VISION_FREQUENCY":             INI_ITEM_NUM_MANDATORY + 5,
    "FEX_FREQUENCY":                INI_ITEM_NUM_MANDATORY + 6,
    "DRAM_FREQUENCY":               INI_ITEM_NUM_MANDATORY + 7,
    "DRAM_TYPE":                    INI_ITEM_NUM_MANDATORY + 8,
    "DRAM_SPEED_GRADE":             INI_ITEM_NUM_MANDATORY + 9,
    "DRAM_SIZE":                    INI_ITEM_NUM_MANDATORY + 10,
    "DRAM_BUS_WIDTH":               INI_ITEM_NUM_MANDATORY + 11,
    "DRAM_DUAL_RANK":               INI_ITEM_NUM_MANDATORY + 12,
    "DRAM_TRAINING":                INI_ITEM_NUM_MANDATORY + 13,
    "DDRC_TERM":                    INI_ITEM_NUM_MANDATORY + 14,
    "DDRC_DDS":                     INI_ITEM_NUM_MANDATORY + 15,
    "DDRC_PDDS":                    INI_ITEM_NUM_MANDATORY + 16,
    "DDRC_READ_DLL":                INI_ITEM_NUM_MANDATORY + 17,
    "DDRC_SYNC_DLL":                INI_ITEM_NUM_MANDATORY + 18,
    "DDRC_WRITE_DLL":               INI_ITEM_NUM_MANDATORY + 19,
    "DDRC_DQ_READ_DELAY":           INI_ITEM_NUM_MANDATORY + 20,
    "DDRC_DQ_WRITE_DELAY":          INI_ITEM_NUM_MANDATORY + 21,
    "DDRC_DQ_VREF":                 INI_ITEM_NUM_MANDATORY + 22,
    "DDRC_CA_VREF":                 INI_ITEM_NUM_MANDATORY + 22.1,
    "DDRC_DQ_CA_VREF":              INI_ITEM_NUM_MANDATORY + 22.2,
    # BYTE_MAP is mandatory if bytes are swapped
    "DDRC_BYTE_MAP":                INI_ITEM_NUM_MANDATORY + 23,
    "DDRC0_BYTE_MAP":               INI_ITEM_NUM_MANDATORY + 23.1,
    "DDRC1_BYTE_MAP":               INI_ITEM_NUM_MANDATORY + 23.2,
    "DDRC2_BYTE_MAP":               INI_ITEM_NUM_MANDATORY + 23.3,
    "DDRC3_BYTE_MAP":               INI_ITEM_NUM_MANDATORY + 23.4,
    "DDRC4_BYTE_MAP":               INI_ITEM_NUM_MANDATORY + 23.5,
    "DDRC5_BYTE_MAP":               INI_ITEM_NUM_MANDATORY + 23.6,
    "DDRC6_BYTE_MAP":               INI_ITEM_NUM_MANDATORY + 23.7,
    "DDRC7_BYTE_MAP":               INI_ITEM_NUM_MANDATORY + 23.8,
    "DRAM_DS":                      INI_ITEM_NUM_MANDATORY + 24,
    "DRAM_ODT":                     INI_ITEM_NUM_MANDATORY + 25,
    "DRAM_VREF":                    INI_ITEM_NUM_MANDATORY + 26,

    # ========== Optional part ==========
    "DRAM_TRAINING_REPEAT_NUM":     INI_ITEM_NUM_OPTIONAL_DDR + 1,
    "DRAM_TRAINING_VREF":           INI_ITEM_NUM_OPTIONAL_DDR + 2.0,
    "DRAM_TRAINING_RDDLY_RDDLL":    INI_ITEM_NUM_OPTIONAL_DDR + 2.1,
    "DRAM_TRAINING_WRDLY_DDRVREF":  INI_ITEM_NUM_OPTIONAL_DDR + 2.2,
    "DRAM_TRAINING_RDDLY_OFFSET":   INI_ITEM_NUM_OPTIONAL_DDR + 3.0,
    "DRAM_TRAINING_WRDLY_OFFSET":   INI_ITEM_NUM_OPTIONAL_DDR + 3.1,
    "DRAM_TRAINING_VREF_OFFSET":    INI_ITEM_NUM_OPTIONAL_DDR + 3.2,
    "DRAM_TRAINING_DDRVREF_OFFSET": INI_ITEM_NUM_OPTIONAL_DDR + 3.3,
    "DRAM_TRAINING_RDDLL":          INI_ITEM_NUM_OPTIONAL_DDR + 4.0,
    "DRAM_TRAINING_WRDLL":          INI_ITEM_NUM_OPTIONAL_DDR + 4.1,
    "DRAM_TRAINING_RDDLL_OFFSET":   INI_ITEM_NUM_OPTIONAL_DDR + 5.0,
    "DRAM_TRAINING_WRDLL_OFFSET":   INI_ITEM_NUM_OPTIONAL_DDR + 5.1,
    "DDRC_DQS_SYNC_MODE":           INI_ITEM_NUM_OPTIONAL_DDR + 6,
    "DDRC_DQS_DLL_MODE":            INI_ITEM_NUM_OPTIONAL_DDR + 7,
    "DRAM_LINK_ECC_READ":           INI_ITEM_NUM_OPTIONAL_DDR + 8,
    "DRAM_LINK_ECC_WRITE":          INI_ITEM_NUM_OPTIONAL_DDR + 8.1,
    "DRAM_DQ_VREF":                 INI_ITEM_NUM_OPTIONAL_DDR + 9.0,
    "DRAM_CA_VREF":                 INI_ITEM_NUM_OPTIONAL_DDR + 9.1,
    "DRAM_DQ_ODT":                  INI_ITEM_NUM_OPTIONAL_DDR + 10.0,
    "DRAM_CA_ODT":                  INI_ITEM_NUM_OPTIONAL_DDR + 10.1,
    "DRAM_WCK_ODT":                 INI_ITEM_NUM_OPTIONAL_DDR + 11.0,
    "DRAM_NT_DQ_ODT":               INI_ITEM_NUM_OPTIONAL_DDR + 11.1,
    "DRAM_SOC_ODT":                 INI_ITEM_NUM_OPTIONAL_DDR + 12,

    "DDRC0_TERM":                   INI_ITEM_NUM_OPTIONAL_DDR + 13.0,
    "DDRC0_WORD0_DQ_TERM":          INI_ITEM_NUM_OPTIONAL_DDR + 13.01,
    "DDRC0_WORD1_DQ_TERM":          INI_ITEM_NUM_OPTIONAL_DDR + 13.02,
    "DDRC1_TERM":                   INI_ITEM_NUM_OPTIONAL_DDR + 13.1,
    "DDRC1_WORD0_DQ_TERM":          INI_ITEM_NUM_OPTIONAL_DDR + 13.11,
    "DDRC1_WORD1_DQ_TERM":          INI_ITEM_NUM_OPTIONAL_DDR + 13.12,

    "DDRC0_DDS":                    INI_ITEM_NUM_OPTIONAL_DDR + 14.0,
    "DDRC0_DQ_DDS":                 INI_ITEM_NUM_OPTIONAL_DDR + 14.01,
    "DDRC0_WORD0_DQ_DDS":           INI_ITEM_NUM_OPTIONAL_DDR + 14.011,
    "DDRC0_WORD1_DQ_DDS":           INI_ITEM_NUM_OPTIONAL_DDR + 14.012,
    "DDRC0_CA_DDS":                 INI_ITEM_NUM_OPTIONAL_DDR + 14.02,
    "DDRC0_WORD0_CA_DDS":           INI_ITEM_NUM_OPTIONAL_DDR + 14.021,
    "DDRC0_WORD1_CA_DDS":           INI_ITEM_NUM_OPTIONAL_DDR + 14.022,
    "DDRC1_DDS":                    INI_ITEM_NUM_OPTIONAL_DDR + 14.1,
    "DDRC1_DQ_DDS":                 INI_ITEM_NUM_OPTIONAL_DDR + 14.11,
    "DDRC1_WORD0_DQ_DDS":           INI_ITEM_NUM_OPTIONAL_DDR + 14.111,
    "DDRC1_WORD1_DQ_DDS":           INI_ITEM_NUM_OPTIONAL_DDR + 14.112,
    "DDRC1_CA_DDS":                 INI_ITEM_NUM_OPTIONAL_DDR + 14.12,
    "DDRC1_WORD0_CA_DDS":           INI_ITEM_NUM_OPTIONAL_DDR + 14.121,
    "DDRC1_WORD1_CA_DDS":           INI_ITEM_NUM_OPTIONAL_DDR + 14.122,

    "DDRC0_PDDS":                   INI_ITEM_NUM_OPTIONAL_DDR + 15.0,
    "DDRC0_DQ_PDDS":                INI_ITEM_NUM_OPTIONAL_DDR + 15.01,
    "DDRC0_WORD0_DQ_PDDS":          INI_ITEM_NUM_OPTIONAL_DDR + 15.011,
    "DDRC0_WORD1_DQ_PDDS":          INI_ITEM_NUM_OPTIONAL_DDR + 15.012,
    "DDRC0_CA_PDDS":                INI_ITEM_NUM_OPTIONAL_DDR + 15.02,
    "DDRC0_WORD0_CA_PDDS":          INI_ITEM_NUM_OPTIONAL_DDR + 15.021,
    "DDRC0_WORD1_CA_PDDS":          INI_ITEM_NUM_OPTIONAL_DDR + 15.022,
    "DDRC1_PDDS":                   INI_ITEM_NUM_OPTIONAL_DDR + 15.1,
    "DDRC1_DQ_PDDS":                INI_ITEM_NUM_OPTIONAL_DDR + 15.11,
    "DDRC1_WORD0_DQ_PDDS":          INI_ITEM_NUM_OPTIONAL_DDR + 15.111,
    "DDRC1_WORD1_DQ_PDDS":          INI_ITEM_NUM_OPTIONAL_DDR + 15.112,
    "DDRC1_CA_PDDS":                INI_ITEM_NUM_OPTIONAL_DDR + 15.12,
    "DDRC1_WORD0_CA_PDDS":          INI_ITEM_NUM_OPTIONAL_DDR + 15.121,
    "DDRC1_WORD1_CA_PDDS":          INI_ITEM_NUM_OPTIONAL_DDR + 15.122,

    "DDRC0_READ_DLL":               INI_ITEM_NUM_OPTIONAL_DDR + 16.0,
    "DDRC0_BYTE0_READ_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 16.01,
    "DDRC0_BYTE1_READ_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 16.02,
    "DDRC0_BYTE2_READ_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 16.03,
    "DDRC0_BYTE3_READ_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 16.04,
    "DDRC1_READ_DLL":               INI_ITEM_NUM_OPTIONAL_DDR + 16.1,
    "DDRC1_BYTE0_READ_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 16.11,
    "DDRC1_BYTE1_READ_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 16.12,
    "DDRC1_BYTE2_READ_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 16.13,
    "DDRC1_BYTE3_READ_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 16.14,

    "DDRC0_SYNC_DLL":               INI_ITEM_NUM_OPTIONAL_DDR + 17.0,
    "DDRC0_BYTE0_SYNC_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 17.01,
    "DDRC0_BYTE1_SYNC_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 17.02,
    "DDRC0_BYTE2_SYNC_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 17.03,
    "DDRC0_BYTE3_SYNC_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 17.04,
    "DDRC1_SYNC_DLL":               INI_ITEM_NUM_OPTIONAL_DDR + 17.1,
    "DDRC1_BYTE0_SYNC_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 17.11,
    "DDRC1_BYTE1_SYNC_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 17.12,
    "DDRC1_BYTE2_SYNC_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 17.13,
    "DDRC1_BYTE3_SYNC_DLL":         INI_ITEM_NUM_OPTIONAL_DDR + 17.14,

    "DDRC0_WRITE_DLL":              INI_ITEM_NUM_OPTIONAL_DDR + 18.0,
    "DDRC0_BYTE0_WRITE_DLL":        INI_ITEM_NUM_OPTIONAL_DDR + 18.01,
    "DDRC0_BYTE1_WRITE_DLL":        INI_ITEM_NUM_OPTIONAL_DDR + 18.02,
    "DDRC0_BYTE2_WRITE_DLL":        INI_ITEM_NUM_OPTIONAL_DDR + 18.03,
    "DDRC0_BYTE3_WRITE_DLL":        INI_ITEM_NUM_OPTIONAL_DDR + 18.04,
    "DDRC1_WRITE_DLL":              INI_ITEM_NUM_OPTIONAL_DDR + 18.1,
    "DDRC1_BYTE0_WRITE_DLL":        INI_ITEM_NUM_OPTIONAL_DDR + 18.11,
    "DDRC1_BYTE1_WRITE_DLL":        INI_ITEM_NUM_OPTIONAL_DDR + 18.12,
    "DDRC1_BYTE2_WRITE_DLL":        INI_ITEM_NUM_OPTIONAL_DDR + 18.13,
    "DDRC1_BYTE3_WRITE_DLL":        INI_ITEM_NUM_OPTIONAL_DDR + 18.14,

    "DDRC0_DQ_READ_DELAY":          INI_ITEM_NUM_OPTIONAL_DDR + 19.0,
    "DDRC0_BYTE0_DQ_READ_DELAY":    INI_ITEM_NUM_OPTIONAL_DDR + 19.01,
    "DDRC0_BYTE1_DQ_READ_DELAY":    INI_ITEM_NUM_OPTIONAL_DDR + 19.02,
    "DDRC0_BYTE2_DQ_READ_DELAY":    INI_ITEM_NUM_OPTIONAL_DDR + 19.03,
    "DDRC0_BYTE3_DQ_READ_DELAY":    INI_ITEM_NUM_OPTIONAL_DDR + 19.04,
    "DDRC1_DQ_READ_DELAY":          INI_ITEM_NUM_OPTIONAL_DDR + 19.1,
    "DDRC1_BYTE0_DQ_READ_DELAY":    INI_ITEM_NUM_OPTIONAL_DDR + 19.11,
    "DDRC1_BYTE1_DQ_READ_DELAY":    INI_ITEM_NUM_OPTIONAL_DDR + 19.12,
    "DDRC1_BYTE2_DQ_READ_DELAY":    INI_ITEM_NUM_OPTIONAL_DDR + 19.13,
    "DDRC1_BYTE3_DQ_READ_DELAY":    INI_ITEM_NUM_OPTIONAL_DDR + 19.14,

    "DDRC0_DQ_WRITE_DELAY":         INI_ITEM_NUM_OPTIONAL_DDR + 20.0,
    "DDRC0_BYTE0_DQ_WRITE_DELAY":   INI_ITEM_NUM_OPTIONAL_DDR + 20.01,
    "DDRC0_BYTE1_DQ_WRITE_DELAY":   INI_ITEM_NUM_OPTIONAL_DDR + 20.02,
    "DDRC0_BYTE2_DQ_WRITE_DELAY":   INI_ITEM_NUM_OPTIONAL_DDR + 20.03,
    "DDRC0_BYTE3_DQ_WRITE_DELAY":   INI_ITEM_NUM_OPTIONAL_DDR + 20.04,
    "DDRC1_DQ_WRITE_DELAY":         INI_ITEM_NUM_OPTIONAL_DDR + 20.1,
    "DDRC1_BYTE0_DQ_WRITE_DELAY":   INI_ITEM_NUM_OPTIONAL_DDR + 20.11,
    "DDRC1_BYTE1_DQ_WRITE_DELAY":   INI_ITEM_NUM_OPTIONAL_DDR + 20.12,
    "DDRC1_BYTE2_DQ_WRITE_DELAY":   INI_ITEM_NUM_OPTIONAL_DDR + 20.13,
    "DDRC1_BYTE3_DQ_WRITE_DELAY":   INI_ITEM_NUM_OPTIONAL_DDR + 20.14,

    "DDRC_WCK_WRITE_DELAY":         INI_ITEM_NUM_OPTIONAL_DDR + 21,
    "DDRC0_WCK_WRITE_DELAY":        INI_ITEM_NUM_OPTIONAL_DDR + 21.1,
    "DDRC1_WCK_WRITE_DELAY":        INI_ITEM_NUM_OPTIONAL_DDR + 21.2,

    "DDRC0_DQ_CA_VREF":             INI_ITEM_NUM_OPTIONAL_DDR + 22.0,
    "DDRC1_DQ_CA_VREF":             INI_ITEM_NUM_OPTIONAL_DDR + 22.1,

    "DDRC_DQS_VREF":                INI_ITEM_NUM_OPTIONAL_DDR + 23,
    "DRAM_ODTE_CK":                 INI_ITEM_NUM_OPTIONAL_DDR + 24.0,
    "DRAM_ODTE_CS":                 INI_ITEM_NUM_OPTIONAL_DDR + 24.1,
    "DRAM_ODTD_CA":                 INI_ITEM_NUM_OPTIONAL_DDR + 25,
    "DRAM_PU_CAL":                  INI_ITEM_NUM_OPTIONAL_DDR + 26,

    "DDRC0_CA_DEALY":               INI_ITEM_NUM_OPTIONAL_DDR + 27.0,
    "DDRC0_WORD0_CA_DELAY":         INI_ITEM_NUM_OPTIONAL_DDR + 27.01,
    "DDRC0_WORD1_CA_DELAY":         INI_ITEM_NUM_OPTIONAL_DDR + 27.02,
    "DDRC1_CA_DEALY":               INI_ITEM_NUM_OPTIONAL_DDR + 27.1,
    "DDRC1_WORD0_CA_DELAY":         INI_ITEM_NUM_OPTIONAL_DDR + 27.11,
    "DDRC1_WORD1_CA_DELAY":         INI_ITEM_NUM_OPTIONAL_DDR + 27.12,

    "DDRC0_CK_DEALY":               INI_ITEM_NUM_OPTIONAL_DDR + 28.0,
    "DDRC0_WORD0_CK_DELAY":         INI_ITEM_NUM_OPTIONAL_DDR + 28.01,
    "DDRC0_WORD1_CA_DELAY":         INI_ITEM_NUM_OPTIONAL_DDR + 28.02,
    "DDRC1_CK_DEALY":               INI_ITEM_NUM_OPTIONAL_DDR + 28.1,
    "DDRC1_WORD0_CK_DELAY":         INI_ITEM_NUM_OPTIONAL_DDR + 28.11,
    "DDRC1_WORD1_CA_DELAY":         INI_ITEM_NUM_OPTIONAL_DDR + 28.12,
})

DRIVE_STRENGTH = ("Disable", "RZQ/1", "RZQ/2", "RZQ/3", "RZQ/4", "RZQ/5", "RZQ/6", "RZQ/7")

DRAM_DS = {
    "DDR4":   ("RFU", "RZQ/5", "RZQ/6", "RZQ/7"),
    "LPDDR4": ("RFU", "RZQ/1", "RZQ/2", "RZQ/3", "RZQ/4", "RZQ/5", "RZQ/6"),
    "LPDDR5": ("RFU", "RZQ/1", "RZQ/2", "RZQ/3", "RZQ/4", "RZQ/5", "RZQ/6")
}

DRAM_ODT = {
    "DDR4":   ("Disable", "RZQ/1", "RZQ/2", "RZQ/3", "RZQ/4", "RZQ/5", "RZQ/6", "RZQ/7"),
    "LPDDR4": ("Disable", "RZQ/1", "RZQ/2", "RZQ/3", "RZQ/4", "RZQ/5", "RZQ/6"),
    "LPDDR5": ("Disable", "RZQ/1", "RZQ/2", "RZQ/3", "RZQ/4", "RZQ/5", "RZQ/6")
}

def calculate_PLL_freq(pll_raw, name):
    reg = Register(name, bitarray.util.int2ba(pll_raw, 32, 'little'), None, True,
        Bitfield("Intp", (31, 24), lambda x: x+1),
        Bitfield("Sout", (19, 16), lambda x: x+1),
        Bitfield("Sdiv", (15, 12), lambda x: x+1)
    )

    freq = int(24 * reg.bitfield["Intp"].mapped_value() * reg.bitfield["Sdiv"].mapped_value() / reg.bitfield["Sout"].mapped_value())
    return freq

def all_equal(iterable):
    g = groupby(iterable)
    return next(g, True) and not next(g, False)

def replace_from_list(sym, key_list, replace_id):
    for k in key_list:
        key_prefix = k[:-1]
        for idx in [pattern.start() for pattern in re.finditer(key_prefix, sym)]:
            idx_next = idx + len(key_prefix)
            #print("idx:%d, len(key_prefix):%d, idx_next:%d sym:%s" % (idx, len(key_prefix), idx_next, sym))
            if sym[idx_next].isdigit() or sym[idx_next] == 'X':
                replace_str = "%s%d" % (key_prefix, replace_id)
                new_str = "%s%s%s" % (sym[:idx], replace_str, sym[idx_next+1:])
                #print("sym -> [%s] (%d)" % (new_str, replace_id))
                return new_str

    print("sym [%s] replace failed..." % sym)
    return sym

def collect_from_all_byte(sym, d, platform, delete=True):
    regs = []
    k = sym
    for die in range(DIE_COUNT[platform]):
        k = replace_from_list(k, ["DieX", "DX"], die)
        for byte in range(BYTE_COUNT[platform]):
            k = replace_from_list(k, ["ByteX", "BX"], byte)

            if k in d:
                reg = (k, bitarray.util.int2ba(d[k][0], 32, 'little'))
                regs.append(reg)
                if delete: del d[k]

    if not all_equal([r for (name, r) in regs]):
        print("[Warning] %s is not identical on all die/bytes!" % sym)
        for i_die, die in enumerate(range(DIE_COUNT[platform])):
            for i_byte, byte in enumerate(range(BYTE_COUNT[platform])):
                idx = die*BYTE_COUNT[platform] + byte
                v = bitarray.util.ba2int(regs[idx][1])
                if (i_die == 0) and (i_byte == 0):
                    print("          Use %s (0x%08x) as default! (Info could be lost in translations...)" % (regs[idx][0], v))
                    print("          (D%dB%d: 0x%08x" % (die, byte, v), end='')
                else:
                    print(", D%dB%d: 0x%08x" % (die, byte, v), end='')
        print(")")

    # [TODO] Return die0 byte0 only?
    return regs[0]

def collect_from_all_ch(sym, d, platform, delete=True):
    regs = []
    k = sym
    for ch in range(CH_COUNT[platform]):
        k = replace_from_list(k, ["ChX", "CX"], ch)
        for die in range(DIE_COUNT[platform]):
            k = replace_from_list(k, ["DieX", "DX"], die)

            if k in d:
                reg = (k, bitarray.util.int2ba(d[k][0], 32, 'little'))
                regs.append(reg)
                if delete: del d[k]

    if not all_equal([r for (name, r) in regs]):
        print("[Warning] %s is not identical on all channel/dies!" % sym)
        for i_ch, ch in enumerate(range(CH_COUNT[platform])):
            for i_die, die in enumerate(range(DIE_COUNT[platform])):
                idx = ch*DIE_COUNT[platform] + die
                v = bitarray.util.ba2int(regs[idx][1])
                if (i_ch == 0) and (i_die == 0):
                    print("          Use %s (0x%08x) as default! (Info could be lost in translations...)" % (regs[idx][0], v))
                    print("          (C%dD%d: 0x%08x" % (ch, die, v), end='')
                else:
                    print(", C%dD%d: 0x%08x" % (ch, die, v), end='')
        print(")")

    # [TODO] Return die0 only?
    return regs[0]

def collect_from_all_die(sym, d, platform, delete=True):
    regs = []
    k = sym
    for die in range(DIE_COUNT[platform]):
        k = replace_from_list(k, ["DieX", "DX"], die)

        if k in d:
            reg = (k, bitarray.util.int2ba(d[k][0], 32, 'little'))
            regs.append(reg)
            if delete: del d[k]

    if not all_equal([r for (name, r) in regs]):
        print("[Warning] %s is not identical on all die!" % sym)
        for i, die in enumerate(range(DIE_COUNT[platform])):
            v = bitarray.util.ba2int(regs[die][1])
            if (i == 0):
                print("          Use %s (0x%08x) as default! (Info could be lost in translations...)" % (regs[die][0], v))
                print("          (D%d: 0x%08x" % (die, v), end='')
            else:
               print(", D%d: 0x%08x" % (die, v), end='')
        print(")")

    # [TODO] Return die0 only?
    return regs[0]

def collect_from_all_host(sym, d, platform, diff, delete=True):
    regs = []
    k = sym
    for host in range(HOST_COUNT[platform]):
        k = replace_from_list(k, ["HostX", "HX", "DdrcX"], host)
        #print("@collect_from_all_host - %s (%d)" % (k, host))

        if (diff == "byte"):
            reg = collect_from_all_byte(k, d, platform, delete)
        elif (diff == "ch"):
            reg = collect_from_all_ch(k, d, platform, delete)
        elif (diff == "die"):
            reg = collect_from_all_die(k, d, platform, delete)
        else:
            if k in d:
                reg = (k, bitarray.util.int2ba(d[k][0], 32, 'little'))
                if delete: del d[k]

        regs.append(reg)

    if not all_equal([r for (name, r) in regs]):
        print("[Warning] %s is not identical on all hosts!" % sym)
        for i, host in enumerate(range(HOST_COUNT[platform])):
            v = bitarray.util.ba2int(regs[host][1])
            if (i == 0):
                print("          Use %s (0x%08x) as default! (Info could be lost in translations...)" % (regs[host][0], v))
                print("          (H%d: 0x%08x" % (host, v), end='')
            else:
                print(", H%d: 0x%08x)" % (host, v), end='')
        print(")")

    # [TODO] Return host0 only?
    return regs[0]

def collect_from_target(sym, d, target, delete=True):
    # target should be in "HxDxCxBx" format
    k = sym

    if target:
        # dissect target string to find all index of 'number'
        pos = []
        for i, c in enumerate(target):
            if c.isdigit():
                pos.append(i)

        # group sequential number index and keep in pos_range
        pos_range = []
        for key, g in groupby(enumerate(pos), lambda x: x[0]-x[1]):
            group = map(itemgetter(1), g)
            group = list(map(int,group))
            pos_range.append((group[0], group[-1]))

        for (idx_lo, idx_hi) in pos_range:
            target_id = int(target[idx_lo:idx_hi+1])
            #print("%s (%d, %d) %d - %s" % (target, idx_lo, idx_hi, target_id, k))
            if target[idx_lo-1] == 'H':
                k = replace_from_list(k, ["HostX", "HX", "DdrcX"], target_id)
            elif target[idx_lo-1] == 'D':
                k = replace_from_list(k, ["DieX", "DX"], target_id)
            elif target[idx_lo-1] == 'C':
                k = replace_from_list(k, ["ChX", "CX"], target_id)
            elif target[idx_lo-1] == 'B':
                k = replace_from_list(k, ["ByteX", "BX"], target_id)

    if k in d:
        reg = (k, bitarray.util.int2ba(d[k][0], 32, 'little'))
        if delete: del d[k]
        return reg
    else:
        print("%s not found in dict (already deleted?)" % k)
        return None

def collect_multiple(sym, d, platform, diff, delete=True):
    regs = []
    for k in sym:
        if diff in ["byte", "ch", "die", "host"]:
            reg = collect_from_all_host(k, d, platform, diff, delete)
        else:
            if k in d:
                reg = (k, bitarray.util.int2ba(d[k][0], 32, 'little'))
                if delete: del d[k]

        regs.append(reg)

    if not all_equal([r for (name, r) in regs]):
        print("[Warning] Register value not identical!")
        for i, (name, r) in enumerate(regs):
            v = bitarray.util.ba2int(r)
            if (i == 0):
                print("          Use %s (0x%08x) as default! (Info could be lost in translations...)" % (name, v))
                print("          (%s: 0x%08x" % (name, v), end='')
            else:
                print(", %s: 0x%08x" % (name, v), end='')
        print(")")

    # [TODO] Return regs[0] only?
    return regs[0]

