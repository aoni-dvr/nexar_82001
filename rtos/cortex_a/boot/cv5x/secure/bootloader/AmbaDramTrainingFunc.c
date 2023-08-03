#include "AmbaDramTrain.h"
#include "AmbaDramTrainingLog.h"

extern u64_t base_addr;
extern u64_t log_ptr;
extern delay_info_t delay_info[];
extern u32_t is_lp5;
extern u32_t vref_margin[2];
extern u32_t dtte_vref_margin;
extern u32_t step_select_oh;
extern u32_t dual_die_en;
extern u32_t tr_link_ecc_en;
extern u32_t mr11_d0;
extern u32_t mr11_d1;
extern u32_t mr13;
extern u32_t mr16;
extern u32_t mr18;
extern u32_t mr41;
extern u32_t vref_offsets;
extern u32_t debug_level;
extern u32_t vref_pass_range;
extern u32_t read_delay_offset[2];
extern u32_t write_delay_offset[2];

#ifdef DDRCT_SI_DATA_PATTERN
static u32_t read_data_pattern = 0x95a6;
static u32_t write_data_pattern[][9] = {
  {0x0, 0xffffffff, 0x8001ffff, 0x80017ffe, 0x80017ffe, 0x7ffe, 0x80017ffe, 0x80017ffe, 0x33c303f0 ^ 0xaaaaaaaa},
  {0x80017ffe, 0xffffffff, 0xbffd4002, 0xbffd4002, 0xbffd4002, 0xbffd0000, 0xbffd4002, 0xbffd4002, 0x33c303f0},
  {0xffff4002, 0x2004ffff, 0x2004dffb, 0x2004dffb, 0xdffb, 0x2004dffb, 0x2004dffb, 0x2004dffb, 0x33c303f0 ^ 0x55555555},
  {0xffffffff, 0xeff71008, 0xeff71008, 0xeff71008, 0xeff70000, 0xeff71008, 0xeff71008, 0xffff1008, 0x33c303f0},
  {0x810ffff, 0x810f7ef, 0x810f7ef, 0xf7ef, 0x810f7ef, 0x810f7ef, 0x810f7ef, 0xffffffff, 0xa659a9a6},
  {0xfbdf0420, 0xfbdf0420, 0xfbdf0420, 0xfbdf0000, 0xfbdf0420, 0xfbdf0420, 0xffff0420, 0x240ffff, 0xa659a9a6 ^ 0xffffffff},
  {0x240fdbf, 0x240fdbf, 0xfdbf, 0x240fdbf, 0x240fdbf, 0x240fdbf, 0xffffffff, 0xfe7f0180, 0xa659a9a6 ^ 0x55555555},
  {0xfe7f0180, 0xfe7f0180, 0xfe7f0000, 0xfe7f0180, 0xfe7f0180, 0xffff0180, 0xffffffff, 0x0,  0xa659a9a6 ^ 0xaaaaaaaa}
};
#endif

void reset_cas_effect(void) {

  if (is_lp5) {
  // Reset cas effect register
  ddrc_uinst(DDRC_UINST_WCK_CTL, 8, 0);

#if 0
  // WKC2CK sync off
  writel(base_addr | DDRC_UINST_REG(3), (3 << 8) | 3);
  writel(base_addr | DDRC_UINST_REG(1), 0x7c);
  ddrc_uinst(DDRC_UINST_DCMD, 0, 0);
#endif

  // Release wck low
  ddrc_uinst(DDRC_UINST_WCK_CTL, 0, 0);

  // Issue CAS FS
  ddrc_uinst(DDRC_UINST_WCK_CTL, 16, 0);
  }

  // Reset the fifo pointers
  ddrc_uinst(DDRC_UINST_RPTR, 0, 0);
}

/* This function will send the uinst and poll the busy bit until the
   uinst is done */
void ddrc_uinst(u32_t opcode,
                u32_t iop0, u32_t iop1) {
  writel(base_addr | DDRC_UINST_REG(0), opcode | (iop0 << 8) | (iop1 << 16));
  writel(base_addr | DDRC_UINST_EN_REG, 1);
  while (readl(base_addr | DDRC_UINST_EN_REG) & 1);
}

/* Mode register write */
void ddrc_mrw(u32_t addr, u32_t data, u32_t did, u32_t cid) {
  // if did == 2 then write to both dies
  // if cid == 2 then write to both channels
  did = did + 1;
  cid = cid + 1;
  writel(base_addr | DDRC_MODE_REG, (data | (addr << 16) | (1 << 24) | (did << 25) | (cid << 27) | (1 << 31)));
  while(readl(base_addr | DDRC_MODE_REG) & (1 << 31));
}

/* Mode register read */
u32_t ddrc_mrr(u32_t addr, u32_t did, u32_t cid) {
  u32_t data = 0;
  u32_t temp;
  did = did + 1;
  cid = cid + 1;
  writel(base_addr | DDRC_MODE_REG, (data | (addr << 16) | (did << 25) | (cid << 27) | (1 << 31)));
  while(readl(base_addr | DDRC_MODE_REG) & (1 << 31));
  temp = readl(base_addr | DDRC_MODE_REG);
  if (cid == 1) {
    data = temp & 0xff;
  } else if (cid == 2) {
    data = (temp & 0xff00) >> 8;
  } else {
    data = temp & 0xffff;
  }
  print("Mode register read (did,cid,addr,data) : ", 1, 2);
  printInt((did << 28) | (cid << 24) | (addr << 16) | data, 16, 0, 2);
  return data;
}


/* This function is just simple timer */
__attribute__((optimize("O0"))) void ddrc_wait(int x) {
#ifndef DDRCT_SIM_ENV
  for (int i=0;i<x;i++) {
    for (int j=0;j<8;j++) {
    }
  }
#endif
}

/* This func is doing the ddrc register read modify write
   operation. */
void ddrc_read_modify_write(u32_t addr,
                            u32_t data, u32_t vld) {
  u32_t tmp;
  tmp = readl(base_addr | addr);
  tmp = (tmp & ~vld) | (data & vld);
  writel(base_addr | addr, tmp);
}

// Training preamble
void ddrc_init_mpc_pattern(void) {
  ddrc_mrw(is_lp5 ? 31 : 15, 0x55, 2, 2);
  ddrc_mrw(is_lp5 ? 32 : 20, 0x55, 2, 2);

  // Initialize MR and write data fifo
  ddrc_mrw(is_lp5 ? 33 : 32, 0x1c, 2, 2);
  ddrc_mrw(is_lp5 ? 34 : 40, 0x59, 2, 2);
  writel(base_addr | DDRC_MPC_WDATA_REG(0), 0x55555555);
  writel(base_addr | DDRC_MPC_WDATA_REG(1), 0xaaaaaaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(2), 0x5555aaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(3), 0x55555555);
  writel(base_addr | DDRC_MPC_WDATA_REG(4), 0x5555aaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(5), 0xaaaa5555);
  writel(base_addr | DDRC_MPC_WDATA_REG(6), 0x5555aaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(7), 0x5555aaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(8), 0x33c303f0);

  // flex pattern
  /*
  ddrc_mrw(is_lp5 ? 33 : 32, 0x7B, 2, 2);
  ddrc_mrw(is_lp5 ? 34 : 40, 0xB7, 2, 2);
  writel(base_addr | DDRC_MPC_WDATA_REG(0), 0xaaaaaaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(1), 0xaaaa5555);
  writel(base_addr | DDRC_MPC_WDATA_REG(2), 0xaaaaaaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(3), 0x5555aaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(4), 0xaaaaaaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(5), 0x5555aaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(6), 0xaaaaaaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(7), 0xaaaa5555);
  writel(base_addr | DDRC_MPC_WDATA_REG(8), 0xcf3f3fcf);
  */

  // toggle pattern
  /*
  ddrc_mrw(is_lp5 ? 33 : 32, 0xaa, 2, 2);
  ddrc_mrw(is_lp5 ? 34 : 40, 0x55, 2, 2);
  writel(base_addr | DDRC_MPC_WDATA_REG(0), 0xaaaa5555);
  writel(base_addr | DDRC_MPC_WDATA_REG(1), 0xaaaa5555);
  writel(base_addr | DDRC_MPC_WDATA_REG(2), 0xaaaa5555);
  writel(base_addr | DDRC_MPC_WDATA_REG(3), 0xaaaa5555);
  writel(base_addr | DDRC_MPC_WDATA_REG(4), 0x5555aaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(5), 0x5555aaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(6), 0x5555aaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(7), 0x5555aaaa);
  writel(base_addr | DDRC_MPC_WDATA_REG(8), 0x3333cccc);
  */
}


void ddrc_train_preamble(u32_t sref_en) {

  // Disable DRAM and auto refresh
  ddrc_read_modify_write(DDRC_CONTROL_REG, 0, 3);

#ifdef DDRCT_SI_DATA_PATTERN
  ddrc_mrw(is_lp5 ? 33 : 32, read_data_pattern & 0xff, 2, 2);
  ddrc_mrw(is_lp5 ? 34 : 40, (read_data_pattern >> 8) & 0xff, 2, 2);
#else
  // Initialize pattern
  ddrc_init_mpc_pattern();
#endif

  if (sref_en) {
    // both dies/both channels
    writel(base_addr | DDRC_UINST_REG(3), (3 << 8) | 3);

    // Wait for ddrc to be idle
    while (!(readl(base_addr | DDRC_STATUS_REG) & 4));

    // Go into self refresh
    writel(base_addr | DDRC_UINST_REG(1), is_lp5 ? 0x68 : 0x18);
    ddrc_uinst(DDRC_UINST_DCMD, 0, 0);
  }
}

// Training postamble
void ddrc_train_postamble(u32_t sref_en) {

  // unist to both dies/channels
  writel(base_addr | DDRC_UINST_REG(3), (3 << 8) |3);

  if (sref_en) {
    // Exit self refresh
    writel(base_addr | DDRC_UINST_REG(1), is_lp5 ? 0x28 : 0x14);
    ddrc_uinst(DDRC_UINST_DCMD, 0, 0);

    // issue auto refresh to all banks
    writel(base_addr | DDRC_UINST_REG(1), is_lp5 ? (0x40 << 8) | 0x38 : 0x28);
    ddrc_uinst(DDRC_UINST_DCMD, 0, 0);
  }
  // Enable ddrc and auto refresh
  ddrc_read_modify_write(DDRC_CONTROL_REG, 3, 3);

  if (is_lp5) {
    reset_cas_effect();
  }

  // Restore wdata and mask for DTTE pattern
  ddrc_init_mpc_pattern();
}

// ddrc get vfine
u32_t ddrc_get_vfine(u32_t bid) {
  u32_t data;
  data = readl(base_addr | (DDRIO_DLL_STATUS_BASE + ((bid >> 1) << 2)));
  if (bid%2) data = data >> 16;
  data = (data >> 1) & 0x1f;
  return data;
}

// fd2r
u32_t ddrc_fd2r(u32_t delay, u32_t vfine) {
  if (delay < vfine) {
    return ((1 << 5) | (vfine - delay));
  }
  return (delay - vfine);
}

// General Delay update function
// if idx=0xff, update all bit delays
void ddrc_set_dly(u32_t did, u32_t bid, enum delay_id_t delay_id, u32_t delay, u32_t idx) {
  // get delay info from global variable
  delay_info_t dinf = delay_info[delay_id];
  u32_t boffset = dinf.boffset + dinf.die_offset * did * 32 + dinf.byte_offset * bid * 8;
  u32_t dly_cnt = dinf.count;
  u32_t wen = (1 << dinf.width) - 1;
  u32_t reg_addr;
  u32_t reg_data;
  u32_t reg_wen;

  if (dinf.is_dll_sel) {
    delay = ddrc_fd2r(delay, ddrc_get_vfine(bid));
  }
  // update offset to correct bit, if updating single bit index
  if (idx != 0xff) {
    boffset += dinf.pitch * idx;
    dly_cnt = 1;
  }

  for (u32_t i = 0; i < dly_cnt; i++) {
    reg_addr = (boffset >> 5) * 4;
    reg_data = (delay << (boffset % 32));
    reg_wen = (wen << (boffset % 32));
    ddrc_read_modify_write(reg_addr, reg_data, reg_wen);
    boffset = boffset + dinf.pitch;
  }
}

// Reverse of delay update function, read delay value from ddrc reg
u32_t ddrc_get_dly(u32_t did, u32_t bid, enum delay_id_t delay_id, u32_t idx) {
  // idx -> in case of per bit delay, which bit to read
  delay_info_t dinf = delay_info[delay_id];
  u32_t boffset = dinf.boffset + dinf.die_offset * did * 32 + dinf.byte_offset * bid * 8 + idx * dinf.pitch;
  u32_t wen = (1 << dinf.width) - 1;
  u32_t reg_addr = (boffset >> 5) * 4;
  u32_t data = readl(base_addr | reg_addr);
  data = (data >> (boffset % 32)) & wen;
  return data;
}

// Vref update
void ddrc_vref_update(u32_t did, u32_t bid, u32_t vref, u32_t vref_ids) {
  u32_t temp, ben = 0;
  temp = (vref << 6) | (vref);
  temp |= temp << 16;
  if (vref_ids & 1) {
    ben |= 0x3f << 6;
  }
  if (vref_ids & 2) {
    ben |= 0x3f;
  }
  ben = (ben << ((bid & 1) * 16));
  if ((vref_ids & 1) | (vref_ids & 2)) {
    ddrc_read_modify_write(DDRC_READ_DQ_VREF_REG(bid / 2),
                           temp, ben);
  }

  // write vref update
  if (vref_ids & 4) {
    // range0: [0-50]
    // range1: [21-50]
    if(!is_lp5 && vref > 50){
      vref = ((vref - 30) | 0x40);
    }
    if (is_lp5 && bid%2==1) {
      ddrc_mrw(15, vref, did, bid >> 1);
    } else {
      ddrc_mrw(14, vref, did, bid >> 1);
    }
    // Also update ddrc WRITE_VREF register
    // LP4: We enter this function only for odd bytes. We also need to
    // update even byte with same value.
    if (is_lp5) {
      ddrc_read_modify_write(DDRC_WRITE_VREF_REG(did),
                             (vref << 24) | (vref << 16) | (vref << 8) | vref,
                             (0xFF << (bid*8)));
    } else {
      ddrc_read_modify_write(DDRC_WRITE_VREF_REG(did),
                             (vref << 24) | (vref << 16) | (vref << 8) | vref,
                             (0xFFFF << ((bid >> 1)*16)));
    }
  }
}

// Trigger RTT
void ddrc_get_rtt(u32_t did_en) {
  // save original dqs gate mode to be restored later.
  u32_t gate_reg = readl(base_addr | DDRC_GATE_REG);

  for (u32_t did = 0; did < 2; did++) {
    if (!(did_en & (1 << did))) {
      continue;
    }
    // Go to enhanced mode (LP5) or read preamble training (LP4)
    if (is_lp5) {
      ddrc_mrw(0x2e, 0x1, did, 2);
    } else {
      ddrc_mrw(0xd, 0xa, did, 2);
    }
    // Set gate into always on mode
    ddrc_read_modify_write(DDRC_GATE_REG, 1 << 11, 0x3 << 11);

    // RTT
    writel(base_addr | DDRC_INIT_CTL_REG, (1 << 0x2) | (did << 10));
    while(readl(base_addr | DDRC_INIT_CTL_REG) & (1 << 2));

    // Restore dqs gate mode
    writel(base_addr | DDRC_GATE_REG, gate_reg);

    // Come out of enhanced mode/read preamble mode
    if (is_lp5) {
      ddrc_mrw(0x2e, 0x0, did, 2);
    } else {
      ddrc_mrw(0xd, 0x8, did, 2);
    }
    // Reset the fifo pointers
    ddrc_uinst(DDRC_UINST_RPTR, 0, 0);
  }
}

void ddrc_set_cacs_coarse_delay(u32_t cid, u32_t delay) {
  ddrc_read_modify_write(DDRC_CA_DELAY_C_REG, (delay << 8) | delay, 0xff << (cid * 8));
  if (is_lp5) {
    // chip select coarse delay is CKE delay
    ddrc_read_modify_write(DDRC_CKE_DELAY_C_REG, (delay << 8) | delay, 0xff << (cid * 8));
  }
}

void ddrc_log(u32_t data) {
  if (log_ptr < (DDRCT_LOG_SPACE+DDRCT_LOG_SIZE)) {
    writel(log_ptr, data);
    log_ptr += 4;
  }
}

void ddrc_zq_calibration(void) {
  writel(AHOST_DDRC_REG_BASE_ADDR | DDRC_INIT_CTL_REG, 1 << 8);
  if (is_lp5) {
    writel(AHOST_DDRC_REG_BASE_ADDR | 0xb0, 0x670);
    writel(AHOST_DDRC_REG_BASE_ADDR | 0xac, 0xd);
    writel(AHOST_DDRC_REG_BASE_ADDR | 0xb8, 0x303);
    writel(AHOST_DDRC_REG_BASE_ADDR | 0xbc, 0x1);
  }
}


#ifdef DDRCT_INCLUDE_DTTE_CODE
/* DTTE register write */
void dtte_write(u32_t addr,
                u32_t data) {
  writel(base_addr | 0xf8, addr);
  writel(base_addr | 0xfc, data);
}

