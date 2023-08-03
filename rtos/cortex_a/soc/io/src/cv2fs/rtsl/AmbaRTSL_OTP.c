/**
 *  @file AmbaRTSL_OTP.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details OTP (One-Time Programming) Control APIs
 *
 */

#include "AmbaTypes.h"

#include "AmbaRTSL_OTP.h"

// In CV2FS, every 32-bit has 8-bit CRC (CRC8) and they occupies 64-bit memory space.
// In the 64-bit memory space, most significant 24-bit is not used.
//
// For some zones, they have CRC8 protected. That is, when you read data from these zones,
// hardware (OTP engine) will calculate CRC8 value of data (32-bit) and compare it with following 8-bit CRC8 value.
// It the comparsion fails, hardware generates an OTP error (ID 28) to CEHU.
//
// Not that even the CRC8 comparsion fails, the data still can read correctly.
// Software can:
//    1. calculate CRC8 value of data (32-bit) and compare it with following 8-bit CRC8 value manually
//    2. or check CEHU error
// each time when it tries to read data from these zones.

#if defined(CONFIG_BUILD_SSP_OTP) || defined(CONFIG_QNX)
#define BIT_ADDR_SECURE_BOOT          (0x06U)   // 1 bit
#define BIT_ADDR_JTAG_EFUSE           (0x5FU)   // 1 bit
#define BIT_ADDR_WRITE_LOCK           (0xC0U * 2U)   // bit 5: C1 lock, bit 6: C2 lock, bit 7: C3 lock
#endif

#define BIT_ADDR_READ_TEMP_SENSOR0    (0x200U)       // 32-bits, temperature sensor output data of write lock bits
#define BIT_ADDR_READ_TEMP_SENSOR1    (0x240U)       // 32-bits, temperature sensor output data of write lock bits
#define BIT_ADDR_READ_TEMP_SENSOR2    (0x280U)       // 32-bits, temperature sensor output data of write lock bits
#define BIT_ADDR_READ_TEMP_SENSOR3    (0x2C0U)       // 32-bits, temperature sensor output data of write lock bits
#define BIT_ADDR_READ_TEMP_SENSOR4    (0x300U)       // 32-bits, temperature sensor output data of write lock bits
#define BIT_ADDR_READ_TEMP_SENSOR5    (0x340U)       // 32-bits, temperature sensor output data of write lock bits
#define BIT_ADDR_READ_TEMP_SENSOR6    (0x280U)       // 32-bits, temperature sensor output data of write lock bits
#define BIT_ADDR_READ_TEMP_SENSOR7    (0x2C0U)       // 32-bits, temperature sensor output data of write lock bits
#define BIT_ADDR_READ_TEMP_SENSOR8    (0x300U)       // 32-bits, temperature sensor output data of write lock bits
#define BIT_ADDR_READ_TEMP_SENSOR9    (0x3C0U)       // 32-bits, temperature sensor output data of write lock bits

#if defined(CONFIG_BUILD_SSP_OTP) || defined(CONFIG_QNX)
#define BIT_ADDR_WRITE_LOCK_CONFIRM   (0x1C0U)       // 32-bits, reverse order of write lock bits
#define BIT_ADDR_AMBA_UNIQUE_ID       (0x3E00U * 2U) // 128 bits, CRC8 protected.
#define BIT_ADDR_DATA_INVALID         (0x3E80U * 2U) // 32-bits, bit 0: C1 invalid, bit1: C2 invalid
#define BIT_ADDR_DATA_INVALID_CONFIRM (0x7D40U)      // 32-bits, reverse order of data invalid bits
#define BIT_ADDR_CUSTOMER_UNIQUE_ID   (0x3F00U * 2U) // 128 bits, CRC8 protected.
#define BIT_ADDR_RESERVED_B2          (0x3F80U * 2U) // 128 bits, CRC8 protected.
#define BIT_ADDR_MONO_COUNTER         (0x4100U * 2U) // 256 bits
#define BIT_ADDR_AES_KEY_1            (0x4200U * 2U) // 256 bits x 4, CRC8 protected.
#define BIT_ADDR_RESERVED_DX_1        (0x4A00U * 2U) // 256 bits x 6, CRC8 protected.
#define BIT_ADDR_RSA_KEY_1            (0x5000U * 2U) // 4096 bits, Zone C1, CRC8 protected.
#define BIT_ADDR_RSA_KEY_2            (0x6000U * 2U) // 4096 bits, Zone C2, CRC8 protected.
#define BIT_ADDR_RSA_KEY_3            (0x7000U * 2U) // 4096 bits, Zone C3, CRC8 protected.
#define BIT_DATA_PITCH                (64U)
#endif
#define BIT_ADDR_MASK                 (0x0FFFFU)

