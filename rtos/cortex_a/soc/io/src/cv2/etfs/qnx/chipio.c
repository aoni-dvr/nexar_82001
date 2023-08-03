/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
 * Copyright 2016, Freescale Semiconductor, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include <stdlib.h>
#include <stdio.h>
#include <fs/etfs.h>
#include <arm/inout.h>
#include <string.h>
#include <sys/neutrino.h>
#include <unistd.h>
#include <errno.h>

#include <arm/imx/imx_apbh.h>
#include <arm/imx/imx_gpmi.h>
#include <aarch64/imx8_common/imx_bch.h>
//#include <arm/imx/imx_bch.h>

#include "chipio.h"
#include "devio.h"
#include "dma_descriptor.h"
#include "bch_ecc.h"
#include "apbh_dma.h"
#include "gpmi_pio.h"

/**
 * @file       imx-micron/chipio.c
 * @addtogroup etfs_chipio Chip interface
 * @{
 */

/**
 * Creates block erase descriptor.
 *
 * @param superStruct Block erase descriptor.
 * @param page        Page number.
 */
void create_blockErase_descriptor(dma_blk_erase_t *superStruct, unsigned page)
{
    uint8_t u32ChipSelect = 0;
    //
    superStruct->address_and_cmd_buf[0] = NANDCMD_BLOCK_ERASE;  //cmd
    superStruct->address_and_cmd_buf[1] = NAND_ADDR_ROW1(page);
    superStruct->address_and_cmd_buf[2] = NAND_ADDR_ROW2(page);
    superStruct->address_and_cmd_buf[3] = NAND_ADDR_ROW3(page);
    superStruct->erase_blk_confirm_cmd = NANDCMD_BLOCK_ERASE_CONFIRM;

    //CMD + ADDRESS
    superStruct->tx_cle1_row_dma.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->tx_cle2_dma);
    superStruct->tx_cle1_row_dma.cmd.U = NAND_DMA_COMMAND_CMD((NAND_ROW_ADDRESS_CYCLES + 1), 0, NAND_LOCK,3);
    superStruct->tx_cle1_row_dma.bar = virtual_to_physical_addr(&superStruct->address_and_cmd_buf);
    superStruct->tx_cle1_row_dma.gpmi_ctrl0.U = NAND_DMA_COMMAND_PIO(u32ChipSelect, (NAND_ROW_ADDRESS_CYCLES + 1), ASSERT_CS);
    superStruct->tx_cle1_row_dma.gpmi_compare.U = 0;
    superStruct->tx_cle1_row_dma.gpmi_eccctrl.U = 0;

    // ERASE ACK
    superStruct->tx_cle2_dma.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->wait_dma);
    superStruct->tx_cle2_dma.cmd.U = NAND_DMA_COMMAND_CMD(1, 0, NAND_LOCK,1);
    superStruct->tx_cle2_dma.bar = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->erase_blk_confirm_cmd);
    superStruct->tx_cle2_dma.gpmi_ctrl0.U = NAND_DMA_COMMAND_PIO(u32ChipSelect, 1, ASSERT_CS);

    // WAIT
    superStruct->wait_dma.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->sense);
    superStruct->wait_dma.cmd.U = NAND_DMA_WAIT4RDY_CMD;
    superStruct->wait_dma.bar = 0x0;
    superStruct->wait_dma.gpmi_ctrl0.U = NAND_DMA_WAIT4RDY_PIO(u32ChipSelect);

    //SENSE
    superStruct->sense.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->success);
    superStruct->sense.cmd.U = NAND_DMA_SENSE_CMD(0);
    superStruct->sense.bar = virtual_to_physical_addr(&superStruct->failure);
    superStruct->sense.gpmi_ctrl0.U = 0;

    // TERMINATOR
    superStruct->success.nxt = (apbh_dma_t*)0x0;
    superStruct->success.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->success.bar = (void *)NAND_EOK;
    superStruct->failure.nxt = (apbh_dma_t*)0x0;
    superStruct->failure.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->failure.bar = (void*)0x03;
}

/**
 * Creates read ECC descriptor (read data using BCH peripheral).
 *
 * @param superStruct Read ECC descriptor.
 * @param page        Page number.
 * @param ret_data    Pointer to read data buffer.
 */
