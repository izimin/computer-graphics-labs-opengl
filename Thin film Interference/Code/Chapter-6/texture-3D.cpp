//
// Sample to show creating and using 3D textures in OpenGL
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#include	"libExt.h"
#include	"libTexture.h"
#include	"Vector3D.h"
#include	"Vector2D.h"
#include	"Torus.h"

#ifdef	MACOSX
	#include	<GLUT/glut.h>
	
	#define	GL_TEXTURE_3D_EXT	GL_TEXTURE_3D
	#define	glTexImage3DEXT		glTexImage3D
#else
	#include	<glut.h>
#endif

#include	<stdio.h>
#include	<stdlib.h>

Vector3D	eye   ( 2, 2, 2 );			// camera position
unsigned	tex3D;
float	 	angle  = 0;
float	 	angle2 = 0;
Torus		torus ( 1, 3, 30, 30 );

unsigned	createSimpleTexture ( int size )
{
	printf ( "Creating 3D texture.\n" );

	GLuint   	texture;
	byte      * data    = new byte [3*size*size*size];
	int			offs    = 0;
	float		step    = 1.0f / (float) size;

	for ( int i = 0; i < size; i++ )
		for ( int j = 0; j < size; j++ )
			for ( int k = 0; k < size; k++ )
			{
				data [offs++] = (byte)(255*step*i);
				data [offs++] = (byte)(255*step*j);
				data [offs++] = (byte)(255*step*k);
			}

	glGenTextures ( 1, &texture);
	glBindTexture ( GL_TEXTURE_3D_EXT, texture );

	glTexImage3DEXT ( GL_TEXTURE_3D_EXT, 0, GL_RGB, size, size, size,
					  0, GL_RGB, GL_UNSIGNED_BYTE, data );

	glTexParameteri ( GL_TEXTURE_3D_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_3D_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_S,     GL_REPEAT );
	glTexParameteri ( GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_T,     GL_REPEAT );
	glTexParameteri ( GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_R,     GL_REPEAT );

	delete data;

	printf ( "Done.\n" );

	return texture;
}

void init ()
{
	glClearColor ( 0, 0, 0, 1 );
	glEnable     ( GL_DEPTH_TEST );

	glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
	glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void display ()
{
	int	i;

	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable      ( GL_TEXTURE_3D_EXT );
	glBindTexture ( GL_TEXTURE_3D_EXT, tex3D );
	glMatrixMode  ( GL_MODELVIEW );
	glPushMatrix  ();
	glTranslatef  ( -4, -4, -4 );
	glRotatef     ( angle,  1, 1, 0 );
	glRotatef     ( angle2, 0, 1, 1 );

	torus.draw ();

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
   gluLookAt      ( eye.x, eye.y, eye.z,	// eye
					0, 0, 0,				// center
					0.0, 0.0, 1.0 );		// up
}


void key ( unsigned char key, int x, int y )
{
	if ( key == 27 || key == 'q' || key == 'Q' )		// quit requested
		exit ( 0 );

	if ( key == 'p' || key == 'P' )						// saving screenshots
		saveScreenShot ( "texture-3D.tga" );
}

void	animate ()
{
	angle  = 0.04f * glutGet ( GLUT_ELAPSED_TIME );
	angle2 = 0.02f * glutGet ( GLUT_ELAPSED_TIME );

	glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize  ( 500, 500 );


								// create window
	glutCreateWindow ( "OpenGL EXT_texture3D example " );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutKeyboardFunc ( key );
	glutIdleFunc     ( animate );

#ifndef	MACOSX
	if ( !isExtensionSupported ( "GL_EXT_texture3D" ) )
	{
		printf ( "EXT_texture3D NOT supported.\n" );

		return 1;
	}
#endif

	initExtensions ();
	init           ();

	tex3D  = createSimpleTexture ( 32 );

	glutMainLoop ();

	return 0;
}
