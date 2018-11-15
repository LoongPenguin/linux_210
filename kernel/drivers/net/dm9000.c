/*
 *      Davicom DM9000 Fast Ethernet driver for Linux.
 * 	Copyright (C) 1997  Sten Wang
 *
 * 	This program is free software; you can redistribute it and/or
 * 	modify it under the terms of the GNU General Public License
 * 	as published by the Free Software Foundation; either version 2
 * 	of the License, or (at your option) any later version.
 *
 * 	This program is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * (C) Copyright 1997-1998 DAVICOM Semiconductor,Inc. All Rights Reserved.
 *
 * Additional updates, Copyright:
 *	Ben Dooks <ben@simtec.co.uk>
 *	Sascha Hauer <s.hauer@pengutronix.de>
 */

#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/crc32.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/dm9000.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/slab.h>

#include <asm/delay.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/gpio.h>
#include <asm/gpio.h>
#include <asm/delay.h>
#include <linux/clk.h>
#include <plat/gpio-cfg.h>

#include "dm9000.h"

/* Board/System/Debug information/definition ---------------- */

#define DM9000_PHY		0x40	/* PHY address 0x01 */

#define CARDNAME	"dm9000"
#define DRV_VERSION	"1.31"

/*
 * Transmit timeout, default 5 seconds.
 */
static int watchdog = 5000;
module_param(watchdog, int, 0400);
MODULE_PARM_DESC(watchdog, "transmit timeout in milliseconds");

/* DM9000 register address locking.
 *
 * The DM9000 uses an address register to control where data written
 * to the data register goes. This means that the address register
 * must be preserved over interrupts or similar calls.
 *
 * During interrupt and other critical calls, a spinlock is used to
 * protect the system, but the calls themselves save the address
 * in the address register in case they are interrupting another
 * access to the device.
 *
 * For general accesses a lock is provided so that calls which are
 * allowed to sleep are serialised so that the address register does
 * not need to be saved. This lock also serves to serialise access
 * to the EEPROM and PHY access registers which are shared between
 * these two devices.
 */

/* The driver supports the original DM9000E, and now the two newer
 * devices, DM9000A and DM9000B.
 */

static void dm9000_power_int(void)
{
}

static void dm9000_power_on(int flag)
{
	if(flag)
	{
	}
	else
	{
	}
}

enum dm9000_type {
	TYPE_DM9000E,	/* original DM9000 */
	TYPE_DM9000A,
	TYPE_DM9000C
};

/* Structure/enum declaration ------------------------------- */
typedef struct board_info {

	void __iomem	*io_addr;	/* Register I/O base address */
	void __iomem	*io_data;	/* Data I/O address */
	u16		 irq;		/* IRQ */

	u16		tx_pkt_cnt;
	u16		dbug_cnt;
	u8		io_mode;		/* 0:word, 2:byte */
	u8		phy_addr;
	u8		imr_all;
	u8		wait_reset;

	unsigned int	flags;
	unsigned int	in_suspend :1;
	unsigned int	wake_supported :1;
	int		debug_level;

	enum dm9000_type type;

	void (*inblk)(void __iomem *port, void *data, int length);
	void (*outblk)(void __iomem *port, void *data, int length);
	void (*dumpblk)(void __iomem *port, int length);

	struct device	*dev;	     /* parent device */

	struct resource	*addr_res;   /* resources found */
	struct resource *data_res;
	struct resource	*addr_req;   /* resources requested */
	struct resource *data_req;
	struct resource *irq_res;

	int		 irq_wake;

	struct mutex	 addr_lock;	/* phy and eeprom access lock */

	struct delayed_work phy_poll;
	struct net_device  *ndev;

	spinlock_t	lock;

	struct mii_if_info mii;
	u32		msg_enable;
	u32		wake_state;

} board_info_t;

/* debug code */

#define dm9000_dbg(db, lev, msg...) do {		\
	if ((lev) < CONFIG_DM9000_DEBUGLEVEL &&		\
	    (lev) < db->debug_level) {			\
		dev_dbg(db->dev, msg);			\
	}						\
} while (0)

static inline board_info_t *to_dm9000_board(struct net_device *dev)
{
	return netdev_priv(dev);
}

static void dm9000_phy_write(struct net_device *, int, int, int);

/* DM9000 network board routine ---------------------------- */

/*
 *   Read a byte from I/O port
 */
static u8
ior(board_info_t * db, int reg)
{
	writeb(reg, db->io_addr);
	return readb(db->io_data);
}

/*
 *   Write a byte to I/O port
 */

static void
iow(board_info_t * db, int reg, int value)
{
	writeb(reg, db->io_addr);
	writeb(value, db->io_data);
}

static void
dm9000_reset(board_info_t * db)
{
	dev_dbg(db->dev, "resetting device\n");

	/* RESET device */
	writeb(DM9000_NCR, db->io_addr);
	udelay(200);
	writeb(NCR_RST, db->io_data);
	udelay(200);
}

/* routines for sending block to chip */

static void dm9000_outblk_8bit(void __iomem *reg, void *data, int count)
{
	writesb(reg, data, count);
}

static void dm9000_outblk_16bit(void __iomem *reg, void *data, int count)
{
	writesw(reg, data, (count+1) >> 1);
}

static void dm9000_outblk_32bit(void __iomem *reg, void *data, int count)
{
	writesl(reg, data, (count+3) >> 2);
}

/* input block from chip to memory */

static void dm9000_inblk_8bit(void __iomem *reg, void *data, int count)
{
	readsb(reg, data, count);
}


static void dm9000_inblk_16bit(void __iomem *reg, void *data, int count)
{
	readsw(reg, data, (count+1) >> 1);
}

static void dm9000_inblk_32bit(void __iomem *reg, void *data, int count)
{
	readsl(reg, data, (count+3) >> 2);
}

/* dump block from chip to null */

static void dm9000_dumpblk_8bit(void __iomem *reg, int count)
{
	int i;
	int tmp;

	for (i = 0; i < count; i++)
		tmp = readb(reg);
}

static void dm9000_dumpblk_16bit(void __iomem *reg, int count)
{
	int i;
	int tmp;

	count = (count + 1) >> 1;

	for (i = 0; i < count; i++)
		tmp = readw(reg);
}

static void dm9000_dumpblk_32bit(void __iomem *reg, int count)
{
	int i;
	int tmp;

	count = (count + 3) >> 2;

	for (i = 0; i < count; i++)
		tmp = readl(reg);
}

/* dm9000_set_io
 *
 * select the specified set of io routines to use with the
 * device
 */

