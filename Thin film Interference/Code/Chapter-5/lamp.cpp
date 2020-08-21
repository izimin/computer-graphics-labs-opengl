//
// Sample to OpenGL cubic maps - using a lamp with color plate (mask) attached to it.
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#include    "libExt.h"
#include    "libTexture.h"
#include    "Vector3D.h"
#include    "Vector2D.h"

#ifdef	MACOSX
	#include	<GLUT/glut.h>
#else
	#include	<glut.h>
#endif

#include    <stdio.h>
#include    <stdlib.h>

Vector3D    eye   ( 2, 2, 2 );          // camera position
Vector3D    light ( 0.5, 0, 1 );        // light position
float    angle    = 0;
float    angle2   = 0;
int      reflMode = GL_REFLECTION_MAP_ARB;
int      object   = 0;
unsigned texture;
unsigned lampMap;

Vector3D    v  [12];
Vector2D    t0 [12];
Vector3D    t1 [12];

void init ()
{
    glClearColor ( 0, 0, 0, 1 );
    glEnable     ( GL_DEPTH_TEST );

    glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
    glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void display ()
{
    int i;

    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode       ( GL_MODELVIEW );

                                        // setup normal texture
    glActiveTextureARB ( GL_TEXTURE0_ARB );
    glEnable           ( GL_TEXTURE_2D );
    glDisable          ( GL_TEXTURE_CUBE_MAP_ARB );
    glBindTexture      ( GL_TEXTURE_2D, texture );
    glTexEnvi          ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

                                        // setup lamp texture
    glActiveTextureARB ( GL_TEXTURE1_ARB );
    glEnable           ( GL_TEXTURE_CUBE_MAP_ARB );
    glBindTexture      ( GL_TEXTURE_CUBE_MAP_ARB, lampMap );
    glTexEnvi          ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

                                        // apply rotation to lamp
    glMatrixMode       ( GL_TEXTURE );
    glLoadIdentity     ();
    glRotatef          ( angle,  1, 1, 0 );
    glRotatef          ( angle2, 0, 1, 1 );

                                        // compute texture coords into lamp
    for ( i = 0; i < 12; i++ )
        t1 [i] = v [i] - light;

                                        // now draw
    glBegin ( GL_QUADS );

    for ( i = 0; i < 12; i++ )
    {
        glMultiTexCoord2fv ( GL_TEXTURE0_ARB, t0 [i] );
        glMultiTexCoord3fv ( GL_TEXTURE1_ARB, t1 [i] );
        glVertex3fv        ( v [i]                   );
    }

    glEnd   ();

    glutSwapBuffers ();
}

void reshape ( int w, int h )
{
   glViewport     ( 0, 0, (GLsizei)w, (GLsizei)h );
   glMatrixMode   ( GL_PROJECTION );
   glLoadIdentity ();
   gluPerspective ( 60.0, (GLfloat)w/(GLfloat)h, 1.0, 60.0 );
   glMatrixMode   ( GL_MODELVIEW );
   glLoadIdentity ();
   gluLookAt      ( eye.x, eye.y, eye.z,    // eye
                    0, 0, 0,                // center
                    0.0, 0.0, 1.0 );        // up
}

void key ( unsigned char key, int x, int y )
{
    if ( key == 27 || key == 'q' || key == 'Q' )        // quit requested
        exit ( 0 );
}

void    animate ()
{
    angle  = 0.04f * glutGet ( GLUT_ELAPSED_TIME );
    angle2 = 0.01f * glutGet ( GLUT_ELAPSED_TIME );

    glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
                                // initialize glut
    glutInit            ( &argc, argv );
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize  ( 500, 500 );


                                // create window
    int win = glutCreateWindow ( "OpenGL cubemap projector example" );

                                // register handlers
    glutDisplayFunc  ( display );
    glutReshapeFunc  ( reshape );
    glutKeyboardFunc ( key );
    glutIdleFunc     ( animate );

    if ( !isExtensionSupported ( "GL_ARB_multitexture" ) )
    {
        printf ( "ARB_multitexture NOT supported.\n" );

        return 1;
    }

    if ( !isExtensionSupported ( "GL_ARB_texture_cube_map" ) )
    {
        printf ( "ARB_texture_cube_map NOT supported.\n" );

        return 1;
    }

    init           ();
    initExtensions ();

    const char * faces [6] =
    {
        "../Textures/floor.bmp",
        "../Textures/floor.bmp",
        "../Textures/floor.bmp",
        "../Textures/floor.bmp",
        "../Textures/floor.bmp",
        "../Textures/floor.bmp"
    };

    lampMap = createCubeMap   ( true, faces );
    texture = createTexture2D ( true, "../Textures/block.bmp" );

    v [0 ] = Vector3D ( -1, -1, 0 );
    v [1 ] = Vector3D (  1, -1, 0 );
    v [2 ] = Vector3D (  1,  1, 0 );
    v [3 ] = Vector3D ( -1,  1, 0 );

    v [4 ] = Vector3D ( -1, -1, 0 );
    v [5 ] = Vector3D (  1, -1, 0 );
    v [6 ] = Vector3D (  1, -1, 2 );
    v [7 ] = Vector3D ( -1, -1, 2 );

    v [8 ] = Vector3D ( -1, -1, 0 );
    v [9 ] = Vector3D ( -1,  1, 0 );
    v [10] = Vector3D ( -1,  1, 2 );
    v [11] = Vector3D ( -1, -1, 2 );


                                                // setup texture coordinates for bump map
    t0 [0 ] = Vector2D ( 0, 0 );
    t0 [1 ] = Vector2D ( 1, 0 );
    t0 [2 ] = Vector2D ( 1, 1 );
    t0 [3 ] = Vector2D ( 0, 1 );

    t0 [4 ] = Vector2D ( 0, 0 );
    t0 [5 ] = Vector2D ( 1, 0 );
    t0 [6 ] = Vector2D ( 1, 1 );
    t0 [7 ] = Vector2D ( 0, 1 );

    t0 [8 ] = Vector2D ( 0, 0 );
    t0 [9 ] = Vector2D ( 1, 0 );
    t0 [10] = Vector2D ( 1, 1 );
    t0 [11] = Vector2D ( 0, 1 );

    glutMainLoop ();

    return 0;
}
