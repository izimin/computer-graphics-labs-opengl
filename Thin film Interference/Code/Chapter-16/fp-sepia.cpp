//
// Sample to image postprocessing via p-buffer and fragment programs
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
#include	"TypeDefs.h"
#include	"Vector3D.h"
#include	"Vector2D.h"
#include	"boxes.h"
#include	"PBuffer.h"
#include	"VertexProgram.h"
#include	"FragmentProgram.h"

Vector3D	eye   ( -0.5, -0.5, 1.5 );	// camera position
unsigned	decalMap;					// decal (diffuse) texture
unsigned	stoneMap;
unsigned	teapotMap;
float		angle     = 0;
float		rot       = 0;
bool		useFilter = true;

VertexProgram	vertexProgram;
FragmentProgram	fragmentProgram;

PBuffer		pbuffer ( 512, 512, PBuffer :: modeTextureMipmap | PBuffer :: modeAlpha |
                      PBuffer :: modeDepth | PBuffer :: modeTexture2D, true );

void	renderToBuffer ();

void init ()
{
	glClearColor ( 0.0, 0.0, 0.0, 1.0 );
	glEnable     ( GL_DEPTH_TEST );
	glEnable     ( GL_TEXTURE_2D );
	glDepthFunc  ( GL_LEQUAL     );

	glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
	glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void displayBoxes ()
{
	glMatrixMode ( GL_MODELVIEW );
	glPushMatrix ();

    glRotatef    ( rot, 0, 0, 1 );

	drawBox  ( Vector3D ( -5, -5, 0 ), Vector3D ( 10, 10, 3 ), stoneMap, false );
	drawBox  ( Vector3D ( 3, 2, 0.5 ), Vector3D ( 1,  2,  2 ), decalMap );

	glBindTexture   ( GL_TEXTURE_2D, teapotMap );
	glTranslatef    ( 0.2, 1, 1.5 );
	glRotatef       ( angle * 45.3, 1, 0, 0 );
	glRotatef       ( angle * 57.2, 0, 1, 0 );
	glutSolidTeapot ( 0.3 );

	glPopMatrix     ();
}

void	startOrtho ()
{
	glMatrixMode   ( GL_PROJECTION );					// select the projection matrix
	glPushMatrix   ();									// store the projection matrix
	glLoadIdentity ();									// reset the projection matrix
														// set up an ortho screen
	glOrtho        ( 0, 512, 0, 512, -1, 1 );
	glMatrixMode   ( GL_MODELVIEW );					// select the modelview matrix
	glPushMatrix   ();									// store the modelview matrix
	glLoadIdentity ();									// reset the modelview matrix
}

void	endOrtho ()
{
	glMatrixMode ( GL_PROJECTION );						// select the projection matrix
	glPopMatrix  ();									// restore the old projection matrix
	glMatrixMode ( GL_MODELVIEW );						// select the modelview matrix
	glPopMatrix  ();									// restore the old projection matrix
}

void display ()
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	renderToBuffer ();

	if ( !pbuffer.bindToTexture () )
		pbuffer.printLastError ();

	startOrtho ();

	glBindTexture ( GL_TEXTURE_2D, pbuffer.getTexture () );

	if ( useFilter )
	{
		vertexProgram.enable   ();
		vertexProgram.bind     ();
		fragmentProgram.enable ();
		fragmentProgram.bind   ();
	}

	glBegin ( GL_QUADS );

		glTexCoord2f ( 0, 0 );
		glVertex2f   ( 0, 0 );

		glTexCoord2f ( 1,   0 );
		glVertex2f   ( 511, 0 );

		glTexCoord2f ( 1, 1 );
		glVertex2f   ( 511, 511 );

		glTexCoord2f ( 0, 1 );
		glVertex2f   ( 0, 511 );
	glEnd   ();

	if ( useFilter )
	{
		vertexProgram.disable   ();
		fragmentProgram.disable ();
	}

	endOrtho ();

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
					3, 3, 1,				// center
					0, 0, 1 );				// up
}

void key ( unsigned char key, int x, int y )
{
	if ( key == 27 || key == 'q' || key == 'Q' )		// quit requested
    	exit ( 0 );

    if ( key == 'f' || key == 'F' )
    	useFilter = !useFilter;
}

void	specialKey ( int key, int x, int y )
{
    if ( key == GLUT_KEY_RIGHT )
    	rot += 5;
    else
    if ( key == GLUT_KEY_LEFT )
    	rot -= 5;

    glutPostRedisplay ();
}

void	animate ()
{
	angle  = 0.004f * glutGet ( GLUT_ELAPSED_TIME );

	glutPostRedisplay ();
}

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

void	renderToBuffer ()
{
	if ( pbuffer.isLost () )
		pbuffer.create ();

	if ( !pbuffer.makeCurrent () )
		printf ( "makeCurrent failed\n" );

	glClearColor ( 0, 0, 1, 1 );
	glClear      ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	reshape ( pbuffer.getWidth (), pbuffer.getHeight () );

	displayBoxes ();

	pbuffer.restoreCurrent ();
}

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize  ( 512, 512 );


								// create window
	glutCreateWindow ( "OpenGL image postprocessing - sepia" );

								// register handlers
	glutDisplayFunc  ( display    );
	glutReshapeFunc  ( reshape    );
	glutKeyboardFunc ( key        );
	glutSpecialFunc  ( specialKey );
	glutIdleFunc     ( animate    );

	init ();
	initExtensions ();

	assertExtensionsSupported ( "GL_ARB_fragment_program GL_ARB_vertex_program" );

	if ( !pbuffer.checkExtensions () )
	{
		printf ( "Pbuffer extensions not found\n" );

		return 1;
	}

	if ( !vertexProgram.load ( "sepia.vp" ) )
	{
		printf ( "Error loading vertex program:\n %s\n", vertexProgram.getErrorString ().c_str () );

		return 1;
	}

	if ( !fragmentProgram.load ( "sepia.fp" ) )
	{
		printf ( "Error loading fragment program:\n %s\n", fragmentProgram.getErrorString ().c_str () );

		return 2;
	}

	decalMap  = createTexture2D ( true, "../Textures/oak.bmp" );
	stoneMap  = createTexture2D ( true, "../Textures/block.bmp" );
	teapotMap = createTexture2D ( true, "../Textures/Oxidated.jpg" );

	printf ( "Press F key to turn filtering on/off\n" );

	initPBuffer  ();
	glutMainLoop ();

	return 0;
}