static void dm9000_set_io(struct board_info *db, int byte_width)
{
	/* use the size of the data resource to work out what IO
	 * routines we want to use
	 */

	switch (byte_width) {
	case 1:
		db->dumpblk = dm9000_dumpblk_8bit;
		db->outblk  = dm9000_outblk_8bit;
		db->inblk   = dm9000_inblk_8bit;
		break;


	case 3:
		dev_dbg(db->dev, ": 3 byte IO, falling back to 16bit\n");
	case 2:
		db->dumpblk = dm9000_dumpblk_16bit;
		db->outblk  = dm9000_outblk_16bit;
		db->inblk   = dm9000_inblk_16bit;
		break;

	case 4:
	default:
		db->dumpblk = dm9000_dumpblk_32bit;
		db->outblk  = dm9000_outblk_32bit;
		db->inblk   = dm9000_inblk_32bit;
		break;
	}
}

static void dm9000_schedule_poll(board_info_t *db)
{
	if (db->type == TYPE_DM9000E)
		schedule_delayed_work(&db->phy_poll, HZ * 2);
}

static int dm9000_ioctl(struct net_device *dev, struct ifreq *req, int cmd)
{
	board_info_t *dm = to_dm9000_board(dev);

	if (!netif_running(dev))
		return -EINVAL;

	return generic_mii_ioctl(&dm->mii, if_mii(req), cmd, NULL);
}

static unsigned int
dm9000_read_locked(board_info_t *db, int reg)
{
	unsigned long flags;
	unsigned int ret;

	spin_lock_irqsave(&db->lock, flags);
	ret = ior(db, reg);
	spin_unlock_irqrestore(&db->lock, flags);

	return ret;
}

static int dm9000_wait_eeprom(board_info_t *db)
{
	unsigned int status;
	int timeout = 8;	/* wait max 8msec */

	/* The DM9000 data sheets say we should be able to
	 * poll the ERRE bit in EPCR to wait for the EEPROM
	 * operation. From testing several chips, this bit
	 * does not seem to work.
	 *
	 * We attempt to use the bit, but fall back to the
	 * timeout (which is why we do not return an error
	 * on expiry) to say that the EEPROM operation has
	 * completed.
	 */

	while (1) {
		status = dm9000_read_locked(db, DM9000_EPCR);

		if ((status & EPCR_ERRE) == 0)
			break;

		msleep(1);

		if (timeout-- < 0) {
			dev_dbg(db->dev, "timeout waiting EEPROM\n");
			break;
		}
	}

	return 0;
}

/*
 *  Read a word data from EEPROM
 */
static void
dm9000_read_eeprom(board_info_t *db, int offset, u8 *to)
{
	unsigned long flags;

	if (db->flags & DM9000_PLATF_NO_EEPROM) {
		to[0] = 0xff;
		to[1] = 0xff;
		return;
	}

	mutex_lock(&db->addr_lock);

	spin_lock_irqsave(&db->lock, flags);

	iow(db, DM9000_EPAR, offset);
	iow(db, DM9000_EPCR, EPCR_ERPRR);

	spin_unlock_irqrestore(&db->lock, flags);

	dm9000_wait_eeprom(db);

	/* delay for at-least 150uS */
	msleep(1);

	spin_lock_irqsave(&db->lock, flags);

	iow(db, DM9000_EPCR, 0x0);

	to[0] = ior(db, DM9000_EPDRL);
	to[1] = ior(db, DM9000_EPDRH);

	spin_unlock_irqrestore(&db->lock, flags);

	mutex_unlock(&db->addr_lock);
}

/*
 * Write a word data to SROM
 */
static void
dm9000_write_eeprom(board_info_t *db, int offset, u8 *data)
{
	unsigned long flags;

	if (db->flags & DM9000_PLATF_NO_EEPROM)
		return;

	mutex_lock(&db->addr_lock);

	spin_lock_irqsave(&db->lock, flags);
	iow(db, DM9000_EPAR, offset);
	iow(db, DM9000_EPDRH, data[1]);
	iow(db, DM9000_EPDRL, data[0]);
	iow(db, DM9000_EPCR, EPCR_WEP | EPCR_ERPRW);
	spin_unlock_irqrestore(&db->lock, flags);

	dm9000_wait_eeprom(db);

	mdelay(1);	/* wait at least 150uS to clear */

	spin_lock_irqsave(&db->lock, flags);
	iow(db, DM9000_EPCR, 0);
	spin_unlock_irqrestore(&db->lock, flags);

	mutex_unlock(&db->addr_lock);
}

/* ethtool ops */

static void dm9000_get_drvinfo(struct net_device *dev,
			       struct ethtool_drvinfo *info)
{
	board_info_t *dm = to_dm9000_board(dev);

	strcpy(info->driver, CARDNAME);
	strcpy(info->version, DRV_VERSION);
	strcpy(info->bus_info, to_platform_device(dm->dev)->name);
}

static u32 dm9000_get_msglevel(struct net_device *dev)
{
	board_info_t *dm = to_dm9000_board(dev);

	return dm->msg_enable;
}

static void dm9000_set_msglevel(struct net_device *dev, u32 value)
{
	board_info_t *dm = to_dm9000_board(dev);

	dm->msg_enable = value;
}

static int dm9000_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	board_info_t *dm = to_dm9000_board(dev);

	mii_ethtool_gset(&dm->mii, cmd);
	return 0;
}

static int dm9000_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	board_info_t *dm = to_dm9000_board(dev);

	return mii_ethtool_sset(&dm->mii, cmd);
}

static int dm9000_nway_reset(struct net_device *dev)
{
	board_info_t *dm = to_dm9000_board(dev);
	return mii_nway_restart(&dm->mii);
}

static u32 dm9000_get_link(struct net_device *dev)
{
	board_info_t *dm = to_dm9000_board(dev);
	u32 ret;

	if (dm->flags & DM9000_PLATF_EXT_PHY)
		ret = mii_link_ok(&dm->mii);
	else
		ret = dm9000_read_locked(dm, DM9000_NSR) & NSR_LINKST ? 1 : 0;

	return ret;
}

#define DM_EEPROM_MAGIC		(0x444D394B)

static int dm9000_get_eeprom_len(struct net_device *dev)
{
	return 128;
}

static int dm9000_get_eeprom(struct net_device *dev,
			     struct ethtool_eeprom *ee, u8 *data)
{
	board_info_t *dm = to_dm9000_board(dev);
	int offset = ee->offset;
	int len = ee->len;
	int i;

	/* EEPROM access is aligned to two bytes */

