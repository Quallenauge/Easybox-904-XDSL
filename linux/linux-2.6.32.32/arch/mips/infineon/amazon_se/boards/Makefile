#
# Makefile for the platform specific kernel interface routines under Linux.
#

all: ifxmips_amazon_se_boards.a

obj-$(CONFIG_AMAZON_SE_REF_BOARD) += amazon_se_ref_board.o

EXTRA_AFLAGS := $(CFLAGS)

clean:
	rm -f *.o *.a

