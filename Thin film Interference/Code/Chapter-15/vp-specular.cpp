//
// Sample to to specular per-pixel lighting with Register Combiners in OpenGL
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
#include	"Data.h"

Vector3D	eye   ( 7, 5, 7 );			// camera position
Vector3D	light ( 5, 0, 4 );			// light position
unsigned	normCubeMap;				// normalization cubemap id
unsigned	bumpMap;					// normal map
unsigned	decalMap;					// decal (diffuse) texture
unsigned	program;					// vertex program id
float	 	angle = 0;
Torus		torus ( 1, 3, 30, 30 );

Vector3D	rot ( 0, 0, 0 );
int			mouseOldX = 0;
int			mouseOldY = 0;

/////////////////////////////////////////////////////////////////////////////////

GLuint	loadProgram ( const char * fileName )
{
	Data	data ( fileName );

	if ( !data.isOk () || data.isEmpty () )
		return 0;

	GLuint	id;

	glGenProgramsARB ( 1, &id );
	glBindProgramARB ( GL_VERTEX_PROGRAM_ARB, id );

	glProgramStringARB ( GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
                         data.getLength (), data.getPtr () );

    if ( glGetError () == GL_INVALID_OPERATION )
    {
    	GLint			 errorCode;
    	const char * errorString;

        glGetIntegerv ( GL_PROGRAM_ERROR_POSITION_ARB, &errorCode );

        errorString = (const char *)glGetString ( GL_PROGRAM_ERROR_STRING_ARB );

		printf ( "Error loading program: \n%s\n", errorString );

        return 0;
	}

	return id;
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
	float	bias         [4] = { 0.75, 0.75, 0.75, 0.75 };
	float	ambientColor [4] = { 0, 0, 0.5f, 1 };	// dark blue

	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode   ( GL_MATRIX0_ARB );
	glLoadIdentity ();
    glRotatef    ( rot.x, 1, 0, 0 );
    glRotatef    ( rot.y, 0, 1, 0 );
    glRotatef    ( rot.z, 0, 0, 1 );

	glMatrixMode   ( GL_MODELVIEW );
	glPushMatrix   ();

    glRotatef    ( rot.x, 1, 0, 0 );
    glRotatef    ( rot.y, 0, 1, 0 );
    glRotatef    ( rot.z, 0, 0, 1 );

												// setup texture units
												// bind bump (normal) map to texture unit 0
	glActiveTextureARB ( GL_TEXTURE0_ARB );
	glEnable           ( GL_TEXTURE_2D );
	glBindTexture      ( GL_TEXTURE_2D, bumpMap );

												// bind normalization cube map to texture unit 1
	glActiveTextureARB ( GL_TEXTURE1_ARB );
	glEnable           ( GL_TEXTURE_CUBE_MAP_ARB );
	glBindTexture      ( GL_TEXTURE_CUBE_MAP_ARB, normCubeMap );

												// setup register combiners
	glEnable                ( GL_REGISTER_COMBINERS_NV );
	glCombinerParameteriNV  ( GL_NUM_GENERAL_COMBINERS_NV, 2 );
	glCombinerParameterfvNV ( GL_CONSTANT_COLOR0_NV, bias );
	glCombinerParameterfvNV ( GL_CONSTANT_COLOR1_NV, ambientColor );

												// setup general combiner 0
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

												// setup output of (h,n)
	glCombinerOutputNV ( GL_COMBINER0_NV, GL_RGB,
							GL_SPARE0_NV,		// AB output
							GL_DISCARD_NV,		// CD output
							GL_DISCARD_NV, 		// sum output
							GL_NONE,			// no scale
							GL_NONE,			// no bias
							GL_TRUE,			// AB = A dot B
							GL_FALSE, GL_FALSE );

												// now spare0.rgb contains (h,n)

												// setup general combiner 1

												// A = max ( (n,h), 0 )
	glCombinerInputNV ( GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV,
							GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// B = 1
	glCombinerInputNV ( GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV,
							GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB );

												// C = -constant color0.rgb (-0.75)
	glCombinerInputNV ( GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR0_NV,
						GL_SIGNED_NEGATE_NV, GL_RGB );

												// configure D = 1
	glCombinerInputNV ( GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO,
						GL_UNSIGNED_INVERT_NV, GL_RGB );

												// setup output of (h,n) - 0.75
	glCombinerOutputNV ( GL_COMBINER1_NV, GL_RGB,
							GL_DISCARD_NV,		// AB output
							GL_DISCARD_NV,		// CD output
							GL_SPARE0_NV, 		// sum output
							GL_SCALE_BY_FOUR_NV,// scale = 4
							GL_NONE,			// no bias
							GL_FALSE,			// we do not need here dot product
							GL_FALSE,
							GL_FALSE );			// neither we need mux here

												// now spare0.rgb = 4*((n,h) - 0.75)

												// configure final combiner

												// A.rgb = max ( 0, spare0.rgb )
	glFinalCombinerInputNV ( GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// B.rgb = max ( 0, spare0.rgb )
	glFinalCombinerInputNV ( GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// C = 0
	glFinalCombinerInputNV ( GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// D = constant_color1.rgb
	glFinalCombinerInputNV ( GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// E = 0
	glFinalCombinerInputNV ( GL_VARIABLE_E_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// F = 0
	glFinalCombinerInputNV ( GL_VARIABLE_F_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

												// G.alpa = 1
	glFinalCombinerInputNV ( GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA );

												// so at output we'll get
												// RGB   = max ( 0, max ( (h, n), 0 ) - 0.75 ) ^ 2
												// alpha = 1

	glEnable         ( GL_VERTEX_PROGRAM_ARB );
	glBindProgramARB ( GL_VERTEX_PROGRAM_ARB, program );

	torus.draw  ();

	glDisable   ( GL_VERTEX_PROGRAM_ARB );
	glPopMatrix ();

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

										// setup data
	float	light4D [4];
	float	eye4D   [4];

	light4D [0] = light.x;
	light4D [1] = light.y;
	light4D [2] = light.z;
	light4D [3] = 1;

	eye4D [0] = eye.x;
	eye4D [1] = eye.y;
	eye4D [2] = eye.z;
	eye4D [3] = 1;

	glProgramLocalParameter4fvARB ( GL_VERTEX_PROGRAM_ARB, 0, eye4D   );
	glProgramLocalParameter4fvARB ( GL_VERTEX_PROGRAM_ARB, 1, light4D );

	glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize  ( 500, 500 );


								// create window
	glutCreateWindow ( "OpenGL per-pixel specular lit torus with register combiners" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutMouseFunc    ( mouse   );
	glutMotionFunc   ( motion  );
	glutKeyboardFunc ( key );
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

	if ( !isExtensionSupported ( "GL_ARB_vertex_program" ) )
	{
		printf ( "GL_ARB_vertex_program NOT supported" );

		return 3;
	}

	initExtensions ();

	bumpMap     = createNormalMapFromHeightMap ( false, "../Textures/Bumpmaps/h.tga", 3 );
	decalMap    = createTexture2D              ( true,  "../Textures/block.bmp" );
	normCubeMap = createNormalizationCubemap   ( 32 );
	program     = loadProgram                  ( "specular.vp" );

	if ( program == 0 )
		return 1;

	glutMainLoop ();

	return 0;
}
