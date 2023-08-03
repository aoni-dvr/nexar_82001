/**
 *  @file AmbaRTSL_OTP.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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

#if defined(CONFIG_BUILD_SSP_OTP) || defined(CONFIG_QNX)

#define BIT_ADDR_SECURE_BOOT         0x06U   // 1 bit
#define BIT_ADDR_JTAG_EFUSE          0x3FU   // 1 bit
#define BIT_ADDR_WRITE_LOCK          0xC0U   // bit 5: C1 lock, bit 6: C2 lock, bit 7: C3 lock
#define BIT_ADDR_AMBA_UNIQUE_ID      0x3E00U // 128 bits
#define BIT_ADDR_DATA_INVALID        0x3E80U // bit 0: C1 invalid, bit1: C2 invalid
#define BIT_ADDR_CUSTOMER_UNIQUE_ID  0x3F00U // 128 bits
#define BIT_ADDR_RESERVED_B2         0x3F80U // 128 bits
#define BIT_ADDR_MONO_COUNTER        0x4100U // 256 bits
#define BIT_ADDR_AES_KEY_1           0x4200U // 256 bits x 4
#define BIT_ADDR_RESERVED_DX_1       0x4A00U // 256 bits x 6
#define BIT_ADDR_RSA_KEY_1           0x5000U // 4096 bits
#define BIT_ADDR_RSA_KEY_2           0x6000U // 4096 bits
#define BIT_ADDR_RSA_KEY_3           0x7000U // 4096 bits

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
            retry_count++;
            if (retry_count > 100000U) {
                uret = RTSL_ERR_OTP_WAIT_TIMEOUT;
                break;
            }
        }
    } while ((value & Mask) == 0U);
    return uret;
}

static UINT32 otp_uint32_read(UINT32 Address, UINT32 *Data)
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
        value &= ~(CTRL1_ADDR_MASK);
        value |= (Address & CTRL1_ADDR_MASK);
        AmbaCSL_OtpCtrl1Write(value);

        // Enable Read Operation
        ctrl1_reg_bit_set(READ_ENABLE);

        // Wait until Read Done
        uret = otp_controller_wait(READ_OBSV_DONE);

        if (uret == 0U) {
            *Data = AmbaCSL_OtpDataRead();
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

static UINT32 otp_bit_read(UINT32 Address, UINT32 *BitValue)
{
    UINT32 mask  = 0x1FU;
    UINT32 aaddr = Address & (~mask);
    UINT32 bfs = (Address & mask);
    UINT32 data = 0;
    UINT32 bit = 0;
    UINT32 uret;

    uret = otp_uint32_read(aaddr, &data);

    if (uret == 0U) {
        bfs = ((UINT32)1U) << bfs;
        if ((data & bfs) != 0U) {
            bit = 1;
        }
        *BitValue = bit;
    }

    return uret;
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
                value &= ~(CTRL1_ADDR_MASK);
                value |= (Address & CTRL1_ADDR_MASK);
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
    UINT32 base_one = 1;

    // program data one by one bit
    for (i = 0; i < ByteSize; i++) {
        UINT8 byte_data = Buffer[i];
        for (shift = 0; shift < 8U; shift++) {
            UINT32 bit_data = ((UINT32)byte_data >> shift) & base_one;
            if (bit_data != 0U) {
                uret = otp_bit_write(address + bit_offset);
                if (uret != RTSL_ERR_OTP_SUCCESS) {
                    break;
                }
            }
            bit_offset++;
        }
        if (uret != RTSL_ERR_OTP_SUCCESS) {
            break;
        }
    }
    return uret;
}

static UINT32 otp_read(UINT32 OtpBitAddress, UINT8 *Buffer, UINT32 ByteSize)
{
    UINT32 uret = RTSL_ERR_OTP_SUCCESS;
    UINT32 i, data;
    for (i = 0; i < (ByteSize/4U); i++) {
        uret = otp_uint32_read(OtpBitAddress + (i * 32U), &data);
        if (uret != RTSL_ERR_OTP_SUCCESS) {
            break;
        } else {
            Buffer[i*4U] = (UINT8)(data & 0x0FFU);
            Buffer[(i*4U)+1U] = (UINT8)((data >> 8U) & 0x0FFU);
            Buffer[(i*4U)+2U] = (UINT8)((data >> 16U) & 0x0FFU);
            Buffer[(i*4U)+3U] = (UINT8)((data >> 24U) & 0x0FFU);
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
        switch (KeyIndex) {
        case 0:
            address = BIT_ADDR_RSA_KEY_1;
            lock_addr = BIT_ADDR_WRITE_LOCK + 5U;
            break;
        case 1:
            address = BIT_ADDR_RSA_KEY_2;
            lock_addr = BIT_ADDR_WRITE_LOCK + 6U;
            break;
        case 2:
            address = BIT_ADDR_RSA_KEY_3;
            lock_addr = BIT_ADDR_WRITE_LOCK + 7U;
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
            }
        }
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
            UINT32 data_invalid = 0;
            UINT32 base_one = 1;
            // read 'write lock' bits
            uret = otp_uint32_read(BIT_ADDR_WRITE_LOCK, &write_lock);
            if (uret == RTSL_ERR_OTP_SUCCESS) {
                // read 'data invalid' bits
                uret = otp_uint32_read(BIT_ADDR_DATA_INVALID, &data_invalid);
                if (uret == RTSL_ERR_OTP_SUCCESS) {
                    UINT32 mask;

                    // The 3rd Public Key cannot be invalid.
                    data_invalid &= ~(base_one << 2U);

                    // make key status information
                    *KeyStatus = 0;

                    mask = base_one << (5U + KeyIndex);
                    if ((write_lock & mask) != 0U) {
                        *KeyStatus |= 0x1U;
                    }

                    mask = base_one << KeyIndex;
                    if ((data_invalid & mask) != 0U) {
                        *KeyStatus |= 0x2U;
                    }

                    // read key
                    uret = otp_read(address, KeyBuffer, 512);
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
        uret = otp_read(BIT_ADDR_AMBA_UNIQUE_ID, Buffer, 16);
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
        uret = otp_read(BIT_ADDR_CUSTOMER_UNIQUE_ID, Buffer, 16);
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
    if (Counter == NULL) {
        uret = RTSL_ERR_OTP_PARAM;
    } else {
        uret = otp_read(BIT_ADDR_MONO_COUNTER, buffer, 32);
        if (uret == RTSL_ERR_OTP_SUCCESS) {
            // count the number of 1 in buffer
            UINT32 byte_idx;
            UINT32 bit_idx;
            *Counter = 0;
            for (byte_idx = 0; byte_idx < 32U; byte_idx++) {
                UINT8 byte_value = buffer[byte_idx];
                for (bit_idx = 0; bit_idx < 8U; bit_idx++) {
                    UINT8 mask = (UINT8)(1UL << bit_idx);
                    if ((byte_value & mask) != 0U) {
                        *Counter = *Counter + 1U;
                    }
                }
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

    uret = AmbaRTSL_OtpMonoCounterRead(&counter);
    if (uret == RTSL_ERR_OTP_SUCCESS) {
        if (counter < 256U) {
            uret = otp_bit_write(BIT_ADDR_MONO_COUNTER + counter);
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
        uret = otp_bit_write(BIT_ADDR_SECURE_BOOT + 32U);
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
        uret = otp_read(BIT_ADDR_AES_KEY_1 + (32U * KeyIndex * 8U), KeyBuffer, 32);
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
        uret = otp_read(BIT_ADDR_RESERVED_DX_1 + (32U * Index * 8U), Buffer, 32);
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
        uret = otp_read(BIT_ADDR_RESERVED_B2, Buffer, 16);
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
        UINT32 data_invalid = 0;
        // read 'data invalid' bits
        uret = otp_uint32_read(BIT_ADDR_DATA_INVALID, &data_invalid);
        if (uret == RTSL_ERR_OTP_SUCCESS) {
            UINT32 mask;

            mask = ((UINT32)1U) << KeyIndex;
            if ((data_invalid & mask) == 0U) {
                uret = otp_bit_write(BIT_ADDR_DATA_INVALID + KeyIndex);
            } else {
                // if it is revoked, do nothing
            }
        }
    }
    return uret;
}

#endif // #if defined(CONFIG_BUILD_SSP_OTP) || defined(CONFIG_QNX)

