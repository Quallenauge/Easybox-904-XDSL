#
# Makefile for the platform specific kernel interface routines under Linux.
#

all: ifxmips_danube.a

obj-y   := danube.o ifxmips_clk.o
obj-$(CONFIG_DANUBE_REF_BOARD) += danube_ref_board.o

EXTRA_AFLAGS := $(CFLAGS)

clean:
	rm -f *.o *.a

