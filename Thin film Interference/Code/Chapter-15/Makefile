#
# Makefile to build examples for chapter 11 for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>
#

include ../make.inc

ALL = vp-info vp-diffuse vp-specular vp-embm particles

all: $(ALL)

vp-info: vp-info.o $(OBJS)
	cc $(LDFLAGS) vp-info.o $(OBJS) -o vp-info $(LIBS) -lc

vp-diffuse: vp-diffuse.o Torus.o $(OBJS)
	cc $(LDFLAGS) vp-diffuse.o Torus.o $(OBJS) -o vp-diffuse $(LIBS) -lc

vp-specular: vp-specular.o Torus.o $(OBJS)
	cc $(LDFLAGS) vp-specular.o Torus.o $(OBJS) -o vp-specular $(LIBS) -lc

vp-embm: vp-embm.o Torus.o VertexProgram.o ParamProxy.o $(OBJS)
	cc $(LDFLAGS) vp-embm.o Torus.o VertexProgram.o ParamProxy.o $(OBJS) -o vp-embm $(LIBS) -lc

particles: particles.o VertexProgram.o ParamProxy.o $(OBJS)
	cc $(LDFLAGS) particles.o VertexProgram.o ParamProxy.o $(OBJS) -o particles $(LIBS) -lc

clean:
	rm -r -f $(ALL) $(OBJS) vp-info.o vp-diffuse.o vp-specular.o vp-embm.o particles.o Torus.o VertexProgram.o ParamProxy.o 2> /dev/nul


include ../rules.linux
