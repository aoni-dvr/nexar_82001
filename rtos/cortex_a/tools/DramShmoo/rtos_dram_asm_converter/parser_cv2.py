import bitarray
import bitarray.util

import plat

from util import Bitfield, Register

__version__ = "v0.8"

DRAM_TYPE_TABLE = {1: "DDR3", 4: "MDDR3", 6: "LPDDR4"}

ASM_TEMPLATE = {"LPDDR4": "/lib/CV2_LPDDR4_temp.asm"}

H_TO_ASM_MAP = {
    "PLL_CORTEX_CTRL_REG_VAL": ["PLL_CortexCtrlParam"],

    "PLL_DDR_CTRL_REG_VAL": ["PLL_DdrCtrlParam"],
    "PLL_DDR_CTRL2_REG_VAL": ["PLL_DdrCtrl2Param"],
    "PLL_DDR_CTRL3_REG_VAL": ["PLL_DdrCtrl3Param"],

    "DRAM_MODE_REG_VAL": ["DRAMC_ModeParam"],

    "DDRIO_DLL0_DDRC0_VAL": ["Ddrc0_DLL_Setting0Param"],
    "DDRIO_DLL1_DDRC0_VAL": ["Ddrc0_DLL_Setting1Param"],
    "DDRIO_DLL2_DDRC0_VAL": ["Ddrc0_DLL_Setting2Param"],
    "DDRIO_DLL3_DDRC0_VAL": ["Ddrc0_DLL_Setting3Param"],
    "DDRIO_DLL0_DDRC1_VAL": ["Ddrc1_DLL_Setting0Param"],
    "DDRIO_DLL1_DDRC1_VAL": ["Ddrc1_DLL_Setting1Param"],
    "DDRIO_DLL2_DDRC1_VAL": ["Ddrc1_DLL_Setting2Param"],
    "DDRIO_DLL3_DDRC1_VAL": ["Ddrc1_DLL_Setting3Param"],
    # n-to-1 mapping
    # ---------- DDRC0 ----------
    "DDRIO_DLL_CTRL_SEL0_DDRC0_VAL": ["DLL_CtrlSel0Param"],
    "DDRIO_DLL_CTRL_SEL1_DDRC0_VAL": ["DLL_CtrlSel1Param"],
    "DDRIO_DLL_CTRL_SEL2_DDRC0_VAL": ["DLL_CtrlSel2Param"],
    "DDRIO_DLL_CTRL_SEL3_DDRC0_VAL": ["DLL_CtrlSel3Param"],
    "DDRIO_DLL_CTRL_MISC_DDRC0_VAL": ["DLL_CtrlSelMiscParam"],
    "DDRIO_DLL_SYNC_CTRL_SEL0_DDRC0_VAL": ["DLL_CtrlSyncTrackParam"],
    # ---------- DDRC1 ----------
    "DDRIO_DLL_CTRL_SEL0_DDRC1_VAL": ["DLL_CtrlSel0Param"],
    "DDRIO_DLL_CTRL_SEL1_DDRC1_VAL": ["DLL_CtrlSel1Param"],
    "DDRIO_DLL_CTRL_SEL2_DDRC1_VAL": ["DLL_CtrlSel2Param"],
    "DDRIO_DLL_CTRL_SEL3_DDRC1_VAL": ["DLL_CtrlSel3Param"],
    "DDRIO_DLL_CTRL_MISC_DDRC1_VAL": ["DLL_CtrlSelMiscParam"],
    "DDRIO_DLL_SYNC_CTRL_SEL0_DDRC1_VAL": ["DLL_CtrlSyncTrackParam"],

    # n-to-1 mapping
    # ---------- DDRC0 ----------
    "DRAM_CONTROL_DDRC0_VAL": ["DRAM_CtrlParam"],
    "DRAM_CONFIG_DDRC0_VAL": ["DRAM_ConfigParam"],
    "DRAM_TIMING1_DDRC0_VAL": ["DRAM_Timing1Param"],
    "DRAM_TIMING2_DDRC0_VAL": ["DRAM_Timing2Param"],
    "DRAM_TIMING3_DDRC0_VAL": ["DRAM_Timing3Param"],
    "DRAM_TIMING4_DDRC0_VAL": ["DRAM_Timing4Param"],
    "DRAM_TIMING5_LP4TRAIN_DDRC0_VAL": ["DRAM_Timing5Lp4TrainingParam"],
    "DRAM_BYTE_MAP_DDRC0_VAL": ["DRAM_ByteMapParam"],
    # ---------- DDRC1 ----------
    "DRAM_CONTROL_DDRC1_VAL": ["DRAM_CtrlParam"],
    "DRAM_CONFIG_DDRC1_VAL": ["DRAM_ConfigParam"],
    "DRAM_TIMING1_DDRC1_VAL": ["DRAM_Timing1Param"],
    "DRAM_TIMING2_DDRC1_VAL": ["DRAM_Timing2Param"],
    "DRAM_TIMING3_DDRC1_VAL": ["DRAM_Timing3Param"],
    "DRAM_TIMING4_DDRC1_VAL": ["DRAM_Timing4Param"],
    "DRAM_TIMING5_LP4TRAIN_DDRC1_VAL": ["DRAM_Timing5Lp4TrainingParam"],
    "DRAM_BYTE_MAP_DDRC1_VAL": ["DRAM_ByteMapParam"],

    # 1-to-1 mapping
    # ---------- DDRC0 ----------
    "DRAM_PAD_TERM_DDRC0_VAL": ["Ddrc0_DRAM_PadTermParam"],
    "DRAM_PAD_TERM2_DDRC0_VAL": ["Ddrc0_DRAM_PadTerm2Param"],
    "DRAM_PAD_TERM3_DDRC0_VAL": ["Ddrc0_DRAM_PadTerm3Param"],
    "LPDDR4_DQ_CA_VREF_DDRC0_VAL": ["Ddrc0_DRAM_lpddr4DqCaVrefParam"],
    "LPDDR4_DQ_WRITE_DLY_DDRC0_VAL": ["Ddrc0_DRAM_lpddr4DqWriteDlyParam"],
    "LPDDR4_DQ_READ_DLY_DDRC0_VAL": ["Ddrc0_DRAM_lpddr4DqReadDlyParam"],
    "LPDDR4_DQS_WRITE_DLY_DDRC0_VAL": ["Ddrc0_DRAM_lpddr4DqsWriteDlyParam"],
    "LPDDR4_TRAIN_MPC_RDLY_DDRC0_VAL": ["Ddrc0_DRAM_lpddr4TrainMpcRdlyParam"],
    # ---------- DDRC1 ----------
    "DRAM_PAD_TERM_DDRC1_VAL": ["Ddrc1_DRAM_PadTermParam"],
    "DRAM_PAD_TERM2_DDRC1_VAL": ["Ddrc1_DRAM_PadTerm2Param"],
    "DRAM_PAD_TERM3_DDRC1_VAL": ["Ddrc1_DRAM_PadTerm3Param"],
    "LPDDR4_DQ_CA_VREF_DDRC1_VAL": ["Ddrc1_DRAM_lpddr4DqCaVrefParam"],
    "LPDDR4_DQ_WRITE_DLY_DDRC1_VAL": ["Ddrc1_DRAM_lpddr4DqWriteDlyParam"],
    "LPDDR4_DQ_READ_DLY_DDRC1_VAL": ["Ddrc1_DRAM_lpddr4DqReadDlyParam"],
    "LPDDR4_DQS_WRITE_DLY_DDRC1_VAL": ["Ddrc1_DRAM_lpddr4DqsWriteDlyParam"],
    "LPDDR4_TRAIN_MPC_RDLY_DDRC1_VAL": ["Ddrc1_DRAM_lpddr4TrainMpcRdlyParam"],

    # n-to-1 mapping
    # ---------- DDRC0 ----------
    "DRAM_DQS_SYNC_DDRC0_VAL": ["DRAM_DqsSyncPreRttParam", "DRAM_DqsSyncParam"],
    "DRAM_ZQ_CALIB_DDRC0_VAL": ["DRAM_ZqCalibParam"],
    "DRAM_DLL_CALIB_DDRC0_VAL": ["DRAM_DllCalibParam"],
    "DRAM_POWER_DOWN_DDRC0_VAL": ["DRAM_PowerDownParam"],
    # ---------- DDRC1 ----------
    "DRAM_DQS_SYNC_DDRC1_VAL": ["DRAM_DqsSyncPreRttParam", "DRAM_DqsSyncParam"],
    "DRAM_ZQ_CALIB_DDRC1_VAL": ["DRAM_ZqCalibParam"],
    "DRAM_DLL_CALIB_DDRC1_VAL": ["DRAM_DllCalibParam"],
    "DRAM_POWER_DOWN_DDRC1_VAL": ["DRAM_PowerDownParam"],

    # 1-to-n mapping
    # .h actually has seperate parts of MRs for each ddrc, but with same name...
    # This would be handled in rtos_dram_asm_converter.py by adding DUPX for each ddrc.
    "LPDDR4_MR1_VAL": ["LPDDR4_ModeReg1ParamFsp0", "LPDDR4_ModeReg1ParamFsp1"],
    "LPDDR4_MR2_VAL": ["LPDDR4_ModeReg2ParamFsp0", "LPDDR4_ModeReg2ParamFsp1"],
    "LPDDR4_MR3_VAL": ["Ddrc0_LPDDR4_ModeReg3ParamFsp0", "Ddrc0_LPDDR4_ModeReg3ParamFsp1"],
    "LPDDR4_MR11_VAL": ["Ddrc0_LPDDR4_ModeReg11ParamFsp0", "Ddrc0_LPDDR4_ModeReg11ParamFsp1"],
    "LPDDR4_MR12_VAL": ["Ddrc0_LPDDR4_ModeReg12ParamFsp0", "Ddrc0_LPDDR4_ModeReg12ParamFsp1"],
    "LPDDR4_MR13_VAL": ["LPDDR4_ModeReg13ParamFsp0"],
    "LPDDR4_MR14_VAL": ["Ddrc0_LPDDR4_ModeReg14ParamFsp0", "Ddrc0_LPDDR4_ModeReg14ParamFsp1"],
    "LPDDR4_MR22_VAL": ["Ddrc0_LPDDR4_ModeReg22ParamFsp0", "Ddrc0_LPDDR4_ModeReg22ParamFsp1"],
}