void create_readEcc_descriptor(dma_readEcc_device_t *superStruct, unsigned page, void *ret_data)
{
    superStruct->address_and_command_buf[0] = NANDCMD_READ;               // read cmd start
    superStruct->address_and_command_buf[1] = NAND_ADDR_COL1(0);          // column
    superStruct->address_and_command_buf[2] = NAND_ADDR_COL2(0);          // column
    superStruct->address_and_command_buf[3] = NAND_ADDR_ROW1(page);
    superStruct->address_and_command_buf[4] = NAND_ADDR_ROW2(page);
    superStruct->address_and_command_buf[5] = NAND_ADDR_ROW3(page);
    superStruct->read_confirm_command = NANDCMD_READ_CONFIRM;

    //CMD + ADDRESS (addr. cycles depends on memory size, see datasheet)
    superStruct->cmd_addr.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->tx_cle2_dma);
    superStruct->cmd_addr.cmd.U = NAND_DMA_COMMAND_CMD((1 + NAND_ADDRESS_CYCLES), 0, NAND_LOCK,3);
    superStruct->cmd_addr.bar = virtual_to_physical_addr(&superStruct->address_and_command_buf);
    superStruct->cmd_addr.gpmi_ctrl0.U = NAND_DMA_COMMAND_PIO(0, (1 + NAND_ADDRESS_CYCLES), 1);
    superStruct->cmd_addr.gpmi_compare.U = 0;                     //Not used
    superStruct->cmd_addr.gpmi_eccctrl.U = NAND_DMA_ECC_PIO(IMX_GPMI_ECCCTRL_ENABLE_ECC_BV_DISABLE);

    //CMD READ CONFIRM
    superStruct->tx_cle2_dma.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->wait1);
    superStruct->tx_cle2_dma.cmd.U = NAND_DMA_COMMAND_CMD(1, 0, NAND_LOCK,1);
    superStruct->tx_cle2_dma.bar = virtual_to_physical_addr(&superStruct->read_confirm_command);
    superStruct->tx_cle2_dma.gpmi_ctrl0.U = NAND_DMA_COMMAND_PIO(0, 1, 1);

    //WAIT4READY
    superStruct->wait1.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->sense1);
    superStruct->wait1.cmd.U = NAND_DMA_WAIT4RDY_CMD;
    superStruct->wait1.bar = 0;
    superStruct->wait1.gpmi_ctrl0.U = NAND_DMA_WAIT4RDY_PIO(0);

    //SENSE 1 - check for successful Ready. BAR points to alternate branch if timeout occurs.
    superStruct->sense1.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->receive_data);
    superStruct->sense1.cmd.U = NAND_DMA_SENSE_CMD(0);
    superStruct->sense1.bar = virtual_to_physical_addr(&superStruct->failure);
    superStruct->sense1.gpmi_ctrl0.U = 0;

    //DATA RECEIVER
    superStruct->receive_data.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->success);
    superStruct->receive_data.cmd.U = NAND_DMA_RX_CMD_ECC(0, 0);
    superStruct->receive_data.bar = 0;                                            // Not used.
    superStruct->receive_data.gpmi_ctrl0.U = NAND_DMA_RX_PIO(0, PAGESIZE);
    superStruct->receive_data.gpmi_compare.U = 0;                                 // Not used.
    superStruct->receive_data.gpmi_eccctrl.U = (BV_FLD(GPMI_ECCCTRL, ECC_CMD, DECODE) |
            BV_FLD(GPMI_ECCCTRL, ENABLE_ECC, ENABLE) |
            IMX_GPMI_ECCCTRL_BUFFER_MASK(0x1FF));
    superStruct->receive_data.gpmi_ecccount.U = IMX_GPMI_ECCCOUNT_COUNT(PAGESIZE);

    // Setup the data buffer.
    //assert(((uint32_t)dataBuffer & 0x3) == 0);
    //TODO - nand - Is this needed? (virtual_to_physical_addr(ret_data)) && 0xFFFFFFFC;
    if (ret_data != 0) {
        superStruct->receive_data.gpmi_payload.U = (uint32_t)(virtual_to_physical_addr(ret_data));
    }

    // And the Auxiliary buffer here. Commented out but we will keep it here.
    //superStruct->receive_data.gpmi_auxiliary.U = (virtual_to_physical_addr(ret_data));

    //WAIT4READY 2
    superStruct->wait2.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->sense2);
    // Configure to send 3 GPMI PIO reads.
    superStruct->wait2.cmd.U = (IMX_APBH_CHn_CMD_CMDWORDS(3)
                                | IMX_APBH_CHn_CMD_WAIT4ENDCMD(1)
                                | IMX_APBH_CHn_CMD_NANDWAIT4READY(1)
                                | IMX_APBH_CHn_CMD_CHAIN(1)
                                | IMX_APBH_CHn_CMD_NANDLOCK(1)
                                | BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->wait2.bar = 0x0;
    // Disable the Chip Select and other outstanding GPMI things.
    superStruct->wait2.gpmi_ctrl0.U = (BV_FLD(GPMI_CTRL0, COMMAND_MODE, WAIT_FOR_READY)
                                       | IMX_GPMI_CTRL0_WORD_LENGTH(IMX_GPMI_CTRL0_WORD_LENGTH_BV_8_BIT)
                                       | IMX_GPMI_CTRL0_LOCK_CS(0)
                                       | IMX_GPMI_CTRL0_CS(0)
                                       | BV_FLD(GPMI_CTRL0, ADDRESS, NAND_DATA)
                                       | IMX_GPMI_CTRL0_ADDRESS_INCREMENT(0)
                                       | IMX_GPMI_CTRL0_XFER_COUNT(0));
    // Ignore the compare - we need to skip over it.
    superStruct->wait2.gpmi_compare.U = 0;
    // Disable the ECC Block.
    superStruct->wait2.gpmi_eccctrl.U = IMX_GPMI_ECCCTRL_ENABLE_ECC(IMX_GPMI_ECCCTRL_ENABLE_ECC_BV_DISABLE);

    //SENSE 2
    superStruct->sense2.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->success);
    superStruct->sense2.cmd.U = NAND_DMA_SENSE_CMD(0);
    superStruct->sense2.bar = virtual_to_physical_addr(&superStruct->failure);
    superStruct->sense2.gpmi_ctrl0.U = 0;

    // TERMINATOR
    superStruct->success.nxt = (apbh_dma_t*)0x0;
    superStruct->success.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->success.bar = (void *)NAND_EOK;
    superStruct->failure.nxt = (apbh_dma_t*)0x0;
    superStruct->failure.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->failure.bar = (void*)0x03;
}

/**
 * Creates read raw data descriptor (reads whole page (if there is ECC, reads it also))
 *
 * @param superStruct Read raw data descriptor.
 * @param data_size   Size of data to read.
 * @param page        Page number.
 * @param ret_data    Pointer to read data buffer.
 */
