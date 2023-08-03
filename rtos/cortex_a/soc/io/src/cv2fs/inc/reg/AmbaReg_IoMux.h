/**
 *  @file AmbaReg_IoMux.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Definitions & Constants for IO Multiplexing (IOMUX) Control Registers
 *
 */

#ifndef AMBA_REG_IO_MUX_H
#define AMBA_REG_IO_MUX_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * IOMUX: Pin Function Select Register
 */
typedef struct {
    UINT32  Pin0:               1;      /* [0] Function select [0/1/2] for pin[0|32|64|96|128|160|192] */
    UINT32  Pin1:               1;      /* [1] Function select [0/1/2] for pin[1|33|65|97|129|161|193] */
    UINT32  Pin2:               1;      /* [2] Function select [0/1/2] for pin[2|34|66|98|130|162|194] */
    UINT32  Pin3:               1;      /* [3] Function select [0/1/2] for pin[3|35|67|99|131|163|195] */
    UINT32  Pin4:               1;      /* [4] Function select [0/1/2] for pin[4|36|68|100|132|164|196] */
    UINT32  Pin5:               1;      /* [5] Function select [0/1/2] for pin[5|37|69|101|133|165|197] */
    UINT32  Pin6:               1;      /* [6] Function select [0/1/2] for pin[6|38|70|102|134|166|198] */
    UINT32  Pin7:               1;      /* [7] Function select [0/1/2] for pin[7|39|71|103|135|167|199] */
    UINT32  Pin8:               1;      /* [8] Function select [0/1/2] for pin[8|40|72|104|136|168|200] */
    UINT32  Pin9:               1;      /* [9] Function select [0/1/2] for pin[9|41|73|105|137|169|201] */
    UINT32  Pin10:              1;      /* [10] Function select [0/1/2] for pin[10|42|74|106|138|170|202] */
    UINT32  Pin11:              1;      /* [11] Function select [0/1/2] for pin[11|43|75|107|139|171|203] */
    UINT32  Pin12:              1;      /* [12] Function select [0/1/2] for pin[12|44|76|108|140|172|204] */
    UINT32  Pin13:              1;      /* [13] Function select [0/1/2] for pin[13|45|77|109|141|173|205] */
    UINT32  Pin14:              1;      /* [14] Function select [0/1/2] for pin[14|46|78|110|142|174|206] */
    UINT32  Pin15:              1;      /* [15] Function select [0/1/2] for pin[15|47|79|111|143|175|207] */
    UINT32  Pin16:              1;      /* [16] Function select [0/1/2] for pin[16|48|80|112|144|176|208] */
    UINT32  Pin17:              1;      /* [17] Function select [0/1/2] for pin[17|49|81|113|145|177|209] */
    UINT32  Pin18:              1;      /* [18] Function select [0/1/2] for pin[18|50|82|114|146|178|210] */
    UINT32  Pin19:              1;      /* [19] Function select [0/1/2] for pin[19|51|83|115|147|179|211] */
    UINT32  Pin20:              1;      /* [20] Function select [0/1/2] for pin[20|52|84|116|148|180|212] */
    UINT32  Pin21:              1;      /* [21] Function select [0/1/2] for pin[21|53|85|117|149|181|213] */
    UINT32  Pin22:              1;      /* [22] Function select [0/1/2] for pin[22|54|86|118|150|182|214] */
    UINT32  Pin23:              1;      /* [23] Function select [0/1/2] for pin[23|55|87|119|151|183|215] */
    UINT32  Pin24:              1;      /* [24] Function select [0/1/2] for pin[24|56|88|120|152|184|216] */
    UINT32  Pin25:              1;      /* [25] Function select [0/1/2] for pin[25|57|89|121|153|185|217] */
    UINT32  Pin26:              1;      /* [26] Function select [0/1/2] for pin[26|58|90|122|154|186|218] */
    UINT32  Pin27:              1;      /* [27] Function select [0/1/2] for pin[27|59|91|123|155|187|219] */
    UINT32  Pin28:              1;      /* [28] Function select [0/1/2] for pin[28|60|92|124|156|188|220] */
    UINT32  Pin29:              1;      /* [29] Function select [0/1/2] for pin[29|61|93|125|157|189|221] */
    UINT32  Pin30:              1;      /* [30] Function select [0/1/2] for pin[30|62|94|126|158|190|222] */
    UINT32  Pin31:              1;      /* [31] Function select [0/1/2] for pin[31|63|95|127|159|191|223] */
} AMBA_IO_MUX_PIN_FUNC_SEL_REG_s;

/*
 * IOMUX: Register Taken Effect Control Register
 */
typedef struct {
    UINT32  Update:             1;      /* [0] 1 = Make pin function selection registers take effect */
    UINT32  Reserved:           31;     /* [31:1] Reserved */
} AMBA_IO_MUX_REG_UPDATE_REG_s;

/*
 * IOMUX: Pin Function Selection Register
 */
typedef struct {
    volatile UINT32     FuncSelect0;    /* bit[0] of function selection */
    volatile UINT32     FuncSelect1;    /* bit[1] of function selection */
    volatile UINT32     FuncSelect2;    /* bit[2] of function selection */
} AMBA_IO_MUX_PIN_FUNC_REG_s;

/*
 * IOMUX: All Registers
 */
typedef struct {
    AMBA_IO_MUX_PIN_FUNC_REG_s              PinGroup[5];    /* 0x000-0x038(RW): IOMUX Set 0-4 (GPIO 0-31,32-63,64-95,96-127,128-159) Pin Function Selection */
    volatile UINT32                         Reserved[45];   /* 0x03c-0x0EC(RW): Reserved */
    volatile AMBA_IO_MUX_REG_UPDATE_REG_s   RegUpdate;      /* 0x0F0(WO): Register Taken Effect Control */
} AMBA_IO_MUX_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
#ifdef CONFIG_QNX
extern AMBA_IO_MUX_REG_s * pAmbaIOMUX_Reg;
#else
extern AMBA_IO_MUX_REG_s *const pAmbaIOMUX_Reg;
#endif

#endif /* AMBA_REG_IO_MUX_H */
