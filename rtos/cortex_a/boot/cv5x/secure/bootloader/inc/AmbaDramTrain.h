
#ifndef _DDRC_TRAINING_C_H_
#define _DDRC_TRAINING_C_H_

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

//#define DDRCT_PRODUCTION_TRAIN_CODE
#define DDRCT_INCLUDE_DCM_TRAIN_CODE
//#define DDRCT_INCLUDE_RTT_CODE
//#define DDRCT_INCLUDE_READ_TRAIN_CODE
//#define DDRCT_INCLUDE_WRITE_TRAIN_CODE
//#define DDRCT_INCLUDE_WLVL_TRAIN_CODE
//#define DDRCT_INCLUDE_CMD_TRAIN_CODE
#define DDRCT_INCLUDE_RW_TRAIN_CODE
//#define DDRCT_INCLUDE_TRAIN_CODE	// Has same function prototype with DDRCT_PRODUCTION_TRAIN_CODE
#define DDRCT_SI_DATA_PATTERN
#define DISABLE_LP4_WRITE_TRAIN     0

#define DDRC_MIN(a, b) (((a) > (b)) ? (b) : (a))
#define DDRC_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define PRINT_STATUS(x) (x ? print (" : \033[1;32mPASSED\033[0m", 0, 0) : print (" : \033[1;31mFAILED\033[0m", 0, 0))
#define BYTE(x, idx) (((x) >> ((idx)*8)) & 0xff)
#define IDX2OH(x) ((x) == 3 ? 3 : 1 << (x))

// To control amount of code generated, please define flags as needed

/*  During simulation define DDRCT_SIM_ENV for reduced
    simulation time. Since code is running slow (because it is running
    from scratch pad), we don't need to add extra waits. ddrc_wait
    function has 6 load/store to scratch pad which is running @600Mh
    where each load takes about 5ns.
*/
//#define DDRCT_SIM_ENV

/* To control amount of code, define the following variables to
      include needed code */
      /* #define DDRCT_INCLUDE_READ_TRAIN_CODE */
      /* #define DDRCT_INCLUDE_WRITE_TRAIN_CODE */
      /* #define DDRCT_INCLUDE_DTTE_CODE */
      /* #define DDRCT_INCLUDE_PLOT_CODE */
      /* #define DDRCT_INCLUDE_CMDDCMWCK_TRAIN_CODE */

#include "hwio.h"

// 16KB scratchpad for code
// log dump at 0x20e000c000 - 8KB at flash controller FIFO (for dtte/train)
// log dump at 0x20ee000000 - FMEM (for plot data)
// First 4 words are for options. See ddrc_training.c
#include "AmbaCSL_DDRC.h"

#define DDRCT_EXEC_SPACE    0x20ee000000ULL

#if 0
#ifdef DDRCT_INCLUDE_PLOT_CODE
#define DDRCT_LOG_SPACE 0x20ee000000
#define DDRCT_LOG_SIZE 128*1024
#else
#define DDRCT_LOG_SPACE 0x20e000c000
#define DDRCT_LOG_SIZE 8*1024
#endif
#endif

#define DRAM_TRAINING_CODE_SIZE 0x8000
#define DRAM_TRAINING_STACK_SIZE 0x1000

#define DBSE_DEBUG_ADDRESS 0x20ED1F0000ULL

#define DDRC_CA_DELAY_MIN 0
#define DDRC_CA_LP4_COARSE_DELAY_MAX 10
#define DDRC_CA_DELAY_MAX 16
#define DDRC_CA_DELAY_STEP 1
#define DDRC_CA_DELAY_FUOT 4

#define DDRC_WCK_DELAY_MIN 0
#define DDRC_WCK_DELAY_MAX 31
#define DDRC_WCK_DELAY_STEP 1

#define DDRC_CA_VREF_MIN 0
#define DDRC_CA_LP4_VREF_MAX 80
#define DDRC_CA_LP5_VREF_MAX 127
#define DDRC_CA_VREF_STEP 1

#define DDRC_CA_VREF_PASS_RANGE 8
#define DDRC_CA_VREF_MARGIN 4

#define DDRC_RDT_DQDELAY_MIN 0
#define DDRC_RDT_DQDELAY_MAX 31
#define DDRC_RDT_DQDELAY_STEP 1
#define DDRC_RDT_DQDELAY_FUOT 4

#define DDRC_RDT_DLLSEL_MIN 0
#define DDRC_RDT_DLLSEL_MAX 31
#define DDRC_RDT_DLLSEL_STEP 1

