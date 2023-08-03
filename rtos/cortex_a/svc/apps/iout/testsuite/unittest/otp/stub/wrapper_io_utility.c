#include <test_group.h>
#include <AmbaTypes.h>
#include <AmbaIOUtility.h>

static UINT8 scratchpad_memory[200];
static UINT8 otp_memory[1024*8];

UINT8 flag_otp_engine_running   = 1;
UINT8 flag_otp_write_done_fail  = 0;
UINT8 flag_otp_write_fail       = 0;
UINT8 flag_otp_read_fail        = 0;
UINT8 flag_otp_rrdy_fail        = 0;
UINT32 otp_rrdy_fail_cnt_down   = 0xFFFFFFFF;
UINT32 otp_write_fail_cnt_down  = 0xFFFFFFFF;

#define OTP_READ_FSM_EN_POS      22U
#define OTP_READ_EN_POS          21U
#define OTP_DBG_READ_MODE_POS    20U
#define OTP_FSM_WRITE_MODE_POS   18U
#define OTP_PROG_EN_POS          17U
#define OTP_FSM_PROG_EN_POS      16U

#define OTP_READ_FSM_EN_MASK     (1U << OTP_READ_FSM_EN_POS)
#define OTP_READ_EN_MASK         (1U << OTP_READ_EN_POS)
#define OTP_DBG_READ_MODE_MASK   (1U << OTP_DBG_READ_MODE_POS)
#define OTP_FSM_WRITE_MODE_MASK  (1U << OTP_FSM_WRITE_MODE_POS)
#define OTP_PROG_EN_MASK         (1U << OTP_PROG_EN_POS)
#define OTP_FSM_PROG_EN_MASK     (1U << OTP_FSM_PROG_EN_POS)

#define OTP_WRITE_CTRL_PROG_DONE_POS   4U
#define OTP_WRITE_CTRL_PROG_FAIL_POS   3U
#define OTP_WRITE_CTRL_PROG_READY_POS  2U
#define OTP_READ_CTRL_READ_READY_POS   1U
#define OTP_READ_CTRL_READ_DONE_POS    0U

#define OTP_WRITE_CTRL_PROG_DONE_MASK  (1U << OTP_WRITE_CTRL_PROG_DONE_POS)
#define OTP_WRITE_CTRL_PROG_FAIL_MASK  (1U << OTP_WRITE_CTRL_PROG_FAIL_POS)
#define OTP_WRITE_CTRL_PROG_READY_MASK (1U << OTP_WRITE_CTRL_PROG_READY_POS)
#define OTP_READ_CTRL_READ_READY_MASK  (1U << OTP_READ_CTRL_READ_READY_POS)
#define OTP_READ_CTRL_READ_DONE_MASK   (1U << OTP_READ_CTRL_READ_DONE_POS)

static UINT8 flag_debug = 0;

