/*
 * Target Wake Time Module which is responsible for acting as an
 * interface between the userspace and firmware.
 *
 * Portions of this code are copyright (c) 2022 Cypress Semiconductor Corporation,
 * an Infineon company
 *
 * This program is the proprietary software of infineon and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and infineon (an "Authorized License").
 * Except as set forth in an Authorized License, infineon grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and infineon expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY INFINEON AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of infineon, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of infineon
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND INFINEON MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  INFINEON SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * INFINEON OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF INFINEON HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 *
 * <<Infineon-WL-IPTag/Open:>>
 *
 * $Id$
 */

#ifdef WL11AX

#include "wl_twt.h"

/*
 * Wake Duration derivation from Nominal Minimum Wake Duration
 */
static inline uint32
wl_twt_min_twt_to_wake_dur(uint8 min_twt, uint8 min_twt_unit)
{
	uint32 wake_dur;

	if (min_twt_unit == 1) {
		/*
		 * If min_twt_unit is 1, then min_twt is
		 * in units of TUs (i.e) 102400 usecs.
		 */
		wake_dur = (uint32)min_twt * 102400;
	} else if (min_twt_unit == 0) {
		/*
		 * If min_twt_unit is 0, then min_twt is
		 * in units of 256 usecs.
		 */
		wake_dur = (uint32)min_twt * 256;
	} else {
		/* Invalid min_twt */
		wake_dur = 0;
	}

	return wake_dur;
}

/*
 * Wake Interval derivation from Wake Interval Mantissa & Exponent
 */
static inline uint32
wl_twt_float_to_uint32(uint8 exponent, uint16 mantissa)
{
	return (uint32)mantissa << exponent;
}

int wl_twt_setup(struct wireless_dev *wdev, struct wl_twt wl_twt)
{
	wl_twt_setup_t val;
	s32 bw;
	u8 mybuf[WLC_IOCTL_SMLEN] = {0};
	u8 resp_buf[WLC_IOCTL_SMLEN] = {0};
	uint8 *rem = mybuf;
	uint16 rem_len = sizeof(mybuf);

	bzero(&val, sizeof(val));
	val.version = WL_TWT_SETUP_VER;
	val.length = sizeof(val.version) + sizeof(val.length);

	/* Default values, Override Below */
	val.desc.flow_flags = 0x0;
	val.desc.wake_dur = 0xFFFFFFFF;
	val.desc.wake_int = 0xFFFFFFFF;
	val.desc.wake_int_max = 0xFFFFFFFF;

	/* Setup command */
	val.desc.setup_cmd = wl_twt.setup_cmd;

	/* Flow flags */
	val.desc.flow_flags |= ((wl_twt.negotiation_type & 0x02) >> 1 ?
				WL_TWT_FLOW_FLAG_BROADCAST : 0);
	val.desc.flow_flags |= (wl_twt.implicit ? WL_TWT_FLOW_FLAG_IMPLICIT : 0);
	val.desc.flow_flags |= (wl_twt.flow_type ? WL_TWT_FLOW_FLAG_UNANNOUNCED : 0);
	val.desc.flow_flags |= (wl_twt.trigger ? WL_TWT_FLOW_FLAG_TRIGGER : 0);
	val.desc.flow_flags |= ((wl_twt.negotiation_type & 0x01) ?
				WL_TWT_FLOW_FLAG_WAKE_TBTT_NEGO : 0);
	val.desc.flow_flags |= (wl_twt.requestor ? WL_TWT_FLOW_FLAG_REQUEST : 0);
	val.desc.flow_flags |= (wl_twt.protection ? WL_TWT_FLOW_FLAG_PROTECT : 0);

	if (wl_twt.flow_id) {
		/* Flow ID */
		val.desc.flow_id = wl_twt.flow_id;
	} else if (wl_twt.bcast_twt_id) {
		/* Broadcast TWT ID */
		val.desc.bid = wl_twt.bcast_twt_id;
	} else {
		/* Let the FW choose the Flow ID, Broadcast TWT ID */
		val.desc.flow_id = 0xFF;
		val.desc.bid = 0xFF;
	}

	if (wl_twt.twt) {
		/* Target Wake Time parameter */
		val.desc.wake_time_h = htod32((uint32)(wl_twt.twt >> 32));
		val.desc.wake_time_l = htod32((uint32)(wl_twt.twt));
		val.desc.wake_type = WL_TWT_TIME_TYPE_BSS;
	} else if (wl_twt.twt_offset) {
		/* Target Wake Time offset parameter */
		val.desc.wake_time_h = htod32((uint32)(wl_twt.twt_offset >> 32));
		val.desc.wake_time_l = htod32((uint32)(wl_twt.twt_offset));
		val.desc.wake_type = WL_TWT_TIME_TYPE_OFFSET;
	} else {
		/* Let the FW choose the Target Wake Time */
		val.desc.wake_time_h = 0x0;
		val.desc.wake_time_l = 0x0;
		val.desc.wake_type = WL_TWT_TIME_TYPE_AUTO;
	}

	/* Wake Duration or Service Period */
	val.desc.wake_dur = htod32(wl_twt_min_twt_to_wake_dur(wl_twt.min_twt,
							      wl_twt.min_twt_unit));

	/* Wake Interval or Service Interval */
	val.desc.wake_int = htod32(wl_twt_float_to_uint32(wl_twt.exponent,
							  wl_twt.mantissa));

	/* TWT Negotiation_type */
	val.desc.negotiation_type = (uint8)wl_twt.negotiation_type;

	bw = bcm_pack_xtlv_entry(&rem, &rem_len, WL_TWT_CMD_SETUP, sizeof(val),
				 (uint8 *)&val, BCM_XTLV_OPTION_ALIGN32);
	if (bw != BCME_OK) {
		goto exit;
	}

	bw = wldev_iovar_setbuf(wdev_to_ndev(wdev), "twt", mybuf,
				sizeof(mybuf) - rem_len, resp_buf,
				WLC_IOCTL_SMLEN, NULL);
	if (bw < 0) {
		WL_ERR(("TWT config set failed. ret:%d\n", bw));
	} else {
		WL_INFORM(("TWT config setup succeeded\n"
			   "Setup command	: %u\n"
			   "Flow flags		: 0x %02x\n"
			   "Flow ID		: %u\n"
			   "Broadcast TWT ID	: %u\n"
			   "Wake Time H,L	: 0x %08x %08x\n"
			   "Wake Type		: %u\n"
			   "Wake Dururation	: %u usecs\n"
			   "Wake Interval	: %u usecs\n"
			   "Negotiation type	: %u\n",
			   val.desc.setup_cmd,
			   val.desc.flow_flags,
			   val.desc.flow_id,
			   val.desc.bid,
			   val.desc.wake_time_h,
			   val.desc.wake_time_l,
			   val.desc.wake_type,
			   val.desc.wake_dur,
			   val.desc.wake_int,
			   val.desc.negotiation_type));
	}
exit:
	return bw;
}

