//
// Create a 3D periodic noise texture and save it in DDS file
//
// Author: Alex V. Boreskoff
//

#include    "libExt.h"
#include    "noise.h"
#include    "libTexture3D.h"

#ifdef	_WIN32
	#include	<malloc.h>
#else
	#include	<stdlib.h>
#endif

#include    <stdio.h>
#include    <stdlib.h>

class   VCube
{
    int         n;
    int         n2;
    Vector3D  * v;

public:
    VCube ( int size )
    {
        n  = size;
        n2 = n*n;
        v  = new Vector3D [n*n*n];
    }

    ~VCube ()
    {
        delete v;
    }

    Vector3D&   vertex ( int i, int j, int k )
    {
        return v [i + j*n + k*n2];
    }
};

inline  float   drop ( float t )
{
    return 1.0f - pow ( t, 10 );
}

const Vector3D  offsGreen ( 0.12345, 0.23456, 0.34567 );
const Vector3D  offsBlue  ( 0.35792, 0.46803, 0.79486 );

static  byte * createNoiseTextureBytes ( int size, int octaves )
{
                                    // build cube lattice with noise
    VCube       vc ( size );
    int         i, j, k;
    float       inv  = 1.0f / (float)(size-1);
    float       inv2 = inv * octaves;
    Vector3D    v;
    Noise       noise;

                                    // fill the lattice with noise
    for ( i = 0; i < size; i++ )
        for ( j = 0; j < size; j++ )
            for ( k = 0; k < size; k++ )
            {
                v.x = i * inv2;
                v.y = j * inv2;
                v.z = k * inv2;

                vc.vertex ( i, j, k ) = Vector3D ( noise.noise ( v ),
                                                   noise.noise ( v + offsGreen ),
                                                   noise.noise ( v + offsBlue  ) );
            }

                                    // now we want to make it periodic by adjuisting
                                    // border values
    VCube   vc2 ( size );

    for ( i = 0; i < size; i++ )
        for ( j = 0; j < size; j++ )
            for ( k = 0; k < size; k++ )
            {
                float   xDrop = drop ( i * inv );
                float   yDrop = drop ( j * inv );
                float   zDrop = drop ( k * inv );

                vc2.vertex ( i, j, k ) = xDrop    *yDrop    *zDrop    *vc.vertex (i,j,k) +
                                         (1-xDrop)*yDrop    *zDrop    *vc.vertex ( size-1-i, j, k ) +
                                         xDrop    *(1-yDrop)*zDrop    *vc.vertex ( i, size-1-j, k ) +
                                         (1-xDrop)*(1-yDrop)*zDrop    *vc.vertex ( size-1-i, size-1-j, k ) +
                                         xDrop    *yDrop    *(1-zDrop)*vc.vertex (i,j,size-1-k) +
                                         (1-xDrop)*yDrop    *(1-zDrop)*vc.vertex ( size-1-i, j, size-1-k ) +
                                         xDrop    *(1-yDrop)*(1-zDrop)*vc.vertex ( i, size-1-j, size-1-k ) +
                                         (1-xDrop)*(1-yDrop)*(1-zDrop)*vc.vertex ( size-1-i, size-1-j, size-1-k );
            }

                                        // now create 3D texture
    byte * noiseTable = (byte *) malloc ( size * size * size * 3 );

    if ( noiseTable == NULL )
        return NULL;

    byte  * ptr  = noiseTable;

    for ( i = 0; i < size; i++ )
        for ( j = 0; j < size; j++ )
            for ( k = 0; k < size; k++ )
            {
                Vector3D    n ( vc2.vertex ( i, j, k ) );

                *ptr++ = (byte)(127.5 * ( 1.0 + n.x ) );
                *ptr++ = (byte)(127.5 * ( 1.0 + n.y ) );
                *ptr++ = (byte)(127.5 * ( 1.0 + n.z ) );
            }

    return noiseTable;
}

bool        saveNoiseTexture3D   ( int size, int octaves, const char * fileName )
{
    byte * data = createNoiseTextureBytes ( size, octaves );

    if ( data == NULL )
        return false;

    bool    result = saveTexture3D ( size, size, size, 3, data, fileName );

    free ( data );

    return result;
}

int main ( int argc, char * argv [] )
{
    if ( argc != 4 )
    {
        printf ( "Usage:\n" );
        printf ( "\tmake-3d-noise size octaves filename\n" );
        printf ( "Where\n\tsize     is texture size in texels,\n" );
        printf ( "\toctaves  is number of \"periods\" of noise\n" );
        printf ( "\tfilename is the name (with dds extension) texture will be saved to.\n" );

        return 1;
    }

    int size    = atoi ( argv [1] );
    int octaves = atoi ( argv [2] );

    if ( size < 1 || octaves < 1 )
    {
        printf ( "Illegal data.\n" );

        return 1;
    }

    printf ( "Building %dx%dx%d noise texture...\n", size, size, size );

    if ( saveNoiseTexture3D ( size, octaves, argv [3] ) )
        printf ( "3D noise texture successfully created !\n" );
    else
        printf ( "Error creating texture.\n" );

    return 0;
}