	if ((len & 1) != 0 || (offset & 1) != 0)
		return -EINVAL;

	if (dm->flags & DM9000_PLATF_NO_EEPROM)
		return -ENOENT;

	ee->magic = DM_EEPROM_MAGIC;

	for (i = 0; i < len; i += 2)
		dm9000_read_eeprom(dm, (offset + i) / 2, data + i);

	return 0;
}

static int dm9000_set_eeprom(struct net_device *dev,
			     struct ethtool_eeprom *ee, u8 *data)
{
	board_info_t *dm = to_dm9000_board(dev);
	int offset = ee->offset;
	int len = ee->len;
	int i;

	/* EEPROM access is aligned to two bytes */

	if ((len & 1) != 0 || (offset & 1) != 0)
		return -EINVAL;

	if (dm->flags & DM9000_PLATF_NO_EEPROM)
		return -ENOENT;

	if (ee->magic != DM_EEPROM_MAGIC)
		return -EINVAL;

	for (i = 0; i < len; i += 2)
		dm9000_write_eeprom(dm, (offset + i) / 2, data + i);

	return 0;
}

static void dm9000_get_wol(struct net_device *dev, struct ethtool_wolinfo *w)
{
	board_info_t *dm = to_dm9000_board(dev);

	memset(w, 0, sizeof(struct ethtool_wolinfo));

	/* note, we could probably support wake-phy too */
	w->supported = dm->wake_supported ? WAKE_MAGIC : 0;
	w->wolopts = dm->wake_state;
}

static int dm9000_set_wol(struct net_device *dev, struct ethtool_wolinfo *w)
{
	board_info_t *dm = to_dm9000_board(dev);
	unsigned long flags;
	u32 opts = w->wolopts;
	u32 wcr = 0;

	if (!dm->wake_supported)
		return -EOPNOTSUPP;

	if (opts & ~WAKE_MAGIC)
		return -EINVAL;

	if (opts & WAKE_MAGIC)
		wcr |= WCR_MAGICEN;

	mutex_lock(&dm->addr_lock);

	spin_lock_irqsave(&dm->lock, flags);
	iow(dm, DM9000_WCR, wcr);
	spin_unlock_irqrestore(&dm->lock, flags);

	mutex_unlock(&dm->addr_lock);

	if (dm->wake_state != opts) {
		/* change in wol state, update IRQ state */

		if (!dm->wake_state)
			set_irq_wake(dm->irq_wake, 1);
		else if (dm->wake_state & !opts)
			set_irq_wake(dm->irq_wake, 0);
	}

	dm->wake_state = opts;
	return 0;
}

static const struct ethtool_ops dm9000_ethtool_ops = {
	.get_drvinfo		= dm9000_get_drvinfo,
	.get_settings		= dm9000_get_settings,
	.set_settings		= dm9000_set_settings,
	.get_msglevel		= dm9000_get_msglevel,
	.set_msglevel		= dm9000_set_msglevel,
	.nway_reset		= dm9000_nway_reset,
	.get_link		= dm9000_get_link,
	.get_wol		= dm9000_get_wol,
	.set_wol		= dm9000_set_wol,
 	.get_eeprom_len		= dm9000_get_eeprom_len,
 	.get_eeprom		= dm9000_get_eeprom,
 	.set_eeprom		= dm9000_set_eeprom,
};

static void dm9000_show_carrier(board_info_t *db,
				unsigned carrier, unsigned nsr)
{
	struct net_device *ndev = db->ndev;
	unsigned ncr = dm9000_read_locked(db, DM9000_NCR);

	if (carrier)
		dev_info(db->dev, "%s: link up, %dMbps, %s-duplex, no LPA\n",
			 ndev->name, (nsr & NSR_SPEED) ? 10 : 100,
			 (ncr & NCR_FDX) ? "full" : "half");
	else
		dev_info(db->dev, "%s: link down\n", ndev->name);
}

static void
dm9000_poll_work(struct work_struct *w)
{
	struct delayed_work *dw = to_delayed_work(w);
	board_info_t *db = container_of(dw, board_info_t, phy_poll);
	struct net_device *ndev = db->ndev;

	if (db->flags & DM9000_PLATF_SIMPLE_PHY &&
	    !(db->flags & DM9000_PLATF_EXT_PHY)) {
		unsigned nsr = dm9000_read_locked(db, DM9000_NSR);
		unsigned old_carrier = netif_carrier_ok(ndev) ? 1 : 0;
		unsigned new_carrier;

		new_carrier = (nsr & NSR_LINKST) ? 1 : 0;

		if (old_carrier != new_carrier) {
			if (netif_msg_link(db))
				dm9000_show_carrier(db, new_carrier, nsr);

			if (!new_carrier)
				netif_carrier_off(ndev);
			else
				netif_carrier_on(ndev);
		}
	} else
		mii_check_media(&db->mii, netif_msg_link(db), 0);
	
	if (netif_running(ndev))
		dm9000_schedule_poll(db);
}

/* dm9000_release_board
 *
 * release a board, and any mapped resources
 */

static void
dm9000_release_board(struct platform_device *pdev, struct board_info *db)
{
	/* unmap our resources */

	iounmap(db->io_addr);
	iounmap(db->io_data);

	/* release the resources */

	release_resource(db->data_req);
	kfree(db->data_req);

	release_resource(db->addr_req);
	kfree(db->addr_req);
}

static unsigned char dm9000_type_to_char(enum dm9000_type type)
{
	switch (type) {
	case TYPE_DM9000E: return 'e';
	case TYPE_DM9000A: return 'a';
	case TYPE_DM9000C: return 'c';
	}

	return '?';
}

/*
 *  Set DM9000 multicast address
 */
static void
dm9000_hash_table_unlocked(struct net_device *dev)
{
	board_info_t *db = netdev_priv(dev);
	struct netdev_hw_addr *ha;
	int i, oft;
	u32 hash_val;
	u16 hash_table[4];
	u8 rcr = RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN;

	dm9000_dbg(db, 1, "entering %s\n", __func__);

	for (i = 0, oft = DM9000_PAR; i < 6; i++, oft++)
		iow(db, oft, dev->dev_addr[i]);

	/* Clear Hash Table */
	for (i = 0; i < 4; i++)
		hash_table[i] = 0x0;

	/* broadcast address */
	hash_table[3] = 0x8000;

	if (dev->flags & IFF_PROMISC)
		rcr |= RCR_PRMSC;

	if (dev->flags & IFF_ALLMULTI)
		rcr |= RCR_ALL;

	/* the multicast address in Hash Table : 64 bits */
	netdev_for_each_mc_addr(ha, dev) {
		hash_val = ether_crc_le(6, ha->addr) & 0x3f;
		hash_table[hash_val / 16] |= (u16) 1 << (hash_val % 16);
	}

	/* Write the hash table to MAC MD table */
	for (i = 0, oft = DM9000_MAR; i < 4; i++) {
		iow(db, oft++, hash_table[i]);
		iow(db, oft++, hash_table[i] >> 8);
	}

	iow(db, DM9000_RCR, rcr);
}