u32_t dtte_read(u32_t addr) {
  u32_t data;
  writel(base_addr | 0xf8, addr);
  data = readl(base_addr | 0xfc);
  return data;
}

u32_t ddrc_fr2d(u32_t delay, u32_t vfine){
   if ((delay >> 5) & 1){
      delay = delay & 0x1F;
      delay = vfine - delay;
      return delay;
   }
   return (delay+vfine);
}

/* DTTE register init */
void dtte_init() {
  u32_t data;
  u8_t vref_step_size_log2 = 2;
  u8_t read_vref_grid_size = 0x0; /* We can't train read for vref in cv5 */
  u8_t write_vref_grid_size = 0xf;
  u8_t read_delay_grid_size = 0xf;
  u8_t write_delay_grid_size = 0xf;

  // FIXME: DTTE_MPC_RMASK_DISABLE
  // write dqs training (id=1), vref_grid_size=0
  // read trainings: 0, 2, 3, 5, 7
  // write trainings: 1, 4, 6

  // Let DTTE run back to back, ddrc will throttle it to be
  // only run during refresh period.
  dtte_write(DTTE_TRAIN_PERIOD_REG, DTTE_TRAIN_PERIOD);

  // Grid step size (delay_step_size = 1, vref_step_size = 1)
  data = (vref_step_size_log2 << 24) | (vref_step_size_log2 << 16) | (vref_step_size_log2 << 8) | (vref_step_size_log2);
  dtte_write(DTTE_GRID_STEP_SIZE_LOG2_REG(0), data);
  dtte_write(DTTE_GRID_STEP_SIZE_LOG2_REG(1), data);

  // grid size
  data = (write_delay_grid_size << 24) | (0 << 16) | (read_delay_grid_size << 8) | (read_vref_grid_size);
  dtte_write(DTTE_GRID_SIZE_DIV2_REG(0), data);

  data = (read_delay_grid_size << 24) | (read_vref_grid_size << 16) | (read_delay_grid_size << 8) | (read_vref_grid_size);
  dtte_write(DTTE_GRID_SIZE_DIV2_REG(1), data);

  data = (read_delay_grid_size << 24) | (read_vref_grid_size << 16) | (write_delay_grid_size << 8) | (write_vref_grid_size);
  dtte_write(DTTE_GRID_SIZE_DIV2_REG(2), data);

  data = (read_delay_grid_size << 24) | (read_vref_grid_size << 16) | (write_delay_grid_size << 8) | (write_vref_grid_size);
  dtte_write(DTTE_GRID_SIZE_DIV2_REG(3), data);

  // Clip values
  data = (63 << 8) | (31 << 24);  // 0 read dqs
  dtte_write(DTTE_CLIP_VALUE_REG(0), data);
  dtte_write(DTTE_CLIP_VALUE_REG(2), data);
  dtte_write(DTTE_CLIP_VALUE_REG(3), data);
  dtte_write(DTTE_CLIP_VALUE_REG(5), data);
  dtte_write(DTTE_CLIP_VALUE_REG(7), data);

  data = ((is_lp5 ? 127 : 50) << 8) | (31 << 24);
  dtte_write(DTTE_CLIP_VALUE_REG(1), data);
  dtte_write(DTTE_CLIP_VALUE_REG(4), data);
  dtte_write(DTTE_CLIP_VALUE_REG(6), data);

  // expected pattern
  dtte_write(DTTE_EXPECTED_PATTERN_REG(0), 0x55555555);
  dtte_write(DTTE_EXPECTED_PATTERN_REG(1), 0xaaaaaaaa);
  dtte_write(DTTE_EXPECTED_PATTERN_REG(2), 0x5555aaaa);
  dtte_write(DTTE_EXPECTED_PATTERN_REG(3), 0x55555555);
  dtte_write(DTTE_EXPECTED_PATTERN_REG(4), 0x5555aaaa);
  dtte_write(DTTE_EXPECTED_PATTERN_REG(5), 0xaaaa5555);
  dtte_write(DTTE_EXPECTED_PATTERN_REG(6), 0x5555aaaa);
  dtte_write(DTTE_EXPECTED_PATTERN_REG(7), 0x5555aaaa);
  dtte_write(DTTE_EXPECTED_PATTERN_REG(8), 0x33c303f0);

  // Algorithm error mask (read trainings are all in 1D - time axis)
  // write dqs delay training (id = 1) is in 1D.
  // 0x1e means disable error ids = 4, 3, 2, 1
  dtte_write(0x14c, (0x1e << 0) | (0x1e << 8) | (0x1e << 16) | (0x1e << 24));
  dtte_write(0x150, (0x1e << 8) | (0x1e << 24));

  // dtte vref mgn
  dtte_set_vref_margin(0);
}

void dtte_set_vref_margin(bool_t is_coarse) {
  u8_t vref_margin_l = (dtte_vref_margin >> (!is_coarse * 16 + 0)) & 0xff;
  u8_t vref_margin_h = (dtte_vref_margin >> (!is_coarse * 16 + 8)) & 0xff;
  u32_t data = ((vref_margin_h << 28) | (vref_margin_l << 24) |
                (vref_margin_h << 20) | (vref_margin_l << 16) |
                (vref_margin_h << 12) | (vref_margin_l << 8) |
                (vref_margin_h << 4) | vref_margin_l);
  for (u8_t i=0; i<8; i++) {
    dtte_write(DTTE_VREF_MGN_REG(i), data);
  }
}

void dbse_start() {
  u32_t tmp;
  tmp = readl(DBSE_DEBUG_ADDRESS | 0x24);
  writel(DBSE_DEBUG_ADDRESS | 0x24, tmp & 0xfffffffe);
  writel(DBSE_DEBUG_ADDRESS | 0x24, tmp | 1);
}

void dbse_wait_for_done() {
  u32_t tmp;
  do {
    tmp = readl(DBSE_DEBUG_ADDRESS | 0x20);
  } while (!(tmp & (1 << 16)));
}

// dump DTTE info
// 1. algorithm error register
// 2. center delay for all bits/bytes
// 3. center_vref
// 4. grid status register (32x32 bits)
// if no algo error:
// 5. DTTE_VAR_REG
// It will also update center point into ddrc if no algorithm error.

void dtte_dump_grid(u8_t bid, bool_t dtte_update_en) {
  u32_t data;
  u8_t train_id, did;
  u8_t center_vref;
  u8_t center_dly[16];
  u8_t center_dm_dly[2];
  u8_t vfine[2];

  // Read observation information from DTTE
  data = dtte_read(DTTE_OBSV_CONFIG_REG);
  train_id = (data >> 4) & 7;
  did = (data >> 2) & 1;

  // get grid size and step size
  u8_t grid_size_div2[2];
  u8_t grid_step_size_log2[2];
  data = dtte_read(DTTE_GRID_SIZE_DIV2_REG(train_id / 2));
  data = data >> ((train_id%2)*16);
  grid_size_div2[0] = data & 0xff;
  grid_size_div2[1] = (data >> 8) & 0xff;
  data = dtte_read(DTTE_GRID_STEP_SIZE_LOG2_REG(train_id / 4));
  data = data >> ((train_id%4)*8);
  grid_step_size_log2[0] = data & 0x3;
  grid_step_size_log2[1] = (data >> 4) & 0x3;

  u32_t algo_error;
  algo_error = dtte_read(DTTE_ALGO_ERROR_REG);
  ddrc_log(algo_error);

  // Read center point
  // DTTE regiser will be set by dtte in ddrc, so we can
  // just read center point
  u32_t cp_info[11];
  for (int i = 0; i < 11; i++) {
    cp_info[i] = readl(base_addr | (DDRC_DTTE_CP_INFO_REG(i)));
    ddrc_log(cp_info[i]);
  }
  center_vref = BYTE(cp_info[0], bid);
  for (int b = 0; b < 2; b++) {
    vfine[b] = BYTE(cp_info[1], (bid >> 1) * 2 + b);
    center_dm_dly[b] = BYTE(cp_info[2] , (bid >> 1) * 2 + b);
    for (int bit = 0; bit < 8; bit++) {
      center_dly[bit] = BYTE(cp_info[(bid >> 1) * 4 + 3 + (bit >> 2)], bit%4);
      center_dly[bit + 8] = BYTE(cp_info[(bid >> 1) * 4 + 5 + (bit >> 2)], bit%4);
    }
  }

  if (train_id == 2 || train_id == 3 || train_id == 4) {
    // dll select
    center_dly[0] = ddrc_fr2d(center_dly[0], vfine[0]);
    center_dly[8] = ddrc_fr2d(center_dly[8], vfine[1]);
    for (int i = 1; i < 8; i++) {
      center_dly[i] = center_dly[0];
      center_dly[i+8] = center_dly[1];
    }
  }

  // Read variance register
  u8_t train_var[2];
  int var_delay, var_vref;
  data = dtte_read(DTTE_VAR_REG);
  ddrc_log(data);

  train_var[0] = (data & 0xff);
  train_var[1] = (data >> 8) & 0xff;
  var_delay = (data >> 24) & 0xFF;
  var_vref = (data >> 16) & 0xFF;
  if (var_delay >> 7) {
    var_delay = (var_delay ^ 0xff) + 1;
    var_delay = 0 - var_delay;
  }
  if (var_vref >> 7) {
    var_vref = (var_vref ^ 0xff) + 1;
    var_vref = 0 - var_vref;
  }

  // Log grid status
  if (bid == 1) {
    uart_putstr("\033[4;36H"); // set cursor to 4,36
  } else if (bid == 2) {
    uart_putstr("\033[4;70H"); // set cursor to 4,70
  } else if (bid == 3) {
    uart_putstr("\033[4;104H"); // set cursor to 4,104
  } else {
    uart_putstr("\033[4;2H"); // set cursor to 4,2
  }
  for (int i = 0; i < 32; i++) {
    data = dtte_read(DTTE_GRID_STATUS_REG(i));
    ddrc_log(data);
    uart_putstr("\033[31B"); // move cursor down 31 lines
    for (int j = 0; j < 32; j++) {
      if (i == grid_size_div2[1]  && j == grid_size_div2[0]) {
        uart_putstr("\033[1;32mC\033[0m");
      } else if (data & (1 << j)) {
        if (i == (grid_size_div2[1] + (var_delay >> grid_step_size_log2[1])) &&
            j == (grid_size_div2[0] + (var_vref >> grid_step_size_log2[0]))) {
          uart_putstr("\033[1;35mN\033[0m");
        } else {
          uart_putstr("*");
        }
      } else {
        uart_putstr(".");
      }
      if (j != 31) {
        uart_putstr("\033[1A"); // move cursor up 1 line
        uart_putstr("\033[1D"); // move cursor left
      }
    }
  }
  if (bid == 1) {
    uart_putstr("\033[46;0H"); // set cursor to row 46
  } else if (bid == 2) {
    uart_putstr("\033[52;0H"); // set cursor to row 52
  } else if (bid == 3) {
    uart_putstr("\033[58;0H"); // set cursor to row 58
  } else {
    uart_putstr("\033[40;0H"); // set cursor to row 40
  }

  // Update new center point
  if (!(algo_error & (1 << 16))) {
    // Update center point by writing to DDRC DTTE_Delay and DTTE_Vref
    // registers and triggering update command
    center_vref += var_vref;
    for (int i = 0; i < 16; i++) {
      center_dly[i] += var_delay;
    }
    center_dm_dly[0] += var_delay;
    center_dm_dly[1] += var_delay;

    if (train_id == 2 || train_id == 3 || train_id == 4) {
      // dll select
      center_dly[0] = ddrc_fd2r(center_dly[0], vfine[0]);
      center_dly[8] = ddrc_fd2r(center_dly[8], vfine[1]);
      for (int i = 1; i < 8; i++) {
        center_dly[i] = center_dly[0];
        center_dly[i+8] = center_dly[1];
      }
    }

    print(" center delay = [", 1, 0);
    for (int i = 0; i < 8; i++) {
      if (var_delay != 0) {
        uart_putstr("\033[1;31m", 0, 0);
        printInt(center_dly[(bid%2)*8+i], 16, 1, 0);
        uart_putstr("\033[0m", 0, 0);
      } else {
        printInt(center_dly[(bid%2)*8+i], 16, 1, 0);
      }
    }
    printInt(center_dm_dly[bid%2], 16, 1, 0);
    print("]", 0, 0);
    print(" center vref = [", 1, 0);
    if (var_vref != 0) {
      uart_putstr("\033[1;31m", 0, 0);
      printInt(center_vref, 16, 1, 0);
      uart_putstr("\033[0m", 0, 0);
    } else {
      printInt(center_vref, 16, 1, 0);
    }
    print("]", 0, 0);

    writel(base_addr | (DTTE_DELAY_REG(8)),
           (center_dm_dly[0]) | (center_dm_dly[1] << 8) |
           (center_dm_dly[0] << 16) | (center_dm_dly[1] << 24));
    writel(base_addr | (DTTE_DELAY_REG(9)),
           (center_vref) | (center_vref << 8) |
           (center_vref << 16) | (center_vref << 24));

    for (int i = 0; i < 4; i++) {
      writel(base_addr | (DTTE_DELAY_REG((bid / 2) * 4 + i)),
             (center_dly[i*4+0]) | (center_dly[i*4+1] << 8) |
             (center_dly[i*4+2] << 16) | (center_dly[i*4+3] << 24));
    }

    // Trigger the update
    if (dtte_update_en) {
      writel(base_addr | DTTE_CMD_REG, 0x18);
      while(readl(base_addr | DTTE_CMD_REG) & 0x10);
    }
  }
  // print train var
  print(" current variance (delay, vref) = 0x", 1, 0);
  printInt(((var_delay & 0xff) << 8) | (var_vref & 0xff), 16, 0, 0);
  print(" train variance (delay, vref) = 0x", 1, 0);
  printInt((train_var[1] << 8) | (train_var[0]), 16, 0, 0);
}

void dtte_start_tracking(bool_t abct) {
  dtte_stop_tracking();
  dtte_init();
  // Alternate between coarse/fine for few loops
  // and then set to fine

  if (abct) {
    for (int i = 4 ; i > 0; i--) {
      // Run coarse first
      dtte_set_vref_margin(1);
      writel(base_addr | DDRC_DTTE_DELAY_MAP_REG, 0xff);
      dtte_write(DTTE_TRAINING_ENABLE_REG, 0x1);
      ddrc_wait(100);
      dtte_stop_tracking();
      // Run fine loops
      dtte_set_vref_margin(0);
      writel(base_addr | DDRC_DTTE_DELAY_MAP_REG, 0x0);
      dtte_write(DTTE_TRAINING_ENABLE_REG, 0x1);
      if (i > 1) {
        ddrc_wait(100);
        dtte_stop_tracking();
      }
    }
  } else {
    dtte_write(DTTE_TRAINING_ENABLE_REG, 0x1);
  }
}

void dtte_stop_tracking() {
  u32_t data;
  dtte_write(DTTE_TRAINING_ENABLE_REG, 0x0);
  do {
    // reset grid status ready (safeguard if it was in observation mode)
    dtte_write(DTTE_GRID_STAT_READY_REG, 1);
    dtte_write(DTTE_ALGO_ERROR_REG, 0x10000); // clear algorithm error if any
    data = dtte_read(DTTE_TRAINING_ENABLE_REG);
  } while (!(data & (1<<4)));
}

void dtte_start_obs(bool_t dtte_update_en) {
  dtte_stop_tracking();
  dtte_init();

  u32_t data;
  u32_t dbse_error_count = 0;

  // log some info
  u32_t loops;
  loops = readl(base_addr|DDRC_RSVD_SPACE);
  ddrc_log(loops);

  u8_t obs_train_id;
  u8_t obs_did;
  u8_t obs_bid;

  data = dtte_read(DTTE_DRAM_CONFIG_REG);
  ddrc_log(data);
  data = dtte_read(DTTE_TRAINING_CONFIG1_REG);
  ddrc_log(data);
  data = dtte_read(DTTE_OBSV_CONFIG_REG);
  obs_did = (data >> 2) & 1;
  obs_bid = ((data >> 1) & 1) * 2 + (data & 1);
  ddrc_log(data);
  obs_train_id = (data & (7 << 4)) >> 4;
  data = dtte_read(DTTE_TRAINING_CONFIG0_REG);
  ddrc_log(data);
  data = dtte_read(DTTE_GRID_STEP_SIZE_LOG2_REG(obs_train_id / 4));
  ddrc_log(data);
  // Clip value
  data = dtte_read(DTTE_CLIP_VALUE_REG(obs_train_id));
  ddrc_log(data);

  // start dbse
  dbse_start();

  uart_putstr("\033[2J"); // clear screen
  uart_putstr("\033[0;8H"); // set cursor to 2,2
  print("DTTE observation dump for train id = 0x", 1, 0);
  printInt(obs_train_id, 16, 1, 0);
  print(" did = 0x", 1, 0);
  printInt(obs_did, 16, 0, 0);

  // start dtte
  dtte_write(DTTE_TRAINING_ENABLE_REG, 0x1);

  // Wait for dtte to be ready
  while (!(dtte_read(DTTE_GRID_STAT_READY_REG) & 1));

  for (int i=0; i < loops; i++) {
    for (u8_t bid = 0; bid < DDRC_NUM_BYTES; bid++) {
      // set DTTE CID/BID
      u32_t temp = dtte_read(0x14) & (~0x3);
      temp |= (((bid >> 1) << 1) | (bid % 2));
      dtte_write(0x14, temp);

      // reset grid status
      dtte_write(DTTE_GRID_STAT_READY_REG, 1);

      // Wait for dtte to be ready
      while (!(dtte_read(DTTE_GRID_STAT_READY_REG) & 1));

      // dump
      ddrc_log((0x100b << 16) | i);
      dtte_dump_grid(bid, dtte_update_en);

      // if dbse is done, restart
      temp = readl(DBSE_DEBUG_ADDRESS | 0x20);
      if (temp & (1 << 16)) {
        if (temp & (1 << 17)) {
          dbse_error_count += 1;
        }
        uart_putstr("\033[64;0H");
        print("DBSE status (a0, a1, d0, d1) = ", 1, 0);
        printInt(readl(DBSE_DEBUG_ADDRESS | 0x0), 16, 1, 0);
        printInt(readl(DBSE_DEBUG_ADDRESS | 0x4), 16, 1, 0);
        u32_t d0, d1;
        d0 = readl(DBSE_DEBUG_ADDRESS | 0x8);
        d1 = readl(DBSE_DEBUG_ADDRESS | 0xc);
        printInt(d0, 16, 1, 0);
        printInt(d1, 16, 1, 0);
        printInt(d0^d1, 16, 1, 0);
        printInt(dbse_error_count, 16, 0, 0);
        dbse_start();
      }
    }
  }
}
#endif // DDRCT_INCLUDE_DTTE_CODE

