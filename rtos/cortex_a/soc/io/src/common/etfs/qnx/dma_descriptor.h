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

#ifndef DMA_DESCRIPTOR_H_
#define DMA_DESCRIPTOR_H_

#include <arm/imx/imx_gpmi.h>
#include <arm/imx/imx_apbh.h>

/**
 * @file       imx-micron/dma_descriptor.h
 * @addtogroup etfs_chipio Chip interface
 * @{
 */

#define NAND_LOCK       1 //! Lock the NAND to prevent contention.
#define ASSERT_CS       1 //! Assert the Chip Select during this operation.

#define BV_FLD(reg, field, sym)  IMX_##reg##_##field(IMX_##reg##_##field##_BV_##sym)

#define BF_CS1(reg, f1, v1)  \
        (HW_##reg##_CLR(BM_##reg##_##f1),      \
         HW_##reg##_SET(BF_##reg##_##f1(v1)))

/** Macro/Defines used to create a DMA command word in the chain. */

/** @brief APBH DMA Macro for Wait4Ready command.
 *
 * Transfer one Word to PIO.
 * Wait for DMA to complete before starting next DMA descriptor in chain.
 * Wait for Ready before starting next DMA descriptor in chain.
 * Don't lock the nand while waiting for Ready to go high.
 * Another descriptor follows this one in the chain.
 * This DMA has no transfer.
 */
#define NAND_DMA_WAIT4RDY_CMD \
    (IMX_APBH_CHn_CMD_CMDWORDS(1) | \
     IMX_APBH_CHn_CMD_HALTONTERMINATE(1) | \
     IMX_APBH_CHn_CMD_WAIT4ENDCMD(1) | \
     IMX_APBH_CHn_CMD_NANDWAIT4READY(1) | \
     IMX_APBH_CHn_CMD_NANDLOCK(0) | \
     IMX_APBH_CHn_CMD_CHAIN(1) | \
     BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER))


/** @brief GPMI PIO DMA Macro for Wait4Ready command.
 *
 * Wait for Ready before sending IRQ interrupt.
 * Use 8 bit word length (doesn't really matter since no transfer).
 * Watch u32ChipSelect.
 */
#define NAND_DMA_WAIT4RDY_PIO(u32ChipSelect) \
    (BV_FLD(GPMI_CTRL0, COMMAND_MODE, WAIT_FOR_READY) | \
     IMX_GPMI_CTRL0_WORD_LENGTH(IMX_GPMI_CTRL0_WORD_LENGTH_BV_8_BIT) | \
     BV_FLD(GPMI_CTRL0, ADDRESS, NAND_DATA) | \
     IMX_GPMI_CTRL0_CS(u32ChipSelect))

/**
 * @brief APBH DMA Macro for Transmit Data command.
 * Transfer TransferSize bytes with DMA.
 * Transfer one Word to PIO.
 * Wait for DMA to complete before starting next DMA descriptor in chain.
 * Lock the NAND while waiting for this DMA chain to complete.
 * Decrement semaphore if this is the last part of the chain.
 * Another descriptor follows this one in the chain.
 * This DMA is a read from System Memory - write to device.
 */
#define NAND_DMA_TXDATA_CMD(TransferSize,Semaphore,CommandWords,Wait4End, Cmd) \
    (IMX_APBH_CHn_CMD_XFER_COUNT(TransferSize) | \
     IMX_APBH_CHn_CMD_CMDWORDS(CommandWords) | \
     IMX_APBH_CHn_CMD_HALTONTERMINATE(1) | \
     IMX_APBH_CHn_CMD_WAIT4ENDCMD(Wait4End) | \
     IMX_APBH_CHn_CMD_NANDLOCK(1) | \
     IMX_APBH_CHn_CMD_SEMAPHORE(Semaphore) | \
     IMX_APBH_CHn_CMD_CHAIN(1) | \
     BV_FLD(APBH_CHn_CMD, COMMAND, Cmd))

/** @brief GPMI PIO DMA Macro for Transmit Data command.
 * Setup transfer as a write.
 * Transfer NumBitsInWord bits per DMA cycle.
 * Lock CS during this transaction.
 * Select the appropriate chip.
 * Address lines need to specify Data transfer (0b00)
 * Transfer TransferSize - NumBitsInWord values.
 */
