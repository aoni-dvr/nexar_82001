/*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AMBA_REG_GPIO_H
#define AMBA_REG_GPIO_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * GPIO: Logic State Register
 */
typedef struct {
    UINT32  Data0:              1;      /* [0] 0 = LOW voltage, 1 = HIGH voltage of pin[0|32|64|96|128|160|192] */
    UINT32  Data1:              1;      /* [1] 0 = LOW voltage, 1 = HIGH voltage of pin[1|33|65|97|129|161|193] */
    UINT32  Data2:              1;      /* [2] 0 = LOW voltage, 1 = HIGH voltage of pin[2|34|66|98|130|162|194] */
    UINT32  Data3:              1;      /* [3] 0 = LOW voltage, 1 = HIGH voltage of pin[3|35|67|99|131|163|195] */
    UINT32  Data4:              1;      /* [4] 0 = LOW voltage, 1 = HIGH voltage of pin[4|36|68|100|132|164|196] */
    UINT32  Data5:              1;      /* [5] 0 = LOW voltage, 1 = HIGH voltage of pin[5|37|69|101|133|165|197] */
    UINT32  Data6:              1;      /* [6] 0 = LOW voltage, 1 = HIGH voltage of pin[6|38|70|102|134|166|198] */
    UINT32  Data7:              1;      /* [7] 0 = LOW voltage, 1 = HIGH voltage of pin[7|39|71|103|135|167|199] */
    UINT32  Data8:              1;      /* [8] 0 = LOW voltage, 1 = HIGH voltage of pin[8|40|72|104|136|168|200] */
    UINT32  Data9:              1;      /* [9] 0 = LOW voltage, 1 = HIGH voltage of pin[9|41|73|105|137|169|201] */
    UINT32  Data10:             1;      /* [10] 0 = LOW voltage, 1 = HIGH voltage of pin[10|42|74|106|138|170|202] */
    UINT32  Data11:             1;      /* [11] 0 = LOW voltage, 1 = HIGH voltage of pin[11|43|75|107|139|171|203] */
    UINT32  Data12:             1;      /* [12] 0 = LOW voltage, 1 = HIGH voltage of pin[12|44|76|108|140|172|204] */
    UINT32  Data13:             1;      /* [13] 0 = LOW voltage, 1 = HIGH voltage of pin[13|45|77|109|141|173|205] */
    UINT32  Data14:             1;      /* [14] 0 = LOW voltage, 1 = HIGH voltage of pin[14|46|78|110|142|174|206] */
    UINT32  Data15:             1;      /* [15] 0 = LOW voltage, 1 = HIGH voltage of pin[15|47|79|111|143|175|207] */
    UINT32  Data16:             1;      /* [16] 0 = LOW voltage, 1 = HIGH voltage of pin[16|48|80|112|144|176|208] */
    UINT32  Data17:             1;      /* [17] 0 = LOW voltage, 1 = HIGH voltage of pin[17|49|81|113|145|177|209] */
    UINT32  Data18:             1;      /* [18] 0 = LOW voltage, 1 = HIGH voltage of pin[18|50|82|114|146|178|210] */
    UINT32  Data19:             1;      /* [19] 0 = LOW voltage, 1 = HIGH voltage of pin[19|51|83|115|147|179|211] */
    UINT32  Data20:             1;      /* [20] 0 = LOW voltage, 1 = HIGH voltage of pin[20|52|84|116|148|180|212] */
    UINT32  Data21:             1;      /* [21] 0 = LOW voltage, 1 = HIGH voltage of pin[21|53|85|117|149|181|213] */
    UINT32  Data22:             1;      /* [22] 0 = LOW voltage, 1 = HIGH voltage of pin[22|54|86|118|150|182|214] */
    UINT32  Data23:             1;      /* [23] 0 = LOW voltage, 1 = HIGH voltage of pin[23|55|87|119|151|183|215] */
    UINT32  Data24:             1;      /* [24] 0 = LOW voltage, 1 = HIGH voltage of pin[24|56|88|120|152|184|216] */
    UINT32  Data25:             1;      /* [25] 0 = LOW voltage, 1 = HIGH voltage of pin[25|57|89|121|153|185|217] */
    UINT32  Data26:             1;      /* [26] 0 = LOW voltage, 1 = HIGH voltage of pin[26|58|90|122|154|186|218] */
    UINT32  Data27:             1;      /* [27] 0 = LOW voltage, 1 = HIGH voltage of pin[27|59|91|123|155|187|219] */
    UINT32  Data28:             1;      /* [28] 0 = LOW voltage, 1 = HIGH voltage of pin[28|60|92|124|156|188|220] */
    UINT32  Data29:             1;      /* [29] 0 = LOW voltage, 1 = HIGH voltage of pin[29|61|93|125|157|189|221] */
    UINT32  Data30:             1;      /* [30] 0 = LOW voltage, 1 = HIGH voltage of pin[30|62|94|126|158|190|222] */
    UINT32  Data31:             1;      /* [31] 0 = LOW voltage, 1 = HIGH voltage of pin[31|63|95|127|159|191|223] */
} AMBA_GPIO_LOGIC_STATE_REG_s;

/*
 * GPIO: I/O Type Register
 */