def version():
    return "parser_cv2: %s" % __version__

def get_dram_type(d):
    regs = []
    k_list = ["DRAM_ConfigParam", "DRAM_CONFIG_DDRC0_VAL", "DRAM_CONFIG_DDRC1_VAL"]
    for k, v in d.items():
        if k in k_list:
            reg = Register(k, bitarray.util.int2ba(d[k][0], 32, 'little'), None, False,
                Bitfield("DRAM_TYPE", (29, 27), lambda x: DRAM_TYPE_TABLE[x]),
            )
            regs.append(reg)

    if not plat.all_equal([r.value() for r in regs]):
        print("[Warning] Register value is not all identical!", end='')
        for r in regs:
            print(" (%s: 0x%08x)" % (r.name, r.value()), end='')
        print()

    return regs[0].bitfield["DRAM_TYPE"].mapped_value()

def get_byte_map(raw):
    return "B%dB%dB%dB%d" % ((raw>>6)&0x3, (raw>>4)&0x3, (raw>>2)&0x3, raw&0x3)

def get_en_val(raw, en_bit, val_hi_bit, val_lo_bit):
    # Some DRAM parameter hide in bitfields with enable bit + value bitfield.
    # ex. dq pad term: bit[7:5] (val) + bit[0] (en)
    # This function check if en bit is set and extract the val
    if ((raw >> en_bit) & 0x1 == 0x1):
        val = (raw >> val_lo_bit) & (pow(2, (val_hi_bit - val_lo_bit + 1)) - 1)
        print("[DBG - get_en_val] en[%d], val[%d:%d]=%d(%s), mask=%s" % (en_bit, val_hi_bit, val_lo_bit, val, bin(val), hex(pow(2, (val_hi_bit - val_lo_bit + 1)) - 1)))
        return val
    else:
        print("[DBG - get_en_val] en[%d], val[%d:%d]" % (en_bit, val_hi_bit, val_lo_bit))
        return 0