void create_readRaw_descriptor(dma_readRaw_device_t *superStruct, uint32_t data_size, unsigned page, void *ret_data)
{
    superStruct->address_and_command_buf[0] = NANDCMD_READ;         // read cmd start
    superStruct->address_and_command_buf[1] = NAND_ADDR_COL1(0);    // column
    superStruct->address_and_command_buf[2] = NAND_ADDR_COL1(0);    // column
    superStruct->address_and_command_buf[3] = NAND_ADDR_ROW1(page);
    superStruct->address_and_command_buf[4] = NAND_ADDR_ROW2(page);
    superStruct->address_and_command_buf[5] = NAND_ADDR_ROW3(page);

    superStruct->read_confirm_command = NANDCMD_READ_CONFIRM;

    //CMD + ADDRESS (addr. cycles depends on memory size, see data-sheet)
    superStruct->cmd_addr.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->tx_cle2_dma);
    superStruct->cmd_addr.cmd.U = NAND_DMA_COMMAND_CMD((1 + NAND_ADDRESS_CYCLES), 0, NAND_LOCK,3);
    superStruct->cmd_addr.bar = virtual_to_physical_addr(&superStruct->address_and_command_buf);
    superStruct->cmd_addr.gpmi_ctrl0.U = NAND_DMA_COMMAND_PIO(0, (1 + NAND_ADDRESS_CYCLES), 1);
    superStruct->cmd_addr.gpmi_compare.U = 0;
    superStruct->cmd_addr.gpmi_eccctrl.U = 0;

    //CMD READ CONFIRM
    superStruct->tx_cle2_dma.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->wait1);
    superStruct->tx_cle2_dma.cmd.U = NAND_DMA_COMMAND_CMD(1, 0, NAND_LOCK,1);
    superStruct->tx_cle2_dma.bar = virtual_to_physical_addr(&superStruct->read_confirm_command);
    superStruct->tx_cle2_dma.gpmi_ctrl0.U = NAND_DMA_COMMAND_PIO(0, 1, 1);

    //WAIT4READY
    superStruct->wait1.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->sense1);
    superStruct->wait1.cmd.U = NAND_DMA_WAIT4RDY_CMD;
    superStruct->wait1.bar = 0;
    superStruct->wait1.gpmi_ctrl0.U = NAND_DMA_WAIT4RDY_PIO(0);

    //SENSE 1 - check for successful Ready. BAR points to alternate branch if timeout occurs.
    superStruct->sense1.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->receive_data);
    superStruct->sense1.cmd.U = NAND_DMA_SENSE_CMD(0);
    superStruct->sense1.bar = virtual_to_physical_addr(&superStruct->failure);
    superStruct->sense1.gpmi_ctrl0.U = 0;

    //DATA RECEIVER
    superStruct->receive_data.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->wait2);
    superStruct->receive_data.cmd.U = NAND_DMA_RX_NO_ECC_CMD(data_size, 0);
    superStruct->receive_data.bar = virtual_to_physical_addr(ret_data);
    superStruct->receive_data.gpmi_ctrl0.U = NAND_DMA_RX_PIO(0, data_size);

    //WAIT4READY 2
    superStruct->wait2.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->sense2);
    superStruct->wait2.cmd.U = NAND_DMA_WAIT4RDY_CMD;
    superStruct->wait2.bar = 0x0;
    superStruct->wait2.gpmi_ctrl0.U = NAND_DMA_WAIT4RDY_PIO(0);

    //SENSE 2
    superStruct->sense2.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->success);
    superStruct->sense2.cmd.U = NAND_DMA_SENSE_CMD(0);
    superStruct->sense2.bar = virtual_to_physical_addr(&superStruct->failure);
    superStruct->sense2.gpmi_ctrl0.U = 0;

    // TERMINATOR
    superStruct->success.nxt = (apbh_dma_t*)0x0;
    superStruct->success.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->success.bar = (void *)NAND_EOK;
    superStruct->failure.nxt = (apbh_dma_t*)0x0;
    superStruct->failure.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->failure.bar = (void*)0x03;
}

/**
 * Creates write ECC descriptor (write data using BCH peripheral)
 *
 * @param superStruct Write ECC descriptor.
 * @param page        Page number.
 * @param write_data  Pointer to write data buffer.
 * @param data_size   Size of data to write.
 */
