//
// Simple texture class
//

#ifdef  _WIN32
    #include    <windows.h>
#endif

#ifdef	MACOSX
	#include	<OpenGL/gl.h>
	#include	<OpenGL/glu.h>
	#include	<stdlib.h>					// for malloc
#else
	#include    <GL/gl.h>
	#include    <GL/glu.h>
	#include    <malloc.h>
#endif

#include    <memory.h>

#include    "Texture.h"

Texture :: Texture ()
{
    width         = 0;
    height        = 0;
    numComponents = 0;
    data          = NULL;
    levels        = 0;
    compressed    = false;
    mipmapped     = false;
    format        = GL_NONE;
}

Texture :: Texture ( int theWidth, int theHeight, int theNumComponents )
{
    width         = theWidth;
    height        = theHeight;
    numComponents = theNumComponents;
    data          = (byte *) malloc ( width * height * numComponents );
    levels        = 1;
    compressed    = false;
    mipmapped     = false;

    switch ( numComponents )
    {
        case 1:
            format = GL_ALPHA;
            break;

        case 3:
            format = GL_RGB;
            break;

        case 4:
            format = GL_RGBA;
            break;

        default:
            format = -1;
    }
}

Texture :: ~Texture ()
{
    if ( data != NULL )
        free ( data );
}
                                            // store 32-bit RGBA image into texture in a
                                            // specified line
void    Texture :: putLine ( int y, dword * bits )
{
    if ( y < 0 || y >= height )
        return;

    int    offs = y * width * numComponents;
    byte * ptr  = data + offs;

    if ( numComponents == 4 )               // RGBA image
        memcpy ( ptr, bits, 4 * width );
    else
    if ( numComponents == 3 )               // RGB image
    {
        byte * src = (byte *) bits;

        for ( int i = 0; i < width; i++, src += 4 )
        {
            *ptr++ = src [0];
            *ptr++ = src [1];
            *ptr++ = src [2];
        }
    }
    else
    if ( numComponents == 1 )               // greyscale image
    {
        for ( int i = 0; i < width ; i++, bits++ )
            *ptr++ = *(byte *) bits;
    }
}

bool    Texture :: upload ( int target, bool mipmap )
{
    if ( target == GL_TEXTURE_1D )
    {
        if ( mipmap )
            gluBuild1DMipmaps ( target, getNumComponents (), getWidth (),
                                getFormat (), GL_UNSIGNED_BYTE, getData () );
        else
            glTexImage1D      ( target, 0, getNumComponents (), getWidth (), 0,
                                getFormat (), GL_UNSIGNED_BYTE, getData () );
    }
    else
    {
        if ( mipmap )
            gluBuild2DMipmaps ( target, getNumComponents (), getWidth (), getHeight (),
                                getFormat (), GL_UNSIGNED_BYTE, getData () );
        else
            glTexImage2D      ( target, 0, getNumComponents (), getWidth (), getHeight (), 0,
                                getFormat (), GL_UNSIGNED_BYTE, getData () );
    }

    mipmapped = mipmap;

    return true;
}