#define NAND_DMA_TXDATA_PIO(u32ChipSelect, TransferSize) \
    (BV_FLD(GPMI_CTRL0, COMMAND_MODE, WRITE) | \
     BV_FLD(GPMI_CTRL0, WORD_LENGTH, 8_BIT) | \
     IMX_GPMI_CTRL0_LOCK_CS(1) | \
     IMX_GPMI_CTRL0_CS(u32ChipSelect) | \
     BV_FLD(GPMI_CTRL0, ADDRESS, NAND_DATA) | \
     IMX_GPMI_CTRL0_XFER_COUNT(TransferSize))

/**
 * @brief APBH DMA Macro for Sense command.
 * Transfer no Bytes with DMA.
 * Transfer no Words to PIO.
 * Don't lock the NAND while waiting for Ready to go high.
 * Decrement semaphore if this is the last part of the chain.
 * Another descriptor follows this one in the chain.
 */
#define NAND_DMA_SENSE_CMD(SenseSemaphore) \
    (IMX_APBH_CHn_CMD_CMDWORDS(0) | \
     IMX_APBH_CHn_CMD_HALTONTERMINATE(1) | \
     IMX_APBH_CHn_CMD_SEMAPHORE(SenseSemaphore) | \
     IMX_APBH_CHn_CMD_NANDLOCK(0) | \
     IMX_APBH_CHn_CMD_CHAIN(1) | \
     BV_FLD(APBH_CHn_CMD, COMMAND, DMA_SENSE))

/**
 * @brief APBH DMA Macro for Read Data command with ECC.
 * Receive TransferSize bytes with DMA.
 * Transfer one Word to PIO.
 * Wait for DMA to complete before starting next DMA descriptor in chain.
 * Decrement semaphore if this is the last part of the chain.
 * Unlock the NAND after this DMA chain completes.
 * Another descriptor follows this one in the chain.
 * No DMA transfer here; the ECC8 block becomes the bus master and
 * performs the memory writes itself instead of the DMA.
 */
#define NAND_DMA_RX_CMD_ECC(TransferSize,Semaphore) \
    (IMX_APBH_CHn_CMD_XFER_COUNT(TransferSize) | \
     IMX_APBH_CHn_CMD_CMDWORDS(6) | \
     IMX_APBH_CHn_CMD_HALTONTERMINATE(1) | \
     IMX_APBH_CHn_CMD_WAIT4ENDCMD(1) | \
     IMX_APBH_CHn_CMD_SEMAPHORE(Semaphore) | \
     IMX_APBH_CHn_CMD_NANDLOCK(1) | \
     IMX_APBH_CHn_CMD_CHAIN(1) | \
     BV_FLD(APBH_CHn_CMD, COMMAND, NO_DMA_XFER))

/**
 * @brief APBH DMA Macro for Receive Data with no ECC command.
 * Receive TransferSize bytes with DMA but no ECC.
 * Transfer one Word to PIO.
 * Wait for DMA to complete before starting next DMA descriptor in chain.
 * Decrement semaphore if this is the last part of the chain.
 * Unlock the NAND after this DMA chain completes.
 * Another descriptor follows this one in the chain.
 * This DMA is a write to System Memory - read from device.
 */
#define NAND_DMA_RX_NO_ECC_CMD(TransferSize,Semaphore) \
    (IMX_APBH_CHn_CMD_XFER_COUNT(TransferSize) | \
     IMX_APBH_CHn_CMD_CMDWORDS(1) | \
     IMX_APBH_CHn_CMD_HALTONTERMINATE(1) | \
     IMX_APBH_CHn_CMD_WAIT4ENDCMD(1) | \
     IMX_APBH_CHn_CMD_SEMAPHORE(Semaphore) | \
     IMX_APBH_CHn_CMD_NANDLOCK(0) | \
     IMX_APBH_CHn_CMD_CHAIN(1) | \
     BV_FLD(APBH_CHn_CMD, COMMAND, DMA_WRITE))

/**
 * @brief GPMI PIO DMA Macro for Receive command.
 * Setup transfer as a READ.
 * Transfer NumBitsInWord bits per DMA cycle.
 * Select the appropriate chip.
 * Address lines need to specify Data transfer (0b00)
 * Transfer TransferSize - NumBitsInWord values.
 */