static void ctrl1_reg_bit_clear(UINT32 Bit)
{
    UINT32 value = AmbaCSL_OtpCtrl1Read();
    value = value & (~Bit);
    AmbaCSL_OtpCtrl1Write(value);
}

static void ctrl1_reg_bit_set(UINT32 Bit)
{
    UINT32 value = AmbaCSL_OtpCtrl1Read();
    value = value | Bit;
    AmbaCSL_OtpCtrl1Write(value);
}

static UINT32 otp_controller_wait(UINT32 Mask)
{
    UINT32 retry_count = 0;
    UINT32 value;
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;

    // Wait until Mask is 1
    // Use busy polling to improve performance.
    do {
        value = AmbaCSL_OtpObsvRead();
        if ((value & Mask) == 0U) {
            //AmbaKAL_TaskSleep(1);
            retry_count++;
            if (retry_count > 100000U) {
                uret = RTSL_ERR_OTP_WAIT_TIMEOUT;
                break;
            }
        }
    } while ((value & Mask) == 0U);
    return uret;
}


/**
 * @brief read 32-bit data from OTP
 * @param Address [IN] OTP bit address, it must be 64-bit aligned.
 * @param Data [OUT] 32-bit data
 * @param Crc8 [OUT] CRC8 value inside OTP for Data
*/
static UINT32 otp_uint32_read(UINT32 Address, UINT32 *Data, UINT32 *Crc8)
{
    UINT32 value;
    UINT32 uret;

    AmbaCSL_OtpCtrl1Write(0);
    ctrl1_reg_bit_clear(FSM_WRITE_MODE);       /* fsm_write_mode = 0 */
    ctrl1_reg_bit_set(DBG_READ_MODE);
    ctrl1_reg_bit_set(READ_FSM_ENABLE);

    // Wait until Read Ready
    uret = otp_controller_wait(READ_OBSV_RDY);

    if (uret == RTSL_ERR_OTP_SUCCESS) {
        // Set OTP Read Address
        value = AmbaCSL_OtpCtrl1Read();
        value &= ~(BIT_ADDR_MASK);
        value |= (Address & BIT_ADDR_MASK);
        AmbaCSL_OtpCtrl1Write(value);

        // Enable Read Operation
        ctrl1_reg_bit_set(READ_ENABLE);

        // Wait until Read Done
        uret = otp_controller_wait(READ_OBSV_DONE);

        if (uret == 0U) {
            *Data = AmbaCSL_OtpDataRead();
            *Crc8 = AmbaCSL_OtpCrc8Read();
        }

    }

    // Disable Read Operation
    ctrl1_reg_bit_clear(READ_ENABLE);

    // Wait until Read Ready
    if (otp_controller_wait(READ_OBSV_RDY) != 0U) {
        // ignore this error
    }

    return uret;
}

#if defined(CONFIG_BUILD_SSP_OTP) || defined(CONFIG_QNX)

static UINT8 otp_crc8(const UINT8 *DataArray, UINT32 ArraySize)
{
    const UINT8 generator = 0xD5;
    UINT8 crc = 0xFF; /* start with 0xFF */
    UINT8 i, j;

    for (i = 0; i < ArraySize; i++) {
        crc ^= DataArray[i]; /* XOR-in the next input byte */

        for (j = 0; j < 8U; j++) {
            if ((crc & 0x80U) != 0U) {
                crc = ((crc << 1U) ^ generator);
            } else {
                crc <<= 1U;
            }
        }
    }

    return crc;
}

static UINT32 otp_bit_read(UINT32 Address, UINT32 *BitValue)
{
    UINT32 aaddr = Address & (~0x3FUL);
    UINT32 bfs = (Address & 0x3FUL);
    UINT32 data = 0;
    UINT32 crc8_value;
    UINT32 bit = 0;
    UINT32 uret;

    uret = otp_uint32_read(aaddr, &data, &crc8_value);

    if (uret == 0U) {
        if (bfs > 0x1FUL) {
            // high 32-bit data
            bfs = 1UL << (bfs - 0x20UL);
            if ((crc8_value & bfs) != 0U) {
                bit = 1;
            }
        } else {
            // low 32-bit data
            bfs = 1UL << bfs;
            if ((data & bfs) != 0U) {
                bit = 1;
            }
        }
        *BitValue = bit;
    }

    return uret;
}

static UINT8 otp_data_crc8_be(UINT32 Data, UINT32 BitAddress)
{

    UINT8 byte_array[6];

    byte_array[0] = (UINT8)(Data >> 24U);
    byte_array[1] = (UINT8)(Data >> 16U);
    byte_array[2] = (UINT8)(Data >> 8U);
    byte_array[3] = (UINT8)Data;
    byte_array[4] = (UINT8)(BitAddress >> 8U);
    byte_array[5] = (UINT8)BitAddress;

    return otp_crc8(byte_array, 6);
}

