//
// Sample to to show diffuse and specular per-pixel lighting via Register 
// Combiners in a single pass.
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//


#include	"libExt.h"

#include	<glut.h>
#include	<stdio.h>
#include	<stdlib.h>

#include	"libTexture.h"
#include	"Vector3D.h"
#include	"Vector2D.h"


Vector3D	eye   ( 2, 2, 2 );			// camera position
Vector3D	light ( 0.5, 0, 1 );		// light position
unsigned	normCubeMap;				// normalization cubemap id
unsigned	bumpMap;					// normal map
unsigned	diffuseMap;					// decal (diffuse) texture
unsigned	specularMap;				// specular map

float	 	angle = 0;

Vector3D	v  [4];
Vector2D	t0 [4];
Vector3D	t1 [4];						// l
Vector3D	t2 [4];						// h

										// map vector (x,y,z) to color (r,g,b)
Vector3D	toColor ( const Vector3D& v )
{
	return Vector3D ( (1 + v.x) * 0.5f, (1 + v.y) * 0.5f, (1 + v.z) * 0.5f );
}

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
	float	lightColor   [4] = { 1, 1, 0, 1 };				// yellow light
	float	ambientColor [4] = { 0, 0, 0.5f, 1 };			// dark blue
	float	bias         [4] = { 0.75, 0.75, 0.75, 0.75 };	// bias for specular approximation

	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

												// setup texture units
												// bind bump (normal) map to texture unit 0
	glActiveTextureARB ( GL_TEXTURE0_ARB );
	glEnable           ( GL_TEXTURE_2D );
	glBindTexture      ( GL_TEXTURE_2D, bumpMap );

												// bind diffuse map to texture unit 1
	glActiveTextureARB ( GL_TEXTURE1_ARB );
	glEnable           ( GL_TEXTURE_2D );
	glBindTexture      ( GL_TEXTURE_2D, diffuseMap );

												// bind specular map to texture unit 2
	glActiveTextureARB ( GL_TEXTURE2_ARB );
	glEnable           ( GL_TEXTURE_2D );
	glBindTexture      ( GL_TEXTURE_2D, specularMap );


												// setup register combiners
	glEnable                ( GL_REGISTER_COMBINERS_NV );
	glCombinerParameteriNV  ( GL_NUM_GENERAL_COMBINERS_NV, 7 );
	glCombinerParameterfvNV ( GL_CONSTANT_COLOR0_NV, lightColor );
	glCombinerParameterfvNV ( GL_CONSTANT_COLOR1_NV, bias );

												// configure first general combiner
												// configure A = expand (color1) (l)
	glCombinerInputNV ( GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SECONDARY_COLOR_NV,
                        GL_EXPAND_NORMAL_NV, GL_RGB );

												// configure B = expand (color1) (l)
	glCombinerInputNV ( GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SECONDARY_COLOR_NV,
                        GL_EXPAND_NORMAL_NV, GL_RGB );

												// configure C = expand (color0) (h)
	glCombinerInputNV ( GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_PRIMARY_COLOR_NV,
                        GL_EXPAND_NORMAL_NV, GL_RGB );

												// configure D = expand (color0) (h)
	glCombinerInputNV ( GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV,
                        GL_EXPAND_NORMAL_NV, GL_RGB );

												// setup output of (l,l) and (h,h)
	glCombinerOutputNV ( GL_COMBINER0_NV, GL_RGB,
							GL_SPARE0_NV,		// AB output
							GL_SPARE1_NV,		// CD output
							GL_DISCARD_NV, 		// sum output
							GL_NONE,			// no scale
							GL_NONE,			// no bias
							GL_TRUE,			// AB = A dot B
							GL_TRUE,			// CD = C dot D
							GL_FALSE );

												// now spare0.rgb contains (l,l)
												// and spare1.rgb contains (h,h)

												// configure second combiner
												// A = half-bias ( color1 ) = l / 2
	glCombinerInputNV ( GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SECONDARY_COLOR_NV,
						GL_HALF_BIAS_NORMAL_NV, GL_RGB );

												// B = unsigned-invert ( spare0.rgb ) = (1 - (l,l))
	glCombinerInputNV ( GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV,
						GL_UNSIGNED_INVERT_NV, GL_RGB );

												// C = expand ( color1 ) = l
	glCombinerInputNV ( GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SECONDARY_COLOR_NV,
	                    GL_EXPAND_NORMAL_NV, GL_RGB );

												// D = 1
	glCombinerInputNV ( GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO,
	                    GL_UNSIGNED_INVERT_NV, GL_RGB );

												// setup output of normalized l
	glCombinerOutputNV ( GL_COMBINER1_NV, GL_RGB,
							GL_DISCARD_NV,		// AB output = (l/2)*(1-(l,l))
							GL_DISCARD_NV,		// CD output = l
							GL_SPARE0_NV, 		// sum output
							GL_NONE,			// no scale
							GL_NONE,			// no bias
							GL_FALSE,			// no dot products, no mux
							GL_FALSE, GL_FALSE );

												// configure third combiner
												// A = half-bias ( color0 ) = h / 2
	glCombinerInputNV ( GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV,
						GL_HALF_BIAS_NORMAL_NV, GL_RGB );

												// B = unsigned-invert ( spare1.rgb ) = (1 - (l,l))
	glCombinerInputNV ( GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE1_NV,
						GL_UNSIGNED_INVERT_NV, GL_RGB );

												// C = expand ( color0 ) = h
	glCombinerInputNV ( GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_PRIMARY_COLOR_NV,
	                    GL_EXPAND_NORMAL_NV, GL_RGB );

												// D = 1
	glCombinerInputNV ( GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO,
	                    GL_UNSIGNED_INVERT_NV, GL_RGB );

												// setup output of normalized h
	glCombinerOutputNV ( GL_COMBINER2_NV, GL_RGB,
							GL_DISCARD_NV,		// AB output = (h/2)*(1-(h,h))
							GL_DISCARD_NV,		// CD output = l
							GL_SPARE1_NV, 		// sum output
							GL_NONE,			// no scale
							GL_NONE,			// no bias
							GL_FALSE,			// no dot products, no mux
							GL_FALSE, GL_FALSE );


												// now in spare0 we have normalized l
												// now in spare1 we have normalized h
												// output (l,n)
												// A = expand (texture0) = n
	glCombinerInputNV ( GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB,
                        GL_EXPAND_NORMAL_NV, GL_RGB );

												// configure B = spare0.rgb = l
	glCombinerInputNV ( GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV,
                        GL_SIGNED_IDENTITY_NV, GL_RGB );

												// C = expand (texture0) = n
	glCombinerInputNV ( GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB,
                        GL_EXPAND_NORMAL_NV, GL_RGB );

												// configure D = spare1.rgb = h
	glCombinerInputNV ( GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE0_NV,
                        GL_SIGNED_IDENTITY_NV, GL_RGB );

												// setup output of (l,n)
												// and (h,n)
	glCombinerOutputNV ( GL_COMBINER3_NV, GL_RGB,
							GL_SPARE0_NV,		// AB output
							GL_SPARE1_NV,		// CD output
							GL_DISCARD_NV, 		// sum output
							GL_NONE,			// no scale
							GL_NONE,			// no bias
							GL_TRUE,			// AB = A dot B
							GL_TRUE,			// CD = C dot D
							GL_FALSE );

												// now we have
												// spare0.rgb = (l,n)
												// spare1.rgb = (h,n)

												// configure combiner 4

												// A = unsigned (spare0.rgb)
	glCombinerInputNV ( GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE1_NV,
						GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// B = 1
	glCombinerInputNV ( GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO,
						GL_UNSIGNED_INVERT_NV, GL_RGB );

												// C = -constant-color1.rgb
	glCombinerInputNV ( GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV,
						GL_SIGNED_NEGATE_NV, GL_RGB );

												// D = 1
	glCombinerInputNV ( GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO,
						GL_UNSIGNED_INVERT_NV, GL_RGB );

												// configure output of max(0,(h,n)) - 0.75
	glCombinerOutputNV ( GL_COMBINER4_NV, GL_RGB,
						 GL_DISCARD_NV,			// AB output
						 GL_DISCARD_NV,			// CD output
						 GL_SPARE1_NV,
						 GL_SCALE_BY_FOUR_NV,
						 GL_NONE,				// no bias
						 GL_FALSE, GL_FALSE, GL_FALSE );

												// now spare0 = (l,n)
												//     spare1 = max (0, (h,n)) - 0.75

												// configure combiner 5

												// A = unsigned ( spare0.rgb )
	glCombinerInputNV ( GL_COMBINER5_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV,
						GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// B = tex1 (diffuse map)
	glCombinerInputNV ( GL_COMBINER5_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB,
						GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// C = unsigned ( spare1.rgb )
	glCombinerInputNV ( GL_COMBINER5_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE1_NV,
						GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// D = unsigned ( spare1.rgb )
	glCombinerInputNV ( GL_COMBINER5_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE1_NV,
						GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// configure output:
												// spare0 = AB (max(0,(l,n))*diffuseMap)
												// spare1 = CD ((max (0, max(0,(h,n)) - 0.75)^2)
	glCombinerOutputNV ( GL_COMBINER5_NV, GL_RGB,
						 GL_SPARE0_NV,			// AB output
						 GL_SPARE1_NV,			// CD output
						 GL_DISCARD_NV,			// sum output
						 GL_NONE,				// no scale
						 GL_NONE,				// no bias
						 GL_FALSE, GL_FALSE, GL_FALSE );

						 						// configure combiner 6

						 						// A = unsigned ( spare0.rgb )
	glCombinerInputNV ( GL_COMBINER6_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV,
						GL_UNSIGNED_IDENTITY_NV, GL_RGB );

						 						// B = lightColor ( constant-color0)
	glCombinerInputNV ( GL_COMBINER6_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV,
						GL_UNSIGNED_IDENTITY_NV, GL_RGB );

						 						// C = unsigned ( spare1.rgb )
	glCombinerInputNV ( GL_COMBINER6_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE1_NV,
						GL_UNSIGNED_IDENTITY_NV, GL_RGB );

						 						// D = texure2  (specularMap)
	glCombinerInputNV ( GL_COMBINER6_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE2_ARB,
						GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// configure output:
												// spare0 = AB = full-diffuse-term
												// spare1 = CD = specular * specularMap

	glCombinerOutputNV ( GL_COMBINER6_NV, GL_RGB,
						 GL_SPARE0_NV,			// AB output
						 GL_SPARE1_NV,			// CD output
						 GL_DISCARD_NV,			// sum output
						 GL_NONE,				// no scale
						 GL_NONE,				// no bias
						 GL_FALSE, GL_FALSE, 	// no dot products
						 GL_FALSE );			// no mux

												// configure final combiner:
												// output = spare0 + spare1

												// A = unsigned ( spare0 )
	glFinalCombinerInputNV ( GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// B = 1
	glFinalCombinerInputNV ( GL_VARIABLE_B_NV, GL_ZERO, /*GL_UNSIGNED_IDENTITY_NV*/GL_UNSIGNED_INVERT_NV, GL_RGB );

												// C = 0
	glFinalCombinerInputNV ( GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// D = spare1
	glFinalCombinerInputNV ( GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// E = 0
	glFinalCombinerInputNV ( GL_VARIABLE_E_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// F = 0
	glFinalCombinerInputNV ( GL_VARIABLE_F_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// G.alpha = 1
	glFinalCombinerInputNV ( GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA );

												// so at output we'll get
												// RGB   = c1 + c0 * max ( (l, n), 0 )
												// alpha = 1
	int	i;
												// compute texture coordinates for normalization cibe map
	for (  i = 0; i < 4; i++ )
	{
		Vector3D	lp = (light - v [i]).normalize ();
		Vector3D	vp = (eye   - v [i]).normalize ();

		t1 [i] = lp;
		t2 [i] = (lp + vp).normalize ();
	}
												// now draw quad
	glBegin ( GL_QUADS );

	for ( i = 0; i < 4; i++ )
	{
		glMultiTexCoord2fv  ( GL_TEXTURE0_ARB, t0 [i] );
		glSecondaryColor3fv ( toColor ( t1 [i] )      );
		glColor3fv          ( toColor ( t2 [i] )      );
	  	glVertex3fv         ( v [i]                   );
	}

	glEnd   ();

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
	if ( key == 27 || key == 'q' || key == 'Q' )
    	exit ( 0 );
}

void	animate ()
{
	angle  = 0.004f * glutGet ( GLUT_ELAPSED_TIME );

	light.x = cos ( angle );
	light.y = sin ( angle );
	light.z = 1.8;// + 0.2 * sin ( angle / 4 );

	glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize  ( 500, 500 );


								// create window
	int	win = glutCreateWindow ( "OpenGL per-pixel diffuse lighting with register combiners" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutKeyboardFunc ( key     );
	glutIdleFunc     ( animate );

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

	initExtensions ();
												// setup vertices
	v [0] = Vector3D ( -1.3, -1.3, 0 );
	v [1] = Vector3D (  1.3, -1.3, 0 );
	v [2] = Vector3D (  1.3,  1.3, 0 );
	v [3] = Vector3D ( -1.3,  1.3, 0 );

												// setup texture coordinates for bump map
	t0 [0] = Vector2D ( 0, 0 );
	t0 [1] = Vector2D ( 1, 0 );
	t0 [2] = Vector2D ( 1, 1 );
	t0 [3] = Vector2D ( 0, 1 );

	bumpMap     = createNormalMapFromHeightMap ( false, "../Textures/Bumpmaps/h.tga", 5 );
//	bumpMap     = createNormalMapFromHeightMap ( false, "../Textures/Bumpmaps/blocks.tga", 3 );

	diffuseMap  = createTexture2D            ( true, "blue.bmp" );
	specularMap = createTexture2D            ( true, "red.bmp" );
	normCubeMap = createNormalizationCubemap ( 32 );

	glutMainLoop ();

	return 0;
}
