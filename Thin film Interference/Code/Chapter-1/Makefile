#
# Makefile to build examples for chapter 1 for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>
#

include ../make.inc

ALL = extstring glexts
all: $(ALL)

extstring: extstring.o $(OBJS)
	cc $(LDFLAGS) extstring.o -o extstring $(LIBS) -lc

glexts: glexts.o $(OBJS)
	cc $(LDFLAGS) glexts.o -o glexts $(LIBS) -lc

clean:
	rm -r -f $(ALL) $(OBJS) extstring.o glexts.o 2> /dev/nul


include ../rules.linux