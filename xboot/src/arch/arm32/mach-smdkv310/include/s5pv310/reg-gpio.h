#ifndef __S5PV310_REG_GPIO_H__
#define __S5PV310_REG_GPIO_H__

#include <xboot.h>

/*
 * PART 1, Base Address = 0x11400000
 */
/* PORT A0 */
#define S5PV310_GPA0CON	 	 	(0x11400000 + 0x000)
#define S5PV310_GPA0DAT	 	 	(0x11400000 + 0x004)
#define S5PV310_GPA0PUD	 	 	(0x11400000 + 0x008)
#define S5PV310_GPA0DRV	 	 	(0x11400000 + 0x00C)
#define S5PV310_GPA0CONPDN 	 	(0x11400000 + 0x010)
#define S5PV310_GPA0PUDPDN 	 	(0x11400000 + 0x014)

/* PORT A1 */
#define S5PV310_GPA1CON	 	 	(0x11400000 + 0x020)
#define S5PV310_GPA1DAT	 	 	(0x11400000 + 0x024)
#define S5PV310_GPA1PUD	 	 	(0x11400000 + 0x028)
#define S5PV310_GPA1DRV	 	 	(0x11400000 + 0x02C)
#define S5PV310_GPA1CONPDN 	 	(0x11400000 + 0x030)
#define S5PV310_GPA1PUDPDN 	 	(0x11400000 + 0x034)

/* PORT B */
#define S5PV310_GPBCON	 	 	(0x11400000 + 0x040)
#define S5PV310_GPBDAT	 	 	(0x11400000 + 0x044)
#define S5PV310_GPBPUD	 	 	(0x11400000 + 0x048)
#define S5PV310_GPBDRV	 	 	(0x11400000 + 0x04C)
#define S5PV310_GPBCONPDN 	 	(0x11400000 + 0x050)
#define S5PV310_GPBPUDPDN 	 	(0x11400000 + 0x054)

/* PORT C0 */
#define S5PV310_GPC0CON	 	 	(0x11400000 + 0x060)
#define S5PV310_GPC0DAT	 	 	(0x11400000 + 0x064)
#define S5PV310_GPC0PUD	 	 	(0x11400000 + 0x068)
#define S5PV310_GPC0DRV	 	 	(0x11400000 + 0x06C)
#define S5PV310_GPC0CONPDN 	 	(0x11400000 + 0x070)
#define S5PV310_GPC0PUDPDN 	 	(0x11400000 + 0x074)

/* PORT C1 */
#define S5PV310_GPC1CON	 	 	(0x11400000 + 0x080)
#define S5PV310_GPC1DAT	 	 	(0x11400000 + 0x084)
#define S5PV310_GPC1PUD	 	 	(0x11400000 + 0x088)
#define S5PV310_GPC1DRV	 	 	(0x11400000 + 0x08C)
#define S5PV310_GPC1CONPDN 	 	(0x11400000 + 0x090)
#define S5PV310_GPC1PUDPDN 	 	(0x11400000 + 0x094)

/* PORT D0 */
#define S5PV310_GPD0CON	 	 	(0x11400000 + 0x0A0)
#define S5PV310_GPD0DAT	 	 	(0x11400000 + 0x0A4)
#define S5PV310_GPD0PUD	 	 	(0x11400000 + 0x0A8)
#define S5PV310_GPD0DRV	 	 	(0x11400000 + 0x0AC)
#define S5PV310_GPD0CONPDN 	 	(0x11400000 + 0x0B0)
#define S5PV310_GPD0PUDPDN 	 	(0x11400000 + 0x0B4)

/* PORT D1 */
#define S5PV310_GPD1CON	 	 	(0x11400000 + 0x0C0)
#define S5PV310_GPD1DAT	 	 	(0x11400000 + 0x0C4)
#define S5PV310_GPD1PUD	 	 	(0x11400000 + 0x0C8)
#define S5PV310_GPD1DRV	 	 	(0x11400000 + 0x0CC)
#define S5PV310_GPD1CONPDN 	 	(0x11400000 + 0x0D0)
#define S5PV310_GPD1PUDPDN 	 	(0x11400000 + 0x0D4)

