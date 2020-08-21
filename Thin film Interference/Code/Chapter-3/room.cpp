//
// Sample to show anisotropic texture filtering in OpenGL
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
#include	<list>

using namespace std;

#include	"libTexture.h"
#include	"TypeDefs.h"
#include	"Vector3D.h"
#include	"Vector2D.h"
#include	"boxes.h"

#define	CAPTION	"OpenGL anisotropic filtering example"

Vector3D	eye   ( 0, 0, -2 );			// camera position
Vector3D	viewDir ( 1, 0, 0 );		// viewer direction

unsigned	stoneMap;
int			mouseOldX = 0;
int			mouseOldY = 0;
Vector3D	rot ( 0, 0, 0 );
float		step = 0.003;				// movement rate per second
bool		mousePressed = false;
float		roomSize     = 50;			// room half size
GLint 		maxAniso     = 1;
int			curAniso     = 1;

void	setupCamera ();

void	setAnisotropy ( int aniso )
{
	char	str [256];
	
	if ( aniso < 1 || aniso > maxAniso )
		return;
		
	curAniso = aniso;
	
	glBindTexture   ( GL_TEXTURE_2D, stoneMap );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, curAniso );
    	
    sprintf            ( str, "%s - anisotropy = %2d", CAPTION, curAniso );
   	glutSetWindowTitle ( str );
   }
   
void init ()
{
	glClearColor ( 0.0, 0.0, 0.0, 1.0 );
	glEnable     ( GL_DEPTH_TEST );
	glEnable     ( GL_TEXTURE_2D );
	glDepthFunc  ( GL_LEQUAL     );

	glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
	glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void display ()
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

										// draw main hall
	drawBox  ( Vector3D ( -roomSize, -roomSize, -3 ), Vector3D ( 2*roomSize, 2*roomSize, 7 ), stoneMap, false );

	glutSwapBuffers ();
}

void reshape ( int w, int h )
{
    glViewport     ( 0, 0, (GLsizei)w, (GLsizei)h );
    glMatrixMode   ( GL_PROJECTION );
    glLoadIdentity ();
    gluPerspective ( 60.0, (GLfloat)w/(GLfloat)h, 0.1, 1000 );
	setupCamera    ();
}

void	setupCamera ()
{
	Vector3D	to = eye + viewDir;

  	glMatrixMode   ( GL_MODELVIEW );
   	glLoadIdentity ();
   	gluLookAt      ( eye.x, eye.y, eye.z,	// eye
					 to.x,  to.y,  to.z,	// center
					 0.0, 0.0, 1.0 );		// up

}

void motion ( int x, int y )
{
	static	bool	mouseInited = false;

	if ( !mouseInited )
	{
		mouseInited = true;
		mouseOldX   = x;
		mouseOldY   = y;

		return;
	}

	float	scale = -M_PI / 100;

	viewDir.x = cos ( scale * x );
	viewDir.y = sin ( scale * x );
	viewDir.z = 0;

	setupCamera ();

	glutPostRedisplay ();
}

void mouse ( int button, int state, int x, int y )
{
	if ( button == GLUT_LEFT_BUTTON )
		mousePressed = (state == GLUT_DOWN);

	motion ( x, y );
}

void key ( unsigned char key, int x, int y )
{
	char	str [256];
	
	if ( key == 27 || key == 'q' || key == 'Q' )		// quit requested
    	exit ( 0 );
    	
    if ( key == '+' )
    	setAnisotropy ( curAniso + 1 );
    else
    if ( key == '-' )
    	setAnisotropy ( curAniso - 1 );
}

void	animate ()
{
	static	bool	timeInited = false;
	static	int		lastTime;

	if ( !timeInited )
	{
		lastTime    = glutGet ( GLUT_ELAPSED_TIME );
		timeInited = true;
	}

	if ( mousePressed )
	{
		Vector3D	delta = viewDir * (step * (glutGet ( GLUT_ELAPSED_TIME ) - lastTime)*0.001f);
		Vector3D	e     = eye + delta;

		if ( e.x < roomSize && e.x > -roomSize )
			eye.x = e.x;

		if ( e.y < roomSize && e.y > -roomSize )
			eye.y = e.y;

		setupCamera ();
	}

	glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize  ( 500, 500 );


								// create window
	glutCreateWindow ( CAPTION );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutKeyboardFunc ( key     );
	glutMouseFunc    ( mouse   );
	glutMotionFunc   ( motion  );
	glutPassiveMotionFunc   ( motion  );
	glutIdleFunc     ( animate );

	init           ();
	initExtensions ();
	printfInfo     ();

	if ( !isExtensionSupported ( "GL_EXT_texture_filter_anisotropic" ) )
	{
		printf ( "EXT_texture_filter_anisotropic NOT supported.\n" );

		return 1;
	}
	
	stoneMap = createTexture2D ( true, "../Textures/block.bmp" );

	glGetIntegerv (GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso );
	
	printf ( "Max supported anisotry %d\n", maxAniso );

	setAnisotropy ( 1 );
	
 	glutMainLoop ();

	return 0;
}