typedef struct {
    UINT32  IoType0:            1;      /* [0] 0 = Input, 1 = Output GPIO direction of pin[0|32|64|96|128|160|192] */
    UINT32  IoType1:            1;      /* [1] 0 = Input, 1 = Output GPIO direction of pin[1|33|65|97|129|161|193] */
    UINT32  IoType2:            1;      /* [2] 0 = Input, 1 = Output GPIO direction of pin[2|34|66|98|130|162|194] */
    UINT32  IoType3:            1;      /* [3] 0 = Input, 1 = Output GPIO direction of pin[3|35|67|99|131|163|195] */
    UINT32  IoType4:            1;      /* [4] 0 = Input, 1 = Output GPIO direction of pin[4|36|68|100|132|164|196] */
    UINT32  IoType5:            1;      /* [5] 0 = Input, 1 = Output GPIO direction of pin[5|37|69|101|133|165|197] */
    UINT32  IoType6:            1;      /* [6] 0 = Input, 1 = Output GPIO direction of pin[6|38|70|102|134|166|198] */
    UINT32  IoType7:            1;      /* [7] 0 = Input, 1 = Output GPIO direction of pin[7|39|71|103|135|167|199] */
    UINT32  IoType8:            1;      /* [8] 0 = Input, 1 = Output GPIO direction of pin[8|40|72|104|136|168|200] */
    UINT32  IoType9:            1;      /* [9] 0 = Input, 1 = Output GPIO direction of pin[9|41|73|105|137|169|201] */
    UINT32  IoType10:           1;      /* [10] 0 = Input, 1 = Output GPIO direction of pin[10|42|74|106|138|170|202] */
    UINT32  IoType11:           1;      /* [11] 0 = Input, 1 = Output GPIO direction of pin[11|43|75|107|139|171|203] */
    UINT32  IoType12:           1;      /* [12] 0 = Input, 1 = Output GPIO direction of pin[12|44|76|108|140|172|204] */
    UINT32  IoType13:           1;      /* [13] 0 = Input, 1 = Output GPIO direction of pin[13|45|77|109|141|173|205] */
    UINT32  IoType14:           1;      /* [14] 0 = Input, 1 = Output GPIO direction of pin[14|46|78|110|142|174|206] */
    UINT32  IoType15:           1;      /* [15] 0 = Input, 1 = Output GPIO direction of pin[15|47|79|111|143|175|207] */
    UINT32  IoType16:           1;      /* [16] 0 = Input, 1 = Output GPIO direction of pin[16|48|80|112|144|176|208] */
    UINT32  IoType17:           1;      /* [17] 0 = Input, 1 = Output GPIO direction of pin[17|49|81|113|145|177|209] */
    UINT32  IoType18:           1;      /* [18] 0 = Input, 1 = Output GPIO direction of pin[18|50|82|114|146|178|210] */
    UINT32  IoType19:           1;      /* [19] 0 = Input, 1 = Output GPIO direction of pin[19|51|83|115|147|179|211] */
    UINT32  IoType20:           1;      /* [20] 0 = Input, 1 = Output GPIO direction of pin[20|52|84|116|148|180|212] */
    UINT32  IoType21:           1;      /* [21] 0 = Input, 1 = Output GPIO direction of pin[21|53|85|117|149|181|213] */
    UINT32  IoType22:           1;      /* [22] 0 = Input, 1 = Output GPIO direction of pin[22|54|86|118|150|182|214] */
    UINT32  IoType23:           1;      /* [23] 0 = Input, 1 = Output GPIO direction of pin[23|55|87|119|151|183|215] */
    UINT32  IoType24:           1;      /* [24] 0 = Input, 1 = Output GPIO direction of pin[24|56|88|120|152|184|216] */
    UINT32  IoType25:           1;      /* [25] 0 = Input, 1 = Output GPIO direction of pin[25|57|89|121|153|185|217] */
    UINT32  IoType26:           1;      /* [26] 0 = Input, 1 = Output GPIO direction of pin[26|58|90|122|154|186|218] */
    UINT32  IoType27:           1;      /* [27] 0 = Input, 1 = Output GPIO direction of pin[27|59|91|123|155|187|219] */
    UINT32  IoType28:           1;      /* [28] 0 = Input, 1 = Output GPIO direction of pin[28|60|92|124|156|188|220] */
    UINT32  IoType29:           1;      /* [29] 0 = Input, 1 = Output GPIO direction of pin[29|61|93|125|157|189|221] */
    UINT32  IoType30:           1;      /* [30] 0 = Input, 1 = Output GPIO direction of pin[30|62|94|126|158|190|222] */
    UINT32  IoType31:           1;      /* [31] 0 = Input, 1 = Output GPIO direction of pin[31|63|95|127|159|191|223] */
} AMBA_GPIO_IO_TYPE_REG_s;

/*
 * GPIO: Edge/Level Interrupt Sensitivity Control Register
 */
