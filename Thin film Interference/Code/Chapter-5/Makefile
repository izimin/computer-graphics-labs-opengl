#
# Makefile to build examples for chapter 5 for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>
#

include ../make.inc

ALL = cubemap lamp
all: $(ALL)

cubemap: cubemap.o $(OBJS)
	cc $(LDFLAGS) cubemap.o $(OBJS) -o cubemap $(LIBS) -lc

lamp: lamp.o $(OBJS)
	cc $(LDFLAGS) lamp.o $(OBJS) -o lamp $(LIBS) -lc


clean:
	rm -r -f $(ALL) $(OBJS) cubemap.o lamp.o 2> /dev/nul


include ../rules.linux
