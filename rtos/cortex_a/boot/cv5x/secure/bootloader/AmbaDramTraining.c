#include "AmbaBLD.h"
#include "AmbaDramTrain.h"
#include "AmbaDramTrainingLog.h"

u64_t log_ptr = DDRCT_LOG_SPACE;
u64_t base_addr = HOST0_DDRC_REG_BASE_ADDR;
// delay global structures
delay_info_t  delay_info[] = {
  [WCK_WRDLY_FINE] = {
    .boffset = 0x228*8 + 0,
    .width = 5,
    .pitch = 0,
    .count = 1,
    .byte_offset = 6*4,
    .die_offset = 24,
    .is_dll_sel = 0
  },
  [WCK_WRDLY_COARSE] = {
    .boffset = 0x228*8 + 5,
    .width = 5,
    .pitch = 0,
    .count = 1,
    .byte_offset = 6*4,
    .die_offset = 24,
    .is_dll_sel = 0
  },
  [DQS_WRDLY_FINE] = {
    .boffset = 0x228*8 + 10,
    .width = 5,
    .pitch = 0,
    .count = 1,
    .byte_offset = 6*4,
    .die_offset = 24,
    .is_dll_sel = 0
  },
  [DQS_WRDLY_COARSE] = {
    .boffset = 0x228*8 + 15,
    .width = 5,
    .pitch = 0,
    .count = 1,
    .byte_offset = 6*4,
    .die_offset = 24,
    .is_dll_sel = 0
  },
  [DQS_RDDLY_FINE] = {
    .boffset = 0x228*8 + 20,
    .width = 5,
    .pitch = 0,
    .count = 1,
    .byte_offset = 6*4,
    .die_offset = 24,
    .is_dll_sel = 0
  },
  [DQS_RDDLY_COARSE] = {
    .boffset = 0x228*8 + 25,
    .width = 5,
    .pitch = 0,
    .count = 1,
    .byte_offset = 6*4,
    .die_offset = 24,
    .is_dll_sel = 0
  },
  [DQS_GATE_DLY_FINE] = {
    .boffset = (0x228+4)*8 + 0,
    .width = 5,
    .pitch = 0,
    .count = 1,
    .byte_offset = 6*4,
    .die_offset = 24,
    .is_dll_sel = 0
  },
  [DQS_GATE_DLY_COARSE] = {
    .boffset = (0x228+4)*8 + 5,
    .width = 5,
    .pitch = 0,
    .count = 1,
    .byte_offset = 6*4,
    .die_offset = 24,
    .is_dll_sel = 0
  },
  [DQ_WRDLY_COARSE] = {
    .boffset = (0x228+4)*8 + 10,
    .width = 5,
    .pitch = 0,
    .count = 1,
    .byte_offset = 6*4,
    .die_offset = 24,
    .is_dll_sel = 0
  },
  [DQ_RDDLY_COARSE] = {
    .boffset = (0x228+4)*8 + 15,
    .width = 5,
    .pitch = 0,
    .count = 1,
    .byte_offset = 6*4,
    .die_offset = 24,
    .is_dll_sel = 0
  },
  [DM_WRDLY_FINE] = {
    .boffset = (0x228+4)*8 + 20,
    .width = 5,
    .pitch = 0,
    .count = 1,
    .byte_offset = 6*4,
    .die_offset = 24,
    .is_dll_sel = 0
  },
  [DM_RDDLY_FINE] = {
    .boffset = (0x228+4)*8 + 25,
    .width = 5,
    .pitch = 0,
    .count = 1,
    .byte_offset = 6*4,
    .die_offset = 24,
    .is_dll_sel = 0
  },
  [DQ_WRDLY_FINE] = {
    .boffset = (0x228+8)*8 + 0,
    .width = 5,
    .pitch = 8,
    .count = 8,
    .byte_offset = 6*4,
    .die_offset = 24,
    .is_dll_sel = 0
  },
  [DQ_RDDLY_FINE] = {
    .boffset = (0x228+16)*8 + 0,
    .width = 5,
    .pitch = 8,
    .count = 8,
    .byte_offset = 6*4,
    .die_offset = 24,
    .is_dll_sel = 0
  },
  [DLL_CTRL_SEL0] = {
    .boffset = 0x210*8,
    .width = 6,
    .pitch = 0,
    .count = 1,
    .byte_offset = 1,
    .die_offset = 1,
    .is_dll_sel = 1
  },
  [DLL_CTRL_SEL1] = {
    .boffset = 0x218*8,
    .width = 6,
    .pitch = 0,
    .count = 1,
    .byte_offset = 1,
    .die_offset = 1,
    .is_dll_sel = 1
  },
  [DLL_CTRL_SEL2] = {
    .boffset = 0x220*8,
    .width = 6,
    .pitch = 0,
    .count = 1,
    .byte_offset = 1,
    .die_offset = 1,
    .is_dll_sel = 1
  },
  [CA_DELAY_FINE] = {
    .boffset = 0x2f0*8,
    .width = 5,
    .pitch = 8,
    .count = 7,
    .byte_offset = 4*4,
    .die_offset = 2,
    .is_dll_sel = 0
  }
};

