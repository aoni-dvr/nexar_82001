/**
 *  @file AmbaReg_IoMux.h
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
 *  @details Definitions & Constants for B6 Pin Mux Select Control Registers
 *
 */

#ifndef AMBA_B8_REG_IOMUX_H
#define AMBA_B8_REG_IOMUX_H

/*-----------------------------------------------------------------------------------*\
 * B6 IoMux : Pin Group [31:0] Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Pin0:                   1;      /* [0] PIN 0 */
    UINT32  Pin1:                   1;      /* [1] PIN 1 */
    UINT32  Pin2:                   1;      /* [2] PIN 2 */
    UINT32  Pin3:                   1;      /* [3] PIN 3 */
    UINT32  Pin4:                   1;      /* [4] PIN 4 */
    UINT32  Pin5:                   1;      /* [5] PIN 5 */
    UINT32  Pin6:                   1;      /* [6] PIN 6 */
    UINT32  Pin7:                   1;      /* [7] PIN 7 */
    UINT32  Pin8:                   1;      /* [8] PIN 8 */
    UINT32  Pin9:                   1;      /* [9] PIN 9 */
    UINT32  Pin10:                  1;      /* [10] PIN 10 */
    UINT32  Pin11:                  1;      /* [11] PIN 11 */
    UINT32  Pin12:                  1;      /* [12] PIN 12 */
    UINT32  Pin13:                  1;      /* [13] PIN 13 */
    UINT32  Pin14:                  1;      /* [14] PIN 14 */
    UINT32  Pin15:                  1;      /* [15] PIN 15 */
    UINT32  Pin16:                  1;      /* [16] PIN 16 */
    UINT32  Pin17:                  1;      /* [17] PIN 17 */
    UINT32  Pin18:                  1;      /* [18] PIN 18 */
    UINT32  Pin19:                  1;      /* [19] PIN 19 */
    UINT32  Pin20:                  1;      /* [20] PIN 20 */
    UINT32  Pin21:                  1;      /* [21] PIN 21 */
    UINT32  Pin22:                  1;      /* [22] PIN 22 */
    UINT32  Pin23:                  1;      /* [23] PIN 23 */
    UINT32  Pin24:                  1;      /* [24] PIN 24 */
    UINT32  Pin25:                  1;      /* [25] PIN 25 */
    UINT32  Pin26:                  1;      /* [26] PIN 26 */
    UINT32  Pin27:                  1;      /* [27] PIN 27 */
    UINT32  Pin28:                  1;      /* [28] PIN 28 */
    UINT32  Pin29:                  1;      /* [29] PIN 29 */
    UINT32  Pin30:                  1;      /* [30] PIN 30 */
    UINT32  Pin31:                  1;      /* [31] PIN 31 */
} B8_IOMUX_PIN_GROUP_0_31_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Iomux : Enable Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Enable:                 1;      /* [0] Set enable */
    UINT32  Reserved0:              31;     /* [31:1] */
} B8_IOMUX_ENABLE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 Iomux : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_IOMUX_PIN_GROUP_0_31_REG_s    PinGroup0FuncSelect0;   /* 0x00(RW): Pin Function Select 0 Register for PIN[31:0] */
    volatile B8_IOMUX_PIN_GROUP_0_31_REG_s    PinGroup0FuncSelect1;   /* 0x04(RW): Pin Function Select 1 Register for PIN[31:0] */
    volatile B8_IOMUX_PIN_GROUP_0_31_REG_s    PinGroup0FuncSelect2;   /* 0x08(RW): Pin Function Select 2 Register for PIN[31:0] */
    volatile UINT32                           Reserved0[57];          /* 0x0C-0xEC */
    volatile B8_IOMUX_ENABLE_REG_s            Enable;                 /* 0xF0(RW): Enable Register */
} B8_IOMUX_REG_s;

#endif /* AMBA_B8_REG_IOMUX_H */
