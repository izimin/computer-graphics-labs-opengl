//
// Compressed texture class
//

#ifdef  _WIN32
    #include    <windows.h>
#endif

#ifdef	MACOSX
	#include	<stdlib.h>
#else
	#include    <malloc.h>
#endif

#include    <memory.h>
#include    "libExt.h"
#include    "CompressedTexture.h"

CompressedTexture :: CompressedTexture ( int theWidth, int theHeight, int theNumComponents, int theFormat, int theLevels, int theSize ) : Texture ()
{
    width         = theWidth;
    height        = theHeight;
    numComponents = theNumComponents;
    format        = theFormat;
    data          = (byte *) malloc ( theSize );
    levels        = theLevels;
    compressed    = true;
}

bool    CompressedTexture :: upload ( int target, bool mipmap )
{
    int w    = width;
    int h    = height;
    int offs = 0;
    int blockSize;
    int size;

                                            // get number of bytes per block
    if( format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT || format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )
        blockSize = 8;
    else
        blockSize = 16;

    for ( int i = 0; i < levels; i++ )
    {
        if ( w  == 0 )
            w  = 1;

        if ( h == 0 )
            h = 1;

        size = ((w+3)/4) * ((h+3)/4) * blockSize;

        glCompressedTexImage2DARB ( target, i, format, w, h, 0, size, data + offs );

        offs += size;

        w = w / 2;
        h = h / 2;
    }

    mipmapped = levels > 1;

    return true;
}

