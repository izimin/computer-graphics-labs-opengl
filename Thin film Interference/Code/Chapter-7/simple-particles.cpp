//
// Sample to illustrate particle systems via ARB_point_parameters & ARB_point_sprite
// extensions.
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
#include	"Vector2D.h"
#include	"Emitter.h"
#include	"Flame.h"

Vector3D	eye   ( 3, 3, 3 );			// camera position

Flame	flame   ( 3000, 50, Vector3D ( -1, 0, 0 ), 0.15, 0.2, 1 );
Emitter	emitter ( 1000, 30, Vector3D (  1, 0, 0 ), Vector4D ( 1, 0, 0, 1 ), 4, 6 );

/////////////////////////////////////////////////////////////////////////////////

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

	flame.  render ();
	emitter.render ();

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
}

void	animate ()
{
	flame.  update ();
	emitter.update ();

	glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize  ( 500, 500 );


								// create window
	glutCreateWindow ( "OpenGL particle systems" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutKeyboardFunc ( key     );
	glutIdleFunc     ( animate );

	init           ();
	initExtensions ();

	if ( !isExtensionSupported ( "GL_ARB_point_parameters" ) )
	{
		printf ( "ARB_point_parameters NOT supported.\n" );

		return 1;
	}

	if ( !isExtensionSupported ( "GL_ARB_point_sprite" ) )
	{
		printf ( "GL_ARB_point_sprite NOT supported" );

		return 2;
	}

	unsigned	texture1 = createTexture2D ( true,  "../Textures/particle.bmp" );
	unsigned	texture2 = createTexture2D ( true,  "../Textures/particle.bmp" );

	flame.setTexture   ( texture1 );
	emitter.setTexture ( texture2 );

	glutMainLoop ();

	return 0;
}
