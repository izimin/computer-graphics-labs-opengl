#
# Makefile to build GLSL examples for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>
#

include ../make.inc

ALL = glsl-info glsl-example glsl-example-2 glsl-diffuse glsl-specular glsl-specular-2 glsl-ocean glsl-eroded glsl-gooch glsl-thin-layer glsl-film glsl-aniso glsl-lava glsl-film-2

all: $(ALL)

glsl-info: glsl-info.o $(OBJS)
	cc $(LDFLAGS) glsl-info.o $(OBJS) -o glsl-info $(LIBS) -lc

glsl-example: glsl-example.o $(OBJS)
	cc $(LDFLAGS) glsl-example.o $(OBJS) -o glsl-example $(LIBS) -lc

glsl-example-2: glsl-example-2.o GlslProgram.o $(OBJS)
	cc $(LDFLAGS) glsl-example-2.o GlslProgram.o $(OBJS) -o glsl-example-2 $(LIBS) -lc

glsl-diffuse: glsl-diffuse.o GlslProgram.o $(OBJS)
	cc $(LDFLAGS) glsl-diffuse.o GlslProgram.o $(OBJS) -o glsl-diffuse $(LIBS) -lc

glsl-specular: glsl-specular.o GlslProgram.o Torus.o $(OBJS)
	cc $(LDFLAGS) glsl-specular.o GlslProgram.o Torus.o $(OBJS) -o glsl-specular $(LIBS) -lc

glsl-specular-2: glsl-specular-2.o GlslProgram.o Torus.o $(OBJS)
	cc $(LDFLAGS) glsl-specular-2.o GlslProgram.o Torus.o $(OBJS) -o glsl-specular-2 $(LIBS) -lc

glsl-ocean: glsl-ocean.o GlslProgram.o libTexture3D.o $(OBJS)
	cc $(LDFLAGS) glsl-ocean.o GlslProgram.o libTexture3D.o $(OBJS) -o glsl-ocean $(LIBS) -lc

glsl-eroded: glsl-eroded.o GlslProgram.o Torus.o $(OBJS)
	cc $(LDFLAGS) glsl-eroded.o GlslProgram.o Torus.o $(OBJS) -o glsl-eroded $(LIBS) -lc

glsl-gooch: glsl-gooch.o GlslProgram.o Torus.o $(OBJS)
	cc $(LDFLAGS) glsl-gooch.o GlslProgram.o Torus.o $(OBJS) -o glsl-gooch $(LIBS) -lc

glsl-thin-layer: glsl-thin-layer.o GlslProgram.o Torus.o $(OBJS)
	cc $(LDFLAGS) glsl-thin-layer.o GlslProgram.o Torus.o $(OBJS) -o glsl-thin-layer $(LIBS) -lc

glsl-film: glsl-film.o GlslProgram.o boxes.o PBuffer.o $(OBJS)
	cc $(LDFLAGS) glsl-film.o GlslProgram.o boxes.o PBuffer.o $(OBJS) -o glsl-film $(LIBS) -lc

glsl-film-2: glsl-film-2.o GlslProgram.o boxes.o PBuffer.o $(OBJS)
	cc $(LDFLAGS) glsl-film-2.o GlslProgram.o boxes.o PBuffer.o $(OBJS) -o glsl-film-2 $(LIBS) -lc

glsl-aniso: glsl-aniso.o GlslProgram.o Torus.o $(OBJS)
	cc $(LDFLAGS) glsl-aniso.o GlslProgram.o Torus.o $(OBJS) -o glsl-aniso $(LIBS) -lc

glsl-lava: glsl-lava.o GlslProgram.o libTexture3D.o $(OBJS)
	cc $(LDFLAGS) glsl-lava.o GlslProgram.o libTexture3D.o $(OBJS) -o glsl-lava $(LIBS) -lc

clean:
	rm -r -f $(ALL) $(OBJS) *.o 2> /dev/nul

include ../rules.linux  
