#
# Makefile to build examples for chapter 11 for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>
#

include ../make.inc

ALL         = distance-attenuation reflective-torus

all: $(ALL)

distance-attenuation: distance-attenuation.o PBuffer.o $(OBJS)
	cc $(LDFLAGS) distance-attenuation.o PBuffer.o $(OBJS) -o distance-attenuation $(LIBS) -lc


reflective-torus: reflective-torus.o PBuffer.o Torus.o Matrix3D.o $(OBJS)
	cc $(LDFLAGS) reflective-torus.o PBuffer.o Torus.o Matrix3D.o $(OBJS) -o reflective-torus $(LIBS) -lc

clean:
	rm -r -f $(ALL) $(OBJS) distance-attenuation.o reflective-torus.o PBuffer.o Torus.o Matrix3D.o 2> /dev/nul


include ../rules.linux