int wl_twt_teardown(struct wireless_dev *wdev, struct wl_twt wl_twt)
{
	wl_twt_teardown_t val;
	s32 bw;
	u8 mybuf[WLC_IOCTL_SMLEN] = {0};
	u8 resp_buf[WLC_IOCTL_SMLEN] = {0};
	uint8 *rem = mybuf;
	uint16 rem_len = sizeof(mybuf);

	bzero(&val, sizeof(val));
	val.version = WL_TWT_TEARDOWN_VER;
	val.length = sizeof(val.version) + sizeof(val.length);

	if (wl_twt.flow_id) {
		/* Flow ID */
		val.teardesc.flow_id = wl_twt.flow_id;
	} else if (wl_twt.bcast_twt_id) {
		/* Broadcast TWT ID */
		val.teardesc.bid = wl_twt.bcast_twt_id;
	} else {
		/* Let the FW choose the Flow ID, Broadcast TWT */
		val.teardesc.flow_id = 0xFF;
		val.teardesc.bid = 0xFF;
	}

	/* TWT Negotiation_type */
	val.teardesc.negotiation_type = (uint8)wl_twt.negotiation_type;

	/* Teardown all Negotiated TWT */
	val.teardesc.alltwt = wl_twt.teardown_all_twt;

	bw = bcm_pack_xtlv_entry(&rem, &rem_len, WL_TWT_CMD_TEARDOWN,
		sizeof(val), (uint8 *)&val, BCM_XTLV_OPTION_ALIGN32);
	if (bw != BCME_OK) {
		goto exit;
	}

	bw = wldev_iovar_setbuf(wdev_to_ndev(wdev), "twt",
		mybuf, sizeof(mybuf) - rem_len, resp_buf, WLC_IOCTL_SMLEN, NULL);
	if (bw < 0) {
		WL_ERR(("TWT teardown failed. ret:%d\n", bw));
	} else {
		WL_INFORM(("TWT teardown succeeded\n"
			   "Flow ID		: %u\n"
			   "Broadcast TWT ID	: %u\n"
			   "Negotiation type	: %u\n"
			   "Teardown all TWT	: %u\n",
			   val.teardesc.flow_id,
			   val.teardesc.bid,
			   val.teardesc.negotiation_type,
			   val.teardesc.alltwt));
	}
exit:
	return bw;
}

int wl_twt_oper(struct wireless_dev *wdev, struct wl_twt wl_twt)
{
	int ret = -1;

	switch (wl_twt.twt_oper) {
		case IFX_TWT_OPER_SETUP:
			ret = wl_twt_setup(wdev, wl_twt);
			break;
		case IFX_TWT_OPER_TEARDOWN:
			ret = wl_twt_teardown(wdev, wl_twt);
			break;
		default:
			WL_ERR(("Requested TWT operation (%d) is not supported",
				wl_twt.twt_oper));
			ret = -EINVAL;
			goto exit;
	}
exit:
	return ret;
}

#endif /* WL11AX */
