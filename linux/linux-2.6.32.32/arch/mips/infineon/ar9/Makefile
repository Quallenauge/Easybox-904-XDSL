#
# Makefile for the platform specific kernel interface routines under Linux.
#

all: ifxmips_ar9.a

obj-y   := ar9.o ifxmips_clk.o
obj-$(CONFIG_AR9_REF_BOARD) += ar9_ref_board.o
obj-$(CONFIG_IFX_CLOCK_CHANGE) += ifxmips_fill_icache.o

EXTRA_AFLAGS := $(CFLAGS)

clean:
	rm -f *.o *.a

