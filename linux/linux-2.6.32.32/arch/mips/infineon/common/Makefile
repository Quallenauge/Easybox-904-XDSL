#
# Makefile for the platform specific kernel interface routines under Linux.
#

all: ifxmips_common.a

obj-y   := common_routines.o ifxmips_prom_printf.o ifxmips_prom.o ifxmips_setup.o ifxmips_interrupt.o ifxmips_reset.o ifxmips_gptu.o
obj-y += ifxmips_dma_core.o
obj-y += ifxmips_exin.o
#obj-y += kgdb_serial.o lm.o

#obj-$(CONFIG_IFX_PMCU)      += ifxmips_gptu_pm.o

EXTRA_AFLAGS := $(CFLAGS)

piggy.lzma: $(TOPDIR)/vmlinux
	$(OBJCOPY) -O binary -R .note -R .comment -S $(TOPDIR)/vmlinux piggy
	lzma -f -z --best -v piggy

uImage.lzma: piggy.lzma
	echo Building $@...
	mkimage -A mips -O linux -T kernel -C lzma -a $(LOADADDR) -e \
	0x$(shell grep kernel_entry $(TOPDIR)/System.map | awk '{ printf "%s", $$1 }') \
	-n 'MIPS Linux-$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)' \
	-d piggy.lzma $@

#clean:
#	rm -f uImage pImage piggy piggy.* *.o *.a

