
#include "config.h"

#if defined(CONFIG_BOARD_VERSION_CV22_WALNUT_66_LPDDR4) || defined(CONFIG_BOARD_VERSION_CV22_WALNUT_55_LPDDR4) || defined(CONFIG_BOARD_VERSION_CV22_WALNUT_66_LPDDR4_1200_CORTEX)
#define FREQUENCY	1800000
#endif
#if defined(CONFIG_BOARD_VERSION_CV22_WALNUT_1600_LPDDR4)
#define FREQUENCY	1600000
#endif
#if defined(CONFIG_BOARD_VERSION_CV22_WALNUT_25_LPDDR4) || defined(CONFIG_BOARD_VERSION_CV22_WALNUT_22_LPDDR4)
#define FREQUENCY	1200000
#endif


//DRAM_CONFIG
#define PAGE_SIZE PAGE_SIZE_4K
#define BANK_SIZE BANK_SIZE_8
#define DRAM_SIZE DRAM_SIZE_16G
#define BUS_WIDTH BUS_WIDTH_32
#define DRAM_TYPE DRAM_CHIP_TYPE_LPDDR4

//pll setting
#define PLL_CORE_CTRL_VALUE 0x20100000
#define PLL_IDSP_CTRL_VALUE 0x10000000
#define PLL_CORTEX_CTRL_VALUE 0x29100000
#define MISC_PARAM	0x0000000f

//DQ & CA Termination and DDS
//NOTE: default lower 16bit and high 16bit DDS, PDDS and termination is same
//NOTE: default CA and ADDR DDS, PDDS and termination is same
#define DQ_DDS 0x7 //range: 0x0 -- 0x7
#define DQ_PDDS 0x1 //range: 0x0 -- 0x2
#define DQ_TERM 0x4 //range: 0x0 -- 0x7
#define DQ_TERM_SEL PADS_TERM_ENABLE

#define CA_ADDR_DDS 0x7 //range: 0x0 -- 0x7
#define CA_ADDR_PDDS 0x1 //range: 0x0 -- 0x2
#define CA_ADDR_TERM 0x4 //range: 0x0 -- 0x7
#define CA_ADDR_TERM_SEL PADS_TERM_ENABLE

//mode register 11
#define MR11_DQ_ODT DQ_ODT_RZQ_2
#define MR11_CA_ODT CA_ODT_RZQ_2

//Training parameters: will be changed after training process
#define DRAM_DLL_VALUE 0x0c080c
#define DRAM_LPDDR4_DQCA_VREF_VALUE 0x042850a1
#define DRAM_DQ_WRITE_DELY_VALUE 0xc56ad5ab
#define DRAM_DQ_READ_DLY_VALUE 0x0005ad6b
#define DRAM_MODE_REG14_VALUE 0x010e0012
//Training parameters END

#define DRAM_SELF_REF_VALUE 0x00000000 //gpio address
