#
# (C) Copyright 2006-2007
# Stefan Roese, DENX Software Engineering, sr@denx.de.
#
# (C) Copyright 2008
# Guennadi Liakhovetki, DENX Software Engineering, <lg@denx.de>
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

CONFIG_NAND_SPL	= y

include $(TOPDIR)/config.mk
include $(TOPDIR)/nand_spl/board/lantiq/config.mk

LDSCRIPT= $(TOPDIR)/nand_spl/board/lantiq/u-boot.lds
LDFLAGS	= -Bstatic -T $(nandobj)u-boot.lds -Ttext $(CONFIG_NAND_SPL_TEXT_BASE) $(PLATFORM_LDFLAGS)
AFLAGS	+=-mabicalls -fpic -DCONFIG_NAND_SPL -I$(TOPDIR)/board/$(BOARD)/
CFLAGS	+=-mabicalls -fpic -DCONFIG_NAND_SPL -I$(TOPDIR)/board/$(BOARD)/

SOBJS	= start.o lowlevel_init.o 
COBJS	= nand_boot.o nand_ecc.o lq_nand.o board.o asc.o clock.o tune_ddr.o string.o time.o

SRCS	:= $(addprefix $(obj),$(SOBJS:.o=.S) $(COBJS:.o=.c))
OBJS	:= $(addprefix $(obj),$(SOBJS) $(COBJS))
__OBJS	:= $(SOBJS) $(COBJS)
LNDIR	:= $(OBJTREE)/nand_spl/board/lantiq

nandobj	:= $(OBJTREE)/nand_spl/board/lantiq

ALL	= $(nandobj)u-boot-spl $(nandobj)u-boot-spl.bin $(nandobj)u-boot-spl-16k.bin nandpreload.bin

all:	$(obj).depend $(ALL)
	@rm -f u-boot-spl.bin u-boot-spl-16k.bin 
	@ln -s $(nandobj)u-boot-spl.bin $(TOPDIR)/nand_spl/u-boot-spl.bin
	@ln -s $(nandobj)u-boot-spl-16k.bin $(TOPDIR)/nand_spl/u-boot-spl-16k.bin

LDFLAGS_NANDPRELOAD = -Bstatic -T nandpreload.lds -Ttext $(CONFIG_NAND_PRELOAD_TEXT_BASE)
OBJS_NANDPRELOAD =  start_nandpreload.o
OBJS_NANDPRELOAD += nand_preload.o

nandpreload.bin: nandpreload
	$(OBJCOPY) ${OBJCFLAGS} -O binary $< $@

nandpreload:  $(OBJS_NANDPRELOAD)
	$(LD) $(LDFLAGS_NANDPRELOAD) $(OBJS_NANDPRELOAD) -o nandpreload
				

$(nandobj)u-boot-spl-16k.bin: $(nandobj)u-boot-spl.bin nandpreload.bin
	touch dummy
	$(TOPDIR)/scripts_platform/mk_sf.pl dummy nandpreload.bin $(CONFIG_NAND_PRELOAD_TEXT_BASE) $(nandobj)u-boot-spl-16k.bin
	$(TOPDIR)/scripts_platform/pad2align.sh -n 512 $(nandobj)u-boot-spl-16k.bin
	cat $(nandobj)u-boot-spl.bin >> $(nandobj)u-boot-spl-16k.bin
	$(TOPDIR)/scripts_platform/pad2align.sh -n 16384 $(nandobj)u-boot-spl-16k.bin 

$(nandobj)u-boot-spl.bin:	$(nandobj)u-boot-spl
	$(OBJCOPY) ${OBJCFLAGS} -O binary $< $@

$(nandobj)u-boot-spl:	$(OBJS) $(nandobj)u-boot.lds
	cd $(LNDIR) && $(LD) $(LDFLAGS) $(__OBJS) \
		-Map $(nandobj)u-boot-spl.map \
		-o $(nandobj)u-boot-spl

$(nandobj)u-boot.lds: $(LDSCRIPT)
	$(CPP) $(CPPFLAGS) $(LDPPFLAGS) -ansi -D__ASSEMBLY__ -P - <$^ >$@

# create symbolic links for common files

# from cpu directory
$(obj)start.S:
	@rm -f $@
	@ln -s $(TOPDIR)/arch/mips/cpu/$(BOARD)/start_nand_spl.S $@
	
clock.c:
	@rm -f $@
	@ln -s $(TOPDIR)/arch/mips/cpu/$(BOARD)/clock.c $@

asc.c:
	@rm -f $@
	@ln -s $(TOPDIR)/drivers/serial/lq_asc.c $@

# from board directory
$(obj)lowlevel_init.S:
	@rm -f $@
	@ln -s $(TOPDIR)/board/$(BOARD)/lowlevel_init.S $@


board.c:
	@rm -f $@
	@ln -s $(TOPDIR)/board/$(BOARD)/nand_spl_board.c $@
		

#from lib_bootstrap directory
tune_ddr.c:
	@rm -f $@
	@ln -s $(TOPDIR)/lib_bootstrap/tune_ddr.c $@


# from nand_spl directory
$(obj)nand_boot.c:
	@rm -f $@
	@ln -s $(TOPDIR)/nand_spl/nand_boot.c $@

# from drivers/mtd/nand directory
$(obj)nand_ecc.c:
	@rm -f $@
	@ln -s $(TOPDIR)/drivers/mtd/nand/nand_ecc.c $@

$(obj)lq_nand.c:
	@rm -f $@
	@ln -s $(TOPDIR)/drivers/mtd/nand/lq_nand.c $@


#from lib directory
string.c:
	@rm -f $@
	@ln -s $(TOPDIR)/lib/string.c $@

time.c:
	@rm -f $@
	@ln -s $(TOPDIR)/arch/mips/lib/time.c $@
		

#########################################################################

$(obj)%.o:	$(obj)%.S
	$(CC) $(AFLAGS) -c -o $@ $<

$(obj)%.o:	$(obj)%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