typedef struct {
    UINT32  IntTrigType0:       1;      /* [0] 0 = Edge, 1 = Level triggered interrupt on pin[0|32|64|96|128|160|192] */
    UINT32  IntTrigType1:       1;      /* [1] 0 = Edge, 1 = Level triggered interrupt on pin[1|33|65|97|129|161|193] */
    UINT32  IntTrigType2:       1;      /* [2] 0 = Edge, 1 = Level triggered interrupt on pin[2|34|66|98|130|162|194] */
    UINT32  IntTrigType3:       1;      /* [3] 0 = Edge, 1 = Level triggered interrupt on pin[3|35|67|99|131|163|195] */
    UINT32  IntTrigType4:       1;      /* [4] 0 = Edge, 1 = Level triggered interrupt on pin[4|36|68|100|132|164|196] */
    UINT32  IntTrigType5:       1;      /* [5] 0 = Edge, 1 = Level triggered interrupt on pin[5|37|69|101|133|165|197] */
    UINT32  IntTrigType6:       1;      /* [6] 0 = Edge, 1 = Level triggered interrupt on pin[6|38|70|102|134|166|198] */
    UINT32  IntTrigType7:       1;      /* [7] 0 = Edge, 1 = Level triggered interrupt on pin[7|39|71|103|135|167|199] */
    UINT32  IntTrigType8:       1;      /* [8] 0 = Edge, 1 = Level triggered interrupt on pin[8|40|72|104|136|168|200] */
    UINT32  IntTrigType9:       1;      /* [9] 0 = Edge, 1 = Level triggered interrupt on pin[9|41|73|105|137|169|201] */
    UINT32  IntTrigType10:      1;      /* [10] 0 = Edge, 1 = Level triggered interrupt on pin[10|42|74|106|138|170|202] */
    UINT32  IntTrigType11:      1;      /* [11] 0 = Edge, 1 = Level triggered interrupt on pin[11|43|75|107|139|171|203] */
    UINT32  IntTrigType12:      1;      /* [12] 0 = Edge, 1 = Level triggered interrupt on pin[12|44|76|108|140|172|204] */
    UINT32  IntTrigType13:      1;      /* [13] 0 = Edge, 1 = Level triggered interrupt on pin[13|45|77|109|141|173|205] */
    UINT32  IntTrigType14:      1;      /* [14] 0 = Edge, 1 = Level triggered interrupt on pin[14|46|78|110|142|174|206] */
    UINT32  IntTrigType15:      1;      /* [15] 0 = Edge, 1 = Level triggered interrupt on pin[15|47|79|111|143|175|207] */
    UINT32  IntTrigType16:      1;      /* [16] 0 = Edge, 1 = Level triggered interrupt on pin[16|48|80|112|144|176|208] */
    UINT32  IntTrigType17:      1;      /* [17] 0 = Edge, 1 = Level triggered interrupt on pin[17|49|81|113|145|177|209] */
    UINT32  IntTrigType18:      1;      /* [18] 0 = Edge, 1 = Level triggered interrupt on pin[18|50|82|114|146|178|210] */
    UINT32  IntTrigType19:      1;      /* [19] 0 = Edge, 1 = Level triggered interrupt on pin[19|51|83|115|147|179|211] */
    UINT32  IntTrigType20:      1;      /* [20] 0 = Edge, 1 = Level triggered interrupt on pin[20|52|84|116|148|180|212] */
    UINT32  IntTrigType21:      1;      /* [21] 0 = Edge, 1 = Level triggered interrupt on pin[21|53|85|117|149|181|213] */
    UINT32  IntTrigType22:      1;      /* [22] 0 = Edge, 1 = Level triggered interrupt on pin[22|54|86|118|150|182|214] */
    UINT32  IntTrigType23:      1;      /* [23] 0 = Edge, 1 = Level triggered interrupt on pin[23|55|87|119|151|183|215] */
    UINT32  IntTrigType24:      1;      /* [24] 0 = Edge, 1 = Level triggered interrupt on pin[24|56|88|120|152|184|216] */
    UINT32  IntTrigType25:      1;      /* [25] 0 = Edge, 1 = Level triggered interrupt on pin[25|57|89|121|153|185|217] */
    UINT32  IntTrigType26:      1;      /* [26] 0 = Edge, 1 = Level triggered interrupt on pin[26|58|90|122|154|186|218] */
    UINT32  IntTrigType27:      1;      /* [27] 0 = Edge, 1 = Level triggered interrupt on pin[27|59|91|123|155|187|219] */
    UINT32  IntTrigType28:      1;      /* [28] 0 = Edge, 1 = Level triggered interrupt on pin[28|60|92|124|156|188|220] */
    UINT32  IntTrigType29:      1;      /* [29] 0 = Edge, 1 = Level triggered interrupt on pin[29|61|93|125|157|189|221] */
    UINT32  IntTrigType30:      1;      /* [30] 0 = Edge, 1 = Level triggered interrupt on pin[30|62|94|126|158|190|222] */
    UINT32  IntTrigType31:      1;      /* [31] 0 = Edge, 1 = Level triggered interrupt on pin[31|63|95|127|159|191|223] */
} AMBA_GPIO_INT_TRIG_TYPE0_REG_s;

typedef struct {
    UINT32  IntBothEdge0:       1;      /* [0] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[0|32|64|96|128|160|192] */
    UINT32  IntBothEdge1:       1;      /* [1] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[1|33|65|97|129|161|193] */
    UINT32  IntBothEdge2:       1;      /* [2] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[2|34|66|98|130|162|194] */
    UINT32  IntBothEdge3:       1;      /* [3] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[3|35|67|99|131|163|195] */
    UINT32  IntBothEdge4:       1;      /* [4] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[4|36|68|100|132|164|196] */
    UINT32  IntBothEdge5:       1;      /* [5] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[5|37|69|101|133|165|197] */
    UINT32  IntBothEdge6:       1;      /* [6] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[6|38|70|102|134|166|198] */
    UINT32  IntBothEdge7:       1;      /* [7] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[7|39|71|103|135|167|199] */
    UINT32  IntBothEdge8:       1;      /* [8] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[8|40|72|104|136|168|200] */
    UINT32  IntBothEdge9:       1;      /* [9] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[9|41|73|105|137|169|201] */
    UINT32  IntBothEdge10:      1;      /* [10] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[10|42|74|106|138|170|202] */
    UINT32  IntBothEdge11:      1;      /* [11] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[11|43|75|107|139|171|203] */
    UINT32  IntBothEdge12:      1;      /* [12] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[12|44|76|108|140|172|204] */
    UINT32  IntBothEdge13:      1;      /* [13] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[13|45|77|109|141|173|205] */
    UINT32  IntBothEdge14:      1;      /* [14] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[14|46|78|110|142|174|206] */
    UINT32  IntBothEdge15:      1;      /* [15] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[15|47|79|111|143|175|207] */
    UINT32  IntBothEdge16:      1;      /* [16] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[16|48|80|112|144|176|208] */
    UINT32  IntBothEdge17:      1;      /* [17] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[17|49|81|113|145|177|209] */
    UINT32  IntBothEdge18:      1;      /* [18] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[18|50|82|114|146|178|210] */
    UINT32  IntBothEdge19:      1;      /* [19] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[19|51|83|115|147|179|211] */
    UINT32  IntBothEdge20:      1;      /* [20] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[20|52|84|116|148|180|212] */
    UINT32  IntBothEdge21:      1;      /* [21] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[21|53|85|117|149|181|213] */
    UINT32  IntBothEdge22:      1;      /* [22] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[22|54|86|118|150|182|214] */
    UINT32  IntBothEdge23:      1;      /* [23] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[23|55|87|119|151|183|215] */
    UINT32  IntBothEdge24:      1;      /* [24] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[24|56|88|120|152|184|216] */
    UINT32  IntBothEdge25:      1;      /* [25] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[25|57|89|121|153|185|217] */
    UINT32  IntBothEdge26:      1;      /* [26] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[26|58|90|122|154|186|218] */
    UINT32  IntBothEdge27:      1;      /* [27] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[27|59|91|123|155|187|219] */
    UINT32  IntBothEdge28:      1;      /* [28] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[28|60|92|124|156|188|220] */
    UINT32  IntBothEdge29:      1;      /* [29] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[29|61|93|125|157|189|221] */
    UINT32  IntBothEdge30:      1;      /* [30] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[30|62|94|126|158|190|222] */
    UINT32  IntBothEdge31:      1;      /* [31] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[31|63|95|127|159|191|223] */
} AMBA_GPIO_INT_TRIG_TYPE1_REG_s;

