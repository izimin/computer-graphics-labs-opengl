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
Vector3D	eye   ( 8, 0, 0 );			// camera position
PBuffer		pbuffer ( 128, 128, PBuffer :: modeTextureMipmap | PBuffer :: modeAlpha |
                      PBuffer :: modeDepth | PBuffer :: modeTexture2D, true );

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

void    drawBox ( GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat z1, GLfloat z2, unsigned tex )
{
	glBindTexture ( GL_TEXTURE_2D, tex );

    glBegin ( GL_POLYGON );         // front face
          glNormal3f   ( 0.0, 0.0, 1.0 );

		  glTexCoord2f ( 0, 0 );
          glVertex3f   ( x1, y1, z2 );

		  glTexCoord2f ( 1, 0 );
          glVertex3f   ( x2, y1, z2 );

		  glTexCoord2f ( 1, 1 );
          glVertex3f   ( x2, y2, z2 );

		  glTexCoord2f ( 0, 1 );
          glVertex3f   ( x1, y2, z2 );
        glEnd ();

        glBegin ( GL_POLYGON );         // back face
          glNormal3f ( 0.0, 0.0, -1.0 );

		  glTexCoord2f ( 1, 0 );
          glVertex3f   ( x2, y1, z1 );

		  glTexCoord2f ( 0, 0 );
          glVertex3f   ( x1, y1, z1 );

		  glTexCoord2f ( 0, 1 );
          glVertex3f   ( x1, y2, z1 );

		  glTexCoord2f ( 1, 1 );
          glVertex3f   ( x2, y2, z1 );
        glEnd ();

        glBegin ( GL_POLYGON );         // left face
          glNormal3f ( -1.0, 0.0, 0.0 );

		  glTexCoord2f ( 0, 0 );
          glVertex3f   ( x1, y1, z1 );

		  glTexCoord2f ( 0, 1 );
          glVertex3f   ( x1, y1, z2 );

		  glTexCoord2f ( 1, 1 );
          glVertex3f   ( x1, y2, z2 );

		  glTexCoord2f ( 1, 0 );
          glVertex3f   ( x1, y2, z1 );
        glEnd ();

        glBegin ( GL_POLYGON );         // right face
          glNormal3f ( 1.0, 0.0, 0.0 );

		  glTexCoord2f ( 0, 1 );
          glVertex3f   ( x2, y1, z2 );

		  glTexCoord2f ( 0, 0 );
          glVertex3f   ( x2, y1, z1 );

		  glTexCoord2f ( 1, 0 );
          glVertex3f   ( x2, y2, z1 );

		  glTexCoord2f ( 1, 1 );
          glVertex3f   ( x2, y2, z2 );
        glEnd ();

        glBegin ( GL_POLYGON );         // top face
          glNormal3f ( 0.0, 1.0, 0.0 );

		  glTexCoord2f ( 0, 1 );
          glVertex3f   ( x1, y2, z2 );

		  glTexCoord2f ( 1, 1 );
          glVertex3f   ( x2, y2, z2 );

		  glTexCoord2f ( 1, 0 );
          glVertex3f   ( x2, y2, z1 );

		  glTexCoord2f ( 0, 0 );
          glVertex3f   ( x1, y2, z1 );
        glEnd ();

        glBegin ( GL_POLYGON );         // bottom face
          glNormal3f ( 0.0, -1.0, 0.0 );

		  glTexCoord2f ( 1, 1 );
          glVertex3f   ( x2, y1, z2 );

		  glTexCoord2f ( 1, 0 );
          glVertex3f   ( x1, y1, z2 );

		  glTexCoord2f ( 0, 0 );
          glVertex3f   ( x1, y1, z1 );

		  glTexCoord2f ( 1, 0 );
          glVertex3f   ( x2, y1, z1 );
        glEnd ();
}

void	renderToBuffer ()
{
	if ( pbuffer.isLost () )
		pbuffer.create ();

	if ( !pbuffer.makeCurrent () )
	{
		printf ( "makeCurrent failed\n" );
		
		pbuffer.printLastError ();
	}

	glClearColor ( 0, 0.5, 0, 1 );
	glClear      ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glBindTexture  ( GL_TEXTURE_2D, mainTex );
	glPushMatrix   ();
	glRotatef      ( angle,   0, 1, 0 );
	glRotatef      ( angle/2, 0, 0, 1 );

//	glutSolidTeapot ( 2 );
	glutSolidTorus ( 1, 3, 20, 20 );

	glPopMatrix ();

	pbuffer.restoreCurrent ();
}

void display ()
{
//	initPBuffer ();

	renderToBuffer ();

	glClearColor ( 0.0, 0.0, 1.0, 1.0 );
	glClear      ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable     ( GL_TEXTURE_2D );

	if ( !pbuffer.bindToTexture () )
		pbuffer.printLastError ();

	glPushMatrix   ();
	glRotatef      ( angle,   1, 0, 0 );
	glRotatef      ( angle/2, 0, 1, 0 );

	drawBox ( -1.5, 1.5, -1.5, 1.5, -1.5, 1.5, pbuffer.getTexture () );

	glPopMatrix     ();

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
	glutCreateWindow ( "OpenGL render to texture example" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutKeyboardFunc ( key );
    glutIdleFunc     ( animate  );

	init ();

	printfInfo ();

	initExtensions ();

	mainTex = createTexture2D ( true, "../Textures/block.bmp" );

	initPBuffer ();

	glutMainLoop ();

	return 0;
}
