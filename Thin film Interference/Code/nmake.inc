#
# Include file with basic definitions for all examples for Win32
#
# Author: Alex V. Boreskoff
#

OPENGL         = glut32.lib glu32.lib opengl32.lib
GLUTPATH       = ../glut
LIBEXTPATH     = ../libExt/
LIBTEXTUREPATH = ../libTexture/
LIB3DPATH      = ../3D/
UTILSPATH      = ../utils
PBUFFERPATH    = ../PBuffer/Win32
PROGRAMPATH    = ../Program

PBUFFERINC      = -I../PBuffer -I../PBuffer/Win32
INC             = $(INC) -I$(GLUTPATH) -I$(LIBEXTPATH) -I$(LIBTEXTUREPATH) -I$(UTILSPATH) -I$(LIB3DPATH) -I$(PROGRAMPATH) -I$(LIBTEXTUREPATH)zlib -I$(LIBTEXTUREPATH)libpng -I$(LIBTEXTUREPATH)jpeg
LLDLIBS         = $(LFLAGS) $(OPENGL) $(guilibs) $(LIBTEXTUREPATH)jpeg/libjpeg.lib $(LIBTEXTUREPATH)libpng/libpng.lib $(LIBTEXTUREPATH)zlib/zlib.lib /LIBPATH:$(GLUTPATH)
LIBEXT_OBJS     = libExt.obj
LIBTEXTURE_OBJS = libTexture.obj Texture.obj CompressedTexture.obj BmpLoader.obj TgaLoader.obj DdsLoader.obj JpegLoader.obj PngLoader.obj Data.obj ZipFileSystem.obj
OBJS            = $(LIBEXT_OBJS) $(LIBTEXTURE_OBJS) Vector3D.obj Vector2D.obj
CFLAGS          = $(CFLAGS) /GX