typedef struct {
    UINT32  IntEdgeType0:       1;      /* [0] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[0|32|64|96|128|160|192] */
    UINT32  IntEdgeType1:       1;      /* [1] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[1|33|65|97|129|161|193] */
    UINT32  IntEdgeType2:       1;      /* [2] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[2|34|66|98|130|162|194] */
    UINT32  IntEdgeType3:       1;      /* [3] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[3|35|67|99|131|163|195] */
    UINT32  IntEdgeType4:       1;      /* [4] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[4|36|68|100|132|164|196] */
    UINT32  IntEdgeType5:       1;      /* [5] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[5|37|69|101|133|165|197] */
    UINT32  IntEdgeType6:       1;      /* [6] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[6|38|70|102|134|166|198] */
    UINT32  IntEdgeType7:       1;      /* [7] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[7|39|71|103|135|167|199] */
    UINT32  IntEdgeType8:       1;      /* [8] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[8|40|72|104|136|168|200] */
    UINT32  IntEdgeType9:       1;      /* [9] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[9|41|73|105|137|169|201] */
    UINT32  IntEdgeType10:      1;      /* [10] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[10|42|74|106|138|170|202] */
    UINT32  IntEdgeType11:      1;      /* [11] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[11|43|75|107|139|171|203] */
    UINT32  IntEdgeType12:      1;      /* [12] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[12|44|76|108|140|172|204] */
    UINT32  IntEdgeType13:      1;      /* [13] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[13|45|77|109|141|173|205] */
    UINT32  IntEdgeType14:      1;      /* [14] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[14|46|78|110|142|174|206] */
    UINT32  IntEdgeType15:      1;      /* [15] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[15|47|79|111|143|175|207] */
    UINT32  IntEdgeType16:      1;      /* [16] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[16|48|80|112|144|176|208] */
    UINT32  IntEdgeType17:      1;      /* [17] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[17|49|81|113|145|177|209] */
    UINT32  IntEdgeType18:      1;      /* [18] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[18|50|82|114|146|178|210] */
    UINT32  IntEdgeType19:      1;      /* [19] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[19|51|83|115|147|179|211] */
    UINT32  IntEdgeType20:      1;      /* [20] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[20|52|84|116|148|180|212] */
    UINT32  IntEdgeType21:      1;      /* [21] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[21|53|85|117|149|181|213] */
    UINT32  IntEdgeType22:      1;      /* [22] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[22|54|86|118|150|182|214] */
    UINT32  IntEdgeType23:      1;      /* [23] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[23|55|87|119|151|183|215] */
    UINT32  IntEdgeType24:      1;      /* [24] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[24|56|88|120|152|184|216] */
    UINT32  IntEdgeType25:      1;      /* [25] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[25|57|89|121|153|185|217] */
    UINT32  IntEdgeType26:      1;      /* [26] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[26|58|90|122|154|186|218] */
    UINT32  IntEdgeType27:      1;      /* [27] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[27|59|91|123|155|187|219] */
    UINT32  IntEdgeType28:      1;      /* [28] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[28|60|92|124|156|188|220] */
    UINT32  IntEdgeType29:      1;      /* [29] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[29|61|93|125|157|189|221] */
    UINT32  IntEdgeType30:      1;      /* [30] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[30|62|94|126|158|190|222] */
    UINT32  IntEdgeType31:      1;      /* [31] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[31|63|95|127|159|191|223] */
} AMBA_GPIO_INT_TRIG_TYPE2_REG_s;

/*
 * GPIO: Interrupt Enable Register
 */
