//
// Simple library to load basic textures: tga, bmp
//

#ifdef  _WIN32
    #include    <windows.h>
    #include    <io.h>
#else
    
	#include    <unistd.h>
    
	#include    <sys/types.h>
    
	#define O_BINARY    0

#endif

#pragma	warning (disable:4786)		// FUCK these stuppid bastards !!!

#ifdef	MACOSX
	#include	<OpenGL/gl.h>
	#include	<OpenGL/glu.h>
	#include	<GLUT/glut.h>
	#include	<alloc.h>
#else
	#include    <GL/gl.h>
	#include    <GL/glu.h>
	#include    <glut.h>
	#include	<malloc.h>
#endif

//#include    "../glext.h"
#include    <fcntl.h>
#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <math.h>
#include    <sys/stat.h>


#include    "Vector3D.h"
#include    "Vector2D.h"
#include    "Data.h"
#include    "Texture.h"
#include    "BmpLoader.h"
#include    "TgaLoader.h"
#include    "DdsLoader.h"
#include    "JpegLoader.h"
#include    "PngLoader.h"
#include    "FileSystem.h"
#include    "ZipFileSystem.h"
#include    "libTexture.h"

#ifndef EPS
    define  EPS 0.0001f                     // used in normal generation routines
#endif

#pragma pack (push, 1)
                                            // TGA specific constants and structs
struct  TgaHeader
{
    byte    idLength;
    byte    colormapType;
    byte    imageType;
    uint16  colormapIndex;
    uint16  colormapLength;
    byte    colormapEntrySize;
    uint16  xOrigin;
    uint16  yOrigin;
    uint16  width;
    uint16  height;
    byte    pixelSize;
    byte    imageDesc;
};

#pragma pack (pop)

/////////////////////////// static data /////////////////////////

list <FileSystem *> fileSystems;

                                            // some predecalarations
                                            // functions loading texture from file into
                                            // current target
bool    loadTexture    ( int target, bool mipmap, const char * fileName );

////////////////////// universal data getter ////////////////////////

Data * getFile ( const char * fileName )
{
    if ( fileExist ( fileName ) )
        return new Data ( fileName );

    Data * data;

    for ( list <FileSystem *> :: iterator it = fileSystems.begin (); it != fileSystems.end (); ++it )
        if ( ( data = (*it) -> getFile ( fileName ) ) != NULL )
            return data;

    return NULL;
}

bool        addFileSystem ( FileSystem * fileSystem )
{
    if ( !fileSystem -> isOk () )
    {
        delete fileSystem;

        return false;
    }

    fileSystems.push_back ( fileSystem );

    return true;
}


bool        addZipFileSystem ( const char * fileName )
{
    return addFileSystem ( new ZipFileSystem ( fileName ) );
}

/////////////////////////////////////////////////////////////////////

static  void    getCubeVector ( int side, int cubeSize, int x, int y, Vector3D& v )
{
    float   s  = ((float) x + 0.5f) / (float) cubeSize;
    float   t  = ((float) y + 0.5f) / (float) cubeSize;
    float   sc = 2*s - 1;
    float   tc = 2*t - 1;

    switch ( side )
    {
        case 0:
            v = Vector3D ( 1, -tc, -sc );
            break;

        case 1:
            v = Vector3D ( -1, -tc, sc );
            break;

        case 2:
            v = Vector3D ( sc, 1, tc );
            break;

        case 3:
            v = Vector3D ( sc, -1, -tc );
            break;

        case 4:
            v = Vector3D ( sc, -tc, 1 );
            break;

        case 5:
            v = Vector3D ( -sc, -tc, -1 );
            break;
    }

    v.normalize ();
}

