/**
 *  @file AmbaReg_MISC.h
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
 *  @details Definitions & Constants for RTC (Real-Time Clock) Controller Registers
 *
 */

#ifndef AMBA_REG_MISC_H
#define AMBA_REG_MISC_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * MISC: PWC Sequance Signal Delay Control Register
 */
typedef struct {
    UINT32  Delay:              8;      /* [7:0] PWC Sequence. Used to set delay time(ms) */
    UINT32  Reserved:           24;     /* [31:8] Reserved */
} AMBA_PWC_PSEQ_DELAY_REG_s;

/*
 * MISC: Status Register
 */
typedef struct {
    UINT32  RtcClock:           1;      /* [0] RTC clock value */
    UINT32  Reserved:           1;      /* [1] Reserved */
    UINT32  AlarmWakeUp:        1;      /* [2] RTC alarm wake up state */
    UINT32  PwcWakeUp:          1;      /* [3] RTC wake state with the PWC_WKUP pin */
    UINT32  Reserved1:          28;     /* [31:4] Reserved */
} AMBA_RTC_STATUS_REG_s;

/*
 * MISC: Reset Register
 */
typedef struct {
    UINT32  Reset:              1;      /* [0] Reset the RTC status. 1 = clear the RTC status */
    UINT32  Reserved:           31;     /* [31:1] Reserved */
} AMBA_RTC_RESET_REG_s;

/*
 * MISC: Extra Wake Up Status Register
 */
typedef struct {
    UINT32  PwcWakeUp1:         1;      /* [0] PWC wake state with the PWC_WKUP0 pin */
    UINT32  PwcWakeUp2:         1;      /* [1] PWC wake state with the PWC_WKUP1 pin */
    UINT32  PwcWakeUp3:         1;      /* [2] Reserve */
    UINT32  Reserved:           29;     /* [31:3] Reserved */
} AMBA_PWC_WAKE_UP_REG_s;

/*
 * MISC: PWC Alert Register
 */
typedef struct {
    UINT32  Alert:              1;      /* [0] PWC is forced off by pressing PWC_WKUP longer than 16s. */
    UINT32  Reserved:           31;     /* [31:1] Reserved */
} AMBA_PWC_ALERT_REG_s;

/*
 * MISC: PWC Low Battery Signal Register
 */
typedef struct {
    UINT32  LowBattery:         1;      /* [0] This bit is asserted when the battery level is low */
    UINT32  Reserved:           31;     /* [31:1] Reserved */
} AMBA_PWC_LOW_BATTERY_REG_s;

/*
 * MISC: PWC Status Read Register
 */
typedef struct {
    UINT32  Scratchpad0:        1;      /* [0] Content will be retained if RTC module is kept powered on. */
    UINT32  Scratchpad1:        1;      /* [1] Content will be retained if RTC module is kept powered on. */
    UINT32  ForcePseq3High:     1;      /* [2] 1 = Force PSEQ3 pin keep HIGH */
    UINT32  EnableAlarmWakeup:  1;      /* [3] 1 = Enable Wake-Up by alarm */
    UINT32  Scratchpad2:        1;      /* [4] Content will be retained if RTC module is kept powered on. */
    UINT32  Scratchpad3:        1;      /* [5] Content will be retained if RTC module is kept powered on. */
    UINT32  Scratchpad4:        1;      /* [6] Content will be retained if RTC module is kept powered on. */
    UINT32  Scratchpad5:        1;      /* [7] Content will be retained if RTC module is kept powered on. */
    UINT32  Reserved:           24;     /* [31:8] Reserved */
} AMBA_PWC_STATUS_REG_s;

/*
 * MISC: GPIO Pull-Up/Pull-Down Enable Register
 */