typedef struct {
    UINT32  IntEnable0:         1;      /* [0] 0 = Masked, 1 = Not masked interrupt for pin[0|32|64|96|128|160|192] */
    UINT32  IntEnable1:         1;      /* [1] 0 = Masked, 1 = Not masked interrupt for pin[1|33|65|97|129|161|193] */
    UINT32  IntEnable2:         1;      /* [2] 0 = Masked, 1 = Not masked interrupt for pin[2|34|66|98|130|162|194] */
    UINT32  IntEnable3:         1;      /* [3] 0 = Masked, 1 = Not masked interrupt for pin[3|35|67|99|131|163|195] */
    UINT32  IntEnable4:         1;      /* [4] 0 = Masked, 1 = Not masked interrupt for pin[4|36|68|100|132|164|196] */
    UINT32  IntEnable5:         1;      /* [5] 0 = Masked, 1 = Not masked interrupt for pin[5|37|69|101|133|165|197] */
    UINT32  IntEnable6:         1;      /* [6] 0 = Masked, 1 = Not masked interrupt for pin[6|38|70|102|134|166|198] */
    UINT32  IntEnable7:         1;      /* [7] 0 = Masked, 1 = Not masked interrupt for pin[7|39|71|103|135|167|199] */
    UINT32  IntEnable8:         1;      /* [8] 0 = Masked, 1 = Not masked interrupt for pin[8|40|72|104|136|168|200] */
    UINT32  IntEnable9:         1;      /* [9] 0 = Masked, 1 = Not masked interrupt for pin[9|41|73|105|137|169|201] */
    UINT32  IntEnable10:        1;      /* [10] 0 = Masked, 1 = Not masked interrupt for pin[10|42|74|106|138|170|202] */
    UINT32  IntEnable11:        1;      /* [11] 0 = Masked, 1 = Not masked interrupt for pin[11|43|75|107|139|171|203] */
    UINT32  IntEnable12:        1;      /* [12] 0 = Masked, 1 = Not masked interrupt for pin[12|44|76|108|140|172|204] */
    UINT32  IntEnable13:        1;      /* [13] 0 = Masked, 1 = Not masked interrupt for pin[13|45|77|109|141|173|205] */
    UINT32  IntEnable14:        1;      /* [14] 0 = Masked, 1 = Not masked interrupt for pin[14|46|78|110|142|174|206] */
    UINT32  IntEnable15:        1;      /* [15] 0 = Masked, 1 = Not masked interrupt for pin[15|47|79|111|143|175|207] */
    UINT32  IntEnable16:        1;      /* [16] 0 = Masked, 1 = Not masked interrupt for pin[16|48|80|112|144|176|208] */
    UINT32  IntEnable17:        1;      /* [17] 0 = Masked, 1 = Not masked interrupt for pin[17|49|81|113|145|177|209] */
    UINT32  IntEnable18:        1;      /* [18] 0 = Masked, 1 = Not masked interrupt for pin[18|50|82|114|146|178|210] */
    UINT32  IntEnable19:        1;      /* [19] 0 = Masked, 1 = Not masked interrupt for pin[19|51|83|115|147|179|211] */
    UINT32  IntEnable20:        1;      /* [20] 0 = Masked, 1 = Not masked interrupt for pin[20|52|84|116|148|180|212] */
    UINT32  IntEnable21:        1;      /* [21] 0 = Masked, 1 = Not masked interrupt for pin[21|53|85|117|149|181|213] */
    UINT32  IntEnable22:        1;      /* [22] 0 = Masked, 1 = Not masked interrupt for pin[22|54|86|118|150|182|214] */
    UINT32  IntEnable23:        1;      /* [23] 0 = Masked, 1 = Not masked interrupt for pin[23|55|87|119|151|183|215] */
    UINT32  IntEnable24:        1;      /* [24] 0 = Masked, 1 = Not masked interrupt for pin[24|56|88|120|152|184|216] */
    UINT32  IntEnable25:        1;      /* [25] 0 = Masked, 1 = Not masked interrupt for pin[25|57|89|121|153|185|217] */
    UINT32  IntEnable26:        1;      /* [26] 0 = Masked, 1 = Not masked interrupt for pin[26|58|90|122|154|186|218] */
    UINT32  IntEnable27:        1;      /* [27] 0 = Masked, 1 = Not masked interrupt for pin[27|59|91|123|155|187|219] */
    UINT32  IntEnable28:        1;      /* [28] 0 = Masked, 1 = Not masked interrupt for pin[28|60|92|124|156|188|220] */
    UINT32  IntEnable29:        1;      /* [29] 0 = Masked, 1 = Not masked interrupt for pin[29|61|93|125|157|189|221] */
    UINT32  IntEnable30:        1;      /* [30] 0 = Masked, 1 = Not masked interrupt for pin[30|62|94|126|158|190|222] */
    UINT32  IntEnable31:        1;      /* [31] 0 = Masked, 1 = Not masked interrupt for pin[31|63|95|127|159|191|223] */
} AMBA_GPIO_INT_ENABLE_REG_s;

/*
 * GPIO: Alternate Function Select Register
 */