GLuint    createNormalizationCubemap ( int cubeSize, bool mipmap )
{
    Vector3D    v;
    byte     * pixels = (byte *) malloc ( 3 * cubeSize * cubeSize );

    if ( pixels == NULL )
        return 0;

    GLuint    textureId;

    glGenTextures   ( 1, &textureId );
    glEnable        ( GL_TEXTURE_CUBE_MAP_ARB );
    glBindTexture   ( GL_TEXTURE_CUBE_MAP_ARB, textureId );
    glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                         // set 1-byte alignment
    glTexParameteri ( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );


    for ( int side = 0; side < 6; side++ )
    {
        for ( int x = 0; x < cubeSize; x++ )
            for ( int y = 0; y < cubeSize; y++ )
            {
                int offs = 3 * (y * cubeSize + x);

                getCubeVector ( side, cubeSize, x, y, v );

                pixels [offs    ] = (byte)(128 + 127 * v.x);
                pixels [offs + 1] = (byte)(128 + 127 * v.y);
                pixels [offs + 2] = (byte)(128 + 127 * v.z);
            }

        if ( mipmap )
            gluBuild2DMipmaps ( GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + side, GL_RGB,
                                cubeSize, cubeSize, GL_RGB, GL_UNSIGNED_BYTE, pixels );
        else
            glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + side, 0, GL_RGB,
                                cubeSize, cubeSize, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

    }

    free ( pixels );

    if ( mipmap )
        glTexParameteri ( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    else
        glTexParameteri ( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexParameteri ( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glDisable ( GL_TEXTURE_CUBE_MAP_ARB );

    return textureId;
}

GLuint    createCubeMap ( bool mipmap, const char * maps [] )
{
    GLuint    textureId;

    glGenTextures   ( 1, &textureId );
    glBindTexture   ( GL_TEXTURE_CUBE_MAP_ARB, textureId );
    glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                         // set 1-byte alignment

    glTexParameterf ( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf ( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    for ( int i = 0; i < 6; i++ )
        if ( !loadTexture ( GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i, mipmap, maps [i] ) )
            return 0;

    glEnable ( GL_TEXTURE_CUBE_MAP_ARB );

    if ( mipmap )                                                       // XXX ?
    {
        glTexParameteri ( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri ( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
    else
    {
        glTexParameteri ( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri ( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }

    return textureId;
}

GLuint    createTexture2D ( bool mipmap, const char * fileName )
{
    GLuint    textureId;

    glGenTextures   ( 1, &textureId );
    glBindTexture   ( GL_TEXTURE_2D, textureId );
    glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                         // set 1-byte alignment

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set texture to repeat mode
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    if ( !loadTexture ( GL_TEXTURE_2D, mipmap, fileName ) )
        return 0;

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    return textureId;
}

GLuint    createTexture1D ( bool mipmap, const char * fileName )
{
    GLuint    textureId;

    glGenTextures   ( 1, &textureId );
    glBindTexture   ( GL_TEXTURE_1D, textureId );
    glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                         // set 1-byte alignment

    glTexParameteri ( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set texture to repeat mode
    glTexParameteri ( GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    if ( !loadTexture ( GL_TEXTURE_1D, mipmap, fileName ) )
        return 0;

    glTexParameteri ( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    return textureId;
}

#ifndef _WIN32
    
	#define stricmp strcasecmp

#endif



Texture * getTexture ( const char * fileName )
{
    const char * ext = strrchr ( fileName, '.' );

    if ( ext == NULL )
        return NULL;

    Data    * data = getFile ( fileName );
    Texture * tex = NULL;

    if ( data == NULL || data -> getLength () < 1 )
    {
        fprintf ( stderr, "Cannot load %s\n", fileName );

        return NULL;
    }

    if ( !stricmp ( ext + 1, "bmp" ) )
    {
        BmpLoader   loader;

        tex = loader.load ( data );
    }
    else
    if ( !stricmp ( ext + 1, "tga" ) )
    {
        TgaLoader   loader;

        tex = loader.load ( data );
    }
    else
    if ( !stricmp ( ext + 1, "dds" ) )
    {
        DdsLoader   loader;

        tex = loader.load ( data );
    }
    else
    if ( !stricmp ( ext + 1, "jpg" ) || !stricmp ( ext + 1, "jpeg" ) )
    {
        JpegLoader  loader;

        tex = loader.load ( data );
    }
    else
    if ( !stricmp ( ext + 1, "png" ) )
    {
        PngLoader   loader;

        tex = loader.load ( data );
    }

    if ( tex == NULL )
        fprintf ( stderr, "No loader for %s !!!\n", fileName );

    delete data;

    return tex;
}

bool    loadTexture ( int target, bool mipmap, const char * fileName )
{
    Texture * texture = getTexture ( fileName );

    if ( texture == NULL )
        return false;

    texture -> upload ( target, mipmap );

    if ( texture -> isMipmapped () )
        glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    else
        glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    delete texture;

    return true;
}

//
// Code to work with normal maps was
// based on code from Mark J. Kilgard article
// "A Practical and Robust Bump-mapping Technique for Today's GPUs"
//

Texture * convertHeight2NormalMap ( byte * pixels, int width, int height, float k )
{
    float     oneOver255 = 1.0 / 255.0;
    Texture * normalMap  = new Texture ( width, height, 4 );

    if ( normalMap == NULL )
        return NULL;

    byte    * out = normalMap -> getData ();

    if ( out == NULL )
    {
        delete normalMap;

        return NULL;
    }

    int offs = 0;                   // offset to normalMap

    for ( int i = 0; i < height; i++ )
        for ( int j = 0; j < width; j++ )
        {
                                    // convert height values to [0,1] range
            float   c  = pixels [i*width              + j]           * oneOver255;
            float   cx = pixels [i*width              + (j+1)%width] * oneOver255;
            float   cy = pixels [((i+1)%height)*width + j]           * oneOver255;

                                    // find derivatives
            float   dx = (c - cx) * k;
            float   dy = (c - cy) * k;

                                    // normalize
            float   len = (float) sqrt ( dx*dx + dy*dy + 1 );

                                    // get normal
            float   nx = dx   / len;
            float   ny = dy   / len;
            float   nz = 1.0f / len;

                                    // now convert to color and store in map
            out [offs    ] = (byte)(128 + 127*nx);
            out [offs + 1] = (byte)(128 + 127*ny);
            out [offs + 2] = (byte)(128 + 127*nz);
            out [offs + 3] = 255;

            offs += 4;
        }

    return normalMap;
}

//
// routine to downsample normal map
// renormalizes new values and stores in alpha-channel length before normalization
//

Texture * downSampleNormaMap ( Texture * old, int w2, int h2 )
{
    if ( old == NULL )
        return NULL;

    Texture * map = new Texture ( w2, h2, 4 );

    if ( map == NULL || map -> getData () == NULL )
    {
        delete map;

        return NULL;
    }

    float   oneOver127 = 1.0f / 127.0f;
//    float   oneOver255 = 1.0f / 255.0f;

    byte  * in  = old -> getData   ();
    byte  * out = map -> getData   ();
    int     w   = old -> getWidth  ();
    int     h   = old -> getHeight ();
    float   v [3];                      // here we will store x, y, z components

    for ( int i = 0; i < h; i += 2 )
        for ( int j = 0; j < w; j += 2 )
        {
            int iNext = (i+1) % h;
            int jNext = (j+1) % w;
			
                                // sum up values for RGB components, converting to [-1,1]
            for ( int k = 0; k < 3; k++ )
                v [k]  = oneOver127 * (in [k + 4*(i*w     + j)] + in [k + 4*(i*w     + jNext)] +
                                       in [k + 4*(iNext*w + j)] + in [k + 4*(iNext*w + jNext)]) - 4;

                                // compute length of (x,y,z)
            float   length = (float)sqrt ( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );

                                // check for degenerated case
            if ( length < EPS )
            {
                v [0] = 0;
                v [1] = 0;
                v [2] = 1;
            }
            else
            {
                v [0] /= length;
                v [1] /= length;
                v [2] /= length;
            }

            int index = ((i >> 1)*w2 + (j >> 1)) << 2;

                                // pack normalized vector into color values
            out [index]     = (byte)(128 + 127*v[0]);
            out [index + 1] = (byte)(128 + 127*v[1]);
            out [index + 2] = (byte)(128 + 127*v[2]);

                                // store averaged length as alpha component
            length *= 255*0.25; // since it was build on 2x2 summed values

            if ( length > 1.0f - EPS )
                out [index + 3] = 255;
            else
                out [index + 3] = (byte)length;
        }

    return map;
}

//
// routine to load and mipmap a normal map
//

void    loadNormalMap ( int target, bool mipmap, Texture * map )
{
    int       level  = 0;
    int       width  = map -> getWidth  ();
    int       height = map -> getHeight ();
    Texture * cur    = map;
    Texture * next   = NULL;

                        // load the original map
    glTexImage2D ( target, level, GL_RGBA8, width, height,
                   0, map -> getFormat (), GL_UNSIGNED_BYTE, map -> getData () );

    if ( !mipmap )
        return;
                        // downsample texture
    while ( width > 1 || height > 1 )
    {
        level++;

                        // compute new size for this level
        int newWidth  = width  >> 1;
        int newHeight = height >> 1;

        if ( newWidth < 1 )
            newWidth = 1;

        if ( newHeight < 1 )
            newHeight = 1;

        next = downSampleNormaMap ( cur, newWidth, newHeight );

        if ( next == NULL )
            return;

        glTexImage2D ( target, level, GL_RGBA8, newWidth, newHeight,
                       0, next -> getFormat (), GL_UNSIGNED_BYTE, next -> getData () );

                        // prepare for next iteration
        width  = newWidth;
        height = newHeight;

        if ( cur != map )       // release old temporary textures
            delete cur;

        cur = next;
    }

    if ( cur != map )           // remove last temporary
        delete cur;
}

//
// routine to create bump map from normal map
//

GLuint    createNormalMap ( bool mipmap, const char * fileName )
{
    Texture * texture = getTexture ( fileName );

    if ( texture == NULL )
        return 0;

                                                // check whether it has alpha-channel for mipmapping
    if ( mipmap && texture -> getNumComponents () == 3 )
    {                                           // create RGBA texture with alpha=255
        int       width  = texture -> getWidth  ();
        int       height = texture -> getHeight ();
        Texture * t2     = new Texture ( width, height, 4 );

                                                // now copy all data adding alpha value
        byte * src = texture -> getData ();
        byte * dst = t2      -> getData ();

        for ( int i = 0; i < height; i++ )
            for ( int j = 0; j < width; j++ )
            {
                *dst++ = *src++;                // copy RGB data
                *dst++ = *src++;
                *dst++ = *src++;
                *dst++ = 255;                   // append alpha data
            }

        delete texture;                         // we no longer need it

        texture = t2;                           // make new RGBA texture take it's role
    }

    GLuint    textureId;

    glGenTextures   ( 1, &textureId );
    glBindTexture   ( GL_TEXTURE_2D, textureId );
    glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                         // set 1-byte alignment

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set texture to repeat mode
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    loadNormalMap   ( GL_TEXTURE_2D, mipmap, texture );

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    if ( mipmap )
        glTexParameteri   ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    else
        glTexParameteri   ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    delete texture;

    return textureId;
}

//
// routine to create normal map from heightmap textue
//

GLuint    createNormalMapFromHeightMap ( bool mipmap, const char * fileName, float scale )
{
    Texture * texture = getTexture ( fileName );

    if ( texture == NULL )
        return 0;

    int numComponents = texture -> getNumComponents ();

    if ( numComponents != 1 && numComponents != 3 && numComponents != 4 )
    {
        delete texture;

        return 0;
    }

    int     width  = texture -> getWidth  ();
    int     height = texture -> getHeight ();
    byte  * pixels = new byte [width * height];
    byte  * buf    = pixels;
    byte  * src    = texture -> getData ();

                                        // now convert color image to grey-scale
    if ( numComponents == 1 )
    {
        memcpy ( buf, src, width * height );
    }
    if ( numComponents == 3 )
    {
        for ( int i = 0; i < height; i++ )
            for ( int j = 0; j < width; j++, src += 3 )
                                        // use simple averaging
                *buf++ = (src [0] + src [1] + src [2]) / 3;
    }
    else
    if ( numComponents == 4 )
    {
        for ( int i = 0; i < height; i++ )
            for ( int j = 0; j < width; j++, src += 4 )
                                        // use simple averaging of RGB, ignore alpha
                *buf++ = (src [0] + src [1] + src [2]) / 3;
    }

    delete texture;                     // we no longer need it

    Texture * nmap = convertHeight2NormalMap ( pixels, width, height, scale );

                                        // now load nmap
    GLuint    textureId;

    glGenTextures   ( 1, &textureId );
    glBindTexture   ( GL_TEXTURE_2D, textureId );
    glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                         // set 1-byte alignment

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set texture to repeat mode
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    loadNormalMap   ( GL_TEXTURE_2D, mipmap, nmap );

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    if ( mipmap )
        glTexParameteri   ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    else
        glTexParameteri   ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    delete nmap;
    delete pixels;

    return textureId;
}

                            // simple check whether the file exists
bool    fileExist ( const char * fileName )
{
#ifdef  _WIN32
    struct _stat    buf;

    return _stat ( fileName, &buf ) != -1;
#else
    struct stat buf;

    return stat ( fileName, &buf ) != -1;
#endif
}

bool    saveScreenShot ( const char * fileName )
{
    int    width  = glutGet ( GLUT_WINDOW_WIDTH  );
    int    height = glutGet ( GLUT_WINDOW_HEIGHT );
    byte * ptr    = (byte *) malloc ( 3 * width * height );

    if ( ptr == NULL )
        return false;

                                            // get image from window
    glReadBuffer ( GL_FRONT );
    glReadPixels ( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, ptr );

                                            // now save it as 24-bit uncompressed tga
    TgaHeader   hdr;

    memset ( &hdr, '\0', sizeof ( hdr ) );

    hdr.imageType         = 2;              // unencoded image
    hdr.width             = width;
    hdr.height            = height;
    hdr.pixelSize         = 24;

    int file = open ( fileName, O_WRONLY | O_BINARY | O_CREAT, S_IWRITE );

    if ( file == -1 )
    {
        free ( ptr );

        return false;
    }
                                            // write header
    write ( file, &hdr, sizeof ( hdr ) );


                                            // swap R and B channels
    int     numPixels = width * height;
    byte  * pixel     = ptr;
    byte    temp;

    for ( int i = 0; i < numPixels; i++, pixel += 3 )
    {
        temp      = pixel [0];
        pixel [0] = pixel [2];
        pixel [2] = temp;
    }
                                            // and write it down
    write ( file, ptr, width * height * 3 );
    close ( file );
                                            // cleanup
    free ( ptr );

    return true;
}
