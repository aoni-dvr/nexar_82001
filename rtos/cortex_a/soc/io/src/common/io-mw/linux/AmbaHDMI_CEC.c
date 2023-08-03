// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
 *  @file AmbaHDMI_CEC.c
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
 *
 *  @details HDMI Consumer Electronics Control (CEC) APIs
 *
 */


#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaHDMI.h"
#include "AmbaHDMI_Ctrl.h"
//#include "AmbaRTSL_HDMI.h"


/**
 *  AmbaHDMI_CecDrvEntry - CEC device driver initializations
 *  @return error code
 */
UINT32 AmbaHDMI_CecDrvEntry(void)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    return RetVal;
}

/**
 *  AmbaHDMI_CecEnable - Enable CEC module
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] LogicalAddr Logical address of the hdmi port
 *  @return error code
 */
UINT32 AmbaHDMI_CecEnable(UINT32 HdmiPort, UINT32 LogicalAddr)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    (void) HdmiPort;
    (void) LogicalAddr;
    return RetVal;
}

/**
 *  AmbaHDMI_CecDisable - Disable CEC module
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
UINT32 AmbaHDMI_CecDisable(UINT32 HdmiPort)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    (void) HdmiPort;
    return RetVal;
}

/**
 *  AmbaHDMI_CecWrite - Transmit one CEC message
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] pMsg CEC message
 *  @param[in] MsgSize Size of the CEC message (in Bytes)
 *  @return error code
 */
UINT32 AmbaHDMI_CecWrite(UINT32 HdmiPort, const UINT8 *pMsg, UINT32 MsgSize)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    (void) HdmiPort;
    (void) pMsg;
    (void) MsgSize;
    return RetVal;
}

/**
 *  AmbaHDMI_CecRead - Receive one CEC message
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] pMsg CEC message
 *  @param[out] pMsgSize Size of the CEC message (in Bytes)
 *  @return error code
 */
UINT32 AmbaHDMI_CecRead(UINT32 HdmiPort, UINT8 *pMsg, UINT32 *pMsgSize)
{
    UINT32 RetVal = HDMI_ERR_NONE;
    (void) HdmiPort;
    (void) pMsg;
    (void) pMsgSize;
    return RetVal;
}