static UINT32 otp_bit_write(UINT32 Address)
{
    UINT32 value;
    UINT32 uret;

    uret = otp_bit_read(Address, &value);

    if (uret == RTSL_ERR_OTP_SUCCESS) {
        /* do nothing if the bit has already been set. */
        if (value != 0U) {
            // the bit is always 1, can't set to 1 again
            uret = RTSL_ERR_OTP_PROGRAMMED;
        } else {
            AmbaCSL_OtpCtrl1Write(0);
            ctrl1_reg_bit_set(FSM_WRITE_MODE);  /* fsm_write_mode = 1 */
            ctrl1_reg_bit_set(PROG_FSM_ENABLE); /* prog_fsm_en = 1 */

            // Wait until Prog Ready
            uret = otp_controller_wait(WRITE_PROG_RDY);
            if (uret == RTSL_ERR_OTP_SUCCESS) {
                // Set OTP Wead Address
                value = AmbaCSL_OtpCtrl1Read();
                value &= ~(BIT_ADDR_MASK);
                value |= (Address & BIT_ADDR_MASK);
                AmbaCSL_OtpCtrl1Write(value);

                // Enable Write Operation
                ctrl1_reg_bit_set(PROG_ENABLE);

                // Wait until Prog Done
                uret = otp_controller_wait(WRITE_PROG_DONE);

                if (uret == RTSL_ERR_OTP_SUCCESS) {
                    value = AmbaCSL_OtpObsvRead();
                    if ((value & WRITE_PROG_FAIL) != 0U) {
                        uret = RTSL_ERR_OTP_PROGRAM_FAIL;
                    }
                }
            }

            // Disable Write Operation
            ctrl1_reg_bit_clear(PROG_ENABLE);

            // Wait until Prog Ready
            if (otp_controller_wait(WRITE_PROG_RDY) != 0U) {
                // ignore this error
            }
        }
    }
    return uret;
}

static UINT32 otp_program(UINT32 OtpBitAddress, const UINT8 *Buffer, UINT32 ByteSize)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    UINT32 address = OtpBitAddress;
    UINT32 i, shift;
    UINT32 bit_offset = 0;
    UINT32 u32_value = 0;

    if (((OtpBitAddress % BIT_DATA_PITCH) != 0U) || ((ByteSize % 4U) != 0U) ) {
        // write address must be pitch-aligned
        // size must be 4-byte aligned.
        uret = RTSL_ERR_OTP_NO_ALIGN;
    } else {
        // program data one by one bit
        for (i = 0; i < ByteSize; i++) {
            UINT8 byte_data = Buffer[i];

            if ((i % 4U) == 0U) {
                u32_value = Buffer[i];
                u32_value += (((UINT32)Buffer[i+1U]) << 8U);
                u32_value += (((UINT32)Buffer[i+2U]) << 16U);
                u32_value += (((UINT32)Buffer[i+3U]) << 24U);
            }

            for (shift = 0; shift < 8U; shift++) {
                UINT32 bit_data = ((UINT32)byte_data >> shift) & 0x01UL;
                if (bit_data != 0U) {
                    uret = otp_bit_write(address + bit_offset);
                    if (uret != RTSL_ERR_OTP_SUCCESS) {
                        break;
                    }
                }
                bit_offset++;
            }

            if ((bit_offset % 32U) == 0U) {
                UINT32 bit_steps = BIT_DATA_PITCH - 32U;
                UINT32 crc_source_addr;
                UINT32 crc_prog_addr;
                UINT8  crc8_value;

                // OTP address might have 64-bit aligned.
                // After writing 4 bytes, need to align for next write.
                bit_offset = bit_offset + bit_steps;

                crc_source_addr = ((address + bit_offset) - bit_steps) -32U;
                crc_prog_addr = crc_source_addr + 32U;
                // calculate and program CRC8 for each 32-bit data
                crc8_value = otp_data_crc8_be(u32_value, crc_source_addr);
                // write crc8
                for (shift = 0; shift < 8U; shift++) {
                    UINT32 bit_data = ((UINT32)crc8_value >> shift) & 0x01UL;
                    if (bit_data != 0U) {
                        uret = otp_bit_write(crc_prog_addr + shift);
                        if (uret != RTSL_ERR_OTP_SUCCESS) {
                            break;
                        }
                    }
                }
            }

            if (uret != RTSL_ERR_OTP_SUCCESS) {
                break;
            }
        }
    }

    return uret;
}