#ifdef DDRCT_INCLUDE_PLOT_CODE
// Check pattern
u32_t ddrc_read_multiMPC_check(u32_t bid, u32_t type){
   u32_t fail = 0xff;
   u32_t read_pat, wrt_pat;
   u32_t  tmp_rslt = 0;
   for (int i = 0; i < 8; i++){
     read_pat = readl(base_addr | DDRC_MPC_RDATA_REG(i + ((bid / 2) * 8)));
     wrt_pat  = readl(base_addr | DDRC_MPC_WDATA_REG(i));
     if (type) {
       wrt_pat = wrt_pat ^ 0xffffffff;
     }
     if(bid%2) {
       tmp_rslt |= ((((read_pat^wrt_pat)>>24) & 0xff) | (((read_pat^wrt_pat)>>8) & 0xff));
     }
     else {
       tmp_rslt |= ((((read_pat^wrt_pat)>>16) & 0xff) | ((read_pat^wrt_pat) & 0xff));
     }
   }
   fail &= tmp_rslt;
   return fail;
}

// Check mask bits
u32_t ddrc_read_mask_check(u32_t bid) {
  u32_t tmp_rslt;
  u32_t fail = 0x1;
  u32_t read_pat, wrt_pat;
  read_pat = readl(base_addr | DDRC_MPC_RMASK_REG(bid / 2));
  wrt_pat = readl(base_addr | DDRC_MPC_WDATA_REG(8));
  if (bid%2) {
    tmp_rslt = (read_pat ^ wrt_pat) & 0xaaaaaaaa;
  } else {
    tmp_rslt = (read_pat ^ wrt_pat) & 0x55555555;
  }
  if (!tmp_rslt) {
    fail = 0;
  }
  // Clear MPC rdata register
  writel(base_addr | DDRC_CLEAR_MPC_RDATA, 1);
  return fail;
}

u32_t ddrc_pattern_check(u32_t bid, u32_t is_read, u32_t is_fine, u32_t is_plot) {
  u32_t fail = 0;
  if (is_read) {
    ddrc_uinst(DDRC_UINST_MPC_RD_DQ, DDRC_UINST_BO, DDRC_UINST_N_M1);
  } else {
    ddrc_uinst(DDRC_UINST_MPC_WRRD_FIFO,  DDRC_UINST_BO, DDRC_UINST_N_M1);
  }
  // Reset the fifo pointers
  ddrc_uinst(DDRC_UINST_RPTR, 0, 0);
  if (is_plot) {
    if (is_plot > 1) {
      fail = ddrc_read_mask_check(bid);
    } else {
      fail = ddrc_read_multiMPC_check(bid, 0);
    }
  } else if (is_fine) {
    fail = ddrc_read_multiMPC_check(bid, 0) | (ddrc_read_mask_check(bid) << 8);
  } else  {
    fail = ddrc_read_multiMPC_check(bid, 0) != 0 || ddrc_read_mask_check(bid);
  }
  if (is_read) {
    // Invert pattern and read again, check, and restore
    ddrc_mrw(is_lp5 ? 31 : 15, 0xaa, 2, 2); // fixme: should we just try only channel under training?
    ddrc_mrw(is_lp5 ? 32 : 20, 0xaa, 2, 2);
    ddrc_uinst(DDRC_UINST_MPC_RD_DQ, DDRC_UINST_BO, DDRC_UINST_N_M1);
    // Reset the fifo pointers
    ddrc_uinst(DDRC_UINST_RPTR, 0, 0);
    if (is_plot) {
      if (is_plot > 1) {
        fail |= ddrc_read_mask_check(bid);
      } else {
        fail |= ddrc_read_multiMPC_check(bid, 1);
      }
    } else if (is_fine) {
      fail |= (ddrc_read_multiMPC_check(bid, 1) | (ddrc_read_mask_check(bid) << 8));
    } else {
      fail |= (ddrc_read_multiMPC_check(bid, 1) != 0 || ddrc_read_mask_check(bid));
    }
    ddrc_mrw(is_lp5 ? 31 : 15, 0x55, 2, 2);
    ddrc_mrw(is_lp5 ? 32 : 20, 0x55, 2, 2);
  } else {
    // Invert the write pattern
    u32_t data;
    for (int i = 0; i < 9; i++) {
      data = readl(base_addr | DDRC_MPC_WDATA_REG(i));
      writel(base_addr | DDRC_MPC_WDATA_REG(i), data ^ 0xffffffff);
    }

    ddrc_uinst(DDRC_UINST_MPC_WRRD_FIFO,  DDRC_UINST_BO, DDRC_UINST_N_M1);
    // Reset the fifo pointers
    ddrc_uinst(DDRC_UINST_RPTR, 0, 0);
    if (is_plot) {
      if (is_plot > 1) {
        fail |= ddrc_read_mask_check(bid);
      } else {
        fail |= ddrc_read_multiMPC_check(bid, 0);
      }
    } else if (is_fine) {
      fail |= (ddrc_read_multiMPC_check(bid, 0) | (ddrc_read_mask_check(bid) << 8));
    } else {
      fail |= (ddrc_read_multiMPC_check(bid, 0) != 0 || ddrc_read_mask_check(bid));
    }
    // Invert the write pattern
    for (int i = 0; i < 9; i++) {
      data = readl(base_addr | DDRC_MPC_WDATA_REG(i));
      writel(base_addr | DDRC_MPC_WDATA_REG(i), data ^ 0xffffffff);
    }
  }

  return fail;
}

void ddrc_ca_vref_update(u32_t vref) {
  if(is_lp5) {
    // Drive DM0
    ddrc_uinst(DDRC_UINST_DM0_CTL, 2, 0);
  }
  if(!is_lp5 && vref > 50){
    vref = ((vref - 50) | 0x40);
  }
  // Drive CA vref to DQ[6:0]
  ddrc_uinst(DDRC_UINST_DQD, vref, 0);

  if(is_lp5){
    // LP5 : Trigger toggle DM0
    ddrc_uinst(DDRC_UINST_DM0_CTL, 3, 0);
  } else {
    // LP4 : Trigger toggle DQS
    ddrc_uinst(DDRC_UINST_TOGGLE, 0, 0);
  }

  // Release DQ[6:0]
  ddrc_uinst(DDRC_UINST_DQR, 0, 0);
  ddrc_wait(8); /* vref settle time wait = 250 ns */
}

//command bus training
u32_t ddrc_check_ca_pattern(u32_t cid) {
  u32_t fail = 0;
  for (int i = 0; i < 4; i++) {
    u32_t ca_pattern = 0;
    if (i > 1) ca_pattern = 0xaa;
    if (i%2) ca_pattern = ca_pattern ^ 0xff; // Invert pattern
    ddrc_uinst(DDRC_UINST_CAD, ca_pattern, 0);
    ddrc_wait(1);
    // trigger dq capture
    ddrc_uinst(DDRC_UINST_DQC, 0, 0);
    // check pattern
    u32_t mask = 0x3f;
    if (is_lp5) { mask = 0x7f; }
    ca_pattern = ca_pattern & mask;
    u32_t tmp;
    if (is_lp5) {
      tmp = ((readl(base_addr | DDRC_UINST_RSLT_REG) >> (cid*16)) & mask);
    } else {
      tmp = ((readl(base_addr | DDRC_UINST_RSLT_REG)) >>  (8 + cid*16)) & mask;
    }
    fail |= tmp ^ ca_pattern;
  }
  return fail;
}

// command training shmoo plot
// It does followings -
// 1. Go to high frequency
// 2. Enter into CBT mode 2
// 3. Shmoo vref and command bus delay
// 4a. Iterate for patterns (0x0, 0xff, 0xaa, 0x55):
// 4b. Log pass fail info (pass if all patterns passed).

void ddrc_cbt_plot(u32_t hid, u32_t did, u32_t cid,
                   u32_t min_vref, u32_t max_vref,
                   u32_t min_delay1, u32_t max_delay1,
                   u32_t is_fine,
                   u32_t mr11_d0, u32_t mr11_d1,
                   u32_t mr18, u32_t mr41
                   ) {

  u32_t log_data = 0; /* Collect 4 bytes of data to log */
  u32_t bcnt = 0; /* Log byte count */
  u32_t fail = 0;

  // Save register values to be restored later.
  u32_t reg0 = readl(base_addr | DDRC_CA_DELAY_F_REG(cid * 4 + did * 2 + 0));
  u32_t reg1 = readl(base_addr | DDRC_CA_DELAY_F_REG(cid * 4 + did * 2 + 1));
  u32_t reg2 = readl(base_addr | DDRC_CS_DELAY_F_REG);
  u32_t reg3 = readl(base_addr | DDRC_CA_DELAY_C_REG);
  u32_t reg4 = readl(base_addr | DDRC_CKE_DELAY_C_REG);

  print("***Starting command bus plot for (is_fine, did, cid) = 0x", 1, 0);
  printInt((is_fine << 16) | (did << 8) | cid, 16, 0, 0);
  print ("Before", 0, 1);
  u8_t temp = ddrc_mrr(0x5, did, cid);
  temp = ddrc_mrr(0xc, did, cid);

  // Set CID/DID of uinst
  writel(base_addr | DDRC_UINST_REG(3), ((IDX2OH(cid)) << 8) | (IDX2OH(did)));

  if(is_lp5){
    // Drive WCK low
    ddrc_uinst(DDRC_UINST_WCK_CTL, 2, 0);
    // Drive DQ7 = 0
    ddrc_uinst(DDRC_UINST_DQ7_CTL, 2, 0);
    // Drive DM0 = 0
    ddrc_uinst(DDRC_UINST_DM0_CTL, 2, 0);
    // Drive valid DQ = 0
    ddrc_uinst(DDRC_UINST_DQD, 0, 0);
    // Set MR16[5:4]=2 (fsp1) to trigger CBT mode, set VRCG=1
    ddrc_mrw(16, (mr16 & 0xcf) | (2<<4), did, cid);
    // wait 20ns
    ddrc_wait(1);
    // Release wck low
    ddrc_uinst(DDRC_UINST_WCK_CTL, 0, 0);
    // Wait 5ns
    ddrc_wait(1);
    // Drive DQ7 to high
    ddrc_uinst(DDRC_UINST_DQ7_CTL, 3, 0);
  } else {
    // Set MR13[0]=1 to trigger CBT mode, set VRCG=1
    ddrc_mrw(13, mr13 | 1, did, cid);
    // Issue DQS_PD
    ddrc_uinst(DDRC_UINST_DQS0D, 0, 0);
    // Drive DRAM_CKE LOW
    u32_t mask;
    mask = 1 << ((did << 1) + cid);
    ddrc_read_modify_write(DDRC_CKE_REG, 0, mask);
  }

  for (u8_t vref = max_vref; vref >= min_vref; vref--) {
    printInt(vref, 16, 1, 0);
    print(": ", 1, 0);
    ddrc_ca_vref_update(vref);
    // update the delay
    for (u8_t dly=min_delay1; dly <= max_delay1; dly += 1) {
      // Program delay
      if (is_fine) {
        ddrc_set_dly(did, cid, CA_DELAY_FINE, dly, 0xff);
      } else {
        ddrc_read_modify_write(DDRC_CA_DELAY_C_REG, dly << 8 | dly, 0xff << (cid * 8));
        if (is_lp5) {
          // chip select coarse delay is CKE delay
          ddrc_read_modify_write(DDRC_CKE_DELAY_C_REG, dly << 8 | dly, 0xff << (cid * 8));
        }
      }

      // Check CA patterns
      fail = ddrc_check_ca_pattern(cid);
      if (fail) {
        log_data |= (0xff << (bcnt * 8));
        print(".", 1, 0);
      } else {
        print("*", 1, 0);
      }
      if (bcnt == 3) {
        ddrc_log(log_data);
        bcnt = 0;
        log_data = 0;
      } else {
        bcnt += 1;
      }
    }
    print ("", 0, 0);
    if (vref == min_vref) { break; }
  }

  // Exit training
  // Restore delay registers
  writel(base_addr | DDRC_CA_DELAY_F_REG(cid * 4 + did * 2 + 0), reg0);
  writel(base_addr | DDRC_CA_DELAY_F_REG(cid * 4 + did * 2 + 1), reg1);
  writel(base_addr | DDRC_CS_DELAY_F_REG, reg2);
  writel(base_addr | DDRC_CA_DELAY_C_REG, reg3);
  writel(base_addr | DDRC_CKE_DELAY_C_REG, reg4);

  if(is_lp5) {
    // Drive DQ7 = 0
    ddrc_uinst(DDRC_UINST_DQ7_CTL, 2, 0);
    ddrc_wait(1);
    // drive wck low
    ddrc_uinst(DDRC_UINST_WCK_CTL, 2, 0);
    ddrc_wait(8); /* vref settle time wait = 250 ns */
    // Drive DM 0
    ddrc_uinst(DDRC_UINST_DM0_CTL, 2, 0);
    // Leave CBT mode
    ddrc_mrw(16, mr16, 2, cid);
    // Release DQ7
    ddrc_uinst(DDRC_UINST_DQ7_CTL, 0, 0);
    // Relase DM0
    ddrc_uinst(DDRC_UINST_DM0_CTL, 0, 0);

    // Reset CAS effect
    reset_cas_effect();
  } else {
    // Drive DRAM_CKE HIGH
    u32_t mask;
    mask = 1<< ((did << 1) + cid);
    ddrc_read_modify_write(DDRC_CKE_REG, 0xF, mask);
    // Leave CBT mode
    ddrc_mrw(13, mr13, did, cid);
  }

  print ("After", 0, 1);
  temp = ddrc_mrr(0x5, did, cid);
  temp = ddrc_mrr(0xc, did, cid);
}

// Write levelling
// Change Delay WCK and capture sample phase from device.
// Log the result.

void ddrc_wlvl_plot(u32_t did, u32_t bid, enum delay_id_t delay_id,
                    u32_t min_delay1, u32_t max_delay1,
                    u32_t mr18) {
  print(" wckLevelling: (mr18 << 24 | (did << 8) | bid) ", 1, 0);
  printInt((mr18 << 24) | (did << 8) | (bid), 16, 0, 0);
  u32_t step_count = 0;
  u32_t curr_state = 2; // 0: +, 1: -, 2: x
  // Set CID/DID of uinst
  u32_t cid = bid >> 1;
  writel(base_addr | DDRC_UINST_REG(3), ((cid + 1) << 8) | (did + 1));
  // Drive WCK low
  ddrc_uinst(DDRC_UINST_WCK_CTL, 2, 0);
  // MR18[6] = 1 to trigger WLVL leveling mode
  ddrc_mrw(18, mr18 | (1<<6), did, cid);

  // UINST.WT_PRE to odd (bug 21229 inherited half cycle delay in write
  // leveling).
  u32_t uinst_wt_data = readl(base_addr | DDRC_UINST_WT_REG);
  writel(base_addr | DDRC_UINST_WT_REG, uinst_wt_data^1);

  for (u32_t delay = min_delay1; delay <= max_delay1; delay += 1) {
    ddrc_set_dly(did, bid, delay_id, delay, 0);

    // Toggle WCK
    ddrc_uinst(DDRC_UINST_WCK_CTL, 6, 8);
    // wait for TWL0
    ddrc_wait(1);
    // Capture DQ
    ddrc_uinst(DDRC_UINST_DQC, 1, 0);
    u32_t data = (readl(base_addr | DDRC_UINST_RSLT_REG) & (0xff << (bid*8))) >> (bid*8);
    ddrc_log((delay << 16)  | data);
    if (data == 0xff) {
      if (curr_state == 1) {
        printInt(step_count, 16, 1, 0);
        step_count = 0;
      }
      print(".", 1, 0);
      step_count += 1;
      curr_state = 0;
    } else {
      if (curr_state == 0) {
        printInt(step_count, 16, 1, 0);
        step_count = 0;
      }
      print("+", 1, 0);
      step_count += 1;
      curr_state = 1;
    }
  }
  print("", 0, 0);

  // Exit training
  ddrc_mrw(18, mr18 & (~(1<<6)), did, cid);

  // revert the WT_PRE
  writel(base_addr | DDRC_UINST_WT_REG, uinst_wt_data^1);

  // Reset CAS effect
  reset_cas_effect();
}

