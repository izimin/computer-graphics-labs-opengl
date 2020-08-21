//
// Sample to to per-pixel true reflective bumpmapping in OpenGL
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
#include	"Torus.h"

Vector3D	eye   ( 8, 0, 0 );			// camera position
unsigned	envMap;						// environment cubemap id
unsigned	bumpMap;					// normal map
float	 	angle = 0;
Torus		torus ( 1, 2, 30, 30 );
Vector3D	rot ( 0, 0, 0 );
int			mouseOldX = 0;
int			mouseOldY = 0;

const char * faces [6] =
{
	"../Textures/Cubemaps/cm_left.tga",
	"../Textures/Cubemaps/cm_right.tga",
	"../Textures/Cubemaps/cm_top.tga",
	"../Textures/Cubemaps/cm_bottom.tga",
	"../Textures/Cubemaps/cm_back.tga",
	"../Textures/Cubemaps/cm_front.tga",
};

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

	glEnable ( GL_TEXTURE_SHADER_NV );
	glEnable ( GL_REGISTER_COMBINERS_NV );

												// setup texture stages

												// stage 0:
												// bind bump (normal) map to texture unit 0
	glActiveTextureARB ( GL_TEXTURE0_ARB );
	glEnable           ( GL_TEXTURE_2D );
	glBindTexture      ( GL_TEXTURE_2D, bumpMap );
	glTexEnvi          ( GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D );

												// stage 1:
	glActiveTextureARB ( GL_TEXTURE1_ARB );
	glTexEnvi          ( GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_NV );
	glTexEnvi          ( GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB );


												// stage 2:
	glActiveTextureARB ( GL_TEXTURE2_ARB );
	glTexEnvi          ( GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_NV );
	glTexEnvi          ( GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB );

												// stage 3:
												// bind environment cube map to texture unit 3
	glActiveTextureARB ( GL_TEXTURE3_ARB );
	glEnable           ( GL_TEXTURE_CUBE_MAP_ARB );
	glBindTexture      ( GL_TEXTURE_CUBE_MAP_ARB, envMap );
	glTexEnvi          ( GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV );
	glTexEnvi          ( GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB );

												// setup register combiners
	glFinalCombinerInputNV ( GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// B.rgb = 1
	glFinalCombinerInputNV ( GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB );

												// C = 0
	glFinalCombinerInputNV ( GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// D = 0
	glFinalCombinerInputNV ( GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB );



	glMatrixMode ( GL_MODELVIEW );
	glPushMatrix ();

    glRotatef    ( rot.x, 1, 0, 0 );
    glRotatef    ( rot.y, 0, 1, 0 );
    glRotatef    ( rot.z, 0, 0, 1 );

	torus.draw ( eye );

	glPopMatrix     ();

	glutSwapBuffers ();
}

void motion ( int x, int y )
{
    rot.y -= ((mouseOldY - y) * 180.0f) / 200.0f;
    rot.z -= ((mouseOldX - x) * 180.0f) / 200.0f;
    rot.x  = 0;

    if ( rot.z > 360 )
		rot.z -= 360;

	if ( rot.z < -360 )
		rot.z += 360;

    if ( rot.y > 360 )
		rot.y -= 360;

	if ( rot.y < -360 )
        rot.y += 360;

    mouseOldX = x;
    mouseOldY = y;

	glutPostRedisplay ();
}

void mouse ( int button, int state, int x, int y )
{
    if ( state == GLUT_DOWN )
    {
        mouseOldX = x;
        mouseOldY = y;
	}
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

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize  ( 500, 500 );


								// create window
	glutCreateWindow ( "OpenGL true per-pixel reflective bumpmapping" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutKeyboardFunc ( key     );
	glutMouseFunc    ( mouse   );
    glutMotionFunc   ( motion  );

	init       ();
	printfInfo ();

	if ( !isExtensionSupported ( "GL_ARB_multitexture" ) )
	{
		printf ( "ARB_multitexture NOT supported.\n" );

		return 1;
	}

	if ( !isExtensionSupported ( "GL_NV_register_combiners" ) )
	{
		printf ( "NV_register_combiners NOT supported" );

		return 2;
	}

	if ( !isExtensionSupported ( "GL_NV_texture_shader" ) )
	{
		printf ( "GL_NV_texture_shader NOT supported" );

		return 3;
	}

	initExtensions ();

	bumpMap = createNormalMap              ( false, "../Textures/Bumpmaps/bump.tga" );
	envMap  = createCubeMap                ( true, faces );

	glutMainLoop ();

	return 0;
}