void create_writeEcc_descriptor(dma_programEcc_t *superStruct, unsigned page, void *write_data, uint32_t data_size)
{
    superStruct->address_and_cmd_buf[0] = NANDCMD_PAGE_PROGRAM;   //cmd
    superStruct->address_and_cmd_buf[1] = NAND_ADDR_COL1(0);      //column
    superStruct->address_and_cmd_buf[2] = NAND_ADDR_COL2(0);      //column
    superStruct->address_and_cmd_buf[3] = NAND_ADDR_ROW1(page);   //row
    superStruct->address_and_cmd_buf[4] = NAND_ADDR_ROW2(page);   //row
    superStruct->address_and_cmd_buf[5] = NAND_ADDR_ROW3(page);   //row
    superStruct->wr_confirm_command = NANDCMD_PAGE_PROGRAM_CONFIRM;
    uint32_t u32ChipSelect = 0;

    //CMD + ADDRESS
    superStruct->tx_cle1_addr_dma.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->tx_data_dma);
    superStruct->tx_cle1_addr_dma.cmd.U = NAND_DMA_COMMAND_CMD(NAND_ADDRESS_CYCLES + 1, 0, NAND_LOCK, 3);
    superStruct->tx_cle1_addr_dma.bar = virtual_to_physical_addr(&superStruct->address_and_cmd_buf);
    superStruct->tx_cle1_addr_dma.gpmi_ctrl0.U = NAND_DMA_COMMAND_PIO(u32ChipSelect, NAND_ADDRESS_CYCLES + 1, ASSERT_CS);
    superStruct->tx_cle1_addr_dma.gpmi_compare.U = 0;
    superStruct->tx_cle1_addr_dma.gpmi_eccctrl.U = 0;

    //WAIT4READY
    superStruct->wait1.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->sense1);
    superStruct->wait1.cmd.U = NAND_DMA_WAIT4RDY_CMD;
    superStruct->wait1.bar = 0;
    superStruct->wait1.gpmi_ctrl0.U = NAND_DMA_WAIT4RDY_PIO(0);

    //SENSE 1 - check for successful Ready. BAR points to alternate branch if timeout occurs.
    superStruct->sense1.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->tx_data_dma);
    superStruct->sense1.cmd.U = NAND_DMA_SENSE_CMD(0);
    superStruct->sense1.bar = virtual_to_physical_addr(&superStruct->failure);
    superStruct->sense1.gpmi_ctrl0.U = 0;

    //DATA
    superStruct->tx_data_dma.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->tx_cle2_dma);
    superStruct->tx_data_dma.cmd.U = NAND_DMA_TXDATA_CMD(0, 0, 5, 1, NO_DMA_XFER);
    superStruct->tx_data_dma.bar = 0;//virtual_to_physical_addr(write_data);
    superStruct->tx_data_dma.gpmi_ctrl0.U = NAND_DMA_TXDATA_PIO(u32ChipSelect, 0);
    superStruct->tx_data_dma.gpmi_compare.U = 0;
    superStruct->tx_data_dma.gpmi_eccctrl.U = (BV_FLD(GPMI_ECCCTRL, ECC_CMD, ENCODE) |
            BV_FLD(GPMI_ECCCTRL, ENABLE_ECC, ENABLE) |
            IMX_GPMI_ECCCTRL_BUFFER_MASK(0x1FF));
    superStruct->tx_data_dma.gpmi_ecccount.U = IMX_GPMI_ECCCOUNT_COUNT(PAGESIZE);

    if (write_data != 0) {
        superStruct->tx_data_dma.gpmi_payload.U = (uint32_t)virtual_to_physical_addr(write_data);
    }

    // Auxiliary buffer commented out, but could be useful in future.
    // superStruct->tx_data_dma.gpmi_auxiliary.U = (uint32_t)virtual_to_physical_addr(write_data);

    //CMD ACK
    superStruct->tx_cle2_dma.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->wait_dma);
    superStruct->tx_cle2_dma.cmd.U = NAND_DMA_COMMAND_CMD(1,0,NAND_LOCK,3);
    superStruct->tx_cle2_dma.bar = virtual_to_physical_addr(&superStruct->wr_confirm_command);
    superStruct->tx_cle2_dma.gpmi_ctrl0.U = NAND_DMA_COMMAND_PIO(u32ChipSelect, 1, ASSERT_CS);
    superStruct->tx_cle2_dma.gpmi_compare.U = 0;
    superStruct->tx_cle2_dma.gpmi_eccctrl.U = 0; // disable ecc

    //WAIT4READY
    superStruct->wait_dma.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->sense);
    superStruct->wait_dma.cmd.U = NAND_DMA_WAIT4RDY_CMD;
    superStruct->wait_dma.bar = 0x0;
    superStruct->wait_dma.gpmi_ctrl0.U = NAND_DMA_WAIT4RDY_PIO(u32ChipSelect);

    //SENSE 1 - check for successful Ready. BAR points to alternate branch if timeout occurs.
    superStruct->sense.nxt = (apbh_dma_t*)virtual_to_physical_addr(&superStruct->success);
    superStruct->sense.cmd.U = NAND_DMA_SENSE_CMD(0);
    superStruct->sense.bar = virtual_to_physical_addr(&superStruct->failure);

    // TERMINATOR
    superStruct->success.nxt = (apbh_dma_t*)0x0;
    superStruct->success.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->success.bar = (void *)NAND_EOK;
    superStruct->failure.nxt = (apbh_dma_t*)0x0;
    superStruct->failure.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->failure.bar = (void*)0x03;
}

/**
 * Creates write data descriptor (raw -> without BCH).
 *
 * @param superStruct Write raw data descriptor.
 * @param page        Page number.
 * @param write_data  Pointer to data buffer.
 * @param data_size   Data size.
 */