void ddrc_rw_plot(u32_t did, u32_t bid, u32_t is_read,
                  u32_t vref_ids,
                  u32_t delay2_shmoo_en,
                  enum delay_id_t delay_id1,
                  enum delay_id_t delay_id2,
                  u32_t min_vref,
                  u32_t max_vref,
                  u32_t min_delay1,
                  u32_t max_delay1,
                  u32_t min_delay2,
                  u32_t max_delay2) {

  writel(base_addr | DDRC_UINST_REG(3), (((bid >> 1) + 1) << 8 | (did + 1)));
  u32_t log_data = 0;
  u32_t bcnt = 0;
  u32_t fail = 0;
  print("***Shmoo plot: (is_read,did,bid) = 0x", 1, 0);
  printInt((is_read << 16) | (did << 8) | bid, 16, 0, 0);
  print("****** (delay2_shmoo_en, delay_id2, delay_id1) = 0x", 1, 0);
  printInt((delay2_shmoo_en << 16) | (delay_id2 << 8) | delay_id1, 16, 0, 0);

  // If delay2 is not part of shmoo, set variables so that outer loop
  // is executed only once.
  if (!delay2_shmoo_en) {
    min_delay2 = 0;
    max_delay2 = 0;
  }

  for (u32_t delay2 = min_delay2; delay2 <= max_delay2; delay2 += 1) {
    if (delay2_shmoo_en) {
      ddrc_set_dly(did, bid, delay_id2, delay2, 0xff);
      print("--- delay2 -- : ", 1, 0);
      printInt(delay2, 16, 0, 0);
    }
    for (u32_t vref = max_vref; vref >= min_vref; vref -= 1) {
      printInt(vref, 16, 1, 0);
      print(":", 1, 0);
      ddrc_vref_update(did, bid, vref, vref_ids);
      ddrc_wait(8); /* vref settle time wait = 250 ns */
      for (u32_t delay1 = min_delay1; delay1 <= max_delay1; delay1 += 1) {
        ddrc_set_dly(did, bid, delay_id1, delay1, 0xff);
        fail = ddrc_pattern_check(bid, is_read, 0,
                                  delay_id1 == DM_WRDLY_FINE || delay_id1 == DM_RDDLY_FINE ? 2 : 1);
        if (fail) {
          print(".", 1, 0);
        } else {
          print("*", 1, 0);
        }
        log_data |= (fail << (bcnt*8));
        if (bcnt == 3) {
          ddrc_log(log_data);
          bcnt = 0;
          log_data = 0;
        } else {
          bcnt += 1;
        }
      }
      print("", 0, 0);
      if (vref == min_vref) { break; }
    }
  }

  // log remaining word
  ddrc_log(log_data);
}

#endif

#ifdef DDRCT_INCLUDE_DCM_TRAIN_CODE
void ddrc_dcm_training(u32_t did_en) {
  u32_t tstatus = 1;
  u32_t done = 0; // one bit for each {die, byte}
  u32_t dca[8];
  u32_t dca_dir[8];
  //u32_t wck_duty_en[4];
  u32_t wck_duty_p[4];
  u32_t wck_duty_n[4];
  int start[8]; // start of passing range
  int end[8]; // end of passing range
  u32_t status, idx;
  u32_t temp;
  int i, did, bid;

  print(" DCM training : ", 1, 0);
  print("", 0, 1);

  for (i = 0; i < 8; i++) {
    dca[i] = 0;
    dca_dir[i] = 2; // 0: decrease, 1: increase: 2: first
    start[i] = 0;
    end[i] = 0;
  }

  if (did_en != 3) {done = 0xf0;}

  // Read ddrc wck duty cycle registers
  for (i = 0; i < DDRC_NUM_BYTES; i++) {
    temp = readl(base_addr | (DDRC_WCK_DUTY_CYCLE_REG(i)));
    //wck_duty_en[i] = (temp & (1 << 26)) >> 26;
    wck_duty_p[i] = (temp & (7 << 20)) >> 20;
    wck_duty_n[i] = (temp & (7 << 23)) >> 23;
  }

  while (done != 0xff) {
    // trigger DCM for all dies; all channels
    ddrc_mrw(0x1a, 0x1, 2, 2);
    ddrc_wait(100); // wait for tDCMM=2us
    // flip the bit
    ddrc_mrw(0x1a, 0x3, 2, 2);
    ddrc_wait(100); // wait for tDCMM=2us
    // stop the DCM
    ddrc_mrw(0x1a, 0x2, 2, 2);
    for (did = 0; did < 2; did++) {
      if (!(did_en & (1 << did))) {
        continue;
      }
      status = ddrc_mrr(0x1a, did, 2);
      for (bid = 0; bid < DDRC_NUM_BYTES; bid++) {
        print("(did, bid) : ", 1, 2);
        printInt((did << 8) | (bid << 0), 16, 0, 2);
        idx = 4 * did + bid;
        temp = (status >> ((bid/2) * 8 + (bid%2) * 2 + 2)) & 3;
        if (!(done & (1 << idx))) {
          printInt((bid << 8) | done, 16, 1, 2);
          if (((temp == 0) && (dca_dir[idx] == 0)) |
              ((temp == 3) && (dca_dir[idx] == 1))) {
            // done
            print("*** direction change ", 0, 2);
            done |= (1 << idx);
            end[idx] = (dca[idx] & (1 << 3)) ? dca[idx] & 0x7 : 0 - (dca[idx] & 0x7);
          } else if ((temp != 0 && dca_dir[idx] == 0 && dca[idx] == 7) |
                     (temp != 3 && dca_dir[idx] == 1 && dca[idx] == 15) |
                     (dca_dir[idx] == 2 && dca[idx] == 15)) {
            if ((dca_dir[idx] == 0 && (wck_duty_n[bid] < 3)) |
                (dca_dir[idx] == 1 && (wck_duty_p[bid] < 3)) |
                (dca_dir[idx] == 2 && (wck_duty_p[bid] < 3))) {
              if (did_en == 3) {
                done &= ~(1 << (idx ^ 4));
              }
              if (dca_dir[idx] == 0) {
                wck_duty_n[bid]++;
                print("Increasing ddrc duty cycle n", 0, 2);
              } else {
                wck_duty_p[bid]++;
                print("Increasing ddrc duty cycle p", 0, 2);
              }
              ddrc_read_modify_write(DDRC_WCK_DUTY_CYCLE_REG(bid),
                                     (1 << 26) | (wck_duty_n[bid] << 23) | (wck_duty_p[bid] << 20),
                                     (1 << 26) | (7 << 23) | (7 << 20));
              dca[idx] = 0;
              dca_dir[idx] = 2;
              dca[idx ^ 4] = 0;
              dca_dir[idx ^ 4] = 2;
            } else {
              done |= (1 << idx);
              tstatus = 0;
            }
          } else {
            if (temp == 0 || temp == 3) {
              start[idx] = (dca[idx] & (1 << 3)) ? dca[idx] & 0x7 : 0 - (dca[idx] & 0x7);
              // On temp = [1,2], we increase till we hit 0 or 3 transition.
              // When we revert the direction at 3, we want next loop to start from -1
              if (dca_dir[idx] == 2) {
                dca[idx] = 0;
              }
            }
            dca_dir[idx] = temp == 0 ? 1 : temp == 3 ? 0 : dca_dir[idx];

            if (dca_dir[idx]) {
              print("++", 0, 2);
            } else {
              print("--", 0, 2);
            }
            dca[idx]++;
            if (dca_dir[idx]) {
              dca[idx] |= (1 << 3);
            }
          }
        }

        // Program MR30
        if (bid%2) {
          temp = (dca[4 * did + 2 * (bid/2) + 1] << 4) | (dca[4 * did + 2 * (bid/2) + 0]);
          print("Updating MR30 with ", 1, 2);
          printInt(temp, 16, 0, 2);
          ddrc_mrw(0x1e, temp, did, bid/2);
        }
      }
    }
  }

  // Program MR with DCM result
  for (did = 0; did < 2; did++) {
    if (!(did_en & (1 << did))) {
      continue;
    }

    for (bid = 0; bid < DDRC_NUM_BYTES; bid++) {
      if (bid%2 == 0) {        temp = 0; }
      idx = did * 4 + bid;
      print(" start, end = ", 1, 2);
      printInt(start[idx], 16, 1, 2);
      printInt(end[idx], 16, 0, 2);
      i = (start[idx] + end[idx] + 1) >> 1;
      if (!tstatus) {i = 0; } // Overwrite with 0 if training didn't pass
      if (i < 0) {
        temp |= (-i) << ((bid % 2) * 4);
      } else if (i > 0) {
        temp |= ((1 << 3) | (i)) << ((bid % 2) * 4);
      }
      if (bid%2) {
        print("MR30 <= 0x", 1, 1);
        printInt(temp, 16, 0, 1);
        ddrc_mrw(0x1e, temp, did, bid/2);
        /* Keep MR30 training result in scratchpad for later save */
        writel(DDRCT_RESULT_MR30_ADDR((base_addr == HOST0_DDRC_REG_BASE_ADDR) ? 0 : 1, did, bid/2), temp);
      }
    }
  }
  /* Set magic code to indicate the MR30 result is valid */
  writel(DDRCT_RESULT_MR30_VALID, DDRCT_RESULT_VALID_MAGIC_CODE);

  PRINT_STATUS(tstatus);
}
#endif

#ifdef DDRCT_INCLUDE_CMD_TRAIN_CODE
void ddrc_freq_change(u32_t hid, u32_t go_slow, u32_t reset_dll_divider) {
  u64_t addr = DDRCT_HOST0_PLL_ADDRESS;
  addr += hid * 0x10000;
  u32_t value = readl(addr | 0x14);
  value = (value & (~(1 << 9))) | (go_slow << 9);

  if (go_slow) {
    print("\t freq -> low", 0, 1);
  } else {
    print("\t freq -> high", 0, 1);
  }

  writel(addr | 0x14, value);
  // reset dll/divider
  if (reset_dll_divider) {
    // writel(addr | 0x18, 3);
    //writel(addr | 0x18, 0);
    // reset DLL through init register.
    writel(base_addr | DDRC_INIT_CTL_REG, (1 << 3));
    while(readl(base_addr | DDRC_INIT_CTL_REG) & (1 << 3));
  }
}

u8_t ddrc_compute_pass_fail(u8_t vref_max, u8_t vref_min, train_info_t *tinf, u8_t pass_range) {
   u8_t fail = 0x3;
   u8_t range;
   range = vref_max - vref_min;

   if(range > pass_range) {
     if(vref_min < tinf->vl) {
       fail &= 0x2;
     }
     if(vref_max > tinf->vh) {
       fail &= 0x1;
     }
   }
   return fail;
}

void ddrc_record_min_max(u8_t fail, u8_t dly, train_info_t *tinf) {
   if(!(fail & 0x1)) {
     tinf->tlw[1] = dly;
     if(!(tinf->found_flag & 0x1)){
       tinf->tlw[0] = dly;
       tinf->found_flag |= 1;
     }
   }

   if(!(fail & 0x2)) {
     tinf->thw[1] = dly;
     if(!(tinf->found_flag & 0x2)){
       tinf->thw[0] = dly;
       tinf->found_flag |= 2;
     }
   }
}

u8_t ddrc_compute_mid_dly(train_info_t *tinf) {
  u8_t a, b;
  a = DDRC_MAX(tinf->thw[0], tinf->tlw[0]);
  b = DDRC_MIN(tinf->thw[1], tinf->tlw[1]);
  return (a+b) >> 1;
}

void ddrc_compute_overlap(train_info_t *tinf0, train_info_t *tinf1){
   tinf0->found_flag = tinf0->found_flag & tinf1->found_flag;
   if(tinf0->found_flag == 0x3){
      tinf0->tlw[0] = DDRC_MAX(tinf0->tlw[0], tinf1->tlw[0]);
      tinf0->thw[0] = DDRC_MAX(tinf0->thw[0], tinf1->thw[0]);
      tinf0->tlw[1] = DDRC_MIN(tinf0->tlw[1], tinf1->thw[1]);
      tinf0->thw[1] = DDRC_MIN(tinf0->thw[1], tinf1->thw[1]);
   }
}
// Update vref min/max
void ddrc_update_vref_min_max(u16_t fail, u8_t vref, u8_t *vref_max, u8_t *vref_min, u8_t *pw) {
  u8_t range;
  if (!fail) {
    *vref_max = vref;
    if(!(*pw)){
      *vref_min = vref;
    }
    *pw = 1;
  } else {
    // fail case
    // filter noise at floor
    range = *vref_max - *vref_min;
    if (*pw && range <= 2) {
      *pw = 0; /* reset */
    }
  }
}

// Compute vh/vl
void ddrc_update_vhvl(u8_t vref_max, u8_t vref_min, train_info_t *tinf, u8_t pass_range) {
  u8_t range;
  range = vref_max - vref_min;
  if (range > pass_range) {
    tinf->vl = DDRC_MIN(vref_min, tinf->vl);
    tinf->vh = DDRC_MAX(vref_max, tinf->vh);
  }
}

void ddrc_ca_vref_update(u32_t vref) {
  if(is_lp5) {
    // Drive DM0
    ddrc_uinst(DDRC_UINST_DM0_CTL, 2, 0);
  }
  if(!is_lp5 && vref > 50){
    vref = ((vref - 50) | 0x40);
  }
  // Drive CA vref to DQ[6:0]
  ddrc_uinst(DDRC_UINST_DQD, vref, 0);

  if(is_lp5){
    // LP5 : Trigger toggle DM0
    ddrc_uinst(DDRC_UINST_DM0_CTL, 3, 0);
  } else {
    // LP4 : Trigger toggle DQS
    ddrc_uinst(DDRC_UINST_TOGGLE, 0, 0);
  }

  // Release DQ[6:0]
  ddrc_uinst(DDRC_UINST_DQR, 0, 0);
  ddrc_wait(8); /* vref settle time wait = 250 ns */
}

//command bus training
u32_t ddrc_check_ca_pattern(u32_t cid) {
  u32_t fail = 0;
  for (int i = 0; i < 4; i++) {
    u32_t ca_pattern = 0;
    if (i > 1) ca_pattern = 0xaa;
    if (i%2) ca_pattern = ca_pattern ^ 0xff; // Invert pattern
    ddrc_uinst(DDRC_UINST_CAD, ca_pattern, 0);
    ddrc_wait(1);
    // trigger dq capture
    ddrc_uinst(DDRC_UINST_DQC, 0, 0);
    // check pattern
    u32_t mask = 0x3f;
    if (is_lp5) { mask = 0x7f; }
    ca_pattern = ca_pattern & mask;
    u32_t tmp;
    if (is_lp5) {
      tmp = ((readl(base_addr | DDRC_UINST_RSLT_REG) >> (cid*16)) & mask);
    } else {
      tmp = ((readl(base_addr | DDRC_UINST_RSLT_REG)) >>  (8 + cid*16)) & mask;
    }
    fail |= tmp ^ ca_pattern;
  }
  return fail;
}

void ddrc_odt_enable() {
  if (is_lp5) {
    // fsp1
    ddrc_mrw(16, (mr16 & 0xfc) | (1), 2, 2);
    ddrc_mrw(18, mr18, 2, 2);
    ddrc_mrw(11, mr11_d0, 2, 2);
    ddrc_mrw(11, mr11_d1, 2, 2);
    ddrc_mrw(41, mr41, 2, 2);

    // fsp0
    ddrc_mrw(16, mr16 & 0xfc, 2, 2);
    ddrc_mrw(18, mr18, 2, 2);
    ddrc_mrw(11, mr11_d0, 2, 2);
    ddrc_mrw(11, mr11_d1, 2, 2);
    ddrc_mrw(41, mr41, 2, 2);
  }
}

void ddrc_odt_disable() {
  if (is_lp5) {
    // fsp1
    ddrc_mrw(16, (mr16 & 0xfc) | (1), 2, 2);
    ddrc_mrw(18, mr18 & 0xf8, 2, 2);
    ddrc_mrw(11, mr11_d0 & 0xf7, 2, 2);
    ddrc_mrw(11, mr11_d1 & 0xf7, 2, 2);
    ddrc_mrw(41, mr41 & 0x1f, 2, 2);

    // fsp0
    ddrc_mrw(16, mr16 & 0xfc, 2, 2);
    ddrc_mrw(18, mr18 & 0xf8, 2, 2);
    ddrc_mrw(11, mr11_d0 & 0xf7, 2, 2);
    ddrc_mrw(11, mr11_d1 & 0xf7, 2, 2);
    ddrc_mrw(41, mr41 & 0x1f, 2, 2);
  }
}