/**
 * @brief read OTP data
 * @param FlagCheckCrc8 0: Don't check CRC8 1: Check CRC8
 * @retval RTSL_ERR_OTP_CRC crc8 check error. Note data is still filled into buffer.
*/
static UINT32 otp_read(UINT32 OtpBitAddress, UINT8 *Buffer, UINT32 ByteSize, UINT32 FlagCheckCrc8)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    UINT32 i;
    UINT32 data;
    UINT32 crc8_value;
    UINT32 crc8_calculated;
    UINT32 flag_crc8_error = 0;

    if (((OtpBitAddress % BIT_DATA_PITCH) != 0U) || ((ByteSize % 4U) != 0U) ) {
        // write address must be pitch-aligned
        // size must be 4-byte aligned.
        uret = RTSL_ERR_OTP_NO_ALIGN;
    } else {
        for (i = 0; i < (ByteSize/4U); i++) {
            uret = otp_uint32_read(OtpBitAddress + (i * BIT_DATA_PITCH), &data, &crc8_value);
            if (uret != RTSL_ERR_OTP_SUCCESS) {
                break;
            } else {
                Buffer[i*4U] = (UINT8)(data & 0x0FFU);
                Buffer[(i*4U)+1U] = (UINT8)((data >> 8U) & 0x0FFU);
                Buffer[(i*4U)+2U] = (UINT8)((data >> 16U) & 0x0FFU);
                Buffer[(i*4U)+3U] = (UINT8)((data >> 24U) & 0x0FFU);

                // check crc8 if needed
                if (FlagCheckCrc8 != 0U) {
                    crc8_calculated = otp_data_crc8_be(data, OtpBitAddress + (i * BIT_DATA_PITCH));
                    if (crc8_calculated != crc8_value) {
                        flag_crc8_error = 1;
                    }
                }
            }
        }
    }

    if (flag_crc8_error != 0U) {
        if (uret == RTSL_ERR_OTP_SUCCESS) {
            uret = RTSL_ERR_OTP_CRC;
        }
    }

    return uret;
}

/**
 * Write RSA public key into OTP
 * @param KeyBuffer  [IN] Buffer contains RSA key with Ambarella specific key format.
 * @param BufferSize [IN] The size must be larger than 512 bytes.
 * @param KeyIndex   [IN] Valid value: 0, 1, 2.
 */
UINT32 AmbaRTSL_OtpPublicKeyWrite(const UINT8 *KeyBuffer, UINT32 BufferSize, UINT32 KeyIndex)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    if ((BufferSize < 512U) || (KeyBuffer == NULL)) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        UINT32 address = 0;
        UINT32 lock_addr = 0;
        UINT32 lock_confirm_addr = 0;
        switch (KeyIndex) {
        case 0:
            address = BIT_ADDR_RSA_KEY_1;
            lock_addr = BIT_ADDR_WRITE_LOCK + 5U;
            lock_confirm_addr = BIT_ADDR_WRITE_LOCK_CONFIRM + (31U - 5U);
            break;
        case 1:
            address = BIT_ADDR_RSA_KEY_2;
            lock_addr = BIT_ADDR_WRITE_LOCK + 6U;
            lock_confirm_addr = BIT_ADDR_WRITE_LOCK_CONFIRM + (31U - 6U);
            break;
        case 2:
            address = BIT_ADDR_RSA_KEY_3;
            lock_addr = BIT_ADDR_WRITE_LOCK + 7U;
            lock_confirm_addr = BIT_ADDR_WRITE_LOCK_CONFIRM + (31U - 7U);
            break;
        default:
            uret = RTSL_ERR_OTP_PARAM;
            break;
        }
        if (uret == RTSL_ERR_OTP_SUCCESS) {
            uret = otp_program(address, KeyBuffer, 512);

            if (uret == RTSL_ERR_OTP_SUCCESS) {
                // set 'write lock bit'
                uret = otp_bit_write(lock_addr);
                if (uret == RTSL_ERR_OTP_SUCCESS) {
                    // set 'write lock confirm bit'
                    uret = otp_bit_write(lock_confirm_addr);
                }
            }
        }
    }
    return uret;
}
static UINT32 u32_value_reverse(UINT32 Input)
{
    // source: stackoverflow
    // Here's explanation: Let us divide all bits in block size b,
    // starting with b=1. Now we swap each adjacent block.
    // Double block size and repeat.
    // Continue until block size is half of word size.
    // For 32 bits, this will be 5 steps.
    // Each step can be written as ((x & mask) << b) | ((x & mask') << b).
    // Thus in 5 statements, we can reverse 32 bit int.
    UINT32 x = Input;
    x = ((x >> 1U) & 0x55555555u) | ((x & 0x55555555u) << 1U);
    x = ((x >> 2U) & 0x33333333u) | ((x & 0x33333333u) << 2U);
    x = ((x >> 4U) & 0x0f0f0f0fu) | ((x & 0x0f0f0f0fu) << 4U);
    x = ((x >> 8U) & 0x00ff00ffu) | ((x & 0x00ff00ffu) << 8U);
    x = ((x >> 16U) & 0xffffu) | ((x & 0xffffu) << 16U);
    return x;
}

