#
# Makefile to build fragment program examples for for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>
#

include ../make.inc

ALL = fp-info fp-specular fp-specular2 fp-specular3 chrome Minneart Minneart-2 aniso fp-glow fp-sepia fp-edge

all: $(ALL)

fp-info: fp-info.o $(OBJS)
	cc $(LDFLAGS) fp-info.o $(OBJS) -o fp-info $(LIBS) -lc

fp-specular: fp-specular.o VertexProgram.o FragmentProgram.o ParamProxy.o Torus.o $(OBJS)
	cc $(LDFLAGS) fp-specular.o VertexProgram.o FragmentProgram.o ParamProxy.o Torus.o $(OBJS) -o fp-specular $(LIBS) -lc

fp-specular2: fp-specular2.o VertexProgram.o FragmentProgram.o ParamProxy.o Torus.o $(OBJS)
	cc $(LDFLAGS) fp-specular2.o VertexProgram.o FragmentProgram.o ParamProxy.o Torus.o $(OBJS) -o fp-specular2 $(LIBS) -lc

fp-specular3: fp-specular3.o VertexProgram.o FragmentProgram.o ParamProxy.o Torus.o $(OBJS)
	cc $(LDFLAGS) fp-specular3.o VertexProgram.o FragmentProgram.o ParamProxy.o Torus.o $(OBJS) -o fp-specular3 $(LIBS) -lc

chrome: chrome.o VertexProgram.o FragmentProgram.o ParamProxy.o Torus.o $(OBJS)
	cc $(LDFLAGS) chrome.o VertexProgram.o FragmentProgram.o ParamProxy.o Torus.o $(OBJS) -o chrome $(LIBS) -lc

Minneart: Minneart.o Torus.o FragmentProgram.o VertexProgram.o ParamProxy.o $(OBJS)
	cc $(LDFLAGS) Minneart.o Torus.o FragmentProgram.o VertexProgram.o ParamProxy.o $(OBJS) -o Minneart $(LIBS) -lc

Minneart-2: Minneart-2.o Torus.o FragmentProgram.o VertexProgram.o ParamProxy.o $(OBJS)
	cc $(LDFLAGS) Minneart-2.o Torus.o FragmentProgram.o VertexProgram.o ParamProxy.o $(OBJS) -o Minneart-2 $(LIBS) -lc

aniso: aniso.o FragmentProgram.o VertexProgram.o ParamProxy.o $(OBJS)
	cc $(LDFLAGS) aniso.o FragmentProgram.o VertexProgram.o ParamProxy.o $(OBJS) -o aniso $(LIBS) -lc

fp-glow: fp-glow.o FragmentProgram.o VertexProgram.o ParamProxy.o Torus.o $(OBJS)
	cc $(LDFLAGS) fp-glow.o FragmentProgram.o VertexProgram.o ParamProxy.o Torus.o $(OBJS) -o fp-glow $(LIBS) -lc

fp-sepia: fp-sepia.o FragmentProgram.o VertexProgram.o ParamProxy.o PBuffer.o boxes.o $(OBJS)
	cc $(LDFLAGS) fp-sepia.o FragmentProgram.o VertexProgram.o ParamProxy.o PBuffer.o boxes.o $(OBJS) -o fp-sepia $(LIBS) -lc

fp-edge: fp-edge.o FragmentProgram.o VertexProgram.o ParamProxy.o PBuffer.o boxes.o $(OBJS)
	cc $(LDFLAGS) fp-edge.o FragmentProgram.o VertexProgram.o ParamProxy.o PBuffer.o boxes.o $(OBJS) -o fp-edge $(LIBS) -lc

clean:
	rm -r -f $(ALL) $(OBJS) fp-info.o fp-specular.o fp-specular2.o fp-specular3.o chrome.o Minneart.o Minneart-2.o aniso.o fp-glow.o fp-sepia.o fp-edge.o Torus.o VertexProgram.o FragmentProgram.o ParamProxy.o PBuffer.o boxes.o 2> /dev/nul


include ../rules.linux