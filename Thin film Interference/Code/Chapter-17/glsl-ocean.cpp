//
// Sample showing how to use GLSL programs
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
#include    "libTexture3D.h"
#include    "Vector3D.h"
#include    "Vector2D.h"
#include    "Vector4D.h"
#include    "Data.h"
#include    "GlslProgram.h"
#include    "Torus.h"

Vector3D    eye   ( 0, 0, 1 );          // camera position
Vector3D    light ( 0, 1.5, -6 );         // light position
float       angle = 0;
Vector3D    rot ( 0, 0, 0 );
int         mouseOldX = 0;
int         mouseOldY = 0;
unsigned    noiseMap;
unsigned    noise2Map;
GlslProgram program;
GlslProgram skyProgram;

int         frames       = 0;
int         totalFrames  = 0;
int         elapsedTime  = 0;
float       fps          = 0;

const char * caption = "GLSL Ocean demo";

/////////////////////////////////////////////////////////////////////////////////

void    updateFps ()
{
    frames++;
    totalFrames++;

    if ( (frames % 5) == 0 )                        // compute every 5 frames
    {
        int     time2 = glutGet ( GLUT_ELAPSED_TIME );
        char    str [256];

        fps         = (float)frames / (0.001 * (time2 - elapsedTime));
        elapsedTime = time2;
        frames      = 0;

        sprintf ( str, "%s, FPS : %5.2f", caption, fps );

        glutSetWindowTitle ( str );
    }
}

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

    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ();
                                                // the sun
    glColor3f       ( 1, 0.8, 0 );
    glTranslatef ( light.x, light.y, light.z );
    glutSolidSphere ( 0.5, 30, 30);
    glTranslatef ( -light.x, -light.y, -light.z );


    glEnable    ( GL_BLEND );
    glBlendFunc ( GL_ONE, GL_ONE );
                                                // the sky
    glActiveTextureARB ( GL_TEXTURE0_ARB );
    glBindTexture      ( GL_TEXTURE_3D, noiseMap );

    glActiveTextureARB ( GL_TEXTURE1_ARB );
    glBindTexture      ( GL_TEXTURE_2D, noise2Map );

    glColor3f       ( 0, 0.8, 0.8 );

    skyProgram.bind ();

    glBegin ( GL_QUADS );

        glTexCoord2f ( 0.0, 0.0);
        glVertex3f   ( -10.0, 1.0, 1.0);
        glTexCoord2f ( 1.0, 0.0);
        glVertex3f   ( 10.0, 1.0, 1.0);
        glTexCoord2f ( 1.0, 1.0);
        glVertex3f   ( 10.0, 0.0, -10.0);
        glTexCoord2f ( 0.0, 1.0);
        glVertex3f   ( -10.0, 0.0, -10.0);

    glEnd ();

    skyProgram.unbind ();

    glDisable ( GL_BLEND );

                                                // the ocean
    glColor3f       ( 0, 0.4, 0.8 );

    program.bind ();

    glBegin ( GL_QUADS );
        glTexCoord2f(0, 0);
        glVertex3f(-10.0, -1.0, 1.0);
        glTexCoord2f(1, 0);
        glVertex3f(10.0, -1.0, 1.0);
        glTexCoord2f(1, 1);
        glVertex3f(10.0, 0.0, -10.0);
        glTexCoord2f(0, 1);
        glVertex3f(-10.0, 0.0, -10.0);
    glEnd();

    program.unbind ();

    glPopMatrix ();

    updateFps ();

    glutSwapBuffers ();
}

void reshape ( int w, int h )
{
   glViewport     ( 0, 0, (GLsizei)w, (GLsizei)h );
   glMatrixMode   ( GL_PROJECTION );
                                            // factor all camera ops into projection matrix
    glLoadIdentity ();
    gluPerspective ( 60.0, (GLfloat)w/(GLfloat)h, 1.0, 60.0 );
    gluLookAt      ( eye.x, eye.y, eye.z,
                     0, 0, 0,
                     0, 1, 0 );
//    glRotatef      ( rot.x, 0, 1, 0 );
//    glRotatef      ( rot.y, 1, 0, 0 );
    glMatrixMode   ( GL_MODELVIEW );
    glLoadIdentity ();
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
    if ( key == 27 || key == 'q' || key == 'Q' )    //  quit requested
        exit ( 0 );
}

void    animate ()
{
    angle  = 0.001f * glutGet ( GLUT_ELAPSED_TIME );
/*
    light.x = 2*cos ( angle );
    light.y = 2*sin ( angle );
    light.z = 3 + 0.3 * sin ( angle / 3 );
*/
    program.bind ();
    program.setUniformVector ( "eyePos",   Vector4D ( eye,   1 ) );
    program.setUniformVector ( "lightPos", Vector4D ( light, 1 ) );
    program.setUniformVector ( "time",     Vector4D ( angle, 0, 0, 0 ) );
    program.unbind ();

    skyProgram.bind ();
    skyProgram.setUniformVector ( "eyePos",   Vector4D ( eye,   1 ) );
    skyProgram.setUniformVector ( "lightPos", Vector4D ( light, 1 ) );
    skyProgram.setUniformVector ( "time",     Vector4D ( angle, 0, 0, 0 ) );
    skyProgram.unbind ();

    glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
                                // initialize glut
    glutInit            ( &argc, argv );
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize  ( 500, 500 );


                                // create window
    glutCreateWindow ( caption );

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

    assertExtensionsSupported ( "GL_ARB_shading_language_100 GL_ARB_shader_objects" );

    if ( !program.loadShaders ( "ocean.vsh", "ocean.fsh" ) )
    {
        printf ( "Error loading ocean shaders:\n%s\n", program.getLog ().c_str () );

        return 3;
    }

    if ( !skyProgram.loadShaders ( "sky.vsh", "sky.fsh" ) )
    {
        printf ( "Error loading sky shaders:\n%s\n", skyProgram.getLog ().c_str () );

        return 3;
    }

    noiseMap  = createTexture3D ( false, "../Textures/Noise/noise-3D.dds" );
    noise2Map = createTexture2D ( true, "noise-1.png" );

                                        // install program object as part of
                                        // current state
    program.bind ();
    program.setTexture ( "myNoise",  0 );
    program.setTexture ( "myNoise2", 1 );
    program.unbind ();

    skyProgram.bind ();
    skyProgram.setTexture ( "myNoise",  0 );
    skyProgram.setTexture ( "myNoise2", 1 );
    skyProgram.unbind ();

    glutMainLoop ();

    return 0;
}