static UINT32 rsa_pkey_info_read(UINT32 *WriteLock, UINT32 *WriteLockConfirm, UINT32 *DataInvalid, UINT32 *DataInvalidConfirm)
{
    UINT32 uret;
    UINT32 crc8_value;

    // write lock and data invalid bits don't have CRC8 protection.
    // read 'write lock' bits
    uret = otp_uint32_read(BIT_ADDR_WRITE_LOCK, WriteLock, &crc8_value);
    if (uret == RTSL_ERR_OTP_SUCCESS) {
        // read 'write lock confirm' bits
        uret = otp_uint32_read(BIT_ADDR_WRITE_LOCK_CONFIRM, WriteLockConfirm, &crc8_value);
        if (uret == RTSL_ERR_OTP_SUCCESS) {
            // read 'data invalid' bits
            uret = otp_uint32_read(BIT_ADDR_DATA_INVALID, DataInvalid, &crc8_value);
            if (uret == RTSL_ERR_OTP_SUCCESS) {
                // read 'data invalid confirm' bits
                uret = otp_uint32_read(BIT_ADDR_DATA_INVALID_CONFIRM, DataInvalidConfirm, &crc8_value);
            }
        }
    }
    return uret;
}

static UINT32 rsa_pkey_info_check(UINT32 WriteLock, UINT32 WriteLockConfirm, UINT32 DataInvalid, UINT32 DataInvalidConfirm)
{
    UINT32 uret;
    // acording to spec, x and x_confirm should have reverse bit order.
    if (WriteLock != u32_value_reverse(WriteLockConfirm)) {
        uret = RTSL_ERR_OTP_WL_CONFIRM;
    } else if (DataInvalid != u32_value_reverse(DataInvalidConfirm)) {
        uret = RTSL_ERR_OTP_DI_CONFIRM;
    } else {
        uret = RTSL_ERR_OTP_SUCCESS;
    }
    return uret;
}

/**
 * Read RSA public key from OTP
 * @param KeyBuffer  [OUT] Buffer contains RSA key with Ambarella specific key format.
 * @param BufferSize [IN] The size must be larger than 512 bytes.
 * @param KeyIndex   [IN] Valid value: 0, 1, 2.
 * @param KeyStatus  [OUT] bit 0: write lock bit (1 for lock). bit 1: data invalid bit (1 for invalid)
 */
UINT32 AmbaRTSL_OtpPublicKeyRead(UINT8 *KeyBuffer, UINT32 BufferSize, UINT32 KeyIndex, UINT32 *KeyStatus)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    if ((BufferSize < 512U) || (KeyBuffer == NULL) || (KeyStatus == NULL)) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        UINT32 address = 0;
        switch (KeyIndex) {
        case 0:
            address = BIT_ADDR_RSA_KEY_1;
            break;
        case 1:
            address = BIT_ADDR_RSA_KEY_2;
            break;
        case 2:
            address = BIT_ADDR_RSA_KEY_3;
            break;
        default:
            uret = RTSL_ERR_OTP_PARAM;
            break;
        }
        if (uret == RTSL_ERR_OTP_SUCCESS) {
            UINT32 write_lock = 0;
            UINT32 write_lock_confirm = 0;
            UINT32 data_invalid = 0;
            UINT32 data_invalid_confirm = 0;
            // read 'write lock' and 'data invalid' information
            uret = rsa_pkey_info_read(&write_lock, &write_lock_confirm, &data_invalid, &data_invalid_confirm);
            if (uret == RTSL_ERR_OTP_SUCCESS) {
                uret = rsa_pkey_info_check(write_lock, write_lock_confirm, data_invalid, data_invalid_confirm);
                if (uret == RTSL_ERR_OTP_SUCCESS) {
                    UINT32 mask;

                    // The 3rd Public Key cannot be invalid.
                    data_invalid &= ~(1UL << 2U);

                    // make key status information
                    *KeyStatus = 0;

                    mask = 1UL << (5U + KeyIndex);
                    if ((write_lock & mask) != 0U) {
                        *KeyStatus |= 0x1U;
                    }

                    mask = 1UL << KeyIndex;
                    if ((data_invalid & mask) != 0U) {
                        *KeyStatus |= 0x2U;
                    }

                    // read key
                    uret = otp_read(address, KeyBuffer, 512, 1);

                }
            }
        }
    }
    return uret;
}