void create_writeRaw_descriptor(dma_programRaw_t *superStruct, unsigned page, void *write_data, uint32_t data_size)
{
    uint32_t u32ChipSelect = 0;

    superStruct->address_and_cmd_buf[0] = NANDCMD_PAGE_PROGRAM;     //cmd
    superStruct->address_and_cmd_buf[1] = 0;                        //column
    superStruct->address_and_cmd_buf[2] = 0;                        //column
    superStruct->address_and_cmd_buf[3] = NAND_ADDR_ROW1(page);     //row
    superStruct->address_and_cmd_buf[4] = NAND_ADDR_ROW1(page);;    //row
    superStruct->address_and_cmd_buf[5] = NAND_ADDR_ROW1(page);;    //row
    superStruct->wr_confirm_command = NANDCMD_PAGE_PROGRAM_CONFIRM; //cmd

    //CMD + ADDRESS
    superStruct->tx_cle1_addr_dma.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->tx_data_dma);
    superStruct->tx_cle1_addr_dma.cmd.U = NAND_DMA_COMMAND_CMD(NAND_ADDRESS_CYCLES + 1, 0, NAND_LOCK, 3);
    superStruct->tx_cle1_addr_dma.bar = virtual_to_physical_addr(&superStruct->address_and_cmd_buf);
    superStruct->tx_cle1_addr_dma.gpmi_ctrl0.U = NAND_DMA_COMMAND_PIO(u32ChipSelect, NAND_ADDRESS_CYCLES + 1, ASSERT_CS);
    superStruct->tx_cle1_addr_dma.gpmi_compare.U = 0;
    superStruct->tx_cle1_addr_dma.gpmi_eccctrl.U = 0;

    //DATA
    superStruct->tx_data_dma.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->tx_cle2_dma);
    superStruct->tx_data_dma.cmd.U = NAND_DMA_TXDATA_CMD(data_size, 0, 1, 1, DMA_READ);
    if (write_data != 0) {
        superStruct->tx_data_dma.bar = virtual_to_physical_addr(write_data);
    }
    superStruct->tx_data_dma.gpmi_ctrl0.U = NAND_DMA_TXDATA_PIO(u32ChipSelect, data_size);

    //CMD ACK
    superStruct->tx_cle2_dma.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->wait_dma);
    superStruct->tx_cle2_dma.cmd.U = NAND_DMA_COMMAND_CMD(1,0,NAND_LOCK,3);
    superStruct->tx_cle2_dma.bar = virtual_to_physical_addr(&superStruct->wr_confirm_command);
    superStruct->tx_cle2_dma.gpmi_ctrl0.U = NAND_DMA_COMMAND_PIO(u32ChipSelect, 1, ASSERT_CS);
    superStruct->tx_cle2_dma.gpmi_compare.U = 0;
    superStruct->tx_cle2_dma.gpmi_eccctrl.U = 0; //NAND_DMA_ECC_PIO(IMX_GPMI_ECCCTRL_ENABLE_ECC_BV_DISABLE);

    //WAIT4READY
    superStruct->wait_dma.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->sense);
    superStruct->wait_dma.cmd.U = NAND_DMA_WAIT4RDY_CMD;
    superStruct->wait_dma.bar = 0x0;
    superStruct->wait_dma.gpmi_ctrl0.U = NAND_DMA_WAIT4RDY_PIO(u32ChipSelect);

    //SENSE 1 - check for successful Ready. BAR points to alternate branch if timeout occurs.
    superStruct->sense.nxt = (apbh_dma_t*)virtual_to_physical_addr(&superStruct->success);
    superStruct->sense.cmd.U = NAND_DMA_SENSE_CMD(0);
    superStruct->sense.bar = virtual_to_physical_addr(&superStruct->failure);

    // TERMINATOR
    superStruct->success.nxt = (apbh_dma_t*)0x0;
    superStruct->success.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->success.bar = (void *)NAND_EOK;
    superStruct->failure.nxt = (apbh_dma_t*)0x0;
    superStruct->failure.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->failure.bar = (void*)0x03;
}

/**
 * Creates read status (enhanced).
 *
 * @param superStruct   Read status descriptor (enhanced).
 * @param memory_status Memory status.
 * @param page          Page number.
 */
void create_readStatus2_descriptor(dma_read_status_t *superStruct, uint8_t *memory_status, unsigned page)
{
    superStruct->status2_and_addr_command_buf[0] = NANDCMD_READ_STATUS_ENHANCED;
    superStruct->status2_and_addr_command_buf[1] = NAND_ADDR_ROW1(page);
    superStruct->status2_and_addr_command_buf[2] = NAND_ADDR_ROW2(page);
    superStruct->status2_and_addr_command_buf[3] = NAND_ADDR_ROW3(page);
    uint8_t chipSelect = 0;

    //WAIT 1
    superStruct->wait1.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->sense1);
    superStruct->wait1.cmd.U = NAND_DMA_WAIT4RDY_CMD;
    superStruct->wait1.bar = 0;
    superStruct->wait1.gpmi_ctrl0.U = NAND_DMA_WAIT4RDY_PIO(chipSelect);

    //SENSE 1 - check for successful Ready. BAR points to alternate branch if timeout occurs.
    superStruct->sense1.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->read_status_cmd);
    superStruct->sense1.cmd.U = NAND_DMA_SENSE_CMD(0);
    superStruct->sense1.bar = virtual_to_physical_addr(&superStruct->failure);
    superStruct->sense1.gpmi_ctrl0.U = 0;

    //ID_CMD
    superStruct->read_status_cmd.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->receive_data);
    superStruct->read_status_cmd.cmd.U = NAND_DMA_COMMAND_CMD(3+1, 0, NAND_LOCK, 3);
    superStruct->read_status_cmd.bar = virtual_to_physical_addr(&superStruct->status2_and_addr_command_buf);
    superStruct->read_status_cmd.gpmi_ctrl0.U = NAND_DMA_COMMAND_PIO(0, 3+1, 1);
    superStruct->read_status_cmd.gpmi_compare.U = 0;
    superStruct->read_status_cmd.gpmi_eccctrl.U = 0;

    //RECEIVER
    superStruct->receive_data.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->success);
    superStruct->receive_data.cmd.U = NAND_DMA_RX_NO_ECC_CMD(NANDCMD_READ_STATUS_SIZE, 0);
    superStruct->receive_data.bar = virtual_to_physical_addr(memory_status);
    superStruct->receive_data.gpmi_ctrl0.U = NAND_DMA_RX_PIO(0, NANDCMD_READ_STATUS_SIZE);

    // TERMINATOR
    superStruct->success.nxt = (apbh_dma_t*)0x0;
    superStruct->success.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->success.bar = (void *)NAND_EOK;
    superStruct->failure.nxt = (apbh_dma_t*)0x0;
    superStruct->failure.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->failure.bar = (void*)0x03;
}

/**
 * Creates read status descriptor.
 *
 * @param superStruct   Read status descriptor.
 * @param memory_status Memory status value.
 */
