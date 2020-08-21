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
#include    "Vector3D.h"
#include    "Vector2D.h"
#include    "Data.h"

Vector3D    eye   ( 7, 5, 7 );          // camera position
Vector3D    light ( 5, 0, 4 );          // light position
float       angle = 0;
Vector3D    rot ( 0, 0, 0 );
int         mouseOldX = 0;
int         mouseOldY = 0;

GLhandleARB program        = 0;         // program handles
GLhandleARB vertexShader   = 0;
GLhandleARB fragmentShader = 0;

/////////////////////////////////////////////////////////////////////////////////

//
// Returns 1 if an OpenGL error occurred, 0 otherwise.
//

bool    checkOpenGLError ()
{
    GLenum glErr;
    bool    retCode = true;

    for ( ; ; )
    {
        GLenum  glErr = glGetError ();

        if ( glErr == GL_NO_ERROR )
            return retCode;

        printf ( "glError: %s\n", gluErrorString ( glErr ) );

        retCode = false;
        glErr   = glGetError ();
    }

    return retCode;
}


//
// Print out the information log for a shader object or a program object
//

void printInfoLog ( GLhandleARB object )
{
    GLint       logLength     = 0;
    GLsizei     charsWritten  = 0;
    GLcharARB * infoLog;

    checkOpenGLError();             // Check for OpenGL errors

    glGetObjectParameterivARB ( object, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logLength );

    if ( !checkOpenGLError() )              // check for OpenGL errors
        exit ( 1 );

    if ( logLength > 0 )
    {
        infoLog = (GLcharARB*) malloc ( logLength );

        if ( infoLog == NULL )
        {
            printf("ERROR: Could not allocate InfoLog buffer\n");

            exit ( 1 );
        }

        glGetInfoLogARB ( object, logLength, &charsWritten, infoLog );

        printf ( "InfoLog:\n%s\n\n", infoLog );
        free   ( infoLog );
    }

    if ( !checkOpenGLError () )             // check for OpenGL errors
        exit ( 1 );
}

bool    loadShader ( GLhandleARB shader, const char * fileName )
{
    printf ( "Loading %s\n", fileName );

    Data data ( fileName );

    if ( !data.isOk () || data.isEmpty () )
        exit ( 1 );

    const char * body = (const char *) data.getPtr ( 0 );
    GLint		len   = data.getLength ();
    GLint       compileStatus;

    glShaderSourceARB ( shader, 1, &body, &len );


                                        // compile the particle vertex shader, and print out
    glCompileShaderARB ( shader );

    if ( !checkOpenGLError() )          // check for OpenGL errors
        return false;

    glGetObjectParameterivARB ( shader, GL_OBJECT_COMPILE_STATUS_ARB, &compileStatus );
    printInfoLog              ( shader );

    return compileStatus != 0;
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

    glMatrixMode   ( GL_MODELVIEW );
    glPushMatrix   ();

    glRotatef    ( rot.x, 1, 0, 0 );
    glRotatef    ( rot.y, 0, 1, 0 );
    glRotatef    ( rot.z, 0, 0, 1 );

    glutSolidTeapot ( 2 );

    glPopMatrix ();

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
    angle  = 0.004f * glutGet ( GLUT_ELAPSED_TIME );

    glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
                                // initialize glut
    glutInit            ( &argc, argv );
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize  ( 500, 500 );


                                // create window
    glutCreateWindow ( "Simple example of using GLSL shaders" );

                                // register handlers
    glutDisplayFunc  ( display );
    glutReshapeFunc  ( reshape );
    glutKeyboardFunc ( key );
    glutMouseFunc    ( mouse   );
    glutMotionFunc   ( motion  );
    glutIdleFunc     ( animate );

    init           ();
    initExtensions ();
    printfInfo     ();

    if ( !isExtensionSupported ( "GL_ARB_shading_language_100" ) )
    {
        printf ( "GL_ARB_shading_language_100 NOT supported.\n" );

        return 1;
    }

    if ( !isExtensionSupported ( "GL_ARB_shader_objects" ) )
    {
        printf ( "GL_ARB_shader_objects NOT supported" );

        return 2;
    }

    GLint       linked;

                                        // create a vertex shader object and a fragment shader object
    vertexShader   = glCreateShaderObjectARB ( GL_VERTEX_SHADER_ARB   );
    fragmentShader = glCreateShaderObjectARB ( GL_FRAGMENT_SHADER_ARB );

                                        // load source code strings into shaders
    if ( !loadShader ( vertexShader, "simplest.vsh" ) )
        exit ( 1 );

    if ( !loadShader ( fragmentShader, "simplest.fsh" ) )
        exit ( 1 );

                                        // create a program object and attach the
                                        // two compiled shaders
    program = glCreateProgramObjectARB ();

    glAttachObjectARB ( program, vertexShader   );
    glAttachObjectARB ( program, fragmentShader );

                                        // link the program object and print out the info log
    glLinkProgramARB ( program );

    if ( !checkOpenGLError() )          // check for OpenGL errors
        exit ( 1 );

    glGetObjectParameterivARB ( program, GL_OBJECT_LINK_STATUS_ARB, &linked );

    printInfoLog ( program );

    if ( !linked )
        return 0;

                                        // install program object as part of
                                        // current state
    glUseProgramObjectARB ( program );
    glutMainLoop          ();

    return 0;
}
