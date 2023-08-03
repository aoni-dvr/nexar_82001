/**
 *  @file AmbaReg_IoCtrl.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Definitions & Constants for B6 I2C I/O Control Registers
 *
 */

#ifndef AMBA_B8_REG_IO_CTRL_H
#define AMBA_B8_REG_IO_CTRL_H

/*-----------------------------------------------------------------------------------------------*\
 * B6 I/O Control :
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  I2cBridgeEnable:            1;      /* [0]: I2C mode: 0 = master, 1 = bridge */
    UINT32  I2cChanID:                  2;      /* [2:1]: Channel ID for I2C master */
    UINT32  I2cBridgeOnHighSpeed:       1;      /* [3]: 1 = Enable bridge during high-speed mode */
    UINT32  Reserved0:                  4;      /* [7:4]: Reserved */
    UINT32  CoProgram:                  1;      /* [8]: 1 = Enable register write on both near side and far side */
    UINT32  Reserved1:                  2;      /* [10:9]: Reserved */
    UINT32  MphyTxFpgaMode:             1;      /* [11]: MPHY TX mode: 1 = FPGA, 0 = Internal IP */
    UINT32  MphyRxFpgaMode:             1;      /* [12]: MPHY RX mode: 1 = FPGA, 0 = Internal IP */
    UINT32  Reserved2:                  2;      /* [14:13]: Reserved */
    UINT32  I2cBridgeSelect:            4;      /* [18:15]: One-hot encoded bridge select */
    UINT32  I2cBridgeReadMode:          1;      /* [19]: 0 = OR data from all the channels, 1 = AND data from all the channels */
    UINT32  Reserved3:                  12;     /* [31:20]: Reserved */
} B8_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 I2C I/O : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_CTRL_REG_s Ctrl; /* 0x000(RW): */
} B8_IO_CTRL_REG_s;

#endif /* AMBA_B8_REG_IO_CTRL_H */