// Returns fine delay overflow/underflow status
// 0 - no overflow
// 1 - underflow
// 2 - overflow
u32_t ddrc_ca_calibration(u8_t hid, u8_t did, u8_t cid, train_info_t *tinf,
                         bool_t is_fine) {

  u32_t status = 0;
  u32_t reg0, reg1, reg2;
  // Save register values to be restored later
  if (is_fine) {
    reg0 = readl(base_addr | DDRC_CA_DELAY_F_REG(cid * 4 + did * 2 + 0));
    reg1 = readl(base_addr | DDRC_CA_DELAY_F_REG(cid * 4 + did * 2 + 1));
    reg2 = readl(base_addr | DDRC_CKE_DELAY_F_REG);
  } else {
    reg0 = readl(base_addr | DDRC_CA_DELAY_C_REG);
    reg1 = readl(base_addr | DDRC_CKE_DELAY_C_REG);
    reg2 = 0;
  }

  // For dual rank system,
  // disable WCK ODT and NT-ODT for both ranks before entering into
  // training mode.
  ddrc_odt_disable();

  // Set CID/DID of uinst (both dies)
  writel(base_addr | DDRC_UINST_REG(3), ((IDX2OH(cid)) << 8) | (IDX2OH(3)));

  if(is_lp5){
    // Drive WCK low
    ddrc_uinst(DDRC_UINST_WCK_CTL, 2, 0);
    // Drive DQ7 = 0
    ddrc_uinst(DDRC_UINST_DQ7_CTL, 2, 0);
    // Drive DM0 = 0
    ddrc_uinst(DDRC_UINST_DM0_CTL, 2, 0);
    // Drive valid DQ = 0
    ddrc_uinst(DDRC_UINST_DQD, 0, 0);
    // Set MR16[5:4]=2 (fsp1) to trigger CBT mode, set VRCG=1 (both dies)
    ddrc_mrw(16, (mr16 & 0xcf) | (2<<4), 2, cid);
    // wait 20ns
    ddrc_wait(1);
    // Release wck low
    ddrc_uinst(DDRC_UINST_WCK_CTL, 0, 0);
    // Wait 5ns
    ddrc_wait(1);
    // Drive DQ7 to high
    ddrc_uinst(DDRC_UINST_DQ7_CTL, 3, 0);
  } else {
    // Set MR13[0]=1 to trigger CBT mode, set VRCG=1
    ddrc_mrw(13, mr13 | 1, did, cid);
    // Issue DQS_PD
    ddrc_uinst(DDRC_UINST_DQS0D, 0, 0);
    // Drive DRAM_CKE LOW
    u32_t mask;
    mask = 1 << ((did << 1) + cid);
    ddrc_read_modify_write(DDRC_CKE_REG, 0, mask);
  }

  // Change to high frequency
  ddrc_freq_change(hid, 0, 0);
  u8_t bit_cnt = 1;
  if (is_fine) {
    bit_cnt = is_lp5 ? 7 : 6;
  }

  for (u8_t step = 0; step < 2; step++) {
    for (u8_t dly = DDRC_CA_DELAY_MIN; dly < ((is_lp5 || is_fine) ? DDRC_CA_DELAY_MAX : DDRC_CA_LP4_COARSE_DELAY_MAX); dly += DDRC_CA_DELAY_STEP) {
      // Program delay
      if (is_fine) {
        ddrc_set_dly(did, cid, CA_DELAY_FINE, dly, 0xff);
      } else {
        ddrc_read_modify_write(DDRC_CA_DELAY_C_REG, dly << 8 | dly, 0xff << (cid * 8));
        if (is_lp5) {
          // chip select coarse delay is CKE delay
          ddrc_read_modify_write(DDRC_CKE_DELAY_C_REG, dly << 8 | dly, 0xff << (cid * 8));
        }
      }
      printInt(dly, 16, 1, 2);
      u8_t fail;
      u8_t vref_max[7];
      u8_t vref_min[7];
      u8_t pass_window[7];

      for (u32_t i = 0; i < 7; i++) {
        vref_max[i] = 0;
        vref_min[i] = 0xff;
        pass_window[i] = 0;
      }
      for (u8_t vref = DDRC_CA_VREF_MIN; vref < (is_lp5 ? DDRC_CA_LP5_VREF_MAX : DDRC_CA_LP4_VREF_MAX); vref += DDRC_CA_VREF_STEP) {
        ddrc_ca_vref_update(vref);
        fail = ddrc_check_ca_pattern(cid);
        if (fail) {
          print (".", 1, 2);
        } else {
          print ("*", 1, 2);
        }
        for (u8_t bit = 0; bit < bit_cnt; bit++) {
          ddrc_update_vref_min_max(fail & (1 << bit), vref, &vref_max[bit], &vref_min[bit], &pass_window[bit]);
        }
      }
      print ("", 0, 2);
      for (u8_t bit = 0; bit < bit_cnt; bit++) {
        if(!step) {
          ddrc_update_vhvl(vref_max[bit], vref_min[bit], &tinf[bit], DDRC_CA_VREF_PASS_RANGE);
        }
        else {
          fail = ddrc_compute_pass_fail(vref_max[bit], vref_min[bit], &tinf[bit], DDRC_CA_VREF_PASS_RANGE);
          ddrc_record_min_max(fail, dly, &tinf[bit]);
        }
      }
    }
    if (!step) {
      for (u8_t bit = 0; bit < bit_cnt; bit++) {
        (&tinf[bit])->vl += DDRC_CA_VREF_MARGIN;
        (&tinf[bit])->vh -= DDRC_CA_VREF_MARGIN;
      }
    }

    // If it is fine training and any delay overflow/underflow, update status
    if (step && is_fine) {
      for (u8_t bit = 0; bit < bit_cnt; bit++) {
        if (tinf[bit].found_flag) {
          u8_t tmp = ddrc_compute_mid_dly(&tinf[bit]);
          if (tmp <= DDRC_CA_DELAY_FUOT) { status |= 1; }
          if (tmp >= (((is_lp5 || is_fine)? DDRC_CA_DELAY_MAX : DDRC_CA_LP4_COARSE_DELAY_MAX) - DDRC_CA_DELAY_FUOT)) { status |= 2;}
        }
      }
    }
  }

  // Exit training
  // Restore delays
  if (is_fine) {
    writel(base_addr | DDRC_CA_DELAY_F_REG(cid * 4 + did * 2 + 0), reg0);
    writel(base_addr | DDRC_CA_DELAY_F_REG(cid * 4 + did * 2 + 1), reg1);
    writel(base_addr | DDRC_CKE_DELAY_F_REG, reg2);
  } else {
    writel(base_addr | DDRC_CA_DELAY_C_REG, reg0);
    writel(base_addr | DDRC_CKE_DELAY_C_REG, reg1);
  }

  // Change freq back to low
  ddrc_freq_change(hid, 1, 0);

  if(is_lp5) {
    // Drive DQ7 = 0
    ddrc_uinst(DDRC_UINST_DQ7_CTL, 2, 0);
    ddrc_wait(1);
    // drive wck low
    ddrc_uinst(DDRC_UINST_WCK_CTL, 2, 0);
    ddrc_wait(8); /* vref settle time wait = 250 ns */
    // Drive DM 0
    ddrc_uinst(DDRC_UINST_DM0_CTL, 2, 0);
    // Leave CBT mode (both dies)
    ddrc_mrw(16, mr16, 2, cid);
    // Release DQ7
    ddrc_uinst(DDRC_UINST_DQ7_CTL, 0, 0);
    // Relase DM0
    ddrc_uinst(DDRC_UINST_DM0_CTL, 0, 0);

    // Recover ODT values
    ddrc_odt_enable();

    // Reset CAS effect
    reset_cas_effect();
  } else {
    // Drive DRAM_CKE HIGH
    u32_t mask;
    mask = 1<< ((did << 1) + cid);
    ddrc_read_modify_write(DDRC_CKE_REG, 0xF, mask);
    // Leave CBT mode
    ddrc_mrw(13, mr13, did, cid);
  }

  return status;
}

void ddrc_ca_training(u32_t hid, u32_t cmd_ds)
{
  u8_t status = 0;
  u8_t max_dies = 1;

  if (dual_die_en) { max_dies = 2; }
  u8_t bit_cnt = 1;
  for (u8_t del_type = 0; del_type < 2; del_type++) {
    if ((del_type == 0 && cmd_ds == 2) ||
        (del_type == 1 && cmd_ds == 1)) {
      continue;
    }

    if (del_type == 1) {
      bit_cnt = is_lp5 ? 7 : 6;
    }

    u8_t fine_itr_max = 4;

    for (u8_t cid = 0; cid < (DDRC_NUM_BYTES >> 1); cid += 1) {
      bool_t tstatus = 1;
      train_info_t tinf[2][7];
      for (int i = 0; i < 7; i++) {
        tinf[0][i].found_flag = 0;
        tinf[0][i].thw[0] = 0;
        tinf[0][i].thw[1] = 0;
        tinf[0][i].tlw[0] = 0;
        tinf[0][i].tlw[1] = 0;
        tinf[0][i].vl = 0;
        tinf[0][i].vh = 0;

        tinf[1][i].found_flag = 0;
        tinf[1][i].thw[0] = 0;
        tinf[1][i].thw[1] = 0;
        tinf[1][i].tlw[0] = 0;
        tinf[1][i].tlw[1] = 0;
        tinf[1][i].vl = 0;
        tinf[1][i].vh = 0;
      }

      print(" Cmd training for channel " , 1, 0);
      printInt((del_type << 24) | cid, 16, 1, 0);
      print("", 0, 1);

      if (cmd_ds == 0x3) {
        // set fine delays to be 10
        // We don't update chip select fine delay as part of training
        for (u8_t did = 0; did < max_dies; did++) {
          ddrc_set_dly(did, cid, CA_DELAY_FINE, 0x10, 0xff);
        }
      }

      for (u8_t did = 0; did < max_dies; did++) {
        status = ddrc_ca_calibration(hid, did, cid, &tinf[did][0], del_type);
      }

      // combine found for all bits
      // compute vref as intersection across all bits
      // same delay across dies but different vref
      u8_t delay_found = 3;
      for (u8_t did = 0; did < max_dies; did++) {
        u8_t found_flag = 3;
        u8_t vref = 0;
        u8_t vref_min = 0;
        u8_t vref_max = 0xff;
        for (u8_t bit = 0; bit < bit_cnt; bit++) {
          // Log all info
          ddrc_log((tinf[did][bit].found_flag) |
                   (did) << 2             |
                   (cid) << 4             |
                   (bit) << 8             |
                   (tinf[did][bit].vl << 16) |
                   (tinf[did][bit].vh << 24));
          ddrc_log((tinf[did][bit].tlw[0]) |
                   (tinf[did][bit].tlw[1] << 8) |
                   (tinf[did][bit].thw[0] << 16) |
                   (tinf[did][bit].thw[1] << 24));

          printInt((tinf[did][bit].found_flag) |
                   (did) << 2             |
                   (cid) << 4             |
                   (bit) << 8             |
                   (tinf[did][bit].vl << 16) |
                   (tinf[did][bit].vh << 24), 16, 1, 1);
          printInt((tinf[did][bit].tlw[0]) |
                   (tinf[did][bit].tlw[1] << 8) |
                   (tinf[did][bit].thw[0] << 16) |
                   (tinf[did][bit].thw[1] << 24), 16, 0, 1);

          found_flag &= tinf[did][bit].found_flag;
          // Take intersection of all bits
          if (tinf[did][bit].vl > vref_min) {
            vref_min = tinf[did][bit].vl;
          }
          if (tinf[did][bit].vh < vref_max) {
            vref_max = tinf[did][bit].vh;
          }
          if (did == 1) {
            // Final result will be put onto tinf[0]
            tinf[0][bit].found_flag &= tinf[1][bit].found_flag;
            if (tinf[1][bit].thw[0] > tinf[0][bit].thw[0]) {
              tinf[0][bit].thw[0] = tinf[1][bit].thw[0];
            }
            if (tinf[1][bit].tlw[0] > tinf[0][bit].tlw[0]) {
              tinf[0][bit].tlw[0] = tinf[1][bit].tlw[0];
            }
            if (tinf[1][bit].thw[1] < tinf[0][bit].thw[1]) {
              tinf[0][bit].thw[1] = tinf[1][bit].thw[1];
            }
            if (tinf[1][bit].tlw[1] < tinf[0][bit].tlw[1]) {
              tinf[0][bit].tlw[1] = tinf[1][bit].tlw[1];
            }
          }
        }
        vref = (vref_max + vref_min) >> 1;
        ddrc_log(vref | (cid << 8) | (did << 16) | (found_flag << 24));
        if (found_flag == 3) {
          ddrc_mrw(0xc, vref, did, cid);
          print("vref = ", 1, 1);
          printInt(vref, 16, 1, 1);
          // Also program fsp1 vref
          if(is_lp5){
            ddrc_mrw(16, (mr16 & 0xfc) | (1<<0), did, cid);
            ddrc_mrw(0xc, vref, did, cid);
            ddrc_mrw(16, mr16, did, cid);
          } else {
            if(vref > 50) {
              vref = ((vref - 50) | 0x40);
            }
            ddrc_mrw(13, (mr13 & 0xBF) | (1 << 6), did, cid);
            ddrc_mrw(0xc, vref, did, cid);
            ddrc_mrw(13, mr13, did, cid);
          }
        } else {
          print("vref = --, ", 1, 1);
          tstatus = 0;
        }
        delay_found &= found_flag;
      }

      // Program delays if training succeeds
      u8_t tmp = 0xff;
      print("delay = ", 1, 1);
      for (u8_t bit = 0; bit < bit_cnt; bit++) {
        if (delay_found == 3) {
          tmp = ddrc_compute_mid_dly(&tinf[0][bit]);
          if (del_type == 0) {
            ddrc_set_cacs_coarse_delay(cid, tmp);
          } else {
            ddrc_set_dly(0, cid, CA_DELAY_FINE, tmp, bit);
            ddrc_set_dly(1, cid, CA_DELAY_FINE, tmp, bit);
          }
        } else {
          tstatus = 0;
        }

        // Log all info
        printInt(tmp, 16, 1, 1);
      }
      print("", 0, 1);
      PRINT_STATUS(tstatus);
    }
  }

  // Switch to high frequency
  ddrc_freq_change(hid, 0, 1);

}

#endif

// Write levelling training
#ifdef DDRCT_INCLUDE_WLVL_TRAIN_CODE
u32_t ddrc_wlvl_get_current_phase(u32_t cid) {
  u32_t curr_phase = 0;
  // Toggle WCK
  ddrc_uinst(DDRC_UINST_WCK_CTL, 6, 8);
  // wait for TWL0
  // Capture DQ
  ddrc_uinst(DDRC_UINST_DQC, 1, 0);
  u32_t data = readl(base_addr | DDRC_UINST_RSLT_REG);
  printInt(data, 16, 0, 1);
  data = (data >> (cid * 16)) & 0xffff;

  for (int i = 0; i < 2; i++) {
    if (((data >> (i*8)) & 0xff) == 0xff) {
      curr_phase |= (1 << i);
    }
  }
  return curr_phase;
}