static void
dm9000_hash_table(struct net_device *dev)
{
	board_info_t *db = netdev_priv(dev);
	unsigned long flags;

	spin_lock_irqsave(&db->lock, flags);
	dm9000_hash_table_unlocked(dev);
	spin_unlock_irqrestore(&db->lock, flags);
}

/*
 * Initialize dm9000 board
 */
static void
dm9000_init_dm9000(struct net_device *dev)
{
	board_info_t *db = netdev_priv(dev);
	unsigned int imr;
	unsigned int ncr;

	dm9000_dbg(db, 1, "entering %s\n", __func__);
	
	if(!(ior(db, DM9000_NSR) & 0x40))
	{
		/* GPIO0 on pre-activate PHY */
		iow(db, DM9000_GPCR, GPCR_GEP_CNTL);	/* Let GPIO0 output */
		iow(db, DM9000_GPR, 1);	/* REG_1F bit0 activate phyxcer */
		udelay(100);
		
		dm9000_phy_write(dev, 0, MII_BMCR, BMCR_RESET);	/* PHY RESET */
		udelay(100);
		
		dm9000_phy_write(dev, 0, 0x14, 0x0200);
		if(TYPE_DM9000C == db->type)		
			dm9000_phy_write(dev, 0, 0x1b, 0x1e00);
		
		iow(db, DM9000_GPR, 0);	/* Enable PHY */
		do
		{
			udelay(500);
		}while(0x46 != ior(db, 0x28));
	}
	
	dm9000_reset(db);

	ncr = (db->flags & DM9000_PLATF_EXT_PHY) ? NCR_EXT_PHY : 0;

	/* if wol is needed, then always set NCR_WAKEEN otherwise we end
	 * up dumping the wake events if we disable this. There is already
	 * a wake-mask in DM9000_WCR */
	if (db->wake_supported)
		ncr |= NCR_WAKEEN;

	iow(db, DM9000_NCR, ncr);

	/* Program operating register */
	iow(db, DM9000_IMR, IMR_PAR);
	iow(db, DM9000_TCR, 0);	        /* TX Polling clear */
	iow(db, DM9000_BPTR, 0x3f);	/* Less 3Kb, 200us */
	iow(db, DM9000_FCR, 0x28);	/* Flow Control */
	/* clear TX status */
	iow(db, DM9000_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);
	iow(db, DM9000_ISR, ISR_CLR_STATUS); /* Clear interrupt status */
	
	iow(db, 0x2d, 0x80);
	
	if(TYPE_DM9000C == db->type)
	{
		iow(db, DM9000_SMCR, 0);        /* Special Mode */
		iow(db, 0x38, 0x6b);
	}
	else
	{
		iow(db, DM9000_SMCR, 0x08);        /* Special Mode */
		iow(db, 0x38, 0x2b);
	}

	/* Set address filter table */
	dm9000_hash_table_unlocked(dev);

	imr = IMR_PAR | IMR_PTM | IMR_PRM;
	if (db->type != TYPE_DM9000E)
		imr |= IMR_LNKCHNG;

	db->imr_all = imr;

	/* Enable TX/RX interrupt mask */
	iow(db, DM9000_IMR, imr);

	/* Init Driver variable */
	db->wait_reset = 0;
	db->tx_pkt_cnt = 0;
	dev->trans_start = 0;
}

/* Our watchdog timed out. Called by the networking layer */
static void dm9000_timeout(struct net_device *dev)
{
	board_info_t *db = netdev_priv(dev);
	u8 reg_save;
	unsigned long flags;

	/* Save previous register address */
	reg_save = readb(db->io_addr);
	spin_lock_irqsave(&db->lock, flags);

	netif_stop_queue(dev);
//	dm9000_reset(db);
	dm9000_init_dm9000(dev);
	/* We can accept TX packets again */
	dev->trans_start = jiffies; /* prevent tx timeout */
	netif_wake_queue(dev);

	/* Restore previous register address */
	writeb(reg_save, db->io_addr);
	spin_unlock_irqrestore(&db->lock, flags);
}

/*
 *  Hardware start transmission.
 *  Send a packet to media from the upper layer.
 */
static int
dm9000_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	unsigned long flags;
	board_info_t *db = netdev_priv(dev);

	dm9000_dbg(db, 3, "%s:\n", __func__);

	if ((db->tx_pkt_cnt > 0) && !netif_carrier_ok(dev))
		return NETDEV_TX_BUSY;

	spin_lock_irqsave(&db->lock, flags);

	netif_stop_queue(dev);
	
	db->tx_pkt_cnt++;
	dev->stats.tx_packets++;
	dev->stats.tx_bytes += skb->len;

	/* Set TX length to DM9000 */
	iow(db, DM9000_TXPLL, skb->len);
	iow(db, DM9000_TXPLH, skb->len >> 8);

	/* Move data to DM9000 TX RAM */
	writeb(DM9000_MWCMD, db->io_addr);
	(db->outblk)(db->io_data, skb->data, skb->len);

	/* Issue TX polling command */
	iow(db, DM9000_TCR, TCR_TXREQ);	/* Cleared after TX complete */
	dev->trans_start = jiffies;

	spin_unlock_irqrestore(&db->lock, flags);

	/* free this SKB */
	dev_kfree_skb(skb);

	return NETDEV_TX_OK;
}

/*
 * DM9000 interrupt handler
 * receive the packet to upper layer, free the transmitted packet
 */

static void dm9000_tx_done(struct net_device *dev, board_info_t *db)
{
	int tx_status = ior(db, DM9000_TCR);	/* Got TX status */

	if (tx_status & TCR_TXREQ)
	{
		dev->stats.tx_fifo_errors++;
	}
	else
	{
		if(db->tx_pkt_cnt && !db->wait_reset)
		{
			/* One packet sent complete */
			db->tx_pkt_cnt = 0;
			dev->trans_start = 0;
			netif_wake_queue(dev);
		}
	}
}