void create_readStatus_descriptor(dma_read_status_t *superStruct, uint8_t *memory_status)
{
    superStruct->status_command = NANDCMD_READ_STATUS;
    uint8_t chipSelect = 0;

    //WAIT 1
    superStruct->wait1.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->sense1);
    superStruct->wait1.cmd.U = NAND_DMA_WAIT4RDY_CMD;
    superStruct->wait1.bar = 0;
    superStruct->wait1.gpmi_ctrl0.U = NAND_DMA_WAIT4RDY_PIO(chipSelect);

    //SENSE 1 - check for successful Ready. BAR points to alternate branch if timeout occurs.
    superStruct->sense1.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->read_status_cmd);
    superStruct->sense1.cmd.U = NAND_DMA_SENSE_CMD(0);
    superStruct->sense1.bar = virtual_to_physical_addr(&superStruct->failure);
    superStruct->sense1.gpmi_ctrl0.U = 0;

    //ID_CMD
    superStruct->read_status_cmd.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->receive_data);
    superStruct->read_status_cmd.cmd.U = NAND_DMA_COMMAND_CMD(NANDCMD_READ_STATUS_SIZE, 0, NAND_LOCK, 3);
    superStruct->read_status_cmd.bar = virtual_to_physical_addr(&superStruct->status_command);
    superStruct->read_status_cmd.gpmi_ctrl0.U = NAND_DMA_COMMAND_PIO(0, NANDCMD_READ_STATUS_SIZE, 1);
    superStruct->read_status_cmd.gpmi_compare.U = 0;
    superStruct->read_status_cmd.gpmi_eccctrl.U = 0;

    //RECEIVER
    superStruct->receive_data.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->success);
    superStruct->receive_data.cmd.U = NAND_DMA_RX_NO_ECC_CMD(NANDCMD_READ_STATUS_SIZE, 0);
    superStruct->receive_data.bar = virtual_to_physical_addr(memory_status);
    superStruct->receive_data.gpmi_ctrl0.U = NAND_DMA_RX_PIO(0, NANDCMD_READ_STATUS_SIZE);

    // TERMINATOR
    superStruct->success.nxt = (apbh_dma_t*)0x0;
    superStruct->success.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->success.bar = (void *)NAND_EOK;
    superStruct->failure.nxt = (apbh_dma_t*)0x0;
    superStruct->failure.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->failure.bar = (void*)0x03;
}

/**
 * Creates read identification descriptor.
 *
 * @param superStruct Read identification descriptor.
 * @param ret_raw_id  Raw identification value.
 */
void create_readId_descriptor(dma_read_id_device_t *superStruct, uint8_t *ret_raw_id)
{
    superStruct->read_id_command_buf[0] = NANDCMD_READ_ID;
    superStruct->read_id_command_buf[1] = NANDCMD_READ_ID_TYPE;
    uint8_t chipSelect = 0;

    //WAIT 1
    superStruct->wait1.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->sense1);
    superStruct->wait1.cmd.U = NAND_DMA_WAIT4RDY_CMD;
    superStruct->wait1.bar = 0;
    superStruct->wait1.gpmi_ctrl0.U = NAND_DMA_WAIT4RDY_PIO(chipSelect);

    //SENSE 1 - check for successful Ready. BAR points to alternate branch if timeout occurs.
    superStruct->sense1.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->read_id_cmd);
    superStruct->sense1.cmd.U = NAND_DMA_SENSE_CMD(0);
    superStruct->sense1.bar = virtual_to_physical_addr(&superStruct->failure);
    superStruct->sense1.gpmi_ctrl0.U = 0;

    //ID_CMD
    superStruct->read_id_cmd.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->receive_data);
    superStruct->read_id_cmd.cmd.U = NAND_DMA_COMMAND_CMD(NANDCMD_READ_ID_SIZE, 0, NAND_LOCK,3);
    superStruct->read_id_cmd.bar = virtual_to_physical_addr(&superStruct->read_id_command_buf);
    superStruct->read_id_cmd.gpmi_ctrl0.U = NAND_DMA_COMMAND_PIO(chipSelect, NANDCMD_READ_ID_SIZE, 1);
    superStruct->read_id_cmd.gpmi_compare.U = 0;
    superStruct->read_id_cmd.gpmi_eccctrl.U = 0;

    //RECEIVER
    superStruct->receive_data.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->success);
    superStruct->receive_data.cmd.U = NAND_DMA_RX_NO_ECC_CMD(NANDCMD_READ_ID_RESULT_SIZE, 0);
    superStruct->receive_data.bar = virtual_to_physical_addr(ret_raw_id);
    superStruct->receive_data.gpmi_ctrl0.U = NAND_DMA_RX_PIO(chipSelect, NANDCMD_READ_ID_RESULT_SIZE);

    // TERMINATOR
    superStruct->success.nxt = (apbh_dma_t*)0x0;
    superStruct->success.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->success.bar = (void *)NAND_EOK;
    superStruct->failure.nxt = (apbh_dma_t*)0x0;
    superStruct->failure.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->failure.bar = (void*)0x03;
}

/**
 * Creates reset descriptor.
 *
 * @param superStruct Reset descriptor.
 */