/* PORT E0 */
#define S5PV310_GPE0CON	 	 	(0x11400000 + 0x0E0)
#define S5PV310_GPE0DAT	 	 	(0x11400000 + 0x0E4)
#define S5PV310_GPE0PUD	 	 	(0x11400000 + 0x0E8)
#define S5PV310_GPE0DRV	 	 	(0x11400000 + 0x0EC)
#define S5PV310_GPE0CONPDN 	 	(0x11400000 + 0x0F0)
#define S5PV310_GPE0PUDPDN 	 	(0x11400000 + 0x0F4)

/* PORT E1 */
#define S5PV310_GPE1CON	 	 	(0x11400000 + 0x100)
#define S5PV310_GPE1DAT	 	 	(0x11400000 + 0x104)
#define S5PV310_GPE1PUD	 	 	(0x11400000 + 0x108)
#define S5PV310_GPE1DRV	 	 	(0x11400000 + 0x10C)
#define S5PV310_GPE1CONPDN 	 	(0x11400000 + 0x110)
#define S5PV310_GPE1PUDPDN 	 	(0x11400000 + 0x114)

/* PORT E2 */
#define S5PV310_GPE2CON	 	 	(0x11400000 + 0x120)
#define S5PV310_GPE2DAT	 	 	(0x11400000 + 0x124)
#define S5PV310_GPE2PUD	 	 	(0x11400000 + 0x128)
#define S5PV310_GPE2DRV	 	 	(0x11400000 + 0x12C)
#define S5PV310_GPE2CONPDN 	 	(0x11400000 + 0x130)
#define S5PV310_GPE2PUDPDN 	 	(0x11400000 + 0x134)

/* PORT E3 */
#define S5PV310_GPE3CON	 	 	(0x11400000 + 0x140)
#define S5PV310_GPE3DAT	 	 	(0x11400000 + 0x144)
#define S5PV310_GPE3PUD	 	 	(0x11400000 + 0x148)
#define S5PV310_GPE3DRV	 	 	(0x11400000 + 0x14C)
#define S5PV310_GPE3CONPDN 	 	(0x11400000 + 0x150)
#define S5PV310_GPE3PUDPDN 	 	(0x11400000 + 0x154)

/* PORT E4 */
#define S5PV310_GPE4CON	 	 	(0x11400000 + 0x160)
#define S5PV310_GPE4DAT	 	 	(0x11400000 + 0x164)
#define S5PV310_GPE4PUD	 	 	(0x11400000 + 0x168)
#define S5PV310_GPE4DRV	 	 	(0x11400000 + 0x16C)
#define S5PV310_GPE4CONPDN 	 	(0x11400000 + 0x170)
#define S5PV310_GPE4PUDPDN 	 	(0x11400000 + 0x174)

/* PORT F0 */
#define S5PV310_GPF0CON	 	 	(0x11400000 + 0x180)
#define S5PV310_GPF0DAT	 	 	(0x11400000 + 0x184)
#define S5PV310_GPF0PUD	 	 	(0x11400000 + 0x188)
#define S5PV310_GPF0DRV	 	 	(0x11400000 + 0x18C)
#define S5PV310_GPF0CONPDN 	 	(0x11400000 + 0x190)
#define S5PV310_GPF0PUDPDN 	 	(0x11400000 + 0x194)

/* PORT F1 */
#define S5PV310_GPF1CON	 	 	(0x11400000 + 0x1A0)
#define S5PV310_GPF1DAT	 	 	(0x11400000 + 0x1A4)
#define S5PV310_GPF1PUD	 	 	(0x11400000 + 0x1A8)
#define S5PV310_GPF1DRV	 	 	(0x11400000 + 0x1AC)
#define S5PV310_GPF1CONPDN 	 	(0x11400000 + 0x1B0)
#define S5PV310_GPF1PUDPDN 	 	(0x11400000 + 0x1B4)

/* PORT F2 */
#define S5PV310_GPF2CON	 	 	(0x11400000 + 0x1C0)
#define S5PV310_GPF2DAT	 	 	(0x11400000 + 0x1C4)
#define S5PV310_GPF2PUD	 	 	(0x11400000 + 0x1C8)
#define S5PV310_GPF2DRV	 	 	(0x11400000 + 0x1CC)
#define S5PV310_GPF2CONPDN 	 	(0x11400000 + 0x1D0)
#define S5PV310_GPF2PUDPDN 	 	(0x11400000 + 0x1D4)