struct dm9000_rxhdr {
	u8	RxPktReady;
	u8	RxStatus;
	__le16	RxLen;
} __attribute__((__packed__));

/*
 *  Received a packet and pass to upper layer
 */
static void
dm9000_rx(struct net_device *dev)
{
	board_info_t *db = netdev_priv(dev);
	struct dm9000_rxhdr rxhdr;
	struct sk_buff *skb;
	u8 rxbyte, *rdptr;
	bool GoodPacket;
	int RxLen;
	int save_mrr, calc_mrr, check_mrr;

	/* Check packet ready or not */
	do {
		ior(db, DM9000_MRCMDX);	/* Dummy read */
		save_mrr = (ior(db, 0xf5) << 8) | ior(db, 0xf4);
		/* Get most updated data */
		rxbyte = ior(db, DM9000_MRCMDX);
		
		if(rxbyte != DM9000_PKT_RDY)
		{
			/* Status check: this byte must be 0 or 1 */
			if (rxbyte > DM9000_PKT_RDY) {
				dev_warn(db->dev, "status check fail: %d\n", rxbyte);
				iow(db, DM9000_RCR, 0x00);	/* Stop Device */
				iow(db, DM9000_IMR, IMR_PAR);	/* Stop INT request */
				
				db->wait_reset = 1;
				dev->trans_start = 1;
			}
			
			return;
		}

		/* A packet ready now  & Get status/length */
		GoodPacket = true;
		writeb(DM9000_MRCMD, db->io_addr);
		(db->inblk)(db->io_data, &rxhdr, sizeof(rxhdr));

		RxLen = le16_to_cpu(rxhdr.RxLen);
		
		calc_mrr = save_mrr + 4 + RxLen;
		if(0x00 == db->io_mode)  //16 bit only
		{
			if(RxLen & 0x01) calc_mrr++;
		}
		if(calc_mrr > 0x3fff) calc_mrr -= 0x3400;

		if (netif_msg_rx_status(db))
			dev_dbg(db->dev, "RX: status %02x, length %04x\n",
				rxhdr.RxStatus, RxLen);

		/* Packet Status check */
		if (RxLen < 0x40) {
			GoodPacket = false;
			if (netif_msg_rx_err(db))
				dev_dbg(db->dev, "RX: Bad Packet (runt)\n");
		}

		if (RxLen > DM9000_PKT_MAX) {
			dev_dbg(db->dev, "RST: RX Len:%x\n", RxLen);
		}

		/* rxhdr.RxStatus is identical to RSR register. */
		if (rxhdr.RxStatus & (RSR_FOE | RSR_CE | RSR_AE |
				      RSR_PLE | RSR_RWTO |
				      RSR_LCS | RSR_RF)) {
			if (rxhdr.RxStatus & RSR_FOE) {
				if (netif_msg_rx_err(db))
					dev_dbg(db->dev, "fifo error\n");
				dev->stats.rx_fifo_errors++;
			}
			if (rxhdr.RxStatus & RSR_CE) {
				if (netif_msg_rx_err(db))
					dev_dbg(db->dev, "crc error\n");
				dev->stats.rx_crc_errors++;
				GoodPacket = false;
			}
			if (rxhdr.RxStatus & RSR_RF) {
				if (netif_msg_rx_err(db))
					dev_dbg(db->dev, "length error\n");
				dev->stats.rx_length_errors++;
				GoodPacket = false;
			}
		}

		/* Move data from DM9000 */
		if (GoodPacket &&
		    ((skb = dev_alloc_skb(RxLen + 4)) != NULL)) {
			skb_reserve(skb, 2);
			rdptr = (u8 *) skb_put(skb, RxLen - 4);

			/* Read received packet from RX SRAM */

			(db->inblk)(db->io_data, rdptr, RxLen);
			dev->stats.rx_bytes += RxLen;

			/* Pass to upper layer */
			skb->protocol = eth_type_trans(skb, dev);

			netif_rx(skb);
			dev->stats.rx_packets++;
			
			check_mrr = (ior(db, 0xf5) << 8) | ior(db, 0xf4);
			if(calc_mrr != check_mrr)
			{
				
				if (netif_msg_rx_err(db))
					dev_dbg(db->dev, "rx point error %04x %04x %04x %04x\n",
						save_mrr, RxLen, calc_mrr, check_mrr);
				
				iow(db, 0xf5, (calc_mrr >> 8) & 0xff);
				iow(db, 0xf4, calc_mrr & 0xff);
			}
		
		} else {
			/* need to dump the packet's data */
			iow(db, 0xf5, (calc_mrr >> 8) & 0xff);
			iow(db, 0xf4, calc_mrr & 0xff);

//			(db->dumpblk)(db->io_data, RxLen);
		}
		
	} while (rxbyte & DM9000_PKT_RDY);
}

static irqreturn_t dm9000_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	board_info_t *db = netdev_priv(dev);
	int int_status;
	unsigned long flags;
	u8 reg_save;

	dm9000_dbg(db, 3, "entering %s\n", __func__);

	/* A real interrupt coming */

	/* holders of db->lock must always block IRQs */
	spin_lock_irqsave(&db->lock, flags);

	/* Save previous register address */
	reg_save = readb(db->io_addr);

	/* Disable all interrupts */
	iow(db, DM9000_IMR, IMR_PAR);

	/* Got DM9000 interrupt status */
	int_status = ior(db, DM9000_ISR);	/* Got ISR */
	iow(db, DM9000_ISR, int_status);	/* Clear ISR status */

	if (netif_msg_intr(db))
		dev_dbg(db->dev, "interrupt status %02x\n", int_status);

	/* Received the coming packet */
	if (int_status & ISR_PRS)
		dm9000_rx(dev);

	/* Got DM9000 interrupt status */
	int_status |= ior(db, DM9000_ISR);	/* Got ISR */

	/* Trnasmit Interrupt check */
	if (int_status & ISR_PTS)
	{
		iow(db, DM9000_ISR, ISR_PTS);	/* Clear ISR status */
		dm9000_tx_done(dev, db);
	}

	if (db->type != TYPE_DM9000E) {
		if (int_status & ISR_LNKCHNG) {
			/* fire a link-change request */
			schedule_delayed_work(&db->phy_poll, 1);
		}
	}

	/* Re-enable interrupt mask */
	iow(db, DM9000_IMR, db->imr_all);

	/* Restore previous register address */
	writeb(reg_save, db->io_addr);

	spin_unlock_irqrestore(&db->lock, flags);

	return IRQ_HANDLED;
}