#define DDRC_RDT_VREF_MIN 0
#define DDRC_RDT_VREF_MAX 63
#define DDRC_RDT_VREF_STEP 1

#define DDRC_RDT_VREF_PASS_RANGE 6
#define DDRC_RDT_VREF_MARGIN 4

#define DDRC_WRT_DELAY_MIN 0
#define DDRC_WRT_DELAY_MAX 31
#define DDRC_WRT_DELAY_STEP 1
#define DDRC_WRT_DELAY_FUT 8
#define DDRC_WRT_DELAY_FOT 8

#define DDRC_WRT_VREF_MIN 0
#define DDRC_LP5_WRT_VREF_MAX 64
#define DDRC_LP4_WRT_VREF_MAX 79
#define DDRC_WRT_VREF_STEP 2

#define DDRC_WRT_VREF_PASS_RANGE 16
#define DDRC_WRT_VREF_MARGIN_LOW 4
#define DDRC_WRT_VREF_MARGIN_HIGH 4

#define DDRCT_HOST0_PLL_ADDRESS 0x20ed180000ULL
#define DDRCT_HOST1_PLL_ADDRESS 0x20ed190000ULL
#define DDRCT_DRAM_CONFIG 0x1000000000ULL

#define DDRC_NUM_BYTES 4

#define DDRC_UINST_N_M1   0x0
#define DDRC_UINST_BO     0x0

#define DDRC_UINST_CS_ASRT  0x1
#define DDRC_UINST_CS_REL   0x2
#define DDRC_UINST_DQD      0x3
#define DDRC_UINST_DQR      0x4
#define DDRC_UINST_DQC      0x5
#define DDRC_UINST_CAD      0x6
#define DDRC_UINST_DQS0D    0x7
#define DDRC_UINST_DQS0U    0x8
#define DDRC_UINST_TOGGLE   0x9
#define DDRC_UINST_MPC_RD_DQ  0xa
#define DDRC_UINST_MPC_WRRD_FIFO  0xb
#define DDRC_UINST_DCMD     0xd
#define DDRC_UINST_WCK_CTL  0xe
#define DDRC_UINST_DQ7_CTL  0xf
#define DDRC_UINST_DM0_CTL  0x10
#define DDRC_UINST_GC       0x11
#define DDRC_UINST_RPTR     0x12

#define DDRC_CONTROL_REG    0x0
#define DDRC_CONFIG_REG     0x4
#define DDRC_GATE_REG       0x8
#define DDRC_INIT_CTL_REG   0x2C
#define DDRC_MODE_REG       0x30
#define DDRC_RSVD_SPACE     0x38
#define DDRC_MPC_WDATA_REG(n)  (0x40 + ((n)*4))
#define DDRC_MPC_RDATA_REG(n)  (0x64 + ((n)*4))
#define DDRC_MPC_RMASK_REG(n)  (0xA4 + ((n)*4))
#define DDRC_UINST_REG(n)   (0xAC + ((n)*4))
#define DDRC_UINST_WT_REG   0xB4
#define DDRC_UINST_EN_REG   0xBC
#define DDRC_UINST_RSLT_REG 0xC0
#define DDRC_SCRATCHPAD_REG 0xC4
#define DDRC_CLEAR_MPC_RDATA 0xCC
#define DDRC_CKE_REG        0xD0
#define DTTE_DELAY_REG(n)   (0x108 + ((n)*4))
#define DTTE_VREF_REG       0x12c
#define DTTE_CMD_REG        0x130
#define DDRC_DTTE_DELAY_MAP_REG 0x134
#define DDRC_DTTE_CP_INFO_REG(n) (0x138 + ((n)*4))
#define DDRC_WRITE_VREF_REG(n) (0x164 + ((n)*4))
#define DDRC_DLL0_REG(n)       (0x210 + ((n)*4))
#define DDRC_DLL1_REG(n)       (0x218 + ((n)*4))
#define DDRC_DLL2_REG(n)       (0x220 + ((n)*4))
#define DDRC_BYTE_DELAY_REG 0x228
#define DDRC_CA_DELAY_C_REG 0x2EC
#define DDRC_CA_DELAY_F_REG(n) (0x2F0 + ((n)*4))
#define DDRC_CS_DELAY_F_REG 0x310
#define DDRC_CKE_DELAY_C_REG 0x314
#define DDRC_CKE_DELAY_F_REG 0x318
#define DDRC_WCK_DUTY_CYCLE_REG(n) (0x33c + ((n)*4))
#define DDRC_READ_DQ_VREF_REG(n) (0x378 + ((n)*4))
#define DDRIO_DLL_STATUS_BASE 0x388
#define DDRC_STATUS_REG 0x3A4
#define DDRC_DRIVE_ECC_REG 0x3C0
#define DDRC_MR46_VALUES 0x3d4
#define DDRC_CK_DELAY_REG 0x2e8

