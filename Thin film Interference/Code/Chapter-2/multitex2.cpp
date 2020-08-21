//
// Sample to show multitexturing in OpenGL.
// In this example we show product of two textures - texture1 and texture2
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#include	"libExt.h"
#include	"libTexture.h"
#include	"Vector3D.h"
#include	"Vector2D.h"

#ifdef	MACOSX
	#include	<GLUT/glut.h>
#else
	#include	<glut.h>
#endif

#include	<stdio.h>
#include	<stdlib.h>

GLuint texture1, texture2;

void init ()
{
	glClearColor ( 0.0, 0.0, 0.0, 1.0 );
	glEnable     ( GL_DEPTH_TEST );
}

void display ()
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glActiveTextureARB ( GL_TEXTURE0_ARB );
	glEnable           ( GL_TEXTURE_2D );
	glBindTexture      ( GL_TEXTURE_2D, texture1 );
	glTexEnvi          ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	glActiveTextureARB ( GL_TEXTURE1_ARB );
	glEnable           ( GL_TEXTURE_2D );
	glBindTexture      ( GL_TEXTURE_2D, texture2 );
	glTexEnvi          ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glMatrixMode       ( GL_TEXTURE );
	glScalef           ( 3, 3, 3 );

	glBegin ( GL_QUADS );

		glMultiTexCoord2f ( GL_TEXTURE0_ARB, 0, 0 );
		glMultiTexCoord2f ( GL_TEXTURE1_ARB, 0, 0 );
		glVertex3f        ( -1, -1, 0 );

		glMultiTexCoord2f ( GL_TEXTURE0_ARB, 1, 0 );
		glMultiTexCoord2f ( GL_TEXTURE1_ARB, 1, 0 );
		glVertex3f        ( 1, -1, 0 );

		glMultiTexCoord2f ( GL_TEXTURE0_ARB, 1, 1 );
		glMultiTexCoord2f ( GL_TEXTURE1_ARB, 1, 1 );
		glVertex3f        ( 1, 1, 0 );

		glMultiTexCoord2f ( GL_TEXTURE0_ARB, 0, 1 );
		glMultiTexCoord2f ( GL_TEXTURE1_ARB, 0, 1 );
		glVertex3f        ( -1, 1, 0 );

	glEnd   ();

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
	glutCreateWindow ( "OpenGL Multitexture example 2" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
    glutKeyboardFunc ( key );

	initExtensions ();

	if ( !isExtensionSupported ( "GL_ARB_multitexture" ) )
	{
		printf ( "ARB_multitexture NOT supported.\n" );

		return 1;
	}

	texture1 = createTexture2D ( true, "../Textures/block.bmp" );
	texture2 = createTexture2D ( true, "../Textures/oak.bmp" );

	init ();

	glutMainLoop ();

	return 0;
}