static irqreturn_t dm9000_wol_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	board_info_t *db = netdev_priv(dev);
	unsigned long flags;
	unsigned nsr, wcr;

	spin_lock_irqsave(&db->lock, flags);

	nsr = ior(db, DM9000_NSR);
	wcr = ior(db, DM9000_WCR);

	dev_dbg(db->dev, "%s: NSR=0x%02x, WCR=0x%02x\n", __func__, nsr, wcr);

	if (nsr & NSR_WAKEST) {
		/* clear, so we can avoid */
		iow(db, DM9000_NSR, NSR_WAKEST);

		if (wcr & WCR_LINKST)
			dev_info(db->dev, "wake by link status change\n");
		if (wcr & WCR_SAMPLEST)
			dev_info(db->dev, "wake by sample packet\n");
		if (wcr & WCR_MAGICST )
			dev_info(db->dev, "wake by magic packet\n");
		if (!(wcr & (WCR_LINKST | WCR_SAMPLEST | WCR_MAGICST)))
			dev_err(db->dev, "wake signalled with no reason? "
				"NSR=0x%02x, WSR=0x%02x\n", nsr, wcr);

	}

	spin_unlock_irqrestore(&db->lock, flags);

	return (nsr & NSR_WAKEST) ? IRQ_HANDLED : IRQ_NONE;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
/*
 *Used by netconsole
 */
static void dm9000_poll_controller(struct net_device *dev)
{
	disable_irq(dev->irq);
	dm9000_interrupt(dev->irq, dev);
	enable_irq(dev->irq);
}
#endif

/*
 *  Open the interface.
 *  The interface is opened whenever "ifconfig" actives it.
 */
static int
dm9000_open(struct net_device *dev)
{
	board_info_t *db = netdev_priv(dev);
	unsigned long irqflags = db->irq_res->flags & IRQF_TRIGGER_MASK;

	if (netif_msg_ifup(db))
		dev_dbg(db->dev, "enabling %s\n", dev->name);

	/* If there is no IRQ type specified, default to something that
	 * may work, and tell the user that this is a problem */

	if (irqflags == IRQF_TRIGGER_NONE)
		dev_warn(db->dev, "WARNING: no IRQ resource flags set.\n");

	irqflags |= IRQF_SHARED;

	if (request_irq(dev->irq, dm9000_interrupt, irqflags, dev->name, dev))
		return -EAGAIN;

	/* Initialize DM9000 board */
//	dm9000_reset(db);
	dm9000_init_dm9000(dev);

	/* Init driver variable */
	db->dbug_cnt = 0;

	mii_check_media(&db->mii, netif_msg_link(db), 1);
	netif_start_queue(dev);
	
	dm9000_schedule_poll(db);

	return 0;
}

/*
 * Sleep, either by using msleep() or if we are suspending, then
 * use mdelay() to sleep.
 */
static void dm9000_msleep(board_info_t *db, unsigned int ms)
{
	if (db->in_suspend)
		mdelay(ms);
	else
		msleep(ms);
}

/*
 *   Read a word from phyxcer
 */
static int
dm9000_phy_read(struct net_device *dev, int phy_reg_unused, int reg)
{
	board_info_t *db = netdev_priv(dev);
	unsigned long flags;
	unsigned int reg_save;
	int ret;

	mutex_lock(&db->addr_lock);

	spin_lock_irqsave(&db->lock,flags);

	/* Save previous register address */
	reg_save = readb(db->io_addr);

	/* Fill the phyxcer register into REG_0C */
	iow(db, DM9000_EPAR, DM9000_PHY | reg);

	iow(db, DM9000_EPCR, EPCR_ERPRR | EPCR_EPOS);	/* Issue phyxcer read command */
	
	do
	{
		writeb(reg_save, db->io_addr);
		spin_unlock_irqrestore(&db->lock,flags);

		dm9000_msleep(db, 1);		/* Wait read complete */

		spin_lock_irqsave(&db->lock,flags);
		reg_save = readb(db->io_addr);
	}while((EPCR_ERPRR | EPCR_EPOS) != ior(db, DM9000_EPCR));

	iow(db, DM9000_EPCR, 0x0);	/* Clear phyxcer read command */

	/* The read data keeps on REG_0D & REG_0E */
	ret = (ior(db, DM9000_EPDRH) << 8) | ior(db, DM9000_EPDRL);

	/* restore the previous address */
	writeb(reg_save, db->io_addr);
	spin_unlock_irqrestore(&db->lock,flags);

	mutex_unlock(&db->addr_lock);

	dm9000_dbg(db, 5, "phy_read[%02x] -> %04x\n", reg, ret);
	return ret;
}

/*
 *   Write a word to phyxcer
 */
static void
dm9000_phy_write(struct net_device *dev,
		 int phyaddr_unused, int reg, int value)
{
	board_info_t *db = netdev_priv(dev);
	unsigned long flags;
	unsigned long reg_save;

	dm9000_dbg(db, 5, "phy_write[%02x] = %04x\n", reg, value);
	mutex_lock(&db->addr_lock);

	spin_lock_irqsave(&db->lock,flags);

	/* Save previous register address */
	reg_save = readb(db->io_addr);

	/* Fill the phyxcer register into REG_0C */
	iow(db, DM9000_EPAR, DM9000_PHY | reg);

	/* Fill the written data into REG_0D & REG_0E */
	iow(db, DM9000_EPDRL, value);
	iow(db, DM9000_EPDRH, value >> 8);

	iow(db, DM9000_EPCR, EPCR_EPOS | EPCR_ERPRW);	/* Issue phyxcer write command */

	do
	{
		writeb(reg_save, db->io_addr);
		spin_unlock_irqrestore(&db->lock, flags);

		dm9000_msleep(db, 1);		/* Wait write complete */

		spin_lock_irqsave(&db->lock,flags);
		reg_save = readb(db->io_addr);
	}while((EPCR_EPOS | EPCR_ERPRW) != ior(db, DM9000_EPCR));

	iow(db, DM9000_EPCR, 0x0);	/* Clear phyxcer write command */

	/* restore the previous address */
	writeb(reg_save, db->io_addr);

	spin_unlock_irqrestore(&db->lock, flags);
	mutex_unlock(&db->addr_lock);
}

static void
dm9000_shutdown(struct net_device *dev)
{
	board_info_t *db = netdev_priv(dev);

	/* RESET device */
	dm9000_phy_write(dev, 0, MII_BMCR, BMCR_RESET);	/* PHY RESET */
	iow(db, DM9000_GPR, 0x01);	/* Power-Down PHY */
	udelay(100);
	iow(db, DM9000_IMR, IMR_PAR);	/* Disable all interrupt */
	iow(db, DM9000_RCR, 0x00);	/* Disable RX */
}

