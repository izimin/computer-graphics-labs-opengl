//
// Sample to OpenGL cubic maps
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

float	 angle    = 0;
float	 angle2   = 0;
int		 reflMode = GL_REFLECTION_MAP_ARB;
int		 object   = 0;
unsigned texture;

void init ()
{
	glClearColor ( 0, 0, 0, 1 );
	glEnable     ( GL_DEPTH_TEST );

	glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
	glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void display ()
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glTexGeni ( GL_S, GL_TEXTURE_GEN_MODE, reflMode );
    glTexGeni ( GL_T, GL_TEXTURE_GEN_MODE, reflMode );
	glTexGeni ( GL_R, GL_TEXTURE_GEN_MODE, reflMode );

	glEnable  ( GL_TEXTURE_CUBE_MAP_ARB );

	glBindTexture ( GL_TEXTURE_CUBE_MAP_ARB, texture );

	glTexParameteri ( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri ( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP );

    glEnable  ( GL_TEXTURE_GEN_S );
    glEnable  ( GL_TEXTURE_GEN_T );
    glEnable  ( GL_TEXTURE_GEN_R );

	glPushMatrix    ();
	glTranslatef    ( 2, 2, 2 );
	glRotatef       ( angle,  1, 1, 0 );
	glRotatef       ( angle2, 0, 1, 1 );

	if ( object == 0 )
		glutSolidTeapot ( 1 );
	else
	if ( object == 1 )
		glutSolidSphere ( 1, 30, 30 );
	else
	if ( object == 2 )
		glutSolidCube ( 1 );
	else
	if ( object == 3 )
		glutSolidCone ( 0.7, 1.2, 30, 30 );
	else
	if ( object == 4 )
		glutSolidTorus ( 0.5, 1, 40, 40 );
	else
	if ( object == 5 )
		glutSolidDodecahedron ();
	else
	if ( object == 6 )
		glutSolidOctahedron ();
	else
	if ( object == 7 )
		glutSolidTetrahedron ();
	else
	if ( object == 8 )
		glutSolidIcosahedron ();

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

	if ( key == 'n' || key == 'N' )
		reflMode = GL_NORMAL_MAP_ARB;
	else
	if ( key == 'r' || key == 'R' )
		reflMode = GL_REFLECTION_MAP_ARB;
	else
	if ( key == '0' )
		object = 0;
	else
	if ( key == '1' )
		object = 1;
	else
	if ( key == '2' )
		object = 2;
	else
	if ( key == '3' )
		object = 3;
	else
	if ( key == '4' )
		object = 4;
	else
	if ( key == '5' )
		object = 5;
	else
	if ( key == '6' )
		object = 6;
	else
	if ( key == '7' )
		object = 7;
	else
	if ( key == '8' )
		object = 8;
	else
	if ( key == '+' )
	{
		if ( ++object > 8 )
			object = 0;
	}
	else
	if ( key == '-' )
	{
		if ( --object < 0 )
			object = 8;
	}
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
	glutInitWindowSize  ( 500, 500 );


								// create window
	int	win = glutCreateWindow ( "OpenGL cubemap example" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutKeyboardFunc ( key );
	glutIdleFunc     ( animate );

	if ( !isExtensionSupported ( "GL_ARB_texture_cube_map" ) )
	{
		printf ( "ARB_texture_cube_map NOT supported.\n" );

		return 1;
	}

	init           ();
	initExtensions ();

	const char * faces [6] =
	{
		"../Textures/Cubemaps/cm_left.tga",
		"../Textures/Cubemaps/cm_right.tga",
		"../Textures/Cubemaps/cm_top.tga",
		"../Textures/Cubemaps/cm_bottom.tga",
		"../Textures/Cubemaps/cm_back.tga",
		"../Textures/Cubemaps/cm_front.tga",
	};

	texture = createCubeMap ( true, faces );

	glutMainLoop ();

	return 0;
}
