#
# Makefile to build fragment programs examples for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>
#

include ../make.inc

all: room

room: room.o boxes.o $(OBJS)
	cc $(LDFLAGS) room.o boxes.o $(OBJS) -o room $(LIBS) -lc

clean:
	rm -r -f room room.o boxes.o $(OBJS) 2> /dev/nul


include ../rules.linux
