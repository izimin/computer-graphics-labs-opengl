//
// Sample to show how to per-pixel dot3 bumpmapping in OpenGL
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#include    "libExt.h"
#include    "libTexture.h"
#include    "Vector3D.h"
#include    "Vector2D.h"
#include    "Torus.h"

#ifdef	MACOSX
	#include	<GLUT/glut.h>
#else
	#include	<glut.h>
#endif

#include    <stdio.h>
#include    <stdlib.h>

Vector3D    eye   ( 7, 5, 7 );          // camera position
Vector3D    light ( 5, 0, 4 );          // light position
unsigned   normCubeMap;                // normalization cubemap id
unsigned   bumpMap;                    // normal map
unsigned   decalMap;                   // decal (diffuse) texture
float      angle = 0;

Torus       torus ( 1, 3, 30, 30 );

void init ()
{
    glClearColor ( 0, 0, 0, 1 );
    glEnable     ( GL_DEPTH_TEST );
    glDepthFunc  ( GL_LEQUAL );

    glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
    glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void display ()
{
    int i;

    glClear   ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glColor3f ( 1, 1, 1 );

                                        // draw light source
    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ();

    glTranslatef       ( light.x, light.y, light.z );
    glActiveTextureARB ( GL_TEXTURE0_ARB );
    glDisable          ( GL_TEXTURE_2D );
    glActiveTextureARB ( GL_TEXTURE1_ARB );
    glDisable          ( GL_TEXTURE_CUBE_MAP_ARB );
    glActiveTextureARB ( GL_TEXTURE2_ARB );
    glDisable          ( GL_TEXTURE_2D );
    glColor4f          ( 1, 1, 1, 1 );
    glutSolidSphere    ( 0.1f, 15, 15 );
    glPopMatrix        ();

                                        // bind bump (normal) map to texture unit 0
    glActiveTextureARB ( GL_TEXTURE0_ARB );
    glEnable           ( GL_TEXTURE_2D );
    glBindTexture      ( GL_TEXTURE_2D, bumpMap );

                                            // setup environment for dot3
    glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB );
    glTexEnvi ( GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,  GL_TEXTURE );
    glTexEnvi ( GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,  GL_REPLACE );

                                            // bind normalization cube map to texture unit 1
    glActiveTextureARB ( GL_TEXTURE1_ARB );
    glEnable           ( GL_TEXTURE_CUBE_MAP_ARB );
    glBindTexture      ( GL_TEXTURE_CUBE_MAP_ARB, normCubeMap );

                                            // setup environment for dot3
    glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB );
    glTexEnvi ( GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,  GL_TEXTURE );
    glTexEnvi ( GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,  GL_PREVIOUS_ARB );
    glTexEnvi ( GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,  GL_DOT3_RGB_ARB );

                                            // setup decal texture
    glActiveTextureARB ( GL_TEXTURE2_ARB );
    glEnable           ( GL_TEXTURE_2D );
    glBindTexture      ( GL_TEXTURE_2D, decalMap );

                                            // setup environment for decal texture
    glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB );
    glTexEnvi ( GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,  GL_TEXTURE );
    glTexEnvi ( GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,  GL_PREVIOUS_ARB );
    glTexEnvi ( GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,  GL_MODULATE );

                                                // now draw torus
    torus.calcLightVectors ( light );
    torus.draw             ();

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

    if ( key == 'p' || key == 'P' )                     // saving screenshots
        saveScreenShot ( "dot3-4.tga" );
}

void    animate ()
{
    angle  = 0.004f * glutGet ( GLUT_ELAPSED_TIME );

    light.x = 3.5*cos ( angle );
    light.y = sin ( angle );
    light.z = 4 + sin ( angle / 3 );

    glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
                                // initialize glut
    glutInit            ( &argc, argv );
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize  ( 500, 500 );


                                // create window
    int win = glutCreateWindow ( "OpenGL texture_env_dot3 example 5" );

                                // register handlers
    glutDisplayFunc  ( display );
    glutReshapeFunc  ( reshape );
    glutKeyboardFunc ( key );
    glutIdleFunc     ( animate );

    if ( !isExtensionSupported ( "GL_ARB_texture_env_combine" ) )
    {
        printf ( "ARB_texture_env_combine NOT supported.\n" );

        return 1;
    }

    if ( !isExtensionSupported ( "GL_ARB_texture_cube_map" ) )
    {
        printf ( "ARB_texture_cube_map NOT supported.\n" );

        return 1;
    }

    if ( !isExtensionSupported ( "GL_ARB_texture_env_dot3" ) )
    {
        printf ( "ARB_texture_env_dot3 NOT supported.\n" );

        return 1;
    }

    init           ();
    initExtensions ();

    bumpMap     = createNormalMapFromHeightMap ( false, "../Textures/Bumpmaps/16tile01Bump.png", 5 );
    decalMap    = createTexture2D              ( true,  "../Textures/16tile01.jpg" );
    normCubeMap = createNormalizationCubemap   ( 32 );

    glutMainLoop ();

    return 0;
}
