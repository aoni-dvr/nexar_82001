/*
 * drivers/net/phy/marvell_88q6113.c
 *
 * Driver for Marvell 88Q6113 PHYs
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/hwmon.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>
#include <linux/marvell_phy.h>
#include <linux/of.h>

#include <linux/io.h>
#include <asm/irq.h>
#include <linux/uaccess.h>

#define	RGMII_RX_TIMING		(1 << 15)
#define	RGMII_TX_TIMING		(1 << 14)

#define MARVELL_PORT_STATUS_REG		0x0
#define MARVELL_PHYSICAL_CTRL_REG	0x1

#define MARVELL_RGMII_PORT7	0x7
#define MARVELL_RGMII_PORT8	0x8
#define MARVELL_GLB2_PORT	0x1C


#define MARVELL_GLB2_SMI_CTRL	0x18
#define MARVELL_GLB2_SMI_DATA	0x19

#define SMI_CTRL_READ		((1 << 15) | (1 << 12) | (2 << 10))
#define SMI_CTRL_WRITE		((1 << 15) | (1 << 12) | (1 << 10))

MODULE_DESCRIPTION("Marvell 88Q6113 PHY driver");
MODULE_AUTHOR("Jon");
MODULE_LICENSE("GPL");

#if 0
static int m88q6113_glb2_smi_busy(struct phy_device *phydev)
{
	unsigned int value;
	unsigned int timeout = 1000;

	do {
		value = mdiobus_read(phydev->mdio.bus, MARVELL_GLB2_PORT,
				MARVELL_GLB2_SMI_CTRL);

		if (!(value & (1 << 15)))
			break;
		usleep_range(1000, 2000);

	} while(--timeout);

	if (!timeout)
		return -ETIMEDOUT;

	return 0;
}

static u16 m88q6113_glb2_smi_read(struct phy_device *phydev, unsigned int devaddr,
		unsigned int reg)
{
	u16 ctrl = SMI_CTRL_READ;

	ctrl |= ((devaddr & 0x1F) << 5);
	ctrl |= (reg & 0x1F);

	if (m88q6113_glb2_smi_busy(phydev))
		return 0;

	mdiobus_write(phydev->mdio.bus, MARVELL_GLB2_PORT, MARVELL_GLB2_SMI_CTRL, ctrl);

	if (m88q6113_glb2_smi_busy(phydev))
		return 0;

	return mdiobus_read(phydev->mdio.bus, MARVELL_GLB2_PORT, MARVELL_GLB2_SMI_DATA);
}
static int m88q6113_glb2_smi_write(struct phy_device *phydev, unsigned int devaddr,
		unsigned int reg, u16 data)
{
	u16 ctrl = SMI_CTRL_WRITE;

	ctrl |= ((devaddr & 0x1F) << 5);
	ctrl |= (reg & 0x1F);

	if (m88q6113_glb2_smi_busy(phydev))
		return 0;

	mdiobus_write(phydev->mdio.bus, MARVELL_GLB2_PORT, MARVELL_GLB2_SMI_DATA, data);
	mdiobus_write(phydev->mdio.bus, MARVELL_GLB2_PORT, MARVELL_GLB2_SMI_CTRL, ctrl);

	return 0;

}
#endif
static int m88q6113_probe(struct phy_device *phydev)
{
	return 0;
}

static int m88q6113_config_init_dummy(struct phy_device *phydev)
{
    return 0;
}

static int m88q6113_config_init(struct phy_device *phydev)
{
	unsigned int value;

	/* force 1000Mbps FULL duplex */
	mdiobus_write(phydev->mdio.bus, MARVELL_RGMII_PORT7,
			MARVELL_PHYSICAL_CTRL_REG, 0x203E);

	mdiobus_write(phydev->mdio.bus, MARVELL_RGMII_PORT8,
			MARVELL_PHYSICAL_CTRL_REG, 0x203E);

	/* adjust RGMII timing */
	value = mdiobus_read(phydev->mdio.bus, MARVELL_RGMII_PORT7, MARVELL_PHYSICAL_CTRL_REG);
	//value |= RGMII_RX_TIMING;	// NG if enabled
	value |= RGMII_TX_TIMING;	// That is OK.
	mdiobus_write(phydev->mdio.bus, MARVELL_RGMII_PORT7, MARVELL_PHYSICAL_CTRL_REG, value);

	value = mdiobus_read(phydev->mdio.bus, MARVELL_RGMII_PORT8, MARVELL_PHYSICAL_CTRL_REG);
	//value |= RGMII_RX_TIMING;	// NG if enabled
	value |= RGMII_TX_TIMING;	// That is OK.
	mdiobus_write(phydev->mdio.bus, MARVELL_RGMII_PORT8, MARVELL_PHYSICAL_CTRL_REG, value);