/* PORT F3 */
#define S5PV310_GPF3CON	 	 	(0x11400000 + 0x1E0)
#define S5PV310_GPF3DAT	 	 	(0x11400000 + 0x1E4)
#define S5PV310_GPF3PUD	 	 	(0x11400000 + 0x1E8)
#define S5PV310_GPF3DRV	 	 	(0x11400000 + 0x1EC)
#define S5PV310_GPF3CONPDN 	 	(0x11400000 + 0x1F0)
#define S5PV310_GPF3PUDPDN 	 	(0x11400000 + 0x1F4)

/* PORT ETC0 */
#define S5PV310_ETC0PUD	 	 	(0x11400000 + 0x208)
#define S5PV310_ETC0DRV		 	(0x11400000 + 0x20C)

/* PORT ETC1 */
#define S5PV310_ETC1PUD	 	 	(0x11400000 + 0x228)
#define S5PV310_ETC1DRV		 	(0x11400000 + 0x22C)

/*
 * PART 2, Base Address = 0x11000000
 */
/* PORT J0 */
#define S5PV310_GPJ0CON	 	 	(0x11000000 + 0x000)
#define S5PV310_GPJ0DAT	 	 	(0x11000000 + 0x004)
#define S5PV310_GPJ0PUD	 	 	(0x11000000 + 0x008)
#define S5PV310_GPJ0DRV	 	 	(0x11000000 + 0x00C)
#define S5PV310_GPJ0CONPDN 	 	(0x11000000 + 0x010)
#define S5PV310_GPJ0PUDPDN 	 	(0x11000000 + 0x014)

/* PORT J1 */
#define S5PV310_GPJ1CON	 	 	(0x11000000 + 0x020)
#define S5PV310_GPJ1DAT	 	 	(0x11000000 + 0x024)
#define S5PV310_GPJ1PUD	 	 	(0x11000000 + 0x028)
#define S5PV310_GPJ1DRV	 	 	(0x11000000 + 0x02C)
#define S5PV310_GPJ1CONPDN 	 	(0x11000000 + 0x030)
#define S5PV310_GPJ1PUDPDN 	 	(0x11000000 + 0x034)

/* PORT K0 */
#define S5PV310_GPK0CON	 	 	(0x11000000 + 0x040)
#define S5PV310_GPK0DAT	 	 	(0x11000000 + 0x044)
#define S5PV310_GPK0PUD	 	 	(0x11000000 + 0x048)
#define S5PV310_GPK0DRV	 	 	(0x11000000 + 0x04C)
#define S5PV310_GPK0CONPDN 	 	(0x11000000 + 0x050)
#define S5PV310_GPK0PUDPDN 	 	(0x11000000 + 0x054)

/* PORT K1 */
#define S5PV310_GPK1CON	 	 	(0x11000000 + 0x060)
#define S5PV310_GPK1DAT	 	 	(0x11000000 + 0x064)
#define S5PV310_GPK1PUD	 	 	(0x11000000 + 0x068)
#define S5PV310_GPK1DRV	 	 	(0x11000000 + 0x06C)
#define S5PV310_GPK1CONPDN 	 	(0x11000000 + 0x070)
#define S5PV310_GPK1PUDPDN 	 	(0x11000000 + 0x074)

/* PORT K2 */
#define S5PV310_GPK2CON	 	 	(0x11000000 + 0x080)
#define S5PV310_GPK2DAT	 	 	(0x11000000 + 0x084)
#define S5PV310_GPK2PUD	 	 	(0x11000000 + 0x088)
#define S5PV310_GPK2DRV	 	 	(0x11000000 + 0x08C)
#define S5PV310_GPK2CONPDN 	 	(0x11000000 + 0x090)
#define S5PV310_GPK2PUDPDN 	 	(0x11000000 + 0x094)

/* PORT K3 */
#define S5PV310_GPK3CON	 	 	(0x11000000 + 0x0A0)
#define S5PV310_GPK3DAT	 	 	(0x11000000 + 0x0A4)
#define S5PV310_GPK3PUD	 	 	(0x11000000 + 0x0A8)
#define S5PV310_GPK3DRV	 	 	(0x11000000 + 0x0AC)
#define S5PV310_GPK3CONPDN 	 	(0x11000000 + 0x0B0)
#define S5PV310_GPK3PUDPDN 	 	(0x11000000 + 0x0B4)

