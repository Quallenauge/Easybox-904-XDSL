CC = gcc
CFLAGS = -Wall
OBJS = con2fbmap.o

all: con2fbmap

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

con2fbmap: $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm -f *.o
