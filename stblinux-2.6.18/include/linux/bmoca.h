#ifndef _BMOCA_H_
#define _BMOCA_H_

#include <linux/if.h>
#include <linux/types.h>
#include <linux/ioctl.h>

/* NOTE: These need to match what is defined in the API template */
#define MOCA_IE_DRV_PRINTF	0xff00
#define MOCA_IE_WDT		0xff01

#define MOCA_IOC_MAGIC		'M'

#define MOCA_IOCTL_GET_DRV_INFO	_IOR(MOCA_IOC_MAGIC, 0, struct moca_kdrv_info)
#define MOCA_IOCTL_START	_IOW(MOCA_IOC_MAGIC, 1, struct moca_xfer)
#define MOCA_IOCTL_STOP		_IO(MOCA_IOC_MAGIC, 2)
#define MOCA_IOCTL_READMEM	_IOR(MOCA_IOC_MAGIC, 3, struct moca_xfer)

struct moca_kdrv_info {
	uint32_t		version;
	uint32_t		uptime;
	int32_t			refcount;
	uint32_t		gp1;

	char			enet_name[IFNAMSIZ];
	uint32_t		enet_id;

	uint32_t		macaddr_hi;
	uint32_t		macaddr_lo;
};

struct moca_xfer {
	void			*buf;
	uint32_t		len;
	uint32_t		moca_addr;
};

#ifdef __KERNEL__

static inline void mac_to_u32(uint32_t *hi, uint32_t *lo, const uint8_t *mac)
{
	*hi = (mac[0] << 24) | (mac[1] << 16) | (mac[2] << 8) | (mac[3] << 0);
	*lo = (mac[4] << 24) | (mac[5] << 16);
}

static inline void u32_to_mac(uint8_t *mac, uint32_t hi, uint32_t lo)
{
	mac[0] = (hi >> 24) & 0xff;
	mac[1] = (hi >> 16) & 0xff;
	mac[2] = (hi >>  8) & 0xff;
	mac[3] = (hi >>  0) & 0xff;
	mac[4] = (lo >> 24) & 0xff;
	mac[5] = (lo >> 16) & 0xff;
}

struct moca_platform_data {
	char			enet_name[IFNAMSIZ];
	unsigned int		enet_id;

	uint32_t		macaddr_hi;
	uint32_t		macaddr_lo;

	phys_t			bcm3450_i2c_base;
	int			bcm3450_i2c_addr;
};

#endif /* __KERNEL__ */

#endif /* ! _BMOCA_H_ */
