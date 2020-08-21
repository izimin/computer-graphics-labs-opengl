//
// Class to load images from DDS files,
// based on ogl_dds_texture_loader.cpp by Kevin Harris (kevin@codesampler.com)
//
// Author: Alex V. Boreskoff
//

#include    <stdio.h>

#ifdef  _WIN32
    
	#include    <windows.h>
#endif



#include    <string.h>
#include	"libExt.h"
#include    "Data.h"
#include    "DdsLoader.h"
#include    "CompressedTexture.h"
#include    "DdsDefs.h"

const unsigned long FOURCC_DXT1 = 0x31545844; //(MAKEFOURCC('D','X','T','1'))
const unsigned long FOURCC_DXT3 = 0x33545844; //(MAKEFOURCC('D','X','T','3'))
const unsigned long FOURCC_DXT5 = 0x35545844; //(MAKEFOURCC('D','X','T','5'))

Texture * DdsLoader :: load ( Data * data )
{
    DDS_HEADER      ddsd;
    char            filecode [4];
    int             factor;
    int             bufferSize;
    int             format;

                                        // Verify the file is a true .dds file
    data -> seekAbs  ( 0 );
    data -> getBytes ( filecode, 4 );

    if( strncmp ( filecode, "DDS ", 4 ) != 0 )
        return NULL;

                                        // Get the surface descriptor
    data -> getBytes ( &ddsd, sizeof ( ddsd ) );

    int numComponents = 0;

    if ( ddsd.ddspf.dwFlags & DDS_ALPHA_PIXELS )        // ARGB
        numComponents = 4;
    else                                                // RGB
        numComponents = 3;

    if ( (ddsd.ddspf.dwFlags & DDS_FOURCC) == 0 )       // not compressed
    {
        int bytesPerLine = ddsd.dwWidth * numComponents;

        if ( (bytesPerLine & 3) != 0 )                  // do dword alignment
            bytesPerLine += 4 - (bytesPerLine & 3);

        byte    * buf     = new byte [bytesPerLine];
        Texture * texture = new Texture ( ddsd.dwWidth, ddsd.dwHeight, numComponents );

        for ( int i = 0; i < (int)ddsd.dwHeight; i++ )
        {
            data -> getBytes ( buf, bytesPerLine );

                                                        // rearrange components
            byte * dest = texture -> getData () + i * ddsd.dwWidth * numComponents;
            byte * src  = buf;

            for ( register int j = 0; j < (int)ddsd.dwWidth; j++ )
            {
                dest [0] = src [2];                     // red
                dest [1] = src [1];                     // green
                dest [2] = src [0];                     // blue

                if ( numComponents == 4 )
                    dest [3] = src [3];                 // alpha

                dest += numComponents;
                src  += numComponents;
            }
        }

        return texture;
    }

    //
    // This .dds loader supports the loading of compressed formats DXT1, DXT3
    // and DXT5.
    //

    int numBlocks = ((ddsd.dwWidth + 3)/4) * ((ddsd.dwHeight + 3)/4);   // number of 4*4 texel blocks
    int blockSize = 0;

    switch ( ddsd.ddspf.dwFourCC )
    {
        case FOURCC_DXT1:
                            // DXT1's compression ratio is 8:1
            if ( numComponents == 4 )
                format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            else
                format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;

            factor    = 2;
            blockSize = numBlocks * 8;                              // get size of byte image (8 bytes per block)
            break;

        case FOURCC_DXT3:
                            // DXT3's compression ratio is 4:1
            format     = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            factor     = 4;
            blockSize  = numBlocks * 16;                            // get size of byte image (16 bytes per block)
            break;

        case FOURCC_DXT5:
                            // DXT5's compression ratio is 4:1
            format    = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            factor    = 4;
            blockSize = numBlocks * 16;                         // get size of byte image (16 bytes per block)
            break;

        default:
            return NULL;
    }

    //
    // How big will the buffer need to be to load all of the pixel data
    // including mip-maps?
    //

//    if( ddsd.dwPitchOrLinearSize == 0 )
//        return NULL;

                                            // check for RGBA-only formats
    if ( format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT || format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT )
        numComponents = 4;
    else
        numComponents = 3;

    if( ddsd.dwPitchOrLinearSize != 0 )
        if( ddsd.dwMipMapCount > 1 )
            bufferSize = ddsd.dwPitchOrLinearSize * factor;
        else
            bufferSize = ddsd.dwPitchOrLinearSize;
    else
        bufferSize = blockSize;

    int mipMapsCount = ddsd.dwMipMapCount;

    if ( mipMapsCount < 1 )
        mipMapsCount = 1;

    CompressedTexture * texture = new CompressedTexture ( ddsd.dwWidth, ddsd.dwHeight, numComponents, format, mipMapsCount, bufferSize );

    data -> getBytes ( texture -> getData (), bufferSize );

    return texture;
}