def parse(out, d, platform):
    # ========== Search for DRAM_BUS_WIDTH ==========
    k = "DRAMC_ModeParam"
    if k in d:
        dram_bus_width_table = {0: "16 bits", 1: "32 bits", 2: "64 bits"}
        reg = Register(k, bitarray.util.int2ba(d[k][0], 32, 'little'), out, True,
            Bitfield("DRAM_BUS_WIDTH", (4, 3), lambda x: dram_bus_width_table[x])
        )
        bus_width = int(reg.bitfield["DRAM_BUS_WIDTH"].mapped_value().split()[0])
        del d[k]

    # ========== Search for DRAM TYPE/SIZE/DUAL_RANK ==========
    k = "DRAM_ConfigParam"
    if k in d:
        dram_dual_rank_table = {0: "Disable", 1: "Enable"}
        dram_die_size_table = {
            0: "256 Mbits",
            1: "512 Mbits",
            2: "1 Gbits",
            3: "2 Gbits",
            4: "4 Gbits",
            5: "8 Gbits",
            6: "16 Gbits"
        }
        dram_size_table = {k: "%d %s" % (int(v.split()[0])*4, v.split()[1])
            for k, v in dram_die_size_table.items()}

        # dram type = reg[2:0]
        # dram dual rank = reg[26]
        # dram size (per ch/die) = reg[5:2] (only 1 die per channel)
        reg = Register(k, bitarray.util.int2ba(d[k][0], 32, 'little'), out, True,
            Bitfield("DRAM_TYPE", (29, 27), lambda x: DRAM_TYPE_TABLE[x]),
            Bitfield("DRAM_DUAL_RANK", (26, 26), lambda x: dram_dual_rank_table[x]),
            Bitfield("DRAM_DIE_SIZE", (7, 5), lambda x: dram_die_size_table[x])
        )
        dram_type = reg.bitfield["DRAM_TYPE"].mapped_value()
        dual_rank = reg.bitfield["DRAM_DUAL_RANK"].value()
        dram_size_per_die, postfix = reg.bitfield["DRAM_DIE_SIZE"].mapped_value().split()
        dram_size_per_die = int(dram_size_per_die) if (postfix[0] == 'G') else (int(dram_size_per_die) / 1024)
        dram_size = dram_size_per_die * (2 if dual_rank else 1) * (bus_width / 16)
        out.update({"DRAM_SIZE": "%d %s" % (int(dram_size), postfix)})
        del out["DRAM_DIE_SIZE"]
        del d[k]
        #print("dram_size=%d, dram_size_per_die=%d, dual_rank=%d, bus_width=%d" % (dram_size, dram_size_per_die, dual_rank, bus_width))
        print("[Info] DRAM type: %s" % dram_type)

    # ========== Find DRAM freq ==========
    k = "PLL_DdrCtrlParam"
    freq = plat.calculate_PLL_freq(d[k][0], k)
    if (dram_type == "LPDDR4"):
        freq = freq / 2 # LPDDR4 clock is divided by 2 in PLL module
    out.update({"DRAM_FREQUENCY": "%d MHz" % freq})

    # ========== Find dq r/w dly ==========
    for host in range(plat.HOST_COUNT[platform]):
        k = "Ddrc%d_DRAM_lpddr4DqReadDlyParam" % host
        reg_name, reg_raw = plat.collect_from_target(k, d, None, True)
        reg = Register(reg_name, reg_raw, out, True,
            #Bitfield("DDRC%d_DQ_READ_DELAY" % host, (4, 0), lambda x: (x >> 1) & 0xf if (x & 0x1 == 1) else 0)
            Bitfield("DDRC%d_DQ_READ_DELAY" % host, (4, 0), None)   # bstiniparser would take care of en bit
        )

        k = "Ddrc%d_DRAM_lpddr4DqWriteDlyParam" % host
        reg_name, reg_raw = plat.collect_from_target(k, d, None, True)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DDRC%d_DQ_WRITE_DELAY" % host, (6, 0), None)
        )

    # ========== Find dq vref ==========
    for host in range(plat.HOST_COUNT[platform]):
        k = "Ddrc%d_DRAM_lpddr4DqCaVrefParam" % host
        reg_name, reg_raw = plat.collect_from_target(k, d, None, True)
        reg = Register(reg_name, reg_raw, out, True,
            #Bitfield("DDRC_DQ_VREF", (27, 21), None),
            #Bitfield("DDRC_CA_VREF", (14, 10), None),
            Bitfield("DDRC%d_DQ_CA_VREF" % host, (27, 21), None) # use only DQ ch B
        )

    # ========== Find DLL ==========
    for host in range(plat.HOST_COUNT[platform]):
        k = "Ddrc%d_DLL_Setting0Param" % host
        reg_name, reg_raw = plat.collect_from_target(k, d, None, True)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DDRC%d_READ_DLL" % host, (5, 0), hex)
        )

        k = "Ddrc%d_DLL_Setting1Param" % host
        reg_name, reg_raw = plat.collect_from_target(k, d, None, True)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DDRC%d_SYNC_DLL" % host, (5, 0), hex)
        )

        k = "Ddrc%d_DLL_Setting2Param" % host
        reg_name, reg_raw = plat.collect_from_target(k, d, None, True)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DDRC%d_WRITE_DLL" % host, (5, 0), hex)
        )

    # ========== Find TERM/DDS/PDDS ==========
    for host in range(plat.HOST_COUNT[platform]):
        dq_term = {}
        dq_dds = {}
        dq_pdds = {}
        dq_upper = "DQ[31:16]"
        dq_lower = "DQ[15:0]"
        k = "Ddrc%d_DRAM_PadTermParam" % host
        reg_name, reg_raw = plat.collect_from_target(k, d, None, True)
        reg = Register(reg_name, reg_raw, None, True,
            Bitfield("DDRC%d_NOTERM" % host, (0, 0), None),
            Bitfield("DDRC%d_TERM" % host, (7, 5), None),
            Bitfield("DDRC%d_DDS" % host, (4, 2), None)
        )
        dq_noterm = reg.bitfield["DDRC%d_NOTERM" % host].value()
        dq_term_val = reg.bitfield["DDRC%d_TERM" % host].value()
        dq_dds_val = reg.bitfield["DDRC%d_DDS" % host].value()
        dq_term.update({dq_upper: 0 if dq_noterm else dq_term_val})     # DQ[31:16] of TERM
        dq_dds.update({dq_upper: dq_dds_val})                           # DQ[31:16] of DDS

        k = "Ddrc%d_DRAM_PadTerm2Param" % host
        reg_name, reg_raw = plat.collect_from_target(k, d, None, True)
        reg = Register(reg_name, reg_raw, None, True,
            Bitfield("DDRC%d_PDDS" % host, (10, 8), None)
        )
        dq_pdds_val = reg.bitfield["DDRC%d_PDDS" % host].value()
        dq_pdds.update({dq_upper: dq_pdds_val})                         # DQ[31:16] of PDDS

        k = "Ddrc%d_DRAM_PadTerm3Param" % host
        reg_name, reg_raw = plat.collect_from_target(k, d, None, True)
        reg = Register(reg_name, reg_raw, None, True,
            Bitfield("DDRC%d_NOTERM" % host, (15, 15), None),
            Bitfield("DDRC%d_TERM" % host, (19, 17), None),
            Bitfield("DDRC%d_DDS" % host, (14, 12), None),
            Bitfield("DDRC%d_PDDS" % host, (22, 20), None)
        )
        dq_noterm = reg.bitfield["DDRC%d_NOTERM" % host].value()
        dq_term_val = reg.bitfield["DDRC%d_TERM" % host].value()
        dq_dds_val = reg.bitfield["DDRC%d_DDS" % host].value()
        dq_pdds_val = reg.bitfield["DDRC%d_PDDS" % host].value()
        dq_term.update({dq_upper: 0 if dq_noterm else dq_term_val})     # DQ[15:0] of TERM
        dq_dds.update({dq_lower: dq_dds_val})                           # DQ[15:0] of DDS
        dq_pdds.update({dq_lower: dq_pdds_val})                         # DQ[15:0] of PDDS

        if not plat.all_equal([r for r in dq_term.values()]):
            print("[Warning] DDRC%d_DQ_TERM is not identical on all DQ! " % host, end='')
            for r, v in dq_term.items():
                print("%s:%d " % (r, v), end='')
            print()

        if not plat.all_equal([r for r in dq_dds.values()]):
            print("[Warning] DDRC%d_DQ_DDS is not identical on all DQ! " % host, end='')
            for r, v in dq_dds.items():
                print("%s:%d " % (r, v), end='')
            print()

        if not plat.all_equal([r for r in dq_pdds.values()]):
            print("[Warning] DDRC%d_DQ_PDDS is not identical on all DQ! " % host, end='')
            for r, v in dq_pdds.items():
                print("%s:%d " % (r, v), end='')
            print()

        out.update({"DDRC%d_TERM" % host: "%s" % plat.DRIVE_STRENGTH[dq_term[dq_upper]]})
        out.update({"DDRC%d_DDS" % host: "%s" % plat.DRIVE_STRENGTH[dq_dds[dq_upper]]})
        out.update({"DDRC%d_PDDS" % host: "%s" % plat.DRIVE_STRENGTH[dq_pdds[dq_upper]]})



    if (dram_type == "LPDDR4"):
        # ========== Find DRAM_DS ==========
        k = "DdrcX_LPDDR4_ModeReg3ParamFsp0"
        reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, None)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DRAM_DS", (5, 3), lambda x: plat.DRAM_DS[dram_type][x])
        )

        # ========== Find DRAM_ODT ==========
        k = "DdrcX_LPDDR4_ModeReg11ParamFsp0"
        reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, None)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DRAM_DQ_ODT", (2, 0), lambda x: plat.DRAM_ODT[dram_type][x]),
            Bitfield("DRAM_CA_ODT", (6, 4), lambda x: plat.DRAM_ODT[dram_type][x])
        )

        # ========== Find DRAM_CA_VREF ==========
        k = "DdrcX_LPDDR4_ModeReg12ParamFsp0"
        reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, None)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DRAM_CA_VREF", (5, 0), None)
        )

        # ========== Find DRAM_DQ_VREF ==========
        k = "DdrcX_LPDDR4_ModeReg14ParamFsp0"
        reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, None)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DRAM_DQ_VREF", (5, 0), None)
        )

        # ========== Find DRAM_SOC_ODT ==========
        k = "DdrcX_LPDDR4_ModeReg22ParamFsp0"
        reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, None)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DRAM_SOC_ODT", (2, 0), lambda x: plat.DRAM_ODT[dram_type][x])
        )

    # ========== Find DDRC_BYTE_MAP ==========
    k = "DRAM_ByteMapParam"
    reg_name, reg_raw = plat.collect_from_target(k, d, None, True)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DDRC_BYTE_MAP", (7, 0), lambda x: get_byte_map(x))
    )

    # ========== Extra ==========
    out.update({"CORE_FREQUENCY": "456 MHz"})
    out.update({"IDSP_FREQUENCY": "456 MHz"})
    out.update({"VISION_FREQUENCY": "504 MHz"})
    out.update({"FEX_FREQUENCY": "504 MHz"})
    out.update({"DRAM_TRAINING": "Disable"})
    out.update({"CHIP_NAME": platform})

    # ========== Show unused ==========
    for k in d:
        print("[Warning] %s unused! (Info could be lost in translations...)" % k)