void create_reset_descriptor(dma_reset_device_t *superStruct)
{
    uint8_t chipSelect = 0;
    superStruct->reset_command = 0xFF;

    //WAIT 1
    superStruct->wait1.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->sense1);
    superStruct->wait1.cmd.U = NAND_DMA_WAIT4RDY_CMD;
    superStruct->wait1.bar = 0;
    superStruct->wait1.gpmi_ctrl0.U = NAND_DMA_WAIT4RDY_PIO(chipSelect);

    //SENSE 1 - check for successful Ready. BAR points to alternate branch if timeout occurs.
    superStruct->sense1.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->reset_cmd);
    superStruct->sense1.cmd.U = NAND_DMA_SENSE_CMD(0);
    superStruct->sense1.bar = 0;
    superStruct->sense1.gpmi_ctrl0.U = 0;

    //RESET CMD
    superStruct->reset_cmd.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->success);
    superStruct->reset_cmd.cmd.U = NAND_DMA_COMMAND_CMD(2,0,NAND_LOCK,3);
    superStruct->reset_cmd.bar = virtual_to_physical_addr(&superStruct->reset_command);
    superStruct->reset_cmd.gpmi_ctrl0.U = NAND_DMA_COMMAND_PIO(0,2,1);
    superStruct->reset_cmd.gpmi_compare.U = 0;
    superStruct->reset_cmd.gpmi_eccctrl.U = 0;

    //WAIT 2
    superStruct->wait2.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->sense2);
    superStruct->wait2.cmd.U = NAND_DMA_WAIT4RDY_CMD;
    superStruct->wait2.bar = 0;
    superStruct->wait2.gpmi_ctrl0.U = NAND_DMA_WAIT4RDY_PIO(chipSelect);

    //SENSE 2
    superStruct->sense2.nxt = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->success);
    superStruct->sense2.cmd.U = NAND_DMA_SENSE_CMD(0);
    superStruct->sense2.bar = (apbh_dma_gpmi1_t*)virtual_to_physical_addr(&superStruct->failure);
    superStruct->sense2.gpmi_ctrl0.U = 0;

    //TERMINATOR
    superStruct->success.nxt = (apbh_dma_t*)0x0;
    superStruct->success.cmd.U = (IMX_APBH_CHn_CMD_IRQONCMPLT(1) |
                                  IMX_APBH_CHn_CMD_WAIT4ENDCMD(0) |
                                  IMX_APBH_CHn_CMD_SEMAPHORE(1) |
                                  BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER));
    superStruct->success.bar = (void *)NAND_EOK;
    superStruct->failure.nxt = (apbh_dma_t*)0x0;
    superStruct->failure.cmd.U = superStruct->success.cmd.U;
    superStruct->failure.bar = (void*)0x03;
}

/**
 * NAND controller peripherals (GPMI, BCH, APBH-DMA) initialization.
 *
 * @param cio Low level driver handle.
 *
 * @return EOK always.
 */
int nand_init(chipio *cio)
{
    int status = EOK;

    // Initialize DMA
    apbh_init(cio);
    apbh_init_dma_channel(cio);

    // Soft-reset GPMI
    gpmi_soft_reset(cio);

    // Initialize ECC engine.
    bch_init(cio);

    // Put GPMI in NAND mode, disable DEVICE reset, and make certain polarity is active high,
    // sample on GPMI clock. We also enable ganged R/B# and DECOUPLE_CS, which allows us to
    // only have to use a single DMA channel.
    out32( cio->gpmi_pio_reg_base + IMX_GPMI_CTRL1_OFFSET,
           IMX_GPMI_CTRL1_DECOUPLE_CS(1) |
           IMX_GPMI_CTRL1_GANGED_RDYBUSY(1) |
           IMX_GPMI_CTRL1_BCH_MODE(1) |
           IMX_GPMI_CTRL1_BURST_EN(1) |
           IMX_GPMI_CTRL1_DEV_RESET(IMX_GPMI_CTRL1_DEV_RESET_BV_DISABLED) |
           IMX_GPMI_CTRL1_ATA_IRQRDY_POLARITY(IMX_GPMI_CTRL1_ATA_IRQRDY_POLARITY_BV_ACTIVEHIGH) |
           IMX_GPMI_CTRL1_GPMI_MODE(IMX_GPMI_CTRL1_GPMI_MODE_BV_NAND) );

    //TODO - nand - Make clock initialization more robust
    // Assume ~200 MHz input clock (5 ns)
    uint8_t data_setup_cycles = 8;
    uint8_t data_hold_cycles = 6;
    uint8_t addr_setup_cycles = 3;

    out32( cio->gpmi_pio_reg_base + IMX_GPMI_TIMING0_OFFSET,
           IMX_GPMI_TIMING0_ADDRESS_SETUP(addr_setup_cycles) |
           IMX_GPMI_TIMING0_DATA_SETUP(data_setup_cycles) |
           IMX_GPMI_TIMING0_DATA_HOLD(data_hold_cycles) );

    gpmi_set_busy_timeout(cio, 0xFFFF);

    bch_set_layout(cio);
    bch_set_erase_threshold(cio,0);

    return status;
}

/**
 * Starts requested DMA transfer to GPMI.
 *
 * @param dma       DMA transfer configuration.
 * @param chipio    Low level driver handle.
 * @param wait_flag Wait flag value.
 * @param phy_addr  Physical address of the dma chain.
 *
 * @return Execution status (NAND_XXX error code).
 */
int run_dma(apbh_dma_t *dma, chipio *chipio, uint8_t wait_flag, uint32_t phy_addr)
{
    uint8_t chipSelect = 0;
    uint32_t r32ChipDmaNumber = chipSelect;//(NAND0_APBH_CH); //+chipSelect);
    uint32_t r32ChannelMask = 0x1;
    //
    uintptr_t apbh_virt_base = chipio->apbh_dma_reg_base;

    bool bDmaIsRunning;
    int  rtStatus = 0;

    // Alert me if a DMA chain is currently running!!
    // Note that this checks the SEMA.PHORE register field for the DMA.
    bDmaIsRunning = is_dma_active(chipio);

    if (bDmaIsRunning) {
        return NAND_DMA_EIO;
    }
    if (phy_addr == NULL) {
        return NAND_DMA_EIO;
    }

    // At this point, there should be no DMA running.
    // Sanity-test to make sure the DMA channel IRQ is deasserted.
    if (((*(uint32_t*)(apbh_virt_base + IMX_APBH_CTRL1_OFFSET)) & (r32ChannelMask)) != 0) {
        return NAND_DMA_EIO;
    }

    // Initialize DMA by setting up NextCMD field
    out32(apbh_virt_base + IMX_APBH_CHn_NXTCMDAR_OFFSET(r32ChipDmaNumber), phy_addr);

    // Start DMA by incrementing the semaphore.
    (*(volatile imx_apbh_chn_sema_t *) (apbh_virt_base + IMX_APBH_CHn_SEMA_OFFSET(r32ChipDmaNumber))).B.INCREMENT_SEMA = 1;

    //Waits for BCH engine completion - this data transfer is executed by BCH
    if (wait_flag == NAND_DMA_BCH_TRANS) {
        bch_intr_wait(chipio);
    }
    //wait for dma transaction completion
    apbh_intr_wait(chipio);
    //
    rtStatus = in32(apbh_virt_base + IMX_APBH_CHn_BAR_OFFSET(0));
    return rtStatus;
}

