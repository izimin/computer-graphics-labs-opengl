//
// Simple rendering to texture example
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

#include	"libTexture.h"
#include	"Vector3D.h"
#include	"Vector2D.h"
#include	"PBuffer.h"

float		angle   = 0;
unsigned	mainTex = 0;
Vector3D	eye     ( 4.5, 0, 0 );			// camera position
PBuffer		pbuffer ( 256, 256, PBuffer :: modeTextureMipmap | PBuffer :: modeAlpha |
                      PBuffer :: modeDepth | PBuffer :: modeTexture2D, true );

Vector3D	vertexGrid  [30][30];
Vector2D	textureGrid [30][30];

void 	reshape ( int w, int h );
void	init    ();

void	initPBuffer ()
{
	static	bool	inited = false;

	if ( inited )
		return;

	inited = true;

	if ( !pbuffer.create () )
	{
		printf ( "create error\n" );
		
		pbuffer.printLastError ();
	}

	pbuffer.makeCurrent ();

	init ();

	reshape ( pbuffer.getWidth (), pbuffer.getHeight () );

	pbuffer.restoreCurrent ();
}

void init ()
{
	glClearColor ( 0.0, 0.0, 0.0, 1.0 );
	glEnable     ( GL_DEPTH_TEST );
	glEnable	 ( GL_TEXTURE_2D );
	glDepthFunc  ( GL_LEQUAL );

	glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
	glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}


void	renderToBuffer ()
{
	if ( pbuffer.isLost () )
		pbuffer.create ();

	if ( !pbuffer.makeCurrent () )
		printf ( "makeCurrent failed\n" );

	glClearColor ( 0, 0, 1, 1 );
	glClear      ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable       ( GL_TEXTURE_2D );
	glBindTexture  ( GL_TEXTURE_2D, mainTex );
glMatrixMode ( GL_TEXTURE );
glLoadIdentity ();
glMatrixMode ( GL_MODELVIEW );

	glPushMatrix   ();

	glRotatef      ( 820,  0, 1, 0 );
	glRotatef      ( 445,  0, 0, 1 );

	glutSolidTeapot ( 1.5 );

//	glTranslatef ( -4, 0, 0 );
//	glRotatef    ( angle, 0, 1, 0 );
//	glutSolidTorus ( 1, 3, 20, 20 );

	glPopMatrix ();

	pbuffer.restoreCurrent ();
}

void	distortVertex ( Vector3D& v, float time )
{
	float	sine   = 0.015 * sin ( 11*v.y + 16*v.z + time / 5 );
	float	cosine = 0.015 * cos ( 7*v.y - 14*v.z + time / 5 );

	v.y += sine;
	v.z += cosine;
}

void display ()
{
	initPBuffer ();

	renderToBuffer ();

	glClearColor ( 0.0, 0.0, 1.0, 1.0 );
	glClear      ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable     ( GL_TEXTURE_2D );

	if ( !pbuffer.bindToTexture () )
		pbuffer.printLastError ();

	glBindTexture ( GL_TEXTURE_2D, pbuffer.getTexture () );

	float	time = 0.08 * glutGet ( GLUT_ELAPSED_TIME );

	glBegin ( GL_TRIANGLES );

	for ( int i = 0; i < 29; i++ )
		for ( int j = 0; j < 29; j++ )
		{
			Vector3D	v00 ( vertexGrid  [i][j    ] );
			Vector3D	v01 ( vertexGrid  [i][j+1]   );
			Vector3D	v10 ( vertexGrid  [i+1][j]   );
			Vector3D	v11 ( vertexGrid  [i+1][j+1] );

			distortVertex ( v00, time );
			distortVertex ( v01, time );
			distortVertex ( v10, time );
			distortVertex ( v11, time );

			glTexCoord2fv ( textureGrid [i][j] );
			glVertex3fv   ( v00  );

			glTexCoord2fv ( textureGrid [i][j+1] );
			glVertex3fv   ( v01 );

			glTexCoord2fv ( textureGrid [i+1][j+1] );
			glVertex3fv   ( v11 );

			glTexCoord2fv ( textureGrid [i][j] );
			glVertex3fv   ( v00 );

			glTexCoord2fv ( textureGrid [i+1][j] );
			glVertex3fv   ( v10 );

			glTexCoord2fv ( textureGrid [i+1][j+1] );
			glVertex3fv   ( v11 );
		}

	glEnd ();

	if ( !pbuffer.unbindFromTexture () )
		pbuffer.printLastError ();

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

void animate ()
{
	angle = 0.08 * glutGet ( GLUT_ELAPSED_TIME );

	glutPostRedisplay ();
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
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_ALPHA );
	glutInitWindowSize  ( 500, 500 );


								// create window
	int	win = glutCreateWindow ( "OpenGL render to texture example 2 (distort)" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutKeyboardFunc ( key );
    glutIdleFunc     ( animate  );

	init ();

	printfInfo ();

	initExtensions ();

	initPBuffer ();

	mainTex = createTexture2D ( false, "../Textures/FieldStone.tga" );//block.bmp" );

	for ( int i = 0; i < 30; i++ )
		for ( int j = 0; j < 30; j++ )
		{
			vertexGrid  [i][j] = Vector3D ( 0, (i-15)*0.09, (j-15)*0.09 );
			textureGrid [i][j] = Vector2D ( (float) i / 29.0, (float) j / 29.0f );
		}

	glutMainLoop ();

	return 0;
}
