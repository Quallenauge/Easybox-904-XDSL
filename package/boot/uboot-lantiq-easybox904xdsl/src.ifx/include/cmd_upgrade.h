/*
 * (C) Copyright 2000
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

// 509071:tc.che 2005/09/07 Reduced flash writing time.

//509071:tc.chen start
// the header of configuration data.
struct conf_header {
	unsigned long size;
	unsigned long crc;
};
//509061:tc.chen end

enum ExpandDir { FORWARD = 0, BACKWARD = 1 };
#if 1 //ctc
 #define MAX_PARTITION	20
 #define MAX_DATABLOCK	20
#else
 #define MAX_PARTITION	10
 #define MAX_DATABLOCK	16
#endif

#define UBOOTCONFIG_COPY_HEADER	"123456789ABCDEF"

#ifndef BUILD_FOR_KERNEL

int do_upgrade(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
int upgrade_img(ulong srcAddr,ulong srcLen,char *imgName,enum ExpandDir dir,int bSaveEnv);/*000001:Nirav*/

int saveenv_copy();

#endif
