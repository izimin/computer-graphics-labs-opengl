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

unsigned texture1, texture2;
float	 angle  = 0;
float	 angle2 = 0;

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
    glEnable           ( GL_TEXTURE_GEN_S );
    glEnable           ( GL_TEXTURE_GEN_T );
    glTexGeni          ( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
    glTexGeni          ( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
	glBindTexture      ( GL_TEXTURE_2D, texture2 );
	glTexEnvi          ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );


	glPushMatrix    ();
	glTranslatef    ( 2, 2, 2 );
	glRotatef       ( angle,  1, 1, 0 );
	glRotatef       ( angle2, 0, 1, 1 );
	glutSolidTeapot ( 1 );
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
   gluLookAt      ( 0.0, 0.0, 0.0,		// eye
					5.0, 5.0, 5.0,		// center
					0.0, 1.0, 0.0 );
}

void key ( unsigned char key, int x, int y )
{
    if ( key == 27 || key == 'q' || key == 'Q' )		// quit requested
        exit ( 0 );

	if ( key == 'p' || key == 'P' )					// saving screenshots
		saveScreenShot ( "multitex3.tga" );
}

void	animate ()
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
	glutInitWindowSize  ( 400, 400 );


								// create window
	int	win = glutCreateWindow ( "OpenGL Multitexture example 3" );

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

	initExtensions ();

	texture1 = createTexture2D ( true, "../Textures/block.bmp" );
	texture2 = createTexture2D ( true, "../Textures/stars.bmp" );

	init ();

	glutMainLoop ();

	return 0;
}
