//
// Sample to check for GLSL program support in OpenGL card and driver
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


void init ()
{
    glClearColor ( 0.0, 0.0, 0.0, 1.0 );
    glEnable     ( GL_DEPTH_TEST );
}

void display ()
{
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glutSwapBuffers ();
}

void reshape ( int w, int h )
{
   glViewport     ( 0, 0, (GLsizei)w, (GLsizei)h );
   glMatrixMode   ( GL_PROJECTION );
   glLoadIdentity ();
   glMatrixMode   ( GL_MODELVIEW );
   glLoadIdentity ();
}

void key ( unsigned char key, int x, int y )
{
   if ( key == 27 || key == 'q' || key == 'Q' )      // quit requested
      exit ( 0 );
}

int main ( int argc, char * argv [] )
{
                                    // initialize glut
    glutInit            ( &argc, argv );
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize  ( 400, 400 );


                                // create window
    glutCreateWindow ( "OpenGL GLSL info" );

                                // register handlers
    glutDisplayFunc  ( display );
    glutReshapeFunc  ( reshape );
    glutKeyboardFunc ( key );

    init           ();
    initExtensions ();

    const char * vendor    = (const char *)glGetString ( GL_VENDOR     );
    const char * renderer  = (const char *)glGetString ( GL_RENDERER   );
    const char * version   = (const char *)glGetString ( GL_VERSION    );
    const char * extension = (const char *)glGetString ( GL_EXTENSIONS );

    printf ( "Vendor:   %s\nRenderer: %s\nVersion:  %s\n", vendor, renderer, version );

    if ( !isExtensionSupported ( "GL_ARB_shading_language_100" ) )
    {
        printf ( "GL_ARB_shading_language_100 extension NOT supported.\n" );

        return 1;
    }

    printf ( "GL_ARB_shading_language_100 extension is supported !\n" );

#ifndef	MACOSX
    const char * slVer = (const char *) glGetString ( GL_SHADING_LANGUAGE_VERSION_ARB );

    if ( glGetError() != GL_NO_ERROR )
        printf ( "Shading language supported: 1.051\n" );
    else
        printf ( "Shading language supported: %s\n", slVer );
#endif

    GLint maxVertexAttribs;
    GLint maxVertexTextureUnits;
    GLint maxFragmentTextureUnits;
    GLint maxCombinedTextureUnits;
    GLint maxVertexUniformComponents;
    GLint maxVaryingFloats;
    GLint maxFragmentUniformComponents;
    GLint maxTextureCoords;

    glGetIntegerv ( GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB,    &maxVertexUniformComponents );
    glGetIntegerv ( GL_MAX_VERTEX_ATTRIBS_ARB,               &maxVertexAttribs );
    glGetIntegerv ( GL_MAX_TEXTURE_IMAGE_UNITS_ARB,          &maxFragmentTextureUnits );
    glGetIntegerv ( GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB,   &maxVertexTextureUnits );
    glGetIntegerv ( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB, &maxCombinedTextureUnits );
    glGetIntegerv ( GL_MAX_VARYING_FLOATS_ARB,               &maxVaryingFloats );
    glGetIntegerv ( GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB,  &maxFragmentUniformComponents );
    glGetIntegerv ( GL_MAX_TEXTURE_COORDS_ARB,               &maxTextureCoords            );


    printf ( "\nGLSL program limits:\n"                                               );
    printf ( "\tmax vertex attributes           : %d\n", maxVertexAttribs             );
    printf ( "\tmax vertex texture units        : %d\n", maxVertexTextureUnits        );
    printf ( "\tmax fragment texture units      : %d\n", maxFragmentTextureUnits      );
    printf ( "\tmax combined texture units      : %d\n", maxCombinedTextureUnits      );
    printf ( "\tmax vertex uniform components   : %d\n", maxVertexUniformComponents   );
    printf ( "\tmax varying floats              : %d\n", maxVaryingFloats             );
    printf ( "\tmax fragment uniform components : %d\n", maxFragmentUniformComponents );
    printf ( "\tmax texture coords              : %d\n", maxTextureCoords             );

    return 0;
}
