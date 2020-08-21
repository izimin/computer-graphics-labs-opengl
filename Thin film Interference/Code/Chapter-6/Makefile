#
# Makefile to build examples for chapter 6 for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>
#

include ../make.inc

ALL = texture-3D make-3d-noise

all: $(ALL)

texture-3D: texture-3D.o Torus.o $(OBJS)
	cc $(LDFLAGS) texture-3D.o Torus.o $(OBJS) -o texture-3D $(LIBS) -lc

make-3d-noise: make-3d-noise.o noise.o libTexture3D.o $(OBJS)
	cc $(LDFLAGS) make-3d-noise.o noise.o libTexture3D.o $(OBJS) -o make-3d-noise $(LIBS) -lc


clean:
	rm -r -f $(ALL) $(OBJS) texture-3D.o Torus.o make-3d-noise.o noise.o libTexture3D.o 2> /dev/nul


include ../rules.linux
