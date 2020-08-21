//
// Example on glow in OpenGL
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#include    "libExt.h"

#ifdef	MACOSX
	#include	<GLUT/glut.h>
#else
	#include	<glut.h>
#endif

#include    <stdio.h>
#include    <stdlib.h>

#include    "libTexture.h"
#include    "Vector3D.h"
#include    "Vector2D.h"
#include    "Torus.h"
#include    "VertexProgram.h"
#include    "FragmentProgram.h"

Vector3D    eye   ( 7, 5, 7 );          // camera position
Vector3D    light ( 5, 0, 4 );          // light position
unsigned    envMap;                 // decal map
unsigned    bumpMap;
float       angle = 0;
Torus       torus ( 1, 3, 30, 30 );
Vector3D    rot ( 0, 0, 0 );
int         mouseOldX = 0;
int         mouseOldY = 0;

VertexProgram   vertexProgram;
FragmentProgram fragmentProgram;

/////////////////////////////////////////////////////////////////////////////////

void init ()
{
    glClearColor ( 0.0, 0.0, 0.0, 1.0 );
    glEnable     ( GL_DEPTH_TEST );
    glDepthFunc  ( GL_LEQUAL );

    glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
    glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void display ()
{
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                                                // setup texture units
                                                // bind bump (normal) map to texture unit 0
    glMatrixMode   ( GL_MATRIX0_ARB );
    glLoadIdentity ();
    glRotatef    ( rot.x, 1, 0, 0 );
    glRotatef    ( rot.y, 0, 1, 0 );
    glRotatef    ( rot.z, 0, 0, 1 );

    glMatrixMode   ( GL_MODELVIEW );
    glPushMatrix   ();

    glRotatef    ( rot.x, 1, 0, 0 );
    glRotatef    ( rot.y, 0, 1, 0 );
    glRotatef    ( rot.z, 0, 0, 1 );

                                    // draw normal torus with env.mapping
    glActiveTextureARB ( GL_TEXTURE0_ARB );
    glBindTexture      ( GL_TEXTURE_2D, envMap );
    glEnable           ( GL_TEXTURE_GEN_S );
    glEnable           ( GL_TEXTURE_GEN_T );
    glTexGeni          ( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
    glTexGeni          ( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );

    torus.draw ();

    glDisable           ( GL_TEXTURE_GEN_S );
    glDisable           ( GL_TEXTURE_GEN_T );

    glActiveTextureARB ( GL_TEXTURE0_ARB );
    glEnable           ( GL_TEXTURE_2D );
    glBindTexture      ( GL_TEXTURE_2D, bumpMap );

    glEnable ( GL_BLEND );
    glBlendFunc ( GL_ONE, GL_ONE );

    vertexProgram.enable   ();
    vertexProgram.bind     ();
    fragmentProgram.enable ();
    fragmentProgram.bind   ();

    torus.draw  ();

    vertexProgram.disable   ();
    fragmentProgram.disable ();

    glDisable       ( GL_BLEND );
    glPopMatrix     ();
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
                    0, 0, 1 );              // up
}

void motion ( int x, int y )
{
    rot.y -= ((mouseOldY - y) * 180.0f) / 200.0f;
    rot.z -= ((mouseOldX - x) * 180.0f) / 200.0f;
    rot.x  = 0;

    if ( rot.z > 360 )
        rot.z -= 360;

    if ( rot.z < -360 )
        rot.z += 360;

    if ( rot.y > 360 )
        rot.y -= 360;

    if ( rot.y < -360 )
        rot.y += 360;

    mouseOldX = x;
    mouseOldY = y;

    glutPostRedisplay ();
}

void mouse ( int button, int state, int x, int y )
{
    if ( state == GLUT_DOWN )
    {
        mouseOldX = x;
        mouseOldY = y;
    }
}

void key ( unsigned char key, int x, int y )
{
    if ( key == 27 || key == 'q' || key == 'Q' )        //quit requested
        exit ( 0 );
}

void    animate ()
{
    angle  = 0.004f * glutGet ( GLUT_ELAPSED_TIME );

    light.x = 2*cos ( angle );
    light.y = 2*sin ( angle );
    light.z = 3 + 0.3 * sin ( angle / 3 );

                                        // setup data
    vertexProgram.local [0] = eye;
    vertexProgram.local [1] = light;

    glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
                                //initialise glut
    glutInit            ( &argc, argv );
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize  ( 500, 500 );


                                // create window
    glutCreateWindow ( "OpenGL glowing torus via fragment & vertex programs" );

                                // register handlers
                                // register handlers
    glutDisplayFunc  ( display );
    glutReshapeFunc  ( reshape );
    glutKeyboardFunc ( key     );
    glutMouseFunc    ( mouse   );
    glutMotionFunc   ( motion  );
    glutIdleFunc     ( animate );

    init           ();
    initExtensions ();
    printfInfo     ();

    if ( !isExtensionSupported ( "GL_ARB_multitexture" ) )
    {
        printf ( "ARB_multitexture NOT supported.\n" );

        return 1;
    }

    if ( !isExtensionSupported ( "GL_ARB_vertex_program" ) )
    {
        printf ( "GL_ARB_vertex_program NOT supported" );

        return 2;
    }

    if ( !isExtensionSupported ( "GL_ARB_fragment_program" ) )
    {
        printf ( "GL_ARB_fragment_program NOT supported" );

        return 3;
    }


    bumpMap = createTexture2D  ( false, "../Textures/Bumpmaps/bump.tga" );
    envMap  = createTexture2D  ( true,  "../Textures/stars.bmp" );

    if ( !vertexProgram.load ( "glow.vp" ) )
    {
        printf ( "Error loading vertex program:\n %s\n", vertexProgram.getErrorString ().c_str () );

        return 1;
    }

    if ( !fragmentProgram.load ( "glow.fp" ) )
    {
        printf ( "Error loading fragment program:\n %s\n", fragmentProgram.getErrorString ().c_str () );

        return 2;
    }

    glutMainLoop ();

    return 0;
}