/*
 * Stop the interface.
 * The interface is stopped when it is brought.
 */
static int
dm9000_stop(struct net_device *ndev)
{
	board_info_t *db = netdev_priv(ndev);

	if (netif_msg_ifdown(db))
		dev_dbg(db->dev, "shutting down %s\n", ndev->name);

	cancel_delayed_work_sync(&db->phy_poll);

	netif_stop_queue(ndev);
	netif_carrier_off(ndev);

	/* free interrupt */
	free_irq(ndev->irq, ndev);

	dm9000_shutdown(ndev);

	return 0;
}

static const struct net_device_ops dm9000_netdev_ops = {
	.ndo_open		= dm9000_open,
	.ndo_stop		= dm9000_stop,
	.ndo_start_xmit		= dm9000_start_xmit,
	.ndo_tx_timeout		= dm9000_timeout,
	.ndo_set_multicast_list	= dm9000_hash_table,
	.ndo_do_ioctl		= dm9000_ioctl,
	.ndo_change_mtu		= eth_change_mtu,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_set_mac_address	= eth_mac_addr,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= dm9000_poll_controller,
#endif
};

static unsigned char mac[6] = {0x00,0x09,0xc0,0xff,0xec,0x48};
static int __init mac_setup(char * str)
{
	unsigned int tmp[6];

    if ((str != NULL) && (*str != '\0'))
    {
    	if(sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]) == 6)
    	{
			mac[0] = tmp[0] & 0xff;
			mac[1] = tmp[1] & 0xff;
			mac[2] = tmp[2] & 0xff;
			mac[3] = tmp[3] & 0xff;
			mac[4] = tmp[4] & 0xff;
			mac[5] = tmp[5] & 0xff;
    	}
	}
	else
	{
		mac[0] = 0x00;
		mac[1] = 0x09;
		mac[2] = 0xc0;
		mac[3] = 0xff;
		mac[4] = 0xec;
		mac[5] = 0x48;
	}

	return 1;
}
__setup("mac=", mac_setup);

/*
 * Search DM9000 board, allocate space and register it
 */
static int __devinit
dm9000_probe(struct platform_device *pdev)
{
	struct dm9000_plat_data *pdata = pdev->dev.platform_data;
	struct board_info *db;	/* Point a board information structure */
	struct net_device *ndev;
	const unsigned char *mac_src;
	int ret = 0;
	int iosize;
	int i;
	u32 id_val;

	/* Init network device */
	ndev = alloc_etherdev(sizeof(struct board_info));
	if (!ndev) {
		dev_err(&pdev->dev, "could not allocate device.\n");
		return -ENOMEM;
	}

	SET_NETDEV_DEV(ndev, &pdev->dev);

	dev_dbg(&pdev->dev, "dm9000_probe()\n");

	/* setup board info structure */
	db = netdev_priv(ndev);

	db->dev = &pdev->dev;
	db->ndev = ndev;

	spin_lock_init(&db->lock);
	mutex_init(&db->addr_lock);

	INIT_DELAYED_WORK(&db->phy_poll, dm9000_poll_work);

	db->addr_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	db->data_res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	db->irq_res  = platform_get_resource(pdev, IORESOURCE_IRQ, 0);

	if (db->addr_res == NULL || db->data_res == NULL ||
	    db->irq_res == NULL) {
		dev_err(db->dev, "insufficient resources\n");
		ret = -ENOENT;
		goto out;
	}

	db->irq_wake = platform_get_irq(pdev, 1);
	if (db->irq_wake >= 0) {
		dev_dbg(db->dev, "wakeup irq %d\n", db->irq_wake);

		ret = request_irq(db->irq_wake, dm9000_wol_interrupt,IRQF_SHARED, dev_name(db->dev), ndev);
		//lqm changed irq method.
		//ret = request_irq(db->irq_wake, dm9000_wol_interrupt,IORESOURCE_IRQ_HIGHLEVEL, dev_name(db->dev), ndev);
		if (ret) {
			dev_err(db->dev, "cannot get wakeup irq (%d)\n", ret);
		} else {

			/* test to see if irq is really wakeup capable */
			ret = set_irq_wake(db->irq_wake, 1);
			if (ret) {
				dev_err(db->dev, "irq %d cannot set wakeup (%d)\n",
					db->irq_wake, ret);
				ret = 0;
			} else {
				set_irq_wake(db->irq_wake, 0);
				db->wake_supported = 1;
			}
		}
	}

	iosize = resource_size(db->addr_res);
	db->addr_req = request_mem_region(db->addr_res->start, iosize,
					  pdev->name);

	if (db->addr_req == NULL) {
		dev_err(db->dev, "cannot claim address reg area\n");
		ret = -EIO;
		goto out;
	}

	db->io_addr = ioremap(db->addr_res->start, iosize);

	if (db->io_addr == NULL) {
		dev_err(db->dev, "failed to ioremap address reg\n");
		ret = -EINVAL;
		goto out;
	}

	iosize = resource_size(db->data_res);
	db->data_req = request_mem_region(db->data_res->start, iosize,
					  pdev->name);

	if (db->data_req == NULL) {
		dev_err(db->dev, "cannot claim data reg area\n");
		ret = -EIO;
		goto out;
	}

	db->io_data = ioremap(db->data_res->start, iosize);

	if (db->io_data == NULL) {
		dev_err(db->dev, "failed to ioremap data reg\n");
		ret = -EINVAL;
		goto out;
	}

	/* fill in parameters for net-dev structure */
	ndev->base_addr = (unsigned long)db->io_addr;
	ndev->irq	= db->irq_res->start;

	/* ensure at least we have a default set of IO routines */
	dm9000_set_io(db, iosize);

	/* check to see if anything is being over-ridden */
	if (pdata != NULL) {
		/* check to see if the driver wants to over-ride the
		 * default IO width */

		if (pdata->flags & DM9000_PLATF_8BITONLY)
			dm9000_set_io(db, 1);

		if (pdata->flags & DM9000_PLATF_16BITONLY)
			dm9000_set_io(db, 2);

		if (pdata->flags & DM9000_PLATF_32BITONLY)
			dm9000_set_io(db, 4);

		/* check to see if there are any IO routine
		 * over-rides */

		if (pdata->inblk != NULL)
			db->inblk = pdata->inblk;

		if (pdata->outblk != NULL)
			db->outblk = pdata->outblk;

		if (pdata->dumpblk != NULL)
			db->dumpblk = pdata->dumpblk;

		db->flags = pdata->flags;
	}

#ifdef CONFIG_DM9000_FORCE_SIMPLE_PHY_POLL
	db->flags |= DM9000_PLATF_SIMPLE_PHY;
#endif

	dm9000_reset(db);

	/* try multiple times, DM9000 sometimes gets the read wrong */
	for (i = 0; i < 8; i++) {
		id_val  = ior(db, DM9000_VIDL);
		id_val |= (u32)ior(db, DM9000_VIDH) << 8;
		id_val |= (u32)ior(db, DM9000_PIDL) << 16;
		id_val |= (u32)ior(db, DM9000_PIDH) << 24;

		if (id_val == DM9000_ID)
			break;
		dev_err(db->dev, "read wrong id 0x%08x\n", id_val);
	}

	if (id_val != DM9000_ID) {
		dev_err(db->dev, "wrong id: 0x%08x\n", id_val);
		ret = -ENODEV;
		goto out;
	}

	/* Identify what type of DM9000 we are working on */

	/* I/O mode */
	db->io_mode = ior(db, DM9000_ISR) >> 6;	/* ISR bit7:6 keeps I/O mode */	
	id_val = ior(db, DM9000_CHIPR);
	dev_dbg(db->dev, "dm9000 revision 0x%02x  , io_mode %02x \n", id_val, db->io_mode);

	switch (id_val) {
	case CHIPR_DM9000A:
		db->type = TYPE_DM9000A;
		break;
	case 0x1a:
		db->type = TYPE_DM9000C;
		break;
	default:
		dev_dbg(db->dev, "ID %02x => defaulting to DM9000E\n", id_val);
		db->type = TYPE_DM9000E;
	}

	/* from this point we assume that we have found a DM9000 */

	/* driver system function */
	ether_setup(ndev);

	ndev->netdev_ops	= &dm9000_netdev_ops;
	ndev->watchdog_timeo	= msecs_to_jiffies(watchdog);
	ndev->ethtool_ops	= &dm9000_ethtool_ops;

	db->msg_enable       = NETIF_MSG_LINK;
	db->mii.phy_id_mask  = 0x1f;
	db->mii.reg_num_mask = 0x1f;
	db->mii.force_media  = 0;
	db->mii.full_duplex  = 0;
	db->mii.dev	     = ndev;
	db->mii.mdio_read    = dm9000_phy_read;
	db->mii.mdio_write   = dm9000_phy_write;

	mac_src = "eeprom";

	/* try reading the node address from the attached EEPROM */
	for (i = 0; i < 6; i += 2)
		dm9000_read_eeprom(db, i / 2, ndev->dev_addr+i);

	if (!is_valid_ether_addr(ndev->dev_addr) && pdata != NULL) {
		mac_src = "platform data";
		//memcpy(ndev->dev_addr, pdata->dev_addr, 6);
		/* mac from bootloader */
		memcpy(ndev->dev_addr, mac, 6);
	}

	if (!is_valid_ether_addr(ndev->dev_addr)) {
		/* try reading from mac */
		
		mac_src = "chip";
		for (i = 0; i < 6; i++)
			ndev->dev_addr[i] = ior(db, i+DM9000_PAR);
	}

	if (!is_valid_ether_addr(ndev->dev_addr))
		dev_warn(db->dev, "%s: Invalid ethernet MAC address. Please "
			 "set using ifconfig\n", ndev->name);

	platform_set_drvdata(pdev, ndev);
	ret = register_netdev(ndev);

	if (ret == 0)
		printk(KERN_INFO "%s: dm9000%c at %p,%p IRQ %d MAC: %pM (%s)\n",
		       ndev->name, dm9000_type_to_char(db->type),
		       db->io_addr, db->io_data, ndev->irq,
		       ndev->dev_addr, mac_src);
	return 0;

out:
	dev_err(db->dev, "not found (%d).\n", ret);

	dm9000_release_board(pdev, db);
	free_netdev(ndev);

	return ret;
}