typedef struct {
    UINT32  Enable0:            1;      /* [0] 1 = Enable pull-up/pull-down function for GPIO[0|32|64|96|128|160|192] */
    UINT32  Enable1:            1;      /* [1] 1 = Enable pull-up/pull-down function for GPIO[1|33|65|97|129|161|193] */
    UINT32  Enable2:            1;      /* [2] 1 = Enable pull-up/pull-down function for GPIO[2|34|66|98|130|162|194] */
    UINT32  Enable3:            1;      /* [3] 1 = Enable pull-up/pull-down function for GPIO[3|35|67|99|131|163|195] */
    UINT32  Enable4:            1;      /* [4] 1 = Enable pull-up/pull-down function for GPIO[4|36|68|100|132|164|196] */
    UINT32  Enable5:            1;      /* [5] 1 = Enable pull-up/pull-down function for GPIO[5|37|69|101|133|165|197] */
    UINT32  Enable6:            1;      /* [6] 1 = Enable pull-up/pull-down function for GPIO[6|38|70|102|134|166|198] */
    UINT32  Enable7:            1;      /* [7] 1 = Enable pull-up/pull-down function for GPIO[7|39|71|103|135|167|199] */
    UINT32  Enable8:            1;      /* [8] 1 = Enable pull-up/pull-down function for GPIO[8|40|72|104|136|168|200] */
    UINT32  Enable9:            1;      /* [9] 1 = Enable pull-up/pull-down function for GPIO[9|41|73|105|137|169|201] */
    UINT32  Enable10:           1;      /* [10] 1 = Enable pull-up/pull-down function for GPIO[10|42|74|106|138|170|202] */
    UINT32  Enable11:           1;      /* [11] 1 = Enable pull-up/pull-down function for GPIO[11|43|75|107|139|171|203] */
    UINT32  Enable12:           1;      /* [12] 1 = Enable pull-up/pull-down function for GPIO[12|44|76|108|140|172|204] */
    UINT32  Enable13:           1;      /* [13] 1 = Enable pull-up/pull-down function for GPIO[13|45|77|109|141|173|205] */
    UINT32  Enable14:           1;      /* [14] 1 = Enable pull-up/pull-down function for GPIO[14|46|78|110|142|174|206] */
    UINT32  Enable15:           1;      /* [15] 1 = Enable pull-up/pull-down function for GPIO[15|47|79|111|143|175|207] */
    UINT32  Enable16:           1;      /* [16] 1 = Enable pull-up/pull-down function for GPIO[16|48|80|112|144|176|208] */
    UINT32  Enable17:           1;      /* [17] 1 = Enable pull-up/pull-down function for GPIO[17|49|81|113|145|177|209] */
    UINT32  Enable18:           1;      /* [18] 1 = Enable pull-up/pull-down function for GPIO[18|50|82|114|146|178|210] */
    UINT32  Enable19:           1;      /* [19] 1 = Enable pull-up/pull-down function for GPIO[19|51|83|115|147|179|211] */
    UINT32  Enable20:           1;      /* [20] 1 = Enable pull-up/pull-down function for GPIO[20|52|84|116|148|180|212] */
    UINT32  Enable21:           1;      /* [21] 1 = Enable pull-up/pull-down function for GPIO[21|53|85|117|149|181|213] */
    UINT32  Enable22:           1;      /* [22] 1 = Enable pull-up/pull-down function for GPIO[22|54|86|118|150|182|214] */
    UINT32  Enable23:           1;      /* [23] 1 = Enable pull-up/pull-down function for GPIO[23|55|87|119|151|183|215] */
    UINT32  Enable24:           1;      /* [24] 1 = Enable pull-up/pull-down function for GPIO[24|56|88|120|152|184|216] */
    UINT32  Enable25:           1;      /* [25] 1 = Enable pull-up/pull-down function for GPIO[25|57|89|121|153|185|217] */
    UINT32  Enable26:           1;      /* [26] 1 = Enable pull-up/pull-down function for GPIO[26|58|90|122|154|186|218] */
    UINT32  Enable27:           1;      /* [27] 1 = Enable pull-up/pull-down function for GPIO[27|59|91|123|155|187|219] */
    UINT32  Enable28:           1;      /* [28] 1 = Enable pull-up/pull-down function for GPIO[28|60|92|124|156|188|220] */
    UINT32  Enable29:           1;      /* [29] 1 = Enable pull-up/pull-down function for GPIO[29|61|93|125|157|189|221] */
    UINT32  Enable30:           1;      /* [30] 1 = Enable pull-up/pull-down function for GPIO[30|62|94|126|158|190|222] */
    UINT32  Enable31:           1;      /* [31] 1 = Enable pull-up/pull-down function for GPIO[31|63|95|127|159|191|223] */
} AMBA_GPIO_PULL_ENABLE_REG_s;

/*
 * MISC: GPIO Pull-Up/Pull-Down Selection Register
 */
