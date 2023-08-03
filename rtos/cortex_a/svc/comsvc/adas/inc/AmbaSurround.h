/**
 *  @file AmbaSurround.h
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
 *  @details Definitions for Ambarella Surround Function
 *
 */

#ifndef AMBA_SURROUND_H
#define AMBA_SURROUND_H

#include "AmbaTypes.h"
#include "Amba_Vehicles.h"
#include "AmbaAdasErrorCode.h"
#include "AmbaSR_BbxConv.h"

#define  AMBA_SR_MAX_OBJECT_NUM           (150U)                /**< The maximum number of Objects in each captured frame */


#define  AMBA_SR_CANBUS_TURN_LIGHT_N        (0U)                /**< Non */
#define  AMBA_SR_CANBUS_TURN_LIGHT_L        (1U)                /**< Left */
#define  AMBA_SR_CANBUS_TURN_LIGHT_R        (2U)                /**< Right */
#define  AMBA_SR_CANBUS_TURN_LIGHT_UNDEF    (3U)                /**< UnDefine */


#define  AMBA_SR_CANBUS_GEAR_P              (0U)                /**< P */
#define  AMBA_SR_CANBUS_GEAR_R              (1U)                /**< R */
#define  AMBA_SR_CANBUS_GEAR_N              (2U)                /**< N */
#define  AMBA_SR_CANBUS_GEAR_D              (3U)                /**< D */
#define  AMBA_SR_CANBUS_GEAR_UNDEF          (4U)                /**< UnDefine */


typedef struct {
    UINT8   FlagValidRawData;                                  /**< [Valid = 1, Invalid = 0], This flag is "Valid" when raw CANBus data is available */
    UINT32  CapTS;                                             /**< Capture time */
    UINT8   CANBusTurnLightStatus;                             /**< Turn Light Status */
    UINT8   CANBusGearStatus;                                  /**< Gear Status */
    UINT32  RawSpeed;                                          /**< Raw Speed without unit */
    UINT32  WheelDir;                                          /**< Direction of Wheel */
    UINT32  RawWheelAngle;                                     /**< Raw Wheel Angle without unit */
    UINT32  RawWheelAngleSpeed;                                /**< Raw Wheel Angle Speed without unit */
} AMBA_SR_CANBUS_RAW_DATA_s;


typedef struct {
    UINT8   FlagValidTransferData;                             /**< [Valid = 1, Invalid = 0], This flag is "Valid" when transferred CANBus data is available */
    UINT32  CapTS;                                             /**< Capture time */
    UINT8   CANBusTurnLightStatus;                             /**< Turn Light Status */
    UINT8   CANBusGearStatus;                                  /**< Gear Status */
    DOUBLE  TransferSpeed;                                     /**< Transferred Speed in km/h */
    UINT32  WheelDir;                                          /**< Direction of Wheel */
    DOUBLE  TransferWheelAngle;                                /**< Transferred Wheel Angle in rad (pi) */
    DOUBLE  TransferWheelAngleSpeed;                           /**< Transferred Wheel Angle Speed */
} AMBA_SR_CANBUS_TRANSFER_DATA_s;


typedef struct {
    AMBA_SR_WORLD_3D_OBJECT_s  Object;                         /**< Object Info with Relative Location */
    UINT32  CapTS;                                             /**< Capture time */
    UINT8   FlagValidSpeed;                                    /**< [Valid = 1, Invalid = 0], This flag is "Valid" when acquiring 2 consecutive frames at least */
    UINT8   FlagValidAcc;                                      /**< [Valid = 1, Invalid = 0], This flag is "Valid" when acquiring 3 consecutive frames at least */
    UINT8   FlagValidRelative;                                 /**< [Valid = 1, Invalid = 0], This flag is supposed to be "Valid" unless the relative location in Object Info is unavailable */
    UINT8   FlagValidReal;                                     /**< [Valid = 1, Invalid = 0], This flag is "Valid" when CANBus data is available and vice versa */
    DOUBLE  RelativeSpeed;                                     /**< Relative Speed in km/h */
    DOUBLE  RelativeAcc;                                       /**< Relative Acceleration in km/h^2 */
    DOUBLE  RelativeSpeedDir[2];                               /**< Direction [x,y] of Relative Speed in unit vector */
    DOUBLE  RelativeAccDir[2];                                 /**< Direction [x,y] of Relative Acceleration in unit vector */
    DOUBLE  RealSpeed;                                         /**< Real Speed in km/h */
    DOUBLE  RealAcc;                                           /**< Real Acceleration in km/h^2 */
    DOUBLE  RealSpeedDir[2];                                   /**< Direction [x,y] of Real Speed in unit vector */
    DOUBLE  RealAccDir[2];                                     /**< Direction [x,y] of Real Acceleration in unit vector */
} AMBA_SR_OBJECT_DATA_s;


typedef struct {
    UINT32  TotalNum;                                          /**< Total Number of detected Object in captured frame */
    AMBA_SR_OBJECT_DATA_s  SRObject[AMBA_SR_MAX_OBJECT_NUM];   /**< The Info of each detected Object */
} AMBA_SR_SROBJECT_DATA_s;


typedef struct {
    UINT32  TSClock;                                           /**< Clock Info used to calculate system time from Timetick */
    AMBA_COMPACT_CAR_SPEC_s SelfCarSpec;                       /**< SelfCar Spec */
    AMBA_SR_SROBJECT_DATA_s *pInSRData;                        /**< Pointer to the Ring Buffer that buffers input Object */
    AMBA_SR_CANBUS_TRANSFER_DATA_s *pInSelfData;               /**< Pointer to the Ring Buffer that buffers input transferred CANBus */
    UINT32  DataRingSz;                                        /**< Ring Buffer Size */
    UINT32  Resv[8];                                           /**< Reserved for further usage */
} AMBA_SR_OBJ_CFG_s;

/**
* Get default configuration of object reconstruct function.
* @param [out] config: Configuration of object reconstruct function.
* @return ErrorCode
*/
UINT32 AmbaSR_ObjGetDefaultCfg(AMBA_SR_OBJ_CFG_s *config);

/**
* Set configuration of object reconstruct function.
* @param [in] config: Configuration of object reconstruct function.
* @return ErrorCode
*/
UINT32 AmbaSR_ObjSetConfig (const AMBA_SR_OBJ_CFG_s *config);

/**
* Get the detailed information of detected objects, such as relative 3D position, relative speed and relative acceleration.
* @param [in] DataWp: Location of current frame's object data in ring buffer.
* @param [in] pCANBusData: Current frame's CANBus data.
* @param [out] pOutSRData: The detailed information of detected objects.
* @return ErrorCode
*/
UINT32 AmbaSR_ObjReconstruct(const UINT32 DataWp, const AMBA_SR_CANBUS_TRANSFER_DATA_s *pCANBusData, AMBA_SR_SROBJECT_DATA_s *pOutSRData);

#endif //AMBA_SURROUND_H
