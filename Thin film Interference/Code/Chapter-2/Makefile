#
# Makefile to build fragment programs examples for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>
#

include ../make.inc

all: multitex1 multitex2 multitex3 multitex4 fog

multitex1: multitex1.o $(OBJS)
	cc $(LDFLAGS) multitex1.o $(OBJS) -o multitex1 $(LIBS) -lc

multitex2: multitex2.o $(OBJS)
	cc $(LDFLAGS) multitex2.o $(OBJS) -o multitex2 $(LIBS) -lc

multitex3: multitex3.o $(OBJS)
	cc $(LDFLAGS) multitex3.o $(OBJS) -o multitex3 $(LIBS) -lc

multitex4: multitex4.o $(OBJS)
	cc $(LDFLAGS) multitex4.o $(OBJS) -o multitex4 $(LIBS) -lc

fog: fog.o $(OBJS)
	cc $(LDFLAGS) fog.o $(OBJS) -o fog $(LIBS) -lc


clean:
	rm -r -f multitex1 multitex2 multitex3 multitex4 fog $(OBJS) multitex1.o multitex2.o multitex3.o multitex4.o fog.o 2> /dev/nul


include ../rules.linux
