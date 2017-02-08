/*
 * (C) Copyright 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Misc functions
 */
#include <common.h>
#include <command.h>

int do_sleep_msec( u32 delay )
{
	ulong start = get_timer(0);

	while (get_timer(start) < delay) {
		if (ctrlc ()) {
			return (-1);
		}
		udelay (100);
	}

	return 0;
}

int do_sleep (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong delay;

	if (argc != 2) {
		cmd_usage(cmdtp);
		return 1;
	}

	delay = simple_strtoul(argv[1], NULL, 10) * CONFIG_SYS_HZ;

	return do_sleep_msec( delay );
}

U_BOOT_CMD(
	sleep ,    2,    1,     do_sleep,
	"delay execution for some time",
	"N\n"
	"    - delay execution for N seconds (N is _decimal_ !!!)\n"
);


#ifdef CONFIG_DRIVER_VR9 /* ctc added for RTL8367RB */


int do_mdio (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	u8	phy;
	u8	reg;
	u16	data;
	u32	tmp;
	extern u16  vr9_mdio_read( u8 phyaddr, u8 phyreg );
	extern void vr9_mdio_write( u8 phyaddr, u8 phyreg, u16 data );

	if ( argc == 4 && strcmp(argv[1],"read") == 0)
	{
		//
		tmp = simple_strtoul(argv[2], NULL, 16);
		if ( tmp > 0x1f )
			goto do_mdio_err;
		phy = (u8)tmp;
		//
		tmp = simple_strtoul(argv[3], NULL, 16);
		if ( tmp > 0x1f )
			goto do_mdio_err;
		reg = (u8)tmp;
		//
		tmp = vr9_mdio_read( phy, reg );
		printf( "data: 0x%04x\n", tmp );
		return 0;
	}
	else if ( argc == 5 && strcmp(argv[1],"write") == 0)
	{
		//
		tmp = simple_strtoul(argv[2], NULL, 16);
		if ( tmp > 0x1f )
			goto do_mdio_err;
		phy = (u8)tmp;
		//
		tmp = simple_strtoul(argv[3], NULL, 16);
		if ( tmp > 0x1f )
			goto do_mdio_err;
		reg = (u8)tmp;
		//
		tmp = simple_strtoul(argv[4], NULL, 16);
		if ( tmp > 0xffff )
			goto do_mdio_err;
		data = (u16)tmp;
		//
		vr9_mdio_write( phy, reg, data );
		return 0;
	}
	else
	{
		goto do_mdio_err;
	}

	return 0;

do_mdio_err:

	cmd_usage(cmdtp);

	return 1;
}

U_BOOT_CMD(
	mdio ,    5,    1,     do_mdio,
	"mdio read and write",
	"read  <phy> <reg>        - read MDIO register, hexadecimal\n"
	"mdio write <phy> <reg> <data> - write MDIO register, hexadecimal\n"
	"                        <phy> - 0x00-0x1F\n"
	"                        <reg> - 0x00-0x1F\n"
	"                       <data> - 0x0000-0xFFFF\n"
);


int do_smi (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	u32	addr, data, tmp, retVal;
	extern int smi_read(unsigned int mAddrs, unsigned int *rData);
	extern int smi_write(unsigned int mAddrs, unsigned int rData);

	if (argc < 2)
		return -1;
	
	if(argc == 3 && strcmp(argv[1],"read") == 0)
	{
			tmp = simple_strtoul(argv[2], NULL, 16);
			if(tmp > 0xffff)
					goto do_smi_err;

			if(0 != (retVal = smi_read(tmp, &data)))
				printf("[%s:%d] smi read failed!!!\n", __func__,__LINE__);
			
			printf("data: 0x%04x\n", data);

			return 0;
	}
	else if(argc == 4 && strcmp(argv[1],"write") == 0)
	{
			tmp = simple_strtoul(argv[2], NULL, 16);
			if(tmp > 0xffff)
				goto do_smi_err;

			addr = (u16)tmp;

			tmp = simple_strtoul(argv[3], NULL, 16);
			if ( tmp > 0xffff )
				goto do_smi_err;

			data = (u16)tmp;

			if(0 != (retVal = smi_write(addr, data)))
				printf("[%s:%d] smi write failed!!!\n", __func__,__LINE__);

			return 0;
	}
	else
	{
			goto do_smi_err;
	}

	return 0;

do_smi_err:

	cmd_usage(cmdtp);

	return 1;
}

U_BOOT_CMD(
	smi ,    4,    1,     do_smi,
	"smi read and write",
	"read  <addr>        - read SMI register, hexadecimal\n"
	"smi write <addr> <data> - write SMI register, hexadecimal\n"
	"                 <addr> - 0x0000-0xFFFF\n"
	"                 <data> - 0x0000-0xFFFF\n"
);

int do_sw (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	u32		tmp;
	extern int rtl_sw_reset( u32 delay );
	extern int rtl_sw_init( void );
	extern void rtl_msleep( u32 delay );

	if(argc == 3 && strcmp(argv[1],"reset") == 0)
	{
			tmp = simple_strtoul(argv[2], NULL, 10)/* * (CFG_HZ / 1000)*/;
			rtl_sw_reset( tmp );
			return 0;
	}
	else if(argc == 2 && strcmp(argv[1],"init") == 0)
	{
			rtl_sw_init();
	}
	else if ( argc == 3 && strcmp(argv[1],"restart") == 0)
	{
			tmp = simple_strtoul(argv[2], NULL, 10)/* * (CFG_HZ / 1000)*/;
			rtl_sw_reset( 100 );
			rtl_msleep( tmp );
			rtl_sw_init();

			return 0;
	}
	else
	{
			goto do_sw_err;
	}

	return 0;

do_sw_err:

	printf ("Usage:\n%s\n", cmdtp->usage);

	return 1;
}

U_BOOT_CMD(
	sw ,    3,    1,     do_sw,
	"switch operation",
	"reset  <msec>  - reset switch (pull-high, pull-low, wait, pull-high)\n"
	"sw init           - init switch registers\n"
	"sw restart <msec> - restart switch (reset, wait, init)\n"
	"           <msec> - waiting time in milli-seconds, decimal\n"
);

#endif /* CONFIG_DRIVER_VR9 */
