//
// Sample to to show Register Combiners support in OpenGL card and driver
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#include	"libExt.h"



#ifdef	MACOSX
	#include	<GLUT/glut.h>
#else
	#include	<glut.h>
#endif

#include	<stdio.h>

#include	<stdlib.h>



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
  if ( key == 27 || key == 'q' || key == 'Q' )		// quit requested
    exit ( 0 );
}

int main ( int argc, char * argv [] )
{
							// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );   
	glutInitWindowSize  ( 400, 400 );


							// create window
	glutCreateWindow ( "OpenGL example 1" );

							// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
    glutKeyboardFunc ( key );

	init ();

	const char * vendor    = (const char *)glGetString ( GL_VENDOR     );
	const char * renderer  = (const char *)glGetString ( GL_RENDERER   );
	const char * version   = (const char *)glGetString ( GL_VERSION    );
	const char * extension = (const char *)glGetString ( GL_EXTENSIONS );

	printf ( "Vendor:   %s\nRenderer: %s\nVersion:  %s\n", vendor, renderer, version );

	if ( !isExtensionSupported ( "GL_ARB_multitexture" ) )
	{
		printf ( "ARB_multitexture NOT supported.\n" );

		return 1;
	}

	if ( !isExtensionSupported ( "GL_NV_register_combiners" ) )
	{
		printf ( "NV_register_combiners NOT supported" );

		return 2;
	}

	GLint	maxTextureUnits;
	GLint	maxCombiners;

	glGetIntegerv ( GL_MAX_TEXTURE_UNITS_ARB,    &maxTextureUnits );
	glGetIntegerv ( GL_MAX_GENERAL_COMBINERS_NV, &maxCombiners    );

	printf ( "ARB_multitexture supported.\nMax texture units %d.\n", maxTextureUnits );
	printf ( "NV_register_combiners supported.\nMax general combiners %d.\n", maxCombiners );

	return 0;
}
