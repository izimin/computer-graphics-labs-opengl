#
# Makefile to build examples for chapter 7 for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>
#

include ../make.inc

ALL = simple-particles
all: $(ALL)

simple-particles: simple-particles.o ParticleSystem.o $(OBJS)
	cc $(LDFLAGS) simple-particles.o ParticleSystem.o $(OBJS) -o simple-particles $(LIBS) -lc


clean:
	rm -r -f $(ALL) $(OBJS) simple-particles.o ParticleSystem.o 2> /dev/nul


include ../rules.linux