void ddrc_wlvl_train(u32_t wlvl_ds) {
  int tstatus = 1;
  print(" wckLevelling: (dual_die_en, mr18, wlvl_ds) = ", 1, 0);
  printInt((dual_die_en << 16) | (mr18 << 8) | (wlvl_ds), 16, 1, 0);
  print("", 0, 1);

  int did, cid, cstatus;
  u32_t orig_ckd, orig_cad, orig_csd;
  u32_t uinst_wt_data;
  u32_t ckd, ca_cs_dly, prev_phase, curr_phase;
  u32_t wckd[2];
  int i;
  int del_type;
  int all_done;
  int max_itr;
  u32_t orig_dly[4];

  did = 0;

  // Go into training
  // Drive WCK low
  ddrc_uinst(DDRC_UINST_WCK_CTL, 2, 0);
  // MR18[6] = 1 to trigger WLVL leveling mode
  // All channels to go into training
  ddrc_mrw(18, mr18 | (1<<6), did, 2);

  for (cid = 0; cid < 2; cid++) {
    cstatus = 1;
    orig_dly[0] = ddrc_get_dly(did, cid*2+0, WCK_WRDLY_COARSE, 0);
    orig_dly[1] = ddrc_get_dly(did, cid*2+1, WCK_WRDLY_COARSE, 0);
    orig_dly[2] = ddrc_get_dly(did, cid*2+0, WCK_WRDLY_FINE, 0);
    orig_dly[3] = ddrc_get_dly(did, cid*2+1, WCK_WRDLY_FINE, 0);
    orig_ckd = readl(base_addr | DDRC_CK_DELAY_REG);
    orig_cad = readl(base_addr | DDRC_CA_DELAY_C_REG);
    orig_csd = readl(base_addr | DDRC_CKE_DELAY_C_REG);

    // Set CID/DID of uinst
    writel(base_addr | DDRC_UINST_REG(3), ((cid + 1) << 8) | (did + 1));

    // UINST.WT_PRE to odd (during write leveling, design
    // drives WCK half cycle late if WT_PRE is even, so to fix this issue
    // we need WT_PRE to be odd.
    u32_t uinst_wt_data = readl(base_addr | DDRC_UINST_WT_REG);
    writel(base_addr | DDRC_UINST_WT_REG, uinst_wt_data | 1);

    // if training both fine and coarse, set fine to 0
#if 0
    if (wlvl_ds == 3) {
      for (i = 0; i < 2; i++) {
        ddrc_set_dly(0, cid*2 + i, WCK_WRDLY_FINE, 0x0, 0);
        if (dual_die_en)
          ddrc_set_dly(1, cid*2 + i, WCK_WRDLY_FINE, 0x0, 0);
      }
    }
#endif

    for (del_type=0; del_type < 2; del_type++) {
      if ((del_type == 0 && wlvl_ds == 2) ||
          (del_type == 1 && wlvl_ds == 1)) {
        continue;
      }
      // track both bytes together
      // get current ck coarse delay
      ckd = (orig_ckd >> (cid * 16 + 5)) & 0x1f;
      ca_cs_dly = (orig_cad >> (cid * 8)) & 0xff;
      prev_phase = 4; // [0]: byte0, [1]: byte1
      for (i = 0; i < 2; i++) {
        if (!del_type) {
          wckd[i] = ckd; // Start from current ck delay for coarse
        } else {
          wckd[i] = 0x0;
        }
      }

      all_done = 0; // [0]: byte0, [1]: byte1
      max_itr = 32;

      while (all_done != 3) {
        // program delays
        print("delay = 0x", 1, 1);
        printInt((wckd[1] << 16) | wckd[0], 16, 1, 1);
        for (i = 0; i < 2; i++) {
          ddrc_set_dly(0, cid*2+i, del_type ? WCK_WRDLY_FINE : WCK_WRDLY_COARSE, wckd[i], 0);
          ddrc_set_dly(1, cid*2+i, del_type ? WCK_WRDLY_FINE : WCK_WRDLY_COARSE, wckd[i], 0);
        }
        // get current phase
        curr_phase = ddrc_wlvl_get_current_phase(cid);

        if (prev_phase != 4) {
          all_done |= (curr_phase ^ prev_phase);
        }

        if (all_done == 3) {
          // All done
          break;
        }

        prev_phase = curr_phase;
        if (max_itr < 0) {
          print("ERROR: Max iteration of 32 exceeded", 0, 0);
          break;
        }
        max_itr -= 1;

        // Based on current phase, increase/decrease wck delay
        int x = 0;

        for (i = 0; i < 2; i++) {
          if (!(all_done & (1 << i))) {
            if (curr_phase & (1 << i)) {
              if (wckd[i] > 0) {
                wckd[i] -= 1;
              } else {
                if (!del_type) {
                  print ("wck[", 1, 1);
                  printInt(i, 16, 1, 1);
                  print("] underflow", 0, 1);
                  // Increase clock delay and try again
                  x = 1;
                } else {
                  all_done |= (1 << i);
                }
              }
            } else {
              if (wckd[i] < 31) {
                wckd[i] += 1;
              } else {
                if (!del_type) {
                  print ("wck[", 1, 1);
                  printInt(i, 16, 1, 1);
                  print("] overflow", 0, 1);
                  // Decrease clock delay and try again
                  x = -1;
                } else {
                  all_done |= (1 << i);
                }
              }
            }
          }
        }

        // Change CK delay if needed and possible
        int update = 0;
        if (x != 0) {
          if (x > 0) {
            if (ca_cs_dly < (32 - x) && ckd < (32 - x)) {
              update = 1;
              print("ca/cs/ck + by ", 1, 1);
              printInt((cid << 16) | x, 16, 0, 1);
              ca_cs_dly += x;
              ckd += x;
            } else {
              print("ERROR: ck delay overflow", 0, 0);
              tstatus = 0;
              cstatus = 0;
              break;
            }
          } else {
            x = -x;
            if (ca_cs_dly > (x-1) && ckd > (x-1)) {
              update = 1;
              print("ca/cs/ck - by ", 1, 1);
              printInt((cid << 16) | x, 16, 0, 1);
              ca_cs_dly -= x;
              ckd -= x;
            } else {
              print("ERROR: ck delay underflow", 0, 0);
              tstatus = 0;
              cstatus = 0;
              break;
            }
          }
        }

        if (update) {
          ddrc_set_cacs_coarse_delay(cid, ca_cs_dly);
          ddrc_read_modify_write(DDRC_CK_DELAY_REG, (ckd << 21) | (ckd << 5), (0x1f << (5 + cid*16)));
          for (i = 0; i < 2; i++) {
            wckd[i] = ckd; // Start from current ck delay
          }

          // Reset and start over
          all_done = 0;
          max_itr = 32;
          prev_phase = 4;
        }
      }

      // If it is coarse delay and we are going to tune fine delay next
      // Adjust coarse to be one less if curr_phase == 1
      if (!del_type && wlvl_ds == 3) {
        for (int i = 0; i < 2; i++) {
          if (curr_phase & ( 1 << i)) {
            u32_t dly = ddrc_get_dly(0, cid*2 + i, WCK_WRDLY_COARSE, 0);
            if (dly > 0) {
              dly -= 1;
              ddrc_set_dly(0, cid*2 + i, WCK_WRDLY_COARSE, dly, 0);
              ddrc_set_dly(1, cid*2 + i, WCK_WRDLY_COARSE, dly, 0);
            }
          }
        }
      }

      // Keep wck coarse delay to be as small as possible
      // Try to keep wck coarse delay as small as possible
      // even at the cost of less margin for ca/cs delay (up to +2
      // taps).
      if (!del_type) {
        u32_t dly0 = ddrc_get_dly(did, cid*2+0, WCK_WRDLY_COARSE, 0);
        u32_t dly1 = ddrc_get_dly(did, cid*2+1, WCK_WRDLY_COARSE, 0);
        u32_t min_dly = DDRC_MIN(dly0, dly1);
        u32_t aaa = DDRC_MIN((ca_cs_dly+2), ckd);
        // +2 is allowable margin between ca/cs and ck to keep wck delay small.
        if (aaa < min_dly) {
          min_dly = aaa;
        }
        if (min_dly > 0) {
          ddrc_set_dly(0, cid*2+0, WCK_WRDLY_COARSE, dly0 - min_dly, 0);
          ddrc_set_dly(0, cid*2+1, WCK_WRDLY_COARSE, dly1 - min_dly, 0);
          ddrc_set_dly(1, cid*2+0, WCK_WRDLY_COARSE, dly0 - min_dly, 0);
          ddrc_set_dly(1, cid*2+1, WCK_WRDLY_COARSE, dly1 - min_dly, 0);

          if (ca_cs_dly < min_dly) {
            ca_cs_dly = 0;
          } else {
            ca_cs_dly -= min_dly;
          }
          ckd -= min_dly;
          ddrc_set_cacs_coarse_delay(cid, ca_cs_dly);
          ddrc_read_modify_write(DDRC_CK_DELAY_REG, (ckd << 21) | (ckd << 5), (0x1f << (5 + cid*16)));
        }
      }

      // If training failed, revert WCK/CK delays
      if (!cstatus) {
        ddrc_set_dly(0, cid*2+0, WCK_WRDLY_COARSE, orig_dly[0], 0);
        ddrc_set_dly(0, cid*2+1, WCK_WRDLY_COARSE, orig_dly[1], 0);
        ddrc_set_dly(0, cid*2+0, WCK_WRDLY_FINE, orig_dly[2], 0);
        ddrc_set_dly(0, cid*2+1, WCK_WRDLY_FINE, orig_dly[3], 0);
        ddrc_set_dly(1, cid*2+0, WCK_WRDLY_COARSE, orig_dly[0], 0);
        ddrc_set_dly(1, cid*2+1, WCK_WRDLY_COARSE, orig_dly[1], 0);
        ddrc_set_dly(1, cid*2+0, WCK_WRDLY_FINE, orig_dly[2], 0);
        ddrc_set_dly(1, cid*2+1, WCK_WRDLY_FINE, orig_dly[3], 0);
        writel(base_addr | DDRC_CK_DELAY_REG, orig_ckd);
        writel(base_addr | DDRC_CA_DELAY_C_REG, orig_cad);
        writel(base_addr | DDRC_CKE_DELAY_C_REG, orig_csd);
      } else {
        int dly[4];
        dly[0] = ddrc_get_dly(did, cid*2+0, WCK_WRDLY_COARSE, 0);
        dly[1] = ddrc_get_dly(did, cid*2+1, WCK_WRDLY_COARSE, 0);
        dly[2] = ddrc_get_dly(did, cid*2+0, WCK_WRDLY_FINE, 0);
        dly[3] = ddrc_get_dly(did, cid*2+1, WCK_WRDLY_FINE, 0);
        print("New WCK delay (b fine, b coarse, a fine, a coarse) = 0x", 1, 1);
        printInt((dly[3] << 24) | (dly[1] << 16) | (dly[2] << 8) | (dly[0]), 16, 0, 1);
        print("New CK delay = 0x", 1, 1);
        printInt(readl(base_addr | DDRC_CK_DELAY_REG), 16, 0, 1);
        print("New ca/cs delay = 0x", 1, 1);
        printInt(readl(base_addr | DDRC_CKE_DELAY_C_REG), 16, 0, 1);
        ddrc_log((dly[3] << 24) | (dly[1] << 16) | (dly[2] << 8) | (dly[0]));
      }
    }

    // revert the WT_PRE
    writel(base_addr | DDRC_UINST_WT_REG, uinst_wt_data);

  }

  // Exit training
  ddrc_mrw(18, mr18, 2, 2);

  // Reset CAS effect
  reset_cas_effect();

  // trigger RTT
  ddrc_get_rtt(dual_die_en ? 3 : 1);

  PRINT_STATUS(tstatus);
}

#endif

#ifdef DDRCT_INCLUDE_ODT_DS_SHMOO_CODE
void ddrc_mrw_all(u8_t addr, u8_t data) {
  writel(AHOST_DDRC_REG_BASE_ADDR | DDRC_MODE_REG, (data | (addr << 16) | (1 << 24) | (3 << 25) | (3 << 27) | (1 << 31)));
  while(readl(HOST0_DDRC_REG_BASE_ADDR | DDRC_MODE_REG) & (1 << 31));
  while(readl(HOST1_DDRC_REG_BASE_ADDR | DDRC_MODE_REG) & (1 << 31));
}

void ddrc_health_check() {
  int itr_count = 4;
  int dbse_fail_count = 0;
  int rd_sbe[2][4];
  int wr_sbe[2][2];
  int wr_dbe[2][2];
  int i, j;
  u32_t data;

  for (j = 0; j < 2; j++) {
    for (i = 0; i < 4; i++) {
      rd_sbe[j][i] = 0;
      wr_sbe[j][i/2] = 0;
      wr_dbe[j][i/2] = 0;
    }
  }

  for (i = 0; i < itr_count ; i++) {
    // Reset ecc errors
    writel(AHOST_DDRC_REG_BASE_ADDR | 0x3c0, 0x0);
    writel(AHOST_DDRC_REG_BASE_ADDR | 0x3c0, 0x2);

    // start DBSE
    data = readl(DBSE_DEBUG_ADDRESS | 0x24);
    writel(DBSE_DEBUG_ADDRESS | 0x24, data & 0xfffffffe);
    writel(DBSE_DEBUG_ADDRESS | 0x24, data | 1);

    ddrc_wait(100);

    // Wait for DBSE done
    while(!(readl(DBSE_DEBUG_ADDRESS | 0x20) & (1 << 16)));

    // check if BIST failed
    if (readl(DBSE_DEBUG_ADDRESS | 0x20) & (1 << 17)) {
      dbse_fail_count++;
    }

    // Check read ECC
    for (j = 0; j < 4; j++) {
      data = readl(HOST0_DDRC_REG_BASE_ADDR | (0x3c4 + j * 4));
      rd_sbe[0][j] += data;
      data = readl(HOST1_DDRC_REG_BASE_ADDR | (0x3c4 + j * 4));
      rd_sbe[1][j] += data;
    }

    // Check write ECC
    writel(AHOST_DDRC_REG_BASE_ADDR | DDRC_MODE_REG, 0x9a2b0000);
    data = readl(HOST0_DDRC_REG_BASE_ADDR | DDRC_MODE_REG);
    wr_sbe[0][0] = (data & 0x3f);
    if (data & (1 << 7)) {
      wr_dbe[0][0] += 1;
    }
    data >>= 8;
    wr_sbe[0][1] = (data & 0x3f);
    if (data & (1 << 7)) {
      wr_dbe[0][1] += 1;
    }

    data = readl(HOST1_DDRC_REG_BASE_ADDR | DDRC_MODE_REG);
    wr_sbe[1][0] = (data & 0x3f);
    if (data & (1 << 7)) {
      wr_dbe[1][0] += 1;
    }
    data >>= 8;
    wr_sbe[1][1] = (data & 0x3f);
    if (data & (1 << 7)) {
      wr_dbe[1][1] += 1;
    }
  }

  // Change color of print if error
  if (dbse_fail_count != 0) {
    uart_putstr("\033[1;35m", 0, 0);
  } else {
    uart_putstr("\033[1;33m", 0, 0);
  }

  print ("dbse error count = 0x", 1, 0);
  printInt(dbse_fail_count, 16, 1, 0);
  print("h0 rd sbe = (", 1, 0);
  printInt(rd_sbe[0][0], 16, 1, 0);
  printInt(rd_sbe[0][1], 16, 1, 0);
  printInt(rd_sbe[0][2], 16, 1, 0);
  printInt(rd_sbe[0][3], 16, 1, 0);
  print(") ", 1, 0);
  print("h1 rd sbe = (", 1, 0);
  printInt(rd_sbe[1][0], 16, 1, 0);
  printInt(rd_sbe[1][1], 16, 1, 0);
  printInt(rd_sbe[1][2], 16, 1, 0);
  printInt(rd_sbe[1][3], 16, 1, 0);
  print(") ", 1, 0);

  print("wr sbe = (", 1, 0);
  printInt((wr_sbe[0][0]) | (wr_sbe[0][1] << 8) | (wr_sbe[1][0] << 16) | (wr_sbe[1][1] << 24), 16, 1, 0);
  print(") ", 1, 0);
  print("wr dbe = (", 1, 0);
  printInt((wr_dbe[0][0]) | (wr_dbe[0][1] << 8) | (wr_dbe[1][0] << 16) | (wr_dbe[1][1] << 24), 16, 1, 0);
  print(") ", 0, 0);

  uart_putstr("\033[0m"); /* Revert color back to normal */
}

void ddrc_odt_dds_shmoo(bool_t is_write, u32_t params_shmoo_range) {
  u8_t odt, pdds, dds;
  u8_t nt_odt, nt_odt_en;
  int data;

  int odt_range_min = params_shmoo_range & 0xf;
  int odt_range_max = (params_shmoo_range >> 4) & 0xf;
  int dds_range_min = (params_shmoo_range >> 8) & 0xf;
  int dds_range_max = (params_shmoo_range >> 12) & 0xf;
  int pdds_range_min = (params_shmoo_range >> 16) & 0xf;
  int pdds_range_max = (params_shmoo_range >> 20) & 0xf;
  int nt_odt_range_min = (params_shmoo_range >> 24) & 0xf;
  int nt_odt_range_max = (params_shmoo_range >> 28) & 0xf;

  // disable DTTE
  writel(AHOST_DDRC_REG_BASE_ADDR | 0xf8, DTTE_TRAINING_ENABLE_REG);
  writel(AHOST_DDRC_REG_BASE_ADDR | 0xfc, 0);

  do {
    // reset grid status ready (safeguard if it was in observation mode)
    writel(AHOST_DDRC_REG_BASE_ADDR | 0xf8, DTTE_GRID_STAT_READY_REG);
    writel(AHOST_DDRC_REG_BASE_ADDR | 0xfc, 1);

    // clear algorithm error if any
    writel(AHOST_DDRC_REG_BASE_ADDR | 0xf8, DTTE_ALGO_ERROR_REG);
    writel(AHOST_DDRC_REG_BASE_ADDR | 0xfc, 0x10000);

    // wait for done
    data = 0;
    writel(AHOST_DDRC_REG_BASE_ADDR | 0xf8, DTTE_TRAINING_ENABLE_REG);
    data |= readl(HOST0_DDRC_REG_BASE_ADDR | 0xfc);
    data |= readl(HOST1_DDRC_REG_BASE_ADDR | 0xfc);
  } while (!(data & (1<<4)));

  // Write
  if (is_write) {
    print("Starting ODT/PDDS write shmoo, it may take a while", 0, 0);
    int ca_odt = ddrc_mrr(11, 0, 0); /* read ca odt, it is same for all dies/ch/hosts */
    for (pdds = pdds_range_min; pdds <= pdds_range_max; pdds++) {
      for (dds = dds_range_min; dds <= dds_range_max; dds++) {
        // program dds/pdds for host0
        data = readl(HOST0_DDRC_REG_BASE_ADDR | 0x374);
        data = (data & (~((7 << 23) | (7 << 16) | (7 << 7) | (7 << 0)))) | (dds << 23) | (pdds << 16) | (dds << 7) | (pdds);
        writel(HOST0_DDRC_REG_BASE_ADDR | 0x374, data);

        // program dds/pdds for host1
        data = readl(HOST1_DDRC_REG_BASE_ADDR | 0x374);
        data = (data & (~((7 << 23) | (7 << 16) | (7 << 7) | (7 << 0)))) | (dds << 23) | (pdds << 16) | (dds << 7) | (pdds);
        writel(HOST1_DDRC_REG_BASE_ADDR | 0x374, data);

        // Reset the fifo pointers
        writel(AHOST_DDRC_REG_BASE_ADDR | DDRC_UINST_REG(0), DDRC_UINST_RPTR);
        writel(AHOST_DDRC_REG_BASE_ADDR | DDRC_UINST_EN_REG, 1);
        while(readl(HOST0_DDRC_REG_BASE_ADDR | DDRC_UINST_EN_REG) & 1);
        while(readl(HOST1_DDRC_REG_BASE_ADDR | DDRC_UINST_EN_REG) & 1);

        for (nt_odt_en = 0; nt_odt_en < 2; nt_odt_en++) {
          for (odt = odt_range_min; odt <= odt_range_max; odt++) {
            ddrc_mrw_all(11, odt | (nt_odt_en << 3) | (ca_odt << 4)); // program ODT/NT_ODT_ENABLE
            for (nt_odt = nt_odt_range_min; nt_odt <= nt_odt_range_max; nt_odt++) {
              ddrc_mrw_all(41, nt_odt); /* Program NT_ODT */
              print("(pdds, dds, nt_odt_en, odt, nt_odt) = 0x", 1, 0);
              printInt(nt_odt | (odt << 4) | (nt_odt_en << 8) | (dds << 12) | (pdds << 16), 16, 0, 0);
              ddrc_health_check();
            }
          }
        }
      }
    }
  }

  // Read
  if (!is_write) {
    print("Starting ODT/PDDS read shmoo, it may take a while", 0, 0);
    for (pdds = pdds_range_min; pdds <= pdds_range_max; pdds++) {
      ddrc_mrw_all(3, pdds); /* program pdds */
      for (odt = odt_range_min; odt <= odt_range_max; odt++) {
        // program odt for host0
        data = readl(HOST0_DDRC_REG_BASE_ADDR | 0x374);
        data = (data & (~((7 << 19) | (7 << 3)))) | (odt << 19) | (odt << 3);
        writel(HOST0_DDRC_REG_BASE_ADDR | 0x374, data);

        // program odt for host1
        data = readl(HOST1_DDRC_REG_BASE_ADDR | 0x374);
        data = (data & (~((7 << 19) | (7 << 3)))) | (odt << 19) | (odt << 3);
        writel(HOST1_DDRC_REG_BASE_ADDR | 0x374, data);

        // Reset the fifo pointers
        writel(AHOST_DDRC_REG_BASE_ADDR | DDRC_UINST_REG(0), DDRC_UINST_RPTR);
        writel(AHOST_DDRC_REG_BASE_ADDR | DDRC_UINST_EN_REG, 1);
        while(readl(HOST0_DDRC_REG_BASE_ADDR | DDRC_UINST_EN_REG) & 1);
        while(readl(HOST1_DDRC_REG_BASE_ADDR | DDRC_UINST_EN_REG) & 1);

        print("(pdds, odt) = 0x", 1, 0);
        printInt((pdds << 8) | odt, 16, 0, 0);
        ddrc_health_check();
      }
    }
  }
}