/* PORT L0 */
#define S5PV310_GPL0CON	 	 	(0x11000000 + 0x0C0)
#define S5PV310_GPL0DAT	 	 	(0x11000000 + 0x0C4)
#define S5PV310_GPL0PUD	 	 	(0x11000000 + 0x0C8)
#define S5PV310_GPL0DRV	 	 	(0x11000000 + 0x0CC)
#define S5PV310_GPL0CONPDN 	 	(0x11000000 + 0x0D0)
#define S5PV310_GPL0PUDPDN 	 	(0x11000000 + 0x0D4)

/* PORT L1 */
#define S5PV310_GPL1CON	 	 	(0x11000000 + 0x0E0)
#define S5PV310_GPL1DAT	 	 	(0x11000000 + 0x0E4)
#define S5PV310_GPL1PUD	 	 	(0x11000000 + 0x0E8)
#define S5PV310_GPL1DRV	 	 	(0x11000000 + 0x0EC)
#define S5PV310_GPL1CONPDN 	 	(0x11000000 + 0x0F0)
#define S5PV310_GPL1PUDPDN 	 	(0x11000000 + 0x0F4)

/* PORT L2 */
#define S5PV310_GPL2CON	 	 	(0x11000000 + 0x100)
#define S5PV310_GPL2DAT	 	 	(0x11000000 + 0x104)
#define S5PV310_GPL2PUD	 	 	(0x11000000 + 0x108)
#define S5PV310_GPL2DRV	 	 	(0x11000000 + 0x10C)
#define S5PV310_GPL2CONPDN 	 	(0x11000000 + 0x110)
#define S5PV310_GPL2PUDPDN 	 	(0x11000000 + 0x114)

/* PORT Y0 */
#define S5PV310_GPY0CON	 	 	(0x11000000 + 0x120)
#define S5PV310_GPY0DAT	 	 	(0x11000000 + 0x124)
#define S5PV310_GPY0PUD	 	 	(0x11000000 + 0x128)
#define S5PV310_GPY0DRV	 	 	(0x11000000 + 0x12C)
#define S5PV310_GPY0CONPDN 	 	(0x11000000 + 0x130)
#define S5PV310_GPY0PUDPDN 	 	(0x11000000 + 0x134)

/* PORT Y1 */
#define S5PV310_GPY1CON	 	 	(0x11000000 + 0x140)
#define S5PV310_GPY1DAT	 	 	(0x11000000 + 0x144)
#define S5PV310_GPY1PUD	 	 	(0x11000000 + 0x148)
#define S5PV310_GPY1DRV	 	 	(0x11000000 + 0x14C)
#define S5PV310_GPY1CONPDN 	 	(0x11000000 + 0x150)
#define S5PV310_GPY1PUDPDN 	 	(0x11000000 + 0x154)

/* PORT Y2 */
#define S5PV310_GPY2CON	 	 	(0x11000000 + 0x160)
#define S5PV310_GPY2DAT	 	 	(0x11000000 + 0x164)
#define S5PV310_GPY2PUD	 	 	(0x11000000 + 0x168)
#define S5PV310_GPY2DRV	 	 	(0x11000000 + 0x16C)
#define S5PV310_GPY2CONPDN 	 	(0x11000000 + 0x170)
#define S5PV310_GPY2PUDPDN 	 	(0x11000000 + 0x174)

/* PORT Y3 */
#define S5PV310_GPY3CON	 	 	(0x11000000 + 0x180)
#define S5PV310_GPY3DAT	 	 	(0x11000000 + 0x184)
#define S5PV310_GPY3PUD	 	 	(0x11000000 + 0x188)
#define S5PV310_GPY3DRV	 	 	(0x11000000 + 0x18C)
#define S5PV310_GPY3CONPDN 	 	(0x11000000 + 0x190)
#define S5PV310_GPY3PUDPDN 	 	(0x11000000 + 0x194)

