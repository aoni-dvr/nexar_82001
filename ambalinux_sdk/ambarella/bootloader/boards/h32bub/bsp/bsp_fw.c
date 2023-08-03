/*
 * History:
 *	Author: Cao Rongrong <rrcao@ambarella.com>
 *
 * Copyright (c) 2017 Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <bldfunc.h>

static flpart_table_t gPtb;
static int ptbOk;

static void update_mac_info(u8* oldMac, u8* newMac)
{
	if (oldMac && newMac) {
		if ((newMac[0]|newMac[1]|newMac[2]|newMac[3]|newMac[4]|newMac[5]) != 0) {
			memcpy(oldMac, newMac, 6);
		}
	}
}

static void update_ptb_info(flpart_table_t* oldPtb, flpart_table_t* newPtb)
{
	if (oldPtb && newPtb) {
		if (strlen(newPtb->dev.sn) > 0) {
			memset(oldPtb->dev.sn, 0, sizeof(oldPtb->dev.sn));
			strncpy(oldPtb->dev.sn, (const char*)newPtb->dev.sn, strlen(newPtb->dev.sn));
		}
		update_mac_info(oldPtb->dev.eth[0].mac, newPtb->dev.eth[0].mac);
		update_mac_info(oldPtb->dev.eth[1].mac, newPtb->dev.eth[1].mac);
		update_mac_info(oldPtb->dev.wifi[0].mac, newPtb->dev.wifi[0].mac);
		update_mac_info(oldPtb->dev.wifi[1].mac, newPtb->dev.wifi[1].mac);
		update_mac_info(oldPtb->dev.usb_eth[0].mac, newPtb->dev.usb_eth[0].mac);
		update_mac_info(oldPtb->dev.usb_eth[1].mac, newPtb->dev.usb_eth[1].mac);
		oldPtb->dev.auto_boot = 1;
		oldPtb->dev.usbdl_mode = 0;
	}
}

/* ==========================================================================*/
void hook_pre_memfwprog(fwprog_cmd_t *fwprog_cmd)
{
	if (fwprog_cmd) {
		memset(&gPtb, 0, sizeof(gPtb));
		ptbOk = flprog_get_part_table(&gPtb);
		if (ptbOk < 0) {
			printf("PTB load error!\n");
			memset(&gPtb, 0, sizeof(gPtb));
		}
		switch(fwprog_cmd->cmd[0]) {
			 case 0xb007: {
				u32 i = 0;
				for (i = PART_BST; i < HAS_IMG_PARTS; ++ i) {
					flprog_erase_partition(i);
				}
			 }break;
			 case 0x0005: {
				u32 i = 0;
				for (i = PART_PBA; i < HAS_IMG_PARTS; ++ i) {
					flprog_erase_partition(i);
				}
			 }break;
			 case 0x0A11: {
				flprog_erase_partition(HAS_IMG_PARTS);
			 }break;
#if defined(NAND_SUPPORT_BBT)
			 case 0x0BB7: {
				nand_erase_bbt();
			 }break;
#endif
			 default:break;
		}
	}
}

void hook_post_memfwprog(fwprog_cmd_t *fwprog_cmd)
{
	if (fwprog_cmd) {
		flpart_table_t ptb;
		int retval = flprog_get_part_table(&ptb);
		if (retval < 0) {
			printf("PTB load error!\n");
		} else {
			u8* verStr = fwprog_cmd->data + sizeof(flpart_table_t);
			u32 magic = verStr[0]<<24 | verStr[1]<<16 | verStr[2]<<8 | verStr[3];
			u32 ver = verStr[4]<<24 | verStr[5]<<16 | verStr[6]<<8 | verStr[7];
			if ((magic == (('A'<<24) | ('m'<<16) | ('b'<<8) | 'a')) && (ver > 0x03030100)) {
				flpart_table_t* ptbData = (flpart_table_t*)fwprog_cmd->data;
				update_ptb_info(&gPtb, ptbData);
			} else {
				if (strlen((const char*)fwprog_cmd->data) > 0) {
					int len = strlen((const char*)fwprog_cmd->data) > sizeof(gPtb.dev.sn) ?
							sizeof(gPtb.dev.sn) : strlen((const char*)fwprog_cmd->data);
					memset(gPtb.dev.sn, 0, sizeof(gPtb.dev.sn));
					strncpy(gPtb.dev.sn, (const char*)fwprog_cmd->data, len);
				}
			}
			update_ptb_info(&ptb, &gPtb);
			ptbOk = flprog_set_part_table(&ptb);
			if (ptbOk < 0) {
				printf("PTB save error!\n");
			}
		}
	}
	if (!fwprog_cmd || (fwprog_cmd && (fwprog_cmd->cmd[7] != MEMFWPROG_CMD_VERIFY))) {
		rct_reset_chip();
	}
}