#define NAND_DMA_RX_PIO(u32ChipSelect, TransferSize) \
    (BV_FLD(GPMI_CTRL0, COMMAND_MODE, READ) | \
     BV_FLD(GPMI_CTRL0, WORD_LENGTH, 8_BIT) | \
     IMX_GPMI_CTRL0_CS(u32ChipSelect) | \
     IMX_GPMI_CTRL0_LOCK_CS(0) | \
     BV_FLD(GPMI_CTRL0, ADDRESS, NAND_DATA) | \
     IMX_GPMI_CTRL0_ADDRESS_INCREMENT(0) | \
     IMX_GPMI_CTRL0_XFER_COUNT(TransferSize))

/**
 * @brief APBH DMA Macro for sending NAND Command sequence.
 * Transmit TransferSize bytes to DMA.
 * Transfer one Word to PIO.
 * Wait for DMA to complete before starting next DMA descriptor in chain.
 * Decrement semaphore if this is the last part of the chain.
 * Lock the NAND until the next chain.
 * Another descriptor follows this one in the chain.
 * This DMA is a read from System Memory - write to device.
 */
#define NAND_DMA_COMMAND_CMD(TransferSize,Semaphore,NandLock,CmdWords) \
    (IMX_APBH_CHn_CMD_XFER_COUNT(TransferSize) | \
     IMX_APBH_CHn_CMD_CMDWORDS(CmdWords) | \
     IMX_APBH_CHn_CMD_HALTONTERMINATE(1) | \
     IMX_APBH_CHn_CMD_WAIT4ENDCMD(1) | \
     IMX_APBH_CHn_CMD_SEMAPHORE(Semaphore) | \
     IMX_APBH_CHn_CMD_NANDLOCK(NandLock) | \
     IMX_APBH_CHn_CMD_CHAIN(1) | \
     BV_FLD(APBH_CHn_CMD, COMMAND, DMA_READ))

/**
 * @brief GPMI PIO DMA Macro when sending a command.
 * Setup transfer as a WRITE.
 * Transfer NumBitsInWord bits per DMA cycle.
 * Lock CS during and after this transaction.
 * Select the appropriate chip.
 * Address lines need to specify Command transfer (0b01)
 * Increment the Address lines if AddrIncr is set.
 * Transfer TransferSize - NumBitsInWord values.
 */
#define NAND_DMA_COMMAND_PIO(u32ChipSelect,TransferSize,AssertCS) \
    (BV_FLD(GPMI_CTRL0, COMMAND_MODE, WRITE) | \
     IMX_GPMI_CTRL0_WORD_LENGTH(IMX_GPMI_CTRL0_WORD_LENGTH_BV_8_BIT) | \
     IMX_GPMI_CTRL0_LOCK_CS(AssertCS) | \
     IMX_GPMI_CTRL0_CS(u32ChipSelect) | \
     BV_FLD(GPMI_CTRL0, ADDRESS, NAND_CLE) | \
     IMX_GPMI_CTRL0_ADDRESS_INCREMENT((TransferSize > 0) ? IMX_GPMI_CTRL0_ADDRESS_INCREMENT_BV_ENABLED : 0) | \
     IMX_GPMI_CTRL0_XFER_COUNT(TransferSize))

/**
 * @brief GPMI PIO DMA Macro for disabling ECC during this write.
 */
#define NAND_DMA_ECC_PIO(EnableDisable) (IMX_GPMI_ECCCTRL_ENABLE_ECC(EnableDisable))

/**
 * @brief GPMI PIO DMA Macro sequence for ECC decode.
 * Setup READ transfer ECC Control register.
 * Setup for ECC Decode, 4 Bit.
 * Enable the ECC block
 * The ECC Buffer Mask determines which fields are corrected.
 */
#define NAND_DMA_ECC_CTRL_PIO(EccBufferMask, decode_encode_size) \
    (IMX_GPMI_ECCCTRL_ECC_CMD(decode_encode_size) | \
     IMX_GPMI_ECCCTRL_ENABLE_ECC(IMX_GPMI_ECCCTRL_ENABLE_ECC_BV_ENABLE) | \
     IMX_GPMI_ECCCTRL_BUFFER_MASK(EccBufferMask) )

/** @}*/

#endif /* DMA_DESCRIPTOR_H_ */
