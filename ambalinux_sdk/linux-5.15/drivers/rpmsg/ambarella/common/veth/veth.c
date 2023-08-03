/*
 * Author: Tzu-Jung Lee <tjlee@ambarella.com>
 * Copyright (C) 2012-2013, Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>

#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/time.h>
#include <linux/crc16.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/rpmsg.h>

#include <linux/of.h>

#include <soc/ambarella/ambalink/rpmsg_compat.h>


#if 0
/* The feature bitmap for virtio rpmsg */
#define VIRTIO_RPMSG_F_NS       0 /* RP supports name service notifications */
/**
 *    struct rpmsg_hdr - common header for all rpmsg messages
 *    @src: source address
 *    @dst: destination address
 *    @reserved: reserved for future use
 *    @len: length of payload (in bytes)
 *    @flags: message flags
 *    @data: @len bytes of message payload data
 *
 *    Every message sent(/received) on the rpmsg bus begins with this header.
 **/
struct rpmsg_hdr {
	u32 src;
	u32 dst;
	u32 reserved;
	u16 len;
	u16 flags;
	u8 data[0];
} __packed;
/**
 * struct rpmsg_ns_msg - dynamic name service announcement message
 * @name: name of remote service that is published
 * @addr: address of remote service that is published
 * @flags: indicates whether service is created or destroyed
 *
 * This message is sent across to publish a new service, or announce
 * about its removal. When we receive these messages, an appropriate
 * rpmsg channel (i.e device) is created/destroyed. In turn, the ->probe()
 * or ->remove() handler of the appropriate rpmsg driver will be invoked
 * (if/as-soon-as one is registered).
 **/
struct rpmsg_ns_msg {
	char name[RPMSG_NAME_SIZE];
	u32 addr;
	u32 flags;
} __packed;
/**
 * enum rpmsg_ns_flags - dynamic name service announcement flags
 *
 * @RPMSG_NS_CREATE: a new remote service was just created
 * @RPMSG_NS_DESTROY: a known remote service was just destroyed
 */
enum rpmsg_ns_flags {
	RPMSG_NS_CREATE         = 0,
	RPMSG_NS_DESTROY        = 1,
};
#endif



#define MIN_MTU 68
#define RPMSG_BUFFER_SIZE (2048) // TODO: get from virtio or DTS
#define MAX_MTU (RPMSG_BUFFER_SIZE - ETH_HLEN - sizeof(struct rpmsg_hdr))
#define DEF_MTU MAX_MTU

//extern int ambveth_do_send(void *data, int len);

struct ambveth
{
	spinlock_t		tx_lock;
	spinlock_t		rx_lock;

	struct net_device_stats	stats;
	struct net_device	*ndev;

	struct rpmsg_device_id	rpdev_id_tbl;
	struct rpmsg_driver	rpdev_drv;
	struct rpmsg_device	*rpdev;
	int			rpdev_role; // 0: Master/Sender, 1: Slace/Echo-back
};

void ambveth_enqueue(void *priv, void *data, int len)
{
	struct ambveth		*lp;
	struct sk_buff		*skb;

	lp = (struct ambveth *)priv;

	skb = dev_alloc_skb(len + NET_IP_ALIGN);
	if (!skb) {
		lp->stats.rx_dropped++;
		return;
	}

	spin_lock(&(lp->rx_lock));
	skb_put(skb, len);

	memcpy(skb->data, data, len);

	lp->stats.rx_packets++;
	lp->stats.rx_bytes += len;

	skb->dev = lp->ndev;
	skb->protocol = eth_type_trans(skb, skb->dev);
	spin_unlock(&(lp->rx_lock));

	netif_rx_ni(skb);
}

