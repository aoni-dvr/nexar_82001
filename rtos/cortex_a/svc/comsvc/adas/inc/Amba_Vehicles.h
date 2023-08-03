/**
 * @file Amba_Vehicles.h
 *
 * Copyright (c) 2018 Ambarella International LP
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
 *
 *  @details Amba_Vehicles
 *
 */

#ifndef AMBA_VEHICLES_H
#define AMBA_VEHICLES_H
#define UNKNOW_HAND_DRIVE      (0)
#define LEFT_HAND_DRIVE        (1)
#define RIGHT_HAND_DRIVE       (2)

#define GEARBOX_TYPE_UNKNOWN   (0)
#define GEARBOX_TYPE_MT        (1) /**< Manual transmission */
#define GEARBOX_TYPE_AT        (2) /**< Automa tictransmisson */
#define GEARBOX_TYPE_CVT       (3) /**< Continuous variable transmission */
#define GEARBOX_TYPE_DCT       (4) /**< Double clutch transmission */
#define GEARBOX_TYPE_AMT       (5) /**< Auto-clutch manual transmission */

#define DRIVING_MODE_UNKNOWN   (0)
#define DRIVING_MODE_FF        (1) /**< Front Engine Front Drive */
#define DRIVING_MODE_FR        (2) /**< Front Engine Rear Drive */
#define DRIVING_MODE_4WD       (3) /**< Four Wheel Drive */
#define DRIVING_MODE_MR        (4) /**< Midship Engine Rear Drive */

/** Define for max vehicle speed in kph */
#define AMBA_VEHICLE_MAX_SPEED              (256U)

/** Define for SSG profile data number */
#define AMBA_VEHICLE_SSG_DATA_NUM           (128U)

typedef struct {
    UINT32 Length;                                         /**< Car Length, unit:mm*/
    UINT32 Width;                                          /**< Car width, unit:mm*/
    UINT32 Height;                                         /**< Car height, unit:mm*/
} AMBA_VEHICLES_SIZE_s;

typedef struct {
    UINT32 SectionWidth;                                   /**< Section width, unit:mm */
    UINT32 SectionHeight;                                  /**< Section height, unit:mm */
    UINT32 OverallDiameter;                                /**< Overall diameter, unit:mm */
} AMBA_VEHICLES_WHEEL_s;
/**
Define of AMBA_VEHICLES_WHEEL_s
Section Width
 ->      <-
  /------\    - ------------------
  |      |    ^                 ^
  |      |    Section Height
  |      |    v
  |------|    -
  |      |
  |      |
  |======|                   Overall diameter
  |      |
  |      |
  |------|
  |      |
  |      |
  |      |                      v
  \------/                      _
**/

typedef struct {
    AMBA_VEHICLES_SIZE_s Size;
    UINT32 LeftRightHandDrive;                             /**< LEFT/RIGHT_HAND_DRIVE */
    UINT32 GearboxType;                                    /**< GEARBOX_TYPE_MT/AT/CVT/DCT/AMT */
    UINT32 DrivingMode;                                    /**< DRIVING_MODE_/FF/FR/4WD/MR */
    UINT32 WheelBase;                                      /**< Wheel base, unit: mm */
    UINT32 FrontOverhang;                                  /**< Front Overhang, unit: mm */
    UINT32 BackOverhang;                                   /**< Front Overhang, unit: mm */
    UINT32 FrontTrack;                                     /**< Front Track, unit: mm */
    UINT32 RearTrack;                                      /**< Rear Track, unit: mm */
    AMBA_VEHICLES_WHEEL_s FrontWheel;                      /**< Front wheel */
    AMBA_VEHICLES_WHEEL_s RearWheel;                       /**< Rear wheel */
} AMBA_COMPACT_CAR_SPEC_s;

typedef struct {
    UINT32 DataNum;                                        /**< Profile data number */
    DOUBLE LateralAcc[AMBA_VEHICLE_SSG_DATA_NUM];          /**< Lateral accerlation in m/sec^2 */
    DOUBLE SteerAng[AMBA_VEHICLE_SSG_DATA_NUM];            /**< Steering angle in degree */
} AMBA_VEHICLE_SSG_PROFILE_s;

typedef struct {
    AMBA_VEHICLE_SSG_PROFILE_s SSGProfile;                 /**< Self steering gradient profile, steering angle to lateral accerlation */
    DOUBLE SteerGearRatio[AMBA_VEHICLE_MAX_SPEED];         /**< The ratio of steering angle to wheel angle with speed (kph) index, e.g., 360:24 degree, ratio = 15.0 */
    DOUBLE MaxSteerAng;                                    /**< maximum turning angle of steering in degree, e.g., 2.5*360 = 900 deg */
    DOUBLE MaxSteerAngRate;                                /**< maximun steering angle velocity */
    DOUBLE TireFriction;                                   /**< Tire friction */
} AMBA_VEHICLE_PHYSICAL_CTRL_s;

typedef struct {
    DOUBLE DetectionDelay;                                 /**< Time delay by object detection, unit: msec */
    DOUBLE ReconstructionDelay;                            /**< Time delay by surrounding reconstruction, unit: msec */
    DOUBLE OutputDelay;                                    /**< Time delay to output, unit: msec  */
} AMBA_VEHICLE_SYSTEM_DELAY_s;

typedef struct {
    UINT32 Clock;                                          /**< System Clock in KHz, time(ms) = TimeStamp / Clock */
    AMBA_VEHICLE_SYSTEM_DELAY_s Delay;                     /**< Delay of system */
} AMBA_VEHICLE_SYSTEM_SPEC_s;

#endif