typedef struct {
    UINT32  AltFunc0:           1;      /* [0] 0 = GPIO, 1 = Alternate HW function for pin[0|32|64|96|128|160|192] */
    UINT32  AltFunc1:           1;      /* [1] 0 = GPIO, 1 = Alternate HW function for pin[1|33|65|97|129|161|193] */
    UINT32  AltFunc2:           1;      /* [2] 0 = GPIO, 1 = Alternate HW function for pin[2|34|66|98|130|162|194] */
    UINT32  AltFunc3:           1;      /* [3] 0 = GPIO, 1 = Alternate HW function for pin[3|35|67|99|131|163|195] */
    UINT32  AltFunc4:           1;      /* [4] 0 = GPIO, 1 = Alternate HW function for pin[4|36|68|100|132|164|196] */
    UINT32  AltFunc5:           1;      /* [5] 0 = GPIO, 1 = Alternate HW function for pin[5|37|69|101|133|165|197] */
    UINT32  AltFunc6:           1;      /* [6] 0 = GPIO, 1 = Alternate HW function for pin[6|38|70|102|134|166|198] */
    UINT32  AltFunc7:           1;      /* [7] 0 = GPIO, 1 = Alternate HW function for pin[7|39|71|103|135|167|199] */
    UINT32  AltFunc8:           1;      /* [8] 0 = GPIO, 1 = Alternate HW function for pin[8|40|72|104|136|168|200] */
    UINT32  AltFunc9:           1;      /* [9] 0 = GPIO, 1 = Alternate HW function for pin[9|41|73|105|137|169|201] */
    UINT32  AltFunc10:          1;      /* [10] 0 = GPIO, 1 = Alternate HW function for pin[10|42|74|106|138|170|202] */
    UINT32  AltFunc11:          1;      /* [11] 0 = GPIO, 1 = Alternate HW function for pin[11|43|75|107|139|171|203] */
    UINT32  AltFunc12:          1;      /* [12] 0 = GPIO, 1 = Alternate HW function for pin[12|44|76|108|140|172|204] */
    UINT32  AltFunc13:          1;      /* [13] 0 = GPIO, 1 = Alternate HW function for pin[13|45|77|109|141|173|205] */
    UINT32  AltFunc14:          1;      /* [14] 0 = GPIO, 1 = Alternate HW function for pin[14|46|78|110|142|174|206] */
    UINT32  AltFunc15:          1;      /* [15] 0 = GPIO, 1 = Alternate HW function for pin[15|47|79|111|143|175|207] */
    UINT32  AltFunc16:          1;      /* [16] 0 = GPIO, 1 = Alternate HW function for pin[16|48|80|112|144|176|208] */
    UINT32  AltFunc17:          1;      /* [17] 0 = GPIO, 1 = Alternate HW function for pin[17|49|81|113|145|177|209] */
    UINT32  AltFunc18:          1;      /* [18] 0 = GPIO, 1 = Alternate HW function for pin[18|50|82|114|146|178|210] */
    UINT32  AltFunc19:          1;      /* [19] 0 = GPIO, 1 = Alternate HW function for pin[19|51|83|115|147|179|211] */
    UINT32  AltFunc20:          1;      /* [20] 0 = GPIO, 1 = Alternate HW function for pin[20|52|84|116|148|180|212] */
    UINT32  AltFunc21:          1;      /* [21] 0 = GPIO, 1 = Alternate HW function for pin[21|53|85|117|149|181|213] */
    UINT32  AltFunc22:          1;      /* [22] 0 = GPIO, 1 = Alternate HW function for pin[22|54|86|118|150|182|214] */
    UINT32  AltFunc23:          1;      /* [23] 0 = GPIO, 1 = Alternate HW function for pin[23|55|87|119|151|183|215] */
    UINT32  AltFunc24:          1;      /* [24] 0 = GPIO, 1 = Alternate HW function for pin[24|56|88|120|152|184|216] */
    UINT32  AltFunc25:          1;      /* [25] 0 = GPIO, 1 = Alternate HW function for pin[25|57|89|121|153|185|217] */
    UINT32  AltFunc26:          1;      /* [26] 0 = GPIO, 1 = Alternate HW function for pin[26|58|90|122|154|186|218] */
    UINT32  AltFunc27:          1;      /* [27] 0 = GPIO, 1 = Alternate HW function for pin[27|59|91|123|155|187|219] */
    UINT32  AltFunc28:          1;      /* [28] 0 = GPIO, 1 = Alternate HW function for pin[28|60|92|124|156|188|220] */
    UINT32  AltFunc29:          1;      /* [29] 0 = GPIO, 1 = Alternate HW function for pin[29|61|93|125|157|189|221] */
    UINT32  AltFunc30:          1;      /* [30] 0 = GPIO, 1 = Alternate HW function for pin[30|62|94|126|158|190|222] */
    UINT32  AltFunc31:          1;      /* [31] 0 = GPIO, 1 = Alternate HW function for pin[31|63|95|127|159|191|223] */
} AMBA_GPIO_ALT_FUNC_CTRL_REG_s;

/*
 * GPIO: Interrput Status Register
 */
typedef struct {
    UINT32  IntStatus0:         1;      /* [0] 1 = Interrupt condition is detected on pin[0|32|64|96|128|160|192] */
    UINT32  IntStatus1:         1;      /* [1] 1 = Interrupt condition is detected on pin[1|33|65|97|129|161|193] */
    UINT32  IntStatus2:         1;      /* [2] 1 = Interrupt condition is detected on pin[2|34|66|98|130|162|194] */
    UINT32  IntStatus3:         1;      /* [3] 1 = Interrupt condition is detected on pin[3|35|67|99|131|163|195] */
    UINT32  IntStatus4:         1;      /* [4] 1 = Interrupt condition is detected on pin[4|36|68|100|132|164|196] */
    UINT32  IntStatus5:         1;      /* [5] 1 = Interrupt condition is detected on pin[5|37|69|101|133|165|197] */
    UINT32  IntStatus6:         1;      /* [6] 1 = Interrupt condition is detected on pin[6|38|70|102|134|166|198] */
    UINT32  IntStatus7:         1;      /* [7] 1 = Interrupt condition is detected on pin[7|39|71|103|135|167|199] */
    UINT32  IntStatus8:         1;      /* [8] 1 = Interrupt condition is detected on pin[8|40|72|104|136|168|200] */
    UINT32  IntStatus9:         1;      /* [9] 1 = Interrupt condition is detected on pin[9|41|73|105|137|169|201] */
    UINT32  IntStatus10:        1;      /* [10] 1 = Interrupt condition is detected on pin[10|42|74|106|138|170|202] */
    UINT32  IntStatus11:        1;      /* [11] 1 = Interrupt condition is detected on pin[11|43|75|107|139|171|203] */
    UINT32  IntStatus12:        1;      /* [12] 1 = Interrupt condition is detected on pin[12|44|76|108|140|172|204] */
    UINT32  IntStatus13:        1;      /* [13] 1 = Interrupt condition is detected on pin[13|45|77|109|141|173|205] */
    UINT32  IntStatus14:        1;      /* [14] 1 = Interrupt condition is detected on pin[14|46|78|110|142|174|206] */
    UINT32  IntStatus15:        1;      /* [15] 1 = Interrupt condition is detected on pin[15|47|79|111|143|175|207] */
    UINT32  IntStatus16:        1;      /* [16] 1 = Interrupt condition is detected on pin[16|48|80|112|144|176|208] */
    UINT32  IntStatus17:        1;      /* [17] 1 = Interrupt condition is detected on pin[17|49|81|113|145|177|209] */
    UINT32  IntStatus18:        1;      /* [18] 1 = Interrupt condition is detected on pin[18|50|82|114|146|178|210] */
    UINT32  IntStatus19:        1;      /* [19] 1 = Interrupt condition is detected on pin[19|51|83|115|147|179|211] */
    UINT32  IntStatus20:        1;      /* [20] 1 = Interrupt condition is detected on pin[20|52|84|116|148|180|212] */
    UINT32  IntStatus21:        1;      /* [21] 1 = Interrupt condition is detected on pin[21|53|85|117|149|181|213] */
    UINT32  IntStatus22:        1;      /* [22] 1 = Interrupt condition is detected on pin[22|54|86|118|150|182|214] */
    UINT32  IntStatus23:        1;      /* [23] 1 = Interrupt condition is detected on pin[23|55|87|119|151|183|215] */
    UINT32  IntStatus24:        1;      /* [24] 1 = Interrupt condition is detected on pin[24|56|88|120|152|184|216] */
    UINT32  IntStatus25:        1;      /* [25] 1 = Interrupt condition is detected on pin[25|57|89|121|153|185|217] */
    UINT32  IntStatus26:        1;      /* [26] 1 = Interrupt condition is detected on pin[26|58|90|122|154|186|218] */
    UINT32  IntStatus27:        1;      /* [27] 1 = Interrupt condition is detected on pin[27|59|91|123|155|187|219] */
    UINT32  IntStatus28:        1;      /* [28] 1 = Interrupt condition is detected on pin[28|60|92|124|156|188|220] */
    UINT32  IntStatus29:        1;      /* [29] 1 = Interrupt condition is detected on pin[29|61|93|125|157|189|221] */
    UINT32  IntStatus30:        1;      /* [30] 1 = Interrupt condition is detected on pin[30|62|94|126|158|190|222] */
    UINT32  IntStatus31:        1;      /* [31] 1 = Interrupt condition is detected on pin[31|63|95|127|159|191|223] */
} AMBA_GPIO_INT_STATUS_REG_s;