#endif

//#define DDRC_DETAIL_PLOT
#if defined(DDRC_DETAIL_PLOT)
#define DDRC_PLOT_EXEC(x)    x
#else
#define DDRC_PLOT_EXEC(x)
#endif

u64_t plot_log_ptr = DDRCT_EXEC_SPACE + 0x100000UL;
void ddrc_plot_reset(void) {
    plot_log_ptr = DDRCT_EXEC_SPACE + 0x100000UL;
}
void ddrc_plot_set32(u32_t data) {
  if (plot_log_ptr < (DDRCT_EXEC_SPACE+0x200000UL)) {
    writel(plot_log_ptr, data);
    plot_log_ptr += 4;
  }
}
u32_t ddrc_plot_get32(void) {
  if (plot_log_ptr < (DDRCT_EXEC_SPACE+0x200000UL)) {
    plot_log_ptr += 4;
  }
  return readl(plot_log_ptr - 4);
}
void ddrc_plot_set64(u64_t data) {
  if (plot_log_ptr < (DDRCT_EXEC_SPACE+0x200000UL)) {
    writeq(plot_log_ptr, data);
    plot_log_ptr += 8;
  }
}
u64_t ddrc_plot_get64(void) {
  if (plot_log_ptr < (DDRCT_EXEC_SPACE+0x200000UL)) {
    plot_log_ptr += 8;
  }
  return readq(plot_log_ptr - 8);
}
void ddrc_plot_set(bool_t track_bits, u64_t fail) {
    if (track_bits) {
        ddrc_plot_set64(fail);
    } else {
        ddrc_plot_set32((u32_t) fail & 0xffffffff);
    }
}

void ddrc_plot_show(bool_t is_2step, u8_t min_delay, u8_t max_delay, u8_t delay_step, u8_t min_vref, u8_t max_vref, u8_t vref_step, bool_t track_bits) {
#define DDRC_DETAIL_PLOT_DEBUG    1
    u64_t fail;
    int bit_start, bit_end;

    if (track_bits) {
        bit_start = 0;
        bit_end = 39;
    } else {
        bit_start = 36;
        bit_end = 39;
    }
    for (int i = bit_start; i <= bit_end; i++) {
      print("Detail plot ", 1, DDRC_DETAIL_PLOT_DEBUG);
      printInt(i, 16, 1, DDRC_DETAIL_PLOT_DEBUG);
      print(":", 0, DDRC_DETAIL_PLOT_DEBUG);
      ddrc_plot_reset();
      for (u8_t step = 0; step < (is_2step ? 2 : 1); step++) {
        for (u8_t dly = min_delay; dly <= max_delay; dly += delay_step) {
          printInt(dly, 16, 1, DDRC_DETAIL_PLOT_DEBUG);
          print(":", 1, DDRC_DETAIL_PLOT_DEBUG);
          for (int vref = max_vref; vref >= min_vref; vref -= vref_step) {
            if (1/*track_bits*/) {
                fail = ddrc_plot_get64();
            } else {
                fail = ddrc_plot_get32();
            }
            if (fail & (1ULL << i)) {
              print(".", 1, DDRC_DETAIL_PLOT_DEBUG);
            } else {
              print("*", 1, DDRC_DETAIL_PLOT_DEBUG);
            }
          }
          print("", 0, DDRC_DETAIL_PLOT_DEBUG);
        }
      }
    }
}

u32_t ddrc_offset_delay(u32_t delay, int offset)
{
    int delay_tmp;
    u32_t target_delay;

    if (offset == 0) {
        return delay;
    }

    /* Apply offset and limit delay in the boundary */
    delay_tmp = delay + offset;
    if (delay_tmp > 31) {
        delay_tmp = 31;
    } else if (delay_tmp < 0) {
        delay_tmp = 0;
    }
    target_delay = delay_tmp;
    return target_delay;
}

int ddrc_get_delay_offset(u32_t bid, enum delay_id_t delay_id)
{
    int coarse_delay_offset = 0, fine_delay_offset = 0;

    /* Get fine delay offset */
    if ((delay_id == DQ_RDDLY_COARSE) || (delay_id == DQ_RDDLY_FINE)) {
        fine_delay_offset = (read_delay_offset[(base_addr == HOST0_DDRC_REG_BASE_ADDR) ? 0 : 1] >> (bid*8)) & 0xff;
    } else if ((delay_id == DQ_WRDLY_COARSE) || (delay_id == DQ_WRDLY_FINE)) {
        fine_delay_offset = (write_delay_offset[(base_addr == HOST0_DDRC_REG_BASE_ADDR) ? 0 : 1] >> (bid*8)) & 0xff;
    }
    if (fine_delay_offset != 0) {
        if (fine_delay_offset & 0x80) {
            fine_delay_offset |= 0xffffff00; /* Keep the negative from s8_t to s32_t */
        }
        /* Calculate the coarse delay offset if the fine delay offset is big */
        if ((fine_delay_offset >= 12) || (fine_delay_offset <= -12)) {
            coarse_delay_offset = fine_delay_offset / 12;
            fine_delay_offset -= (coarse_delay_offset * 12);
        }
    }

    if ((delay_id == DQ_RDDLY_COARSE) || (delay_id == DQ_WRDLY_COARSE)) {
        return coarse_delay_offset;
    } else if ((delay_id == DQ_RDDLY_FINE) || (delay_id == DQ_WRDLY_FINE)) {
        return fine_delay_offset;
    } else {
        return 0;
    }
}

void ddrc_apply_delay_offset(u32_t did, enum delay_id_t delay_id)
{
    u32_t track_bits = 0;
    u32_t bid, i;
    u32_t delay_old, delay_new;
    int delay_offset;

    for (bid = 0; bid < DDRC_NUM_BYTES; bid++) {
        delay_offset = ddrc_get_delay_offset(bid, delay_id);
        if (delay_offset != 0) {
            /* Print offset */
            print("   offset", 1, 1);
            print((bid == 0) ? "0=" : (bid == 1) ? "1=" : (bid == 2) ? "2=" : "3=" , 1, 1);
            printInt(delay_offset & 0xff, 16, 1, 1);

            // For these ids, we have per bit delay.
            if (delay_id == DQ_RDDLY_FINE || delay_id == DQ_WRDLY_FINE) {
                track_bits = 1;
            }
            print("delays = ", 1, 1);
            for (i = 0; i < 8; i++) {
                /* Update dq delay */
                delay_old = ddrc_get_dly(did, bid, delay_id, i);
                delay_new = ddrc_offset_delay(delay_old, delay_offset);
                ddrc_set_dly(did, bid, delay_id, delay_new, i);
                printInt(delay_new, 16, track_bits, 1);
                if (i == 0 && !track_bits) {
                    break;
                }
            }
            if (track_bits) {
                /* Update dm delay */
                delay_old = ddrc_get_dly(did, bid, delay_id == DQ_RDDLY_FINE ? DM_RDDLY_FINE : DM_WRDLY_FINE, 0);
                delay_new = ddrc_offset_delay(delay_old, delay_offset);
                ddrc_set_dly(did, bid, delay_id == DQ_RDDLY_FINE ? DM_RDDLY_FINE : DM_WRDLY_FINE, delay_new, 0);
                printInt(delay_new, 16, 0, 1);
            }
        }
    }
}

#ifdef DDRCT_INCLUDE_RW_TRAIN_CODE
/* This is data check function for both channels */
u32_t ddrc_dq_check(u32_t is_inverted) {
  u32_t fail = 0xffffffff;
  u32_t read_pat0, read_pat1, wrt_pat;
  u32_t tmp_rslt0 = 0;
  u32_t tmp_rslt1 = 0;

  for (int i = 0; i < 8; i++) {
    read_pat0 = readl(base_addr | DDRC_MPC_RDATA_REG(0 * 8 + i));
    read_pat1 = readl(base_addr | DDRC_MPC_RDATA_REG(1 * 8 + i));
    wrt_pat = readl(base_addr | DDRC_MPC_WDATA_REG(i));
    if (is_inverted) {
      wrt_pat = wrt_pat ^ 0xffffffff;
    }
    tmp_rslt0 |= read_pat0 ^ wrt_pat;
    tmp_rslt1 |= read_pat1 ^ wrt_pat;
  }
  tmp_rslt0 = (tmp_rslt0 >> 16 | tmp_rslt0);
  tmp_rslt1 = (tmp_rslt1 >> 16 | tmp_rslt1);
  fail &= (tmp_rslt0 | (0xffff << 16));
  fail &= ((tmp_rslt1  << 16) | 0xffff);
  return fail;
}

/* This is dm check function for both channels */
u32_t ddrc_dm_check(void) {
  u32_t fail = 0xf;
  u32_t tmp_rslt, mask = 0x55555555;
  u32_t wrt_pat = readl(base_addr | DDRC_MPC_WDATA_REG(8));
  u32_t read_pat = 0;

  for (int i = 0; i < 4; i++) {
    if (i%2 == 0) {
      read_pat = readl(base_addr | DDRC_MPC_RMASK_REG( i / 2));
    }
    tmp_rslt = (read_pat ^ wrt_pat) & mask;
    if (!tmp_rslt) {
      fail &= ~(1 << i);
    }
    mask = mask ^ 0xffffffff;
  }
  return fail;
}

#ifdef DDRCT_SI_DATA_PATTERN
u64_t ddrc_check(u32_t is_read) {
  u64_t fail = 0;
  u32_t temp;
  for (int loop = 0; loop < 8; loop++) {
    u32_t data[9];
    u32_t mask = 0;
    // write inversion pattern
    if (is_read) {
      mask = (1 << loop) | (0x8000 >> loop);
      ddrc_mrw(is_lp5 ? 31 : 15, mask & 0xff, 2, 2);
      ddrc_mrw(is_lp5 ? 32 : 20, (mask >> 8) & 0xff, 2, 2);
      data[8] = 0;
    } else {
      data[8] = write_data_pattern[loop][8];
    }
    for (int i = 0; i < 8; i++) {
      if (is_read) {
        if (read_data_pattern & (1 << (2*i))) {
          data[i] = mask ^ 0xffff;
          data[8] |= (0x3 << (4*i));
        } else {
          data[i] = mask;
        }

        if (read_data_pattern & (1 << (2*i+1))) {
          data[i] |= (mask ^ 0xffff) << 16;
          data[8] |= (0x3 << (4*i+2));
        } else {
          data[i] |= (mask << 16);
        }
      } else {
        data[i] = write_data_pattern[loop][i];
      }
    }

    // write data
    for (int i = 0; i < 9; i++) {
      writel(base_addr | DDRC_MPC_WDATA_REG(i), data[i]);
    }

    // issue mpc
    ddrc_uinst(is_read ? DDRC_UINST_MPC_RD_DQ : DDRC_UINST_MPC_WRRD_FIFO,
               DDRC_UINST_BO, DDRC_UINST_N_M1);
    ddrc_uinst(DDRC_UINST_RPTR, 0, 0); /* reset FIFO pointer */

    // check
    fail |= ((u64_t) ddrc_dm_check() << 32) | ddrc_dq_check(0);
    temp = ddrc_dq_check(0);
    for (int i = 0; i < DDRC_NUM_BYTES; i++) {
      if (temp & (0xff << (i * 8))) {
        fail |= (1ULL << (i + 36));
      }
    }
    fail |= ((u64_t) ddrc_dm_check() << 36);
  }

  return fail;
}

#else
u64_t ddrc_check(u32_t is_read) {
  u64_t fail = 0;
  u32_t data;


  // issue mpc
  ddrc_uinst(is_read ? DDRC_UINST_MPC_RD_DQ : DDRC_UINST_MPC_WRRD_FIFO,
             DDRC_UINST_BO, DDRC_UINST_N_M1);
  ddrc_uinst(DDRC_UINST_RPTR, 0, 0); /* reset FIFO pointer */
  fail |= ((u64_t) ddrc_dm_check() << 32) | ddrc_dq_check(0);
  data = ddrc_dq_check(0);
  for (int i = 0; i < DDRC_NUM_BYTES; i++) {
    if (data & (0xff << (i * 8))) {
      fail |= (1ULL << (i + 36));
    }
    fail |= ((u64_t) ddrc_dm_check() << 36);
  }
  // Invert the pattern and check again
  if (is_read) {
    ddrc_mrw(is_lp5 ? 31 : 15, 0xaa, 2, 2);
    ddrc_mrw(is_lp5 ? 32 : 20, 0xaa, 2, 2);
    ddrc_uinst(DDRC_UINST_MPC_RD_DQ, DDRC_UINST_BO, DDRC_UINST_N_M1);
    ddrc_uinst(DDRC_UINST_RPTR, 0, 0); /* reset FIFO pointer */

    fail |= ((u64_t) ddrc_dm_check() << 32) | ddrc_dq_check(0);
    data = ddrc_dq_check(0);
    for (int i = 0; i < DDRC_NUM_BYTES; i++) {
      if (data & (0xff << (i * 8))) {
        fail |= (1ULL << (i + 36));
      }
      fail |= ((u64_t) ddrc_dm_check() << 36);
    }

    ddrc_mrw(is_lp5 ? 31 : 15, 0x55, 2, 2);
    ddrc_mrw(is_lp5 ? 32 : 20, 0x55, 2, 2);
  } else {
    for (int i = 0; i < 9; i++) {
      data = readl(base_addr | DDRC_MPC_WDATA_REG(i));
      writel(base_addr | DDRC_MPC_WDATA_REG(i), data ^ 0xffffffff);
    }
    ddrc_uinst(DDRC_UINST_MPC_WRRD_FIFO,  DDRC_UINST_BO, DDRC_UINST_N_M1);
    ddrc_uinst(DDRC_UINST_RPTR, 0, 0); /* reset FIFO pointer */

    fail |= ((u64_t) ddrc_dm_check() << 32) | ddrc_dq_check(0);
    data = ddrc_dq_check(0);
    for (int i = 0; i < DDRC_NUM_BYTES; i++) {
      if (data & (0xff << (i * 8))) {
        fail |= (1ULL << (i + 36));
      }
      fail |= ((u64_t) ddrc_dm_check() << 36);
    }

    for (int i = 0; i < 9; i++) {
      data = readl(base_addr | DDRC_MPC_WDATA_REG(i));
      writel(base_addr | DDRC_MPC_WDATA_REG(i), data ^ 0xffffffff);
    }
  }
  return fail;
}
#endif

