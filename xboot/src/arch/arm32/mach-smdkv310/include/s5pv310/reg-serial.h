#ifndef __S5PV310_REG_SERIAL_H__
#define __S5PV310_REG_SERIAL_H__

#include <xboot.h>

/* uart 0 */
#define S5PV310_ULCON0	 	 	(0x13800000 + 0x0000)
#define S5PV310_UCON0	 	 	(0x13800000 + 0x0004)
#define S5PV310_UFCON0	 	 	(0x13800000 + 0x0008)
#define S5PV310_UMON0	 	 	(0x13800000 + 0x000C)
#define S5PV310_UTRSTAT0		(0x13800000 + 0x0010)
#define S5PV310_UERSTAT0		(0x13800000 + 0x0014)
#define S5PV310_UFSTAT0			(0x13800000 + 0x0018)
#define S5PV310_UMSTAT0			(0x13800000 + 0x001C)
#define S5PV310_UTXH0			(0x13800000 + 0x0020)
#define S5PV310_URXH0			(0x13800000 + 0x0024)
#define S5PV310_UBRDIV0			(0x13800000 + 0x0028)
#define S5PV310_UDIVSLOT0		(0x13800000 + 0x002C)
#define S5PV310_UINTP0			(0x13800000 + 0x0030)
#define S5PV310_UINTSP0			(0x13800000 + 0x0034)
#define S5PV310_UINTM0			(0x13800000 + 0x0038)

/* uart 1 */
#define S5PV310_ULCON1	 	 	(0x13810000 + 0x0000)
#define S5PV310_UCON1	 	 	(0x13810000 + 0x0004)
#define S5PV310_UFCON1	 	 	(0x13810000 + 0x0008)
#define S5PV310_UMON1	 	 	(0x13810000 + 0x000C)
#define S5PV310_UTRSTAT1		(0x13810000 + 0x0010)
#define S5PV310_UERSTAT1		(0x13810000 + 0x0014)
#define S5PV310_UFSTAT1			(0x13810000 + 0x0018)
#define S5PV310_UMSTAT1			(0x13810000 + 0x001C)
#define S5PV310_UTXH1			(0x13810000 + 0x0020)
#define S5PV310_URXH1			(0x13810000 + 0x0024)
#define S5PV310_UBRDIV1			(0x13810000 + 0x0028)
#define S5PV310_UDIVSLOT1		(0x13810000 + 0x002C)
#define S5PV310_UINTP1			(0x13810000 + 0x0030)
#define S5PV310_UINTSP1			(0x13810000 + 0x0034)
#define S5PV310_UINTM1			(0x13810000 + 0x0038)

/* uart 2 */
#define S5PV310_ULCON2	 	 	(0x13820000 + 0x0000)
#define S5PV310_UCON2	 	 	(0x13820000 + 0x0004)
#define S5PV310_UFCON2	 	 	(0x13820000 + 0x0008)
#define S5PV310_UMON2	 	 	(0x13820000 + 0x000C)
#define S5PV310_UTRSTAT2		(0x13820000 + 0x0010)
#define S5PV310_UERSTAT2		(0x13820000 + 0x0014)
#define S5PV310_UFSTAT2			(0x13820000 + 0x0018)
#define S5PV310_UMSTAT2			(0x13820000 + 0x001C)
#define S5PV310_UTXH2			(0x13820000 + 0x0020)
#define S5PV310_URXH2			(0x13820000 + 0x0024)
#define S5PV310_UBRDIV2			(0x13820000 + 0x0028)
#define S5PV310_UDIVSLOT2		(0x13820000 + 0x002C)
#define S5PV310_UINTP2			(0x13820000 + 0x0030)
#define S5PV310_UINTSP2			(0x13820000 + 0x0034)
#define S5PV310_UINTM2			(0x13820000 + 0x0038)

/* uart 3 */
#define S5PV310_ULCON3	 	 	(0x13830000 + 0x0000)
#define S5PV310_UCON3	 	 	(0x13830000 + 0x0004)
#define S5PV310_UFCON3	 	 	(0x13830000 + 0x0008)
#define S5PV310_UMON3	 	 	(0x13830000 + 0x000C)
#define S5PV310_UTRSTAT3		(0x13830000 + 0x0010)
#define S5PV310_UERSTAT3		(0x13830000 + 0x0014)
#define S5PV310_UFSTAT3			(0x13830000 + 0x0018)
#define S5PV310_UMSTAT3			(0x13830000 + 0x001C)
#define S5PV310_UTXH3			(0x13830000 + 0x0020)
#define S5PV310_URXH3			(0x13830000 + 0x0024)
#define S5PV310_UBRDIV3			(0x13830000 + 0x0028)
#define S5PV310_UDIVSLOT3		(0x13830000 + 0x002C)
#define S5PV310_UINTP3			(0x13830000 + 0x0030)
#define S5PV310_UINTSP3			(0x13830000 + 0x0034)
#define S5PV310_UINTM3			(0x13830000 + 0x0038)

/* uart 4 */
#define S5PV310_ULCON4	 	 	(0x13840000 + 0x0000)
#define S5PV310_UCON4	 	 	(0x13840000 + 0x0004)
#define S5PV310_UFCON4	 	 	(0x13840000 + 0x0008)
#define S5PV310_UMON4	 	 	(0x13840000 + 0x000C)
#define S5PV310_UTRSTAT4		(0x13840000 + 0x0010)
#define S5PV310_UERSTAT4		(0x13840000 + 0x0014)
#define S5PV310_UFSTAT4			(0x13840000 + 0x0018)
#define S5PV310_UMSTAT4			(0x13840000 + 0x001C)
#define S5PV310_UTXH4			(0x13840000 + 0x0020)
#define S5PV310_URXH4			(0x13840000 + 0x0024)
#define S5PV310_UBRDIV4			(0x13840000 + 0x0028)
#define S5PV310_UDIVSLOT4		(0x13840000 + 0x002C)
#define S5PV310_UINTP4			(0x13840000 + 0x0030)
#define S5PV310_UINTSP4			(0x13840000 + 0x0034)
#define S5PV310_UINTM4			(0x13840000 + 0x0038)


#define S5PV310_UFSTAT_TXFULL	(1<<24)
#define S5PV310_UFSTAT_RXFULL	(1<<8)
#define S5PV310_UFSTAT_TXCOUNT	(0xFF<<16)
#define S5PV310_UFSTAT_RXCOUNT	(0xFF<<0)

#define S5PV310_UTRSTAT_TXE	  	(1<<2)
#define S5PV310_UTRSTAT_TXFE	(1<<1)
#define S5PV310_UTRSTAT_RXDR	(1<<0)

#define S5PV310_UERSTAT_OVERRUN	(1<<0)
#define S5PV310_UERSTAT_PARITY	(1<<1)
#define S5PV310_UERSTAT_FRAME	(1<<2)
#define S5PV310_UERSTAT_BREAK	(1<<3)

#define S5PV310_UMSTAT_CTS	  	(1<<0)
#define S5PV310_UMSTAT_DCTS		(1<<4)


#endif /* __S5PV310_REG_SERIAL_H__ */