/**
 * Checks if previous DMA transaction was OK.
 *
 * @param chipio Low level driver handle.
 *
 * @return Status of the DMA.
 */
bool is_dma_active(chipio *chipio)
{
    uint32_t chipSelect = 0;
    uint32_t u32Sema = 0;
    uint32_t r32ChipDmaNumber = chipSelect;//(NAND0_APBH_CH); //+chipSelect);
    uint32_t u32Run = 0;

    uintptr_t apbh_virt_base = chipio->apbh_dma_reg_base;
    uintptr_t gpmi_virt_base = chipio->gpmi_pio_reg_base;

    u32Sema = (*(volatile imx_apbh_chn_sema_t *) (apbh_virt_base + IMX_APBH_CHn_SEMA_OFFSET(r32ChipDmaNumber))).B.INCREMENT_SEMA;
    u32Run = (*(volatile imx_gpmi_ctrl0_t *) (gpmi_virt_base + IMX_GPMI_CTRL0_OFFSET)).B.RUN;

    // A nonzero value for either of these means a DMA is running.
    return (u32Sema | u32Run);
}

/**
 * Waits to receive NAND ready busy signal.
 *
 * @param cio         Low level driver handle.
 * @param time_out    Requested time out value.
 * @param chip_select Chip-select number.
 *
 * @retval 0  If device is not busy.
 * @retval -1 If error.
 */
int nand_wait_busy (chipio *cio, uint32_t time_out, uint8_t chip_select)
{
    uint32_t stat;

    while (time_out) {
        stat = *(uint32_t*) (cio->gpmi_pio_reg_base + IMX_GPMI_STAT_OFFSET);
        if (!(stat & IMX_GPMI_STAT_READY_BUSY(chip_select))) {
            return (0);
        }
        delay(1);
    }
    return (-1);        /* We will exit from the log and never reach here */
}

/**
 * Transforms page representation from device (Nand memory layout) to processor.
 *
 * @param parsed_data Nfc data representation.
 * @param raw_data    Data from NAND memory.
 */
void device_to_nfc(uint8_t *parsed_data, uint8_t *raw_data)
{
    uint32_t dData_index, data_size, spare_size;
    int i;
    uint8_t bbi_backup = 0;

    dData_index = 0;
    data_size = 0;
    spare_size = 0;
    for (i = 0; i < (DATASIZE / BCH_SUBBLOCK_SIZE); i++) {
        /* Copy data bytes */
        memcpy((void *)(parsed_data + data_size), (void *)(raw_data + dData_index), BCH_SUBBLOCK_SIZE);
        dData_index += BCH_SUBBLOCK_SIZE;
        data_size += BCH_SUBBLOCK_SIZE;
        /* Copy fs meta data */
        memcpy((void *)(parsed_data + 4096 + spare_size), (void *)(raw_data + dData_index), ETFS_META_SIZE_PER_SUBBLOCK);
        dData_index += (ETFS_META_SIZE_PER_SUBBLOCK);
        spare_size += (ETFS_META_SIZE_PER_SUBBLOCK);
    }
    // BBI swap
    bbi_backup = parsed_data[DATASIZE]; //(BBI backup for move to DATA area)
    parsed_data[DATASIZE] = parsed_data[4096 - (3 * ETFS_META_SIZE_PER_SUBBLOCK)];
    parsed_data[4096 - (3 * ETFS_META_SIZE_PER_SUBBLOCK)] = bbi_backup;
}

/**
 * Transforms page representation in processor to device (NAND memory layout).
 *
 * @param device_data Data to be written to NAND memory.
 * @param nfc_data    Data to parse.
 * @param ecc_size    Size of ECC in bytes t*GF/8, intended only for RAW writes to device. For BCH writes use 0 always.
 */
void nfc_to_device(uint8_t *device_data, uint8_t *nfc_data, uint8_t ecc_size)
{
    uint32_t dData_index, data_size, spare_size;
    int i;
    uint8_t bbi_backup;

    dData_index = 0;
    data_size = 0;
    spare_size = 0;

    for (i = 0; i < (DATASIZE / BCH_SUBBLOCK_SIZE); i++) {
        /* Copy data bytes */
        memcpy((void *)(device_data + dData_index), (void *)(nfc_data + data_size), BCH_SUBBLOCK_SIZE);
        dData_index += (BCH_SUBBLOCK_SIZE);
        data_size += BCH_SUBBLOCK_SIZE;
        /* Copy fs spare data */
        memcpy((void *)(device_data + dData_index), (void *)(nfc_data + 4096 + spare_size), ETFS_META_SIZE_PER_SUBBLOCK);
        dData_index += (ETFS_META_SIZE_PER_SUBBLOCK);
        spare_size += (ETFS_META_SIZE_PER_SUBBLOCK);
        if (ecc_size > 0) {
            /* Copy ecc bytes (0xFF) when using raw writes */
            memcpy((void *)(device_data + dData_index), (void *)(nfc_data), BCH_ECC_SIZE);
            dData_index += (BCH_ECC_SIZE);
        }
    }
    // BBI swap
    bbi_backup = device_data[BCH_SUBBLOCK_SIZE]; //(BBI backup for move to DATA area)
    device_data[BCH_SUBBLOCK_SIZE] = device_data[DATASIZE];
    device_data[DATASIZE] = bbi_backup;
}

/** @}*/
