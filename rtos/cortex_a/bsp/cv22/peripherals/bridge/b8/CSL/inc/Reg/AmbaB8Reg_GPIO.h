/**
 *  @file AmbaReg_GPIO.h
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
 *  @details Definitions & Constants for General Purpose Input / Output (GPIO) Control Registers
 *
 */

#ifndef AMBA_B8_REG_GPIO_H
#define AMBA_B8_REG_GPIO_H

/*-----------------------------------------------------------------------------------------------*\
 * Definitions & Constants for GPIO Data Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Data0:              1;      /* [0] 0 = Low data level, 1 = High data level of pin[0|32|64|96|128] */
    UINT32  Data1:              1;      /* [1] 0 = Low data level, 1 = High data level of pin[1|33|65|97|129] */
    UINT32  Data2:              1;      /* [2] 0 = Low data level, 1 = High data level of pin[2|34|66|98|130] */
    UINT32  Data3:              1;      /* [3] 0 = Low data level, 1 = High data level of pin[3|35|67|99|131] */
    UINT32  Data4:              1;      /* [4] 0 = Low data level, 1 = High data level of pin[4|36|68|100|132] */
    UINT32  Data5:              1;      /* [5] 0 = Low data level, 1 = High data level of pin[5|37|69|101|133] */
    UINT32  Data6:              1;      /* [6] 0 = Low data level, 1 = High data level of pin[6|38|70|102|134] */
    UINT32  Data7:              1;      /* [7] 0 = Low data level, 1 = High data level of pin[7|39|71|103|135] */
    UINT32  Data8:              1;      /* [8] 0 = Low data level, 1 = High data level of pin[8|40|72|104|136] */
    UINT32  Data9:              1;      /* [9] 0 = Low data level, 1 = High data level of pin[9|41|73|105|137] */
    UINT32  Data10:             1;      /* [10] 0 = Low data level, 1 = High data level of pin[10|42|74|106|138] */
    UINT32  Data11:             1;      /* [11] 0 = Low data level, 1 = High data level of pin[11|43|75|107|139] */
    UINT32  Data12:             1;      /* [12] 0 = Low data level, 1 = High data level of pin[12|44|76|108|140] */
    UINT32  Data13:             1;      /* [13] 0 = Low data level, 1 = High data level of pin[13|45|77|109|141] */
    UINT32  Data14:             1;      /* [14] 0 = Low data level, 1 = High data level of pin[14|46|78|110|142] */
    UINT32  Data15:             1;      /* [15] 0 = Low data level, 1 = High data level of pin[15|47|79|111|143] */
    UINT32  Data16:             1;      /* [16] 0 = Low data level, 1 = High data level of pin[16|48|80|112|144] */
    UINT32  Data17:             1;      /* [17] 0 = Low data level, 1 = High data level of pin[17|49|81|113|145] */
    UINT32  Data18:             1;      /* [18] 0 = Low data level, 1 = High data level of pin[18|50|82|114|146] */
    UINT32  Data19:             1;      /* [19] 0 = Low data level, 1 = High data level of pin[19|51|83|115|147] */
    UINT32  Data20:             1;      /* [20] 0 = Low data level, 1 = High data level of pin[20|52|84|116|148] */
    UINT32  Data21:             1;      /* [21] 0 = Low data level, 1 = High data level of pin[21|53|85|117|149] */
    UINT32  Data22:             1;      /* [22] 0 = Low data level, 1 = High data level of pin[22|54|86|118|150] */
    UINT32  Data23:             1;      /* [23] 0 = Low data level, 1 = High data level of pin[23|55|87|119|151] */
    UINT32  Data24:             1;      /* [24] 0 = Low data level, 1 = High data level of pin[24|56|88|120|152] */
    UINT32  Data25:             1;      /* [25] 0 = Low data level, 1 = High data level of pin[25|57|89|121|153] */
    UINT32  Data26:             1;      /* [26] 0 = Low data level, 1 = High data level of pin[26|58|90|122|154] */
    UINT32  Data27:             1;      /* [27] 0 = Low data level, 1 = High data level of pin[27|59|91|123|155] */
    UINT32  Data28:             1;      /* [28] 0 = Low data level, 1 = High data level of pin[28|60|92|124|156] */
    UINT32  Data29:             1;      /* [29] 0 = Low data level, 1 = High data level of pin[29|61|93|125|157] */
    UINT32  Data30:             1;      /* [30] 0 = Low data level, 1 = High data level of pin[30|62|94|126|158] */
    UINT32  Data31:             1;      /* [31] 0 = Low data level, 1 = High data level of pin[31|63|95|127|159] */
} B8_GPIO_DATA_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions & Constants for GPIO Data Direction Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Direction0:         1;      /* [0] 0 = Input, 1 = Output GPIO direction of pin[0|32|64|96|128] */
    UINT32  Direction1:         1;      /* [1] 0 = Input, 1 = Output GPIO direction of pin[1|33|65|97|129] */
    UINT32  Direction2:         1;      /* [2] 0 = Input, 1 = Output GPIO direction of pin[2|34|66|98|130] */
    UINT32  Direction3:         1;      /* [3] 0 = Input, 1 = Output GPIO direction of pin[3|35|67|99|131] */
    UINT32  Direction4:         1;      /* [4] 0 = Input, 1 = Output GPIO direction of pin[4|36|68|100|132] */
    UINT32  Direction5:         1;      /* [5] 0 = Input, 1 = Output GPIO direction of pin[5|37|69|101|133] */
    UINT32  Direction6:         1;      /* [6] 0 = Input, 1 = Output GPIO direction of pin[6|38|70|102|134] */
    UINT32  Direction7:         1;      /* [7] 0 = Input, 1 = Output GPIO direction of pin[7|39|71|103|135] */
    UINT32  Direction8:         1;      /* [8] 0 = Input, 1 = Output GPIO direction of pin[8|40|72|104|136] */
    UINT32  Direction9:         1;      /* [9] 0 = Input, 1 = Output GPIO direction of pin[9|41|73|105|137] */
    UINT32  Direction10:        1;      /* [10] 0 = Input, 1 = Output GPIO direction of pin[10|42|74|106|138] */
    UINT32  Direction11:        1;      /* [11] 0 = Input, 1 = Output GPIO direction of pin[11|43|75|107|139] */
    UINT32  Direction12:        1;      /* [12] 0 = Input, 1 = Output GPIO direction of pin[12|44|76|108|140] */
    UINT32  Direction13:        1;      /* [13] 0 = Input, 1 = Output GPIO direction of pin[13|45|77|109|141] */
    UINT32  Direction14:        1;      /* [14] 0 = Input, 1 = Output GPIO direction of pin[14|46|78|110|142] */
    UINT32  Direction15:        1;      /* [15] 0 = Input, 1 = Output GPIO direction of pin[15|47|79|111|143] */
    UINT32  Direction16:        1;      /* [16] 0 = Input, 1 = Output GPIO direction of pin[16|48|80|112|144] */
    UINT32  Direction17:        1;      /* [17] 0 = Input, 1 = Output GPIO direction of pin[17|49|81|113|145] */
    UINT32  Direction18:        1;      /* [18] 0 = Input, 1 = Output GPIO direction of pin[18|50|82|114|146] */
    UINT32  Direction19:        1;      /* [19] 0 = Input, 1 = Output GPIO direction of pin[19|51|83|115|147] */
    UINT32  Direction20:        1;      /* [20] 0 = Input, 1 = Output GPIO direction of pin[20|52|84|116|148] */
    UINT32  Direction21:        1;      /* [21] 0 = Input, 1 = Output GPIO direction of pin[21|53|85|117|149] */
    UINT32  Direction22:        1;      /* [22] 0 = Input, 1 = Output GPIO direction of pin[22|54|86|118|150] */
    UINT32  Direction23:        1;      /* [23] 0 = Input, 1 = Output GPIO direction of pin[23|55|87|119|151] */
    UINT32  Direction24:        1;      /* [24] 0 = Input, 1 = Output GPIO direction of pin[24|56|88|120|152] */
    UINT32  Direction25:        1;      /* [25] 0 = Input, 1 = Output GPIO direction of pin[25|57|89|121|153] */
    UINT32  Direction26:        1;      /* [26] 0 = Input, 1 = Output GPIO direction of pin[26|58|90|122|154] */
    UINT32  Direction27:        1;      /* [27] 0 = Input, 1 = Output GPIO direction of pin[27|59|91|123|155] */
    UINT32  Direction28:        1;      /* [28] 0 = Input, 1 = Output GPIO direction of pin[28|60|92|124|156] */
    UINT32  Direction29:        1;      /* [29] 0 = Input, 1 = Output GPIO direction of pin[29|61|93|125|157] */
    UINT32  Direction30:        1;      /* [30] 0 = Input, 1 = Output GPIO direction of pin[30|62|94|126|158] */
    UINT32  Direction31:        1;      /* [31] 0 = Input, 1 = Output GPIO direction of pin[31|63|95|127|159] */
} B8_GPIO_DIR_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions & Constants for GPIO Edge/Level Interrupt Sensitivity Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  IntTrigType0:       1;      /* [0] 0 = Edge, 1 = Level triggered interrupt on pin[0|32|64|96|128] */
    UINT32  IntTrigType1:       1;      /* [1] 0 = Edge, 1 = Level triggered interrupt on pin[1|33|65|97|129] */
    UINT32  IntTrigType2:       1;      /* [2] 0 = Edge, 1 = Level triggered interrupt on pin[2|34|66|98|130] */
    UINT32  IntTrigType3:       1;      /* [3] 0 = Edge, 1 = Level triggered interrupt on pin[3|35|67|99|131] */
    UINT32  IntTrigType4:       1;      /* [4] 0 = Edge, 1 = Level triggered interrupt on pin[4|36|68|100|132] */
    UINT32  IntTrigType5:       1;      /* [5] 0 = Edge, 1 = Level triggered interrupt on pin[5|37|69|101|133] */
    UINT32  IntTrigType6:       1;      /* [6] 0 = Edge, 1 = Level triggered interrupt on pin[6|38|70|102|134] */
    UINT32  IntTrigType7:       1;      /* [7] 0 = Edge, 1 = Level triggered interrupt on pin[7|39|71|103|135] */
    UINT32  IntTrigType8:       1;      /* [8] 0 = Edge, 1 = Level triggered interrupt on pin[8|40|72|104|136] */
    UINT32  IntTrigType9:       1;      /* [9] 0 = Edge, 1 = Level triggered interrupt on pin[9|41|73|105|137] */
    UINT32  IntTrigType10:      1;      /* [10] 0 = Edge, 1 = Level triggered interrupt on pin[10|42|74|106|138] */
    UINT32  IntTrigType11:      1;      /* [11] 0 = Edge, 1 = Level triggered interrupt on pin[11|43|75|107|139] */
    UINT32  IntTrigType12:      1;      /* [12] 0 = Edge, 1 = Level triggered interrupt on pin[12|44|76|108|140] */
    UINT32  IntTrigType13:      1;      /* [13] 0 = Edge, 1 = Level triggered interrupt on pin[13|45|77|109|141] */
    UINT32  IntTrigType14:      1;      /* [14] 0 = Edge, 1 = Level triggered interrupt on pin[14|46|78|110|142] */
    UINT32  IntTrigType15:      1;      /* [15] 0 = Edge, 1 = Level triggered interrupt on pin[15|47|79|111|143] */
    UINT32  IntTrigType16:      1;      /* [16] 0 = Edge, 1 = Level triggered interrupt on pin[16|48|80|112|144] */
    UINT32  IntTrigType17:      1;      /* [17] 0 = Edge, 1 = Level triggered interrupt on pin[17|49|81|113|145] */
    UINT32  IntTrigType18:      1;      /* [18] 0 = Edge, 1 = Level triggered interrupt on pin[18|50|82|114|146] */
    UINT32  IntTrigType19:      1;      /* [19] 0 = Edge, 1 = Level triggered interrupt on pin[19|51|83|115|147] */
    UINT32  IntTrigType20:      1;      /* [20] 0 = Edge, 1 = Level triggered interrupt on pin[20|52|84|116|148] */
    UINT32  IntTrigType21:      1;      /* [21] 0 = Edge, 1 = Level triggered interrupt on pin[21|53|85|117|149] */
    UINT32  IntTrigType22:      1;      /* [22] 0 = Edge, 1 = Level triggered interrupt on pin[22|54|86|118|150] */
    UINT32  IntTrigType23:      1;      /* [23] 0 = Edge, 1 = Level triggered interrupt on pin[23|55|87|119|151] */
    UINT32  IntTrigType24:      1;      /* [24] 0 = Edge, 1 = Level triggered interrupt on pin[24|56|88|120|152] */
    UINT32  IntTrigType25:      1;      /* [25] 0 = Edge, 1 = Level triggered interrupt on pin[25|57|89|121|153] */
    UINT32  IntTrigType26:      1;      /* [26] 0 = Edge, 1 = Level triggered interrupt on pin[26|58|90|122|154] */
    UINT32  IntTrigType27:      1;      /* [27] 0 = Edge, 1 = Level triggered interrupt on pin[27|59|91|123|155] */
    UINT32  IntTrigType28:      1;      /* [28] 0 = Edge, 1 = Level triggered interrupt on pin[28|60|92|124|156] */
    UINT32  IntTrigType29:      1;      /* [29] 0 = Edge, 1 = Level triggered interrupt on pin[29|61|93|125|157] */
    UINT32  IntTrigType30:      1;      /* [30] 0 = Edge, 1 = Level triggered interrupt on pin[30|62|94|126|158] */
    UINT32  IntTrigType31:      1;      /* [31] 0 = Edge, 1 = Level triggered interrupt on pin[31|63|95|127|159] */
} B8_GPIO_IS_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions & Constants for GPIO Edge Interrupt Sensitivity Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  IntBothEdge0:       1;      /* [0] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[0|32|64|96|128] */
    UINT32  IntBothEdge1:       1;      /* [1] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[1|33|65|97|129] */
    UINT32  IntBothEdge2:       1;      /* [2] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[2|34|66|98|130] */
    UINT32  IntBothEdge3:       1;      /* [3] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[3|35|67|99|131] */
    UINT32  IntBothEdge4:       1;      /* [4] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[4|36|68|100|132] */
    UINT32  IntBothEdge5:       1;      /* [5] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[5|37|69|101|133] */
    UINT32  IntBothEdge6:       1;      /* [6] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[6|38|70|102|134] */
    UINT32  IntBothEdge7:       1;      /* [7] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[7|39|71|103|135] */
    UINT32  IntBothEdge8:       1;      /* [8] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[8|40|72|104|136] */
    UINT32  IntBothEdge9:       1;      /* [9] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[9|41|73|105|137] */
    UINT32  IntBothEdge10:      1;      /* [10] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[10|42|74|106|138] */
    UINT32  IntBothEdge11:      1;      /* [11] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[11|43|75|107|139] */
    UINT32  IntBothEdge12:      1;      /* [12] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[12|44|76|108|140] */
    UINT32  IntBothEdge13:      1;      /* [13] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[13|45|77|109|141] */
    UINT32  IntBothEdge14:      1;      /* [14] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[14|46|78|110|142] */
    UINT32  IntBothEdge15:      1;      /* [15] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[15|47|79|111|143] */
    UINT32  IntBothEdge16:      1;      /* [16] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[16|48|80|112|144] */
    UINT32  IntBothEdge17:      1;      /* [17] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[17|49|81|113|145] */
    UINT32  IntBothEdge18:      1;      /* [18] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[18|50|82|114|146] */
    UINT32  IntBothEdge19:      1;      /* [19] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[19|51|83|115|147] */
    UINT32  IntBothEdge20:      1;      /* [20] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[20|52|84|116|148] */
    UINT32  IntBothEdge21:      1;      /* [21] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[21|53|85|117|149] */
    UINT32  IntBothEdge22:      1;      /* [22] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[22|54|86|118|150] */
    UINT32  IntBothEdge23:      1;      /* [23] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[23|55|87|119|151] */
    UINT32  IntBothEdge24:      1;      /* [24] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[24|56|88|120|152] */
    UINT32  IntBothEdge25:      1;      /* [25] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[25|57|89|121|153] */
    UINT32  IntBothEdge26:      1;      /* [26] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[26|58|90|122|154] */
    UINT32  IntBothEdge27:      1;      /* [27] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[27|59|91|123|155] */
    UINT32  IntBothEdge28:      1;      /* [28] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[28|60|92|124|156] */
    UINT32  IntBothEdge29:      1;      /* [29] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[29|61|93|125|157] */
    UINT32  IntBothEdge30:      1;      /* [30] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[30|62|94|126|158] */
    UINT32  IntBothEdge31:      1;      /* [31] 0 = Single edge/level, 1 = Dual edge/level triggered interrupt on pin[31|63|95|127|159] */
} B8_GPIO_IBE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions & Constants for GPIO Interrupt Event Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  IntEdgeType0:       1;      /* [0] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[0|32|64|96|128] */
    UINT32  IntEdgeType1:       1;      /* [1] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[1|33|65|97|129] */
    UINT32  IntEdgeType2:       1;      /* [2] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[2|34|66|98|130] */
    UINT32  IntEdgeType3:       1;      /* [3] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[3|35|67|99|131] */
    UINT32  IntEdgeType4:       1;      /* [4] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[4|36|68|100|132] */
    UINT32  IntEdgeType5:       1;      /* [5] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[5|37|69|101|133] */
    UINT32  IntEdgeType6:       1;      /* [6] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[6|38|70|102|134] */
    UINT32  IntEdgeType7:       1;      /* [7] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[7|39|71|103|135] */
    UINT32  IntEdgeType8:       1;      /* [8] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[8|40|72|104|136] */
    UINT32  IntEdgeType9:       1;      /* [9] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[9|41|73|105|137] */
    UINT32  IntEdgeType10:      1;      /* [10] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[10|42|74|106|138] */
    UINT32  IntEdgeType11:      1;      /* [11] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[11|43|75|107|139] */
    UINT32  IntEdgeType12:      1;      /* [12] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[12|44|76|108|140] */
    UINT32  IntEdgeType13:      1;      /* [13] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[13|45|77|109|141] */
    UINT32  IntEdgeType14:      1;      /* [14] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[14|46|78|110|142] */
    UINT32  IntEdgeType15:      1;      /* [15] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[15|47|79|111|143] */
    UINT32  IntEdgeType16:      1;      /* [16] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[16|48|80|112|144] */
    UINT32  IntEdgeType17:      1;      /* [17] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[17|49|81|113|145] */
    UINT32  IntEdgeType18:      1;      /* [18] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[18|50|82|114|146] */
    UINT32  IntEdgeType19:      1;      /* [19] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[19|51|83|115|147] */
    UINT32  IntEdgeType20:      1;      /* [20] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[20|52|84|116|148] */
    UINT32  IntEdgeType21:      1;      /* [21] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[21|53|85|117|149] */
    UINT32  IntEdgeType22:      1;      /* [22] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[22|54|86|118|150] */
    UINT32  IntEdgeType23:      1;      /* [23] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[23|55|87|119|151] */
    UINT32  IntEdgeType24:      1;      /* [24] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[24|56|88|120|152] */
    UINT32  IntEdgeType25:      1;      /* [25] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[25|57|89|121|153] */
    UINT32  IntEdgeType26:      1;      /* [26] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[26|58|90|122|154] */
    UINT32  IntEdgeType27:      1;      /* [27] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[27|59|91|123|155] */
    UINT32  IntEdgeType28:      1;      /* [28] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[28|60|92|124|156] */
    UINT32  IntEdgeType29:      1;      /* [29] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[29|61|93|125|157] */
    UINT32  IntEdgeType30:      1;      /* [30] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[30|62|94|126|158] */
    UINT32  IntEdgeType31:      1;      /* [31] 0 = Falling edges/low level, 1 = Rising edges/high level triggered interrupt on pin[31|63|95|127|159] */
} B8_GPIO_IEV_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions & Constants for GPIO Interrupt Mask Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  IntEnable0:         1;      /* [0] 0 = Masked, 1 = Not masked interrupt for pin[0|32|64|96|128] */
    UINT32  IntEnable1:         1;      /* [1] 0 = Masked, 1 = Not masked interrupt for pin[1|33|65|97|129] */
    UINT32  IntEnable2:         1;      /* [2] 0 = Masked, 1 = Not masked interrupt for pin[2|34|66|98|130] */
    UINT32  IntEnable3:         1;      /* [3] 0 = Masked, 1 = Not masked interrupt for pin[3|35|67|99|131] */
    UINT32  IntEnable4:         1;      /* [4] 0 = Masked, 1 = Not masked interrupt for pin[4|36|68|100|132] */
    UINT32  IntEnable5:         1;      /* [5] 0 = Masked, 1 = Not masked interrupt for pin[5|37|69|101|133] */
    UINT32  IntEnable6:         1;      /* [6] 0 = Masked, 1 = Not masked interrupt for pin[6|38|70|102|134] */
    UINT32  IntEnable7:         1;      /* [7] 0 = Masked, 1 = Not masked interrupt for pin[7|39|71|103|135] */
    UINT32  IntEnable8:         1;      /* [8] 0 = Masked, 1 = Not masked interrupt for pin[8|40|72|104|136] */
    UINT32  IntEnable9:         1;      /* [9] 0 = Masked, 1 = Not masked interrupt for pin[9|41|73|105|137] */
    UINT32  IntEnable10:        1;      /* [10] 0 = Masked, 1 = Not masked interrupt for pin[10|42|74|106|138] */
    UINT32  IntEnable11:        1;      /* [11] 0 = Masked, 1 = Not masked interrupt for pin[11|43|75|107|139] */
    UINT32  IntEnable12:        1;      /* [12] 0 = Masked, 1 = Not masked interrupt for pin[12|44|76|108|140] */
    UINT32  IntEnable13:        1;      /* [13] 0 = Masked, 1 = Not masked interrupt for pin[13|45|77|109|141] */
    UINT32  IntEnable14:        1;      /* [14] 0 = Masked, 1 = Not masked interrupt for pin[14|46|78|110|142] */
    UINT32  IntEnable15:        1;      /* [15] 0 = Masked, 1 = Not masked interrupt for pin[15|47|79|111|143] */
    UINT32  IntEnable16:        1;      /* [16] 0 = Masked, 1 = Not masked interrupt for pin[16|48|80|112|144] */
    UINT32  IntEnable17:        1;      /* [17] 0 = Masked, 1 = Not masked interrupt for pin[17|49|81|113|145] */
    UINT32  IntEnable18:        1;      /* [18] 0 = Masked, 1 = Not masked interrupt for pin[18|50|82|114|146] */
    UINT32  IntEnable19:        1;      /* [19] 0 = Masked, 1 = Not masked interrupt for pin[19|51|83|115|147] */
    UINT32  IntEnable20:        1;      /* [20] 0 = Masked, 1 = Not masked interrupt for pin[20|52|84|116|148] */
    UINT32  IntEnable21:        1;      /* [21] 0 = Masked, 1 = Not masked interrupt for pin[21|53|85|117|149] */
    UINT32  IntEnable22:        1;      /* [22] 0 = Masked, 1 = Not masked interrupt for pin[22|54|86|118|150] */
    UINT32  IntEnable23:        1;      /* [23] 0 = Masked, 1 = Not masked interrupt for pin[23|55|87|119|151] */
    UINT32  IntEnable24:        1;      /* [24] 0 = Masked, 1 = Not masked interrupt for pin[24|56|88|120|152] */
    UINT32  IntEnable25:        1;      /* [25] 0 = Masked, 1 = Not masked interrupt for pin[25|57|89|121|153] */
    UINT32  IntEnable26:        1;      /* [26] 0 = Masked, 1 = Not masked interrupt for pin[26|58|90|122|154] */
    UINT32  IntEnable27:        1;      /* [27] 0 = Masked, 1 = Not masked interrupt for pin[27|59|91|123|155] */
    UINT32  IntEnable28:        1;      /* [28] 0 = Masked, 1 = Not masked interrupt for pin[28|60|92|124|156] */
    UINT32  IntEnable29:        1;      /* [29] 0 = Masked, 1 = Not masked interrupt for pin[29|61|93|125|157] */
    UINT32  IntEnable30:        1;      /* [30] 0 = Masked, 1 = Not masked interrupt for pin[30|62|94|126|158] */
    UINT32  IntEnable31:        1;      /* [31] 0 = Masked, 1 = Not masked interrupt for pin[31|63|95|127|159] */
} B8_GPIO_IE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions & Constants for GPIO Raw Interrput Status Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RawIntStatus0:      1;      /* [0] 1 = Interrupt condition is detected on pin[0|32|64|96|128] */
    UINT32  RawIntStatus1:      1;      /* [1] 1 = Interrupt condition is detected on pin[1|33|65|97|129] */
    UINT32  RawIntStatus2:      1;      /* [2] 1 = Interrupt condition is detected on pin[2|34|66|98|130] */
    UINT32  RawIntStatus3:      1;      /* [3] 1 = Interrupt condition is detected on pin[3|35|67|99|131] */
    UINT32  RawIntStatus4:      1;      /* [4] 1 = Interrupt condition is detected on pin[4|36|68|100|132] */
    UINT32  RawIntStatus5:      1;      /* [5] 1 = Interrupt condition is detected on pin[5|37|69|101|133] */
    UINT32  RawIntStatus6:      1;      /* [6] 1 = Interrupt condition is detected on pin[6|38|70|102|134] */
    UINT32  RawIntStatus7:      1;      /* [7] 1 = Interrupt condition is detected on pin[7|39|71|103|135] */
    UINT32  RawIntStatus8:      1;      /* [8] 1 = Interrupt condition is detected on pin[8|40|72|104|136] */
    UINT32  RawIntStatus9:      1;      /* [9] 1 = Interrupt condition is detected on pin[9|41|73|105|137] */
    UINT32  RawIntStatus10:     1;      /* [10] 1 = Interrupt condition is detected on pin[10|42|74|106|138] */
    UINT32  RawIntStatus11:     1;      /* [11] 1 = Interrupt condition is detected on pin[11|43|75|107|139] */
    UINT32  RawIntStatus12:     1;      /* [12] 1 = Interrupt condition is detected on pin[12|44|76|108|140] */
    UINT32  RawIntStatus13:     1;      /* [13] 1 = Interrupt condition is detected on pin[13|45|77|109|141] */
    UINT32  RawIntStatus14:     1;      /* [14] 1 = Interrupt condition is detected on pin[14|46|78|110|142] */
    UINT32  RawIntStatus15:     1;      /* [15] 1 = Interrupt condition is detected on pin[15|47|79|111|143] */
    UINT32  RawIntStatus16:     1;      /* [16] 1 = Interrupt condition is detected on pin[16|48|80|112|144] */
    UINT32  RawIntStatus17:     1;      /* [17] 1 = Interrupt condition is detected on pin[17|49|81|113|145] */
    UINT32  RawIntStatus18:     1;      /* [18] 1 = Interrupt condition is detected on pin[18|50|82|114|146] */
    UINT32  RawIntStatus19:     1;      /* [19] 1 = Interrupt condition is detected on pin[19|51|83|115|147] */
    UINT32  RawIntStatus20:     1;      /* [20] 1 = Interrupt condition is detected on pin[20|52|84|116|148] */
    UINT32  RawIntStatus21:     1;      /* [21] 1 = Interrupt condition is detected on pin[21|53|85|117|149] */
    UINT32  RawIntStatus22:     1;      /* [22] 1 = Interrupt condition is detected on pin[22|54|86|118|150] */
    UINT32  RawIntStatus23:     1;      /* [23] 1 = Interrupt condition is detected on pin[23|55|87|119|151] */
    UINT32  RawIntStatus24:     1;      /* [24] 1 = Interrupt condition is detected on pin[24|56|88|120|152] */
    UINT32  RawIntStatus25:     1;      /* [25] 1 = Interrupt condition is detected on pin[25|57|89|121|153] */
    UINT32  RawIntStatus26:     1;      /* [26] 1 = Interrupt condition is detected on pin[26|58|90|122|154] */
    UINT32  RawIntStatus27:     1;      /* [27] 1 = Interrupt condition is detected on pin[27|59|91|123|155] */
    UINT32  RawIntStatus28:     1;      /* [28] 1 = Interrupt condition is detected on pin[28|60|92|124|156] */
    UINT32  RawIntStatus29:     1;      /* [29] 1 = Interrupt condition is detected on pin[29|61|93|125|157] */
    UINT32  RawIntStatus30:     1;      /* [30] 1 = Interrupt condition is detected on pin[30|62|94|126|158] */
    UINT32  RawIntStatus31:     1;      /* [31] 1 = Interrupt condition is detected on pin[31|63|95|127|159] */
} B8_GPIO_RIS_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions & Constants for GPIO Masked Interrput Status Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  MaskedIntStatus0:   1;      /* [0] 1 = Asserting interrupt for pin[0|32|64|96|128] */
    UINT32  MaskedIntStatus1:   1;      /* [1] 1 = Asserting interrupt for pin[1|33|65|97|129] */
    UINT32  MaskedIntStatus2:   1;      /* [2] 1 = Asserting interrupt for pin[2|34|66|98|130] */
    UINT32  MaskedIntStatus3:   1;      /* [3] 1 = Asserting interrupt for pin[3|35|67|99|131] */
    UINT32  MaskedIntStatus4:   1;      /* [4] 1 = Asserting interrupt for pin[4|36|68|100|132] */
    UINT32  MaskedIntStatus5:   1;      /* [5] 1 = Asserting interrupt for pin[5|37|69|101|133] */
    UINT32  MaskedIntStatus6:   1;      /* [6] 1 = Asserting interrupt for pin[6|38|70|102|134] */
    UINT32  MaskedIntStatus7:   1;      /* [7] 1 = Asserting interrupt for pin[7|39|71|103|135] */
    UINT32  MaskedIntStatus8:   1;      /* [8] 1 = Asserting interrupt for pin[8|40|72|104|136] */
    UINT32  MaskedIntStatus9:   1;      /* [9] 1 = Asserting interrupt for pin[9|41|73|105|137] */
    UINT32  MaskedIntStatus10:  1;      /* [10] 1 = Asserting interrupt for pin[10|42|74|106|138] */
    UINT32  MaskedIntStatus11:  1;      /* [11] 1 = Asserting interrupt for pin[11|43|75|107|139] */
    UINT32  MaskedIntStatus12:  1;      /* [12] 1 = Asserting interrupt for pin[12|44|76|108|140] */
    UINT32  MaskedIntStatus13:  1;      /* [13] 1 = Asserting interrupt for pin[13|45|77|109|141] */
    UINT32  MaskedIntStatus14:  1;      /* [14] 1 = Asserting interrupt for pin[14|46|78|110|142] */
    UINT32  MaskedIntStatus15:  1;      /* [15] 1 = Asserting interrupt for pin[15|47|79|111|143] */
    UINT32  MaskedIntStatus16:  1;      /* [16] 1 = Asserting interrupt for pin[16|48|80|112|144] */
    UINT32  MaskedIntStatus17:  1;      /* [17] 1 = Asserting interrupt for pin[17|49|81|113|145] */
    UINT32  MaskedIntStatus18:  1;      /* [18] 1 = Asserting interrupt for pin[18|50|82|114|146] */
    UINT32  MaskedIntStatus19:  1;      /* [19] 1 = Asserting interrupt for pin[19|51|83|115|147] */
    UINT32  MaskedIntStatus20:  1;      /* [20] 1 = Asserting interrupt for pin[20|52|84|116|148] */
    UINT32  MaskedIntStatus21:  1;      /* [21] 1 = Asserting interrupt for pin[21|53|85|117|149] */
    UINT32  MaskedIntStatus22:  1;      /* [22] 1 = Asserting interrupt for pin[22|54|86|118|150] */
    UINT32  MaskedIntStatus23:  1;      /* [23] 1 = Asserting interrupt for pin[23|55|87|119|151] */
    UINT32  MaskedIntStatus24:  1;      /* [24] 1 = Asserting interrupt for pin[24|56|88|120|152] */
    UINT32  MaskedIntStatus25:  1;      /* [25] 1 = Asserting interrupt for pin[25|57|89|121|153] */
    UINT32  MaskedIntStatus26:  1;      /* [26] 1 = Asserting interrupt for pin[26|58|90|122|154] */
    UINT32  MaskedIntStatus27:  1;      /* [27] 1 = Asserting interrupt for pin[27|59|91|123|155] */
    UINT32  MaskedIntStatus28:  1;      /* [28] 1 = Asserting interrupt for pin[28|60|92|124|156] */
    UINT32  MaskedIntStatus29:  1;      /* [29] 1 = Asserting interrupt for pin[29|61|93|125|157] */
    UINT32  MaskedIntStatus30:  1;      /* [30] 1 = Asserting interrupt for pin[30|62|94|126|158] */
    UINT32  MaskedIntStatus31:  1;      /* [31] 1 = Asserting interrupt for pin[31|63|95|127|159] */
} B8_GPIO_MIS_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions & Constants for GPIO Interrput Clear Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ClearInt0:          1;      /* [0] 1 = Clear edge detection logic of pin[0|32|64|96|128] */
    UINT32  ClearInt1:          1;      /* [1] 1 = Clear edge detection logic of pin[1|33|65|97|129] */
    UINT32  ClearInt2:          1;      /* [2] 1 = Clear edge detection logic of pin[2|34|66|98|130] */
    UINT32  ClearInt3:          1;      /* [3] 1 = Clear edge detection logic of pin[3|35|67|99|131] */
    UINT32  ClearInt4:          1;      /* [4] 1 = Clear edge detection logic of pin[4|36|68|100|132] */
    UINT32  ClearInt5:          1;      /* [5] 1 = Clear edge detection logic of pin[5|37|69|101|133] */
    UINT32  ClearInt6:          1;      /* [6] 1 = Clear edge detection logic of pin[6|38|70|102|134] */
    UINT32  ClearInt7:          1;      /* [7] 1 = Clear edge detection logic of pin[7|39|71|103|135] */
    UINT32  ClearInt8:          1;      /* [8] 1 = Clear edge detection logic of pin[8|40|72|104|136] */
    UINT32  ClearInt9:          1;      /* [9] 1 = Clear edge detection logic of pin[9|41|73|105|137] */
    UINT32  ClearInt10:         1;      /* [10] 1 = Clear edge detection logic of pin[10|42|74|106|138] */
    UINT32  ClearInt11:         1;      /* [11] 1 = Clear edge detection logic of pin[11|43|75|107|139] */
    UINT32  ClearInt12:         1;      /* [12] 1 = Clear edge detection logic of pin[12|44|76|108|140] */
    UINT32  ClearInt13:         1;      /* [13] 1 = Clear edge detection logic of pin[13|45|77|109|141] */
    UINT32  ClearInt14:         1;      /* [14] 1 = Clear edge detection logic of pin[14|46|78|110|142] */
    UINT32  ClearInt15:         1;      /* [15] 1 = Clear edge detection logic of pin[15|47|79|111|143] */
    UINT32  ClearInt16:         1;      /* [16] 1 = Clear edge detection logic of pin[16|48|80|112|144] */
    UINT32  ClearInt17:         1;      /* [17] 1 = Clear edge detection logic of pin[17|49|81|113|145] */
    UINT32  ClearInt18:         1;      /* [18] 1 = Clear edge detection logic of pin[18|50|82|114|146] */
    UINT32  ClearInt19:         1;      /* [19] 1 = Clear edge detection logic of pin[19|51|83|115|147] */
    UINT32  ClearInt20:         1;      /* [20] 1 = Clear edge detection logic of pin[20|52|84|116|148] */
    UINT32  ClearInt21:         1;      /* [21] 1 = Clear edge detection logic of pin[21|53|85|117|149] */
    UINT32  ClearInt22:         1;      /* [22] 1 = Clear edge detection logic of pin[22|54|86|118|150] */
    UINT32  ClearInt23:         1;      /* [23] 1 = Clear edge detection logic of pin[23|55|87|119|151] */
    UINT32  ClearInt24:         1;      /* [24] 1 = Clear edge detection logic of pin[24|56|88|120|152] */
    UINT32  ClearInt25:         1;      /* [25] 1 = Clear edge detection logic of pin[25|57|89|121|153] */
    UINT32  ClearInt26:         1;      /* [26] 1 = Clear edge detection logic of pin[26|58|90|122|154] */
    UINT32  ClearInt27:         1;      /* [27] 1 = Clear edge detection logic of pin[27|59|91|123|155] */
    UINT32  ClearInt28:         1;      /* [28] 1 = Clear edge detection logic of pin[28|60|92|124|156] */
    UINT32  ClearInt29:         1;      /* [29] 1 = Clear edge detection logic of pin[29|61|93|125|157] */
    UINT32  ClearInt30:         1;      /* [30] 1 = Clear edge detection logic of pin[30|62|94|126|158] */
    UINT32  ClearInt31:         1;      /* [31] 1 = Clear edge detection logic of pin[31|63|95|127|159] */
} B8_GPIO_IC_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions & Constants for GPIO Data Mask Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DataMask0:          1;      /* [0] 1 = Enable software data level access for pin[0|32|64|96|128] */
    UINT32  DataMask1:          1;      /* [1] 1 = Enable software data level access for pin[1|33|65|97|129] */
    UINT32  DataMask2:          1;      /* [2] 1 = Enable software data level access for pin[2|34|66|98|130] */
    UINT32  DataMask3:          1;      /* [3] 1 = Enable software data level access for pin[3|35|67|99|131] */
    UINT32  DataMask4:          1;      /* [4] 1 = Enable software data level access for pin[4|36|68|100|132] */
    UINT32  DataMask5:          1;      /* [5] 1 = Enable software data level access for pin[5|37|69|101|133] */
    UINT32  DataMask6:          1;      /* [6] 1 = Enable software data level access for pin[6|38|70|102|134] */
    UINT32  DataMask7:          1;      /* [7] 1 = Enable software data level access for pin[7|39|71|103|135] */
    UINT32  DataMask8:          1;      /* [8] 1 = Enable software data level access for pin[8|40|72|104|136] */
    UINT32  DataMask9:          1;      /* [9] 1 = Enable software data level access for pin[9|41|73|105|137] */
    UINT32  DataMask10:         1;      /* [10] 1 = Enable software data level access for pin[10|42|74|106|138] */
    UINT32  DataMask11:         1;      /* [11] 1 = Enable software data level access for pin[11|43|75|107|139] */
    UINT32  DataMask12:         1;      /* [12] 1 = Enable software data level access for pin[12|44|76|108|140] */
    UINT32  DataMask13:         1;      /* [13] 1 = Enable software data level access for pin[13|45|77|109|141] */
    UINT32  DataMask14:         1;      /* [14] 1 = Enable software data level access for pin[14|46|78|110|142] */
    UINT32  DataMask15:         1;      /* [15] 1 = Enable software data level access for pin[15|47|79|111|143] */
    UINT32  DataMask16:         1;      /* [16] 1 = Enable software data level access for pin[16|48|80|112|144] */
    UINT32  DataMask17:         1;      /* [17] 1 = Enable software data level access for pin[17|49|81|113|145] */
    UINT32  DataMask18:         1;      /* [18] 1 = Enable software data level access for pin[18|50|82|114|146] */
    UINT32  DataMask19:         1;      /* [19] 1 = Enable software data level access for pin[19|51|83|115|147] */
    UINT32  DataMask20:         1;      /* [20] 1 = Enable software data level access for pin[20|52|84|116|148] */
    UINT32  DataMask21:         1;      /* [21] 1 = Enable software data level access for pin[21|53|85|117|149] */
    UINT32  DataMask22:         1;      /* [22] 1 = Enable software data level access for pin[22|54|86|118|150] */
    UINT32  DataMask23:         1;      /* [23] 1 = Enable software data level access for pin[23|55|87|119|151] */
    UINT32  DataMask24:         1;      /* [24] 1 = Enable software data level access for pin[24|56|88|120|152] */
    UINT32  DataMask25:         1;      /* [25] 1 = Enable software data level access for pin[25|57|89|121|153] */
    UINT32  DataMask26:         1;      /* [26] 1 = Enable software data level access for pin[26|58|90|122|154] */
    UINT32  DataMask27:         1;      /* [27] 1 = Enable software data level access for pin[27|59|91|123|155] */
    UINT32  DataMask28:         1;      /* [28] 1 = Enable software data level access for pin[28|60|92|124|156] */
    UINT32  DataMask29:         1;      /* [29] 1 = Enable software data level access for pin[29|61|93|125|157] */
    UINT32  DataMask30:         1;      /* [30] 1 = Enable software data level access for pin[30|62|94|126|158] */
    UINT32  DataMask31:         1;      /* [31] 1 = Enable software data level access for pin[31|63|95|127|159] */
} B8_GPIO_MASK_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions & Constants for GPIO Enable Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Enable:             1;       /* [0] 0 = Disabled, 1 = Enabled */
    UINT32  Reserved:           31;      /* [31:1] Reserved */
} B8_GPIO_ENABLE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions & Constants for GPIO Mode Select Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ModeSelect0:        1;      /* [0] 0 = Software control mode, 1 = Hardware control mode for pin[0|32|64|96|128] */
    UINT32  ModeSelect1:        1;      /* [1] 0 = Software control mode, 1 = Hardware control mode for pin[1|33|65|97|129] */
    UINT32  ModeSelect2:        1;      /* [2] 0 = Software control mode, 1 = Hardware control mode for pin[2|34|66|98|130] */
    UINT32  ModeSelect3:        1;      /* [3] 0 = Software control mode, 1 = Hardware control mode for pin[3|35|67|99|131] */
    UINT32  ModeSelect4:        1;      /* [4] 0 = Software control mode, 1 = Hardware control mode for pin[4|36|68|100|132] */
    UINT32  ModeSelect5:        1;      /* [5] 0 = Software control mode, 1 = Hardware control mode for pin[5|37|69|101|133] */
    UINT32  ModeSelect6:        1;      /* [6] 0 = Software control mode, 1 = Hardware control mode for pin[6|38|70|102|134] */
    UINT32  ModeSelect7:        1;      /* [7] 0 = Software control mode, 1 = Hardware control mode for pin[7|39|71|103|135] */
    UINT32  ModeSelect8:        1;      /* [8] 0 = Software control mode, 1 = Hardware control mode for pin[8|40|72|104|136] */
    UINT32  ModeSelect9:        1;      /* [9] 0 = Software control mode, 1 = Hardware control mode for pin[9|41|73|105|137] */
    UINT32  ModeSelect10:       1;      /* [10] 0 = Software control mode, 1 = Hardware control mode for pin[10|42|74|106|138] */
    UINT32  ModeSelect11:       1;      /* [11] 0 = Software control mode, 1 = Hardware control mode for pin[11|43|75|107|139] */
    UINT32  ModeSelect12:       1;      /* [12] 0 = Software control mode, 1 = Hardware control mode for pin[12|44|76|108|140] */
    UINT32  ModeSelect13:       1;      /* [13] 0 = Software control mode, 1 = Hardware control mode for pin[13|45|77|109|141] */
    UINT32  ModeSelect14:       1;      /* [14] 0 = Software control mode, 1 = Hardware control mode for pin[14|46|78|110|142] */
    UINT32  ModeSelect15:       1;      /* [15] 0 = Software control mode, 1 = Hardware control mode for pin[15|47|79|111|143] */
    UINT32  ModeSelect16:       1;      /* [16] 0 = Software control mode, 1 = Hardware control mode for pin[16|48|80|112|144] */
    UINT32  ModeSelect17:       1;      /* [17] 0 = Software control mode, 1 = Hardware control mode for pin[17|49|81|113|145] */
    UINT32  ModeSelect18:       1;      /* [18] 0 = Software control mode, 1 = Hardware control mode for pin[18|50|82|114|146] */
    UINT32  ModeSelect19:       1;      /* [19] 0 = Software control mode, 1 = Hardware control mode for pin[19|51|83|115|147] */
    UINT32  ModeSelect20:       1;      /* [20] 0 = Software control mode, 1 = Hardware control mode for pin[20|52|84|116|148] */
    UINT32  ModeSelect21:       1;      /* [21] 0 = Software control mode, 1 = Hardware control mode for pin[21|53|85|117|149] */
    UINT32  ModeSelect22:       1;      /* [22] 0 = Software control mode, 1 = Hardware control mode for pin[22|54|86|118|150] */
    UINT32  ModeSelect23:       1;      /* [23] 0 = Software control mode, 1 = Hardware control mode for pin[23|55|87|119|151] */
    UINT32  ModeSelect24:       1;      /* [24] 0 = Software control mode, 1 = Hardware control mode for pin[24|56|88|120|152] */
    UINT32  ModeSelect25:       1;      /* [25] 0 = Software control mode, 1 = Hardware control mode for pin[25|57|89|121|153] */
    UINT32  ModeSelect26:       1;      /* [26] 0 = Software control mode, 1 = Hardware control mode for pin[26|58|90|122|154] */
    UINT32  ModeSelect27:       1;      /* [27] 0 = Software control mode, 1 = Hardware control mode for pin[27|59|91|123|155] */
    UINT32  ModeSelect28:       1;      /* [28] 0 = Software control mode, 1 = Hardware control mode for pin[28|60|92|124|156] */
    UINT32  ModeSelect29:       1;      /* [29] 0 = Software control mode, 1 = Hardware control mode for pin[29|61|93|125|157] */
    UINT32  ModeSelect30:       1;      /* [30] 0 = Software control mode, 1 = Hardware control mode for pin[30|62|94|126|158] */
    UINT32  ModeSelect31:       1;      /* [31] 0 = Software control mode, 1 = Hardware control mode for pin[31|63|95|127|159] */
} B8_GPIO_AFSEL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 GPIO All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_GPIO_DATA_REG_s   PinLevel;       /* 0x000: Pin Data Register */
    volatile B8_GPIO_DIR_REG_s    PinDirection;   /* 0x004: Pin Direction Register */
    volatile B8_GPIO_IS_REG_s     IntType;        /* 0x008: Interrupt Sensitivity Control Register */
    volatile B8_GPIO_IBE_REG_s    IntDualEdge;    /* 0x00C: Interrupt on both Rising and Falling Data Edge Register */
    volatile B8_GPIO_IEV_REG_s    IntPolarity;    /* 0x010: Interrupt on Data Low/High Level (Falling/Rising Edge) Register */
    volatile B8_GPIO_IE_REG_s     IntEnable;      /* 0x014: Interrupt Enable (Unmask) Control Register */
    volatile B8_GPIO_AFSEL_REG_s  ModeSelect;     /* 0x018: Mode control selection Register */
    volatile B8_GPIO_RIS_REG_s    IntStatusRaw;   /* 0x01C: Raw Interrupt Status Register */
    volatile B8_GPIO_MIS_REG_s    IntStatus;      /* 0x020: Masked Interrupt Status Register */
    volatile B8_GPIO_IC_REG_s     IntClear;       /* 0x024: Interrupt Clear Register */
    volatile B8_GPIO_MASK_REG_s   PinMask;        /* 0x028: Pin Data Enable Register */
    volatile B8_GPIO_ENABLE_REG_s Enable;         /* 0x02C: Enable Register */
} B8_GPIO_REG_s;

#endif /* AMBA_B8_REG_GPIO_H */