typedef struct {
    UINT32  Direction0:         1;      /* [0] 0 = Pull-down, 1 = Pull-up function for GPIO[0|32|64|96|128|160|192] */
    UINT32  Direction1:         1;      /* [1] 0 = Pull-down, 1 = Pull-up function for GPIO[1|33|65|97|129|161|193] */
    UINT32  Direction2:         1;      /* [2] 0 = Pull-down, 1 = Pull-up function for GPIO[2|34|66|98|130|162|194] */
    UINT32  Direction3:         1;      /* [3] 0 = Pull-down, 1 = Pull-up function for GPIO[3|35|67|99|131|163|195] */
    UINT32  Direction4:         1;      /* [4] 0 = Pull-down, 1 = Pull-up function for GPIO[4|36|68|100|132|164|196] */
    UINT32  Direction5:         1;      /* [5] 0 = Pull-down, 1 = Pull-up function for GPIO[5|37|69|101|133|165|197] */
    UINT32  Direction6:         1;      /* [6] 0 = Pull-down, 1 = Pull-up function for GPIO[6|38|70|102|134|166|198] */
    UINT32  Direction7:         1;      /* [7] 0 = Pull-down, 1 = Pull-up function for GPIO[7|39|71|103|135|167|199] */
    UINT32  Direction8:         1;      /* [8] 0 = Pull-down, 1 = Pull-up function for GPIO[8|40|72|104|136|168|200] */
    UINT32  Direction9:         1;      /* [9] 0 = Pull-down, 1 = Pull-up function for GPIO[9|41|73|105|137|169|201] */
    UINT32  Direction10:        1;      /* [10] 0 = Pull-down, 1 = Pull-up function for GPIO[10|42|74|106|138|170|202] */
    UINT32  Direction11:        1;      /* [11] 0 = Pull-down, 1 = Pull-up function for GPIO[11|43|75|107|139|171|203] */
    UINT32  Direction12:        1;      /* [12] 0 = Pull-down, 1 = Pull-up function for GPIO[12|44|76|108|140|172|204] */
    UINT32  Direction13:        1;      /* [13] 0 = Pull-down, 1 = Pull-up function for GPIO[13|45|77|109|141|173|205] */
    UINT32  Direction14:        1;      /* [14] 0 = Pull-down, 1 = Pull-up function for GPIO[14|46|78|110|142|174|206] */
    UINT32  Direction15:        1;      /* [15] 0 = Pull-down, 1 = Pull-up function for GPIO[15|47|79|111|143|175|207] */
    UINT32  Direction16:        1;      /* [16] 0 = Pull-down, 1 = Pull-up function for GPIO[16|48|80|112|144|176|208] */
    UINT32  Direction17:        1;      /* [17] 0 = Pull-down, 1 = Pull-up function for GPIO[17|49|81|113|145|177|209] */
    UINT32  Direction18:        1;      /* [18] 0 = Pull-down, 1 = Pull-up function for GPIO[18|50|82|114|146|178|210] */
    UINT32  Direction19:        1;      /* [19] 0 = Pull-down, 1 = Pull-up function for GPIO[19|51|83|115|147|179|211] */
    UINT32  Direction20:        1;      /* [20] 0 = Pull-down, 1 = Pull-up function for GPIO[20|52|84|116|148|180|212] */
    UINT32  Direction21:        1;      /* [21] 0 = Pull-down, 1 = Pull-up function for GPIO[21|53|85|117|149|181|213] */
    UINT32  Direction22:        1;      /* [22] 0 = Pull-down, 1 = Pull-up function for GPIO[22|54|86|118|150|182|214] */
    UINT32  Direction23:        1;      /* [23] 0 = Pull-down, 1 = Pull-up function for GPIO[23|55|87|119|151|183|215] */
    UINT32  Direction24:        1;      /* [24] 0 = Pull-down, 1 = Pull-up function for GPIO[24|56|88|120|152|184|216] */
    UINT32  Direction25:        1;      /* [25] 0 = Pull-down, 1 = Pull-up function for GPIO[25|57|89|121|153|185|217] */
    UINT32  Direction26:        1;      /* [26] 0 = Pull-down, 1 = Pull-up function for GPIO[26|58|90|122|154|186|218] */
    UINT32  Direction27:        1;      /* [27] 0 = Pull-down, 1 = Pull-up function for GPIO[27|59|91|123|155|187|219] */
    UINT32  Direction28:        1;      /* [28] 0 = Pull-down, 1 = Pull-up function for GPIO[28|60|92|124|156|188|220] */
    UINT32  Direction29:        1;      /* [29] 0 = Pull-down, 1 = Pull-up function for GPIO[29|61|93|125|157|189|221] */
    UINT32  Direction30:        1;      /* [30] 0 = Pull-down, 1 = Pull-up function for GPIO[30|62|94|126|158|190|222] */
    UINT32  Direction31:        1;      /* [31] 0 = Pull-down, 1 = Pull-up function for GPIO[31|63|95|127|159|191|223] */
} AMBA_GPIO_PULL_SELECT_REG_s;

/*
 * MISC: All Registers
 */
