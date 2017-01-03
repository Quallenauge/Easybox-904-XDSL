#
# Makefile for the platform specific kernel interface routines under Linux.
#

all: ifxmips_ar10.a

obj-y   := ar10.o ifxmips_clk.o
obj-$(CONFIG_AR10_REF_BOARD) += ar10_ref_board.o

EXTRA_AFLAGS := $(CFLAGS)

clean:
	rm -f *.o *.a

