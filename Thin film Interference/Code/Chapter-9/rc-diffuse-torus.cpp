//
// Sample to to show diffuse lighting via Register Combiners support in OpenGL
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

Vector3D	eye   ( 7, 5, 7 );			// camera position
Vector3D	light ( 5, 0, 4 );			// light position
unsigned	normCubeMap;				// normalization cubemap id
unsigned	bumpMap;					// normal map
unsigned	decalMap;					// decal (diffuse) texture
float	 	angle = 0;
Torus		torus ( 1, 3, 30, 30 );

////////////////////////////////////// methods ////////////////////////////////////////////

void display ()
{
	float	lightColor   [4] = { 1, 1, 0, 1 };		// yellow light
	float	ambientColor [4] = { 0, 0, 0.5f, 1 };	// dark blue

	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

													// draw light
	glMatrixMode ( GL_MODELVIEW );
	glPushMatrix ();

	glDisable          ( GL_REGISTER_COMBINERS_NV );
	glTranslatef       ( light.x, light.y, light.z );
	glActiveTextureARB ( GL_TEXTURE0_ARB );
	glDisable          ( GL_TEXTURE_2D );
	glActiveTextureARB ( GL_TEXTURE1_ARB );
	glDisable          ( GL_TEXTURE_CUBE_MAP_ARB );
	glActiveTextureARB ( GL_TEXTURE2_ARB );
	glDisable          ( GL_TEXTURE_2D );
	glColor4f          ( 1, 1, 1, 1 );
	glutSolidSphere    ( 0.07f, 15, 15 );
	glPopMatrix        ();
												// setup texture units
												// bind bump (normal) map to texture unit 0
	glActiveTextureARB ( GL_TEXTURE0_ARB );
	glEnable           ( GL_TEXTURE_2D );
	glBindTexture      ( GL_TEXTURE_2D, bumpMap );
	glMatrixMode       ( GL_TEXTURE );
	glLoadIdentity     ();
	glScalef           ( 1, 2, 1 );
	glMatrixMode       ( GL_MODELVIEW );

												// bind normalization cube map to texture unit 1
	glActiveTextureARB ( GL_TEXTURE1_ARB );
	glEnable           ( GL_TEXTURE_CUBE_MAP_ARB );
	glBindTexture      ( GL_TEXTURE_CUBE_MAP_ARB, normCubeMap );

												// bind decal texture to texture unit 2
	glActiveTextureARB ( GL_TEXTURE2_ARB );
	glEnable           ( GL_TEXTURE_2D );
	glBindTexture      ( GL_TEXTURE_2D, decalMap );
	glMatrixMode       ( GL_TEXTURE );
	glLoadIdentity     ();
	glScalef           ( 1, 2, 1 );
	glMatrixMode       ( GL_MODELVIEW );

												// setup register combiners
	glEnable                ( GL_REGISTER_COMBINERS_NV );
	glCombinerParameteriNV  ( GL_NUM_GENERAL_COMBINERS_NV, 1 );
	glCombinerParameterfvNV ( GL_CONSTANT_COLOR0_NV, lightColor );
	glCombinerParameterfvNV ( GL_CONSTANT_COLOR1_NV, ambientColor );

												// configure A = expand (tex0) (bumpmap)
	glCombinerInputNV ( GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB,
                        GL_EXPAND_NORMAL_NV, GL_RGB );

												// configure B = expand (tex1) (norm. map)
	glCombinerInputNV ( GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB,
                        GL_EXPAND_NORMAL_NV, GL_RGB );

												// configure C = 0
	glCombinerInputNV ( GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO,
						GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// configure D = 0
	glCombinerInputNV ( GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO,
						GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// setup output of (l,n)
	glCombinerOutputNV ( GL_COMBINER0_NV, GL_RGB,
							GL_SPARE0_NV,		// AB output
							GL_DISCARD_NV,		// CD output
							GL_DISCARD_NV, 		// sum output
							GL_NONE,			// no scale
							GL_NONE,			// no bias
							GL_TRUE,			// AB = A dot B
							GL_FALSE, GL_FALSE );

												// configure A.alpha = 1
	glCombinerInputNV ( GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO,
							GL_UNSIGNED_INVERT_NV, GL_ALPHA );

												// configure B.alpha = lz
	glCombinerInputNV ( GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB,
							GL_EXPAND_NORMAL_NV, GL_BLUE );

												// configure C.alpha = 1
	glCombinerInputNV ( GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO,
							GL_UNSIGNED_INVERT_NV, GL_ALPHA );

												// configure D.alpha = lz
	glCombinerInputNV ( GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_TEXTURE1_ARB,
							GL_EXPAND_NORMAL_NV, GL_BLUE );

	glCombinerOutputNV ( GL_COMBINER0_NV, GL_ALPHA,
							GL_DISCARD_NV,		// AB
							GL_DISCARD_NV,		// CD
							GL_SPARE0_NV,		// AB+CD
							GL_SCALE_BY_FOUR_NV,
							GL_NONE,			// bias
							GL_FALSE, GL_FALSE, GL_FALSE );

												// now spare0.rgb contains (l,n)
												//     spare0.alpha contains 8*lz

												// configure final combiner

												// A.rgb = max ( spare0.rgb, 0 )
	glFinalCombinerInputNV ( GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// B.rgb = EF
	glFinalCombinerInputNV ( GL_VARIABLE_B_NV, GL_E_TIMES_F_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// C = 0
	glFinalCombinerInputNV ( GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// D = 0
	glFinalCombinerInputNV ( GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// E = texture2
	glFinalCombinerInputNV ( GL_VARIABLE_E_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// F = selfShadow = 8*lz
	glFinalCombinerInputNV ( GL_VARIABLE_F_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA );

												// G.alpa = 1
	glFinalCombinerInputNV ( GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA );

												// so at output we'll get
												// RGB   = c1 + c0 * max ( (l, n), 0 )
												// alpha = 1




												// now draw torus
	torus.calcLightVectors ( light );
	torus.draw             ();

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
	angle  = 0.004f * glutGet ( GLUT_ELAPSED_TIME );

	light.x = 2*cos ( angle );
	light.y = 2*sin ( angle );
	light.z = 3 + 0.3 * sin ( angle / 3 );

	glutPostRedisplay ();
}

void init ()
{
	glClearColor ( 0.0, 0.0, 0.0, 1.0 );
	glEnable     ( GL_DEPTH_TEST );
	glDepthFunc  ( GL_LEQUAL );

	glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
	glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize  ( 500, 500 );


								// create window
	int	win = glutCreateWindow ( "Torus with  per-pixel diffuse lighting via register combiners" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutKeyboardFunc ( key     );
	glutIdleFunc     ( animate );

	init ();

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

	initExtensions ();

	bumpMap     = createTexture2D            ( false, "../Textures/Bumpmaps/FieldStone-DOT3.tga" );
	decalMap    = createTexture2D            ( true,  "../Textures/FieldStone.tga" );
	normCubeMap = createNormalizationCubemap   ( 32 );

	glutMainLoop ();

	return 0;
}