/**
 * Read Ambarella Unique ID from OTP
 * @param KeyBuffer  [OUT] Buffer contains Ambarella Unique ID.
 * @param BufferSize [IN] The size must be larger than 16 bytes.
 */
UINT32 AmbaRTSL_OtpAmbaUniqueIDRead(UINT8 *Buffer, UINT32 BufferSize)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    if ((BufferSize < 16U) || (Buffer == NULL)) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        uret = otp_read(BIT_ADDR_AMBA_UNIQUE_ID, Buffer, 16, 1);
    }
    return uret;
}

/**
 * Write customer's unique ID into OTP
 * @param KeyBuffer  [IN] Buffer contains customer's unique ID
 * @param BufferSize [IN] The size must be larger than 16 bytes.
 */
UINT32 AmbaRTSL_OtpCustomerUniIDWrite(const UINT8 *Buffer, UINT32 BufferSize)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    if ((BufferSize < 16U) || (Buffer == NULL)) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        UINT32 address = BIT_ADDR_CUSTOMER_UNIQUE_ID;
        uret = otp_program(address, Buffer, 16);
    }
    return uret;
}

/**
 * Read Customer's Unique ID from OTP
 * @param KeyBuffer  [OUT] Buffer contains Customer Unique ID.
 * @param BufferSize [IN] The size must be larger than 16 bytes.
 */
UINT32 AmbaRTSL_OtpCustomerUniIDRead(UINT8 *Buffer, UINT32 BufferSize)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    if ((BufferSize < 16U) || (Buffer == NULL)) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        uret = otp_read(BIT_ADDR_CUSTOMER_UNIQUE_ID, Buffer, 16, 1);
    }
    return uret;
}

static UINT8 u8_value_reverse(UINT8 Input)
{
    UINT8 x = Input;
    x = ((x & 0xF0U) >> 4U) | ((x & 0x0FU) << 4U);
    x = ((x & 0xCCU) >> 2U) | ((x & 0x33U) << 2U);
    x = ((x & 0xAAU) >> 1U) | ((x & 0x55U) << 1U);
    return x;
}

static void u128_value_reverse(const UINT8 *In, UINT8 *Out)
{
    UINT32 i;
    for (i = 0; i < 16U; i++) {
        Out[i] = u8_value_reverse(In[15U-i]);
    }
}

static UINT32 u128_value_compare(const UINT8 *In, const UINT8 *Out)
{
    UINT32 i;
    UINT32 uret = 0;
    for (i = 0; i < 16U; i++) {
        if (In[i] != Out[i]) {
            uret = 1;
            break;
        }
    }
    return uret;
}

/**
 * Read monotonic counter value from OTP
 * @param Counter  [OUT] monotonic counter value.
 */
UINT32 AmbaRTSL_OtpMonoCounterRead(UINT32 *Counter)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    UINT8  buffer[32];
    UINT8  confirm[16];
    if (Counter == NULL) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        uret = otp_read(BIT_ADDR_MONO_COUNTER, buffer, 32, 0); // monotonic counter doesn't have CRC8 protected
        if (uret == RTSL_ERR_OTP_SUCCESS) {
            // count the number of 1 in buffer
            UINT32 byte_idx;
            UINT32 bit_idx;
            *Counter = 0;
            for (byte_idx = 0; byte_idx < 16U; byte_idx++) {
                UINT8 byte_value = buffer[byte_idx];
                for (bit_idx = 0; bit_idx < 8U; bit_idx++) {
                    UINT8 mask = (UINT8)(1UL << bit_idx);
                    if ((byte_value & mask) != 0U) {
                        *Counter = *Counter + 1U;
                    }
                }
            }

            // check Monotonic count with Monotonic count confirm
            // Monotonic count confirm is the bit-reverse-order of Monotonic count
            u128_value_reverse(&buffer[16], &confirm[0]);
            if (u128_value_compare(&buffer[0], &confirm[0]) != 0U) {
                uret = RTSL_ERR_OTP_MC_CONFIRM;
            }
        }
    }
    return uret;
}

/**
 * Increase monotonic counter by one in OTP.
 */
