#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 1995, 1998, 2001, 2002 by Ralf Baechle
# Copyright (C) 2004  Maciej W. Rozycki
#

#
# Some DECstations need all possible sections of an ECOFF executable
#
ifdef CONFIG_MACH_DECSTATION
  E2EFLAGS = -a
else
  E2EFLAGS =
endif

#
# Drop some uninteresting sections in the kernel.
# This is only relevant for ELF kernels but doesn't hurt a.out
#
drop-sections	= .reginfo .mdebug .comment .note .pdr .options .MIPS.options
strip-flags	= $(addprefix --remove-section=,$(drop-sections))

VMLINUX = vmlinux

all: vmlinux.ecoff vmlinux.srec addinitrd

TOPDIR ?= $(srctree)

piggy.gzip: $(CONFIGURE) $(VMLINUX)
	$(Q)$(OBJCOPY) -O binary -R .note -R .comment -S $(TOPDIR)/vmlinux piggy
	$(Q)gzip -9 < piggy > piggy.gzip

piggy.bz2: $(CONFIGURE) $(VMLINUX)
	$(Q)$(OBJCOPY) -O binary -R .note -R .comment -S $(TOPDIR)/vmlinux piggy
	$(Q)bzip2 -9 < piggy > piggy.bz2

piggy.lzma: $(CONFIGURE) $(VMLINUX)
	$(Q)$(OBJCOPY) -O binary -R .note -R .comment -S $(TOPDIR)/vmlinux piggy
	$(Q)lzma e piggy piggy.lzma

uImage.gzip : piggy.gzip
	$(Q)mkimage -A mips -O linux -T kernel -C gzip -a $(LOADADDR) -e \
	0x$(shell grep kernel_entry $(TOPDIR)/System.map | awk '{ printf "%s", $$1 }') \
	-n 'MIPS Linux-$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)' \
	-d piggy.gzip uImage

uImage.bzip2 : piggy.bz2
	$(Q)mkimage -A mips -O linux -T kernel -C bzip2 -a $(LOADADDR) -e \
	0x$(shell grep kernel_entry $(TOPDIR)/System.map | awk '{ printf "%s", $$1 }') \
	-n 'MIPS Linux-$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)' \
	-d piggy.bz2 uImage

uImage.lzma : piggy.lzma
	$(Q)mkimage -A mips -O linux -T kernel -C lzma -a $(LOADADDR) -e \
	0x$(shell grep kernel_entry $(TOPDIR)/System.map | awk '{ printf "%s", $$1 }') \
	-n 'MIPS Linux-$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)' \
	-d piggy.lzma uImage

vmlinux.ecoff: $(obj)/elf2ecoff $(VMLINUX)
	$(obj)/elf2ecoff $(VMLINUX) vmlinux.ecoff $(E2EFLAGS)

$(obj)/elf2ecoff: $(obj)/elf2ecoff.c
	$(HOSTCC) -o $@ $^

vmlinux.bin: $(VMLINUX)
	$(OBJCOPY) -O binary $(strip-flags) $(VMLINUX) $(obj)/vmlinux.bin

vmlinux.srec: $(VMLINUX)
	$(OBJCOPY) -S -O srec $(strip-flags) $(VMLINUX) $(obj)/vmlinux.srec

$(obj)/addinitrd: $(obj)/addinitrd.c
	$(HOSTCC) -o $@ $^

clean-files += addinitrd \
	       elf2ecoff \
	       vmlinux.bin \
	       vmlinux.ecoff \
	       vmlinux.srec