static int
dm9000_drv_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct net_device *ndev = platform_get_drvdata(pdev);
	board_info_t *db;

	if (ndev) {
		db = netdev_priv(ndev);
		db->in_suspend = 1;

		if (!netif_running(ndev))
			return 0;

		netif_device_detach(ndev);

		/* only shutdown if not using WoL */
		if (!db->wake_state)
			dm9000_shutdown(ndev);
	}

	dm9000_power_on(0);
	return 0;
}

static int
dm9000_drv_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct net_device *ndev = platform_get_drvdata(pdev);
	board_info_t *db = netdev_priv(ndev);

	dm9000_power_on(1);

	if (ndev) {
		if (netif_running(ndev)) {
			/* reset if we were not in wake mode to ensure if
			 * the device was powered off it is in a known state */
			if (!db->wake_state) {
//				dm9000_reset(db);
				dm9000_init_dm9000(ndev);
			}

			netif_device_attach(ndev);
		}

		db->in_suspend = 0;
	}
	return 0;
}

static const struct dev_pm_ops dm9000_drv_pm_ops = {
	.suspend	= dm9000_drv_suspend,
	.resume		= dm9000_drv_resume,
};

static int __devexit
dm9000_drv_remove(struct platform_device *pdev)
{
	struct net_device *ndev = platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);

	unregister_netdev(ndev);
	dm9000_release_board(pdev, (board_info_t *) netdev_priv(ndev));
	free_netdev(ndev);		/* free device structure */

	dev_dbg(&pdev->dev, "released and freed device\n");
	return 0;
}

static struct platform_driver dm9000_driver = {
	.driver	= {
		.name    = "dm9000",
		.owner	 = THIS_MODULE,
		.pm	 = &dm9000_drv_pm_ops,
	},
	.probe   = dm9000_probe,
	.remove  = __devexit_p(dm9000_drv_remove),
};

static int __init
dm9000_init(void)
{
	/* disable buzzer */
	s3c_gpio_setpull(S5PV210_GPD0(2), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPD0(2), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPD0(2), 0);

	dm9000_power_int();
	printk(KERN_INFO "%s Ethernet Driver, V%s\n", CARDNAME, DRV_VERSION);

	return platform_driver_register(&dm9000_driver);
}

static void __exit
dm9000_cleanup(void)
{
	platform_driver_unregister(&dm9000_driver);
}

module_init(dm9000_init);
module_exit(dm9000_cleanup);

MODULE_AUTHOR("Sascha Hauer, Ben Dooks");
MODULE_DESCRIPTION("Davicom DM9000 network driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:dm9000");