UINT32 AmbaRTSL_OtpMonoCounterIncrease(void)
{
    UINT32 counter;
    UINT32 uret;
    UINT32 offset;

    // 0x8200 dir ---->  0x8300       <---- dir 0x8400
    // +-------------------+----------------------+
    // |  Counter          |    Counter Confirm   |
    // +-------------------+----------------------+

    uret = AmbaRTSL_OtpMonoCounterRead(&counter);
    if (uret == RTSL_ERR_OTP_SUCCESS) {
        if (counter < 128U) {
            // each entry is 64-bit but only first 32-bit is used
            offset = ((counter / 32U) * 64U) + (counter % 32U);
            uret = otp_bit_write(BIT_ADDR_MONO_COUNTER + offset);
            // need to write Monotonic count confirm
            // Monotonic count confirm is the bit-reverse-order of Monotonic count
            if (uret == RTSL_ERR_OTP_SUCCESS) {
                // each entry is 64-bit but only first 32-bit is used
                offset = 7U - (counter / 32U); // N-th 32-bit offset
                offset = (offset * 64U) + (31U - (counter % 32U)); // bit offset
                uret = otp_bit_write(BIT_ADDR_MONO_COUNTER + offset);
            }
        } else {
            uret = RTSL_ERR_OTP_REACH_MAX;
        }
    }
    return uret;
}

/**
 * Enable secure boot in OTP configuration zone.
 */
UINT32 AmbaRTSL_OtpSecureBootEnable(void)
{
    UINT32 uret;
    uret = otp_bit_write(BIT_ADDR_SECURE_BOOT);
    if (uret == RTSL_ERR_OTP_SUCCESS) {
        uret = otp_bit_write(BIT_ADDR_SECURE_BOOT + 64U);
        /* when enable secure boot, JTAG efuse shall be set also, otherwise JTAG is always enabled */
        if (uret == RTSL_ERR_OTP_SUCCESS) {
            uret = otp_bit_write(BIT_ADDR_JTAG_EFUSE);
        }
    }
    return uret;
}

/**
 * Set JTAG efuse in OTP configuration zone.
 * It will disable JTAG when in secure boot mode.
 */
UINT32 AmbaRTSL_OtpJtagEfuseSet(void)
{
    UINT32 uret;
    uret = otp_bit_write(BIT_ADDR_JTAG_EFUSE);
    return uret;
}

/**
 * Write AES key into OTP
 * @param KeyBuffer  [IN] Buffer contains AES key.
 * @param BufferSize [IN] The size must be larger than 32 bytes.
 * @param KeyIndex   [IN] Valid value: 0, 1, 2, 3.
 */
UINT32 AmbaRTSL_OtpAesKeyWrite(const UINT8 *KeyBuffer, UINT32 BufferSize, UINT32 KeyIndex)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    if ((BufferSize < 32U) || (KeyBuffer == NULL)) {
        uret = RTSL_ERR_OTP_PARAM;
    } else if (KeyIndex >= 4U) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        uret = otp_program(BIT_ADDR_AES_KEY_1 + (32U * KeyIndex * 8U), KeyBuffer, 32);
    }
    return uret;
}

/**
 * Read AES key from OTP
 * @param KeyBuffer  [OUT] Buffer contains AES key.
 * @param BufferSize [IN] The size must be larger than 32 bytes.
 * @param KeyIndex   [IN] Valid value: 0, 1, 2, 3.
 */
UINT32 AmbaRTSL_OtpAesKeyRead(UINT8 *KeyBuffer, UINT32 BufferSize, UINT32 KeyIndex)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    if ((BufferSize < 32U) || (KeyBuffer == NULL)) {
        uret = RTSL_ERR_OTP_PARAM;
    } else if (KeyIndex >= 4U) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        uret = otp_read(BIT_ADDR_AES_KEY_1 + (32U * KeyIndex * 8U), KeyBuffer, 32, 1);
    }
    return uret;
}

/**
 * Read data from OTP Reserved DX zone
 * @param KeyBuffer  [OUT] Buffer contains data.
 * @param BufferSize [IN] The size must be larger than 32 bytes.
 * @param Index      [IN] valid value: 0~5
 */
UINT32 AmbaRTSL_OtpReservedDXRead(UINT8 *Buffer, UINT32 BufferSize, UINT32 Index)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    if ((BufferSize < 32U) || (Buffer == NULL)) {
        uret = RTSL_ERR_OTP_PARAM;
    } else if (Index >= 6U) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        uret = otp_read(BIT_ADDR_RESERVED_DX_1 + (32U * Index * 8U), Buffer, 32, 1);
    }
    return uret;
}

/**
 * Write data into OTP Reserved DX zone
 * @param KeyBuffer  [IN] Buffer contains data.
 * @param BufferSize [IN] The size must be larger than 32 bytes.
 * @param Index      [IN] valid value: 0~5
 */
UINT32 AmbaRTSL_OtpReservedDXWrite(const UINT8 *Buffer, UINT32 BufferSize, UINT32 Index)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    if ((BufferSize < 32U) || (Buffer == NULL)) {
        uret = RTSL_ERR_OTP_PARAM;
    } else if (Index >= 6U) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        uret = otp_program(BIT_ADDR_RESERVED_DX_1 + (32U * Index * 8U), Buffer, 32);
    }
    return uret;
}