#define OPTION_SIZE     16
u32_t options[OPTION_SIZE];
u32_t debug_level;
u32_t is_lp5;
u32_t vref_margin[2];
u32_t dtte_vref_margin;
u32_t step_select_oh;
u32_t dual_die_en;
u32_t tr_link_ecc_en;
u32_t mr11_d0;
u32_t mr11_d1;
u32_t mr13;
u32_t mr16;
u32_t mr18;
u32_t mr41;
u32_t start_host_id;
u32_t end_host_id;
u32_t vref_offsets;
u32_t vref_pass_range = 4;
u32_t read_delay_offset[2];
u32_t write_delay_offset[2];


int ddrc_training (void)
{
#ifdef DDRCT_INCLUDE_CMD_TRAIN_CODE
  u32_t cmd_en;
  u32_t cmd_ds;
#endif
#ifdef DDRCT_INCLUDE_WLVL_TRAIN_CODE
  u32_t wlvl_en;
  u32_t wlvl_ds;
#endif

  // uart init for print outs
  //plat_pinmux_init();
  //uart_init();

  for (int i = 0; i < OPTION_SIZE; i++) {
    options[i] = readl(log_ptr);
    log_ptr += 4;
  }

#ifdef DDRCT_INCLUDE_CMD_TRAIN_CODE
  cmd_en = (options[0] >> 0) & 1;
  cmd_ds = (options[0] >> 8) & 3;
#endif
#ifdef DDRCT_INCLUDE_WLVL_TRAIN_CODE
  wlvl_en = (options[0] >> 1) & 1;
  wlvl_ds = (options[0] >> 10) & 3;
#endif

  dual_die_en = (options[0] >> 19) & 1;
  tr_link_ecc_en = (options[0] >> 21) & 1;
  start_host_id = (options[0] >> 24) & 0xf;
  end_host_id = (options[0] >> 28) & 0xf;
  is_lp5 = (options[1] >> 30) & 1;
  mr11_d0 = (options[3] >> 0) & 0xff;
  mr11_d1 = (options[3] >> 8) & 0xff;
  mr18 = (options[3] >> 16) & 0xff;
  mr41 = (options[3] >> 24) & 0xff;
  mr13 = (options[4] >> 0) & 0xff;
  mr16 = (options[4] >> 8) & 0xff;

  debug_level = options[5];
  vref_margin[0] = options[6];
  print ("hey", 0, 0);
  vref_margin[1] = options[7];
  dtte_vref_margin = options[8];
  step_select_oh = options[9];
  vref_offsets = options[10];

  vref_pass_range = options[11];

  read_delay_offset[0] = options[12];
  read_delay_offset[1] = options[13];
  write_delay_offset[0] = options[14];
  write_delay_offset[1] = options[15];

#ifdef DDRCT_INCLUDE_PLOT_CODE
  if (options[1] & (1 << 31)) {
    u32_t plot_type = (options[0] & 0x3);
    u32_t hid = (options[0] >> 3) & 0xf;
    u32_t bid = (options[0] >> 7) & 0x3;
    u32_t did = (options[0] >> 9) & 0x1;
    u32_t delay2_shmoo_en = (options[0] >> 10) & 0x1;
    enum delay_id_t delay_id1 = (options[0] >> 11) & 0x1f;
    enum delay_id_t delay_id2 = (options[0] >> 16) & 0x1f;
    u32_t min_delay2 = (options[0] >> 21) & 0x1f;
    u32_t max_delay2 = (options[0] >> 26) & 0x1f;
    u32_t min_delay1 = (options[1] >> 0) & 0x1f;
    u32_t max_delay1 = (options[1] >> 5) & 0x1f;
    u32_t min_vref = (options[1] >> 10) & 0x7f;
    u32_t max_vref = (options[1] >> 17) & 0x7f;

    base_addr = HOST0_DDRC_REG_BASE_ADDR + (hid * 0x1000);

    // Run train preamble code
    ddrc_train_preamble(1);

    // Save registers to be restored after plot
    // DLL SEL*, VREF*, BYTE_DELAYS
    // for cbt: saved/restored within that function
    u32_t offset = (did*4+bid)*24;
    u32_t reg [11];
    u32_t old_vref;
    reg[0] = readl(base_addr | (DDRC_BYTE_DELAY_REG + offset + 0));
    reg[1] = readl(base_addr | (DDRC_BYTE_DELAY_REG + offset + 1*4));
    reg[2] = readl(base_addr | (DDRC_BYTE_DELAY_REG + offset + 2*4));
    reg[3] = readl(base_addr | (DDRC_BYTE_DELAY_REG + offset + 3*4));
    reg[4] = readl(base_addr | (DDRC_BYTE_DELAY_REG + offset + 4*4));
    reg[5] = readl(base_addr | (DDRC_BYTE_DELAY_REG + offset + 5*4));
    reg[6] = readl(base_addr | DDRC_DLL0_REG(did));
    reg[7] = readl(base_addr | DDRC_DLL1_REG(did));
    reg[8] = readl(base_addr | DDRC_DLL2_REG(did));
    reg[9] = readl(base_addr | DDRC_READ_DQ_VREF_REG(bid / 2));
    reg[10] = readl(base_addr | DDRC_WRITE_VREF_REG(did));
    old_vref = (reg[10] >> (bid * 8)) & 0xff;

    if (plot_type == 0) {
      ddrc_cbt_plot(hid, did, bid >> 1, min_vref, max_vref, min_delay1, max_delay1,
                    delay_id1 == WCK_WRDLY_FINE, mr11_d0, mr11_d1, mr18, mr41);
    }
    if (plot_type == 1) {
      u8_t vref_ids = 2;
      if (delay_id1 == 4 ||
          delay_id1 == 5 ||
          delay_id1 == 14 ||
          delay_id1 == 15) {
        vref_ids = 1;
      }
      ddrc_rw_plot(did, bid, 1, vref_ids, delay2_shmoo_en, delay_id1, delay_id2, min_vref, max_vref,
                   min_delay1, max_delay1, min_delay2, max_delay2);
    }
    if (plot_type == 2) {
      ddrc_rw_plot(did, bid, 0, 4, delay2_shmoo_en, delay_id1, delay_id2, min_vref, max_vref,
                   min_delay1, max_delay1, min_delay2, max_delay2);
    }
    if (plot_type == 3) {
      ddrc_wlvl_plot(did, bid, delay_id1, min_delay1, max_delay1, mr18);
    }

    // Restore registers
    writel(base_addr | (DDRC_BYTE_DELAY_REG + offset + 0),    reg[0]);
    writel(base_addr | (DDRC_BYTE_DELAY_REG + offset + 1*4),  reg[1]);
    writel(base_addr | (DDRC_BYTE_DELAY_REG + offset + 2*4),  reg[2]);
    writel(base_addr | (DDRC_BYTE_DELAY_REG + offset + 3*4),  reg[3]);
    writel(base_addr | (DDRC_BYTE_DELAY_REG + offset + 4*4),  reg[4]);
    writel(base_addr | (DDRC_BYTE_DELAY_REG + offset + 5*4),  reg[5]);
    writel(base_addr | DDRC_DLL0_REG(did),               reg[6]);
    writel(base_addr | DDRC_DLL1_REG(did),               reg[7]);
    writel(base_addr | DDRC_DLL2_REG(did),               reg[8]);
    writel(base_addr | DDRC_READ_DQ_VREF_REG(bid / 2), reg[9]);
    writel(base_addr | DDRC_WRITE_VREF_REG(did), reg[10]);
    if (bid%2 & is_lp5) {
      ddrc_mrw(15, old_vref, did, bid >> 1);
    } else {
      ddrc_mrw(14, old_vref, did, bid >> 1);
    }
    // Run postamble code
    ddrc_train_postamble(1);
  }
#endif

#ifndef DDRCT_INCLUDE_DTTE_CODE
  if (!(options[1] & (1 << 31))) {
    for (u32_t hid = start_host_id; hid <= end_host_id; hid++) {
      print("DDRCT: host = 0x", 1, 0);
      printInt(hid, 16, 0, 0);

      base_addr = HOST0_DDRC_REG_BASE_ADDR + (hid * 0x1000);

      // Run train preamble code
      ddrc_train_preamble(1);

#ifdef DDRCT_INCLUDE_CMD_TRAIN_CODE
      if (cmd_en) {
        ddrc_ca_training(hid, cmd_ds);
      }
#endif

#ifdef DDRCT_INCLUDE_WLVL_TRAIN_CODE
      if (wlvl_en) {
        ddrc_wlvl_train(wlvl_ds);
      }
#endif

#ifdef DDRCT_INCLUDE_RW_TRAIN_CODE
      ddrc_train_all();
#endif

      // Run postamble code
      ddrc_train_postamble(1);
    }

    // Issue ZQ calibration
    ddrc_zq_calibration();

  }
#endif


#ifdef DDRCT_INCLUDE_DTTE_CODE
  u32_t dtte_en = (options[1] >> 0) & 1;
  u32_t dtte_mode = (options[1] >> 1) & 1;
  u32_t obs_host_id = (options[1] >> 8) & 0xf;
  u32_t abct = (options[1] >> 16) & 0x1;
  u32_t dtte_update_en = (options[1] >> 17) & 0x1;

  for (u32_t hid = start_host_id; hid <= end_host_id; hid++) {
    print("DDRCT: host = 0x", 1, 0);
    printInt(hid, 16, 0, 0);
    base_addr = HOST0_DDRC_REG_BASE_ADDR + (hid * 0x1000);
    if (dtte_en) {
      if (dtte_mode) {
        print ("Starting DTTE in tracking mode", 0, 0);
        dtte_start_tracking(abct);
      } else {
        if (hid == obs_host_id) {
          print ("Starting DTTE in observation mode for host = 0x ", 1, 0);
          printInt(hid, 16, 0, 0);
          dtte_start_obs(dtte_update_en);
        }
      }
    }
  }
#endif

#ifdef DDRCT_INCLUDE_ODT_DS_SHMOO_CODE
  u32_t odt_shmoo_is_write = (options[0] >> 1) & 1;
  u32_t params_shmoo_range = options[1];
  ddrc_odt_dds_shmoo(odt_shmoo_is_write, params_shmoo_range);
#endif

  print("", 0, 0);
  return 0;
}