/* This is general shmoo training code. Shmoo give delay_id for all
   bits and select the delay that gives maximum vref range. Average if
   same max range across delay.
*/
void ddrc_rw_train(u32_t did,
                   u32_t is_read,
                   u32_t vref_ids,
                   enum delay_id_t delay_id,
                   u32_t is_2step
                   ) {

  // Total 32 bits + 4 dm bits + 4 byte wise status
  bit_train_info_t train_info[40];
  u64_t fail;
  u32_t track_bits = 0;
  bool_t tstatus = 1;
  u8_t vref_max[40] = {0};
  u8_t vref_min[40] = {0};
  u32_t temp;
  u32_t sum_vref = 0;
  int index = 0;
  u32_t is_shared_vref = (!is_lp5 && !is_read);
  int vref_step = 1;
  DDRC_PLOT_EXEC(int max_vref = 0);

  // For these ids, we have per bit delay.
  if (delay_id == DQ_RDDLY_FINE || delay_id == DQ_WRDLY_FINE) {
    track_bits = 1;
  }

  DDRC_PLOT_EXEC(ddrc_plot_reset());
  for (int step = 0; step < (is_2step ? 2 : 1); step++) {
    for (int i = 0; i < 40; i++) {
      // 2 steps :
      // step 0 : ovl/ovh as tinf->vl/vh
      //
      // step 1 : ovl/ovh copied to vref_max/vref_min
      //          ovl/ovh will be used as tlw[0]/tlw[1]
      //          odly/oht will be used as thw[0]/thw[1]
      train_info[i].ovl = 0xff;
      train_info[i].ovh = 0;
      train_info[i].odly = 0xff;
      train_info[i].oht = 0;
    }

    for (int dly = 0; dly < 32; dly++) {
      int vref = 63;
      temp = 0ULL;
      printInt(dly, 16, 1, 2);
      print(":", 1, 2);
      /* program dly */
      for (int bid = 0; bid < DDRC_NUM_BYTES; bid++) {
        ddrc_set_dly(did, bid, delay_id, dly, 0xff);
        /* program DM/DQS related delay when applicable */
        if (delay_id == DQ_RDDLY_FINE) {
          ddrc_set_dly(did, bid, DM_RDDLY_FINE, dly, 0xff);
        }
        if (delay_id == DQ_WRDLY_FINE) {
          ddrc_set_dly(did, bid, DM_WRDLY_FINE, dly, 0xff);
        }
      }

      /* reset current vl/vh/state */
      for (int i = 0; i < 40; i++) {
        train_info[i].cvl = 0x0;
        train_info[i].cvh = 0x0;
      }

      if (!is_read) {
        vref = is_lp5 ? 127 : 79;
        vref_step = 2;
      }

      DDRC_PLOT_EXEC(max_vref = vref);
      while (vref >= 0) {
        /* program vref */
        for (int bid = 0; bid < DDRC_NUM_BYTES; bid++) {
          ddrc_vref_update(did, bid, vref, vref_ids);
        }
        ddrc_wait(8); /* vref settle time wait = 250 ns */
        fail = ddrc_check(is_read);

        if (fail & (1ULL << ((debug_level >> (8 * (track_bits + 1))) & 0xff))) {
          print(".", 1, 2);
        } else {
          print("*", 1, 2);
        }
        DDRC_PLOT_EXEC(ddrc_plot_set(1, fail));

        // update vl/vh
        for (int i = 0; i < 40; i++) {
          if (temp & (1ULL << i)) {
            continue;
          }

          if (!(fail & (1ULL << i))) {
            train_info[i].cvl = vref;
            if (train_info[i].cvh == 0) {
              train_info[i].cvh = vref;
            }
          } else {
            // filter noise
            if (train_info[i].cvh != 0) {
              if ((train_info[i].cvh - train_info[i].cvl + vref_step) >= vref_pass_range) {
                temp |= (1ULL << i);
              } else {
                train_info[i].cvh = 0;
              }
            }
          }
        }
        vref = vref - vref_step;
      }
      print("", 0, 2);
      if (step) {
        for (int i = 0; i < 40; i++) {
          // Compute pass/fail
          temp = 0x3;
          if (train_info[i].cvl <= vref_min[i]) {
            temp &= 0x2;
          }
          if (train_info[i].cvh >= vref_max[i]) {
            temp &= 0x1;
          }

          // update tlw
          if(!(temp & 0x1)) {
            train_info[i].tlw_max = dly;
            if (train_info[i].tlw_min == 0xff) {
              train_info[i].tlw_min = dly;
            }
          }

          // update thw
          if(!(temp & 0x2)) {
            train_info[i].thw_max = dly;
            if (train_info[i].thw_min == 0xff) {
              train_info[i].thw_min = dly;
            }
          }
        }
      } else {
        // keep track of optimal delay and range
        for (int i = 0; i < 40; i++) {
          if (train_info[i].cvh != 0x0) {
            temp = (train_info[i].cvh - train_info[i].cvl + vref_step);
            if (!is_2step) {
              if (temp > train_info[i].oht) {
                train_info[i].oht = temp;
                train_info[i].odly = dly;
                train_info[i].ovh = train_info[i].cvh;
                train_info[i].ovl = train_info[i].cvl;
              } else if (temp == train_info[i].oht) {
                train_info[i].odly = (train_info[i].odly + dly + 1) / 2;
              }
            } else {
              if (temp > 0) {
                train_info[i].ovh =  DDRC_MAX(train_info[i].cvh, train_info[i].ovh);
                train_info[i].ovl =  DDRC_MIN(train_info[i].cvl, train_info[i].ovl);
              }
            }
          }
        }
      }
    }

    // Adjust vh/vl for next round and copy to vref_min/max
    // track_bits = is_fine
    if(is_2step) {
      for (int i = 0; i < 40; i++) {
        if (!step) {
          vref_min[i]  = train_info[i].ovl + ((vref_margin[!is_read] >> (track_bits * 16)) & 0xff);
          vref_max[i]  = train_info[i].ovh - ((vref_margin[!is_read] >> (track_bits * 16 + 8)) & 0xff);
        } else {
          // Overwrite these variables so that programming delays/vref
          // is same between those two algorithms.
          train_info[i].odly = (DDRC_MAX(train_info[i].thw_min, train_info[i].tlw_min) +
                                DDRC_MIN(train_info[i].thw_max, train_info[i].tlw_max) + 1) >> 1;
          // Do this after delay computation
          train_info[i].ovl  = vref_min[i];
          train_info[i].ovh  = vref_max[i];
        }
      }
    }
  }

  // Program trained delays and vref
  // vh/vl is intersection across all bits
  // vref = (vh + vl) / 2
  for (int b = 0; b < DDRC_NUM_BYTES; b++) {
    sum_vref += train_info[b + 36].ovl + train_info[b + 36].ovh;
    print("delays = ", 1, 1);
    for (int i = 0; i < 8; i++) {
      /* program delay */
      index = track_bits ? (b * 8 + i) : (36 + b);
      ddrc_set_dly(did, b, delay_id, train_info[index].odly, i);
      printInt(train_info[index].odly, 16, 1, 1);
      ddrc_log(train_info[index].odly);
      if ((!is_2step && (train_info[index].oht == 0)) ||
          ( is_2step && (train_info[index].odly == 0x80))) {
        print("", 0, 1);
        print("failed bit id = 0x", 1, 1);
        printInt(b * 8 + i, 16, 0, 1);
        tstatus = 0;
      }
      if (i == 0 && !track_bits) {
        break;
      }
    }
    if (track_bits) {
      /* program dm delay */
      ddrc_set_dly(did, b, delay_id == DQ_RDDLY_FINE ? DM_RDDLY_FINE : DM_WRDLY_FINE,
                   train_info[32 + b].odly, 0);
      printInt(train_info[32 + b].odly, 16, 1, 1);
      ddrc_log(train_info[32 + b].odly);

      if ((!is_2step && (train_info[32 + b].oht == 0)) ||
          ( is_2step && (train_info[32 + b].odly == 0x80))) {
        print("", 0, 1);
        print("failed bit id = 0x", 1, 1);
        printInt(32 + b, 16, 0, 1);
        tstatus = 0;
      }
    }

    if (!is_shared_vref || (b%2) == 1) {
      int offset = 0;
      u32_t avg_vref = sum_vref >> (is_shared_vref + 1);
      u32_t offset_source = (base_addr == HOST0_DDRC_REG_BASE_ADDR) ? vref_offsets : vref_offsets >> 16;
      /* extract and extend offset to signed value */
      if (is_read) {
        __asm__ __volatile__("sbfx %0, %1, #0, #8\n\t" : "=&r"(offset) : "r"(offset_source));
      } else {
        __asm__ __volatile__("sbfx %0, %1, #8, #8\n\t" : "=&r"(offset) : "r"(offset_source));
      }
      avg_vref += offset;
      if (offset != 0) {
          print("offset=0x", 1, 1);
          printInt(offset, 16, 1, 1);
      }
      print("vref = 0x", 1, 1);
      printInt(avg_vref, 16, 0, 1);
      ddrc_log(avg_vref);
      ddrc_vref_update(did, b, avg_vref, vref_ids);
      sum_vref = 0;
    }
  }
  ddrc_wait(8); /* vref settle time wait = 250 ns */

  PRINT_STATUS(tstatus);
  DDRC_PLOT_EXEC(ddrc_plot_show(is_2step, 0, 31, 1, 0, max_vref, vref_step, track_bits));
}

u32_t check_fine_delay(u32_t did, enum delay_id_t delay_id) {
  // delay_id = DQ_WRDLY_COARSE or DQS_WRDLY_COARSE
  // Read Refresh multiplier and find out
  // temp. category (Low, Medium, High)
  // Low: Keep fine range between (4-20)
  // Medium: Keep fine range between (8-24)
  // High: Keep fine range between (12-32)
  int min_fine_delay = 8;
  int max_fine_delay = 24;
  int temp = ddrc_mrr(4, did, 2) & 0x1f;
  int retry = 0;
  temp = temp < 4 ? -1 : temp > 6 ? 1 : 0;
  min_fine_delay += temp * 4;
  max_fine_delay += temp * 4;
  // Remove DM from calculation because it is generally outlier.
  for (int bid = 0; bid < DDRC_NUM_BYTES; bid++) {
    int coarse_delay = ddrc_get_dly(did, bid, delay_id, 0);
    temp = 0;
    for (int i = 0; i < (delay_id == DQ_WRDLY_COARSE ? 8 : 1); i++) {
      temp += ddrc_get_dly(did, bid, i == 8 ? DM_WRDLY_FINE :
                           (delay_id == DQ_WRDLY_COARSE ? DQ_WRDLY_FINE : DQS_WRDLY_FINE), i);
    }
    if (delay_id == DQ_WRDLY_COARSE) {
      temp /= 8;
    }
    if (temp < min_fine_delay && coarse_delay > 0) {
      temp = -1;
    } else if (temp > max_fine_delay && coarse_delay < 31) {
      temp = 1;
    } else {
      temp = 0;
    }
    if (temp != 0) {
      print("Coarse ", 1, 1);
      printInt(bid, 16, 1, 1);
      print("adjust ", 1, 1);
      printInt(temp & 0xff, 16, 0, 1);
      retry = 1;
      ddrc_set_dly(did, bid, delay_id, coarse_delay + temp, 0);
    }
  }
  return retry;
}

// top level training function
// step_select_oh[0] = dq read delay
// step_select_oh[1] = DLL SEL0
// step_select_oh[2] = dq write delay
// step_select_oh[3] = dqs write delay (LP5 only)

void ddrc_train_all() {
  u32_t gate_reg;
  u32_t retry;

  // Reset CAS effect
  reset_cas_effect();

  // trigger RTT
  ddrc_get_rtt(dual_die_en ? 3 : 1);

  // DCM training
  if (is_lp5) {
    ddrc_dcm_training(dual_die_en ? 3 : 1);
  }

  // WFF2RFF time (program in UINST.OP[0])
  // LP4: WL + BL/2+1 + RU(tWTR)
  // LP5: Max(3nCK, WL+BL/n max-RL +Max[RU(10ns/tCK),
  // 4nCK)])
  // @ NT-ODT disabled (MR11 OP[3]=0B)
  // Max(3nCK, WL+BL/n max-RL+RU[tODToff(max)/tCK]
  // +Max[RU(10ns/tCK), 4nCK)])
  // @ NT-ODT enabled (MR11 OP[3]=1B)
  // Lp4: 18 + 8 + 1 + 22 = 49
  // Lp5: 9 + 8 - 16 + 20 + 10 = 31
  // We have simulated with 64 in verification
  writel(base_addr | DDRC_UINST_REG(1), 64);

  // save orignal dqs gate mode to be restored later
  gate_reg = readl(base_addr | DDRC_GATE_REG);

  for (int did = 0; did < (dual_die_en ? 2 : 1); did++) {

    /* Enable both channels */
    writel(base_addr | DDRC_UINST_REG(3), 0x3 << 8 | (1 << did));

    // Go to enhanced mode/read preamble mode
    if (is_lp5) {
      ddrc_mrw(0x2e, 0x1, did, 2);
    } else {
      ddrc_mrw(0xd, 0xa, did, 2);
    }

    // Set gate into always on mode
    ddrc_read_modify_write(DDRC_GATE_REG, 1 << 11, 0x3 << 11);

    // Set dq fine delay and DLL sel0 to middle
    for (int bid = 0; bid < DDRC_NUM_BYTES; bid++) {
      ddrc_set_dly(did, bid, DQ_RDDLY_FINE, 0x10, 0xff);
      ddrc_set_dly(did, bid, DM_RDDLY_FINE, 0x10, 0xff);
#if DISABLE_LP4_WRITE_TRAIN
      if (is_lp5) {
#endif
      ddrc_set_dly(did, bid, DQ_WRDLY_FINE, 0x10, 0xff);
      ddrc_set_dly(did, bid, DM_WRDLY_FINE, 0x10, 0xff);
#if DISABLE_LP4_WRITE_TRAIN
      }
#endif
      if (is_lp5) {
        ddrc_set_dly(did, bid, DLL_CTRL_SEL0, 0x10, 0);
        ddrc_set_dly(did, bid, DQS_RDDLY_FINE, 0x0, 0);
        ddrc_set_dly(did, bid, DQS_WRDLY_FINE, 0x10, 0xff);
      }
    }

    print(" DQ read coarse delay shmoo", 1, 0);
    print("", 0, 1);
    ddrc_rw_train(did, 1, is_lp5 ? 3 : 2, DQ_RDDLY_COARSE, 0);
    ddrc_apply_delay_offset(did, DQ_RDDLY_COARSE);

    print(" DQ read fine delay shmoo", 1, 0);
    print("", 0, 1);
    ddrc_rw_train(did, 1, is_lp5 ? 3 : 2, DQ_RDDLY_FINE,
                  step_select_oh & (1 << 0));
    ddrc_apply_delay_offset(did, DQ_RDDLY_FINE);

    if (is_lp5) {
      print(" DQS RDDLY fine delay shmoo", 1, 0);
      print("", 0, 1);
      ddrc_rw_train(did, 1, is_lp5 ? 3 : 1, DQS_RDDLY_FINE, 0);

      print(" DLL SEL0 delay shmoo", 1, 0);
      print("", 0, 1);
      ddrc_rw_train(did, 1, is_lp5 ? 3 : 1, DLL_CTRL_SEL0, 0);
    }

    // RTT
    // Recover the gate mode (This step is needed otherwise RTT will
    // hang).
    writel(base_addr | DDRC_GATE_REG, gate_reg);
    ddrc_uinst(DDRC_UINST_RPTR, 0, 0); /* Reset fifo pointers */

    ddrc_get_rtt(1 << did);

#if DISABLE_LP4_WRITE_TRAIN
    if (is_lp5) {
#endif
    print(" DQ write coarse delay shmoo", 1, 0);
    print("", 0, 1);
    ddrc_rw_train(did, 0, 4, DQ_WRDLY_COARSE, 0);
    ddrc_apply_delay_offset(did, DQ_WRDLY_COARSE);

    print(" DQ write fine delay shmoo", 1, 0);
    print("", 0, 1);
    ddrc_rw_train(did, 0, 4, DQ_WRDLY_FINE,
                  step_select_oh & (1 << 2));

    retry = check_fine_delay(did, DQ_WRDLY_COARSE);
    if (retry) {
      print(" DQ write fine delay shmoo - again", 1, 0);
      print("", 0, 1);
      ddrc_rw_train(did, 0, 4, DQ_WRDLY_FINE,
                    step_select_oh & (1 << 2));
    }
    ddrc_apply_delay_offset(did, DQ_WRDLY_FINE);
#if DISABLE_LP4_WRITE_TRAIN
    }
#endif

    if (is_lp5 && tr_link_ecc_en) {
      /* This is for write link ECC */
      ddrc_read_modify_write(DDRC_DRIVE_ECC_REG, 1 << 2, 1 << 2);
      ddrc_mrw(0x2e, 0x4, did, 2);

      print(" DQS write coarse delay shmoo", 1, 0);
      print("", 0, 1);
      ddrc_rw_train(did, 0, 4, DQS_WRDLY_COARSE, 0);

      print(" DQS write fine delay shmoo", 1, 0);
      print("", 0, 1);
      ddrc_rw_train(did, 0, 4, DQS_WRDLY_FINE,
                    step_select_oh & (1 << 3));

      retry = check_fine_delay(did, DQS_WRDLY_COARSE);
      if (retry) {
        print(" DQS write fine delay shmoo - again", 1, 0);
        print("", 0, 1);
        ddrc_rw_train(did, 0, 4, DQS_WRDLY_FINE,
                      step_select_oh & (1 << 3));
      }

      ddrc_read_modify_write(DDRC_DRIVE_ECC_REG, 0 << 2, 1 << 2);
      ddrc_mrw(0x2e, 0x0, did, 2);
    }
  }
}

#endif