/**
 * Read data from OTP Reserved B2 zone
 * @param KeyBuffer  [OUT] Buffer contains data.
 * @param BufferSize [IN] The size must be larger than 16 bytes.
 */
UINT32 AmbaRTSL_OtpReservedB2Read(UINT8 *Buffer, UINT32 BufferSize)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    if ((BufferSize < 16U) || (Buffer == NULL)) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        uret = otp_read(BIT_ADDR_RESERVED_B2, Buffer, 16, 1);
    }
    return uret;
}
/**
 * Write data into OTP Reserved B2 zone
 * @param KeyBuffer  [OUT] Buffer contains data.
 * @param BufferSize [IN] The size must be larger than 16 bytes.
 */
UINT32 AmbaRTSL_OtpReservedB2Write(const UINT8 *Buffer, UINT32 BufferSize)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    if ((BufferSize < 16U) || (Buffer == NULL)) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        UINT32 address = BIT_ADDR_RESERVED_B2;
        uret = otp_program(address, Buffer, 16);
    }
    return uret;
}
/**
 * Revoke RSA public key inside OTP
 * @param KeyIndex   [IN] Valid value: 0, 1.
 */
UINT32 AmbaRTSL_OtpPublicKeyRevoke(UINT32 KeyIndex)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;

    if (KeyIndex > 1U) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        UINT32 write_lock = 0;
        UINT32 write_lock_confirm = 0;
        UINT32 data_invalid = 0;
        UINT32 data_invalid_confirm = 0;
        // read 'write lock' and 'data invalid' information
        // and check if the information is correct
        uret = rsa_pkey_info_read(&write_lock, &write_lock_confirm, &data_invalid, &data_invalid_confirm);
        if (uret == RTSL_ERR_OTP_SUCCESS) {
            uret = rsa_pkey_info_check(write_lock, write_lock_confirm, data_invalid, data_invalid_confirm);
            if (uret == RTSL_ERR_OTP_SUCCESS) {
                UINT32 mask = 1UL << KeyIndex;
                if ((data_invalid & mask) == 0U) {
                    // program 'data invalid' and 'data invalid confirm'
                    uret = otp_bit_write(BIT_ADDR_DATA_INVALID + KeyIndex);
                    if (uret == 0U) {
                        uret = otp_bit_write(BIT_ADDR_DATA_INVALID_CONFIRM + (31UL - KeyIndex));
                    }
                } else {
                    // if it is revoked, do nothing
                }
            }
        }
    }
    return uret;
}

#endif // #if defined(CONFIG_BUILD_SSP_OTP) || defined(CONFIG_QNX)

/**
 * Read data from OTP temperature sensor output data
 * @param Index     [IN]  Index.
 * @param Data_tsl  [OUT] temperature sensor data @ -40 degree.
 * @param Data_tsm  [OUT] temperature sensor data @  25 degree.
 * @param Data_tsh  [OUT] temperature sensor data @ 105 degree.
 */
UINT32 AmbaRTSL_OtpTempSensorRead(UINT32 Index, UINT32 *Data_tsl, UINT32 *Data_tsm, UINT32 *Data_tsh)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    UINT32 crc8_value;
    UINT32 Buffer = 0;
    UINT32 Address;

    if ((Data_tsl == NULL) || (Data_tsm == NULL) || (Data_tsh == NULL)) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        switch (Index) {
        case 0U:
            Address = BIT_ADDR_READ_TEMP_SENSOR0;
            break;
        case 1U:
            Address = BIT_ADDR_READ_TEMP_SENSOR1;
            break;
        case 2U:
            Address = BIT_ADDR_READ_TEMP_SENSOR2;
            break;
        case 3U:
            Address = BIT_ADDR_READ_TEMP_SENSOR3;
            break;
        case 4U:
            Address = BIT_ADDR_READ_TEMP_SENSOR4;
            break;
        case 5U:
            Address = BIT_ADDR_READ_TEMP_SENSOR5;
            break;
        case 6U:
            Address = BIT_ADDR_READ_TEMP_SENSOR6;
            break;
        case 7U:
            Address = BIT_ADDR_READ_TEMP_SENSOR7;
            break;
        case 8U:
            Address = BIT_ADDR_READ_TEMP_SENSOR8;
            break;
        case 9U:
            Address = BIT_ADDR_READ_TEMP_SENSOR9;
            break;
        default:
            Address = 0U;
            break;
        }

        if (Address != 0x0U) {
            uret = otp_uint32_read(Address, &Buffer, &crc8_value);
            *Data_tsh = (Buffer >> 18U) & 0x1FFU;
            *Data_tsm = (Buffer >> 9U) & 0x1FFU;
            *Data_tsl = Buffer & 0x1FFU;
        }
    }
    return uret;
}