/* PORT Y4 */
#define S5PV310_GPY4CON	 	 	(0x11000000 + 0x1A0)
#define S5PV310_GPY4DAT	 	 	(0x11000000 + 0x1A4)
#define S5PV310_GPY4PUD	 	 	(0x11000000 + 0x1A8)
#define S5PV310_GPY4DRV	 	 	(0x11000000 + 0x1AC)
#define S5PV310_GPY4CONPDN 	 	(0x11000000 + 0x1B0)
#define S5PV310_GPY4PUDPDN 	 	(0x11000000 + 0x1B4)

/* PORT Y5 */
#define S5PV310_GPY5CON	 	 	(0x11000000 + 0x1C0)
#define S5PV310_GPY5DAT	 	 	(0x11000000 + 0x1C4)
#define S5PV310_GPY5PUD	 	 	(0x11000000 + 0x1C8)
#define S5PV310_GPY5DRV	 	 	(0x11000000 + 0x1CC)
#define S5PV310_GPY5CONPDN 	 	(0x11000000 + 0x1D0)
#define S5PV310_GPY5PUDPDN 	 	(0x11000000 + 0x1D4)

/* PORT Y6 */
#define S5PV310_GPY6CON	 	 	(0x11000000 + 0x1E0)
#define S5PV310_GPY6DAT	 	 	(0x11000000 + 0x1E4)
#define S5PV310_GPY6PUD	 	 	(0x11000000 + 0x1E8)
#define S5PV310_GPY6DRV	 	 	(0x11000000 + 0x1EC)
#define S5PV310_GPY6CONPDN 	 	(0x11000000 + 0x1F0)
#define S5PV310_GPY6PUDPDN 	 	(0x11000000 + 0x1F4)

/* PORT ETC6 */
#define S5PV310_ETC6PUD	 	 	(0x11000000 + 0x208)
#define S5PV310_ETC6DRV		 	(0x11000000 + 0x20C)

/* PORT ETC8 */
#define S5PV310_ETC8PUD	 	 	(0x11000000 + 0x228)
#define S5PV310_ETC8DRV		 	(0x11000000 + 0x22C)

/* PORT X0 */
#define S5PV310_GPX0CON	 	 	(0x11000000 + 0xC00)
#define S5PV310_GPX0DAT	 	 	(0x11000000 + 0xC04)
#define S5PV310_GPX0PUD	 	 	(0x11000000 + 0xC08)
#define S5PV310_GPX0DRV	 	 	(0x11000000 + 0xC0C)

/* PORT X1 */
#define S5PV310_GPX1CON	 	 	(0x11000000 + 0xC20)
#define S5PV310_GPX1DAT	 	 	(0x11000000 + 0xC24)
#define S5PV310_GPX1PUD	 	 	(0x11000000 + 0xC28)
#define S5PV310_GPX1DRV	 	 	(0x11000000 + 0xC2C)

/* PORT X2 */
#define S5PV310_GPX2CON	 	 	(0x11000000 + 0xC40)
#define S5PV310_GPX2DAT	 	 	(0x11000000 + 0xC44)
#define S5PV310_GPX2PUD	 	 	(0x11000000 + 0xC48)
#define S5PV310_GPX2DRV	 	 	(0x11000000 + 0xC4C)

/* PORT X3 */
#define S5PV310_GPX3CON	 	 	(0x11000000 + 0xC60)
#define S5PV310_GPX3DAT	 	 	(0x11000000 + 0xC64)
#define S5PV310_GPX3PUD	 	 	(0x11000000 + 0xC68)
#define S5PV310_GPX3DRV	 	 	(0x11000000 + 0xC6C)

/*
 * PART 3, Base Address = 0x03860000
 */
/* PORT Z */
#define S5PV310_GPZCON	 	 	(0x03860000 + 0x000)
#define S5PV310_GPZDAT	 	 	(0x03860000 + 0x004)
#define S5PV310_GPZPUD	 	 	(0x03860000 + 0x008)
#define S5PV310_GPZDRV	 	 	(0x03860000 + 0x00C)
#define S5PV310_GPZCONPDN 	 	(0x03860000 + 0x010)
#define S5PV310_GPZPUDPDN 	 	(0x03860000 + 0x014)

#endif /* __S5PV310_REG_GPIO_H__ */