/*
 * GPIO: Interrput Clear Register (only work for edge-sensitive interrupts)
 */
typedef struct {
    UINT32  ClearInt0:          1;      /* [0] 1 = Clear edge detection logic of pin[0|32|64|96|128|160|192] */
    UINT32  ClearInt1:          1;      /* [1] 1 = Clear edge detection logic of pin[1|33|65|97|129|161|193] */
    UINT32  ClearInt2:          1;      /* [2] 1 = Clear edge detection logic of pin[2|34|66|98|130|162|194] */
    UINT32  ClearInt3:          1;      /* [3] 1 = Clear edge detection logic of pin[3|35|67|99|131|163|195] */
    UINT32  ClearInt4:          1;      /* [4] 1 = Clear edge detection logic of pin[4|36|68|100|132|164|196] */
    UINT32  ClearInt5:          1;      /* [5] 1 = Clear edge detection logic of pin[5|37|69|101|133|165|197] */
    UINT32  ClearInt6:          1;      /* [6] 1 = Clear edge detection logic of pin[6|38|70|102|134|166|198] */
    UINT32  ClearInt7:          1;      /* [7] 1 = Clear edge detection logic of pin[7|39|71|103|135|167|199] */
    UINT32  ClearInt8:          1;      /* [8] 1 = Clear edge detection logic of pin[8|40|72|104|136|168|200] */
    UINT32  ClearInt9:          1;      /* [9] 1 = Clear edge detection logic of pin[9|41|73|105|137|169|201] */
    UINT32  ClearInt10:         1;      /* [10] 1 = Clear edge detection logic of pin[10|42|74|106|138|170|202] */
    UINT32  ClearInt11:         1;      /* [11] 1 = Clear edge detection logic of pin[11|43|75|107|139|171|203] */
    UINT32  ClearInt12:         1;      /* [12] 1 = Clear edge detection logic of pin[12|44|76|108|140|172|204] */
    UINT32  ClearInt13:         1;      /* [13] 1 = Clear edge detection logic of pin[13|45|77|109|141|173|205] */
    UINT32  ClearInt14:         1;      /* [14] 1 = Clear edge detection logic of pin[14|46|78|110|142|174|206] */
    UINT32  ClearInt15:         1;      /* [15] 1 = Clear edge detection logic of pin[15|47|79|111|143|175|207] */
    UINT32  ClearInt16:         1;      /* [16] 1 = Clear edge detection logic of pin[16|48|80|112|144|176|208] */
    UINT32  ClearInt17:         1;      /* [17] 1 = Clear edge detection logic of pin[17|49|81|113|145|177|209] */
    UINT32  ClearInt18:         1;      /* [18] 1 = Clear edge detection logic of pin[18|50|82|114|146|178|210] */
    UINT32  ClearInt19:         1;      /* [19] 1 = Clear edge detection logic of pin[19|51|83|115|147|179|211] */
    UINT32  ClearInt20:         1;      /* [20] 1 = Clear edge detection logic of pin[20|52|84|116|148|180|212] */
    UINT32  ClearInt21:         1;      /* [21] 1 = Clear edge detection logic of pin[21|53|85|117|149|181|213] */
    UINT32  ClearInt22:         1;      /* [22] 1 = Clear edge detection logic of pin[22|54|86|118|150|182|214] */
    UINT32  ClearInt23:         1;      /* [23] 1 = Clear edge detection logic of pin[23|55|87|119|151|183|215] */
    UINT32  ClearInt24:         1;      /* [24] 1 = Clear edge detection logic of pin[24|56|88|120|152|184|216] */
    UINT32  ClearInt25:         1;      /* [25] 1 = Clear edge detection logic of pin[25|57|89|121|153|185|217] */
    UINT32  ClearInt26:         1;      /* [26] 1 = Clear edge detection logic of pin[26|58|90|122|154|186|218] */
    UINT32  ClearInt27:         1;      /* [27] 1 = Clear edge detection logic of pin[27|59|91|123|155|187|219] */
    UINT32  ClearInt28:         1;      /* [28] 1 = Clear edge detection logic of pin[28|60|92|124|156|188|220] */
    UINT32  ClearInt29:         1;      /* [29] 1 = Clear edge detection logic of pin[29|61|93|125|157|189|221] */
    UINT32  ClearInt30:         1;      /* [30] 1 = Clear edge detection logic of pin[30|62|94|126|158|190|222] */
    UINT32  ClearInt31:         1;      /* [31] 1 = Clear edge detection logic of pin[31|63|95|127|159|191|223] */
} AMBA_GPIO_INT_CLR_REG_s;

/*
 * GPIO: Data Enable Register
 */
