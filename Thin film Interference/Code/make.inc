#
# Include file with basic definitions for all examples for Linux
#
# Author: Alex V. Boreskoff
#

SHELL          = /bin/sh
GLUTPATH       = /home/alex/glut/glut-3.7
LIBEXTPATH     = ../libExt
LIBTEXTUREPATH = ../libTexture
LIB3DPATH      = ../3D
UTILSPATH      = ../utils
PBUFFERPATH    = ../PBuffer/Linux
PROGRAMPATH    = ../Program

PBUFFERINC      = -I../PBuffer -I../PBuffer/Linux
CFLAGS          = -x c++ -g -I/usr/include/GL -Wall -I$(GLUTPATH)/include/GL
LIBS            = -L/usr/X11R6/lib -L$(GLUTPATH)/lib -lglut -lGL -lGLU -lXmu -lXt -lX11 -lXi -lm -ljpeg -lz -lpng

LIBEXT_OBJS     = libExt.o
LIBTEXTURE_OBJS = libTexture.o Texture.o CompressedTexture.o BmpLoader.o TgaLoader.o DdsLoader.o JpegLoader.o PngLoader.o Data.o ZipFileSystem.o
OBJS            = $(LIBEXT_OBJS) $(LIBTEXTURE_OBJS) Vector3D.o
INCLUDE         = -I$(LIBEXTPATH) -I$(LIBTEXTUREPATH) -I$(LIB3DPATH) -I$(UTILSPATH) -I$(PROGRAMPATH) $(PBUFFERINC)
