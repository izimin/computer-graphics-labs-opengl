//
// Simple demo for GL_texture_env_combine
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
#else
	#include	<glut.h>
#endif

#include	<stdio.h>
#include	<stdlib.h>


float	 	angle  = 0;
float	 	angle2 = 0;
unsigned 	mainTex;				// main texture
unsigned 	envMap;					// env. map
Torus		torus ( 1, 3, 30, 30 );

void init ()
{
	glClearColor ( 0, 0, 0, 1 );
	glEnable     ( GL_DEPTH_TEST );
}

void display ()
{
	glClear         ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glPushMatrix    ();
	glTranslatef    ( 7, 7, 7 );
	glRotatef       ( angle,  1, 1, 0 );
	glRotatef       ( angle2, 0, 1, 1 );

											// main texture
	glActiveTextureARB ( GL_TEXTURE0_ARB );
	glEnable           ( GL_TEXTURE_2D );
	glBindTexture      ( GL_TEXTURE_2D, mainTex );
	glTexEnvi          ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

											// env. map
	glActiveTextureARB ( GL_TEXTURE1_ARB );
	glEnable           ( GL_TEXTURE_2D );
    glEnable           ( GL_TEXTURE_GEN_S );
    glEnable           ( GL_TEXTURE_GEN_T );
    glTexGeni          ( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
    glTexGeni          ( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
	glBindTexture      ( GL_TEXTURE_2D, envMap );

											// setup combine mode
	glTexEnvi          ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE_ARB );
	glTexEnvi          ( GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,   GL_INTERPOLATE_ARB );
	glTexEnvi          ( GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE );

											// setup Arg0
	glTexEnvi ( GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,    GL_PREVIOUS_ARB );
    glTexEnvi ( GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,   GL_SRC_COLOR    );
	glTexEnvi ( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB,  GL_PREVIOUS_ARB );
    glTexEnvi ( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA    );

                                          // now setup Arg1
    glTexEnvi ( GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,    GL_TEXTURE   );
    glTexEnvi ( GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,   GL_SRC_COLOR );
    glTexEnvi ( GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB,  GL_TEXTURE   );
    glTexEnvi ( GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA );

                                          // now setup Arg2 (weight factor)
    glTexEnvi ( GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB,    GL_PREVIOUS_ARB );
    glTexEnvi ( GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB,   GL_SRC_ALPHA    );
    glTexEnvi ( GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_ARB,  GL_PREVIOUS_ARB );
    glTexEnvi ( GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_ARB, GL_SRC_ALPHA    );

	torus.draw ();

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
	glutCreateWindow ( "OpenGL texure_env_combine example 1" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
    glutKeyboardFunc ( key     );
	glutIdleFunc     ( animate );

	init ();

	if ( !isExtensionSupported ( "GL_ARB_multitexture" ) )
	{
		printf ( "ARB_multitexture NOT supported.\n" );

		return 1;
	}

	if ( !isExtensionSupported ( "GL_ARB_texture_env_combine" ) )
	{
		printf ( "ARB_texture_env_combine NOT supported.\n" );

		return 1;
	}

	initExtensions ();

	mainTex = createTexture2D ( true, "../Textures/block-with-alpha.tga" );
	envMap  = createTexture2D ( true, "../Textures/stars.bmp" );

	glutMainLoop ();

	return 0;
}
