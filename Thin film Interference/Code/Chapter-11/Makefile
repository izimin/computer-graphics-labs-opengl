#
# Makefile to build examples for chapter 11 for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>
#

include ../make.inc

#PBUFFERPATH = Linux
#INCLUDE     = -I$(LIBEXTPATH) -I$(LIBTEXTUREPATH) -I$(LIB3DPATH) -I$(PBUFFERPATH)
ALL         = example-1 example-2 room

all: $(ALL)

example-1: example-1.o PBuffer.o $(OBJS)
	cc $(LDFLAGS) example-1.o PBuffer.o $(OBJS) -o example-1 $(LIBS) -lc


example-2: example-2.o PBuffer.o $(OBJS)
	cc $(LDFLAGS) example-2.o PBuffer.o $(OBJS) -o example-2 $(LIBS) -lc

room: room.o PBuffer.o boxes.o $(OBJS)
	cc $(LDFLAGS) room.o PBuffer.o boxes.o $(OBJS) -o room $(LIBS) -lc

clean:
	rm -r -f $(ALL) $(OBJS) example-1.o example-2.o room.o PBuffer.o boxes.o 2> /dev/nul

include ../rules.linux  