#if 0
	printk("Marvell Faster Link up ...\n");

	/* Faster Link up */
	m88q6113_glb2_smi_write(phydev, phydev->mdio.addr, 0x1D, 0x1B);

	value = m88q6113_glb2_smi_read(phydev, phydev->mdio.addr, 0x1E);
	value |= (1 << 1);
	m88q6113_glb2_smi_write(phydev, phydev->mdio.addr, 0x1E, value);

	value = m88q6113_glb2_smi_read(phydev, phydev->mdio.addr, 0x1C);
	value &= ~(1 << 7);
	m88q6113_glb2_smi_write(phydev, phydev->mdio.addr, 0x1C, value);

	m88q6113_glb2_smi_write(phydev, phydev->mdio.addr, 0x0E, 0x003c);
	m88q6113_glb2_smi_write(phydev, phydev->mdio.addr, 0x0D, 0x0007);
	m88q6113_glb2_smi_write(phydev, phydev->mdio.addr, 0x0D, 0x4007);
	m88q6113_glb2_smi_write(phydev, phydev->mdio.addr, 0x0E, 0x0000);

	value = m88q6113_glb2_smi_read(phydev, phydev->mdio.addr, 0x00);
	value |= (1 << 15);
	m88q6113_glb2_smi_write(phydev, phydev->mdio.addr, 0x00, value);
#endif


	return 0;
}

static int m88q6113_config_aneg_dummy(struct phy_device *phydev)
{ 
    msleep(2000);
    return 0;
}

static int m88q6113_config_aneg(struct phy_device *phydev)
{
	unsigned int value;
    //For AQR port 0 LED: 1E.C430-C432
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0xC430);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA01E);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0x009F);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA41E);

	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0xC431);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA01E);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0x007F);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA41E);

	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0xC432);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA01E);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0x007F);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA41E);

//For AQR port 0 auto-nego: 04.C441
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0xC441);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA004);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xAC04);
	value = mdiobus_read(phydev->mdio.bus, 0x1C, 0x19);

    value |= 0x0008;
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0xC441);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA004);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, value);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA404);
    msleep(500);

    //For AQR port 8 LED: 1E.C430-C432
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0xC430);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA11E);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0x009F);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA51E);

	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0xC431);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA11E);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0x007F);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA51E);

	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0xC432);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA11E);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0x007F);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA51E);

//For AQR port 8 auto-nego: 04.C441
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0xC441);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA104);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xAD04);
	value = mdiobus_read(phydev->mdio.bus, 0x1C, 0x19);
    value |= 0x0008;

	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, 0xC441);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA104);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x19, value);
	mdiobus_write(phydev->mdio.bus, 0x1C, 0x18, 0xA504);
    msleep(500);
#if 0
	int ctrl;

	ctrl = m88q6113_glb2_smi_read(phydev, phydev->mdio.addr, 0x00);
	ctrl |= BMCR_ANENABLE | BMCR_ANRESTART;
	m88q6113_glb2_smi_write(phydev, phydev->mdio.addr, 0x00, ctrl);
#endif

	return 0;
}

static int m88q6113_read_status(struct phy_device *phydev)
{
	phydev->duplex = DUPLEX_FULL;
	phydev->speed = SPEED_1000;
	phydev->pause = 0;
	phydev->asym_pause = 0;

	phydev->link = 1;

	return 0;
}
static int m88q6113_config_intr(struct phy_device *phydev)
{
	return 0;
}

static int m88q6113_aneg_done(struct phy_device *phydev)
{
#if 0
	unsigned int value;

	value = m88q6113_glb2_smi_read(phydev, phydev->mdio.addr, 0x01);

	return value & (1 << 5);
#else
	return 1;
#endif
}

static struct phy_driver marvell_88q6113_drivers[] = {
	{
		.phy_id = 0x0000f132,
		.phy_id_mask = MARVELL_PHY_ID_MASK,
		.name = "Marvell 88Q6113 SOM1",
		.features = PHY_GBIT_FEATURES,
		.probe = m88q6113_probe,
		.config_init = &m88q6113_config_init,
		.config_aneg = &m88q6113_config_aneg,
		.read_status = &m88q6113_read_status,
		.config_intr = &m88q6113_config_intr,
		.aneg_done = &m88q6113_aneg_done,
	},
	{
		.phy_id = 0x0000ffff,
		.phy_id_mask = MARVELL_PHY_ID_MASK,
		.name = "Marvell 88Q6113 SOM2",
		.features = PHY_GBIT_FEATURES,
		.probe = m88q6113_probe,
		.config_init = &m88q6113_config_init_dummy,
		.config_aneg = &m88q6113_config_aneg_dummy,
		.read_status = &m88q6113_read_status,
		.config_intr = &m88q6113_config_intr,
		.aneg_done = &m88q6113_aneg_done,
	},

};

module_phy_driver(marvell_88q6113_drivers);

static struct mdio_device_id __maybe_unused marvell_88q6113_tbl[] = {
	{ 0x0000f132, 0xfffffff0 },
	{ 0x0000ffff, 0xfffffff0 },
	{ }
};

MODULE_DEVICE_TABLE(mdio, marvell_88q6113_tbl);