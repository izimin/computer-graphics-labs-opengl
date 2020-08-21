#
# Makefile to build examples for chapter 8 for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>
#

include ../make.inc

ALL = dot3-1 dot3-2 dot3-3 dot3-4 dot3-5
all: $(ALL)

dot3-1: dot3-1.o $(OBJS)
	cc $(LDFLAGS) dot3-1.o $(OBJS) -o dot3-1 $(LIBS) -lc


dot3-2: dot3-2.o $(OBJS)
	cc $(LDFLAGS) dot3-2.o $(OBJS) -o dot3-2 $(LIBS) -lc

dot3-3: dot3-3.o $(OBJS)
	cc $(LDFLAGS) dot3-3.o $(OBJS) -o dot3-3 $(LIBS) -lc

dot3-4: dot3-4.o $(OBJS)
	cc $(LDFLAGS) dot3-4.o $(OBJS) -o dot3-4 $(LIBS) -lc

dot3-5: dot3-5.o Torus.o $(OBJS)
	cc $(LDFLAGS) dot3-5.o Torus.o $(OBJS) -o dot3-5 $(LIBS) -lc

clean:
	rm -r -f $(ALL) $(OBJS) dot3-1.o dot3-2.o dot3-3.o dot3-4.o dot3-5.o Torus.o 2> /dev/nul


include ../rules.linux
