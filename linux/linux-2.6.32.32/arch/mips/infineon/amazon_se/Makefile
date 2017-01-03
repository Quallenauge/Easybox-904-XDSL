#
# Makefile for the platform specific kernel interface routines under Linux.
#

all: ifxmips_amazon_se.a

obj-y   := amazon_se.o ifxmips_clk.o
obj-y	+= boards/

EXTRA_AFLAGS := $(CFLAGS)

clean:
	rm -f *.o *.a

