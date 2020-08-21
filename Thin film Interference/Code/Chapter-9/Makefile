#
# Makefile to build examples for chapter 9 for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>
#

include ../make.inc

ALL = rc-info rc-diffuse-1 rc-diffuse-2 rc-diffuse-3 rc-diffuse-torus rc-specular-1 rc-specular-2 rc-specular-torus
all: $(ALL)

rc-info: rc-info.o $(OBJS)
	cc $(LDFLAGS) rc-info.o $(OBJS) -o rc-info $(LIBS) -lc

rc-diffuse-1: rc-diffuse-1.o $(OBJS)
	cc $(LDFLAGS) rc-diffuse-1.o $(OBJS) -o rc-diffuse-1 $(LIBS) -lc


rc-diffuse-2: rc-diffuse-2.o $(OBJS)
	cc $(LDFLAGS) rc-diffuse-2.o $(OBJS) -o rc-diffuse-2 $(LIBS) -lc

rc-diffuse-3: rc-diffuse-3.o $(OBJS)
	cc $(LDFLAGS) rc-diffuse-3.o $(OBJS) -o rc-diffuse-3 $(LIBS) -lc

rc-diffuse-torus: rc-diffuse-torus.o Torus.o $(OBJS)
	cc $(LDFLAGS) rc-diffuse-torus.o Torus.o $(OBJS) -o rc-diffuse-torus $(LIBS) -lc

rc-specular-1: rc-specular-1.o $(OBJS)
	cc $(LDFLAGS) rc-specular-1.o $(OBJS) -o rc-specular-1 $(LIBS) -lc

rc-specular-2: rc-specular-2.o $(OBJS)
	cc $(LDFLAGS) rc-specular-2.o $(OBJS) -o rc-specular-2 $(LIBS) -lc

rc-specular-torus: rc-specular-torus.o Torus.o Matrix4x4.o $(OBJS)
	cc $(LDFLAGS) rc-specular-torus.o Torus.o Matrix4x4.o $(OBJS) -o rc-specular-torus $(LIBS) -lc

clean:
	rm -r -f $(ALL) $(OBJS) rc-info.o rc-diffuse-1.o rc-diffuse-2.o rc-diffuse-3.o rc-diffuse-torus.o rc-specular-1.o rc-specular-2.o rc-specular-torus.o Torus.o Matrix4x4.o 2> /dev/nul


include ../rules.linux