static void handle_otp_register_write(UINT64 Address, UINT32 Value)
{
#if defined(CONFIG_SOC_CV22)

#define OTP_CTRL1_REG_OFFSET  ((UINT32)0xA0U)
#define OTP_OBSV_REG_OFFSET   ((UINT32)0xA4U)
#define OTP_DATA0_REG_OFFSET  ((UINT32)0xA8U)
#define OTP_DATA1_REG_OFFSET  ((UINT32)0xB8U)

#else
#error "OTP engine not implemented!"
#endif
    if (flag_debug != 0U) {
        printf("handle_otp_register_write(): 0x%llX, 0x%X\n", Address, Value);
    }
    if (Address == OTP_CTRL1_REG_OFFSET) {
        UINT32 *u32_ptr_obsv  = (UINT32 *)&scratchpad_memory[OTP_OBSV_REG_OFFSET];
        UINT32 *u32_ptr_data0 = (UINT32 *)&scratchpad_memory[OTP_DATA0_REG_OFFSET];
        if (Value == 0U) {
            // reset, clear observe and data0 register
            *u32_ptr_obsv = 0;
            *u32_ptr_data0 = 0;
            return;
        }

        if (((Value & OTP_FSM_WRITE_MODE_MASK) == 0U) &&
            ((Value & OTP_DBG_READ_MODE_MASK) != 0U) &&
            ((Value & OTP_READ_FSM_EN_MASK) != 0U) &&
            ((Value & OTP_READ_EN_MASK) == 0U)
           ) {
            if (flag_debug != 0U) {
                printf("handle_otp_register_write(): read fsm en\n");
            }
            // enable read FSM, must set read ready in observe register
            UINT32 *u32_ptr_obsv = (UINT32 *)&scratchpad_memory[OTP_OBSV_REG_OFFSET];
            if (flag_otp_rrdy_fail == 0U) {
                *u32_ptr_obsv = *u32_ptr_obsv | OTP_READ_CTRL_READ_READY_MASK;
            } else {
                if (otp_rrdy_fail_cnt_down > 0U) {
                    // set read ready
                    *u32_ptr_obsv = *u32_ptr_obsv | OTP_READ_CTRL_READ_READY_MASK;
                    otp_rrdy_fail_cnt_down--;
                } else {
                    // clear read ready
                    *u32_ptr_obsv = *u32_ptr_obsv & (~OTP_READ_CTRL_READ_READY_MASK);
                }
            }
        }

        if (((Value & OTP_FSM_WRITE_MODE_MASK) == 0U) &&
            ((Value & OTP_DBG_READ_MODE_MASK) != 0U) &&
            ((Value & OTP_READ_FSM_EN_MASK) != 0U) &&
            ((Value & OTP_READ_EN_MASK) != 0U)
           ) {
            if (flag_debug != 0U) {
                printf("handle_otp_register_write(): read en\n");
            }
            // read with address set

            // 1. extract bit address
            UINT32 bit_address = Value & 0x0FFFFU;

            // 2. read OTP memory
            UINT32 byte_address = bit_address / 8U;
            UINT32 *u32_ptr = (UINT32 *)&otp_memory[byte_address];

            // 3. put data into data0 register
            *u32_ptr_data0 = *u32_ptr;

            if (flag_otp_read_fail == 0U) {
                // 4. set read done in observe register
                *u32_ptr_obsv = *u32_ptr_obsv | OTP_READ_CTRL_READ_DONE_MASK;
            } else {
                *u32_ptr_obsv = *u32_ptr_obsv & (~OTP_READ_CTRL_READ_DONE_MASK);
            }
        }

        if (((Value & OTP_FSM_WRITE_MODE_MASK) != 0U) &&
            ((Value & OTP_FSM_PROG_EN_MASK) != 0U) &&
            ((Value & OTP_PROG_EN_MASK) == 0U)
           ) {
            if (flag_debug != 0U) {
                printf("handle_otp_register_write(): write fsm en\n");
            }
            // enable write FSM, must set write ready in observe register
            UINT32 *u32_ptr_obsv = (UINT32 *)&scratchpad_memory[OTP_OBSV_REG_OFFSET];
            *u32_ptr_obsv = *u32_ptr_obsv | OTP_WRITE_CTRL_PROG_READY_MASK;
        }

        if (((Value & OTP_FSM_WRITE_MODE_MASK) != 0U) &&
            ((Value & OTP_FSM_PROG_EN_MASK) != 0U) &&
            ((Value & OTP_PROG_EN_MASK) != 0U)
           ) {
            if (flag_debug != 0U) {
                printf("handle_otp_register_write(): write en\n");
            }
            // write with address set

            // 1. extract bit address
            UINT32 bit_address = Value & 0x0FFFFU;
            UINT32 byte_address = bit_address / 8U;
            UINT32 bit_offset = bit_address % 8U;
            UINT8  u8_value = otp_memory[byte_address];

            // 2. set the bit
            u8_value |= (1U << bit_offset);

            // 3. write the otp memory
            otp_memory[byte_address] = u8_value;

            // 4. set write done in observe register
            if (flag_otp_write_done_fail == 0U) {
                *u32_ptr_obsv = *u32_ptr_obsv | OTP_WRITE_CTRL_PROG_DONE_MASK;
            } else {
                *u32_ptr_obsv = *u32_ptr_obsv & (~OTP_WRITE_CTRL_PROG_DONE_MASK);
            }

            // 5. set write fail if flag set
            if (flag_otp_write_fail == 0U) {
                *u32_ptr_obsv = *u32_ptr_obsv & (~OTP_WRITE_CTRL_PROG_FAIL_MASK);
            } else {
                if (otp_write_fail_cnt_down > 0U) {
                    // don't set fail if count > 0
                    *u32_ptr_obsv = *u32_ptr_obsv & (~OTP_WRITE_CTRL_PROG_FAIL_MASK);
                    otp_write_fail_cnt_down--;
                } else {
                    // set fail if count = 0
                    *u32_ptr_obsv = *u32_ptr_obsv | OTP_WRITE_CTRL_PROG_FAIL_MASK;
                }
            }
        }
    }
}

// OTP CSL only access 3 registers

void IO_UtilityRegWrite32(UINT64 Address, UINT32 Value)
{
    UINT32 *ptr_u32;

    Address &= 0x000000FF;
    ptr_u32 = (UINT32 *)&scratchpad_memory[Address];
    *ptr_u32 = Value;
    if (flag_otp_engine_running != 0U) {
        handle_otp_register_write(Address, Value);
    }
}

UINT32 IO_UtilityRegRead32(UINT64 Address)
{
    UINT32 *ptr_u32;
    Address &= 0x000000FF;
    ptr_u32 = (UINT32 *)&scratchpad_memory[Address];
    return *ptr_u32;
}