static int ambveth_start_xmit(struct sk_buff *skb, struct net_device *ndev)
{
	struct ambveth		*lp;
	int			ret;

	lp = netdev_priv(ndev);

	/* It is normal if remote OS is not ready yet */
	if (!lp->rpdev) {
		//return NETDEV_TX_BUSY; //do not trigger re-send
		pr_debug("NULL rpdev");
		lp->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

	spin_lock(&(lp->tx_lock));
	ret = rpmsg_trysend(lp->rpdev->ept, skb->data, skb->len);
	if (ret) {
	    lp->stats.tx_dropped++;

	    spin_unlock(&(lp->tx_lock));
	    return NETDEV_TX_BUSY;
	}

	lp->stats.tx_packets++;
	lp->stats.tx_bytes += skb->len;

	dev_kfree_skb(skb);
	spin_unlock(&(lp->tx_lock));

	return NETDEV_TX_OK;
}

static int ambveth_open(struct net_device *ndev)
{
	struct ambveth	*lp;

	lp = netdev_priv(ndev);

	netif_start_queue(ndev);

	return 0;
}

static int ambveth_stop(struct net_device *ndev)
{
	struct ambveth	*lp;

	lp = netdev_priv(ndev);

	netif_stop_queue(ndev);
	flush_scheduled_work();

	return 0;
}

static void ambveth_timeout(struct net_device *ndev, unsigned int txqueue)
{
	// TODO: txqueue
	(void)txqueue;
	netif_wake_queue(ndev);
}

static struct net_device_stats *ambveth_get_stats(struct net_device *ndev)
{
	struct ambveth *lp;

	lp = netdev_priv(ndev);

	return &lp->stats;
}

static int ambveth_change_mtu(struct net_device *dev, int mtu)
{
	if (mtu < MIN_MTU || mtu + dev->hard_header_len > MAX_MTU)

		return -EINVAL;

	dev->mtu = mtu;

	return 0;
}

static int ambveth_ioctl(struct net_device *ndev, struct ifreq *ifr, int cmd)
{
	int		ret = 0;

	if (!netif_running(ndev)) {
		ret = -EINVAL;
		goto ambveth_get_settings_exit;
	}

ambveth_get_settings_exit:
	return ret;
}

/* ==========================================================================*/
static const struct net_device_ops ambveth_netdev_ops = {
	.ndo_open		= ambveth_open,
	.ndo_stop		= ambveth_stop,
	.ndo_start_xmit		= ambveth_start_xmit,
	.ndo_set_mac_address 	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_do_ioctl		= ambveth_ioctl,
	.ndo_tx_timeout		= ambveth_timeout,
	.ndo_get_stats		= ambveth_get_stats,
	.ndo_change_mtu		= ambveth_change_mtu,
};

/* ========================= RPMSG Driver =================================== */

static int rpmsg_veth_server_cb(struct rpmsg_device *rpdev, void *data, int len,
			void *priv, u32 src)
{
	if (rpdev->dst == RPMSG_ADDR_ANY) {
		rpdev->dst = src;
		return 0;
	}

	ambveth_enqueue(priv, data, len);
	return 0;
}

static int rpmsg_veth_server_probe(struct rpmsg_device *rpdev)
{
	int ret = 0;
	struct rpmsg_channel_info chinfo;
	struct device_driver *d_drv = rpdev->dev.driver;
	struct rpmsg_driver *r_drv = container_of(d_drv, struct rpmsg_driver, drv);
	struct ambveth *lp = container_of(r_drv, struct ambveth, rpdev_drv);

	rpdev->ept->priv = lp;
	lp->rpdev = rpdev;

	/* Created by NS, need to echo back */
	if ((rpdev->src != RPMSG_ADDR_ANY) &&
		(rpdev->dst != RPMSG_ADDR_ANY)) {
		lp->rpdev_role = 0;
		strncpy(chinfo.name, rpdev->id.name, sizeof(chinfo.name));
		chinfo.src = rpdev->src;
		chinfo.dst = rpdev->dst;
		rpmsg_send(rpdev->ept, &chinfo, sizeof(chinfo));
	}

	return ret;
}

static void rpmsg_veth_server_remove(struct rpmsg_device *rpdev)
{
}

/* ========================================================================== */

static int ambveth_drv_probe(struct platform_device *pdev)
{

	int			ret = 0;
	struct net_device	*ndev;
	struct ambveth		*lp;
	char			mac_addr[6];
	const char *name;

	ret = of_property_read_string(pdev->dev.of_node, "amb,rpmsg_id", &name);
	if (ret < 0)
		return -EINVAL;
	else
		dev_info(&pdev->dev, "Create app device named as %s\n", name);

	ndev = alloc_etherdev(sizeof(struct ambveth));
	if (!ndev)
		ret = -ENOMEM;
	SET_NETDEV_DEV(ndev, &pdev->dev);

	lp = netdev_priv(ndev);
	lp->ndev = ndev;
	spin_lock_init(&lp->tx_lock);
	spin_lock_init(&lp->rx_lock);

	ndev->netdev_ops = &ambveth_netdev_ops;
	ndev->mtu = DEF_MTU;
	strcpy(ndev->name, "veth%d");

	if (!is_valid_ether_addr(mac_addr))
		random_ether_addr(mac_addr);

	memcpy(ndev->dev_addr, mac_addr, 6);

	ret = register_netdev(ndev);
	if (ret)
		dev_err(&pdev->dev, " register_netdev fail%d.\n", ret);

	/* Register RPMSG driver */
	memset(lp->rpdev_id_tbl.name, 0, RPMSG_NAME_SIZE);
	strlcpy(lp->rpdev_id_tbl.name, name, RPMSG_NAME_SIZE);
	lp->rpdev_drv.drv.name	= lp->rpdev_id_tbl.name;
	lp->rpdev_drv.drv.owner	= THIS_MODULE;
	lp->rpdev_drv.id_table	= &lp->rpdev_id_tbl;
	lp->rpdev_drv.probe	= rpmsg_veth_server_probe,
	lp->rpdev_drv.callback	= rpmsg_veth_server_cb,
	lp->rpdev_drv.remove	= rpmsg_veth_server_remove,

	ret = register_rpmsg_driver(&lp->rpdev_drv);
	if (ret)
		dev_err(&pdev->dev, "register_rpmsg_driver failed\n");

	return ret;
}

static const struct of_device_id ambarella_veth_dt_ids[] = {
	{ .compatible = "ambarella,veth" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ambarella_veth_dt_ids);

static struct platform_driver ambveth_driver = {
	.probe		= ambveth_drv_probe,
	.driver = {
		.name	= "ambveth",
		.owner	= THIS_MODULE,
		.of_match_table	= ambarella_veth_dt_ids,
	},
};

static int __init ambveth_init(void)
{
	return platform_driver_register(&ambveth_driver);
}

static void __exit ambveth_fini(void)
{
}

module_init(ambveth_init);
module_exit(ambveth_fini);

MODULE_DESCRIPTION("Ambarella veth driver");
MODULE_LICENSE("GPL");