typedef struct {
    volatile UINT32                             Reserved0[8];       /* Reserved */
    volatile AMBA_PWC_PSEQ_DELAY_REG_s          Pseq1Delay;         /* 0x020(RW): PWC Time Interval between PWC_WKUP and PWC_PSEQ1 Voltage Changes */
    volatile AMBA_PWC_PSEQ_DELAY_REG_s          Pseq2Delay;         /* 0x024(RW): PWC Time Interval between PWC_PSEQ1 and PWC_PSEQ2 Voltage Changes */
    volatile AMBA_PWC_PSEQ_DELAY_REG_s          Pseq3Delay;         /* 0x028(RW): PWC Time Interval between PWC_PSEQ2 and PWC_PSEQ3 Voltage Changes */
    volatile UINT32                             AlarmWrite;         /* 0x02C(RW): RTC Alarm Time Programming Register */
    volatile UINT32                             CurTimeWrite;       /* 0x030(RW): RTC Current Time Programming Register */
    volatile UINT32                             CurTimeRead;        /* 0x034(RO): RTC Current Time Value Register */
    volatile UINT32                             AlarmRead;          /* 0x038(RO): RTC Alarm Time Value Register */
    volatile AMBA_RTC_STATUS_REG_s              RtcStatus;          /* 0x03C(RO): RTC Status Register */
    volatile AMBA_RTC_RESET_REG_s               RtcReset;           /* 0x040(RW): RTC Reset Register */
    volatile UINT32                             Reserved1[14];      /* 0x044-0x078: Reserved */
    volatile AMBA_PWC_WAKE_UP_REG_s             ExtraWakeUpStatus;  /* 0x07C(RO): RTC Extra Wake Up Status Register */
    volatile UINT32                             GpioPullEnable0;    /* 0x080(RW): Pull-Up/Pull-Down Enable Registers For GPIO Pins */
    volatile UINT32                             GpioPullEnable1;    /* 0x084(RW): Pull-Up/Pull-Down Enable Registers For GPIO Pins */
    volatile UINT32                             GpioPullEnable2;    /* 0x088(RW): Pull-Up/Pull-Down Enable Registers For GPIO Pins */
    volatile UINT32                             GpioPullEnable3;    /* 0x08C(RW): Pull-Up/Pull-Down Enable Registers For GPIO Pins */
    volatile UINT32                             GpioPullEnable4;    /* 0x090(RW): Pull-Up/Pull-Down Enable Registers For GPIO Pins */
    volatile UINT32                             GpioPullSelect0;    /* 0x094(RW): Pull-Up/Pull-Down Function Select Registers For GPIO Pins */
    volatile UINT32                             GpioPullSelect1;    /* 0x098(RW): Pull-Up/Pull-Down Function Select Registers For GPIO Pins */
    volatile UINT32                             GpioPullSelect2;    /* 0x09C(RW): Pull-Up/Pull-Down Function Select Registers For GPIO Pins */
    volatile UINT32                             GpioPullSelect3;    /* 0x0A0(RW): Pull-Up/Pull-Down Function Select Registers For GPIO Pins */
    volatile UINT32                             GpioPullSelect4;    /* 0x0A4(RW): Pull-Up/Pull-Down Function Select Registers For GPIO Pins */
    volatile AMBA_PWC_ALERT_REG_s               ForcePwcOff;        /* 0x0A8(RO): PWC Forced Off State Register */
    volatile AMBA_PWC_LOW_BATTERY_REG_s         LowBattery;         /* 0x0AC(RO): PWC Low Battery Signal Register */
    volatile UINT32                             Reserved2;          /* 0x0B0: Reserved */
    volatile UINT32                             PwcStatusRead;      /* 0x0B4(RO): PWC Status Read Register */
    volatile UINT32                             Reserved3[2];       /* 0x0B8-0x0BC: Reserved */
    volatile UINT32                             PwcStatusWrite;     /* 0x0C0(RW): PWC Status Set Register */
    volatile UINT32                             Reserved4[3];       /* 0x0C4-0x0CC: Reserved */
    volatile AMBA_PWC_PSEQ_DELAY_REG_s          Pseq4Delay;         /* 0x0D0(RW): PWC Time Interval between PWC_PSEQ3 and PWC_RSTOB Voltage Changes */
    volatile UINT32                             Reserved5[10];      /* 0x0D4-0x0F8: Reserved */
    volatile UINT32                             CurTimerLatch;      /* 0x0FC(RW): RTC Latch Current Timer Value From Write(0x30) to Read(0x34) */
    volatile UINT32                             GpioPullEnable5;    /* 0x100(RW): Pull-Up/Pull-Down Enable Registers For GPIO Pins */
    volatile UINT32                             Reserved6[2];       /* 0x104-0x108: Reserved */
    volatile UINT32                             GpioPullSelect5;    /* 0x10C(RW): Pull-Up/Pull-Down Function Select Registers For GPIO Pins */
} AMBA_MISC_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
#ifdef CONFIG_QNX
extern AMBA_MISC_REG_s *pAmbaMISC_Reg;
extern AMBA_MISC_REG_s *pAmbaPWC_Reg;
#else
extern AMBA_MISC_REG_s *const pAmbaMISC_Reg;
extern AMBA_MISC_REG_s *const pAmbaPWC_Reg;
#endif
#endif /* AMBA_REG_MISC_H */