typedef struct {
    UINT32  Enable0:            1;      /* [0] 1 = Enable logic state access for pin[0|32|64|96|128|160|192] */
    UINT32  Enable1:            1;      /* [1] 1 = Enable logic state access for pin[1|33|65|97|129|161|193] */
    UINT32  Enable2:            1;      /* [2] 1 = Enable logic state access for pin[2|34|66|98|130|162|194] */
    UINT32  Enable3:            1;      /* [3] 1 = Enable logic state access for pin[3|35|67|99|131|163|195] */
    UINT32  Enable4:            1;      /* [4] 1 = Enable logic state access for pin[4|36|68|100|132|164|196] */
    UINT32  Enable5:            1;      /* [5] 1 = Enable logic state access for pin[5|37|69|101|133|165|197] */
    UINT32  Enable6:            1;      /* [6] 1 = Enable logic state access for pin[6|38|70|102|134|166|198] */
    UINT32  Enable7:            1;      /* [7] 1 = Enable logic state access for pin[7|39|71|103|135|167|199] */
    UINT32  Enable8:            1;      /* [8] 1 = Enable logic state access for pin[8|40|72|104|136|168|200] */
    UINT32  Enable9:            1;      /* [9] 1 = Enable logic state access for pin[9|41|73|105|137|169|201] */
    UINT32  Enable10:           1;      /* [10] 1 = Enable logic state access for pin[10|42|74|106|138|170|202] */
    UINT32  Enable11:           1;      /* [11] 1 = Enable logic state access for pin[11|43|75|107|139|171|203] */
    UINT32  Enable12:           1;      /* [12] 1 = Enable logic state access for pin[12|44|76|108|140|172|204] */
    UINT32  Enable13:           1;      /* [13] 1 = Enable logic state access for pin[13|45|77|109|141|173|205] */
    UINT32  Enable14:           1;      /* [14] 1 = Enable logic state access for pin[14|46|78|110|142|174|206] */
    UINT32  Enable15:           1;      /* [15] 1 = Enable logic state access for pin[15|47|79|111|143|175|207] */
    UINT32  Enable16:           1;      /* [16] 1 = Enable logic state access for pin[16|48|80|112|144|176|208] */
    UINT32  Enable17:           1;      /* [17] 1 = Enable logic state access for pin[17|49|81|113|145|177|209] */
    UINT32  Enable18:           1;      /* [18] 1 = Enable logic state access for pin[18|50|82|114|146|178|210] */
    UINT32  Enable19:           1;      /* [19] 1 = Enable logic state access for pin[19|51|83|115|147|179|211] */
    UINT32  Enable20:           1;      /* [20] 1 = Enable logic state access for pin[20|52|84|116|148|180|212] */
    UINT32  Enable21:           1;      /* [21] 1 = Enable logic state access for pin[21|53|85|117|149|181|213] */
    UINT32  Enable22:           1;      /* [22] 1 = Enable logic state access for pin[22|54|86|118|150|182|214] */
    UINT32  Enable23:           1;      /* [23] 1 = Enable logic state access for pin[23|55|87|119|151|183|215] */
    UINT32  Enable24:           1;      /* [24] 1 = Enable logic state access for pin[24|56|88|120|152|184|216] */
    UINT32  Enable25:           1;      /* [25] 1 = Enable logic state access for pin[25|57|89|121|153|185|217] */
    UINT32  Enable26:           1;      /* [26] 1 = Enable logic state access for pin[26|58|90|122|154|186|218] */
    UINT32  Enable27:           1;      /* [27] 1 = Enable logic state access for pin[27|59|91|123|155|187|219] */
    UINT32  Enable28:           1;      /* [28] 1 = Enable logic state access for pin[28|60|92|124|156|188|220] */
    UINT32  Enable29:           1;      /* [29] 1 = Enable logic state access for pin[29|61|93|125|157|189|221] */
    UINT32  Enable30:           1;      /* [30] 1 = Enable logic state access for pin[30|62|94|126|158|190|222] */
    UINT32  Enable31:           1;      /* [31] 1 = Enable logic state access for pin[31|63|95|127|159|191|223] */
} AMBA_GPIO_ENABLE_REG_s;

/*
 * GPIO: Global Enable Register
 */
typedef struct {
    UINT32  Enable:             1;       /* [0] 0 = Disabled, 1 = Enabled */
    UINT32  Reserved:           31;      /* [31:1] Reserved */
} AMBA_GPIO_GLOBAL_ENABLE_REG_s;

/*
 * GPIO: All Registers
 */
typedef struct {
    volatile UINT32                         PinLevel;       /* 0x000(RW): Pin Data Register */
    volatile UINT32                         PinDirection;   /* 0x004(RW): Pin Direction Register */
    volatile UINT32                         IntTrigType0;   /* 0x008(RW): Interrupt Sensitivity Control Register */
    volatile UINT32                         IntTrigType1;   /* 0x00C(RW): Interrupt on both Rising and Falling Data Edge Register */
    volatile UINT32                         IntTrigType2;   /* 0x010(RW): Interrupt on Data Low/High Level (Falling/Rising Edge) Register */
    volatile UINT32                         IntEnable;      /* 0x014(RW): Interrupt Enable (Unmask) Control Register */
    volatile UINT32                         PinAltFuncSel;  /* 0x018(RW): Pin Alternate Function Selection Register */
    volatile UINT32                         IntStatusRaw;   /* 0x01C(RO): Raw Interrupt Status Register */
    volatile UINT32                         IntStatus;      /* 0x020(RO): Masked Interrupt Status Register */
    volatile UINT32                         IntClear;       /* 0x024(WO): Interrupt Clear Register */
    volatile UINT32                         PinMask;        /* 0x028(RW): Pin Data Enable Register */
    volatile AMBA_GPIO_GLOBAL_ENABLE_REG_s  Enable;         /* 0x02C(WO): Enable Register */
} AMBA_GPIO_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_GPIO_REG_s * pAmbaGPIO_Reg[AMBA_NUM_GPIO_GROUP];

#endif /* AMBA_REG_GPIO_H */