void PreTraining(ULONG StackAddr);

// DTTE registers
#define DTTE_DRAM_CONFIG_REG 0x0
#define DTTE_TRAINING_CONFIG0_REG 0x8
#define DTTE_TRAINING_CONFIG1_REG 0xc
#define DTTE_OBSV_CONFIG_REG 0x14
#define DTTE_TRAIN_PERIOD_REG 0x18
#define DTTE_TRAINING_ENABLE_REG 0x1c
#define DTTE_GRID_SIZE_DIV2_REG(n) (0x24 + ((n)*4))
#define DTTE_CLIP_VALUE_REG(n) (0x44 + ((n)*4))
#define DTTE_GRID_STEP_SIZE_LOG2_REG(n) (0x84 + ((n)*4))
#define DTTE_VREF_MGN_REG(n) (0x100 + ((n)*4))
#define DTTE_VAR_REG 0x140
#define DTTE_ALGO_ERROR_REG 0x148
#define DTTE_EXPECTED_PATTERN_REG(n) (0x1a0 + ((n)*4))
#define DTTE_GRID_STATUS_REG(n) (0x1e0 + ((n)*4))
#define DTTE_GRID_STAT_READY_REG 0x2e0

// DTTE params
// Controls time gap between byte/channel training events.
#define DTTE_TRAIN_PERIOD 0x0

typedef unsigned int   u32_t;
typedef unsigned long long u64_t;
typedef unsigned short u16_t;
typedef unsigned char  u8_t ;
typedef unsigned char  uchar_t ;
typedef char           byte_t;
typedef unsigned char  bool_t ;

typedef struct Train_info {
  u32_t found_flag;
  u32_t thw[2];
  u32_t tlw[2];
  u32_t vl;
  u32_t vh;
} train_info_t;

typedef struct Bit_info {
  u32_t cvl; /* Current Vl */
  u32_t cvh; /* Current Vh */
  union {
    u32_t ovl; /* Optimal Vl, corresponds to oht */
    u32_t tlw_min; // TLW[0]
  };
  union {
    u32_t ovh; /* Optimal Vh, corresponds to oht */
    u32_t tlw_max; // TLW[1]
  };
  union {
    u32_t odly; /* Optimal delay, corresponds to oht */
    u32_t thw_min; // THW[0]
  };
  union {
    u32_t oht; /* Optimal height */
    u32_t thw_max; // THW[1]
  };
} bit_train_info_t;

// information about delay that needs to be updated.
enum delay_id_t {
  WCK_WRDLY_FINE=0,
  WCK_WRDLY_COARSE=1,
  DQS_WRDLY_FINE=2,
  DQS_WRDLY_COARSE=3,
  DQS_RDDLY_FINE=4,
  DQS_RDDLY_COARSE=5,
  DQS_GATE_DLY_FINE=6,
  DQS_GATE_DLY_COARSE=7,
  DQ_WRDLY_COARSE=8,
  DQ_RDDLY_COARSE=9,
  DM_WRDLY_FINE=10,
  DM_RDDLY_FINE=11,
  DQ_WRDLY_FINE=12,
  DQ_RDDLY_FINE=13,
  DLL_CTRL_SEL0=14,
  DLL_CTRL_SEL1=15,
  DLL_CTRL_SEL2=16,
  CA_DELAY_FINE=17
};

typedef struct Delay_info {
  u16_t boffset; // bit offset from base address for byte0/die0
  u8_t  width;   // delay width
  u8_t  pitch;   // pitch in unit of bits
  u8_t  count;   // How many delay elements to update
  u8_t  byte_offset; // In bytes
  u8_t  die_offset; // In words
  bool_t is_dll_sel; // set for DLL select
} delay_info_t;

void ddrc_uinst(u32_t opcode, u32_t iop0, u32_t iop1);
  #ifdef DDRCT_INCLUDE_DTTE_CODE
void dtte_set_vref_margin(bool_t is_coarse);
void dtte_stop_tracking();
  #endif
void ddrc_train_preamble(u32_t sref_en);
void ddrc_train_postamble(u32_t sref_en);
void ddrc_zq_calibration(void);
void ddrc_train_all(void);


#endif
